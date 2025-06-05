/******************************************************************************
  @file    Request.cpp
  @brief   Implementation of perflock requests

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-REQUEST"
#include <cstdio>
#include <cstring>
#include <pthread.h>

#include <time.h>
#include "PerfController.h"
#include "Request.h"
#include "ResourceInfo.h"
#include "MpctlUtils.h"
#include "ResourceQueues.h"
#include "client.h"
#include "PerfLog.h"

#define UNSUPPORTED -1

Request::Request(uint32_t duration, pid_t client_pid, pid_t client_tid, enum client_t client) {
    mVersion = LOCK_UNKNOWN_VERSION;
    mTimerCreated = false;
    mTimer = 0;
    mPid = client_pid;
    mTid = client_tid;
    mClient = client;
    mDuration = duration;
    mNumArgs = 0;
    mResources = NULL;
    mPriority = REGULAR_PRIORITY;
    mCluster = TargetConfig::getTargetConfig().getNumCluster();

}

Request::~Request() {
    DeleteTimer();
    if (mResources) {
        delete[] mResources;
        mResources = NULL;
    }
    mPriority = REGULAR_PRIORITY;
}

Request::Request(Request const& req) {
    mVersion = req.mVersion;
    mTimer = req.mTimer;
    mPid = req.mPid;
    mTid = req.mTid;
    mClient = req.mClient;
    mDuration = req.mDuration;
    mNumArgs = req.mNumArgs;
    mPriority = req.mPriority;
    if (mResources == NULL) {
        if (mNumArgs > 0) {
            mResources = new(std::nothrow) ResourceInfo[mNumArgs];
        }
    }
    if (mResources && req.mResources) {
        for (unsigned int i = 0 ; i < mNumArgs; i++) {
            mResources[i] = req.mResources[i];
        }
    }
}

Request& Request::operator=(Request const& req) {
    mVersion = req.mVersion;
    mTimer = req.mTimer;
    mPid = req.mPid;
    mTid = req.mTid;
    mClient = req.mClient;
    mDuration = req.mDuration;
    mPriority = req.mPriority;
    if (mNumArgs < req.mNumArgs && mResources != NULL) {
        delete[] mResources;
        mResources = NULL;
    }
    mNumArgs = req.mNumArgs;
    if (mResources == NULL) {
        if (mNumArgs > 0) {
            mResources = new(std::nothrow) ResourceInfo[mNumArgs];
        }
    }
    if (mResources && req.mResources) {
        for (unsigned int i = 0 ; i < mNumArgs; i++) {
            mResources[i] = req.mResources[i];
        }
    }
    return *this;
}

bool Request::operator==(Request const& req) {
    bool ret = true;
    uint32_t i = 0;

    ret = (mDuration == req.mDuration) && (mNumArgs == req.mNumArgs);

    if (ret && mResources && req.mResources) {
        for (i=0; i<req.mNumArgs && i < mNumArgs; i++) {
            ret = (mResources[i] == req.mResources[i]);
            if (!ret) {
                break;
            }
        } /*for (i=0; i<req.mNumArgs; i++)*/
    }
    else {
        ret = false;
    }

    return ret;
}

bool Request::operator!=(Request const& req) {
    return !(*this == req);
}

uint32_t Request::GetNumLocks() {
    return mNumArgs;
}

ResourceInfo *Request::GetResource(uint32_t idx) {
    if (mResources && (idx < mNumArgs)) {
        return &mResources[idx];
    }
    else {
        return NULL;
    }
}

uint32_t Request::OverrideClientValue(uint16_t major, uint16_t minor,
                                                      uint32_t value) {
            if ((major == SCHED_MAJOR_OPCODE) &&
                    ((minor == SCHED_GROUP_OPCODE) ||
                    (minor == SCHED_FREQ_AGGR_GROUP_OPCODE &&
                    value == 1)))
                return mTid;
            return value;
}

bool Request::IsResAlwaysResetOnRelease(uint16_t major, uint16_t minor) {
            return ((major == SCHED_MAJOR_OPCODE) &&
                    ((minor == SCHED_GROUP_OPCODE) ||
                    (minor == SCHED_FREQ_AGGR_GROUP_OPCODE) ||
                    (minor == SCHED_BOOST_OPCODE)));
}

//backward compatibility for sched up/down migrate
bool Request::TranslateUpDownMigrate(int8_t upmigrate_idx[], int8_t downmigrate_idx[]) {
    int8_t up_idx, down_idx;
    uint32_t up_value, down_value, override_value;

    if (mCluster <= 0 || mCluster > MAX_CLUSTER ||
        upmigrate_idx == NULL || downmigrate_idx == NULL)
        return false;

    for (int8_t i = 0; i < mCluster; i++) {
        up_idx = upmigrate_idx[i];
        down_idx = downmigrate_idx[i];
        up_value = 0;
        down_value = 0;

        //no mig opcode for ith cluster in req, continue
        if ((up_idx == -1) && (down_idx == -1))
            continue;

        if (up_idx != -1 && down_idx != -1) {
            //combine both values into one
            up_value = mResources[up_idx].GetValue();
            down_value = mResources[down_idx].GetValue();
            if (up_value <= 0 || up_value > numeric_limits<short>::max() ||
                down_value <= 0 || down_value > numeric_limits<short>::max()) {
                QLOGE(LOG_TAG, "Invalid value found for migrate opcode");
                return false;
            }
            override_value = (down_value | (up_value << 16));

            //set one of the opcodes to new index
            mResources[up_idx].SetMinor(SCHED_UP_DOWN_MIGRATE);
            mResources[up_idx].DiscoverQueueIndex();
            mResources[up_idx].SetValue(override_value);
            //unsupport other one
            mResources[down_idx].SetMajor(MISC_MAJOR_OPCODE);
            mResources[down_idx].SetMinor(UNSUPPORTED_OPCODE);
            mResources[down_idx].DiscoverQueueIndex();
        } else {
            QLOGE(LOG_TAG, "Request rejected.Pass both up and down migrate values for %" PRId8 " cluster",i);
            return false;
        }
    }
    return true;
}

//backward compatibility for sched group up/down migrate
bool Request::TranslateGrpUpDownMigrate(int8_t grp_upmigrate_idx, int8_t grp_downmigrate_idx) {
    uint32_t up_value, down_value, override_value;

    if (grp_upmigrate_idx == -1 || grp_downmigrate_idx == -1)
        return false;

    up_value = 0;
    down_value = 0;

    //combine both values into one
    up_value = mResources[grp_upmigrate_idx].GetValue();
    down_value = mResources[grp_downmigrate_idx].GetValue();
    if (up_value <= 0 || up_value > numeric_limits<short>::max() ||
            down_value <= 0 || down_value > numeric_limits<short>::max()) {
        QLOGE(LOG_TAG, "Invalid value found for group migrate opcode");
        return false;
    }
    override_value = (down_value | (up_value << 16));

    //set one of the opcodes to new index
    mResources[grp_upmigrate_idx].SetMinor(SCHED_GROUP_UP_DOWN_MIGRATE);
    mResources[grp_upmigrate_idx].DiscoverQueueIndex();
    mResources[grp_upmigrate_idx].SetValue(override_value);
    //unsupport other one
    mResources[grp_downmigrate_idx].SetMajor(MISC_MAJOR_OPCODE);
    mResources[grp_downmigrate_idx].SetMinor(UNSUPPORTED_OPCODE);
    mResources[grp_downmigrate_idx].DiscoverQueueIndex();

    return true;
}

/*******************************************************************
 **   request processing which includes                           **
 **   1.parsing                                                   **
 **   2.validating                                                **
 **   3.physical translation                                      **
 **   4.backward compatibility                                    **
 *******************************************************************/
bool Request::Process(uint32_t nargs, int32_t list[]) {
    uint8_t version = 0;
    uint32_t i = 0;
    bool ret = true;
    int8_t state = LOCK_UNKNOWN_VERSION;
    bool potentialOldRequest = false;
    uint32_t opcode = 0;

    bool migrate_flag = false, grp_migrate_flag = false;
    int8_t upmigrate_idx[mCluster];
    int8_t downmigrate_idx[mCluster];
    memset(upmigrate_idx, -1, sizeof(upmigrate_idx));
    memset(downmigrate_idx, -1, sizeof(downmigrate_idx));
    int8_t grp_upmigrate_idx = -1, grp_downmigrate_idx = -1;

    if (NULL == list) {
        return false;
    }

    /** inital validation                                    **
     ** 1.num args checking                                  **
     ** 2.odd number of args in a new request                **
     ** 3.unsupported version                                */
    potentialOldRequest = ((nargs%2) != 0);
    version = (list[0] & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;

    if (LOCK_OLD_VERSION == version) {
        mNumArgs = nargs;
        state = LOCK_OLD_VERSION;
    } else if (LOCK_NEW_VERSION == version) {
        //in a new type of request we're getting odd number of args
        if (potentialOldRequest) {
            ret = false;
        }
        mNumArgs = nargs / 2;
        state = LOCK_NEW_VERSION;
    } else {
        //not supported version
        ret = false;
    }

    mVersion = version;

    if (mNumArgs > MAX_RESOURCES_PER_REQUEST) {
        //truncating/discarding few locks might not be a good idea as requestor
        //might be expecting all locks, returning error will give requestor a chance
        //to correct the requet
        QLOGE(LOG_TAG, "max resources per request exceeded");
        return false;
    }

    //initial sanity check before we create memory
    if (!ret) {
        return ret;
    }

    //create memory for resources
    mResources = new(std::nothrow) ResourceInfo[mNumArgs];
    if (NULL == mResources) {
        return false;
    }

    /** next level of validation (for all resources)         **
     ** all the below checks are performed per resource      **
     ** 1.mixed args checking (not allowed)                  **
     ** 2.major/minor out of bounds                          **
     ** 3.unsupported opcode                                 **
     ** 4.unsupported version                                */
    QLOGL(LOG_TAG, QLOG_L1, "mNumArgs=%" PRIu32, mNumArgs);
    for (i = 0; i < mNumArgs; i++) {
        if (LOCK_NEW_VERSION == state) {
            //new request processing
            opcode = list[i*2];
            version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;
            if (version != LOCK_NEW_VERSION) {
                //request conatins mixed old, new type resources
                ret = false;
                break;
            }
            QLOGL(LOG_TAG, QLOG_L2, "populate_resource_info: opcde 0x%" PRIx32, opcode);

            //parse new request
            mResources[i].ParseNewRequest(opcode);

            if (mResources[i].GetQIndex() == UNSUPPORTED_Q_INDEX) {
                //major/minor out of bounds or unsupported opcode was provided
                QLOGE(LOG_TAG, "Qindx %" PRIu16 " is  not supported", mResources[i].GetQIndex());
                ret = false;
                break;
            }

            uint32_t override_val = OverrideClientValue(
                                         mResources[i].GetMajor(),
                                         mResources[i].GetMinor(), list[i*2+1]);

            //backward compatibility for sched up/down migrate
            if (Target::getCurTarget().isUpDownCombSupported() &&
                 mResources[i].GetMajor() == SCHED_MAJOR_OPCODE) {
                uint16_t minor = mResources[i].GetMinor();
                if (minor == SCHED_UPMIGRATE_OPCODE) {
                    upmigrate_idx[(int)mResources[i].GetCluster()] = i;
                    migrate_flag = true;
                }
                if (minor == SCHED_DOWNMIGRATE_OPCODE) {
                    downmigrate_idx[(int)mResources[i].GetCluster()] = i;
                    migrate_flag = true;
                }
            }

            //backward compatibility for sched group up/down migrate
            if (Target::getCurTarget().isGrpUpDownCombSupported() &&
                 mResources[i].GetMajor() == SCHED_MAJOR_OPCODE) {
                uint16_t minor = mResources[i].GetMinor();
                if (minor == SCHED_GROUP_UPMIGRATE_OPCODE) {
                    grp_upmigrate_idx = i;
                    grp_migrate_flag = true;
                }
                if (minor == SCHED_GROUP_DOWNMIGRATE_OPCODE) {
                    grp_downmigrate_idx = i;
                    grp_migrate_flag = true;
                }
            }

            //store value which was in next arg
            mResources[i].SetValue(override_val);
            QLOGL(LOG_TAG, QLOG_L3, "value stored index=%" PRIu32, (i*2+1));

            //translate to physical only for core-dependent and non-display resources
            uint8_t nodeType = OptsData::getInstance().get_node_type(mResources[i].GetQIndex());
            int32_t qIndex = mResources[i].GetQIndex();
            bool needsPhysicalTranslation = !(nodeType == SINGLE_NODE || qIndex < MAX_DISPLAY_MINOR_OPCODE ||
                                              qIndex == DISPLAY_DOZE_OPCODE);
            if (needsPhysicalTranslation && !mResources[i].TranslateToPhysical()) {
                QLOGE(LOG_TAG, "Translate to physical failed");
                mResources[i].SetCluster(UNSUPPORTED);
                mResources[i].SetCore(UNSUPPORTED);
                continue;
            }

            //value mapping to physical
            if (!mResources[i].TranslateValueMapToPhysical()) {
                QLOGE(LOG_TAG, "value mapping to physical failed");
                ret = false;
                break;
            }
            QLOGL(LOG_TAG, QLOG_L3, "Translate to physical value succed ret=%" PRIu8, ret);
        }
        else if (LOCK_OLD_VERSION == state) {
            //old request processing
            uint32_t resource = 0;
            uint32_t old_opcode = list[i];
            opcode = list[i];
            QLOGL(LOG_TAG, QLOG_L2, "resource is old type");
            version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;

            if (version != LOCK_OLD_VERSION) {
                QLOGE(LOG_TAG, "request conatins mixed old, new type resource");
                //request conatins mixed old, new type resources
                ret = false;
                break;
            }

            if (opcode > 0xFFFF) {
                QLOGE(LOG_TAG, "opcode 0x%" PRIx32" size is gretaer than valid old opcode, not valid request", opcode);
                ret = false;
                break;
            }

            //backward compatibility
            //convert old opcode to new
            opcode = TranslateOpcodeToNew(opcode);

            //parse new opcode now
            mResources[i].ParseNewRequest(opcode);

            if (mResources[i].GetQIndex() == UNSUPPORTED_Q_INDEX) {
                QLOGE(LOG_TAG, "Unsupported q index %" PRIu16 " for resource 0x%" PRIx32,mResources[i].GetQIndex(), opcode);
                //major/minor out of bounds or unsupported opcode was provided
                ret = false;
                break;
            }

            //Physical translation layer not needed
            //The old opcodes donot use logical mapping, they are already physical
            //however correct physical convention need to be used
            if (!mResources[i].TranslateToPhysicalOld()) {
                ret = false;
                break;
            }

            //backward compatibility
            //convert old value to new, store value which was in next arg
            uint32_t val = TranslateValueToNew(mResources[i].GetMajor(), mResources[i].GetMinor(), old_opcode);

            QLOGL(LOG_TAG, QLOG_L2, "Value for old resource returned as 0x%" PRIx32" %" PRIu32, val, val);
            mResources[i].SetValue(val);
        }
        else {
            //unsupported version args
            ret = false;
            break;
        }

        //check for display off/doze case
        if (mResources[i].GetQIndex() == DISPLAY_OFF_INDEX ||
                mResources[i].GetQIndex() == DISPLAY_DOZE_INDEX) {
            //display off/doze request gets priority
            QLOGL(LOG_TAG, QLOG_L2, "setting mPriority to HIGH_PRIORITY for display/display doze");
            mPriority = HIGH_PRIORITY;
        }
        if (mResources[i].GetQIndex() == ALWAYS_ALLOW_INDEX) {
            QLOGL(LOG_TAG, QLOG_L2, "Recieved always allow request");
            mPriority = ALWAYS_ALLOW;
        }
        QLOGL(LOG_TAG, QLOG_L3, "ret=%" PRIu8,ret);
    } /*for (i = 0; i < nargs; i++)*/

    if (migrate_flag)
        ret = TranslateUpDownMigrate(upmigrate_idx, downmigrate_idx);
    if (grp_migrate_flag)
        ret = TranslateGrpUpDownMigrate(grp_upmigrate_idx, grp_downmigrate_idx);

    QLOGL(LOG_TAG, QLOG_L3, "ret=%" PRIu8 " outside for loop", ret);

    if (!ret) {
        //invalid request, let us clean up and bail out
        QLOGE(LOG_TAG, "invalid request");
        delete[] mResources;
        mResources = NULL;
    }

    return ret;
}


/****************************************************************************
 **Backward compatibility layer                                            **
 ***************************************************************************/
#define UNSUPPORTED_OPCODE 0x42C00000
#define OLD_RESOURCE_COUNT 85

static uint32_t old_opcde_to_new_opcde[OLD_RESOURCE_COUNT] = {
    0x40000000, /*DISPLAY*/
    0x40400000, /*POWER_COLLAPSE,*/
    0x40800000, /*CPU0_MIN_FREQ */
    0x40800010, /*CPU1_MIN_FREQ*/
    0x40800020, /*CPU2_MIN_FREQ*/
    0x40800030, /*CPU3_MIN_FREQ*/
    UNSUPPORTED_OPCODE, /*UNSUPPORTED_0,*/
    0x41000000, /*CLUSTR_0_CPUS_ON,*/
    0x41004000,    /*CLUSTR_0_MAX_CORES, */
    UNSUPPORTED_OPCODE, /*UNSUPPORTED_2,*/
    UNSUPPORTED_OPCODE, /* UNSUPPORTED_3,*/
    0x42400000, /*SAMPLING_RATE, */
    0x42404000, /*ONDEMAND_IO_IS_BUSY,*/
    0x42408000, /*ONDEMAND_SAMPLING_DOWN_FACTOR,*/
    0x41424000, /*INTERACTIVE_TIMER_RATE,*/
    0x41414000, /*INTERACTIVE_HISPEED_FREQ,*/
    0x41410000, /*INTERACTIVE_HISPEED_LOAD,*/
    0x4240C000, /*SYNC_FREQ,*/
    0x42410000, /*OPTIMAL_FREQ,*/
    UNSUPPORTED_OPCODE,/*SCREEN_PWR_CLPS,*/ //TODO
    0x4241C000, /*THREAD_MIGRATION, */
    0x40804000, /*CPU0_MAX_FREQ,*/
    0x40804010, /*CPU1_MAX_FREQ, */
    0x40804020, /*CPU2_MAX_FREQ,*/
    0x40804030, /*CPU3_MAX_FREQ,*/
    0x42414000, /*ONDEMAND_ENABLE_STEP_UP,*/
    0x42418000, /*ONDEMAND_MAX_INTERMEDIATE_STEPS, */
    0x41418000, /*INTERACTIVE_IO_BUSY,*/
    0x42000000, /*KSM_RUN_STATUS,*/
    0x42004000, /*KSM_PARAMS,*/
    0x40C00000, /*SCHED_BOOST,*/
    0x40800100, /*CPU4_MIN_FREQ,*/
    0x40800110, /*CPU5_MIN_FREQ,*/
    0x40800120, /*CPU6_MIN_FREQ,*/
    0x40800130, /*CPU7_MIN_FREQ,*/
    0x40804100, /*CPU4_MAX_FREQ,*/
    0x40804110, /*CPU5_MAX_FREQ,*/
    0x40804120, /*CPU6_MAX_FREQ,*/
    0x40804130, /*CPU7_MAX_FREQ,*/
    0x41400000, /*CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY,*/
    0x41404000, /*CPU0_INTERACTIVE_BOOST,*/
    0x41408000, /*CPU0_INTERACTIVE_BOOSTPULSE,*/
    0x4140C000, /*CPU0_INTERACTIVE_BOOSTPULSE_DURATION,*/
    0x41410000, /*CPU0_INTERACTIVE_GO_HISPEED_LOAD,*/
    0x41414000, /*CPU0_INTERACTIVE_HISPEED_FREQ,*/
    0x41418000, /*CPU0_INTERACTIVE_IO_IS_BUSY,*/
    0x4141C000, /*CPU0_INTERACTIVE_MIN_SAMPLE_TIME,*/
    0x41420000, /*CPU0_INTERACTIVE_TARGET_LOADS,*/
    0x41424000, /*CPU0_INTERACTIVE_TIMER_RATE,*/
    0x41428000, /*CPU0_INTERACTIVE_TIMER_SLACK,*/
    0x41400100, /*CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY,*/
    0x41404100, /*CPU4_INTERACTIVE_BOOST,*/
    0x41408100, /*CPU4_INTERACTIVE_BOOSTPULSE,*/
    0x4140C100, /*CPU4_INTERACTIVE_BOOSTPULSE_DURATION,*/
    0x41410100, /*CPU4_INTERACTIVE_GO_HISPEED_LOAD,*/
    0x41414100, /*CPU4_INTERACTIVE_HISPEED_FREQ,*/
    0x41418100, /*CPU4_INTERACTIVE_IO_IS_BUSY,*/
    0x4141C100, /*CPU4_INTERACTIVE_MIN_SAMPLE_TIME,*/
    0x41420100, /*CPU4_INTERACTIVE_TARGET_LOADS,*/
    0x41424100, /*CPU4_INTERACTIVE_TIMER_RATE,*/
    0x41428100, /*CPU4_INTERACTIVE_TIMER_SLACK,*/
    0x41004100, /*CLUSTR_1_MAX_CORES,*/
    0x40C04000, /*SCHED_PREFER_IDLE,*/
    0x40C08000, /*SCHED_MIGRATE_COST,*/
    0x40C0C000, /*SCHED_SMALL_TASK,*/
    0x40C10000, /*SCHED_MOSTLY_IDLE_LOAD,*/
    0x40C14000, /*SCHED_MOSTLY_IDLE_NR_RUN,*/
    0x40C18000, /*SCHED_INIT_TASK_LOAD,*/
    0x41C04000, /*VIDEO_DECODE_PLAYBACK_HINT,*/
    0x41C08000, /*DISPLAY_LAYER_HINT,*/
    0x41C00000, /*VIDEO_ENCODE_PLAYBACK_HINT,*/
    0x41800000, /*CPUBW_HWMON_MIN_FREQ,*/
    0x41804000, /*CPUBW_HWMON_DECAY_RATE,*/
    0x41808000, /*CPUBW_HWMON_IO_PERCENT,*/
    0x4142C000, /*CPU0_INTERACTIVE_MAX_FREQ_HYSTERESIS,*/
    0x4142C100, /*CPU4_INTERACTIVE_MAX_FREQ_HYSTERESIS,*/
    0x42800000, /*GPU_DEFAULT_PWRLVL,*/
    0x41000100, /*CLUSTR_1_CPUS_ON,*/
    0x40C1C000, /*SCHED_UPMIGRATE,*/
    0x40C20000, /*SCHED_DOWNMIGRATE*/
    0x40C24000, /*SCHED_MOSTLY_IDLE_FREQ*/
    0x42C04000, /*IRQ_BALANCER*/
    0x41430000, /*CPU0_INTERACTIVE_USE_SCED_LOAD,*/
    0x41434000, /*CPU0_INTERACTIVE_USE_MIGRATION_NOTIF,*/
    0x42C08000  /*INPUT_BOSST_RESET*/
};

/* Map old opcode to new opcode.
 * Also populate various elements of the ResourceInfo
 * data structure.
 * */
uint32_t Request::TranslateOpcodeToNew(uint32_t opcode) {
    QLOGV(LOG_TAG, "Map old to new opcde 0x%" PRIx32, opcode);
    opcode = opcode & 0xffff;
    uint32_t tmp = (opcode & 0xff00)>>8 ;

    if (tmp < OLD_RESOURCE_COUNT) {
        QLOGL(LOG_TAG, QLOG_L2, "Mapping old resource to new, indexing with 0x%" PRIx32, tmp);
        opcode = old_opcde_to_new_opcde[tmp];
    }
    else {
        QLOGE(LOG_TAG, "Unsupported old resource id 0x%" PRIx32,opcode);
        opcode = UNSUPPORTED_OPCODE;
    }

    return opcode;
}

/* This function converts old version values to new
 * version values.
 * */
uint32_t Request::TranslateValueToNew(uint16_t major, uint16_t minor, uint32_t opcode) {
    //this is an old opcode which has opcode+value, get the value now
    uint32_t value = (opcode & 0xff);
    uint32_t resource = (opcode & 0xff00) >> 8;

    //Reset value passed with old opcode, change it to MAX_LVL
    if ((resource == ONDEMAND_IO_IS_BUSY) || (resource == THREAD_MIGRATION) ||
        (resource >= ONDEMAND_ENABLE_STEP_UP && resource <= INTERACTIVE_IO_BUSY) ||
        (resource == CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY) ||
        (resource == CPU0_INTERACTIVE_BOOST) ||
        (resource >= CPU0_INTERACTIVE_IO_IS_BUSY &&
        resource <= CPU0_INTERACTIVE_TARGET_LOADS) ||
        (resource == CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY) ||
        (resource == CPU4_INTERACTIVE_BOOST) ||
        (resource >= CPU4_INTERACTIVE_IO_IS_BUSY &&
        resource <= CPU4_INTERACTIVE_TARGET_LOADS) ||
        resource == SCHED_BOOST) {
        if (value == 0xFF) {
            value = MAX_LVL;
            return value;
        }
    } else {
        if (value == 0) {
            value = MAX_LVL;
            return value;
        }
    }

    if (major == CPUFREQ_MAJOR_OPCODE) {
        value = value * CONVERT_IN_MHZ;
    } else if (major == ONDEMAND_MAJOR_OPCODE) {
        switch (minor) {
            case OND_OPIMAL_FREQ_OPCODE:
            case OND_SYNC_FREQ_OPCODE:
                value = value * CONVERT_IN_MHZ;
                break;
            case OND_SAMPLING_RATE_OPCODE:
                value = 0xFF - (value & 0xFF);
                value = value * CONVERT_IN_MSEC;
                break;
        }
    } else if (major == CORE_HOTPLUG_MAJOR_OPCODE) {
        if (minor == CORE_HOTPLUG_MAX_CORE_ONLINE_OPCODE) {
            value = 0xF - (value & 0xF);
        }
    } else if (major == INTERACTIVE_MAJOR_OPCODE) {
        switch (minor) {
            case INTERACTIVE_HISPEED_FREQ_OPCODE:
                value = value * CONVERT_IN_MHZ;
                break;
            case INTERACTIVE_MIN_SAMPLE_TIME_OPCODE:
            case INTERACTIVE_BOOSTPULSE_DURATION_OPCODE:
                value = value * CONVERT_IN_MSEC;
                break;
            case INTERACTIVE_TIMER_RATE_OPCODE:
            case INTERACTIVE_TIMER_SLACK_OPCODE:
                value = 0xFF - (value & 0xFF);
                value = value * CONVERT_IN_MSEC;
                break;
        }
    } else if (major == SCHED_MAJOR_OPCODE) {
        if (minor == SCHED_MOSTLY_IDLE_FREQ_OPCODE){
            value = value * CONVERT_IN_MHZ;
        }
    }

    return value;
}

void Request::RequestTimerCallback(sigval_t pvData) {
    if (pvData.sival_int != 0) {
        perf_lock_rel(pvData.sival_int);
    }
}

int32_t Request::CreateTimer(int32_t req_handle) {
    int32_t rc = 0;
    struct sigevent sigEvent;

    try {
        std::scoped_lock lck(mTimerCreatedLock);
        sigEvent.sigev_notify = SIGEV_THREAD;
        sigEvent.sigev_notify_function = &RequestTimerCallback;
        sigEvent.sigev_notify_attributes = NULL;
        sigEvent.sigev_value.sival_int = req_handle;

        rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &mTimer);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to create timer");
            return rc;
        }
        mTimerCreated = true;
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }

    return rc;
}

int32_t Request::SetTimer() {
    int32_t rc = FAILED;
    uint32_t uTime = 0;
    struct itimerspec mTimeSpec;

    uTime = GetDuration();

    mTimeSpec.it_value.tv_sec = uTime / TIME_MSEC_IN_SEC;
    mTimeSpec.it_value.tv_nsec = (uTime % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;

    //if timer created then arm it otherwise don't
    try {
        std::scoped_lock lck(mTimerCreatedLock);
        if (mTimerCreated) {
            rc = timer_settime(mTimer, 0, &mTimeSpec, NULL);
            if (rc != 0) {
                rc = FAILED;
                QLOGE(LOG_TAG, "Failed to set timer");
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }

    return rc;
}

int32_t Request::DeleteTimer() {
    int32_t rc = FAILED;

    try {
        std::scoped_lock lck(mTimerCreatedLock);
        if (mTimerCreated) {
            rc = timer_delete(mTimer);
            if (rc != 0) {
                QLOGE(LOG_TAG, "Failed to delete timer for a request");
            }
            mTimerCreated = false;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}
