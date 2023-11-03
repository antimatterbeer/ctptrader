#include <gtest/gtest.h>

#include <base/ref.hpp>
#include <core/ref_center.hpp>

using namespace ctptrader;

TEST(DataRefTest, LoadFromCsv) {
  core::RefCenter<base::Account> acc_ref;
  EXPECT_TRUE(acc_ref.LoadFromCsv("./fixtures/account.csv"));
  EXPECT_EQ(acc_ref.Size(), 2);
  EXPECT_EQ(acc_ref.Get(0).id_, 0);
  EXPECT_EQ(acc_ref.Get(0).name_, "test001");
  EXPECT_EQ(acc_ref.Get(0).alias_, "测试账户1");
  EXPECT_EQ(acc_ref.Get(1).id_, 1);
  EXPECT_EQ(acc_ref.Get(1).name_, "test002");
  EXPECT_EQ(acc_ref.Get(1).alias_, "测试账户2");
}