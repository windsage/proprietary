/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsSetUiTtyModeMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

void dispatchSendUiTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    RIL_TtyNotifyInfo ttyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    Marshal::Result ret = p.read(ttyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsSetUiTtyModeMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    // Set parameters
    if (ttyInfo.mode != RIL_TtyModeType::RIL_TTY_MODE_INVALID) {
      msg->setTtyMode(convertTtyMode(ttyInfo.mode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchSendUiTtyMode:resp: errorCode=%d", errorCode);
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
