/******************************************************************************
#  @file    MbnModule.h
#  @brief   Header file for MbnModule Class. Provides interface to read and update
#           mbn.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/PdcModemEndPoint.h"
#include "modules/nas/NasFlexMapStatusIndMessage.h"

#include "modules/mbn/qcril_qmi_pdc.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "modules/mbn/qcril_mbn_hw_update.h"
#include "modules/mbn/qcril_mbn_sw_update.h"
#include "SsrIndicationMessage.h"
#include "interfaces/nas/QcRilGetMaxModemSubscriptionsSyncMessage.h"
#include <framework/AddPendingMessageList.h>
#include <framework/QcrilInitMessage.h>
#include "interfaces/mbn/QcRilDeactivateSwMbnsRequestMessage.h"

//TODO add dmsendpoint

class MbnModule : public Module, public AddPendingMessageList {
public:
    static constexpr const int QCRIL_MBN_SW_MAX_INSTANCE_ID = 4;

protected:
    int mMaxApssInstanceId = 1;
    string mSwConfigId;
    string mSwActiveConfigId;
    int mSwMbnCurInstanceId;
    int mSwMbnCurSubId;
    bool mSwMbnNeedRestart;
    //TODO add remaining global var/structures
    bool mReady;
    bool mWaitingForDmsClient;
    bool mDmsEndPointStatus;

public:
    MbnModule();
    ~MbnModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalPdcModuleCleanup();
#endif
protected:
    void handlePdcQmiIndMessage(std::shared_ptr<Message> msg);
    void handleRilInit(std::shared_ptr<QcrilInitMessage> msg);
    void handlePdcEndpointStatusIndMessage(std::shared_ptr<Message> msg);
    void handleFlexMapStatusIndication(std::shared_ptr<Message> msg);
    void handleFileObserverEvents(std::shared_ptr<Message> msg);
    void handleSetMbnConfig(std::shared_ptr<QcRilRequestSetMbnConfigMessage> msg);
    void handleGetMbnConfig(std::shared_ptr<QcRilRequestGetMbnConfigMessage> msg);
    void handleGetAvailableMbnConfig(std::shared_ptr<QcRilRequestGetAvlMbnConfigMessage> msg);
    void handleCleanupMbnConfig(std::shared_ptr<QcRilRequestCleanupMbnConfigMessage> msg);
    void handleSelectMbnConfig(std::shared_ptr<QcRilRequestSelectMbnConfigMessage> msg);
    void handleGetMetaInfo(std::shared_ptr<QcRilRequestGetMetaInfoMessage> msg);
    void handleDeactivateMbnConfig(std::shared_ptr<QcRilRequestDeactivateMbnConfigMessage> msg);
    void handleGetQcVersionOfFile(std::shared_ptr<QcRilRequestGetQcVersionOfFileMessage> msg);
    void handleValidateMbnConfig(std::shared_ptr<QcRilRequestValidateMbnConfigMessage> msg);
    void handleGetQcVersionOfConfig(std::shared_ptr<QcRilRequestGetQcVersionOfConfigMessage> msg);
    void handleGetOemVersionOfFile(std::shared_ptr<QcRilRequestGetOemVersionOfFileMessage> msg);
    void handleGetOemVersionOfConfig(std::shared_ptr<QcRilRequestGetOemVersionOfConfigMessage> msg);
    void handleActivateMbnConfig(std::shared_ptr<QcRilRequestActivateMbnConfigMessage> msg);
    void handleEnableModemUpdate(std::shared_ptr<QcRilRequestEnableModemUpdateMessage> msg);
    void handleSetModemsConfig(std::shared_ptr<QcRilRequestSetModemsConfigMessage> msg);
    void handleGetModemsConfig(std::shared_ptr<QcRilRequestGetModemsConfigMessage> msg);
    void verifySwMbnUpdateHelper(std::shared_ptr<Message> msg);
    void handleSsrIndicationMessage(std::shared_ptr<SsrIndicationMessage> msg);
    qcril_mbn_sw_modem_switch_state processFlexMapStatus(NasFlexmapState state);
    void handleDeactivateSwMbnConfigs(std::shared_ptr<QcRilDeactivateSwMbnsRequestMessage> msg);

    // Handler for DMS Endpoint Status Indications
    void handleDmsEndpointStatusIndMessage(std::shared_ptr<Message> msg);

};

MbnModule& getMbnModule();
