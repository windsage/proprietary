/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestGetColrMessage.h>
#include <interfaces/voice/QcRilRequestSetColrMessage.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice{

void dispatchGetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    RIL_Errno resultCode = RIL_E_SUCCESS;
    auto msg = std::make_shared<QcRilRequestGetColrMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        resultCode = RIL_E_NO_MEMORY;
        return;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> p = nullptr;
            RIL_Errno errorCode = dispatchStatusToRilErrno(status);
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                auto colrInfo = std::static_pointer_cast<qcril::interfaces::ColrInfo>(resp->data);
                if (colrInfo) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        RIL_ColrInfo rilColr{};
                        ril::socket::utils::convertToSocket(rilColr, colrInfo);
                        if(p->write(rilColr) != Marshal::Result::SUCCESS) {
                          p = nullptr;
                          errorCode = RIL_E_INTERNAL_ERR;
                        }
                        ril::socket::utils::release(rilColr);
                    } else {
                      errorCode = RIL_E_NO_MEMORY;
                    }
                }
            }
            QCRIL_LOG_DEBUG("dispatchGetColr resp: errorCode=%d", errorCode);
            sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
    if (resultCode != RIL_E_SUCCESS) {
      sendResponse(context, resultCode, nullptr);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void dispatchSetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    RIL_Errno resultCode = RIL_E_SUCCESS;
    auto msg = std::make_shared<QcRilRequestSetColrMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        resultCode = RIL_E_NO_MEMORY;
        return;
    }
    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        resultCode = RIL_E_NO_MEMORY;
        return;
    }
    RIL_ColrInfo colr{};
    p.read(colr);
    if (colr.presentation != RIL_IP_PRESENTATION_INVALID) {
        msg->setPresentation(convertIpPresentation(colr.presentation));
    }
    p.release(colr);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> p = nullptr;
            RIL_Errno errorCode = dispatchStatusToRilErrno(status);
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                auto errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
                if (errorDetails) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        RIL_IMS_SipErrorInfo rilErrInfo = {};
                        ril::socket::utils::convertToSocket(rilErrInfo, *errorDetails);
                        if(p->write(rilErrInfo) != Marshal::Result::SUCCESS) {
                          p = nullptr;
                          errorCode = RIL_E_INTERNAL_ERR;
                        }
                        ril::socket::utils::release(rilErrInfo);
                    } else {
                      errorCode = RIL_E_NO_MEMORY;
                    }
                }
            }
            QCRIL_LOG_DEBUG("dispatchSetColr resp: errorCode=%d", errorCode);
            sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
    if (resultCode != RIL_E_SUCCESS) {
      sendResponse(context, resultCode, nullptr);
    }
    QCRIL_LOG_FUNC_RETURN();
}
}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
