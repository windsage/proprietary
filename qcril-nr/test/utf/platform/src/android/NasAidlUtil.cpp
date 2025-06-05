/*===========================================================================
 *
 *    Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "platform/android/NasAidlUtil.h"
#include "ril_utf_core.h"

std::vector<aidlnetwork::RadioAccessSpecifier> convertAidlRadioAccessSpecifier(
    const uint32_t len, const RIL_RadioAccessSpecifier in[MAX_RADIO_ACCESS_NETWORKS])
{
  using rasBands = aidlnetwork::RadioAccessSpecifierBands;
  std::vector<aidlnetwork::RadioAccessSpecifier> out;
  if (len) {
    out.resize(len);
    QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: len- %d\n", len);

    for (uint32_t i = 0; i < len && i < MAX_RADIO_ACCESS_NETWORKS; i++) {
      QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: RAS %d- RAT %d\n",i,in[i].radio_access_network);
      out[i].accessNetwork = convertRilNetworkTypeToAidl(in[i].radio_access_network);
      if (out[i].accessNetwork == aidlradio::AccessNetwork::GERAN) {
        std::vector<aidlnetwork::GeranBands> gbands;
        gbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          gbands[j] = static_cast<aidlnetwork::GeranBands>(in[i].bands.geran_bands[j]);
        }
        out[i].bands.set<rasBands::geranBands>(std::move(gbands));
     } else if (out[i].accessNetwork == aidlradio::AccessNetwork::UTRAN) {
        std::vector<aidlnetwork::UtranBands> ubands;
        ubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          ubands[j] = static_cast<aidlnetwork::UtranBands>(in[i].bands.utran_bands[j]);
        }
        out[i].bands.set<rasBands::utranBands>(std::move(ubands));
      } else if (out[i].accessNetwork == aidlradio::AccessNetwork::EUTRAN) {
        std::vector<aidlnetwork::EutranBands> eubands;
        eubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          eubands[j] = static_cast<aidlnetwork::EutranBands>(in[i].bands.eutran_bands[j]);
        }
        out[i].bands.set<rasBands::eutranBands>(std::move(eubands));
      } else if (out[i].accessNetwork == aidlradio::AccessNetwork::NGRAN) {
        std::vector<aidlnetwork::NgranBands> ngbands;
        ngbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          QCRIL_LOG_DEBUG("convertRadioAccessSpecifier: bands - %d\n", in[i].bands.ngran_bands[j]);
          ngbands[j] = static_cast<aidlnetwork::NgranBands>(in[i].bands.ngran_bands[j]);
        }
        out[i].bands.set<rasBands::ngranBands>(std::move(ngbands));
      }
      out[i].channels.resize(in[i].channels_length);
      for (uint32_t j = 0; j < in[i].channels_length && i < MAX_CHANNELS; j++) {
        out[i].channels[j] = in[i].channels[j];
      }
    }
  }
  return out;
}

std::vector<qtiradioaidl::RadioAccessSpecifier> convertAidlRadioAccessSpecifierQtiRadio(
    const uint32_t len, const RIL_RadioAccessSpecifier in[MAX_RADIO_ACCESS_NETWORKS])
{
  using rasBands = qtiradioaidl::RadioAccessSpecifierBands;
  std::vector<qtiradioaidl::RadioAccessSpecifier> out;
  if (len) {
    out.resize(len);
    QCRIL_LOG_DEBUG("convertAidlRadioAccessSpecifierQtiRadio: len- %d\n", len);

    for (uint32_t i = 0; i < len && i < MAX_RADIO_ACCESS_NETWORKS; i++) {
      QCRIL_LOG_DEBUG("convertAidlRadioAccessSpecifierQtiRadio: RAS %d- RAT %d\n",
             i,
             in[i].radio_access_network);
      out[i].accessNetwork = convertRilNetworkTypeToQtiRadioAidl(in[i].radio_access_network);
      if (out[i].accessNetwork == qtiradioaidl::AccessNetwork::GERAN) {
        std::vector<qtiradioaidl::GeranBands> gbands;
        gbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          gbands[j] = static_cast<qtiradioaidl::GeranBands>(in[i].bands.geran_bands[j]);
        }
        out[i].bands.set<rasBands::geranBands>(std::move(gbands));
      } else if (out[i].accessNetwork == qtiradioaidl::AccessNetwork::UTRAN) {
        std::vector<qtiradioaidl::UtranBands> ubands;
        ubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          ubands[j] = static_cast<qtiradioaidl::UtranBands>(in[i].bands.utran_bands[j]);
        }
        out[i].bands.set<rasBands::utranBands>(std::move(ubands));
      } else if (out[i].accessNetwork == qtiradioaidl::AccessNetwork::EUTRAN) {
        std::vector<qtiradioaidl::EutranBands> eubands;
        eubands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          eubands[j] = static_cast<qtiradioaidl::EutranBands>(in[i].bands.eutran_bands[j]);
        }
        out[i].bands.set<rasBands::eutranBands>(std::move(eubands));
      } else if (out[i].accessNetwork == qtiradioaidl::AccessNetwork::NGRAN) {
        std::vector<qtiradioaidl::NgranBands> ngbands;
        ngbands.resize(in[i].bands_length);
        for (uint32_t j = 0; j < in[i].bands_length && j < MAX_BANDS; j++) {
          QCRIL_LOG_DEBUG("convertAidlRadioAccessSpecifierQtiRadio: bands - %d\n",
                 in[i].bands.ngran_bands[j]);
          ngbands[j] = static_cast<qtiradioaidl::NgranBands>(in[i].bands.ngran_bands[j]);
        }
        out[i].bands.set<rasBands::ngranBands>(std::move(ngbands));
      }
      out[i].channels.resize(in[i].channels_length);
      for (uint32_t j = 0; j < in[i].channels_length && i < MAX_CHANNELS; j++) {
        out[i].channels[j] = in[i].channels[j];
      }
    }
  }
  return out;
}

aidlradio::AccessNetwork convertRilNetworkTypeToAidl(RIL_RadioAccessNetworks rilType) {
  switch(rilType) {
    case RIL_RadioAccessNetworks::GERAN:
      return aidlradio::AccessNetwork::GERAN;
    case RIL_RadioAccessNetworks::UTRAN:
      return aidlradio::AccessNetwork::UTRAN;
    case RIL_RadioAccessNetworks::EUTRAN:
      return aidlradio::AccessNetwork::EUTRAN;
    case RIL_RadioAccessNetworks::NGRAN:
      return aidlradio::AccessNetwork::NGRAN;
    case RIL_RadioAccessNetworks::CDMA:
      return aidlradio::AccessNetwork::CDMA2000;
    case RIL_RadioAccessNetworks::IWLAN:
      return aidlradio::AccessNetwork::IWLAN;

    case RIL_RadioAccessNetworks::UNKNOWN:
    default:
	return aidlradio::AccessNetwork::UNKNOWN;
  }
}

qtiradioaidl::AccessNetwork convertRilNetworkTypeToQtiRadioAidl(RIL_RadioAccessNetworks rilType)
{
  switch (rilType) {
    case RIL_RadioAccessNetworks::GERAN:
  return qtiradioaidl::AccessNetwork::GERAN;
    case RIL_RadioAccessNetworks::UTRAN:
  return qtiradioaidl::AccessNetwork::UTRAN;
    case RIL_RadioAccessNetworks::EUTRAN:
  return qtiradioaidl::AccessNetwork::EUTRAN;
    case RIL_RadioAccessNetworks::NGRAN:
  return qtiradioaidl::AccessNetwork::NGRAN;
    case RIL_RadioAccessNetworks::CDMA:
  return qtiradioaidl::AccessNetwork::CDMA2000;
    case RIL_RadioAccessNetworks::IWLAN:
  return qtiradioaidl::AccessNetwork::IWLAN;
    case RIL_RadioAccessNetworks::UNKNOWN:
    default:
  return qtiradioaidl::AccessNetwork::UNKNOWN;
  }
}

void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest &in,
                                     aidlnetwork::NetworkScanRequest &out) {
  out.type = aidlnetwork::NetworkScanRequest::SCAN_TYPE_ONE_SHOT;
  if (in.type == RIL_ONE_SHOT) {
    out.type = aidlnetwork::NetworkScanRequest::SCAN_TYPE_ONE_SHOT;
  } else if (in.type == RIL_PERIODIC) {
    out.type = aidlnetwork::NetworkScanRequest::SCAN_TYPE_PERIODIC;
  }
  out.interval = in.interval;
  out.specifiers = convertAidlRadioAccessSpecifier(in.specifiers_length, in.specifiers);
  out.maxSearchTime = in.maxSearchTime;
  out.incrementalResults = in.incrementalResults;
  out.incrementalResultsPeriodicity = in.incrementalResultsPeriodicity;
  out.mccMncs.resize(in.mccMncLength);
  for (uint32_t j = 0; j < in.mccMncLength && j < MAX_MCC_MNC_LIST_SIZE; j++) {
    out.mccMncs[j] = std::string(in.mccMncs[j]);
  }
}

void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest& in,
                                     qtiradioaidl::NetworkScanRequest& out)
{
  out.type = qtiradioaidl::NetworkScanRequest::SCAN_TYPE_ONE_SHOT;
  if (in.type == RIL_ONE_SHOT) {
    out.type = qtiradioaidl::NetworkScanRequest::SCAN_TYPE_ONE_SHOT;
  } else if (in.type == RIL_PERIODIC) {
    out.type = qtiradioaidl::NetworkScanRequest::SCAN_TYPE_PERIODIC;
  }
  out.interval = in.interval;
  out.specifiers = convertAidlRadioAccessSpecifierQtiRadio(in.specifiers_length, in.specifiers);
  out.maxSearchTime = in.maxSearchTime;
  out.incrementalResults = in.incrementalResults;
  out.incrementalResultsPeriodicity = in.incrementalResultsPeriodicity;
  out.mccMncs.resize(in.mccMncLength);
  for (uint32_t j = 0; j < in.mccMncLength && j < MAX_MCC_MNC_LIST_SIZE; j++) {
    out.mccMncs[j] = std::string(in.mccMncs[j]);
  }
}

qtiradioaidl::AccessMode convertAccessMode(const RIL_AccessMode& in)
{
  switch (in) {
    case RIL_ACCESS_MODE_PLMN:
  return qtiradioaidl::AccessMode::PLMN;
    case RIL_ACCESS_MODE_SNPN:
  return qtiradioaidl::AccessMode::SNPN;
    case RIL_ACCESS_MODE_INVALID:
    default:
  return qtiradioaidl::AccessMode::INVALID;
  }
}

qtiradioaidl::SearchType convertSearchType(const RIL_SearchType& in)
{
  switch (in) {
    case RIL_SEARCH_TYPE_PLMN_AND_CAG:
  return qtiradioaidl::SearchType::PLMN_AND_CAG;
    case RIL_SEARCH_TYPE_PLMN_ONLY:
  return qtiradioaidl::SearchType::PLMN_ONLY;
    case RIL_SEARCH_TYPE_INVALID:
    default:
  return qtiradioaidl::SearchType::INVALID;
  }
}
void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest& in,
                                     qtiradioaidl::QtiNetworkScanRequest& out)
{
  convertNetworkScanRequestToAidl(in, out.nsr);
  out.accessMode = convertAccessMode(in.accessMode);
  out.searchType = convertSearchType(in.searchType);
}

static void fillNetworkStatus(int32_t status, char* buf, size_t len) {
    switch (status) {
      case aidlnetwork::OperatorInfo::STATUS_UNKNOWN:
        strlcpy(buf, "unknown", len);
        break;
      case aidlnetwork::OperatorInfo::STATUS_AVAILABLE:
        strlcpy(buf, "available", len);
        break;
      case aidlnetwork::OperatorInfo::STATUS_CURRENT:
        strlcpy(buf, "current", len);
        break;
      case aidlnetwork::OperatorInfo::STATUS_FORBIDDEN:
      default:
        strlcpy(buf, "forbidden", len);
        break;
    }
}
ril_request_query_available_networks_resp_t* convertAidlNetworkscanResponseToRil(
    const std::vector<aidlnetwork::OperatorInfo>& networkInfos)
{
  // use malloc since framework will free it use free()
  auto ptr = (ril_request_query_available_networks_resp_t*)malloc(
      sizeof(ril_request_query_available_networks_resp_t));
  if (ptr == nullptr) return ptr;
  memset(ptr, 0, sizeof(*ptr));

  unsigned int nw_cnt = networkInfos.size();
  if (nw_cnt == 0) {
    return ptr;  // keep the same logic as copy_ril_request_query_available_networks_resp()
  }
  auto scanResp = (qcril_qmi_nw_scan_resp_helper_type*)malloc(
      nw_cnt * sizeof(qcril_qmi_nw_scan_resp_helper_type));
  if (scanResp == nullptr) {
    free(ptr);
    return nullptr;
  }
  memset(scanResp, 0, sizeof(*scanResp));

  unsigned int idx = 0;
  for (unsigned int item = 0; item < nw_cnt; item++) {
    ptr->nw_scan_info_array[idx++] = scanResp[item].long_eons;
    strlcpy(scanResp[item].long_eons,
            networkInfos[item].alphaLong.c_str(),
            sizeof(scanResp[item].long_eons));
    ptr->nw_scan_info_array[idx++] = scanResp[item].short_eons;
    strlcpy(scanResp[item].short_eons,
            networkInfos[item].alphaShort.c_str(),
            sizeof(scanResp[item].short_eons));
    ptr->nw_scan_info_array[idx++] = scanResp[item].mccmnc_info;
    strlcpy(scanResp[item].mccmnc_info,
            networkInfos[item].operatorNumeric.c_str(),
            sizeof(scanResp[item].mccmnc_info));
    ptr->nw_scan_info_array[idx++] = scanResp[item].network_status;
    fillNetworkStatus(networkInfos[item].status,
                      scanResp[item].network_status,
                      sizeof(scanResp[item].network_status));
  }
  return ptr;
}

void convertSignalStrengthNr(const aidlnetwork::NrSignalStrength& in, RIL_NR_SignalStrength& out)
{
}
void convertSignalStrengthNr(const qtiradioaidl::NrSignalStrength& in, RIL_NR_SignalStrength& out)
{
}
void convertCellIdentityNr(const aidlnetwork::CellIdentityNr& in, RIL_CellIdentityNr& out)
{
    if (!in.mcc.empty() && in.mcc.length() < MAX_RIL_MCC_MNC_SIZE) {
        strlcpy(out.mcc, in.mcc.c_str(), sizeof(out.mcc));
    }
    if (!in.mnc.empty() && in.mnc.length() < MAX_RIL_MCC_MNC_SIZE) {
        strlcpy(out.mnc, in.mnc.c_str(), sizeof(out.mnc));
    }
    out.nci = in.nci;
    out.pci = in.pci;
    out.tac = in.tac;
    out.nrarfcn = in.nrarfcn;
    // TODO
    // operatorNames
    // additionalPlmns
    // bands
}
void convertCellIdentityNr(const qtiradioaidl::CellIdentityNr& in, RIL_CellIdentityNr& out)
{
    if (!in.mcc.empty() && in.mcc.length() < MAX_RIL_MCC_MNC_SIZE) {
        strlcpy(out.mcc, in.mcc.c_str(), sizeof(out.mcc));
    }
    if (!in.mnc.empty() && in.mnc.length() < MAX_RIL_MCC_MNC_SIZE) {
        strlcpy(out.mnc, in.mnc.c_str(), sizeof(out.mnc));
    }
    out.nci = in.nci;
    out.pci = in.pci;
    out.tac = in.tac;
    out.nrarfcn = in.nrarfcn;
    // TODO
    // operatorNames
    // additionalPlmns
    // bands
}
void convertCagInfo(const qtiradioaidl::CagInfo& in, RIL_CagInfo& out)
{
    if (!in.cagName.empty() && in.cagName.length() < MAX_CAG_NAME_LEN) {
        strlcpy(out.cagName, in.cagName.c_str(), sizeof(out.cagName));
    }
    out.cagId = in.cagId;
    out.cagOnlyAccess = in.cagOnlyAccess;
    out.cagInAllowedList = in.cagInAllowedList;
}
void convertSnpnInfo(const qtiradioaidl::SnpnInfo& in, RIL_SnpnInfo& out)
{
    // TODO
}
void convertCellIdentityNr(const qtiradioaidl::QtiCellIdentityNr& in, RIL_CellIdentityNr& out)
{
    if (in.cNr) {
        convertCellIdentityNr(*(in.cNr), out);
    }
    if (in.snpnInfo) {
        out.snpn_info_valid = TRUE;
        convertSnpnInfo(*(in.snpnInfo), out.snpn_info);
    }
    if (in.cagInfo) {
        out.cag_info_valid = TRUE;
        convertCagInfo(*(in.cagInfo), out.cag_info);
    }
}
void convertCellInfoNr(const aidlnetwork::CellInfoNr& in, RIL_CellInfoNr& out)
{
    convertCellIdentityNr(in.cellIdentityNr, out.cellIdentityNr);
    convertSignalStrengthNr(in.signalStrengthNr, out.signalStrengthNr);
}
void convertCellInfoNr(const qtiradioaidl::QtiCellInfoNr& in, RIL_CellInfoNr& out)
{
    convertCellIdentityNr(in.cellIdentityNr, out.cellIdentityNr);
    convertSignalStrengthNr(in.signalStrengthNr, out.signalStrengthNr);
}

void convert(const int32_t& aidlBarringServiceType, Ril_BarringServiceType& rilBarringServiceType)
{
  if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_CS_SERVICE) {
     rilBarringServiceType = CS_SERVICE;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_PS_SERVICE) {
    rilBarringServiceType = PS_SERVICE;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_CS_VOICE) {
    rilBarringServiceType = CS_VOICE;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_MO_SIGNALLING) {
    rilBarringServiceType = MO_SIGNALLING;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_MO_DATA) {
    rilBarringServiceType = MO_DATA;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_CS_FALLBACK) {
    rilBarringServiceType = CS_FALLBACK;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_MMTEL_VOICE) {
    rilBarringServiceType = MMTEL_VOICE;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_MMTEL_VIDEO) {
    rilBarringServiceType = MMTEL_VIDEO;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_EMERGENCY) {
    rilBarringServiceType = EMERGENCY;
  } else if (aidlBarringServiceType == aidlnetwork::BarringInfo::SERVICE_TYPE_SMS) {
    rilBarringServiceType = BARRING_SMS;
  } else {
    rilBarringServiceType= static_cast<Ril_BarringServiceType>(aidlBarringServiceType);
  }
}

void convert(const int32_t& aidlBarringType, Ril_BarringType& rilBarringType) {
  if (aidlBarringType == aidlnetwork::BarringInfo::BARRING_TYPE_NONE) {
    rilBarringType = BARRING_NONE;
  } else if (aidlBarringType == aidlnetwork::BarringInfo::BARRING_TYPE_CONDITIONAL) {
    rilBarringType = CONDITIONAL;
  } else if (aidlBarringType == aidlnetwork::BarringInfo::BARRING_TYPE_UNCONDITIONAL) {
    rilBarringType = UNCONDITIONAL;
  }
}

void convert(const ::aidlnetwork::BarringInfo& aidlBarringInfo, RIL_BarringInfo& rilBarringInfo)
{
  convert(aidlBarringInfo.serviceType, rilBarringInfo.barringService);
  convert(aidlBarringInfo.barringType, rilBarringInfo.barringType);

  rilBarringInfo.typeSpecificInfo.barringFactor = aidlBarringInfo.barringTypeSpecificInfo->factor;
  rilBarringInfo.typeSpecificInfo.barringTimeSeconds = aidlBarringInfo.barringTypeSpecificInfo->timeSeconds;
  rilBarringInfo.typeSpecificInfo.isBarred = aidlBarringInfo.barringTypeSpecificInfo->isBarred;
}

void convertCellBarringInfoNr(const std::vector<::aidlnetwork::BarringInfo>& in, RIL_CellBarringInfo* out)
{
  for (size_t i = 0; i < in.size(); i++) {
    convert(in[i], out->barringInfo[i]);
  }
}

void convertNetworkInfo(const aidlnetwork::CellInfo& in, RIL_CellInfo_v12& out)
{
    out.registered = in.registered;
    out.connStatus = static_cast<RIL_CellConnectionStatus>(in.connectionStatus);  // TODO convert
    // TODO convert all CellInfos
    if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::gsm) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    } else if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::wcdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    } else if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::tdscdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_TD_SCDMA;
    } else if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::lte) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_LTE;
    } else if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::nr) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_NR;
        auto nrCellInfo = in.ratSpecificInfo.get<aidlnetwork::CellInfoRatSpecificInfo::nr>();
        convertCellInfoNr(nrCellInfo, out.CellInfo.nr);
    } else if (in.ratSpecificInfo.getTag() == aidlnetwork::CellInfoRatSpecificInfo::cdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_CDMA;
    }
}
void convertNetworkInfo(const qtiradioaidl::QtiCellInfo& in, RIL_CellInfo_v12& out)
{
    out.registered = in.registered;
    out.connStatus = static_cast<RIL_CellConnectionStatus>(in.connectionStatus);  // TODO convert
    // TODO convert all CellInfos
    if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::gsm) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    } else if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::wcdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    } else if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::tdscdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_TD_SCDMA;
    } else if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::lte) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_LTE;
    } else if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::nr) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_NR;
        auto nrCellInfo = in.ratSpecificInfo.get<qtiradioaidl::QtiCellInfoRatSpecificInfo::nr>();
        convertCellInfoNr(nrCellInfo, out.CellInfo.nr);
    } else if (in.ratSpecificInfo.getTag() == qtiradioaidl::QtiCellInfoRatSpecificInfo::cdma) {
        out.cellInfoType = RIL_CELL_INFO_TYPE_CDMA;
    }
}
void convertNetworkScanResultToRil(
  const ::aidl::android::hardware::radio::network::NetworkScanResult& in,
  RIL_NetworkScanResult &out) {
  out.status = (RIL_ScanStatus)in.status;
  out.error = (RIL_Errno)in.error;
  out.network_infos_length = in.networkInfos.size();
  if (out.network_infos_length) {
        qcril_malloc2(out.network_infos, out.network_infos_length);
        if (out.network_infos) {
        for (size_t i = 0; i < out.network_infos_length; i++) {
        convertNetworkInfo(in.networkInfos[i], out.network_infos[i]);
        }
        }
  }
}

void convertNetworkScanResultToRil(const qtiradioaidl::QtiNetworkScanResult& in,
                                   RIL_NetworkScanResult& out)
{
  out.status = (RIL_ScanStatus)in.status;
  out.error = (RIL_Errno)in.error;
  out.network_infos_length = in.networkInfos.size();
  if (out.network_infos_length) {
        qcril_malloc2(out.network_infos, out.network_infos_length);
        if (out.network_infos) {
        for (size_t i = 0; i < out.network_infos_length; i++) {
        convertNetworkInfo(in.networkInfos[i], out.network_infos[i]);
        }
        }
  }
}

RIL_Errno convertAidlUsageSettingtoRil(const aidlnetwork::UsageSetting& aidlUSetting,
                                       RIL_UsageSettingMode& rilUSetting)
{
  switch (aidlUSetting) {
    case aidlnetwork::UsageSetting::VOICE_CENTRIC:
      rilUSetting = RIL_UsageSettingMode::RIL_VOICE_CENTRIC;
      break;
    case aidlnetwork::UsageSetting::DATA_CENTRIC:
      rilUSetting = RIL_UsageSettingMode::RIL_DATA_CENTRIC;
      break;
    default:
      return RIL_E_INVALID_ARGUMENTS;
  }
  return RIL_E_SUCCESS;
}

bool convertMsimPreference(const Msim_Preference from, aidlqtiradioconfig::MsimPreference& to)
{
  switch (from) {
    case MSIM_PREFERENCE_DSDA:
      to = aidlqtiradioconfig::MsimPreference::DSDA;
      break;
    case MSIM_PREFERENCE_DSDS:
      to = aidlqtiradioconfig::MsimPreference::DSDS;
      break;
    default:
      return false;
  }

  return true;
}

void convertInternalSignalStrengthToRil(const aidlnetwork::GsmSignalStrength from,
                                        RIL_GW_SignalStrength& to)
{
  to.signalStrength = from.signalStrength;
  to.bitErrorRate = from.bitErrorRate;
  // TODO: Why does "timingAdvance" exist soley in AIDL?
}
void convertInternalSignalStrengthToRil(const aidlnetwork::WcdmaSignalStrength from,
                                        RIL_WCDMA_SignalStrength& to)
{
  to.valid = true;
  to.signalStrength = from.signalStrength;
  to.bitErrorRate = from.bitErrorRate;
  to.rscp = from.rscp;
  to.ecio = from.ecno;  // Why is this ecno in AIDL but ecio in RIL?
}
void convertInternalSignalStrengthToRil(const aidlnetwork::CdmaSignalStrength from,
                                        RIL_CDMA_SignalStrength& to)
{
  to.dbm = from.dbm;
  to.ecio = from.ecio;
}
void convertInternalSignalStrengthToRil(const aidlnetwork::EvdoSignalStrength from,
                                        RIL_EVDO_SignalStrength& to)
{
  to.dbm = from.dbm;
  to.ecio = from.ecio;
  to.signalNoiseRatio = from.signalNoiseRatio;
}
void convertInternalSignalStrengthToRil(const aidlnetwork::LteSignalStrength from,
                                        RIL_LTE_SignalStrength_v8& to)
{
  to.signalStrength = from.signalStrength;
  to.rsrp = from.rsrp;
  to.rsrq = from.rsrq;
  to.rssnr = from.rssnr;
  to.cqi = from.cqi;
  to.timingAdvance = from.timingAdvance;
  // Why does cqiTableIndex exist in AIDL but not RIL
}
void convertInternalSignalStrengthToRil(const aidlnetwork::TdscdmaSignalStrength from,
                                        RIL_TD_SCDMA_SignalStrength& to)
{
  to.signalStrength = from.signalStrength;
  to.bitErrorRate = from.bitErrorRate;
  to.rscp = from.rscp;
}
void convertInternalSignalStrengthToRil(const aidlnetwork::NrSignalStrength from,
                                        RIL_NR_SignalStrength& to)
{
  to.ssRsrp = from.ssRsrp;
  to.ssRsrq = from.ssRsrq;
  to.ssSinr = from.ssSinr;
  to.csiRsrp = from.csiRsrp;
  to.csiRsrq = from.csiRsrq;
  to.csiSinr = from.csiSinr;
  // Why does csiCqiTableIndex and csiCqiReport exist in only AIDL but not RIL?
}

void convertAidlStrengthToRil(const ::aidlnetwork::SignalStrength from, RIL_SignalStrength* to)
{
  convertInternalSignalStrengthToRil(from.gsm, to->GW_SignalStrength);
  convertInternalSignalStrengthToRil(from.cdma, to->CDMA_SignalStrength);
  convertInternalSignalStrengthToRil(from.evdo, to->EVDO_SignalStrength);
  convertInternalSignalStrengthToRil(from.lte, to->LTE_SignalStrength);
  convertInternalSignalStrengthToRil(from.tdscdma, to->TD_SCDMA_SignalStrength);
  convertInternalSignalStrengthToRil(from.wcdma, to->WCDMA_SignalStrength);
  convertInternalSignalStrengthToRil(from.nr, to->NR_SignalStrength);
}

qtiradioaidl::NrConfig convert_nr_config_to_aidl(RIL_NR_DISABLE_MODE mode)
{
  switch (mode) {
    case NR_DISABLE_MODE_NONE:
      return qtiradioaidl::NrConfig::NR_CONFIG_COMBINED_SA_NSA;
    case NR_DISABLE_MODE_SA:
      return qtiradioaidl::NrConfig::NR_CONFIG_NSA;
    case NR_DISABLE_MODE_NSA:
      return qtiradioaidl::NrConfig::NR_CONFIG_SA;
    default:
      return qtiradioaidl::NrConfig::NR_CONFIG_INVALID;
  }
}

RIL_NR_DISABLE_MODE convert_nr_config_to_ril(qtiradioaidl::NrConfig config)
{
  switch (config) {
    case qtiradioaidl::NrConfig::NR_CONFIG_COMBINED_SA_NSA:
      return NR_DISABLE_MODE_NONE;
    case qtiradioaidl::NrConfig::NR_CONFIG_NSA:
      return NR_DISABLE_MODE_SA;
    case qtiradioaidl::NrConfig::NR_CONFIG_SA:
      return NR_DISABLE_MODE_NSA;
    default:
      return NR_DISABLE_MODE_NONE;
  }
}
void convertRILDeviceStateToAidl(::aidlmodem::DeviceStateType &in_deviceStateType,
                                 const RIL_DeviceStateType &deviceState) {
  switch(deviceState) {
    case RIL_DST_POWER_SAVE_MODE:
      in_deviceStateType = ::aidlmodem::DeviceStateType::POWER_SAVE_MODE;
      break;
    case RIL_DST_CHARGING_STATE:
      in_deviceStateType = ::aidlmodem::DeviceStateType::CHARGING_STATE;
      break;
    case RIL_DST_LOW_DATA_EXPECTED:
      in_deviceStateType = ::aidlmodem::DeviceStateType::LOW_DATA_EXPECTED;
      break;
  }
}

bool convertToAidlRadioCapability(const RIL_RadioCapability& from, aidlmodem::RadioCapability& to)
{
  switch (from.phase) {
    case 0:
      to.phase = aidlmodem::RadioCapability::PHASE_CONFIGURED;
      break;
    case 1:
      to.phase = aidlmodem::RadioCapability::PHASE_START;
      break;
    case 2:
      to.phase = aidlmodem::RadioCapability::PHASE_APPLY;
      break;
    case 3:
      to.phase = aidlmodem::RadioCapability::PHASE_UNSOL_RSP;
      break;
    case 4:
      to.phase = aidlmodem::RadioCapability::PHASE_FINISH;
      break;
    default:
      return false;
  }

  switch (from.status) {
    case 0:
      to.status = aidlmodem::RadioCapability::STATUS_NONE;
      break;
    case 1:
      to.status = aidlmodem::RadioCapability::STATUS_SUCCESS;
      break;
    case 2:
      to.status = aidlmodem::RadioCapability::STATUS_FAIL;
      break;
    default:
      return false;
  }

  to.session = from.session;
  to.raf = from.rat;

  to.logicalModemUuid = std::string(from.logicalModemUuid);

  return true;
}

bool convertRadioCapabilityToRil(const aidlmodem::RadioCapability& from, RIL_RadioCapability& to)
{
  switch (from.phase) {
    case aidlmodem::RadioCapability::PHASE_CONFIGURED:
      to.phase = 0;
      break;
    case aidlmodem::RadioCapability::PHASE_START:
      to.phase = 1;
      break;
    case aidlmodem::RadioCapability::PHASE_APPLY:
      to.phase = 2;
      break;
    case aidlmodem::RadioCapability::PHASE_UNSOL_RSP:
      to.phase = 3;
      break;
    case aidlmodem::RadioCapability::PHASE_FINISH:
      to.phase = 4;
      break;
    default:
      return false;
  }

  switch (from.status) {
    case aidlmodem::RadioCapability::STATUS_NONE:
      to.status = 0;
      break;
    case aidlmodem::RadioCapability::STATUS_SUCCESS:
      to.status = 1;
      break;
    case aidlmodem::RadioCapability::STATUS_FAIL:
      to.status = 2;
      break;
    default:
      return false;
  }

  to.session = from.session;
  to.rat = from.raf;

  if (from.logicalModemUuid.size() >= sizeof(to.logicalModemUuid)) {
    return false;
  }
  strlcpy(to.logicalModemUuid, from.logicalModemUuid.c_str(), sizeof(to.logicalModemUuid));

  return true;
}

void convertToRil(const std::string& imei, const std::string& imeisv, const std::string& esn,
  const std::string& meid, ril_request_device_identity_type_t* out) {
  strlcpy(out->imei, imei.c_str(), IMEI_MAX_STR_SIZE);
  strlcpy(out->imeisv, imeisv.c_str(), IMEISV_MAX_STR_SIZE);
  strlcpy(out->esn, esn.c_str(), ESN_MAX_STR_SIZE);
  strlcpy(out->meid, meid.c_str(), MEID_MAX_STR_SIZE);
}

void convertAidlActivityStatsToRil(const aidlmodem::ActivityStatsInfo& from, RIL_ActivityStatsInfo& to) {
  to.sleep_mode_time_ms = from.sleepModeTimeMs;
  to.idle_mode_time_ms = from.idleModeTimeMs;
  if(from.techSpecificInfo.size() > 0) {
    size_t iterations = MIN(RIL_NUM_TX_POWER_LEVELS, from.techSpecificInfo[0].txmModetimeMs.size());
    to.rx_mode_time_ms = from.techSpecificInfo[0].rxModeTimeMs;
    for(size_t i=0;i<iterations;++i) {
      to.tx_mode_time_ms[i] = from.techSpecificInfo[0].txmModetimeMs[i];
    }
  }
}

