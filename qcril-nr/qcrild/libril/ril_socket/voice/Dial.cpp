/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <ril_socket_api.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

static std::string getExtra(std::string key, char** extras, uint32_t length)
{
  if (key.empty()) return "";
  if (!extras) return "";

  std::string value;
  for (size_t i = 0; i < length; i++) {
    if (extras[i] == nullptr) continue;
    std::string extra = extras[i];
    if (extra.find(key) != std::string::npos && extra.find("=") != std::string::npos) {
      value = extra.substr(extra.find("=") + 1);
    }
  }
  return value;
}

void dispatchDial(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  RIL_DialParams dial{};
  do {
    auto msg = std::make_shared<QcRilRequestDialMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }

    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    Marshal::Result result = p.read<RIL_DialParams>(dial);
    if (result != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
      resultCode = RIL_E_INTERNAL_ERR;
      break;
    }

    // Set parameters
    if (dial.address != nullptr) {
      msg->setAddress(dial.address);
    } else if (!dial.hasIsConferenceUri || !dial.isConferenceUri) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    if (dial.clirMode != INT32_MAX) {
      msg->setClir(dial.clirMode);
    }
    if (dial.hasCallDetails) {
      msg->setCallType(convertCallType(dial.callDetails.callType));
      msg->setCallDomain(convertCallDomain(dial.callDetails.callDomain));
      msg->setRttMode(convertRttMode(dial.callDetails.rttMode));

      if (dial.callDetails.extrasLength) {
        std::string displayText =
            getExtra("DisplayText", dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!displayText.empty()) {
          msg->setDisplayText(displayText);
        }
        std::string retryCallFailReason =
            getExtra("RetryCallFailReason", dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!retryCallFailReason.empty()) {
          msg->setRetryCallFailReason(
              static_cast<qcril::interfaces::CallFailCause>(std::stoi(retryCallFailReason)));
        }
        std::string retryCallFailMode = getExtra(
            "RetryCallFailRadioTech", dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!retryCallFailMode.empty()) {
          msg->setRetryCallFailMode(static_cast<RIL_RadioTechnology>(std::stoi(retryCallFailMode)));
        }
      }
    }

    if (dial.hasIsConferenceUri) {
      msg->setIsConferenceUri(dial.isConferenceUri);
    }
    if (dial.hasIsCallPull) {
      msg->setIsCallPull(dial.isCallPull);
    }
    if (dial.hasIsEncrypted) {
      msg->setIsEncrypted(dial.isEncrypted);
    }
    if (dial.multiLineInfo.msisdn != nullptr) {
      msg->setOriginatingNumber(dial.multiLineInfo.msisdn);
      msg->setIsSecondary((dial.multiLineInfo.lineType == RIL_LINE_TYPE_SECONDARY));
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
          QCRIL_LOG_DEBUG("dispatchDial resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }
  p.release(dial);

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
