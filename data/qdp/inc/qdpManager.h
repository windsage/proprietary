/*===========================================================================

  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "qdp.h"

void manager_init
(
  int subId,
  boolean profileUpdate,
  boolean isApAssist
);

int manager_match_lte_attach_params
(
  unsigned int profile_id,
  const char **param_strings
);

int manager_find_or_create_profile_for_lte_attach
(
  const char **param_strings,
  unsigned int *profile_id,
  qdp_profile_pdn_type *pdn_type,
  qdp_error_info_t *error_info
);

int manager_find_or_create_profile_for_setup_data
(
  const char** param_strings,
  unsigned int *profile_id,
  qdp_profile_pdn_type *pdn,
  qdp_error_info_t *error_info,
  qdp_tech_t tech
);

int manager_find_or_create_profile_for_emergency_call
(
  const char **qdpParams,
  unsigned int *profileId,
  qdp_tech_t *techType,
  qdp_profile_pdn_type *pdnType,
  char *apn
);

int manager_find_profile_by_apn_class
(
  int apnclass,
  unsigned int *profile_id_list,
  uint8 *profile_id_list_len,
  qdp_profile_pdn_type *profile_pdn_type,
  qdp_tech_t tech
);

int manager_add_profile
(
  int apnClass,
  const char **param_strings,
  uint* profile_id_list,
  qdp_profile_pdn_type *profile_pdn_type,
  qdp_tech_t tech
);

int manager_delete_profile
(
  unsigned int profile_id
);

int qdp_modify_profile_param
(
  qdp_profile_info_type  *profile
);

int qdp_update_profile_param
(
  wds_modify_profile_settings_req_msg_v01  *p_params
);

int qdp_3gpp_profile_update
(
  const char **param_strings,
  unsigned int profile_id,
  wds_modify_profile_settings_req_msg_v01 *modify_params,
  boolean *updateNeeded
);

int qdp_3gpp2_profile_update
(
  const char **param_strings,
  unsigned int profile_id,
  wds_modify_profile_settings_req_msg_v01 *modify_params
);

void release_key
(
  unsigned int profileId
);

void release_all_key_instances
(
  unsigned int profileId
);

void cleanup_keyList();


#ifdef __cplusplus
}
#endif
