/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

bool convertCallFwdTimerInfo(qcril::interfaces::CallFwdTimerInfo &out,
                             const RIL_CallFwdTimerInfo &in) {
  out.setYear(in.year);
  out.setMonth(in.month);
  out.setDay(in.day);
  out.setHour(in.hour);
  out.setMinute(in.minute);
  out.setSecond(in.second);
  out.setTimezone(in.timezone);
  return true;
}

void dispatchQueryCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(context);
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
    uint32_t reason = 0;
    Marshal::Result ret = p.read(reason);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    // Set parameters
    msg->setReason(reason);
    uint32_t serviceClass = 0;
    ret = p.read(serviceClass);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    msg->setServiceClass(serviceClass);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
        {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          RIL_QueryCallForwardStatusInfo outCallFwdInfo{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
            ril::socket::utils::convertToSocket(outCallFwdInfo, respData);
          }
          auto p = std::make_shared<Marshal>();
          if (p) {
            if (p->write(outCallFwdInfo) != Marshal::Result::SUCCESS) {
              p = nullptr;
              errorCode = RIL_E_INTERNAL_ERR;
            }
          }
          QCRIL_LOG_DEBUG("dispatchQueryCallForwardStatus:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (resultCode != RIL_E_SUCCESS) {
    sendResponse(context, resultCode, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void dispatchSetCallForward(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno resultCode = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      resultCode = RIL_E_NO_MEMORY;
      break;
    }
    RIL_CallForwardParams callForwardInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    Marshal::Result ret = p.read(callForwardInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      resultCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    msg->setStatus(callForwardInfo.status);
    msg->setReason(callForwardInfo.reason);
    msg->setServiceClass(callForwardInfo.serviceClass);
    msg->setToa(callForwardInfo.toa);
    if (callForwardInfo.number != nullptr) {
      msg->setNumber(callForwardInfo.number);
    }
    msg->setTimeSeconds(callForwardInfo.timeSeconds);
    if (callForwardInfo.callFwdTimerStart) {
      auto callFwdTimerStart = std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
      convertCallFwdTimerInfo(*callFwdTimerStart, *callForwardInfo.callFwdTimerStart);
      msg->setCallFwdTimerStart(callFwdTimerStart);
    }

    if (callForwardInfo.callFwdTimerEnd) {
      auto callFwdTimerEnd = std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
      convertCallFwdTimerInfo(*callFwdTimerEnd, *callForwardInfo.callFwdTimerEnd);
      msg->setCallFwdTimerEnd(callFwdTimerEnd);
    }
    p.release(callForwardInfo);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void
        {
          RIL_SetCallForwardStatusInfo outCallFwdInfo{};
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::SetCallForwardRespData>(resp->data);
            ril::socket::utils::convertToSocket(outCallFwdInfo, respData);
          }
          auto p = std::make_shared<Marshal>();
          if (p) {
            if (p->write(outCallFwdInfo) != Marshal::Result::SUCCESS) {
              p = nullptr;
              errorCode = RIL_E_INTERNAL_ERR;
            }
          }
          QCRIL_LOG_DEBUG("dispatchImsSetCallForwardStatus:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, p);
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
