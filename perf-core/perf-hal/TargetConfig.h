/******************************************************************************
  @file    TargetConfig.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_TARGETCONFIG_H__
#define __PERF_TARGETCONFIG_H__

#include <string>
#include "XmlParser.h"
#include <vector>
#include <atomic>
#include <map>
#include "DivergentConfig.h"

#define MAX_SUPPORTED_SOCIDS 15
#define MAX_CONFIGS_SUPPORTED_PER_PLATFORM 5
#define API_LEVEL_PROP_NAME "ro.board.first_api_level"
#define S_API_LEVEL 31
#define MAX_ARGS_PER_REQUEST_LIMIT 256

using namespace std;

enum {
    CLUSTER_BASED_GOV_INSTANCE, /*both cluster based/core based*/
    SINGLE_GOV_INSTANCE,
    MAX_GOVINSTANCETYPE,
}IntGovInstanceType;

enum {
    INTERACTIVE, /*both cluster based/core based*/
    SCHEDUTIL,
    MAX_CPUFREQGOV,
}CpufreqGov;

typedef enum {
        MAP_RES_TYPE_ANY   = 0,
        MAP_RES_TYPE_720P  = 720,
        MAP_RES_TYPE_HD_PLUS = 721, //Using 721 for HD+ to align with existing implementation
        MAP_RES_TYPE_1080P = 1080,
        MAP_RES_TYPE_2560  = 2560
}ValueMapResType;

#define MEM_1GB (1*1024*1024)
#define MEM_2GB (2*1024*1024)
#define MEM_3GB (3*1024*1024)
#define MEM_4GB (4*1024*1024)
#define MEM_6GB (6*1024*1024)
#define MEM_8GB (8*1024*1024)
#define MEM_10GB (10*1024*1024)
#define MEM_12GB (12*1024*1024)

enum {
    RAM_1GB = 1, /*Ram Size*/
    RAM_2GB,
    RAM_3GB,
    RAM_4GB,
    RAM_6GB = 6,
    RAM_8GB = 8,
    RAM_10GB = 10,
    RAM_12GB = 12/*For 12 and 12+ GB Ram size targets*/
}RamVar;

/* TargetConfig class: It contains target
 * specific information. It is a singleton
 * class. On perf-hal a object for this
 * class is initialized with the current target
 * specific information.
 * */
class TargetConfig {

private:
    class TargetConfigInfo {
    public:
        bool mSyncCore = true, mUpdateTargetInfo = true;
        uint8_t mNumCluster = 0, mTotalNumCores = 0, mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE, mCpufreqGov = INTERACTIVE;
        uint16_t mTargetMaxArgsPerReq = 0;
        string mTargetName;
        uint32_t mType = 0, mType2 = 0, mNumSocids = 0;
        int8_t mCoreCtlCpu = -1,mMinCoreOnline = 0, mCalculatedCores = 0;
        uint32_t mCpumaxfrequency[MAX_CLUSTER], mCpuCappedMaxfreq[MAX_CLUSTER];
        int16_t mMinFpsTuning = 0, mSupportedSocids[MAX_SUPPORTED_SOCIDS];
        int8_t mCorepercluster[MAX_CLUSTER];
        std::map<std::string, int> mClusterNameToIdMap;
        bool CheckSocID(int16_t SocId);
        bool CheckClusterInfo();
    };

private:
    int16_t mSocID;
    int32_t mRam;
    int32_t mRamKB;
    bool mSyncCore;  /*true- if target is sync core*/
    uint8_t mNumCluster;  /*total number of cluster*/
    uint8_t mTotalNumCores;    /*total number of cores*/
    uint8_t mNumBigCores; /*number of big cores*/
    int8_t *mCorePerCluster; /*number of cores per cluster*/
    uint32_t mResolution;
    string mTargetName; /*Store the name of that target*/
    string mVariant; /*Store the Variant of current target (go/32/64 variant)*/
    /* Array for storing cpu max frequency reset value
     *   for each cluster. */
    uint32_t mCpuMaxFreqResetVal[MAX_CLUSTER];
    uint32_t mCpuCappedMaxfreqVal[MAX_CLUSTER];
    int8_t mCoreCtlCpu; /*core_ctl is enabled on which physical core*/
    int8_t mMinCoreOnline; /*Minimum number of cores needed to be online*/
    //defines which type of int gov instance target has
    //paths will vary based on this
    uint8_t mGovInstanceType;
    //returns cpufreq governor
    uint8_t mCpufreqGov;
    uint32_t mType;
    uint32_t mType2;
    int16_t mMinFpsTuning; /*Minmum FPS for which system tuning is supported*/
    // Singelton object of this class
    static TargetConfig cur_target;
    //target config object
    vector<TargetConfigInfo*> mTargetConfigs;
    uint16_t mTargetMaxArgsPerReq;
    uint32_t mDivergentNumber;
    bool mDisplayEnabled;
    bool mGpuEnabled;

    // map to associate cluster names like "little", "big" and "prime"
    // to their physical cluster ids.
    // For example,
    // "little" -> 0,
    // "big" -> 1,
    // "prime" -> 2
    std::map<std::string, int> mClusterNameToIdMap;

    /*
     * kernel base version
     */
    string mFullKernelVersion;
    int8_t mKernelMajorVersion;
    int8_t mKernelMinorVersion;
    int32_t mFirstAPILevel;
    //init support routines
    int16_t readSocID();
    uint32_t readResolution();
    int32_t readRamSize(void);
    //read Kernel Version of the Target
    void readKernelVersion();
    void readVariant();
    //target specific configc xml CB
    static void TargetConfigsCB(xmlNodePtr node, void *index);
    static uint32_t ConvertToIntArray(char *mappings, int16_t mapArray[], uint32_t msize);
    static long int ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue);
    void InitTargetConfigXML();
    atomic<bool> mInitCompleted;

    //ctor, copy ctor, assignment overloading
    TargetConfig();
    TargetConfig(TargetConfig const& oh);
    TargetConfig& operator=(TargetConfig const& oh);

    // DivergentConfig object
    DivergentConfig &divergentConf = DivergentConfig::getDivergentConfig();

    bool CheckDefaultDivergent(TargetConfigInfo *config);
    bool isDefaultDivergent = true;

    uint8_t determineTotalNumCores(TargetConfigInfo *config) {
        if (config == NULL)
            return 0;
        return isDefaultDivergent ? config->mTotalNumCores : divergentConf.getTotalNumCores();
    }

    void setUnsupportedPhysicalCluster(uint8_t cluster);

    std::string getClusterType(uint8_t cluster);

    void updateClusterNameToIdMap(uint8_t cluster);

    uint8_t determineCoresPerCluster(TargetConfigInfo *config, uint8_t cluster) {
        if (config == NULL)
            return 0;

        if (!isClusterSupported(cluster))
            return 0;

        return isDefaultDivergent ? config->mCorepercluster[cluster] : divergentConf.getCoresInCluster(cluster);
    }

    uint8_t determineGovernorInstType(TargetConfigInfo *config) {
        if (config == NULL)
            return CLUSTER_BASED_GOV_INSTANCE;
        return isDefaultDivergent ? config->mGovInstanceType : divergentConf.getGovInstanceType();
    }

public:
    ~TargetConfig();

    void InitializeTargetConfig();
    void TargetConfigInit();

    bool isSyncCore() {
        return mSyncCore;
    }
    int8_t getNumCluster() {
        return mNumCluster;
    }
    int8_t getTotalNumCores()  {
        return mTotalNumCores;
    }
    int8_t getBigCoresNumber() {
        return mNumBigCores;
    }
    int8_t getCoresInCluster(int8_t cluster);
    uint32_t getCpuMaxFreqResetVal(int8_t cluster);
    uint32_t getCpuCappedMaxFreqVal(int8_t cluster);

    bool isClusterSupported(uint8_t cluster);

    int16_t getSocID() {
    return mSocID;
    }
    uint32_t getResolution() {
        return mResolution;
    }
    int8_t getCoreCtlCpu() {
        return mCoreCtlCpu;
    }
    int8_t getMinCoreOnline() {
        return mMinCoreOnline;
    }
    uint8_t getGovInstanceType() {
        return mGovInstanceType;
    }
    uint8_t getCpufreqGov() {
        return mCpufreqGov;
    }
    string &getTargetName() {
        return mTargetName;
    }
    string &getVariant() {
        return mVariant;
    }
    int8_t getKernelMinorVersion() {
        return mKernelMinorVersion;
    }
    int8_t getKernelMajorVersion() {
        return mKernelMajorVersion;
    }
    string &getFullKernelVersion() {
        return mFullKernelVersion;
    }
    int32_t getRamSize() {
        return mRam;
    }
    int32_t getRamInKB() {
        return mRamKB;
    }

    uint32_t getType() {
        return mType;
    }

    uint32_t getType2() {
        return mType2;
    }

    int16_t getMinFpsTuning() {
        return mMinFpsTuning;
    }

    uint32_t getDivergentNumber() {
        return mDivergentNumber;
    }

    void setInitCompleted(bool flag);

    bool getInitCompleted() {
        return mInitCompleted.load();
    }
    //Initialize cur_target
    static TargetConfig &getTargetConfig() {
        return cur_target;
    }
    TargetConfigInfo *getTargetConfigInfo(int16_t socId);
    int8_t getPhysicalClusterId(std::string& clusterName);

    uint16_t getTargetMaxArgsPerReq() {
        return mTargetMaxArgsPerReq;
    }

    uint16_t getMaxArgsPerReq() {
        return mTargetMaxArgsPerReq;
    }

    bool getIsDefaultDivergent() {
        return isDefaultDivergent;
    }

    bool isDisplayEnabled() {
        return mDisplayEnabled;
    }

    bool isGpuEnabled() {
        return mGpuEnabled;
    }

    void DumpAll();

};

#endif /*__PERF_TARGETCONFIG_H__*/
