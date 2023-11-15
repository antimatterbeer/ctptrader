#pragma once

#include <core/ctx.hpp>
#include <core/stg.hpp>

namespace ctptrader::logger {

class Logger : public core::IStrategy {
public:
  ~Logger() = default;

  void Init(toml::table &config) override {
    for (auto &i : *config["instruments"].as_array()) {
      auto inst = i.value<std::string>();
      if (inst.has_value()) {
        auto inst_id = GetContext()->GetInstrumentCenter().GetID(inst.value());
        if (inst_id > 0) {
          WatchInstrument(inst_id);
        }
      }
    }
    for (auto &a : *config["accounts"].as_array()) {
      auto acc = a.value<std::string>();
      if (acc.has_value()) {
        auto acc_id = GetContext()->GetAccountCenter().GetID(acc.value());
        if (acc_id > 0) {
          WatchAccount(acc_id);
        }
      }
    }
  }

  void OnStatic(const base::Static &st) override {
    GetContext()->GetLogger()->info("On static. Instrument: {}", st.id_);
  }

  void OnDepth(const base::Depth &depth) override {
    GetContext()->GetLogger()->info("On depth. Instrument: {}", depth.id_);
  }

  void OnBar(const base::Bar &bar) override {
    GetContext()->GetLogger()->info("On bar. Instrument: {}", bar.id_);
  }

  void OnBalance(const base::Balance &bal) override {
    GetContext()->GetLogger()->info("On balance. Account: {}", bal.id_);
  }
};

} // namespace ctptrader::logger