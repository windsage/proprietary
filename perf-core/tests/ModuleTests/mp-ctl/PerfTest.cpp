/******************************************************************************
  @file  PerfTest.cpp
  @brief test module to test all perflocks

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2018, 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOCK_TEST_TAG "TEST-PERFLOCKS"
#include "PerfTest.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "HintExtHandler.h"
#include "TargetConfig.h"
#include "ResourceInfo.h"
#include "Request.h"
#include "OptsData.h" // for FREAD_STR
#include <thread>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <inttypes.h>
#include <VendorIPerf.h>

#define RESOURCE_CONFIGS_XML_ROOT "ResourceConfigs"
#define RESOURCE_CONFIGS_XML_CHILD_CONFIG "PerfResources"
#define RESOURCE_CONFIGS_XML_CONFIG_TAG "Config"
#define RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG "Target"
#define RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG "OpcodeValue"
#define RESOURCE_CONFIGS_XML_ELEM_MAJORVALUE_TAG "MajorValue"
#define RESOURCE_CONFIGS_XML_ELEM_MINORVALUE_TAG "MinorValue"
#define RESOURCE_CONFIGS_XML_MAJORCONFIG_TAG "Major"
#define RESOURCE_CONFIGS_XML_MINORCONFIG_TAG "Minor"
#define RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG "WVal"
#define RESOURCE_CONFIGS_XML_ELEM_READ_TAG "RVal"
#define RESOURCE_CONFIGS_XML_ELEM_CLUSTER_TAG "Cluster"
#define RESOURCE_CONFIGS_XML_ELEM_EXCEPTION_TAG "Exception"
#define TEST_COMMONRESOURCE_CONFIGS_XML (VENDOR_DIR"/perf/testcommonresourceconfigs.xml")
#define TEST_TARGET_RESOURCE_CONFIGS_XML (VENDOR_DIR"/perf/testtargetresourceconfigs.xml")
#define BUS_DCVS_AVAILABLE_FREQ_L3_PATH "/sys/devices/system/cpu/bus_dcvs/L3/available_frequencies"
#define BUS_DCVS_AVAILABLE_FREQ_DDR_PATH "/sys/devices/system/cpu/bus_dcvs/DDR/available_frequencies"

map<uint16_t, testResourceConfigInfo> PerfTest::mTestResourceConfigInfo;
char* PerfTest::getPassFail(bool rc, char* ptr) {
    if (rc == true) {
        snprintf(ptr,10,"PASSED");
    } else {
        snprintf(ptr,10,"FAILED");
    }
    return ptr;
}

int8_t PerfTest::readPerfTestXml() {
    const string xmlResourceConfigRoot(RESOURCE_CONFIGS_XML_ROOT);
    const string xmlChildResourceConfig(RESOURCE_CONFIGS_XML_CHILD_CONFIG);
    const string fPerfCommonResourcesName(TEST_COMMONRESOURCE_CONFIGS_XML);
    const string fPerfResourceName(TEST_TARGET_RESOURCE_CONFIGS_XML);
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if(xmlParser == NULL) {
        return FAILED;
    }
    int8_t idnum;
    int16_t major_value = -1;
    major_value = -1;
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, PerfCommonResourcesCB, &major_value);
    xmlParser->Parse(fPerfCommonResourcesName);
    xmlParser->DeRegister(idnum);
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, PerfTargetResourcesCB, NULL);
    xmlParser->Parse(fPerfResourceName);
    xmlParser->DeRegister(idnum);
    delete xmlParser;
    return SUCCESS;
}

void PerfTest::PerfCommonResourcesCB(xmlNodePtr node, void *prev_major) {
    int16_t minor_value = -1;
    int16_t *major_value = NULL;
    int32_t wval = -1, rval = -1;
    bool special_node = false;
    int8_t cluster = 0;
    char *exceptn =  NULL;

    if(NULL == prev_major) {
        QLOGE(LOCK_TEST_TAG, "Initialization of Major value Failed");
        return;
    }

    major_value = (int16_t*)prev_major;
    //Parsing the Major tag for the groups major value.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_MAJORCONFIG_TAG)) {
        *major_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG, *major_value);
        QLOGI(LOCK_TEST_TAG, "Identified major resource with Opcode value = 0x%" PRIX16, *major_value);
    }

    //Parsing the Minor tag for resource noe path and it's minor value.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_MINORCONFIG_TAG)) {
        minor_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_OPCODEVALUE_TAG, minor_value);
        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_EXCEPTION_TAG)) {
            exceptn = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_EXCEPTION_TAG);
        } else {
            if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG)) {
                wval = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG, wval);
                rval = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_READ_TAG, wval);
            }
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_CLUSTER_TAG)) {
            cluster = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_CLUSTER_TAG, 0);
        }

        //Ensuring both the major and minor values are initialized and are in the ranges.
        if ((*major_value == -1) || (*major_value >= MAX_MAJOR_RESOURCES) || (minor_value == -1)) {
            QLOGW(LOCK_TEST_TAG, "Major=0x%" PRIX16 " or Minor=0x%" PRIX16 " values are incorrectly Mentioned in %s", *major_value,
                    minor_value, TEST_COMMONRESOURCE_CONFIGS_XML);
        } else {
            UpdatePerfResource(*major_value, minor_value, wval, rval, special_node, cluster, exceptn);
        }

        if (exceptn != NULL)
            xmlFree(exceptn);
    }
    return;
}

void PerfTest::PerfTargetResourcesCB(xmlNodePtr node, void *) {
    int minor_value = -1,
    major_value = -1,
    wval = -1,
    rval = -1;

    bool special_node = false;
    char *target =  NULL;
    bool  valid_target = true;
    char *exceptn =  NULL;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    const char *tname = tc.getTargetName().c_str();
    int8_t cluster = 0;

    //Parsing the only supported configs tag.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_CONFIG_TAG)) {
        major_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_MAJORVALUE_TAG, major_value);
        minor_value = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_MINORVALUE_TAG, minor_value);


        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_EXCEPTION_TAG)) {
            exceptn = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_EXCEPTION_TAG);
        } else {
            if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG)) {
                wval = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG, wval);
                rval = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_READ_TAG, wval);
            }
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG)) {
            target = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_TARGET_TAG);
        }

        if (xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_CLUSTER_TAG)) {
            cluster = ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_CLUSTER_TAG, 0);
        }

        //Ensuring both the major and minor values are initialized and are in the ranges.
        if ((major_value == -1) || (major_value >= MAX_MAJOR_RESOURCES) || (minor_value == -1)) {
            QLOGW(LOCK_TEST_TAG, "Major=0x%" PRIX16 " or Minor=0x%" PRIX16 " values are incorrectly Mentioned in %s", major_value,
                                        minor_value, TEST_TARGET_RESOURCE_CONFIGS_XML);
            return;
        }

        if (target && tname) {
            valid_target = false;
            char *pos = NULL;
            char *tname_token = strtok_r(target, ", ", &pos);
            while(tname_token != NULL) {
                if((strlen(tname_token) == strlen(tname)) and (!strncmp(tname, tname_token,strlen(tname)))) {
                    valid_target = true;
                    break;
                }
                tname_token = strtok_r(NULL, ",", &pos);
            }
        }

        if (valid_target) {
            UpdatePerfResource(major_value, minor_value, wval, rval, special_node, cluster, exceptn);
        }

        if (target != NULL)
            xmlFree(target);

        if (exceptn != NULL)
            xmlFree(exceptn);
    }
    return;
}

void PerfTest::UpdatePerfResource(int16_t major, int16_t minor, int32_t wval, int32_t rval, bool special_node, int8_t cluster, char *exceptn) {
    ResourceInfo tmpr;
    tmpr.SetMajor(major);
    tmpr.SetMinor(minor);
    uint16_t qindx = tmpr.DiscoverQueueIndex();
    mTestResourceConfigInfo.insert_or_assign(qindx, testResourceConfigInfo(qindx, major, minor, wval, rval, special_node, cluster, exceptn));
}

testResourceConfigInfo::testResourceConfigInfo(uint16_t qindex,int16_t major, int16_t minor, int32_t wval, int32_t rval, bool special_node, int8_t cluster,char *exceptn) {
    mQindex = qindex;
    mMajor = major;
    mMinor = minor;
    mWriteValue = wval;
    mReadVal = rval;
    mSpecialNode = special_node;
    mCluster = cluster;
    mSupported = true;
    memset(mNodePath, 0 ,NODE_MAX);
    memset(mException, 0 ,NODE_MAX);
    mExceptionFlag = false;
    if (exceptn != NULL) {
        snprintf(mException, NODE_MAX, "%s",exceptn);
        mExceptionFlag = true;
    }
}

PerfTest::PerfTest() {
    mTc.InitializeTargetConfig();
    PerfConfigDataStore &configStore = PerfConfigDataStore::getPerfDataStore();
    configStore.ConfigStoreInit();
    mT.InitializeTarget();
    if (checkNodeExist(TEST_COMMONRESOURCE_CONFIGS_XML) != SUCCESS) {
        QLOGE(LOCK_TEST_TAG, "%s does not exist on target", TEST_COMMONRESOURCE_CONFIGS_XML);
        mTestEnabled = false;
        return;
    }
    mRunningTest = RF_NODE_TEST;
    mTestEnabled = true;
    readPerfTestXml();
    QLOGE(LOCK_TEST_TAG, "%s", __func__);
    for (auto &i : mTestResourceConfigInfo) {
        setResourceInfo(i.first, i.second);
    }
    mSkipHints[VENDOR_HINT_DISPLAY_OFF] = true;
    mSkipHints[VENDOR_HINT_DISPLAY_ON] = true;
    mSkipHints[VENDOR_HINT_PASS_PID] = true;
}

long int PerfTest::ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue) {
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

PerfTest &PerfTest::getPerfTest() {
    static PerfTest mPerfTest;
    return mPerfTest;
}

int32_t PerfTest::getOpcode(testResourceConfigInfo &resource) {
    int32_t val = 0;
    int8_t core = 0;
    val |= (LOCK_NEW_VERSION << SHIFT_BIT_VERSION);
    val |= (NO_VALUE_MAP << SHIFT_BIT_MAP);
    val |= (resource.mMajor << SHIFT_BIT_MAJOR_TYPE);
    val |= (resource.mMinor << SHIFT_BIT_MINOR_TYPE);
    val |= (resource.mCluster << SHIFT_BIT_CLUSTER);
    val |= (core << SHIFT_BIT_CORE);
    return val;
}

int32_t PerfTest::setResourceInfo(uint16_t qindex, testResourceConfigInfo &resource) {
    bool is_supported = false;
    bool is_special = false;
    char node_path[NODE_MAX] = "";
    memset(node_path,0, NODE_MAX);
    int32_t rc = FAILED;
    rc = mT.getConfigPerflockNode(qindex, node_path, is_supported);
    if (rc != FAILED &&
            (strncasecmp("SPECIAL_NODE", node_path, strlen("SPECIAL_NODE")) == 0)) {
        /* If a node path starts with the key word "SPECIAL_NODE", then it's type is
           assigned as special_node and their ApplyOpts and ResetOpts needs to be defined in mpctl*/
        is_special = true;
    } else if ((resource.mExceptionFlag == false) &&
                ((strstr(node_path, "%s") != NULL) ||
                ((strstr(node_path, "proc") != NULL) && (strstr(node_path, "%d") != NULL)))) {

        resource.mExceptionFlag = true;
        is_special = true;
        snprintf(resource.mException, NODE_MAX, "Node path construction is required");
    }

    if (rc != FAILED && is_supported == true && is_special == false && resource.mExceptionFlag == false) {
        if(strstr(node_path, "%d") != NULL) {
            char d_name[NODE_MAX] = "";
            uint8_t node_i = 0;
            for (int8_t j = 0; j < mTc.getNumCluster(); j++) {
                node_i = 0;
                for (uint8_t i = 0; i < NODE_MAX && node_path[i] != '\0'; i++, node_i++) {
                    if (node_path[i] == '%' && (i + 1) < NODE_MAX && node_path[i + 1] == 'd') {
                        d_name[node_i] = ('0' + mT.getFirstCoreIndex(j));
                        i++;
                    } else {
                        d_name[node_i] = node_path[i];
                    }
                }
                d_name[node_i] = '\0';
                if (rc == SUCCESS) {
                    break;
                }
            }
            resource.setNodePath(d_name);
        } else {
            resource.setNodePath(node_path);
        }
    } else {
        resource.setNodePath(node_path);
    }

    resource.setSupported(is_supported);
    resource.mSpecialNode |= is_special;
    return rc;
}

int32_t PerfTest::getNodeVal(int32_t, testResourceConfigInfo &resource, const char *node_path, char *value) {
    int32_t rc = FAILED;
    if (value == NULL || node_path == NULL) {
        return FAILED;
    }

    memset(value, 0, NODE_MAX);
    char temp_path[NODE_MAX];
    switch (resource.mQindex) {
        /*
           case CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE:
           snprintf(temp_path, NODE_MAX, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 0);
           break;
           case CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE:
           snprintf(temp_path, NODE_MAX, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 0);
           break;
         */
        default:
            snprintf(temp_path, NODE_MAX, "%s",node_path);
            break;
    }
    QLOGI(LOCK_TEST_TAG, "Reading Node %s for Qindex [%" PRIu16 "]", temp_path, resource.mQindex);
    FREAD_BUF_STR(temp_path, value, NODE_MAX, rc);
    return rc;
}

int8_t PerfTest::processString(char * str) {
    if (str == NULL) {
        return FAILED;
    }
    QLOGI(LOCK_TEST_TAG, "Entered");
    uint32_t len = strlen(str);
    for (uint32_t i = 0; i < len && i < NODE_MAX; i++) {
        switch(str[i]) {
            case '\n':
            case '\t':
                str[i] = ' ';
        }
    }
    QLOGI(LOCK_TEST_TAG, "Returning");
    return SUCCESS;
}

int8_t PerfTest::getCoreClusterfromOpcode(int32_t opcode, int8_t &cluster, int8_t &core) {
    cluster = (opcode & EXTRACT_CLUSTER) >> SHIFT_BIT_CLUSTER;
    core = (opcode & EXTRACT_CORE) >> SHIFT_BIT_CORE;
    QLOGI(LOCK_TEST_TAG, "Cluster %" PRId8 " Core %" PRId8, cluster, core);
    return SUCCESS;
}

uint32_t PerfTest::getNextAvailableFreq(const char* node, uint32_t &val) {
    char listf[FREQLIST_STR];
    int32_t rc = FAILED;
    char *cfL = NULL, *pcfL = NULL;
    uint32_t cpu_val = val;
    FREAD_STR(node, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        listf[rc - 1] = '\0';
        processString(listf);
        QLOGI(LOCK_TEST_TAG, "Initializing available freq as %s from %s Value to be taken %d",
                listf, node, val);
        cfL = strtok_r(listf, " ", &pcfL);
        if (cfL) {
            cpu_val = strtol(cfL, NULL, 0);
            if (cpu_val < val) {
                while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
                    cpu_val = strtol(cfL, NULL, 0);
                    if (cpu_val >= val) {
                        val = cpu_val;
                        QLOGI(LOCK_TEST_TAG, "Returning  Value to be taken %d",val);
                        return val;
                    }
                }
            }
        }
    } else {
        QLOGW(LOCK_TEST_TAG, "Initialization of available freq failed, as %s not present", node);
    }
    val = cpu_val;
    QLOGI(LOCK_TEST_TAG, "Returning  Value to be taken %d",val);
    return val;
}

uint32_t PerfTest::getNextCPUFreq(int32_t opcode, uint32_t &val) {
    int8_t core = 0,cluster = 0;
    int8_t cpu = 0;
    char avlnode[NODE_MAX];
    uint32_t cpu_val = val;

    getCoreClusterfromOpcode(opcode, cluster, core);
    int8_t pycluster = mT.getPhysicalCluster(cluster);
    int8_t cpu_num = mT.getFirstCoreIndex(pycluster);
    QLOGI(LOCK_TEST_TAG, "logical [%" PRId8 " : %" PRId8 "] Physical [%" PRId8 " : %" PRId8 "]", cluster, core, pycluster, cpu_num);
    if (cpu < 0 || cpu >= mTc.getTotalNumCores()) {
        QLOGE(LOCK_TEST_TAG, "Incorrect cpu%" PRId8,cpu);
        return 0;
    }

    snprintf(avlnode, NODE_MAX, AVL_FREQ_NODE, cpu_num);
    cpu_val = getNextAvailableFreq(avlnode, val);
    return val;
}

int8_t PerfTest::checkNodeExist(const char * node_path) {
    if (access(node_path, F_OK) == 0) {
        return SUCCESS;
    }
    return FAILED;
}

int8_t testResourceConfigInfo::setNodePath(const char* path) {
    int8_t rc = FAILED;
    if(path != NULL) {
        snprintf(mNodePath,NODE_MAX, "%s", path);
        rc = SUCCESS;
    }
    return rc;
}

int8_t PerfTest::getMaxNode(const char* source, char* node_path) {
    int8_t rc = FAILED;
    char *pos = NULL;
    char temp_path[NODE_MAX] = {0};
    if (source == NULL || node_path == NULL) {
        return FAILED;
    }
    memcpy(temp_path, source, NODE_MAX);
    temp_path[NODE_MAX -1] = '\0';
    if ((pos = strstr(temp_path, "min_freq")) != NULL) {
        *pos = '\0';
        snprintf(node_path, NODE_MAX, "%smax_freq", temp_path);
        rc = SUCCESS;
    } else {
    rc = FAILED;
    }
    return rc;
}

int8_t PerfTest::getAvailableFreqNode(const char* source, char* node_path) {
    int8_t rc = FAILED;
    char *pos = NULL;
    char temp_path[NODE_MAX] = {0};
    if (source == NULL || node_path == NULL) {
        return FAILED;
    }
    memcpy(temp_path, source, NODE_MAX);
    temp_path[NODE_MAX -1] = '\0';
    if (((pos = strstr(temp_path, "min_freq")) != NULL) || ((pos = strstr(temp_path, "max_freq")) != NULL)) {
        *pos = '\0';
        snprintf(node_path, NODE_MAX, "%savailable_frequencies", temp_path);
        if (checkNodeExist(node_path) == FAILED) {
            snprintf(node_path, NODE_MAX, "%s../available_frequencies", temp_path);
        }
        rc = SUCCESS;
    } else {
        rc = FAILED;
    }
    if (rc == SUCCESS) {
        rc = checkNodeExist(node_path);
    }
    return rc;
}

/* Store all the available GPU freq in an integer array */
uint32_t PerfTest::getNextGPUFreq(uint32_t &val) {
    char listf[FREQLIST_STR] = "";
    uint32_t GpuAvailFreq[GPU_FREQ_LVL];
    uint32_t TotalGpuFreq = 0;
    int32_t rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGI(LOG_TAG, "Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(TotalGpuFreq >= GPU_FREQ_LVL) {
                    QLOGE(LOG_TAG, "Number of frequency is more than the size of the array.Exiting");
                    break;
                }
                GpuAvailFreq[TotalGpuFreq++] = strtol(cFreqL, NULL, 0);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(GpuAvailFreq, GpuAvailFreq + TotalGpuFreq);
    } else {
        QLOGE(LOG_TAG, "Initialization of GPU available freq failed, as %s not present", GPU_AVAILABLE_FREQ);
        return val;
    }
    for (uint32_t i = 0 ;i < TotalGpuFreq; i++) {
        if (GpuAvailFreq[i] >= val) {
            val = GpuAvailFreq[i];
            return val;
        }
    }
    if (TotalGpuFreq >0 && TotalGpuFreq < GPU_FREQ_LVL) {
        val = GpuAvailFreq[TotalGpuFreq - 1];
    }
    return val;
}

uint32_t PerfTest::checkNodeVal(int32_t opcode, testResourceConfigInfo &resource, const char* observed, uint32_t &expected, char node_path[NODE_MAX]) {
    if (observed == NULL) {
        return FAILED;
    }
    QLOGI(LOCK_TEST_TAG, "Entered");
    char node_val[NODE_MAX] = "";
    char temp_node[NODE_MAX] = "";
    char expected_s[NODE_MAX];
    char original_expected_s[NODE_MAX];
    char available_freq_path[NODE_MAX] = "";
    char *pos = NULL;
    char *pos1 = NULL;
    uint32_t min_val = 0;
    uint32_t max_val = UINT_MAX;
    uint32_t max_freq_val = UINT_MAX;
    uint32_t original_expected = expected;

    switch(resource.mQindex) {
        case CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE:
        case CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE:
        case INTERACTIVE_START_INDEX + SCHEDUTIL_HISPEED_FREQ_OPCODE:
            expected = getNextCPUFreq(opcode, expected);
            break;
            /* TODO verify with kernel team direct value is written on l3 nodes
               case MEMLAT_START_INDEX +  BUS_DCVS_L3_MEMLAT_MINFREQ_PRIME_OPCODE:
               expected = getNextAvailableFreq(BUS_DCVS_AVAILABLE_FREQ_L3_PATH, expected);
               break;
             */
        case LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_MINFREQ_OPCODE_V2:
            {
                min_val = getNextAvailableFreq(BUS_DCVS_AVAILABLE_FREQ_DDR_PATH, min_val);
                max_val = getNextAvailableFreq(BUS_DCVS_AVAILABLE_FREQ_DDR_PATH, max_val);
                if (expected < min_val) {
                    expected = min_val;
                } else if (expected > max_val) {
                    expected = max_val;
                }
            }
            break;
        case MEMLAT_START_INDEX + BUS_DCVS_LLCC_DDR_LAT_MINFREQ_OPCODE:
            {
                if (getMaxNode(resource.mNodePath,temp_node) == SUCCESS) {
                    if (SUCCESS == checkNodeExist(temp_node)) {
                        int8_t rc = FAILED;
                        FREAD_BUF_STR(temp_node, node_val, NODE_MAX, rc);
                        if (rc != FAILED) {
                            max_freq_val = strtol(node_val,NULL,0);
                            if (expected > max_freq_val) {
                                expected = max_freq_val;
                            }
                        }
                    }
                }
                expected = getNextAvailableFreq(BUS_DCVS_AVAILABLE_FREQ_DDR_PATH, expected);
                break;
            }
        case SCHED_START_INDEX + SCHED_CPUSET_TOP_APP_OPCODE:
        case SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE:
        case SCHED_START_INDEX + SCHED_CPUSET_SYSTEM_BACKGROUND_OPCODE:
        case SCHED_START_INDEX + SCHED_CPUSET_BACKGROUND_OPCODE:
            {
                if (mRunningTest == RF_HINT_TEST) {
                    QLOGI(LOCK_TEST_TAG, "Returning");
                    return SUCCESS;
                }
                break;
            }
        case GPU_START_INDEX + GPU_MIN_FREQ_OPCODE:
        case GPU_START_INDEX + GPU_MAX_FREQ_OPCODE:
            expected = getNextGPUFreq(expected);
            break;
        case GPU_START_INDEX + GPU_MIN_FREQ_MHZ_OPCODE:
            expected = expected*1000000;
            expected = getNextGPUFreq(expected);
            expected = expected/1000000;
            break;
        default:
            int8_t tmprc = getAvailableFreqNode(node_path, available_freq_path);
            if (tmprc == SUCCESS) {
                expected = getNextAvailableFreq(available_freq_path, expected);
            }
            break;
    }
    snprintf(expected_s, NODE_MAX,"%" PRIu32, expected);
    snprintf(original_expected_s, NODE_MAX,"%" PRIu32, original_expected);
    memcpy(node_val, observed, NODE_MAX);
    node_val[NODE_MAX - 1] = '\0';
    QLOGI(LOCK_TEST_TAG, "%s", node_val);
    char *val_token_1 = strtok_r(node_val, " ", &pos1);
    while(val_token_1 != NULL) {
        char *val_token = strtok_r(val_token_1, ":", &pos);
        QLOGI(LOCK_TEST_TAG, "%s", val_token);
        while (val_token != NULL) {
            if (strstr(val_token, ".") != NULL) {
                uint32_t float_val = strtol(val_token, NULL, 0);
                if (float_val == expected) {
                    QLOGI(LOCK_TEST_TAG, "Returning");
                    return SUCCESS;
                } else if (float_val == original_expected) {
                    expected = original_expected;
                    return SUCCESS;
                }
            } else {
                if (areSameStrings(expected_s,val_token) == SUCCESS) {
                    QLOGI(LOCK_TEST_TAG, "Returning Val%" PRIu32, expected);
                    return SUCCESS;
                } else if (areSameStrings(original_expected_s, val_token) == SUCCESS) {
                    QLOGI(LOCK_TEST_TAG, "Returning Val%" PRIu32, original_expected);
                    expected = original_expected;
                    return SUCCESS;
                }
            }
            val_token = strtok_r(NULL, " ", &pos);
        }
        val_token_1 = strtok_r(NULL, " ", &pos1);
    }
    expected = original_expected;
    QLOGI(LOCK_TEST_TAG, "Returning");
    return FAILED;
}

int8_t hintComparator(pair<int32_t, pair<int32_t,int32_t>> A, pair<int32_t, pair<int32_t,int32_t>> B) {
    if (A.first == B.first) {
        if (A.second.first == B.second.first) {
            return A.second.second < B.second.second;
        } else {
            return A.second.first < B.second.first;
        }
    }
    return A.first < B.first;
}

int8_t PerfTest::areSameStrings(const char *node_val_before, const char *node_value_after) {
    int8_t rc = FAILED;
    if (!node_val_before || !node_value_after) {
        return rc;
    }
    if (strlen(node_val_before) == strlen(node_value_after) && (!strncmp(node_value_after, node_val_before, strlen(node_val_before)))) {
        rc = SUCCESS;
    }
    return rc;
}

//Time in ms
int8_t PerfTest::gotoSleep(uint32_t sleepTime) {
    int8_t rc = FAILED;
    struct timespec sleep_time;
    if (sleepTime < 0) {
        QLOGE(LOCK_TEST_TAG, "Invalid Sleep Time %" PRId32, sleepTime);
    } else {
        rc = SUCCESS;
        sleep_time.tv_sec = sleepTime / TIME_MSEC_IN_SEC;
        sleep_time.tv_nsec = (sleepTime % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
        nanosleep(&sleep_time, NULL);
    }
    return rc;
}

int32_t PerfTest::processHintActions(mpctl_msg_t *pMsg, uint32_t fps, int32_t &fpsEqHandle) {
    uint32_t size = 0;
    char node_val[NODE_MAX] = "";
    int32_t rc = FAILED;
    uint32_t cur_fps = 0;
    uint32_t sleepTime = 13000;
    if (pMsg == NULL) {
        return size;
    }
    HintExtHandler &hintExt = HintExtHandler::getInstance();
    int32_t preActionRetVal = 0;
    int32_t timeout = 0;

    if (LAUNCH_TYPE_ATTACH_APPLICATION != pMsg->hint_type) {
        preActionRetVal = hintExt.FetchConfigPreAction(pMsg);
    }
    if (preActionRetVal == PRE_ACTION_SF_RE_VAL) {
        QLOGI(LOCK_TEST_TAG, "Special handling for SF/RE pid hint");
        return preActionRetVal;
    }
    else if (preActionRetVal == PRE_ACTION_NO_FPS_UPDATE) {
        QLOGI(LOCK_TEST_TAG, "Preaction FPS, dont update FPS");
        return FAILED;
    }
    /*
    fps = FpsUpdateAction::getInstance().GetFps();
    */
    if (fps != 0 ||  pMsg->hint_id == VENDOR_HINT_SCROLL_BOOST) {
        if (pMsg->hint_type < 30) {
            FREAD_BUF_STR(CURRENT_FPS_FILE, node_val, NODE_MAX, rc);
            processString(node_val);
            cur_fps = strtol(node_val, NULL, 0);
            if (fps != cur_fps) {
                QLOGE(LOCK_TEST_TAG, "Switching Fps to %" PRIu32 " from %" PRIu32 " to Test hint 0x0000%" PRIX32
                    " type %" PRId32 " with Fps %" PRIu32, fps , cur_fps, pMsg->hint_id, pMsg->hint_type, fps);
                if (fps > cur_fps) {
                    sleepTime = 2000;
                }
                fpsEqHandle = perf_hint(VENDOR_HINT_FPS_UPDATE, "RF-TEST-FPS_UPDATE", 0, fps > 0 ? fps:30);
                gotoSleep(sleepTime);
                uint32_t len = 1;
                if (fps > 0) {
                    len = log10(fps) + 2;
                }
                FREAD_BUF_STR(CURRENT_FPS_FILE, node_val, len, rc);
                processString(node_val);
                cur_fps = strtol(node_val, NULL, 0);
                if (cur_fps != fps) {
                    QLOGE(LOCK_TEST_TAG, "Unable to Switch to Fps %" PRIu32 " from %" PRIu32 " to Test hint 0x0000%" PRIX32
                          " type %" PRId32 " with Fps %" PRIu32, fps, cur_fps, pMsg->hint_id, pMsg->hint_type, fps);
                    return -1;
                }
            }
        }
    }


    size = mT.getBoostConfig(pMsg->hint_id, pMsg->hint_type, pMsg->pl_args, &timeout,
            fps);

    if (size == 0) {
        //hint lookup failed, bailout
        QLOGW(LOCK_TEST_TAG, "Unsupported hint_id=0x%" PRIX32 ", hint_type=%" PRId32 " for this target", pMsg->hint_id, pMsg->hint_type);
        return FAILED;
    }
    pMsg->data = size;
    if (LAUNCH_TYPE_ATTACH_APPLICATION != pMsg->hint_type) {
        hintExt.FetchConfigPostAction(pMsg);
    }

    if (pMsg->pl_time <= 0) {
        pMsg->pl_time = timeout;
    }

    if (pMsg->pl_time < 0) {
        QLOGE(LOCK_TEST_TAG, "No valid timer value for perflock request");
        return FAILED;
    }
    return size;
}

int8_t PerfTest::getNodePath(int32_t opcode, testResourceConfigInfo &resource, char *node_path) {
    int8_t core = 0,cluster = 0;
    int8_t rc = FAILED;
    char *pos = NULL;
    if (node_path == NULL) {
        return rc;
    }

    getCoreClusterfromOpcode(opcode, cluster, core);
    int8_t pycluster = mT.getPhysicalCluster(cluster);
    int8_t cpu_num = mT.getFirstCoreIndex(pycluster);
    cpu_num +=core;
    memset(node_path, 0 , NODE_MAX);
    memcpy(node_path, resource.mNodePath, NODE_MAX);
    node_path[NODE_MAX - 1] = '\0';
    switch(resource.mQindex) {
        case MEMLAT_START_INDEX + L2_MEMLAT_RATIO_CEIL_0_OPCODE:
        case MEMLAT_START_INDEX + L2_MEMLAT_RATIO_CEIL_1_OPCODE:
        case MEMLAT_START_INDEX + L3_MEMLAT_STALL_FLOOR_0_OPCODE:
        case MEMLAT_START_INDEX + L3_MEMLAT_STALL_FLOOR_1_OPCODE:
        case MEMLAT_START_INDEX + MEMLAT_MIN_FREQ_0_OPCODE:
        case MEMLAT_START_INDEX + MEMLAT_MIN_FREQ_1_OPCODE:
        case MEMLAT_START_INDEX + LLCC_MEMLAT_RATIO_CELL_0_OPCODE:
        case MEMLAT_START_INDEX + LLCC_MEMLAT_RATIO_CELL_1_OPCODE:
        case MEMLAT_START_INDEX + LLCC_MEMLAT_STALL_FLOOR_0_OPCODE:
        case MEMLAT_START_INDEX + LLCC_MEMLAT_STALL_FLOOR_1_OPCODE:
        case MEMLAT_START_INDEX + LLCC_DDR_LAT_RATIO_CELL_0_OPCODE:
        case MEMLAT_START_INDEX + LLCC_DDR_LAT_RATIO_CELL_1_OPCODE:
        case MEMLAT_START_INDEX + LLCC_DDR_LAT_STALL_FLOOR_0_OPCODE:
        case MEMLAT_START_INDEX + LLCC_DDR_LAT_STALL_FLOOR_1_OPCODE:
        case MEMLAT_START_INDEX + BUS_DCVS_LLCC_DDR_GOLD_LAT_EN_SPM_VOTE:
        case MEMLAT_START_INDEX + BUS_DCVS_LLCC_DDR_PRIME_LAT_EN_SPM_VOTE:
            return SUCCESS;
    }
    if ((pos = strstr(node_path, "policy0")) != NULL) {
        pos += strlen("policy");
        *pos = '0' + cpu_num;
        rc = SUCCESS;
        QLOGI(LOCK_TEST_TAG, " 0x%" PRIX32 " :: %s",opcode, node_path);
    } else if ((pos = strstr(node_path, "cpu0")) != NULL) {
        pos += strlen("cpu");
        *pos = '0' + cpu_num;
        rc = SUCCESS;
        QLOGI(LOCK_TEST_TAG, "0x%" PRIX32 " :: %s",opcode, node_path);
    } else {
        rc = SUCCESS;
    }
    return rc;
}

int32_t PerfTest::PerfHintTest(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    vector<pair<int32_t, pair<int32_t,uint32_t>>> hint_types;
    bool firstFpsUpdate = false;
    uint32_t size = mT.GetAllBoostHintType(hint_types);
    int32_t pl_args[MAX_ARGS_PER_REQUEST];
    uint32_t duration = 5000;
    char node_val_after[MAX_RESOURCES_PER_REQUEST][NODE_MAX];
    char node_val_before[MAX_RESOURCES_PER_REQUEST][NODE_MAX];
    char node_paths[MAX_RESOURCES_PER_REQUEST][NODE_MAX];
    int8_t nodes_flags[MAX_RESOURCES_PER_REQUEST];
    mpctl_msg_t pMsg;
    HintExtHandler &hintExt = HintExtHandler::getInstance();
    mRunningTest = RF_HINT_TEST;

    map<uint16_t, testResourceConfigInfo>::iterator hint_resources[MAX_RESOURCES_PER_REQUEST];
    if (size == 0 || hint_types.size() <= 0) {
        return FAILED;
    }
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create default values file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Handle|HintId|HintType|Fps|Timeout|Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    sort(hint_types.begin(), hint_types.end(), hintComparator);
    res = SUCCESS;
    for(auto &hint : hint_types) {
        perf_hint(VENDOR_HINT_DISPLAY_ON, "RF-DISP-ON", 10000000, -1);

        bool hintExcluded = false;
        int32_t hint_id = hint.first;
        int32_t hint_type = hint.second.first;
        uint32_t fps = hint.second.second;
        int32_t numArgs = 0;
        int32_t handle = -1;
        int32_t fpsEqHandle = -1;
        rc = FAILED;

        if (mSkipHints.find(hint_id) != mSkipHints.end()) {
            continue;
        }
        memset(pl_args, 0, sizeof(int32_t) * MAX_ARGS_PER_REQUEST);
        memset(nodes_flags, 0, sizeof(int8_t) * MAX_RESOURCES_PER_REQUEST);
        memset(&pMsg, 0, sizeof(mpctl_msg_t));
        pMsg.pl_time = 0;
        pMsg.hint_id = hint_id;
        pMsg.hint_type = hint_type;
        pMsg.version = MSG_VERSION;
        pMsg.size = sizeof(mpctl_msg_t);

        if (hintExt.FetchHintExcluder(&pMsg) == SUCCESS) {
            hintExcluded = true;
        } else {
            QLOGI(LOCK_TEST_TAG, "Sending Hint: 0x%" PRIX32 " %" PRId32, hint_id, hint_type);
            numArgs = processHintActions(&pMsg, fps, fpsEqHandle);
            if (numArgs < 0) {
                continue;
            }
        }

        //get iterator's for all resources in hint
        numArgs = numArgs < MAX_ARGS_PER_REQUEST ? numArgs : MAX_ARGS_PER_REQUEST;
        for (uint8_t idx=0; idx < numArgs; idx+=2) {
            QLOGI(LOCK_TEST_TAG, "Sending Hint: 0x%" PRIX32 " %" PRId32 " i: [%" PRIu8 "]", hint_id, hint_type, idx);
            memset(node_val_after[idx/2], 0, sizeof(char) * NODE_MAX);
            memset(node_val_before[idx/2], 0, sizeof(char) * NODE_MAX);
            ResourceInfo tmpr;
            tmpr.ParseNewRequest(pMsg.pl_args[idx]);
            hint_resources[idx/2] = mTestResourceConfigInfo.find(tmpr.GetQIndex());
            if (hint_resources[idx/2] != mTestResourceConfigInfo.end()) {
                getNodePath(pMsg.pl_args[idx], hint_resources[idx/2]->second, node_paths[idx/2]);
            }
        }

        for (uint8_t idx=0; idx < numArgs; idx+=2) {
            QLOGI(LOCK_TEST_TAG, "Reading Before Value for 0x%" PRIX32 " %" PRId32 " 0x%" PRId32 " ", hint_id, hint_type, pMsg.pl_args[idx/2]);
            if (hint_resources[idx/2] != mTestResourceConfigInfo.end()) {
                testResourceConfigInfo &resource = hint_resources[idx/2]->second;
                rc = checkNodeExist(resource.mNodePath);
                if (rc == SUCCESS) {
                    rc = getNodeVal(pMsg.pl_args[idx], resource, node_paths[idx/2], node_val_before[idx/2]);
                }
            }
        }
        if (hintExcluded) {
            rc = SUCCESS;
        } else {
            duration = pMsg.pl_time;
            if (duration == 0 || duration > 2000) {
                duration = 2000;
            }
            handle = perf_hint(hint_id, "RF-TEST", duration, hint_type);
            if (firstFpsUpdate == false && hint_id == VENDOR_HINT_FPS_UPDATE) {
                firstFpsUpdate = true;
                uint32_t sleepTime = 13000;
                gotoSleep(sleepTime);
                handle = 0;
            }
        }

        if (handle >= 0 && hintExcluded == false) {
            uint32_t sleepTime = duration/2;
            gotoSleep(sleepTime);
            for (uint8_t idx=0; idx < numArgs; idx+=2) {
                QLOGI(LOCK_TEST_TAG, "%d Reading After Value for 0x%" PRIu32 " %" PRId32 " 0x%" PRIX32 " ",__LINE__, hint_id, hint_type, pMsg.pl_args[idx/2]);
                if (hint_resources[idx/2] == mTestResourceConfigInfo.end()) {
                    snprintf(node_val_before[idx/2], NODE_MAX, "Node Does not exist in test xml");
                    snprintf(node_val_after[idx/2], NODE_MAX, "Node Does not exist in test xml");
                    nodes_flags[idx/2] = FAILED;
                } else {
                    testResourceConfigInfo &resource = hint_resources[idx/2]->second;
                    rc = checkNodeExist(resource.mNodePath);
                    if (rc == FAILED) {
                        snprintf(node_val_before[idx/2], NODE_MAX, "Node Does not exist on target");
                        snprintf(node_val_after[idx/2], NODE_MAX, "Node Does not exist on target");
                        nodes_flags[idx/2] = FAILED;
                    } else {
                        rc = getNodeVal(pMsg.pl_args[idx], resource, node_paths[idx/2], node_val_after[idx/2]);
                    }
                }
            }

            perf_lock_rel(handle);
            perf_lock_rel(fpsEqHandle);
            if (numArgs <= 0) {
                snprintf(outfile, OUT_LINE_BUF, "%" PRId32 "|0x0000%" PRIX32 "|%" PRId32 "|%" PRIu32 "|%" PRId32 "|%" PRId32
                        "|%" PRIu8 "|%" PRIu8 "|%" PRIu8 "|%s|%" PRIu8 "|%" PRIu8 "|%s|%" PRIu8 "|%" PRIu8 "|%s|%s\n",
                        handle, hint_id, hint_type,fps, duration,
                        numArgs, 0, 0, 0,
                        "No Resources in this Hint", 0,
                        0, "No Resources in this Hint",
                        0, 0, "No Resources in this Hint",
                        getPassFail(FAILED == SUCCESS, rc_s));
                fwrite(outfile, sizeof(char), strlen(outfile), defval);
            }
            for (int32_t idx=0; idx < numArgs; idx+=2) {
                QLOGI(LOCK_TEST_TAG, "%d Checking Value for 0x%" PRIu32 " %" PRId32 " 0x%" PRId32 " ",__LINE__, hint_id, hint_type, pMsg.pl_args[idx/2]);
                rc = FAILED;
                uint32_t expected_val = pMsg.pl_args[idx + 1];

                if (hint_resources[idx/2] != mTestResourceConfigInfo.end()) {
                    rc = FAILED;
                    testResourceConfigInfo &resource = hint_resources[idx/2]->second;
                    processString(node_val_before[idx/2]);
                    processString(node_val_after[idx/2]);
                    if (resource.mReadVal/resource.mWriteValue >=10) {
                        expected_val *= resource.mReadVal/resource.mWriteValue;
                    }
                    if (resource.mExceptionFlag) {
                        rc = SUCCESS;
                        snprintf(node_val_before[idx/2], NODE_MAX, "Exception: %s", resource.mException);
                        snprintf(node_val_after[idx/2], NODE_MAX, "Exception: %s", resource.mException);
                    } else if (resource.mSpecialNode) {
                        rc = SUCCESS;
                        snprintf(node_val_before[idx/2], NODE_MAX, "Special Node");
                        snprintf(node_val_after[idx/2], NODE_MAX, "Special Node");
                    } else if (resource.mSupported == false) {
                        rc = FAILED;
                        snprintf(node_val_before[idx/2], NODE_MAX, "Node Not Supported on target");
                        snprintf(node_val_after[idx/2], NODE_MAX, "Node Not Supported on target");
                    } else if (nodes_flags[idx/2] != FAILED) {
                        rc = checkNodeVal(pMsg.pl_args[idx], resource, node_val_after[idx/2], expected_val, node_paths[idx/2]);
                    }
                    snprintf(outfile, OUT_LINE_BUF, "%" PRId32 "|0x0000%" PRIX32 "|%" PRId32 "|%" PRIu32 "|%" PRId32 "|%" PRIu16
                            "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16 "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                            handle, hint_id, hint_type, fps,duration,
                            resource.mQindex, pMsg.pl_args[idx], resource.mMajor, resource.mMinor,
                            node_paths[idx/2], resource.mSpecialNode,
                            resource.mSupported, node_val_before[idx/2],
                            pMsg.pl_args[idx + 1], expected_val,
                            node_val_after[idx/2], getPassFail(rc == SUCCESS, rc_s));
                } else {
                    ResourceInfo tmpr;
                    tmpr.ParseNewRequest(pMsg.pl_args[idx]);
                    snprintf(outfile, OUT_LINE_BUF, "%" PRId32 "|0x0000%" PRIX32 "|%" PRId32 "|%" PRIu32 "|%" PRId32 "|%" PRIu16
                            "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16 "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                            handle, hint_id, hint_type, fps,duration,
                            tmpr.GetQIndex(), pMsg.pl_args[idx], tmpr.GetMajor(), tmpr.GetMinor(),
                            node_val_after[idx/2], -1,
                            -1, node_val_before[idx/2],
                            pMsg.pl_args[idx + 1], expected_val,
                            node_val_after[idx/2], getPassFail(rc == SUCCESS, rc_s));
                }
                if (rc == FAILED) {
                    res = rc;
                }
                fwrite(outfile, sizeof(char), strlen(outfile), defval);
            }
        } else {
            if (hintExcluded) {
                rc = SUCCESS;
                snprintf(node_val_after[0], NODE_MAX, "Hint Excluded");
            } else {
                rc = res = FAILED;
                snprintf(node_val_after[0], NODE_MAX, "Failed to Acquire Hint");
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRId8 "|0x0000%" PRIX32 "|%" PRId32 "|%" PRIu32 "|%" PRId8 "|%" PRId8 "|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId8 "|%" PRId8 "|%s|%s\n",
                    -1, hint_id, hint_type, fps,-1,
                    -1, -1, -1, -1,
                    node_val_after[0], -1,
                    -1, node_val_after[0],
                    -1, -1, node_val_after[0],
                    getPassFail(rc == SUCCESS, rc_s));
            fwrite(outfile, sizeof(char), strlen(outfile), defval);
        }
        snprintf(outfile, OUT_LINE_BUF, " | | | | | | | | | | | | | | | |*\n");
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }

    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

int32_t PerfTest::PerfPropTest(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char prop_def_val[PROP_VAL_LENGTH] = {0};
    char prop_ret_val[PROP_VAL_LENGTH] = {0};
    char rc_s[NODE_MAX] = {0};
    unordered_map<std::string, std::string> allProps;
    PerfConfigDataStore &configStore = PerfConfigDataStore::getPerfDataStore();
    rc = configStore.getProps(allProps);
    if (rc == FAILED) {
        QLOGE(LOCK_TEST_TAG, "Failed to read prop for running test.");
        return FAILED;
    }
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        QLOGE(LOCK_TEST_TAG, "Running Test without writing to file");
    } else {
        defval = fopen(filename, "w+");
    }
    snprintf(outfile, OUT_LINE_BUF, "Property|Expected Value|Received Value|RESULT\n");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        QLOGE(LOCK_TEST_TAG, "Running Test without writing to file");
    } else {
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    res = SUCCESS;
    snprintf(prop_def_val,PROP_VAL_LENGTH, "Undefined");
    for (auto &prop : allProps) {
        rc = FAILED;
        memset(prop_ret_val, 0, PROP_VAL_LENGTH);

        snprintf(prop_ret_val,PROP_VAL_LENGTH,"%s", perf_get_prop(prop.first.c_str(), prop_def_val).value);
        rc = areSameStrings(prop_ret_val, prop.second.c_str());
        snprintf(outfile, OUT_LINE_BUF, "%s|%s|%s|%s\n",prop.first.c_str(), prop.second.c_str(), prop_ret_val, getPassFail(rc == SUCCESS, rc_s));
        QLOGE(LOCK_TEST_TAG, "%s", outfile);
        if (rc == FAILED) {
            res = rc;
        }
        if (defval != NULL) {
            fwrite(outfile, sizeof(char), strlen(outfile), defval);
        }
    }
    if (defval != NULL) {
        QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
        fclose(defval);
    }
    return res;
}

int32_t PerfTest::PerfLockTest(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_value_restored[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[2] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create default values file\n");
        return FAILED;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|Restored Val|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {
        perf_hint(VENDOR_HINT_DISPLAY_ON, "RF-DISP-ON", 10000000, -1);
        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(node_value_restored, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %d", idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8 "|%" PRId8 "|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml", -1 , -1, -1,
                    -1,-1,-1, -1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor ox%" PRIX16, resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            int32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_value_restored, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
                snprintf(node_value_restored, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
                snprintf(node_value_restored, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_value_restored, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;
                    int32_t handle = perf_lock_acq(0, TIMEOUT_DURATION, lock_args, 2);
                    if (handle > 0) {
                        processString(node_val_before);
                        uint32_t duration = TIMEOUT_DURATION_SLEEP;
                        gotoSleep(duration);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        perf_lock_rel(handle);
                        processString(node_val_after);
                        expected_val = resource.mReadVal;
                        gotoSleep(100);
                        rc = getNodeVal(opcode, resource, node_paths, node_value_restored);
                        processString(node_value_restored);
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);
                        if (areSameStrings(node_val_before, node_value_restored) == SUCCESS && rc == SUCCESS) {
                            rc = SUCCESS;
                        } else {
                            rc = FAILED;
                        }
                    } else {
                        rc = FAILED;
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16
                    "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val,
                    node_val_after, node_value_restored, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

int32_t PerfTest::PerfNodeTest(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = "";
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return res;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|Major|Minor|NodePath|Special Resource|Supported|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {
        rc = FAILED;
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8 "|%" PRId8
                    "|%s\n",idx, -1, -1,"Not Exist in test xml", -1 , -1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            rc = SUCCESS;
            if(resource.mSupported && resource.mSpecialNode == false) {
                if (!resource.mExceptionFlag) {
                    rc = checkNodeExist(resource.mNodePath);
                    if (rc == FAILED) {
                        res = rc;
                    }
                }
            }

            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX16 "|0x%" PRIX16 "|%s|%" PRId8 "|%" PRId8 "|%s\n",resource.mQindex,
                    resource.mMajor, resource.mMinor,
                    resource.mNodePath, resource.mSpecialNode,
                    resource.mSupported, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    if (rc == FAILED) {
        res = rc;
    }
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

int32_t PerfTest::DisplayTestsEW(const char *) {
    return SUCCESS;
}
int32_t PerfTest::DisplayTestsLC(const char *) {
    return SUCCESS;
}
int32_t PerfTest::DisplayTestsFPSU(const char *) {
    return SUCCESS;
}
int32_t PerfTest::AVCDenialTest(const char * filename) {
    int32_t rc = perf_hint(VENDOR_HINT_TEST, filename, 0, PERF_AVC_TEST_TYPE);
    return rc;
}

//do delete char buffer returned by this method
const char *PerfTest::getActiveReqDump() {
    const char *tmp = perf_sync_request(SYNC_CMD_DUMP_DEG_INFO);
    return tmp;
}

int32_t PerfTest::clearAllReqExistInActiveReq() {
    const char *tmp = getActiveReqDump();
    if (tmp == NULL) {
        QLOGE(LOCK_TEST_TAG, "Failed to release all existing request");
        return FAILED;
    }

    uint32_t len = strlen(tmp);
    char result[len+1];
    memcpy(result, tmp, len+1);
    result[len] = '\0';
    if (tmp != NULL) {
        free((void*)tmp);
    }
    char *pos = NULL;
    char *token = strtok_r(result, "\n", &pos);
    while(token != NULL) {
        if(strstr(token, "|") != NULL) {
            QLOGD(LOCK_TEST_TAG, "%s", token);
            int32_t colindex = 0;
            char *posi = NULL;
            char *coli = strtok_r(token, "|", &posi);
            while(coli != NULL) {
                if (colindex == REQ_DUMP_HANDLE_COLUMN) { //Handle column from dump
                    int32_t val =  strtol(coli, NULL, 0);
                    if (val > 0) {
                        QLOGE(LOCK_TEST_TAG, "Sending PerfLock Rel for Handle: %d", val);
                        perf_lock_rel(val);
                    }
                    break;
                }
                coli = strtok_r(NULL, "|", &posi);
                colindex++;
            }
        }
        token = strtok_r(NULL, "\n", &pos);
    }
    return SUCCESS;
}

bool PerfTest::checkReqExistInActiveReq(int32_t checkvalue, int32_t column) {
    const char *tmp = getActiveReqDump();
    bool rc = false;
    if (tmp == NULL) {
        return rc;
    }

    uint32_t len = strlen(tmp);
    QLOGE(LOCK_TEST_TAG, "\n %s \n", tmp);
    char result[len+1];
    memcpy(result, tmp, len+1);
    result[len] = '\0';
    if (tmp != NULL) {
        free((void*)tmp);
    }
    char *pos = NULL;
    char *token = strtok_r(result, "\n", &pos);
    while(token != NULL && rc == false) {
        if(strstr(token, "|") != NULL) {
            QLOGE(LOCK_TEST_TAG, "%s", token);
            int32_t colindex = 0;
            char *posi = NULL;
            char *coli = strtok_r(token, "|", &posi);
            while(coli != NULL && rc == false) {
                if (colindex == column && column == REQ_DUMP_HANDLE_COLUMN) { //Handle column from dump
                    int32_t val =  strtol(coli, NULL, 0);
                    if (val > 0 && val == checkvalue) {
                        rc = true;
                    }
                    QLOGE(LOCK_TEST_TAG, "%d token: %s [ check: %d,  val: %d] rc: %d",colindex, coli, checkvalue, val, rc);
                    break;
                }
                coli = strtok_r(NULL, "|", &posi);
                colindex++;
            }
        }
        token = strtok_r(NULL, "\n", &pos);
    }
    return rc;
}

int8_t PerfTest::getDisplayOffHintCount() {
    uint32_t count = 0;
    const char *tmp = getActiveReqDump();
    if (tmp == NULL) {
        return count;
    }

    uint32_t len = strlen(tmp);
    char result[len+1];
    memcpy(result, tmp, len+1);
    result[len] = '\0';
    if (tmp != NULL) {
        free((void*)tmp);
    }
    char *pos = NULL;
    char *token = strtok_r(result, "\n", &pos);
    while(token != NULL) {
        if(strstr(token, "0x1040") != NULL) {
            QLOGE(LOCK_TEST_TAG, "%s", token);
            count++;
            if (count > 1) {
                QLOGE(LOCK_TEST_TAG, "Found Multiple entries for Display off Hint");
                break;
            }
        }
        token = strtok_r(NULL, "\n", &pos);
    }
    return count;
}

int8_t PerfTest::checkDisplayOffHintCount() {
    int8_t rc = SUCCESS;
    uint32_t count = getDisplayOffHintCount();
    if (count == 0 || count > 1) {
        rc = FAILED;
    }
    return rc;
}

int32_t PerfTest::RunTest(TestData_t &data) {
    int32_t rc = FAILED;
    PerfTest &pt = getPerfTest();
    if (pt.isTestEnabled() == false) {
        return rc;
    }
    if (data.mTestName == RF_PERF_NODE_TEST) {

        rc =  pt.PerfNodeTest(data.mTestDetailedFile.c_str());

    } else if ( data.mTestName == RF_PERF_LOCK_TEST) {

        rc =  pt.PerfLockTest(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == RF_PERF_HINT_TEST) {

        rc =  pt.PerfHintTest(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_TEST_EARLYWAKEUP) {

        rc =  pt.DisplayTestsEW(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_TESTS_LARGECOMP) {

        rc =  pt.DisplayTestsLC(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_TESTS_FPS_UPDATE) {

        rc =  pt.DisplayTestsFPSU(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == AVC_DENIAL_TEST) {

        rc =  pt.AVCDenialTest(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_TEST) {

        rc = pt.DisplayOnOFF(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY) {

        rc = pt.DisplayOnOFFAlwaysApply(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_OFF_NODE_CHECK) {

        rc = pt.DisplayOFF(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == PERF_GET_PROP_TEST) {

        rc = pt.PerfPropTest(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_REQUEST_PENDED_TEST) {

        rc = pt.DisplayOnOFFRequestPended(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_OFF_REQUEST_UNPENDED_TEST) {

        rc = pt.DisplayOFFCheckUnpended(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_OFF_REQUEST_EXPIRED_RELEASED_TEST) {

        rc = pt.DisplayOFFReqExpired(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPIRED_REL_TEST) {

        rc = pt.DisplayOnOFFAlwaysApplyReqExpired(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TEST) {

        rc = pt.DisplayOnOFFAlwaysApplyRelReq(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_AFTER_OFF_TEST) {

        rc = pt.DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOff(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TIMEDOUT_AFTER_OFF_TEST) {

        rc = pt.DisplayOnOFFAlwaysApplyRelReqTimedOutAfterDisaplyOff(data.mTestDetailedFile.c_str());

    } else if (data.mTestName == DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_AFTER_OFF_TEST_CONCERUNCY) {

        rc = pt.DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOffConceruncy(data.mTestDetailedFile.c_str());

    }





    else {

        rc = FAILED;

    }
    return rc;
}

//interface for TTF
static RegressionFramework ttfarl = {
   RF_PERF_NODE_TEST,  PerfTest::RunTest, MPCTL,
};

static RegressionFramework ttfar2 = {
   RF_PERF_LOCK_TEST,  PerfTest::RunTest, MPCTL,
};

static RegressionFramework ttfar3 = {
   RF_PERF_HINT_TEST, PerfTest::RunTest, MPCTL,
};

static RegressionFramework ttfar4 = {
   DISPLAY_TEST_EARLYWAKEUP, PerfTest::RunTest, MPCTL,
};

static RegressionFramework ttfar5 = {
   DISPLAY_TESTS_LARGECOMP, PerfTest::RunTest, MPCTL,
};
static RegressionFramework ttfar6 = {
   DISPLAY_TESTS_FPS_UPDATE, PerfTest::RunTest, MPCTL,
};
static RegressionFramework ttfar7 = {
   AVC_DENIAL_TEST, PerfTest::RunTest, MPCTL,
};
static RegressionFramework ttfar8 = {
   PERF_GET_PROP_TEST, PerfTest::RunTest, MPCTL,
};
//Register Test here at end
