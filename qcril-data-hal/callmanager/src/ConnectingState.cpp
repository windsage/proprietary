/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/GetPdnThrottleTimeResponseInd.h"

using namespace std;
using namespace SM;
using namespace rildata;

void ConnectingState::enter(void) {
  response = {};
  response.respErr = ResponseError_t::NO_ERROR;
  response.call.suggestedRetryTime = -1;
  response.call.cause = DataCallFailCause_t::NONE;
  response.call.cid = -1;
  response.call.active = _eInactive;
  response.call.type = "";
  response.call.ifname = "";
  response.call.addresses = "";
  response.call.dnses = "";
  response.call.gateways = "";
  response.call.pcscf = "";
  response.call.mtu = 0;
  response.call.mtuV4 = 0;
  response.call.mtuV6 = 0;
  response.call.trafficDescriptors.empty();
}

void ConnectingState::exit(void) {
  stopTimer(DataCallTimerType::SetupDataCall);
  if(moveToLatching) return;
  if (response.respErr == ResponseError_t::NO_ERROR &&
      response.call.cause == DataCallFailCause_t::NONE) {
    response.call.suggestedRetryTime = -1;
    response.call.cid = callInfo.cid;
    response.call.active = callInfo.consolidatedActive;
    response.call.type = callInfo.ipType;
    response.call.ifname = callInfo.deviceName;
    response.call.addresses = callInfo.convertToLegacyLinkAddresses();
    response.call.linkAddresses = callInfo.convertAddrStringToLinkAddresses();
    response.call.dnses = callInfo.convertToResultDnsAddresses();
    if (callInfo.pduInfoList.size() > 0) {
      response.call.gateways = callInfo.pduInfoList.front().concatGatewayAddress();
      response.call.pcscf = callInfo.pduInfoList.front().concatPcscfAddress();
    }
    response.call.mtu = callInfo.mtu;
    response.call.mtuV4 = callInfo.consolidatedMtuV4;
    response.call.mtuV6 = callInfo.consolidatedMtuV6;
    for (auto psp : callInfo.pduSessionParams) {
      for (auto rp : psp.requestProfiles) {
        if (rp.trafficDescriptor.has_value())
        {
          response.call.trafficDescriptors.push_back(rp.trafficDescriptor.value());
        }
      }
    }
    if((callInfo.pduSessionParams[0].requestProfiles.size() > 0) &&
       (((int)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) & ((int)ApnTypes_t::IMS)))
    {
      initializeQos();
    }
  }
  setupDataCallCompleted(response);
}

int ConnectingState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      Log::getInstance().d("setupdatacall with the same parameters is requested");
      shared_ptr<SetupDataCallRequestBase> m =
        std::static_pointer_cast<SetupDataCallRequestBase>(event.msg);
      SetupDataCallResponse_t result = {};
      result.respErr = ResponseError_t::NO_ERROR;
      result.call = {.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_1};
      result.call.suggestedRetryTime = -1;
      sendSetupDataCallResponse(m, result);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      /* Telephony couldn't request for deactivate data in connectingstate as
         it is not aware about cid value in connectingState.
         so if there is a race condition and deactivate data is received in this state,
         it may be for the older call. So sending success from here as
         older call with the same cid is already disconnected. */

      shared_ptr<DeactivateDataCallRequestMessage> m =
         std::static_pointer_cast<DeactivateDataCallRequestMessage>(event.msg);
      if(m != nullptr)
      {
        sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
        //release wakelock
        if(m->getAcknowlegeRequestCb() != nullptr) {
          auto cb = *(m->getAcknowlegeRequestCb().get());
          cb(m->getSerial());
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      shared_ptr<RilEventDataCallback> m =
        std::static_pointer_cast<RilEventDataCallback>(event.msg);
      if (m != nullptr) {
        EventDataType *pEvtData = m->getEventData();
        if (pEvtData != nullptr) {
          dsi_net_evt_t netEvent = pEvtData->evt;
          switch (netEvent) {
            case DSI_EVT_NET_IS_CONN:
            {
              configureParams();
              callInfo.pduInfoList.front().active = _eActivePhysicalLinkUp;
              callInfo.calculateConsolidatedActive();

              if(callInfo.deviceName.empty())
              {
                callInfo.v4Connected = false;
                callInfo.v6Connected = false;
                Log::getInstance().d("iface is down whille handle IS_CONN, wait NO_NET");
                break;
              }

              if( callInfo.currentRAT == HandoffNetworkType_t::_eIWLAN &&
                  callInfo.dsd_endpoint->getDsdCIWlanCapability()) {
                HandoffNetworkType_t currentDataBearerRAT = callInfo.currentRAT;
                if(DsiWrapper::getInstance().dsiGetCurrentDataBearerTech(callInfo.dsiHandle, currentDataBearerRAT) == DSI_SUCCESS) {
                  callInfo.currentRAT = currentDataBearerRAT;
                  callInfo.preferredRAT = currentDataBearerRAT;
                }
              }
              createQmiWdsClients();
              transitionTo(_eConnected);
              break;
            }
            case DSI_EVT_NET_NO_NET:
            {
              dsi_ce_reason_t dsiReason = getVerboseCallEndReason();
              if ((dsiReason.reason_type == DSI_CE_TYPE_INTERNAL) &&
                  (dsiReason.reason_code == WDS_VCER_INTERNAL_APN_TYPE_MISMATCH_V01) &&
                  (callInfo.callBringUpCapability != BringUpCapability::BRING_UP_LEGACY)){
                Log::getInstance().d("call failed for cid = " + std::to_string(callInfo.cid)
                             + " due to APN type mismatch, fallback to legacy bringup");
                callInfo.callBringUpCapability = BringUpCapability::BRING_UP_LEGACY;
                if (callInfo.dsiHandle != nullptr) {
                  DsiWrapper::getInstance().dsiRelDataSrvcHndl(callInfo.dsiHandle);
                  callInfo.dsiHandle = nullptr;
                }
                if (isEmergencyCall()) {
                  if (setEmergencyProfileInfo()) {
                    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiStartDataCall(callInfo.dsiHandle)) {
                      Log::getInstance().d("Invalid apn configuration");
                      response.call.cause = CallEndReason::getCallEndReason(
                          getVerboseCallEndReason());
                      transitionTo(_eDisconnected);
                    }
                    break;
                  }
                }
                if (CallStateFailureReason::NoError == setProfileInfo()) {
                  if (DSI_SUCCESS != DsiWrapper::getInstance().dsiStartDataCall(callInfo.dsiHandle)) {
                    Log::getInstance().d("Invalid apn configuration");
                    response.call.cause = CallEndReason::getCallEndReason(
                        getVerboseCallEndReason());
                    transitionTo(_eDisconnected);
                  }
                } else {
                  transitionTo(_eDisconnected);
                }
                return IState<CallEventType>::HANDLED;
              }
              else {
                bool throttleTimeRequested = false;
                if (isReasonPdnThrottled(dsiReason)) {
                  auto msg = std::static_pointer_cast<SetupDataCallRequestBase>(callInfo.pduSessionParams[0].request);
                  if (msg != nullptr) {
                    RequestSource_t src = msg->getRequestSource();
                    ThrottleApn_t throttleApn;
                    throttleApn.profileInfo = msg->getDataProfileInfo();
                    throttleApn.timer[(int)src] = TimeKeeper::no_timer;
                    globalInfo.serviceStatus.throttledApns[msg->getApn()] = throttleApn;
                    if (src == RequestSource_t::RADIO) {
                      getPdnThrottleTime();
                      throttleTimeRequested = true;
                    }
                  }
                }
                response.call.cause = CallEndReason::getCallEndReason(dsiReason);
                if (!throttleTimeRequested) {
                  transitionTo(_eDisconnected);
                }
                // if throttle was requested, remain in ConnectingState until ThrottleTimeAvailable
                return IState<CallEventType>::HANDLED;
              }
              break;
            }
#ifndef RIL_FOR_MDM_LE
            case DSI_EVT_NET_ERR_CALL_EXISTS:
            {
              int ipType = 0;
              dsi_ce_reason_t v4Reason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IP");
              dsi_ce_reason_t v6Reason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IPV6");
              if (v4Reason.reason_type == DSI_CE_TYPE_CALL_ALREADY_EXISTS) {
                ipType += DSI_IP_VERSION_4;
              }
              if (v6Reason.reason_type == DSI_CE_TYPE_CALL_ALREADY_EXISTS) {
                ipType += DSI_IP_VERSION_6;
              }
              DsiWrapper::getInstance().dsiCheckIfCallExists(callInfo.dsiHandle,
                callInfo.pduSessionParams[0].dnnName,
                WDSModemEndPointModule::convertToApnTypeMask((int32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap),
                ipType,
                callInfo.deviceName);
              if (!callInfo.deviceName.empty()) {
                Log::getInstance().d("latching to device name "+callInfo.deviceName);
                moveToLatching=true;
                transitionTo(_eLatching);
              }
              else {
                transitionTo(_eDisconnected);
              }
              break;
            }
#endif
            default:
              break;
          }
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::ThrottleTimeAvailable:
    {
      shared_ptr<GetPdnThrottleTimeResponseInd> m =
        std::static_pointer_cast<GetPdnThrottleTimeResponseInd>(event.msg);
      if (m != nullptr) {
        response.call.suggestedRetryTime = m->getThrottleTime();
      }
      transitionTo(_eDisconnected);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        if (m->getType() == DataCallTimerType::SetupDataCall) {
          response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
          transitionTo(_eDisconnected);
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        DsiWrapper::getInstance().dsiStopDataCall(callInfo.dsiHandle);
        response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::CleanUp:
    {
      transitionTo(_eIdle);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DisconnectCallFromOlderDDSSub:
    {
      DsiWrapper::getInstance().dsiStopDataCall(callInfo.dsiHandle);
      response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
      transitionTo(_eIdle);
      return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
