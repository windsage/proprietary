/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/SetDataThrottlingRequestMessage.h>
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

void dispatchSetDataThrottling(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching SetDataThrottling request.");
    int32_t action = 0;
    int64_t duration = 0;
    if (p.read(action) == Marshal::Result::FAILURE || p.read(duration) == Marshal::Result::FAILURE) {
        RLOGI("Message SetDataThrottling Could not be dispatched. Marshal Read Error");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    auto msg = std::make_shared<rildata::SetDataThrottlingRequestMessage>(
        (rildata::DataThrottleAction_t)action, duration);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
        [context]  (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<RIL_Errno> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                if (rsp == nullptr) {
                    sendResponse(context, RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
                }
                else if (status == Message::Callback::Status::SUCCESS) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("SetDataThrottling RIL_Errno= %d ", *rsp);
                    sendResponse(context, errorCode, p);
                }
                else {
                    sendResponse(context, errorCode, nullptr);
                }
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

}
}
}
