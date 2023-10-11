#include <gtest/gtest.h>

#include <base/timestamp.hpp>

using namespace ctptrader;

TEST(Timestamp, FromString) {
  auto ts = base::Timestamp::FromString("20231011 11:29:58");
  EXPECT_EQ(ts.ToString(), "20231011 11:29:58.000");
}