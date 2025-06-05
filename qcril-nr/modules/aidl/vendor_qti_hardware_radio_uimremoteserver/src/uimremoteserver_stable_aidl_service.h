/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <aidl/vendor/qti/hardware/radio/uim_remote_server/BnUimRemoteServiceServer.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_server/IUimRemoteServiceServerResponse.h>
#include <aidl/vendor/qti/hardware/radio/uim_remote_server/IUimRemoteServiceServerIndication.h>
#include "framework/legacy.h"
#include "QtiMutex.h"
#include "UimRemoteServerContext.h"

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::uim_remote_server;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_server {
namespace implementation {

class IUimRemoteServiceServerImpl : public aidlimports::BnUimRemoteServiceServer {
 public:
  void deathNotifier(void* cookie);
  std::shared_ptr<UimRemoteServerContext> getContext(int32_t serial);
  void setInstanceId(qcril_instance_id_e_type instanceId);
  qcril_instance_id_e_type getInstanceId();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication>& in_indicationCallback);

  // AIDL request functions
  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication>& in_indicationCallback)
      override;
  ::ndk::ScopedAStatus uimRemoteServiceServerApduReq(
      int32_t in_serial,
      aidlimports::UimRemoteServiceServerApduType in_apduType,
      const std::vector<uint8_t>& in_command) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerConnectReq(int32_t in_serial,
                                                        int32_t in_maxMsgSize) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerDisconnectReq(int32_t in_serial) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerPowerReq(int32_t in_serial, bool state) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerResetSimReq(int32_t in_serial) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerSetTransferProtocolReq(
      int32_t in_serial,
      aidlimports::UimRemoteServiceServerTransferProtocol in_transferProtocol) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerTransferAtrReq(int32_t in_serial) override;
  ::ndk::ScopedAStatus uimRemoteServiceServerTransferCardReaderStatusReq(int32_t in_serial) override;

  // Unsol Message handlers
  void uimRemoteServiceServerDisconnectIndication(
      aidlimports::UimRemoteServiceServerDisconnectType disconnectType);
  void uimRemoteServiceServerStatusIndication(aidlimports::UimRemoteServiceServerStatus status);

  // Send Responses
  void sendApduResponse(int32_t serial,
                        aidlimports::UimRemoteServiceServerResultCode resultCode,
                        const std::vector<uint8_t>& apduRsp);
  void sendConnectResponse(int32_t serial,
                           aidlimports::UimRemoteServiceServerConnectRsp sapConnectRsp,
                           int32_t maxMsgSize);
  void sendDisconnectResponse(int32_t serial);
  void sendErrorResponse(int32_t serial);
  void sendPowerResponse(int32_t serial, aidlimports::UimRemoteServiceServerResultCode resultCode);
  void sendResetSimResponse(int32_t serial,
                            aidlimports::UimRemoteServiceServerResultCode resultCode);
  void sendTransferAtrResponse(int32_t serial,
                               aidlimports::UimRemoteServiceServerResultCode resultCode,
                               const std::vector<uint8_t>& atr);
  void sendTransferCardReaderStatusResponse(int32_t serial,
                                            aidlimports::UimRemoteServiceServerResultCode resultCode,
                                            int32_t cardReaderStatus);
  void sendTransferProtocolResponse(int32_t serial,
                                    aidlimports::UimRemoteServiceServerResultCode resultCode);

 private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient{ nullptr };
  std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse> mResponseCb;
  std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication> mIndicationCb;
  std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse> getResponseCallback();
  std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();
};

}  // namespace implementation
}  // namespace uim_remote_server
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
