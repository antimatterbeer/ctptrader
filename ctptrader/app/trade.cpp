#include "trade.hpp"
#include <app/trade.hpp>

namespace ctptrader::app {

void TraderSpi::OnFrontConnected() {
  ctx_->Logger()->info("Connected to trade front. Sending login request.");
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, broker_id_.c_str());
  strcpy(req.UserID, user_id_.c_str());
  strcpy(req.Password, password_.c_str());
  if (api_->ReqUserLogin(&req, 0) != 0) {
    ctx_->Logger()->error("Sending login request failed");
  } else {
    ctx_->Logger()->info("Sending login request succeeded");
  }
}

void TraderSpi::OnFrontDisconnected(int nReason) {
  ctx_->Logger()->info("Disconnected from trade front, reason: {}", nReason);
}

void TraderSpi::OnHeartBeatWarning(int nTimeLapse) {
  ctx_->Logger()->info("Heartbeat warning, time lapse: {}", nTimeLapse);
}

void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {

    ctx_->Logger()->info("Login succeeded. Confirming settlement info");
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, broker_id_.c_str());
    strcpy(req.InvestorID, user_id_.c_str());
    if (api_->ReqSettlementInfoConfirm(&req, 0) != 0) {
      ctx_->Logger()->error("Sending settlement info confirm request failed");
    } else {
      ctx_->Logger()->info("Sending settlement info confirm request succeeded");
    }
  } else {
    ctx_->Logger()->error("Login failed, error id: {}, error message: {}",
                          pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void TraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_->Logger()->info("Logout succeeded");
  } else {
    ctx_->Logger()->error("Logout failed, error id: {}, error message: {}",

                          pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void TraderSpi::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TraderSpi::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  base::Balance balance{
      ctx_->AccountCenter().GetID(pTradingAccount->AccountID),
      pTradingAccount->Available,
      pTradingAccount->Balance,
      pTradingAccount->CurrMargin,
      pTradingAccount->FrozenMargin,
  };
  if (!tx_.Write(balance)) {
    ctx_->Logger()->error("Write balance failed");
  }
}

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
  TraderSpi spi(&ctx_, api, rsp_channel_, broker_id_, user_id_, password_);

  std::thread t([&]() {
    util::ShmSpscReader<base::Msg, 20> rx(rsp_channel_);
    base::Msg msg;
    while (!stop_) {
      if (rx.Read(msg)) {
        /* TODO */
      }
    }
  });

  api->RegisterSpi(&spi);
  api->RegisterFront((char *)front_address.c_str());
  api->Init();
  api->Join();
  api->Release();
}

} // namespace ctptrader::app