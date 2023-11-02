#pragma once

#include <variant>

#include <base/date.hpp>
#include <base/def.hpp>
#include <base/timestamp.hpp>

namespace ctptrader::base {

struct alignas(8) Session {
  Date trading_day_;    // +4 bytes
  bool is_day_trading_; // +1 bytes
};
static_assert(sizeof(Session) == 8);

struct alignas(8) Bar {
  Date trading_day_;      // +4 bytes
  Timestamp update_time_; // +16 bytes
  InstrumentID id_;       // +4 bytes
  Price open_;            // +8 bytes
  Price high_;            // +8 bytes
  Price low_;             // +8 bytes
  Price close_;           // +8 bytes
  Volume volume_;         // +4 bytes
  Money turnover_;        // +8 bytes
};
static_assert(sizeof(Bar) == 80);

struct alignas(8) Static {
  Date trading_day_;  // +4 bytes
  InstrumentID id_;   // +4 bytes
  Price prev_close_;  // +8 bytes
  Price upper_limit_; // +8 bytes
  Price lower_limit_; // +8 bytes
};
static_assert(sizeof(Static) == 40);

struct alignas(8) Depth {
  Timestamp update_time_; // +16 bytes
  InstrumentID id_;       // +4 bytes
  Price open_;            // +8 bytes
  Price high_;            // +8 bytes
  Price low_;             // +8 bytes
  Price last_;            // +8 bytes
  Volume open_interest_;  // +4 bytes
  Volume volume_;         // +4 bytes
  Money turnover_;        // +8 bytes
  Price ask_price_[5];    // +40 bytes
  Price bid_price_[5];    // +40 bytes
  Volume ask_volume_[5];  // +20 bytes
  Volume bid_volume_[5];  // +20 bytes
};
static_assert(sizeof(Depth) == 192);

struct alignas(8) Balance {
  AccountID id_;    // +4 bytes
  Money balance_;   // +8 bytes
  Money available_; // +8 bytes
};
static_assert(sizeof(Balance) == 24);

struct alignas(8) NewOrder {
  Timestamp create_time_; // +16 bytes
  ID instrument_id_;      // +4 bytes
  ID client_id_;          // +4 bytes
  ID strategy_id_;        // +4 bytes
  PriceType price_type_;  // +4 bytes
  Direction direction_;   // +4 bytes
  Price price_;           // +8 bytes
  Volume volume_;         // +4 bytes
};

struct alignas(8) CancelOrder {
  Timestamp create_time_; // +16 bytes
  ID instrument_id_;      // +4 bytes
  ID client_id_;          // +4 bytes
  ID strategy_id_;        // +4 bytes
};

struct alignas(8) OrderUpdate {
  Timestamp update_time_; // +16 bytes
  ID instrument_id_;      // +4 bytes
  ID order_id_;           // +4 bytes
  ID client_id_;          // +4 bytes
  Price price_;           // +8 bytes
  Volume volume_;         // +4 bytes
  Volume filled_volume_;  // +4 bytes
  Volume pulled_volume_;  // +4 bytes
};

using Msg = std::variant<Bar, Static, Depth, Balance>;

} // namespace ctptrader::base