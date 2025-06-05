/******************************************************************************
  @file    client.cpp
  @brief   Android performance PerfLock library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014,2017,2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG         "ANDR-PERF-CLIENT"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <shared_mutex>
#include <exception>
#include <cerrno>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cutils/properties.h>
#include "client.h"
#include "PerfController.h"
#include <aidl/vendor/qti/hardware/perf2/BnPerf.h>
#include <aidl/vendor/qti/hardware/perf2/BnPerfCallback.h>
#include "PerfThreadPool.h"
#include "PerfOffloadHelper.h"
#include "PerfLog.h"
#include "Common.h"
#include <pthread.h>
#define FAILED                  -1
#define SUCCESS                 0
#define THREADS_IN_POOL 4
#define DEFAULT_OFFLOAD_FLAG 0
#define SET_OFFLOAD_FLAG 1
#define INIT_NOT_COMPLETED -3

using std::unique_lock;
using std::shared_lock;
using std::shared_timed_mutex;
using std::timed_mutex;
using std::defer_lock;
using CM = std::chrono::milliseconds;

using IPerfAidl = ::aidl::vendor::qti::hardware::perf2::IPerf;
using IPerfCallbackAidl = ::aidl::vendor::qti::hardware::perf2::IPerfCallback;
using ::ndk::SpAIBinder;

#define TIMEOUT 10
static double perf_hal_ver = 0.0;
static bool perf_init_completed_flag = false;

static std::shared_ptr<IPerfAidl> gPerfAidl = nullptr;
static shared_timed_mutex gPerf_lock;
static timed_mutex timeoutMutex;
static timed_mutex perfInitTimeoutMutex;
static ::ndk::ScopedAIBinder_DeathRecipient deathRecipient;

static bool is_perf_hal_alive();
static bool is_perf_init_completed();
static int32_t perf_hint_private(int hint, const char *pkg, int duration, int type, int offloadFlag, int numArgs, int list[]);
static void perf_event_private(int eventId, const char *pkg, int offloadFlag, int numArgs, int list[]);
static int32_t perf_hint_acq_rel_private(int handle, int hint, const char *pkg, int duration, int type, int offloadFlag, int numArgs, int list[]);

void serviceDiedAidl(void* /*cookie*/) {
    try {
        unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
        gPerfAidl = nullptr;
        QLOGE(LOG_TAG, "perf hal just died");
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

static bool getPerfAidlAndLinkToDeath() {
    bool rc = true;

    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl == nullptr) {
            read_lock.unlock();
            {
                unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
                if (gPerfAidl == nullptr) {
                    const std::string instance = std::string() + IPerfAidl::descriptor + "/default";
                    auto perfBinder = ::ndk::SpAIBinder(AServiceManager_checkService(instance.c_str()));

                    if (perfBinder.get() != nullptr) {
                        gPerfAidl = IPerfAidl::fromBinder(perfBinder);
                        if (AIBinder_isRemote(perfBinder.get())) {
                            deathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(AIBinder_DeathRecipient_new(&serviceDiedAidl));
                            auto status = ::ndk::ScopedAStatus::fromStatus(AIBinder_linkToDeath(perfBinder.get(),
                                        deathRecipient.get(), (void*) serviceDiedAidl));
                            if (!status.isOk()) {
                                gPerfAidl = nullptr;
                                rc = false;
                                QLOGE(LOG_TAG, "linking perf aidl service to death failed: %d: %s", status.getStatus(), status.getMessage());
                            }
                        }
                    } else {
                        QLOGE(LOG_TAG, "perf aidl service doesn't exist");
                        rc = false;
                    }
                }
            }
        }
    } catch (std::exception &e) {
        rc = false;
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        rc = false;
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}


int perf_lock_acq(int handle, int duration, int list[], int numArgs)
{
    int rc = -1;
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }
    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST)
        return FAILED;
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> boostsList;
        std::vector<int32_t> paramList;
        int32_t intReturn = 0;

        paramList.push_back(gettid());
        paramList.push_back(getpid());
        boostsList.assign(list, (list + numArgs));
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfLockAcquire(handle, duration, boostsList, paramList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

int perf_lock_rel(int handle)
{
    QLOGV(LOG_TAG, "inside perf_lock_rel of client");
    if (getPerfAidlAndLinkToDeath() == false) {
        return FAILED;
    }
    if (handle == 0)
        return FAILED;

    try {
        std::vector<int32_t> paramList;
        paramList.push_back(gettid());
        paramList.push_back(getpid());

        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfLockRelease(handle, paramList);
            read_lock.unlock();
            return SUCCESS;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return FAILED;
}

int perf_hint(int hint, const char *pkg, int duration, int type)
{
    return perf_hint_private(hint, pkg, duration, type, DEFAULT_OFFLOAD_FLAG, 0, NULL);
}

int perf_hint_private(int hint, const char *pkg, int duration, int type, int offloadFlag, int numArgs, int list[])
{
    int rc = FAILED;
    QLOGV(LOG_TAG, "inside perf_hint of client");
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> paramList;
        int32_t intReturn = 0;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        paramList.assign(list, (list + numArgs));
        paramList.emplace(paramList.begin(),offloadFlag);
        paramList.emplace(paramList.begin(),getpid());
        paramList.emplace(paramList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfHint(hint, pkg_s, duration, type, paramList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

int perf_callback_register(const std::shared_ptr<IPerfCallbackAidl>& callback, int32_t clientId) {
    int rc = FAILED;
    QLOGV(LOG_TAG, "inside perf_callback_register of client");
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (callback != nullptr) {
        try {
            int32_t intReturn = 0;
            shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
            if (gPerfAidl != NULL) {
                gPerfAidl->perfCallbackRegister(callback, clientId, &intReturn);
                read_lock.unlock();
                rc = intReturn;
            } else {
                QLOGE(LOG_TAG, "NULL gPerfAidl when perf_callback_register");
            }
        } catch (std::exception &e) {
            QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        }
    } else {
        QLOGE(LOG_TAG, "Invalid empty registercallback.");
    }
    return rc;
}

int perf_callback_deregister(const std::shared_ptr<IPerfCallbackAidl>& callback, int32_t clientId) {
    int rc = FAILED;
    QLOGV(LOG_TAG, "inside perf_callback_deregister of client");
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (callback != nullptr) {
        try {
            int32_t intReturn = 0;
            shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
            if (gPerfAidl != NULL) {
                gPerfAidl->perfCallbackDeregister(callback, clientId, &intReturn);
                read_lock.unlock();
                rc = intReturn;
            }  else {
                QLOGE(LOG_TAG, "NULL gPerfAidl when perf_callback_deregister");
            }
        } catch (std::exception &e) {
            QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        }
    } else {
        QLOGE(LOG_TAG, "Invalid empty deregister callback.");
    }
    return rc;
}

int perf_get_feedback(int req, const char *pkg) {

    QLOGV(LOG_TAG, "inside %s of client", __func__);
    return perf_get_feedback_extn(req, pkg, 0, NULL);
}

//This is the new perf get prop extn API which takes a buffer and its size for writing result values.
int perf_get_prop_extn(const char *prop , char *buffer, size_t buffer_size, const char *def_val) {

    int rc = FAILED;
    char *out = buffer;
    if (buffer_size < PROP_VAL_LENGTH || out == NULL) {
        if (out != NULL) {
            QLOGE(LOG_TAG, "Invalid Buffer passed of size %zu Expected: %d", buffer_size, PROP_VAL_LENGTH);
        } else {
            QLOGE(LOG_TAG, "Invalid empty buffer passed");
        }
        return rc;
    }
    if (getPerfAidlAndLinkToDeath() == false) {
        out = NULL;
    }
    if(out != NULL) {
        out[0] = '\0';
        string aidlReturn = "";

        try {
            shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
            if (gPerfAidl != NULL) {
                gPerfAidl->perfGetProp(prop, def_val, &aidlReturn);
                read_lock.unlock();
                strlcpy(out, aidlReturn.c_str(), PROP_VAL_LENGTH);
            } else if (def_val != NULL) {
                strlcpy(out, def_val, PROP_VAL_LENGTH);
            }
            QLOGI(LOG_TAG, "VALUE RETURNED FOR PROPERTY %s :: %s", prop, out);
        } catch (std::exception &e) {
            QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        }
    } else if (def_val != NULL) {
       rc = strlcpy(buffer, def_val, PROP_VAL_LENGTH);
    }
    return rc;
}

//This  API is getting deprecated...
PropVal perf_get_prop(const char *prop , const char *def_val) {

    pid_t client_tid = 0;
    PropVal return_value = {""};
    return_value.value[0] = '\0';
    char *out = NULL;
    if (getPerfAidlAndLinkToDeath() == true) {
        out = (char *)malloc(PROP_VAL_LENGTH * sizeof(char));
    }
    if(out != NULL) {
        out[0] = '\0';
        client_tid = gettid();
        string aidlReturn = "";

        try {
            shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
            if (gPerfAidl != NULL) {
                gPerfAidl->perfGetProp(prop, def_val, &aidlReturn);
                read_lock.unlock();
                strlcpy(out, aidlReturn.c_str(), PROP_VAL_LENGTH);
            } else if (def_val != NULL) {
                strlcpy(out, def_val, PROP_VAL_LENGTH);
            }
            strlcpy(return_value.value, out, PROP_VAL_LENGTH);
            QLOGI(LOG_TAG, "VALUE RETURNED FOR PROPERTY %s :: %s",prop,return_value.value);
        } catch (std::exception &e) {
            QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        }
        free(out);
    } else if (def_val != NULL) {
        strlcpy(return_value.value, def_val, PROP_VAL_LENGTH);
    }
    return return_value;
}

/* this function returns a malloced string which must be freed by the caller */
const char* perf_sync_request(int cmd) {
    QLOGV(LOG_TAG, "perf_sync_request cmd:%d", cmd);
    char *out = NULL;
    if (getPerfAidlAndLinkToDeath() ==false) {
        return out;
    }
    try {
        std::vector<int32_t> paramList;
        string aidlReturn = "";
        paramList.push_back(gettid());
        paramList.push_back(getpid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfSyncRequest(cmd, "", paramList, &aidlReturn);
            read_lock.unlock();
            out = strdup(aidlReturn.c_str());
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return out;
}

int perf_lock_acq_rel(int handle, int duration, int list[], int numArgs, int reserveNumArgs) {
    int rc = -1;
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST ||
            reserveNumArgs < 0 || reserveNumArgs > MAX_RESERVE_ARGS_PER_REQUEST ||
            (reserveNumArgs + numArgs) > (MAX_ARGS_PER_REQUEST + MAX_RESERVE_ARGS_PER_REQUEST) ||
            (reserveNumArgs + numArgs) <= 0) {
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> boostsList;
        std::vector<int32_t> reservedList;
        int32_t intReturn = 0;

        boostsList.assign(list, (list + numArgs));
        reservedList.assign(list + numArgs, (list + numArgs+ reserveNumArgs));
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfLockAcqAndRelease(handle, duration, boostsList, reservedList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

int perf_get_feedback_extn(int req, const char *pkg, int numArgs, int list[]) {
    int rc = -1;
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    try {
        std::vector<int32_t> reservedList;
        int32_t intReturn = 0;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        reservedList.assign(list, (list + numArgs));
        reservedList.emplace(reservedList.begin(),DEFAULT_OFFLOAD_FLAG);
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfGetFeedback(req, pkg_s, reservedList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

void perf_event(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    perf_event_private(eventId, pkg, DEFAULT_OFFLOAD_FLAG, numArgs, list);
}

void perf_event_private(int eventId, const char *pkg, int offloadFlag, int numArgs, int list[]) {

    if (getPerfAidlAndLinkToDeath() == false) {
        return;
    }


    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return;
        }
        std::vector<int32_t> reservedList;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        reservedList.assign(list, (list + numArgs));
        reservedList.emplace(reservedList.begin(),offloadFlag);
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());

        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfEvent(eventId, pkg_s, reservedList);
            read_lock.unlock();
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

int perf_hint_acq_rel(int handle, int hint, const char *pkg, int duration, int type,
        int numArgs, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    return perf_hint_acq_rel_private(handle, hint, pkg, duration, type, DEFAULT_OFFLOAD_FLAG, numArgs, list);
}

int32_t perf_hint_acq_rel_private(int handle, int hint, const char *pkg, int duration, int type,
        int offloadFlag, int numArgs, int list[]) {
    int32_t rc = FAILED;
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }


    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> reservedList;
        int32_t intReturn = 0;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        reservedList.assign(list, (list + numArgs));
        reservedList.emplace(reservedList.begin(),offloadFlag);
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfHintAcqRel(handle, hint,
                    pkg_s, duration, type, reservedList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

int perf_hint_renew(int handle, int hint, const char *pkg, int duration, int type,
        int numArgs, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);

    int rc = -1;
    if (getPerfAidlAndLinkToDeath() == false) {
        return rc;
    }

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> reservedList;
        int32_t intReturn = 0;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        reservedList.assign(list, (list + numArgs));
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfHintRenew(handle, hint,
                    pkg_s, duration, type, reservedList, &intReturn);
            read_lock.unlock();
            rc = intReturn;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return rc;
}

/*
below are thread based offload API's
*/
int perf_hint_offload(int hint, const char *pkg, int duration, int type, int listlen, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (listlen > MAX_RESERVE_ARGS_PER_REQUEST || listlen < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d Max Accepted %d", listlen, MAX_RESERVE_ARGS_PER_REQUEST);
        return FAILED;
    }

    if (!is_perf_hal_alive() || !is_perf_init_completed()) {
        return rc;
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
    int32_t offload_handle = poh.getNewOffloadHandle();
    if (offload_handle == FAILED) {
        QLOGE(LOG_TAG, "Failed to create Offload Handle in %s", __func__);
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len);
        pkg_t[len] = '\0';
    }

    if (listlen > 0) {
        list_t = (int*)calloc(listlen, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * listlen);
    }

    try {
        rc = ptp.placeTask([=]() mutable {
                    int32_t handle = perf_hint_private(hint, pkg_t, duration, type, SET_OFFLOAD_FLAG, listlen, list_t);
                    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                    int32_t res = poh.mapPerfHandle(offload_handle, handle);
                    if (res == RELEASE_REQ_STATE) {
                        perf_lock_rel(handle);
                        poh.releaseHandle(offload_handle);
                    }
                    if (pkg_t != NULL) {
                       free(pkg_t);
                       pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to Offload hint: 0x%X", hint);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    } else {
        rc = offload_handle;
    }
    return rc;
}

int perf_lock_rel_offload(int handle) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    int rc = FAILED;
    if (!is_perf_hal_alive()) {
        return rc;
    }
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);

    try {
        rc = ptp.placeTask([=]() {
                PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                int32_t releaseHandle = poh.getPerfHandle(handle);
                    if (releaseHandle > 0) {
                        int p_handle = perf_lock_rel(releaseHandle);
                        poh.releaseHandle(handle);
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to Offload release for %d", handle);
    }
    return rc;
}

int perf_hint_acq_rel_offload(int handle, int hint, const char *pkg, int duration, int type, int numArgs, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d Max Accepted %d", numArgs, MAX_RESERVE_ARGS_PER_REQUEST);
        return FAILED;
    }

    if (!is_perf_hal_alive() || !is_perf_init_completed()) {
        return rc;
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
    int32_t offload_handle = poh.getNewOffloadHandle();
    if (offload_handle == FAILED) {
        QLOGE(LOG_TAG, "Failed to create Offload Handle in %s", __func__);
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len);
        pkg_t[len] = '\0';
    }
    if (numArgs > 0) {
        list_t = (int*)calloc(numArgs, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * numArgs);
    }

    try {
        rc = ptp.placeTask([=]() mutable {
                PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                int32_t releaseHandle = poh.getPerfHandle(handle);
                QLOGV(LOG_TAG, "%s releasing %d mapped to %d",__func__,releaseHandle, handle);
                int32_t p_handle = perf_hint_acq_rel_private(releaseHandle, hint, pkg_t, duration, type, SET_OFFLOAD_FLAG, numArgs, list_t);
                int32_t res = poh.mapPerfHandle(offload_handle, p_handle);
                if (res == RELEASE_REQ_STATE) {
                    perf_lock_rel(p_handle);
                    poh.releaseHandle(offload_handle);
                }
                if (releaseHandle > 0) {
                    poh.releaseHandle(handle);
                }

                if (pkg_t != NULL) {
                    free(pkg_t);
                    pkg_t = NULL;
                }
                if (list_t != NULL) {
                    free(list_t);
                    list_t = NULL;
                }
        });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to Offload hint: 0x%X", hint);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    } else {
        rc = offload_handle;
    }
    return rc;
}

int perf_event_offload(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    if (!is_perf_hal_alive() || !is_perf_init_completed()) {
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len);
        pkg_t[len] = '\0';
    }
    if (numArgs > 0) {
        list_t = (int*)calloc(numArgs, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * numArgs);
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    try {
        rc = ptp.placeTask([=]() mutable {
                    perf_event_private(eventId, pkg_t, SET_OFFLOAD_FLAG, numArgs, list_t);
                    if (pkg_t != NULL) {
                        free(pkg_t);
                        pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to Offload event: 0x%X", eventId);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    }
    return rc;
}

static bool is_perf_hal_alive() {
    QLOGV(LOG_TAG, "inside %s of client", __func__);
    return getPerfAidlAndLinkToDeath();
}

static bool is_perf_init_completed() {
    int32_t handle = FAILED;
    if (perf_init_completed_flag == true) {
        return perf_init_completed_flag;
    }
    try {
        unique_lock<timed_mutex> lck(perfInitTimeoutMutex, defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "Process is Flooding request to PerfService");
        } else {
            if (perf_init_completed_flag != true)  {
                handle = perf_hint(VENDOR_HINT_PERF_INIT_CHECK, NULL, 0, -1);
                if (handle != INIT_NOT_COMPLETED) {
                    perf_init_completed_flag = true;
                }
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return perf_init_completed_flag;
}

void perf_lock_cmd(int cmd)
{
    if (getPerfAidlAndLinkToDeath() == false) {
        return;
    }
    pid_t client_tid = 0;

    QLOGV(LOG_TAG, "inside perf_lock_cmd of client");
    client_tid = gettid();
    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfAidl != NULL) {
            gPerfAidl->perfLockCmd(cmd, client_tid);
            read_lock.unlock();
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

//need to deprecate
PropVal perf_wait_get_prop(const char * /*prop*/, const char * /*def_val*/) {
    PropVal return_value = {""};
    return_value.value[0] = '\0';
    return return_value;
}
//end of deprecated API's

void getthread_name(char *buf, int buf_size){
    int rc=-1;
    rc=pthread_getname_np(pthread_self(), buf, buf_size);
    if(rc!=0){
       QLOGE(LOG_TAG,"Failed in getting thread name");
    }
}

