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
      LOG_INFO("[CtpMG]Subscribe market data. InstrumentID = {}", i);
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
  fmt::println("{} {}", pDepthMarketData->ActionDay,
               pDepthMarketData->UpdateTime);
  base::Depth depth;
  std::string instrument_id(pDepthMarketData->InstrumentID);
  depth.update_time_ = base::Timestamp::FromString(fmt::format(
      "{} {}", pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime));
  depth.instrument_id_ = InstrumentCenter().GetID(instrument_id);
  depth.open_ = pDepthMarketData->OpenPrice;
  depth.high_ = pDepthMarketData->HighestPrice;
  depth.low_ = pDepthMarketData->LowestPrice;
  depth.last_ = pDepthMarketData->LastPrice;
  depth.open_interest_ = pDepthMarketData->OpenInterest;
  depth.volume_ = pDepthMarketData->Volume;
  depth.turnover_ = pDepthMarketData->Turnover;
  depth.ask_price_[0] = pDepthMarketData->AskPrice1;
  depth.bid_price_[0] = pDepthMarketData->BidPrice1;
  depth.ask_volume_[0] = pDepthMarketData->AskVolume1;
  depth.bid_volume_[0] = pDepthMarketData->BidVolume1;
  depth.update_time_ = base::Timestamp::FromString(fmt::format(
      "{} {}", pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime));
  if (!received_[depth.instrument_id_]) [[unlikely]] {
    base::Static st;
    st.instrument_id_ = depth.instrument_id_;
    st.prev_close_ = pDepthMarketData->PreClosePrice;
    st.upper_limit_ = pDepthMarketData->UpperLimitPrice;
    st.lower_limit_ = pDepthMarketData->LowerLimitPrice;
    st.trading_day_ = base::Date(std::atoi(pDepthMarketData->TradingDay));
    if (!tx_.Write(st)) {
      LOG_ERROR("[CtpMG]Write to channel failed");
    }
  }
  if (!tx_.Write(depth)) {
    LOG_ERROR("[CtpMG]Write to channel failed");
  }
}

void Spi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
  LOG_INFO("[CtpMG]Quote received. InstrumentID = {}",
           pForQuoteRsp->InstrumentID);
}

bool CtpMG::Init(const toml::table &config) {
  LOG_INFO("[CtpMG]Init");
  broker_id_ = config["broker_id"].value_or<std::string>("");
  user_id_ = config["user_id"].value_or<std::string>("");
  password_ = config["password"].value_or<std::string>("");
  auto market_front = config["market_front"].value_or<std::string>("");
  auto instruments = config["instruments"].as_array();
  if (broker_id_.empty() || user_id_.empty() || password_.empty() ||
      market_front.empty() || instruments->empty()) {
    std::cerr << "Error: broker_id, user_id, password, market_front, "
                 "instruments must be specified"
              << std::endl;
    return false;
  }
  front_address_ = fmt::format("tcp://{}", market_front);
  for (auto &&ele : *instruments) {
    auto v = ele.value<std::string>();
    if (v.has_value()) {
      instruments_.emplace_back(v.value());
    }
  }
  return true;
}

void CtpMG::Start() {
  auto *api = CThostFtdcMdApi::CreateFtdcMdApi();
  Spi spi(api, std::move(broker_id_), std::move(user_id_), std::move(password_),
          channel_, instruments_);
  api->RegisterSpi(&spi);
  api->RegisterFront((char *)front_address_.c_str());
  api->Init();
  api->Join();
  api->Release();
}

} // namespace ctptrader::app
