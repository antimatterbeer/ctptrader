#pragma once

#include <core/ctx.hpp>
#include <core/stg.hpp>

namespace ctptrader::logger {

class Logger : public core::IStrategy {
public:
  ~Logger() = default;

  void Init(toml::table& config) override {
    auto inst_id = GetContext()->InstrumentRef().GetID("cu2311");
    WatchInstrument(inst_id);
    auto acc_id = GetContext()->AccountRef().GetID("test001");
    WatchAccount(acc_id);
  }

  void OnStatic(const base::Static &st) override {
    GetContext()->Logger()->info("On static");
  }

  void OnDepth(const base::Depth &depth) override {
    GetContext()->Logger()->info("On depth");
  }

  void OnBar(const base::Bar &bar) override {
    GetContext()->Logger()->info("On bar");
  }

  void OnBalance(const base::Balance &bal) override {
    GetContext()->Logger()->info("On balance");
  }
};

} // namespace ctptrader::logger