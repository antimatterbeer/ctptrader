#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <toml.hpp>

#include <base/context.hpp>
#include <base/def.hpp>
#include <base/msg.hpp>

namespace ctptrader::base {

class IStrategy : public boost::noncopyable {
public:
  virtual ~IStrategy() = default;

  virtual void Init() = 0;

  virtual void OnStatic(const Static &static_data) {}

  virtual void OnDepth(const Depth &depth_data) {}

  virtual void OnBar(const Bar &bar_data) {}

  void SetContext(Context *context) {
    context_ = context;
    instrument_interest_.resize(context_->GetSymbols().size(), 0);
  }

  bool IsInterested(ID id) const { return instrument_interest_[id] > 0; }

protected:
  Context *GetContext() const { return context_; }
  void SetInterest(ID id) { instrument_interest_[id] = 1; }

private:
  Context *context_;
  std::vector<int> instrument_interest_;
};

} // namespace ctptrader::base