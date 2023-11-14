#include <gtest/gtest.h>

#include <core/ctx.hpp>

namespace {

using namespace ctptrader::base;
using namespace ctptrader::core;

TEST(RefCenterTest, Init) {
  RefCenter<Account> ac;
  EXPECT_TRUE(ac.LoadFromCsv("./test/account.csv"));
  EXPECT_EQ(ac.Count(), 2);
  EXPECT_TRUE(ac.HasID(0));
  EXPECT_FALSE(ac.HasID(2));
  EXPECT_TRUE(ac.HasName("test001"));
  EXPECT_FALSE(ac.HasName("test003"));
  EXPECT_EQ(ac.Get(0).name_, "test001");
  EXPECT_EQ(ac.Get("test001").id_, 0);
  EXPECT_EQ(ac.GetID("test003"), -1);
}

TEST(BufCenterTest, Init) {
  Bar bar;
  bar.id_ = 0;
  bar.close_ = 1.0;
  BufCenter<Bar, 240> bc;
  EXPECT_EQ(bc.Count(), 0);
  EXPECT_EQ(bc.Capacity(), 240);
  bc.Resize(10);
  EXPECT_EQ(bc.Count(), 10);
  EXPECT_EQ(bc.Size(0), 0);
  EXPECT_FALSE(bc.HasValue(0));
  EXPECT_FALSE(bc.HasPrev(0));
  bc.PushBack(bar);
  EXPECT_EQ(bc.Size(0), 1);
  EXPECT_TRUE(bc.HasValue(0));
  EXPECT_FALSE(bc.HasPrev(0));
  EXPECT_EQ(bc.Back(bar.id_).close_, bar.close_);
}

TEST(ContextTest, Init) {
  Context ctx;
  EXPECT_TRUE(ctx.Init("./test"));
}

} // namespace
