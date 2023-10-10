#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ctptrader::base {

class NonCopyable {
public:
  NonCopyable() {}

protected:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable(NonCopyable &&) = delete;
  NonCopyable &operator=(NonCopyable &&) = delete;
};

/// @brief ID type
using ID = int64_t;

/// @brief Price type
using Price = double;

/// @brief Volume type
using Volume = int;

/// @brief Large volume type
using LargeVolume = long;

/// @brief Money type
using Money = double;

/// @brief Multiple type
using Mutiple = int;

/// @brief Ratio type
using Ratio = double;

/// @brief 交易所
enum Exchange {
  Exchange_Invalid = -1,
  /// @brief 上交所
  Exchange_XSHG,
  /// @brief 深交所
  Exchange_XSHE,
  /// @brief 上期所
  Exchange_SHFE,
  /// @brief 大商所
  Exchange_DCE,
  /// @brief 郑商所
  Exchange_CZCE,
  /// @brief 中金所
  Exchange_FFEX,
  /// @brief 上海国际能源交易中心
  Exchange_INE,
  /// @brief 广期所
  Exchange_GFEX
};

const static std::unordered_map<std::string, Exchange> ExchangeMap = {
    {"XSHG", Exchange_XSHG}, {"XSHE", Exchange_XSHE}, {"SHFE", Exchange_SHFE},
    {"DCE", Exchange_DCE},   {"CZCE", Exchange_CZCE}, {"FFEX", Exchange_FFEX},
    {"INE", Exchange_INE},   {"GFEX", Exchange_GFEX}};

/// @brief 合约类型
enum InstrumentType {
  /// @brief 无效
  InstrumentType_Invalid = -1,
  /// @brief 股票
  InstrumentType_Stock,
  /// @brief 指数
  InstrumentType_Index,
  /// @brief 交易所交易基金
  InstrumentType_ETF,
  /// @brief 上市型开放式基金
  InstrumentType_LOF,
  /// @brief 可转债
  InstrumentType_Convertable,
  /// @brief 现货
  InstrumentType_Spot,
  /// @brief 期货
  InstrumentType_Futures,
  /// @brief 期权
  InstrumentType_Options,
  /// @brief 回购
  InstrumentType_Repo,
};

const static std::unordered_map<std::string, InstrumentType> InstrumentTypeMap =
    {{"stock", InstrumentType_Stock},
     {"index", InstrumentType_Index},
     {"etf", InstrumentType_ETF},
     {"lof", InstrumentType_LOF},
     {"convertable", InstrumentType_Convertable},
     {"spot", InstrumentType_Spot},
     {"futures", InstrumentType_Futures},
     {"options", InstrumentType_Options},
     {"repo", InstrumentType_Repo}};

/// @brief 订单方向
enum Direction {
  Direction_Invalid = -1,
  /// @brief 买
  Direction_Buy,
  /// @brief 卖
  Direction_Sell
};

const static std::unordered_map<std::string, Direction> DirectionMap = {
    {"buy", Direction_Buy}, {"sell", Direction_Sell}};

/// @brief 价格类型
enum PriceType {
  /// @brief 无效
  PriceType_Invalid = -1,
  /// @brief 市价
  PriceType_Market,
  /// @brief 限价
  PriceType_Limit,
};

const static std::unordered_map<std::string, PriceType> PriceTypeMap = {
    {"market", PriceType_Market}, {"limit", PriceType_Limit}};

enum OrderStatus {
  /// @brief 无效
  OrderStatus_Invalid = -1,
  /// @brief 全部成交
  OrderStatus_AllTraded,
  /// @brief 部分成交还在队列中
  OrderStatus_PartTradedQueueing,
  /// @brief 部分成交不在队列中
  OrderStatus_PartTradedNotQueueing,
  /// @brief 未成交还在队列中
  OrderStatus_NoTradeQueueing,
  /// @brief 未成交不在队列中
  OrderStatus_NoTradeNotQueueing,
  /// @brief 撤单
  OrderStatus_Canceled,
  /// @brief 未知
  OrderStatus_Unknown,
  /// @brief 尚未触发
  OrderStatus_NotTouched,
  /// @brief 已触发
  OrderStatus_Touched
};

const static std::unordered_map<std::string, OrderStatus> OrderStatusMap = {
    {"all_traded", OrderStatus_AllTraded},
    {"part_traded_queueing", OrderStatus_PartTradedQueueing},
    {"part_traded_not_queueing", OrderStatus_PartTradedNotQueueing},
    {"no_trade_queueing", OrderStatus_NoTradeQueueing},
    {"no_trade_not_queueing", OrderStatus_NoTradeNotQueueing},
    {"canceled", OrderStatus_Canceled},
    {"unknown", OrderStatus_Unknown},
    {"not_touched", OrderStatus_NotTouched},
    {"touched", OrderStatus_Touched},
};

enum Currency {
  Currency_Invalid = -1,
  /// @brief 人民币
  Currency_CNY,
  /// @brief 离岸人民币
  Currency_CHN,
  /// @brief 美元
  Currency_USD,
  /// @brief 港币
  Currency_HKD,
};

const static std::unordered_map<std::string, Currency> CurrencyMap = {
    {"CNY", Currency_CNY},
    {"CHN", Currency_CHN},
    {"USD", Currency_USD},
    {"HKD", Currency_HKD},
};

} // namespace ctptrader::base