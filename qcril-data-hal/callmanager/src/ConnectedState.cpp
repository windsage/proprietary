/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "event/LinkPropertiesChangedMessage.h"
#include "CallInfo.h"
#include "CallState.h"
#include "CallManager.h"

#include "DSDModemEndPoint.h"

using namespace std;
using namespace SM;
using namespace rildata;

void ConnectedState::enter(void) {
  partialRetryCount = 0;
  dataCallListChanged();
  if (isPartiallyConnected()) {
    startPartialRetryTimerForTemporaryFailure();
  }
}

void ConnectedState::exit(void) {
  stopTimer(DataCallTimerType::PartialRetry);
  stopTimer(DataCallTimerType::PartialRetryResponse);
  stopTimer(DataCallTimerType::IPAddressReconfigure);
  partialRetryCount = 0;
  if((callInfo.pduSessionParams[0].requestProfiles.size() > 0) &&
     (((int)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) & ((int)ApnTypes_t::IMS)))
  {
    deInitializeQos();
  }
}

TimeKeeper::millisec ConnectedState::getTimeoutMillis(unsigned long maxTimeoutMillis) {
  // Timeout value doubles after each retry until MAX is reached
  TimeKeeper::millisec defaultTimeoutMillis = (TimeKeeper::millisec)DEFAULT_PARTIAL_RETRY_TIMEOUT * 1000;
  TimeKeeper::millisec timeoutMillis = defaultTimeoutMillis << partialRetryCount;
  // handle integer overflow
  if (timeoutMillis <= 0 || timeoutMillis > maxTimeoutMillis) {
    timeoutMillis = maxTimeoutMillis;
  }
  return timeoutMillis;
}

bool ConnectedState::isPartiallyConnected() {
  // if dual ip call but not all IPs have connected
  if (callInfo.pduSessionParams[0].requestProfiles.size() > 0)
  {
    return (callInfo.pduSessionParams[0].requestProfiles[0].protocol == "IPV4V6" &&
        (!callInfo.v4Connected || !callInfo.v6Connected));
  }
  return false;
}

void ConnectedState::startPartialRetryTimerForTemporaryFailure() {
  if (!globalInfo.partialRetryEnabled) {
    Log::getInstance().d("Partial retry isn't Enabled!");
    return;
  }
  DataCallFailCause_t consolidatedReason = CallEndReason::getCallEndReason(getVerboseCallEndReason());
  if (!CallEndReason::isPermanentFailure(consolidatedReason)) {
    auto timeoutMillis = getTimeoutMillis(globalInfo.maxPartialRetryTimeout * 1000);
    Log::getInstance().d("partial retry timer started for " +
                         std::to_string((unsigned long)timeoutMillis) + "ms");
    startTimer(DataCallTimerType::PartialRetry, timeoutMillis);
  } else {
    Log::getInstance().d("Skipping partial retry due to permanent CE reason");
  }
}

void ConnectedState::startPartialRetryResponseTimer() {
  TimeKeeper::millisec timeoutMillis = SETUP_DATA_CALL_TIMEOUT_SECONDS * 1000;
  startTimer(DataCallTimerType::PartialRetryResponse, timeoutMillis);
}

void ConnectedState::initiatePartialRetry() {
  dsi_call_param_value_t partialRetry;
  partialRetry.buf_val = nullptr;
  partialRetry.num_val = TRUE;
  if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(callInfo.dsiHandle, DSI_CALL_INFO_PARTIAL_RETRY, &partialRetry)) {
    Log::getInstance().d("Unable to set partial retry param");
    return;
  }
  // this event will only come after temporary failure
  if (DSI_SUCCESS != DsiWrapper::getInstance().dsiStartDataCall(callInfo.dsiHandle)) {
    Log::getInstance().d("dsiStartDataCall failed");
    return;
  }
  startPartialRetryResponseTimer();
  partialRetryCount++;
}

int ConnectedState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestBase> m =
        std::static_pointer_cast<SetupDataCallRequestBase>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                            HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        if ((m->getDataRequestReason() == DataRequestReason_t::HANDOVER) &&
          ((NetType & callInfo.currentRAT) == 0)) {
          callInfo.pduSessionParams[0].request = event.msg;
          // Handover scenario
          Log::getInstance().d("request reason handover with current and preferred network types different");
          Log::getInstance().d("Handover for Apn = ["+m->getApn()+
                          "] from ["+std::to_string((int)callInfo.currentRAT)+
                          "] to ["+std::to_string((int)callInfo.preferredRAT)+"]");
          HandoffNetworkType_t prefSys = callInfo.preferredRAT;

          if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
            auto setup16Msg = std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(event.msg);
            if (setup16Msg->getOptionalTrafficDescriptor().has_value()) {
              for (auto& psp : callInfo.pduSessionParams) {
                for (auto rp : psp.requestProfiles) {
                  if (rp.trafficDescriptor.has_value())
                  {
                    if (rp.trafficDescriptor.value() == setup16Msg->getOptionalTrafficDescriptor().value()) {
                      psp.request = m;
                      if (psp.pendingAck) {
                        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, psp.dnnName, prefSys);
                        psp.pendingAck = false;
                      }
                    }
                  }
                }
              }
            }
            else {
              callInfo.pduSessionParams[0].request = m;
              if (callInfo.pduSessionParams[0].pendingAck) {
                sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), prefSys);
                callInfo.pduSessionParams[0].pendingAck = false;
              }
            }
          } // if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
          else {
            callInfo.pduSessionParams[0].request = m;
            if (callInfo.pduSessionParams[0].pendingAck) {
              sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), prefSys);
              callInfo.pduSessionParams[0].pendingAck = false;
            }
          }
          // release wakelock
          if(m->getAcknowlegeRequestCb() != nullptr) {
            auto cb = *(m->getAcknowlegeRequestCb().get());
            cb(m->getSerial());
          }
          transitionTo(_eHandover);
        } else {
          SetupDataCallResponse_t result = {};
          if ((m->getRequestSource()==RequestSource_t::RADIO &&
              callInfo.currentRAT==HandoffNetworkType_t::_eIWLAN) ||
              (m->getRequestSource()==RequestSource_t::IWLAN &&
              callInfo.currentRAT==HandoffNetworkType_t::_eWWAN)) {
            Log::getInstance().d("Request APN connection is already exist on another transport");
            result.respErr = ResponseError_t::NO_ERROR;
            result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
            result.call.suggestedRetryTime = -1;
          }
          else {
            //Respond with success either when request reason is NORMAL or
            //request reason is HANDOVER and prefNetwork == currentNetwork
            Log::getInstance().d("Respond with existing call because the request reason is "+
                                  std::to_string((int)m->getDataRequestReason())+
                                  ", or the requested preferred network type is "+
                                  std::to_string((int)NetType)+
                                  " while current network type is "+
                                  std::to_string((int)callInfo.currentRAT));

            result.respErr = ResponseError_t::NO_ERROR;
            result.call.cause = DataCallFailCause_t::NONE;
            result.call.suggestedRetryTime = -1;
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
            bool profileExists = false;
            for (auto psp : callInfo.pduSessionParams)
            {
              for (auto rp : psp.requestProfiles)
              {
                if (rp == m->getDataProfileInfo())
                {
                  profileExists = true;
                }
              }
            }
            if (!profileExists)
            {
              callInfo.pduSessionParams[0].requestProfiles.push_back(m->getDataProfileInfo());
              callInfo.callParamsChanged = true;
            }
            for (auto psp : callInfo.pduSessionParams) {
              for (auto rp : psp.requestProfiles) {
                if (rp.trafficDescriptor.has_value())
                {
                  result.call.trafficDescriptors.push_back(rp.trafficDescriptor.value());
                }
              }
            }
          }
          sendSetupDataCallResponse(m, result);
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
        if (m->getDataRequestReason() == DataRequestReason_t::HANDOVER) {
          // Perform bitwise& to set network type to preferred only
          HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
              callInfo.preferredRAT & callInfo.currentRAT);
          callInfo.currentRAT = target;

          sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
          dataCallListChanged();
        } else {
          DsiWrapper::getInstance().dsiStopDataCall(callInfo.dsiHandle);
          callInfo.pduSessionParams[0].request = m;
          transitionTo(_eDisconnecting);
        }
        //release wakelock
        if(m->getAcknowlegeRequestCb() != nullptr) {
          auto cb = *(m->getAcknowlegeRequestCb().get());
          cb(m->getSerial());
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
          case DSI_EVT_NET_IS_CONN:
            stopTimer(DataCallTimerType::PartialRetryResponse);
            partialRetryCount = 0;
            configureParams();
            callInfo.pduInfoList.front().active = _eActivePhysicalLinkUp;
            callInfo.calculateConsolidatedActive();
            if (isPartiallyConnected()) {
              startPartialRetryTimerForTemporaryFailure();
            }

            if( (!(callInfo.pduInfoList.front().v4Info) &&
                 !(callInfo.pduInfoList.front().v6Info)) ||
                (callInfo.pduInfoList.front().v4Info->addrMap.empty() &&
                 callInfo.pduInfoList.front().v6Info->addrMap.empty()) ) {
              callInfo.callParamsChanged = false;
              Log::getInstance().d("No addresses received from DSI layer. Starting Timer");
              startTimer(DataCallTimerType::IPAddressReconfigure, (TimeKeeper::millisec)IP_ADDRESS_RECONFIGURE_TIMEOUT_SECONDS * 1000);
            } else {
              Log::getInstance().d("Atleast one address received from DSI layer. Stopping timer if any");
              stopTimer(DataCallTimerType::IPAddressReconfigure);
            }
            break;

          case DSI_EVT_NET_PARTIAL_CONN:
            // partial retry failure case
            startPartialRetryTimerForTemporaryFailure();
            break;

          //Need to support this event for LE
          case DSI_EVT_NET_NEWMTU:
            configureMtu(0);
            break;

          //Legacy events still have to be handled for LE
          case DSI_EVT_NET_NEWADDR: [[fallthrough]];
          case DSI_EVT_NET_RECONFIGURED:
          case DSI_EVT_NET_DELADDR:
#ifndef RIL_FOR_MDM_LE
          case DSI_EVT_PDU_RECONFIGED:
#endif
            // once both IPs have connected, stop partial retry attempts
            callInfo.callParamsChanged = true;
            configureParams();

            if( (!(callInfo.pduInfoList.front().v4Info) &&
                !(callInfo.pduInfoList.front().v6Info)) ||
                (callInfo.pduInfoList.front().v4Info->addrMap.empty() &&
                 callInfo.pduInfoList.front().v6Info->addrMap.empty()) ) {
              callInfo.callParamsChanged = false;
              Log::getInstance().d("No addresses received from DSI layer. Starting Timer");
              startTimer(DataCallTimerType::IPAddressReconfigure, (TimeKeeper::millisec)IP_ADDRESS_RECONFIGURE_TIMEOUT_SECONDS * 1000);
            } else {
              Log::getInstance().d("Atleast one address received from DSI layer. Stopping timer if any");
              stopTimer(DataCallTimerType::IPAddressReconfigure);
            }
            if (!isPartiallyConnected()) {
              Log::getInstance().d("Dual ip connection established");
              stopTimer(DataCallTimerType::PartialRetry);
              stopTimer(DataCallTimerType::PartialRetryResponse);
              partialRetryCount = 0;
              createQmiWdsClients();
            }
            else if (isPartiallyConnected()) {
              startPartialRetryTimerForTemporaryFailure();
            }
            break;

          case DSI_EVT_NET_NO_NET:
            callInfo.consolidatedActive = _eInactive;
            callInfo.v4Connected = false;
            callInfo.v6Connected = false;
            callInfo.cause = CallEndReason::getCallEndReason(getVerboseCallEndReason());
            callInfo.callParamsChanged = true;
            transitionTo(_eDisconnected);
            break;
#ifndef RIL_FOR_MDM_LE
          case DSI_EVT_PDU_AVAILABLE:
            if (callInfo.pduInfoList.size() == 1)  {
              Log::getInstance().d("Adding pdu info with id " +
                                   std::to_string(pEvtData->payload.pdu_id));
              PduInfo secPduInfo;
              secPduInfo.pduId = pEvtData->payload.pdu_id;
              callInfo.pduInfoList.push_back(secPduInfo);
            }
            else {
              Log::getInstance().d("Invalid pdu list length " +
                                   std::to_string(callInfo.pduInfoList.size()));
            }
            break;
          case DSI_EVT_PDU_PROMOTION:
            if (callInfo.pduInfoList.size() == 2)  {
              Log::getInstance().d("Promoting pdu with id " +
                                   std::to_string(callInfo.pduInfoList.back().pduId));
              callInfo.pduInfoList.pop_front();
              //Item at the front is now the new primary, set the pdu id to 0
              callInfo.pduInfoList.front().pduId = 0;
              if (callInfo.wds_v4_call_endpoint != nullptr) {
                callInfo.wds_v4_call_endpoint->queryNewPrimaryPduInfo();
              }
              if (callInfo.wds_v6_call_endpoint != nullptr) {
                callInfo.wds_v6_call_endpoint->queryNewPrimaryPduInfo();
              }

            }
            else {
              Log::getInstance().d("Invalid pdu list length " +
                                   std::to_string(callInfo.pduInfoList.size()));
            }
            break;

          case DSI_EVT_PDU_RELEASE:
            if (callInfo.pduInfoList.size() == 2)  {
              Log::getInstance().d("Releasing pdu with id " +
                                   std::to_string(callInfo.pduInfoList.back().pduId));
              callInfo.pduInfoList.pop_back();
            }
            else {
              Log::getInstance().d("Invalid pdu list length " +
                                   std::to_string(callInfo.pduInfoList.size()));
            }
            break;

          case DSI_EXTENDED_IP_CHANGE_EV:
            if (pEvtData->payload.ext_ip_changes == DSI_EXT_IP_PCSF_SERVER_ADDR_LIST) {
              configurePcscfAddr(pEvtData->payload.pdu_id);
            }
            else if (pEvtData->payload.ext_ip_changes == DSI_EXT_IP_MTU) {
              configureMtu(pEvtData->payload.pdu_id);
            }
            break;
#endif
          default:
            break;
        }
      }
      dataCallListChanged();
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      std::shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        switch (m->getType()) {
          case DataCallTimerType::PartialRetry:
          {
            initiatePartialRetry();
            break;
          }
          case DataCallTimerType::PartialRetryResponse:
            startPartialRetryTimerForTemporaryFailure();
            break;
          case DataCallTimerType::IPAddressReconfigure:
          {
            Log::getInstance().d(" IPAddressReconfigure timer expired. No DSI addresses received.Tearing down the call");
            DsiWrapper::getInstance().dsiStopDataCall(callInfo.dsiHandle);
            transitionTo(_eDisconnecting);
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
    case CallEventTypeEnum::HandoverInformationInd:
    {
      std::shared_ptr<HandoverInformationIndMessage> m =
        std::static_pointer_cast<HandoverInformationIndMessage>(event.msg);
      if (m != nullptr) {
        switch (m->getState()) {
          case HandoffState_t::Init:
          {
            if (m->hasPreferredNetwork() && m->getPreferredNetwork() != callInfo.currentRAT) {
              callInfo.preferredRAT = m->getPreferredNetwork();
              transitionTo(_eHandover);
            }
            break;
          }
          case HandoffState_t::Success:
          {
            //Out of band handoff success received in Connected state
            //Update RATs just to be safe and fix some race conditions
            if (m->hasPreferredNetwork()) {
              if (m->getPreferredNetwork() != callInfo.currentRAT) {
                Log::getInstance().d("current RAT changed from " +
                std::to_string(callInfo.currentRAT) + " to " +
                std::to_string(m->getPreferredNetwork()));
                callInfo.currentRAT = m->getPreferredNetwork();
                callInfo.preferredRAT = m->getPreferredNetwork();
                callInfo.callParamsChanged = true;
                dataCallListChanged();
              }
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
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        // If RAT is same as current, or there is already handover pending for that RAT
        if (m->getPrefNetwork() == callInfo.currentRAT ||
            m->getPrefNetwork() == callInfo.preferredRAT) {
          sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
        } else {
          callInfo.preferredRAT = m->getPrefNetwork();
          if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
            if(m->getDnnName().has_value()) {
              for (auto& psp : callInfo.pduSessionParams) {
                if(psp.dnnName == m->getDnnName().value()) {
                  psp.pendingAck = true;
                  break;
                }
              }
            }
            else {
              callInfo.pduSessionParams[0].pendingAck = true;
            }
          }
          else {
            callInfo.pduSessionParams[0].pendingAck = true;
          }
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::LinkPropertiesChanged:
    {
      std::shared_ptr<LinkPropertiesChangedMessage> m = std::static_pointer_cast<LinkPropertiesChangedMessage>(event.msg);
      if(m != nullptr) {
        switch (m->getLinkPropertiesChanged()) {
          //Still need to handle pcscf address change from wds for LE
          case LinkPropertiesChangedType_t::PcscfAddressChanged:
          {
            configurePcscfAddr(0);
            break;
          }
          case LinkPropertiesChangedType_t::LinkActiveStateChanged:
          {
            if (m->hasActiveState() && globalInfo.linkStateChangeReport) {
              if (m->hasIsSecPdu() && (m->getIsSecPdu())) {
                if (callInfo.pduInfoList.size() == 2) {
                  if (callInfo.pduInfoList.back().active != m->getActiveState()) {
                callInfo.callParamsChanged = true;
                    callInfo.pduInfoList.back().active = m->getActiveState();
                    callInfo.calculateConsolidatedActive();
                  }
                }
              }
              else {
                if (callInfo.pduInfoList.front().active != m->getActiveState()) {
                    callInfo.callParamsChanged = true;
                    callInfo.pduInfoList.front().active = m->getActiveState();
                    callInfo.calculateConsolidatedActive();
                  }
              }
            }
            break;
          }
          case LinkPropertiesChangedType_t::QosDataReceived:
          {
            callInfo.qosSessions = m->getQosSessions();
            callInfo.callParamsChanged = true;
          }
          break;
          default:
            break;
        }
      }
      dataCallListChanged();
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::CleanUp:
    {
        transitionTo(_eDisconnected);
        return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DisconnectAllTDs:
    {
        transitionTo(_eDisconnected);
        return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
