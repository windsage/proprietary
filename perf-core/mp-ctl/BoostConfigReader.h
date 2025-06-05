/******************************************************************************
  @file    Boostconfigreader.h
  @brief   Implementation of boostconfigreader

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __BOOSTCONFIG_READER__H_
#define __BOOSTCONFIG_READER__H_

#include <vector>
#include "XmlParser.h"
#include "Target.h"
#include <unordered_map>

/* size of logical cluster map is 8, size of logical
 * freq map is 5, so taking max(8) */
#define MAX_MAP_TABLE_SIZE 8

#define TARG_NAME_LEN 32
#define MAX_OPCODE_VALUE_TABLE_SIZE 64

using namespace std;

class PerfDataStore {
public:
    typedef enum {
        MAP_TYPE_UNKNOWN,
        MAP_TYPE_FREQ,
        MAP_TYPE_CLUSTER
    }ValueMapType;

private:
    class ParamsMappingInfo {
    public:
        explicit ParamsMappingInfo(uint32_t mtype, const char *tname, uint32_t res, int32_t maptable[], uint32_t mapsize);

        uint32_t mMapType;
        char mTName[TARG_NAME_LEN];
        uint32_t mResolution;
        int32_t mMapTable[MAX_MAP_TABLE_SIZE];
        uint32_t mMapSize;
    };

    class BoostConfigInfo {
    public:
        explicit BoostConfigInfo(int32_t idnum, int32_t type, bool enable, int32_t timeout, int32_t fps,const char *tname, uint32_t res, char *resourcesPtr);
        int32_t mId;
        int32_t mType;
        bool mEnable;
        int32_t mTimeout;
        int32_t mFps;
        char mTName[TARG_NAME_LEN];
        uint32_t mResolution;
        int32_t mConfigTable[MAX_OPCODE_VALUE_TABLE_SIZE];
        uint32_t mConfigsSize;
        uint32_t mDivergentNumber;
        bool mUseDivergentConfig;
    };

    /*Following class is used to Store the information present in resource config XML files.
    mResId is calculated from Major and Minor values, mNodePath for sysfsnode path and mSupported
    flag to indicate whether this resource is supported for that target or not.*/
    class ResourceConfigInfo {
    public:
        explicit ResourceConfigInfo(int32_t idx, char *rsrcPath, bool supported);
        int32_t mResId;
        char mNodePath[NODE_MAX];
        bool mSupported;
    };

private:
    //perf boost configs
    unordered_map<int32_t,unordered_map<int32_t,vector<BoostConfigInfo>>> mBoostConfigs;

    //power hint
    unordered_map<int32_t,unordered_map<int32_t,vector<BoostConfigInfo>>> mPowerHint;

    //perf params mappings
    vector<ParamsMappingInfo*> mBoostParamsMappings;

    //perf resource configs
    vector<ResourceConfigInfo*> mResourceConfig;

    //perf sys nodes configs
    vector<ResourceConfigInfo*> mSysNodesConfig;

    //Context Hints
    unordered_map<int32_t,unordered_map<int32_t,vector<int32_t>>> mContextHints;

    // Singelton object of this class
    static PerfDataStore mPerfStore;

    friend void Target::TargetInit();

private:
    //xml open/read/close
    void XmlParserInit();

    //target specific boost params xml CBs
    static void BoostParamsMappingsCB(xmlNodePtr node, void *);

    //target specific boost configurations xml CB
    static void BoostConfigsCB(xmlNodePtr node, void *);

    //perflock resource configurations xml CB
    static void CommonResourcesCB(xmlNodePtr node, void *);
    static void TargetResourcesCB(xmlNodePtr node, void *);

    //common sysnodes config
    static void CommonSysNodesCB(xmlNodePtr node, void *);
    static void TargetSysNodesCB(xmlNodePtr node, void *);


    //support routines
    ValueMapType ConvertToEnumMappingType(char *maptype);
    ValueMapResType ConvertToEnumResolutionType(char *res);
    static uint32_t ConvertToIntArray(char *mappings, int32_t mapArray[], uint32_t msize);
    static void UpdateResourceConfig(int resId, const char *node, bool supported);
    static void UpdateSysNodeConfig(int resId, const char *node, bool supported);
    static void UpdateConfig(vector<ResourceConfigInfo*> &config, int32_t resId, const char *node, bool supported);
    static void assignBCIObjs(PerfDataStore *store, BoostConfigInfo &obj, int32_t idnum, int32_t type);
    static void setIgnoreHints(PerfDataStore *store, int32_t id, int32_t type, char* Hints);

    //ctor, copy ctor, assignment overloading
    PerfDataStore();
    PerfDataStore(PerfDataStore const& oh);
    PerfDataStore& operator=(PerfDataStore const& oh);

public:
    //interface to boost params mappings
    static long int ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue);
    uint32_t GetFreqMap(uint32_t res, int32_t **maparray, const char *tname);
    uint32_t GetClusterMap(int32_t **maparray, const char *tname);

    //interface to boost configs
    uint32_t GetBoostConfig(int32_t hintId, int32_t type, int32_t *mapArray, int32_t *timeout,
                      const char *tName = NULL, uint32_t res = MAP_RES_TYPE_ANY, int32_t fps = 0);

    uint32_t GetAllBoostHintType(vector<pair<int32_t, pair<int32_t,uint32_t>>> &hints_list);

    int32_t GetResourceConfigNode(int32_t resId, char *node, bool &supported);

    int32_t GetSysNode(const int32_t idx_value, char *node_path, bool &supported);
    int32_t GetSysNode(const int32_t idx_value, char *node_path);

    bool getIgnoreHints(int32_t id, int32_t type, vector<int32_t> &hints);

    //Initialize cur_target
    void Init();
    void TargetResourcesInit();

    static PerfDataStore *getPerfDataStore() {
        return &mPerfStore;
    }

    //dtor
    ~PerfDataStore();
};
#endif /*__BOOSTCONFIG_READER__H_*/
