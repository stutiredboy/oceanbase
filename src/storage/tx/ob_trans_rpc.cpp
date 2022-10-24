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

#include "ob_trans_rpc.h"
#include "share/ob_errno.h"
#include "share/ob_cluster_version.h"
#include "lib/oblog/ob_log.h"
#include "rpc/obrpc/ob_rpc_net_handler.h"
#include "ob_trans_service.h"
#include "share/rc/ob_tenant_base.h"

namespace oceanbase
{

using namespace common;
using namespace transaction;
using namespace obrpc;
using namespace storage;
using namespace share;

namespace obrpc
{
OB_SERIALIZE_MEMBER(ObTransRpcResult, status_, send_timestamp_, private_data_);
OB_SERIALIZE_MEMBER(ObTxRpcRollbackSPResult, status_, send_timestamp_, addr_, born_epoch_);

int refresh_location_cache(const share::ObLSID ls)
{
  return MTL(ObTransService *)->refresh_location_cache(ls);
}

int handle_trans_msg_callback(const share::ObLSID &sender_ls_id,
                              const share::ObLSID &receiver_ls_id,
                              const transaction::ObTransID &tx_id,
                              const int16_t msg_type,
                              const int status,
                              const ObAddr &addr,
                              const int64_t request_id,
                              const int64_t private_data)
{
  return MTL(ObTransService *)->handle_trans_msg_callback(sender_ls_id,
                                                  receiver_ls_id,
                                                  tx_id,
                                                  msg_type,
                                                  status,
                                                  addr,
                                                  request_id,
                                                  private_data);
}


int handle_sp_rollback_resp(const share::ObLSID &receiver_ls_id,
                            const int64_t epoch,
                            const transaction::ObTransID &tx_id,
                            const int status,
                            const ObAddr &addr,
                            const int64_t request_id,
                            const ObTxRpcRollbackSPResult &result)
{
  return MTL(ObTransService *)->handle_sp_rollback_resp(receiver_ls_id,
                  epoch, tx_id, status, addr, request_id, result);
}

void ObTransRpcResult::reset()
{
  status_ = OB_SUCCESS;
  send_timestamp_ = 0L;
  private_data_ = 0;
}

void ObTransRpcResult::init(const int status, const int64_t timestamp)
{
  status_ = status;
  send_timestamp_ = timestamp;
}

#define TX_Process(name, handle_func)                                   \
int ObTx##name##P::process()                                            \
{                                                                       \
  int ret = OB_SUCCESS;                                                 \
  static const int64_t STATISTICS_INTERVAL_US = 10000000;               \
  RLOCAL(int64_t, total_rt);                                            \
  RLOCAL(int64_t, total_process);                                       \
  const int64_t run_ts = get_run_timestamp();                           \
  transaction::ObTransService *txs  = nullptr;                          \
  uint64_t tenant_id = rpc_pkt_->get_tenant_id();                       \
  if (tenant_id != MTL_ID()) {                                          \
    ret = OB_ERR_UNEXPECTED;                                            \
  }                                                                     \
  if (OB_FAIL(ret)) {                                                   \
    TRANS_LOG(WARN, "tenant is not match", K(ret), K(tenant_id));       \
  } else if (OB_ISNULL(txs = MTL(transaction::ObTransService*))) {      \
    ret = OB_ERR_UNEXPECTED;                                            \
    TRANS_LOG(WARN, "get tx service fail", K(ret), K(tenant_id));       \
  } else {                                                              \
    if (!arg_.is_valid()) {                                             \
      ret = OB_INVALID_ARGUMENT;                                        \
      TRANS_LOG(ERROR, "msg is invalid", K(ret), K_(arg));              \
    } else {                                                            \
      ret = (*txs).handle_func(arg_, result_);                          \
    }                                                                   \
    const int64_t cur_ts = ObTimeUtility::current_time();               \
    total_rt = total_rt + (cur_ts - run_ts);                            \
    total_process++;                                                    \
    if (OB_FAIL(ret)) {                                                 \
      TRANS_LOG(WARN, "handle txn message fail", KR(ret), "msg", arg_); \
    }                                                                   \
  }                                                                     \
  if (REACH_TIME_INTERVAL(STATISTICS_INTERVAL_US)) {                    \
    TRANS_LOG(INFO, "transaction rpc statistics",                       \
              "total_rt", (int64_t)total_rt,                            \
              "total_process", (int64_t)total_process,                  \
              "avg_rt", total_rt / (total_process + 1));                \
    total_rt = 0;                                                       \
    total_process = 0;                                                  \
  }                                                                     \
  return OB_SUCCESS;                                                    \
}

TX_Process(Commit, handle_trans_commit_request);
TX_Process(CommitResp, handle_trans_commit_response);
TX_Process(Abort, handle_trans_abort_request);
TX_Process(RollbackSP, handle_sp_rollback_request);
TX_Process(Keepalive, handle_trans_keepalive);
TX_Process(KeepaliveResp, handle_trans_keepalive_response);
TX_Process(SubPrepare, handle_sub_prepare_request);
TX_Process(SubPrepareResp, handle_sub_prepare_response);
TX_Process(SubCommit, handle_sub_commit_request);
TX_Process(SubCommitResp, handle_sub_commit_response);
TX_Process(SubRollback, handle_sub_rollback_request);
TX_Process(SubRollbackResp, handle_sub_rollback_response);

} // obrpc

namespace transaction
{
int ObTransRpc::init(ObTransService *trans_service,
                     rpc::frame::ObReqTransport *req_transport,
                     const common::ObAddr &self,
                     obrpc::ObBatchRpc *batch_rpc)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    TRANS_LOG(WARN, "ObTransRpc inited twice");
    ret = OB_INIT_TWICE;
  } else if (OB_ISNULL(req_transport)
          || OB_ISNULL(trans_service)
          || !self.is_valid()
          || OB_ISNULL(batch_rpc)) {
    TRANS_LOG(WARN, "invalid argument", KP(req_transport), KP(trans_service), K(self), KP(batch_rpc));
    ret = OB_INVALID_ARGUMENT;
  } else if (OB_SUCCESS != (ret = tx_commit_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_commit_resp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_abort_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_rollback_sp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_keepalive_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_keepalive_resp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_prepare_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_prepare_resp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_commit_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_commit_resp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_rollback_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_SUCCESS != (ret = tx_sub_rollback_resp_cb_.init())) {
    TRANS_LOG(WARN, "transaction callback init error", KR(ret));
  } else if (OB_FAIL(rpc_proxy_.init(req_transport, self))) {
    TRANS_LOG(WARN, "init rpc_proxy fail", KR(ret));
  } else {
    trans_service_ = trans_service;
    last_stat_ts_ = ObTimeUtility::current_time();
    batch_rpc_ = batch_rpc;
    is_inited_ = true;
    TRANS_LOG(INFO, "transaction rpc inited success");
  }
  return ret;
}

int ObTransRpc::start()
{
  int ret = OB_SUCCESS;

  if (!is_inited_) {
    TRANS_LOG(WARN, "ObTransRpc is not inited");
    ret = OB_NOT_INIT;
  } else if (is_running_) {
    TRANS_LOG(WARN, "ObTransRpc is already running");
    ret = OB_ERR_UNEXPECTED;
  } else {
    is_running_ = true;
    TRANS_LOG(INFO, "ObTransRpc start success");
  }

  return ret;
}

void ObTransRpc::stop()
{
  if (!is_inited_) {
    TRANS_LOG(WARN, "ObTransRpc is not inited");
  } else if (!is_running_) {
    TRANS_LOG(WARN, "ObTransRpc already has been stopped");
  } else {
    is_running_ = false;
    TRANS_LOG(INFO, "ObTransRpc stop success");
  }
}

void ObTransRpc::wait()
{
  if (!is_inited_) {
    TRANS_LOG(WARN, "ObTransRpc is not inited");
  } else if (is_running_) {
    TRANS_LOG(WARN, "ObTransRpc is already running");
  } else {
    TRANS_LOG(INFO, "ObTransRpc wait success");
  }
}

void ObTransRpc::destroy()
{
  if (is_inited_) {
    if (is_running_) {
      stop();
      wait();
    }
    is_inited_ = false;
    trans_service_ = NULL;
    TRANS_LOG(INFO, "transaction rpc destroyed");
  }
}
int ObTransRpc::post_commit_msg_(const ObAddr &server, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
  const int64_t msg_type = msg.get_msg_type();
  const uint64_t tenant_id = msg.tenant_id_;
  switch (msg_type)
  {
    case TX_COMMIT:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_commit_msg(static_cast<ObTxCommitMsg&>(msg), &tx_commit_cb_);
      break;
    }
    case TX_COMMIT_RESP:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_commit_resp_msg(static_cast<ObTxCommitRespMsg&>(msg), &tx_commit_resp_cb_);
      break;
    }
    case TX_ABORT:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_abort_msg(static_cast<ObTxAbortMsg&>(msg), &tx_abort_cb_);
      break;
    }
    default:
      ret = OB_NOT_SUPPORTED;
      TRANS_LOG(WARN, "rpc proxy not supported", K(tenant_id), K(server), K(msg));
      break;
  }
  return ret;
}

int ObTransRpc::post_(const ObAddr &server, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
  const int64_t msg_type = msg.get_msg_type();
  const uint64_t tenant_id = msg.tenant_id_;
  switch (msg_type)
  {
    case ROLLBACK_SAVEPOINT:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_rollback_sp_msg(static_cast<ObTxRollbackSPMsg &>(msg), &tx_rollback_sp_cb_);
      break;
    }
    case KEEPALIVE:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_keep_alive_msg(static_cast<ObTxKeepaliveMsg &>(msg), &tx_keepalive_cb_);
      break;
    }
    case KEEPALIVE_RESP:
    {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_keep_alive_resp_msg(static_cast<ObTxKeepaliveRespMsg &>(msg), &tx_keepalive_resp_cb_);
      break;
    }
    case TX_COMMIT:
    case TX_COMMIT_RESP:
    case TX_ABORT:
    {
      // Why we shoud set a new mehtod : post_commit_msg ?
      // Method stack size is overflow（max size = 10KB) because of rpc_proxy deep_copy
      ret = post_commit_msg_(server, msg);
      break;
    }
    case SUBPREPARE:
    case SUBCOMMIT:
    case SUBROLLBACK:
    {
      ret = post_sub_request_msg_(server, msg);
      break;
    }
    case SUBPREPARE_RESP:
    case SUBCOMMIT_RESP:
    case SUBROLLBACK_RESP:
    {
      ret = post_sub_response_msg_(server, msg);
      break;
    }
    default:
      ret = OB_NOT_SUPPORTED;
      TRANS_LOG(WARN, "rpc proxy not supported", K(tenant_id), K(server), K(msg));
      break;
  }
  return ret;
}

int ObTransRpc::post_msg(const ObAddr &server, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
  uint64_t tenant_id = msg.get_tenant_id();

  if (OB_UNLIKELY(!is_inited_)) {
    TRANS_LOG(WARN, "ObTransRpc not inited");
    ret = OB_NOT_INIT;
  } else if (OB_UNLIKELY(!is_running_)) {
    TRANS_LOG(WARN, "ObTransRpc is not running");
    ret = OB_NOT_RUNNING;
  } else if (OB_UNLIKELY(!is_valid_tenant_id(tenant_id)) ||
      OB_UNLIKELY(!server.is_valid()) || OB_UNLIKELY(!msg.is_valid())) {
    TRANS_LOG(WARN, "invalid argument", K(tenant_id), K(server), K(msg));
    ret = OB_INVALID_ARGUMENT;
  } else if (ObTxMsgTypeChecker::is_2pc_msg_type(msg.get_msg_type())) {
    if (OB_FAIL(batch_rpc_->post(msg.tenant_id_,
                                 server,
                                 obrpc::ObRpcNetHandler::CLUSTER_ID,
                                 obrpc::TRX_BATCH_REQ_NODELAY,
                                 msg.get_msg_type(),
                                 msg.get_receiver(),
                                 msg))) {
      TRANS_LOG(WARN, "post msg failed", K(ret));
    }
  } else if (OB_FAIL(post_(server, msg))) {
    TRANS_LOG(WARN, "post msg error", K(ret), K(server), K(msg));
  } else {
    // do nothing
  }

  if (OB_SUCC(ret)) {
    total_trans_msg_count_++;
    statistics_();
    TRANS_LOG(DEBUG, "post transaction message success", K(msg));
  }

  return ret;
}

int ObTransRpc::post_msg(const ObLSID &p, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
#ifdef TRANS_ERROR
  const int64_t random = ObRandom::rand(1, 100);
  if (0 == random % 20) {
    //mock package drop: 5%
    TRANS_LOG(INFO, "post trans msg failed for random error (discard msg)", K(tenant_id), K(server), K(msg));
    return ret;
  } else if (0 == random % 50) {
    TRANS_LOG(INFO, "post trans msg failed for random error (delayed msg)", K(tenant_id), K(server), K(msg));
  } else {
    // do nothing
  }
#endif

  uint64_t tenant_id = msg.tenant_id_;
  int64_t cluster_id = GCONF.cluster_id;
  ObAddr server;

  if (OB_UNLIKELY(!is_inited_)) {
    TRANS_LOG(WARN, "ObTransRpc not inited");
    ret = OB_NOT_INIT;
  } else if (OB_UNLIKELY(!is_running_)) {
    TRANS_LOG(WARN, "ObTransRpc is not running");
    ret = OB_NOT_RUNNING;
  } else if (OB_UNLIKELY(!is_valid_tenant_id(tenant_id)) || OB_UNLIKELY(!msg.is_valid())) {
    TRANS_LOG(WARN, "invalid argument", K(tenant_id), K(msg));
    ret = OB_INVALID_ARGUMENT;
  } else if (OB_FAIL(trans_service_->get_location_adapter()->nonblock_get_leader(cluster_id, tenant_id, p, server))) {
    TRANS_LOG(WARN, "get leader failed", KR(ret), K(msg), K(cluster_id), K(p));
  } else if (ObTxMsgTypeChecker::is_2pc_msg_type(msg.get_msg_type())) {
    // 2pc msg optimization
    const int64_t dst_cluster_id = obrpc::ObRpcNetHandler::CLUSTER_ID;
    uint64_t tenant_id = msg.tenant_id_;
    if (OB_FAIL(batch_rpc_->post(tenant_id,
                                 server,
                                 dst_cluster_id,
                                 obrpc::TRX_BATCH_REQ_NODELAY,
                                 msg.get_msg_type(),
                                 msg.get_receiver(),
                                 msg))) {
      TRANS_LOG(WARN, "post msg failed", K(ret));
    }
  } else if (OB_FAIL(post_(server, msg))) {
    TRANS_LOG(WARN, "post msg error", K(ret), K(server), K(msg));
  } else {
    // do nothing
  }

  if (OB_SUCC(ret)) {
    total_trans_msg_count_++;
    statistics_();
    TRANS_LOG(DEBUG, "post transaction message success", K(msg));
  }

  return ret;
}

int ObTransRpc::post_sub_request_msg_(const ObAddr &server, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
  const int64_t msg_type = msg.get_msg_type();
  const uint64_t tenant_id = msg.tenant_id_;
  switch (msg_type) {
    case SUBPREPARE: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_prepare_msg(static_cast<ObTxSubPrepareMsg&>(msg), &tx_sub_prepare_cb_);
      break;
    }
    case SUBCOMMIT: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_commit_msg(static_cast<ObTxSubCommitMsg&>(msg), &tx_sub_commit_cb_);
      break;
    }
    case SUBROLLBACK: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_rollback_msg(static_cast<ObTxSubRollbackMsg&>(msg), &tx_sub_rollback_cb_);
      break;
    }
    default: {
      ret = OB_NOT_SUPPORTED;
      TRANS_LOG(WARN, "rpc proxy not supported", K(tenant_id), K(server), K(msg));
      break;
    }
  }
  return ret;
}

int ObTransRpc::post_sub_response_msg_(const ObAddr &server, ObTxMsg &msg)
{
  int ret = OB_SUCCESS;
  const int64_t msg_type = msg.get_msg_type();
  const uint64_t tenant_id = msg.tenant_id_;
  switch (msg_type) {
    case SUBPREPARE_RESP: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_prepare_resp_msg(static_cast<ObTxSubPrepareRespMsg&>(msg), &tx_sub_prepare_resp_cb_);
      break;
    }
    case SUBCOMMIT_RESP: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_commit_resp_msg(static_cast<ObTxSubCommitRespMsg&>(msg), &tx_sub_commit_resp_cb_);
      break;
    }
    case SUBROLLBACK_RESP: {
      ret = rpc_proxy_.to(server).by(tenant_id).timeout(GCONF._ob_trans_rpc_timeout).
              post_sub_rollback_resp_msg(static_cast<ObTxSubRollbackRespMsg&>(msg), &tx_sub_rollback_resp_cb_);
      break;
    }
    default: {
      ret = OB_NOT_SUPPORTED;
      TRANS_LOG(WARN, "rpc proxy not supported", K(tenant_id), K(server), K(msg));
      break;
    }
  }
  return ret;
}

void ObTransRpc::statistics_()
{
  const int64_t cur_ts = ObTimeUtility::current_time();
  if (cur_ts - last_stat_ts_ > STAT_INTERVAL) {
    TRANS_LOG(INFO, "rpc statistics", K_(total_trans_msg_count), K_(total_trans_resp_msg_count));
    total_trans_msg_count_ = 0;
    total_trans_resp_msg_count_ = 0;
    last_stat_ts_ = cur_ts;
  }
}

} // transaction

} // oceanbase
