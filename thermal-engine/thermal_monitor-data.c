/*===========================================================================

Copyright (c) 2012-2015,2019-2021 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"

#define VDD_RESTRICTION_THRESH 5000
#define VDD_RESTRICTION_THRESH_CLR 10000

static struct setting_info tm_cfgs_waipio[] =
{
	{
		.desc = "CPUSS-NSP-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-NSP",
			.sampling_period_ms = 10,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 75000,
				.lvl_clr = 60000,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "cpu-cluster0",
						.info = 1363200,
					},
					[1] = {
						.device = "cpu-cluster1",
						.info = 1324800,
					},
					[2] = {
						.device = "cpu-cluster2",
						.info = 1728000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 80000,
				.lvl_clr = 75000,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "cpu-cluster0",
						.info = 1363200,
					},
					[1] = {
						.device = "cpu-cluster1",
						.info = 633600,
					},
					[2] = {
						.device = "cpu-cluster2",
						.info = 806400,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 85000,
				.lvl_clr = 75000,
				.num_actions = 6,
				.actions = {
					[0] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[3] = {
						.device = "cpu-cluster0",
						.info = 1363200,
					},
					[4] = {
						.device = "cpu-cluster1",
						.info = 633600,
					},
					[5] = {
						.device = "cpu-cluster2",
						.info = 806400,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-NSP-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MODEM-NSP",
			.sampling_period_ms = 10,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 75000,
				.lvl_clr = 65000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 7,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 2,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 85000,
				.lvl_clr = 75000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 10,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-VDD",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdmss-0",
			.sampling_period_ms = 10,
			.descending_thresh = 1,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 5000,
				.lvl_clr = 10000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_vdd",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "PM8450-TZ",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8450_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_9607[] =
{
	{
		.desc = "MODEM_VDD",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "nav",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 3,
				.actions[0] = {
					.device = "modem_vdd",
					.info = 1,
				},
				.actions[1] = {
					.device = "cx",
					.info = 1,
				},
				.actions[2] = {
					.device = "mx",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_waipio_critical_trips[] =
{
	{
		.desc = "XO-THERM-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "xo-therm",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 90000,
				.lvl_clr = 89999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "NSP-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-NSP-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_cliffs_critical_trips[] =
{
	{
		.desc = "XO-THERM-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-0",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 90000,
				.lvl_clr = 89999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "NSP-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-NSP-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_waipio_qrd[] =
{
	{
		.desc = "WLS-CHARGE",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "wlc-therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 47000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 8,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 50000,
				.lvl_clr = 49000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 12,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 52000,
				.lvl_clr = 51000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 16,
					},
				},
			},
		},
	},
	{
		.desc = "CHARGE-CPU-GAME",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "CHARGE-CPU-SENSOR",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 40000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 8,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 42000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 16,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-FR2-NSA-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MODEM-FR2-NSA-SKIN",
			.sampling_period_ms = 100,
			.num_thresholds = 5,
			._n_thresholds = 5,
			._n_to_clear = 5,
			._n_actions = 5,
			._n_action_info = 5,
			.t[0] = {
				.lvl_trig = 49000,
				.lvl_clr = 48000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 7,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 2,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 51500,
				.lvl_clr = 50000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 9,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 3,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 53000,
				.lvl_clr = 52000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 11,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 3,
					},
				},
			},
			.t[3] = {
				.lvl_trig = 55000,
				.lvl_clr = 53500,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 11,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 10,
					},
				},
			},
			.t[4] = {
				.lvl_trig = 75000,
				.lvl_clr = 73000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "modem_lte_dsc",
						.info = 255,
					},
					[1] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_diwali_critical_trips[] =
{
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "NSP-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-NSP-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_kalama_misc_list[] =
{
	{
		.desc = "MISC-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MISC-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550VS_E_TZ-CAMERA-EXIT",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550vs_e_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550B_TZ-BATTERY-CNTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550b_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 3,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-VDD",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdmss-0",
			.sampling_period_ms = 10,
			.descending_thresh = 1,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 5000,
				.lvl_clr = 10000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_vdd",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_kalama_qrd[] =
{
	{
		.desc = "PAUSE-CPU7-LOW",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "CPU-TSKIN-SENSOR",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 51000,
				.lvl_clr = 49000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pause-cpu7",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_khaje[] =
{
	{
		.desc = "CDSP-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cdsp",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-0-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdm-0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-1-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdm-1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "GPU-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "gpu",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "CAMERA-BW-SCORE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "camera_bw_score",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 4702,
				.lvl_clr = 4701,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_bw",
						.info = 1,
					}
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mapss",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 5,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
				.actions[1] = {
					.device = "cx-cdev-lvl",
					.info = 1,
				},
				.actions[2] = {
					.device = "mx-cdev-lvl",
					.info = 1,
				},
				.actions[3] = {
					.device = "lmh-cpu-vdd0",
					.info = 1,
				},
				.actions[4] = {
					.device = "rpm-smd-cdev",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "camera",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 5,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
				.actions[1] = {
					.device = "cx-cdev-lvl",
					.info = 1,
				},
				.actions[2] = {
					.device = "mx-cdev-lvl",
					.info = 1,
				},
				.actions[3] = {
					.device = "lmh-cpu-vdd0",
					.info = 1,
				},
				.actions[4] = {
					.device = "rpm-smd-cdev",
					.info = 2,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_khaje_qrd[] =
{
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet-therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 56000,
				.lvl_clr = 52000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_pineapple_misc_list[] =
{
	{
		.desc = "CPU-GOLD-PLUS-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-PLUS-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MISC-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MISC-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550VE_TZ-CAMERA-EXIT",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550ve_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550B_TZ-BATTERY-CNTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550b_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 3,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-VDD",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdmss-0",
			.sampling_period_ms = 10,
			.descending_thresh = 1,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 5000,
				.lvl_clr = 10000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_vdd",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "SOCD-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "socd",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100,
				.lvl_clr = 99,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "modem_nr_dsc",
						.info = 255,
					},
					[1] = {
						.device = "modem_lte_dsc",
						.info = 255,
					},
					[2] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CHARGE-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "skin-msm-therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 51000,
				.lvl_clr = 49000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 9,
					},
				},
			},
		},
	},
	{
		.desc = "BACKLIGHT-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "skin-msm-therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 51000,
				.lvl_clr = 49000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "panel0-backlight",
						.info = 100,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_pineapple_qrd[] =
{
	{
		.desc = "LTE-NR-PA-THERM-TSKIN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "FR1-PA-TSKIN-SENSOR",
			.sampling_period_ms = 1000,
			.num_thresholds = 5,
			._n_thresholds = 5,
			._n_to_clear = 5,
			._n_actions = 5,
			._n_action_info = 5,
			.t[0] = {
				.lvl_trig = 47000,
				.lvl_clr = 46500,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 11,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 48000,
				.lvl_clr = 47500,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_nr_sdr0_scg_dsc",
						.info = 11,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 49000,
				.lvl_clr = 47500,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 12,
					},
				},
			},
			.t[3] = {
				.lvl_trig = 50000,
				.lvl_clr = 48500,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_nr_sdr0_scg_dsc",
						.info = 12,
					},
				},
			},
			.t[4] = {
				.lvl_trig = 95000,
				.lvl_clr = 94500,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 255,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-FR2-NSA-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "FR2-MSM-THERM-TSKIN-SENSOR",
			.sampling_period_ms = 1000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_nr_scg_dsc",
						.info = 2,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 46000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_nr_scg_dsc",
						.info = 3,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 70000,
				.lvl_clr = 65000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_nr_scg_dsc",
						.info = 12,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_holi[] =
{
	{
		.desc = "Q6-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "q6-hvx",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_holi_qrd[] =
{
	{
		.desc = "Q6-CX-TJ-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "q6-hvx",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet-therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_cliffs_misc_list[] =
{
	{
		.desc = "MISC-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MISC-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM-VDD",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdmss-0",
			.sampling_period_ms = 10,
			.descending_thresh = 1,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 5000,
				.lvl_clr = 10000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_vdd",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550VE_TZ-CAMERA-EXIT",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550ve_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "FAIL-SAFE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-FAIL-SAFE-TJ",
			.sampling_period_ms = 1,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 255000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "cpu-cluster1",
						.info = 480000,
					},
					[5] = {
						.device = "cpu-cluster2",
						.info = 480000,
					},
					[6] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 100000,
				.num_actions = 10,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 255000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "cpu-cluster0",
						.info = 364800,
					},
					[5] = {
						.device = "cpu-cluster1",
						.info = 480000,
					},
					[6] = {
						.device = "cpu-cluster2",
						.info = 480000,
					},
					[7] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_cliffs_qrd[] =
{
	{
		.desc = "FAIL-SAFE-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-4",
			.sampling_period_ms = 2000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 60000,
				.lvl_clr = 55000,
				.num_actions = 8,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 255000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[7] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 65000,
				.lvl_clr = 60000,
				.num_actions = 10,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 255000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[7] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[8] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[9] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
		},
	},
	{
		.desc = "CAMERA_SKIN_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 53000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "MONITOR_SKIN_GOLDPLUS",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-4",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 46000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pause-cpu7",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "MODEM_LTE_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-1",
			.sampling_period_ms = 2000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 52000,
				.lvl_clr = 47000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_lte_sdr0_dsc",
					.info = 11,
				},
			},
			.t[1] = {
				.lvl_trig = 56000,
				.lvl_clr = 51000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_lte_sdr0_dsc",
					.info = 12,
				},
			},
			.t[2] = {
				.lvl_trig = 65000,
				.lvl_clr = 60000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_lte_sdr0_dsc",
					.info = 255,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_monaco_wdp[] =
{
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cam-therm",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 45000,
				.lvl_clr = 41000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pa-therm0",
			.sampling_period_ms = 1000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 44000,
				.lvl_clr = 42000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[2] = {
				.lvl_trig = 52000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				},
			},
		},
	},
	{
		.desc = "CPU2_CPU3_HOTPLUG_SKIN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sdm-skin-therm",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 44000,
				.lvl_clr = 40000,
				.num_actions = 2,
				.actions[0] = {
					.device = "hotplug_2",
					.info = 1,
				},
				.actions[1] = {
					.device = "hotplug_3",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sdm-skin-therm",
			.sampling_period_ms = 1000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 7,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 42000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 9,
				},
			},
			.t[2] = {
				.lvl_trig = 48000,
				.lvl_clr = 44000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 15,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_pitti_critical_trips[] =
{
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_pitti_qrd[] =
{
	{
		.desc = "FAIL-SAFE-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-3",
			.sampling_period_ms = 2000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 60000,
				.lvl_clr = 55000,
				.num_actions = 6,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 340000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[5] = {
						.device = "modem_nr_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 65000,
				.lvl_clr = 60000,
				.num_actions = 8,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 340000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[5] = {
						.device = "modem_nr_dsc",
						.info = 255,
					},
					[6] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[7] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-3",
			.sampling_period_ms = 10000,
			.num_thresholds = 4,
			._n_thresholds = 4,
			._n_to_clear = 4,
			._n_actions = 4,
			._n_action_info = 4,
			.t[0] = {
				.lvl_trig = 47000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 4,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 48000,
				.lvl_clr = 46000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 6,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 49000,
				.lvl_clr = 47000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 8,
					},
				},
			},
			.t[3] = {
				.lvl_trig = 50000,
				.lvl_clr = 48000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 9,
					},
				},
			},
		},
	},
	{
		.desc = "CAMERA-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 47000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM_SUB6_LTE_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-5",
			.sampling_period_ms = 2000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 54000,
				.lvl_clr = 51000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_lte_sdr0_dsc",
					.info = 11,
				},
			},
			.t[1] = {
				.lvl_trig = 57000,
				.lvl_clr = 54000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_lte_sdr0_dsc",
					.info = 12,
				},
			},
		},
	},
	{
		.desc = "MODEM_SUB6_NR_LTE_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-5",
			.sampling_period_ms = 2000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 54000,
				.lvl_clr = 51000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_nr_sdr0_dsc",
					.info = 11,
				},
			},
			.t[1] = {
				.lvl_trig = 57000,
				.lvl_clr = 54000,
				.num_actions = 1,
				.actions[0] = {
					.device = "pa_nr_sdr0_dsc",
					.info = 12,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_pitti[] =
{
	{
		.desc = "FAIL-SAFE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-FAIL-SAFE-TJ",
			.sampling_period_ms = 1,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 6,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 340000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[5] = {
						.device = "modem_nr_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 100000,
				.num_actions = 10,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 340000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[6] = {
						.device = "modem_lte_dsc",
						.info = 255,
					},
					[7] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[8] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[9] = {
						.device = "modem_nr_dsc",
						.info = 255,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_niobe_qxr[] =
{
	{
		.desc = "NIOBE-FAN-LOW",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-2",
			.sampling_period_ms = 5000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 53600,
				.lvl_clr = 43600,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "fan-max31760",
						.info = 40,
					},
				},
			},
		},
	},
	{
		.desc = "NIOBE-FAN-HIGH",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-2",
			.sampling_period_ms = 5000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 56000,
				.lvl_clr = 54000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "fan-max31760",
						.info = 60,
					},
				},
			},
		},
	},
};


static struct setting_info tm_cfgs_niobe[] =
{
	{
		.desc = "PMXR2230-TZ",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pmxr2230_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_niobe_critical_trips[] =
{
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "NSP-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-NSP-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_volcano_qrd[] =
{
	{
		.desc = "CAMERA-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
					[1] = {
						.device = "camcorder",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "FAIL-SAFE-SKIN-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-3",
			.sampling_period_ms = 2000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 65000,
				.lvl_clr = 62000,
				.num_actions = 8,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 264000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[7] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 70000,
				.lvl_clr = 68000,
				.num_actions = 9,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 264000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[7] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[8] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
		},
	},
	{
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-7",
			.sampling_period_ms = 10000,
			.num_thresholds = 6,
			._n_thresholds = 6,
			._n_to_clear = 6,
			._n_actions = 6,
			._n_action_info = 6,
			.t[0] = {
				.lvl_trig = 60000,
				.lvl_clr = 58000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 6,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 62000,
				.lvl_clr = 60000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 12,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 64000,
				.lvl_clr = 62000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 18,
					},
				},
			},
			.t[3] = {
				.lvl_trig = 66000,
				.lvl_clr = 64000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 24,
					},
				},
			},
			.t[4] = {
				.lvl_trig = 68000,
				.lvl_clr = 66000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 30,
					},
				},
			},
			.t[5] = {
				.lvl_trig = 70000,
				.lvl_clr = 68000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "battery",
						.info = 35,
					},
				},
			},
		},
	},
	{
		.desc = "MODEM_LTE_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-2",
			.sampling_period_ms = 10000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 11,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 60000,
				.lvl_clr = 55000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 12,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 65000,
				.lvl_clr = 60000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "pa_lte_sdr0_dsc",
						.info = 255,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_volcano_itemp_critical_trips[] =
{
	{
		.desc = "XO-THERM-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-0",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 90000,
				.lvl_clr = 89999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr = 117999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr = 117999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MSM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MSM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr = 117999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr = 117999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_volcano_critical_trips[] =
{
	{
		.desc = "XO-THERM-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "sys-therm-0",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 90000,
				.lvl_clr = 89999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MSM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MSM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MDM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MDM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_volcano[] =
{
	{
		.desc = "FAIL-SAFE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-FAIL-SAFE-TJ",
			.sampling_period_ms = 1,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 8,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 264000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[7] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 100000,
				.num_actions = 9,
				.actions = {
					[0] = {
						.device = "gpu",
						.info = 264000000,
					},
					[1] = {
						.device = "camera",
						.info = 10,
					},
					[2] = {
						.device = "camcorder",
						.info = 10,
					},
					[3] = {
						.device = "cdsp_sw",
						.info = 6,
					},
					[4] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[7] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[8] = {
						.device = "modem_nr_scg_dsc",
						.info = 255,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_anorak_critical_trips[] =
{
	{
		.desc = "CPU-GOLD-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "CPU-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-CPU-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "NSP-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-NSP-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "MM-TJ-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-MM-TJ-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR-SHUTDOWN",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr = 114999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550B_TZ-CAMERA-EXIT",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550b_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 94999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
		},
	},
	{
		.desc = "PM8550B_LITE_TZ-CAMERA-EXIT",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8550b_lite_tz",
			.sampling_period_ms = 10,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 125000,
				.lvl_clr = 124999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "camera",
						.info = 10,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 135000,
				.lvl_clr = 134999,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "shutdown",
						.info = 0,
					},
				},
			},
		},
	},
};

void thermal_monitor_init_data(struct thermal_setting_t *setting)
{
	enum therm_msm_id msm_id = therm_get_msm_id();
	switch (msm_id) {
		case THERM_WAIPIO:
		case THERM_CAPE:
			add_settings(setting, tm_cfgs_waipio,
					ARRAY_SIZE(tm_cfgs_waipio));
			add_settings(setting, tm_cfgs_waipio_critical_trips,
					ARRAY_SIZE(tm_cfgs_waipio_critical_trips));

			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_waipio_qrd,
							ARRAY_SIZE(tm_cfgs_waipio_qrd));
					break;
			}
			break;
		case THERM_DIWALI:
			add_settings(setting, tm_cfgs_diwali_critical_trips,
					ARRAY_SIZE(tm_cfgs_diwali_critical_trips));
			break;
		case THERM_KALAMA:
			add_settings(setting, tm_cfgs_waipio_critical_trips,
					ARRAY_SIZE(tm_cfgs_waipio_critical_trips));
			add_settings(setting, tm_cfgs_kalama_misc_list,
					ARRAY_SIZE(tm_cfgs_kalama_misc_list));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_kalama_qrd,
							ARRAY_SIZE(tm_cfgs_kalama_qrd));
					break;
			}
			break;
		case THERM_MDM_9607:
			add_settings(setting, tm_cfgs_9607,
					ARRAY_SIZE(tm_cfgs_9607));
			break;
		case THERM_KHAJE:
			add_settings(setting, tm_cfgs_khaje,
					ARRAY_SIZE(tm_cfgs_khaje));

			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_khaje_qrd,
							ARRAY_SIZE(tm_cfgs_khaje_qrd));
				break;
			}
			break;
		case THERM_PINEAPPLE:
			add_settings(setting, tm_cfgs_waipio_critical_trips,
					ARRAY_SIZE(tm_cfgs_waipio_critical_trips));
			add_settings(setting, tm_cfgs_pineapple_misc_list,
					ARRAY_SIZE(tm_cfgs_pineapple_misc_list));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_pineapple_qrd,
							ARRAY_SIZE(tm_cfgs_pineapple_qrd));
					break;
			}
			break;
		case THERM_HOLI:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
			case THERM_PLATFORM_IDP:
			case THERM_PLATFORM_ATP:
				add_settings(setting, tm_cfgs_holi,
						ARRAY_SIZE(tm_cfgs_holi));
				break;
			case THERM_PLATFORM_QRD:
				add_settings(setting, tm_cfgs_holi_qrd,
						ARRAY_SIZE(tm_cfgs_holi_qrd));
				break;
			}
			break;
		case THERM_CLIFFS:
			add_settings(setting, tm_cfgs_cliffs_critical_trips,
					ARRAY_SIZE(tm_cfgs_cliffs_critical_trips));
			add_settings(setting, tm_cfgs_cliffs_misc_list,
					ARRAY_SIZE(tm_cfgs_cliffs_misc_list));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_cliffs_qrd,
							ARRAY_SIZE(tm_cfgs_cliffs_qrd));
					break;
			}
			break;
		case THERM_MONACO:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
			case THERM_PLATFORM_IDP:
			case THERM_PLATFORM_ATP:
				break;
			default:
				add_settings(setting, tm_cfgs_monaco_wdp,
					ARRAY_SIZE(tm_cfgs_monaco_wdp));
				break;
			}
			break;
		case THERM_PITTI:
			add_settings(setting, tm_cfgs_pitti,
					ARRAY_SIZE(tm_cfgs_pitti));
			add_settings(setting, tm_cfgs_pitti_critical_trips,
					ARRAY_SIZE(tm_cfgs_pitti_critical_trips));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_pitti_qrd,
						ARRAY_SIZE(tm_cfgs_pitti_qrd));
					break;
			}
			break;
		case THERM_NIOBE:
			add_settings(setting, tm_cfgs_niobe,
					ARRAY_SIZE(tm_cfgs_niobe));
			add_settings(setting, tm_cfgs_niobe_critical_trips,
					ARRAY_SIZE(tm_cfgs_niobe_critical_trips));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QXR:
					add_settings(setting, tm_cfgs_niobe_qxr,
							ARRAY_SIZE(tm_cfgs_niobe_qxr));
					break;
			}
			break;
		case THERM_VOLCANO:
			add_settings(setting, tm_cfgs_volcano,
					ARRAY_SIZE(tm_cfgs_volcano));
			if (therm_get_limit_profile() == THERM_LIMIT_PROFILE_0)
				add_settings(setting, tm_cfgs_volcano_critical_trips,
					ARRAY_SIZE(tm_cfgs_volcano_critical_trips));
			else
				add_settings(setting, tm_cfgs_volcano_itemp_critical_trips,
					ARRAY_SIZE(tm_cfgs_volcano_itemp_critical_trips));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, tm_cfgs_volcano_qrd,
							ARRAY_SIZE(tm_cfgs_volcano_qrd));
					break;
			}
			break;
		case THERM_ANORAK:
			add_settings(setting, tm_cfgs_anorak_critical_trips,
					ARRAY_SIZE(tm_cfgs_anorak_critical_trips));
			break;
		default:
			msg("%s: ERROR Unknown device ID %d\n",
							__func__, msm_id);
			break;
	}
}
