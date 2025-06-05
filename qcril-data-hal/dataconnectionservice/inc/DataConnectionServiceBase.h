/*
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "MessageCommon.h"
#include "GetBearerAllocationRequestMessage.h"
#include "GetAllBearerAllocationsRequestMessage.h"
#include "RegisterBearerAllocationUpdateRequestMessage.h"
#include "BnDataConnectionServiceBase.h"
#include <hardware_legacy/power.h>

using namespace rildata;
using namespace aidl::vendor::qti::hardware::data::connectionaidl;

struct deathCookie {
    void* self;
    uint64_t cookie;
};

namespace aidl::vendor::qti::hardware::data::connectionaidl {

class DataConnectionServiceBase : public BnDataConnectionServiceBase {
private:
    std::mutex wakelockMutex;
    TimeKeeper::timer_id wakelockTimerId;
    bool wakelockAcquired;

    static constexpr TimeKeeper::millisec BEARER_ALLOCATION_TIMEOUT = 500;
    static constexpr const char *BEARER_ALLOCATION_WAKELOCK = "qcril_bearer_allocation_result_wakelock";

    void releaseWakelockCb(void *);
    void acquireWakelock(TimeKeeper::millisec timeout);
    void transpose(const AllocatedBearer_t& connection,
                   AllocatedBearers& bearers);
    void transpose(const RatType_t& commonType,
                   RatType& ratType);
    void transpose(const ResponseError_t& commonError,
                   ErrorReason& halError);

    uint64_t generateCookie();
    uint64_t registerResponseCb(const std::shared_ptr<IDataConnectionResponse>& cb);
    uint64_t registerIndicationCb(const std::shared_ptr<IDataConnectionIndication>& cb);

    std::mutex cookieMutex;
    std::mutex registeredResponsesMutex;
    std::mutex registeredIndicationsMutex;
    std::mutex mDeathNotifierMutex;

    uint64_t nextCookie;
    std::map<uint64_t, const std::shared_ptr<IDataConnectionResponse>> registeredResponses;
    std::map<uint64_t, const std::shared_ptr<IDataConnectionIndication>> registeredIndications;
    std::map<uint64_t, AIBinder_DeathRecipient*> mDeathRecipients;

public:
    DataConnectionServiceBase();
    ~DataConnectionServiceBase() {}

    void deathNotifier(uint64_t cookie);

    /**AIDL APIS**/
    ::ndk::ScopedAStatus getAllBearerAllocations(const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionResponse>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override;
    ::ndk::ScopedAStatus getBearerAllocation(int32_t , const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionResponse>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override;
    ::ndk::ScopedAStatus registerForAllBearerAllocationUpdates(const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionIndication>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override;
    ::ndk::ScopedAStatus getConfig(const std::string& , const std::string& , std::string* ) override;

    void sendBearerAllocationResult(uint64_t ,
                                    std::shared_ptr<AllocatedBearerResult_t> );
    void sendAllBearerAllocationsResult(uint64_t ,
                                        std::shared_ptr<AllocatedBearerResult_t> );
    void notifyBearerAllocationUpdate(std::shared_ptr<AllocatedBearerResult_t> result);
};

}
