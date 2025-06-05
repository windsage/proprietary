/******************************************************************************
  @file    Target.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>
#include "Request.h"
#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"
#include "XmlParser.h"
#include "ResourceInfo.h"
#include "PerfLog.h"
#include <algorithm>

Target Target::cur_target;

#define PMQOSWFI_LATENCY_NODE "/sys/devices/system/cpu/cpu%" PRIu8 "/cpuidle/state0/latency"

/*
 * Definition for various get functions
 * */
int8_t Target::getPhysicalCluster(int8_t logicalCluster) {
    int8_t cluster = -1;

    if (!mTargetConfig.getIsDefaultDivergent()) {
        std::string clusterName = mLogicalClusterIdToNameMap.at(logicalCluster);
        return mTargetConfig.getPhysicalClusterId(clusterName);
    }

    // If logicalCluster lies  within the range of the
    // physical clusters, then return the cluster from
    // the physicalCLusterMap.
    if (logicalCluster < mTargetConfig.getNumCluster()) {
        if (mPhysicalClusterMap) {
            cluster = mPhysicalClusterMap[logicalCluster];
        }
    } else if ((logicalCluster >= START_PRE_DEFINE_CLUSTER) &&
               (logicalCluster < MAX_PRE_DEFINE_CLUSTER)) {
        //logical cluster lies in pre-defined cluster range
        if (mPredefineClusterMap) {
            cluster = mPredefineClusterMap[logicalCluster - START_PRE_DEFINE_CLUSTER];
        }
    } else {
        QLOGE(LOG_TAG, "Error: Invalid logical cluster id %" PRId8, logicalCluster);
    }

    if (cluster > mTargetConfig.getNumCluster()) {
        cluster = -1;
    }
    return cluster;
}

bool Target::isLittleCluster(uint8_t cluster_id) {
    return !getClusterName(cluster_id).compare("little");
}

int8_t Target::getFirstCoreOfPerfCluster() {
    int8_t perfCluster = -1;
    int8_t supportedCore = -1;

    if (!mTargetConfig.getIsDefaultDivergent()) {
        for (uint8_t i = 0; i < mTargetConfig.getNumCluster(); i++) {
            // The non-little clusters are perf clusters
            if (!isLittleCluster(i) && mTargetConfig.isClusterSupported(i)) {
                return getFirstCoreIndex(i);
            }
        }
        return supportedCore;
    }

    if ((NULL == mPhysicalClusterMap) || (NULL == mLastCoreIndex)) {
        return supportedCore;
    }

    perfCluster = mPhysicalClusterMap[0];
    if ((perfCluster >= 0) && (perfCluster < mTargetConfig.getNumCluster())) {
        if (perfCluster == 0) {
            supportedCore = 0;
        } else {
            supportedCore = mLastCoreIndex[perfCluster-1] + 1;
        }
    }
    return supportedCore;
}

int8_t Target::getFirstCoreOfClusterWithSpilloverRT() {
    int8_t spilloverRTCluster = -1;
    int8_t supportedCore = -1;
    int8_t numSilvers = -1;
    if ((NULL == mPhysicalClusterMap) || (NULL == mLastCoreIndex)) {
        return supportedCore;
    }
    numSilvers = getNumLittleCores();
    if(numSilvers > 2 || numSilvers < 0) {
        QLOGE(LOG_TAG, "numSilvers:%d, Perfd will not sched RT spillover beyond silver", numSilvers);
        return supportedCore;
    }
    //For CPU topology with <= 2 silvers, use deterministic Gold cluster
    //for display heavy RT (SurfaceFlinger, RenderEngine)
    if(mTargetConfig.getNumCluster() == 4) {
        spilloverRTCluster = mPhysicalClusterMap[3];
        if ((spilloverRTCluster >= 0) && (spilloverRTCluster < mTargetConfig.getNumCluster())) {
            if (spilloverRTCluster == 0) {
                supportedCore = 0;
            } else {
                supportedCore = mLastCoreIndex[spilloverRTCluster-1] + 1;
            }
        }
    }
    //If in future, there is a new CPU topology with 3 clusters with <= 2 silvers,
    //the designated Gold cluster for placing SF/RE i.e. spilloverRTCluster
    //could be different
    return supportedCore;
}

int8_t Target::getNumLittleCores() {
    int8_t littleClusterIndex = -1;
    int8_t numSilvers = -1;
    //For targets with < 3 clusters (say Pakala with 2 clusters & no silvers), return FAILED
    //For future targets with >=3 clusters and no silvers, this function will be revisited
    if(mTargetConfig.getNumCluster() < 3) {
        QLOGL(LOG_TAG, QLOG_L1, "Number of clusters < 3, return FAILED");
        return FAILED;
    }
    littleClusterIndex = mPhysicalClusterMap[1];
    if(littleClusterIndex < 0 || littleClusterIndex >= mTargetConfig.getNumCluster())
        return FAILED;
    numSilvers = getLastCoreIndex(littleClusterIndex) - getFirstCoreIndex(littleClusterIndex) + 1;
    return numSilvers;
}

int8_t Target::getPhysicalCore(int8_t cluster, int8_t logicalCore) {
    int8_t physicalCore = -1;

    if (!mTargetConfig.getIsDefaultDivergent()) {
        if (cluster < 0)
            return physicalCore;

        if (logicalCore > mTargetConfig.getCoresInCluster(cluster) - 1) {
            return physicalCore;
        }

        if (mTargetConfig.isClusterSupported(cluster)) {
            return getFirstCoreIndex(cluster) + logicalCore;
        }
        return physicalCore;
    }

    if(cluster < 0 || cluster >= mTargetConfig.getNumCluster())
        return physicalCore;

    if (cluster == 0) {
        physicalCore = logicalCore;
    } else {
        if (NULL != mLastCoreIndex) {
            physicalCore = logicalCore + mLastCoreIndex[cluster-1] + 1;
        }
    }
    if ((NULL != mLastCoreIndex) && (physicalCore > mLastCoreIndex[cluster])) {
        physicalCore = -1;
    }

    return physicalCore;
}

int8_t Target::getLastCoreIndex(int8_t cluster) {
    if (mLastCoreIndex) {
        if (cluster >=0 && cluster < mTargetConfig.getNumCluster()) {
            return mLastCoreIndex[cluster];
        } else {
            QLOGE(LOG_TAG, "Error: Invalid cluster id %" PRId8, cluster);
        }
    } else {
        QLOGE(LOG_TAG, "Eror: Target not initialized");
    }
    return -1;
}

int8_t Target::getFirstCoreIndex(int8_t cluster) {
    if (mLastCoreIndex) {
        if (cluster == 0) {
            return 0;
        } else if (cluster >=1 && cluster < mTargetConfig.getNumCluster()) {
            return mLastCoreIndex[cluster-1] + 1;
        } else {
            QLOGE(LOG_TAG, "Error: Invalid cluster id %" PRId8, cluster);
        }
    } else {
        QLOGE(LOG_TAG, "Eror: Target not initialized");
    }
    return -1;
}

Target::Target() {
    QLOGV(LOG_TAG, "Target constructor");
    mPhysicalClusterMap = NULL;
    mPredefineClusterMap = NULL;
    mLastCoreIndex = NULL;
    memset(mResourceSupported, 0x00, sizeof(mResourceSupported));
    mPmQosWfi = 0;
    memset(mStorageNode, '\0', sizeof(mStorageNode));
    mTotalGpuFreq = 0;
    mTotalGpuBusFreq = 0;
    mTraceFlag = false;
}

Target::~Target() {
    QLOGV(LOG_TAG, "Target destructor");
    if (mPhysicalClusterMap){
        delete [] mPhysicalClusterMap;
        mPhysicalClusterMap = NULL;
    }

    if (mPredefineClusterMap){
        mPredefineClusterMap = NULL;
    }

    if (mLastCoreIndex) {
        delete [] mLastCoreIndex;
        mLastCoreIndex = NULL;
    }
}

/* Create a logical to physical cluster
 * mapping, in which Logical cluster "0"
 * maps to physical cluster "0". Useful
 * for the targets in which cluster0 is
 * the perf cluster.
 * */
void Target::mLogicalPerfMapPerfCluster() {
    int8_t i;
    int8_t num_clusters = mTargetConfig.getNumCluster();
    mPhysicalClusterMap = new(std::nothrow) int8_t[num_clusters];
    if(mPhysicalClusterMap) {
        for(i=0; i < num_clusters; i++) {
            mPhysicalClusterMap[i] = i ;
        }
   } else {
     QLOGE(LOG_TAG, "Error: Could not map Logical perf cluster to perf\n");
   }
}

/* Create a logical to physical cluster mapping
 * in which logical cluster "0" maps to power
 * cluster
 * */
void Target::mLogicalPerfMapPowerCluster() {
    int8_t num_clusters = mTargetConfig.getNumCluster();
    int8_t i, cluster = (num_clusters > MIN_CLUSTERS) ? MIN_CLUSTERS - 1 : num_clusters-1;
    mPhysicalClusterMap = new(std::nothrow) int8_t[num_clusters];

    if(mPhysicalClusterMap) {
        //This will retain the Big/little mapping similar to 2-cluster
        for(i=0; i < MIN_CLUSTERS && cluster >= 0; i++,cluster--) {
            mPhysicalClusterMap[i] = cluster;
        }

        // mType2 == 1
        if(mTargetConfig.getType2() == 1) {
            cluster = MIN_CLUSTERS + 1;
            for(i=MIN_CLUSTERS; i < MIN_CLUSTERS + 2 && cluster >= MIN_CLUSTERS; i++,cluster--) {
                mPhysicalClusterMap[i] = cluster;
            }
        }

        //This will keep 1:1 mapping for cluster after 2.
        cluster = (mTargetConfig.getType2() == 1) ? MIN_CLUSTERS + 2 : MIN_CLUSTERS;
        for(i=cluster; i < num_clusters; i++) {
            mPhysicalClusterMap[i] = i ;
        }
    } else {
        QLOGE(LOG_TAG, "Error: Could not map logical perf to power cluster\n");
    }
}

/* Calculate the number/index for last
 * core in this cluster
 * */
void Target::mCalculateCoreIdx() {
    int8_t cumlative_core, i;
    mLastCoreIndex = new(std::nothrow) int8_t[mTargetConfig.getNumCluster()];
    if(mLastCoreIndex && mTargetConfig.getCoresInCluster(0)) {
        cumlative_core = 0;
        for(i =0;i < mTargetConfig.getNumCluster(); i++) {
            int8_t coreInCluster = mTargetConfig.getCoresInCluster(i);
            if (coreInCluster == 0) {
                mLastCoreIndex[i] = (i == 0) ? 0 : mLastCoreIndex[i - 1];
                continue;
            }
            mLastCoreIndex[i] = cumlative_core + (coreInCluster - 1);
            cumlative_core = mLastCoreIndex[i] + 1;
        }
    } else {
        QLOGE(LOG_TAG, "Error: Initializing core index failed\n");
    }
}

/* Resource support functions. */

/* Return the bitmap value for the asked resource.
 * If Resource is supported it will return 1,
 * else 0.
 * */
bool Target::isResourceSupported(uint16_t major, uint16_t minor) {
    uint64_t tmp = 0;
    bool ret = false;

    if (major < MAX_MAJOR_RESOURCES) {
        tmp = mResourceSupported[major];
        ret = ((tmp >> minor) & 0x1);
    }

    return ret;
}

/* This function sets all the minor resources for a
 * major resource to be supported.
 * */
void Target::mSetAllResourceSupported() {
    uint64_t minorIdx,rshIdx;
    for (uint8_t i = 0; i < MAX_MAJOR_RESOURCES; i++) {
        minorIdx = ResourceInfo::GetMinorIndex(i);
        rshIdx = sizeof(uint64_t)*8 - minorIdx;
        mResourceSupported[i] = ((uint64_t)~0) >> rshIdx;
    }
}

/* This function resets the minor rsource in the specified
 * major resource to false (not supported).
 * */
void Target::mResetResourceSupported(uint16_t major, uint16_t minor) {
    unsigned long tmp;
    if (major < MAX_MAJOR_RESOURCES) {
        if (minor < ResourceInfo::GetMinorIndex(major)) {
            mResourceSupported[major] &= ~(1 << minor);
        }
    } /*if (major < MAX_MAJOR_RESOURCES)*/
}

/** This function returns the cluster number to which a
 * given cpu belongs. It also updates input parameters
 * with first cpu and last cpu in that cluster.
 * On error the return value is -1
 */
int8_t Target::getClusterForCpu(int8_t cpu, int8_t &startcpu, int8_t &endcpu) {
    int8_t i;
    int8_t cluster = -1;
    bool success = false;

    startcpu = 0;
    endcpu = -1;

    if ((cpu < 0) || (cpu > mTargetConfig.getTotalNumCores()) || (NULL == mLastCoreIndex)) {
        QLOGE(LOG_TAG, "Invalid cpu number %" PRId8 ". Cannot find cluster.", cpu);
        return -1;
    }

    for (i = 0 ; i < mTargetConfig.getNumCluster() && !success; i++) {
        endcpu = mLastCoreIndex[i];
        if (cpu >= startcpu && cpu <= endcpu) {
            cluster = i;
            success = true;
        }
        else {
            startcpu = mLastCoreIndex[i] + 1;
        }
    }

    if (!success){
        cluster = -1;
        startcpu = -1;
        endcpu = -1;
    }

    return cluster;
}


/* Functions to support value maps. */

/* This function initializes the valueMap for all
 * the resources. The default is "false" and
 * no map is specified.
 *
 * Whichever resource needs a map and has to be
 * specified should be done in specifc target
 * functions.
 * */
void Target::mInitAllResourceValueMap() {
    uint16_t i = 0;
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        mValueMap[i].mapSize = -1;
        mValueMap[i].map = NULL;
    }
}

void Target::readPmQosWfiValue() {
    int32_t fd = -1;
    int8_t i, n;
    char tmp[NODE_MAX];
    uint16_t latency = 0, max_latency = 0;
    const uint8_t MAX_BUF_SIZE = 16;
    char buf[MAX_BUF_SIZE];

    //read the PmQosWfi latency for all cpus to find the max value.
    for(i = 0; i < mTargetConfig.getTotalNumCores(); i++) {
        snprintf(tmp, NODE_MAX, PMQOSWFI_LATENCY_NODE, i);
        fd = open(tmp, O_RDONLY);
        if (fd != -1) {
            memset(buf, 0x0, sizeof(buf));
            n = read(fd, buf, MAX_BUF_SIZE-1);
            if (n > 0) {
                latency = strtol(&buf[0], NULL, 0);
                max_latency = max(latency, max_latency);
            }
            close(fd);
        } else {
            QLOGL(LOG_TAG, QLOG_WARNING, "Unable to read node = %s", tmp);
        }
    }

    if(max_latency == 0) {
        QLOGE(LOG_TAG, "Unable to get mPmQosWfi latency, So initializing to default 0 value");
    } else {
        mPmQosWfi = max_latency + 1;
        QLOGL(LOG_TAG, QLOG_L2, "mPmQosWfi latency caluculated as = %" PRIu32, mPmQosWfi);
    }
}
/* This function returns the mapped value for
 * a major resource and minor resource, specified
 * through the index.
 * */
int32_t Target::getMappedValue(uint16_t resrcIndex, uint16_t val) {
    if ((resrcIndex >= 0) && (resrcIndex < MAX_MINOR_RESOURCES)
           && (resrcIndex != UNSUPPORTED_Q_INDEX)) {
        if(mValueMap[resrcIndex].map != NULL) {
            if (val >= 0 && val < mValueMap[resrcIndex].mapSize) {
                return mValueMap[resrcIndex].map[val];
            } else {
                QLOGE(LOG_TAG, "Value Map not defined for this %" PRIu16 " index for the %" PRIu16 " value", resrcIndex, val);
            }
        } else {
            QLOGE(LOG_TAG, "Value map not defined for this %" PRIu16 " index", resrcIndex);
        }
    } else {
        QLOGE(LOG_TAG, "Error: Cannot find mapped value for the resource with index %" PRIu16, resrcIndex);
    }
    return -1;
}

/* Calculate bitmask of all CPUs in target.
 * For example, if total number of CPUs is 4,
 * then bitmask will be 0b1111 (or 0xF).
 * */
int8_t Target::getAllCpusBitmask() {
    if(mTargetConfig.getTotalNumCores()) {
        return (1 << mTargetConfig.getTotalNumCores()) - 1;
    } else {
        QLOGE(LOG_TAG, "Error: Initializing total cores failed\n");
        return -1;
    }
}

/* Store all the available GPU freq in an integer array */
void Target::mInitGpuAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int32_t rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuFreq >= GPU_FREQ_LVL) {
                    QLOGE(LOG_TAG, "Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuAvailFreq[mTotalGpuFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuAvailFreq, mGpuAvailFreq + mTotalGpuFreq);
    } else {
        QLOGE(LOG_TAG, "Initialization of GPU available freq failed, as %s not present", GPU_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int32_t Target::findNextGpuFreq(int32_t freq) {
    uint32_t i = 0;

    for (i = 0; i < mTotalGpuFreq; i++) {
        if (mGpuAvailFreq[i] >= freq) {
            return mGpuAvailFreq[i];
        }
    }

    if ((mTotalGpuFreq != 0) && (i ==  mTotalGpuFreq)) {
        return mGpuAvailFreq[mTotalGpuFreq-1];
    }

    return FAILED;
}

/* Return max GPU freq */
int32_t Target::getMaxGpuFreq() {
    if (mTotalGpuFreq > 0) {
        return mGpuAvailFreq[mTotalGpuFreq-1];
    }

    return FAILED;
}

/* Store all the available GPU bus freq in an integer array */
void Target::mInitGpuBusAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int32_t rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_BUS_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuBusFreq >= GPU_FREQ_LVL) {
                    QLOGE(LOG_TAG, "Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuBusAvailFreq[mTotalGpuBusFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuBusAvailFreq, mGpuBusAvailFreq + mTotalGpuBusFreq);
    } else {
        QLOGE(LOG_TAG, "Initialization of GPU Bus available freq failed, as %s not present", GPU_BUS_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int32_t Target::findNextGpuBusFreq(int32_t freq) {
    uint32_t i = 0;

    for (i = 0; i < mTotalGpuBusFreq; i++) {
        if (mGpuBusAvailFreq[i] >= freq) {
            return mGpuBusAvailFreq[i];
        }
    }

    if ((mTotalGpuBusFreq != 0) && (i ==  mTotalGpuBusFreq)) {
        return mGpuBusAvailFreq[mTotalGpuBusFreq-1];
    }

    return FAILED;
}
uint32_t Target::getBoostConfig(int32_t hintId, int32_t type, int32_t *mapArray, int32_t *timeout, uint32_t fps) {
    uint32_t size = 0;

    if (NULL == mapArray) {
        return size;
    }

    if (NULL != mPerfDataStore) {
        size = mPerfDataStore->GetBoostConfig(hintId, type, mapArray, timeout,
                                                 mTargetConfig.getTargetName().c_str(), mTargetConfig.getResolution(), fps);
    }
    return size;
}

int32_t Target::getConfigPerflockNode(int32_t resId, char *node, bool &supported) {
    int32_t ret = FAILED;

    if (resId < 0) {
        return 0;
    }

    if (NULL != mPerfDataStore) {
        ret = mPerfDataStore->GetResourceConfigNode(resId, node, supported);
    }
    return ret;
}

uint32_t Target::GetAllBoostHintType(vector<pair<int32_t, pair<int32_t,uint32_t>>> &hints_list) {
    if (NULL != mPerfDataStore) {
        return mPerfDataStore->GetAllBoostHintType(hints_list);
    }
    return 0;
}

void Target::Dump() {
    int8_t i;
    int8_t num_clusters = mTargetConfig.getNumCluster();
    for (i =0; i < num_clusters; i++) {
        if (mPhysicalClusterMap) {
            QLOGV(LOG_TAG, "Logical cluster %" PRId8 " is mapped to physical cluster %" PRId8, i, mPhysicalClusterMap[i]);
        }
        if (mLastCoreIndex) {
            QLOGV(LOG_TAG, "End index for physical cluster %" PRId8 " is %" PRId8,i, mLastCoreIndex[i]);
        }
    }
    for (i=0; i < MAX_PRE_DEFINE_CLUSTER-START_PRE_DEFINE_CLUSTER; i++) {
        if (mPredefineClusterMap) {
            QLOGV(LOG_TAG, "Logical pre-defined cluster %" PRId8 " is mapped to physical cluster %" PRId32,
                  i+START_PRE_DEFINE_CLUSTER, mPredefineClusterMap[i]);
        }
    }
    QLOGV(LOG_TAG, "PmQosWfi_latency %" PRIu32, mPmQosWfi);

    return;
}
