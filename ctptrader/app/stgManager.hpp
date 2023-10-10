#pragma once

#include <dlfcn.h>
#include <iostream>

#include <base/msg.hpp>
#include <base/stg.hpp>
#include <util/channel.hpp>

namespace ctptrader::app {

class StgInstance {
public:
  ~StgInstance() {
    if (handle_) {
      dlclose(handle_);
    }
    if (stg_) {
      deleter_(stg_);
    }
  }
  bool Load(std::string_view libpath);
  bool Init(const toml::table &config) { return stg_->Init(config); }
  void OnSession(const base::Session &session) { stg_->OnSession(session); }
  void OnBar(const base::Bar &bar) { stg_->OnBar(bar); }
  void OnStatic(const base::Static &st) { stg_->OnStatic(st); }
  void OnDepth(const base::Depth &depth) { stg_->OnDepth(depth); }
  void OnEndOfDay(const base::Date &Date) { stg_->OnEndOfDay(Date); }
  bool IsInterested(base::ID instrument_id) {
    return stg_->IsInterested(instrument_id);
  }

private:
  void *handle_;
  base::Stg::Ptr stg_;
  base::Stg::Creator creator_;
  base::Stg::Deleter deleter_;
};

class StgManager : public base::NonCopyable {
public:
  StgManager(std::string_view market_channel) : rx_(market_channel) {}
  ~StgManager() = default;

  bool Init(const toml::table &config);

  void Start();

  void operator()(const base::Bar &bar) {
    for (auto &stg : stgs_) {
      if (stg.IsInterested(bar.instrument_id_)) {
        stg.OnBar(bar);
      }
    }
  }

  void operator()(const base::Static &st) {
    for (auto &stg : stgs_) {
      if (stg.IsInterested(st.instrument_id_)) {
        stg.OnStatic(st);
      }
    }
  }

  void operator()(const base::Depth &depth) {
    for (auto &stg : stgs_) {
      if (stg.IsInterested(depth.instrument_id_)) {
        stg.OnDepth(depth);
      }
    }
  }

private:
  std::vector<StgInstance> stgs_;
  util::ShmSpscReader<base::Msg, 1024> rx_;
};

} // namespace ctptrader::app