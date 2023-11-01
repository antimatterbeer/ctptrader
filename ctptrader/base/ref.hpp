#pragma once

#include <csv.hpp>

#include <base/date.hpp>
#include <base/def.hpp>

namespace ctptrader::base {

struct Underlying {
  ID id_;
  std::string name_;
  Exchange exchange_;
  Mutiple multiple_;
  Volume lot_size_;
  Price tick_size_;
};

struct Instrument {
  ID id_;
  std::string name_;
  std::string underlying_;
  InstrumentType type_;
  Date create_date_;
  Date expire_date_;
  Ratio long_margin_ratio_;
  Ratio short_margin_ratio_;
};

struct CalendarDate {
  ID id_;
  std::string name_;
  Date date_;
  bool is_trading_day_;
  ID prev_trading_day_id_;
  ID next_trading_day_id_;
};

struct Account {
  AccountID id_;
  std::string name_;
  std::string alias_;
};

} // namespace ctptrader::base