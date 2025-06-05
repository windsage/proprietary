/******************************************************************************
  @file    AtFwdServiceImpl.h
  @brief   ATFwd Stable AIDL service implementation for interacting with the
           Android module.
 -------------------------------------------------------------------------------

  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 -------------------------------------------------------------------------------
 *******************************************************************************/

#pragma once
#include <mutex>
#include <aidl/vendor/qti/hardware/radio/atfwd/BnAtFwd.h>
#include "AtCmdFwd.h"

namespace aidlimports {
  using namespace aidl::vendor::qti::hardware::radio::atfwd;
}

typedef void (*ResponseCallback)(int32_t, aidlimports::AtCmdResponse);
typedef void (*DeathRecipientCallback)();

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace atfwd {
namespace implementation {

class AtFwdServiceImpl : public aidlimports::BnAtFwd {

public:
  // Requests from an AIDL client to this service
  ::ndk::ScopedAStatus setIndicationCallback(
      const std::shared_ptr<aidlimports::IAtFwdIndication>& indicationCallback) override;
  ::ndk::ScopedAStatus sendAtCommandProcessedState(int32_t serial,
      const aidlimports::AtCmdResponse& state) override;

  // Indication from this service to AIDL clients
  bool onAtCommandForwarded(int32_t serial, const AtCmd& command);

  // Methods for the AtFwd daemon to interact with the AIDL server
  void deathNotifier(void* cookie);
  void setAtCommandResponseFuncPointer(ResponseCallback responseCallback);
  void setClientDeathRecipientFuncPointer(DeathRecipientCallback deathCallback);

private:
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  std::mutex indicationCallbackMutex;

  virtual void clearCallbacks();
  std::shared_ptr<aidlimports::IAtFwdIndication> mIndicationCb;
  std::shared_ptr<aidlimports::IAtFwdIndication> getIndicationCallback();
  ResponseCallback onAtCommandResponse;
  DeathRecipientCallback onClientDied;
};
} // namespace implementation
} // namespace atfwd
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
} // namespace aidl