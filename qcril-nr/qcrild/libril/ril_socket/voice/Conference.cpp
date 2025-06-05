/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestConferenceMessage.h>
#include <Marshal.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchConference(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestConferenceMessage>(context);
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

          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            std::shared_ptr<qcril::interfaces::SipErrorInfo> rilErrorInfo =
                std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
            if (rilErrorInfo != nullptr) {
              RIL_IMS_SipErrorInfo errorDetails{};
              ril::socket::utils::convertToSocket(errorDetails, *rilErrorInfo);
              p = std::make_shared<Marshal>();
              if (p) {
                if (p->write(errorDetails) != Marshal::Result::SUCCESS) {
                  p = nullptr;
                  errorCode = RIL_E_INTERNAL_ERR;
                }
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
              ril::socket::utils::release(errorDetails);
            }
          }
          QCRIL_LOG_DEBUG("dispatchConference: resp errorCode=%d", errorCode);
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
