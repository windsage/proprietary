/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <QtiMutex.h>

#include "framework/Log.h"
#ifdef __ANDROID__
#define LOG_TAG "RILQ"
#define TAG LOG_TAG
#include <utils/Log.h>
#endif

#ifdef RIL_FOR_MDM_LE
#define LOG_TAG "RILQ"
#define TAG LOG_TAG
#endif

using std::string;
using std::lock_guard;
using std::endl;
using std::cout;
using std::chrono::system_clock;

const std::string PIIDefault<std::string>::value = "<hidden>";

template <>
const char* PII<const char *, std::string>(const char * arg, const std::string &def) {
    if (enablePIILogging) {
        return arg ? arg : "<null>";
    }
    return def.c_str();
}

template <>
const char* PII<char *, std::string>(char * arg, const std::string &def) {
    return PII<const char *, std::string>(arg, def);
}

void Log::setEnabled(bool enabled) {
  lock_guard<qtimutex::QtiSharedMutex> lock(mMutex);
  mEnabled = enabled;
}

void Log::d(string str) {
#if defined (__ANDROID__) || defined (QMI_RIL_UTF) || defined (RIL_FOR_MDM_LE)
  if (mEnabled) {
    QCRIL_LOG_DEBUG("%s", str.c_str());
  }
#else
  //lock_guard<mutex> lock(mMutex);
  auto myid = std::this_thread::get_id();
  std::stringstream ss;
  ss << myid;
  string threadId = ss.str();
  system_clock::time_point p = system_clock::now();
  std::time_t t = system_clock::to_time_t(p);
  string time{std::ctime(&t)};
  // cout << time;
  if (mEnabled) {
    cout << "[" + threadId + "]:" + str << '\n';
  }
#endif
}

void Log::logTime(string str) {
  //lock_guard<mutex> lock(mMutex);
  system_clock::time_point p = system_clock::now();
  std::time_t t = system_clock::to_time_t(p);
  string time{std::ctime(&t)};
  if (mEnabled) {
#if defined (__ANDROID__) || defined (QMI_RIL_UTF)
    QCRIL_LOG_DEBUG("%s[%s", str.c_str(), time.c_str());
#else
    cout << str << " [" << time;
#endif
  }
}

Log &Log::getInstance() {
  static Log sInstance;
  return sInstance;
}
