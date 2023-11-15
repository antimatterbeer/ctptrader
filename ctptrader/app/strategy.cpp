#include "strategy.hpp"
#include <app/strategy.hpp>

namespace ctptrader::app {

bool StrategyManager::Init(toml::table &global_config,
                           toml::table &app_config) {
  auto data_folder = global_config["data_folder"].value_or("");
  if (!ctx_.Init(data_folder)) {
    return false;
  }
  for (auto &s : *app_config["stg"].as_array()) {
    auto stg_config = *s.as_table();
    auto name = stg_config["name"].value<std::string>();
    auto libpath = stg_config["libpath"].value<std::string>();
    if (name.has_value() && libpath.has_value()) {
      stgs_.emplace_back(name.value(), libpath.value());
      stgs_.back().Instance().SetContext(&ctx_);
      stgs_.back().Instance().Init(stg_config);
      ctx_.GetLogger()->info("Loaded strategy: {}", name.value());
    }
  }
  return true;
}

void StrategyManager::Run() {
  base::Msg msg;
  while (!stop_) {
    if (md_rx_.Read(msg)) {
      std::visit(
          [this](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, base::Static>) {
              OnStatic(arg);
            } else if constexpr (std::is_same_v<T, base::Bar>) {
              OnBar(arg);
            } else if constexpr (std::is_same_v<T, base::Depth>) {
              OnDepth(arg);
            } else if constexpr (std::is_same_v<T, base::Balance>) {
              OnBalance(arg);
            }
          },
          msg);
    }
  }
}

} // namespace ctptrader::app