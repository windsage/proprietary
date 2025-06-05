/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/Module.h>
#include <framework/Message.h>
#include <framework/QcrilInitMessage.h>
#include <framework/QcrilImsClientConnected.h>
#include <framework/AddPendingMessageList.h>
#include <modules/qmi/QmiAsyncResponseMessage.h>
#include "modules/sms/SmsImsServiceStatusInd.h"
#include "modules/dms/DeviceImsCapabilityInd.h"
#include <interfaces/ims/QcRilRequestImsGetRegStateMessage.h>
#include <interfaces/ims/QcRilRequestImsSetMediaInfoMessage.h>
#include <interfaces/ims/QcRilRequestImsRegistrationChangeMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>

#include <interfaces/ims/QcRilRequestImsQueryMultiSimVoiceCapability.h>
#include <interfaces/ims/QcRilRequestImsGetRtpStatsMessage.h>
#include <interfaces/ims/QcRilRequestImsGetRtpErrorStatsMessage.h>
#include <interfaces/ims/QcRilRequestImsGetSubConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsSendRttMessage.h>
#include <interfaces/ims/QcRilRequestImsGeoLocationInfoMessage.h>
#include <interfaces/ims/QcRilRequestImsSetServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsSetConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsGetConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsRegisterMultiIdentityMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryVirtualLineInfo.h>
#include <interfaces/ims/QcRilRequestImsExitSmsCallbackModeMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryCIWLANConfigSyncMessage.h>
#include <interfaces/ims/QcRilRequestImsFeatureSupportedSyncMessage.h>

class ImsModule : public Module, public AddPendingMessageList {
  public:
    ImsModule();
    ~ImsModule();

    void init();

    bool isReady();
#ifdef QMI_RIL_UTF
    void qcrilHalImsModuleCleanup();
#endif

  private:
    bool mReady = false;
    bool mRegisterForGeoLocReq = false;
    uint8_t mModemEndPointReadyMask = 0x00;
    SmsImsServiceStatusInd::SmsImsServiceStatusInfo sSmsImsServiceStatus;

    void initImsa(qcril_instance_id_e_type id);
    void handleImsaQmiIndMessage(std::shared_ptr<Message> msg);
    void handleImsaEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void initImss(qcril_instance_id_e_type id);
    void handleImssQmiIndMessage(std::shared_ptr<Message> msg);
    void handleImssEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void handleSmsImsServiceStatusInd(std::shared_ptr<SmsImsServiceStatusInd> msg);
    void handleDeviceImsCapabilityInd(std::shared_ptr<DeviceImsCapabilityInd> msg);

    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);

    void broadcastReady();
    void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
    void handleQcrilImsClientConnected(std::shared_ptr<QcrilImsClientConnected> msg);

    void handleQcRilRequestImsGetRegStateMessage(
        std::shared_ptr<QcRilRequestImsGetRegStateMessage> msg);
    void handleQcRilRequestImsRegistrationChangeMessage(
        std::shared_ptr<QcRilRequestImsRegistrationChangeMessage> msg);
    void handleQcRilRequestImsQueryServiceStatusMessage(
        std::shared_ptr<QcRilRequestImsQueryServiceStatusMessage> msg);

    void handleQcRilRequestImsGetRtpStatsMessage(
        std::shared_ptr<QcRilRequestImsGetRtpStatsMessage> msg);
    void handleQcRilRequestImsGetRtpErrorStatsMessage(
        std::shared_ptr<QcRilRequestImsGetRtpErrorStatsMessage> msg);
    void handleQcRilRequestImsGetSubConfigMessage(
        std::shared_ptr<QcRilRequestImsGetSubConfigMessage> msg);
    void handleQcRilRequestImsSetServiceStatusMessage(
        std::shared_ptr<QcRilRequestImsSetServiceStatusMessage> msg);
    void handleQcRilRequestImsSendRttMessage(
        std::shared_ptr<QcRilRequestImsSendRttMessage> msg);
    void handleQcRilRequestImsGeoLocationInfoMessage(
        std::shared_ptr<QcRilRequestImsGeoLocationInfoMessage> msg);
    void handleQcRilRequestImsSetConfigMessage(
        std::shared_ptr<QcRilRequestImsSetConfigMessage> msg);
    void handleQcRilRequestImsGetConfigMessage(
        std::shared_ptr<QcRilRequestImsGetConfigMessage> msg);
    void handleQcRilRequestImsRegisterMultiLineMessage(
        std::shared_ptr<QcRilRequestImsRegisterMultiIdentityMessage> msg);
    void handleQcRilRequestImsQueryLineInfoMessage(
        std::shared_ptr<QcRilRequestImsQueryVirtualLineInfo> msg);
    void handleQcRilRequestImsSetMediaInfoMessage(
        std::shared_ptr<QcRilRequestImsSetMediaInfoMessage> msg);
    void handleQcRilRequestImsQueryMultiSimVoiceCapability(
        std::shared_ptr<QcRilRequestImsQueryMultiSimVoiceCapability> msg);
    void handleQcRilRequestImsExitSmsCallbackMode(
        std::shared_ptr<QcRilRequestImsExitSmsCallbackModeMessage> msg);
    void handleQcRilRequestImsQueryCiWlanConfig(
        std::shared_ptr<QcRilRequestImsQueryCIWLANConfigSyncMessage> msg);
    void handleQcRilRequestImsFeatureSupported(
        std::shared_ptr<QcRilRequestImsFeatureSupportedSyncMessage> msg);

    bool isImsaModemEndpointReady();
    void setImsaModemEndpointReady();
    void setImsaModemEndpointNotReady();
    bool isImssModemEndpointReady();
    void setImssModemEndpointReady();
    void setImssModemEndpointNotReady();
    bool isAllModemEndpointsReady();
};

std::shared_ptr<ImsModule> getImsModule();

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_ims_module_cleanup();
#endif
