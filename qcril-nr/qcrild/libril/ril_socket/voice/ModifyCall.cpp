/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsModifyCallConfirmMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallInitiateMessage.h>
#include <interfaces/voice/QcRilRequestImsCancelModifyCallMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchModifyCallInitiate(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    RIL_CallModifyInfo modifyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    Marshal::Result ret = p.read(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsModifyCallInitiateMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    // Set parameters
    msg->setCallId(modifyInfo.callId);
    msg->setCallType(convertCallType(modifyInfo.callType));
    msg->setCallDomain(convertCallDomain(modifyInfo.callDomain));
    if (modifyInfo.hasRttMode) {
      msg->setRttMode(convertRttMode(modifyInfo.rttMode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchModifyCallInitiate:resp: errorCode=%d", errorCode);
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

void dispatchModifyCallConfirm(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    RIL_CallModifyInfo modifyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    Marshal::Result ret = p.read(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsModifyCallConfirmMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    // Set parameters
    msg->setCallId(modifyInfo.callId);
    msg->setCallType(convertCallType(modifyInfo.callType));
    msg->setCallDomain(convertCallDomain(modifyInfo.callDomain));
    if (modifyInfo.hasRttMode) {
      msg->setRttMode(convertRttMode(modifyInfo.rttMode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchModifyCallConfirm:resp: errorCode=%d", errorCode);
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

void dispatchCancelModifyCall(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    uint32_t callId = 0;
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    Marshal::Result ret = p.read(callId);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsCancelModifyCallMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    // Set parameters
    msg->setCallId(callId);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchCancelModifyCall:resp: errorCode=%d", errorCode);
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
