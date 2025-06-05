/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LOC_NET_IFACE_TELSDK_H
#define LOC_NET_IFACE_TELSDK_H

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <algorithm>
#include <chrono>
#include <utility>

#include <telux/data/DataConnectionManager.hpp>
#include <telux/data/DataFactory.hpp>
#include <telux/common/CommonDefines.hpp>
#include <telux/data/DataDefines.hpp>

#include <telux/tel/CellInfo.hpp>
#include <telux/tel/PhoneManager.hpp>
#include <telux/tel/PhoneFactory.hpp>

#include <telux/tel/Phone.hpp>
#include <telux/tel/PhoneListener.hpp>
#include <telux/tel/PhoneDefines.hpp>
#include <telux/tel/VoiceServiceInfo.hpp>
#include <telux/tel/ECallDefines.hpp>


#include "LocNetExt.h"
#include "LocTimer.h"
#include <loc_gps.h>


#ifdef NO_UNORDERED_SET_OR_MAP
    #include <set>
    #define unordered_set set
#else
    #include <unordered_set>
#endif

typedef unordered_set<string> stateClientSet;

#define LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC 10
#define LOCNET_TELSDK_CONNECT_CB_TIMEOUT_IN_SEC 10
#define LOCNET_TELSDK_CELLINFO_TIMEOUT_IN_SEC 5

#define LOCNET_TELSDK_MCC_UPDATED_BIT 0
#define LOCNET_TELSDK_MNC_UPDATED_BIT 1
#define LOCNET_TELSDK_TAC_UPDATED_BIT 2

#define LOCNET_PHONE_MANAGER_RETRY_TIME_IN_SEC 5
#define LOCNET_PHONE_MANAGER_RETRY_TIMES 10
#define LOCNET_PHONE_MANAGER_RETRY_TIMER "PhoneManagerRetryTimer"

using namespace std;
using namespace telux::data;
using namespace telux::common;
using namespace telux::tel;

using LocNetTelSdkInitResponseCb = std::function<void(const LocNetStatusType status)>;
using profileIdType = uint8_t;

class LocNetTelSdkIface;
class LocNetTelSdkPhoneListener;
class LocNetTelSdkDSSManager;

enum class LocNetTelSdkState {
    LOCNET_TELSDK_STATE_INIT,
    LOCNET_TELSDK_STATE_INIT_ONGOING,
    LOCNET_TELSDK_STATE_READY,
};

enum class LocNetTelSdkProfileState {
    LOCNET_PROFILE_STATE_NOT_READY,
    LOCNET_PROFILE_STATE_READY,
    LOCNET_PROFILE_STATE_CONNECTING,
    LOCNET_PROFILE_STATE_CONNECTED,
    LOCNET_PROFILE_STATE_DISCONNECTING,
    LOCNET_PROFILE_STATE_DISCONNECTED
};

enum class LocNetClientState {
    LOCNET_CLIENT_NOT_READY,
    LOCNET_CLIENT_READY
};

struct LocNetTelSdkSlotDataCtx {
    LocNetTelSdkState state;
    stateClientSet telSdkInitStateClientSet;
    std::shared_ptr<IDataConnectionManager> dataConnManager;
    std::shared_ptr<IDataConnectionListener> dataConnListener;
    std::shared_ptr<IDataProfileManager> dataProfileManager;
    std::unique_ptr<LocNetTelSdkDSSManager> dataServingSystemManager;
    std::vector<DataProfile> dataProfileList;
};

struct LocNetNwInfo {
    LocNetMccMncInfo mccMncInfo;
    int tacInfo;
    CellType cellType;
};

struct LocNetTelSdkPhoneCtx {
    std::shared_ptr<IPhone> iPhone;
    LocNetNwInfo nwInfo;
};

struct LocNetTelSdkProfileCtx {
    LocNetTelSdkProfileState profileState;
    std::map<LocNetTelSdkProfileState, stateClientSet> profileStateClientSetMap;
    LocNetDataCallParams profileCallParams;
};


/* Global static utility functions */
static SlotId convertSubId(LocSubId sub);
static LocSubId convertToLocSubId(SlotId slot);
static IpFamilyType convertIpType(LocApnIpType ipType);

class LocNetTimer : public LocTimer {
public:
    LocNetTimer(const string& name):LocTimer(), mName(name),
            mStarted(false) {
        LOC_LOGd("LocNetTimer %s created.", mName.c_str());
    }

    ~LocNetTimer() {
        stop();
    }

    inline void set(const time_t waitTimeSec, const locNetRunnable& runable) {
        mWaitTimeInMs = waitTimeSec * (time_t)1000;
        mRunable = runable;
    }
    inline void start() {
        LOC_LOGd("start timer %s, waiting %u ms...", mName.c_str(), (uint32_t)mWaitTimeInMs);
        mStarted = true;
        LocTimer::start(mWaitTimeInMs,
                false // wait for next time AP wakes on other events rather than
                      // waking it up on this timer expiration
                );
    }
    inline void restart() {
        stop();
        start();
    }
    inline void stop() {
        if (!mStarted) {
            LOC_LOGd("time %s not started yet.", mName.c_str());
            return;
        }
        LOC_LOGd("stop timer %s", mName.c_str());
        LocTimer::stop();
        mStarted = false;
    }
    inline bool isStarted() {
        return mStarted;
    }
private:
    /* Override */
    inline virtual void timeOutCallback() override {
        LOC_LOGd("timer %s timeout", mName.c_str());
        mStarted = false;
        mRunable();
    }
private:
    string mName;
    time_t mWaitTimeInMs;
    locNetRunnable mRunable;
    bool mStarted;
     /* Timestamp when timer is started */
    struct timespec mStartTime;
};

class LocNetTelSdkPhoneManager {
public:
    LocNetTelSdkPhoneManager();
    ~LocNetTelSdkPhoneManager();

    /* getter and setters */
    void getPhoneManager();
    inline SlotId getSlotIdMappedPhoneId(const int phoneId) {
        std::map<SlotId, int>::iterator it = mSlotPhoneIdMap.begin();
        for (;it != mSlotPhoneIdMap.end(); ++it) {
            if (phoneId == it->second) {
                return it->first;
            }
        }
        return INVALID_SLOT_ID;
    }
    inline std::mutex& getLocNetPhoneManagerMutex() {return mLock;}
    inline std::map<int, LocNetTelSdkPhoneCtx>& getPhoneManagerCtxMap() {
        return mPhoneidCtxMap;
    }
    inline const std::function<void(uint8_t, int)>& getNotifySubscriberCb() {
        return mNotifySubscribers;
    }

    /* operations */
    void initPhoneManager(const SlotId sub);
    bool processCellInfo(std::vector<std::shared_ptr<CellInfo>>& cellInfoList,
            LocNetNwInfo &nwInfo);
    void registerNotifyItem(const NotifyItemCb notifyCb, const string client);

private:
    std::mutex mLock;
    std::condition_variable mCond;
    LocNetTimer mRetryTimer;
    uint8_t mRetryCount;
    std::shared_ptr<LocNetTelSdkPhoneListener> mPhoneListener;
    std::map<string, NotifyItemCb> mClientNotifyMap;
    std::map<SlotId, int> mSlotPhoneIdMap;
    std::map<int, LocNetTelSdkPhoneCtx> mPhoneidCtxMap;
    std::shared_ptr<IPhoneManager> mPhoneManager;
    std::function<void(uint8_t, int)> mNotifySubscribers;
};

class LocNetTelSdkPhoneListener : public IPhoneListener {
public:
    LocNetTelSdkPhoneListener(LocNetTelSdkPhoneManager &instance): mPhoneMgr(instance) {
        /* noop */
    }
    ~LocNetTelSdkPhoneListener() = default;
    void onCellInfoListChanged(int phoneId,
            std::vector<std::shared_ptr<CellInfo>> cellInfoList);
private:
    LocNetTelSdkPhoneManager& mPhoneMgr;
};
class LocNetTelSdkDSSManager {
public:
    LocNetTelSdkDSSManager(SlotId subId, LocNetTelSdkIface& mInstance):
            subId(subId),
            mLocNetInstance(mInstance),
            dataServingSystemManager(nullptr),
            dssMgrServiceStatus(telux::common::ServiceStatus::SERVICE_FAILED) {
        dssMgrNwServiceState.networkRat = telux::data::NetworkRat::UNKNOWN;
        dssMgrNwServiceState.serviceState = telux::data::DataServiceState::UNKNOWN;
    }
    ~LocNetTelSdkDSSManager() = default;
    inline void setDssManager(std::shared_ptr<telux::data::IServingSystemManager> dssMgr)     {
        std::lock_guard<std::mutex> lock(mSyncLock);
        dataServingSystemManager = dssMgr;
    }
    inline void setDssListener(std::shared_ptr<telux::data::IServingSystemListener> dssListener) {
        const std::lock_guard<std::mutex> lock(mSyncLock);
        dataServingSystemListener = dssListener;
    }
    inline std::weak_ptr<telux::data::IServingSystemManager> getDssManager() {
        return dataServingSystemManager;
    }
    inline std::weak_ptr<telux::data::IServingSystemListener> getDssListener() {
        return dataServingSystemListener;
    }
    inline SlotId getSubId() {
        return subId;
    }
    inline telux::common::ServiceStatus getDssManagerServiceStatus() {
        return dssMgrServiceStatus;
    }
    inline void doDeregisterListener() {
        if (nullptr != dataServingSystemManager && nullptr != dataServingSystemListener) {
            auto retStatus = dataServingSystemManager->deregisterListener(
                dataServingSystemListener);
            if (telux::common::Status::SUCCESS == retStatus) {
                LOC_LOGd("successfull dereg of DSS listener");
            }
        }
    }
    void updateNwServiceStatusToClients(telux::data::ServiceStatus status);
    void updateServiceState(telux::common::ServiceStatus status);
private:
    std::mutex mSyncLock;
    const SlotId subId;
    LocNetTelSdkIface& mLocNetInstance;
    telux::common::ServiceStatus dssMgrServiceStatus;
    telux::data::ServiceStatus dssMgrNwServiceState;
    std::shared_ptr<telux::data::IServingSystemManager> dataServingSystemManager;
    std::shared_ptr<telux::data::IServingSystemListener> dataServingSystemListener;
};
class LocNetTelSdkDSSListener : public telux::data::IServingSystemListener {
public:
    LocNetTelSdkDSSListener(LocNetTelSdkDSSManager* dssMgr):
        IServingSystemListener(),
        mDssMgr(dssMgr),
        mListenerRegStatus(false) {
        //noop
    }
    /*Informs about changes in the service state of Network ie unknown/available/unavailable
      This can be possible only if the Data serving system manager internal state is Not
      SERVICE FAILED.
    */
    inline void onServiceStateChanged(telux::data::ServiceStatus status) override {
        LOC_LOGd("Nw Service Status %u  rat %u", static_cast<uint32_t>(status.serviceState),
            static_cast<uint32_t>(status.networkRat));
        if (nullptr != mDssMgr) {
            mDssMgr->updateNwServiceStatusToClients(status);
        }
    }
    /*Informs about changes in service status of the tesldk data serving system manager.
      The service state of the Data serving system manager to provide service to its
      client. This is entirely different from service state of the network which Data
      serving system manager listens/maintains.
    */
    inline void onServiceStatusChange(telux::common::ServiceStatus status) {
        LOC_LOGd("Serving system manager Service Status %u", static_cast<uint32_t>(status));
        if (nullptr != mDssMgr) {
            mDssMgr->updateServiceState(status);
        }
    }
    inline bool getListenerRegStatus() {
        return mListenerRegStatus;
    }
    inline void setListenerRegStatus(const bool status) {
        mListenerRegStatus = status;
    }
private:
    LocNetTelSdkDSSManager* mDssMgr;
    bool mListenerRegStatus;
};

class LocNetTelSdkClient {
public:
    LocNetTelSdkClient():mPrefApn(""), mPrefIpType(IpFamilyType::IPV4),
        mClientName(""), mSubId(DEFAULT_SLOT_ID), mProfileId(0),
        mState(LocNetClientState::LOCNET_CLIENT_NOT_READY) {
        /*noop*/
    }

    /* getter and setters */
    inline void setClientSubId(const LocSubId sub) {
        mSubId = convertSubId(sub);
    }
    inline void setClientIpType(const LocApnIpType ipType) {
        mPrefIpType = convertIpType(ipType);
    }
    inline void setClientApn(const string apn) {mPrefApn = apn;}
    inline void setClientProfileId(const profileIdType id) {mProfileId = id;}
    inline void setClientName(const string name) {mClientName = name;}
    inline void setClientState(LocNetClientState state) {
        LOC_LOGd("Client %s state update Prev=%d new=%d", mClientName.c_str(),
            mState, state);
        mState = state;
    }
    inline void setClientStatusCb(const StatusCb cb) {
        mStatusCb = cb;
    }
    inline string getClientName() {return mClientName;}
    inline profileIdType getClientProfileId() {return mProfileId;}
    inline IpFamilyType getClientIpType() {return mPrefIpType;}
    inline string getClientApn() {return mPrefApn;}
    inline SlotId getClientPrefSubId() {return mSubId;}
    inline LocNetClientState getClientState() {return mState;}
    inline const StatusCb getStatusCb() {return mStatusCb;}

    /* operations */
    inline void executeStatusCb(LocNetStatusType status,
            LocNetDataCallParams *params) {
        if (mStatusCb) {
            mStatusCb(getClientName(), status, params);
        }
    }

private:
    string            mClientName;
    string            mPrefApn;
    SlotId            mSubId;
    profileIdType     mProfileId;
    IpFamilyType      mPrefIpType;
    LocNetClientState mState;
    StatusCb          mStatusCb;
};

class LocNetTelSdkDataListener : public IDataConnectionListener {
public:
    LocNetTelSdkDataListener(LocNetTelSdkIface& instance):
            mLocNetTelSdk(instance),
            mDataConnMgrServiceStatus(telux::common::ServiceStatus::SERVICE_UNAVAILABLE) {
        /*noop*/
    }
    void onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall) override;
    void onServiceStatusChange(telux::common::ServiceStatus status) override;
    inline void setSubId(SlotId sub) {mSubId = sub;}
    inline SlotId getSubId() {return mSubId;}

private:
    inline LocNetTelSdkIface& getLocNetTelSdkInstance()             {
        return mLocNetTelSdk;
    }
private:
    LocNetTelSdkIface &mLocNetTelSdk;
    SlotId mSubId;
    telux::common::ServiceStatus mDataConnMgrServiceStatus;
};

class LocNetTelSdkDataProfileListCallback : public IDataProfileListCallback {
public:
    LocNetTelSdkDataProfileListCallback(SlotId subId, LocNetTelSdkIface& instance):
        mSubId(subId),
        mLocNetTelSdk(instance) {
        /*noop*/
    }
    ~LocNetTelSdkDataProfileListCallback() = default;

    void onProfileListResponse(const std::vector<std::shared_ptr<DataProfile>> &profiles,
            telux::common::ErrorCode error) override ;
    inline telux::common::ErrorCode getErrorCode() {return mError;}
private:
    LocNetTelSdkIface&                        mLocNetTelSdk;
    telux::common::ErrorCode                  mError;
    SlotId mSubId;
};

class LocNetTelSdkIface              {
public:

    LocNetTelSdkIface(LocNetExt *instance);

    ~LocNetTelSdkIface();
    /** @brief
        Initialize LocNetTelSdkIface. <br/>
        @param statusCb: status callback function
        @param client: pointer to client context
    */
    void init(const StatusCb statusCb, const LocNetClientContext *client);
    /** @brief
        Setup Data backhaul connection <br/>
        @param client: client requesting connection
        @return true, on success
        @return false, on failure
    */
    bool connectBackhaul(const string& client);
    /** @brief
        Disconnects the Data backhaul connection <br/>
        @param client: client requesting disconnection
        @return true, on success
        @return false, on failure
    */
    bool disconnectBackhaul(const string& client);
    /** @brief
        De-Initialize LocNetTelSdkIface <br/>
        @param client: client requesting de-Initialize
    */
    void deinitialize(const string& client);

    /* Getter and Setters */
    inline LocNetTelSdkClient* getLocNetTelSdkClient(const string& client) {
        std::vector<LocNetTelSdkClient*>::iterator it = mClients.begin();
        for (; it != mClients.end(); ++it) {
            if (NULL != (*it) && (0 == client.compare((*it)->getClientName()))) {
                return (*it);
            }
         }
         return NULL;
     }
    inline const std::vector<LocNetTelSdkClient*>& getLocNetTelSdkClientList() {
        return mClients;
    }
    inline std::mutex& getLocNetTelSdkLock() {return mLock;}
    inline std::condition_variable& getLocNetTelSdkCond() {return mCond;}

    inline stateClientSet& getInitStateClientSet(const SlotId subId) {
        return mDataConnCtxMap[subId].telSdkInitStateClientSet;
    }
    inline LocNetTelSdkState getLocNetTelSdkState(const SlotId subId) {
        return mDataConnCtxMap[subId].state;
    }
    inline std::shared_ptr<IDataConnectionManager>&
            getLocNetDataConnManager(const SlotId subId) {
        return mDataConnCtxMap[subId].dataConnManager;
    }
    inline std::shared_ptr<IDataProfileManager>&
            getLocNetDataProfileManager(const SlotId subId) {
        return mDataConnCtxMap[subId].dataProfileManager;
    }
    inline std::shared_ptr<IDataConnectionListener>&
            getLocNetDataConnListener(const SlotId subId) {
        return mDataConnCtxMap[subId].dataConnListener;
    }
    inline std::vector<DataProfile>&
            getLocNetDataProfilesList(const SlotId subId) {
        return mDataConnCtxMap[subId].dataProfileList;
    }
    inline std::map<profileIdType, LocNetTelSdkProfileCtx>&
            getLocNetProfileIdMap() {
        return mProfileContextMap;
    }
    inline LocNetTelSdkProfileState&
            getLocNetProfileDataCallState(const profileIdType profileId) {
        return mProfileContextMap[profileId].profileState;
    }
    inline stateClientSet&
            getLocNetClientStateSet(profileIdType profileId, LocNetTelSdkProfileState state) {
        return mProfileContextMap[profileId].profileStateClientSetMap[state];
    }
    inline LocNetDataCallParams* getProfileCallParams(profileIdType profile) {
        return &mProfileContextMap[profile].profileCallParams;;
    }
    inline LocNetExt* getLocNetExtIface() {
        return mExtIfaceInstance;
    }
    inline LocNetTelSdkDSSManager* getLocNetDataServingSystemManager(
            SlotId subId) {
        const std::lock_guard<std::mutex> lock(mSyncLock);
        return nullptr != mDataConnCtxMap[subId].dataServingSystemManager ?
            mDataConnCtxMap[subId].dataServingSystemManager.get() : nullptr;
    }
    inline std::unique_ptr<LocNetTelSdkDSSManager> moveLocNetDataServingSystemManager(
            SlotId subId) {
        const std::lock_guard<std::mutex> lock(mSyncLock);
        auto temp = std::move(mDataConnCtxMap[subId].dataServingSystemManager);
        mDataConnCtxMap[subId].dataServingSystemManager = nullptr;
        return std::move(temp);
    }

    inline void setLocNetTelSdkState(SlotId subId, LocNetTelSdkState state) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        LOC_LOGd("TelSdk state for sub %d updated (prev = %d new = %d)", subId,
            mDataConnCtxMap[subId].state, state);
        mDataConnCtxMap[subId].state = state;
    }
    inline void setLocNetDataConnManager(SlotId subId,
        std::shared_ptr<IDataConnectionManager> connMgr) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        mDataConnCtxMap[subId].dataConnManager = connMgr;
    }
    inline void setLocNetDataProfileManager(SlotId subId,
        std::shared_ptr<IDataProfileManager> profileMgr) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        mDataConnCtxMap[subId].dataProfileManager = profileMgr;
    }
    inline void setLocNetDataConnListener(SlotId subId,
        std::shared_ptr<IDataConnectionListener> connListener) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        mDataConnCtxMap[subId].dataConnListener = connListener;
    }
    inline void setLocNetDataProfilesList(SlotId subId,
        const std::vector<DataProfile>&& profileList) {
        mDataConnCtxMap[subId].dataProfileList = std::move(profileList);
    }
    inline void setLocNetDataServingSystemManager(SlotId subId,
            std::unique_ptr<LocNetTelSdkDSSManager>& dataServingSystemMgr) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        mDataConnCtxMap[subId].dataServingSystemManager = std::move(dataServingSystemMgr);
    }
    inline void setLocNetProfileDataCallState(profileIdType profileId,
        LocNetTelSdkProfileState state) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        LocNetTelSdkProfileState prevState = mProfileContextMap[profileId].profileState;
        mProfileContextMap[profileId].profileState = state;
        LOC_LOGd("profile %d state updated (Prev = %d ,Now = %d)",
                profileId, prevState, state);
    }
    inline void setProfileCallParams(profileIdType profile,
        LocNetDataCallParams params) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        if ( LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED ==
                getLocNetProfileDataCallState(profile) ||
                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING ==
                getLocNetProfileDataCallState(profile)) {
            mProfileContextMap[profile].profileCallParams = params;
        }
    }
    /* Utility functions */
    inline bool isClientPresentInProfileStateClientSet(const profileIdType profileId,
            LocNetTelSdkProfileState state, const string &client) {
        auto it = getLocNetClientStateSet(profileId, state).find(client);
        return (it != getLocNetClientStateSet(profileId, state).end() ? true : false);
    }
    inline void addToProfileStateClientSet(const profileIdType profileId,
            LocNetTelSdkProfileState state, const string client) {
        if (!isClientPresentInProfileStateClientSet(profileId, state, client)) {
            std::lock_guard<std::mutex> lock(mSyncLock);
            LOC_LOGd("client %s inserted to profile %u state %d Set", client.c_str(),
                    profileId, state);
            getLocNetClientStateSet(profileId, state).insert(client);
        }
    }

    inline void removeFromProfileStateClientSet(const profileIdType profileId,
            const LocNetTelSdkProfileState state, const string client) {
        if (client.empty()) {
            LOC_LOGe("Client string empty");
            return;
        }
        LOC_LOGd("remove client %s from profile %u state %d set", client.c_str(), profileId,
            state);
        std::lock_guard<std::mutex> lock(mSyncLock);
        getLocNetClientStateSet(profileId, state).erase(client);
    }

    inline void addToInitStateClientSet(const SlotId subId, const string client) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        auto &initStateClientSet = getInitStateClientSet(subId);
        auto it = initStateClientSet.find(client);
        if (it != initStateClientSet.end()) {
            LOC_LOGd("Client %s Exists", (*it));
            return;
        }
        initStateClientSet.emplace(client);
        LOC_LOGd("client %s added to initStateClientSet", client.c_str());
    }

    inline void removeFromInitStateClientSet(const SlotId subId, const string client) {
        std::lock_guard<std::mutex> lock(mSyncLock);
        getInitStateClientSet(subId).erase(client);
        LOC_LOGd("client %s removed from InitStateClientSet", client.c_str());
    }

    LocNetTelSdkProfileState processIDataCallObject(LocNetTelSdkProfileState& state,
        const std::shared_ptr<IDataCall> &dataCall);

    /** @brief
        DeRegister Data Listener with TelSdk <br/>
        @param subId: slot id for data connection
        @return true, on success
        @return false, on failure
    */
    bool doDeregisterListener(const SlotId subId);

private:
    /** @brief
        Initialize Locnetelsdk <br/>
        @param subId: slot id for data connection
        @return LocNetTelSdkState
    */
    LocNetTelSdkState doLocNetTelSdkInit(const SlotId subId);
    /** @brief
        Client specific init with mapped Profile <br/>
        @param client: pointer to a LocNetTelSdkClient
        @return LocNetClientState
    */
    LocNetClientState doLocNetClientSpecificInit(LocNetTelSdkClient* client);
    /** @brief
        Get the data connection manager from TelSdk <br/>
        @param subId: slot id for data connection
        @param locNetInitResp: lambda for handling the response
        @return LocNetTelSdkState
    */
    LocNetTelSdkState getDataConnMgr(const SlotId subId,
        const LocNetTelSdkInitResponseCb& locNetInitResp);
    /** @brief
        Get the data profile manager from TelSdk <br/>
        @param subId: slot id for data connection
        @param locNetInitResp: lambda for handling the response
        @return LocNetTelSdkState
    */
    LocNetTelSdkState getDataProfileMgr(const SlotId subId,
        const LocNetTelSdkInitResponseCb& locNetInitResp);
    /** @brief
        Get the Data serving system manager with TelSdk <br/>
        @param subId: slot id for data connection
        @return LocNetTelSdkState
    */
    LocNetTelSdkState getDataServingSystemMgr(const SlotId subId);
    /** @brief
        Register a Data listener with TelSdk <br/>
        @param subId: slot id for data connection
        @return true, on success
        @return false, on failure
    */
    bool doRegisterListener(const SlotId subId);
    /** @brief
        Add a client to LocNetTelSdkIface <br/>
        @param cb: status callback provided by client
        @param client: pointer to LocNetClientContext
    */
    void addClient(const LocNetClientContext* client, const StatusCb cb);
    /** @brief
        Remove a client from LocNetTelSdkIface list <br/>
        @param client: client to be removed
    */
    void removeClient(const string& client);
    /** @brief
        Update the params of exxisting client <br/>
        @param client: pointer to existing LocNetClientContext
        @param newClientCtx: pointer to new LocNetClientContext
        @param cb : status callback provided by client
    */
    void updateClient(LocNetTelSdkClient *client,
        const LocNetClientContext *newClientCtx, const StatusCb cb);

private:
    LocNetExt *mExtIfaceInstance;
    /* For binding with Condition variable */
    std::mutex mLock;
    /* To protect the writes to class data structures */
    std::mutex mSyncLock;
    std::condition_variable mCond;
    std::vector<LocNetTelSdkClient*> mClients;
    std::map<SlotId, LocNetTelSdkSlotDataCtx> mDataConnCtxMap;
    std::map<profileIdType, LocNetTelSdkProfileCtx> mProfileContextMap;
};

#endif /* #ifndef LOC_NET_IFACE_TELSDK_H */
