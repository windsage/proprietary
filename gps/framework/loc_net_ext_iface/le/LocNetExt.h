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
#ifndef LOC_NET_EXT_H
#define LOC_NET_EXT_H

#include <string>
#include "msg_q.h"
#include <MsgTask.h>
#include <log_util.h>
#include <loc_gps.h>
#include <functional>
#include <map>
#include <LocNetExtIface.h>

using namespace std;
using namespace loc_util;

using locNetRunnable = std::function<void()>;

class LocNetTelSdkIface;
class LocNetTelSdkPhoneManager;

typedef enum {
    LOC_NET_MSG_INIT_NETWORK_MANAGER = 1,
    LOC_NET_MSG_DEINIT_NETWORK_MANAGER,
    LOC_NET_MSG_CONNECT_BACKHAUL,
    LOC_NET_MSG_DISCONNECT_BACKHAUL,
    LOC_NET_MSG_REG_NOTIFY_CB,
    LOC_NET_MSG_STATUS_RESP_CB,
    LOC_NET_MSG_NOTIFY_NW_INFO_CB,
} LocNetExtMsgId;

struct LocNetExtMsgHdr {
    LocNetExtMsgId msgId;
    inline LocNetExtMsgHdr(LocNetExtMsgId id): msgId(id) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_INIT_NETWORK_MANAGER */
struct LocNetExtMsgInitNetworkManager: public LocNetExtMsgHdr {
    LocNetClientContext mClientCtx;
    inline LocNetExtMsgInitNetworkManager(LocNetClientContext clientCtx) :
        LocNetExtMsgHdr(LOC_NET_MSG_INIT_NETWORK_MANAGER),
        mClientCtx(clientCtx) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_DEINIT_NETWORK_MANAGER */
struct LocNetExtMsgDeinitNetworkManager: public LocNetExtMsgHdr {
    string mClient;
    inline LocNetExtMsgDeinitNetworkManager(const string client) :
        LocNetExtMsgHdr(LOC_NET_MSG_DEINIT_NETWORK_MANAGER),
        mClient(client) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_CONNECT_BACKHAUL */
struct LocNetExtMsgConnectBackhaul: public LocNetExtMsgHdr {
    string mClient;
    inline LocNetExtMsgConnectBackhaul(const string client) :
        LocNetExtMsgHdr(LOC_NET_MSG_CONNECT_BACKHAUL),
        mClient(client) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_DISCONNECT_BACKHAUL */
struct LocNetExtMsgDisconnectBackhaul: public LocNetExtMsgHdr {
    string mClient;
    inline LocNetExtMsgDisconnectBackhaul(const string client) :
        LocNetExtMsgHdr(LOC_NET_MSG_DISCONNECT_BACKHAUL),
        mClient(client) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_STATUS_RESP_CB */
struct LocNetExtMsgRespCb: public LocNetExtMsgHdr {
    StatusResp mResp;
    inline LocNetExtMsgRespCb(const StatusResp &in_resp) :
        LocNetExtMsgHdr(LOC_NET_MSG_STATUS_RESP_CB),
        mResp(in_resp) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_REG_NOTIFY_CB */
struct LocNetExtMsgRegisterNotifyCb: public LocNetExtMsgHdr {
    string mClient;
    inline LocNetExtMsgRegisterNotifyCb(string client) :
        LocNetExtMsgHdr(LOC_NET_MSG_REG_NOTIFY_CB),
        mClient(client) {
        /* noop */
    }
};

/* definition for message with msg id of LOC_NET_MSG_NOTIFY_NW_INFO_CB */
struct LocNetExtMsgNotifyNwInfoCb: public LocNetExtMsgHdr {
    LocNetNetworkInfo mNwInfo;
    LocSubId mSubId;
    string mClient;
    inline LocNetExtMsgNotifyNwInfoCb(string& client, LocNetNetworkInfo& in_nwInfo,
        LocSubId& in_subID) :
        LocNetExtMsgHdr(LOC_NET_MSG_NOTIFY_NW_INFO_CB),
        mClient(client),
        mNwInfo(in_nwInfo),
        mSubId(in_subID) {
        /* noop */
    }
};

/** @brief
    StatusCb is for receiving status indications from network manager <br/>
    @param client: client name
    @param Status: status indication defined in LocNetStatusType
    @param callParams: Data call parameters liek Ip address, interface name.
          If Status is "LOCNET_CONNECTED", valid call Params expected.
          For other status values, it shall be NULL<br/>
*/

class LocNetExt : public LocNetExtIface {
public:
    /** @brief
        Creates an instance of LocNetExt object. <br/>
    */
    LocNetExt();
    /** @brief
        Delete an instance of LocNetExt object. <br/>
    */
    ~LocNetExt() override;
    /** @brief
        Initialize network manager. <br/>
        @param statusCb: status callback function
        @param clientCtx: pointer to client context
        @return true, on success
        @return false, on failure
    */
    bool init(const StatusCb statusCb, const LocNetClientContext* clientCtx) override;
    /** @brief
        Setup backhaul connection <br/>
        @param client: client name
        @return true, on success
        @return false, on failure
    */
    bool connectBackhaul(const string& client) override;

    /** @brief
        Disconnects the backhaul connection<br/>
        @param client: client name
        @return true, on success
        @return false, on failure
    */
    bool disconnectBackhaul(const string& client) override;
    /** @brief
        Deinitialize connection manager<br/>
        @return true, on success
        @return false, on failure
    */
    bool deinitialize(const string& client) override;
    /** @brief
        Register network info notify callback to connection Manager<br/>
        @return true, on success
        @return false, on failure
    */
    bool registerNotifyCb(const NotifyItemCb notifyCb, const string& client) override;
    /** @brief
        Handle connection status indications from connection manager<br/>
    */
    void handleStatusCb(const string& client, const LocNetStatusType Status,
            const LocNetDataCallParams* callParams) override;
    /** @brief
        Handle Network info indications from Phone manager<br/>
    */
    void handleNotifyCb(const string& client, const LocNetNetworkInfo *nwInfo,
            const LocSubId subId) override;
    /** @brief
        Queue a Executable lambda to msgTask<br/>
    */
    void sendMsg(locNetRunnable& runable) const;

private:
    /** @brief
        Queue Messages to Msgtask thread<br/>
    */
    void queueLocNetExtMsg(const LocNetExtMsgHdr* msg);
    /** @brief
        Process messages in Msgtask thread<br/>
    */
    void processLocNetExtMsg(const LocNetExtMsgHdr* msg);

private:
    static MsgTask* mMsgTask;
    std::map<string, StatusCb> mStatusCbClientMap;
    std::map<string, NotifyItemCb> mNotifyCbClientMap;
    LocNetTelSdkIface        *mBackhaulConnIface;
    LocNetTelSdkPhoneManager *mPhoneManager;
};
#endif /* #ifndef LOC_NET_EXT_IFACE_H */
