#pragma once

#include <csv.hpp>

#include <base/date.hpp>
#include <base/def.hpp>

namespace ctptrader::service {

struct Underlying {
  base::ID id_;
  std::string name_;
  base::Exchange exchange_;
  base::Mutiple multiple_;
  base::Volume lot_size_;
  base::Price tick_size_;

  base::ID ID() const { return id_; }
  std::string_view Name() const { return name_; }
  base::Exchange Exchange() const { return exchange_; }
  base::Mutiple Multiple() const { return multiple_; }
  base::Volume LotSize() const { return lot_size_; }
  base::Price TickSize() const { return tick_size_; }
};

struct Instrument {
  base::ID id_;
  std::string name_;
  std::string underlying_;
  base::InstrumentType type_;
  base::Date create_date_;
  base::Date expire_date_;
  base::Ratio long_margin_ratio_;
  base::Ratio short_margin_ratio_;

  base::ID ID() const { return id_; }
  std::string_view Name() const { return name_; }
  std::string_view Underlying() const { return underlying_; }
  base::InstrumentType Type() const { return type_; }
  base::Date CreateDate() const { return create_date_; }
  base::Date ExpireDate() const { return expire_date_; }
  base::Ratio LongMarginRatio() const { return long_margin_ratio_; }
  base::Ratio ShortMarginRatio() const { return short_margin_ratio_; }
};

struct CalendarDate {
  base::ID id_;
  std::string name_;
  base::Date date_;
  bool is_trading_day_;
  base::ID prev_trading_day_id_;
  base::ID next_trading_day_id_;

  base::ID ID() const { return id_; }
  std::string_view Name() const { return name_; }
  base::Date Date() const { return date_; }
  bool IsTradingDay() const { return is_trading_day_; }
  base::ID PrevTradingDayID() const { return prev_trading_day_id_; }
  base::ID NextTradingDayID() const { return next_trading_day_id_; }
};

} // namespace ctptrader::service