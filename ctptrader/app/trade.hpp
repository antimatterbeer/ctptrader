#pragma once

#include <ThostFtdcTraderApi.h>

#include <base/msg.hpp>
#include <core/app.hpp>
#include <core/ctx.hpp>
#include <util/channel.hpp>

namespace ctptrader::app {
class TraderSpi : public CThostFtdcTraderSpi {
public:
  TraderSpi(core::Context *ctx, CThostFtdcTraderApi *api,
            std::string_view trade_channel, std::string_view broker_id,
            std::string_view user_id, std::string_view password)
      : ctx_(ctx)
      , api_(api)
      , tx_(trade_channel)
      , broker_id_(broker_id)
      , user_id_(user_id)
      , password_(password) {}

  void OnFrontConnected() override;

  void OnFrontDisconnected(int nReason) override;

  void OnHeartBeatWarning(int nTimeLapse) override;

  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) override;

  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) override;

  void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

  void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) override;

  void
  OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast) override;

  void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) override;

  void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) override;

  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                  bool bIsLast) override;

  void OnRtnOrder(CThostFtdcOrderField *pOrder) override;

  void OnRtnTrade(CThostFtdcTradeField *pTrade) override;

private:
  core::Context *ctx_;
  CThostFtdcTraderApi *api_;
  util::ShmSpscWriter<base::Msg, 20> tx_;
  const std::string broker_id_;
  const std::string user_id_;
  const std::string password_;
};

class TradeManager : public core::IApp {
public:
  TradeManager(std::string_view trade_channel)
      : trade_channel_(trade_channel) {}

  bool Init(toml::table &global_config, toml::table &app_config) override;

  void Run() override;

private:
  const std::string trade_channel_;
  core::Context ctx_;
  std::string broker_id_;
  std::string user_id_;
  std::string password_;
  std::string trade_front_;
};

} // namespace ctptrader::app
