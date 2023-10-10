#include <gtest/gtest.h>

#include <base/def.hpp>
#include <service/dataService.hpp>
#include <util/csvReader.hpp>

using namespace ctptrader;

TEST(RefCenter, UnderlyingCenter) {
  service::RefCenter<service::Underlying> uc;
  ASSERT_TRUE(uc.FromCSV("./fixtures/underlying.csv"));
  ASSERT_EQ(uc.Size(), 2);
  auto &u = uc.Get(0);
  ASSERT_EQ(u.ID(), 0);
  ASSERT_EQ(u.Name(), "cu");
  ASSERT_EQ(u.Exchange(), base::Exchange::Exchange_SHFE);
  ASSERT_EQ(u.Multiple(), 5);
  ASSERT_EQ(u.LotSize(), 5);
  ASSERT_EQ(u.TickSize(), 2);
}

TEST(RefCenter, InstrumentCenter) {
  service::RefCenter<service::Instrument> ic;
  ASSERT_TRUE(ic.FromCSV("./fixtures/instrument.csv"));
  ASSERT_EQ(ic.Size(), 11);
  auto &i = ic.Get("cu2311");
  ASSERT_EQ(i.ID(), 0);
  ASSERT_EQ(i.Name(), "cu2311");
  ASSERT_EQ(i.Underlying(), "cu");
  ASSERT_EQ(i.Type(), base::InstrumentType::InstrumentType_Futures);
  ASSERT_EQ(i.CreateDate(), base::Date(20221116));
  ASSERT_EQ(i.ExpireDate(), base::Date(20231115));
  ASSERT_EQ(i.LongMarginRatio(), 0.1);
  ASSERT_EQ(i.ShortMarginRatio(), 0.1);
}

TEST(RefCenter, CalendarCenter) {
  service::RefCenter<service::CalendarDate> cc;
  ASSERT_TRUE(cc.FromCSV("./fixtures/calendar.csv"));
  ASSERT_EQ(cc.Size(), 3653);
  auto &cd = cc.Get(1);
  ASSERT_EQ(cd.ID(), 1);
  ASSERT_EQ(cd.Name(), "20150102");
  ASSERT_EQ(cd.Date(), base::Date(20150102));
  ASSERT_FALSE(cd.IsTradingDay());
  ASSERT_EQ(cd.PrevTradingDayID(), 0);
  ASSERT_EQ(cd.NextTradingDayID(), 4);
}

TEST(DataService, Init) {
  ASSERT_TRUE(DataService().Init("./fixtures"));
  ASSERT_EQ(DataService().UnderlyingCenter().Size(), 2);
  ASSERT_EQ(DataService().InstrumentCenter().Size(), 11);
  ASSERT_EQ(DataService().CalendarCenter().Size(), 3653);
}