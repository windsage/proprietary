/******************************************************************************
#  Copyright (c) 2018, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <telephony/ril.h>
#include <optional>
#include "interfaces/common.h"

// Message Callback structure
struct RilSendSmsResult_t: public qcril::interfaces::BasePayload {
  int       messageRef;
  std::string ackPDU;
  int       errorCode;
  std::optional<bool> bLteOnlyReg;
  int       rpCause;
  int       causeCode;
  RIL_RadioTechnology rat;

  inline RilSendSmsResult_t():
        messageRef(-1), errorCode(-1) {}
  inline RilSendSmsResult_t(int ref, int code):
        messageRef(ref), errorCode(code) {}
  inline RilSendSmsResult_t(int ref, int code, bool lteOnly):
        messageRef(ref), errorCode(code), bLteOnlyReg(lteOnly) {}
  inline RilSendSmsResult_t(int ref, bool lteOnly, int rpCause, int causeCode):
        messageRef(ref), bLteOnlyReg(lteOnly), rpCause(rpCause), causeCode(causeCode) {}
  inline RilSendSmsResult_t(int ref, bool lteOnly, int rpCause, int causeCode,
        RIL_RadioTechnology rat):
        messageRef(ref), bLteOnlyReg(lteOnly), rpCause(rpCause), causeCode(causeCode), rat(rat) {}
};

struct RilWriteSmsToSimResult_t: public qcril::interfaces::BasePayload {
  int32_t   recordNumber;
  inline RilWriteSmsToSimResult_t(int32_t index = -1):
        recordNumber(index) {}
};

struct RilGetSmscAddrResult_t: public qcril::interfaces::BasePayload {
  string    smscAddr;
  inline RilGetSmscAddrResult_t(){}
  template<typename T>
  inline RilGetSmscAddrResult_t(T&& addr):
        smscAddr(std::forward<T>(addr)) {}
};

struct RilGetGsmBroadcastConfigResult_t: public qcril::interfaces::BasePayload {
  std::vector<RIL_GSM_BroadcastSmsConfigInfo> configList;
  inline RilGetGsmBroadcastConfigResult_t(){}
  template<typename T>
  inline RilGetGsmBroadcastConfigResult_t(T&& list):
        configList(std::forward<T>(list)) {}
};

struct RilGetCdmaBroadcastConfigResult_t: public qcril::interfaces::BasePayload {
  std::vector<RIL_CDMA_BroadcastSmsConfigInfo> configList;
  inline RilGetCdmaBroadcastConfigResult_t(){}
  template<typename T>
  inline RilGetCdmaBroadcastConfigResult_t(T&& list):
        configList(std::forward<T>(list)) {}
};

struct RilGetImsRegistrationResult_t: public qcril::interfaces::BasePayload {
  bool isRegistered;
  RIL_RadioTechnologyFamily ratFamily;
  inline RilGetImsRegistrationResult_t():
        isRegistered(false), ratFamily(RADIO_TECH_3GPP) {}
};
