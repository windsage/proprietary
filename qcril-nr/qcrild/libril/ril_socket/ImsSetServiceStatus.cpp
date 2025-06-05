/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsSetServiceStatusMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

/* 176 is number of RIL_IMS_CallType (11) x
 * number of RIL_IMS_StatusType (4) x
 * number of services that can be enabled (4).
 * Services are VoLTE, WiFi-Calling, Video Telephony,
 * and UT.
 */
#define MAX_NUM_SS_INFO 176

namespace ril {
namespace socket {
namespace api {

void dispatchImsSetServiceStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_Errno err = RIL_E_NO_MEMORY; // used in fail case
  std::vector<RIL_IMS_ServiceStatusInfo> ssInfoList;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    int32_t numSsInfo = 0;
    p.read(numSsInfo);
    /* Sanity check size to prevent the vector from
     * becoming too large.
     */
    if (numSsInfo > MAX_NUM_SS_INFO) {
      QCRIL_LOG_DEBUG("numSsInfo is too large: %d", numSsInfo);
      err = RIL_E_INVALID_ARGUMENTS;
      sendFailure = true;
      break;
    }
    for (int i = 0; i < numSsInfo; i++) {
      RIL_IMS_ServiceStatusInfo ss = {};
      p.read(ss);
      ssInfoList.push_back(ss);
    }
    auto msg = std::make_shared<QcRilRequestImsSetServiceStatusMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    for (const auto& ssInfo : ssInfoList) {
      auto& callType = ssInfo.callType;
      auto& networkMode = ssInfo.accTechStatus.networkMode;
      auto& status = ssInfo.accTechStatus.status;
      if ((callType == RIL_IMS_CALL_TYPE_VOICE) &&
          (networkMode == RADIO_TECH_LTE)) {
        msg->setVolteEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if ((callType == RIL_IMS_CALL_TYPE_VOICE) &&
          ((networkMode == RADIO_TECH_IWLAN) ||
           (networkMode == RADIO_TECH_WIFI))) {
        msg->setWifiCallingEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if (callType == RIL_IMS_CALL_TYPE_VT) {
        msg->setVideoTelephonyEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if (callType == RIL_IMS_CALL_TYPE_UT) {
        msg->setUTEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsSetServiceStatus:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, err, nullptr);
  }
  for (auto &ss : ssInfoList) {
    p.release(ss);
  }

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
