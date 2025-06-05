/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "GARDEN_GardenUtil"

#include <loc_pla.h>
#include <log_util.h>
#include <GardenUtil.h>
#include <regex>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unistd.h>

using std::mutex;
using std::unique_lock;
using std::cv_status;
using namespace std::chrono;

namespace garden {

bool Waitable::wait(int timeoutInSec) {
    unique_lock<mutex> lck(mLock);
    auto now = chrono::system_clock::now();

    bool res = true;

    // A notify did not ocurred before this wait call, so wait.
    if (mCounter <= 0) {
        res = (mCv.wait_until(lck, now + chrono::seconds(timeoutInSec)) == cv_status::no_timeout);
    }

    if (res) {
        mCounter--;
    }
    return res;
}

void Waitable::notify() {
    unique_lock<mutex> lck(mLock);
    mCounter++;
    mCv.notify_all();
}

void Waitable::reset() {
    unique_lock<mutex> lck(mLock);
    mCounter = 0;
}

uint64_t getRealTimeNsec()
{
    struct timespec ts = {};

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

uint64_t getUpTimeSec()
{
    struct timespec ts = {};

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1000000000LL);
}

std::string trimWhitespaces(std::string str) {

    std::string ltrimmed = std::regex_replace(str, std::regex("^\\s+"), std::string(""));
    return std::regex_replace(ltrimmed, std::regex("\\s+$"), std::string(""));
}

// Returns argument count in the string (argc) and populates the arguments in argv
int parseStrToArgv(
    std::string str, char* argv[], int maxArgCount, int maxArgLen) {

    int argc = 0;
    int argLen = 0;
    char lastChar = '\0';
    bool isLastCharWhitespace = false;
    bool isInsideQuotes = false;

    std::string trimmed = trimWhitespaces(str);
    if (trimmed.size() > 0) argc = 0;

    for (char c : trimmed) {

        if (argLen >= maxArgLen || argc >= maxArgCount) {
            gardenError("max reached argLen %d (max %d) argc %d (max %d)",
                            argLen, maxArgLen, argc, maxArgCount);
            return -1;
        }

        bool isWhitespace = (c == ' ' || c == '\t');
        bool isRegularQuote = (c == '"' && lastChar != '\\');
        bool isEscapedQuote = (c == '"' && lastChar == '\\');
        isInsideQuotes = isRegularQuote? !isInsideQuotes: isInsideQuotes;

        if (!isInsideQuotes && isWhitespace && isLastCharWhitespace) continue;

        // when to copy current char in an arg
        if ((isInsideQuotes || !isWhitespace) && !isRegularQuote) {

            if (isEscapedQuote) {
                argv[argc][argLen-1] = c;
            } else {
                argv[argc][argLen] = c;
            }
            argLen++;

        } else if (!isInsideQuotes && isWhitespace) {

            argv[argc][argLen] = '\0';
            argc++;
            argLen = 0;
        }

        lastChar = c;
        isLastCharWhitespace = isWhitespace;
    }

    return argc + 1;
}

// Duration string can be in seconds or milliseconds
// It should always end with either or below:
// "s"      for duration in seconds, e.g. 10s
// "ms"     for duration in milliseconds, e.g. 3000ms
// The returned value is always in milliseconds
int parseDurationMillis(std::string durationStr) {

    std::transform(durationStr.begin(), durationStr.end(), durationStr.begin(),
            [](unsigned char c) -> unsigned char { return std::tolower(c); });
    size_t msPos = durationStr.find("ms");
    size_t sPos = durationStr.find("s");
    if (msPos != std::string::npos) {
        std::string millisStr = durationStr.substr(0, msPos);
        return atoi(millisStr.c_str());
    } else if (sPos != std::string::npos) {
        std::string secStr = durationStr.substr(0, sPos);
        return atoi(secStr.c_str()) * 1000;
    }
    return -1;
}

bool parseIntegerList(std::vector<int>& intList, std::string intListStr) {

    bool ret = true;
    std::stringstream ss(intListStr);
    std::string value;
    while (getline(ss, value, ' ')) {
        std::string trimmedValue = trimWhitespaces(value);
        if (trimmedValue.size() > 0) {
            int intValue = atoi(trimmedValue.c_str());
            if (intValue != 0 || 0 == trimmedValue.compare("0")) {
                intList.push_back(intValue);
            } else {
                gardenError("Invalid int value [%s]", trimmedValue.c_str());
                ret = false;
            }
        }
    }
    return ret;
}

bool parseUInt8List(std::vector<uint8_t>& uint8List, std::string uint8ListStr) {

    bool ret = true;
    std::stringstream ss(uint8ListStr);
    std::string value;
    while (getline(ss, value, ' ')) {
        std::string trimmedValue = trimWhitespaces(value);
        if (trimmedValue.size() > 0) {
            int intValue = atoi(trimmedValue.c_str());
            if ((intValue != 0 || 0 == trimmedValue.compare("0")) &&
                    intValue >= 0 && intValue <= 255)  {
                uint8List.push_back(intValue);
            } else {
                gardenError("Invalid uint8_t value [%s]", trimmedValue.c_str());
                ret = false;
            }
        }
    }
    return ret;
}

// Parsing optarg string
bool parseLocGpsPositionMode(LocGpsPositionMode& mode, char opt, std::string modeStr) {

    std::transform(modeStr.begin(), modeStr.end(), modeStr.begin(),
            [](unsigned char c) -> unsigned char { return std::tolower(c); });
    if (0 == modeStr.compare("standalone")) {
        mode = LOC_GPS_POSITION_MODE_STANDALONE;
    } else if (0 == modeStr.compare("msb")) {
        mode = LOC_GPS_POSITION_MODE_MS_BASED;
    } else if (0 == modeStr.compare("msa")) {
        mode = LOC_GPS_POSITION_MODE_MS_ASSISTED;;
    } else {
        gardenError("Invalid mode value -[%c] [%s]", opt, modeStr.c_str());
        return false;
    }
    return true;
}

bool parseLocGpsPositionRecurrence(
        LocGpsPositionRecurrence& recurrence, char opt, std::string recurrenceStr) {

    std::transform(recurrenceStr.begin(), recurrenceStr.end(), recurrenceStr.begin(),
            [](unsigned char c) -> unsigned char { return std::tolower(c); });
    if (0 == recurrenceStr.compare("periodic")) {
        recurrence = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
    } else if (0 == recurrenceStr.compare("single")) {
        recurrence = LOC_GPS_POSITION_RECURRENCE_SINGLE;
    } else {
        gardenError("Invalid recurrence value -[%c] [%s]", opt, optarg);
        return false;
    }
    return true;
}

bool parseMinIntervalMillis(uint32_t& minIntervalMillis, char opt, std::string minIntervalStr) {

    int interval = parseDurationMillis(minIntervalStr);
    if (interval >= 0) {
        minIntervalMillis = interval;
        return true;
    }
    return false;
}

bool parsePreferredAccuracyMeters(uint32_t& preferredAccuracyMeters, char opt, std::string optarg) {

    preferredAccuracyMeters = atoi(optarg.c_str());
    return true;
}

bool parsePreferredTimeMillis(
        uint32_t& preferredTimeMillis, char opt, std::string preferredTimeStr) {

    int time = parseDurationMillis(preferredTimeStr);
    if (time >= 0) {
        preferredTimeMillis = time;
        return true;
    }
    return false;
}

std::vector<std::string> strUtilTokenize(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    // Tokenize by whitespace
    for (std::string s; stream >> s; ) {
        tokens.push_back(s);
    }
    return tokens;
}

// constellationListStr must be a space separate list of constellation values
// Constellation values can be one of: gps / glo / gal / bds / qzss/ navic / gps
bool parseGnssSvTypesMask(std::string constellationListStr, GnssSvTypesMask& mask)
{
    mask = 0;
    std::vector<std::string> constellations = strUtilTokenize(constellationListStr);
    std::string constellation;
    for (auto itor = constellations.begin(); itor < constellations.end(); ++itor) {
        constellation = *itor;
        if (0 == constellation.compare("glo")) {
            mask |= GNSS_SV_TYPES_MASK_GLO_BIT;
        } else if (0 == constellation.compare("gal")) {
            mask |= GNSS_SV_TYPES_MASK_GAL_BIT;
        } else if (0 == constellation.compare("bds")) {
            mask |= GNSS_SV_TYPES_MASK_BDS_BIT;
        } else if (0 == constellation.compare("qzss")) {
            mask |= GNSS_SV_TYPES_MASK_QZSS_BIT;
        } else if (0 == constellation.compare("navic")) {
            mask |= GNSS_SV_TYPES_MASK_NAVIC_BIT;
        } else if (0 == constellation.compare("gps")) {
            mask |= GNSS_SV_TYPES_MASK_GPS_BIT;
        } else {
            gardenPrint("Invalid constellation string [%s]", constellation.c_str());
            return false;
        }
    }
    return true;
}

void gnssSvTypesMaskToDisabledConstellationVec(
        GnssSvTypesMask enableMask, GnssSvTypesMask disableMask,
        std::vector<uint8_t>& disabledVec) {

    // add disabled constellations
    if (GNSS_SV_TYPES_MASK_GPS_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_GPS);
    }
    if (GNSS_SV_TYPES_MASK_GLO_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_GLONASS);
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_BEIDOU);
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_QZSS);
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_GALILEO);
    }
    if (GNSS_SV_TYPES_MASK_NAVIC_BIT & disableMask) {
        disabledVec.push_back(GNSS_SV_TYPE_NAVIC);
    }

    //enable constellation as inverted
    if (GNSS_SV_TYPES_MASK_GPS_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_GPS));
    }
    if (GNSS_SV_TYPES_MASK_GLO_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_GLONASS));
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_BEIDOU));
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_QZSS));
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_GALILEO));
    }
    if (GNSS_SV_TYPES_MASK_NAVIC_BIT & enableMask) {
        disabledVec.push_back(~((int)GNSS_SV_TYPE_NAVIC));
    }
}
} // namespace garden
