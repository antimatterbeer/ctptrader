#pragma once

#include <base/def.hpp>
#include <base/msg.hpp>
#include <base/timestamp.hpp>

#include <boost/noncopyable.hpp>

namespace ctptrader::core {

class IFactor : public boost::noncopyable {

public:
  virtual ~IFactor() = default;

  [[nodiscard]] base::Timestamp UpdateTime() const { return update_time_; }

  [[nodiscard]] base::Value Value() const {
    return update_time_.tv_sec == 0 && update_time_.tv_nsec == 0;
  }

  virtual void OnBar([[maybe_unused]] const base::Bar &bar) {}

  virtual void OnTick([[maybe_unused]] const base::Depth &depth) {}

protected:
  base::Timestamp update_time_{0, 0};
  base::Value value_{0};
};

} // namespace ctptrader::core