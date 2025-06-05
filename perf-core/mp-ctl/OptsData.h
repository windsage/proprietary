/******************************************************************************
  @file    OptsData.h
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_DATA__H_
#define __OPTS_DATA__H_

#include <cstdlib>
#include <fcntl.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <unordered_set>
#include <unordered_map>
#include "Target.h"
#include "MpctlUtils.h"
#include "PerfLog.h"
#include <utils/TypeHelpers.h>
#include <unistd.h>
#include <config.h>

#define SYSFS_PREFIX            "/sys/devices/system/"
#define DEVFREQ_PATH            "/sys/class/devfreq/"
#define CPUP                    "/sys/devices/system/cpu/present"

#define SCHED_FOREGROUND_BOOST "/dev/cpuset/foreground/boost/cpus"
/*SCHED_FREQ_AGGREGATE_NODE is being used in class sched_freq_aggr_group_data
to decide whether the node is present or not.*/
#define SCHED_FREQ_AGGREGATE_NODE  "/proc/sys/kernel/sched_freq_aggregate"

#define KSM_RUN_NODE  "/sys/kernel/mm/ksm/run"
#define KSM_SLEEP_MILLI_SECS_NODE "/sys/kernel/mm/ksm/sleep_millisecs"
#define KSM_PAGES_TO_SCAN_NODE "/sys/kernel/mm/ksm/pages_to_scan"

//used in target.cpp
#define GPU_AVAILABLE_FREQ  "/sys/class/kgsl/kgsl-3d0/devfreq/available_frequencies"
#define GPU_BUS_AVAILABLE_FREQ  "/sys/class/devfreq/soc:qcom,gpubw/available_frequencies"

//following 4 are updated in same call.
#define GPU_FORCE_RAIL_ON   "/sys/class/kgsl/kgsl-3d0/force_rail_on"
#define GPU_FORCE_CLK_ON    "/sys/class/kgsl/kgsl-3d0/force_clk_on"
#define GPU_IDLE_TIMER      "/sys/class/kgsl/kgsl-3d0/idle_timer"
#define GPU_FORCE_NO_NAP    "/sys/class/kgsl/kgsl-3d0/force_no_nap"

//lock min cores and lock max cores
#define CORE_CTL_MIN_CPU        (SYSFS_PREFIX"cpu/cpu%" PRId32 "/core_ctl/min_cpus")
#define CORE_CTL_MAX_CPU        (SYSFS_PREFIX"cpu/cpu%" PRId32 "/core_ctl/max_cpus")

#define AVL_FREQ_NODE          (SYSFS_PREFIX"cpu/cpu%" PRId32 "/cpufreq/scaling_available_frequencies")
#define CPUINFO_FREQ_NODE      (SYSFS_PREFIX"cpu/cpu%" PRId32 "/cpufreq/cpuinfo_%s_freq")
#define STORAGE_EMMC_CLK_SCALING_DISABLE "/sys/class/mmc_host/mmc0/clk_scaling/enable"
#define STORAGE_UFS_CLK_SCALING_DISABLE "/sys/class/scsi_host/host0/../../../clkscale_enable"
#define KERNEL_VERSION_NODE  "/proc/sys/kernel/osrelease"
#define FOREGROUND_TASK_NODE "/dev/cpuset/foreground/cgroup.procs"
#define SYSBG_TASK_NODE "/dev/cpuset/system-background/cgroup.procs"
#define THREAD_CGROUP_NODE ("/proc/%" PRId32 "/cpuset")
#define CPUSET_NODE ("/dev/cpuset%s/cgroup.procs")

/*TODO Check its usage*/
#define c0f   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"                    // cpu0 available freq

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define MAX_CPUS            8

#define MAX_FREQ_CPUBW      50
#define MAX_FREQ_LLCBW      50
#define MAX_FREQ_L3L        50
#define MAX_FREQ_L3B        50
#define FREQLIST_MAX            32
#define FREQLIST_STR            1024
#define ENABLE_PC_LATENCY   0

#define CPU_INDEX 27  //cpu index in all sysfs node paths related to managing cores

#define MAX_FREQ            50
#define FREQ_MULTIPLICATION_FACTOR  1000ul
#define TIMER_MULTIPLICATION_FACTOR 1000ul

#define FREAD_STR(fn, pstr, len, rc)    { int fd;                       \
                                          rc = -1;                      \
                                          fd = open(fn, O_RDONLY);      \
                                          if (fd >= 0) {                \
                                              rc = read(fd, pstr, len); \
                                              pstr[len-1] = '\0';       \
                                              close(fd);                \
                                          }                             \
                                        }

#define FWRITE_STR(fn, pstr, len, rc)   { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_WRONLY);       \
                                          if (fd >= 0) {                 \
                                              rc = write(fd, pstr, len); \
                                              close(fd);                 \
                                          }                              \
                                        }
#define FREAD_BUF_STR(fn, pstr, len, rc) { FILE *fd;                                    \
                                          rc = -1;                                      \
                                          fd = fopen(fn, "r");                          \
                                          if (fd != NULL) {                             \
                                              rc = fread(pstr, sizeof(char), len, fd);  \
                                              pstr[len-1] = '\0';                       \
                                              fclose(fd);                               \
                                          }                                             \
                                        }
#define FWRITE_BUF_STR(fn, pstr, len, rc) { FILE *fd;                                   \
                                          rc = -1;                                      \
                                          fd = fopen(fn, "w");                          \
                                          if (fd != NULL) {                             \
                                              rc = fwrite(pstr, sizeof(char), len, fd); \
                                              fclose(fd);                               \
                                          }                                             \
                                        }

static const char *dnsIPs[] = {
    "8.8.8.8",/* Google */
    "8.8.4.4",/* Google */
    "114.114.114.114", /* 114DNS */
    "114.114.115.115", /* 114DNS */
    "1.2.4.8", /* CNNIC SDNS */
    "210.2.4.8", /* CNNIC SDNS */
    "223.5.5.5", /*  Ali DNS */
    "223.6.6.6", /* Ali DNS */
    "216.146.35.35", /* Dyn DNS */
    "216.146.36.36", /* Dyn DNS */
    "208.67.222.123", /* OpenDNS */
    "208.67.220.123" /* OpenDNS */
};

struct cpu_freq_mapping {
    const char * min;
    const char * max;
    const char * online;
    int in_use;
};

struct cpu_freq_resource_value {
    int32_t avl_freq[MAX_FREQ];
    int32_t count;
    int32_t min_freq_pos;
    uint8_t valid;
};

typedef struct hint_associated_data {
    struct sigevent mSigEvent;
    struct itimerspec mInterval;
    timer_t tTimerId;
    int disp_single_layer;
    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;
    int timer_created;
}hint_associated_data;


class OptsData {
private:
    static OptsData mOptsData;

    friend class OptsHandler;
    friend class ResetHandler;

private:
    OptsData();
    OptsData(OptsData const& d);
    OptsData& operator=(OptsData const& d);

private:

    char cpubw_path[NODE_MAX];
    char cpubw_maxfreq_path[NODE_MAX];
    char cpubw_hwmon_hist_memory_path[NODE_MAX];
    char cpubw_hwmon_hyst_length_path[NODE_MAX];
    char cpubw_hwmon_hyst_count_path[NODE_MAX];
    uint32_t  cpubw_avail_freqs[MAX_FREQ_CPUBW];
    uint32_t  cpubw_avail_freqs_n;
    char cpubw_hm_s[NODE_MAX];             // cpubw hist memory storage
    int  cpubw_hm_sl;                      // cpubw hist memory string length
    char cpubw_hl_s[NODE_MAX];             // cpubw hyst length storage
    int  cpubw_hl_sl;                      // cpubw hyst length string length
    char cpubw_hc_s[NODE_MAX];             // cpubw hyst count storage
    int  cpubw_hc_sl;                      // cpubw hyst count string length


    uint32_t avail_freqs[MAX_FREQ];
    uint16_t avail_freqs_n;

    char npubw_path[NODE_MAX];
    char npubw_hwmon_hist_memory_path[NODE_MAX];
    char npubw_hwmon_hyst_length_path[NODE_MAX];
    char npubw_hwmon_hyst_count_path[NODE_MAX];
    char npubw_hm_s[NODE_MAX];             // npubw hist memory storage
    int  npubw_hm_sl;                      // npubw hist memory string length
    char npubw_hl_s[NODE_MAX];             // npubw hyst length storage
    int  npubw_hl_sl;                      // npubw hyst length string length
    char npubw_hc_s[NODE_MAX];             // npubw hyst count storage
    int  npubw_hc_sl;                      // npubw hyst count string length

    char npu_llcbw_path[NODE_MAX];
    char npu_llcbw_hwmon_hist_memory_path[NODE_MAX];
    char npu_llcbw_hwmon_hyst_length_path[NODE_MAX];
    char npu_llcbw_hwmon_hyst_count_path[NODE_MAX];
    char npu_llcbw_hm_s[NODE_MAX];             // npu_llcbw hist memory storage
    int  npu_llcbw_hm_sl;                      // npu_llcbw hist memory string length
    char npu_llcbw_hl_s[NODE_MAX];             // npu_llcbw hyst length storage
    int  npu_llcbw_hl_sl;                      // npu_llcbw hyst length string length
    char npu_llcbw_hc_s[NODE_MAX];             // npu_llcbw hyst count storage
    int  npu_llcbw_hc_sl;                      // npu_llcbw hyst count string length

    char llcbw_path[NODE_MAX];
    char llcbw_maxfreq_path[NODE_MAX];
    char llcbw_hwmon_hist_memory_path[NODE_MAX];
    char llcbw_hwmon_hyst_length_path[NODE_MAX];
    char llcbw_hwmon_hyst_count_path[NODE_MAX];
    uint32_t  llcbw_avail_freqs[MAX_FREQ_LLCBW];
    uint32_t  llcbw_avail_freqs_n;
    char llcbw_hm_s[NODE_MAX];             // llcbw hist memory storage
    int  llcbw_hm_sl;                      // llcbw hist memory string length
    char llcbw_hl_s[NODE_MAX];             // llcbw hyst length storage
    int  llcbw_hl_sl;                      // llcbw hyst length string length
    char llcbw_hc_s[NODE_MAX];             // llcbw hyst count storage
    int  llcbw_hc_sl;                      // llcbw hyst count string length

    char l3_path[MAX_CLUSTER][NODE_MAX];
    char l3_minfreq_path[MAX_CLUSTER][NODE_MAX];
    char l3_maxfreq_path[MAX_CLUSTER][NODE_MAX];
    uint32_t  l3_avail_freqs[MAX_CLUSTER][MAX_FREQ_L3L];
    uint32_t  l3_avail_freqs_n[MAX_CLUSTER];
    char l3_minf_s[MAX_CLUSTER][NODE_MAX];             // l3 little min freq storage
    int  l3_minf_sl[MAX_CLUSTER];                      // l3 little min freq string length
    int  l3_maxf_sl[MAX_CLUSTER];                      // l3 little max freq string length
    char l3_maxf_s[MAX_CLUSTER][NODE_MAX];             // l3 little max freq storage

    char schedb_n[NODE_MAX];                  // sched boost node string

    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;

    char ksm_run_node[NODE_MAX];
    char ksm_param_sleeptime[NODE_MAX];
    char ksm_param_pages_to_scan[NODE_MAX];
    char ksm_sleep_millisecs[PROPERTY_VALUE_MAX];
    char ksm_pages_to_scan[PROPERTY_VALUE_MAX];
    int32_t is_ksm_supported;

    unsigned int  c0fL[FREQLIST_MAX];                // cpu0 list of available freq
    char c0fL_s[FREQLIST_STR];              // cpu0 list of available freq
    uint32_t  c0fL_n;                            // cpu0 #freq
    int  c4fL[FREQLIST_MAX];                // cpu4 list of available freq
    char c4fL_s[FREQLIST_STR];              // cpu4 list of available freq string
    int  c4fL_n;                            // cpu4 #freq

    int32_t kpm_hotplug_support; //1 represent hotplug is supported through KPM, 0 hotplug not supported, -1 not initialized
    int32_t lock_max_clust0;     //store cluster0 value for lock max core perflock, through KPM
    int32_t lock_max_clust1;     //store cluster1 value for lock max core perflock, through KPM

    char core_ctl_min_cpu_node[NODE_MAX];   //Actual path of min_cpu node based on target
    char core_ctl_max_cpu_node[NODE_MAX];   //Actual path of max_cpu node based on target

    int32_t core_ctl_present;    //1 represent core_ctl is present, 0 core ctl not present, -1 not initialized

    char core_ctl_min_s[MAX_CLUSTER][NODE_MAX];     // core control min_cpu storage
    char core_ctl_max_s[MAX_CLUSTER][NODE_MAX];     // core control max_cpu storage
    int core_ctl_min_sl[MAX_CLUSTER];               // core control min_cpu storage length
    int core_ctl_max_sl[MAX_CLUSTER];               // core control max_cpu storage legth

    int32_t min_cores[MAX_CLUSTER];
    int32_t max_cores[MAX_CLUSTER];

    /* This array will be used to store max/min frequency
     * for each cpu.
     */
    struct cpu_freq_resource_value cpu_freq_val[MAX_CPUS];

    bool min_freq_prop_0_set;
    bool min_freq_prop_4_set;
    int32_t  min_freq_prop_0_val;
    int32_t  min_freq_prop_4_val;

    //hint data
    hint_associated_data mHintData;

    //irq data
    int  *irq_bal_sp; // irq balancer storage pointer
    int32_t stored_irq_balancer;

    //Common variables for all resources.
    char sysfsnode_path[MAX_MINOR_RESOURCES][NODE_MAX];
    bool is_supported[MAX_MINOR_RESOURCES];
    int32_t node_type[MAX_MINOR_RESOURCES];
    char sysfsnode_storage[MAX_MINOR_RESOURCES][NODE_MAX];
    int32_t sysfsnode_storage_length[MAX_MINOR_RESOURCES];

    //Interactive node storages for both clusters
    int32_t cluster0_interactive_node_storage_length[MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster0_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    int32_t cluster1_interactive_node_storage_length[ MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster1_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    int32_t cluster2_interactive_node_storage_length[ MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster2_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    int32_t cluster3_interactive_node_storage_length[ MAX_INTERACTIVE_MINOR_OPCODE];
    char cluster3_interactive_node_storage[MAX_INTERACTIVE_MINOR_OPCODE][NODE_MAX];

    /*MEM_LAT Storage Variables*/
    int32_t memlat_minfreq_node_strg_len[MAX_CLUSTER][MAX_MEMLAT_MINOR_OPCODE];
    char memlat_minfreq_node_strg[MAX_CLUSTER][MAX_MEMLAT_MINOR_OPCODE][NODE_MAX];

    //Storage paths for all cpus of resource SCHED_MOSTLY_IDLE_FREQ_OPCODE
    char sch_cpu_pwr_cost_s[MAX_CPUS][NODE_MAX];
    int32_t  sch_cpu_pwr_cost_sl[MAX_CPUS];

    char sch_upmigrate_s[MAX_CLUSTER][NODE_MAX];
    int32_t sch_upmigrate_sl[MAX_CLUSTER];
    char sch_downmigrate_s[MAX_CLUSTER][NODE_MAX];
    int32_t sch_downmigrate_sl[MAX_CLUSTER];
    uint16_t upmigrate_val[MAX_CLUSTER] = {0};
    uint16_t downmigrate_val[MAX_CLUSTER] = {0};
    char sched_upmigrate[NODE_MAX] = "";
    char sched_downmigrate[NODE_MAX] = "";

    char grp_upmigrate_s[NODE_MAX];
    int grp_upmigrate_sl;
    char grp_downmigrate_s[NODE_MAX];
    int grp_downmigrate_sl;
    char sched_grp_upmigrate[NODE_MAX] = "";
    char sched_grp_downmigrate[NODE_MAX] = "";

    char sch_load_boost_s[MAX_CPUS][NODE_MAX];
    int32_t  sch_load_boost_sl[MAX_CPUS];

    char core_ctl_enable_s[MAX_CLUSTER][NODE_MAX];
    int32_t  core_ctl_enable_sl[MAX_CLUSTER];

    char core_ctl_cpu_not_preferred_s[MAX_CLUSTER][NODE_MAX];
    int32_t core_ctl_cpu_not_preferred_sl[MAX_CLUSTER];

    char adaptive_low_freq_s[MAX_CLUSTER][NODE_MAX];
    int32_t  adaptive_low_freq_sl[MAX_CLUSTER];

    char adaptive_high_freq_s[MAX_CLUSTER][NODE_MAX];
    int32_t  adaptive_high_freq_sl[MAX_CLUSTER];

    char core_ctl_offline_delay_ms_s[MAX_CLUSTER][NODE_MAX];
    int32_t  core_ctl_offline_delay_ms_sl[MAX_CLUSTER];

    char core_ctl_partial_cpus_s[MAX_CLUSTER][NODE_MAX];
    int32_t  core_ctl_partial_cpus_sl[MAX_CLUSTER];

    char sch_util_busy_hyst_cpy_ns_s[MAX_CLUSTER][NODE_MAX];
    int32_t  sch_util_busy_hyst_cpy_ns_sl[MAX_CLUSTER];

    char sch_coloc_busy_hyst_cpu_busy_pct_s[MAX_CLUSTER][NODE_MAX];
    int32_t  sch_coloc_busy_hyst_cpu_busy_pct_sl[MAX_CLUSTER];

    char sch_coloc_busy_hyst_cpu_ns_s[MAX_CLUSTER][NODE_MAX];
    int32_t sch_coloc_busy_hyst_cpu_ns_sl[MAX_CLUSTER];

    int earlyWakeupDispId;
    float fps_hyst_time_sec = -1.0f;

    char sch_fmax_cap_s[MAX_CLUSTER][NODE_MAX];
    int32_t sch_fmax_cap_sl[MAX_CLUSTER];

    char sched_idle_enough_s[MAX_CLUSTER][NODE_MAX];
    int32_t sched_idle_enough_sl[MAX_CLUSTER];

    char sched_util_thres_pct_clust_s[MAX_CLUSTER][NODE_MAX];
    int32_t sched_util_thres_pct_clust_sl[MAX_CLUSTER];

    unordered_map<uint32_t, uint32_t> postboot_scaling_minfreq;

public:
    int8_t init_available_cpubw_freq();
    int32_t find_next_cpubw_available_freq(uint32_t freq);
    int32_t find_next_llccbw_available_freq(uint32_t freq);
    int8_t init_cpubw_hwmon_path();

    int8_t init_llcbw_hwmon_path();

    int8_t init_npubw_hwmon_path();
    int8_t init_npu_llcbw_hwmon_path();
    /*Functions to handle new Opcode for llccbw and cpubw*/
    int8_t init_llcbw_hwmon_path_newOpcode();
    int8_t init_cpubw_hwmon_path_newOpcode();

    void init_node_paths();
    void get_nodes_folder_path(const char *node_path, char *folder_path);
    int8_t init_l3_path();

    int8_t init_devfreq_freqlist(char*);
    int8_t init_devfreq_child_path(const char*, char *);

    int vid_create_timer();

    int8_t init_ksm();
    int8_t toggle_ksm_run(uint8_t run);

    void parse_avail_freq_list();
    void initPostbootScalingMinFreq();
    unsigned int find_next_avail_freq(unsigned int freq);
    uint8_t kpm_support_for_hotplug();

    int8_t core_ctl_init();
    uint8_t check_core_ctl_presence();

    int32_t setup_cpu_freq_values();

    int8_t get_reset_cpu_freq(int8_t cpu, uint8_t ftype);
    int32_t find_next_cpu_frequency(int8_t cpu, uint32_t freq);
    int32_t init_available_freq(int8_t cpu);
    void check_min_freq_prop_set(int8_t cpu);
    int32_t find_frequency_pos(int8_t cpu, uint32_t freq);
    uint32_t getEarlyWakeupDispId() { return earlyWakeupDispId; }
    void setEarlyWakeupDispId(int dId) { earlyWakeupDispId = dId; }
    float get_fps_hyst_time();
    void set_fps_hyst_time(float);
    int8_t get_node_type(int32_t qIndex);
    int32_t hwcPid = 0, sfPid = 0, reTid = 0;
    int32_t appPid = 0, renderTid = 0;
    char sfCgroup[NODE_MAX] = "";
    char hwcCgroup[NODE_MAX] = "";

public:
    ~OptsData();

    int32_t Init();

    static OptsData &getInstance() {
        return mOptsData;
    }

};

#endif /*__OPTS_DATA__H_*/
