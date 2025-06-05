/******************************************************************************
  @file    ril_utf_i_ims_aidl_radio_sim.cpp
  @brief   RIL UTF IMS AIDL implementation

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "ibinder_internal.h"
#include <binder/IBinder.h>
#include <android/binder_status.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <aidl/vendor/qti/hardware/radio/ims/IImsRadio.h>
#include "aidl/vendor/qti/hardware/radio/ims/IImsRadioResponse.h"
#include "aidl/vendor/qti/hardware/radio/ims/IImsRadioIndication.h"

#include "ril_utf_core.h"
#include "ril_utf_service_manager.h"
#include "ril_utf_i_ims_aidl_radio_sim.h"
#include "ril_utf_i_ims_aidl_radio_sim_utils.h"
#include "ril_utf_ims_sim.h"

using namespace vendor::qti::hardware::radio::ims::aidl::utils;

static void send_i_ims_radio_enqueue_ims_expect(int size, void *payload, int msg_id, int token,
                                                enum ril_utf_q_element_t type, ims_Error error) {
  int my_expect_slot;

  // acquire mutex
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: IMS recv thread locking exec_q_access in %s", __func__);
  EXEC_Q_LOCK();
  // Grab next available slot in expect table
  for (int i = 0; i < MAX_EXPECTS; ++i) {
    if (expect_table[i].in_use == 0) {
      my_expect_slot = i;
      expect_table[my_expect_slot].in_use = 1;
      break;
    }
  }

  // verify that we are allowed to proceed
  while (expect_table[my_expect_slot].expect_ready != 1) {
    struct timeval e_timeout;
    struct timespec thread_timeout;
    e_timeout.tv_sec = 0;
    e_timeout.tv_usec = EXPECT_TIMEOUT;
    calc_timeout(&thread_timeout, &e_timeout);
    pthread_cond_timedwait(&expect_ready_sig, &exec_q_access, &thread_timeout);
  }

  enqueue_ims_expect(size, payload, msg_id, token, type, error);

  // signal test thread.
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: Setting expect_received to true @ %s %s %d ", __FILE__, __func__,
                __LINE__);
  expect_table[my_expect_slot].expect_received = 1;
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: Signalling Expectation");
  pthread_cond_signal(&signal_exptectation);

  // release mutex
  EXEC_Q_UNLOCK();
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: %s. released exec_q_mutex.", __func__);
}

class IImsAidlRadioResponseClient : public imsaidl::IImsRadioResponse {
 public:
  ::ndk::ScopedAStatus dialResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: dialResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_DIAL, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus answerResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: answerResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_ANSWER, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus hangupResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: hangupResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_HANGUP, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus requestRegistrationChangeResponse(int32_t token,
      imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: requestRegistrationChangeResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus queryServiceStatusResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const std::vector<
          imsaidl::ServiceStatusInfo>& srvStatusList) {
    RIL_UTF_DEBUG(
        "\n QMI_RIL_UTL: queryServiceStatusResponse");
    ims_SrvStatusList *msg_data = (ims_SrvStatusList *)qcril_malloc(sizeof(ims_SrvStatusList));
    if(msg_data != nullptr) {
        convertFromAidl(srvStatusList, *msg_data);
        send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SrvStatusList), msg_data,
                                            ims_MsgId_REQUEST_QUERY_SERVICE_STATUS, token,
                                            ril_utf_ims_response, convertFromAidl(errorCode));
    }
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setServiceStatusResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setServiceStatusResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SET_SERVICE_STATUS, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus holdResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo& sipError) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: holdResponse");
    ims_SipErrorInfo *msg_data = nullptr;
    if (errorCode != imsaidl::ErrorCode::SUCCESS) {
      std::string logStr = sipError.toString();
      QCRIL_LOG_INFO("sipError = %s", logStr.c_str());
      msg_data = (ims_SipErrorInfo *)qcril_malloc(sizeof(ims_SipErrorInfo));
      convertFromAidl(sipError, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SipErrorInfo), msg_data, ims_MsgId_REQUEST_HOLD,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus resumeResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo& sipError) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: resumeResponse");
    ims_SipErrorInfo *msg_data = nullptr;
    if (errorCode != imsaidl::ErrorCode::SUCCESS) {
      msg_data = (ims_SipErrorInfo *)qcril_malloc(sizeof(ims_SipErrorInfo));
      convertFromAidl(sipError, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SipErrorInfo), msg_data,
                                        ims_MsgId_REQUEST_RESUME, token, ril_utf_ims_response,
                                        convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setConfigResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ConfigInfo& config) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setConfigResponse");
    ims_ConfigMsg *msg_data = (ims_ConfigMsg *)qcril_malloc(sizeof(ims_ConfigMsg));
    convertFromAidl(config, *msg_data);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ConfigMsg), msg_data,
                                        ims_MsgId_REQUEST_SET_IMS_CONFIG, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getConfigResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ConfigInfo& config) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getConfigResponse");
    ims_ConfigMsg *msg_data = (ims_ConfigMsg *)qcril_malloc(sizeof(ims_ConfigMsg));
    convertFromAidl(config, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ConfigMsg), msg_data,
                                        ims_MsgId_REQUEST_GET_IMS_CONFIG, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getImsRegistrationStateResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::RegistrationInfo& registration) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getRegistrationResponse");
    ims_Registration *msg_data = (ims_Registration *)qcril_malloc(sizeof(ims_Registration));
    convertFromAidl(registration, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Registration), msg_data,
      ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, token,
      ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus suppServiceStatusResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SuppServiceStatus& suppServiceStatus) {
    RIL_UTF_DEBUG(
        "\n QMI_RIL_UTL: suppServiceStatusResponse");
    uint32_t origToken = (token & 0x0000FFFF);
    uint32_t msgId = (token & 0xFFFF0000) >> 16;

    ims_SuppSvcResponse *msg_data = nullptr;
    if ((msgId == ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS && errorCode != imsaidl::ErrorCode::SUCCESS) ||
        (msgId != ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS)) {
      msg_data = (ims_SuppSvcResponse *)qcril_malloc(sizeof(ims_SuppSvcResponse));
      convertFromAidl(suppServiceStatus, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SuppSvcResponse), msg_data, msgId, origToken,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus conferenceResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo &errorInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: conferenceResponse");
    ims_SipErrorInfo *msg_data = nullptr;
    if (errorCode != imsaidl::ErrorCode::SUCCESS) {
      msg_data = (ims_SipErrorInfo *)qcril_malloc(sizeof(ims_SipErrorInfo));
      convertFromAidl(errorInfo, *msg_data);
    }

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SipErrorInfo), msg_data,
                                        ims_MsgId_REQUEST_CONFERENCE, token, ril_utf_ims_response,
                                        convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getClipResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ClipProvisionStatus& clipProvisionStatus) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getClipResponse %d, %d", token, errorCode);
    std::string logStr = clipProvisionStatus.toString();
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getClipResponse %d, %d, %s", token, errorCode, logStr.c_str());
    ims_ClipProvisionStatus *msg_data =
        (ims_ClipProvisionStatus *)qcril_malloc(sizeof(ims_ClipProvisionStatus));

    if (errorCode == imsaidl::ErrorCode::SUCCESS) {
      msg_data->has_clip_status = TRUE;
      switch (clipProvisionStatus.clipStatus) {
        case imsaidl::ClipStatus::NOT_PROVISIONED:
          msg_data->clip_status = ims_ClipStatus_NOT_PROVISIONED;
          break;
        case imsaidl::ClipStatus::PROVISIONED:
          msg_data->clip_status = ims_ClipStatus_PROVISIONED;
          break;
        case imsaidl::ClipStatus::STATUS_UNKNOWN:
          msg_data->clip_status = ims_ClipStatus_STATUS_UNKNOWN;
          break;
        case imsaidl::ClipStatus::INVALID:
        default:
          msg_data->has_clip_status = FALSE;
          break;
      }
    }
    if (errorCode != imsaidl::ErrorCode::SUCCESS) {
      msg_data->has_errorDetails = true;
      convertFromAidl(clipProvisionStatus.errorDetails, msg_data->errorDetails);
    }

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ClipProvisionStatus), msg_data,
                                        ims_MsgId_REQUEST_QUERY_CLIP, token, ril_utf_ims_response,
                                        convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getClirResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ClirInfo &clirInfo, bool hasClirInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getClirResponse");
    ims_Clir *msg_data = (ims_Clir *)qcril_malloc(sizeof(ims_Clir));
    if (hasClirInfo) {
      convertFromAidl(clirInfo, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Clir), msg_data, ims_MsgId_REQUEST_GET_CLIR,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setClirResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setClirResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SET_CLIR,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getColrResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ColrInfo& colrInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getColrResponse");
    ims_Colr *msg_data = (ims_Colr *)qcril_malloc(sizeof(ims_Colr));
    if(errorCode == imsaidl::ErrorCode::SUCCESS) {
      convertFromAidl(colrInfo, *msg_data);
    }
    if (errorCode != imsaidl::ErrorCode::SUCCESS &&
        colrInfo.errorDetails.errorCode != 0) {
      msg_data->has_errorDetails = TRUE;
      convertFromAidl(colrInfo.errorDetails, msg_data->errorDetails);
    }

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Colr), msg_data, ims_MsgId_REQUEST_GET_COLR,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus exitEmergencyCallbackModeResponse(int32_t token,
                                                                      imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: exitEmergencyCallbackModeResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendDtmfResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendDtmfResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_DTMF, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus startDtmfResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: startDtmfResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_DTMF_START, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus stopDtmfResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: stopDtmfResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_DTMF_STOP, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setUiTTYModeResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setUiTTYModeResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SEND_UI_TTY_MODE, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus modifyCallInitiateResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: modifyCallInitiateResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_MODIFY_CALL_INITIATE, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus modifyCallConfirmResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: modifyCallConfirmResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus queryCallForwardStatusResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const std::vector<imsaidl::CallForwardInfo> &callForwardInfoList,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: queryCallForwardStatusResponse");
    ims_CallForwardInfoList *msg_data =
        (ims_CallForwardInfoList *)qcril_malloc(sizeof(ims_CallForwardInfoList));

    int len = callForwardInfoList.size();
    if (len > 0) {
      ims_CallForwardInfoList_CallForwardInfo **dptr =
          (ims_CallForwardInfoList_CallForwardInfo **)qcril_malloc(
              sizeof(ims_CallForwardInfoList_CallForwardInfo *) * (len + 1));
      for (int i = 0; i < len; i++) {
        dptr[i] = (ims_CallForwardInfoList_CallForwardInfo *)qcril_malloc(
            sizeof(ims_CallForwardInfoList_CallForwardInfo));
        convertFromAidl(callForwardInfoList[i], *dptr[i]);
      }
      msg_data->info.arg = dptr;
    }
    msg_data->has_errorDetails = true;
    convertFromAidl(errorDetails, msg_data->errorDetails);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallForwardInfoList), msg_data,
                                        ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getCallWaitingResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::CallWaitingInfo& info,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getCallWaitingResponse");
    ims_CallWaitingInfo *msg_data =
        (ims_CallWaitingInfo *)qcril_malloc(sizeof(ims_CallWaitingInfo));

    if (info.serviceStatus != imsaidl::ServiceClassStatus::INVALID) {
      msg_data->has_service_status = TRUE;
      convertFromAidl(info.serviceStatus, msg_data->service_status);
    }
    if (info.serviceClass != INT32_MAX) {
      msg_data->has_service_class = TRUE;
      msg_data->service_class.has_service_Class = TRUE;
      msg_data->service_class.service_Class = info.serviceClass;
    }

    if (errorCode != imsaidl::ErrorCode::SUCCESS) {
      msg_data->has_errorDetails = true;
      convertFromAidl(errorDetails, msg_data->errorDetails);
    }

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallWaitingInfo), msg_data,
                                        ims_MsgId_REQUEST_QUERY_CALL_WAITING, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus explicitCallTransferResponse(int32_t token,
      imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo& in_errorInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: explicitCallTransferResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setSuppServiceNotificationResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      imsaidl::ServiceClassStatus serviceStatus) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setSuppServiceNotificationResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION,
                                       token, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getRtpStatisticsResponse(int32_t token, imsaidl::ErrorCode errorCode,
      int64_t packetCount) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getRtpErrorStatisticsResponse(int32_t token, imsaidl::ErrorCode errorCode,
      int64_t packetErrorCount) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus addParticipantResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: addParticipantResponse");
    ims_SipErrorInfo *msg_data = (ims_SipErrorInfo *)qcril_malloc(sizeof(ims_SipErrorInfo));
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SipErrorInfo), msg_data,
                                        ims_MsgId_REQUEST_ADD_PARTICIPANT, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus deflectCallResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: deflectCallResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_DEFLECT_CALL, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendGeolocationInfoResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendGeolocationInfoResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO, token,
                                          ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getImsSubConfigResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::ImsSubConfigInfo& subConfigInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: getImsSubConfigResponse");
    ims_ImsSubConfig *msg_data = nullptr;
    if (errorCode == imsaidl::ErrorCode::SUCCESS) {
      msg_data = (ims_ImsSubConfig *)qcril_malloc(sizeof(ims_ImsSubConfig));
      convertFromAidl(subConfigInfo, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ImsSubConfig), msg_data,
                                        ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendRttMessageResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendRttMessageResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_SEND_RTT_MSG, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus cancelModifyCallResponse(int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: cancelModifyCallResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_CANCEL_MODIFY_CALL, token,
                                        ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendSmsResponse(
      int32_t token,
      const imsaidl::SmsSendResponse& smsResponse) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendSmsResponse");
    ims_SmsSendResponse *msg_data =
        (ims_SmsSendResponse *)qcril_malloc(sizeof(ims_SmsSendResponse));
    convertFromAidl(smsResponse, *msg_data);
    ims_Error errorCode = convertFromAidl(smsResponse);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SmsSendResponse), msg_data,
        ims_MsgId_REQUEST_SEND_SMS, token, ril_utf_ims_response, errorCode);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus registerMultiIdentityLinesResponse(int32_t token,
                                                                       imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: registerMultiIdentityLinesResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_REQUEST_REGISTER_MULTI_IDENTITY_LINES,
                                        token, ril_utf_ims_response, convertFromAidl(errorCode));

    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus queryVirtualLineInfoResponse(
      int32_t token, const std::string& msisdn,
      const std::vector<std::string>& virtualLineInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: queryVirtualLineInfoResponse");
      ims_VirtualLineInfoResp *msg_data =
        (ims_VirtualLineInfoResp *)qcril_malloc(sizeof(ims_VirtualLineInfoResp));
      if (!msisdn.empty()) {
        strlcpy(msg_data->msisdn, msisdn.c_str(), msisdn.size()+1);
      }
      if (virtualLineInfo.size()) {
        msg_data->lines_len = virtualLineInfo.size();
        for(uint32_t i = 0; i < 10 && i < virtualLineInfo.size(); i++) {
          if (!virtualLineInfo[i].empty()) {
            strlcpy(msg_data->lines[i], virtualLineInfo[i].c_str(), virtualLineInfo[i].size()+1);
          }
        }
      }
      send_i_ims_radio_enqueue_ims_expect(sizeof(ims_VirtualLineInfoResp), msg_data,
                                          ims_MsgId_REQUEST_QUERY_VIRTUAL_LINE_INFO,
                                          token, ril_utf_ims_response, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setCallForwardStatusResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::CallForwardStatusInfo& callForwardStatus) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setCallForwardStatusResponse");
      uint32_t origToken = (token & 0x0000FFFF);
      uint32_t msgId = (token & 0xFFFF0000) >> 16;

      ims_CallForwardStatusInfo *msg_data = nullptr;
      msg_data = (ims_CallForwardStatusInfo *)qcril_malloc(sizeof(ims_CallForwardStatusInfo));
      convertFromAidl(callForwardStatus, *msg_data);
      send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallForwardStatusInfo), msg_data, msgId,
                                          origToken, ril_utf_ims_response, convertFromAidl(errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendUssdResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendUssdResponse");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus cancelPendingUssdResponse(
      int32_t token, imsaidl::ErrorCode errorCode,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: cancelPendingUssdResponse");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendSipDtmfResponse(
      int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendSipDtmfResponse");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus setMediaConfigurationResponse(
      int32_t token, imsaidl::ErrorCode errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: setMediaConfigurationResponse");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus queryMultiSimVoiceCapabilityResponse(
      int32_t token, imsaidl::ErrorCode in_errorCode,
      imsaidl::MultiSimVoiceCapability in_voiceCapability) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: queryMultiSimVoiceCapabilityResponse");
    ims_MultiSimVoiceCapability* msg_data = nullptr;
    if (in_errorCode == imsaidl::ErrorCode::SUCCESS) {
      msg_data = (ims_MultiSimVoiceCapability*)qcril_malloc(sizeof(ims_MultiSimVoiceCapability));
      convertFromAidl(in_voiceCapability, *msg_data);
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_MultiSimVoiceCapability), msg_data,
                                        ims_MsgId_REQUEST_QUERY_MULTI_SIM_VOICE_CAPABILITY, token,
                                        ril_utf_ims_response, convertFromAidl(in_errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus exitSmsCallBackModeResponse(int32_t in_token,
      ::aidl::vendor::qti::hardware::radio::ims::ErrorCode in_errorCode) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendVosSupportStatusResponse(int32_t in_token,
      ::aidl::vendor::qti::hardware::radio::ims::ErrorCode in_errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendVosSupportStatusResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr,
                                        ims_MsgId_REQUEST_SEND_VOS_SUPPORT_STATUS, in_token,
                                        ril_utf_ims_response, convertFromAidl(in_errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus sendVosActionInfoResponse(int32_t in_token,
      ::aidl::vendor::qti::hardware::radio::ims::ErrorCode in_errorCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: sendVosActionInfoResponse");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr,
                                        ims_MsgId_REQUEST_SEND_VOS_ACTION_INFO, in_token,
                                        ril_utf_ims_response, convertFromAidl(in_errorCode));
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::SpAIBinder asBinder() {
    return ::ndk::SpAIBinder();
  }
  bool isRemote() {
    return false;
  }
};

class IImsAidlRadioIndicationClient : public imsaidl::IImsRadioIndication {
 public:
  ::ndk::ScopedAStatus onCallStateChanged(
      const std::vector<imsaidl::CallInfo>&
          callList) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onCallStateChanged");
    ims_CallList* msg_data = (ims_CallList*)qcril_malloc(sizeof(ims_CallList));
    convertFromAidl(callList, *msg_data);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallList), msg_data,
                                        ims_MsgId_UNSOL_RESPONSE_CALL_STATE_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRing() {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRing");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_CALL_RING, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRingbackTone(
      imsaidl::ToneOperation tone) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRingbackTone");
    ims_RingBackTone *msg_data = (ims_RingBackTone *)qcril_malloc(sizeof(ims_RingBackTone));

    msg_data->has_flag = (tone != imsaidl::ToneOperation::INVALID);
    if (msg_data->has_flag) {
      convertFromAidl(tone, msg_data->flag);
    }
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_RINGBACK_TONE, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRegistrationChanged(
      const imsaidl::RegistrationInfo& registration) {
    RIL_UTF_DEBUG(
        "\n QMI_RIL_UTL: onRegistrationChanged");
    ims_Registration *msg_data = (ims_Registration *)qcril_malloc(sizeof(ims_Registration));
    assert(msg_data != nullptr);
    convertFromAidl(registration, *msg_data);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Registration), msg_data,
                                        ims_MsgId_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onHandover(
      const imsaidl::HandoverInfo& handover) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onHandover");
    ims_Handover* msg_data = (ims_Handover*)qcril_malloc(sizeof(ims_Handover));
    convertFromAidl(handover, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Handover), msg_data,
                                        ims_MsgId_UNSOL_RESPONSE_HANDOVER, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onServiceStatusChanged(
      const std::vector<imsaidl::ServiceStatusInfo>& srvStatusList) {
    RIL_UTF_DEBUG(
        "\n QMI_RIL_UTL: onServiceStatusChanged");
    ims_SrvStatusList *msg_data = (ims_SrvStatusList *)qcril_malloc(sizeof(ims_SrvStatusList));

    convertFromAidl(srvStatusList, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SrvStatusList), msg_data,
                                        ims_MsgId_UNSOL_SRV_STATUS_UPDATE, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRadioStateChanged(
      imsaidl::RadioState radioState) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRadioStateChanged");
    ims_RadioStateChanged *msg_data =
        (ims_RadioStateChanged *)qcril_malloc(sizeof(ims_RadioStateChanged));
    msg_data->has_state = true;
    msg_data->state = ims_RadioState_RADIO_STATE_ON;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_RadioStateChanged), msg_data,
                                        ims_MsgId_UNSOL_RADIO_STATE_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onEmergencyCallBackModeChanged(
      imsaidl::EmergencyCallBackMode mode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onEnterEmergencyCallBackMode");
    if (mode == imsaidl::EmergencyCallBackMode::ENTER) {
      send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE,
                                         -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    } else if (mode == imsaidl::EmergencyCallBackMode::EXIT) {
      send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,
                                         -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    }
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onTtyNotification(
      const imsaidl::TtyInfo& ttyInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onTtyNotification");
    ims_TtyNotify *msg_data = (ims_TtyNotify *)qcril_malloc(sizeof(ims_TtyNotify));
    int len = ttyInfo.userData.size();
    RIL_UTF_DEBUG("\n ttyInfo.userData.size() = %d", len);
    if (len) {
      qcril_binary_data_type *userDataPtr =
        (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
      userDataPtr->len = len;
      userDataPtr->data = (uint8_t *)ttyInfo.userData.data();
      msg_data->userData.arg = userDataPtr;
    }
    msg_data->has_mode = FALSE;
    if (ttyInfo.mode  != imsaidl::TtyMode::INVALID) {
      convertFromAidl(ttyInfo.mode, msg_data->mode);
      msg_data->has_mode = TRUE;
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_TtyNotify), msg_data,
                                        ims_MsgId_UNSOL_TTY_NOTIFICATION, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRefreshConferenceInfo(
      const imsaidl::ConferenceInfo& conferenceInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRefreshConferenceInfo");
    ims_ConfInfo *msg_data = (ims_ConfInfo *)qcril_malloc(sizeof(ims_ConfInfo));
    int len = conferenceInfo.confInfoUri.size();
    RIL_UTF_DEBUG("\n conferenceInfo.confInfoUri.size() = %d", len);
    if (len) {
      qcril_binary_data_type *confInfoUriPtr =
        (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
      confInfoUriPtr->len = len;
      confInfoUriPtr->data = (uint8_t *)qcril_malloc(len);
      memcpy(confInfoUriPtr->data, conferenceInfo.confInfoUri.data(), len);
      msg_data->conf_info_uri.arg = confInfoUriPtr;
    }
    msg_data->has_confCallState = TRUE;
    if (conferenceInfo.conferenceCallState  == imsaidl::ConferenceCallState::RINGING) {
      msg_data->confCallState = ims_ConfCallState_RINGING;
    } else if (conferenceInfo.conferenceCallState  == imsaidl::ConferenceCallState::FOREGROUND) {
      msg_data->confCallState = ims_ConfCallState_FOREGROUND;
    } else if (conferenceInfo.conferenceCallState  == imsaidl::ConferenceCallState::BACKGROUND) {
      msg_data->confCallState = ims_ConfCallState_BACKGROUND;
    } else {
      msg_data->has_confCallState = FALSE;
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ConfInfo), msg_data,
                                        ims_MsgId_UNSOL_REFRESH_CONF_INFO, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRefreshViceInfo(
      const const imsaidl::ViceInfo& viceInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRefreshViceInfo");
    ims_ViceInfo *msg_data = (ims_ViceInfo *)qcril_malloc(sizeof(ims_ViceInfo));
    int len = viceInfo.viceInfoUri.size();
    RIL_UTF_DEBUG("\n viceInfo.viceInfoUri.size() = %d", len);
    if (len) {
      qcril_binary_data_type *viceInfoUriPtr =
        (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
      viceInfoUriPtr->len = len;
      viceInfoUriPtr->data = (uint8_t *)qcril_malloc(len);
      memcpy(viceInfoUriPtr->data, viceInfo.viceInfoUri.data(), len);
      msg_data->vice_info_uri.arg = viceInfoUriPtr;
    }
    // Decode message and enqueue
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ConfInfo), msg_data,
                                        ims_MsgId_UNSOL_REFRESH_VICE_INFO, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onModifyCall(
      const const imsaidl::CallModifyInfo& callModifyInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onModifyCall");
    ims_CallModify *msg_data = (ims_CallModify *)qcril_malloc(sizeof(ims_CallModify));
    convertFromAidl(callModifyInfo, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallModify), msg_data,
                                        ims_MsgId_UNSOL_MODIFY_CALL, -1, ril_utf_ims_indication,
                                        ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSuppServiceNotification(
      const const imsaidl::SuppServiceNotification&
          suppServiceNotification) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onSuppServiceNotification");
    ims_SuppSvcNotification *msg_data =
        (ims_SuppSvcNotification *)qcril_malloc(sizeof(ims_SuppSvcNotification));
    convertFromAidl(suppServiceNotification, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SuppSvcNotification), msg_data,
                                        ims_MsgId_UNSOL_SUPP_SVC_NOTIFICATION, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onMessageWaiting(
      const imsaidl::MessageWaitingIndication&
          messageWaitingIndication) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onMessageWaiting");
    ims_Mwi *msg_data = (ims_Mwi *)qcril_malloc(sizeof(ims_Mwi));
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Mwi), msg_data,
                                        ims_MsgId_UNSOL_MWI, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onGeolocationInfoRequested(double lat, double lon) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onGeolocationInfoRequested");
    ims_GeoLocationInfo *msg_data =
        (ims_GeoLocationInfo *)qcril_malloc(sizeof(ims_GeoLocationInfo));
    msg_data->has_lat = true;
    msg_data->lat = lat;
    msg_data->has_lon = true;
    msg_data->lon = lon;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_GeoLocationInfo), msg_data,
                                        ims_MsgId_UNSOL_REQUEST_GEOLOCATION, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onImsSubConfigChanged(
      const const imsaidl::ImsSubConfigInfo& config) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onImsSubConfigChanged");
    ims_ImsSubConfig *msg_data = (ims_ImsSubConfig *)qcril_malloc(sizeof(ims_ImsSubConfig));
    convertFromAidl(config, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ImsSubConfig), msg_data,
                                        ims_MsgId_UNSOL_IMS_SUB_CONFIG_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onParticipantStatusInfo(
      const const imsaidl::ParticipantStatusInfo&
          participantStatusInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onParticipantStatusInfo");
    ims_ParticipantStatusInfo *msg_data =
        (ims_ParticipantStatusInfo *)qcril_malloc(sizeof(ims_ParticipantStatusInfo));
    convertFromAidl(participantStatusInfo, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_ParticipantStatusInfo), msg_data,
                                        ims_MsgId_UNSOL_PARTICIPANT_STATUS_INFO, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRegistrationBlockStatus(
      const imsaidl::RegistrationBlockStatusInfo& blockStatusInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRegistrationBlockStatus");
    ims_RegistrationBlockStatus *msg_data =
        (ims_RegistrationBlockStatus *)qcril_malloc(sizeof(ims_RegistrationBlockStatus));
    convertFromAidl(blockStatusInfo, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_RegistrationBlockStatus), msg_data,
                                        ims_MsgId_UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRttMessageReceived(
      const std::string&  message) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRttMessageReceived");
    ims_RttMessage *msg_data =
        (ims_RttMessage *)qcril_malloc(sizeof(ims_RttMessage));
    size_t destSize = sizeof(char) * (message.size() + 1);
    msg_data->rttMessage.arg = qcril_malloc(destSize);
    if( msg_data->rttMessage.arg != nullptr) {
      strlcpy((char *)msg_data->rttMessage.arg, message.c_str(), destSize);
    }
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRttMessageReceived %s ",message.c_str());
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_RttMessage), msg_data,
                                        ims_MsgId_UNSOL_RESPONSE_RTT_MSG_RECEIVED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onVoWiFiCallQuality(
      imsaidl::VoWiFiCallQuality voWiFiCallQualityInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onVoWiFiCallQuality");
    ims_VoWiFiCallQuality *msg_data =
        (ims_VoWiFiCallQuality *)qcril_malloc(sizeof(ims_VoWiFiCallQuality));

    msg_data->has_quality = TRUE;
    switch (voWiFiCallQualityInfo) {
      case imsaidl::VoWiFiCallQuality::EXCELLENT:
        msg_data->quality = ims_voWiFiQuality_VOWIFI_QUALITY_EXCELLENT;
        break;
      case imsaidl::VoWiFiCallQuality::FAIR:
        msg_data->quality = ims_voWiFiQuality_VOWIFI_QUALITY_FAIR;
        break;
      case imsaidl::VoWiFiCallQuality::BAD:
        msg_data->quality = ims_voWiFiQuality_VOWIFI_QUALITY_BAD;
        break;
      case imsaidl::VoWiFiCallQuality::NONE:
        msg_data->quality = ims_voWiFiQuality_VOWIFI_QUALITY_NONE;
        break;
      case imsaidl::VoWiFiCallQuality::INVALID:
      default:
        msg_data->has_quality = FALSE;
        break;
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallModify), msg_data,
                                        ims_MsgId_UNSOL_VOWIFI_CALL_QUALITY, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSupplementaryServiceIndication(
      const imsaidl::StkCcUnsolSsResult& ss) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onSupplementaryServiceIndication");
    //StkCcUnsolSsResult *msg_data = (StkCcUnsolSsResult *)qcril_malloc(sizeof(StkCcUnsolSsResult));
    send_i_ims_radio_enqueue_ims_expect(0, nullptr,
                                        ims_MsgId_UNSOL_ON_SS, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);

    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSmsSendStatusReport(
      const imsaidl::SmsSendStatusReport& smsStatusReport) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onSmsSendStatusReport");
    utf_ims_sms_status_report_t* statusRep =
        (utf_ims_sms_status_report_t*)qcril_malloc(sizeof(utf_ims_sms_status_report_t));
    assert(statusRep != nullptr);
    convertFromAidl(smsStatusReport, *statusRep);

    send_i_ims_radio_enqueue_ims_expect(sizeof(utf_ims_sms_status_report_t), statusRep,
                                        ims_MsgId_UNSOL_SMS_STATUS_REPORT, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onIncomingSms(
      const imsaidl::IncomingSms& imsSms) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onIncomingSms");
    utf_ims_mt_sms_t* smsMsg = (utf_ims_mt_sms_t*)qcril_malloc(sizeof(utf_ims_mt_sms_t));
    assert(smsMsg != nullptr);
    convertFromAidl(imsSms, *smsMsg);

    send_i_ims_radio_enqueue_ims_expect(sizeof(utf_ims_mt_sms_t), smsMsg,
                                        ims_MsgId_UNSOL_INCOMING_IMS_SMS, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onVopsChanged(bool isVopsEnabled) { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus onIncomingCallAutoRejected(
      const imsaidl::AutoCallRejectionInfo&
          autoCallRejectionInfo) {
    ims_AutoCallRejectionInfo *msg_data =
              (ims_AutoCallRejectionInfo *)qcril_malloc(sizeof(ims_AutoCallRejectionInfo));
    convertFromAidl(autoCallRejectionInfo, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_AutoCallRejectionInfo), msg_data,
                                        ims_MsgId_UNSOL_AUTO_CALL_REJECTION_IND, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onVoiceInfoChanged(
      imsaidl::VoiceInfo voiceInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onVoiceInfoChanged");
    ims_VoiceInfoData *msg_data =
          (ims_VoiceInfoData*)qcril_malloc(sizeof(ims_VoiceInfoData));
    switch(voiceInfo) {
        case imsaidl::VoiceInfo::SILENT:
             msg_data->voiceInfo = ims_Voice_Info_SILENT;
            break;
        case imsaidl::VoiceInfo::SPEECH:
             msg_data->voiceInfo = ims_Voice_Info_SPEECH;
            break;
        default:
             msg_data->voiceInfo = ims_Voice_Info_UNKNOWN;
            break;
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_VoiceInfoData), msg_data,
                                        ims_MsgId_UNSOL_VOICE_INFO, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onMultiIdentityRegistrationStatusChange(
      const std::vector<imsaidl::MultiIdentityLineInfo>& info) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onMultiIdentityRegistrationStatusChange");
    ims_MultiIdentityStatus *msg_data =
          (ims_MultiIdentityStatus*)qcril_malloc(sizeof(ims_MultiIdentityStatus));
    convertFromAidl(info, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_MultiIdentityStatus), msg_data,
                                        ims_MsgId_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onMultiIdentityInfoPending() {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onMultiIdentityInfoPending");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_MULTI_IDENTITY_PENDING_INFO,
                                        -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onModemSupportsWfcRoamingModeConfiguration(
      bool wfcRoamingConfigurationSupport) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onModemSupportsWfcRoamingModeConfiguration");
    ims_WfcRoamingConfigSupportInfo *msg_data =
          (ims_WfcRoamingConfigSupportInfo *)qcril_malloc(sizeof(ims_WfcRoamingConfigSupportInfo));
    msg_data->wfcRoamingConfigSupport = true;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_WfcRoamingConfigSupportInfo), msg_data,
                                        ims_MsgId_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onUssdMessageFailed(
      imsaidl::UssdModeType type,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onUssdMessageFailed");
    ims_UssdMessageFailedInfo *msg_data =
        (ims_UssdMessageFailedInfo *)qcril_malloc(sizeof(ims_UssdMessageFailedInfo));
    assert(msg_data != nullptr);

    switch(type) {
      case imsaidl::UssdModeType::NOTIFY:
        msg_data->type = ims_UssdModeType_NOTIFY;
        break;
      case imsaidl::UssdModeType::REQUEST:
        msg_data->type = ims_UssdModeType_REQUEST;
        break;
      case imsaidl::UssdModeType::NW_RELEASE:
        msg_data->type = ims_UssdModeType_NW_RELEASE;
        break;
      case imsaidl::UssdModeType::LOCAL_CLIENT:
        msg_data->type = ims_UssdModeType_LOCAL_CLIENT;
        break;
      case imsaidl::UssdModeType::NOT_SUPPORTED:
        msg_data->type = ims_UssdModeType_NOT_SUPPORTED;
        break;
      case imsaidl::UssdModeType::NW_TIMEOUT:
        msg_data->type = ims_UssdModeType_NW_TIMEOUT;
        break;
    }
    convertFromAidl(errorDetails, msg_data->errorDetails);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_UssdMessageFailedInfo), msg_data,
                                        ims_MsgId_UNSOL_USSD_FAILED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onUssdReceived(
      imsaidl::UssdModeType type,
      const std::string& msg,
      const imsaidl::SipErrorInfo& errorDetails) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onUssdReceived");
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onIncomingCallComposerCallAutoRejected(
      const imsaidl::CallComposerAutoRejectionInfo& info) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: onIncomingCallComposerCallAutoRejected");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onCallComposerInfoAvailable(
      const imsaidl::CallComposerInfo& callComposerInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: onCallComposerInfoAvailable");
    auto data = (ims_CallComposer*)qcril_malloc(sizeof(ims_CallComposer));
    assert(data != nullptr);
    convertFromAidl(callComposerInfo, *data);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CallComposer), data,
                                        ims_MsgId_UNSOL_CALL_COMPOSER_INFO, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onRetrievingGeoLocationDataStatus(
      imsaidl::GeoLocationDataStatus geoLocationDataStatus) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onRetrievingGeoLocationDataStatus");
    ims_GeoLocationDataStatusInfo* msg_data =
        (ims_GeoLocationDataStatusInfo*)qcril_malloc(sizeof(ims_GeoLocationDataStatusInfo));
    switch (geoLocationDataStatus) {
      case imsaidl::GeoLocationDataStatus::TIMEOUT:
        msg_data->geoLocationDataStatus = ims_GeoLocationDataStatus_TIMEOUT;
        break;
      case imsaidl::GeoLocationDataStatus::NO_CIVIC_ADDRESS:
        msg_data->geoLocationDataStatus = ims_GeoLocationDataStatus_NO_CIVIC_ADDRESS;
        break;
      case imsaidl::GeoLocationDataStatus::ENGINE_LOCK:
        msg_data->geoLocationDataStatus = ims_GeoLocationDataStatus_ENGINE_LOCK;
        break;
      case imsaidl::GeoLocationDataStatus::RESOLVED:
        msg_data->geoLocationDataStatus = ims_GeoLocationDataStatus_RESOLVED;
        break;
      default:
        msg_data->geoLocationDataStatus = ims_GeoLocationDataStatus_UNKNOWN;
        break;
    }
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_GeoLocationDataStatusInfo), msg_data,
                                        ims_MsgId_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSipDtmfReceived(
      const std::string& configCode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onSipDtmfReceived");
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onServiceDomainChanged(
      imsaidl::SystemServiceDomain domain) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSmsCallBackModeChanged(
      imsaidl::SmsCallBackMode mode) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onSmsCallBackModeChanged");
    ims_SmsCallbackModeStatusInfo* msg_data =
        (ims_SmsCallbackModeStatusInfo*)qcril_malloc(sizeof(ims_SmsCallbackModeStatusInfo));
    switch (mode) {
      case imsaidl::SmsCallBackMode::ENTER:
        msg_data->smsCallbackModeStatus = ims_SmsCallbackModeStatus_ENTER;
        break;
      case imsaidl::SmsCallBackMode::EXIT:
        msg_data->smsCallbackModeStatus = ims_SmsCallbackModeStatus_EXIT;
        break;
      default:
        msg_data->smsCallbackModeStatus = ims_SmsCallbackModeStatus_UNKNOWN;
        break;
    }
    // Decode message and enqueue
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SmsCallbackModeStatusInfo), msg_data,
                                        ims_MsgId_UNSOL_SMS_CALLBACK_MODE_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onConferenceCallStateCompleted() {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onConferenceCallStateCompleted");
    send_i_ims_radio_enqueue_ims_expect(0, nullptr, ims_MsgId_UNSOL_CONFERENCE_CALL_STATE_COMPLETED,
                                        -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onIncomingDtmfStart(const imsaidl::DtmfInfo& in_dtmfInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onIncomingDtmfStart");
    auto data = (ims_Dtmf*)qcril_malloc(sizeof(ims_Dtmf));
    assert(data != nullptr);
    size_t destSize = sizeof(char) * (in_dtmfInfo.dtmf.size() + 1);
    data->dtmf.arg = qcril_malloc(destSize);
    if (data->dtmf.arg != nullptr) {
      strlcpy((char*)data->dtmf.arg, in_dtmfInfo.dtmf.c_str(), destSize);
    }
    data->callId = in_dtmfInfo.callId;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Dtmf), data, ims_MsgId_UNSOL_INCOMING_DTMF_START,
                                        -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onIncomingDtmfStop(const imsaidl::DtmfInfo& in_dtmfInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onIncomingDtmfStop");
    auto data = (ims_Dtmf*)qcril_malloc(sizeof(ims_Dtmf));
    assert(data != nullptr);
    size_t destSize = sizeof(char) * (in_dtmfInfo.dtmf.size() + 1);
    data->dtmf.arg = qcril_malloc(destSize);
    if (data->dtmf.arg != nullptr) {
      strlcpy((char*)data->dtmf.arg, in_dtmfInfo.dtmf.c_str(), destSize);
    }
    data->callId = in_dtmfInfo.callId;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_Dtmf), data, ims_MsgId_UNSOL_INCOMING_DTMF_STOP,
                                        -1, ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onMultiSimVoiceCapabilityChanged(
      imsaidl::MultiSimVoiceCapability in_voiceCapability) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onMultiSimVoiceCapabilityChanged");
    ims_MultiSimVoiceCapability* msg_data =
        (ims_MultiSimVoiceCapability*)qcril_malloc(sizeof(ims_MultiSimVoiceCapability));
    assert(msg_data != nullptr);
    convertFromAidl(in_voiceCapability, *msg_data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_MultiSimVoiceCapability), msg_data,
                                        ims_MsgId_UNSOL_MULTI_SIM_VOICE_CAPABILITY_CHANGED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onIncomingCallAutoRejected2(const imsaidl::AutoCallRejectionInfo2& autoReject) {
    auto data = (ims_AutoCallRejectionInfo2*)qcril_malloc(sizeof(ims_AutoCallRejectionInfo2));
    assert(data != nullptr);
    if (autoReject.callComposerInfo) {
      convertFromAidl(*autoReject.callComposerInfo, data->composer);
    }
    convertFromAidl(autoReject.autoCallRejectionInfo, data->rej);
    if (autoReject.ecnamInfo) {
      convertFromAidl(*autoReject.ecnamInfo, data->ecnamInfo);
    }
    data->isDcCall = autoReject.isDcCall;

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_AutoCallRejectionInfo2), data,
                                        ims_MsgId_UNSOL_INCOMING_CALL_AUTO_REJECTED, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onPreAlertingCallInfoAvailable(
      const imsaidl::PreAlertingCallInfo& preAlertingInfo) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: onPreAlertingCallInfoAvailable");
    auto data = (ims_PreAlertingCallInfo*)qcril_malloc(sizeof(ims_PreAlertingCallInfo));
    assert(data != nullptr);
    convertFromAidl(preAlertingInfo, *data);

    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_PreAlertingCallInfo), data,
                                        ims_MsgId_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onCiWlanNotification(
      const imsaidl::CiWlanNotificationInfo info) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: onCiWlanNotification");
    ims_CiWlanNotificationInfo *data = (ims_CiWlanNotificationInfo*)qcril_malloc(sizeof(ims_CiWlanNotificationInfo));
    assert(data != nullptr);
    convertFromAidl(info, *data);
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_CiWlanNotificationInfo), data,
                                        ims_MsgId_UNSOL_C_IWLAN_EXIT_NOTIFICATION, -1,
                                        ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus onSrtpEncryptionStatusChanged(
      const imsaidl::SrtpEncryptionInfo& info) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: onSrtpEncryptionStatusChanged");
    ims_SrtpEncryptionStatus* data = (ims_SrtpEncryptionStatus*)qcril_malloc(sizeof(ims_SrtpEncryptionStatus));
    assert(data != nullptr);
    data->encryptionInfo.call_id = info.callId;
    data->encryptionInfo.categories = info.categories;
    send_i_ims_radio_enqueue_ims_expect(sizeof(ims_SrtpEncryptionStatus), data,
                                                ims_MsgId_UNSOL_SRTP_ENCRYPTION_STATUS, -1,
                                                ril_utf_ims_indication, ims_Error_E_SUCCESS);
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) {
    return ndk::ScopedAStatus::ok();
  }
  ::ndk::SpAIBinder asBinder() {
    return ::ndk::SpAIBinder();
  }
  bool isRemote() {
    return false;
  }
};

static std::shared_ptr<IImsAidlRadioResponseClient> mRespClient;
static std::shared_ptr<IImsAidlRadioIndicationClient> mIndClient;
static std::shared_ptr<aidl::vendor::qti::hardware::radio::ims::IImsRadio> utfImsAidlRadio = nullptr;

void reset_i_ims_aidl_radio() {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: reset_i_ims_radio");
    if (utfImsAidlRadio != nullptr) {
      RIL_UTF_DEBUG("\n QMI_RIL_UTL: calling clearCallback");
      utfImsAidlRadio->setCallback(nullptr, nullptr);
      utfImsAidlRadio = nullptr;
    }
}

void connect_to_i_ims_aidl_radio() {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: connect_to_i_ims_aidl_radio");
  while (utfImsAidlRadio == nullptr) {
    auto imsBinder = ::ndk::SpAIBinder(AServiceManager_getService(
        "vendor.qti.hardware.radio.ims.IImsRadio/imsradio0"));
    utfImsAidlRadio = aidl::vendor::qti::hardware::radio::ims::IImsRadio::fromBinder(imsBinder);
    if (utfImsAidlRadio != nullptr) {
      RIL_UTF_DEBUG("\n QMI_RIL_UTL: calling setCallback");
      mRespClient = ndk::SharedRefBase::make<IImsAidlRadioResponseClient>();
      mIndClient = ndk::SharedRefBase::make<IImsAidlRadioIndicationClient>();
      utfImsAidlRadio->setCallback(mRespClient, mIndClient);
      break;
    }
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: get(imsradio0) returned null");
    usleep(1);
  }
}

void send_dial_req(uint32_t token, void *msg) {
    ims_Dial* i_req = (ims_Dial*)msg;
    if (i_req->isEmergencyDial) {
      imsaidl::EmergencyDialRequest req {};
      convertToAidl(*i_req, req);
      utfImsAidlRadio->emergencyDial(token, req);
    } else {
      imsaidl::DialRequest req {};
      convertToAidl(*i_req, req);
      utfImsAidlRadio->dial(token, req);
    }
}

void send_answer(uint32_t token, void *msg) {
  imsaidl::AnswerRequest req {};
  ims_Answer *i_req = (ims_Answer *)msg;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->answer(token, req);
}

void send_hangup(uint32_t token, void *msg) {
  imsaidl::HangupRequestInfo req = {};
  ims_Hangup *i_req = (ims_Hangup *)msg;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->hangup(token, req);
}

void send_set_service_status_req(uint32_t token, void *msg) {
  std::vector<imsaidl::ServiceStatusInfo> req;
  ims_SetServiceStatusType *i_req = (ims_SetServiceStatusType *)msg;
  if (i_req->has_volteEnabled) {
    imsaidl::ServiceStatusInfo ssInfo = {};
    ssInfo.callType = imsaidl::CallType::VOICE;
    ssInfo.accTechStatus.resize(1);
    ssInfo.accTechStatus[0].networkMode = imsaidl::RadioTechType::LTE;
    ssInfo.accTechStatus[0].status = i_req->volteEnabled ? imsaidl::StatusType::ENABLED
      : imsaidl::StatusType::DISABLED;
    req.push_back(ssInfo);
  }
  if (i_req->has_videoTelephonyEnabled) {
    imsaidl::ServiceStatusInfo ssInfo = {};
    ssInfo.callType = imsaidl::CallType::VT;
    ssInfo.accTechStatus.resize(1);
    ssInfo.accTechStatus[0].networkMode = imsaidl::RadioTechType::LTE;
    ssInfo.accTechStatus[0].status = i_req->videoTelephonyEnabled
      ? imsaidl::StatusType::ENABLED
      : imsaidl::StatusType::DISABLED;
    req.push_back(ssInfo);
  }
  if (i_req->has_wifiCallingEnabled) {
    imsaidl::ServiceStatusInfo ssInfo = {};
    ssInfo.callType = imsaidl::CallType::VOICE;
    ssInfo.accTechStatus.resize(1);
    ssInfo.accTechStatus[0].networkMode = imsaidl::RadioTechType::WIFI;
    ssInfo.accTechStatus[0].status = i_req->wifiCallingEnabled
      ? imsaidl::StatusType::ENABLED
      : imsaidl::StatusType::DISABLED;
    req.push_back(ssInfo);
  }
  if (i_req->has_utEnabled) {
    imsaidl::ServiceStatusInfo ssInfo = {};
    ssInfo.callType = imsaidl::CallType::UT;
    ssInfo.accTechStatus.resize(1);
    ssInfo.accTechStatus[0].networkMode = imsaidl::RadioTechType::LTE;
    ssInfo.accTechStatus[0].status = i_req->utEnabled ? imsaidl::StatusType::ENABLED
      : imsaidl::StatusType::DISABLED;
    req.push_back(ssInfo);
  }
  utfImsAidlRadio->setServiceStatus(token, req);
}

void send_query_call_forward_status(uint32_t token, void *msg) {
  imsaidl::CallForwardInfo req = {};
  ims_CallForwardInfoList *i_req = (ims_CallForwardInfoList *)msg;
  ims_CallForwardInfoList_CallForwardInfo **ptr =
    (ims_CallForwardInfoList_CallForwardInfo **)i_req->info.arg;
  if (ptr && ptr[0]) {
    convertToAidl(*ptr[0], req);
  }
  utfImsAidlRadio->queryCallForwardStatus(token, req);
}

void send_set_call_forward_status(uint32_t token, void *msg) {
  uint32_t extToken =
    (ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS << 16 | (0x0000FFFF & token));

  imsaidl::CallForwardInfo req = {};
  ims_CallForwardInfoList *i_req = (ims_CallForwardInfoList *)msg;
  ims_CallForwardInfoList_CallForwardInfo **ptr =
    (ims_CallForwardInfoList_CallForwardInfo **)i_req->info.arg;
  if (ptr && ptr[0]) {
    convertToAidl(*ptr[0], req);
  }
  utfImsAidlRadio->setCallForwardStatus(extToken, req);
}

void send_get_call_waiting(uint32_t token, void *msg) {
  uint32_t serviceClass = 0;
  ims_ServiceClass *i_req = (ims_ServiceClass *)msg;
  if (i_req->has_service_Class) {
    serviceClass = i_req->service_Class;
  }
  utfImsAidlRadio->getCallWaiting(token, serviceClass);
}

void send_set_supp_service_notification(uint32_t token, void *msg) {
  imsaidl::ServiceClassStatus status = imsaidl::ServiceClassStatus::INVALID;
  ims_SuppSvcStatus *i_req = (ims_SuppSvcStatus *)msg;
  if (i_req->has_status) {
    if (i_req->status == ims_ServiceClassStatus_ENABLED) {
      status = imsaidl::ServiceClassStatus::ENABLED;
    } else {
      status = imsaidl::ServiceClassStatus::DISABLED;
    }
  }
  utfImsAidlRadio->setSuppServiceNotification(token, status);
}

void send_explicit_call_transfer(uint32_t token, void *msg) {
  imsaidl::ExplicitCallTransferInfo ectInfo {};
  ims_ExplicitCallTransfer *i_req = (ims_ExplicitCallTransfer *)msg;
  convertToAidl(*i_req, ectInfo);
  utfImsAidlRadio->explicitCallTransfer(token, ectInfo);
}

void send_deflect_call(uint32_t token, void *msg) {
  imsaidl::DeflectRequestInfo deflectInfo = { .connIndex=INT32_MAX, .number=""};
  ims_DeflectCall *i_req = (ims_DeflectCall *)msg;
  convertToAidl(*i_req, deflectInfo);
  utfImsAidlRadio->deflectCall(token, deflectInfo);
}

void send_dtmf(uint32_t token, void *msg) {
  imsaidl::DtmfInfo dtmfInfo = {};
  ims_Dtmf *i_req = (ims_Dtmf *)msg;
  if (i_req->dtmf.arg) {
    dtmfInfo.dtmf = (char *)(i_req->dtmf.arg);
  }
  utfImsAidlRadio->sendDtmf(token, dtmfInfo);
}

void send_start_dtmf(uint32_t token, void *msg) {
  imsaidl::DtmfInfo dtmfInfo = {};
  ims_Dtmf *i_req = (ims_Dtmf *)msg;
  if (i_req->dtmf.arg) {
    dtmfInfo.dtmf = (char *)(i_req->dtmf.arg);
  }
  utfImsAidlRadio->startDtmf(token, dtmfInfo);
}

void send_request_registration_change(uint32_t token, void *msg) {
  imsaidl::RegState req = imsaidl::RegState::INVALID;
  ims_Registration *i_req = (ims_Registration *)msg;
  if (i_req->has_state) {
    convertToAidl(i_req->state, req);
  }
  utfImsAidlRadio->requestRegistrationChange(token, req);
}

void send_hold(uint32_t token, void *msg) {
  ims_Hold *i_req = (ims_Hold *)msg;
  uint32_t callId = INT32_MAX;
  if (i_req->has_callId) {
    callId = i_req->callId;
  }
  utfImsAidlRadio->hold(token, callId);
}

void send_resume(uint32_t token, void *msg) {
  ims_Resume *i_req = (ims_Resume *)msg;
  uint32_t callId = INT32_MAX;
  if (i_req->has_callId) {
    callId = i_req->callId;
  }
  utfImsAidlRadio->resume(token, callId);
}

void send_geo_location_info(uint32_t token, void *msg) {
  ims_GeoLocationInfo *i_req = (ims_GeoLocationInfo *)msg;
  imsaidl::GeoLocationInfo req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->sendGeolocationInfo(token, req);
}

void send_set_clir(uint32_t token, void *msg) {
  imsaidl::ClirInfo req = {};
  ims_Clir *i_req = (ims_Clir *)msg;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->setClir(token, req);
}

void send_set_call_waiting(uint32_t token, void *msg) {
  uint32_t extToken = (ims_MsgId_REQUEST_SET_CALL_WAITING << 16 | (0x0000FFFF & token));
  imsaidl::CallWaitingInfo req {.serviceStatus = imsaidl::ServiceClassStatus::INVALID,
    .serviceClass = INT32_MAX};
  ims_CallWaitingInfo *i_req = (ims_CallWaitingInfo *)msg;
  if (i_req->has_service_class && i_req->service_class.has_service_Class) {
    req.serviceClass = i_req->service_class.service_Class;
  }
  if (i_req->has_service_status) {
    if (i_req->service_status == ims_ServiceClassStatus_ENABLED) {
      req.serviceStatus = imsaidl::ServiceClassStatus::ENABLED;
    } else {
      req.serviceStatus = imsaidl::ServiceClassStatus::DISABLED;
    }
  }
  utfImsAidlRadio->setCallWaiting(extToken, req);
}

void send_set_colr(uint32_t token, void *msg) {
  uint32_t extToken = (ims_MsgId_REQUEST_SET_COLR << 16 | (0x0000FFFF & token));
  imsaidl::ColrInfo req {};
  ims_Colr *i_req = (ims_Colr *)msg;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->setColr(extToken, req);
}

void send_supp_service_status(uint32_t token, void *msg) {
  uint32_t extToken = (ims_MsgId_REQUEST_SUPP_SVC_STATUS << 16 | (0x0000FFFF & token));
  ims_SuppSvcRequest *i_req = (ims_SuppSvcRequest *)msg;
  imsaidl::SuppServiceStatusRequest req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->suppServiceStatus(extToken, req);
}

void send_set_uitty_mode(uint32_t token, void *msg) {
  ims_TtyNotify *i_req = (ims_TtyNotify *)msg;
  imsaidl::TtyInfo req = {.mode = imsaidl::TtyMode::INVALID};
  if (i_req->has_mode) {
    convertToAidl(i_req->mode, req.mode);
  }
  utfImsAidlRadio->setUiTtyMode(token, req);
}

void send_set_config(uint32_t token, void *msg) {
  ims_ConfigMsg *i_req = (ims_ConfigMsg *)msg;
  imsaidl::ConfigInfo req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->setConfig(token, req);
}

void send_get_config(uint32_t token, void *msg) {
  ims_ConfigMsg *i_req = (ims_ConfigMsg *)msg;
  imsaidl::ConfigInfo req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->getConfig(token, req);
}

void send_modify_call_initiate(uint32_t token, void *msg) {
  ims_CallModify *i_req = (ims_CallModify *)msg;
  imsaidl::CallModifyInfo req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->modifyCallInitiate(token, req);
}

void send_modify_call_confirm(uint32_t token, void *msg) {
  ims_CallModify *i_req = (ims_CallModify *)msg;
  imsaidl::CallModifyInfo req {};
  convertToAidl(*i_req, req);
  utfImsAidlRadio->modifyCallConfirm(token, req);
}

void send_cancel_modify_call(uint32_t token, void *msg) {
  ims_CancelModifyCall *i_req = (ims_CancelModifyCall *)msg;
  uint32_t callId = i_req->callId;
  utfImsAidlRadio->cancelModifyCall(token, callId);
}

void send_sms(uint32_t token, void *msg) {
  auto i_req = static_cast<RIL_IMS_SMS_Message*>(msg);
  imsaidl::SmsSendRequest imsSms {};
  convertToAidl(*i_req, imsSms);
  utfImsAidlRadio->sendSms(token, imsSms);
}

void send_acknowledge_sms(uint32_t token, void *msg) {
  auto i_req = static_cast<utf_ims_ack_sms_t*>(msg);
  imsaidl::AcknowledgeSmsInfo req {};
  req.messageRef = i_req->message_id;
  convertToAidl(i_req->delivery_result, req.smsDeliverStatus);
  utfImsAidlRadio->acknowledgeSms(token, req);
}

void send_register_multi_identity_lines(uint32_t token, void *msg) {
  ims_MultiIdentityStatus *i_req = (ims_MultiIdentityStatus *)msg;
  std::vector<imsaidl::MultiIdentityLineInfo> req;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->registerMultiIdentityLines(token, req);
}

void send_query_virtual_line_info(uint32_t token, void *msg) {
  ims_VirtualLineInfoReq *i_req = (ims_VirtualLineInfoReq *)msg;
  std::string msisdn = i_req->msisdn;
  utfImsAidlRadio->queryVirtualLineInfo(token, msisdn);
}

void send_rtt_message(uint32_t token, void *msg) {
  ims_RttMessage *i_req = (ims_RttMessage *)msg;
  std:string rttMessage = (const char*)(i_req->rttMessage.arg);
  utfImsAidlRadio->sendRttMessage(token, rttMessage);
}

void send_add_participant(uint32_t token, void *msg) {
  imsaidl::DialRequest req {};
  ims_Dial *i_req = (ims_Dial *)msg;
  convertToAidl(*i_req, req);
  utfImsAidlRadio->addParticipant(token, req);
}

void send_set_media_config(uint32_t token, void *msg) {
    imsaidl::MediaConfig req {};
    utfImsAidlRadio->setMediaConfiguration(token, req);
}

void send_vos_action_info(uint32_t token, void *msg) {
  ims_VosActionInfo *i_req = (ims_VosActionInfo *)msg;
  imsaidl::VosActionInfo req = convertToAidl(*i_req);
  utfImsAidlRadio->sendVosActionInfo(token, req);
}

void send_i_ims_aidl_radio_request(ims_MsgType type, ims_MsgId msgId, uint32_t token, void *msg) {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: send_i_ims_aidl_radio_request");
  if (utfImsAidlRadio != nullptr) {
    switch (msgId) {
      case ims_MsgId_REQUEST_DIAL:
        send_dial_req(token, msg);
        break;
      case ims_MsgId_REQUEST_ADD_PARTICIPANT:
        send_add_participant(token, msg);
        break;
      case ims_MsgId_REQUEST_ANSWER:
        send_answer(token, msg);
        break;
      case ims_MsgId_REQUEST_HANGUP:
        send_hangup(token, msg);
        break;
      case ims_MsgId_REQUEST_IMS_REGISTRATION_STATE: {
        utfImsAidlRadio->getImsRegistrationState(token);
      } break;
      case ims_MsgId_REQUEST_SET_SERVICE_STATUS:
        send_set_service_status_req(token, msg);
        break;
      case ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS:
        send_query_call_forward_status(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS:
        send_set_call_forward_status(token, msg);
        break;
      case ims_MsgId_REQUEST_QUERY_CALL_WAITING:
        send_get_call_waiting(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION:
        send_set_supp_service_notification(token, msg);
        break;
      case ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER:
        send_explicit_call_transfer(token, msg);
        break;
      case ims_MsgId_REQUEST_DEFLECT_CALL:
        send_deflect_call(token, msg);
        break;
      case ims_MsgId_REQUEST_DTMF:
        send_dtmf(token, msg);
        break;
      case ims_MsgId_REQUEST_DTMF_START:
        send_start_dtmf(token, msg);
        break;
      case ims_MsgId_REQUEST_DTMF_STOP:
        utfImsAidlRadio->stopDtmf(token);
        break;
      case ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE:
        send_request_registration_change(token, msg);
        break;
      case ims_MsgId_REQUEST_HOLD:
        send_hold(token, msg);
        break;
      case ims_MsgId_REQUEST_RESUME:
        send_resume(token, msg);
        break;
      case ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO:
        send_geo_location_info(token, msg);
        break;
      case ims_MsgId_REQUEST_CONFERENCE:
        utfImsAidlRadio->conference(token);
        break;
      case ims_MsgId_REQUEST_QUERY_CLIP:
        utfImsAidlRadio->getClip(token);
        break;
      case ims_MsgId_REQUEST_GET_CLIR:
        utfImsAidlRadio->getClir(token);
        break;
      case ims_MsgId_REQUEST_GET_COLR:
        utfImsAidlRadio->getColr(token);
        break;
      case ims_MsgId_REQUEST_SET_CLIR:
        send_set_clir(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_CALL_WAITING:
        send_set_call_waiting(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_COLR:
        send_set_colr(token, msg);
        break;
      case ims_MsgId_REQUEST_SUPP_SVC_STATUS:
        send_supp_service_status(token, msg);
        break;
      case ims_MsgId_REQUEST_QUERY_SERVICE_STATUS:
        utfImsAidlRadio->queryServiceStatus(token);
        break;
      case ims_MsgId_REQUEST_SEND_UI_TTY_MODE:
        send_set_uitty_mode(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_IMS_CONFIG:
        send_set_config(token, msg);
        break;
      case ims_MsgId_REQUEST_GET_IMS_CONFIG:
        send_get_config(token, msg);
        break;
      case ims_MsgId_REQUEST_MODIFY_CALL_INITIATE:
        send_modify_call_initiate(token, msg);
        break;
      case ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM:
        send_modify_call_confirm(token, msg);
        break;
      case ims_MsgId_REQUEST_CANCEL_MODIFY_CALL:
        send_cancel_modify_call(token, msg);
        break;
      case ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
        utfImsAidlRadio->exitEmergencyCallbackMode(token);
        break;
      case ims_MsgId_REQUEST_SEND_SMS:
        send_sms(token, msg);
        break;
      case ims_MsgId_REQUEST_ACK_SMS:
        send_acknowledge_sms(token, msg);
        break;
      case ims_MsgId_REQUEST_REGISTER_MULTI_IDENTITY_LINES:
        send_register_multi_identity_lines(token, msg);
        break;
      case ims_MsgId_REQUEST_QUERY_VIRTUAL_LINE_INFO:
        send_query_virtual_line_info(token, msg);
        break;
      case ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG:
        utfImsAidlRadio->getImsSubConfig(token);
        break;
      case ims_MsgId_REQUEST_SEND_RTT_MSG:
        send_rtt_message(token, msg);
        break;
      case ims_MsgId_REQUEST_SET_MEDIA_INFO:
        send_set_media_config(token, msg);
      case ims_MsgId_REQUEST_QUERY_MULTI_SIM_VOICE_CAPABILITY:
        utfImsAidlRadio->queryMultiSimVoiceCapability(token);
        break;
      case ims_MsgId_REQUEST_SEND_VOS_SUPPORT_STATUS:
        utfImsAidlRadio->sendVosSupportStatus(token, msg);
        break;
      case ims_MsgId_REQUEST_SEND_VOS_ACTION_INFO:
        send_vos_action_info(token, msg);
        break;
      default:
        RIL_UTF_DEBUG("\n QMI_RIL_UTL: unrecognized msgId: %d\n", msgId);
        break;
    }
  } else {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: get(imsaidlradio0) returned null");
  }
}
