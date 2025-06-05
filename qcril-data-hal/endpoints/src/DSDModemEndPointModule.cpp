/******************************************************************************
#  Copyright (c) 2018-2024 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <arpa/inet.h>
#include <cstring>
#include <string.h>
#include "DSDModemEndPointModule.h"
#include "DSDModemEndPoint.h"
#include <modules/qmi/QmiIndMessage.h>
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "UnSolMessages/ProcessCurrentRoamingStatus.h"
#include "UnSolMessages/DsdSystemStatusFragmentMessage.h"
#include "UnSolMessages/DsdSystemStatusPerApnMessage.h"
#include "UnSolMessages/DataNrIconTypeIndMessage.h"
#include "UnSolMessages/PreferredApnUpdatedMessage.h"
#include "UnSolMessages/UiInfoFragmentMessage.h"
#include "request/GetDataNrIconTypeMessage.h"

#if (!defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE))
#include "request/SetVowifiConfigurationMessage.h"
#endif

#include "UnSolMessages/SegmentTimeoutMessage.h"
#include "UnSolMessages/SetApnPreferredSystemResultMessage.h"
#include "UnSolMessages/IntentToChangeApnPreferredSystemMessage.h"
#include "qcril_data.h"
#include "sync/ChangeStackIdSyncMessage.h"
#include "request/GetDsdSystemStatusMessage.h"
#include "request/SetRatPrefMessage.h"
#include "UnSolMessages/CurrentDDSIndMessage.h"
#include "UnSolMessages/CurrentRoamingStatusChangedMessage.h"
#include "UnSolMessages/DDSSwitchResultIndMessage.h"
#include "UnSolMessages/AutoDdsSwitchControlIndMessage.h"
#include "UnSolMessages/DataCapabilityChangeIndMessage.h"
#include "event/DdsSwitchRecommendInternalMessage.h"
#include "MessageCommon.h"
#include "DataConfig.h"
#define TAG "DSD EndpointM"

using namespace rildata;

DEFINE_MSG_ID_INFO(DSDModemEndPoint_ENDPOINT_STATUS_IND)
DEFINE_MSG_ID_INFO(DSDModemEndPoint_QMI_IND)

DSDModemEndPointModule::DSDModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner,
        REG_MSG(DSDModemEndPoint_QMI_IND),
            REG_MSG(DSDModemEndPoint_ENDPOINT_STATUS_IND))
{
  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  homeRatPref = RatPreference::Inactive;
  roamingRatPref = RatPreference::Inactive;
  ratPrefReceived = false;
  isRoaming = false;

  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  mMessageHandler = {
      HANDLER_MULTI(QmiIndMessage, DSDModemEndPoint_QMI_IND, DSDModemEndPointModule::handleQmiDsdIndMessage),
      HANDLER(GetDsdSystemStatusMessage, DSDModemEndPointModule::handleGetDsdSystemStatus),
      HANDLER(SetRatPrefMessage, DSDModemEndPointModule::handleSetRatPref),
      HANDLER(ProcessCurrentRoamingStatus, DSDModemEndPointModule::handleCurrentRoamingStatusMessage),
      HANDLER(GetDataNrIconTypeMessage, DSDModemEndPointModule::handleGetDataNrIconType),
      HANDLER(SegmentTimeoutMessage, DSDModemEndPointModule::handleSegmentTimeout),
      HANDLER(SetNrUCIconConfigureMessage, DSDModemEndPointModule::handleSetNrUCIconConfigure),
#if (!defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE))
      HANDLER(SetVowifiConfigurationMessage, DSDModemEndPointModule::handleSetVowifiConfiguration),
      HANDLER(ChangeStackIdDsdSyncMessage, DSDModemEndPointModule::handleChangeStackIdDsdSyncMessage),
#endif
  };

  std::unordered_map<unsigned int, SegmentTracker::transactionCompleteCb_t> segmentHandlers = {
    {QMI_DSD_SYSTEM_STATUS_V2_IND_V01, std::bind(&DSDModemEndPointModule::processDsdSystemStatusIndComplete, this, _1, _2, _3)},
    {QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01, std::bind(&DSDModemEndPointModule::processDsdSystemStatusResultComplete, this, _1, _2, _3)},
    {QMI_DSD_UI_INFO_IND_V01, std::bind(&DSDModemEndPointModule::processDsdUiInfoIndComplete, this, _1, _2, _3)},
    {QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01, std::bind(&DSDModemEndPointModule::processDsdUiInfoResultComplete, this, _1, _2, _3)},
  };
  segmentTracker.setHandlers(segmentHandlers);
  segmentTracker.setEndPointName(mName);
}

DSDModemEndPointModule::~DSDModemEndPointModule()
{
  mLooper = nullptr;
}

void DSDModemEndPointModule::init()
{
    ModemEndPointModule::init();
    mStackId[0] = 0;
}

qmi_idl_service_object_type DSDModemEndPointModule::getServiceObject()
{
  return dsd_get_service_object_v01();
}

bool DSDModemEndPointModule::handleQmiBinding
(
  qcril_instance_id_e_type instanceId, int8_t stackId)
{
  Log::getInstance().d(
        "[DSDModemEndPointModule] handleQmiBinding");

  (void)instanceId;
  if (stackId < 0)
    return false;

  dsd_bind_subscription_req_msg_v01 bind_request;
  dsd_bind_subscription_resp_msg_v01 bind_resp;
  memset(&bind_request, 0, sizeof(bind_request));
  memset(&bind_resp, 0, sizeof(bind_resp));

  if (stackId == 0) {
    bind_request.bind_subs = DSD_PRIMARY_SUBS_V01;
  } else if (stackId == 1) {
    bind_request.bind_subs = DSD_SECONDARY_SUBS_V01;
  } else if (stackId == 2) {
    bind_request.bind_subs = DSD_TERTIARY_SUBS_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_DSD_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&bind_request, sizeof(bind_request),
         (void*)&bind_resp, sizeof(bind_resp),
         ModemEndPointModule::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[DSDModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);
  return false;
}

void DSDModemEndPointModule::handleCurrentRoamingStatusMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<ProcessCurrentRoamingStatus>(msg);
  if(m != nullptr) {
    processRoamingStatusChangeInd(m->getData());
  }
}

void DSDModemEndPointModule::handleQmiDsdIndMessage(std::shared_ptr<Message> msg)
{
  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + msg->dump());

  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    indicationHandler(indData->msgId, indData->indData, indData->indSize);
  }
}

void DSDModemEndPointModule::indicationHandler
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
)
{
  (void)decoded_payload_len;
  Log::getInstance().d("[DSDModemEndPointModule]: indicationHandler ENTRY");

  if(decoded_payload != nullptr) {
    Log::getInstance().d("msg_id = "+ std::to_string(msg_id));
    switch(msg_id)
    {
      case QMI_DSD_SYSTEM_STATUS_IND_V01:
        processSystemStatusInd((dsd_system_status_ind_msg_v01 *)decoded_payload);
        break;
      case QMI_DSD_INTENT_TO_CHANGE_APN_PREF_SYS_IND_V01:
         processIntentToChangeApnPrefSysInd((dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_AP_ASST_APN_PREF_SYS_RESULT_IND_V01:
         processApAsstApnPrefSysResultInd((dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_SYSTEM_STATUS_V2_IND_V01:
         processSystemStatusInd((dsd_system_status_v2_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01:
         processSystemStatusInd((dsd_get_system_status_v2_result_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_UI_INFO_IND_V01:
         processUiInfoInd((dsd_ui_info_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01:
         processUiInfoInd((dsd_get_ui_info_v2_result_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_PREFERRED_APN_UPDATED_IND_V01:
         processPrefApnUpdatedInd((dsd_preferred_apn_updated_ind_msg_v01 *)decoded_payload);
         break;
      case QMI_DSD_TEMP_DDS_REC_CAPABILITY_IND_V01:
         processDdsRecommendationCapabilityInd((dsd_temp_dds_rec_capability_ind_v01 *)decoded_payload);
         break;
      case QMI_DSD_DDS_RECOMMENDATION_IND_V01:
         processDdsRecommendationInd((dsd_dds_recommendation_ind_v01 *)decoded_payload);
         break;
      case QMI_DSD_CIWLAN_CAPABILITY_IND_V01:
         processDsdCIWlanCapabilityInd((dsd_ciwlan_capability_ind_v01 *)decoded_payload);
         break;
      case QMI_DSD_SWITCH_DDS_IND_V01:
         processQmiDdsSwitchInd((dsd_switch_dds_ind_msg_v01*)decoded_payload);
         break;
      case QMI_DSD_CURRENT_DDS_IND_V01:
         processQmiCurrentDdsInd((dsd_current_dds_ind_msg_v01*)decoded_payload);
         break;
      case QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01:
         processRoamingStatusChangeInd((dsd_roaming_status_change_ind_msg_v01 *)decoded_payload);
         break;
      default:
         Log::getInstance().d("Unexpected dsd indication, ignoring = "+ std::to_string(msg_id));
         break;
    }
  }
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param ind_data [description]
 */
void DSDModemEndPointModule::processSystemStatusInd(dsd_system_status_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: process dsd_system_status_ind_msg_v01");

  if (ind_data != nullptr) {
    auto msg = std::make_shared<rildata::DsdSystemStatusMessage>(*ind_data);
    msg->broadcast();
  }

  // TODO: eveluate data registration state
}

void DSDModemEndPointModule::processQmiDdsSwitchInd(dsd_switch_dds_ind_msg_v01 *ind)
{
  if (ind != nullptr)
  {
    Log::getInstance().d("[DSDModemEndPointModule]:"
      "QMI_DSD_SWITCH_DDS_IND_V01: result = " + std::to_string(ind->dds_switch_result));
    auto msg = std::make_shared<DDSSwitchResultIndMessage>();

    if(ind->dds_switch_result == DSD_DDS_SWITCH_NOT_ALLOWED_V01)
      msg->setAllowed(false);
    else
      msg->setAllowed(true);

    msg->broadcast();
  }
}

void DSDModemEndPointModule::processQmiCurrentDdsInd(dsd_current_dds_ind_msg_v01 *ind)
{
  if (ind != nullptr)
  {
    Log::getInstance().d("[DSDModemEndPointModule]:"
          "QMI_DSD_CURRENT_DDS_IND_V01: dds=" + std::to_string(ind->dds));
    dsd_dds_switch_type switchType = static_cast<dsd_dds_switch_type>(ind->dds_switch_type);
    int sub = (ind->dds == DSD_PRIMARY_SUBS_V01)?0:1;
    auto msg = std::make_shared<CurrentDDSIndMessage>(sub, switchType);
    msg->broadcast();
  }
}

bool convertRoamingStatus(uint8_t status) {
  switch (status) {
    case 0:
      return false;
    case 1:
      return true;
    default:
      Log::getInstance().d("[DSDModemEndPointModule]: invalid roaming status " + std::to_string((int)status));
      return false;
  }
}

void DSDModemEndPointModule::processRoamingStatusChangeInd(dsd_roaming_status_change_ind_msg_v01 *ind_data)
{
  if (ind_data != nullptr && ind_data->roaming_status_valid) {
    Log::getInstance().d("[DSDModemEndPointModule]: process dsd_roaming_status_change_ind_msg_v01 ENTRY");
    isRoaming = convertRoamingStatus(ind_data->roaming_status);
    std::shared_ptr<rildata::CurrentRoamingStatusChangedMessage> msg =
        std::make_shared<rildata::CurrentRoamingStatusChangedMessage>(isRoaming);
    msg->broadcast();

    //Send rat pref again to modem based on current roaming status
    if (ratPrefReceived) {
      sendNotifyDataSettings(isRoaming?roamingRatPref:homeRatPref);
    }
  }
}

dsd_apn_avail_sys_info_type_v01 convertApnAvailSystoV1(const dsd_apn_system_status_info_type_v01& apn_avail_sys_v2) {
  dsd_apn_avail_sys_info_type_v01 apn_avail_sys_v1 = {
    .apn_avail_sys_len = apn_avail_sys_v2.apn_avail_sys_len
  };
  memcpy(&apn_avail_sys_v1.apn_name, &apn_avail_sys_v2.apn_name, sizeof(apn_avail_sys_v1.apn_name));
  for (int i = 0; i < apn_avail_sys_v2.apn_avail_sys_len; i++) {
    apn_avail_sys_v1.apn_avail_sys[i] = apn_avail_sys_v2.apn_avail_sys[i];
  }
  return apn_avail_sys_v1;
}

void DSDModemEndPointModule::processDsdSystemStatusIndComplete(SegmentationStatus_t status,
  uint16_t, std::vector<std::shared_ptr<Message>> segments) {
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      vector<dsd_apn_avail_sys_info_type_v01> apn_sys_status;
      dsd_system_status_ind_msg_v01 sys_status_v1 = { .avail_sys_valid = false };
      // consolidate the segments, and split it into two messages
      for (auto msg : segments) {
        auto m = std::static_pointer_cast<DsdSystemStatusFragmentMessage>(msg);
        if (m != nullptr) {
          dsd_system_status_v2_ind_msg_v01 indMsg = m->getParams();
          if (indMsg.apn_avail_sys_valid) {
            for (int i = 0; i < indMsg.apn_avail_sys_len; i++) {
              dsd_apn_avail_sys_info_type_v01 apn_avail_sys_v1 = convertApnAvailSystoV1(indMsg.apn_avail_sys[i]);
              apn_sys_status.push_back(apn_avail_sys_v1);
            }
          }
          if (!sys_status_v1.avail_sys_valid && indMsg.avail_sys_valid) {
            sys_status_v1.avail_sys_valid = true;
            sys_status_v1.avail_sys_len = indMsg.avail_sys_len;
            int source_size = sizeof(indMsg.avail_sys) / sizeof(dsd_system_status_info_type_v01);
            int dest_size = sizeof(sys_status_v1.avail_sys) / sizeof(dsd_system_status_info_type_v01);
            int arr_size = MIN(dest_size, source_size);
            for (int i = 0; i < arr_size; i++) {
                sys_status_v1.avail_sys[i] = indMsg.avail_sys[i];
            }
          }
          if(indMsg.null_bearer_reason_valid) {
            sys_status_v1.null_bearer_reason_valid = true;
            sys_status_v1.null_bearer_reason = indMsg.null_bearer_reason;
          }
        }
      }
      Log::getInstance().d("[DSDModemEndPointModule]: processDsdSystemStatusIndComplete broadcasting sys status v2");
      //Broadcast system status first before the per APN msg
      auto dsdSysStatus = std::make_shared<DsdSystemStatusMessage>(sys_status_v1);
      dsdSysStatus->broadcast();

      auto perApnInd = std::make_shared<DsdSystemStatusPerApnMessage>(apn_sys_status);
      perApnInd->broadcast();
      break;
    }
    default:
      break;
  }
}

void DSDModemEndPointModule::processDsdSystemStatusResultComplete(SegmentationStatus_t status,
  uint16_t txId, std::vector<std::shared_ptr<Message>> segments) {
  SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01, txId);
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      vector<dsd_apn_avail_sys_info_type_v01> apn_sys_status;
      dsd_system_status_ind_msg_v01 sys_status_v1 = { .avail_sys_valid = false };
      // consolidate the segments, and split it into two messages
      for (auto msg : segments) {
        auto m = std::static_pointer_cast<DsdSystemStatusFragmentMessage>(msg);
        if (m != nullptr) {
          dsd_system_status_v2_ind_msg_v01 indMsg = m->getParams();
          if (indMsg.apn_avail_sys_valid) {
            for (int i = 0; i < indMsg.apn_avail_sys_len; i++) {
              dsd_apn_avail_sys_info_type_v01 apn_avail_sys_v1 = convertApnAvailSystoV1(indMsg.apn_avail_sys[i]);
              apn_sys_status.push_back(apn_avail_sys_v1);
            }
          }
          if (!sys_status_v1.avail_sys_valid && indMsg.avail_sys_valid) {
            sys_status_v1.avail_sys_valid = true;
            sys_status_v1.avail_sys_len = indMsg.avail_sys_len;
            int source_size = sizeof(indMsg.avail_sys) / sizeof(dsd_system_status_info_type_v01);
            int dest_size = sizeof(sys_status_v1.avail_sys) / sizeof(dsd_system_status_info_type_v01);
            int arr_size = MIN(dest_size, source_size);
            for (int i = 0; i < arr_size; i++) {
                sys_status_v1.avail_sys[i] = indMsg.avail_sys[i];
            }
          }
          if (indMsg.null_bearer_reason_valid) {
            sys_status_v1.null_bearer_reason_valid = true;
            sys_status_v1.null_bearer_reason = indMsg.null_bearer_reason;
          }
        }
      }
      if (pendingSegmentRequests.find(key) != pendingSegmentRequests.end()) {
        // if pending request message exists, respond to it with system status
        DsdSystemStatusResult_t response = {
          .resp_ind = sys_status_v1,
          .apn_sys = apn_sys_status
        };
        auto reqMsg = static_pointer_cast<GetDsdSystemStatusMessage>(pendingSegmentRequests[key]);
        if (reqMsg != nullptr) {
          auto responsePtr = std::make_shared<rildata::DsdSystemStatusResult_t>(response);
          reqMsg->sendResponse(reqMsg, Message::Callback::Status::SUCCESS, responsePtr);
          pendingSegmentRequests.erase(key);
          break;
        } else {
          Log::getInstance().d("[DSDModemEndPointModule]: processDsdSystemStatusResultComplete reqMsg is null");
        }
      } else {
        //Broadcast system status first before the per APN msg
        auto dsdSysStatus = std::make_shared<DsdSystemStatusMessage>(sys_status_v1);
        dsdSysStatus->broadcast();

        auto perApnInd = std::make_shared<DsdSystemStatusPerApnMessage>(apn_sys_status);
        perApnInd->broadcast();
      }
      break;
    }
    default:
    {
      // if segmentation error happened, initiate a new request
      dsd_get_system_status_v2_req_msg_v01  sys_req_msg;
      dsd_get_system_status_v2_resp_msg_v01  sys_resp_msg;

      memset(&sys_req_msg, 0, sizeof(sys_req_msg));
      memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));

      /* Query the current system status from QMI-DSD service */

      int rc = qmi_client_send_msg_sync( mQmiSvcClient,
                                  QMI_DSD_GET_SYSTEM_STATUS_V2_REQ_V01,
                                  &sys_req_msg,
                                  sizeof(sys_req_msg),
                                  &sys_resp_msg,
                                  sizeof(sys_resp_msg),
                                  QCRIL_DATA_QMI_TIMEOUT);
      if (rc == QMI_NO_ERR) {
        Log::getInstance().d("[DSDModemEndPointModule] processDsdSystemStatusResultComplete "
                              "Failed to send QMI_DSD_GET_SYSTEM_STATUS_V2_REQ_V01 for retry");
        return;
      }
      if (pendingSegmentRequests.find(key) != pendingSegmentRequests.end()) {
        // if pending request message exists, respond to it with failure
        DsdSystemStatusResult_t response = {};
        auto reqMsg = static_pointer_cast<GetDsdSystemStatusMessage>(pendingSegmentRequests[key]);
        if (reqMsg != nullptr) {
          auto responsePtr = std::make_shared<rildata::DsdSystemStatusResult_t>(response);
          reqMsg->sendResponse(reqMsg, Message::Callback::Status::FAILURE, responsePtr);
          pendingSegmentRequests.erase(key);
          break;
        } else {
          Log::getInstance().d("[DSDModemEndPointModule]: processDsdSystemStatusResultComplete reqMsg is null");
        }
      }
      // No indication to broadcast
    }
  }
}

std::shared_ptr<rildata::UiInfoFragmentMessage> consolidateUiInfo(std::vector<std::shared_ptr<Message>> segments) {
  for (auto msg : segments) {
    auto m = std::static_pointer_cast<rildata::UiInfoFragmentMessage>(msg);
    if (m->hasIcon()) {
      return m;
    }
  }
  Log::getInstance().d("[DSDModemEndPointModule]: consolidateUiInfo icon not found");
  return nullptr;
}

void DSDModemEndPointModule::processDsdUiInfoIndComplete(SegmentationStatus_t status,
  uint16_t, std::vector<std::shared_ptr<Message>> segments) {
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      auto consolidatedMsg = consolidateUiInfo(segments);
      if (consolidatedMsg != nullptr && consolidatedMsg->hasIcon()) {
        auto broadcastMsg = std::make_shared<rildata::DataNrIconTypeIndMessage>(consolidatedMsg->getIcon());
        broadcastMsg->broadcast();
      }
      return;
    }
    default:
      Log::getInstance().d("[DSDModemEndPointModule]: processDsdUiInfoIndComplete Error occured during segmentation");
      return;
  }
}

rildata::NrIconEnum_t convertUiMask(const dsd_sys_ui_mask_v01& ui_mask) {
  if (0 != (QMI_DSD_UI_MASK_3GPP_5G_UWB_V01 & ui_mask)) {
    Log::getInstance().d("[DSDModemEndPointModule]: convertUiMask UWB");
    return rildata::NrIconEnum_t::UWB;
  } else if (0 != (QMI_DSD_UI_MASK_3GPP_5G_BASIC_V01 & ui_mask)) {
    Log::getInstance().d("[DSDModemEndPointModule]: convertUiMask BASIC");
    return rildata::NrIconEnum_t::BASIC;
  } else {
    Log::getInstance().d("[DSDModemEndPointModule]: convertUiMask NONE");
    return rildata::NrIconEnum_t::NONE;
  }
}

void DSDModemEndPointModule::processDsdUiInfoResultComplete(SegmentationStatus_t status,
  uint16_t txId, std::vector<std::shared_ptr<Message>> segments) {
  SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01, txId);
  switch (status) {
    case SegmentationStatus_t::Success:
    {
      if (!segments.empty()) {
        auto consolidatedMsg = consolidateUiInfo(segments);
        if (pendingSegmentRequests.find(key) != pendingSegmentRequests.end()) {
          // if pending request message exists, respond to it with icon type
          auto reqMsg = static_pointer_cast<GetDataNrIconTypeMessage>(pendingSegmentRequests[key]);
          if (reqMsg != nullptr) {
            std::shared_ptr<rildata::NrIconType_t> icon;
            if (consolidatedMsg != nullptr && consolidatedMsg->hasIcon()) {
              icon = std::make_shared<rildata::NrIconType_t>(consolidatedMsg->getIcon());
            } else {
              icon = std::make_shared<rildata::NrIconType_t>(NrIconEnum_t::NONE);
            }
            reqMsg->sendResponse(reqMsg, Message::Callback::Status::SUCCESS, icon);
            pendingSegmentRequests.erase(key);
            break;
          }
        } else {
          // if no pending request message exists, send indication with icon type
          if (consolidatedMsg != nullptr) {
            NrIconType_t icon{consolidatedMsg->getIcon()};
            auto broadcastMsg = std::make_shared<rildata::DataNrIconTypeIndMessage>(icon);
            broadcastMsg->broadcast();
          }
        }
      } else {
        Log::getInstance().d("[DSDModemEndPointModule]: processDsdUiInfoResultComplete segments is empty");
      }
      break;
    }
    default:
    {
      // if segmentation error happened, initiate a new request
      dsd_get_ui_info_req_msg_v01 qmiReq;
      dsd_get_ui_info_resp_msg_v01 qmiResp;
      memset(&qmiReq, 0, sizeof(qmiReq));
      memset(&qmiResp, 0, sizeof(qmiResp));

      int retVal = qmi_client_send_msg_sync(mQmiSvcClient,
                                QMI_DSD_GET_UI_INFO_REQ_V01,
                                (void *)&qmiReq, sizeof(qmiReq),
                                (void *)&qmiResp, sizeof(qmiResp),
                                QCRIL_DATA_QMI_TIMEOUT);
      if (retVal != QMI_NO_ERR) {
        Log::getInstance().d("[DSDModemEndPointModule] processDsdUiInfoResultComplete "
                              "Failed to send QMI_DSD_GET_UI_INFO_REQ_V01 for retry");
        break;
      }
      if (pendingSegmentRequests.find(key) != pendingSegmentRequests.end()) {
        // if pending request message exists, respond to it with icon type
        auto reqMsg = static_pointer_cast<GetDataNrIconTypeMessage>(pendingSegmentRequests[key]);
        if (reqMsg != nullptr) {
          std::shared_ptr<NrIconType_t> result;
          if (qmiResp.global_ui_info_valid) {
            NrIconEnum_t iconType = convertUiMask(qmiResp.global_ui_info.ui_mask);
            result = std::make_shared<NrIconType_t>(iconType);
          } else {
            result = std::make_shared<NrIconType_t>(NrIconEnum_t::NONE);
          }
          pendingSegmentRequests.erase(key);
          reqMsg->sendResponse(reqMsg, Message::Callback::Status::SUCCESS, result);
        }
      } else {
        if (qmiResp.global_ui_info_valid) {
          NrIconEnum_t iconType = convertUiMask(qmiResp.global_ui_info.ui_mask);
          NrIconType_t icon{iconType};
          auto broadcastMsg = std::make_shared<rildata::DataNrIconTypeIndMessage>(icon);
          broadcastMsg->broadcast();
        }
      }
    }
  }
}

void DSDModemEndPointModule::processUiInfoInd(dsd_ui_info_ind_msg_v01 *ind_data)
{
  if (ind_data != nullptr)
  {
    Log::getInstance().d("[DSDModemEndPointModule]: process dsd_ui_info_ind_msg_v01 ENTRY");
    if(ind_data->tx_info_valid) {
      std::shared_ptr<rildata::UiInfoFragmentMessage> msg =
          std::make_shared<rildata::UiInfoFragmentMessage>();
      if (ind_data->global_ui_info_valid) {
        msg->setIcon(convertUiMask(ind_data->global_ui_info.ui_mask));
      }
      segmentTracker.addSegment(QMI_DSD_UI_INFO_IND_V01, ind_data->tx_info.transaction_id,
                                ind_data->tx_info.segment_index, ind_data->tx_info.num_segments, msg);
    }
    else if (ind_data->global_ui_info_valid)
    {
      rildata::NrIconEnum_t iconType = convertUiMask(ind_data->global_ui_info.ui_mask);
      std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg =
          std::make_shared<rildata::DataNrIconTypeIndMessage>(iconType);
      msg->broadcast();
    }
  }
}

void DSDModemEndPointModule::processUiInfoInd(dsd_get_ui_info_v2_result_ind_msg_v01 *ind_data)
{
  if (ind_data != nullptr)
  {
    Log::getInstance().d("[DSDModemEndPointModule]: process dsd_get_ui_info_v2_result_ind_msg_v01 ENTRY");
    std::shared_ptr<rildata::UiInfoFragmentMessage> msg =
        std::make_shared<rildata::UiInfoFragmentMessage>();
    if (ind_data->global_ui_info_valid) {
      msg->setIcon(convertUiMask(ind_data->global_ui_info.ui_mask));
    }
    segmentTracker.addSegment(QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01, ind_data->tx_info.transaction_id,
                              ind_data->tx_info.segment_index, ind_data->tx_info.num_segments, msg);
  }
}

void DSDModemEndPointModule::processSystemStatusInd(dsd_system_status_v2_ind_msg_v01 *ind_data)
{
  if (ind_data != nullptr) {
    Log::getInstance().d("[DSDModemEndPointModule]: process dsd_system_status_v2_ind_msg_v01");
    auto msg = std::make_shared<rildata::DsdSystemStatusFragmentMessage>(*ind_data);
    segmentTracker.addSegment(QMI_DSD_SYSTEM_STATUS_V2_IND_V01, ind_data->tx_info.transaction_id,
                            ind_data->tx_info.segment_index, ind_data->tx_info.num_segments, msg);
  }
}

dsd_system_status_v2_ind_msg_v01 convertToSystemStatusV2Ind(const dsd_get_system_status_v2_result_ind_msg_v01& ind_result)
{
  dsd_system_status_v2_ind_msg_v01 ind_msg = {
    .tx_info = ind_result.tx_info,
    .avail_sys_valid = ind_result.avail_sys_valid,
    .avail_sys_len = ind_result.avail_sys_len,
    .recommended_pref_sys_index_valid = ind_result.recommended_pref_sys_index_valid,
    .recommended_pref_sys_index = ind_result.recommended_pref_sys_index,
    .null_bearer_reason_valid = ind_result.null_bearer_reason_valid,
    .null_bearer_reason = ind_result.null_bearer_reason,
    .apn_avail_sys_valid = ind_result.apn_avail_sys_valid,
    .apn_avail_sys_len = ind_result.apn_avail_sys_len,
  };
  memcpy(&ind_msg.avail_sys, &ind_result.avail_sys, sizeof(ind_msg.avail_sys));
  memcpy(&ind_msg.apn_avail_sys, &ind_result.apn_avail_sys, sizeof(ind_msg.apn_avail_sys));
  return ind_msg;
}

void DSDModemEndPointModule::processSystemStatusInd(dsd_get_system_status_v2_result_ind_msg_v01 *ind_result_data)
{
  if (ind_result_data != nullptr) {
    Log::getInstance().d("[DSDModemEndPointModule]: process dsd_get_system_status_v2_result_ind_msg_v01");
    dsd_system_status_v2_ind_msg_v01 ind_data = convertToSystemStatusV2Ind(*ind_result_data);
    auto msg = std::make_shared<rildata::DsdSystemStatusFragmentMessage>(ind_data);
    segmentTracker.addSegment(QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01, ind_data.tx_info.transaction_id,
                              ind_data.tx_info.segment_index, ind_data.tx_info.num_segments, msg);
  }
}

void DSDModemEndPointModule::getUiInfo(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<GetDataNrIconTypeMessage>(m);
  std::shared_ptr<NrIconType_t> result = nullptr;
  dsd_get_ui_info_req_msg_v01 qmiReq;
  dsd_get_ui_info_resp_msg_v01 qmiResp;
  memset(&qmiReq, 0, sizeof(qmiReq));
  memset(&qmiResp, 0, sizeof(qmiResp));

  int retVal = qmi_client_send_msg_sync(mQmiSvcClient,
                            QMI_DSD_GET_UI_INFO_REQ_V01,
                            (void *)&qmiReq, sizeof(qmiReq),
                            (void *)&qmiResp, sizeof(qmiResp),
                            QCRIL_DATA_QMI_TIMEOUT);
  if (retVal != QMI_NO_ERR) {
    Log::getInstance().d("[DSDModemEndPointModule::getUiInfo]"
                          "Failed to send QMI_DSD_GET_UI_INFO_REQ_V01");
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, result);
    return;
  }
  if (qmiResp.global_ui_info_valid) {
    NrIconEnum_t iconType = convertUiMask(qmiResp.global_ui_info.ui_mask);
    result = std::make_shared<NrIconType_t>(iconType);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
  } else if (qmiResp.tx_id_valid) {
    SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01, qmiResp.tx_id);
    Log::getInstance().d("[DSDModemEndPointModule::getUiInfo]"
                         " Begin transaction txId=" + key);
    pendingSegmentRequests[key] = msg;
    segmentTracker.startTransactionTimer(QMI_DSD_GET_UI_INFO_V2_RESULT_IND_V01, qmiResp.tx_id);
    // If there is transaction id, delay the response until following QMI segments arrive
  } else {
    result = std::make_shared<NrIconType_t>(NrIconEnum_t::NONE);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
  }
}

void DSDModemEndPointModule::handleGetDataNrIconType(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<GetDataNrIconTypeMessage>(msg);
  if (m != NULL)
  {
    if ( static_cast<DSDModemEndPoint *>(&mOwner)->getNrIconReportDisable() ) {
      Log::getInstance().d("handleGetDataNrIconType icontype disabled NONE");
      std::shared_ptr<NrIconType_t> result = std::make_shared<NrIconType_t>(NrIconEnum_t::NONE);
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
    } else {
      getUiInfo(msg);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/**
 * @brief This indication is from modem that preferred RAT is changed
 * @details Modem wants to handoff to target RAT and inform to QNP to update
 *          preferred RAT change to Transport Manager
 *
 * @param ind_data [description]
 */
void DSDModemEndPointModule::processIntentToChangeApnPrefSysInd(dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processQmiDsdIntentToChangeApnPrefSysInd = "+std::to_string(ind_data->apn_pref_sys_len));

  int numOfApns = ind_data->apn_pref_sys_len;
  for(int i=0 ; i<numOfApns ; i++) {
    //ind_data->apn_pref_sys[i].apn_name;
    // TODO: Search APN type for apn name
    // if data for APN name is connected, change the state to HANDOVER

    //ind_data->apn_pref_sys[i].pref_sys;
  }

  auto msg = std::make_shared<rildata::IntentToChangeApnPreferredSystemMessage>(*ind_data);
  msg->broadcast();
}

/**
 *
 */
void DSDModemEndPointModule::processApAsstApnPrefSysResultInd(dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processApAsstApnPrefSysResultInd = "+std::to_string(ind_data->result_info.result));

  auto msg = std::make_shared<rildata::SetApnPreferredSystemResultMessage>(*ind_data);
  msg->broadcast();
}

void DSDModemEndPointModule::processPrefApnUpdatedInd(dsd_preferred_apn_updated_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processPrefApnUpdatedInd");

  auto msg = std::make_shared<rildata::PreferredApnUpdatedMessage>(*ind_data);
  msg->broadcast();
}

void DSDModemEndPointModule::processDdsRecommendationCapabilityInd(dsd_temp_dds_rec_capability_ind_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processDdsRecommendationCapabilityInd="+
                       std::to_string(ind_data->temp_dds_rec_capability));
  std::optional<bool> ddsSwitchRecCap = static_cast<DSDModemEndPoint *>(&mOwner)->getDdsSwitchRecCapEnabled();
  bool newSwitchCapability = ind_data->temp_dds_rec_capability ? true : false;
  Log::getInstance().d("[DSDModemEndPointModule]: ddsSwitchRecCap="+
                        std::to_string((int)ddsSwitchRecCap.has_value()));

  bool needReport = false;
  AutoDdsSwitchControl_t ctrl = AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_ENABLED;
  if (!ddsSwitchRecCap.has_value()) {
    if (newSwitchCapability) {
      ctrl = AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_CAPABILITY_CHANGED;
    }
    needReport = true;
  }
  else if (!ddsSwitchRecCap.value() && newSwitchCapability) {
    ctrl = AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_CAPABILITY_CHANGED;
    needReport = true;
  }
  else if (ddsSwitchRecCap.value() && !newSwitchCapability) {
    ctrl = AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_ENABLED;
    needReport = true;
  }
  static_cast<DSDModemEndPoint *>(&mOwner)->setDdsSwitchRecCapEnabled(newSwitchCapability);
  Log::getInstance().d("[DSDModemEndPointModule]: DSDModemEndPoint::mDdsSwitchRecCapEnabled="+
                        std::to_string((int)newSwitchCapability));
  if(needReport) {
    auto msg = std::make_shared<rildata::AutoDdsSwitchControlIndMessage>(ctrl);
    msg->broadcast();
  }
}

void DSDModemEndPointModule::processDdsRecommendationInd(dsd_dds_recommendation_ind_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processDdsRecommendationInd");
  SubscriptionId_t subId = SubscriptionId_t::PRIMARY_SUBSCRIPTION_ID;
  switch (ind_data->recommended_dds) {
    case DSD_SECONDARY_SUBS_V01:
      subId = SubscriptionId_t::SECONDARY_SUBSCRIPTION_ID;
      break;
    case DSD_TERTIARY_SUBS_V01:
      subId = SubscriptionId_t::TERTIARY_SUBSCRIPTION_ID;
      break;
    case DSD_PRIMARY_SUBS_V01:
      subId = SubscriptionId_t::PRIMARY_SUBSCRIPTION_ID;
      break;
    default:
      subId = SubscriptionId_t::INVALID_SUBSCRIPTION_ID;
      return;
  }
  Log::getInstance().d("[DSDModemEndPointModule]: subId="+std::to_string((int)subId)+
                        " src="+std::to_string((int)ind_data->source));

  #ifndef RIL_FOR_LOW_RAM
  if(ind_data->perm_dds_cause_code_valid) {
    Log::getInstance().d("[DSDModemEndPointModule]: permanent cause code:" +
                         std::to_string(ind_data->perm_dds_cause_code));
  }
  #endif

  DdsSwitchType_t switchType = DdsSwitchType_t::Permanent;
  if (ind_data->switch_type_valid &&
      ind_data->switch_type == DSD_DDS_SWITCH_PERMANENT_V01)
  {
    bool isIgnore = false;
    if(ind_data->source != DSD_DDS_RECOMMEND_SOURCE_UNSPECIFIED_V01) {
      isIgnore = true;
    }
    #ifndef RIL_FOR_LOW_RAM
    else if(ind_data->perm_dds_cause_code_valid && ind_data->perm_dds_cause_code!=QMI_DSD_PERM_DDS_REASON_TEMP_CLEAN_UP_V01) {
      isIgnore = true;
    }
    #endif

    if(isIgnore) {
      Log::getInstance().d("[DSDModemEndPointModule]: ignore recommendation");
      return;
    }
  }
  switchType = (ind_data->switch_type == DSD_DDS_SWITCH_TEMPORARY_V01) ?
                DdsSwitchType_t::Temporary : DdsSwitchType_t::Permanent;

  DdsSwitchLevel_t level = DdsSwitchLevel_t::None;
  if (ind_data->temp_rec_level_valid) {
    level = (DdsSwitchLevel_t)ind_data->temp_rec_level;
  }

  auto msg = std::make_shared<DdsSwitchRecommendInternalMessage>(subId, switchType, level);
  msg->broadcast();
}

void DSDModemEndPointModule::processDsdCIWlanCapabilityInd(dsd_ciwlan_capability_ind_v01 * ind_data)
{
  bool capability = ind_data->ciwlan_capability ? true:false;
  rildata::CIWlanCapabilityType type = rildata::CIWlanCapabilityType::None;
  if (ind_data->ciwlan_subs_support_valid)
  {
    type = (ind_data->ciwlan_subs_support == DSD_CIWLAN_SUBS_SUPPORT_DDS_ONLY_V01) ?
      rildata::CIWlanCapabilityType::DDSSupported :
      rildata::CIWlanCapabilityType::BothSubsSupported;
  }
  Log::getInstance().d("[DSDModemEndPointModule]: processDsdCIWlanCapabilityInd="
                       +std::to_string(ind_data->ciwlan_capability)
                       +"type="+std::to_string((int)type));
  if ((static_cast<DSDModemEndPoint *>(&mOwner)->getDsdCIWlanCapability() != capability) ||
      (static_cast<DSDModemEndPoint *>(&mOwner)->getDsdCIWlanCapType() != type))
  {
    static_cast<DSDModemEndPoint *>(&mOwner)->setDsdCIWlanCapability(capability);
    static_cast<DSDModemEndPoint *>(& mOwner)->setDsdCIWlanCapType(type);
    auto msg = std::make_shared<DataCapabilityChangeIndMessage>();
    msg->setCIWlanSupported(capability, type);
    msg->broadcast();
  }
}

dsd_ui_icon_mode_type_enum_v01 DSDModemEndPointModule::convertToQmiMode(NrUCIconMode_t mode) {
  switch (mode) {
    case MODE_CONNECTED :
      return DSD_UI_ICON_MODE_CONNECTED_V01;
    case MODE_IDLE :
      return DSD_UI_ICON_MODE_IDLE_V01;
    default:
      return DSD_UI_ICON_MODE_CONNECTED_AND_IDLE_V01;
  }
}

void DSDModemEndPointModule::convertBandInfo(NrUCIconBandInfo_t bandInfo, dsd_ui_icon_band_info_type_v01 & qmiBandInfo) {
  if (bandInfo.enabled ) {
    qmiBandInfo.action = DSD_UI_ICON_ACTION_UPDATE_V01;
  }
  else {
    qmiBandInfo.action = DSD_UI_ICON_ACTION_REMOVE_V01;
  }
  qmiBandInfo.mode = convertToQmiMode(bandInfo.mode);
  qmiBandInfo.band_list_len =  bandInfo.band.size();
  for (int i = 0; i < bandInfo.band.size(); i++)  {
    qmiBandInfo.band_list[i] = bandInfo.band[i];
  }
}

void DSDModemEndPointModule::handleSetNrUCIconConfigure(std::shared_ptr<Message> m) {
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + m->dump());
  std::shared_ptr<SetNrUCIconConfigureMessage> msg = std::static_pointer_cast<SetNrUCIconConfigureMessage>(m);
  if (msg != nullptr) {
    dsd_set_ui_icon_config_req_msg_v01 req_msg;
    dsd_set_ui_icon_config_resp_msg_v01 resp_msg;
    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    RIL_Errno retVal = RIL_E_SUCCESS;
    do {
      if (mQmiSvcClient == nullptr) {
        retVal = RIL_E_INTERNAL_ERR;
        break;
      }
      //This value always has to be passed to modem
      req_msg.sib2_ui_icon_val_valid = true;
      if (msg->hasSibTwoValue()) {
        req_msg.sib2_ui_icon_val = msg->getSibTwoValue();
      }
      else {
        dsd_get_ui_icon_config_req_msg_v01 get_req_msg;
        dsd_get_ui_icon_config_resp_msg_v01 get_resp_msg;
        memset(&req_msg, 0, sizeof(req_msg));
        memset(&resp_msg, 0, sizeof(resp_msg));
        int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                    QMI_DSD_GET_UI_ICON_CONFIG_REQ_V01,
                                    (void *)&get_req_msg,
                                    sizeof(get_req_msg),
                                    (void *)&get_resp_msg,
                                    sizeof(get_resp_msg),
                                    QCRIL_DATA_QMI_TIMEOUT);
        if ((rc != QMI_NO_ERR) || (resp_msg.resp.result == QMI_RESULT_FAILURE_V01 )) {
        Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_GET_UI_ICON_CONFIG_REQ_V01");
        retVal = RIL_E_MODEM_ERR;
        break;
        } else{
          Log::getInstance().d("[DSDModemEndPointModule] QMI_DSD_GET_UI_ICON_CONFIG_REQ_V01::Successfully sent");
          if (get_resp_msg.sib2_ui_icon_val_valid) {
            req_msg.sib2_ui_icon_val = get_resp_msg.sib2_ui_icon_val;
          }
          else {
            Log::getInstance().d("[DSDModemEndPointModule] sib2_ui_icon_val missing");
              retVal = RIL_E_MODEM_ERR;
              break;
          }
        }
      }
      if (msg->hasNsaBandList()) {
        req_msg.uwb_nsa_band_list_valid = true;
        convertBandInfo(msg->getNsaBandList(), req_msg.uwb_nsa_band_list);
      }

      if (msg->hasSaBandList()) {
        req_msg.uwb_sa_band_list_valid = true;
        convertBandInfo(msg->getSaBandList(), req_msg.uwb_sa_band_list);
      }

      if (msg->hasMinBandWidth()) {
        req_msg.uwb_bandwidth_valid = true;
        NrUCIconBandwidthInfo_t bandwidthInfo = msg->getMinBandwidth();
        if (bandwidthInfo.enabled) {
          req_msg.uwb_bandwidth.action = DSD_UI_ICON_ACTION_UPDATE_V01;
        }
        else {
          req_msg.uwb_bandwidth.action = DSD_UI_ICON_ACTION_REMOVE_V01;
        }
        req_msg.uwb_bandwidth.mode = convertToQmiMode(bandwidthInfo.mode);
        req_msg.uwb_bandwidth.bandwidth = bandwidthInfo.bandwidth;
      }

      std::vector<NrUCIconRefreshTime_t> timeList = msg->getRefreshTimeList();

      for (int i = 0; i < timeList.size(); i++) {
        switch (timeList[i].timerType) {
          case SCG_TO_MCG_TIMER:
            req_msg.timer_scg_to_mcg_valid = true;
            req_msg.timer_scg_to_mcg = timeList[i].timeValue;
            break;
          case IDLE_TO_CONNECT_TIMER:
            req_msg.timer_idle_to_connect_valid = true;
            req_msg.timer_idle_to_connect = timeList[i].timeValue;
            break;
          case IDLE_TIMER:
            req_msg.timer_idle_valid = true;
            req_msg.timer_idle = timeList[i].timeValue;
            break;
          default:
            //Shouldnt come here at all
            break;
        }
      }

      int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                    QMI_DSD_SET_UI_ICON_CONFIG_REQ_V01,
                                    (void *)&req_msg,
                                    sizeof(req_msg),
                                    (void *)&resp_msg,
                                    sizeof(resp_msg),
                                    QCRIL_DATA_QMI_TIMEOUT);
      if ((rc != QMI_NO_ERR) || (resp_msg.resp.result == QMI_RESULT_FAILURE_V01 )) {
        Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_SET_UI_ICON_CONFIG_REQ_V01");
        retVal = RIL_E_MODEM_ERR;
      }
    } while(0);
    std::shared_ptr<RIL_Errno> result = std::make_shared<RIL_Errno>(retVal);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
  } else {
    Log::getInstance().d("[DSDModemEndPointModule]: Improper message received = " + msg->dump());
  }
}

void DSDModemEndPointModule::handleSegmentTimeout(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleSegmentTimeout : Handling msg = " + m->dump());

  auto msg= std::static_pointer_cast<SegmentTimeoutMessage>(m);
  if(msg != nullptr && msg->getEndPointName() == mName)
  {
    segmentTracker.handleSegmentTimeout(msg->getMessageId(), msg->getTransactionId());
  }
}

void DSDModemEndPointModule::handleGetDsdSystemStatusV2(std::shared_ptr<Message> m) {
  Log::getInstance().d("[DSDModemEndPointModule]: handleGetDsdSystemStatusV2 ENTRY");
  auto msg= std::static_pointer_cast<GetDsdSystemStatusMessage>(m);
  if (msg != nullptr) {
    dsd_get_system_status_v2_req_msg_v01  sys_req_msg;
    dsd_get_system_status_v2_resp_msg_v01  sys_resp_msg;

    memset(&sys_req_msg, 0, sizeof(sys_req_msg));
    memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));

    /* Query the current system status from QMI-DSD service */
    int rc = qmi_client_send_msg_sync( mQmiSvcClient,
                                QMI_DSD_GET_SYSTEM_STATUS_V2_REQ_V01,
                                &sys_req_msg,
                                sizeof(sys_req_msg),
                                &sys_resp_msg,
                                sizeof(sys_resp_msg),
                                QCRIL_DATA_QMI_TIMEOUT);

    if (QMI_NO_ERR != rc || QMI_NO_ERR != sys_resp_msg.resp.result) {
      QCRIL_LOG_ERROR("failed to send dsd_get_system_status_v2, rc=%d err=%d",rc,
                                sys_resp_msg.resp.error);
      msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
      return;
    }

    if (sys_resp_msg.transaction_id_valid) {
      SegmentTracker::KeyType_t key = SegmentTracker::serialize(QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01, sys_resp_msg.transaction_id);
      Log::getInstance().d("[DSDModemEndPointModule::handleGetDsdSystemStatusV2]"
                          " Begin transaction tx=" + key);
      pendingSegmentRequests[key] = msg;
      // expect an indication later to respond to the message
      segmentTracker.startTransactionTimer(QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01, sys_resp_msg.transaction_id);
    } else {
      Log::getInstance().d("[DSDModemEndPointModule::handleGetDsdSystemStatusV2] invalid transaction id");
      msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
  }
  else {
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}

#if (!defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE))
void DSDModemEndPointModule::handleChangeStackIdDsdSyncMessage(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + msg->dump());
    auto m = std::static_pointer_cast<ChangeStackIdDsdSyncMessage>(msg);
    if (m != nullptr) {
      bool successBit = DSDModemEndPointModule::handleQmiBinding(
            static_cast<qcril_instance_id_e_type>(m->getInstanceId()), m->getStackId());
      if (successBit) {
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
        return;
      }
      Log::getInstance().d("[DSDModemEndPointModule]: Stack Id Changed Failed");
      m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
}

void DSDModemEndPointModule::handleSetVowifiConfiguration(std::shared_ptr<Message> m) {
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + m->dump());
  std::shared_ptr<SetVowifiConfigurationMessage> msg = std::static_pointer_cast<SetVowifiConfigurationMessage>(m);
  if (msg != nullptr) {
    qmi_dsd_configure_vowifi_service_req_msg_v01 req_msg;
    qmi_dsd_configure_vowifi_service_resp_msg_v01 resp_msg;
    memset(&req_msg, 0, sizeof(req_msg));
    memset(&resp_msg, 0, sizeof(resp_msg));

    RIL_Errno retVal = RIL_E_SUCCESS;
    if (msg->hasFqdn()) {
      req_msg.fqdn_valid = true;
      strlcpy(req_msg.fqdn, msg->getFqdn().c_str(), std::min((size_t)QMI_DSD_MAX_FQDN_LEN_V01, msg->getFqdn().length()) + 1);
    }
    if (msg->hasV4Address()) {
      in_addr buffer{};
      int convertResult = inet_pton(AF_INET, msg->getV4Address().c_str(), &buffer);
      if (convertResult == 1) {
        req_msg.vowifi_ipv4_addr_valid = true;
        assert(sizeof(buffer) == sizeof(req_msg.vowifi_ipv4_addr));
        memcpy((void *)&req_msg.vowifi_ipv4_addr, (void *)&buffer, sizeof(buffer));
      } else {
        Log::getInstance().d("[DSDModemEndPointModule] V4 conversion failed");
      }
    }
    if (msg->hasV6Address()) {
      in6_addr buffer{};
      int convertResult = inet_pton(AF_INET6, msg->getV6Address().c_str(), &buffer);
      if (convertResult == 1) {
        req_msg.vowifi_ipv6_addr_valid = true;
        assert(sizeof(buffer) == sizeof(req_msg.vowifi_ipv6_addr));
        memcpy((void *)req_msg.vowifi_ipv6_addr, (void *)&buffer, sizeof(buffer));
      } else {
        Log::getInstance().d("[DSDModemEndPointModule] V6 conversion failed");
      }
    }
    int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                  QMI_DSD_CONFIGURE_VOWIFI_SERVICE_REQ_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  QCRIL_DATA_QMI_TIMEOUT);
    if ((rc != QMI_NO_ERR) || (resp_msg.resp.result == QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_CONFIGURE_VOWIFI_SERVICE_REQ_V01");
        retVal = RIL_E_OEM_ERROR_2;
    }
    std::shared_ptr<RIL_Errno> result = std::make_shared<RIL_Errno>(retVal);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
  } else {
    Log::getInstance().d("[DSDModemEndPointModule]: Improper message received = " + msg->dump());
  }
}
#endif //RIL_FOR_LOW_RAM

void DSDModemEndPointModule::handleGetDsdSystemStatusV1(std::shared_ptr<Message> m) {
  Log::getInstance().d("[DSDModemEndPointModule]: handleGetDsdSystemStatusV1 ENTRY");
  auto msg= std::static_pointer_cast<GetDsdSystemStatusMessage>(m);
  if (msg != nullptr) {
    dsd_get_system_status_req_msg_v01  sys_req_msg;
    dsd_get_system_status_resp_msg_v01  sys_resp_msg;

    memset(&sys_req_msg, 0, sizeof(sys_req_msg));
    memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));

    /* Query the current system status from QMI-DSD service */

    qmi_client_send_msg_sync( mQmiSvcClient,
                                  QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                  &sys_req_msg,
                                  sizeof(sys_req_msg),
                                  &sys_resp_msg,
                                  sizeof(sys_resp_msg),
                                  QCRIL_DATA_QMI_TIMEOUT);

    if (QMI_NO_ERR != sys_resp_msg.resp.result) {
      QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",
                                  sys_resp_msg.resp.error);
      return;
    }

    /* Process the system status response */
    DsdSystemStatusResult_t result = {};
    if(sys_resp_msg.avail_sys_valid) {
      result.resp_ind.avail_sys_valid = true;
      result.resp_ind.avail_sys_len = sys_resp_msg.avail_sys_len;
      memcpy(&result.resp_ind.avail_sys, &sys_resp_msg.avail_sys, sizeof(result.resp_ind.avail_sys));
    }
    if(sys_resp_msg.apn_avail_sys_info_valid) {
      result.resp_ind.apn_avail_sys_info_valid = true;
      result.resp_ind.apn_avail_sys_info_len = sys_resp_msg.apn_avail_sys_info_len;
      memcpy(&result.resp_ind.apn_avail_sys_info, &sys_resp_msg.apn_avail_sys_info, sizeof(result.resp_ind.apn_avail_sys_info));
    }
    if(sys_resp_msg.global_pref_sys_valid) {
      result.resp_ind.global_pref_sys_valid = true;
      memcpy(&result.resp_ind.global_pref_sys_valid, &sys_resp_msg.global_pref_sys_valid, sizeof(result.resp_ind.global_pref_sys_valid));
    }
    if(sys_resp_msg.apn_pref_sys_valid) {
      result.resp_ind.apn_pref_sys_valid = true;
      result.resp_ind.apn_pref_sys_len = sys_resp_msg.apn_pref_sys_len;
      memcpy(&result.resp_ind.apn_pref_sys, &sys_resp_msg.apn_pref_sys, sizeof(result.resp_ind.apn_pref_sys));
    }
    if(sys_resp_msg.null_bearer_reason_valid) {
      result.resp_ind.null_bearer_reason_valid = true;
      memcpy(&result.resp_ind.null_bearer_reason, &sys_resp_msg.null_bearer_reason, sizeof(result.resp_ind.null_bearer_reason));
    }

    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<DsdSystemStatusResult_t>(result));
    return;
  }
}

dsd_rat_preference_enum_v01 convertRatPreferenceToDsd(RatPreference pref) {
  switch (pref) {
    case RatPreference::Inactive:
      return DSD_RAT_PREFERENCE_INACTIVE_V01;
    case RatPreference::CellularOnly:
      return DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01;
    case RatPreference::WifiOnly:
      return DSD_RAT_PREFERENCE_WIFI_ONLY_V01;
    case RatPreference::CellularPreferred:
      return DSD_RAT_PREFERENCE_CELLULAR_PREFERRED_V01;
    case RatPreference::WifiPreferred:
      return DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01;
    case RatPreference::ImsPreferred:
      return DSD_RAT_PREFERENCE_IMS_PREFERRED_V01;
    default:
      Log::getInstance().d("[DSDModemEndPointModule]: received invalid rat pref=" + to_string((int)pref));
      return DSD_RAT_PREFERENCE_ENUM_MIN_ENUM_VAL_V01;
  }
}

void DSDModemEndPointModule::handleSetRatPref(std::shared_ptr<Message> m) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + m->dump());
  std::shared_ptr<SetRatPrefMessage> msg = std::static_pointer_cast<SetRatPrefMessage>(m);
  if (msg != nullptr) {

    RIL_Errno retVal = RIL_E_SUCCESS;
    //Check for validity of RIL values
    if ((msg->getRatPreference() >= RatPreference::Inactive &&
         msg->getRatPreference() <= RatPreference::ImsPreferred) &&
        (msg->getRoamingRatPreference() >= RatPreference::Inactive &&
         msg->getRoamingRatPreference() <= RatPreference::ImsPreferred)) {

      //Cache the latest values from the message
      homeRatPref = msg->getRatPreference();
      roamingRatPref = msg->getRoamingRatPreference();
      ratPrefReceived = true;
      if (mQmiSvcClient == nullptr) {
        retVal = RIL_E_OEM_ERROR_1;
      }
      else {
        if (!sendNotifyDataSettings(isRoaming? roamingRatPref:homeRatPref)) {
          retVal = RIL_E_OEM_ERROR_2;
        }
      }
    }
    else {
      retVal = RIL_E_INVALID_ARGUMENTS;
    }

    std::shared_ptr<RIL_Errno> result = std::make_shared<RIL_Errno>(retVal);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
  } else {
    Log::getInstance().d("[DSDModemEndPointModule]: Improper message received = " + msg->dump());
  }
}

bool DSDModemEndPointModule::sendNotifyDataSettings(RatPreference ratPref) {
  bool retVal = true;
  dsd_notify_data_settings_req_msg_v01 req_msg;
  dsd_notify_data_settings_resp_msg_v01 resp_msg;
  memset(&req_msg, 0, sizeof(req_msg));
  memset(&resp_msg, 0, sizeof(resp_msg));

  req_msg.rat_preference_valid = true;
  req_msg.rat_preference = convertRatPreferenceToDsd(ratPref);

  int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                    QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                    (void *)&req_msg,
                                    sizeof(req_msg),
                                    (void *)&resp_msg,
                                    sizeof(resp_msg),
                                    QCRIL_DATA_QMI_TIMEOUT);
  if ((rc != QMI_NO_ERR) || (resp_msg.resp.result == QMI_RESULT_FAILURE_V01 )) {
    Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01");
    retVal = false;
  }
  return retVal;
}

void DSDModemEndPointModule::handleGetDsdSystemStatus(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + m->dump());
  auto msg= std::static_pointer_cast<GetDsdSystemStatusMessage>(m);

  if (mQmiSvcClient != nullptr)
  {
    if ( static_cast<DSDModemEndPoint *>(&mOwner)->getSystemStatusV2() ) {
      handleGetDsdSystemStatusV2(m);
    } else
    {
      handleGetDsdSystemStatusV1(m);
    }
  }
}

#ifdef QMI_RIL_UTF
void DSDModemEndPointModule::cleanUpQmiSvcClient()
{
  ModemEndPointModule::cleanUpQmiSvcClient();
  static_cast<DSDModemEndPoint *>(&mOwner)->setSystemStatusV2(false);
  static_cast<DSDModemEndPoint *>(&mOwner)->setUiInfoV2(false);
  ratPrefReceived = false;
  static_cast<DSDModemEndPoint *>(&mOwner)->cleanup();
}
#endif
