/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#include "ob_fifo_arena.h"
#include "math.h"
#include "ob_memstore_allocator_mgr.h"
#include "share/ob_tenant_mgr.h"
#include "observer/omt/ob_tenant_config_mgr.h"
#include "lib/alloc/alloc_struct.h"
#include "lib/stat/ob_diagnose_info.h"

using namespace oceanbase::lib;
using namespace oceanbase::omt;
namespace oceanbase
{
namespace common
{
#define myassert(x) if (!x) { ob_abort(); }
int64_t ObFifoArena::total_hold_ = 0;

int64_t ObFifoArena::Page::get_actual_hold_size()
{
  //every time of alloc_page, ruturn a chunk actually
  return ObTenantCtxAllocator::get_obj_hold(this);
}

void ObFifoArena::ObWriteThrottleInfo::reset()
{
  decay_factor_ = 0.0;
  alloc_duration_ = 0;
  trigger_percentage_ = 0;
  memstore_threshold_ = 0;
  ATOMIC_SET(&period_throttled_count_, 0);
  ATOMIC_SET(&period_throttled_time_, 0);
  ATOMIC_SET(&total_throttled_count_, 0);
  ATOMIC_SET(&total_throttled_time_, 0);
}

void ObFifoArena::ObWriteThrottleInfo::reset_period_stat_info()
{
  ATOMIC_SET(&period_throttled_count_, 0);
  ATOMIC_SET(&period_throttled_time_, 0);
}

void ObFifoArena::ObWriteThrottleInfo::record_limit_event(int64_t interval)
{
  ATOMIC_INC(&period_throttled_count_);
  ATOMIC_FAA(&period_throttled_time_, interval);
  ATOMIC_INC(&total_throttled_count_);
  ATOMIC_FAA(&total_throttled_time_, interval);
}

int ObFifoArena::ObWriteThrottleInfo::check_and_calc_decay_factor(int64_t memstore_threshold,
                                                                  int64_t trigger_percentage,
                                                                  int64_t alloc_duration)
{
  int ret = OB_SUCCESS;
  if (memstore_threshold != memstore_threshold_
      || trigger_percentage != trigger_percentage_
      ||  alloc_duration != alloc_duration_) {
    memstore_threshold_ = memstore_threshold;
    trigger_percentage_ = trigger_percentage;
    alloc_duration_ = alloc_duration;
    int64_t available_mem = (100 - trigger_percentage_) * memstore_threshold_ / 100;
    double N =  static_cast<double>(available_mem) / static_cast<double>(MEM_SLICE_SIZE);
    decay_factor_ = (static_cast<double>(alloc_duration) - N * static_cast<double>(MIN_INTERVAL))/ static_cast<double>((((N*(N+1)*N*(N+1)))/4));
    decay_factor_ = decay_factor_ < 0 ? 0 : decay_factor_;
    COMMON_LOG(INFO, "recalculate decay factor", K(memstore_threshold_), K(trigger_percentage_),
               K(decay_factor_), K(alloc_duration), K(available_mem), K(N));
  }
  return ret;
}

int ObFifoArena::init(uint64_t tenant_id)
{
  int ret = OB_SUCCESS;
  lib::ObMallocAllocator *allocator = lib::ObMallocAllocator::get_instance();
  uint64_t ctx_id = ObCtxIds::MEMSTORE_CTX_ID;

  if (OB_ISNULL(allocator)) {
    ret = OB_INIT_FAIL;
    OB_LOG(ERROR, "mallocator instance is NULLL", K(ret));
  } else if (OB_ISNULL(allocator_ = allocator->get_tenant_ctx_allocator(tenant_id, ctx_id))) {
    if (OB_FAIL(allocator->create_tenant_ctx_allocator(tenant_id, ctx_id))) {
      OB_LOG(ERROR, "fail to create tenant allocator", K(tenant_id), K(ctx_id), K(ret));
    } else if (OB_ISNULL(allocator_ = allocator->get_tenant_ctx_allocator(tenant_id, ctx_id))) {
      ret = OB_ERR_UNEXPECTED;
      OB_LOG(ERROR, "tenant allocator is null", K(tenant_id), K(ctx_id), K(ret));
    }
  }

  if (OB_SUCC(ret)) {
    attr_.tenant_id_ = tenant_id;
    attr_.label_ = ObNewModIds::OB_MEMSTORE;
    attr_.ctx_id_ = ctx_id;
  }
  return ret;
}

void ObFifoArena::reset()
{
  COMMON_LOG(INFO, "MTALLOC.reset", "tenant_id", get_tenant_id());
  shrink_cached_page(0);
}

void ObFifoArena::update_nway_per_group(int64_t nway)
{
  if (nway <= 0) {
    nway = 1;
  } else if (nway > Handle::MAX_NWAY) {
    nway = Handle::MAX_NWAY;
  }
  if (nway > nway_) {
    ATOMIC_STORE(&nway_, nway);
  } else if (nway < nway_) {
    ATOMIC_STORE(&nway_, nway);
    WaitQuiescent(get_qs());
    shrink_cached_page(nway);
  }
}

void ObFifoArena::shrink_cached_page(int64_t nway)
{
  for(int64_t i = 0; i < MAX_CACHED_PAGE_COUNT; i++) {
    if ((i % Handle::MAX_NWAY) >= nway) {
      Page** paddr = cur_pages_ + i;
      Page* page = NULL;
      CriticalGuard(get_qs());
      if (NULL != (page = ATOMIC_LOAD(paddr))) {
        Ref* ref = page->frozen();
        if (NULL != ref) {
          // There may be concurrent removal, no need to pay attention to the return value
          UNUSED(ATOMIC_BCAS(paddr, page, NULL));
          IGNORE_RETURN ATOMIC_FAA(&retired_, page->hold());
          release_ref(ref);
        }
      }
    }
  }
}

void* ObFifoArena::alloc(int64_t adv_idx, Handle& handle, int64_t size)
{
  int ret = OB_SUCCESS;
  void* ptr = NULL;
  int64_t rsize = size + sizeof(Page) + sizeof(Ref);
  speed_limit(ATOMIC_LOAD(&hold_), size);
  CriticalGuard(get_qs());
  int64_t way_id = get_way_id();
  int64_t idx = get_idx(adv_idx, way_id);
  Page** paddr = cur_pages_ + idx;
  if (adv_idx < 0 || size < 0) {
    COMMON_LOG(INFO, "invalid argument", K(adv_idx), K(size));
    ret = OB_INVALID_ARGUMENT;
  } else if (rsize > PAGE_SIZE) {
    Page* page = NULL;
    if (NULL == (page = alloc_page(rsize))) {
      ret = OB_ALLOCATE_MEMORY_FAILED;
    } else {
      bool need_switch = false;
      handle.add_allocated(page->hold());
      ptr = handle.ref_and_alloc(way_id, need_switch, page, size);
      page->frozen();
      retire_page(way_id, handle, page);
    }
  } else {
    while (OB_SUCC(ret) && NULL == ptr) {
      Page* page = NULL;
      bool need_switch = false;
      if (NULL != (page = ATOMIC_LOAD(paddr))) {
        Ref* ref = handle.get_match_ref(way_id, page);
        if (NULL != ref) {
          ptr = handle.alloc(need_switch, ref, page, size);
        } else {
          LockGuard guard(handle.lock_);
          if (NULL == (ref = handle.get_match_ref(way_id, page))) {
            ptr = handle.ref_and_alloc(way_id, need_switch, page, size);
          }
        }
      }
      if (NULL == page || need_switch) {
        Page* new_page = NULL;
        int64_t alloc_size = PAGE_SIZE;
        if (NULL != page) {
          retire_page(way_id, handle, page);
        }
        if (NULL == (new_page = alloc_page(alloc_size))) {
          // There may be concurrent removal, no need to pay attention to the return value
          UNUSED(ATOMIC_BCAS(paddr, page, NULL));
          ret = OB_ALLOCATE_MEMORY_FAILED;
        } else if (ATOMIC_BCAS(paddr, page, new_page)) {
          handle.add_allocated(new_page->hold());
        } else {
          destroy_page(new_page);
        }
      }
    }
  }
  return ptr;
}

void ObFifoArena::release_ref(Ref* ref)
{
  if (0 == ref->page_->xref(ref->allocated_)) {
    free_page(ref->page_);
  }
}

void ObFifoArena::free(Handle& handle)
{
  bool wait_qs_done = false;
  for(int i = 0; i < Handle::MAX_NWAY; i++) {
    Ref* ref = NULL;
    Ref* next_ref = handle.ref_[i];
    if (NULL != next_ref && !wait_qs_done) {
      WaitQuiescent(get_qs());
      wait_qs_done = true;
    }
    while(NULL != (ref = next_ref)) {
      next_ref = ref->next_;
      release_ref(ref);
    }
  }
  handle.reset();
}

ObFifoArena::Page* ObFifoArena::alloc_page(int64_t size)
{
  Page* page = (Page*)allocator_->alloc(size, attr_);
  if (NULL != page) {
    ATOMIC_FAA(&allocated_, size);
    ATOMIC_FAA(&total_hold_, size);
    ATOMIC_AAF(&hold_, page->get_actual_hold_size());
    page->set(size);
  }
  return page;
}

void ObFifoArena::free_page(Page* page)
{
  if (NULL != page && NULL != allocator_) {
    ATOMIC_FAA(&reclaimed_, page->hold());
    ATOMIC_FAA(&total_hold_, -page->hold());
    ATOMIC_FAA(&hold_, -page->get_actual_hold_size());
    allocator_->free(page);
  }
}

void ObFifoArena::retire_page(int64_t idx, Handle& handle, Page* page)
{
  if (NULL != page) {
    ATOMIC_FAA(&retired_, page->hold());
    handle.add_ref(idx, &page->self_ref_);
  }
}

void ObFifoArena::destroy_page(Page* page)
{
  if (NULL != page && NULL != allocator_) {
    ATOMIC_FAA(&allocated_, -page->hold());
    ATOMIC_FAA(&total_hold_, -page->hold());
    allocator_->free(page);
  }
}

bool ObFifoArena::need_do_writing_throttle() const
{
  bool need_do_writing_throttle = false;
  int64_t trigger_percentage = get_writing_throttling_trigger_percentage_();
  if (trigger_percentage < 100) {
    int64_t trigger_mem_limit = lastest_memstore_threshold_ * trigger_percentage / 100;
    int64_t cur_mem_hold = ATOMIC_LOAD(&hold_);
    need_do_writing_throttle = cur_mem_hold > trigger_mem_limit;
  }

  return need_do_writing_throttle;
}

void ObFifoArena::speed_limit(const int64_t cur_mem_hold, const int64_t alloc_size)
{
  int ret = OB_SUCCESS;
  int64_t trigger_percentage = get_writing_throttling_trigger_percentage_();
  int64_t trigger_mem_limit = 0;
  bool need_speed_limit = false;
  int64_t seq = 0;
  int64_t throttling_interval = 0;
  if (trigger_percentage < 100) {
    if (OB_UNLIKELY(cur_mem_hold < 0 || alloc_size <= 0 || lastest_memstore_threshold_ <= 0 || trigger_percentage <= 0)) {
      COMMON_LOG(ERROR, "invalid arguments", K(cur_mem_hold), K(alloc_size), K(lastest_memstore_threshold_), K(trigger_percentage));
    } else if (cur_mem_hold > (trigger_mem_limit = lastest_memstore_threshold_ * trigger_percentage / 100)) {
      need_speed_limit = true;
      int64_t alloc_duration = get_writing_throttling_maximum_duration_();
      if (OB_FAIL(throttle_info_.check_and_calc_decay_factor(lastest_memstore_threshold_, trigger_percentage, alloc_duration))) {
        COMMON_LOG(WARN, "failed to check_and_calc_decay_factor", K(cur_mem_hold), K(alloc_size), K(throttle_info_));
      }
    }
    advance_clock();
    seq = ATOMIC_AAF(&max_seq_, alloc_size);
    get_seq() = seq;
    tl_need_speed_limit() = need_speed_limit;

    if (need_speed_limit && REACH_TIME_INTERVAL(1 * 1000 * 1000L)) {
      COMMON_LOG(INFO, "report write throttle info", K(alloc_size), K(attr_), K(throttling_interval),
                  "max_seq_", ATOMIC_LOAD(&max_seq_), K(clock_),
                  K(cur_mem_hold), K(throttle_info_), K(seq));
    }
  }
}

bool ObFifoArena::check_clock_over_seq(const int64_t req)
{
  advance_clock();
  int64_t clock = ATOMIC_LOAD(&clock_);
  return req <= clock;
}

void ObFifoArena::advance_clock()
{
  int64_t cur_ts = ObTimeUtility::current_time();
  int64_t old_ts = last_update_ts_;
  if ((cur_ts - last_update_ts_ > ADVANCE_CLOCK_INTERVAL) &&
       old_ts == ATOMIC_CAS(&last_update_ts_, old_ts, cur_ts)) {
    int64_t trigger_percentage = get_writing_throttling_trigger_percentage_();
    int64_t trigger_mem_limit = lastest_memstore_threshold_ * trigger_percentage / 100;
    int64_t cur_mem_hold = ATOMIC_LOAD(&hold_);
    bool need_speed_limit = false;
    if (trigger_percentage < 100) {
      need_speed_limit = cur_mem_hold > trigger_mem_limit;
    }
    int64_t mem_limit =
      (need_speed_limit ? calc_mem_limit(cur_mem_hold, trigger_mem_limit, ADVANCE_CLOCK_INTERVAL) :
                          trigger_mem_limit - cur_mem_hold);
    int64_t clock = ATOMIC_LOAD(&clock_);
    int64_t max_seq = ATOMIC_LOAD(&max_seq_);
    ATOMIC_SET(&clock_, min(max_seq, clock + mem_limit));
    if (REACH_TIME_INTERVAL(100 * 1000L)) {
      COMMON_LOG(INFO, "current clock is ",
                K(clock_), K(max_seq_), K(mem_limit), K(cur_mem_hold), K(attr_.tenant_id_));
    }
  }
}

int64_t ObFifoArena::expected_wait_time(const int64_t seq) const
{
  int64_t expected_wait_time = 0;
  int64_t trigger_percentage = get_writing_throttling_trigger_percentage_();
  int64_t trigger_mem_limit = lastest_memstore_threshold_ * trigger_percentage / 100;
  int64_t can_assign_in_next_period = calc_mem_limit(hold_, trigger_mem_limit, ADVANCE_CLOCK_INTERVAL);
  int64_t clock = ATOMIC_LOAD(&clock_);
  if (seq > clock) {
    expected_wait_time = (seq - clock) * ADVANCE_CLOCK_INTERVAL / can_assign_in_next_period;
  }
  return expected_wait_time;
}

int64_t ObFifoArena::calc_mem_limit(const int64_t cur_mem_hold, const int64_t trigger_mem_limit, const int64_t dt) const
{
  double cur_chunk_seq = static_cast<double>(((cur_mem_hold - trigger_mem_limit) + MEM_SLICE_SIZE - 1)/ (MEM_SLICE_SIZE));
  int64_t mem_can_be_assigned = 0;

  int64_t allocate_size_in_the_page = MEM_SLICE_SIZE - (cur_mem_hold - trigger_mem_limit) % MEM_SLICE_SIZE;
  int64_t accumulate_interval = 0;
  int64_t the_page_interval = 0;
  while (accumulate_interval < dt) {
    the_page_interval = static_cast<int64_t>(throttle_info_.decay_factor_ * cur_chunk_seq * cur_chunk_seq * cur_chunk_seq) * allocate_size_in_the_page / MEM_SLICE_SIZE;
    accumulate_interval += the_page_interval;

    mem_can_be_assigned += (accumulate_interval > dt ?
                            allocate_size_in_the_page - (accumulate_interval - dt) * allocate_size_in_the_page / the_page_interval :
                            allocate_size_in_the_page);
    allocate_size_in_the_page = MEM_SLICE_SIZE;
    cur_chunk_seq += double(1);
  }

  return mem_can_be_assigned;
}

int64_t ObFifoArena::get_throttling_interval(const int64_t cur_mem_hold,
                                             const int64_t alloc_size,
                                             const int64_t trigger_mem_limit)
{
  constexpr int64_t MIN_INTERVAL_PER_ALLOC = 20;
  int64_t chunk_cnt = ((alloc_size + MEM_SLICE_SIZE - 1) / (MEM_SLICE_SIZE));
  int64_t chunk_seq = ((cur_mem_hold - trigger_mem_limit) + MEM_SLICE_SIZE - 1)/ (MEM_SLICE_SIZE);
  int64_t ret_interval = 0;
  double cur_chunk_seq  = 1.0;
  for (int64_t i = 0; i < chunk_cnt && cur_chunk_seq > 0.0; ++i) {
    cur_chunk_seq = static_cast<double>(chunk_seq - i);
    ret_interval += static_cast<int64_t>(throttle_info_.decay_factor_ * cur_chunk_seq * cur_chunk_seq * cur_chunk_seq);
  }
  return alloc_size * ret_interval / MEM_SLICE_SIZE + MIN_INTERVAL_PER_ALLOC;
}

void ObFifoArena::set_memstore_threshold(int64_t memstore_threshold)
{
  ATOMIC_STORE(&lastest_memstore_threshold_, memstore_threshold);
}

template<int64_t N>
struct INTEGER_WRAPPER
{
  INTEGER_WRAPPER() : v_(N) {}
  int64_t v_;
};

int64_t ObFifoArena::get_writing_throttling_trigger_percentage_() const
{
  RLOCAL(INTEGER_WRAPPER<DEFAULT_TRIGGER_PERCENTAGE>, wrapper);
  int64_t &trigger_percentage = (&wrapper)->v_;
  if (TC_REACH_TIME_INTERVAL(5 * 1000 * 1000)) { // 5s
    omt::ObTenantConfigGuard tenant_config(TENANT_CONF(attr_.tenant_id_));
    if (!tenant_config.is_valid()) {
      COMMON_LOG(INFO, "failed to get tenant config", K(attr_));
    } else {
      trigger_percentage = tenant_config->writing_throttling_trigger_percentage;
    }
  }
  return trigger_percentage;
}

int64_t ObFifoArena::get_writing_throttling_maximum_duration_() const
{
  RLOCAL(INTEGER_WRAPPER<DEFAULT_DURATION>, wrapper);
  int64_t &duration = (&wrapper)->v_;
  if (TC_REACH_TIME_INTERVAL(1 * 1000 * 1000)) { // 1s
    omt::ObTenantConfigGuard tenant_config(TENANT_CONF(attr_.tenant_id_));
    if (!tenant_config.is_valid()) {
      //keep default
      COMMON_LOG(INFO, "failed to get tenant config", K(attr_));
    } else {
      duration = tenant_config->writing_throttling_maximum_duration;
    }
  }
  return duration;
}

}; // end namespace allocator
}; // end namespace oceanbase
