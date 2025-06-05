/*===========================================================================
   Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#pragma once

#include "QtiMutex.h"
#include "framework/legacy.h"
#include "framework/UnSolicitedMessage.h"

#include "MessageCommon.h"
#include "BnRadioDataBase.h"
#include "RadioDataContext.h"
#include "request/SetupDataCallRequestMessage.h"

#ifdef SET_LOCAL_URSP_CONFIG
#include "request/SetLocalUrspConfigurationRequestMessage.h"
#endif

#include "request/DeactivateDataCallRequestMessage.h"
#include "request/SetLocalUrspConfigurationRequestMessage.h"
#include "UnSolMessages/RadioDataCallListChangeIndMessage.h"
#include "UnSolMessages/ThrottledApnTimerExpirationMessage.h"
#include "UnSolMessages/RadioKeepAliveStatusIndMessage.h"
#include "UnSolMessages/SlicingConfigChangedIndMessage.h"
#include "UnSolMessages/RilPCODataMessage.h"
#include "interfaces/RilAcknowledgeRequestMessage.h"

using namespace aidl::android::hardware::radio;
using namespace aidl::android::hardware::radio::data;

class RadioDataServiceImpl : public BnRadioDataBase {
 private:
  string mName = "RadioDataServiceImpl";
  std::shared_ptr<IRadioDataResponse> mIRadioDataResponse;
  std::shared_ptr<IRadioDataIndication> mIRadioDataIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;

 protected:
  std::shared_ptr<IRadioDataResponse> getResponseCallback();
  std::shared_ptr<IRadioDataIndication> getIndicationCallback();

 public:
  RadioDataServiceImpl(qcril_instance_id_e_type instance);
  ~RadioDataServiceImpl();

  android::status_t registerService();

  void setResponseFunctions_nolock(
    const std::shared_ptr<IRadioDataResponse>& radioDataResponse,
    const std::shared_ptr<IRadioDataIndication>& radioDataIndication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<RadioDataContext> getContext(uint32_t serial) {
    std::shared_ptr<RadioDataContext> ctx = std::make_shared<RadioDataContext>(mInstanceId, serial);
    return ctx;
  }

  void deathNotifier();

  // AIDL APIs
  ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<IRadioDataResponse>& in_radioDataResponse, const std::shared_ptr<IRadioDataIndication>& in_radioDataIndication) override;
  ::ndk::ScopedAStatus setupDataCall(int32_t in_serial, ::aidl::android::hardware::radio::AccessNetwork in_accessNetwork,
                                    const ::aidl::android::hardware::radio::data::DataProfileInfo& in_dataProfileInfo, bool in_roamingAllowed,
                                    ::aidl::android::hardware::radio::data::DataRequestReason in_reason,
                                    const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& in_addresses,
                                    const std::vector<std::string>& in_dnses, int32_t in_pduSessionId, const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& in_sliceInfo,
                                    bool in_matchAllRuleAllowed) override;
  ::ndk::ScopedAStatus setLocalUrspConfiguration(int32_t serial, std::vector<UrspRule> urspRules);
  ::ndk::ScopedAStatus deactivateDataCall(int32_t serial, int32_t cid, ::aidl::android::hardware::radio::data::DataRequestReason requestReason) override;
  ::ndk::ScopedAStatus getDataCallList(int32_t serial) override;
  ::ndk::ScopedAStatus setInitialAttachApn(int32_t serial, const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo>& dataProfileInfo) override;
  ::ndk::ScopedAStatus setDataProfile(int32_t in_serial, const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& in_profiles) override;
  ::ndk::ScopedAStatus setDataThrottling(int32_t serial, ::aidl::android::hardware::radio::data::DataThrottlingAction dataThrottlingAction, int64_t completionDurationMillis) override;
  ::ndk::ScopedAStatus setDataAllowed(int32_t serial, bool allow) override;
  ::ndk::ScopedAStatus startKeepalive(int32_t serial, const ::aidl::android::hardware::radio::data::KeepaliveRequest& in_keepalive) override;
  ::ndk::ScopedAStatus stopKeepalive(int32_t serial, int32_t sessionHandle) override;
  ::ndk::ScopedAStatus responseAcknowledgement() override;
  ::ndk::ScopedAStatus allocatePduSessionId(int32_t in_serial) override;
  ::ndk::ScopedAStatus releasePduSessionId(int32_t serial, int32_t id) override;
  ::ndk::ScopedAStatus startHandover(int32_t serial, int32_t callId) override;
  ::ndk::ScopedAStatus cancelHandover(int32_t serial, int32_t callId) override;
  ::ndk::ScopedAStatus getSlicingConfig(int32_t serial) override;

  #ifdef SET_LOCAL_URSP_CONFIG
  ::ndk::ScopedAStatus setLocalUrspConfiguration(int32_t serial, std::vector<UrspRule> urspRules);
  #endif


  //response APIs
  void sendResponseForSetupDataCall(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg);
  void sendResponseForDeactivateDataCall(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg);
  void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr, int serial, Message::Callback::Status status);
  void setInitialAttachApnResponse(RadioResponseInfo responseInfo);
  void setDataProfileResponse(RadioResponseInfo responseInfo);
  void sendResponseForSetDataThrottling(int32_t serial,RIL_Errno errorCode);
  void sendResponseForSetDataAllowed( int32_t serial, RIL_Errno errorCode);
  void sendAcknowledgeRequest(int32_t serial);
  void sendResponseForReleasePduSessionId(int32_t serial, RIL_Errno errorCode);
  void sendStartKeepAliveResponse(RadioResponseInfo responseInfo, KeepaliveStatus result);
  void sendStopKeepAliveResponse(RadioResponseInfo responseInfo);
  void sendResponseForAllocatePduSessionId(int32_t serial, RIL_Errno errorCode);
  void sendResponseForCancelHandover(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetSlicingConfig(  RadioResponseInfo respInfo, rildata::SlicingConfig_t slicingConfig);
  void sendResponseForStartHandover(int32_t serial, RIL_Errno errorCode);

  //indication APIs
  void sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg);
  void sendUnthrottleApnMessage(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg);
  void sendKeepAliveStatusInd(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg);
  void sendRilPCODataInd(std::shared_ptr<rildata::RilPCODataMessage> msg);
  void sendSlicingConfigChange(std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg);

};
