#pragma once

#include <ThostFtdcTraderApi.h>

#include <service/dataService.hpp>

namespace ctptrader::app {

class TraderSpi : public CThostFtdcTraderSpi {
public:
  void OnFrontConnected() override { LOG_INFO("[CtpTG]Front connected"); }

  void OnFrontDisconnected(int nReason) override {
    LOG_INFO("[CtpTG]Front disconnected. Reason = {}", nReason);
  }

  void OnHeartBeatWarning(int nTimeLapse) override {
    LOG_INFO("[CtpTG]Heart beat warning received. TimeLapse = {}", nTimeLapse);
  }

  void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast) override{

  };

  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) override{};

  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) override{};

  void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) override{};
};

} // namespace ctptrader::app