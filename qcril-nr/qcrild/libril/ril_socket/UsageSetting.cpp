/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/nas/RilRequestGetUsageSettingMessage.h>
#include <interfaces/nas/RilRequestSetUsageSettingMessage.h>
#include <interfaces/nas/nas_types.h>
#include <telephony/ril.h>

#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

void dispatchSetUsageSetting(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_UsageSettingMode rilMode;
  if (p.read(rilMode) != Marshal::Result::SUCCESS) {
    sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    return;
  }

  auto msg = std::make_shared<RilRequestSetUsageSettingMessage>(context, rilMode);
  if (msg == nullptr) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
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
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchGetUsageSetting(std::shared_ptr<SocketRequestContext> context, Marshal& /*p*/) {
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetUsageSettingMessage>(context);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<qcril::interfaces::RilGetUsageSettingResult_t> rilUsageSettingResult{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilUsageSettingResult =
              std::static_pointer_cast<qcril::interfaces::RilGetUsageSettingResult_t>(resp->data);
          }
          auto p = std::make_shared<Marshal>();
          if (p) {
            p->write(rilUsageSettingResult->mode);
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}
