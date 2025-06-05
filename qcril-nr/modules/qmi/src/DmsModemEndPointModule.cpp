/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include "modules/qmi/DmsModemEndPointModule.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"
#include "device_management_service_v01.h"

DEFINE_MSG_ID_INFO(DMS_QMI_IND)
DEFINE_MSG_ID_INFO(DMS_ENDPOINT_STATUS_IND)

DmsModemEndPointModule::DmsModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner,
        REG_MSG(DMS_QMI_IND), REG_MSG(DMS_ENDPOINT_STATUS_IND)) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
  };
}

DmsModemEndPointModule::~DmsModemEndPointModule() {
    mLooper = nullptr;
}

bool DmsModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  dms_bind_subscription_req_msg_v01 dms_bind_request;
  dms_bind_subscription_resp_msg_v01 dms_bind_resp;
  memset(&dms_bind_request, 0, sizeof(dms_bind_request));
  memset(&dms_bind_resp, 0, sizeof(dms_bind_resp));

  if (stackId == 0) {
    dms_bind_request.bind_subs = DMS_PRIMARY_SUBS_V01;
  } else if (stackId == 1) {
    dms_bind_request.bind_subs = DMS_SECONDARY_SUBS_V01;
  } else if (stackId == 2) {
    dms_bind_request.bind_subs = DMS_TERTIARY_SUBS_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_DMS_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&dms_bind_request, sizeof(dms_bind_request),
         (void*)&dms_bind_resp, sizeof(dms_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && dms_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[DmsModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    else if(QMI_SERVICE_ERR == res)
    {
        return false;
    }

    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}


qmi_idl_service_object_type DmsModemEndPointModule::getServiceObject() {
  return dms_get_service_object_v01();
}
