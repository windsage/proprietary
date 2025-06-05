/******************************************************************************
  @file    OptsHandler.h
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_HANDLER__H_
#define __OPTS_HANDLER__H_

#include "ResourceInfo.h"
#include "OptsData.h"
#include "config.h"

enum req_action_type {
    ADD_NEW_REQUEST = 0,
    ADD_AND_UPDATE_REQUEST = 1,
    PEND_REQUEST = 2,
    ADD_IN_ORDER = 3,
    PEND_ADD_IN_ORDER = 4,
    EQUAL_ADD_IN_ORDER = 5
};

typedef int32_t (*ApplyOpts)(Resource &, OptsData &);
typedef int32_t (*ResetOpts)(Resource &, OptsData &);
typedef int32_t (*CompareOpts)(uint32_t, uint32_t);

typedef struct OptsTable {
    ApplyOpts mApplyOpts;
    ResetOpts mResetOpts;
    CompareOpts mCompareOpts;
} OptsTable;

typedef struct SchedPolicyTable {
    unsigned int tid;
    int policy;
    struct sched_param param;
} SchedPolicyTable;

class OptsHandler {
    private:
        OptsTable mOptsTable[MAX_MINOR_RESOURCES];
        void LoadSecureLibrary(bool onoff);

        //singleton
        static OptsHandler mOptsHandler;
        static std::vector<SchedPolicyTable> mSP;

        void *mLibHandle = nullptr;

    private:
        OptsHandler();
        OptsHandler(OptsHandler const& oh);
        OptsHandler& operator=(OptsHandler const& oh);

    private:
        static int32_t update_node_param(uint8_t opcode_type, const char node[NODE_MAX],
                                        char node_strg[NODE_MAX], uint32_t node_strg_l);
    private:
        //all minor resource actions
        static int32_t dummy(Resource &r, OptsData &d);

        /*a common function to handle all the generic perflock calls.*/
        static int32_t modify_sysfsnode(Resource &r, OptsData &d);

        /*set pmQos latency values*/
        static int32_t pmQoS_cpu_dma_latency(Resource &r, OptsData &d);

        /*cpu freq actions*/
        static int32_t cpu_options(Resource &r, OptsData &d);
        static int32_t set_scaling_min_freq(Resource &r, OptsData &d);

        /*sched actions*/
        static int32_t set_sched_boost(Resource &r, OptsData &d);
        static int32_t reset_sched_boost(Resource &r, OptsData &d);
        static int32_t sched_task_boost(Resource &r, OptsData &d);
        static int32_t sched_reset_task_boost(Resource &r, OptsData &d);
        static int32_t sched_enable_task_boost_renderthread(Resource &r, OptsData &d);
        static int32_t sched_util_busy_hyst_cpu_ns(Resource &r, OptsData &d);
        static int32_t sched_coloc_busy_hyst_cpu_ns(Resource &r, OptsData &d);
        static int32_t sched_coloc_busy_hyst_cpu_busy_pct(Resource &r, OptsData &d);
        static int32_t sched_low_latency(Resource &r, OptsData &d);
        static int32_t sched_reset_low_latency(Resource &r, OptsData &d);
        static int32_t sched_thread_pipeline(Resource &r, OptsData &d);
        static int32_t sched_reset_thread_pipeline(Resource &r, OptsData &d);
        static int32_t sched_task_load_boost(Resource &r, OptsData &d);
        static int32_t sched_reset_task_load_boost(Resource &r, OptsData &d);
        static int32_t sched_fmax_cap(Resource &r, OptsData &d);
        static int32_t sched_idle_enough_clust(Resource &r, OptsData &d);
        static int32_t sched_idle_enough_clust_system(Resource &r, OptsData &d);
        static int32_t sched_util_thres_pct_clust(Resource &r, OptsData &d);
        static int32_t sched_util_thres_pct_system(Resource &r, OptsData &d);

        /*corectl actions*/
        static int32_t lock_min_cores(Resource &r, OptsData &d);
        static int32_t lock_max_cores(Resource &r, OptsData &d);

        /*cpubw hwmon actions*/
        static int32_t cpubw_hwmon_hyst_opt(Resource &r, OptsData &d);
        static int32_t bus_dcvs_hyst_opt(Resource &r, OptsData &d);

        /*video hints actions*/
        static int32_t handle_disp_hint(Resource &r, OptsData &d);
        static int32_t handle_vid_decplay_hint(Resource &r, OptsData &d);
        static int32_t handle_vid_encplay_hint(Resource &r, OptsData &d);
        static int32_t handle_early_wakeup_hint(Resource &r, OptsData &d);

        /*ksm actions*/
        static int32_t disable_ksm(Resource &r, OptsData &d);
        static int32_t enable_ksm(Resource &r, OptsData &d);
        static int32_t set_ksm_param(Resource &r, OptsData &d);
        static int32_t reset_ksm_param(Resource &r, OptsData &d);

        /*gpu actions*/
        static int32_t gpu_maxfreq(Resource &r, OptsData &d);
        static int32_t gpu_disable_gpu_nap(Resource &r, OptsData &d);
        static int32_t gpu_is_app_fg(Resource &r, OptsData &d);
        static int32_t gpu_is_app_bg(Resource &r, OptsData &d);

        /*miscellaneous actions, irq*/
        static int32_t unsupported(Resource &r, OptsData &d);
        static int32_t irq_balancer(Resource &r, OptsData &d);
        static int32_t keep_alive(Resource &r, OptsData &d);
        static void* keep_alive_thread(void*);
        static int32_t set_pid_affine(Resource &r, OptsData &d);
        static int32_t reset_pid_affine(Resource &r, OptsData &d);
        static int32_t getThreadCgroup(int32_t tid, char *cgroup);
        static int32_t perfmode_entry_pasr(Resource &r, OptsData &d);
        static int32_t perfmode_exit_pasr(Resource &r, OptsData &d);
        static int32_t handle_fps_hyst(Resource &r, OptsData &d);
        static int32_t set_scheduler(Resource &r, OptsData &d);
        static int32_t reset_scheduler(Resource &r, OptsData &d);
        static int32_t set_display_heavy_rt_affine(Resource &r, OptsData &d);
        static int32_t reset_display_heavy_rt_affine(Resource &r, OptsData &d);
        static int32_t enable_gold_dynprefetcher(Resource &r, OptsData &d);
        static int32_t disable_gold_dynprefetcher(Resource &r, OptsData &d);

        /*llcbw hwmon actions*/
        static int32_t llcbw_hwmon_hyst_opt(Resource &r, OptsData &d);

        /*memlat (l3, l2) actions*/
        static int32_t l3_min_freq(Resource &r, OptsData &d);
        static int32_t l3_max_freq(Resource &r, OptsData &d);

        /* migrate (upmigrate, downmigrate) actions */
        static int32_t migrate(Resource &r, OptsData &d);
        static int32_t migrate_action(Resource &r, OptsData &d, int32_t action, int32_t flag, uint32_t reqval);
        static int32_t migrate_action_apply(Resource &r, OptsData &d);
        static int32_t migrate_action_release(Resource &r, OptsData &d);
        static int16_t read_curmigrate_val(const char *node_path, int8_t cluster);
        static int16_t value_percluster(char node_val[NODE_MAX], int8_t cluster);
        static int32_t grp_migrate_action_apply(Resource &r, OptsData &d);
        static int32_t grp_migrate_action_release(Resource &r, OptsData &d);

        /*compare functions*/
        static int32_t lower_is_better(uint32_t reqLevel, uint32_t curLevel);
        static int32_t higher_is_better(uint32_t reqLevel, uint32_t curLevel);
        static int32_t lower_is_better_negative(uint32_t reqLevel_s, uint32_t curLevel_s);
        static int32_t higher_is_better_negative(uint32_t reqLevel_s, uint32_t curLevel_s);
        static int32_t always_apply(uint32_t reqLevel, uint32_t curLevel);
        static int32_t add_in_order(uint32_t reqLevel, uint32_t curLevel);
        static int32_t add_in_order_fps_based_taskboost(uint32_t reqLevel, uint32_t curLevel);
        static int32_t migrate_lower_is_better(uint32_t reqLevel, uint32_t curLevel);

        /*Validating function for cluster and core*/
        static int32_t ValidateClusterAndCore(int8_t cluster, int8_t core, uint8_t resourceStatus,
                                                uint8_t nodeType);
        static int32_t CustomizeRequestValue(Resource &r, OptsData &d, char *tmp_s);
        static void CustomizePostNodeUpdation(Resource &r, OptsData &d, uint8_t perflock_call,
                                                char node_strg[NODE_MAX], uint32_t node_strg_l);
        static int32_t GetNodeStorageLink(Resource &r, OptsData &d, char **node_storage, int32_t **node_storage_length);
        /*miscellaneous utility functions*/
        static char *cpuset_bitmask_to_str(uint32_t cpuset_bitmask);
        static int32_t parse_mig_vals(uint32_t index, const uint32_t mig_val, char *acqval,
                                  const char *inbuf);
        static int32_t parseMultiValNode(const uint32_t mig_val, char *acqval,
                                  const char *inbuf, int8_t core_start, int8_t core_end);
        static int32_t multiValNodeFunc(Resource &r, OptsData &d, char *node_storage, int32_t *node_storage_length, int8_t core_start, int8_t core_end);
        static int32_t multiValClustNodeFunc(Resource &r, OptsData &d, char *node_storage, int32_t *node_storage_length);
        static int32_t parse_freq_vals(uint32_t index, const uint32_t freq_val, char *acqval,
                                  const char *inbuf);

        /*npu actions*/
        static int32_t npu_llcbw_hwmon_hyst_opt(Resource &r, OptsData &d);
        static int32_t npubw_hwmon_hyst_opt(Resource &r, OptsData &d);

        /* common function */
        static int32_t apply_value(Resource &r, OptsData &d);
        static int32_t reset_value(Resource &r, OptsData &d);

    public:
        ~OptsHandler();

        int32_t Init();
        void InitializeOptsTable();
        int32_t ApplyOpt(Resource &r);
        int32_t ResetOpt(Resource &r);
        int32_t CompareOpt(uint32_t qindx, uint32_t reqVal, uint32_t curVal);

        static OptsHandler &getInstance() {
            return mOptsHandler;
        }
};

#endif /*__OPTS_HANDLER__H_*/
