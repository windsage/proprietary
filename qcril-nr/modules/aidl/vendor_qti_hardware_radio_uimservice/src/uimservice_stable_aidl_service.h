/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <aidl/vendor/qti/hardware/radio/uim/BnUim.h>
#include "framework/legacy.h"
#include "QtiMutex.h"
#include "IUimContext.h"

#undef TAG
#define TAG "RILQ"

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::uim;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim {
namespace implementation {

class IUimImpl : public aidlimports::BnUim {
 private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  AIBinder_DeathRecipient* mGBADeathRecipient = nullptr;
  std::shared_ptr<aidlimports::IUimResponse> mGBAServiceResponseCb;
  std::shared_ptr<aidlimports::IUimResponse> mRemoteSimlockResponseCb;
  std::shared_ptr<aidlimports::IUimIndication> mIndicationCb;
  std::shared_ptr<aidlimports::IUimResponse> getRemoteSimlockResponseCallback();
  std::shared_ptr<aidlimports::IUimResponse> getGBAServiceResponseCallback();
  std::shared_ptr<aidlimports::IUimIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearGBACallbacks_nolock();
  void clearCallbacks();
  void clearGBACallbacks();
  void sendResponseForUimRemoteSimlock(int32_t token,
                                       UimRemoteSimlockResponseType response,
                                       UimRemoteSimlockOperationType op_type,
                                       std::vector<uint8_t>& simlock_data,
                                       UimRemoteSimlockVersion version,
                                       UimRemoteSimlockStatus status,
                                       int32_t timer_value);
  void sendResponseForuimGbaInit(int32_t token,
                                 aidlimports::UimGbaStatus gbaStatus,
                                 bool isKsNafEncrypted,
                                 std::vector<uint8_t> ksNaf,
                                 std::string bTid,
                                 std::string lifeTime);
  void sendResponseForUimGbaGetImpi(int32_t token,
                                    aidlimports::UimGbaStatus gbaStatus,
                                    bool is_impi_encrypted,
                                    std::vector<uint8_t> impi);

 public:
  IUimImpl(qcril_instance_id_e_type instance);
  ~IUimImpl();
  void deathNotifier(void* cookie);
  void gbaDeathNotifier(void* cookie);
  std::shared_ptr<IUimContext> getContext(int32_t serial);
  qcril_instance_id_e_type getInstanceId();

  void setRemoteSimlockResponse_nolock(
      const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse,
      const std::shared_ptr<aidlimports::IUimIndication>& in_uimServiceIndication);
  void setGBAServiceResponse_nolock(
      const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse);

  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse,
      const std::shared_ptr<aidlimports::IUimIndication>& in_uimServiceIndication) override;
  ::ndk::ScopedAStatus setGbaCallback(
      const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse) override;
  ::ndk::ScopedAStatus uimRemoteSimlockRequest(int32_t token,
                                               aidlimports::UimRemoteSimlockOperationType simlockOp,
                                               const std::vector<uint8_t>& simlockData) override;
  ::ndk::ScopedAStatus uimGbaInit(int32_t token,
                                  const std::vector<uint8_t>& securityProtocol,
                                  const std::string& nafFullyQualifiedDomainName,
                                  aidlimports::UimApplicationType appType,
                                  bool forceBootStrapping,
                                  const std::string& apn) override;
  ::ndk::ScopedAStatus uimGetImpi(int32_t token,
                                  aidlimports::UimApplicationType apptype,
                                  bool secure) override;
};

}  // namespace implementation
}  // namespace uim
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
