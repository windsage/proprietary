/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hidl_impl/1.2/radio_service_utils_1_2.h"
#include "hidl_impl/1.1/radio_service_utils_1_1.h"
#include "hidl_impl/1.0/radio_service_utils_1_0.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

bool checkThresholdAndHysteresis(int32_t hysteresisMs, int32_t hysteresisDb,
                                 const hidl_vec<int32_t>& thresholdsDbm) {
  QCRIL_LOG_FUNC_ENTRY();

  bool ret = true;
  // hysteresisDb must be smaller than the smallest threshold delta.
  int size = thresholdsDbm.size();
  int diff = INT_MAX;
  for (int i = 0; i < size-1; i++) {
    if (abs(thresholdsDbm[i + 1] - thresholdsDbm[i]) < diff) {
      diff = abs(thresholdsDbm[i + 1] - thresholdsDbm[i]);
    }
  }

  if (hysteresisMs < 0 && hysteresisDb < 0 && size == 0) {
    QCRIL_LOG_DEBUG("Invalid arguments");
    ret = false;
  }

  if (hysteresisDb == 0){
    QCRIL_LOG_DEBUG("HysteresisDb is 0 - disabling hysteresis");
  } else if (diff <= hysteresisDb) {
    QCRIL_LOG_DEBUG(
        "Invalid arguments passed, hysteresisDb: %d must be smaller than the smallest threshold "
        "delta: %d ",
        hysteresisDb, diff);
    ret = false;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
}

RIL_Errno sanityCheckSignalStrengthCriteriaParams(int32_t hysteresisMs, int32_t hysteresisDb,
                                                  const hidl_vec<int32_t>& thresholdsDbm,
                                                  V1_2::AccessNetwork ran) {
  (void)ran;
  QCRIL_LOG_DEBUG("performing sanity check");
  if (checkThresholdAndHysteresis(hysteresisMs, hysteresisDb, thresholdsDbm)) {
    return RIL_E_SUCCESS;
  } else {
    return RIL_E_INVALID_ARGUMENTS;
  }
}

RIL_RadioAccessNetworks convertHidlAccessNetworkToRilAccessNetwork(V1_2::AccessNetwork ran) {
  RIL_RadioAccessNetworks ret = RIL_RadioAccessNetworks::UNKNOWN;
  switch (ran) {
    case V1_2::AccessNetwork::GERAN:
      ret = RIL_RadioAccessNetworks::GERAN;
      break;
    case V1_2::AccessNetwork::UTRAN:
      ret = RIL_RadioAccessNetworks::UTRAN;
      break;
    case V1_2::AccessNetwork::EUTRAN:
      ret = RIL_RadioAccessNetworks::EUTRAN;
      break;
    case V1_2::AccessNetwork::CDMA2000:
      ret = RIL_RadioAccessNetworks::CDMA;
      break;
    case V1_2::AccessNetwork::IWLAN:
      ret = RIL_RadioAccessNetworks::IWLAN;
      break;
    default:
      ret = RIL_RadioAccessNetworks::UNKNOWN;
      break;
  }
  return ret;
}

void fillInSignalStrengthCriteria(std::vector<qcril::interfaces::SignalStrengthCriteriaEntry>& out,
                                  int32_t hysteresisMs, int32_t hysteresisDb,
                                  const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork ran) {
  RIL_RadioAccessNetworks ril_ran = convertHidlAccessNetworkToRilAccessNetwork(ran);

  /* for IRADIO 1_2, since there is no value specified in measure type, fix them as to current
   * code logic in order to make common code work for all IRADIO versions */
  RIL_SignalMeasureType type = MT_RSSI;
  if (ril_ran == EUTRAN || ril_ran == NGRAN) type = MT_RSRP;

  /*Timer support was added only for 1.5 HAL
    Setting this to an invalid value to ensure that it is not set in the req message
    sent to the modem */
  hysteresisMs = -1;

  // IRADIO 1_2 does not implement 'isEnabled', so setting it to true
  out.push_back({ ril_ran, type, hysteresisMs, hysteresisDb, thresholdsDbm, true });
}

RIL_Errno fillNetworkScanRequest_1_2(const V1_2::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest) {
  if (request.mccMncs.size() > MAX_MCC_MNC_LIST_SIZE) {
    return RIL_E_INVALID_ARGUMENTS;
  }

  auto res = fillNetworkScanRequest_1_1((const V1_1::NetworkScanRequest&)request, scanRequest);
  if (res != RIL_E_SUCCESS) {
    return res;
  }

  scanRequest.maxSearchTime = request.maxSearchTime;
  scanRequest.incrementalResults = request.incrementalResults;
  scanRequest.incrementalResultsPeriodicity = request.incrementalResultsPeriodicity;
  scanRequest.mccMncLength = request.mccMncs.size();

  for (size_t i = 0; i < request.mccMncs.size(); ++i) {
    strlcpy(scanRequest.mccMncs[i], request.mccMncs[i].c_str(), MAX_MCC_MNC_LEN + 1);
  }

  return RIL_E_SUCCESS;
}

int convertToHidl(V1_2::AudioQuality& aq, enum RIL_AudioQuality rilAudioQuality) {
  switch (rilAudioQuality) {
    case RIL_AUDIO_QUAL_UNSPECIFIED:
    default:
      aq = V1_2::AudioQuality::UNSPECIFIED;
      break;
    case RIL_AUDIO_QUAL_AMR:
      aq = V1_2::AudioQuality::AMR;
      break;
    case RIL_AUDIO_QUAL_AMR_WB:
      aq = V1_2::AudioQuality::AMR_WB;
      break;
    case RIL_AUDIO_QUAL_GSM_EFR:
      aq = V1_2::AudioQuality::GSM_EFR;
      break;
    case RIL_AUDIO_QUAL_GSM_FR:
      aq = V1_2::AudioQuality::GSM_FR;
      break;
    case RIL_AUDIO_QUAL_GSM_HR:
      aq = V1_2::AudioQuality::GSM_HR;
      break;
    case RIL_AUDIO_QUAL_EVRC:
      aq = V1_2::AudioQuality::EVRC;
      break;
    case RIL_AUDIO_QUAL_EVRC_B:
      aq = V1_2::AudioQuality::EVRC_B;
      break;
    case RIL_AUDIO_QUAL_EVRC_WB:
      aq = V1_2::AudioQuality::EVRC_WB;
      break;
    case RIL_AUDIO_QUAL_EVRC_NW:
      aq = V1_2::AudioQuality::EVRC_NW;
      break;
  }
  return 0;
}

int convertToHidl(V1_2::Call& out, const qcril::interfaces::CallInfo& in) {
  convertToHidl(out.base, in);
  if (in.hasAudioQuality()) {
    convertToHidl(out.audioQuality, in.getAudioQuality());
  }
  return 0;
}

int convertToHal(V1_2::WcdmaSignalStrength& out, const RIL_WCDMA_SignalStrength& in) {
  int ret = 1;
  ret = convertToHal(out.base, in);
  if (ret) return ret;
  out.rscp = in.rscp;
  out.ecno = in.ecio;
  ret = 0;
  return ret;
}

void convertToHal(V1_2::TdscdmaSignalStrength& out, const RIL_TD_SCDMA_SignalStrength& in) {
  out.rscp = in.rscp;
  out.signalStrength = INT_MAX;
  out.bitErrorRate = INT_MAX;
}

int convertRilSignalStrengthToHal(V1_2::SignalStrength& signalStrength,
                                  const RIL_SignalStrength& rilSignalStrength) {
  convertToHal(signalStrength.gsm, rilSignalStrength.GW_SignalStrength);
  convertToHal(signalStrength.cdma, rilSignalStrength.CDMA_SignalStrength);
  convertToHal(signalStrength.evdo, rilSignalStrength.EVDO_SignalStrength);
  convertToHal(signalStrength.lte, rilSignalStrength.LTE_SignalStrength);
  convertToHal(signalStrength.tdScdma, rilSignalStrength.TD_SCDMA_SignalStrength);
  convertToHal(signalStrength.wcdma, rilSignalStrength.WCDMA_SignalStrength);
  return 0;
}

void fillCellIdentityGsm(V1_2::CellIdentityGsm& out, const RIL_CellIdentityGsm_v12& in) {
  fillCellIdentityGsm(out.base, in);
  out.operatorNames.alphaLong = in.operatorNames.alphaLong;
  out.operatorNames.alphaShort = in.operatorNames.alphaShort;
}

void fillCellIdentityWcdma(V1_2::CellIdentityWcdma& out, const RIL_CellIdentityWcdma_v12& in) {
  fillCellIdentityWcdma(out.base, in);
  out.operatorNames.alphaLong = in.operatorNames.alphaLong;
  out.operatorNames.alphaShort = in.operatorNames.alphaShort;
}

void fillCellIdentityCdma(V1_2::CellIdentityCdma& out, const RIL_CellIdentityCdma& in) {
  fillCellIdentityCdma(out.base, in);
  out.operatorNames.alphaLong = in.operatorNames.alphaLong;
  out.operatorNames.alphaShort = in.operatorNames.alphaShort;
}

void fillCellIdentityLte(V1_2::CellIdentityLte& out, const RIL_CellIdentityLte_v12& in) {
  fillCellIdentityLte(out.base, in);
  out.operatorNames.alphaLong = in.operatorNames.alphaLong;
  out.operatorNames.alphaShort = in.operatorNames.alphaShort;
  out.bandwidth = in.bandwidth;
}

void fillCellIdentityTdscdma(V1_2::CellIdentityTdscdma& out, const RIL_CellIdentityTdscdma& in) {
  fillCellIdentityTdscdma(out.base, in);
  out.operatorNames.alphaLong = in.operatorNames.alphaLong;
  out.operatorNames.alphaShort = in.operatorNames.alphaShort;
  out.uarfcn = INT_MAX;
}

void fillCellIdentityResponse(V1_2::CellIdentity& cellIdentity,
                              const RIL_CellIdentity_v16& rilCellIdentity) {
  cellIdentity.cellIdentityGsm.resize(0);
  cellIdentity.cellIdentityWcdma.resize(0);
  cellIdentity.cellIdentityCdma.resize(0);
  cellIdentity.cellIdentityTdscdma.resize(0);
  cellIdentity.cellIdentityLte.resize(0);
  cellIdentity.cellInfoType = static_cast<V1_0::CellInfoType>(rilCellIdentity.cellInfoType);
  switch (rilCellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      cellIdentity.cellIdentityGsm.resize(1);
      fillCellIdentityGsm(cellIdentity.cellIdentityGsm[0], rilCellIdentity.cellIdentityGsm);
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      cellIdentity.cellIdentityWcdma.resize(1);
      fillCellIdentityWcdma(cellIdentity.cellIdentityWcdma[0], rilCellIdentity.cellIdentityWcdma);
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      cellIdentity.cellIdentityCdma.resize(1);
      fillCellIdentityCdma(cellIdentity.cellIdentityCdma[0], rilCellIdentity.cellIdentityCdma);
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      cellIdentity.cellIdentityLte.resize(1);
      fillCellIdentityLte(cellIdentity.cellIdentityLte[0], rilCellIdentity.cellIdentityLte);
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      cellIdentity.cellIdentityTdscdma.resize(1);
      fillCellIdentityTdscdma(cellIdentity.cellIdentityTdscdma[0],
                              rilCellIdentity.cellIdentityTdscdma);
      break;
    }

    default: {
      break;
    }
  }
}

int fillVoiceRegistrationStateResponse(V1_2::VoiceRegStateResult& voiceRegResponse,
                                       const RIL_VoiceRegistrationStateResponse& voiceRegState) {
  voiceRegResponse.regState = static_cast<V1_0::RegState>(voiceRegState.regState);
  voiceRegResponse.rat = voiceRegState.rat;
  voiceRegResponse.cssSupported = voiceRegState.cssSupported;
  voiceRegResponse.roamingIndicator = voiceRegState.roamingIndicator;
  voiceRegResponse.systemIsInPrl = voiceRegState.systemIsInPrl;
  voiceRegResponse.defaultRoamingIndicator = voiceRegState.defaultRoamingIndicator;
  voiceRegResponse.reasonForDenial = voiceRegState.reasonForDenial;
  fillCellIdentityResponse(voiceRegResponse.cellIdentity, voiceRegState.cellIdentity);
  return 0;
}

void fillDataRegistrationStateResponse(V1_2::DataRegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in) {
  out.regState = static_cast<V1_0::RegState>(in.regState);
  out.rat = in.rat;
  out.reasonDataDenied = in.reasonDataDenied;
  out.maxDataCalls = in.maxDataCalls;
  fillCellIdentityResponse(out.cellIdentity, in.cellIdentity);
}

void convertRilCellInfoToHal_1_2(V1_2::CellInfo& hidlCellInfo, const RIL_CellInfo_v12& rilCellInfo) {
  switch (rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      hidlCellInfo.gsm.resize(1);
      V1_2::CellInfoGsm* cellInfoGsm = &hidlCellInfo.gsm[0];

      new (&cellInfoGsm->cellIdentityGsm.base.mcc)
          hidl_string(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc,
                      strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc,
                              sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc) - 1));

      new (&cellInfoGsm->cellIdentityGsm.base.mnc)
          hidl_string(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc,
                      strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc,
                              sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc) - 1));

      cellInfoGsm->cellIdentityGsm.base.lac = rilCellInfo.CellInfo.gsm.cellIdentityGsm.lac;
      cellInfoGsm->cellIdentityGsm.base.cid = rilCellInfo.CellInfo.gsm.cellIdentityGsm.cid;
      cellInfoGsm->cellIdentityGsm.base.arfcn = rilCellInfo.CellInfo.gsm.cellIdentityGsm.arfcn;
      cellInfoGsm->cellIdentityGsm.base.bsic = rilCellInfo.CellInfo.gsm.cellIdentityGsm.bsic;
      cellInfoGsm->signalStrengthGsm.signalStrength =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.signalStrength;
      cellInfoGsm->signalStrengthGsm.bitErrorRate =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.bitErrorRate;
      cellInfoGsm->signalStrengthGsm.timingAdvance =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.timingAdvance;

      new (&cellInfoGsm->cellIdentityGsm.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong) - 1));

      new (&cellInfoGsm->cellIdentityGsm.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort) - 1));
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      hidlCellInfo.wcdma.resize(1);
      V1_2::CellInfoWcdma* cellInfoWcdma = &hidlCellInfo.wcdma[0];

      new (&cellInfoWcdma->cellIdentityWcdma.base.mcc)
          hidl_string(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc,
                      strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc,
                              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc) - 1));

      new (&cellInfoWcdma->cellIdentityWcdma.base.mnc)
          hidl_string(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc,
                      strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc,
                              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc) - 1));

      cellInfoWcdma->cellIdentityWcdma.base.lac = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.lac;
      cellInfoWcdma->cellIdentityWcdma.base.cid = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.cid;
      cellInfoWcdma->cellIdentityWcdma.base.psc = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.psc;
      cellInfoWcdma->cellIdentityWcdma.base.uarfcn =
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.uarfcn;
      cellInfoWcdma->signalStrengthWcdma.base.signalStrength =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.signalStrength;
      cellInfoWcdma->signalStrengthWcdma.base.bitErrorRate =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;

      new (&cellInfoWcdma->cellIdentityWcdma.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong) - 1));

      new (&cellInfoWcdma->cellIdentityWcdma.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort,
          strnlen(
              rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort,
              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort) - 1));
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      hidlCellInfo.cdma.resize(1);
      V1_2::CellInfoCdma* cellInfoCdma = &hidlCellInfo.cdma[0];
      cellInfoCdma->cellIdentityCdma.base.networkId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.networkId;
      cellInfoCdma->cellIdentityCdma.base.systemId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.systemId;
      cellInfoCdma->cellIdentityCdma.base.baseStationId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.basestationId;
      cellInfoCdma->cellIdentityCdma.base.longitude =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.longitude;
      cellInfoCdma->cellIdentityCdma.base.latitude =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.latitude;
      cellInfoCdma->signalStrengthCdma.dbm = rilCellInfo.CellInfo.cdma.signalStrengthCdma.dbm;
      cellInfoCdma->signalStrengthCdma.ecio = rilCellInfo.CellInfo.cdma.signalStrengthCdma.ecio;
      cellInfoCdma->signalStrengthEvdo.dbm = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.dbm;
      cellInfoCdma->signalStrengthEvdo.ecio = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.ecio;
      cellInfoCdma->signalStrengthEvdo.signalNoiseRatio =
          rilCellInfo.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;

      new (&cellInfoCdma->cellIdentityCdma.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong) - 1));

      new (&cellInfoCdma->cellIdentityCdma.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort) - 1));
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      hidlCellInfo.lte.resize(1);
      V1_2::CellInfoLte* cellInfoLte = &hidlCellInfo.lte[0];

      new (&cellInfoLte->cellIdentityLte.base.mcc)
          hidl_string(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc,
                      strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc,
                              sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc) - 1));

      new (&cellInfoLte->cellIdentityLte.base.mnc)
          hidl_string(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc,
                      strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc,
                              sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc) - 1));

      cellInfoLte->cellIdentityLte.base.ci = rilCellInfo.CellInfo.lte.cellIdentityLte.ci;
      cellInfoLte->cellIdentityLte.base.pci = rilCellInfo.CellInfo.lte.cellIdentityLte.pci;
      cellInfoLte->cellIdentityLte.base.tac = rilCellInfo.CellInfo.lte.cellIdentityLte.tac;
      cellInfoLte->cellIdentityLte.base.earfcn = rilCellInfo.CellInfo.lte.cellIdentityLte.earfcn;
      cellInfoLte->signalStrengthLte.signalStrength =
          rilCellInfo.CellInfo.lte.signalStrengthLte.signalStrength;
      cellInfoLte->signalStrengthLte.rsrp = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrp;
      cellInfoLte->signalStrengthLte.rsrq = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrq;
      cellInfoLte->signalStrengthLte.rssnr = rilCellInfo.CellInfo.lte.signalStrengthLte.rssnr;
      cellInfoLte->signalStrengthLte.cqi = rilCellInfo.CellInfo.lte.signalStrengthLte.cqi;
      cellInfoLte->signalStrengthLte.timingAdvance =
          rilCellInfo.CellInfo.lte.signalStrengthLte.timingAdvance;

      new (&cellInfoLte->cellIdentityLte.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong) - 1));

      new (&cellInfoLte->cellIdentityLte.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort) - 1));

      cellInfoLte->cellIdentityLte.bandwidth = rilCellInfo.CellInfo.lte.cellIdentityLte.bandwidth;
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      hidlCellInfo.tdscdma.resize(1);
      V1_2::CellInfoTdscdma* cellInfoTdscdma = &hidlCellInfo.tdscdma[0];

      new (&cellInfoTdscdma->cellIdentityTdscdma.base.mcc)
          hidl_string(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                      strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                              sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc) - 1));

      new (&cellInfoTdscdma->cellIdentityTdscdma.base.mnc)
          hidl_string(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                      strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                              sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc) - 1));

      cellInfoTdscdma->cellIdentityTdscdma.base.lac =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.lac;
      cellInfoTdscdma->cellIdentityTdscdma.base.cid =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.cid;
      cellInfoTdscdma->cellIdentityTdscdma.base.cpid =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.cpid;
      cellInfoTdscdma->signalStrengthTdscdma.rscp =
          rilCellInfo.CellInfo.tdscdma.signalStrengthTdscdma.rscp;
      break;
    }

    default: {
      break;
    }
  }
}

void convertRilCellInfoListToHal_1_2(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                     hidl_vec<V1_2::CellInfo>& records) {
  auto num = rillCellInfo.size();
  records.resize(num);

  for (unsigned int i = 0; i < num; i++) {
    records[i].cellInfoType = static_cast<V1_0::CellInfoType>(rillCellInfo[i].cellInfoType);
    records[i].registered = rillCellInfo[i].registered;
    records[i].timeStampType = static_cast<V1_0::TimeStampType>(rillCellInfo[i].timeStampType);
    records[i].timeStamp = rillCellInfo[i].timeStamp;
    records[i].connectionStatus =
        static_cast<V1_2::CellConnectionStatus>(rillCellInfo[i].connStatus);
    // All vectors should be size 0 except one which will be size 1. Set everything to
    // size 0 initially.
    records[i].gsm.resize(0);
    records[i].wcdma.resize(0);
    records[i].cdma.resize(0);
    records[i].lte.resize(0);
    records[i].tdscdma.resize(0);

    convertRilCellInfoToHal_1_2(records[i], rillCellInfo[i]);
  }
}

RIL_Errno convertLcResultToRilError(rildata::LinkCapCriteriaResult_t result) {
    RIL_Errno ret = RIL_Errno::RIL_E_SUCCESS;

    switch(result) {
        case rildata::radio_not_available:
            ret = RIL_Errno::RIL_E_RADIO_NOT_AVAILABLE;
            break;
        case rildata::request_not_supported:
            ret = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED;
            break;
        case rildata::internal_err:
            ret = RIL_Errno::RIL_E_INTERNAL_ERR;
            break;
        case rildata::invalid_arguments:
            ret = RIL_Errno::RIL_E_INVALID_ARGUMENTS;
            break;
        case rildata::success:
        default:
            ret = RIL_Errno::RIL_E_SUCCESS;
            break;
    }

    return ret;
}

rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork(V1_2::AccessNetwork ran)
{
    rildata::AccessNetwork_t ret;
    switch(ran) {
        case V1_2::AccessNetwork::GERAN:
            ret = rildata::AccessNetwork_t::GERAN;
            break;
        case V1_2::AccessNetwork::UTRAN:
            ret = rildata::AccessNetwork_t::UTRAN;
            break;
        case V1_2::AccessNetwork::EUTRAN:
            ret = rildata::AccessNetwork_t::EUTRAN;
            break;
        case V1_2::AccessNetwork::CDMA2000:
            ret = rildata::AccessNetwork_t::CDMA;
            break;
        case V1_2::AccessNetwork::IWLAN:
            ret = rildata::AccessNetwork_t::IWLAN;
            break;
        default:
            ret = rildata::AccessNetwork_t::UNKNOWN;
            break;
    }
    return ret;
}

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
