#pragma once
#define FMT_HEADER_ONLY

#include <string_view>
#include <vector>

#include <ThostFtdcMdApi.h>
#include <fmt/format.h>
#include <toml.hpp>

#include <base/msg.hpp>
#include <core/app.hpp>
#include <core/ctx.hpp>
#include <util/channel.hpp>

namespace ctptrader::app {

class MdSpi final : public CThostFtdcMdSpi {
public:
  MdSpi(core::Context *ctx, CThostFtdcMdApi *api,
        const std::string_view market_channel, const std::string_view broker_id,
        const std::string_view user_id, const std::string_view password)
      : ctx_(ctx)
      , api_(api)
      , tx_(market_channel)
      , broker_id_(broker_id)
      , user_id_(user_id)
      , password_(password) {}

  ~MdSpi() = default;

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

  void SetInterests(std::vector<std::string> instruments);

private:
  core::Context *ctx_;
  CThostFtdcMdApi *api_{nullptr};
  util::ShmSpscWriter<base::Msg, 20> tx_;
  const std::string broker_id_;
  const std::string user_id_;
  const std::string password_;
  std::vector<int> interests_;
  std::vector<int> received_;
  base::Depth depth_;
  base::Static static_;
  int request_id_{0};
};

class MarketManager : public core::IApp {
public:
  explicit MarketManager(const std::string_view market_channel)
      : market_channel_(market_channel) {}

  bool Init(toml::table &global_config, toml::table &app_config) override {
    std::string data_folder = global_config["data_folder"].value_or("");
    if (!ctx_.Init(data_folder)) {
      return false;
    }
    broker_id_ = global_config["broker_id"].value_or("");
    user_id_ = global_config["user_id"].value_or("");
    password_ = global_config["password"].value_or("");
    market_front_ = app_config["front"].value_or("");
    if (broker_id_.empty() || user_id_.empty() || password_.empty() ||
        market_front_.empty()) {
      return false;
    }
    for (const auto &c : *app_config["instruments"].as_array()) {
      auto i = c.value<std::string>();
      if (i.has_value()) {
        instruments_.push_back(i.value());
      }
    }
    if (instruments_.empty()) {
      return false;
    }
    return true;
  }

  void Run() override {
    const auto front_address = fmt::format("tcp://{}", market_front_);
    auto *api = CThostFtdcMdApi::CreateFtdcMdApi();
    MdSpi spi(&ctx_, api, market_channel_, broker_id_, user_id_, password_);
    spi.SetInterests(instruments_);
    api->RegisterSpi(&spi);
    api->RegisterFront(const_cast<char *>(front_address.c_str()));
    api->Init();
    api->Join();
    api->Release();
  }

private:
  const std::string market_channel_;
  std::string broker_id_;
  std::string user_id_;
  std::string password_;
  std::string market_front_;
  std::vector<std::string> instruments_;
};

} // namespace ctptrader::app