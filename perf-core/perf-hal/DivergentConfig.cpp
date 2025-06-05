/******************************************************************************
  @file    DivergentConfig.cpp
  @brief   Implementation of DivergentConfig

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-DIVERGENTCONFIG"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <algorithm>
#include "TargetConfig.h"
#include "PerfLog.h"
#include "DivergentConfig.h"

#define POLICY_DIR_NAME_LEN 6
#define DIVERGENT_NUM_BIT_LEN 4

DivergentConfig DivergentConfig::divergent;

static bool dirExists(const char *dirPath) {
    if (dirPath == NULL) {
        return false;
    }

    DIR* dir = opendir(dirPath);
    if (dir != nullptr) {
        closedir(dir);
        return true;
    }
    return false;
}

static void readValueFromFile(int32_t fd, char* val) {
    if (val == NULL) {
        QLOGE(LOG_TAG, "Invalid file name");
        return;
    }
    int32_t valLength = read(fd, val, sizeof(val) - 1);

    if (valLength <= 0) {
        val = NULL;
        QLOGE(LOG_TAG, "Failed to read");
        return;
    }

    val[valLength] = '\0';
}

static int32_t convertStrValToInt(char *val) {
    if (val == NULL) {
        return -1;
    }
    return static_cast<int32_t>(strtol(val, &val, 10));
}

static void appendToPath(const char *subsystem, char *result, size_t resultSize) {
    if (subsystem == NULL || result == NULL) {
        return;
    }

    if (resultSize < strlen(SUBSYSTEM_AVAILABILITY_CHECK_NODE) +
                     strlen(subsystem) + 1) {

        result[0] = '\0';
        return;
    }

    strlcpy(result, SUBSYSTEM_AVAILABILITY_CHECK_NODE, resultSize);
    strlcat(result, subsystem, resultSize);
}

void DivergentConfig::readNumClusters() {
    const char *sysfsDir = POLICY_DIR_PATH;

    if (!dirExists(sysfsDir)) {
        QLOGE(LOG_TAG, "Directory does not exist");
        return;
    }

    DIR* dir = opendir(sysfsDir);
    if (!dir) {
        QLOGE(LOG_TAG, "Failed to open directory %s", sysfsDir);
        return;
    }

    struct dirent* entry;
    char *ptr;
    uint8_t policy_id;
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "policy", POLICY_DIR_NAME_LEN) == 0) {
            ptr = entry->d_name + POLICY_DIR_NAME_LEN;
            policy_id = static_cast<int8_t>(strtol(ptr, &ptr, 10));
            if (mNumCluster < MAX_CLUSTER) {
                mFirstCorePerCluster[mNumCluster] = policy_id;
                mNumCluster++;
            } else {
                QLOGE(LOG_TAG, "Invalid cluster!");
                break;
            }
        }
    }
    closedir(dir);
}

void DivergentConfig::readPerClusterInfo() {
    if (mClusterInfo == NULL)
        return;

    for (uint8_t i = 0; i < mNumCluster; i++) {
        char filename[NODE_MAX];
        snprintf(filename, sizeof(filename), RELATED_CPUS_NODE_PATH,
                 mFirstCorePerCluster[i]);

        FILE* fd = fopen(filename, "r");
        if (fd == NULL) {
            QLOGE(LOG_TAG, "Failed to open file: %s", filename);
            return;
        }

        clusterInfo info;
        info.numCores = 0;
        int32_t coreNum;

        while (fscanf(fd, "%" PRId32, &coreNum) == 1) {
            if (info.numCores == 0) {
                info.firstCore = static_cast<uint8_t>(coreNum);
                info.numCores++;
                continue;
            }
            info.numCores++;
        }
        info.lastCore = coreNum;

        if (mClusterInfo != nullptr) {
            mClusterInfo[i] = info;
        }

        mTotalNumCores += info.numCores;
        fclose(fd);
    }
}

void DivergentConfig::readCapacityPerCluster() {
    if (mClusterInfo == NULL)
        return;

    for (uint8_t i = 0; i < mNumCluster; i++) {
        char filename[NODE_MAX] = {};
        snprintf(filename, NODE_MAX, CPU_CAPACITY_NODE_PATH,
                 mClusterInfo[i].firstCore);

        int32_t fd = open(filename, O_RDONLY);
        if (fd == -1) {
            QLOGE(LOG_TAG, "Failed to open file: %s", filename);
            return;
        }

        char line[NODE_MAX] = {};
        readValueFromFile(fd, line);
        int32_t capacity = convertStrValToInt(line);

        mClusterInfo[i].capacity = capacity;
        close(fd);
    }
}

void DivergentConfig::readPhysicalCluster() {
    if (mClusterInfo == NULL)
        return;

    for (uint8_t i = 0; i < mNumCluster; i++) {
        char filename[NODE_MAX] = {};
        snprintf(filename, NODE_MAX, PHYSICAL_PKG_ID_NODE_PATH,
                 mClusterInfo[i].firstCore);

        int32_t fd = open(filename, O_RDONLY);
        if (fd == -1) {
            QLOGE(LOG_TAG, "Failed to open file: %s", PHYSICAL_PKG_ID_NODE_PATH);
            return;
        }

        char line[NODE_MAX] = {};
        readValueFromFile(fd, line);
        uint8_t physicalCluster = static_cast<uint8_t>(convertStrValToInt(line));

        mClusterInfo[i].physicalClusterId = physicalCluster;
        close(fd);
    }
}

void DivergentConfig::determineGovernorInstType() {
    if (mNumCluster == 1)
        mGovInstanceType = SINGLE_GOV_INSTANCE;
}


bool isSubSystemEnabled(const char* subsystem) {
    if (subsystem == NULL) {
        return false;
    }

    char filename[NODE_MAX] = {};
    memset(filename, 0, sizeof(filename));

    appendToPath(subsystem, filename, sizeof(filename));
    if (filename[0] != '\0') {
        QLOGI(LOG_TAG, "filename: %s", filename);
    } else {
        QLOGE(LOG_TAG, "Buffer was too small to hold the result");
    }

    int32_t fd = open(filename, O_RDONLY);

    if (fd < 0) {
        QLOGE(LOG_TAG, "Failed to open file: %s", filename);
        return false;
    }

    char buffer[NODE_MAX] = {};
    int32_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytesRead <= 0) {
        QLOGE(LOG_TAG, "Couldn't read the file: %s", filename);
        return false;
    }

    buffer[bytesRead] = '\0';

    char* ptr = buffer;
    while(*ptr) {
        char *endPtr;
        uint8_t value = static_cast<uint8_t>(strtoul(ptr, &endPtr, 16));

        if (ptr == endPtr) {
            return false; // conversion failed
        }

        // check if any bit is 0
        if ((value & 1) == 0) {
             return true;
        }

        // Move to the next comma separated value
        ptr = endPtr;
        if (*ptr == ',') {
            ++ptr;
        }
    }
    return true;
}

void DivergentConfig::DumpAll() {
    if (mClusterInfo == NULL) {
        QLOGE(LOG_TAG, "Failed initializing divergent config");
        return;
    }

    for (uint8_t i = 0; i < mNumCluster; i++) {
        QLOGI(LOG_TAG, "physicalClusterId for cluster %" PRId8 " = %" PRId8, i, mClusterInfo[i].physicalClusterId);
        QLOGI(LOG_TAG, "first core for cluster %" PRId8 " = %" PRId8, i, mClusterInfo[i].firstCore);
        QLOGI(LOG_TAG, "last core for cluster %" PRId8 " = %" PRId8, i, mClusterInfo[i].lastCore);
        QLOGI(LOG_TAG, "number of cores in cluster %" PRId8 " = %" PRId8, i, mClusterInfo[i].numCores);
        QLOGI(LOG_TAG, "capacity of cluster %" PRId8 " = %" PRId8, i, mClusterInfo[i].capacity);
    }

    QLOGI(LOG_TAG, "Divergent Number = 0x%x", divergentNumber);
    QLOGI(LOG_TAG, "isDisplayEnabled = %" PRId8, isDisplayEnabled);
    QLOGI(LOG_TAG, "isGpuEnabled = %" PRId8, isGpuEnabled);
}

DivergentConfig::DivergentConfig() : mGovInstanceType(CLUSTER_BASED_GOV_INSTANCE) {
    readNumClusters();
    if (mNumCluster > 0)
        mClusterInfo = new clusterInfo[mNumCluster];

    readPerClusterInfo();
    readCapacityPerCluster();
    readPhysicalCluster();

    if (mClusterInfo != NULL)
        sort(mClusterInfo, mClusterInfo + mNumCluster);

    determineGovernorInstType();

    GenerateDivergentNumber();

    isDisplayEnabled = isSubSystemEnabled("display");
    isGpuEnabled = isSubSystemEnabled("gpu");
    DumpAll();
}

DivergentConfig::~DivergentConfig() {
    if (mClusterInfo != NULL) {
        delete[] mClusterInfo;
        mClusterInfo = NULL;
    }
}

uint8_t DivergentConfig::getCoresInCluster(uint8_t physicalClusterId) {
    if (mClusterInfo == NULL)
        return 0;

    for (uint8_t i = 0; i < mNumCluster; i++) {
        if (mClusterInfo[i].physicalClusterId == physicalClusterId) {
            return mClusterInfo[i].numCores;
        }
    }

    return 0;
}

void DivergentConfig::GenerateDivergentNumber() {
    if (mClusterInfo == NULL)
        return;

    divergentNumber = divergentNumber | mNumCluster;
    for (uint8_t i = 0; i < mNumCluster; i++) {
        divergentNumber = divergentNumber | ((mClusterInfo[i].numCores <<
                                             (DIVERGENT_NUM_BIT_LEN * (i + 1))));
    }
    QLOGE(LOG_TAG, "Divergent Number = %x", divergentNumber);
}

bool DivergentConfig::checkClusterPresent(uint8_t cluster) {
    for (uint8_t i = 0; i < mNumCluster; i++) {
        if (getPhysicalCluster(i) == cluster) {
            return true;
        }
    }
    return false;
}

