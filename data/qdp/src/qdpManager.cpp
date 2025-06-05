/*===========================================================================

  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "Util.h"
#include "qdpManager.h"
#include <map>
#include <string>

using namespace qdp;

Util util;
std::multimap<unsigned int, std::shared_ptr<qdp::IProfileKey>> keyList;
boolean apAssist = FALSE;

AuthType convert_to_auth_type
(
  const char *auth
)
{
  if (!strncmp(auth, QDP_RIL_PAP_CHAP_NOT_ALLOWED, sizeof(QDP_RIL_PAP_CHAP_NOT_ALLOWED)))
    return AuthType::NONE;
  else if (!strncmp(auth, QDP_RIL_PAP_ONLY_ALLOWED, sizeof(QDP_RIL_PAP_ONLY_ALLOWED)))
    return AuthType::PAP;
  else if (!strncmp(auth, QDP_RIL_CHAP_ONLY_ALLOWED, sizeof(QDP_RIL_CHAP_ONLY_ALLOWED)))
    return AuthType::CHAP;
  else
    return AuthType::PAP_CHAP;
}

IPType convert_to_ip_type
(
  const char *ip
)
{
  if (!strncasecmp(ip, QDP_RIL_IP_4, strlen(QDP_RIL_IP_4)+1))
    return IPType::V4;
  else if (!strncasecmp(ip, QDP_RIL_IP_6, strlen(QDP_RIL_IP_6)+1))
    return IPType::V6;
  else if (!strncasecmp(ip, QDP_RIL_IP_4_6, strlen(QDP_RIL_IP_4_6)+1))
    return IPType::V4V6;
  else
    return IPType::UNKNOWN;
}

ApnTypes convert_to_apn_type
(
  const char *apnType
)
{
  uint32_t type = atoi(apnType);
  switch (type) {
    case WDS_APN_TYPE_MASK_DEFAULT_V01:
      return ApnTypes::DEFAULT;
    case WDS_APN_TYPE_MASK_MMS_V01:
      return ApnTypes::MMS;
    case WDS_APN_TYPE_MASK_SUPL_V01:
      return ApnTypes::SUPL;
    case WDS_APN_TYPE_MASK_DUN_V01:
      return ApnTypes::DUN;
    case WDS_APN_TYPE_MASK_HIPRI_V01:
      return ApnTypes::HIPRI;
    case WDS_APN_TYPE_MASK_FOTA_V01:
      return ApnTypes::FOTA;
    case WDS_APN_TYPE_MASK_IMS_V01:
      return ApnTypes::IMS;
    case WDS_APN_TYPE_MASK_CBS_V01:
      return ApnTypes::CBS;
    case WDS_APN_TYPE_MASK_IA_V01:
      return ApnTypes::IA;
    case WDS_APN_TYPE_MASK_EMERGENCY_V01:
      return ApnTypes::EMERGENCY;
    case WDS_APN_TYPE_MASK_MCX_V01:
      return ApnTypes::MCX;
    case WDS_APN_TYPE_MASK_UT_V01:
      return ApnTypes::XCAP;
    default:
      return ApnTypes::NONE;
  }
}

IPType convert_from_pdp_type
(
  wds_pdp_type_enum_v01 type
)
{
  switch(type) {
    case WDS_PDP_TYPE_PDP_IPV4_V01:
      return IPType::V4;
    case WDS_PDP_TYPE_PDP_IPV6_V01:
      return IPType::V6;
    case WDS_PDP_TYPE_PDP_IPV4V6_V01:
      return IPType::V4V6;
    default:
      return IPType::UNKNOWN;
  }
}

IPType convert_from_wds_common_pdp_type
(
  wds_common_pdp_type_enum_v01 type
)
{
  switch(type) {
    case WDS_COMMON_PDP_TYPE_PDP_IPV4_V01:
      return IPType::V4;
    case WDS_COMMON_PDP_TYPE_PDP_IPV6_V01:
      return IPType::V6;
    case WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01:
      return IPType::V4V6;
    default:
      return IPType::UNKNOWN;
  }
}

IPType convert_from_wds_pdn_type
(
  wds_profile_pdn_type_enum_v01 type
)
{
  switch(type) {
    case WDS_PROFILE_PDN_TYPE_IPV4_V01:
      return IPType::V4;
    case WDS_PROFILE_PDN_TYPE_IPV6_V01:
      return IPType::V6;
    case WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01:
      return IPType::V4V6;
    default:
      return IPType::UNKNOWN;
  }
}

AuthType convert_from_wds_auth_type
(
  wds_auth_pref_mask_v01 type
)
{
   AuthType result = AuthType::NONE;
   if (type == QMI_WDS_MASK_AUTH_PREF_PAP_V01) {
     result = AuthType::PAP;
   } else if (type == QMI_WDS_MASK_AUTH_PREF_CHAP_V01) {
     result = AuthType::CHAP;
   } else if(type == (QMI_WDS_MASK_AUTH_PREF_PAP_V01 | QMI_WDS_MASK_AUTH_PREF_CHAP_V01))
     result = AuthType::PAP_CHAP;
   return result;
}

void convert_to_profile_params
(
  ProfileParams &params,
  const char ** param_strings
)
{
  if(param_strings[QDP_RIL_IP_FAMILY] != NULL)
    params.setHomeIPType(convert_to_ip_type(param_strings[QDP_RIL_IP_FAMILY]));
  if(param_strings[QDP_RIL_IP_ROAMING] != NULL)
    params.setRoamIPType(convert_to_ip_type(param_strings[QDP_RIL_IP_ROAMING]));
  if(param_strings[QDP_RIL_AUTH] != NULL)
    params.setAuthType(convert_to_auth_type(param_strings[QDP_RIL_AUTH]));
  if(param_strings[QDP_RIL_APN] != NULL)
    params.setApnName(param_strings[QDP_RIL_APN]);
  if(param_strings[QDP_RIL_NAI] != NULL)
    params.setUserName(param_strings[QDP_RIL_NAI]);
  if(param_strings[QDP_RIL_PASSWORD] != NULL)
    params .setPassword(param_strings[QDP_RIL_PASSWORD]);
  if(param_strings[QDP_RIL_APN_TYPE_BITMASK] != NULL)
    params.setApnTypesMask((uint32_t)convert_to_apn_type(param_strings[QDP_RIL_APN_TYPE_BITMASK]));
}

void convert_to_modify_profile_params_from_qdp_profile
(
  ModifyProfileParams &params,
  qdp_profile_info_type *profile
)
{
  if(profile->prof_id.profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    if(profile->prof_set.apn_name_valid)
      params.setApnName(profile->prof_set.apn_name);
    if (profile->prof_set.username_valid)
      params.setUserName(profile->prof_set.username);
    if (profile->prof_set.password_valid)
      params.setPassword(profile->prof_set.password);
    if (profile->prof_set.authentication_preference_valid)
      params.setAuthType(convert_from_wds_auth_type(profile->prof_set.authentication_preference));
    if(profile->prof_set.pdp_type_valid)
      params.setHomeIPType(convert_from_pdp_type(profile->prof_set.pdp_type));
  }
  else if(profile->prof_id.profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    if(profile->prof_set.apn_string_valid)
      params.setApnName(profile->prof_set.apn_string);
    if(profile->prof_set.user_id_valid)
      params.setUserName(profile->prof_set.user_id);
    if(profile->prof_set.auth_password_valid)
      params.setPassword(profile->prof_set.auth_password);
    if(profile->prof_set.auth_protocol_valid)
      params.setAuthType((AuthType)profile->prof_set.auth_protocol);
    if(profile->prof_set.pdn_type_valid)
      params.setHomeIPType(convert_from_wds_pdn_type(profile->prof_set.pdn_type));
  }
}

void convert_to_modify_profile_params
(
  ModifyProfileParams &params,
  wds_modify_profile_settings_req_msg_v01 *p_params
)
{
  if(p_params->max_pdn_conn_per_block_valid)
    params.setMaxPdnsConnPerBlock(p_params->max_pdn_conn_per_block);
  if(p_params->max_pdn_conn_timer_valid)
    params.setMaxPdnConnTimer(p_params->max_pdn_conn_timer);
  if(p_params->pdn_req_wait_interval_valid)
    params.setPdnReqWaitInterval(p_params->pdn_req_wait_interval);
  if(p_params->apn_disabled_flag_valid)
    params.setIsApnDisabled(p_params->apn_disabled_flag);

  if(p_params->profile.profile_type  == WDS_PROFILE_TYPE_3GPP_V01)
  {
    if(p_params->apn_name_valid)
      params.setApnName(p_params->apn_name);
    if(p_params->username_valid)
      params.setUserName(p_params->username);
    if(p_params->password_valid)
      params.setPassword(p_params->password);
    if(p_params->authentication_preference_valid)
      params.setAuthType(convert_from_wds_auth_type(p_params->authentication_preference));
    if(p_params->pdp_type_valid)
      params.setHomeIPType(convert_from_pdp_type(p_params->pdp_type));
    if(p_params->lte_roaming_pdp_type_valid && p_params->umts_roaming_pdp_type_valid &&
       p_params->lte_roaming_pdp_type == p_params->umts_roaming_pdp_type)
         params.setRoamIPType(convert_from_wds_common_pdp_type(p_params->lte_roaming_pdp_type));
    }

  else if(p_params->profile.profile_type  == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    if(p_params->apn_string_valid)
      params.setApnName(p_params->apn_string);
    if(p_params->user_id_valid)
      params.setUserName(p_params->user_id);
    if(p_params->pdn_level_auth_password_valid)
      params.setPassword(p_params->pdn_level_auth_password);
    if(p_params->auth_protocol_valid)
      params.setAuthType((AuthType)p_params->auth_protocol);
    if(p_params->pdn_type_valid)
      params.setHomeIPType(convert_from_wds_pdn_type(p_params->pdn_type));
  }
}

void convert_to_add_profile_params
(
  AddProfileParams &params,
  const char **param_strings
)
{
  if(param_strings[QDP_RIL_IP_FAMILY] != NULL)
    params.setHomeIPType(convert_to_ip_type(param_strings[QDP_RIL_IP_FAMILY]));
  if(param_strings[QDP_RIL_IP_ROAMING] != NULL)
    params.setRoamIPType(convert_to_ip_type(param_strings[QDP_RIL_IP_ROAMING]));
  if(param_strings[QDP_RIL_AUTH] != NULL)
    params.setAuthType(convert_to_auth_type(param_strings[QDP_RIL_AUTH]));
  if(param_strings[QDP_RIL_APN] != NULL)
    params.setApnName(param_strings[QDP_RIL_APN]);
  if(param_strings[QDP_RIL_NAI] != NULL)
    params.setUserName(param_strings[QDP_RIL_NAI]);
  if(param_strings[QDP_RIL_PASSWORD] != NULL)
    params .setPassword(param_strings[QDP_RIL_PASSWORD]);
  if(apAssist && param_strings[QDP_RIL_APN_TYPE_BITMASK] != NULL)
    params.setApnTypesMask((uint32_t)convert_to_apn_type(param_strings[QDP_RIL_APN_TYPE_BITMASK]));
}

wds_auth_pref_mask_v01 convert_to_wds_auth
(
  AuthType type
)
{
  wds_auth_pref_mask_v01 result = 0;
  if (((uint32_t)type & (uint32_t)AuthType::PAP) == (uint32_t)AuthType::PAP) {
    result |= QMI_WDS_MASK_AUTH_PREF_PAP_V01;
  }
  if (((uint32_t)type & (uint32_t)AuthType::CHAP) == (uint32_t)AuthType::CHAP) {
    result |= QMI_WDS_MASK_AUTH_PREF_CHAP_V01;
  }
  return result;
}

wds_common_pdp_type_enum_v01 convert_to_common_pdp_type
(
  IPType ip
)
{
  switch(ip) {
    case IPType::UNKNOWN:
      return WDS_COMMON_PDP_TYPE_PDP_MAX_V01;
    case IPType::V4:
      return WDS_COMMON_PDP_TYPE_PDP_IPV4_V01;
    case IPType::V6:
      return WDS_COMMON_PDP_TYPE_PDP_IPV6_V01;
    default:
      return WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01;
  }
}

wds_pdp_type_enum_v01 convert_to_pdp_type
(
  IPType type
)
{
  switch (type) {
    case IPType::V4:
      return WDS_PDP_TYPE_PDP_IPV4_V01;
    case IPType::V6:
      return WDS_PDP_TYPE_PDP_IPV6_V01;
    default:
      return WDS_PDP_TYPE_PDP_IPV4V6_V01;
  }
}

wds_profile_pdn_type_enum_v01 convert_to_pdn_type
(
  const char *ip
)
{
  if (!strncasecmp(ip, QDP_RIL_IP_4, strlen(QDP_RIL_IP_4)+1))
    return WDS_PROFILE_PDN_TYPE_IPV4_V01;
  else if (!strncasecmp(ip, QDP_RIL_IP_6, strlen(QDP_RIL_IP_6)+1))
    return WDS_PROFILE_PDN_TYPE_IPV6_V01;
  else if (!strncasecmp(ip, QDP_RIL_IP_4_6, strlen(QDP_RIL_IP_4_6)+1))
    return WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01;
  else
    return WDS_PROFILE_PDN_TYPE_UNSPECIFIED_V01;
}

wds_profile_auth_protocol_enum_v01 convert_to_wds_auth_protocol
(
  const char *auth
)
{
  if (!strncmp(auth, QDP_RIL_PAP_CHAP_NOT_ALLOWED, sizeof(QDP_RIL_PAP_CHAP_NOT_ALLOWED)))
    return WDS_PROFILE_AUTH_PROTOCOL_NONE_V01;
  else if (!strncmp(auth, QDP_RIL_PAP_ONLY_ALLOWED, sizeof(QDP_RIL_PAP_ONLY_ALLOWED)))
    return WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;
  else if (!strncmp(auth, QDP_RIL_CHAP_ONLY_ALLOWED, sizeof(QDP_RIL_CHAP_ONLY_ALLOWED)))
    return WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;
  else
    return WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
}

qdp_profile_pdn_type convert_to_qdp_pdn_type
(
  IPType ip
)
{
  switch(ip)
  {
    case IPType::V4:
      return QDP_PROFILE_PDN_TYPE_IPV4;
    case IPType::V6:
      return QDP_PROFILE_PDN_TYPE_IPV6;
    case IPType::V4V6:
      return QDP_PROFILE_PDN_TYPE_IPV4V6;
    default:
      return QDP_PROFILE_PDN_TYPE_INVALID;
  }
}

wds_apn_type_mask_v01 convert_to_single_qmi_apn_type
(
  ApnTypes type
)
{
  switch (type) {
    case ApnTypes::DEFAULT:
      return WDS_APN_TYPE_MASK_DEFAULT_V01;
    case ApnTypes::MMS:
      return WDS_APN_TYPE_MASK_MMS_V01;
    case ApnTypes::SUPL:
      return WDS_APN_TYPE_MASK_SUPL_V01;
    case ApnTypes::DUN:
      return WDS_APN_TYPE_MASK_DUN_V01;
    case ApnTypes::HIPRI:
      return WDS_APN_TYPE_MASK_HIPRI_V01;
    case ApnTypes::FOTA:
      return WDS_APN_TYPE_MASK_FOTA_V01;
    case ApnTypes::IMS:
      return WDS_APN_TYPE_MASK_IMS_V01;
    case ApnTypes::CBS:
      return WDS_APN_TYPE_MASK_CBS_V01;
    case ApnTypes::IA:
      return WDS_APN_TYPE_MASK_IA_V01;
    case ApnTypes::EMERGENCY:
      return WDS_APN_TYPE_MASK_EMERGENCY_V01;
    case ApnTypes::MCX:
      return WDS_APN_TYPE_MASK_MCX_V01;
    case ApnTypes::XCAP:
      return WDS_APN_TYPE_MASK_UT_V01;
    default:
      return WDS_APN_TYPE_MASK_UNSPECIFIED_V01;
  }
}

wds_apn_type_mask_v01 convert_to_wds_apn_type(uint32_t types)
{
  wds_apn_type_mask_v01 result = 0;
  for (uint32_t i = 1; i < static_cast<int32_t>(ApnTypes::MAX); i <<= 1) {
      bool matchApnType = (i & types) == i;
      if (matchApnType) {
      result |= convert_to_single_qmi_apn_type(static_cast<ApnTypes>(i));
    }
  }
  return result;
}

void set_error_info
(
  IPType ip,
  std::string apnName,
  qdp_error_info_t *error_info,
  TechType tech,
  bool isHomeIP
)
{
  qdp_tech_t qdpTech = (tech == TechType::THREE_GPP)?QDP_3GPP:QDP_3GPP2;
  FilterProfileParams fpp;
  std::list<std::shared_ptr<IProfileKey>> foundProfile;
  if(ip == IPType::V4) {
    if(isHomeIP)
      fpp.setFilterByTechType(tech).setFilterByIsQdpProfile(false).setHomeIPType(IPType::V6).setApnName(apnName);
    else
      fpp.setFilterByTechType(tech).setFilterByIsQdpProfile(false).setRoamIPType(IPType::V6).setApnName(apnName);
    foundProfile = Manager::getInstance().filter(fpp);
    if(foundProfile.size()>0) {
      error_info->error =  QDP_ERROR_ONLY_IPV6_ALLOWED;
      error_info->tech = qdpTech;
    }

  } else if(ip == IPType::V6) {
    if(isHomeIP)
      fpp.setFilterByTechType(tech).setFilterByIsQdpProfile(false).setHomeIPType(IPType::V4).setApnName(apnName);
    else
      fpp.setFilterByTechType(tech).setFilterByIsQdpProfile(false).setRoamIPType(IPType::V4).setApnName(apnName);
    foundProfile = Manager::getInstance().filter(fpp);
    if(foundProfile.size()>0) {
      error_info->error =  QDP_ERROR_ONLY_IPV4_ALLOWED;
      error_info->tech = qdpTech;
    }
  }
}

void manager_init
(
  int subId,
  boolean updateAllowed,
  boolean isApAssist
)
{
  util.setProfileUpdateAllowed(updateAllowed);
  Manager::getInstance().init(subId);
  apAssist = isApAssist;
}

ProfileParams get_profile_params_from_key
(
  std::shared_ptr<IProfileKey> key
)
{
  ProfileParams p_params;
  std::optional<ProfileParams> profile = (Manager::getInstance().getProfile(key));
  if(profile.has_value()) {
    p_params = profile.value();
  }
  return p_params;
}

int manager_modify_profile
(
  ModifyProfileParams params,
  std::shared_ptr<IProfileKey> key
)
{
  bool ret_val = Manager::getInstance().modifyProfile(params, key);
  if(ret_val) {
    return QDP_SUCCESS;
  }
  return QDP_FAILURE;
}


int manager_match_lte_attach_params
(
  unsigned int profile_id,
  const char **param_strings
)
{
  int ret = QDP_FAILURE;
  ProfileParams params;
  convert_to_profile_params(params,param_strings);
  if(util.matchLteAttachProfileId(profile_id, params))
  {
    ret = QDP_SUCCESS;
  }
  return ret;
}

int manager_find_or_create_profile_for_lte_attach
(
  const char **param_strings,
  unsigned int *profile_id,
  qdp_profile_pdn_type *pdn_type,
  qdp_error_info_t *error_info
)
{
  std::shared_ptr<IProfileKey> attachProfile;
  ProfileParams params;
  convert_to_profile_params(params, param_strings);

  attachProfile = util.findOrCreateProfileForLteAttach(params);
  *profile_id = QDP_INVALID_PROFILE;
  if(attachProfile != nullptr) {
    *profile_id = attachProfile->getProfileId();
    ProfileParams attach_params = get_profile_params_from_key(attachProfile);
    if(attach_params.hasHomeIPType()) {
      *pdn_type = convert_to_qdp_pdn_type(attach_params.getHomeIPType());
    }
  } else {
    //set error info and return if attachProfile is null
    IPType ip = IPType::UNKNOWN;
    std::string apn;
    if(param_strings[QDP_RIL_APN] != nullptr) {
      apn = param_strings[QDP_RIL_APN];
    }
    if(param_strings[QDP_RIL_IP_FAMILY] != nullptr) {
      ip = convert_to_ip_type(param_strings[QDP_RIL_IP_FAMILY]);
      if (ip == IPType::V4 || ip == IPType::V6) {
        set_error_info(ip, apn, error_info, TechType::THREE_GPP, TRUE);
      }
    }
  }
  return QDP_SUCCESS;
}

int manager_find_or_create_profile_for_setup_data
(
  const char** param_strings,
  unsigned int *profile_id,
  qdp_profile_pdn_type *pdn,
  qdp_error_info_t *error_info,
  qdp_tech_t tech
)
{
  ProfileParams params;
  *profile_id = QDP_INVALID_PROFILE;
  std::shared_ptr<IProfileKey> keyVal;
  convert_to_profile_params(params, param_strings);

  TechType tt = (tech==QDP_3GPP)?(TechType::THREE_GPP):(TechType::THREE_GPP2);
  keyVal = util.findOrCreateProfileForSetupDataCall(tt, params);

  if(keyVal != nullptr) {
    *profile_id = keyVal->getProfileId();
    keyList.insert({*profile_id, keyVal});
    ProfileParams p_params = get_profile_params_from_key(keyVal);
    if(p_params.hasHomeIPType()) {
      *pdn = convert_to_qdp_pdn_type(p_params.getHomeIPType());
    }
  } else {
    //setting error_info
    IPType ip = IPType::UNKNOWN;
    std::string apn;
    if(param_strings[QDP_RIL_APN] != nullptr) {
        apn = param_strings[QDP_RIL_APN];
    }
    if(param_strings[QDP_RIL_IP_FAMILY] != nullptr) {
      ip = convert_to_ip_type(param_strings[QDP_RIL_IP_FAMILY]);
      if (ip == IPType::V4 || ip == IPType::V6) {
        set_error_info(ip, apn, error_info, tt, TRUE);
      }
    }
    if(tech == QDP_3GPP && param_strings[QDP_RIL_IP_ROAMING] != nullptr) {
      ip = convert_to_ip_type(param_strings[QDP_RIL_IP_ROAMING]);
      if(ip == IPType::V4 || ip == IPType::V6) {
        set_error_info(ip, apn, error_info, tt, FALSE);
      }
    }
  }
  return QDP_SUCCESS;
}

int manager_find_or_create_profile_for_emergency_call
(
  const char **qdpParams,
  unsigned int *profileId,
  qdp_tech_t *techType,
  qdp_profile_pdn_type *pdn,
  char *apn
)
{
  ProfileParams params;
  std::shared_ptr<IProfileKey> key3gppVal;
  convert_to_profile_params(params, qdpParams);
  key3gppVal = util.findOrCreateProfileForEmergencyDataCall(params);

  if(key3gppVal == nullptr)
  {
    *profileId = QDP_INVALID_PROFILE;
    return QDP_SUCCESS;
  }

  *profileId = key3gppVal->getProfileId();
  *techType = QDP_3GPP;
  keyList.insert({*profileId, key3gppVal});

  ProfileParams p_params = get_profile_params_from_key(key3gppVal);
  if (p_params.hasApnName() && apn != nullptr) {
    std::string apnName = p_params.getApnName();
    strlcpy(apn, apnName.c_str(), apnName.size());
  }
  if(p_params.hasHomeIPType()) {
      *pdn = convert_to_qdp_pdn_type(p_params.getHomeIPType());
  }
  return QDP_SUCCESS;
}

int manager_find_profile_by_apn_class
(
  int apnclass,
  unsigned int *profile_id_list,
  uint8 *profile_id_list_len,
  qdp_profile_pdn_type *profile_pdn_type,
  qdp_tech_t tech
)
{
  FilterProfileParams fpp;
  TechType tt = (tech == QDP_3GPP)?(TechType::THREE_GPP):(TechType::THREE_GPP2);
  fpp.setFilterByTechType(tt).setApnClass((ApnClass)apnclass);
  std::list<std::shared_ptr<IProfileKey>> profileList = Manager::getInstance().filter(fpp);
  if(!profileList.empty()) {
    for(auto it = profileList.begin(); it!= profileList.end(); it++) {
      ProfileParams params = get_profile_params_from_key(*it);
      profile_id_list[*profile_id_list_len] = (*it)->getProfileId();
      *profile_pdn_type = convert_to_qdp_pdn_type(params.getHomeIPType());
      (*profile_id_list_len)++;
    }
  }
  return QDP_SUCCESS;
}

int manager_add_profile
(
  int apnClass,
  const char **param_strings,
  uint* profile_id_list,
  qdp_profile_pdn_type *profile_pdn_type,
  qdp_tech_t tech
)
{
  TechType tt = (tech == QDP_3GPP)?(TechType::THREE_GPP):(TechType::THREE_GPP2);
  AddProfileParams params(TRUE, tt, FALSE);
  convert_to_add_profile_params(params, param_strings);
  params.setApnClass((ApnClass)apnClass);
  std::shared_ptr<IProfileKey> profileKey = Manager::getInstance().addProfile(params);

  if(profileKey != nullptr) {
    profile_id_list[0] = profileKey->getProfileId();
    *profile_pdn_type = convert_to_qdp_pdn_type(params.getHomeIPType());
    keyList.insert({profileKey->getProfileId(), profileKey});
    return QDP_SUCCESS;
  }
  return QDP_FAILURE;
}

int manager_delete_profile
(
  unsigned int profile_id
)
{
  int ret = QDP_FAILURE;
  FilterProfileParams fpp;
  fpp.setFilterByProfileId((ProfileId)profile_id);
  std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
  if (!foundProfiles.empty()) {
     bool rc = Manager::getInstance().deleteProfile(foundProfiles.front());
     ret = (rc==TRUE)?QDP_SUCCESS:QDP_FAILURE;
  }
  return ret;
}

int qdp_3gpp_profile_update
(
  const char **param_strings,
  unsigned int profile_id,
  wds_modify_profile_settings_req_msg_v01 *modify_params,
  boolean *updateNeeded
)
{
  FilterProfileParams fpp;
  fpp.setFilterByProfileId((ProfileId)profile_id);
  std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
  if (!foundProfiles.empty()) {
    std::optional<ProfileParams> params = (Manager::getInstance().getProfile(foundProfiles.front()));
    modify_params->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
    modify_params->profile.profile_index = profile_id;

    if(params->hasMaxPdnsConnPerBlock()) {
      modify_params->max_pdn_conn_per_block_valid = TRUE;
      modify_params->max_pdn_conn_per_block = params->getMaxPdnsConnPerBlock();
    }
    if(params->hasMaxPdnConnTimer()) {
      modify_params->max_pdn_conn_timer_valid = TRUE;
      modify_params->max_pdn_conn_timer = params->getMaxPdnConnTimer();
    }
    if(params->hasPdnReqWaitInterval()) {
      modify_params->pdn_req_wait_interval_valid = TRUE;
      modify_params->pdn_req_wait_interval = params->getPdnReqWaitInterval();
    }
    if(params->hasIsApnDisabled()) {
      modify_params->apn_disabled_flag_valid = TRUE;
      modify_params->apn_disabled_flag = params->getIsApnDisabled();
    }

    if(param_strings[QDP_RIL_APN] != NULL) {
      if(!params->hasApnName() || (strncasecmp(param_strings[QDP_RIL_APN], params->getApnName().c_str(), (params->getApnName().size())+1))) {
        modify_params->apn_name_valid = TRUE;
        strlcpy(modify_params->apn_name, param_strings[QDP_RIL_APN],
                std::min(QMI_WDS_APN_NAME_MAX_V01, (int)strlen(param_strings[QDP_RIL_APN])) + 1);
        *updateNeeded = TRUE;
      }
    } else if(!params->hasApnName() || !params->getApnName().empty()) {
      modify_params->apn_name_valid = TRUE;
      modify_params->apn_name[0] = '\0';
      *updateNeeded = TRUE;
    }

    if(param_strings[QDP_RIL_NAI] != NULL) {
      if(!params->hasUserName()  || (strncasecmp(param_strings[QDP_RIL_NAI], params->getUserName().c_str(), (params->getUserName().size())+1))) {
        modify_params->username_valid = TRUE;
        strlcpy(modify_params->username, param_strings[QDP_RIL_NAI],
                std::min(QMI_WDS_USER_NAME_MAX_V01, (int)strlen(param_strings[QDP_RIL_NAI])) + 1);
        *updateNeeded = TRUE;
      }
    } else if(!params->hasUserName() || !params->getUserName().empty()) {
      modify_params->username_valid = TRUE;
      modify_params->username[0] = '\0';
      *updateNeeded = TRUE;
    }

    if(param_strings[QDP_RIL_PASSWORD] != NULL) {
      if(!params->hasPassword() || (strncasecmp(param_strings[QDP_RIL_PASSWORD], params->getPassword().c_str(), (params->getPassword().size())+1))) {
        modify_params->password_valid = TRUE;
        strlcpy(modify_params->password, param_strings[QDP_RIL_PASSWORD],
            std::min(QMI_WDS_PASSWORD_MAX_V01, (int)strlen(param_strings[QDP_RIL_PASSWORD])) + 1);
      }
    } else if(!params->hasPassword() || !params->getPassword().empty()) {
      modify_params->password_valid = TRUE;
      modify_params->password[0] = '\0';
      *updateNeeded = TRUE;
    }

    if(param_strings[QDP_RIL_AUTH] != NULL) {
      AuthType auth = convert_to_auth_type(param_strings[QDP_RIL_AUTH]);
      if(!params->hasAuthType() || auth != params->getAuthType()) {
        modify_params->authentication_preference_valid = TRUE;
        modify_params->authentication_preference =  convert_to_wds_auth(auth);
        *updateNeeded = TRUE;
      }
    }

    if(param_strings[QDP_RIL_IP_FAMILY] != NULL) {
      IPType ip = convert_to_ip_type(param_strings[QDP_RIL_IP_FAMILY]);
      if(!params->hasHomeIPType() || ip != params->getHomeIPType()) {
        modify_params->pdp_type_valid = TRUE;
        modify_params->pdp_type = convert_to_pdp_type(ip);
        *updateNeeded = TRUE;
      }
    }

    if(param_strings[QDP_RIL_IP_ROAMING] != NULL) {
      IPType ip = convert_to_ip_type(param_strings[QDP_RIL_IP_ROAMING]);
      if(!params->hasRoamIPType() || ip != params->getRoamIPType()) {
        modify_params->lte_roaming_pdp_type_valid = TRUE;
        modify_params->umts_roaming_pdp_type_valid = TRUE;
        modify_params->lte_roaming_pdp_type = convert_to_common_pdp_type(ip);
        modify_params->umts_roaming_pdp_type = modify_params->lte_roaming_pdp_type;
        *updateNeeded = TRUE;
      }
    }

    if(apAssist && param_strings[QDP_RIL_APN_TYPE_BITMASK] != NULL) {
      if(!params->hasApnTypesMask() || (uint32_t)(convert_to_apn_type(param_strings[QDP_RIL_APN_TYPE_BITMASK])) != params->getApnTypesMask()) {
        modify_params->apn_type_mask_valid = TRUE;
        modify_params->apn_type_mask = strtoul(param_strings[QDP_RIL_APN_TYPE_BITMASK], NULL, 0);
      }
    }
    return QDP_SUCCESS;
  }
  return QDP_FAILURE;
}

int qdp_3gpp2_profile_update
(
  const char **param_strings,
  unsigned int profile_id,
  wds_modify_profile_settings_req_msg_v01 *modify_params
)
{
  modify_params->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
  modify_params->profile.profile_index = profile_id;

  if(param_strings[QDP_RIL_APN] != NULL) {
    modify_params->apn_string_valid = TRUE;
    strlcpy(modify_params->apn_string, param_strings[QDP_RIL_APN],
            std::min(QMI_WDS_APN_NAME_MAX_V01, (int)strlen(param_strings[QDP_RIL_APN])) + 1);
  }

  if(param_strings[QDP_RIL_NAI] != NULL) {
    modify_params->user_id_valid  = TRUE;
    strlcpy(modify_params->user_id, param_strings[QDP_RIL_NAI],
            std::min(QMI_WDS_USER_NAME_MAX_V01, (int)strlen(param_strings[QDP_RIL_NAI])) + 1);
  }

  if(param_strings[QDP_RIL_PASSWORD] != NULL) {
    modify_params->pdn_level_auth_password_valid = TRUE;
    strlcpy(modify_params->pdn_level_auth_password, param_strings[QDP_RIL_PASSWORD], sizeof(modify_params->pdn_level_auth_password));
    modify_params->auth_password_valid = TRUE;
    strlcpy(modify_params->auth_password, param_strings[QDP_RIL_PASSWORD],
            std::min(QMI_WDS_PASSWORD_MAX_V01, (int)strlen(param_strings[QDP_RIL_PASSWORD])) + 1);
  }

  if(param_strings[QDP_RIL_AUTH] != NULL) {
    modify_params->auth_protocol_valid = TRUE;
    modify_params->auth_protocol =  convert_to_wds_auth_protocol(param_strings[QDP_RIL_AUTH]);
  }

  if(param_strings[QDP_RIL_IP_FAMILY] != NULL) {
    modify_params->pdn_type_valid = TRUE;
    modify_params->pdn_type = convert_to_pdn_type(param_strings[QDP_RIL_IP_FAMILY]);
  }
   return QDP_SUCCESS;
}

int qdp_modify_profile_param
(
  qdp_profile_info_type  *profile
)
{
  FilterProfileParams fpp;
  fpp.setFilterByProfileId((ProfileId)profile->prof_id.profile_index);
  std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
  if (!foundProfiles.empty()) {
    ModifyProfileParams params;
    convert_to_modify_profile_params_from_qdp_profile(params, profile);
    return manager_modify_profile(params, foundProfiles.front());
  }
  return QDP_FAILURE;
}

int qdp_update_profile_param
(
  wds_modify_profile_settings_req_msg_v01  *p_params
)
{
  FilterProfileParams fpp;
  fpp.setFilterByProfileId((ProfileId)p_params->profile.profile_index);
  std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
  if(!foundProfiles.empty()) {
    ModifyProfileParams params;
    convert_to_modify_profile_params(params, p_params);
    return manager_modify_profile(params, foundProfiles.front());
  }
  return QDP_FAILURE;
}

void release_key
(
  unsigned int profileId
)
{
  auto it = keyList.find(profileId);
  if(it != keyList.end()) {
    keyList.erase(it);
  }
}

void release_all_key_instances
(
  unsigned int profileId
)
{
  keyList.erase(profileId);
}

void cleanup_keyList()
{
  keyList.clear();
}
