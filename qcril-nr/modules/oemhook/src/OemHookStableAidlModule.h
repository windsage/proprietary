/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Log.h>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <modules/pbm/PbmModule.h>
#include <modules/mbn/MbnModule.h>
#include <interfaces/uim/UimSimlockTempUnlockExpireInd.h>
#include <interfaces/uim/UimCardStateChangeInd.h>
#include <interfaces/uim/UimSlotStatusInd.h>
#include <interfaces/uim/UimSimRefreshIndication.h>
#include <interfaces/uim/UimVoltageStatusInd.h>
#include <interfaces/sms/QcRilUnsolWmsReadyMessage.h>
#include "interfaces/dms/RilUnsolMaxActiveDataSubsChangedMessage.h"
#include "interfaces/nas/RilUnsolCsgIdChangedMessage.h"
#include "interfaces/nas/RilUnsolEngineerModeMessage.h"
#include <modules/uim_remote/UimRmtRemoteSimStatusIndMsg.h>
#include "stable_aidl_impl/qti_oem_hook_stable_aidl_service.h"
#include "interfaces/nas/RilUnsolOemNetworkScanMessage.h"
#include "interfaces/nas/RilUnsolSubProvisioningStatusMessage.h"

namespace aidlimplimports {
  using namespace aidl::vendor::qti::hardware::radio::qcrilhook::implementation;
}

class OemHookStableAidlModule: public Module {
    public:
        OemHookStableAidlModule();
        virtual void init() {
            Module::init();
        }
        void sendIndication(uint8_t* buf, size_t bufLen);
        void sendResponse(int serial, RIL_Errno errorCode, uint8_t* buf, size_t bufLen);

    private:
        std::shared_ptr<aidlimplimports::OemHookStable> mOemHookStable = nullptr;
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleAdnRecordsOnSimMessage(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
        void handleAdnInitDoneMessage(std::shared_ptr<QcRilUnsolAdnInitDoneMessage> msg);
        void handleMbnConfigResultMessage(std::shared_ptr<QcRilUnsolMbnConfigResultMessage> msg);
        void handleMbnConfigClearedMessage(std::shared_ptr<QcRilUnsolMbnConfigClearedMessage> msg);
        void handleMbnValidateDumpedMessage(std::shared_ptr<QcRilUnsolMbnValidateDumpedMessage> msg);
        void handleMbnConfigListMessage(std::shared_ptr<QcRilUnsolMbnConfigListMessage> msg);
        void handleMbnValidateConfigMessage(std::shared_ptr<QcRilUnsolMbnValidateConfigMessage> msg);
        void handleUimSimlockTempUnlockExpireInd(std::shared_ptr<UimSimlockTempUnlockExpireInd> msg);
        void handleUimCardStateChangeInd(std::shared_ptr<UimCardStateChangeInd> msg);
        void handleUimSlotStatusInd(std::shared_ptr<UimSlotStatusInd> msg);
        void handleUimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg);
        void handleUimVoltageStatusInd(std::shared_ptr<UimVoltageStatusInd> msg);
        void handleUimRmtRemoteSimStatusIndMsg(std::shared_ptr<UimRmtRemoteSimStatusIndMsg> msg);
        void handleQcRilUnsolDtmfMessage(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
        void handleQcRilUnsolExtBurstIntlMessage(std::shared_ptr<QcRilUnsolExtBurstIntlMessage> msg);
        void handleQcRilUnsolNssReleaseMessage(std::shared_ptr<QcRilUnsolNssReleaseMessage> msg);
        void handleQcRilUnsolSuppSvcErrorCodeMessage(std::shared_ptr<QcRilUnsolSuppSvcErrorCodeMessage> msg);
        void handleQcRilUnsolSpeechCodecInfoMessage(std::shared_ptr<QcRilUnsolSpeechCodecInfoMessage> msg);
        void handleQcRilUnsolAudioStateChangedMessage(std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg);
        void handleQcRilUnsolWmsReadyMessage(std::shared_ptr<QcRilUnsolWmsReadyMessage> msg);
        void handleRilUnsolMaxActiveDataSubsChangedMessage(
            std::shared_ptr<RilUnsolMaxActiveDataSubsChangedMessage> msg);
        void handleRilUnsolCsgIdChangedMessage(std::shared_ptr<RilUnsolCsgIdChangedMessage> msg);
        void handleRilUnsolEngineerModeMessage(std::shared_ptr<RilUnsolEngineerModeMessage> msg);
        void handleRilUnsolOemNetworkScanMessage(std::shared_ptr<RilUnsolOemNetworkScanMessage> msg);
        void handleRilUnsolSubProvisionStatusMessage(
            std::shared_ptr<RilUnsolSubProvisioningStatusMessage> msg);

#ifndef RIL_FOR_LOW_RAM
        void handleSaiListChangeIndMessage(std::shared_ptr<Message> msg);
        void handleTMGIListChangeIndMessage(std::shared_ptr<Message> msg);
        void handleTMGIListOOSWarningIndMessage(std::shared_ptr<Message> msg);
        void handleTMGIAvailableIndMessage(std::shared_ptr<Message> msg);
        void handleContentDescUpdateIndMessage(std::shared_ptr<Message> msg);
        void handleInterestedTMGIListIndMessage(std::shared_ptr<Message> msg);
#endif
};
