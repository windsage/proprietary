/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"

#include <limits>

#include <framework/GenericCallback.h>
#include <ril_socket_api.h>
#include "ril_utils.h"

#include <request/GetSlicingConfigRequestMessage.h>
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>

#ifndef RIL_FOR_MDM_LE
#include <utils/Log.h>
#else
#include <utils/Log2.h>
#endif

namespace ril {
namespace socket {
namespace api {

void dispatchGetSlicingConfigRequest(std::shared_ptr<SocketRequestContext> context, Marshal &/*p*/) {
    using namespace rildata;
    static int serial = 0;
    RLOGI("Dispatching dispatchGetSlicingConfigRequest");
    auto msg = std::make_shared<GetSlicingConfigRequestMessage>(serial++, nullptr);
    GenericCallback<GetSlicingConfigResp_t> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<GetSlicingConfigResp_t> responseDataPtr) -> void {
        if (status == Message::Callback::Status:: NO_HANDLER_FOUND) {
            sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
            return;
        }
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchGetSlicingConfig Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (msg && responseDataPtr) {
                rilSlicingConfig slicingConfig = {};
                if ((status == Message::Callback::Status::SUCCESS) && (responseDataPtr->respErr == ResponseError_t::NO_ERROR)) {
                    ril::socket::utils::convertToSocket(slicingConfig, responseDataPtr->slicingConfig);
                    p->write(slicingConfig);
                    RLOGI("GetSlicingConfig wrote %d bytes", p->dataSize());
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                } else {
                    switch (responseDataPtr->respErr) {
                        RLOGI("dispatchGetDataCallList  return status NOT Success");
                        case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                        case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                        default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;
                    }
                }
                sendResponse(context, errorCode, p);
                p->release(slicingConfig);
            } else {
                RLOGI("dispatchGetSlicingConfig  response ptr is NULL");
                sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
            }
        }
    });
    if (msg) {
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

}
}
}