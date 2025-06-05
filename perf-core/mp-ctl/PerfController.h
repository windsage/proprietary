/******************************************************************************
  @file    PerfController.h
  @brief   Header file for communication and actions for PerfLock

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017,2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_CONTROLLER_H__
#define __PERF_CONTROLLER_H__

#include <pthread.h>
#include <cutils/properties.h>
#include "VendorIPerf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRACE_BUF_SZ 512

#define MIN_CLUSTERS 2

#define MPCTL_VERSION      3
#define OLD_MPCTL_VERSION  2
#define SYNC_REQ_VAL_LEN  100000

#define BOOT_CMPLT_PROP "vendor.post_boot.parsed"
#define BUILD_DEBUGGABLE "ro.debuggable"
#define TARGET_INIT_COMPLETE "vendor.target.init.complete"
#ifndef INDEFINITE_DURATION
#define INDEFINITE_DURATION     0
#endif
#define MPCTL_INIT_COMPLETE "vendor.mpctl.init.complete"

/*All resources are categorized into one of the following types
  based their sysfs node manipulations.*/
enum {
    /*Resources which updates a single normal node can be categorized as SINGLE_NODE.*/
    SINGLE_NODE,
    /* All the resources whose sysfs node paths can go offline depending on the core
    status(offline/online) and also updating a single core automatically updates all the
    cores of that cluster, can be assigned as type INTERACTIVE_NODE.
    Example -  schedutil_hispeed_freq, we find the first online core in the requested cluster
    and update it with requested vlaue. As a result all the cores in that cluster automatically
    gets updated with the same value for schedutil_hispeed_freq.*/
    INTERACTIVE_NODE,
    /*The resources for which we manually update all the cores of both clusters with a same
    value basing on the requested value, can be categorized as UPDATE_ALL_CORES.
    Example - sched_prefer_idle, when we get a acquire request call for this resource, we
    update the given Sysfs node for all the cores of both clusters with a same value.*/
    UPDATE_ALL_CORES,
    /*A special type of UPDATE_ALL_CORES, where we manually update cores of both clusters with
    different values basing on the requested value, can be assigned as UPDATE_CORES_PER_CLUSTER.
    Example - sched_mostly_idle_freq, For an acquire request call we update the given Sysfs node for
    all the cores but with different values for different clusters.*/
    UPDATE_CORES_PER_CLUSTER,
    /*All the resources which provide an option to select a particular core to get updated
    can be assigned as type SELECT_CORE_TO_UPDATE
    Example - sched_static_cpu_pwr_cost, for this resource in the Opcode we can provide the
    exact core number which needs to be updated.*/
    SELECT_CORE_TO_UPDATE,
    /*All other resources which update multiple nodes or needs special treament.*/
    SPECIAL_NODE,
    /*Adding memlat as a node type for handling memlat resource request*/
    MEM_LAT_NODE,
};

enum QIdxMajor {
QINDEX_DISPLAY_OFF_MAJOR_OPCODE = 0,
QINDEX_POWER_COLLAPSE_MAJOR_OPCODE = QINDEX_DISPLAY_OFF_MAJOR_OPCODE + MAX_DISPLAY_MINOR_OPCODE,
QINDEX_CPUFREQ_MAJOR_OPCODE = QINDEX_POWER_COLLAPSE_MAJOR_OPCODE + MAX_PC_MINOR_OPCODE,
QINDEX_SCHED_MAJOR_OPCODE = QINDEX_CPUFREQ_MAJOR_OPCODE + MAX_CPUFREQ_MINOR_OPCODE,
QINDEX_CORE_HOTPLUG_MAJOR_OPCODE = QINDEX_SCHED_MAJOR_OPCODE + MAX_SCHED_MINOR_OPCODE,
QINDEX_INTERACTIVE_MAJOR_OPCODE = QINDEX_CORE_HOTPLUG_MAJOR_OPCODE + MAX_CORE_HOTPLUG_MINOR_OPCODE,
QINDEX_CPUBW_HWMON_MAJOR_OPCODE = QINDEX_INTERACTIVE_MAJOR_OPCODE + MAX_INTERACTIVE_MINOR_OPCODE,
QINDEX_VIDEO_MAJOR_OPCODE = QINDEX_CPUBW_HWMON_MAJOR_OPCODE + MAX_CPUBW_HWMON_MINOR_OPCODE,
QINDEX_KSM_MAJOR_OPCODE = QINDEX_VIDEO_MAJOR_OPCODE + MAX_VIDEO_MINOR_OPCODE,
QINDEX_ONDEMAND_MAJOR_OPCODE = QINDEX_KSM_MAJOR_OPCODE + MAX_KSM_MINOR_OPCODE,
QINDEX_GPU_MAJOR_OPCODE = QINDEX_ONDEMAND_MAJOR_OPCODE + MAX_OND_MINOR_OPCODE,
QINDEX_MISC_MAJOR_OPCODE = QINDEX_GPU_MAJOR_OPCODE + MAX_GPU_MINOR_OPCODE,
QINDEX_LLCBW_HWMON_MAJOR_OPCODE = QINDEX_MISC_MAJOR_OPCODE + MAX_MISC_MINOR_OPCODE,
QINDEX_MEMLAT_MAJOR_OPCODE = QINDEX_LLCBW_HWMON_MAJOR_OPCODE + MAX_LLCBW_HWMON_MINOR_OPCODE,
QINDEX_NPU_MAJOR_OPCODE = QINDEX_MEMLAT_MAJOR_OPCODE + MAX_MEMLAT_MINOR_OPCODE,
QINDEX_SCHED_EXT_MAJOR_OPCODE = QINDEX_NPU_MAJOR_OPCODE + MAX_NPU_MINOR_OPCODE,
};

enum map_type {
    NO_VALUE_MAP = 0,
    VALUE_MAPPED = 1
};

/* Enum for pre-defined cluster map.
 * */
enum predefine_cluster_map {
    START_PRE_DEFINE_CLUSTER = 8,
    LAUNCH_CLUSTER = 8,
    SCROLL_CLUSTER = 9,
    ANIMATION_CLUSTER = 10,
    PREDEFINED_CLUSTER_11 = 11,
    PREDEFINED_CLUSTER_12 = 12,
    PREDEFINED_CLUSTER_13 = 13,
    PREDEFINED_CLUSTER_14 = 14,
    PREDEFINED_CLUSTER_15 = 15,
    MAX_PRE_DEFINE_CLUSTER
};

/* Enum for value mapping for frequency.
 * A target needs to define the mapped
 * values for these.
 * */
enum freq_map {
    LOWEST_FREQ = 0,
    LEVEL1_FREQ = 1,
    LEVEL2_FREQ = 2,
    LEVEL3_FREQ = 3,
    HIGHEST_FREQ = 4,
    FREQ_MAP_MAX
};

/*older resources
 *   supported till v2.0*/

enum resources {
    DISPLAY = 0,
    POWER_COLLAPSE,
    CPU0_MIN_FREQ,
    CPU1_MIN_FREQ,
    CPU2_MIN_FREQ,
    CPU3_MIN_FREQ,
    UNSUPPORTED_0,
    CLUSTR_0_CPUS_ON,
    CLUSTR_0_MAX_CORES,
    UNSUPPORTED_2,
    UNSUPPORTED_3,
    SAMPLING_RATE,
    ONDEMAND_IO_IS_BUSY,
    ONDEMAND_SAMPLING_DOWN_FACTOR,
    INTERACTIVE_TIMER_RATE,
    INTERACTIVE_HISPEED_FREQ,
    INTERACTIVE_HISPEED_LOAD,
    SYNC_FREQ,
    OPTIMAL_FREQ,
    SCREEN_PWR_CLPS,
    THREAD_MIGRATION,
    CPU0_MAX_FREQ,
    CPU1_MAX_FREQ,
    CPU2_MAX_FREQ,
    CPU3_MAX_FREQ,
    ONDEMAND_ENABLE_STEP_UP,
    ONDEMAND_MAX_INTERMEDIATE_STEPS,
    INTERACTIVE_IO_BUSY,
    KSM_RUN_STATUS,
    KSM_PARAMS,
    SCHED_BOOST,
    CPU4_MIN_FREQ,
    CPU5_MIN_FREQ,
    CPU6_MIN_FREQ,
    CPU7_MIN_FREQ,
    CPU4_MAX_FREQ,
    CPU5_MAX_FREQ,
    CPU6_MAX_FREQ,
    CPU7_MAX_FREQ,
    CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY,
    CPU0_INTERACTIVE_BOOST,
    CPU0_INTERACTIVE_BOOSTPULSE,
    CPU0_INTERACTIVE_BOOSTPULSE_DURATION,
    CPU0_INTERACTIVE_GO_HISPEED_LOAD,
    CPU0_INTERACTIVE_HISPEED_FREQ,
    CPU0_INTERACTIVE_IO_IS_BUSY,
    CPU0_INTERACTIVE_MIN_SAMPLE_TIME,
    CPU0_INTERACTIVE_TARGET_LOADS,
    CPU0_INTERACTIVE_TIMER_RATE,
    CPU0_INTERACTIVE_TIMER_SLACK,
    CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY,
    CPU4_INTERACTIVE_BOOST,
    CPU4_INTERACTIVE_BOOSTPULSE,
    CPU4_INTERACTIVE_BOOSTPULSE_DURATION,
    CPU4_INTERACTIVE_GO_HISPEED_LOAD,
    CPU4_INTERACTIVE_HISPEED_FREQ,
    CPU4_INTERACTIVE_IO_IS_BUSY,
    CPU4_INTERACTIVE_MIN_SAMPLE_TIME,
    CPU4_INTERACTIVE_TARGET_LOADS,
    CPU4_INTERACTIVE_TIMER_RATE,
    CPU4_INTERACTIVE_TIMER_SLACK,
    CLUSTR_1_MAX_CORES,
    SCHED_PREFER_IDLE,
    SCHED_MIGRATE_COST,
    SCHED_SMALL_TASK,
    SCHED_MOSTLY_IDLE_LOAD,
    SCHED_MOSTLY_IDLE_NR_RUN,
    SCHED_INIT_TASK_LOAD,
    VIDEO_DECODE_PLAYBACK_HINT,
    DISPLAY_LAYER_HINT,
    VIDEO_ENCODE_PLAYBACK_HINT,
    CPUBW_HWMON_MIN_FREQ,
    CPUBW_HWMON_DECAY_RATE,
    CPUBW_HWMON_IO_PERCENT,
    CPU0_INTERACTIVE_MAX_FREQ_HYSTERESIS,
    CPU4_INTERACTIVE_MAX_FREQ_HYSTERESIS,
    GPU_DEFAULT_PWRLVL,
    CLUSTR_1_CPUS_ON,
    SCHED_UPMIGRATE,
    SCHED_DOWNMIGRATE,
    SCHED_MOSTLY_IDLE_FREQ,
    IRQ_BALANCER,
    INTERACTIVE_USE_SCHED_LOAD,
    INTERACTIVE_USE_MIGRATION_NOTIF,
    INPUT_BOOST_RESET,
    /* represents the maximum number of
     * optimizations allowed per
     * request and should always be
     * the last element
     */
    OPTIMIZATIONS_MAX
};

enum {
    SYNC_CMD_DUMP_DEG_INFO,
};

typedef struct client_msg_int_t {
    int32_t handle;
} client_msg_int_t;

typedef struct {
    char value[PROP_VAL_LENGTH];
} client_msg_str_t;

typedef struct {
    char value[SYNC_REQ_VAL_LEN];
} client_msg_sync_req_t;

typedef enum {
    PERF_LOCK_CMD = 0,
    PERF_LOCK_RELEASE,
    PERF_HINT,
    PERF_GET_PROP,
    PERF_LOCK_ACQUIRE,
    PERF_LOCK_ACQUIRE_RELEASE,
    PERF_GET_FEEDBACK,
    PERF_EVENT,
    PERF_HINT_ACQUIRE_RELEASE,
    PERF_HINT_RENEW,
    PERF_SYNC_REQ,
} PerfAPI_t;

typedef enum {
    PERF_LOCK_TYPE = 0,
    PERF_GET_PROP_TYPE,
    PERF_SYNC_REQ_TYPE,
} ReplyMsgType_t;

typedef enum {
    PERF_AVC_TEST_TYPE = 0,
} PerfTestType;

#ifdef __cplusplus
}
#endif

#endif /* __PERF_CONTROLLER_H__ */
