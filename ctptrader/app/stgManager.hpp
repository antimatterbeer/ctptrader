#pragma once

#include <dlfcn.h>
#include <iostream>

#include <base/msg.hpp>
#include <base/stg.hpp>
#include <util/channel.hpp>

#include <service/dataService.hpp>

namespace ctptrader::app {

class StgInstance {
public:
  StgInstance(std::string_view libpath);

  ~StgInstance() {
    if (stg_) {
      deleter_(stg_);
    }
    if (handle_) {
      dlclose(handle_);
    }
  }
  bool Load(std::string_view libpath);
  bool Init(const toml::table &config) { return stg_->Init(config); }
  void OnSession(const base::Session &session) { stg_->OnSession(session); }
  void OnBar(const base::Bar &bar) { stg_->OnBar(bar); }
  void OnStatic(const base::Static &st) { stg_->OnStatic(st); }
  void OnDepth(const base::Depth &depth) { stg_->OnDepth(depth); }
  void OnEndOfDay(const base::Date &Date) { stg_->OnEndOfDay(Date); }

private:
  void *handle_{};
  base::Stg::Ptr stg_{};
  base::Stg::Creator creator_{};
  base::Stg::Deleter deleter_{};
};

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
      stg.OnBar(bar);
    }
  }

  void operator()(const base::Static &st) {
    LOG_INFO("[StgManager]OnStatic. instrument_id: {}", st.instrument_id_);
    for (auto &stg : stgs_) {
      stg.OnStatic(st);
    }
  }

  void operator()(const base::Depth &depth) {
    LOG_INFO("[StgManager]OnDepth. instrument_id: {}", depth.instrument_id_);
    for (auto &stg : stgs_) {
      stg.OnDepth(depth);
    }
  }

private:
  std::vector<StgInstance> stgs_;
  util::MsgReceiver rx_;
};

} // namespace ctptrader::app