/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <Marshal.h>
#include <marshal/CallInfo.h>
#include <interfaces/voice/QcRilRequestGetCurrentCallsMessage.h>
#include <ril_utils.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchGetCurrentCalls(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  RIL_Errno resultCode = RIL_E_SUCCESS;
  QCRIL_LOG_FUNC_ENTRY();
  do {
    auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    msg->setDomain(qcril::interfaces::CallDomain::AUTOMATIC);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
        {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p;
          std::vector<RIL_Call> calls;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto currentCalls =
                std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
            if (currentCalls) {
              auto& callList = currentCalls->getCallInfoListRef();
              uint32_t sz = callList.size();
              p = std::make_shared<Marshal>();
              if (p) {
                p->write(sz);
                for (auto& callInfo : callList) {
                  RIL_CallInfo rilCall{};
                  ril::socket::utils::convertToSocket(rilCall, callInfo);
                  p->write(rilCall);
                  ril::socket::utils::release(rilCall);
                }
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
          }
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
