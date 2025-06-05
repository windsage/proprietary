/******************************************************************************
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef RIL_FOR_MDM_LE
#include "VoiceCallModemEndPoint.h"
#include <sstream>

using std::to_string;

void VoiceCallModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                        GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}

bool VoiceCallModemEndPoint::isVoiceCall()
{
  //int rc = QMI_NO_ERR;
  bool ret = FALSE;
  voice_get_all_call_info_req_msg_v02 request;
  voice_get_all_call_info_resp_msg_v02 response;

  memset(&request, 0, sizeof(request));
  memset(&response, 0, sizeof(response));

  auto rc = sendRawSync(QMI_VOICE_GET_ALL_CALL_INFO_REQ_V02, &request, (int)sizeof(request),
                        &response, (int)sizeof(response), QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || response.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    Log::getInstance().d("Failed to retrieve voicecall information");
  }

  else {
    if(response.call_info_len != 0)
    {
      for (uint32_t i = 0; i < response.call_info_len; i++)
      {
        if(response.call_info[i].call_state != call_state_enum_v02::CALL_STATE_END_V02)
        {
          ret = TRUE;
          break;
        }
      }
    }
  }
  return ret;
}

bool VoiceCallModemEndPoint::isVoiceCallInActiveState()
{
  if(mCallState == VoiceCallStateEnum::CALL_STATE_ENUM_MIN_ENUM_VAL ||
     mCallState == VoiceCallStateEnum::CALL_STATE_END ||
     mCallState == VoiceCallStateEnum::CALL_STATE_ENUM_MAX_ENUM_VAL)
    return FALSE;
  else
    return TRUE;
}
#endif