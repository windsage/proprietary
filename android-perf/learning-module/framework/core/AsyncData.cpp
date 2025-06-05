/******************************************************************************
 *   @file    AsyncData.cpp
 *   @brief   AsyncId and AsyncTrigger related helpers
 *
 *   DESCRIPTION
 *      AsyncId is the internal representation of hintID from perf-hal. These
 *   definitions are used to map perf-hal provided hintID's and request to their
 *   internal representations.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017-2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "AsyncData.h"
#include "vector"
#include "DynamicLoader.h"

/* Default Constructor */
AsyncTriggerPayload::AsyncTriggerPayload() {
    hintID = 0;
    hintType = DEFAULT_HINTTYPE;
    duration = 0;
    appPID = 0;
    appThreadTID = 0;
    timeStamp = 0;
    handle = 0;
    version = 0;
    size = 0;
    feedback = NULL;
}

/* Constructor from mpctl_msg_t */
AsyncTriggerPayload::AsyncTriggerPayload(mpctl_msg_t* msg) {
    if(msg == NULL) {
        DEBUGE(LOG_TAG_FEATURE, "Error: msg is null");
        return;
    }
    try {
        if(msg->size != sizeof(mpctl_msg_t)) {
            DEBUGE(LOG_TAG_FEATURE, "mpctl_msg_t is not compatible. Hence, Discarding Request.");
            return;
        }
        uint16_t numMaxArgs = 0;
        if(DynamicLoader::getInstance().mCurrVendor == NULL) {
            DEBUGE(LOG_TAG_FEATURE, "mCurrVendor is null!! Can't update numMaxArgs.");
        } else {
            numMaxArgs = (DynamicLoader::getInstance().mCurrVendor)->getMaxArgsPerReq();
        }
        hintID = msg->hint_id;
        hintType = msg->hint_type;
        appName = msg->usrdata_str;
        duration = msg->pl_time;
        appPID = msg->client_pid;
        appThreadTID = msg->client_tid;
        timeStamp = 0;
        handle = msg->pl_handle;
        app_workload_type = msg->app_workload_type;
        app_pid = msg->app_pid;
        version = msg->version;
        size = sizeof(AsyncTriggerPayload);
        argListSz = 0;
        feedback = NULL;
        if (msg->data <= numMaxArgs) {
            argList = std::vector<int32_t> (numMaxArgs, 0);
            argListSz = msg->data;
            for (uint16_t i = 0 ; i < msg->data; i++) {
                argList[i] = msg->pl_args[i];
            }
        }
    } catch (std::exception &e) {
        DEBUGE(LOG_TAG_FEATURE, "Caught exception: %s in %s",e.what(), __func__);
    } catch (...) {
        appName.clear();
        DEBUGE(LOG_TAG_FEATURE, "Caught Exception in %s",__func__);
    }
}

AsyncIdMap::AsyncIdMap() {
    this->operator []("VENDOR_HINT_FIRST_LAUNCH_BOOST") = VENDOR_HINT_FIRST_LAUNCH_BOOST;
    this->operator []("VENDOR_HINT_FIRST_DRAW") = VENDOR_HINT_FIRST_DRAW;
    this->operator []("VENDOR_HINT_DISPLAY_OFF") = VENDOR_HINT_DISPLAY_OFF;
    this->operator []("VENDOR_HINT_DISPLAY_ON") = VENDOR_HINT_DISPLAY_ON;
    this->operator []("VENDOR_HINT_TAP_EVENT") = VENDOR_HINT_TAP_EVENT;
    this->operator []("VENDOR_HINT_APP_WORKLOAD_TYPE") = VENDOR_HINT_APP_WORKLOAD_TYPE;
    this->operator []("VENDOR_HINT_WORKLOAD_HEAVY") = VENDOR_HINT_WORKLOAD_HEAVY;
    this->operator []("VENDOR_HINT_ACTIVITY_START") = VENDOR_HINT_ACTIVITY_START;
    this->operator []("VENDOR_HINT_ACTIVITY_STOP") = VENDOR_HINT_ACTIVITY_STOP;
    this->operator []("VENDOR_HINT_ACTIVITY_RESUME") = VENDOR_HINT_ACTIVITY_RESUME;
    this->operator []("VENDOR_HINT_ACTIVITY_PAUSE") = VENDOR_HINT_ACTIVITY_PAUSE;
    this->operator []("VENDOR_HINT_KILL") = VENDOR_HINT_KILL;
    this->operator []("VENDOR_HINT_APP_UPDATE") = VENDOR_HINT_APP_UPDATE;
    this->operator []("VENDOR_HINT_SENSOR_DATA") = VENDOR_HINT_SENSOR_DATA;
    this->operator []("VENDOR_HINT_HEADROOM_REGULATOR") = VENDOR_HINT_HEADROOM_REGULATOR;
    this->operator []("VENDOR_HINT_THERMAL_UPDATE") = VENDOR_HINT_THERMAL_UPDATE;
    this->operator []("VENDOR_HINT_TOGGLE_GAMEOPT") = VENDOR_HINT_TOGGLE_GAMEOPT;
    this->operator []("VENDOR_HINT_PICARD_TOP_APP") = VENDOR_HINT_PICARD_TOP_APP;
    this->operator []("VENDOR_HINT_PICARD_RENDER_RATE") = VENDOR_HINT_PICARD_RENDER_RATE;
    this->operator []("VENDOR_HINT_PICARD_LOW_LAT") = VENDOR_HINT_PICARD_LOW_LAT;
    this->operator []("VENDOR_HINT_PICARD_HIGH_CPUUTIL") = VENDOR_HINT_PICARD_HIGH_CPUUTIL;
    this->operator []("VENDOR_HINT_PICARD_LOAD_CHANGED") = VENDOR_HINT_PICARD_LOAD_CHANGED;
    this->operator []("VENDOR_HINT_PICARD_THREAD_PIPELINE") = VENDOR_HINT_PICARD_THREAD_PIPELINE;
    this->operator []("VENDOR_HINT_SCROLL_BOOST") = VENDOR_HINT_SCROLL_BOOST;
    this->operator []("VENDOR_HINT_FD_COUNT") = VENDOR_HINT_FD_COUNT;
    this->operator []("VENDOR_HINT_DRAG_START") = VENDOR_HINT_DRAG_START;
    this->operator []("VENDOR_HINT_DRAG_END") = VENDOR_HINT_DRAG_END;
    this->operator []("VENDOR_HINT_BINDAPP") = VENDOR_HINT_BINDAPP;
    this->operator []("VENDOR_HINT_WARM_LAUNCH") = VENDOR_HINT_WARM_LAUNCH;
    this->operator []("VENDOR_HINT_PKG_SPEED") = VENDOR_HINT_PKG_SPEED;
    this->operator []("VENDOR_HINT_PKG_INSTALL") = VENDOR_HINT_PKG_INSTALL;
    this->operator []("VENDOR_HINT_PKG_UNINSTALL") = VENDOR_HINT_PKG_UNINSTALL;
    this->operator []("VENDOR_HINT_PICARD_HINT_SCENARIO") = VENDOR_HINT_PICARD_HINT_SCENARIO;
    this->operator []("VENDOR_HINT_GAME_ACTION_DISABLE") = VENDOR_HINT_GAME_ACTION_DISABLE;
}

AsyncIdMap::~AsyncIdMap() {}
