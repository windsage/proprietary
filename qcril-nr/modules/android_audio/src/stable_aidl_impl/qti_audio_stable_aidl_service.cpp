/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qti_audio_stable_aidl_service.h"
#include <framework/Log.h>
#include <cutils/properties.h>
#include "interfaces/audio/QcRilAudioManagerReadyMessage.h"
#include "interfaces/voice/QcRilUnsolAudioStateChangedMessage.h"
#include "interfaces/voice/QcRilRequestSetAudioServiceStatusMessage.h"
#include "qcril_other.h"
#include "qcril_legacy_apis.h"

#undef TAG
#define TAG "RILQ"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace am {
namespace implementation {



/*
 * Sets the instance of the Audio service corresponding to the RIL instance
 */
void QcrilAudioSAidlService::setInstanceId(qcril_instance_id_e_type instId) {
    mInstanceId = instId;
}

/*
 * Helper to get the instance of the Audio service
 */
qcril_instance_id_e_type QcrilAudioSAidlService::getInstanceId() {
    return mInstanceId;
}

/*
 * Returns the request callback object to call into Telephony APIs
 */
std::shared_ptr<IQcRilAudioRequest> QcrilAudioSAidlService::getRequestCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mRequestCb;
}

/*
 * Clear callbacks when Audio service dies.
 */
void QcrilAudioSAidlService::clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("clearCallbacks_nolock");
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
    mRequestCb = nullptr;
}

void QcrilAudioSAidlService::clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
}

/*
 * Callback to notify death and clear callbacks
 */
void QcrilAudioSAidlService::deathNotifier(void* /*cookie*/) {
    QCRIL_LOG_DEBUG("QcrilAudioSAidlService:: Client died. Cleaning up callbacks");
    clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
    QcrilAudioSAidlService* pQcrilAudioSAidlService = static_cast<QcrilAudioSAidlService*>(cookie);
    if (pQcrilAudioSAidlService != nullptr) {
        pQcrilAudioSAidlService->deathNotifier(cookie);
    }
}

std::shared_ptr<QcRilAudioContext> QcrilAudioSAidlService::getContext(uint32_t serial) {
    return std::make_shared<QcRilAudioContext>(mInstanceId, serial);
}

/*
 *@func Sets the Audio request callback object
 */

void QcrilAudioSAidlService::setRequestInterface_nolock(
    const std::shared_ptr<IQcRilAudioRequest>& in_requestCallback) {
    QCRIL_LOG_DEBUG("QcrilAudioSAidlService::setRequestInterface_nolock");
    mRequestCb = in_requestCallback;
}

//IQcRilAudio interface implementation

/*
 * @func Exchanges Request/Response callback functions
 * @param IQcRilAudioRequest callback Object : Audio request callback
 *      functions for Qcril client
 * @return IQcRilAudioResponse callback Object : Audio response callback
 *     functions for Telephony server
 */

ndk::ScopedAStatus QcrilAudioSAidlService::setRequestInterface(const std::shared_ptr<IQcRilAudioRequest>& in_requestCallback, std::shared_ptr<IQcRilAudioResponse>* _aidl_return) {

    QCRIL_LOG_FUNC_ENTRY();
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    if (mRequestCb != nullptr) {
        AIBinder_unlinkToDeath(mRequestCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }

    setRequestInterface_nolock(in_requestCallback);

    if (mRequestCb != nullptr) {
        AIBinder_DeathRecipient_delete(mDeathRecipient);
        mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
        if (mDeathRecipient) {
            AIBinder_linkToDeath(mRequestCb->asBinder().get(), mDeathRecipient,
                                 reinterpret_cast<void*>(this));
        }
    }

    if (_aidl_return != nullptr) {
        if (mResponseCb != nullptr) {
            *_aidl_return =  mResponseCb;
        } else {
            QCRIL_LOG_DEBUG("QcRilAudio : Failed to create Response Callback object");
        }
    }

    auto msg = std::make_shared<QcRilAudioManagerReadyMessage>(
        this->getContext(0));
    if (msg) {
        QCRIL_LOG_DEBUG("QcrilAudioSAidlService::setRequestInterface_AM is ready");
        msg->dispatch();
    }

    QCRIL_LOG_FUNC_RETURN("exit");
    return ndk::ScopedAStatus::ok();
}

/*
 * @func Set Telephony Audio Manager state
 * @param AudioError Enum representing AM state
 */

ndk::ScopedAStatus QcrilAudioSAidlService::setError(const AudioError errorCode) {
    QCRIL_LOG_FUNC_ENTRY();
    if (errorCode == AudioError::STATUS_OK) {
      auto msg = std::make_shared<QcRilRequestSetAudioServiceStatusMessage>(
          this->getContext(0));
      if (msg) {
        msg->setIsReady(true);
        msg->dispatch();
      }
    } // TODO : what about other cases ?

    QCRIL_LOG_FUNC_RETURN("exit");
    return ndk::ScopedAStatus::ok();
}

/*
 * @func Queries the audio parameters from Telephony
 * @param QcRilGetAudioParamMessage : Async message to query audio parameters
 */

void QcrilAudioSAidlService::onQueryParameters(std::shared_ptr<QcRilQueryAudioParamMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg != nullptr) {
        auto requestCb = getRequestCallback();
        if (requestCb == nullptr) {
            QCRIL_LOG_DEBUG(" Telephony AM Error: Request callback"
                "interface not available.");
            msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
        } else {
            std::pair<int32_t, bool> tokenStatus{};
            tokenStatus = msgQueue.insert(msg);
            if (tokenStatus.second == true) {
                requestCb->queryParameters(tokenStatus.first,
                    msg->getQueryString());}
        }
    } else {
        QCRIL_LOG_DEBUG(" QcRilGetAudioParamMessage is Null ");
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
    QCRIL_LOG_FUNC_RETURN("exit");
}

/*
 * Helper to convert call state to Audio num
 */

uint32_t QcrilAudioSAidlService::convertCallStateToAudioNum(const AmCallState& call_state) {
  uint32_t result = 0;
  switch (call_state) {
    case AmCallState::INVALID:
      result = 0;
      break;
    case AmCallState::INACTIVE:
      result = 1;
      break;
    case AmCallState::ACTIVE:
      result = 2;
      break;
    case AmCallState::HOLD:
      result = 3;
      break;
    case AmCallState::LOCAL_HOLD:
      result = 4;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}

/*
 * Helper to convert audio parameters to string
 */

std::string QcrilAudioSAidlService::convertRilAudioParamtersToString(
    const qcril::interfaces::QcRilSetAudioParams& params) {
  string aParams = "vsid=" + std::to_string(params.vsid) +
                   ";call_state=" + std::to_string(
                   convertCallStateToAudioNum(params.call_state)) + ";call_type=";
  string call_mode_string = "UNKNOWN";
  switch (params.call_mode) {
    case CALL_MODE_CDMA_V02:
      call_mode_string = "CDMA";
      break;
    case CALL_MODE_GSM_V02:
      call_mode_string = "GSM";
      break;
    case CALL_MODE_UMTS_V02:
      call_mode_string = "UMTS";
      break;
    case CALL_MODE_NR5G_V02:
      call_mode_string = "NR5G";
      break;
    case CALL_MODE_LTE_V02:
      call_mode_string = "LTE";
      break;
    case CALL_MODE_TDS_V02:
      call_mode_string = "TDS";
      break;
    case CALL_MODE_WLAN_V02:
      call_mode_string = "WLAN";
      break;
    default:
      call_mode_string = "UNKNOWN";
      break;
  }
  aParams += call_mode_string;
  aParams += ";crs_call=" + std::string(params.crs_call ? "true" : "false");
  return aParams;
}

/*
 * @func Sets the audio parameters in Telephony to avoid two voices in ACTIVE state
 * @param QcRilGetAudioParamMessage : Async message to send audio parameters
 */

void QcrilAudioSAidlService::onSetParameters(std::shared_ptr<QcRilSetAudioParamMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    std::shared_ptr<RIL_Errno> respPtr = nullptr;
    if (msg != nullptr) {
        auto requestCb = getRequestCallback();
        if (requestCb == nullptr) {
            QCRIL_LOG_DEBUG(" Telephony AM Error: Request"
                "callback interface not available.");
            msg->sendResponse(msg, Message::Callback::Status::FAILURE, respPtr);
        } else {
            std::pair<int32_t, bool> tokenStatus{};
            if (!msg->isParamEmpty()) {
                tokenStatus = msgQueue.insert(msg);
                auto param = msg->getFirstParam();
                std::string oemString = "";
                std::string paramString = convertRilAudioParamtersToString(param);
                oemString += paramString;
                oemString += ";";
                if (tokenStatus.second == true) {
                    requestCb->setParameters(tokenStatus.first, paramString);
                }
#ifndef QMI_RIL_UTF
                /* Send OEM Hook Unsol*/
                auto oemmsg =
                    std::make_shared<QcRilUnsolAudioStateChangedMessage>(oemString);
                if (oemmsg != nullptr) {
                    oemmsg->broadcast();
                }
#endif
            } else { //all params have been set
                respPtr = std::make_shared<RIL_Errno>(RIL_Errno::RIL_E_SUCCESS);
                msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                        respPtr);
            }
        }
    } else {
        QCRIL_LOG_DEBUG(" QcRilSetAudioParamMessage is Null ");
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, respPtr);
    }
    QCRIL_LOG_FUNC_RETURN("exit");
}

/*
 * Helper to convert audio call state from Telephony to
 *  enum qcril::interfaces::AudioCallState
 */
AmCallState QcrilAudioSAidlService::QcrilAudioResponseCallback::convertAudioNumToCallState(const uint32_t call_state) {

  AmCallState result = AmCallState::INVALID;
  switch (call_state) {
    case 0:
      result = AmCallState::INVALID;
      break;
    case 1:
      result = AmCallState::INACTIVE;
      break;
    case 2:
      result = AmCallState::ACTIVE;
      break;
    case 3:
      result = AmCallState::HOLD;
      break;
    case 4:
      result = AmCallState::LOCAL_HOLD;
      break;
    default:
      result = AmCallState::INVALID;
      break;
  }
  return result;
}

//IQcRilAudioResponse interface implementation

/*
 * @func Response callback function for Telephony to pass Audio parameters.
 * @param token : integer token to match the request and responses
 * @param params : string returned from Telephony in the format of
 *    all_call_states=281022464:1,282857472:1,281026560:1,276836352:1
 *    OR isCRSsupported=1
 */

ndk::ScopedAStatus QcrilAudioSAidlService::QcrilAudioResponseCallback::queryParametersResponse(const int32_t token, const std::string& params) {
    QCRIL_LOG_FUNC_ENTRY();
    std::shared_ptr<qcril::interfaces::QcRilGetAudioParams> respPtr = nullptr;
    Message::Callback::Status status = Message::Callback::Status::FAILURE;
    std::shared_ptr<QcRilQueryAudioParamMessage> queryParamMsg = nullptr;
    std::shared_ptr<Message> msg = mService.msgQueue.find(token);
    QCRIL_LOG_DEBUG("[QcRilAudio]: params:%s", params.c_str());
    if (msg) {
        queryParamMsg = std::static_pointer_cast<QcRilQueryAudioParamMessage>(msg);
    }
    if (queryParamMsg != nullptr ) {
        auto index = params.find("isCRSsupported=1");
        respPtr = std::make_shared<qcril::interfaces::QcRilGetAudioParams>();
        if (respPtr != nullptr) {
            status = Message::Callback::Status::SUCCESS;
            respPtr->is_crs_supported = (index != std::string::npos);
            respPtr->call_state = AmCallState::INVALID;
            QCRIL_LOG_DEBUG("[QcRilAudio]: queryParametersResponse is_crs_supported :%d",
                respPtr->is_crs_supported);
        }
        index = params.find("all_call_states");
        if (index != std::string::npos) {
            index = params.find(queryParamMsg->getVsid());
            if (index != std::string::npos && (index + queryParamMsg->getVsid().length() + 1)
                < params.length()) {
                QCRIL_LOG_DEBUG("[QcRilAudio]: Couldn't find the given vsid");
            } else {
                if (respPtr != nullptr) {
                    status = Message::Callback::Status::SUCCESS;
                    respPtr->call_state = convertAudioNumToCallState(
                        params[index + queryParamMsg->getVsid().length() + 1] - '0');
                    QCRIL_LOG_DEBUG("[QcRilAudio]: Call State for vsid:%s=%d",
                        queryParamMsg->getVsid().c_str(), respPtr->call_state);
                }
            }
        }
        queryParamMsg->sendResponse(queryParamMsg, status, respPtr);
        mService.msgQueue.erase(token); //erase token
    }
    // Ignoring response if token not found
    QCRIL_LOG_FUNC_RETURN("exit");
    return ndk::ScopedAStatus::ok();
}

RIL_Errno convertAudioSetParamErrorResponse(const AudioError& errorCode) {
    RIL_Errno res = RIL_Errno::RIL_E_GENERIC_FAILURE;
    switch (errorCode) {
        case AudioError::STATUS_OK:
            res = RIL_Errno::RIL_E_SUCCESS;
            break;
        case AudioError::GENERIC_FAILURE:
            res = RIL_Errno::RIL_E_GENERIC_FAILURE;
            break;
        case AudioError::STATUS_SERVER_DIED:
            res = RIL_Errno::RIL_E_AUDIO_SERVER_DIED;
            break;
        default:
            res = RIL_Errno::RIL_E_GENERIC_FAILURE; // TODO: Check if generic error on default is right?
            break;
    }
    return res;
}

/*
 * @func Response callback for telephony to setResponse
 * @param token - unique token for each request/response
 * @param errorCode corresponding to enum AudioError
 */

ndk::ScopedAStatus QcrilAudioSAidlService::QcrilAudioResponseCallback::setParametersResponse(const int32_t token, AudioError errorCode){
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_DEBUG("QcrilAudioResponseCallback::setParametersResponse");
    std::shared_ptr<QcRilSetAudioParamMessage> setParamMsg = nullptr;
    std::shared_ptr<Message> msg = mService.msgQueue.find(token);
    if (msg) {
        setParamMsg = std::static_pointer_cast<QcRilSetAudioParamMessage>(msg);
    }
    if ( setParamMsg != nullptr ) {
        if (errorCode == AudioError::STATUS_OK) {
            setParamMsg->removeFirstParam();
            mService.msgQueue.erase(token); //erase token
            mService.onSetParameters(setParamMsg);
        } else { //failed response
            auto respPtr = std::make_shared<RIL_Errno>(convertAudioSetParamErrorResponse(errorCode));
            setParamMsg->sendResponse(setParamMsg, Message::Callback::Status::SUCCESS, respPtr);
            mService.msgQueue.erase(token); //erase token
        }
    } else { //ignore response for unknown token
        QCRIL_LOG_INFO("Unknown token: %d ", token);
    }

    QCRIL_LOG_FUNC_RETURN("exit");
    return ndk::ScopedAStatus::ok();
}

}
}
}
}
}
}
}
