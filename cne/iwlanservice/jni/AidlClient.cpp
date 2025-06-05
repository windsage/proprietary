/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android/binder_ibinder_jni.h>
#include <android/binder_parcel.h>
#include <android/binder_parcel_jni.h>
#include <android/binder_parcel_utils.h>
#include "AidlClient.h"
#include <thread>
#include <android/log.h>

static std::unique_ptr<AidlClient> gAidlClient[2];
static jclass jniGlobalRefCls = nullptr;
static jclass jniGlobalParcelCls = nullptr;

using ::aidl::android::hardware::radio::data::DataProfileInfo;
using ::aidl::android::hardware::radio::RadioResponseInfo;
using ::aidl::android::hardware::radio::data::SetupDataCallResult;
using ::aidl::android::hardware::radio::RadioIndicationType;
using ::aidl::android::hardware::radio::data::DataRequestReason;
using ::aidl::android::hardware::radio::data::LinkAddress;
using ::aidl::android::hardware::radio::AccessNetwork;
using ::aidl::android::hardware::radio::data::SliceInfo;
using ::aidl::vendor::qti::hardware::data::iwlandata::QualifiedNetworks;

#define IWLAN_SERVICE_SLOT_0 0
#define IWLAN_SERVICE_SLOT_1 1


static bool isValidSlotId(int slotId)
{
  if (slotId < IWLAN_SERVICE_SLOT_0 || slotId > IWLAN_SERVICE_SLOT_1) {
    return false;
  }
  return true;
}

static void initialize(JNIEnv *, jobject , jint slotId)
{
  ABinderProcess_startThreadPool();
  // Init AidlClient object for slotId
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->initialize();
  }
}

static void setupDataCall(JNIEnv *env, jobject jb, jint slotId, jint jSerial,
                          jint jAccessNetwork, jobject jDataProfileInfo,
                          jboolean jRoamingAllowed, jint jReason, jobjectArray jAddresses,
                          jobjectArray jStr, jint jPduSessionId, jobject jSliceInfo,
                          jboolean jMatchAllRuleAllowed)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->setupDataCall(env, jb, jSerial, jAccessNetwork, jDataProfileInfo,
                                       jRoamingAllowed, jReason, jAddresses, jStr,
                                       jPduSessionId, jSliceInfo, jMatchAllRuleAllowed);
  }
}

static void deactivateDataCall(JNIEnv *env, jobject jb, jint slotId, jint serial, jint cid,
                               jint jReason)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->deactivateDataCall(env, jb, serial, cid, jReason);
  }
}

static void getDataCallList(JNIEnv *env, jobject jb, jint slotId, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->getDataCallList(env, jb, serial);
  }
}

static void getDataRegistrationState(JNIEnv *env, jobject jb, jint slotId, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->getDataRegistrationState(env, jb, serial);
  }
}

static void getAllQualifiedNetworks(JNIEnv *env, jobject jb, jint slotId, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->getAllQualifiedNetworks(env, jb, serial);
  }
}

static void responseAcknowledgement(JNIEnv *env, jobject jb, jint slotId)
{
  #pragma unused (env)
  #pragma unused (jb)
  if (isValidSlotId(slotId) && gAidlClient[slotId]) {
    gAidlClient[slotId]->responseAcknowledgement(env, jb);
  }
}

static JNINativeMethod nMethods[] = {
  {"initialize",
    "(I)V",
    (void*)initialize},
  {"setupDataCall",
    "(IIILandroid/os/Parcel;ZI[Landroid/os/Parcel;[Ljava/lang/String;ILandroid/os/Parcel;Z)V",
    (void*)setupDataCall},
    {"deactivateDataCall",
      "(IIII)V",
      (void*)deactivateDataCall},
  {"getDataCallList",
      "(II)V",
      (void*)getDataCallList},
  {"getDataRegistrationState",
      "(II)V",
      (void*)getDataRegistrationState},
  {"getAllQualifiedNetworks",
      "(II)V",
      (void*)getAllQualifiedNetworks},
  {"responseAcknowledgement",
      "(I)V",
      (void*)responseAcknowledgement},
};

static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* pMethods,
                                 int numMethods)
{
  jclass clazz;
  LOG_D(AidlClient::LOGTAG, "registerNativeMethods");

  clazz = env->FindClass(className);
  if (clazz == NULL) {
    LOG_E(AidlClient::LOGTAG, "Native registration unable to find class '%s'", className);
    return JNI_FALSE;
  }
  if (env->RegisterNatives(clazz, pMethods, numMethods) < 0) {
    LOG_E(AidlClient::LOGTAG, "RegisterNatives failed for '%s'", className);
    return JNI_FALSE;
  }
  jclass jniHelperClass = env->FindClass(AidlClient::JAVA_CLASS_CALLBACK);
  if(jniHelperClass == NULL) {
    LOG_D(AidlClient::LOGTAG, "Java callback class not found");
  }
  jniGlobalRefCls = (jclass)env->NewGlobalRef(jniHelperClass);
  env->DeleteLocalRef(jniHelperClass);
  return JNI_TRUE;
}

static void obtainParcelClass(JNIEnv* env)
{
  const char* const kParcelPathName = "android/os/Parcel";
  jclass clazz = env->FindClass(kParcelPathName);

  if (clazz == NULL)
  {
    LOG_D(AidlClient::LOGTAG, "Find class returned NULL");
  }
  jniGlobalParcelCls = static_cast<jclass>(env->NewGlobalRef(clazz));
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
  JNIEnv* env = NULL;
  LOG_I(AidlClient::LOGTAG, "JNI_OnLoad entry");
  gAidlClient[0] = std::make_unique<AidlClient>(0);
  gAidlClient[0]->setJavaVm(vm);
  gAidlClient[1] = std::make_unique<AidlClient>(1);
  gAidlClient[1]->setJavaVm(vm);
  if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
    LOG_E(AidlClient::LOGTAG, "JNI_OnLoad GetEnv failed");
    return -1;
  }
  if (!registerNativeMethods(env, AidlClient::JAVA_CLASS,
                    nMethods, sizeof(nMethods) / sizeof(nMethods[0]))) {
    LOG_E(AidlClient::LOGTAG, "registerNativeMethods failed");
    return -1;
  }
  obtainParcelClass(env);
  LOG_D(AidlClient::LOGTAG, "JNI_OnLoad exit");
  return JNI_VERSION_1_6;
}

AidlClient::AidlClient(int slotId) : mSlotId(slotId)
{
  mService = nullptr;
  mSvcIndication = nullptr;
  mSvcResponse = nullptr;
  mDeathCookie = nullptr;
  mVm = nullptr;
}

AidlClient::~AidlClient()
{
  mService = nullptr;
  mSvcIndication = nullptr;
  mSvcResponse = nullptr;
  mDeathCookie = nullptr;
  mVm = nullptr;
}

void AidlClient::jniMainThread()
{
  while(true) {
    handleEvents();
    std::unique_lock<std::mutex> lock(mEventMutex);
    mEventCv.wait(lock);
    LOG_D(LOGTAG, "new aidl event arrived");
    lock.unlock();
  }
}

static void deathCallback(void* data)
{
  DeathCookie* dCookie = static_cast<DeathCookie*>(data);
  if (!dCookie) {
    return;
  }
  CallbackEvent newEv(CallbackEvent::SERVICE_DEATH);
  newEv.setSlotId(dCookie->slotId);
  gAidlClient[dCookie->slotId]->pushEv(newEv);
}

void AidlClient::bindToService()
{
  const std::string instance = std::string(IIWlan::descriptor) + (mSlotId == 0 ?
                               "/slot1" : "/slot2");
  if (mService == nullptr) {
    AIBinder* iBinder = AServiceManager_waitForService(instance.c_str());
    ::ndk::SpAIBinder spBinder(iBinder);
    mService = IIWlan::fromBinder(spBinder);

    if(mService == nullptr) {
      LOG_E(AidlClient::LOGTAG, "Unable to bind to AIDL Service");
      return;
    }
    mSvcResponse = ndk::SharedRefBase::make<IWlanServiceResponse>(this);
    mSvcIndication = ndk::SharedRefBase::make<IWlanServiceIndication>(this);
    mDeathCookie = new (std::nothrow) DeathCookie {};
    if (mDeathCookie) {
      mDeathCookie->self = reinterpret_cast<void*>(this);
      mDeathCookie->slotId = mSlotId;
    }
    mSvcResponse->mSlotId = mSlotId;
    mSvcIndication->mSlotId = mSlotId;
    mDeathRecipient = ndk::ScopedAIBinder_DeathRecipient(
                      AIBinder_DeathRecipient_new(&deathCallback));
    AIBinder_linkToDeath(mService->asBinder().get(), mDeathRecipient.get(),
                         reinterpret_cast<void*>(mDeathCookie));
    mService->setResponseFunctions(mSvcResponse, mSvcIndication);
  }
  /**
   * let this thread expire.
   * joinThreadPool() the S-AIDL does not seem to like this and crashes.
   * ABinderProcess_joinThreadPool();
   **/
}

jobject AidlClient::getJavaCallbackClass(JNIEnv *env, const char* strmethodName,
                                         const char* methodParams, jmethodID& mId)
{
  LOG_D(AidlClient::LOGTAG, "getJavaCallbackClass start");
  mId = env->GetMethodID(jniGlobalRefCls, strmethodName, methodParams);
  LOG_D(AidlClient::LOGTAG, "getJavaCallbackClass End midIs[%d]", (mId != NULL));
  if (mId == NULL) {
    return NULL;
  }
  return env->AllocObject(jniGlobalRefCls);
}

void AidlClient::handleServiceDeath(jint slot)
{
  LOG_D(AidlClient::LOGTAG, "handleServiceDeath");
  AIBinder_unlinkToDeath(mService->asBinder().get(), mDeathRecipient.get(),
                         reinterpret_cast<void*>(this));
  mService = nullptr;
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env)) {
    LOG_D(AidlClient::LOGTAG, "handleservice death post event to java");
    std::string methodName = "aidlServiceDied";
    std::string parameterList = "(I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                                callback);
    if (jniCallbackObj != NULL) {
              env->CallVoidMethod(jniCallbackObj, callback, (jint)slot);
        LOG_D(AidlClient::LOGTAG, "handleservicedied sent to java");
        env->DeleteLocalRef(jniCallbackObj);
    }
    detachThread();
  }

  bindToService();
}

void AidlClient::initialize()
{
  std::thread jniThread(std::bind(&AidlClient::jniMainThread, this));
  jniThread.detach();
  bindToService();
}

bool AidlClient::isThreadAttached(JNIEnv** env)
{
  if (env == nullptr || mVm == nullptr)
    return false;
  *env = nullptr;
  LOG_D(AidlClient::LOGTAG, "isThreadAttached start");
  int status = mVm->GetEnv(reinterpret_cast<void**>(env), JNI_VERSION_1_6);
  switch(status) {
    case JNI_OK:
      LOG_D(AidlClient::LOGTAG, "jni attached");
      break;
    case JNI_EDETACHED:
      LOG_D(AidlClient::LOGTAG, "jni detached");
      if (JNI_OK != mVm->AttachCurrentThread(env, 0)) {
        LOG_E(AidlClient::LOGTAG, "Failed to attach jni");
        return false;
      }
      LOG_D(AidlClient::LOGTAG, "jni attached");
      break;
    default:
      LOG_E(AidlClient::LOGTAG, "JNI ERROR, %d", status);
      return false;
  }
  return true;
}

void AidlClient::setupDataCall(JNIEnv *env, jobject /*jb*/, jint jSerial, jint jAccessNetwork,
                               jobject jDataProfileInfo, jboolean jRoamingAllowed, jint jReason,
                               jobjectArray jAddresses, jobjectArray jStr, jint jPduSessionId,
                               jobject jSliceInfo, jboolean jMatchAllRuleAllowed)
{
  LOG_D(AidlClient::LOGTAG, "setupDataCall start");
  AccessNetwork aJAccessNetwork = static_cast<AccessNetwork>(jAccessNetwork);
  DataRequestReason aJReason = static_cast<DataRequestReason>(jReason);
  DataProfileInfo mInfo = {};
  SliceInfo mSlice = {};
  std::vector<LinkAddress> aLinkAddress;
  std::vector<std::string> dnsList;
  int arrSize = env->GetArrayLength(jStr);
  for (int i = 0; i < arrSize; i++) {
    jstring finalStr = (jstring) env->GetObjectArrayElement(jStr, i);
    const char *passString = env->GetStringUTFChars(finalStr, NULL);
    std::string pass(passString);
    dnsList.push_back(pass);
    env->ReleaseStringUTFChars(finalStr, passString);
  }

  ndk::ScopedAParcel aInfo(AParcel_fromJavaParcel(env,jDataProfileInfo));
  mInfo.readFromParcel(aInfo.get());
  ndk::ScopedAParcel aSlice(AParcel_fromJavaParcel(env,jSliceInfo));
  mSlice.readFromParcel(aSlice.get());
  jsize arrSizeObj = env->GetArrayLength(jAddresses);
  for (int i = 0; i < arrSizeObj; i++) {
    LinkAddress Laddr;
    ndk::ScopedAParcel aLaddr(AParcel_fromJavaParcel(
                              env, env->GetObjectArrayElement(jAddresses, i)));
    Laddr.readFromParcel(aLaddr.get());
    aLinkAddress.push_back(Laddr);
  }
  if (mService) {
    mService->setupDataCall(jSerial, aJAccessNetwork, mInfo, jRoamingAllowed, aJReason,
                            aLinkAddress, dnsList, jPduSessionId, mSlice, jMatchAllRuleAllowed);
  }
}

void AidlClient::deactivateDataCall(JNIEnv *env, jobject jb, jint serial, jint cid, jint jReason)
{
  #pragma unused (env)
  #pragma unused (jb)
  LOG_D(AidlClient::LOGTAG, "deactivateDataCall start");
  if (mService == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Aidl Service is not present");
  }
  DataRequestReason aReason = static_cast<DataRequestReason>(jReason);
  if (mService) {
    mService->deactivateDataCall(serial, cid, aReason);
  }
}

void AidlClient::getDataCallList(JNIEnv *env, jobject jb, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  LOG_D(AidlClient::LOGTAG, "getDataCallList start");
  if (mService == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Aidl Service is not present");
  }
  if (mService) {
    mService->getDataCallList(serial);
  }
}

void AidlClient::getDataRegistrationState(JNIEnv *env, jobject jb, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  LOG_D(AidlClient::LOGTAG, "getDataRegistrationState start");
  if (mService == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Aidl Service is not present");
  }
  if (mService) {
    mService->getDataRegistrationState(serial);
  }
}

void AidlClient::getAllQualifiedNetworks(JNIEnv *env, jobject jb, jint serial)
{
  #pragma unused (env)
  #pragma unused (jb)
  LOG_D(AidlClient::LOGTAG, "getAllQualifiedNetworks start");
  if (mService == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Aidl Service is not present");
  }
  if (mService) {
    mService->getAllQualifiedNetworks(serial);
  }
}
void AidlClient::responseAcknowledgement(JNIEnv *env, jobject jb)
{
  #pragma unused (env)
  #pragma unused (jb)
  LOG_D(AidlClient::LOGTAG, "responseAcknowledgement start");
  if (mService == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Aidl Service is not present");
  }
  if (mService) {
    mService->responseAcknowledgement();
  }
}

void AidlClient::pushEv(CallbackEvent& event)
{
  mEventQueueMutex.lock();
  mEventQueue.push(event);
  mEventQueueMutex.unlock();
  mEventCv.notify_all();
}

static jobject createJavaParcelObject(JNIEnv* env)
{
  if (jniGlobalParcelCls == nullptr) {
    LOG_D(AidlClient::LOGTAG, "Failed to get Parcel class");
    return nullptr;
  }
  jmethodID obtainMID = env->GetStaticMethodID(jniGlobalParcelCls, "obtain", "()Landroid/os/Parcel;");
  return env->CallStaticObjectMethod(jniGlobalParcelCls, obtainMID);
}

void AidlClient::deactivateDataCallResponse(const IWlanResponseInfo& in_response, int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "deactivateDataCallResponse start");
    std::string methodName = "deactivateDataCallResponse";
    std::string parameterList = "(Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      jobject jParcelResponseInfo = createJavaParcelObject(env);
      AParcel* nativeInResponse = AParcel_fromJavaParcel(env, jParcelResponseInfo);
      binder_status_t result = in_response.writeToParcel(nativeInResponse);
      if (result != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
      }
      AParcel_setDataPosition(nativeInResponse, 0);

      env->CallVoidMethod(jniCallbackObj, callback, jParcelResponseInfo, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      AParcel_delete(nativeInResponse);
    }
    detachThread();
  }
}

void AidlClient::getDataRegistrationStateResponse(const IWlanResponseInfo& in_response,
                                                  const IWlanDataRegStateResult& in_result,
                                                  int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "getDataRegistrationStateResponse start");
    std::string methodName = "getDataRegistrationStateResponse";
    std::string parameterList = "(Landroid/os/Parcel;Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      jobject jParcelResponseInfo = createJavaParcelObject(env);
      AParcel* nativeInResponse = AParcel_fromJavaParcel(env, jParcelResponseInfo);
      binder_status_t result = in_response.writeToParcel(nativeInResponse);
      if (result != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
      }
      AParcel_setDataPosition(nativeInResponse, 0);

      jobject jParcelRegResult = createJavaParcelObject(env);
      AParcel* nativeRegResult = AParcel_fromJavaParcel(env, jParcelRegResult);
      binder_status_t result2 = in_result.writeToParcel(nativeRegResult);
      if (result2 != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result2);
      }
      AParcel_setDataPosition(nativeRegResult, 0);

      env->CallVoidMethod(jniCallbackObj, callback, jParcelResponseInfo,
                          jParcelRegResult, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      AParcel_delete(nativeInResponse);
      AParcel_delete(nativeRegResult);
    }
    detachThread();
  }
}

void AidlClient::modemSupportNotPresent(int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "modemSupportNotPresent start");
    std::string methodName = "modemSupportNotPresent";
    std::string parameterList = "(I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      env->CallVoidMethod(jniCallbackObj, callback, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
    }
    detachThread();
  }
}

void AidlClient::dataRegistrationStateChangeIndication(int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "dataRegistrationStateChangeIndication start");
    std::string methodName = "dataRegistrationStateChangeIndication";
    std::string parameterList = "(I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      env->CallVoidMethod(jniCallbackObj, callback, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
    }
    detachThread();
  }
}

void AidlClient::acknowledgeRequest(int32_t in_serial,
                                    int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "acknowledgeRequest start");
    std::string methodName = "acknowledgeRequest";
    std::string parameterList = "(II)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      env->CallVoidMethod(jniCallbackObj, callback, (jint)in_serial, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
    }
    detachThread();
  }
}

void AidlClient::unthrottleApn(RadioIndicationType in_type,
                               const DataProfileInfo& in_dataProfileInfo,
                               int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "unthrottleApn start");
    std::string methodName = "unthrottleApn";
    std::string parameterList = "(ILandroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
        jobject jParcelDataProfile = createJavaParcelObject(env);
        AParcel* nativeDataProfileInfo = AParcel_fromJavaParcel(env, jParcelDataProfile);
        binder_status_t result = in_dataProfileInfo.writeToParcel(nativeDataProfileInfo);
        if (result != STATUS_OK) {
          LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
        }
        AParcel_setDataPosition(nativeDataProfileInfo, 0);
        env->CallVoidMethod(jniCallbackObj, callback, (jint)in_type, jParcelDataProfile,
                            (jint)slot);
        env->DeleteLocalRef(jniCallbackObj);
        AParcel_delete(nativeDataProfileInfo);
    }
    detachThread();
  }
}

void AidlClient::setupDataCallResponse(const RadioResponseInfo& in_info,
                                       const SetupDataCallResult& in_dcResponse,
                                       int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "setupDataCallResponse start");
    std::string methodName = "setupDataCallResponse";
    std::string parameterList = "(Landroid/os/Parcel;Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      jobject jParcelInfo = createJavaParcelObject(env);
      AParcel* nativeInfo = AParcel_fromJavaParcel(env, jParcelInfo);
      binder_status_t result = in_info.writeToParcel(nativeInfo);
      if (result != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
      }
      AParcel_setDataPosition(nativeInfo, 0);

      jobject jParcelDcResponse = createJavaParcelObject(env);
      AParcel* nativeDcResponse = AParcel_fromJavaParcel(env, jParcelDcResponse);
      binder_status_t result2 = in_dcResponse.writeToParcel(nativeDcResponse);
      if (result2 != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result2);
      }
      AParcel_setDataPosition(nativeDcResponse, 0);

      env->CallVoidMethod(jniCallbackObj, callback, jParcelInfo, jParcelDcResponse, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      AParcel_delete(nativeInfo);
      AParcel_delete(nativeDcResponse);
    }
    detachThread();
  }
}

void AidlClient::getDataCallListResponse(const RadioResponseInfo& in_info,
                                         const std::vector<SetupDataCallResult>& in_dcList,
                                         int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "getDataCallListResponse start");
    std::string methodName = "getDataCallListResponse";
    std::string parameterList = "(Landroid/os/Parcel;[Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(), callback);
    if (jniCallbackObj != NULL) {
      jobject jParcelResInfo = createJavaParcelObject(env);
      AParcel* nativeResInfo = AParcel_fromJavaParcel(env, jParcelResInfo);
      binder_status_t result = in_info.writeToParcel(nativeResInfo);
      if (result != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
      }
      AParcel_setDataPosition(nativeResInfo, 0);

      jobjectArray setupObj = env->NewObjectArray(in_dcList.size(),
                              env->FindClass("android/os/Parcel"), NULL);

      AParcel *parcelArray[in_dcList.size()];
      for (int i = 0; i < in_dcList.size(); i++){
        jobject jParcelSetupRes = createJavaParcelObject(env);
        AParcel* nativeSetupRes = AParcel_fromJavaParcel(env, jParcelSetupRes);
        parcelArray[i] = nativeSetupRes;
        binder_status_t result2 = in_dcList[i].writeToParcel(nativeSetupRes);
        if (result2 != STATUS_OK) {
          LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result2);
        }
        AParcel_setDataPosition(nativeSetupRes, 0);
        env->SetObjectArrayElement(setupObj, i, jParcelSetupRes);
      }
      env->CallVoidMethod(jniCallbackObj, callback, jParcelResInfo, setupObj, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      for (int i = 0; i < in_dcList.size(); i++) {
        AParcel_delete(parcelArray[i]);
      }
      env->DeleteLocalRef(setupObj);
      AParcel_delete(nativeResInfo);
    }
    detachThread();
  }
}

void AidlClient::dataCallListChanged(RadioIndicationType in_type,
                                     const std::vector<SetupDataCallResult>& in_dcList,
                                     int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "dataCallListChanged start");
    std::string methodName = "dataCallListChanged";
    std::string parameterList = "(I[Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      jobjectArray setupObj = env->NewObjectArray(in_dcList.size(),
                                                  env->FindClass("android/os/Parcel"), NULL);

      AParcel *parcelArray[in_dcList.size()];
      for (int i = 0; i < in_dcList.size(); i++){
        jobject jParcelSetupRes = createJavaParcelObject(env);
        AParcel* nativeSetupRes = AParcel_fromJavaParcel(env, jParcelSetupRes);
        parcelArray[i] = nativeSetupRes;
        binder_status_t result2 = in_dcList[i].writeToParcel(nativeSetupRes);
        if (result2 != STATUS_OK) {
          LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result2);
        }
        AParcel_setDataPosition(nativeSetupRes, 0);
        env->SetObjectArrayElement(setupObj, i, jParcelSetupRes);
      }
      env->CallVoidMethod(jniCallbackObj, callback, (jint)in_type, setupObj, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      for (int i = 0; i < in_dcList.size(); i++) {
        AParcel_delete(parcelArray[i]);
      }
      env->DeleteLocalRef(setupObj);
    }
    detachThread();
  }
}

void AidlClient::getAllQualifiedNetworksResponse(const IWlanResponseInfo& in_info,
                                                 const std::vector<QualifiedNetworks>& in_qualifiedNetworksList,
                                                 int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "getAllQualifiedNetworksResponse start");
    std::string methodName = "getAllQualifiedNetworksResponse";
    std::string parameterList = "(Landroid/os/Parcel;[Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(env, methodName.data(), parameterList.data(),
                                                  callback);
    if (jniCallbackObj != NULL) {
      jobject jParcelResInfo = createJavaParcelObject(env);
      AParcel* nativeResInfo = AParcel_fromJavaParcel(env, jParcelResInfo);
      binder_status_t result = in_info.writeToParcel(nativeResInfo);
      if (result != STATUS_OK) {
        LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
      }
      AParcel_setDataPosition(nativeResInfo, 0);


      jobjectArray qualObject = env->NewObjectArray(in_qualifiedNetworksList.size(),
                                                    env->FindClass("android/os/Parcel"), NULL);

      AParcel *parcelArray[in_qualifiedNetworksList.size()];
      for (int i = 0; i < in_qualifiedNetworksList.size(); i++){
        jobject jParcelQualNet = createJavaParcelObject(env);
        AParcel* nativeQualNet = AParcel_fromJavaParcel(env, jParcelQualNet);
        parcelArray[i] = nativeQualNet;
        binder_status_t result2 = in_qualifiedNetworksList[i].writeToParcel(nativeQualNet);
        if (result2 != STATUS_OK) {
          LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result2);
        }
        AParcel_setDataPosition(nativeQualNet, 0);
        env->SetObjectArrayElement(qualObject, i, jParcelQualNet);
      }

      env->CallVoidMethod(jniCallbackObj, callback, jParcelResInfo, qualObject, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);
      for (int i = 0; i < in_qualifiedNetworksList.size(); i++) {
        AParcel_delete(parcelArray[i]);
      }
      env->DeleteLocalRef(qualObject);
      AParcel_delete(nativeResInfo);
    }
    detachThread();
  }
}


void AidlClient::qualifiedNetworksChangeIndication(const std::vector<QualifiedNetworks>& in_qualifiedNetworksList,
                                                   int slot)
{
  JNIEnv* env = nullptr;
  if (isThreadAttached(&env))
  {
    LOG_D(AidlClient::LOGTAG, "qualifiedNetworksChangeIndication start");
    std::string methodName = "qualifiedNetworksChangeIndication";
    std::string parameterList = "([Landroid/os/Parcel;I)V";
    jmethodID callback = NULL;
    jobject jniCallbackObj = getJavaCallbackClass(
      env, methodName.data(), parameterList.data(), callback);
    if (jniCallbackObj != NULL) {

      jobjectArray qualObject = env->NewObjectArray(in_qualifiedNetworksList.size(),
                                                    env->FindClass("android/os/Parcel"), NULL);

      AParcel *parcelArray[in_qualifiedNetworksList.size()];
      for (int i = 0; i < in_qualifiedNetworksList.size(); i++){
        jobject jParcelQualNet = createJavaParcelObject(env);
        AParcel* nativeQualNet = AParcel_fromJavaParcel(env, jParcelQualNet);
        parcelArray[i] = nativeQualNet;
        binder_status_t result = in_qualifiedNetworksList[i].writeToParcel(nativeQualNet);
        if (result != STATUS_OK) {
          LOG_E(AidlClient::LOGTAG, "Write to Parcel Failed, error : [%d]", result);
        }
        AParcel_setDataPosition(nativeQualNet, 0);
        env->SetObjectArrayElement(qualObject, i, jParcelQualNet);
      }
      env->CallVoidMethod(jniCallbackObj, callback, qualObject, (jint)slot);
      env->DeleteLocalRef(jniCallbackObj);

      for (int i = 0; i < in_qualifiedNetworksList.size(); i++) {
        AParcel_delete(parcelArray[i]);
      }
      env->DeleteLocalRef(qualObject);
    }
    detachThread();
  }
}

void AidlClient::handleEvents() {
  while (1) {
    mEventQueueMutex.lock();
    if (mEventQueue.empty()) {
      mEventQueueMutex.unlock();
      return;
    }
    CallbackEvent next = mEventQueue.front();
    mEventQueue.pop();
    mEventQueueMutex.unlock();
    handleEvent(next);
  }
}

void AidlClient::handleEvent(CallbackEvent& event) {
  LOG_D(LOGTAG, "handleEvent %d", event.getType());
  switch (event.getType())
  {
    case CallbackEvent::SERVICE_DEATH:
    {
      handleServiceDeath(event.getSlotId());
      break;
    }
    case CallbackEvent::MODEM_SUPPORT_NOT_PRESENT:
    {
      modemSupportNotPresent(event.getSlotId());
      break;
    }
    case CallbackEvent::DATA_REG_STATE_CHANGE_IND:
    {
      dataRegistrationStateChangeIndication(event.getSlotId());
      break;
    }
    case CallbackEvent::DATA_CALL_LIST_CHANGED:
    {
      dataCallListChanged(event.getRadioIndicationType(), event.getDcList(), event.getSlotId());
      break;
    }
    case CallbackEvent::QUALIFIED_NET_CHANGE_IND:
    {
      qualifiedNetworksChangeIndication(event.getQualifiedNetworksList(), event.getSlotId());
      break;
    }
    case CallbackEvent::UNTHROTTLE_APN:
    {
      unthrottleApn(event.getRadioIndicationType(), event.getDataProfileInfo(), event.getSlotId());
      break;
    }
    case CallbackEvent::SETUP_DATA_CALL_RESP:
    {
      setupDataCallResponse(event.getRadioResponseInfo(), event.getSetupDataCallResult(),
                            event.getSlotId());
      break;
    }
    case CallbackEvent::DEACTIVATE_DATA_CALL_RESP:
    {
      deactivateDataCallResponse(event.getIWlanResponseInfo(), event.getSlotId());
      break;
    }
    case CallbackEvent::GET_DATA_CALL_LIST_RESP:
    {
      getDataCallListResponse(event.getRadioResponseInfo(), event.getDcList(), event.getSlotId());
      break;
    }
    case CallbackEvent::GET_DATA_REG_STATE_RESP:
    {
      getDataRegistrationStateResponse(event.getIWlanResponseInfo(),
                                       event.getIWlanDataRegStateResult(), event.getSlotId());
      break;
    }
    case CallbackEvent::GET_ALL_QUAL_NET_RESP:
    {
      getAllQualifiedNetworksResponse(event.getIWlanResponseInfo(), event.getQualifiedNetworksList(),
                                      event.getSlotId());
      break;
    }
    case CallbackEvent::ACKNOWLEDGE_REQUEST:
    {
      acknowledgeRequest(event.getSerial(), event.getSlotId());
      break;
    }
    default:
      LOG_E(AidlClient::LOGTAG, "No known event found");
  }
}

IWlanServiceIndication::IWlanServiceIndication(AidlClient *aidlClient)
{
  mAidlClient = aidlClient;
}

IWlanServiceIndication::~IWlanServiceIndication()
{
}

IWlanServiceResponse::IWlanServiceResponse(AidlClient *aidlClient)
{
  mAidlClient = aidlClient;
}

IWlanServiceResponse::~IWlanServiceResponse()
{
}

::ndk::ScopedAStatus IWlanServiceIndication::unthrottleApn(RadioIndicationType in_type,
                                                           const DataProfileInfo& in_dataProfileInfo)
{
  CallbackEvent newEv(CallbackEvent::UNTHROTTLE_APN);
  newEv.setUnthrottleApn(in_type, in_dataProfileInfo);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::acknowledgeRequest(int32_t in_serial)
{
  CallbackEvent newEv(CallbackEvent::ACKNOWLEDGE_REQUEST);
  newEv.setAcknowledgeRequest(in_serial);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceIndication::dataRegistrationStateChangeIndication()
{
  CallbackEvent newEv(CallbackEvent::DATA_REG_STATE_CHANGE_IND);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceIndication::modemSupportNotPresent()
{
  CallbackEvent newEv(CallbackEvent::MODEM_SUPPORT_NOT_PRESENT);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::getDataRegistrationStateResponse(
                                           const IWlanResponseInfo& in_response,
                                           const IWlanDataRegStateResult& in_result)
{
  CallbackEvent newEv(CallbackEvent::GET_DATA_REG_STATE_RESP);
  newEv.setDataRegistrationStateResponse(in_response, in_result);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::deactivateDataCallResponse(
                                            const IWlanResponseInfo& in_response)
{
  CallbackEvent newEv(CallbackEvent::DEACTIVATE_DATA_CALL_RESP);
  newEv.setDeactivateDataCallResponse(in_response);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::setupDataCallResponse(
                                            const RadioResponseInfo& in_info,
                                            const SetupDataCallResult& in_dcResponse)
{
  CallbackEvent newEv(CallbackEvent::SETUP_DATA_CALL_RESP);
  newEv.setSetupDataCallResponse(in_info, in_dcResponse);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::getDataCallListResponse(
                                            const RadioResponseInfo& in_info,
                                            const std::vector<SetupDataCallResult>& in_dcResponse)
{
  CallbackEvent newEv(CallbackEvent::GET_DATA_CALL_LIST_RESP);
  newEv.setDataCallListResponse(in_info, in_dcResponse);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceResponse::getAllQualifiedNetworksResponse(
                                            const IWlanResponseInfo& in_info,
                                            const std::vector<QualifiedNetworks>& in_qualifiedNetworksList)
{
  CallbackEvent newEv(CallbackEvent::GET_ALL_QUAL_NET_RESP);
  newEv.setAllQualifiedNetworksResponse(in_info, in_qualifiedNetworksList);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceIndication::dataCallListChanged(
                                            RadioIndicationType in_type,
                                            const std::vector<SetupDataCallResult>& in_dcList)
{
  CallbackEvent newEv(CallbackEvent::DATA_CALL_LIST_CHANGED);
  newEv.setDataCallListChanged(in_type, in_dcList);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceIndication::qualifiedNetworksChangeIndication(
                                            const std::vector<QualifiedNetworks>& in_qualifiedNetworksList)
{
  CallbackEvent newEv(CallbackEvent::QUALIFIED_NET_CHANGE_IND);
  newEv.setQualifiedNetworksChangeIndication(in_qualifiedNetworksList);
  newEv.setSlotId(mSlotId);
  mAidlClient->pushEv(newEv);
  return ::ndk::ScopedAStatus::ok();
}
