/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        A N T E N N A _ S W I T C H _ I N T E R F A C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the antswitch service Data structures.

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.0/antswitch/src/antenna_switch_interface_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9
   It was generated on: Tue Oct  1 2019 (Spin 0)
   From IDL File: antenna_switch_interface_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "antenna_switch_interface_v01.h"
#include "common_v01.h"
#include "system_common_v01.h"


/*Type Definitions*/
static const uint8_t antswitch_config_info_per_tech_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_config_info_per_tech_type_v01, tech_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_config_info_per_tech_type_v01, carrier_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_config_info_per_tech_type_v01, config),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_config_info_per_tech_type_v01, band),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t antswitch_bands_bitmask_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(antswitch_bands_bitmask_type_v01, mask),
  14,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ant_switch_config_change_ind_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ant_switch_config_change_ind_info_type_v01, tech_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ant_switch_config_change_ind_info_type_v01, carrier_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ant_switch_config_change_ind_info_type_v01, excep_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ant_switch_config_change_ind_info_type_v01, band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ant_switch_config_change_ind_info_type_v01, config),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t antswitch_set_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_set_config_req_msg_v01, ant_switch_config)
};

static const uint8_t antswitch_set_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_set_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t antswitch_get_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(antswitch_get_config_req_msg_v01, tech_id) - QMI_IDL_OFFSET8(antswitch_get_config_req_msg_v01, tech_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_get_config_req_msg_v01, tech_id)
};

static const uint8_t antswitch_get_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, antswitch_cfg) - QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, antswitch_cfg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, antswitch_cfg),
  ANTSWITCH_MAX_MODEM_ACTIVE_TECHS_V01,
  QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, antswitch_cfg) - QMI_IDL_OFFSET8(antswitch_get_config_resp_msg_v01, antswitch_cfg_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t antswitch_enh_set_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_enh_set_config_req_msg_v01, ant_switch_config),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_enh_set_config_req_msg_v01, bands_bitmask),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(antswitch_enh_set_config_req_msg_v01, operation)
};

static const uint8_t antswitch_enh_set_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_enh_set_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t antswitch_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(antswitch_indication_register_req_msg_v01, enable_config_change_ind) - QMI_IDL_OFFSET8(antswitch_indication_register_req_msg_v01, enable_config_change_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(antswitch_indication_register_req_msg_v01, enable_config_change_ind)
};

static const uint8_t antswitch_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t antswitch_config_change_ind_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_config_change_ind_v01, new_config_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(antswitch_config_change_ind_v01, old_config_info),
  QMI_IDL_TYPE88(0, 2)
};

/* Type Table */
static const qmi_idl_type_table_entry  antswitch_type_table_v01[] = {
  {sizeof(antswitch_config_info_per_tech_type_v01), antswitch_config_info_per_tech_type_data_v01},
  {sizeof(antswitch_bands_bitmask_type_v01), antswitch_bands_bitmask_type_data_v01},
  {sizeof(ant_switch_config_change_ind_info_type_v01), ant_switch_config_change_ind_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry antswitch_message_table_v01[] = {
  {sizeof(antswitch_set_config_req_msg_v01), antswitch_set_config_req_msg_data_v01},
  {sizeof(antswitch_set_config_resp_msg_v01), antswitch_set_config_resp_msg_data_v01},
  {sizeof(antswitch_get_config_req_msg_v01), antswitch_get_config_req_msg_data_v01},
  {sizeof(antswitch_get_config_resp_msg_v01), antswitch_get_config_resp_msg_data_v01},
  {sizeof(antswitch_enh_set_config_req_msg_v01), antswitch_enh_set_config_req_msg_data_v01},
  {sizeof(antswitch_enh_set_config_resp_msg_v01), antswitch_enh_set_config_resp_msg_data_v01},
  {sizeof(antswitch_indication_register_req_msg_v01), antswitch_indication_register_req_msg_data_v01},
  {sizeof(antswitch_indication_register_resp_msg_v01), antswitch_indication_register_resp_msg_data_v01},
  {sizeof(antswitch_config_change_ind_v01), antswitch_config_change_ind_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object antswitch_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *antswitch_qmi_idl_type_table_object_referenced_tables_v01[] =
{&antswitch_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &system_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object antswitch_qmi_idl_type_table_object_v01 = {
  sizeof(antswitch_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(antswitch_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  antswitch_type_table_v01,
  antswitch_message_table_v01,
  antswitch_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry antswitch_service_command_messages_v01[] = {
  {QMI_ANTSWITCH_SET_ANT_SWITCH_STATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 0), 7},
  {QMI_ANTSWITCH_GET_ANT_SWITCH_STATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_ANTSWITCH_ENH_SET_ANT_SWITCH_STATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 4), 129},
  {QMI_ANTSWITCH_ENABLE_CONFIG_CHANGE_IND_REQ_MSG_V01, QMI_IDL_TYPE16(0, 6), 4}
};

static const qmi_idl_service_message_table_entry antswitch_service_response_messages_v01[] = {
  {QMI_ANTSWITCH_SET_ANT_SWITCH_STATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_ANTSWITCH_GET_ANT_SWITCH_STATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 3), 523},
  {QMI_ANTSWITCH_ENH_SET_ANT_SWITCH_STATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_ANTSWITCH_ENABLE_CONFIG_CHANGE_IND_RESP_MSG_V01, QMI_IDL_TYPE16(0, 7), 7}
};

static const qmi_idl_service_message_table_entry antswitch_service_indication_messages_v01[] = {
  {QMI_ANTSWITCH_CONFIG_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 8), 46}
};

/*Service Object*/
struct qmi_idl_service_object antswitch_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x4A,
  523,
  { sizeof(antswitch_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(antswitch_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(antswitch_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { antswitch_service_command_messages_v01, antswitch_service_response_messages_v01, antswitch_service_indication_messages_v01},
  &antswitch_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type antswitch_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( ANTSWITCH_V01_IDL_MAJOR_VERS != idl_maj_version || ANTSWITCH_V01_IDL_MINOR_VERS != idl_min_version
       || ANTSWITCH_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&antswitch_qmi_idl_service_object_v01;
}

