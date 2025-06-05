/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

// Framework headers
#include <framework/Module.h>
#include <framework/TimeKeeper.h>
#include <framework/add_message_id.h>
#include <framework/SolicitedMessage.h>
#include <framework/QcrilInitMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/AddPendingMessageList.h>

// Module headers
#include "telephony/ril.h"
#include <modules/android/RilRequestMessage.h>

#include <modules/secure_mode/RegisterSecureModeIndicationMessage.h>
#include <interfaces/securemode/RilRequestGetSecureModeStatusMessage.h>

// Peripheral Control Service Headers
#include "IPeripheralState.h"
#include "CPeripheralAccessControl.h"
#include "peripheralStateUtils.h"

#define PERIPHERAL_CONTROL_SERVICE_LIB "libPeripheralStateUtils.so"

typedef void* (*registerPeripheralCBFnPtr)(uint32_t peripheral, PeripheralStateCB NotifyEvent);
typedef int32_t (*getPeripheralStateFnPtr) (void* ctx);
typedef int32_t (*deRegisterPeripheralCBFnPtr)(void* ctx);

class SecureModeModule: public Module, public AddPendingMessageList
{
    private:
        uint8_t mSecureModeState{IPeripheralState_STATE_NONSECURE};
        void* mSecureModeContext{nullptr};
        TimeKeeper::timer_id mRetryTimer{TimeKeeper::no_timer};
        void* mPeripheralControlLibInstance{nullptr};
        registerPeripheralCBFnPtr mRegisterPeriferalCb{nullptr};
        getPeripheralStateFnPtr mGetPeripheralState{nullptr};
        deRegisterPeripheralCBFnPtr mDeRegisterPeriferalCb{nullptr};
        static constexpr TimeKeeper::millisec SECURE_MODE_RETRY_TIMEOUT = 1000;
        static constexpr int32_t mMaxRetries = 15;
        static constexpr int32_t mMaxRetriesWithoutBackoff = 5;
    public:
        SecureModeModule();
        void secureModeRetryHandler(void*);
        void registerSecureModeIndications();
        void resetSecureModeState();
        void deRegisterSecureModeIndications();
        static int32_t SecureModeNotifyEvent(const uint32_t peripheral, const uint8_t state);
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleGetSecureModeStatusMessage(
            std::shared_ptr<RilRequestGetSecureModeStatusMessage> msg);
        void handleRegisterSecureModeIndicationMessage(
            std::shared_ptr<RegisterSecureModeIndicationMessage> msg);
};

