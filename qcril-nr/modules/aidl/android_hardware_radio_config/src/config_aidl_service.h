/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "BnRadioConfigDefault.h"
#include "IRadioConfigContext.h"
#include "QtiMutex.h"
#include "aidl/android/hardware/radio/sim/CardStatus.h"
#include "binder/IBinder.h"
#include "framework/legacy.h"
#include "interfaces/mbn/QcRilRequestGetModemsConfigMessage.h"
#include "interfaces/mbn/QcRilRequestSetModemsConfigMessage.h"
#include "interfaces/mbn/mbn.h"
#include "interfaces/nas/RilRequestGetPhoneCapabilityMessage.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/uim/UimGetSlotStatusRequestMsg.h"
#include "interfaces/uim/UimSlotStatusInd.h"
#include "interfaces/uim/UimSwitchSlotPortRequestMsg.h"
#include "interfaces/uim/UimSwitchSlotRequestMsg.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "request/SetPreferredDataModemRequestMessage.h"
#include <memory>

namespace aidlconfig {
using namespace aidl::android::hardware::radio::config;
}

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}

namespace aidlsim {
using namespace aidl::android::hardware::radio::sim;
}

class IRadioConfigImpl : public aidlconfig::BnRadioConfigDefault {
private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient{nullptr};
  std::shared_ptr<aidlconfig::IRadioConfigIndication> mIndicationCb;
  std::shared_ptr<aidlconfig::IRadioConfigResponse> mResponseCb;

protected:
  std::shared_ptr<aidlconfig::IRadioConfigResponse> getResponseCallback();
  std::shared_ptr<aidlconfig::IRadioConfigIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();

//Send Response Functions

  void sendResponseForGetHalDeviceCapabilities(int32_t in_serial,
    RIL_Errno errorCode, bool modemReducedFeatureSet);
  void sendResponseForGetNumOfLiveModems(int32_t in_serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::ModemsConfigResp> result);
  void sendResponseForGetPhoneCapability(int32_t in_serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilPhoneCapabilityResult_t> payload);
  void sendResponseForGetSimSlotsStatus(int32_t in_serial, RIL_Errno errorCode,
    std::shared_ptr<RIL_UIM_SlotsStatusInfo> responseDataPtr);
  void sendResponseForSetNumOfLiveModems(int32_t in_serial, RIL_Errno errorCode);
  void sendResponseForSetSimSlotsMapping(int32_t in_serial, RIL_Errno errorCode);
  void sendResponseForSetPreferredDataModem(int32_t in_serial, std::shared_ptr<rildata::SetPreferredDataModemResponse_t> response);
public:

  IRadioConfigImpl() = delete;
  IRadioConfigImpl(qcril_instance_id_e_type instance);
  ~IRadioConfigImpl();

  android::status_t registerService();

  void deathNotifier(void* cookie);

  std::shared_ptr<IRadioConfigContext> getContext(uint32_t serial);

  qcril_instance_id_e_type getInstanceId();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlconfig::IRadioConfigResponse>& in_radioConfigResponse,
      const std::shared_ptr<aidlconfig::IRadioConfigIndication>& in_radioConfigIndication);

// Config Request APIs
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlconfig::IRadioConfigResponse>& in_radioConfigResponse,
      const std::shared_ptr<aidlconfig::IRadioConfigIndication>& in_radioConfigIndication) override;
  ::ndk::ScopedAStatus getHalDeviceCapabilities(int32_t in_serial) override;
  ::ndk::ScopedAStatus getNumOfLiveModems(int32_t in_serial) override;
  ::ndk::ScopedAStatus getPhoneCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus getSimSlotsStatus(int32_t in_serial) override;
  ::ndk::ScopedAStatus setNumOfLiveModems(int32_t in_serial, int8_t in_numOfLiveModems) override;
  ::ndk::ScopedAStatus setPreferredDataModem(int32_t in_serial, int8_t in_modemId) override;
  ::ndk::ScopedAStatus setSimSlotsMapping(int32_t in_serial, const std::vector<aidlconfig::SlotPortMapping>& in_slotMap) override;

// Send Indication Functions

  void sendSlotStatusIndication(const std::shared_ptr<UimSlotStatusInd> msg);

};

