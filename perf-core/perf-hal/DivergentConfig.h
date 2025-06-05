/******************************************************************************
  @file    DivergentConfig.h
  @brief   Implementation of DivergentConfigs

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_DIVERGENTCONFIG_H__
#define __PERF_DIVERGENTCONFIG_H__

#define FAILED -1
#define MAX_CLUSTER 4
#define NODE_MAX 150

#define POLICY_DIR_PATH "/sys/devices/system/cpu/cpufreq/"
#define RELATED_CPUS_NODE_PATH "/sys/devices/system/cpu/cpufreq/policy%d/related_cpus"
#define CPU_CAPACITY_NODE_PATH "/sys/devices/system/cpu/cpu%d/cpu_capacity"
#define PHYSICAL_PKG_ID_NODE_PATH "/sys/devices/system/cpu/cpu%d/topology/cluster_id"
#define SUBSYSTEM_AVAILABILITY_CHECK_NODE "/sys/devices/soc0/"

class DivergentConfig {

private:

    struct clusterInfo {
        uint8_t physicalClusterId = 0;
        uint8_t firstCore = 0;
        uint8_t lastCore = 0;
        uint8_t numCores = 0;
        int32_t capacity = 0;

        // to sort the cluster info based on physicalClusterId.
        // needed for generating correct divergent number
        bool operator<(const clusterInfo& cInfo) const {
            return physicalClusterId < cInfo.physicalClusterId;
        }
    };

    uint8_t mNumCluster = 0;
    uint8_t mTotalNumCores = 0;
    uint8_t mGovInstanceType;
    uint8_t mCoreCtlCpu;
    uint32_t divergentNumber = 0;
    clusterInfo *mClusterInfo = nullptr;
    uint8_t mFirstCorePerCluster[MAX_CLUSTER];

    bool isDisplayEnabled = true;
    bool isGpuEnabled = true;

    // Singleton object of this class
    static DivergentConfig divergent;

    // ctor, copy ctor and assignment overloading
    DivergentConfig();
    DivergentConfig(DivergentConfig const& oh);
    DivergentConfig& operator=(DivergentConfig const& oh);

    // Read functions
    void readNumClusters();
    void readNumCores();
    void readPerClusterInfo();
    void readCapacityPerCluster();
    void readCoreCtlCpu();
    void readPhysicalCluster();

    bool isSubsystemEnabled(const char* subsystem);

    void determineGovernorInstType();
    void determineType();

    void GenerateDivergentNumber();

public:
    ~DivergentConfig();

    void InitializeDivergentConfig();
    bool checkClusterPresent(uint8_t cluster);

    int8_t getNumCluster() {
        return mNumCluster;
    }

    uint8_t getTotalNumCores() {
        return mTotalNumCores;
    }

    uint8_t getCoreCtlCpu() {
        return mCoreCtlCpu;
    }

    uint8_t getGovInstanceType() {
        return mGovInstanceType;
    }

    uint8_t getCoresInCluster(uint8_t cluster);

    uint32_t getDivergentNumber() {
        return divergentNumber;
    }

    int8_t getFirstPhysicalCluster() {
        return mClusterInfo[0].physicalClusterId;
    }

    int8_t getPhysicalCluster(uint8_t index) {
        if (mClusterInfo == NULL)
            return -1;

        return mClusterInfo[index].physicalClusterId;
    }

    static DivergentConfig& getDivergentConfig() {
        return divergent;
    }

    bool checkDisplayEnabled() {
        return isDisplayEnabled;
    }

    bool checkGpuEnabled() {
        return isGpuEnabled;
    }

    void DumpAll();
};

#endif /* __PERF_DIVERGENTCONFIG_H__ */
