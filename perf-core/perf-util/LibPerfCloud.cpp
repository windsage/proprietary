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

#include "LibPerfCloud.h"

#include <cutils/properties.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <tranlog/libtranlog.h>
#include <unistd.h>

#include "PerfLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LIB-PERF-CLOUD"

// Cloud control configuration
static const char *PERF_CLDCTL_ID = "1008000048";
static const char *PERF_CLDCTL_VER = "v1.0";
static const char *PERF_FILE_TYPE = "f_type";

static const int PERF_CONFIG_STORE_MASK = 0b001;     // 1 - perfconfigstore.xml
static const int PERF_BOOSTS_CONFIG_MASK = 0b010;    // 2 - perfboostsconfig.xml
static const int PERF_CONFIG_MASK = 0b100;           // 4 - perf_config.xml

static int cloudSupport = 0;

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
static int PerfCloudCopyFile(char *src, const char *dest) {
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
        close(fd1);
        close(fd2);
        return ret;
    }

    fileSize = lseek(fd1, 0, SEEK_END);
    QLOGD(LOG_TAG, "fileSize = %d ", fileSize);
    lseek(fd1, 0, SEEK_SET);

    buff = (char *)malloc(SIZE_1_K_BYTES);
    if (buff == NULL) {
        QLOGE(LOG_TAG, "buff malloc fail!");
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
 * Generate source file path by combining base path and filename
 * @param filePath Base directory path
 * @param fileName Target filename
 * @return Allocated string containing full source path, caller must free
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
 * Copy configuration file from source to destination if source exists
 * @param srcPath Source file path (will be freed by this function)
 * @param destPath Destination file path
 */
static void DoFileCopy(char *srcPath, const char *destPath) {
    struct stat statBuf;

    if (!srcPath) {
        QLOGE(LOG_TAG, "srcPath is null!");
        return;
    }

    if (!destPath) {
        QLOGE(LOG_TAG, "destPath is null!");
        free(srcPath);
        return;
    }

    if (0 == stat(srcPath, &statBuf)) {
        QLOGI(LOG_TAG, "srcPath =%s", srcPath);
        PerfCloudCopyFile(srcPath, destPath);
        free(srcPath);
    } else {
        QLOGE(LOG_TAG, "srcPath %s not found", srcPath);
        free(srcPath);
    }
}

/**
 * Copy perf_config.xml to both vendor and data paths
 * @param srcPath Source file path (will be freed by this function)
 */
static void DoPerfConfigFileCopy(char *srcPath) {
    struct stat statBuf;

    if (!srcPath) {
        QLOGE(LOG_TAG, "srcPath is null!");
        return;
    }

    if (0 == stat(srcPath, &statBuf)) {
        QLOGI(LOG_TAG, "srcPath =%s", srcPath);
        // Copy to data vendor path
        PerfCloudCopyFile(srcPath, DATA_VENDOR_PERF_CONFIG_FILE);
        // Also copy to vendor etc path if needed
        PerfCloudCopyFile(srcPath, "/vendor/etc/perf/perf_config.xml");
        free(srcPath);
    } else {
        QLOGE(LOG_TAG, "srcPath %s not found", srcPath);
        free(srcPath);
    }
}

/*
The file mask corresponds to the XML configuration files to be updated:
perf_config boosts configstore
    0        0       1   //=1 - perfconfigstore.xml
    0        1       0   //=2 - perfboostsconfig.xml
    1        0       0   //=4 - perf_config.xml
........................
    1        1       1   //=7 - All files

Example cloud command:
{"v":"v1.0","m":"a","t":"20201123","e":false,"f":"http://cdn.shalltry.com/public/OSFeature_test/file/1605096764001.zip","f_type":"5"}

f_type="5" means: perfconfigstore(1) + perf_config(4) = 5
f_type="7" means: perfconfigstore(1) + perfboostsconfig(2) + perf_config(4) = 7
*/

/**
 * Cloud control data callback function
 * Handles cloud configuration updates and XML file synchronization
 * @param key Configuration key (not used in current implementation)
 */
static void PerfCloudctlDataCallback(char *key) {
    char *content = getConfig(PERF_CLDCTL_ID);
    char *fileType = NULL;
    char *filePath = NULL;

    QLOGI(LOG_TAG, "content = %s", content);

    if (content) {
        /* Get file type mask */
        fileType = getString(content, PERF_FILE_TYPE);
        if (!fileType) {
            QLOGE(LOG_TAG, "fileType is fail!");
            goto OUT;
        }

        int fileNum = atoi(fileType);
        QLOGI(LOG_TAG, "fileNum = %d", fileNum);

        /* Get download file path */
        filePath = getFilePath(PERF_CLDCTL_ID);
        if (!filePath) {
            QLOGE(LOG_TAG, "filePath is fail!");
            goto OUT;
        }

        QLOGI(LOG_TAG, "filePath = %s", filePath);

        /* Process each XML file based on mask - only 3 files needed */
        if (fileNum & PERF_CONFIG_STORE_MASK) {
            char *srcConfigStorePath = GetSrcFilePath(filePath, PERF_CONFIG_STORE_FILE);
            DoFileCopy(srcConfigStorePath, DATA_VENDOR_PERF_CONFIG_STORE_FILE);
        }

        if (fileNum & PERF_BOOSTS_CONFIG_MASK) {
            char *srcBoostsPath = GetSrcFilePath(filePath, PEFF_BOOSTS_CONFIG_FILE);
            DoFileCopy(srcBoostsPath, DATA_VENDOR_PEFF_BOOSTS_CONFIG_FILE);
        }

        if (fileNum & PERF_CONFIG_MASK) {
            char *srcPerfConfigPath = GetSrcFilePath(filePath, PERF_CONFIG_FILE);
            DoPerfConfigFileCopy(srcPerfConfigPath);
        }
    }

OUT:
    if (fileType) {
        free(fileType);
    }

    if (filePath) {
        free(filePath);
    }

    if (content) {
        free(content);
    }

    feedBack(PERF_CLDCTL_ID, 1);
}

static struct config_notify perfCloudctlNotify = {.notify = PerfCloudctlDataCallback};

/**
 * Register cloud control listener for perf configuration updates
 */
void RegistCloudctlListener() {
    cloudSupport = CheckPropertyValue((char *)TRAN_PERF_CLOUD_PROP);

    if (!cloudSupport) {
        QLOGD(LOG_TAG, "perf doesn't support cloudEngine");
        return;
    }

    int ret = startListener(&perfCloudctlNotify, PERF_CLDCTL_ID, PERF_CLDCTL_VER);
    if (ret == 0) {
        QLOGE(LOG_TAG, "RegistCloudctlListener failed!");
    } else {
        QLOGI(LOG_TAG, "RegistCloudctlListener success");
    }
}

/**
 * Unregister cloud control listener
 */
void UnregistCloudctlListener() {
    if (!cloudSupport) {
        QLOGD(LOG_TAG, "perf doesn't support cloudEngine");
        return;
    }

    stopListener(PERF_CLDCTL_ID);
    QLOGI(LOG_TAG, "UnregistCloudctlListener success");
}
