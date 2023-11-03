#pragma once

#include <core/ctx.hpp>
#include <core/stg.hpp>
#include <util/channel.hpp>
#include <util/proxy.hpp>

namespace ctptrader::app {

class StgManager {
public:
  StgManager(core::Context &&ctx, std::string_view market_channel)
      : ctx_(std::move(ctx))
      , md_rx_(market_channel) {}

  void AddStrategy(toml::table &config) {
    auto name = config["name"].value<std::string>();
    auto libpath = config["libpath"].value<std::string>();
    if (!name.has_value() || !libpath.has_value()) {
      ctx_.Logger()->error("Failed to load strategy.");
      return;
    }
    stgs_.emplace_back(name.value(), libpath.value());
    stgs_.back().Instance().SetContext(&ctx_);
    stgs_.back().Instance().Init(config);
    ctx_.Logger()->info("Loaded strategy: {}", name.value());
  }

  void OnMsg(const base::Msg &msg) { std::visit(*this, msg); }

  void operator()(const base::Static &st) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(st.id_)) {
        s.Instance().OnStatic(st);
      }
    }
    ctx_.OnStatic(st);
  }

  void operator()(const base::Bar &bar) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(bar.id_)) {
        s.Instance().OnBar(bar);
      }
    }
    ctx_.OnBar(bar);
  }

  void operator()(const base::Depth &depth) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesInstrument(depth.id_)) {
        s.Instance().OnDepth(depth);
      }
    }
    ctx_.OnDepth(depth);
  }

  void operator()(const base::Balance &bal) {
    for (auto &s : stgs_) {
      if (s.Instance().WatchesAccount(bal.id_)) {
        s.Instance().OnBalance(bal);
      }
    }
    ctx_.OnBalance(bal);
  }

  void Start() {
    base::Msg msg;
    while (true) {
      if (md_rx_.Read(msg)) {
        OnMsg(msg);
      }
    }
  }

private:
  core::Context ctx_;
  util::ShmSpscReader<base::Msg, 20> md_rx_;
  std::vector<util::Proxy<core::IStrategy>> stgs_;
};

inline int start_strategy(toml::table &global_config, toml::table &app_config) {
  std::string market_channel = global_config["market_channel"].value_or("");
  std::string data_folder = global_config["data_folder"].value_or("");
  auto stgs = *app_config["stg"].as_array();
  core::Context ctx;
  if (!ctx.Init(data_folder)) {
    std::cerr << "Failed to initialize context.\n";
    return 1;
  }
  StgManager app(std::move(ctx), market_channel);
  for (auto &s : stgs) {
    auto stg = *s.as_table();
    app.AddStrategy(stg);
  }
  app.Start();
  return 0;
}

} // namespace ctptrader::app