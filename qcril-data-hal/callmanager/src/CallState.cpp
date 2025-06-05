/******************************************************************************
#  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "CallState.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "MessageCommon.h"
#include "QdpAdapter.h"
#include "Util.h"
#include "qcril_data.h"
#include "UnSolMessages/QosInitializeMessage.h"
#include "UnSolMessages/GetPdnThrottleTimeResponseInd.h"

extern "C" {
#include "qmi_platform.h"
}

using namespace rildata;
using namespace qdp;

void CallState::enter() {
}

void CallState::exit() {
}

CallState::~CallState() {
}

void CallState::stopTimer(DataCallTimerType type) {
  if (callInfo.timers.find(type) != callInfo.timers.end()) {
    TimeKeeper::timer_id timer = callInfo.timers[type];
    if (timer != TimeKeeper::no_timer) {
      TimeKeeper::getInstance().clear_timer(timer);
      callInfo.timers.erase(type);
    }
  }
}

void CallState::timeoutHandler(DataCallTimerType type, void *data) {
  std::ignore = data;
  auto timeoutMsg = std::make_shared<DataCallTimerExpiredMessage>(callInfo.cid, type);
  if (timeoutMsg != nullptr) {
    timeoutMsg->broadcast();
    string logMsg = "[" + getName() + "]: Timer expired for " +
                        DataCallTimerExpiredMessage::getStringType(type);
    Log::getInstance().d(logMsg);
  }
}

void CallState::startTimer(DataCallTimerType type, TimeKeeper::millisec timeout) {
  // if timer exists, restart it with the new timeout
  auto handler = bind(&CallState::timeoutHandler, this, type, std::placeholders::_1);
  if( (type == DataCallTimerType::IPAddressReconfigure) && (callInfo.timers.find(type) != callInfo.timers.end())) {
    TimeKeeper::timer_id timer = callInfo.timers[type];
    if (timer != TimeKeeper::no_timer) {
      return;//timer exists
    }
  } else if( type != DataCallTimerType::IPAddressReconfigure) {
    stopTimer(type);
  }
  callInfo.timers[type] = TimeKeeper::getInstance().set_timer(handler, nullptr, timeout);
  string logMsg = "[" + getName() + "]: Timer started for " +
                        DataCallTimerExpiredMessage::getStringType(type) + " for " +
                        to_string(static_cast<long>(timeout)) + "ms";
  Log::getInstance().d(logMsg);
  logBuffer.addLogWithTimestamp(logMsg);
}

DataCallResult_t CallState::trimDataCallResult(const DataCallResult_t& r) {
  // removes 1.6 parameters from the data call result
  DataCallResult_t trimmed = r;
  for (LinkAddress_t& la : trimmed.linkAddresses) {
    la.properties= AddressProperty_t::NONE;
    la.deprecationTime = MAX_LINX_ADDRESS_TIMER_VAL;
    la.expirationTime = MAX_LINX_ADDRESS_TIMER_VAL;
  }
  trimmed.suggestedRetryTime = -1;
  trimmed.qosSessions = {};
  trimmed.handoverFailureMode = HandoverFailureMode_t::Legacy;
  trimmed.trafficDescriptors = {};
  trimmed.defaultQos.nrQos = {};
  trimmed.defaultQos.epsQos = {};
  return trimmed;
};

void CallState::sendSetupDataCallResponse(std::shared_ptr<SetupDataCallRequestBase> msg, const SetupDataCallResponse_t& result)
{
  if (msg != nullptr) {
    std::shared_ptr<SetupDataCallResponse_t> resp;
    if(result.respErr == ResponseError_t::NO_ERROR && globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_4) {
      SetupDataCallResponse_t trimmedResult = { ResponseError_t::NO_ERROR, trimDataCallResult(result.call) };
      resp = std::make_shared<SetupDataCallResponse_t>(trimmedResult);
    } else {
      resp = std::make_shared<SetupDataCallResponse_t>(result);
    }
    stringstream ss;
    ss << "[" << getName() << "]: " << msg->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  }
  else {
    Log::getInstance().d("[" + getName() + "]: Corrupted SetupDataCallRequestMessage - not able to send response");
  }
}

void CallState::sendDeactivateDataCallResponse(std::shared_ptr<DeactivateDataCallRequestMessage> msg, const ResponseError_t& result)
{
  if (msg != nullptr) {
    auto resp = std::make_shared<ResponseError_t>(result);
    stringstream ss;
    ss << "[" << getName() << "]: " << msg->getSerial() << "< deactivateDataCallResponse resp=" << (int)result << " cid=" << callInfo.cid;
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  }
  else {
    Log::getInstance().d("[" + getName() + "]: Corrupted DeactivateDataCallRequestMessage - not able to send response");
  }
}

void CallState::setupDataCallCompleted(const SetupDataCallResponse_t& result)
{
  Log::getInstance().d("[" + getName() + "]: setupDataCallCompleted = "+std::to_string((int)result.respErr));

  if(callInfo.pduSessionParams[0].request != nullptr) {
    Log::getInstance().d("[" + getName() + "]: found message = " + callInfo.pduSessionParams[0].request->get_message_name());
    std::shared_ptr<SetupDataCallRequestBase> base =
      std::static_pointer_cast<SetupDataCallRequestBase>(callInfo.pduSessionParams[0].request);
    if(base != nullptr) {
      sendSetupDataCallResponse(base, result);
      if(result.respErr == ResponseError_t::NO_ERROR) {
          callInfo.callParamsChanged = true;
      } else {
          Log::getInstance().d("[" + getName() + "]: setupdatacallResp failed");
          Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
          //release wakelock
          if(base->getAcknowlegeRequestCb() != nullptr) {
            auto cb = *(base->getAcknowlegeRequestCb().get());
            cb(base->getSerial());
          }
      }
      callInfo.pduSessionParams[0].request = nullptr;
    }
  }
  else {
    Log::getInstance().d("[" + getName() + "]: not found");
  }

  for (auto psp : callInfo.pduSessionParams) {
    if (psp.request != nullptr) {
      std::shared_ptr<SetupDataCallRequestBase> ms = std::static_pointer_cast<SetupDataCallRequestBase>(psp.request);
      if(ms != nullptr) {
        sendSetupDataCallResponse(ms, result);
        if(result.respErr == ResponseError_t::NO_ERROR) {
          callInfo.callParamsChanged = true;
        } else {
          Log::getInstance().d("[" + getName() + "]: setupdatacallResp failed");
          Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
          //release wakelock
          if(ms->getAcknowlegeRequestCb() != nullptr) {
            auto cb = *(ms->getAcknowlegeRequestCb().get());
            cb(ms->getSerial());
          }
        }
      }
    }
  }
}

void CallState::setupDataCallFailed(const ResponseError_t& respErr, const DataCallFailCause_t& cause) {
  SetupDataCallResponse_t result = {};
  result.respErr = respErr;
  result.call.suggestedRetryTime = -1;
  result.call.cause = cause;
  result.call.cid = -1;
  result.call.active = _eInactive;
  result.call.type = "";
  result.call.ifname = "";
  result.call.addresses = "";
  result.call.dnses = "";
  result.call.gateways = "";
  result.call.pcscf = "";
  result.call.mtu = 0;
  setupDataCallCompleted(result);
}

void CallState::deactivateDataCallCompleted(const ResponseError_t& result)
{
  Log::getInstance().d("[" + getName() + "]: deactivateDataCallCompleted = "+std::to_string((int)result));

  if(callInfo.pduSessionParams[0].request != nullptr) {
    Log::getInstance().d("[" + getName() + "]: found message = " + callInfo.pduSessionParams[0].request->get_message_name());
    std::shared_ptr<DeactivateDataCallRequestMessage> ms = std::static_pointer_cast<DeactivateDataCallRequestMessage>(callInfo.pduSessionParams[0].request);
    sendDeactivateDataCallResponse(ms, result);
    if(result != ResponseError_t::NO_ERROR)
    {
      Log::getInstance().d("[" + getName() + "]: DeactivatedatacallResp failed");

      if( ms != nullptr)
      {
        Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
        //release wakelock
        if(ms->getAcknowlegeRequestCb() != nullptr) {
          auto cb = *(ms->getAcknowlegeRequestCb().get());
          cb(ms->getSerial());
        }
      }
    }
    callInfo.pduSessionParams[0].request = nullptr;
  }
  else {
    Log::getInstance().d("[" + getName() + "]: not found");
  }
}

std::string CallState::getCallApn() {
  std::optional<ProfileParams> profile;
  if (callInfo.pduSessionParams[0].profileKey3gpp != nullptr) {
    profile = Manager::getInstance().getProfile(callInfo.pduSessionParams[0].profileKey3gpp);
  } else if (callInfo.pduSessionParams[0].profileKey3gpp2 != nullptr) {
    profile = Manager::getInstance().getProfile(callInfo.pduSessionParams[0].profileKey3gpp2);
  }
  #ifdef QMI_RIL_UTF
  return callInfo.pduSessionParams[0].dnnName;
  #endif
  if (profile) {
    return profile->getApnName();
  }
  return "";
}

void CallState::lookupModemProfiles() {
  ProfileParams inputParams = convertDataProfileInfoToProfileParams(callInfo.pduSessionParams[0].requestProfiles[0]);
  inputParams.setApnName(callInfo.pduSessionParams[0].dnnName);
  if (isEmergencyCall()) {
    callInfo.pduSessionParams[0].profileKey3gpp = util.findOrCreateProfileForEmergencyDataCall(inputParams);
  } else {
    callInfo.pduSessionParams[0].profileKey3gpp = util.findOrCreateProfileForSetupDataCall(TechType::THREE_GPP, inputParams);
#ifndef RIL_FOR_MODEM_LITE
    callInfo.pduSessionParams[0].profileKey3gpp2 = util.findOrCreateProfileForSetupDataCall(TechType::THREE_GPP2, inputParams);
#endif
  }
}

bool CallState::isEmergencyCall() {
  if (callInfo.pduSessionParams[0].requestProfiles.size() > 0)
  {
    return callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap == ApnTypes_t::EMERGENCY;
  }
  return false;
}

dsi_ce_reason_t CallState::pickTemporaryCallEndReason(dsi_ce_reason_t v4Reason, dsi_ce_reason_t v6Reason) {
  DataCallFailCause_t v4RilReason = CallEndReason::getCallEndReason(v4Reason);
  if (!CallEndReason::isPermanentFailure(v4RilReason)) {
    return v4Reason;
  } else {
    return v6Reason;
  }
}

dsi_ce_reason_t CallState::getVerboseCallEndReason() {
  dsi_ce_reason_t dsiEndReason = {DSI_CE_TYPE_INVALID, WDS_VCER_MIP_HA_REASON_UNKNOWN_V01};
  std::string currentIpType = "IP";
  if (callInfo.pduSessionParams[0].requestProfiles.size() > 0)
  {
    if (globalInfo.isRoaming)
    {
      currentIpType = callInfo.pduSessionParams[0].requestProfiles[0].roamingProtocol;
    } else {
      currentIpType = callInfo.pduSessionParams[0].requestProfiles[0].homeProtocol;
    }
  }

  // do not query for an IP type if it is already connected
  bool v4ReasonNeeded = false;
  bool v6ReasonNeeded = false;
  if (currentIpType == "IPV6" || currentIpType == "IPV4V6") {
    v6ReasonNeeded = !callInfo.v6Connected;
  }
  if (currentIpType == "IP" || currentIpType == "IPV4V6") {
    v4ReasonNeeded = !callInfo.v4Connected;
  }

  if (v4ReasonNeeded && v6ReasonNeeded) {
    dsi_ce_reason_t v4Reason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IP");
    dsi_ce_reason_t v6Reason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IPV6");
    dsiEndReason = pickTemporaryCallEndReason(v4Reason, v6Reason);
  } else if (v4ReasonNeeded) {
    dsiEndReason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IP");
  } else if (v6ReasonNeeded) {
    dsiEndReason = DsiWrapper::getInstance().dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IPV6");
  }
  return dsiEndReason;
}

bool CallState::setEmergencyProfileInfo() {
  Log::getInstance().d("[CallInfo]: setEmergencyProfileInfo");

  // getting dsi handle
  dsi_hndl_t dsiHandle = nullptr;
  if (isDsiex2Supported) {
    if((dsiHandle = DsiWrapper::getInstance().dsiGetDataSrvcHndl(CallState::dsiNetCallback2, DSI_MODE_GENERAL_EX, &callInfo)) == nullptr) {
      Log::getInstance().d("Unable to get dsi handle");
      return false;
    }
  } else {
    if((dsiHandle = DsiWrapper::getInstance().dsiGetDataSrvcHndl(CallState::dsiNetCallback, &callInfo)) == nullptr) {
      Log::getInstance().d("Unable to get dsi handle");
      return false;
    }
  }

  // store dsiHandle into call instance
  callInfo.dsiHandle = dsiHandle;
  lookupModemProfiles();

  #ifdef QMI_RIL_UTF
    dsi_call_param_value_t utf_apn;
    string apn_name = callInfo.pduSessionParams[0].dnnName;
    utf_apn.buf_val = (char *)(apn_name.c_str());
    utf_apn.num_val = apn_name.length();
    DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_APN_NAME, &utf_apn);
  #endif

  if((int)callInfo.callBringUpCapability & (int)BringUpCapability::BRING_UP_APN_TYPE) {
    /* set DSI bringup mode as apn-type based bringup */
    wds_apn_type_mask_v01 wdsApnTypeMask;
    wdsApnTypeMask =
      WDSModemEndPointModule::convertToApnTypeMask((int32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap);
    Log::getInstance().d("Bringup using apn type mask = " +std::to_string((int)wdsApnTypeMask));
    dsi_call_param_value_t bringUpMode;
    bringUpMode.buf_val = nullptr;
    bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_TYPE_MASK;
    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                     DSI_CALL_INFO_CALL_BRINGUP_MODE,
                                                                     &bringUpMode)) {
      Log::getInstance().d("Unable to set bringup mode");
      return false;
    }
    /* set apnTypeBitmask */
    dsi_call_param_value_t apnTypeMask;
    apnTypeMask.buf_val = (char*)&wdsApnTypeMask;
    apnTypeMask.num_val = (int)sizeof(wds_apn_type_mask_v01);
    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                     DSI_CALL_INFO_APN_TYPE_MASK,
                                                                     &apnTypeMask)) {
      Log::getInstance().d("Unable to set bringup apn type mask");
      return false;
    }
  } else {
    dsi_call_param_value_t dsi_param;
    if(callInfo.pduSessionParams[0].profileKey3gpp != nullptr) {
      dsi_param.buf_val = nullptr;
      dsi_param.num_val = callInfo.pduSessionParams[0].profileKey3gpp->getProfileId();
      DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &dsi_param);
    } else if(callInfo.pduSessionParams[0].profileKey3gpp2 != nullptr) {
      dsi_param.buf_val = nullptr;
      dsi_param.num_val = callInfo.pduSessionParams[0].profileKey3gpp2->getProfileId();
      DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &dsi_param);
    } else {
      Log::getInstance().d("Failed to get emergency profile");
      return false;
    }
  }

  // set IP family
  if(!callInfo.pduSessionParams[0].requestProfiles[0].protocol.empty()) {
    Log::getInstance().d("[qdp] protocol = " + callInfo.pduSessionParams[0].requestProfiles[0].protocol);
    dsi_call_param_value_t ipFamily;
    ipFamily.buf_val = nullptr;
    ipFamily.num_val = (callInfo.pduSessionParams[0].requestProfiles[0].protocol.compare(string("IP"))==0)?DSI_IP_VERSION_4:
                       (callInfo.pduSessionParams[0].requestProfiles[0].protocol.compare(string("IPV6"))==0)?DSI_IP_VERSION_6:DSI_IP_VERSION_4_6;
    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_IP_VERSION, &ipFamily)) {
      Log::getInstance().d("Unable to set ip version");
      return false;
    }
  }

  return true;
}

/*===========================================================================*/
/*!
    @brief
    Sets the RIL provided profile id (technology specific) into the
    dsi_hndl

    @return
    true
    false
*/
/*=========================================================================*/
bool setRilProfileId
(
  const dsi_hndl_t dsiHndl,
  const DataProfileId_t profile,
  const DataProfileInfoType_t tech
)
{
  int ret = false;
  int reti = true;
  dsi_call_param_value_t profile_id;
  dsi_call_param_value_t call_type;
  dsi_call_param_identifier_t profile_param_id = DSI_CALL_INFO_MIN;
  int32_t rilProfile = (int32_t)profile;

  do
  {
    /* sanity checking */
    if ((RIL_DATA_PROFILE_DEFAULT == rilProfile))
    {
      Log::getInstance().d("[CallInfo]: default profile id"+ std::to_string(rilProfile)+"provided. " \
                      "no need to set in dsi_hndl");
      ret = true;
      break;
    }

    /* prepare dsi parameter with profile id */
    memset( &profile_id, 0, sizeof( dsi_call_param_value_t ) );
    profile_id.buf_val = NULL;
    profile_id.num_val = rilProfile;
    Log::getInstance().d("RIL provided PROFILE ID Number"+ std::to_string(profile_id.num_val));

    /* adjust the profile id according to ril.h */
    if (profile_id.num_val < (int)(RIL_DATA_PROFILE_OEM_BASE))
    {
      reti = true;
      switch(profile_id.num_val)
      {
      case RIL_DATA_PROFILE_TETHERED:
        Log::getInstance().d("RIL provided"+std::to_string(profile_id.num_val)+"profile id. Tethered call " \
                          "will be used");
        call_type.buf_val = NULL;
        call_type.num_val = DSI_CALL_TYPE_TETHERED;
        if (DsiWrapper::getInstance().dsiSetDataCallParam(dsiHndl,
                                                          DSI_CALL_INFO_CALL_TYPE,
                                                         &call_type) != DSI_SUCCESS)
        {
          Log::getInstance().d("Couldn't set call_type"+ std::to_string(call_type.num_val));
          reti = false;
          break;
        }
        break;
      default:
        Log::getInstance().d("RIL provided"+std::to_string(profile_id.num_val)+ "profile id. This is currently "
                        "not supported");
      }
      if (reti != true)
      {
        break;
      }
    }
    else
    {
      /* adjust for the OEM base */
      profile_id.num_val -= RIL_DATA_PROFILE_OEM_BASE;
      Log::getInstance().d("profile_id.num_val"+std::to_string(profile_id.num_val)+"will be used (android provided"+ \
        std::to_string(profile_id.num_val + RIL_DATA_PROFILE_OEM_BASE));

      /* figure out whether this is umts or cdma profile id */
      if (tech == DataProfileInfoType_t::THREE_GPP2)
      {
        profile_param_id = DSI_CALL_INFO_CDMA_PROFILE_IDX;
      }
      else if (tech == DataProfileInfoType_t::THREE_GPP)
      {
        profile_param_id = DSI_CALL_INFO_UMTS_PROFILE_IDX;
      }
      else
      {
        Log::getInstance().d("RIL provided incorrect/malformed technology"+std::to_string((int)tech));
        break;
      }

      /* now set the profile id onto dsi_hndl */
      if ( ( DsiWrapper::getInstance().dsiSetDataCallParam(dsiHndl,
                                                           profile_param_id,
                                                          &profile_id ) ) != DSI_SUCCESS )
      {
        Log::getInstance().d("unable to set profile id "+std::to_string(profile_id.num_val));
        break;
      }
    }

    ret = true;
  } while (0);

  if (true == ret)
  {
    Log::getInstance().d("setRilProfileId successful");

  }
  else
  {
    Log::getInstance().d("setRilProfileId failed");
  }

  return ret;
}

CallStateFailureReason CallState::setProfileInfo() {
  Log::getInstance().d("[CallInfo]: setProfileInfo");

  // getting dsi handle
  dsi_hndl_t dsiHandle = nullptr;
  if (isDsiex2Supported) {
    if((dsiHandle = DsiWrapper::getInstance().dsiGetDataSrvcHndl(CallState::dsiNetCallback2, DSI_MODE_GENERAL_EX, &callInfo)) == nullptr) {
      Log::getInstance().d("Unable to get dsi handle");
      return CallStateFailureReason::DsiHandleFailed;
    }
  } else {
    if((dsiHandle = DsiWrapper::getInstance().dsiGetDataSrvcHndl(CallState::dsiNetCallback, &callInfo)) == nullptr) {
      Log::getInstance().d("Unable to get dsi handle");
      return CallStateFailureReason::DsiHandleFailed;
    }
  }
  // store dsiHandle into call instance
  callInfo.dsiHandle = dsiHandle;

  // look up profile using qdp
  if(!callInfo.pduSessionParams[0].dnnName.empty() || DATA_IS_RIL_RADIO_TECH_3GPP(callInfo.radioRAT)) {
    //If either cdma or umts profile ids are available on profile info, this is
    //likely a retry due to fallback. No need to do qdp lookup again
    if (callInfo.pduSessionParams[0].profileKey3gpp == nullptr && callInfo.pduSessionParams[0].profileKey3gpp2 == nullptr) {
      lookupModemProfiles();
    }
    if (callInfo.pduSessionParams[0].profileKey3gpp != nullptr || callInfo.pduSessionParams[0].profileKey3gpp2 != nullptr) {
      // set default profile number
      if((uint32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap & (uint32_t)ApnTypes_t::DEFAULT &&
          callInfo.pduSessionParams[0].profileKey3gpp != nullptr) {
        if(globalInfo.apAssistMode) {
          Log::getInstance().d("Set default 3gpp profile num = " + callInfo.pduSessionParams[0].profileKey3gpp->dump());
          if(callInfo.wds_endpoint) {
            bool hasTrafficDescriptor = false;
            if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
              std::shared_ptr<SetupDataCallRequestMessage_1_6> msg =
              std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(callInfo.pduSessionParams[0].request);
              if (msg != nullptr && msg->getOptionalTrafficDescriptor().has_value()) {
                if (msg->getOptionalTrafficDescriptor().value().osAppId.has_value()) {
                  hasTrafficDescriptor = true;
                }
              }
            }
            if (!hasTrafficDescriptor || callInfo.callBringUpCapability == BringUpCapability::BRING_UP_APN_TYPE) {
              callInfo.wds_endpoint->setDefaultProfileNum(TechType::THREE_GPP, callInfo.pduSessionParams[0].profileKey3gpp->getProfileId());
            }
          }
          else {
            Log::getInstance().d("wds_endpoint is nullptr ");
          }
        }
      }

      #ifdef QMI_RIL_UTF
        dsi_call_param_value_t utf_apn;
        string apn_name = callInfo.pduSessionParams[0].dnnName;
        utf_apn.buf_val = (char *)(apn_name.c_str());
        utf_apn.num_val = apn_name.length();
        DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_APN_NAME, &utf_apn);
      #endif

      dsi_call_param_value_t bringUpMode;
      bringUpMode.buf_val = nullptr;
      bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_INVALID;

      if((int)callInfo.callBringUpCapability & (int)BringUpCapability::BRING_UP_APN_TYPE) {
        /*If more than one profile with the same APNtype is found
          then bring up the call using APNName*/
        if(getProfileCountForApnType(callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) > 1 )
        {
          if((int)callInfo.callBringUpCapability & (int)BringUpCapability::BRING_UP_APN_NAME)
          {
            Log::getInstance().d("More than one profile found with ApnType" +
                                 std::to_string((int)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap));
            Log::getInstance().d("Bringup the call by dnn name " + callInfo.pduSessionParams[0].dnnName);
            bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_NAME;

            dsi_call_param_value_t apnName;
            string apn = callInfo.pduSessionParams[0].dnnName;
            apnName.buf_val = (char *)(apn.c_str());
            apnName.num_val = apn.length();
            if(DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                            DSI_CALL_INFO_APN_NAME,
                                                                            &apnName)) {
              Log::getInstance().d("Unable to set the apn name");
              return CallStateFailureReason::DsiSetParamsFailed;
            }
          }
        }
        else
        {
          wds_apn_type_mask_v01 wdsApnTypeMask =
            WDSModemEndPointModule::convertToApnTypeMask((int32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap);
          /* set DSI bringup mode as apn-type based bringup */
          Log::getInstance().d("Bringup the call by APNType " + std::to_string((int)wdsApnTypeMask));
          bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_TYPE_MASK;

          dsi_call_param_value_t apnTypeMask;
          apnTypeMask.buf_val = (char*)&wdsApnTypeMask;
          apnTypeMask.num_val = (int)sizeof(wds_apn_type_mask_v01);
          if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                           DSI_CALL_INFO_APN_TYPE_MASK,
                                                                           &apnTypeMask)) {
            Log::getInstance().d("Unable to set bringup apn type mask");
            return CallStateFailureReason::DsiSetParamsFailed;
          }
        }

        if(bringUpMode.num_val != DSI_CALL_BRINGUP_MODE_INVALID)
        {
          if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                           DSI_CALL_INFO_CALL_BRINGUP_MODE,
                                                                           &bringUpMode)) {
            Log::getInstance().d("Unable to set bringup mode");
            return CallStateFailureReason::DsiSetParamsFailed;
          }
        }
      }
      else if ((int)callInfo.callBringUpCapability & (int)BringUpCapability::BRING_UP_APN_NAME) {
        Log::getInstance().d("Bringup the call by dnn name " + callInfo.pduSessionParams[0].dnnName);
        bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_NAME;

        dsi_call_param_value_t apnName;
        string apn = callInfo.pduSessionParams[0].dnnName;
        apnName.buf_val = (char *)(apn.c_str());
        apnName.num_val = apn.length();
        if(DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                        DSI_CALL_INFO_APN_NAME,
                                                                        &apnName)) {
          Log::getInstance().d("Unable to set the apn name");
          return CallStateFailureReason::DsiSetParamsFailed;
        }
        if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                         DSI_CALL_INFO_CALL_BRINGUP_MODE,
                                                                         &bringUpMode)) {
          Log::getInstance().d("Unable to set bringup mode");
          return CallStateFailureReason::DsiSetParamsFailed;
        }
      }
      /* profile ID based bringup */
      if(bringUpMode.num_val == DSI_CALL_BRINGUP_MODE_INVALID) {
        Log::getInstance().d("ProfileId based Bringup");
        dsi_call_param_value_t apnInfo;

        if(callInfo.pduSessionParams[0].profileKey3gpp != nullptr) {
          apnInfo.buf_val = nullptr;
          apnInfo.num_val = callInfo.pduSessionParams[0].profileKey3gpp->getProfileId();
          DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &apnInfo);
        }

        if(callInfo.pduSessionParams[0].profileKey3gpp2 != nullptr) {
          apnInfo.buf_val = nullptr;
          apnInfo.num_val = callInfo.pduSessionParams[0].profileKey3gpp2->getProfileId();
          DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &apnInfo);
        }

        if(callInfo.pduSessionParams[0].profileKey3gpp2 == nullptr || callInfo.pduSessionParams[0].profileKey3gpp == nullptr) {
          Log::getInstance().d("[qdp] apn = " + callInfo.pduSessionParams[0].dnnName + ", length = " + std::to_string(callInfo.pduSessionParams[0].dnnName.length()));
          string apnStr = callInfo.pduSessionParams[0].dnnName;
          apnInfo.buf_val = (char *)apnStr.c_str();
          apnInfo.num_val = apnStr.length();
          if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_APN_NAME, &apnInfo)) {
            Log::getInstance().d("Unable to set apn name");
            return CallStateFailureReason::DsiSetParamsFailed;
          }
        } else {
            Log::getInstance().d("[qdp] umtsProfileId = "+
                                 std::to_string((callInfo.pduSessionParams[0].profileKey3gpp == nullptr ? -1 : callInfo.pduSessionParams[0].profileKey3gpp->getProfileId()))
                                 +", "+
                                 "[qdp] cdmaProfileId = "+
                                 std::to_string((callInfo.pduSessionParams[0].profileKey3gpp2 == nullptr ? -1 : callInfo.pduSessionParams[0].profileKey3gpp2->getProfileId()))
                                 );
        }
      } /* profile-id based bringup */
    } /* profile present or created (if needed) */
  } else { /* profile doesnt have valid APN name */

    Log::getInstance().d("RIL did not provide APN, not setting any APN in start_nw_params");
    /* use the RIL profile id only if APN was not provided */
    /* set profile id in the dsi store */

    if (true != setRilProfileId( callInfo.dsiHandle,
                                 callInfo.pduSessionParams[0].requestProfiles[0].profileId,
                                 callInfo.pduSessionParams[0].requestProfiles[0].dataProfileInfoType))
    {
      Log::getInstance().d("could not set ril profile id for dsiHndle");
      return CallStateFailureReason::DsiSetParamsFailed;
    }

    /* Update dsi tech pref to cdma so that route lookup returns cdma iface */
    if(DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO( callInfo.radioRAT ))
    {
      dsi_call_param_value_t techpref_info;
      techpref_info.buf_val = NULL;
      techpref_info.num_val = DSI_RADIO_TECH_CDMA;

      Log::getInstance().d("As Radio tech is 3gpp2, Setting DSI param -DSI_CALL_INFO_TECH_PREF");

      if ( ( DSI_SUCCESS !=  DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle,
                                                                           DSI_CALL_INFO_TECH_PREF,
                                                                           &techpref_info ) ) )
      {
        Log::getInstance().d("unable to set tech pref");
        return CallStateFailureReason::DsiSetParamsFailed;
      }
    }
  }

  // set auth preference
  int authType = (int)callInfo.pduSessionParams[0].requestProfiles[0].authType;
  Log::getInstance().d("[qdp] authtype = " + std::to_string(authType));
  dsi_call_param_value_t authPref;
  authPref.buf_val = nullptr;
  authPref.num_val = authType;
  if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_AUTH_PREF, &authPref)) {
    Log::getInstance().d("Unable to set auth type");
    return CallStateFailureReason::DsiSetParamsFailed;
  }

  // set username
  if(!callInfo.pduSessionParams[0].requestProfiles[0].username.empty()) {
    Log::getInstance().d("[qdp] username = " + callInfo.pduSessionParams[0].requestProfiles[0].username);
    dsi_call_param_value_t username;
    username.buf_val = (char *)callInfo.pduSessionParams[0].requestProfiles[0].username.c_str();
    username.num_val = callInfo.pduSessionParams[0].requestProfiles[0].username.length();
    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_USERNAME, &username)) {
      Log::getInstance().d("Unable to set username");
      return CallStateFailureReason::DsiSetParamsFailed;
    }
  }

  // set password
  if(!callInfo.pduSessionParams[0].requestProfiles[0].password.empty()) {
    Log::getInstance().d("[qdp] password = " + callInfo.pduSessionParams[0].requestProfiles[0].password);
    dsi_call_param_value_t password;
    string passwordStr = callInfo.pduSessionParams[0].requestProfiles[0].password;
    password.buf_val = (char *)passwordStr.c_str();
    password.num_val = callInfo.pduSessionParams[0].requestProfiles[0].password.length();
    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_PASSWORD, &password)) {
      Log::getInstance().d("Unable to set password");
      return CallStateFailureReason::DsiSetParamsFailed;
    }
  }

  // set IP family
  if(!callInfo.pduSessionParams[0].requestProfiles[0].protocol.empty()) {
    Log::getInstance().d("[qdp] protocol = " + callInfo.pduSessionParams[0].requestProfiles[0].protocol);
    dsi_call_param_value_t ipFamily;
    ipFamily.buf_val = nullptr;

    if(callInfo.pduSessionParams[0].requestProfiles[0].protocol.compare(string("IP"))==0) {
      ipFamily.num_val = DSI_IP_VERSION_4;
    }
    else if(callInfo.pduSessionParams[0].requestProfiles[0].protocol.compare(string("IPV6"))==0) {
      ipFamily.num_val = DSI_IP_VERSION_6;
      /*-------------------------------------------------------------------
        Add IPv4 automatically if CLAT is enabled on this profile
      -------------------------------------------------------------------*/
      if( global_qcril_clat_supported )
      {
        if (!callInfo.pduSessionParams[0].dnnName.empty() &&
             ((uint32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap
                & (uint32_t)ApnTypes_t::DEFAULT) == (uint32_t)ApnTypes_t::DEFAULT)
        {
          Log::getInstance().d("[CallInfo] Automatic IPv4 request for APN "
                               + callInfo.pduSessionParams[0].dnnName);
          ipFamily.num_val = DSI_IP_VERSION_4_6;
          callInfo.pduSessionParams[0].requestProfiles[0].protocol = "IPV4V6";
        }
        else {
          bool isRadioTech3gpp2 = callInfo.pduSessionParams[0].requestProfiles[0].dataProfileInfoType == DataProfileInfoType_t::THREE_GPP2;
          std::optional<ProfileParams> profileParams;
          if (isRadioTech3gpp2 && callInfo.pduSessionParams[0].profileKey3gpp2 != nullptr) {
            profileParams = Manager::getInstance().getProfile(callInfo.pduSessionParams[0].profileKey3gpp2);
          }
          if (!profileParams && !isRadioTech3gpp2 && callInfo.pduSessionParams[0].profileKey3gpp != nullptr) {
            profileParams = Manager::getInstance().getProfile(callInfo.pduSessionParams[0].profileKey3gpp);
          }
          if (profileParams && profileParams->hasIsClatEnabled() && profileParams->getIsClatEnabled()) {
            ipFamily.num_val = DSI_IP_VERSION_4_6;
            callInfo.pduSessionParams[0].requestProfiles[0].protocol = "IPV4V6";
          }
        }
      }
    }
    else
    {
      ipFamily.num_val = DSI_IP_VERSION_4_6;
      callInfo.pduSessionParams[0].requestProfiles[0].protocol = "IPV4V6";
    }

    if ((!callInfo.pduSessionParams[0].requestProfiles[0].roamingProtocol.empty()) &&
      (callInfo.pduSessionParams[0].requestProfiles[0].protocol.compare
       (callInfo.pduSessionParams[0].requestProfiles[0].roamingProtocol) != 0)) {
      Log::getInstance().d("Home Ip type is different from Roaming Ip type, use IPv4v6");
      ipFamily.num_val = DSI_IP_VERSION_4_6;
      callInfo.pduSessionParams[0].requestProfiles[0].protocol = "IPV4V6";
    }

    if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_IP_VERSION, &ipFamily)) {
      Log::getInstance().d("Unable to set ip version");
      return CallStateFailureReason::DsiSetParamsFailed;
    }
  }

#ifndef RIL_FOR_MDM_LE
  // set disallow roaming state
  dsi_call_param_value_t disallowRoaming;
  disallowRoaming.buf_val = nullptr;
  disallowRoaming.num_val = callInfo.roamingAllowed ? 0 : 1;
  if (DSI_SUCCESS != DsiWrapper::getInstance().dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_DISALLOW_ROAMING, &disallowRoaming)) {
    Log::getInstance().d("Unable to set disallow roaming");
    return CallStateFailureReason::DsiSetParamsFailed;
  }
#endif

  return CallStateFailureReason::NoError;
}

/**
 * @brief static function for DSI callback
 * @details static function requires to use call instance
 *
 * @param userData is CallState * type
 */
void CallState::dsiNetCallback(
  dsi_hndl_t dsiHandle,
  void *userData,
  dsi_net_evt_t netEvent,
  dsi_evt_payload_t *payload
)
{
  std::ignore = dsiHandle;

  Log::getInstance().d("dsi net evt = " + std::to_string(netEvent));

  EventDataType callbackEvent;
  memset(&callbackEvent, 0, sizeof(EventDataType));

  callbackEvent.evt = netEvent;
  callbackEvent.data = (void *)userData;
  callbackEvent.data_len = sizeof(CallInfo *);
  callbackEvent.self = (void *)&callbackEvent;
  memset(&callbackEvent.payload, '\0', sizeof(dsi_evt_payload_t));

  if(payload != nullptr)
  {
    memcpy(&callbackEvent.payload, payload, sizeof(dsi_evt_payload_t));
  }

  std::shared_ptr<RilEventDataCallback> msg = std::make_shared<RilEventDataCallback>(callbackEvent);
  if(msg) {
    msg->setCid(((CallInfo*)userData)->cid);
    msg->broadcast();
  }
}

bool isConnectBasedEvent(dsi_net_evt_t &evt)
{
    switch(evt) {
      case DSI_EVT_NET_IS_CONN:
        return true;
      case DSI_EVT_NET_RECONFIGURED:
        return true;
      case DSI_EVT_NET_NEWADDR:
        return true;
      case DSI_EVT_NET_DELADDR:
        return true;
      case DSI_EVT_NET_PARTIAL_CONN:
        return true;
      default:
        return false;
    }
    return false;
}

/**
 * @brief static function for DSI ex2 callback
 * @details static function requires to use call instance
 *
 * @param userData is CallState * type
 */
void CallState::dsiNetCallback2(
  dsi_hndl_t dsiHandle,
  void *userData,
  dsi_net_evt_t netEvent,
  dsi_evt_payload_ex_t *payload
)
{
  Log::getInstance().d("dsi net evt ex_2= " + std::to_string(netEvent));

  EventDataType callbackEvent;
  memset(&callbackEvent, 0, sizeof(EventDataType));

  callbackEvent.evt = netEvent;
  callbackEvent.data = (void *)userData;
  callbackEvent.data_len = sizeof(CallInfo *);
  callbackEvent.self = (void *)&callbackEvent;
  callbackEvent.hndl = dsiHandle;
  callbackEvent.payload_ex = {};

  if(payload != nullptr)
  {
    if (isConnectBasedEvent(netEvent)) {
      callbackEvent.payload_ex.connectInfo = payload->dev_info;
      callbackEvent.payload_ex.isConnectInfoValid = true;
    }
    if (netEvent == DSI_EVT_NET_NO_NET) {
      callbackEvent.payload_ex.disconnectInfo = payload->disconnect_info;
      callbackEvent.payload_ex.isDisconnectInfoValid = true;
    }
    if (netEvent == DSI_EVT_NET_NEWMTU) {
      callbackEvent.payload_ex.connectInfo.mtu = payload->mtu;
      callbackEvent.payload_ex.isMtuValid = true;
    }
  }

  std::shared_ptr<RilEventDataCallback> msg = std::make_shared<RilEventDataCallback>(callbackEvent);
  if(msg) {
    msg->setCid(((CallInfo*)userData)->cid);
    msg->broadcast();
  }
}

void CallState::compareCallParams(const CallInfo& callInst) {
  if(callInfo.v4Connected != callInst.v4Connected) {
    Log::getInstance().d("compareCallParams - v4Connected state is changed");
    callInfo.v4Connected = callInst.v4Connected;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.v6Connected != callInst.v6Connected) {
    Log::getInstance().d("compareCallParams - v6Connected state is changed");
    callInfo.v6Connected = callInst.v6Connected;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.deviceName.compare(callInst.deviceName) != 0) {
    Log::getInstance().d("compareCallParams - deviceName is changed");
    callInfo.deviceName = callInst.deviceName;
    callInfo.callParamsChanged = true;
  }
  if (callInfo.pduInfoList.size() != callInst.pduInfoList.size()) {
    Log::getInstance().d("compareCallParams - pdu info list size is changed");
    callInfo.pduInfoList = callInst.pduInfoList;
    callInfo.callParamsChanged = true;
  }
  else {
    Log::getInstance().d("compareCallParams - pdu info list size is same");
    if (callInfo.pduInfoList.front() != callInst.pduInfoList.front()) {
      callInfo.pduInfoList.front() = callInst.pduInfoList.front();
      Log::getInstance().d("compareCallParams - pdu info front is changed");
      callInfo.callParamsChanged = true;
    }
    if (callInfo.pduInfoList.size() > 1) {
      if (callInfo.pduInfoList.back() != callInst.pduInfoList.back()) {
        callInfo.pduInfoList.back() = callInst.pduInfoList.back();
        Log::getInstance().d("compareCallParams - pdu info back is changed");
        callInfo.callParamsChanged = true;
      }
    }
  }
  if(callInfo.ipType.compare(callInst.ipType) != 0) {
    Log::getInstance().d("compareCallParams - ipType is changed");
    callInfo.ipType = callInst.ipType;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.mtu != callInst.mtu) {
    Log::getInstance().d("compareCallParams - MTU is changed");
    callInfo.mtu = callInst.mtu;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.consolidatedMtuV4 != callInst.consolidatedMtuV4) {
    Log::getInstance().d("compareCallParams - v4 MTU is changed");
    callInfo.consolidatedMtuV4 = callInst.consolidatedMtuV4;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.consolidatedMtuV6 != callInst.consolidatedMtuV6) {
    Log::getInstance().d("compareCallParams - v6 MTU is changed");
    callInfo.consolidatedMtuV6 = callInst.consolidatedMtuV6;
    callInfo.callParamsChanged = true;
  }
}

void CallState::configureParams() {

  CallInfo tempCallinfo = {};
  PduInfo primPduInfo = {};
  PduInfo secPduInfo = {};
  string v4PcscfAddr, v6PcscfAddr;

  primPduInfo.pduId = 0;

  DsiWrapper::getInstance().dsiGetDeviceName(callInfo.dsiHandle, tempCallinfo.deviceName);
  DsiWrapper::getInstance().dsiGetAddresses(callInfo.dsiHandle, tempCallinfo.v4Connected, tempCallinfo.v6Connected,
                                            primPduInfo, secPduInfo);

  DsiWrapper::getInstance().dsiGetPcscfAddresses(callInfo.dsiHandle, v4PcscfAddr,
                                                 v6PcscfAddr, 0);
  tempCallinfo.mtu = DsiWrapper::getInstance().dsiGetMtu(callInfo.dsiHandle);
  dsi_v4_v6_mtu_t v4_v6_mtu;
  DsiWrapper::getInstance().dsiGetMtuByFamily(callInfo.dsiHandle, v4_v6_mtu, 0);
  if (primPduInfo.v4Info) {
    primPduInfo.v4Info->pcscfAddress = v4PcscfAddr;
    primPduInfo.v4Info->mtu = v4_v6_mtu.v4_mtu;
  }
  if (primPduInfo.v6Info) {
    primPduInfo.v6Info->pcscfAddress = v6PcscfAddr;
    primPduInfo.v6Info->mtu = v4_v6_mtu.v6_mtu;
  }

  if (callInfo.pduInfoList.size() > 1) {
    secPduInfo.pduId = callInfo.pduInfoList.back().pduId;
    v4PcscfAddr.clear();
    v6PcscfAddr.clear();
    DsiWrapper::getInstance().dsiGetPcscfAddresses(callInfo.dsiHandle, v4PcscfAddr,
                                                   v6PcscfAddr, secPduInfo.pduId);
    v4_v6_mtu = {0,0};
    DsiWrapper::getInstance().dsiGetMtuByFamily(callInfo.dsiHandle, v4_v6_mtu, secPduInfo.pduId);
    if (secPduInfo.v4Info) {
      secPduInfo.v4Info->pcscfAddress = v4PcscfAddr;
      secPduInfo.v4Info->mtu = v4_v6_mtu.v4_mtu;
    }
    if (secPduInfo.v6Info) {
      secPduInfo.v6Info->pcscfAddress = v6PcscfAddr;
      secPduInfo.v6Info->mtu = v4_v6_mtu.v6_mtu;
    }
  }

  //Check if it a DELADDR scenario
  if ((!tempCallinfo.v4Connected && callInfo.v4Connected) ||
      (!tempCallinfo.v6Connected && callInfo.v6Connected)) {
    callInfo.v4HandedOver = HandOverState::Success;
    callInfo.v6HandedOver = HandOverState::Success;
  }
  else {
    //If HO had failed for one of the IP types, then reset info for the
    //IP type. This is needed until the address actually gets deleted. This
    //is to resolve the race condition until address in dsi actually gets
    //deleted
    IpFamilyType_t ipType = determineIpType();
    if (ipType == IpFamilyType_t::IPv4) {
      primPduInfo.v6Info.reset();
      secPduInfo.v6Info.reset();
      tempCallinfo.v6Connected = false;
    }
    else if (ipType == IpFamilyType_t::IPv6) {
      primPduInfo.v4Info.reset();
      secPduInfo.v4Info.reset();
      tempCallinfo.v4Connected = false;
    }
  }
  if(tempCallinfo.v4Connected && tempCallinfo.v6Connected) {
    tempCallinfo.ipType = "IPV4V6";
  }
  else if (tempCallinfo.v6Connected) {
    tempCallinfo.ipType = "IPV6";
  }
  else if (tempCallinfo.v4Connected) {
    tempCallinfo.ipType = "IP";
  }

  tempCallinfo.pduInfoList.push_back(primPduInfo);
  if (callInfo.pduInfoList.size() > 1) {
    tempCallinfo.pduInfoList.push_back(secPduInfo);
  }

  // Only set the V6 MTU if it has not been set before.
  // This prevents the framework from overriding the route set by kernel.
  tempCallinfo.consolidatedMtuV6 = callInfo.consolidatedMtuV6;
  tempCallinfo.calculateConsolidatedMtu();

  compareCallParams(tempCallinfo);
  if (isDsiex2Supported)
    DsiWrapper::getInstance().dsiEraseConnectionMap(callInfo.dsiHandle);
}

void CallState::configurePcscfAddr(uint32_t pduId) {
  std::string newPcscfV4 = "";
  std::string newPcscfV6 = "";
  IpFamilyType_t ipType = determineIpType();
  DsiWrapper::getInstance().dsiGetPcscfAddresses(callInfo.dsiHandle, newPcscfV4,
                                                 newPcscfV6, pduId);
  if (ipType == IpFamilyType_t::IPv4) {
    newPcscfV6 = "";
  }
  else if (ipType == IpFamilyType_t::IPv6) {
    newPcscfV4 = "";
  }
  if (pduId == 0) {
    if ((callInfo.pduInfoList.front().v4Info) &&
        (callInfo.pduInfoList.front().v4Info->pcscfAddress.compare(newPcscfV4))) {
      callInfo.callParamsChanged = true;
      callInfo.pduInfoList.front().v4Info->pcscfAddress = newPcscfV4;
    }
    if ((callInfo.pduInfoList.front().v6Info) &&
        (callInfo.pduInfoList.front().v6Info->pcscfAddress.compare(newPcscfV6))) {
      callInfo.callParamsChanged = true;
      callInfo.pduInfoList.front().v6Info->pcscfAddress = newPcscfV6;
    }
  }
  else {
    if (callInfo.pduInfoList.size() > 1) {
      if (callInfo.pduInfoList.back().v4Info) {
        callInfo.pduInfoList.back().v4Info->pcscfAddress = newPcscfV4;
      }
      if (callInfo.pduInfoList.back().v6Info) {
        callInfo.pduInfoList.back().v6Info->pcscfAddress = newPcscfV6;
      }
    }
  }
}

void CallState::configureMtu(uint32_t pduId) {
  callInfo.mtu = DsiWrapper::getInstance().dsiGetMtu(callInfo.dsiHandle);
  dsi_v4_v6_mtu_t v4_v6_mtu;
  IpFamilyType_t ipType = determineIpType();
  DsiWrapper::getInstance().dsiGetMtuByFamily(callInfo.dsiHandle, v4_v6_mtu, pduId);
  if (ipType == IpFamilyType_t::IPv4) {
    v4_v6_mtu.v6_mtu = 0;
  }
  else if (ipType == IpFamilyType_t::IPv6) {
    v4_v6_mtu.v4_mtu = 0;
  }
  if (pduId == 0) {
    if (callInfo.pduInfoList.front().v4Info) {
      callInfo.pduInfoList.front().v4Info->mtu = v4_v6_mtu.v4_mtu;
    }
    if (callInfo.pduInfoList.front().v6Info) {
      callInfo.pduInfoList.front().v6Info->mtu = v4_v6_mtu.v6_mtu;
    }
  }
  else {
    if (callInfo.pduInfoList.size() > 1) {
      if (callInfo.pduInfoList.back().v4Info) {
        callInfo.pduInfoList.back().v4Info->mtu = v4_v6_mtu.v4_mtu;
      }
      if (callInfo.pduInfoList.back().v6Info) {
        callInfo.pduInfoList.back().v6Info->mtu = v4_v6_mtu.v6_mtu;
      }
    }
  }
  callInfo.calculateConsolidatedMtu();
  if (isDsiex2Supported)
    DsiWrapper::getInstance().dsiEraseConnectionMap(callInfo.dsiHandle);
}

void CallState::createQmiWdsClients() {
  Log::getInstance().d("createQmiWdsClients");

  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_linux_get_conn_id_by_name_ex(callInfo.deviceName.c_str(), &ep_type, &epid, &mux_id);
  QmiSetupRequestCallback qmiWdsSetupCallback("CallInfo-Token");

  if(callInfo.v4Connected && callInfo.wds_v4_call_endpoint == nullptr) {
    callInfo.wds_v4_call_endpoint = std::make_unique<WDSCallModemEndPoint>(
                                                                "WDSCallv4ModemEndPoint_" + std::to_string(callInfo.cid),
                                                                callInfo.cid,
                                                                callInfo.pduSessionParams[0].requestProfiles[0].apn,
                                                                callInfo.deviceName,
                                                                IpFamilyType_t::IPv4,
                                                                globalInfo.reportPhysicalChannelConfig);
    callInfo.wds_v4_call_endpoint->setBindMuxDataPortParams(ep_type, epid, mux_id);
    callInfo.wds_v4_call_endpoint->requestSetup("CallInfo-v4-token", static_cast<qcril_instance_id_e_type>(global_instance_id),
    &qmiWdsSetupCallback);
    callInfo.wds_v4_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport, 0);
  }

  if(callInfo.v6Connected && callInfo.wds_v6_call_endpoint == nullptr) {
    callInfo.wds_v6_call_endpoint = std::make_unique<WDSCallModemEndPoint>(
                                                                "WDSCallv6ModemEndPoint_" + std::to_string(callInfo.cid),
                                                                callInfo.cid,
                                                                callInfo.pduSessionParams[0].requestProfiles[0].apn,
                                                                callInfo.deviceName,
                                                                IpFamilyType_t::IPv6,
                                                                globalInfo.reportPhysicalChannelConfig);
    callInfo.wds_v6_call_endpoint->setBindMuxDataPortParams(ep_type, epid, mux_id);
    callInfo.wds_v6_call_endpoint->requestSetup("CallInfo-v6-token", static_cast<qcril_instance_id_e_type>(global_instance_id),
    &qmiWdsSetupCallback);
    callInfo.wds_v6_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport, 0);
  }
}

void CallState::initializeQos()
{
  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_linux_get_conn_id_by_name_ex(callInfo.deviceName.c_str(), &ep_type, &epid, &mux_id);
  auto m = std::make_shared<QosInitializeMessage>(callInfo.cid, mux_id,ep_type, epid);
  m->broadcast();
  Log::getInstance().d("[" + getName() + "]: initializeQos done");
}

void CallState::deInitializeQos()
{
  auto m = std::make_shared<QosInitializeMessage>(0,0,0,0,true);
  m->broadcast();
  Log::getInstance().d("[" + getName() + "]: deInitializeQos done");
}

void CallState::dataCallListChanged() {
  if (callInfo.dataCallListChangedCallback == nullptr) {
    Log::getInstance().d("[" + getName() + "]: dataCallListChangedCallback not found");
  } else {
    callInfo.dataCallListChangedCallback();
  }
}

void CallState::getPdnThrottleTime() {
  if (callInfo.wds_endpoint != nullptr) {
    // for now, QMI WDS does not handle 3GPP2 throttle
    DataProfileInfoType_t techType = DataProfileInfoType_t::THREE_GPP;
    callInfo.wds_endpoint->getPdnThrottleTime(callInfo.pduSessionParams[0].dnnName, techType, callInfo.pduSessionParams[0].requestProfiles[0].protocol,
                                                     callInfo.cid);
  } else {
    Log::getInstance().d("[" + getName() + "]: failed to get throttle time");
    auto indMsg = std::make_shared<GetPdnThrottleTimeResponseInd>(callInfo.cid, -1);
    indMsg->broadcast();
  }
}

bool CallState::isReasonPdnThrottled(dsi_ce_reason_t dsiReason) {
  return (dsiReason.reason_type == DSI_CE_TYPE_INTERNAL &&
             (dsiReason.reason_code == WDS_VCER_INTERNAL_ERR_PDN_IPV4_CALL_THROTTLED_V01 ||
              dsiReason.reason_code == WDS_VCER_INTERNAL_ERR_PDN_IPV6_CALL_THROTTLED_V01 ||
              dsiReason.reason_code == WDS_VCER_INTERNAL_ROUTING_FAIL_V01)) ||
         (dsiReason.reason_type == DSI_CE_TYPE_3GPP_SPEC_DEFINED &&
             (dsiReason.reason_code == WDS_VCER_3GPP_OPERATOR_DETERMINED_BARRING_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_UNKNOWN_APN_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_UNKNOWN_PDP_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_AUTH_FAILED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_GGSN_REJECT_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_ACTIVATION_REJECT_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_OPTION_NOT_SUPPORTED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_OPTION_UNSUBSCRIBED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_OPTION_TEMP_OOO_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_NSAPI_ALREADY_USED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_NETWORK_FAILURE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_UMTS_REACTIVATION_REQ_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_PDP_WITHOUT_ACTIVE_TFT_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_LADN_DNN_NOT_AVAIL_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_IP_V4_ONLY_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_IP_V6_ONLY_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_ESM_INFO_NOT_RECEIVED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_PDN_CONN_DOES_NOT_EXIST_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_IP_V4V6_ONLY_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_NON_IP_ONLY_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_ETHERNET_ONLY_ALLOWED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MAX_ACTIVE_PDP_CONTEXT_REACHED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_UNSUPPORTED_APN_IN_CURRENT_PLMN_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_FOR_SLICE_DNN_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_NOT_SUPPORTED_SSC_MODE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_FOR_SLICE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MISSING_OR_UNKNOWN_DNN_IN_SLICE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MAX_DATA_RATE_LOW_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MESSAGE_INCORRECT_SEMANTIC_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_INVALID_MANDATORY_INFO_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MESSAGE_TYPE_UNSUPPORTED_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MSG_TYPE_NONCOMPATIBLE_STATE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_UNKNOWN_INFO_ELEMENT_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_CONDITIONAL_IE_ERROR_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_PROTOCOL_ERROR_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_APN_TYPE_CONFLICT_V01 ||
              dsiReason.reason_code == WDS_VCER_3GPP_INVALID_PCSCF_ADDR_V01
             ));
}

void CallState::cleanupKeepAlive(int cid) {
  if (callInfo.cleanupKeepAliveCallback == nullptr) {
    Log::getInstance().d("[" + getName() + "]:cleanupKeepAliveCallback not found");
  } else {
    callInfo.cleanupKeepAliveCallback(cid);
  }
}

dsd_apn_pref_sys_enum_v01 convertToDsdPrefSys(HandoffNetworkType_t network) {
  switch (network) {
    case _eWWAN:
      return DSD_APN_PREF_SYS_WWAN_V01;
    case _eWLAN:
      return DSD_APN_PREF_SYS_WLAN_V01;
    case _eIWLAN:
      return DSD_APN_PREF_SYS_IWLAN_V01;
    case _eCIWLAN:
      return DSD_APN_PREF_SYS_CIWLAN_V01;
    default:
      Log::getInstance().d("invalid preferred sys=" +
                           std::to_string(static_cast<int>(network)));
      return DSD_APN_PREF_SYS_WWAN_V01;
  }
}

bool CallState::sendSetApnPreferredSystemRequest(std::shared_ptr<DSDModemEndPoint> dsd_endpoint, string apnName, HandoffNetworkType_t prefNetwork) {
  int rc;
  bool ret = true;
  dsd_set_apn_preferred_system_req_msg_v01 req;
  dsd_set_apn_preferred_system_resp_msg_v01 resp;
  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  dsd_apn_pref_sys_enum_v01 prefSys = convertToDsdPrefSys(prefNetwork);
  Log::getInstance().d("sendSetApnPreferredSystemRequest for apn ="+apnName+
                       ", prefSys ="+std::to_string((int)prefSys));

  strlcpy(req.apn_pref_sys.apn_name, apnName.c_str(), QMI_DSD_MAX_APN_LEN_V01+1);
  req.apn_pref_sys.pref_sys = prefSys;
  req.is_ap_asst_valid = true;
  req.is_ap_asst = true;

  if (dsd_endpoint != nullptr) {
    rc = dsd_endpoint->sendRawSync(
      QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01,
      &req, sizeof(req),
      &resp, sizeof(resp));
  } else {
    Log::getInstance().d("Failed to get dsd endpoint");
    rc = false;
  }

  if (rc!=QMI_NO_ERR) {
    Log::getInstance().d("Failed to set APN preferred system change = " + std::to_string(rc)
    + " resp.result = " + std::to_string(resp.resp.result)
    + " resp.error = " + std::to_string(resp.resp.error));
    ret = false;
  }

  return ret;
}

IpFamilyType_t CallState::determineIpType()
{
  if(DUAL_IP_HO_ONLY_IPV6_HO_SUCCESS) {
    return IpFamilyType_t::IPv6;
  } else if (DUAL_IP_HO_ONLY_IPV4_HO_SUCCESS) {
    return IpFamilyType_t::IPv4;
  }
  return IpFamilyType_t::IPv4v6;
}

void CallState::setCurrentRATAndConfigureParams(std::shared_ptr<HandoverInformationIndMessage> m)
{
  if (m->hasCurrentNetwork()) {
    callInfo.currentRAT = m->getCurrentNetwork();
  }
  if (m->hasPreferredNetwork()) {
    callInfo.preferredRAT = m->getPreferredNetwork();
  }
  HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(callInfo.preferredRAT | callInfo.currentRAT);
  callInfo.currentRAT = target;

  configureParams();
}

#ifndef RIL_FOR_MDM_LE
void CallState::setThirdPartyHOParams(std::vector<std::string> addr)
{
  if (callInfo.dsiHandle) {
    dsi_call_param_value_t apnInfo;
    memset(&apnInfo,0,sizeof(apnInfo));
    dsi_wds_handoff_ctxt_t hoParams;
    memset(&hoParams,0,sizeof(hoParams));
    bool ipV4Found=false, ipV6Found= false;
    for (unsigned long i=0 ; i<addr.size(); i++) {
      std::string addrNoPrefix = addr[i].substr(0,addr[i].find("/"));
      //Do simple check for '.' to decide if ipv4 or ipv6 addr
      if (addrNoPrefix.find(".")!=std::string::npos) {
        Log::getInstance().d("found IPV4 HO addr" + addrNoPrefix);
        ipV4Found = true;
        strlcpy(hoParams.ipv4_addr,addrNoPrefix.c_str(),sizeof(hoParams.ipv4_addr));
      }
      else
      {
        Log::getInstance().d("found IPV6 HO addr" + addrNoPrefix);
        ipV6Found = true;
        strlcpy(hoParams.ipv6_addr,addrNoPrefix.c_str(),sizeof(hoParams.ipv6_addr));
      }
    }

    if (ipV4Found && ipV6Found) {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V4_V6;
    }
    else if (ipV4Found) {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V4;
    }
    else {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V6;
    }
    apnInfo.num_val = sizeof(hoParams);
    apnInfo.buf_val = (char*)&hoParams;
    if (DSI_SUCCESS != dsi_set_data_call_param(callInfo.dsiHandle, DSI_CALL_INFO_HANDOFF_CONTEXT, &apnInfo)) {
      Log::getInstance().d("Unable to set handoff context");
    }
  }
  else {
    Log::getInstance().d("dsi handle is null, not setting handoff context");
  }
}
#endif
