/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/RilRequestGetSysSelChannelsMessage.h>
#include <interfaces/nas/RilRequestSetSysSelChannelsMessage.h>
#include <interfaces/nas/nas_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

void dispatchSetSystemSelectionChannels(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  RIL_Errno err = RIL_E_GENERIC_FAILURE;
  bool sendFailure = false;

  do {
    RIL_SysSelChannels info {};
    p.read(info);
    auto msg = std::make_shared<RilRequestSetSysSelChannelsMessage>(context, info);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      err = RIL_E_NO_MEMORY;
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
    sendResponse(context, err, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void dispatchGetSystemSelectionChannels(std::shared_ptr<SocketRequestContext> context, Marshal& /*p*/) {
  RIL_Errno err = RIL_E_GENERIC_FAILURE;
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetSysSelChannelsMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      err = RIL_E_NO_MEMORY;
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<qcril::interfaces::RilGetSysSelResult_t> result {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            result = std::static_pointer_cast<qcril::interfaces::RilGetSysSelResult_t>(resp->data);
          }
          auto p = std::make_shared<Marshal>();
          p->write(result->respData);
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, err, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}


}  // namespace ril::socket::api
