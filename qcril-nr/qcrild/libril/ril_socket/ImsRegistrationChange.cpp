/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsRegistrationChangeMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsRegistrationChange(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    RIL_IMS_RegistrationState regState;
    p.read(regState);
    auto msg = std::make_shared<QcRilRequestImsRegistrationChangeMessage>(
        context, convertRegState(regState));
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
          QCRIL_LOG_DEBUG("dispatchImsRegistrationChange:resp: errorCode=%d", errorCode);
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
