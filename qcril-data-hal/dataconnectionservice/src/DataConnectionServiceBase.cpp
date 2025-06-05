/******************************************************************************
#  Copyright (c) 2021,2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "DataConnectionServiceBase.h"
#include "MessageCommon.h"
#include "RegisterBearerAllocationUpdateRequestMessage.h"
#include <cutils/properties.h>

namespace aidl::vendor::qti::hardware::data::connectionaidl {

DataConnectionServiceBase::DataConnectionServiceBase()
{
    wakelockTimerId = TimeKeeper::no_timer;
    wakelockAcquired = false;
    registeredResponses.clear();
    registeredIndications.clear();
    nextCookie = 0;
    mDeathRecipients.clear();
    Log::getInstance().d("DataConnectionServiceBase ctor");
}

void DataConnectionServiceBase::acquireWakelock(TimeKeeper::millisec timeout) {
#ifndef QMI_RIL_UTF
    std::lock_guard<std::mutex> lock(wakelockMutex);
    // if already acquired, refresh the timer
    if (wakelockAcquired) {
        Log::getInstance().d("DataConnectionServiceBase::refreshing wakelock");
        bool timerCleared = TimeKeeper::getInstance().clear_timer(wakelockTimerId);
        if (!timerCleared) {
            Log::getInstance().d("Failed to clear wakelock timer");
        }
    } else {
        Log::getInstance().d("DataConnectionServiceBase::acquiring wakelock");
        if (acquire_wake_lock(PARTIAL_WAKE_LOCK, BEARER_ALLOCATION_WAKELOCK) < 0) {
            Log::getInstance().d("Failed to acquire wakelock");
        }
    }
    wakelockTimerId = TimeKeeper::getInstance().set_timer(
        std::bind(&DataConnectionServiceBase::releaseWakelockCb, this, std::placeholders::_1),
        nullptr,
        timeout);
#endif
}

void DataConnectionServiceBase::releaseWakelockCb(void *) {
#ifndef QMI_RIL_UTF
    Log::getInstance().d("DataConnectionServiceBase::release wakelock");
    std::lock_guard<std::mutex> lock(wakelockMutex);
    if (wakelockAcquired)
    {
        if (release_wake_lock(BEARER_ALLOCATION_WAKELOCK) < 0)
        {
            Log::getInstance().d("DataConnectionServiceBase::wakelock not acquired");
        }
        wakelockAcquired = false;
    }
#endif
}

uint64_t DataConnectionServiceBase::generateCookie() {
    uint64_t newCookie;
    cookieMutex.lock();
    newCookie = nextCookie++;
    cookieMutex.unlock();
    return newCookie;
}

static void deathRecpCallback(void* data)
{
    deathCookie* dCookie = static_cast<deathCookie*>(data);
    if (!dCookie) return;
    DataConnectionServiceBase* impl = static_cast<DataConnectionServiceBase*>(dCookie->self);
    if (impl != nullptr) {
        impl->deathNotifier(dCookie->cookie);
    }
    if (dCookie)
        delete dCookie;
}

void DataConnectionServiceBase::deathNotifier(uint64_t cookie)
{
    registeredResponsesMutex.lock();
    registeredResponses.erase(cookie);
    registeredResponsesMutex.unlock();
    registeredIndicationsMutex.lock();
    registeredIndications.erase(cookie);
    if (registeredIndications.empty()) {
        auto requestMsg = std::make_shared<RegisterBearerAllocationUpdateRequestMessage>(false);
        requestMsg->dispatch();
    }
    registeredIndicationsMutex.unlock();
    mDeathNotifierMutex.lock();
    mDeathRecipients.erase(cookie);
    mDeathNotifierMutex.unlock();
}

uint64_t DataConnectionServiceBase::registerResponseCb(const std::shared_ptr<IDataConnectionResponse>& cb) {
    uint64_t cookie = generateCookie();
    if (cb == nullptr) {
        Log::getInstance().d("registerResponseCb cb is null");
        return cookie;
    }
    registeredResponsesMutex.lock();
    Log::getInstance().d("+cookie "+ std::to_string(cookie));
    registeredResponses.insert({cookie, cb});
    registeredResponsesMutex.unlock();
#ifndef QMI_RIL_UTF
    mDeathNotifierMutex.lock();
    deathCookie* dCookie = new (std::nothrow) deathCookie {};
    if (dCookie) {
      dCookie->self = reinterpret_cast<void*>(this);
      dCookie->cookie = cookie;
      if (mDeathRecipients[cookie]) {
        AIBinder_unlinkToDeath(cb->asBinder().get(), mDeathRecipients[cookie],
                             reinterpret_cast<void*>(dCookie));
        mDeathRecipients[cookie] = nullptr;
      }
      mDeathRecipients[cookie] = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipients[cookie] != nullptr) {
        AIBinder_linkToDeath(cb->asBinder().get(), mDeathRecipients[cookie],
                             reinterpret_cast<void*>(dCookie));
        Log::getInstance().d("DeathRecipient is linked successfully");
      }
    }
    mDeathNotifierMutex.unlock();
#endif
    return cookie;
}

uint64_t DataConnectionServiceBase::registerIndicationCb(const std::shared_ptr<IDataConnectionIndication>& cb) {
    uint64_t cookie = generateCookie();
    if (cb == nullptr) {
        Log::getInstance().d("registerIndicationCb cb is null");
        return cookie;
    }
    registeredIndicationsMutex.lock();
    if (registeredIndications.empty()) {
        auto requestMsg = std::make_shared<RegisterBearerAllocationUpdateRequestMessage>(true);
        requestMsg->dispatch();
    }
    Log::getInstance().d("+cookie "+ std::to_string(cookie));
    registeredIndications.insert({cookie, cb});
    registeredIndicationsMutex.unlock();
#ifndef QMI_RIL_UTF
    mDeathNotifierMutex.lock();
    deathCookie* dCookie = new (std::nothrow) deathCookie {};
    if (dCookie) {
      dCookie->self = reinterpret_cast<void*>(this);
      dCookie->cookie = cookie;
      if (mDeathRecipients[cookie]) {
        AIBinder_unlinkToDeath(cb->asBinder().get(), mDeathRecipients[cookie],
                             reinterpret_cast<void*>(dCookie));
        mDeathRecipients[cookie] = nullptr;
      }
      mDeathRecipients[cookie] = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipients[cookie] != nullptr) {
        AIBinder_linkToDeath(cb->asBinder().get(), mDeathRecipients[cookie],
                             reinterpret_cast<void*>(dCookie));
        Log::getInstance().d("DeathRecipient is linked successfully");
      }
    }
    mDeathNotifierMutex.unlock();
#endif
    return cookie;
}

void DataConnectionServiceBase::notifyBearerAllocationUpdate(std::shared_ptr<AllocatedBearerResult_t> result) {
    Log::getInstance().d("DataConnectionServiceBase::notifyBearerAllocationUpdate");
    if (result == nullptr) {
        Log::getInstance().d("bearer allocation update with null");
        return;
    }
    std::vector<AllocatedBearers> bearersList;
    for (auto connection : result->connections) {
        AllocatedBearers bearers;
        transpose(connection, bearers);
        bearersList.push_back(bearers);
    }

    // invoke indication callback on all registered clients
    acquireWakelock(BEARER_ALLOCATION_TIMEOUT);
    std::lock_guard<std::mutex> lock(registeredIndicationsMutex);
    for (auto it = registeredIndications.begin(); it != registeredIndications.end(); ++it) {
        const std::shared_ptr<IDataConnectionIndication> clientCb = it->second;
        if (clientCb != nullptr) {
            clientCb->onBearerAllocationUpdate(bearersList);
        }
    }
}

::ndk::ScopedAStatus DataConnectionServiceBase::getBearerAllocation(int32_t cid ,
           const std::shared_ptr<IDataConnectionResponse>& cb, StatusCode* status)
{
    std::ignore = status;
    if (cb == nullptr) {
        Log::getInstance().d("getBearerAllocation cb is null");
        return ndk::ScopedAStatus::ok();
    }
    uint64_t cookie = registerResponseCb(cb);
    Log::getInstance().d("DataConnectionServiceBase::getBearerAllocation()" + std::to_string(cookie));
    auto requestMsg = std::make_shared<GetBearerAllocationRequestMessage>(cid);
    auto responseFn = std::bind(&DataConnectionServiceBase::sendBearerAllocationResult, this, cookie, std::placeholders::_3);
    GenericCallback<AllocatedBearerResult_t> requestCb(responseFn);
    requestMsg->setCallback(&requestCb);
    requestMsg->dispatch();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DataConnectionServiceBase::getAllBearerAllocations(
    const std::shared_ptr<IDataConnectionResponse>& cb,
    StatusCode* status)
{
    std::ignore = status;
    uint64_t cookie = registerResponseCb(cb);
    Log::getInstance().d("DataConnectionServiceBase::getAllBearerAllocations()" + std::to_string(cookie));
    if (cb == nullptr) {
        Log::getInstance().d("getAllBearerAllocations cb is null");
        return ndk::ScopedAStatus::ok();
    }
    auto requestMsg = std::make_shared<GetAllBearerAllocationsRequestMessage>();
    auto responseFn = std::bind(&DataConnectionServiceBase::sendAllBearerAllocationsResult, this, cookie, std::placeholders::_3);
    GenericCallback<AllocatedBearerResult_t> requestCb(responseFn);
    requestMsg->setCallback(&requestCb);
    requestMsg->dispatch();
    return ndk::ScopedAStatus::ok();
}

void DataConnectionServiceBase::sendBearerAllocationResult(
    uint64_t cookie,
    std::shared_ptr<AllocatedBearerResult_t> result
) {
    registeredResponsesMutex.lock();
    std::shared_ptr<IDataConnectionResponse> cb = registeredResponses[cookie];
    registeredResponsesMutex.unlock();
    Log::getInstance().d("DataConnectionServiceBase::sendBearerAllocationResult()" + std::to_string(cookie));
    if (cb == nullptr) {
        Log::getInstance().d("sendBearerAllocationResult() client callback is null");
        return;
    }
    acquireWakelock(BEARER_ALLOCATION_TIMEOUT);
    AllocatedBearers bearers = {};
    ErrorReason error = ErrorReason::HARDWARE_ERROR;
    if (result == nullptr) {
        Log::getInstance().d("bearer list is null");
        cb->onBearerAllocationResponse(error, bearers);
        return;
    }
    transpose(result->error, error);
    if (result->error != ResponseError_t::NO_ERROR ||
        result->connections.empty()) {
        Log::getInstance().d("getBearerAllocation returned error");
        cb->onBearerAllocationResponse(error, bearers);
        return;
    }

    AllocatedBearer_t bearerResult = result->connections.front();
    transpose(bearerResult, bearers);
    cb->onBearerAllocationResponse(error, bearers);
}

void DataConnectionServiceBase::sendAllBearerAllocationsResult(
    uint64_t cookie,
    std::shared_ptr<AllocatedBearerResult_t> result
) {
    registeredResponsesMutex.lock();
    std::shared_ptr<IDataConnectionResponse> cb = registeredResponses[cookie];
    registeredResponsesMutex.unlock();
    Log::getInstance().d("DataConnectionImpl::sendAllBearerAllocationsResult()" + std::to_string(cookie));
    if (cb == nullptr) {
        Log::getInstance().d("sendAllBearerAllocationsResult() client callback is null");
        return;
    }
    acquireWakelock(BEARER_ALLOCATION_TIMEOUT);
    std::vector<AllocatedBearers> bearersList;
    ErrorReason error = ErrorReason::HARDWARE_ERROR;
    if (result == nullptr) {
        Log::getInstance().d("bearer list is null");
        cb->onAllBearerAllocationsResponse(error, bearersList);
        return;
    }
    transpose(result->error, error);
    if (result->error != ResponseError_t::NO_ERROR) {
        Log::getInstance().d("getAllBearerAllocations returned error");
        cb->onAllBearerAllocationsResponse(error, bearersList);
        return;
    }
    for (auto connection : result->connections) {
        AllocatedBearers bearers = {};
        transpose(connection, bearers);
        bearersList.push_back(bearers);
    }
    cb->onAllBearerAllocationsResponse(error, bearersList);
}

::ndk::ScopedAStatus DataConnectionServiceBase::registerForAllBearerAllocationUpdates(const std::shared_ptr<IDataConnectionIndication>& cb, StatusCode* status)
{
    std::ignore = status;
    Log::getInstance().d("DataConnectionServiceBase::registerForBearerAllocationUpdates");
    registerIndicationCb(cb);
    return ndk::ScopedAStatus::ok();
}

void DataConnectionServiceBase::transpose(
    const AllocatedBearer_t& connection,
    AllocatedBearers& bearers)
{
    bearers.cid = connection.cid;
    bearers.apn = connection.apn;
    std::vector<BearerInfo> dataBearers;
    for (auto connectionBearers : connection.bearers) {
        BearerInfo bearerInfo;
        bearerInfo.bearerId = connectionBearers.bearerId;
        transpose(connectionBearers.uplink, bearerInfo.uplink);
        transpose(connectionBearers.downlink, bearerInfo.downlink);
        dataBearers.push_back(bearerInfo);
    }
    bearers.bearers = dataBearers;
}

void DataConnectionServiceBase::transpose(
    const RatType_t& commonType,
    RatType& ratType)
{
    switch (commonType) {
        case RatType_t::RAT_4G:
            ratType = RatType::RAT_4G;
            break;
        case RatType_t::RAT_5G:
            ratType = RatType::RAT_5G;
            break;
        case RatType_t::RAT_SPLITED:
            ratType = RatType::RAT_5G;
            break;
        case RatType_t::RAT_UNSPECIFIED:
        default:
            ratType = RatType::UNSPECIFIED;
            break;
    }
}

void DataConnectionServiceBase::transpose(
    const ResponseError_t& commonError,
    ErrorReason& halError)
{
    switch (commonError) {
        case ResponseError_t::NO_ERROR:
            halError = ErrorReason::NO_ERROR;
            break;
        case ResponseError_t::CALL_NOT_AVAILABLE:
            halError = ErrorReason::CALL_NOT_AVAILABLE;
            break;
        default:
            halError = ErrorReason::HARDWARE_ERROR;
            break;
    }
}

::ndk::ScopedAStatus DataConnectionServiceBase::getConfig(const std::string& key, const std::string& defaultValue, std::string* configValue)
{
    Log::getInstance().d("DataConnectionServiceBase::getConfig");
    char prop[PROPERTY_VALUE_MAX] = {'\0'};
    property_get(key.c_str(), prop, defaultValue.c_str());
    std::string str(prop);
    configValue = &str;
    return ndk::ScopedAStatus::ok();
}
}//namespace
