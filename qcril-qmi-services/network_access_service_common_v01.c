/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        N E T W O R K _ A C C E S S _ S E R V I C E _ C O M M O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the network_access_service_common service Data structures.

  Copyright (c) 2014, 2016-2021 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.2/nas/src/network_access_service_common_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9
   It was generated on: Tue Mar 16 2021 (Spin 0)
   From IDL File: network_access_service_common_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "network_access_service_common_v01.h"


/*Type Definitions*/
static const uint8_t nas_plmn_id_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_id_ext_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_id_ext_type_v01, mnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_id_ext_type_v01, mnc_includes_pcs_digit),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* Type Table */
static const qmi_idl_type_table_entry  network_access_service_common_type_table_v01[] = {
  {sizeof(nas_plmn_id_ext_type_v01), nas_plmn_id_ext_type_data_v01}
};

/* Message Table */
/* No Messages Defined in IDL */

/* Range Table */
/* Predefine the Type Table Object */
const qmi_idl_type_table_object network_access_service_common_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *network_access_service_common_qmi_idl_type_table_object_referenced_tables_v01[] =
{&network_access_service_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object network_access_service_common_qmi_idl_type_table_object_v01 = {
  sizeof(network_access_service_common_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  0,
  1,
  network_access_service_common_type_table_v01,
  NULL,
  network_access_service_common_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};
