/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
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

#include "hidl_impl/1.5/radio_service_utils_1_5.h"
#include "hidl_impl/1.4/radio_service_utils_1_4.h"
#include "hidl_impl/1.2/radio_service_utils_1_2.h"
#include "hidl_impl/1.0/radio_service_utils_1_0.h"
#include "RadioServiceModule.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

RIL_RadioAccessNetworks convertHidlRadioAccessNetworkToRilRadioAccessNetwork(
    V1_5::RadioAccessNetworks ran) {
  switch (ran) {
    case V1_5::RadioAccessNetworks::GERAN:
      return RIL_RadioAccessNetworks::GERAN;
    case V1_5::RadioAccessNetworks::UTRAN:
      return RIL_RadioAccessNetworks::UTRAN;
    case V1_5::RadioAccessNetworks::EUTRAN:
      return RIL_RadioAccessNetworks::EUTRAN;
    case V1_5::RadioAccessNetworks::NGRAN:
      return RIL_RadioAccessNetworks::NGRAN;
    case V1_5::RadioAccessNetworks::CDMA2000:
      return RIL_RadioAccessNetworks::CDMA;
    default:
      return RIL_RadioAccessNetworks::UNKNOWN;
  }
}

V1_5::RadioAccessNetworks convertRilRadioAccessNetworkToHidlRadioAccessNetwork(
    RIL_RadioAccessNetworks ran) {
  switch (ran) {
    case RIL_RadioAccessNetworks::GERAN:
      return V1_5::RadioAccessNetworks::GERAN;
    case RIL_RadioAccessNetworks::UTRAN:
      return V1_5::RadioAccessNetworks::UTRAN;
    case  RIL_RadioAccessNetworks::EUTRAN:
      return V1_5::RadioAccessNetworks::EUTRAN;
    case RIL_RadioAccessNetworks::NGRAN:
      return V1_5::RadioAccessNetworks::NGRAN;
    case RIL_RadioAccessNetworks::CDMA:
      return V1_5::RadioAccessNetworks::CDMA2000;
    default:
      return V1_5::RadioAccessNetworks::UNKNOWN;
  }
}

RIL_Errno sanityCheck(const hidl_vec<V1_5::RadioAccessSpecifier>& ras) {
  // sanity check

  for (size_t i = 0; i < ras.size(); ++i) {
    if (ras[i].channels.size() > MAX_CHANNELS) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    // bands is safe union, make sure to check descriminator before accessing
    // any safe union fields
    if ((ras[i].bands.getDiscriminator() ==
         V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::geranBands) &&
        (ras[i].bands.geranBands().size() > MAX_BANDS)) {
      return RIL_E_INVALID_ARGUMENTS;
    } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::utranBands) &&
               (ras[i].bands.utranBands().size() > MAX_BANDS)) {
      return RIL_E_INVALID_ARGUMENTS;
    } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::eutranBands) &&
               (ras[i].bands.eutranBands().size() > MAX_BANDS)) {
      return RIL_E_INVALID_ARGUMENTS;
    } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::ngranBands) &&
               (ras[i].bands.ngranBands().size() > MAX_BANDS)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
  }

  return RIL_E_SUCCESS;
}

RIL_Errno convertRilRasToHidlRas(V1_5::RadioAccessSpecifier& ras_to,
                                 const RIL_RadioAccessSpecifier& ras_from) {
  ras_to.radioAccessNetwork =
      convertRilRadioAccessNetworkToHidlRadioAccessNetwork(ras_from.radio_access_network);
  ras_to.channels.resize(ras_from.channels_length);
  hidl_vec<V1_1::GeranBands> gbands;
  hidl_vec<V1_5::UtranBands> ubands;
  hidl_vec<V1_5::EutranBands> eubands;
  hidl_vec<V1_5::NgranBands> ngbands;
  switch (ras_from.radio_access_network) {
    case RIL_RadioAccessNetworks::GERAN:
      gbands.resize(ras_from.bands_length);
      for (size_t idx = 0; idx < ras_from.bands_length; ++idx) {
        gbands[idx] =
          static_cast<V1_1::GeranBands>(ras_from.bands.geran_bands[idx]);
      }
      ras_to.bands.geranBands(std::move(gbands));
      break;
    case RIL_RadioAccessNetworks::UTRAN:
      ubands.resize(ras_from.bands_length);
      for (size_t idx = 0; idx < ras_from.bands_length; ++idx) {
        ubands[idx] =
          static_cast<V1_5::UtranBands>(ras_from.bands.utran_bands[idx]);
      }
      ras_to.bands.utranBands(std::move(ubands));
      break;
    case RIL_RadioAccessNetworks::EUTRAN:
      eubands.resize(ras_from.bands_length);
      for (size_t idx = 0; idx < ras_from.bands_length; ++idx) {
        eubands[idx] =
          static_cast<V1_5::EutranBands>(ras_from.bands.eutran_bands[idx]);
      }
      ras_to.bands.eutranBands(std::move(eubands));
      break;
    case RIL_RadioAccessNetworks::NGRAN:
      ngbands.resize(ras_from.bands_length);
      for (size_t idx = 0; idx < ras_from.bands_length; ++idx) {
        ngbands[idx] =
          static_cast<V1_5::NgranBands>(ras_from.bands.eutran_bands[idx]);
      }
      ras_to.bands.ngranBands(std::move(ngbands));
      break;
    default:
      return RIL_E_INVALID_ARGUMENTS;
  }

  return RIL_E_SUCCESS;
}

RIL_Errno convertToHidl(hidl_vec<V1_5::RadioAccessSpecifier>& out,
                        const RIL_SysSelChannels& in) {
  RIL_Errno ret = RIL_E_SUCCESS;
  out.resize(in.specifiers_length);
  for (size_t i = 0; i < out.size(); ++i) {
    ret = convertRilRasToHidlRas(out[i], in.specifiers[i]);
    if (ret != RIL_E_SUCCESS) {
      QCRIL_LOG_ERROR("failed to convert ril fields to hidl fields - %d", ret);
    }
  }
  return ret;
}

RIL_Errno convertHidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
                                 const V1_5::RadioAccessSpecifier& ras_from) {
  ras_to.radio_access_network =
      convertHidlRadioAccessNetworkToRilRadioAccessNetwork(ras_from.radioAccessNetwork);
  ras_to.channels_length = ras_from.channels.size();

  std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
  switch (ras_from.radioAccessNetwork) {
    case V1_5::RadioAccessNetworks::GERAN:
      if (ras_from.bands.getDiscriminator() ==
          V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::geranBands) {
        ras_to.bands_length = ras_from.bands.geranBands().size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.geran_bands[idx] =
              static_cast<RIL_GeranBands>(ras_from.bands.geranBands()[idx]);
        }
      }
      break;
    case V1_5::RadioAccessNetworks::UTRAN:
      if (ras_from.bands.getDiscriminator() ==
          V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::utranBands) {
        ras_to.bands_length = ras_from.bands.utranBands().size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.utran_bands[idx] =
              static_cast<RIL_UtranBands>(ras_from.bands.utranBands()[idx]);
        }
      }
      break;
    case V1_5::RadioAccessNetworks::EUTRAN:
      if (ras_from.bands.getDiscriminator() ==
          V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::eutranBands) {
        ras_to.bands_length = ras_from.bands.eutranBands().size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.eutran_bands[idx] =
              static_cast<RIL_EutranBands>(ras_from.bands.eutranBands()[idx]);
        }
      }
      break;
    case V1_5::RadioAccessNetworks::NGRAN:
      if (ras_from.bands.getDiscriminator() ==
          V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::ngranBands) {
        ras_to.bands_length = ras_from.bands.ngranBands().size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.ngran_bands[idx] =
              static_cast<RIL_NgranBands>(ras_from.bands.ngranBands()[idx]);
        }
      }
      break;
    default:
      return RIL_E_INVALID_ARGUMENTS;
  }

  return RIL_E_SUCCESS;
}

RIL_Errno fillSetSystemSelectionChannelRequest_1_5(
    const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers, RIL_SysSelChannels& request) {
  RIL_Errno error = sanityCheck(specifiers);
  if (error != RIL_E_SUCCESS) {
    QCRIL_LOG_ERROR("sanity check failed - %d", error);
    return error;
  }

  request.specifiers_length = specifiers.size();
  if (request.specifiers_length <= MAX_RADIO_ACCESS_SPECIFIERS) {
    request.specifiers_latest = new RIL_RadioAccessSpecifier[request.specifiers_length];
  } else {
    return RIL_E_INVALID_ARGUMENTS;
  }
  if (request.specifiers_latest) {
    memset(request.specifiers_latest, 0, request.specifiers_length*sizeof(RIL_RadioAccessSpecifier));
    for (size_t i = 0; i < request.specifiers_length; i++) {
      error = convertHidlRasToRilRas(request.specifiers_latest[i], specifiers[i]);
      if (error != RIL_E_SUCCESS) {
        QCRIL_LOG_ERROR("failed to convert hidl fields to ril fields - %d", error);
        return error;
      }
    }
  } else {
    return RIL_E_NO_MEMORY;
  }
  return RIL_E_SUCCESS;
}

RIL_Errno fillNetworkScanRequest_1_5(const V1_5::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest) {
  if (request.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
    QCRIL_LOG_ERROR("MAX RANs exceeded");
    return RIL_E_INVALID_ARGUMENTS;
  }

  if (request.mccMncs.size() > MAX_MCC_MNC_LIST_SIZE) {
     QCRIL_LOG_ERROR("MAX MccMncs exceeded");
     return RIL_E_INVALID_ARGUMENTS;
  }
  RIL_Errno error = sanityCheck(request.specifiers);
  if (error != RIL_E_SUCCESS) {
    QCRIL_LOG_ERROR("sanity check failed - %d", error);
    return error;
  }

  if (request.type == V1_1::ScanType::ONE_SHOT) {
    scanRequest.type = RIL_ONE_SHOT;
  } else if (request.type == V1_1::ScanType::PERIODIC) {
    scanRequest.type = RIL_PERIODIC;
  }
  scanRequest.interval = request.interval;
  scanRequest.maxSearchTime = request.maxSearchTime;
  scanRequest.incrementalResults = request.incrementalResults;
  scanRequest.incrementalResultsPeriodicity = request.incrementalResultsPeriodicity;
  scanRequest.mccMncLength = request.mccMncs.size();
  scanRequest.specifiers_length = request.specifiers.size();

  for (size_t i = 0; i < request.specifiers.size(); ++i) {
    error = convertHidlRasToRilRas(scanRequest.specifiers[i], request.specifiers[i]);
    if (error != RIL_E_SUCCESS) {
      QCRIL_LOG_ERROR("failed to convert hidl fields to ril fields - %d", error);
      return error;
    }
  }

  for (size_t i = 0; i < request.mccMncs.size(); ++i) {
    strlcpy(scanRequest.mccMncs[i], request.mccMncs[i].c_str(), MAX_MCC_MNC_LEN + 1);
  }

  return RIL_E_SUCCESS;
}

RIL_Errno sanityCheckSignalStrengthCriteriaParams(const V1_5::SignalThresholdInfo& threshold,
                                                  V1_5::AccessNetwork ran) {
  (void)ran;
  QCRIL_LOG_DEBUG("performing sanity check");
  if (checkThresholdAndHysteresis(threshold.hysteresisMs, threshold.hysteresisDb,
                                  threshold.thresholds)) {
    return RIL_E_SUCCESS;
  } else {
    return RIL_E_INVALID_ARGUMENTS;
  }
}

RIL_RadioAccessNetworks convertHidlAccessNetworkToRilAccessNetwork(V1_5::AccessNetwork ran) {
  if (ran == V1_5::AccessNetwork::NGRAN) {
    return RIL_RadioAccessNetworks::NGRAN;
  } else {
    return convertHidlAccessNetworkToRilAccessNetwork(static_cast<V1_2::AccessNetwork>(ran));
  }
}

RIL_SignalMeasureType convertHidlMeasureTypetoRilMeasureType(V1_5::SignalMeasurementType type) {
  RIL_SignalMeasureType ret = MT_UNKNOWN;
  switch (type) {
    case V1_5::SignalMeasurementType::RSSI:
      ret = MT_RSSI;
      break;
    case V1_5::SignalMeasurementType::RSCP:
      ret = MT_RSCP;
      break;
    case V1_5::SignalMeasurementType::RSRP:
      ret = MT_RSRP;
      break;
    case V1_5::SignalMeasurementType::RSRQ:
      ret = MT_RSRQ;
      break;
    case V1_5::SignalMeasurementType::RSSNR:
      ret = MT_RSSNR;
      break;
    case V1_5::SignalMeasurementType::SSRSRP:
      ret = MT_SSRSRP;
      break;
    case V1_5::SignalMeasurementType::SSRSRQ:
      ret = MT_SSRSRQ;
      break;
    case V1_5::SignalMeasurementType::SSSINR:
      ret = MT_SSSINR;
      break;
    default:
      ret = MT_UNKNOWN;
  }
  return ret;
}

void fillInSignalStrengthCriteria(std::vector<qcril::interfaces::SignalStrengthCriteriaEntry>& out,
                                  const V1_5::SignalThresholdInfo& signalThresholdInfo,
                                  V1_5::AccessNetwork ran, bool& disableAllTypes) {
  RadioServiceModule& rilModule = getRadioServiceModule();
  RIL_RadioAccessNetworks ril_ran = convertHidlAccessNetworkToRilAccessNetwork(ran);
  auto type = convertHidlMeasureTypetoRilMeasureType(signalThresholdInfo.signalMeasurement);

  switch (ril_ran) {
    case EUTRAN:
      if ((signalThresholdInfo.isEnabled) &&
          ((type == MT_RSRP) || (type == MT_RSRQ) || (type == MT_RSSNR))) {
        rilModule.incLteReqCnt();
      } else if ((!signalThresholdInfo.isEnabled) &&
                 ((type == MT_RSRP) || (type == MT_RSRQ) || (type == MT_RSSNR))) {
        rilModule.incLteReqCnt();
        rilModule.incLteDisCnt();
        disableAllTypes = rilModule.checkLteDisCnt();
      } else {
        QCRIL_LOG_DEBUG(" Unexpected signal type received for EUTRAN");
      }
      rilModule.checkLteReqCnt();
      break;
    case NGRAN:
      if ((signalThresholdInfo.isEnabled) &&
          ((type == MT_SSRSRP) || (type == MT_SSRSRQ) || (type == MT_SSSINR))) {
        rilModule.incNr5gReqCnt();
      } else if ((!signalThresholdInfo.isEnabled) &&
                 ((type == MT_SSRSRP) || (type == MT_SSRSRQ) || (type == MT_SSSINR))) {
        rilModule.incNr5gReqCnt();
        rilModule.incNr5gDisCnt();
        disableAllTypes = rilModule.checkNr5gDisCnt();
      } else {
        QCRIL_LOG_DEBUG(" Unexpected signal type received for NGRAN");
      }
      rilModule.checkNr5gReqCnt();
      break;
    default:
      break;
  }

  out.push_back({ ril_ran, type, signalThresholdInfo.hysteresisMs, signalThresholdInfo.hysteresisDb,
                  signalThresholdInfo.thresholds, signalThresholdInfo.isEnabled });
}

template <typename T>
bool getHidlPlmnFromCellIdentity(hidl_string& mcc, hidl_string& mnc, const T& cellIdentity) {
  if (cellIdentity.primary_plmn_mcc[0] != '\0' && cellIdentity.primary_plmn_mnc[0] != '\0') {
    mcc = hidl_string(
        cellIdentity.primary_plmn_mcc,
        strnlen(cellIdentity.primary_plmn_mcc, sizeof(cellIdentity.primary_plmn_mcc) - 1));
    mnc = hidl_string(
        cellIdentity.primary_plmn_mnc,
        strnlen(cellIdentity.primary_plmn_mnc, sizeof(cellIdentity.primary_plmn_mnc) - 1));
    return true;

  } else if (cellIdentity.mcc[0] != '\0' && cellIdentity.mnc[0] != '\0') {
    mcc = hidl_string(cellIdentity.mcc, strnlen(cellIdentity.mcc, sizeof(cellIdentity.mcc) - 1));
    mnc = hidl_string(cellIdentity.mnc, strnlen(cellIdentity.mnc, sizeof(cellIdentity.mnc) - 1));
    return true;
  }
  return false;
}

void convertGsmCellIdentityRilToHidl(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
                                     V1_5::CellIdentityGsm& hidlGsmCellIdentity) {
  (void)getHidlPlmnFromCellIdentity(hidlGsmCellIdentity.base.base.mcc,
                                    hidlGsmCellIdentity.base.base.mnc, rilGsmCellIdentity);

  hidlGsmCellIdentity.base.base.lac = rilGsmCellIdentity.lac;
  hidlGsmCellIdentity.base.base.cid = rilGsmCellIdentity.cid;
  hidlGsmCellIdentity.base.base.arfcn = rilGsmCellIdentity.arfcn;
  hidlGsmCellIdentity.base.base.bsic = rilGsmCellIdentity.bsic;

  hidlGsmCellIdentity.base.operatorNames.alphaLong =
      hidl_string(rilGsmCellIdentity.operatorNames.alphaLong,
                  strnlen(rilGsmCellIdentity.operatorNames.alphaLong,
                          sizeof(rilGsmCellIdentity.operatorNames.alphaLong) - 1));

  hidlGsmCellIdentity.base.operatorNames.alphaShort =
      hidl_string(rilGsmCellIdentity.operatorNames.alphaShort,
                  strnlen(rilGsmCellIdentity.operatorNames.alphaShort,
                          sizeof(rilGsmCellIdentity.operatorNames.alphaShort) - 1));
}

void convertWcdmaCellIdentityRilToHidl(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
                                       V1_5::CellIdentityWcdma& hidlWcdmaCellIdentity) {
  (void)getHidlPlmnFromCellIdentity(hidlWcdmaCellIdentity.base.base.mcc,
                                    hidlWcdmaCellIdentity.base.base.mnc, rilWcdmaCellIdentity);

  hidlWcdmaCellIdentity.base.base.lac = rilWcdmaCellIdentity.lac;
  hidlWcdmaCellIdentity.base.base.cid = rilWcdmaCellIdentity.cid;
  hidlWcdmaCellIdentity.base.base.psc = rilWcdmaCellIdentity.psc;
  hidlWcdmaCellIdentity.base.base.uarfcn = rilWcdmaCellIdentity.uarfcn;

  hidlWcdmaCellIdentity.base.operatorNames.alphaLong =
      hidl_string(rilWcdmaCellIdentity.operatorNames.alphaLong,
                  strnlen(rilWcdmaCellIdentity.operatorNames.alphaLong,
                          sizeof(rilWcdmaCellIdentity.operatorNames.alphaLong) - 1));

  hidlWcdmaCellIdentity.base.operatorNames.alphaShort =
      hidl_string(rilWcdmaCellIdentity.operatorNames.alphaShort,
                  strnlen(rilWcdmaCellIdentity.operatorNames.alphaShort,
                          sizeof(rilWcdmaCellIdentity.operatorNames.alphaShort) - 1));
}

void convertTdscdmaCellIdentityRilToHidl(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
                                         V1_5::CellIdentityTdscdma& hidlTdscdmaCellIdentity) {
  (void)getHidlPlmnFromCellIdentity(hidlTdscdmaCellIdentity.base.base.mcc,
                                    hidlTdscdmaCellIdentity.base.base.mnc, rilTdscdmaCellIdentity);

  hidlTdscdmaCellIdentity.base.base.lac = rilTdscdmaCellIdentity.lac;
  hidlTdscdmaCellIdentity.base.base.cid = rilTdscdmaCellIdentity.cid;
  hidlTdscdmaCellIdentity.base.base.cpid = rilTdscdmaCellIdentity.cpid;

  hidlTdscdmaCellIdentity.base.operatorNames.alphaLong =
      hidl_string(rilTdscdmaCellIdentity.operatorNames.alphaLong,
                  strnlen(rilTdscdmaCellIdentity.operatorNames.alphaLong,
                          sizeof(rilTdscdmaCellIdentity.operatorNames.alphaLong) - 1));

  hidlTdscdmaCellIdentity.base.operatorNames.alphaShort =
      hidl_string(rilTdscdmaCellIdentity.operatorNames.alphaShort,
                  strnlen(rilTdscdmaCellIdentity.operatorNames.alphaShort,
                          sizeof(rilTdscdmaCellIdentity.operatorNames.alphaShort) - 1));

  hidlTdscdmaCellIdentity.base.uarfcn = 0;  // default value
}

void convertCdmaCellIdentityRilToHidl(const RIL_CellIdentityCdma& rilCdmaCellIdentity,
                                      V1_2::CellIdentityCdma& hidlCdmaCellIdentity) {
  hidlCdmaCellIdentity.base.networkId = rilCdmaCellIdentity.networkId;
  hidlCdmaCellIdentity.base.systemId = rilCdmaCellIdentity.systemId;
  hidlCdmaCellIdentity.base.baseStationId = rilCdmaCellIdentity.basestationId;
  hidlCdmaCellIdentity.base.longitude = rilCdmaCellIdentity.longitude;
  hidlCdmaCellIdentity.base.latitude = rilCdmaCellIdentity.latitude;

  hidlCdmaCellIdentity.operatorNames.alphaLong =
      hidl_string(rilCdmaCellIdentity.operatorNames.alphaLong,
                  strnlen(rilCdmaCellIdentity.operatorNames.alphaLong,
                          sizeof(rilCdmaCellIdentity.operatorNames.alphaLong) - 1));

  hidlCdmaCellIdentity.operatorNames.alphaShort =
      hidl_string(rilCdmaCellIdentity.operatorNames.alphaShort,
                  strnlen(rilCdmaCellIdentity.operatorNames.alphaShort,
                          sizeof(rilCdmaCellIdentity.operatorNames.alphaShort) - 1));
}

void convertLteCellIdentityRilToHidl(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
                                     V1_5::CellIdentityLte& hidlLteCellIdentity) {
  (void)getHidlPlmnFromCellIdentity(hidlLteCellIdentity.base.base.mcc,
                                    hidlLteCellIdentity.base.base.mnc, rilLteCellIdentity);

  hidlLteCellIdentity.base.base.ci = rilLteCellIdentity.ci;
  hidlLteCellIdentity.base.base.pci = rilLteCellIdentity.pci;
  hidlLteCellIdentity.base.base.tac = rilLteCellIdentity.tac;
  hidlLteCellIdentity.base.base.earfcn = rilLteCellIdentity.earfcn;

  hidlLteCellIdentity.base.operatorNames.alphaLong =
      hidl_string(rilLteCellIdentity.operatorNames.alphaLong,
                  strnlen(rilLteCellIdentity.operatorNames.alphaLong,
                          sizeof(rilLteCellIdentity.operatorNames.alphaLong) - 1));

  hidlLteCellIdentity.base.operatorNames.alphaShort =
      hidl_string(rilLteCellIdentity.operatorNames.alphaShort,
                  strnlen(rilLteCellIdentity.operatorNames.alphaShort,
                          sizeof(rilLteCellIdentity.operatorNames.alphaShort) - 1));

  hidlLteCellIdentity.base.bandwidth = rilLteCellIdentity.bandwidth;

  // Fill band only if its valid
  if (rilLteCellIdentity.band != INT_MAX && rilLteCellIdentity.band != 0) {
    hidlLteCellIdentity.bands.resize(1);
    hidlLteCellIdentity.bands[0] = static_cast<V1_5::EutranBands>(rilLteCellIdentity.band);
    QCRIL_LOG_DEBUG("band - %d", hidlLteCellIdentity.bands[0]);
  }
  if (rilLteCellIdentity.band == INT_MAX) {
    hidlLteCellIdentity.bands.resize(1);
    hidlLteCellIdentity.bands[0] = calculateBandFromEarfcn(hidlLteCellIdentity.base.base.earfcn);
    QCRIL_LOG_DEBUG("band - %d", hidlLteCellIdentity.bands[0]);
  }
}

V1_5::EutranBands calculateBandFromEarfcn(int earfcn) {
  if (earfcn >= 70596) {
      return V1_5::EutranBands::BAND_88;
  } else if (earfcn >= 70546) {
      return V1_5::EutranBands::BAND_87;
  } else if (earfcn >= 70366) {
      return V1_5::EutranBands::BAND_85;
  } else if (earfcn >= 69036) {
      return V1_5::EutranBands::BAND_74;
  } else if (earfcn >= 68986) {
      return V1_5::EutranBands::BAND_73;
  } else if (earfcn >= 68936) {
      return V1_5::EutranBands::BAND_72;
  } else if (earfcn >= 68586) {
      return V1_5::EutranBands::BAND_71;
  } else if (earfcn >= 68336) {
      return V1_5::EutranBands::BAND_70;
  } else if (earfcn >= 67536) {
      return V1_5::EutranBands::BAND_68;
  } else if (earfcn >= 66436) {
      return V1_5::EutranBands::BAND_66;
  } else if (earfcn >= 65536) {
      return V1_5::EutranBands::BAND_65;
  } else if (earfcn >= 60140) {
      return V1_5::EutranBands::BAND_53;
  } else if (earfcn >= 59140) {
      return V1_5::EutranBands::BAND_52;
  } else if (earfcn >= 59090) {
      return V1_5::EutranBands::BAND_51;
  } else if (earfcn >= 58240) {
      return V1_5::EutranBands::BAND_50;
  } else if (earfcn >= 56740) {
      return V1_5::EutranBands::BAND_49;
  } else if (earfcn >= 55240) {
      return V1_5::EutranBands::BAND_48;
  } else if (earfcn >= 54540) {
      return V1_5::EutranBands::BAND_47;
  } else if (earfcn >= 46790) {
      return V1_5::EutranBands::BAND_46;
  } else if (earfcn >= 46590) {
      return V1_5::EutranBands::BAND_45;
  } else if (earfcn >= 45590) {
      return V1_5::EutranBands::BAND_44;
  } else if (earfcn >= 43590) {
      return V1_5::EutranBands::BAND_43;
  } else if (earfcn >= 41590) {
      return V1_5::EutranBands::BAND_42;
  } else if (earfcn >= 39650) {
      return V1_5::EutranBands::BAND_41;
  } else if (earfcn >= 38650) {
      return V1_5::EutranBands::BAND_40;
  } else if (earfcn >= 38250) {
      return V1_5::EutranBands::BAND_39;
  } else if (earfcn >= 37750) {
      return V1_5::EutranBands::BAND_38;
  } else if (earfcn >= 37550) {
      return V1_5::EutranBands::BAND_37;
  } else if (earfcn >= 36950) {
      return V1_5::EutranBands::BAND_36;
  } else if (earfcn >= 36350) {
      return V1_5::EutranBands::BAND_35;
  } else if (earfcn >= 36200) {
      return V1_5::EutranBands::BAND_34;
  } else if (earfcn >= 36000) {
      return V1_5::EutranBands::BAND_33;
  } else if (earfcn >= 9870) {
      return V1_5::EutranBands::BAND_31;
  } else if (earfcn >= 9770) {
      return V1_5::EutranBands::BAND_30;
  } else if (earfcn >= 9210) {
      return V1_5::EutranBands::BAND_28;
  } else if (earfcn >= 9040) {
      return V1_5::EutranBands::BAND_27;
  } else if (earfcn >= 8690) {
      return V1_5::EutranBands::BAND_26;
  } else if (earfcn >= 8040) {
      return V1_5::EutranBands::BAND_25;
  } else if (earfcn >= 7700) {
      return V1_5::EutranBands::BAND_24;
  } else if (earfcn >= 7500) {
      return V1_5::EutranBands::BAND_23;
  } else if (earfcn >= 6600) {
      return V1_5::EutranBands::BAND_22;
  } else if (earfcn >= 6450) {
      return V1_5::EutranBands::BAND_21;
  } else if (earfcn >= 6150) {
      return V1_5::EutranBands::BAND_20;
  } else if (earfcn >= 6000) {
      return V1_5::EutranBands::BAND_19;
  } else if (earfcn >= 5850) {
      return V1_5::EutranBands::BAND_18;
  } else if (earfcn >= 5730) {
      return V1_5::EutranBands::BAND_17;
  } else if (earfcn >= 5280) {
      return V1_5::EutranBands::BAND_14;
  } else if (earfcn >= 5180) {
      return V1_5::EutranBands::BAND_13;
  } else if (earfcn >= 5010) {
      return V1_5::EutranBands::BAND_12;
  } else if (earfcn >= 4750) {
      return V1_5::EutranBands::BAND_11;
  } else if (earfcn >= 4150) {
      return V1_5::EutranBands::BAND_10;
  } else if (earfcn >= 3800) {
      return V1_5::EutranBands::BAND_9;
  } else if (earfcn >= 3450) {
      return V1_5::EutranBands::BAND_8;
  } else if (earfcn >= 2750) {
      return V1_5::EutranBands::BAND_7;
  } else if (earfcn >= 2650) {
      return V1_5::EutranBands::BAND_6;
  } else if (earfcn >= 2400) {
      return V1_5::EutranBands::BAND_5;
  } else if (earfcn >= 1950) {
      return V1_5::EutranBands::BAND_4;
  } else if (earfcn >= 1200) {
      return V1_5::EutranBands::BAND_3;
  } else if (earfcn >= 600) {
      return V1_5::EutranBands::BAND_2;
  } else if (earfcn >= 0) {
      return V1_5::EutranBands::BAND_1;
  }
  return V1_5::EutranBands::BAND_1;
}

void convertNrCellIdentityRilToHidl(const RIL_CellIdentityNr& rilNrCellIdentity,
                                    V1_5::CellIdentityNr& hidlNrCellIdentity) {
  (void)getHidlPlmnFromCellIdentity(hidlNrCellIdentity.base.mcc, hidlNrCellIdentity.base.mnc,
                                    rilNrCellIdentity);

  hidlNrCellIdentity.base.nci = rilNrCellIdentity.nci;
  hidlNrCellIdentity.base.pci = rilNrCellIdentity.pci;
  hidlNrCellIdentity.base.tac = rilNrCellIdentity.tac;
  hidlNrCellIdentity.base.nrarfcn = rilNrCellIdentity.nrarfcn;

  hidlNrCellIdentity.base.operatorNames.alphaLong =
      hidl_string(rilNrCellIdentity.operatorNames.alphaLong,
                  strnlen(rilNrCellIdentity.operatorNames.alphaLong,
                          sizeof(rilNrCellIdentity.operatorNames.alphaLong) - 1));

  hidlNrCellIdentity.base.operatorNames.alphaShort =
      hidl_string(rilNrCellIdentity.operatorNames.alphaShort,
                  strnlen(rilNrCellIdentity.operatorNames.alphaShort,
                          sizeof(rilNrCellIdentity.operatorNames.alphaShort) - 1));

  // Fill band only if its valid
  if (rilNrCellIdentity.band != INT_MAX && rilNrCellIdentity.band != 0) {
    hidlNrCellIdentity.bands.resize(1);
    QCRIL_LOG_DEBUG("band - %d", rilNrCellIdentity.band);
    hidlNrCellIdentity.bands[0] = static_cast<V1_5::NgranBands>(rilNrCellIdentity.band);
  }
}

void convertCellIdentityRilToHidl(const RIL_CellIdentity_v16& in, V1_5::CellIdentity& out) {
  switch (in.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      V1_5::CellIdentityGsm cellIdentityGsm{};
      convertGsmCellIdentityRilToHidl(in.cellIdentityGsm, cellIdentityGsm);
      out.gsm(std::move(cellIdentityGsm));
      break;
    }
    case RIL_CELL_INFO_TYPE_WCDMA: {
      V1_5::CellIdentityWcdma cellIdentityWcdma{};
      convertWcdmaCellIdentityRilToHidl(in.cellIdentityWcdma, cellIdentityWcdma);
      out.wcdma(std::move(cellIdentityWcdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      V1_2::CellIdentityCdma cellIdentityCdma{};
      convertCdmaCellIdentityRilToHidl(in.cellIdentityCdma, cellIdentityCdma);
      out.cdma(std::move(cellIdentityCdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      V1_5::CellIdentityLte cellIdentityLte{};
      convertLteCellIdentityRilToHidl(in.cellIdentityLte, cellIdentityLte);
      out.lte(std::move(cellIdentityLte));
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      V1_5::CellIdentityTdscdma cellIdentityTdscdma{};
      convertTdscdmaCellIdentityRilToHidl(in.cellIdentityTdscdma, cellIdentityTdscdma);
      out.tdscdma(std::move(cellIdentityTdscdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      V1_5::CellIdentityNr cellIdentityNr{};
      convertNrCellIdentityRilToHidl(in.cellIdentityNr, cellIdentityNr);
      out.nr(std::move(cellIdentityNr));
      break;
    }

    default: {
      break;
    }
  }
}

void convertRilBarringServiceType(const qcril::interfaces::RilBarringServiceType& inBarringService,
                                  V1_5::BarringInfo::ServiceType& outBarringService) {
  if (qcril::interfaces::RilBarringServiceType::CS_SERVICE == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_SERVICE;
  } else if (qcril::interfaces::RilBarringServiceType::PS_SERVICE == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::PS_SERVICE;
  } else if (qcril::interfaces::RilBarringServiceType::CS_VOICE == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_VOICE;
  } else if (qcril::interfaces::RilBarringServiceType::MO_SIGNALLING == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::MO_SIGNALLING;
  } else if (qcril::interfaces::RilBarringServiceType::MO_DATA == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::MO_DATA;
  } else if (qcril::interfaces::RilBarringServiceType::CS_FALLBACK == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_FALLBACK;
  } else if (qcril::interfaces::RilBarringServiceType::MMTEL_VOICE == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::MMTEL_VOICE;
  } else if (qcril::interfaces::RilBarringServiceType::MMTEL_VIDEO == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::MMTEL_VIDEO;
  } else if (qcril::interfaces::RilBarringServiceType::EMERGENCY == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::EMERGENCY;
  } else if (qcril::interfaces::RilBarringServiceType::SMS == inBarringService) {
    outBarringService = V1_5::BarringInfo::ServiceType::SMS;
  }
}

void convertRilBarringType(const qcril::interfaces::RilBarringType& inBarringType,
                           V1_5::BarringInfo::BarringType& outBarringType) {
  if (qcril::interfaces::RilBarringType::NONE == inBarringType) {
    outBarringType = V1_5::BarringInfo::BarringType::NONE;
  } else if (qcril::interfaces::RilBarringType::CONDITIONAL == inBarringType) {
    outBarringType = V1_5::BarringInfo::BarringType::CONDITIONAL;
  } else if (qcril::interfaces::RilBarringType::UNCONDITIONAL == inBarringType) {
    outBarringType = V1_5::BarringInfo::BarringType::UNCONDITIONAL;
  }
}

void convertRilBarringInfoList(const std::vector<qcril::interfaces::RILBarringInfo>& rilBarInfo,
                               hidl_vec<V1_5::BarringInfo>& records) {
  auto num = rilBarInfo.size();
  records.resize(num);
  V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional cond{};

  QCRIL_LOG_INFO("num: %lu", num);
  for (unsigned int i = 0; i < num; i++) {
    QCRIL_LOG_INFO(
        "iteration i: %d, serviceType: %d, barringType: %d, barringFactor: %d, "
        "barringTimeSeconds: %d, isBarred: %d",
        i, rilBarInfo[i].barringService, rilBarInfo[i].barringType,
        rilBarInfo[i].typeSpecificInfo.barringFactor,
        rilBarInfo[i].typeSpecificInfo.barringTimeSeconds,
        rilBarInfo[i].typeSpecificInfo.isBarred);
    convertRilBarringServiceType(rilBarInfo[i].barringService, records[i].serviceType);
    convertRilBarringType(rilBarInfo[i].barringType, records[i].barringType);
    cond.factor = rilBarInfo[i].typeSpecificInfo.barringFactor;
    cond.timeSeconds = rilBarInfo[i].typeSpecificInfo.barringTimeSeconds;
    cond.isBarred = rilBarInfo[i].typeSpecificInfo.isBarred;
    records[i].barringTypeSpecificInfo.conditional(cond);
  }
}

void fillVoiceRegistrationStateResponse(V1_5::RegStateResult& out,
                                        const RIL_VoiceRegistrationStateResponse& in) {
  out.regState = static_cast<V1_0::RegState>(in.regState);
  out.rat = static_cast<V1_4::RadioTechnology>(in.rat);
  out.reasonForDenial = static_cast<V1_5::RegistrationFailCause>(in.reasonForDenial);

  convertCellIdentityRilToHidl(in.cellIdentity, out.cellIdentity);

  if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
    V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo cdmaInfo;
    cdmaInfo.cssSupported = static_cast<bool>(in.cssSupported);
    cdmaInfo.roamingIndicator = in.roamingIndicator;
    cdmaInfo.systemIsInPrl = static_cast<V1_5::PrlIndicator>(in.systemIsInPrl);
    cdmaInfo.defaultRoamingIndicator = in.defaultRoamingIndicator;
    out.accessTechnologySpecificInfo.cdmaInfo(std::move(cdmaInfo));
  } else {
    out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
                                     string(in.cellIdentity.cellIdentityGsm.mnc));
  }
}

void fillDataRegistrationStateResponse(V1_5::RegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in) {
  out.regState = static_cast<V1_0::RegState>(in.regState);
  out.rat = static_cast<V1_4::RadioTechnology>(in.rat);

  if (in.cellIdentity.cellInfoType != RIL_CELL_INFO_TYPE_CDMA) {
    out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
                                     string(in.cellIdentity.cellIdentityGsm.mnc));
  }

  // TODO: Add mapping
  out.reasonForDenial = static_cast<V1_5::RegistrationFailCause>(in.reasonDataDenied);

  convertCellIdentityRilToHidl(in.cellIdentity, out.cellIdentity);

  if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_LTE) {
    V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo eutranInfo;

    if (in.lteVopsInfoValid) {
      eutranInfo.lteVopsInfo.isVopsSupported = static_cast<bool>(in.lteVopsInfo.isVopsSupported);
      eutranInfo.lteVopsInfo.isEmcBearerSupported =
          static_cast<bool>(in.lteVopsInfo.isEmcBearerSupported);
    }

    if (in.nrIndicatorsValid) {
      eutranInfo.nrIndicators.isEndcAvailable = static_cast<bool>(in.nrIndicators.isEndcAvailable);
      eutranInfo.nrIndicators.isDcNrRestricted = static_cast<bool>(in.nrIndicators.isDcNrRestricted);
      eutranInfo.nrIndicators.isNrAvailable =
          static_cast<bool>(in.nrIndicators.plmnInfoListR15Available);
    }

    if (in.lteVopsInfoValid || in.nrIndicatorsValid) {
      out.accessTechnologySpecificInfo.eutranInfo(std::move(eutranInfo));
    }
  }
}

RIL_UIM_PersoSubstate convertHidlToRilPersoType(V1_5::PersoSubstate persoType) {
  QCRIL_LOG_INFO("convertHidlToRilPersoType: persoType %d ", persoType);
  switch (persoType) {
    case V1_5::PersoSubstate::SIM_SPN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN;
    case V1_5::PersoSubstate::SIM_SPN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN_PUK;
    case V1_5::PersoSubstate::SIM_SP_EHPLMN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN;
    case V1_5::PersoSubstate::SIM_SP_EHPLMN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK;
    case V1_5::PersoSubstate::SIM_ICCID:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID;
    case V1_5::PersoSubstate::SIM_ICCID_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID_PUK;
    case V1_5::PersoSubstate::SIM_IMPI:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI;
    case V1_5::PersoSubstate::SIM_IMPI_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI_PUK;
    case V1_5::PersoSubstate::SIM_NS_SP:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP;
    case V1_5::PersoSubstate::SIM_NS_SP_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP_PUK;
    default:
      return (RIL_UIM_PersoSubstate)persoType;
      /* Only above values require conversion, others are 1:1 mapped */
  }
}

RIL_RadioTechnology getRilRadioTechnologyFromRan(V1_5::RadioAccessNetworks ran) {
  switch (ran) {
    case V1_5::RadioAccessNetworks::GERAN:
      return RADIO_TECH_GSM;
    case V1_5::RadioAccessNetworks::UTRAN:
      return RADIO_TECH_UMTS;
    case V1_5::RadioAccessNetworks::EUTRAN:
      return RADIO_TECH_LTE;
    case V1_5::RadioAccessNetworks::NGRAN:
      return RADIO_TECH_5G;
    case V1_5::RadioAccessNetworks::CDMA2000:
      return RADIO_TECH_IS95A;
    default: {
      break;
    }
  }

  return RADIO_TECH_UNKNOWN;
}

void convertCellInfoRilToHidl(const RIL_CellInfo_v12& rilCellInfo, V1_5::CellInfo& hidlCellInfo) {
  switch (rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      V1_5::CellInfoGsm cellInfoGsm{};
      convertGsmCellIdentityRilToHidl(rilCellInfo.CellInfo.gsm.cellIdentityGsm,
                                      cellInfoGsm.cellIdentityGsm);
      cellInfoGsm.signalStrengthGsm.signalStrength =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.signalStrength;
      cellInfoGsm.signalStrengthGsm.bitErrorRate =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.bitErrorRate;
      cellInfoGsm.signalStrengthGsm.timingAdvance =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.timingAdvance;
      hidlCellInfo.ratSpecificInfo.gsm(std::move(cellInfoGsm));
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      V1_5::CellInfoWcdma cellInfoWcdma{};
      convertWcdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma,
                                        cellInfoWcdma.cellIdentityWcdma);
      cellInfoWcdma.signalStrengthWcdma.base.signalStrength =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.signalStrength;
      cellInfoWcdma.signalStrengthWcdma.base.bitErrorRate =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;
      hidlCellInfo.ratSpecificInfo.wcdma(std::move(cellInfoWcdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      V1_2::CellInfoCdma cellInfoCdma{};
      convertCdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.cdma.cellIdentityCdma,
                                       cellInfoCdma.cellIdentityCdma);
      cellInfoCdma.signalStrengthCdma.dbm = rilCellInfo.CellInfo.cdma.signalStrengthCdma.dbm;
      cellInfoCdma.signalStrengthCdma.ecio = rilCellInfo.CellInfo.cdma.signalStrengthCdma.ecio;
      cellInfoCdma.signalStrengthEvdo.dbm = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.dbm;
      cellInfoCdma.signalStrengthEvdo.ecio = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.ecio;
      cellInfoCdma.signalStrengthEvdo.signalNoiseRatio =
          rilCellInfo.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;
      hidlCellInfo.ratSpecificInfo.cdma(std::move(cellInfoCdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      V1_5::CellInfoLte cellInfoLte{};
      convertLteCellIdentityRilToHidl(rilCellInfo.CellInfo.lte.cellIdentityLte,
                                      cellInfoLte.cellIdentityLte);
      cellInfoLte.signalStrengthLte.signalStrength =
          rilCellInfo.CellInfo.lte.signalStrengthLte.signalStrength;
      cellInfoLte.signalStrengthLte.rsrp = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrp;
      cellInfoLte.signalStrengthLte.rsrq = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrq;
      cellInfoLte.signalStrengthLte.rssnr = rilCellInfo.CellInfo.lte.signalStrengthLte.rssnr;
      cellInfoLte.signalStrengthLte.cqi = rilCellInfo.CellInfo.lte.signalStrengthLte.cqi;
      cellInfoLte.signalStrengthLte.timingAdvance =
          rilCellInfo.CellInfo.lte.signalStrengthLte.timingAdvance;
      hidlCellInfo.ratSpecificInfo.lte(std::move(cellInfoLte));
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      V1_5::CellInfoNr cellInfoNr{};
      convertNrCellIdentityRilToHidl(rilCellInfo.CellInfo.nr.cellIdentityNr,
                                     cellInfoNr.cellIdentityNr);
      cellInfoNr.signalStrengthNr.ssRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrp;
      cellInfoNr.signalStrengthNr.ssRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrq;
      cellInfoNr.signalStrengthNr.ssSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.ssSinr;
      cellInfoNr.signalStrengthNr.csiRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrp;
      cellInfoNr.signalStrengthNr.csiRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrq;
      cellInfoNr.signalStrengthNr.csiSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.csiSinr;
      hidlCellInfo.ratSpecificInfo.nr(std::move(cellInfoNr));
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      V1_5::CellInfoTdscdma cellInfoTdscdma{};
      convertTdscdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma,
                                          cellInfoTdscdma.cellIdentityTdscdma);
      cellInfoTdscdma.signalStrengthTdscdma.rscp =
          rilCellInfo.CellInfo.tdscdma.signalStrengthTdscdma.rscp;
      hidlCellInfo.ratSpecificInfo.tdscdma(std::move(cellInfoTdscdma));
      break;
    }

    default: {
      break;
    }
  }
}

void convertRilCellInfoList(const std::vector<RIL_CellInfo_v12>& rilCellInfo,
                            hidl_vec<V1_5::CellInfo>& records) {
  auto num = rilCellInfo.size();
  records.resize(num);

  for (unsigned int i = 0; i < num; i++) {
    records[i].registered = rilCellInfo[i].registered;
    // TODO: Use a conversion function to translate RIL_TimeStampType to HIDL TimeStampType
    records[i].timeStampType = (V1_0::TimeStampType)rilCellInfo[i].timeStampType;
    records[i].timeStamp = rilCellInfo[i].timeStamp;
    records[i].connectionStatus = (V1_2::CellConnectionStatus)rilCellInfo[i].connStatus;
    convertCellInfoRilToHidl(rilCellInfo[i], records[i]);
  }
}

rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork_1_5(
    V1_5::AccessNetwork ran) {
    if (ran == V1_5::AccessNetwork::NGRAN) {
        return rildata::AccessNetwork_t::NGRAN;
    } else {
        return convertHidlAccessNetworkToDataAccessNetwork_1_4(
            (V1_4::AccessNetwork)ran);
    }
}

vector<V1_5::LinkAddress> convertLinkAddressToHidlVector(vector<rildata::LinkAddress_t> addresses) {
    vector<V1_5::LinkAddress> hidlAddressesVector;
    for (rildata::LinkAddress_t addr : addresses) {
        V1_5::LinkAddress hidlAddr = {
            .address = addr.address,
            .properties = (int)addr.properties,
            .deprecationTime = addr.deprecationTime,
            .expirationTime = addr.expirationTime,
        };
        hidlAddressesVector.push_back(hidlAddr);
    }
    return hidlAddressesVector;
}

V1_5::SetupDataCallResult convertDcResultToHidlDcResult_1_5(const rildata::DataCallResult_t& result) {
    V1_5::SetupDataCallResult dcResult = {};
    dcResult.cause = convertDcFailCauseToHidlDcFailCause_1_4(result.cause);
    dcResult.suggestedRetryTime = result.suggestedRetryTime;
    dcResult.cid = result.cid;
    dcResult.active = (V1_4::DataConnActiveStatus)result.active;
    dcResult.type = convertStringToPdpProtocolType(result.type);
    dcResult.ifname = result.ifname;
    dcResult.addresses = convertLinkAddressToHidlVector(result.linkAddresses);
    dcResult.dnses = convertAddrStringToHidlStringVector(result.dnses);
    dcResult.gateways = convertAddrStringToHidlStringVector(result.gateways);
    dcResult.pcscf = convertAddrStringToHidlStringVector(result.pcscf);
    dcResult.mtuV4 = result.mtuV4;
    dcResult.mtuV6 = result.mtuV6;
    return dcResult;
}

rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil(const V1_5::DataProfileInfo& profile) {
    rildata::DataProfileInfo_t rilProfile = {};
    rilProfile.profileId = (rildata::DataProfileId_t)profile.profileId;
    rilProfile.apn = profile.apn;
    rilProfile.protocol = convertPdpProtocolTypeToString(profile.protocol);
    rilProfile.roamingProtocol = convertPdpProtocolTypeToString(profile.roamingProtocol);
    rilProfile.authType = (rildata::ApnAuthType_t)profile.authType;
    rilProfile.username = profile.user;
    rilProfile.password = profile.password;
    rilProfile.dataProfileInfoType = (rildata::DataProfileInfoType_t)profile.type,
    rilProfile.maxConnsTime = profile.maxConnsTime;
    rilProfile.maxConns = profile.maxConns;
    rilProfile.waitTime = profile.waitTime;
    rilProfile.enableProfile = profile.enabled;
    rilProfile.supportedApnTypesBitmap = (rildata::ApnTypes_t)profile.supportedApnTypesBitmap;
    rilProfile.bearerBitmap = (rildata::RadioAccessFamily_t)profile.bearerBitmap;
    rilProfile.mtu = profile.mtuV4;
    rilProfile.preferred = profile.preferred;
    rilProfile.persistent = profile.persistent;
    return rilProfile;
}

V1_0::RadioError convertGetIccCardStatusResponse_1_5(V1_5::CardStatus& cardStatus,
                                                 std::shared_ptr<RIL_UIM_CardStatus> p_cur) {
  V1_0::RadioError ret = V1_0::RadioError::INVALID_RESPONSE;
  if (p_cur) {
    ret = static_cast<V1_0::RadioError>(p_cur->err);
    if (p_cur->gsm_umts_subscription_app_index >= p_cur->num_applications ||
        p_cur->cdma_subscription_app_index >= p_cur->num_applications ||
        p_cur->ims_subscription_app_index >= p_cur->num_applications) {
      //      RLOGE("Invalid response");
      if (p_cur->err == RIL_UIM_E_SUCCESS) {
        ret = V1_0::RadioError::INVALID_RESPONSE;
      }
    } else {
      cardStatus.base.base.base.cardState = static_cast<V1_0::CardState>(p_cur->card_state);
      cardStatus.base.base.base.universalPinState = static_cast<V1_0::PinState>(p_cur->universal_pin_state);
      cardStatus.base.base.base.gsmUmtsSubscriptionAppIndex = p_cur->gsm_umts_subscription_app_index;
      cardStatus.base.base.base.cdmaSubscriptionAppIndex = p_cur->cdma_subscription_app_index;
      cardStatus.base.base.base.imsSubscriptionAppIndex = p_cur->ims_subscription_app_index;
       //Fill 1.1 hal fields
       cardStatus.base.base.physicalSlotId = p_cur->physical_slot_id;
       cardStatus.base.base.atr            = p_cur->atr;
       cardStatus.base.base.iccid          = p_cur->iccid;

      //Fill 1.4 hal fields
      if(!p_cur->eid.empty()) {
          cardStatus.base.eid = p_cur->eid;
      }
      RIL_UIM_AppStatus* rilAppStatus = p_cur->applications;
      cardStatus.base.base.base.applications.resize(p_cur->num_applications);
       cardStatus.applications.resize(p_cur->num_applications);
      V1_0::AppStatus* appStatus_1_0 = cardStatus.base.base.base.applications.data();
      V1_5::AppStatus* appStatus = cardStatus.applications.data();
#if VDBG
//      RLOGD("num_applications %d", p_cur->num_applications);
#endif
      for (int i = 0; i < p_cur->num_applications; i++) {
        //Fill 1.0 fields
        appStatus_1_0[i].appType = static_cast<V1_0::AppType>(rilAppStatus[i].app_type);
        appStatus_1_0[i].appState = static_cast<V1_0::AppState>(rilAppStatus[i].app_state);
        appStatus_1_0[i].persoSubstate =
            static_cast<V1_0::PersoSubstate>(rilAppStatus[i].perso_substate);
        appStatus_1_0[i].aidPtr = rilAppStatus[i].aid_ptr;
        appStatus_1_0[i].appLabelPtr = rilAppStatus[i].app_label_ptr;
        appStatus_1_0[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
        appStatus_1_0[i].pin1 = static_cast<V1_0::PinState>(rilAppStatus[i].pin1);
        appStatus_1_0[i].pin2 = static_cast<V1_0::PinState>(rilAppStatus[i].pin2);

        //Fill 1.5 Fields
        appStatus[i].base.appType = static_cast<V1_0::AppType>(rilAppStatus[i].app_type);
        appStatus[i].base.appState = static_cast<V1_0::AppState>(rilAppStatus[i].app_state);
        appStatus[i].base.persoSubstate =
            static_cast<V1_0::PersoSubstate>(rilAppStatus[i].perso_substate);
        appStatus[i].base.aidPtr = rilAppStatus[i].aid_ptr;
        appStatus[i].base.appLabelPtr = rilAppStatus[i].app_label_ptr;
        appStatus[i].base.pin1Replaced = rilAppStatus[i].pin1_replaced;
        appStatus[i].base.pin1 = static_cast<V1_0::PinState>(rilAppStatus[i].pin1);
        appStatus[i].base.pin2 = static_cast<V1_0::PinState>(rilAppStatus[i].pin2);

        //Fill 1.5 fields

        appStatus[i].persoSubstate = (V1_5::PersoSubstate)rilAppStatus[i].perso_substate;
      }
    }
  }
  return ret;
}

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
