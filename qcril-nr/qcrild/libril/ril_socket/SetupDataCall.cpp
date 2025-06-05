/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>
#include "ril_utils.h"

#include <request/SetupDataCallRequestMessage.h>
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

void dispatchSetupDataCall(std::shared_ptr<SocketRequestContext> context, Marshal& parcel) {
    RLOGI("Dispatching SetUpDataCall request.");
    using namespace rildata;
    int32_t serial =0;
    RIL_SetUpDataCallParams callParams;
    memset(&callParams, 0, sizeof(RIL_SetUpDataCallParams));
    if (parcel.read(callParams) != Marshal::Result::SUCCESS)
    {
        RLOGI("SetupDataCallRequestMessage could not be dispatched. Marshal Read Failed.");
        return;
    }
    AccessNetwork_t accnet = AccessNetwork_t::UNKNOWN;
    switch(callParams.accessNetwork) {
        case RIL_RadioAccessNetworks::GERAN:
            accnet = AccessNetwork_t::GERAN;
            break;
        case RIL_RadioAccessNetworks::UTRAN:
            accnet = AccessNetwork_t::UTRAN;
            break;
        case RIL_RadioAccessNetworks::NGRAN:
            accnet = AccessNetwork_t::NGRAN;
            break;
        case RIL_RadioAccessNetworks::CDMA:
            accnet = AccessNetwork_t::CDMA;
            break;
        case RIL_RadioAccessNetworks::EUTRAN:
            accnet = AccessNetwork_t::EUTRAN;
            break;
        default:
            accnet = AccessNetwork_t::UNKNOWN;
    }
    RIL_DataProfileInfo dataProfileInfo = callParams.profileInfo;
    bool roamingAllowed = static_cast<bool>(callParams.roamingAllowed);
    RIL_RadioDataRequestReasons reason = callParams.reason;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;
    int i = 0;
    string s = "";
    string apn = "";
    string protocol = "";
    string roamingProtocol = "";
    string user = "";
    string password = "";
    if (dataProfileInfo. apn) {
        apn = std::string(dataProfileInfo.apn);
    }
    if (dataProfileInfo.protocol) {
        protocol = std::string(dataProfileInfo.protocol);
    }
    if (dataProfileInfo.roamingProtocol) {
        roamingProtocol = std::string(dataProfileInfo.roamingProtocol);
    }
    if (dataProfileInfo.user) {
        user = std::string(dataProfileInfo.user);
    }
    if (dataProfileInfo.password) {
        password = std::string(dataProfileInfo.password);
    }
    if (callParams.addresses) {
        while(callParams.addresses[i] != '\0') {
            if (callParams.addresses[i] != ',') {
                s += callParams.addresses[i];
            }
            else {
                radioAddresses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioAddresses.push_back(s);
        }
    }
    i = 0;
    s = "";
    if (callParams.dnses) {
        while(callParams.dnses[i] != '\0') {
            if (callParams.dnses[i] != ',') {
                s += callParams.dnses[i];
            }
            else {
                radioDnses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioDnses.push_back(s);
        }
    }
    DataProfileInfo_t dataProfile = {
        .profileId = (DataProfileId_t)dataProfileInfo.profileId,
        .apn = apn,
        .protocol = protocol,
        .roamingProtocol = roamingProtocol,
        .authType = (ApnAuthType_t)dataProfileInfo.authType,
        .username = user,
        .password = password,
        .dataProfileInfoType = (DataProfileInfoType_t)dataProfileInfo.type,
        .maxConnsTime = dataProfileInfo.maxConnsTime,
        .maxConns = dataProfileInfo.maxConns,
        .waitTime = dataProfileInfo.waitTime,
        .enableProfile = (bool)dataProfileInfo.enabled,
        .supportedApnTypesBitmap = (ApnTypes_t)(dataProfileInfo.supportedTypesBitmask),
        .bearerBitmap = (RadioAccessFamily_t)(dataProfileInfo.bearerBitmask),
        .mtu = dataProfileInfo.mtu,
        .preferred = (bool)dataProfileInfo.preferred,
        .persistent = (bool)dataProfileInfo.persistent,
    };
    auto msg =
        std::make_shared<SetupDataCallRequestMessage>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        dataProfile,
        roamingAllowed,
        (DataRequestReason_t)reason,
        radioAddresses,
        radioDnses,
        nullptr);
    RLOGI("Dispatching dispatchSetupDataCall request.");
    if ( msg ) {
        GenericCallback<SetupDataCallResponse_t> cb([context](std::shared_ptr<Message> msg,
            Message::Callback::Status status,
            std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
        if (status == Message::Callback::Status:: NO_HANDLER_FOUND) {
            sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
            return;
        }
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchSetupDataCall Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            RIL_Data_Call_Response_v11 dcResult = {};
            if ( msg && (status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {

                RLOGI("setup data call cb invoked status %d ", status);
                RLOGI("cause = %d", rsp->call.cause);
                RLOGI("suggestedRetryTime = %lld", rsp->call.suggestedRetryTime);
                RLOGI("cid = %d", rsp->call.cid);
                RLOGI("active = %d", rsp->call.active);
                RLOGI("type = %s", rsp->call.type.c_str());
                RLOGI("ifname = %s", rsp->call.ifname.c_str());
                RLOGI("addresses = %s", rsp->call.addresses.c_str());
                RLOGI("dnses = %s", rsp->call.dnses.c_str());
                RLOGI("gateways = %s", rsp->call.gateways.c_str());
                RLOGI("pcscf = %s", rsp->call.pcscf.c_str());
                RLOGI("mtu = %d", rsp->call.mtu);
                errorCode = static_cast<RIL_Errno>(rsp->respErr);
                dcResult.status = static_cast<int>(rsp->call.cause);
                dcResult.suggestedRetryTime = rsp->call.suggestedRetryTime;
                dcResult.cid = rsp->call.cid;
                dcResult.active = (int)rsp->call.active;

                dcResult.type = new char[ (rsp->call.type.length())+1]{};
                if (dcResult.type) {
                    strlcpy(dcResult.type, rsp->call.type.c_str(), (rsp->call.type.length())+1);
                }

                dcResult.ifname = new char[(rsp->call.ifname.length())+1]{};
                if(dcResult.ifname) {
                    strlcpy(dcResult.ifname, rsp->call.ifname.c_str(), (rsp->call.ifname.length())+1);
                }

                dcResult.addresses = new char[(rsp->call.addresses.length())+1]{};
                if (dcResult.addresses) {
                    strlcpy(dcResult.addresses, rsp->call.addresses.c_str(), (rsp->call.addresses.length())+1);
                }

                dcResult.dnses = new char[(rsp->call.dnses.length())+1]{};
                if (dcResult.dnses) {
                    strlcpy(dcResult.dnses, rsp->call.dnses.c_str(), (rsp->call.dnses.length())+1);
                }

                dcResult.gateways = new char[(rsp->call.gateways.length())+1]{};
                if (dcResult.gateways) {
                    strlcpy(dcResult.gateways, rsp->call.gateways.c_str(), (rsp->call.gateways.length())+1);
                }

                dcResult.pcscf = new char[(rsp->call.pcscf.length())+1]{};
                if (dcResult.pcscf) {
                    strlcpy(dcResult.pcscf, rsp->call.pcscf.c_str(), (rsp->call.pcscf.length())+1);
                }
                dcResult.mtu = rsp->call.mtu;
                if (p->write(dcResult) == Marshal::Result::FAILURE) {
                    RLOGI("SetupDataCall Response could not be sent to Client. Marshal Write Failed");
                    sendResponse(context, errorCode, nullptr);
                } else {
                    sendResponse(context, errorCode, p);
                }
                p->release(dcResult);
            } else {
                if (rsp) {
                    switch(rsp->respErr) {
                        case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                        case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                        case ResponseError_t::CALL_NOT_AVAILABLE: errorCode = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                        default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;

                    }
                } else {
                    errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                }
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
    parcel.release(callParams);
}

void dispatchSetupDataCall_1_6(std::shared_ptr<SocketRequestContext> context, Marshal& parcel) {
    RLOGI("Dispatching SetUpDataCall request.");
    using namespace rildata;
    int32_t serial =0;
    RIL_SetUpDataCallParams_Ursp callParams = {};
    if (parcel.read(callParams) != Marshal::Result::SUCCESS)
    {
        RLOGI("SetupDataCallRequestMessage could not be dispatched. Marshal Read Failed.");
        return;
    }
    AccessNetwork_t accnet = AccessNetwork_t::UNKNOWN;
    switch(callParams.accessNetwork) {
        case RIL_RadioAccessNetworks::GERAN:
            accnet = AccessNetwork_t::GERAN;
            break;
        case RIL_RadioAccessNetworks::UTRAN:
            accnet = AccessNetwork_t::UTRAN;
            break;
        case RIL_RadioAccessNetworks::NGRAN:
            accnet = AccessNetwork_t::NGRAN;
            break;
        case RIL_RadioAccessNetworks::CDMA:
            accnet = AccessNetwork_t::CDMA;
            break;
        case RIL_RadioAccessNetworks::EUTRAN:
            accnet = AccessNetwork_t::EUTRAN;
            break;
        default:
            accnet = AccessNetwork_t::UNKNOWN;
    }
    RIL_DataProfileInfo dataProfileInfo = callParams.profileInfo;
    bool roamingAllowed = static_cast<bool>(callParams.roamingAllowed);
    RIL_RadioDataRequestReasons reason = callParams.reason;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;
    bool trafficDescriptorValid = dataProfileInfo.trafficDescriptorValid;
    bool matchAllRuleAllowed = callParams.matchAllRuleAllowed;
    int i = 0;
    string s = "";
    if (callParams.addresses != nullptr) {
        while(callParams.addresses[i] != '\0') {
            if (callParams.addresses[i] != ',') {
                s += callParams.addresses[i];
            }
            else {
                radioAddresses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioAddresses.push_back(s);
        }
    }
    i = 0;
    s = "";
    if (callParams.dnses) {
        while(callParams.dnses[i] != '\0') {
            if (callParams.dnses[i] != ',') {
                s += callParams.dnses[i];
            }
            else {
                radioDnses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioDnses.push_back(s);
        }
    }

    // Check for nulls
    bool invalid_value = false;

    if (!dataProfileInfo.apn) {
        RLOGI("dataProfileInfo.apn is a nullptr.");
        invalid_value = true;
    }

    if (!dataProfileInfo.protocol) {
        RLOGI("dataProfileInfo.protocol is a nullptr.");
        invalid_value = true;
    }

    if (!dataProfileInfo.roamingProtocol) {
        RLOGI("dataProfileInfo.roamingProtocol is a nullptr.");
        invalid_value = true;
    }

    if (!dataProfileInfo.user) {
        RLOGI("dataProfileInfo.user is a nullptr.");
        invalid_value = true;
    }

    if (!dataProfileInfo.password) {
        RLOGI("dataProfileInfo.password is a nullptr.");
        invalid_value = true;
    }
    if (dataProfileInfo.trafficDescriptor.dnnValid &&
        !dataProfileInfo.trafficDescriptor.dnn)
    {
        RLOGI("dataProfileInfo.trafficDescriptor.dnn is a nullptr.");
        invalid_value = true;
    }

    if (invalid_value) {
        sendResponse(context, RIL_Errno::RIL_E_INVALID_ARGUMENTS, nullptr);
        // Release callParams before we return.
        parcel.release(callParams);
        return;
    }

    DataProfileInfo_t dataProfile = {
        .profileId = (DataProfileId_t)dataProfileInfo.profileId,
        .apn = std::string(dataProfileInfo.apn),
        .protocol = std::string(dataProfileInfo.protocol),
        .roamingProtocol = std::string(dataProfileInfo.roamingProtocol),
        .authType = (ApnAuthType_t)dataProfileInfo.authType,
        .username = std::string(dataProfileInfo.user),
        .password = std::string(dataProfileInfo.password),
        .dataProfileInfoType = (DataProfileInfoType_t)dataProfileInfo.type,
        .maxConnsTime = dataProfileInfo.maxConnsTime,
        .maxConns = dataProfileInfo.maxConns,
        .waitTime = dataProfileInfo.waitTime,
        .enableProfile = (bool)dataProfileInfo.enabled,
        .supportedApnTypesBitmap = (ApnTypes_t)(dataProfileInfo.supportedTypesBitmask),
        .bearerBitmap = (RadioAccessFamily_t)(dataProfileInfo.bearerBitmask),
        .mtuV4 = dataProfileInfo.mtuV4,
        .mtuV6 = dataProfileInfo.mtuV6,
        .preferred = (bool)dataProfileInfo.preferred,
        .persistent = (bool)dataProfileInfo.persistent,
    };
    auto msg =
        std::make_shared<SetupDataCallRequestMessage_1_6>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        dataProfile,
        roamingAllowed,
        (DataRequestReason_t)reason,
        radioAddresses,
        radioDnses,
        matchAllRuleAllowed,
        nullptr);
        RLOGI("Dispatching dispatchSetupDataCall request.");
    if ( msg ) {
        if (trafficDescriptorValid) {
            TrafficDescriptor_t td = {};
            if (dataProfileInfo.trafficDescriptor.dnnValid) {
                td.dnn = std::string(dataProfileInfo.trafficDescriptor.dnn);
            }
            if (dataProfileInfo.trafficDescriptor.osAppIdValid) {
                td.osAppId = std::vector<uint8_t>(dataProfileInfo.trafficDescriptor.osAppId,
                    dataProfileInfo.trafficDescriptor.osAppId + dataProfileInfo.trafficDescriptor.osAppIdLength);
            }
            msg->setOptionalTrafficDescriptor(td);
        }
        GenericCallback<SetupDataCallResponse_t> cb([context](std::shared_ptr<Message> msg,
            Message::Callback::Status status,
            std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
        if (status == Message::Callback::Status:: NO_HANDLER_FOUND) {
            sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
            return;
        }
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchSetupDataCall Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            RIL_Data_Call_Response_v11 dcResult = {};
            if ( msg && (status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {

                RLOGI("setup data call cb invoked status %d ", status);
                std::stringstream ss;
                rsp->call.dump("", ss);
                RLOGI("call=%s", ss.str().c_str());
                errorCode = static_cast<RIL_Errno>(rsp->respErr);
                ril::socket::utils::convertToSocket(dcResult, rsp->call);
                if (p->write(dcResult) == Marshal::Result::FAILURE) {
                    RLOGI("SetupDataCall Response could not be sent to Client. Marshal Write Failed");
                    sendResponse(context, errorCode, nullptr);
                } else {
                    sendResponse(context, errorCode, p);
                }
                p->release(dcResult);
            } else {
                if (rsp) {
                    switch(rsp->respErr) {
                        case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                        case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                        case ResponseError_t::CALL_NOT_AVAILABLE: errorCode = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                        default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;
                    }
                } else {
                    errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                }
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
    parcel.release(callParams);
}

}
}
}
