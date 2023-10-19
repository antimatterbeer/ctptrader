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

  [[nodiscard]] base::ID ID() const { return id_; }
  [[nodiscard]] std::string_view Name() const { return name_; }
  [[nodiscard]] base::Exchange Exchange() const { return exchange_; }
  [[nodiscard]] base::Mutiple Multiple() const { return multiple_; }
  [[nodiscard]] base::Volume LotSize() const { return lot_size_; }
  [[nodiscard]] base::Price TickSize() const { return tick_size_; }
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

  [[nodiscard]] base::ID ID() const { return id_; }
  [[nodiscard]] std::string_view Name() const { return name_; }
  [[nodiscard]] std::string_view Underlying() const { return underlying_; }
  [[nodiscard]] base::InstrumentType Type() const { return type_; }
  [[nodiscard]] base::Date CreateDate() const { return create_date_; }
  [[nodiscard]] base::Date ExpireDate() const { return expire_date_; }
  [[nodiscard]] base::Ratio LongMarginRatio() const {
    return long_margin_ratio_;
  }
  [[nodiscard]] base::Ratio ShortMarginRatio() const {
    return short_margin_ratio_;
  }
};

struct CalendarDate {
  base::ID id_;
  std::string name_;
  base::Date date_;
  bool is_trading_day_;
  base::ID prev_trading_day_id_;
  base::ID next_trading_day_id_;

  [[nodiscard]] base::ID ID() const { return id_; }
  [[nodiscard]] std::string_view Name() const { return name_; }
  [[nodiscard]] base::Date Date() const { return date_; }
  [[nodiscard]] bool IsTradingDay() const { return is_trading_day_; }
  [[nodiscard]] base::ID PrevTradingDayID() const {
    return prev_trading_day_id_;
  }
  [[nodiscard]] base::ID NextTradingDayID() const {
    return next_trading_day_id_;
  }
};

} // namespace ctptrader::service