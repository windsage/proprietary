/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GardenUtil.h"
#include "IGnssApiCase.h"
#include "GnssLocationAPI.h"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <map>

#ifdef __ANDROID__
#include "GfwAidlApi.h"
#endif

using std::lock_guard;

using namespace garden;

class LocMsgWrapper : public LocMsg {
public:
    LocMsgWrapper(Runnable& runnable) : mRunnable(move(runnable)) {
    }

    virtual void proc() const {
        mRunnable();
    }
private:
    Runnable mRunnable;
};

IGnssAPI* IGnssApiCase::mGnssAPI = nullptr;

IGnssApiCase::IGnssApiCase() :
    mCb(nullptr),
    mMsgTask("garden::IGnssApiCase") {
}

IGnssApiCase::~IGnssApiCase() {}

GARDEN_RESULT IGnssApiCase::preRun() {
    lock_guard<std::mutex> guard(mLock);
    if (getGnssAPI() == nullptr) {
        GARDEN_ERROR("getGnssAPI() failed.");
    }
    if (!mCb) {
        mCb = new IGnssApiCb(this);
        mGnssAPI->setGnssCbs(mCb);
    }
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT IGnssApiCase::postRun() {

    if (mGnssAPI) {
        mGnssAPI->setGnssCbs(nullptr);
    }
    if (mCb) {
        delete mCb;
        mCb = nullptr;
    }
    return GARDEN_RESULT_PASSED;
}

static std::map<int, std::string> parseOptions(int argc, char* argv[], const char* optStr) {

    extern char *optarg;
    int opt = -1;
    std::map<int, std::string> options;
    optind = 0; // reset getopt global state
    while ((opt = getopt(argc, argv, optStr)) != -1) {
        if (optarg != NULL) {
            std::string optargStr(optarg);
            options[opt] = optargStr;
        } else {
            options[opt] = "";
        }
    }
    return options;
}

GARDEN_RESULT IGnssApiCase::loadFromCmdStr(std::string cmdListStr) {

    GARDEN_RESULT result = GARDEN_RESULT_PASSED;

    gardenPrint("IGnssApiCase::loadFromCmdStr [%s] cmdLen %d",
                    cmdListStr.c_str(), cmdListStr.size());

    // The command list string will be semicolon (;) separated list
    // of commands.
    std::stringstream cmdListSS(cmdListStr);
    std::vector<std::string> commandList;
    std::string command;

    while (getline(cmdListSS, command, ';')) {
        std::string trimmedCmd = trimWhitespaces(command);
        if (trimmedCmd.size() > 0) {
            commandList.push_back(trimmedCmd);
        }
    }

    gardenPrint("Parsed %d commands", commandList.size());

    // Process each command
    for (auto cmd: commandList) {
        auto res = processCommand(cmd);
        if (GARDEN_RESULT_PASSED != res) {
            result = res;
        }
    }

    return result;
}

GARDEN_RESULT IGnssApiCase::loadFromCmdFile(std::string cmdFile) {

    // todo: implement
    gardenPrint("IGnssApiCase::loadFromCmdFile [%s] TBD", cmdFile.c_str());
    return GARDEN_RESULT_FAILED;
}

GARDEN_RESULT IGnssApiCase::loadFromLocIpc() {

    // todo: implement
    gardenPrint("IGnssApiCase::loadFromLocIpc TBD");
    return GARDEN_RESULT_FAILED;
}

GARDEN_RESULT IGnssApiCase::processCommand(std::string cmdStr) {

    gardenPrint("\n--- processCommand [%s] entry ---", cmdStr.c_str());

    // Extract the command name and invoke corresponding handler
    std::string command = cmdStr.substr(0, cmdStr.find(" "));

    int ret = -1;
    if (0 == command.compare("gnssStart")) {
        ret = cmdHandlerGnssStart(cmdStr);
    } else if (0 == command.compare("gnssStop")) {
        ret = cmdHandlerGnssStop(cmdStr);
    } else if (0 == command.compare("gnssSetPositionMode")) {
        ret = cmdHandlerGnssSetPositionMode(cmdStr);
    } else if (0 == command.compare("gnssVisibilityCtrlEnableNfwLocationAccess")) {
        ret = cmdHandlerGnssVisibilityCtrlEnableNfwLocationAccess(cmdStr);
    } else if (0 == command.compare("gnssConfigSetBlacklist")) {
        ret = cmdHandlerGnssConfigSetBlacklist(cmdStr);
    } else if (0 == command.compare("setGnssSvTypeConfig")) {
        ret = cmdHandlerSetGnssSvTypeConfig(cmdStr);
    } else if (0 == command.compare("sleep")) {
        ret = cmdHandlerSleep(cmdStr);
    } else if (0 == command.compare("help")) {
        ret = cmdHandlerHelp(cmdStr);
    } else {
        gardenPrint("No command handler found for command [%s]", command.c_str());
    }

    cmdHelpGnssStart();

    gardenPrint("--- processCommand [%s] exit ---\n", cmdStr.c_str());
    return ret == 0 ? GARDEN_RESULT_PASSED: GARDEN_RESULT_FAILED;
}

int IGnssApiCase::cmdHandlerGnssStart(std::string cmdStr) {

    gardenPrint("gnssStart");
    return getGnssAPI()->gnssStart();
}
void IGnssApiCase::cmdHelpGnssStart() {
}

int IGnssApiCase::cmdHandlerGnssStop(std::string cmdStr) {

    gardenPrint("gnssStop");
    return getGnssAPI()->gnssStop();
}
void IGnssApiCase::cmdHelpGnssStop() {
}

int IGnssApiCase::cmdHandlerGnssSetPositionMode(std::string cmdStr) {

    LocGpsPositionMode mode = LOC_GPS_POSITION_MODE_MS_BASED;
    LocGpsPositionRecurrence recurrence = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
    uint32_t minIntervalMillis = 1000;
    uint32_t preferredAccuracyMeters = 0;
    uint32_t preferredTimeMillis = 0;
    bool lowPowerMode = false;

    // Parse arguments from cmdStr
    char argvBuffer[20][MAX_ARGUMENT_LEN];
    memset(argvBuffer, 0, 20*MAX_ARGUMENT_LEN);
    char* argv[20];
    for (int i = 0; i < 20; i++) {
        argv[i] = &argvBuffer[i][0];
    }
    int argc = parseStrToArgv(cmdStr, argv, 20, MAX_ARGUMENT_LEN);
    bool argsOk = true, executeApi = true;
    std::map<int, std::string> options = parseOptions(argc, argv, "m:r:i:a:t:p:h");
    for (auto it = options.begin(); it != options.end(); ++it) {
        int opt = it->first;
        std::string optarg = it->second;
        switch (opt) {
        case 'm':
            argsOk &= parseLocGpsPositionMode(mode, opt, optarg);
            break;
        case 'r':
            argsOk &= parseLocGpsPositionRecurrence(recurrence, opt, optarg);
            break;
        case 'i':
            argsOk &= parseMinIntervalMillis(minIntervalMillis, opt, optarg);
            break;
        case 'a':
            argsOk &= parsePreferredAccuracyMeters(preferredAccuracyMeters, opt, optarg);
            break;
        case 't':
            argsOk &= parsePreferredTimeMillis(preferredTimeMillis, opt, optarg);
            break;
        case 'p':
            lowPowerMode = (atoi(optarg.c_str()) != 0);
            break;
        case 'h':
            cmdHelpGnssSetPositionMode();
            executeApi = false;
            break;
        default:
            GARDEN_ERROR("Invalid option: %c", opt);
            cmdHelpGnssSetPositionMode();
            executeApi = false;
            break;
        }
    }

    if (!argsOk) {
        gardenError("Invalid args.");
        return -1;
    }

    if (executeApi) {
        gardenPrint("Invoking gnssSetPositionMode(mode=%d, recurrence=%d, "
            "minIntervalMillis=%d, preferredAccuracyMeters=%d, preferredTimeMillis=%d, "
            "lowPowerMode=%d)", mode, recurrence, minIntervalMillis, preferredAccuracyMeters,
            preferredTimeMillis, lowPowerMode);

        return getGnssAPI()->gnssSetPositionMode(
            mode, recurrence, minIntervalMillis, preferredAccuracyMeters, preferredTimeMillis,
            lowPowerMode);
    } else {
        return 0;
    }
}
void IGnssApiCase::cmdHelpGnssSetPositionMode() {

    gardenPrint("");
    gardenPrint("usage: gnssSetPositionMode [<options>]");
    gardenPrint("-m \t\t Set Position Mode");
    gardenPrint("   \t\t Can be one of below values:");
    gardenPrint("   \t\t - standalone");
    gardenPrint("   \t\t - msb");
    gardenPrint("   \t\t - msa");
    gardenPrint("   \t\t e.g. -m msb");
    gardenPrint("");
}

int IGnssApiCase::cmdHandlerGnssVisibilityCtrlEnableNfwLocationAccess(std::string cmdStr) {

    gardenPrint("gnssVisibilityCtrlEnableNfwLocationAccess");

    // API arguments
    const ::std::vector<::std::string> proxyApps;

    // todo :: parse arguments from cmdStr

    return getGnssAPI()->gnssVisibilityCtrlEnableNfwLocationAccess(proxyApps);
}
void IGnssApiCase::cmdHelpGnssVisibilityCtrlEnableNfwLocationAccess() {

}

int IGnssApiCase::cmdHandlerGnssConfigSetBlacklist(std::string cmdStr) {

    // API arguments
    std::vector<int> constellation;
    std::vector<int> svIDs;

    // Parse arguments from cmdStr
    char argvBuffer[20][MAX_ARGUMENT_LEN];
    memset(argvBuffer, 0, 20*MAX_ARGUMENT_LEN);
    char* argv[20];
    for (int i = 0; i < 20; i++) {
        argv[i] = &argvBuffer[i][0];
    }
    int argc = parseStrToArgv(cmdStr, argv, 20, MAX_ARGUMENT_LEN);
    bool argsOk = true, executeApi = true;
    std::map<int, std::string> options = parseOptions(argc, argv, "c:s:h");
    for (auto it = options.begin(); it != options.end(); ++it) {
        int opt = it->first;
        std::string optarg = it->second;
        switch (opt) {
        case 'c':
            argsOk &= parseIntegerList(constellation, optarg);
            break;
        case 's':
            argsOk &= parseIntegerList(svIDs, optarg);
            break;
        case 'h':
            cmdHelpGnssConfigSetBlacklist();
            executeApi = false;
            break;
        default:
            GARDEN_ERROR("Invalid option: %c", opt);
            cmdHelpGnssConfigSetBlacklist();
            executeApi = false;
            break;
        }
    }

    if (!argsOk) {
        gardenError("Invalid args.");
        return -1;
    }

    if (executeApi) {
        gardenPrint("Invoking gnssConfigSetBlacklist("
            "constellation list [%s] size %d, SV ID list [%s] size %d)",
            vectorToString<int>(constellation).c_str(), constellation.size(),
            vectorToString<int>(svIDs).c_str(), svIDs.size());
        return getGnssAPI()->gnssConfigSetBlacklist(constellation, svIDs);
    } else {
        return 0;
    }
}
void IGnssApiCase::cmdHelpGnssConfigSetBlacklist() {

    gardenPrint("");
    gardenPrint("usage: gnssConfigSetBlacklist [<options>]");
    gardenPrint("-c \t\t Set constellation list");
    gardenPrint("   \t\t e.g. -c \"3 3 3\"");
    gardenPrint("-s \t\t Set SV ID list");
    gardenPrint("   \t\t e.g. -s \"123 124 125\"");
    gardenPrint("");
}

int IGnssApiCase::cmdHandlerSetGnssSvTypeConfig(std::string cmdStr) {

    // API arguments
    std::vector<uint8_t> disableVec;
    GnssSvTypesMask enableMask = 0, disableMask = 0;

    // Parse arguments from cmdStr
    char argvBuffer[20][MAX_ARGUMENT_LEN];
    memset(argvBuffer, 0, 20*MAX_ARGUMENT_LEN);
    char* argv[20];
    for (int i = 0; i < 20; i++) {
        argv[i] = &argvBuffer[i][0];
    }
    int argc = parseStrToArgv(cmdStr, argv, 20, MAX_ARGUMENT_LEN);
    bool argsOk = true, executeApi = true;
    std::map<int, std::string> options = parseOptions(argc, argv, "e:d:h");
    for (auto it = options.begin(); it != options.end(); ++it) {
        int opt = it->first;
        std::string optarg = it->second;
        switch (opt) {
        case 'e':
            argsOk &= parseGnssSvTypesMask(optarg, enableMask);
            break;
        case 'd':
            argsOk &= parseGnssSvTypesMask(optarg, disableMask);
            break;
        case 'h':
            cmdHelpSetGnssSvTypeConfig();
            executeApi = false;
            break;
        default:
            gardenPrint("Invalid option: %d", opt);
            cmdHelpSetGnssSvTypeConfig();
            argsOk = false;
            executeApi = false;
            break;
        }
    }

    if (!argsOk) {
        gardenError("Invalid args.");
        return -1;
    }

    if (executeApi) {

        gardenPrint("EnableMask 0x%" PRIx64 " DisableMask 0x%" PRIx64 "", enableMask, disableMask);
        gnssSvTypesMaskToDisabledConstellationVec(enableMask, disableMask, disableVec);

        gardenPrint("Invoking setGnssSvTypeConfig(disableVec [%s] size %d)",
            vectorToString<uint8_t>(disableVec).c_str(), disableVec.size());
        getGnssAPI()->setGnssSvTypeConfig(disableVec);
    }

    return 0;
}
void IGnssApiCase::cmdHelpSetGnssSvTypeConfig() {

    gardenPrint("");
    gardenPrint("usage: setGnssSvTypeConfig [<options>]");
    gardenPrint("-e \t\t Set constellation list to be enabled");
    gardenPrint("   \t\t e.g. -e \"gps glo\"");
    gardenPrint("-d \t\t Set constellation list to be disabled");
    gardenPrint("   \t\t e.g. -d \"gps glo\"");
    gardenPrint("List of constellation str: [gps glo gal bds qzss navic]")
    gardenPrint("");
}

int IGnssApiCase::cmdHandlerSleep(std::string cmdStr) {

    int durationMillis = -1;

    // Parse arguments from cmdStr
    char argvBuffer[20][MAX_ARGUMENT_LEN];
    memset(argvBuffer, 0, 20*MAX_ARGUMENT_LEN);
    char* argv[20];
    for (int i = 0; i < 20; i++) {
        argv[i] = &argvBuffer[i][0];
    }
    int argc = parseStrToArgv(cmdStr, argv, 20, MAX_ARGUMENT_LEN);
    bool argsOk = true, executeApi = true;
    std::map<int, std::string> options = parseOptions(argc, argv, "t:h");
    for (auto it = options.begin(); it != options.end(); ++it) {
        int opt = it->first;
        std::string optarg = it->second;
        switch (opt) {
        case 't':
            durationMillis = parseDurationMillis(optarg);
            argsOk &= (durationMillis > 0);
            break;
        case 'h':
            cmdHelpSleep();
            break;
        default:
            GARDEN_ERROR("Invalid option: %c", opt);
            cmdHelpSleep();
            break;
        }
    }

    if (!argsOk) {
        gardenError("Invalid args.");
        return -1;
    }

    if (executeApi) {
        gardenPrint("Going to sleep for %d milliseconds", durationMillis);
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMillis));
    }

    return 0;
}
void IGnssApiCase::cmdHelpSleep() {

}

int IGnssApiCase::cmdHandlerHelp(std::string cmdStr) {

    gardenPrint("List of supported commands below. "
        "Use -h option with any command name to fetch command help.");
    gardenPrint("- gnssStart");
    gardenPrint("- gnssStop");
    gardenPrint("- gnssSetPositionMode");
    gardenPrint("- gnssConfigSetBlacklist");

    return 0;
}

void IGnssApiCase::sendMsg(LocMsg* msg) const {

    if (msg != nullptr) {
        mMsgTask.sendMsg(msg);
    }
}

void IGnssApiCase::sendMsg(Runnable& runnable) const {
    mMsgTask.sendMsg(new LocMsgWrapper(runnable));
}

IGnssAPI* IGnssApiCase::getGnssAPI() {

    if (!mGnssAPI) {
        gardenPrint("Creating remote api.");

#ifdef __ANDROID__
        mGnssAPI = new GfwAidlApi();
#endif
        if (!mGnssAPI) {
            gardenPrint("Failed to create remote api.");
        }
    }
    return mGnssAPI;
}

void IGnssApiCase::destroyGnssAPI() {

    if (mGnssAPI) {
        mGnssAPI->setGnssCbs(nullptr);
        delete mGnssAPI;
        mGnssAPI = nullptr;
    }
}
