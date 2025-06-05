/******************************************************************************
  @file  PerfGlueLayer.cpp
  @brief  PerfGlueLayer glues modules to the framework which needs perf events

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-GLUELAYER"
#include "PerfLog.h"
#include <cutils/properties.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <algorithm>
#include "PerfGlueLayer.h"
#include "PerfController.h"
#include "PerfThreadPool.h"
#include "TargetConfig.h"
#include <config.h>

using namespace std;

PerfModule PerfGlueLayer::mModules[MAX_MODULES];
vector<string> PerfGlueLayer::mLoadedModules;
PerfModule *PerfGlueLayer::mMpctlMod = NULL;
pthread_mutex_t PerfGlueLayer::mMutex = PTHREAD_MUTEX_INITIALIZER;

PerfGlueLayer::PerfGlueLayer() {
}

PerfGlueLayer::PerfGlueLayer(const char *libname, int32_t *events, int32_t numevents) {
    Register(libname, events, numevents);
}

PerfGlueLayer::~PerfGlueLayer() {
}

//interface for modules to be glued to this layer
int16_t PerfGlueLayer::Register(const char *libname, int32_t *events, int32_t numevents) {
    int16_t ret = -1, len = 0;
    bool exists = false;

    if ((NULL == libname) || (NULL == events)) {
        QLOGE(LOG_TAG, "registration of perfmodule with perf-hal unsuccessful: no lib or events");
        return ret;
    }

    pthread_mutex_lock(&mMutex);
    //check if already registered
    for (uint8_t i=0; i<MAX_MODULES; i++) {
        len = strlen(mModules[i].GetLib().mLibFileName);
        if (!mModules[i].IsEmpty() &&
            !strncmp(mModules[i].GetLib().mLibFileName, libname, len)) {
            exists = true;
            break;
        }
    }

    if (exists) {
        pthread_mutex_unlock(&mMutex);
        QLOGL(LOG_TAG, QLOG_WARNING, "Register: already registered:%s", libname);
        return ret;
    }

    //now get first available node
    for (uint8_t i=0; i<MAX_MODULES; i++) {
        if (mModules[i].IsEmpty()){
            ret = mModules[i].Register(libname, events, numevents);
            if (ret >= 0) {
                if ((VENDOR_PERF_HINT_BEGIN == events[0]) && (VENDOR_POWER_HINT_END == events[1])) {
                    mMpctlMod = &mModules[i];
                }
                ret = i;
            }
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return ret;
}

#define INVALID_VAL (-2)
#define INIT_NOT_COMPLETED (-3)
//layer need to be exposed to HAL
int32_t PerfGlueLayer::PerfGlueLayerSubmitRequest(mpctl_msg_t *msg) {
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    int32_t handle = -1, ret = -1;
    uint8_t cmd = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (tc.getInitCompleted() == false) {
        QLOGE(LOG_TAG, "Request not Submitted as Target Init is not Complete");
        return INIT_NOT_COMPLETED;
    }

    if (NULL == msg) {
        return handle;
    }

    pthread_mutex_lock(&mMutex);

    cmd = msg->req_type;

    if ((cmd >= MPCTL_CMD_PERFLOCKACQ) &&
        (cmd <= MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS)) {
        //direct perflock request
        QLOGL(LOG_TAG, QLOG_L2, "Direct perflock request");
        if (mMpctlMod && !mMpctlMod->IsEmpty()) {
            if (mMpctlMod->GetLib().SubmitRequest)
                handle = mMpctlMod->GetLib().SubmitRequest(msg);
        }
    }
    else if (MPCTL_CMD_PERFLOCKHINTACQ == cmd || MPCTL_CMD_PERFEVENT == cmd) {
        //hint, this can be for any module which were registered earlier
        QLOGL(LOG_TAG, QLOG_L2, "Hint request");
        handle = INVALID_VAL;
        uint32_t hint = msg->hint_id;
        mpctl_msg_t *tmp_msg = NULL;
        tmp_msg =  (mpctl_msg_t*)calloc(1,sizeof(mpctl_msg_t));
        if(tmp_msg != NULL) {
            memcpy(tmp_msg, msg, sizeof(mpctl_msg_t));
            bool resized = false;
            int32_t rc = FAILED;
            do {
                try {
                        rc = ptp.placeTask([=] () mutable {
                                for (uint8_t i = 0; i < MAX_MODULES; i++) {
                                    if (&mModules[i] == mMpctlMod) {
                                        continue;
                                    }
                                    if (!mModules[i].IsEmpty() && mModules[i].IsThisEventRegistered(hint)) {
                                        if (mModules[i].GetLib().SubmitRequest) {
                                            int32_t handle_in = mModules[i].GetLib().SubmitRequest(tmp_msg);
                                            QLOGL(LOG_TAG, QLOG_L2, "Request %x for %s returned Handle: %" PRId32, tmp_msg->hint_id, mModules[i].GetLib().mLibFileName,
                                            handle_in);
                                            (void) handle_in;
                                        }
                                    }
                                }
                                if (tmp_msg != NULL) {
                                    free(tmp_msg);
                                    tmp_msg = NULL;
                                }
                            });
                    } catch (std::exception &e) {
                        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
                    } catch (...) {
                        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
                    }
                if (rc == FAILED && resized == false) {
                    uint32_t new_size = ptp.resize(1);
                    QLOGE(LOG_TAG, "Request %x Failed, Adding thread.\nNew Pool Size: %" PRIu32, msg->hint_id, new_size);
                    resized = true;
                    continue;
                } else {
                    break;
                }
            } while(true);
            if (rc == FAILED && tmp_msg != NULL) {
                free(tmp_msg);
                tmp_msg = NULL;
            }
        }
        if (mMpctlMod && !(mMpctlMod->IsEmpty()) && mMpctlMod->IsThisEventRegistered(hint)){
            if((mMpctlMod->GetLib()).SubmitRequest) {
                handle = ret = mMpctlMod->GetLib().SubmitRequest(msg);
                QLOGL(LOG_TAG, QLOG_L1, "Package: %s, hint:%x, libname: %s, Handle:%" PRId32,msg->usrdata_str,hint,mMpctlMod->GetLib().mLibFileName,handle);
            }
        }
    } else if (MPCTL_CMD_PERFGETFEEDBACK == cmd) {
        //hint, this can be for any module which were registered earlier
        QLOGL(LOG_TAG, QLOG_L2, "FEEDBACK request");
        handle = INVALID_VAL;
        uint32_t hint = msg->hint_id;
        for (uint8_t i = 0; i < MAX_MODULES; i++) {
            if (&mModules[i] == mMpctlMod) {
                continue;
            }
            if (!mModules[i].IsEmpty() &&
                    mModules[i].IsThisEventRegistered(hint)) {
                if(mModules[i].GetLib().SubmitRequest){
                    ret = mModules[i].GetLib().SubmitRequest(msg);
                    if (&mModules[i] == mMpctlMod)
                        handle = ret;
                }
            }
        }
    }

        // make sure mpctl handle return is important and returned, if mpctl
        // handle is not relevant then return whatever other modules returned
        if (handle == INVALID_VAL) {
            handle = ret;
    }
    pthread_mutex_unlock(&mMutex);

    return handle;
}

std::string PerfGlueLayer::PerfGlueLayerSyncRequest(int32_t cmd) {
    pthread_mutex_lock(&mMutex);
    std::string res;
    for (uint8_t i = 0; i < MAX_MODULES; i++) {
        if (!mModules[i].IsEmpty()) {
            if (mModules[i].GetLib().SyncRequest) {
                char *debugInfo = NULL;
                mModules[i].GetLib().SyncRequest(cmd, &debugInfo);
                if (debugInfo) {
                    res.append(debugInfo);
                    res.append("\n");
                    free(debugInfo);
                    debugInfo = NULL;
                }
            }
        }
    }
    pthread_mutex_unlock(&mMutex);
    return res;
}

int32_t PerfGlueLayer::PerfGlueLayerInit() {
    int32_t ret = -1;

    pthread_mutex_lock(&mMutex);
    for (uint8_t i = 0; i < MAX_MODULES; i++) {
        //init all registered modules
        if (!mModules[i].IsEmpty()){
            if (mModules[i].GetLib().Init) {
                ret = mModules[i].GetLib().Init();
            }
            if (ret < 0) {
                //load or init failed, deregister module
                mModules[i].Deregister();
            }
        } /*if (!mModules[i].IsEmpty())*/
    }
    pthread_mutex_unlock(&mMutex);

    return ret;
}

int32_t PerfGlueLayer::PerfGlueLayerExit() {
    int32_t ret = 0;

    QLOGV(LOG_TAG, "PerfGlueLayer:Exit");

    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    pMsg.req_type = MPCTL_CMD_EXIT;
    if (mMpctlMod && !mMpctlMod->IsEmpty()) {
        if (mMpctlMod->GetLib().SubmitRequest) {
            mMpctlMod->GetLib().SubmitRequest(&pMsg);
        }
    }

    pthread_mutex_lock(&mMutex);
    for (uint8_t i = 0; i < MAX_MODULES; i++) {
        //Exit all registered modules
        if (!mModules[i].IsEmpty()){
            if (mModules[i].GetLib().Exit) {
                mModules[i].GetLib().Exit();
            }
            mModules[i].Deregister();
        } /*if (!mModules[i].IsEmpty())*/
    }
    mLoadedModules.clear();
    pthread_mutex_unlock(&mMutex);

    return ret;
}

int32_t PerfGlueLayer::LoadPerfLib(const char *libname) {
    int32_t ret = -1, len = -1, libret = -1;
    uint8_t i = 0;
    PerfModule tmp;
    bool goahead = false;

    if (NULL == libname) {
        return ret;
    }

    pthread_mutex_lock(&mMutex);
    //exceeding num loaded modules
    if (mLoadedModules.size() >= MAX_MODULES) {
        pthread_mutex_unlock(&mMutex);
        QLOGL(LOG_TAG, QLOG_WARNING, "LoadPerfLib(%s): exceeding limit of loaded modules", libname);
        return ret;
    }

    //is this lib loading currently or loaded already
    if (!mLoadedModules.empty() &&
        std::find(mLoadedModules.begin(),
                  mLoadedModules.end(),
                  libname) != mLoadedModules.end()) {
        pthread_mutex_unlock(&mMutex);
        QLOGL(LOG_TAG, QLOG_WARNING, "LoadPerfLib: already loaded or loading:%s", libname);
        return ret;
    }

    //check if we can load
    for (i = 0; i < MAX_MODULES; i++) {
        if (mModules[i].IsEmpty()) {
            goahead = true;
            break;
        }
    }
    if (!goahead) {
        pthread_mutex_unlock(&mMutex);
        QLOGE(LOG_TAG, "LoadPerfLib: no space left for loading: %s", libname);
        return ret;
    }

    mLoadedModules.push_back(libname);
    pthread_mutex_unlock(&mMutex);

    //This will call the register for lib
    libret = tmp.LoadPerfLib(libname);

    pthread_mutex_lock(&mMutex);
    if (libret < 0) {
        mLoadedModules.erase(std::remove(mLoadedModules.begin(),
                                         mLoadedModules.end(),
                                         libname),
                             mLoadedModules.end());
        QLOGE(LOG_TAG, "LoadPerfLib: library didn't load");
        pthread_mutex_unlock(&mMutex);
        return ret;
    }

    for (uint8_t i = 0; i < MAX_MODULES; i++) {
        len = strlen(mModules[i].GetLib().mLibFileName);
        if (!mModules[i].IsEmpty() &&
            !strncmp(mModules[i].GetLib().mLibFileName, libname, len) &&
            !mModules[i].GetLib().is_opened) {
            //load the module
            mModules[i].GetLib().is_opened = tmp.GetLib().is_opened;
            mModules[i].GetLib().dlhandle = tmp.GetLib().dlhandle;
            mModules[i].GetLib().Init = tmp.GetLib().Init;
            mModules[i].GetLib().Exit = tmp.GetLib().Exit;
            mModules[i].GetLib().SubmitRequest = tmp.GetLib().SubmitRequest;
            mModules[i].GetLib().SyncRequest = tmp.GetLib().SyncRequest;
            ret = i;
            QLOGL(LOG_TAG, QLOG_L1, "PerfGlueLayer:LoadPerfLib loading %s at %" PRIu8, libname, (uint8_t)ret);
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);

    if (ret < 0) {
        QLOGL(LOG_TAG, QLOG_L1, "PerfGlueLayer:UnLoadPerfLib %s as \"ret < 0\"\n", libname);
        tmp.UnloadPerfLib();

        pthread_mutex_lock(&mMutex);
        mLoadedModules.erase(std::remove(mLoadedModules.begin(),
                                         mLoadedModules.end(),
                                         libname),
                             mLoadedModules.end());
        pthread_mutex_unlock(&mMutex);
    }

    return ret;
}

PerfModule::PerfModule() {
    mRegistered = false;
    mEventsLowerBound = -1;
    mEventsUpperBound = -1;
    mNumEvents = 0;
    for (uint8_t j = 0; j < MAX_EVENTS; j++) {
        mEvents[j] = -1;
    }
    memset(&mLibHandle,0x00, sizeof(mLibHandle));
}

PerfModule::~PerfModule() {
}

int32_t PerfModule::LoadPerfLib(const char *libname) {
    const char *rc = NULL;
    int32_t ret = -1;

    if (!mLibHandle.is_opened && (NULL != libname)) {
         mLibHandle.dlhandle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
         if (mLibHandle.dlhandle == NULL) {
             QLOGE(LOG_TAG, "%s Failed to (dl)open %s %s\n", __func__, libname, dlerror());
             return ret;
         }

         dlerror();

         *(void **) (&mLibHandle.Init) = dlsym(mLibHandle.dlhandle, "perfmodule_init");
         if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "%s Failed to get perfmodule_init\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         *(void **) (&mLibHandle.Exit) = dlsym(mLibHandle.dlhandle, "perfmodule_exit");
         if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "%s Failed to get perfmodule_exit\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         *(void **) (&mLibHandle.SubmitRequest) = dlsym(mLibHandle.dlhandle, "perfmodule_submit_request");
         if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "%s Failed to get perfmodule_submit_request\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         *(void **) (&mLibHandle.SyncRequest) =
             dlsym(mLibHandle.dlhandle, "perfmodule_sync_request_ext");
         if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "%s Failed to get perfmodule_sync_request_ext\n", __func__);
         }

         mLibHandle.is_opened = true;
         strlcpy(mLibHandle.mLibFileName, libname, MAX_FILE_NAME);
         ret = 0;
    }

    return ret;
}

void PerfModule::UnloadPerfLib() {
    if (mLibHandle.is_opened && mLibHandle.dlhandle) {
        QLOGL(LOG_TAG, QLOG_L1, "PerfModule:UnloadPerfLib %s", mLibHandle.mLibFileName);
            dlclose(mLibHandle.dlhandle);
            mLibHandle.dlhandle = NULL;
        }
        mLibHandle.Init = NULL;
        mLibHandle.Exit = NULL;
        mLibHandle.SubmitRequest = NULL;
        mLibHandle.SyncRequest = NULL;
        mLibHandle.is_opened = false;
        mLibHandle.mLibFileName[0] = '\0';

    return;
}

bool PerfModule::IsThisEventRegistered(int32_t event) {
    bool ret = false;
    ret = (event >= mEventsLowerBound) && (event <= mEventsUpperBound);
    if (!ret) {
        //only search when event not in range
        for (uint8_t i=0; i < mNumEvents; i++) {
            if (event == mEvents[i]) {
                ret = true;
                break;
            }
        } /*for (int i=0; i<mNumEvents; i++)*/
    }
    return ret;
}

int16_t PerfModule::Register(const char *libname, int32_t *events, int32_t numevents) {
    int16_t ret = -1;

    if ((NULL == libname) || (NULL == events) || (numevents < MIN_EVENTS)) {
        QLOGE(LOG_TAG, "no lib or events");
        return ret;
    }

    if (numevents > MAX_EVENTS) {
        numevents = MAX_EVENTS;
    }

    if (!mRegistered) {
        mEventsLowerBound = events[0];
        mEventsUpperBound = events[1];
        strlcpy(mLibHandle.mLibFileName, libname, MAX_FILE_NAME);
        mRegistered = true;
        mNumEvents = numevents-2;
        for (uint8_t j = 0; j < mNumEvents; j++) {
            mEvents[j] = events[j+2];
        }
        ret = 0;
    }
    return ret;
}

bool PerfModule::Deregister(){
    QLOGV(LOG_TAG, "PerfModule:Deregister %s\n", mLibHandle.mLibFileName);
    UnloadPerfLib();
    mEventsLowerBound = -1;
    mEventsUpperBound = -1;
    for (uint8_t j = 0; j < MAX_EVENTS; j++) {
        mEvents[j] = -1;
    }
    mRegistered = false;
    return true;
}

void PerfModule::Dump() {
    QLOGV(LOG_TAG, "mRegistered:%" PRId8 " mEventsLowerBound:0x%x mEventsUpperBound:0x%x mNumEvents:%" PRIu8 " mLibHandle.mLibFileName:%s",
            mRegistered, mEventsLowerBound, mEventsUpperBound, mNumEvents, mLibHandle.mLibFileName);
}
