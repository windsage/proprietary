/******************************************************************************
  @file    PerfConfig.h
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_PERF_CONFIG_H
#define VENDOR_PERF_CONFIG_H

#include <unordered_map>
#include "PerfController.h"
#include "XmlParser.h"
#include "TargetConfig.h"

typedef int32_t (*feature_knob_func_ptr)(int32_t);

using namespace std;
class PerfConfigDataStore {

private:
    //perf config store
    unordered_map<std::string, std::string> mPerfConfigStore;

    // Singelton object of this class
    static PerfConfigDataStore mPerfStorage;
    static feature_knob_func_ptr mFeature_knob_func;
public:
    static PerfConfigDataStore &getPerfDataStore() {
        return mPerfStorage;
    }

private:
    //perf config store xml CB
    static void PerfConfigStoreCB(xmlNodePtr node, void *);
    //ctor, copy ctor, assignment overloading
    PerfConfigDataStore();
    PerfConfigDataStore(PerfConfigDataStore const& oh);
    PerfConfigDataStore& operator=(PerfConfigDataStore const& oh);
    //Update property value based on target/kernel
    static void UpdatePerfConfig(char *name, char *value);
public:
    char* GetProperty(const char *name, char *value, const int32_t value_length);
    ValueMapResType ConvertToEnumResolutionType(char *res);
    void ConfigStoreInit();
    int32_t getProps(unordered_map<std::string, std::string> &configstore);
    ~PerfConfigDataStore();
    static std::atomic_bool mPerfConfigInit;
};
#endif  // VENDOR_PERF_CONFIG_H
