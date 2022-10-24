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

#include "sql/engine/expr/ob_expr_greater_than.h"
#include "ob_expr_test_utils.h"
using namespace oceanbase::sql;
using namespace oceanbase::common;

#include <gtest/gtest.h>
class ObExprGreaterThanTest: public ::testing::Test
{
  public:
    ObExprGreaterThanTest();
    virtual ~ObExprGreaterThanTest();
    virtual void SetUp();
    virtual void TearDown();
  private:
    // disallow copy
    ObExprGreaterThanTest(const ObExprGreaterThanTest &other);
    ObExprGreaterThanTest& operator=(const ObExprGreaterThanTest &other);
  protected:
    // data members
};

ObExprGreaterThanTest::ObExprGreaterThanTest()
{
}

ObExprGreaterThanTest::~ObExprGreaterThanTest()
{
}

void ObExprGreaterThanTest::SetUp()
{
}

void ObExprGreaterThanTest::TearDown()
{
}

#define T(t1, v1, t2, v2, res) COMPARE_EXPECT(ObExprGreaterThan, &buf, calc_result2, t1, v1, t2, v2, res)
#define T_BIN(t1, v1, t2, v2, res) COMPARE_EXPECT_BIN(ObExprGreaterThan, &buf, calc_result2, t1, v1, t2, v2, res)
#define T_GEN(t1, v1, t2, v2, res) COMPARE_EXPECT_GEN(ObExprGreaterThan, &buf, calc_result2, t1, v1, t2, v2, res)

TEST_F(ObExprGreaterThanTest, collation_test)
{
  ObMalloc buf;
  T_BIN(varchar, "ß", varchar, "s", MY_TRUE);
  T_GEN(varchar, "ß", varchar, "s", MY_FALSE);
}

/*
TEST_F(ObExprGreaterThanTest, basic_test)
{
  ObExprGreaterThan gt;
  ASSERT_EQ(2, gt.get_param_num());
  ObMalloc buf;
  // special vs special
  T(null, , null, , MY_NULL);
  T(null, , min, , MY_NULL);
  T(min, , null, , MY_NULL);
  T(null, , max, , MY_NULL);
  T(max, , null, , MY_NULL);
  T(min, , max, , MY_FALSE);
  T(min, , min, , MY_FALSE);
  T(max, , min, , MY_TRUE);
  T(max, , max, , MY_FALSE);
  // int vs int
  T(int, 1, int, 2, MY_FALSE);
  T(int, 1, int, 1, MY_FALSE);
  T(int, 2, int, 1, MY_TRUE);
  // int vs special
  T(int, 1, null, , MY_NULL);
  T(null, , int, 1, MY_NULL);
  T(int, 1, min, , MY_TRUE);
  T(min, , int, 1, MY_FALSE);
  T(int, 1, max, , MY_FALSE);
  T(max, , int, 1, MY_TRUE);
  // varchar vs varchar
  T(varchar, "", varchar, "", MY_FALSE);
  T(varchar, "", varchar, "a", MY_FALSE);
  T(varchar, "a", varchar, "", MY_TRUE);
  T(varchar, "a", varchar, "a", MY_FALSE);
  T(varchar, "a", varchar, "b", MY_FALSE);
  T(varchar, "a", varchar, "aa", MY_FALSE);
  T(varchar, "aa", varchar, "aa", MY_FALSE);
  // varchar vs special
  T(varchar, "", null, , MY_NULL);
  T(null, , varchar, "", MY_NULL);
  T(varchar, "", min, , MY_TRUE);
  T(min, , varchar, "", MY_FALSE);
  T(varchar, "", max, , MY_FALSE);
  T(max, , varchar, "", MY_TRUE);
  // float vs float
  T(float, 1.0, float, 2.0, MY_FALSE);
  T(float, 1.0, float, 1.0, MY_FALSE);
  T(float, 2.0, float, 1.0, MY_TRUE);
  // float vs special
  T(float, 1.0, null, , MY_NULL);
  T(null, , float, 1.0, MY_NULL);
  T(float, 1.0, min, , MY_TRUE);
  T(min, , float, 1.0, MY_FALSE);
  T(float, 1.0, max, , MY_FALSE);
  T(max, , float, 1.0, MY_TRUE);
  // double vs double
  T(double, 1.0, double, 2.0, MY_FALSE);
  T(double, 1.0, double, 1.0, MY_FALSE);
  T(double, 2.0, double, 1.0, MY_TRUE);
  // double vs special
  T(double, 1.0, null, , MY_NULL);
  T(null, , double, 1.0, MY_NULL);
  T(double, 1.0, min, , MY_TRUE);
  T(min, , double, 1.0, MY_FALSE);
  T(double, 1.0, max, , MY_FALSE);
  T(max, , double, 1.0, MY_TRUE);
  // precise_datetime vs precise_datetime
  T(precise_datetime, 1, precise_datetime, 2, MY_FALSE);
  T(precise_datetime, 1, precise_datetime, 1, MY_FALSE);
  T(precise_datetime, 2, precise_datetime, 1, MY_TRUE);
  // precise_datetime vs special
  T(precise_datetime, 1, null, , MY_NULL);
  T(null, , precise_datetime, 1, MY_NULL);
  T(precise_datetime, 1, min, , MY_TRUE);
  T(min, , precise_datetime, 1, MY_FALSE);
  T(precise_datetime, 1, max, , MY_FALSE);
  T(max, , precise_datetime, 1, MY_TRUE);
  // ctime vs ctime
  T(ctime, 1, ctime, 2, MY_FALSE);
  T(ctime, 1, ctime, 1, MY_FALSE);
  T(ctime, 2, ctime, 1, MY_TRUE);
  // ctime vs special
  T(ctime, 1, null, , MY_NULL);
  T(null, , ctime, 1, MY_NULL);
  T(ctime, 1, min, , MY_TRUE);
  T(min, , ctime, 1, MY_FALSE);
  T(ctime, 1, max, , MY_FALSE);
  T(max, , ctime, 1, MY_TRUE);
  // mtime vs mtime
  T(mtime, 1, mtime, 2, MY_FALSE);
  T(mtime, 1, mtime, 1, MY_FALSE);
  T(mtime, 2, mtime, 1, MY_TRUE);
  // mtime vs special
  T(mtime, 1, null, , MY_NULL);
  T(null, , mtime, 1, MY_NULL);
  T(mtime, 1, min, , MY_TRUE);
  T(min, , mtime, 1, MY_FALSE);
  T(mtime, 1, max, , MY_FALSE);
  T(max, , mtime, 1, MY_TRUE);
  // bool vs bool
  T(bool, true, bool, true, MY_FALSE);
  T(bool, true, bool, false, MY_TRUE);
  T(bool, false, bool, false, MY_FALSE);
  T(bool, false, bool, true, MY_FALSE);
  // bool vs special
  T(bool, true, null, , MY_NULL);
  T(null, , bool, true, MY_NULL);
  T(bool, true, min, , MY_TRUE);
  T(min, , bool, true, MY_FALSE);
  T(bool, true, max, , MY_FALSE);
  T(max, , bool, true, MY_TRUE);
}

TEST_F(ObExprGreaterThanTest, promotion_test)
{
  ObMalloc buf;
  // int vs float
  T(int, 1, float, 2.0, MY_FALSE);
  T(int, 1, float, 1.0, MY_FALSE);
  T(int, 1, float, 0.0, MY_TRUE);
  T(float, 1.0, int, 2, MY_FALSE);
  T(float, 1.0, int, 1, MY_FALSE);
  T(float, 1.0, int, 0, MY_TRUE);
  // int vs double
  T(int, 1, double, 2.0, MY_FALSE);
  T(int, 1, double, 1.0, MY_FALSE);
  T(int, 1, double, 0.0, MY_TRUE);
  T(double, 1.0, int, 2, MY_FALSE);
  T(double, 1.0, int, 1, MY_FALSE);
  T(double, 1.0, int, 0, MY_TRUE);
  // int vs pdatetime
  T(int, 1, precise_datetime, 2, MY_ERROR);
  T(int, 1, precise_datetime, 1, MY_ERROR);
  T(int, 1, precise_datetime, 0, MY_ERROR);
  T(precise_datetime, 1, int, 2, MY_ERROR);
  T(precise_datetime, 1, int, 1, MY_ERROR);
  T(precise_datetime, 1, int, 0, MY_ERROR);
  // int vs varchar
  T(int, 1, varchar, "2", MY_FALSE);
  T(int, 1, varchar, "1", MY_FALSE);
  T(int, 1, varchar, "0", MY_TRUE);
  T(varchar, "1", int, 2, MY_FALSE);
  T(varchar, "1", int, 1, MY_FALSE);
  T(varchar, "1", int, 0, MY_TRUE);
  //T(int, 1, varchar, "2ab", MY_FALSE);
  //T(int, 1, varchar, "1ab", MY_FALSE);
  //T(int, 1, varchar, "0ab", MY_TRUE);
  //T(int, 1, varchar, "ab", MY_TRUE);
  //T(int, 0, varchar, "ab", MY_FALSE);
  //T(int, -1, varchar, "ab", MY_FALSE);
  // int vs bool
  T(int, 1, bool, false, MY_ERROR);
  T(int, -1, bool, false, MY_ERROR);
  T(int, 0, bool, false, MY_ERROR);
  T(int, 1, bool, true, MY_ERROR);
  T(int, -1, bool, true, MY_ERROR);
  T(int, 0, bool, true, MY_ERROR);
  // float vs double
  T(float, 1.0, double, 2.0, MY_FALSE);
  T(float, 1.0, double, 1.0, MY_FALSE);
  T(float, 2.0, double, 1.0, MY_TRUE);
  T(double, 1.0, float, 2.0, MY_FALSE);
  T(double, 1.0, float, 1.0, MY_FALSE);
  T(double, 2.0, float, 1.0, MY_TRUE);
  // float vs pdatetime
  T(float, 1.0, precise_datetime, 2, MY_ERROR);
  T(float, 1.0, precise_datetime, 1, MY_ERROR);
  T(float, 1.0, precise_datetime, 0, MY_ERROR);
  T(precise_datetime, 1, float, 2.0, MY_ERROR);
  T(precise_datetime, 1, float, 1.0, MY_ERROR);
  T(precise_datetime, 1, float, 0.0, MY_ERROR);
  // float vs varchar
  T(float, 1.0, varchar, "2.0", MY_FALSE);
  T(float, 1.0, varchar, "1.0", MY_FALSE);
  T(float, 1.0, varchar, "0.0", MY_TRUE);
  T(varchar, "1.0", float, 2.0, MY_FALSE);
  T(varchar, "1.0", float, 1.0, MY_FALSE);
  T(varchar, "1.0", float, 0.0, MY_TRUE);
  //T(float, 1.0, varchar, "2.0ab", MY_FALSE);
  //T(float, 1.0, varchar, "1.0ab", MY_FALSE);
  //T(float, 1.0, varchar, "0.0ab", MY_TRUE);
  //T(float, 1.0, varchar, "ab", MY_TRUE);
  //T(float, 0.0, varchar, "ab", MY_FALSE);
  //T(float, -1.0, varchar, "ab", MY_FALSE);
  // float vs bool
  T(float, 1.0, bool, false, MY_ERROR);
  T(float, -1.0, bool, false, MY_ERROR);
  T(float, 0.0, bool, false, MY_ERROR);
  T(float, 1.0, bool, true, MY_ERROR);
  T(float, -1.0, bool, true, MY_ERROR);
  T(float, 0.0, bool, true, MY_ERROR);
  // double vs pdatetime
  T(double, 1.0, precise_datetime, 2, MY_ERROR);
  T(double, 1.0, precise_datetime, 1, MY_ERROR);
  T(double, 1.0, precise_datetime, 0, MY_ERROR);
  T(precise_datetime, 1, double, 2.0, MY_ERROR);
  T(precise_datetime, 1, double, 1.0, MY_ERROR);
  T(precise_datetime, 1, double, 0.0, MY_ERROR);
  // double vs varchar
  T(double, 1.0, varchar, "2.0", MY_FALSE);
  T(double, 1.0, varchar, "1.0", MY_FALSE);
  T(double, 1.0, varchar, "0.0", MY_TRUE);
  T(varchar, "1.0", double, 2.0, MY_FALSE);
  T(varchar, "1.0", double, 1.0, MY_FALSE);
  T(varchar, "1.0", double, 0.0, MY_TRUE);
  //T(double, 1.0, varchar, "2.0ab", MY_FALSE);
  //T(double, 1.0, varchar, "1.0ab", MY_FALSE);
  //T(double, 1.0, varchar, "0.0ab", MY_TRUE);
  //T(double, 1.0, varchar, "ab", MY_TRUE);
  //T(double, 0.0, varchar, "ab", MY_FALSE);
  //T(double, -1.0, varchar, "ab", MY_FALSE);
  // double vs bool
  T(double, 1.0, bool, false, MY_ERROR);
  T(double, -1.0, bool, false, MY_ERROR);
  T(double, 0.0, bool, false, MY_ERROR);
  T(double, 1.0, bool, true, MY_ERROR);
  T(double, -1.0, bool, true, MY_ERROR);
  T(double, 0.0, bool, true, MY_ERROR);
  // pdatetime vs varchar
  T(pdatetime, "2013-12-16 19:26:30", varchar, "2013-12-16 19:26:30.1", MY_FALSE);
  T(pdatetime, "2013-12-16 19:26:30", varchar, "2013-12-16 19:26:30", MY_FALSE);
  T(pdatetime, "2013-12-16 19:26:30", varchar, "2013-12-16 19:26:29", MY_TRUE);
  T(varchar, "2013-12-16 19:26:30", pdatetime, "2013-12-16 19:26:30.1", MY_FALSE);
  T(varchar, "2013-12-16 19:26:30", pdatetime, "2013-12-16 19:26:30", MY_FALSE);
  T(varchar, "2013-12-16 19:26:30", pdatetime, "2013-12-16 19:26:29", MY_TRUE);
  // pdatetime vs bool
  T(precise_datetime, 1, bool, false, MY_ERROR);
  T(precise_datetime, -1, bool, false, MY_ERROR);
  T(precise_datetime, 0, bool, false, MY_ERROR);
  T(precise_datetime, 1, bool, true, MY_ERROR);
  T(precise_datetime, -1, bool, true, MY_ERROR);
  T(precise_datetime, 0, bool, true, MY_ERROR);
  // varchar vs bool
  T(varchar, "true", bool, false, MY_TRUE);
  T(varchar, "false", bool, false, MY_FALSE);
  T(varchar, "kaka", bool, false, MY_ERROR);
  T(varchar, "true", bool, true, MY_FALSE);
  T(varchar, "false", bool, true, MY_FALSE);
  T(varchar, "kaka", bool, true, MY_ERROR);
  T(bool, false, varchar, "true", MY_FALSE);
  T(bool, false, varchar, "false", MY_FALSE);
  T(bool, false, varchar, "kaka", MY_ERROR);
  T(bool, true, varchar, "true", MY_FALSE);
  T(bool, true, varchar, "false", MY_TRUE);
  T(bool, true, varchar, "kaka", MY_ERROR);
}

#define R(t1, v1, t2, v2, res) ROW1_COMPARE_EXPECT(ObExprGreaterThan, &buf, calc, t1, v1, t2, v2, res)
TEST_F(ObExprGreaterThanTest, row1_basic_test)
{
  ObExprGreaterThan gt;
  ASSERT_EQ(2, gt.get_param_num());
  ObMalloc buf;
  // special vs special
  R(null, , null, , MY_FALSE);
  R(null, , min, , MY_TRUE);
  R(min, , null, , MY_FALSE);
  R(null, , max, , MY_FALSE);
  R(max, , null, , MY_TRUE);
  R(min, , max, , MY_FALSE);
  R(min, , min, , MY_FALSE);
  R(max, , min, , MY_TRUE);
  R(max, , max, , MY_FALSE);
  // int vs int
  R(int, 1, int, 2, MY_FALSE);
  R(int, 1, int, 1, MY_FALSE);
  R(int, 2, int, 1, MY_TRUE);
  // int vs special
  R(int, 1, null, , MY_TRUE);
  R(null, , int, 1, MY_FALSE);
  R(int, 1, min, , MY_TRUE);
  R(min, , int, 1, MY_FALSE);
  R(int, 1, max, , MY_FALSE);
  R(max, , int, 1, MY_TRUE);
  // varchar vs varchar
  R(varchar, "", varchar, "", MY_FALSE);
  R(varchar, "", varchar, "a", MY_FALSE);
  R(varchar, "a", varchar, "", MY_TRUE);
  R(varchar, "a", varchar, "a", MY_FALSE);
  R(varchar, "a", varchar, "b", MY_FALSE);
  R(varchar, "a", varchar, "aa", MY_FALSE);
  R(varchar, "aa", varchar, "aa", MY_FALSE);
  // varchar vs special
  R(varchar, "", null, , MY_TRUE);
  R(null, , varchar, "", MY_FALSE);
  R(varchar, "", min, , MY_TRUE);
  R(min, , varchar, "", MY_FALSE);
  R(varchar, "", max, , MY_FALSE);
  R(max, , varchar, "", MY_TRUE);
  // float vs float
  R(float, 1.0, float, 2.0, MY_FALSE);
  R(float, 1.0, float, 1.0, MY_FALSE);
  R(float, 2.0, float, 1.0, MY_TRUE);
  // float vs special
  R(float, 1.0, null, , MY_TRUE);
  R(null, , float, 1.0, MY_FALSE);
  R(float, 1.0, min, , MY_TRUE);
  R(min, , float, 1.0, MY_FALSE);
  R(float, 1.0, max, , MY_FALSE);
  R(max, , float, 1.0, MY_TRUE);
  // double vs double
  R(double, 1.0, double, 2.0, MY_FALSE);
  R(double, 1.0, double, 1.0, MY_FALSE);
  R(double, 2.0, double, 1.0, MY_TRUE);
  // double vs special
  R(double, 1.0, null, , MY_TRUE);
  R(null, , double, 1.0, MY_FALSE);
  R(double, 1.0, min, , MY_TRUE);
  R(min, , double, 1.0, MY_FALSE);
  R(double, 1.0, max, , MY_FALSE);
  R(max, , double, 1.0, MY_TRUE);
  // precise_datetime vs precise_datetime
  R(precise_datetime, 1, precise_datetime, 2, MY_FALSE);
  R(precise_datetime, 1, precise_datetime, 1, MY_FALSE);
  R(precise_datetime, 2, precise_datetime, 1, MY_TRUE);
  // precise_datetime vs special
  R(precise_datetime, 1, null, , MY_TRUE);
  R(null, , precise_datetime, 1, MY_FALSE);
  R(precise_datetime, 1, min, , MY_TRUE);
  R(min, , precise_datetime, 1, MY_FALSE);
  R(precise_datetime, 1, max, , MY_FALSE);
  R(max, , precise_datetime, 1, MY_TRUE);
  // ctime vs ctime
  R(ctime, 1, ctime, 2, MY_FALSE);
  R(ctime, 1, ctime, 1, MY_FALSE);
  R(ctime, 2, ctime, 1, MY_TRUE);
  // ctime vs special
  R(ctime, 1, null, , MY_TRUE);
  R(null, , ctime, 1, MY_FALSE);
  R(ctime, 1, min, , MY_TRUE);
  R(min, , ctime, 1, MY_FALSE);
  R(ctime, 1, max, , MY_FALSE);
  R(max, , ctime, 1, MY_TRUE);
  // mtime vs mtime
  R(mtime, 1, mtime, 2, MY_FALSE);
  R(mtime, 1, mtime, 1, MY_FALSE);
  R(mtime, 2, mtime, 1, MY_TRUE);
  // mtime vs special
  R(mtime, 1, null, , MY_TRUE);
  R(null, , mtime, 1, MY_FALSE);
  R(mtime, 1, min, , MY_TRUE);
  R(min, , mtime, 1, MY_FALSE);
  R(mtime, 1, max, , MY_FALSE);
  R(max, , mtime, 1, MY_TRUE);
  // bool vs bool
  R(bool, true, bool, true, MY_FALSE);
  R(bool, true, bool, false, MY_TRUE);
  R(bool, false, bool, false, MY_FALSE);
  R(bool, false, bool, true, MY_FALSE);
  // bool vs special
  R(bool, true, null, , MY_TRUE);
  R(null, , bool, true, MY_FALSE);
  R(bool, true, min, , MY_TRUE);
  R(min, , bool, true, MY_FALSE);
  R(bool, true, max, , MY_FALSE);
  R(max, , bool, true, MY_TRUE);
}

#define W(t11, v11, t12, v12, t21, v21, t22, v22, res) \
  ROW2_COMPARE_EXPECT(ObExprGreaterThan, &buf, calc, t11, v11, t12, v12, t21, v21, t22, v22, res)

TEST_F(ObExprGreaterThanTest, row2_basic_test)
{
  ObMalloc buf;
  // (int, int) vs (int int)
  W(int, 0, int, 0, int, 0, int, 0, MY_FALSE);
  W(int, 0, int, 0, int, 0, int, 1, MY_FALSE);
  W(int, 0, int, 0, int, 1, int, 0, MY_FALSE);
  W(int, 0, int, 0, int, 1, int, 1, MY_FALSE);
  W(int, 0, int, 1, int, 0, int, 0, MY_TRUE);
  W(int, 0, int, 1, int, 0, int, 1, MY_FALSE);
  W(int, 0, int, 1, int, 1, int, 0, MY_FALSE);
  W(int, 0, int, 1, int, 1, int, 1, MY_FALSE);
  W(int, 1, int, 0, int, 0, int, 0, MY_TRUE);
  W(int, 1, int, 0, int, 0, int, 1, MY_TRUE);
  W(int, 1, int, 0, int, 1, int, 0, MY_FALSE);
  W(int, 1, int, 0, int, 1, int, 1, MY_FALSE);
  W(int, 1, int, 1, int, 0, int, 0, MY_TRUE);
  W(int, 1, int, 1, int, 0, int, 1, MY_TRUE);
  W(int, 1, int, 1, int, 1, int, 0, MY_TRUE);
  W(int, 1, int, 1, int, 1, int, 1, MY_FALSE);
  // (int, varchar) vs (varchar, double)
  W(int, 0, varchar, "0", varchar, "0", double, 0, MY_FALSE);
  W(int, 0, varchar, "0", varchar, "0", double, 1, MY_FALSE);
  W(int, 0, varchar, "0", varchar, "1", double, 0, MY_FALSE);
  W(int, 0, varchar, "0", varchar, "1", double, 1, MY_FALSE);
  W(int, 0, varchar, "1", varchar, "0", double, 0, MY_TRUE);
  W(int, 0, varchar, "1", varchar, "0", double, 1, MY_FALSE);
  W(int, 0, varchar, "1", varchar, "1", double, 0, MY_FALSE);
  W(int, 0, varchar, "1", varchar, "1", double, 1, MY_FALSE);
  W(int, 1, varchar, "0", varchar, "0", double, 0, MY_TRUE);
  W(int, 1, varchar, "0", varchar, "0", double, 1, MY_TRUE);
  W(int, 1, varchar, "0", varchar, "1", double, 0, MY_FALSE);
  W(int, 1, varchar, "0", varchar, "1", double, 1, MY_FALSE);
  W(int, 1, varchar, "1", varchar, "0", double, 0, MY_TRUE);
  W(int, 1, varchar, "1", varchar, "0", double, 1, MY_TRUE);
  W(int, 1, varchar, "1", varchar, "1", double, 0, MY_TRUE);
  W(int, 1, varchar, "1", varchar, "1", double, 1, MY_FALSE);
  // special values
  W(min, , min, , min, , min, , MY_FALSE);
  W(min, , min, , min, , null, , MY_FALSE);
  W(min, , min, , min, , int,  0, MY_FALSE);
  W(min, , min, , min, , max, , MY_FALSE);
  W(min, , min, , null, , min, , MY_FALSE);
  W(min, , min, , null, , null, , MY_FALSE);
  W(min, , min, , null, , int,  0, MY_FALSE);
  W(min, , min, , null, , max, , MY_FALSE);
  W(min, , min, , int,  0, min, , MY_FALSE);
  W(min, , min, , int,  0, null, , MY_FALSE);
  W(min, , min, , int,  0, int,  0, MY_FALSE);
  W(min, , min, , int,  0, max, , MY_FALSE);
  W(min, , min, , max, , min, , MY_FALSE);
  W(min, , min, , max, , null, , MY_FALSE);
  W(min, , min, , max, , int,  0, MY_FALSE);
  W(min, , min, , max, , max, , MY_FALSE);

  W(min, , null, , min, , min, , MY_TRUE);
  W(min, , null, , min, , null, , MY_FALSE);
  W(min, , null, , min, , int,  0, MY_FALSE);
  W(min, , null, , min, , max, , MY_FALSE);
  W(min, , null, , null, , min, , MY_FALSE);
  W(min, , null, , null, , null, , MY_FALSE);
  W(min, , null, , null, , int,  0, MY_FALSE);
  W(min, , null, , null, , max, , MY_FALSE);
  W(min, , null, , int,  0, min, , MY_FALSE);
  W(min, , null, , int,  0, null, , MY_FALSE);
  W(min, , null, , int,  0, int,  0, MY_FALSE);
  W(min, , null, , int,  0, max, , MY_FALSE);
  W(min, , null, , max, , min, , MY_FALSE);
  W(min, , null, , max, , null, , MY_FALSE);
  W(min, , null, , max, , int,  0, MY_FALSE);
  W(min, , null, , max, , max, , MY_FALSE);

  W(min, , int, 0, min, , min, , MY_TRUE);
  W(min, , int, 0, min, , null, , MY_TRUE);
  W(min, , int, 0, min, , int,  0, MY_FALSE);
  W(min, , int, 0, min, , max, , MY_FALSE);
  W(min, , int, 0, null, , min, , MY_FALSE);
  W(min, , int, 0, null, , null, , MY_FALSE);
  W(min, , int, 0, null, , int,  0, MY_FALSE);
  W(min, , int, 0, null, , max, , MY_FALSE);
  W(min, , int, 0, int,  0, min, , MY_FALSE);
  W(min, , int, 0, int,  0, null, , MY_FALSE);
  W(min, , int, 0, int,  0, int,  0, MY_FALSE);
  W(min, , int, 0, int,  0, max, , MY_FALSE);
  W(min, , int, 0, max, , min, , MY_FALSE);
  W(min, , int, 0, max, , null, , MY_FALSE);
  W(min, , int, 0, max, , int,  0, MY_FALSE);
  W(min, , int, 0, max, , max, , MY_FALSE);

  W(min, , max, , min, , min, , MY_TRUE);
  W(min, , max, , min, , null, , MY_TRUE);
  W(min, , max, , min, , int,  0, MY_TRUE);
  W(min, , max, , min, , max, , MY_FALSE);
  W(min, , max, , null, , min, , MY_FALSE);
  W(min, , max, , null, , null, , MY_FALSE);
  W(min, , max, , null, , int,  0, MY_FALSE);
  W(min, , max, , null, , max, , MY_FALSE);
  W(min, , max, , int,  0, min, , MY_FALSE);
  W(min, , max, , int,  0, null, , MY_FALSE);
  W(min, , max, , int,  0, int,  0, MY_FALSE);
  W(min, , max, , int,  0, max, , MY_FALSE);
  W(min, , max, , max, , min, , MY_FALSE);
  W(min, , max, , max, , null, , MY_FALSE);
  W(min, , max, , max, , int,  0, MY_FALSE);
  W(min, , max, , max, , max, , MY_FALSE);


  W(null, , min, , min, , min, , MY_TRUE);
  W(null, , min, , min, , null, , MY_TRUE);
  W(null, , min, , min, , int,  0, MY_TRUE);
  W(null, , min, , min, , max, , MY_TRUE);
  W(null, , min, , null, , min, , MY_FALSE);
  W(null, , min, , null, , null, , MY_FALSE);
  W(null, , min, , null, , int,  0, MY_FALSE);
  W(null, , min, , null, , max, , MY_FALSE);
  W(null, , min, , int,  0, min, , MY_FALSE);
  W(null, , min, , int,  0, null, , MY_FALSE);
  W(null, , min, , int,  0, int,  0, MY_FALSE);
  W(null, , min, , int,  0, max, , MY_FALSE);
  W(null, , min, , max, , min, , MY_FALSE);
  W(null, , min, , max, , null, , MY_FALSE);
  W(null, , min, , max, , int,  0, MY_FALSE);
  W(null, , min, , max, , max, , MY_FALSE);

  W(null, , null, , min, , min, , MY_TRUE);
  W(null, , null, , min, , null, , MY_TRUE);
  W(null, , null, , min, , int,  0, MY_TRUE);
  W(null, , null, , min, , max, , MY_TRUE);
  W(null, , null, , null, , min, , MY_TRUE);
  W(null, , null, , null, , null, , MY_FALSE);
  W(null, , null, , null, , int,  0, MY_FALSE);
  W(null, , null, , null, , max, , MY_FALSE);
  W(null, , null, , int,  0, min, , MY_FALSE);
  W(null, , null, , int,  0, null, , MY_FALSE);
  W(null, , null, , int,  0, int,  0, MY_FALSE);
  W(null, , null, , int,  0, max, , MY_FALSE);
  W(null, , null, , max, , min, , MY_FALSE);
  W(null, , null, , max, , null, , MY_FALSE);
  W(null, , null, , max, , int,  0, MY_FALSE);
  W(null, , null, , max, , max, , MY_FALSE);

  W(null, , int, 0, min, , min, , MY_TRUE);
  W(null, , int, 0, min, , null, , MY_TRUE);
  W(null, , int, 0, min, , int,  0, MY_TRUE);
  W(null, , int, 0, min, , max, , MY_TRUE);
  W(null, , int, 0, null, , min, , MY_TRUE);
  W(null, , int, 0, null, , null, , MY_TRUE);
  W(null, , int, 0, null, , int,  0, MY_FALSE);
  W(null, , int, 0, null, , max, , MY_FALSE);
  W(null, , int, 0, int,  0, min, , MY_FALSE);
  W(null, , int, 0, int,  0, null, , MY_FALSE);
  W(null, , int, 0, int,  0, int,  0, MY_FALSE);
  W(null, , int, 0, int,  0, max, , MY_FALSE);
  W(null, , int, 0, max, , min, , MY_FALSE);
  W(null, , int, 0, max, , null, , MY_FALSE);
  W(null, , int, 0, max, , int,  0, MY_FALSE);
  W(null, , int, 0, max, , max, , MY_FALSE);

  W(null, , max, , min, , min, , MY_TRUE);
  W(null, , max, , min, , null, , MY_TRUE);
  W(null, , max, , min, , int,  0, MY_TRUE);
  W(null, , max, , min, , max, , MY_TRUE);
  W(null, , max, , null, , min, , MY_TRUE);
  W(null, , max, , null, , null, , MY_TRUE);
  W(null, , max, , null, , int,  0, MY_TRUE);
  W(null, , max, , null, , max, , MY_FALSE);
  W(null, , max, , int,  0, min, , MY_FALSE);
  W(null, , max, , int,  0, null, , MY_FALSE);
  W(null, , max, , int,  0, int,  0, MY_FALSE);
  W(null, , max, , int,  0, max, , MY_FALSE);
  W(null, , max, , max, , min, , MY_FALSE);
  W(null, , max, , max, , null, , MY_FALSE);
  W(null, , max, , max, , int,  0, MY_FALSE);
  W(null, , max, , max, , max, , MY_FALSE);

  W(int, 0, min, , min, , min, , MY_TRUE);
  W(int, 0, min, , min, , null, , MY_TRUE);
  W(int, 0, min, , min, , int,  0, MY_TRUE);
  W(int, 0, min, , min, , max, , MY_TRUE);
  W(int, 0, min, , null, , min, , MY_TRUE);
  W(int, 0, min, , null, , null, , MY_TRUE);
  W(int, 0, min, , null, , int,  0, MY_TRUE);
  W(int, 0, min, , null, , max, , MY_TRUE);
  W(int, 0, min, , int,  0, min, , MY_FALSE);
  W(int, 0, min, , int,  0, null, , MY_FALSE);
  W(int, 0, min, , int,  0, int,  0, MY_FALSE);
  W(int, 0, min, , int,  0, max, , MY_FALSE);
  W(int, 0, min, , max, , min, , MY_FALSE);
  W(int, 0, min, , max, , null, , MY_FALSE);
  W(int, 0, min, , max, , int,  0, MY_FALSE);
  W(int, 0, min, , max, , max, , MY_FALSE);

  W(int, 0, null, , min, , min, , MY_TRUE);
  W(int, 0, null, , min, , null, , MY_TRUE);
  W(int, 0, null, , min, , int,  0, MY_TRUE);
  W(int, 0, null, , min, , max, , MY_TRUE);
  W(int, 0, null, , null, , min, , MY_TRUE);
  W(int, 0, null, , null, , null, , MY_TRUE);
  W(int, 0, null, , null, , int,  0, MY_TRUE);
  W(int, 0, null, , null, , max, , MY_TRUE);
  W(int, 0, null, , int,  0, min, , MY_TRUE);
  W(int, 0, null, , int,  0, null, , MY_FALSE);
  W(int, 0, null, , int,  0, int,  0, MY_FALSE);
  W(int, 0, null, , int,  0, max, , MY_FALSE);
  W(int, 0, null, , max, , min, , MY_FALSE);
  W(int, 0, null, , max, , null, , MY_FALSE);
  W(int, 0, null, , max, , int,  0, MY_FALSE);
  W(int, 0, null, , max, , max, , MY_FALSE);

  W(int, 0, int, 0, min, , min, , MY_TRUE);
  W(int, 0, int, 0, min, , null, , MY_TRUE);
  W(int, 0, int, 0, min, , int,  0, MY_TRUE);
  W(int, 0, int, 0, min, , max, , MY_TRUE);
  W(int, 0, int, 0, null, , min, , MY_TRUE);
  W(int, 0, int, 0, null, , null, , MY_TRUE);
  W(int, 0, int, 0, null, , int,  0, MY_TRUE);
  W(int, 0, int, 0, null, , max, , MY_TRUE);
  W(int, 0, int, 0, int,  0, min, , MY_TRUE);
  W(int, 0, int, 0, int,  0, null, , MY_TRUE);
  W(int, 0, int, 0, int,  0, int,  0, MY_FALSE);
  W(int, 0, int, 0, int,  0, max, , MY_FALSE);
  W(int, 0, int, 0, max, , min, , MY_FALSE);
  W(int, 0, int, 0, max, , null, , MY_FALSE);
  W(int, 0, int, 0, max, , int,  0, MY_FALSE);
  W(int, 0, int, 0, max, , max, , MY_FALSE);

  W(int, 0, max, , min, , min, , MY_TRUE);
  W(int, 0, max, , min, , null, , MY_TRUE);
  W(int, 0, max, , min, , int,  0, MY_TRUE);
  W(int, 0, max, , min, , max, , MY_TRUE);
  W(int, 0, max, , null, , min, , MY_TRUE);
  W(int, 0, max, , null, , null, , MY_TRUE);
  W(int, 0, max, , null, , int,  0, MY_TRUE);
  W(int, 0, max, , null, , max, , MY_TRUE);
  W(int, 0, max, , int,  0, min, , MY_TRUE);
  W(int, 0, max, , int,  0, null, , MY_TRUE);
  W(int, 0, max, , int,  0, int,  0, MY_TRUE);
  W(int, 0, max, , int,  0, max, , MY_FALSE);
  W(int, 0, max, , max, , min, , MY_FALSE);
  W(int, 0, max, , max, , null, , MY_FALSE);
  W(int, 0, max, , max, , int,  0, MY_FALSE);
  W(int, 0, max, , max, , max, , MY_FALSE);

  W(max, , min, , min, , min, , MY_TRUE);
  W(max, , min, , min, , null, , MY_TRUE);
  W(max, , min, , min, , int,  0, MY_TRUE);
  W(max, , min, , min, , max, , MY_TRUE);
  W(max, , min, , null, , min, , MY_TRUE);
  W(max, , min, , null, , null, , MY_TRUE);
  W(max, , min, , null, , int,  0, MY_TRUE);
  W(max, , min, , null, , max, , MY_TRUE);
  W(max, , min, , int,  0, min, , MY_TRUE);
  W(max, , min, , int,  0, null, , MY_TRUE);
  W(max, , min, , int,  0, int,  0, MY_TRUE);
  W(max, , min, , int,  0, max, , MY_TRUE);
  W(max, , min, , max, , min, , MY_FALSE);
  W(max, , min, , max, , null, , MY_FALSE);
  W(max, , min, , max, , int,  0, MY_FALSE);
  W(max, , min, , max, , max, , MY_FALSE);

  W(max, , null, , min, , min, , MY_TRUE);
  W(max, , null, , min, , null, , MY_TRUE);
  W(max, , null, , min, , int,  0, MY_TRUE);
  W(max, , null, , min, , max, , MY_TRUE);
  W(max, , null, , null, , min, , MY_TRUE);
  W(max, , null, , null, , null, , MY_TRUE);
  W(max, , null, , null, , int,  0, MY_TRUE);
  W(max, , null, , null, , max, , MY_TRUE);
  W(max, , null, , int,  0, min, , MY_TRUE);
  W(max, , null, , int,  0, null, , MY_TRUE);
  W(max, , null, , int,  0, int,  0, MY_TRUE);
  W(max, , null, , int,  0, max, , MY_TRUE);
  W(max, , null, , max, , min, , MY_TRUE);
  W(max, , null, , max, , null, , MY_FALSE);
  W(max, , null, , max, , int,  0, MY_FALSE);
  W(max, , null, , max, , max, , MY_FALSE);

  W(max, , int, 0, min, , min, , MY_TRUE);
  W(max, , int, 0, min, , null, , MY_TRUE);
  W(max, , int, 0, min, , int,  0, MY_TRUE);
  W(max, , int, 0, min, , max, , MY_TRUE);
  W(max, , int, 0, null, , min, , MY_TRUE);
  W(max, , int, 0, null, , null, , MY_TRUE);
  W(max, , int, 0, null, , int,  0, MY_TRUE);
  W(max, , int, 0, null, , max, , MY_TRUE);
  W(max, , int, 0, int,  0, min, , MY_TRUE);
  W(max, , int, 0, int,  0, null, , MY_TRUE);
  W(max, , int, 0, int,  0, int,  0, MY_TRUE);
  W(max, , int, 0, int,  0, max, , MY_TRUE);
  W(max, , int, 0, max, , min, , MY_TRUE);
  W(max, , int, 0, max, , null, , MY_TRUE);
  W(max, , int, 0, max, , int,  0, MY_FALSE);
  W(max, , int, 0, max, , max, , MY_FALSE);

  W(max, , max, , min, , min, , MY_TRUE);
  W(max, , max, , min, , null, , MY_TRUE);
  W(max, , max, , min, , int,  0, MY_TRUE);
  W(max, , max, , min, , max, , MY_TRUE);
  W(max, , max, , null, , min, , MY_TRUE);
  W(max, , max, , null, , null, , MY_TRUE);
  W(max, , max, , null, , int,  0, MY_TRUE);
  W(max, , max, , null, , max, , MY_TRUE);
  W(max, , max, , int,  0, min, , MY_TRUE);
  W(max, , max, , int,  0, null, , MY_TRUE);
  W(max, , max, , int,  0, int,  0, MY_TRUE);
  W(max, , max, , int,  0, max, , MY_TRUE);
  W(max, , max, , max, , min, , MY_TRUE);
  W(max, , max, , max, , null, , MY_TRUE);
  W(max, , max, , max, , int,  0, MY_TRUE);
  W(max, , max, , max, , max, , MY_FALSE);
}
*/

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}
