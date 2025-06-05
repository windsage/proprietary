/******************************************************************************
#  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/StartKeepAliveRequestMessage.h>
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

/*  bool stoi_wrapper: Wrapper for strtoll with some extra args
 *  and error handling. This shouldn't be used for types larger
 *  than long long int because it might return false due
 *  to an ERANGE errno value.
 *
 *  const std::string &`s`: Input String to convert to a numeric value
 *
 *  T `out`:                output numeric value of type T
 *
 *  size_t `maxLen`:        Maximum length of the string `s`.
 *                          This is useful if we know the number should
 *                          not exceed a specific number of digits
 *                          (e.g. 3 for an IP octet).
 *
 *  bool `keepString`:      If false, this function will fail if
 *                          the strtoll endptr is non-empty (i.e. if there were
 *                          characters after the number that was converted)
 *
 *  int `base`:             Base of number (e.g. 10 for decimal numbers)
 *
 * Returns True if the conversion succeeded and out was modified. False, otherwise.
 */
template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 1>
bool stoi_wrapper(const std::string &s, T &out, size_t maxLen, bool keepString, int base) {
    // Use long long int to perform checks and convert to smaller type if needed.
    long long int temp;
    char *endptr; // strtol output for non-numeric chracters

    /* An empty string obviously can't be converted.
     * Also reject strings larger than maxLen.
    */
    if (s.empty() || s.length() > maxLen) {
        RLOGI("stoi_wrapper: Invalid length: %zu", s.length());
        return false;
    }
    const char *str = s.c_str();

    errno = 0; // Clear errno
    int errno_cache = 0;
    /* Store strtoll ret in temp variable to avoid corrupting out
     * in a fail case.
     */
    temp = strtoll(str, &endptr, base);
    // Cache errno in case it changes due to side effects
    errno_cache = errno;
    if (temp == 0) {
        if (errno_cache == EINVAL) {
            RLOGI("stoi_wrapper: strtol conversion failed: invalid value.");
            return false;
        }
    }
    else if (errno_cache == ERANGE) {
        /* strtoll returns LLONG_MAX or LLONG_MIN when errno is ERANGE.
         * We need to check for ERANGE separately in case this function
         * was called for a type T that is bigger than long long int.
         */
        RLOGI("stoi_wrapper: strtol conversion failed: out of range.");
        return false;
    }
    // If keepString is false and there is a leftover string, return false.
    if (!keepString) {
        if (endptr && endptr[0] != '\0') {
            RLOGI("stoi_wrapper: Couldn't convert to an integer: %s", str);
            return false;
        }
    }
    // Check if temp could fit into T.
    if (temp > numeric_limits<T>::max() ||
        temp < numeric_limits<T>::min())
    {
        RLOGI("stoi_wrapper: Failed to convert: %lli", temp);
        return false;
    }
    // If temp is a valid number, convert it and assign to out.
    out = (T)temp;
    return true;
}

inline bool isValidIpOctet(const std::string &s, uint8_t &octet) {
    return stoi_wrapper(s, octet, 3, false, 10);
}

void dispatchStartKeepAlive(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching StartKeepAlive request.");
    using namespace rildata;
    int32_t serial = 0;
    RIL_KeepaliveRequest request = {};
    p.read(request);
    rildata::KeepaliveRequest_t data = {};
    data.type = static_cast<KeepaliveType_t>(request.type);
    int i = 0;
    string s = "";
    uint8_t current_byte = 0;
    while(i < MAX_INADDR_LEN && request.sourceAddress[i] != '\0') {
        if (request.sourceAddress[i] == '.' || request.sourceAddress[i] == ':') {
            i++;
            if (isValidIpOctet(s, current_byte)) {
                data.sourceAddress.push_back(current_byte);
            }
            else {
                RLOGI("Source Address::Only IPV4 address is supported");
                sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
                return;
            }
            s = "";
            continue;
        }
        s += request.sourceAddress[i];
        i++;
    }
    /* Push back the trailing octet at the end of the IP address string.
     * The above loop will only push octets that are followed by '.' or ':'.
     * An additional push_back is needed to push the final octet to the vector.
     * For example if the IP address is "123.34.0.255", the loop would push
     * 123, 34, and 0 to the vector, as they are followed by a '.' in the string.
     * We need another push_back to append to 255 to our address vector,
     * as it is not followed by '.' or ':'.
     */
    if (isValidIpOctet(s, current_byte)) {
        data.sourceAddress.push_back(current_byte);
    }
    else {
        RLOGI("Source Address::Only IPV4 address is supported");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    s = "";
    data.sourcePort = request.sourcePort;
    i = 0;
    while(i < MAX_INADDR_LEN && request.destinationAddress[i] != '\0') {
        if (request.destinationAddress[i] == '.' || request.destinationAddress[i] == ':') {
            i++;
            if (isValidIpOctet(s, current_byte)) {
                data.destinationAddress.push_back(current_byte);
            }
            else {
                RLOGI("Destination Address::Only IPV4 address is supported");
                sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
                return;
            }
            s = "";
            continue;
        }
        s += request.destinationAddress[i];
        i++;
    }
    /* Push back the trailing octet at the end of the IP address string.
     * The above loop will only push octets that are followed by '.' or ':'.
     * An additional push_back is needed to push the final octet to the vector.
     * For example if the IP address is "123.34.0.255", the loop would push
     * 123, 34, and 0 to the vector, as they are followed by a '.' in the string.
     * We need another push_back to append to 255 to our address vector,
     * as it is not followed by '.' or ':'.
     */
    if (isValidIpOctet(s, current_byte)) {
        data.destinationAddress.push_back(current_byte);
    }
    else {
        RLOGI("Destination Address::Only IPV4 address is supported");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
            return;
    }
    data.destinationPort = request.destinationPort;
    data.maxKeepaliveIntervalMillis = request.maxKeepaliveIntervalMillis;
    data.cid = request.cid;
    p.release(request);
    auto msg =
        std::make_shared<StartKeepAliveRequestMessage>(
            serial,
            data,
            nullptr);
        if(msg) {
        GenericCallback<StartKeepAliveResp_t> cb(
            [context]  (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
            std::shared_ptr<StartKeepAliveResp_t> responseDataPtr) -> void {
            if (status == Message::Callback::Status:: NO_HANDLER_FOUND) {
                sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
                return;
            }
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                int val = -1;
                if (responseDataPtr == nullptr) {
                    sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
                }
                else if (status == Message::Callback::Status::SUCCESS) {
                    RIL_Errno errorCode = RIL_Errno::RIL_E_SUCCESS;
                    val = static_cast<int>(responseDataPtr->error);
                    p->write(val);
                    val = static_cast<int>(responseDataPtr->handle);
                    p->write(val);
                    val = static_cast<int>(responseDataPtr->status);
                    p->write(val);
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
