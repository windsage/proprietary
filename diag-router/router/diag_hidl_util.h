/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service utils implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef __DIAG_HIDL_UTIL_H__
#define __DIAG_HIDL_UTIL_H__
#include <log/log.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <signal.h>
#include <string>
#include <map>
#include <memory>
#include <vendor/qti/diaghal/1.0/Idiagcallback.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <hidl/LegacySupport.h>

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using vendor::qti::diaghal::V1_0::Idiagcallback;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
#define DIAG_IOCTL_COMMAND_REG       0 /* IOCTL for packet registration
                                  Clients can use this to register to respond to packets from host tool */
#define DIAG_IOCTL_COMMAND_DEREG     1 /* IOCTL for de-registration */
/* Client process uses this to de-register itself, while unloading gracefully. */
#define DIAG_IOCTL_MASK_REG          2 /* IOCTL for registration for mask-change */
#define DIAG_IOCTL_MASK_DEREG        3
#define DIAG_IOCTL_GETEVENTMASK      4 /* For Client process to get event mask from DCM */
#define DIAG_IOCTL_GETLOGMASK        5
#define DIAG_IOCTL_GETMSGMASK        6
#define DIAG_IOCTL_GET_DELAYED_RSP_ID  8 /* Diag_LSM uses this IOCTL to get the next delayed response id in the system. */
#define DIAG_IOCTL_LSM_DEINIT		9
#define DIAG_IOCTL_SWITCH_LOGGING	7
#define DIAG_IOCTL_DCI_INIT		20
#define DIAG_IOCTL_DCI_DEINIT		21
#define DIAG_IOCTL_DCI_SUPPORT		22
#define DIAG_IOCTL_DCI_REG		23
#define DIAG_IOCTL_DCI_STREAM_INIT	24
#define DIAG_IOCTL_DCI_HEALTH_STATS	25
#define DIAG_IOCTL_DCI_LOG_STATUS	26
#define DIAG_IOCTL_DCI_EVENT_STATUS	27
#define DIAG_IOCTL_DCI_CLEAR_LOGS	28
#define DIAG_IOCTL_DCI_CLEAR_EVENTS	29
#define DIAG_IOCTL_REMOTE_DEV		32
#define DIAG_IOCTL_VOTE_REAL_TIME	33
#define DIAG_IOCTL_GET_REAL_TIME	34
#define DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE	35
#define DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE	36
#define DIAG_IOCTL_REGISTER_CALLBACK	37
#define DIAG_IOCTL_HDLC_TOGGLE		38
#define DIAG_IOCTL_QUERY_PD_LOGGING	39
#define DIAG_IOCTL_QUERY_CON_ALL	40
#define DIAG_IOCTL_QUERY_MD_PID		41
#define DIAG_IOCTL_MDM_HDLC_TOGGLE	44
#define DIAG_IOCTL_QUERY_MASK 		45
#define DIAG_IOCTL_SET_OVERRIDE_PID	47
#define DATA_TYPE_UPDATE_QDSS_ETR1_SUPPORT	48
#define DIAG_IOCTL_VOTE_DCI_BUFFERING_MODE	49
#define DIAG_IOCTL_DCI_DRAIN_IMMEDIATE	50

void add_client_info_to_list(const sp<Idiagcallback>& callback, pid_t pid);
int add_client_registrations_to_apps(void *data, int len, pid_t pid);
void remove_client_registrations_from_apps(pid_t pid);
void remove_client_info_from_list(pid_t pid);
int diag_fwd_command_to_client(void * buf, int len, pid_t pid);
sp<Idiagcallback> get_client_info_from_pid(pid_t pid);
hidl_memory diag_get_hidl_buffer();
#endif
