#include "trade.hpp"
#include <app/trade.hpp>

namespace ctptrader::app {

void TraderSpi::OnFrontConnected() {}

void TraderSpi::OnFrontDisconnected(int nReason) {}

void TraderSpi::OnHeartBeatWarning(int nTimeLapse) {}

void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {}

void TraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                 CThostFtdcRspInfoField *pRspInfo,
                                 int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspOrderAction(
    CThostFtdcInputOrderActionField *pInputOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast) {}

void TraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {}

void TraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) {}

bool TradeManager::Init(toml::table &global_config, toml::table &app_config) {
  auto data_folder = global_config["data_folder"].value_or("");
  if (!ctx_.Init(data_folder)) {
    return false;
  }
  broker_id_ = app_config["broker_id"].value_or("");
  user_id_ = app_config["user_id"].value_or("");
  password_ = app_config["password"].value_or("");
  trade_front_ = app_config["front"].value_or("");
  if (broker_id_.empty() || user_id_.empty() || password_.empty() ||
      trade_front_.empty()) {
    return false;
  }
  return false;
}

void TradeManager::Run() {
  auto front_address = fmt::format("tcp://{}", trade_front_);
  CThostFtdcTraderApi *api = CThostFtdcTraderApi::CreateFtdcTraderApi();
  TraderSpi spi(&ctx_, api, trade_channel_, broker_id_, user_id_, password_);
  api->RegisterSpi(&spi);
  api->RegisterFront((char *)front_address.c_str());
  api->Init();
  api->Join();
  api->Release();
}

} // namespace ctptrader::app