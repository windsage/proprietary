/******************************************************************************
  @file    TargetConfig.cpp
  @brief   Implementation of TargetConfig

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGETCONFIG"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>
#include <sys/utsname.h>
#include "TargetConfig.h"
#include <PerfController.h>
#include "PerfLog.h"
#include "SecureOperations.h"

#define FAILED -1
TargetConfig TargetConfig::cur_target;

//resolution nodes
#define RESOLUTION_DRM_NODE "/sys/class/drm/card0-DSI-1/modes"
#define RESOLUTION_GRAPHICS_NODE "/sys/class/graphics/fb0/modes"

//targetconfig xml
#define TARGET_CONFIGS_XML (VENDOR_DIR"/perf/targetconfig.xml")
#define KERNEL_VERSION_NODE  "/proc/sys/kernel/osrelease"

//target config tags
#define TARGET_CONFIGS_XML_ROOT "TargetConfig"
#define TARGET_CONFIGS_XML_CHILD_CONFIG "Config%d"
#define TARGET_CONFIGS_XML_ELEM_TARGETINFO_TAG "TargetInfo"
#define TARGET_CONFIGS_XML_ELEM_NUMCLUSTERS_TAG "NumClusters"
#define TARGET_CONFIGS_XML_ELEM_TOTALNUMCORES_TAG "TotalNumCores"
#define TARGET_CONFIGS_XML_ELEM_CORECTLCPU_TAG "CoreCtlCpu"
#define TARGET_CONFIGS_XML_ELEM_SYNCORE_TAG "SynCore"
#define TARGET_CONFIGS_XML_ELEM_MINCOREONLINE_TAG "MinCoreOnline"
#define TARGET_CONFIGS_XML_ELEM_GOVINSTANCETYPE_TAG "GovInstanceType"
#define TARGET_CONFIGS_XML_ELEM_CPUFREQGOV_TAG "CpufreqGov"
#define TARGET_CONFIGS_XML_ELEM_TARGET_TAG "Target"
#define TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG "SocIds"
#define TARGET_CONFIGS_XML_ELEM_CLUSTER_TAG "ClustersInfo"
#define TARGET_CONFIGS_XML_ELEM_ID_TAG "Id"
#define TARGET_CONFIGS_XML_ELEM_NUMCORES_TAG "NumCores"
#define TARGET_CONFIGS_XML_ELEM_TYPE_TAG "Type"
#define TARGET_CONFIGS_XML_ELEM_MAXFREQUENCY_TAG "MaxFrequency"
#define TARGET_CONFIGS_XML_ELEM_MINFPSTUNING_TAG "MinFpsForTuning"
#define TARGET_CONFIGS_XML_ELEM_CAPPEDMAXFREQ_TAG "CappedMaxFreq"
#define TARGET_CONFIGS_XML_ELEM_TARGETMAXARGSPERREQ_TAG "TargetMaxArgsPerReq"


/*
 * Definition for various get functions
 * */
int8_t TargetConfig::getCoresInCluster(int8_t cluster) {
    if (mCorePerCluster) {
        if ((cluster >= 0) && (cluster < mNumCluster)) {
            int8_t tmp = mCorePerCluster[cluster];
            if (tmp <= mTotalNumCores) {
                return tmp;
            }
        } else {
            QLOGE(LOG_TAG, "Error: Invalid cluster id %" PRId8,cluster);
        }
    } else {
        QLOGE(LOG_TAG, "Error: TargetConfig not initialized");
    }
    return FAILED;
}

/* Returns the value to be written on cpu max freq
   sysfs node after perflock release.
*/
uint32_t TargetConfig::getCpuMaxFreqResetVal(int8_t cluster) {
    if (cluster >= 0 && cluster < MAX_CLUSTER) {
        return mCpuMaxFreqResetVal[cluster];
    }
    return FAILED;
}

/* Returns the value to be written on cpu max freq
   sysfs node if 0xFFFF val is passed.
*/
uint32_t TargetConfig::getCpuCappedMaxFreqVal(int8_t cluster) {
    if (cluster >= 0 && cluster < MAX_CLUSTER) {
        return mCpuCappedMaxfreqVal[cluster];
    }
    return FAILED;
}

TargetConfig::TargetConfig() {
    QLOGV(LOG_TAG, "TargetConfig constructor");
    mSyncCore = true;
    mNumCluster = 0;
    mTotalNumCores = 0;
    mCorePerCluster = NULL;
    mCoreCtlCpu = -1;
    mMinCoreOnline = 0;
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;
    mCpufreqGov = INTERACTIVE;
    mType = 0;
    mRam = -1;
    mRamKB = -1;
    mMinFpsTuning = 0;
    mFirstAPILevel = S_API_LEVEL;
    mTargetMaxArgsPerReq = MAX_ARGS_PER_REQUEST;
    mDisplayEnabled = true;
    mGpuEnabled = true;
    mInitCompleted.store(false);
    /* Setting reset value to INT_MAX to avoid race condition with thermal engine.
     * INT_MAX work as vote removed from perfd side */
    for (uint8_t i = 0; i < MAX_CLUSTER; i++)
         mCpuMaxFreqResetVal[i] = INT_MAX;
    mResolution = MAP_RES_TYPE_ANY;
    QLOGV(LOG_TAG, "TargetConfig constructor exit");
}

void TargetConfig::setInitCompleted(bool flag) {
    mInitCompleted.store(flag);
    QLOGV(LOG_TAG, "Setting prop %s : %" PRId8, MPCTL_INIT_COMPLETE, flag);
    /* MPCTL_INIT_COMPLETE this property is defined in sepolicy rules
     * from vendor T onwards, if this property is not set only WLC
     * will be impacted. */
    if (flag == true && mFirstAPILevel > S_API_LEVEL) {
        property_set(MPCTL_INIT_COMPLETE,"1");
    }
}

TargetConfig::~TargetConfig() {
    QLOGV(LOG_TAG, "TargetConfig destructor");
    if (mCorePerCluster) {
        delete [] mCorePerCluster;
        mCorePerCluster = NULL;
    }

    //delete target configs
    while (!mTargetConfigs.empty()) {
        TargetConfigInfo *tmp = mTargetConfigs.back();
        if (tmp) {
            delete tmp;
        }
        mTargetConfigs.pop_back();
    }
}

int32_t TargetConfig::readRamSize(void) {
    FILE *memInfo = fopen("/proc/meminfo", "r");
    int32_t ram = -1;
    if (memInfo == NULL) {
        return -1;
    }
    char line[256];
    while (fgets(line, sizeof(line), memInfo)) {
        int32_t memTotal = -1;
        char* end = NULL;
        char* token = strtok_r(line, ":", &end);
        if ((end != NULL) && (token != NULL) && (strncmp(token, "MemTotal", 8) == 0)) {
            token = strtok_r(NULL, " ", &end);
            if ((end != NULL) && (token != NULL)) {
                memTotal = strtol(token, &end, 10);
                mRamKB = memTotal;
                if (memTotal < MEM_1GB) {
                    ram = RAM_1GB;
                } else if (memTotal < MEM_2GB) {
                    ram = RAM_2GB;
                } else if (memTotal < MEM_3GB) {
                    ram = RAM_3GB;
                } else if (memTotal < MEM_4GB) {
                    ram = RAM_4GB;
                } else if (memTotal < MEM_6GB) {
                    ram = RAM_6GB;
                } else if (memTotal < MEM_8GB) {
                    ram = RAM_8GB;
                } else if (memTotal < MEM_10GB) {
                    ram = RAM_10GB;
                } else {
                    ram = RAM_12GB;
                }
                if(ram != -1) {
                    break;
                }
            }
        }
    }
    fclose(memInfo);
    return ram;
}

int16_t TargetConfig::readSocID() {
    int8_t fd;
    int16_t soc = -1;
    if (!access("/sys/devices/soc0/soc_id", F_OK)) {
        fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
    } else {
        fd = open("/sys/devices/system/soc/soc0/id", O_RDONLY);
    }
    if (fd != -1)
    {
        char raw_buf[5];
        read(fd, raw_buf,4);
        raw_buf[4] = 0;
        soc = atoi(raw_buf);
        close(fd);
    }
    mSocID = soc; /* set target socid */
    QLOGL(LOG_TAG, QLOG_L1 , "socid of the device: %" PRId16, soc);
    return soc;
}

void TargetConfig::readKernelVersion(){
    int8_t fd = -1;
    int32_t n;
    int8_t rc = 0;
    const uint8_t MAX_BUF_SIZE = 16;
    char kernel_version_buf[MAX_BUF_SIZE];
    int8_t kernelMajor = -1, kernelMinor = -1;
    mKernelMinorVersion = -1;
    mKernelMajorVersion = -1;
    fd = open(KERNEL_VERSION_NODE, O_RDONLY);
    if (fd != -1) {
        memset(kernel_version_buf, 0x0, sizeof(kernel_version_buf));
        n = read(fd, kernel_version_buf, MAX_BUF_SIZE-1);
        if (n > 0) {
            rc = sscanf(kernel_version_buf, "%" SCNd8".%" SCNd8".", &kernelMajor, &kernelMinor);
            if (rc != 2) {
                mKernelMinorVersion = -1;
                mKernelMajorVersion = -1;
                QLOGE(LOG_TAG, "sscanf failed, kernel version set to -1");
            } else {
                mKernelMinorVersion = kernelMinor;
                mKernelMajorVersion = kernelMajor;
                snprintf(kernel_version_buf, MAX_BUF_SIZE, "%" PRId8".%" PRId8, mKernelMajorVersion, mKernelMinorVersion);
            }
            mFullKernelVersion = string(kernel_version_buf);
        } else
            QLOGE(LOG_TAG, "Kernel Version node not present");
    close(fd);
    }
}

void TargetConfig::readVariant() {
    char target_name_var[PROPERTY_VALUE_MAX];
    if (property_get("ro.product.name", target_name_var, "Undefined") > 0) {
        target_name_var[PROPERTY_VALUE_MAX-1]='\0';
    }
    mVariant = string(target_name_var);
}

void TargetConfig::updateClusterNameToIdMap(uint8_t cluster) {
    if (!isDefaultDivergent && !divergentConf.checkClusterPresent(cluster)) {
        setUnsupportedPhysicalCluster(cluster);
    }
}

void TargetConfig::setUnsupportedPhysicalCluster(uint8_t cluster) {
    mClusterNameToIdMap[getClusterType(cluster)] = -1;
}

bool TargetConfig::isClusterSupported(uint8_t cluster) {
    return mClusterNameToIdMap[getClusterType(cluster)] != -1;
}

std::string TargetConfig::getClusterType(uint8_t cluster) {
    for (auto it = mClusterNameToIdMap.begin(); it != mClusterNameToIdMap.end(); ++it) {
        if (it->second == cluster) {
            return it->first;
        }
    }
    return "";
}

/* A single InitializeTargetConfig function for all the targets which
   parses XML file for target configs.
 * For adding a new TargetConfig, create a targetconfig.xml file.
*/


/* Init Sequence
 * 1. Pouplate from targetconfig.xml
 * 2. Init the target
 */
void TargetConfig::InitializeTargetConfig() {
    int16_t socid = 0;
    uint32_t res = 0;
    char prop_val[PROPERTY_VALUE_MAX];

    QLOGV(LOG_TAG, "Inside InitializeTargetConfig");

    socid = readSocID();
    mRam = readRamSize();
    readVariant();

    //get resolution
    (void) readResolution();
    res = getResolution();
    /*
     * Initialize kernel version
     */
    readKernelVersion();
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;

    if (property_get(API_LEVEL_PROP_NAME, prop_val, "31")) {
        mFirstAPILevel = atoi(prop_val);
    }

    InitTargetConfigXML();

    TargetConfigInit();
    QLOGL(LOG_TAG, QLOG_L1, "Init complete for: %s", getTargetName().c_str());

    // view stored info
    DumpAll();
}

void TargetConfig::TargetConfigInit() {

    QLOGV(LOG_TAG, "TargetConfigInit start");
    TargetConfigInfo *config = getTargetConfigInfo(getSocID());

    if (NULL == config) {
        QLOGE(LOG_TAG, "Initialization of TargetConfigInfo Object failed");
        return;
    }

    if (!config->mUpdateTargetInfo) {
        QLOGE(LOG_TAG, "Target Initialized with default available values, as mistake in target config XML file");
        return;
    }

    if(config->mCalculatedCores != config->mTotalNumCores) {
        QLOGE(LOG_TAG, "Target Initialized with default values, as mismatch between the TotalNumCores and CalculatedCores.");
        return;
    }

    isDefaultDivergent = CheckDefaultDivergent(config);

    mTargetName = string(config->mTargetName);
    QLOGL(LOG_TAG, QLOG_L1, "Init %s",!mTargetName.empty() ? mTargetName.c_str() : "Target");

    if(config->mTargetMaxArgsPerReq != 0) {
        mTargetMaxArgsPerReq = config->mTargetMaxArgsPerReq;
        if(mTargetMaxArgsPerReq % 2 != 0 || mTargetMaxArgsPerReq > MAX_ARGS_PER_REQUEST_LIMIT) {
            mTargetMaxArgsPerReq = MAX_ARGS_PER_REQUEST;
            QLOGE(LOG_TAG, "TargetMaxArgsPerReq initialized with wrong value in TargetConfig.xml.");
        }
    }

    mClusterNameToIdMap = config->mClusterNameToIdMap;

    mNumCluster = config->mNumCluster;
    mTotalNumCores = config->mTotalNumCores;
    if (mNumCluster > 0) {
        mCorePerCluster = new(std::nothrow) int8_t[mNumCluster];
    } else {
        mCorePerCluster = NULL;
    }
    if (mCorePerCluster) {
        for (uint8_t i=0;i<mNumCluster;i++) {
            mCorePerCluster[i] = determineCoresPerCluster(config, i);
            mCpuMaxFreqResetVal[i] = config->mCpumaxfrequency[i];
            mCpuCappedMaxfreqVal[i] = config->mCpuCappedMaxfreq[i];

            updateClusterNameToIdMap(i);
        }
    } else {
        QLOGE(LOG_TAG, "Error: Could not initialize cores in cluster \n");
    }

    mSyncCore = config->mSyncCore;
    mType = config->mType;
    mType2 = config->mType2;

    if(mCorePerCluster) {
        if (mType == 1) {
            // Cluster 0 is little core.
             mNumBigCores = mTotalNumCores - mCorePerCluster[0];
        } else {
            /* Cluster 1 is little core.
            TODO: Need redesinate if no little core in future. */
            mNumBigCores = mTotalNumCores - mCorePerCluster[1];
        }
    }
    if (config->mCoreCtlCpu < 0 || config->mCoreCtlCpu >= mTotalNumCores) {
        QLOGL(LOG_TAG, QLOG_WARNING,  "CoreCtlCpu is incorrectly specified in XML file, So Initializing to -1");
    } else {
        mCoreCtlCpu = config->mCoreCtlCpu;
    }

    if (config->mMinCoreOnline < 0 || config->mMinCoreOnline > mTotalNumCores) {
        QLOGL(LOG_TAG, QLOG_WARNING,  "MinCoreOnline is incorrectly specified in XML file, So Initializing to 0");
    } else {
        mMinCoreOnline = config->mMinCoreOnline;
    }

    if (config->mGovInstanceType >= MAX_GOVINSTANCETYPE) {
        QLOGL(LOG_TAG, QLOG_WARNING,  "GovInstanceType is incorrectly specified in XML file, So Initializing to CLUSTER_BASED_GOV_INSTANCE");
    } else {
        mGovInstanceType = determineGovernorInstType(config);
    }

    if (config->mCpufreqGov >= MAX_CPUFREQGOV) {
        QLOGL(LOG_TAG, QLOG_WARNING,  "CpufreqGov is incorrectly specified in XML file, So Initializing to INTERACTIVE");
    } else {
        mCpufreqGov = config->mCpufreqGov;
    }

    mMinFpsTuning = config->mMinFpsTuning;

    mDivergentNumber = divergentConf.getDivergentNumber();

    mDisplayEnabled = divergentConf.checkDisplayEnabled();
    mGpuEnabled = divergentConf.checkGpuEnabled();

    /*Deleting target configs vector, after target initialized with the required values
      which are parsed from the XML file. As the vector is not needed anymore.*/
    while (mTargetConfigs.empty()) {
        config = mTargetConfigs.back();
        if (config) {
            delete config;
        }
        mTargetConfigs.pop_back();
    }
    QLOGV(LOG_TAG, "TargetConfigInit end");

}

bool TargetConfig::CheckDefaultDivergent(TargetConfigInfo *config) {
    return config->mTotalNumCores == divergentConf.getTotalNumCores();
}

void TargetConfig::InitTargetConfigXML() {
    const string fTargetConfigName(TARGET_CONFIGS_XML);
    const string xmlTargetConfigRoot(TARGET_CONFIGS_XML_ROOT);
    uint8_t idnum, i;
    char TargetConfigXMLtag[NODE_MAX] = "";
    string xmlChildTargetConfig;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }
    QLOGV(LOG_TAG, "InitTargetConfigXML start");

    //target configs
    /*Multitple targets can share the same platform. So, these multple target configs
      are differentiated with the config index number in the configs tag. Parsing of XML
      file is done by identifying these configs.*/
    for(i = 1; i <= MAX_CONFIGS_SUPPORTED_PER_PLATFORM; i++) {
        snprintf(TargetConfigXMLtag, NODE_MAX, TARGET_CONFIGS_XML_CHILD_CONFIG, i);
        xmlChildTargetConfig = TargetConfigXMLtag;
        idnum = xmlParser->Register(xmlTargetConfigRoot, xmlChildTargetConfig, TargetConfigsCB, &i);
        xmlParser->Parse(fTargetConfigName);
        xmlParser->DeRegister(idnum);
    }
    delete xmlParser;
    QLOGV(LOG_TAG, "InitTargetConfigXML end");

    return;
}

uint32_t TargetConfig::ConvertToIntArray(char *str, int16_t intArray[], uint32_t size) {
    uint32_t i = 0;
    char *pch = NULL;
    char *end = NULL;
    char *endPtr = NULL;

    if ((NULL == str) || (NULL == intArray)) {
        return i;
    }

    end = str + strlen(str);

    do {
        pch = strchr(str, ',');
        intArray[i] = strtol(str, &endPtr, 0);
        i++;
        str = pch;
        if (NULL != pch) {
            str++;
        }
    } while ((NULL != str) && (str < end) && (i < size));

    return i;
}

long int TargetConfig::ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue) {
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

void TargetConfig::TargetConfigsCB(xmlNodePtr node, void *index) {

    char *idPtr = NULL;
    char *tmp_target = NULL;
    int8_t id = 0, core_per_cluster = 0, target_index = 0;
    long int frequency = INT_MAX;
    long int capped_max_freq = INT_MAX;
    bool cluster_type_present = false;

    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (NULL == index) {
        QLOGE(LOG_TAG, "Unable to get the target config index of XML.");
        return;
    }
    target_index = *((int8_t*)index) - 1;
    if((target_index < 0) || (target_index >= MAX_CONFIGS_SUPPORTED_PER_PLATFORM)) {
        QLOGE(LOG_TAG, "Invalid config index value while parsing the XML file.");
        return;
    }


    /*Parsing of a config tag is done by multiple calls to the parsing function, so for
    the first call to the function we create a config object and for all the next calls
    we use the same object. */
    if ((int8_t)tc.mTargetConfigs.size() <= target_index) {
        auto tmp = new(std::nothrow) TargetConfigInfo;
        if (tmp != NULL)
            tc.mTargetConfigs.push_back(tmp);
    }
    TargetConfigInfo *config = tc.mTargetConfigs[target_index];
    if (NULL == config) {
        QLOGE(LOG_TAG, "Initialization of TargetConfigInfo object failed");
        return;
    }

    //Parsing Targetconfig tag which has all the target related information.
    if (!xmlStrcmp(node->name, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGETINFO_TAG)) {
        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGET_TAG)) {
            tmp_target = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGET_TAG);
            if(tmp_target != NULL) {
                config->mTargetName = string(tmp_target);
                xmlFree(tmp_target);
            }
        }

        //third argument defaultvalue is needed to retain the same value in case of any error.
        config->mSyncCore = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_SYNCORE_TAG, config->mSyncCore);
        config->mNumCluster = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_NUMCLUSTERS_TAG, config->mNumCluster);
        config->mTotalNumCores = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_TOTALNUMCORES_TAG, config->mTotalNumCores);
        config->mCoreCtlCpu = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_CORECTLCPU_TAG, config->mCoreCtlCpu);
        config->mMinCoreOnline = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MINCOREONLINE_TAG, config->mMinCoreOnline);
        config->mGovInstanceType = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_GOVINSTANCETYPE_TAG, config->mGovInstanceType);
        config->mCpufreqGov = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_CPUFREQGOV_TAG, config->mCpufreqGov);
        config->mMinFpsTuning = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MINFPSTUNING_TAG, config->mMinFpsTuning);
        config->mTargetMaxArgsPerReq = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_TARGETMAXARGSPERREQ_TAG, config->mTargetMaxArgsPerReq);

        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG);
            if (NULL != idPtr) {
                config->mNumSocids = ConvertToIntArray(idPtr,config->mSupportedSocids, MAX_SUPPORTED_SOCIDS);
                xmlFree(idPtr);
            }
        }

        //checks to ensure the target configs are mentioned correctly in XML file.
        if ((config->mNumCluster <= 0) || (config->mNumCluster > MAX_CLUSTER)) {
            config->mUpdateTargetInfo = false;
            QLOGE(LOG_TAG, "Number of clusters are not mentioned correctly in targetconfig xml file");
        }

        if (config->mTotalNumCores <= 0) {
            config->mUpdateTargetInfo = false;
            QLOGE(LOG_TAG, "Number of cores are not mentioned correctly in targetconfig xml file");
        }

        if (config->mNumSocids <= 0) {
            config->mUpdateTargetInfo = false;
            QLOGE(LOG_TAG, "Supported Socids of the target are not mentioned correctly in targetconfig xml file.");
        }

        QLOGL(LOG_TAG, QLOG_L1, "Identified Target with clusters=%" PRIu8 " cores=%" PRIu8 " core_ctlcpu=%" PRId8 " min_coreonline=%" PRId8 " target_name=%s",
              config->mNumCluster, config->mTotalNumCores, config->mCoreCtlCpu,
              config->mMinCoreOnline, !config->mTargetName.empty() ? config->mTargetName.c_str() : "Not mentioned");
    }

    /* Parsing the clusterinfo tag, which has all the cluster based information.
       Each cluster has a seperate clusterinfo tag. */
    if (!xmlStrcmp(node->name, BAD_CAST TARGET_CONFIGS_XML_ELEM_CLUSTER_TAG)) {
        id = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_ID_TAG, id);
        core_per_cluster = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_NUMCORES_TAG, core_per_cluster);

        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TYPE_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TYPE_TAG);
            if (NULL != idPtr) {
                cluster_type_present = true;

                config->mClusterNameToIdMap[idPtr] = id;

                if (id == 0) {
                    if (strncmp("little",idPtr,strlen(idPtr))== 0) {
                        config->mType = 1;
                    } else {
                        config->mType = 0;
                    }
                }
                if (id == 2) {
                    if (strncmp("titanium",idPtr,strlen(idPtr))== 0) {
                        config->mType2 = 1;
                    } else {
                        config->mType2 = 0;
                    }
                }
                xmlFree(idPtr);
            }
        }

        frequency = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MAXFREQUENCY_TAG, frequency);
        if (frequency <= 0 || frequency > INT_MAX) {
            QLOGL(LOG_TAG, QLOG_WARNING,  "Cluster %" PRId8 " max possible frequency is mentioned incorrectly, assigning UNIT_MAX", id);
            frequency = INT_MAX;
        }

        capped_max_freq = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_CAPPEDMAXFREQ_TAG, capped_max_freq);
        if (capped_max_freq <= 0 || capped_max_freq >= INT_MAX) {
            QLOGL(LOG_TAG, QLOG_WARNING,  "Cluster %" PRId8 " max frequency for capped value is mentioned incorrectly, assigning UNIT_MAX", id);
            capped_max_freq  = INT_MAX;
        } else {
            //updating reset max freq also in case max capped freq is provided
            frequency = capped_max_freq;
            capped_max_freq = SecInt::Divide(capped_max_freq, 1000); //convert to Mhz
        }

        //ensuring the cluster_id is in the range
        if ((id < 0) || (id >= config->mNumCluster) || (id >= MAX_CLUSTER)) {
            config->mUpdateTargetInfo = false;
            QLOGE(LOG_TAG, "Cluster id is not mentioned correctly in targetconfig.xml file, aborting parsing of XML");
            return;
        } else {
            config->mCorepercluster[id] = core_per_cluster;
            config->mCpumaxfrequency[id] = frequency;
            config->mCpuCappedMaxfreq[id] = capped_max_freq;
            config->mCalculatedCores += core_per_cluster;
        }

        //need this check to decide the mapping of logical clusters to physical clusters.
        if ((config->mNumCluster > 1) && (!cluster_type_present)) {
            config->mUpdateTargetInfo = false;
            QLOGE(LOG_TAG, "Number of clusters is more than 1 but the type is not mentioned for cluster id=%" PRId8,id);
        }

        QLOGL(LOG_TAG, QLOG_L1, "Identified CPU cluster with id=%" PRId8 " cores=%" PRId8 " max_freq=%" PRId64 " capped_freq=%" PRId64,
                id, core_per_cluster, (int64_t)frequency, (int64_t)capped_max_freq);
    }

    return;
}

bool TargetConfig::TargetConfigInfo::CheckSocID(int16_t SocId) {
    bool valid_socid = false;
    for (uint32_t i = 0; i < mNumSocids; i++) {
        if (mSupportedSocids[i] == SocId) {
            valid_socid = true;
            break;
        }
    }
    return valid_socid;
}

bool TargetConfig::TargetConfigInfo::CheckClusterInfo() {
    int8_t total_cores = 0;
    for (uint8_t i = 0; i < mNumCluster; i++) {
        total_cores += mCorepercluster[i];
    }
    return total_cores == mTotalNumCores?true:false;
}

/*Depending on the socid of device, we select the target configs from the multiple
configs in the XML file. */
TargetConfig::TargetConfigInfo* TargetConfig::getTargetConfigInfo(int16_t socId) {
    vector<TargetConfigInfo*>::iterator itbegin = mTargetConfigs.begin();
    vector<TargetConfigInfo*>::iterator itend = mTargetConfigs.end();

    for (vector<TargetConfigInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((NULL != *it) && ((*it)->mNumSocids > 0)) {
            if ((*it)->CheckSocID(socId)) {
                if(!(*it)->CheckClusterInfo()) {
                    (*it)->mUpdateTargetInfo = false;
                    QLOGE(LOG_TAG, "Target Info with maching SocId have invalid cluster info");
                }
                return (*it);
            }
        }
    }
    QLOGE(LOG_TAG, "Target Initializes with default values, as Target socid not supported");
    return NULL;
}

uint32_t TargetConfig::readResolution() {
    int8_t fd = -1;
    uint32_t ret = MAP_RES_TYPE_ANY, n = 0;
    int32_t w = 0, h = 0, t = 0;
    char *pch = NULL;
    const uint8_t MAX_BUF_SIZE = 16;

    //use the new node if present to get the resolution.
    if (!access(RESOLUTION_DRM_NODE, F_OK)) {
        fd = open(RESOLUTION_DRM_NODE, O_RDONLY);
    } else {
        fd = open(RESOLUTION_GRAPHICS_NODE, O_RDONLY);
    }

    if (fd != -1) {
        char buf[MAX_BUF_SIZE];
        memset(buf, 0x0, sizeof(buf));
        n = read(fd, buf, MAX_BUF_SIZE-1);
        if (n > 0) {
            //parse U:1080x1920p-60 (or) 1440x2560
            if (isdigit(buf[0])) {
                w = atoi(&buf[0]); //for new node
            } else {
                w = atoi(&buf[2]);//for old node
            }
            pch = strchr(buf, 'x');
            if (NULL != pch) {
                h = atoi(pch+1);
            }

            if (h && (w/h > 0)) {
                t = w;
                w = h;
                h = t;
            }

            //convert resolution to enum
            if (w <= 800 && h <= 1280) {
                ret = MAP_RES_TYPE_720P;
            }
            else if (w <= 800 && h <= 1440) {
                ret = MAP_RES_TYPE_HD_PLUS;
            }
            else if (w <= 1080 && h <= 1920) {
                ret = MAP_RES_TYPE_1080P;
            }
            else if (w <= 1440 && h <= 2560) {
                ret = MAP_RES_TYPE_2560;
            }
        }
        close(fd);
    }
    mResolution = ret; /* set the resolution*/
    QLOGL(LOG_TAG, QLOG_L1, "Resolution of the device: %" PRIu32, ret);
    return ret;
}
void TargetConfig::DumpAll() {
    int8_t i;
    QLOGL(LOG_TAG, QLOG_L1, "SocId: %" PRId16 " \n", getSocID());
    QLOGL(LOG_TAG, QLOG_L1, "Resolution: %" PRIu32 " \n", getResolution());
    QLOGL(LOG_TAG, QLOG_L1, "Ram: %" PRId32 " GB\n", getRamSize());
    QLOGL(LOG_TAG, QLOG_L1, "Total Ram: %" PRId32 " KB \n", getRamInKB());
    QLOGL(LOG_TAG, QLOG_L1, "Kernel Major Version: %" PRId8 " \n", getKernelMajorVersion());
    QLOGL(LOG_TAG, QLOG_L1, "Kernel Minor Version: %" PRId8 " \n", getKernelMinorVersion());
    QLOGL(LOG_TAG, QLOG_L1, "Full Kernel Version: %s \n", getFullKernelVersion().c_str());
    QLOGL(LOG_TAG, QLOG_L1, "TargetName: %s \n", getTargetName().c_str());
    QLOGL(LOG_TAG, QLOG_L1, "Variant: %s \n", getVariant().c_str());
    QLOGL(LOG_TAG, QLOG_L1, "Type: %" PRIu32 " \n", getType());
    QLOGL(LOG_TAG, QLOG_L1, "Number of cluster %" PRId8 " \n", getNumCluster());
    QLOGL(LOG_TAG, QLOG_L1, "Number of cores %" PRId8 " \n", getTotalNumCores());
    for (i =0; i < getNumCluster(); i++) {
        if (getCoresInCluster(0)) {
            QLOGL(LOG_TAG, QLOG_L1, "Cluster %" PRId8 " has %" PRId8 " cores", i, getCoresInCluster(i));
        }
        QLOGL(LOG_TAG, QLOG_L1, "Cluster %" PRId8 " can have max limit frequency of %" PRIu32 " ", i, getCpuMaxFreqResetVal(i));
    }
    QLOGL(LOG_TAG, QLOG_L1, "Core_ctl_cpu %" PRId8, getCoreCtlCpu());
    QLOGL(LOG_TAG, QLOG_L1, "min_core_online %" PRId8, getMinCoreOnline());
    QLOGL(LOG_TAG, QLOG_L1, "SyncCore_value %" PRId8, isSyncCore());
    QLOGL(LOG_TAG, QLOG_L1, "GovInstance_type %" PRIu8, getGovInstanceType());
    QLOGL(LOG_TAG, QLOG_L1, "CpufreqGov_type %" PRIu8, getCpufreqGov());

    return;
}


int8_t TargetConfig::getPhysicalClusterId(std::string& clusterName) {
    int8_t physicalClusterId = -1;

    try {
        physicalClusterId = mClusterNameToIdMap.at(clusterName);
    } catch(...) {
        physicalClusterId = -1;
    }

    return physicalClusterId;
}
