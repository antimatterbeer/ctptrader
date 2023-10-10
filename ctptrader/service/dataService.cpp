#include <service/dataService.hpp>
#include <util/csvReader.hpp>

namespace ctptrader::service {

template <> bool RefCenter<Underlying>::FromCSV(std::string_view path) {
  util::CsvReader<6> reader(path);
  Underlying u;
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

template <> bool RefCenter<Instrument>::FromCSV(std::string_view path) {
  util::CsvReader<8> reader(path);
  Instrument i;
  while (reader.ReadRow(i.id_, i.name_, i.underlying_, i.type_, i.create_date_,
                        i.expire_date_, i.long_margin_ratio_,
                        i.short_margin_ratio_)) {
    if (i.id_ != vec_.size()) {
      std::cerr << "ID is not continuous.\n";
      return false;
    }
    vec_.emplace_back(i);
    id_map_[i.name_] = i.id_;
  }
  return true;
}

template <> bool RefCenter<CalendarDate>::FromCSV(std::string_view path) {
  util::CsvReader<5> reader(path);
  struct Row {
    base::ID id_;
    int date_;
    bool is_trading_day_;
    int prev_trading_day_;
    int next_trading_day_;
  } row;
  std::vector<Row> cache;
  std::unordered_map<int, base::ID> id_map;
  while (reader.ReadRow(row.id_, row.date_, row.is_trading_day_,
                        row.prev_trading_day_, row.next_trading_day_)) {
    cache.emplace_back(row);
    id_map[row.date_] = row.id_;
  }

  CalendarDate cd;
  for (auto &r : cache) {
    if (r.id_ != vec_.size()) {
      std::cerr << "ID is not continuous.\n";
      return false;
    }
    cd.id_ = r.id_;
    cd.name_ = std::to_string(r.date_);
    cd.date_ = base::Date(r.date_);
    cd.is_trading_day_ = r.is_trading_day_;
    cd.prev_trading_day_id_ = id_map[r.prev_trading_day_];
    cd.next_trading_day_id_ = id_map[r.next_trading_day_];
    vec_.emplace_back(cd);
    id_map_[cd.name_] = r.id_;
  }
  return true;
}

bool DataService::Init(std::string_view path) {
  std::filesystem::path folder(path);
  if (!std::filesystem::is_directory(folder)) {
    std::cerr << "Directory " << path << " does not exist.\n";
    return false;
  }
  underlying_center_.Clear();
  instrument_center_.Clear();
  calendar_center_.Clear();
  if (!underlying_center_.FromCSV((folder / "underlying.csv").string())) {
    std::cerr << "Failed to load underlying.csv.\n";
    return false;
  }
  if (!instrument_center_.FromCSV((folder / "instrument.csv").string())) {
    std::cerr << "Failed to load instrument.csv.\n";
    return false;
  }
  if (!calendar_center_.FromCSV((folder / "calendar.csv").string())) {
    std::cerr << "Failed to load calendar.csv.\n";
    return false;
  }
  return true;
}

} // namespace ctptrader::service