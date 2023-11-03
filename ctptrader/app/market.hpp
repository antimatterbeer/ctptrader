#pragma once

#include <string_view>
#include <vector>

#include <ThostFtdcMdApi.h>
#include <fmt/format.h>
#include <toml.hpp>

#include <base/msg.hpp>
#include <core/ctx.hpp>
#include <util/channel.hpp>

namespace ctptrader::app {

class MdSpi : public CThostFtdcMdSpi {
public:
  MdSpi(core::Context &&ctx, std::string_view market_channel,
        std::string_view broker_id, std::string_view user_id,
        std::string_view password, CThostFtdcMdApi *api)
      : ctx_(std::move(ctx))
      , tx_(market_channel)
      , broker_id_(broker_id)
      , user_id_(user_id)
      , password_(password)
      , api_(api) {}

  ~MdSpi() {}

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
  core::Context ctx_;
  util::ShmSpscWriter<base::Msg, 20> tx_;
  const std::string broker_id_;
  const std::string user_id_;
  const std::string password_;
  CThostFtdcMdApi *api_{nullptr};
  std::vector<int> interests_;
  std::vector<int> received_;
  base::Depth depth_;
  base::Static static_;
};

inline int start_market(toml::table &global_config, toml::table &app_config) {
  std::string market_channel = global_config["market_channel"].value_or("");
  std::string data_folder = global_config["data_folder"].value_or("");
  std::string broker_id = global_config["broker_id"].value_or("");
  std::string user_id = global_config["user_id"].value_or("");
  std::string password = global_config["password"].value_or("");
  std::string market_front = app_config["front"].value_or("");
  std::vector<std::string> instruments;
  for (const auto &c : *app_config["instruments"].as_array()) {
    auto i = c.value<std::string>();
    if (i.has_value()) {
      instruments.push_back(i.value());
    }
  }

  if (market_channel.empty() || data_folder.empty() || broker_id.empty() ||
      user_id.empty() || password.empty() || market_front.empty() ||
      instruments.empty()) {
    std::cerr << "Invalid config file.\n";
    return 1;
  }

  core::Context ctx;
  if (!ctx.Init(data_folder)) {
    std::cerr << "Failed to initialize context.\n";
    return 1;
  }
  auto front_address = fmt::format("tcp://{}", market_front.data());
  auto *api = CThostFtdcMdApi::CreateFtdcMdApi();
  MdSpi spi(std::move(ctx), market_channel, broker_id, user_id, password, api);
  spi.SetInterests(instruments);
  api->RegisterSpi(&spi);
  api->RegisterFront((char *)front_address.c_str());
  api->Init();
  api->Join();
  api->Release();
  return 0;
}

} // namespace ctptrader::app