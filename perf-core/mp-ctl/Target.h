/******************************************************************************
  @file    Target.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __TARGET_SPECIFICS__H_
#define __TARGET_SPECIFICS__H_

#include "ResourceInfo.h"
#include "PerfController.h"
#include "TargetConfig.h"
#include <PerfConfig.h>

#define GPU_FREQ_LVL 16
//TODO Remove the usage for this.
#define MAX_CORES 8

using namespace std;

class PerfDataStore;
/* Structure for defining value maps.
 * */
typedef struct value_map{
    int32_t *map;
    uint32_t mapSize;
}value_map;

/* Target class: It contains target
 * specific information. It is a singleton
 * class. On mpctl-server-init a object for this
 * class is initialized with the current target
 * specific information.
 * */
class Target {

private:
    int8_t *mPhysicalClusterMap; /*logical to physical cluster map*/
    int32_t *mPredefineClusterMap; /*pre-defined logical cluster to physical cluster map*/
    int8_t *mLastCoreIndex; /*Maintain end core index for each physical cluster*/

    /*resource supported bitmaps which will let us know which resources are
     *supported on this target. This is a bitmap of 64bits per Major        *
     *resource. Every bit representing the minor resource in the major      *
     *resource.                                                             */
    uint64_t mResourceSupported[MAX_MAJOR_RESOURCES];


    /*Holds the latency to restrict to WFI
     * This value is target specific. Set this to 1 greater than the WFI
     * of the Power Cluster (cluster 0 pm-cpu "wfi"). Look for "wfi". This will
     * ensure the requirement to restrict a vote to WFI for both clusters
     */
    uint32_t mPmQosWfi;

    char mStorageNode[NODE_MAX];

    /* Value map is declared for each resource and defined
     * by a target for specific resources.*/
    struct value_map mValueMap[MAX_MINOR_RESOURCES];


    // Singelton object of this class
    static Target cur_target;


    /* Array for storing available GPU frequencies */
    int32_t mGpuAvailFreq[GPU_FREQ_LVL];
    int32_t mGpuBusAvailFreq[GPU_FREQ_LVL];

    /* Store no. of gpu freq level */
    uint32_t mTotalGpuFreq;
    uint32_t mTotalGpuBusFreq;

    //boost configs, target configs and mappings store
    PerfDataStore *mPerfDataStore;

    bool mUpDownComb = false;
    bool mGrpUpDownComb = false;
    bool mTraceFlag;
    TargetConfig &mTargetConfig = TargetConfig::getTargetConfig();

    void ExtendedTargetInit();
    void InitializeKPMnodes();

    void mLogicalPerfMapPerfCluster();
    void mLogicalPerfMapPowerCluster();
    void mCalculateCoreIdx();
    void mSetAllResourceSupported();
    void mResetResourceSupported(uint16_t, uint16_t);
    void mInitAllResourceValueMap();
    void mInitGpuAvailableFreq();
    void mInitGpuBusAvailableFreq();

    //init support routines
    void readPmQosWfiValue();
    //ctor, copy ctor, assignment overloading
    Target();
    Target(Target const& oh);
    Target& operator=(Target const& oh);

    // map to associate logical cluster ids to their cluster names
    const std::map<uint8_t, std::string> mLogicalClusterIdToNameMap = {
        { 0, "big" },
        { 1, "little" },
        { 2, "prime" },
        { 3, "titanium" },
    };

public:
    ~Target();

    void InitializeTarget();
    void TargetInit();

    int8_t getPhysicalCluster(int8_t logicalCluster);
    int8_t getFirstCoreOfPerfCluster();
    int8_t getFirstCoreOfClusterWithSpilloverRT();
    int8_t getNumLittleCores();
    int8_t getLastCoreIndex(int8_t cluster);
    int8_t getFirstCoreIndex(int8_t cluster);
    int8_t getPhysicalCore(int8_t cluster, int8_t logicalCore);
    bool isResourceSupported(uint16_t major, uint16_t minor);
    int8_t getClusterForCpu(int8_t cpu, int8_t &startcpu, int8_t &endcpu);
    int32_t getMappedValue(uint16_t qindx, uint16_t val);
    int8_t getAllCpusBitmask();
    int32_t findNextGpuFreq(int32_t freq);
    int32_t getMaxGpuFreq();
    int32_t findNextGpuBusFreq(int32_t freq);
    uint32_t getBoostConfig(int32_t hintId, int32_t type, int32_t *mapArray, int32_t *timeout, uint32_t fps);
    uint32_t GetAllBoostHintType(vector<pair<int32_t, pair<int32_t,uint32_t>>> &hints_list);
    int32_t getConfigPerflockNode(int32_t resId, char *node, bool &supported);
    bool isLittleCluster(uint8_t cluster_id);

    char* getStorageNode() {
        return mStorageNode;
    }

    uint32_t getMinPmQosLatency() {
        return mPmQosWfi;
    }

    void Dump();

    //Initialize cur_target
    static Target &getCurTarget() {
        return cur_target;
    }

    bool isUpDownCombSupported() {
        return mUpDownComb;
    }
    void setTraceEnabled(bool flag) {
        mTraceFlag = flag;
    }
    bool isTraceEnabled() {
        return mTraceFlag;
    }

    bool isGrpUpDownCombSupported() {
        return mGrpUpDownComb;
    }

    std::string getClusterName(uint8_t logicalClusterId) {
        return mLogicalClusterIdToNameMap.at(logicalClusterId);
    }
};

#endif /*__TARGET_SPECIFICS__H_*/
