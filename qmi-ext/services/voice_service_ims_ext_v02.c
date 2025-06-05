/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        V O I C E _ S E R V I C E _ I M S _ E X T _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the voice_ims_ext service Data structures.

  Copyright (c) 2013-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.HI.1.0.r30/Main/modem_proc/qmimsgs/voice_ext/src/voice_service_ims_ext_v02.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.4 
   It was generated on: Fri May  1 2015 (Spin 1)
   From IDL File: voice_service_ims_ext_v02.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "voice_service_ims_ext_v02.h"
#include "common_v01.h"
#include "voice_service_common_v02.h"


/*Type Definitions*/
static const uint8_t voice_srvcc_call_context_type_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, instance_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, call_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, call_state),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, call_substate),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, is_mpty),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, direction),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_type_v02, calling_number),
  QMI_VOICE_NUMBER_MAX_V02,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t voice_srvcc_alerting_info_type_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_alerting_info_type_v02, instance_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_alerting_info_type_v02, alerting_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t voice_srvcc_call_context_name_type_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_name_type_v02, instance_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_name_type_v02, name_pi),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_name_type_v02, caller_name_utf16),
  QMI_VOICE_CALLER_NAME_MAX_V02,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_name_type_v02, caller_name_utf16) - QMI_IDL_OFFSET8(voice_srvcc_call_context_name_type_v02, caller_name_utf16_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t voice_srvcc_call_context_num_pi_type_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_num_pi_type_v02, instance_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(voice_srvcc_call_context_num_pi_type_v02, pi),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t voice_srvcc_call_config_req_msg_data_v02[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(voice_srvcc_call_config_req_msg_v02, srvcc_call_context),
  QMI_VOICE_SRVCC_CALL_CONTEXT_ARRAY_MAX_V02,
  QMI_IDL_OFFSET8(voice_srvcc_call_config_req_msg_v02, srvcc_call_context) - QMI_IDL_OFFSET8(voice_srvcc_call_config_req_msg_v02, srvcc_call_context_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_alerting_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_alerting_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(voice_srvcc_call_config_req_msg_v02, srvcc_alerting_info),
  QMI_VOICE_SRVCC_CALL_CONTEXT_ARRAY_MAX_V02,
  QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_alerting_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_alerting_info_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_num_pi_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_num_pi_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(voice_srvcc_call_config_req_msg_v02, srvcc_num_pi_info),
  QMI_VOICE_SRVCC_CALL_CONTEXT_ARRAY_MAX_V02,
  QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_num_pi_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_num_pi_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_caller_name_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_caller_name_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(voice_srvcc_call_config_req_msg_v02, srvcc_caller_name_info),
  QMI_VOICE_SRVCC_CALL_CONTEXT_ARRAY_MAX_V02,
  QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_caller_name_info) - QMI_IDL_OFFSET16RELATIVE(voice_srvcc_call_config_req_msg_v02, srvcc_caller_name_info_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t voice_srvcc_call_config_resp_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(voice_srvcc_call_config_resp_msg_v02, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t voice_e911_orig_fail_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(voice_e911_orig_fail_ind_msg_v02, e911_end_reason),
  QMI_IDL_TYPE88(2, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  voice_ims_ext_type_table_v02[] = {
  {sizeof(voice_srvcc_call_context_type_v02), voice_srvcc_call_context_type_data_v02},
  {sizeof(voice_srvcc_alerting_info_type_v02), voice_srvcc_alerting_info_type_data_v02},
  {sizeof(voice_srvcc_call_context_name_type_v02), voice_srvcc_call_context_name_type_data_v02},
  {sizeof(voice_srvcc_call_context_num_pi_type_v02), voice_srvcc_call_context_num_pi_type_data_v02}
};

/* Message Table */
static const qmi_idl_message_table_entry voice_ims_ext_message_table_v02[] = {
  {sizeof(voice_srvcc_call_config_req_msg_v02), voice_srvcc_call_config_req_msg_data_v02},
  {sizeof(voice_srvcc_call_config_resp_msg_v02), voice_srvcc_call_config_resp_msg_data_v02},
  {sizeof(voice_e911_orig_fail_ind_msg_v02), voice_e911_orig_fail_ind_msg_data_v02}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object voice_ims_ext_qmi_idl_type_table_object_v02;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *voice_ims_ext_qmi_idl_type_table_object_referenced_tables_v02[] =
{&voice_ims_ext_qmi_idl_type_table_object_v02, &common_qmi_idl_type_table_object_v01, &voice_service_common_qmi_idl_type_table_object_v02};

/*Type Table Object*/
static const qmi_idl_type_table_object voice_ims_ext_qmi_idl_type_table_object_v02 = {
  sizeof(voice_ims_ext_type_table_v02)/sizeof(qmi_idl_type_table_entry ),
  sizeof(voice_ims_ext_message_table_v02)/sizeof(qmi_idl_message_table_entry),
  1,
  voice_ims_ext_type_table_v02,
  voice_ims_ext_message_table_v02,
  voice_ims_ext_qmi_idl_type_table_object_referenced_tables_v02,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry voice_ims_ext_service_command_messages_v02[] = {
  {QMI_VOICE_SRVCC_CALL_CONFIG_REQ_V02, QMI_IDL_TYPE16(0, 0), 3229}
};

static const qmi_idl_service_message_table_entry voice_ims_ext_service_response_messages_v02[] = {
  {QMI_VOICE_SRVCC_CALL_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7}
};

static const qmi_idl_service_message_table_entry voice_ims_ext_service_indication_messages_v02[] = {
  {QMI_VOICE_E911_ORIG_FAIL_IND_V02, QMI_IDL_TYPE16(0, 2), 6}
};

/*Service Object*/
struct qmi_idl_service_object voice_ims_ext_qmi_idl_service_object_v02 = {
  0x06,
  0x02,
  0x09,
  3229,
  { sizeof(voice_ims_ext_service_command_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(voice_ims_ext_service_response_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(voice_ims_ext_service_indication_messages_v02)/sizeof(qmi_idl_service_message_table_entry) },
  { voice_ims_ext_service_command_messages_v02, voice_ims_ext_service_response_messages_v02, voice_ims_ext_service_indication_messages_v02},
  &voice_ims_ext_qmi_idl_type_table_object_v02,
  0x04,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type voice_ims_ext_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( VOICE_IMS_EXT_V02_IDL_MAJOR_VERS != idl_maj_version || VOICE_IMS_EXT_V02_IDL_MINOR_VERS != idl_min_version 
       || VOICE_IMS_EXT_V02_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&voice_ims_ext_qmi_idl_service_object_v02;
}

