/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include <framework/AddPendingMessageList.h>
#include <framework/QcrilInitMessage.h>
#include <interfaces/voice/QcrilVoiceClientConnected.h>
#include <interfaces/uim/QcrilSimClientConnected.h>
#include "modules/qmi/PbmModemEndPoint.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include "modules/android/ClientConnectedMessage.h"
#include "modules/pbm/NetworkDetectedEccNumberInfoMessage.h"
#include "modules/pbm/GetNetworkDetectedEccNumberInfoMessage.h"
#include "modules/uim/UimCardStatusIndMsg.h"
#include "qcril_pbm.h"

class PbmModule : public Module, public AddPendingMessageList {
  public:
    PbmModule();
    ~PbmModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalPbmModuleCleanup();
#endif

  private:
    bool mReady = false;
    void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
    void handlePbmEndpointStatusIndMessage(std::shared_ptr<Message> msg);
    void handlePbmQmiIndMessage(std::shared_ptr<Message> msg);
    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
    void handleGetAdnRecord(std::shared_ptr<QcRilRequestGetAdnRecordMessage> msg);
    void handleUpdateAdnRecord(std::shared_ptr<QcRilRequestUpdateAdnRecordMessage> msg);
    void handleUpdateEccListMessage(std::shared_ptr<PbmFetchEccListMessage> msg);
    void handleNetworkDetectedEccNumberInfoMessage(
            std::shared_ptr<NetworkDetectedEccNumberInfoMessage> msg);
    void handleGetNetworkDetectedEccNumberInfoMessage(
            std::shared_ptr<GetNetworkDetectedEccNumberInfoMessage> msg);
    void handleCheckNumIsEmergency(std::shared_ptr<RilNumberIsEmergency> msg);
    void handleSendEccListInd(std::shared_ptr<RilPbmSendEccListInd> msg);
    void handleEnabEmerNumInd(std::shared_ptr<RilPbmEnabEmerNumInd> msg);
    void handleCheckIfNumFromPbm(std::shared_ptr<RilEmerNumFromPbm> msg);
    void handlePbmSupportsEcc(std::shared_ptr<RilPbmSupportEcc> msg);
    void handlePbmFillEccMap(std::shared_ptr<RilPbmFillEccMap> msg);
    void handleQcrilVoiceClientConnected(std::shared_ptr<QcrilVoiceClientConnected> msg);
    void handleQcrilSimClientConnected(std::shared_ptr<QcrilSimClientConnected> msg);
    void handleGetAdnCapacity(std::shared_ptr<QcRilRequestGetAdnCapacityMessage> msg);
    void handleGetSimPhonebookRecords(std::shared_ptr<QcRilRequestGetPhonebookRecordsMessage> msg);
    void handleUpdatePhonebookRecords(std::shared_ptr<QcRilRequestUpdatePhonebookRecordsMessage> msg);
    void handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg);
};

std::shared_ptr<PbmModule> getPbmModule();
