#pragma once

#include <vector>

#define FMT_HEADER_ONLY
#include <DolphinDB.h>
#include <Util.h>
#include <fmt/format.h>

namespace ctptrader::util {

using dolphindb::ConstantSP;
using dolphindb::DBConnection;
using dolphindb::TableSP;

template <uint8_t N> class TableReader {
public:
  TableReader(TableSP table)
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
  void ReadCell(size_t idx) {}

  template <typename T, typename... Types>
  void ReadCell(size_t idx, T &t, Types &...args) {
    Get(idx, t);
    ReadCell(idx + 1, args...);
  }

  void Get(size_t idx, char &value) { value = cols_[idx]->getChar(row_idx_); }
  void Get(size_t idx, bool &value) { value = cols_[idx]->getBool(row_idx_); }
  void Get(size_t idx, int &value) { value = cols_[idx]->getInt(row_idx_); }
  void Get(size_t idx, long &value) { value = cols_[idx]->getLong(row_idx_); }
  void Get(size_t idx, float &value) { value = cols_[idx]->getFloat(row_idx_); }
  void Get(size_t idx, double &value) {
    value = cols_[idx]->getDouble(row_idx_);
  }

private:
  size_t row_count_;
  size_t row_idx_{0};
  std::vector<ConstantSP> cols_;
};

} // namespace ctptrader::util