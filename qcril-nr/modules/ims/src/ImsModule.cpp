/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILQ"

#include <framework/Log.h>
#include <framework/Module.h>
#include <framework/ModuleLooper.h>
#include <framework/QcrilInitMessage.h>
#include "modules/qmi/ImsaModemEndPoint.h"
#include "modules/qmi/ImssModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "qtiril-loadable/QtiRilLoadable.h"

#include "ImsModule.h"
#include "ImsInitMessage.h"

#include "qcril_qmi_imss.h"
#include "qcril_qmi_imss_v02.h"
#include "qcril_qmi_imsa.h"

// Used for mModemEndPointReadyMask
#define IMSA_MODEM_END_POINT 0x01
#define IMSS_MODEM_END_POINT 0x02

DECLARE_MSG_ID_INFO(IMSA_QMI_IND);
DECLARE_MSG_ID_INFO(IMSS_QMI_IND);
DECLARE_MSG_ID_INFO(IMSA_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(IMSS_ENDPOINT_STATUS_IND);

ImsModule::ImsModule() : AddPendingMessageList("ImsModule") {
  mName = "ImsModule";
  mLooper = nullptr;

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(QcrilInitMessage, ImsModule::handleQcrilInitMessage),
      HANDLER(QcrilImsClientConnected, ImsModule::handleQcrilImsClientConnected),
      HANDLER_MULTI(QmiIndMessage, IMSA_QMI_IND, ImsModule::handleImsaQmiIndMessage),
      HANDLER_MULTI(QmiIndMessage, IMSS_QMI_IND, ImsModule::handleImssQmiIndMessage),
      // QMI Async response
      HANDLER(QmiAsyncResponseMessage, ImsModule::handleQmiAsyncRespMessage),
      // End Point Status Indication
      HANDLER_MULTI(EndpointStatusIndMessage, IMSA_ENDPOINT_STATUS_IND, ImsModule::handleImsaEndpointStatusIndMessage),
      HANDLER_MULTI(EndpointStatusIndMessage, IMSS_ENDPOINT_STATUS_IND, ImsModule::handleImssEndpointStatusIndMessage),
      HANDLER(DeviceImsCapabilityInd, ImsModule::handleDeviceImsCapabilityInd),
      // SMS handling
      HANDLER(SmsImsServiceStatusInd, ImsModule::handleSmsImsServiceStatusInd),

      // Qcril Interface Requests
      HANDLER(QcRilRequestImsGetRegStateMessage,
              ImsModule::handleQcRilRequestImsGetRegStateMessage),
      HANDLER(QcRilRequestImsRegistrationChangeMessage,
              ImsModule::handleQcRilRequestImsRegistrationChangeMessage),
      HANDLER(QcRilRequestImsQueryServiceStatusMessage,
              ImsModule::handleQcRilRequestImsQueryServiceStatusMessage),

      HANDLER(QcRilRequestImsGetRtpStatsMessage,
          ImsModule::handleQcRilRequestImsGetRtpStatsMessage),
      HANDLER(QcRilRequestImsGetRtpErrorStatsMessage,
          ImsModule::handleQcRilRequestImsGetRtpErrorStatsMessage),
      HANDLER(QcRilRequestImsGetSubConfigMessage,
          ImsModule::handleQcRilRequestImsGetSubConfigMessage),
      HANDLER(QcRilRequestImsSetServiceStatusMessage,
          ImsModule::handleQcRilRequestImsSetServiceStatusMessage),
      HANDLER(QcRilRequestImsSendRttMessage,
          ImsModule::handleQcRilRequestImsSendRttMessage),
      HANDLER(QcRilRequestImsGeoLocationInfoMessage,
          ImsModule::handleQcRilRequestImsGeoLocationInfoMessage),
      HANDLER(QcRilRequestImsSetConfigMessage,
          ImsModule::handleQcRilRequestImsSetConfigMessage),
      HANDLER(QcRilRequestImsGetConfigMessage,
          ImsModule::handleQcRilRequestImsGetConfigMessage),
      HANDLER(QcRilRequestImsRegisterMultiIdentityMessage,
          ImsModule::handleQcRilRequestImsRegisterMultiLineMessage),
      HANDLER(QcRilRequestImsQueryVirtualLineInfo,
          ImsModule::handleQcRilRequestImsQueryLineInfoMessage),
      HANDLER(QcRilRequestImsSetMediaInfoMessage,
          ImsModule::handleQcRilRequestImsSetMediaInfoMessage),
      HANDLER(QcRilRequestImsQueryMultiSimVoiceCapability,
          ImsModule::handleQcRilRequestImsQueryMultiSimVoiceCapability),
      HANDLER(QcRilRequestImsExitSmsCallbackModeMessage,
          ImsModule::handleQcRilRequestImsExitSmsCallbackMode),
      HANDLER(QcRilRequestImsQueryCIWLANConfigSyncMessage,
          ImsModule::handleQcRilRequestImsQueryCiWlanConfig),
      HANDLER(QcRilRequestImsFeatureSupportedSyncMessage,
          ImsModule::handleQcRilRequestImsFeatureSupported),
  };
  ModemEndPointFactory<ImsaModemEndPoint>::getInstance().buildEndPoint();
  ModemEndPointFactory<ImssModemEndPoint>::getInstance().buildEndPoint();
}

ImsModule::~ImsModule() {
  mReady = false;
}

void ImsModule::init() {
  Module::init();

  // Initializations complete.
  mReady = false;
}


void ImsModule::initImsa(qcril_instance_id_e_type id) {
  Log::getInstance().d("[" + mName + "]: initialize IMSA Modem Endpoint module");
  /* Init QMI IMSA services.*/
  QmiSetupRequestCallback qmiImsaSetupCallback("ImsModule-Token");
  ModemEndPointFactory<ImsaModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
      "ImsModule-Token", id, &qmiImsaSetupCallback);
}

void ImsModule::initImss(qcril_instance_id_e_type id) {
  Log::getInstance().d("[" + mName + "]: initialize IMSS Modem Endpoint module");
  QmiSetupRequestCallback qmiImssSetupCallback("ImssModule-Token");
  ModemEndPointFactory<ImssModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
      "ImssModule-Token", id, &qmiImssSetupCallback);
}

bool ImsModule::isReady() {
  return mReady;
}

void ImsModule::broadcastReady() {
  Log::getInstance().d("[" + mName + "]: broadcastReady");
  std::shared_ptr<ImsInitMessage> imsInitMsg = std::make_shared<ImsInitMessage>();
  if(imsInitMsg) {
    imsInitMsg->broadcast();
  }
}

bool ImsModule::isImsaModemEndpointReady() {
  bool isReady = false;
  if (mModemEndPointReadyMask & IMSA_MODEM_END_POINT) {
    isReady = true;
  }
  return isReady;
}

void ImsModule::setImsaModemEndpointReady() {
  mModemEndPointReadyMask |= IMSA_MODEM_END_POINT;
}

void ImsModule::setImsaModemEndpointNotReady() {
  mModemEndPointReadyMask &= ~IMSA_MODEM_END_POINT;
}

bool ImsModule::isImssModemEndpointReady() {
  bool isReady = false;
  if (mModemEndPointReadyMask & IMSS_MODEM_END_POINT) {
    isReady = true;
  }
  return isReady;
}

void ImsModule::setImssModemEndpointReady() {
  mModemEndPointReadyMask |= IMSS_MODEM_END_POINT;
}

void ImsModule::setImssModemEndpointNotReady() {
  mModemEndPointReadyMask &= ~IMSS_MODEM_END_POINT;
}

bool ImsModule::isAllModemEndpointsReady() {
  bool isReady = false;
  uint8_t servicesNeeded = (IMSA_MODEM_END_POINT|IMSS_MODEM_END_POINT);

  Log::getInstance().d("[" + mName + "]: mModemEndPointReadyMask = " +
      std::to_string(mModemEndPointReadyMask));

  if ((mModemEndPointReadyMask & servicesNeeded) == servicesNeeded) {
    isReady = true;
  }

  return isReady;
}

void ImsModule::handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_imss_pre_init();
  qcril_qmi_imsa_pre_init();
  auto id = msg->get_instance_id();
  initImsa(id);
  initImss(id);
}

void ImsModule::handleQcrilImsClientConnected(std::shared_ptr<QcrilImsClientConnected> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImssModemEndpointReady()) {
    qcril_qmi_ims_send_unsol_wfc_roaming_config();
  }
  if (!mRegisterForGeoLocReq && isImsaModemEndpointReady()) {
    qcril_qmi_imsa_register_for_geo_location_request();
  }
  mRegisterForGeoLocReq = true;
}

void ImsModule::handleImsaEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (!isImsaModemEndpointReady()) {
      qcril_qmi_imsa_init();
      setImsaModemEndpointReady();
      if (mRegisterForGeoLocReq) {
        qcril_qmi_imsa_register_for_geo_location_request();
      }
    }
    if (isAllModemEndpointsReady()) {
      mReady = true;
      broadcastReady();
    }
  } else {
    // state is not operational
    setImsaModemEndpointNotReady();
    mReady = false;
    if (!isAllModemEndpointsReady()) {
      clearPendingMessageList();
    }
    // clean up
    qcril_qmi_imsa_cleanup();
  }
}

void ImsModule::handleImssEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (!isImssModemEndpointReady()) {
      qcril_qmi_imss_init();
      setImssModemEndpointReady();
    }
    if (isAllModemEndpointsReady()) {
      mReady = true;
      broadcastReady();
    }
  } else {
    // state is not operational
    setImssModemEndpointNotReady();
    mReady = false;
    if (!isAllModemEndpointsReady()) {
      clearPendingMessageList();
    }
    // clean up
    qcril_qmi_imss_cleanup();
  }
}

void ImsModule::handleImsaQmiIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_imsa_unsol_ind_cb_helper(indData->msgId, indData->indData, indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void ImsModule::handleImssQmiIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_imss_unsol_ind_cb_helper_v02(indData->msgId, indData->indData, indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void ImsModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void ImsModule::handleQcRilRequestImsGetRegStateMessage(
    std::shared_ptr<QcRilRequestImsGetRegStateMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImsaModemEndpointReady()) {
    qcril_qmi_imsa_request_ims_registration_state(msg);
  } else {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
}

void ImsModule::handleQcRilRequestImsQueryServiceStatusMessage(
    std::shared_ptr<QcRilRequestImsQueryServiceStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImsaModemEndpointReady()) {
    qcril_qmi_imsa_request_query_ims_srv_status(msg);
  } else {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
}

void ImsModule::handleQcRilRequestImsRegistrationChangeMessage(
    std::shared_ptr<QcRilRequestImsRegistrationChangeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_imss_request_set_ims_registration_v02(msg);
}

void ImsModule::handleQcRilRequestImsGetRtpStatsMessage(
    std::shared_ptr<QcRilRequestImsGetRtpStatsMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImsaModemEndpointReady()) {
    qcril_qmi_imsa_request_get_rtp_statistics(msg);
  } else {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
}

void ImsModule::handleQcRilRequestImsGetRtpErrorStatsMessage(
    std::shared_ptr<QcRilRequestImsGetRtpErrorStatsMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImsaModemEndpointReady()) {
    qcril_qmi_imsa_request_get_rtp_error_statistics(msg);
  } else {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
}

void ImsModule::handleQcRilRequestImsGetSubConfigMessage(
    std::shared_ptr<QcRilRequestImsGetSubConfigMessage> msg) {

    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imsa_request_get_ims_sub_config(msg);
}

void ImsModule::handleQcRilRequestImsSendRttMessage(
    std::shared_ptr<QcRilRequestImsSendRttMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imss_request_send_rtt_msg(msg);
}

void ImsModule::handleQcRilRequestImsGeoLocationInfoMessage(
    std::shared_ptr<QcRilRequestImsGeoLocationInfoMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imsa_set_geo_loc(msg);
}

void ImsModule::handleQcRilRequestImsSetServiceStatusMessage(
    std::shared_ptr<QcRilRequestImsSetServiceStatusMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
    qcril_qmi_imss_request_set_ims_srv_status_v02(msg);
}

void ImsModule::handleQcRilRequestImsSetConfigMessage(
    std::shared_ptr<QcRilRequestImsSetConfigMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imss_request_set_ims_config_v02(msg);
}

void ImsModule::handleQcRilRequestImsGetConfigMessage(
    std::shared_ptr<QcRilRequestImsGetConfigMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imss_request_get_ims_config_v02(msg);
}

void ImsModule::handleQcRilRequestImsRegisterMultiLineMessage(
    std::shared_ptr<QcRilRequestImsRegisterMultiIdentityMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imss_request_register_multi_identity_lines(msg);
}

void ImsModule::handleQcRilRequestImsQueryMultiSimVoiceCapability(
    std::shared_ptr<QcRilRequestImsQueryMultiSimVoiceCapability> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_imss_request_get_dual_sim_voice_capability(msg);
}

void ImsModule::handleQcRilRequestImsQueryCiWlanConfig(
    std::shared_ptr<QcRilRequestImsQueryCIWLANConfigSyncMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_imss_request_get_c_iwlan_config(msg);
}

void ImsModule::handleQcRilRequestImsFeatureSupported(
    std::shared_ptr<QcRilRequestImsFeatureSupportedSyncMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  bool isFeatureSupported = qcril_qmi_imss_request_ims_feature_supported(msg);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
      std::make_shared<bool>(isFeatureSupported));
}

void ImsModule::handleQcRilRequestImsQueryLineInfoMessage(
    std::shared_ptr<QcRilRequestImsQueryVirtualLineInfo> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (isImsaModemEndpointReady()) {
    qcril_qmi_imsa_request_query_virtual_line_info(msg);
  } else {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
}

void ImsModule::handleSmsImsServiceStatusInd(std::shared_ptr<SmsImsServiceStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  sSmsImsServiceStatus = msg->getSmsImsServiceStatusInfo();
  qcril_qmi_imsa_sms_ims_service_status_hdlr(sSmsImsServiceStatus.sms_ims_service_status);
}

void ImsModule::handleDeviceImsCapabilityInd(std::shared_ptr<DeviceImsCapabilityInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto deviceImsCap = msg->getDeviceImsCapability();
  qcril_qmi_imsa_ims_capability_ind_hdlr(deviceImsCap);
}

void ImsModule::handleQcRilRequestImsSetMediaInfoMessage(
        std::shared_ptr<QcRilRequestImsSetMediaInfoMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_imss_request_set_ims_media_config(msg);
}

void ImsModule::handleQcRilRequestImsExitSmsCallbackMode(
    std::shared_ptr<QcRilRequestImsExitSmsCallbackModeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_imss_request_exit_sms_callback_mode(msg);
}

#if 0
static load_module<ImsModule> sImsModule;

ImsModule *getImsModule() {
    return &(sImsModule.get_module());
}
#endif

#ifndef QMI_RIL_UTF
static struct LoadableHeader *theHeader = nullptr;
static QtiSharedMutex theHeaderMutex;
extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader == nullptr) {
        auto theModule = std::make_shared<ImsModule>();
        if (theModule) {
            theModule->init();
        }
        auto header = new LoadableHeader{"ImsModule", std::static_pointer_cast<Module>(theModule)};
        theHeader = header;
    }
    return theHeader;
}

extern "C"
int loadable_deinit() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        delete theHeader;
    }
    return 0;
}

std::shared_ptr<ImsModule> getImsModule() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        return std::static_pointer_cast<ImsModule>(theHeader->theModule);
    } else {
        return nullptr;
    }
}
#else
static load_sharedptr_module<ImsModule> sImsModule;
std::shared_ptr<ImsModule> getImsModule() {
    return (sImsModule.get_module());
}

#endif

#ifdef QMI_RIL_UTF
void ImsModule::qcrilHalImsModuleCleanup()
{
    Log::getInstance().d("[" + mName + "]: qcrilHalImsModuleCleanup");
    std::shared_ptr<ImsaModemEndPoint> mImsaModemEndPoint =
                        ModemEndPointFactory<ImsaModemEndPoint>::getInstance().buildEndPoint();
    ImsaModemEndPointModule* mImsaModemEndPointModule =
                        (ImsaModemEndPointModule*)mImsaModemEndPoint->mModule;
    std::shared_ptr<ImssModemEndPoint> mImssModemEndPoint =
                        ModemEndPointFactory<ImssModemEndPoint>::getInstance().buildEndPoint();
    ImssModemEndPointModule* mImssModemEndPointModule =
                        (ImssModemEndPointModule*)mImssModemEndPoint->mModule;

    mImsaModemEndPointModule->cleanUpQmiSvcClient();
    mImssModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
    mModemEndPointReadyMask = 0x00;
    mRegisterForGeoLocReq = false;
    qcril_qmi_imsa_cleanup();
    qcril_qmi_imss_pre_init();
    qcril_qmi_imsa_pre_init();
}

void qcril_qmi_hal_ims_module_cleanup()
{
    auto module = getImsModule();
    module->qcrilHalImsModuleCleanup();
}
#endif
