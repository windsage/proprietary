/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "UimLpaServiceAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <framework/Log.h>
#include "interfaces/uim/qcril_uim_lpa.h"
#include "modules/qmi/UimHttpModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiUimHttpSetupRequestCallback.h"

#ifdef QMI_RIL_UTF
#include "UimHttpModemEndPointModule.h"
#endif

static load_module<UimLpaServiceAidlModule> sUimLpaServiceAidlModule;

UimLpaServiceAidlModule::UimLpaServiceAidlModule()
{
  mName = "UimLpaServiceAidlModule";
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, UimLpaServiceAidlModule::handleQcrilInit),
    HANDLER(aidlimplimports::LpaQmiUimHttpResponseMsg,
            UimLpaServiceAidlModule::handleQmiUimHttpResponse),
    HANDLER(LpaQmiUimHttpIndicationMsg, UimLpaServiceAidlModule::handleQmiUimHttpIndication),
    HANDLER(aidlimplimports::UimLpaRespMessage,
            UimLpaServiceAidlModule::handleUimLpaResponseMessage),
    HANDLER(UimLpaIndicationMsg, UimLpaServiceAidlModule::handleUimLpaIndicationMessage),
  };
}

UimLpaServiceAidlModule::~UimLpaServiceAidlModule()
{
}

void UimLpaServiceAidlModule::init()
{
  Module::init();
  ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint();
}

void UimLpaServiceAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg)
{
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));

  if (mIUimLpaImpl == nullptr) {
    mIUimLpaImpl = ndk::SharedRefBase::make<aidlimplimports::IUimLpaImpl>(instance_id);
    std::string serviceInstance = std::string(aidlimplimports::IUimLpaImpl::descriptor) +
                                  "/UimLpa" + std::to_string(instance_id);

    binder_status_t status =
        AServiceManager_addService(mIUimLpaImpl->asBinder().get(), serviceInstance.c_str());

    QCRIL_LOG_INFO("UimLpaService stable AIDL addService, status = %d", status);
    if (status != STATUS_OK) {
      mIUimLpaImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }

  QmiUimHttpSetupRequestCallback qmiSetupCallback("Uim-Module-Token");
  ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
        "Client-Uim-http-Serv-Token", msg->get_instance_id(), &qmiSetupCallback);
}

void UimLpaServiceAidlModule::handleUimLpaIndicationMessage(
    std::shared_ptr<UimLpaIndicationMsg> msg_ptr)
{
  if (msg_ptr == nullptr) {
    return;
  }

  QCRIL_LOG_ERROR("Received lpa indication %d", msg_ptr->get_ind_id());
  switch (msg_ptr->get_ind_id()) {
    case UIM_LPA_PROFILE_PROGRESS_IND_ID:
      if (msg_ptr->get_data_ptr() != nullptr) {
        qcril_uim_lpa_add_profile_progress_ind_type* ind_data_ptr =
            static_cast<qcril_uim_lpa_add_profile_progress_ind_type*>(msg_ptr->get_data_ptr());
        lpa_service_add_profile_progress_ind_type lpa_ind;

        memset(&lpa_ind, 0x00, sizeof(lpa_ind));
        lpa_ind.status = static_cast<lpa_service_add_profile_status_type>(ind_data_ptr->status);
        lpa_ind.cause = static_cast<lpa_service_add_profile_failure_cause>(ind_data_ptr->cause);
        lpa_ind.progress = ind_data_ptr->progress;
        lpa_ind.policyMask = static_cast<lpa_service_profile_policy_mask>(ind_data_ptr->policyMask);
        lpa_ind.userConsentRequired = ind_data_ptr->userConsentRequired;
        lpa_ind.profile_name = ind_data_ptr->profile_name;
        lpa_ind.user_consent_type =
            static_cast<lpa_user_consent_type>(ind_data_ptr->user_consent_type);

        if (mIUimLpaImpl != nullptr) {
          mIUimLpaImpl->uimLpaAddProfileProgressInd(&lpa_ind);
        }
      }
      break;
    case UIM_LPA_USER_CONSENT_IND_ID:
      if (msg_ptr->get_data_ptr() != nullptr) {
        qcril_uim_lpa_user_consent_ind_type* ind_data_ptr =
            static_cast<qcril_uim_lpa_user_consent_ind_type*>(msg_ptr->get_data_ptr());
        lpa_service_user_consent_type user_consent_ind;
        memset(&user_consent_ind, 0, sizeof(user_consent_ind));
        user_consent_ind.user_consent_type =
            static_cast<lpa_user_consent_type>(ind_data_ptr->user_consent_type);

        if (mIUimLpaImpl != nullptr) {
          mIUimLpaImpl->uimLpaEndUserConsentIndication(&user_consent_ind);
        }
      }
      break;
    default:
      break;
  }
}

void UimLpaServiceAidlModule::handleQmiUimHttpIndication(
    std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg_ptr)
{
  if (msg_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  QCRIL_LOG_INFO("QMI UIM HTTP Indication : %d", msg_ptr->get_ind_id());

  switch (msg_ptr->get_ind_id()) {
    case QMI_UIM_HTTP_SRVC_UP_IND_MSG:
      break;
    case QMI_UIM_HTTP_SRVC_DOWN_IND_MSG:
      break;
    case QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG: {
      uim_http_transaction_ind_msg* ind_ptr =
          static_cast<uim_http_transaction_ind_msg*>(msg_ptr->get_message());
      lpa_service_http_transaction_ind_type* svc_ind_ptr = nullptr;

      if (ind_ptr == nullptr) {
        QCRIL_LOG_ERROR("Invalid null ind_ptr");
        break;
      }

      svc_ind_ptr = new lpa_service_http_transaction_ind_type;

      if (svc_ind_ptr == nullptr) {
        QCRIL_LOG_ERROR("Invalid memory failure");
        break;
      }
      memset(svc_ind_ptr, 0x00, sizeof(lpa_service_http_transaction_ind_type));

      svc_ind_ptr->tokenId = ind_ptr->token_id;
      svc_ind_ptr->payload_len = ind_ptr->payload_len;
      svc_ind_ptr->payload = ind_ptr->payload_ptr;
      if (ind_ptr->url_valid) {
        svc_ind_ptr->url = ind_ptr->url;
      }
      if (ind_ptr->headers_valid) {
        uint32_t i = 0;

        svc_ind_ptr->contentType = ind_ptr->headers.content_type;
        svc_ind_ptr->no_of_headers =
            (ind_ptr->headers.custom_header_len < UIM_HTTP_CUST_HEADER_MAX_COUNT)
                ? ind_ptr->headers.custom_header_len
                : UIM_HTTP_CUST_HEADER_MAX_COUNT;

        svc_ind_ptr->customHeaders =
            new lpa_service_http_custom_header_type[svc_ind_ptr->no_of_headers];

        if (svc_ind_ptr->customHeaders == nullptr) {
          svc_ind_ptr->no_of_headers = 0;
        } else {
          for (i = 0; i < svc_ind_ptr->no_of_headers; i++) {
            svc_ind_ptr->customHeaders[i].headerName = ind_ptr->headers.custom_header[i].name;
            svc_ind_ptr->customHeaders[i].headerValue = ind_ptr->headers.custom_header[i].value;
          }
        }
      }

      if (mIUimLpaImpl != nullptr) {
        mIUimLpaImpl->uimLpaHttpTxnIndication(svc_ind_ptr);
      }

      if (svc_ind_ptr->customHeaders != nullptr) {
        delete[] svc_ind_ptr->customHeaders;
        svc_ind_ptr->customHeaders = nullptr;
      }
    } break;
    default:
      break;
  }
}

void UimLpaServiceAidlModule::handleQmiUimHttpResponse(
    std::shared_ptr<aidlimplimports::LpaQmiUimHttpResponseMsg> msg_ptr)
{
  lpa_qmi_uim_http_rsp_data_type* rsp_data_ptr = nullptr;
  lpa_service_result_type http_result = LPA_SERVICE_RESULT_SUCCESS;

  if (msg_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  rsp_data_ptr = msg_ptr->get_message();
  if (rsp_data_ptr == nullptr || rsp_data_ptr->rsp_id == LPA_QMI_UIM_HTTP_INVALID_RSP) {
    QCRIL_LOG_ERROR("Invalid null response data");
    return;
  }

  if (rsp_data_ptr->qmi_error_code != 0 || rsp_data_ptr->transp_err != 0) {
    http_result = LPA_SERVICE_RESULT_FAILURE;
  }
  if (mIUimLpaImpl != nullptr) {
    mIUimLpaImpl->uimLpaHttpTxnCompletedResponse(rsp_data_ptr->token, http_result);
  }
} /* handleQmiUimHttpResponse */

void UimLpaServiceAidlModule::handleUimLpaResponseMessage(
    std::shared_ptr<aidlimplimports::UimLpaRespMessage> msg_ptr)
{
  if (msg_ptr == nullptr) {
    return;
  }

  switch (msg_ptr->get_response_id()) {
    case UIM_LPA_USER_RESP_ID: {
      lpa_service_user_resp_type resp_data;

      memset(&resp_data, 0x00, sizeof(lpa_service_http_transaction_ind_type));
      if (msg_ptr->get_message_ptr() != nullptr) {
        qcril_uim_lpa_user_resp_type* resp_ptr =
            static_cast<qcril_uim_lpa_user_resp_type*>(msg_ptr->get_message_ptr());

        resp_data.event = static_cast<lpa_service_user_event_type>(resp_ptr->event);
        resp_data.result = static_cast<lpa_service_result_type>(resp_ptr->result);
        resp_data.eid = resp_ptr->eid;
        resp_data.eid_len = resp_ptr->eid_len;
        resp_data.profiles = (lpa_service_profile_info_type*)resp_ptr->profiles;
        resp_data.no_of_profiles = resp_ptr->no_of_profiles;
        resp_data.srvAddr.smdpAddress = resp_ptr->srvAddr.smdpAddress;
        resp_data.srvAddr.smdsAddress = resp_ptr->srvAddr.smdsAddress;
        resp_data.euicc_info2 = resp_ptr->euicc_info2;
        resp_data.euicc_info2_len = resp_ptr->euicc_info2_len;
      } else {
        resp_data.result = LPA_SERVICE_RESULT_FAILURE;
      }

      if (mIUimLpaImpl != nullptr) {
        mIUimLpaImpl->uimLpaUserResponse(msg_ptr->get_token(), &resp_data);
      }
    } break;
    default:
      break;
  }
} /* handleUimLpaResponseMessage */

#ifdef QMI_RIL_UTF
void UimLpaServiceAidlModule::cleanup()
{
    std::shared_ptr<UimHttpModemEndPoint> mUimHttpModemEndPoint =
                    ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint();
    UimHttpModemEndPointModule* mUimHttpModemEndPointModule =
                   (UimHttpModemEndPointModule*)mUimHttpModemEndPoint->mModule;
    mUimHttpModemEndPointModule->cleanUpQmiSvcClient();
}

void qcril_qmi_uim_lpa_service_aidl_module_cleanup()
{
  sUimLpaServiceAidlModule.get_module().cleanup();
}
#endif
