/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsHoldMessage.h>
#include <interfaces/voice/QcRilRequestImsResumeMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchHold(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;

  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint32_t callId;
    Marshal::Result ret = p.read(callId);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read callId");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    auto msg = std::make_shared<QcRilRequestImsHoldMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    msg->setCallId(callId);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            std::shared_ptr<qcril::interfaces::SipErrorInfo> rilErrorInfo;
            RIL_IMS_SipErrorInfo errorDetails{};
            if (resp->data != nullptr) {
              rilErrorInfo = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
            }
            if (rilErrorInfo != nullptr) {
              ril::socket::utils::convertToSocket(errorDetails, *rilErrorInfo);
              p = std::make_shared<Marshal>();
              if (p) {
                if(p->write(errorDetails) != Marshal::Result::SUCCESS) {
                  p = nullptr;
                  errorCode = RIL_E_INTERNAL_ERR;
                }
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
              ril::socket::utils::release(errorDetails);
            }
          }
          QCRIL_LOG_DEBUG("disptachHold: resp errorCode=%d", errorCode);
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


void dispatchResume(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;

  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint32_t callId;
    Marshal::Result ret = p.read(callId);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read callId");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    auto msg = std::make_shared<QcRilRequestImsResumeMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    msg->setCallId(callId);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg */, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p;
          RIL_IMS_SipErrorInfo errorDetails{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            std::shared_ptr<qcril::interfaces::SipErrorInfo> rilErrorInfo;
            errorCode = resp->errorCode;
            if (resp->data != nullptr) {
              rilErrorInfo = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
            }
            if (rilErrorInfo != nullptr) {
              ril::socket::utils::convertToSocket(errorDetails, *rilErrorInfo);
              p = std::make_shared<Marshal>();
              if (p) {
                if(p->write(errorDetails) != Marshal::Result::SUCCESS) {
                  p = nullptr;
                  errorCode = RIL_E_INTERNAL_ERR;
                }
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
          }
          QCRIL_LOG_DEBUG("disptachResume: resp errorCode=%d", errorCode);
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
