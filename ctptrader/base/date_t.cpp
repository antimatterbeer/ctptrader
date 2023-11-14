#include <gtest/gtest.h>

#include <base/date.hpp>

namespace {

using namespace ctptrader::base;

TEST(DateTest, Constructor) {
  Date date(20230101);
  EXPECT_TRUE(date.IsValid());
  EXPECT_EQ(date.Year(), 2023);
  EXPECT_EQ(date.Month(), 1);
  EXPECT_EQ(date.Day(), 1);
  EXPECT_EQ(date.AsInt(), 20230101);
  EXPECT_EQ(date.DayOfWeek(), 0);
}

TEST(Date, Operator) {
  Date date1(20230101);
  Date date2(20230101);
  Date date3(20230102);
  EXPECT_EQ(date1, date2);
  EXPECT_NE(date1, date3);
  EXPECT_LT(date1, date3);
  EXPECT_LE(date1, date3);
  EXPECT_GT(date3, date1);
  EXPECT_GE(date3, date1);
}

TEST(Date, AddDays) {
  Date date(20230101);
  EXPECT_EQ(date.AddDays(1).AsInt(), 20230102);
  EXPECT_EQ(date.AddDays(-1).AsInt(), 20221231);
}

} // namespace