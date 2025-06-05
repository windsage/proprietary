/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h>
#include <interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetNetworkSelectionAutomatic(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  bool sendFailure = false;

  do {
    RIL_AccessMode accessMode;
    if (p.read(accessMode) != Marshal::Result::SUCCESS) {
      sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
      return;
    }
    auto msg = std::make_shared<RilRequestSetNetworkSelectionAutoMessage>(context);
    if (msg == nullptr) {
      sendFailure = true;
      break;
    }
    msg->setAccessMode(accessMode);
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
  } while (false);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
}

void dispatchSetNetworkSelectionManual(std::shared_ptr<SocketRequestContext> context,
                                          Marshal& p) {
  bool sendFailure = false;
  RIL_SetNetworkSelectionManualParam setNetworkSelectionMode = {};
  RIL_Errno err = RIL_E_INTERNAL_ERR; // for failure case

  do {
    if (p.read(setNetworkSelectionMode) != Marshal::Result::SUCCESS) {
      sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
      return;
    }

    RIL_RadioTechnology rat = RADIO_TECH_UNKNOWN;
    switch (setNetworkSelectionMode.ran) {
      case GERAN:
        rat = RADIO_TECH_GSM;
        break;
      case UTRAN:
        rat = RADIO_TECH_UMTS;
        break;
      case EUTRAN:
        rat = RADIO_TECH_LTE;
        break;
      case NGRAN:
        rat = RADIO_TECH_5G;
        break;
      case CDMA:
        rat = RADIO_TECH_IS95A;
        break;
      case IWLAN:
        rat = RADIO_TECH_IWLAN;
        break;
      default:
        break;
    }

    // Check for nullptr before allocation
    if (setNetworkSelectionMode.operatorNumeric == nullptr) {
      sendFailure = true;
      err = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(
        context, setNetworkSelectionMode.operatorNumeric, rat);
    if (msg == nullptr) {
      sendFailure = true;
      err = RIL_E_NO_MEMORY;
      break;
    }

    RIL_AccessMode rilAccessMode = setNetworkSelectionMode.accessMode;
    if (rilAccessMode != RIL_ACCESS_MODE_INVALID) {
      msg->setAccessMode(rilAccessMode);
    }

    if (setNetworkSelectionMode.cagIdValid) {
      msg->setCagId(setNetworkSelectionMode.cagId);
    }

    if (setNetworkSelectionMode.snpnNidLength > 0 &&
        setNetworkSelectionMode.snpnNidLength < MAX_SNPN_NID_LEN) {
      std::vector<uint8_t> snpnNid(
          setNetworkSelectionMode.snpnNid,
          setNetworkSelectionMode.snpnNid + setNetworkSelectionMode.snpnNidLength);
      if (!snpnNid.empty()) {
        msg->setSnpnId(snpnNid);
      }
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
  } while (false);

  p.release(setNetworkSelectionMode);

  if (sendFailure) {
    sendResponse(context, err, nullptr);
  }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
