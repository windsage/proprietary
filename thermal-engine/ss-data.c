/*===========================================================================

Copyright (c) 2013-2016,2021 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"
#include "ss_algorithm.h"

static struct setting_info ss_cfgs_waipio[] =
{
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-GPU-NSP",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 80000,
			.set_point_clr = 70000,
		},
	},
};

static struct setting_info ss_cfgs_9607[] =
{
	{
		.desc = "SS-CPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpuss",
			.device = "cpufreq-cpu0",
			.sampling_period_ms = 50,
			.set_point = 105000,
			.set_point_clr = 75000,
		},
	},
};

static struct setting_info ss_cfgs_waipio_qrd[] =
{
	{
		.desc = "SS-VIRTUAL-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-GPU",
			.device = "gpu",
			.sampling_period_ms = 500,
			.set_point = 56000,
			.set_point_clr = 52000,
			.device_mtgn_max_limit = 350000000,
		},
	},
	{
		.desc = "SS-CLUSTER1-HIGH",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-CHARGE-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 55000,
			.set_point_clr = 51000,
			.device_mtgn_max_limit = 1209600,
		},
	},
	{
		.desc = "SS-CLUSTER2-HIGH",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-CHARGE-SENSOR",
			.device = "cpu-cluster2",
			.sampling_period_ms = 1000,
			.set_point = 55000,
			.set_point_clr = 51000,
			.device_mtgn_max_limit = 1497600,
		},
	},
	{
		.desc = "SS-CLUSTER1-LOW",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-CHARGE-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 50000,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 1651200,
		},
	},
	{
		.desc = "SS-CLUSTER2-LOW",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-CHARGE-SENSOR",
			.device = "cpu-cluster2",
			.sampling_period_ms = 1000,
			.set_point = 50000,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 2054400,
		},
	},
};

static struct setting_info ss_cfgs_kalama_qrd[] =
{
	{
		.desc = "SS-GPU-SKIN",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "GPU-TSKIN-SENSOR",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 46500,
			.set_point_clr = 44000,
			.device_mtgn_max_limit = 348000000,
		},
	},
	{
		.desc = "SS-CLUSTER1-LOW",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 51000,
			.set_point_clr = 49000,
			.device_mtgn_max_limit = 1920000,
		},
	},
};

static struct setting_info ss_cfgs_pineapple_qrd[] =
{
	{
		.desc = "SS-GPU-SKIN-TEMP",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "GPU-TSKIN-SENSOR",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 47500,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 310000000,
		},
	},
	{
		.desc = "SS-CLUSTER1-TRIP1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 5000,
			.set_point = 48000,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 1708800,
		},
	},
	{
		.desc = "SS-CLUSTER2-TRIP1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster2",
			.sampling_period_ms = 5000,
			.set_point = 48000,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 1708800,
		},
	},
	{
		.desc = "SS-CLUSTER3-TRIP1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster3",
			.sampling_period_ms = 5000,
			.set_point = 48000,
			.set_point_clr = 46000,
			.device_mtgn_max_limit = 1593600,
		},
	},
	{
		.desc = "SS-CLUSTER1-TRIP2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 5000,
			.set_point = 51000,
			.set_point_clr = 49000,
			.device_mtgn_max_limit = 1612800,
		},
	},
	{
		.desc = "SS-CLUSTER2-TRIP2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster2",
			.sampling_period_ms = 5000,
			.set_point = 51000,
			.set_point_clr = 49000,
			.device_mtgn_max_limit = 1612800,
		},
	},
	{
		.desc = "SS-CLUSTER3-TRIP2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "CPU-TSKIN-SENSOR",
			.device = "cpu-cluster3",
			.sampling_period_ms = 5000,
			.set_point = 51000,
			.set_point_clr = 49000,
			.device_mtgn_max_limit = 1363200,
		},
	},

};

static struct setting_info ss_cfgs_monaco[] =
{
    {
        .desc = "SS-CPU",
        .algo_type = SS_ALGO_TYPE,
        .data.ss =
        {
            .sensor = "sdm-skin-therm",
            .device = "cpu-cluster0",
            .sampling_period_ms = 1000,
            .set_point = 44000,
            .set_point_clr = 40000,
            .time_constant = 0,
            .device_mtgn_max_limit = 864000,
        },
    },
    {
        .desc = "SS-GPU-SKIN-TEMP",
        .algo_type = SS_ALGO_TYPE,
        .data.ss =
        {
            .sensor = "quiet-therm",
            .device = "gpu",
            .sampling_period_ms = 10000,
            .set_point = 48000,
            .set_point_clr = 44000,
            .time_constant = 0,
            .device_mtgn_max_limit = 400000000,
        },
    },
    {
        .desc = "SS_GPU",
        .algo_type = SS_ALGO_TYPE,
        .data.ss =
        {
            .sensor = "gpu",
            .device = "gpu",
            .sampling_period_ms = 250,
            .set_point = 85000,
            .set_point_clr = 65000,
            .time_constant = 0,
        },
    },
};

static struct setting_info ss_cfgs_cliffs_qrd[] =
{
	{
		.desc = "SS-SKIN-GPU-LOW",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-4",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 47000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 736000000,
		},
	},
	{
		.desc = "SS-SKIN-GPU-HIGH",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-4",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 49000,
			.set_point_clr = 47000,
			.device_mtgn_max_limit = 500000000,
		},
	},
	{
		.desc = "SS-SKIN-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-4",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 46000,
			.set_point_clr = 44000,
			.device_mtgn_max_limit = 1593600,
		},
	},
};

static struct setting_info ss_cfgs_volcano[] =
{
	{
		.desc = "SS-SKIN-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 47000,
			.set_point_clr = 45000,
			.time_constant = 0,
			.device_mtgn_max_limit = 895000000,
		},
	},
	{
		.desc = "SS-SKIN-GOLDPLUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "cpu-cluster2",
			.sampling_period_ms = 1000,
			.set_point = 47000,
			.set_point_clr = 45000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1651200,
		},
	},
	{
		.desc = "SS-SKIN-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 47000,
			.set_point_clr = 45000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1612800,
		},
	},
	{
		.desc = "SS-SKIN-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "cpu-cluster0",
			.sampling_period_ms = 1000,
			.set_point = 48000,
			.set_point_clr = 46000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1497600,
		},
	},
};

static struct setting_info ss_cfgs_pitti_qrd[] =
{
	{
		.desc = "SS-SKIN-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 46000,
			.set_point_clr = 44000,
			.time_constant = 0,
			.device_mtgn_max_limit = 875000000,
		},
	},
	{
		.desc = "SS-SKIN-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 46000,
			.set_point_clr = 44000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1651200,
		},
	},
	{
		.desc = "SS-SKIN-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "sys-therm-3",
			.device = "cpu-cluster0",
			.sampling_period_ms = 1000,
			.set_point = 47000,
			.set_point_clr = 45000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1632000,
		},
	},
};

static struct setting_info ss_cfgs_niobe_qxr[] =
{
	{
		.desc = "SS-SKIN-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-TSKIN-SENSOR",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 41000,
			.set_point_clr = 39000,
			.time_constant = 0,
			.device_mtgn_max_limit = 415000000,
		},
	},
	{
		.desc = "SS-SKIN-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-TSKIN-SENSOR",
			.device = "cpu-cluster1",
			.sampling_period_ms = 1000,
			.set_point = 41000,
			.set_point_clr = 39000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1651200,
		},
	},
	{
		.desc = "SS-SKIN-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-TSKIN-SENSOR",
			.device = "cpu-cluster0",
			.sampling_period_ms = 1000,
			.set_point = 41000,
			.set_point_clr = 39000,
			.time_constant = 0,
			.device_mtgn_max_limit = 1651200,
		},
	},
};


void ss_init_data(struct thermal_setting_t *setting)
{
	enum therm_msm_id msm_id = therm_get_msm_id();
	switch (msm_id) {
		case THERM_WAIPIO:
		case THERM_CAPE:
			add_settings(setting, ss_cfgs_waipio,
					ARRAY_SIZE(ss_cfgs_waipio));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, ss_cfgs_waipio_qrd,
							ARRAY_SIZE(ss_cfgs_waipio_qrd));
					break;
			}
			break;
		case THERM_KALAMA:
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, ss_cfgs_kalama_qrd,
							ARRAY_SIZE(ss_cfgs_kalama_qrd));
					break;
			}
			break;
		case THERM_PINEAPPLE:
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, ss_cfgs_pineapple_qrd,
							ARRAY_SIZE(ss_cfgs_pineapple_qrd));
					break;
			}
			break;
		case THERM_MONACO:
			add_settings(setting, ss_cfgs_monaco,
					ARRAY_SIZE(ss_cfgs_monaco));
			break;
		case THERM_CLIFFS:
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, ss_cfgs_cliffs_qrd,
							ARRAY_SIZE(ss_cfgs_cliffs_qrd));
					break;
			}
			break;
		case THERM_MDM_9607:
			add_settings(setting, ss_cfgs_9607,
					ARRAY_SIZE(ss_cfgs_9607));
			break;
		case THERM_VOLCANO:
			add_settings(setting, ss_cfgs_volcano,
					ARRAY_SIZE(ss_cfgs_volcano));
			break;
		case THERM_NIOBE:
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QXR:
					add_settings(setting, ss_cfgs_niobe_qxr,
							ARRAY_SIZE(ss_cfgs_niobe_qxr));
					break;
				}
			break;
		case THERM_PITTI:
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, ss_cfgs_pitti_qrd,
							ARRAY_SIZE(ss_cfgs_pitti_qrd));
					break;
			}
			break;
		default:
			msg("%s: ERROR Unknown device ID %d\n",
							__func__, msm_id);
			/* Hopefully kernel governor's are enabled */
			return;
	}
}
