/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocNetTelSdkIfaceLe"

#include "LocNetTelSdkIface.h"
#include <list>
#include <chrono>
#include <future>

IpFamilyType convertIpType(LocApnIpType ipType) {
    IpFamilyType prefIpType;
    switch (ipType) {
        case LOC_APN_IP_IPV4:
            prefIpType = IpFamilyType::IPV4;
            break;
        case LOC_APN_IP_IPV6:
            prefIpType = IpFamilyType::IPV6;
            break;
        case LOC_APN_IP_IPV4V6:
            prefIpType = IpFamilyType::IPV4V6;
            break;
        default:
            prefIpType = IpFamilyType::IPV4;
            break;
    }
    return prefIpType;
}

SlotId convertSubId(LocSubId sub) {
    SlotId prefSub;
    switch (sub) {
        case LOC_DEFAULT_SUB:
            prefSub = DEFAULT_SLOT_ID;
            break;
        case LOC_PRIMARY_SUB:
            prefSub = SLOT_ID_1;
            break;
        case LOC_SECONDARY_SUB:
            prefSub = SLOT_ID_2;
            break;
        case LOC_TERTIARY_SUB:
            /*Currently unsupported in telux.
              Map it to default Sub
             */
        default:
            prefSub = DEFAULT_SLOT_ID;
            break;
    }
    return prefSub;
}

LocSubId convertToLocSubId(SlotId slot) {
    LocSubId subId;
    switch (slot) {
        case SLOT_ID_1:
            subId = LOC_PRIMARY_SUB;
            break;
        case SLOT_ID_2:
            subId = LOC_SECONDARY_SUB;
            break;
        default:
            subId = LOC_DEFAULT_SUB;
            break;
    }
    return subId;
}

void LocNetTelSdkPhoneListener::onCellInfoListChanged(int phoneId,
        std::vector<std::shared_ptr<CellInfo>> cellInfoList) {
    std::lock_guard<std::mutex> lock(mPhoneMgr.getLocNetPhoneManagerMutex());
    auto &phoneCtx = mPhoneMgr.getPhoneManagerCtxMap();
    LocNetNwInfo retNwInfo;
    auto const &notifyCb =  mPhoneMgr.getNotifySubscriberCb();
    bool retVal = false;

    retVal = mPhoneMgr.processCellInfo(cellInfoList, retNwInfo);
    if (retVal) {
        uint8_t flag = 0;
        /* We need to update the NwInfo */
        if (retNwInfo.cellType == phoneCtx[phoneId].nwInfo.cellType) {
            if (retNwInfo.mccMncInfo.mcc != phoneCtx[phoneId].nwInfo.mccMncInfo.mcc) {
                phoneCtx[phoneId].nwInfo.mccMncInfo.mcc = retNwInfo.mccMncInfo.mcc;
                flag |= (1 << LOCNET_TELSDK_MCC_UPDATED_BIT);
            }

            if (retNwInfo.mccMncInfo.mnc != phoneCtx[phoneId].nwInfo.mccMncInfo.mnc) {
                phoneCtx[phoneId].nwInfo.mccMncInfo.mnc = retNwInfo.mccMncInfo.mnc;
                flag |= (1 << LOCNET_TELSDK_MNC_UPDATED_BIT);
            }

            if (retNwInfo.tacInfo != phoneCtx[phoneId].nwInfo.tacInfo) {
                phoneCtx[phoneId].nwInfo.tacInfo = retNwInfo.tacInfo;
                flag |= (1 << LOCNET_TELSDK_TAC_UPDATED_BIT);
            }
        } else {
            /* CellType changed update and notify */
            phoneCtx[phoneId].nwInfo = retNwInfo;
            flag |= (1 << LOCNET_TELSDK_MCC_UPDATED_BIT);
            flag |= (1 << LOCNET_TELSDK_MNC_UPDATED_BIT);
            flag |= (1 << LOCNET_TELSDK_TAC_UPDATED_BIT);
        }
        /* NotifySubscribers */
        notifyCb(flag, phoneId);
    }
}

LocNetTelSdkPhoneManager::LocNetTelSdkPhoneManager():
        mRetryTimer(LOCNET_PHONE_MANAGER_RETRY_TIMER), mRetryCount(0) {
    mPhoneListener = std::make_shared<LocNetTelSdkPhoneListener>(*this);

    locNetRunnable phnMgrRetryRunable = [&] {
        mRetryCount++;
        LOC_LOGd("Phone Manager retry count %d", mRetryCount);
        if (mRetryCount > LOCNET_PHONE_MANAGER_RETRY_TIMES) {
            LOC_LOGd("Get Phone Manager retry attempst exhausted");
            return;
        }
        getPhoneManager();
    };

    mRetryTimer.set(LOCNET_PHONE_MANAGER_RETRY_TIME_IN_SEC, phnMgrRetryRunable);

    mNotifySubscribers  = [this] (const uint8_t flag, const int phoneId) {
        std::map<string, NotifyItemCb>::iterator it = mClientNotifyMap.begin();
        for (; it != mClientNotifyMap.end(); ++it) {
            if (!(it->first.empty())) {
                auto cb = it->second;
                LocSubId sub = convertToLocSubId(getSlotIdMappedPhoneId(phoneId));
                LocNetNetworkInfo nwInfo;

                if (flag & (1 << LOCNET_TELSDK_MCC_UPDATED_BIT) ||
                        flag & (1 << LOCNET_TELSDK_MNC_UPDATED_BIT)) {
                    nwInfo.mccMncInfo = mPhoneidCtxMap[phoneId].nwInfo.mccMncInfo;
                }

                if (flag & (1 << LOCNET_TELSDK_TAC_UPDATED_BIT)) {
                    nwInfo.tac = mPhoneidCtxMap[phoneId].nwInfo.tacInfo;
                }
                nwInfo.itemMask = flag;
                /* Execute Callback */
                cb(it->first, &nwInfo, sub);
            }
        }
    };
}

LocNetTelSdkPhoneManager::~LocNetTelSdkPhoneManager() {
    /* Remove Listener */
    telux::common::Status retStatus = mPhoneManager->removeListener(mPhoneListener);
    if (telux::common::Status::SUCCESS == retStatus) {
        LOC_LOGd("remove Listener successfull");
    }
}

void LocNetTelSdkPhoneManager::registerNotifyItem(const NotifyItemCb notifyCb,
        const string client) {
    mClientNotifyMap[client] = notifyCb;
    if (NULL == mPhoneManager.get() && !mRetryTimer.isStarted()) {
        getPhoneManager();
    }
}

void LocNetTelSdkPhoneManager::getPhoneManager() {
    /* Get the Iphone manager */
    auto phoneManager = PhoneFactory::getInstance().getPhoneManager();
    if (NULL == phoneManager.get()) {
        LOC_LOGe("No Phone Manager instance returned");
        return;
    }

    mPhoneManager = phoneManager;
    bool isPhoneManagerReady = mPhoneManager->isSubsystemReady();

    auto doRetry = [this] {
        mPhoneManager.reset();
        mRetryTimer.start();
    };

    if (isPhoneManagerReady) {
        initPhoneManager(SLOT_ID_1);
        initPhoneManager(SLOT_ID_2);
        /* register listener */
        telux::common::Status retStatus = mPhoneManager->registerListener(mPhoneListener);
        if (telux::common::Status::SUCCESS != retStatus) {
            LOC_LOGd("register Phone Listener failed");
            doRetry();
            return;
        }
        mRetryCount = 0;
    } else {
        LOC_LOGd("Phone Manager not read.Retry");
        doRetry();
    }
}

bool LocNetTelSdkPhoneManager::processCellInfo(std::vector<std::shared_ptr<CellInfo>>& cellInfoList,
        LocNetNwInfo &nwInfo) {
    bool retVal = true;

    for (auto cellInfo : cellInfoList) {
        if (!cellInfo->isRegistered()) {
            continue;
        }

        nwInfo.cellType = cellInfo->getType();
        LOC_LOGd("CellType %d", (int)nwInfo.cellType);
        switch (nwInfo.cellType) {
            case CellType::LTE: {
                auto lteCellInfo = std::static_pointer_cast<telux::tel::LteCellInfo>(cellInfo);
                nwInfo.mccMncInfo.mcc = lteCellInfo->getCellIdentity().getMcc();
                nwInfo.mccMncInfo.mnc = lteCellInfo->getCellIdentity().getMnc();
                nwInfo.tacInfo = lteCellInfo->getCellIdentity().getTrackingAreaCode();
                break;
            }
            case CellType::WCDMA: {
                auto wcdmaCellInfo = std::static_pointer_cast<telux::tel::WcdmaCellInfo>(cellInfo);
                nwInfo.mccMncInfo.mcc = wcdmaCellInfo->getCellIdentity().getMcc();
                nwInfo.mccMncInfo.mnc = wcdmaCellInfo->getCellIdentity().getMnc();
                nwInfo.tacInfo = wcdmaCellInfo->getCellIdentity().getLac();
                break;
            }
            case CellType::TDSCDMA: {
                auto tdscdmaCellInfo = \
                    std::static_pointer_cast<telux::tel::TdscdmaCellInfo>(cellInfo);
                nwInfo.mccMncInfo.mcc = tdscdmaCellInfo->getCellIdentity().getMcc();
                nwInfo.mccMncInfo.mnc = tdscdmaCellInfo->getCellIdentity().getMnc();
                nwInfo.tacInfo = tdscdmaCellInfo->getCellIdentity().getLac();
                break;
            }
            case CellType::GSM: {
                auto gsmCellInfo = std::static_pointer_cast<telux::tel::GsmCellInfo>(cellInfo);
                nwInfo.mccMncInfo.mcc = gsmCellInfo->getCellIdentity().getMcc();
                nwInfo.mccMncInfo.mnc = gsmCellInfo->getCellIdentity().getMnc();
                nwInfo.tacInfo = gsmCellInfo->getCellIdentity().getLac();
                break;
            }
            default:
                LOC_LOGd("CellType not supported");
                retVal = false;
                break;
        }
        LOC_LOGd("Mcc %d Mnc %d Tac %d", nwInfo.mccMncInfo.mcc, nwInfo.mccMncInfo.mnc,
                nwInfo.tacInfo);
        break;
    }
    return retVal;
}

void LocNetTelSdkPhoneManager::initPhoneManager(const SlotId subId) {
    if (INVALID_SLOT_ID == subId) {
        LOC_LOGe("Invalid sub id input");
        return;
    }

    /* Get the Phone Id for the input SubID */
    int phoneId = mPhoneManager->getPhoneIdFromSlotId(subId);
    mSlotPhoneIdMap[subId] = phoneId;

    /* get Iphone for phone ID */
    auto phone = mPhoneManager->getPhone(phoneId);
    if (NULL == phone.get()) {
        LOC_LOGe("No Iphone Instance returned");
        return;
    }

    mPhoneidCtxMap[phoneId].iPhone = phone;
    /* request cellInfo */
    telux::common::ErrorCode retError;
    bool retCode = false;
    LocNetNwInfo retNwInfo;

    auto cellInfoCb = [&] (std::vector<std::shared_ptr<CellInfo>> cellInfoList,
            telux::common::ErrorCode error) {
        std::lock_guard<std::mutex> lock(mLock);
        retError = error;
        if (telux::common::ErrorCode::SUCCESS == error) {
        retCode = processCellInfo(cellInfoList, retNwInfo);
        }
        mCond.notify_all();
    };

    std::unique_lock<std::mutex> uLock(mLock);
    telux::common::Status retStatus = phone->requestCellInfo(cellInfoCb);
    if (telux::common::Status::SUCCESS == retStatus) {
        /* Do a conditional wait for CellinfoCb */
        std::cv_status cvStatus = mCond.wait_for(uLock,
                std::chrono::seconds(LOCNET_TELSDK_CELLINFO_TIMEOUT_IN_SEC));

        if (std::cv_status::timeout == cvStatus ||
                telux::common::ErrorCode::SUCCESS != retError) {
            LOC_LOGe("Request CellinfoCb timedout or error %d", retError);
            if (!mRetryTimer.isStarted()) {
                mRetryTimer.start();
            }
            return;
        }
        /* We have the network info store it to respective phoneid */
        if (retCode) {
            uint8_t flag = 0;
            /* We need to update the NwInfo */
            if (retNwInfo.cellType == mPhoneidCtxMap[phoneId].nwInfo.cellType) {
                if (retNwInfo.mccMncInfo.mcc != mPhoneidCtxMap[phoneId].nwInfo.mccMncInfo.mcc) {
                    mPhoneidCtxMap[phoneId].nwInfo.mccMncInfo.mcc = retNwInfo.mccMncInfo.mcc;
                    flag |= (1 << LOCNET_TELSDK_MCC_UPDATED_BIT);
                }

                if (retNwInfo.mccMncInfo.mnc != mPhoneidCtxMap[phoneId].nwInfo.mccMncInfo.mnc) {
                    mPhoneidCtxMap[phoneId].nwInfo.mccMncInfo.mnc = retNwInfo.mccMncInfo.mnc;
                    flag |= (1 << LOCNET_TELSDK_MNC_UPDATED_BIT);
                }

                if (retNwInfo.tacInfo != mPhoneidCtxMap[phoneId].nwInfo.tacInfo) {
                    mPhoneidCtxMap[phoneId].nwInfo.tacInfo = retNwInfo.tacInfo;
                    flag |= (1 << LOCNET_TELSDK_TAC_UPDATED_BIT);
                }
            } else {
                /* CellType changed update and notify */
                mPhoneidCtxMap[phoneId].nwInfo = retNwInfo;
                flag |= (1 << LOCNET_TELSDK_MCC_UPDATED_BIT);
                flag |= (1 << LOCNET_TELSDK_MNC_UPDATED_BIT);
                flag |= (1 << LOCNET_TELSDK_TAC_UPDATED_BIT);
            }
            LOC_LOGd("NW info collected. Notify subscribers flag %u", flag);
            /* NotifySubscribers */
            mNotifySubscribers(flag, phoneId);
        }
    }
}

void LocNetTelSdkDataProfileListCallback::onProfileListResponse(
        const std::vector<std::shared_ptr<DataProfile>> &profiles, telux::common::ErrorCode error) {
        std::lock_guard<std::mutex> lock(mLocNetTelSdk.getLocNetTelSdkLock());
        mError = error;
        std::vector<DataProfile> profilesVec;
        if (telux::common::ErrorCode::SUCCESS == error) {
            for (auto &profile : profiles) {
                profilesVec.push_back(*profile);
            }
            LOC_LOGd("Loaded %u profiles for sub %u", profilesVec.size(), mSubId);
        }
        mLocNetTelSdk.setLocNetDataProfilesList(mSubId, std::move(profilesVec));
        mLocNetTelSdk.getLocNetTelSdkCond().notify_all();
}

void LocNetTelSdkDataListener::onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall) {
    LocNetTelSdkIface &locNetTelSdkInstance = getLocNetTelSdkInstance();
    SlotId callSubId = dataCall->getSlotId();
    profileIdType callProfileId = static_cast<profileIdType>(dataCall->getProfileId());

    if (LocNetTelSdkState::LOCNET_TELSDK_STATE_READY != \
            locNetTelSdkInstance.getLocNetTelSdkState(callSubId)) {
        LOC_LOGd("Data Listener could not be processed in locNetTelSdk state = %d",
            locNetTelSdkInstance.getLocNetTelSdkState(callSubId));
        return;
    }

    LocNetTelSdkProfileState state = \
        locNetTelSdkInstance.getLocNetProfileDataCallState(callProfileId);
    LocNetTelSdkProfileState retProfileState = \
        locNetTelSdkInstance.processIDataCallObject(state, dataCall);

    LOC_LOGd("Data call object processed. retProfileState = %d", retProfileState);

    switch (retProfileState) {
        case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING: {
            /*Data listener came back with connecting status.
              We remain in this state.Client needs to handle
              this at their end.
             */
            LOC_LOGd("Wait for the Data Listener from TelSdk");
            break;
        }
        case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING: {
            /*Data listener came back with connecting status.
              We remain in this state.Client needs to handle
              this at their end.
             */
            LOC_LOGd("Wait for the Data Listener from TelSdk");
            break;
        }
        case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTED: {
            /*1.Check if any client was added in Connecting set during ongoing
                disconnection.
              2.Isssue a connect backhaul for the first client in the list
            */
            auto &connectingStateSet = locNetTelSdkInstance.getLocNetClientStateSet(
                    callProfileId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING);
            if (!connectingStateSet.empty()) {
                /* Issue a connectBackhaul for the first client */
                auto iter = connectingStateSet.begin();
                string clientName = *iter;
                locNetTelSdkInstance.removeFromProfileStateClientSet(callProfileId,
                        LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING, clientName);
                locNetRunnable connRunable = [&, clientName] {
                    locNetTelSdkInstance.getLocNetExtIface()->connectBackhaul(clientName);
                };
                locNetTelSdkInstance.getLocNetExtIface()->sendMsg(connRunable);
            }
            locNetTelSdkInstance.setLocNetProfileDataCallState(callProfileId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
            break;
        }
        case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY: {

            if (state == retProfileState) {
                /* Unexpected listener executed. Ignore */
                break;
            }

            /*Inform clients of failure*/
            auto iter = locNetTelSdkInstance.getLocNetClientStateSet(callProfileId,
                        state).begin();
            while (iter != locNetTelSdkInstance.getLocNetClientStateSet(callProfileId,
                        state).end()) {
                auto currentIter = iter++;
                if (currentIter->empty()) {
                    continue;
                }
                LocNetTelSdkClient *clt = locNetTelSdkInstance.getLocNetTelSdkClient(*currentIter);
                if (NULL != clt) {
                    clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                    locNetTelSdkInstance.removeFromProfileStateClientSet(callProfileId,
                        state, clt->getClientName());
                }
            }
            locNetTelSdkInstance.setLocNetProfileDataCallState(callProfileId,
                retProfileState);
            /*1.Check if any client was added in Connecting set during ongoing
                disconnection.
              2.Isssue a connect backhaul for the first client in the set
            */
            if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING == state) {
                auto &connectingStateSet = locNetTelSdkInstance.getLocNetClientStateSet(
                        callProfileId,
                        LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING);
                if (!connectingStateSet.empty()) {
                    /* Issue a connectBackhaul for the first client */
                    auto iter = connectingStateSet.begin();
                    string clientName = *iter;
                    locNetTelSdkInstance.removeFromProfileStateClientSet(callProfileId,
                           LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING, clientName);
                    locNetRunnable connRunable = [&, clientName] {
                        locNetTelSdkInstance.getLocNetExtIface()->connectBackhaul(clientName);
                    };
                    locNetTelSdkInstance.getLocNetExtIface()->sendMsg(connRunable);
                }
            }
            break;
        }
        default:
            break;
    }
}

void LocNetTelSdkDataListener::onServiceStatusChange(telux::common::ServiceStatus status) {


    LOC_LOGd("Data Connection Manager service status %d", status);


    mDataConnMgrServiceStatus = status;
}
void LocNetTelSdkDSSManager::updateNwServiceStatusToClients(telux::data::ServiceStatus status) {
    const std::lock_guard<std::mutex> lock(mSyncLock);
    if (INVALID_SLOT_ID == subId) {
        LOC_LOGd("Invalid SubId in listener.Ignore service Status");
        return;
    }
    dssMgrNwServiceState = status;
    LOC_LOGd("Nw RAT %u status %u", status.networkRat, status.serviceState);
    /*1.Get the SubId for which the dataListener is executed.
      2.For all the clients of locNetTelsdk on this subId, send service status.
    */
    const std::vector<LocNetTelSdkClient*>& clientList =
        mLocNetInstance.getLocNetTelSdkClientList();
    if (!clientList.empty()) {
        for (auto clt: clientList) {
            if (NULL != clt && subId == clt->getClientPrefSubId()) {
                LOC_LOGd("client %s subId %d", clt->getClientName().c_str(), subId);
                if (telux::data::DataServiceState::IN_SERVICE == status.serviceState) {
                    clt->executeStatusCb(LOCNET_SERVICE_AVAILABLE, NULL);
                } else {
                    clt->executeStatusCb(LOCNET_SERVICE_UNAVAILABLE, NULL);
                }
            }
        }
     }
}
void LocNetTelSdkDSSManager::updateServiceState(telux::common::ServiceStatus status) {
    const std::lock_guard<std::mutex> lock(mSyncLock);
    dssMgrServiceStatus = status;
    if (telux::common::ServiceStatus::SERVICE_FAILED == status) {
        const std::vector<LocNetTelSdkClient*> &clientList  =
            mLocNetInstance.getLocNetTelSdkClientList();
        if (!clientList.empty()) {
            for (auto clt: clientList) {
                if (NULL != clt && subId == clt->getClientPrefSubId()) {
                    LOC_LOGd("client %s subId %d", clt->getClientName().c_str(), subId);
                    clt->executeStatusCb(LOCNET_SERVICE_UNAVAILABLE, NULL);
                }
            }
         }
    }
}

LocNetTelSdkIface::LocNetTelSdkIface(LocNetExt *instance):
    mExtIfaceInstance(instance) {
    setLocNetTelSdkState(SLOT_ID_1, LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT);
    setLocNetTelSdkState(SLOT_ID_2, LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT);
}

LocNetTelSdkIface::~LocNetTelSdkIface()
{
    auto doStopDataCall = [this] (SlotId subId) {

        auto stopDataCallRespCb = [&] (const std::shared_ptr<IDataCall> &dataCall,
                telux::common::ErrorCode error) {
            /*noop*/
        };

        profileIdType profileId;
        IpFamilyType  profileIp;
        bool isExists = false;
        for (auto elem : mClients) {
            if (subId == elem->getClientPrefSubId()) {
                profileId = elem->getClientProfileId();
                profileIp = elem->getClientIpType();
                isExists = true;
                break;
            }
        }

        if (isExists) {
            LocNetTelSdkProfileState profileState = \
                getLocNetProfileDataCallState(profileId);
            if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED == profileState ||
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING == profileState) {
                getLocNetDataConnManager(subId)->stopDataCall(profileId,
                    profileIp, stopDataCallRespCb, OperationType::DATA_LOCAL);
            }
        }
    };

    /* Reset DataContext for subID 1 */
    doStopDataCall(SLOT_ID_1);
    doDeregisterListener(SLOT_ID_1);
    getLocNetDataServingSystemManager(SLOT_ID_1)->doDeregisterListener();
    getLocNetDataConnManager(SLOT_ID_1).reset();
    getLocNetDataProfileManager(SLOT_ID_1).reset();
    getLocNetDataProfilesList(SLOT_ID_1).clear();

    /* Reset DataContext for subID 2 */
    doStopDataCall(SLOT_ID_2);
    doDeregisterListener(SLOT_ID_2);
    getLocNetDataServingSystemManager(SLOT_ID_2)->doDeregisterListener();
    getLocNetDataConnManager(SLOT_ID_2).reset();
    getLocNetDataProfileManager(SLOT_ID_2).reset();
    getLocNetDataProfilesList(SLOT_ID_2).clear();
    LOC_LOGd("~LocNetTelSdkIface destroyed");
}

void LocNetTelSdkIface::deinitialize(const string& client) {
    LocNetTelSdkClient *inClient = getLocNetTelSdkClient(client);
    if (NULL == inClient) {
        LOC_LOGd("Client not found.Nothing to deInitialize");
        return;
    }

    SlotId subId = inClient->getClientPrefSubId();
    profileIdType profileId = inClient->getClientProfileId();
    IpFamilyType profileIp;
    int countSubIdClients = 0;
    for (auto elem : mClients) {
        if (subId == elem->getClientPrefSubId()) {
            profileIp = elem->getClientIpType();
            countSubIdClients++;
        }
    }

    if (1 == countSubIdClients) {
        auto stopDataCallRespCb = [&] (const std::shared_ptr<IDataCall> &dataCall,
                telux::common::ErrorCode error) {
            /*noop*/
        };
        LocNetTelSdkProfileState profileState = \
            getLocNetProfileDataCallState(profileId);
        if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED == profileState ||
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING == profileState) {
            getLocNetDataConnManager(subId)->stopDataCall(profileId,
                    profileIp, stopDataCallRespCb, OperationType::DATA_LOCAL);
            setLocNetProfileDataCallState(profileId,
                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
        }
        doDeregisterListener(subId);
        getLocNetDataConnManager(subId).reset();
        getLocNetDataProfileManager(subId).reset();
        getLocNetDataProfilesList(subId).clear();
        setLocNetTelSdkState(subId, LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT);
    }

    removeClient(client);
    LOC_LOGd("deinit completed for client %s sub %d", client.c_str(), subId);
}

void LocNetTelSdkIface::addClient(const LocNetClientContext* client, const StatusCb cb) {
    if (NULL != client) {
        LocNetTelSdkClient *clt = getLocNetTelSdkClient(client->client);
        if (clt != NULL) {
            /* Client already exists , need to check if new param provided */
            updateClient(clt, client, cb);
        } else {
            LocNetTelSdkClient *newClt = new LocNetTelSdkClient();
            if (NULL != newClt) {
                std::lock_guard<std::mutex> lock(mSyncLock);
                newClt->setClientApn(client->prefApn);
                newClt->setClientName(client->client);
                newClt->setClientIpType(client->prefIpType);
                newClt->setClientSubId(client->prefSub);
                newClt->setClientStatusCb(cb);
                mClients.push_back(newClt);
                LOC_LOGd("client %s added successfully", newClt->getClientName().c_str());
            }
        }
    }
}

void LocNetTelSdkIface::updateClient(LocNetTelSdkClient *client,
    const LocNetClientContext *newClientCtx, const StatusCb cb) {

    std::lock_guard<std::mutex> lock(mSyncLock);

    if (NULL == client || NULL == newClientCtx) {
        LOC_LOGd("Null input params");
        return;
    }

    bool isUpdated = false;

    /* Check if new values are provided */
    if ((!newClientCtx->prefApn.empty()) &&
            (0 != client->getClientApn().compare(newClientCtx->prefApn))) {
        client->setClientApn(newClientCtx->prefApn);
        isUpdated = true;
    }

    if (client->getClientIpType() != convertIpType(newClientCtx->prefIpType)) {
        client->setClientIpType(newClientCtx->prefIpType);
        isUpdated = true;
    }

    if (client->getClientPrefSubId() != convertSubId(newClientCtx->prefSub)) {
        client->setClientSubId(newClientCtx->prefSub);
        isUpdated = true;
    }

    client->setClientStatusCb(cb);
    /*client params are updated,It means it would probable need to
      work with a new profile. Mark the client state as NOT READY
     */
    if (isUpdated) {
        client->setClientState(LocNetClientState::LOCNET_CLIENT_NOT_READY);
        LOC_LOGd("client %s state updated to %d", client->getClientName(),
            client->getClientState());
    }
}

void LocNetTelSdkIface::removeClient(const string& client) {
    std::lock_guard<std::mutex> lock(mSyncLock);

    std::vector<LocNetTelSdkClient*>::iterator end;
    LocNetTelSdkClient *clt = getLocNetTelSdkClient(client);
    if (NULL != clt) {
        end = std::remove(mClients.begin(), mClients.end(), clt);
        mClients.erase(end, mClients.end());
        LOC_LOGd("Client %s removed successfully", client.c_str());
    }
}

LocNetTelSdkState LocNetTelSdkIface::getDataConnMgr(const SlotId subId,
    const LocNetTelSdkInitResponseCb& locNetInitResp) {

    LocNetTelSdkState retState = getLocNetTelSdkState(subId);

    do {
        telux::common::ServiceStatus initStatus = \
            telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
        /* Get the Data Conn manager Instance */
        auto initCb = [&] (telux::common::ServiceStatus status) {
            std::lock_guard<std::mutex> lock(mLock);
            initStatus = status;
            mCond.notify_all();
        };

        std::shared_ptr<IDataConnectionManager> connMgr = NULL;
        /*IMP INFO: connMgr should be created outside the scope of uLock.
          As when the InitCbTimeout occurs ,uLock will acuire mLock and
          when it breaks it will detsroy the connMgr object from stack.
          The destruction sequence at telsdk gets blocked on initRespCb
          waiting on uLock.Thus creating deadlock.
        */
        std::unique_lock<std::mutex> uLock(mLock);
        connMgr = DataFactory::getInstance().getDataConnectionManager(subId, initCb);

        if (NULL == connMgr.get()) {
            /*TelSdk could not create a connManager .Move State back to Init.
              Inform the clients in init list for this subId and remove
              them from list
             */
            locNetInitResp(LOCNET_GENERAL_ERROR);
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
            break;
        }

        /* Wait for InitCb */
        std::cv_status cvStatus = mCond.wait_for(uLock,
            std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));

        if (std::cv_status::timeout == cvStatus) {
            /*Teldsdk did not execute initCb .Move State back to Init.
              Inform the clients in init list for this subId.
             */
            LOC_LOGd("InitCb response timedout");
            locNetInitResp(LOCNET_GENERAL_ERROR);
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
            break;
        }

        LOC_LOGd("InitCb successfully executed ServiceStatus %d", initStatus);
        /* InitCb executed check the service status */
        switch (initStatus) {
            case telux::common::ServiceStatus::SERVICE_AVAILABLE: {
                setLocNetDataConnManager(subId, connMgr);
                retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING;
                break;
            }
            case telux::common::ServiceStatus::SERVICE_FAILED:
            case telux::common::ServiceStatus::SERVICE_UNAVAILABLE: {
                locNetInitResp(LOCNET_GENERAL_ERROR);
                retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                break;
            }
            default: {
                locNetInitResp(LOCNET_GENERAL_ERROR);
                LOC_LOGd("Invalid Service status %d", initStatus);
                retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                break;
            }
        }
    } while (0);

    return retState;
}

LocNetTelSdkState LocNetTelSdkIface::getDataProfileMgr(const SlotId subId,
    const LocNetTelSdkInitResponseCb& locNetInitResp) {

    LocNetTelSdkState retState = getLocNetTelSdkState(subId);

    do {
        telux::common::ServiceStatus initStatus = \
            telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
        /* Get the Data Conn manager Instance */
        auto initCb = [&] (telux::common::ServiceStatus status) {
            std::lock_guard<std::mutex> lock(mLock);
            initStatus = status;
            mCond.notify_all();
        };

        std::unique_lock<std::mutex> uLock(mLock);
        auto profileMgr = DataFactory::getInstance().getDataProfileManager(subId, initCb);

        if (NULL == profileMgr.get()) {
            /*Teldsdk did not return a valid profile Manager instance
              Inform the clients in init list for this subId
             */
            LOC_LOGd("Get Profile Manager failed sub %d", subId);
            locNetInitResp(LOCNET_GENERAL_ERROR);
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
            break;
        }

        /* Timed Wait for InitCb to be executed. */
        std::cv_status cvStatus = mCond.wait_for(uLock,
                std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));

        uLock.unlock();

        if (std::cv_status::timeout == cvStatus) {
            /*Teldsdk did not execute initCb .Move State back to Init.
              Inform the clients in init list for this subId
             */
            LOC_LOGd("InitCb response timed out for get Profile Manager Sub %d", subId);
            locNetInitResp(LOCNET_GENERAL_ERROR);
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
            break;
        }

        LOC_LOGd("get Profile Manager InitCb response  Status %d", initStatus);
        /* InitCb executed check the service status */
        switch (initStatus) {
            case telux::common::ServiceStatus::SERVICE_AVAILABLE: {
                setLocNetDataProfileManager(subId, profileMgr);
                auto dataProfileListCb = std::make_shared<LocNetTelSdkDataProfileListCallback>(
                    subId, *this);
                /* Request for data profile lists */
                if (NULL == dataProfileListCb.get()) {
                    LOC_LOGd("Profile List callback NULL.Exit here");
                    locNetInitResp(LOCNET_GENERAL_ERROR);
                    retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                    break;
                }

                telux::common::Status retStatus;
                std::unique_lock<std::mutex> uLock(mLock);
                retStatus = profileMgr->requestProfileList(dataProfileListCb);

                if (telux::common::Status::SUCCESS == retStatus) {
                    std::cv_status cvStatus = mCond.wait_for(uLock,
                            std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));

                    if (std::cv_status::timeout == cvStatus ||
                            telux::common::ErrorCode::SUCCESS !=
                                dataProfileListCb->getErrorCode()) {
                        /*Teldsdk did not execute respond back with profiles list .
                          Move State back to Init.
                          Inform the clients in init list for this subId
                         */
                        LOC_LOGd("Error %d or Request timed out",
                            dataProfileListCb->getErrorCode());
                        locNetInitResp(LOCNET_GENERAL_ERROR);
                        retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                        break;
                    }

                    LOC_LOGd("Profile Lists Cb Executed");
                    retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING;
                }
                break;
            }
            case telux::common::ServiceStatus::SERVICE_FAILED:
            case telux::common::ServiceStatus::SERVICE_UNAVAILABLE: {
                /* TODO: Register Profile Listener . Right now we are
                   working with default profile only so not needed as of now
                 */
                locNetInitResp(LOCNET_GENERAL_ERROR);
                retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                break;
            }
            default: {
                locNetInitResp(LOCNET_GENERAL_ERROR);
                LOC_LOGd("Invalid Service status %d", initStatus);
                retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
                break;
            }
        }
    } while (0);
    return retState;
}
LocNetTelSdkState LocNetTelSdkIface::getDataServingSystemMgr(const SlotId subId) {
    LocNetTelSdkState retState = getLocNetTelSdkState(subId);
    do {
        telux::common::ServiceStatus initStatus = \
            telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
        if (nullptr != getLocNetDataServingSystemManager(subId)) {
            auto locNetDssMgr = getLocNetDataServingSystemManager(subId);
            LOC_LOGd("locNtedssmgr created");
            if (locNetDssMgr && !locNetDssMgr->getDssManager().expired() &&
                    telux::common::ServiceStatus::SERVICE_FAILED !=
                    locNetDssMgr->getDssManagerServiceStatus()) {
                if (!locNetDssMgr->getDssListener().expired() &&
                        !dynamic_cast<LocNetTelSdkDSSListener*>(
                        locNetDssMgr->getDssListener().lock().get())->getListenerRegStatus()) {
                    //try registering listener again
                    if (telux::common::Status::SUCCESS ==
                            locNetDssMgr->getDssManager().lock()->registerListener(
                            locNetDssMgr->getDssListener())) {
                        dynamic_cast<LocNetTelSdkDSSListener*>(
                            locNetDssMgr->getDssListener().lock().get())->
                            setListenerRegStatus(true);
                        LOC_LOGd("DSS listener registered successfully");
                    }
                }
                break;
            }
        } else {
            std::unique_ptr<LocNetTelSdkDSSManager> locNetDssMgr(std::move(
                new LocNetTelSdkDSSManager(subId, *this)));
            if (nullptr == locNetDssMgr) {
                LOC_LOGd("LocNetDssMgr could not be created.");
                break;
            }
            setLocNetDataServingSystemManager(subId, locNetDssMgr);
        }
        auto initCb = [this, &initStatus] (telux::common::ServiceStatus status) {
            std::lock_guard<std::mutex> lock(mLock);
            initStatus = status;
            mCond.notify_all();
        };
        std::unique_lock<std::mutex> uLock(mLock);
        auto dataServingSystemMgr = DataFactory::getInstance().getServingSystemManager(
            subId, initCb);
        if (nullptr == dataServingSystemMgr) {
            /*Teldsdk did not return a valid Data serving system
              Manager instance. Do not treat it as fatal.
             */
            LOC_LOGd("Get DSS Manager failed sub %d", subId);
            break;
        }
        auto locNetDssMgr = getLocNetDataServingSystemManager(subId);
        if (nullptr != locNetDssMgr) {
            locNetDssMgr->setDssManager(dataServingSystemMgr);
        }
        std::cv_status cvStatus = mCond.wait_for(uLock,
                std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));
        uLock.unlock();
        if (std::cv_status::timeout == cvStatus) {
            LOC_LOGd("response timed out for DSS Manager Sub %d",
                subId);
            break;
        }
        LOC_LOGd("DSS manager InitCb response  Status %d", initStatus);
        if (nullptr != locNetDssMgr && locNetDssMgr->getDssListener().expired()) {
            auto listener = std::make_shared<LocNetTelSdkDSSListener>(locNetDssMgr);
            LOC_LOGd("listener created");
            if (nullptr != listener) {
                locNetDssMgr->setDssListener(listener);
                if (telux::common::Status::SUCCESS ==
                        dataServingSystemMgr->registerListener(listener)) {
                    listener->setListenerRegStatus(true);
                    LOC_LOGd("DSS listener registered successfully");
                }
            }
        }
        switch (initStatus) {
            case telux::common::ServiceStatus::SERVICE_AVAILABLE: {
                    /* 1. Get the network service state from Data serving system manager
                       2. Register a listener with Data serving system manager.
                    */
                    auto serviceStatusRespCb = [this, locNetDssMgr] (
                            telux::data::ServiceStatus serviceStatus,
                            telux::common::ErrorCode error) {
                        std::lock_guard<std::mutex> lock(mLock);
                        locNetDssMgr->updateNwServiceStatusToClients(serviceStatus);
                        mCond.notify_all();
                    };
                    std::unique_lock<std::mutex> uLock(mLock);
                    telux::common::Status retStatus = dataServingSystemMgr->requestServiceStatus(
                        serviceStatusRespCb);
                    std::cv_status cvStatus = mCond.wait_for(uLock,
                        std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));
                    uLock.unlock();
                    if (std::cv_status::timeout == cvStatus) {
                        LOC_LOGd("response timed out for get service status Sub %d", subId);
                    }
                    if (!locNetDssMgr->getDssListener().expired() &&
                            !dynamic_cast<LocNetTelSdkDSSListener*>(
                            locNetDssMgr->getDssListener().lock().get())->getListenerRegStatus()) {
                        //try registering listener again
                        if (telux::common::Status::SUCCESS ==
                                dataServingSystemMgr->registerListener(
                                locNetDssMgr->getDssListener())) {
                            dynamic_cast<LocNetTelSdkDSSListener*>(
                                locNetDssMgr->getDssListener().lock().get())->
                                setListenerRegStatus(true);
                            LOC_LOGd("DSS listener registered successfully");
                        }
                    }
                }
                break;
            case telux::common::ServiceStatus::SERVICE_UNAVAILABLE:
                LOC_LOGd("DSS Manger service is Unavailable");
                break;
            case telux::common::ServiceStatus::SERVICE_FAILED: {
                    auto dssMgr = moveLocNetDataServingSystemManager(subId);
                }
                break;
            default:
                LOC_LOGd("Invalid Service status %d", initStatus);
                break;
        }
    } while (0);
    return retState;
}

void LocNetTelSdkIface::init(const StatusCb statusCb, const LocNetClientContext *client) {
    addClient(client, statusCb);

    LocNetTelSdkClient *telSdkClient = getLocNetTelSdkClient(client->client);
    if (NULL == telSdkClient) {
        LOC_LOGd("Client registration failed");
        if (NULL != client && NULL != statusCb) {
            statusCb(client->client, LOCNET_GENERAL_ERROR, NULL);
        }
        return;
    }

    /* Get the preferred SubId from Client */
    SlotId subId = telSdkClient->getClientPrefSubId();
    LocNetTelSdkState sdkState = getLocNetTelSdkState(subId);

    auto notifyClients = [] (LocNetClientState state, LocNetTelSdkClient *clt) {
        if (LocNetClientState::LOCNET_CLIENT_READY == state) {
            clt->executeStatusCb(LOCNET_INITIALIZED, NULL);
        } else {
            clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
        }
    };

    LOC_LOGd("sub %d TelSdkState %d", subId, sdkState);
    switch (sdkState) {
        case LocNetTelSdkState::LOCNET_TELSDK_STATE_READY: {
            LocNetClientState clientState = doLocNetClientSpecificInit(telSdkClient);
            telSdkClient->setClientState(clientState);
            notifyClients(clientState, telSdkClient);
            break;
        }
        case LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT: {
            addToInitStateClientSet(subId, telSdkClient->getClientName());
            setLocNetTelSdkState(subId, LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING);
            sdkState = doLocNetTelSdkInit(subId);
            setLocNetTelSdkState(subId, sdkState);
            /* Here the State should have moved to READY */
            if (LocNetTelSdkState::LOCNET_TELSDK_STATE_READY == sdkState) {
                auto it = getInitStateClientSet(subId).begin();
                while (it != getInitStateClientSet(subId).end()) {
                    auto currentIt = it++;
                    LocNetTelSdkClient *waitingClient = (!currentIt->empty()) ? \
                        getLocNetTelSdkClient(*currentIt) : NULL;
                    if (NULL == waitingClient) {
                        continue;
                    }
                    LocNetClientState clientState = doLocNetClientSpecificInit(waitingClient);
                    waitingClient->setClientState(clientState);
                    notifyClients(clientState, waitingClient);
                    removeFromInitStateClientSet(subId, waitingClient->getClientName());
                }
            }
            break;
        }
        case LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING: {
            addToInitStateClientSet(subId, telSdkClient->getClientName());
            break;
        }
        default: {
            LOC_LOGd("Unexpected state %d ", sdkState);
            notifyClients(LocNetClientState::LOCNET_CLIENT_NOT_READY, telSdkClient);
            break;
        }
    }
}

LocNetClientState LocNetTelSdkIface::doLocNetClientSpecificInit(LocNetTelSdkClient* clt) {
    if (NULL == clt) {
        LOC_LOGd("Null input client");
        return LocNetClientState::LOCNET_CLIENT_NOT_READY;
    }

    if (LocNetClientState::LOCNET_CLIENT_READY == \
            clt->getClientState()) {
        LOC_LOGd("Client is Already initialized and Ready");
        return LocNetClientState::LOCNET_CLIENT_READY;
    }

    SlotId prefSubId = clt->getClientPrefSubId();
    LOC_LOGd("sub Id is %u", prefSubId);
    auto profileList = getLocNetDataProfilesList(prefSubId);
    LocNetClientState retState = LocNetClientState::LOCNET_CLIENT_NOT_READY;
    /* Get the Default profile from data conn Manager */
    telux::common::ErrorCode retError;
    int retProfileId;
    SlotId retSlotId;

    auto defProfileRespCb = [&] (int profileId, SlotId slotId,
            telux::common::ErrorCode error) {
        std::lock_guard<std::mutex> lock(mLock);
        retError = error;
        retProfileId = profileId;
        retSlotId = slotId;
        mCond.notify_all();
    };

    LOC_LOGd("Get default profile");
    telux::common::Status retStatus;

    std::unique_lock<std::mutex> uLock(mLock);
    if (NULL == getLocNetDataConnManager(prefSubId).get()) {
        LOC_LOGd("No valid data connection manager exists for sub %d", prefSubId);
        return retState;
    }

    retStatus = getLocNetDataConnManager(prefSubId)->getDefaultProfile(
            OperationType::DATA_LOCAL, defProfileRespCb);

    if (telux::common::Status::SUCCESS != retStatus) {
        LOC_LOGd("GetDefault profile failed sub %d", prefSubId);
        return retState;
    }
    /* do a timed wait for Default Profile resp cb to be executed */
    std::cv_status cvStatus = mCond.wait_for(uLock,
            std::chrono::seconds(LOCNET_TELSDK_INIT_CB_TIMEOUT_IN_SEC));

    if (std::cv_status::timeout == cvStatus ||
            telux::common::ErrorCode::SUCCESS != retError) {
        /*Teldsdk did not execute default ProfileRespCb .
          Move Client State back to Not ready.Inform the client.
         */
        LOC_LOGd("get Default profile Timed out or Error %d occured", retError);
        retState = LocNetClientState::LOCNET_CLIENT_NOT_READY;
        return retState;
    } else {
        LOC_LOGd("Default Profile Id returned is %d", retProfileId);
        if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_NOT_READY ==
                getLocNetProfileDataCallState(retProfileId) ||
                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTED ==
                getLocNetProfileDataCallState(retProfileId)) {
            setLocNetProfileDataCallState(retProfileId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
        }
        clt->setClientProfileId(retProfileId);
        retState = LocNetClientState::LOCNET_CLIENT_READY;
    }

    for (auto &profile: profileList) {
        string profileApn = profile.getApn();
        string prefClientApn = clt->getClientApn();
        IpFamilyType prefClientIpType = clt->getClientIpType();
        IpFamilyType ipType = profile.getIpFamilyType();

        LOC_LOGd("profileApn %s profileIpType %d", profileApn.c_str(), ipType);
        LOC_LOGd("prefApn %s prefClientIp %d ", prefClientApn.c_str(), prefClientIpType);
        if (!profileApn.empty() && !prefClientApn.empty() &&
                (0 == profileApn.compare(prefClientApn)) &&
                (ipType == prefClientIpType)) {
            /* Matching profile found get profile details */
            profileIdType profId = static_cast<profileIdType>(profile.getId());
            LOC_LOGd("Matching profile found. Profile Id = %d", profId);

            setLocNetProfileDataCallState(profId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
            clt->setClientProfileId(profId);
            retState = LocNetClientState::LOCNET_CLIENT_READY;
            return retState;
        }
    }
    return retState;
}

LocNetTelSdkState LocNetTelSdkIface::doLocNetTelSdkInit(const SlotId subId) {
    LocNetTelSdkState retState = getLocNetTelSdkState(subId);

    do {
        if (INVALID_SLOT_ID == subId) {
            LOC_LOGd("Invalid input Sub id %d", subId);
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT;
            break;
        }

        if (LocNetTelSdkState::LOCNET_TELSDK_STATE_READY ==
                getLocNetTelSdkState(subId)) {
            retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_READY;
            break;
        }

        auto initFailResp = [&] (const LocNetStatusType status) {
            auto it = getInitStateClientSet(subId).begin();
            while (it != getInitStateClientSet(subId).end()) {
                auto currentIt = it++;
                if (!currentIt->empty()) {
                    LocNetTelSdkClient *clt = getLocNetTelSdkClient(*currentIt);
                    if (NULL != clt && NULL != clt->getStatusCb()) {
                        clt->executeStatusCb(status, NULL);
                    }
                }
                removeFromInitStateClientSet(subId, *currentIt);
            }
        };
        retState = getDataServingSystemMgr(subId);

        retState = getDataConnMgr(subId, initFailResp);
        if (LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING != retState) {
            LOC_LOGd("Get Data connection Manager for Sub %d failed", subId);
            break;
        }

        retState = getDataProfileMgr(subId, initFailResp);
        if (LocNetTelSdkState::LOCNET_TELSDK_STATE_INIT_ONGOING != retState) {
            LOC_LOGd("Get Data Profile Manager for Sub %d failed", subId);
            break;
        }

        retState = LocNetTelSdkState::LOCNET_TELSDK_STATE_READY;
    } while (0);

    return retState;
}

LocNetTelSdkProfileState LocNetTelSdkIface::processIDataCallObject(LocNetTelSdkProfileState& state,
    const std::shared_ptr<IDataCall> &dataCall) {
    LocNetTelSdkProfileState retProfileState = state;

    do {
        if (NULL == dataCall.get()) {
            /* Nothing to process */
            LOC_LOGd("Null DataCall input");
            break;
        }

        profileIdType callProfileId = static_cast<profileIdType>(dataCall->getProfileId());
        auto const &stateClientSet = getLocNetClientStateSet(callProfileId, state);
        telux::data::DataCallStatus callStatus = dataCall->getDataCallStatus();
        DataBearerTechnology bearerTech = dataCall->getCurrentBearerTech();

        LOC_LOGd("Data Call (status %d) (state %d) (profileId %d) (bearerTech %d)",
            callStatus, state, callProfileId, bearerTech);

        auto doSetCallParams = [&] (profileIdType profileId,
            const std::shared_ptr<IDataCall> &dataCall,
            telux::data::IpFamilyInfo callIpFamilyInfo) {
            LocNetDataCallParams newParams;

            newParams.ip = callIpFamilyInfo.addr.ifAddress;
            if (!newParams.ip.empty()) {
                newParams.validityMask |= LOCNET_DATA_CALL_PARAMS_IP_VALID_BIT;
            }

            newParams.primaryDns = callIpFamilyInfo.addr.primaryDnsAddress;
            if (!newParams.primaryDns.empty()) {
                newParams.validityMask |= LOCNET_DATA_CALL_PARAMS_PRIMARY_DNS_VALID_BIT;
            }

            newParams.secondaryDns = callIpFamilyInfo.addr.secondaryDnsAddress;
            if (!newParams.secondaryDns.empty()) {
                newParams.validityMask |= LOCNET_DATA_CALL_PARAMS_SECONDARY_DNS_VALID_BIT;
            }

            newParams.interfaceName = dataCall->getInterfaceName();
            if (!newParams.interfaceName.empty()) {
                newParams.validityMask |= LOCNET_DATA_CALL_PARAMS_INTERFACE_NAME_VALID_BIT;
            }
            /* Add the params to ProfileParamsMaps */
            setProfileCallParams(profileId, newParams);
        };


        switch (state) {
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING : {
                switch (callStatus) {
                    case telux::data::DataCallStatus::NET_CONNECTED: {

                        LocNetTelSdkProfileState prevState = state;
                        setLocNetProfileDataCallState(callProfileId,
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED);

                        SlotId callSubId = dataCall->getSlotId();
                        telux::data::IpFamilyInfo callIpFamilyInfo;

                        /* check if there are any clients in connecting list */
                        if (stateClientSet.empty()) {
                            /* Force ipv4 as call family here */
                            callIpFamilyInfo = dataCall->getIpv4Info();
                            doSetCallParams(callProfileId, dataCall, callIpFamilyInfo);
                            retProfileState = \
                                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED;
                            break;
                        }
                        auto iter = stateClientSet.begin();
                        while (iter != stateClientSet.end()) {
                            auto currentIter = iter++;
                            LocNetTelSdkClient *inClient = getLocNetTelSdkClient(*currentIter);

                            if (NULL == inClient) {
                                callIpFamilyInfo = dataCall->getIpv4Info();
                                doSetCallParams(callProfileId, dataCall, callIpFamilyInfo);
                                retProfileState = \
                                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED;
                                LOC_LOGw("Null client returned");
                                continue;
                            }
                            IpFamilyType ipType = inClient->getClientIpType();
                            if (IpFamilyType::IPV4 == ipType) {
                                callIpFamilyInfo = dataCall->getIpv4Info();
                            } else if (IpFamilyType::IPV6 == ipType) {
                                callIpFamilyInfo = dataCall->getIpv6Info();
                            } else if (IpFamilyType::IPV4V6 == ipType) {
                                /* Prefer IPv4 */
                                callIpFamilyInfo = dataCall->getIpv4Info();
                            }

                            doSetCallParams(callProfileId, dataCall, callIpFamilyInfo);
                            /* Execute statusCb with Success */
                            inClient->executeStatusCb(LOCNET_CONNECTED,
                                    getProfileCallParams(callProfileId));

                            /* remove clt from connecting state set and add to connected list */
                            removeFromProfileStateClientSet(callProfileId, prevState,
                                    inClient->getClientName());
                            addToProfileStateClientSet(callProfileId,
                                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED,
                                    inClient->getClientName());
                        }
                        retProfileState = \
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED;
                        break;
                    }
                    case telux::data::DataCallStatus::NET_CONNECTING: {
                        /* Remain in connecting state */
                        retProfileState = state;
                        break;
                    }
                    default: {
                        /*Start Data Call din't succeed,return profile to READY state*/
                        retProfileState = LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY;
                        break;
                    }
                };
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED : {
                switch (callStatus) {
                    case telux::data::DataCallStatus::NET_DISCONNECTING:
                    case telux::data::DataCallStatus::NET_NO_NET: {
                        /*Inform clients in connected set with error and
                          move to READY State.
                         */
                        retProfileState = LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY;
                        break;
                    }
                    case telux::data::DataCallStatus::NET_RECONFIGURED: {
                        /*Interface Ip address is changed ,inform clients
                          in connected state list with new value
                         */
                        string callInterfaceName = dataCall->getInterfaceName();
                        IpFamilyInfo callIpFamilyInfo;
                        for (auto &clt : stateClientSet) {
                            LocNetTelSdkClient *inClient = getLocNetTelSdkClient(clt);
                            if (NULL == inClient) {
                                LOC_LOGe("Null inClient input");
                                continue;
                            }
                            IpFamilyType ipType = inClient->getClientIpType();
                            if (IpFamilyType::IPV4 == ipType) {
                                callIpFamilyInfo = dataCall->getIpv4Info();
                            } else if (IpFamilyType::IPV6 == ipType) {
                                callIpFamilyInfo = dataCall->getIpv6Info();
                            } else if (IpFamilyType::IPV4V6 == ipType) {
                                /* taking ipv4 */
                                callIpFamilyInfo = dataCall->getIpv4Info();
                            }

                            doSetCallParams(callProfileId, dataCall, callIpFamilyInfo);
                            inClient->executeStatusCb(LOCNET_CONNECTED,
                                    getProfileCallParams(callProfileId));
                        }
                        retProfileState = \
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED;
                        break;
                    }
                    default:
                        /* Stay in the same state. do nothing */
                        retProfileState = state;
                        break;
                }
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING : {
                switch (callStatus) {
                    case telux::data::DataCallStatus::NET_NO_NET: {
                        /*.Deregister the listener*/
                        doDeregisterListener(dataCall->getSlotId());
                        /*Get the profile id , inform the client of successfull
                          Disconnection. Empty the state set.
                         */
                        auto iter = stateClientSet.begin();
                        while (iter != stateClientSet.end()) {
                            auto currentIter = iter++;
                            LocNetTelSdkClient *inClient = getLocNetTelSdkClient(*currentIter);
                            if (NULL != inClient) {
                                inClient->executeStatusCb(LOCNET_DISCONNECTED, NULL);
                                /* remove clt from disconnecting state set */
                                removeFromProfileStateClientSet(callProfileId,
                                        state, inClient->getClientName());
                            }
                        }
                        retProfileState = \
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTED;
                        break;
                    }
                    case telux::data::DataCallStatus::NET_DISCONNECTING: {
                        retProfileState = \
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING;
                        break;
                    }
                    default: {
                        /* Not expected in this State.Inform clients of error */
                        retProfileState = LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY;
                        break;
                    }
                };
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_NOT_READY :
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY :
            default:
                LOC_LOGd("Ignore listener. profile state = %d", state);
                retProfileState = LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY;
                break;
        };
    } while (0);
    return retProfileState;
}

bool LocNetTelSdkIface::doDeregisterListener(const SlotId subId) {
    auto &connManager = getLocNetDataConnManager(subId);
    if (NULL != connManager.get()) {
        LOC_LOGd("Deregister Data Connection listener for sub %d", subId);
        auto &dataListener = getLocNetDataConnListener(subId);
        if (NULL != dataListener.get()) {
            telux::common::Status retStatus;
            retStatus = connManager->deregisterListener(
                    std::weak_ptr<IDataConnectionListener>(dataListener));
            if (telux::common::Status::SUCCESS == retStatus) {
                dataListener.reset();
                LOC_LOGd("deregistration complete");
                return true;
            }
        }
    }
    return false;
}

bool LocNetTelSdkIface::doRegisterListener(const SlotId subId) {

    if (NULL == getLocNetDataConnListener(subId).get()) {
        auto listener = std::make_shared<LocNetTelSdkDataListener>(*this);
        if (NULL == listener) {
            LOC_LOGe("Null listener returned");
            return false;
        }
        listener->setSubId(subId);
        setLocNetDataConnListener(subId, listener);
    }

    /* Register the listener to subId specific data Connection manager */
    telux::common::Status retStatus;
    retStatus = getLocNetDataConnManager(subId)-> \
        registerListener(getLocNetDataConnListener(subId));

    if (telux::common::Status::SUCCESS != retStatus) {
        LOC_LOGd("Data Listener registration failed for Sub %d", subId);
        /* Register listener not successfull */
        return false;
    }
    return true;
}

bool LocNetTelSdkIface::connectBackhaul(const string& client) {
    bool retVal = false;

    do {
        LocNetTelSdkClient *clt = getLocNetTelSdkClient(client);
        if (NULL == clt) {
            LOC_LOGe("client not found");
            retVal = false;
            break;
        }

        SlotId subId = clt->getClientPrefSubId();

        if ((LocNetTelSdkState::LOCNET_TELSDK_STATE_READY !=
                getLocNetTelSdkState(subId))||
                (LocNetClientState::LOCNET_CLIENT_READY !=
                clt->getClientState())) {
            /* This is unexpected Call from client. execute StatusCb with error */
            clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
            retVal = false;
            break;
        }

        auto notifyFailResp = [&] (const profileIdType profileId,
                const LocNetTelSdkProfileState profileState, const LocNetStatusType status) {
            /* Execute StatusCb for clients in State list */
            auto iter = getLocNetClientStateSet(profileId, profileState).begin();
            while (iter != getLocNetClientStateSet(profileId, profileState).end()) {
                auto currentIter = iter++;
                if (currentIter->empty()) {
                    continue;
                }
                LocNetTelSdkClient *clt = getLocNetTelSdkClient(*currentIter);
                if (NULL != clt) {
                    clt->executeStatusCb(status, NULL);
                    removeFromProfileStateClientSet(profileId, profileState,
                            clt->getClientName());
                }
            }
        };

        /* Get client profile Id */
        profileIdType clientProfileId = clt->getClientProfileId();
        LocNetTelSdkProfileState clientProfileState = \
            getLocNetProfileDataCallState(clientProfileId);

        LOC_LOGd("sub %d profileId %d profilestate %d", subId, clientProfileId,
            clientProfileState);
        switch (clientProfileState) {
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED: {
                LOC_LOGd("Client profile State is Connected,"
                        "Add client to state set, execute StatusCb");
                addToProfileStateClientSet(clientProfileId, clientProfileState,
                        clt->getClientName());
                clt->executeStatusCb(LOCNET_CONNECTED,
                        getProfileCallParams(clientProfileId));
                retVal = true;
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING: {
                /* 1. check if same client is requesting connect again.
                   2. If yes it means Listener has not responded with connected for
                      long time now. Do start data call again.
                   3. If new client , simply add it to connecting state set and return.
                 */
                if (!isClientPresentInProfileStateClientSet(clientProfileId, clientProfileState,
                        clt->getClientName())) {
                    addToProfileStateClientSet(clientProfileId, clientProfileState,
                            clt->getClientName());
                    retVal = true;
                }
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING: {
                /* 1. If this same client is trying to connect again, then we
                      remove it from disconnecting list, add to connecting
                      list. The state Set is expected to have only one client
                      if it is in disconnecting state.
                   2. If some other client , then add that client to connecting
                      state List.When ongoing disconnection completes , process the list
                      and issue a start data call.
                 */
                auto &stateSet = getLocNetClientStateSet(clientProfileId, clientProfileState);
                auto it = stateSet.find(client);
                if (it == stateSet.end()) {
                    retVal = false;
                    break;
                }
                /* check state again for profile id */
                if (clientProfileState == getLocNetProfileDataCallState(clientProfileId)) {
                    /* Still the same i.e listener has not executed */
                    removeFromProfileStateClientSet(clientProfileId, clientProfileState,
                            client);
                    retVal = false;
                    break;
                }
                retVal = true;
                break;
            }
            default:
                break;
        };

        if (retVal) {
            LOC_LOGd("Start Data Call not needed");
            break;
        }

        /* do a registration for DataListener */
        if (!doRegisterListener(subId)) {
            notifyFailResp(clientProfileId, clientProfileState, LOCNET_GENERAL_ERROR);
            setLocNetProfileDataCallState(clientProfileId, clientProfileState);
            retVal = false;
            break;
        }

        /* do a Start data call to TelSdk */
        telux::common::Status retStatus;
        telux::common::ErrorCode retError;
        LocNetTelSdkProfileState retProfileState = clientProfileState;

        auto startDataCallRespCb = [&] (const std::shared_ptr<IDataCall> &dataCall,
                telux::common::ErrorCode error) {
            std::lock_guard<std::mutex> lock(mLock);
            if (NULL != dataCall.get() && (telux::common::ErrorCode::SUCCESS == error ||
                        telux::common::ErrorCode::NO_EFFECT == error)) {
                profileIdType callProfileId = static_cast<profileIdType>(dataCall->getProfileId());
                retProfileState = processIDataCallObject(
                    getLocNetProfileDataCallState(callProfileId), dataCall);
            }
            retError = error;
            mCond.notify_all();
        };

        std::unique_lock<std::mutex> uLock(mLock);
        retStatus = getLocNetDataConnManager(subId)->startDataCall(clientProfileId,
            clt->getClientIpType(), startDataCallRespCb, OperationType::DATA_LOCAL);

        if (telux::common::Status::SUCCESS == retStatus) {
            setLocNetProfileDataCallState(clientProfileId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING);

            addToProfileStateClientSet(clientProfileId,
                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING, clt->getClientName());

            std::cv_status cvStatus = mCond.wait_for(uLock,
                    std::chrono::seconds(LOCNET_TELSDK_CONNECT_CB_TIMEOUT_IN_SEC));

            /*IMP INFO: Here the current thread reacquires the mutex mLock using uLock.
                        so even if startdataCall decides to execute the startDataCallRespCb
                        It should be blocked on uLock at its end. mLock will be released
                        when exit from current loop using break. after that any Cb execution
                        from Telsdk context shall be ignored in READY STATE.
              TODO: For future we can think of redesigning it in way we don;t have to worry
                    about waiting for startDataCallRespCb execution wait.
             */
            if (std::cv_status::timeout == cvStatus ||
                    !(telux::common::ErrorCode::SUCCESS == retError ||
                    telux::common::ErrorCode::NO_EFFECT == retError)) {
                /* Start data Call failed, inform client */
                LOC_LOGd("Start Data call Timed Out or Error %d", retError);
                doDeregisterListener(subId);
                notifyFailResp(clientProfileId, getLocNetProfileDataCallState(clientProfileId),
                        LOCNET_GENERAL_ERROR);
                setLocNetProfileDataCallState(clientProfileId,
                        LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                retVal = false;
                break;
            }

            LOC_LOGd("Data call object processed. retProfileState = %d", retProfileState);
            if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED == retProfileState) {
                retVal = true;
                break;
            } else if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING ==
                    retProfileState) {
                /*Need to wait for listener to be executed.
                  If Telsdk takes too much time to respond back,
                  client should retry call at its end.
                 */
                LOC_LOGd("Wait for the Data Listener from TelSdk");
                retVal = true;
                break;
            } else if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY == retProfileState) {
                /* Inform clients of failure */
                doDeregisterListener(subId);
                notifyFailResp(clientProfileId, getLocNetProfileDataCallState(clientProfileId),
                        LOCNET_GENERAL_ERROR);
                setLocNetProfileDataCallState(clientProfileId, retProfileState);
                retVal = false;
                break;
            }
        } else {
            /* Start data Call failed . inform client */
            doDeregisterListener(subId);
            notifyFailResp(clientProfileId, getLocNetProfileDataCallState(clientProfileId),
                    LOCNET_GENERAL_ERROR);
            setLocNetProfileDataCallState(clientProfileId,
                    LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
            retVal = false;
            break;
        }
    } while (0);

    return retVal;
}

bool LocNetTelSdkIface::disconnectBackhaul(const string& client) {
    bool retVal = false;
    LocNetTelSdkClient *clt = getLocNetTelSdkClient(client);

    do {
        if (NULL == clt) {
            LOC_LOGe("client not found");
            retVal = false;
            break;
        }

        SlotId subId = clt->getClientPrefSubId();

        if ((LocNetTelSdkState::LOCNET_TELSDK_STATE_READY !=
                getLocNetTelSdkState(subId)) ||
                (LocNetClientState::LOCNET_CLIENT_READY !=
                clt->getClientState())) {
            /* This is unexpected Call from client. execute StatusCnb with error */
            clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
            retVal = false;
            break;
        }

        /* Get client profile Id */
        profileIdType clientProfileId = clt->getClientProfileId();
        LocNetTelSdkProfileState curProfileState = \
            getLocNetProfileDataCallState(clientProfileId);

        LOC_LOGd("profileid=%d profile state=%d sub=%d", clientProfileId,
            curProfileState, subId);
        switch (curProfileState) {
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY:
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTED: {

                if (curProfileState != LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY) {
                    /* Check if client is present in current set of profileState */
                    auto  &profileStateSet = getLocNetClientStateSet(clientProfileId,
                            curProfileState);
                    auto it = profileStateSet.find(client);
                    if (it == profileStateSet.end()) {
                        //This request is not expected for this client
                        clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                        retVal = false;
                        break;
                    }

                    if (profileStateSet.size() > 1) {
                        /*Since there are other clients we cannot issue
                          a stop call to telsdk.Respond success to this
                          client and remove it from connected set
                         */
                        removeFromProfileStateClientSet(clientProfileId, curProfileState, client);
                        clt->executeStatusCb(LOCNET_DISCONNECTED, NULL);
                        retVal = true;
                        break;
                    }
                }
                /* Only one client left in Connected list */
                telux::common::Status retStatus;
                telux::common::ErrorCode retErrorCode;
                LocNetTelSdkProfileState retProfileState = curProfileState;

                auto stopDataCallRespCb = [&] (const std::shared_ptr<IDataCall> &dataCall,
                        telux::common::ErrorCode error) {
                    std::lock_guard<std::mutex> lock(mLock);
                    /*ToDo:Currently treating NO EFFECT along with SUCCESS.In future Telsdk
                      should not be sending NO EFFECT
                     */
                    if (NULL != dataCall.get() && (telux::common::ErrorCode::SUCCESS == error ||
                            telux::common::ErrorCode::NO_EFFECT == error)) {
                        profileIdType callProfileId = static_cast<profileIdType>(
                                dataCall->getProfileId());
                        retProfileState = processIDataCallObject(
                                getLocNetProfileDataCallState(callProfileId), dataCall);
                    }
                    retErrorCode = error;
                    mCond.notify_all();
                };

                std::unique_lock<std::mutex> uLock(mLock);
                retStatus = getLocNetDataConnManager(subId)->stopDataCall(clientProfileId,
                        clt->getClientIpType(), stopDataCallRespCb, OperationType::DATA_LOCAL);

                if (telux::common::Status::SUCCESS == retStatus) {
                    /* remove client from Connected state */
                    removeFromProfileStateClientSet(clientProfileId, curProfileState, client);

                    setLocNetProfileDataCallState(clientProfileId,
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING);
                    /* add client to disconnecting state set */
                    addToProfileStateClientSet(clientProfileId,
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING,
                            client);

                    std::cv_status cvStatus = mCond.wait_for(uLock,
                            std::chrono::seconds(LOCNET_TELSDK_CONNECT_CB_TIMEOUT_IN_SEC));

                    /*IMP INFO: Here the current thread reacquires the mutex mLock using uLock.
                                so even if stopdataCall decides to execute the stopDataCallRespCb
                                It should be blocked on uLock at its end. mLock will be released
                                when exit from current loop using break. after that any Cb execution
                                from Telsdk context shall be ignored in READY STATE.
                      TODO: For future we can think of redesigning it in way we don;t have to worry
                            about waiting for stopDataCallRespCb execution wait.
                     */
                    if (std::cv_status::timeout == cvStatus ||
                            telux::common::ErrorCode::SUCCESS != retErrorCode) {
                        /*Response timed out or error was indicated by TelSdk.
                          Inform client with error.Remove the client from
                          disconnecting state set. Change state to READY.
                         */
                        LOC_LOGd("StopRespCb timedout or error %d occured", retErrorCode);
                        clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                        removeFromProfileStateClientSet(clientProfileId,
                                getLocNetProfileDataCallState(clientProfileId),
                                clt->getClientName());

                        setLocNetProfileDataCallState(clientProfileId,
                                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                        retVal = false;
                        break;
                    }

                    LOC_LOGd("StopRespCb executed successfully retProfileState=%d",
                            retProfileState);
                    if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTED ==
                            retProfileState) {
                        LOC_LOGd("profile %d is disconnected Successfully",
                                clientProfileId);
                        /*.Deregister the listener Move the Profile state to ready*/
                        doDeregisterListener(subId);
                        setLocNetProfileDataCallState(clientProfileId,
                                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                        retVal = true;
                        break;
                    } else if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY ==
                            retProfileState) {
                        /* Error .Inform clients.*/
                        clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                        removeFromProfileStateClientSet(clientProfileId,
                                getLocNetProfileDataCallState(clientProfileId),
                                clt->getClientName());
                        setLocNetProfileDataCallState(clientProfileId,
                                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                        retVal = false;
                        break;
                    } else if (LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING ==
                            retProfileState) {
                        /* Wait for NET_NO_NET in DataListener */
                        retVal = true;
                        break;
                    }
                } else {
                    LOC_LOGd("stop data Call returned failure reason %d", retStatus);
                    clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                    setLocNetProfileDataCallState(clientProfileId,
                                LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                    retVal = false;
                }
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_CONNECTING:{
                /* 1.If the Client requesting disconnect is present in connecting
                     Set, remove it.
                   2.If after removing the CLient the list becomes empty, execute a
                     lambda for disconnect call.
                   3.If after (1) above list is not empty, send send success disconnect
                     to this client.
                 */
                auto &connectingStateSet = getLocNetClientStateSet(clientProfileId,
                        curProfileState);
                auto it = connectingStateSet.find(client);

                if (it != connectingStateSet.end()) {
                    removeFromProfileStateClientSet(clientProfileId, curProfileState,
                            client);
                    if (connectingStateSet.empty()) {
                        /* issue a lambda to do disconnectBackhaul */
                        locNetRunnable disconnectRunable = [&, client] {
                            getLocNetExtIface()->disconnectBackhaul(client);
                        };
                        getLocNetExtIface()->sendMsg(disconnectRunable);
                        retVal = true;
                        break;
                    }
                }

                if (!connectingStateSet.empty()) {
                    /* send success to client */
                    clt->executeStatusCb(LOCNET_DISCONNECTED, NULL);
                }
                retVal = true;
                break;
            }
            case LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_DISCONNECTING: {
                /*add client to client state Set, to be informed
                  when disconnecting is complete.
                 */
                 addToProfileStateClientSet(clientProfileId, curProfileState, client);
                 /*If the same client is present in disconnecting state. It means it is
                   probably retrying disconnection.*/
                 retVal = true;
                 break;
            }
            default: {
                /* Unwanted disconnect Backhaul request from client */
                clt->executeStatusCb(LOCNET_GENERAL_ERROR, NULL);
                setLocNetProfileDataCallState(clientProfileId,
                            LocNetTelSdkProfileState::LOCNET_PROFILE_STATE_READY);
                retVal = false;
                break;
            }
        };
    } while (0);

    return retVal;
}
