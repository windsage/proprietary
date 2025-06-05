/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        N E T W O R K _ A C C E S S _ S E R V I C E _ E X T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the nas_ext service Data structures.

  Copyright (c) 2013-2022 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.5/nas_ext/src/network_access_service_ext_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Thu Jun 23 2022 (Spin 0)
   From IDL File: network_access_service_ext_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "network_access_service_ext_v01.h"
#include "common_v01.h"
#include "network_access_service_common_v01.h"


/*Type Definitions*/
static const uint8_t nas_subscription_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_subscription_info_type_v01, subs_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_type_v01, ph_cmd),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t nas_set_e911_state_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, action),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, redial) - QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, redial_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, redial),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_mode_pref) - QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_mode_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, elenl_info) - QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, elenl_info_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, elenl_info),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_type) - QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_type_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_req_msg_v01, emerg_type)
};

static const uint8_t nas_set_e911_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_e911_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_e911_state_resp_msg_v01, emerg_type) - QMI_IDL_OFFSET8(nas_set_e911_state_resp_msg_v01, emerg_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_e911_state_resp_msg_v01, emerg_type)
};

static const uint8_t nas_e911_state_ready_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, is_ready_due_to_csfb) - QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, is_ready_due_to_csfb_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, is_ready_due_to_csfb),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, emerg_type) - QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, emerg_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_e911_state_ready_ind_msg_v01, emerg_type)
};

static const uint8_t nas_subscription_change_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_change_ind_msg_v01, subscription_info) - QMI_IDL_OFFSET8(nas_subscription_change_ind_msg_v01, subscription_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_subscription_change_ind_msg_v01, subscription_info),
  QMI_IDL_TYPE88(0, 0)
};

/*
 * nas_ims_proceed_with_subscription_change_req_msg is empty
 * static const uint8_t nas_ims_proceed_with_subscription_change_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_ims_proceed_with_subscription_change_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ims_proceed_with_subscription_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_t3346_timer_status_change_ind_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_t3346_timer_status_change_ind_v01, timer_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_t3346_timer_status_change_ind_v01, radio_access_technology) - QMI_IDL_OFFSET8(nas_t3346_timer_status_change_ind_v01, radio_access_technology_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_t3346_timer_status_change_ind_v01, radio_access_technology)
};

static const uint8_t nas_ims_call_state_nofification_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_type) - QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, sys_mode) - QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, sys_mode_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, sys_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_direction) - QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_direction_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_direction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_end_reason) - QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_end_reason_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_end_reason),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_termination) - QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_termination_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ims_call_state_nofification_req_msg_v01, call_termination)
};

static const uint8_t nas_ims_call_state_notification_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ims_call_state_notification_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_call_mode_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_call_mode_ind_v01, call_mode)
};

/*
 * nas_get_call_mode_req_msg is empty
 * static const uint8_t nas_get_call_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_call_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_call_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_call_mode_resp_msg_v01, call_mode) - QMI_IDL_OFFSET8(nas_get_call_mode_resp_msg_v01, call_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_call_mode_resp_msg_v01, call_mode)
};

static const uint8_t nas_volte_state_notification_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_volte_state_notification_req_msg_v01, is_volte_enabled)
};

static const uint8_t nas_volte_state_notification_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_volte_state_notification_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_e911_search_fail_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_e911_search_fail_ind_msg_v01, emerg_mode_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_e911_search_fail_ind_msg_v01, emerg_type) - QMI_IDL_OFFSET8(nas_e911_search_fail_ind_msg_v01, emerg_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_e911_search_fail_ind_msg_v01, emerg_type)
};

static const uint8_t nas_voims_state_notification_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_voims_state_notification_req_msg_v01, volte_is_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_voims_state_notification_req_msg_v01, vonr_is_enabled)
};

static const uint8_t nas_voims_state_notification_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_voims_state_notification_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_call_pref_change_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_call_pref_change_req_msg_v01, mode_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_call_pref_change_req_msg_v01, call_type)
};

static const uint8_t nas_call_pref_change_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_call_pref_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_call_pref_change_resp_msg_v01, concurrent_call_not_possible) - QMI_IDL_OFFSET8(nas_call_pref_change_resp_msg_v01, concurrent_call_not_possible_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_call_pref_change_resp_msg_v01, concurrent_call_not_possible)
};

static const uint8_t nas_call_ready_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_call_ready_ind_msg_v01, sys_mode)
};

static const uint8_t nas_mmtel_response_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_mmtel_response_ind_msg_v01, mmtel_response),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_mmtel_response_ind_msg_v01, call_type),

  0x03,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_mmtel_response_ind_msg_v01, sys_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_mmtel_response_ind_msg_v01, barring_time)
};

static const uint8_t nas_uac_barring_alleviation_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_uac_barring_alleviation_ind_msg_v01, call_type)
};

/*
 * nas_get_e911_sub_req_msg is empty
 * static const uint8_t nas_get_e911_sub_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_e911_sub_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_e911_sub_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_e911_sub_resp_msg_v01, e911_sub) - QMI_IDL_OFFSET8(nas_get_e911_sub_resp_msg_v01, e911_sub_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_e911_sub_resp_msg_v01, e911_sub)
};

static const uint8_t nas_wifi_ho_deprioritize_nr5g_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wifi_ho_deprioritize_nr5g_req_msg_v01, deprioritize_nr5g)
};

static const uint8_t nas_wifi_ho_deprioritize_nr5g_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_wifi_ho_deprioritize_nr5g_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_emerg_avoid_plmn_rat_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, plmn) - QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, plmn_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, rat_mask) - QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, rat_mask_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_req_msg_v01, rat_mask)
};

static const uint8_t nas_emerg_avoid_plmn_rat_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_emerg_avoid_plmn_rat_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_mo_ims_reg_uac_barring_alleviation_ind_msg is empty
 * static const uint8_t nas_mo_ims_reg_uac_barring_alleviation_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_notify_nw_congestion_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, nw_congestion) - QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, nw_congestion_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, nw_congestion),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, timer_value) - QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, timer_value_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_notify_nw_congestion_ind_msg_v01, timer_value)
};

static const uint8_t nas_start_ims_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_start_ims_reg_req_msg_v01, sys_mode)
};

static const uint8_t nas_start_ims_reg_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, timer_value) - QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, timer_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, timer_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, status) - QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_start_ims_reg_resp_msg_v01, status)
};

static const uint8_t nas_stop_ims_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_stop_ims_reg_req_msg_v01, sys_mode)
};

static const uint8_t nas_stop_ims_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_stop_ims_reg_resp_msg_v01, result),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_force_lte_fallback_req_msg is empty
 * static const uint8_t nas_force_lte_fallback_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_force_lte_fallback_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_force_lte_fallback_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_vonr_not_possible_ind_msg is empty
 * static const uint8_t nas_vonr_not_possible_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_e911_sms_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_e911_sms_status_ind_msg_v01, sms_ready) - QMI_IDL_OFFSET8(nas_e911_sms_status_ind_msg_v01, sms_ready_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_e911_sms_status_ind_msg_v01, sms_ready)
};

/* Type Table */
static const qmi_idl_type_table_entry  nas_ext_type_table_v01[] = {
  {sizeof(nas_subscription_info_type_v01), nas_subscription_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry nas_ext_message_table_v01[] = {
  {sizeof(nas_set_e911_state_req_msg_v01), nas_set_e911_state_req_msg_data_v01},
  {sizeof(nas_set_e911_state_resp_msg_v01), nas_set_e911_state_resp_msg_data_v01},
  {sizeof(nas_e911_state_ready_ind_msg_v01), nas_e911_state_ready_ind_msg_data_v01},
  {sizeof(nas_subscription_change_ind_msg_v01), nas_subscription_change_ind_msg_data_v01},
  {sizeof(nas_ims_proceed_with_subscription_change_req_msg_v01), 0},
  {sizeof(nas_ims_proceed_with_subscription_change_resp_msg_v01), nas_ims_proceed_with_subscription_change_resp_msg_data_v01},
  {sizeof(nas_t3346_timer_status_change_ind_v01), nas_t3346_timer_status_change_ind_data_v01},
  {sizeof(nas_ims_call_state_nofification_req_msg_v01), nas_ims_call_state_nofification_req_msg_data_v01},
  {sizeof(nas_ims_call_state_notification_resp_msg_v01), nas_ims_call_state_notification_resp_msg_data_v01},
  {sizeof(nas_call_mode_ind_v01), nas_call_mode_ind_data_v01},
  {sizeof(nas_get_call_mode_req_msg_v01), 0},
  {sizeof(nas_get_call_mode_resp_msg_v01), nas_get_call_mode_resp_msg_data_v01},
  {sizeof(nas_volte_state_notification_req_msg_v01), nas_volte_state_notification_req_msg_data_v01},
  {sizeof(nas_volte_state_notification_resp_msg_v01), nas_volte_state_notification_resp_msg_data_v01},
  {sizeof(nas_e911_search_fail_ind_msg_v01), nas_e911_search_fail_ind_msg_data_v01},
  {sizeof(nas_voims_state_notification_req_msg_v01), nas_voims_state_notification_req_msg_data_v01},
  {sizeof(nas_voims_state_notification_resp_msg_v01), nas_voims_state_notification_resp_msg_data_v01},
  {sizeof(nas_call_pref_change_req_msg_v01), nas_call_pref_change_req_msg_data_v01},
  {sizeof(nas_call_pref_change_resp_msg_v01), nas_call_pref_change_resp_msg_data_v01},
  {sizeof(nas_call_ready_ind_msg_v01), nas_call_ready_ind_msg_data_v01},
  {sizeof(nas_mmtel_response_ind_msg_v01), nas_mmtel_response_ind_msg_data_v01},
  {sizeof(nas_uac_barring_alleviation_ind_msg_v01), nas_uac_barring_alleviation_ind_msg_data_v01},
  {sizeof(nas_get_e911_sub_req_msg_v01), 0},
  {sizeof(nas_get_e911_sub_resp_msg_v01), nas_get_e911_sub_resp_msg_data_v01},
  {sizeof(nas_wifi_ho_deprioritize_nr5g_req_msg_v01), nas_wifi_ho_deprioritize_nr5g_req_msg_data_v01},
  {sizeof(nas_wifi_ho_deprioritize_nr5g_resp_msg_v01), nas_wifi_ho_deprioritize_nr5g_resp_msg_data_v01},
  {sizeof(nas_emerg_avoid_plmn_rat_req_msg_v01), nas_emerg_avoid_plmn_rat_req_msg_data_v01},
  {sizeof(nas_emerg_avoid_plmn_rat_resp_msg_v01), nas_emerg_avoid_plmn_rat_resp_msg_data_v01},
  {sizeof(nas_mo_ims_reg_uac_barring_alleviation_ind_msg_v01), 0},
  {sizeof(nas_notify_nw_congestion_ind_msg_v01), nas_notify_nw_congestion_ind_msg_data_v01},
  {sizeof(nas_start_ims_reg_req_msg_v01), nas_start_ims_reg_req_msg_data_v01},
  {sizeof(nas_start_ims_reg_resp_msg_v01), nas_start_ims_reg_resp_msg_data_v01},
  {sizeof(nas_stop_ims_reg_req_msg_v01), nas_stop_ims_reg_req_msg_data_v01},
  {sizeof(nas_stop_ims_reg_resp_msg_v01), nas_stop_ims_reg_resp_msg_data_v01},
  {sizeof(nas_force_lte_fallback_req_msg_v01), 0},
  {sizeof(nas_force_lte_fallback_resp_msg_v01), nas_force_lte_fallback_resp_msg_data_v01},
  {sizeof(nas_vonr_not_possible_ind_msg_v01), 0},
  {sizeof(nas_e911_sms_status_ind_msg_v01), nas_e911_sms_status_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object nas_ext_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *nas_ext_qmi_idl_type_table_object_referenced_tables_v01[] =
{&nas_ext_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &network_access_service_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object nas_ext_qmi_idl_type_table_object_v01 = {
  sizeof(nas_ext_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(nas_ext_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  nas_ext_type_table_v01,
  nas_ext_message_table_v01,
  nas_ext_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry nas_ext_service_command_messages_v01[] = {
  {QMI_NAS_SET_E911_STATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 0), 30},
  {QMI_NAS_IMS_PROCEED_WITH_SUBSCRIPTION_CHANGE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_NAS_IMS_CALL_STATE_NOTIFICATION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 7), 40},
  {QMI_NAS_GET_CALL_MODE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 10), 0},
  {QMI_NAS_VOLTE_STATE_NOTIFICATION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 12), 4},
  {QMI_NAS_VOIMS_STATE_NOTIFICATION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 15), 8},
  {QMI_NAS_CALL_PREF_CHANGE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 17), 16},
  {QMI_NAS_GET_E911_SUB_REQ_MSG_V01, QMI_IDL_TYPE16(0, 22), 0},
  {QMI_NAS_WIFI_HO_DEPRIORITIZE_NR5G_REQ_MSG_V01, QMI_IDL_TYPE16(0, 24), 4},
  {QMI_NAS_EMERG_AVOID_PLMN_RAT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 26), 19},
  {QMI_NAS_START_IMS_REG_REQ_V01, QMI_IDL_TYPE16(0, 30), 4},
  {QMI_NAS_STOP_IMS_REG_REQ_V01, QMI_IDL_TYPE16(0, 32), 4},
  {QMI_NAS_FORCE_LTE_FALLBACK_REQ_MSG_V01, QMI_IDL_TYPE16(0, 34), 0}
};

static const qmi_idl_service_message_table_entry nas_ext_service_response_messages_v01[] = {
  {QMI_NAS_SET_E911_STATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_NAS_IMS_PROCEED_WITH_SUBSCRIPTION_CHANGE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_NAS_IMS_CALL_STATE_NOTIFICATION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_NAS_GET_CALL_MODE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 11), 14},
  {QMI_NAS_VOLTE_STATE_NOTIFICATION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_NAS_VOIMS_STATE_NOTIFICATION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 16), 7},
  {QMI_NAS_CALL_PREF_CHANGE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 18), 11},
  {QMI_NAS_GET_E911_SUB_RESP_MSG_V01, QMI_IDL_TYPE16(0, 23), 11},
  {QMI_NAS_WIFI_HO_DEPRIORITIZE_NR5G_RESP_MSG_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_NAS_EMERG_AVOID_PLMN_RAT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_NAS_START_IMS_REG_RESP_V01, QMI_IDL_TYPE16(0, 31), 21},
  {QMI_NAS_STOP_IMS_REG_RESP_V01, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_NAS_FORCE_LTE_FALLBACK_RESP_MSG_V01, QMI_IDL_TYPE16(0, 35), 7}
};

static const qmi_idl_service_message_table_entry nas_ext_service_indication_messages_v01[] = {
  {QMI_NAS_E911_STATE_READY_IND_V01, QMI_IDL_TYPE16(0, 2), 11},
  {QMI_NAS_SUBSCRIPTION_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 3), 8},
  {QMI_NAS_T3346_TIMER_STATUS_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 6), 11},
  {QMI_NAS_CALL_MODE_IND_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_NAS_CALL_READY_IND_V01, QMI_IDL_TYPE16(0, 19), 4},
  {QMI_NAS_MMTEL_RESPONSE_IND_V01, QMI_IDL_TYPE16(0, 20), 29},
  {QMI_NAS_UAC_BARRING_ALLEVIATION_IND_V01, QMI_IDL_TYPE16(0, 21), 11},
  {QMI_NAS_E911_SEARCH_FAIL_IND_V01, QMI_IDL_TYPE16(0, 14), 12},
  {QMI_NAS_MO_IMS_REG_UAC_BARRING_ALLEVIATION_IND_V01, QMI_IDL_TYPE16(0, 28), 0},
  {QMI_NAS_NOTIFY_NW_CONGESTION_IND_V01, QMI_IDL_TYPE16(0, 29), 11},
  {QMI_NAS_VONR_NOT_POSSIBLE_IND_V01, QMI_IDL_TYPE16(0, 36), 0},
  {QMI_NAS_E911_SMS_STATUS_IND_V01, QMI_IDL_TYPE16(0, 37), 4}
};

/*Service Object*/
struct qmi_idl_service_object nas_ext_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x03,
  40,
  { sizeof(nas_ext_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nas_ext_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nas_ext_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { nas_ext_service_command_messages_v01, nas_ext_service_response_messages_v01, nas_ext_service_indication_messages_v01},
  &nas_ext_qmi_idl_type_table_object_v01,
  0x1D,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type nas_ext_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( NAS_EXT_V01_IDL_MAJOR_VERS != idl_maj_version || NAS_EXT_V01_IDL_MINOR_VERS != idl_min_version
       || NAS_EXT_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&nas_ext_qmi_idl_service_object_v01;
}

