/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "BnSapDefault.h"
#include "QtiMutex.h"
#include "framework/legacy.h"
#include <binder/IBinder.h>
#undef TAG
#define TAG "RILQ"

namespace aidlimports {
using namespace aidl::android::hardware::radio::sap;
}

class ISapImpl : public aidlimports::BnSapDefault {
private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient *mDeathRecipient = nullptr;
  AIBinder_DeathRecipient *mGBADeathRecipient = nullptr;
  std::shared_ptr<aidlimports::ISapCallback> mSapServiceResponseCb;
  std::shared_ptr<aidlimports::ISapCallback> getSAPServiceResponseCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();
  void sendresponseforconnect(int32_t token, aidlimports::SapConnectRsp rsp);
  void sendresponsefordisconnect(int32_t token);
  void sendresponseforapdu(int32_t token, ::aidlimports::SapResultCode result,
                           std::vector<uint8_t> &apdu_data);
  void sendresponseforcardreaderstatus(int32_t token,
                                       ::aidlimports::SapResultCode result,
                                       int32_t status);
  void sendresponseforsapreset(int32_t token,
                               ::aidlimports::SapResultCode result);
  void sendresponseforsap(int32_t token, ::aidlimports::SapResultCode resp);
  void sendresponsefortransferatr(int32_t token,
                                  ::aidlimports::SapResultCode result,
                                  std::vector<uint8_t> atr_data);

public:
  ISapImpl(qcril_instance_id_e_type instance);
  ~ISapImpl();

  void deathNotifier(void *cookie);

  qcril_instance_id_e_type getInstanceId() { return mInstanceId; }

  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::ISapCallback> &in_uimSapResponse)
      override;

  ::ndk::ScopedAStatus
  apduReq(int32_t token, aidlimports::SapApduType simlockOp,
          const std::vector<uint8_t> &simlockData) override;

  ::ndk::ScopedAStatus connectReq(int32_t in_serial,
                                  int32_t in_maxMsgSizeBytes) override;

  ::ndk::ScopedAStatus disconnectReq(int32_t in_serial) override;

  ::ndk::ScopedAStatus powerReq(int32_t in_serial, bool in_powerOn) override;

  ::ndk::ScopedAStatus resetSimReq(int32_t in_serial) override;

  ::ndk::ScopedAStatus setTransferProtocolReq(
      int32_t in_serial,
      ::aidlimports::SapTransferProtocol in_transferProtocol) override;

  ::ndk::ScopedAStatus transferAtrReq(int32_t in_serial) override;

  ::ndk::ScopedAStatus transferCardReaderStatusReq(int32_t in_serial) override;

  void sendStatusInd(int32_t token, ::aidlimports::SapStatus status);
  void sendDisconnectInd(int32_t token,
                         ::aidlimports::SapDisconnectType disconnectType);
};
