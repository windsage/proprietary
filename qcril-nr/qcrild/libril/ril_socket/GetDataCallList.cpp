/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"

#include <limits>

#include <framework/GenericCallback.h>
#include <ril_socket_api.h>
#include "ril_utils.h"

#include <request/GetRadioDataCallListRequestMessage.h>
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

void dispatchGetDataCallList(std::shared_ptr<SocketRequestContext> context, Marshal& /*p*/) {
    using namespace rildata;
    RLOGI("Dispatching dispatchGetDataCallList request.");
    auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();

    GenericCallback<DataCallListResult_t> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
        if (status == Message::Callback::Status:: NO_HANDLER_FOUND) {
            sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
            return;
        }
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchGetDataCallList Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (msg && responseDataPtr) {
                if ((status == Message::Callback::Status::SUCCESS) && (responseDataPtr->respErr == ResponseError_t::NO_ERROR)) {
                    QCRIL_LOG_DEBUG("Call list size = %d", responseDataPtr->call.size());

                    if (responseDataPtr->call.size() > std::numeric_limits<int32_t>::max()) {
                        QCRIL_LOG_DEBUG("Call list size exceeds maximum allowed.");
                        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, p);
                        return;
                    }

                    int32_t dataCallListLen = static_cast<int32_t>(responseDataPtr->call.size());
                    RLOGI("GetDataCallList write length into marshalled object");
                    p->write(dataCallListLen);
                    RIL_Data_Call_Response_v11 dataCallList = {};
                    int i = 0;
                    for (rildata::DataCallResult_t entry: responseDataPtr->call) {
                        dataCallList = {};
                        RLOGI("dispatchGetDataCallList retrieving & copying call data");
                        std::stringstream ss;
                        entry.dump("", ss);
                        RLOGI("%s", ss.str().c_str());
                        ril::socket::utils::convertToSocket(dataCallList, entry);
                        p->write(dataCallList);
                        RLOGI("GetDataCallList wrote list item %d to socket", i);
                        p->release(dataCallList);
                        i++;
                    }
                    RLOGI("GetDataCallList wrote %d bytes", (int)p->dataSize());
                } else {
                    switch (responseDataPtr->respErr) {
                        RLOGI("dispatchGetDataCallList  return status NOT Success");
                        case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                        case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                        case ResponseError_t::CALL_NOT_AVAILABLE: errorCode = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                        default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;
                    }
                }
                sendResponse(context, errorCode, p);
            } else {
                RLOGI("dispatchGetDataCallList  response ptr is NULL");
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
