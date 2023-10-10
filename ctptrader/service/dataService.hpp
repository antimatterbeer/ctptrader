#pragma once

#include <filesystem>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include <service/ref.hpp>

namespace ctptrader::service {

template <typename T> class RefCenter {
  static_assert(std::is_same<decltype(T::id_), base::ID>::value,
                "T must have an int field id_");
  static_assert(std::is_same<decltype(T::name_), std::string>::value,
                "T must have a string field name_");

public:
  bool FromCSV(std::string_view path);

  void Clear() {
    vec_.clear();
    id_map_.clear();
  }

  int Size() const { return vec_.size(); }

  bool Has(base::ID id) const { return id > 0 && id < Size(); }

  bool Has(std::string_view name) const {
    return id_map_.count(name.data()) > 0;
  }

  base::ID GetID(std::string_view name) const {
    return id_map_.at(name.data());
  }

  const T &Get(base::ID id) const { return vec_[id]; }

  const T &Get(std::string_view name) const { return Get(GetID(name)); }

private:
  std::vector<T> vec_;
  std::unordered_map<std::string, base::ID> id_map_;
};

template <> bool RefCenter<Underlying>::FromCSV(std::string_view path);
template <> bool RefCenter<Instrument>::FromCSV(std::string_view path);
template <> bool RefCenter<CalendarDate>::FromCSV(std::string_view path);

class Interest {
public:
  void Reset(int size) { interests_.assign(size, false); }
  void SetInterest(base::ID id) { interests_[id] = true; }
  bool IsInterested(base::ID id) const { return interests_[id]; }
  int Size() const { return interests_.size(); }

private:
  std::vector<bool> interests_;
};

class DataService {
public:
  static DataService &Get() {
    static DataService ds;
    return ds;
  }

  DataService() : logger_(spdlog::default_logger()) {}
  ~DataService() = default;

  bool Init(std::string_view folder);

  const RefCenter<Underlying> &UnderlyingCenter() const {
    return underlying_center_;
  }

  const RefCenter<Instrument> &InstrumentCenter() const {
    return instrument_center_;
  }

  const RefCenter<CalendarDate> &CalendarCenter() const {
    return calendar_center_;
  }

  const std::shared_ptr<spdlog::logger> &Logger() const { return logger_; }

private:
  std::shared_ptr<spdlog::logger> logger_;
  RefCenter<Underlying> underlying_center_;
  RefCenter<Instrument> instrument_center_;
  RefCenter<CalendarDate> calendar_center_;
};
} // namespace ctptrader::service

namespace ctptrader {
inline service::DataService &DataService() {
  return service::DataService::Get();
}

inline const service::RefCenter<service::Underlying> &UnderlyingCenter() {
  return DataService().UnderlyingCenter();
}

inline const service::RefCenter<service::Instrument> &InstrumentCenter() {
  return DataService().InstrumentCenter();
}

inline const service::RefCenter<service::CalendarDate> &CalendarDateCenter() {
  return DataService().CalendarCenter();
}

inline const std::shared_ptr<spdlog::logger> &Logger() {
  return DataService().Logger();
}

#define LOG_TRACE(...) ctptrader::Logger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) ctptrader::Logger()->debug(__VA_ARGS__)
#define LOG_INFO(...) ctptrader::Logger()->info(__VA_ARGS__)
#define LOG_WARN(...) ctptrader::Logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ctptrader::Logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ctptrader::Logger()->critical(__VA_ARGS__)

} // namespace ctptrader
