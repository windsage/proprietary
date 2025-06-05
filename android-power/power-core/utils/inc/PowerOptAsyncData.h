/******************************************************************************
 *   @file    PowerOptAsyncData.h
 *   @brief   AsyncTrigger related helpers
 *
 *   DESCRIPTION
 *     AsyncTrigger related helper functions
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef POWEROPTASYNCDATA_H
#define POWEROPTASYNCDATA_H
#include <string>
#include <utils/Timers.h>
#include <stdlib.h>

#define DEFAULT_HINTTYPE_POWERMODULE -1
#define MAX_STR_LEN 128
#define STANDBY_ID 0x005F
#define VENDOR_HINT_USE_PSM  0x000015E0

/**
 * AsyncTriggerPayload is payload received from perf HAL along with the Async
 * triggers. This is just an alternate internal LM representation of
 * powerctl_msg_t.
 *
 * The powerctl_msg_t type has the following members:
 *  Name            type          Comments
 *  data           uint16_t      PerfLock specific (number of perflock args)
 *  pl_handle      int32_t       PerfLock specific (perflock handle)
 *  req_type       uint8_t       PerfLock specific (perflock type - would be HINT for us)
 *  profile        int32_t       PerfLock specific (profile)
 *  pl_time        int32_t       Duration of Hint
 *  pl_args        int32_t[]     PerfLock specific (perflock args)
 *  client_pid     pid_t         PID of the client
 *  client_tid     pid_t         TID of the client
 *  hint_id        uint32_t      Actual Async Hint
 *  hint_type      int32_t       Async Hint may have a subtype
 *  *userdata      void          Reserved - Not used currently
 *  usrdata_str    char[]        Appname
 *
 * All the members relevant to LM (non-perflock-specific members) are mapped
 * to equivalent internal members in AsyncTriggerPayload.
 */

 struct powerctl_msg_t {
        uint32_t hintID;
        int32_t hintType;
        int32_t duration;
        char appName[MAX_STR_LEN];
        pid_t appPID;
        pid_t appThreadTID;
        nsecs_t timeStamp;
};

struct AsyncTriggerPayload {
    uint32_t hintID;    /* powerctl_msg_t.hint_id */
    int32_t hintType;           /* powerctl_msg_t.hint_type */
    std::string appName;    /* powerctl_msg_t.usrdata_str */
    int32_t duration;           /* powerctl_msg_t.pl_time */
    pid_t appPID;           /* powerctl_msg_t.client_pid */
    pid_t appThreadTID;     /* powerctl_msg_t.client_tid */
    nsecs_t timeStamp;      /* Time when the Async arrived */

    /* Default Constructor */
    AsyncTriggerPayload() {
        hintID = 0;
        hintType = DEFAULT_HINTTYPE_POWERMODULE;
        duration = 0;
        appPID = 0;
        appThreadTID = 0;
        timeStamp = 0;
    }

    /* Constructor from powerctl_msg_t */
    AsyncTriggerPayload(powerctl_msg_t* msg) {
        hintID = msg->hintID;
        hintType = msg->hintType;
        appName = msg->appName;
        duration = msg->duration;
        appPID = msg->appPID;
        appThreadTID = msg->appThreadTID;
        timeStamp = 0;
    }
};


#endif /* POWEROPTASYNCDATA_H */
