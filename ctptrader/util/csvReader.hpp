#pragma once

#include <string_view>

#include <csv.hpp>

#include <base/date.hpp>
#include <base/def.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::util {

template <int N> class CsvReader {
public:
  explicit CsvReader(const std::string_view path)
      : reader_(path) {}
  ~CsvReader() = default;

  template <typename... Types> bool ReadRow(Types &...args) {
    static_assert(sizeof...(args) == N, "Number of columns mismatch");
    if (!reader_.read_row(row_)) {
      return false;
    }
    ReadCell(0, args...);
    return true;
  }

private:
  void ReadCell(size_t) const {}

  template <typename T, typename... ColTypes>
  void ReadCell(size_t i, T &t, ColTypes &...args) const {
    Read(i, t);
    ReadCell(i + 1, args...);
  }

  void Read(size_t i, bool &v) const { v = row_[i].get<bool>(); }

  void Read(size_t i, int &v) const { v = row_[i].get<int>(); }

  void Read(size_t i, long &v) const { v = row_[i].get<long>(); }

  void Read(size_t i, float &v) const { v = row_[i].get<float>(); }

  void Read(size_t i, double &v) const { v = row_[i].get<double>(); }

  void Read(size_t i, std::string &v) const { v = row_[i].get<std::string>(); }

  void Read(size_t i, base::Date &v) const {
    v = base::Date(row_[i].get<int>());
  }

  void Read(size_t i, base::Timestamp &v) const {
    v = base::Timestamp::FromString(row_[i].get<std::string>());
  }

  void Read(size_t i, base::Exchange &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::ExchangeMap.find(s);
    v = it == base::ExchangeMap.end() ? base::Exchange_Invalid : it->second;
  }

  void Read(size_t i, base::InstrumentType &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::InstrumentTypeMap.find(s);
    v = it == base::InstrumentTypeMap.end() ? base::InstrumentType_Invalid
                                            : it->second;
  }

  void Read(size_t i, base::Direction &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::DirectionMap.find(s);
    v = it == base::DirectionMap.end() ? base::Direction_Invalid : it->second;
  }

  void Read(size_t i, base::PriceType &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::PriceTypeMap.find(s);
    v = it == base::PriceTypeMap.end() ? base::PriceType_Invalid : it->second;
  }

  void Read(size_t i, base::OrderStatus &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::OrderStatusMap.find(s);
    v = it == base::OrderStatusMap.end() ? base::OrderStatus_Invalid
                                         : it->second;
  }

  void Read(size_t i, base::Currency &v) const {
    auto s = row_[i].get<std::string>();
    auto it = base::CurrencyMap.find(s);
    v = it == base::CurrencyMap.end() ? base::Currency_Invalid : it->second;
  }

private:
  csv::CSVReader reader_;
  csv::CSVRow row_;
};

} // namespace ctptrader::util