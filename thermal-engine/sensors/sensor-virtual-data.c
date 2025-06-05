/*===========================================================================
Copyright (c) 2015,2019-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"

static char *vs_cpu_list_waipio[] = {
	"cpu-1-7",
	"cpu-1-1",
	"cpu-0-3",
	"cpuss-1",
};

static char *vs_cpu_trip_list_waipio[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
	"cpu-1-1",
};

static char *vs_gpu_list_waipio[] = {
	"gpuss-0",
	"gpuss-1",
};

static char *vs_gpu_trip_list_waipio[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
	"gpuss-0",
};

static char *vs_modem_list_waipio[] = {
	"mdmss-0",
	"mdmss-1",
	"mdmss-2",
	"mdmss-3",
};

static char *vs_modem_trip_list_waipio[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
	"mdmss-2",
};

static char *vs_trip_logic_waipio[] = {
	"||",
	"||",
	"&&",
};

static char *vs_gpu_trip_list_waipio_qrd[] = {
       "gpuss-1",
       "skin-msm-therm",
};

static char *vs_gpu_trip_logic_waipio_qrd[] = {
       "&&",
};

static int vs_gpu_list_waipio_qrd_weight[] = {
       2,
       8,
};

static char *vs_battery_trip_list_waipio_qrd[] = {
       "skin-msm-therm",
};

static char *vs_battery_list_waipio_qrd[] = {
       "wlc-therm",
};

static char *vs_cpu_trip_list_waipio_qrd[] = {
       "conn-therm",
};

static char *vs_cpu_list_waipio_qrd[] = {
       "skin-msm-therm",
};

static char *vs_modem_skin_list_waipio_qrd[] = {
	"skin-msm-therm",
};

static char *vs_modem_skin_trip_list_waipio_qrd[] = {
	"mdmss-2",
	"sub1-modem-cfg",
};

static char *vs_modem_skin_trip_logic_waipio_qrd[] = {
       "&&",
};

static char *vs_cpu_gold_list_waipio_critical[] = {
	"cpu-1-0",
	"cpu-1-1",
	"cpu-1-2",
	"cpu-1-3",
	"cpu-1-4",
	"cpu-1-5",
	"cpu-1-6",
	"cpu-1-7",
	"cpu-1-8",
};

static char *vs_cpu_gold_trip_logic_waipio_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_list_waipio_critical[] = {
	"cpu-0-0",
	"cpu-0-1",
	"cpu-0-2",
	"cpu-0-3",
	"cpuss-0",
	"cpuss-1",
	"cpuss-2",
	"cpuss-3",
};

static char *vs_cpu_trip_logic_waipio_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_nsp_list_waipio_critical[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
};

static char *vs_nsp_trip_logic_waipio_critical[] = {
	"||",
	"||",
};

static char *vs_modem_trip_logic_waipio_critical[] = {
	"||",
	"||",
	"||",
};

static char *vs_mm_list_waipio_critical[] = {
	"gpuss-0",
	"gpuss-1",
	"video",
	"ddr",
	"camera-0",
	"camera-1",
};

static char *vs_mm_trip_logic_waipio_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_pmic_alarm_list_waipio_critical[] = {
	"pm8450_tz",
	"pm8350_tz",
	"pm8350c_tz",
	"pm8350b_tz",
	"pmr735a_tz",
	"pmr735b_tz",
	"pm8010i_tz",
	"pm8010j_tz",
};

static char *vs_pmic_alarm_trip_logic_waipio_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_gold_list_diwali_critical[] = {
	"cpu-1-0",
	"cpu-1-1",
	"cpu-1-2",
	"cpu-1-3",
	"cpu-1-4",
	"cpu-1-5",
	"cpu-1-6",
	"cpu-1-7",
};

static char *vs_cpu_gold_trip_logic_diwali_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_list_diwali_critical[] = {
	"cpu-0-0",
	"cpu-0-1",
	"cpu-0-2",
	"cpu-0-3",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_cpu_trip_logic_diwali_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_nsp_list_diwali_critical[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
};

static char *vs_nsp_trip_logic_diwali_critical[] = {
	"||",
	"||",
};

static char *vs_modem_list_diwali_critical[] = {
	"mdmss-0",
	"mdmss-1",
	"mdmss-2",
	"mdmss-3",
};

static char *vs_modem_trip_logic_diwali_critical[] = {
	"||",
	"||",
	"||",
};

static char *vs_mm_list_diwali_critical[] = {
	"gpuss-0",
	"gpuss-1",
	"video",
	"ddr",
	"camera-0",
	"camera-1",
};

static char *vs_mm_trip_logic_diwali_critical[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_pmic_alarm_list_diwali_critical[] = {
	"pm7250b_tz",
	"pm7325_tz",
	"pm8350c_tz",
	"pmr735a_tz",
};

static char *vs_pmic_alarm_trip_logic_diwali_critical[] = {
	"||",
	"||",
	"||",
};

static char *vs_pmic_alarm_kalama_list[] = {
	"pm8550_tz",
	"pm8550b_tz",
	"pm8550ve_f_tz",
	"pm8550vs_c_tz",
	"pm8550vs_d_tz",
	"pm8550vs_e_tz",
	"pm8550vs_g_tz",
	"pmr735d_k_tz",
	"pmr735d_l_tz",
};

static char *vs_9_sensors_or_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_gold_list_kalama[] = {
	"cpu-1-0",
	"cpu-1-1",
	"cpu-1-2",
	"cpu-1-3",
	"cpu-1-4",
	"cpu-1-5",
	"cpu-1-6",
	"cpu-1-7",
	"cpu-1-8",
	"cpu-1-9",
	"cpu-1-10",
};

static char *vs_silver_mhm_cpu_list_kalama[] = {
	"cpu-0-0",
	"cpu-0-1",
	"cpu-0-2",
	"cpuss-0",
	"cpuss-1",
	"cpuss-2",
	"cpuss-3",
};

static char *vs_mm_sensors_list_kalama[] = {
	"gpuss-0",
	"gpuss-1",
	"gpuss-2",
	"gpuss-3",
	"gpuss-4",
	"gpuss-5",
	"gpuss-6",
	"gpuss-7",
	"video",
	"camera-0",
	"camera-1",
};

static char *vs_nsp_4_sensors_list[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
	"nspss-3",
};

static char *vs_misc_4_sensors_list[] = {
	"aoss-0",
	"aoss-1",
	"aoss-2",
	"ddr",
};

static char *vs_12_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_11_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_10_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_3_sensors_or_trip_logic[] = {
	"||",
	"||",
};

static char *vs_7_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_gpu_trip_list_Kalama_qrd[] = {
       "gpuss-2",
};

static char *vs_cpu_trip_list_Kalama_qrd[] = {
       "cpu-1-9",
};

static char *vs_skin_list_Kalama_qrd[] = {
       "skin-msm-therm",
};

static char *vs_gpu_trip_list_pineapple_qrd[] = {
	"gpuss-2",
	"wls-therm",
};

static char *vs_gpu_sensors_trip_pineapple_logic[] = {
	"&&",
};

static char *vs_skin_list_pineapple_qrd[] = {
	"skin-msm-therm",
};

static char *vs_cpu_trip_list_pineapple_qrd[] = {
	"cpu-2-1-1",
	"cpu-1-2-1",
	"cpu-0-1-0",
	"wls-therm",
};

static char *vs_cpu_sensors_trip_pineapple_logic[] = {
	"||",
	"||",
	"&&",
};

static char *vs_pmic_alarm_pineapple_list[] = {
	"pm8550_tz",
	"pm8550b_tz",
	"pm8550b_lite_tz",
	"pm8550ve_tz",
	"pm8550vs_c_tz",
	"pm8550vs_d_tz",
	"pm8550vs_e_tz",
	"pm8550vs_g_tz",
	"pmr735d_tz",
};

static char *vs_6_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_gold_list_pineapple[] = {
	"cpu-2-0-0",
	"cpu-2-0-1",
	"cpu-2-1-0",
	"cpu-2-1-1",
	"cpu-2-2-0",
	"cpu-2-2-1",
	"cpu-1-0-0",
	"cpu-1-0-1",
	"cpu-1-1-0",
	"cpu-1-1-1",
};

static char *vs_cpu_gold_plus_list_pineapple[] = {
	"cpu-1-2-0",
	"cpu-1-2-1",
	"cpu-1-2-2",
};

static char *vs_silver_mhm_cpu_list_pineapple[] = {
	"cpu-0-0-0",
	"cpu-0-1-0",
	"cpuss-0",
	"cpuss-1",
	"cpuss-2",
	"cpuss-3",
};

static char *vs_nsp_sensors_pineapple[] = {
	"nsphvx-0",
	"nsphvx-1",
	"nsphmx-0",
	"nsphmx-1",
	"nsphmx-2",
	"nsphmx-3",
};

static char *vs_fr1_pa_trip_list_pineapple_qrd[] = {
	"sdr0_pa",
};

static char *vs_fr1_pa_sensor_list_pineapple_qrd[] = {
	"pa-therm-1",
};

static char *vs_fr2_msm_therm_trip_list_pineapple_qrd[] = {
	"mmw0",
};

static char *vs_5_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_gold_goldp_list_cliffs[] = {
	"cpu-1-0-0",
	"cpu-1-0-1",
	"cpu-1-1-0",
	"cpu-1-1-1",
	"cpu-1-2-0",
	"cpu-1-2-1",
	"cpu-1-3-0",
	"cpu-1-3-1",
	"cpu-2-0-0",
	"cpu-2-0-1",
	"cpu-2-0-2",
};

static char *vs_silver_mhm_cpu_list_cliffs[] = {
	"cpu-0-0-0",
	"cpu-0-1-0",
	"cpu-0-2-0",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_mm_sensors_list_cliffs[] = {
	"gpuss-0",
	"gpuss-1",
	"gpuss-2",
	"gpuss-3",
	"video",
	"camera-0",
	"camera-1",
};

static char *vs_nsp_sensors_list_cliffs[] = {
	"nsphvx-0",
	"nsphvx-1",
	"nsphmx-0",
	"nsphmx-1",
};

static char *vs_pmic_alarm_cliff_list[] = {
	"pm8550vs_c_tz",
	"pm8010m_tz",
	"pm8010n_tz",
	"pmr735a_tz",
	"pm8550ve_tz",
	"pm7550ba_tz",
	"pmxr2230_tz",
};

static char *vs_fail_safe_list_pitti[] = {
	"gpuss",
	"camera",
	"mdmss-0",
	"mdmss-1",
	"mdmss-2",
};

static char *vs_fail_safe_trip_logic_pitti[] = {
	"||",
	"||",
	"||",
	"||",
};

static char *vs_cpu_gold_list_pitti_critical[] = {
	"cpu-1-0",
	"cpu-1-1",
	"cpu-2-0",
	"cpu-2-1",
};

static char *vs_cpu_gold_trip_logic_pitti_critical[] = {
	"||",
	"||",
	"||",
};

static char *vs_cpu_list_pitti_critical[] = {
	"cpu-0-0",
	"cpu-0-1",
	"cpu-0-2",
	"cpu-0-3",
	"cpu-0-4",
	"cpu-0-5",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_modem_list_pitti_critical[] = {
	"mdmss-0",
	"mdmss-1",
	"mdmss-2",
};

static char *vs_modem_trip_logic_pitti_critical[] = {
	"||",
	"||",
};

static char *vs_mm_list_pitti_critical[] = {
	"gpuss",
	"ddr",
	"camera",
};

static char *vs_pmic_alarm_list_pitti_critical[] = {
	"pm7250b_tz",
	"pm6125_tz",
};

static char *vs_pmic_alarm_trip_logic_pitti_critical[] = {
	"||",
};

static char *vs_tskin_trip_niobe_qxr[] = {
	"sys-therm-2",
};

static char *vs_tskin_list_niobe_qxr[] = {
	"sys-therm-2",
	"sys-therm-1",
};

static int vs_tskin_weight_niobe_qxr[] = {
	 75,
	-15,
};

static char *vs_cpu_gold_list_niobe[] = {
	"cpu-1-0-0",
	"cpu-1-0-1",
	"cpu-1-1-0",
	"cpu-1-1-1",
	"cpu-1-2-0",
	"cpu-1-2-1",
	"cpu-1-3-0",
	"cpu-1-3-1",
};

static char *vs_8_sensors_or_trip_logic[] = {
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
	"||",
};

static char *vs_silver_mhm_cpu_list_niobe[] = {
	"cpu-0-0-0",
	"cpu-0-0-1",
	"cpu-0-1-0",
	"cpu-0-1-1",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_nsp_sensors_list_niobe[] = {
	"nsphvx-0",
	"nsphvx-1",
	"nsphvx-2",
	"nsphmx-0",
	"nsphmx-1",
};

static char *vs_pmic_alarm_niobe_list[] = {
	"pm8550b_tz",
	"pm8550b_lite_tz",
	"pm8550b_i_tz",
	"pm8550b_i_lite_tz",
	"pm8550vs_c_tz",
	"pm8550ve_d_tz",
	"pmxr2230_tz",
	"pm8550ve_tz",
	"pm8010m_tz",
	"pm8010n_tz",
};

static char *vs_cpu_gold_goldp_list_volcano[] = {
	"cpu-1-0-0",
	"cpu-1-0-1",
	"cpu-1-1-0",
	"cpu-1-1-1",
	"cpu-1-2-0",
	"cpu-1-2-1",
	"cpu-1-3-0",
	"cpu-1-3-1",
};

static char *vs_silver_mhm_cpu_list_volcano[] = {
	"cpu-0-0-0",
	"cpu-0-1-0",
	"cpu-0-2-0",
	"cpu-0-3-0",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_msm_sensors_list_volcano[] = {
	"gpuss-0",
	"gpuss-1",
	"camera-0",
	"nsphvx-0",
	"nsphmx-0",
	"nsphmx-1",
	"aoss-0",
	"aoss-1",
	"ddr",
};

static char *vs_pmic_alarm_volcano_list[] = {
	"pm8550vs_c_tz",
	"pmiv0104_tz",
	"pmxr2230_tz",
};

static char *vs_fail_safe_volcano_list[] = {
	"nsphvx-0",
	"nsphmx-0",
	"nsphmx-1",
	"gpuss-0",
	"gpuss-1",
	"camera-0",
	"mdmss-0",
	"mdmss-1",
	"mdmss-2",
	"mdmss-3",
};

static char *vs_cpu_gold_list_anorak_critical [] = {
	"cpu-1-0-0",
	"cpu-1-0-1",
	"cpu-1-1-0",
	"cpu-1-1-1",
	"cpu-1-2-0",
	"cpu-1-2-1",
	"cpu-1-3-0",
	"cpu-1-3-1",
};

static char *vs_cpu_list_anorak_critical[] = {
	"cpu-0-0-0",
	"cpu-0-0-1",
	"cpu-0-1-0",
	"cpu-0-1-1",
	"cpuss-0",
	"cpuss-1",
};

static char *vs_nsp_list_anorak_critical[] = {
	"nspss-0",
	"nspss-1",
	"nspss-2",
};

static char *vs_mm_list_anorak_critical[] = {
	"gpuss-0",
	"gpuss-1",
	"gpuss-2",
	"gpuss-3",
	"gpuss-4",
	"gpuss-5",
	"gpuss-6",
	"gpuss-7",
	"video",
	"ddr",
	"camera-0",
	"camera-1",
};

static char *vs_pmic_alarm_list_anorak_critical[] = {
	"pm8550b_tz",
	"pm8550ve_f_tz",
	"pm8550vs_c_tz",
	"pm8550vs_d_tz",
	"pm8550vs_e_tz",
	"pm8550vs_g_tz",
	"pm8010m_tz"
	"pm8010n_tz"
	"pmxr2230_tz"
};

static struct setting_info vs_cfgs_waipio[] =
{
	{
		.desc = "VIRTUAL-CPU-NSP",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.dynamic = 0,
		.data.v = {
			.trip_sensor_list = vs_cpu_trip_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_cpu_trip_list_waipio),
			.trip_sensor_logic = vs_trip_logic_waipio,
			.logic_cnt = ARRAY_SIZE(vs_trip_logic_waipio),
			.sensor_list = vs_cpu_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[2] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[3] = {
				.lvl_trig = 70000,
				.lvl_clr =  60000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-GPU-NSP",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.dynamic = 0,
		.data.v = {
			.trip_sensor_list = vs_gpu_trip_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_gpu_trip_list_waipio),
			.trip_sensor_logic = vs_trip_logic_waipio,
			.logic_cnt = ARRAY_SIZE(vs_trip_logic_waipio),
			.sensor_list = vs_gpu_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_gpu_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[2] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[3] = {
				.lvl_trig = 70000,
				.lvl_clr =  60000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MODEM-NSP",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.dynamic = 0,
		.data.v = {
			.trip_sensor_list = vs_modem_trip_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_trip_list_waipio),
			.trip_sensor_logic = vs_trip_logic_waipio,
			.logic_cnt = ARRAY_SIZE(vs_trip_logic_waipio),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[2] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[3] = {
				.lvl_trig = 75000,
				.lvl_clr =  65000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_waipio_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_waipio_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_waipio_critical),
			.trip_sensor_logic = vs_cpu_gold_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_gold_trip_logic_waipio_critical),
			.sensor_list = vs_cpu_gold_list_waipio_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_waipio_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_list_waipio_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_list_waipio_critical),
			.trip_sensor_logic = vs_cpu_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_trip_logic_waipio_critical),
			.sensor_list = vs_cpu_list_waipio_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_waipio_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_list_waipio_critical,
			.trip_cnt = ARRAY_SIZE(vs_nsp_list_waipio_critical),
			.trip_sensor_logic = vs_nsp_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_nsp_trip_logic_waipio_critical),
			.sensor_list = vs_nsp_list_waipio_critical,
			.list_cnt = ARRAY_SIZE(vs_nsp_list_waipio_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_list_waipio_critical,
			.trip_cnt = ARRAY_SIZE(vs_mm_list_waipio_critical),
			.trip_sensor_logic = vs_mm_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_mm_trip_logic_waipio_critical),
			.sensor_list = vs_mm_list_waipio_critical,
			.list_cnt = ARRAY_SIZE(vs_mm_list_waipio_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_list_waipio_critical,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_list_waipio_critical),
			.trip_sensor_logic = vs_pmic_alarm_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_pmic_alarm_trip_logic_waipio_critical),
			.sensor_list = vs_pmic_alarm_list_waipio_critical,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_list_waipio_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_waipio_qrd[] =
{
	{
		.desc = "VIRTUAL-GPU",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_gpu_trip_list_waipio_qrd,
			.trip_cnt = ARRAY_SIZE(vs_gpu_trip_list_waipio_qrd),
			.trip_sensor_logic = vs_gpu_trip_logic_waipio_qrd,
			.logic_cnt = ARRAY_SIZE(vs_gpu_trip_logic_waipio_qrd),
			.sensor_list = vs_gpu_trip_list_waipio_qrd,
			.list_cnt = ARRAY_SIZE(vs_gpu_trip_list_waipio_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 70000,
				.lvl_clr =  50000,
			},
			.t[1] = {
				.lvl_trig = 52000,
				.lvl_clr =  48000,
			},
			.sampling_period_ms = 500,
			.weight_list = vs_gpu_list_waipio_qrd_weight,
			.weight_cnt = ARRAY_SIZE(vs_gpu_list_waipio_qrd_weight),
			.weight_size = 1,
			.math_type = MATH_WEIGHT,
		},
	},
	{
		.desc = "CHARGE-CPU-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_battery_trip_list_waipio_qrd,
			.trip_cnt = ARRAY_SIZE(vs_battery_trip_list_waipio_qrd),
			.sensor_list = vs_battery_list_waipio_qrd,
			.list_cnt = ARRAY_SIZE(vs_battery_list_waipio_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr =  50000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "CPU-CHARGE-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_trip_list_waipio_qrd,
			.trip_cnt = ARRAY_SIZE(vs_cpu_trip_list_waipio_qrd),
			.sensor_list = vs_cpu_list_waipio_qrd,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_waipio_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 44000,
				.lvl_clr =  38000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MODEM-FR2-NSA-SKIN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_skin_trip_list_waipio_qrd,
			.trip_cnt = ARRAY_SIZE(vs_modem_skin_trip_list_waipio_qrd),
			.trip_sensor_logic = vs_modem_skin_trip_logic_waipio_qrd,
			.logic_cnt = ARRAY_SIZE(vs_modem_skin_trip_logic_waipio_qrd),
			.sensor_list = vs_modem_skin_list_waipio_qrd,
			.list_cnt = ARRAY_SIZE(vs_modem_skin_list_waipio_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 45000,
				.lvl_clr =  42000,
			},
			.t[1] = {
				.lvl_trig = 3,
				.lvl_clr =  0,
			},
			.sampling_period_ms = 500,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_diwali_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_diwali_critical),
			.trip_sensor_logic = vs_cpu_gold_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_gold_trip_logic_diwali_critical),
			.sensor_list = vs_cpu_gold_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_list_diwali_critical),
			.trip_sensor_logic = vs_cpu_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_trip_logic_diwali_critical),
			.sensor_list = vs_cpu_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_nsp_list_diwali_critical),
			.trip_sensor_logic = vs_nsp_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_nsp_trip_logic_diwali_critical),
			.sensor_list = vs_nsp_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_nsp_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_diwali_critical),
			.trip_sensor_logic = vs_modem_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_diwali_critical),
			.sensor_list = vs_modem_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_modem_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_mm_list_diwali_critical),
			.trip_sensor_logic = vs_mm_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_mm_trip_logic_diwali_critical),
			.sensor_list = vs_mm_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_mm_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_list_diwali_critical,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_list_diwali_critical),
			.trip_sensor_logic = vs_pmic_alarm_trip_logic_diwali_critical,
			.logic_cnt = ARRAY_SIZE(vs_pmic_alarm_trip_logic_diwali_critical),
			.sensor_list = vs_pmic_alarm_list_diwali_critical,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_list_diwali_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_kalama_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_kalama,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_kalama),
			.trip_sensor_logic = vs_11_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_11_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_list_kalama,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_kalama),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_kalama,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_kalama),
			.trip_sensor_logic = vs_7_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_7_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_kalama,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_kalama),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_4_sensors_list,
			.trip_cnt = ARRAY_SIZE(vs_nsp_4_sensors_list),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_nsp_4_sensors_list,
			.list_cnt = ARRAY_SIZE(vs_nsp_4_sensors_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_sensors_list_kalama,
			.trip_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.trip_sensor_logic = vs_11_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_11_sensors_or_trip_logic),
			.sensor_list = vs_mm_sensors_list_kalama,
			.list_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_kalama_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_kalama_list),
			.trip_sensor_logic = vs_9_sensors_or_logic,
			.logic_cnt = ARRAY_SIZE(vs_9_sensors_or_logic),
			.sensor_list = vs_pmic_alarm_kalama_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_kalama_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MISC-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_misc_4_sensors_list,
			.trip_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_misc_4_sensors_list,
			.list_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_kalama_qrd[] =
{
	{
		.desc = "GPU-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_gpu_trip_list_Kalama_qrd,
			.trip_cnt = ARRAY_SIZE(vs_gpu_trip_list_Kalama_qrd),
			.sensor_list = vs_skin_list_Kalama_qrd,
			.list_cnt = ARRAY_SIZE(vs_skin_list_Kalama_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 80000,
				.lvl_clr =  60000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "CPU-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_trip_list_Kalama_qrd,
			.trip_cnt = ARRAY_SIZE(vs_cpu_trip_list_Kalama_qrd),
			.sensor_list = vs_skin_list_Kalama_qrd,
			.list_cnt = ARRAY_SIZE(vs_skin_list_Kalama_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 70000,
				.lvl_clr =  60000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},

};

static struct setting_info vs_cfgs_pineapple_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-PLUS-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_plus_list_pineapple,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_plus_list_pineapple),
			.trip_sensor_logic = vs_3_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_3_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_plus_list_pineapple,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_plus_list_pineapple),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_pineapple,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_pineapple),
			.trip_sensor_logic = vs_10_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_10_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_list_pineapple,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_pineapple),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_pineapple,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_pineapple),
			.trip_sensor_logic = vs_6_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_6_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_pineapple,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_pineapple),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_sensors_pineapple,
			.trip_cnt = ARRAY_SIZE(vs_nsp_sensors_pineapple),
			.trip_sensor_logic = vs_6_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_6_sensors_or_trip_logic),
			.sensor_list = vs_nsp_sensors_pineapple,
			.list_cnt = ARRAY_SIZE(vs_nsp_sensors_pineapple),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_sensors_list_kalama,
			.trip_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.trip_sensor_logic = vs_11_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_11_sensors_or_trip_logic),
			.sensor_list = vs_mm_sensors_list_kalama,
			.list_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_pineapple_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_pineapple_list),
			.trip_sensor_logic = vs_9_sensors_or_logic,
			.logic_cnt = ARRAY_SIZE(vs_9_sensors_or_logic),
			.sensor_list = vs_pmic_alarm_pineapple_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_pineapple_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MISC-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_misc_4_sensors_list,
			.trip_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_misc_4_sensors_list,
			.list_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_pineapple_qrd[] =
{
	{
		.desc = "GPU-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_gpu_trip_list_pineapple_qrd,
			.trip_cnt = ARRAY_SIZE(vs_gpu_trip_list_pineapple_qrd),
			.trip_sensor_logic = vs_gpu_sensors_trip_pineapple_logic,
			.logic_cnt = ARRAY_SIZE(vs_gpu_sensors_trip_pineapple_logic),
			.sensor_list = vs_skin_list_pineapple_qrd,
			.list_cnt = ARRAY_SIZE(vs_skin_list_pineapple_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 66000,
				.lvl_clr =  55000,
			},
			.t[1] = {
				.lvl_trig = 39000,
				.lvl_clr =  38000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "CPU-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_trip_list_pineapple_qrd,
			.trip_cnt = ARRAY_SIZE(vs_cpu_trip_list_pineapple_qrd),
			.trip_sensor_logic = vs_cpu_sensors_trip_pineapple_logic,
			.logic_cnt = ARRAY_SIZE(vs_cpu_sensors_trip_pineapple_logic),
			.sensor_list = vs_skin_list_pineapple_qrd,
			.list_cnt = ARRAY_SIZE(vs_skin_list_pineapple_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 68000,
				.lvl_clr =  58000,
			},
			.t[1] = {
				.lvl_trig = 70000,
				.lvl_clr =  60000,
			},
			.t[2] = {
				.lvl_trig = 65000,
				.lvl_clr =  55000  ,
			},
			.t[3] = {
				.lvl_trig = 39000,
				.lvl_clr =  38000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "FR1-PA-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_fr1_pa_trip_list_pineapple_qrd,
			.trip_cnt = ARRAY_SIZE(vs_fr1_pa_trip_list_pineapple_qrd),
			.sensor_list = vs_fr1_pa_sensor_list_pineapple_qrd,
			.list_cnt = ARRAY_SIZE(vs_fr1_pa_sensor_list_pineapple_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 45000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "FR2-MSM-THERM-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_fr2_msm_therm_trip_list_pineapple_qrd,
			.trip_cnt = ARRAY_SIZE(vs_fr2_msm_therm_trip_list_pineapple_qrd),
			.sensor_list = vs_skin_list_pineapple_qrd,
			.list_cnt = ARRAY_SIZE(vs_skin_list_pineapple_qrd),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 30000,
				.lvl_clr =  25000,
			},
			.sampling_period_ms = 1000,
			.math_type = MATH_MAX,
		},
	},

};

static struct setting_info vs_cfgs_cliffs_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_goldp_list_cliffs,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_goldp_list_cliffs),
			.trip_sensor_logic = vs_11_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_11_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_goldp_list_cliffs,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_plus_list_pineapple),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_cliffs,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_cliffs),
			.trip_sensor_logic = vs_5_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_5_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_cliffs,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_cliffs),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_sensors_list_cliffs,
			.trip_cnt = ARRAY_SIZE(vs_nsp_sensors_list_cliffs),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_nsp_sensors_list_cliffs,
			.list_cnt = ARRAY_SIZE(vs_nsp_sensors_list_cliffs),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_sensors_list_cliffs,
			.trip_cnt = ARRAY_SIZE(vs_mm_sensors_list_cliffs),
			.trip_sensor_logic = vs_7_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_7_sensors_or_trip_logic),
			.sensor_list = vs_mm_sensors_list_cliffs,
			.list_cnt = ARRAY_SIZE(vs_mm_sensors_list_cliffs),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MISC-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_misc_4_sensors_list,
			.trip_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_misc_4_sensors_list,
			.list_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_cliff_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_cliff_list),
			.trip_sensor_logic = vs_7_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_7_sensors_or_trip_logic),
			.sensor_list = vs_pmic_alarm_cliff_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_cliff_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_pitti_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_pitti_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_pitti_critical),
			.trip_sensor_logic = vs_cpu_gold_trip_logic_pitti_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_gold_trip_logic_pitti_critical),
			.sensor_list = vs_cpu_gold_list_pitti_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_pitti_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_list_pitti_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_list_pitti_critical),
			.trip_sensor_logic = vs_8_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_8_sensors_or_trip_logic),
			.sensor_list = vs_cpu_list_pitti_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_pitti_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_pitti_critical,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_pitti_critical),
			.trip_sensor_logic = vs_modem_trip_logic_pitti_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_pitti_critical),
			.sensor_list = vs_modem_list_pitti_critical,
			.list_cnt = ARRAY_SIZE(vs_modem_list_pitti_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_list_pitti_critical,
			.trip_cnt = ARRAY_SIZE(vs_mm_list_pitti_critical),
			.trip_sensor_logic = vs_3_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_3_sensors_or_trip_logic),
			.sensor_list = vs_mm_list_pitti_critical,
			.list_cnt = ARRAY_SIZE(vs_mm_list_pitti_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_list_pitti_critical,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_list_pitti_critical),
			.trip_sensor_logic = vs_pmic_alarm_trip_logic_pitti_critical,
			.logic_cnt = ARRAY_SIZE(vs_pmic_alarm_trip_logic_pitti_critical),
			.sensor_list = vs_pmic_alarm_list_pitti_critical,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_list_pitti_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_pitti[] =
{
	{
		.desc = "VIRTUAL-FAIL-SAFE-TJ",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.dynamic = 0,
		.data.v = {
			.trip_sensor_list = vs_fail_safe_list_pitti,
			.trip_cnt = ARRAY_SIZE(vs_fail_safe_list_pitti),
			.trip_sensor_logic = vs_fail_safe_trip_logic_pitti,
			.logic_cnt = ARRAY_SIZE(vs_fail_safe_trip_logic_pitti),
			.sensor_list = vs_fail_safe_list_pitti,
			.list_cnt = ARRAY_SIZE(vs_fail_safe_list_pitti),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 80000,
				.lvl_clr =  75000,
			},
			.t[1] = {
				.lvl_trig = 80000,
				.lvl_clr =  75000,
			},
			.t[2] = {
				.lvl_trig = 80000,
				.lvl_clr =  75000,
			},
			.t[3] = {
				.lvl_trig = 80000,
				.lvl_clr =  75000,
			},
			.t[4] = {
				.lvl_trig = 80000,
				.lvl_clr =  75000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_niobe_qxr_trips[] =
{
	{
		.desc = "VIRTUAL-TSKIN-SENSOR",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_tskin_trip_niobe_qxr,
			.trip_cnt = ARRAY_SIZE(vs_tskin_trip_niobe_qxr),
			.sensor_list = vs_tskin_list_niobe_qxr,
			.list_cnt = ARRAY_SIZE(vs_tskin_list_niobe_qxr),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 58000,
				.lvl_clr =  56000,
			},
			.sampling_period_ms = 1000,
			.weight_list = vs_tskin_weight_niobe_qxr,
			.denominator = 100,
			.weight_cnt = ARRAY_SIZE(vs_tskin_weight_niobe_qxr),
			.weight_size = 1,
			.y_intercept = 7890,
			.math_type = MATH_WEIGHT,
		},
	},
};

static struct setting_info vs_cfgs_niobe_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_niobe,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_niobe),
			.trip_sensor_logic = vs_8_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_8_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_list_niobe,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_niobe),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_niobe,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_niobe),
			.trip_sensor_logic = vs_6_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_6_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_niobe,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_niobe),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_sensors_list_niobe,
			.trip_cnt = ARRAY_SIZE(vs_nsp_sensors_list_niobe),
			.trip_sensor_logic = vs_5_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_5_sensors_or_trip_logic),
			.sensor_list = vs_nsp_sensors_list_niobe,
			.list_cnt = ARRAY_SIZE(vs_nsp_sensors_list_niobe),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_sensors_list_kalama,
			.trip_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.trip_sensor_logic = vs_11_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_11_sensors_or_trip_logic),
			.sensor_list = vs_mm_sensors_list_kalama,
			.list_cnt = ARRAY_SIZE(vs_mm_sensors_list_kalama),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MISC-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_misc_4_sensors_list,
			.trip_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_misc_4_sensors_list,
			.list_cnt = ARRAY_SIZE(vs_misc_4_sensors_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_niobe_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_niobe_list),
			.trip_sensor_logic = vs_10_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_10_sensors_or_trip_logic),
			.sensor_list = vs_pmic_alarm_niobe_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_niobe_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_volcano_itemp_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_goldp_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_goldp_list_volcano),
			.trip_sensor_logic = vs_8_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_8_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_goldp_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_goldp_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_volcano),
			.trip_sensor_logic = vs_6_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_6_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MSM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_msm_sensors_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_msm_sensors_list_volcano),
			.trip_sensor_logic = vs_9_sensors_or_logic,
			.logic_cnt = ARRAY_SIZE(vs_9_sensors_or_logic),
			.sensor_list = vs_msm_sensors_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_msm_sensors_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 118000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_volcano_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_volcano_list),
			.trip_sensor_logic = vs_nsp_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_nsp_trip_logic_waipio_critical),
			.sensor_list = vs_pmic_alarm_volcano_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_volcano_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_volcano_critical_trips[] =
{
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_goldp_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_goldp_list_volcano),
			.trip_sensor_logic = vs_8_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_8_sensors_or_trip_logic),
			.sensor_list = vs_cpu_gold_goldp_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_goldp_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_silver_mhm_cpu_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_volcano),
			.trip_sensor_logic = vs_6_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_6_sensors_or_trip_logic),
			.sensor_list = vs_silver_mhm_cpu_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_silver_mhm_cpu_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MDM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_modem_list_waipio,
			.trip_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.trip_sensor_logic = vs_modem_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_modem_trip_logic_waipio_critical),
			.sensor_list = vs_modem_list_waipio,
			.list_cnt = ARRAY_SIZE(vs_modem_list_waipio),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MSM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_msm_sensors_list_volcano,
			.trip_cnt = ARRAY_SIZE(vs_msm_sensors_list_volcano),
			.trip_sensor_logic = vs_9_sensors_or_logic,
			.logic_cnt = ARRAY_SIZE(vs_9_sensors_or_logic),
			.sensor_list = vs_msm_sensors_list_volcano,
			.list_cnt = ARRAY_SIZE(vs_msm_sensors_list_volcano),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_volcano_list,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_volcano_list),
			.trip_sensor_logic = vs_nsp_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_nsp_trip_logic_waipio_critical),
			.sensor_list = vs_pmic_alarm_volcano_list,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_volcano_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_volcano[] =
{
	{
		.desc = "VIRTUAL-FAIL-SAFE-TJ",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.dynamic = 0,
		.data.v = {
			.trip_sensor_list = vs_fail_safe_volcano_list,
			.trip_cnt = ARRAY_SIZE(vs_fail_safe_volcano_list),
			.trip_sensor_logic = vs_10_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_10_sensors_or_trip_logic),
			.sensor_list = vs_fail_safe_volcano_list,
			.list_cnt = ARRAY_SIZE(vs_fail_safe_volcano_list),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[2] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[3] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[4] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[5] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[6] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[7] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[8] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.t[9] = {
				.lvl_trig = 100000,
				.lvl_clr =  95000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

static struct setting_info vs_cfgs_anorak_critical_trips[] = {
	{
		.desc = "VIRTUAL-CPU-GOLD-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_gold_list_anorak_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_gold_list_anorak_critical),
			.trip_sensor_logic = vs_cpu_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_cpu_trip_logic_waipio_critical),
			.sensor_list = vs_cpu_gold_list_anorak_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_gold_list_anorak_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-CPU-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_cpu_list_anorak_critical,
			.trip_cnt = ARRAY_SIZE(vs_cpu_list_anorak_critical),
			.trip_sensor_logic = vs_mm_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_mm_trip_logic_waipio_critical),
			.sensor_list = vs_cpu_list_anorak_critical,
			.list_cnt = ARRAY_SIZE(vs_cpu_list_anorak_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-NSP-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_nsp_list_anorak_critical,
			.trip_cnt = ARRAY_SIZE(vs_nsp_list_anorak_critical),
			.trip_sensor_logic = vs_nsp_trip_logic_waipio_critical,
			.logic_cnt = ARRAY_SIZE(vs_nsp_trip_logic_waipio_critical),
			.sensor_list = vs_nsp_list_anorak_critical,
			.list_cnt = ARRAY_SIZE(vs_nsp_list_anorak_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-MM-TJ-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_mm_list_anorak_critical,
			.trip_cnt = ARRAY_SIZE(vs_mm_list_anorak_critical),
			.trip_sensor_logic = vs_12_sensors_or_trip_logic,
			.logic_cnt = ARRAY_SIZE(vs_12_sensors_or_trip_logic),
			.sensor_list = vs_mm_list_anorak_critical,
			.list_cnt = ARRAY_SIZE(vs_mm_list_anorak_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[9] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[10] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[11] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
	{
		.desc = "VIRTUAL-PMIC-ALARM-SHUTDOWN",
		.algo_type = VIRTUAL_SENSOR_TYPE,
		.data.v = {
			.trip_sensor_list = vs_pmic_alarm_list_anorak_critical,
			.trip_cnt = ARRAY_SIZE(vs_pmic_alarm_list_anorak_critical),
			.trip_sensor_logic = vs_9_sensors_or_logic,
			.logic_cnt = ARRAY_SIZE(vs_9_sensors_or_logic),
			.sensor_list = vs_pmic_alarm_list_anorak_critical,
			.list_cnt = ARRAY_SIZE(vs_pmic_alarm_list_anorak_critical),
			.list_size = 1,
			.t[0] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[1] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[3] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[4] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[5] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[6] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[7] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.t[8] = {
				.lvl_trig = 115000,
				.lvl_clr =  100000,
			},
			.sampling_period_ms = 10,
			.math_type = MATH_MAX,
		},
	},
};

void virtual_sensors_init_data(struct thermal_setting_t *setting)
{
	switch (therm_get_msm_id()) {
		case THERM_WAIPIO:
		case THERM_CAPE:
			add_settings(setting, vs_cfgs_waipio,
					ARRAY_SIZE(vs_cfgs_waipio));
			add_settings(setting, vs_cfgs_waipio_critical_trips,
					ARRAY_SIZE(vs_cfgs_waipio_critical_trips));

			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, vs_cfgs_waipio_qrd,
							ARRAY_SIZE(vs_cfgs_waipio_qrd));
					break;
			}
			break;
		case THERM_DIWALI:
			add_settings(setting, vs_cfgs_diwali_critical_trips,
					ARRAY_SIZE(vs_cfgs_diwali_critical_trips));
			break;
		case THERM_KALAMA:
			add_settings(setting, vs_cfgs_kalama_critical_trips,
					ARRAY_SIZE(vs_cfgs_kalama_critical_trips));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, vs_cfgs_kalama_qrd,
							ARRAY_SIZE(vs_cfgs_kalama_qrd));
					break;
			}
			break;
		case THERM_PINEAPPLE:
			add_settings(setting, vs_cfgs_pineapple_critical_trips,
					ARRAY_SIZE(vs_cfgs_pineapple_critical_trips));

			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					add_settings(setting, vs_cfgs_pineapple_qrd,
							ARRAY_SIZE(vs_cfgs_pineapple_qrd));
					break;
			}
			break;
		case THERM_CLIFFS:
			add_settings(setting, vs_cfgs_cliffs_critical_trips,
					ARRAY_SIZE(vs_cfgs_cliffs_critical_trips));
			break;
		case THERM_PITTI:
			add_settings(setting, vs_cfgs_pitti,
					ARRAY_SIZE(vs_cfgs_pitti));
			add_settings(setting, vs_cfgs_pitti_critical_trips,
					ARRAY_SIZE(vs_cfgs_pitti_critical_trips));
			break;
		case THERM_NIOBE:
			add_settings(setting, vs_cfgs_niobe_critical_trips,
					ARRAY_SIZE(vs_cfgs_niobe_critical_trips));
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QXR:
					add_settings(setting, vs_cfgs_niobe_qxr_trips,
							ARRAY_SIZE(vs_cfgs_niobe_qxr_trips));
					break;
			}
			break;
		case THERM_VOLCANO:
			add_settings(setting, vs_cfgs_volcano,
					ARRAY_SIZE(vs_cfgs_volcano));
			if (therm_get_limit_profile() == THERM_LIMIT_PROFILE_0)
				add_settings(setting, vs_cfgs_volcano_critical_trips,
					ARRAY_SIZE(vs_cfgs_volcano_critical_trips));
			else
				add_settings(setting, vs_cfgs_volcano_itemp_critical_trips,
				ARRAY_SIZE(vs_cfgs_volcano_itemp_critical_trips));

			break;
		case THERM_ANORAK:
			add_settings(setting, vs_cfgs_anorak_critical_trips,
					ARRAY_SIZE(vs_cfgs_anorak_critical_trips));
			break;
		default:
			dbgmsg("%s: No virtual sensors added for this platform\n",
				__func__);
			return;
	}
}
