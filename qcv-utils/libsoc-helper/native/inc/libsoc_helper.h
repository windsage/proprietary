/*
 * Copyright (c) 2020, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __LIBSOC_HELPER_H__
#define __LIBSOC_HELPER_H__

// enums for msm cpu
typedef enum msm_cpu {
    MSM_CPU_VOLCANO    = 636,
    MSM_CPU_VOLCANO6   = 640,
    APQ_CPU_VOLCANO    = 641,
    MSM_CPU_VOLCANO6I = 657,
    APQ_CPU_VOLCANO6I = 658,
    MSM_CPU_PITTI      = 623,
    MSM_CPU_NIOBE      = 629,
    MSM_CPU_NIOBEP     = 652,
    MSM_CPU_KALAMA      = 519,
    MSM_CPU_KALAMA_GAME = 600,
    APQ_CPU_KALAMA_GAME = 601,
    MSM_CPU_SUN         = 618,
    MSM_CPU_BLAIR      = 507,
    APQ_CPU_BLAIR      = 565,
    MSM_CPU_BLAIR_LITE = 578,
    MSM_CPU_HOLI       = 454,
    MSM_CPU_HOLI_H     = 472,
    MSM_CPU_PINEAPPLE  = 557,
    APQ_CPU_PINEAPPLE  = 577,
    APQ_CPU_PINEAPPLE_GAME = 682,
    APQ_CPU_TABLET     = 696,
    MSM_CPU_CLIFFS7    = 632,
    MSM_CPU_CLIFFS     = 614,
    APQ_CPU_CLIFFS     = 642,
    APQ_CPU_CLIFFS7    = 643,
    MSM_CPU_CROW       = 608,
    MSM_CPU_SDM845      = 321,
    MSM_CPU_SDM670      = 336,
    MSM_CPU_SM8150      = 339,
    MSM_CPU_KONA        = 356,
    MSM_CPU_SDM710      = 360,
    MSM_CPU_LITO        = 400,
    MSM_CPU_LAHAINA     = 415,
    APQ_CPU_LAHAINA     = 439,
    MSM_CPU_SHIMA       = 450,
    MSM_CPU_TARO        = 457,
    APQ_CPU_TARO        = 482,
    MSM_CPU_TARO_LTE    = 552,
    MSM_CPU_SM8325      = 501,
    APQ_CPU_SM8325P     = 502,
    MSM_CPU_YUPIK       = 475,
    APQ_CPU_YUPIK       = 499,
    MSM_CPU_YUPIK_IOT   = 497,
    APQ_CPU_YUPIKP_IOT  = 498,
    MSM_CPU_YUPIK_LTE   = 515,
    MSM_CPU_DIWALI      = 506,
    MSM_CPU_DIWALI_LTE  = 564,
    APQ_CPU_DIWALI      = 547,
    MSM_CPU_CAPE        = 530,
    APQ_CPU_CAPE        = 531,
    MSM_CPU_CAPE_LTE    = 540,
    MSM_CPU_PARROT      = 537,
    APQ_CPU_PARROT      = 583,
    MSM_CPU_PARROT7     = 613,
    MSM_CPU_UKEE        = 591,
    MSM_CPU_RAVELIN     = 568,
    APQ_CPU_RAVELIN     = 602,
    MSM_CPU_RAVELIN_IOT = 581,
    APQ_CPU_RAVELIN_IOT = 582,
    MSM_CPU_UNKNOWN     = -1,
} msm_cpu_t;

// struct to hold soc_info
typedef struct soc_info_v0_1 {
    msm_cpu_t msm_cpu;
    msm_cpu_t cpu_variant;
} soc_info_v0_1_t;

// returns msm_cpu and cpu_variant as an enum. msm_cpu would be
// common across all variants e.g. msm_cpu would be populated as
// MSM_CPU_TARO for both MSM and APQ variants.
// cpu_variant would be the enum corresponding to soc_id
// e.g. MSM_CPU_TARO for MSM and APQ_CPU_TARO for APQ.
// MSM_CPU_UNKNOWN is the default value if soc_id is not mapped.
void get_soc_info(soc_info_v0_1_t *soc_info);

#endif  // __LIBSOC_HELPER_H__
