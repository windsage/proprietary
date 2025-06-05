/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "SecureModeModule"

#include <dlfcn.h>
#include <limits.h>


#include "SecureModeModule.h"
#include "interfaces/securemode/securemode_types.h"
#include "interfaces/securemode/SecureModeStateChangeIndMessage.h"

static load_module<SecureModeModule> the_module;

SecureModeModule::SecureModeModule() : AddPendingMessageList("SecureModeModule")
{
  mName = "SecureModeModule";
  using std::placeholders::_1;

  mMessageHandler = {
       HANDLER(QcrilInitMessage, SecureModeModule::handleQcrilInit),
       HANDLER(RegisterSecureModeIndicationMessage,
               SecureModeModule::handleRegisterSecureModeIndicationMessage),
       HANDLER(RilRequestGetSecureModeStatusMessage,
               SecureModeModule::handleGetSecureModeStatusMessage),
  };
}

int32_t SecureModeModule::SecureModeNotifyEvent(const uint32_t peripheral, const uint8_t state)
{
  Log::getInstance().d("[ SecureModeModule ]: Received Peripheral Notification from TZ");
  Log::getInstance().d("[ SecureModeModule ]: Peripheral :" + std::to_string(peripheral));
  Log::getInstance().d("[ SecureModeModule ]: State :" + std::to_string(state));
  if (peripheral != CPeripheralAccessControl_MODEM_UID) {
    Log::getInstance().d("[ SecureModeModule ]: Only interested in Modem Peripheral Indication");
    return 0;
  }

  if (state == STATE_RESET_CONNECTION) {
    auto msg = std::make_shared<RegisterSecureModeIndicationMessage>();
    if (msg) {
      msg->broadcast();
    }
  } else {
    auto msg = std::make_shared<SecureModeStateChangeIndMessage>(state);
    if (msg) {
      msg->broadcast();
    }
  }
  return 0;
}

void SecureModeModule::handleRegisterSecureModeIndicationMessage(
    std::shared_ptr<RegisterSecureModeIndicationMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  registerSecureModeIndications();
}

void SecureModeModule::secureModeRetryHandler(void*) {
  Log::getInstance().d("[" + mName + "]: SecureModeRetryHandler");
  // Safety check to ensure we don't reregister if already done
  if(mSecureModeContext == NULL) {
    auto msg = std::make_shared<RegisterSecureModeIndicationMessage>();
    if (msg) {
      msg->broadcast();
    }
  } else {
    Log::getInstance().d("[" + mName + "]: should never reach here, ignoring secure mode ind reg retry");
  }
}

void SecureModeModule::registerSecureModeIndications() {
  static int counter = 0;
  TimeKeeper::millisec timeout = SECURE_MODE_RETRY_TIMEOUT;
  Log::getInstance().d("[" + mName + "]: registerForSecureModeIndications");
  mPeripheralControlLibInstance = dlopen("libPeripheralStateUtils.so", RTLD_LAZY);

  if( mPeripheralControlLibInstance == nullptr ) {
    Log::getInstance().d("[" + mName + "]: Unable to load peripheral Control Lib instance");
    return;
  }

  dlerror(); // clear previous dlopen errors

  mRegisterPeriferalCb = reinterpret_cast<registerPeripheralCBFnPtr>(dlsym (mPeripheralControlLibInstance, "registerPeripheralCB"));
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
    Log::getInstance().d("[" + mName + "]: Cannot find registerPeripheralCB symbol");
    return;
  }

  if(mRegisterPeriferalCb == nullptr) {
    Log::getInstance().d("[" + mName + "]: mGetPeripheralState is null");
    return;
  }

  // Before registering cancel mRetryTimer if any
  if(mRetryTimer != TimeKeeper::no_timer) {
    TimeKeeper::getInstance().clear_timer(mRetryTimer);
    mRetryTimer = TimeKeeper::no_timer;
  }

  // After mMaxRetriesWithoutBackoff retries, use back off timer
  if(counter > mMaxRetriesWithoutBackoff && counter <= mMaxRetries) {
    timeout = SECURE_MODE_RETRY_TIMEOUT * (1<<(counter-mMaxRetriesWithoutBackoff));
    Log::getInstance().d("[" + mName + "]: using back off timer");
  }

  if (counter > mMaxRetries) {
    Log::getInstance().d("[" + mName + "]: exceeded maximum retries");
    return;
  }

  mSecureModeContext = mRegisterPeriferalCb(CPeripheralAccessControl_MODEM_UID,
                                             SecureModeModule::SecureModeNotifyEvent);
  if (mSecureModeContext == NULL) {
    Log::getInstance().d("[" + mName + "]: Registering for Modem Peripheral failed");
    mRetryTimer = TimeKeeper::getInstance().set_timer(
      std::bind(&SecureModeModule::secureModeRetryHandler, this, std::placeholders::_1),
      nullptr,
      timeout);
      counter++;
    return;
  }

  // if registration is successful, reset counter
  counter = 0;

  Log::getInstance().d("[" + mName + "]: Secure Peripheral for Modem is registered");

  dlerror(); // clear previous dlopen errors
  mGetPeripheralState = reinterpret_cast<getPeripheralStateFnPtr>(dlsym(mPeripheralControlLibInstance, "getPeripheralState"));
  dlsym_error = dlerror();

  if( dlsym_error ) {
    Log::getInstance().d("[" + mName + "]: Cannot find getPeripheralState symbol");
    return;
  }

  if(mGetPeripheralState == nullptr) {
    Log::getInstance().d("[" + mName + "]: mGetPeripheralState is null");
    return;
  }


  // Get the modem peripheral state
  auto state = mGetPeripheralState(mSecureModeContext);
  if (state == PRPHRL_ERROR) {
    Log::getInstance().d("[" + mName + "]: getPeripheralState Failed");
    return;
  }

  Log::getInstance().d("[" + mName + "]: getPeripheralState Success");

  if (state != mSecureModeState) {
    mSecureModeState = state;
    // As there is change in state, send unsol to telephony
    auto msg = std::make_shared<SecureModeStateChangeIndMessage>(state);
    if (msg) {
      msg->broadcast();
    }
  }

  Log::getInstance().d("[" + mName + "]: SecureMode Peripheral Status on registration :" +
      std::to_string(mSecureModeState));
}

void SecureModeModule::resetSecureModeState() {
  mSecureModeState = IPeripheralState_STATE_NONSECURE;
  mPeripheralControlLibInstance = nullptr;
  mRegisterPeriferalCb = nullptr;
  mSecureModeContext = nullptr;
  mDeRegisterPeriferalCb = nullptr;
  mGetPeripheralState = nullptr;
}

// TODO: when is this function called ?
void SecureModeModule::deRegisterSecureModeIndications() {
  Log::getInstance().d("[" + mName + "]: deRegisterForSecureModeIndications");
  if (mSecureModeContext == NULL) {
    Log::getInstance().d("[" + mName + "]: Secure Mode Peripheral control context not available");
    return;
  }

  dlerror(); // clear previous dlopen errors
  const char* dlsym_error = dlerror();
  mDeRegisterPeriferalCb = reinterpret_cast<deRegisterPeripheralCBFnPtr>(dlsym (mPeripheralControlLibInstance, "deregisterPeripheralCB"));
  if (dlsym_error) {
    Log::getInstance().d("[" + mName + "]: Cannot find deregisterPeripheralCB symbol");
    return;
  }

  if(mDeRegisterPeriferalCb == nullptr) {
    Log::getInstance().d("[" + mName + "]: mDeRegisterPeriferalCb is null");
    return;
  }

  auto state = mDeRegisterPeriferalCb(mSecureModeContext);
  if (state == PRPHRL_ERROR) {
    Log::getInstance().d("[" + mName + "]: deregisterPeripheralCB Failed");
    return;
  } else if (state == PRPHRL_SUCCESS) {
    Log::getInstance().d("[" + mName + "]: deregisterPeripheralCB Success");
    resetSecureModeState();
  }
}

void SecureModeModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg->get_instance_id() == QCRIL_DEFAULT_INSTANCE_ID) {
    auto msg = std::make_shared<RegisterSecureModeIndicationMessage>();
    if (msg) {
      msg->broadcast();
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Ignore secure mode registration for RIL1");
  }
}

void SecureModeModule::handleGetSecureModeStatusMessage(
    std::shared_ptr<RilRequestGetSecureModeStatusMessage> msg) {

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<qcril::interfaces::RilSecureModeStatus_t> payload = nullptr;
  std::shared_ptr<QcRilRequestMessageCallbackPayload> respData = nullptr;
  if (mGetPeripheralState && mSecureModeContext) {
    auto state = mGetPeripheralState(mSecureModeContext);
    if (state != PRPHRL_ERROR) {
      Log::getInstance().d("[" + mName + "]: getPeripheralState Success");
      if (state != mSecureModeState) {
        mSecureModeState = state;
        Log::getInstance().d("[" + mName + "]: SecureMode Peripheral Status changed to :" +
            std::to_string(mSecureModeState));
      }
      //TODO: if state is reconnect, reregister.
      payload = std::make_shared<qcril::interfaces::RilSecureModeStatus_t>(mSecureModeState);
      respData = std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_SUCCESS, payload);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
      return;
    } else {
      Log::getInstance().d("[" + mName + "]: getPeripheralState returned an error");
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
      return;
    }
  } else {
    Log::getInstance().d("[" + mName + "]: mGetPeripheralState and/or mSecureModeContext is NULL");
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
    return;
  }
}

