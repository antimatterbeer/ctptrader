#pragma once

#include <compare>
#include <cstring>
#include <string>
#include <time.h>

namespace ctptrader::base {

enum { Hour = 0, Minute, Second, Millisecond, Microsecond, Nanosecond };

static constexpr long NANOSINSECOND = 1000000000L;
static constexpr long MICROSINSECOND = 1000000L;
static constexpr long MILLISINSECOND = 1000L;
static constexpr long NANOSINMILLI = 1000000L;
static constexpr long MICROSINMILLI = 1000L;
static constexpr long NANOSINMICRO = 1000L;

class Timestamp : public timespec {
public:
  std::string ToString() const {
    char buf[32];
    struct tm t;
    localtime_r(&tv_sec, &t);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03ld", t.tm_year + 1900,
            t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
            tv_nsec / NANOSINMILLI);
    return buf;
  }

  std::string ToDate() const {
    char buf[32];
    struct tm t;
    localtime_r(&tv_sec, &t);
    sprintf(buf, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    return buf;
  }

  std::string ToTime() const {
    char buf[32];
    struct tm t;
    localtime_r(&tv_sec, &t);
    sprintf(buf, "%02d:%02d:%02d.%03ld", t.tm_hour, t.tm_min, t.tm_sec,
            tv_nsec / NANOSINMILLI);
    return buf;
  }

  bool IsEmpty() const { return tv_sec == 0 && tv_nsec == 0; }
  void clear() { tv_sec = tv_nsec = 0; }

  // operators
  bool operator==(const Timestamp &rhs) const {
    return tv_sec == rhs.tv_sec && tv_nsec == rhs.tv_nsec;
  }

#if __cplusplus >= 202002L
  std::strong_ordering operator<=>(const Timestamp &rhs) const {
    auto cmp = tv_sec <=> rhs.tv_sec;
    return cmp != 0 ? cmp : tv_nsec <=> rhs.tv_nsec;
  }
#else
  bool operator<(const Timestamp &rhs) const {
    return tv_sec < rhs.tv_sec ||
           (tv_sec == rhs.tv_sec && tv_nsec < rhs.tv_nsec);
  }

  bool operator<=(const Timestamp &rhs) const {
    return tv_sec < rhs.tv_sec ||
           (tv_sec == rhs.tv_sec && tv_nsec <= rhs.tv_nsec);
  }

  bool operator>(const Timestamp &rhs) const {
    return tv_sec > rhs.tv_sec ||
           (tv_sec == rhs.tv_sec && tv_nsec > rhs.tv_nsec);
  }

  bool operator>=(const Timestamp &rhs) const {
    return tv_sec > rhs.tv_sec ||
           (tv_sec == rhs.tv_sec && tv_nsec >= rhs.tv_nsec);
  }

  bool operator!=(const Timestamp &rhs) const {
    return tv_sec != rhs.tv_sec || tv_nsec != rhs.tv_nsec;
  }
#endif

  Timestamp operator+(const Timestamp &rhs) const {
    Timestamp ts;
    ts.tv_nsec = tv_nsec + rhs.tv_nsec;
    if (ts.tv_nsec < 0) {
      ts.tv_nsec += NANOSINSECOND;
      ts.tv_sec -= 1;
    }
    ts.tv_sec = tv_sec + rhs.tv_sec + ts.tv_nsec / NANOSINSECOND;
    ts.tv_nsec %= NANOSINSECOND;
    return ts;
  }

  Timestamp &operator+=(const Timestamp &rhs) {
    tv_nsec += rhs.tv_nsec;
    tv_sec += rhs.tv_sec + tv_nsec / NANOSINSECOND;
    tv_nsec %= NANOSINSECOND;
    return *this;
  }

  Timestamp operator-(const Timestamp &rhs) const {
    Timestamp ts;
    if (tv_nsec < rhs.tv_nsec) {
      ts.tv_sec = tv_sec - rhs.tv_sec - 1;
      ts.tv_nsec = tv_nsec - rhs.tv_nsec + NANOSINSECOND;
    } else {
      ts.tv_nsec = tv_nsec - rhs.tv_nsec;
      ts.tv_sec = tv_sec - rhs.tv_sec;
    }
    return ts;
  }

  Timestamp &operator-=(const Timestamp &rhs) {
    if (tv_nsec < rhs.tv_nsec) {
      tv_sec -= rhs.tv_sec + 1;
      tv_nsec -= rhs.tv_nsec - NANOSINSECOND;
    } else {
      tv_sec -= rhs.tv_sec;
      tv_nsec -= rhs.tv_nsec;
    }
    return *this;
  }

  // static
  __attribute__((__always_inline__)) static Timestamp
  FromNanoSeconds(time_t nanoseconds) {
    return Timestamp{nanoseconds / NANOSINSECOND, nanoseconds % NANOSINSECOND};
  }

  __attribute__((__always_inline__)) static Timestamp
  FromMilliSeconds(time_t milliseconds) {
    return Timestamp{milliseconds / MILLISINSECOND,
                     (milliseconds % MILLISINSECOND) * NANOSINMILLI};
  }

  __attribute__((__always_inline__)) static Timestamp
  FromMicroSeconds(time_t microseconds) {
    return Timestamp{microseconds / MICROSINSECOND,
                     (microseconds % MICROSINSECOND) * NANOSINMICRO};
  }

  __attribute__((__always_inline__)) static Timestamp
  FromSeconds(time_t seconds) {
    return Timestamp{seconds, 0};
  }

  static Timestamp FromString(std::string_view s) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    strptime(s.data(), "%Y-%m-%d %H:%M:%S", &tm);
    return Timestamp::FromSeconds(mktime(&tm));
  }
} __attribute__((packed));

static Timestamp GetTimestamp() {
  Timestamp ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts;
}

static void ToLocalTime(const Timestamp &ts_, struct tm &result_) {
  ::localtime_r(&ts_.tv_sec, &result_);
}

} // namespace ctptrader::base