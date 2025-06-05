/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <framework/QcrilInitMessage.h>
#include <modules/android/ril_message_factory.h>
#include <modules/qmi/ModemEndPointFactory.h>
#include <modules/qmi/QmiSetupRequestCallback.h>
#include <modules/qmi/QmiIndMessage.h>
#include <modules/sms/SmsImsNetworkStateChangedInd.h>
#include <modules/sms/SmsTriggerPowerUpOptimization.h>
#include <modules/qmi/QmiSmsSetupRequestCallback.h>
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <framework/Message.h>
#include <framework/Module.h>
#include <framework/Log.h>
#include <framework/qcril_event.h>
#include <functional>
#include <framework/PolicyManager.h>
#include <framework/SingleDispatchRestriction.h>
#include <framework/RejectSimultaneousDispatchRestriction.h>
#include <qtiril-loadable/QtiRilLoadable.h>

#include "SmsModule.h"
#include "modules/sms/qcril_qmi_sms.h"

#undef TAG
#define TAG "SmsModule"

using std::placeholders::_1;

DECLARE_MSG_ID_INFO(SMS_QMI_IND);
DECLARE_MSG_ID_INFO(SMS_ENDPOINT_STATUS_IND);

#ifndef QMI_RIL_UTF
static struct LoadableHeader *theHeader = nullptr;
static QtiSharedMutex theHeaderMutex;
extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader == nullptr) {
        auto theModule = std::make_shared<SmsModule>();
        if (theModule) {
            theModule->init();
        }
        auto header = new LoadableHeader{"SmsModule", std::static_pointer_cast<Module>(theModule)};
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

std::shared_ptr<SmsModule> getSmsModule() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        return std::static_pointer_cast<SmsModule>(theHeader->theModule);
    } else {
        return nullptr;
    }
}
#else
static load_sharedptr_module<SmsModule> sms_module;
std::shared_ptr<SmsModule> getSmsModule() {
    return (sms_module.get_module());
}

#endif

SmsModule::SmsModule() : AddPendingMessageList("SmsModule") {
    mName = "SmsModule";
    ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint();

    mMessageHandler = {
        HANDLER(QcrilInitMessage, SmsModule::handleQcrilInit),
        HANDLER_MULTI(QmiIndMessage, SMS_QMI_IND, SmsModule::handleSmsQmiIndMessage),
        HANDLER_MULTI(EndpointStatusIndMessage, SMS_ENDPOINT_STATUS_IND, SmsModule::handleSmsEndPointStatusIndMessage),
        HANDLER(QmiAsyncResponseMessage, SmsModule::handleQmiAsyncRespMessage),
        HANDLER(RilRequestSendSmsMessage, SmsModule::handleSendSms),
        HANDLER(RilRequestAckGsmSmsMessage, SmsModule::handleSmsAcknowledge),
        HANDLER(RilRequestWriteSmsToSimMessage, SmsModule::handleWriteSmsToSim),
        HANDLER(RilRequestDeleteSmsOnSimMessage, SmsModule::handleDeleteSmsOnSim),
        HANDLER(RilRequestGetSmscAddressMessage, SmsModule::handleGetSmscAddress),
        HANDLER(RilRequestSetSmscAddressMessage, SmsModule::handleSetSmscAddress),
        HANDLER(RilRequestReportSmsMemoryStatusMessage, SmsModule::handleReportSmsMemoryStatus),
        HANDLER(RilRequestGetGsmBroadcastConfigMessage, SmsModule::handleGsmGetBroadcastSmsConfig),
        HANDLER(RilRequestGsmSetBroadcastSmsConfigMessage, SmsModule::handleGsmSetBroadcastSmsConfig),
        HANDLER(RilRequestGsmSmsBroadcastActivateMessage, SmsModule::handleGsmSmsBroadcastActivation),
        HANDLER(RilRequestCdmaSendSmsMessage, SmsModule::handleCdmaSendSms),
        HANDLER(RilRequestAckCdmaSmsMessage, SmsModule::handleCdmaSmsAcknowledge),
        HANDLER(RilRequestCdmaWriteSmsToRuimMessage, SmsModule::handleCdmaWriteSmsToRuim),
        HANDLER(RilRequestCdmaDeleteSmsOnRuimMessage, SmsModule::handleCdmaDeleteSmsOnRuim),
        HANDLER(RilRequestGetCdmaBroadcastConfigMessage, SmsModule::handleCdmaGetBroadcastSmsConfig),
        HANDLER(RilRequestCdmaSetBroadcastSmsConfigMessage, SmsModule::handleCdmaSetBroadcastSmsConfig),
        HANDLER(RilRequestCdmaSmsBroadcastActivateMessage, SmsModule::handleCdmaSmsBroadcastActivation),
        HANDLER(RilRequestGetImsRegistrationMessage, SmsModule::handleImsRegistrationState),
        HANDLER(RilRequestImsSendSmsMessage, SmsModule::handleImsSendSms),
        HANDLER(RilRequestAckImsSmsMessage, SmsModule::handleImsSmsAcknowledge),
        HANDLER(SmsGetImsServiceStatusMessage, SmsModule::handleSmsGetImsServiceStatusMessage),
        HANDLER(QcRilEventSmsRawReadMessage, SmsModule::handleSmsRawReadMessage),
        HANDLER(QcRilEventSmsQueryWmsReadyMessage, SmsModule::handleSmsQueryWmsReadyMessage),
        HANDLER(QcRilSyncQuerySmsFormatMessage, SmsModule::handleSmsQueryFormatMessage),
        HANDLER(QcRilEventSmsResetAckNeededForPowerOptMessage,
                SmsModule::handleSmsResetAckNeededForPowerOptMessage),
        HANDLER(NasPowerOnOptUiReadyIndMessage,
                SmsModule::handleNasPowerOnOptUiReadyIndMessage),
        HANDLER(QcRilUnsolOperationalStatusMessage,
                SmsModule::handleUnsolOperationalStatusMessage),
        HANDLER(QcRilUnsolImsRegStateMessage,
                SmsModule::handleUnsolImsRegStateMessage),
    };
}

SmsModule::~SmsModule() {}

void SmsModule::init() {
    Module::init();

    std::vector<std::string> ring{
      RilRequestCdmaSendSmsMessage::MESSAGE_NAME,
      RilRequestImsSendSmsMessage::MESSAGE_NAME,
      RilRequestSendSmsMessage::MESSAGE_NAME
    };
    PolicyManager::getInstance().setMessageRestriction(ring,
           std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestWriteSmsToSimMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestAckGsmSmsMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestAckCdmaSmsMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestAckImsSmsMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());
}

void SmsModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_sms_pre_init();
    // Init QMI services
    QmiSmsSetupRequestCallback qmiSmsSetupCallback("SmsModule-Token");
    ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
            "SmsModule-Token", msg->get_instance_id(), &qmiSmsSetupCallback);
}

void SmsModule::handleSmsQmiIndMessage(std::shared_ptr<Message> msg) {
    auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));

    Log::getInstance().d("[SmsModule]: Handling msg = " + msg->dump());
    QmiIndMsgDataStruct *indData = shared_indMsg->getData();
    if (indData != nullptr) {
        qcril_qmi_sms_unsolicited_indication_cb_helper(indData->msgId, indData->indData,
                                                       indData->indSize);
    } else {
        Log::getInstance().d("Unexpected, null data from message");
    }
}

void SmsModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void SmsModule::handleSmsEndPointStatusIndMessage(std::shared_ptr<Message> msg) {
    auto shared_msg(
        std::static_pointer_cast<EndpointStatusIndMessage>(msg));

    Log::getInstance().d("[SmsModule]: Handling msg = " + msg->dump());
    if (shared_msg->getState() == ModemEndPoint::State::OPERATIONAL) {
        if (!mReady) {
            qmi_ril_set_sms_svc_status( QMI_RIL_SMS_SVC_NOT_INITIALZIED );
            qmi_ril_set_primary_client_status( QMI_RIL_SMS_PRIMARY_CLIENT_NOT_SET );

            qmi_client_error_type client_err = qcril_qmi_sms_init();
            if (client_err)
            {
                Log::getInstance().d("qcril_qmi_sms_init returned failure:" + std::to_string(client_err));
            }

            client_err = qcril_sms_perform_initial_configuration();
            if (client_err != QMI_NO_ERR)
            {
                Log::getInstance().d("Failed to perform SMS initial configuration.");
            }

            mReady = true;
        }
    } else {
        mReady = false;
        qmi_ril_set_sms_svc_status( QMI_RIL_SMS_SVC_NOT_INITIALZIED );
        qmi_ril_set_primary_client_status( QMI_RIL_SMS_PRIMARY_CLIENT_NOT_SET );
        clearPendingMessageList();
    }
}

void SmsModule::handleSendSms(std::shared_ptr<RilRequestSendSmsMessage> msg) {
    qcril_sms_request_send_gw_sms(msg);
}

void SmsModule::handleSmsAcknowledge(std::shared_ptr<RilRequestAckGsmSmsMessage> msg) {
    qcril_sms_request_sms_acknowledge(msg);
}

void SmsModule::handleWriteSmsToSim(std::shared_ptr<RilRequestWriteSmsToSimMessage> msg) {
    qcril_sms_request_write_sms_to_sim(msg);
}

void SmsModule::handleDeleteSmsOnSim(std::shared_ptr<RilRequestDeleteSmsOnSimMessage> msg) {
    qcril_sms_request_delete_sms_on_sim(msg);
}

void SmsModule::handleGetSmscAddress(std::shared_ptr<RilRequestGetSmscAddressMessage> msg) {
    qcril_sms_request_get_smsc_address(msg);
}

void SmsModule::handleSetSmscAddress(std::shared_ptr<RilRequestSetSmscAddressMessage> msg) {
    qcril_sms_request_set_smsc_address(msg);
}

void SmsModule::handleReportSmsMemoryStatus(std::shared_ptr<RilRequestReportSmsMemoryStatusMessage> msg) {
    qcril_sms_request_report_sms_memory_status(msg);
}

void SmsModule::handleGsmGetBroadcastSmsConfig(std::shared_ptr<RilRequestGetGsmBroadcastConfigMessage> msg) {
    qcril_sms_request_gsm_get_broadcast_sms_config(msg);
}

void SmsModule::handleGsmSetBroadcastSmsConfig(std::shared_ptr<RilRequestGsmSetBroadcastSmsConfigMessage> msg) {
    qcril_sms_request_gsm_set_broadcast_sms_config(msg);
}

void SmsModule::handleGsmSmsBroadcastActivation(std::shared_ptr<RilRequestGsmSmsBroadcastActivateMessage> msg) {
    qcril_sms_request_gsm_sms_broadcast_activation(msg);
}

void SmsModule::handleCdmaSendSms(std::shared_ptr<RilRequestCdmaSendSmsMessage> msg) {
    qcril_sms_request_cdma_send_sms(msg);
}

void SmsModule::handleCdmaSmsAcknowledge(std::shared_ptr<RilRequestAckCdmaSmsMessage> msg) {
    qcril_sms_request_cdma_sms_acknowledge(msg);
}

void SmsModule::handleCdmaWriteSmsToRuim(std::shared_ptr<RilRequestCdmaWriteSmsToRuimMessage> msg) {
    qcril_sms_request_cdma_write_sms_to_ruim(msg);
}

void SmsModule::handleCdmaDeleteSmsOnRuim(std::shared_ptr<RilRequestCdmaDeleteSmsOnRuimMessage> msg) {
    qcril_sms_request_cdma_delete_sms_on_ruim(msg);
}

void SmsModule::handleCdmaGetBroadcastSmsConfig(std::shared_ptr<RilRequestGetCdmaBroadcastConfigMessage> msg) {
    qcril_sms_request_cdma_get_broadcast_sms_config(msg);
}

void SmsModule::handleCdmaSetBroadcastSmsConfig(std::shared_ptr<RilRequestCdmaSetBroadcastSmsConfigMessage> msg) {
    qcril_sms_request_cdma_set_broadcast_sms_config(msg);
}

void SmsModule::handleCdmaSmsBroadcastActivation(std::shared_ptr<RilRequestCdmaSmsBroadcastActivateMessage> msg) {
    qcril_sms_request_cdma_sms_broadcast_activation(msg);
}

void SmsModule::handleImsRegistrationState(std::shared_ptr<RilRequestGetImsRegistrationMessage> msg) {
    qcril_sms_request_ims_registration_state(msg);
}

void SmsModule::handleImsSendSms(std::shared_ptr<RilRequestImsSendSmsMessage> msg) {
    qcril_sms_request_ims_send_sms(msg);
}

void SmsModule::handleImsSmsAcknowledge(std::shared_ptr<RilRequestAckImsSmsMessage> msg) {
    qcril_sms_request_ims_ack_sms(msg);
}

void SmsModule::handleSmsGetImsServiceStatusMessage(std::shared_ptr<SmsGetImsServiceStatusMessage> msg) {

    QCRIL_LOG_FUNC_ENTRY();
    qmi_ril_sms_ims_reg_state ims_sms_state;
    RIL_Errno res = RIL_E_SUCCESS;
    SmsImsServiceStatusInd::SmsImsServiceStatusInfo smsImsServiceStatus;
    res = qcril_qmi_sms_retrieve_ims_sms_registration_state(ims_sms_state, smsImsServiceStatus);

    QCRIL_LOG_DEBUG("Received SmsGetImsServiceStatusMessage Msg. Executing handleSmsGetImsServiceStatusMessage  ");

    msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                               std::make_shared<SmsImsServiceStatusInd::SmsImsServiceStatusInfo>(smsImsServiceStatus));
    QCRIL_LOG_FUNC_RETURN();
}

void SmsModule::handleSmsRawReadMessage(std::shared_ptr<QcRilEventSmsRawReadMessage> msg) {
    qcril_sms_request_raw_read(msg);
}

void SmsModule::handleSmsQueryWmsReadyMessage(
        std::shared_ptr<QcRilEventSmsQueryWmsReadyMessage> /* msg */) {
    query_wms_ready_status();
}

void SmsModule::handleSmsQueryFormatMessage(std::shared_ptr<QcRilSyncQuerySmsFormatMessage> msg ) {
    auto format = qcril_qmi_sms_get_ims_sms_format();
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::shared_ptr<qmi_ril_sms_format_type>(
        new qmi_ril_sms_format_type{format}));
}

void SmsModule::handleSmsResetAckNeededForPowerOptMessage(
        std::shared_ptr<QcRilEventSmsResetAckNeededForPowerOptMessage> /* msg */) {
    qcril_qmi_sms_reset_mt_sms_ack_needed_power_opt_buffer();
}

void SmsModule::handleNasPowerOnOptUiReadyIndMessage(
        std::shared_ptr<NasPowerOnOptUiReadyIndMessage> /* msg */) {
    qcril_qmi_sms_notify_mt_sms_with_ack_needed_power_opt_to_atel();
    qcril_qmi_sms_notify_mt_sms_with_ack_not_needed_power_opt_to_atel();
}

void SmsModule::handleUnsolOperationalStatusMessage(
        std::shared_ptr<QcRilUnsolOperationalStatusMessage> msg) {
    qcril_qmi_sms_set_operational_status(msg->getOperationalState());
}

void SmsModule::handleUnsolImsRegStateMessage(
        std::shared_ptr<QcRilUnsolImsRegStateMessage> msg) {
    qcril_sms_update_ims_registration_state(msg);
}

#ifdef QMI_RIL_UTF
void SmsModule::qcrilHalSmsModuleCleanup()
{
    std::shared_ptr<SmsModemEndPoint> mSmsModemEndPoint =
                ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint();
    SmsModemEndPointModule* mSmsModemEndPointModule =
               (SmsModemEndPointModule*)mSmsModemEndPoint->mModule;
    mSmsModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif
