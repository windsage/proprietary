/******************************************************************************

                          S H S C _ A P S . H

******************************************************************************/

/******************************************************************************

  @file    shsc_aps.h
  @brief   shsusrd apps prioritization service header file

  DESCRIPTION
  Header file for shsusrd apps prioritization service.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __SHSC_APS_H
#define __SHSC_APS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * APS message defintions
 */

#define SHSC_MSG_APS_ADD_FLOW 10
#define SHSC_MSG_APS_DELETE_FLOW 11
#define SHSC_MSG_APS_UPDATE_FLOW 12
#define SHSC_MSG_APS_FLOW_REMOVED_IND 13

/* APS result codes */
enum aps_result_code_e {
	APS_SUCCESS,
	APS_INVALID_PARAMS,
	APS_TOO_MANY_FLOWS,
	APS_DUP_FLOW,
	APS_INVALID_IP_ADDR,
	APS_INTERNAL_ERROR,
	APS_FLOW_NOT_FOUND,
	APS_MISSING_ARGS,
	APS_ERR_NO_NET,
	APS_ERR_MAX,
};

/* Priority in descending order */
enum aps_priority_e {
	APS_PRIORITY_UNSPECIFIED,
	APS_PRIORITY_1,
	APS_PRIORITY_2,
	APS_PRIORITY_3,
	APS_PRIORITY_4,
	ASP_PRIOROTY_MAX,
};

/* Flow removed reason */
enum aps_flow_removed_reason_e {
	APS_FLOW_REMOVED_UNKNOWN,
	APS_FLOW_REMOVED_EXPIRED,
	APS_FLOW_REMOVED_NO_LONGER_VALID,
	APS_FLOW_REMOVED_MAX,
};

/* Flow update reason */
enum aps_flow_update_reason_e {
	APS_FLOW_UPDATE_UNKNOWN,
	APS_FLOW_UPDATE_DELETED,
	APS_FLOW_UPDATE_INACTIVE,
	APS_FLOW_UPDATE_ACTIVE,
	APS_FLOW_UPDATE_PRIORITY,
	APS_FLOW_UPDATE_MAX,
};

/* Flow direction */
enum aps_flow_direction_e {
	APS_FLOW_DIR_UL,
	APS_FLOW_DIR_DL,
	APS_FLOW_DIR_UL_DL,
	APS_FLOW_DIR_MAX,
};

/*
 * ADD FLOW
 * Source should always be UE regardless of direction
 */
struct aps_add_flow_req {
	uint32_t flow_id;
	int32_t ip_type; /* AF_INET, AF_INET6 */
	uint32_t saddr[4]; /* NW byte order */
	uint32_t daddr[4]; /* NW byte order */
	uint16_t sport;
	uint16_t dport;
	uint8_t protocol; /* protocol number */
	uint8_t direction; /* aps_flow_direction_e */
	uint8_t app_type;
	uint8_t app_priority; /* From modem */
	uint8_t is_app_type_priority; /* From modem */
	uint8_t bearer_priority; /* From modem */
	uint8_t tos; /* TOS or Traffic class */
	uint8_t tos_mask;
	uint32_t flow_label;
	uint32_t inactivity_duration; /* seconds */
	uint8_t reserved[68];
};

struct aps_add_flow_resp {
	uint8_t result_code; /* aps_result_code_e */
	uint8_t priority; /* combined aps_priority_e */
};

/*
 * DELETE FLOW
 */
#define APS_MAX_NUM_FLOWS 255
struct aps_flow_delete_result {
	uint32_t flow_id;
	uint8_t delete_result; /* aps_result_code_e */
};

struct aps_delete_flow_req {
	uint8_t num_flows;
	uint32_t flow_id[APS_MAX_NUM_FLOWS];
};

struct aps_delete_flow_resp {
	uint8_t result_code; /* aps_result_code_e */
	uint8_t num_flows;
	struct aps_flow_delete_result result[APS_MAX_NUM_FLOWS];
};

/*
 * UPDATE FLOW
 */
struct aps_flow_update_info {
	uint32_t flow_id;
	uint8_t update_reason; /* aps_flow_update_reason_e */
	uint8_t new_app_priority;
	uint8_t is_app_type_priority;
	uint8_t new_bearer_priority;
};

struct aps_flow_update_result {
	uint32_t flow_id;
	uint8_t update_result; /* aps_result_code_e */
	uint8_t new_priority; /* aps_priority_e */
};

struct aps_update_flow_req {
	uint8_t num_flows;
	struct aps_flow_update_info info[APS_MAX_NUM_FLOWS];
};

struct aps_update_flow_resp {
	uint8_t result_code; /* aps_result_code_e */
	uint8_t num_flows;
	struct aps_flow_update_result result[APS_MAX_NUM_FLOWS];
};

/*
 * FLOW INDICATION
 */
struct aps_flow_removed_ind {
	uint32_t flow_id;
	uint8_t reason_code; /* aps_flow_removed_reason_e */
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SHSC_APS_H */
