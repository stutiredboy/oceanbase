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

#ifndef SRC_SQL_ENGINE_EXPR_OB_EXPR_UUID_H_
#define SRC_SQL_ENGINE_EXPR_OB_EXPR_UUID_H_

#include "sql/engine/expr/ob_expr_operator.h"

namespace oceanbase
{
namespace sql
{

class ObBigEndian
{
public:
  static int put_uint16(unsigned char *b, uint16_t v);
  static int put_uint32(unsigned char *b, uint32_t v);
private :
  //disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObBigEndian);
};

struct ObUUIDNode
{
  ObUUIDNode();
  int init();
  unsigned char mac_addr_[6];
  bool is_inited_;
};

class ObUUIDTime
{
public:
  static int get_time(uint64_t &time, uint16_t &seq);
private:
  static int time_now(uint64_t &now);
  static void reset_clock_seq();
  static const uint64_t LILLIAN = 2299160; // Julian day of 15 Oct 1582
  static const uint64_t UNIXX = 2440587; // Julian day of 1 Jan 1970
  static const uint64_t EPOCH = UNIXX - LILLIAN; // Days between EPOCHs
  static const uint64_t G1582 = EPOCH * 86400; // seconds between EPOCHs
  static const uint64_t G1582NS100 = G1582 * 10000000; // 100s of a nanoseconds between EPOCHs

  static uint64_t lasttime_;
  static uint16_t clock_seq_;
  static common::ObLatch lock_;
};


class ObExprUuid : public ObFuncExprOperator
{
public:
  explicit ObExprUuid(common::ObIAllocator &alloc);
  explicit ObExprUuid(
    common::ObIAllocator &alloc,
    ObExprOperatorType type,
    const char *name,
    int32_t param_num,
    int32_t dimension);
  virtual ~ObExprUuid();
  virtual int calc_result_type0(ObExprResType &type,
                                common::ObExprTypeCtx &type_ctx) const;
  static int init();
  static int eval_uuid(const ObExpr &expr, ObEvalCtx &ctx, ObDatum &expr_datum);
  //used to generate uuid for server_uuid system variable.
  static int gen_server_uuid(char *server_uuid, const int64_t uuid_len);
  virtual int cg_expr(ObExprCGCtx &op_cg_ctx,
                      const ObRawExpr &raw_expr,
                      ObExpr &rt_expr) const override;
  //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
  static const common::ObLength LENGTH_UUID = 36;//chars not bytes
protected:
  static int calc(unsigned char *scratch);
private:
  static ObUUIDNode *uuid_node;
  //disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObExprUuid);
};

inline int ObExprUuid::calc_result_type0(ObExprResType &type,
                                         common::ObExprTypeCtx &type_ctx) const
{
  UNUSED(type_ctx);
  type.set_varchar();
  type.set_collation_level(common::CS_LEVEL_IMPLICIT);
  type.set_collation_type(common::ObCharset::get_default_collation(common::ObCharset::get_default_charset()));
  type.set_length(ObExprUuid::LENGTH_UUID);
  return common::OB_SUCCESS;
}

class ObExprSysGuid : public ObExprUuid
{
public:
  explicit ObExprSysGuid(common::ObIAllocator &alloc);
  virtual ~ObExprSysGuid();
  virtual int calc_result_type0(
    ObExprResType &type, common::ObExprTypeCtx &type_ctx) const;

  static int eval_sys_guid(const ObExpr &expr, ObEvalCtx &ctx, ObDatum &expr_datum);
  virtual int cg_expr(ObExprCGCtx &,
                      const ObRawExpr &,
                      ObExpr &rt_expr) const override
  {
    rt_expr.eval_func_ = eval_sys_guid;
    return common::OB_SUCCESS;
  }
private:
  static const common::ObLength LENGTH_SYS_GUID = 16;
private:
  DISALLOW_COPY_AND_ASSIGN(ObExprSysGuid);
};

inline int ObExprSysGuid::calc_result_type0(
  ObExprResType &type, common::ObExprTypeCtx &type_ctx) const
{
  UNUSED(type_ctx);
  type.set_raw();
  type.set_length(LENGTH_SYS_GUID);
  return common::OB_SUCCESS;
}

}
}


#endif /* SRC_SQL_ENGINE_EXPR_OB_EXPR_UUID_H_ */
