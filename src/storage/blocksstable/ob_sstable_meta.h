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

#ifndef OCEANBASE_STORAGE_BLOCKSSTABLE_OB_SSTABLE_META_H
#define OCEANBASE_STORAGE_BLOCKSSTABLE_OB_SSTABLE_META_H

#include "share/schema/ob_table_schema.h"
#include "storage/ob_storage_schema.h"
#include "storage/ob_i_table.h"
#include "storage/blocksstable/ob_sstable_meta_info.h"

namespace oceanbase
{
namespace storage
{
class ObStoreRow;
struct ObTabletCreateSSTableParam;
}
namespace blocksstable
{
//For compatibility, the variables in this struct MUST NOT be deleted or moved.
//You should ONLY add variables at the end.
//Note that if you use complex structure as variables, the complex structure should also keep compatibility.
struct ObSSTableBasicMeta final
{
public:
  static const int32_t SSTABLE_BASIC_META_VERSION = 1;
  static const int64_t SSTABLE_FORMAT_VERSION_1 = 1;

  ObSSTableBasicMeta();
  ~ObSSTableBasicMeta() = default;
  bool operator==(const ObSSTableBasicMeta &other) const;
  bool operator!=(const ObSSTableBasicMeta &other) const;
  bool is_valid() const;
  void reset();

  OB_INLINE int64_t get_total_macro_block_count() const
  {
    return data_macro_block_count_ + index_macro_block_count_;
  }
  OB_INLINE int64_t get_data_macro_block_count() const { return data_macro_block_count_; }
  OB_INLINE int64_t get_data_micro_block_count() const { return data_micro_block_count_; }
  OB_INLINE int64_t get_index_macro_block_count() const { return index_macro_block_count_; }
  OB_INLINE int64_t get_total_use_old_macro_block_count() const
  {
    return use_old_macro_block_count_;
  }
  OB_INLINE int64_t get_upper_trans_version() const { return ATOMIC_LOAD(&upper_trans_version_); }
  OB_INLINE int64_t get_max_merged_trans_version() const { return max_merged_trans_version_; }
  OB_INLINE int64_t get_ddl_log_ts() const { return ddl_log_ts_; }
  OB_INLINE int64_t get_create_snapshot_version() const { return create_snapshot_version_; }
  OB_INLINE int64_t get_filled_tx_log_ts() const { return filled_tx_log_ts_; }
  OB_INLINE int16_t get_data_index_tree_height() const { return data_index_tree_height_; }

  int set_upper_trans_version(const int64_t upper_trans_version);
  int64_t get_recycle_version() const;
  int serialize(char *buf, const int64_t buf_len, int64_t &pos) const;
  int deserialize(const char *buf, const int64_t data_len, int64_t& pos);
  int64_t get_serialize_size() const;
public:
  TO_STRING_KV(K_(version), K_(length), K(row_count_), K(occupy_size_), K(original_size_),
      K(data_checksum_), K(index_type_), K(rowkey_column_count_), K(column_cnt_),
      K(data_macro_block_count_), K(data_micro_block_count_), K(use_old_macro_block_count_),
      K(index_macro_block_count_), K(sstable_format_version_), K(schema_version_),
      K(create_snapshot_version_), K(progressive_merge_round_),
      K(progressive_merge_step_), K(data_index_tree_height_), K(table_mode_),
      K(upper_trans_version_), K(max_merged_trans_version_), K_(recycle_version),
      K(ddl_log_ts_), K(filled_tx_log_ts_),
      K(contain_uncommitted_row_), K(status_), K_(row_store_type), K_(compressor_type),
      K_(encrypt_id), K_(master_key_id), KPHEX_(encrypt_key, sizeof(encrypt_key_)));

public:
  int32_t version_;
  int32_t length_;
  int64_t row_count_;
  int64_t occupy_size_;
  int64_t original_size_;
  int64_t data_checksum_;
  int64_t index_type_;
  int64_t rowkey_column_count_; //rowkey for sstable, including multi-version columns if needed
  int64_t column_cnt_;
  int64_t data_macro_block_count_;
  int64_t data_micro_block_count_;
  int64_t use_old_macro_block_count_;
  int64_t index_macro_block_count_;
  int64_t sstable_format_version_;
  int64_t schema_version_;
  int64_t create_snapshot_version_;
  int64_t progressive_merge_round_;
  int64_t progressive_merge_step_;
  int64_t upper_trans_version_;
  // major/buf minor: snapshot version; others: max commit version
  int64_t max_merged_trans_version_;
  // recycle_version only avaliable for minor sstable, recored recycled multi version start
  int64_t recycle_version_;
  int64_t ddl_log_ts_; // only used in DDL SSTable, all MB in DDL SSTable should have the same log_ts(start_log_ts)
  int64_t filled_tx_log_ts_; // only for rebuild
  int16_t data_index_tree_height_;
  share::schema::ObTableMode table_mode_;
  uint8_t status_;
  bool contain_uncommitted_row_;
  common::ObRowStoreType row_store_type_;
  common::ObCompressorType compressor_type_;
  int64_t encrypt_id_;
  int64_t master_key_id_;
  char encrypt_key_[share::OB_MAX_TABLESPACE_ENCRYPT_KEY_LENGTH];
  //Add new variable need consider ObSSTableMetaChecker
};

class ObSSTableMeta final
{
public:
  ObSSTableMeta();
  ~ObSSTableMeta();
  int init(const storage::ObTabletCreateSSTableParam &param, common::ObIAllocator *allocator);
  void reset();
  OB_INLINE bool is_valid() const { return is_inited_; }
  OB_INLINE bool contain_uncommitted_row() const { return basic_meta_.contain_uncommitted_row_; }
  OB_INLINE bool is_empty() const { return 0 == basic_meta_.data_macro_block_count_; }
  OB_INLINE ObSSTableBasicMeta &get_basic_meta() { return basic_meta_; }
  OB_INLINE const ObSSTableBasicMeta &get_basic_meta() const { return basic_meta_; }
  OB_INLINE const common::ObIArray<int64_t> &get_col_checksum() const { return column_checksums_; }
  OB_INLINE int64_t get_row_count() const { return basic_meta_.row_count_; }
  OB_INLINE int64_t get_rowkey_column_count() const { return basic_meta_.rowkey_column_count_; }
  OB_INLINE int64_t get_column_count() const { return basic_meta_.column_cnt_; }
  OB_INLINE int64_t get_schema_rowkey_column_count() const
  {
    return basic_meta_.rowkey_column_count_ - ObMultiVersionRowkeyHelpper::get_extra_rowkey_col_cnt();
  }
  OB_INLINE int64_t get_schema_column_count() const
  {
    return basic_meta_.column_cnt_ - ObMultiVersionRowkeyHelpper::get_extra_rowkey_col_cnt();
  }

  OB_INLINE int16_t get_index_tree_height() const { return basic_meta_.data_index_tree_height_; }
  OB_INLINE const ObRootBlockInfo &get_root_info() const { return data_root_info_; }
  OB_INLINE const ObSSTableMacroInfo &get_macro_info() const { return macro_info_; }
  int get_index_tree_root(
      const ObTableReadInfo &index_read_info,
      blocksstable::ObMicroBlockData &index_data,
      const bool need_transform);
  int load_root_block_data(); //TODO:@jinzhu remove me after using kv cache.
  int serialize(char *buf, const int64_t buf_len, int64_t &pos) const;
  int deserialize(
      common::ObIAllocator *allocator,
      const char *buf,
      const int64_t data_len,
      int64_t &pos);
  int64_t get_serialize_size() const;
  TO_STRING_KV(K_(basic_meta), K_(column_checksums), K_(data_root_info), K_(macro_info), KP_(allocator));
private:
  bool check_meta() const;
  int init_base_meta(const ObTabletCreateSSTableParam &param, common::ObIAllocator *allocator);
  int init_data_index_tree_info(const storage::ObTabletCreateSSTableParam &param);
  int prepare_column_checksum(const common::ObIArray<int64_t> &column_checksums);
  int transform_root_block_data(const ObTableReadInfo &read_info);
  int serialize_(char *buf, const int64_t buf_len, int64_t &pos) const;
  int deserialize_(
      common::ObIAllocator *allocator,
      const char *buf,
      const int64_t data_len,
      int64_t &pos);
  int64_t get_serialize_size_() const;
private:
  friend class ObSSTable;
  static const int64_t SSTABLE_META_VERSION = 1;
  typedef common::ObFixedArray<int64_t, common::ObIAllocator> ColChecksumArray;
private:
  ObSSTableBasicMeta basic_meta_;
  ColChecksumArray column_checksums_;
  ObRootBlockInfo data_root_info_;
  ObSSTableMacroInfo macro_info_;
  // The following fields don't to persist
  common::ObIAllocator *allocator_;
  common::TCRWLock lock_;
  bool is_inited_;
  DISALLOW_COPY_AND_ASSIGN(ObSSTableMeta);
};

class ObMigrationSSTableParam final
{
public:
  ObMigrationSSTableParam();
  ~ObMigrationSSTableParam();
  bool is_valid() const;
  void reset();
  int assign(const ObMigrationSSTableParam &param);
  TO_STRING_KV(K_(basic_meta), K_(column_checksums), K_(column_default_checksums), K_(table_key));
private:
  static const int64_t MIGRATION_SSTABLE_PARAM_VERSION = 1;
  typedef common::ObSEArray<int64_t, common::OB_ROW_DEFAULT_COLUMNS_COUNT> ColChecksumArray;
public:
  ObSSTableBasicMeta basic_meta_;
  ColChecksumArray column_checksums_;
  storage::ObITable::TableKey table_key_;
  ColChecksumArray column_default_checksums_;
  OB_UNIS_VERSION(MIGRATION_SSTABLE_PARAM_VERSION);
private:
  DISALLOW_COPY_AND_ASSIGN(ObMigrationSSTableParam);
};

class ObSSTableMetaChecker
{
public:
  static int check_sstable_meta(
      const ObSSTableMeta &old_sstable_meta,
      const ObSSTableMeta &new_sstable_meta);
  static int check_sstable_meta(
      const ObMigrationSSTableParam &migration_param,
      const ObSSTableMeta &new_sstable_meta);
private:
  static int check_sstable_basic_meta_(
      const ObSSTableBasicMeta &old_sstable_basic_meta,
      const ObSSTableBasicMeta &new_sstable_basic_meta);
  static int check_sstable_column_checksum_(
      const common::ObIArray<int64_t> &old_column_checksum,
      const common::ObIArray<int64_t> &new_column_checksum);
};


} // namespace blocksstable
} // namespace oceanbase

#endif /* OCEANBASE_STORAGE_BLOCKSSTABLE_OB_SSTABLE_META_H */
