/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qti_radio_stable_aidl_service_utils.h"

#undef TAG
#define TAG "RILQ"

extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace utils {

#ifndef QMI_RIL_UTF
NrIconType convert_nrIconType_from_response(std::shared_ptr<rildata::NrIconType_t> resp) {
  if (resp->isNone()) {
    return NrIconType::TYPE_NONE;
  } else if (resp->isBasic()) {
    return NrIconType::TYPE_5G_BASIC;
  } else if (resp->isUwb()) {
    return NrIconType::TYPE_5G_UWB;
  } else {
    return NrIconType::INVALID;
  }
}

NrIconType convert_nrIconType_from_indication(std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg) {
  if (msg->isNone()) {
    return NrIconType::TYPE_NONE;
  } else if (msg->isBasic()) {
    return NrIconType::TYPE_5G_BASIC;
  } else if (msg->isUwb()) {
    return NrIconType::TYPE_5G_UWB;
  } else {
    return NrIconType::INVALID;
  }
}

rildata::DdsSwitchUserSelection_t convertUserPeferenceForDataDuringVoiceCall(bool userPreference) {
  if(userPreference) {
    return rildata::DdsSwitchUserSelection_t::ENABLE_DATA_DURING_VOICE;
  } else {
    return rildata::DdsSwitchUserSelection_t::DISABLE_DATA_DURING_VOICE;
  }
}

rildata::SubscriptionId_t convertQcRilInstanceId(qcril_instance_id_e_type instanceId) {
  switch(instanceId) {
    case qcril_instance_id_e_type::QCRIL_DEFAULT_INSTANCE_ID:
      return rildata::SubscriptionId_t::PRIMARY_SUBSCRIPTION_ID;
    case qcril_instance_id_e_type::QCRIL_SECOND_INSTANCE_ID:
      return rildata::SubscriptionId_t::SECONDARY_SUBSCRIPTION_ID;
    case qcril_instance_id_e_type::QCRIL_THIRD_INSTANCE_ID:
      return rildata::SubscriptionId_t::TERTIARY_SUBSCRIPTION_ID;
    default:
      return rildata::SubscriptionId_t::INVALID_SUBSCRIPTION_ID;
  }
}

bool convertSmartDdsSwitchCapabilityFromResponse(
        std::shared_ptr<rildata::SmartDdsSwitchCapability_t> caps) {
  if (caps && *caps == rildata::SmartDdsSwitchCapability_t::SMART_DDS_SWITCH_CAPABILITY_SUPPORTED) {
    return true;
  } else {
    return false;
  }
}

rildata::NrUCIconMode_t convertNrUCBandMode(const aidlimports::NrUwbIconMode &in) {
 switch(in) {
   case aidlimports::NrUwbIconMode::NONE:
     return rildata::NrUCIconMode_t::MODE_NONE;
   case aidlimports::NrUwbIconMode::CONNECTED:
     return rildata::NrUCIconMode_t::MODE_CONNECTED;
   case aidlimports::NrUwbIconMode::IDLE:
     return rildata::NrUCIconMode_t::MODE_IDLE;
   case aidlimports::NrUwbIconMode::CONNECTED_AND_IDLE:
     return rildata::NrUCIconMode_t::MODE_CONNECTED_AND_IDLE;
   default:
     return rildata::NrUCIconMode_t::MODE_NONE;
 }
}

void convertNrUCIconBandInfo(rildata::NrUCIconBandInfo_t& out,
    const aidlimports::NrUwbIconBandInfo &in) {
  out.enabled = in.enabled;
  out.mode = convertNrUCBandMode(in.mode);
  uint32_t count = in.bands.size();
  out.band.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    out.band[i] = in.bands[i];
  }
}

void convertNrUCIconBandwidthInfo(rildata::NrUCIconBandwidthInfo_t& out,
    const aidlimports::NrUwbIconBandwidthInfo& in) {
  out.enabled = in.enabled;
  out.mode = convertNrUCBandMode(in.mode);
  out.bandwidth = in.bandwidth;
}

rildata::NrUCIconRefreshTimer_t convertNrUCIconRefreshTimer(
    const aidlimports::NrUwbIconRefreshTimerType &in) {
 switch(in) {
   case aidlimports::NrUwbIconRefreshTimerType::SCG_TO_MCG:
     return rildata::NrUCIconRefreshTimer_t::SCG_TO_MCG_TIMER;
   case aidlimports::NrUwbIconRefreshTimerType::IDLE_TO_CONNECT:
     return rildata::NrUCIconRefreshTimer_t::IDLE_TO_CONNECT_TIMER;
   case aidlimports::NrUwbIconRefreshTimerType::IDLE:
     return rildata::NrUCIconRefreshTimer_t::IDLE_TIMER;
   default:
     return rildata::NrUCIconRefreshTimer_t::SCG_TO_MCG_TIMER;
 }
}

void convertNrUCIconRefreshTime(rildata::NrUCIconRefreshTime_t& out,
    const aidlimports::NrUwbIconRefreshTime& in) {
  out.timerType = convertNrUCIconRefreshTimer(in.timerType);
  out.timeValue = in.timeValue;
}
#endif

EndcStatus convert_endc_status(
    std::shared_ptr<qcril::interfaces::RilQueryEndcStatusResult_t> payload) {
  return (payload->status == ENDC_STATUS_ENABLED) ? EndcStatus::ENABLED : EndcStatus::DISABLED;
}

NrConfig convert_nr_disable_mode(RIL_NR_DISABLE_MODE mode) {
  switch (mode) {
    case NR_DISABLE_MODE_NONE:
      return NrConfig::NR_CONFIG_COMBINED_SA_NSA;
    case NR_DISABLE_MODE_SA:
      return NrConfig::NR_CONFIG_NSA;
    case NR_DISABLE_MODE_NSA:
      return NrConfig::NR_CONFIG_SA;
    default:
      return NrConfig::NR_CONFIG_INVALID;
  }
}

RIL_NR_DISABLE_MODE convert_nr_config(NrConfig config) {
  switch (config) {
    case NrConfig::NR_CONFIG_COMBINED_SA_NSA:
      return NR_DISABLE_MODE_NONE;
    case NrConfig::NR_CONFIG_NSA:
      return NR_DISABLE_MODE_SA;
    case NrConfig::NR_CONFIG_SA:
      return NR_DISABLE_MODE_NSA;
    default:
      return NR_DISABLE_MODE_NONE;
  }
}

property_id_type getPropertyId(std::string prop) {
  static std::unordered_map<std::string, property_id_type> propertyMap {
    {"persist.vendor.radio.poweron_opt", PERSIST_VENDOR_RADIO_POWERON_OPT},
    {"persist.vendor.radio.apm_sim_not_pwdn", PERSIST_VENDOR_RADIO_APM_SIM_NOT_PWDN},
    {"persist.vendor.radio.enableadvancedscan", PERSIST_VENDOR_RADIO_ENABLEADVANCEDSCAN},
  };

  auto searchResult = propertyMap.find(prop);
  if (searchResult != propertyMap.end()) {
    return searchResult->second;
  }
  return PROPERTY_ID_MAX;
}

bool convertCallForwardInfo(aidlimports::CallForwardInfo& out,
                            const qcril::interfaces::CallForwardInfo& in) {
  if (in.hasStatus()) {
    out.status = in.getStatus();
  }
  if (in.hasReason()) {
    out.reason = in.getReason();
  }
  if (in.hasServiceClass()) {
    out.serviceClass = in.getServiceClass();
  }
  if (in.hasToa()) {
    out.toa = in.getToa();
  }
  if (in.hasNumber() && !in.getNumber().empty()) {
    out.number = in.getNumber().c_str();
  }
  if (in.hasTimeSeconds()) {
    out.timeSeconds = in.getTimeSeconds();
  }
  return true;
}

bool convertCallForwardInfoList(std::vector<aidlimports::CallForwardInfo>& out,
                                const std::vector<qcril::interfaces::CallForwardInfo>& in) {
  uint32_t count = in.size();
  out.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    convertCallForwardInfo(out[i], in[i]);
  }
  return true;
}

qcril::interfaces::FacilityType convertFacilityType(std::string in) {
  if (in == "AO") {
    return qcril::interfaces::FacilityType::BAOC;
  } else if (in == "OI") {
    return qcril::interfaces::FacilityType::BAOIC;
  } else if (in == "OX") {
    return qcril::interfaces::FacilityType::BAOICxH;
  } else if (in == "AI") {
    return qcril::interfaces::FacilityType::BAIC;
  } else if (in == "IR") {
    return qcril::interfaces::FacilityType::BAICr;
  } else if (in == "AB") {
    return qcril::interfaces::FacilityType::BA_ALL;
  } else if (in == "AG") {
    return qcril::interfaces::FacilityType::BA_MO;
  } else if (in == "AC") {
    return qcril::interfaces::FacilityType::BA_MT;
  }
  return qcril::interfaces::FacilityType::UNKNOWN;
}

void convert_imei_info(aidlimports::ImeiInfo& out,
    const std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t>& in) {
  out.imei = in->getImei();
  if (in->getImeiType() == qcril::interfaces::ImeiType::IMEI_TYPE_PRIMARY) {
    out.type = aidlimports::ImeiType::IMEI_TYPE_PRIMARY;
  } else if (in->getImeiType() == qcril::interfaces::ImeiType::IMEI_TYPE_SECONDARY) {
    out.type = aidlimports::ImeiType::IMEI_TYPE_SECONDARY;
  } else {
    out.type = aidlimports::ImeiType::IMEI_TYPE_UNKNOWN;
  }
}

McfgRefreshState convertPdcRefreshToAidlMcfgRefresh(qcril::interfaces::PdcRefreshEvent event) {
    switch (event) {
    case qcril::interfaces::PdcRefreshEvent::REFRESH_START:
      return aidlimports::McfgRefreshState::START;
    case qcril::interfaces::PdcRefreshEvent::REFRESH_COMPLETE:
      return aidlimports::McfgRefreshState::COMPLETE;
    case qcril::interfaces::PdcRefreshEvent::CLIENT_REFRESH:
      return aidlimports::McfgRefreshState::CLIENT_REFRESH;
    default:
      return aidlimports::McfgRefreshState::START;
  }
  return aidlimports::McfgRefreshState::START;
}

RIL_Errno fillNetworkScanRequest(const aidlimports::QtiNetworkScanRequest& request,
                                 RIL_NetworkScanRequest& scanRequest) {
  QCRIL_LOG_INFO("fillNetworkScanRequest");
  RIL_Errno error = rasArgsCheck(request.nsr.specifiers);
  if (error != RIL_E_SUCCESS) {
    QCRIL_LOG_ERROR("RAS Args check failed - %d", error);
    return error;
  }

  if (request.nsr.type == aidlimports::NetworkScanRequest::SCAN_TYPE_ONE_SHOT) {
    scanRequest.type = RIL_ONE_SHOT;
  } else if (request.nsr.type == aidlimports::NetworkScanRequest::SCAN_TYPE_PERIODIC) {
    scanRequest.type = RIL_PERIODIC;
  }
  scanRequest.interval = request.nsr.interval;
  scanRequest.maxSearchTime = request.nsr.maxSearchTime;
  scanRequest.incrementalResults = request.nsr.incrementalResults;
  scanRequest.incrementalResultsPeriodicity = request.nsr.incrementalResultsPeriodicity;
  scanRequest.mccMncLength = request.nsr.mccMncs.size();
  scanRequest.specifiers_length = request.nsr.specifiers.size();

  RIL_AccessMode accessMode = convertAccessMode(request.accessMode);
  scanRequest.accessMode = accessMode;

  RIL_SearchType searchType = convertSearchType(request.searchType);
  scanRequest.searchType = searchType;

  for (size_t i = 0; i < request.nsr.specifiers.size(); ++i) {
    error = convertAidlRasToRilRas(scanRequest.specifiers[i], request.nsr.specifiers[i]);
    if (error != RIL_E_SUCCESS) {
      QCRIL_LOG_ERROR("failed to convert hidl fields to ril fields - %d", error);
      return error;
    }
  }

  for (size_t i = 0; i < request.nsr.mccMncs.size(); ++i) {
    strlcpy(scanRequest.mccMncs[i], request.nsr.mccMncs[i].c_str(), MAX_MCC_MNC_LEN + 1);
  }

  return RIL_E_SUCCESS;
}

RIL_AccessMode convertAccessMode(aidlimports::AccessMode accessMode) {
  switch(accessMode) {
    case aidlimports::AccessMode::PLMN:
      return RIL_ACCESS_MODE_PLMN;
    case aidlimports::AccessMode::SNPN:
      return RIL_ACCESS_MODE_SNPN;
    default:
      return RIL_ACCESS_MODE_INVALID;
  }
}

RIL_SearchType convertSearchType(aidlimports::SearchType searchType) {
  switch(searchType) {
    case aidlimports::SearchType::PLMN_AND_CAG:
      return RIL_SEARCH_TYPE_PLMN_AND_CAG;
    case aidlimports::SearchType::PLMN_ONLY:
      return RIL_SEARCH_TYPE_PLMN_ONLY;
    default:
      return RIL_SEARCH_TYPE_INVALID;
  }
}

aidlimports::AccessMode convertAccessModeFromRil(RIL_AccessMode ril_access_mode) {
  switch(ril_access_mode) {
    case RIL_ACCESS_MODE_PLMN:
      return aidlimports::AccessMode::PLMN;
    case RIL_ACCESS_MODE_SNPN:
      return aidlimports::AccessMode::SNPN;
    case RIL_ACCESS_MODE_INVALID:
      return aidlimports::AccessMode::INVALID;
    default:
      return aidlimports::AccessMode::INVALID;
  }

}

aidlimports::SignalQuality convertSignalQualityFromRil(RIL_SignalQuality ril_signal_quality) {
  switch(ril_signal_quality) {
    case RIL_SIGNAL_QUALITY_LOW:
      return aidlimports::SignalQuality::SIGNAL_QUALITY_LOW;
    case RIL_SIGNAL_QUALITY_HIGH:
      return aidlimports::SignalQuality::SIGNAL_QUALITY_HIGH;
    case RIL_SIGNAL_QUALITY_UNKNOWN:
      return aidlimports::SignalQuality::SIGNAL_QUALITY_INVALID;
    default:
      return aidlimports::SignalQuality::SIGNAL_QUALITY_INVALID;
  }

}

RIL_Errno rasArgsCheck(const std::vector<aidlimports::RadioAccessSpecifier>& specifiers) {
  using rasBands = aidlimports::RadioAccessSpecifierBands;

  if (specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
    QCRIL_LOG_ERROR("MAX RANs exceeded");
    return RIL_E_INVALID_ARGUMENTS;
  }

  for (size_t i = 0; i < specifiers.size(); ++i) {
    if (specifiers[i].channels.size() > MAX_CHANNELS) {
      QCRIL_LOG_ERROR("MAX channels exceeded");
      return RIL_E_INVALID_ARGUMENTS;
    }

    if (specifiers[i].bands.getTag() == rasBands::noinit) {
      QCRIL_LOG_ERROR("Bands not initialized");
      return RIL_E_INVALID_ARGUMENTS;
    }

    if ((specifiers[i].bands.getTag() == rasBands::geranBands) &&
        (specifiers[i].bands.get<rasBands::geranBands>().size() > MAX_BANDS)) {
          QCRIL_LOG_ERROR("geranBands: MAX bands exceeded");
          return RIL_E_INVALID_ARGUMENTS;
    } else if ((specifiers[i].bands.getTag() == rasBands::utranBands) &&
        (specifiers[i].bands.get<rasBands::utranBands>().size() > MAX_BANDS)) {
          QCRIL_LOG_ERROR("utranBands: MAX bands exceeded");
          return RIL_E_INVALID_ARGUMENTS;
    } else if ((specifiers[i].bands.getTag() == rasBands::eutranBands) &&
        (specifiers[i].bands.get<rasBands::eutranBands>().size() > MAX_BANDS)) {
          QCRIL_LOG_ERROR("eutranBands: MAX bands exceeded");
          return RIL_E_INVALID_ARGUMENTS;
    } else if ((specifiers[i].bands.getTag() == rasBands::ngranBands) &&
        (specifiers[i].bands.get<rasBands::ngranBands>().size() > MAX_BANDS)) {
          QCRIL_LOG_ERROR("ngranBands: MAX bands exceeded");
          return RIL_E_INVALID_ARGUMENTS;
    }
  }
  return RIL_E_SUCCESS;
}

RIL_Errno convertAidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
                                 const aidlimports::RadioAccessSpecifier& ras_from) {
  QCRIL_LOG_INFO("convertAidlRasToRilRas");
  using rasBands = aidlimports::RadioAccessSpecifierBands;
  ras_to.radio_access_network =
      convertAidlRanToRilRan(ras_from.accessNetwork);
  ras_to.channels_length = ras_from.channels.size();

  std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
  switch (ras_from.accessNetwork) {
    case aidlimports::AccessNetwork::GERAN:
      if (ras_from.bands.getTag() == rasBands::geranBands) {
        auto vec = ras_from.bands.get<rasBands::geranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.geran_bands[idx] =
              static_cast<RIL_GeranBands>(vec[idx]);
        }
      }
      break;
    case aidlimports::AccessNetwork::UTRAN:
      if (ras_from.bands.getTag() == rasBands::utranBands) {
        auto vec = ras_from.bands.get<rasBands::utranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.utran_bands[idx] =
              static_cast<RIL_UtranBands>(vec[idx]);
        }
      }
      break;
    case aidlimports::AccessNetwork::EUTRAN:
      if (ras_from.bands.getTag() == rasBands::eutranBands) {
        auto vec = ras_from.bands.get<rasBands::eutranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.eutran_bands[idx] =
              static_cast<RIL_EutranBands>(vec[idx]);
        }
      }
      break;
    case aidlimports::AccessNetwork::NGRAN:
      if (ras_from.bands.getTag() == rasBands::ngranBands) {
        auto vec = ras_from.bands.get<rasBands::ngranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.ngran_bands[idx] =
              static_cast<RIL_NgranBands>(vec[idx]);
        }
      }
      break;
    default:
      return RIL_E_INVALID_ARGUMENTS;
  }
  return RIL_E_SUCCESS;
}

RIL_RadioAccessNetworks convertAidlRanToRilRan(aidlimports::AccessNetwork ran) {
  switch (ran) {
    case aidlimports::AccessNetwork::GERAN:
      return RIL_RadioAccessNetworks::GERAN;
    case aidlimports::AccessNetwork::UTRAN:
      return RIL_RadioAccessNetworks::UTRAN;
    case aidlimports::AccessNetwork::EUTRAN:
      return RIL_RadioAccessNetworks::EUTRAN;
    case aidlimports::AccessNetwork::NGRAN:
      return RIL_RadioAccessNetworks::NGRAN;
    case aidlimports::AccessNetwork::CDMA2000:
      return RIL_RadioAccessNetworks::CDMA;
    default:
      return RIL_RadioAccessNetworks::UNKNOWN;
  }
}

void convert(const std::vector<RIL_CellInfo_v12>& rilCellInfos,
    std::vector<aidlimports::QtiCellInfo>& aidlCellInfos) {
  size_t num = rilCellInfos.size();
  QCRIL_LOG_INFO("convert ril_infos to aidl_infos : ril_infos nw count : %d", num);
  aidlCellInfos.resize(num);
  for (unsigned int i = 0; i < num; i++) {
    convert(rilCellInfos[i], aidlCellInfos[i]);
  }
}

void convert(const RIL_CellInfo_v12& rilCellInfo, aidlimports::QtiCellInfo& aidlCellInfo) {
  aidlCellInfo.registered = rilCellInfo.registered;

  aidlimports::CellConnectionStatus connectionStatus {};
  connectionStatus = static_cast<aidlimports::CellConnectionStatus>(rilCellInfo.connStatus);
  aidlCellInfo.connectionStatus = connectionStatus;

  QCRIL_LOG_INFO("rilCellInfo.cellInfoType : %d", rilCellInfo.cellInfoType);
  switch (rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      aidlimports::CellInfoGsm aidlGsmCellInfo {};
      convert(rilCellInfo.CellInfo.gsm, aidlGsmCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::gsm>(
          aidlGsmCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      aidlimports::CellInfoWcdma aidlWcdmaCellInfo {};
      convert(rilCellInfo.CellInfo.wcdma, aidlWcdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::wcdma>(
          aidlWcdmaCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      aidlimports::CellInfoCdma aidlCdmaCellInfo {};
      convert(rilCellInfo.CellInfo.cdma, aidlCdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::cdma>(
          aidlCdmaCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      aidlimports::CellInfoLte aidlLteCellInfo {};
      convert(rilCellInfo.CellInfo.lte, aidlLteCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::lte>(
          aidlLteCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      QCRIL_LOG_INFO("cellInfoTypeNR enter: ");
      aidlimports::QtiCellInfoNr aidlNrCellInfo {};
      convert(rilCellInfo.CellInfo.nr, aidlNrCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::nr>(
          aidlNrCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      aidlimports::CellInfoTdscdma aidlTdscdmaCellInfo {};
      convert(rilCellInfo.CellInfo.tdscdma, aidlTdscdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlimports::QtiCellInfoRatSpecificInfo::Tag::tdscdma>(
          aidlTdscdmaCellInfo);
      break;
    }

    default: {
      break;
    }
  }
}

/**************************************** GSM  *****************************************/
void convert(const RIL_CellInfoGsm_v12& rilGsmCellInfo,
    aidlimports::CellInfoGsm& aidlGsmCellInfo) {
  convert(rilGsmCellInfo.cellIdentityGsm, aidlGsmCellInfo.cellIdentityGsm);
  convert(rilGsmCellInfo.signalStrengthGsm, aidlGsmCellInfo.signalStrengthGsm);
}

void convert(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
    aidlimports::CellIdentityGsm& aidlGsmCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlGsmCellIdentity.mcc,
      aidlGsmCellIdentity.mnc, rilGsmCellIdentity);

  aidlGsmCellIdentity.lac = rilGsmCellIdentity.lac;
  aidlGsmCellIdentity.cid = rilGsmCellIdentity.cid;
  aidlGsmCellIdentity.arfcn = rilGsmCellIdentity.arfcn;
  aidlGsmCellIdentity.bsic = rilGsmCellIdentity.bsic;

  convert(rilGsmCellIdentity, aidlGsmCellIdentity.operatorNames);
}

void convert(const RIL_GSM_SignalStrength_v12& rilGsmSignalStrength,
    aidlimports::GsmSignalStrength& aidlGsmSignalStrength) {
  aidlGsmSignalStrength.signalStrength = rilGsmSignalStrength.signalStrength;
  aidlGsmSignalStrength.bitErrorRate = rilGsmSignalStrength.bitErrorRate;
  aidlGsmSignalStrength.timingAdvance = rilGsmSignalStrength.timingAdvance;
}

/**************************************** GSM  *****************************************/

/*************************************** WCDMA *****************************************/
void convert(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
    aidlimports::CellIdentityWcdma& aidlWcdmaCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlWcdmaCellIdentity.mcc,
      aidlWcdmaCellIdentity.mnc, rilWcdmaCellIdentity);

  aidlWcdmaCellIdentity.lac = rilWcdmaCellIdentity.lac;
  aidlWcdmaCellIdentity.cid = rilWcdmaCellIdentity.cid;
  aidlWcdmaCellIdentity.psc = rilWcdmaCellIdentity.psc;
  aidlWcdmaCellIdentity.uarfcn = rilWcdmaCellIdentity.uarfcn;

  convert(rilWcdmaCellIdentity, aidlWcdmaCellIdentity.operatorNames);
}

void convert(const RIL_SignalStrengthWcdma& rilWcdmaSignalStrength,
    aidlimports::WcdmaSignalStrength& aidlWcdmaSignalStrength) {
  aidlWcdmaSignalStrength.signalStrength = rilWcdmaSignalStrength.signalStrength;
  aidlWcdmaSignalStrength.bitErrorRate = rilWcdmaSignalStrength.bitErrorRate;
}

void convert(const RIL_CellInfoWcdma_v12& rilWcdmaCellInfo,
    aidlimports::CellInfoWcdma& aidlWcdmaCellInfo) {
  convert(rilWcdmaCellInfo.cellIdentityWcdma, aidlWcdmaCellInfo.cellIdentityWcdma);
  convert(rilWcdmaCellInfo.signalStrengthWcdma, aidlWcdmaCellInfo.signalStrengthWcdma);
}
/*************************************** WCDMA *****************************************/


/*************************************** CDMA ******************************************/
void convert(const RIL_CellIdentityCdma& rilCdmaCellIdentity,
    aidlimports::CellIdentityCdma& aidlCdmaCellIdentity) {
  aidlCdmaCellIdentity.networkId = rilCdmaCellIdentity.networkId;
  aidlCdmaCellIdentity.systemId = rilCdmaCellIdentity.systemId;
  aidlCdmaCellIdentity.baseStationId = rilCdmaCellIdentity.basestationId;
  aidlCdmaCellIdentity.longitude = rilCdmaCellIdentity.longitude;
  aidlCdmaCellIdentity.latitude = rilCdmaCellIdentity.latitude;

  convert(rilCdmaCellIdentity, aidlCdmaCellIdentity.operatorNames);
}

void convert(const RIL_CDMA_SignalStrength& rilCdmaSignalStrength,
    aidlimports::CdmaSignalStrength& aidlCdmaSignalStrength) {
  aidlCdmaSignalStrength.dbm = rilCdmaSignalStrength.dbm;
  aidlCdmaSignalStrength.ecio = rilCdmaSignalStrength.ecio;
}

void convert(const RIL_EVDO_SignalStrength& rilEvdoSignalStrength,
    aidlimports::EvdoSignalStrength& aidlEvdoSignalStrength) {
  aidlEvdoSignalStrength.dbm = rilEvdoSignalStrength.dbm;
  aidlEvdoSignalStrength.ecio = rilEvdoSignalStrength.ecio;
  aidlEvdoSignalStrength.signalNoiseRatio = rilEvdoSignalStrength.signalNoiseRatio;
}

void convert(const RIL_CellInfoCdma& rilCdmaCellInfo,
    aidlimports::CellInfoCdma& aidlCdmaCellInfo) {
  convert(rilCdmaCellInfo.cellIdentityCdma, aidlCdmaCellInfo.cellIdentityCdma);
  convert(rilCdmaCellInfo.signalStrengthCdma, aidlCdmaCellInfo.signalStrengthCdma);
  convert(rilCdmaCellInfo.signalStrengthEvdo, aidlCdmaCellInfo.signalStrengthEvdo);
}
/*************************************** CDMA ******************************************/


/*************************************** TDSCDMA ***************************************/
void convert(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
    aidlimports::CellIdentityTdscdma& aidlTdscdmaCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlTdscdmaCellIdentity.mcc,
      aidlTdscdmaCellIdentity.mnc, rilTdscdmaCellIdentity);

  aidlTdscdmaCellIdentity.lac = rilTdscdmaCellIdentity.lac;
  aidlTdscdmaCellIdentity.cid = rilTdscdmaCellIdentity.cid;
  aidlTdscdmaCellIdentity.cpid = rilTdscdmaCellIdentity.cpid;
  aidlTdscdmaCellIdentity.uarfcn = 0;  // default value

  convert(rilTdscdmaCellIdentity, aidlTdscdmaCellIdentity.operatorNames);
}

void convert(const RIL_TD_SCDMA_SignalStrength& rilTdscdmaSignalStrength,
    aidlimports::TdscdmaSignalStrength& aidlTdscdmaSignalStrength) {
  aidlTdscdmaSignalStrength.signalStrength = INT_MAX;
  aidlTdscdmaSignalStrength.bitErrorRate = INT_MAX;
  aidlTdscdmaSignalStrength.rscp = rilTdscdmaSignalStrength.rscp;
}

void convert(const RIL_CellInfoTdscdma& rilTdscdmaCellInfo,
    aidlimports::CellInfoTdscdma& aidlTdscdmaCellInfo) {
  convert(rilTdscdmaCellInfo.cellIdentityTdscdma, aidlTdscdmaCellInfo.cellIdentityTdscdma);
  convert(rilTdscdmaCellInfo.signalStrengthTdscdma, aidlTdscdmaCellInfo.signalStrengthTdscdma);
}
/*************************************** TDSCDMA ***************************************/


/*************************************** LTE *******************************************/
aidlimports::EutranBands mapEarfcnToEutranBand(int earfcn) {
  if (earfcn >= 70596) {
    return aidlimports::EutranBands::BAND_88;
  } else if (earfcn >= 70546) {
    return aidlimports::EutranBands::BAND_87;
  } else if (earfcn >= 70366) {
    return aidlimports::EutranBands::BAND_85;
  } else if (earfcn >= 69036) {
    return aidlimports::EutranBands::BAND_74;
  } else if (earfcn >= 68986) {
    return aidlimports::EutranBands::BAND_73;
  } else if (earfcn >= 68936) {
    return aidlimports::EutranBands::BAND_72;
  } else if (earfcn >= 68586) {
    return aidlimports::EutranBands::BAND_71;
  } else if (earfcn >= 68336) {
    return aidlimports::EutranBands::BAND_70;
  } else if (earfcn >= 67536) {
    return aidlimports::EutranBands::BAND_68;
  } else if (earfcn >= 66436) {
    return aidlimports::EutranBands::BAND_66;
  } else if (earfcn >= 65536) {
    return aidlimports::EutranBands::BAND_65;
  } else if (earfcn >= 60140) {
    return aidlimports::EutranBands::BAND_53;
  } else if (earfcn >= 59140) {
    return aidlimports::EutranBands::BAND_52;
  } else if (earfcn >= 59090) {
    return aidlimports::EutranBands::BAND_51;
  } else if (earfcn >= 58240) {
    return aidlimports::EutranBands::BAND_50;
  } else if (earfcn >= 56740) {
    return aidlimports::EutranBands::BAND_49;
  } else if (earfcn >= 55240) {
    return aidlimports::EutranBands::BAND_48;
  } else if (earfcn >= 54540) {
    return aidlimports::EutranBands::BAND_47;
  } else if (earfcn >= 46790) {
    return aidlimports::EutranBands::BAND_46;
  } else if (earfcn >= 46590) {
    return aidlimports::EutranBands::BAND_45;
  } else if (earfcn >= 45590) {
    return aidlimports::EutranBands::BAND_44;
  } else if (earfcn >= 43590) {
    return aidlimports::EutranBands::BAND_43;
  } else if (earfcn >= 41590) {
    return aidlimports::EutranBands::BAND_42;
  } else if (earfcn >= 39650) {
    return aidlimports::EutranBands::BAND_41;
  } else if (earfcn >= 38650) {
    return aidlimports::EutranBands::BAND_40;
  } else if (earfcn >= 38250) {
    return aidlimports::EutranBands::BAND_39;
  } else if (earfcn >= 37750) {
    return aidlimports::EutranBands::BAND_38;
  } else if (earfcn >= 37550) {
    return aidlimports::EutranBands::BAND_37;
  } else if (earfcn >= 36950) {
    return aidlimports::EutranBands::BAND_36;
  } else if (earfcn >= 36350) {
    return aidlimports::EutranBands::BAND_35;
  } else if (earfcn >= 36200) {
    return aidlimports::EutranBands::BAND_34;
  } else if (earfcn >= 36000) {
    return aidlimports::EutranBands::BAND_33;
  } else if (earfcn >= 9870) {
    return aidlimports::EutranBands::BAND_31;
  } else if (earfcn >= 9770) {
    return aidlimports::EutranBands::BAND_30;
  } else if (earfcn >= 9210) {
    return aidlimports::EutranBands::BAND_28;
  } else if (earfcn >= 9040) {
    return aidlimports::EutranBands::BAND_27;
  } else if (earfcn >= 8690) {
    return aidlimports::EutranBands::BAND_26;
  } else if (earfcn >= 8040) {
    return aidlimports::EutranBands::BAND_25;
  } else if (earfcn >= 7700) {
    return aidlimports::EutranBands::BAND_24;
  } else if (earfcn >= 7500) {
    return aidlimports::EutranBands::BAND_23;
  } else if (earfcn >= 6600) {
    return aidlimports::EutranBands::BAND_22;
  } else if (earfcn >= 6450) {
    return aidlimports::EutranBands::BAND_21;
  } else if (earfcn >= 6150) {
    return aidlimports::EutranBands::BAND_20;
  } else if (earfcn >= 6000) {
    return aidlimports::EutranBands::BAND_19;
  } else if (earfcn >= 5850) {
    return aidlimports::EutranBands::BAND_18;
  } else if (earfcn >= 5730) {
    return aidlimports::EutranBands::BAND_17;
  } else if (earfcn >= 5280) {
    return aidlimports::EutranBands::BAND_14;
  } else if (earfcn >= 5180) {
    return aidlimports::EutranBands::BAND_13;
  } else if (earfcn >= 5010) {
    return aidlimports::EutranBands::BAND_12;
  } else if (earfcn >= 4750) {
    return aidlimports::EutranBands::BAND_11;
  } else if (earfcn >= 4150) {
    return aidlimports::EutranBands::BAND_10;
  } else if (earfcn >= 3800) {
    return aidlimports::EutranBands::BAND_9;
  } else if (earfcn >= 3450) {
    return aidlimports::EutranBands::BAND_8;
  } else if (earfcn >= 2750) {
    return aidlimports::EutranBands::BAND_7;
  } else if (earfcn >= 2650) {
    return aidlimports::EutranBands::BAND_6;
  } else if (earfcn >= 2400) {
    return aidlimports::EutranBands::BAND_5;
  } else if (earfcn >= 1950) {
    return aidlimports::EutranBands::BAND_4;
  } else if (earfcn >= 1200) {
    return aidlimports::EutranBands::BAND_3;
  } else if (earfcn >= 600) {
    return aidlimports::EutranBands::BAND_2;
  } else if (earfcn >= 0) {
    return aidlimports::EutranBands::BAND_1;
  }

  return aidlimports::EutranBands::BAND_1;
}

void convert(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
    aidlimports::CellIdentityLte& aidlLteCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlLteCellIdentity.mcc,
      aidlLteCellIdentity.mnc, rilLteCellIdentity);

  aidlLteCellIdentity.ci = rilLteCellIdentity.ci;
  aidlLteCellIdentity.pci = rilLteCellIdentity.pci;
  aidlLteCellIdentity.tac = rilLteCellIdentity.tac;
  aidlLteCellIdentity.earfcn = rilLteCellIdentity.earfcn;

  convert(rilLteCellIdentity, aidlLteCellIdentity.operatorNames);

  aidlLteCellIdentity.bandwidth = rilLteCellIdentity.bandwidth;

  // Fill band only if its valid
  if (rilLteCellIdentity.band != INT_MAX && rilLteCellIdentity.band != 0) {
    aidlLteCellIdentity.bands.push_back(
        static_cast<aidlimports::EutranBands>(rilLteCellIdentity.band));
    QCRIL_LOG_DEBUG("band - %d", aidlLteCellIdentity.bands[0]);
  }

  if (rilLteCellIdentity.band == INT_MAX) {
    aidlLteCellIdentity.bands.push_back(
        mapEarfcnToEutranBand(aidlLteCellIdentity.earfcn));
    QCRIL_LOG_DEBUG("band - %d", aidlLteCellIdentity.bands[0]);
  }
}

void convert(const RIL_LTE_SignalStrength_v8& rilLteSignalStrength,
    aidlimports::LteSignalStrength& aidlLteSignalStrength) {
  aidlLteSignalStrength.signalStrength = rilLteSignalStrength.signalStrength;
  aidlLteSignalStrength.rsrp = rilLteSignalStrength.rsrp;
  aidlLteSignalStrength.rsrq = rilLteSignalStrength.rsrq;
  aidlLteSignalStrength.rssnr = rilLteSignalStrength.rssnr;
  aidlLteSignalStrength.cqi = rilLteSignalStrength.cqi;
  aidlLteSignalStrength.timingAdvance = rilLteSignalStrength.timingAdvance;
  aidlLteSignalStrength.cqiTableIndex = INT_MAX;
}

void convert(const RIL_CellInfoLte_v12& rilLteCellInfo,
    aidlimports::CellInfoLte& aidlLteCellInfo) {
  convert(rilLteCellInfo.cellIdentityLte, aidlLteCellInfo.cellIdentityLte);
  convert(rilLteCellInfo.signalStrengthLte, aidlLteCellInfo.signalStrengthLte);
}
/*************************************** LTE *******************************************/


/**************************************** NR *******************************************/
void convert(const RIL_CellInfoNr& rilNrCellInfo,
             aidlimports::QtiCellInfoNr& aidlNrCellInfo) {
  QCRIL_LOG_INFO("convert Cell Info NR");
  convert(rilNrCellInfo.cellIdentityNr, aidlNrCellInfo.cellIdentityNr);
  convert(rilNrCellInfo.signalStrengthNr, aidlNrCellInfo.signalStrengthNr);
}

void convert(const RIL_CellIdentityNr& rilNrCellIdentity,
             aidlimports::QtiCellIdentityNr& qtiCellIdentityNr) {
  QCRIL_LOG_INFO("convert NR Cell Identitty NR");
  aidlimports::CellIdentityNr cellIdentityNr{};
  getAidlPlmnFromCellIdentity(cellIdentityNr.mcc, cellIdentityNr.mnc,
      rilNrCellIdentity);
  cellIdentityNr.nci = rilNrCellIdentity.nci;
  cellIdentityNr.pci = rilNrCellIdentity.pci;
  cellIdentityNr.tac = rilNrCellIdentity.tac;
  cellIdentityNr.nrarfcn = rilNrCellIdentity.nrarfcn;
  convert(rilNrCellIdentity, cellIdentityNr.operatorNames);
  // Fill band only if its valid
  if (rilNrCellIdentity.band != INT_MAX && rilNrCellIdentity.band != 0) {
    QCRIL_LOG_DEBUG("band - %d", rilNrCellIdentity.band);
    cellIdentityNr.bands.push_back(
        static_cast<aidlimports::NgranBands>(rilNrCellIdentity.band));
  }
  qtiCellIdentityNr.cNr = cellIdentityNr;

  /* SNPN Info */
  if (rilNrCellIdentity.snpn_info_valid) {
    aidlimports::SnpnInfo snpnInfo{};
    convert(rilNrCellIdentity.snpn_info, snpnInfo);
    qtiCellIdentityNr.snpnInfo = std::move(snpnInfo);
  }

  /* CAG Info */
  if (rilNrCellIdentity.cag_info_valid) {
    aidlimports::CagInfo cagInfo{};
    convert(rilNrCellIdentity.cag_info, cagInfo);
    qtiCellIdentityNr.cagInfo = std::move(cagInfo);
  }
}

void convert(const RIL_NR_SignalStrength& rilNrSignalStrength,
  aidlimports::NrSignalStrength& aidlNrSignalStrength) {
  QCRIL_LOG_INFO("convert NR Signal Strength");
  aidlNrSignalStrength.ssRsrp = rilNrSignalStrength.ssRsrp;
  aidlNrSignalStrength.ssRsrq = rilNrSignalStrength.ssRsrq;
  aidlNrSignalStrength.ssSinr = rilNrSignalStrength.ssSinr;
  aidlNrSignalStrength.csiRsrp = rilNrSignalStrength.csiRsrp;
  aidlNrSignalStrength.csiRsrq = rilNrSignalStrength.csiRsrq;
  aidlNrSignalStrength.csiSinr = rilNrSignalStrength.csiSinr;
  aidlNrSignalStrength.csiCqiTableIndex = INT_MAX;
}

//SNPN conversion
void convert(const RIL_SnpnInfo& rilSnpnInfo, aidlimports::SnpnInfo& snpnInfo) {
  snpnInfo.nid.insert(snpnInfo.nid.begin(), std::begin(rilSnpnInfo.nid), std::end(rilSnpnInfo.nid));
  snpnInfo.mcc = rilSnpnInfo.mcc;
  snpnInfo.mnc = rilSnpnInfo.mnc;
  snpnInfo.operatorNumeric = snpnInfo.mcc + snpnInfo.mnc;
  snpnInfo.signalStrength = rilSnpnInfo.signalStrength;
  snpnInfo.signalQuality = convertSignalQualityFromRil(rilSnpnInfo.signalQuality);
  QCRIL_LOG_INFO("Convert Filling SNPN Info: %s", ::android::internal::ToString(snpnInfo).c_str());
}

// CAG Conversion
void convert(const RIL_CagInfo& rilCagInfo, aidlimports::CagInfo& cagInfo) {
  cagInfo.cagName = rilCagInfo.cagName;
  cagInfo.cagId = rilCagInfo.cagId;
  cagInfo.cagOnlyAccess = rilCagInfo.cagOnlyAccess;
  cagInfo.cagInAllowedList = rilCagInfo.cagInAllowedList;
  QCRIL_LOG_INFO("Convert Filling CAG Info: %s", ::android::internal::ToString(cagInfo).c_str());
}

/**************************************** NR *******************************************/

template <typename T>
bool getAidlPlmnFromCellIdentity(std::string& mcc, std::string& mnc, const T& cellIdentity) {
  if (cellIdentity.primary_plmn_mcc[0] != '\0' && cellIdentity.primary_plmn_mnc[0] != '\0') {
    mcc = std::string(
        cellIdentity.primary_plmn_mcc,
        strnlen(cellIdentity.primary_plmn_mcc, sizeof(cellIdentity.primary_plmn_mcc) - 1));
    mnc = std::string(
        cellIdentity.primary_plmn_mnc,
        strnlen(cellIdentity.primary_plmn_mnc, sizeof(cellIdentity.primary_plmn_mnc) - 1));
    return true;
  } else if (cellIdentity.mcc[0] != '\0' && cellIdentity.mnc[0] != '\0') {
    mcc = std::string(cellIdentity.mcc, strnlen(cellIdentity.mcc, sizeof(cellIdentity.mcc) - 1));
    mnc = std::string(cellIdentity.mnc, strnlen(cellIdentity.mnc, sizeof(cellIdentity.mnc) - 1));
    return true;
  }

  return false;
}

template <typename T>
void convert(const T& rilCellIdentity, aidlimports::OperatorInfo& aidlOperatorInfo) {
  aidlOperatorInfo.alphaLong = std::string(
      rilCellIdentity.operatorNames.alphaLong,
      strnlen(rilCellIdentity.operatorNames.alphaLong,
          sizeof(rilCellIdentity.operatorNames.alphaLong) - 1));

  aidlOperatorInfo.alphaShort = std::string(
      rilCellIdentity.operatorNames.alphaShort,
      strnlen(rilCellIdentity.operatorNames.alphaShort,
          sizeof(rilCellIdentity.operatorNames.alphaShort) - 1));

  std::string mcc;
  std::string mnc;
  getAidlPlmnFromCellIdentity(mcc, mnc, rilCellIdentity);
  aidlOperatorInfo.operatorNumeric = mcc + mnc;

  aidlOperatorInfo.status = aidlimports::OperatorInfo::STATUS_CURRENT;
}

template <>
bool getAidlPlmnFromCellIdentity<RIL_CellIdentityCdma>(std::string& /*mcc*/, std::string& /*mnc*/,
    const RIL_CellIdentityCdma& /*cellIdentity*/) {
  return true;
}

RIL_RadioTechnology getRilRadioTechnologyFromRan(aidlimports::AccessNetwork ran) {
  switch (ran) {
    case aidlimports::AccessNetwork::GERAN:
      return RADIO_TECH_GSM;
    case aidlimports::AccessNetwork::UTRAN:
      return RADIO_TECH_UMTS;
    case aidlimports::AccessNetwork::EUTRAN:
      return RADIO_TECH_LTE;
    case aidlimports::AccessNetwork::NGRAN:
      return RADIO_TECH_5G;
    case aidlimports::AccessNetwork::CDMA2000:
      return RADIO_TECH_IS95A;
    default: {
      break;
    }
  }
  return RADIO_TECH_UNKNOWN;
}

aidlimports::CiwlanMode convertCiwlanModeFromRil(qcril::interfaces::ImsCiWlanMode mode) {
  switch (mode) {
    case qcril::interfaces::ImsCiWlanMode::C_IWLAN_ONLY:
      return aidlimports::CiwlanMode::ONLY;
    case qcril::interfaces::ImsCiWlanMode::C_IWLAN_PREFERRED:
      return aidlimports::CiwlanMode::PREFERRED;
    default:
      return aidlimports::CiwlanMode::INVALID;
  }
}

aidlimports::PersoUnlockStatus convertToAidlPersoLockStatus(RIL_UIM_unlock_Status status) {
  switch (status) {
    case RIL_UIM_UNLOCK_STATUS_UNKNOWN:
      return aidlimports::PersoUnlockStatus::UNKNOWN;
    case RIL_UIM_UNLOCK_STATUS_TEMPORARY_UNLOCKED:
      return aidlimports::PersoUnlockStatus::TEMPORARY_UNLOCKED;
    case RIL_UIM_UNLOCK_STATUS_PERMANENTLY_UNLOCKED:
      return aidlimports::PersoUnlockStatus::PERMANENT_UNLOCKED;
    default:
      return aidlimports::PersoUnlockStatus::UNKNOWN;
  }
}

aidlimports::PersoUnlockStatus convertRilPersolockStatus(RIL_UIM_Perso_lock_Status persoLockStatus)
{
  aidlimports::PersoUnlockStatus persoUnlockStatus = aidlimports::PersoUnlockStatus::UNKNOWN;
  RIL_UIM_App_perso_lock *persoLock = NULL;

  for (int index = 0; index < RIL_UIM_CARD_MAX_APPS; index++) {
    persoLock = &persoLockStatus.applications[index];
    if (persoLock == NULL) {
       continue;
    }
    if (persoLock->app_type == RIL_UIM_APPTYPE_UNKNOWN ||
        persoLock->app_type == RIL_UIM_APPTYPE_ISIM) {
      continue;
    }

    persoUnlockStatus = convertToAidlPersoLockStatus(persoLock->unlock_status);
    if (persoUnlockStatus == aidlimports::PersoUnlockStatus::TEMPORARY_UNLOCKED) {
      break;
    }
  }
  return persoUnlockStatus;
}

}  // namespace utils
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
