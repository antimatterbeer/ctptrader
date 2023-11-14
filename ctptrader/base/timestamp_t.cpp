#include <gtest/gtest.h>

#include <base/timestamp.hpp>

namespace {

using namespace ctptrader::base;

constexpr long Ts = 1672533025;
constexpr auto TimeStr = "2023-01-01 08:30:25.000";

TEST(TimestampTest, ToDate) {
  Timestamp ts = Timestamp::FromSeconds(Ts);
  EXPECT_EQ(ts.ToDate(), "2023-01-01");
}

TEST(TimestampTest, ToTime) {
  Timestamp ts = Timestamp::FromSeconds(Ts);
  EXPECT_EQ(ts.ToTime(), "08:30:25.000");
}

TEST(TimestampTest, ToString) {
  Timestamp ts = Timestamp::FromSeconds(Ts);
  EXPECT_EQ(ts.ToString(), TimeStr);
}

TEST(TimestampTest, IsEmpty) {
  Timestamp ts{0, 0};
  EXPECT_TRUE(ts.IsEmpty());
  ts.tv_sec = 1;
  EXPECT_FALSE(ts.IsEmpty());
}

TEST(TimestampTest, Clear) {
  Timestamp ts = Timestamp::FromSeconds(Ts);
  EXPECT_FALSE(ts.IsEmpty());
  ts.Clear();
  EXPECT_TRUE(ts.IsEmpty());
}

TEST(TimestampTest, Operators) {
  Timestamp ts1 = Timestamp::FromSeconds(Ts);
  Timestamp ts2 = Timestamp::FromSeconds(Ts);
  Timestamp ts3 = Timestamp::FromSeconds(Ts + 1);
  EXPECT_EQ(ts1, ts2);
  EXPECT_NE(ts1, ts3);
  EXPECT_LT(ts1, ts3);
  EXPECT_LE(ts1, ts3);
  EXPECT_GT(ts3, ts1);
  EXPECT_GE(ts3, ts1);
}

TEST(TimestampTest, FromNanoSeconds) {
  Timestamp ts = Timestamp::FromNanoSeconds(1234567890123456789);
  EXPECT_EQ(ts.tv_sec, 1234567890);
  EXPECT_EQ(ts.tv_nsec, 123456789);
}

TEST(TimestampTest, FromMilliSeconds) {
  Timestamp ts = Timestamp::FromMilliSeconds(1234567890);
  EXPECT_EQ(ts.tv_sec, 1234567);
  EXPECT_EQ(ts.tv_nsec, 890000000);
}

TEST(TimestampTest, FromMicroSeconds) {
  Timestamp ts = Timestamp::FromMicroSeconds(1234567890123);
  EXPECT_EQ(ts.tv_sec, 1234567);
  EXPECT_EQ(ts.tv_nsec, 890123000);
}

TEST(TimestampTest, FromSeconds) {
  Timestamp ts = Timestamp::FromSeconds(1234567890);
  EXPECT_EQ(ts.tv_sec, 1234567890);
  EXPECT_EQ(ts.tv_nsec, 0);
}

} // namespace
