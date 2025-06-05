/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <telephony/ril_call.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <interfaces/voice/QcRilRequestHangupWaitingOrBackgroundMessage.h>
#include <interfaces/voice/QcRilRequestHangupForegroundResumeBackgroundMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchHangup(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    RIL_Hangup hangup{};
    Marshal::Result ret = p.read(hangup);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    auto msg = std::make_shared<QcRilRequestHangupMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    // Set parameters
    if (hangup.connIndex != INT32_MAX) {
      msg->setCallIndex(hangup.connIndex);
    }
    if (hangup.connUri != nullptr && strlen(hangup.connUri) > 0) {
      msg->setConnectionUri(hangup.connUri);
    }
    if (hangup.hasRejectCause) {
        msg->setRejectCause(static_cast<qcril::interfaces::CallFailCause>(hangup.rejectCause));
    }
    p.release(hangup);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
        {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchHangup:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchHangupWaitingOrBackground(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestHangupWaitingOrBackgroundMessage>(context);
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
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchHangupWaitingOrBackground:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchHangupForegroundResumeBackground(std::shared_ptr<SocketRequestContext> context,
                                              Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestHangupForegroundResumeBackgroundMessage>(context);
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
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchHangupForegroundResumeBackground:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
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
