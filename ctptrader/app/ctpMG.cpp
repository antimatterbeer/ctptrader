#include <cstring>
#include <fmt/format.h>

#include <app/ctpMG.hpp>

namespace ctptrader::app {

void Spi::OnFrontConnected() {
  LOG_INFO("[CtpMG]Front connected");
  CThostFtdcReqUserLoginField req{};
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, broker_id_.c_str());
  strcpy(req.UserID, user_id_.c_str());
  strcpy(req.Password, password_.c_str());
  if (api_->ReqUserLogin(&req, ++request_id_) == 0) {
    LOG_INFO("[CtpMG]Send ReqUserLogin success");
  } else {
    LOG_ERROR("[CtpMG]Send ReqUserLogin failed");
  }
}

void Spi::OnFrontDisconnected(int nReason) {
  LOG_INFO("[CtpMG]Front disconnected. Reason = {}", nReason);
}

void Spi::OnHeartBeatWarning(int nTimeLapse) {
  LOG_INFO("[CtpMG]Heart beat warning received. TimeLapse = {}", nTimeLapse);
}

void Spi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast) {
  if (pRspInfo && pRspInfo->ErrorID == 0) {
    LOG_INFO("[CtpMG]User login success");
    std::vector<char *> instruments;
    for (auto &i : instruments_) {
      instruments.emplace_back(const_cast<char *>(i.c_str()));
    }
    int res = api_->SubscribeMarketData(instruments.data(), instruments.size());
    if (res == 0) {
      LOG_INFO("[CtpMG]Send SubscribeMarketData request success");
    } else {
      LOG_ERROR("[CtpMG]Send SubscribeMarketData request failed");
    }
  } else {
    LOG_ERROR("[CtpMG]User login failed");
  }
}

void Spi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) {
  LOG_INFO("[CtpMG]User logout success");
}

void Spi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast) {
  LOG_ERROR("[CtpMG]Response error.ErrorID = {}, ErrorMsg = {}",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

void Spi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG_INFO("[CtpMG]Subscribe market data response received. InstrumentID = {}",
           pSpecificInstrument->InstrumentID);
}

void Spi::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG_INFO(
      "[CtpMG]Unsubscribe market data response received. InstrumentID = {}",
      pSpecificInstrument->InstrumentID);
}

void Spi::OnRspSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG_INFO("[CtpMG]Subscribe quote response received. InstrumentID = {}",
           pSpecificInstrument->InstrumentID);
}

void Spi::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG_INFO("[CtpMG]Unsubscribe quote response received. InstrumentID = {}",
           pSpecificInstrument->InstrumentID);
}

void Spi::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  base::Depth d;
  memset(&d, 0, sizeof(d));
  std::string instrument_id(pDepthMarketData->InstrumentID);
  d.update_time_ = base::Timestamp::FromString(fmt::format(
      "{} {}", pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime));
  d.instrument_id_ = InstrumentCenter().GetID(instrument_id);
  d.open_ = pDepthMarketData->OpenPrice;
  d.high_ = pDepthMarketData->HighestPrice;
  d.low_ = pDepthMarketData->LowestPrice;
  d.last_ = pDepthMarketData->LastPrice;
  d.open_interest_ = pDepthMarketData->OpenInterest;
  d.volume_ = pDepthMarketData->Volume;
  d.turnover_ = pDepthMarketData->Turnover;
  d.ask_price_[0] = pDepthMarketData->AskPrice1;
  d.bid_price_[0] = pDepthMarketData->BidPrice1;
  d.ask_volume_[0] = pDepthMarketData->AskVolume1;
  d.bid_volume_[0] = pDepthMarketData->BidVolume1;
  tx_.Write(d);
}

void Spi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {}

} // namespace ctptrader::ctpMG
