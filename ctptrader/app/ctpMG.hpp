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
      std::string_view shm_addr, std::vector<std::string> &instruments)
      : api_(api)
      , broker_id_(std::move(broker_id))
      , user_id_(std::move(user_id))
      , password_(std::move(password))
      , instruments_(std::move(instruments))
      , tx_(shm_addr) {
    received_.resize(InstrumentCenter().Size(), false);
  }

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
  util::MsgSender tx_;
  std::vector<bool> received_;
  int request_id_{0};
};

class CtpMG {
public:
  CtpMG(std::string_view channel)
      : channel_(channel) {}
  ~CtpMG() {}
  bool Init(const toml::table &config);
  void Start();

private:
  const std::string channel_;
  std::string broker_id_;
  std::string user_id_;
  std::string password_;
  std::vector<std::string> instruments_;
  std::string front_address_;
};

} // namespace ctptrader::app
