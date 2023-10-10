#pragma once

#include <base/stg.hpp>
#include <service/dataService.hpp>

namespace ctptrader::demo {

class Logger : public base::Stg {
public:
  ~Logger() = default;
  bool Init(const toml::table &config) override {
    LOG_INFO("[Logger]Init");
    return true;
  }
  void OnSession(const base::Session &session) override {
    LOG_INFO("[Logger]OnSession");
  }
  void OnBar(const base::Bar &bar) override {
    LOG_INFO("[Logger]OnBar. open: {} high: {} low: {} close: {} volume: {} "
             "turnover: {}",
             bar.open_, bar.high_, bar.low_, bar.close_, bar.volume_,
             bar.turnover_);
  }
  void OnStatic(const base::Static &st) override {
    LOG_INFO("[Logger]OnStatic. instrument_id: {} prev_close: {}",
             st.instrument_id_, st.prev_close_);
  }
  void OnDepth(const base::Depth &depth) override {
    LOG_INFO("[Logger]OnDepth. instrument_id: {} update_time: {}",
             depth.instrument_id_, depth.update_time_.ToString());
  }
  void OnEndOfDay(const base::Date &Date) override {
    LOG_INFO("[Logger]OnEndOfDay. date: {}", Date.ToString());
  }
};

} // namespace ctptrader::demo