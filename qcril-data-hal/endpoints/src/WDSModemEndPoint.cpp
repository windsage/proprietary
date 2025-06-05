/*==============================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
#include "WDSModemEndPoint.h"
#include "qcril_data.h"
#include "request/GetPdnThrottleTimeMessage.h"
#include "UnSolMessages/GetPdnThrottleTimeResponseInd.h"
#include "MessageCommon.h"
#include "QdpAdapter.h"
#include <sstream>

using std::to_string;
using namespace qdp;

constexpr const char *WDSModemEndPoint::NAME;

void WDSModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}

Message::Callback::Status WDSModemEndPoint::getAttachList
(
  std::shared_ptr<std::list<uint16_t>>& attach_list
)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  // Allow any QMI IDL API invocation only if we are in OPERATIONAL state.
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::getAttachList] : ENTRY");
#ifndef RIL_FOR_MODEM_LITE
    wds_get_lte_attach_pdn_list_req_msg_v01 req;
    wds_get_lte_attach_pdn_list_resp_msg_v01 resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    auto rc = sendRawSync(QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&resp, sizeof(resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint]:QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01 is sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
      std::list<uint16_t> resp_attach_list;
      if (resp.attach_pdn_list_valid)
      {
        if( resp.pending_attach_pdn_list_valid )
        {
          for (uint32_t i = 0; i < resp.pending_attach_pdn_list_len; i++)
          {
            Log::getInstance().d("[WDSModemEndPoint]:Attach pending pdn list profile = "
                                      +std::to_string(resp.pending_attach_pdn_list[i]));
            resp_attach_list.push_back(resp.pending_attach_pdn_list[i]);
          }
        }
        else
        {
          for (uint32_t i = 0; i < resp.attach_pdn_list_len; i++)
          {
            Log::getInstance().d("[WDSModemEndPoint]:Attach pdn list profile = "
                                  +std::to_string(resp.attach_pdn_list[i]));
            resp_attach_list.push_back(resp.attach_pdn_list[i]);
          }
        }
      }
      attach_list = std::make_shared<std::list<uint16_t>>(resp_attach_list);
    }
#else
    wds_get_lte_attach_pdn_list_lite_req_msg_v01 req;
    wds_get_lte_attach_pdn_list_lite_resp_msg_v01 resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    auto rc = sendRawSync(QMI_WDS_GET_LTE_ATTACH_PDN_LIST_LITE_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&resp, sizeof(resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint]:QMI_WDS_GET_LTE_ATTACH_PDN_LIST_LITE_REQ_V01 is sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
      std::list<uint16_t> resp_attach_list;
      if( resp.attach_pdp_type_valid)
      {
        for (uint32_t i = 0; i < resp.attach_pdp_type_len; i++)
        {
          std::string apn = "";
          Log::getInstance().d("[WDSModemEndPoint]:Attach pdn list profile pdp Type = "
                                    + std::to_string(resp.attach_pdp_type[i]));
          if (resp.attach_apn_name_valid && i < resp.attach_apn_name_len) {
            apn = std::string(resp.attach_apn_name[i].attach_apn);
          }
          Log::getInstance().d("[WDSModemEndPoint]:Attach pdn list profile apn name = " + apn);
          resp_attach_list.push_back(getProfileId(resp.attach_pdp_type[i], apn));
        }
      }
      else if (resp.pending_attach_pdp_type_valid)
      {
        std::string apn = "";
        for (uint32_t i = 0; i < resp.pending_attach_pdp_type_len; i++)
        {
          Log::getInstance().d("[WDSModemEndPoint]:Attach pending pdn list profile = "
                                +std::to_string(resp.pending_attach_pdp_type[i]));
          if (resp.pending_attach_apn_name_valid && i < resp.pending_attach_apn_name_len) {
            apn = std::string(resp.pending_attach_apn_name[i].attach_apn);
          }
          Log::getInstance().d("[WDSModemEndPoint]:Attach pending pdn list profile apn name = " + apn);
          resp_attach_list.push_back(getProfileId(resp.pending_attach_pdp_type[i], apn));
        }
      }
      attach_list = std::make_shared<std::list<uint16_t>>(resp_attach_list);
    }
#endif
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::getLteAttachParams(wds_get_lte_attach_params_resp_msg_v01& attach_param)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;

  if(getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::getLteAttachParams] : ENTRY");
    wds_get_lte_attach_params_req_msg_v01 req;
    memset(&req, 0, sizeof(req));

    auto rc = sendRawSync(QMI_WDS_GET_LTE_ATTACH_PARAMS_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&attach_param, sizeof(attach_param),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (attach_param.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint]::QMI_WDS_GET_LTE_ATTACH_PARAMS_REQ_V01 is sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::setAttachList
(
  const std::shared_ptr<std::list<uint16_t>>& attach_list,
  const rildata::AttachListAction action
)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::setAttachList] : ENTRY");
#ifndef RIL_FOR_MODEM_LITE
    wds_set_lte_attach_pdn_list_req_msg_v01 req;
    wds_set_lte_attach_pdn_list_resp_msg_v01 resp;

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.attach_pdn_list_len = (*attach_list).size();
    std::list<uint16_t>::const_iterator it;
    uint8_t i = 0;
    for(auto it : (*attach_list))
    {
      req.attach_pdn_list[i++] = (it);
      Log::getInstance().d("[WDSModemEndPoint]:elements set in attach list ="+ std::to_string(it));
    }

    if( action == rildata::AttachListAction::DISCONNECT_ATTACH_APN_ONLY )
    {
      req.action_valid = TRUE;
      req.action = wds_attach_pdn_list_action_enum_type_v01::WDS_ATTACH_PDN_LIST_ACTION_DETACH_OR_PDN_DISCONNECT_V01;
    }
    else if (action == rildata::AttachListAction::NONE )
    {
      req.action_valid = FALSE;
      req.action = wds_attach_pdn_list_action_enum_type_v01::WDS_ATTACH_PDN_LIST_ACTION_NONE_V01;
    }

    auto rc = sendRawSync(QMI_WDS_SET_LTE_ATTACH_PDN_LIST_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&resp, sizeof(resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint] : QMI_WDS_SET_LTE_ATTACH_PDN_LIST_REQ_V01 sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
    }
#else
    wds_set_lte_attach_pdn_list_lite_req_msg_v01 req;
    wds_set_lte_attach_pdn_list_lite_resp_msg_v01 resp;

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.pdp_type_len = (*attach_list).size();
    req.apn_name_len = (*attach_list).size();
    if (req.apn_name_len > 0)
       req.apn_name_valid = true;
    std::list<uint16_t>::const_iterator it;
    uint8_t i = 0;

    for(auto it : (*attach_list))
    {
      std::string apnName = "";
      qdp::IPType protocol;
      getApnNameProtocol (it, apnName, protocol);
      req.pdp_type[i] = convertToWdsPdpType(protocol);
      strlcpy(req.apn_name[i].attach_apn, apnName.c_str(), QMI_WDS_APN_NAME_MAX_V01);
      i++;
      Log::getInstance().d("[WDSModemEndPoint]:elements set in attach list ="+ std::to_string(it));
    }

    if( action == rildata::AttachListAction::DISCONNECT_ATTACH_APN_ONLY )
    {
      req.action_valid = TRUE;
      req.action = wds_attach_pdn_list_action_enum_type_v01::WDS_ATTACH_PDN_LIST_ACTION_DETACH_OR_PDN_DISCONNECT_V01;
    }
    else if (action == rildata::AttachListAction::NONE )
    {
      req.action_valid = FALSE;
      req.action = wds_attach_pdn_list_action_enum_type_v01::WDS_ATTACH_PDN_LIST_ACTION_NONE_V01;
    }

    auto rc = sendRawSync(QMI_WDS_SET_LTE_ATTACH_PDN_LIST_LITE_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&resp, sizeof(resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint] : QMI_WDS_SET_LTE_ATTACH_PDN_LIST_LITE_REQ_V01 sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
    }
#endif
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::getAttachListCapability
(
  rildata::AttachListCap& cap
)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::getAttachListCapability] : ENTRY");

    wds_get_capabilities_req_msg_v01 req;
    wds_get_capabilities_resp_msg_v01 resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.attach_pdn_list_action_capability_valid = TRUE;
    req.attach_pdn_list_action_capability = AttachAction::ATTACH_PDN_ACTION_SUPPORTED;

    auto rc = sendRawSync(QMI_WDS_GET_CAPABILITIES_REQ_V01,
                     (void *)&req, sizeof(req),
                     (void*)&resp, sizeof(resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint] : QMI_WDS_GET_CAPABILITIES_REQ_V01 sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
      cap = rildata::AttachListCap::ACTION_NOT_SUPPORTED;

      if (resp.attach_pdn_list_action_capability_valid &&
          (resp.attach_pdn_list_action_capability == AttachAction::ATTACH_PDN_ACTION_SUPPORTED))
        {
          Log::getInstance().d("[WDSModemEndPoint]:Get Capabilities : Action_Supported");
          cap = rildata::AttachListCap::ACTION_SUPPORTED;
        }
    }
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::getCallBringUpCapability
(
  rildata::BringUpCapability& callBringUpCapability
)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::getCallBringUpCapability] : ENTRY");
    wds_get_capabilities_req_msg_v01 ps_req;
    wds_get_capabilities_resp_msg_v01 ps_resp;
    memset(&ps_req, 0, sizeof(wds_get_capabilities_req_msg_v01));
    memset(&ps_resp, 0, sizeof(wds_get_capabilities_resp_msg_v01));

    ps_req.bring_up_capability_valid = TRUE;
    ps_req.bring_up_capability = TRUE;
    ps_req.is_clat_supported_valid = TRUE;
    ps_req.is_clat_supported = TRUE;

    callBringUpCapability = rildata::BringUpCapability::BRING_UP_LEGACY;
    global_qcril_clat_supported = false;

    auto rc = sendRawSync(QMI_WDS_GET_CAPABILITIES_REQ_V01,
                     (void *)&ps_req, sizeof(ps_req),
                     (void*)&ps_resp, sizeof(ps_resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (ps_resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint] : QMI_WDS_GET_CAPABILITIES_REQ_V01 sent Successfully");
      ret = Message::Callback::Status::SUCCESS;

      if (TRUE == ps_resp.bring_up_capability_valid) {
        if(ps_resp.bring_up_capability & WDS_BRING_UP_BY_APN_TYPE_V01) {
          callBringUpCapability = (rildata::BringUpCapability)((int)callBringUpCapability | (int)BringUpCapability::BRING_UP_APN_TYPE);
        }
        if(ps_resp.bring_up_capability & WDS_BRING_UP_BY_APN_NAME_V01) {
          callBringUpCapability = (rildata::BringUpCapability)((int)callBringUpCapability | (int)BringUpCapability::BRING_UP_APN_NAME);
        }
      }

      if (ps_resp.is_clat_supported_valid && ps_resp.is_clat_supported) {
        global_qcril_clat_supported = true;
      }
    }
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::setV2Capabilities(bool nswo) {
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint::setV2Capabilities] Entry");
    wds_set_capabilities_req_msg_v01 cap_req;
    wds_set_capabilities_resp_msg_v01 cap_resp;

    memset(&cap_req, 0, sizeof(cap_req));
    memset(&cap_resp, 0, sizeof(cap_resp));

    cap_req.pdn_throttle_cap_valid = true;
    cap_req.pdn_throttle_cap = WDS_CAPABILITY_PDN_THROTTLE_V2_V01;
    if(nswo) {
      cap_req.nswo_cap_valid = true;
      cap_req.nswo_cap = 1;
    }

    mUsingPdnThrottleV2 = false;

    auto rc = sendRawSync(QMI_WDS_SET_CAPABILITIES_REQ_V01,
                     (void *)&cap_req, sizeof(cap_req),
                     (void*)&cap_resp, sizeof(cap_resp),
                     DEFAULT_SYNC_TIMEOUT);

    if( rc == QMI_NO_ERR && (cap_resp.resp.result == QMI_RESULT_SUCCESS_V01))
    {
      Log::getInstance().d("[WDSModemEndPoint] : QMI_WDS_SET_CAPABILITIES_REQ_V01 sent Successfully");
      ret = Message::Callback::Status::SUCCESS;

      if(cap_resp.pdn_throttle_cap_valid &&
        cap_resp.pdn_throttle_cap == WDS_CAPABILITY_PDN_THROTTLE_V2_V01) {
        mUsingPdnThrottleV2 = true;
        std::stringstream ss;
        ss << "[WDSModemEndPoint] " << std::boolalpha <<"pdnThrottleV2=" << mUsingPdnThrottleV2;
        Log::getInstance().d(ss.str());
      }
    }
  }
  return ret;
}

void WDSModemEndPoint::getPdnThrottleTime(std::string apn, DataProfileInfoType_t techType, std::string ipType, int cid) {
  auto msg = std::make_shared<GetPdnThrottleTimeMessage>(apn, techType, ipType);
  GenericCallback<int64_t> cb([cid](std::shared_ptr<Message>, Message::Callback::Status status,
                          std::shared_ptr<int64_t> r) -> void {
    if (status == Message::Callback::Status::SUCCESS && r != nullptr) {
      auto indMsg = std::make_shared<GetPdnThrottleTimeResponseInd>(cid, *r);
      indMsg->broadcast();
    } else {
      Log::getInstance().d("[WDSModemEndPoint::getPdnThrottleTime] Failed to get throttle time");
      auto indMsg = std::make_shared<GetPdnThrottleTimeResponseInd>(cid, -1);
      indMsg->broadcast();
    }
  });
  msg->setCallback(&cb);
  msg->dispatch();
}

#ifndef RIL_FOR_MODEM_LITE
Message::Callback::Status WDSModemEndPoint::setDefaultProfileNum(TechType techType, ProfileId index)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if(getState() == ModemEndPoint::State::OPERATIONAL) {
    Log::getInstance().d("[WDSModemEndPoint::setDefaultProfileNum] = "+std::to_string(index));

    wds_set_default_profile_num_req_msg_v01 ps_req;
    wds_set_default_profile_num_resp_msg_v01 ps_resp;

    memset(&ps_req, 0, sizeof(wds_set_default_profile_num_req_msg_v01));
    memset(&ps_resp, 0, sizeof(wds_set_default_profile_num_resp_msg_v01));
    ps_req.profile_identifier.profile_family = WDS_PROFILE_FAMILY_EMBEDDED_V01;
    switch (techType) {
      case TechType::THREE_GPP:
        ps_req.profile_identifier.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
        break;
      case TechType::THREE_GPP2:
        ps_req.profile_identifier.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
        break;
      default:
        Log::getInstance().d("[WDSModemEndPoint::setDefaultProfileNum] = invalid tech type");
        break;
    }
    ps_req.profile_identifier.profile_index = (uint8_t)index;

    auto rc = sendRawSync(QMI_WDS_SET_DEFAULT_PROFILE_NUM_REQ_V01,
                  (void*)&ps_req, sizeof(ps_req),
                  (void*)&ps_resp, sizeof(ps_resp),
                  DEFAULT_SYNC_TIMEOUT);

    if(rc == QMI_NO_ERR && (ps_resp.resp.result == QMI_RESULT_SUCCESS_V01)) {
      Log::getInstance().d("[WDSModemEndPoint]::QMI_WDS_SET_DEFAULT_PROFILE_NUM_V01 is sent Successfully");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}
#else
Message::Callback::Status WDSModemEndPoint::setDefaultProfileNum(TechType techType, ProfileId index)
{
    (void) techType;
    Message::Callback::Status ret = Message::Callback::Status::FAILURE;

    if(getState() == ModemEndPoint::State::OPERATIONAL) {
      std::string apn = "";
      qdp::IPType protocol;
      getApnNameProtocol (index, apn, protocol);

      wds_set_default_profile_lite_req_msg_v01 ps_req;
      wds_set_default_profile_lite_resp_msg_v01 ps_resp;
      memset(&ps_req, 0, sizeof(wds_set_default_profile_lite_req_msg_v01));
      memset(&ps_resp, 0, sizeof(wds_set_default_profile_lite_resp_msg_v01));

      if (protocol == IPType::V4) {
        ps_req.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
      } else if (protocol == IPType::V6) {
        ps_req.pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
      } else {
        ps_req.pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
      }
      if (!apn.empty()) {
        ps_req.apn_name_valid = true;
        strlcpy(ps_req.apn_name, apn.c_str(), apn.length()+1);
      }
      auto rc = sendRawSync(QMI_WDS_SET_DEFAULT_PROFILE_LITE_REQ_V01,
                  (void*)&ps_req, sizeof(ps_req),
                  (void*)&ps_resp, sizeof(ps_resp),
                  DEFAULT_SYNC_TIMEOUT);

      if(rc == QMI_NO_ERR && (ps_resp.resp.result == QMI_RESULT_SUCCESS_V01)) {
        Log::getInstance().d("[WDSModemEndPoint]::QMI_WDS_SET_DEFAULT_PROFILE_LITE_REQ_V01 is sent Successfully");
        ret = Message::Callback::Status::SUCCESS;
      }
    }
    return ret;
}
#endif

bool WDSModemEndPoint::getReportingStatus()
{
  return mIsReportDataRegistrationRejectCause;
}

bool WDSModemEndPoint::getDataRegistrationState()
{
  return mIsDataRegistered;
}

void WDSModemEndPoint::updateDataRegistrationState(bool registered)
{
  mIsDataRegistered = registered;
  Log::getInstance().d("[WDSModemEndPoint::updateDataRegistrationState] = "+
                        std::to_string(mIsDataRegistered));
}

void WDSModemEndPoint::registerforWdsIndication()
{
  if(getState() == ModemEndPoint::State::OPERATIONAL) {
    Log::getInstance().d("WDSModemEndPoint::registerforIndications");
    wds_indication_register_req_msg_v01 req;
    wds_indication_register_resp_msg_v01 resp;

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.report_td_info_valid = TRUE;
    req.report_td_info = 1;

    req.report_pdn_throttle_info_valid = true;
    req.report_pdn_throttle_info = 1;

    req.report_ma_keep_alive_status_valid = true;
    req.report_ma_keep_alive_status = true;

    auto rc = sendRawSync(QMI_WDS_INDICATION_REGISTER_REQ_V01,
                          (void*)&req, sizeof(req),
                          (void*)&resp, sizeof(resp),
                          DEFAULT_SYNC_TIMEOUT);

    if (rc == QMI_NO_ERR && resp.resp.result == QMI_RESULT_SUCCESS_V01)
    {
      Log::getInstance().d("[WDSModemEndPoint]:QMI_WDS_INDICATION_REGISTER_REQ_V01 is Successfully sent");
    }
  }
}

Message::Callback::Status WDSModemEndPoint::registerDataRegistrationRejectCause(bool enable)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  Log::getInstance().d("[WDSModemEndPoint::registerDataRegistrationRejectCause] = "+
                        std::to_string(enable));
  if (mIsReportDataRegistrationRejectCause != enable) {
    if(getState() == ModemEndPoint::State::OPERATIONAL) {

        wds_indication_register_req_msg_v01 req;
        wds_indication_register_resp_msg_v01 resp;

        memset(&req, 0, sizeof(req));
        memset(&resp, 0, sizeof(resp));
        req.report_lte_attach_failure_info_valid = true;
        req.report_lte_attach_failure_info = enable;
        auto rc = sendRawSync(QMI_WDS_INDICATION_REGISTER_REQ_V01,
                              (void*)&req, sizeof(req),
                              (void*)&resp, sizeof(resp),
                              DEFAULT_SYNC_TIMEOUT);

        if(rc == QMI_NO_ERR && resp.resp.result == QMI_RESULT_SUCCESS_V01) {
          Log::getInstance().d("[WDSModemEndPoint]::QMI_WDS_INDICATION_REGISTER_REQ_V01 is Successfully sent");
          mIsReportDataRegistrationRejectCause = enable;
          ret = Message::Callback::Status::SUCCESS;
        }
    } else {
        Log::getInstance().d("[WDSModemEndPoint]::Modem is non operational");
    }
  }
  return ret;
}

Message::Callback::Status WDSModemEndPoint::getPduSessionParamLookup(uint16_t txId, ApnTypes_t apnType,
                                                                     std::optional<TrafficDescriptor_t> td,
                                                                     bool matchAllRuleAllowed)
{
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[WDSModemEndPoint]: getPduSessionParamLookup() txId="+std::to_string(txId));
    wds_pdu_session_param_lookup_req_msg_v01 req;
    wds_pdu_session_param_lookup_resp_msg_v01 resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.tx_id = txId;
    req.tx_td_info.total_td_frag_num = 1;
    req.tx_td_info.td_frag_index = 0;
    req.apn_type_mask_valid = 1;
    req.apn_type_mask = WDSModemEndPointModule::convertToApnTypeMask((int32_t)apnType);
    req.allow_match_all_valid = 1;
    req.allow_match_all = matchAllRuleAllowed;

    if(td.has_value()) {
      if (td.value().dnn.has_value()) {
        Log::getInstance().d("dnn="+td.value().dnn.value());
        req.dnn_valid = 1;
        memcpy(req.dnn, td.value().dnn.value().c_str(), td.value().dnn.value().length());
      }
      else {
        Log::getInstance().d("dnn invalid");
      }
      if (td.value().osAppId.has_value() && td.value().osAppId.value().size() > 17) {
        req.app_id_list_valid = 1;
        req.app_id_list_len = 1;
        // 16 bytes for OSId + 1 byte for OSAppId length + up to 255 bytes for OSAppId
        req.app_id_list[0].os_id_valid = 1;
        std::copy(td.value().osAppId.value().begin(),
                  td.value().osAppId.value().begin()+16,
                  req.app_id_list[0].os_id);
        req.app_id_list[0].os_app_id_len = td.value().osAppId.value()[16];
        std::copy(td.value().osAppId.value().begin()+17,
                  td.value().osAppId.value().begin()+17+td.value().osAppId.value()[16],
                  req.app_id_list[0].os_app_id);
      }
      else {
        Log::getInstance().d("osAppId invalid");
      }
    }

    int rc = sendRawSync(QMI_WDS_PDU_SESSION_PARAM_LOOKUP_REQ_V01,
                         (void *)&req, sizeof(req),
                         (void *)&resp, sizeof(resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc == QMI_NO_ERR) && (resp.resp.result == QMI_RESULT_SUCCESS_V01 )) {
      Log::getInstance().d("[WDSModemEndPoint] QMI_WDS_PDU_SESSION_PARAM_LOOKUP_REQ_V01::Successfully sent");
      ret = Message::Callback::Status::SUCCESS;
    }
  }
  return ret;
}

void WDSModemEndPoint::getSlicingConfigRequest(std::shared_ptr<GetSlicingConfigRequestMessage> m)
{
  Log::getInstance().d("[WDSModemEndPoint]: handleGetSlicingConfigRequest");
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    auto msg = std::make_shared<GetSlicingConfigInternalMessage>(m);
    msg->broadcast();
  }
  else
  {
    rildata::GetSlicingConfigResp_t resp;
    resp.respErr = ResponseError_t::INTERNAL_ERROR;
    m->sendResponse(m, Message::Callback::Status::FAILURE, std::make_shared<rildata::GetSlicingConfigResp_t>(resp));
    Log::getInstance().d("[WDSModemEndPoint::getSlicingConfigrequest]"
                         " not operational");
  }
  //release wakeLock
  if(m->getAcknowlegeRequestCb() != nullptr) {
    auto cb = *(m->getAcknowlegeRequestCb().get());
    cb(m->getSerial());
  }
}
