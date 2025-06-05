/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#include <stdint.h>
#include <IzatRemoteApi.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IzatTypes.h>
#include <algorithm>
#include <vector>
#include <IzatDefines.h>
#include <loc_pla.h>
#include <log_util.h>
#include "loc_cfg.h"
#include "DataItemId.h"
#include "DataItemConcreteTypeFieldNames.h"

using namespace std;
using namespace qc_loc_fw;
using namespace izat_manager;

namespace izat_remote_api {

struct OutCard {
    const char* mTo;
    OutPostcard* const mCard;
    inline OutCard(const char* to) :
        mTo(to), mCard(OutPostcard::createInstance()) {}
    inline ~OutCard() { delete mCard; }
};

class IzatNotifierProxy : public IIPCMessagingResponse {
private:
    const char* const mName;
    IPCMessagingProxy* const mIPCProxy;
    std::vector<IzatNotifier*> mNotifiers;
    inline IzatNotifierProxy(const char* const name, IPCMessagingProxy* ipcProxy) :
        mName(name), mIPCProxy(ipcProxy), mNotifiers() {
    }
    inline ~IzatNotifierProxy() { }
public:
    IPCMessagingProxy* getIPCProxy() { return mIPCProxy; }
    static IzatNotifierProxy* get(const char* const name) {
        IPCMessagingProxy* ipcProxy = IPCMessagingProxy::getInstance();
        if (ipcProxy == nullptr) {
            LOC_LOGe("ipcProxy is null");
            return nullptr;
        }
        IzatNotifierProxy* notifierProxy = (IzatNotifierProxy*)
            ipcProxy->getResponseObj(name);
        if (notifierProxy == nullptr) {
            notifierProxy = new IzatNotifierProxy(name, ipcProxy);
            ipcProxy->registerResponseObj(name, notifierProxy);
        }
        return notifierProxy;
    }
    static void drop(IzatNotifierProxy* notifierProxy) {
        if (notifierProxy->mNotifiers.size() == 0) {
            notifierProxy->mIPCProxy->unregisterResponseObj(notifierProxy->mName);
            delete notifierProxy;
        }
    }
    inline void addNotifier(IzatNotifier* notifier, const OutCard* subCard) {
        mNotifiers.push_back(notifier);
        if (subCard && mNotifiers.size() == 1) {
            mIPCProxy->sendMsg(subCard->mCard, subCard->mTo);
        }
    }
    inline void removeNotifier(IzatNotifier* notifier) {
        std::vector<IzatNotifier*>::iterator it =
            find(mNotifiers.begin(), mNotifiers.end(), notifier);
        if (it != mNotifiers.end()) {
            mNotifiers.erase(it);
        }
    }
    inline virtual void handleMsg(InPostcard * const in_card) final {
         for (auto const& notifier : mNotifiers) {
              notifier->handleMsg(in_card);
         }
    }
    inline void sendCard(OutPostcard* card, bool deleteCard = true) {
        if (mIPCProxy && card) {
            mIPCProxy->sendIpcMessage(card->getEncodedBuffer());
            if (deleteCard) {
                delete card;
            }
        }
    }
};
} // namespace izat_remote_api
