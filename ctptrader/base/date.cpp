#include <base/date.hpp>

namespace ctptrader::base {

inline bool IsLeapYear(int year) {
  return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

inline int DaysInMonth(int year, int month) {
  static const int DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12) {
    return 0;
  }
  if (month == 2 && IsLeapYear(year)) {
    return 29;
  }
  return DAYS_IN_MONTH[month - 1];
}

bool Date::IsValid() const {
  auto y = Year();
  if (y < 1900 || y > 2100) {
    return false;
  }
  auto m = Month();
  auto d = Day();
  if (m < 1 || m > 12 || d < 1 || d > DaysInMonth(y, m)) {
    return false;
  }
  return true;
}

int Date::DayOfWeek() const {
  tm t{};
  t.tm_year = Year() - 1900;
  t.tm_mon = Month() - 1;
  t.tm_mday = Day();
  t.tm_hour = 12;
  mktime(&t);
  return t.tm_wday;
}

Date Date::AddDays(int days) const {
  tm t{};
  t.tm_year = Year() - 1900;
  t.tm_mon = Month() - 1;
  t.tm_mday = Day() + days;
  t.tm_hour = 12;
  mktime(&t);
  return Date((t.tm_year + 1900) * 10000 + (t.tm_mon + 1) * 100 + t.tm_mday);
}

Date Date::Today() {
  time_t t = time(nullptr);
  tm *local_time = localtime(&t);
  return Date((local_time->tm_year + 1900) * 10000 +
              (local_time->tm_mon + 1) * 100 + local_time->tm_mday);
}

} // namespace ctptrader::base