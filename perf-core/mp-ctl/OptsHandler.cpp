/******************************************************************************
  @file    OptsHandler.cpp
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-OPTSHANDLER"
#include "PerfLog.h"
#include <config.h>
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <dlfcn.h>
#include "Request.h"
#include "MpctlUtils.h"
#include "OptsData.h"
#include "ResourceQueues.h"
#include "Target.h"
#include "ResourceInfo.h"
#include "OptsHandler.h"
#include "PerfController.h"
#include "BoostConfigReader.h"
#include "SecureOperations.h"

#include "OptsHandlerExtn.h"

#define ENABLE_PREFER_IDLE  1
#define WRITE_MIGRATE_COST  0

#define DISABLE_PC_LATENCY  1
#define MAX_LPM_BIAS_HYST 100
#define DEFAULT_LPM_BIAS_HYST 0

#define CPUSET_STR_SIZE 86

#define RELEASE_LOCK 0
#define ACQUIRE_LOCK 1
#define BASE_10 10

#define ACQ 1
#define REL 0
#define UP 1
#define DOWN 0
typedef enum {
    UPDOWN,
    DOWNUP
} MIG_SEQ;

OptsHandler OptsHandler::mOptsHandler;
std::vector<SchedPolicyTable> OptsHandler::mSP;

namespace {
    pthread_t id_ka_thread;
    bool tKillFlag = true;
    bool isThreadAlive = false;
    pthread_mutex_t ka_mutex = PTHREAD_MUTEX_INITIALIZER;
}

OptsHandler::OptsHandler() {
    LoadSecureLibrary(true);
    InitializeOptsTable();
}

OptsHandler::~OptsHandler() {
    LoadSecureLibrary(false);
}

int32_t OptsHandler::Init() {
    int32_t retval = 0;
    init_pasr();
    retval = init_nr_thread();
    if (retval != SUCCESS) {
        QLOGE(LOG_TAG, "Could not create noroot_thread");
    }
    return retval;

}


int32_t (*mApplyValue)(int32_t, int16_t, int16_t, bool) = NULL;
void OptsHandler::LoadSecureLibrary(bool onoff) {
    QLOGV(LOG_TAG, "LoadSecureLibrary");
    if (onoff) {
        mLibHandle = dlopen("libqti-qesdk-secure.so", RTLD_NOW);
        if (mLibHandle) {
            mApplyValue = (int32_t (*)(int32_t, int16_t, int16_t, bool))dlsym(mLibHandle, "apply_value");
            if (!mApplyValue) {
                QLOGE(LOG_TAG, "Unable to get apply_value function handle.");
            }
        } else {
            QLOGE(LOG_TAG, "Unable to open %s: %s", "libqti-qesdk-secure.so", dlerror());
        }
    } else {
        if (mLibHandle)
            if (dlclose(mLibHandle))
                QLOGE(LOG_TAG, "Error occurred while closing library handle.");
    }
}

void OptsHandler::InitializeOptsTable() {
/* Assuming that all the information related to resources, such as CompareOpts., are dependend only the
resource and not on targets. If this assumption changes it's better to move mOptsTable to TargetInit.
 * Assigning default ApplyOts and ResetOpts function to all the resources and also CompareOpts to
 higher_is_better by default. If any resource does not follow the default types, they need to be changed.*/

    int32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    uint32_t idx = 0;
    for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
        mOptsTable[idx] = {modify_sysfsnode, modify_sysfsnode, higher_is_better};
    }

    /* changing CompareOpts to lower_is_better basing on the resource index, which is
       the sum of it's major group start id and minor id. */
    mOptsTable[POWER_COLLAPSE_START_INDEX + L2_POWER_COLLAPSE_PERF_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_STATIC_CPU_PWR_COST_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_RESTRICT_CLUSTER_SPILL_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_FREQ_AGGR_THRESHOLD_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_UTIL_BUSY_HYST_CPU_NS] = {sched_util_busy_hyst_cpu_ns, sched_util_busy_hyst_cpu_ns, higher_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_FORCE_LB_ENABLE].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_MIN_TASK_UTIL_FOR_UCLAMP].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_RATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_SLACK_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + INTERACTIVE_MAX_FREQ_HYSTERESIS_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + SCHEDUTIL_HISPEED_LOAD_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + SCHEDUTIL_TARGET_LOAD_THRESH_OPCODE].mCompareOpts = higher_is_better;
    mOptsTable[INTERACTIVE_START_INDEX + SCHEDUTIL_TARGET_LOAD_SHIFT_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[ONDEMAND_START_INDEX + OND_SAMPLING_RATE_OPCODE].mCompareOpts = lower_is_better;
    mOptsTable[GPU_START_INDEX + GPU_MIN_POWER_LEVEL].mCompareOpts = lower_is_better;

    /* changing CompareOpts to always_apply basing on the resource index.*/
    mOptsTable[SCHED_START_INDEX + SCHED_WINDOW_TICKS_UPDATE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_TOP_APP_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_SYSTEM_BACKGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_CPUSET_BACKGROUND_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_CPUSET_AUDIO_APP_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHEDTUNE_PREFER_IDLE_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_TUNE_PREFER_IDLE_FOREGROUND].mCompareOpts = always_apply;
    /* This is a temp fix for the usable perf mode get precedence. Will revert once a long term fix is available */
    mOptsTable[SCHED_START_INDEX + SCHED_BUSY_HYSTERSIS_CPU_MASK_OPCODE].mCompareOpts = higher_is_better;
    mOptsTable[SCHED_START_INDEX + SCHED_BUSY_HYSTERESIS_ENABLE_COLOC_CPUS].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_COLOC_BIAS_HYST].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + SCHED_WINDOW_STATS_POLICY].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + STUNE_TOPAPP_SCHEDTUNE_COLOCATE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + CPUCTL_TOPAPP_UCLAMP_LATENCY_SENSITIVE].mCompareOpts = always_apply;
    mOptsTable[SCHED_START_INDEX + CPUCTL_FG_UCLAMP_LATENCY_SENSITIVE].mCompareOpts = always_apply;
    mOptsTable[INTERACTIVE_START_INDEX + SCHEDUTIL_PREDICTIVE_LOAD_OPCODE].mCompareOpts = always_apply;
    mOptsTable[MISC_START_INDEX + STORAGE_CLK_SCALING_DISABLE_OPCODE].mCompareOpts = always_apply;
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_ASYMCAP_BOOST].mCompareOpts = lower_is_better;

    /* changing ApplyOpts and ResetOpts functions to call different function.*/
    mOptsTable[DISPLAY_START_INDEX + DISPLAY_OFF_OPCODE] = {dummy, dummy, higher_is_better};
    mOptsTable[MISC_START_INDEX + DISPLAY_DOZE_OPCODE] = {dummy, dummy, always_apply};

    mOptsTable[POWER_COLLAPSE_START_INDEX + POWER_COLLAPSE_OPCODE] =
                        {pmQoS_cpu_dma_latency, pmQoS_cpu_dma_latency, lower_is_better};

    /* cpu freq */
    mOptsTable[CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE] = {cpu_options, cpu_options, higher_is_better};
    mOptsTable[CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE] = {cpu_options, cpu_options, higher_is_better};
    mOptsTable[CPUFREQ_START_INDEX + SCALING_MIN_FREQ_OPCODE] = {set_scaling_min_freq, set_scaling_min_freq, higher_is_better};

    /* sched group */
    mOptsTable[SCHED_START_INDEX + SCHED_BOOST_OPCODE] = {set_sched_boost, reset_sched_boost, always_apply};
    mOptsTable[SCHED_START_INDEX + SCHED_UPMIGRATE_OPCODE] = {migrate, migrate, lower_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_DOWNMIGRATE_OPCODE] = {migrate, migrate, lower_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_GROUP_OPCODE] = {dummy, dummy, always_apply};
    mOptsTable[SCHED_START_INDEX + SCHED_FREQ_AGGR_GROUP_OPCODE] =
                        {dummy, dummy, always_apply};
    mOptsTable[SCHED_START_INDEX + SCHED_TASK_BOOST] =
                        {sched_task_boost, sched_reset_task_boost, add_in_order};
    mOptsTable[SCHED_START_INDEX + SCHED_ENABLE_TASK_BOOST_RENDERTHREAD] =
                        {sched_enable_task_boost_renderthread, sched_reset_task_boost,
                         add_in_order_fps_based_taskboost};
    mOptsTable[SCHED_START_INDEX + SCHED_DISABLE_TASK_BOOST_RENDERTHREAD] =
                        {sched_reset_task_boost, sched_reset_task_boost,
                         add_in_order_fps_based_taskboost};
    mOptsTable[SCHED_START_INDEX + SCHED_LOW_LATENCY] =
                        {sched_low_latency, sched_reset_low_latency, add_in_order};
    mOptsTable[SCHED_START_INDEX + SCHED_WAKE_UP_IDLE] =
                        {sched_low_latency, sched_reset_low_latency, add_in_order};
    mOptsTable[SCHED_START_INDEX + SCHED_COLOC_BUZY_HYST_CPU_PCT] =
                        {sched_coloc_busy_hyst_cpu_busy_pct, sched_coloc_busy_hyst_cpu_busy_pct, lower_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_COLOC_BUZY_HYST_CPU_NS] =
                        {sched_coloc_busy_hyst_cpu_ns, sched_coloc_busy_hyst_cpu_ns, higher_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_UP_DOWN_MIGRATE] =
                        {migrate_action_apply, migrate_action_release, migrate_lower_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_GROUP_UP_DOWN_MIGRATE] =
                        {grp_migrate_action_apply, grp_migrate_action_release, migrate_lower_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_TASK_LOAD_BOOST] =
                        {sched_task_load_boost, sched_reset_task_load_boost, add_in_order};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_CLUSTER_UTIL_THRES_PCT] =
                        {sched_util_thres_pct_system, sched_util_thres_pct_system, lower_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_UTIL_THRES_PCT_CLUST] =
                        {sched_util_thres_pct_clust, sched_util_thres_pct_clust, lower_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_IDLE_ENOUGH] =
                        {sched_idle_enough_clust_system, sched_idle_enough_clust_system, lower_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_IDLE_ENOUGH_CLUST] =
                        {sched_idle_enough_clust, sched_idle_enough_clust, lower_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_EM_INFLATE_PCT].mCompareOpts = higher_is_better;
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_EM_INFLATE_THRES].mCompareOpts = lower_is_better;
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_FMAX_CAP] =
                        {sched_fmax_cap, sched_fmax_cap, higher_is_better};
    mOptsTable[SCHED_EXT_START_INDEX + SCHED_MAX_FREQ_PARTIAL_HALT].mCompareOpts = higher_is_better;

    /* hotplug */
    mOptsTable[CORE_HOTPLUG_START_INDEX + CORE_HOTPLUG_MIN_CORE_ONLINE_OPCODE] =
                        {lock_min_cores, lock_min_cores, higher_is_better};
    mOptsTable[CORE_HOTPLUG_START_INDEX + CORE_HOTPLUG_MAX_CORE_ONLINE_OPCODE] =
                        {lock_max_cores, lock_max_cores, higher_is_better};

    /* cpubw hwmon */
    // Do the following only for kernel 5.10 & above
    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        mOptsTable[CPUBW_HWMON_START_INDEX + CPUBW_HWMON_HYST_OPT_OPCODE] =
                        {bus_dcvs_hyst_opt, bus_dcvs_hyst_opt, higher_is_better};
    } else {
        mOptsTable[CPUBW_HWMON_START_INDEX + CPUBW_HWMON_HYST_OPT_OPCODE] =
                        {cpubw_hwmon_hyst_opt, cpubw_hwmon_hyst_opt, higher_is_better};
    }

    /* video */
    mOptsTable[VIDEO_START_INDEX + VIDEO_ENCODE_PB_HINT] =
                        {handle_vid_encplay_hint, handle_vid_encplay_hint, higher_is_better};
    mOptsTable[VIDEO_START_INDEX + VIDEO_DECODE_PB_HINT] =
                        {handle_vid_decplay_hint, handle_vid_decplay_hint, higher_is_better};
    mOptsTable[VIDEO_START_INDEX + VIDEO_DISPLAY_PB_HINT] =
                        {handle_disp_hint, handle_disp_hint, higher_is_better};

    /* display */
    mOptsTable[VIDEO_START_INDEX + DISPLAY_EARLY_WAKEUP_HINT] =
                        {handle_early_wakeup_hint, handle_early_wakeup_hint, always_apply};

    /* ksm */
    mOptsTable[KSM_START_INDEX + KSM_ENABLE_DISABLE_OPCODE] =
                        {disable_ksm, enable_ksm, higher_is_better};
    mOptsTable[KSM_START_INDEX + KSM_SET_RESET_OPCODE] =
                        {set_ksm_param, reset_ksm_param, higher_is_better};

    /* gpu */
    mOptsTable[GPU_START_INDEX + GPU_MAX_FREQ_OPCODE] =
                        {gpu_maxfreq, gpu_maxfreq, higher_is_better};
    mOptsTable[GPU_START_INDEX + GPU_DISABLE_GPU_NAP_OPCODE] =
                        {gpu_disable_gpu_nap, gpu_disable_gpu_nap, higher_is_better};
    mOptsTable[GPU_START_INDEX + GPU_IS_APP_FG_OPCODE] = {gpu_is_app_fg, dummy, always_apply};
    mOptsTable[GPU_START_INDEX + GPU_IS_APP_BG_OPCODE] = {gpu_is_app_bg, dummy, always_apply};
    mOptsTable[GPU_START_INDEX + GPU_LOAD_MOD_PERCENT_OPCODE] = {apply_value, apply_value, always_apply};

    /* miscellaneous */
    mOptsTable[MISC_START_INDEX + UNSUPPORTED_OPCODE] = {unsupported, unsupported, higher_is_better};
    mOptsTable[MISC_START_INDEX + IRQ_BAL_OPCODE] = {irq_balancer, irq_balancer, higher_is_better};
    mOptsTable[MISC_START_INDEX + NET_KEEP_ALIVE_OPCODE] = {keep_alive, dummy, higher_is_better};
    mOptsTable[MISC_START_INDEX + PID_AFFINE] = {set_pid_affine, reset_pid_affine, always_apply};
    mOptsTable[MISC_START_INDEX + DISABLE_PASR] = {perfmode_entry_pasr, perfmode_exit_pasr, always_apply};
    mOptsTable[MISC_START_INDEX + FPS_HYST_OPCODE] = {handle_fps_hyst, handle_fps_hyst, higher_is_better};
    mOptsTable[MISC_START_INDEX + ALWAYS_ALLOW_OPCODE] = {dummy, dummy, always_apply};
    mOptsTable[MISC_START_INDEX + SET_SCHEDULER] = {set_scheduler, reset_scheduler, add_in_order};
    mOptsTable[MISC_START_INDEX + SCHED_THREAD_PIPELINE] =
                        {sched_thread_pipeline, sched_reset_thread_pipeline, add_in_order};
    mOptsTable[MISC_START_INDEX + DISPLAY_HEAVY_RT_AFFINE] =
                        {set_display_heavy_rt_affine, reset_display_heavy_rt_affine, always_apply};
    mOptsTable[MISC_START_INDEX + CONTENT_FPS_OPCODE].mCompareOpts = always_apply;
    mOptsTable[MISC_START_INDEX + GOLD_DYNPREFETCHER_OPCODE] =
                        {enable_gold_dynprefetcher, disable_gold_dynprefetcher, always_apply};

    /*llcbw hwmon*/
    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        mOptsTable[LLCBW_HWMON_START_INDEX + LLCBW_HWMON_HYST_OPT_OPCODE] =
                        {bus_dcvs_hyst_opt, bus_dcvs_hyst_opt, higher_is_better};

        mOptsTable[LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_HYST_OPT] =
                        {bus_dcvs_hyst_opt, bus_dcvs_hyst_opt, higher_is_better};

        mOptsTable[CPUBW_HWMON_START_INDEX + CPU_LLCC_BW_HYST_OPT] =
                        {bus_dcvs_hyst_opt, bus_dcvs_hyst_opt, higher_is_better};
    }
    else {
        mOptsTable[LLCBW_HWMON_START_INDEX + LLCBW_HWMON_HYST_OPT_OPCODE] =
                        {llcbw_hwmon_hyst_opt, llcbw_hwmon_hyst_opt, higher_is_better};

        mOptsTable[LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_HYST_OPT] =
                        {llcbw_hwmon_hyst_opt, llcbw_hwmon_hyst_opt, higher_is_better};

        mOptsTable[CPUBW_HWMON_START_INDEX + CPU_LLCC_BW_HYST_OPT] =
                        {cpubw_hwmon_hyst_opt, cpubw_hwmon_hyst_opt, higher_is_better};
    }


    /* memlat */
    mOptsTable[MEMLAT_START_INDEX + L3_MEMLAT_MINFREQ_OPCODE] = {l3_min_freq, l3_min_freq, higher_is_better};
    mOptsTable[MEMLAT_START_INDEX + L3_MEMLAT_MAXFREQ_OPCODE] = {l3_max_freq, l3_max_freq, higher_is_better};
    mOptsTable[MEMLAT_START_INDEX + BUS_DCVS_LLCC_DDR_BOOST_FREQ].mCompareOpts = higher_is_better;
    mOptsTable[MEMLAT_START_INDEX + BUS_DCVS_LLCC_L3_BOOST_FREQ].mCompareOpts = higher_is_better;
    mOptsTable[MEMLAT_START_INDEX + BUS_DCVS_LLCC_LLCC_BOOST_FREQ].mCompareOpts = higher_is_better;
    mOptsTable[MEMLAT_START_INDEX + BUS_DCVS_LLCC_DDRQOS_BOOST_FREQ].mCompareOpts = higher_is_better;
    mOptsTable[MEMLAT_START_INDEX + BUS_DCVS_MEMLAT_SAMPLE_MS_OPCODE].mCompareOpts = lower_is_better;


    /* npubw_llcbw_ddr hwmon */
    mOptsTable[NPU_START_INDEX + NPU_LLC_DDR_HWMON_HYST_OPT_OPCODE] =
                        {npubw_hwmon_hyst_opt, npubw_hwmon_hyst_opt, higher_is_better};
    /*npu_llcbw hwmon*/
    mOptsTable[NPU_START_INDEX + NPU_LLCBW_HWMON_HYST_OPT_OPCODE] =
                        {npu_llcbw_hwmon_hyst_opt, npu_llcbw_hwmon_hyst_opt, higher_is_better};
    mOptsTable[SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE].mCompareOpts = higher_is_better_negative;
}

int32_t OptsHandler::ApplyOpt(Resource &resObj) {
    int32_t ret = FAILED;
    uint16_t idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mApplyOpts(resObj, dataObj);
    } else {
        QLOGE(LOG_TAG, "Failed to call apply optimization for 0x%" PRIx16, resObj.qindex);
    }
    return ret;
}

int32_t OptsHandler::ResetOpt(Resource &resObj) {
    int32_t ret = FAILED;
    uint16_t idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mResetOpts(resObj, dataObj);
    } else {
        QLOGE(LOG_TAG, "Failed to call reset optimization for 0x%" PRIx16, idx);
    }
    return ret;
}

int32_t OptsHandler::CompareOpt(uint32_t qindx, uint32_t reqVal, uint32_t curVal) {
    int32_t ret = ADD_NEW_REQUEST;

    //First resource id present
    if (qindx >=0 && qindx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[qindx].mCompareOpts(reqVal, curVal);
    } else {
        QLOGE(LOG_TAG, "Cannot find a compareOpt function");
        return FAILED;
    }
    return ret;
}

int32_t OptsHandler::ValidateClusterAndCore(int8_t cluster, int8_t core, uint8_t resourceStatus, uint8_t nodeType) {
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    int8_t supportedCore = -1, core_no = -1;

    if (resourceStatus == CORE_INDEPENDENT && nodeType == SINGLE_NODE) {
        return SUCCESS;
    }

    //First check cluster and core are in valid range
    if ((cluster < 0) || (cluster > tc.getNumCluster())) {
        QLOGE(LOG_TAG, "Invalid cluster no. %" PRId8, cluster);
        return FAILED;
    }
    if ((core < 0) || (core > t.getLastCoreIndex(cluster))) {
        QLOGE(LOG_TAG, "Invalid core no. %" PRId8, core);
        return FAILED;
    }

    //Second check resource is supported on that core or not
    core_no = t.getFirstCoreIndex(cluster);
    switch (resourceStatus) {
        case SYNC_CORE: //Resource is sync core based, only first core of cluster is supported
            if ((core != 0) && (core != core_no)) {
                QLOGE(LOG_TAG, "Core %" PRId8 " is not supported for this perflock resource, instead use core 0/%" PRId8, core, core_no);
                return FAILED;
            }
            break;
       case ASYNC_CORE: //Resource is async core based, all core accepted
            QLOGL(LOG_TAG, QLOG_L2, "Core %" PRId8 " is supported for this perflock resource", core);
            break;
       case CORE_INDEPENDENT: //Resource does not depend on core, only first core of perf cluster is accepted
            supportedCore = t.getFirstCoreOfPerfCluster();
            if (supportedCore == -1 && tc.getIsDefaultDivergent()) {
                // for a divergent target, a core independent opcode can accept any core and cluster
                QLOGE(LOG_TAG, "Core %" PRId8 " is not supported for this perflock resource, instead use core %" PRId8, core, supportedCore);
                return FAILED;
            }
            break;
       default:
            QLOGE(LOG_TAG, "Invalid resource based core status");
            return FAILED;
    }
    return SUCCESS;
}

/*All possible modifications to the requested value and any pre acquire node
updations can to be done in this function and finally update the acqval with
the string with which the requested node needs to be acquired.*/
int32_t OptsHandler::CustomizeRequestValue(Resource &r, OptsData &d, char *acqval) {
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;
    int32_t rc = FAILED, setval = FAILED;
    int8_t cpu = -1;
    char *tmp_here = NULL, tmp_node[NODE_MAX] = "";
    bool valid_bound = true;
    int8_t valCluster1 = 0, valCluster2 = 0;
    uint32_t hwmon_max_freq_index = 0, hwmon_min_freq_index = 0;
    int32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    Target &target = Target::getCurTarget();
    char tmp_val[NODE_MAX] = "";
    int8_t coresInCluster = 0, cluster = -1, rem = -1, tmpval = 0, i = 0;
    int8_t not_pref[MAX_CORES];
    uint32_t str_index = 0;

    switch (idx) {
    case POWER_COLLAPSE_START_INDEX + L2_POWER_COLLAPSE_PERF_OPCODE:
    case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
    case SCHED_START_INDEX + SCHED_ENABLE_THREAD_GROUPING_OPCODE:
    case SCHED_START_INDEX + SCHED_RESTRICT_CLUSTER_SPILL_OPCODE:
    case SCHED_START_INDEX + SCHEDTUNE_PREFER_IDLE_OPCODE:
    case SCHED_START_INDEX + SCHED_TUNE_PREFER_IDLE_FOREGROUND:
    case SCHED_START_INDEX + STUNE_TOPAPP_SCHEDTUNE_COLOCATE:
    case SCHED_START_INDEX + CPUCTL_TOPAPP_UCLAMP_LATENCY_SENSITIVE:
    case SCHED_START_INDEX + CPUCTL_FG_UCLAMP_LATENCY_SENSITIVE:
    case ONDEMAND_START_INDEX + OND_IO_IS_BUSY_OPCODE:
    case ONDEMAND_START_INDEX + OND_ENABLE_STEP_UP:
    case INTERACTIVE_START_INDEX + INTERACTIVE_BOOST_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_IO_IS_BUSY_OPCODE:
    case INTERACTIVE_START_INDEX + SCHEDUTIL_PREDICTIVE_LOAD_OPCODE:
        reqval = !!reqval;
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case SCHED_START_INDEX + SCHED_MIGRATE_COST_OPCODE:
        snprintf(acqval, NODE_MAX, "%" PRId8, WRITE_MIGRATE_COST);
        break;

    case SCHED_START_INDEX + SCHED_CPUSET_TOP_APP_OPCODE:
    case SCHED_START_INDEX + SCHED_CPUSET_SYSTEM_BACKGROUND_OPCODE:
    case SCHED_START_INDEX + SCHED_CPUSET_BACKGROUND_OPCODE:
    case SCHED_EXT_START_INDEX + SCHED_CPUSET_AUDIO_APP_OPCODE:
        tmp_here = cpuset_bitmask_to_str(reqval);
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            delete[] tmp_here;
        } else {
            QLOGE(LOG_TAG, "Failed to get the cpuset bitmask for requested %" PRIu32 " value", reqval);
            return FAILED;
        }
        break;

    case SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE:
        /* Assumption, foreground/boost/cpus and foreground/cpus have same
        bitmask all the times. During acquire first update foreground/boost/cpus
        follwed by foreground/cpus and during release first release foreground/cpus
        foolowed by foreground/boost/cpus */
        tmp_here = cpuset_bitmask_to_str(reqval);
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            FWRITE_STR(SCHED_FOREGROUND_BOOST, acqval, strlen(acqval), rc);
            QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %s, return value %" PRId32, SCHED_FOREGROUND_BOOST, acqval, rc);
            delete[] tmp_here;
        } else {
            QLOGE(LOG_TAG, "Failed to get the cpuset bitmask for requested value = %" PRIu32, reqval);
            return FAILED;
        }
        break;

    case SCHED_START_INDEX + SCHEDTUNE_BOOST_OPCODE:
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE:
        /* Allow negative value of SCHED_LOAD_BOOST */
        snprintf(acqval, NODE_MAX, "%" PRId32, reqval);
        break;

    case POWER_COLLAPSE_START_INDEX + LPM_BIAS_HYST_OPCODE:
        kernelMajor = tc.getKernelMajorVersion();
        kernelMinor = tc.getKernelMinorVersion();

        if (reqval <= 0 || reqval >= MAX_LPM_BIAS_HYST) {
            QLOGL(LOG_TAG, QLOG_L2, "Requested value is %" PRIu32 " out of limits, resetting to default bias of %" PRId8,
                                                        reqval, DEFAULT_LPM_BIAS_HYST);
            reqval = DEFAULT_LPM_BIAS_HYST;
        }

        if ((kernelMajor == 4 && kernelMinor >= 19) || kernelMajor > 4) {
            /* Value passed is in milliseconds; convert to nanoseconds */
            reqval = SecInt::Multiply(reqval, 1000000ul);
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    //todo: check this
    case INTERACTIVE_START_INDEX + INTERACTIVE_USE_SCHED_LOAD_OPCODE:
    //todo: need to ask power team to send a value instead of setting it to zero over here
    case INTERACTIVE_START_INDEX + INTERACTIVE_USE_MIGRATION_NOTIF_OPCODE:
    /* turn off ignore_hispeed_on_notify */
    case INTERACTIVE_START_INDEX + INTERACTIVE_IGNORE_HISPEED_NOTIF_OPCODE:
    /* Removes input boost during keypress scenarios */
    case MISC_START_INDEX + INPUT_BOOST_RESET_OPCODE:
        for(i = 0;i < tc.getTotalNumCores();i++)
            strlcat(acqval,"0 ",NODE_MAX);
        break;

    case INTERACTIVE_START_INDEX + SCHEDUTIL_HISPEED_FREQ_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_HISPEED_FREQ_OPCODE:
        if (reqval > 0 ) {
            reqval = SecInt::Multiply(reqval, FREQ_MULTIPLICATION_FACTOR);
            reqval = d.find_next_cpu_frequency(r.core, reqval);
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case ONDEMAND_START_INDEX + OND_SYNC_FREQ_OPCODE:
    case ONDEMAND_START_INDEX + OND_OPIMAL_FREQ_OPCODE:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, FREQ_MULTIPLICATION_FACTOR);
            reqval = d.find_next_avail_freq(reqval);
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case SCHED_START_INDEX + SCHED_PREFER_IDLE_OPCODE:
        snprintf(acqval, NODE_MAX, "%" PRId8, ENABLE_PREFER_IDLE);
        break;

    case CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MAXFREQ_OPCODE:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, 100ul);
            setval = d.find_next_cpubw_available_freq(reqval);
        }
        if (setval == FAILED) {
            QLOGE(LOG_TAG, "Error! Perflock failed, invalid freq value %" PRId32, setval);
            return FAILED;
        }
        snprintf(acqval, NODE_MAX, "%" PRId32, setval);
        /* read hwmon min_freq */
        hwmon_min_freq_index = CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MINFREQ_OPCODE;
        FREAD_STR(d.sysfsnode_path[hwmon_min_freq_index], tmp_node, NODE_MAX, rc);
        if (rc < 0) {
            QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[hwmon_min_freq_index]);
            return FAILED;
        }
        /*max_freq cannot be lower than min_freq, if reqval
        is lower than minfreq, request is denied*/
        valid_bound = minBoundCheck(d.sysfsnode_storage[idx], tmp_node, acqval, d.cpubw_avail_freqs[d.cpubw_avail_freqs_n-1]);
        if (!valid_bound) {
            QLOGE(LOG_TAG, "Min bounds check failed");
            return FAILED;
        }
        break;

    /*new opcode for cpu-llcc-ddr-bw max_freq node*/
    case LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_MAX_FREQ:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, 100ul);
            setval = d.find_next_llccbw_available_freq(reqval);
        }
        if (setval == FAILED) {
            QLOGE(LOG_TAG, "Error! Perflock failed, invalid freq value %" PRId32, setval);
            return FAILED;
        }
        snprintf(acqval, NODE_MAX, "%" PRId32, setval);
        /* read hwmon min_freq */
        hwmon_min_freq_index = LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_MIN_FREQ;
        FREAD_STR(d.sysfsnode_path[hwmon_min_freq_index], tmp_node, NODE_MAX, rc);
        if (rc < 0) {
            QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[hwmon_min_freq_index]);
            return FAILED;
        }
        /*max_freq cannot be lower than min_freq, if reqval
        is lower than minfreq, request is denied*/
        valid_bound = minBoundCheck(d.sysfsnode_storage[idx], tmp_node, acqval, d.llcbw_avail_freqs[d.llcbw_avail_freqs_n-1]);
        if (!valid_bound) {
            QLOGE(LOG_TAG, "Min bounds check failed");
            return FAILED;
        }
        break;

     /*new opcode for cpu-llcc-ddr-bw min_freq node*/
     case LLCBW_HWMON_START_INDEX + LLCC_DDR_BW_MIN_FREQ:
         if (reqval > 0) {
             reqval = SecInt::Multiply(reqval, 100ul);
             tmp_here = get_devfreq_new_val(reqval, d.llcbw_avail_freqs, d.llcbw_avail_freqs_n, d.llcbw_maxfreq_path);
         }
         if (tmp_here != NULL) {
             strlcpy(acqval, tmp_here, NODE_MAX);
             free(tmp_here);
         } else {
             QLOGE(LOG_TAG, "Unable to find the new devfreq_val for the requested value %" PRIu32, reqval);
             return FAILED;
         }
         break;

     /*new opcode for cpu-cpu-llcc-bw min_freq node*/
     case CPUBW_HWMON_START_INDEX + CPU_LLCC_BW_MIN_FREQ:
         if (reqval > 0) {
             reqval = SecInt::Multiply(reqval, 100ul);
             tmp_here = get_devfreq_new_val(reqval, d.cpubw_avail_freqs, d.cpubw_avail_freqs_n, d.cpubw_maxfreq_path);
         }
         if (tmp_here != NULL) {
             strlcpy(acqval, tmp_here, NODE_MAX);
             free(tmp_here);
         } else {
             QLOGE(LOG_TAG, "Unable to find the new devfreq_val for the requested value %" PRIu32, reqval);
             return FAILED;
         }
         break;

    case CPUBW_HWMON_START_INDEX + CPUBW_HWMON_MINFREQ_OPCODE:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, 100ul);
            tmp_here = get_devfreq_new_val(reqval, d.cpubw_avail_freqs, d.cpubw_avail_freqs_n, d.cpubw_maxfreq_path);
        }
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            free(tmp_here);
        } else {
            QLOGE(LOG_TAG, "Unable to find the new devfreq_val for the requested value %" PRIu32, reqval);
            return FAILED;
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_ABOVE_HISPEED_DELAY_OPCODE:
        if (reqval == 0xFE) {
            snprintf(acqval, NODE_MAX, "19000 1400000:39000 1700000:19000");
        } else if (reqval == 0xFD) {
            snprintf(acqval, NODE_MAX, "%" PRId16, 19000);
        } else {
            reqval = SecInt::Multiply(reqval, 10000ul);
            snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_TARGET_LOADS_OPCODE:
        if (reqval == 0xFE) {
            snprintf(acqval, NODE_MAX, "85 1500000:90 1800000:70");
        } else {
            snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        }
        break;

    case SCHED_START_INDEX + SCHED_MOSTLY_IDLE_FREQ_OPCODE:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, FREQ_MULTIPLICATION_FACTOR);
            if ((cpu = get_online_core(0, target.getLastCoreIndex(0)))!= FAILED) {
                 valCluster1 =  d.find_next_cpu_frequency(cpu, reqval);
                QLOGL(LOG_TAG, QLOG_L2, "valCluster1=%" PRId8, valCluster1);
            }
            if ((cpu = get_online_core(target.getLastCoreIndex(0)+1, target.getLastCoreIndex(1))) != FAILED) {
                 valCluster2 =  d.find_next_cpu_frequency(cpu, reqval);
                QLOGL(LOG_TAG, QLOG_L2, "valCluster2=%" PRId8, valCluster2);
            }
            // 4Cluster: Marked as not supported commonresourceconfig.xml. So no need to implement for 4 cluster.
        }

        if (d.node_type[idx] == UPDATE_CORES_PER_CLUSTER) {
            snprintf(acqval, NODE_MAX, "%" PRId8 ",%" PRId8, valCluster1, valCluster2);
        } else {
            snprintf(acqval, NODE_MAX, "%" PRId8, valCluster1);
        }
        break;

    case INTERACTIVE_START_INDEX + INTERACTIVE_BOOSTPULSE_DURATION_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_MIN_SAMPLE_TIME_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_RATE_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_TIMER_SLACK_OPCODE:
    case INTERACTIVE_START_INDEX + INTERACTIVE_MAX_FREQ_HYSTERESIS_OPCODE:
    case ONDEMAND_START_INDEX + OND_SAMPLING_RATE_OPCODE: // 0xff - (data & 0xff); //todo
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, TIMER_MULTIPLICATION_FACTOR);
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case GPU_START_INDEX + GPU_MIN_FREQ_OPCODE:
        /* find the nearest >= available freq lvl, after multiplying
        * input value with 1000000 */
        reqval = SecInt::Multiply(reqval, 1000000ul);
        reqval = target.findNextGpuFreq(reqval);
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case GPU_START_INDEX + GPU_BUS_MIN_FREQ_OPCODE:
    case GPU_START_INDEX + GPU_BUS_MAX_FREQ_OPCODE:
        /* find the nearest >= available freq lvl */
        reqval = target.findNextGpuBusFreq(reqval);
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;
    case GPU_START_INDEX + GPU_MIN_FREQ_MHZ_OPCODE:
        reqval = SecInt::Multiply(reqval, 1000000ul);
        reqval = target.findNextGpuFreq(reqval);
        reqval = SecInt::Divide(reqval, 1000000ul);
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case ONDEMAND_START_INDEX + NOTIFY_ON_MIGRATE:
        if (reqval != 1) {
            reqval = 0;
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case MISC_START_INDEX + STORAGE_CLK_SCALING_DISABLE_OPCODE:
        if (reqval == 1) {
            reqval = 0;
        } else {
            reqval = 1;
        }
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;

    case LLCBW_HWMON_START_INDEX + LLCBW_HWMON_MINFREQ_OPCODE:
        if (reqval > 0) {
            reqval = SecInt::Multiply(reqval, 100ul);
            tmp_here = get_devfreq_new_val(reqval, d.llcbw_avail_freqs, d.llcbw_avail_freqs_n, d.llcbw_maxfreq_path);
        }
        if (tmp_here != NULL) {
            strlcpy(acqval, tmp_here, NODE_MAX);
            free(tmp_here);
        } else {
            QLOGE(LOG_TAG, "Unable to find the new devfreq_val for the requested value %" PRIu32, reqval);
            return FAILED;
        }
        break;

    case CORE_HOTPLUG_START_INDEX + CORE_CTL_CPU_NOT_PREFERRED_BIG:
        cluster = r.cluster;
        coresInCluster = tc.getCoresInCluster(cluster);
        if(coresInCluster < 0 || coresInCluster > MAX_CORES) {
            QLOGE(LOG_TAG, "Cores in a cluster cannot be %" PRId8, coresInCluster);
            return FAILED;
        }
        tmpval = reqval;
        for(i = 0; i < coresInCluster; i++) {
            rem = tmpval % 2;
            tmpval = tmpval/2;
            not_pref[coresInCluster - i - 1] = rem;
        }
        for(i = 0; i < coresInCluster; i++) {
            int chars_copied = snprintf(tmp_val + str_index, NODE_MAX - str_index, "%d ", not_pref[i]);
            str_index += chars_copied;
        }
        if(str_index > 0) {
            tmp_val[str_index-1] = '\0';
        }
        strlcpy(acqval, tmp_val, NODE_MAX);
        break;

    default:
        snprintf(acqval, NODE_MAX, "%" PRIu32, reqval);
        break;
    }

    return SUCCESS;
}

/*Based on resource type, we select which all nodes to be updated. Default case
is to update the given single node with value node_strg.*/
int32_t OptsHandler::update_node_param(uint8_t node_type, const char node[NODE_MAX],
                                            char node_strg[NODE_MAX], uint32_t node_strg_l) {
    int32_t rc = -1;
    int8_t i = 0;
    char tmp_node[NODE_MAX] = "", tmp_s[NODE_MAX] = "";
    char *pch = NULL;
    Target &target = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    uint32_t valCluster1 = 0, valCluster2 = 0;

    switch (node_type) {
    case UPDATE_ALL_CORES:
        /* In current implementation we are considering all cores should have same
        value. If this assumption changes, code needs to be updated.*/
        strlcpy(tmp_node, node, NODE_MAX);
        for (i = 0; i < tc.getTotalNumCores(); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, node_strg, node_strg_l);
        }
        break;

    case UPDATE_CORES_PER_CLUSTER:
        /* During acquire call, cores of diffrent cluster are changed with different value.
        and on release all the cores are reset to previously stored value from core 0. */
        valCluster1 = strtol(node_strg, NULL, 0);
        pch = strchr(node_strg, ',');
        if(pch != NULL) {
            valCluster2 = strtol(pch+1, NULL, 0);
        } else {
            valCluster2 = valCluster1;
        }
        // 4Cluster: Used only for SCHED_MOSTLY_IDLE_FREQ_OPCODE which is unsupported. No need to implement for 4 cluster.

        strlcpy(tmp_node, node, NODE_MAX);
        snprintf(tmp_s, NODE_MAX, "%" PRIu32, valCluster1);
        for (i = 0; i <= target.getLastCoreIndex(0); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, tmp_s, strlen(tmp_s));
        }
        snprintf(tmp_s, NODE_MAX, "%" PRIu32, valCluster2);
        for (i = target.getLastCoreIndex(0)+1; i <= target.getLastCoreIndex(1); i++) {
            tmp_node[CPU_INDEX] = i + '0';
            rc = change_node_val(tmp_node, tmp_s, strlen(tmp_s));
        }
        break;

    default:
        rc = change_node_val(node, node_strg, node_strg_l);
        break;
    }
    return rc;
}

/* Wrapper functions for function pointers */
int32_t OptsHandler::dummy(Resource &,  OptsData &) {
    return 0;
}

/* Unsupported resource opcode*/
int32_t OptsHandler::unsupported(Resource &, OptsData &) {
    QLOGE(LOG_TAG, "Error: This resource is not supported");
    return FAILED;
}

/*For each request, we first validate the cores and clusters and then basing on resource type,
we get the node storage pointers for that resource. Also necessary changes are made to node
path basing on core and cluster of request.*/
int32_t OptsHandler::GetNodeStorageLink(Resource &r, OptsData &d, char **node_storage, int32_t **node_storage_length) {
    uint16_t idx = r.qindex;
    int32_t rc = FAILED;
    unsigned char minor = r.minor;
    int8_t cpu = -1;
    int8_t core = r.core;
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();

    switch(d.node_type[idx]) {
    case SELECT_CORE_TO_UPDATE:
    /* we find the node storage paths basing on the requested core.*/
        rc = ValidateClusterAndCore(r.cluster, r.core, ASYNC_CORE, SELECT_CORE_TO_UPDATE);
        if (rc == FAILED) {
            QLOGE(LOG_TAG, "Request on invalid core or cluster");
            return FAILED;
        }

        {
            int8_t VAR_CPU_INDEX = ((string)d.sysfsnode_path[idx]).find_first_of("0123456789");
            d.sysfsnode_path[idx][VAR_CPU_INDEX] = core + '0';
        }

        if (idx == (SCHED_START_INDEX + SCHED_STATIC_CPU_PWR_COST_OPCODE)) {
            if(core < 0 || core >= MAX_CPUS)
                return FAILED;
            *node_storage = d.sch_cpu_pwr_cost_s[core];
            *node_storage_length = &d.sch_cpu_pwr_cost_sl[core];
        }
        else if (idx == (SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE)) {
            if(core < 0 || core >= MAX_CPUS)
                return FAILED;
            *node_storage = d.sch_load_boost_s[core];
            *node_storage_length = &d.sch_load_boost_sl[core];
        } else if (idx == (CORE_HOTPLUG_START_INDEX + CORE_CTL_ENABLE_BIG_OPCODE)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.core_ctl_enable_s[r.cluster];
            *node_storage_length = &d.core_ctl_enable_sl[r.cluster];
        } else if (idx == (CORE_HOTPLUG_START_INDEX + CORE_CTL_CPU_NOT_PREFERRED_BIG)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.core_ctl_cpu_not_preferred_s[r.cluster];
            *node_storage_length = &d.core_ctl_cpu_not_preferred_sl[r.cluster];
        } else if (idx == (CPUFREQ_START_INDEX + WALT_ADAPTIVE_LOW_FREQ_OPCODE)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.adaptive_low_freq_s[r.cluster];
            *node_storage_length = &d.adaptive_low_freq_sl[r.cluster];
        } else if (idx == (CPUFREQ_START_INDEX + WALT_ADAPTIVE_HIGH_FREQ_OPCODE)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.adaptive_high_freq_s[r.cluster];
            *node_storage_length = &d.adaptive_high_freq_sl[r.cluster];
        } else if(idx == (CORE_HOTPLUG_START_INDEX + CORE_CTL_OFFLINE_DELAY_MS_BIG)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.core_ctl_offline_delay_ms_s[r.cluster];
            *node_storage_length = &d.core_ctl_offline_delay_ms_sl[r.cluster];
        } else if(idx == (CORE_HOTPLUG_START_INDEX + CORE_CTL_MIN_PARTIAL_CPUS_BIG)) {
            if (r.cluster < 0 || r.cluster >= MAX_CLUSTER)
                return FAILED;
            *node_storage = d.core_ctl_partial_cpus_s[r.cluster];
            *node_storage_length = &d.core_ctl_partial_cpus_sl[r.cluster];
        }
        break;

    case INTERACTIVE_NODE:
    /*If target's govinstance type is of cluster based, then we get the node storage
      paths of first online cpu in requested cluster and returns failed if all cores in
      that cluster are offline. Default case is to return the cluster 0 node storage paths.*/
        if (CLUSTER_BASED_GOV_INSTANCE == tc.getGovInstanceType()) {
            rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, INTERACTIVE_NODE);
            if (rc == FAILED) {
                QLOGE(LOG_TAG, "Request on invalid core or cluster");
                return FAILED;
            }

            if (CLUSTER0 == r.cluster) {
                if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                    d.sysfsnode_path[idx][CPU_INDEX] = cpu + '0';
                    *node_storage = d.cluster0_interactive_node_storage[minor];
                    *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
                } else {
                    QLOGE(LOG_TAG, "Error! No core is online for cluster %" PRId8, r.cluster);
                    return FAILED;
                }
            } else {
                if ((cpu = get_online_core(t.getFirstCoreIndex(r.cluster), t.getLastCoreIndex(r.cluster))) != FAILED) {
                    d.sysfsnode_path[idx][CPU_INDEX] = cpu + '0';
                    if (r.cluster == 1) {
                      *node_storage = d.cluster1_interactive_node_storage[minor];
                      *node_storage_length = &d.cluster1_interactive_node_storage_length[minor];
                    } else if (r.cluster == 2) {
                      *node_storage = d.cluster2_interactive_node_storage[minor];
                      *node_storage_length = &d.cluster2_interactive_node_storage_length[minor];
                    } else if (r.cluster == 3) {
                      *node_storage = d.cluster3_interactive_node_storage[minor];
                      *node_storage_length = &d.cluster3_interactive_node_storage_length[minor];
                    }
                } else {
                    QLOGE(LOG_TAG, "Error! No core is online for cluster %" PRId8, r.cluster);
                    return FAILED;
                }
            }
        } else {
            rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, INTERACTIVE_NODE);
            if (rc == FAILED) {
                QLOGE(LOG_TAG, "Request on invalid core or cluster");
                return FAILED;
            }
            *node_storage = d.cluster0_interactive_node_storage[minor];
            *node_storage_length = &d.cluster0_interactive_node_storage_length[minor];
        }
        break;

        case MEM_LAT_NODE:

        if (CLUSTER_BASED_GOV_INSTANCE == tc.getGovInstanceType()) {
            rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, MEM_LAT_NODE);
            if (rc == FAILED) {
                QLOGE(LOG_TAG, "Request on invalid core or cluster of mem_lat");
                return FAILED;
            }

            int8_t MEM_CPU_INDEX = ((string)d.sysfsnode_path[idx]).find_first_of("0123456789");

            if (CLUSTER0 == r.cluster) {
               d.sysfsnode_path[idx][MEM_CPU_INDEX] = '0';
               *node_storage = d.memlat_minfreq_node_strg[r.cluster][minor];
               *node_storage_length = &d.memlat_minfreq_node_strg_len[r.cluster][minor];
            } else {
                cpu = t.getLastCoreIndex(r.cluster-1)+1;
                d.sysfsnode_path[idx][MEM_CPU_INDEX] = cpu + '0';
                *node_storage = d.memlat_minfreq_node_strg[r.cluster][minor];
                *node_storage_length = &d.memlat_minfreq_node_strg_len[r.cluster][minor];
            }
        } else {
               rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, MEM_LAT_NODE);
               if (rc == FAILED) {
                   QLOGE(LOG_TAG, "Request on invalid core or cluster");
                   return FAILED;
                }
                *node_storage = d.memlat_minfreq_node_strg[CLUSTER0][minor];
                *node_storage_length = &d.memlat_minfreq_node_strg_len[CLUSTER0][minor];
        }
    break;

    default:
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, SINGLE_NODE);
        if (rc == FAILED) {
            QLOGE(LOG_TAG, "Request on invalid core or cluster");
            return FAILED;
        }

        *node_storage = d.sysfsnode_storage[idx];
        *node_storage_length = &d.sysfsnode_storage_length[idx];
        break;
    }

    return SUCCESS;
}

/*An optional function, which is called after every acquire/release call for
handling all requests post node updations.*/
void OptsHandler::CustomizePostNodeUpdation(Resource &r, OptsData &d, uint8_t perflock_call,
                                            char node_strg[NODE_MAX], uint32_t node_strg_l) {
    uint16_t idx = r.qindex, rc;
    if (perflock_call == ACQUIRE_LOCK) {
        switch(idx) {
        case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
            break;
        }
    } else if (perflock_call == RELEASE_LOCK) {
        switch(idx) {
        case SCHED_START_INDEX + SCHED_CPUSET_FOREGROUND_OPCODE:
            /* Only for foreground cpuset: restore foreground/boost/cpus
            after restoring foreground/cpus with all cpus bitmask */
            FWRITE_STR(SCHED_FOREGROUND_BOOST, node_strg, node_strg_l, rc);
            QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %s, return value %" PRIu16, SCHED_FOREGROUND_BOOST, node_strg, rc);
            break;

        case SCHED_START_INDEX + SCHED_SET_FREQ_AGGR_OPCODE:
            break;
        }
    }
}

/*A common function to handle all the generic perflock calls.*/
int32_t OptsHandler::modify_sysfsnode(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    int32_t rc = FAILED;
    char tmp_s[NODE_MAX]= "";
    uint32_t reqval = r.value;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    /*The first step for any request is to validate it and then get the Node storage paths.
    For acqiure call these node storage paths are used to store the current value of nodes
    and for release call the nodes are updated with values in these storage paths.*/
    rc = GetNodeStorageLink(r, d, &node_storage, &node_storage_length);

    /*Release call, release happens only if we have any previously stored value.
      For only interactive type nodes, on release failure we wait on poll_thread.*/
    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from function = %s",
                                                        idx, d.sysfsnode_path[idx], __func__);

        if (rc != FAILED && *node_storage_length > 0) {
            rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], node_storage, *node_storage_length);
            CustomizePostNodeUpdation(r, d, RELEASE_LOCK, node_storage, *node_storage_length);
            *node_storage_length = -1;
        } else if (rc == FAILED) {
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for resource index=%" PRIu16 ", node path=%s",
                                                        idx, d.sysfsnode_path[idx]);
        } else {
            //Double release can happen in case of display off, but its not an issue
            QLOGL(LOG_TAG, QLOG_WARNING, "perf_lock_rel: failed for %s as previous value is not stored", d.sysfsnode_path[idx]);
        }

        if(d.node_type[idx] == INTERACTIVE_NODE) {
            if (rc < 0 && CLUSTER_BASED_GOV_INSTANCE == tc.getGovInstanceType()) {
                signal_chk_poll_thread(d.sysfsnode_path[idx], rc);
            }
        }
        return rc;
    }

    /*steps followed for Acquire call
      1. Ensuring that we got the node storage pointers correctly.
      2. Storing the previous node value, only if it is not already stored.
      3. Customizing the requested value.
      4. Updating all the required nodes basing on the opcode type.*/

    QLOGL(LOG_TAG, QLOG_L2, "Perflock Acquire call for resource index = %" PRIu16 ", path = %s, from function = %s",
                                                        idx, d.sysfsnode_path[idx], __func__);
    QLOGL(LOG_TAG, QLOG_L2, "Requested value = %" PRIu32, reqval);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for resource index=%" PRIu16 ", node path=%s",
                                                        idx, d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (*node_storage_length <= 0) {
        *node_storage_length = save_node_val(d.sysfsnode_path[idx], node_storage);
        if (*node_storage_length <= 0) {
            QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
            return FAILED;
        }
    }

    rc = CustomizeRequestValue(r, d, tmp_s);
    if (rc == SUCCESS) {
        QLOGL(LOG_TAG, QLOG_L2, "After customizing, the request reqval=%s", tmp_s);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
        CustomizePostNodeUpdation(r, d, ACQUIRE_LOCK, tmp_s, strlen(tmp_s));
        return rc;
    } else {
        QLOGE(LOG_TAG, "Error! Perflock failed, invalid request value %" PRIu32, reqval);
        return FAILED;
    }
}

int32_t OptsHandler::pmQoS_cpu_dma_latency(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    static char pmqos_cpu_dma_s[NODE_MAX];// pm qos cpu dma latency string
    uint32_t latency = r.value;
    static int32_t fd = FAILED;
    uint32_t min_pmqos_latency = 0;
    Target &t = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "%s:Request on invalid core or cluster", __func__);
        return FAILED;
    }
    /*Set minimum latency(floor) to WFI for the target.
     * Clients thus do not need to explicitly mention the WFI latency
     * & continue to use the existing value of 0x1 to disable PC and use WFI.
     * If a client gives an explicit value > WFI, then
     * client knows what it is doing and hence honor.
     */
     min_pmqos_latency = t.getMinPmQosLatency();
     if (latency < min_pmqos_latency) {
         QLOGL(LOG_TAG, QLOG_L2, "%s:Requested Latency=0x%" PRIx32, __func__, latency);
         QLOGL(LOG_TAG, QLOG_L2, "%s:Target Min WFI Latency is =0x%" PRIx32 ", setting same", __func__,min_pmqos_latency);
         latency = min_pmqos_latency;
     } else {
         QLOGL(LOG_TAG, QLOG_L2, "%s:Client latency=0x%" PRIx32 " > trgt min latency=0x%" PRIx32, __func__, latency,min_pmqos_latency);
         QLOGL(LOG_TAG, QLOG_L2, "No Override with min latency for target");
     }
    /*If there is a lock acquired, then we close, to release
     *that fd and open with new value.Doing this way
     * in this case actually handles concurrency.
     */
    if (fd >= 0) { //close the device node that was opened.
        rc = close(fd);
        fd = FAILED;
        QLOGL(LOG_TAG, QLOG_L2, "%s:Released the PMQos Lock and dev node closed, rc=%" PRId32, __func__, rc);
    }

    /*Check if Perf lock request to acquire(!= MAX_LVL)*/
    if (latency != MAX_LVL) {
        //FWRITE_STR not used since it closes the fd. This shouldnt be done in this case
        ///kernel/msm-4.4/Documentation/power/pm_qos_interface.txt#95
        fd = open(d.sysfsnode_path[idx], O_WRONLY);
        if (fd >= 0) {
           snprintf(pmqos_cpu_dma_s, NODE_MAX, "%" PRIx32, latency);
           rc = write(fd, pmqos_cpu_dma_s, strlen(pmqos_cpu_dma_s));// Write the CPU DMA Latency value
           if (rc < 0) {
               QLOGE(LOG_TAG, "%s:Writing to the PM QoS node failed=%" PRId32, __func__, rc);
               close(fd);
               fd = FAILED;
           } else {
               //To remove the user mode request for a target value simply close the device node
               QLOGL(LOG_TAG, QLOG_L2, "%s:Sucessfully applied PMQos Lock for fd=%" PRId32 ", latency=0x%s, rc=%" PRId32, __func__, fd, pmqos_cpu_dma_s, rc);
           }
       } else {
           rc = FAILED;
           QLOGE(LOG_TAG, "%s:Failed to Open PMQos Lock for fd=%" PRId32 ", latency=0x%s, rc=%" PRId32, __func__, fd, pmqos_cpu_dma_s, rc);
       }
    }
    return rc;
}

/* CPU options */
int32_t OptsHandler::cpu_options(Resource &r,  OptsData &d) {
    int32_t rc = -1;
    uint32_t i = 0;
    uint16_t idx = r.qindex;
    char fmtStr[NODE_MAX];
    uint32_t reqval  = r.value;
    uint32_t valToUpdate;
    char nodeToUpdate[NODE_MAX];
    uint8_t min_freq = (CPUFREQ_MIN_FREQ_OPCODE == r.minor) ? 1 : 0;
    int8_t cpu = -1;
    static bool is_sync_cores; //Updating a single core freq, updates all cores freq.
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    int8_t startCpu, endCpu;
    int8_t cluster, coresInCluster;

    is_sync_cores = tc.isSyncCore();
    cpu = r.core;
    if (r.cluster >= tc.getNumCluster() || r.cluster < 0) {
        QLOGE(LOG_TAG, "Invalid Cluster id=%" PRId8, r.cluster);
        return FAILED;
    }
    /* For sync_cores, where changing frequency for one core changes, ignore the requests
     * for resources other than for core0 and core4. This is to ensure that we may not end
     * up loosing concurrency support for cpufreq perflocks.
     */

    cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
    if ((startCpu < 0) || (endCpu < 0) || (cluster < 0)) {
        QLOGE(LOG_TAG, "Could not find a cluster corresponding the core %" PRId8, cpu);
        return FAILED;
    }
    if (is_sync_cores && (cpu > startCpu && cpu <= endCpu)) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Warning: Resource [%" PRIu16 ", %" PRIu16 "] not supported for core %" PRId8 ". Instead use resource for core %" PRId8, r.major, r.minor,cpu,startCpu);
        return FAILED;
    }
    /* Calculate the value that needs to be updated to KPM.
     * If lock is being released (reqVal == 0) then
     * update with 0 (for min_freq) and cpu max reset value (for max_freq)
     * If lock is being acquired then reqVal is multiplied
     * with 100000 and updated by finding next closest frequency.
     * */
    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Releasing the node %s", d.sysfsnode_path[idx]);
        valToUpdate = 0;
        strlcpy(nodeToUpdate, d.sysfsnode_path[idx], strlen(d.sysfsnode_path[idx])+1);
        if (!min_freq){
            valToUpdate = tc.getCpuMaxFreqResetVal(cluster);
        }
    } else {
        if (reqval == 0xFFFF) {
            reqval = tc.getCpuCappedMaxFreqVal(cluster);
            QLOGL(LOG_TAG, QLOG_L2, "reqval is 0xFFFF, new value %" PRIu32, reqval);
        }
        reqval = SecInt::Multiply(reqval, FREQ_MULTIPLICATION_FACTOR);
        valToUpdate = d.find_next_cpu_frequency(r.core, reqval);
        strlcpy(nodeToUpdate, d.sysfsnode_path[idx], strlen(d.sysfsnode_path[idx])+1);
    }
    QLOGL(LOG_TAG, QLOG_L2, "Freq value to be updated %" PRIu32, valToUpdate);

    /* Construct the formatted string with which KPM node is updated
     */
    coresInCluster = tc.getCoresInCluster(cluster);
    if (coresInCluster < 0){
        QLOGE(LOG_TAG, "Cores in a cluster cannot be %" PRId8, coresInCluster);
        return FAILED;
    }

    if (cluster == 0) {
        rc = update_freq_node(0, t.getLastCoreIndex(0), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE(LOG_TAG, "Perflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else if (cluster < MAX_CLUSTER) {
        rc = update_freq_node(t.getLastCoreIndex(cluster - 1)+1, t.getLastCoreIndex(cluster), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE(LOG_TAG, "Peflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else {
        QLOGE(LOG_TAG, "Cluster Id %" PRId8 " not supported\n",cluster);
        return FAILED;
    }

    /* Finally update the KPM Node. The cluster based node is updated in
     * fmtStr, and KPM node is available in nodeToUpdate.
     * */
     FWRITE_STR(nodeToUpdate, fmtStr, strlen(fmtStr), rc);
     QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %s return value %" PRId32, nodeToUpdate , fmtStr, rc );
     return rc;
}

/* scaling min freq */
int32_t OptsHandler::set_scaling_min_freq(Resource &r, OptsData &d) {
    int32_t rc = -1;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;
    uint32_t valToUpdate = 0;
    char tmp_s[NODE_MAX] = "";
    uint32_t postboot_freq = 0;

    rc = GetNodeStorageLink(r, d, &node_storage, &node_storage_length);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Unable to find node storage for idx=%" PRIu16 ", node path=%s",
                idx, d.sysfsnode_path[idx]);
        return rc;
    }

    auto it = d.postboot_scaling_minfreq.find(r.core);
    if (it != d.postboot_scaling_minfreq.end()) {
        postboot_freq = it->second;
    }
    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Releasing the node %s", d.sysfsnode_path[idx]);
        valToUpdate = d.find_next_cpu_frequency(r.core, postboot_freq);
    } else {
        reqval = SecInt::Multiply(reqval, FREQ_MULTIPLICATION_FACTOR);
        valToUpdate = d.find_next_cpu_frequency(r.core, reqval);
        //Accept the request only if the freq is below post boot freq
        if (valToUpdate >= postboot_freq) {
            QLOGE(LOG_TAG, "Ignoring scaling_min_freq request");
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%" PRId32, valToUpdate);
    FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %" PRIu32 "return value %" PRId32,
                    d.sysfsnode_path[idx], valToUpdate, rc);
    return rc;
}

/* Set sched boost */
int32_t OptsHandler::set_sched_boost(Resource &r, OptsData &d) {
    int32_t rc = -1;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX];
    uint32_t reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    snprintf(tmp_s, NODE_MAX, "%" PRIu32, reqval);
    FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.sysfsnode_path[idx], tmp_s, rc);
    return rc;
}

/* Reset sched boost */
int32_t OptsHandler::reset_sched_boost(Resource &r, OptsData &d) {
    char tmp_s[NODE_MAX];
    int32_t reqval = r.value;
    uint16_t idx = r.qindex;
    int32_t rc = -1;
    uint32_t kernelMajor = 0;
    uint32_t kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();

    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    if ((kernelMajor <= 4 && kernelMinor <= 9)) {
        reqval = 0;
    } else {
        reqval = -reqval;
    }

    snprintf(tmp_s, NODE_MAX, "%" PRId32, reqval);
    FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %d", d.sysfsnode_path[idx], tmp_s, rc);
    return rc;
}



char *OptsHandler::cpuset_bitmask_to_str(uint32_t bitmask) {
    uint32_t i = 0;
    uint32_t str_index = 0;
    char *cpuset_str = new(std::nothrow) char[CPUSET_STR_SIZE];
    if(cpuset_str != NULL) {
        memset(cpuset_str, '\0', CPUSET_STR_SIZE * sizeof(char));
        while(bitmask != 0) {
            if(bitmask & 1 && str_index < CPUSET_STR_SIZE - 1) {
                int chars_copied = snprintf(cpuset_str + str_index, CPUSET_STR_SIZE - str_index, "%" PRIu32 ",", i);
                str_index += chars_copied;
            }
            bitmask = bitmask >> 1;
            i++;
        }
        if(str_index > 0) {
            cpuset_str[str_index-1] = '\0';
        }
    }
    return cpuset_str;
}


/* Add apply_value */
int32_t OptsHandler::apply_value(Resource &r, OptsData &) {
    int32_t rc = -1;
    QLOGV(LOG_TAG, "%s, %d, %d, 0x%x: mApplyValue\n", __FUNCTION__, r.value, r.core, r.qindex);
    if (mApplyValue != NULL)
        rc = mApplyValue(r.value, r.core, r.qindex, false);
    else
        QLOGE(LOG_TAG, "%s: No mApplyValue\n", __FUNCTION__);

    return rc;
}

/* reset_value */
int32_t OptsHandler::reset_value(Resource &r, OptsData &) {
    int32_t rc = -1;
    QLOGV(LOG_TAG, "%s, %d, %d, 0x%x: mApplyValue\n", __FUNCTION__, r.value, r.core, r.qindex);
    if (mApplyValue != NULL)
        rc = mApplyValue(r.value, r.core, r.qindex, true);
    else
        QLOGE(LOG_TAG, "%s: No mApplyValue\n", __FUNCTION__);

    return rc;
}

int32_t OptsHandler::l3_min_freq(Resource &r,  OptsData &d) {
    int32_t rc = FAILED;
    char* setval_Cluster[MAX_CLUSTER] = {NULL};
    uint32_t reqval = r.value;
    static uint32_t stored_l3_min_freq = 0;
    int8_t count = 0;
    uint16_t qIdx = r.qindex;
    Target &t = Target::getCurTarget();
    TargetConfig &tc =  TargetConfig::getTargetConfig();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if(rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    if(reqval == MAX_LVL) {

        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            if(d.l3_minf_sl[j] > 0) {
                FWRITE_STR(d.l3_minfreq_path[j], d.l3_minf_s[j], d.l3_minf_sl[j], rc);
                QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel_l3Cluster%" PRIu8 ": updated %s with %s return value %" PRId32,j, d.l3_minfreq_path[j], d.l3_minf_s[j], rc);
            }
        }
        stored_l3_min_freq = 0;
        return rc;
    }

    if(reqval > 0) {
        reqval = SecInt::Multiply(reqval, 100000ul);
        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            setval_Cluster[j] = get_devfreq_new_val(reqval, d.l3_avail_freqs[j], d.l3_avail_freqs_n[j], d.l3_maxfreq_path[j]);
            if(!stored_l3_min_freq) {
                d.l3_minf_sl[j] = save_node_val(d.l3_minfreq_path[j], d.l3_minf_s[j]);
                if(d.l3_minf_sl[j] > 0)
                    count++;
            }
            if(setval_Cluster[j] != NULL) {
                FWRITE_STR(d.l3_minfreq_path[j], setval_Cluster[j], strlen(setval_Cluster[j]), rc);
                QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq_l3Cluster%" PRIu8 ": updated %s with %s return value %" PRId32,j, d.l3_minfreq_path[j], setval_Cluster[j], rc);
                free(setval_Cluster[j]);
            }
        }
    }


    if(count == tc.getNumCluster())
        stored_l3_min_freq = 1;

    return rc;
}

int32_t OptsHandler::l3_max_freq(Resource &r,  OptsData &d) {
    int32_t rc = FAILED;
    char* setval_Cluster[MAX_CLUSTER] = {NULL};
    uint32_t reqval = r.value;
    static uint32_t stored_l3_max_freq = 0;
    int8_t count = 0;
    uint16_t qIdx = r.qindex;
    Target &t = Target::getCurTarget();
    TargetConfig &tc =  TargetConfig::getTargetConfig();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if(rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    if(reqval == MAX_LVL) {

        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            if(d.l3_maxf_sl[j] > 0) {
                FWRITE_STR(d.l3_maxfreq_path[j], d.l3_maxf_s[j], d.l3_maxf_sl[j], rc);
                QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel_l3Cluster%" PRIu8 ": updated %s with %s return value %" PRId32,j, d.l3_maxfreq_path[j], d.l3_maxf_s[j], rc);
            }
        }
        stored_l3_max_freq = 0;
        return rc;
    }

    if(reqval > 0) {
        reqval = SecInt::Multiply(reqval, 100000ul);
        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            setval_Cluster[j] = get_devfreq_new_val(reqval, d.l3_avail_freqs[j], d.l3_avail_freqs_n[j], d.l3_maxfreq_path[j]);
            if(!stored_l3_max_freq) {
                d.l3_maxf_sl[j] = save_node_val(d.l3_maxfreq_path[j], d.l3_maxf_s[j]);
                if(d.l3_maxf_sl[j] > 0)
                    count++;
            }
            if(setval_Cluster[j] != NULL) {
                FWRITE_STR(d.l3_maxfreq_path[j], setval_Cluster[j], strlen(setval_Cluster[j]), rc);
                QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq_l3Cluster%" PRIu8 ": updated %s with %s return value %" PRId32,j, d.l3_maxfreq_path[j], setval_Cluster[j], rc);
                free(setval_Cluster[j]);
            }
        }
    }


    if(count == tc.getNumCluster())
        stored_l3_max_freq = 1;

    return rc;
}


int32_t OptsHandler::npubw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int32_t rc = 0;
    char tmp_s[NODE_MAX];
    static uint32_t stored_hyst_opt = 0;
    uint32_t reqval = r.value;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.npubw_hc_sl > 0) {
           FWRITE_STR(d.npubw_hwmon_hyst_count_path, d.npubw_hc_s, d.npubw_hc_sl, rc);
           if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.npubw_hwmon_hyst_count_path);
               return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npubw_hwmon_hyst_count_path, d.npubw_hc_s, rc);
        }
        if (d.npubw_hm_sl > 0) {
            FWRITE_STR(d.npubw_hwmon_hist_memory_path, d.npubw_hm_s, d.npubw_hm_sl, rc);
            if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.npubw_hwmon_hist_memory_path);
               return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npubw_hwmon_hist_memory_path, d.npubw_hm_s, rc);
        }
        if (d.npubw_hl_sl > 0) {
            FWRITE_STR(d.npubw_hwmon_hyst_length_path, d.npubw_hl_s, d.npubw_hl_sl, rc);
            if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.npubw_hwmon_hyst_length_path);
               return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npubw_hwmon_hyst_length_path, d.npubw_hl_s, rc);
        }
        stored_hyst_opt = 0;

        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.npubw_hwmon_hyst_count_path, d.npubw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npubw_hc_sl = strlen(d.npubw_hc_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npubw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.npubw_hwmon_hist_memory_path, d.npubw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npubw_hm_sl = strlen(d.npubw_hm_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npubw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.npubw_hwmon_hyst_length_path, d.npubw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npubw_hl_sl = strlen(d.npubw_hl_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npubw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.npubw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npubw_hwmon_hyst_count_path, tmp_s, rc);
    FWRITE_STR(d.npubw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npubw_hwmon_hist_memory_path, tmp_s, rc);
    FWRITE_STR(d.npubw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npubw_hwmon_hyst_length_path, tmp_s, rc);
    return rc;
}

int32_t OptsHandler::npu_llcbw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int32_t rc = 0;
    char tmp_s[NODE_MAX];
    static uint32_t stored_hyst_opt = 0;
    uint32_t reqval = r.value;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.npu_llcbw_hc_sl > 0) {
            FWRITE_STR(d.npu_llcbw_hwmon_hyst_count_path, d.npu_llcbw_hc_s, d.npu_llcbw_hc_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.npu_llcbw_hwmon_hyst_count_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hyst_count_path, d.npu_llcbw_hc_s, rc);
        }
        if (d.npu_llcbw_hm_sl > 0) {
            FWRITE_STR(d.npu_llcbw_hwmon_hist_memory_path, d.npu_llcbw_hm_s, d.npu_llcbw_hm_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.npu_llcbw_hwmon_hist_memory_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hist_memory_path, d.npu_llcbw_hm_s, rc);
        }
        if (d.npu_llcbw_hl_sl > 0) {
            FWRITE_STR(d.npu_llcbw_hwmon_hyst_length_path, d.npu_llcbw_hl_s, d.npu_llcbw_hl_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.npu_llcbw_hwmon_hyst_length_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hyst_length_path, d.npu_llcbw_hl_s, rc);
        }
        stored_hyst_opt = 0;

        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.npu_llcbw_hwmon_hyst_count_path, d.npu_llcbw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npu_llcbw_hc_sl = strlen(d.npu_llcbw_hc_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npu_llcbw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.npu_llcbw_hwmon_hist_memory_path, d.npu_llcbw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npu_llcbw_hm_sl = strlen(d.npu_llcbw_hm_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npu_llcbw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.npu_llcbw_hwmon_hyst_length_path, d.npu_llcbw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.npu_llcbw_hl_sl = strlen(d.npu_llcbw_hl_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.npu_llcbw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.npu_llcbw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hyst_count_path, tmp_s, rc);
    FWRITE_STR(d.npu_llcbw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hist_memory_path, tmp_s, rc);
    FWRITE_STR(d.npu_llcbw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.npu_llcbw_hwmon_hyst_length_path, tmp_s, rc);
    return rc;
}

int32_t OptsHandler::bus_dcvs_hyst_opt(Resource &r, OptsData &d) {
    int32_t rc = 0;
    char tmp_s[NODE_MAX];
    static uint32_t stored_hyst_opt = 0;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;
    char hyst_len[NODE_MAX];
    char hist_mem[NODE_MAX];
    char hyst_trigger_count[NODE_MAX];
    static char hyst_len_s[NODE_MAX];
    static char hist_mem_s[NODE_MAX];
    static char hyst_trigger_count_s[NODE_MAX];
    static uint32_t hls = 0;
    static uint32_t hms = 0;
    static uint32_t htcs = 0;
    int32_t ret[3] = {-1,-1,-1};

    if (!d.is_supported[idx]) {
       QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
       return FAILED;
    }

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    snprintf(hyst_len, NODE_MAX, "%s/hyst_length", d.sysfsnode_path[idx]);
    snprintf(hist_mem, NODE_MAX, "%s/hist_memory", d.sysfsnode_path[idx]);
    snprintf(hyst_trigger_count, NODE_MAX, "%s/hyst_trigger_count", d.sysfsnode_path[idx]);

    if (reqval == MAX_LVL) {
        if (hls > 0 && hms > 0 && htcs > 0) {
            FWRITE_STR(hyst_len, hyst_len_s, hls, ret[0]);
            FWRITE_STR(hist_mem, hist_mem_s, hms, ret[1]);
            FWRITE_STR(hyst_trigger_count, hyst_trigger_count_s, htcs, ret[2]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, hyst_len, hyst_len_s, ret[0]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, hist_mem, hist_mem_s, ret[1]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32,
                    hyst_trigger_count, hyst_trigger_count_s, ret[2]);
            stored_hyst_opt = 0;
        }
        rc = ret[0] && ret[1] && ret[2];
        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(hyst_len, hyst_len_s, NODE_MAX, ret[0]);
        FREAD_STR(hist_mem, hist_mem_s, NODE_MAX, ret[1]);
        FREAD_STR(hyst_trigger_count, hyst_trigger_count_s, NODE_MAX, ret[2]);
        if (ret[0] > 0 && ret[1] > 0 && ret[2] > 0) {
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, hyst_len, hyst_len_s, ret[0]);
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, hist_mem, hist_mem_s, ret[1]);
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, hyst_trigger_count,
                    hyst_trigger_count_s, ret[2]);
            hls = strlen(hyst_len_s);
            hms = strlen(hist_mem_s);
            htcs = strlen(hyst_trigger_count_s);
            stored_hyst_opt = 1;
        } else {
            if (ret[0] < 0)
               QLOGE(LOG_TAG, "Failed to read %s", hyst_len);
            if (ret[1] < 0)
               QLOGE(LOG_TAG, "Failed to read %s", hist_mem);
            if (ret[2] < 0)
               QLOGE(LOG_TAG, "Failed to read %s", hyst_trigger_count);
            return FAILED;
        }
    }
    snprintf(tmp_s, NODE_MAX, "%" PRIu32, reqval);
    FWRITE_STR(hyst_len, tmp_s, strlen(tmp_s), ret[0]);
    FWRITE_STR(hist_mem, tmp_s, strlen(tmp_s), ret[1]);
    FWRITE_STR(hyst_trigger_count, tmp_s, strlen(tmp_s), ret[2]);

    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, hyst_len, tmp_s, ret[0]);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, hist_mem, tmp_s, ret[1]);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32,
            hyst_trigger_count, tmp_s, ret[2]);

    if (ret[0] < 0 || ret[1] < 0 || ret[2] < 0) {
        QLOGE(LOG_TAG, "Acquiring cpubw_hwmon_hyst_opt failed\n");
        return FAILED;
    }
    rc = ret[0] && ret[1] && ret[2];
    return rc;
}

int32_t OptsHandler::cpubw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int32_t rc = 0;
    char tmp_s[NODE_MAX];
    static uint32_t stored_hyst_opt = 0;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_hc_sl > 0) {
           FWRITE_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, d.cpubw_hc_sl, rc);
           if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.cpubw_hwmon_hyst_count_path);
               return FAILED;
            }
        }
        if (d.cpubw_hm_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, d.cpubw_hm_sl, rc);
            if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.cpubw_hwmon_hist_memory_path);
               return FAILED;
            }
        }
        if (d.cpubw_hl_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, d.cpubw_hl_sl, rc);
            if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to write %s", d.cpubw_hwmon_hyst_length_path);
               return FAILED;
            }
        }
        stored_hyst_opt = 0;

        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hc_sl = strlen(d.cpubw_hc_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.cpubw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hm_sl = strlen(d.cpubw_hm_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.cpubw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hl_sl = strlen(d.cpubw_hl_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.cpubw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.cpubw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.cpubw_hwmon_hyst_count_path, tmp_s, rc);
    FWRITE_STR(d.cpubw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.cpubw_hwmon_hist_memory_path, tmp_s, rc);
    FWRITE_STR(d.cpubw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.cpubw_hwmon_hyst_length_path, tmp_s, rc);
    return rc;
}

int32_t OptsHandler::llcbw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int32_t rc = 0;
    char tmp_s[NODE_MAX];
    static uint32_t stored_hyst_opt = 0;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.llcbw_hc_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, d.llcbw_hc_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.llcbw_hwmon_hyst_count_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, rc);
        }
        if (d.llcbw_hm_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, d.llcbw_hm_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.llcbw_hwmon_hist_memory_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, rc);
        }
        if (d.llcbw_hl_sl > 0) {
            FWRITE_STR(d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, d.llcbw_hl_sl, rc);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to write %s", d.llcbw_hwmon_hyst_length_path);
                return FAILED;
            }
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, rc);
        }
        stored_hyst_opt = 0;

        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.llcbw_hwmon_hyst_count_path, d.llcbw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hc_sl = strlen(d.llcbw_hc_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.llcbw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.llcbw_hwmon_hist_memory_path, d.llcbw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hm_sl = strlen(d.llcbw_hm_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.llcbw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.llcbw_hwmon_hyst_length_path, d.llcbw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.llcbw_hl_sl = strlen(d.llcbw_hl_s);
        } else {
            QLOGE(LOG_TAG, "Failed to read %s", d.llcbw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.llcbw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hyst_count_path, tmp_s, rc);
    FWRITE_STR(d.llcbw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hist_memory_path, tmp_s, rc);
    FWRITE_STR(d.llcbw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, d.llcbw_hwmon_hyst_length_path, tmp_s, rc);
    return rc;
}

/* Function to handle single layer display hint
 * state = 0, is not single layer
 * state = 1, is single layer
 */
int32_t OptsHandler::handle_disp_hint(Resource &r, OptsData &d) {
    uint32_t reqval = r.value;
    int32_t rc = FAILED;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.disp_single_layer = reqval;
    QLOGL(LOG_TAG, QLOG_L2, "Display sent layer=%d", d.mHintData.disp_single_layer);

    /* Video is started, but display sends multiple
     * layer hint, so rearm timer, handled as
     * condition2 in slvp callback
     */
    if (d.mHintData.slvp_perflock_set == 1 && d.mHintData.disp_single_layer != 1) {
        QLOGL(LOG_TAG, QLOG_L2, "Display to rearm timer,layer=%d",d.mHintData.disp_single_layer);

        /* rearm timer here, release handle in SLVP callback */
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to receive video playback hint
 * state = 0, video stopped
 * state = 1, single instance of video
 */
int32_t OptsHandler::handle_vid_decplay_hint(Resource &r, OptsData &d) {
    uint32_t reqval = r.value;
    int32_t rc = FAILED;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.vid_hint_state = reqval;

    /* timer is created only once here on getting video hint */
    if (d.mHintData.vid_hint_state == 1 && !d.mHintData.timer_created) {
        QLOGL(LOG_TAG, QLOG_L2, "Video sent hint, create timer");
        return vid_create_timer(&d.mHintData);
    } else {
        /* only rearm here, handle conditions in SLVP callback */
        QLOGL(LOG_TAG, QLOG_L2, "Video rearm timer");
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to recieve video encode hint
 * for WFD use case.
 */
int32_t OptsHandler::handle_vid_encplay_hint(Resource &r, OptsData &d) {
    uint32_t reqval = r.value;
    int32_t rc = FAILED;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    /* reqval - 1, encode start
     * reqval - 0, encode stop
     */
    d.mHintData.vid_enc_start = reqval;
    /* rearm timer if encode is atarted
     * handle condition4 in callback */
    if (d.mHintData.slvp_perflock_set == 1) {
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

int32_t OptsHandler::disable_ksm(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t qIdx = r.qindex;
    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(0);
}

int32_t OptsHandler::enable_ksm(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(1);
}

int32_t OptsHandler::set_ksm_param(Resource &r, OptsData &d) {
    int32_t rc = 0;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       char sleep_time[PROPERTY_VALUE_MAX];
       char scan_page[PROPERTY_VALUE_MAX];
       memset(sleep_time, 0, sizeof(sleep_time));
       memset(scan_page, 0, sizeof(scan_page));

       strlcpy(sleep_time, "20", PROPERTY_VALUE_MAX);
       strlcpy(scan_page, "300", PROPERTY_VALUE_MAX);

       FWRITE_STR(d.ksm_param_sleeptime, sleep_time, strlen(sleep_time), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, scan_page, strlen(scan_page), rc);
    }
    return rc;
}

int32_t OptsHandler::reset_ksm_param(Resource &r, OptsData &d) {
    int32_t rc = 0;
    uint16_t qIdx = r.qindex;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       FWRITE_STR(d.ksm_param_sleeptime, d.ksm_sleep_millisecs, strlen(d.ksm_sleep_millisecs), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, d.ksm_pages_to_scan, strlen(d.ksm_pages_to_scan), rc);
    }
    return rc;
}

int32_t OptsHandler::lock_min_cores(Resource &r,  OptsData &d) {
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (d.core_ctl_present && t.isResourceSupported(r.major, r.minor)) {
        int32_t rc = -1;
        int8_t coresInCluster, cluster = -1;
        char tmp_s[NODE_MAX];
        uint32_t reqval = r.value;
        static uint32_t stored_val[MAX_CLUSTER] = {0};

        cluster = r.cluster;
        snprintf(d.core_ctl_min_cpu_node, NODE_MAX, CORE_CTL_MIN_CPU, t.getFirstCoreIndex(cluster));

        if (reqval == MAX_LVL) {
            d.min_cores[cluster] = atoi(d.core_ctl_min_s[cluster]);
            FWRITE_STR(d.core_ctl_min_cpu_node, d.core_ctl_min_s[cluster], d.core_ctl_min_sl[cluster], rc);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updating %s with %s", d.core_ctl_min_cpu_node, d.core_ctl_min_s[cluster]);
            stored_val[cluster] = 0;
            return rc;
        }

        if (!stored_val[cluster]) {
            FREAD_STR(d.core_ctl_min_cpu_node, d.core_ctl_min_s[cluster], NODE_MAX, rc);
            if (rc >= 0) {
                d.core_ctl_min_sl[cluster] = strlen(d.core_ctl_min_s[cluster]);
                QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, d.core_ctl_min_cpu_node, d.core_ctl_min_s[cluster], rc);
            } else {
                QLOGE(LOG_TAG, "Failed to read %s", d.core_ctl_min_cpu_node);
                return FAILED;
            }
            stored_val[cluster] = 1;
        }

        d.min_cores[cluster] = reqval;
        coresInCluster = tc.getCoresInCluster(cluster);
        if ((coresInCluster >= 0) && (d.min_cores[cluster] > coresInCluster)) {
            d.min_cores[cluster] = coresInCluster;
        }

        if (d.min_cores[cluster] > d.max_cores[cluster]) {
            d.min_cores[cluster] = d.max_cores[cluster];
        }

        snprintf(tmp_s, NODE_MAX, "%" PRId32, d.min_cores[cluster]);
        FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
        QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updating %s with %s", d.core_ctl_min_cpu_node, tmp_s);
        return rc;
    }
    else {
        QLOGE(LOG_TAG, "lock_min_cores perflock is not supported");
        return FAILED;
    }
    return 0;
}

int32_t OptsHandler::lock_max_cores(Resource &r,  OptsData &d) {
    char tmp_s[NODE_MAX];
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    uint32_t reqval = r.value;
    static uint32_t stored_val[MAX_CLUSTER] = {0};
    int32_t rc = FAILED;
    int8_t cluster = -1;

    if (d.core_ctl_present > 0) {
        cluster = r.cluster;
        snprintf(d.core_ctl_max_cpu_node, NODE_MAX, CORE_CTL_MAX_CPU, t.getFirstCoreIndex(cluster));

        if (reqval == MAX_LVL) {
           /* Update max_core first otherwise min_core wiil not update */
            d.max_cores[cluster] = atoi(d.core_ctl_max_s[cluster]);
            FWRITE_STR(d.core_ctl_max_cpu_node, d.core_ctl_max_s[cluster], d.core_ctl_max_sl[cluster], rc);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updating %s with %s ", d.core_ctl_max_cpu_node, d.core_ctl_max_s[cluster]);
            if (d.min_cores[cluster] >= 0) {
               snprintf(d.core_ctl_min_cpu_node, NODE_MAX, CORE_CTL_MIN_CPU, t.getFirstCoreIndex(cluster));
               snprintf(tmp_s, NODE_MAX, "%" PRId32, d.min_cores[cluster]);
               FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
               QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updating %s with %s ", d.core_ctl_min_cpu_node, tmp_s);
               stored_val[cluster] = 0;
            }
            return rc;
       }

       //getCoresInCluster can return FAILED. Hence, can't be unsigned.
       if ((reqval < (uint32_t)tc.getMinCoreOnline()) || (reqval > (uint32_t)tc.getCoresInCluster(cluster))) {
            QLOGE(LOG_TAG, "Error: perf-lock failed, invalid no. of cores requested to be online");
            return FAILED;
       }

        if (!stored_val[cluster]) {
            FREAD_STR(d.core_ctl_max_cpu_node, d.core_ctl_max_s[cluster], NODE_MAX, rc);
            if (rc >= 0) {
                d.core_ctl_max_sl[cluster] = strlen(d.core_ctl_max_s[cluster]);
                QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, d.core_ctl_max_cpu_node, d.core_ctl_max_s[cluster], rc);
                stored_val[cluster] = 1;
            } else {
                QLOGE(LOG_TAG, "Failed to read %s", d.core_ctl_max_cpu_node);
                return FAILED;
            }
       }

        d.max_cores[cluster] = reqval;
        snprintf(tmp_s, NODE_MAX, "%" PRId32, d.max_cores[cluster]);
        FWRITE_STR(d.core_ctl_max_cpu_node, tmp_s, strlen(tmp_s), rc);
        QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updating %s with %s ", d.core_ctl_max_cpu_node, tmp_s);
        return rc;
    }
    else if (d.kpm_hotplug_support > 0) {
        if (tc.getCoresInCluster(r.cluster) <= 0) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Warning: Cluster %" PRId8 " does not exist, resource is not supported", r.cluster);
           return FAILED;
        }

        if (reqval == MAX_LVL) {
            if (r.cluster == 0) {
               d.lock_max_clust0 = -1;
            } else if (r.cluster == 1) {
               d.lock_max_clust1 = -1;
            }
        } else {
            d.max_cores[r.cluster] = reqval;

            if (d.max_cores[r.cluster] > tc.getCoresInCluster(r.cluster)) {
               QLOGE(LOG_TAG, "Error! perf-lock failed, invalid no. of cores requested to be online");
               return FAILED;
            }

            if (r.cluster == 0) {
               d.lock_max_clust0 = d.max_cores[r.cluster];
            } else if (r.cluster == 1) {
               d.lock_max_clust1 = d.max_cores[r.cluster];
            }
       }

       snprintf(tmp_s, NODE_MAX, "%" PRId32 ":%" PRId32, d.lock_max_clust0, d.lock_max_clust1);
       char kpmSysNode[NODE_MAX];
       memset(kpmSysNode, 0, sizeof(kpmSysNode));
       PerfDataStore *store = PerfDataStore::getPerfDataStore();
       store->GetSysNode(KPM_MAX_CPUS, kpmSysNode);

       QLOGE(LOG_TAG, "Write %s into %s", tmp_s, kpmSysNode);
       FWRITE_STR(kpmSysNode, tmp_s, strlen(tmp_s), rc);
       return rc;
    }
    else
       return FAILED;
    return 0;
}

int32_t OptsHandler::handle_early_wakeup_hint(Resource &r, OptsData &d) {
    uint32_t reqVal = r.value;
    uint32_t displayId = d.getEarlyWakeupDispId();
    EventData *evData;
    noroot_args *msg;

    TargetConfig &tc = TargetConfig::getTargetConfig();
    if (!tc.getIsDefaultDivergent() && !tc.isDisplayEnabled()) {
        QLOGE(LOG_TAG, "Display not enabled");
        return FAILED;
    }

    /* perf_lock_rel */
    QLOGL(LOG_TAG, QLOG_L2, "drmIOCTL reqVal: %" PRIu32 " displayId: %" PRIu32, reqVal, displayId);
    /* perf_lock_rel */
    if (reqVal == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "drmIOCTL perf_lock_rel");
        return SUCCESS;
    }

    if (!is_no_root_alive()) {
        QLOGE(LOG_TAG, "Can't process request: noroot_thread is not running");
        return FAILED;
    }

    EventQueue *NRevqueue = get_nr_queue();
    if (NRevqueue == NULL) {
        QLOGE(LOG_TAG, "NR Event Data Queue is disabled");
        return FAILED;
    }
    evData = NRevqueue->GetDataPool().Get();

    if (!evData || !(evData->mEvData)) {
        QLOGE(LOG_TAG, "Event data pool ran empty");
        return FAILED;
    }

    msg = (noroot_args *)evData->mEvData;
    msg->val = displayId;
    msg->retval = 0;
    evData->mEvType = DISPLAY_EARLY_WAKEUP_HINT;
    /* perf_lock_acq */
    QLOGL(LOG_TAG, QLOG_L2, "calling drmIOCTLLib reqVal: %" PRIu32, reqVal);
    NRevqueue->Wakeup(evData);
    return SUCCESS;
}

/* GPU max_freq */
int32_t OptsHandler::gpu_maxfreq(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint32_t reqval = r.value;
    char tmp_s[NODE_MAX] = "";
    uint16_t idx = r.qindex;
    Target &t = Target::getCurTarget();

    if(reqval == MAX_LVL) { // Release
        //Always restore to the max GPU freq
        reqval = t.getMaxGpuFreq();
        QLOGL(LOG_TAG, QLOG_L2, "perflock release node %s value:%" PRIu32,
                                            d.sysfsnode_path[idx], reqval);
    } else {
        reqval = SecInt::Multiply(reqval, 1000000ul);
        reqval = t.findNextGpuFreq(reqval);
        QLOGL(LOG_TAG, QLOG_L2, "perflock apply node %s value:%" PRIu32,
                                            d.sysfsnode_path[idx], reqval);
    }
    snprintf(tmp_s, NODE_MAX, "%" PRIu32, reqval);
    FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    if(rc < 0 ) {
        QLOGE(LOG_TAG, "Failed to write:%s value:%" PRIu32, d.sysfsnode_path[idx], reqval);
    }

    return rc;
}

/* Disable GPU Nap */
int32_t OptsHandler::gpu_disable_gpu_nap(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint32_t stored_gpu_idle_timer = 0;
    uint32_t reqval = r.value;
    char tmp_s1[NODE_MAX];
    char tmp_s2[NODE_MAX];
    int32_t ret[4] = {-1,-1,-1,-1};
    static char gpu_nap_s[4][NODE_MAX]; // input value node
    static int32_t  gpu_nap_sl[4] = {-1,-1,-1,-1}; // input value string length
    static uint32_t stored_gpu_nap = 0;
    uint16_t qIdx = r.qindex;

    Target &target = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (gpu_nap_sl[0] > 0 && gpu_nap_sl[1] > 0 && gpu_nap_sl[2] && gpu_nap_sl[3]) {
            FWRITE_STR(GPU_FORCE_RAIL_ON, gpu_nap_s[0], gpu_nap_sl[0], ret[0]);
            FWRITE_STR(GPU_FORCE_CLK_ON, gpu_nap_s[1], gpu_nap_sl[1], ret[1]);
            FWRITE_STR(GPU_IDLE_TIMER, gpu_nap_s[2], gpu_nap_sl[2], ret[2]);
            FWRITE_STR(GPU_FORCE_NO_NAP, gpu_nap_s[3], gpu_nap_sl[3], ret[3]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, GPU_FORCE_RAIL_ON, gpu_nap_s[0], ret[0]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, GPU_FORCE_CLK_ON, gpu_nap_s[1], ret[1]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, GPU_IDLE_TIMER, gpu_nap_s[2], ret[2]);
            QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32, GPU_FORCE_NO_NAP, gpu_nap_s[3], ret[3]);
            stored_gpu_nap = 0;
        }
        rc = ret[0] && ret[1] && ret[2] && ret[3];
        return rc;
    }

    if (!stored_gpu_nap) {
        FREAD_STR(GPU_FORCE_RAIL_ON, gpu_nap_s[0], NODE_MAX, ret[0]);
        FREAD_STR(GPU_FORCE_CLK_ON, gpu_nap_s[1], NODE_MAX, ret[1]);
        FREAD_STR(GPU_IDLE_TIMER, gpu_nap_s[2], NODE_MAX, ret[2]);
        FREAD_STR(GPU_FORCE_NO_NAP, gpu_nap_s[3], NODE_MAX, ret[3]);
        if (ret[0] > 0 && ret[1] > 0 && ret[2] > 0 && ret[3] > 0) {
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, GPU_FORCE_RAIL_ON, gpu_nap_s[0], ret[0]);
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, GPU_FORCE_CLK_ON, gpu_nap_s[1], ret[1]);
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, GPU_IDLE_TIMER, gpu_nap_s[2], ret[2]);
            QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, GPU_FORCE_NO_NAP, gpu_nap_s[3], ret[3]);
            gpu_nap_sl[0] = strlen(gpu_nap_s[0]);
            gpu_nap_sl[1] = strlen(gpu_nap_s[1]);
            gpu_nap_sl[2] = strlen(gpu_nap_s[2]);
            gpu_nap_sl[3] = strlen(gpu_nap_s[3]);
            stored_gpu_nap = 1;
            stored_gpu_idle_timer = strtod(gpu_nap_s[2], NULL);
        } else {
            if (ret[0] < 0)
                QLOGE(LOG_TAG, "Failed to read %s", GPU_FORCE_RAIL_ON);
            if (ret[1] < 0)
                QLOGE(LOG_TAG, "Failed to read %s", GPU_FORCE_CLK_ON);
            if (ret[2] < 0)
                QLOGE(LOG_TAG, "Failed to read %s", GPU_IDLE_TIMER);
            if (ret[3] < 0)
                QLOGE(LOG_TAG, "Failed to read %s", GPU_FORCE_NO_NAP);
            return FAILED;
        }
    }

    snprintf(tmp_s1, NODE_MAX, "%" PRId32, 1);
    snprintf(tmp_s2, NODE_MAX, "%" PRIu32, reqval);

    FWRITE_STR(GPU_FORCE_RAIL_ON, tmp_s1, strlen(tmp_s1), ret[0]);
    FWRITE_STR(GPU_FORCE_CLK_ON, tmp_s1, strlen(tmp_s1), ret[1]);
    if (reqval > stored_gpu_idle_timer) {
        FWRITE_STR(GPU_IDLE_TIMER, tmp_s2, strlen(tmp_s2), ret[2]);
    }
    FWRITE_STR(GPU_FORCE_NO_NAP, tmp_s1, strlen(tmp_s1), ret[3]);

    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, GPU_FORCE_RAIL_ON, tmp_s1, ret[0]);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, GPU_FORCE_CLK_ON, tmp_s1, ret[1]);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, GPU_IDLE_TIMER, tmp_s2, ret[2]);
    QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32, GPU_FORCE_NO_NAP, tmp_s1, ret[3]);

    if (ret[0] < 0 && ret[1] < 0 && ret[2] < 0 && ret[3] < 0) {
        QLOGE(LOG_TAG, "Acquiring GPU nap lock failed\n");
        return FAILED;
    }
    rc = ret[0] && ret[1] && ret[2] && ret[3];
    return rc;
}

/* Set irq_balancer */
int32_t OptsHandler::irq_balancer(Resource &r, OptsData &d) {
    int32_t rc = 0;
    int8_t i = 0;
    int32_t tmp_s[MAX_CPUS];
    uint32_t reqval = r.value;
    uint16_t qIdx = r.qindex;
    Target &target = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();

    rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[qIdx]);
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.irq_bal_sp) {
            send_irq_balance(d.irq_bal_sp, NULL);
            d.stored_irq_balancer = 0;
            free(d.irq_bal_sp);
            d.irq_bal_sp = NULL;
        }
        return rc;
    }

    if (reqval > 0) {
        for (i = 0; i < tc.getTotalNumCores(); i++)
            tmp_s[i] = (reqval & (1 << i)) >> i;

        if (!d.stored_irq_balancer) {
            if (!d.irq_bal_sp) {
                d.irq_bal_sp = (int *) malloc(sizeof(*d.irq_bal_sp) * MAX_CPUS);
            }
            send_irq_balance(tmp_s, &d.irq_bal_sp);
            d.stored_irq_balancer = 1;
        } else {
            send_irq_balance(tmp_s, NULL);
        }
    }

    return rc;
}

int32_t OptsHandler::sched_util_busy_hyst_cpu_ns(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    Target &t = Target::getCurTarget();
    int8_t core_start = -1, core_end = -1;
    uint16_t qIdx = r.qindex;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[qIdx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    core_start = t.getFirstCoreIndex(r.cluster);
    core_end = t.getLastCoreIndex(r.cluster);

    if(core_start == -1 || core_end == -1) {
        return FAILED;
    }
    if (r.cluster < 0 || r.cluster >= MAX_CLUSTER) {
        return FAILED;
    }

    node_storage = d.sch_util_busy_hyst_cpy_ns_s[r.cluster];
    node_storage_length = &d.sch_util_busy_hyst_cpy_ns_sl[r.cluster];

    return multiValNodeFunc(r, d, node_storage, node_storage_length, core_start, core_end);
}

int32_t OptsHandler::sched_coloc_busy_hyst_cpu_busy_pct(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    Target &t = Target::getCurTarget();
    int8_t core_start = -1, core_end = -1;
    uint16_t qIdx = r.qindex;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[qIdx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    core_start = t.getFirstCoreIndex(r.cluster);
    core_end = t.getLastCoreIndex(r.cluster);

    if(core_start == -1 || core_end == -1) {
        return FAILED;
    }

    node_storage = d.sch_coloc_busy_hyst_cpu_busy_pct_s[r.cluster];
    node_storage_length = &d.sch_coloc_busy_hyst_cpu_busy_pct_sl[r.cluster];

    return multiValNodeFunc(r, d, node_storage, node_storage_length, core_start, core_end);
}

int32_t OptsHandler::sched_coloc_busy_hyst_cpu_ns(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    Target &t = Target::getCurTarget();
    int8_t core_start = -1, core_end = -1;
    uint16_t qIdx = r.qindex;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[qIdx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    core_start = t.getFirstCoreIndex(r.cluster);
    core_end = t.getLastCoreIndex(r.cluster);

    if(core_start == -1 || core_end == -1) {
        return FAILED;
    }

    node_storage = d.sch_coloc_busy_hyst_cpu_ns_s[r.cluster];
    node_storage_length = &d.sch_coloc_busy_hyst_cpu_ns_sl[r.cluster];

    return multiValNodeFunc(r, d, node_storage, node_storage_length, core_start, core_end);
}

int32_t OptsHandler::multiValClustNodeFunc(Resource &r, OptsData &d, char *node_storage, int32_t *node_storage_length) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX]= "";
    char tmp_val[NODE_MAX];
    char node_buff[NODE_MAX];
    uint32_t reqval = r.value;
    const size_t reqval_len = 3;  /* keeping 3 for 100 percent */
    uint32_t i = 0, c_index = r.cluster;

    if(node_storage == NULL || node_storage_length == NULL) {
        QLOGE(LOG_TAG, "Perflock node storage error");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from \
              function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length > 0) {
            rc = change_node_val(d.sysfsnode_path[idx], node_storage, strlen(node_storage));
            *node_storage_length = -1;
            return rc;
        }
        else
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for \
                  resource index=%" PRIu16 ", node path=%s", idx, d.sysfsnode_path[idx]);
    }
    else {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock acquire call for resource index = %" PRIu16 ", path = %s, from \
              function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length <= 0) {
            *node_storage_length = save_node_val(d.sysfsnode_path[idx],
                                                 node_storage);
            if (*node_storage_length <= 0) {
                QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
                return FAILED;
            }
        }

        FREAD_STR(d.sysfsnode_path[idx], node_buff, NODE_MAX, rc);
        if (rc < 0) {
            QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
            return FAILED;
        }

        char *pos = NULL;
        char *val = strtok_r(node_buff, "\t", &pos);

        if (!val) {
            QLOGE(LOG_TAG, "Failed to parse sched idle enough clust values");
            return FAILED;
        }

        if (i == c_index)
            snprintf(tmp_val, NODE_MAX, "%" PRIu32, reqval);
        else
            strlcpy(tmp_val, val, NODE_MAX);
        i++;
        while (NULL != (val = strtok_r(nullptr, "\t", &pos))) {
            strlcat(tmp_val, " ", NODE_MAX);

            if (i == c_index) {
                char val_str[reqval_len + 1];
                snprintf(val_str, reqval_len + 1, "%" PRIu32, reqval);
                strlcat(tmp_val, val_str, NODE_MAX);
            }
            else
                strlcat(tmp_val, val, NODE_MAX);
            i++;
        }

        QLOGL(LOG_TAG, QLOG_L2, "Perflock acquired for resource path = %s with value %s", d.sysfsnode_path[idx], tmp_val);
        rc = change_node_val(d.sysfsnode_path[idx], tmp_val, strlen(tmp_val));
    }

    return rc;
}

int32_t OptsHandler::multiValNodeFunc(Resource &r, OptsData &d, char *node_storage, int32_t *node_storage_length, int8_t core_start, int8_t core_end) {
    int32_t rc;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX]= "";
    uint32_t reqval = r.value;
    char *pos;
    int32_t old_value = -1;

    if(node_storage == NULL || node_storage_length == NULL) {
        QLOGE(LOG_TAG, "Perflock node storage error");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from \
                function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length > 0) {
                int8_t core_index = 0;
                old_value = -1;
                char *old_val_s = strtok_r(node_storage, "\t", &pos);
                if (!old_val_s)
                    return FAILED;
                if(core_index >= core_start && core_index <= core_end) {
                    old_value = strtol(old_val_s, NULL, BASE_10);
                }
                core_index++;
                while ((old_val_s = strtok_r(NULL, "\t", &pos)) && old_value == -1) {
                    if(core_index >= core_start && core_index <= core_end) {
                        old_value = strtol(old_val_s, NULL, BASE_10);
                    }
                    core_index++;
                }
            if (old_value == -1)
                return FAILED;
            parseMultiValNode(old_value, tmp_s,
                    d.sysfsnode_path[idx], core_start, core_end);
            rc = change_node_val_buf(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
            *node_storage_length = -1;
            return rc;
        }
        else
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for \
                    resource index=%" PRIu16 ", node path=%s", idx, d.sysfsnode_path[idx]);
    }
    else {
        if (*node_storage_length <= 0) {
            *node_storage_length = save_node_val(d.sysfsnode_path[idx],
                    node_storage);
            if (*node_storage_length <= 0) {
                QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
                return FAILED;
            }
        }
        /* extract other values within the node */
        if (parseMultiValNode(reqval, tmp_s,
                    d.sysfsnode_path[idx], core_start, core_end) == -1) {
            QLOGE(LOG_TAG, "Failed to parse migration value(s)");
            return FAILED;
        }

        return change_node_val_buf(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    }
    return FAILED;
}

int32_t OptsHandler::migrate(Resource &r, OptsData &d) {
    int32_t rc;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX]= "";
    uint32_t reqval = r.value;
    char *node_storage = NULL, *pos;
    int32_t *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    //UP Down migration thresholds are not valid for Silver cluster
    if (r.cluster == CLUSTER0) {
        QLOGE(LOG_TAG, "Request on invalid Cluster");
        return FAILED;
    }

    if (idx == SCHED_START_INDEX + SCHED_UPMIGRATE_OPCODE) {
        node_storage = d.sch_upmigrate_s[r.cluster];
        node_storage_length = &d.sch_upmigrate_sl[r.cluster];
    }
    else if (idx == SCHED_START_INDEX + SCHED_DOWNMIGRATE_OPCODE) {
        node_storage = d.sch_downmigrate_s[r.cluster];
        node_storage_length = &d.sch_downmigrate_sl[r.cluster];
    }
    else {
        QLOGE(LOG_TAG, "Resource index %" PRIu16 ": , not supported in function: %s", idx,
              __func__);
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from \
              function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length > 0) {
            if (r.cluster <= CLUSTER1) {
                char *gold_val = strtok_r(node_storage, "\t", &pos);
                if (!gold_val)
                    return FAILED;
                strlcpy(tmp_s, gold_val, NODE_MAX);
            }
            else if (r.cluster == CLUSTER2) {
                int prime_val;
                char *prime_str = strtok_r(node_storage, "\t", &pos);
                prime_str = strtok_r(nullptr, "\t", &pos);

                if (!prime_str)
                    return FAILED;
                prime_val = strtol(prime_str, NULL, BASE_10);
                parse_mig_vals(r.cluster, prime_val, tmp_s,
                               d.sysfsnode_path[idx]);
            }
            rc = change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
            *node_storage_length = -1;
            return rc;
        }
        else
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for \
                  resource index=%" PRIu16 ", node path=%s", idx, d.sysfsnode_path[idx]);
    }
    else {
        if (*node_storage_length <= 0) {
            *node_storage_length = save_node_val(d.sysfsnode_path[idx],
                                                 node_storage);
            if (*node_storage_length <= 0) {
                QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
                return FAILED;
            }
        }
        if (r.cluster == CLUSTER2) {
            /* extract other values within the node */
            if (parse_mig_vals(r.cluster, reqval, tmp_s,
                d.sysfsnode_path[idx]) == -1) {
                QLOGE(LOG_TAG, "Failed to parse migration value(s)");
                return FAILED;
            }
        }
        else
            snprintf(tmp_s, NODE_MAX, "%" PRIu32, reqval);

        return change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    }
    return FAILED;
}

/*support for combined upmigrate downmigrate opcode*/
int16_t OptsHandler::value_percluster(char node_val[NODE_MAX], int8_t cluster) {
     int16_t value = FAILED;
     char *pos;

     if (cluster <= CLUSTER1) {
         char *gold_val = strtok_r(node_val, "\t", &pos);
         if (!gold_val)
             return FAILED;
         value = strtol(gold_val, NULL, BASE_10);
     }
     else if (cluster == CLUSTER2) {
         char *cluster2_str = strtok_r(node_val, "\t", &pos);
         cluster2_str = strtok_r(nullptr, "\t", &pos);
         if (!cluster2_str)
              return FAILED;
         value = strtol(cluster2_str, NULL, BASE_10);
     }
     else if (cluster == CLUSTER3) {
         char *cluster3_str = strtok_r(node_val, "\t", &pos);
         cluster3_str = strtok_r(nullptr, "\t", &pos);
         if (!cluster3_str)
              return FAILED;
         value = strtol(cluster3_str, NULL, BASE_10);
     }
     return value;
}

/*Read and parse current upmigarte downmigarte values on the nodes*/
int16_t OptsHandler::read_curmigrate_val(const char *node_path, int8_t cluster) {
    if (node_path == NULL)
        return FAILED;

    char node_val[NODE_MAX] = "";

    int32_t node_val_length = save_node_val(node_path, node_val);
    if (node_val_length <= 0) {
        QLOGE(LOG_TAG, "Failed to read %s", node_path);
        return FAILED;
    }
    return value_percluster(node_val, cluster);
}

/* Migarate acquire call func
*  upmigrate_val = 31:16 bit in reqval
*  downmigrate_val = 15:0 bit in reqval
*  eg: reqval = 0x003C0032 is concatinated value with
*               0x32 for downmigrate, 0x3C for upmigrate.
*/
int32_t OptsHandler::migrate_action_apply(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    uint32_t reqval = r.value;
    int32_t rc = FAILED;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }
    if (ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }
    //UP Down migration thresholds are not valid for Silver cluster
    if (r.cluster == CLUSTER0) {
        QLOGE(LOG_TAG, "Request on invalid Cluster");
        return FAILED;
    }

    //parse request value
    int16_t reqUp_val = (int16_t) (reqval >> 16);
    int16_t reqDown_val = (int16_t) reqval;
    if (reqUp_val < 0 || reqDown_val < 0) {
        QLOGE(LOG_TAG, "Invalid migrate request values passed");
        return FAILED;
    }

    if ((d.sched_upmigrate[0] == '\0') && (d.sched_downmigrate[0] == '\0')) {
       snprintf(d.sched_upmigrate, NODE_MAX, d.sysfsnode_path[idx], "sched_upmigrate");
       snprintf(d.sched_downmigrate, NODE_MAX, d.sysfsnode_path[idx], "sched_downmigrate");
    }

    //determine order in which nodes are to be updated
    MIG_SEQ seq = UPDOWN;
    int16_t curUp_val = 0, curDown_val = 0;
    curUp_val = read_curmigrate_val(d.sched_upmigrate, r.cluster);
    curDown_val = read_curmigrate_val(d.sched_downmigrate, r.cluster);
    if (curUp_val == FAILED || curDown_val == FAILED) {
        QLOGE(LOG_TAG, "Unable to read default migrate values");
        return FAILED;
    }

    if (reqUp_val && reqDown_val && reqUp_val >= reqDown_val) {//both vals passed
        seq = (reqUp_val < curDown_val) ? DOWNUP : UPDOWN;
    }
    else {
        QLOGE(LOG_TAG, "Unable to apply, please pass valid values");
        return FAILED;
    }

    d.upmigrate_val[r.cluster] = reqUp_val;
    d.downmigrate_val[r.cluster] = reqDown_val;

    //action is called accordingly based on seq
    if (seq == UPDOWN) {
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_upmigrate);
        rc = migrate_action(r, d, ACQ, UP, d.upmigrate_val[r.cluster]);
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_downmigrate);
        rc =  migrate_action(r, d, ACQ, DOWN, d.downmigrate_val[r.cluster]);
    }
    else {
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_downmigrate);
        rc =  migrate_action(r, d, ACQ, DOWN, d.downmigrate_val[r.cluster]);
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_upmigrate);
        rc = migrate_action(r, d, ACQ, UP, d.upmigrate_val[r.cluster]);
    }
    return rc;
}

/*Release call for up down migrate*/
int32_t OptsHandler::migrate_action_release(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    int32_t rc = FAILED;

    //get upmigrate def value
    char up_node_val[NODE_MAX];
    strlcpy(up_node_val, d.sch_upmigrate_s[r.cluster], NODE_MAX);
    int16_t upmig_def_value = value_percluster(up_node_val, r.cluster);
    int16_t curDown_val = read_curmigrate_val(d.sched_downmigrate, r.cluster);

    //if up reset value is less than cur down, release down first
    if (upmig_def_value < curDown_val) {
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_downmigrate);
        rc = migrate_action(r, d, REL, DOWN, MAX_LVL);
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_upmigrate);
        rc = migrate_action(r, d, REL, UP, MAX_LVL);
    } else {
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_upmigrate);
        rc = migrate_action(r, d, REL, UP, MAX_LVL);
        snprintf(d.sysfsnode_path[idx], NODE_MAX, "%s", d.sched_downmigrate);
        rc = migrate_action(r, d, REL, DOWN, MAX_LVL);
    }
    return rc;
}

/* Migrate nodes are update here based on acq or rel call
*  @action = Either its a acquire or a release(ACQ/REL)
*  @flag = Indicates upmigrate node or downmigrate node
*  @reqval = Respective val after bit operation
*/
int32_t OptsHandler::migrate_action(Resource &r, OptsData &d, int32_t action, int32_t flag, uint32_t reqval) {
    int32_t rc;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX]= "";
    char *node_storage = NULL, *pos;
    int32_t *node_storage_length = NULL;

    if (flag == UP) {
        node_storage = d.sch_upmigrate_s[r.cluster];
        node_storage_length = &d.sch_upmigrate_sl[r.cluster];
    }
    else if (flag == DOWN) {
        node_storage = d.sch_downmigrate_s[r.cluster];
        node_storage_length = &d.sch_downmigrate_sl[r.cluster];
    }
    else {
        QLOGE(LOG_TAG, "Resource index %" PRIu16 ": , not supported in function: %s", idx,
              __func__);
        return FAILED;
    }

    if (action == REL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from \
              function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length > 0) {
            if (r.cluster <= CLUSTER1) {
               int gold_val;
               char *gold_str = strtok_r(node_storage, "\t", &pos);

               if (!gold_str)
                   return FAILED;
               gold_val = strtol(gold_str, NULL, BASE_10);
               parse_mig_vals(r.cluster, gold_val, tmp_s,
                                 d.sysfsnode_path[idx]);
            }
            else if (r.cluster == CLUSTER2) {
                int cluster2_val;
                char *cluster2_str = strtok_r(node_storage, "\t", &pos);
                cluster2_str = strtok_r(nullptr, "\t", &pos);

                if (!cluster2_str)
                    return FAILED;
                cluster2_val = strtol(cluster2_str, NULL, BASE_10);
                parse_mig_vals(r.cluster, cluster2_val, tmp_s,
                               d.sysfsnode_path[idx]);
            }
            else if(r.cluster == CLUSTER3) {
                int cluster3_val;
                char *cluster3_str = strtok_r(node_storage, "\t", &pos);
                cluster3_str = strtok_r(nullptr, "\t", &pos);

                if (!cluster3_str)
                    return FAILED;
                cluster3_val = strtol(cluster3_str, NULL, BASE_10);
                parse_mig_vals(r.cluster, cluster3_val, tmp_s,
                               d.sysfsnode_path[idx]);
            }
            rc = change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
            *node_storage_length = -1;
            return rc;
        }
        else
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for \
                  resource index=%" PRIu16 ", node path=%s", idx, d.sysfsnode_path[idx]);
    }
    else {
        if (*node_storage_length <= 0) {
            *node_storage_length = save_node_val(d.sysfsnode_path[idx],
                                                 node_storage);
            if (*node_storage_length <= 0) {
                QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
                return FAILED;
            }
        }

        if (parse_mig_vals(r.cluster, reqval, tmp_s,
                d.sysfsnode_path[idx]) == -1) {
                QLOGE(LOG_TAG, "Failed to parse migration value(s)");
                return FAILED;
        }
        return change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    }
    return FAILED;
}

int32_t OptsHandler::keep_alive(Resource &r, OptsData &) {
    int32_t rc = -1;
    tKillFlag = (r.value == 0) ? false : true;

    QLOGV(LOG_TAG, "keep alive tKillFlag:%" PRId8 "\n", tKillFlag);
    pthread_mutex_lock(&ka_mutex);
    if (!isThreadAlive) {
        if(!tKillFlag) {
            rc = pthread_create(&id_ka_thread, NULL, keep_alive_thread, NULL);
            if (rc!=0)
            {
                QLOGE(LOG_TAG, "Unable to create keepAlive thread, and error code is %" PRId32 "\n",rc);
            }
        }
    }
    pthread_mutex_unlock(&ka_mutex);
    return rc;
}

void* OptsHandler::keep_alive_thread(void*) {
    uint32_t len;
    uint32_t i;
    char cmd[50];

    while (true) {
        pthread_mutex_lock(&ka_mutex);
        if (!tKillFlag) {
            isThreadAlive = true;
            pthread_mutex_unlock(&ka_mutex);
            len = SecInt::Divide(sizeof (dnsIPs), sizeof (*dnsIPs));
            i = rand()%len;
            snprintf(cmd, 30, "ping -c 1 %s",dnsIPs[i]);
            system(cmd);
            QLOGL(LOG_TAG, QLOG_L3, "Hello KeepAlive~\n");
            sleep(5);
        } else {
            isThreadAlive = false;
            pthread_mutex_unlock(&ka_mutex);
            break;
        }
    }
    QLOGV(LOG_TAG, "Keep Alive Thread has gone.~~~\n");
    pthread_exit(0);
    return NULL;
}

int32_t OptsHandler::perfmode_entry_pasr(Resource &r, OptsData &d) {
    return pasr_entry_func(r,d);
}

int32_t OptsHandler::perfmode_exit_pasr(Resource &r, OptsData &d) {
    return pasr_exit_func(r,d);
}

/* Group Migarate acquire call func
*  upmigrate_val = 31:16 bit in reqval
*  downmigrate_val = 15:0 bit in reqval
*  eg: reqval = 0x003C0032 is concatinated value with
*               0x32 for downmigrate, 0x3C for upmigrate.
*/
int32_t OptsHandler::grp_migrate_action_apply(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    uint32_t reqval = r.value;
    int32_t rc = FAILED;
    char tmp_s[NODE_MAX]= "";
    int16_t curUp_val = 0, curDown_val = 0;
    int32_t ret[2] = {-1};

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource combined group up/down migrate not supported");
        return FAILED;
    }
    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    //parse request value
    int16_t reqUp_val = (int16_t) (reqval >> 16);
    int16_t reqDown_val = (int16_t) reqval;
    if (reqUp_val < 0 || reqDown_val < 0) {
        QLOGE(LOG_TAG, "Invalid migrate request values passed");
        return FAILED;
    }

    if ((d.sched_grp_upmigrate[0] == '\0') && (d.sched_grp_downmigrate[0] == '\0')) {
       snprintf(d.sched_grp_upmigrate, NODE_MAX, d.sysfsnode_path[idx], "sched_group_upmigrate");
       snprintf(d.sched_grp_downmigrate, NODE_MAX, d.sysfsnode_path[idx], "sched_group_downmigrate");
    }

    //determine order in which nodes are to be updated
    MIG_SEQ seq = UPDOWN;
    FREAD_STR(d.sched_grp_upmigrate, tmp_s, NODE_MAX, ret[0]);
    curUp_val = atoi(tmp_s);
    FREAD_STR(d.sched_grp_downmigrate, tmp_s, NODE_MAX, ret[1]);
    curDown_val = atoi(tmp_s);
    if (ret[0] == FAILED || ret[1] == FAILED) {
        QLOGE(LOG_TAG, "Unable to read default migrate values");
        return FAILED;
    }
    if (reqUp_val && reqDown_val && reqUp_val >= reqDown_val) {//both vals passed
        seq = (reqUp_val < curDown_val) ? DOWNUP : UPDOWN;
    }
    else {
        QLOGE(LOG_TAG, "Unable to apply, please pass valid values");
        return FAILED;
    }

    //store default value, will be required during release
    if(d.grp_upmigrate_sl <= 0) {
        d.grp_upmigrate_sl = save_node_val(d.sched_grp_upmigrate, d.grp_upmigrate_s);
        if(d.grp_upmigrate_sl < 0)
            QLOGE(LOG_TAG, "Unable to store default value for %s", d.sched_grp_upmigrate);
        QLOGL(LOG_TAG, QLOG_L2, "d.grp_upmigrate_s:%s d.grp_upmigrate_sl:%" PRId32, d.grp_upmigrate_s, d.grp_upmigrate_sl);
    }
    if(d.grp_downmigrate_sl <= 0) {
        d.grp_downmigrate_sl = save_node_val(d.sched_grp_downmigrate, d.grp_downmigrate_s);
        if(d.grp_downmigrate_sl < 0)
            QLOGE(LOG_TAG, "Unable to store default value for %s", d.sched_grp_downmigrate);
        QLOGL(LOG_TAG, QLOG_L2, "d.grp_downmigrate_s:%s d.grp_downmigrate_sl:%" PRId32, d.grp_downmigrate_s, d.grp_downmigrate_sl);
    }

    //action is called accordingly based on seq
    if (seq == UPDOWN) {
        snprintf(tmp_s, NODE_MAX, "%" PRId16, reqUp_val);
        FWRITE_STR(d.sched_grp_upmigrate, tmp_s, strlen(tmp_s), rc);
        QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_upmigrate, tmp_s);
        snprintf(tmp_s, NODE_MAX, "%" PRId16, reqDown_val);
        QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_downmigrate, tmp_s);
        FWRITE_STR(d.sched_grp_downmigrate, tmp_s, strlen(tmp_s), rc);
    }
    else {
        snprintf(tmp_s, NODE_MAX, "%" PRId16, reqDown_val);
        FWRITE_STR(d.sched_grp_downmigrate, tmp_s, strlen(tmp_s), rc);
        QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_downmigrate, tmp_s);
        snprintf(tmp_s, NODE_MAX, "%" PRId16, reqUp_val);
        FWRITE_STR(d.sched_grp_upmigrate, tmp_s, strlen(tmp_s), rc);
        QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_upmigrate, tmp_s);
    }
    return rc;
}

/*Release call for Group up down migrate*/
int32_t OptsHandler::grp_migrate_action_release(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    int32_t rc = FAILED;
    char tmp_s[NODE_MAX]= "";

    //get group upmigrate def value
    char up_node_val[NODE_MAX], down_node_val[NODE_MAX];
    strlcpy(up_node_val, d.grp_upmigrate_s, NODE_MAX);
    int upmig_def_value = atoi(up_node_val);

    //get group downmigrate current value
    int curDown_val, ret;
    FREAD_STR(d.sched_grp_downmigrate, down_node_val, NODE_MAX, ret);
    curDown_val = atoi(down_node_val);

    //if up reset value is less than cur down, release down first
    if (d.grp_upmigrate_sl > 0 && d.grp_downmigrate_sl > 0) {
        if (upmig_def_value < curDown_val) {
            snprintf(tmp_s, NODE_MAX, "%s", d.grp_downmigrate_s);
            FWRITE_STR(d.sched_grp_downmigrate, tmp_s, strlen(tmp_s), rc);
            QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_downmigrate, tmp_s);
            snprintf(tmp_s, NODE_MAX, "%s", d.grp_upmigrate_s);
            FWRITE_STR(d.sched_grp_upmigrate, tmp_s, strlen(tmp_s), rc);
            QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_upmigrate, tmp_s);
        } else {
            snprintf(tmp_s, NODE_MAX, "%s", d.grp_upmigrate_s);
            FWRITE_STR(d.sched_grp_upmigrate, tmp_s, strlen(tmp_s), rc);
            QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_upmigrate, tmp_s);
            snprintf(tmp_s, NODE_MAX, "%s", d.grp_downmigrate_s);
            FWRITE_STR(d.sched_grp_downmigrate, tmp_s, strlen(tmp_s), rc);
            QLOGL(LOG_TAG, QLOG_L2, "updating %s with %s", d.sched_grp_downmigrate, tmp_s);
        }
        d.grp_downmigrate_sl = 0;
        d.grp_upmigrate_sl = 0;
    }
    return rc;
}


/* Return ADD_AND_UPDATE_REQUEST if reqVal is greater than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int32_t OptsHandler::higher_is_better(uint32_t reqLevel, uint32_t curLevel) {
    int32_t ret;

    if (reqLevel > curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGL(LOG_TAG, QLOG_L2, "higher is better called , returning %" PRId32, ret);
    return ret;
}

int32_t OptsHandler::lower_is_better_negative(uint32_t reqLevel_s, uint32_t curLevel_s) {
    int32_t ret;
    uint32_t reqLevel = reqLevel_s, curLevel = curLevel_s;

    if (reqLevel < curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGL(LOG_TAG, QLOG_L2, "lower is better called , returning %" PRId32, ret);
    return ret;
}

int32_t OptsHandler::higher_is_better_negative(uint32_t reqLevel_s, uint32_t curLevel_s) {
    int32_t ret;
    uint32_t reqLevel = reqLevel_s, curLevel = curLevel_s;

    if (reqLevel > curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGL(LOG_TAG, QLOG_L2, "higher is better called , returning %" PRId32, ret);
    return ret;
}

/* Return ADD_AND_UPDATE_REQUEST if reqVal is lower than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int32_t OptsHandler::lower_is_better(uint32_t reqLevel, uint32_t curLevel) {
    int32_t ret;

    if (reqLevel < curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGL(LOG_TAG, QLOG_L2, "lower_is_better called, returning %" PRId32, ret);
    return ret;
}

/* Return ADD_AND_UPDATE_REQUEST if parsed reqVal is lower than curVal
 * Return PEND_REQUEST if parsed reqVal is less or equal to curVal
 * */
int32_t OptsHandler::migrate_lower_is_better(uint32_t reqLevel, uint32_t curLevel) {
    int32_t ret;

    int16_t reqUp_val = (int16_t) (reqLevel >> 16);
    int16_t reqDown_val = (int16_t) reqLevel;
    int16_t curUp_val = (int16_t) (curLevel >> 16);
    int16_t curDown_val = (int16_t) curLevel;

    if ((reqUp_val < curUp_val) && (reqDown_val <= reqUp_val)) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGL(LOG_TAG, QLOG_L2, "migrate_lower_is_better, curUp: %" PRId16 ", curDown: %" PRId16 ", reqUp: %" PRId16 ", reqDown: %" PRId16 ", ret: %" PRId32,
            curUp_val, curDown_val, reqUp_val, reqDown_val, ret);
    return ret;
}

/* ALways return ADD_AND_UPDATE_REQUEST
 */
int32_t OptsHandler::always_apply(uint32_t, uint32_t) {

    QLOGL(LOG_TAG, QLOG_L2, "always_apply called, returning %" PRId8, ADD_AND_UPDATE_REQUEST);
    return ADD_AND_UPDATE_REQUEST;
}

/* Used by SCHED_TASK_BOOST, node path should be /proc/<tid>/sched_boost
 * use this help to record SCHED_TASK_BOOST perflock in order by tid.
 */
int32_t OptsHandler::add_in_order(uint32_t reqLevel, uint32_t curLevel) {
    int32_t ret;

    if (reqLevel == curLevel) {
        ret = EQUAL_ADD_IN_ORDER;
    } else if (reqLevel > curLevel) {
        ret = ADD_IN_ORDER;
    } else {
        ret = PEND_ADD_IN_ORDER;
    }

    QLOGL(LOG_TAG, QLOG_L2, "add_in_order called, returning %" PRId32, ret);
    return ret;
}

/* Used by SCHED_ENABLE_TASK_BOOST_RENDERTHREAD & SCHED_DISABLE_TASK_BOOST_RENDERTHREAD,
 * node path is /proc/<tid>/sched_boost
 */
int32_t OptsHandler::add_in_order_fps_based_taskboost(uint32_t, uint32_t)
{
    QLOGL(LOG_TAG, QLOG_L2, "add_in_order_fps_based_taskboost: Returning ADD_IN_ORDER");
    return ADD_IN_ORDER;
}

/*
 * Obtains all migration values within a node except that at @index.
 * @index: Index of supplied migration value.
 * @mig_val: Migration value supplied.
 * @err: Used for error checking.
 * @acqval: Output buffer where parsed values are stored.
 * @inbuf: Path where values are parsed from.
 */
int32_t OptsHandler::parseMultiValNode(const uint32_t mig_val, char *acqval,
                                const char *inbuf, int8_t core_start, int8_t core_end) {
    int32_t rc = -1;
    char *pos;
    char node_buff[NODE_MAX];
    const size_t kmax_mig_len = 10; /* max migration value is 10 digits */
    int16_t i = 0;

    if (inbuf)
        FREAD_STR(inbuf, node_buff, NODE_MAX, rc);

    if (!inbuf || rc < 0) {
        QLOGE(LOG_TAG, "Failed to read %s", inbuf);

        return FAILED;
    }

    char *val = strtok_r(node_buff, "\t", &pos);

    if (!val) {
        QLOGE(LOG_TAG, "Failed to parse migration values");
        return FAILED;
    }

    if (i >= core_start && i <= core_end) {
        snprintf(acqval, NODE_MAX, "%" PRIu32, mig_val);
        rc = 0;
    }
    else
        strlcpy(acqval, val, NODE_MAX);
    i++;

    while (NULL != (val = strtok_r(nullptr, "\t", &pos))) {

        strlcat(acqval, "\t", NODE_MAX);

        if(i >= core_start && i <= core_end) {
            char val_str[kmax_mig_len + 1];
            snprintf(val_str, kmax_mig_len + 1, "%" PRIu32, mig_val);
            strlcat(acqval, val_str, NODE_MAX);
            rc = 0;
        }
        else
            strlcat(acqval, val, NODE_MAX);
        i++;
    }
    return rc;
}

/*
 * Obtains all migration values within a node except that at @index.
 * @index: Index of supplied migration value.
 * @mig_val: Migration value supplied.
 * @err: Used for error checking.
 * @acqval: Output buffer where parsed values are stored.
 * @inbuf: Path where values are parsed from.
 */
int32_t OptsHandler::parse_mig_vals(uint32_t cluster, const uint32_t mig_val, char *acqval,
                                const char *inbuf) {
    int32_t rc = -1;
    char node_buff[NODE_MAX];
    const size_t kmax_mig_len = 4; /* max migration value is 4 digits */

    if (inbuf)
        FREAD_STR(inbuf, node_buff, NODE_MAX, rc);

    if (!inbuf || rc < 0) {
        QLOGE(LOG_TAG, "Failed to read %s", inbuf);

        return -1;
    }

    char *pos;
    char *val = strtok_r(node_buff, "\t", &pos);

    if (!val) {
        QLOGE(LOG_TAG, "Failed to parse migration values");

        return -1;
    }
    uint32_t i = 0;

    uint32_t index = cluster ? cluster - 1 : cluster; // cluster 1 = index 0 and so forth

    if (i == index) {
        snprintf(acqval, NODE_MAX, "%" PRIu32, mig_val);
        rc = 0;
    }
    else
        strlcpy(acqval, val, NODE_MAX);
    i++;

    while (NULL != (val = strtok_r(nullptr, "\t", &pos))) {
        strlcat(acqval, "\t", NODE_MAX);

        if (i == index) {
            char val_str[kmax_mig_len + 1];
            snprintf(val_str, kmax_mig_len + 1, "%" PRIu32, mig_val);
            strlcat(acqval, val_str, NODE_MAX);
            rc = 0;
        }
        else
            strlcat(acqval, val, NODE_MAX);
        i++;
    }
    return rc;
}

/**
 The value /proc/<pid>/sched_boost
 0:disable task boost
 1:task will prefer to use gold or super big core according to the cpu state
 2:task will prefer to use super big core, need un-isolate it first
 3:similar as affinity without inheritable, force task to use the max capability core
*/
int32_t OptsHandler::sched_task_boost(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    int32_t kernelMajor = 0, kernelMinor = 0;
    int32_t reqval = r.value;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    //Check if any decrement value is encoded
    int16_t dec = (int16_t) (reqval >> 16);
    // invalid boost. Force to default MAX
    if (dec > 2)
        dec = 0;
    uint32_t tid = reqval & 0x0000FFFF;
    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRIu32 " %" PRId16, tid, TASK_BOOST_STRICT_MAX - dec);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        char node_path[NODE_MAX] = "";
        snprintf(node_path, NODE_MAX, d.sysfsnode_path[idx], tid);
        snprintf(tmp_s, NODE_MAX, "%" PRId16, TASK_BOOST_STRICT_MAX - dec);
        rc = update_node_param(d.node_type[idx], node_path, tmp_s, strlen(tmp_s));
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't boost task %" PRIu32, tid);
    return rc;
}

int32_t OptsHandler::sched_reset_task_boost(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    uint32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();
    uint32_t reqval = r.value & 0x0000FFFF;

    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRIu32 " %" PRId8, reqval, 0);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        char node_path[NODE_MAX] = "";
        snprintf(node_path, NODE_MAX, d.sysfsnode_path[idx], reqval);
        snprintf(tmp_s, NODE_MAX, "%" PRId8, 0);
        rc = update_node_param(d.node_type[idx], node_path, tmp_s, strlen(tmp_s));
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't reset task %" PRIu32, r.value);
    else
        QLOGL(LOG_TAG, QLOG_L2, "Reset task %" PRIu32, r.value);
    return rc;
}

/* Apply task boost to top-app's render thread tid */
int32_t OptsHandler::sched_enable_task_boost_renderthread(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char node_path[NODE_MAX] = "";
    snprintf(node_path, NODE_MAX, d.sysfsnode_path[idx], r.value);
    char tmp_s[NODE_MAX] = "";
    snprintf(tmp_s, NODE_MAX, "%" PRId8, TASK_BOOST_ON_MID);
    rc =  update_node_param(d.node_type[idx], node_path, tmp_s, strlen(tmp_s));
    if (rc < 0)
       QLOGE(LOG_TAG, "can't boost task %" PRIu32, r.value);
    else
       QLOGL(LOG_TAG, QLOG_L2, "Boosted task %" PRIu32, r.value);
    return rc;
}

int32_t OptsHandler::gpu_is_app_fg(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    char fg_node_path[NODE_MAX] = "";

    snprintf(fg_node_path, NODE_MAX, d.sysfsnode_path[idx], r.value);
    if (access(fg_node_path, F_OK) != -1) {
       snprintf(tmp_s, NODE_MAX, "%s", "foreground");
       rc =  update_node_param(d.node_type[idx], fg_node_path, tmp_s, strlen(tmp_s));
       if (rc < 0)
          QLOGE(LOG_TAG, "Failed to update %s with %s return value %" PRId32,fg_node_path,tmp_s,rc);
       else
          QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32,fg_node_path,tmp_s,rc);
    }
    return rc;
}

int32_t OptsHandler::gpu_is_app_bg(Resource &r, OptsData &d) {
     int32_t rc = FAILED;
     uint16_t idx = r.qindex;
     char tmp_s[NODE_MAX] = "";
     char bg_node_path[NODE_MAX] = "";

     snprintf(bg_node_path, NODE_MAX, d.sysfsnode_path[idx], r.value);
     if (access(bg_node_path, F_OK) != -1) {
        snprintf(tmp_s, NODE_MAX, "%s", "background");
        rc =  update_node_param(d.node_type[idx], bg_node_path, tmp_s, strlen(tmp_s));
        if (rc < 0)
           QLOGE(LOG_TAG, "Failed to update %s with %s return value %" PRId32,bg_node_path,tmp_s,rc);
        else
           QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32,bg_node_path,tmp_s,rc);
     }
     return rc;
}

/* Set sched sysfs node: /proc/<pid>/sched_low_latency
   Boost specific low latency tasks to ensure they get into the runqueue
   as fast as possible.
*/
int32_t OptsHandler::sched_low_latency(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    uint32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRIu32 " %" PRId8, r.value, 1);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        char node_path[NODE_MAX] = "";
        snprintf(node_path, NODE_MAX, d.sysfsnode_path[idx], r.value);
        snprintf(tmp_s, NODE_MAX, "%" PRId8, 1);
        rc = update_node_param(d.node_type[idx], node_path, tmp_s, strlen(tmp_s));
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't set low latency for tid %" PRIu32, r.value);
    return rc;
}

int32_t OptsHandler::sched_reset_low_latency(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    uint32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRIu32 " %" PRId8, r.value, 0);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        char node_path[NODE_MAX] = "";
        snprintf(node_path, NODE_MAX, d.sysfsnode_path[idx], r.value);
        snprintf(tmp_s, NODE_MAX, "%" PRId8, 0);
        rc = update_node_param(d.node_type[idx], node_path, tmp_s, strlen(tmp_s));
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't reset low latency for tid %" PRIu32, r.value);
    else
        QLOGL(LOG_TAG, QLOG_L2, "Reset low latency for tid %" PRIu32, r.value);
    return rc;
}

int32_t OptsHandler::sched_task_load_boost(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char sign_bit = (r.value & 0x80) ? '-' : ' ';
    uint32_t val = r.value & 0x7F;
    int32_t tid = r.value >> 8;
    char tmp_s[NODE_MAX];
    if (val > 90) {
        // > 90 or < -90 is not allowed
        QLOGE(LOG_TAG, "Invalid value for task_load_boost: %c%" PRIu32, sign_bit, val);
        return rc;
    }
    snprintf(tmp_s, NODE_MAX, "%" PRId32 " %c%" PRIu32, tid, sign_bit, val);
    rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    if (rc < 0) {
        QLOGE(LOG_TAG, "can't apply task_load_boost for %s", tmp_s);
    }
    return rc;
}

int32_t OptsHandler::sched_reset_task_load_boost(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    int32_t tid = r.value >> 8;
    char tmp_s[NODE_MAX];
    snprintf(tmp_s, NODE_MAX, "%" PRId32 " 0", tid);
    rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    if (rc < 0) {
        QLOGE(LOG_TAG, "can't release task_load_boost for %s", tmp_s);
    }
    return rc;
}

int32_t OptsHandler::getThreadCgroup(int32_t tid, char *cgrp) {
    char fName[NODE_MAX] = "";
    int32_t rc = FAILED;

    memset(cgrp, '\0', NODE_MAX);
    snprintf(fName, NODE_MAX, THREAD_CGROUP_NODE, tid);
    FREAD_STR(fName, cgrp, NODE_MAX, rc);
    if (rc < 0) {
        QLOGE(LOG_TAG, "Failed to read cgroup for tid:%" PRId32, tid);
        return FAILED;
    }
    cgrp[rc-1] = '\0';
    return rc;
}

int32_t OptsHandler::set_pid_affine(Resource &, OptsData &d) {
    FILE *fgSet;
    char buff[NODE_MAX];
    int32_t rc = 0;
    uint32_t len = 0;
    int8_t cpu = -1, cluster = -1;
    int8_t startCpu = -1, endCpu  = -1;
    Target &t = Target::getCurTarget();
    if(d.hwcPid <= 0 && d.sfPid <= 0 && d.reTid <= 0) {
        QLOGE(LOG_TAG, "HWC, SF, RE TIDs <= 0, set_pid_affine failed");
        return FAILED;
    }
    if (d.hwcPid > 0) {
        getThreadCgroup(d.hwcPid, d.hwcCgroup);
        fgSet = fopen(FOREGROUND_TASK_NODE, "a+");
        if (fgSet == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create foreground cgroup file\n");
            return FAILED;
        }
        memset(buff, 0, sizeof(buff));
        QLOGL(LOG_TAG, QLOG_L2, "writing hwc pid:%" PRId32 " on node:%s\n", d.hwcPid, FOREGROUND_TASK_NODE);
        snprintf(buff, NODE_MAX, "%" PRId32, d.hwcPid);
        len = strlen(buff);
        if (len >= (NODE_MAX-1)) {
            fclose(fgSet);
            return FAILED;
        }
        buff[len+1] = '\0';
        rc = fwrite(buff, sizeof(char), len+1, fgSet);
        fclose(fgSet);
    }

    /* writing SF's PID to foreground/cgroup.procs causes all threads(including RE thread)
        in the threadgroup to be attached to fg cgroup*/
    if (d.sfPid > 0) {
        getThreadCgroup(d.sfPid, d.sfCgroup);
        fgSet = fopen(FOREGROUND_TASK_NODE, "a+");
        if (fgSet == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create foreground cgroup file\n");
            return FAILED;
        }
        memset(buff, 0, sizeof(buff));
        QLOGL(LOG_TAG, QLOG_L2, "writing sf pid:%" PRId32 " on node:%s\n", d.sfPid, FOREGROUND_TASK_NODE);
        snprintf(buff, NODE_MAX, "%" PRId32, d.sfPid);
        len = strlen(buff);
        if (len >= (NODE_MAX-1)) {
            fclose(fgSet);
            return FAILED;
        }
        buff[len+1] = '\0';
        rc = fwrite(buff, sizeof(char), len+1, fgSet);
        fclose(fgSet);
    }

    cpu_set_t set;

    CPU_ZERO(&set);
    //setting affinity to Gold Cluster
    cpu = t.getFirstCoreOfPerfCluster();
    if (cpu < 0) {
        QLOGE(LOG_TAG, "Couldn't find perf cluster");
        return FAILED;
    }
    cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
    if ((startCpu < 0) || (endCpu < 0) || (cluster < 0)) {
        QLOGE(LOG_TAG, "Could not find a cluster corresponding the core %" PRId8, cpu);
        return FAILED;
    }
    for (int32_t  i = startCpu; i <= endCpu; i++) {
        CPU_SET(i, &set);
    }
    rc = sched_setaffinity(d.hwcPid, sizeof(cpu_set_t), &set);
    QLOGL(LOG_TAG, QLOG_L2, "sched_setaffinity hwcPid:%" PRId32 " rc:%" PRId32, d.hwcPid, rc);
    rc = sched_setaffinity(d.sfPid, sizeof(cpu_set_t), &set);
    QLOGL(LOG_TAG, QLOG_L2, "sched_setaffinity sfPid:%" PRId32 " rc:%" PRId32, d.sfPid, rc);
    rc = sched_setaffinity(d.reTid, sizeof(cpu_set_t), &set);
    QLOGL(LOG_TAG, QLOG_L2, "sched_setaffinity reTid:%" PRId32 " rc:%" PRId32, d.reTid, rc);

    return SUCCESS;
}

int32_t OptsHandler::reset_pid_affine(Resource &, OptsData &d) {
    FILE *pFile;
    char buff[NODE_MAX];
    int32_t rc = 0;
    uint32_t len = 0;
    if (d.hwcPid > 0) {
        char fname[NODE_MAX] = "";

        if (d.hwcCgroup[0] == '\0') {
            snprintf(fname, NODE_MAX, SYSBG_TASK_NODE);
        } else {
            snprintf(fname, NODE_MAX, CPUSET_NODE, d.hwcCgroup);
        }
        pFile = fopen(fname, "a+");
        if (pFile == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create system background cgroup file\n");
            return FAILED;
        }
        memset(buff, 0, sizeof(buff));
        QLOGL(LOG_TAG, QLOG_L2, "writing hwc pid:%" PRId32 " on node:%s\n", d.hwcPid, fname);
        snprintf(buff, NODE_MAX, "%" PRId32, d.hwcPid);
        len = strlen(buff);
        if (len >= (NODE_MAX-1)) {
            fclose(pFile);
            return FAILED;
        }
        buff[len+1] = '\0';
        rc = fwrite(buff, sizeof(char), len+1, pFile);
        fclose(pFile);
    }

    /* writing SF's PID to <cgroup>/cgroup.procs causes all threads(including RE thread)
        in the threadgroup to be attached to bg/custom cgroup*/
    if (d.sfPid > 0) {
        char fname[NODE_MAX] = "";

        if (d.sfCgroup[0] == '\0') {
            snprintf(fname, NODE_MAX, SYSBG_TASK_NODE);
        } else {
            snprintf(fname, NODE_MAX, CPUSET_NODE, d.sfCgroup);
        }
        pFile = fopen(fname, "a+");
        if (pFile == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create system background cgroup file\n");
            return FAILED;
        }
        memset(buff, 0, sizeof(buff));
        QLOGL(LOG_TAG, QLOG_L2, "writing sf pid:%" PRId32 " on node:%s\n", d.sfPid, fname);
        snprintf(buff, NODE_MAX, "%" PRId32, d.sfPid);
        len = strlen(buff);
        if (len >= (NODE_MAX-1)) {
            fclose(pFile);
            return FAILED;
        }
        buff[len+1] = '\0';
        rc = fwrite(buff, sizeof(char), len+1, pFile);
        fclose(pFile);
    }
    return SUCCESS;
}

int32_t OptsHandler::handle_fps_hyst(Resource &r, OptsData &d) {
    uint32_t reqVal = r.value;

    if (reqVal == MAX_LVL) { /* perf_lock_rel */
        QLOGL(LOG_TAG, QLOG_L2, "OptsHandler::handle_fps_hyst perf_lock_rel reqVal: %" PRIu32, reqVal);
        d.set_fps_hyst_time(-1.0f);
    }
    else {  /* perf_lock_acq */
        QLOGL(LOG_TAG, QLOG_L2, "OptsHandler::handle_fps_hyst perf_lock_acq reqVal: %" PRIu32, reqVal);
        d.set_fps_hyst_time(reqVal);
    }
    return 0;
}

int32_t OptsHandler::set_scheduler(Resource &r, OptsData &) {
    int32_t rc = 0;
    uint32_t reqVal = r.value;

    if (reqVal == 0)
        return FAILED;

    // Allow atmost 3 tids to change sched policy
    if (mSP.size() == 3) {
        SchedPolicyTable spt = mSP.at(2);
        uint32_t old_tid = spt.tid;
        uint32_t orig_policy = spt.policy;
        struct sched_param orig_sp = spt.param;
        // requirement to set to 0
        orig_sp.sched_priority = 0;
        rc = sched_setscheduler(old_tid, orig_policy, &orig_sp);
        if (rc < 0) {
            QLOGE(LOG_TAG, "Failed to reset tid scheduler for removal:%" PRId32 " rc: %" PRId32, old_tid, rc);
        }
        mSP.pop_back();
    } else {
        SchedPolicyTable npt;
        uint32_t orig_policy = sched_getscheduler(reqVal);
        struct sched_param orig_sp;
        sched_getparam(reqVal, &orig_sp);
        npt.tid = reqVal;
        npt.policy = orig_policy;
        npt.param = orig_sp;
        OptsHandler::mSP.insert(OptsHandler::mSP.begin(), npt);
        struct sched_param param;
        param.sched_priority = 0;
        rc = sched_setscheduler(reqVal, SCHED_IDLE, &param);
        QLOGL(LOG_TAG, QLOG_L2, "sched_setscheduler tid:%" PRIu32 " rc:%" PRId32, reqVal, rc);
    }
    return SUCCESS;
}

int32_t OptsHandler::reset_scheduler(Resource &r, OptsData &) {
    int32_t rc = 0;
    uint32_t reqVal = r.value;
    if (reqVal == 0)
        return FAILED;

    for (auto it = mSP.begin(); it != mSP.end(); ++it) {
        if (it->tid == reqVal) {
            int orig_policy = it->policy;
            struct sched_param orig_sp = it->param;
            // requirement to set to 0
            orig_sp.sched_priority = 0;
            rc = sched_setscheduler(reqVal, orig_policy, &orig_sp);
            if (rc < 0) {
               QLOGE(LOG_TAG, "Failed to reset tid scheduler:%" PRIu32 " rc: %" PRId32, reqVal, rc);
               return FAILED;
            }
            mSP.erase(it);
            break;
        }
    }
    QLOGL(LOG_TAG, QLOG_L3, "reset setscheduler tid:%" PRIu32 " rc:%" PRId32, reqVal, rc);
    return SUCCESS;
}

/* Set sched sysfs node: /proc/sys/walt/sched_thread_pipeline
   Boost specific tasks to ensure they are low latency & also
   have silver task filtering disabled
*/
int32_t OptsHandler::sched_thread_pipeline(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    uint32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRIu32 " %" PRId8, r.value, 1);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        QLOGE(LOG_TAG, "can't set thread pipeline for tid %" PRIu32 " due to unsupported kernel", r.value);
        return rc;
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't set thread pipeline for tid %" PRIu32 " %s", r.value, d.sysfsnode_path[idx]);
    return rc;
}

int32_t OptsHandler::sched_reset_thread_pipeline(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";
    uint32_t kernelMajor = 0, kernelMinor = 0;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    kernelMajor = tc.getKernelMajorVersion();
    kernelMinor = tc.getKernelMinorVersion();

    if ((kernelMajor == 5 && kernelMinor >= 10) || kernelMajor > 5) {
        snprintf(tmp_s, NODE_MAX, "%" PRId32 " %" PRId8, r.value, 0);
        rc = update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    } else {
        QLOGE(LOG_TAG, "can't reset thread pipeline for tid %" PRIu32 " due to unsupported kernel", r.value);
        return rc;
    }
    if (rc < 0)
        QLOGE(LOG_TAG, "can't reset thread pipeline for tid %" PRIu32, r.value);
    return rc;
}

//Function to run display heavy RT (SurfaceFlinger, RenderEngine) on deterministic Gold
//cluster for CPU topology with <=2 silvers
int32_t OptsHandler::set_display_heavy_rt_affine(Resource&, OptsData &d) {
    int32_t rc = 0;
    int8_t cpu = -1, cluster = -1;
    int8_t startCpu = -1, endCpu  = -1;
    Target &t = Target::getCurTarget();
    int8_t numSilvers = -1;
    if(d.sfPid <= 0 && d.reTid <= 0) {
        QLOGE(LOG_TAG, "SF, RE Pid <= 0, set_display_heavy_rt_affine failed");
        return FAILED;
    }
    numSilvers = t.getNumLittleCores();
    if(numSilvers > 2 || numSilvers < 0) {
        QLOGE(LOG_TAG, "numSilvers:%d, will not determine RT spillover beyond silver", numSilvers);
        return FAILED;
    }
    //For CPU topology with 2 silvers, run SF, RE on deterministic Gold Cluster
    //Due to sync feature, Hardware Composer will run on same core as SF.
    cpu = t.getFirstCoreOfClusterWithSpilloverRT();
    if (cpu < 0) {
        QLOGE(LOG_TAG, "getFirstCoreOfClusterWithSpilloverRT failed");
        return FAILED;
    }
    cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
    if ((startCpu < 0) || (endCpu < 0) || (cluster < 0)) {
        QLOGE(LOG_TAG, "Could not find a cluster corresponding to the core %" PRId8, cpu);
        return FAILED;
    }
    QLOGL(LOG_TAG, QLOG_L1, "Run SF/RE on cluster=%d CPUs(%d-%d)", cluster, startCpu, endCpu);
    cpu_set_t set;
    CPU_ZERO(&set);
    for (int32_t  i = startCpu; i <= endCpu; i++) {
        CPU_SET(i, &set);
    }
    if (d.sfPid > 0) {
        rc = sched_setaffinity(d.sfPid, sizeof(cpu_set_t), &set);
        QLOGL(LOG_TAG, QLOG_L1, "sched_setaffinity sfPid:%" PRId32 " rc:%" PRId32, d.sfPid, rc);
    }
    if (d.reTid > 0) {
        rc = sched_setaffinity(d.reTid, sizeof(cpu_set_t), &set);
        QLOGL(LOG_TAG, QLOG_L1, "sched_setaffinity reTid:%" PRId32 " rc:%" PRId32, d.reTid, rc);
    }

    return SUCCESS;
}

int32_t OptsHandler::reset_display_heavy_rt_affine(Resource&, OptsData &d) {
    int32_t rc = 0;
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    int8_t numSilvers = t.getNumLittleCores();
    if(numSilvers > 2 || numSilvers < 0) {
        QLOGL(LOG_TAG, QLOG_L1, "numSilvers: %d, No need to reset any affinities", numSilvers);
        return FAILED;
    }
    cpu_set_t set;
    CPU_ZERO(&set);
    for (int32_t  i = 0; i < tc.getTotalNumCores(); i++) {
        CPU_SET(i, &set);
    }
    if (d.sfPid > 0) {
        rc = sched_setaffinity(d.sfPid, sizeof(cpu_set_t), &set);
        QLOGL(LOG_TAG, QLOG_L1, "sched_setaffinity sfPid:%" PRId32 " rc:%" PRId32, d.sfPid, rc);
    }
    if (d.reTid > 0) {
        rc = sched_setaffinity(d.reTid, sizeof(cpu_set_t), &set);
        QLOGL(LOG_TAG, QLOG_L1, "sched_setaffinity reTid:%" PRId32 " rc:%" PRId32, d.reTid, rc);
    }
    return SUCCESS;
}

/* Enable sysfs node: /sys/devices/system/cpu/dynpf/enable_dynpf
   Enable dynamic prefetcher for Gold.
*/
int32_t OptsHandler::enable_gold_dynprefetcher(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";

    if (access(d.sysfsnode_path[idx], F_OK) != -1) {
        snprintf(tmp_s, NODE_MAX, "%" PRId8, 1);
        rc =  update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
        if (rc < 0)
           QLOGE(LOG_TAG, "Failed to update %s with %s return value %" PRId32,d.sysfsnode_path[idx],tmp_s,rc);
        else
           QLOGL(LOG_TAG, QLOG_L2, "perf_lock_acq: updated %s with %s return value %" PRId32,
                   d.sysfsnode_path[idx],tmp_s,rc);
     }
     return rc;
}

int32_t OptsHandler::disable_gold_dynprefetcher(Resource &r, OptsData &d) {
    int32_t rc = FAILED;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX] = "";

    if (access(d.sysfsnode_path[idx], F_OK) != -1) {
        snprintf(tmp_s, NODE_MAX, "%" PRId8, 0);
        rc =  update_node_param(d.node_type[idx], d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
        if (rc < 0)
           QLOGE(LOG_TAG, "Failed to update %s with %s return value %" PRId32,d.sysfsnode_path[idx],tmp_s,rc);
        else
           QLOGL(LOG_TAG, QLOG_L2, "perf_lock_rel: updated %s with %s return value %" PRId32,
                   d.sysfsnode_path[idx],tmp_s,rc);
     }
     return rc;
}

/*For setting /proc/sys/walt/sched_fmax_cap node */
int32_t OptsHandler::sched_fmax_cap(Resource &r, OptsData &d) {
    int32_t rc;
    uint16_t idx = r.qindex;
    char tmp_s[NODE_MAX]= "";
    uint32_t reqval = r.value;
    char *node_storage = NULL, *pos = NULL;
    int32_t *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    if (idx == SCHED_EXT_START_INDEX + SCHED_FMAX_CAP) {
        node_storage = d.sch_fmax_cap_s[r.cluster];
        node_storage_length = &d.sch_fmax_cap_sl[r.cluster];
    }
    else {
        QLOGE(LOG_TAG, "Resource index %" PRIu16 ": , not supported in function: %s", idx,
              __func__);
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        QLOGL(LOG_TAG, QLOG_L2, "Perflock release call for resource index = %" PRIu16 ", path = %s, from \
              function = %s", idx, d.sysfsnode_path[idx], __func__);

        if (*node_storage_length > 0) {
            int8_t cluster_index = 0;
            int32_t cluster_value = -1;
            char *cluster_val_s = strtok_r(node_storage, "\t", &pos);
            if (!cluster_val_s)
                return FAILED;
            cluster_value = strtol(cluster_val_s, NULL, BASE_10);

            while (NULL != (cluster_val_s = strtok_r(NULL, "\t", &pos)) && cluster_index != r.cluster) {
                cluster_value = strtol(cluster_val_s, NULL, BASE_10);
                cluster_index++;
            }
            if (parse_freq_vals(r.cluster, cluster_value, tmp_s,
                              d.sysfsnode_path[idx]) == -1) {
                QLOGE(LOG_TAG, "Failed to parse frequency value(s)");
                return FAILED;
            }
            rc = change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
            *node_storage_length = -1;
            return rc;
        }
        else
            QLOGE(LOG_TAG, "Unable to find the correct node storage pointers for \
                  resource index=%" PRIu16 ", node path=%s", idx, d.sysfsnode_path[idx]);
    }
    else {
        if (*node_storage_length <= 0) {
            *node_storage_length = save_node_val(d.sysfsnode_path[idx],
                                                 node_storage);
            if (*node_storage_length <= 0) {
                QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx]);
                return FAILED;
            }
        }

        if (parse_freq_vals(r.cluster, reqval, tmp_s,
            d.sysfsnode_path[idx]) == -1) {
            QLOGE(LOG_TAG, "Failed to parse frequency value(s)");
            return FAILED;
        }
        return change_node_val(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s));
    }
    return FAILED;
}

int32_t OptsHandler::parse_freq_vals(uint32_t cluster, const uint32_t freq_val, char *acqval,
                                const char *inbuf) {
    int32_t rc = -1;
    char node_buff[NODE_MAX];
    const size_t kmax_freq_len = 10;
    uint32_t i = 0, index = cluster;

    if (inbuf)
        FREAD_STR(inbuf, node_buff, NODE_MAX, rc);

    if (!inbuf || rc < 0) {
        QLOGE(LOG_TAG, "Failed to read %s", inbuf);
        return FAILED;
    }

    char *pos = NULL;
    char *val = strtok_r(node_buff, "\t", &pos);

    if (!val) {
        QLOGE(LOG_TAG, "Failed to parse frequency values");
        return FAILED;
    }

    if (i == index) {
        snprintf(acqval, NODE_MAX, "%" PRIu32, freq_val);
        rc = 0;
    }
    else
        strlcpy(acqval, val, NODE_MAX);
    i++;
    while (NULL != (val = strtok_r(nullptr, "\t", &pos))) {
        strlcat(acqval, " ", NODE_MAX);

        if (i == index) {
            char val_str[kmax_freq_len + 1];
            snprintf(val_str, kmax_freq_len + 1, "%" PRIu32, freq_val);
            strlcat(acqval, val_str, NODE_MAX);
            rc = 0;
        }
        else
            strlcat(acqval, val, NODE_MAX);
        i++;
    }
    return rc;
}

int32_t OptsHandler::sched_idle_enough_clust_system(Resource &r, OptsData &d) {
    int32_t rc;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;
    uint16_t idx_clust = SCHED_EXT_START_INDEX + SCHED_IDLE_ENOUGH_CLUST;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    char node_buff[NODE_MAX];
    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    node_storage = d.sched_idle_enough_s[r.cluster];
    node_storage_length = &d.sched_idle_enough_sl[r.cluster];
    if(node_storage == NULL || node_storage_length == NULL) {
        QLOGE(LOG_TAG, "Perflock node storage error");
        return FAILED;
    }
    QLOGL(LOG_TAG, QLOG_L2, "node_storage: %s", node_storage);

    // Check the presense of cluster wise node. If it is present use it otherwise fall back to system wise node
    FREAD_STR(d.sysfsnode_path[idx_clust], node_buff, NODE_MAX, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "cluster wise node: %s is present", d.sysfsnode_path[idx_clust]);
        if (reqval == MAX_LVL) {
            QLOGL(LOG_TAG, QLOG_L2, "Perflock call release call for resource index = %" PRIu16 ", path = %s, from \
                  function = %s", idx_clust, d.sysfsnode_path[idx_clust], __func__);
            rc = change_node_val(d.sysfsnode_path[idx_clust], node_storage, strlen(node_storage));
        }
        else {
            QLOGL(LOG_TAG, QLOG_L2, "Perflock call acquire call for resource index = %" PRIu16 ", path = %s, from \
                  function = %s", idx_clust, d.sysfsnode_path[idx_clust], __func__);
            if (*node_storage_length <= 0) {
                *node_storage_length = save_node_val(d.sysfsnode_path[idx_clust],
                                                     node_storage);
                if (*node_storage_length <= 0) {
                    QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx_clust]);
                    return FAILED;
                }
            }

            uint32_t str_index = 0;
            char tmp_val[NODE_MAX] = {0};
            for(int8_t c = 0; c < tc.getNumCluster(); c++) {
                int chars_copied = snprintf(tmp_val + str_index, NODE_MAX - str_index, "%d ", reqval);
                str_index += chars_copied;
            }
            if(str_index > 0) {
                tmp_val[str_index-1] = '\0';
            }
            rc = change_node_val(d.sysfsnode_path[idx_clust], tmp_val, strlen(tmp_val));
        }
        return rc;
    }
    else {
        return modify_sysfsnode(r, d);
    }
}

int32_t OptsHandler::sched_idle_enough_clust(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    node_storage = d.sched_idle_enough_s[r.cluster];
    node_storage_length = &d.sched_idle_enough_sl[r.cluster];

    return multiValClustNodeFunc(r, d, node_storage, node_storage_length);
}

int32_t OptsHandler::sched_util_thres_pct_system(Resource &r, OptsData &d) {
    int32_t rc;
    uint32_t reqval = r.value;
    uint16_t idx = r.qindex;
    uint16_t idx_clust = SCHED_EXT_START_INDEX + SCHED_UTIL_THRES_PCT_CLUST;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;
    char node_buff[NODE_MAX];
    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    node_storage = d.sched_util_thres_pct_clust_s[r.cluster];
    node_storage_length = &d.sched_util_thres_pct_clust_sl[r.cluster];
    if(node_storage == NULL || node_storage_length == NULL) {
        QLOGE(LOG_TAG, "Perflock node storage error");
        return FAILED;
    }
    QLOGL(LOG_TAG, QLOG_L2, "node_storage: %s", node_storage);

    // Check the presense of cluster wise node. If it is present use it otherwise fall back to system wise node
    FREAD_STR(d.sysfsnode_path[idx_clust], node_buff, NODE_MAX, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "cluster wise node: %s is present", d.sysfsnode_path[idx_clust]);
        if (reqval == MAX_LVL) {
            QLOGL(LOG_TAG, QLOG_L2, "Perflock call release call for resource index = %" PRIu16 ", path = %s, from \
                  function = %s", idx_clust, d.sysfsnode_path[idx_clust], __func__);
            rc = change_node_val(d.sysfsnode_path[idx_clust], node_storage, strlen(node_storage));
        }
        else {
            QLOGL(LOG_TAG, QLOG_L2, "Perflock call acquire call for resource index = %" PRIu16 ", path = %s, from \
                  function = %s", idx_clust, d.sysfsnode_path[idx_clust], __func__);
            if (*node_storage_length <= 0) {
                *node_storage_length = save_node_val(d.sysfsnode_path[idx_clust],
                                                     node_storage);
                if (*node_storage_length <= 0) {
                    QLOGE(LOG_TAG, "Failed to read %s", d.sysfsnode_path[idx_clust]);
                    return FAILED;
                }
            }

            uint32_t str_index = 0;
            char tmp_val[NODE_MAX] = {0};
            for(int8_t c = 0; c < tc.getNumCluster(); c++) {
                int chars_copied = snprintf(tmp_val + str_index, NODE_MAX - str_index, "%d ", reqval);
                str_index += chars_copied;
            }
            if(str_index > 0) {
                tmp_val[str_index-1] = '\0';
            }
            rc = change_node_val(d.sysfsnode_path[idx_clust], tmp_val, strlen(tmp_val));
        }
        return rc;
    }
    else {
        return modify_sysfsnode(r, d);
    }
}

int32_t OptsHandler::sched_util_thres_pct_clust(Resource &r, OptsData &d) {
    uint16_t idx = r.qindex;
    char *node_storage = NULL;
    int32_t *node_storage_length = NULL;

    if (!d.is_supported[idx]) {
        QLOGE(LOG_TAG, "Perflock resource %s not supported", d.sysfsnode_path[idx]);
        return FAILED;
    }

    if (ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT, d.node_type[idx]) == FAILED) {
        QLOGE(LOG_TAG, "Request on invalid core or cluster");
        return FAILED;
    }

    node_storage = d.sched_util_thres_pct_clust_s[r.cluster];
    node_storage_length = &d.sched_util_thres_pct_clust_sl[r.cluster];

    return multiValClustNodeFunc(r, d, node_storage, node_storage_length);
}
