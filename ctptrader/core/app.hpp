#pragma once

#include <string_view>

#include <boost/noncopyable.hpp>

#include <core/ctx.hpp>

namespace ctptrader::core {

class IApp : public boost::noncopyable {
public:
  virtual bool Init(toml::table &global_config, toml::table &app_config) = 0;

  virtual void Run() = 0;

protected:
  core::Context ctx_;
};

}; // namespace ctptrader::core