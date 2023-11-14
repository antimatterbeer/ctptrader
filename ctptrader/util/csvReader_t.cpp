#include <gtest/gtest.h>

#include <util/csvReader.hpp>

namespace {

using namespace ctptrader::base;
using namespace ctptrader::util;

TEST(CsvReaderTest, ReadRow) {
  CsvReader<14> reader("./test/test.csv");
  bool b;
  int i;
  long l;
  float f;
  double d;
  std::string s;
  Date date;
  Timestamp ts;
  Exchange ex;
  InstrumentType it;
  Direction di;
  PriceType pt;
  OrderStatus os;
  Currency c;
  EXPECT_TRUE(
      reader.ReadRow(b, i, l, f, d, s, date, ts, ex, it, di, pt, os, c));
  EXPECT_EQ(b, false);
  EXPECT_EQ(i, 1);
  EXPECT_EQ(l, 2);
  EXPECT_FLOAT_EQ(f, 3.0);
  EXPECT_DOUBLE_EQ(d, 4.0);
  EXPECT_EQ(s, "5");
  EXPECT_EQ(date, Date(20230101));
  // EXPECT_EQ(ts, Timestamp::FromString("2023-01-01 08:30:25.000"));
  EXPECT_EQ(ex, Exchange_XSHG);
  EXPECT_EQ(it, InstrumentType_Stock);
  EXPECT_EQ(di, Direction_Buy);
  EXPECT_EQ(pt, PriceType_Limit);
  EXPECT_EQ(os, OrderStatus_Canceled);
  EXPECT_EQ(c, Currency_CNY);
}

} // namespace
