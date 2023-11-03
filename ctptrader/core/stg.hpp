#pragma once

#include <boost/noncopyable.hpp>
#include <toml.hpp>

#include <base/msg.hpp>
#include <core/ctx.hpp>

namespace ctptrader::core {

class IStrategy : public boost::noncopyable {
public:
  virtual ~IStrategy() = default;
  virtual void Init(toml::table &config) = 0;
  virtual void OnStatic(const base::Static &st) {}
  virtual void OnDepth(const base::Depth &depth) {}
  virtual void OnBar(const base::Bar &bar) {}
  virtual void OnBalance(const base::Balance &bal) {}

  void SetContext(Context *ctx) {
    ctx_ = ctx;
    ins_interests_.assign(ctx->InstrumentRef().Size(), 0);
    acc_interests_.assign(ctx->AccountRef().Size(), 0);
  }

  [[nodiscard]] bool WatchesInstrument(base::InstrumentID id) const {
    return ins_interests_[id] > 0;
  }

  [[nodiscard]] bool WatchesAccount(base::AccountID id) const {
    return acc_interests_[id] > 0;
  }

protected:
  [[nodiscard]] Context *GetContext() const { return ctx_; }

  void WatchInstrument(base::InstrumentID id) { ins_interests_[id] = 1; }

  void WatchAccount(base::AccountID id) { acc_interests_[id] = 1; }

private:
  Context *ctx_;
  std::vector<int> ins_interests_;
  std::vector<int> acc_interests_;
};

} // namespace ctptrader::core