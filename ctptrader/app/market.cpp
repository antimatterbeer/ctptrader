#include <app/market.hpp>

namespace ctptrader::app {

void MdSpi::OnFrontConnected() {
  ctx_.Logger()->info("Connected to market server, sending login request");
  CThostFtdcReqUserLoginField req{};
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, broker_id_.c_str());
  strcpy(req.UserID, user_id_.c_str());
  strcpy(req.Password, password_.c_str());
  if (api_->ReqUserLogin(&req, 0) != 0) {
    ctx_.Logger()->error("Sending login request failed");
  } else {
    ctx_.Logger()->info("Sending login request succeeded");
  }
}

void MdSpi::OnFrontDisconnected(int nReason) {
  ctx_.Logger()->info("Disconnected from market server, reason: {}", nReason);
}

void MdSpi::OnHeartBeatWarning(int nTimeLapse) {
  ctx_.Logger()->info("Heartbeat warning, time lapse: {}", nTimeLapse);
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info("Login succeeded. Subscribing market data");
    std::vector<char *> instruments;
    for (auto i = 0; i < interests_.size(); ++i) {
      if (interests_[i] == 1) {
        auto name = ctx_.InstrumentRef().Get(i).name_;
        instruments.push_back(const_cast<char *>(name.c_str()));
      }
    }
    api_->SubscribeMarketData(instruments.data(), instruments.size());
  } else {
    ctx_.Logger()->error("Login failed, error id: {}, error message: {}",
                         pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info("Logout succeeded");
  } else {
    ctx_.Logger()->error("Logout failed, error id: {}, error message: {}",
                         pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) {
  ctx_.Logger()->error("Error, error id: {}, error message: {}",
                       pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

void MdSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info("Subscribing market data succeeded, instrument id: {}",
                        pSpecificInstrument->InstrumentID);
  } else {
    ctx_.Logger()->error(
        "Subscribing market data failed, error id: {}, error message: {}",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info(
        "Unsubscribing market data succeeded, instrument id: {}",
        pSpecificInstrument->InstrumentID);
  } else {
    ctx_.Logger()->error(
        "Unsubscribing market data failed, error id: {}, error message: {}",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info("Subscribing quote data succeeded, instrument id: {}",
                        pSpecificInstrument->InstrumentID);
  } else {
    ctx_.Logger()->error(
        "Subscribing quote data failed, error id: {}, error message: {}",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    ctx_.Logger()->info("Unsubscribing quote data succeeded, instrument id: {}",
                        pSpecificInstrument->InstrumentID);
  } else {
    ctx_.Logger()->error(
        "Unsubscribing quote data failed, error id: {}, error message: {}",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {

  auto id = ctx_.InstrumentRef().GetID(pDepthMarketData->InstrumentID);
  if (!received_[id] == 0) {
    static_.id_ = id;
    // static_.trading_day_ =
    // base::Date::FromString(pDepthMarketData->TradingDay);
    static_.prev_close_ = pDepthMarketData->PreClosePrice;
    static_.upper_limit_ = pDepthMarketData->UpperLimitPrice;
    static_.lower_limit_ = pDepthMarketData->LowerLimitPrice;
    tx_.Write(static_);
    received_[id] = 1;
  }

  depth_.id_ = id;
  depth_.open_ = pDepthMarketData->OpenPrice;
  depth_.high_ = pDepthMarketData->HighestPrice;
  depth_.low_ = pDepthMarketData->LowestPrice;
  depth_.last_ = pDepthMarketData->ClosePrice;
  depth_.open_interest_ = pDepthMarketData->OpenInterest;
  depth_.volume_ = pDepthMarketData->Volume;
  depth_.turnover_ = pDepthMarketData->Turnover;
  depth_.ask_price_[0] = pDepthMarketData->AskPrice1;
  depth_.bid_price_[0] = pDepthMarketData->BidPrice1;
  depth_.ask_volume_[0] = pDepthMarketData->AskVolume1;
  depth_.bid_volume_[0] = pDepthMarketData->BidVolume1;
  tx_.Write(depth_);
}

void MdSpi::SetInterests(std::vector<std::string> instruments) {
  interests_.assign(ctx_.InstrumentRef().Size(), 0);
  received_.assign(ctx_.InstrumentRef().Size(), 0);
  for (auto &i : instruments) {
    auto id = ctx_.InstrumentRef().GetID(i);
    if (id >= 0) {
      interests_[id] = 1;
    }
  }
}

} // namespace ctptrader::app