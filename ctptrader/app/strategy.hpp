#pragma once

#include <core/app.hpp>
#include <core/ctx.hpp>
#include <core/stg.hpp>
#include <util/channel.hpp>
#include <util/proxy.hpp>

namespace ctptrader::app {

class StrategyManager : public core::IApp {
public:
  StrategyManager(std::string_view market_channel)
      : md_rx_(market_channel) {}

  bool Init(toml::table &global_config, toml::table &app_config) override;

  void Run() override;

private:
  void OnStatic(const base::Static &st) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(st.id_)) {
        s.Instance().OnStatic(st);
      }
    }
    ctx_.OnStatic(st);
  }

  void OnBar(const base::Bar &bar) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(bar.id_)) {
        s.Instance().OnBar(bar);
      }
    }
    ctx_.OnBar(bar);
  }

  void OnDepth(const base::Depth &depth) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(depth.id_)) {
        s.Instance().OnDepth(depth);
      }
    }
    ctx_.OnDepth(depth);
  }

  void OnBalance(const base::Balance &bal) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesAccount(bal.id_)) {
        s.Instance().OnBalance(bal);
      }
    }
    ctx_.OnBalance(bal);
  }

private:
  util::ShmSpscReader<base::Msg, 20> md_rx_;
  std::vector<util::Proxy<core::IStrategy>> stgs_;
  bool stop_ = false;
};

} // namespace ctptrader::app