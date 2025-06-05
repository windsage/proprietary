/*!
  @file
  qdp.c

  @brief
  provides an API for Qualcomm data profile management.

*/

/*===========================================================================

  Copyright (c) 2010-2015, 2017-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //linux/pkgs/proprietary/qc-ril/main/source/qcril_data.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/10   js      created file

===========================================================================*/
/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "qdp_platform.h"
#include "qmi_platform_config.h"
#include "qdp.h"
#include "qdpManager.h"

#ifndef RIL_FOR_MDM_LE
#ifdef FEATURE_QDP_LINUX_ANDROID
  #include <cutils/properties.h>
#endif
#endif

#define QCRIL_APN_TYPES_STR_MAX        16

typedef struct qdp_param_tech_map_s
{
  qdp_ril_param_idx_t ril_param_idx;
  qdp_tech_t tech_mask; /* 3gpp/3gpp2/both */
  int max_param_len;
} qdp_param_tech_map_t;


qdp_param_tech_map_t param_tech_map_tbl[QDP_RIL_PARAM_MAX] =
{
  { QDP_RIL_TECH, QDP_NOTECH, 0 },
  { QDP_RIL_PROFILE_ID, QDP_NOTECH, 0 },
  { QDP_RIL_APN, (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP), QMI_WDS_APN_NAME_MAX_V01 },
  { QDP_RIL_NAI, (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP), QMI_WDS_USER_NAME_MAX_V01 },
  { QDP_RIL_PASSWORD, (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP), QMI_WDS_PASSWORD_MAX_V01 },
  { QDP_RIL_AUTH, (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP), sizeof(wds_profile_auth_protocol_enum_v01) },
  { QDP_RIL_IP_FAMILY, QDP_NOTECH | QDP_3GPP2 | QDP_3GPP, sizeof(QDP_RIL_IP_4_6) },
  { QDP_RIL_IP_ROAMING, QDP_NOTECH | QDP_3GPP, sizeof(QDP_RIL_IP_4_6) },
  { QDP_RIL_APN_TYPE_BITMASK, QDP_NOTECH | QDP_3GPP | QDP_3GPP2, QCRIL_APN_TYPES_STR_MAX}
};

#define QDP_VALIDATE_RIL_PARAM_INDEX(i) \
  if (i<QDP_RIL_PARAM_MAX && i>0)

#define QDP_GET_RIL_PARAM_IDX(i) \
  param_tech_map_tbl[i].ril_param_idx

#define QDP_GET_3GPP_MAX_LEN(i) \
  param_tech_map_tbl[i].max_param_len

#define QDP_GET_3GPP2_MAX_LEN(i) \
  param_tech_map_tbl[i].max_param_len

#define QDP_GET_TECH_MASK(i) \
  param_tech_map_tbl[i].tech_mask


typedef struct qdp_param_s
{
  char * buf;
  int len;
} qdp_param_t;

boolean qdp_inited = FALSE;
boolean profile_update = FALSE;
boolean manager_inited = FALSE;
boolean is_apassist = FALSE;

#define QDP_INIT_BARRIER \
  do \
  { \
    if (FALSE == qdp_inited || FALSE == manager_inited) \
    { \
      QDP_LOG_ERROR("%s","qdp not inited"); \
      return QDP_FAILURE; \
    } \
  } while (0)

#define QDP_RIL_DATA_PROFILE_OEM_BASE 1000

#define QDP_INVALID_PROFILE_ID 0xFF

boolean validate_param_string(const char **param_strings)
{
  int i = 2,temp_len = 0;
  if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
     (strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0 ||
       (i == QDP_RIL_APN && strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) == 0))) {
    temp_len = (int)strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
    if(temp_len > QDP_GET_3GPP_MAX_LEN(i)) {
      QDP_LOG_ERROR("RIL param length too long [%d] for ril index [%d]", temp_len, i);
      return FALSE;
    }
  }
  return TRUE;
}

void qdp_profile_cleanup
(
  void
)
{
  QDP_LOG_DEBUG("%s", "qdp_profile_cleanup : ENTRY");
  cleanup_keyList();
  QDP_LOG_DEBUG("%s", "qdp_profile_cleanup : EXIT");
}

void qdp_profile_read_ril_params(const char  ** param_strings,
                                 unsigned int *profile_id_3gpp_list,
                                 boolean *lookup_3gpp_profile,
                                 unsigned int *profile_id_3gpp2_list,
                                 boolean *lookup_3gpp2_profile)
{
  QDP_LOG_DEBUG("%s","qdp_profile_read_ril_params : ENTRY");
  int temp_profile_id = 0, i;
  bool temp_profile_id_valid = FALSE;
  int tech_mask = 0;
  if(NULL != param_strings[QDP_RIL_PROFILE_ID])
  {
    temp_profile_id = atoi(param_strings[QDP_RIL_PROFILE_ID]);
    if (temp_profile_id >= QDP_RIL_DATA_PROFILE_OEM_BASE) {
      temp_profile_id = (temp_profile_id - QDP_RIL_DATA_PROFILE_OEM_BASE);
      temp_profile_id_valid = TRUE;
      QDP_LOG_DEBUG("provided profile id is %d", temp_profile_id);
    }
  }

  if(param_strings[QDP_RIL_TECH] != NULL)
  {
    if (0 == strncmp(QDP_RIL_3GPP, param_strings[QDP_RIL_TECH], QDP_RIL_TECH_LEN)) {
      QDP_LOG_DEBUG("%s", "RIL prefers 3GPP");
      *lookup_3gpp_profile = TRUE;
      if(temp_profile_id_valid){
        profile_id_3gpp_list[0] = temp_profile_id;
      }
    }
    if (0 == strncmp(QDP_RIL_3GPP2, param_strings[QDP_RIL_TECH], QDP_RIL_TECH_LEN)) {
      *lookup_3gpp2_profile = TRUE;
      QDP_LOG_DEBUG("%s", "RIL prefers 3GPP2");
      if(temp_profile_id_valid){
        profile_id_3gpp2_list[0] = temp_profile_id;
      }
    }
    if(0 == strncmp(QDP_RIL_AUTO, param_strings[QDP_RIL_TECH], QDP_RIL_TECH_LEN)) {
      *lookup_3gpp_profile = TRUE;
      *lookup_3gpp2_profile = TRUE;
      QDP_LOG_DEBUG("%s", "RIL prefers AUTO");
      if(temp_profile_id_valid){
        profile_id_3gpp_list[0] = temp_profile_id;
        profile_id_3gpp2_list[0] = temp_profile_id;
      }
    }
  }

  if(*lookup_3gpp_profile == FALSE && *lookup_3gpp2_profile == FALSE) {
    for(i=0; i<QDP_RIL_PARAM_MAX; i++) {
      if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
           (strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0 ||
            (i == QDP_RIL_APN && strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) == 0)))
      {
        tech_mask |= QDP_GET_TECH_MASK(i);
      }
    }
    if (tech_mask & QDP_3GPP) {
      *lookup_3gpp_profile = TRUE;
    }
    if (tech_mask & QDP_3GPP2) {
      *lookup_3gpp2_profile = TRUE;
    }
  }

  QDP_LOG_DEBUG("%s","qdp_profile_read_ril_params : EXIT");
}

/*===========================================================================
  FUNCTION:  qdp_lte_attach_profile_lookup
===========================================================================*/
/*!
  @brief
  This function is used to query and update LTE attach profiles on the modem.
  It receives the new LTE attach parameters and tries to lookup valid 3GPP
  profiles. If a valid profile does not exist then a new profile will be
  created with the profile parameters received in the input. If a valid
  profile exists, it will be updated with the new parameters.

  @params
  param_strings: a pointer to the array of parameter string which contains
  the new LTE attach parameters.

  profile_id_lte: It is a in/out parameter.In Legacy case, it provides default
  attach profile id. In new implementation, it is invalid parameter and gets
  updated with correct output profile id after look up.

  input_prof_id: It is a input parameter. Applicable to newer implementation.
  For non empty attach pdn list. it holds index 0 entry of attach list
  in case of empty attach pdn list, it holds invalid value

  prof_params: out parameter which will hold the profile parameters
  before the profile is updated. If the LTE attach process fails we would
  need this to restore the modem profile to its previous state

  @return
  QDP_SUCCESS
  QDP_FAILURE
*/
/*=========================================================================*/
int qdp_lte_attach_profile_lookup
(
  const char             **param_strings,
  unsigned int           *profile_id_lte,
  qdp_profile_pdn_type   *profile_pdp_type_lte,
  unsigned int           *input_prof_id,
  qdp_profile_info_type  *prof_params,
  qdp_error_info_t       *error_info
)
{
  QDP_LOG_DEBUG("%s","qdp_lte_attach_profile_lookup ENTRY");
  if( NULL == param_strings          ||
      NULL == profile_id_lte         ||
      NULL == profile_pdp_type_lte   ||
      NULL == input_prof_id          ||
      NULL == error_info             ||
      !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return QDP_FAILURE;
  }

  QDP_INIT_BARRIER;

  error_info->error = QDP_ERROR_NONE;
  error_info->tech = QDP_NOTECH;
  prof_params->is_valid = FALSE;

  int ret = manager_find_or_create_profile_for_lte_attach(param_strings,
                                                          profile_id_lte,
                                                          profile_pdp_type_lte,
                                                          error_info);
  if(*profile_id_lte != QDP_INVALID_PROFILE)
  {
    QDP_LOG_DEBUG("received profile id %d", *profile_id_lte);
  }
  QDP_LOG_DEBUG("%s","qdp_lte_attach_profile_lookup Exit");
  return ret;

} /* qdp_lte_attach_profile_lookup */

/*===========================================================================
  FUNCTION:  qdp_match_lte_attach_profile_params
===========================================================================*/
/*!
    @brief

    @params

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_match_lte_attach_profile_params
(
  unsigned int  profile_id,
  const char    **param_strings
)
{
  QDP_LOG_DEBUG("%s","qdp_match_lte_attach_profile_param ENTRY");

  QDP_INIT_BARRIER;

  int ret = QDP_FAILURE;

  if (!param_strings || !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return ret;
  }

  ret = manager_match_lte_attach_params(profile_id, param_strings);
  QDP_LOG_DEBUG("%s","qdp_match_lte_attach_profile_param EXIT");
  return ret;
}

/*===========================================================================
  FUNCTION:  emergencyProfileLookup
===========================================================================*/
/*!
  @brief
  This function is used to query for emergency data call profiles on the
  modem. It bypasses parameter validation, and instead uses the following
  algorithm to select a profile.

  Lookup profiles with emergency call support
      0 profiles found:
          create profile using the RIL params, then use it
      1 profile found:
          use it
      >1 profiles found:
          match by APN name, use it
          if no match found
              use the first profile

  @params
  [in]qdpParams:  a pointer to the array of parameter string which contains
                  the setup data call parameters. This parameter list uses
                  the same ordering as param_strings in qdp_profile_look_up,
                  specified by qdp_ril_param_idx_t

  [out]profileId: the profile id of the selected profile
  [out]pdnType:   IPV4, IPV6, or IPV4V6
  [out]techType:  3GPP or 3GPP2 profile

  @return
  QDP_SUCCESS     if profile was found/created
  QDP_FAILURE     if profile was not found/created
*/
/*=========================================================================*/
int emergencyProfileLookup
(
  const char **qdpParams,
  unsigned int *profileId,
  qdp_profile_pdn_type *pdnType,
  qdp_tech_t *techType
)
{
  QDP_LOG_DEBUG("%s","emergencyProfileLookup ENTRY");
  QDP_INIT_BARRIER;
  int rc = emergencyProfileLookup2(qdpParams, profileId, NULL, pdnType, techType);
  QDP_LOG_DEBUG("%s","emergencyProfileLookup EXIT");
  return rc;
}

/*===========================================================================
  FUNCTION:  emergencyProfileLookup2
===========================================================================*/
/*!
  @brief
  This function is used to query for emergency data call profiles on the
  modem. It bypasses parameter validation, and instead uses the following
  algorithm to select a profile.

  Lookup profiles with emergency call support
      0 profiles found:
          create profile using the RIL params, then use it
      1 profile found:
          use it
      >1 profiles found:
          match by APN name, use it
          if no match found
              use the first profile

  @params
  [in]qdpParams:  a pointer to the array of parameter string which contains
                  the setup data call parameters. This parameter list uses
                  the same ordering as param_strings in qdp_profile_look_up,
                  specified by qdp_ril_param_idx_t

  [out]profileId: the profile id of the selected profile
  [out]apnName:   the apn name of the selected profile
  [out]pdnType:   IPV4, IPV6, or IPV4V6
  [out]techType:  3GPP or 3GPP2 profile

  @return
  QDP_SUCCESS     if profile was found/created
  QDP_FAILURE     if profile was not found/created
*/
/*=========================================================================*/
int emergencyProfileLookup2
(
  const char **qdpParams,
  unsigned int *profileId,
  char         *apnName,
  qdp_profile_pdn_type *pdnType,
  qdp_tech_t *techType
)
{
  QDP_LOG_DEBUG("%s","emergencyProfileLookup2 ENTRY");
  QDP_INIT_BARRIER;
  if (NULL == qdpParams ||
      NULL == profileId ||
      NULL == pdnType ||
      NULL == techType)
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return QDP_FAILURE;
  }

  int rc = manager_find_or_create_profile_for_emergency_call(qdpParams,
                                                             profileId,
                                                             techType,
                                                             pdnType,
                                                             apnName);
  QDP_LOG_DEBUG("received profile id %d", rc);
  QDP_LOG_DEBUG("%s","emergencyProfileLookup2 EXIT");
  return rc;
}

/*===========================================================================
  FUNCTION:  qdp_profile_look_up
===========================================================================*/
/*!
    @brief
    Given the set of RIL SETUP_DATA_CALL parameter string array
    (please refer to qdp_ril_param_idx_t to see what order strings
    are expected to be in, to omit certain parameter(s), leave the
    corresponding entry NULL in the array), this function determines
    the applicable technologies, and returns the corresponding 3gpp
    and/or 3gpp2 profile id(s) to be used for data call route look up.
    If required, this function would create new profile(s) on behalf
    of the caller.

    if a non-zero profile_id is returned for a given technology
    (i.e. 3gpp, 3gpp2), the caller of this API is automatically
    assumed to have a reference count on it, which, in turn, must
    be released by calling qdp_profile_release() API when ever
    the caller no longer needs to sustain a data call with that
    profile id.

    @params
    param_strings: a pointer to the array of parameter string such
    that *param_strings[i] is ith parameter string
    profile_id_3gpp: placeholder for the 3gpp profile id (output)
    profile_id_3gpp2: placeholder for the 3gpp2 profile id (output)
    ril_tech: current technology specified at RIL API
    error_info: place holder for specific lookup error information

    @Examples
    User can query both 3gpp and 3gpp2 profile ids using subset of parameters
    listed in qdp_ril_param_idx_t.

    Example 1: if user provides valid values for
    QDP_RIL_TECH = QDP_RIL_3GPP2
    QDP_RIL_APN = "3GPP2_APN"
    qdp_profile_look_up() will try to look up *only* 3gpp2 profile id with
    APN set to "3GPP2_APN"

    Example 2: If user provides valid values for
    QDP_RIL_TECH = "QDP_RIL_AUTO"
    QDP_RIL_APN = "APN"
    QDP_RIL_NAI = "USER"
    qdp_profile_look_up() will try to look up
    * 3gpp profile using "APN" (NAI does not apply to 3GPP profiles)
    * 3gpp2 profile using "APN", and "USER"

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_look_up
(
  const char  ** param_strings,    /* the order of params must match with the
                                      order specified in qdp_ril_param_idx_t */
  unsigned int * profile_id_3gpp,  /* value (not pointer it-self) must
                                      be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp,  /* 3gpp profile PDN type */
  unsigned int * profile_id_3gpp2, /* value must be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp2, /* 3gpp2 profile PDN type */
  qdp_error_info_t * error_info
)
{
  QDP_LOG_DEBUG("%s","qdp_profile_look_up ENTRY");
  if( NULL == param_strings          ||
      NULL == profile_id_3gpp        ||
      NULL == profile_pdn_type_3gpp  ||
      NULL == profile_id_3gpp2       ||
      NULL == profile_pdn_type_3gpp2 ||
      NULL == error_info             ||
      !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return QDP_FAILURE;
  }

  QDP_INIT_BARRIER;

  int ret = QDP_FAILURE;
  boolean lookup_3gpp_profile = FALSE, lookup_3gpp2_profile = FALSE;

  error_info->error = QDP_ERROR_NONE;
  error_info->tech = QDP_NOTECH;

  qdp_profile_read_ril_params(param_strings,
                              profile_id_3gpp,
                              &lookup_3gpp_profile,
                              profile_id_3gpp2,
                              &lookup_3gpp2_profile);

  if(lookup_3gpp_profile)
  {
    ret = manager_find_or_create_profile_for_setup_data(param_strings,
                                                        profile_id_3gpp,
                                                        profile_pdn_type_3gpp,
                                                        error_info,
                                                        QDP_3GPP);
    QDP_LOG_DEBUG("Received 3gpp profile id %d", *profile_id_3gpp);
  }

  if(lookup_3gpp2_profile)
  {
    ret = manager_find_or_create_profile_for_setup_data(param_strings,
                                                        profile_id_3gpp2,
                                                        profile_pdn_type_3gpp2,
                                                        error_info,
                                                        QDP_3GPP2);
    QDP_LOG_DEBUG("Received 3gpp2 profile id %d", *profile_id_3gpp2);
  }

  QDP_LOG_DEBUG("%s","qdp_profile_look_up EXIT");
  return ret;
}

/*===========================================================================
    FUNCTION:  qdp_profile_look_up_by_param
===========================================================================*/
/*!
     @brief
     qdp_profile_look_up_by_param is an extension of qdp_profile_look_up
     where profile look up is performed by looking for a profile ID that
     matches a given param in the profile. Currently only APN CLASS
     matching is supported.

     @params
     param_strings: a pointer to the array of parameter string such
     that *param_strings[i] is ith parameter string
     profile_id_3gpp: placeholder for the 3gpp profile id (output)
     profile_id_3gpp2: placeholder for the 3gpp2 profile id (output)
     ril_tech: current technology specified at RIL API
     error_info: place holder for specific lookup error information

     @return
     QDP_SUCCESS
     QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_look_up_by_param
(
  const char                             **param_strings,   /* the order of params must match with the
                                                               order specified in qdp_ril_param_idx_t */
  int                                    param_to_match,
  int                                    param_value,
  unsigned int                           *profile_id_3gpp_list,
  uint8                            *profile_id_3gpp_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type_3gpp,  /* 3gpp profile PDN type */
  unsigned int                           *profile_id_3gpp2_list,
  uint8                           *profile_id_3gpp2_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type_3gpp2,  /* 3gpp2 profile PDN type */
  qdp_tech_t                             *tech_type,
  qdp_error_info_t                       *error_info
)
{
  if( NULL == param_strings || NULL == profile_id_3gpp_list || NULL == profile_pdn_type_3gpp  || profile_id_3gpp_list_len == NULL ||
      profile_id_3gpp2_list_len == NULL || NULL == profile_id_3gpp2_list || NULL == profile_pdn_type_3gpp2 || NULL == error_info ||
      NULL == tech_type || !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return QDP_FAILURE;
  }

  QDP_INIT_BARRIER;

  int ret = QDP_FAILURE, rc;
  boolean lookup_3gpp_profile = FALSE;
  boolean lookup_3gpp2_profile = FALSE;
  *profile_id_3gpp_list_len = 0;
  *profile_id_3gpp_list_len = 0;
  QDP_LOG_DEBUG("%s","qdp_profile_look_up_by_param ENTRY");

  error_info->error = QDP_ERROR_NONE;
  error_info->tech = QDP_NOTECH;

  qdp_profile_read_ril_params(param_strings,
                              profile_id_3gpp_list,
                              &lookup_3gpp_profile,
                              profile_id_3gpp2_list,
                              &lookup_3gpp2_profile);


  if(lookup_3gpp_profile) {
    QDP_LOG_DEBUG("%s","lookup_3gpp_profile is set");
    rc = manager_find_profile_by_apn_class(param_value,
                                           profile_id_3gpp_list,
                                           profile_id_3gpp_list_len,
                                           profile_pdn_type_3gpp,
                                           QDP_3GPP);
    QDP_LOG_DEBUG("3gpp list len is %d", *profile_id_3gpp_list_len);
    *tech_type = QDP_3GPP;
  }

  if(lookup_3gpp2_profile) {
    QDP_LOG_DEBUG("%s","lookup_3gpp2_profile is set");
    rc = manager_find_profile_by_apn_class(param_value,
                                           profile_id_3gpp2_list,
                                           profile_id_3gpp2_list_len,
                                           profile_pdn_type_3gpp2,
                                           QDP_3GPP2);
    QDP_LOG_DEBUG("3gpp2 list len is %d", *profile_id_3gpp2_list_len);
  }

  if(lookup_3gpp_profile && *profile_id_3gpp_list_len == 0) {
    QDP_LOG_DEBUG("%s","lookup_3gpp_profile is set but no matching profile found so creating a profile");
    manager_add_profile(param_value,
                        param_strings,
                        profile_id_3gpp_list,
                        profile_pdn_type_3gpp,
                        QDP_3GPP);
    *profile_id_3gpp_list_len = 1;
  }

  if(lookup_3gpp2_profile && *profile_id_3gpp2_list == 0) {
    QDP_LOG_DEBUG("%s","lookup_3gpp2_profile is set but no matching profile found so creating a profile");
    manager_add_profile(param_value,
                        param_strings,
                        profile_id_3gpp2_list,
                        profile_pdn_type_3gpp2,
                        QDP_3GPP2);
    *profile_id_3gpp2_list_len = 1;
  }

  QDP_LOG_DEBUG("%s","qdp_profile_look_up_by_param EXIT");
  return QDP_SUCCESS;
}

/*===========================================================================
 FUNCTION: qdp_3gpp_profile_update_params
===========================================================================*/
/*!
    @brief
    Updates the WDS input structure with the given 3gpp profile parameters

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)
    p_params: out parameter to hold the profile parameters for a given
    profile id before it is updated.
    updateNeeded: out parameter for return boolean (TRUE/FALSE) indicating
    if update is needed or not

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_3gpp_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
)
{
  int ret = QDP_FAILURE;
  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params ENTRY");
  QDP_INIT_BARRIER;
  if( NULL == param_strings || NULL == p_params || !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return ret;
  }
  boolean updateNeeded = FALSE;
  ret = qdp_3gpp_profile_update(param_strings,
                                profile_id,
                                p_params,
                                &updateNeeded);
  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params EXIT");
  return ret;
}

/*===========================================================================
 FUNCTION: qdp_3gpp_profile_update_params
===========================================================================*/
/*!
    @brief
    Updates the WDS input structure with the given 3gpp profile parameters

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)
    p_params: out parameter to hold the profile parameters for a given
    profile id before it is updated.
    updateNeeded: out parameter for return boolean (TRUE/FALSE) indicating
    if update is needed or not
    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp_profile_update_params_v2
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params,
  boolean                                  *updateNeeded
)
{
  int ret = QDP_FAILURE;
  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params ENTRY");
  QDP_INIT_BARRIER;
  if( NULL == param_strings || NULL == p_params ||
      NULL == updateNeeded || !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return ret;
  }
  ret = qdp_3gpp_profile_update(param_strings,
                                profile_id,
                                p_params,
                                updateNeeded);

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params EXIT");
  return ret;
}
/*===========================================================================
 FUNCTION: qdp_3gpp2_profile_update_params
===========================================================================*/
/*!
    @brief
    Updates the WDS input structure with the given EPC profile parameters

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_3gpp2_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
)
{
  int ret = QDP_FAILURE;
  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_update_params ENTRY");
  QDP_INIT_BARRIER;
  if( NULL == param_strings || NULL == p_params || !validate_param_string(param_strings))
  {
    QDP_LOG_ERROR("%s","NULL params rcvd");
    return QDP_FAILURE;
  }
  ret = qdp_3gpp2_profile_update(param_strings,
                                 profile_id,
                                 p_params);
  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_update_params EXIT");
  return ret;
}

/*===========================================================================
 FUNCTION: qdp_profile_update_ex
===========================================================================*/
/*!
   @brief
   Extending qdp_epc_profile_update API for the client to update any
   parameter of choice

*/

/*=========================================================================*/
int qdp_profile_update_ex
(
  wds_modify_profile_settings_req_msg_v01  *p_params,
  int                                      *error_code
)
{
  QDP_LOG_DEBUG("%s","qdp_profile_update_ex ENTRY");
  QDP_INIT_BARRIER;
  *error_code = qdp_update_profile_param(p_params);
  QDP_LOG_DEBUG("%s","qdp_profile_update_ex EXIT");
  return QDP_SUCCESS;
}

/*===========================================================================
 FUNCTION: qdp_modify_profile
===========================================================================*/
/*!
    @brief
    Updates the Modem profile referenced by the given input parameter

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_modify_profile
(
  qdp_profile_info_type  *profile,
  int                    *error_code
)
{
  QDP_LOG_DEBUG("%s","qdp_modify_profile ENTRY");
  QDP_INIT_BARRIER;
  *error_code = qdp_modify_profile_param(profile);
  QDP_LOG_DEBUG("%s","qdp_modify_profile EXIT");
  return QDP_SUCCESS;
}

/*===========================================================================
  FUNCTION:  qdp_profile_release
===========================================================================*/
/*!
    @brief
    decreases the reference count on the given profile

    profile is automatically deleted when ref count goes to zero

    @params
    profile_id: profile id

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_release
(
  unsigned int profile_id
)
{
  QDP_LOG_DEBUG("%s","qdp_profile_release ENTRY");
  QDP_INIT_BARRIER;
  release_key(profile_id);
  QDP_LOG_DEBUG("%s","qdp_profile_release EXIT");
  return QDP_SUCCESS;
}

/*===========================================================================
  FUNCTION:  qdp_profile_release_ex
===========================================================================*/
/*!
    @brief
    Deletes the profile irrespective of the ref count.

    @params
    profile_id: profile id

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_release_ex
(
  unsigned int profile_id
)
{
  QDP_LOG_DEBUG("%s","qdp_profile_release_ex ENTRY");
  QDP_INIT_BARRIER;
  int ret = QDP_FAILURE;
  ret = manager_delete_profile(profile_id);
  release_all_key_instances(profile_id);
  QDP_LOG_DEBUG("%s","qdp_profile_release_ex EXIT");
  return QDP_SUCCESS;
}

/*===========================================================================
  FUNCTION:  qdp_init
===========================================================================*/
/*!
    @brief
    initializes qdp module in legacy mode

    @params
    default_port: default qmi port to be used for QDP operations

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_init
(
  const char * default_port
)
{
  return qdp_init2(default_port,FALSE);
}

/*===========================================================================
  FUNCTION:  qdp_init2
===========================================================================*/
/*!
    @brief
    initializes qdp module

    @params
    default_port: default qmi port to be used for QDP operations
    isAPAssist : whether device is in AP Assist mode or legacy

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_init2
(
  const char * default_port,
  boolean isAPAssist
)
{
  return qdp_init3(default_port,
                   isAPAssist,
                   FALSE);
}

/*===========================================================================
  FUNCTION:  qdp_init2
===========================================================================*/
/*!
    @brief
    initializes qdp module

    @params
    default_port: default qmi port to be used for QDP operations
    isAPAssist : whether device is in AP Assist mode or legacy

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_init3
(
  const char * default_port,
  boolean isAPAssist,
  boolean profile_update_value
)
{
  QDP_LOG_DEBUG("%s","qdp_init ENTRY");

  if (TRUE == qdp_inited)
  {
    QDP_LOG_ERROR("%s","qdp already inited");
    return QDP_SUCCESS;
  }
  qdp_platform_init();
  qdp_inited = TRUE;
  is_apassist = isAPAssist;

  qdp_profile_cleanup();
  profile_update = profile_update_value;

  return QDP_SUCCESS;
}

/*===========================================================================
  FUNCTION:  qdp_set_subscription
===========================================================================*/
/*!
    @brief
    Sets the appropriate subscription as a result the WDS client get binded to this subscription

    @params
    subs_id:  Subscription ID

    @return
    QDP_SUCCESS
    QDP_FAILURE

    @notes
       Dependencies
    - qdp_init() must be called for the associated port first.
*/
/*=========================================================================*/
int qdp_set_subscription
(
  int subs_id
)
{
  QDP_LOG_DEBUG("%s","qdp_set_subscription ENTRY");

  if(!qdp_inited) {
    QDP_LOG_DEBUG("%s", "qdp not initied");
    return QDP_FAILURE;
  }

  manager_init(subs_id, profile_update, is_apassist);
  manager_inited = TRUE;
  QDP_LOG_DEBUG("%s","qdp_set_subscription EXIT success");
  return QDP_SUCCESS;
}


/*===========================================================================
  FUNCTION:  qdp_deinit
===========================================================================*/
/*!
    @brief
    free up resources acquired during initialization

    @params

    @return
    none
*/
/*=========================================================================*/
void qdp_deinit
(
  void
)
{
  QDP_LOG_DEBUG("%s","qdp_deinit ENTRY");
  if (TRUE != qdp_inited || TRUE != manager_inited)
  {
    QDP_LOG_ERROR("%s","qdp not inited yet");
  }

  qdp_inited = FALSE;

  /* clean up profiles created by qdp module */
  qdp_profile_cleanup();
}
