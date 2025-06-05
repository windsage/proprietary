/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IGNSSAPI_CASE_H
#define IGNSSAPI_CASE_H

#include <condition_variable>
#include <LocTimer.h>
#include <MsgTask.h>
#include "IGardenCase.h"
#include "IGnssAPI.h"
#include "IGnssApiCb.h"
#include "GardenUtil.h"

using namespace loc_util;

namespace garden {

using Runnable = std::function<void()>;

class IGnssApiCase : public IGardenCase {

    friend class IGnssApiCb;

public:
    IGnssApiCase();
    virtual ~IGnssApiCase();

    virtual GARDEN_RESULT preRun() override;
    virtual GARDEN_RESULT postRun() override;

    void sendMsg(LocMsg* msg) const;
    void sendMsg(Runnable& runnable) const;

    // Command Handlers :: each corresponds to an API in IGnssApi
    int cmdHandlerGnssStart(std::string cmdStr);
    int cmdHandlerGnssStop(std::string cmdStr);
    int cmdHandlerGnssSetPositionMode(std::string cmdStr);
    int cmdHandlerGnssVisibilityCtrlEnableNfwLocationAccess(std::string cmdStr);
    int cmdHandlerGnssConfigSetBlacklist(std::string cmdStr);
    int cmdHandlerSetGnssSvTypeConfig(std::string cmdStr);

    // Utility commands - do not correspond to any API in IGnssApi
    int cmdHandlerSleep(std::string cmdStr);
    int cmdHandlerHelp(std::string cmdStr);

    // Command Helpers :: prints help for the commands
    void cmdHelpGnssStart();
    static void cmdHelpGnssStop();
    void cmdHelpGnssSetPositionMode();
    void cmdHelpGnssVisibilityCtrlEnableNfwLocationAccess();
    void cmdHelpGnssConfigSetBlacklist();
    void cmdHelpSetGnssSvTypeConfig();
    void cmdHelpSleep();

    // Receive commands over LocIpc and execute
    GARDEN_RESULT loadFromLocIpc();

    // Parse semicolon separated list of commands to execute
    GARDEN_RESULT loadFromCmdStr(std::string cmdListStr);

    // Parse commands from a file and execute
    GARDEN_RESULT loadFromCmdFile(std::string cmdFile);

    // Process a command
    GARDEN_RESULT processCommand(std::string cmdStr);

    static IGnssAPI* getGnssAPI();
    static void destroyGnssAPI();

public:
    mutex mLock;
    static IGnssAPI* mGnssAPI;
protected:
    GnssCbBase* mCb;
private:
    MsgTask mMsgTask;
};

} // namespace garden
#endif // IGNSSAPI_CASE_H
