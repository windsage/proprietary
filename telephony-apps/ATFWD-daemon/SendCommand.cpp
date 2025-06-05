/*!
  @file
  SendCommand.cpp

  @brief
  Places a binder call to Android's IAtCmd service to forward an AT command and receives the
  response back from the client.
*/

/*===========================================================================
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_TAG "Atfwd_sendCommand"

#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>
#include "common_log.h"

#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/AtCmd.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/AtCmdResponse.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/AtCmdResult.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/AtCmdToken.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/AtCmdTokenNumMax.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/IAtFwd.h>
#include <aidl/vendor/qti/hardware/radio/atfwd/IAtFwdIndication.h>
#include "AtFwdNativeModule/AtFwdServiceImpl.h"

namespace aidlimports {
  using namespace aidl::vendor::qti::hardware::radio::atfwd;
}

namespace aidlimplimports {
  using namespace aidl::vendor::qti::hardware::radio::atfwd::implementation;
}

/* Serial number to be sent to the Java module along with the AT command */
int mSerial;

/* Variables/objects used for handling the AT command reponse received from the Java module */
AtCmdResponse mAtCmdResponse;
bool mIsResponseValid = false;
bool mIsResponseReceived = false;

/* AIDL service implementation */
std::shared_ptr<aidlimplimports::AtFwdServiceImpl> mAtFwdAidlService = nullptr;

/* Mutex and condition variables to enable synchronization */
pthread_cond_t responseCond;
pthread_mutex_t responseMutex;

/*
 * This is invoked from AtFwdServiceImpl when the Java module sends back the response after
 * processing an AT command.
 * This function will process the response, determine its validity, and then convert it from
 * aidl type to local type, to be finally consumed by the daemon.
 *
 * @param serial is the serial number sent to the Java module along with the AT command
 * @param aidlResponse is the AtCmdResponse instance received from the Java module
 */
void onAtCommandResponse(int serial, aidlimports::AtCmdResponse aidlResponse) {
  mAtCmdResponse.result = (int) aidlResponse.result;

  if(aidlResponse.result != aidlimports::AtCmdResult::ATCMD_RESULT_OK) {
    LOGE("onAtCommandResponse: result is not ATCMD_RESULT_OK");
    mAtCmdResponse.response = nullptr;
    mIsResponseValid = false;

  } else if (aidlResponse.response == "") {
    LOGD("onAtCommandResponse: valid result with a blank string");
    mAtCmdResponse.response = nullptr;
    mIsResponseValid = true;

  } else {
    const char *responseString = aidlResponse.response.c_str();
    int responseStringLength = aidlResponse.response.size() + 1;
    mAtCmdResponse.response = (char *)malloc(sizeof(char) * responseStringLength);

    if (mAtCmdResponse.response == NULL) {
      LOGE("onAtCommandResponse: malloc error!");
      mIsResponseValid = false;
    } else {
      strlcpy(mAtCmdResponse.response, responseString, responseStringLength);
      mIsResponseValid = true;
    }
  }

  LOGI("onAtCommandResponse serial: %d, result: %d, response: %s",
      serial, mAtCmdResponse.result, mAtCmdResponse.response);

  // Signal the receiver thread that the response has now been received
  pthread_mutex_lock(&responseMutex);
  mIsResponseReceived = true;
  pthread_cond_signal(&responseCond);
  pthread_mutex_unlock(&responseMutex);
}

/*
 * This is invoked from AtFwdServiceImpl when the Java module somehow loses connection
 * to this services. If we are waiting for a response from the client, we stop doing it now
 * and send the signal back to the daemon.
 */
void onClientDied() {
  LOGI("onClientDied");
  pthread_mutex_lock(&responseMutex);
  mIsResponseValid = false;
  mIsResponseReceived = true;
  pthread_cond_signal(&responseCond);
  pthread_mutex_unlock(&responseMutex);
}

/*
 * Initialize the mutex/condition variables and callbacks
 * Mutex and the condition variable are used to wait for the Java module to send the
 * response of the forwarded AT command.
 * This is needed because the daemon makes a synchronous call while sending the command and
 * expects the result in the same call.
 */
void initMutexAndCallbacks() {
  if (mAtFwdAidlService != nullptr) {
    // Set the function pointer to receive callbacks when an AT command
    // has been processed by the APPS
    mAtFwdAidlService->setAtCommandResponseFuncPointer(onAtCommandResponse);

    // Set the function pointer to receive callbacks when the client dies
    mAtFwdAidlService->setClientDeathRecipientFuncPointer(onClientDied);
  }
  // Initialize the mutex and condition
  pthread_mutexattr_t attr;
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&responseMutex, &attr);
  pthread_cond_init(&responseCond, NULL);

  // Initialize the serial number for the AT commands
  mSerial = 0;
}

/*
 * @return the next serial number to be passed to Java module along with the AT command
 */
int getNextSerial() {
  return ++mSerial;
}

/*
 * Registers the Stable AIDL service IAtFwd with ServiceManager.
 * Invoked from atfwd_daemon.c
 */
extern "C" void registerHalService() {
  bool success = ABinderProcess_setThreadPoolMaxThreadCount(1);
  ABinderProcess_startThreadPool();
  LOGD("registerHalService, thread pool set state: %d", success);

  std::string serviceName = std::string() + aidlimports::IAtFwd::descriptor + "/AtFwdAidl";

  if (mAtFwdAidlService == nullptr) {

    mAtFwdAidlService = ndk::SharedRefBase::make<aidlimplimports::AtFwdServiceImpl>();
    binder_status_t status =
        AServiceManager_addService(mAtFwdAidlService->asBinder().get(), serviceName.c_str());

    LOGI("registerHalService, addService status: %d, name: %s", status, serviceName.c_str());

    if (status == STATUS_OK) {
      initMutexAndCallbacks();
    } else {
      mAtFwdAidlService = nullptr;
      LOGE("Error registering service");
    }
  } else {
    LOGE("registerHalService, service is already registered!");
  }
}

/*
 * Forwards AT command to the Java module via the AIDL service.
 *
 * @return true if AIDL API was invoked successfully, false otherwise.
 */
bool sendCommandToNewAidlService(const AtCmd *cmd) {
  // Convert local AtCmdResponse to AIDL type
  aidlimports::AtCmd aidlCommand{};
  aidlCommand.name = cmd->name;
  aidlCommand.opCode = cmd->opcode;
  aidlCommand.token.numberOfItems = cmd->ntokens;
  aidlCommand.token.items.resize(aidlCommand.token.numberOfItems);
  for (unsigned int i = 0; i < aidlCommand.token.numberOfItems; i++) {
    aidlCommand.token.items[i] = cmd->tokens[i];
  }

  // Call AIDL API
  if (mAtFwdAidlService != nullptr) {
    return mAtFwdAidlService->onAtCommandForwarded(getNextSerial(), aidlCommand);
  }
  return false;
}

/*
 * Sends the AT command received by the modem to the Java module, and returns the
 * corresponding reponse received.
 * Invoked from atfwd_daemon.c
 *
 * @param AtCmd - AT command forwarded by the modem
 * @return AtCmdResponse - the response received from the Java module
 *         after the given AT command has been processed
 */
extern "C" AtCmdResponse *processAtCommand(const AtCmd *cmd) {
  if (cmd == nullptr) {
    LOGE("processAtCommand: cmd is nullptr!");
    return nullptr;
  }

  if (cmd->name == nullptr) {
    LOGE("processAtCommand: cmd name is nullptr!");
    return nullptr;
  }

  if (mAtFwdAidlService == nullptr) {
    LOGE("processAtCommand: AIDL service is not up!");
    return nullptr;
  }

  AtCmdResponse* atCmdResponse;
  pthread_mutex_lock(&responseMutex);

  // These will get set when we receive the response from the client
  mIsResponseValid = false;
  mIsResponseReceived = false;

  bool isRequestSentSuccessfully = sendCommandToNewAidlService(cmd);

  if (!isRequestSentSuccessfully) {
    LOGE("processAtCommand: Unable to send request to AIDL service!");
    atCmdResponse = nullptr;
  } else {
    // Request was successfully sent. Wait for the response signal.
    while (!mIsResponseReceived) {
      LOGI("processAtCommand: Waiting for response signal");
      pthread_cond_wait(&responseCond, &responseMutex);
    }

    if (mIsResponseValid) {
      LOGI("processAtCommand: Received response: {%d, %s}",
          mAtCmdResponse.result, mAtCmdResponse.response);
      atCmdResponse = &mAtCmdResponse;
    } else {
      LOGE("processAtCommand: Received response is invalid");
      atCmdResponse = nullptr;
    }

  }

  pthread_mutex_unlock(&responseMutex);
  return atCmdResponse;
}
