/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021, 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2020 The Linux Foundation. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above
        copyright notice, this list of conditions and the following
        disclaimer in the documentation and/or other materials provided
        with the distribution.
      * Neither the name of The Linux Foundation nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=============================================================================*/
#ifndef LOC_NET_EXT_IFACE_H
#define LOC_NET_EXT_IFACE_H

#include <string>
#include <functional>

#define LOCNET_DATA_CALL_PARAMS_IP_VALID_BIT             (1 << 0)
#define LOCNET_DATA_CALL_PARAMS_INTERFACE_NAME_VALID_BIT (1 << 1)
#define LOCNET_DATA_CALL_PARAMS_PRIMARY_DNS_VALID_BIT    (1 << 2)
#define LOCNET_DATA_CALL_PARAMS_SECONDARY_DNS_VALID_BIT  (1 << 3)

using namespace std;

typedef enum {
    LOC_NET_NOTIFY_MCC_MNC_ITEM,
    LOC_NET_NOTIFY_TAC_ITEM
} LocNetNotifyItemIdType;


/** @brief
    Response indication from StatusCb<br/>
*/
typedef enum {
    /** Locnet connectionManager is successfully
        initialized,
      * Unsolicited or some time after calling
        init()
    */
    LOCNET_INITIALIZED = 0,
    /** Connection is setup and ready for
        download.
      * unsolicited or some time after calling
        connectBackhaul()
      * Valid data call params expected from
        StatusCb
    */
    LOCNET_CONNECTED = 1,
    /** Connection is not ready or disconnected
        Unsolicited or some time after calling
        disconnectBackhaul()
    */
    LOCNET_DISCONNECTED = 2,
    /** General Error, For any other issues
        during connection,disconnection,Init
        or timeout
    */
    LOCNET_GENERAL_ERROR = 3,
    /**Service is unavailable.*/
    LOCNET_SERVICE_UNAVAILABLE = 4,
    /**Service is available.*/
    LOCNET_SERVICE_AVAILABLE = 5,
} LocNetStatusType;

struct LocNetClientContext {
    string        client;
    LocSubId      prefSub;
    string        prefApn;
    LocApnIpType  prefIpType;

    LocNetClientContext(): client(""),
        prefApn(""), prefSub(LOC_DEFAULT_SUB),
        prefIpType(LOC_APN_IP_IPV4V6) {
        /* noop */
    }
};

struct LocNetDataCallParams {
    string ip;
    string interfaceName;
    string primaryDns;
    string secondaryDns;
    uint8_t validityMask;

    LocNetDataCallParams():ip(""),
        interfaceName(""), primaryDns(""), secondaryDns(""),
        validityMask(0) {
        /* noop */
    }

    LocNetDataCallParams(const struct LocNetDataCallParams *in) {
        if (NULL != in) {
            validityMask = in->validityMask;
            if (validityMask & LOCNET_DATA_CALL_PARAMS_IP_VALID_BIT) {
                ip = in->ip;
            }
            if (validityMask & LOCNET_DATA_CALL_PARAMS_INTERFACE_NAME_VALID_BIT) {
                interfaceName = in->interfaceName;
            }
            if (validityMask & LOCNET_DATA_CALL_PARAMS_PRIMARY_DNS_VALID_BIT) {
                primaryDns = in->primaryDns;
            }
            if (validityMask & LOCNET_DATA_CALL_PARAMS_SECONDARY_DNS_VALID_BIT) {
                secondaryDns = in->secondaryDns;
            }
        }
    }

    LocNetDataCallParams& operator= (const struct LocNetDataCallParams& in) {
        validityMask = in.validityMask;
        if (validityMask & LOCNET_DATA_CALL_PARAMS_IP_VALID_BIT) {
            ip = in.ip;
        }
        if (validityMask & LOCNET_DATA_CALL_PARAMS_INTERFACE_NAME_VALID_BIT) {
            interfaceName = in.interfaceName;
        }
        if (validityMask & LOCNET_DATA_CALL_PARAMS_PRIMARY_DNS_VALID_BIT) {
            primaryDns = in.primaryDns;
        }
        if (validityMask & LOCNET_DATA_CALL_PARAMS_SECONDARY_DNS_VALID_BIT) {
            secondaryDns = in.secondaryDns;
        }
        return *this;
    }
};

struct LocNetMccMncInfo {
    int mcc;
    int mnc;
    LocNetMccMncInfo():mcc(0), mnc(0){
        /* noop */
    }
};

struct LocNetNetworkInfo {
    LocNetMccMncInfo mccMncInfo;
    int tac;
    uint8_t itemMask;
    LocNetNetworkInfo():tac(0), itemMask(0) {
        /* noop */
    }
};

typedef struct {
    string client;
    LocNetStatusType status;
    LocNetDataCallParams callParams;
} StatusResp;


/** @brief
    StatusCb is for receiving status indications from network manager <br/>
    @param client: client name
    @param Status: status indication defined in LocNetStatusType
    @param callParams: Data call parameters liek Ip address, interface name.
          If Status is "LOCNET_CONNECTED", valid call Params expected.
          For other status values, it shall be NULL<br/>
*/
using StatusCb = std::function<void(const string& client, const LocNetStatusType Status,
        const LocNetDataCallParams *callParams)>;
using NotifyItemCb = std::function<void(const string& client, const LocNetNetworkInfo *nwInfo,
        const LocSubId sub)>;

class LocNetExtIface {
public:
    /** @brief
        Creates an instance of LocNetExtIface object. <br/>
    */
    inline LocNetExtIface() {
    }

    /** @brief
        Delete an instance of LocNetExtIface object. <br/>
    */
    virtual ~LocNetExtIface() {};

    /** @brief
        Initialize network manager. <br/>
        @param statusCb: status callback function
        @param clientCtx: pointer to client context
        @return true, on success
        @return false, on failure
    */
    virtual bool init(const StatusCb statusCb, const LocNetClientContext* clientCtx) = 0;

    /** @brief
        Setup backhaul connection <br/>
        @param client: client name
        @return true, on success
        @return false, on failure
    */
    virtual bool connectBackhaul(const string& client) = 0;

    /** @brief
        Disconnects the backhaul connection<br/>
        @param client: client name
        @return true, on success
        @return false, on failure
    */
    virtual bool disconnectBackhaul(const string& client) = 0;

    /** @brief
        Deinitialize connection manager<br/>
        @return true, on success
        @return false, on failure
    */
    virtual bool deinitialize(const string& client) = 0;

    /** @brief
        Register network info notify callback to connection Manager<br/>
        @return true, on success
        @return false, on failure
    */
    virtual bool registerNotifyCb(const NotifyItemCb notifyCb,
                                         const string& client) = 0;

    /** @brief
        Handle connection status indications from connection manager<br/>
    */
    virtual void handleStatusCb(const string& client, const LocNetStatusType Status,
            const LocNetDataCallParams* callParams) = 0;

    /** @brief
        Handle Network info indications from Phone manager<br/>
    */
    virtual void handleNotifyCb(const string& client, const LocNetNetworkInfo *nwInfo,
            const LocSubId subId) = 0;

    LocNetExtIface(const LocNetExtIface &) = delete;
    LocNetExtIface(LocNetExtIface &&) = delete;
    LocNetExtIface& operator=(const LocNetExtIface &) = delete;
    LocNetExtIface& operator=(LocNetExtIface &&) = delete;
};

// Entry point to the loc net ext impl,
// Must implement: extern "C" LocNetExtIface* getLocNetExtInstance()
typedef LocNetExtIface* (getLocNetExtFn)();

#endif /* #ifndef LOC_NET_EXT_IFACE_H */
