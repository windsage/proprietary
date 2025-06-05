/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GARDEN_UTIL_H
#define GARDEN_UTIL_H

#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG 0
#ifndef LOG_TAG
#define LOG_TAG "LocSvc_Garden"
#endif

#include <inttypes.h>
#include <iostream>
#include <unistd.h>
#include <loc_pla.h>
#include <log_util.h>
#include "IGardenCase.h"
#include <loc_gps.h>
#include <LocationDataTypes.h>

#include <mutex>
#include <condition_variable>

using namespace std;

namespace garden {

#define GARDEN_HEAD(fmt) "%s:%d] " fmt "\n"

#define gardenError(fmt, ...) \
    fprintf(stderr, GARDEN_HEAD(fmt), __FILE__, __LINE__, ##__VA_ARGS__); \
    LOC_LOGE(GARDEN_HEAD(fmt), __FILE__, __LINE__, ##__VA_ARGS__); \

#define gardenPrint(fmt, ...) \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__); \
    LOC_LOGV(fmt, ##__VA_ARGS__)

#define menuPrint(fmt, ...) \
    fprintf(stdout, fmt, ##__VA_ARGS__)

#define GARDEN_ERROR(fmt, ...) \
{ \
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ASSERT_EQ(val, exp, fmt, ...) \
if (val != exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ASSERT_NEQ(val, exp, fmt, ...) \
if (val == exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ABORT_EQ(val, exp, fmt, ...) \
if (val != exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_ABORT; \
}

#define GARDEN_ABORT_NEQ(val, exp, fmt, ...) \
if (val == exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_ABORT; \
}

uint64_t getRealTimeNsec();
uint64_t getUpTimeSec();

std::string trimWhitespaces(std::string str);

// Parse a string to c style
#define MAX_ARGUMENT_LEN 256
int parseStrToArgv(
    std::string str, char* argv[], int maxArgCount, int maxArgLen);

// Parse time duration from string which can be of following format
// <val>s - seconds - e.g. "2s" - return 2000
// <val>ms - milliseconds - e.g. "3000ms" - return 3000
// Return -1 for invalid format (anything except above 2 formats)
int parseDurationMillis(std::string durationStr);

// Parse a list of integers seperated by space in a string
// The input must be a string like, "23 44 12 8"
bool parseIntegerList(std::vector<int>& intList, std::string intListStr);
bool parseUInt8List(std::vector<uint8_t>& uint8List, std::string uint8ListStr);

// Parsing optarg string
bool parseLocGpsPositionMode(LocGpsPositionMode& mode, char opt, std::string optarg);
bool parseLocGpsPositionRecurrence(
        LocGpsPositionRecurrence& recurrence, char opt, std::string optarg);
bool parseMinIntervalMillis(uint32_t& minIntervalMillis, char opt, std::string optarg);
bool parsePreferredAccuracyMeters(uint32_t& preferredAccuracyMeters, char opt, std::string optarg);
bool parsePreferredTimeMillis(uint32_t& preferredTimeMillis, char opt, std::string optarg);

std::vector<std::string> strUtilTokenize(const std::string& str);
bool parseGnssSvTypesMask(std::string constellationListStr, GnssSvTypesMask& mask);

void gnssSvTypesMaskToDisabledConstellationVec(
        GnssSvTypesMask enableMask, GnssSvTypesMask disableMask,
        std::vector<uint8_t>& disabledVec);

template <typename T>
std::string vectorToString(const std::vector<T>& vec) {

    std::string res(" ");
    for (auto& val: vec) {
        res += std::to_string(val) + " ";
    }
    return res;
}

class Waitable {
protected:
    int mCounter = 0;
    mutex mLock;
    condition_variable mCv;
public:
    bool wait(int timeInSec);
    void notify();
    void reset();
};


} // namespace garden
#endif // GARDEN_UTIL_H
