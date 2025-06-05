/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/legacy.h"
#include "QtiMutex.h"
#include "binder/IBinder.h"
#include "BnQtiRadioConfigDefault.h"
#include "IQtiRadioConfigContext.h"
#include "interfaces/securemode/RilRequestGetSecureModeStatusMessage.h"
#include "interfaces/securemode/SecureModeStateChangeIndMessage.h"
#include "interfaces/securemode/securemode_types.h"
#include "request/GetCIWlanCapabilityMessage.h"
#include <memory>
#include "UnSolMessages/DataCapabilityChangeIndMessage.h"
#include "UnSolMessages/DataPPDataCapabilityChangeIndMessage.h"
#include "UnSolMessages/DataSubRecommendationIndMessage.h"
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/MsimPreference.h>
#include "qti_radio_config_aidl_service_utils.h"
#include "interfaces/uim/UimGetSimTypeRequestMsg.h"
#include "interfaces/uim/UimSetSimTypeRequestMsg.h"
#include "interfaces/uim/qcril_uim_types.h"

namespace aidlqtiradioconfigimpl {
using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig::implementation;
}

namespace aidlqtiradioconfig {
using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig;
}

namespace aidlqtiradiocommon {
using namespace aidl::vendor::qti::hardware::radio;
}

class IQtiRadioConfigImpl : public aidlqtiradioconfigimpl::BnQtiRadioConfigDefault {
private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient{nullptr};
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication> mIndicationCb;
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> mResponseCb;

protected:
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> getResponseCallback();
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();

  void sendResponseForGetSecureModeStatus(int32_t in_serial,
    RIL_Errno errorCode, bool status);
  void sendResponseForSetMsimPreference(int32_t in_serial,
    RIL_Errno errorCode);
  void sendResponseForGetSimTypeInfo(int32_t in_serial, RIL_Errno errorCode,
                                     std::shared_ptr<RIL_UIM_GetSimTypeResp> respData);
  void sendResponseForSetSimType(int32_t in_serial, RIL_Errno errorCode);
  void sendResponseForGetCiwlanCapability(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<rildata::CIWlanCapabilitySupport_t> resp);
  void sendResponseForGetDualDataCapability(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<bool> resp);
  void sendResponseForSetDualDataUserPreference(int32_t in_serial,
    RIL_Errno errorCode);
  void sendResponseForGetDdsSwitchCapability(int32_t serial, RIL_Errno errorCode, bool support);
  void sendResponseForSendUserPreferenceForDataDuringVoiceCall(int32_t serial, RIL_Errno errorCode);

 public:
  IQtiRadioConfigImpl() = delete;
  IQtiRadioConfigImpl(qcril_instance_id_e_type instance);
  ~IQtiRadioConfigImpl();

  android::status_t registerService();

  void deathNotifier(void* cookie);

  std::shared_ptr<IQtiRadioConfigContext> getContext(uint32_t serial);

  qcril_instance_id_e_type getInstanceId();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse>& in_radioConfigResponse,
      const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication>& in_radioConfigIndication);

// Config Request APIs
  ::ndk::ScopedAStatus setCallbacks(
      const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse>& in_radioConfigResponse,
      const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication>& in_radioConfigIndication) override;
  ::ndk::ScopedAStatus getSecureModeStatus(int32_t in_serial) override;
  ::ndk::ScopedAStatus setMsimPreference(int32_t in_serial,
      aidlqtiradioconfig::MsimPreference pref) override;
  ::ndk::ScopedAStatus getSimTypeInfo(int32_t in_serial) override;
  ::ndk::ScopedAStatus setSimType(int32_t in_serial,
                                  const std::vector<aidlqtiradioconfig::SimType>& simType) override;

  ::ndk::ScopedAStatus getDualDataCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus setDualDataUserPreference(int32_t in_serial, bool enable) override;
  ::ndk::ScopedAStatus getCiwlanCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus getDdsSwitchCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus sendUserPreferenceForDataDuringVoiceCall(int32_t in_serial,
       const std::vector<bool>& isAllowedOnSlot) override;
  ::ndk::ScopedAStatus isFeatureSupported(int feature, bool* _aidl_return);

// Send Indication Functions
  void sendOnSecureModeIndication(const std::shared_ptr<SecureModeStateChangeIndMessage> msg);
  void sendCiwlanCapabilityChanged(std::shared_ptr<rildata::DataCapabilityChangeIndMessage> msg);
  void sendDualDataCapabilityChanged(
      std::shared_ptr<rildata::DataPPDataCapabilityChangeIndMessage> msg);
  void sendDualDataRecommendation(std::shared_ptr<rildata::DataSubRecommendationIndMessage> msg);

  template <typename R>
  bool isFeatureSupportedByModule(int feature, std::shared_ptr<R> msg);
};
