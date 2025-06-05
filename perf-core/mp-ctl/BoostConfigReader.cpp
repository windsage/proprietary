/******************************************************************************
  @file    BoostConfigReader.cpp
  @brief   Implementation of reading boost config xml files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-BOOSTCONFIG"

#include "BoostConfigReader.h"
#include "MpctlUtils.h"
#include "PerfLog.h"
#include "config.h"

//perf mapping tags in xml file
#define PERF_BOOSTS_XML_ROOT "PerfBoosts"
#define PERF_BOOSTS_XML_CHILD_MAPPINGS "BoostParamsMappings"
#define PERF_BOOSTS_XML_ATTRIBUTES_TAG "BoostAttributes"
#define PERF_BOOSTS_XML_MAPTYPE_TAG "MapType"
#define PERF_BOOSTS_XML_RESOLUTION_TAG "Resolution"
#define PERF_BOOSTS_XML_MAPPINGS_TAG "Mappings"
#define PERF_BOOSTS_XML_TARGET_TAG "Target"

//perf boost config tags
#define BOOSTS_CONFIGS_XML_ROOT "BoostConfigs"
#define BOOSTS_CONFIGS_XML_CHILD_CONFIG "PerfBoost"
#define BOOSTS_CONFIGS_XML_ELEM_CONFIG_TAG "Config"
#define BOOSTS_CONFIGS_XML_ELEM_RESOURCES_TAG "Resources"
#define BOOSTS_CONFIGS_XML_ELEM_ENABLE_TAG "Enable"
#define BOOSTS_CONFIGS_XML_ELEM_ID_TAG "Id"
#define BOOSTS_CONFIGS_XML_ELEM_TYPE_TAG "Type"
#define BOOSTS_CONFIGS_XML_ELEM_TIMEOUT_TAG "Timeout"
#define BOOSTS_CONFIGS_XML_ELEM_KERNEL_TAG "Kernel"
#define BOOSTS_CONFIGS_XML_ELEM_FPS_TAG "Fps"
#define BOOSTS_CONFIGS_XML_DIVERGENT_TAG "Divergent"

//resource config tags
#define RESOURCE_CONFIGS_XML_ROOT "ResourceConfigs"
#define RESOURCE_CONFIGS_XML_CHILD_CONFIG "PerfResources"
#define RESOURCE_CONFIGS_XML_MAJORCONFIG_TAG "Major"
#define RESOURCE_CONFIGS_XML_MINORCONFIG_TAG "Minor"
#define RESOURCE_CONFIGS_XML_SYSNODE_TAG "SysNode"
#define RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG "OpcodeValue"
#define RESOURCE_CONFIGS_XML_ELEM_NODE_TAG "Node"
#define RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG "Supported"
#define RESOURCE_CONFIGS_XML_CONFIG_TAG "Config"
#define RESOURCE_CONFIGS_XML_ELEM_MAJORVALUE_TAG "MajorValue"
#define RESOURCE_CONFIGS_XML_ELEM_MINORVALUE_TAG "MinorValue"
#define RESOURCE_CONFIGS_XML_ELEM_KERNEL_TAG "Kernel"
#define RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG "Target"
#define RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG "Idx"
#define RESOURCE_CONFIGS_XML_IGNORE_HINTS_TAG "IgnoreHints"

//power hint tags
#define POWER_HINT_XML_ROOT "HintConfigs"
#define POWER_HINT_XML_CHILD_CONFIG "Powerhint"

#define MAP_TYPE_VAL_FREQ "freq"
#define MAP_TYPE_VAL_CLUSTER "cluster"
#define MAP_RES_TYPE_VAL_1080p "1080p"
#define MAP_RES_TYPE_VAL_2560 "2560"
#define MAP_RES_TYPE_VAL_720p "720p"
#define MAP_RES_TYPE_VAL_HD_PLUS "HD+"

#define FPS_CONFIG_COUNT 16
#define MAX_IGNORE_HINTS 8

using namespace std;

#define PERF_MAPPING_XML (VENDOR_DIR"/perf/perfmapping.xml")
#define PERF_BOOSTS_CONFIGS_XML (VENDOR_DIR"/perf/perfboostsconfig.xml")
#define POWER_CONFIGS_XML (VENDOR_DIR"/powerhint.xml")
#define COMMONRESOURCE_CONFIGS_XML (VENDOR_DIR"/perf/commonresourceconfigs.xml")
#define COMMON_SYSNODES_CONFIGS_XML (VENDOR_DIR"/perf/commonsysnodesconfigs.xml")
#define TARGETRESOURCE_CONFIGS_XML (VENDOR_DIR"/perf/targetresourceconfigs.xml")
#define TARGETRE_SYSNODES_CONFIGS_XML (VENDOR_DIR"/perf/targetsysnodesconfigs.xml")

PerfDataStore PerfDataStore::mPerfStore;

PerfDataStore::ParamsMappingInfo::ParamsMappingInfo(uint32_t mtype, const char *tname, uint32_t res, int32_t maptable[], uint32_t mapsize) {
    mMapType = mtype;
    memset(mTName, 0, sizeof(mTName));
    if (tname) {
        strlcpy(mTName, tname, TARG_NAME_LEN);
    }
    mResolution = res;
    mMapSize = (mapsize<=MAX_MAP_TABLE_SIZE)?mapsize:MAX_MAP_TABLE_SIZE;
    memset(mMapTable, -1, sizeof(mMapTable));
    for (uint32_t i=0; i < mMapSize; i++) {
        mMapTable[i] = maptable[i];
    }
}

PerfDataStore::BoostConfigInfo::BoostConfigInfo(int32_t idnum, int32_t type, bool enable, int32_t timeout, int32_t fps, const char *tname, uint32_t res, char *resourcesPtr) {
    mId = idnum;
    mType = type;
    mEnable = enable;
    mTimeout = timeout;
    mFps = fps;
    mUseDivergentConfig = false;

    memset(mTName, 0, sizeof(mTName));
    if (tname) {
        strlcpy(mTName, tname, TARG_NAME_LEN);
    }

    mResolution = res;

    memset(mConfigTable, -1, sizeof(mConfigTable));
    mConfigsSize = ConvertToIntArray(resourcesPtr, mConfigTable, MAX_OPCODE_VALUE_TABLE_SIZE);
}

PerfDataStore::ResourceConfigInfo::ResourceConfigInfo(int32_t idx, char *rsrcPath, bool supported) {
    mSupported = supported;
    mResId = idx;

    memset(mNodePath, 0, sizeof(mNodePath));
    if ((rsrcPath != NULL) && (strlen(rsrcPath) > 0)) {
        strlcpy(mNodePath, rsrcPath, NODE_MAX);
    }
}

PerfDataStore::PerfDataStore() {
}

PerfDataStore::~PerfDataStore() {
    //delete mappings
    while (!mBoostParamsMappings.empty()) {
        ParamsMappingInfo *tmp = mBoostParamsMappings.back();
        if (tmp) {
            delete tmp;
        }
        mBoostParamsMappings.pop_back();
    }
    //delete resource configs
    while (!mResourceConfig.empty()) {
        ResourceConfigInfo *tmp = mResourceConfig.back();
        if (tmp) {
            delete tmp;
        }
        mResourceConfig.pop_back();
    }
    //delete sysnodes configs
    while (!mSysNodesConfig.empty()) {
        ResourceConfigInfo *tmp = mSysNodesConfig.back();
        if (tmp) {
            delete tmp;
        }
        mSysNodesConfig.pop_back();
    }
}

void PerfDataStore::Init() {
    XmlParserInit();
}

void PerfDataStore::XmlParserInit() {
    const string fMappingsName(PERF_MAPPING_XML);
    const string fCommonResourcesName(COMMONRESOURCE_CONFIGS_XML);
    const string fCommonSysNodesName(COMMON_SYSNODES_CONFIGS_XML);
    const string xmlMappingsRoot(PERF_BOOSTS_XML_ROOT);
    const string xmlChildMappings(PERF_BOOSTS_XML_CHILD_MAPPINGS);
    const string xmlResourceConfigRoot(RESOURCE_CONFIGS_XML_ROOT);
    const string xmlChildResourceConfig(RESOURCE_CONFIGS_XML_CHILD_CONFIG);

    int32_t idnum;

    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }

    //appboosts mappings
    idnum = xmlParser->Register(xmlMappingsRoot, xmlChildMappings, BoostParamsMappingsCB, NULL);
    xmlParser->Parse(fMappingsName);
    xmlParser->DeRegister(idnum);

    //common resource configs
    /*In common resource configs XMl file, the major and minor values are present in different fields
      and both these values are required to calculate the resource index. So, passing Major value
      as an argument while parsing each row of XML file and update it when we see a new Major value.*/
    int32_t major_value = -1;
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, CommonResourcesCB, &major_value);
    xmlParser->Parse(fCommonResourcesName);
    xmlParser->DeRegister(idnum);

    //sysnode configs
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, CommonSysNodesCB, NULL);
    xmlParser->Parse(fCommonSysNodesName);
    xmlParser->DeRegister(idnum);

    delete xmlParser;

    return;
}

void PerfDataStore::TargetResourcesInit() {
    const string xmlResourceConfigRoot(RESOURCE_CONFIGS_XML_ROOT);
    const string xmlChildResourceConfig(RESOURCE_CONFIGS_XML_CHILD_CONFIG);
    const string fTargetResourcesName(TARGETRESOURCE_CONFIGS_XML);
    const string fTargetSysNodesName(TARGETRE_SYSNODES_CONFIGS_XML);
    int32_t idnum;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, TargetResourcesCB, NULL);
    xmlParser->Parse(fTargetResourcesName);
    xmlParser->DeRegister(idnum);

    const string xmlConfigsRoot(BOOSTS_CONFIGS_XML_ROOT);
    const string xmlChildConfigs(BOOSTS_CONFIGS_XML_CHILD_CONFIG);
    const string fPerfConfigsName(PERF_BOOSTS_CONFIGS_XML);

    //sysnode configs
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, TargetSysNodesCB, NULL);
    xmlParser->Parse(fTargetSysNodesName);
    xmlParser->DeRegister(idnum);

    //perf boost configs
    idnum = xmlParser->Register(xmlConfigsRoot, xmlChildConfigs, BoostConfigsCB, NULL);
    xmlParser->Parse(fPerfConfigsName);
    xmlParser->DeRegister(idnum);

    const string fPowerHintName(POWER_CONFIGS_XML);
    const string xmlPHintRoot(POWER_HINT_XML_ROOT);
    const string xmlPowerChildConfigs(POWER_HINT_XML_CHILD_CONFIG);

    //power boost configs
    idnum = xmlParser->Register(xmlPHintRoot, xmlPowerChildConfigs, BoostConfigsCB, NULL);
    xmlParser->Parse(fPowerHintName);
    xmlParser->DeRegister(idnum);

    delete xmlParser;
    return;
}

void PerfDataStore::BoostParamsMappingsCB(xmlNodePtr node, void *) {
    char *maptype = NULL, *resolution = NULL, *mappings = NULL, *tname = NULL;
    uint8_t mtype = MAP_TYPE_UNKNOWN;
    uint8_t res = MAP_RES_TYPE_ANY;
    int32_t marray[MAX_MAP_TABLE_SIZE];
    uint32_t msize = 0;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    if(!xmlStrcmp(node->name, BAD_CAST PERF_BOOSTS_XML_ATTRIBUTES_TAG)) {
        maptype = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_MAPTYPE_TAG);
        mtype = store->ConvertToEnumMappingType(maptype);

        tname = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_TARGET_TAG);

        resolution = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_RESOLUTION_TAG);
        res = store->ConvertToEnumResolutionType(resolution);

        mappings = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_MAPPINGS_TAG);
        msize = store->ConvertToIntArray(mappings, marray, MAX_MAP_TABLE_SIZE);

        QLOGL(LOG_TAG, QLOG_L1, "Identified maptype %s target %s resolution %s mappings %s in mappings table",
              maptype ? maptype : "NULL",
              tname ? tname : "NULL",
              resolution ? resolution : "NULL",
              mappings ? mappings : "NULL");

        if (mappings != NULL) {
            auto tmp = new(std::nothrow) ParamsMappingInfo(mtype, tname, res, marray, msize);
            if(tmp != NULL)
                store->mBoostParamsMappings.push_back(tmp);
            xmlFree(mappings);
        }
        if (maptype) {
            xmlFree(maptype);
        }
        if (resolution) {
            xmlFree(resolution);
        }
        if (tname) {
            xmlFree(tname);
        }
    }
    return;
}

void PerfDataStore::BoostConfigsCB(xmlNodePtr node, void *) {
    char *idPtr = NULL, *resourcesPtr = NULL, *enPtr = NULL, *tname = NULL;
    char *timeoutPtr = NULL, *targetPtr = NULL, *resPtr = NULL, *fpsPtr = NULL;
    char *kernel = NULL, *divergentPtr = NULL, *ignorePtr = NULL;
    int32_t idnum = -1, type = -1, timeout = -1, fps = 0;
    uint32_t res = 0, divergentNumber = 0;
    bool en = false;

    TargetConfig &tc = TargetConfig::getTargetConfig();
    const char *target_name = tc.getTargetName().c_str();
    const char *KernelVer = tc.getFullKernelVersion().c_str();
    uint32_t tc_resolution = tc.getResolution();
    uint32_t target_divergentNumber = tc.getDivergentNumber();
    bool valid_kernel = true, valid_resolution = true, valid_target_name = true;
    bool valid_divergent = true;

   PerfDataStore *store = PerfDataStore::getPerfDataStore();

    if(!xmlStrcmp(node->name, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_CONFIG_TAG)) {
        if(xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_ID_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_ID_TAG);
            if (NULL != idPtr) {
                idnum = strtol(idPtr, NULL, 0);
                xmlFree(idPtr);
            }
        }

        if(xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_TYPE_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_TYPE_TAG);
            if (NULL != idPtr) {
                type = strtol(idPtr, NULL, 0);
                xmlFree(idPtr);
            }
        }

        if(xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_ENABLE_TAG)) {
            enPtr = (char *) xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_ENABLE_TAG);
            if (NULL != enPtr) {
                en = (0 == strncmp(enPtr, "true", strlen(enPtr)));
                xmlFree(enPtr);
            }
        }

        if(xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_TIMEOUT_TAG)) {
            timeoutPtr = (char *) xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_TIMEOUT_TAG);
            if (NULL != timeoutPtr) {
                timeout = strtol(timeoutPtr, NULL, 0);
                xmlFree(timeoutPtr);
            }
        }

        if(xmlHasProp(node, BAD_CAST PERF_BOOSTS_XML_TARGET_TAG)) {
            tname = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_TARGET_TAG);
            if (tname != NULL) {
                valid_target_name = false;
                char *pos = NULL;
                char *tname_token = strtok_r(tname, ",", &pos);
                while(tname_token != NULL) {
                    if((strlen(tname_token) == strlen(target_name)) and (!strncmp(target_name, tname_token,strlen(target_name)))) {
                        valid_target_name = true;
                        break;
                    }
                    tname_token = strtok_r(NULL, ",", &pos);
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_BOOSTS_XML_RESOLUTION_TAG)) {
            resPtr = (char *) xmlGetProp(node, BAD_CAST PERF_BOOSTS_XML_RESOLUTION_TAG);
            if (resPtr != NULL) {
                res = store->ConvertToEnumResolutionType(resPtr);
                if (res == tc_resolution) {
                    valid_resolution = true;
                } else {
                    valid_resolution = false;
                }
            }
        }
        int32_t supportedFPSArray[FPS_CONFIG_COUNT] = {0,};
        int32_t avaiableFPS = 0;
        if (xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_FPS_TAG)) {
            fpsPtr = (char *)xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_FPS_TAG);
            if (NULL != fpsPtr) {
                avaiableFPS = ConvertToIntArray(fpsPtr, supportedFPSArray,
                    FPS_CONFIG_COUNT);
                xmlFree(fpsPtr);
            }
        }

        if (xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_DIVERGENT_TAG)) {
            divergentPtr = (char *)xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_DIVERGENT_TAG);
            if (NULL != divergentPtr) {
                divergentNumber = strtoul(divergentPtr, NULL, 0);
                if (divergentNumber != target_divergentNumber) {
                    valid_divergent = false;
                }
                else {
                    valid_divergent = true;
                }
            }
        }

        if(xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_RESOURCES_TAG)) {
            resourcesPtr = (char *) xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_RESOURCES_TAG);
        }
        if (xmlHasProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_KERNEL_TAG)) {
            kernel = (char *)xmlGetProp(node, BAD_CAST BOOSTS_CONFIGS_XML_ELEM_KERNEL_TAG);
        }

        if (KernelVer && kernel) {
           if (!strncmp(KernelVer, kernel, strlen(kernel))) {
               valid_kernel = true;
            } else
               valid_kernel = false;
        }

        if (valid_kernel and valid_resolution and valid_target_name and resourcesPtr != NULL and en) {

            QLOGL(LOG_TAG, QLOG_L1, "Identified id=0x%" PRId32 " type=%" PRId32 " enable=%" PRId8 " timeout=%" PRId32 " fps=%" PRId32 " target=%s resolution=%s config=%s in table",
              idnum, type, en, timeout, fps,
              target_name ? target_name : "NULL",
              resPtr ? resPtr : "NULL",
              resourcesPtr ? resourcesPtr : "NULL");
            if (avaiableFPS > 0 &&
                (avaiableFPS <= (int32_t)(sizeof(supportedFPSArray)/sizeof(supportedFPSArray[0])))) {
                auto obj = BoostConfigInfo(idnum, type, en, timeout,
                                               supportedFPSArray[0], tname, res, resourcesPtr);
                obj.mDivergentNumber = divergentNumber;
                if (valid_divergent)
                    obj.mUseDivergentConfig = true;
                for(int32_t fpsIdx = 0; fpsIdx < avaiableFPS; fpsIdx++) {
                    obj.mFps = supportedFPSArray[fpsIdx];
                    assignBCIObjs(store, obj, idnum, type);
                }
            } else {
                auto obj = BoostConfigInfo(idnum, type, en, timeout,
                                                fps, tname, res, resourcesPtr);
                obj.mDivergentNumber = divergentNumber;
                if (valid_divergent)
                    obj.mUseDivergentConfig = true;

                assignBCIObjs(store, obj, idnum, type);
            }

            ignorePtr = (char *) xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_IGNORE_HINTS_TAG);
            if (ignorePtr != NULL) {
                QLOGL(LOG_TAG, QLOG_L1, "id=%x type=%" PRId32 " ignore=%s in table",
                                         idnum, type, ignorePtr);
                setIgnoreHints(store, idnum, type, ignorePtr);
                xmlFree(ignorePtr);
            }
        } else if(!valid_kernel){
            QLOGL(LOG_TAG, QLOG_WARNING, "Kernel Not Found");
        }

        if (resourcesPtr != NULL) {
            xmlFree(resourcesPtr);
        }

        if (kernel != NULL)
           xmlFree(kernel);
        //did not release these only, now copied in boostconfig, you can release now
        if (resPtr) {
            xmlFree(resPtr);
        }
        if (tname) {
            xmlFree(tname);
        }
        if (divergentPtr) {
            xmlFree(divergentPtr);
        }
    }
    return;
}

/*For the context Hint<id, type>, add the Ignore hints list*/
void PerfDataStore::setIgnoreHints(PerfDataStore *store, int32_t id, int32_t type, char* Hints) {
    uint32_t size = 0;
    int32_t HintArr[MAX_IGNORE_HINTS];
    if (!store) {
        return;
    }
    size = ConvertToIntArray(Hints, HintArr, MAX_IGNORE_HINTS);
    for (uint32_t i = 0; i< size; i++) {
        store->mContextHints[id][type].push_back(HintArr[i]);
    }
}

/*For the context Hint<id, type>, return the Ignore hints list if any*/
bool PerfDataStore::getIgnoreHints(int32_t id, int32_t type, vector<int32_t> &hints) {
    bool found = false;
    if (mContextHints.find(id) != mContextHints.end() &&
        mContextHints[id].find(type) != mContextHints[id].end()) {
        hints = mContextHints[id][type];
        found = true;
    }
    return found;
}

void PerfDataStore::assignBCIObjs(PerfDataStore *store, BoostConfigInfo &obj,
                                    int32_t idnum, int32_t type) {
    if (!store) {
        return;
    }
    //vendor perf hint
    if (idnum > VENDOR_HINT_START && idnum < VENDOR_PERF_HINT_END) {
        store->mBoostConfigs[idnum][type].push_back(obj);
    } else {
        //power hint
        store->mPowerHint[idnum][type].push_back(obj);
    }
}

void PerfDataStore::CommonSysNodesCB(xmlNodePtr node, void *) {
    int32_t idx_value = -1;
    char *nodePath = NULL;
    char *supportedPtr = NULL;
    bool supported = true;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    //Parsing the SysNode tag.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_SYSNODE_TAG)) {
        // Parsing Idx
        if (!xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG)) {
            return;
        }
        idx_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG, idx_value);

        // Parsing Node
        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG)) {
            nodePath = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG)) {
            supportedPtr = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG);
            if (NULL != supportedPtr) {
                if (strncasecmp("no", supportedPtr, strlen(supportedPtr))== 0) {
                    supported = false;
                }
                xmlFree(supportedPtr);
            }
        }

        if (nodePath != NULL) {
            QLOGL(LOG_TAG, QLOG_L1, "Identified resource with index_value=%" PRId32 " node=%s supported=%" PRId8, idx_value, nodePath, supported);
            auto tmp = new(std::nothrow) ResourceConfigInfo(idx_value, nodePath, supported);
            if (tmp != NULL)
                store->mSysNodesConfig.push_back(tmp);
            xmlFree(nodePath);
        }
    }
    return;
}

void PerfDataStore::TargetSysNodesCB(xmlNodePtr node, void *) {
    int32_t idx_value = -1;
    char *nodePath = NULL;
    char *supportedPtr = NULL;
    bool supported = true;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    //Parsing the SysNode tag.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_SYSNODE_TAG)) {
        // Parsing Idx
        if (!xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG)) {
            return;
        }
        idx_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG, idx_value);

        // Parsing Node
        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG)) {
            nodePath = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG)) {
            supportedPtr = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG);
            if (NULL != supportedPtr) {
                if (strncasecmp("no", supportedPtr, strlen(supportedPtr))== 0) {
                    supported = false;
                }
                xmlFree(supportedPtr);
            }
        }

        if (nodePath != NULL) {
            QLOGE(LOG_TAG, "Identified resource with index_value=%" PRId32 " node=%s supported=%" PRId8, idx_value, nodePath, supported);
        }
        UpdateSysNodeConfig(idx_value, nodePath, supported);
        if (nodePath != NULL) {
            xmlFree(nodePath);
        }
    }
    return;
}

int32_t PerfDataStore::GetSysNode(const int32_t idx_value, char *node_path) {
    bool supported = false;
    return GetSysNode(idx_value, node_path, supported);
}

int32_t PerfDataStore::GetSysNode(const int32_t idx_value, char *node_path, bool &supported) {
    vector<ResourceConfigInfo*>::iterator itbegin = mSysNodesConfig.begin();
    vector<ResourceConfigInfo*>::iterator itend = mSysNodesConfig.end();

    if (node_path == NULL) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Incoming string pointer is NULL");
        return FAILED;
    }
    for (vector<ResourceConfigInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((*it != NULL) and (*it)->mResId == idx_value) {
            strlcpy(node_path, (*it)->mNodePath, NODE_MAX);
            supported = (*it)->mSupported;
            QLOGL(LOG_TAG, QLOG_L2, "SysNode with index %" PRId32 " present sysnode %s", idx_value, node_path);
            return SUCCESS;
        }
    }
    QLOGE(LOG_TAG, "SysNode with index %" PRId32 " not present", idx_value);

    return FAILED;
}


/*Parses the common resource config XML file and stores the node path and supported field
for all valid resource whose indices are calculated based on major and minor values.*/
void PerfDataStore::CommonResourcesCB(xmlNodePtr node, void *prev_major) {
    int32_t minor_value = -1, qindx = -1;
    char *idPtr = NULL, *rsrcPath = NULL;
    int32_t *major_value = NULL;
    bool supported = true;
    ResourceInfo tmpr;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    if(NULL == prev_major) {
        QLOGE(LOG_TAG, "Initialization of Major value Failed");
        return;
    }

    major_value = (int*)prev_major;
    //Parsing the Major tag for the groups major value.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_MAJORCONFIG_TAG)) {
        *major_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG, *major_value);
        QLOGL(LOG_TAG, QLOG_L1, "Identified major resource with Opcode value = %" PRId32, *major_value);
    }

    //Parsing the Minor tag for resource noe path and it's minor value.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_MINORCONFIG_TAG)) {
        minor_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG, minor_value);

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG)) {
            rsrcPath = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG);
            if (NULL != idPtr) {
                if (strncasecmp("no",idPtr,strlen(idPtr))== 0) {
                    supported = false;
                }
                xmlFree(idPtr);
            }
        }

        //Ensuring both the major and minor values are initialized and are in the ranges.
        if ((*major_value == -1) || (*major_value >= MAX_MAJOR_RESOURCES) || (minor_value == -1)) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Major=%" PRId32 " or Minor=%" PRId32 " values are incorrectly Mentioned in %s", *major_value,
                                        minor_value, COMMONRESOURCE_CONFIGS_XML);
            return;
        }
        tmpr.SetMajor(*major_value);
        tmpr.SetMinor(minor_value);
        qindx = tmpr.DiscoverQueueIndex();
        if (rsrcPath != NULL) {
            QLOGL(LOG_TAG, QLOG_L1, "Identified resource with index_value=%" PRId32 " node=%s supported=%" PRId8, qindx, rsrcPath, supported);
            auto tmp = new(std::nothrow) ResourceConfigInfo(qindx, rsrcPath, supported);
            if (tmp != NULL)
                store->mResourceConfig.push_back(tmp);
            xmlFree(rsrcPath);
        }
    }
    return;
}

/*Parses the Target specific resource config XML file and updates mResourceConfig for
all the valid entries. A valid entry needs to have both Major and Minor values, followed
by either node path or supported field.*/
void PerfDataStore::TargetResourcesCB(xmlNodePtr node, void *) {
    int32_t minor_value = -1, major_value = -1, qindx = -1;
    char *idPtr = NULL, *rsrcPath = NULL, *kernelVer = NULL, *target =  NULL;
    bool supported = true, valid_target = true, valid_kernel = true;
    ResourceInfo tmpr;
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();

    const char *kernel = tc.getFullKernelVersion().c_str();
    const char *tname = tc.getTargetName().c_str();
    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    //Parsing the only supported configs tag.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_CONFIG_TAG)) {
        major_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_MAJORVALUE_TAG, major_value);
        minor_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_MINORVALUE_TAG, minor_value);

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG)) {
            rsrcPath = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_NODE_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_KERNEL_TAG)) {
            kernelVer = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_KERNEL_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG)) {
            target = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_SUPPORTED_TAG);
            if (NULL != idPtr) {
                if (strncasecmp("no",idPtr,strlen(idPtr))== 0) {
                    supported = false;
                }
                xmlFree(idPtr);
            }
        }

        //Ensuring both the major and minor values are initialized and are in the ranges.
        if ((major_value == -1) || (major_value >= MAX_MAJOR_RESOURCES) || (minor_value == -1)) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Major=%" PRId32 " or Minor=%" PRId32 " values are incorrectly Mentioned in %s", major_value,
                                        minor_value, TARGETRESOURCE_CONFIGS_XML);
            return;
        }

        if (kernelVer && kernel) {
           if (!strncmp(kernel, kernelVer, strlen(kernelVer))) {
               valid_kernel = true;
           } else
               valid_kernel = false;
        }

        if (target && tname) {
            valid_target = false;
            char *pos = NULL;
            char *tname_token = strtok_r(target, ",", &pos);
            while(tname_token != NULL) {
                if((strlen(tname_token) == strlen(tname)) and (!strncmp(tname, tname_token,strlen(tname)))) {
                    valid_target = true;
                    break;
                }
                tname_token = strtok_r(NULL, ",", &pos);
            }
        }

        tmpr.SetMajor(major_value);
        tmpr.SetMinor(minor_value);
        qindx = tmpr.DiscoverQueueIndex();

        if (!tc.getIsDefaultDivergent() && !tc.isGpuEnabled()) {
            if (qindx > GPU_START_INDEX and qindx < MAX_GPU_MINOR_OPCODE) {
                supported = false;
            }
        }

        if (rsrcPath == NULL)
            QLOGL(LOG_TAG, QLOG_L2, "Identified resource with index_value=%" PRId32 " to update supported=%" PRId8, qindx, supported);
        else
            QLOGL(LOG_TAG, QLOG_L2, "Identified resource with index_value=%" PRId32 " to update with node=%s supported=%" PRId8,
                    qindx, rsrcPath, supported);

        if (valid_kernel && valid_target) {
            UpdateResourceConfig(qindx, rsrcPath, supported);
        }

        if (kernelVer != NULL)
            xmlFree(kernelVer);
        if (target != NULL)
            xmlFree(target);
        if(rsrcPath != NULL)
            xmlFree(rsrcPath);
    }
    return;
}

PerfDataStore::ValueMapType
PerfDataStore::ConvertToEnumMappingType(char *maptype) {
    ValueMapType ret = MAP_TYPE_UNKNOWN;

    if (NULL == maptype) {
        return ret;
    }

    switch(maptype[0]) {
    case 'f':
        if (!strncmp(maptype, MAP_TYPE_VAL_FREQ, strlen(MAP_TYPE_VAL_FREQ))) {
            ret = MAP_TYPE_FREQ;
        }
        break;
    case 'c':
        if (!strncmp(maptype, MAP_TYPE_VAL_CLUSTER, strlen(MAP_TYPE_VAL_CLUSTER))) {
            ret = MAP_TYPE_CLUSTER;
        }
        break;
    }
    return ret;
}

ValueMapResType
PerfDataStore::ConvertToEnumResolutionType(char *res) {
    ValueMapResType ret = MAP_RES_TYPE_ANY;

    if (NULL == res) {
        return ret;
    }

    switch(res[0]) {
    case '1':
        if (!strncmp(res, MAP_RES_TYPE_VAL_1080p, strlen(MAP_RES_TYPE_VAL_1080p))) {
            ret = MAP_RES_TYPE_1080P;
        }
        break;
    case '7':
        if (!strncmp(res, MAP_RES_TYPE_VAL_720p, strlen(MAP_RES_TYPE_VAL_720p))) {
            ret = MAP_RES_TYPE_720P;
        }
        break;
    case 'H':  //Denotes HD_PLUS Resolution ( 720x1440)
        if (!strncmp(res, MAP_RES_TYPE_VAL_HD_PLUS, strlen(MAP_RES_TYPE_VAL_HD_PLUS))) {
            ret = MAP_RES_TYPE_HD_PLUS;
        }
        break;
    case '2':
        if (!strncmp(res, MAP_RES_TYPE_VAL_2560, strlen(MAP_RES_TYPE_VAL_2560))) {
            ret = MAP_RES_TYPE_2560;
        }
    }
    return ret;
}

uint32_t PerfDataStore::ConvertToIntArray(char *str, int32_t intArray[], uint32_t size) {
    uint32_t i = 0;
    char *endPtr;

    if ((NULL == str) || (NULL == intArray)) {
        return i;
    }

    char *pos = NULL;
    char *token = strtok_r(str, ", ", &pos);
    while(token != NULL && i < size) {
        intArray[i] = strtol(token, &endPtr, 0);
        if (*endPtr != '\0') {
            QLOGE(LOG_TAG, "Invalid value found in strtol : %s", token);
        }
        i++;
        token = strtok_r(NULL, ", ", &pos);
    }
    return i;
}

long int PerfDataStore::ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue) {
    /* For a given XML node pointer, checks the presence of tag.
       If tag present, returns the converted numeric of tags value.
       else returns the default value of that tag.
    */
    char *idPtr = NULL;

    if (xmlHasProp(node, BAD_CAST tag)) {
        idPtr = (char *)xmlGetProp(node, BAD_CAST tag);
        if (NULL != idPtr) {
            defaultvalue = strtol(idPtr, NULL, 0);
            xmlFree(idPtr);
        }
    }
    return defaultvalue;
}

uint32_t PerfDataStore::GetFreqMap(uint32_t res, int32_t **maparray, const char *tname) {
    uint32_t mapsize = 0;

    if (!maparray || !tname) {
        return mapsize;
    }

    vector<ParamsMappingInfo*>::iterator itbegin = mBoostParamsMappings.begin();
    vector<ParamsMappingInfo*>::iterator itend = mBoostParamsMappings.end();

    for (vector<ParamsMappingInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((NULL != *it) && ((*it)->mMapType == MAP_TYPE_FREQ) && ((*it)->mResolution == res) && !strncmp((*it)->mTName, tname, strlen(tname))) {
                mapsize = (*it)->mMapSize;
                *maparray = (*it)->mMapTable;
        }
    }
    return mapsize;
}

uint32_t PerfDataStore::GetClusterMap(int32_t **maparray, const char *tname) {
    uint32_t mapsize = 0;

    if (!maparray || !tname) {
        return mapsize;
    }

    vector<ParamsMappingInfo*>::iterator itbegin = mBoostParamsMappings.begin();
    vector<ParamsMappingInfo*>::iterator itend = mBoostParamsMappings.end();

    for (vector<ParamsMappingInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((NULL != *it) && (*it)->mMapType == MAP_TYPE_CLUSTER && !strncmp((*it)->mTName, tname, strlen(tname))) {
                *maparray = (*it)->mMapTable;
                mapsize = MAX_MAP_TABLE_SIZE;
        }
    }
    return mapsize;
}

uint32_t PerfDataStore::GetAllBoostHintType(vector<pair<int32_t, pair<int32_t,uint32_t>>> &hints_list) {
    uint32_t count = 0;
    try {
        for (auto &i : mBoostConfigs) {
            for (auto &j : i.second) {
                for (auto &k : j.second) {
                    hints_list.push_back({i.first, {j.first,k.mFps}});
                    count++;
                }
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return count;
}

uint32_t PerfDataStore::GetBoostConfig(int32_t hintId, int32_t type, int32_t *mapArray,
                                        int32_t *timeout, const char *, uint32_t res,
                                        int32_t fps) {
    uint32_t mapsize = 0;

    if (!mapArray) {
        return mapsize;
    }

    vector<BoostConfigInfo>::iterator itbegin;
    vector<BoostConfigInfo>::iterator itend;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    bool init_iters = false;
    uint32_t divergentNumber = tc.getDivergentNumber();

    //vendor perf hint
    if (hintId > VENDOR_HINT_START && hintId < VENDOR_PERF_HINT_END) {
        if (mBoostConfigs.find(hintId) != mBoostConfigs.end() && mBoostConfigs[hintId].find(type) != mBoostConfigs[hintId].end()) {
            itbegin = mBoostConfigs[hintId][type].begin();
            itend = mBoostConfigs[hintId][type].end();
            init_iters = true;
        }
    } else {
        //power hint
        if (mPowerHint.find(hintId) != mPowerHint.end() && mPowerHint[hintId].find(type) != mPowerHint[hintId].end()) {
            itbegin = mPowerHint[hintId][type].begin();
            itend = mPowerHint[hintId][type].end();
            init_iters = true;
        }
    }

    if (!init_iters) {
        return mapsize;
    }

    bool useDivergentConfig = false;
    for (auto it = itbegin; it != itend; ++it) {
        if (it->mUseDivergentConfig && it->mDivergentNumber == divergentNumber) {
            useDivergentConfig = true;
            break;
        }
    }

    for (vector<BoostConfigInfo>::iterator it = itbegin; it != itend; ++it) {
        if ((((*it).mResolution == MAP_RES_TYPE_ANY) || (*it).mResolution == res) &&
                ((*it).mFps == 0 || (*it).mFps == fps)) {

            if (useDivergentConfig && it->mDivergentNumber != divergentNumber)
                continue;

            mapsize = (*it).mConfigsSize;
            for (uint32_t i=0; i<mapsize; i++) {
                mapArray[i] = (*it).mConfigTable[i];
            }
            if (timeout) {
                *timeout = (*it).mTimeout;
            }
            if ((*it).mFps != 0)
                break;
        }
    }
    return mapsize;
}

/* If resId's information is present in resource configs XML, we retrive it's
node path and supported field. Once we retrieve this nodes information in OptsData
it can be erased from PerfDataStore */
int32_t PerfDataStore::GetResourceConfigNode(int32_t resId, char *node, bool &supported) {
    vector<ResourceConfigInfo*>::iterator itbegin = mResourceConfig.begin();
    vector<ResourceConfigInfo*>::iterator itend = mResourceConfig.end();

    for (vector<ResourceConfigInfo*>::iterator it = itbegin; it != itend; ) {
        if ((NULL != *it) && ((*it)->mResId == resId)) {
            strlcpy(node, (*it)->mNodePath, NODE_MAX);
            supported = (*it)->mSupported;
            it = mResourceConfig.erase(it);
            return SUCCESS;
        } else {
             ++it;
        }
    }
    return FAILED;
}

/* If resId present in ResourceConfig, we update it's information with target
specific ones, metioned in target resource Xml file.*/
void PerfDataStore::UpdateResourceConfig(int resId, const char *node, bool supported) {
    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    UpdateConfig(store->mResourceConfig, resId, node, supported);
}

void PerfDataStore::UpdateSysNodeConfig(int resId, const char *node, bool supported) {
    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    UpdateConfig(store->mSysNodesConfig, resId, node, supported);
}

void PerfDataStore::UpdateConfig(vector<ResourceConfigInfo*> &config, int32_t resId, const char *node, bool supported) {
    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    vector<ResourceConfigInfo*>::iterator itbegin = config.begin();
    vector<ResourceConfigInfo*>::iterator itend = config.end();

    for (vector<ResourceConfigInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((NULL != *it) && ((*it)->mResId == resId)) {
            //Ensuring mNodePath is valid when copying.
            if((node != NULL) && (strlen(node) > 0)) {
                strlcpy((*it)->mNodePath, node, NODE_MAX);
            }
            (*it)->mSupported = supported;
            return;
        }
    }
    QLOGL(LOG_TAG, QLOG_WARNING, "Unable to find resource index=%" PRId32 ", to update.", resId);
}
