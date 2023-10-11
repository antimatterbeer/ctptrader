#include "stgManager.hpp"
#include <app/stgManager.hpp>

namespace ctptrader::app {

StgInstance::StgInstance(std::string_view libpath) {
  handle_ = dlopen(libpath.data(), RTLD_NOW | RTLD_NODELETE);
  if (!handle_) {
    std::cerr << dlerror() << std::endl;
    return;
  }
  creator_ = reinterpret_cast<base::Stg::Creator>(dlsym(handle_, "creator"));
  if (!creator_) {
    std::cerr << dlerror() << std::endl;
    return;
  }
  deleter_ = reinterpret_cast<base::Stg::Deleter>(dlsym(handle_, "deleter"));
  if (!deleter_) {
    std::cerr << dlerror() << std::endl;
    return;
  }
  stg_ = creator_();
}

bool StgManager::Init(const toml::table &config) {
  LOG_INFO("[StgManager]Init");
  auto stgs = config["stg"].as_array();
  for (const auto &e : *stgs) {
    auto stg_config = e.as_table();
    auto name = (*stg_config)["name"].value<std::string>();
    auto libpath = (*stg_config)["libpath"].value<std::string>();
    stgs_.emplace_back(libpath.value());
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