/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "uimlpaservice_stable_aidl_service.h"
#include <framework/Log.h>
#include "LpaUimReqMessageCallback.h"
#include "interfaces/lpa/LpaUimHttpRequestMsg.h"
#include "LpaQmiUimHttpRequestCallback.h"
#include "interfaces/lpa/LpaUimHttpTxnFailIndMsg.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace implementation {

IUimLpaImpl::IUimLpaImpl(qcril_instance_id_e_type instance) : mInstanceId(instance)
{
}

IUimLpaImpl::~IUimLpaImpl()
{
}

qcril_instance_id_e_type IUimLpaImpl::getInstanceId()
{
  return mInstanceId;
}

std::shared_ptr<aidlimports::IUimLpaResponse> IUimLpaImpl::getResponseCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

std::shared_ptr<aidlimports::IUimLpaIndication> IUimLpaImpl::getIndicationCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void IUimLpaImpl::clearCallbacks_nolock()
{
  QCRIL_LOG_DEBUG("clearCallbacks_nolock");
  mResponseCb = nullptr;
  mIndicationCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void IUimLpaImpl::clearCallbacks()
{
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

static void deathRecpCallback(void* cookie)
{
  IUimLpaImpl* uimLpaImpl = static_cast<IUimLpaImpl*>(cookie);
  if (uimLpaImpl != nullptr) {
    uimLpaImpl->deathNotifier(cookie);
  }
}

::ndk::ScopedAStatus IUimLpaImpl::setCallback(
    const std::shared_ptr<::aidl::vendor::qti::hardware::radio::lpa::IUimLpaResponse>&
        in_responseCallback,
    const std::shared_ptr<::aidl::vendor::qti::hardware::radio::lpa::IUimLpaIndication>&
        in_indicationCallback)
{
  QCRIL_LOG_DEBUG("IUimLpaService::setResposeFuntions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(
        mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
  }
  setResponseFunctions_nolock(in_responseCallback, in_indicationCallback);
  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(
          mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

void IUimLpaImpl::deathNotifier(void*)
{
  QCRIL_LOG_DEBUG("IUimLpaService::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

void IUimLpaImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlimports::IUimLpaResponse>& in_uimLpaServiceRepsonse,
    const std::shared_ptr<aidlimports::IUimLpaIndication>& in_uimLpaServiceIndication)
{
  QCRIL_LOG_DEBUG("IUimLpaService::setResponseFunctions_nolock");
  mResponseCb = in_uimLpaServiceRepsonse;
  mIndicationCb = in_uimLpaServiceIndication;
}

void IUimLpaImpl::uimLpaUserResponse(int32_t token, lpa_service_user_resp_type* user_resp)
{
  UimLpaProfileInfo* profiles = NULL;
  uint8_t p_count = 0;

  auto respCb = getResponseCallback();
  UimLpaUserResp userResp {};
  if (respCb == NULL || user_resp == NULL) {
    QCRIL_LOG_ERROR("UimLpaImpl::uimLpaUserResponse responseCb is null or userResp NULL");
    return;
  }

  QCRIL_LOG_INFO("UimLpaImpl::uimLpaUserResponse token=%d event=%d", token, user_resp->event);

  userResp.event = (UimLpaUserEventId)user_resp->event;
  userResp.result = (UimLpaResult)user_resp->result;

  /**
   * userResp.eid is std::vector<uint8_t>
   * user_resp->eid is uint8_t*
   */
  for (int i = 0; i < user_resp->eid_len; i++) {
    userResp.eid.push_back(user_resp->eid[i]);
  }

  if (user_resp->no_of_profiles != 0 && user_resp->profiles != NULL) {
    profiles = new UimLpaProfileInfo[user_resp->no_of_profiles];

    if (profiles == NULL) {
      QCRIL_LOG_ERROR("Allocation failed");
      return;
    }
    for (p_count = 0; p_count < user_resp->no_of_profiles; p_count++) {
      profiles[p_count].state = static_cast<UimLpaProfileState>(user_resp->profiles[p_count].state);

      /**
       * user_resp->profiles[].iccid is uint8_t*
       * profiles[].iccid is std::vector<uint8_t>
       */
      for (int i = 0; i < user_resp->profiles[p_count].iccid_len; i++) {
        profiles[p_count].iccid.push_back(user_resp->profiles[p_count].iccid[i]);
      }
      if (user_resp->profiles[p_count].profileName != NULL) {
          profiles[p_count].profileName = user_resp->profiles[p_count].profileName;
      }
      else {
        profiles[p_count].profileName = "";
      }
      if (user_resp->profiles[p_count].nickName != NULL) {
          profiles[p_count].nickName = user_resp->profiles[p_count].nickName;
      }
      else {
        profiles[p_count].nickName = "";
      }
      if (user_resp->profiles[p_count].spName != NULL) {
          profiles[p_count].spName = user_resp->profiles[p_count].spName;
      }
      else {
        profiles[p_count].spName = "";
      }
      profiles[p_count].iconType =
          static_cast<UimLpaIconType>(user_resp->profiles[p_count].iconType);

      /**
       * user_resp->profiles[].icon is uint8_t*
       * profiles[].icon is std::vector<uint8_t>
       */
      for (int i = 0; i < user_resp->profiles[p_count].icon_len; i++) {
        profiles[p_count].icon.push_back(user_resp->profiles[p_count].icon[i]);
      }

      profiles[p_count].profileClass =
          static_cast<UimLpaProfileClassType>(user_resp->profiles[p_count].profileClass);
      profiles[p_count].profilePolicy =
          static_cast<UimLpaProfilePolicyMask>(user_resp->profiles[p_count].profilePolicy);
    }
  }

  /**
   * userResp.profiles is std::vector<UimLpaProfileInfo>
   * profiles is UimLpaProfileInfo*
   * Profiles pointer condition check was added to prevent
     KW issue when user_reqp->no_of_profiles is non zero
     and user_resp->profile is null
   */

  if (profiles != NULL) {
    for (int i = 0; i < user_resp->no_of_profiles; i++) {
      userResp.profiles.push_back(profiles[i]);
    }
  }


  if (user_resp->srvAddr.smdpAddress != NULL) {
    userResp.srvAddr.smdpAddress = user_resp->srvAddr.smdpAddress;
  } else {
    userResp.srvAddr.smdsAddress = "";
  }
  if (user_resp->srvAddr.smdsAddress != NULL) {
    userResp.srvAddr.smdsAddress = user_resp->srvAddr.smdsAddress;
  } else {
    userResp.srvAddr.smdsAddress = "";
  }

  for (int i = 0;i< user_resp->euicc_info2_len;i++)
  {
    userResp.euicc_info2.push_back(user_resp->euicc_info2[i]);
  }
  auto ret = respCb->uimLpaUserResponse(token, userResp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
  if (profiles != NULL) {
    delete[] profiles;
    profiles = NULL;
  }
  return;
}

/*===========================================================================

FUNCTION:  uimLpaHttpTxnCompletedResponse

===========================================================================*/
void IUimLpaImpl::uimLpaHttpTxnCompletedResponse(int32_t token, lpa_service_result_type result)
{
  auto respCb = getResponseCallback();
  if (respCb == NULL) {
    QCRIL_LOG_ERROR("uimLpaHttpTxnCompletedResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("UimLpaHttpTxnCompletedResponse token=%d result=%d", token, result);
  auto ret = respCb->uimLpaHttpTxnCompletedResponse(token, (UimLpaResult)result);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
} /* uimLpaHttpTxnCompletedResponse */

void IUimLpaImpl::handleUimLpaUserRequest(int token, lpa_service_user_req_type* user_req_ptr)
{
  QCRIL_LOG_DEBUG("handleUimLpaUserRequest token=%d", token);

  std::shared_ptr<UimLpaReqMessage> lpa_req_ptr = nullptr;
  LpaUimReqMessageCallback lpaUimReqcb("UIM LPA REQUEST");
  lpa_service_user_resp_type resp_data;

  memset(&resp_data, 0x00, sizeof(resp_data));
  resp_data.result = LPA_SERVICE_RESULT_FAILURE;

  if (user_req_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null data");
    uimLpaUserResponse(token, &resp_data);
    return;
  }

  QCRIL_LOG_INFO("UimLpaUserRequest: token_id: 0x%x", token);

  lpa_req_ptr = std::make_shared<UimLpaReqMessage>(
      UIM_LPA_USER_REQUEST_ID, &lpaUimReqcb, user_req_ptr, token);

  if (lpa_req_ptr != nullptr) {
    lpa_req_ptr->dispatch();
  } else {
    resp_data.event = user_req_ptr->event;
    uimLpaUserResponse(token, &resp_data);
  }
} /* handleUimLpaUserRequest */

void IUimLpaImpl::handleUimLpaHttpTxnCompletedRequest(
    int token, lpa_service_http_transaction_req_type* http_req_ptr)
{
  std::shared_ptr<LpaUimHttpRequestMsg> lpa_req_ptr = nullptr;
  LpaQmiUimHttpRequestCallback lpaHttpReqcb("QMI UIM HTTP REQUEST");

  if (http_req_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null data");
    uimLpaHttpTxnCompletedResponse(token, LPA_SERVICE_RESULT_FAILURE);
    return;
  }

  QCRIL_LOG_INFO("handleUimLpaHttpTxnCompletedRequest: token_id: 0x%x", token);

  lpa_req_ptr = std::make_shared<LpaUimHttpRequestMsg>(
      http_req_ptr, token, LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED, &lpaHttpReqcb);

  if (lpa_req_ptr != nullptr) {
    lpa_req_ptr->dispatch();
  } else {
    uimLpaHttpTxnCompletedResponse(token, LPA_SERVICE_RESULT_FAILURE);
  }
} /* handleUimLpaHttpTxnCompletedRequest */

::ndk::ScopedAStatus IUimLpaImpl::uimLpaHttpTxnCompletedRequest(
    int32_t in_token,
    ::aidl::vendor::qti::hardware::radio::lpa::UimLpaResult in_result,
    const std::vector<uint8_t>& in_responsePayload,
    const std::vector<::aidl::vendor::qti::hardware::radio::lpa::UimLpaHttpCustomHeader>&
        in_customHeaders)
{
  lpa_service_http_transaction_req_type http_req;
  memset(&http_req, 0x00, sizeof(http_req));
  http_req.tokenId = in_token;
  http_req.result = static_cast<lpa_service_result_type>(in_result);
  http_req.payload = in_responsePayload.data();
  http_req.payload_len = in_responsePayload.size();

  QCRIL_LOG_INFO("UimLpaHttpTxnCompletedRequest: token_id: 0x%x", in_token);

  if (in_customHeaders.size() != 0 && in_customHeaders.data() != 0) {
    http_req.customHeaders = new lpa_service_http_custom_header_req_type[in_customHeaders.size()]{};
    if (http_req.customHeaders != NULL) {
      uint8_t itr = 0;
      const UimLpaHttpCustomHeader* headers = in_customHeaders.data();
      for (; itr < in_customHeaders.size(); itr++) {
        http_req.customHeaders[itr].headerName = headers[itr].headerName.c_str();
        http_req.customHeaders[itr].headerValue = headers[itr].headerValue.c_str();
      }
      http_req.no_of_headers = in_customHeaders.size();
    }
  }

  handleUimLpaHttpTxnCompletedRequest(in_token, &http_req);

  if (http_req.customHeaders != NULL) {
    delete[] http_req.customHeaders;
    http_req.customHeaders = NULL;
  }
  return ndk::ScopedAStatus::ok();
} /* UimLpaHttpTxnCompletedRequest */

::ndk::ScopedAStatus IUimLpaImpl::uimLpaUserRequest(
    int32_t in_token, const ::aidl::vendor::qti::hardware::radio::lpa::UimLpaUserReq& in_userReq)
{
  lpa_service_user_req_type user_req;
  memset(&user_req, 0x00, sizeof(user_req));
  QCRIL_LOG_DEBUG("UimLpaUserRequest: token_id: -1x%x, event: %d", in_token, in_userReq.event);
  user_req.event = static_cast<lpa_service_user_event_type>(in_userReq.event);
  user_req.activationCode = in_userReq.activationCode.c_str();
  user_req.confirmationCode = in_userReq.confirmationCode.c_str();
  user_req.nickname = in_userReq.nickname.c_str();
  user_req.iccid = in_userReq.iccid.data();
  user_req.iccid_len = in_userReq.iccid.size();
  user_req.resetMask = in_userReq.resetMask;
  user_req.userOk = in_userReq.userOk;
  user_req.srvOpReq.opCode = static_cast<lpa_service_srv_addr_op_type>(in_userReq.srvOpReq.opCode);
  user_req.srvOpReq.smdpAddress = in_userReq.srvOpReq.smdpAddress.c_str();
  handleUimLpaUserRequest(in_token, &user_req);
  return ndk::ScopedAStatus::ok();
} /* UimLpaImpl::UimLpaUserRequest */

void IUimLpaImpl::uimLpaAddProfileProgressInd(lpa_service_add_profile_progress_ind_type* progInd)
{
  auto indCb = getIndicationCallback();
  if (indCb == NULL || progInd == NULL) {
    QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd indicationCb is null or progInd");
    return;
  } else {
    UimLpaAddProfileProgressInd progress;
    memset(&progress, 0x00, sizeof(progress));
    progress.status = static_cast<UimLpaAddProfileStatus>(progInd->status);
    progress.cause = static_cast<UimLpaAddProfileFailureCause>(progInd->cause);
    progress.progress = progInd->progress;
    progress.policyMask = static_cast<UimLpaProfilePolicyMask>(progInd->policyMask);
    progress.userConsentRequired = progInd->userConsentRequired;
    auto ret = indCb->uimLpaAddProfileProgressIndication(progress);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
} /* UimLpaImpl::uimLpaAddProfileProgressInd */

void IUimLpaImpl::uimLpaEndUserConsentIndication(lpa_service_user_consent_type* ind)
{
  auto indCb = getIndicationCallback();

  if (ind == NULL) {
    QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd progInd is null");
    return;
  }
  if (indCb != NULL) {
    UimLpaUserConsentType user_consent_type;

    user_consent_type = utils::convertLpaServiceTypesToUserConsentType(ind->user_consent_type);
    auto ret = indCb->uimLpaEndUserConsentIndication(user_consent_type);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IUimLpaImpl::sendUimLpaHttpTxnFailIndication(int token)
{
  std::shared_ptr<LpaUimHttpTxnFailIndMsg> httpTxnFailInd = nullptr;

  QCRIL_LOG_INFO("Sending Http Txn failure ind for token : %d ", token);

  httpTxnFailInd = std::make_shared<LpaUimHttpTxnFailIndMsg>(token);

  if (httpTxnFailInd != nullptr) {
    httpTxnFailInd->broadcast();
  }
}

void IUimLpaImpl::uimLpaHttpTxnIndication(lpa_service_http_transaction_ind_type* txnInd)
{
  UimLpaHttpTransactionInd httpTxnInd;
  uint8_t h_count = 0;
  UimLpaHttpCustomHeader* headers = NULL;
  if (txnInd == NULL) {
    QCRIL_LOG_ERROR("uimLpaHttpTxnIndication is null");
    return;
  }
  auto indCb = getIndicationCallback();

  if (indCb == NULL) {
    QCRIL_LOG_ERROR("uimLpaHttpTxnIndication indicationCb is null");
    if (txnInd != NULL) {
      sendUimLpaHttpTxnFailIndication(txnInd->tokenId);
    }
    return;
  }
  memset(&httpTxnInd, 0x00, sizeof(httpTxnInd));
  httpTxnInd.tokenId = txnInd->tokenId;

  /**
   * httpTxnInd.payload is std::vector<uint8_t>
   * txnInd->payload is uint8_t*
   */

  for (int i = 0; i < txnInd->payload_len; i++) {
    httpTxnInd.payload.push_back(txnInd->payload[i]);
  }

  QCRIL_LOG_INFO("uimLpaHttpTxnIndication: token_id: 0x%x, payload_len: %d",
                 httpTxnInd.tokenId,
                 txnInd->payload_len);

  if (txnInd->contentType != NULL) {
    httpTxnInd.contentType = txnInd->contentType;
  } else {
    std::string str = {};
    str.resize(0);
    httpTxnInd.contentType = str;
  }

  if (txnInd->no_of_headers != 0) {
    headers = new UimLpaHttpCustomHeader[txnInd->no_of_headers];

    if (headers == NULL) {
      QCRIL_LOG_ERROR("Allocation failed for headers");
    }

    if (txnInd->customHeaders != NULL) {
      for (h_count = 0; h_count < txnInd->no_of_headers; h_count++) {
        headers[h_count].headerName = txnInd->customHeaders[h_count].headerName;
        headers[h_count].headerValue = txnInd->customHeaders[h_count].headerValue;
      }
    }
  }

  /**
   * headers is UimLpaHttpCustomHeader*
   * httpTxnInd.customHeaders std::vector<UimLpaHttpCustomHeader>
   */
  for (int i = 0; i < txnInd->no_of_headers; i++) {
    httpTxnInd.customHeaders.push_back(headers[i]);
  }
  httpTxnInd.url = txnInd->url;
  auto ret = indCb->uimLpaHttpTxnIndication(httpTxnInd);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
  if (headers != NULL) {
    delete[] headers;
    headers = NULL;
  }
} /* uimLpaHttpTxnIndication */

}  // namespace implementation
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
