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
  virtual void OnStatic([[maybe_unused]] const base::Static &st) {}
  virtual void OnDepth([[maybe_unused]] const base::Depth &depth) {}
  virtual void OnBar([[maybe_unused]] const base::Bar &bar) {}
  virtual void OnBalance([[maybe_unused]] const base::Balance &bal) {}

  /**
   * @brief Sets the context for the strategy.
   *
   * @param ctx A pointer to the context object.
   */
  void SetContext(Context *ctx) {
    ctx_ = ctx;
    ins_interests_.assign(ctx->InstrumentCenter().Count(), 0);
    acc_interests_.assign(ctx->AccountCenter().Count(), 0);
  }

  /**
   * @brief Checks if the strategy is currently watching the given instrument.
   *
   * @param id The ID of the instrument to check.
   * @return true if the strategy is watching the instrument, false otherwise.
   */
  [[nodiscard]] bool WatchesInstrument(base::InstrumentID id) const {
    return ins_interests_[id] > 0;
  }

  /**
   * @brief Checks if the strategy is watching the given account.
   *
   * @param id The ID of the account to check.
   * @return true if the strategy is watching the account, false otherwise.
   */
  [[nodiscard]] bool WatchesAccount(base::AccountID id) const {
    return acc_interests_[id] > 0;
  }

protected:
  /**
   * @brief Get the context object associated with this strategy.
   *
   * @return Context* A pointer to the context object.
   */
  [[nodiscard]] Context *GetContext() const { return ctx_; }

  /**
   * @brief Adds an instrument to the list of instruments to be watched by the
   * strategy.
   *
   * @param id The ID of the instrument to be watched.
   */
  void WatchInstrument(base::InstrumentID id) { ins_interests_[id] = 1; }

  /**
   * @brief Adds the specified account ID to the list of accounts to watch for
   * updates.
   *
   * @param id The ID of the account to watch.
   */
  void WatchAccount(base::AccountID id) { acc_interests_[id] = 1; }

private:
  Context *ctx_;
  std::vector<int> ins_interests_;
  std::vector<int> acc_interests_;
};

} // namespace ctptrader::core