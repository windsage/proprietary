/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <framework/legacy.h>
#include <QtiMutex.h>
#include <binder/IBinder.h>
#include "BnRadioModemDefault.h"
#include "IRadioModemContext.h"

#include "interfaces/dms/dms_types.h"
#include "interfaces/nas/nas_types.h"

#include "interfaces/RilUnsolRilConnectedMessage.h"
#include "interfaces/nas/RilUnsolModemRestartMessage.h"
#include "interfaces/nas/RilUnsolRadioCapabilityMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"

namespace aidlmodem {
  using namespace aidl::android::hardware::radio;
  using namespace aidl::android::hardware::radio::modem;
}

class IRadioModemImpl : public aidlmodem::BnRadioModemDefault {
 private:
  std::shared_ptr<aidlmodem::IRadioModemResponse> mIRadioModemResponse;
  std::shared_ptr<aidlmodem::IRadioModemIndication> mIRadioModemIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;

  std::shared_ptr<aidlmodem::IRadioModemResponse> getResponseCallback();
  std::shared_ptr<aidlmodem::IRadioModemIndication> getIndicationCallback();

 public:
  IRadioModemImpl(qcril_instance_id_e_type instance);
  ~IRadioModemImpl();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlmodem::IRadioModemResponse>& radioModemResponse,
      const std::shared_ptr<aidlmodem::IRadioModemIndication>& radioModemIndication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<IRadioModemContext> getContext(uint32_t serial) {
    std::shared_ptr<IRadioModemContext> ctx = std::make_shared<IRadioModemContext>(mInstanceId, serial);
    return ctx;
  }

  void deathNotifier();

  // AIDL APIs
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlmodem::IRadioModemResponse>& in_radioModemResponse,
      const std::shared_ptr<aidlmodem::IRadioModemIndication>& in_radioModemIndication
  ) override;

  ::ndk::ScopedAStatus enableModem(int32_t in_serial, bool in_on) override;
  ::ndk::ScopedAStatus getBasebandVersion(int32_t in_serial) override;
  ::ndk::ScopedAStatus getDeviceIdentity(int32_t in_serial) override;
  ::ndk::ScopedAStatus getHardwareConfig(int32_t in_serial) override;
  ::ndk::ScopedAStatus getModemActivityInfo(int32_t in_serial) override;
  ::ndk::ScopedAStatus getModemStackStatus(int32_t in_serial) override;
  ::ndk::ScopedAStatus getRadioCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus nvReadItem(int32_t in_serial, aidlmodem::NvItem in_itemId) override;
  ::ndk::ScopedAStatus nvResetConfig(int32_t in_serial, aidlmodem::ResetNvType in_resetType) override;
  ::ndk::ScopedAStatus nvWriteCdmaPrl(int32_t in_serial, const std::vector<uint8_t>& in_prl) override;
  ::ndk::ScopedAStatus nvWriteItem(int32_t in_serial, const aidlmodem::NvWriteItem& in_item) override;
  ::ndk::ScopedAStatus requestShutdown(int32_t in_serial) override;
  ::ndk::ScopedAStatus responseAcknowledgement() override;
  ::ndk::ScopedAStatus sendDeviceState(int32_t in_serial, aidlmodem::DeviceStateType in_deviceStateType, bool in_state) override;
  ::ndk::ScopedAStatus setRadioCapability(int32_t in_serial, const aidlmodem::RadioCapability& in_rc) override;
  ::ndk::ScopedAStatus setRadioPower(int32_t in_serial, bool in_powerOn, bool in_forEmergencyCall, bool in_preferredForEmergencyCall) override;
  ::ndk::ScopedAStatus getImei(int32_t in_serial) override;

 private:
  // responses
  void sendResponseForSetRadioPower(int32_t serial, RIL_Errno errorCode);
  void sendResponseForEnableModem(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetBasebandVersion(int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> rilResult);
  void sendResponseForGetDeviceIdentity(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilDeviceIdentityResult_t> result);
  void sendResponseForGetHardwareConfig(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetModemActivityInfo(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result);
  void sendResponseForGetModemStackStatus(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetModemStackResult_t> rilModemStackResult);
  void sendResponseForGetRadioCapability(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult);
  void sendResponseForNvReadItem(int32_t serial, RIL_Errno errorCode);
  void sendResponseForNvResetConfig(int32_t serial, RIL_Errno errorCode);
  void sendResponseForNvWriteCdmaPrl(int32_t serial, RIL_Errno errorCode);
  void sendResponseForNvWriteItem(int32_t serial, RIL_Errno errorCode);
  void sendResponseForRequestShutdown(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSendDeviceState(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetRadioCapability(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult);
  void sendResponseForGetImei(int32_t serial,
                              RIL_Errno error,
                              std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> result);

 public:
  // indications
  void sendModemReset(std::shared_ptr<RilUnsolModemRestartMessage> msg);
  void sendRadioCapability(std::shared_ptr<RilUnsolRadioCapabilityMessage> msg);
  void sendRadioStateChanged(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg);
  void sendRilConnected(std::shared_ptr<RilUnsolRilConnectedMessage> msg);
};
