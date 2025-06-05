/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <aidl/vendor/qti/hardware/radio/uim_remote_client/BnUimRemoteServiceClient.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteClientApduStatus.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteEventReqType.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteClientApduRsp.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteClientEventRsp.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/IUimRemoteServiceClientResponse.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/IUimRemoteServiceClientIndication.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteClientVoltageClass.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteClientPowerDownMode.h>
#include "modules/uim_remote/UimRmtQmiUimRemoteIndMsg.h"
#include "modules/uim_remote/UimRmtCardAppInitStatusIndMsg.h"

#include <framework/legacy.h>
#include "QtiMutex.h"
#include "UimRemoteClientContext.h"

class UimRemoteClientAidlModule;

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::uim_remote_client;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_client {
namespace implementation {

class IUimRemoteServiceClientImpl : public aidlimports::BnUimRemoteServiceClient {
 public:
  IUimRemoteServiceClientImpl(UimRemoteClientAidlModule* module);
  void deathNotifier(void* cookie);
  std::shared_ptr<UimRemoteClientContext> getContext(int32_t serial);
  void setInstanceId(int instanceId);
  int getInstanceId();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication>& in_indicationCallback);

  // AIDL request functions
  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication>& in_indicationCallback)
      override;
  ::ndk::ScopedAStatus uimRemoteServiceClientApduReq(
      int32_t in_serial,
      aidlimports::UimRemoteClientApduStatus in_apduStatus,
      const std::vector<uint8_t>& in_apduResponse) override;
  ::ndk::ScopedAStatus uimRemoteServiceClientEventReq(
      int32_t in_serial, const aidlimports::UimRemoteEventReqType& in_eventReq) override;

  // Unsol Message handlers
  void sendUimRemoteServiceClientApduInd(uint32_t apduLen, std::vector<uint8_t> apduInd);
  void sendUimRemoteServiceClientConnectInd();
  void sendUimRemoteServiceClientDisconnectInd();
  void sendUimRemoteServiceClientPowerUpInd(bool hasTimeOut,
                                            int32_t timeOut,
                                            bool hasVoltageClass,
                                            aidlimports::UimRemoteClientVoltageClass voltageClass);
  void sendUimRemoteServiceClientPowerDownInd(bool hasMode,
                                              aidlimports::UimRemoteClientPowerDownMode mode);
  void sendUimRemoteServiceClientResetInd();
  void sendUimRemoteServiceClientServiceInd(bool status);
  void sendUimRemoteServiceClientCardInitStatusInd(const UimRmtCardAppInitStatusIndData* msg);

  // Send Responses
  void sendUimRemoteClientEventResponse(int32_t token,
                                        aidlimports::UimRemoteClientEventRsp eventResp);
  void sendUimRemoteClientApduResponse(int32_t token, aidlimports::UimRemoteClientApduRsp apduResp);

 private:
  int mInstanceId;
  UimRemoteClientAidlModule* mModule;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient{ nullptr };
  std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse> mResponseCb;
  std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication> mIndicationCb;
  std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse> getResponseCallback();
  std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();
};

}  // namespace implementation
}  // namespace uim_remote_client
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
