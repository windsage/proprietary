/*
 * Copyright (c) 2020, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cutils/properties.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "libsoc_helper.h"

#define SOC_ID_SUPPORT_PROPERTY "ro.vendor.qti.soc_id"
#define SOC_ID_SYSFS_NODE "/sys/devices/soc0/soc_id"

typedef struct cpu_info_v0_1 {
    int cpu_id;
    msm_cpu_t msm_cpu;
    msm_cpu_t cpu_variant;
} cpu_info_v0_1_t;

static cpu_info_v0_1_t cpu_info[] = {
    {636, MSM_CPU_VOLCANO,    MSM_CPU_VOLCANO},
    {640, MSM_CPU_VOLCANO,    MSM_CPU_VOLCANO6},
    {641, MSM_CPU_VOLCANO,    APQ_CPU_VOLCANO},
    {657, MSM_CPU_VOLCANO,    MSM_CPU_VOLCANO6I},
    {658, MSM_CPU_VOLCANO,    APQ_CPU_VOLCANO6I},
    {623, MSM_CPU_PITTI,    MSM_CPU_PITTI},
    {629, MSM_CPU_NIOBE,    MSM_CPU_NIOBE},
    {652, MSM_CPU_NIOBE,    MSM_CPU_NIOBEP},
    {519, MSM_CPU_KALAMA,    MSM_CPU_KALAMA},
    {600, MSM_CPU_KALAMA,    MSM_CPU_KALAMA_GAME},
    {601, MSM_CPU_KALAMA,    APQ_CPU_KALAMA_GAME},
    {507, MSM_CPU_BLAIR, MSM_CPU_BLAIR},
    {565, MSM_CPU_BLAIR, APQ_CPU_BLAIR},
    {578, MSM_CPU_BLAIR, MSM_CPU_BLAIR_LITE},
    {454, MSM_CPU_HOLI, MSM_CPU_HOLI},
    {472, MSM_CPU_HOLI, MSM_CPU_HOLI_H},
    {557, MSM_CPU_PINEAPPLE, MSM_CPU_PINEAPPLE},
    {577, MSM_CPU_PINEAPPLE, APQ_CPU_PINEAPPLE},
    {682, MSM_CPU_PINEAPPLE, APQ_CPU_PINEAPPLE_GAME},
    {696, MSM_CPU_PINEAPPLE, APQ_CPU_TABLET},
    {618, MSM_CPU_SUN,       MSM_CPU_SUN},
    {632, MSM_CPU_CLIFFS, MSM_CPU_CLIFFS7},
    {614, MSM_CPU_CLIFFS, MSM_CPU_CLIFFS},
    {642, MSM_CPU_CLIFFS, APQ_CPU_CLIFFS},
    {643, MSM_CPU_CLIFFS, APQ_CPU_CLIFFS7},
    {608, MSM_CPU_CROW,  MSM_CPU_CROW},
    {321, MSM_CPU_SDM845,  MSM_CPU_SDM845},
    {336, MSM_CPU_SDM670,  MSM_CPU_SDM670},
    {339, MSM_CPU_SM8150,  MSM_CPU_SM8150},
    {356, MSM_CPU_KONA,    MSM_CPU_KONA},
    {360, MSM_CPU_SDM710,  MSM_CPU_SDM710},
    {400, MSM_CPU_LITO,    MSM_CPU_LITO},
    {415, MSM_CPU_LAHAINA, MSM_CPU_LAHAINA},
    {439, MSM_CPU_LAHAINA, APQ_CPU_LAHAINA},
    {450, MSM_CPU_SHIMA,   MSM_CPU_SHIMA},
    {457, MSM_CPU_TARO,    MSM_CPU_TARO},
    {482, MSM_CPU_TARO,    APQ_CPU_TARO},
    {552, MSM_CPU_TARO,    MSM_CPU_TARO_LTE},
    {501, MSM_CPU_SM8325,  MSM_CPU_SM8325},
    {502, MSM_CPU_SM8325,  APQ_CPU_SM8325P},
    {475, MSM_CPU_YUPIK,   MSM_CPU_YUPIK},
    {499, MSM_CPU_YUPIK,   APQ_CPU_YUPIK},
    {497, MSM_CPU_YUPIK,   MSM_CPU_YUPIK_IOT},
    {498, MSM_CPU_YUPIK,   APQ_CPU_YUPIKP_IOT},
    {515, MSM_CPU_YUPIK,   MSM_CPU_YUPIK_LTE},
    {506, MSM_CPU_DIWALI,  MSM_CPU_DIWALI},
    {564, MSM_CPU_DIWALI,  MSM_CPU_DIWALI_LTE},
    {547, MSM_CPU_DIWALI,  APQ_CPU_DIWALI},
    {530, MSM_CPU_CAPE,  MSM_CPU_CAPE},
    {531, MSM_CPU_CAPE,  APQ_CPU_CAPE},
    {540, MSM_CPU_CAPE,  MSM_CPU_CAPE_LTE},
    {537, MSM_CPU_PARROT,  MSM_CPU_PARROT},
    {583, MSM_CPU_PARROT,  APQ_CPU_PARROT},
    {613, MSM_CPU_PARROT,  MSM_CPU_PARROT7},
    {591, MSM_CPU_UKEE,  MSM_CPU_UKEE},
    {568, MSM_CPU_RAVELIN,  MSM_CPU_RAVELIN},
    {602, MSM_CPU_RAVELIN,  APQ_CPU_RAVELIN},
    {581, MSM_CPU_RAVELIN,  MSM_CPU_RAVELIN_IOT},
    {582, MSM_CPU_RAVELIN,  APQ_CPU_RAVELIN_IOT},
};

// internal function to get soc_id
static int get_soc_id();

// internal function to lookup sysfs
static int lookup_sysfs_node();

// internal function to map soc_id to an enum
void map_soc_id_to_enum(soc_info_v0_1_t *, int);

void get_soc_info(soc_info_v0_1_t *soc_info) {
    int soc_id = -1;
    soc_id = get_soc_id();
    map_soc_id_to_enum(soc_info, soc_id);
}

static int get_soc_id() {
    int msm_soc_id = -1;
    char soc_id_prop[PROPERTY_VALUE_MAX] = {0};
    long value = 0;
    char *end_ptr;

    if (property_get(SOC_ID_SUPPORT_PROPERTY, soc_id_prop, "0")){
        errno = 0;
        value = strtol(soc_id_prop, &end_ptr, 10);
        if ((errno == ERANGE) || (end_ptr == soc_id_prop) ||
            (value == 0)) {
            msm_soc_id = lookup_sysfs_node();
        } else {
            msm_soc_id = value;
        }
    } else {
        msm_soc_id = lookup_sysfs_node();
    }

    return msm_soc_id;
}

static int lookup_sysfs_node() {
    FILE *fp = NULL;
    int sysfs_soc_id = -1;

    fp = fopen(SOC_ID_SYSFS_NODE, "r");
    if (NULL != fp) {
        if (0 == fscanf(fp, "%d", &sysfs_soc_id)) {
            sysfs_soc_id = -1;
        }
        fclose(fp);
    }

    return sysfs_soc_id;
}

void map_soc_id_to_enum(soc_info_v0_1_t *soc_info, int soc_id) {
    int index = 0;
    soc_info->msm_cpu = MSM_CPU_UNKNOWN;
    soc_info->cpu_variant = MSM_CPU_UNKNOWN;

    for (index=0; index < sizeof(cpu_info)/sizeof(cpu_info[0]); index++) {
        if (soc_id == cpu_info[index].cpu_id) {
            soc_info->msm_cpu = cpu_info[index].msm_cpu;
            soc_info->cpu_variant = cpu_info[index].cpu_variant;
            break;
        }
    }
}
