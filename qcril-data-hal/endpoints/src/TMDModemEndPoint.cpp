/**
* Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#include "TMDModemEndPoint.h"

using rildata::TMDModemEndPoint;

constexpr const char *TMDModemEndPoint::NAME;

void TMDModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}

DeviceMitigationLevel_t TMDModemEndPoint::getCurrentMitigationLevel(std::string device)
{
  Log::getInstance().d("[" + mName + "]: getCurrentMitigationLevel="+device);
  DeviceMitigationLevel_t level = {};
  tmd_get_mitigation_level_req_msg_v01 req;
  tmd_get_mitigation_level_resp_msg_v01 resp;
  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  memcpy(&req.mitigation_device.mitigation_dev_id, device.c_str(), device.length()+1);

  int rc = sendRawSync(QMI_TMD_GET_MITIGATION_LEVEL_REQ_V01,
                    (void *)&req, sizeof(req),
                    (void *)&resp, sizeof(resp),
                    DEFAULT_SYNC_TIMEOUT);

  if( rc != QMI_NO_ERR || (resp.resp.result != QMI_RESULT_SUCCESS_V01))
  {
    Log::getInstance().d("[TMDModemEndPoint]: failed with rc=" + std::to_string(rc) +
    ", qmi_err=" + std::to_string(resp.resp.error));
  }

  if (rc == QMI_NO_ERR)
  {
    if(resp.current_mitigation_level_valid) {
      level.current = resp.current_mitigation_level;
    }
    if(resp.requested_mitigation_level_valid) {
      level.requested = resp.requested_mitigation_level;
    }
  }

  return level;
}

Message::Callback::Status TMDModemEndPoint::setMitigationLevel(std::string device, int level)
{
  Log::getInstance().d("[" + mName + "]: setMitigationLevel="+device+", level="+std::to_string(level));
  int rc;
  Message::Callback::Status ret = Message::Callback::Status::FAILURE;
  tmd_set_mitigation_level_req_msg_v01 req;
  tmd_set_mitigation_level_resp_msg_v01 resp;
  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  strlcpy((char*)&req.mitigation_dev_id.mitigation_dev_id, device.c_str(), device.length()+1);
  req.mitigation_level = level;

  rc = sendRawSync(QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01,
                    (void *)&req, sizeof(req),
                    (void *)&resp, sizeof(resp),
                    DEFAULT_SYNC_TIMEOUT);

  if( rc != QMI_NO_ERR || (resp.resp.result != QMI_RESULT_SUCCESS_V01))
  {
    Log::getInstance().d("[TMDModemEndPoint]: failed with rc=" + std::to_string(rc) +
    ", qmi_err=" + std::to_string(resp.resp.error));
  }

  if (rc == QMI_NO_ERR)
  {
    ret = Message::Callback::Status::SUCCESS;
  }

  return ret;
}
