/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <functional>
#include "ImsVoiceModule.h"
#include <framework/Module.h>
#include <framework/PolicyManager.h>
#include <framework/SingleDispatchAndFamilyPairTimerRestriction.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilUnsolImsConferenceCallStateCompletedMessage.h>
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <framework/SingleDispatchRestriction.h>
#include "qcril_am.h"
#include "qcril_cm_ss.h"
#include "qcril_other.h"
#include "qcril_qmi_voice.h"
#include "qcril_legacy_apis.h"

#define TAG "RILQ"

ImsVoiceModule::ImsVoiceModule() : VoiceModule("ImsVoiceModule") {
  using std::placeholders::_1;

  std::unordered_map<std::reference_wrapper<message_id_info>, std::function<void(std::shared_ptr<Message>)>>
      subMessageHandler = {
          HANDLER(QcRilRequestImsHoldMessage, ImsVoiceModule::handleQcRilRequestImsHoldMessage),
          HANDLER(QcRilRequestImsResumeMessage, ImsVoiceModule::handleQcRilRequestImsResumeMessageWrapper),
          HANDLER(QcRilRequestImsAddParticipantMessage,
                  ImsVoiceModule::handleQcRilRequestImsAddParticipantMessage),
          HANDLER(QcRilRequestImsModifyCallInitiateMessage,
                  ImsVoiceModule::handleQcRilRequestImsModifyCallInitiateMessage),
          HANDLER(QcRilRequestImsCancelModifyCallMessage,
                  ImsVoiceModule::handleQcRilRequestImsCancelModifyCallMessage),
          HANDLER(QcRilRequestImsModifyCallConfirmMessage,
                  ImsVoiceModule::handleQcRilRequestImsModifyCallConfirmMessage),
          HANDLER(QcRilRequestImsDeflectCallMessage,
                  ImsVoiceModule::handleQcRilRequestImsDeflectCallMessage),
          HANDLER(QcRilRequestImsSetUiTtyModeMessage,
                  ImsVoiceModule::handleQcRilRequestImsSetUiTtyModeMessage),
          HANDLER(QcRilRequestImsSendDtmfMessage,
                  ImsVoiceModule::handleQcRilRequestImsSendDtmfMessage),
          HANDLER(QcRilRequestImsStartDtmfMessage,
                  ImsVoiceModule::handleQcRilRequestImsStartDtmfMessage),
          HANDLER(QcRilRequestImsStopDtmfMessage,
                  ImsVoiceModule::handleQcRilRequestImsStopDtmfMessage),
          HANDLER(IpcMultiSimCapabilityMessage,
                  ImsVoiceModule::handleIpcMultiSimCapabilityMessage),
          HANDLER(QcRilUnsolImsMultiSimVoiceCapabilityChanged,
                  ImsVoiceModule::handleMultiSimCapabilityMessage),
          HANDLER(IpcDisconnectCallsMessage, ImsVoiceModule::handleIpcDisconnectCallsMessage),
          HANDLER(EndCallListMessage, ImsVoiceModule::handleEndCallListMessage),
      };
  mMessageHandler.insert(subMessageHandler.begin(), subMessageHandler.end());
}

ImsVoiceModule::~ImsVoiceModule() {}

void ImsVoiceModule::init() {
  QCRIL_LOG_FUNC_ENTRY();
  /* Call base init before doing any other stuff.*/
  VoiceModule::init();

  // REVIST: who set this? ro property?
  qmi_ril_get_property_value_from_integer(QCRIL_QMI_VOICE_DTMF_INTERVAL, &mDtmfInterval,
                                          QCRIL_QMI_VOICE_DTMF_INTERVAL_VAL);

  // ring1: DTMF related. Set DTMF_START and DTMF_STOP as family pair
  std::vector<std::string> ring1{
      QcRilRequestImsSendDtmfMessage::MESSAGE_NAME,
      QcRilRequestImsStartDtmfMessage::MESSAGE_NAME,
      QcRilRequestImsStopDtmfMessage::MESSAGE_NAME,
  };
  auto rest = std::make_shared<SingleDispatchAndFamilyPairTimerRestriction>();
  if (rest) {
    rest->setMessagePair(QcRilRequestImsStartDtmfMessage::MESSAGE_NAME,
                         QcRilRequestImsStopDtmfMessage::MESSAGE_NAME);
    // Start to Stop time
    rest->setMessageTimer(QcRilRequestImsStartDtmfMessage::MESSAGE_NAME, mDtmfInterval);
    // Pause time
    int timer = 0;
    qmi_ril_get_property_value_from_integer(QCRIL_QMI_VOICE_DTMF_PAUSE_INTERVAL, &timer,
                                            QCRIL_QMI_VOICE_DTMF_INTERVAL_VAL);
    rest->setMessageTimer(QcRilRequestImsStopDtmfMessage::MESSAGE_NAME, timer);
    rest->setMessageTimer(QcRilRequestImsSendDtmfMessage::MESSAGE_NAME, timer);
  }
  PolicyManager::getInstance().setMessageRestriction(ring1, rest);

  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsHoldMessage(
    std::shared_ptr<QcRilRequestImsHoldMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  qcril_qmi_voice_voip_call_info_entry_type *call_info = nullptr;
  qcril_qmi_voice_voip_current_call_summary_type calls_summary = {};
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req = {};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (!msg->hasCallId()) {
        QCRIL_LOG_ERROR("Invalid callId");
        rilErr = RIL_E_INVALID_STATE;
        break;
      }

      qcril_qmi_voice_voip_generate_summary(&calls_summary);
      QCRIL_LOG_ERROR("Number of voip calls = %d", calls_summary.nof_voip_calls);
      if (calls_summary.nof_voip_calls <= QMI_RIL_ZERO) {
        QCRIL_LOG_ERROR("Do not have any voip active calls");
        rilErr = RIL_E_INVALID_STATE;
        break;
      }

      uint8_t call_id = msg->getCallId();
      call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(call_id);
      if (!call_info) {
        QCRIL_LOG_ERROR("Do not have any voip active calls or invalid call id in request");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_CALL_HOLD_V02;
      manage_ip_calls_req.call_id_valid = TRUE;
      manage_ip_calls_req.call_id = call_info->voice_scv_info.call_id;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_manage_ip_calls_req(
          req_id, manage_ip_calls_req,
          std::bind(&ImsVoiceModule::processHoldResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsResumeMessageWrapper(
    std::shared_ptr<QcRilRequestImsResumeMessage> msg) {
  RIL_Errno rilErr = RIL_E_SUCCESS;
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  // Check if we can release audio on the other sub.
  if(isReleaseAudioOnOtherSubRequired()) {
    rilErr = dispatchReleaseAudio(msg);
    if(rilErr != RIL_E_SUCCESS) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  } else {
    // If there is no call on the other sub, we just acquire audio outright.
    setIsAudioReleased(false);
    handleQcRilRequestResumeMessage(msg);
  }
}

void ImsVoiceModule::handleQcRilRequestImsAddParticipantMessage(
    std::shared_ptr<QcRilRequestImsAddParticipantMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  qcril_qmi_voice_voip_current_call_summary_type calls_summary = {};
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req = {};
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = nullptr;
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (msg->getAddress().size() > QMI_VOICE_SIP_URI_MAX_V02) {
        QCRIL_LOG_ERROR("invalid parameter");
        rilErr = RIL_E_INVALID_STATE;
        break;
      }

      qcril_qmi_voice_voip_generate_summary(&calls_summary);
      QCRIL_LOG_DEBUG("number of calls = %d", calls_summary.nof_calls_overall);

      if (calls_summary.nof_calls_overall == 1) {
        call_info_entry = calls_summary.active_or_single_call;
      } else {
        // Get the call in conversation state if more than one call
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state(
            CALL_STATE_CONVERSATION_V02);
      }

      QCRIL_LOG_INFO(".. call_info_entry %p", call_info_entry);

      if (!call_info_entry || (VOICE_INVALID_CALL_ID == call_info_entry->qmi_call_id) ||
          !(CALL_STATE_CONVERSATION_V02 == call_info_entry->voice_scv_info.call_state ||
            CALL_STATE_HOLD_V02 == call_info_entry->voice_scv_info.call_state) ||
          !(CALL_TYPE_VOICE_IP_V02 == call_info_entry->voice_scv_info.call_type ||
            CALL_TYPE_VT_V02 == call_info_entry->voice_scv_info.call_type)) {
        QCRIL_LOG_ERROR("No valid call info entry");
        qcril_qmi_voice_voip_call_info_dump(call_info_entry);
        rilErr = RIL_E_INVALID_STATE;
        break;
      }

      manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_ADD_PARTICIPANT_V02;
      manage_ip_calls_req.call_id_valid = TRUE;
      manage_ip_calls_req.call_id = call_info_entry->voice_scv_info.call_id;
      manage_ip_calls_req.sip_uri_valid = TRUE;
      memcpy(&manage_ip_calls_req.sip_uri, msg->getAddress().c_str(), msg->getAddress().size());

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_manage_ip_calls_req(
          req_id, manage_ip_calls_req,
          std::bind(&ImsVoiceModule::processAddParticipantResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsModifyCallInitiateMessage(
    std::shared_ptr<QcRilRequestImsModifyCallInitiateMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req = {};
  qcril_qmi_voice_voip_call_info_entry_type *call_info = nullptr;
  auto pendingMsgStatus = std::make_pair(0, false);
  boolean is_emergency_ip = FALSE;

  if (msg != nullptr) {
    do {
      if (msg->hasCallId() && msg->getCallId() >= VOICE_LOWEST_CALL_ID &&
          msg->getCallId() <= VOICE_HIGHEST_CALL_ID) {
        call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(msg->getCallId());
      } else {
        QCRIL_LOG_ERROR("Invalid CallId");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      if (call_info == NULL) {
        QCRIL_LOG_DEBUG("android call-id = %d not found, rejecting modify initiate request",
                        msg->getCallId());
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      if (call_info->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_IP_V02 ||
          call_info->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_VT_V02) {
        is_emergency_ip = TRUE;
      }
      if (!msg->hasCallType() || msg->getCallType() == qcril::interfaces::CallType::UNKNOWN) {
        QCRIL_LOG_DEBUG("Invalid CallType");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      if (!msg->hasCallDomain() || msg->getCallDomain() == qcril::interfaces::CallDomain::UNKNOWN) {
        QCRIL_LOG_DEBUG("Invalid CallDomain");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint8_t ret = convert_call_info_to_qmi(
          msg->getCallType(), msg->getCallDomain(), is_emergency_ip, manage_ip_calls_req.call_type,
          manage_ip_calls_req.audio_attrib_valid, manage_ip_calls_req.audio_attrib,
          manage_ip_calls_req.video_attrib_valid, manage_ip_calls_req.video_attrib);

      if (!ret) {
        QCRIL_LOG_DEBUG("Failed to convert the call type details");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      if (msg->hasRttMode()) {
        manage_ip_calls_req.rtt_mode_valid =
            convert_rtt_mode_to_qmi(msg->getRttMode(), manage_ip_calls_req.rtt_mode);
      }
      manage_ip_calls_req.call_type_valid = TRUE;
      manage_ip_calls_req.call_id_valid = TRUE;
      manage_ip_calls_req.call_id = call_info->qmi_call_id;
      manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_MODIFY_CALL_V02;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      msg->setPendingMessageState(PendingMessageState::AWAITING_CALLBACK);
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_manage_ip_calls_req(
          req_id, manage_ip_calls_req,
          std::bind(&ImsVoiceModule::processModifyCallInitiateResponse, this,
                    std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsCancelModifyCallMessage(
    std::shared_ptr<QcRilRequestImsCancelModifyCallMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_ims_call_cancel_req_msg_v02 call_cancel_req = {};
  qcril_qmi_voice_voip_call_info_entry_type *call_info = nullptr;
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (!msg->hasCallId()) {
        QCRIL_LOG_ERROR("Invalid call id");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(msg->getCallId());

      if (call_info == nullptr) {
        QCRIL_LOG_ERROR("android call-id = %d not found, rejecting modify initiate request",
                        msg->getCallId());
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      call_cancel_req.call_id = call_info->qmi_call_id;
      call_cancel_req.cancel_type = VOIP_CANCEL_TYPE_CANCEL_CALL_UPGRADE_V02;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_ims_call_cancel_req(
          req_id, call_cancel_req,
          std::bind(&ImsVoiceModule::processCancelModifyCallResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsModifyCallConfirmMessage(
    std::shared_ptr<QcRilRequestImsModifyCallConfirmMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req{};
  qcril_qmi_voice_voip_call_info_entry_type *call_info = nullptr;
  auto pendingMsgStatus = std::make_pair(0, false);
  boolean is_emergency_ip = FALSE;

  if (msg != nullptr) {
    do {
      memset(&manage_ip_calls_req, 0, sizeof(manage_ip_calls_req));

      if (msg->hasCallId() && msg->getCallId() >= VOICE_LOWEST_CALL_ID &&
          msg->getCallId() <= VOICE_HIGHEST_CALL_ID) {
        call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(msg->getCallId());
      } else {
        QCRIL_LOG_ERROR("Invalid CallId");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      if (call_info == NULL) {
        QCRIL_LOG_DEBUG("android call-id = %d not found, rejecting modify initiate request",
                        msg->getCallId());
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      if (call_info->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_IP_V02 ||
          call_info->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_VT_V02) {
        is_emergency_ip = TRUE;
      }
      if (!msg->hasCallType() || msg->getCallType() == qcril::interfaces::CallType::UNKNOWN) {
        QCRIL_LOG_DEBUG("Invalid CallType");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      if (!msg->hasCallDomain() || msg->getCallDomain() == qcril::interfaces::CallDomain::UNKNOWN) {
        QCRIL_LOG_DEBUG("Invalid CallDomain");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint8_t ret = convert_call_info_to_qmi(
          msg->getCallType(), msg->getCallDomain(), is_emergency_ip, manage_ip_calls_req.call_type,
          manage_ip_calls_req.audio_attrib_valid, manage_ip_calls_req.audio_attrib,
          manage_ip_calls_req.video_attrib_valid, manage_ip_calls_req.video_attrib);
      manage_ip_calls_req.call_type_valid = ret;

      if (!ret) {
        QCRIL_LOG_DEBUG("Failed to convert the call type details");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      if (msg->hasRttMode()) {
        manage_ip_calls_req.rtt_mode_valid =
            convert_rtt_mode_to_qmi(msg->getRttMode(), manage_ip_calls_req.rtt_mode);
      }

      bool is_audio_attrib_valid =
          (call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUDIO_ATTR_VALID) ? true
                                                                                        : false;
      bool is_video_attrib_valid =
          (call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VIDEO_ATTR_VALID) ? true
                                                                                        : false;
      bool is_rtt_mode_valid =
          (call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_MODE_VALID) ? true
                                                                                      : false;

      bool accept = !qcril_qmi_voice_match_modem_call_type(
          manage_ip_calls_req.call_type, manage_ip_calls_req.audio_attrib_valid,
          manage_ip_calls_req.audio_attrib, manage_ip_calls_req.video_attrib_valid,
          manage_ip_calls_req.video_attrib, manage_ip_calls_req.rtt_mode_valid,
          manage_ip_calls_req.rtt_mode, call_info->voice_scv_info.call_type, is_audio_attrib_valid,
          call_info->voice_audio_attrib.call_attributes, is_video_attrib_valid,
          call_info->voice_video_attrib.call_attributes,
          is_rtt_mode_valid, call_info->rtt_mode.rtt_mode);

      if (accept) {
        manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_MODIFY_ACCEPT_V02;
      } else {
        manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_MODIFY_REJECT_V02;
      }

      manage_ip_calls_req.call_id_valid = TRUE;
      manage_ip_calls_req.call_id = call_info->qmi_call_id;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_manage_ip_calls_req(
          req_id, manage_ip_calls_req,
          std::bind(&ImsVoiceModule::processModifyCallConfirmResponse, this,
                    std::placeholders::_1));

      if (rilErr == RIL_E_SUCCESS) {
        call_info->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MODIFY_CONFIRM_PENDING;
      }
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsDeflectCallMessage(
    std::shared_ptr<QcRilRequestImsDeflectCallMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  qcril_qmi_voice_voip_call_info_entry_type *call_info = nullptr;
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req = {};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (!msg->hasCallId() || !msg->hasNumber()) {
        QCRIL_LOG_ERROR("Mandatory params missing");
        rilErr = RIL_E_INVALID_ARGUMENTS;
        break;
      }

      uint8_t call_id = msg->getCallId();
      QCRIL_LOG_ERROR("call id = %d", call_id);
      call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(call_id);
      if (!call_info) {
        QCRIL_LOG_ERROR("Do not have any voip active calls or invalid call id in request");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }

      manage_ip_calls_req.sups_type = VOIP_SUPS_TYPE_CALL_DEFLECTION_V02;
      manage_ip_calls_req.call_id_valid = TRUE;
      manage_ip_calls_req.call_id = call_info->voice_scv_info.call_id;

      if (msg->hasNumber() && msg->getNumber().size() > 0) {
        manage_ip_calls_req.sip_uri_valid = TRUE;
        strlcpy(manage_ip_calls_req.sip_uri, msg->getNumber().c_str(),
                sizeof(manage_ip_calls_req.sip_uri));
      }

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_manage_ip_calls_req(
          req_id, manage_ip_calls_req,
          std::bind(&ImsVoiceModule::processDeflectCallResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsSetUiTtyModeMessage(
    std::shared_ptr<QcRilRequestImsSetUiTtyModeMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_set_config_req_msg_v02 set_config_req = {};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      bool isValid = false;
      if (msg->hasTtyMode()) {
        isValid = qcril_qmi_voice_map_ril_tty_mode_to_qmi_tty_mode(set_config_req.ui_tty_setting,
                                                                   msg->getTtyMode());
        set_config_req.ui_tty_setting_valid = isValid ? TRUE : FALSE;
      }
      if (!isValid) {
        QCRIL_LOG_ERROR("Invalid parameter: ttyMode");
        rilErr = RIL_E_INVALID_ARGUMENTS;
        break;
      }

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_set_config_req_sync(
          req_id, set_config_req,
          std::bind(&ImsVoiceModule::processSetUiTtyModeResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsSendDtmfMessage(
    std::shared_ptr<QcRilRequestImsSendDtmfMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_start_cont_dtmf_req_msg_v02 start_cont_dtmf_req = {};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (!msg->hasDigit()) {
        QCRIL_LOG_ERROR("Invalid parameter: digit");
        rilErr = RIL_E_INVALID_ARGUMENTS;
        break;
      }

      start_cont_dtmf_req.cont_dtmf_info.digit = msg->getDigit();
      start_cont_dtmf_req.cont_dtmf_info.call_id = QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_start_cont_dtmf_req(
          req_id, start_cont_dtmf_req,
          std::bind(&ImsVoiceModule::processStartContDtmfResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsStartDtmfMessage(
    std::shared_ptr<QcRilRequestImsStartDtmfMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_start_cont_dtmf_req_msg_v02 start_cont_dtmf_req = {};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      if (!msg->hasDigit()) {
        QCRIL_LOG_ERROR("Invalid parameter: digit");
        rilErr = RIL_E_INVALID_ARGUMENTS;
        break;
      }

      start_cont_dtmf_req.cont_dtmf_info.digit = msg->getDigit();
      start_cont_dtmf_req.cont_dtmf_info.call_id = QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID;

      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_start_cont_dtmf_req(
          req_id, start_cont_dtmf_req,
          std::bind(&ImsVoiceModule::processStartContDtmfResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleQcRilRequestImsStopDtmfMessage(
    std::shared_ptr<QcRilRequestImsStopDtmfMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  RIL_Errno rilErr = RIL_E_SUCCESS;
  voice_stop_cont_dtmf_req_msg_v02 stop_cont_dtmf_req = {
      .call_id = QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID};
  auto pendingMsgStatus = std::make_pair(0, false);

  if (msg != nullptr) {
    do {
      pendingMsgStatus = getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;

      rilErr = qcril_qmi_voice_process_stop_cont_dtmf_req(
          req_id, stop_cont_dtmf_req,
          std::bind(&ImsVoiceModule::processStopContDtmfResponse, this, std::placeholders::_1));
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getPendingMessageList().erase(pendingMsgStatus.first);
      }
      std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}


/*===============================================================================================
 * Callback functions
 *===============================================================================================*/

void ImsVoiceModule::processAddParticipantResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);

    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
    if (pendingMsg) {
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processHoldResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
  voice_manage_ip_calls_resp_msg_v02 *qmiResp = nullptr;

  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);

    if (data->errNo != RIL_E_SUCCESS) {
      uint32_t failureCauseDescriptionLen = 0;
      uint16_t *failureCauseDescription = nullptr;
      qmiResp = static_cast<voice_manage_ip_calls_resp_msg_v02 *>(data->respData);
      if (qmiResp) {
        if (qmiResp->end_reason_text_valid && qmiResp->end_reason_text_len > 0) {
          for (uint32_t i = 0; i < qmiResp->end_reason_text_len; i++) {
            if (qmiResp->end_reason_text[i].end_reason_text_len > 0) {
              failureCauseDescriptionLen = qmiResp->end_reason_text[i].end_reason_text_len;
              failureCauseDescription = qmiResp->end_reason_text[i].end_reason_text;
            }
          }
        }
        errorDetails = buildSipErrorInfo(qmiResp->sip_error_code_valid, qmiResp->sip_error_code,
                                         failureCauseDescriptionLen, failureCauseDescription);
      }
    }

    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, errorDetails);
    if (pendingMsg) {
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processDeflectCallResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);
    if (pendingMsg) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processModifyCallInitiateResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  voice_manage_ip_calls_resp_msg_v02 *qmiResp = nullptr;

  if (data) {
    auto pendingMsg = getPendingMessageList().find(data->req_id);

    qmiResp = static_cast<voice_manage_ip_calls_resp_msg_v02 *>(data->respData);
    if (data->errNo == RIL_E_SUCCESS) {
      /* wait for modify indication, update state to wait for more events */
      if (pendingMsg) {
        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        msg->setPendingMessageState(PendingMessageState::AWAITING_INDICATION);
      }
    } else {
      getPendingMessageList().erase(data->req_id);

      if (qmiResp && (qmiResp->failure_cause_valid == TRUE) &&
          (qmiResp->failure_cause == QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_REJ_V02)) {
        data->errNo = RIL_E_REJECTED_BY_REMOTE;
      }
      if (pendingMsg) {
        auto respPayload =
            std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      }
    }
    if (qmiResp && qmiResp->call_modified_cause_valid && qmiResp->call_id_valid) {
      qcril_qmi_voice_call_info_entry_set_call_modified_cause(
          qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(qmiResp->call_id),
          qmiResp->call_modified_cause);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processModifyCallConfirmResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  voice_manage_ip_calls_resp_msg_v02 *qmiResp = nullptr;

  if (data) {
    auto pendingMsg = getPendingMessageList().find(data->req_id);
    qmiResp = static_cast<voice_manage_ip_calls_resp_msg_v02 *>(data->respData);
    if (data->errNo == RIL_E_SUCCESS) {
      /* wait for modify indication, update state to wait for more events */
      if (pendingMsg) {
        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        msg->setPendingMessageState(PendingMessageState::AWAITING_INDICATION);
      }
    } else {
      getPendingMessageList().erase(data->req_id);

      if (pendingMsg) {
        auto respPayload =
            std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
        auto msg(std::static_pointer_cast<QcRilRequestImsModifyCallConfirmMessage>(pendingMsg));
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      }
    }
    if (qmiResp && qmiResp->call_modified_cause_valid && qmiResp->call_id_valid) {
      qcril_qmi_voice_call_info_entry_set_call_modified_cause(
          qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(qmiResp->call_id),
          qmiResp->call_modified_cause);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processCancelModifyCallResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);

    if (pendingMsg) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processSetUiTtyModeResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  voice_set_config_resp_msg_v02 *qmiResp = nullptr;
  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);
    qmiResp = static_cast<voice_set_config_resp_msg_v02 *>(data->respData);
    if (qmiResp && (data->errNo == RIL_E_SUCCESS)) {
      QCRIL_LOG_INFO(".. outcome  %d, %d", qmiResp->ui_tty_setting_outcome_valid,
                     qmiResp->ui_tty_setting_outcome);
      if (qmiResp->ui_tty_setting_outcome_valid && qmiResp->ui_tty_setting_outcome != 0x00) {
        data->errNo = RIL_E_MODEM_ERR;
      }
    }
    if (pendingMsg) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processStartContDtmfResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  if (data) {
    bool sendResp = true;
    auto pendingMsg = getPendingMessageList().find(data->req_id);
    if (pendingMsg &&
        (pendingMsg->get_message_id() ==
                QcRilRequestImsSendDtmfMessage::get_class_message_id())) {
      sendResp = false;
      TimeKeeper::millisec timer = mDtmfInterval;
      uint16_t req_id = data->req_id;
      RIL_Errno errNo = data->errNo;
      TimeKeeper::getInstance().set_timer(
          [this, req_id, errNo](void *user_data) {
            QCRIL_NOTUSED(user_data);
            voice_stop_cont_dtmf_req_msg_v02 stop_cont_dtmf_req = {
                .call_id = QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID};
            RIL_Errno rilErr = qcril_qmi_voice_process_stop_cont_dtmf_req(
                req_id, stop_cont_dtmf_req,
                std::bind(&ImsVoiceModule::processStopContDtmfResponse, this,
                          std::placeholders::_1));
            if (rilErr != RIL_E_SUCCESS) {
              auto pendingMsg = getPendingMessageList().extract(req_id);
              if (pendingMsg) {
                auto respPayload =
                    std::make_shared<QcRilRequestMessageCallbackPayload>(errNo, nullptr);
                auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
                msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
              }
            }
          },
          nullptr, timer);
    }
    if (sendResp) {
      auto pendingMsg = getPendingMessageList().extract(data->req_id);
      if (pendingMsg) {
        auto respPayload =
            std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::processStopContDtmfResponse(CommonVoiceResponseData *data) {
  QCRIL_LOG_FUNC_ENTRY();
  if (data) {
    auto pendingMsg = getPendingMessageList().extract(data->req_id);
    if (pendingMsg) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(data->errNo, nullptr);
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void ImsVoiceModule::handleMultiSimCapabilityMessage
  (std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  if(msg && msg->getMultiSimVoiceCapability() != qcril::interfaces::MultiSimVoiceCapability::UNKNOWN) {
    // Update voice module cache
    mMultiSimVoiceCap = msg->getMultiSimVoiceCapability();

    // This msg is supposed to come only from RIL bound with primary stack of modem.
    // Send this infromation to other RILD
    auto ipcMsg = std::make_shared<IpcMultiSimCapabilityMessage>(qmi_ril_get_process_instance_id());
    if (ipcMsg) {
      ipcMsg->setMultiSimVoiceCapability(msg->getMultiSimVoiceCapability());
      ipcMsg->broadcast();
    }
  }
}

void ImsVoiceModule::handleIpcMultiSimCapabilityMessage
  (std::shared_ptr<IpcMultiSimCapabilityMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  // This code is RIL instance agnostic, only one RILD should be sending
  // IpcMultiSimCapabilityMessage, other RILD will just update its internal cache.
  if (msg && msg->getIsRemote()) {
    if(msg->getMultiSimVoiceCapability() != qcril::interfaces::MultiSimVoiceCapability::UNKNOWN) {
      mMultiSimVoiceCap = msg->getMultiSimVoiceCapability();
    }
  }
}
