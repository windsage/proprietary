/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        O V E R _ T H E _ T O P _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ott service Data structures.

  Copyright (c) 2018-2020, 2022 Qualcomm Technologies, Inc. All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Tue Mar 21 2023 (Spin 0)
   From IDL File: over_the_top_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "over_the_top_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t ott_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_bind_subscription_req_msg_v01, subscription)
};

static const uint8_t ott_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * ott_get_bind_subscription_req_msg is empty
 * static const uint8_t ott_get_bind_subscription_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ott_get_bind_subscription_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_bind_subscription_resp_msg_v01, subscription) - QMI_IDL_OFFSET8(ott_get_bind_subscription_resp_msg_v01, subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_bind_subscription_resp_msg_v01, subscription)
};

static const uint8_t ott_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_reporting_status_change) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_reporting_status_change_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_reporting_status_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_info) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_info_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_ul_thrpt_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_reporting_status_change) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_reporting_status_change_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_reporting_status_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_info) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_info_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_dl_thrpt_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_link_latency) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_link_latency_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_link_latency),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_tput_test_result) - QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_tput_test_result_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_indication_register_req_msg_v01, report_tput_test_result)
};

static const uint8_t ott_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ott_configure_uplink_throughput_settings_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_report_period) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_report_period_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_report_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, report_ul_queue_size) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, report_ul_queue_size_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, report_ul_queue_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, rat_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_time_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_mag_chng_size_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_threshold_list),
  QMI_OTT_UL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_throughput_threshold_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_threshold_action) - QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_threshold_action_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_req_msg_v01, ul_threshold_action)
};

static const uint8_t ott_configure_uplink_throughput_settings_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_configure_uplink_throughput_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ott_uplink_throughput_reporting_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, reporting_status) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, reporting_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, reporting_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_interval) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_interval_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, rat_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_time_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_mag_chng_size_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_threshold_list),
  QMI_OTT_UL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_uplink_throughput_reporting_status_ind_msg_v01, actual_ul_throughput_threshold_list_len)
};

/*
 * ott_query_uplink_throughput_reporting_status_req_msg is empty
 * static const uint8_t ott_query_uplink_throughput_reporting_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ott_query_uplink_throughput_reporting_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, reporting_status) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, reporting_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, reporting_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_interval) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_interval_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, rat_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_time_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_mag_chng_size_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_threshold_list),
  QMI_OTT_UL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_reporting_status_resp_msg_v01, actual_ul_throughput_threshold_list_len)
};

static const uint8_t ott_uplink_throughput_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_rate) - QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, confidence_level) - QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, confidence_level_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, confidence_level),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_queue_size) - QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_queue_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_uplink_throughput_info_ind_msg_v01, uplink_queue_size)
};

static const uint8_t ott_query_uplink_throughput_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_req_msg_v01, report_ul_queue_size) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_req_msg_v01, report_ul_queue_size_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_req_msg_v01, report_ul_queue_size)
};

static const uint8_t ott_query_uplink_throughput_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_rate) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, confidence_level) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, confidence_level_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, confidence_level),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_queue_size) - QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_queue_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_uplink_throughput_info_resp_msg_v01, uplink_queue_size)
};

static const uint8_t ott_configure_downlink_throughput_settings_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_report_period) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_report_period_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_report_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, rat_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_time_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_mag_chng_size_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_threshold_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_threshold_list),
  QMI_OTT_DL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_throughput_threshold_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_threshold_action) - QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_threshold_action_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_req_msg_v01, dl_threshold_action)
};

static const uint8_t ott_configure_downlink_throughput_settings_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_configure_downlink_throughput_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ott_downlink_throughput_reporting_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, reporting_status) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, reporting_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, reporting_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_interval) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_interval_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, rat_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_time_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_mag_chng_size_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_threshold_list),
  QMI_OTT_DL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_downlink_throughput_reporting_status_ind_msg_v01, actual_dl_throughput_threshold_list_len)
};

/*
 * ott_query_downlink_throughput_reporting_status_req_msg is empty
 * static const uint8_t ott_query_downlink_throughput_reporting_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ott_query_downlink_throughput_reporting_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, reporting_status) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, reporting_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, reporting_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_interval) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_interval_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, rat_type) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, rat_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_time) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_time_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_mag_chng_size) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_mag_chng_size_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_hyst_mag_chng_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_threshold_list),
  QMI_OTT_UL_THRESHOLD_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_threshold_list) - QMI_IDL_OFFSET8(ott_query_downlink_throughput_reporting_status_resp_msg_v01, actual_dl_throughput_threshold_list_len)
};

static const uint8_t ott_downlink_throughput_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, downlink_rate) - QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, downlink_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, downlink_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, confidence_level) - QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, confidence_level_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, confidence_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, is_suspended) - QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, is_suspended_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, is_suspended),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_lte_percent) - QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_lte_percent_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_lte_percent),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_5g_percent) - QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_5g_percent_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_downlink_throughput_info_ind_msg_v01, split_5g_percent)
};

/*
 * ott_get_downlink_throughput_info_req_msg is empty
 * static const uint8_t ott_get_downlink_throughput_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ott_get_downlink_throughput_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, downlink_rate) - QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, downlink_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, downlink_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, confidence_level) - QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, confidence_level_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, confidence_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_lte_percent) - QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_lte_percent_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_lte_percent),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_5g_percent) - QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_5g_percent_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_get_downlink_throughput_info_resp_msg_v01, split_5g_percent)
};

static const uint8_t ott_get_capabilities_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_capabilities_req_msg_v01, req_capability_mask)
};

static const uint8_t ott_get_capabilities_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, ul_capability) - QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, ul_capability_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, ul_capability),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, dl_capability) - QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, dl_capability_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_capabilities_resp_msg_v01, dl_capability)
};

/*
 * ott_get_link_latency_info_req_msg is empty
 * static const uint8_t ott_get_link_latency_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ott_get_link_latency_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_report_status) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_report_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_report_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_report_status) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_report_status_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_report_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_latency_est) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_latency_est_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_latency_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_latency_est) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_latency_est_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_latency_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, actual_report_interval) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, actual_report_interval_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, actual_report_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_variance_est) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_variance_est_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, ul_link_variance_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_variance_est) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_variance_est_valid)),
  0x16,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, dl_link_variance_est),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, rat) - QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, rat_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_link_latency_info_resp_msg_v01, rat)
};

static const uint8_t ott_link_latency_info_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_report_status),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_report_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_latency_est) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_latency_est_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_latency_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_latency_est) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_latency_est_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_latency_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, actual_report_interval) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, actual_report_interval_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, actual_report_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_variance_est) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_variance_est_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, ul_link_variance_est),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_variance_est) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_variance_est_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, dl_link_variance_est),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, rat) - QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, rat_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_link_latency_info_ind_msg_v01, rat)
};

static const uint8_t ott_get_throughput_test_config_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ip_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv4_addr) - QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv4_addr_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv4_addr),
  QMI_IDL_TYPE88(2, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv6_addr) - QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv6_addr_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_req_msg_v01, ipv6_addr),
  QMI_IDL_TYPE88(2, 7)
};

static const uint8_t ott_get_throughput_test_config_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, mtu) - QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, mtu_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, mtu),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, ul_configured_throughput) - QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, ul_configured_throughput_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, ul_configured_throughput),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, dl_configured_throughput) - QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, dl_configured_throughput_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_get_throughput_test_config_info_resp_msg_v01, dl_configured_throughput)
};

static const uint8_t ott_throughput_test_config_param_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, test_duration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_max_count) - QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_max_count_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_max_count),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_interval) - QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_interval_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, retry_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, ul_filter_list) - QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, ul_filter_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, ul_filter_list),
  QMI_OTT_FILTER_LIST_LEN_V01,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, ul_filter_list) - QMI_IDL_OFFSET8(ott_throughput_test_config_param_req_msg_v01, ul_filter_list_len),
  QMI_IDL_TYPE88(2, 15),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, ul_packet_payload) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, ul_packet_payload_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ott_throughput_test_config_param_req_msg_v01, ul_packet_payload),
  QMI_OTT_PACKET_PAYLOAD_MAX_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, ul_packet_payload) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, ul_packet_payload_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, dl_filter_list) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, dl_filter_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ott_throughput_test_config_param_req_msg_v01, dl_filter_list),
  QMI_OTT_FILTER_LIST_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, dl_filter_list) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, dl_filter_list_len),
  QMI_IDL_TYPE88(2, 15),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, server_packet_payload) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, server_packet_payload_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ott_throughput_test_config_param_req_msg_v01, server_packet_payload),
  QMI_OTT_PACKET_PAYLOAD_MAX_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, server_packet_payload) - QMI_IDL_OFFSET16RELATIVE(ott_throughput_test_config_param_req_msg_v01, server_packet_payload_len)
};

static const uint8_t ott_throughput_test_config_param_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_throughput_test_config_param_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ott_set_throughput_test_operation_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, ul_action) - QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, ul_action_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, ul_action),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, dl_action) - QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, dl_action_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_req_msg_v01, dl_action)
};

static const uint8_t ott_set_throughput_test_operation_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_set_throughput_test_operation_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ott_throughput_test_result_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, fail_reason) - QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, fail_reason_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, fail_reason),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, uplink_tput_info_list) - QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, uplink_tput_info_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, uplink_tput_info_list),
  QMI_OTT_UPLINK_TPUT_INFO_LIST_V01,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, uplink_tput_info_list) - QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, uplink_tput_info_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, downlink_tput_info_list) - QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, downlink_tput_info_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, downlink_tput_info_list),
  QMI_OTT_DOWNLINK_TPUT_INFO_LIST_V01,
  QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, downlink_tput_info_list) - QMI_IDL_OFFSET8(ott_throughput_test_result_ind_msg_v01, downlink_tput_info_list_len)
};

static const uint8_t ott_config_link_latency_report_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ott_config_link_latency_report_req_msg_v01, report_interval) - QMI_IDL_OFFSET8(ott_config_link_latency_report_req_msg_v01, report_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ott_config_link_latency_report_req_msg_v01, report_interval)
};

static const uint8_t ott_config_link_latency_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ott_config_link_latency_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry ott_message_table_v01[] = {
  {sizeof(ott_bind_subscription_req_msg_v01), ott_bind_subscription_req_msg_data_v01},
  {sizeof(ott_bind_subscription_resp_msg_v01), ott_bind_subscription_resp_msg_data_v01},
  {sizeof(ott_get_bind_subscription_req_msg_v01), 0},
  {sizeof(ott_get_bind_subscription_resp_msg_v01), ott_get_bind_subscription_resp_msg_data_v01},
  {sizeof(ott_indication_register_req_msg_v01), ott_indication_register_req_msg_data_v01},
  {sizeof(ott_indication_register_resp_msg_v01), ott_indication_register_resp_msg_data_v01},
  {sizeof(ott_configure_uplink_throughput_settings_req_msg_v01), ott_configure_uplink_throughput_settings_req_msg_data_v01},
  {sizeof(ott_configure_uplink_throughput_settings_resp_msg_v01), ott_configure_uplink_throughput_settings_resp_msg_data_v01},
  {sizeof(ott_uplink_throughput_reporting_status_ind_msg_v01), ott_uplink_throughput_reporting_status_ind_msg_data_v01},
  {sizeof(ott_query_uplink_throughput_reporting_status_req_msg_v01), 0},
  {sizeof(ott_query_uplink_throughput_reporting_status_resp_msg_v01), ott_query_uplink_throughput_reporting_status_resp_msg_data_v01},
  {sizeof(ott_uplink_throughput_info_ind_msg_v01), ott_uplink_throughput_info_ind_msg_data_v01},
  {sizeof(ott_query_uplink_throughput_info_req_msg_v01), ott_query_uplink_throughput_info_req_msg_data_v01},
  {sizeof(ott_query_uplink_throughput_info_resp_msg_v01), ott_query_uplink_throughput_info_resp_msg_data_v01},
  {sizeof(ott_configure_downlink_throughput_settings_req_msg_v01), ott_configure_downlink_throughput_settings_req_msg_data_v01},
  {sizeof(ott_configure_downlink_throughput_settings_resp_msg_v01), ott_configure_downlink_throughput_settings_resp_msg_data_v01},
  {sizeof(ott_downlink_throughput_reporting_status_ind_msg_v01), ott_downlink_throughput_reporting_status_ind_msg_data_v01},
  {sizeof(ott_query_downlink_throughput_reporting_status_req_msg_v01), 0},
  {sizeof(ott_query_downlink_throughput_reporting_status_resp_msg_v01), ott_query_downlink_throughput_reporting_status_resp_msg_data_v01},
  {sizeof(ott_downlink_throughput_info_ind_msg_v01), ott_downlink_throughput_info_ind_msg_data_v01},
  {sizeof(ott_get_downlink_throughput_info_req_msg_v01), 0},
  {sizeof(ott_get_downlink_throughput_info_resp_msg_v01), ott_get_downlink_throughput_info_resp_msg_data_v01},
  {sizeof(ott_get_capabilities_req_msg_v01), ott_get_capabilities_req_msg_data_v01},
  {sizeof(ott_get_capabilities_resp_msg_v01), ott_get_capabilities_resp_msg_data_v01},
  {sizeof(ott_get_link_latency_info_req_msg_v01), 0},
  {sizeof(ott_get_link_latency_info_resp_msg_v01), ott_get_link_latency_info_resp_msg_data_v01},
  {sizeof(ott_link_latency_info_ind_msg_v01), ott_link_latency_info_ind_msg_data_v01},
  {sizeof(ott_get_throughput_test_config_info_req_msg_v01), ott_get_throughput_test_config_info_req_msg_data_v01},
  {sizeof(ott_get_throughput_test_config_info_resp_msg_v01), ott_get_throughput_test_config_info_resp_msg_data_v01},
  {sizeof(ott_throughput_test_config_param_req_msg_v01), ott_throughput_test_config_param_req_msg_data_v01},
  {sizeof(ott_throughput_test_config_param_resp_msg_v01), ott_throughput_test_config_param_resp_msg_data_v01},
  {sizeof(ott_set_throughput_test_operation_mode_req_msg_v01), ott_set_throughput_test_operation_mode_req_msg_data_v01},
  {sizeof(ott_set_throughput_test_operation_mode_resp_msg_v01), ott_set_throughput_test_operation_mode_resp_msg_data_v01},
  {sizeof(ott_throughput_test_result_ind_msg_v01), ott_throughput_test_result_ind_msg_data_v01},
  {sizeof(ott_config_link_latency_report_req_msg_v01), ott_config_link_latency_report_req_msg_data_v01},
  {sizeof(ott_config_link_latency_report_resp_msg_v01), ott_config_link_latency_report_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ott_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ott_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ott_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ott_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(ott_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  ott_message_table_v01,
  ott_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ott_service_command_messages_v01[] = {
  {QMI_OTT_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 0), 7},
  {QMI_OTT_GET_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_OTT_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 4), 24},
  {QMI_OTT_CONFIGURE_UPLINK_THROUGHPUT_SETTINGS_REQ_V01, QMI_IDL_TYPE16(0, 6), 123},
  {QMI_OTT_QUERY_UPLINK_THROUGHPUT_REPORTING_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 9), 0},
  {QMI_OTT_QUERY_UPLINK_THROUGHPUT_INFO_REQ_V01, QMI_IDL_TYPE16(0, 12), 4},
  {QMI_OTT_CONFIGURE_DOWNLINK_THROUGHPUT_SETTINGS_REQ_V01, QMI_IDL_TYPE16(0, 14), 119},
  {QMI_OTT_QUERY_DOWNLINK_THROUGHPUT_REPORTING_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 17), 0},
  {QMI_OTT_GET_DOWNLINK_THROUGHPUT_INFO_REQ_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_OTT_GET_CAPABILITIES_REQ_V01, QMI_IDL_TYPE16(0, 22), 11},
  {QMI_OTT_GET_LINK_LATENCY_INFO_REQ_V01, QMI_IDL_TYPE16(0, 24), 0},
  {QMI_OTT_GET_THROUGHPUT_TEST_CONFIG_INFO_REQ_V01, QMI_IDL_TYPE16(0, 27), 38},
  {QMI_OTT_THROUGHPUT_TEST_CONFIG_PARAM_REQ_V01, QMI_IDL_TYPE16(0, 29), 1731},
  {QMI_OTT_SET_THROUGHPUT_TEST_OPERATION_MODE_REQ_V01, QMI_IDL_TYPE16(0, 31), 14},
  {QMI_OTT_CONFIG_LINK_LATENCY_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 34), 7}
};

static const qmi_idl_service_message_table_entry ott_service_response_messages_v01[] = {
  {QMI_OTT_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_OTT_GET_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_OTT_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_OTT_CONFIGURE_UPLINK_THROUGHPUT_SETTINGS_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_OTT_QUERY_UPLINK_THROUGHPUT_REPORTING_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 10), 126},
  {QMI_OTT_QUERY_UPLINK_THROUGHPUT_INFO_RESP_V01, QMI_IDL_TYPE16(0, 13), 25},
  {QMI_OTT_CONFIGURE_DOWNLINK_THROUGHPUT_SETTINGS_RESP_V01, QMI_IDL_TYPE16(0, 15), 7},
  {QMI_OTT_QUERY_DOWNLINK_THROUGHPUT_REPORTING_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 18), 126},
  {QMI_OTT_GET_DOWNLINK_THROUGHPUT_INFO_RESP_V01, QMI_IDL_TYPE16(0, 21), 26},
  {QMI_OTT_GET_CAPABILITIES_RESP_V01, QMI_IDL_TYPE16(0, 23), 29},
  {QMI_OTT_GET_LINK_LATENCY_INFO_RESP_V01, QMI_IDL_TYPE16(0, 25), 73},
  {QMI_OTT_GET_THROUGHPUT_TEST_CONFIG_INFO_RESP_V01, QMI_IDL_TYPE16(0, 28), 28},
  {QMI_OTT_THROUGHPUT_TEST_CONFIG_PARAM_RESP_V01, QMI_IDL_TYPE16(0, 30), 7},
  {QMI_OTT_SET_THROUGHPUT_TEST_OPERATION_MODE_RESP_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_OTT_CONFIG_LINK_LATENCY_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 35), 7}
};

static const qmi_idl_service_message_table_entry ott_service_indication_messages_v01[] = {
  {QMI_OTT_UPLINK_THROUGHPUT_REPORTING_STATUS_IND_V01, QMI_IDL_TYPE16(0, 8), 119},
  {QMI_OTT_UPLINK_THROUGHPUT_INFO_IND_V01, QMI_IDL_TYPE16(0, 11), 18},
  {QMI_OTT_DOWNLINK_THROUGHPUT_REPORTING_STATUS_IND_V01, QMI_IDL_TYPE16(0, 16), 119},
  {QMI_OTT_DOWNLINK_THROUGHPUT_INFO_IND_V01, QMI_IDL_TYPE16(0, 19), 23},
  {QMI_OTT_LINK_LATENTCY_INFO_IND_V01, QMI_IDL_TYPE16(0, 26), 66},
  {QMI_OTT_THROUGHPUT_TEST_RESULT_IND_V01, QMI_IDL_TYPE16(0, 33), 278}
};

/*Service Object*/
struct qmi_idl_service_object ott_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x44,
  1731,
  { sizeof(ott_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ott_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ott_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ott_service_command_messages_v01, ott_service_response_messages_v01, ott_service_indication_messages_v01},
  &ott_qmi_idl_type_table_object_v01,
  0x07,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ott_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( OTT_V01_IDL_MAJOR_VERS != idl_maj_version || OTT_V01_IDL_MINOR_VERS != idl_min_version
       || OTT_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ott_qmi_idl_service_object_v01;
}
