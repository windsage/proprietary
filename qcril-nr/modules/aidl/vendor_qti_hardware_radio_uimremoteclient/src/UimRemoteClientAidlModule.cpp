/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include <framework/ModuleLooper.h>
#include <framework/Log.h>
#include "UimRemoteClientAidlModule.h"
#include "UimRmtClientRequestCallbackAidl.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <modules/qmi/ModemEndPointFactory.h>
#include <modules/qmi/UimRemoteModemEndPoint.h>
#include <modules/qmi/QmiUimRemoteSetupRequestCallback.h>
#include "modules/uim_remote/UimRmtQmiUimRemoteReqMsg.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"

static load_module<UimRemoteClientAidlModule> sUimRemoteClientAidlModule;

static bool isUimRemoteSimConnected = false;

UimRemoteClientAidlModule::UimRemoteClientAidlModule()
{
  mName = "UimRemoteClientAidlModule";
  mReady = false;
  mApduId = 0;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, UimRemoteClientAidlModule::handleQcrilInitMessage),
    HANDLER(UimRmtQmiUimRemoteRespMsg, UimRemoteClientAidlModule::handleUimRemoteRespMessage),
    HANDLER(UimRmtQmiUimRemoteIndMsg, UimRemoteClientAidlModule::handleUimRemoteIndMessage),
    HANDLER(UimRmtCardAppInitStatusIndMsg,
            UimRemoteClientAidlModule::handleUimRemoteCardAppInitStatusIndMessage),
  };
}

UimRemoteClientAidlModule::~UimRemoteClientAidlModule()
{
  mReady = false;
}

void UimRemoteClientAidlModule::init()
{
  Module::init();

  // Initializations complete.
  mReady = false;

  // Clear the remote SIM in Connected mode flag
  isUimRemoteSimConnected = false;

  ModemEndPointFactory<UimRemoteModemEndPoint>::getInstance().buildEndPoint();
}

bool UimRemoteClientAidlModule::isReady()
{
  return mReady;
} /* UimRemoteClientAidlModule::isReady */

/*===========================================================================

FUNCTION:  UimRemoteClientAidlModule::handleQcrilInitMessage

===========================================================================*/
void UimRemoteClientAidlModule::handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg)
{
  QmiUimRemoteSetupRequestCallback qmiSetupCallback("Uim-Module-Token");
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));

  if (mUimRemoteServiceClientImpl == nullptr) {
    mUimRemoteServiceClientImpl =
        ndk::SharedRefBase::make<aidlimplimports::IUimRemoteServiceClientImpl>(this);
    mUimRemoteServiceClientImpl->setInstanceId((instance_id));
    std::string serviceInstance =
        std::string(aidlimplimports::IUimRemoteServiceClientImpl::descriptor) + "/uimRemoteClient" +
        std::to_string(instance_id);

    binder_status_t status = AServiceManager_addService(
        mUimRemoteServiceClientImpl->asBinder().get(), serviceInstance.c_str());

    ModemEndPointFactory<UimRemoteModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
        "Client-Uim-remote-Serv-Token", msg->get_instance_id(), &qmiSetupCallback);

    QCRIL_LOG_INFO("UimRemoteClient stable AIDL addService, status = %d", status);
    if (status != STATUS_OK) {
      mUimRemoteServiceClientImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }
}

/*===========================================================================

FUNCTION:  UimRemoteClientAidlModule::qcril_uim_remote_client_request_event

===========================================================================*/
void UimRemoteClientAidlModule::qcril_uim_remote_client_request_event(
    int32_t token, qcril_uim_remote_event_req_type* user_req_ptr)
{
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg> req_ptr = nullptr;
  UimRmtClientRequestCallbackAidl req_cb("UIM Remote REQUEST");
  std::shared_ptr<qmi_uim_remote_request_msg> req_data_ptr = nullptr;
  int instance_id = mUimRemoteServiceClientImpl->getInstanceId();
  qmi_uim_remote_slot_type slot = QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;

  req_data_ptr = std::shared_ptr<qmi_uim_remote_request_msg>(new qmi_uim_remote_request_msg{});

  slot = qcril_uim_remote_convert_instance_to_slot_type(instance_id);

  if (!mReady || user_req_ptr == nullptr || req_data_ptr == nullptr) {
    if (mUimRemoteServiceClientImpl != nullptr) {
      mUimRemoteServiceClientImpl->sendUimRemoteClientEventResponse(
          token,
          static_cast<aidlimports::UimRemoteClientEventRsp>(QCRIL_UIM_REMOTE_CLIENT_EVENT_FAILURE));
    }
    return;
  }

  QCRIL_LOG_INFO("qcril_uim_remote_client_request_event: token :%d, event: %d",
                 token,
                 user_req_ptr->event_type);

  req_data_ptr->token = token;
  req_data_ptr->req_id = QMI_UIM_REMOTE_REQ_EVENT_REQUEST;

  req_data_ptr->req_data.event_req.event =
      static_cast<qmi_uim_remote_event_type>(user_req_ptr->event_type);
  req_data_ptr->req_data.event_req.slot = slot;

  if (user_req_ptr->atr_len > 0 && user_req_ptr->atr_len <= QMI_UIM_REMOTE_MAX_ATR_LEN) {
    memcpy(req_data_ptr->req_data.event_req.atr, user_req_ptr->atr, user_req_ptr->atr_len);
    req_data_ptr->req_data.event_req.atr_len = user_req_ptr->atr_len;
  }

  req_data_ptr->req_data.event_req.wakeup_support_valid = user_req_ptr->has_wakeup_support;
  req_data_ptr->req_data.event_req.wakeup_support = user_req_ptr->wakeup_support;
  req_data_ptr->req_data.event_req.error_cause_valid = user_req_ptr->has_error_code;
  req_data_ptr->req_data.event_req.error_cause =
      static_cast<qmi_uim_remote_card_error_type>(user_req_ptr->error_code);
  req_data_ptr->req_data.event_req.transport_valid = user_req_ptr->has_transport;
  req_data_ptr->req_data.event_req.transport =
      static_cast<qmi_uim_remote_transport>(user_req_ptr->transport);
  req_data_ptr->req_data.event_req.usage_valid = user_req_ptr->has_usage;
  req_data_ptr->req_data.event_req.usage = static_cast<qmi_uim_remote_usage>(user_req_ptr->usage);
  req_data_ptr->req_data.event_req.apdu_timeout_valid = user_req_ptr->has_apdu_timeout;
  req_data_ptr->req_data.event_req.apdu_timeout = user_req_ptr->apdu_timeout;
  req_data_ptr->req_data.event_req.disable_all_polling_valid = user_req_ptr->has_disable_all_polling;
  req_data_ptr->req_data.event_req.disable_all_polling = user_req_ptr->disable_all_polling;
  req_data_ptr->req_data.event_req.poll_interval_valid = user_req_ptr->has_poll_timer;
  req_data_ptr->req_data.event_req.poll_interval = user_req_ptr->poll_timer;

  req_ptr = std::make_shared<UimRmtQmiUimRemoteReqMsg>(req_data_ptr, &req_cb);

  if (req_ptr != nullptr) {
    req_ptr->dispatch();
  } else {
    if (mUimRemoteServiceClientImpl != nullptr) {
      mUimRemoteServiceClientImpl->sendUimRemoteClientEventResponse(
          token,
          static_cast<aidlimports::UimRemoteClientEventRsp>(QCRIL_UIM_REMOTE_CLIENT_EVENT_FAILURE));
    }
  }
} /* UimRemoteClientAidlModule::qcril_uim_remote_client_request_event */

/*===========================================================================

FUNCTION:  UimRemoteClientAidlModule::handleUimRemoteRespMessage

===========================================================================*/
void UimRemoteClientAidlModule::handleUimRemoteRespMessage(
    std::shared_ptr<UimRmtQmiUimRemoteRespMsg> msg_ptr)
{
  std::shared_ptr<qmi_uim_remote_resp_msg> rsp_data_ptr = nullptr;
  int32_t token = -1;

  if (msg_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  rsp_data_ptr = msg_ptr->get_message();
  token = msg_ptr->get_token();

  QCRIL_LOG_INFO("handleUimRemoteRespMessage, token: %d", token);

  switch (msg_ptr->get_req_type()) {
    case QMI_UIM_REMOTE_REQ_EVENT_REQUEST:
      if (rsp_data_ptr == nullptr || rsp_data_ptr->trans_err != 0 || rsp_data_ptr->err_code != 0 ||
          rsp_data_ptr->result != QMI_UIM_REMOTE_RESULT_SUCCESS) {
        mUimRemoteServiceClientImpl->sendUimRemoteClientEventResponse(
            token,
            static_cast<aidlimports::UimRemoteClientEventRsp>(
                QCRIL_UIM_REMOTE_CLIENT_EVENT_FAILURE));
      } else {
        mUimRemoteServiceClientImpl->sendUimRemoteClientEventResponse(
            token,
            static_cast<aidlimports::UimRemoteClientEventRsp>(
                QCRIL_UIM_REMOTE_CLIENT_EVENT_SUCCESS));
      }
      break;
    case QMI_UIM_REMOTE_REQ_APDU_REQUEST:
      if (rsp_data_ptr == nullptr || rsp_data_ptr->trans_err != 0 || rsp_data_ptr->err_code != 0 ||
          rsp_data_ptr->result != QMI_UIM_REMOTE_RESULT_SUCCESS) {
        mUimRemoteServiceClientImpl->sendUimRemoteClientApduResponse(
            token,
            static_cast<aidlimports::UimRemoteClientApduRsp>(QCRIL_UIM_REMOTE_CLIENT_APDU_FAILURE));
      } else {
        mUimRemoteServiceClientImpl->sendUimRemoteClientApduResponse(
            token,
            static_cast<aidlimports::UimRemoteClientApduRsp>(QCRIL_UIM_REMOTE_CLIENT_APDU_SUCCESS));
      }
      break;
    default:
      QCRIL_LOG_ERROR("Invalid request: %d", msg_ptr->get_req_type());
      break;
  }
}

/*===========================================================================

FUNCTION:  UimRemoteClientAidlModule::handleUimRemoteIndMessage

===========================================================================*/
void UimRemoteClientAidlModule::handleUimRemoteIndMessage(
    std::shared_ptr<UimRmtQmiUimRemoteIndMsg> msg_ptr)
{
  std::shared_ptr<qmi_uim_remote_ind_msg> ind_data_ptr = nullptr;

  if (msg_ptr == nullptr || mUimRemoteServiceClientImpl == nullptr) {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  ind_data_ptr = msg_ptr->get_message();

  if (ind_data_ptr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null ind_data_ptr");
    return;
  }

  QCRIL_LOG_INFO("QMI UIM remote Indication : %d", ind_data_ptr->ind_id);

  switch (ind_data_ptr->ind_id) {
    case QMI_UIM_REMOTE_SRVC_UP_IND_MSG:
      mReady = TRUE;
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientServiceInd(true);
      break;
    case QMI_UIM_REMOTE_SRVC_DOWN_IND_MSG:
      mReady = FALSE;
      isUimRemoteSimConnected = false;
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientServiceInd(false);
      break;
    case QMI_UIM_REMOTE_APDU_IND:
      mApduId = ind_data_ptr->ind_data.apdu_ind.apdu_id;
      if (ind_data_ptr->ind_data.apdu_ind.command_apdu_len > 0 &&
          ind_data_ptr->ind_data.apdu_ind.command_apdu_len <= QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN) {
        uint8_t* cmd_apdu = ind_data_ptr->ind_data.apdu_ind.command_apdu;
        std::vector<uint8_t> data(cmd_apdu,
                                  cmd_apdu + ind_data_ptr->ind_data.apdu_ind.command_apdu_len);
        mUimRemoteServiceClientImpl->sendUimRemoteServiceClientApduInd(
            ind_data_ptr->ind_data.apdu_ind.command_apdu_len, data);
      }
      break;
    case QMI_UIM_REMOTE_CONNECT_IND:
      isUimRemoteSimConnected = true;
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientConnectInd();
      break;
    case QMI_UIM_REMOTE_DISCONNECT_IND:
      isUimRemoteSimConnected = false;
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientResetInd();
      break;
    case QMI_UIM_REMOTE_POWER_DOWN_IND:
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientPowerDownInd(
          (bool)ind_data_ptr->ind_data.pdown_ind.mode_valid,
          (aidlimports::UimRemoteClientPowerDownMode)ind_data_ptr->ind_data.pdown_ind.mode);
      break;
    case QMI_UIM_REMOTE_POWER_UP_IND:
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientPowerUpInd(
          static_cast<bool>(ind_data_ptr->ind_data.pup_ind.response_timeout_valid),
          ind_data_ptr->ind_data.pup_ind.response_timeout,
          static_cast<bool>(ind_data_ptr->ind_data.pup_ind.voltage_class_valid),
          static_cast<aidlimports::UimRemoteClientVoltageClass>(
              ind_data_ptr->ind_data.pup_ind.voltage_class));
      break;
    case QMI_UIM_REMOTE_CARD_RESET_IND:
      mUimRemoteServiceClientImpl->sendUimRemoteServiceClientResetInd();
      break;
    default:
      break;
  }
} /* UimRemoteClientModule::handleUimRemoteIndMessage */

/*=========================================================================

  FUNCTION:  handleUimRemoteCardAppInitStatusIndMessage

===========================================================================*/
/*!
    @brief
    Uim Remote Card App Initialization status indication message sent from
    UIM module to UIM remote client module

    @return
    None
*/
/*=========================================================================*/

void UimRemoteClientAidlModule::handleUimRemoteCardAppInitStatusIndMessage(
    std::shared_ptr<UimRmtCardAppInitStatusIndMsg> msgPtr)
{
  QCRIL_LOG_INFO("handleUimRemoteCardAppInitStatusIndMessage ");

  if (msgPtr == nullptr || mUimRemoteServiceClientImpl == nullptr) {
    QCRIL_LOG_ERROR("Invalid null msgPtr");
    return;
  }

  const UimRmtCardAppInitStatusIndData* indDataPtr = nullptr;

  indDataPtr = msgPtr->getCardAppInitStatusIndData();

  if (indDataPtr == nullptr) {
    QCRIL_LOG_ERROR("Invalid null indDataPtr");
    return;
  }

  if (isUimRemoteSimConnected) {
    mUimRemoteServiceClientImpl->sendUimRemoteServiceClientCardInitStatusInd(indDataPtr);
  } else {
    QCRIL_LOG_ERROR(
        "UIM remote SIM is not in connected Mode - Not sending CardAppInitStatusInd to remote "
        "clients ");
    return;
  }
} /* UimRemoteClientAidlModule::handleUimRemoteCardAppInitStatusIndMessage */

/*=========================================================================

  FUNCTION:  qcril_uim_remote_convert_instance_to_slot_type

===========================================================================*/
qmi_uim_remote_slot_type UimRemoteClientAidlModule::qcril_uim_remote_convert_instance_to_slot_type(
    uint32_t instance)
{
  switch (instance) {
    case 0:
      return QMI_UIM_REMOTE_SLOT_1;

    case 1:
      return QMI_UIM_REMOTE_SLOT_2;

    case 2:
      return QMI_UIM_REMOTE_SLOT_3;

    default:
      break;
  }

  return QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;
}

/*===========================================================================

FUNCTION:  UimRemoteClientAidlModule::qcril_uim_remote_client_request_apdu

===========================================================================*/
void UimRemoteClientAidlModule::qcril_uim_remote_client_request_apdu(
    int32_t token,
    aidlimports::UimRemoteClientApduStatus apdu_status,
    uint32_t apdu_data_len,
    const uint8_t* apdu_ptr)
{
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg> req_ptr = nullptr;
  UimRmtClientRequestCallbackAidl req_cb("UIM Remote REQUEST");
  std::shared_ptr<qmi_uim_remote_request_msg> req_data_ptr = nullptr;
  int instance_id = mUimRemoteServiceClientImpl->getInstanceId();
  qmi_uim_remote_slot_type slot = QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE;

  req_data_ptr = std::shared_ptr<qmi_uim_remote_request_msg>(new qmi_uim_remote_request_msg{});

  slot = qcril_uim_remote_convert_instance_to_slot_type(instance_id);

  if (!mReady || req_data_ptr == nullptr) {
    if (mUimRemoteServiceClientImpl != nullptr) {
      mUimRemoteServiceClientImpl->sendUimRemoteClientApduResponse(
          token,
          static_cast<aidlimports::UimRemoteClientApduRsp>(QCRIL_UIM_REMOTE_CLIENT_APDU_FAILURE));
    }
    return;
  }

  QCRIL_LOG_INFO(
      "qcril_uim_remote_client_request_apdu: token :%d, apdu_status: %d", token, apdu_status);

  req_data_ptr->token = token;
  req_data_ptr->req_id = QMI_UIM_REMOTE_REQ_APDU_REQUEST;

  req_data_ptr->req_data.apdu_req.slot = slot;
  req_data_ptr->req_data.apdu_req.apdu_id = mApduId;
  req_data_ptr->req_data.apdu_req.apdu_status = static_cast<qmi_uim_remote_result>(apdu_status);

  if (apdu_data_len > 0 && apdu_data_len <= QMI_UIM_REMOTE_MAX_RESPONSE_APDU_SEGMENT_LEN &&
      apdu_ptr != nullptr) {
    req_data_ptr->req_data.apdu_req.response_apdu_info_valid = 1;
    req_data_ptr->req_data.apdu_req.response_apdu_info.total_response_apdu_size = apdu_data_len;
    req_data_ptr->req_data.apdu_req.response_apdu_info.response_apdu_segment_offset = 0;

    memcpy(req_data_ptr->req_data.apdu_req.response_apdu_segment, apdu_ptr, apdu_data_len);
    req_data_ptr->req_data.apdu_req.response_apdu_segment_len = apdu_data_len;
  }

  req_ptr = std::make_shared<UimRmtQmiUimRemoteReqMsg>(req_data_ptr, &req_cb);

  if (req_ptr != nullptr) {
    req_ptr->dispatch();
  } else {
    if (mUimRemoteServiceClientImpl != nullptr) {
      mUimRemoteServiceClientImpl->sendUimRemoteClientApduResponse(
          token,
          static_cast<aidlimports::UimRemoteClientApduRsp>(QCRIL_UIM_REMOTE_CLIENT_APDU_FAILURE));
    }
  }
} /* UimRemoteClientModule::qcril_uim_remote_client_request_apdu */
