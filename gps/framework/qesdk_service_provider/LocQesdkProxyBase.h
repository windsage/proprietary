/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_QESDK_PPE_PROXY_H
#define LOC_QESDK_PPE_PROXY_H

#include <memory>
#include <utility>
#include "LocationDataTypes.h"
#include <base_util/postcard.h>

namespace location_qesdk {

typedef enum {
    QESDK_SESSION_TYPE_INVALID,
    QESDK_SESSION_TYPE_PRECISE = 1,
    QESDK_SESSION_TYPE_GTP,
    QESDK_SESSION_TYPE_PRECISE_GTP,
    QESDK_SESSION_TYPE_RL
} QesdkSessionType;

typedef std::function<void(uint32_t pid, uint32_t uid, bool isStart, uint32_t minIntervalMillis,
        QesdkSessionType sessionType)> sendQesdkLocationReqToLocAidl;
typedef std::function<void(uint32_t pid, uint32_t uid, bool userConsent)> UserConsentToLocAidlCb;

class LocQesdkPPEProxyBase {
public:
    virtual void setLocationReqCallback(const sendQesdkLocationReqToLocAidl& reqCallback) = 0;
    virtual void setUserConsentCallback(const UserConsentToLocAidlCb& userConsentCb) = 0;

    virtual uint8_t requestPreciseLocationUpdates(uint32_t pid, uint32_t uid,
            uint32_t minIntervalMillis) = 0;
    virtual uint8_t removePreciseLocationUpdates(uint32_t pid, uint32_t uid) = 0;

    virtual uint8_t requestGtpLocationUpdates(uint32_t pid, uint32_t uid,
            uint32_t minIntervalMillis) = 0;
    virtual uint8_t removeGtpLocationUpdates(uint32_t pid, uint32_t uid) = 0;
    virtual uint8_t requestGtpPassiveLocationUpdates(uint32_t pid, uint32_t uid) = 0;
    virtual uint8_t removeGtpPassiveLocationUpdates(uint32_t pid, uint32_t uid) = 0;
    virtual uint8_t notifyUserConsent(uint32_t pid, uint32_t uid, bool userConsent) = 0;

    virtual uint8_t requestPreciseGtpLocationUpdates(uint32_t pid, uint32_t uid,
            uint32_t minIntervalMillis) = 0;
    virtual uint8_t removePreciseGtpLocationUpdates(uint32_t pid, uint32_t uid) = 0;

    virtual uint8_t requestRlLocationUpdates(uint32_t pid, uint32_t uid,
            uint32_t minIntervalMillis) = 0;
    virtual uint8_t removeRlLocationUpdates(uint32_t pid, uint32_t uid) = 0;

    virtual void handleAidlClientSsr() = 0;
    inline virtual ~LocQesdkPPEProxyBase() {}
};
}
#endif
