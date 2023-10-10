#include <app/stgManager.hpp>

namespace ctptrader::app {

bool StgInstance::Load(std::string_view libpath) {
  handle_ = dlopen(libpath.data(), RTLD_NOW | RTLD_NODELETE);
  if (!handle_) {
    std::cerr << dlerror() << std::endl;
    return false;
  }
  creator_ = reinterpret_cast<base::Stg::Creator>(dlsym(handle_, "creator"));
  if (!creator_) {
    std::cerr << dlerror() << std::endl;
    return false;
  }
  deleter_ = reinterpret_cast<base::Stg::Deleter>(dlsym(handle_, "deleter"));
  if (!deleter_) {
    std::cerr << dlerror() << std::endl;
    return false;
  }
  stg_ = creator_();
  return true;
}

bool StgManager::Init(const toml::table &config) {
  auto stgs = config["stgManager"]["stg"].as_array();
  for (const auto &e : *stgs) {
    auto stg_config = e.as_table();
    auto name = (*stg_config)["name"].value<std::string>();
    auto libpath = (*stg_config)["libpath"].value<std::string>();
    StgInstance stg;
    if (stg.Load(libpath.value()) && stg.Init(*stg_config)) {
      stgs_.push_back(std::move(stg));
    }
  }
  return true;
}

void StgManager::Start() {
  base::Msg msg;
  while (true) {
    if (rx_.Read(msg)) {
      std::visit(*this, msg);
    }
  }
}

} // namespace ctptrader::app