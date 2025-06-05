/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <limits>
#include <telephony/ril_call.h>
#include <Marshal.h>
#include <ril_socket_api.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchSetCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  RIL_CallWaitingSettings cwSettings{};
  if (p.read(cwSettings) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
    sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    return;
  }
  auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(context);
  if (!msg) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    return;
  }
  msg->setServiceStatus(cwSettings.enabled ? qcril::interfaces::ServiceClassStatus::ENABLED
                                           : qcril::interfaces::ServiceClassStatus::DISABLED);
  if (cwSettings.serviceClass != std::numeric_limits<uint32_t>::max()) {
    msg->setServiceClass(cwSettings.serviceClass);
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [context](std::shared_ptr<Message> /*msg*/,
                Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
      {
        std::shared_ptr<Marshal> p;
        RIL_Errno errorCode = dispatchStatusToRilErrno(status);

        if (status == Message::Callback::Status::SUCCESS && resp) {
          errorCode = resp->errorCode;
          auto sipErrorInfo = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          RIL_IMS_SipErrorInfo sipError{ .errorCode = std::numeric_limits<uint32_t>::max(),
                                         .errorString = nullptr };

          if (sipErrorInfo) {
            if (sipErrorInfo->hasErrorCode()) {
              sipError.errorCode = sipErrorInfo->getErrorCode();
            }

            std::string errorString;
            if (sipErrorInfo->hasErrorString()) {
              errorString = sipErrorInfo->getErrorString();
              sipError.errorString = const_cast<char*>(errorString.c_str());
            }
            p = std::make_shared<Marshal>();
            if (p) {
              if (p->write(sipError) != Marshal::Result::SUCCESS) {
                p = nullptr;
                errorCode = RIL_E_INTERNAL_ERR;
              }
            }
          }
        }
        QCRIL_LOG_DEBUG("dispatchSetCallWaiting:resp: errorCode=%d", errorCode);
        sendResponse(context, errorCode, p);
      });
  msg->setCallback(&cb);
  msg->dispatch();
}

void dispatchQueryCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  uint32_t serviceClass = std::numeric_limits<uint32_t>::max();
  if (p.read(serviceClass) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
    sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    return;
  }
  auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(context);
  if (!msg) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    return;
  }
  if (serviceClass != std::numeric_limits<uint32_t>::max()) {
    msg->setServiceClass(serviceClass);
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [context](std::shared_ptr<Message> /*msg*/,
                Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
      {
        std::shared_ptr<Marshal> p;
        RIL_Errno errorCode = dispatchStatusToRilErrno(status);
        if (status == Message::Callback::Status::SUCCESS && resp) {
          errorCode = resp->errorCode;
          auto cwInfo = std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);
          if (cwInfo) {
            RIL_QueryCallWaitingResponse queryCwResp{};
            queryCwResp.callWaitingSettings.enabled =
                cwInfo->hasStatus() &&
                cwInfo->getStatus() == qcril::interfaces::ServiceClassStatus::ENABLED;
            queryCwResp.callWaitingSettings.serviceClass = std::numeric_limits<uint32_t>::max();
            if (cwInfo->hasServiceClass()) {
              queryCwResp.callWaitingSettings.serviceClass = cwInfo->getServiceClass();
            }
            std::string sipErrorString;
            queryCwResp.sipError.errorCode = std::numeric_limits<uint32_t>::max();
            queryCwResp.sipError.errorString = nullptr;
            if (cwInfo->hasErrorDetails()) {
              std::shared_ptr<qcril::interfaces::SipErrorInfo> sipErrorInfo =
                  cwInfo->getErrorDetails();
              if (sipErrorInfo) {
                if (sipErrorInfo->hasErrorCode()) {
                  queryCwResp.sipError.errorCode = sipErrorInfo->getErrorCode();
                }
                if (sipErrorInfo->hasErrorString()) {
                  sipErrorString = sipErrorInfo->getErrorString();
                  queryCwResp.sipError.errorString = const_cast<char*>(sipErrorString.c_str());
                }
              }
            }
            p = std::make_shared<Marshal>();
            if (!p || p->write(queryCwResp) != Marshal::Result::SUCCESS) {
              p = nullptr;
              errorCode = RIL_E_INTERNAL_ERR;
            }
          }
        }
        QCRIL_LOG_DEBUG("dispatchQueryCallWaiting:resp: errorCode=%d", errorCode);
        sendResponse(context, errorCode, p);
      });
  msg->setCallback(&cb);
  msg->dispatch();
}

}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
