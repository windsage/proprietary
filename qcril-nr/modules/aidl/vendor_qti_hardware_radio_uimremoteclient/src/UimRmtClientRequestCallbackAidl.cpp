/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/uim_remote/UimRmtQmiUimRemoteReqMsg.h"
#include "framework/Log.h"
#include "UimRmtQmiUimRemoteRespMsg.h"
#include "UimRmtClientRequestCallbackAidl.h"

#define TAG "UimRmtClientRequestCallbackAidl"

/*===========================================================================
  FUNCTION  onResponse - response function
===========================================================================*/
void UimRmtClientRequestCallbackAidl::onResponse(
    std::shared_ptr<Message> msg_ptr,
    Message::Callback::Status status,
    std::shared_ptr<qmi_uim_remote_resp_msg> responseDataPtr)
{
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg> req_msg_ptr =
      std::static_pointer_cast<UimRmtQmiUimRemoteReqMsg>(msg_ptr);
  std::shared_ptr<UimRmtQmiUimRemoteRespMsg> resp_msg_ptr = nullptr;
  int32_t token = -1;
  qmi_uim_remote_req_type req_type = QMI_UIM_REMOTE_INVALID_REQUEST;

  (void)status;

  Log::getInstance().d("[UimRmtClientRequestCallbackAidl]: Callback executed. client data = " +
                       mClientToken);

  if (req_msg_ptr != nullptr) {
    std::shared_ptr<qmi_uim_remote_request_msg> req_data_ptr = req_msg_ptr->get_message();

    if (req_data_ptr != nullptr) {
      token = req_data_ptr->token;
      req_type = req_data_ptr->req_id;
    }
  }

  Log::getInstance().d("[UimRmtClientRequestCallbackAidl]: Callback executed. token" +
                       std::to_string(token) + "req_type:" + std::to_string(req_type));

  resp_msg_ptr = std::make_shared<UimRmtQmiUimRemoteRespMsg>(token, req_type, responseDataPtr);

  if (resp_msg_ptr != nullptr) {
    resp_msg_ptr->dispatch();
  }
} /* UimRmtClientRequestCallbackAidl::onResponse */

/*===========================================================================
  FUNCTION  clone - clone function
===========================================================================*/
Message::Callback* UimRmtClientRequestCallbackAidl::clone() const
{
  UimRmtClientRequestCallbackAidl* clone = new UimRmtClientRequestCallbackAidl(mClientToken);
  return clone;
} /* UimRmtClientRequestCallbackAidl::clone */