/*===========================================================================

  Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "QdpAdapter.h"
#include "Manager.h"
#include "wireless_data_service_v01.h"

using namespace rildata;
using namespace qdp;

AuthType convertToQdp(ApnAuthType_t input)
{
    switch (input) {
        case ApnAuthType_t::NO_PAP_NO_CHAP:
            return AuthType::NONE;
        case ApnAuthType_t::PAP_NO_CHAP:
            return AuthType::PAP;
        case ApnAuthType_t::NO_PAP_CHAP:
            return AuthType::CHAP;
        case ApnAuthType_t::PAP_CHAP:
            return AuthType::PAP_CHAP;
        default:
            return (AuthType)input;
    }
}

TechType convertToQdp(DataProfileInfoType_t input)
{
    switch (input) {
        case DataProfileInfoType_t::THREE_GPP:
            return TechType::THREE_GPP;
        case DataProfileInfoType_t::THREE_GPP2:
            return TechType::THREE_GPP2;
        default:
            return (TechType)input;
    }
}

std::optional<ApnClass> convertToQdp(DataProfileId_t input)
{
    switch (input) {
        case DataProfileId_t::DEFAULT:
            return ApnClass::INET;
        case DataProfileId_t::IMS:
            return ApnClass::IMS;
        case DataProfileId_t::FOTA:
            return ApnClass::ADMN;
        case DataProfileId_t::CBS:
            return ApnClass::APP;
        default:
            return std::optional<ApnClass>();
    }
}

uint32_t convertToQdp(ApnTypes_t input)
{
    return (uint32_t)input;
}

IPType convertProtocolToQdp(std::string protocol)
{
    if (protocol == "IP") {
        return IPType::V4;
    } else if (protocol == "IPV6") {
        return IPType::V6;
    } else if (protocol == "IPV4V6") {
        return IPType::V4V6;
    } else {
        return IPType::UNKNOWN;
    }
}

ProfileParams convertDataProfileInfoToProfileParams(const DataProfileInfo_t& dataProfileInfo)
{
    ProfileParams params;
    params.setHomeIPType(convertProtocolToQdp(dataProfileInfo.protocol))
        .setRoamIPType(convertProtocolToQdp(dataProfileInfo.roamingProtocol))
        .setAuthType(convertToQdp(dataProfileInfo.authType))
        .setIsApnDisabled(!dataProfileInfo.enableProfile)
        .setApnTypesMask(convertToQdp(dataProfileInfo.supportedApnTypesBitmap));

    if (dataProfileInfo.maxConns > 0) {
        params.setMaxPdnsConnPerBlock(dataProfileInfo.maxConns);
    }
    if (dataProfileInfo.maxConnsTime > 0) {
        params.setMaxPdnConnTimer(dataProfileInfo.maxConnsTime);
    }
    if (dataProfileInfo.waitTime > 0) {
        params.setPdnReqWaitInterval(dataProfileInfo.waitTime);
    }
    auto apnClass = convertToQdp(dataProfileInfo.profileId);
    if (apnClass) {
        params.setApnClass(*apnClass);
    }
    params.setApnName(dataProfileInfo.apn);
    if (!dataProfileInfo.username.empty()) {
        params.setUserName(dataProfileInfo.username);
    }
    if (!dataProfileInfo.password.empty()) {
        params.setPassword(dataProfileInfo.password);
    }
    return params;
}

ApnAuthType_t convertFromQdp(AuthType input)
{
    switch (input) {
        case AuthType::NONE:
            return ApnAuthType_t::NO_PAP_NO_CHAP;
        case AuthType::PAP:
            return ApnAuthType_t::PAP_NO_CHAP;
        case AuthType::CHAP:
            return ApnAuthType_t::NO_PAP_CHAP;
        case AuthType::PAP_CHAP:
            return ApnAuthType_t::PAP_CHAP;
        default:
            return (ApnAuthType_t)input;
    }
}

DataProfileInfoType_t convertFromQdp(TechType input)
{
    switch (input) {
        case TechType::THREE_GPP:
            return DataProfileInfoType_t::THREE_GPP;
        case TechType::THREE_GPP2:
            return DataProfileInfoType_t::THREE_GPP2;
        default:
            return (DataProfileInfoType_t)input;
    }
}

DataProfileId_t convertFromQdp(ApnClass input)
{
    switch (input) {
        case ApnClass::INET:
            return DataProfileId_t::DEFAULT;
        case ApnClass::IMS:
            return DataProfileId_t::IMS;
        case ApnClass::ADMN:
            return DataProfileId_t::FOTA;
        case ApnClass::APP:
            return DataProfileId_t::CBS;
        default:
            return (DataProfileId_t)input;
    }
}

std::string convertFromQdp(IPType input)
{
    switch (input) {
        case IPType::V4:
            return "IP";
        case IPType::V6:
            return "IPV6";
        case IPType::V4V6:
            return "IPV4V6";
        default:
            return "IPV4V6";
    }
}

DataProfileInfo_t convertProfileParamsToDataProfileInfo(const ProfileParams& params)
{
    DataProfileInfo_t dataProfileInfo = {};
    if (params.hasApnClass()) {
        dataProfileInfo.profileId = convertFromQdp(params.getApnClass());
    }
    if (params.hasApnName()) {
        dataProfileInfo.apn = params.getApnName();
    }
    if (params.hasHomeIPType()) {
        dataProfileInfo.protocol = convertFromQdp(params.getHomeIPType());
    }
    if (params.hasRoamIPType()) {
        dataProfileInfo.roamingProtocol = convertFromQdp(params.getRoamIPType());
    }
    if (params.hasAuthType()) {
        dataProfileInfo.authType = convertFromQdp(params.getAuthType());
    }
    if (params.hasUserName()) {
        dataProfileInfo.username = params.getUserName();
    }
    if (params.hasPassword()) {
        dataProfileInfo.password = params.getPassword();
    }
    if (params.hasMaxPdnsConnPerBlock()) {
        dataProfileInfo.maxConns = params.getMaxPdnsConnPerBlock();
    }
    if (params.hasMaxPdnConnTimer()) {
        dataProfileInfo.maxConnsTime = params.getMaxPdnConnTimer();
    }
    if (params.hasPdnReqWaitInterval()) {
        dataProfileInfo.waitTime = params.getPdnReqWaitInterval();
    }
    if (params.hasIsApnDisabled()) {
        dataProfileInfo.enableProfile = !params.getIsApnDisabled();
    }
    if (params.hasApnTypesMask()) {
        dataProfileInfo.supportedApnTypesBitmap = (ApnTypes_t)params.getApnTypesMask();
    }
    return dataProfileInfo;
}

ApnTypes_t getApnTypesForName(std::string apn) {
    FilterProfileParams fpp;
    fpp.setFilterByTechType(TechType::THREE_GPP).setApnName(apn);
    std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
    if (foundProfiles.empty()) {
        fpp.setFilterByTechType(TechType::THREE_GPP2);
        foundProfiles = Manager::getInstance().filter(fpp);
    }
    if (!foundProfiles.empty()) {
        std::optional<ProfileParams> profile = (Manager::getInstance().getProfile(foundProfiles.front()));
        if (profile && profile->hasApnTypesMask()) {
            return (ApnTypes_t)profile->getApnTypesMask();
        }
    }
    return ApnTypes_t::NONE;
}

int getProfileCountForApnType(ApnTypes_t apn) {
    FilterProfileParams fpp;
    fpp.setFilterByTechType(TechType::THREE_GPP);
    fpp.setApnTypesMask(convertToQdp(apn));
    std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
    return foundProfiles.size();
}

int convertInstanceIdToSubId(qcril_instance_id_e_type instanceId) {
    switch (instanceId)
    {
       case QCRIL_DEFAULT_INSTANCE_ID: return WDS_PRIMARY_SUBS_V01;
       case QCRIL_SECOND_INSTANCE_ID: return WDS_SECONDARY_SUBS_V01;
       case QCRIL_THIRD_INSTANCE_ID: return WDS_TERTIARY_SUBS_V01;
       default: return WDS_DEFAULT_SUBS_V01;
    }
}

bool isNullProfile(uint16_t profileId) {
    FilterProfileParams fpp;
    fpp.setFilterByProfileId((ProfileId)profileId);
    std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
    bool ret = FALSE;
    if (!foundProfiles.empty()) {
        std::optional<ProfileParams> profile = (Manager::getInstance().getProfile(foundProfiles.front()));
        if (profile && profile->hasApnName() && (profile->getApnName().compare("") == 0)) {
              ret = TRUE;
        }
    }
    return ret;
}

int getProfileId(wds_pdp_type_enum_v01 protocol, std::string apn)
{
    FilterProfileParams fpp;
    fpp.setApnName(apn);
    if (protocol == WDS_PDP_TYPE_PDP_IPV4_V01) {
      fpp.setHomeIPType(IPType::V4);
    } else if (protocol == WDS_PDP_TYPE_PDP_IPV6_V01) {
      fpp.setHomeIPType(IPType::V6);
    } else {
      fpp.setHomeIPType(IPType::V4V6);
    }
    auto matches = Manager::getInstance().filter(fpp);
    for (auto key : matches) {
      if (key) {
        return (static_cast<int>(key->getProfileId()));
      }
    }
    return -1;
}

void getApnNameProtocol (uint16_t profileId, std::string &apnName, qdp::IPType &protocol)
{
    FilterProfileParams fpp;
    fpp.setFilterByProfileId((ProfileId)profileId);
    std::list<std::shared_ptr<IProfileKey>> foundProfiles = Manager::getInstance().filter(fpp);
    if (!foundProfiles.empty()) {
        std::optional<ProfileParams> profile = (Manager::getInstance().getProfile(foundProfiles.front()));
        if (profile && profile->hasApnName()) {
          apnName = profile->getApnName();
        }
        if (profile && profile->hasHomeIPType()) {
            protocol = profile->getHomeIPType();
        }
    }
}

wds_pdp_type_enum_v01 convertToWdsPdpType(qdp::IPType &type)
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