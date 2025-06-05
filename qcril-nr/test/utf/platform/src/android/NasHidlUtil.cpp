/*===========================================================================
 *
 *    Copyright (c) 2019 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "platform/android/NasHidlUtil.h"
#include "ril_utf_core.h"

static void convertHidlMccMncToRil(const hidl_string &in, char* out) {
    strlcpy(out, in.c_str(), MAX_RIL_MCC_MNC_SIZE);
}

static void convertHidlOperatorNamesToRil(const V1_2::CellIdentityOperatorNames& in,
        RIL_CellIdentityOperatorNames& out) {
    strlcpy(out.alphaShort, in.alphaShort.c_str(), MAX_OPERATOR_NAME_LEN);
    strlcpy(out.alphaLong, in.alphaLong.c_str(), MAX_OPERATOR_NAME_LEN);
}

void fillCellIdentityGsm(const V1_0::CellIdentityGsm &in, RIL_CellIdentityGsm_v12 &out) {
    convertHidlMccMncToRil(in.mcc, out.mcc);
    convertHidlMccMncToRil(in.mnc, out.mnc);
    out.lac = in.lac;
    out.cid = in.cid;
    out.arfcn = in.arfcn;
    out.bsic = in.bsic;
}

void fillCellIdentityWcdma(const V1_0::CellIdentityWcdma &in, RIL_CellIdentityWcdma_v12 &out) {
    convertHidlMccMncToRil(in.mcc, out.mcc);
    convertHidlMccMncToRil(in.mnc, out.mnc);
    out.lac = in.lac;
    out.cid = in.cid;
    out.psc = in.psc;
    out.uarfcn = in.uarfcn;
}

void fillCellIdentityCdma(const V1_0::CellIdentityCdma &in, RIL_CellIdentityCdma &out) {
    out.networkId = in.networkId;
    out.systemId = in.systemId;
    out.basestationId = in.baseStationId;
    out.longitude = in.longitude;
    out.latitude = in.latitude;
}

void fillCellIdentityLte(const V1_0::CellIdentityLte &in, RIL_CellIdentityLte_v12 &out) {
    convertHidlMccMncToRil(in.mcc, out.mcc);
    convertHidlMccMncToRil(in.mnc, out.mnc);
    out.ci = in.ci;
    out.pci = in.pci;
    out.tac = in.tac;
    out.earfcn = in.earfcn;
}

void fillCellIdentityTdscdma(const V1_0::CellIdentityTdscdma &in, RIL_CellIdentityTdscdma &out) {
    convertHidlMccMncToRil(in.mcc, out.mcc);
    convertHidlMccMncToRil(in.mnc, out.mnc);
    out.lac = in.lac;
    out.cid = in.cid;
    out.cpid = in.cpid;
}

void fillCellIdentityGsm(const V1_2::CellIdentityGsm &in, RIL_CellIdentityGsm_v12 &out) {
    fillCellIdentityGsm(in.base, out);
    convertHidlOperatorNamesToRil(in.operatorNames, out.operatorNames);
}

void fillCellIdentityWcdma(const V1_2::CellIdentityWcdma &in, RIL_CellIdentityWcdma_v12 &out) {
    fillCellIdentityWcdma(in.base, out);
    convertHidlOperatorNamesToRil(in.operatorNames, out.operatorNames);
}

void fillCellIdentityCdma(const V1_2::CellIdentityCdma &in, RIL_CellIdentityCdma &out) {
    fillCellIdentityCdma(in.base, out);
    convertHidlOperatorNamesToRil(in.operatorNames, out.operatorNames);
}

void fillCellIdentityLte(const V1_2::CellIdentityLte &in, RIL_CellIdentityLte_v12 &out) {
    fillCellIdentityLte(in.base, out);
    convertHidlOperatorNamesToRil(in.operatorNames, out.operatorNames);
    out.bandwidth = in.bandwidth;
}

void fillCellIdentityTdscdma(const V1_2::CellIdentityTdscdma &in, RIL_CellIdentityTdscdma &out) {
    fillCellIdentityTdscdma(in.base, out);
    convertHidlOperatorNamesToRil(in.operatorNames, out.operatorNames);
    // RIL structure has no uarfcn field
}

void fillCellIdentityGsm(const V1_5::CellIdentityGsm &in, RIL_CellIdentityGsm_v12 &out) {
    fillCellIdentityGsm(in.base, out);
}

void fillCellIdentityWcdma(const V1_5::CellIdentityWcdma &in, RIL_CellIdentityWcdma_v12 &out) {
    fillCellIdentityWcdma(in.base, out);
}

void fillCellIdentityLte(const V1_5::CellIdentityLte &in, RIL_CellIdentityLte_v12 &out) {
    fillCellIdentityLte(in.base, out);
}

void fillCellIdentityNr(const V1_4::CellIdentityNr& in, RIL_CellIdentityNr& out)
{
    convertHidlMccMncToRil(in.mcc, out.mcc);
    convertHidlMccMncToRil(in.mnc, out.mnc);
    // TODO other fields
}

void fillCellIdentityNr(const V1_5::CellIdentityNr& in, RIL_CellIdentityNr& out)
{
    fillCellIdentityNr(in.base, out);
    // TODO other fields
}

void fillCellIdentityTdscdma(const V1_5::CellIdentityTdscdma &in, RIL_CellIdentityTdscdma &out) {
    fillCellIdentityTdscdma(in.base, out);
}

ril_request_operator_resp_from_qcril_t* convertHidlOperatorReponseToRil(
        const hidl_string& longName, const hidl_string& shortName, const hidl_string& numeric) {
    // use malloc since framework will free it use free()
    auto ptr = (ril_request_operator_resp_from_qcril_t*)malloc(
            sizeof(ril_request_operator_resp_from_qcril_t));
    if (ptr == nullptr)
        return nullptr;
    memset(ptr->operator_info_array, 0, NAS_OPERATOR_RESP_MAX_ARR_SIZE);

    auto operatorResp = (ril_request_operator_resp_t*)malloc(
            sizeof(ril_request_operator_resp_t));
    if (operatorResp == nullptr) {
        free(ptr);
        return nullptr;
    }
    memset(operatorResp, '\0', sizeof(*operatorResp));
    strlcpy(operatorResp->long_eons, longName.c_str(), sizeof(operatorResp->long_eons));
    strlcpy(operatorResp->short_eons, shortName.c_str(), sizeof(operatorResp->short_eons));
    strlcpy(operatorResp->mcc_mnc_ascii, numeric.c_str(), sizeof(operatorResp->mcc_mnc_ascii));

    ptr->operator_info_array[0] = operatorResp->long_eons;
    ptr->operator_info_array[1] = operatorResp->short_eons;
    ptr->operator_info_array[2] = operatorResp->mcc_mnc_ascii;

    return ptr;
}

static void fillNetworkStatus(V1_0::OperatorStatus status, char* buf, size_t len) {
    switch (status) {
    case V1_0::OperatorStatus::UNKNOWN:
        strlcpy(buf, "unknown", len);
        break;
    case V1_0::OperatorStatus::AVAILABLE:
        strlcpy(buf, "available", len);
        break;
    case V1_0::OperatorStatus::CURRENT:
        strlcpy(buf, "current", len);
        break;
    case V1_0::OperatorStatus::FORBIDDEN:
    default:
        strlcpy(buf, "forbidden", len);
        break;
    }
}

ril_request_query_available_networks_resp_t* convertHidlLegacyNetworkscanResponseToRil(
        const hidl_vec<V1_0::OperatorInfo>& networkInfos) {
    // use malloc since framework will free it use free()
    auto ptr = (ril_request_query_available_networks_resp_t *)malloc(
            sizeof(ril_request_query_available_networks_resp_t));
    if (ptr == nullptr)
        return ptr;
    memset(ptr, 0, sizeof(*ptr));

    unsigned int nw_cnt = networkInfos.size();
    if (nw_cnt == 0) { // keep the same logic as copy_ril_request_query_available_networks_resp()
        return ptr;
    }
    auto scanResp = (qcril_qmi_nw_scan_resp_helper_type *)malloc(
            nw_cnt * sizeof(qcril_qmi_nw_scan_resp_helper_type));
    if (scanResp == nullptr) {
        free(ptr);
        return nullptr;
    }
    memset(scanResp, 0, sizeof(*scanResp));

    unsigned int idx = 0;
    for (unsigned int item = 0; item < nw_cnt; item++) {
        ptr->nw_scan_info_array[idx++] = scanResp[item].long_eons;
        strlcpy(scanResp[item].long_eons, networkInfos[item].alphaLong.c_str(),
                sizeof(scanResp[item].long_eons));
        ptr->nw_scan_info_array[idx++] = scanResp[item].short_eons;
        strlcpy(scanResp[item].short_eons, networkInfos[item].alphaShort.c_str(),
                sizeof(scanResp[item].short_eons));
        ptr->nw_scan_info_array[idx++] = scanResp[item].mccmnc_info;
        strlcpy(scanResp[item].mccmnc_info, networkInfos[item].operatorNumeric.c_str(),
                sizeof(scanResp[item].mccmnc_info));
        ptr->nw_scan_info_array[idx++] = scanResp[item].network_status;
        fillNetworkStatus(networkInfos[item].status, scanResp[item].network_status,
                sizeof(scanResp[item].network_status));
    }
    return ptr;
}

int convertToRil(const V1_0::GsmSignalStrength &in, RIL_GW_SignalStrength &out) {
    out.signalStrength = in.signalStrength;
    out.bitErrorRate = in.bitErrorRate;
    return 0;
}

int convertToRil(const V1_0::GsmSignalStrength &in, RIL_WCDMA_SignalStrength &out) {
    out.signalStrength = in.signalStrength;
    out.bitErrorRate = in.bitErrorRate;
    return 0;
}

int convertToRil(const V1_0::WcdmaSignalStrength &in, RIL_GW_SignalStrength &out) {
    out.signalStrength = in.signalStrength;
    out.bitErrorRate = in.bitErrorRate;
    return 0;
}

int convertToRil(const V1_0::WcdmaSignalStrength &in, RIL_WCDMA_SignalStrength &out) {
    out.signalStrength = in.signalStrength;
    out.bitErrorRate = in.bitErrorRate;
    return 0;
}

int convertToRil(const V1_2::WcdmaSignalStrength &in, RIL_WCDMA_SignalStrength &out) {
    int ret = convertToRil(in.base, out);
    if (ret) return ret;
    out.rscp = in.rscp;
    out.ecio = in.ecno;
    return 0;
}

int convertToRil(const V1_0::CdmaSignalStrength &in, RIL_CDMA_SignalStrength &out) {
    out.dbm = in.dbm;
    out.ecio = in.ecio;
    return 0;
}

int convertToRil(const V1_0::EvdoSignalStrength &in, RIL_EVDO_SignalStrength &out) {
    out.dbm = in.dbm;
    out.ecio = in.ecio;
    out.signalNoiseRatio = in.signalNoiseRatio;
    return 0;
}

int convertToRil(const V1_0::LteSignalStrength &in, RIL_LTE_SignalStrength_v8 &out) {
    out.signalStrength = in.signalStrength;
    out.rsrp = in.rsrp;
    out.rsrq = in.rsrq;
    out.rssnr = in.rssnr;
    out.cqi = in.cqi;
    out.timingAdvance = in.timingAdvance;

    // Fixup LTE for backwards compatibility
    // signalStrength: -1 -> 99
    if (out.signalStrength == 99) {
        out.signalStrength = -1;
    }

    // rsrp: INT_MAX -> 1, all other positive value to negative
    if (out.rsrp == INT_MAX) {
        out.rsrp = -1;
    } else if (out.rsrp > 1) {
        out.rsrp = -out.rsrp;
    }

    // rsrq: INT_MAX -> -1
    if (out.rsrq == INT_MAX) {
        out.rsrq = -1;
    }

    // cqi: INT_MAX -> -1
    if (out.cqi == INT_MAX) {
        out.cqi = -1;
    }
    return 0;
}

int convertToRil(const V1_0::TdScdmaSignalStrength &in, RIL_TD_SCDMA_SignalStrength &out) {
    out.rscp = in.rscp;
    return 0;
}

int convertToRil(const V1_2::TdscdmaSignalStrength &in, RIL_TD_SCDMA_SignalStrength &out) {
    out.rscp = in.rscp;
    return 0;
}

int convertToRil(const V1_6::NrSignalStrength &in, RIL_NR_SignalStrength &out) {
    out.ssRsrp = in.base.ssRsrp;
    out.ssRsrq = in.base.ssRsrq;
    out.ssSinr = in.base.ssSinr;
    out.csiRsrp = in.base.csiRsrp;
    out.csiRsrq = in.base.csiRsrq;
    out.csiSinr = in.base.csiSinr;
    return 0;
}

int convertToRil(const V1_6::LteSignalStrength &in, RIL_LTE_SignalStrength_v8 &out) {
    return convertToRil(in.base, out);
}

void convertHidlSignalStrengthToRil(const V1_0::SignalStrength& in,
        RIL_SignalStrength_v10& out) {
    convertToRil(in.gw, out.GW_SignalStrength);
    convertToRil(in.gw, out.WCDMA_SignalStrength);
    convertToRil(in.cdma, out.CDMA_SignalStrength);
    convertToRil(in.evdo, out.EVDO_SignalStrength);
    convertToRil(in.lte, out.LTE_SignalStrength);
    convertToRil(in.tdScdma, out.TD_SCDMA_SignalStrength);
}

void convertHidlSignalStrengthToRil(const V1_2::SignalStrength& in,
        RIL_SignalStrength_v10& out) {
    convertToRil(in.gsm, out.GW_SignalStrength);
    convertToRil(in.wcdma, out.WCDMA_SignalStrength);
    convertToRil(in.cdma, out.CDMA_SignalStrength);
    convertToRil(in.evdo, out.EVDO_SignalStrength);
    convertToRil(in.lte, out.LTE_SignalStrength);
    convertToRil(in.tdScdma, out.TD_SCDMA_SignalStrength);
}

void convertHidlSignalStrengthToRil(const V1_4::SignalStrength& in,
        RIL_SignalStrength_v10& out) {
    convertToRil(in.gsm, out.GW_SignalStrength);
    convertToRil(in.wcdma, out.WCDMA_SignalStrength);
    convertToRil(in.cdma, out.CDMA_SignalStrength);
    convertToRil(in.evdo, out.EVDO_SignalStrength);
    convertToRil(in.lte, out.LTE_SignalStrength);
    convertToRil(in.tdscdma, out.TD_SCDMA_SignalStrength);
}

void convertHidlSignalStrengthToRil(const V1_6::SignalStrength& in,
        RIL_SignalStrength_v10& out) {
    convertToRil(in.gsm, out.GW_SignalStrength);
    convertToRil(in.wcdma, out.WCDMA_SignalStrength);
    convertToRil(in.cdma, out.CDMA_SignalStrength);
    convertToRil(in.evdo, out.EVDO_SignalStrength);
    convertToRil(in.lte, out.LTE_SignalStrength);
    convertToRil(in.tdscdma, out.TD_SCDMA_SignalStrength);
}

void convertHidlDataRegistrationStateResponseToRil_1_4(
        const V1_4::DataRegStateResult& hidlData,
        RIL_DataRegistrationStateResponse& rilData) {
    convertHidlDataRegistrationStateResponseToRil(hidlData.base, rilData);
}

void convertHidlDataRegistrationStateResponseToRil_1_5(
        const V1_5::RegStateResult& hidlData,
        RIL_DataRegistrationStateResponse& rilData) {
    rilData.regState = (RIL_RegState)hidlData.regState;
    rilData.rat = (RIL_RadioTechnology)hidlData.rat;
    // TODO: add other areas...currently UTF does not compare these items
}

void convertToRil(const hidl_string& imei, const hidl_string& imeisv, const hidl_string& esn,
        const hidl_string& meid, ril_request_device_identity_type_t* out) {
    strlcpy(out->imei, imei.c_str(), IMEI_MAX_STR_SIZE);
    strlcpy(out->imeisv, imeisv.c_str(), IMEISV_MAX_STR_SIZE);
    strlcpy(out->esn, esn.c_str(), ESN_MAX_STR_SIZE);
    strlcpy(out->meid, meid.c_str(), MEID_MAX_STR_SIZE);
}

void convertToRil(const hidl_vec<V1_4::EmergencyNumber>& in,
        ril_unsol_emergency_list_t *out) {
    out->ecc_list.emergency_list_length = in.size();
    for(int i=0; i< in.size(); i++) {
        out->ecc_list.emergency_numbers[i].category = in[i].categories;
        out->ecc_list.emergency_numbers[i].sources = in[i].sources;
        strlcpy(out->ecc_list.emergency_numbers[i].number,
                in[i].number.c_str(), MAX_EMERGENCY_NUMBER_LENGTH);
        strlcpy(out->ecc_list.emergency_numbers[i].mcc,
                in[i].mcc.c_str(), MAX_RIL_MCC_MNC_SIZE);
        strlcpy(out->ecc_list.emergency_numbers[i].mnc,
                in[i].mnc.c_str(), MAX_RIL_MCC_MNC_SIZE);
        out->ecc_list.emergency_numbers[i].no_of_urns = in[i].urns.size();
        for ( int j = 0; j < in[i].urns.size(); j++ ) {
            strlcpy(out->ecc_list.emergency_numbers[i].urn[j], in[i].urns[j].c_str(), MAX_URN_LENGTH);
        }
    }
}

RIL_CellInfoType getCellInfoType(V1_5::CellIdentity::hidl_discriminator discriminator) {
    switch (discriminator) {
    case V1_5::CellIdentity::hidl_discriminator::gsm:
        return RIL_CELL_INFO_TYPE_GSM;
    case V1_5::CellIdentity::hidl_discriminator::wcdma:
        return RIL_CELL_INFO_TYPE_WCDMA;
    case V1_5::CellIdentity::hidl_discriminator::tdscdma:
        return RIL_CELL_INFO_TYPE_TD_SCDMA;
    case V1_5::CellIdentity::hidl_discriminator::cdma:
        return RIL_CELL_INFO_TYPE_CDMA;
    case V1_5::CellIdentity::hidl_discriminator::lte:
        return RIL_CELL_INFO_TYPE_LTE;
    case V1_5::CellIdentity::hidl_discriminator::nr:
        return RIL_CELL_INFO_TYPE_NR;
    case V1_5::CellIdentity::hidl_discriminator::noinit:
    default:
        return RIL_CELL_INFO_TYPE_NONE;
    }
}

template<>
void convertHidlCellIdentityResponsetoRil(const V1_5::CellIdentity &cellIdentity,
        RIL_CellIdentity_v16 &rilCellIdentity) {
    auto discriminator = cellIdentity.getDiscriminator();
    rilCellIdentity.cellInfoType = getCellInfoType(discriminator);
    switch (rilCellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        fillCellIdentityGsm(cellIdentity.gsm(), rilCellIdentity.cellIdentityGsm);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        fillCellIdentityWcdma(cellIdentity.wcdma(), rilCellIdentity.cellIdentityWcdma);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        fillCellIdentityCdma(cellIdentity.cdma(), rilCellIdentity.cellIdentityCdma);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        fillCellIdentityLte(cellIdentity.lte(), rilCellIdentity.cellIdentityLte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        fillCellIdentityTdscdma(cellIdentity.tdscdma(), rilCellIdentity.cellIdentityTdscdma);
        break;
    case RIL_CELL_INFO_TYPE_NR:
        fillCellIdentityNr(cellIdentity.nr(), rilCellIdentity.cellIdentityNr);
        break;
    default:
        break;
    }
}


template<>
void convertHidlVoiceRegistrationStateResponseToRil(const V1_5::RegStateResult& hidlData,
        RIL_VoiceRegistrationStateResponse& rilData) {
    rilData.regState = (RIL_RegState)hidlData.regState;
    rilData.rat = (RIL_RadioTechnology)hidlData.rat;
    // TODO: add other areas...currently UTF does not compare these items
    convertHidlCellIdentityResponsetoRil(hidlData.cellIdentity, rilData.cellIdentity);
}

RIL_Errno convertFromHidl(
    const hidl_vec<V1_5::RadioAccessSpecifier>& in, RIL_SysSelChannels& out) {
  RIL_Errno error = RIL_E_GENERIC_FAILURE;
  out.specifiers_length = in.size();
  for (size_t i = 0; i < in.size(); ++i) {
    error =
      android::hardware::radio::utils::convertHidlRasToRilRas(out.specifiers[i], in[i]);
    if (error != RIL_E_SUCCESS) {
      QCRIL_LOG_ERROR("failed to convert hidl fields to ril fields - %d", error);
      return error;
    }
  }
  return RIL_E_SUCCESS;
}

hidl_vec<V1_5::RadioAccessSpecifier>
convertRadioAccessSpecifier(const uint32_t len, const RIL_RadioAccessSpecifier in[MAX_RADIO_ACCESS_NETWORKS]) {
  hidl_vec<V1_5::RadioAccessSpecifier> out;
  if (len) {
    out.resize(len);
    QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: len- %d", len);
    for (uint32_t i = 0; i < len && i < MAX_RADIO_ACCESS_NETWORKS; i++) {
      QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: ras - %d", in[i].radio_access_network);
      out[i].radioAccessNetwork = (V1_5::RadioAccessNetworks)in[i].radio_access_network;
      if (out[i].radioAccessNetwork == V1_5::RadioAccessNetworks::GERAN) {
        hidl_vec<V1_1::GeranBands> gbands;
        gbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          gbands[j] = static_cast<V1_1::GeranBands>(in[i].bands.geran_bands[j]);
        }
        out[i].bands.geranBands(std::move(gbands));
      } else if (out[i].radioAccessNetwork == V1_5::RadioAccessNetworks::UTRAN) {
        hidl_vec<V1_5::UtranBands> ubands;
        ubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          ubands[j] = static_cast<V1_5::UtranBands>(in[i].bands.utran_bands[j]);
        }
        out[i].bands.utranBands(std::move(ubands));
      } else if (out[i].radioAccessNetwork == V1_5::RadioAccessNetworks::EUTRAN) {
        hidl_vec<V1_5::EutranBands> eubands;
        eubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          eubands[j] = static_cast<V1_5::EutranBands>(in[i].bands.eutran_bands[j]);
        }
        out[i].bands.eutranBands(std::move(eubands));
      } else if (out[i].radioAccessNetwork == V1_5::RadioAccessNetworks::NGRAN) {
        QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: 5g");
        hidl_vec<V1_5::NgranBands> ngbands;
        ngbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: bands - %d", in[i].bands.ngran_bands[j]);
          ngbands[j] = static_cast<V1_5::NgranBands>(in[i].bands.ngran_bands[j]);
        }
        out[i].bands.ngranBands(std::move(ngbands));
      }
      out[i].channels.resize(in[i].channels_length);
      for (uint32_t j = 0; j < in[i].channels_length && i < MAX_CHANNELS; j++) {
        out[i].channels[j] = in[i].channels[j];
      }
    }
  }
  return out;
}

void convertCellInfoGsmToUtf(RIL_CellInfoGsm_v12 &out, const V1_5::CellInfoGsm& in) {
}

void convertCellInfoCdmaToUtf(RIL_CellInfoCdma &out, const V1_2::CellInfoCdma& in) {
}

void convertCellInfoWcdmaToUtf(RIL_CellInfoWcdma_v12 &out, const V1_5::CellInfoWcdma& in) {
}

void convertCellInfoTdscdmaToUtf(RIL_CellInfoTdscdma &out, const V1_5::CellInfoTdscdma& in) {
}

void convertCellInfoLteToUtf(RIL_CellInfoLte_v12 &out, const V1_5::CellInfoLte& in) {
}

void convertCellInfoNrToUtf(RIL_CellInfoNr &out, const V1_5::CellInfoNr& in) {
}

void convertRILCellInfoToUtf(RIL_CellInfo_v12 &out, const V1_5::CellInfo& in) {
//  out.cellInfoType = (RIL_CellInfoType) in.cellInfoType;

  out.registered = in.registered;
  out.timeStampType = (RIL_TimeStampType) in.timeStampType;
  out.timeStamp = in.timeStamp;
  out.connStatus = (RIL_CellConnectionStatus) in.connectionStatus;

  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    convertCellInfoGsmToUtf(out.CellInfo.gsm, in.ratSpecificInfo.gsm());
  }
  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_CDMA;
    convertCellInfoCdmaToUtf(out.CellInfo.cdma, in.ratSpecificInfo.cdma());
  }
  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    convertCellInfoWcdmaToUtf(out.CellInfo.wcdma, in.ratSpecificInfo.wcdma());
  }
  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_TD_SCDMA;
    convertCellInfoTdscdmaToUtf(out.CellInfo.tdscdma, in.ratSpecificInfo.tdscdma());
  }
  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_LTE;
    convertCellInfoLteToUtf(out.CellInfo.lte, in.ratSpecificInfo.lte());
  }
  if (in.ratSpecificInfo.getDiscriminator() == V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr) {
    out.cellInfoType = RIL_CELL_INFO_TYPE_NR;
    convertCellInfoNrToUtf(out.CellInfo.nr, in.ratSpecificInfo.nr());
  }
}

void convertNetworkScanRequestToHidl(const RIL_NetworkScanRequest &in,
                                     V1_5::NetworkScanRequest &out) {
  out.type = V1_1::ScanType::ONE_SHOT;
  if (in.type == RIL_ONE_SHOT) {
    out.type = V1_1::ScanType::ONE_SHOT;
  } else if (in.type == RIL_PERIODIC) {
    out.type = V1_1::ScanType::PERIODIC;
  }
  out.interval = in.interval;
  out.specifiers = convertRadioAccessSpecifier(in.specifiers_length, in.specifiers);
  out.maxSearchTime = in.maxSearchTime;
  out.incrementalResults = in.incrementalResults;
  out.incrementalResultsPeriodicity = in.incrementalResultsPeriodicity;
  out.mccMncs.resize(in.mccMncLength);
  for (uint32_t j = 0; j < in.mccMncLength && j < MAX_MCC_MNC_LIST_SIZE; j++) {
    out.mccMncs[j] = in.mccMncs[j];
  }
}
void convertNetworkScanResultToRil(const V1_5::NetworkScanResult &in, RIL_NetworkScanResult &out) {
  out.status = (RIL_ScanStatus)in.status;
  out.error = (RIL_Errno)in.error;
  out.network_infos_length = in.networkInfos.size();
  out.network_infos = (RIL_CellInfo_v12*)malloc(sizeof(RIL_CellInfo_v12)*out.network_infos_length);
  //qcril_malloc2(out.network_infos, out.network_infos_length);
  if (out.network_infos) {
    // TODO
  }
}
void convertNetworkScanResultToRil(const V1_6::NetworkScanResult &in, RIL_NetworkScanResult &out) {
  out.status = (RIL_ScanStatus)in.status;
  out.error = (RIL_Errno)in.error;
  out.network_infos_length = in.networkInfos.size();
  out.network_infos = (RIL_CellInfo_v12*)malloc(sizeof(RIL_CellInfo_v12)*out.network_infos_length);
  //qcril_malloc2(out.network_infos, out.network_infos_length);
  if (out.network_infos) {
    // TODO
  }
}
