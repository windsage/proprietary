/******************************************************************************
  @file    RestoreHandler.cpp
  @brief   Implementation of nodes restoration

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-RESETHANDLER"
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <inttypes.h>
#include "Request.h"

#define RESET_SCHED_BOOST   0
#define MAX_VALUE_USHORT   255

#include "OptsData.h"
#include "RestoreHandler.h"
#include "MpctlUtils.h"
#include "PerfController.h"
#include "BoostConfigReader.h"
#include "PerfLog.h"

#define DEFAULT_VALUES_FILE "/data/vendor/perfd/default_values"

//strings for storing and identifying the sysfs node values in the default values file.
#define INDEX_ONLY "index_%" PRId32
#define INDEX_WITH_CORE "index_%" PRId32 "_core_%" PRId32
#define INDEX_WITH_CLUSTER "index_%" PRId32 "_cluster_%" PRId32
#define CORE_CTL_MIN "core_ctl_min_cluster_%" PRId32
#define CORE_CTL_MAX "core_ctl_max_cluster_%" PRId32

ResetHandler ResetHandler::mResetHandler;

ResetHandler::ResetHandler() {
}

ResetHandler::~ResetHandler() {
}

int8_t ResetHandler::Init() {
    return 0;
}

void ResetHandler::reset_to_default_values(OptsData &d) {
    FILE *defval;
    uint16_t idx;
    int8_t i, j;
    int32_t rc;
    int8_t cpu;
    char buf[NODE_MAX], val_str[NODE_MAX], buf2[NODE_MAX], tmp[NODE_MAX];
    char tmp_s[NODE_MAX];

    TargetConfig &tc = TargetConfig::getTargetConfig();
    Target &t = Target::getCurTarget();
    int8_t clusterNum, startCpu, endCpu;

    clusterNum = tc.getNumCluster();
    defval = fopen(DEFAULT_VALUES_FILE, "a+");
    if (defval == NULL) {
        QLOGE(LOG_TAG, "Cannot open/create default values file");
        return;
    }

    fseek (defval, 0, SEEK_END);

    if (ftell(defval) == 0) {
        //All the special node types and resource with multiple node updations are hard coded.
        write_to_file(defval, "ksm_run_node", d.ksm_run_node);
        write_to_file(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_file(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_file(defval, "gpu_force_rail_on", GPU_FORCE_RAIL_ON);
        write_to_file(defval, "gpu_force_clk_on", GPU_FORCE_CLK_ON);
        write_to_file(defval, "gpu_idle_timer", GPU_IDLE_TIMER);
        write_to_file(defval, "gpu_force_no_nap", GPU_FORCE_NO_NAP);
        write_to_file(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_file(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_file(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_file(defval, "llchistmem", d.llcbw_hwmon_hist_memory_path);
        write_to_file(defval, "llchystlen", d.llcbw_hwmon_hyst_length_path);
        write_to_file(defval, "llchystcnt", d.llcbw_hwmon_hyst_count_path);
        write_to_file(defval, "foreground_boost", SCHED_FOREGROUND_BOOST);
        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            snprintf(tmp, NODE_MAX, "l3Cluster%" PRId8 "minf", j);
            write_to_file(defval, tmp, d.l3_minfreq_path[j]);
            snprintf(tmp, NODE_MAX, "l3Cluster%" PRId8 "maxf", j);
            write_to_file(defval, tmp, d.l3_maxfreq_path[j]);
        }

        for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
            if (!d.is_supported[idx])
                continue;

            /*Restore data for scaling_min_freq nodes are read from postboot settings
            and stored at OptsData::postboot_scaling_minfreq
            Hence, skip for scaling_min_freq opcode.
            */
            if (idx == (CPUFREQ_START_INDEX + SCALING_MIN_FREQ_OPCODE)) {
                continue;
            }

            strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
            switch (d.node_type[idx]) {
            case INTERACTIVE_NODE:
            /*For cluster based all the interactive nodes of same cluster have same values.
              So, we only store the values of first online cpu of each cluster in the format
              "index_resourceId_with_clusterId". For single gov instance type, we can store the
              value with format "index_resourceId" */
                if (tc.getGovInstanceType() == CLUSTER_BASED_GOV_INSTANCE) {
                    for (i = 0, startCpu = 0; i < clusterNum; i++) {
                        cpu = get_online_core(startCpu, t.getLastCoreIndex(i));
                        buf[CPU_INDEX] = cpu + '0';
                        snprintf(tmp, NODE_MAX, INDEX_WITH_CLUSTER, idx, i);
                        QLOGL(LOG_TAG, QLOG_L2, "writing from node %s to default file as : %s", buf, tmp);
                        write_to_file(defval, tmp, buf);
                        startCpu = t.getLastCoreIndex(i)+1;
                    }
                } else {
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    QLOGL(LOG_TAG, QLOG_L2, "writing from node %s to default file as : %s", buf, tmp);
                    write_to_file(defval, tmp, buf);
                }
                break;

            case SELECT_CORE_TO_UPDATE:
            /*For this node_type we store the values of all the cores in format
              "Index_resourceId_with_coreId", as we can have individual perflock calls
              for all the cores.*/
                for (i = 0; i < tc.getTotalNumCores(); i++) {
                    int8_t VAR_CPU_INDEX = ((string)buf).find_first_of("0123456789");
                    if ((i + '0') > MAX_VALUE_USHORT)
                        buf[VAR_CPU_INDEX] = MAX_VALUE_USHORT;
                    else
                        buf[VAR_CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_WITH_CORE, idx, i);
                    QLOGL(LOG_TAG, QLOG_L2, "writing from node %s to default file as : %s", buf, tmp);
                    write_to_file(defval, tmp, buf);
                }
                break;

            case SPECIAL_NODE:
            /*All the nodes of special type need to be written seperately, as they
              might have multitple sysfs nodes to store.*/
                break;

            default:
            /*All the nodes of type single, update_all_cores and update_cores_per_cluster
              can be stored with format "index_resourceId".*/
                snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                QLOGL(LOG_TAG, QLOG_L2, "writing from node %s to default file as : %s", buf, tmp);
                write_to_file(defval, tmp, buf);
                break;
            }
        }

        for (uint16_t i = 0; i < MAX_CLUSTER; i++) {
            snprintf(d.core_ctl_min_cpu_node, NODE_MAX, CORE_CTL_MIN_CPU, t.getFirstCoreIndex(i));
            FREAD_STR(d.core_ctl_min_cpu_node, buf2, NODE_MAX, rc);
            if (rc > 0) {
                snprintf(tmp, NODE_MAX, CORE_CTL_MIN, i);
                write_to_file(defval, tmp, d.core_ctl_min_cpu_node);
            }
            snprintf(d.core_ctl_max_cpu_node, NODE_MAX, CORE_CTL_MAX_CPU, t.getFirstCoreIndex(i));
            FREAD_STR(d.core_ctl_max_cpu_node, buf2, NODE_MAX, rc);
            if (rc > 0) {
                snprintf(tmp, NODE_MAX, CORE_CTL_MAX, i);
                write_to_file(defval, tmp, d.core_ctl_max_cpu_node);
            }
        }
        /* sched_upmigrate is type: SPECIAL_NODE */
        snprintf(tmp, NODE_MAX, INDEX_ONLY, SCHED_START_INDEX +
                 SCHED_UPMIGRATE_OPCODE);
        write_to_file(defval, tmp, d.sysfsnode_path[SCHED_START_INDEX +
                      SCHED_UPMIGRATE_OPCODE]);

        /* sched_downmigrate is type: SPECIAL_NODE */
        snprintf(tmp, NODE_MAX, INDEX_ONLY, SCHED_START_INDEX +
                 SCHED_DOWNMIGRATE_OPCODE);
        write_to_file(defval, tmp, d.sysfsnode_path[SCHED_START_INDEX +
                      SCHED_DOWNMIGRATE_OPCODE]);

        fwrite("File created", sizeof(char), strlen("File created"), defval);
   } else {
        reset_freq_to_default(d);

        //Even while restoring, all the special node types are hard coded.
        write_to_node(defval, "ksm_run_node", d.ksm_run_node);
        write_to_node(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_node(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_node(defval, "gpu_force_rail_on", GPU_FORCE_RAIL_ON);
        write_to_node(defval, "gpu_force_clk_on", GPU_FORCE_CLK_ON);
        write_to_node(defval, "gpu_idle_timer", GPU_IDLE_TIMER);
        write_to_node(defval, "gpu_force_no_nap", GPU_FORCE_NO_NAP);
        write_to_node(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_node(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_node(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_node(defval, "llchistmem", d.llcbw_hwmon_hist_memory_path);
        write_to_node(defval, "llchystlen", d.llcbw_hwmon_hyst_length_path);
        write_to_node(defval, "llchystcnt", d.llcbw_hwmon_hyst_count_path);
        for(int8_t j = 0; j < tc.getNumCluster(); j++) {
            snprintf(tmp, NODE_MAX, "l3Cluster%" PRId8"minf", j);
            write_to_node(defval, tmp, d.l3_minfreq_path[j]);
            snprintf(tmp, NODE_MAX, "l3Cluster%" PRId8"maxf", j);
            write_to_node(defval, tmp, d.l3_maxfreq_path[j]);
        }

        if (d.core_ctl_present > 0) {
            for (uint16_t i =0; i < MAX_CLUSTER; i++) {
                snprintf(d.core_ctl_max_cpu_node, NODE_MAX, CORE_CTL_MAX_CPU, t.getFirstCoreIndex(i));
                FREAD_STR(d.core_ctl_max_cpu_node, buf2, NODE_MAX, rc);
                if (rc > 0) {
                    snprintf(tmp, NODE_MAX, CORE_CTL_MAX, i);
                    QLOGL(LOG_TAG, QLOG_L2, "Updating %s with %s", d.core_ctl_max_cpu_node, tmp);
                    write_to_node(defval, tmp, d.core_ctl_max_cpu_node);
                }
                snprintf(d.core_ctl_min_cpu_node, NODE_MAX, CORE_CTL_MIN_CPU, t.getFirstCoreIndex(i));
                FREAD_STR(d.core_ctl_min_cpu_node, buf2, NODE_MAX, rc);
                if (rc > 0) {
                    snprintf(tmp, NODE_MAX, CORE_CTL_MIN, i);
                    QLOGL(LOG_TAG, QLOG_L2, "Updating %s with %s", d.core_ctl_min_cpu_node, tmp);
                    write_to_node(defval, tmp, d.core_ctl_min_cpu_node);
                }
            }
        }
        else if (d.kpm_hotplug_support > 0){
            char kpmSysNode[NODE_MAX];
            snprintf(tmp_s, NODE_MAX, "%" PRId8 ":%" PRId8, tc.getCoresInCluster(0), tc.getCoresInCluster(1));
            memset(kpmSysNode, 0, sizeof(kpmSysNode));
            PerfDataStore *store = PerfDataStore::getPerfDataStore();
            store->GetSysNode(KPM_MAX_CPUS, kpmSysNode);
            FWRITE_STR(kpmSysNode, tmp_s, strlen(tmp_s), rc);
            if (rc > 0) {
                QLOGE(LOG_TAG, "Reset cores success");
            } else {
                //QLOGE(LOG_TAG, "Could not update the %s node \n", KPM_MANAGED_CPUS);
            }
        }
        for (idx = 0; idx < MAX_MINOR_RESOURCES; idx++) {
            if (!d.is_supported[idx])
                continue;

            if (idx == (CPUFREQ_START_INDEX + SCALING_MIN_FREQ_OPCODE)) {
                reset_scaling_min_freq(d);
                continue;
            }
            strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
            switch (d.node_type[idx]) {
            case UPDATE_ALL_CORES:
            case UPDATE_CORES_PER_CLUSTER:
            /*For these node types, when storing it is enough to store only the values
              of first cpu. But when restoring it is required to update all the cores,
              as these node_types modify all the cores for an acquire call. */
                for (i = 0; i < tc.getTotalNumCores(); i++) {
                    if ((i + '0') > MAX_VALUE_USHORT)
                        buf[CPU_INDEX] = MAX_VALUE_USHORT;
                    else
                        buf[CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    QLOGL(LOG_TAG, QLOG_L2, "writing from default file as : %s to node %s", tmp, buf);
                    write_to_node(defval, tmp, buf);
                }
                break;

            case SELECT_CORE_TO_UPDATE:
            /*For this node type the same way as storing we restore for all the cores.*/
                for (i = 0; i < tc.getTotalNumCores(); i++) {
                    uint8_t VAR_CPU_INDEX = ((string)buf).find_first_of("0123456789");
                    if ((i + '0') > MAX_VALUE_USHORT)
                        buf[VAR_CPU_INDEX] = MAX_VALUE_USHORT;
                    else
                        buf[VAR_CPU_INDEX] = i + '0';
                    snprintf(tmp, NODE_MAX, INDEX_WITH_CORE, idx, i);
                    QLOGL(LOG_TAG, QLOG_L2, "writing from default file as : %s to node %s", tmp, buf);
                    write_to_node(defval, tmp, buf);
                }
                break;

            case MEM_LAT_NODE:
            case SINGLE_NODE:
                snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                QLOGL(LOG_TAG, QLOG_L2, "writing from default file as : %s to node %s", tmp, buf);
                write_to_node(defval, tmp, buf);
                break;

            default:
            /*All the interactive and special node types are restored separately.*/
                break;
            }
        }

        //restore foreground/boost/cpus after restoring foreground/cpus
        write_to_node(defval, "foreground_boost", SCHED_FOREGROUND_BOOST);

        /* resotring of interactive nodes done differently by calling the function reset_cpu_nodes,
        inorder to retain the previous method.*/
        if (CLUSTER_BASED_GOV_INSTANCE == tc.getGovInstanceType()) {
            startCpu = 0; //core 0 cluster 0
            for (i = 0; i < clusterNum; i++) {
                if ((cpu = get_online_core(startCpu, t.getLastCoreIndex(i))) != FAILED) {
                    reset_cpu_nodes(cpu);
                }
                startCpu = t.getLastCoreIndex(i)+1;
            }
        } else {
            for (idx = INTERACTIVE_START_INDEX; idx < CPUBW_HWMON_START_INDEX; ++idx) {
                if (d.is_supported[idx]) {
                    snprintf(tmp, NODE_MAX, INDEX_ONLY, idx);
                    write_to_node(defval, tmp, d.sysfsnode_path[idx]);
                }
            }
        }

        /* sched_upmigrate is type: SPECIAL_NODE */
        snprintf(tmp, NODE_MAX, INDEX_ONLY, SCHED_START_INDEX +
                 SCHED_UPMIGRATE_OPCODE);
        write_to_node(defval, tmp, d.sysfsnode_path[SCHED_START_INDEX +
                      SCHED_UPMIGRATE_OPCODE]);

        /* sched_downmigrate is type: SPECIAL_NODE */
        snprintf(tmp, NODE_MAX, INDEX_ONLY, SCHED_START_INDEX +
                 SCHED_DOWNMIGRATE_OPCODE);
        write_to_node(defval, tmp, d.sysfsnode_path[SCHED_START_INDEX +
                      SCHED_DOWNMIGRATE_OPCODE]);

        reset_gpu_max_freq(d);
        reset_sched_boost(d);
    }
    fclose(defval);
}

void ResetHandler::reset_freq_to_default(OptsData &d) {
    int8_t i = 0, prevcores = 0;
    int8_t rc = -1;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    Target &t = Target::getCurTarget();
    uint32_t restoreVal = 0;
    char tmp_s[NODE_MAX] = "";

    for (i=0, prevcores = 0; i < tc.getNumCluster(); i++) {
        rc = update_freq_node(prevcores, t.getLastCoreIndex(i) , 0, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGL(LOG_TAG, QLOG_L2, "reset_freq_to_default reset min freq req for CPUs %" PRId8 "-%" PRId8 ": %s", prevcores, t.getLastCoreIndex(i), tmp_s);
            FWRITE_STR(d.sysfsnode_path[CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE], tmp_s, strlen(tmp_s), rc);
        }

        restoreVal = tc.getCpuMaxFreqResetVal(i);
        rc = update_freq_node(prevcores, t.getLastCoreIndex(i), restoreVal, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGL(LOG_TAG, QLOG_L2, "reset_freq_to_default reset max freq req for CPUs %" PRId8 "-%" PRId8 ": %s", prevcores, t.getLastCoreIndex(i), tmp_s);
            FWRITE_STR(d.sysfsnode_path[CPUFREQ_START_INDEX + CPUFREQ_MAX_FREQ_OPCODE], tmp_s, strlen(tmp_s), rc);
        }
        prevcores = t.getLastCoreIndex(i) + 1;
    }
}

void ResetHandler::reset_scaling_min_freq(OptsData &d) {

    char buf[NODE_MAX];
    int8_t rc = -1;
    char sysfspath[NODE_MAX] = "";
    int8_t cpuid_idx = -1;
    int32_t cpuid = 0, cpufreq = 0;

    strlcpy(sysfspath, d.sysfsnode_path[CPUFREQ_START_INDEX + SCALING_MIN_FREQ_OPCODE], NODE_MAX);
    cpuid_idx = static_cast<string>(sysfspath).find_first_of("0123456789");

    for (auto s: d.postboot_scaling_minfreq) {
        cpuid = s.first;
        cpufreq = s.second;
        snprintf(buf, NODE_MAX, "%" PRId32, cpufreq);
        sysfspath[cpuid_idx] = cpuid + '0';
        QLOGL(LOG_TAG, QLOG_L2, "Restoring node:%s with value:%" PRId32, sysfspath, cpufreq);
        FWRITE_STR(sysfspath, buf, strlen(buf), rc);
        if (rc < 0) {
            QLOGE(LOG_TAG, "Could not restore value to node: %s", sysfspath);
        }
    }
    return;
}

void ResetHandler::reset_gpu_max_freq(OptsData &d) {
    char buf[NODE_MAX];
    int32_t rc;
    uint16_t idx = GPU_START_INDEX + GPU_MAX_FREQ_OPCODE;
    FREAD_STR(d.sysfsnode_path[idx], buf, NODE_MAX, rc);
    int32_t cur_max_freq = atoi(buf);
    Target &t = Target::getCurTarget();
    int32_t avail_max_freq = t.getMaxGpuFreq();
    if (cur_max_freq < avail_max_freq) {
        char tmp_s[NODE_MAX];
        snprintf(tmp_s, NODE_MAX, "%" PRId32, avail_max_freq);
        FWRITE_STR(d.sysfsnode_path[idx], tmp_s, strlen(tmp_s), rc);
    }
}

void ResetHandler::reset_sched_boost(OptsData &d) {
    char tmp_s[NODE_MAX];
    int32_t rc = FAILED;
    snprintf(tmp_s, NODE_MAX, "%" PRIu8, RESET_SCHED_BOOST);
    FWRITE_STR(d.sysfsnode_path[SCHED_START_INDEX + SCHED_BOOST_OPCODE], tmp_s, strlen(tmp_s), rc);
}

void ResetHandler::reset_cpu_nodes(int8_t cpu) {
    FILE *defval;
    char buf[NODE_MAX] = "";
    char tmp[NODE_MAX] = "";
    int16_t rc = FAILED;
    uint8_t idx = 0;
    int8_t startcpu = 0, endcpu = 0, cluster = 0;
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    OptsData &d = OptsData::getInstance();

    defval = fopen(DEFAULT_VALUES_FILE, "r");
    if(defval == NULL) {
        QLOGE(LOG_TAG, "Cannot read default values file");
        return;
    }
    //TODO: confirm
    cluster = t.getClusterForCpu(cpu, startcpu, endcpu);
    if (cluster >= 0) {
        for (idx = INTERACTIVE_START_INDEX; idx < CPUBW_HWMON_START_INDEX; ++idx) {
            if (d.is_supported[idx]) {
                strlcpy(buf, d.sysfsnode_path[idx], NODE_MAX);
                buf[CPU_INDEX] = cpu + '0';
                snprintf(tmp, NODE_MAX, INDEX_WITH_CLUSTER, idx, cluster);
                QLOGL(LOG_TAG, QLOG_L2, "writing from %s to node: %s", tmp, buf);
                rc = write_to_node(defval, tmp, buf);
                if ((rc <= 0) && (SINGLE_GOV_INSTANCE != tc.getGovInstanceType()))
                    signal_chk_poll_thread(buf, rc);
            }
        }
    }

    fclose(defval);
    QLOGL(LOG_TAG, QLOG_L2, "CPU:%" PRId8 " Reset Nodes relevant to Profile Manager", cpu);
    return;
}
