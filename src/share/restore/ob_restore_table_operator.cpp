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

#define USING_LOG_PREFIX SHARE
#include "share/restore/ob_restore_table_operator.h"
#include <cstdint>
#include "lib/string/ob_sql_string.h"
#include "lib/utility/ob_macro_utils.h"
#include "lib/ob_define.h"
#include "lib/oblog/ob_log_module.h"
#include "lib/ob_errno.h"
#include "share/config/ob_server_config.h"
#include "share/inner_table/ob_inner_table_schema_constants.h"
#include "ob_log_archive_source.h"
#include "share/backup/ob_backup_struct.h"

using namespace oceanbase::share;
using namespace oceanbase::common;
using namespace sqlclient;
ObTenantRestoreTableOperator::ObTenantRestoreTableOperator() :
  is_inited_(false),
  user_tenant_id_(OB_INVALID_TENANT_ID),
  proxy_(NULL)
{}

int ObTenantRestoreTableOperator::init(const uint64_t user_tenant_id, ObISQLClient *proxy)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("restore table operator init twice", K(ret));
  } else if (! is_user_tenant(user_tenant_id) || OB_ISNULL(proxy)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(user_tenant_id), K(proxy));
  } else {
    user_tenant_id_ = user_tenant_id;
    proxy_ = proxy;
    is_inited_ = true;
  }
  return ret;
}

int ObTenantRestoreTableOperator::insert_source(const ObLogArchiveSourceItem &item)
{
  int ret = OB_SUCCESS;
  int64_t affected_rows = 0;
  ObDMLSqlSplicer dml;
  ObSqlString sql;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tenant restore table operator not init", K(ret));
  } else if (OB_UNLIKELY(! item.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(item));
  } else if (OB_FAIL(fill_log_archive_source_(item, dml))) {
    LOG_WARN("fill log archive source failed", K(ret), K(item));
  } else if (OB_FAIL(dml.splice_insert_update_sql(OB_ALL_LOG_ARCHIVE_SOURCE_TNAME, sql))) {
    LOG_WARN("splice insert update sql failed", K(ret), K(item));
  } else if (OB_FAIL(proxy_->write(get_exec_tenant_id_(), sql.ptr(), affected_rows))) {
    LOG_WARN("exec sql failed", K(ret), K(item), K(sql), K_(user_tenant_id));
  }
  return ret;
}

int ObTenantRestoreTableOperator::update_source_until_ts(const ObLogArchiveSourceItem &item)
{
  int ret = OB_SUCCESS;
  ObSqlString sql;
  ObDMLSqlSplicer dml;
  int64_t affected_rows = 0;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tenant restore table operator not init", K(ret));
  } else if (OB_UNLIKELY(OB_INVALID_TIMESTAMP == item.until_ts_)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(item.until_ts_));
  } else if (OB_FAIL(dml.add_pk_column(OB_STR_TENANT_ID, item.tenant_id_))) {
    LOG_WARN("failed to add column", K(ret), K(item.tenant_id_));
  } else if (OB_FAIL(dml.add_pk_column(OB_STR_LOG_ARCHIVE_SOURCE_ID, item.id_))) {
    LOG_WARN("failed to add column", K(ret), K(item.id_));
  } else if (OB_FAIL(dml.add_column(OB_STR_LOG_ARCHIVE_SOURCE_UNTIL_TS, item.until_ts_))) {
    LOG_WARN("failed to add column", K(ret), K(item.until_ts_));
  } else if (OB_FAIL(dml.splice_update_sql(OB_ALL_LOG_ARCHIVE_SOURCE_TNAME, sql))) {
    LOG_WARN("fill source until_ts failed", K(ret), K(item.id_), K(item.until_ts_));
  } else if (OB_FAIL(proxy_->write(get_exec_tenant_id_(), sql.ptr(), affected_rows))) {
    LOG_WARN("failed to exec sql", K(ret), K(sql), K_(user_tenant_id));
  }
  return ret;
}

int ObTenantRestoreTableOperator::delete_source()
{
  int ret = OB_SUCCESS;
  ObSqlString sql;
  int64_t affected_rows = 0;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tenant restore table operator not init", K(ret));
  } else if (OB_FAIL(sql.append_fmt("delete from %s", OB_ALL_LOG_ARCHIVE_SOURCE_TNAME))) {
    LOG_WARN("sql append failed", K(ret));
  } else if (OB_FAIL(proxy_->write(get_exec_tenant_id_(), sql.ptr(), affected_rows))) {
    LOG_WARN("failed to exec sql", K(ret), K(sql), K_(user_tenant_id));
  }
  return ret;
}

int ObTenantRestoreTableOperator::get_source(ObLogArchiveSourceItem &item)
{
  int ret = OB_SUCCESS;
  ObSqlString sql;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("tenant restore table operator not init", K(ret));
  } else {
    SMART_VAR(common::ObMySQLProxy::MySQLResult, res) {
      ObMySQLResult *result = NULL;
      if (OB_FAIL(fill_select_source_(sql))) {
        LOG_WARN("fill get source sql failed", K(ret));
      } else if (OB_FAIL(sql.append_fmt(" where %s=%ld and %s=%ld",
              OB_STR_TENANT_ID, item.tenant_id_,
              OB_STR_LOG_ARCHIVE_SOURCE_ID, item.id_))) {
        LOG_WARN("sql append failed", K(ret));
      } else if (OB_FAIL(proxy_->read(res, get_exec_tenant_id_(), sql.ptr()))) {
        LOG_WARN("sql read failed", K(ret), K(sql));
      } else if (OB_ISNULL(result = res.get_result())) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("result is NULL", K(ret));
      } else if (OB_FAIL(parse_log_archive_source_(*result, item))) {
        LOG_WARN("parse log archive source failed", K(ret));
      }
    }
  }
  return ret;
}

uint64_t ObTenantRestoreTableOperator::get_exec_tenant_id_() const
{
  return gen_meta_tenant_id(user_tenant_id_);
}

int ObTenantRestoreTableOperator::fill_log_archive_source_(const ObLogArchiveSourceItem &item,
    ObDMLSqlSplicer &dml)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(dml.add_pk_column(OB_STR_TENANT_ID, item.tenant_id_))) {
    LOG_WARN("failed to add column", K(ret), K(item));
  } else if (OB_FAIL(dml.add_pk_column(OB_STR_LOG_ARCHIVE_SOURCE_ID, item.id_))) {
    LOG_WARN("failed to add column", K(ret), K(item));
  } else if (OB_FAIL(dml.add_column(OB_STR_LOG_ARCHIVE_SOURCE_TYPE,
          ObLogArchiveSourceItem::get_source_type_str(item.type_)))) {
    LOG_WARN("failed to add column", K(ret), K(item));
  } else if (OB_FAIL(dml.add_column(OB_STR_LOG_ARCHIVE_SOURCE_VALUE, item.value_.ptr()))) {
    LOG_WARN("failed to add column", K(ret), K(item));
  } else if (OB_FAIL(dml.add_column(OB_STR_LOG_ARCHIVE_SOURCE_UNTIL_TS, item.until_ts_))) {
    LOG_WARN("failed to add column", K(ret), K(item));
  }
  return ret;
}

int ObTenantRestoreTableOperator::fill_select_source_(common::ObSqlString &sql)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(sql.append_fmt("select %s", OB_STR_TENANT_ID))) {
    LOG_WARN("sql append failed", K(ret));
  } else if (OB_FAIL(sql.append_fmt(", %s", OB_STR_LOG_ARCHIVE_SOURCE_ID))) {
    LOG_WARN("sql append failed", K(ret));
  } else if  (OB_FAIL(sql.append_fmt(", %s", OB_STR_LOG_ARCHIVE_SOURCE_TYPE))) {
    LOG_WARN("sql append failed", K(ret));
  } else if  (OB_FAIL(sql.append_fmt(", %s", OB_STR_LOG_ARCHIVE_SOURCE_VALUE))) {
    LOG_WARN("sql append failed", K(ret));
  } else if  (OB_FAIL(sql.append_fmt(", %s", OB_STR_LOG_ARCHIVE_SOURCE_UNTIL_TS))) {
    LOG_WARN("sql append failed", K(ret));
  } else if (OB_FAIL(sql.append_fmt(" from %s", OB_ALL_LOG_ARCHIVE_SOURCE_TNAME))) {
    LOG_WARN("sql append failed", K(ret));
  }
  return ret;
}

int ObTenantRestoreTableOperator::parse_log_archive_source_(ObMySQLResult &result, ObLogArchiveSourceItem &item)
{
  int ret = OB_SUCCESS;
  ObString type;
  ObLogArchiveSourceItem item_local;
  EXTRACT_INT_FIELD_MYSQL(result, OB_STR_TENANT_ID, item_local.tenant_id_, uint64_t);
  EXTRACT_INT_FIELD_MYSQL(result, OB_STR_LOG_ARCHIVE_SOURCE_ID, item_local.id_, int64_t);
  EXTRACT_VARCHAR_FIELD_MYSQL_SKIP_RET(result, OB_STR_LOG_ARCHIVE_SOURCE_TYPE, type);
  if (OB_SUCC(ret)) {
    item_local.type_ = ObLogArchiveSourceItem::get_source_type(type);
  }
  EXTRACT_VARCHAR_FIELD_MYSQL_SKIP_RET(result, OB_STR_LOG_ARCHIVE_SOURCE_VALUE, item_local.value_);
  EXTRACT_INT_FIELD_MYSQL(result, OB_STR_LOG_ARCHIVE_SOURCE_UNTIL_TS, item_local.until_ts_, int64_t);
  OZ (item.deep_copy(item_local));
  return ret;
}
