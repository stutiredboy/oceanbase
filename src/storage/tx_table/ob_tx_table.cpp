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

#define USING_LOG_PREFIX STORAGE
#include "storage/tx_table/ob_tx_table.h"

#include "share/ob_ls_id.h"
#include "share/schema/ob_table_schema.h"
#include "storage/ls/ob_ls.h"
#include "storage/memtable/mvcc/ob_mvcc_ctx.h"
#include "storage/access/ob_store_row_iterator.h"
#include "storage/tx/ob_trans_ctx_mgr.h"
#include "storage/tx/ob_trans_define.h"
#include "storage/tx/ob_trans_part_ctx.h"
#include "storage/tx/ob_trans_service.h"
#include "storage/tx_storage/ob_ls_map.h"
#include "storage/tx_storage/ob_ls_service.h"
#include "storage/tx/ob_tx_data_functor.h"
#include "storage/tx_table/ob_tx_table_define.h"
#include "storage/tx_table/ob_tx_table_iterator.h"
#include "storage/tablet/ob_tablet.h"
#include "storage/tablet/ob_tablet_iterator.h"

namespace oceanbase
{
using namespace share;
namespace storage
{
int ObTxTable::init(ObLS *ls)
{
  int ret = OB_SUCCESS;
  if (IS_INIT) {
    ret = OB_INIT_TWICE;
    LOG_WARN("init twice", K(ret));
  } else if (OB_ISNULL(ls)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("invalid argument", K(ret), KP(ls));
  } else if (OB_FAIL(tx_data_table_.init(ls, &tx_ctx_table_))) {
    LOG_WARN("tx data table init failed", K(ret));
  } else if (OB_FAIL(tx_ctx_table_.init(ls->get_ls_id()))) {
    LOG_WARN("tx ctx table init failed", K(ret));
  } else {
    ls_ = ls;
    epoch_ = 0;
    max_tablet_clog_checkpoint_ = 0;
    state_ = TxTableState::ONLINE;
    LOG_INFO("init tx table successfully", K(ret), K(ls->get_ls_id()));
    is_inited_ = true;
  }
  if (OB_FAIL(ret)) {
    destroy();
  }
  return ret;
}

int ObTxTable::start()
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(tx_data_table_.start())) {
    LOG_WARN("start tx data table failed.", KR(ret));
  } else {
    LOG_INFO("tx table start finish", KR(ret), KPC(this));
  }
  return ret;
}

void ObTxTable::stop()
{
  ATOMIC_STORE(&state_, TxTableState::OFFLINE);
  tx_data_table_.stop();
  LOG_INFO("tx table stop finish", KPC(this));
}

int ObTxTable::prepare_offline()
{
  int ret = OB_SUCCESS;
  ATOMIC_STORE(&state_, TxTableState::PREPARE_OFFLINE);
  TRANS_LOG(INFO, "tx table prepare offline succeed", KPC(this));
  return ret;
}

int ObTxTable::offline_tx_ctx_table_()
{
  int ret = OB_SUCCESS;
  ObTabletHandle handle;
  ObTablet *tablet;
  ObLSTabletService *ls_tablet_svr = ls_->get_tablet_svr();

  if (NULL == ls_tablet_svr) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("get ls tablet svr failed", K(ret));
  } else if (OB_FAIL(ls_tablet_svr->get_tablet(LS_TX_CTX_TABLET,
                                               handle))) {
    LOG_WARN("get tablet failed", K(ret));
    if (OB_TABLET_NOT_EXIST == ret) {
      // a ls that of migrate does not have tx ctx tablet
      ret = OB_SUCCESS;
    }
  } else if (FALSE_IT(tablet = handle.get_obj())) {
  } else if (OB_FAIL(tablet->release_memtables())) {
    LOG_WARN("failed to release memtables", K(ret), KPC(ls_));
  } else if (OB_FAIL(tx_ctx_table_.offline())) {
    LOG_WARN("failed to offline tx ctx table", K(ret), KPC(ls_));
  } else {
    // do nothing
  }

  return ret;
}

int ObTxTable::offline_tx_data_table_()
{
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    STORAGE_LOG(WARN, "tx table is not init", KR(ret));
  } else if (OB_FAIL(tx_data_table_.offline())) {
    STORAGE_LOG(WARN, "tx data table offline failed", KR(ret), "ls_id", ls_->get_ls_id());
  }
  return ret;
}

int ObTxTable::offline()
{
  int ret = OB_SUCCESS;
  ObTabletHandle handle;
  ObTablet *tablet;
  ObLSTabletService *ls_tablet_svr = nullptr;

  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret));
  } else if (OB_FAIL(offline_tx_ctx_table_())) {
    LOG_WARN("offline tx ctx table failed", K(ret));
  } else if (OB_FAIL(offline_tx_data_table_())) {
    LOG_WARN("offline tx data table failed", K(ret));
  } else {
    ATOMIC_STORE(&state_, TxTableState::OFFLINE);
    LOG_INFO("tx table offline succeed", "ls_id", ls_->get_ls_id(), KPC(this));
  }

  return ret;
}

int ObTxTable::prepare_online()
{
  int ret = OB_SUCCESS;
  int64_t tmp_max_tablet_clog_checkpiont = -1;
  max_tablet_clog_checkpoint_ = INT64_MAX;

  ATOMIC_INC(&epoch_);

  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret));
  } else if (OB_FAIL(get_max_tablet_clog_checkpoint_(tmp_max_tablet_clog_checkpiont))) {
    LOG_WARN("get max tablet clog checkpint failed", KR(ret));
  } else if (OB_FAIL(load_tx_data_table_())) {
    LOG_WARN("failed to load tx data table", K(ret));
  } else if (OB_FAIL(load_tx_ctx_table_())) {
    LOG_WARN("failed to load tx ctx table", K(ret));
  } else {
    max_tablet_clog_checkpoint_ = tmp_max_tablet_clog_checkpiont;
    prepare_online_ts_ = ObTimeUtil::current_time();
    ATOMIC_STORE(&state_, ObTxTable::PREPARE_ONLINE);
    LOG_INFO("tx table prepare online succeed", "ls_id", ls_->get_ls_id(), KPC(this));
  }

  return ret;
}

int ObTxTable::check_and_online()
{
  int ret = OB_SUCCESS;
  int64_t max_consequent_callbacked_log_ts = 0;

  if (OB_FAIL(ls_->get_max_decided_log_ts_ns(max_consequent_callbacked_log_ts))) {
    LOG_WARN("get max decided log ts from ls failed", KR(ret), "ls_id", ls_->get_ls_id());
  } else if (max_consequent_callbacked_log_ts >= max_tablet_clog_checkpoint_) {
    ATOMIC_STORE(&state_, TxTableState::ONLINE);
    LOG_INFO("tx table online finish",
             "ls_id",
             ls_->get_ls_id(),
             K(max_consequent_callbacked_log_ts),
             K(max_tablet_clog_checkpoint_));
  } else {
    int64_t current_ts = ObTimeUtil::current_time();
    int64_t time_after_prepare_online_ms = (current_ts - prepare_online_ts_) / 1000LL;
    LOG_INFO("tx table is PREPARE_ONLINE but not ONLINE yet",
             "ls_id", ls_->get_ls_id(),
             K(max_consequent_callbacked_log_ts),
             K(max_tablet_clog_checkpoint_),
             K(time_after_prepare_online_ms),
             KTIME(current_ts),
             KTIME(prepare_online_ts_));
  }
  return ret;
}


int ObTxTable::prepare_for_safe_destroy()
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(tx_data_table_.prepare_for_safe_destroy())) {
    LOG_WARN("tx data table prepare for safe destory failed", KR(ret));
  }
  return ret;
}

int ObTxTable::create_tablet(const lib::Worker::CompatMode compat_mode, const int64_t create_scn)
{
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else {
    const uint64_t tenant_id = ls_->get_tenant_id();
    const share::ObLSID &ls_id = ls_->get_ls_id();
    if (OB_FAIL(create_data_tablet_(tenant_id,
                                    ls_id,
                                    compat_mode,
                                    create_scn))) {
      LOG_WARN("create data tablet failed", K(ret));
    } else if (OB_FAIL(create_ctx_tablet_(tenant_id,
                                          ls_id,
                                          compat_mode,
                                          create_scn))) {
      LOG_WARN("create ctx tablet failed", K(ret));
    }
    if (OB_FAIL(ret)) {
      int tmp_ret = remove_tablet();
      LOG_WARN("remove tablet init failed", K(tmp_ret));
    }
  }
  return ret;
}

int ObTxTable::get_ctx_table_schema_(
    const uint64_t tenant_id,
    share::schema::ObTableSchema &schema)
{
  int ret = OB_SUCCESS;
  uint64_t table_id = ObTabletID::LS_TX_CTX_TABLET_ID;
  const char *const AUTO_INC_ID = "id";
  const char *const META_NAME = "tx_info_meta";
  const char *const VALUE_NAME = "tx_info";
  const int64_t SCHEMA_VERSION = 1;
  const char *const TABLE_NAME = "tx_ctx_table";

  common::ObObjMeta INC_ID_TYPE;
  INC_ID_TYPE.set_int();
  ObColumnSchemaV2 id_column;
  id_column.set_tenant_id(tenant_id);
  id_column.set_table_id(table_id);
  id_column.set_column_id(common::OB_APP_MIN_COLUMN_ID);
  id_column.set_schema_version(SCHEMA_VERSION);
  id_column.set_rowkey_position(1);
  id_column.set_order_in_rowkey(ObOrderType::ASC);
  id_column.set_meta_type(INC_ID_TYPE); // int64_t

  common::ObObjMeta META_TYPE;
  META_TYPE.set_binary();
  ObColumnSchemaV2 meta_column;
  meta_column.set_tenant_id(tenant_id);
  meta_column.set_table_id(table_id);
  meta_column.set_column_id(common::OB_APP_MIN_COLUMN_ID + 1);
  meta_column.set_schema_version(SCHEMA_VERSION);
  meta_column.set_data_length(MAX_TX_CTX_TABLE_META_LENGTH);
  meta_column.set_meta_type(META_TYPE);

  common::ObObjMeta DATA_TYPE;
  DATA_TYPE.set_binary();
  ObColumnSchemaV2 value_column;
  value_column.set_tenant_id(tenant_id);
  value_column.set_table_id(table_id);
  value_column.set_column_id(common::OB_APP_MIN_COLUMN_ID + 2);
  value_column.set_schema_version(SCHEMA_VERSION);
  value_column.set_data_length(MAX_TX_CTX_TABLE_VALUE_LENGTH);
  value_column.set_meta_type(DATA_TYPE);

  schema.set_tenant_id(tenant_id);
  schema.set_database_id(OB_SYS_DATABASE_ID);
  schema.set_table_id(table_id);
  schema.set_schema_version(SCHEMA_VERSION);

  if (OB_FAIL(id_column.set_column_name(AUTO_INC_ID))) {
    LOG_WARN("failed to set column name", K(ret), K(AUTO_INC_ID));
  } else if (OB_FAIL(meta_column.set_column_name(META_NAME))) {
    LOG_WARN("failed to set column name", K(ret), K(META_NAME));
  } else if (OB_FAIL(value_column.set_column_name(VALUE_NAME))) {
    LOG_WARN("failed to set column name", K(ret), K(VALUE_NAME));
  } else if (OB_FAIL(schema.set_table_name(TABLE_NAME))) {
    LOG_WARN("failed to set table name", K(ret), K(TABLE_NAME));
  } else if (OB_FAIL(schema.add_column(id_column))) {
    LOG_WARN("failed to add column", K(ret), K(id_column));
  } else if (OB_FAIL(schema.add_column(meta_column))) {
    LOG_WARN("failed to add column", K(ret), K(meta_column));
  } else if (OB_FAIL(schema.add_column(value_column))) {
    LOG_WARN("failed to add column", K(ret), K(value_column));
  }
  return ret;
}

int ObTxTable::gen_create_tablet_arg_(
    const ObTabletID &tablet_id,
    const uint64_t tenant_id,
    const ObLSID ls_id,
    const lib::Worker::CompatMode compat_mode,
    const schema::ObTableSchema &table_schema,
    obrpc::ObBatchCreateTabletArg &arg)
{
  int ret = OB_SUCCESS;
  ObCreateTabletInfo create_tablet_info;
  ObArray<common::ObTabletID> tablet_ids;
  ObArray<int64_t> tablet_schema_idxs;

  // frozen_timestamp: next merge time
  int64_t frozen_timestamp = LS_INNER_TABLET_FROZEN_TIMESTAMP;

  arg.reset();
  // create ObCreateTabletInfo
  if (OB_FAIL(tablet_ids.push_back(tablet_id))) {
    LOG_WARN("insert tablet id failed", K(ret), K(tablet_id));
    // only one tablet, only one schema
  } else if (OB_FAIL(tablet_schema_idxs.push_back(0))) {
    LOG_WARN("insert tablet schema idx failed", K(ret));
  //TODO ls inited in sys tenant, the special tablet always init as mysql mode?? @cxf262476
  } else if (OB_FAIL(create_tablet_info.init(tablet_ids,
                                             tablet_id,
                                             tablet_schema_idxs,
                                             compat_mode,
                                             false/*is_create_bind_hidden_tablets*/))) {
    LOG_WARN("create tablet info init failed", K(ret), K(tablet_ids), K(tablet_id));
  // create ObBatchCreateTabletArg
  } else if (OB_FAIL(arg.init_create_tablet(ls_id,
                                            frozen_timestamp))) {
    LOG_WARN("ObBatchCreateTabletArg init create tablet failed", K(ret), K(tenant_id),
             K(ls_id), K(frozen_timestamp));
  } else if (OB_FAIL(arg.table_schemas_.push_back(table_schema))) {
    LOG_WARN("add table schema failed", K(ret), K(table_schema));
  } else if (OB_FAIL(arg.tablets_.push_back(create_tablet_info))) {
    LOG_WARN("add create tablet info failed", K(ret), K(create_tablet_info));
  }

  return ret;
}

int ObTxTable::gen_remove_tablet_arg_(
    const common::ObTabletID &tablet_id,
    const uint64_t tenant_id,
    const share::ObLSID ls_id,
    obrpc::ObBatchRemoveTabletArg &arg)
{
  int ret = OB_SUCCESS;
  arg.reset();
  if (OB_FAIL(arg.tablet_ids_.push_back(tablet_id))) {
    LOG_WARN("insert tablet id failed", K(ret), K(tablet_id));
  } else {
    arg.id_ = ls_id;
  }
  return ret;
}

int ObTxTable::create_ctx_tablet_(
    const uint64_t tenant_id,
    const ObLSID ls_id,
    const lib::Worker::CompatMode compat_mode,
    const int64_t create_scn)
{
  int ret = OB_SUCCESS;
  obrpc::ObBatchCreateTabletArg arg;
  const bool no_need_write_clog = true;
  share::schema::ObTableSchema table_schema;

  if (OB_FAIL(get_ctx_table_schema_(tenant_id,
                                    table_schema))) {
    LOG_WARN("get ctx table schema failed", K(ret));
  } else if (OB_FAIL(gen_create_tablet_arg_(LS_TX_CTX_TABLET,
                                            tenant_id,
                                            ls_id,
                                            compat_mode,
                                            table_schema,
                                            arg))) {
    LOG_WARN("gen create tablet arg failed", K(ret), K(LS_TX_CTX_TABLET), K(tenant_id),
             K(ls_id), K(table_schema));
  } else if (OB_FAIL(ls_->batch_create_tablets(arg,
                                               create_scn,
                                               no_need_write_clog))) {
    LOG_WARN("create ctx tablet failed", K(ret), K(arg), K(create_scn));
  }
  return ret;
}

int ObTxTable::get_data_table_schema_(const uint64_t tenant_id,
                                      share::schema::ObTableSchema &schema)
{
  int ret = OB_SUCCESS;
  uint64_t table_id = ObTabletID::LS_TX_DATA_TABLET_ID;
  const char *const TX_ID_NAME = "tx_id";
  const char *const IDX_NAME = "idx";
  const char *const TOTAL_ROW_CNT_NAME = "total_row_cnt";
  const char *const END_TS_NAME = "end_log_ts";
  const char *const VALUE_NAME = "tx_info";
  const char *const TABLE_NAME = "tx_data_table";
  const int64_t SCHEMA_VERSION = 1;
  const int64_t MAX_TX_ID_LENGTH = 100; // the real length is no more than 64 + 1
  const int64_t MAX_TX_INFO_LENGTH = common::OB_MAX_VARCHAR_LENGTH;

  common::ObObjMeta TX_ID_TYPE;
  TX_ID_TYPE.set_int();
  common::ObObjMeta IDX_TYPE;
  IDX_TYPE.set_int();
  common::ObObjMeta TOTAL_ROW_CNT_TYPE;
  TOTAL_ROW_CNT_TYPE.set_int();
  common::ObObjMeta END_TS_TYPE;
  END_TS_TYPE.set_int();
  common::ObObjMeta DATA_TYPE;
  DATA_TYPE.set_binary();

  ObColumnSchemaV2 tx_id_column;
  tx_id_column.set_tenant_id(tenant_id);
  tx_id_column.set_table_id(table_id);
  tx_id_column.set_column_id(ObTxDataTable::TX_ID);
  tx_id_column.set_schema_version(SCHEMA_VERSION);
  tx_id_column.set_rowkey_position(1);
  tx_id_column.set_order_in_rowkey(ObOrderType::ASC);
  tx_id_column.set_meta_type(TX_ID_TYPE); // int64_t

  ObColumnSchemaV2 idx_column;
  idx_column.set_tenant_id(tenant_id);
  idx_column.set_table_id(table_id);
  idx_column.set_column_id(ObTxDataTable::IDX);
  idx_column.set_schema_version(SCHEMA_VERSION);
  idx_column.set_rowkey_position(2);
  idx_column.set_order_in_rowkey(ObOrderType::ASC);
  idx_column.set_meta_type(IDX_TYPE); // int64_t

  ObColumnSchemaV2 total_row_cnt_column;
  total_row_cnt_column.set_tenant_id(tenant_id);
  total_row_cnt_column.set_table_id(table_id);
  total_row_cnt_column.set_column_id(ObTxDataTable::TOTAL_ROW_CNT);
  total_row_cnt_column.set_schema_version(SCHEMA_VERSION);
  total_row_cnt_column.set_meta_type(TOTAL_ROW_CNT_TYPE);
  total_row_cnt_column.set_rowkey_position(0);

  ObColumnSchemaV2 end_ts_column;
  end_ts_column.set_tenant_id(tenant_id);
  end_ts_column.set_table_id(table_id);
  end_ts_column.set_column_id(ObTxDataTable::END_LOG_TS);
  end_ts_column.set_schema_version(SCHEMA_VERSION);
  end_ts_column.set_meta_type(END_TS_TYPE);
  end_ts_column.set_rowkey_position(0);

  ObColumnSchemaV2 value_column;
  value_column.set_tenant_id(tenant_id);
  value_column.set_table_id(table_id);
  value_column.set_column_id(ObTxDataTable::VALUE);
  value_column.set_schema_version(SCHEMA_VERSION);
  value_column.set_data_length(MAX_TX_INFO_LENGTH);
  value_column.set_meta_type(DATA_TYPE);
  value_column.set_rowkey_position(0);

  schema.set_tenant_id(tenant_id);
  schema.set_database_id(OB_SYS_DATABASE_ID);
  schema.set_table_id(table_id);
  schema.set_schema_version(SCHEMA_VERSION);

  if (OB_FAIL(tx_id_column.set_column_name(TX_ID_NAME))) {
    LOG_WARN("failed to set column name", KR(ret), K(TX_ID_NAME));
  } else if (OB_FAIL(idx_column.set_column_name(IDX_NAME))) {
    LOG_WARN("failed to set column name", KR(ret), K(IDX_NAME));
  } else if (OB_FAIL(total_row_cnt_column.set_column_name(TOTAL_ROW_CNT_NAME))) {
    LOG_WARN("failed to set column name", KR(ret), K(TOTAL_ROW_CNT_NAME));
  } else if (OB_FAIL(end_ts_column.set_column_name(END_TS_NAME))) {
    LOG_WARN("failed to set column name", KR(ret), K(END_TS_NAME));
  } else if (OB_FAIL(value_column.set_column_name(VALUE_NAME))) {
    LOG_WARN("failed to set column name", KR(ret), K(VALUE_NAME));
  } else if (OB_FAIL(schema.set_table_name(TABLE_NAME))) {
    LOG_WARN("failed to set table name", K(ret), K(TABLE_NAME));
  } else if (OB_FAIL(schema.add_column(tx_id_column))) {
    LOG_WARN("failed to add column", K(ret), K(tx_id_column));
  } else if (OB_FAIL(schema.add_column(idx_column))) {
    LOG_WARN("failed to add column", K(ret), K(idx_column));
  } else if (OB_FAIL(schema.add_column(total_row_cnt_column))) {
    LOG_WARN("failed to add column", K(ret), K(total_row_cnt_column));
  } else if (OB_FAIL(schema.add_column(end_ts_column))) {
    LOG_WARN("failed to add column", K(ret), K(end_ts_column));
  } else if (OB_FAIL(schema.add_column(value_column))) {
    LOG_WARN("failed to add column", K(ret), K(value_column));
  }
  return ret;
}

int ObTxTable::create_data_tablet_(
    const uint64_t tenant_id,
    const ObLSID ls_id,
    const lib::Worker::CompatMode compat_mode,
    const int64_t create_scn)
{
  int ret = OB_SUCCESS;
  obrpc::ObBatchCreateTabletArg arg;
  const bool no_need_write_clog = true;
  share::schema::ObTableSchema table_schema;

  if (OB_FAIL(get_data_table_schema_(tenant_id,
                                     table_schema))) {
    LOG_WARN("get data table schema failed", K(ret));
  } else if (OB_FAIL(gen_create_tablet_arg_(LS_TX_DATA_TABLET,
                                            tenant_id,
                                            ls_id,
                                            compat_mode,
                                            table_schema,
                                            arg))) {
    LOG_WARN("gen create tablet arg failed", K(ret), K(LS_TX_DATA_TABLET), K(tenant_id),
             K(ls_id), K(table_schema));
  } else if (OB_FAIL(ls_->batch_create_tablets(arg,
                                               create_scn,
                                               no_need_write_clog))) {
    LOG_WARN("create ctx tablet failed", K(ret), K(arg), K(create_scn));
  }
  return ret;
}

int ObTxTable::remove_tablet_(const common::ObTabletID &tablet_id)
{
  int ret = OB_SUCCESS;
  obrpc::ObBatchRemoveTabletArg arg;
  const bool no_need_write_clog = true;
  uint64_t tenant_id = ls_->get_tenant_id();
  if (OB_FAIL(gen_remove_tablet_arg_(tablet_id,
                                     tenant_id,
                                     ls_->get_ls_id(),
                                     arg))) {
    LOG_WARN("gen remove tablet arg failed", K(ret), K(tablet_id), K(tenant_id), K(ls_->get_ls_id()));
  } else if (OB_FAIL(ls_->batch_remove_tablets(arg,
                                               no_need_write_clog))) {
    LOG_WARN("remove tablet failed", K(ret), K(arg));
  }
  return ret;
}

int ObTxTable::remove_data_tablet_()
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(remove_tablet_(LS_TX_DATA_TABLET))) {
    LOG_WARN("remove tablet failed", K(ret), K(LS_TX_DATA_TABLET));
  }
  return ret;
}

int ObTxTable::remove_ctx_tablet_()
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(remove_tablet_(LS_TX_CTX_TABLET))) {
    LOG_WARN("remove tablet failed", K(ret), K(LS_TX_CTX_TABLET));
  }
  return ret;
}

int ObTxTable::remove_tablet()
{
  int ret = OB_SUCCESS;
  if (OB_NOT_NULL(ls_)) {
    if (OB_FAIL(remove_data_tablet_())) {
      LOG_WARN("remove data tablet failed", K(ret));
    }
    if (OB_FAIL(remove_ctx_tablet_())) {
      LOG_WARN("remove ctx tablet failed", K(ret));
    }
  }
  return ret;
}

int ObTxTable::load_tx_ctx_table_()
{
  int ret = OB_SUCCESS;
  ObTabletHandle handle;
  ObTablet *tablet;
  ObLSTabletService *ls_tablet_svr = ls_->get_tablet_svr();

  if (NULL == ls_tablet_svr) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("get ls tablet svr failed", K(ret));
  } else if (OB_FAIL(ls_tablet_svr->get_tablet(LS_TX_CTX_TABLET,
                                               handle))) {
    LOG_WARN("get tablet failed", K(ret));
  } else if (FALSE_IT(tablet = handle.get_obj())) {
  } else if (OB_FAIL(ls_tablet_svr->create_memtable(LS_TX_CTX_TABLET, 0 /* schema_version */))) {
    LOG_WARN("failed to create memtable", K(ret));
  } else {
    ObTabletTableStore &table_store = tablet->get_table_store();
    ObSSTableArray &sstables = table_store.get_minor_sstables();

    if (!sstables.empty()) {
      ret = restore_tx_ctx_table_(*sstables[0]);
    }
  }

  return ret;
}

int ObTxTable::load_tx_data_table_()
{
  int ret = OB_SUCCESS;
  ObTabletHandle handle;
  ObTablet *tablet;
  ObLSTabletService *ls_tablet_svr = ls_->get_tablet_svr();

  if (NULL == ls_tablet_svr) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("get ls tablet svr failed", K(ret));
  } else if (OB_FAIL(ls_tablet_svr->get_tablet(LS_TX_DATA_TABLET,
                                               handle))) {
    LOG_WARN("get tablet failed", K(ret));
  } else if (FALSE_IT(tablet = handle.get_obj())) {
  } else if (OB_FAIL(ls_tablet_svr->create_memtable(LS_TX_DATA_TABLET, 0 /* schema_version */))) {
    LOG_WARN("failed to create memtable", K(ret));
  } else {
    // load tx data table succed
  }

  return ret;
}

int ObTxTable::get_max_tablet_clog_checkpoint_(int64_t &max_tablet_clog_checkpoint)
{
  int ret = OB_SUCCESS;
  ObLSTabletIterator tablet_iter(ObTabletCommon::DIRECT_GET_COMMITTED_TABLET_TIMEOUT_US);
  if (OB_FAIL(ls_->build_tablet_iter(tablet_iter))) {
    STORAGE_LOG(WARN, "get ls tablet iterator failed", KR(ret), KPC(this));
  } else {
    while (OB_SUCC(ret)) {
      ObTabletHandle tablet_handle;
      if (OB_FAIL(tablet_iter.get_next_tablet(tablet_handle))) {
        if (OB_ITER_END == ret) {
          // iterate tablet done.
        } else {
          STORAGE_LOG(WARN, "get next table from tablet iter failed", KR(ret), KPC(this));
        }
      } else if (OB_UNLIKELY(!tablet_handle.is_valid())) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("invalid tablet handle", KR(ret), K(tablet_handle), KPC(this));
      } else if (tablet_handle.get_obj()->get_tablet_meta().tablet_id_.is_inner_tablet()) {
        // skip inner tablet
      } else {
        int64_t tmp_clog_checkpoint = tablet_handle.get_obj()->get_clog_checkpoint_ts();
        if (tmp_clog_checkpoint > max_tablet_clog_checkpoint) {
          max_tablet_clog_checkpoint = tmp_clog_checkpoint;
        }
      }
    }

    if (OB_ITER_END == ret) {
      ret = OB_SUCCESS;
    }

    STORAGE_LOG(INFO,
                "get max tablet clog checkpiont finish",
                KR(ret),
                "ls_id", ls_->get_ls_id(),
                K(max_tablet_clog_checkpoint));
  }


  return ret;
}

int ObTxTable::load_tx_table()
{
  int ret = OB_SUCCESS;

  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret));
  } else if (OB_FAIL(load_tx_data_table_())) {
    LOG_WARN("failed to load tx data table", K(ret));
  } else if (OB_FAIL(load_tx_ctx_table_())) {
    LOG_WARN("failed to load tx ctx table", K(ret));
  } else {
    // do nothing
  }

  return ret;
}

int ObTxTable::restore_tx_ctx_table_(ObITable &trans_sstable)
{
  int ret = OB_SUCCESS;
  ObStoreRowIterator *row_iter = NULL;
  const blocksstable::ObDatumRow *row = NULL;

  ObArenaAllocator allocator;
  blocksstable::ObDatumRange whole_range;
  whole_range.set_whole_range();

  ObStoreCtx store_ctx;
  ObTableAccessContext access_context;

  common::ObQueryFlag query_flag;
  query_flag.use_row_cache_ = ObQueryFlag::DoNotUseCache;

  common::ObVersionRange trans_version_range;
  trans_version_range.base_version_ = 0;
  trans_version_range.multi_version_start_ = 0;
  trans_version_range.snapshot_version_ = common::ObVersionRange::MAX_VERSION - 2;

  common::ObSEArray<share::schema::ObColDesc, 2> columns;
  ObTableReadInfo read_info;
  share::schema::ObColDesc key;
  key.col_id_ = OB_APP_MIN_COLUMN_ID;
  key.col_type_.set_int();
  key.col_order_ = ObOrderType::ASC;

  share::schema::ObColDesc meta;
  meta.col_id_ = common::OB_APP_MIN_COLUMN_ID + 1;
  meta.col_type_.set_binary();
  meta.col_order_ = ObOrderType::ASC;

  share::schema::ObColDesc value;
  value.col_id_ = common::OB_APP_MIN_COLUMN_ID + 2;
  value.col_type_.set_binary();
  value.col_order_ = ObOrderType::ASC;

  ObTableIterParam iter_param;
  iter_param.tablet_id_ = LS_TX_CTX_TABLET;
  iter_param.table_id_ = 1;

  if (OB_FAIL(access_context.init(query_flag,
                                  store_ctx,
                                  allocator,
                                  trans_version_range))) {
    LOG_WARN("failed to init access context", K(ret));
  } else if (OB_FAIL(columns.push_back(key))) {
    LOG_WARN("failed to push back key", K(ret), K(key));
  } else if (OB_FAIL(columns.push_back(meta))) {
    LOG_WARN("failed to push back meta", K(ret), K(meta));
  } else if (OB_FAIL(columns.push_back(value))) {
    LOG_WARN("failed to push back value", K(ret), K(value));
  } else if (OB_FAIL(read_info.init(
              allocator,
              LS_TX_CTX_SCHEMA_COLUMN_CNT,
              LS_TX_CTX_SCHEMA_ROWKEY_CNT,
              lib::is_oracle_mode(),
              columns))) {
    LOG_WARN("Fail to init read_info", K(ret));
  } else if (FALSE_IT(iter_param.read_info_ = &read_info)) {
  } else if (FALSE_IT(iter_param.full_read_info_ = &read_info)) {
  } else if (OB_FAIL(trans_sstable.scan(iter_param,
                                        access_context,
                                        whole_range,
                                        row_iter))) {
    LOG_WARN("failed to scan trans table", K(ret));
  } else if (NULL == row_iter) {
    // do nothing
  } else {
    while (OB_SUCC(ret)) {
      if (OB_FAIL(row_iter->get_next_row(row))) {
        if (OB_ITER_END != ret) {
          LOG_WARN("failed to get next row", K(ret));
        }
      } else if (OB_FAIL(tx_ctx_table_.recover(*row, *tx_data_table_.get_slice_allocator()))) {
        LOG_WARN("failed to recover tx ctx table", K(ret));
      }
    }

    if (OB_ITER_END == ret) {
      FLOG_INFO("restore trans table in memory", K(ret), K(trans_sstable));
      ret = OB_SUCCESS;
    }
  }

  if (OB_NOT_NULL(row_iter)) {
    row_iter->~ObStoreRowIterator();
    row_iter = nullptr;
  }

  return ret;
}

void ObTxTable::destroy()
{
  tx_data_table_.destroy();
  tx_ctx_table_.reset();
  ls_ = nullptr;
  epoch_ = 0;
  is_inited_ = false;
}

int ObTxTable::alloc_tx_data(ObTxData *&tx_data) {
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret));
  } else if (OB_FAIL(tx_data_table_.alloc_tx_data(tx_data))) {
    LOG_WARN("allocate tx data from tx data table fail.", KR(ret));
  }
  return ret;
}

int ObTxTable::deep_copy_tx_data(ObTxData *in_tx_data, ObTxData *&out_tx_data)
{
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret));
  } else if (OB_FAIL(tx_data_table_.deep_copy_tx_data(in_tx_data, out_tx_data))) {
    LOG_WARN("deep copy tx data from tx data table fail", KR(ret));
  }
  return ret;
}

int ObTxTable::insert(ObTxData *&tx_data) {
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret), KPC(tx_data), KP(this));
  } else if (OB_FAIL(tx_data_table_.insert(tx_data))) {
    LOG_WARN("allocate tx data from tx data table fail.", KR(ret), KPC(tx_data));
  }
  return ret;
}

int ObTxTable::check_with_tx_data(const transaction::ObTransID tx_id,
                                  ObITxDataCheckFunctor &fn,
                                  const int64_t read_epoch)
{
  int ret = OB_SUCCESS;

  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret), K(tx_id));
  } else if (OB_SUCC(tx_ctx_table_.check_with_tx_data(tx_id, fn))) {
    TRANS_LOG(DEBUG, "tx ctx table check with tx data succeed", K(tx_id), K(fn));
  } else if (OB_TRANS_CTX_NOT_EXIST == ret && OB_FAIL(tx_data_table_.check_with_tx_data(tx_id, fn))) {
    if (OB_ITER_END == ret) {
      ret = OB_TRANS_CTX_NOT_EXIST;
    }
    LOG_WARN("check_with_tx_data in tx data table fail.", KR(ret), "ls_id", ls_->get_ls_id(), K(tx_id));
  }

  check_state_and_epoch_(tx_id, read_epoch, true/*need_log_error*/, ret);
  return ret;
}

void ObTxTable::check_state_and_epoch_(const transaction::ObTransID tx_id,
                                       const int64_t read_epoch,
                                       const bool need_log_error,
                                       int &ret)
{
  TxTableState state = ATOMIC_LOAD(&state_);
  int64_t epoch = ATOMIC_LOAD(&epoch_);

  if (OB_UNLIKELY(read_epoch != epoch)) {
    // offline or online has been executed on this tx table, return a specific error code to retry
    ret = OB_REPLICA_NOT_READABLE;
    LOG_WARN("tx table may offline or changed",
             KR(ret),
             "ls_id", ls_->get_ls_id(),
             "state", get_state_string(state),
             K(read_epoch),
             K(epoch));
  } else if (OB_UNLIKELY(TxTableState::ONLINE != state)) {
    // try switch state first
    bool try_switch_state_succ = false;
    if (TxTableState::PREPARE_ONLINE == state) {
      int tmp_ret = check_and_online();
      if (OB_SUCCESS != tmp_ret) {
        LOG_WARN("check and online failed", KR(tmp_ret));
      } else {
        try_switch_state_succ = true;
        state = ATOMIC_LOAD(&state_);
      }
    }

    // check again
    if (!try_switch_state_succ || TxTableState::ONLINE != state) {
      ret = OB_REPLICA_NOT_READABLE;
      LOG_WARN("tx table may offline or changed",
               KR(ret),
               "ls_id", ls_->get_ls_id(),
               "state", get_state_string(state),
               K(read_epoch),
               K(epoch));
    }
  } else if (OB_FAIL(ret)) {
    if (need_log_error && OB_TRANS_CTX_NOT_EXIST == ret) {
      LOG_ERROR("check with tx data failed.", KR(ret), K(tx_id), K(read_epoch), "ls_id", ls_->get_ls_id(), KPC(this));
    } else {
      LOG_WARN("check with tx data failed.", KR(ret), K(tx_id), K(read_epoch), "ls_id", ls_->get_ls_id(), KPC(this));
    }
  }
}

int ObTxTable::get_tx_table_guard(ObTxTableGuard &guard)
{
  return guard.init(this);
}

int64_t ObTxTable::get_filter_col_idx()
{
  return TX_DATA_END_TS_COLUMN + ObMultiVersionRowkeyHelpper::get_extra_rowkey_col_cnt();
}

int ObTxTable::check_row_locked(const transaction::ObTransID &read_trans_id,
                                const transaction::ObTransID &data_trans_id,
                                const int64_t sql_sequence,
                                const int64_t read_epoch,
                                storage::ObStoreRowLockState &lock_state)
{
  CheckRowLockedFunctor fn(read_trans_id, data_trans_id, sql_sequence, lock_state);
  int ret = check_with_tx_data(data_trans_id, fn, read_epoch);
  // TODO(handora.qc): remove it
  LOG_DEBUG("finish check row locked", K(read_trans_id), K(data_trans_id), K(sql_sequence), K(lock_state));
  return ret;
}
int ObTxTable::check_sql_sequence_can_read(const transaction::ObTransID &data_trans_id,
                                           const int64_t sql_sequence,
                                           const int64_t read_epoch,
                                           bool &can_read)
{
  CheckSqlSequenceCanReadFunctor fn(sql_sequence, can_read);
  int ret = check_with_tx_data(data_trans_id, fn, read_epoch);
  // TODO(handora.qc): remove it
  LOG_DEBUG("finish check sql sequence can read", K(data_trans_id), K(sql_sequence), K(can_read));
  return ret;
}

int ObTxTable::get_tx_state_with_log_ts(const transaction::ObTransID &data_trans_id,
                                        const int64_t log_ts,
                                        const int64_t read_epoch,
                                        int64_t &state,
                                        int64_t &trans_version)
{
  GetTxStateWithLogTSFunctor fn(log_ts, state, trans_version);
  int ret = check_with_tx_data(data_trans_id, fn, read_epoch);
  // TODO(handora.qc): remove it
  LOG_DEBUG("finish get tx state with log ts", K(data_trans_id), K(log_ts), K(state), K(trans_version));
  return ret;
}

int ObTxTable::try_get_tx_state(const transaction::ObTransID tx_id,
                                const int64_t read_epoch,
                                int64_t &state,
                                int64_t &trans_version)
{
  int ret = OB_SUCCESS;
  GetTxStateWithLogTSFunctor fn(INT64_MAX, state, trans_version);
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret), K(tx_id));
  } else {
    ret = tx_data_table_.check_with_tx_data(tx_id, fn);
    if (OB_ITER_END == ret) {
      ret = OB_TRANS_CTX_NOT_EXIST;
    }
  }

  check_state_and_epoch_(tx_id, read_epoch, false/*need_log_error*/, ret);
  return ret;
}

int ObTxTable::lock_for_read(const transaction::ObLockForReadArg &lock_for_read_arg,
                             const int64_t read_epoch,
                             bool &can_read,
                             int64_t &trans_version,
                             bool &is_determined_state,
                             const ObCleanoutOp &cleanout_op,
                             const ObReCheckOp &recheck_op)
{
  LockForReadFunctor fn(lock_for_read_arg, can_read, trans_version, is_determined_state, cleanout_op, recheck_op);
  int ret = check_with_tx_data(lock_for_read_arg.data_trans_id_, fn, read_epoch);
  // TODO(handora.qc): remove it
  LOG_DEBUG("finish lock for read", K(lock_for_read_arg), K(can_read), K(trans_version), K(is_determined_state));
  return ret;
}

int ObTxTable::get_recycle_ts(int64_t &recycle_ts)
{
  int ret = OB_SUCCESS;
  int64_t prev_epoch = ATOMIC_LOAD(&epoch_);
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    STORAGE_LOG(WARN, "this tx table is not inited", KR(ret));
  } else if (OB_FAIL(tx_data_table_.get_recycle_ts(recycle_ts))) {
    STORAGE_LOG(WARN, "get recycle ts failed", KR(ret), "ls_id", ls_->get_ls_id());
  } else if (TxTableState::ONLINE != ATOMIC_LOAD(&state_) || prev_epoch != ATOMIC_LOAD(&epoch_)) {
    recycle_ts = 0;
    ret = OB_REPLICA_NOT_READABLE;
    STORAGE_LOG(WARN, "this tx table is migrating or has migrated", KR(ret), "ls_id", ls_->get_ls_id());
  }
  return ret;
}

int ObTxTable::get_upper_trans_version_before_given_log_ts(const int64_t sstable_end_log_ts, int64_t &upper_trans_version)
{
  return tx_data_table_.get_upper_trans_version_before_given_log_ts(sstable_end_log_ts, upper_trans_version);
}

int ObTxTable::get_start_tx_scn(int64_t &start_tx_scn)
{
  return tx_data_table_.get_start_tx_scn(start_tx_scn);
}

int ObTxTable::cleanout_tx_node(const transaction::ObTransID &tx_id,
                                const int64_t read_epoch,
                                memtable::ObMvccRow &value,
                                memtable::ObMvccTransNode &tnode,
                                const bool need_row_latch)
{
  ObCleanoutTxNodeOperation op(value, tnode, need_row_latch);
  CleanoutTxStateFunctor fn(op);
  int ret = check_with_tx_data(tx_id, fn, read_epoch);
  if (OB_TRANS_CTX_NOT_EXIST == ret) {
    if (tnode.is_committed() || tnode.is_aborted()) {
      // may be the concurrent case between cleanout and commit/abort
      ret = OB_SUCCESS;
    }
  }
  return ret;
}

int ObTxTable::supplement_undo_actions_if_exist(ObTxData *&tx_data)
{
  return tx_data_table_.supplement_undo_actions_if_exist(tx_data);
}

int ObTxTable::dump_single_tx_data_2_text(const int64_t tx_id_int, const char *fname)
{
  int ret = OB_SUCCESS;
  STORAGE_LOG(INFO, "start dump single tx data");
  char real_fname[OB_MAX_FILE_NAME_LENGTH];
  FILE *fd = NULL;

  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tx table is not init.", KR(ret), K(tx_id_int));
  } else if (OB_ISNULL(fname)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "fanme is NULL");
  } else if (snprintf(real_fname, sizeof(real_fname), "%s.%ld", fname,
                      ::oceanbase::common::ObTimeUtility::current_time()) >= (int64_t)sizeof(real_fname)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "fname too long", K(fname));
  } else if (NULL == (fd = fopen(real_fname, "w"))) {
    ret = OB_IO_ERROR;
    STORAGE_LOG(WARN, "open file fail:", K(fname));
  } else {
    auto ls_id = ls_->get_ls_id().id();
    auto tenant_id = MTL_ID();
    fprintf(fd, "tenant_id=%ld ls_id=%ld\n", tenant_id, ls_id);

    if (OB_SUCC(tx_ctx_table_.dump_single_tx_data_2_text(tx_id_int, fd))) {
    } else if (OB_TRANS_CTX_NOT_EXIST == ret) {
      ret = OB_SUCCESS;
      ret = tx_data_table_.dump_single_tx_data_2_text(tx_id_int, fd);
    }
  }

  if (NULL != fd) {
    fclose(fd);
    fd = NULL;
  }
  if (OB_FAIL(ret)) {
    STORAGE_LOG(WARN, "dump single tx data fail", K(fname), KR(ret));
  }

  return ret;
}

const char *ObTxTable::get_state_string(const int64_t state) const
{
  STATIC_ASSERT(TxTableState::OFFLINE == 0, "Invalid State Enum");
  STATIC_ASSERT(TxTableState::ONLINE == 1, "Invalid State Enum");
  STATIC_ASSERT(TxTableState::PREPARE_OFFLINE == 2, "Invalid State Enum");
  STATIC_ASSERT(TxTableState::PREPARE_ONLINE == 3, "Invalid State Enum");
  STATIC_ASSERT(TxTableState::STATE_CNT == 4, "Invalid State Enum");
  const static int64_t cnt = TxTableState::STATE_CNT;
  const static char STATE_TO_CHAR[cnt][32] = {"OFFLINE", "ONLINE", "PREPARE_OFFLINE", "PREPARE_ONLINE"};
  return STATE_TO_CHAR[int(state)];
}

// *********************** ObTxTable end. ************************

} // storage
} // oceanbase
