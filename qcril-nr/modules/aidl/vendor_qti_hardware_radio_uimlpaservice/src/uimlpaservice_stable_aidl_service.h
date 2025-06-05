/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "aidl/vendor/qti/hardware/radio/lpa/UimLpaIconType.h"
#include "aidl/vendor/qti/hardware/radio/lpa/BnUimLpa.h"
#include "aidl/vendor/qti/hardware/radio/lpa/UimLpaUserResp.h"
#include "aidl/vendor/qti/hardware/radio/lpa/IUimLpaResponse.h"
#include "aidl/vendor/qti/hardware/radio/lpa/IUimLpaIndication.h"
#include "interfaces/uim/UimLpaReqMessage.h"
#include "uim_lpa_stable_aidl_service_utils.h"
#include "interfaces/lpa/lpa_service_types.h"

#include "framework/legacy.h"
#include "QtiMutex.h"
#include "UimLpaServiceContext.h"

#undef TAG
#define TAG "RILQ"

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::lpa;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace implementation {

class IUimLpaImpl : public aidlimports::BnUimLpa {
 private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient;
  std::shared_ptr<aidlimports::IUimLpaResponse> mResponseCb;
  std::shared_ptr<aidlimports::IUimLpaIndication> mIndicationCb;

 protected:
  std::shared_ptr<aidlimports::IUimLpaResponse> getResponseCallback();
  std::shared_ptr<aidlimports::IUimLpaIndication> getIndicationCallback();
  void clearCallbacks_nolock();
  void clearCallbacks();

 public:
  IUimLpaImpl(qcril_instance_id_e_type instance);
  ~IUimLpaImpl();
  qcril_instance_id_e_type getInstanceId();
  void deathNotifier(void* cookie);
  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlimports::IUimLpaResponse>& in_uimLpaServiceRepsonse,
      const std::shared_ptr<aidlimports::IUimLpaIndication>& in_uimLpaServiceIndication);

  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<::aidl::vendor::qti::hardware::radio::lpa::IUimLpaResponse>&
          in_responseCallback,
      const std::shared_ptr<::aidl::vendor::qti::hardware::radio::lpa::IUimLpaIndication>&
          in_indicationCallback) override;

  ::ndk::ScopedAStatus uimLpaHttpTxnCompletedRequest(
      int32_t in_token,
      ::aidl::vendor::qti::hardware::radio::lpa::UimLpaResult in_result,
      const std::vector<uint8_t>& in_responsePayload,
      const std::vector<::aidl::vendor::qti::hardware::radio::lpa::UimLpaHttpCustomHeader>&
          in_customHeaders) override;

  ::ndk::ScopedAStatus uimLpaUserRequest(
      int32_t in_token,
      const ::aidl::vendor::qti::hardware::radio::lpa::UimLpaUserReq& in_userReq) override;

  void handleUimLpaUserRequest(int token, lpa_service_user_req_type* user_req_ptr);

  void uimLpaUserResponse(int32_t token, lpa_service_user_resp_type* user_resp);

  void uimLpaAddProfileProgressInd(lpa_service_add_profile_progress_ind_type* progInd);

  void uimLpaHttpTxnIndication(lpa_service_http_transaction_ind_type* txnInd);

  void uimLpaEndUserConsentIndication(lpa_service_user_consent_type* ind);

  void uimLpaHttpTxnCompletedResponse(int32_t token, lpa_service_result_type result);

  void handleUimLpaHttpTxnCompletedRequest(int token,
                                           lpa_service_http_transaction_req_type* http_req_ptr);

  void sendUimLpaHttpTxnFailIndication(int token);
};

}  // namespace implementation
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
