/******************************************************************************

  @file    dsi_netctrl_internal.h

  DESCRIPTION
  internal header file for nicm and dsi_netctrl

******************************************************************************/
/*===========================================================================

  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef DSI_NICM_INTERNAL_H
#define DSI_NICM_INTERNAL_H

#include "wireless_data_service_v01.h"

/* Additional EP TYPE */
#define DSI_QMI_WDS_PER_EP_TYPE_BAM_DMUX  0x05

/* Parameter indication bits */
#define DSI_QMI_WDS_ROUTE_LOOKUP_TECH_PREF_PARAM                      0x00000001
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_PARAM                    0x00000002
#define DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_PARAM                       0x00000004
#define DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_PARAM                 0x00000008
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_PARAM              0x00000010
#define DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_PARAM              0x00000020
#define DSI_QMI_WDS_ROUTE_LOOKUP_DATA_CALL_ORIGIN_PARAM               0x00000040
#define DSI_QMI_WDS_ROUTE_LOOKUP_APN_TYPE_PARAM                       0x00000080
#define DSI_QMI_WDS_ROUTE_LOOKUP_APN_TYPE_MASK_PARAM                  0x00000100
#define DSI_QMI_WDS_ROUTE_LOOKUP_BRINGUP_BY_APN_NAME_PARAM            0x00000200
#define DSI_QMI_WDS_ROUTE_LOOKUP_BRINGUP_BY_APN_TYPE_PARAM            0x00000400

typedef enum
{
  DSI_INTERFACE_LOOKUP  = 1,
  DSI_DATAPATH_LOOKUP   = 2
}dsi_qmi_wds_route_lookup_type;

typedef struct
{
  /* Bitmask which indicates which of the below
  ** parameters has been set
  */
  /*Mandatory only valid for route lookup*/
  dsi_qmi_wds_route_lookup_type     route_lookup;
  unsigned long                     params_mask;
  /* Parameters, more will be added later */
  unsigned char                     profile_index;
  unsigned char                     profile_index_3gpp2;
  unsigned char                     tech_pref;
  wds_technology_name_enum_v01      xtended_tech_pref;
  wds_ip_family_preference_enum_v01 ip_family_pref;
  char                              apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  wds_call_type_enum_v01            data_call_origin;
  int                               bringup_by_apn_name;
  int                               bringup_by_apn_type;
  wds_apn_type_enum_v01             apn_type;
  wds_apn_type_mask_v01             apn_type_mask;
} dsi_qmi_wds_route_look_up_params_type;

#endif /* DSI_NICM_INTERNAL_H */