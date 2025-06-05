/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/ims/QcRilRequestImsSendRttMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchSendRttMsg(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  std::string rttMessage;
  RIL_Errno resultCode = RIL_E_SUCCESS;

  if (p.read(rttMessage) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Send Rtt Message is empty");
    resultCode = RIL_E_INTERNAL_ERR;
    QCRIL_LOG_FUNC_RETURN();
    return;
  }

  auto msg = std::make_shared<QcRilRequestImsSendRttMessage>(context,
            rttMessage);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    resultCode = RIL_E_NO_MEMORY;
    QCRIL_LOG_FUNC_RETURN();
    return;
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        RIL_Errno errorCode = dispatchStatusToRilErrno(status);
        if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
          errorCode = resp->errorCode;
        }
        sendResponse(context, errorCode, nullptr);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
