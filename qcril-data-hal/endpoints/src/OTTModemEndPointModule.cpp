/******************************************************************************

  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

#******************************************************************************/
#ifdef FEATURE_DATA_LQE
#include "OTTModemEndPointModule.h"
#include <modules/qmi/QmiIndMessage.h>
#include "framework/Log.h"
#include "sync/ToggleLqeIndicationSyncMessage.h"
#include "UnSolMessages/LceInitMessage.h"
#include "UnSolMessages/LceDeInitMessage.h"
#include "UnSolMessages/LinkCapIndMessage.h"
#include "sync/SetCapReportCriteriaSyncMessage.h"

DEFINE_MSG_ID_INFO(OTTModemEndPoint_QMI_IND)
DEFINE_MSG_ID_INFO(OTTModemEndPoint_ENDPOINT_STATUS_IND)

OTTModemEndPointModule::OTTModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner,
        REG_MSG(OTTModemEndPoint_QMI_IND),
            REG_MSG(OTTModemEndPoint_ENDPOINT_STATUS_IND))
{
  mServiceObject = nullptr;
  lqeInited = false;
  downlinkCapacityKbps = 0;
  uplinkCapacityKbps = 0;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(ToggleLqeIndicationSyncMessage, OTTModemEndPointModule::handleToggleLqeIndicationSyncMessage),
    HANDLER(LceInitMessage, OTTModemEndPointModule::handleLceInitMessage),
    HANDLER(LceDeInitMessage, OTTModemEndPointModule::handleLceDeInitMessage),
    HANDLER(SetCapReportCriteriaSyncMessage, OTTModemEndPointModule::handleSetCapReportCriteriaSyncMessage),
    HANDLER_MULTI(QmiIndMessage, OTTModemEndPoint_QMI_IND, OTTModemEndPointModule::handleOTTQmiIndMessage),
  };
}

OTTModemEndPointModule::~OTTModemEndPointModule()
{
  mLooper = nullptr;
}

void OTTModemEndPointModule::init()
{
  ModemEndPointModule::init();
  mStackId[0] = 0;
}

void OTTModemEndPointModule::handleLceDeInitMessage(std::shared_ptr<Message> msg) {
  if (msg == nullptr) {
    return;
  }
  Log::getInstance().d("[OTTModemEndPointModule]: Handling msg = " + msg->dump());
  lqeInited = false;
  downlinkCapacityKbps = 0;
  uplinkCapacityKbps = 0;
}

bool OTTModemEndPointModule::registerForIndication(LqeDataDirection direction)
{
  ott_indication_register_req_msg_v01 req;
  ott_indication_register_resp_msg_v01 resp;
  memset((void *)&req, 0, sizeof(req));
  memset((void *)&resp, 0, sizeof(resp));
  if (direction == LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION) {
    req.report_dl_thrpt_info_valid = 1;
    req.report_dl_thrpt_info = 1;
  }
  else {
    req.report_ul_thrpt_info_valid = 1;
    req.report_ul_thrpt_info = 1;
  }
  int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                QMI_OTT_INDICATION_REGISTER_REQ_V01,
                                &req,
                                sizeof(ott_indication_register_req_msg_v01),
                                &resp,
                                sizeof(ott_indication_register_resp_msg_v01),
                                QCRIL_DATA_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    Log::getInstance().d("OTT Indication Registration failed with rc : " + std::to_string(rc) + " qmi_err = " + std::to_string(resp.resp.error));
  }
  else {
    Log::getInstance().d("OTT Indication Registration Successful");
    return true;
  }
  return false;
}

void OTTModemEndPointModule::handleLceInitMessage(std::shared_ptr<Message> msg) {
  if (msg == nullptr) {
    return;
  }
  Log::getInstance().d("[OTTModemEndPointModule]: Handling msg = " + msg->dump());
  lqeInited = true;
  if (registerForIndication(LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION) &&
      registerForIndication(LqeDataDirection::LQE_DATA_UPLINK_DIRECTION)) {
    Log::getInstance().d("OTT Indication Registration for threshold based reporting is Successful");
  }
}

void OTTModemEndPointModule::handleToggleLqeIndicationSyncMessage (std::shared_ptr<Message> msg)
{
  int rc = 0;
  if (msg == nullptr) {
    return;
  }
  Log::getInstance().d("[OTTModemEndPointModule]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<ToggleLqeIndicationSyncMessage>(msg);
  if (m == nullptr) {
    return;
  }
  if (mQmiSvcClient == nullptr) {
    Log::getInstance().d("Qmi Client is Nullptr");
    m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    return;
  }
  LqeDataDirection direction = m->getLqeDirection();
  LqeIndicationSwitch indication = m->getLqeIndication();
  if (direction == LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION) {
    ott_configure_downlink_throughput_settings_req_msg_v01 req;
    ott_configure_downlink_throughput_settings_resp_msg_v01 resp;
    memset((void *)&req, 0, sizeof(req));
    memset((void *)&resp, 0, sizeof(resp));
    if (indication == LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS) {
      req.dl_threshold_action_valid = 1;
      req.dl_threshold_action = OTT_THRESHOLD_ACTION_START_V01;
      Log::getInstance().d("Turning on downlink throughput indications:" + std::to_string (req.dl_threshold_action));
    }
    else
    {
      req.dl_threshold_action_valid = 1;
      req.dl_threshold_action = OTT_THRESHOLD_ACTION_STOP_V01;
      Log::getInstance().d("Turning off downlink throughput indications:" + std::to_string (req.dl_threshold_action));
    }
    rc = qmi_client_send_msg_sync(mQmiSvcClient,
                              QMI_OTT_CONFIGURE_DOWNLINK_THROUGHPUT_SETTINGS_REQ_V01,
                              &req,
                              sizeof(req),
                              &resp,
                              sizeof(resp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      Log::getInstance().d("Downlink Indication Processing Failed with rc : " + std::to_string(rc) + " qmi_err = " + std::to_string(resp.resp.error));
      m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
    else {
      Log::getInstance().d("Downlink Indication Processing Successful");
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
    }
  } else {
    ott_configure_uplink_throughput_settings_req_msg_v01 req;
    ott_configure_uplink_throughput_settings_resp_msg_v01 resp;
    memset((void *)&req, 0, sizeof(req));
    memset((void *)&resp, 0, sizeof(resp));
    if (indication == LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS) {
      req.ul_threshold_action_valid = 1;
      req.ul_threshold_action = OTT_THRESHOLD_ACTION_START_V01;
      Log::getInstance().d("Turning on uplink throughput indications:" + std::to_string (req.ul_threshold_action));
    }
    else
    {
      req.ul_threshold_action_valid = 1;
      req.ul_threshold_action = OTT_THRESHOLD_ACTION_STOP_V01;
      Log::getInstance().d("Turning off uplink throughput indications:" + std::to_string (req.ul_threshold_action));
    }
    rc = qmi_client_send_msg_sync(mQmiSvcClient,
                              QMI_OTT_CONFIGURE_UPLINK_THROUGHPUT_SETTINGS_REQ_V01,
                              &req,
                              sizeof(req),
                              &resp,
                              sizeof(resp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      Log::getInstance().d("Uplink Indication Processing Failed with rc : " + std::to_string(rc)
                        +" qmi_err = " + std::to_string(resp.resp.error));
      m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
    else {
      Log::getInstance().d("Uplink Indication Processing Successful");
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
    }
  }
}

bool OTTModemEndPointModule::handleQmiBinding (qcril_instance_id_e_type instanceId, int8_t stackId)
{
  Log::getInstance().d("[OTTModemEndPointModule] handleQmiBinding");
  if (stackId < 0)
    return false;

  ott_bind_subscription_req_msg_v01 bind_request;
  ott_bind_subscription_resp_msg_v01 bind_resp;
  memset(&bind_request, 0, sizeof(bind_request));
  memset(&bind_resp, 0, sizeof(bind_resp));

  if (stackId == 0) {
    bind_request.subscription = OTT_PRIMARY_SUBS_V01;
  } else if (stackId == 1) {
    bind_request.subscription = OTT_SECONDARY_SUBS_V01;
  } else if (stackId == 2) {
    bind_request.subscription = OTT_TERTIARY_SUBS_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_OTT_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&bind_request, sizeof(bind_request),
         (void*)&bind_resp, sizeof(bind_resp),
         ModemEndPointModule::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d("[OTTModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);
  return false;
}

qmi_idl_service_object_type OTTModemEndPointModule::getServiceObject()
{
  return ott_get_service_object_v01();
}

void OTTModemEndPointModule::handleOTTQmiIndMessage(std::shared_ptr<Message> msg)
{
  auto shared_indMsg(
        std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[OTTModemEndPointModule]: Handling msg = " + msg->dump());

  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr)
  {
    OTTUnsolicitedIndHdlr(indData->msgId, indData->indData);
  } else
  {
    Log::getInstance().d("[OTTModemEndPointModule]:Unexpected, null data from message");
  }
}

void OTTModemEndPointModule::OTTUnsolDownLCEHandler(unsigned char *decoded_payload)
{
  if (lqeInited) {
    ott_downlink_throughput_info_ind_msg_v01* res =
    (ott_downlink_throughput_info_ind_msg_v01*)(decoded_payload);
    if (res) {
      if (res->downlink_rate_valid) {
        if (res->downlink_rate != downlinkCapacityKbps){
          /* if there is any change is downlink speed, query for uplink rate as well*/
          ott_query_uplink_throughput_info_req_msg_v01 req;
          ott_query_uplink_throughput_info_resp_msg_v01 resp;
          memset((void *)&req, 0, sizeof(req));
          memset((void *)&resp, 0, sizeof(resp));
          int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                    QMI_OTT_QUERY_UPLINK_THROUGHPUT_INFO_REQ_V01,
                    &req,
                    sizeof(req),
                    &resp,
                    sizeof(resp),
                    QCRIL_DATA_QMI_TIMEOUT);
          if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01) {
            Log::getInstance().d("Uplink Rate Query Failed with rc : " + std::to_string(rc)
                +" qmi_err = " + std::to_string(resp.resp.error));
          }
          else {
            if (resp.uplink_rate_valid) {
              uplinkCapacityKbps = resp.uplink_rate;
            }
          }
          downlinkCapacityKbps = res->downlink_rate;
          LinkCapEstimate_t capacity = {downlinkCapacityKbps, uplinkCapacityKbps};
          auto msg = std::make_shared<LinkCapIndMessage>(capacity);
          msg->broadcast();
        }
      }
    }
  }
  else {
    Log::getInstance().d("[OTTUnsolDownLCEHandler]: LQE is not Inited");
  }
}

void OTTModemEndPointModule::OTTUnsolUpLCEHandler(unsigned char *decoded_payload)
{
  if (lqeInited) {
    ott_uplink_throughput_info_ind_msg_v01* res =
    (ott_uplink_throughput_info_ind_msg_v01*)(decoded_payload);
    if (res) {
      if (res->uplink_rate_valid) {
        if (res->uplink_rate != uplinkCapacityKbps){
          /* if there is any change is uplink speed, query for downlink rate as well*/
          ott_get_downlink_throughput_info_req_msg_v01 req;
          ott_get_downlink_throughput_info_resp_msg_v01 resp;
          memset((void *)&req, 0, sizeof(req));
          memset((void *)&resp, 0, sizeof(resp));
          int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                    QMI_OTT_GET_DOWNLINK_THROUGHPUT_INFO_REQ_V01,
                    &req,
                    sizeof(req),
                    &resp,
                    sizeof(resp),
                    QCRIL_DATA_QMI_TIMEOUT);
          if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01) {
            Log::getInstance().d("Downlink Rate Query Failed with rc : " + std::to_string(rc)
                +" qmi_err = " + std::to_string(resp.resp.error));
          }
          else {
            if (resp.downlink_rate_valid) {
              downlinkCapacityKbps = resp.downlink_rate;
            }
          }
          uplinkCapacityKbps = res->uplink_rate;
          LinkCapEstimate_t capacity = {downlinkCapacityKbps, uplinkCapacityKbps};
          auto msg = std::make_shared<LinkCapIndMessage>(capacity);
          msg->broadcast();
        }
      }
    }
  }
  else {
    Log::getInstance().d("[OTTUnsolUpLCEHandler]: LQE is not Inited");
  }
}

void OTTModemEndPointModule::OTTUnsolicitedIndHdlr(unsigned int msg_id, unsigned char *decoded_payload)
{
  Log::getInstance().d("[OTTModemEndPointModule]: OTTUnsolicitedIndHdlr ENTRY");
  switch(msg_id)
  {
    case QMI_OTT_DOWNLINK_THROUGHPUT_INFO_IND_V01:
      if (lqeInited == false) {
        Log::getInstance().d("LQE is not Inited. Ignoring OTT IND MSG : "+ std::to_string(msg_id));
        break;
      }
      if (decoded_payload != nullptr)
      {
        OTTUnsolDownLCEHandler(decoded_payload);
      }
      break;
    case QMI_OTT_UPLINK_THROUGHPUT_INFO_IND_V01:
      if (lqeInited == false) {
        Log::getInstance().d("LQE is not Inited. Ignoring OTT IND MSG : "+ std::to_string(msg_id));
        break;
      }
      if (decoded_payload != nullptr)
      {
        OTTUnsolUpLCEHandler(decoded_payload);
      }
      break;
    default:
      Log::getInstance().d("Ignoring OTT IND MSG :"+ std::to_string(msg_id));
      break;
  }
}

void OTTModemEndPointModule::handleSetCapReportCriteriaSyncMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[OTTModemEndPointModule]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<SetCapReportCriteriaSyncMessage>(msg);
  if (m == nullptr) {
    return;
  }
  LinkCapCriteria_t criteria = m->getParams();
  ott_configure_downlink_throughput_settings_req_msg_v01 req;
  ott_configure_downlink_throughput_settings_resp_msg_v01 resp;
  ott_configure_uplink_throughput_settings_req_msg_v01 reqUl;
  ott_configure_uplink_throughput_settings_resp_msg_v01 respUl;
  memset((void *)&reqUl, 0, sizeof(reqUl));
  memset((void *)&respUl, 0, sizeof(respUl));
  memset((void *)&req, 0, sizeof(req));
  memset((void *)&resp, 0, sizeof(resp));
  if (criteria.ran == AccessNetwork_t::UTRAN) {
    req.rat_type = OTT_RAT_WCDMA_V01;
    reqUl.rat_type = OTT_RAT_WCDMA_V01;
  }
  else if (criteria.ran == AccessNetwork_t::EUTRAN) {
    req.rat_type = OTT_RAT_LTE_V01;
    reqUl.rat_type = OTT_RAT_LTE_V01;
  }
  else if (criteria.ran == AccessNetwork_t::NGRAN) {
    req.rat_type = OTT_RAT_NR5G_V01;
    reqUl.rat_type = OTT_RAT_NR5G_V01;
  }
  req.rat_type_valid = 1;
  reqUl.rat_type_valid = 1;
  req.dl_throughput_hyst_time_valid = 1;
  reqUl.ul_throughput_hyst_time_valid = 1;
  req.dl_throughput_hyst_time = criteria.hysteresisMs;
  reqUl.ul_throughput_hyst_time = criteria.hysteresisMs;
  req.dl_throughput_hyst_mag_chng_size_valid = 1;
  reqUl.ul_throughput_hyst_mag_chng_size_valid = 1;
  req.dl_throughput_hyst_mag_chng_size = criteria.hysteresisDlKbps;
  reqUl.ul_throughput_hyst_mag_chng_size = criteria.hysteresisUlKbps;
  req.dl_throughput_threshold_list_valid = 1;
  reqUl.ul_throughput_threshold_list_valid = 1;
  req.dl_throughput_threshold_list_len = MIN(QMI_OTT_DL_THRESHOLD_LIST_LEN_V01, criteria.thresholdsDownlinkKbps.size());
  reqUl.ul_throughput_threshold_list_len = MIN(QMI_OTT_UL_THRESHOLD_LIST_LEN_V01, criteria.thresholdsUplinkKbps.size());
  if( req.dl_throughput_threshold_list_len > 0 ) {
    for (uint32_t i = 0; i < req.dl_throughput_threshold_list_len; i++) {
      req.dl_throughput_threshold_list[i] = criteria.thresholdsDownlinkKbps[i];
    }
  }
  if( reqUl.ul_throughput_threshold_list_len > 0 ) {
    for (uint32_t i = 0; i < reqUl.ul_throughput_threshold_list_len; i++) {
      reqUl.ul_throughput_threshold_list[i] = criteria.thresholdsUplinkKbps[i];
    }
  }
  bool dlSetCap = false;
  bool ulSetCap = false;
  int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                              QMI_OTT_CONFIGURE_UPLINK_THROUGHPUT_SETTINGS_REQ_V01,
                              &reqUl,
                              sizeof(reqUl),
                              &respUl,
                              sizeof(respUl),
                              QCRIL_DATA_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc || respUl.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    Log::getInstance().d("Setting Uplink Capacity Failed with rc : " + std::to_string(rc)
                +" qmi_err = " + std::to_string(resp.resp.error));
  }
  else {
    Log::getInstance().d("Setting Uplink Capacity Successful");
    ulSetCap = true;
  }
  rc = qmi_client_send_msg_sync(mQmiSvcClient,
                              QMI_OTT_CONFIGURE_DOWNLINK_THROUGHPUT_SETTINGS_REQ_V01,
                              &req,
                              sizeof(req),
                              &resp,
                              sizeof(resp),
                              QCRIL_DATA_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    Log::getInstance().d("Setting Downlink Capacity Failed with rc : " + std::to_string(rc)
                +" qmi_err = " + std::to_string(resp.resp.error));
  }
  else {
    Log::getInstance().d("Setting Downlink Capacity Successful");
    dlSetCap = true;
  }
  if (dlSetCap && ulSetCap) {
    Log::getInstance().d("[OTTModemEndPointModule]: SetCapReportCriteria is Successful");
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
  }
  else {
    Log::getInstance().d("[OTTModemEndPointModule]: SetCapReportCriteria FAILED");
    m->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}

#endif /* FEATURE_DATA_LQE */
