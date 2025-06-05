/******************************************************************************
  @file    ActiveRequestList.cpp
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-ACTIVEREQLIST"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <cutils/properties.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utils/Timers.h>

#include "ActiveRequestList.h"
#include "MpctlUtils.h"
#include "ResourceInfo.h"
#include "PerfLog.h"
#include "BoostConfigReader.h"
#include "HintExtHandler.h"

uint8_t ActiveRequestList::ACTIVE_REQS_MAX = property_get_bool("ro.config.low_ram", false)? 15:30;
void ActiveRequestList::Reset() {
    pthread_mutex_lock(&mMutex);
    for (auto iter : mActiveList) {
        if((iter.second) != NULL) {
            delete (iter.second);
            iter.second = NULL;
    }
    }
    mActiveList.clear();
    mActiveReqs = 0;
    pthread_mutex_unlock(&mMutex);
}

/* Always add new request to head of active list */
int8_t ActiveRequestList::Add(Request *req, int32_t req_handle, uint32_t hint_id, int32_t hint_type) {
    int8_t rc = SUCCESS;
    vector<int32_t> hints;
    if (req_handle < 1) {
        return FAILED;
    }
    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    HintExtHandler &hintExt = HintExtHandler::getInstance();

    pthread_mutex_lock(&mMutex);
    if (ACTIVE_REQS_MAX <= mActiveReqs) {
        QLOGE(LOG_TAG, "Active Req Limit Reached: %" PRIu8,ACTIVE_REQS_MAX);
        pthread_mutex_unlock(&mMutex);
        return FAILED;
    }
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret == -1) {
        tv.tv_sec = -1;
    }
    auto tmp = new(std::nothrow) RequestListNode(req_handle,req, hint_id, hint_type, tv);
    if (tmp == NULL) {
        rc = FAILED;
    } else {
        try {
            mActiveList[req_handle] = tmp;
            mActiveReqs++;
            if (store && store->getIgnoreHints(hint_id, hint_type, hints)) {
                for (auto hint: hints) {
                    hintExt.setIgnoreHint(hint, true);
                }
            }
        } catch (std::exception &e) {
            rc = FAILED;
            QLOGE(LOG_TAG, "Caught exception: %s in %s",e.what(), __func__);
        } catch (...) {
            rc = FAILED;
            QLOGE(LOG_TAG, "Error in %s",__func__);
        }
    }
    if (rc == FAILED && tmp != NULL) {
        delete tmp;
    }
    pthread_mutex_unlock(&mMutex);
    return rc;
}

int ActiveRequestList::getTimestamp(struct timeval &tv, std::string &timestamp) {
    struct tm *localTime;
    uint32_t msec;
    char sec[20];
    if (tv.tv_sec == -1) {
        timestamp.append("NA");
        return -1;
    }
    localTime = localtime(&tv.tv_sec);
    if (localTime == NULL) {
        timestamp.append("NA");
        return -1;
    }
    msec = tv.tv_usec / 1000;
    strftime(sec, sizeof(sec), "%m-%d %T", localTime);
    timestamp.append(string(sec));
    timestamp.append(".");
    timestamp.append(to_string(msec));
    return 1;
}

void ActiveRequestList::Remove(int32_t handle) {

    vector<int32_t> hints;
    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    HintExtHandler &hintExt = HintExtHandler::getInstance();

    pthread_mutex_lock(&mMutex);
    if(mActiveReqs < 1|| handle < 1) {
        QLOGL(LOG_TAG, QLOG_L3, "No activity to remove");
        pthread_mutex_unlock(&mMutex);
        return;
    }

    auto iter = mActiveList.find(handle);
    if (iter != mActiveList.end()) {
        if (iter->second) {
            if (store && store->getIgnoreHints(iter->second->mHintId, iter->second->mHintType, hints)) {
                for (auto hint: hints) {
                    hintExt.setIgnoreHint(hint, false);
                }
            }
            delete iter->second;
            iter->second = NULL;
            mActiveList.erase(iter->first);
            mActiveReqs--;
        }
    }
    pthread_mutex_unlock(&mMutex);
}

bool ActiveRequestList::CanSubmit() {
    bool status = false;
    pthread_mutex_lock(&mMutex);
    if (mActiveReqs < ACTIVE_REQS_MAX) {
        status = true;
    }
    pthread_mutex_unlock(&mMutex);
    return status;
}

Request * ActiveRequestList::DoesExists(int32_t handle) {
    Request *tmpreq = NULL;

    pthread_mutex_lock(&mMutex);
    auto iter = mActiveList.find(handle);

    if (iter == mActiveList.end() || handle < 1) {
        pthread_mutex_unlock(&mMutex);
        return NULL;
    }

    if (iter->second != NULL && (iter->second)->mHandle != NULL) {
        tmpreq = (iter->second)->mHandle;
    }
    pthread_mutex_unlock(&mMutex);
    return tmpreq;
}

/* Search for request in active list */
Request * ActiveRequestList::IsActive(int32_t handle, Request *req) {
    Request *tmpreq = NULL;

    pthread_mutex_lock(&mMutex);
    if ((handle < 1) || (NULL == req) || (req->GetNumLocks() < 0)) {
        pthread_mutex_unlock(&mMutex);
        return NULL;
    }
    auto iter = mActiveList.find(handle);

    if (iter != mActiveList.end() && iter->second != NULL && (iter->second)->mHandle != NULL) {
        if (*req != *((iter->second)->mHandle)) {
            tmpreq = NULL;
        } else {
            tmpreq = (iter->second)->mHandle;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return tmpreq;
}

void ActiveRequestList::ResetRequestTimer(int32_t handle, Request *req) {
    pthread_mutex_lock(&mMutex);
    if ((handle < 1) || (NULL == req) || (req->GetNumLocks() < 0)) {
        pthread_mutex_unlock(&mMutex);
        return;
    }

    auto iter = mActiveList.find(handle);
    if (iter != mActiveList.end() && iter->second != NULL && (iter->second)->mHandle != NULL
            && (*req == *((iter->second)->mHandle))) {
        if (req->GetDuration() != INDEFINITE_DURATION) {
            req->SetTimer();
        }
    }
    pthread_mutex_unlock(&mMutex);
}

uint8_t ActiveRequestList::GetNumActiveRequests() {
    return mActiveReqs;
}

uint8_t ActiveRequestList::GetActiveReqsInfo(int32_t *handles, uint32_t *pids) {
    uint8_t i = 0;

    if ((NULL == handles) || (NULL == pids)) {
        QLOGE(LOG_TAG, "Invalid data structure to ActiveRequestList");
        return 0;
    }

    pthread_mutex_lock(&mMutex);

    QLOGL(LOG_TAG, QLOG_L3, "Total no. of active requests: %" PRIu8, GetNumActiveRequests());
    for (auto iter : mActiveList) {
        if (i >= ACTIVE_REQS_MAX) {
            break;
        }
        if (iter.second != NULL && (iter.second)->mHandle != NULL) {
            pids[i] = ((iter.second)->mHandle)->GetPid();
            handles[i] = (iter.second)->mRetHandle;
            i++;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return i;
}

void ActiveRequestList::Dump() {
    pthread_mutex_lock(&mMutex);
    for (auto iter : mActiveList) {
        if (iter.second != NULL) {
            QLOGL(LOG_TAG, QLOG_L3, "request handle:%" PRId32, (iter.second)->mRetHandle);
        }
    }
    pthread_mutex_unlock(&mMutex);
}

std::string ActiveRequestList::DumpActive() {
    using namespace std;
    string result;
    stringstream ss;
    int8_t width = 75;

    ss << setfill('=') << setw(strlen("Active Clients") + width);
    ss << "Active Clients" << setw(width) << "\n";
    ss << setfill(' ') << left << setw(15) << " Pid(Tid)" << left << setw(60) << "| PkgName";
    ss << left << setw(15) << "| Duration" << left << setw(30) << "| Timestamp" << left << setw(15) << "| Handle";
    ss << left << setw(20) << "| HintId : HintType" << left << setw(20) << "| OpCodes" << "\n";
    ss << right << setfill('=') << setw(strlen("Active Clients") + (width * 2)) << "\n";

    pthread_mutex_lock(&mMutex);
    stringstream temp;
    int8_t count = mActiveReqs.load();
    for (auto iter : mActiveList) {
        count--;
        Request* handle = (iter.second)->mHandle;
        if (NULL != handle) {
            temp.clear();
            temp.str("");
            temp << " " << handle->GetPid() << "(" << handle->GetTid() << ")";
            ss << dec;
            ss << left << setfill(' ') << setw(15) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << getPkgNameFromPid(handle->GetPid());
            ss << left << setw(60) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << handle->GetDuration();
            ss << left << setw(15) << temp.str();

            temp.clear();
            temp.str("");
            struct timeval tv= (iter.second)->mtv;
            std::string timestamp;
            getTimestamp(tv, timestamp);
            temp << "| " << timestamp;
            ss << left << setw(30) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << (iter.second)->mRetHandle;
            ss << left << setw(15) << temp.str();

            temp.clear();
            temp.str("");
            if ((iter.second)->mHintId > 0) {
                temp <<"| " << hex <<"0x" << (iter.second)->mHintId << " : "<<dec << (iter.second)->mHintType;
            } else {
                temp <<"| " << "Not Hint";
            }
            ss << left << setw(20) << temp.str();

            ss << "| ";
            for (uint32_t i = 0; i < handle->GetNumLocks(); i++) {
                ResourceInfo* res = handle->GetResource(i);
                if (NULL != res) {
                    ss <<"[";
                    ss << hex;
                    ss << "0x";
                    ss << res->GetOpCode();
                    ss << " : ";
                    ss << dec;
                    ss << res->GetValue();
                    ss << "]";
                    if ((i + 1) < handle->GetNumLocks()) {
                    ss <<",";
                    }
                    ss << " ";
                } else {
                    ss << "No Resources";
                    break;
                }
            }
        }
        ss << '\n';
        if (count > 0) {
            ss << right << setfill('-') << setw(strlen("Active Clients") + (width * 2)) << "\n";
        }
    }
    pthread_mutex_unlock(&mMutex);
    ss << setfill('=') << setw(strlen("Active Clients") + (width * 2));
    ss << '\n';
    result = ss.str();
    return result;
}

std::string ActiveRequestList::getPkgNameFromPid(uint32_t pid) {
    using namespace std;
    string pkg;
    char buf[512];
    snprintf(buf, sizeof(buf), "/proc/%" PRIu32 "/cmdline", pid);
    ifstream in(buf);

    if (!in.is_open()) {
        QLOGE(LOG_TAG, "Cannot open cmdline.");
        return "";
    }

    getline(in, pkg, '\0');
    if (pkg.length() <= 0) {
        QLOGE(LOG_TAG, "Cannot get cmdline.");
        return "";
    }

    return pkg;
}
