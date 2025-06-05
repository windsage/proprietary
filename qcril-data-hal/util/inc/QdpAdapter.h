/*===========================================================================

  Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef QDPADAPTER
#define QDPADAPTER

#include "MessageCommon.h"
#include "ProfileParams.h"
#include "wireless_data_service_v01.h"

qdp::ProfileParams convertDataProfileInfoToProfileParams(const rildata::DataProfileInfo_t& dataProfileInfo);
rildata::DataProfileInfo_t convertProfileParamsToDataProfileInfo(const qdp::ProfileParams& dataProfileInfo);
rildata::ApnTypes_t getApnTypesForName(std::string apn);
int getProfileCountForApnType(rildata::ApnTypes_t apnTypes);
int convertInstanceIdToSubId(qcril_instance_id_e_type instanceId);
bool isNullProfile(uint16_t profileId);
int getProfileId(wds_pdp_type_enum_v01, std::string);
void getApnNameProtocol (uint16_t profileId, std::string &apnName, qdp::IPType &protocol);
wds_pdp_type_enum_v01 convertToWdsPdpType(qdp::IPType &type);
#endif
