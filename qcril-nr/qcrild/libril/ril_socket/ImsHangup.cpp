/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsHangup(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestHangupMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
    }

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendFailure = true;
        break;
    }
    RIL_IMS_Hangup hangup{};
    p.read<RIL_IMS_Hangup>(hangup);

    // Set parameters
    if (hangup.connIndex != INT32_MAX) {
        msg->setCallIndex(hangup.connIndex);
    }
    if (hangup.hasMultiParty) {
        msg->setIsMultiParty(hangup.multiParty);
    }

    if (hangup.connUri != nullptr && strlen(hangup.connUri) > 0) {
        msg->setConnectionUri(hangup.connUri);
    }
    if (hangup.conf_id != INT32_MAX) {
        msg->setConferenceId(hangup.conf_id);
    }
    if (hangup.hasFailCauseResponse) {
        msg->setRejectCause(static_cast<qcril::interfaces::CallFailCause>(
                hangup.failCauseResponse.failCause));
        if (hangup.failCauseResponse.errorInfo && hangup.failCauseResponse.errorInfoLen > 0) {
          char *end = nullptr;
          uint32_t rejectionCause = strtoul((char*)hangup.failCauseResponse.errorInfo, &end, 0);
          int errsv = errno;
          if ((errsv  == ERANGE) && (rejectionCause == ULONG_MAX))
          {
            QCRIL_LOG_ERROR("Failed to convert rejectionCause");
            sendFailure = true;
            break;
          }
          msg->setRejectCauseRaw(rejectionCause);
        }
    }
    p.release(hangup);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsAnswer resp: errorCode=%d", errorCode);
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
