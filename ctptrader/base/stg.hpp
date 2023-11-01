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
  virtual void OnStatic(const Static &st) {}
  virtual void OnDepth(const Depth &depth) {}
  virtual void OnBar(const Bar &bar) {}
  virtual void OnBalance(const Balance &bal) {}
  void SetContext(Context *ctx) {
    ctx_ = ctx;
    instrument_interest_.assign(ctx->InstrumentCenter().Size(), 0);
    account_interest_.assign(ctx->AccountCenter().Size(), 0);
  }
  [[nodiscard]] bool IsInstrumentInterested(ID id) const {
    return instrument_interest_[id] > 0;
  }
  [[nodiscard]] bool IsAccountInterested(ID id) const {
    return account_interest_[id] > 0;
  }

protected:
  [[nodiscard]] Context *GetContext() const { return ctx_; }
  void SetInstrumentInterest(ID id) { instrument_interest_[id] = 1; }
  void SetAccountInterest(ID id) { account_interest_[id] = 1; }

private:
  Context *ctx_;
  std::vector<int> instrument_interest_;
  std::vector<int> account_interest_;
};

} // namespace ctptrader::base