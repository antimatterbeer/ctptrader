#pragma once

#include <dlfcn.h>
#include <iostream>

namespace ctptrader::util {

template <typename T> class Proxy {
public:
  using Creator = T *(*)();

  Proxy(std::string_view name, std::string_view libpath)
      : name_(name)
      , libpath_(libpath) {
    handle_ = dlopen(libpath_.c_str(), RTLD_NOW | RTLD_NODELETE);
    if (!handle_) {
      std::cerr << "Failed to load library: " << libpath_ << std::endl;
      return;
    }
    auto creator = (Creator)dlsym(handle_, "creator");
    if (!creator) {
      std::cerr << "Failed to load creator from library: " << libpath_
                << std::endl;
      return;
    }
    instance_ = creator();
    if (!instance_) {
      std::cerr << "Failed to create instance from library: " << libpath_
                << std::endl;
      return;
    }
  }

  ~Proxy() {
    if (handle_) {
      dlclose(handle_);
    }
    delete instance_;
  }

  [[nodiscard]] bool IsValid() const { return instance_ != nullptr; }

  T &Instance() { return *instance_; }

private:
  const std::string name_;
  const std::string libpath_;
  void *handle_{};
  T *instance_{};
};

} // namespace ctptrader::util