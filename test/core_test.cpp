#include <gtest/gtest.h>

#include <base/msg.hpp>
#include <base/ref.hpp>
#include <core/buf_center.hpp>
#include <core/ref_center.hpp>

using namespace ctptrader;

TEST(RefCenter, LoadFromCsv) {
  core::RefCenter<base::Account> rc;
  EXPECT_TRUE(rc.LoadFromCsv("./fixtures/account.csv"));
  EXPECT_EQ(rc.Size(), 2);
  EXPECT_EQ(rc.Get(0).id_, 0);
  EXPECT_EQ(rc.Get(0).name_, "test001");
  EXPECT_EQ(rc.Get(0).alias_, "测试账户1");
  EXPECT_EQ(rc.Get(1).id_, 1);
  EXPECT_EQ(rc.Get(1).name_, "test002");
  EXPECT_EQ(rc.Get(1).alias_, "测试账户2");
}

TEST(BufCenter, PushBack) {
  core::BufCenter<base::Balance, 2> bc;
  bc.Resize(10);
  bc.Push(base::Balance(0, 1000, 1000));
  bc.Push(base::Balance(0, 1100, 1100));
  EXPECT_EQ(bc.Count(), 10);
  EXPECT_EQ(bc.Back(0).available_, 1100);
  EXPECT_EQ(bc.Prev(0).available_, 1000);
}