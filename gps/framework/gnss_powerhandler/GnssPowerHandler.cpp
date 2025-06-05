/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <loc_cfg.h>

#include "GnssPowerHandler.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocSvc_GnssPowerHandler"

// Static class variable initialization
GnssPowerHandler*  GnssPowerHandler::mInstance = nullptr;

#ifdef FEATURE_AUTOMOTIVE
// CB function invoked by Power Mngr lib to send power notification
static int pwrMngrLibStateCb(power_state_t pwr_state)
{
    GnssPowerHandler* autoPwrHdlr = GnssPowerHandler::getInstance();
    if (nullptr != autoPwrHdlr) {
        return autoPwrHdlr->sendPowerState(pwr_state);
    } else {
        syslog(LOG_ERR, "autoPwrHdlr is null");
        return 1;
    }
}

int GnssPowerHandler::sendPowerState(const power_state_t pwr_state) {
    client_ack_t client_ack;
    client_ack.ack = ERR;
    PowerStateType powerState = POWER_STATE_UNKNOWN;

    syslog(LOG_INFO, "sendPowerState: sys_state: %d", pwr_state.sys_state);
    switch (pwr_state.sys_state) {
        case SYS_SUSPEND:
            client_ack.ack = SUSPEND_ACK;
            powerState = POWER_STATE_SUSPEND;
            break;
        case SYS_RESUME:
            client_ack.ack = RESUME_ACK;
            powerState = POWER_STATE_RESUME;
            break;
        case SYS_SHUTDOWN:
            client_ack.ack = SHUTDOWN_ACK;
            powerState = POWER_STATE_SHUTDOWN;
            break;
    }

    syslog(LOG_INFO, "powerState: %d", powerState);
    if (powerState != POWER_STATE_UNKNOWN) {
        if (nullptr == mLocationControlApi) {
            mLocationControlApi = LocationControlAPI::getInstance();
        }
        if (nullptr != mLocationControlApi) {
            mLocationControlApi->powerStateEvent(powerState);
        } else {
            syslog(LOG_ERR, "mLocationControlApi is nullptr");
        }
    } else {
        syslog(LOG_ERR, "powerState is unknwn :%d", powerState);
    }

    //Allow some time to stop the session and write calibration data NVM.
    usleep(ACK_TIMEOUT_US);
    syslog(LOG_INFO, "PowerEvtHandler: pwrStateCb sending ack");
    send_acknowledgement(client_ack);

    return 0;
}
#elif OPENWRT_BUILD
// CB function invoked by Deep Sleep framework to send power notification
static int deepSleepClientCallBack(int dsEvent, void *pvtData1, void *pvtData2)
{
    GnssPowerHandler* leOwrtPwrHdlr = GnssPowerHandler::getInstance();
    if (nullptr != leOwrtPwrHdlr) {
        return leOwrtPwrHdlr->sendPowerState(dsEvent);
    } else {
        LOC_LOGe("leOwrtPwrHdlr is null");
        return 1;
    }
}

int GnssPowerHandler::sendPowerState(const int dsEvent)
{
    int retVal = 1;
    PowerStateType powerState = POWER_STATE_UNKNOWN;
    switch (dsEvent) {
    case DEEP_SLEEP_ENTER:
        powerState = POWER_STATE_DEEP_SLEEP_ENTRY;
        break;
    case DEEP_SLEEP_EXIT:
        powerState = POWER_STATE_DEEP_SLEEP_EXIT;
        break;
    default:
        break;
    }

    LOC_LOGi("sendPowerState dsEvt:%d, pwrState:%d", dsEvent, powerState);
    if (powerState != POWER_STATE_UNKNOWN) {
        if (nullptr == mLocationControlApi) {
            mLocationControlApi = LocationControlAPI::getInstance();
        }
        if (nullptr != mLocationControlApi) {
            mLocationControlApi->powerStateEvent(powerState);
            retVal = 0;
        } else {
            LOC_LOGe("mLocationControlApi is nullptr");
        }
    } else {
        LOC_LOGe("powerState is unknwn :%d", powerState);
    }

    return retVal;
}
#endif

GnssPowerHandler* GnssPowerHandler::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new GnssPowerHandler();
    }
    return mInstance;
}

GnssPowerHandler::GnssPowerHandler()
{
    int retVal = 1;
#ifdef FEATURE_AUTOMOTIVE
    retVal = pwr_state_notification_register(pwrMngrLibStateCb);
    syslog(LOG_INFO, "PowerEvtHandler: pwr_state_notification_register returned: %d", retVal);
#elif OPENWRT_BUILD
    retVal = deepsleep_register_callback(deepSleepClientCallBack,
            (char *)GNSS_DEEPSLEEP_CLIENT_NAME, GNSS_DEEPSLEEP_CLIENT_PRIORITY, NULL);
    LOC_LOGi("deepsleep_register ret %d", retVal);
#endif
    mLocationControlApi = LocationControlAPI::getInstance();
}

GnssPowerHandler::~GnssPowerHandler()
{
#ifdef FEATURE_AUTOMOTIVE
    syslog(LOG_INFO, "GnssPowerHandler destr");
#elif OPENWRT_BUILD
    int retVal = deepsleep_unregister_callback(deepSleepClientCallBack,
            (char *)GNSS_DEEPSLEEP_CLIENT_NAME, GNSS_DEEPSLEEP_CLIENT_PRIORITY, NULL);
    LOC_LOGi("deepsleep_unregister ret %d", retVal);
#endif
}

static GnssPowerHandler* gGnssPwrHdlrImpl = nullptr;

extern "C" int initGnssPowerHandler()
{
    int retVal = 1;
    if (nullptr == gGnssPwrHdlrImpl) {
        gGnssPwrHdlrImpl = GnssPowerHandler::getInstance();
    }
    if (nullptr != gGnssPwrHdlrImpl) {
        retVal = 0;
    }
    return retVal;
}
