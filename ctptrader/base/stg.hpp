#pragma once

#include <vector>

#include <toml.hpp>

#include <base/def.hpp>
#include <base/msg.hpp>

namespace ctptrader::base {

class Stg : public NonCopyable {
public:
  using Ptr = Stg *;
  using Creator = Stg *(*)();
  using Deleter = void (*)(Stg *);

  virtual ~Stg() = default;
  virtual bool Init(const toml::table &config) = 0;
  virtual void OnSession(const Session &session) {}
  virtual void OnBar(const Bar &bar) {}
  virtual void OnStatic(const Static &st) {}
  virtual void OnDepth(const Depth &depth) {}
  virtual void OnEndOfDay(const Date &Date) {}
};

} // namespace ctptrader::base