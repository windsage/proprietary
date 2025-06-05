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
#define LOG_TAG "LocSvc_LocNetExtLE"

#include "LocNetExt.h"
#include "LocNetTelSdkIface.h"
#include <log_util.h>
#include <unistd.h>

static LocNetExtIface* mLocNetExtInstance = nullptr;

extern "C" LocNetExtIface* getLocNetExtInstance() {
    if (mLocNetExtInstance == nullptr) {
        mLocNetExtInstance = new LocNetExt();
    }
    return mLocNetExtInstance;
}

MsgTask* LocNetExt::mMsgTask = NULL;

class LocMsgWrapper : public LocMsg {
private:
    locNetRunnable mRunnable;

public:
    LocMsgWrapper(locNetRunnable& runnable) :
            mRunnable(runnable) {
        /* noop */
    }
    virtual void proc() const {
        mRunnable();
    }
};

LocNetExt::LocNetExt() :
        mBackhaulConnIface(new LocNetTelSdkIface(this)),
        mPhoneManager(new LocNetTelSdkPhoneManager()) {

    if (NULL == mMsgTask) {
        mMsgTask = new MsgTask("LocNetExt");
    }
}

LocNetExt::~LocNetExt() {
    if (NULL != mBackhaulConnIface) {
        delete mBackhaulConnIface;
    }

    if (NULL != mPhoneManager) {
        delete mPhoneManager;
    }
}

void LocNetExt::sendMsg(locNetRunnable& runable) const {
    LocMsgWrapper *msg = new LocMsgWrapper(runable);
    if (NULL == msg) {
        LOC_LOGe("NULL msg");
        return;
    }
    mMsgTask->sendMsg(msg);
}

bool LocNetExt::init(const StatusCb statusCb, const LocNetClientContext *clientCtx) {
    if (NULL != clientCtx && !clientCtx->client.empty()) {
        mStatusCbClientMap[clientCtx->client] = statusCb;
        LocNetExtMsgInitNetworkManager *initMsg = new LocNetExtMsgInitNetworkManager(*clientCtx);
        if (NULL != initMsg) {
            queueLocNetExtMsg(initMsg);
            return true;
        }
    }
    return false;
}

bool LocNetExt::deinitialize(const string& client) {
    LocNetExtMsgDeinitNetworkManager *deinitMsg = new LocNetExtMsgDeinitNetworkManager(client);
    if (NULL == deinitMsg) {
        LOC_LOGe("Null Deinit Msg");
        return false;
    }
    queueLocNetExtMsg(deinitMsg);
    return true;
}

bool LocNetExt::connectBackhaul(const string& client) {
    LocNetExtMsgConnectBackhaul *connectMsg = new LocNetExtMsgConnectBackhaul(client);
    if (NULL == connectMsg) {
        LOC_LOGe("NUll connect Msg");
        return false;
    }
    queueLocNetExtMsg(connectMsg);
    return true;
}

bool LocNetExt::disconnectBackhaul(const string& client) {
    LocNetExtMsgDisconnectBackhaul *disconnectMsg = new LocNetExtMsgDisconnectBackhaul(client);
    if (NULL == disconnectMsg) {
        LOC_LOGe("NUll disconnect Msg");
        return false;
    }
    queueLocNetExtMsg(disconnectMsg);
    return true;
}

bool LocNetExt::registerNotifyCb(NotifyItemCb notifyCb, const string& client) {
    if (!client.empty()) {
        mNotifyCbClientMap[client] = notifyCb;
        LocNetExtMsgRegisterNotifyCb *regNotifyMsg = new LocNetExtMsgRegisterNotifyCb(client);
        if (NULL != regNotifyMsg) {
            queueLocNetExtMsg(regNotifyMsg);
            return true;
        }
    }
    return false;
}

void LocNetExt::handleStatusCb(const string& client, const LocNetStatusType status,
     const LocNetDataCallParams *callParams) {
    StatusResp respInfo;
    respInfo.client = client;
    respInfo.status = status;

    if (NULL != callParams) {
        respInfo.callParams = *callParams;
    }

    LocNetExtMsgRespCb *respMsg = new LocNetExtMsgRespCb(respInfo);
    if (NULL == respMsg) {
        LOC_LOGe("NUll resp Msg");
        return;
    }
    queueLocNetExtMsg(respMsg);
}

void LocNetExt::handleNotifyCb(const string& client, const LocNetNetworkInfo *nwInfo,
    const LocSubId subId) {
    LocNetNetworkInfo netInfo;

    if (NULL != nwInfo) {
        netInfo = *nwInfo;
    }
    LocSubId sub = subId;
    string clt = client;

    LocNetExtMsgNotifyNwInfoCb *notifyMsg = new LocNetExtMsgNotifyNwInfoCb(clt, netInfo, sub);
    if (NULL == notifyMsg) {
        LOC_LOGe("Null notify Msg");
        return;
    }
    queueLocNetExtMsg(notifyMsg);
}

void LocNetExt::queueLocNetExtMsg(const LocNetExtMsgHdr* msg) {
    struct LocNetExtMsgType : public LocMsg {
        LocNetExt* mLocNetMgr;
        const LocNetExtMsgHdr* mMsg;
        inline LocNetExtMsgType(LocNetExt* locNetMgr,
            const LocNetExtMsgHdr* msg) :
            LocMsg(),
            mLocNetMgr(locNetMgr),
            mMsg(msg) {
            /* noop */
        }
        inline virtual void proc() const {
            mLocNetMgr->processLocNetExtMsg(mMsg);
            delete mMsg;
        }
    };
    LocNetExtMsgType *extMsg = new LocNetExtMsgType(this, msg);
    if (NULL == extMsg) {
        LOC_LOGe("NULL extMsg");
        return;
    }
    mMsgTask->sendMsg(extMsg);
}

void LocNetExt::processLocNetExtMsg(const LocNetExtMsgHdr* msg) {
    if (nullptr == msg) {
        LOC_LOGe("msg is NULL!!");
        return;
    }

    switch (msg->msgId) {
        case LOC_NET_MSG_INIT_NETWORK_MANAGER: {
            LOC_LOGd("LOC_NET_MSG_INIT_NETWORK_MANAGER");
            const LocNetExtMsgInitNetworkManager *initMsg = \
                (const LocNetExtMsgInitNetworkManager *)msg;
            if (NULL != mBackhaulConnIface) {
                mBackhaulConnIface->init(std::bind(&LocNetExt::handleStatusCb, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    &initMsg->mClientCtx);
            }
            break;
        }
        case LOC_NET_MSG_DEINIT_NETWORK_MANAGER: {
            LOC_LOGd("LOC_NET_MSG_DEINIT_NETWORK_MANAGER");
            const LocNetExtMsgDeinitNetworkManager *deinitMsg = \
                (const LocNetExtMsgDeinitNetworkManager *)msg;
            if (NULL != mBackhaulConnIface) {
                mBackhaulConnIface->deinitialize(deinitMsg->mClient);
            }
            break;
        }
        case LOC_NET_MSG_CONNECT_BACKHAUL: {
            LOC_LOGd("LOC_NET_MSG_CONNECT_BACKHAUL");
            const LocNetExtMsgConnectBackhaul *connectMsg = \
                (const LocNetExtMsgConnectBackhaul*)msg;
            if (NULL != mBackhaulConnIface) {
                mBackhaulConnIface->connectBackhaul(connectMsg->mClient);
            }
            break;
        }
        case LOC_NET_MSG_DISCONNECT_BACKHAUL: {
            LOC_LOGd("LOC_NET_MSG_DISCONNECT_BACKHAUL");
            const LocNetExtMsgDisconnectBackhaul *disconnectMsg = \
                (const LocNetExtMsgDisconnectBackhaul*)msg;
            if (NULL != mBackhaulConnIface) {
                mBackhaulConnIface->disconnectBackhaul(disconnectMsg->mClient);
            }
            break;
        }
        case LOC_NET_MSG_REG_NOTIFY_CB: {
            LOC_LOGd("LOC_NET_MSG_REG_NOTIFY_CB");
            const LocNetExtMsgRegisterNotifyCb *regMsg = (const LocNetExtMsgRegisterNotifyCb*)msg;
            if ( NULL != mPhoneManager) {
                mPhoneManager->registerNotifyItem(std::bind(&LocNetExt::handleNotifyCb, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    regMsg->mClient);
            }
            break;
        }
        case LOC_NET_MSG_STATUS_RESP_CB: {
            LOC_LOGd("LOC_NET_MSG_STATUS_RESP_CB");
            const LocNetExtMsgRespCb *respMsg = (const LocNetExtMsgRespCb *)msg;
            /* Call client status response callback function */
            if (mStatusCbClientMap[respMsg->mResp.client]) {
                mStatusCbClientMap[respMsg->mResp.client](respMsg->mResp.client,
                    respMsg->mResp.status, &respMsg->mResp.callParams);
            }
            break;
        }
        case LOC_NET_MSG_NOTIFY_NW_INFO_CB: {
            LOC_LOGd("LOC_NET_MSG_NOTIFY_NW_INFO_CB");
            const LocNetExtMsgNotifyNwInfoCb *notifyMsg = (const LocNetExtMsgNotifyNwInfoCb*)msg;
            /* Call the notify Cb function */
            if (mNotifyCbClientMap[notifyMsg->mClient]) {
                mNotifyCbClientMap[notifyMsg->mClient](notifyMsg->mClient, &notifyMsg->mNwInfo,
                notifyMsg->mSubId);
            }
        }
        default:
            break;
    }
}
