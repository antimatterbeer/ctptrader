#pragma once

#include <vector>

#define FMT_HEADER_ONLY
#include <DolphinDB.h>

#include <base/date.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::util {

using dolphindb::ConstantSP;
using dolphindb::DBConnection;
using dolphindb::TableSP;

template <uint8_t N> class TableReader {
public:
  explicit TableReader(const TableSP &table)
      : row_count_(table->rows()) {
    for (size_t i = 0; i < N; ++i) {
      cols_.push_back(table->getColumn(i));
    }
  }
  ~TableReader() = default;

  [[nodiscard]] bool Empty() const { return row_idx_ >= row_count_; }

  [[nodiscard]] int RowCount() const { return row_count_; }

  template <typename... Types> [[nodiscard]] bool Read(Types &...args) {
    if (Empty()) {
      return false;
    }
    ReadCell(0, args...);
    row_idx_++;
    return true;
  }

private:
  void ReadCell(size_t idx) const {}

  template <typename T, typename... Types>
  void ReadCell(size_t idx, T &t, Types &...args) const {
    Get(idx, t);
    ReadCell(idx + 1, args...);
  }

  void Get(size_t i, char &v) const { v = cols_[i]->getChar(row_idx_); }

  void Get(size_t i, bool &v) const { v = cols_[i]->getBool(row_idx_); }

  void Get(size_t i, int &v) const { v = cols_[i]->getInt(row_idx_); }

  void Get(size_t i, long &v) const { v = cols_[i]->getLong(row_idx_); }

  void Get(size_t i, float &v) const { v = cols_[i]->getFloat(row_idx_); }

  void Get(size_t i, double &v) const { v = cols_[i]->getDouble(row_idx_); }

  void Get(size_t i, base::Date &v) const {}

  void Get(size_t i, base::Timestamp &v) const {}

private:
  size_t row_count_;
  size_t row_idx_{0};
  std::vector<ConstantSP> cols_;
};

} // namespace ctptrader::util