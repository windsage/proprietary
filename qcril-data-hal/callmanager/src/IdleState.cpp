/*==============================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"

using namespace SM;
using namespace rildata;

void IdleState::enter(void) {
}

void IdleState::exit(void) {
}

void IdleState::addTimer(void) {
  startTimer(DataCallTimerType::SetupDataCall, (TimeKeeper::millisec)SETUP_DATA_CALL_TIMEOUT_SECONDS * 1000);
}

bool isApnTypeInvalid(ApnTypes_t apnTypes) {
  int eBit = static_cast<int>(ApnTypes_t::EMERGENCY);
  int apnTypesBitmap = static_cast<int>(apnTypes);
  // if emergency bit is set, apn types should be configured to ONLY emergency type
  // otherwise, qcrildata will treat it as a misconfiguration
  if (((eBit & apnTypesBitmap) == eBit) &&
      (eBit != apnTypesBitmap)) {
    return true;
  }
  return false;
}

int IdleState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestBase> m =
        std::static_pointer_cast<SetupDataCallRequestBase>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                            HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        callInfo.currentRAT = NetType;
        callInfo.pduSessionParams[0].request = event.msg;
        callInfo.preferredRAT = NetType;
        std::stringstream ss;
        m->getDataProfileInfo().dump("",ss);
        Log::getInstance().d("DataProfileInfo=" + ss.str());
        callInfo.pduSessionParams[0].requestProfiles.clear();
        callInfo.pduSessionParams[0].requestProfiles.push_back(m->getDataProfileInfo());
        if (DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(callInfo.radioRAT)) {
          callInfo.pduSessionParams[0].dnnName = "";
        }
        callInfo.roamingAllowed = m->getRoamingAllowed();

        if (isApnTypeInvalid(m->getSupportedApnTypesBitmap())) {
          Log::getInstance().d("Invalid apn configuration");
          setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
              DataCallFailCause_t::OEM_DCFAILCAUSE_4);
          transitionTo(_eDisconnected);
        }

        ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[m->getApn()];
        TimeKeeper::getInstance().clear_timer(throttleApn.timer[(int)m->getRequestSource()]);
        throttleApn.timer.erase((int)m->getRequestSource());

        if (isEmergencyCall()) {
          if (setEmergencyProfileInfo()) {
            Log::getInstance().d("Handle emergency data call request");
            if (DSI_SUCCESS != DsiWrapper::getInstance().dsiStartDataCall(callInfo.dsiHandle)) {
              Log::getInstance().d("Invalid apn configuration");
              setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
                  CallEndReason::getCallEndReason(
                    getVerboseCallEndReason()));
              transitionTo(_eDisconnected);
            } else {
              transitionTo(_eConnecting);
            }
            // release wakelock
            if(m->getAcknowlegeRequestCb() != nullptr) {
              auto cb = *(m->getAcknowlegeRequestCb().get());
              cb(m->getSerial());
            }
            return IState<CallEventType>::HANDLED;
          }
          // Fall back to normal setup data call handling
        }
        CallStateFailureReason reason = setProfileInfo();
        if (reason == CallStateFailureReason::NoError) {
          //Need to call DSD_SET_APN_INFO to update APN types for call if in
          //"legacy" mode
          if (!globalInfo.apAssistMode) {
            if(callInfo.dsd_endpoint != nullptr) {
              Message::Callback::Status status = Message::Callback::Status::FAILURE;
              status = callInfo.dsd_endpoint->setApnInfoSync(
                callInfo.pduSessionParams[0].dnnName,
                callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap);
              if (status != Message::Callback::Status::SUCCESS) {
                Log::getInstance().d("SET_APN_INFO request failed,"
                                " result = "+ std::to_string((int) status));
              }
              else {
                Log::getInstance().d("SET_APN_INFO request successful"
                                    ", result = "+ std::to_string((int) status));
              }
            }
            else {
              Log::getInstance().d("ERROR dsd_endpoint is NULL.");
            }
          } //end if(!mApAssistMode)

          //Add handover IP addresses if call is being setup for 3rd party handover
          if ((m->getDataRequestReason() == DataRequestReason_t::HANDOVER)) {
            if (CallManager::iWlanRegistered) {
              Log::getInstance().d("3rd party handover not supported as qc IWLAN used");
              setupDataCallFailed(ResponseError_t::NOT_SUPPORTED,
                DataCallFailCause_t::ERROR_UNSPECIFIED);
              transitionTo(_eDisconnected);
              return IState<CallEventType>::HANDLED;
            }
            Log::getInstance().d("3rd party handover from AP.");
#ifndef RIL_FOR_MDM_LE
            setThirdPartyHOParams(m->getAddresses());
#endif
          }
          if (DSI_SUCCESS != DsiWrapper::getInstance().dsiStartDataCall(callInfo.dsiHandle)) {
            Log::getInstance().d("Invalid apn configuration");
            setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
                CallEndReason::getCallEndReason(
                    getVerboseCallEndReason()));
            transitionTo(_eDisconnected);
            return IState<CallEventType>::HANDLED;
          }
        }
        else if (reason == CallStateFailureReason::DsiHandleFailed) {
            Log::getInstance().d("Dsi get handle will be retried when netmgrd restarts");
            // release wakelock
            if(m->getAcknowlegeRequestCb() != nullptr) {
              auto cb = *(m->getAcknowlegeRequestCb().get());
              cb(m->getSerial());
            }
            return IState<CallEventType>::HANDLED;
        }
        else {
          setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
              DataCallFailCause_t::OEM_DCFAILCAUSE_4);
          transitionTo(_eDisconnected);
          return IState<CallEventType>::HANDLED;
        }
        // release wakelock
        if(m->getAcknowlegeRequestCb() != nullptr) {
          auto cb = *(m->getAcknowlegeRequestCb().get());
          cb(m->getSerial());
        }
        transitionTo(_eConnecting);
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        if (m->getType() == DataCallTimerType::SetupDataCall) {
          SetupDataCallResponse_t response = {};
          response.call.cid = -1;
          response.call.suggestedRetryTime = -1;
          response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
          setupDataCallCompleted(response);
          transitionTo(_eDisconnected);
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::CleanUp:
    {
      stopTimer(DataCallTimerType::SetupDataCall);
      SetupDataCallResponse_t response = {};
      response.call.cid = -1;
      response.call.suggestedRetryTime = -1;
      response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
      setupDataCallCompleted(response);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DisconnectCallFromOlderDDSSub:
    {
      stopTimer(DataCallTimerType::SetupDataCall);
      SetupDataCallResponse_t response = {};
      response.call.cid = -1;
      response.call.suggestedRetryTime = -1;
      response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
      setupDataCallCompleted(response);
      return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
