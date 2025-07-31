/******************************************************************************
  @file    PerfConfig.cpp
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-CONFIG"
#define PERF_CONFIG_STORE_ROOT "PerfConfigsStore"
#define PERF_CONFIG_STORE_CHILD "PerfConfigs"
#define PERF_CONFIG_STORE_PROP "Prop"
#define PERF_CONFIG_STORE_NAME "Name"
#define PERF_CONFIG_STORE_VALUE "Value"
#define PERF_CONFIG_STORE_TARGET "Target"
#define PERF_CONFIG_STORE_TARGET_VARIANT "Variant"
#define PERF_CONFIG_STORE_KERNEL "Kernel"
#define PERF_CONFIG_STORE_RESOLUTION "Resolution"
#define PERF_CONFIG_STORE_SKEW_TYPE "SkewType"
#define PERF_CONFIG_STORE_RAM "Ram"
#define FALSE_STR "false"

#include "PerfLog.h"
#include "PerfConfig.h"
#include "ConfigFileManager.h"
#include <unistd.h>
#include <cutils/properties.h>
#include "MpctlUtils.h"
#include <config.h>
#include <dlfcn.h>

#define OBFUSCATION_LIB_NAME "libskewknob.so"

#define MAP_RES_TYPE_VAL_1080p "1080p"
#define MAP_RES_TYPE_VAL_2560 "2560"
#define MAP_RES_TYPE_VAL_720p "720p"
#define MAP_RES_TYPE_VAL_HD_PLUS "HD+"
using namespace std;

PerfConfigDataStore PerfConfigDataStore::mPerfStorage;
feature_knob_func_ptr PerfConfigDataStore::mFeature_knob_func = NULL;
std::atomic_bool PerfConfigDataStore::mPerfConfigInit = false;

PerfConfigDataStore::PerfConfigDataStore() {
}

PerfConfigDataStore::~PerfConfigDataStore() {
    //delete mappings
    mPerfConfigStore.clear();
}

ValueMapResType
PerfConfigDataStore::ConvertToEnumResolutionType(char *res) {
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

//perf config store xml CB
void PerfConfigDataStore::PerfConfigStoreCB(xmlNodePtr node, void *) {
    char *mName = NULL, *mValue = NULL, *mTarget = NULL, *mKernel = NULL,
         *mResolution = NULL, *mRam = NULL, *mVariant = NULL, *mSkewType = NULL;
    PerfConfigDataStore &store = PerfConfigDataStore::getPerfDataStore();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    const char *target_name = tc.getTargetName().c_str();
    const char *kernelVersion = tc.getFullKernelVersion().c_str();
    const char *target_name_variant = tc.getVariant().c_str();
    uint32_t tc_resolution = tc.getResolution();
    uint32_t tc_ram = tc.getRamSize();
    uint32_t res = 0, ram = 0;
    bool valid_target = true, valid_kernel = true, valid_ram = true,
         valid_resolution = true, valid_target_variant = true;
    char trace_prop[PROPERTY_VALUE_MAX];

    /* Enable traces by adding vendor.debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }
    if(!xmlStrcmp(node->name, BAD_CAST PERF_CONFIG_STORE_PROP)) {
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_NAME)) {
              mName = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_NAME);
        } else {
              QLOGL(LOG_TAG, QLOG_WARNING, "Property not found Name=%s", mName ? mName : "NULL");
        }

        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_VALUE)) {
              mValue = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_VALUE);
        } else {
              QLOGL(LOG_TAG, QLOG_WARNING, "Property not found Name=%s", mValue ? : "NULL");
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET)) {
            mTarget = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET);
            if (mTarget != NULL) {
                valid_target = false;
                char *pos = NULL;
                char *tname_token = strtok_r(mTarget, ",", &pos);
                while(tname_token != NULL) {
                    if((strlen(tname_token) == strlen(target_name)) and (!strncmp(target_name, tname_token,strlen(target_name)))) {
                    valid_target = true;
                        break;
                    }
                    tname_token = strtok_r(NULL, ",", &pos);
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET_VARIANT)) {
            mVariant = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET_VARIANT);
            if (mVariant != NULL) {
                    if(((strlen(mVariant) == strlen(target_name_variant)) and (!strncmp(target_name_variant, mVariant,strlen(mVariant))))) {
                        valid_target_variant = true;
                    } else {
                        valid_target_variant = false;
                    }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_KERNEL)) {
            mKernel = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_KERNEL);
            if (mKernel != NULL) {
                if((strlen(mKernel) == strlen(kernelVersion)) and  !strncmp(kernelVersion, mKernel, strlen(mKernel)) ) {
                    valid_kernel = true;
                } else {
                    valid_kernel = false;
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_RESOLUTION)) {
            mResolution = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_RESOLUTION);
            if (mResolution != NULL) {
                res = store.ConvertToEnumResolutionType(mResolution);
                if (res == tc_resolution) {
                    valid_resolution = true;
                } else {
                    valid_resolution = false;
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_RAM)) {
            mRam = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_RAM);
            if (mRam != NULL) {
                ram = atoi(mRam);
                if (ram == tc_ram) {
                    valid_ram = true;
                } else {
                    valid_ram = false;
                }
            }
        }

        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_SKEW_TYPE)) {
            mSkewType = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_SKEW_TYPE);
            if (mSkewType != NULL) {
                bool skewRetVal = false;
                int32_t skewType = atoi(mSkewType);
                if(mFeature_knob_func) {
                    skewRetVal = mFeature_knob_func(skewType);
                }

                if (!skewRetVal) {
                    QLOGL(LOG_TAG,  QLOG_WARNING, "Defaulting config prop %s because of skew mismatch to false",
                                mName ? mName : "NULL");
                    if (mValue) {
                        xmlFree(mValue);
                        mValue = NULL;
                        mValue = (char *) xmlMalloc(sizeof(FALSE_STR) * sizeof(char));
                        if (mValue) {
                            memset(mValue,0,sizeof(FALSE_STR));
                            strlcpy(mValue, FALSE_STR,  sizeof(FALSE_STR));
                        }
                    }

                }
            }
        }
        QLOGL(LOG_TAG, QLOG_L1, "Identified Name=%s Value=%s for PerfConfigStore in table", mName ? mName : "NULL", mValue ? mValue : "NULL");

        if (mName != NULL and mValue != NULL) {
            if ((valid_kernel and valid_target and valid_target_variant and valid_resolution and valid_ram)) {
                UpdatePerfConfig(mName, mValue);
            } else if (!mTarget and !mVariant and !mKernel and !mResolution and !mRam) {
                try {
                    store.mPerfConfigStore.insert_or_assign(mName, mValue);
                } catch (std::exception &e) {
                    QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
                } catch (...) {
                    QLOGE(LOG_TAG, "Exception caught in %s", __func__);
                }
            }
        }

        if(mName)
             xmlFree(mName);
        if(mValue)
             xmlFree(mValue);
        if(mTarget)
             xmlFree(mTarget);
        if(mVariant)
            xmlFree(mVariant);
        if(mKernel)
             xmlFree(mKernel);
        if(mResolution)
             xmlFree(mResolution);
        if(mRam)
            xmlFree(mRam);
        if(mSkewType)
            xmlFree(mSkewType);
    }
    return;
}

void PerfConfigDataStore::UpdatePerfConfig(char *name, char *value) {
    PerfConfigDataStore &store = PerfConfigDataStore::getPerfDataStore();

    if (NULL == name or NULL == value) {
        return;
    }

    try {
        store.mPerfConfigStore.insert_or_assign(name, value);
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

void PerfConfigDataStore::ConfigStoreInit() {
    int8_t idnum;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }
    std::string configContent;
    std::string configPath = ConfigFileManager::getConfigFilePath("perfconfigstore.xml");
    if (ConfigFileManager::readAndDecryptConfig(configPath, configContent)) {
        // 加载obfuscation库
        void *handle = dlopen(OBFUSCATION_LIB_NAME, RTLD_LAZY);
        if (NULL != handle) {
            mFeature_knob_func = (feature_knob_func_ptr)dlsym(handle, "IsFeatureEnabled");
        }

        const string xmlPerfConfigRoot(PERF_CONFIG_STORE_ROOT);
        const string xmlPerfConfigChild(PERF_CONFIG_STORE_CHILD);

        idnum = xmlParser->Register(xmlPerfConfigRoot, xmlPerfConfigChild, PerfConfigStoreCB, NULL);
        xmlParser->ParseFromMemory(configContent);  // 使用内存解析
        xmlParser->DeRegister(idnum);

        if (NULL != handle) {
            mFeature_knob_func = NULL;
            dlclose(handle);
        }
    } else {
        QLOGE(LOG_TAG, "Failed to read config file: %s", configPath.c_str());
    }
    delete xmlParser;
    mPerfConfigInit = true;
    return;
}

char* PerfConfigDataStore::GetProperty(const char *name, char *value, const int32_t value_length) {
    if (NULL == name or NULL == value) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Couldn't return property, no space");
        return NULL;
    }

    if (!mPerfConfigInit) {
        QLOGE(LOG_TAG, "Perf Config Init has not completed");
        return NULL;
    }

    bool prop_found = false;

    try {
        std::string strName = std::string(name);
        auto it = mPerfConfigStore.find(strName);
        if (it != mPerfConfigStore.end()) {
            strlcpy(value, (it->second).c_str(), value_length);
            prop_found = true;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }

    if (!prop_found) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Property %s not found", name);
        return NULL;
    }
    return value;
}

int32_t PerfConfigDataStore::getProps(unordered_map<std::string, std::string> &configstore) {
    int32_t rc = FAILED;

    try {
        configstore = mPerfConfigStore;
        rc = SUCCESS;
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}
