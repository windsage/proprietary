/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "MessageCommon.h"
#include "framework/Log.h"
#include "CallState.h"
#include "CallManager.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "UnSolMessages/InformHandoverFailureMessage.h"

using namespace std;
using namespace SM;
using namespace rildata;

void HandoverState::enter(void) {
  for (auto& psp : callInfo.pduSessionParams) {
    if (psp.pendingAck) {
      psp.pendingAck = false;
    }
  }
  startTimer(DataCallTimerType::Handover, (TimeKeeper::millisec)HANDOVER_TIMEOUT_SECONDS * 1000);
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
  if( callInfo.v4Connected )
    callInfo.v4HandedOver = HandOverState::Pending;
  if( callInfo.v6Connected )
    callInfo.v6HandedOver = HandOverState::Pending;
  if( callInfo.v4Connected && callInfo.v6Connected )
    callInfo.dualIpHandover = true;
}

void HandoverState::exit(void) {
  stopTimer(DataCallTimerType::Handover);
  response.call.suggestedRetryTime = -1;
  if (response.call.cause == DataCallFailCause_t::NONE) {
    response.call.cid = callInfo.cid;
    response.call.active = callInfo.consolidatedActive;
    response.call.type = callInfo.ipType;
    response.call.ifname = callInfo.deviceName;
    response.call.addresses = callInfo.convertToLegacyLinkAddresses();
    response.call.linkAddresses = callInfo.convertAddrStringToLinkAddresses();
    response.call.dnses = callInfo.convertToResultDnsAddresses();
    response.call.gateways = callInfo.pduInfoList.front().concatGatewayAddress();
    response.call.pcscf = callInfo.pduInfoList.front().concatPcscfAddress();
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
  }
  setupDataCallCompleted(response);
}

int HandoverState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestBase> m =
        std::static_pointer_cast<SetupDataCallRequestBase>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                             HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        SetupDataCallResponse_t result = {};
        result.respErr = ResponseError_t::NO_ERROR;
        result.call.suggestedRetryTime = -1;
        if (m->getDataRequestReason() == DataRequestReason_t::HANDOVER) {
          if (NetType == callInfo.currentRAT) {
            result.call.cause = DataCallFailCause_t::NONE;
            result.call.cid = callInfo.cid;
            result.call.active = callInfo.consolidatedActive;
            result.call.type = callInfo.ipType;
            result.call.ifname = callInfo.deviceName;
            result.call.addresses = callInfo.convertToLegacyLinkAddresses();
            result.call.linkAddresses = callInfo.convertAddrStringToLinkAddresses();
            result.call.dnses = callInfo.convertToResultDnsAddresses();
            result.call.gateways = callInfo.pduInfoList.front().concatGatewayAddress();
            result.call.pcscf = callInfo.pduInfoList.front().concatPcscfAddress();
            result.call.mtu = callInfo.mtu;
            result.call.mtuV4 = callInfo.consolidatedMtuV4;
            result.call.mtuV6 = callInfo.consolidatedMtuV6;
            if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
              for (auto psp : callInfo.pduSessionParams) {
                for (auto rp : psp.requestProfiles) {
                  if (rp.trafficDescriptor.has_value())
                  {
                    result.call.trafficDescriptors.push_back(rp.trafficDescriptor.value());
                  }
                }
              }
            }
          } else {
            /*store the request when there is any delay from the framework in sending
              setupData handoff when modem has already started the HO*/
            if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
              auto setup16Msg = std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(event.msg);
              if (setup16Msg->getOptionalTrafficDescriptor().has_value()) {
                for (auto& psp : callInfo.pduSessionParams) {
                  for (auto rp : psp.requestProfiles) {
                    if (rp.trafficDescriptor.has_value())
                    {
                      if (rp.trafficDescriptor.value() == setup16Msg->getOptionalTrafficDescriptor().value()) {
                        psp.request = setup16Msg;
                        if (psp.pendingAck) {
                          sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, psp.dnnName, callInfo.preferredRAT);
                          psp.pendingAck = false;
                        }
                        return IState<CallEventType>::HANDLED;
                      }
                    }
                  }
                }
              }
              else if (callInfo.pduSessionParams[0].request == nullptr) {
                callInfo.pduSessionParams[0].request = event.msg;
                return IState<CallEventType>::HANDLED;
              } else {
                result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
              }
            } // if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
            else {
              if (callInfo.pduSessionParams[0].request == nullptr) {
                callInfo.pduSessionParams[0].request = event.msg;
                return IState<CallEventType>::HANDLED;
              } else {
                result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
              }
            }
          }
        } else if (m->getDataRequestReason() == DataRequestReason_t::NORMAL) {
          result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_1;
        }
        sendSetupDataCallResponse(m, result);
        if(result.call.cause == DataCallFailCause_t::NONE) {
          // release wakelock
          if(m->getAcknowlegeRequestCb() != nullptr) {
            auto cb = *(m->getAcknowlegeRequestCb().get());
            cb(m->getSerial());
          }
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      std::shared_ptr<DeactivateDataCallRequestMessage> m =
        std::static_pointer_cast<DeactivateDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        if (m->getDataRequestReason() == DataRequestReason_t::NORMAL) {
          DsiWrapper::getInstance().dsiStopDataCall(callInfo.dsiHandle);
          response.respErr = ResponseError_t::INTERNAL_ERROR;
          response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
          transitionTo(_eDisconnecting);
          callInfo.pduSessionParams[0].request = m;
        } else {
          sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::HandoverInformationInd:
    {
      std::shared_ptr<HandoverInformationIndMessage> m =
        std::static_pointer_cast<HandoverInformationIndMessage>(event.msg);
      if (m != nullptr) {
        dsi_ce_reason_t dsiReason = {
          .reason_type = static_cast<dsi_ce_reason_type_e>(CallEndReason::AP_ASSIST_HANDOVER),
          .reason_code = static_cast<int>(ApAssistHandoverFailCause_t::HANDOFF_FAILURE)
        };
        switch (m->getState()) {
          case HandoffState_t::Failure:
          {
            if(callInfo.dualIpHandover) {
              if (m->getIpType() == IpFamilyType_t::IPv4) {
                callInfo.v4HandedOver = HandOverState::Failure;
              } else if (m->getIpType() == IpFamilyType_t::IPv6) {
                callInfo.v6HandedOver = HandOverState::Failure;
              }
            }
            if( DUAL_IP_HO_BOTH_IP_HO_FAILURE || !callInfo.dualIpHandover ) {
              //If HO fails on both IP types, we send failure reason & set SRAT & move to Connected State
              if (m->hasFailReason())
              {
                auto reason = m->getFailReason();
                dsiReason = {
                    .reason_type = static_cast<dsi_ce_reason_type_e>(reason.failure_reason_type),
                    .reason_code = reason.failure_reason};
              }
              response.call.cause = CallEndReason::getCallEndReason(dsiReason);
              //Modem sends these errors when it can't fallback to srat after HO failure.
              //So App should change the RAT to target even if Ho is failed.
              if(response.call.cause == DataCallFailCause_t::SLICE_REJECTED ||
                 response.call.cause == DataCallFailCause_t::PDN_CONN_DOES_NOT_EXIST ||
                 response.call.cause == DataCallFailCause_t::IPV6_PREFIX_UNAVAILABLE)
              {
                response.call.handoverFailureMode = HandoverFailureMode_t::NoFallbackRetrySetupNormal;
                HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
                  callInfo.preferredRAT | callInfo.currentRAT);
                callInfo.currentRAT = target;
              }
              else
              {
                callInfo.preferredRAT = callInfo.currentRAT;
                if (response.call.cause == DataCallFailCause_t::HANDOFF_PREFERENCE_CHANGED) {
                  response.call.handoverFailureMode = HandoverFailureMode_t::Legacy;
                } else {
                  response.call.handoverFailureMode = HandoverFailureMode_t::NoFallbackRetryHandover;
                }
                auto msg = std::make_shared<InformHandoverFailureMessage>(callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap,
                                                                          callInfo.pduSessionParams[0].requestProfiles[0].apn);
                msg->broadcast();
              }
              configureParams();
              transitionTo(_eConnected);
            }
            else if ( DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS )
            {
              //If HO fails on only one of the IP type, then we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              setCurrentRATAndConfigureParams(m);
              transitionTo(_eConnected);
            }
            break;
          }
          case HandoffState_t::PrefSysChangedFailure:
          {
            dsiReason.reason_code = static_cast<int>(ApAssistHandoverFailCause_t::APN_PREF_SYSTEM_CHANGE_REQUEST_FAILURE);
            callInfo.preferredRAT = callInfo.currentRAT;
            response.call.cause = CallEndReason::getCallEndReason(dsiReason);
            configureParams();
            transitionTo(_eConnected);
            break;
          }
          case HandoffState_t::Success:
          {
            if (m->getIpType() == IpFamilyType_t::IPv4) {
              callInfo.v4HandedOver = HandOverState::Success;
            } else if (m->getIpType() == IpFamilyType_t::IPv6) {
              callInfo.v6HandedOver = HandOverState::Success;
            }
            if(!callInfo.dualIpHandover || DUAL_IP_HO_BOTH_IP_HO_SUCCESS) {
              //If HO is successful on both IP types, we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              if ((callInfo.currentRAT == HandoffNetworkType_t::_eIWLAN && callInfo.preferredRAT == HandoffNetworkType_t::_eCIWLAN) ||
                 (callInfo.currentRAT == HandoffNetworkType_t::_eCIWLAN && callInfo.preferredRAT == HandoffNetworkType_t::_eIWLAN)) {
                callInfo.currentRAT = callInfo.preferredRAT;
              }
              else {
                //Use preferred RAT from the indication to fix certain race conditions, eg
                //1. Intent was sent just before setupData was received and is acked immediately by CallManager,
                //   Modem start HO
                //2. Telephony sends setupData on old RAT since it didnt receive QualifiedNetworks change yet,
                //   currentRAT and preferredRAT are set to old RAT in CallInfo
                //3. Call is connected, Telephony immediately starts HO. Pref RAT is not changed here
                //4. HO is completed. Current RAT is set to preferred RAT, however preferred RAT was never
                //   changed to target RAT due to missed intent and no HO init either. Hence current RAT
                //   remains unchanged
                if (m->hasPreferredNetwork()) {
                  callInfo.preferredRAT = m->getPreferredNetwork();
                }
                HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
                    callInfo.preferredRAT | callInfo.currentRAT);
                callInfo.currentRAT = target;
                configureParams();
              }
              transitionTo(_eConnected);
            } else if (DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS) {
              //If HO fails on only one of the IP type, then we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              setCurrentRATAndConfigureParams(m);
              transitionTo(_eConnected);
            }
            break;
          }
          default:
            break;
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      std::shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        switch (m->getType()) {
          case DataCallTimerType::Handover:
          {
            response.call.cause = DataCallFailCause_t::NONE;
            HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
                  callInfo.preferredRAT | callInfo.currentRAT);

            if( callInfo.dualIpHandover && (callInfo.v6HandedOver == HandOverState::Success) ) {
              callInfo.currentRAT = target;
            }
            else if ( callInfo.dualIpHandover && (callInfo.v4HandedOver == HandOverState::Success) ) {
              callInfo.currentRAT = target;
            } else {
              callInfo.preferredRAT = callInfo.currentRAT;
              response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
            }

            configureParams();
            transitionTo(_eConnected);
            break;
          }
          default:
            break;
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      shared_ptr<RilEventDataCallback> m =
        std::static_pointer_cast<RilEventDataCallback>(event.msg);
      if (m != nullptr) {
        EventDataType *pEvtData = m->getEventData();
        dsi_net_evt_t netEvent = pEvtData->evt;
        switch (netEvent) {
          case DSI_EVT_NET_NO_NET:
            callInfo.consolidatedActive = _eInactive;
            callInfo.v4Connected = false;
            callInfo.v6Connected = false;
            response.call.cause = CallEndReason::getCallEndReason(getVerboseCallEndReason());
            callInfo.cause = response.call.cause;
            transitionTo(_eDisconnected);
            dataCallListChanged();
            break;
          case DSI_EVT_NET_DELADDR:
#ifndef RIL_FOR_MDM_LE
          case DSI_EVT_PDU_RECONFIGED:
#endif
            configureParams();
            break;
          default:
            break;
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
        // If there is already handover pending for the other RAT
        if (m->getPrefNetwork() == callInfo.currentRAT) {
            callInfo.preferredRAT = callInfo.currentRAT;
            response.call.cause = DataCallFailCause_t::HANDOFF_PREFERENCE_CHANGED;
            response.call.handoverFailureMode = HandoverFailureMode_t::Legacy;
            configureParams();
            transitionTo(_eConnected);
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::CleanUp:
    {
        transitionTo(_eDisconnected);
        return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
