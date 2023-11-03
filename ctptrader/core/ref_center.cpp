#include <iostream>

#include <base/ref.hpp>
#include <core/ref_center.hpp>
#include <util/csvReader.hpp>

namespace ctptrader::core {

template <>
bool RefCenter<base::Account>::LoadFromCsv(std::string_view filename) {
  util::CsvReader<3> reader(filename);
  base::Account account;
  while (reader.ReadRow(account.id_, account.name_, account.alias_)) {
    if (account.id_ != vec_.size()) {
      std::cerr << "Account id mismatch: " << account.id_ << std::endl;
      return false;
    }
    vec_.emplace_back(account);
    id_map_[account.name_] = account.id_;
  }
  return true;
}

template <>
bool RefCenter<base::Underlying>::LoadFromCsv(std::string_view filename) {
  util::CsvReader<6> reader(filename);
  base::Underlying u;
  while (reader.ReadRow(u.id_, u.name_, u.exchange_, u.multiple_, u.lot_size_,
                        u.tick_size_)) {
    if (u.id_ != vec_.size()) {
      std::cerr << "ID is not continuous.\n";
      return false;
    }
    vec_.emplace_back(u);
    id_map_[u.name_] = u.id_;
  }
  return true;
}

template <>
bool RefCenter<base::Instrument>::LoadFromCsv(std::string_view filename) {
  util::CsvReader<8> reader(filename);
  base::Instrument i;
  while (reader.ReadRow(i.id_, i.name_, i.underlying_, i.type_, i.create_date_,
                        i.expire_date_, i.long_margin_ratio_,
                        i.short_margin_ratio_)) {
    if (i.id_ != vec_.size()) {
      std::cerr << "Instrument id mismatch: " << i.id_ << std::endl;
      return false;
    }
    vec_.emplace_back(i);
    id_map_[i.name_] = i.id_;
  }
  return true;
}

template <>
bool RefCenter<base::CalendarDate>::LoadFromCsv(std::string_view filename) {
  util::CsvReader<5> reader(filename);
  struct Row {
    base::DateID id_;
    int date_;
    bool is_trading_day_;
    int prev_trading_day_;
    int next_trading_day_;
  } row{};
  std::vector<Row> cache;
  std::unordered_map<int, base::DateID> id_map;
  while (reader.ReadRow(row.id_, row.date_, row.is_trading_day_,
                        row.prev_trading_day_, row.next_trading_day_)) {
    cache.emplace_back(row);
    id_map[row.date_] = row.id_;
  }
  base::CalendarDate date;
  for (auto &r : cache) {
    if (r.id_ != vec_.size()) {
      std::cerr << "Date id mismatch: " << r.id_ << std::endl;
      return false;
    }
    date.id_ = r.id_;
    date.name_ = std::to_string(r.date_);
    date.date_ = base::Date(r.date_);
    date.is_trading_day_ = r.is_trading_day_;
    date.prev_trading_day_id_ = id_map[r.prev_trading_day_];
    date.next_trading_day_id_ = id_map[r.next_trading_day_];
    vec_.emplace_back(date);
    id_map_[date.name_] = date.id_;
  }
  return true;
}
} // namespace ctptrader::core