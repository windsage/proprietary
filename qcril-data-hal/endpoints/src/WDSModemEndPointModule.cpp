/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include "WDSModemEndPointModule.h"
#include <modules/qmi/QmiIndMessage.h>
#include "ProfileHandler.h"
#include "request/GetPdnThrottleTimeMessage.h"
#include "UnSolMessages/SetLteAttachPdnListActionResultMessage.h"
#include "MessageCommon.h"
#include "UnSolMessages/KeepAliveIndMessage.h"
#include "UnSolMessages/SegmentTimeoutMessage.h"
#include "UnSolMessages/PduSessionParamLookupResultIndMessage.h"
#include "qcril_data.h"
#include "sync/ChangeStackIdSyncMessage.h"
#include "UnSolMessages/WdsThrottleInfoIndMessage.h"
#include "UnSolMessages/SlicingConfigChangedIndMessage.h"
#include "UnSolMessages/WdsTdInfoFragmentMessage.h"
#include "Encoder.h"

#ifdef SET_LOCAL_URSP_CONFIG
#include "request/SetLocalUrspConfigurationRequestMessage.h"
#endif

using namespace rildata;

DEFINE_MSG_ID_INFO(WDSModemEndPoint_QMI_IND)
DEFINE_MSG_ID_INFO(WDSModemEndPoint_ENDPOINT_STATUS_IND)

WDSModemEndPointModule::WDSModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner,
        REG_MSG(WDSModemEndPoint_QMI_IND),
            REG_MSG(WDSModemEndPoint_ENDPOINT_STATUS_IND))
{
  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  mTxId = 1;

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;

  mMessageHandler = {
      HANDLER(ChangeStackIdWdsSyncMessage,
              WDSModemEndPointModule::handleChangeStackIdWdsSyncMessage),
      HANDLER(GetPdnThrottleTimeMessage,
              WDSModemEndPointModule::handleGetPdnThrottleTime),
      HANDLER(GetSlicingConfigInternalMessage,
              WDSModemEndPointModule::handleGetSlicingConfigInternalMessage),
      #ifdef SET_LOCAL_URSP_CONFIG
      HANDLER(SetLocalUrspConfigurationRequestMessage,
              WDSModemEndPointModule::handleSetLocalUrspConfigurationRequestMessage),
      #endif
      HANDLER_MULTI(QmiIndMessage, WDSModemEndPoint_QMI_IND, WDSModemEndPointModule::handleWdsQmiIndMessage),
  };
  std::unordered_map<unsigned int, SegmentTracker::transactionCompleteCb_t> segmentHandlers = {
    {QMI_WDS_PDN_THROTTLE_INFO_V2_RESULT_IND_V01, std::bind(&WDSModemEndPointModule::processGetPdnThrottleInfoResultComplete, this, _1, _2, _3)},
    {QMI_WDS_PDN_THROTTLE_INFO_V2_IND_V01 , std::bind(&WDSModemEndPointModule::processPdnThrottleInfoIndComplete, this, _1, _2, _3)},
    {QMI_WDS_TD_INFO_IND_V01, std::bind(&WDSModemEndPointModule::processTdInfoIndComplete, this, _1, _2, _3)},
  };
  segmentTracker.setHandlers(segmentHandlers);
  segmentTracker.setEndPointName(mName);
}

WDSModemEndPointModule::~WDSModemEndPointModule()
{
  mLooper = nullptr;
}

void WDSModemEndPointModule::init()
{
  ModemEndPointModule::init();
  mStackId[0] = 0;
}

qmi_idl_service_object_type WDSModemEndPointModule::getServiceObject()
{
  return wds_get_service_object_v01();
}

WdsThrottleInfo convertWdsThrottleInfo(const wds_pdn_throttle_info_type_v01& info) {
  std::string apn = std::string(info.apn_string);
  transform(apn.begin(), apn.end(), apn.begin(), ::tolower);
  WdsThrottleInfo throttleInfo { apn };
  if (info.is_ipv4_throttled) {
    throttleInfo.setV4ThrottleTime(info.remaining_ipv4_throttled_time);
  }
  if (info.is_ipv6_throttled) {
    throttleInfo.setV6ThrottleTime(info.remaining_ipv6_throttled_time);
  }
  return throttleInfo;
}

WdsThrottleInfo convertWdsThrottleInfo(const wds_pdn_throttle_info_per_apn_type_v01& info) {
  std::string apn = std::string(info.apn_string);
  transform(apn.begin(), apn.end(), apn.begin(), ::tolower);
  WdsThrottleInfo throttleInfo { apn };
  if (info.is_ipv4_throttled) {
    throttleInfo.setV4ThrottleTime(info.remaining_ipv4_throttled_time);
  }
  if (info.is_ipv6_throttled) {
    throttleInfo.setV6ThrottleTime(info.remaining_ipv6_throttled_time);
  }
  return throttleInfo;
}

bool WDSModemEndPointModule::handleQmiBinding
(
  qcril_instance_id_e_type instanceId, int8_t stackId
)
{
#ifdef RIL_FOR_MODEM_LITE
  (void) instanceId;
  (void) stackId;
  return true;
#else
  Log::getInstance().d("[WDSModemEndPointModule] handleQmiBinding");
  if (stackId < 0)
    return false;

  wds_bind_subscription_req_msg_v01 bind_request;
  wds_bind_subscription_resp_msg_v01 bind_resp;
  memset(&bind_request, 0, sizeof(bind_request));
  memset(&bind_resp, 0, sizeof(bind_resp));

  if (stackId == 0)
  {
    global_subs_id = WDS_PRIMARY_SUBS_V01;
  } else if (stackId == 1)
  {
    global_subs_id = WDS_SECONDARY_SUBS_V01;
  } else if (stackId == 2)
  {
    global_subs_id = WDS_TERTIARY_SUBS_V01;
  } else
    return false;
  bind_request.subscription = global_subs_id;
  int ntries = 0;
  do
  {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_WDS_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&bind_request, sizeof(bind_request),
         (void*)&bind_resp, sizeof(bind_resp),
         ModemEndPointModule::DEFAULT_SYNC_TIMEOUT);

    if (QMI_NO_ERR == res && bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01)
    {
      Log::getInstance().d(
        "[WDSModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
#endif
}

void WDSModemEndPointModule::handleChangeStackIdWdsSyncMessage(std::shared_ptr<Message> msg)
{
    Log::getInstance().d("[WDSModemEndPointModule]: Handling msg = " + msg->dump());
    auto m = std::static_pointer_cast<ChangeStackIdWdsSyncMessage>(msg);
    if (m == nullptr) {
        return;
    }
    bool successBit = WDSModemEndPointModule::handleQmiBinding(
            static_cast<qcril_instance_id_e_type>(m->getInstanceId()), m->getStackId());
    if (successBit) {
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
        Log::getInstance().d("[WDSModemEndPointModule]: Stack Id Changed Successfully");
        return;
    }
    Log::getInstance().d("[WDSModemEndPointModule]: Stack Id Changed Failed");
    m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
}

void WDSModemEndPointModule::handleWdsQmiIndMessage
(
  std::shared_ptr<Message> msg
)
{
  auto shared_indMsg(
        std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[WDSModemEndPointModule]: Handling msg = " + msg->dump());

  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr)
  {
    WdsUnsolicitedIndHdlr(indData->msgId, indData->indData, indData->indSize);
  }
}

void WDSModemEndPointModule::WdsUnsolicitedIndHdlr
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
)
{
  (void)decoded_payload_len;
  Log::getInstance().d("[WDSModemEndPointModule]: WdsUnsolicitedIndHdlr ENTRY");
  switch(msg_id)
  {
    case QMI_WDS_SET_LTE_ATTACH_PDN_LIST_ACTION_RESULT_IND_V01:
      if (decoded_payload != NULL)
      {
        wds_set_lte_attach_pdn_list_action_result_ind_msg_v01 *res
        = (wds_set_lte_attach_pdn_list_action_result_ind_msg_v01*) (decoded_payload);

        if(res != NULL)
        {
          Log::getInstance().d("[WDSModemEndPointModule]:"
            "WdsUnsolicitedIndHdlr: result = " + std::to_string(res->result));
          auto msg = std::make_shared<rildata::SetLteAttachPdnListActionResultMessage>(*res);
          msg->broadcast();
        }
      }
      break;

    case QMI_WDS_MODEM_ASSISTED_KA_STATUS_IND_V01:
       if(decoded_payload != nullptr)
       {
          wds_modem_assisted_ka_status_ind_msg_v01 *ma_ka_info = (wds_modem_assisted_ka_status_ind_msg_v01*) (decoded_payload);
          if(ma_ka_info != nullptr) {
            rildata::KeepAliveInfo_t k;
            auto tcp_kaInfo = std::make_shared<rildata::TcpKeepAliveIndInfo_t>();
            bool isTcpKeepAlive = false;
            Tcp_keepAlive_Type_t tcpKeepAliveType = Tcp_keepAlive_Type_t::KA_HANDLE_STATUS;
            k.status = (rildata::KeepAliveResult_t)ma_ka_info->status;
            k.handle = (uint32_t)ma_ka_info->keep_alive_handle;
            if(ma_ka_info->keep_alive_type_valid == 1) {
              if(ma_ka_info->keep_alive_type == WDS_KEEPALIVE_TYPE_TCP_V01) {
                tcp_kaInfo->type = TcpKeepaliveType_t::TCP_KEEP_ALIVE;
                isTcpKeepAlive = true;
              }
              if(ma_ka_info->keep_alive_type == WDS_KEEPALIVE_TYPE_TCP_HEARTBEAT_V01) {
                tcp_kaInfo->type = TcpKeepaliveType_t::TCP_HEART_BEAT;
                isTcpKeepAlive = true;
              }
            }
            if(ma_ka_info->config_handle_valid == 1) {
              tcpKeepAliveType = Tcp_keepAlive_Type_t::CONFIG_HANDLE_STATUS;
              tcp_kaInfo->config_handle = ma_ka_info->config_handle;
            }
            if(ma_ka_info->tcp_status_valid == 1 &&
               ma_ka_info->status == WDS_KEEP_ALIVE_TCP_STATUS_V01) {
              tcpKeepAliveType = Tcp_keepAlive_Type_t::TCP_STATUS;
              tcp_kaInfo->tcp_status.uplink_seqnum = ma_ka_info->tcp_status.send_next;
              tcp_kaInfo->tcp_status.uplink_window = ma_ka_info->tcp_status.send_window;
              tcp_kaInfo->tcp_status.downlink_seqnum = ma_ka_info->tcp_status.recv_next;
              tcp_kaInfo->tcp_status.downlink_window = ma_ka_info->tcp_status.recv_window;
            }
            if(isTcpKeepAlive) {
              if(k.status == KeepAliveResult_t::KEEP_ALIVE_RESULT_OK) {
                tcp_kaInfo->status = TcpKaResponseError_t::SUCCESS;
              }else if(k.status == KeepAliveResult_t::KEEP_ALIVE_RESULT_ERROR) {
                tcp_kaInfo->status = TcpKaResponseError_t::ERROR;
              }else if(k.status == KeepAliveResult_t::KEEP_ALIVE_NETWORK_DOWN_ERROR) {
                tcp_kaInfo->status = TcpKaResponseError_t::NETWORK_DOWN;
              } else if(k.status == KeepAliveResult_t::KEEP_ALIVE_TCP_STATUS) {
                tcp_kaInfo->status = TcpKaResponseError_t::TCP_STATUS;
              }
              tcp_kaInfo->ka_handle = k.handle;
              tcp_kaInfo->ind_type = tcpKeepAliveType;
              auto msg = std::make_shared<rildata::KeepAliveIndMessage>(tcp_kaInfo);
              msg->broadcast();
            } else {
              auto msg = std::make_shared<rildata::KeepAliveIndMessage>(k);
              msg->broadcast();
            }
          }
       }
       break;

    case QMI_WDS_LTE_ATTACH_FAILURE_INFO_IND_V01:
       if((decoded_payload != nullptr) &&
          (static_cast<WDSModemEndPoint *>(&mOwner)->getReportingStatus()) &&
          (static_cast<WDSModemEndPoint *>(&mOwner)->getDataRegistrationState() == false))
       {
          wds_lte_attach_failure_info_ind_msg_v01 *rejInd = (wds_lte_attach_failure_info_ind_msg_v01*) (decoded_payload);
          if(rejInd != nullptr && rejInd->lte_attach_reject_reason.reject_reason_type == WDS_VCER_TYPE_3GPP_SPEC_DEFINED_V01) {
            int32_t cause = (int32_t)rejInd->lte_attach_reject_reason.reject_reason;

            PLMN_t plmn = convertPlmn(rejInd->plmn_id);
            PLMN_t primaryPlmn = {0,0,"",""};
            if(rejInd->primary_plmn_id_valid) {
              primaryPlmn = convertPlmn(rejInd->primary_plmn_id);
            }
            auto msg = std::make_shared<rildata::DataRegistrationFailureCauseMessage>(plmn, primaryPlmn, cause, true);
            msg->broadcast();
          }
       }
       break;

    case QMI_WDS_PDN_THROTTLE_INFO_V2_RESULT_IND_V01:
       if(decoded_payload != nullptr)
       {
          wds_pdn_throttle_info_v2_result_ind_msg_v01 *ind_msg = (wds_pdn_throttle_info_v2_result_ind_msg_v01*) (decoded_payload);
          if(ind_msg != nullptr && ind_msg->throttle_info_valid) {
            std::list<WdsThrottleInfo> throttleInfo;
            for (uint32_t i = 0; i < ind_msg->throttle_info_len; i++) {
              WdsThrottleInfo info = convertWdsThrottleInfo(ind_msg->throttle_info[i]);
              throttleInfo.push_back(info);
            }
            auto fragmentMsg = std::make_shared<WdsThrottleInfoFragmentMessage>(throttleInfo);
            segmentTracker.addSegment(msg_id, ind_msg->tx_info.transaction_id,
                                      ind_msg->tx_info.segment_index, ind_msg->tx_info.num_segments, fragmentMsg);
          }
       }
       break;

    case QMI_WDS_PDN_THROTTLE_INFO_V2_IND_V01:
       Log::getInstance().d("QMI_WDS_PDN_THROTTLE_INFO_V2_IND_V01");
       if(decoded_payload != nullptr)
       {
          wds_pdn_throttle_info_v2_ind_msg_v01 *ind_msg = (wds_pdn_throttle_info_v2_ind_msg_v01*) (decoded_payload);
          if(ind_msg != nullptr) {
            std::list<WdsThrottleInfo> throttleInfo;
            if (ind_msg->throttle_info_valid) {
              for (uint32_t i = 0; i < ind_msg->throttle_info_len; i++) {
                WdsThrottleInfo info = convertWdsThrottleInfo(ind_msg->throttle_info[i]);
                throttleInfo.push_back(info);
              }
            }
            auto fragmentMsg = std::make_shared<WdsThrottleInfoFragmentMessage>(throttleInfo);
            segmentTracker.addSegment(msg_id, ind_msg->tx_info.transaction_id,
                                      ind_msg->tx_info.segment_index, ind_msg->tx_info.num_segments, fragmentMsg);
          }
       }
       break;

    case QMI_WDS_PDN_THROTTLE_INFO_IND_V01:
       Log::getInstance().d("QMI_WDS_PDN_THROTTLE_INFO_IND_V01");
       if(decoded_payload != nullptr)
       {
          wds_pdn_throttle_info_ind_msg_v01 *ind_msg = (wds_pdn_throttle_info_ind_msg_v01*) (decoded_payload);
          if(ind_msg != nullptr) {
            std::list<WdsThrottleInfo> throttleInfo;
            if (ind_msg->throttle_info_valid) {
              for (uint32_t i = 0; i < ind_msg->throttle_info_len; i++) {
                WdsThrottleInfo info = convertWdsThrottleInfo(ind_msg->throttle_info[i]);
                throttleInfo.push_back(info);
              }
            }
            auto broadcastMsg = std::make_shared<WdsThrottleInfoIndMessage>(throttleInfo);
            broadcastMsg->broadcast();
            break;
          }
       }
       break;

    case QMI_WDS_PDU_SESSION_PARAM_LOOKUP_RESULT_IND_V01:
        if(decoded_payload != nullptr)
        {
          wds_pdu_session_param_lookup_result_ind_msg_v01 *ind = (wds_pdu_session_param_lookup_result_ind_msg_v01*) (decoded_payload);
          if(ind != nullptr) {
            PduSessionParamLookupResult_t result;
            result.txId = ind->tx_id;
            if(ind->status_valid) {
              result.status = (PduSessionParamLookupResultStatus_t)ind->status;
            }
            if(ind->dnn_slice_valid) {
              string dnnString(ind->dnn_slice);
              result.dnnName = dnnString;
            }
            auto msg = std::make_shared<rildata::PduSessionParamLookupResultIndMessage>(result);
            msg->broadcast();
          }
        }
        break;

    case QMI_WDS_TD_INFO_IND_V01:
      if(decoded_payload != nullptr) {
        wds_td_info_ind_msg_v01 *ind = (wds_td_info_ind_msg_v01*) (decoded_payload);
        if(ind != nullptr) {
            if (ind->tx_info.num_segments == 0) {
              Log::getInstance().d("[WDSModemEndPointModule]: QMI_WDS_TD_INFO_IND_V01 received empty segements");
              //sending td info with resp
              if(getSlicingConfigTxId.find(ind->tx_info.transaction_id) != getSlicingConfigTxId.end())
              {
                auto msg = getSlicingConfigTxId.at(ind->tx_info.transaction_id);
                auto m = static_pointer_cast<GetSlicingConfigRequestMessage>(msg);
                if(m != nullptr)
                {
                  GetSlicingConfigResp_t res = {};
                  auto resp = std::make_shared<GetSlicingConfigResp_t>(res);
                  m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
                  getSlicingConfigTxId.erase(ind->tx_info.transaction_id);
                }
              }
              else {
                auto msg = std::make_shared<rildata::SlicingConfigChangedIndMessage>();
                msg->broadcast();
              }
              return;
            }
            TrafficDescriptorFragment_t frag;
            frag.tdIdx = ind->tx_info.segment_index;

            TrafficDescriptorInfo_t tdInfo;
            if (ind->dnn_list_valid) {
              std::vector<std::string> dnnlist;
              for(int i=0;i<ind->dnn_list_len;i++) {
                string dnn(ind->dnn_list[i].apn_name);
                dnnlist.push_back(dnn);
              }
              tdInfo.dnnList = dnnlist;
            }
            if (ind->app_id_list_valid) {
              std::vector<appIdList_t> applist;
              for(int i=0;i<ind->app_id_list_len;i++) {
                appIdList_t appId;

                //osAppId is formed of osId + appId length + appId

                //If osId is present, add it
                if(ind->app_id_list[i].os_id_valid) {
                  for(int j=0 ; j<QMI_WDS_OS_ID_MAX_V01 ; j++) {
                    appId.osAppId.push_back(ind->app_id_list[i].os_id[j]);
                  }
                }

                //Single byte for appId length
                appId.osAppId.push_back((uint8_t)ind->app_id_list[i].os_app_id_len);
                //Add the appId
                for(int j=0 ; j<ind->app_id_list[i].os_app_id_len ; j++) {
                  appId.osAppId.push_back(ind->app_id_list[i].os_app_id[j]);
                }
                applist.push_back(appId);
              }
              tdInfo.appIdList = applist;
            }
            if(tdInfo.dnnList.size() > 0 || tdInfo.appIdList.size() > 0) {
              frag.tdInfo = tdInfo;
            }
            auto fragmentMsg = std::make_shared<WdsTdInfoFragmentMessage>(frag);
            Log::getInstance().d(fragmentMsg->dump());
            segmentTracker.addFragment(msg_id, ind->tx_info.transaction_id,
                                      ind->tx_info.segment_index, ind->tx_info.num_segments,
                                      ind->tx_td_info.td_frag_index, ind->tx_td_info.total_td_frag_num,
                                      fragmentMsg);
          }
        }
        break;

    default:
      Log::getInstance().d("Ignoring wds ind event "+ std::to_string(msg_id));
      break;
  }
}

PLMN_t WDSModemEndPointModule::convertPlmn(uint8_t plmn_id[])
{
  PLMN_t plmn = {0,0,"",""};
  Log::getInstance().d("convert plmn = "+ std::to_string(plmn_id[0])+", "+std::to_string(plmn_id[1])+", "+std::to_string(plmn_id[2]));

  // plmn_id[0] is |      MCC Digit 2      |      MCC Digit 1      |
  // plmn_id[1] is |      MNC Digit 3      |      MCC Digit 3      |
  // plmn_id[2] is |      MNC Digit 2      |      MNC Digit 1      |
  plmn.mcc = ((plmn_id[0] & 0xF0) >> 4) * 10 + (plmn_id[0] & 0x0F) * 100;
  plmn.mcc += (plmn_id[1] & 0x0F);
  plmn.mnc = ((plmn_id[2] & 0xF0) >> 4) + (plmn_id[2] & 0x0F) * 10;
  int mncDigit3 = (plmn_id[1] & 0xF0) >> 4;
  if(mncDigit3 != 0x0F) {
    plmn.mnc = (plmn.mnc * 10) + mncDigit3;
  }

  plmn.mMcc += std::to_string(plmn_id[0] & 0x0F);
  plmn.mMcc += std::to_string((plmn_id[0] & 0xF0) >> 4);
  plmn.mMcc += std::to_string(plmn_id[1] & 0x0F);

  plmn.mMnc += std::to_string(plmn_id[2] & 0x0F);
  plmn.mMnc += std::to_string((plmn_id[2] & 0xF0) >> 4);
  if(mncDigit3 != 0x0F) {
    plmn.mMnc += std::to_string(mncDigit3);
  }

  Log::getInstance().d("converted plmn : mcc = "+ std::to_string(plmn.mcc)+", mnc = "+std::to_string(plmn.mnc));
  Log::getInstance().d("converted plmn : mMcc = "+ plmn.mMcc+", mMnc = "+plmn.mMnc);
  return plmn;
}

/**
 * @brief Returns boolean to indicate if passed apn type is Internet
 * capable
 *
 * @return boolean
 *
 **/
bool WDSModemEndPointModule::isDefaultProfile
(
  int32_t supportedApnTypesBitmap
)
{
  if ( supportedApnTypesBitmap & (int32_t)ApnTypes_t::DEFAULT) {
    return TRUE;
  }
  return FALSE;
} /* WDSModemEndPointModule::isDefaultProfile() */

/**
 * @brief Converts ApnTypes_t to WDS Apn type mask
 *
 * @return ApnTypes_t
 *
 **/
wds_apn_type_mask_v01 WDSModemEndPointModule::convertToApnTypeMask
(
  int32_t supportedApnTypesBitmap
)
{
  wds_apn_type_mask_v01 apnTypeMask = WDS_APN_TYPE_MASK_UNSPECIFIED_V01;

  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::DEFAULT)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_DEFAULT_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::IMS)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_IMS_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::MMS)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_MMS_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::DUN)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_DUN_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::SUPL)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_SUPL_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::HIPRI)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_HIPRI_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::FOTA)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_FOTA_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::CBS)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_CBS_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::IA)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_IA_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::EMERGENCY)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_EMERGENCY_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::XCAP)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_UT_V01;
  }
  if(supportedApnTypesBitmap & (int32_t)ApnTypes_t::MCX)
  {
    apnTypeMask |= WDS_APN_TYPE_MASK_MCX_V01;
  }
  return apnTypeMask;
}

/**
 * @brief Constructs TDs from fragments of ROUTE_CHANGE or
 *        TD_INFO indication
 *
 * @return Vector of TrafficDescriptor_t
 *
 **/

vector<TrafficDescriptor_t> WDSModemEndPointModule::constructTDsFromFragments(std::vector<std::shared_ptr<Message>> fragments) {

  vector<TrafficDescriptor_t> result;
  //Map tracking TDs by segment
  std::map<uint16_t,TrafficDescriptorInfo_t> tdsPerSegMap;

  //This will require 2 passes.
  //In the first pass, go through all the fragments and accumulate all the
  //dnns and appIds per segment
  //In the second pass, construct the TDs from dnns and appIds per segment

  //1st Pass
  for (auto f : fragments) {
    auto m = std::static_pointer_cast<WdsTdInfoFragmentMessage>(f);
    if (m != nullptr) {
      TrafficDescriptorFragment_t frag = m->getParams();
      if(frag.tdInfo.has_value()) {
        auto it = tdsPerSegMap.find(frag.tdIdx);
        if (it != tdsPerSegMap.end())
        {
          for (auto dnn : frag.tdInfo.value().dnnList) {
            it->second.dnnList.push_back(dnn);
          }
          for (auto appid : frag.tdInfo.value().appIdList) {
            it->second.appIdList.push_back(appid);
          }
        }
        else {
          //Insert new entry in the map
          tdsPerSegMap[frag.tdIdx] = frag.tdInfo.value();
        }
      }
    }
  }

  //2nd Pass
  for (auto tdsPerSeg : tdsPerSegMap)
  {
    if(tdsPerSeg.second.dnnList.size() > 0)
    {
      for(auto dnn : tdsPerSeg.second.dnnList) {
        TrafficDescriptor_t td = {};
        td.dnn = dnn;
        if(tdsPerSeg.second.appIdList.size() == 0) {
          //No app ids, just add dnn
          Log::getInstance().d("Adding td with value : " + td.dump());
          result.push_back(td);
        } else {
          //combine dnn with app id
          for(auto appidlist : tdsPerSeg.second.appIdList) {
            td.osAppId = appidlist.osAppId;
            Log::getInstance().d("Adding td with value : " + td.dump());
            result.push_back(td);
          }
        }
      }
    } else {
      //empty dnn list, just add app ids
      TrafficDescriptor_t td = {};
      for(auto appidlist : tdsPerSeg.second.appIdList) {
        td.osAppId = appidlist.osAppId;
        Log::getInstance().d("Adding td with value : " + td.dump());
        result.push_back(td);
      }
    }
  }
  return result;
}

int64_t WDSModemEndPointModule::getPdnThrottleTime(std::list<WdsThrottleInfo> throttleInfo, std::shared_ptr<Message> m) {
  auto msg= std::static_pointer_cast<GetPdnThrottleTimeMessage>(m);

  if (msg != nullptr) {
    Log::getInstance().d("[WDSModemEndPointModule] getPdnThrottleTime " + msg->dump());
    for (auto info : throttleInfo) {
      Log::getInstance().d("[WDSModemEndPointModule] : compare with " + info.dump());
      if (msg->getApn() == info.getApn()) {
        int64_t rilThrottleTime = -1;
        if (msg->getIpType() == "IP" && info.hasV4ThrottleTime()) {
          rilThrottleTime = convertToRilThrottleTime(info.getV4ThrottleTime());
        } else if (msg->getIpType() == "IPV6" && info.hasV6ThrottleTime()) {
          rilThrottleTime = convertToRilThrottleTime(info.getV6ThrottleTime());
        } else if (msg->getIpType() == "IPV4V6" && (info.hasV4ThrottleTime() || info.hasV6ThrottleTime())) {
          uint32_t modemThrottleTime = 0xffffffff;
          if (info.hasV4ThrottleTime()) {
            modemThrottleTime = info.getV4ThrottleTime();
          }
          if (info.hasV6ThrottleTime()) {
            modemThrottleTime = std::min(modemThrottleTime, info.getV6ThrottleTime());
          }
          rilThrottleTime = convertToRilThrottleTime(modemThrottleTime);
        }
        return rilThrottleTime;
      }
    }
  }
  return -1;
}

void WDSModemEndPointModule::handleGetPdnThrottleTime(std::shared_ptr<Message> m) {
  Log::getInstance().d("[WDSModemEndPointModule]: handleGetPdnThrottleTime ENTRY");
  auto msg= std::static_pointer_cast<GetPdnThrottleTimeMessage>(m);
  if (msg != nullptr) {
    wds_get_pdn_throttle_info_req_msg_v01  req_msg;
    wds_get_pdn_throttle_info_resp_msg_v01  resp_msg;

    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    int rc = qmi_client_send_msg_sync( mQmiSvcClient,
                                QMI_WDS_GET_PDN_THROTTLE_INFO_REQ_V01,
                                &req_msg,
                                sizeof(req_msg),
                                &resp_msg,
                                sizeof(resp_msg),
                                QCRIL_DATA_QMI_TIMEOUT);

    if (QMI_NO_ERR != rc || QMI_NO_ERR != resp_msg.resp.result) {
      Log::getInstance().d("failed to send wds_get_pdn_throttle_info_req_msg_v01");
      msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
      return;
    }

    if (static_cast<WDSModemEndPoint *>(&mOwner)->getPdnThrottleV2()) {
      if (resp_msg.tx_id_valid) {
        SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_WDS_PDN_THROTTLE_INFO_V2_RESULT_IND_V01, resp_msg.tx_id);
        Log::getInstance().d("[WDSModemEndPointModule::handleGetPdnThrottleTime]"
                            " Begin transaction tx=" + key);
        pendingSegmentRequests[key] = msg;
        // expect an indication later to respond to the message
        segmentTracker.startTransactionTimer(QMI_WDS_PDN_THROTTLE_INFO_V2_RESULT_IND_V01, resp_msg.tx_id);
        return;
      }
    } else {
      std::list<WdsThrottleInfo> pdnThrottleInfo;
      if (resp_msg.throttle_info_valid) {
        for (uint32_t i = 0; i < resp_msg.throttle_info_len; i++) {
          pdnThrottleInfo.push_back(convertWdsThrottleInfo(resp_msg.throttle_info[i]));
        }
        std::shared_ptr<int64_t> throttleTime = std::make_shared<int64_t>(getPdnThrottleTime(pdnThrottleInfo, m));
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, throttleTime);
        return;
      }
    }
  }
  else {
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}

void WDSModemEndPointModule::processGetPdnThrottleInfoResultComplete(SegmentationStatus_t status,
  uint16_t txId, std::vector<std::shared_ptr<Message>> segments) {
  SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_WDS_PDN_THROTTLE_INFO_V2_RESULT_IND_V01, txId);
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      std::list<WdsThrottleInfo> throttleInfo;
      for (auto msg : segments) {
        auto m = std::static_pointer_cast<WdsThrottleInfoFragmentMessage>(msg);
        if (m != nullptr) {
          std::list<WdsThrottleInfo> segment = m->getParams();
          throttleInfo.insert(throttleInfo.end(), segment.begin(), segment.end());
        }
      }
      if (pendingSegmentRequests.find(key) != pendingSegmentRequests.end()) {
        // if pending request message exists, respond to it
        auto reqMsg = static_pointer_cast<GetPdnThrottleTimeMessage>(pendingSegmentRequests[key]);
        if (reqMsg != nullptr) {
          auto responsePtr = std::make_shared<int64_t>(getPdnThrottleTime(throttleInfo, reqMsg));
          reqMsg->sendResponse(reqMsg, Message::Callback::Status::SUCCESS, responsePtr);
        }
        pendingSegmentRequests.erase(key);
      } else {
        Log::getInstance().d("[WDSModemEndPointModule]: processGetPdnThrottleInfoResultComplete message already processed");
      }
      break;
    }
    default:
    {
      Log::getInstance().d("[WDSModemEndPointModule]: processGetPdnThrottleInfoResultComplete segmentation failure");
    }
  }
}

void WDSModemEndPointModule::processPdnThrottleInfoIndComplete(SegmentationStatus_t status,
  uint16_t txId, std::vector<std::shared_ptr<Message>> segments) {
  SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_WDS_PDN_THROTTLE_INFO_V2_IND_V01, txId);
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      std::list<WdsThrottleInfo> throttleInfo;
      for (auto msg : segments) {
        auto m = std::static_pointer_cast<WdsThrottleInfoFragmentMessage>(msg);
        if (m != nullptr) {
          std::list<WdsThrottleInfo> segment = m->getParams();
          throttleInfo.insert(throttleInfo.end(), segment.begin(), segment.end());
        }
      }

      auto broadcastMsg = std::make_shared<WdsThrottleInfoIndMessage>(throttleInfo);
      broadcastMsg->broadcast();
      break;
    }
    default:
    {
      Log::getInstance().d("[WDSModemEndPointModule]: processPdnThrottleInfoIndComplete segmentation failure");
    }
  }
}

void WDSModemEndPointModule::processTdInfoIndComplete(SegmentationStatus_t status,
  uint16_t txId, std::vector<std::shared_ptr<Message>> fragments) {
  SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_WDS_TD_INFO_IND_V01, txId);
  switch(status) {
    case SegmentationStatus_t::Success:
    {
      SlicingConfig_t slicingConfig;
      UrspRule_t urspRule = {};

      urspRule.trafficDescriptors = WDSModemEndPointModule::constructTDsFromFragments(fragments);
      slicingConfig.urspRules.push_back(urspRule);

      //sending td info with resp
      if(getSlicingConfigTxId.find(txId) != getSlicingConfigTxId.end())
      {
        auto msg = getSlicingConfigTxId.at(txId);
        auto m = static_pointer_cast<GetSlicingConfigRequestMessage>(msg);
        if(m != nullptr)
        {
          GetSlicingConfigResp_t res = {};
          res.slicingConfig = slicingConfig;
          auto resp = std::make_shared<GetSlicingConfigResp_t>(res);
          m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
          getSlicingConfigTxId.erase(txId);
        }
        break;
      }

      //sending td info with unsol
      auto msg = std::make_shared<rildata::SlicingConfigChangedIndMessage>();
      msg->setSliceConfig(slicingConfig);
      msg->broadcast();
      break;
    }
    default:
    {
      Log::getInstance().d("[WDSModemEndPointModule]: processTdInfoIndComplete segmentation failure");
    }
  }
}

void WDSModemEndPointModule::handleSegmentTimeout(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[WDSModemEndPointModule]: handleSegmentTimeout : Handling msg = " + m->dump());

  auto msg= std::static_pointer_cast<SegmentTimeoutMessage>(m);
  if(msg != nullptr && msg->getEndPointName() == mName)
  {
    segmentTracker.sendErrorResponse(msg->getStatus(), msg->getMessageId(), msg->getTransactionId());
  }
}

void WDSModemEndPointModule::handleGetSlicingConfigInternalMessage(std::shared_ptr<Message> m)
{
  auto msg = std::static_pointer_cast<GetSlicingConfigInternalMessage>(m);
  if(msg != nullptr)
  {
    Log::getInstance().d("[WDSModemEndPointModule]: handleGetSlicingConfigInternalMessage");
    auto slicingMessage = std::static_pointer_cast<GetSlicingConfigRequestMessage>(msg->getMessage());
    if(slicingMessage != nullptr)
    {
      wds_get_td_info_req_msg_v01 req;
      wds_get_td_info_resp_msg_v01 resp;
      memset(&req, 0, sizeof(req));
      memset(&resp, 0, sizeof(resp));

      int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                        QMI_WDS_GET_TD_INFO_REQ_V01,
                                        (void *)&req,
                                        sizeof(req),
                                        (void *)&resp,
                                        sizeof(resp),
                                        QCRIL_DATA_QMI_TIMEOUT);

      if ((rc != QMI_NO_ERR) || (resp.resp.result == QMI_RESULT_FAILURE_V01 )) {
        Log::getInstance().d("[WDSModemEndPoint] Failed to send QMI_WDS_GET_TD_INFO_REQ_V01");
        GetSlicingConfigResp_t res = {};
        res.respErr = ResponseError_t::INTERNAL_ERROR;
        auto resp = std::make_shared<GetSlicingConfigResp_t>(res);
        slicingMessage->sendResponse(slicingMessage, Message::Callback::Status::SUCCESS, resp);
      } else {
        Log::getInstance().d("[WDSModemEndPoint] QMI_WDS_GET_TD_INFO_REQ_V01::Successfully sent");
        if(resp.tx_id_valid)
        {
          Log::getInstance().d("[WDSModemEndPoint] received txId "+ std::to_string(resp.tx_id));
          getSlicingConfigTxId[resp.tx_id] = slicingMessage;
        }
        return;
      }
    }
  }
}

#ifdef SET_LOCAL_URSP_CONFIG
void WDSModemEndPointModule::handleSetLocalUrspConfigurationRequestMessage(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[WDSModemEndPointModule] handleSetLocalUrspConfigurationRequestMessage");

  auto msg= std::static_pointer_cast<SetLocalUrspConfigurationRequestMessage>(m);
  if (mQmiSvcClient != nullptr) {
    wds_set_ursp_local_policy_req_msg_v01 req;
    wds_set_ursp_local_policy_resp_msg_v01 resp;

    std::vector<UrspRule_t> urspRules = msg->getUrspRules();
    int rc;
    uint8_t idx=0;
    uint16_t txid = allocateTxId();
    for (auto it=urspRules.begin() ; it!=urspRules.end() ; ++it) {
      memset(&req, 0, sizeof(req));
      memset(&resp, 0, sizeof(resp));

      req.tx_info.transaction_id = txid;
      req.tx_info.num_segments = urspRules.size();
      req.tx_info.segment_index = idx++;
      req.rule_precedence = it->precedence;
      std::vector<uint8_t> encodedData = encodeUrspRule(*it);

      // TODO: fragmentation for over 2000 bytes data
      if (!encodedData.empty() && encodedData.size() < QMI_WDS_MAX_RULE_DATA_LEN_V01) {
        req.rule_data_valid = true;
        req.rule_data_len = encodedData.size();
        std::copy(encodedData.begin(), encodedData.end(), req.rule_data);
        rc = qmi_client_send_msg_sync (mQmiSvcClient,
                                  QMI_WDS_SET_URSP_LOCAL_POLICY_REQ_V01,
                                  &req, sizeof(req),
                                  (void*)&resp, sizeof(resp),
                                  QCRIL_DATA_QMI_TIMEOUT);
        if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01) {
          Log::getInstance().d("QMI_WDS_SET_URSP_LOCAL_POLICY_REQ_V01: failed with rc="+
                                std::to_string(rc) +", qmi_err=" + std::to_string(resp.resp.error)+
                                ", txid="+std::to_string(txid)+", segidx="+std::to_string(idx));
          msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
          return;
        }
      }
      else {
        Log::getInstance().d("[WDSModemEndPointModule] handleSetLocalUrspConfigurationRequestMessage failure="+
                             std::to_string(encodedData.size()));
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
        return;
      }
    }
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
  }
  else {
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}
#endif
