/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "ProfileHandler.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include <list>
#include "qcril_data.h"
#include "string.h"
#include "MessageCommon.h"
#include "DataModule.h"
#include "DataConfig.h"
#include "QdpAdapter.h"
#include "UnSolMessages/GetModemAttachParamsRetryMessage.h"

using namespace qdp;

#define QCRIL_SUCCESS 0
#define QCRIL_FAILURE 1
#define MAX_IA_RETRY_ATTEMPT 2
#define MODEM_DEFAULT_PROFILE_ID 1

ProfileHandler::ProfileHandler(LocalLogBuffer& logBuffer, Util& util,
                      std::shared_ptr<std::function<void(std::shared_ptr<Message>, RIL_Errno)>> cb):
                      logBuffer(logBuffer), util(util), informIACompletedCb(cb)
{
  attach_list = NULL;
  mApAssistMode = false;
  mIARetry = 0;
  mUpdateAllowed = false;
}

ProfileHandler::~ProfileHandler()
{
  if(mPendingMessage != nullptr)
  {
    sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
    mPendingMessage = nullptr;
    mIARetry = 0;
  }
}

void ProfileHandler::setUpdateAllowed(bool updateAllowed) {
  mUpdateAllowed = updateAllowed;
}

void ProfileHandler::handleGetModemAttachParamsRetryMessage()
{
  if(mPendingMessage != nullptr)
  {
    mIARetry++;
    compareModemAttachParams();
  }
}

void ProfileHandler::getAttachParamsTimeoutHander(void *)
{
  Log::getInstance().d("[ProfileHandler] Retry Attach Timer expired");
  auto msg = std::make_shared<GetModemAttachParamsRetryMessage>();
  msg->broadcast();
}

void ProfileHandler::compareModemAttachParams()
{
  wds_get_lte_attach_params_resp_msg_v01 curr_attach_params;
  memset(&curr_attach_params, 0x0, sizeof(curr_attach_params));
  if (!wds_endpoint)
  {
    sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
    return;
  }
  auto rc = wds_endpoint->getLteAttachParams(curr_attach_params);
  if (rc == Message::Callback::Status::SUCCESS)
  {
    if (curr_attach_params.resp.result == QMI_RESULT_SUCCESS_V01)
    {
      if (!matchModemProfileParams(&curr_attach_params,requestProfile.apn,requestProfile.protocol))
      {
        Log::getInstance().d("[ProfileHandler] : Modem attach params did not match, fallback to inital attach logic");
        initialAttachHandler();
      }
      else {
        sendSetInitialAttachResponse(RIL_E_SUCCESS);
      }
      return;
    }
    else if ((curr_attach_params.resp.result == QMI_RESULT_FAILURE_V01) &&
             (curr_attach_params.resp.error == QMI_ERR_OP_IN_PROGRESS_V01))
    {
      if(mIARetry == MAX_IA_RETRY_ATTEMPT)
      {
        Log::getInstance().d("[ProfileHandler] : Reached maximum retries for GET_LTE_ATTACH_PARAMS");
        sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
        return;
      }
      else {
        Log::getInstance().d("[ProfileHandler] : Starting Retry Attach Timer");
        auto timeoutFn = std::bind(&ProfileHandler::getAttachParamsTimeoutHander, this, std::placeholders::_1);
        mIARetryAttachTimer = TimeKeeper::getInstance().set_timer(timeoutFn, nullptr, RETRY_ATTACH_TIMEOUT);
        return;
      }
    }
  }
  //for any other scenario fallback to the initial attach logic
  initialAttachHandler();
}

string convertToRilProtocol(wds_ip_support_type_enum_v01 ipType)
{
  string rilProtocol = "IPV4V6";
  switch (ipType) {
    case WDS_IP_SUPPORT_TYPE_IPV4_V01:
      rilProtocol = "IP";
      break;
    case WDS_IP_SUPPORT_TYPE_IPV6_V01:
      rilProtocol = "IPV6";
      break;
    case WDS_IP_SUPPORT_TYPE_IPV4V6_V01:
      break;
    default:
      Log::getInstance().d("[ProfileHandler]: Modem has unsopported IP type");
      break;
  }
  return rilProtocol;
}

boolean ProfileHandler::matchModemProfileParams
(
  const wds_get_lte_attach_params_resp_msg_v01 *modem_attach_params,
  std::string ril_apn,
  std::string ril_ipfamily
)
{
  if( (modem_attach_params->apn_string_valid) &&
      (modem_attach_params->ip_type_valid) )
  {
    string attachApn = modem_attach_params->apn_string;
    string attachProtocol = convertToRilProtocol(modem_attach_params->ip_type);
    if (mUpdateAllowed) {
      return (attachApn == ril_apn &&
        (attachProtocol == ril_ipfamily || attachProtocol == "IPV4V6" || ril_ipfamily == "IPV4V6"));
    } else {
      return (attachApn == ril_apn && attachProtocol == ril_ipfamily);
    }
  }
  return false;
}


void ProfileHandler::init(bool apAssistMode, bool updateAllowed)
{
  if (!inited) {
    mApAssistMode = apAssistMode;
    Log::getInstance().d("[ProfileHandler]: initialize Modem Endpoint modules");
    wds_endpoint = ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
    dsd_endpoint = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
    inited = true;
    mUpdateAllowed = updateAllowed;
    Log::getInstance().d("ProfileHandler - request setup done");
  } else {
    Log::getInstance().d("ProfileHandler - already inited");
  }
}

void ProfileHandler::releaseQdpAttachProfile(void)
{
  Log::getInstance().d("[ProfileHandler]: releaseQdpAttachProfile");
  if (attach_list && !attach_list->empty()) {
    if (attachProfileKey != nullptr) {
      Log::getInstance().d("Removing attach profile from list " + attachProfileKey->dump());
      attach_list->pop_front();
      attachProfileKey = nullptr;
    }
  }
  #ifndef QMI_RIL_UTF
  sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
  #endif
  attach_list = nullptr;
}

void ProfileHandler::initialAttachHandler( )
{
  RIL_Errno result = RIL_E_SUCCESS;
  Log::getInstance().d("[ProfileHandler] initialAttachHandler ENTRY");

  if (!attach_list) {
    Log::getInstance().d("[ProfileHandler] attachlist is NULL");
    sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
    return;
  }
  // Check if requestProfile matches any of the the attached profiles
  ProfileHandler::AttachResponse resp = matchAttachProfile(*attach_list);
  if ( resp == SUCCESS )
  {
    attachProfileKey = util.findOrCreateProfileForLteAttach(convertDataProfileInfoToProfileParams(requestProfile));
    if (attachProfileKey == nullptr)
    {
      Log::getInstance().d("[ProfileHandler] No matching profile found "
                           "and failed to create new profile");
      Log::getInstance().d("EXIT with FAILURE" );
      result = RIL_E_OEM_ERROR_3;
      sendSetInitialAttachResponse(RIL_E_OEM_ERROR_3);
      return;
    }
    // update attach list
    if( !(attach_list->empty()) )
    {
      attach_list->pop_front();
    }
    attach_list->push_front(attachProfileKey->getProfileId());
    {
      stringstream ss;
      ss << "New Attach List:";
      copy(attach_list->begin(), attach_list->end(), ostream_iterator<int>(ss, ","));
      Log::getInstance().d("[ProfileHandler] " + ss.str());
      logBuffer.addLogWithTimestamp(ss.str());
    }
    if(requestProfile.preferred &&
      ((int)requestProfile.supportedApnTypesBitmap & (int)ApnTypes_t::DEFAULT) == (int)ApnTypes_t::DEFAULT) {
      Log::getInstance().d("[ProfileHandler] Setting default profile to profileId " + std::to_string(attachProfileKey->getProfileId()));
      wds_endpoint->setDefaultProfileNum(attachProfileKey->getTechType(), attachProfileKey->getProfileId());
    }

    /* Three Cases:

       1st Case:
       Non-LTE RAT: If the current RAT is non-LTE, then we update the attach list
       only.  Detach/Attach request is not sent to modem.

       If the current RAT is LTE & PS Service is available,
       2nd Case:
       LTE( PS Service Available) + DDS SUB: We send Detach request to modem &
       after detach is complete, we update the 'attach list' on modem.
       Later Attach request is sent to modem.

       3rd Case:
       LTE( PS Service Available) + Non-DDS SUB: We update the attach list first &
       then issue Detach request to modem. After receiving Detach response from modem,
       we send this as IA response to telephony without waiting for Detach Indication.
       So IA request ends here. Attach request is NOT sent to modem.
    */

    AttachResult res = setAttachList();
    if ( res == AttachResult::DETACH_NEEDED)
    {
      Log::getInstance().d("[ProfileHandler] :: Legacy method. Obsolete with newer modem versions");
      result = RIL_E_INTERNAL_ERR;
    }
    else if ( res == AttachResult::GENERIC_FAILURE )
    {
      // in the event that attach list update failed due to a QMI error
      // we need to notify RIL
      Log::getInstance().d("[ProfileHandler] GENERIC_FAILURE"
                         "Sending error response");
      result = RIL_E_INTERNAL_ERR;
    }
    #ifdef QMI_RIL_UTF
    releaseQdpAttachProfile();
    #endif
  }
  else
  {
    if (!mApAssistMode && dsd_endpoint)
    {
      dsd_endpoint->setApnInfoSync(requestProfile.apn,
                                            requestProfile.supportedApnTypesBitmap);
    } else
    {
      Log::getInstance().d("[ProfileHandler] Did not update apn types.");
    }
    Log::getInstance().d("[ProfileHandler] Skipping reattach");
    result = RIL_E_SUCCESS;
  }
  sendSetInitialAttachResponse(RIL_E_SUCCESS);
}

ProfileHandler::AttachResponse ProfileHandler::matchAttachProfile(const std::list<uint16_t>& attach_list)
{
  ProfileParams params = convertDataProfileInfoToProfileParams(requestProfile);
  for (auto profileId : attach_list) {
    if (util.matchLteAttachProfileId(profileId, params)) {
      Log::getInstance().d("[ProfileHandler] isAttachRequired() Matched profile = " + std::to_string(profileId));
      return FAILURE;
    }
  }
  return SUCCESS;
}

ProfileHandler::AttachResult ProfileHandler::setAttachList()
{
  AttachResult result = GENERIC_FAILURE;
  do
  {
    AttachListCap cap;
    // Check capability before we set attach list
    Message::Callback::Status status = wds_endpoint->getAttachListCapability(cap);
    if (status != Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("[ProfileHandler] failed to get capability, result = "
        + std::to_string((int) status));
      result = GENERIC_FAILURE;
      break;
    }

    rildata::AttachListAction action =
      rildata::AttachListAction::NONE;

    // If capability is supported then we need to pass in action along
    // with the attach list
    if (cap == rildata::AttachListCap::ACTION_SUPPORTED) {
      action = rildata::AttachListAction::DISCONNECT_ATTACH_APN_ONLY;
      result = NO_DETACH_NEEDED;
      Log::getInstance().d("[ProfileHandler] Action is Supported, only disconnect attach apn");
    } else if( cap == rildata::AttachListCap::ACTION_NOT_SUPPORTED)
    {
      Log::getInstance().d("[ProfileHandler] Action is Not Supported, Detach Needed");
      result = DETACH_NEEDED;
    }

    // If DETACH is needed then we trigger it with Qcril Common - NASModule -LEGACY way
    if( result != DETACH_NEEDED )
    {
      if (!mApAssistMode && dsd_endpoint)
      {
        status = dsd_endpoint->setApnInfoSync(requestProfile.apn,
                                              requestProfile.supportedApnTypesBitmap);
      } else
      {
        Log::getInstance().d("[ProfileHandler] Did not update apn types.");
      }
      status = wds_endpoint->setAttachList(attach_list, action);
      if (status != Message::Callback::Status::SUCCESS)
      {
        Log::getInstance().d("[ProfileHandler] failed to set attach list,"
                              " result = "+ std::to_string((int) status));
        result = GENERIC_FAILURE;
        break;
      }
    }
  }while(0);

  return result;
}

void ProfileHandler::handleWdsUnSolInd
(
  const wds_set_lte_attach_pdn_list_action_result_ind_msg_v01 *res
)
{
  Log::getInstance().d("ProfileHandler::handleWdsUnSolInd ENTRY");
  if ( res && res->result == 1 )
  {
    Log::getInstance().d("handleWdsUnSolInd action result success");
  } else if ( (res && res->result == 0 ) || !res )
  {
    Log::getInstance().d("handleWdsUnSolInd action result failure");
  }
}

void ProfileHandler::sendSetInitialAttachResponse(RIL_Errno result)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mPendingMessage);
  if ( m != nullptr )
  {
    auto cb = *(informIACompletedCb.get());
    cb(m, result);
    mPendingMessage = nullptr;
    mIARetry = 0;
  }
  else
  {
    Log::getInstance().d("[ProfileHandler] :: Msg is NULL");
  }
  TimeKeeper::getInstance().clear_timer(mIARetryAttachTimer);
  mIARetryAttachTimer = TimeKeeper::no_timer;
}

void ProfileHandler::setDefaultProfileNumIfRequired(const std::vector<DataProfileInfo_t>& profileList) {
  for (auto profile : profileList)
  {
    if (profile.preferred)
    {
      //Framework profile set as preferred is found
      //Search for matching 3gpp profile with same APN name and IP type
      FilterProfileParams fpp;
      if (!profile.apn.empty()) {
        fpp.setApnName(profile.apn);
      }
      if (profile.protocol == "IP") {
        fpp.setHomeIPType(IPType::V4);
      } else if (profile.protocol == "IPV6") {
        fpp.setHomeIPType(IPType::V6);
      } else {
        fpp.setHomeIPType(IPType::V4V6);
      }
      auto matches = Manager::getInstance().filter(fpp);
      for (auto key : matches) {
        if (key != nullptr) {
          wds_endpoint->setDefaultProfileNum(key->getTechType(), key->getProfileId());
        }
      }
    } // if preferred
  }
}

/**
 * @brief handleSetDataProfileRequestMessage
 * @details
 * - Process each profile in the incoming message thus:
 * -- Looks up matching profile on modem based on APN class, or creates
 *   one if needed
 * -- If operating in AP assist mode, invokes wds_endpoint to update
 *    profile parameters on modem (see handling for 'persistent' and
 *    'preferred' profiles, introduced in Radio HAL 1.4)
 * - Sends response for this message to the HAL
 * @return void
 **/
void ProfileHandler::handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[ProfileHandler]: handleSetDataProfileRequestMessage");
  std::shared_ptr<SetDataProfileRequestMessage> m =
    std::static_pointer_cast<SetDataProfileRequestMessage>(msg);

  if(m != nullptr) {
    /* examine all provided profiles and update modem profiles if needed */
    std::list<ProfileParams> profileList3gpp, profileList3gpp2;
    std::list<ProfileParams> consolidatedProfileList;
    for(auto profile : m->getProfiles())
    {
      ProfileParams p = convertDataProfileInfoToProfileParams(profile);
      consolidatedProfileList.push_back(p);
      if (!profile.persistent) {
        continue;
      }

      if (profile.dataProfileInfoType == DataProfileInfoType_t::COMMON) {
        profileList3gpp.push_back(p);
        profileList3gpp2.push_back(p);
      } else if (profile.dataProfileInfoType == DataProfileInfoType_t::THREE_GPP) {
        profileList3gpp.push_back(p);
      } else if (profile.dataProfileInfoType == DataProfileInfoType_t::THREE_GPP2) {
        profileList3gpp2.push_back(p);
      }
    }
    if (!profileList3gpp.empty() || !profileList3gpp2.empty()) {
      util.updateModemProfileByApnClass(profileList3gpp, profileList3gpp2);
    }

    util.updateAPNTypeBitmaskOnModemProfile(consolidatedProfileList);

    setDefaultProfileNumIfRequired(m->getProfiles());

    stringstream ss;
    ss << "[ProfileHandler]: " << (int)m->getSerial() << "< setDataProfileResponse Success";
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<RIL_Errno>(RIL_E_SUCCESS));
  }
  else {// (m == nullptr)
    Log::getInstance().d("[ProfileHandler]: Improper SetDataProfileRequestMessage received");
  }
} /* ProfileHandler::handleSetDataProfileRequestMessage() */

void ProfileHandler::handleInitialAttachRequest(std::shared_ptr<Message> msg)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
       std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);
  Log::getInstance().d("[ProfileHandler]: Handling: " + msg->dump());
  if( m != NULL ) {

    if(mPendingMessage != nullptr)
    {
      Log::getInstance().d("[ProfileHandler] Received another IA, Stopping retry timer.");
      sendSetInitialAttachResponse(RIL_E_INTERNAL_ERR);
    }
    mPendingMessage = msg;

    if(!m->hasProfileInfo())
    {
      Log::getInstance().d("[ProfileHandler]: profileInfo is not set");
      #ifndef QMI_RIL_UTF
      if(attachProfileKey)
      #endif
      {
        auto attachProfile = Manager::getInstance().getProfile(attachProfileKey);
        if (attachProfile && attachProfile->getIsQdpProfile() && attachProfile->getIsIAProfile())
        {
          if(attachProfile->hasApnTypesMask() && (((int)attachProfile->getApnTypesMask() & (int)ApnTypes_t::DEFAULT) == (int)ApnTypes_t::DEFAULT))
          {
            Log::getInstance().d("[ProfileHandler] : changing the default profile to profileId 1");
            wds_endpoint->setDefaultProfileNum(TechType::THREE_GPP, MODEM_DEFAULT_PROFILE_ID);
          }

          if(Manager::getInstance().deleteProfile(attachProfileKey))
          {
            Log::getInstance().d("[ProfileHandler] : qdp_ia profile is deleted");
            attachProfileKey = nullptr;
          }
        }
      }
      sendSetInitialAttachResponse(RIL_E_SUCCESS);
      return;
    }

    requestProfile = m->getProfile();
    if(!wds_endpoint)
    {
      Log::getInstance().d("[ProfileHandler] wds_endpoint is NULL. Sending error response");
      sendSetInitialAttachResponse(RIL_E_GENERIC_FAILURE);
      return;
    }
    Message::Callback::Status status = wds_endpoint->getAttachList(attach_list);
    if (status != Message::Callback::Status::SUCCESS)
    {
      Log::getInstance().d("[ProfileHandler] failed to get attach list, result="
                           + std::to_string((int) status));
      sendSetInitialAttachResponse(RIL_E_OEM_ERROR_3);
      return;
    }
    if(attach_list == nullptr) {
      Log::getInstance().d("Modem Attach list is NULL. Returning Failure response");
      sendSetInitialAttachResponse(RIL_E_GENERIC_FAILURE);
      return;
    }

    {
      stringstream ss;
      ss << "Attach List:";
      copy(attach_list->begin(), attach_list->end(), ostream_iterator<int>(ss, ","));
      Log::getInstance().d("[ProfileHandler] " + ss.str());
      logBuffer.addLogWithTimestamp(ss.str());
    }
    //Check if first profile in attach list is a null profile
    if (!attach_list->empty() && isNullProfile(attach_list->front())) {
      compareModemAttachParams();
    }
    else {
      initialAttachHandler();
    }
  }
}

void ProfileHandler::dump(string padding, ostream& os) {
  os << padding << "inited=" << std::boolalpha << inited << endl;
  os << padding << "attach_list=";
  if (attach_list == nullptr) {
    os << "null";
  } else {
    copy(attach_list->begin(), attach_list->end(), ostream_iterator<uint16_t>(cout, ","));
  }
  os << endl;
  os << padding << "attachKey=" << (attachProfileKey == nullptr ? "null" : attachProfileKey->dump()) << endl;
  os << padding << "profile=";
  requestProfile.dump("", os);
  os << endl;
}
