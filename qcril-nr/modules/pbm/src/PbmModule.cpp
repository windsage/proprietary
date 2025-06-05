/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include <framework/QcrilInitMessage.h>
#include "framework/Module.h"
#include "modules/pbm/PbmModule.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/PbmModemEndPoint.h"
#include "modules/qmi/QmiPbmSetupRequestCallback.h"
#include <qtibus/Messenger.h>
#include <qtiril-loadable/QtiRilLoadable.h>

DECLARE_MSG_ID_INFO(PBM_QMI_IND);
DECLARE_MSG_ID_INFO(PBM_ENDPOINT_STATUS_IND);
#ifndef QMI_RIL_UTF

static struct LoadableHeader *theHeader = nullptr;
static QtiSharedMutex theHeaderMutex;
extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader == nullptr) {
        auto theModule = std::make_shared<PbmModule>();
        if (theModule) {
            theModule->init();
        }
        auto header = new LoadableHeader{"PbmModule", std::static_pointer_cast<Module>(theModule)};
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

std::shared_ptr<PbmModule> getPbmModule() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        return std::static_pointer_cast<PbmModule>(theHeader->theModule);
    } else {
        return nullptr;
    }
}
#else
static load_sharedptr_module<PbmModule> pbm_module;
std::shared_ptr<PbmModule> getPbmModule() {
    return (pbm_module.get_module());
}

#endif

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
PbmModule::PbmModule() : AddPendingMessageList("PbmModule")  {
  mName = "PbmModule";
  ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint();

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage,PbmModule::handleQcrilInit),
    HANDLER_MULTI(QmiIndMessage, PBM_QMI_IND, PbmModule::handlePbmQmiIndMessage),
    // QMI Async response
    HANDLER(QmiAsyncResponseMessage, PbmModule::handleQmiAsyncRespMessage),
    HANDLER(QcRilRequestGetAdnRecordMessage, PbmModule::handleGetAdnRecord),
    HANDLER(QcRilRequestUpdateAdnRecordMessage, PbmModule::handleUpdateAdnRecord),
    HANDLER(PbmFetchEccListMessage, PbmModule::handleUpdateEccListMessage),
    HANDLER(NetworkDetectedEccNumberInfoMessage,
            PbmModule::handleNetworkDetectedEccNumberInfoMessage),
    HANDLER(GetNetworkDetectedEccNumberInfoMessage,
            PbmModule::handleGetNetworkDetectedEccNumberInfoMessage),
    HANDLER(RilNumberIsEmergency, PbmModule::handleCheckNumIsEmergency),
    HANDLER(RilEmerNumFromPbm, PbmModule::handleCheckIfNumFromPbm),
    HANDLER(RilPbmSendEccListInd, PbmModule::handleSendEccListInd),
    HANDLER(RilPbmEnabEmerNumInd, PbmModule::handleEnabEmerNumInd),
    HANDLER(RilPbmSupportEcc, PbmModule::handlePbmSupportsEcc),
    HANDLER(RilPbmFillEccMap, PbmModule::handlePbmFillEccMap),
    HANDLER(QcrilVoiceClientConnected, PbmModule::handleQcrilVoiceClientConnected),
    HANDLER(QcrilSimClientConnected, PbmModule::handleQcrilSimClientConnected),
    HANDLER(QcRilRequestGetAdnCapacityMessage, PbmModule::handleGetAdnCapacity),
    HANDLER(QcRilRequestGetPhonebookRecordsMessage, PbmModule::handleGetSimPhonebookRecords),
    HANDLER(QcRilRequestUpdatePhonebookRecordsMessage, PbmModule::handleUpdatePhonebookRecords),
    HANDLER(UimCardStatusIndMsg, PbmModule::handleUimCardStatusIndMsg),

    // End Point Status Indication
    HANDLER_MULTI(EndpointStatusIndMessage, PBM_ENDPOINT_STATUS_IND, PbmModule::handlePbmEndpointStatusIndMessage)
  };
}

/* Follow RAII.
*/
PbmModule::~PbmModule() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void PbmModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void PbmModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_pbm_pre_init();
  /* Init QMI services.*/
  QmiPbmSetupRequestCallback qmiPbmSetupCallback("PbmModule-Token");
  ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
        "PbmModule-Token", msg->get_instance_id(), &qmiPbmSetupCallback);

  Messenger::get().registerForMessage(
      NetworkDetectedEccNumberInfoMessage::get_class_message_id(),
      [](IPCIStream& is) -> std::shared_ptr<IPCMessage> {
        auto msg = std::make_shared<NetworkDetectedEccNumberInfoMessage>();
        if (msg) {
          msg->deserialize(is);
        }
        return msg;
      });
}

void PbmModule::handlePbmEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (!mReady) {
      mReady = true;
      qcril_qmi_pbm_init();
    }
  } else {
    // state is not operational
    mReady = false;
    // clean up
    qcril_qmi_pbm_cleanup();
  }
}

void PbmModule::handlePbmQmiIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_pbm_unsolicited_indication_cb_helper(indData->msgId, indData->indData,
        indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void PbmModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void PbmModule::handleGetAdnRecord(std::shared_ptr<QcRilRequestGetAdnRecordMessage> msg) {
    qcril_qmi_pbm_get_adn_record_hndlr(msg);
}

void PbmModule::handleGetAdnCapacity(std::shared_ptr<QcRilRequestGetAdnCapacityMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_get_all_pb_capacity_hndlr(msg);
}

void PbmModule::handleGetSimPhonebookRecords(std::shared_ptr<QcRilRequestGetPhonebookRecordsMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_get_all_adn_records_hndlr(msg);
}

void PbmModule::handleUpdateEccListMessage(std::shared_ptr<PbmFetchEccListMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_fetch_ecc_list_hndlr(msg);
}

void PbmModule::handleUpdateAdnRecord(std::shared_ptr<QcRilRequestUpdateAdnRecordMessage> msg) {
    qcril_qmi_pbm_update_adn_record_hndlr(msg);
}

void PbmModule::handleUpdatePhonebookRecords(std::shared_ptr<QcRilRequestUpdatePhonebookRecordsMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_update_phone_book_record_hndlr(msg);
}

void PbmModule::handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_uim_card_status_changed_hndlr(msg);
}

void PbmModule::handleSendEccListInd(std::shared_ptr<RilPbmSendEccListInd> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    qmi_ril_send_ecc_list_indication();
    auto resp = std::make_shared<bool>(true);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void PbmModule::handleNetworkDetectedEccNumberInfoMessage(
    std::shared_ptr<NetworkDetectedEccNumberInfoMessage> msg) {
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  if (msg->getAction() == NetworkDetectedEccNumberInfoMessage::Action::SET) {
    qmi_ril_set_network_detected_ecc_number_info(msg->getNumber(), msg->getEmergencyCategory(),
                                                 msg->getEmergencyServiceUrn());
  } else if (msg->getAction() == NetworkDetectedEccNumberInfoMessage::Action::RESET) {
    qmi_ril_reset_network_detected_ecc_number_info();
  }
}

void PbmModule::handleGetNetworkDetectedEccNumberInfoMessage(
    std::shared_ptr<GetNetworkDetectedEccNumberInfoMessage> msg) {
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  qcril_qmi_pbm_get_network_detected_ecc_number(msg);
}

void PbmModule::handleCheckNumIsEmergency(std::shared_ptr<RilNumberIsEmergency> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    auto is_emer = qmi_ril_phone_number_is_emergency(msg->getNumber());
    auto resp = std::make_shared<bool>(is_emer);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void PbmModule::handleCheckIfNumFromPbm(std::shared_ptr<RilEmerNumFromPbm> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    auto is_from_pbm = qmi_ril_number_received_from_pbm(msg->getNumber().c_str());
    auto resp = std::make_shared<bool>(is_from_pbm);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void PbmModule::handlePbmSupportsEcc(std::shared_ptr<RilPbmSupportEcc> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
	auto supports_ecc = qcril_qmi_pbm_centralized_ecc_support_available();
    auto resp = std::make_shared<bool>(supports_ecc);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void PbmModule::handleQcrilVoiceClientConnected(std::shared_ptr<QcrilVoiceClientConnected> msg) {
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  qmi_ril_send_ecc_list_indication();
}

void PbmModule::handleQcrilSimClientConnected(std::shared_ptr<QcrilSimClientConnected> msg) {
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  qcril_qmi_pbm_ril_connected_hndlr();
}

void PbmModule::handleEnabEmerNumInd(std::shared_ptr<RilPbmEnabEmerNumInd> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    qcril_qmi_pbm_enable_emergency_number_indications(msg->getStatus());
    auto resp = std::make_shared<bool>(true);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void PbmModule::handlePbmFillEccMap(std::shared_ptr<RilPbmFillEccMap> msg) {
    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    qmi_ril_fill_ecc_map(msg->getEmergencyNums(),*msg->getEccMap(),
            msg->getSources(), msg->getMcc(), msg->getMnc());
    auto resp = std::make_shared<bool>(true);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

#ifdef QMI_RIL_UTF
void PbmModule::qcrilHalPbmModuleCleanup()
{
    std::shared_ptr<PbmModemEndPoint> mPbmModemEndPoint =
                    ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint();
    PbmModemEndPointModule* mPbmModemEndPointModule =
                   (PbmModemEndPointModule*)mPbmModemEndPoint->mModule;
    mPbmModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif
