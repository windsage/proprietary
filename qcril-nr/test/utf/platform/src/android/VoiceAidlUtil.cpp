/*===========================================================================
 *
 *    Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "platform/android/VoiceAidlUtil.h"

#include "ril_utf_rild_sim.h"

void convertToRil(const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& in,
                  ril_unsol_emergency_list_t* out)
{
  out->ecc_list.emergency_list_length = in.size();
  for (int i = 0; i < in.size(); i++) {
    out->ecc_list.emergency_numbers[i].category = in[i].categories;
    out->ecc_list.emergency_numbers[i].sources = in[i].sources;
    strlcpy(out->ecc_list.emergency_numbers[i].number,
            in[i].number.c_str(),
            MAX_EMERGENCY_NUMBER_LENGTH);
    strlcpy(out->ecc_list.emergency_numbers[i].mcc, in[i].mcc.c_str(), MAX_RIL_MCC_MNC_SIZE);
    strlcpy(out->ecc_list.emergency_numbers[i].mnc, in[i].mnc.c_str(), MAX_RIL_MCC_MNC_SIZE);
    out->ecc_list.emergency_numbers[i].no_of_urns = in[i].urns.size();
    for (int j = 0; j < in[i].urns.size(); j++) {
      strlcpy(out->ecc_list.emergency_numbers[i].urn[j], in[i].urns[j].c_str(), MAX_URN_LENGTH);
    }
  }
}

int convertDialInfotoAidl(void* data,
                          size_t datalen,
                          ::aidl::android::hardware::radio::voice::Dial& dialInfo)
{
  auto payload = static_cast<RIL_Dial*>(data);
  dialInfo.address = payload->address;
  dialInfo.clir = payload->clir;

  if (payload->uusInfo) {
    dialInfo.uusInfo.resize(1);
    dialInfo.uusInfo[0].uusType = payload->uusInfo->uusType;
    dialInfo.uusInfo[0].uusDcs = payload->uusInfo->uusDcs;
    if (payload->uusInfo->uusData && payload->uusInfo->uusLength) {
      char *nullTermStr = strndup(payload->uusInfo->uusData, payload->uusInfo->uusLength);
      dialInfo.uusInfo[0].uusData = nullTermStr;
    }
  }
  return 0;
}

int convertCallForwardInfotoUtf(RIL_CallForwardInfo& out,
                                const ::aidl::android::hardware::radio::voice::CallForwardInfo& in)
{
  out.status = convertCallForwardInfoStatustoUtf(in.status);
  out.reason = in.reason;
  out.serviceClass = in.serviceClass;
  out.toa = in.toa;
  if (in.number.size()) {
    out.number = strdup(in.number.c_str());
  }
  out.timeSeconds = in.timeSeconds;
  return 0;
}

int convertCallForwardInfoStatustoUtf(int in)
{
  switch (in) {
    case ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_DISABLE:
      return 0;
    case ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_ENABLE:
      return 1;
    case ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_INTERROGATE:
      return 2;
    case ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_REGISTRATION:
      return 3;
    case ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_ERASURE:
      return 4;
    default:
      return 0;
  }
  return 0;
}

int convertCallForwardInfotoAidl(void* data,
                                 size_t datalen,
                                 ::aidl::android::hardware::radio::voice::CallForwardInfo& info)
{
  auto payload = static_cast<RIL_CallForwardInfo*>(data);
  info.status = convertCallForwardInfoStatustoAidl(payload->status);
  info.reason = payload->reason;
  info.serviceClass = payload->serviceClass;
  info.toa = payload->toa;
  if (payload->number) {
    info.number = payload->number;
  }
  info.timeSeconds = payload->timeSeconds;
  return 0;
}

int convertCallForwardInfoStatustoAidl(int in)
{
  switch (in) {
    case 0:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_DISABLE;
    case 1:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_ENABLE;
    case 2:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_INTERROGATE;
    case 3:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_REGISTRATION;
    case 4:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_ERASURE;
    default:
      return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_DISABLE;
  }
  return ::aidl::android::hardware::radio::voice::CallForwardInfo::STATUS_DISABLE;
}
