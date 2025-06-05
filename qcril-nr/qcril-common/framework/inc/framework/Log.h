/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#ifdef __cplusplus
#include <string>
#include <vector>
#include <assert.h>
#include <QtiMutex.h>

#ifdef QMI_RIL_UTF
#include "ril_utf_log.h"
#else

#include <Logger.h>

#ifndef QCRIL_LOG_ADDITIONAL
#define QCRIL_LOG_ADDITIONAL    QCRIL_HAL_LOG_ADDITIONAL
#endif

#ifndef QCRIL_LOG_INFO
#define QCRIL_LOG_INFO          QCRIL_HAL_LOG_INFO
#endif

#ifndef QCRIL_LOG_WARN
#define QCRIL_LOG_WARN          QCRIL_HAL_LOG_WARN
#endif

#ifndef QCRIL_LOG_ERROR
#define QCRIL_LOG_ERROR         QCRIL_HAL_LOG_HIGH
#endif

#ifndef QCRIL_LOG_DEBUG
#define QCRIL_LOG_DEBUG         QCRIL_HAL_LOG_DEBUG
#endif

#ifndef QCRIL_LOG_FUNC_ENTRY
#define QCRIL_LOG_FUNC_ENTRY    QCRIL_HAL_LOG_ENTER
#endif

#ifndef QCRIL_LOG_FUNC_RETURN
#define QCRIL_LOG_FUNC_RETURN   QCRIL_HAL_LOG_LEAVE
#endif

#ifndef QCRIL_LOG_ESSENTIAL
#define QCRIL_LOG_ESSENTIAL     QCRIL_HAL_LOG_HIGH  // LPA REARCH - add a new macro for QXDM logging.
#endif

#ifndef QCRIL_LOG_FATAL
#define QCRIL_LOG_FATAL         QCRIL_HAL_LOG_FATAL
#endif

#ifndef QCRIL_LOG_VERBOSE
#define QCRIL_LOG_VERBOSE       QCRIL_HAL_LOG_DEBUG
#endif

#ifndef QCRIL_LOG_CF_PKT_RIL_FN
#define QCRIL_LOG_CF_PKT_RIL_FN(instance, label)
#endif

#ifndef QCRIL_LOG_FUNC_RETURN_WITH_RET
#define QCRIL_LOG_FUNC_RETURN_WITH_RET( ... ) \
{ QCRIL_HAL_LOG_LEAVE("function exit with ret %d", ##__VA_ARGS__ ); }
#endif

#ifndef QCRIL_LOG_ASSERT
#define QCRIL_LOG_ASSERT( cond )    ({ if(!(cond)) {QCRIL_HAL_LOG_FATAL("assert" #cond " failed"); assert(cond);} })
#endif

#endif

using std::vector;
using std::string;

#ifdef ENABLE_PII_LOGGING
constexpr bool enablePIILogging = true;
#else
constexpr bool enablePIILogging = false;
#endif

template <typename T, typename O=void>
struct asConst {
    using type = const T;
};

template <typename T>
struct asConst<T,typename std::enable_if_t<std::is_pointer<T>::value>> {
    using type = typename std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>;
};

template <typename T>
struct PIIDefault {
    static constexpr typename asConst<T>::type value = T();
};

template <>
struct PIIDefault<char *> {
    static constexpr const char *value = "<hidden>";
};

template <>
struct PIIDefault<std::string> {
    static const std::string value;
};

template <typename T, typename D = typename asConst<T>::type>
constexpr typename asConst<T>::type PII(T arg, const D &def = PIIDefault<T>::value) {
    if (enablePIILogging) {
        return arg;
    }
    return def;
}

template <>
const char* PII<char *, std::string>(char * arg, const std::string &def);

template <>
const char* PII<const char *, std::string>(const char * arg, const std::string &def);

class Log {
 private:
  bool mEnabled = true;
  qtimutex::QtiSharedMutex mMutex;
  Log() {}

 public:
  Log(Log const &) = delete;             // Copy construct
  Log(Log &&) = delete;                  // Move construct
  Log &operator=(Log const &) = delete;  // Copy assign
  Log &operator=(Log &&) = delete;       // Move assign

  void d(string str);
  void logTime(string str);
  void setEnabled(bool enabled);
  static Log &getInstance();
};

extern "C" size_t strlcat(char *dst, const char *src, size_t siz);
extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);

#endif  /* __cplusplus */

