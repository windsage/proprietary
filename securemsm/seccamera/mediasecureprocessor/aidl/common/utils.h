/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <QtiGralloc.h>
#include <hardware/gralloc1.h>
#include <string.h>
#include <time.h>
#include <utils/Log.h>
#include <stdio.h>
#include "object.h"
#include <unistd.h>

#if defined(OFFTARGET)
#include <sys/syscall.h>
/* Define method to print thread ID */
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))
#else
#define gettid() (0)
#endif

#if defined(__ANDROID__)
#include <utils/Log.h>
#define PRINTF(...) ALOGI(__VA_ARGS__);
#else
#define PRINTF(...) \
    do {            \
    } while (0);
#endif

/******************************************************************************/
/************************  Helper Functions/Macros  ***************************/
/******************************************************************************/

#define MAX_LOG_MSG_LEN 1024

/* These are global to avoid increasing the stack size since they appear inside
 * each log. */
static char __msg[MAX_LOG_MSG_LEN] __attribute__((unused));
static char __funcName[MAX_LOG_MSG_LEN];

/* Keep everything up to '(' character of string and copy into __funcName */
static inline void _getFuncName(char *fullFuncDef, uint32_t len)
{
    char *p;
    strlcpy(__funcName, fullFuncDef, len);
    if ((p = strchr(__funcName, '('))) {
        *p = 0;
    }
}

/* Format all Trace messages with an optional fail message. The function name is
 * always included. */
static inline void _makeLogMsg(char *__msg, char *tailFmt, char *failFmt,
                               uint32_t len)
{
    /* (proc_id:thread_id) func_name "custom format" [---> "failed: rv"] */
    char *print_fmt = "(%5u:%-5u) Trace %s %s ---> %s\n";

    if (strlen(failFmt) == 0) {
        print_fmt = "(%5u:%-5u) Trace %s %s\n";
    }

    snprintf(__msg, len, print_fmt, getpid(), gettid(), "%s", tailFmt, failFmt);
}

/* More complicated stuff */
#define _NUM_ARGS_EVAL_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
                        _13, _14, _15, N, ...)                             \
    N

#define _NUM_ARGS_(...)                                                      \
    _NUM_ARGS_EVAL_(_0, ##__VA_ARGS__, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, \
                    3, 2, 1, 0)

#define _NUM_ARGS_MIN_2_(...)                                                 \
    _NUM_ARGS_EVAL_(_0, ##__VA_ARGS__, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, \
                    1, 0)

#define _NUM_ARGS_MIN_4_(...)                                                 \
    _NUM_ARGS_EVAL_(_0, ##__VA_ARGS__, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, \
                    1, 0)

#define _GET_MACRO_(name, n) name##n

#define _GET_MACRO(name, n) _GET_MACRO_(name, n)

#define GET_MACRO(macro, ...) \
    _GET_MACRO(macro, _NUM_ARGS_(__VA_ARGS__))(__VA_ARGS__)

/* optional arguments */
#define GET_MACRO_MIN_2(macro, ...) \
    _GET_MACRO(macro, _NUM_ARGS_MIN_2_(__VA_ARGS__))(__VA_ARGS__)

#define GET_MACRO_MIN_4(macro, ...) \
    _GET_MACRO(macro, _NUM_ARGS_MIN_4_(__VA_ARGS__))(__VA_ARGS__)

/* Save an errorCode expression to the provided variable, printf a message, and
 * goto exit */
#define ERROR(rv, errorCode, ...) \
    rv = (errorCode);             \
    PRINTF(__VA_ARGS__)           \
    goto exit;

#define T_LOG(...) GET_MACRO_MIN_2(T_LOG, __VA_ARGS__)
#define T_LOG2(a, b) T_LOG3(a, b, "")

#define T_LOG3(fullFuncDef, failFmt, tailFmt, ...)             \
    {                                                          \
        _getFuncName(fullFuncDef, MAX_LOG_MSG_LEN);            \
        _makeLogMsg(__msg, tailFmt, failFmt, MAX_LOG_MSG_LEN); \
        PRINTF(__msg, __funcName, ##__VA_ARGS__);              \
    }

#define T_CHECK_FUNC(...) GET_MACRO_MIN_4(T_CHECK_FUNC, __VA_ARGS__)
#define T_CHECK_FUNC4(a, b, c, d) T_CHECK_FUNC5(a, b, c, d, "")

/* Call local functions to format a failure message, check if the passing
 * criteria is met and return an error if it is not. */
#define T_CHECK_FUNC5(rv, errorCode, fullFuncDef, expectTrue, tailFmt, ...) \
    {                                                                       \
        strlcpy(__funcName, fullFuncDef, MAX_LOG_MSG_LEN);                  \
        _makeLogMsg(__msg, tailFmt, "failed : %d", MAX_LOG_MSG_LEN);        \
        if (!(expectTrue)) {                                                \
            ERROR(rv, errorCode, __msg, __funcName, ##__VA_ARGS__, rv)      \
        }                                                                   \
    }

/******************************************************************************/
/************************  Public Macro Definitions  **************************/
/******************************************************************************/

/* Print a message to the terminal */
#define LOG_MSG(xx_fmt, ...)                                              \
    {                                                                     \
        PRINTF("(%5u:%-5u) Info  %s:%u " xx_fmt "\n", getpid(), gettid(), \
               __func__, __LINE__, ##__VA_ARGS__);                        \
    }

/* T_CHECK* and T_GUARD* check that a condition is TRUE or that a call returns
 * Object_OK, respectively. With passing criteria, nothing is printed to the
 * terminal and execution continues. For failing criteria, an error is printed
 * and execution will 'goto exit'. */
#define T_CHECK_ERR(cond, errorCode)                                      \
    if (!(cond)) {                                                        \
        ERROR(ret, errorCode,                                             \
              "(%5u:%-5u) Error %s:%u (%s) ---> failed : %d\n", getpid(), \
              gettid(), __func__, __LINE__, #cond, errorCode);            \
    }

/* Log function name and THEN call function. There is never a fail message, but
 * the user may define a format to print on the same line as the Trace. */
#define T_CALL_NO_CHECK(rv, func, ...) \
    T_LOG(#func, "", ##__VA_ARGS__);   \
    rv = func;

/* Same at T_CALL, but user specifies the lhs, pass criteria, and error value */
#define T_CALL_CHECK_ERR(lhs, func, expectTrue, errorCode, ...)         \
    {                                                                   \
        T_CALL_NO_CHECK(lhs, func);                                     \
        T_CHECK_FUNC(ret, errorCode, #func, expectTrue, ##__VA_ARGS__); \
    }

/* Same at T_CALL, but user can over-ride the error value */
#define T_CALL_ERR(func, errorCode, ...)                                              \
    {                                                                                 \
        T_CALL_CHECK_ERR(ret, func, ret == ErrorCode::SECURE_PROCESSOR_OK, errorCode, \
                         ##__VA_ARGS__);                                              \
    }

/* Print expression to terminal, call expression and then check that error value
 * is OK, else print an error and go to exit */
#define T_CALL(func, ...)                     \
    {                                         \
        T_CALL_ERR(func, ret, ##__VA_ARGS__); \
    }

uint32_t calcBufferSize(uint32_t format, uint32_t width, uint32_t stride,
                        uint32_t height);

bool isYUVSPFormat(uint32_t format);

uint32_t calcYStride(uint32_t format, uint32_t stride);

uint32_t calcUVStride(uint32_t format, uint32_t stride);

uint32_t calcUVOffset(uint32_t format, uint32_t stride, uint32_t height);

int64_t getTimeUsec();

int32_t getSecCamAppPath(std::string &fileName, std::string &AppPath);

int32_t getFileSize(std::string const &filename);

int32_t readFile(std::string const &filename, size_t &size, uint8_t *buffer);

#endif  // UTILS_H
