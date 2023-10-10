#pragma once

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <fmt/format.h>
#include <toml.hpp>

#include <base/def.hpp>
#include <base/msg.hpp>
#include <service/dataService.hpp>
#include <util/channel.hpp>

namespace ctptrader::app {

class Spi : public CThostFtdcMdSpi {
public:
  Spi(CThostFtdcMdApi *api, const std::string &&broker_id,
      const std::string &&user_id, const std::string &&password,
      std::string_view shm_addr, std::vector<std::string> instruments)
      : api_(api), broker_id_(std::move(broker_id)),
        user_id_(std::move(user_id)), password_(std::move(password)),
        instruments_(std::move(instruments)), tx_(shm_addr) {}
  ~Spi() {}

  void OnFrontConnected() override;
  void OnFrontDisconnected(int nReason) override;
  void OnHeartBeatWarning(int nTimeLapse) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) override;
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) override;
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                  bool bIsLast) override;
  void
  OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                     CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast) override;
  void
  OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) override;
  void
  OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) override;
  void
  OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) override;
  void OnRtnDepthMarketData(
      CThostFtdcDepthMarketDataField *pDepthMarketData) override;
  void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) override;

private:
  CThostFtdcMdApi *api_;
  const std::string broker_id_;
  const std::string user_id_;
  const std::string password_;
  const std::vector<std::string> instruments_;
  util::ShmSpscWriter<base::Msg, 1024> tx_;
  int request_id_{0};
};

class CtpMG {
public:
  CtpMG(std::string_view channel) : channel_(channel) {}
  ~CtpMG() {}

  bool Init(const toml::table &config) {
    broker_id_ = config["broker_id"].value_or<std::string>("");
    user_id_ = config["user_id"].value_or<std::string>("");
    password_ = config["password"].value_or<std::string>("");
    auto market_front = config["market_front"].value_or<std::string>("");
    auto instruments = config["instruments"].as_array();
    std::vector<std::string> ins;
    for (auto &&ele : *instruments) {
      auto v = ele.value<std::string>();
      if (v.has_value()) {
        ins.emplace_back(v.value());
      }
    }
    if (broker_id_.empty() || user_id_.empty() || password_.empty() ||
        market_front.empty() || ins.empty()) {
      std::cerr << "Error: broker_id, user_id, password, market_front, "
                   "instruments must be specified"
                << std::endl;
      return false;
    }
    front_address_ = fmt::format("tcp://{}", market_front);
    return true;
  }

  void Start() {
    auto *api = CThostFtdcMdApi::CreateFtdcMdApi();
    Spi spi(api, std::move(broker_id_), std::move(user_id_),
            std::move(password_), channel_, std::move(instruments_));
    api->RegisterSpi(&spi);
    api->RegisterFront((char *)front_address_.c_str());
    api->Init();
    api->Join();
    api->Release();
  }

private:
  const std::string channel_;
  std::string broker_id_;
  std::string user_id_;
  std::string password_;
  std::vector<std::string> instruments_;
  std::string front_address_;
};

} // namespace ctptrader::app
