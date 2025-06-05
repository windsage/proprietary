/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/dms/RilRequestRadioPowerMessage.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchRadioPower(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("radio power state is empty");
      sendFailure = true;
      break;
    }

    RIL_RadioPowerReq req {};
    if (p.read(req) != Marshal::Result::SUCCESS) {
        sendFailure = false;
        errorCode = RIL_E_INTERNAL_ERR;
        break;
    }

    auto msg = std::make_shared<RilRequestRadioPowerMessage>(context, !!req.state,
      !!req.forEmergencyCall, !!req.preferredForEmergencyCall);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
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
  } while (FALSE);
  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
