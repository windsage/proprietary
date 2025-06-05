/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestGetClipMessage.h>
//#include <interfaces/voice/voice.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchQueryClip(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestGetClipMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
        {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p;
          RIL_ClipInfo clipStatus{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            std::shared_ptr<qcril::interfaces::ClipInfo> respData;
            errorCode = resp->errorCode;
            respData = std::static_pointer_cast<qcril::interfaces::ClipInfo>(resp->data);
            ril::socket::utils::convertToSocket(clipStatus, respData);
            p = std::make_shared<Marshal>();
            if (p) {
              if (p->write(clipStatus) != Marshal::Result::SUCCESS) {
                p = nullptr;
                errorCode = RIL_E_INTERNAL_ERR;
              }
            } else {
              errorCode = RIL_E_NO_MEMORY;
            }
            ril::socket::utils::release(clipStatus);
          }
          QCRIL_LOG_DEBUG("dispatchQueryClip:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
