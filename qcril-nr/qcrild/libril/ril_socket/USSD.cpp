/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestSendUssdMessage.h>
#include <interfaces/voice/QcRilRequestCancelUssdMessage.h>
#include <qcril_config.h>
#include <ril_utils.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSendUssd(std::shared_ptr<SocketRequestContext> context, Marshal& p, bool isIms) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_NO_MEMORY;
  do {
    if (isIms && !ril::socket::utils::isUssdOverImsSupported()) {
      QCRIL_LOG_DEBUG("USSD cs fallback");
      sendFailure = true;
      errorCode = RIL_E_USSD_CS_FALLBACK;
      break;
    }
    auto msg = std::make_shared<QcRilRequestSendUssdMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    std::string ussd;
    p.read(ussd);
    if (ussd.empty()) {
      QCRIL_LOG_ERROR("ussd is empty");
      sendFailure = true;
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    msg->setUssd(ussd);
    if(isIms) {
      msg->setIsImsRequest(true);
      msg->setIsDomainAuto(true);
    } else if(!ril::socket::utils::isUssdOverImsSupported()) {
      msg->setIsDomainAuto(true);
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
    sendResponse(context, errorCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchCancelUssd(std::shared_ptr<SocketRequestContext> context, Marshal& /*p*/) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestCancelUssdMessage>(context);
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
