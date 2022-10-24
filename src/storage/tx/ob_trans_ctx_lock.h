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

#ifndef OCEANBASE_TRANSACTION_OB_TRANS_CTX_LOCK_
#define OCEANBASE_TRANSACTION_OB_TRANS_CTX_LOCK_

#include "lib/list/ob_dlist.h"
#include "lib/lock/ob_latch.h"
#include "lib/list/ob_dlink_node.h"
#include "lib/objectpool/ob_concurrency_objpool.h"
#include "ob_trans_end_trans_callback.h"

namespace oceanbase
{
namespace memtable
{
class ObIMemtableCtx;
}
namespace transaction
{
class ObTransService;
class ObTransCtx;

class LocalTask : public common::ObDLinkBase<LocalTask>
{
public:
  LocalTask(int64_t msg_type) {}
  ~LocalTask() {}
};

typedef common::ObDList<LocalTask> LocalTaskList;

class CtxLockArg
{
public:
  CtxLockArg() : trans_id_(), task_list_(), commit_cb_(),
      has_pending_callback_(false),
      need_del_ctx_(false),
      p_mt_ctx_(NULL) {}
public:
  ObTransID trans_id_;
  LocalTaskList task_list_;
  ObTxCommitCallback commit_cb_;
  bool has_pending_callback_;
  bool need_del_ctx_;
  // It is used to wake up lock queue after submitting the log for elr transaction
  memtable::ObIMemtableCtx *p_mt_ctx_;
};

class LocalTaskFactory
{
public:
  static LocalTask *alloc(int64_t msg_type)
  { return op_reclaim_alloc_args(LocalTask, msg_type); }
  static void release(const LocalTask *task)
  {
    if (NULL == task) {
      TRANS_LOG(ERROR, "task is null", KP(task));
    } else {
      op_reclaim_free(const_cast<LocalTask *>(task));
      task = NULL;
    }
  }
};

class CtxLock
{
public:
  CtxLock() : lock_(), ctx_(NULL), lock_start_ts_(0) {}
  ~CtxLock() {}
  int init(ObTransCtx *ctx);
  void reset();
  int lock();
  int lock(const int64_t timeout_us);
  int try_lock();
  void unlock();
  void before_unlock(CtxLockArg &arg);
  void after_unlock(CtxLockArg &arg);
  ObTransCtx *get_ctx() { return ctx_; }
  bool is_locked_by_self() const { return lock_.is_wrlocked_by(); }
private:
  static const int64_t WARN_LOCK_TS = 1 * 1000 * 1000;
  DISALLOW_COPY_AND_ASSIGN(CtxLock);
private:
  common::ObLatch lock_;
  ObTransCtx *ctx_;
  int64_t lock_start_ts_;
};

class CtxLockGuard
{
public:
  CtxLockGuard() : lock_(NULL) {}
  explicit CtxLockGuard(CtxLock &lock, const bool need_lock = true) : lock_(&lock) { if (need_lock) lock_->lock(); }
  ~CtxLockGuard();
  void set(CtxLock &lock);
  void reset();
private:
  DISALLOW_COPY_AND_ASSIGN(CtxLockGuard);
private:
  CtxLock *lock_;
};

class TransTableSeqLock
{
public:
  TransTableSeqLock() : seq_(0) {}
  ~TransTableSeqLock() {}

  void write_lock();
  void write_unlock();
  uint64_t read_begin();
  bool read_retry(uint64_t seq);

  DISABLE_COPY_ASSIGN(TransTableSeqLock);
private:
  uint64_t seq_;
};

class CtxTransTableLockGuard : public CtxLockGuard
{
public:
  CtxTransTableLockGuard() : CtxLockGuard(), lock_(NULL) {}
  explicit CtxTransTableLockGuard(CtxLock &lock,
                                  TransTableSeqLock &seqlock,
                                  const bool need_lock = true)
      : CtxLockGuard(lock, need_lock), lock_(&seqlock)
  {
    lock_->write_lock();
  }

  ~CtxTransTableLockGuard();

  void set(CtxLock &lock, TransTableSeqLock &seqlock);
  void reset();

  DISABLE_COPY_ASSIGN(CtxTransTableLockGuard);
private:
  TransTableSeqLock *lock_;
};


} // transaction
} // oceanbase

#endif // OCEANBASE_TRANSACTION_OB_TRANS_CTX_LOCK_
