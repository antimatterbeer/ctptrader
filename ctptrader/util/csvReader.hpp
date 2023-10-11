#pragma once

#include <string_view>

#include <csv.hpp>

#include <base/date.hpp>
#include <base/def.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::util {

template <int N> class CsvReader {
public:
  CsvReader(std::string_view path)
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
  void ReadCell(size_t) {}

  template <typename T, typename... ColTypes>
  void ReadCell(size_t i, T &t, ColTypes &...args) {
    Read(i, t);
    ReadCell(i + 1, args...);
  }
  void Read(size_t i, bool &v);
  void Read(size_t i, int &v);
  void Read(size_t i, long &v);
  void Read(size_t i, float &v);
  void Read(size_t i, double &v);
  void Read(size_t i, std::string &v);
  void Read(size_t i, base::Exchange &v);
  void Read(size_t i, base::InstrumentType &v);
  void Read(size_t i, base::Direction &v);
  void Read(size_t i, base::PriceType &v);
  void Read(size_t i, base::OrderStatus &v);
  void Read(size_t i, base::Currency &v);
  void Read(size_t i, base::Date &v);
  void Read(size_t i, base::Timestamp &v);

private:
  csv::CSVReader reader_;
  csv::CSVRow row_;
};

template <int N> inline void CsvReader<N>::Read(size_t i, bool &v) {
  v = row_[i].get<bool>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, int &v) {
  v = row_[i].get<int>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, long &v) {
  v = row_[i].get<long>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, float &v) {
  v = row_[i].get<float>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, double &v) {
  v = row_[i].get<double>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, std::string &v) {
  v = row_[i].get<std::string>();
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::Exchange &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::ExchangeMap.find(s);
  v = it == base::ExchangeMap.end() ? base::Exchange_Invalid : it->second;
}

template <int N>
inline void CsvReader<N>::Read(size_t i, base::InstrumentType &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::InstrumentTypeMap.find(s);
  v = it == base::InstrumentTypeMap.end() ? base::InstrumentType_Invalid
                                          : it->second;
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::Direction &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::DirectionMap.find(s);
  v = it == base::DirectionMap.end() ? base::Direction_Invalid : it->second;
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::PriceType &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::PriceTypeMap.find(s);
  v = it == base::PriceTypeMap.end() ? base::PriceType_Invalid : it->second;
}

template <int N>
inline void CsvReader<N>::Read(size_t i, base::OrderStatus &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::OrderStatusMap.find(s);
  v = it == base::OrderStatusMap.end() ? base::OrderStatus_Invalid : it->second;
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::Currency &v) {
  auto s = row_[i].get<std::string>();
  auto it = base::CurrencyMap.find(s);
  v = it == base::CurrencyMap.end() ? base::Currency_Invalid : it->second;
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::Date &v) {
  v = base::Date(row_[i].get<int>());
}

template <int N> inline void CsvReader<N>::Read(size_t i, base::Timestamp &v) {
  v = base::Timestamp::FromMilliSeconds(row_[i].get<long>());
}

} // namespace ctptrader::util