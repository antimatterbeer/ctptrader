#include <app/stgManager.hpp>

namespace ctptrader::app {

bool StgManager::Init(const toml::table &config) {
  LOG_INFO("[StgManager]Init");
  auto stgs = config["stg"].as_array();
  for (const auto &e : *stgs) {
    auto stg_config = e.as_table();
    auto name = (*stg_config)["name"].value<std::string>();
    auto libpath = (*stg_config)["libpath"].value<std::string>();
    stgs_.emplace_back(name.value(), libpath.value());
  }
  LOG_INFO("[StgManager]Add {} stgs", stgs_.size());
  return true;
}

void StgManager::Start() {
  base::Msg msg;
  while (true) {
    if (rx_.Read(msg)) {
      LOG_INFO("[StgManager]Read from channel");
      OnMsg(msg);
    }
  }
}

} // namespace ctptrader::app