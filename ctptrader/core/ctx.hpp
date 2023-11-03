#pragma once

#include <string_view>

#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <base/msg.hpp>
#include <base/ref.hpp>
#include <core/buf_center.hpp>
#include <core/ref_center.hpp>

namespace ctptrader::core {

/**
 * @brief The Context class represents the trading context, which contains
 * various data buffers and references.
 */
class Context {
public:
  Context()
      : logger_(spdlog::default_logger()) {}

  /**
   * @brief Initializes the context with the specified data folder.
   *
   * @param data_folder The path to the data folder.
   * @return true if initialization succeeds, false otherwise.
   */
  bool Init(std::string_view config_path);

  /**
   * @brief Callback function for handling static data.
   *
   * @param st The static data to be handled.
   */
  void OnStatic(const base::Static &st) { st_center_.Push(st); }

  /**
   * @brief Callback function for receiving market depth data.
   *
   * @param depth The market depth data received.
   */
  void OnDepth(const base::Depth &depth) { depth_center_.Push(depth); }

  /**
   * @brief Callback function for receiving bar data.
   *
   * @param bar The bar data received.
   */
  void OnBar(const base::Bar &bar) { bar_center_.Push(bar); }

  /**
   * @brief Callback function for balance update.
   *
   * @param bal The updated balance information.
   */
  void OnBalance(const base::Balance &bal) { bal_center_.Push(bal); }

  /**
   * @brief Returns the static data buffer.
   *
   * @return The static data buffer.
   */
  const BufCenter<base::Static, 1> &StaticCenter() const { return st_center_; }

  /**
   * @brief Returns the bar data buffer.
   *
   * @return The bar data buffer.
   */
  const BufCenter<base::Bar, 240> &BarCenter() const { return bar_center_; }

  /**
   * @brief Returns the depth data buffer.
   *
   * @return The depth data buffer.
   */
  const BufCenter<base::Depth, 2> &DepthCenter() const { return depth_center_; }

  /**
   * @brief Returns the balance data buffer.
   *
   * @return The balance data buffer.
   */
  const BufCenter<base::Balance, 2> &BalanceCenter() const {
    return bal_center_;
  }

  /**
   * @brief Returns the account data reference.
   *
   * @return The account data reference.
   */
  const RefCenter<base::Account> &AccountCenter() const { return acc_center_; }

  /**
   * @brief Returns a reference to the calendar date.
   *
   * @return The calendar date reference.
   */
  const RefCenter<base::CalendarDate> &CalendarCenter() const {
    return cal_center_;
  }

  /**
   * @brief Returns the underlying data reference.
   *
   * @return The underlying data reference.
   */
  const RefCenter<base::Underlying> &UnderlyingCenter() const {
    return uly_center_;
  }

  /**
   * @brief Returns the instrument data reference.
   *
   * @return The instrument data reference.
   */
  const RefCenter<base::Instrument> &InstrumentCenter() const {
    return ins_center_;
  }

  const std::shared_ptr<spdlog::logger> Logger() const { return logger_; }

private:
  std::shared_ptr<spdlog::logger> logger_;
  BufCenter<base::Static, 1> st_center_;
  BufCenter<base::Bar, 240> bar_center_;
  BufCenter<base::Depth, 2> depth_center_;
  BufCenter<base::Balance, 2> bal_center_;
  RefCenter<base::Instrument> ins_center_;
  RefCenter<base::Underlying> uly_center_;
  RefCenter<base::Account> acc_center_;
  RefCenter<base::CalendarDate> cal_center_;
};

} // namespace ctptrader::core