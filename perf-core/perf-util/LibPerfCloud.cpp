/*
 * Copyright (C) 2025 Transsion Holdings
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cutils/properties.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <tranlog/libtranlog.h>
#include "PerfLog.h"
#include "LibPerfCloud.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LIB-PERF-CLOUD"

static const char *POWER_CLDCTL_ID = "1001000034";
static const char *POWER_CLDCTL_VER = "v1.0";
static const char *POWER_FILE_TYPE = "f_type";
static const char *POWER_FILE_ENCODE =
    "f_encode";    // Indicates whether the configuration file is encrypted

static const int PERF_TARGET_RESOURCE_CONFIGS_MASK = 0b00000001;
static const int PERF_COMMON_RESOURCE_CONFIGS_MASK = 0b00000010;
static const int PERF_HINT_MASK = 0b00000100;
static const int PERF_BOOSTS_CONFIG_MASK = 0b00010000;
static const int PERF_MAPPING_MASK = 0b00100000;
static const int PERF_CONFIG_STORE_MASK = 0b01000000;
static int encodeSupport = 0;
static int cloudSupport = 0;

static const char *THERMAL_UX_TEMP_MAX_CLOUD_KEY = "thermal_ux_temp_max";
static const char *THERMAL_UX_TEMP_MIN_CLOUD_KEY = "thermal_ux_temp_min";
char *thermalUxTempMaxCloudValue = NULL;
char *thermalUxTempMinCloudValue = NULL;
#define THERMAL_UX_TEMP_MAX_CLOUD_PROP "persist.vendor.powerhal.thermal_ux_temp_max"
#define THERMAL_UX_TEMP_MIN_CLOUD_PROP "persist.vendor.powerhal.thermal_ux_temp_min"

/**
 * Update thermal UX temperature threshold properties
 * Sets the thermal UX temperature maximum and minimum values to system properties
 * if the corresponding cloud values are available
 */
static void UpdateThermalUxTempThreshold() {
    if (thermalUxTempMaxCloudValue) {
        property_set(THERMAL_UX_TEMP_MAX_CLOUD_PROP, thermalUxTempMaxCloudValue);
    }

    if (thermalUxTempMinCloudValue) {
        property_set(THERMAL_UX_TEMP_MIN_CLOUD_PROP, thermalUxTempMinCloudValue);
    }
}

/**
 * Check and retrieve integer value from system property
 * @param prop Property name to check
 * @return Integer value of the property, 0 if property is NULL or not found
 */
static int CheckPropertyValue(char *prop) {
    char propContent[PROPERTY_VALUE_MAX] = "\0";
    int propValue = 0;

    if (prop == NULL)
        return 0;

    property_get(prop, propContent, "0");
    propValue = atoi(propContent);

    return propValue;
}

/**
 * Copy file from source to destination with error handling
 * @param src Source file path
 * @param dest Destination file path
 * @return 0 on success, -1 on failure
 */
static int PowerCloudCopyFile(char *src, const char *dest) {
    int fd1, fd2;
    int fileSize, buffSize;
    int ret = -1;
    char *buff = NULL;

    fd1 = open(src, O_RDWR);
    if (fd1 < 0) {
        QLOGE(LOG_TAG, "open %s failed !", src);
        ret = -1;
        return ret;
    }

    fd2 = open(dest, O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd2 < 0) {
        QLOGE(LOG_TAG, "open %s failed !", dest);
        close(fd1);
        ret = -1;
        return ret;
    }

    ret = chmod(dest, 0664);
    if (ret < 0) {
        QLOGE(LOG_TAG, "chmod %s failed ! ret = %d", dest, ret);
        return ret;
    }

    fileSize = lseek(fd1, 0, SEEK_END);
    QLOGD(LOG_TAG, "fileSize = %d ", fileSize);
    lseek(fd1, 0, SEEK_SET);

    buff = (char *)malloc(SIZE_1_K_BYTES);
    if (buff == NULL) {
        QLOGE(LOG_TAG, "fpsmgr:buff malloc fail!");
        ret = -1;
        goto out;
    }

    while (fileSize > 0) {
        memset(buff, 0, sizeof(buff));
        if (fileSize > SIZE_1_K_BYTES) {
            buffSize = SIZE_1_K_BYTES;
        } else {
            buffSize = fileSize;
        }

        ret = read(fd1, buff, buffSize);
        if (ret < 0) {
            QLOGE(LOG_TAG, "read %s failed ! ret =%d", src, ret);
            break;
        }

        ret = write(fd2, buff, buffSize);
        if (ret < 0) {
            QLOGE(LOG_TAG, "write %s failed ! ret =%d", dest, ret);
            break;
        }

        fileSize -= SIZE_1_K_BYTES;
    }

    free(buff);
out:
    close(fd1);
    close(fd2);
    return ret;
}

/**
 * Generate source file path by combining directory path and file name
 * @param filePath Directory path
 * @param fileName File name to append
 * @return Allocated string containing full file path, NULL on failure
 */
static char *GetSrcFilePath(char *filePath, const char *fileName) {
    char *fileSrcPath = NULL;

    int length = strlen(filePath) + sizeof("/") + strlen(fileName) + sizeof("");
    fileSrcPath = (char *)malloc(length);
    if (fileSrcPath == NULL) {
        QLOGE(LOG_TAG, "alloc mem failed!!");
        goto ERROR;
    }

    sprintf(fileSrcPath, "%s/%s", filePath, fileName);
    fileSrcPath[length - 1] = '\0';

ERROR:
    return fileSrcPath;
}

/**
 * Perform file copy operation with validation
 * Checks if source file exists before copying to destination
 * @param srcPath Source file path (will be freed after use)
 * @param destPath Destination file path
 */
static void DoFileCopy(char *srcPath, char *destPath) {
    struct stat statBuf;

    if (!srcPath) {
        QLOGE(LOG_TAG, "path is null!");
        return;
    }

    if (!destPath) {
        QLOGE(LOG_TAG, "destPath is null!");
        free(srcPath);
        return;
    }

    if (0 == stat(srcPath, &statBuf)) {
        QLOGI(LOG_TAG, "srcPath =%s", srcPath);
        PowerCloudCopyFile(srcPath, destPath);
        free(srcPath);
    } else {
        free(srcPath);
    }
}

/**
 * Cloud control data callback function
 * Handles cloud configuration updates and file synchronization
 * @param key Configuration key (not used in current implementation)
 */
static void PowerCloudctlDataCallback(char *key) {
    char *content = getConfig(POWER_CLDCTL_ID);
    char *fileEncode = NULL;
    char *fileType = NULL;
    char *filePath = NULL;
    QLOGI(LOG_TAG, "content = %s", content);

    char *thermalUxTempMin = NULL;
    char *thermalUxTempMax = NULL;

    if (content) {
        /* update file */
        fileEncode = getString(content, POWER_FILE_ENCODE);
        QLOGI(LOG_TAG, "fileEncode = %s,encodeSupport = %d", fileEncode, encodeSupport);
        if (!fileEncode) {
            QLOGE(LOG_TAG, "fileEncode is fail!");
            goto OUT;
        }

        if (!strcmp(fileEncode, "Y") && !encodeSupport) {
            QLOGD(LOG_TAG, "project need not encodeSupport!");
            goto OUT;
        } else if (!strcmp(fileEncode, "N") && encodeSupport) {
            QLOGD(LOG_TAG, "project need encodeSupport!");
            goto OUT;
        }

        fileType = getString(content, POWER_FILE_TYPE);
        if (!fileType) {
            QLOGE(LOG_TAG, "fileType is fail!");
            goto OUT;
        }

        int fileNum = atoi(fileType);
        QLOGI(LOG_TAG, "fileNum = %d", fileNum);

        filePath = getFilePath(POWER_CLDCTL_ID);
        if (!filePath) {
            QLOGE(LOG_TAG, "filePath is fail!");
            goto OUT;
        }

        QLOGI(LOG_TAG, "filepath =%s", filePath);

        if (fileNum & PERF_CONFIG_STORE_MASK) {
            char *srcPerfConfigStoreFilePath = GetSrcFilePath(filePath, PERF_CONFIG_STORE_FILE);
            DoFileCopy(srcPerfConfigStoreFilePath, DATA_VENDOR_PERF_CONFIG_STORE_FILE);
        }

        if (fileNum & PERF_TARGET_RESOURCE_CONFIGS_MASK) {
            char *srcPerfTargetResourceConfigsFilePath =
                GetSrcFilePath(filePath, PERF_TARGET_RESOURCE_CONFIGS_FILE);
            DoFileCopy(srcPerfTargetResourceConfigsFilePath,
                       DATA_VENDOR_PERF_TARGET_RESOURCE_CONFIGS_FILE);
        }

        if (fileNum & PERF_COMMON_RESOURCE_CONFIGS_MASK) {
            char *srcPerfCommonResourceConfigsFilePath =
                GetSrcFilePath(filePath, PERF_COMMON_RESOURCE_CONFIGS_FILE);
            DoFileCopy(srcPerfCommonResourceConfigsFilePath,
                       DATA_VENDOR_PERF_COMMON_RESOURCE_CONFIGS_FILE);
        }

        if (fileNum & PERF_HINT_MASK) {
            char *srcPerfHintFilePath = GetSrcFilePath(filePath, PERF_HINT_FILE);
            DoFileCopy(srcPerfHintFilePath, DATA_VENDOR_PERF_HINT_FILE);
        }

        if (fileNum & PERF_BOOSTS_CONFIG_MASK) {
            char *srcPerfBoostsConfigFilePath = GetSrcFilePath(filePath, PEFF_BOOSTS_CONFIG_FILE);
            DoFileCopy(srcPerfBoostsConfigFilePath, DATA_VENDOR_PEFF_BOOSTS_CONFIG_FILE);
        }

        if (fileNum & PERF_MAPPING_MASK) {
            char *srcPerfMappingFilePath = GetSrcFilePath(filePath, PERF_MAPPING_FILE);
            DoFileCopy(srcPerfMappingFilePath, DATA_VENDOR_PERF_MAPPING_FILE);
        }

        /* update thermal_ux policy temp Threshold*/
        thermalUxTempMax = getString(content, THERMAL_UX_TEMP_MAX_CLOUD_KEY);
        if (!thermalUxTempMax) {
            QLOGE(LOG_TAG, "get thermal ux temp is fail!");
            goto OUT;
        }
        thermalUxTempMaxCloudValue = thermalUxTempMax;
        QLOGI(LOG_TAG, "thermalUxTemp max = %s", thermalUxTempMaxCloudValue);

        thermalUxTempMin = getString(content, THERMAL_UX_TEMP_MIN_CLOUD_KEY);
        if (!thermalUxTempMin) {
            QLOGE(LOG_TAG, "get thermal ux temp is fail!");
            goto OUT;
        }
        thermalUxTempMinCloudValue = thermalUxTempMin;
        QLOGI(LOG_TAG, "thermalUxTemp min = %s", thermalUxTempMinCloudValue);

        UpdateThermalUxTempThreshold();
    }

    libpowerhal_wrap_ReInit(1);

OUT:
    if (fileType) {
        free(fileType);
    }

    if (filePath) {
        free(filePath);
    }

    if (fileEncode) {
        free(fileEncode);
    }

    if (content) {
        free(content);
    }

    if (thermalUxTempMin) {
        free(thermalUxTempMin);
    }

    if (thermalUxTempMax) {
        free(thermalUxTempMax);
    }

    feedBack(POWER_CLDCTL_ID, 1);
}

static struct config_notify powerCloudctlNotify = {.notify = PowerCloudctlDataCallback};

/**
 * Register cloud control listener
 * Initializes cloud and encode support flags and starts the listener
 * for power cloud control configuration updates
 */
void RegistCloudctlListener() {
    // cloudSupport = CheckPropertyValue(TRAN_POWERHAL_CLOUD_PROP);
    // encodeSupport = CheckPropertyValue(TRAN_POWERHAL_ENCODE_PROP);
    cloudSupport = 1;
    encodeSupport = 1;

    if (!cloudSupport) {
        QLOGD(LOG_TAG, "powerhal doesn't support cloudEngine");
        return;
    }

    // int ret = startListener(&powerCloudctlNotify, POWER_CLDCTL_ID, POWER_CLDCTL_VER);
    int ret = 1;
    if (ret == 0) {
        QLOGE(LOG_TAG, "RegistCloudctlListener failed!");
    }
}

/**
 * Unregister cloud control listener
 * Stops the cloud control listener if cloud support is enabled
 */
void UnregistCloudctlListener() {
    if (!cloudSupport) {
        QLOGD(LOG_TAG, "powerhal doesn't support cloudEngine");
        return;
    }

    // stopListener(POWER_CLDCTL_ID);
}
