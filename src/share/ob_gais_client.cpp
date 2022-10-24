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

#include "share/rc/ob_tenant_base.h"
#define USING_LOG_PREFIX SHARE

#include "share/ob_define.h"
#include "share/ob_gais_msg.h"
#include "share/ob_gais_rpc.h"
#include "share/ob_gais_client.h"

namespace oceanbase
{
using namespace oceanbase::common;
using namespace oceanbase::obrpc;
using namespace oceanbase::observer;
using namespace oceanbase::share;
using namespace oceanbase::transaction;

namespace share
{
int ObGAISClient::init(const ObAddr &self, ObGAISRequestRpc *gais_request_rpc,
                       ObILocationAdapter *location_adapter)
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(is_inited_)) {
    ret = OB_INIT_TWICE;
    LOG_WARN("init twice", KR(ret));
  } else if (OB_UNLIKELY(!self.is_valid()) || OB_ISNULL(gais_request_rpc) ||
             OB_ISNULL(location_adapter)) {
    LOG_WARN("invalid argument", KR(ret), K(self), KP(gais_request_rpc), KP(location_adapter));
    ret = OB_INVALID_ARGUMENT;
  } else {
    self_ = self;
    gais_request_rpc_ = gais_request_rpc;
    location_adapter_ = location_adapter;
    is_inited_ = true;
    LOG_INFO("gais client init success", K(self), KP(this));
  }
  return ret;
}

void ObGAISClient::reset()
{
  is_inited_ = false;
  self_.reset();
  gais_request_rpc_ = NULL;
  location_adapter_ = NULL;
  reset_cache_leader_();
}

int ObGAISClient::get_value(const AutoincKey &key,
                            const uint64_t offset,
                            const uint64_t increment,
                            const uint64_t max_value,
                            const uint64_t table_auto_increment,
                            const uint64_t desired_count,
                            const uint64_t cache_size,
                            uint64_t &sync_value,
                            uint64_t &start_inclusive,
                            uint64_t &end_inclusive)
{
  int ret = OB_SUCCESS;
  const uint64_t tenant_id = key.tenant_id_;
  if (OB_UNLIKELY(!is_inited_)) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    MTL_SWITCH(tenant_id) {
      ObGAISNextAutoIncValReq msg;
      ObGAISNextValRpcResult rpc_result;
      ObAddr leader;
      if (OB_FAIL(get_leader_(tenant_id, leader))) {
        LOG_WARN("get leader fail", K(ret));
        (void)refresh_location_(tenant_id);
      } else if (OB_FAIL(msg.init(key, offset, increment, table_auto_increment, max_value,
                                  desired_count, cache_size, self_))) {
        LOG_WARN("fail to init request msg", K(ret));
      } else if (OB_UNLIKELY(!msg.is_valid())) {
        ret = OB_INVALID_ARGUMENT;
        LOG_WARN("invalid argument", K(ret), K(msg));
      } else if (OB_FAIL(gais_request_rpc_->next_autoinc_val(leader, msg, rpc_result))) {
        LOG_WARN("handle gais request failed", K(ret), K(msg), K(rpc_result));
        (void)refresh_location_(tenant_id);
      } else if (!rpc_result.is_valid()) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("rpc result is unexpected", K(ret), K(rpc_result));
      } else {
        start_inclusive = rpc_result.start_inclusive_;
        end_inclusive = rpc_result.end_inclusive_;
        sync_value = rpc_result.sync_value_;
        LOG_DEBUG("handle gais success", K(rpc_result));
      }
    }
  }
  return ret;
}

int ObGAISClient::get_sequence_value(const AutoincKey &key, uint64_t &sequence_value)
{
  int ret = OB_SUCCESS;
  const uint64_t tenant_id = key.tenant_id_;
  if (OB_UNLIKELY(!is_inited_)) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    MTL_SWITCH(tenant_id) {
      ObGAISAutoIncKeyArg msg;
      ObGAISCurrValRpcResult rpc_result;
      ObAddr leader;
      if (OB_FAIL(get_leader_(tenant_id, leader))) {
        LOG_WARN("get leader fail", K(ret));
        (void)refresh_location_(tenant_id);
      } else if (OB_FAIL(msg.init(key, self_))) {
        LOG_WARN("fail to init request msg", K(ret));
      } else if (OB_UNLIKELY(!msg.is_valid())) {
        ret = OB_INVALID_ARGUMENT;
        LOG_WARN("invalid argument", K(ret), K(msg));
      } else if (OB_FAIL(gais_request_rpc_->curr_autoinc_val(leader, msg, rpc_result))) {
        LOG_WARN("handle gais request failed", K(ret), K(msg), K(rpc_result));
        (void)refresh_location_(tenant_id);
      } else {
        sequence_value = rpc_result.sequence_value_;
        LOG_DEBUG("handle gais success", K(rpc_result));
      }
    }
  }

  return ret;
}

int ObGAISClient::get_auto_increment_values(
    const common::ObIArray<AutoincKey> &autoinc_keys,
    common::hash::ObHashMap<AutoincKey, uint64_t> &seq_values)
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(autoinc_keys.empty())) {
    // do nothing, return empty seq_values
  } else {
    // all keys share the same tenant id
    const uint64_t tenant_id = autoinc_keys.at(0).tenant_id_;
    if (OB_UNLIKELY(!is_inited_)) {
      ret = OB_NOT_INIT;
      LOG_WARN("not inited", K(ret));
    } else {
      MTL_SWITCH(tenant_id) {
        ObGAISAutoIncKeyArg msg;
        ObGAISCurrValRpcResult rpc_result;
        ObAddr leader;
        if (OB_FAIL(get_leader_(tenant_id, leader))) {
          LOG_WARN("get leader fail", K(ret));
          (void)refresh_location_(tenant_id);
        } else {
          for (int64_t i = 0; OB_SUCC(ret) && i < autoinc_keys.count(); ++i) {
            rpc_result.reset();
            AutoincKey key = autoinc_keys.at(i);
            if (OB_FAIL(msg.init(key, self_))) {
              LOG_WARN("fail to init request msg", K(ret));
            } else if (OB_UNLIKELY(!msg.is_valid())) {
              ret = OB_INVALID_ARGUMENT;
              LOG_WARN("invalid argument", K(ret), K(msg));
            } else if (OB_FAIL(gais_request_rpc_->curr_autoinc_val(leader, msg, rpc_result))) {
              LOG_WARN("handle gais request failed", K(ret), K(msg), K(rpc_result));
              (void)refresh_location_(tenant_id);
            } else if (OB_FAIL(seq_values.set_refactored(key, rpc_result.sequence_value_))) {
              LOG_WARN("fail to get int_value.", K(ret));
            } else {
              LOG_DEBUG("handle gais success", K(rpc_result));
            }
          }
        }
      }
    }
  }

  return ret;
}

int ObGAISClient::local_push_to_global_value(const AutoincKey &key,
                                             const uint64_t max_value,
                                             const uint64_t local_sync_value,
                                             uint64_t &global_sync_value)
{
  int ret = OB_SUCCESS;
  const uint64_t tenant_id = key.tenant_id_;
  if (OB_UNLIKELY(!is_inited_)) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    MTL_SWITCH(tenant_id) {
      ObGAISPushAutoIncValReq msg;
      uint64_t new_sync_value = 0;
      ObAddr leader;
      if (OB_FAIL(get_leader_(tenant_id, leader))) {
        LOG_WARN("get leader fail", K(ret));
        (void)refresh_location_(tenant_id);
      } else if (OB_FAIL(msg.init(key, local_sync_value, max_value, self_))) {
        LOG_WARN("fail to init request msg", K(ret));
      } else if (OB_UNLIKELY(!msg.is_valid())) {
        ret = OB_INVALID_ARGUMENT;
        LOG_WARN("invalid argument", K(ret), K(msg));
      } else if (OB_FAIL(gais_request_rpc_->push_autoinc_val(leader, msg, new_sync_value))) {
        LOG_WARN("handle gais request failed", K(ret), K(msg));
        (void)refresh_location_(tenant_id);
      } else {
        global_sync_value = new_sync_value;
        LOG_DEBUG("handle gais success", K(global_sync_value));
      }
    }
  }
  return ret;
}

int ObGAISClient::local_sync_with_global_value(const AutoincKey &key, uint64_t &global_sync_value)
{
  int ret = OB_SUCCESS;
  const uint64_t tenant_id = key.tenant_id_;
  if (OB_UNLIKELY(!is_inited_)) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    MTL_SWITCH(tenant_id) {
      ObGAISAutoIncKeyArg msg;
      ObGAISCurrValRpcResult rpc_result;
      ObAddr leader;
      if (OB_FAIL(get_leader_(tenant_id, leader))) {
        LOG_WARN("get leader fail", K(ret));
        (void)refresh_location_(key.tenant_id_);
      } else if (OB_FAIL(msg.init(key, self_))) {
        LOG_WARN("fail to init request msg", K(ret));
      } else if (OB_UNLIKELY(!msg.is_valid())) {
        ret = OB_INVALID_ARGUMENT;
        LOG_WARN("invalid argument", K(ret), K(msg));
      } else if (OB_FAIL(gais_request_rpc_->curr_autoinc_val(leader, msg, rpc_result))) {
        LOG_WARN("handle gais request failed", K(ret), K(msg));
        (void)refresh_location_(key.tenant_id_);
      } else if (!rpc_result.is_valid()) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("rpc result is unexpected", K(ret), K(rpc_result));
      } else {
        global_sync_value = rpc_result.sync_value_;
        LOG_DEBUG("handle gais success", K(global_sync_value));
      }
    }
  }
  return ret;
}

int ObGAISClient::clear_global_autoinc_cache(const AutoincKey &key)
{
  int ret = OB_SUCCESS;
  const uint64_t tenant_id = key.tenant_id_;
  if (OB_UNLIKELY(!is_inited_)) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    MTL_SWITCH(tenant_id) {
      ObGAISAutoIncKeyArg msg;
      ObAddr leader;
      if (OB_FAIL(get_leader_(tenant_id, leader))) {
        LOG_WARN("get leader fail", K(ret));
        (void)refresh_location_(key.tenant_id_);
      } else if (OB_FAIL(msg.init(key, self_))) {
        LOG_WARN("fail to init request msg", K(ret));
      } else if (OB_UNLIKELY(!msg.is_valid())) {
        ret = OB_INVALID_ARGUMENT;
        LOG_WARN("invalid argument", K(ret), K(msg));
      } else if (OB_FAIL(gais_request_rpc_->clear_autoinc_cache(leader, msg))) {
        LOG_WARN("handle gais request failed", K(ret), K(msg));
        (void)refresh_location_(key.tenant_id_);
      } else {
        LOG_DEBUG("clear global autoinc cache success", K(msg));
      }
    }
  }
  return ret;
}

int ObGAISClient::get_leader_(const uint64_t tenant_id, ObAddr &leader)
{
  int ret = OB_SUCCESS;
  const int64_t cluster_id = GCONF.cluster_id;
  lib::ObMutexGuard guard(cache_leader_mutex_);
  if (OB_LIKELY(gais_cache_leader_.is_valid())) {
    leader = gais_cache_leader_;
  } else if (OB_FAIL(location_adapter_->nonblock_get_leader(
                                        cluster_id, tenant_id, GAIS_LS, leader))) {
    LOG_WARN("gais nonblock get leader failed", K(ret), K(tenant_id), K(GAIS_LS));
  } else if (OB_UNLIKELY(!leader.is_valid())) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("get invaild lear from location adapter", K(ret), K(leader));
  } else {
    gais_cache_leader_ = leader;
  }
  return ret;
}

int ObGAISClient::refresh_location_(const uint64_t tenant_id)
{
  int ret = OB_SUCCESS;
  const int64_t cluster_id = GCONF.cluster_id;
  reset_cache_leader_();
  if (OB_FAIL(location_adapter_->nonblock_renew(cluster_id, tenant_id, GAIS_LS))) {
    LOG_WARN("gais nonblock renew error", KR(ret), K(tenant_id), K(GAIS_LS));
  }
  return ret;
}

} // share
} // oceanbase

