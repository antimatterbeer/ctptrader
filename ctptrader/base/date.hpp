#pragma once

#include <compare>
#include <string>
#include <time.h>

namespace ctptrader::base {

enum DayOfWeek : int {
  Sunday = 0,
  Monday = 1,
  Tueday = 2,
  Wenesday = 3,
  Thursday = 4,
  Friday = 5,
  Saturday = 6
};

/// @brief Date class
class Date {
public:
  Date() : yyyymmdd_(0) {}
  Date(int64_t yyyymmdd) : yyyymmdd_(yyyymmdd) {}
  Date(const Date &date) : yyyymmdd_(date.yyyymmdd_) {}
  std::string ToString() const { return std::to_string(yyyymmdd_); }
  int AsInt() const { return yyyymmdd_; }
  int Year() const { return yyyymmdd_ / 10000; }
  int Month() const { return yyyymmdd_ / 100 % 100; }
  int Day() const { return yyyymmdd_ % 100; }
  bool IsValid() const;
  int DayOfWeek() const;
  Date AddDays(int days) const;
  // operators
  bool operator==(const Date &rhs) const { return yyyymmdd_ == rhs.yyyymmdd_; }

#if __cplusplus >= 202002L
  std::strong_ordering operator<=>(const Date &rhs) const {
    return yyyymmdd_ <=> rhs.yyyymmdd_;
  }
#else
  bool operator<(const Date &rhs) const { return yyyymmdd_ < rhs.yyyymmdd_; }
  bool operator<=(const Date &rhs) const { return yyyymmdd_ <= rhs.yyyymmdd_; }
  bool operator>(const Date &rhs) const { return yyyymmdd_ > rhs.yyyymmdd_; }
  bool operator>=(const Date &rhs) const { return yyyymmdd_ >= rhs.yyyymmdd_; }
  bool operator!=(const Date &rhs) const { return yyyymmdd_ != rhs.yyyymmdd_; }
#endif

public:
  /// @brief Get today's date
  /// @return Date
  static Date Today();
  /// @brief Get date from days offset from 1970-01-01
  /// @param offset
  /// @return Date
  static Date FromOffset(int offset) { return Date(19700101).AddDays(offset); }

private:
  int yyyymmdd_;
};

} // namespace ctptrader::base