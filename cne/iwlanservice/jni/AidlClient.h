/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AIDL_CLIENT_H
#define AIDL_CLIENT_H

#include <jni.h>
#include <android/log.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/IIWlan.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/IIWlanIndication.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/IIWlanResponse.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/QualifiedNetworks.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/BnIWlanIndication.h>
#include <aidl/vendor/qti/hardware/data/iwlandata/BnIWlanResponse.h>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>

#define LOG_V(TAG, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOG_I(TAG, ...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOG_D(TAG, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOG_W(TAG, ...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOG_E(TAG, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


using namespace aidl::vendor::qti::hardware::data::iwlandata;
using namespace aidl::android::hardware::radio;
using namespace aidl::android::hardware::radio::data;

struct DeathCookie {
    void* self;
    int slotId;
};

class CallbackEvent {
public:
    static constexpr const int SERVICE_DEATH = 0;
    static constexpr const int MODEM_SUPPORT_NOT_PRESENT = 1;
    static constexpr const int ACKNOWLEDGE_REQUEST = 2;
    static constexpr const int DATA_REG_STATE_CHANGE_IND = 3;
    static constexpr const int DATA_CALL_LIST_CHANGED = 4;
    static constexpr const int QUALIFIED_NET_CHANGE_IND = 5;
    static constexpr const int UNTHROTTLE_APN = 6;
    static constexpr const int SETUP_DATA_CALL_RESP = 7;
    static constexpr const int DEACTIVATE_DATA_CALL_RESP = 8;
    static constexpr const int GET_DATA_CALL_LIST_RESP = 9;
    static constexpr const int GET_DATA_REG_STATE_RESP = 10;
    static constexpr const int GET_ALL_QUAL_NET_RESP = 11;

    CallbackEvent(int type) { m_Type = type; };


    void setSlotId(int slotId) {
        m_slotId = slotId;
    }
    void setUnthrottleApn(RadioIndicationType radioType, DataProfileInfo dataProfileInfo) {
        m_RadioInd = radioType; m_DataInfo = dataProfileInfo;
    }
    void setAcknowledgeRequest(int32_t serial) {
        m_Serial = serial;
    }
    void setDataRegistrationStateResponse(IWlanResponseInfo info, IWlanDataRegStateResult result) {
        m_Info = info; m_Result = result;
    }
    void setDeactivateDataCallResponse(IWlanResponseInfo info) {
        m_Info = info;
    }
    void setSetupDataCallResponse(RadioResponseInfo in_info, SetupDataCallResult in_dcResponse) {
        m_DataCall = in_dcResponse; m_RadioInfo = in_info;
    }
    void setDataCallListResponse(RadioResponseInfo in_info,
                                 const std::vector<SetupDataCallResult> in_dcResponse) {
        m_RadioInfo = in_info; m_DcResponse = in_dcResponse;
    }
    void setAllQualifiedNetworksResponse(IWlanResponseInfo in_info,
                                         const std::vector<QualifiedNetworks> qualifiedNetworks) {
        m_Info = in_info; m_QualifiedNetworksList = qualifiedNetworks;
    }
    void setDataCallListChanged(RadioIndicationType in_type,
                                const std::vector<SetupDataCallResult>& in_dcList) {
        m_RadioInd = in_type; m_DcResponse = in_dcList;
    }
    void setQualifiedNetworksChangeIndication(
                                const std::vector<QualifiedNetworks>& qualifiedNetworks) {
        m_QualifiedNetworksList = qualifiedNetworks;
    }

    ~CallbackEvent() {};
    int getType() {return m_Type;};
    int getSerial() {return m_Serial;};
    int getSlotId() {return m_slotId;};
    IWlanResponseInfo getIWlanResponseInfo() {return m_Info;};
    IWlanDataRegStateResult getIWlanDataRegStateResult() {return m_Result;};
    DataProfileInfo getDataProfileInfo() {return m_DataInfo;};
    RadioResponseInfo getRadioResponseInfo() {return m_RadioInfo;};
    SetupDataCallResult getSetupDataCallResult() {return m_DataCall;};
    RadioIndicationType getRadioIndicationType() {return m_RadioInd;};
    std::vector<SetupDataCallResult> getDcList() {return m_DcResponse;};
    std::vector<QualifiedNetworks> getQualifiedNetworksList() {return m_QualifiedNetworksList;};

    private:
        int m_Serial, m_Type;
        IWlanResponseInfo m_Info;
        IWlanDataRegStateResult m_Result;
        DataProfileInfo m_DataInfo;
        RadioResponseInfo m_RadioInfo;
        SetupDataCallResult m_DataCall;
        RadioIndicationType m_RadioInd;
        int m_slotId;
        std::vector<SetupDataCallResult> m_DcResponse;
        std::vector<QualifiedNetworks> m_QualifiedNetworksList;
};

class AidlClient;

class IWlanServiceResponse : public BnIWlanResponse
{
    public:
    IWlanServiceResponse(AidlClient *aidlClient);
    int mSlotId;
    virtual ~IWlanServiceResponse();

    ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override;
    ::ndk::ScopedAStatus getDataRegistrationStateResponse(const IWlanResponseInfo& in_info,
                                        const IWlanDataRegStateResult& in_state) override;
    ::ndk::ScopedAStatus deactivateDataCallResponse(const IWlanResponseInfo& in_info) override;
    ::ndk::ScopedAStatus setupDataCallResponse(const RadioResponseInfo& in_info,
                                    const SetupDataCallResult& in_dcResponse) override;
    ::ndk::ScopedAStatus getDataCallListResponse(const RadioResponseInfo& in_info,
                                    const std::vector<SetupDataCallResult>& in_dcResponse) override;
    ::ndk::ScopedAStatus getAllQualifiedNetworksResponse(const IWlanResponseInfo& in_info,
                                const std::vector<QualifiedNetworks>& qualifiedNetworks) override;
    private:
    AidlClient *mAidlClient = nullptr;

};

class IWlanServiceIndication : public BnIWlanIndication
{
    public:
    IWlanServiceIndication(AidlClient *aidlClient);
    int mSlotId;
    virtual ~IWlanServiceIndication();
    ::ndk::ScopedAStatus dataCallListChanged(RadioIndicationType in_type,
                       const std::vector<SetupDataCallResult>& in_dcList) override;
    ::ndk::ScopedAStatus dataRegistrationStateChangeIndication() override;
    ::ndk::ScopedAStatus modemSupportNotPresent() override;
    ::ndk::ScopedAStatus qualifiedNetworksChangeIndication(
                       const std::vector<QualifiedNetworks>& in_qualifiedNetworksList) override;
    ::ndk::ScopedAStatus unthrottleApn(RadioIndicationType in_type,
                       const DataProfileInfo& in_dataProfileInfo) override;

    private:
    AidlClient *mAidlClient = nullptr;
};


class AidlClient {
public:
    static constexpr const char *JAVA_CLASS = "vendor/qti/iwlan/JniHelper";
    static constexpr const char *JAVA_CLASS_CALLBACK = "vendor/qti/iwlan/IWlanAidlClient";
    static constexpr const char *LOGTAG = "IWlanJni";
    AidlClient() = delete;
    AidlClient(int slotId);
    virtual ~AidlClient();
    void setJavaVm(JavaVM* vm) { mVm = vm; };
    bool isThreadAttached(JNIEnv** env);
    void detachThread() {
        mVm->DetachCurrentThread();
    };
    void initialize();
    void setupDataCall(JNIEnv *,jobject, jint, jint, jobject, jboolean, jint,
                       jobjectArray, jobjectArray, jint, jobject, jboolean);
    void deactivateDataCall(JNIEnv *, jobject, jint, jint, jint);
    void getDataCallList(JNIEnv *,jobject,jint);
    void getDataRegistrationState(JNIEnv *, jobject, jint);
    void getAllQualifiedNetworks(JNIEnv *, jobject, jint);
    void responseAcknowledgement(JNIEnv *, jobject);


    jobject getJavaCallbackClass(JNIEnv *env, const char* strmethodName,
                                 const char* methodParams, jmethodID& mId);
    void jniMainThread();
    void pushEv(CallbackEvent& event);
    void unthrottleApn(RadioIndicationType in_type,
                       const DataProfileInfo& in_dataProfileInfo,
                       int slotId);
    void acknowledgeRequest(int32_t in_serial,
                            int slotId);
    void dataRegistrationStateChangeIndication(int slotId);
    void modemSupportNotPresent(int slotId);
    void getDataRegistrationStateResponse(const IWlanResponseInfo& in_info,
                                          const IWlanDataRegStateResult& in_state,
                                          int slotId);
    void deactivateDataCallResponse(const IWlanResponseInfo& in_info,
                                    int slotId);
    void setupDataCallResponse(const RadioResponseInfo& in_info,
                               const SetupDataCallResult& in_dcResponse,
                               int slotId);
    void getDataCallListResponse(const RadioResponseInfo& in_info,
                                 const std::vector<SetupDataCallResult>& in_dcResponse,
                                 int slotId);
    void getAllQualifiedNetworksResponse(const IWlanResponseInfo& in_info,
                                         const std::vector<QualifiedNetworks>& qualifiedNetworks,
                                         int slotId);
    void dataCallListChanged(RadioIndicationType in_type,
                             const std::vector<SetupDataCallResult>& in_dcList,
                             int slotId);
    void qualifiedNetworksChangeIndication(const std::vector<QualifiedNetworks>& qualifiedNetworks,
                                           int slotId);

    private:
    void bindToService();
    void handleEvents();
    void handleEvent(CallbackEvent& event);
    void handleServiceDeath(int slotId);

    int mSlotId;
    std::shared_ptr<IIWlan> mService;
    std::shared_ptr<IWlanServiceIndication> mSvcIndication;
    std::shared_ptr<IWlanServiceResponse> mSvcResponse;
    ndk::ScopedAIBinder_DeathRecipient mDeathRecipient;
    DeathCookie* mDeathCookie;

    JavaVM* mVm;
    std::mutex mEventMutex;
    std::mutex mEventQueueMutex;
    std::condition_variable mEventCv;
    std::queue<CallbackEvent> mEventQueue;
};

#endif //AIDL_CLIENT_H
