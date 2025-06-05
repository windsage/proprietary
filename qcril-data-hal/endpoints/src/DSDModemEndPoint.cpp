/******************************************************************************
#  Copyright (c) 2018-2019, 2021-2024 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "DSDModemEndPoint.h"
#include "request/GetDsdSystemStatusMessage.h"
#include "request/GetDataNrIconTypeMessage.h"
#include "UnSolMessages/DDSSwitchResultIndMessage.h"
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "UnSolMessages/DsdSystemStatusPerApnMessage.h"
#include "UnSolMessages/DataNrIconTypeIndMessage.h"
#include "UnSolMessages/AutoDdsSwitchControlIndMessage.h"
#include "UnSolMessages/DataCapabilityChangeIndMessage.h"
#include "UnSolMessages/ProcessCurrentRoamingStatus.h"
#include "MessageCommon.h"
#include <sstream>

using std::to_string;
using namespace rildata;
constexpr const char *DSDModemEndPoint::NAME;
bool DSDModemEndPoint::mUserDdsSwitchSelection = false;

void DSDModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
  mUserDdsSwitchSelection = false;
}

boolean DSDModemEndPoint::isApnTypeFound
(
  const ApnTypes_t &apn_type, const ApnTypes_t &apn_types_mask
)
{
  if (((int32_t)apn_type & (int32_t)apn_types_mask) == (int32_t)apn_type)
  {
    std::stringstream ss;
    ss << "apn type " << apn_type << " was enabled";
    Log::getInstance().d(ss.str());
    return TRUE;
  }
  return FALSE;
}

void DSDModemEndPoint::sendApnInfoQmi
(
  const std::string &apn_name,
  dsd_apn_type_enum_v01 apn_type
)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("DSDModemEndPoint::sendApnInfoQmi ENTRY");
    dsd_set_apn_info_req_msg_v01 qmiReq;
    dsd_set_apn_info_resp_msg_v01 qmiResp;

    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    strlcpy(&qmiReq.apn_info.apn_name[0], apn_name.c_str(), QMI_DSD_MAX_APN_LEN_V01);

    // indicate that the apn_invalid flag is set
    qmiReq.apn_invalid_valid = FALSE;
    qmiReq.apn_info.apn_type = apn_type;

    auto retVal = sendRawSync(QMI_DSD_SET_APN_INFO_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal == QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_SET_APN_INFO_REQ_V01 is"
                           "sent successfully for type = " + std::to_string(apn_type));
    }
  }
}

Message::Callback::Status DSDModemEndPoint::setApnInfoSync
( const std::string apn_name,
  const ApnTypes_t apnTypesBitmap)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    std::stringstream ss;
    ss << "[DSDModemEndPoint::setApnInfoSync]: apn=" << apn_name << " types=" << apnTypesBitmap;
    Log::getInstance().d(ss.str());

    do
    {
      if ((int32_t)apnTypesBitmap <= 0)
      {
        Log::getInstance().d("Invalid inputs");
        return Message::Callback::Status::SUCCESS;
      }
      if (isApnTypeFound(ApnTypes_t::DEFAULT, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DEFAULT_V01);
      }
      if (isApnTypeFound(ApnTypes_t::IMS, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IMS_V01);
      }
      if (isApnTypeFound(ApnTypes_t::MMS, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_MMS_V01);
      }
      if (isApnTypeFound(ApnTypes_t::DUN, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DUN_V01);
      }
      if (isApnTypeFound(ApnTypes_t::SUPL, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_SUPL_V01);
      }
      if (isApnTypeFound(ApnTypes_t::HIPRI, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_HIPRI_V01);
      }
      if (isApnTypeFound(ApnTypes_t::FOTA, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_FOTA_V01);
      }
      if (isApnTypeFound(ApnTypes_t::CBS, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_CBS_V01);
      }
      if (isApnTypeFound(ApnTypes_t::IA, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IA_V01);
      }
      if (isApnTypeFound(ApnTypes_t::EMERGENCY, apnTypesBitmap))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_EMERGENCY_V01);
      }
    } while(0);
    return Message::Callback::Status::SUCCESS;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::handleSetApnInfo]"
                         "Failed to send message SetApnInfoSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

void DSDModemEndPoint::generateDsdSystemStatusInd()
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::generateDsdSystemStatusInd]"
                         "dispatching message GetDsdSystemStatusMessage");
    auto msg = std::make_shared<GetDsdSystemStatusMessage>();
    GenericCallback<DsdSystemStatusResult_t> cb([](std::shared_ptr<Message>, Message::Callback::Status status,
                            std::shared_ptr<DsdSystemStatusResult_t> r) -> void {
      if (status == Message::Callback::Status::SUCCESS && r != nullptr) {
        auto indMsg = std::make_shared<rildata::DsdSystemStatusMessage>(r->resp_ind);
        indMsg->broadcast();
        auto perApnIndMsg = std::make_shared<rildata::DsdSystemStatusPerApnMessage>(r->apn_sys);
        perApnIndMsg->broadcast();
      } else {
        Log::getInstance().d("[DSDModemEndPoint::generateDsdSystemStatusInd] Failed to get system status");
      }
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    Log::getInstance().d("[DSDModemEndPoint::generateDsdSystemStatusInd]"
                         "Failed to send message GetDsdSystemStatusMessage");
  }
}

Message::Callback::Status DSDModemEndPoint::sendAPAssistIWLANSupported( )
{
  auto ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::sendAPAssistIWLANSupported]");

    dsd_set_capability_req_msg_v01 cap_req;
    dsd_set_capability_resp_msg_v01 cap_resp;

    memset(&cap_req, 0, sizeof(cap_req));
    memset(&cap_resp, 0, sizeof(cap_resp));

    //Send AP capability
    cap_req.ap_asst_mode_valid = true;
    cap_req.ap_asst_mode = DSD_AP_ASST_APN_PREF_SYS_ON_V01;

    int rc = sendRawSync(QMI_DSD_SET_CAPABILITY_REQ_V01,
                         (void *)&cap_req, sizeof(cap_req),
                         (void *)&cap_resp, sizeof(cap_resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (cap_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint:: QMI_DSD_SET_CAPABILITY_REQ_V01 is Successfully sent]");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::setV2Capabilities() {
  auto ret  = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setV2Capabilities]");
    dsd_set_capability_req_msg_v01 cap_req;
    dsd_set_capability_resp_msg_v01 cap_resp;
    memset(&cap_req, 0, sizeof(cap_req));
    memset(&cap_resp, 0, sizeof(cap_resp));

    cap_req.system_status_cap_valid = true;
    cap_req.system_status_cap = DSD_CAPABILITY_SYSTEM_STATUS_V2_V01;

    cap_req.ui_info_cap_valid = true;
    cap_req.ui_info_cap = DSD_CAPABILITY_UI_INFO_V2_V01;

    int rc = sendRawSync(QMI_DSD_SET_CAPABILITY_REQ_V01,
                         (void *)&cap_req, sizeof(cap_req),
                         (void *)&cap_resp, sizeof(cap_resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (cap_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      if (cap_resp.system_status_cap_valid && cap_resp.system_status_cap == DSD_CAPABILITY_SYSTEM_STATUS_V2_V01) {
        mUsingSystemStatusV2 = true;
      }

      if (cap_resp.ui_info_cap_valid && cap_resp.ui_info_cap == DSD_CAPABILITY_UI_INFO_V2_V01) {
        mUsingUiInfoV2 = true;
      }
      stringstream ss;
      ss << "[DSDModemEndPoint] QMI_DSD_SET_CAPABILITY_REQ_V01::Successfully sent. " << std::boolalpha <<
        "systemStatusV2=" << mUsingSystemStatusV2 << " uiInfoV2=" << mUsingUiInfoV2;
      Log::getInstance().d(ss.str());
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::registerForCurrentDDSInd ( )
{
  auto ret  = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForCurrentDDSInd]");
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;
    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_current_dds_valid = true;
    ind_req.report_current_dds = 1;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::registerForSmartTempDDSInd ( )
{
  auto ret  = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForSmartTempDDSInd]");
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;
    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_temp_dds_reco_cap_valid = true;
    ind_req.report_temp_dds_reco_cap = 1;

    ind_req.report_dds_recommendation_valid = true;
    ind_req.report_dds_recommendation = 1;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::registerForAPAsstIWlanInd (bool toRegister )
{
  /* To DO: Check whether this is needed for LE*/
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForAPAsstIWlanInd]");
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;
    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_intent_to_change_pref_sys_valid = true;
    ind_req.report_intent_to_change_pref_sys = toRegister;

    ind_req.report_ap_asst_apn_pref_sys_result_valid = true;
    ind_req.report_ap_asst_apn_pref_sys_result = toRegister;

    ind_req.report_pref_apn_update_valid = true;
    ind_req.report_pref_apn_update = toRegister;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);

    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_INDICATION_REGISTER_REQ_V01 is sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::registerForCIWLAN ( )
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;
    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_ciwlan_cap_valid = true;
    ind_req.report_ciwlan_cap = 1;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);

    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint::registerForCIWLAN] QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
      return Message::Callback::Status::SUCCESS;
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::registerForCIWLAN] Failed to send message registerForDDSInd");
  }
  return Message::Callback::Status::FAILURE;
}

Message::Callback::Status DSDModemEndPoint::registerForRoamingStatusChanged ( )
{
  auto ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForRoamingStatusChanged]");
    // Note that callback is not required for sync calls.
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;

    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_roaming_status_change_valid = true;
    ind_req.report_roaming_status_change = 1;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);

    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint::registerForRoamingStatusChanged]:"
                   "QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
      ret = Message::Callback::Status::SUCCESS;
    }

    //Query the modem for initial roaming status
    dsd_get_current_roaming_status_info_req_msg_v01 req_msg;
    dsd_get_current_roaming_status_info_resp_msg_v01 resp_msg;
    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    rc = sendRawSync(QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_REQ_V01,
                     (void *)&req_msg, sizeof(req_msg),
                     (void *)&resp_msg, sizeof(resp_msg),
                     QCRIL_DATA_QMI_TIMEOUT);

    if ((rc == QMI_NO_ERR) && (ind_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      dsd_roaming_status_change_ind_msg_v01 ind_msg;
      memset(&ind_msg, 0, sizeof(ind_msg));
      if(resp_msg.roaming_status_valid) {
        ind_msg.roaming_status_valid = true;
        ind_msg.roaming_status = resp_msg.roaming_status;
        auto msg = std::make_shared<ProcessCurrentRoamingStatus>(ind_msg);
        msg->broadcast();
      }
    }
  }
  return ret;
}

RIL_Errno DSDModemEndPoint::registerForDataDuringVoiceCall(bool enable)
{
  RIL_Errno status = RIL_E_GENERIC_FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForDataDuringVoiceCall] enable="
                          +std::to_string((int)enable)+
                          " mUserDdsSwitchSelection="+std::to_string((int)mUserDdsSwitchSelection));
    if (enable == mUserDdsSwitchSelection) {
      Log::getInstance().d("[DSDModemEndPoint] registered user selection is not changed");
      return RIL_E_SUCCESS;
    }

    dsd_configure_dynamic_dds_switch_req_msg_v01 switch_req {};
    dsd_configure_dynamic_dds_switch_resp_msg_v01 switch_resp {};

    // enable Temporary DDS recommendation action, Permanent DDS Recommendation Action is enabled by default
    switch_req.dynamic_dds_switch_setting = DSD_DYNAMIC_DDS_SWITCH_RECOMMENDATION_V01;
    switch_req.perm_rec_action_valid = true;
    switch_req.perm_rec_action = DSD_DDS_RECOMMENDATION_STOP_V01;
    switch_req.temp_rec_action_valid = true;
    switch_req.temp_rec_action = enable?
      DSD_DDS_RECOMMENDATION_START_V01:DSD_DDS_RECOMMENDATION_STOP_V01;

    int rc = sendRawSync(QMI_DSD_CONFIGURE_DYNAMIC_DDS_SWITCH_REQ_V01,
                    (void *)&switch_req, sizeof(switch_req),
                    (void *)&switch_resp, sizeof(switch_resp),
                    QCRIL_DATA_QMI_TIMEOUT);

    if ((rc == QMI_NO_ERR) && (switch_resp.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_CONFIGURE_DYNAMIC_DDS_SWITCH_REQ_V01::Successfully sent");
      status = RIL_E_SUCCESS;
    }

    if (status != RIL_E_GENERIC_FAILURE) {
      mUserDdsSwitchSelection = enable;
      Log::getInstance().d("[DSDModemEndPoint::mUserDdsSwitchSelection="+std::to_string((int)mUserDdsSwitchSelection));
    }
  }

  return status;
}

Message::Callback::Status DSDModemEndPoint::setApnPreferredSystemChangeSync(
  const std::string apnName, const int32_t prefRat)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setApnPreferredSystemChangeSync]"
                         "dispatching message SetApnPreferredSystemSyncMessage");

    dsd_set_apn_preferred_system_req_msg_v01 qmiReq;
    dsd_set_apn_preferred_system_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));

    strlcpy(qmiReq.apn_pref_sys.apn_name, apnName.c_str(), QMI_DSD_MAX_APN_LEN_V01+1);
    qmiReq.apn_pref_sys.pref_sys = (dsd_apn_pref_sys_enum_v01)prefRat;
    qmiReq.is_ap_asst_valid = true;
    qmiReq.is_ap_asst = true;
    auto retVal = sendRawSync(QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal == QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01 is sent Successfully");
      return Message::Callback::Status::SUCCESS;
    }
  }
  return Message::Callback::Status::FAILURE;
}

Message::Callback::Status DSDModemEndPoint::getCurrentDDSSync(DDSSubIdInfo &ddsInfo)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::getCurrentDDSSync]");
    dsd_get_current_dds_req_msg_v01 qmiReq;
    dsd_get_current_dds_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    auto retVal = sendRawSync(QMI_DSD_GET_CURRENT_DDS_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR || !qmiResp.dds_valid) {
      Log::getInstance().d("[DSDModemEndPoint] Failed to get dds");
      return Message::Callback::Status::FAILURE;
    }
    switch (qmiResp.dds) {
      case DSD_PRIMARY_SUBS_V01:
        ddsInfo.dds_sub_id = 0;
        break;
      case DSD_SECONDARY_SUBS_V01:
        ddsInfo.dds_sub_id = 1;
        break;
      default:
        Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_GET_CURRENT_DDS_REQ_V01 "
          "invalid subId=" + std::to_string(qmiResp.dds));
        return Message::Callback::Status::FAILURE;
    }
    ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01; //default value
    if(qmiResp.dds_switch_type_valid && qmiResp.dds_switch_type == DSD_DDS_SWITCH_TEMPORARY_V01) {
      ddsInfo.switch_type = DSD_DDS_DURATION_TEMPORARY_V01;
    }
    return Message::Callback::Status::SUCCESS;
  }
  return Message::Callback::Status::FAILURE;
}

Message::Callback::Status DSDModemEndPoint::triggerDDSSwitch(int subId, int& error, dsd_dds_switch_type_enum_v01 switch_type)
{
  auto ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitch]");
    dsd_switch_dds_req_msg_v01 reqMsg;
    dsd_switch_dds_resp_msg_v01 respMsg;

    memset(&reqMsg, 0, sizeof(reqMsg));
    memset(&respMsg, 0, sizeof(respMsg));

    switch (subId) {
      case 0:
        reqMsg.subscription = DSD_PRIMARY_SUBS_V01;
        break;
      case 1:
        reqMsg.subscription = DSD_SECONDARY_SUBS_V01;
        break;
      default:
        respMsg.resp.error = QMI_ERR_INVALID_ARG_V01;
        return ret;
    }

    reqMsg.dds_switch_type_valid = true;
    reqMsg.dds_switch_type = switch_type;

    auto retVal = sendRawSync(QMI_DSD_SWITCH_DDS_REQ_V01,
                              (void *)&reqMsg, sizeof(reqMsg),
                              (void *)&respMsg, sizeof(respMsg),
                              QCRIL_DATA_QMI_TIMEOUT);
    if ((retVal == QMI_NO_ERR) && (respMsg.resp.result != QMI_RESULT_FAILURE_V01)) {
      Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitch] QMI_DSD_SWITCH_DDS_REQ_V01::Successfully sent");
      ret = Message::Callback::Status::SUCCESS;
    } else {
      error = static_cast<int>(SetPreferredDataModemResult_t::QMI_ERROR);
    }
  }
  else
  {
    error = static_cast<int>(SetPreferredDataModemResult_t::MODEM_ENDPOINT_NOT_OPERATIONAL);
    auto msg = std::make_shared<DDSSwitchResultIndMessage>();
    msg->setError(TriggerDDSSwitchError::MODEM_NOT_UP);
    msg->broadcast();
  }
  return ret;
}

Message::Callback::Status DSDModemEndPoint::getDataSystemCapabilitySync()
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::getDataSystemCapabilitySync]");
    dsd_get_capability_req_msg_v01 req_msg;
    dsd_get_capability_resp_msg_v01 resp_msg;
    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    req_msg.temp_dds_rec_capability_valid = 1;
    req_msg.temp_dds_rec_capability = 1;

    int rc = sendRawSync(QMI_DSD_GET_CAPABILITY_REQ_V01,
                        (void *)&req_msg, sizeof(req_msg),
                        (void *)&resp_msg, sizeof(resp_msg),
                        QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (resp_msg.resp.result != QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_GET_CAPABILITY_REQ_V01::Successfully sent");
      if (resp_msg.temp_dds_rec_capability_valid) {
        bool capability = resp_msg.temp_dds_rec_capability ? true : false;
        if (!mDdsSwitchRecCapEnabled.has_value() ||
            (mDdsSwitchRecCapEnabled.has_value() &&
            mDdsSwitchRecCapEnabled.value() != capability)) {
          mDdsSwitchRecCapEnabled = capability;
          if (capability) {
            auto msg = std::make_shared<rildata::AutoDdsSwitchControlIndMessage>
                                        (AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_CAPABILITY_CHANGED);
            msg->broadcast();
          }
        }
        Log::getInstance().d("[DSDModemEndPoint]: temp rec capability="+std::to_string((int)capability));
      }
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::getDataSystemCapabilitySync]"
                         "Failed to query - not operational");
    return Message::Callback::Status::FAILURE;
  }
  return Message::Callback::Status::SUCCESS;
}

Message::Callback::Status DSDModemEndPoint::getMasterRILSpecificCapability()
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::getMasterRILSpecificCapability]");
    dsd_get_capability_req_msg_v01 req_msg;
    dsd_get_capability_resp_msg_v01 resp_msg;
    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    req_msg.ciwlan_capability_valid = 1;
    req_msg.ciwlan_capability = 1;

    int rc = sendRawSync(QMI_DSD_GET_CAPABILITY_REQ_V01,
                        (void *)&req_msg, sizeof(req_msg),
                        (void *)&resp_msg, sizeof(resp_msg),
                        QCRIL_DATA_QMI_TIMEOUT);
    if ((rc != QMI_NO_ERR) || (resp_msg.resp.result == QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPoint] Failed to send QMI_DSD_GET_CAPABILITY_REQ_V01");
    } else{
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_GET_CAPABILITY_REQ_V01::Successfully sent");
      if (resp_msg.ciwlan_capability_valid) {
        bool capability = resp_msg.ciwlan_capability ? true : false;
        rildata::CIWlanCapabilityType type = rildata::CIWlanCapabilityType::None;
        if (resp_msg.ciwlan_subs_support_valid)
        {
          type = (resp_msg.ciwlan_subs_support == DSD_CIWLAN_SUBS_SUPPORT_DDS_ONLY_V01) ?
            rildata::CIWlanCapabilityType::DDSSupported :
            rildata::CIWlanCapabilityType::BothSubsSupported;
        }
        if ((mCIWlanCapability != capability) || (mCIWlanCapType != type))
        {
          mCIWlanCapability = capability;
          mCIWlanCapType = type;
          auto msg = std::make_shared<DataCapabilityChangeIndMessage>();
          msg->setCIWlanSupported(capability, type);
          msg->broadcast();
        }
        Log::getInstance().d("[DSDModemEndPoint]: ciwlan capability="+std::to_string((int)capability)
                             + " ciwlan type="+std::to_string((int)type));
      }
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::getMasterRILSpecificCapability]"
                         "Failed to query - not operational");
    return Message::Callback::Status::FAILURE;
  }
  return Message::Callback::Status::SUCCESS;
}

int DSDModemEndPoint::reportSystemStatusChange(bool report_null_bearer, bool report_so_mask)
{
  Log::getInstance().d("[DSDModemEndPoint]: reportSystemStatusChange report_null_bearer "
        +std::to_string((int)report_null_bearer) + " report_so_mask " + std::to_string((int)report_so_mask));

  dsd_system_status_change_req_msg_v01  qmiReq;
  dsd_system_status_change_resp_msg_v01 qmiResp;
  memset(&qmiReq, 0, sizeof(qmiReq));
  memset(&qmiResp, 0, sizeof(qmiResp));

  if(report_null_bearer) {
    qmiReq.report_null_bearer_reason_valid = true;
    qmiReq.report_null_bearer_reason = true;
  }

  if(mReportLimitedSysIndicationChange != report_so_mask) {
      qmiReq.limit_so_mask_change_ind_valid = true;
      qmiReq.limit_so_mask_change_ind = report_so_mask;
  }

  auto retVal = sendRawSync(QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                           (void *)&qmiReq, sizeof(qmiReq),
                           (void *)&qmiResp, sizeof(qmiResp),
                           QCRIL_DATA_QMI_TIMEOUT);
  if (retVal != QMI_NO_ERR) {
    Log::getInstance().d("[DSDModemEndPoint::toggleLimitedSysIndicationChangeReport] Failed to register");
  }

  /* when mReportLimitedSysIndicationChange = true, limit the DsdSystemStatusMessage*/
  if (!mReportLimitedSysIndicationChange && (mReportLimitedSysIndicationChange != report_so_mask))
  {
    generateDsdSystemStatusInd();
  }

  if(mReportLimitedSysIndicationChange != report_so_mask)
    mReportLimitedSysIndicationChange = report_so_mask;

  return 0;
}

Message::Callback::Status DSDModemEndPoint::handleScreenStateChangeInd(bool screenEnabled)
{
  dsd_register_ui_info_change_req_msg_v01 qmiReq;
  memset(&qmiReq, 0, sizeof(qmiReq));

  if( ((screenEnabled == true ) && ( mScreenState == ScreenState_t::SCREEN_ON )) ||
      ((screenEnabled == false ) &&
       ( mScreenState == ScreenState_t::SCREEN_OFF  || mScreenState == ScreenState_t::NONE)))
  {
    Log::getInstance().d("[DSDModemEndPoint::No change in Screen State!!"
                         "Screen State is " + std::to_string((int)mScreenState));
    return Message::Callback::Status::SUCCESS;
  }

  if (screenEnabled == mUiFilterRegistered) {
    Log::getInstance().d("[DSDModemEndPoint]::UI Indication Registration is already in the requested state");
    mScreenState = ( screenEnabled ) ? ScreenState_t::SCREEN_ON : ScreenState_t::SCREEN_OFF;
    return Message::Callback::Status::SUCCESS;
  }

  uint8_t isRegisterForUiInfo = (screenEnabled)?1:0;
  auto status = sendRegisterForUiChangeInd(isRegisterForUiInfo);
  if (status == Message::Callback::Status::SUCCESS) {
    mUiFilterRegistered = screenEnabled;
    mScreenState = ScreenState_t::SCREEN_OFF;
    if(screenEnabled) {
      mScreenState = ScreenState_t::SCREEN_ON;
      updateInitialUiInfo();
    }
  }
  return status;
}

#ifdef RIL_FOR_MDM_LE
bool DSDModemEndPoint::uiChangeInfoRegistrationRequest(bool regValue)
{
  if (mUiFilterRegistered == regValue) {
    Log::getInstance().d("[DSDModemEndPoint]:: Registration State is Matching with Requested State.");
    return true;
  }
  uint8_t isRegisterForUiInfo = (regValue)?1:0;
  Message::Callback::Status status = sendRegisterForUiChangeInd(isRegisterForUiInfo);
  if (status == Message::Callback::Status::SUCCESS && regValue) {
    updateInitialUiInfo();
    mUiFilterRegistered = regValue;
    return true;
  }
  return false;
}
#endif

void DSDModemEndPoint::registerForUiChangeInd()
{
  if( (!(mNrIconReportDisabled)) && (mScreenState == ScreenState_t::SCREEN_ON || mUiFilterRegistered))
  {
    Log::getInstance().d("[DSDModemEndPoint:: Screen State is ON , register for UI indication");
    Message::Callback::Status status = sendRegisterForUiChangeInd(1);
    if (status == Message::Callback::Status::SUCCESS) {
      updateInitialUiInfo();
      mUiFilterRegistered = true;
    }
  }
}

Message::Callback::Status DSDModemEndPoint::sendRegisterForUiChangeInd(uint8_t isRegisterForUiInfo)
{
  std::stringstream ss;
  ss << std::boolalpha << isRegisterForUiInfo;
  Log::getInstance().d("[DSDModemEndPoint:: sendRegisterForUiChangeInd]"+ ss.str());

  Message::Callback::Status status = Message::Callback::Status::FAILURE;
  dsd_register_ui_info_change_req_msg_v01 qmiReq;
  dsd_register_ui_info_change_resp_msg_v01 qmiResp;
  memset(&qmiReq, 0, sizeof(qmiReq));
  memset(&qmiResp, 0, sizeof(qmiResp));

  if(isRegisterForUiInfo) {
    qmiReq.suppress_so_change_valid = true;
    qmiReq.suppress_so_change = 1;
    qmiReq.suppress_null_bearer_reason_valid = true;
    qmiReq.suppress_null_bearer_reason = 1;
  }
  qmiReq.report_ui_changes_valid = true;
  qmiReq.report_ui_changes = isRegisterForUiInfo;

  auto retVal = sendRawSync(QMI_DSD_REGISTER_UI_INFO_CHANGE_REQ_V01,
                            (void *)&qmiReq, sizeof(qmiReq),
                            (void *)&qmiResp, sizeof(qmiResp),
                            QCRIL_DATA_QMI_TIMEOUT);

  if (retVal == QMI_NO_ERR) {
    status = Message::Callback::Status::SUCCESS;
  }
  return status;
}

void DSDModemEndPoint::updateInitialUiInfo()
{
  auto msg = std::make_shared<GetDataNrIconTypeMessage>();
  GenericCallback<NrIconType_t> cb([](std::shared_ptr<Message>,
                                      Message::Callback::Status status,
                                      std::shared_ptr<NrIconType_t> rsp) -> void {
    if (rsp != nullptr) {
      Log::getInstance().d("updateInitialUiInfo cb invoked status " + std::to_string((int)status));
      auto indMsg = std::make_shared<DataNrIconTypeIndMessage>(*rsp);
      indMsg->broadcast();
    }
  });
  msg->setCallback(&cb);
  msg->dispatch();
}

RIL_Errno DSDModemEndPoint::setIsDataEnabled(bool enabled)
{
  RIL_Errno errNum =  RIL_E_GENERIC_FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setIsDataEnabled] enabled=" + std::to_string(enabled));
    dsd_notify_data_settings_req_msg_v01  qmiReq;
    dsd_notify_data_settings_resp_msg_v01 qmiResp;

    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    qmiReq.data_service_switch_valid = TRUE;
    qmiReq.data_service_switch = enabled;

    auto retVal = sendRawSync(QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal == QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] Failed to send QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01");
      errNum = RIL_E_SUCCESS;
    }
  }
  return errNum;
}

RIL_Errno DSDModemEndPoint::setIsDataRoamingEnabled(bool enabled)
{
  RIL_Errno errNum =  RIL_E_GENERIC_FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setIsDataRoamingEnabled] enabled =" +std::to_string(enabled));
    dsd_notify_data_settings_req_msg_v01  qmiReq;
    dsd_notify_data_settings_resp_msg_v01 qmiResp;

    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    qmiReq.data_service_roaming_switch_valid = TRUE;
    qmiReq.data_service_roaming_switch = enabled;

    auto retVal = sendRawSync(QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal == QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01 is sent Successfully");
      errNum = RIL_E_SUCCESS;
    }
  }
  return errNum;
}

RIL_Errno DSDModemEndPoint::setCIWlanUIOptionSelection(bool enabled)
{
  RIL_Errno errNum =  RIL_E_GENERIC_FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setCIWlanUIOptionSelection] enabled =" + std::to_string(enabled));
    dsd_notify_data_settings_req_msg_v01  qmiReq;
    dsd_notify_data_settings_resp_msg_v01 qmiResp;

    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    qmiReq.ciwlan_switch_valid = TRUE;
    qmiReq.ciwlan_switch = enabled;

    auto retVal = sendRawSync(QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal == QMI_NO_ERR) {
      mCIWlanUISelection = enabled;
      errNum = RIL_E_SUCCESS;
      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01 is sent Successfully"
                           "mCIWlanUISelection updated ="+std::to_string(mCIWlanUISelection));
    }
  }
  return errNum;
}

bool DSDModemEndPoint::deviceShutdownRequest()
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::deviceShutdownRequest]");
    dsd_notify_data_settings_req_msg_v01 qmiReq;
    dsd_notify_data_settings_resp_msg_v01 qmiResp;

    memset(&qmiReq, 0, sizeof(dsd_notify_data_settings_req_msg_v01));
    memset(&qmiResp, 0, sizeof(dsd_notify_data_settings_resp_msg_v01));

    qmiReq.wifi_switch_valid = true;
    qmiReq.wifi_switch = false;

    auto retVal = sendRawSync(QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                (void *)&qmiReq, sizeof(qmiReq),
                                (void *)&qmiResp, sizeof(qmiResp),
                                QCRIL_DATA_QMI_TIMEOUT);
    if (QMI_NO_ERR == retVal) {
      Log::getInstance().d("[DSDModemEndPoint]::QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01 is sent Successfully");
      return true;
    }
  }
  return false;
}

qmi_response_type_v01 DSDModemEndPoint::setQualityMeasurement
(
  dsd_set_quality_measurement_info_req_msg_v01 qmiReq
)
{
  qmi_response_type_v01 resp;
  memset(&resp, 0, sizeof(resp));
  resp.result = QMI_RESULT_FAILURE_V01;
  resp.error = QMI_ERR_INTERNAL_V01;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setQualityMeasurement]");
    dsd_set_quality_measurement_info_resp_msg_v01 qmiResp;
    memset(&qmiResp, 0, sizeof(qmiResp));

    auto rc = sendRawSync(QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_REQ_V01,
                                  (void *)&qmiReq, sizeof(qmiReq),
                                  (void *)&qmiResp, sizeof(qmiResp),
                                  QCRIL_DATA_QMI_TIMEOUT);

    if (QMI_NO_ERR == rc) {
      resp = qmiResp.resp;
      Log::getInstance().d("qcril_data_set_quality_measurement():"
          " result"+std::to_string(resp.result)+" err=" +std::to_string(resp.error));
    }
  }
  return resp;
}

#ifdef QMI_RIL_UTF
void DSDModemEndPoint::cleanup() {
  mScreenState = ScreenState_t::SCREEN_ON;
  mUiFilterRegistered = false;
  mCIWlanCapability = false;
  mCIWlanCapType = None;
  mCIWlanUISelection = false;
}
#endif
