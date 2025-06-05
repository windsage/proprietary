/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestAnswerMessage.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsAnswer(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestAnswerMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
    }

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendFailure = true;
        break;
    }
    RIL_IMS_Answer answer{};
    p.read<RIL_IMS_Answer>(answer);

    // Set parameters
    msg->setCallType(convertCallType(answer.callType));
    if (answer.presentation != RIL_IMS_IP_PRESENTATION_INVALID) {
        msg->setPresentation(convertIpPresentation(answer.presentation));
    }
    msg->setRttMode(convertRttMode(answer.rttMode));

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsAnswer resp: errorCode=%d", errorCode);
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
