#include <app/market.hpp>

namespace ctptrader::app {

void MdSpi::OnFrontConnected() {
  LOG_INFO("Connected to market server, sending login request");
  CThostFtdcReqUserLoginField req{};
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, broker_id_.c_str());
  strcpy(req.UserID, user_id_.c_str());
  strcpy(req.Password, password_.c_str());
  if (api_->ReqUserLogin(&req, 0) != 0) {
    LOG_ERROR("Sending login request failed");
  } else {
    LOG_INFO("Sending login request succeeded");
  }
}

void MdSpi::OnFrontDisconnected(int nReason) {
  LOG_INFO("Disconnected from market server, reason: %d", nReason);
}

void MdSpi::OnHeartBeatWarning(int nTimeLapse) {
  LOG_INFO("Heartbeat warning, time lapse: %d", nTimeLapse);
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                           CThostFtdcRspInfoField *pRspInfo,
                           [[maybe_unused]] int nRequestID,
                           [[maybe_unused]] bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Login succeeded. Trading day: %s", pRspUserLogin->TradingDay);
    std::vector<char *> instruments;
    for (size_t i = 0; i < interests_.size(); ++i) {
      if (interests_[i] == 1) {
        auto name = ctx_->GetInstrumentCenter().Get(i).name_;
        instruments.push_back(const_cast<char *>(name.c_str()));
      }
    }
    api_->SubscribeMarketData(instruments.data(), instruments.size());
  } else {
    LOG_ERROR("Login failed, error id: %d, error message: %s",
              pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUserLogout(
    [[maybe_unused]] CThostFtdcUserLogoutField *pUserLogout,
    CThostFtdcRspInfoField *pRspInfo, [[maybe_unused]] int nRequestID,
    [[maybe_unused]] bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Logout succeeded");
  } else {
    LOG_ERROR("Logout failed, error id: %d, error message: %s",
              pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
                       [[maybe_unused]] int nRequestID,
                       [[maybe_unused]] bool bIsLast) {
  LOG_ERROR("Error, error id: %d, error message: %s", pRspInfo->ErrorID,
            pRspInfo->ErrorMsg);
}

void MdSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, [[maybe_unused]] int nRequestID,
    [[maybe_unused]] bool bIsLast) {
  LOG_INFO(
      "Subscribing market data response received. request id: %d, is last: %d",
      nRequestID, bIsLast);
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Subscribing market data succeeded, instrument id: %s",
             pSpecificInstrument->InstrumentID);
  } else {
    LOG_ERROR("Subscribing market data failed, error id: %d, error message: %s",
              pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, [[maybe_unused]] int nRequestID,
    [[maybe_unused]] bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Unsubscribing market data succeeded, instrument id: %s",
             pSpecificInstrument->InstrumentID);
  } else {
    LOG_ERROR(
        "Unsubscribing market data failed, error id: %d, error message: %s",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, [[maybe_unused]] int nRequestID,
    [[maybe_unused]] bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Subscribing quote data succeeded, instrument id: %s",
             pSpecificInstrument->InstrumentID);
  } else {
    LOG_ERROR("Subscribing quote data failed, error id: %d, error message: %s",
              pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, [[maybe_unused]] int nRequestID,
    [[maybe_unused]] bool bIsLast) {
  if (pRspInfo->ErrorID == 0) {
    LOG_INFO("Unsubscribing quote data succeeded, instrument id: %s",
             pSpecificInstrument->InstrumentID);
  } else {
    LOG_ERROR(
        "Unsubscribing quote data failed, error id: %d, error message: %s",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  }
}

void MdSpi::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  auto id = ctx_->GetInstrumentCenter().GetID(pDepthMarketData->InstrumentID);
  if (!received_[id] == 0) {
    static_.id_ = id;
    // static_.trading_day_ =
    // base::Date::FromString(pDepthMarketData->TradingDay);
    static_.prev_close_ = pDepthMarketData->PreClosePrice;
    static_.upper_limit_ = pDepthMarketData->UpperLimitPrice;
    static_.lower_limit_ = pDepthMarketData->LowerLimitPrice;
    if (!tx_.Write(static_)) {
      LOG_ERROR("Failed to write static data to tx");
    }
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
  if (!tx_.Write(depth_)) {
    LOG_ERROR("Failed to write depth data to tx");
  }
}

void MdSpi::SetInterests(std::vector<std::string> instruments) {
  interests_.assign(ctx_->GetInstrumentCenter().Count(), 0);
  received_.assign(ctx_->GetInstrumentCenter().Count(), 0);
  for (auto &i : instruments) {
    auto id = ctx_->GetInstrumentCenter().GetID(i);
    if (id >= 0) {
      interests_[id] = 1;
    }
  }
}

} // namespace ctptrader::app