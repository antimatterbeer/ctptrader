#pragma once

#include <string_view>

#include <base/msg.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::quant {

class IFactor {
public:
  virtual ~IFactor() {}
  virtual void OnSession(const base::Session &session) {}
  virtual void OnBar(const base::Bar &bar) {}
  virtual void OnStatic(const base::Static &st) {}
  virtual void OnDepth(const base::Depth &depth) {}

  bool HasValue() const { return update_time_ != 0; }
  double Value() const { return value_; }
  base::Timestamp UpdateTime() const { return update_time_; }

protected:
  double value_{0};
  base::Timestamp update_time_{0};
};

} // namespace ctptrader::quant