/******************************************************************************
#  Copyright (c) 2013 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

  @file    ril_utf_log.h
  @brief   RIL UTF logging declarations

  DESCRIPTION
  ---------------------------------------------------------------------------
******************************************************************************/

/* ------------------------------
   UTF Logging Implementations..
   ------------------------------
*/
#ifndef _RIL_UTF_LOG_H
#define _RIL_UTF_LOG_H

#include <pthread.h>
#include <inttypes.h>

#define RIL_UTF_MAX_LOG_MSG_SIZE                512
#define RIL_UTF_THREAD_INFO_MAX_SIZE          64
#define RIL_UTF_THREAD_NAME_MAX_SIZE          64


typedef struct
{
    int is_valid;
    int thread_id;
    char ril_utf_thread_name[RIL_UTF_THREAD_NAME_MAX_SIZE];
} ril_utf_thread_name_info_type;


extern ril_utf_thread_name_info_type ril_utf_thread_name_info[RIL_UTF_THREAD_INFO_MAX_SIZE];
extern FILE *utf_log_file;
extern FILE *utf_results_log_file;

#ifdef __cplusplus
extern "C" {
#endif

void ril_utf_format_log_msg( char *buf_ptr, int buf_size, char *fmt, ... ) ;
int ril_utf_get_thread_name(pthread_t thread_id, char *thread_name);
int ril_utf_check_reboot_state();

#ifdef __cplusplus
}
#endif

int qmi_ril_utf_shutdown();

extern pthread_mutex_t ril_utf_log_lock_mutex;
extern char ril_utf_thread_name[ RIL_UTF_THREAD_NAME_MAX_SIZE ];

#ifndef LEVEL_ESSENTIAL
#define LEVEL_ESSENTIAL 47
#endif

#ifndef LEVEL_DEBUG
#define LEVEL_DEBUG 48
#endif

#define RIL_UTF_MSG(lvl, fmt, ... )    \
{ \
  pthread_mutex_lock(&ril_utf_log_lock_mutex);                                         \
  char log_buf[ RIL_UTF_MAX_LOG_MSG_SIZE ]; \
  char log_fmt[ RIL_UTF_MAX_LOG_MSG_SIZE ]; \
  char log_buf_raw[ RIL_UTF_MAX_LOG_MSG_SIZE ];   \
  snprintf(log_buf_raw, RIL_UTF_MAX_LOG_MSG_SIZE, fmt, ##__VA_ARGS__ );\
  if(ril_utf_get_thread_name(pthread_self(), ril_utf_thread_name))\
  { \
          strlcpy(log_fmt, "\n %s %d RIL[<thread_name: %s>] [%ld] %s: %s ", sizeof(log_fmt));\
          ril_utf_format_log_msg( log_buf, RIL_UTF_MAX_LOG_MSG_SIZE, log_fmt, __FILE__, __LINE__,ril_utf_thread_name, pthread_self(),  __func__ , log_buf_raw);\
  } \
  else  \
  { \
          strlcpy(log_fmt, "\n %s %d RIL [%ld] %s: %s ", sizeof(log_fmt));\
          ril_utf_format_log_msg( log_buf, RIL_UTF_MAX_LOG_MSG_SIZE, log_fmt, __FILE__, __LINE__, pthread_self(), __func__ , log_buf_raw);\
  } \
  if(utf_log_file) {\
    if(lvl == LEVEL_DEBUG)  \
      fprintf(utf_log_file, "%s", log_buf ); \
    else if(lvl == LEVEL_ESSENTIAL) \
      fprintf(utf_log_file, "%s", log_buf_raw); \
  }\
  (void) pthread_mutex_unlock(&ril_utf_log_lock_mutex); \
}

#define RIL_UTF_SIMPLE_LOG(fmt, ... ) \
{ \
  pthread_mutex_lock(&ril_utf_log_lock_mutex); \
  if(utf_log_file) {\
    fprintf(utf_log_file, fmt, ##__VA_ARGS__ ); \
  }\
  pthread_mutex_unlock(&ril_utf_log_lock_mutex); \
}

#define RIL_UTF_ESSENTIAL( ... ) RIL_UTF_MSG(LEVEL_ESSENTIAL,__VA_ARGS__)

// Print log for html generator with "\n@@@@@@" + message
#define RIL_UTF_HTML_LOG(format, ...) \
{ \
  RIL_UTF_ESSENTIAL("\n@@@@@@"); \
  RIL_UTF_ESSENTIAL((const char*)format, ## __VA_ARGS__);\
}

// Print log for html generator with the original message
#define RIL_UTF_HTML_LOG_ORIG(...) RIL_UTF_ESSENTIAL(__VA_ARGS__)

#define RIL_UTF_LOG_RESULT(fmt, ... ) \
{ \
  fprintf(stdout, fmt, ##__VA_ARGS__ ); \
  fflush(stdout); \
  if(utf_results_log_file) {\
    fprintf(utf_results_log_file, fmt, ##__VA_ARGS__ ); \
  }\
}

#ifdef UTF_DEBUG_MODE
#define RIL_UTF_DEBUG( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#else
#define RIL_UTF_DEBUG(...) do{}while(0)
#endif


#define   RIL_UTF_LOG_ENTER(fmt, ...) RIL_UTF_MSG(LEVEL_DEBUG, "> %s: " fmt, __func__, ##__VA_ARGS__)
#define   RIL_UTF_LOG_LEAVE(fmt, ...) RIL_UTF_MSG(LEVEL_DEBUG, "< %s: " fmt, __func__, ##__VA_ARGS__)

#ifndef QCRIL_LOG_ADDITIONAL
#define QCRIL_LOG_ADDITIONAL( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_INFO
#define QCRIL_LOG_INFO( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_WARN
#define QCRIL_LOG_WARN( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_ERROR
#define QCRIL_LOG_ERROR( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_DEBUG
#define QCRIL_LOG_DEBUG( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_FUNC_ENTRY
#define QCRIL_LOG_FUNC_ENTRY    RIL_UTF_LOG_ENTER
#endif

#ifndef QCRIL_LOG_FUNC_RETURN
#define QCRIL_LOG_FUNC_RETURN   RIL_UTF_LOG_LEAVE
#endif

#ifndef QCRIL_LOG_ESSENTIAL
#define QCRIL_LOG_ESSENTIAL( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_FATAL
#define QCRIL_LOG_FATAL( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_VERBOSE
#define QCRIL_LOG_VERBOSE( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_ADDITIONAL
#define QCRIL_LOG_ADDITIONAL( ... )   RIL_UTF_MSG(LEVEL_DEBUG,__VA_ARGS__ )
#endif

#ifndef QCRIL_LOG_CF_PKT_RIL_FN
#define QCRIL_LOG_CF_PKT_RIL_FN(instance, label)
#endif

#ifndef QCRIL_LOG_FUNC_RETURN_WITH_RET
#define QCRIL_LOG_FUNC_RETURN_WITH_RET( ... ) \
{ RIL_UTF_LOG_ENTER("function exit with ret %d", ##__VA_ARGS__ ); }
#endif

#ifndef QCRIL_LOG_ASSERT
#define QCRIL_LOG_ASSERT( cond )    ({ if(!(cond)) {RIL_UTF_MSG(LEVEL_DEBUG, "assert" #cond " failed"); assert(cond);} })
#endif

#ifndef QCRIL_NOTUSED
#define QCRIL_NOTUSED(x) ((void)(x))
#endif

#endif
