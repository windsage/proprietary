/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimLpaReqMessage.h>
#include <interfaces/uim/qcril_uim_types.h>
#include "interfaces/lpa/lpa_service_types.h"
#include "interfaces/uim/qcril_uim_lpa.h"
#include "interfaces/lpa/LpaUimHttpRequestMsg.h"
#include "modules/lpa/LpaUimHttpSrvc.h"
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

RIL_Errno socket_to_internal(UimLpaHttpTrnsReq& in ,lpa_service_http_transaction_req_type& out)
{
    int i=0;
    out.tokenId = in.tokenId;
    out.result = (lpa_service_result_type)in.result;
    if(in.payload) {
       out.payload = in.payload;
    }
    else {
      out.payload = (uint8_t *)"";
    }
    out.payload_len = in.payload_len;
    if (in.no_of_headers > UIM_HTTP_CUST_HEADER_MAX_COUNT) {
        QCRIL_LOG_DEBUG("Invalid number of custom headers: %zu", in.no_of_headers);
        return RIL_E_INVALID_ARGUMENTS;
    }
    out.customHeaders = new lpa_service_http_custom_header_req_type[in.no_of_headers];
    if (out.customHeaders == nullptr) {
        QCRIL_LOG_DEBUG("Allocation for custom headers array failed.");
        return RIL_E_NO_MEMORY;
    }
    for(i = 0;i<in.no_of_headers;i++) {
       out.customHeaders[i].headerName = in.headers[i].headerName;
       out.customHeaders[i].headerValue = in.headers[i].headerValue;
    }
    out.no_of_headers = in.no_of_headers;
    return RIL_E_SUCCESS;
}

// TODO - Find the right UIM module message not found in ril_service.cpp
void sendlpahttprequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    UimLpaHttpTrnsReq params;
    lpa_service_http_transaction_req_type  req_ptr;
    if(p.read(params) == Marshal::Result::SUCCESS) {
        int token = 0;
        RIL_Errno ret = socket_to_internal(params,req_ptr);
        if (ret != RIL_E_SUCCESS) {
            p.release(params);
            sendResponse(context, ret, nullptr);
            return;
        }
        QCRIL_LOG_INFO("Dispatching Sim lpauser request.%d",req_ptr.tokenId);
        std::shared_ptr<LpaUimHttpRequestMsg>   http_req_ptr      = nullptr;
        GenericCallback<lpa_qmi_uim_http_rsp_data_type> cb(
             [context] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
             std::shared_ptr<lpa_qmi_uim_http_rsp_data_type> resp) -> void {
                 QCRIL_LOG_INFO("Sim lpauser request response.");
                 RIL_Errno  errorCode =RIL_E_INTERNAL_ERR;
                 int32_t token;
                 std::shared_ptr<Marshal> p = nullptr;
                 if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                     p = std::make_shared<Marshal>();
                     if (p) {
                         errorCode = static_cast<RIL_Errno>(resp->qmi_error_code);
                         token = resp->token;
                         p->write(token);
                     }
                 }
                 sendResponse(context, errorCode, p);
             });
        http_req_ptr = std::make_shared<LpaUimHttpRequestMsg>(&req_ptr,token,
                                 LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED,&cb);
        if(http_req_ptr != nullptr) {
            http_req_ptr->setCallback(&cb);
            http_req_ptr->dispatch();
        }
        else {
            sendResponse(context,RIL_E_NO_MEMORY,nullptr);
        }
        p.release(params);
    }
    else {
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    }
}
}  // namespace api
}  // namespace socket
}  // namespace ril
