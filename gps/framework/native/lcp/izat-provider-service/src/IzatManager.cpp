/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Manager

  Copyright (c) 2015-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_IzatManager"
#include <unordered_set>
#include <gps_extended_c.h>
#include "IzatManager.h"
#include "WiFiLocationProvider.h"
#include "WWanLocationProvider.h"
#include "IOSFramework.h"
#include "loc_cfg.h"
#include <IPCHandler.h>

#include "IOSListener.h"
#include "IzatPassiveLocationListener.h"
#include "Wiper.h"
#include "SystemStatus.h"
#include <mutex>
#include <cutils/properties.h>
#include <cstdlib>

namespace izat_manager
{
const std::string IzatManager::mConfFile(LOC_PATH_GPS_CONF);
const std::string IzatManager::mIzatConfFile(LOC_PATH_IZAT_CONF);

using namespace std;

const char mode_flp[] = "ENABLED";
static char conf_flp[LOC_MAX_PARAM_STRING];

static loc_param_s_type izatConfParamTable [] =
{
    {"FUSED_HAL_ONLY", &conf_flp, NULL, 's'}
};

IzatManager* IzatManager::mIzatManager = NULL;
std::mutex IzatManager::mMutex;

IIzatManager* getIzatManager(IOSFramework *pOSFrameworkObj) {
    ENTRY_LOG();
    return static_cast<IIzatManager*>(IzatManager::getInstance(pOSFrameworkObj));
}

void IzatManager::registerWithQesdkProxy() {

    typedef void (registerIzatManager)(izat_manager::IzatManager* izatManager);
    void * qesdkHandle = nullptr;
    LOC_LOGd("loading liblocation_qesdk");
    registerIzatManager* qesdkFnPtr = (registerIzatManager*)dlGetSymFromLib(qesdkHandle,
            "liblocation_qesdk.so", "_Z19registerIzatManagerPN12izat_manager11IzatManagerE");
    if (nullptr != qesdkFnPtr) {
        (*qesdkFnPtr)(this);
    } else {
        LOC_LOGe("loading liblocation_qesdk failed!");
    }
}

void IzatManager::sendPostcard(qc_loc_fw::OutPostcard* card, const char* msgTo) {
    if (card != nullptr) {
        mIzatContext.mIPCMessagingProxyObj->sendMsg(card, msgTo);
        delete card;
        card = nullptr;
    }
}

class GLinkLoader : public LocTimer {
    typedef bool glinkCreaterFn(const MsgTask*,
                                const LBSProxyBase* lbsProxy,
                                IzatManager* izatManager);
    typedef void (deleteGlinkFn)();
    const MsgTask* mMsgTask;
    const LBSProxyBase* mLbsProxy;
    IzatManager* mIzatManager;
    void* mLib;
    glinkCreaterFn* mCreater;
    int32_t mRetriesRemaining;
    deleteGlinkFn* mDeleteGlink;

public:
    GLinkLoader(const MsgTask* msgTask, const LBSProxyBase* lbsProxy, IzatManager* izatManager) :
        mLib(nullptr), mMsgTask(msgTask), mLbsProxy(lbsProxy), mIzatManager(izatManager),
        mCreater((glinkCreaterFn*)dlGetSymFromLib(mLib, "libloc_glink.so", "getLocGlink")),
        mRetriesRemaining(mCreater != nullptr ? 10 : 0),
        mDeleteGlink((deleteGlinkFn*)dlGetSymFromLib(
               mLib, "libloc_glink.so", "deleteLocGlinkInstance")) {
            LOC_LOGI("glink loader mCreater");
            start(50, false);
        }
    ~GLinkLoader() {
        if (mRetriesRemaining <= 0) {
            LOC_LOGI("glink loader destructor");
            if (mLib != nullptr) {
                dlclose(mLib);
            }
        }
    }

    virtual void timeOutCallback() {

        if (mRetriesRemaining > 0) {
            if ((*mCreater)(mMsgTask, mLbsProxy, mIzatManager)) {
                LOC_LOGI("glink loader remaining retries %d", mRetriesRemaining);
                delete this;
            } else {
                mRetriesRemaining--;
                start(5000, false);
            }

        } else {
            LOC_LOGE("glink lib loading failed");
            if (nullptr != mDeleteGlink) {
                //if glink loading or connection fails
                //delete the instance of location glink instance.
                (*mDeleteGlink)();
                mDeleteGlink = nullptr;
                //To avoid race condition, delete the glinkloader after 1 sec
                start(1000, false);
            } else {
                delete this;
            }
        }
    }
};

 IzatManager* IzatManager::getInstance(IOSFramework* pOSFrameworkObj) {
    int result = -1;

    ENTRY_LOG();

    lock_guard<std::mutex> guard(mMutex);
    do {
        // already initialized
        BREAK_IF_NON_ZERO(0, mIzatManager);
        BREAK_IF_ZERO(2, pOSFrameworkObj);

        // for now keep the log levels read to be the first thing
        UTIL_READ_CONF_DEFAULT(IzatManager::mConfFile.c_str());
        UTIL_READ_CONF(IzatManager::mIzatConfFile.c_str(), izatConfParamTable);
        mIzatManager = new IzatManager(pOSFrameworkObj);
        BREAK_IF_ZERO(3, mIzatManager);

        result = 0;
    } while(0);

    if (result != 0) {
        delete mIzatManager;
        mIzatManager = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", result);
    return mIzatManager;
}

IzatManager::IzatManager(IOSFramework* pOSFrameworkObj) :
        mSelfName("IZAT-MANAGER"),
        mOSAgentIPCAddressName("OS-Agent"),
        mContext(LocContext::getLocContext(LocContext::mLocationHalName)),
        mFLPProviderEnabled(false),
        mNLPProviderEnabled(false),
        mEmergencyStatus(false),
        mOptInStatus(false),
        mLocAPI(NULL),
        mWiFiLocationProvider(NULL),
        mWWanLocationProvider(NULL),
        mEsStatusObserver(nullptr),
        mOptInObserver(nullptr),
        mIzatProvider(NULL),
        mIsNonEsOdcpiCallbackValid(false) {
    mIzatContext.mOSFrameworkObj = pOSFrameworkObj;
    mIzatContext.mMsgTask = (MsgTask*)mContext->getMsgTask();
    mIzatContext.mFrameworkActionReqObj = nullptr;

   struct InitMsg : public LocMsg {
        IzatManager& mIzatManager;
        inline InitMsg(IzatManager& im) : mIzatManager(im) {}
        inline virtual void proc() const override {
            // IzatManager initalization
            mIzatManager.init();
        }
    };

    mIzatContext.mMsgTask->sendMsg(new InitMsg(*this));

}

IzatManager::~IzatManager() {
    deinit();
}

void IzatManager::init() {
    int result = -1;
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};
    int aon_len = 0;

    ENTRY_LOG();

    do {
        // init has been called if mQNP is not null
        BREAK_IF_NON_ZERO(100, mIzatContext.mFrameworkActionReqObj);

        // hold the pointer the framework action req object in the s_IzatContext.
        mIzatContext.mFrameworkActionReqObj =
                mIzatContext.mOSFrameworkObj->getFrameworkActionReqObj();
        BREAK_IF_ZERO(1, mIzatContext.mFrameworkActionReqObj);

        // hold the pointer the framework subscription object in the s_IzatContext.
        mIzatContext.mSubscriptionObj =
              mIzatContext.mOSFrameworkObj->getDataItemSubscriptionObj();
        BREAK_IF_ZERO(2, mIzatContext.mSubscriptionObj);

        // initialize and register with the IPCMessagingProxy
        IPCMessagingProxy * ipcMessagingProxy = IPCMessagingProxy::getInstance();
        BREAK_IF_ZERO(3, ipcMessagingProxy);
        mIzatContext.mIPCMessagingProxyObj = ipcMessagingProxy;

        BREAK_IF_NON_ZERO(4, mIzatContext.mIPCMessagingProxyObj
                ->registerResponseObj(mSelfName.c_str(), this));

        // Get the SystemStatus OSObserver instance
        SystemStatus *sysStat = SystemStatus::getInstance(mIzatContext.mMsgTask);
        BREAK_IF_ZERO(5, sysStat);
        IOsObserver* osObsrvr = sysStat->getOsObserver();
        mIzatContext.mSystemStatusOsObsrvr = osObsrvr;
        BREAK_IF_ZERO(6, mIzatContext.mSystemStatusOsObsrvr);
        // give the subscription and frameworkaction request obj to system status
        mIzatContext.mSystemStatusOsObsrvr->
            setSubscriptionObj(mIzatContext.mSubscriptionObj);
        mIzatContext.mSystemStatusOsObsrvr->
            setFrameworkActionReqObj(mIzatContext.mFrameworkActionReqObj);

        typedef void (createXtAdapterInstance)();
        void *handle = nullptr;
        createXtAdapterInstance* xtCreator = (createXtAdapterInstance*)
            dlGetSymFromLib(handle, "libxtadapter.so", "createXtAdapterInstance");
        if (NULL != xtCreator) {
            (*xtCreator)();

            setXtAdapterUserPref* setUserPrefFn = (setXtAdapterUserPref*)
                dlGetSymFromLib(handle, "libxtadapter.so", "setXtAdapterUserPref");
            if (nullptr != setUserPrefFn) {
                mOptInObserver = new OptInObserver(this, setUserPrefFn,
                                                   mIzatContext.mSystemStatusOsObsrvr);
            }
        } else {
            LOC_LOGe("Failed to create XTAdapter");
        }

        if (nullptr == mEsStatusObserver) {
            mEsStatusObserver = new EsStatusObserver(this,
                                                   mIzatContext.mSystemStatusOsObsrvr);
        }

        // init the IPCHandler
        IPCHandler *pIPCHandler =
            IPCHandler :: getInstance(mIzatContext.mIPCMessagingProxyObj,
                                      mIzatContext.mMsgTask);
        BREAK_IF_ZERO(7, pIPCHandler);
        pIPCHandler->reg(mOSAgentIPCAddressName, mIzatContext.mSystemStatusOsObsrvr);

        // create WiFi and WWAN Location provider here and subscribe for location requests.
        mWiFiLocationProvider = WiFiLocationProvider::getInstance(&mIzatContext);
        if (NULL != mWiFiLocationProvider) {
            mWiFiLocationProvider->subscribe(this);
        } else {
            LOC_LOGI("WiFi Location provider is not available");
        }

        mWWanLocationProvider = WWanLocationProvider::getInstance(&mIzatContext);
        if (NULL != mWWanLocationProvider) {
            mWWanLocationProvider->subscribe(this);
            ((WWanLocationProvider*)mWWanLocationProvider)->setDataConnectionCb(
                [this](bool isStart) {
                    for (auto it = mListeners.begin();
                            it != mListeners.end(); ++it) {
                        IzatListenerMask streamType = (*it)->listensTo();
                        if (streamType & IZAT_STREAM_REQUEST_NETWORK) {
                            (*it)->onRequestNetwork(isStart);
                        }
                    }
            });
        } else {
            LOC_LOGI("WWan Location provider is not available");
        }

        // initialize the Wiper
        mWiper = Wiper::getInstance(&mIzatContext);
        BREAK_IF_ZERO(12, mWiper);

        // initialize IzatProvider on KaiOS
        if (NULL == mIzatProvider) {
            void * libHandle = nullptr;
            getIzatProviderInst* getter = (getIzatProviderInst*)
                dlGetSymFromLib(libHandle, "libizatprovider.so", "getIzatProvider");
            if (nullptr != getter) {
                mIzatProvider = (IzatProviderBase*)(*getter)();
            }
            if (mIzatProvider) {
                mIzatProvider->setIzatContext(this, &(this->mIzatContext));
                LOC_LOGD("registerGnssStatusCallbackAsync on KaiOS");
                mIzatContext.mMsgTask->sendMsg(new (nothrow) registerGnssStatusCallbackAsync(this));
                mIzatContext.mMsgTask->sendMsg(new (nothrow) registerIzatOdcpiCallbackAsync(this));
            }
        }

        // Set IzatManager instance to LocQesdkApiService
        registerWithQesdkProxy();

        result = 0;
        aon_len = property_get("ro.vendor.qc_aon_presence", prop_value, NULL);
        LOC_LOGI("aon_len: %d, ro.vendor.qc_aon_presence: %d \n", aon_len, atoi(prop_value));
        if (atoi(prop_value) == 0)
        {
            LOC_LOGi("SLATE is not present: Do not load GLink");
        } else {
            new GLinkLoader(mIzatContext.mMsgTask, mContext->getLBSProxyBase(), this);
        }
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

int IzatManager::deinit() {
    int result = -1;

    ENTRY_LOG();

    do {
        IzatPassiveLocationListener::stopAllPassiveLocationListers(this);

        IPCHandler::destroyInstance();

        mNLPProviderEnabled = false;

        Wiper::destroyInstance();
        mWiper = NULL;

        if (mLocAPI != NULL) {
            mLocAPI->destroy();
            mLocAPI = NULL;
        }

        delete (MsgTask*)mIzatContext.mMsgTask;

        memset(&mIzatContext, 0, sizeof(s_IzatContext));

        if (nullptr != mOptInObserver) {
            delete mOptInObserver;
            mOptInObserver = nullptr;
        }

        if (nullptr != mEsStatusObserver) {
            delete mEsStatusObserver;
            mEsStatusObserver = nullptr;
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

const GnssInterface* IzatManager::getGnssInterface() {
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        void * libHandle = nullptr;
        getLocationInterface* getter = (getLocationInterface*)
                dlGetSymFromLib(libHandle, "libgnss.so", "getGnssInterface");

        if (nullptr == getter) {
            getGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

//IIzatManager overrides
int32 IzatManager::subscribeListener(IOSListener* osLocationListener) {
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, osLocationListener);
        subscribeListenerMsg *msg =
                new (nothrow) subscribeListenerMsg(this, osLocationListener);
        BREAK_IF_ZERO(2, msg);
        mIzatContext.mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::unsubscribeListener(IOSListener* osLocationListener) {
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, osLocationListener);
        unsubscribeListenerMsg *msg =
                new (nothrow) unsubscribeListenerMsg(this, osLocationListener);
        BREAK_IF_ZERO(2, msg);
        mIzatContext.mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

void IzatManager::registerGnssStatusCallback() {
    ENTRY_LOG();
    this->enableProvider(IZAT_STREAM_NETWORK);
    void * libHandle = NULL;
    static registerGnssStatuscb* getter;
    if (NULL == getter) {
        getter = (registerGnssStatuscb*) dlGetSymFromLib(libHandle,
             "android.hardware.gnss-aidl-impl-qti.so", "registerGnssStatusCallback");
    }
    if (NULL != getter) {
        auto gnssStatusCbFunPtr = [ osObsrvr = mIzatContext.mSystemStatusOsObsrvr ](bool status) {
            ENHDataItem enh(status, ENHDataItem::FIELD_CONSENT);
            std::unordered_set<IDataItemCore*> dl({&enh});
            osObsrvr->notify(dl);
        };
        auto nlpReqCbFunPtr = [this](bool status) {
            if (mIzatProvider != NULL) {
                mIzatProvider->setNlpRequestStatus(status);
            }
        };
        (*getter)(gnssStatusCbFunPtr, nlpReqCbFunPtr);
        LOC_LOGd("registerGnssStatusCallback success");
    } else {
        LOC_LOGe("registerGnssStatusCallback failed");
    }
}

void IzatManager::registerIzatOdcpiCallback(OdcpiPrioritytype priority,
        OdcpiCallbackTypeMask typeMask) {
    mGnssInterface = getGnssInterface();
    if (nullptr != mGnssInterface) {
        LOC_LOGD("Registering ODCPI callback, priority: %d, typeMask: %d", priority, typeMask);
        odcpiRequestCallback cb = [this, priority](const OdcpiRequestInfo& odcpiRequest) {
            odcpiRequestCb(odcpiRequest, priority);
        };
        mGnssInterface->odcpiInit(cb, priority, typeMask);
    } else {
        LOC_LOGE("Failed to registering ODCPI callback...");
    }
}

void IzatManager::deRegisterIzatOdcpiCbForNonEmergency() {
    mGnssInterface = getGnssInterface();
    if (nullptr != mGnssInterface) {
        LOC_LOGD("De Registering ODCPI callback for non emergency...");
        mGnssInterface->deRegisterOdcpi(
                OdcpiPrioritytype::ODCPI_HANDLER_PRIORITY_HIGH, NON_EMERGENCY_ODCPI);
    }
}

IzatRequest* IzatManager::createIzatReqest(const OdcpiRequestInfo& odcpiRequest,
        IzatRequest::IzatProviderType provider) {
    ENTRY_LOG();
    return IzatRequest::createCriteriaRequest(IzatHorizontalAccuracy::IZAT_HORIZONTAL_FINE,
            IzatOtherAccuracy:: IZAT_OTHER_HIGH, IzatOtherAccuracy:: IZAT_OTHER_HIGH,
            odcpiRequest.tbfMillis, 0, false, odcpiRequest.isEmergencyMode);
}

void IzatManager::odcpiRequestCb(const OdcpiRequestInfo& odcpiRequest,
        OdcpiPrioritytype priority) {
    ENTRY_LOG();
    LOC_LOGD("Odcpi request in start:0 stop:1: %d, priority: %d", odcpiRequest.type, priority);
    if (ODCPI_HANDLER_PRIORITY_HIGH == priority) {
        if (ODCPI_REQUEST_TYPE_START == odcpiRequest.type) {
            handleNonEsOdcpiReqMsg *msg =
                    new (std::nothrow) handleNonEsOdcpiReqMsg(this);
            mIzatContext.mMsgTask->sendMsg(msg);
        }

    } else if (ODCPI_HANDLER_PRIORITY_MEDIUM == priority) {
        IzatRequest::IzatProviderType provider =
                IzatRequest::IzatProviderType::IZAT_STREAM_NETWORK;
        IzatRequest* request = createIzatReqest(odcpiRequest, provider);
        request->setProvider(provider);
        if (ODCPI_REQUEST_TYPE_START == odcpiRequest.type) {
            addRequest(request);
        } else if (ODCPI_REQUEST_TYPE_STOP == odcpiRequest.type){
            removeRequest(request);
        } else {
            LOC_LOGE("Unsupported ODCPI request..");
        }
    }
}

int32 IzatManager::addRequest(const IzatRequest* request) {
    int result = -1;

    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, request);

        LOC_LOGI ("LocTech-Label :: IZATMANAGER :: Add Request In");
        LOC_LOGI
        (
            "LocTech-Value :: Provider: %d, Num Updates: %d, TTFF: %" PRId64 ", Interval: %"
            PRId64 ", Displacement: %f, Horizontal Accuracy: %d, Altitude Accuracy: %d, "
            "Bearing Accuracy: %d, Emergency mode: %d",
                    request->getProvider (),
                    request->getNumUpdates (),
                    request->getTimeForFirstFix (),
                    request->getInterval (),
                    request->getDistance (),
                    request->getHorizontalAccuracy (),
                    request->getAltitudeAccuracy (),
                    request->getBearingAccuracy (),
                    request->getInEmergency()
        );

        // add request: set true in the msg
        addRemoveRequestMsg *msg =
                new (std::nothrow) addRemoveRequestMsg(this, true, request);
        BREAK_IF_ZERO(3, msg);
        mIzatContext.mMsgTask->sendMsg(msg);

        // Register non emergency ODCPI callback when the request accuracy is high
        // AND the request is not single shot
        if (request->getHorizontalAccuracy() == IzatHorizontalAccuracy::IZAT_HORIZONTAL_FINE &&
                request->getNumUpdates() != 1 && !mIsNonEsOdcpiCallbackValid &&
                mIzatProvider == NULL) {
            registerIzatOdcpiCallback(OdcpiPrioritytype::ODCPI_HANDLER_PRIORITY_HIGH,
                    NON_EMERGENCY_ODCPI);
            mIsNonEsOdcpiCallbackValid = true;
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::removeRequest(const IzatRequest* request) {
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, request);
        LOC_LOGI ("LocTech-Label :: IZATMANAGER :: Remove Request In");
        LOC_LOGI
        (
            "LocTech-Value :: Provider: %d, Num Updates: %d, TTFF: %" PRId64 ", Interval: %"
            PRId64 ", Displacement: %f, Horizontal Accuracy: %d, Altitude Accuracy: %d, "
            "Bearing Accuracy: %d InEmergency: %d, request Type: %d",
                    request->getProvider (),
                    request->getNumUpdates (),
                    request->getTimeForFirstFix (),
                    request->getInterval (),
                    request->getDistance (),
                    request->getHorizontalAccuracy (),
                    request->getAltitudeAccuracy (),
                    request->getBearingAccuracy (),
                    request->getInEmergency(),
                    request->getRequestType()
        );

        // remove request: set false in the msg
        addRemoveRequestMsg *msg =
                new (std::nothrow) addRemoveRequestMsg(this, false, request);
        BREAK_IF_ZERO(3, msg);
        mIzatContext.mMsgTask->sendMsg(msg);

        if (mIsNonEsOdcpiCallbackValid) {
            deRegisterIzatOdcpiCbForNonEmergency();
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

void IzatManager::addRemoveRequestMsg::proc() const {
    int result = -1;

    do {
        // first, send the request to the current network provider
        ILocationProvider::LocationRequest locRequest;
        memset(&locRequest, 0, sizeof(ILocationProvider::LocationRequest));

        if (mIsAddRequest) {
            if (mRequest.getNumUpdates() == 1) {
                locRequest.action = ILocationProvider::LocationRequestAction_SingleShot;
            } else {
                locRequest.action = ILocationProvider::LocationRequestAction_Start;
            }
            locRequest.intervalInMsec = capTouint32(mRequest.getInterval());

            locRequest.emergencyRequest = mRequest.getInEmergency();
            if (mRequest.getHorizontalAccuracy() == IZAT_HORIZONTAL_FINE) {
                locRequest.accuracy = ILocationProvider::LocationRequestAccuracy_High;
            } else {
                locRequest.accuracy = ILocationProvider::LocationRequestAccuracy_Nominal;
            }
        } else {
            locRequest.action = ILocationProvider::LocationRequestAction_Stop;
        }

        if (mRequest.getRequestType() == IZAT_WIFI_BASIC ||
                mRequest.getRequestType() == IZAT_WIFI_PREMIUM) {
            LOC_LOGd("Request Type %d, go to WiFiLocationProvider", mRequest.getRequestType());
            if (mIzatManager->mWiFiLocationProvider == nullptr) {
                LOC_LOGe("WifiLocationProvider is null, check GTP mode.");
                break;
            }
            mIzatManager->mWiFiLocationProvider->setRequest(&locRequest);
            //mIzatManager->mWWanLocationProvider->setRequest(&locRequest);
        }
        if (mRequest.getRequestType() == IZAT_WWAN_BASIC ||
                mRequest.getRequestType() == IZAT_WWAN_PREMIUM) {
            LOC_LOGd("Request Type %d, uid %d, go to WWANProvider", mRequest.getRequestType(),
                    mRequest.getRequestUID());
            locRequest.uid = mRequest.getRequestUID();
            mIzatManager->mWWanLocationProvider->setRequest(&locRequest);
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR(%d, result);
}

void IzatManager::handleNonEsOdcpiReqMsg::proc() const {
    int result = -1;

    ILocationProvider::LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(ILocationProvider::LocationRequest));
    locRequest.action = ILocationProvider::LocationRequestAction_Realign_tbf_Immediate;
    locRequest.accuracy = ILocationProvider::LocationRequestAccuracy_High;
    mIzatManager->mWiFiLocationProvider->setRequest(&locRequest);
    result = 0;

    EXIT_LOG_WITH_ERROR(%d, result);
}

int32 IzatManager::enableProvider(IzatRequest::IzatProviderType provider) {
    ENTRY_LOG();
    mIzatContext.mMsgTask->sendMsg(new (nothrow)enableProviderMsg(this, provider));
    EXIT_LOG_WITH_ERROR("%d", 0);

    return 0;
}

int32 IzatManager::disableProvider(IzatRequest::IzatProviderType provider) {
    ENTRY_LOG();
    mIzatContext.mMsgTask->sendMsg(new (nothrow)disableProviderMsg(this, provider));
    EXIT_LOG_WITH_ERROR("%d", 0);

    return 0;
}

int32 IzatManager::notifyNetworkStatus (const IzatNetworkStatus& status) {
    if (mWWanLocationProvider != nullptr) {
        ((WWanLocationProvider*)mWWanLocationProvider)->notifyNetworkStatus(status);
    }
    return 0;
}

void IzatManager::reportLocation(const LocationReport* report, int locCnt,
    const ILocationProvider* providerSrc)
{
    int result = -1;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, report);
        BREAK_IF_ZERO(2, report[0].isValid());

        // Do not report intermediate fixes to Applications, wait for best fix.
        BREAK_IF_NON_ZERO(0, report[0].mIsIntermediateFix);

        // final fix, report to all the listeners
        TListenerCol::const_iterator it = mIzatManager->mListeners.begin();
        IzatLocation locRpt[locCnt];
        for (int i = 0; i < locCnt; i++) {
            locRpt[i] = report[i];
        }
        while (it != mIzatManager->mListeners.end()) {
            (*it)->onLocationChanged(locRpt, locCnt, IZAT_LOCATION_STATUS_FINAL);
            ++it;
        }

        for (int i = 0; i < locCnt; i++) {
            //Inject NLP position with highest accuracy to Modem for non GMS case
            LOC_LOGD("emergency status %d, fused override %d", mEmergencyStatus, getIzatFusedProviderOverride());
            if (getIzatFusedProviderOverride()) {
                mGnssInterface = getGnssInterface();
                if (nullptr != mGnssInterface && locRpt[i].mPositionSource == WIFI) {
                        mGnssInterface->injectLocation(
                                locRpt[i].mLatitude,
                                locRpt[i].mLongitude,
                                locRpt[i].mHorizontalAccuracy);
                }
            }
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::reportError(const IZatLocationError* /*error*/,
    const ILocationProvider* /*providerSrc*/)
{
}

void IzatManager::registerEsStatusCallback() {
    mGnssInterface = getGnssInterface();
    if (nullptr != mGnssInterface) {
        LOC_LOGD("Registering emergency status callback...");
        std::function<void(bool)> cb = [this](bool isEmergencyMode) {
            reportEsStatus(isEmergencyMode);
        };
        mGnssInterface->setEsStatusCallback(cb);
    } else {
        LOC_LOGE("Failed to registering emergency status callback...");
    }
}

void IzatManager::reportEsStatus(bool isEmergencyMode)
{
    int result = -1;
    ENTRY_LOG();
    LOC_LOGd("current emergency mode:%d, reported emergency mode:%d", mEmergencyStatus,
              isEmergencyMode);
    if (isEmergencyMode != mEmergencyStatus) {
        IzatProviderStatus providerStatus = isEmergencyMode ? IZAT_PROVIDER_EMERGENCY_MODE :
                IZAT_PROVIDER_NONE_EMERGENCY_MODE;
        for (auto it = mIzatManager->mListeners.begin(); it != mIzatManager->mListeners.end();
                ++it) {
            IzatListenerMask streamType = (*it)->listensTo();
            if (streamType & IZAT_STREAM_EMERGENCY_STATUS) {
                (*it)->onStatusChanged(providerStatus);
            }
        }
        result = 0;
        mEmergencyStatus = isEmergencyMode;
    }
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::reportOptinStatus(bool optIn) {
    int result = -1;
    ENTRY_LOG();
    LOC_LOGd("current optIn staus:%d, reported optin status:%d", mOptInStatus, optIn);
    if (optIn != mOptInStatus) {
        IzatProviderStatus providerStatus = optIn ? IZAT_PROVIDER_OPTIN_ENABLED :
                IZAT_PROVIDER_OPTIN_DISABLED;
        for (auto it = mIzatManager->mListeners.begin(); it != mIzatManager->mListeners.end();
                ++it) {
            IzatListenerMask streamType = (*it)->listensTo();
            if (streamType & IZAT_STREAM_OPTIN_STATUS) {
                (*it)->onStatusChanged(providerStatus);
            }
        }
        result = 0;
        mOptInStatus = optIn;
    }
    EXIT_LOG_WITH_ERROR("%d", result);
}


void IzatManager::setIzatFusedProviderOverride(bool izatFused) {
    LOC_LOGd("setIzatFusedProviderOverride : %d", izatFused);
    if (mContext) {
        loc_core::LBSProxyBase* lbsProxy = const_cast<loc_core::LBSProxyBase*>(
                mContext->getLBSProxyBase());
        lbsProxy->setIzatFusedProviderOverride(izatFused);
    }
}

bool IzatManager::getIzatFusedProviderOverride() {
    bool res = false;
    if (mContext) {
        res = mContext->getLBSProxyBase()->getIzatFusedProviderOverride();
    }
    return res;
}

// IIPCMessagingResponse overrides
void IzatManager::handleMsg(InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, in_card);
        const char* from = NULL;
        const char* req = NULL;

        BREAK_IF_NON_ZERO(2, in_card->getString("FROM", &from));
        BREAK_IF_NON_ZERO(3, in_card->getString("REQ", &req));

        LOC_LOGi("FROM = %s", from);
        LOC_LOGi("REQ = %s", req);

        if (strcmp(req, "PASSIVE-LOCATION") == 0) {
            IzatListenerMask listensto = 0x0;
            bool intermediateFixAccepted = false;
            BREAK_IF_NON_ZERO(4, in_card->getUInt16("LISTENS-TO", listensto));
            BREAK_IF_NON_ZERO(5, in_card->getBoolDefault("INTERMEDIATE-FIX-ACCEPTED",
                    intermediateFixAccepted));
            if (0x0 != listensto) {
                mIzatContext.mMsgTask->sendMsg(
                        new (nothrow) startPassiveLocationListenerMsg(this, from, listensto,
                                                                     intermediateFixAccepted));
            }
        } else if (strcmp(req, "PASSIVE-LOCATION-UNSUBSCRIBE") == 0) {
           mIzatContext.mMsgTask->sendMsg(new (nothrow) stopPassiveLocationListenerMsg(this, from));
        } else if (strcmp(req, "NLP-API-CONNECTED") == 0) {
            mIzatContext.mMsgTask->sendMsg(new (nothrow) registerIzatOdcpiCallbackAsync(this));
        } else if (strcmp(req, "ZAXIS-API-CONNECTED") == 0) {
            mIzatContext.mMsgTask->sendMsg(new (nothrow) connectToZaxisProviderAsync(this));
        } else if (0 == strcmp(req, "MAP-DATA")) {
            uint32_t dataSize;
            char data;
            string maString;

            BREAK_IF_NON_ZERO(6, in_card->getUInt32("DATA-SIZE", dataSize));
            LOC_LOGi("dataSize = %d", dataSize);
            for (uint16_t i = 0; i < dataSize; i++) {
                std::string fieldName = "DATA_" + std::to_string(i);
                BREAK_IF_NON_ZERO(7, in_card->getInt8(fieldName.c_str(), data));
                maString += data;
            }
            LBSAdapter *mLBSAdapter = LBSAdapter::get(0);
            if (nullptr != mLBSAdapter) {
                LOC_LOGd("Sending Map Data...");
                mLBSAdapter->sendMapData(maString);
            } else {
                LOC_LOGe("Failed to Send Map Data...");
                result = 8;
                break;
            }
        }
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::printUlpLocation (UlpLocation * ulpLocation,
                                    GpsLocationExtended* locationExtended,
                                    enum loc_sess_status status) {
    if (ulpLocation) {
        LOC_LOGD
        (
            "LocTech-Value :: Latitude: %f, Longitude: %f, Altitude: %f, Speed: %f, Bearing: %f,"
            "Accuracy: %f, UTC Time: %" PRIu64 ", Position Source: %x, Location Session Status %d",
            ulpLocation->gpsLocation.latitude,
            ulpLocation->gpsLocation.longitude,
            ulpLocation->gpsLocation.altitude,
            ulpLocation->gpsLocation.speed,
            ulpLocation->gpsLocation.bearing,
            ulpLocation->gpsLocation.accuracy,
            ulpLocation->gpsLocation.timestamp,
            ulpLocation->position_source,
            status
        );
        LOC_LOGD
        (
            "LocTech-Value :: altitudeMeanSeaLevel: %f, pdop: %f, hdop: %f, vdop: %f,"
            "vert_unc: %f, speed_unc: %f, bearing_unc: %f",
            locationExtended->altitudeMeanSeaLevel,
            locationExtended->pdop,
            locationExtended->hdop,
            locationExtended->vdop,
            locationExtended->vert_unc,
            locationExtended->speed_unc,
            locationExtended->bearing_unc

        );
    }
}

void IzatManager::subscribeListenerMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do {
        pair<TListenerCol::iterator, bool> res =
                mIzatManager->mListeners.insert(mListener);
        BREAK_IF_ZERO(1, res.second); // listener was already in the collection
        mIzatManager->registerLocationAPICb();
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::unsubscribeListenerMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do {
        int found = mIzatManager->mListeners.erase(mListener);
        BREAK_IF_ZERO(1, found); // listener was not present in the collection
        if (nullptr != mListener) {
            delete mListener;
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::enableProviderMsg::proc() const {
    ENTRY_LOG();
    if (IZAT_STREAM_NETWORK == mProvider && !mIzatManager->mNLPProviderEnabled) {
        mIzatManager->mNLPProviderEnabled = true;

        if (NULL != mIzatManager->mWiFiLocationProvider) {
            mIzatManager->mWiFiLocationProvider->enable();
        }
    } else if (IZAT_STREAM_FUSED == mProvider) {
        mIzatManager->mFLPProviderEnabled = true;
    }

    LOC_LOGD("NetworkLocationProvider Enabled = %d, FusedLocationProvider Enabled = %d",
            mIzatManager->mNLPProviderEnabled, mIzatManager->mFLPProviderEnabled);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::disableProviderMsg::proc() const {
    ENTRY_LOG();

    if (IZAT_STREAM_NETWORK == mProvider && mIzatManager->mNLPProviderEnabled) {
        mIzatManager->mNLPProviderEnabled = false;

        if (NULL != mIzatManager->mWiFiLocationProvider) {
            mIzatManager->mWiFiLocationProvider->disable();
        }

    } else if (IZAT_STREAM_FUSED == mProvider) {
        mIzatManager->mFLPProviderEnabled = false;
    }

    LOC_LOGD("NetworkLocationProvider Enabled = %d, FusedLocationProvider Enabled = %d",
            mIzatManager->mNLPProviderEnabled, mIzatManager->mFLPProviderEnabled);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::startPassiveLocationListenerMsg::proc() const {
    ENTRY_LOG();
    mIzatManager->registerLocationAPICb();
    IzatPassiveLocationListener::startPassiveLocationListener(
            mClientName, mListensTo,mIntermediateFixAccepted, mIzatManager,
            mIzatManager->mIzatContext.mIPCMessagingProxyObj);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::stopPassiveLocationListenerMsg::proc() const {
    ENTRY_LOG();
    IzatPassiveLocationListener::stopClientPassiveLocationListening(mClientName, mIzatManager);
    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::registerIzatOdcpiCallbackAsync::proc() const {
    ENTRY_LOG();
    if (mIzatManager->mIzatProvider) {
        mIzatManager->mIzatProvider->registerIzatOdcpiCallback();
    }
    mIzatManager->registerGnssStatusCallback();
    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::connectToZaxisProviderAsync::proc() const {
    ENTRY_LOG();
    if (mIzatManager->mIzatProvider) {
        mIzatManager->mIzatProvider->connectToZProvider();
    }
    EXIT_LOG_WITH_ERROR(%d, 0);
}


void IzatManager::registerGnssStatusCallbackAsync::proc() const {
    ENTRY_LOG();
    mIzatManager->registerGnssStatusCallback();
    EXIT_LOG_WITH_ERROR(%d, 0);
}

// register to receive free location update from both GNSS and FLP stack
void IzatManager::registerLocationAPICb() {

    if (mLocAPI == NULL) {
        mLocAPICb.size = sizeof(mLocAPICb);
        mLocAPICb.trackingCb = nullptr;
        mLocAPICb.batchingCb = nullptr;
        mLocAPICb.geofenceBreachCb = nullptr;
        mLocAPICb.geofenceStatusCb = nullptr;
        mLocAPICb.gnssNiCb = nullptr;
        mLocAPICb.gnssSvCb = nullptr;
        mLocAPICb.gnssNmeaCb =  nullptr;
        mLocAPICb.gnssMeasurementsCb = nullptr;
        mLocAPICb.batchingStatusCb = nullptr;

        // mandatory callback
        mLocAPICb.capabilitiesCb = [this](LocationCapabilitiesMask capabilitiesMask) {
            onCapabilitiesCb(capabilitiesMask);
        };


        // mandatory callback
        mLocAPICb.responseCb = [this](LocationError err, uint32_t id_t) {
            onResponseCb(err, id_t);
        };

        // mandatory callback
        mLocAPICb.collectiveResponseCb = [this](size_t count, LocationError* errs, uint32_t* ids) {
            onCollectiveResponseCb(count, errs, ids);
        };

        // tracking callback, this callback will enable to register
        // with both the GNSS and FLP stacks
        mLocAPICb.trackingCb = [this](const Location& locInfo) {
            onTrackingCb(locInfo);
        };

        mLocAPI = LocationAPI::createInstance(mLocAPICb);
        if (mLocAPI == NULL) {
            LOC_LOGD("registerLocationAPICb: failed to create LocationAPI "
                     "instance for both GNSS stack and FLP stack");
        }
    }
}

void IzatManager::onTrackingCb(const Location& locInfo) {

    // queue the free gnss location in the the msg task
    trackingCbMsg *msg =
        new (std::nothrow) trackingCbMsg(this, locInfo);

    if (msg != NULL) {
        mIzatContext.mMsgTask->sendMsg(msg);
    } else {
        LOC_LOGD("onTrackingCb: failed to create message");
    }
}

void IzatManager::trackingCbMsg::proc() const {
    IzatLocation locReport;
    // timestamp always comes in default
    locReport.mHasUtcTimestampInMsec = true;
    locReport.mUtcTimestampInMsec = mLocation.timestamp;

    locReport.mHasPositionTechMask = true;
    locReport.mPositionTechMask = mLocation.techMask;

    if (mLocation.flags & LOCATION_HAS_LAT_LONG_BIT) {
        locReport.mHasLatitude = true;
        locReport.mLatitude = mLocation.latitude;
        locReport.mHasLongitude = true;
        locReport.mLongitude = mLocation.longitude;
    }

    if (mLocation.flags & LOCATION_HAS_ACCURACY_BIT) {
        locReport.mHasHorizontalAccuracy = true;
        locReport.mHorizontalAccuracy = mLocation.accuracy;
    }

    if (mLocation.flags & LOCATION_HAS_ALTITUDE_BIT) {
        locReport.mHasAltitudeWrtEllipsoid = true;
        locReport.mAltitudeWrtEllipsoid = mLocation.altitude;
    }

    if (mLocation.flags & LOCATION_HAS_VERTICAL_ACCURACY_BIT) {
        locReport.mHasVertUnc = true;
        locReport.mVertUnc = mLocation.verticalAccuracy;
    }

    if (mLocation.flags & LOCATION_HAS_SPEED_BIT) {
        locReport.mHasSpeed = true;
        locReport.mSpeed = mLocation.speed;
    }

    if (mLocation.flags & LOCATION_HAS_SPEED_ACCURACY_BIT) {
        locReport.mHasSpeedUnc = true;
        locReport.mSpeedUnc = mLocation.speedAccuracy;
    }

    if (mLocation.flags & LOCATION_HAS_BEARING_BIT) {
        locReport.mHasBearing = true;
        locReport.mBearing = mLocation.bearing;
    }

    if (mLocation.flags & LOCATION_HAS_BEARING_ACCURACY_BIT) {
        locReport.mHasBearingUnc = true;
        locReport.mBearingUnc = mLocation.bearingAccuracy;
    }

    if (mLocation.flags & LOCATION_HAS_SYSTEM_TICK_BIT) {
        locReport.mHasSystemTick = true;
        locReport.mSystemTick = mLocation.systemTick;
    }

    if (mLocation.flags & LOCATION_HAS_CONFORMITY_INDEX_BIT) {
        locReport.mHasConformityIndex = true;
        locReport.mConformityIndex = mLocation.conformityIndex;
    }

    // final fix, report to all the listeners
    TListenerCol::const_iterator it = mIzatManager->mListeners.begin();
    while (it != mIzatManager->mListeners.end()) {
        IzatListenerMask streamType = (*it)->listensTo();
        // we do not report network location from FLP adapter to the listeners
        // as this may cause the network location to be reported to Android app
        if  ((streamType & IZAT_STREAM_GNSS) &&
             ((locReport.mPositionTechMask & LOC_POS_TECH_MASK_SATELLITE) ||
              (locReport.mPositionTechMask & LOC_POS_TECH_MASK_SENSORS))) {
            (*it)->onLocationChanged(&locReport, 1, IZAT_LOCATION_STATUS_FINAL);
        }
        ++it;
    }
}

} // namespace izat_manager
