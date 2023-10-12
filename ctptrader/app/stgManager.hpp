#pragma once

#include <dlfcn.h>
#include <iostream>

#include <base/msg.hpp>
#include <base/stg.hpp>
#include <util/channel.hpp>

#include <service/dataService.hpp>
#include <util/proxy.hpp>

namespace ctptrader::app {

class StgManager : public base::NonCopyable {
public:
  StgManager(std::string_view market_channel)
      : rx_(market_channel) {}
  ~StgManager() = default;

  bool Init(const toml::table &config);

  void Start();

  void OnMsg(const base::Msg &msg) { std::visit(*this, msg); }

  void operator()(const base::Bar &bar) {
    LOG_INFO("[StgManager]OnBar. instrument_id: {}", bar.instrument_id_);
    for (auto &stg : stgs_) {
      if (stg.IsValid()) {
        stg.Instance().OnBar(bar);
      }
    }
  }

  void operator()(const base::Static &st) {
    LOG_INFO("[StgManager]OnStatic. instrument_id: {}", st.instrument_id_);
    for (auto &stg : stgs_) {
      if (stg.IsValid()) {
        stg.Instance().OnStatic(st);
      }
    }
  }

  void operator()(const base::Depth &depth) {
    LOG_INFO("[StgManager]OnDepth. instrument_id: {}", depth.instrument_id_);
    for (auto &stg : stgs_) {
      if (stg.IsValid()) {
        stg.Instance().OnDepth(depth);
      }
    }
  }

private:
  std::vector<util::Proxy<base::Stg>> stgs_;
  util::MsgReceiver rx_;
};

} // namespace ctptrader::app