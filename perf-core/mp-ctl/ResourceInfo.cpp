/******************************************************************************
  @file    ResourceInfo.cpp
  @brief   Implementation of performance resources

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-RESOURCE"
#include "ResourceInfo.h"

#include <string.h>
#include <stdio.h>
#include "Target.h"
#include "MpctlUtils.h"
#include "PerfLog.h"

ResourceInfo::ResourceInfo() {
    memset(&mResource, 0 , sizeof(Resource));
    mResource.major = MISC_MAJOR_OPCODE;
    mResource.minor = UNSUPPORTED_OPCODE;
    mResource.mapping = 0;
    mResource.cluster = -1;
    mResource.core = -1;
    mResource.qindex = UNSUPPORTED_Q_INDEX;
    mResource.opcode = 0;
    mResource.version = 1;
    mResource.value = -1;
}

ResourceInfo::ResourceInfo(ResourceInfo const& resObj) {
    mResource = resObj.mResource;
}

ResourceInfo& ResourceInfo::operator=(ResourceInfo const& resObj) {
    mResource = resObj.mResource;

    return *this;
}

bool ResourceInfo::operator==(const ResourceInfo& resObj) {
  return ((mResource.major == resObj.mResource.major) &&
          (mResource.minor == resObj.mResource.minor) &&
          (mResource.mapping == resObj.mResource.mapping) &&
          (mResource.cluster == resObj.mResource.cluster) &&
          (mResource.value == resObj.mResource.value) &&
          (mResource.core == resObj.mResource.core));
}

bool ResourceInfo::operator!=(const ResourceInfo& resObj) {
  return !(*this == resObj);
}

uint8_t minorIdxMaxSize[MAX_MAJOR_RESOURCES] = {
    MAX_DISPLAY_MINOR_OPCODE,
    MAX_PC_MINOR_OPCODE,
    MAX_CPUFREQ_MINOR_OPCODE,
    MAX_SCHED_MINOR_OPCODE,
    MAX_CORE_HOTPLUG_MINOR_OPCODE,
    MAX_INTERACTIVE_MINOR_OPCODE,
    MAX_CPUBW_HWMON_MINOR_OPCODE,
    MAX_VIDEO_MINOR_OPCODE,
    MAX_KSM_MINOR_OPCODE,
    MAX_OND_MINOR_OPCODE,
    MAX_GPU_MINOR_OPCODE,
    MAX_MISC_MINOR_OPCODE,
    MAX_LLCBW_HWMON_MINOR_OPCODE,
    MAX_MEMLAT_MINOR_OPCODE,
    MAX_NPU_MINOR_OPCODE,
    MAX_SCHED_EXT_MINOR_OPCODE
};

uint16_t ResourceInfo::DiscoverQueueIndex() {
    uint16_t idx = 0;
    QLOGV(LOG_TAG, "QIndex being sought for major %" PRIu16 "minor %" PRIu16,
          mResource.major, mResource.minor);

    //make sure we get the unsupported q index for all invalid major/minor types
    if (mResource.major >= MAX_MAJOR_RESOURCES) {
        mResource.qindex = UNSUPPORTED_Q_INDEX;
        return mResource.qindex;
    }

    switch(mResource.major) {
        case NPU_MAJOR_OPCODE:
            idx = QINDEX_NPU_MAJOR_OPCODE;
            break;
        case MEMLAT_MAJOR_OPCODE:
            idx = QINDEX_MEMLAT_MAJOR_OPCODE;
            break;
        case LLCBW_HWMON_MAJOR_OPCODE:
            idx = QINDEX_LLCBW_HWMON_MAJOR_OPCODE;
            break;
        case MISC_MAJOR_OPCODE:
            idx = QINDEX_MISC_MAJOR_OPCODE;
            break;
        case GPU_MAJOR_OPCODE:
            idx = QINDEX_GPU_MAJOR_OPCODE;
            break;
        case ONDEMAND_MAJOR_OPCODE:
            idx = QINDEX_ONDEMAND_MAJOR_OPCODE;
            break;
        case KSM_MAJOR_OPCODE:
            idx = QINDEX_KSM_MAJOR_OPCODE;
            break;
        case VIDEO_MAJOR_OPCODE:
            idx = QINDEX_VIDEO_MAJOR_OPCODE;
            break;
        case CPUBW_HWMON_MAJOR_OPCODE:
            idx = QINDEX_CPUBW_HWMON_MAJOR_OPCODE;
            break;
        case INTERACTIVE_MAJOR_OPCODE:
            idx = QINDEX_INTERACTIVE_MAJOR_OPCODE;
            break;
        case CORE_HOTPLUG_MAJOR_OPCODE:
            idx = QINDEX_CORE_HOTPLUG_MAJOR_OPCODE;
            break;
        case SCHED_MAJOR_OPCODE:
            idx = QINDEX_SCHED_MAJOR_OPCODE;
            break;
        case CPUFREQ_MAJOR_OPCODE:
            idx = QINDEX_CPUFREQ_MAJOR_OPCODE;
            break;
        case POWER_COLLAPSE_MAJOR_OPCODE:
            idx = QINDEX_POWER_COLLAPSE_MAJOR_OPCODE;
            break;
        case DISPLAY_OFF_MAJOR_OPCODE:
            idx = QINDEX_DISPLAY_OFF_MAJOR_OPCODE;
            break;
       case SCHED_EXT_MAJOR_OPCODE:
            idx = QINDEX_SCHED_EXT_MAJOR_OPCODE;
            break;
    }

    mResource.qindex = idx + mResource.minor;

    if ((mResource.minor >= minorIdxMaxSize[mResource.major]) ||
        (mResource.qindex >= MAX_MINOR_RESOURCES)) {
        mResource.qindex = UNSUPPORTED_Q_INDEX;
    }

    return mResource.qindex;
}

void ResourceInfo::Dump() {
    QLOGV(LOG_TAG, "Resource version=%" PRIu8 ", major=%" PRIu16 ", minor=%" PRIu16 ", map=%" PRIu32,
           mResource.version, mResource.major, mResource.minor, mResource.mapping);
    QLOGV(LOG_TAG, "resource cluster=%" PRId8 ", core=%" PRId8 ", value=%" PRIu32 ", queue-index=%" PRIu16,
          mResource.cluster, mResource.core, mResource.value, mResource.qindex);
}

int8_t ResourceInfo::GetMinorIndex(uint8_t major) {
    if (major >= 0 && major < MAX_MAJOR_RESOURCES) {
       return (int8_t)minorIdxMaxSize[major];
    }
    else {
       QLOGE(LOG_TAG, "Major type 0x%" PRIx8" not supported", major);
    }
    return -1;
}

void ResourceInfo::ParseNewRequest(uint32_t opcode) {
    mResource.opcode = opcode;
    mResource.version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;;
    mResource.mapping = (opcode & EXTRACT_MAP_TYPE) >> SHIFT_BIT_MAP;
    mResource.major = (opcode & EXTRACT_MAJOR_TYPE) >> SHIFT_BIT_MAJOR_TYPE;
    mResource.minor = (opcode & EXTRACT_MINOR_TYPE) >> SHIFT_BIT_MINOR_TYPE;
    mResource.cluster = (opcode & EXTRACT_CLUSTER) >> SHIFT_BIT_CLUSTER;
    mResource.core = (opcode & EXTRACT_CORE) >> SHIFT_BIT_CORE;
    DiscoverQueueIndex();
    QLOGL(LOG_TAG, QLOG_L2, "Resource bitmap %" PRIu32 ", qindex %" PRIu16 ", cluster %" PRId8 ", core %" PRId8 ", major %" PRIx16", minor %" PRIx16" map %" PRIu32, opcode,
          mResource.qindex, mResource.cluster, mResource.core, mResource.major, mResource.minor, mResource.mapping);
    return;
}

/****************************************************************************
 **Physical transaltion layer                                              **
 ***************************************************************************/
bool ResourceInfo::TranslateToPhysical() {
    bool ret = true;
    int8_t tmp = 0;

    //logical cluster/core now translated to physical core/cluster based on target
    tmp = Target::getCurTarget().getPhysicalCluster(mResource.cluster);
    QLOGL(LOG_TAG, QLOG_L2, "physical cluster %" PRId8,tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.cluster = tmp;
    }

    tmp = Target::getCurTarget().getPhysicalCore(mResource.cluster, mResource.core);
    QLOGL(LOG_TAG, QLOG_L2, "physical core %" PRId8,tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.core = tmp;
    }

    QLOGV(LOG_TAG, "returned %" PRIu8, ret);
    return ret;
}

bool ResourceInfo::TranslateToPhysicalOld() {
    bool ret = true;
    int8_t tmp = 0;
    QLOGL(LOG_TAG, QLOG_L2, "Physical cluster is %" PRId8, mResource.cluster);

    /* As some of the resources are core independent
     * we map them to the perf cluster.
     * */
    if ((mResource.major != CPUFREQ_MAJOR_OPCODE) &&
        (mResource.major != INTERACTIVE_MAJOR_OPCODE) &&
        (mResource.major != CORE_HOTPLUG_MAJOR_OPCODE)) {
        tmp = Target::getCurTarget().getPhysicalCluster(mResource.cluster);
        QLOGL(LOG_TAG, QLOG_L2, "physical cluster %" PRId8,tmp);
        if (tmp < 0) {
            ret = false;
        } else {
            mResource.cluster = tmp;
       }
    }

    tmp = Target::getCurTarget().getPhysicalCore(mResource.cluster, mResource.core);
    QLOGL(LOG_TAG, QLOG_L2, "physical core %" PRId8, tmp);
    if (tmp < 0) {
        ret = false;
    } else {
        mResource.core = tmp;
    }
    QLOGV(LOG_TAG, "returned %" PRIu8, ret);
    return ret;
}

bool ResourceInfo::TranslateValueMapToPhysical() {
    bool ret = true;
    int32_t val = 0;

    if (mResource.mapping == VALUE_MAPPED) {
        val = Target::getCurTarget().getMappedValue(mResource.qindex, mResource.value);
        QLOGL(LOG_TAG, QLOG_L2, "Mapped Value is 0x%" PRIx32, val);
        if (val != FAILED) {
            mResource.value = val;
        } else {
            QLOGE(LOG_TAG, "Value could not be mapped for the resource 0x%" PRIx16, mResource.qindex);
            ret = false;
        }
    }

    return ret;
}



