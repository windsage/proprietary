/********************************************************************
Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef _MINKDESCRIPTOR_H_
#define _MINKDESCRIPTOR_H_

#include "object.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_CBOBJ_THREAD_CNT	4
#define DEFAULT_CBREQ_BUFFER_SIZE       (4 * 1024)

static pthread_mutex_t fmtPrintfMtx __attribute__((unused)) = PTHREAD_MUTEX_INITIALIZER;

#define MAX_COMBINED_LOG_MSG_LEN 3072

static char __combinedLog[MAX_COMBINED_LOG_MSG_LEN];


#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)

static inline void _fmt_printf(const char *fmt, ...)
{
     va_list args;
     va_start(args, fmt);
     vsnprintf(__combinedLog, sizeof(__combinedLog), fmt, args);
     va_end(args);
     printf("%s\n", __combinedLog);
 }


#if defined(OE)
#define PRINTF(...)                                                                              \
     if (!pthread_mutex_lock(&fmtPrintfMtx)) {                                                    \
         _fmt_printf(__VA_ARGS__);                                                                \
         fflush(stdout);                                                                          \
         pthread_mutex_unlock(&fmtPrintfMtx);                                                     \
     } else {                                                                                     \
         printf("(%5u:%-5u) Info  %s:%u Minkdescriptor.h PRINTF Macro skipped due to mutex lock error\n", \
                getpid(), gettid(), __func__, __LINE__);                                          \
     }
#endif


/**@brief get root object
* This is used by client to create a root IClientEnv Obj when it starts up
*
* @ param[in] cbthread_cnt: max callback threads number
* @ param[in] cbbuf_size: max callback req buffer size
* @ param[out] rootobj: root IClientEnv Obj
* return value:  Object_OK - success; Object_ERROR - failure
*/
int MinkDescriptor_getRootEnv (
		size_t cbthread_cnt, size_t cbbuf_size, Object *rootobj);

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
/**@brief shutdown Callback service
* This is used by uint test program to shutdown callback service.
*
* @ param[in] rootobj: root IClientEnv Obj, its context has a pointer to MinkCBContext object.
* return value:  Object_OK - success; Object_ERROR - failure
*/
int MinkDescriptor_shutdownCBService(Object rootobj);
#endif

#ifdef __cplusplus
}
#endif

#endif // _MINKDESCRIPTOR_H_

