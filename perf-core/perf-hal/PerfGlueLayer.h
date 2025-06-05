/******************************************************************************
  @file  PerfGlueLayer.h
  @brief  Perf Hal glue layer interface

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef __PERF_GLUE_LAYER_H__
#define __PERF_GLUE_LAYER_H__


#include <pthread.h>
#include <string>
#include <vector>

#define MAX_MODULES 4
#define MAX_EVENTS 48
#define MIN_EVENTS 2
#define MAX_FILE_NAME 128

class EventQueue;
struct mpctl_msg_t;

typedef struct ModuleLibInfo {
    bool is_opened;
    char mLibFileName[MAX_FILE_NAME];
    void *dlhandle;
    int32_t  (*Init)(void);
    void (*Exit)(void);
    int32_t (*SubmitRequest)(mpctl_msg_t*);
    void (*SyncRequest)(int32_t, char **);
} ModuleLibInfo;

class PerfModule {
protected:
    bool mRegistered;
    int32_t mEventsLowerBound;
    int32_t mEventsUpperBound;
    uint8_t mNumEvents;
    int32_t mEvents[MAX_EVENTS];
    ModuleLibInfo mLibHandle;

public:
    PerfModule();
    ~PerfModule();
    int32_t LoadPerfLib(const char *libname);
    void UnloadPerfLib();
    bool IsThisEventRegistered(int32_t event);
    int16_t Register(const char *libname, int32_t *events, int32_t numevents);
    bool Deregister();
    void Dump();

    bool IsEmpty() {
        return !mRegistered;
    }

    ModuleLibInfo &GetLib() {
        return mLibHandle;
    }
};

class PerfGlueLayer {
protected:
    static PerfModule mModules[MAX_MODULES];
    static std::vector<std::string> mLoadedModules;
    static PerfModule *mMpctlMod;
    static pthread_mutex_t mMutex;

    bool Deregister(int32_t handle);
    int16_t Register(const char *libname, int32_t *events, int32_t numevents);
public:
    PerfGlueLayer();

    //events need to be passed as an array, first two elements should contain range
    //of the events interested in (if not interested in range, -1, -1 should be specified),
    //next elements should contain individual events, max is limited by MAX_EVENTS
    //numevents should be first two elements (range) + number of next elements
    PerfGlueLayer(const char *libname, int32_t *events, int32_t numevents);
    int32_t LoadPerfLib(const char *libname);

    ~PerfGlueLayer();

    //interface to be exposed for HAL
    int32_t PerfGlueLayerSubmitRequest(mpctl_msg_t *msg);
    int32_t PerfGlueLayerInit();
    int32_t PerfGlueLayerExit();
    std::string PerfGlueLayerSyncRequest(int32_t cmd);
};

#endif
