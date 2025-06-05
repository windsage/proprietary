/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsQueryMultiSimVoiceCapability.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsQueryMultiSimVoiceCapability(std::shared_ptr<SocketRequestContext> context,
                                             Marshal& /*p*/) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsQueryMultiSimVoiceCapability>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          RIL_IMS_MultiSimVoiceCapability voiceCapability =
              RIL_IMS_MultiSimVoiceCapability::RIL_IMS_NONE;
          std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData> respData = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData = std::static_pointer_cast<qcril::interfaces::MultiSimVoiceCapabilityRespData>(
                resp->data);
            if (respData) {
              voiceCapability =
                  ril::socket::utils::convertMultiSimVoiceCapability(respData->getVoiceCapability());
            }
          }

          auto p = std::make_shared<Marshal>();
          if (p) {
            if (p->write(static_cast<uint8_t>(voiceCapability)) != Marshal::Result::SUCCESS) {
              p = nullptr;
              errorCode = RIL_E_INTERNAL_ERR;
            }
          } else {
            errorCode = RIL_E_NO_MEMORY;
          }
          QCRIL_LOG_DEBUG("dispatchImsQueryMultiSimVoiceCapability:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, p);
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
