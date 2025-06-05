/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "network_aidl_service_utils.h"

#undef TAG
#define TAG "RILQ"

namespace qti {
namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace network {
namespace utils {

aidlnetwork::SuppSvcNotification convertRILSuppSvcNotificationToAidl(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  aidlnetwork::SuppSvcNotification suppSvc{};
  if (msg) {
    if (msg->hasNotificationType()) {
      if (msg->getNotificationType() == qcril::interfaces::NotificationType::MT) {
        suppSvc.isMT = true;
      } else {
        suppSvc.isMT = false;
      }
    }
    if (msg->hasCode()) {
      suppSvc.code = static_cast<int32_t>(msg->getCode());
    }
    if (msg->hasIndex()) {
      suppSvc.index = static_cast<int32_t>(msg->getIndex());
    }
    if (msg->hasType()) {
      suppSvc.type = static_cast<int32_t>(msg->getType());
    }
    if (msg->hasNumber() && !msg->getNumber().empty()) {
      suppSvc.number = msg->getNumber();
    }
  }
  return suppSvc;
}

aidlnetwork::OperatorInfo convertRilNetworkInfotoAidl(const qcril::interfaces::NetworkInfo& rilData) {
  aidlnetwork::OperatorInfo aidlData{};
  aidlData.alphaLong = rilData.alphaLong;
  aidlData.alphaShort = rilData.alphaShort;
  aidlData.operatorNumeric = rilData.operatorNumeric;
  aidlData.status = static_cast<int32_t>(rilData.status);
  return aidlData;
}

qcril::interfaces::FacilityType convertFacilityType(const std::string& in) {
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

RIL_RadioAccessNetworks convertAidlRanToRilRan(
    aidlradio::AccessNetwork ran) {
  switch (ran) {
    case aidlradio::AccessNetwork::GERAN:
      return RIL_RadioAccessNetworks::GERAN;
    case aidlradio::AccessNetwork::UTRAN:
      return RIL_RadioAccessNetworks::UTRAN;
    case aidlradio::AccessNetwork::EUTRAN:
      return RIL_RadioAccessNetworks::EUTRAN;
    case aidlradio::AccessNetwork::NGRAN:
      return RIL_RadioAccessNetworks::NGRAN;
    case aidlradio::AccessNetwork::CDMA2000:
      return RIL_RadioAccessNetworks::CDMA;
    default:
      return RIL_RadioAccessNetworks::UNKNOWN;
  }
}

RIL_Errno convertAidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
                                 const aidlnetwork::RadioAccessSpecifier& ras_from) {

  using rasBands = aidlnetwork::RadioAccessSpecifierBands;
  ras_to.radio_access_network =
      convertAidlRanToRilRan(ras_from.accessNetwork);
  ras_to.channels_length = ras_from.channels.size();

  std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
  switch (ras_from.accessNetwork) {
    case aidlradio::AccessNetwork::GERAN:
      if (ras_from.bands.getTag() == rasBands::geranBands) {
        auto vec = ras_from.bands.get<rasBands::geranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.geran_bands[idx] =
              static_cast<RIL_GeranBands>(vec[idx]);
        }
      }
      break;
    case aidlradio::AccessNetwork::UTRAN:
      if (ras_from.bands.getTag() == rasBands::utranBands) {
        auto vec = ras_from.bands.get<rasBands::utranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.utran_bands[idx] =
              static_cast<RIL_UtranBands>(vec[idx]);
        }
      }
      break;
    case aidlradio::AccessNetwork::EUTRAN:
      if (ras_from.bands.getTag() == rasBands::eutranBands) {
        auto vec = ras_from.bands.get<rasBands::eutranBands>();
        ras_to.bands_length = vec.size();
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
          ras_to.bands.eutran_bands[idx] =
              static_cast<RIL_EutranBands>(vec[idx]);
        }
      }
      break;
    case aidlradio::AccessNetwork::NGRAN:
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

RIL_Errno rasArgsCheck(const std::vector<aidlnetwork::RadioAccessSpecifier>& specifiers) {
  using rasBands = aidlnetwork::RadioAccessSpecifierBands;
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

RIL_Errno fillSetSystemSelectionChannelRequest(const std::vector<aidlnetwork::RadioAccessSpecifier>& specifiers,
  RIL_SysSelChannels& request) {

  RIL_Errno error = RIL_E_SUCCESS;
  error = rasArgsCheck(specifiers);
  if (error != RIL_E_SUCCESS) {
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
      error = convertAidlRasToRilRas(request.specifiers_latest[i], specifiers[i]);
      if (error != RIL_E_SUCCESS) {
        QCRIL_LOG_ERROR("failed to convert Aidl fields to ril fields - %d", error);
        return error;
      }
    }
  } else {
    return RIL_E_NO_MEMORY;
  }
  return RIL_E_SUCCESS;
}

RIL_Errno fillNetworkScanRequest(const aidlnetwork::NetworkScanRequest& request,
    RIL_NetworkScanRequest& scanRequest) {

  if (request.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
     QCRIL_LOG_ERROR("MAX RANs exceeded");
     return RIL_E_INVALID_ARGUMENTS;
  }

  if (request.mccMncs.size() > MAX_MCC_MNC_LIST_SIZE) {
     QCRIL_LOG_ERROR("MAX MccMncs exceeded");
     return RIL_E_INVALID_ARGUMENTS;
  }

  RIL_Errno error = rasArgsCheck(request.specifiers);
  if (error != RIL_E_SUCCESS) {
    QCRIL_LOG_ERROR("RAS Args check failed - %d", error);
    return error;
  }

  if (request.type == aidlnetwork::NetworkScanRequest::SCAN_TYPE_ONE_SHOT) {
    scanRequest.type = RIL_ONE_SHOT;
  } else if (request.type == aidlnetwork::NetworkScanRequest::SCAN_TYPE_PERIODIC) {
    scanRequest.type = RIL_PERIODIC;
  }
  scanRequest.interval = request.interval;
  scanRequest.maxSearchTime = request.maxSearchTime;
  scanRequest.incrementalResults = request.incrementalResults;
  scanRequest.incrementalResultsPeriodicity = request.incrementalResultsPeriodicity;
  scanRequest.mccMncLength = request.mccMncs.size();
  scanRequest.specifiers_length = request.specifiers.size();

  for (size_t i = 0; i < request.specifiers.size(); ++i) {
    error = convertAidlRasToRilRas(scanRequest.specifiers[i], request.specifiers[i]);
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

template <>
bool getAidlPlmnFromCellIdentity<RIL_CellIdentityCdma>(std::string& mcc, std::string& mnc,
    const RIL_CellIdentityCdma& cellIdentity) {
  return true;
}

template <typename T>
void convert(const T& rilCellIdentity, aidlnetwork::OperatorInfo& aidlOperatorInfo) {
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

  aidlOperatorInfo.status = aidlnetwork::OperatorInfo::STATUS_CURRENT;
}

/*************************************** GSM ***************************************/
void convert(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
    aidlnetwork::CellIdentityGsm& aidlGsmCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlGsmCellIdentity.mcc,
      aidlGsmCellIdentity.mnc, rilGsmCellIdentity);

  aidlGsmCellIdentity.lac = rilGsmCellIdentity.lac;
  aidlGsmCellIdentity.cid = rilGsmCellIdentity.cid;
  aidlGsmCellIdentity.arfcn = rilGsmCellIdentity.arfcn;
  aidlGsmCellIdentity.bsic = rilGsmCellIdentity.bsic;

  convert(rilGsmCellIdentity, aidlGsmCellIdentity.operatorNames);
}

void convert(const RIL_GSM_SignalStrength_v12& rilGsmSignalStrength,
    aidlnetwork::GsmSignalStrength& aidlGsmSignalStrength) {
  aidlGsmSignalStrength.signalStrength = rilGsmSignalStrength.signalStrength;
  aidlGsmSignalStrength.bitErrorRate = rilGsmSignalStrength.bitErrorRate;
  aidlGsmSignalStrength.timingAdvance = rilGsmSignalStrength.timingAdvance;
}

void convert(const RIL_CellInfoGsm_v12& rilGsmCellInfo,
    aidlnetwork::CellInfoGsm& aidlGsmCellInfo) {
  convert(rilGsmCellInfo.cellIdentityGsm, aidlGsmCellInfo.cellIdentityGsm);
  convert(rilGsmCellInfo.signalStrengthGsm, aidlGsmCellInfo.signalStrengthGsm);
}
/*************************************** GSM ***************************************/

/*************************************** WCDMA ***************************************/
void convert(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
    aidlnetwork::CellIdentityWcdma& aidlWcdmaCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlWcdmaCellIdentity.mcc,
      aidlWcdmaCellIdentity.mnc, rilWcdmaCellIdentity);

  aidlWcdmaCellIdentity.lac = rilWcdmaCellIdentity.lac;
  aidlWcdmaCellIdentity.cid = rilWcdmaCellIdentity.cid;
  aidlWcdmaCellIdentity.psc = rilWcdmaCellIdentity.psc;
  aidlWcdmaCellIdentity.uarfcn = rilWcdmaCellIdentity.uarfcn;

  convert(rilWcdmaCellIdentity, aidlWcdmaCellIdentity.operatorNames);
}

void convert(const RIL_SignalStrengthWcdma& rilWcdmaSignalStrength,
    aidlnetwork::WcdmaSignalStrength& aidlWcdmaSignalStrength) {
  aidlWcdmaSignalStrength.signalStrength = rilWcdmaSignalStrength.signalStrength;
  aidlWcdmaSignalStrength.bitErrorRate = rilWcdmaSignalStrength.bitErrorRate;
}

void convert(const RIL_CellInfoWcdma_v12& rilWcdmaCellInfo,
    aidlnetwork::CellInfoWcdma& aidlWcdmaCellInfo) {
  convert(rilWcdmaCellInfo.cellIdentityWcdma, aidlWcdmaCellInfo.cellIdentityWcdma);
  convert(rilWcdmaCellInfo.signalStrengthWcdma, aidlWcdmaCellInfo.signalStrengthWcdma);
}
/*************************************** WCDMA ***************************************/

/*************************************** CDMA ***************************************/
void convert(const RIL_CellIdentityCdma& rilCdmaCellIdentity,
    aidlnetwork::CellIdentityCdma& aidlCdmaCellIdentity) {
  aidlCdmaCellIdentity.networkId = rilCdmaCellIdentity.networkId;
  aidlCdmaCellIdentity.systemId = rilCdmaCellIdentity.systemId;
  aidlCdmaCellIdentity.baseStationId = rilCdmaCellIdentity.basestationId;
  aidlCdmaCellIdentity.longitude = rilCdmaCellIdentity.longitude;
  aidlCdmaCellIdentity.latitude = rilCdmaCellIdentity.latitude;

  convert(rilCdmaCellIdentity, aidlCdmaCellIdentity.operatorNames);
}

void convert(const RIL_CDMA_SignalStrength& rilCdmaSignalStrength,
    aidlnetwork::CdmaSignalStrength& aidlCdmaSignalStrength) {
  aidlCdmaSignalStrength.dbm = rilCdmaSignalStrength.dbm;
  aidlCdmaSignalStrength.ecio = rilCdmaSignalStrength.ecio;
}

void convert(const RIL_EVDO_SignalStrength& rilEvdoSignalStrength,
    aidlnetwork::EvdoSignalStrength& aidlEvdoSignalStrength) {
  aidlEvdoSignalStrength.dbm = rilEvdoSignalStrength.dbm;
  aidlEvdoSignalStrength.ecio = rilEvdoSignalStrength.ecio;
  aidlEvdoSignalStrength.signalNoiseRatio = rilEvdoSignalStrength.signalNoiseRatio;
}

void convert(const RIL_CellInfoCdma& rilCdmaCellInfo,
    aidlnetwork::CellInfoCdma& aidlCdmaCellInfo) {
  convert(rilCdmaCellInfo.cellIdentityCdma, aidlCdmaCellInfo.cellIdentityCdma);
  convert(rilCdmaCellInfo.signalStrengthCdma, aidlCdmaCellInfo.signalStrengthCdma);
  convert(rilCdmaCellInfo.signalStrengthEvdo, aidlCdmaCellInfo.signalStrengthEvdo);
}
/*************************************** CDMA ***************************************/

/*************************************** LTE ***************************************/
aidlnetwork::EutranBands mapEarfcnToEutranBand(int earfcn) {
  if (earfcn >= 70596) {
    return aidlnetwork::EutranBands::BAND_88;
  } else if (earfcn >= 70546) {
    return aidlnetwork::EutranBands::BAND_87;
  } else if (earfcn >= 70366) {
    return aidlnetwork::EutranBands::BAND_85;
  } else if (earfcn >= 69036) {
    return aidlnetwork::EutranBands::BAND_74;
  } else if (earfcn >= 68986) {
    return aidlnetwork::EutranBands::BAND_73;
  } else if (earfcn >= 68936) {
    return aidlnetwork::EutranBands::BAND_72;
  } else if (earfcn >= 68586) {
    return aidlnetwork::EutranBands::BAND_71;
  } else if (earfcn >= 68336) {
    return aidlnetwork::EutranBands::BAND_70;
  } else if (earfcn >= 67536) {
    return aidlnetwork::EutranBands::BAND_68;
  } else if (earfcn >= 66436) {
    return aidlnetwork::EutranBands::BAND_66;
  } else if (earfcn >= 65536) {
    return aidlnetwork::EutranBands::BAND_65;
  } else if (earfcn >= 60140) {
    return aidlnetwork::EutranBands::BAND_53;
  } else if (earfcn >= 59140) {
    return aidlnetwork::EutranBands::BAND_52;
  } else if (earfcn >= 59090) {
    return aidlnetwork::EutranBands::BAND_51;
  } else if (earfcn >= 58240) {
    return aidlnetwork::EutranBands::BAND_50;
  } else if (earfcn >= 56740) {
    return aidlnetwork::EutranBands::BAND_49;
  } else if (earfcn >= 55240) {
    return aidlnetwork::EutranBands::BAND_48;
  } else if (earfcn >= 54540) {
    return aidlnetwork::EutranBands::BAND_47;
  } else if (earfcn >= 46790) {
    return aidlnetwork::EutranBands::BAND_46;
  } else if (earfcn >= 46590) {
    return aidlnetwork::EutranBands::BAND_45;
  } else if (earfcn >= 45590) {
    return aidlnetwork::EutranBands::BAND_44;
  } else if (earfcn >= 43590) {
    return aidlnetwork::EutranBands::BAND_43;
  } else if (earfcn >= 41590) {
    return aidlnetwork::EutranBands::BAND_42;
  } else if (earfcn >= 39650) {
    return aidlnetwork::EutranBands::BAND_41;
  } else if (earfcn >= 38650) {
    return aidlnetwork::EutranBands::BAND_40;
  } else if (earfcn >= 38250) {
    return aidlnetwork::EutranBands::BAND_39;
  } else if (earfcn >= 37750) {
    return aidlnetwork::EutranBands::BAND_38;
  } else if (earfcn >= 37550) {
    return aidlnetwork::EutranBands::BAND_37;
  } else if (earfcn >= 36950) {
    return aidlnetwork::EutranBands::BAND_36;
  } else if (earfcn >= 36350) {
    return aidlnetwork::EutranBands::BAND_35;
  } else if (earfcn >= 36200) {
    return aidlnetwork::EutranBands::BAND_34;
  } else if (earfcn >= 36000) {
    return aidlnetwork::EutranBands::BAND_33;
  } else if (earfcn >= 9870) {
    return aidlnetwork::EutranBands::BAND_31;
  } else if (earfcn >= 9770) {
    return aidlnetwork::EutranBands::BAND_30;
  } else if (earfcn >= 9210) {
    return aidlnetwork::EutranBands::BAND_28;
  } else if (earfcn >= 9040) {
    return aidlnetwork::EutranBands::BAND_27;
  } else if (earfcn >= 8690) {
    return aidlnetwork::EutranBands::BAND_26;
  } else if (earfcn >= 8040) {
    return aidlnetwork::EutranBands::BAND_25;
  } else if (earfcn >= 7700) {
    return aidlnetwork::EutranBands::BAND_24;
  } else if (earfcn >= 7500) {
    return aidlnetwork::EutranBands::BAND_23;
  } else if (earfcn >= 6600) {
    return aidlnetwork::EutranBands::BAND_22;
  } else if (earfcn >= 6450) {
    return aidlnetwork::EutranBands::BAND_21;
  } else if (earfcn >= 6150) {
    return aidlnetwork::EutranBands::BAND_20;
  } else if (earfcn >= 6000) {
    return aidlnetwork::EutranBands::BAND_19;
  } else if (earfcn >= 5850) {
    return aidlnetwork::EutranBands::BAND_18;
  } else if (earfcn >= 5730) {
    return aidlnetwork::EutranBands::BAND_17;
  } else if (earfcn >= 5280) {
    return aidlnetwork::EutranBands::BAND_14;
  } else if (earfcn >= 5180) {
    return aidlnetwork::EutranBands::BAND_13;
  } else if (earfcn >= 5010) {
    return aidlnetwork::EutranBands::BAND_12;
  } else if (earfcn >= 4750) {
    return aidlnetwork::EutranBands::BAND_11;
  } else if (earfcn >= 4150) {
    return aidlnetwork::EutranBands::BAND_10;
  } else if (earfcn >= 3800) {
    return aidlnetwork::EutranBands::BAND_9;
  } else if (earfcn >= 3450) {
    return aidlnetwork::EutranBands::BAND_8;
  } else if (earfcn >= 2750) {
    return aidlnetwork::EutranBands::BAND_7;
  } else if (earfcn >= 2650) {
    return aidlnetwork::EutranBands::BAND_6;
  } else if (earfcn >= 2400) {
    return aidlnetwork::EutranBands::BAND_5;
  } else if (earfcn >= 1950) {
    return aidlnetwork::EutranBands::BAND_4;
  } else if (earfcn >= 1200) {
    return aidlnetwork::EutranBands::BAND_3;
  } else if (earfcn >= 600) {
    return aidlnetwork::EutranBands::BAND_2;
  } else if (earfcn >= 0) {
    return aidlnetwork::EutranBands::BAND_1;
  }

  return aidlnetwork::EutranBands::BAND_1;
}

void convert(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
    aidlnetwork::CellIdentityLte& aidlLteCellIdentity) {
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
        static_cast<aidlnetwork::EutranBands>(rilLteCellIdentity.band));
    QCRIL_LOG_DEBUG("band - %d", aidlLteCellIdentity.bands[0]);
  }

  if (rilLteCellIdentity.band == INT_MAX) {
    aidlLteCellIdentity.bands.push_back(
        mapEarfcnToEutranBand(aidlLteCellIdentity.earfcn));
    QCRIL_LOG_DEBUG("band - %d", aidlLteCellIdentity.bands[0]);
  }
}

void convert(const RIL_LTE_SignalStrength_v8& rilLteSignalStrength,
    aidlnetwork::LteSignalStrength& aidlLteSignalStrength) {
  aidlLteSignalStrength.signalStrength = rilLteSignalStrength.signalStrength;
  aidlLteSignalStrength.rsrp = rilLteSignalStrength.rsrp;
  aidlLteSignalStrength.rsrq = rilLteSignalStrength.rsrq;
  aidlLteSignalStrength.rssnr = rilLteSignalStrength.rssnr;
  aidlLteSignalStrength.cqi = rilLteSignalStrength.cqi;
  aidlLteSignalStrength.timingAdvance = rilLteSignalStrength.timingAdvance;
  aidlLteSignalStrength.cqiTableIndex = INT_MAX;
}

void convert(const RIL_CellInfoLte_v12& rilLteCellInfo,
    aidlnetwork::CellInfoLte& aidlLteCellInfo) {
  convert(rilLteCellInfo.cellIdentityLte, aidlLteCellInfo.cellIdentityLte);
  convert(rilLteCellInfo.signalStrengthLte, aidlLteCellInfo.signalStrengthLte);
}
/*************************************** LTE ***************************************/

/*************************************** 5G ***************************************/
void convert(const RIL_CellIdentityNr& rilNrCellIdentity,
    aidlnetwork::CellIdentityNr& aidlNrCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlNrCellIdentity.mcc, aidlNrCellIdentity.mnc,
      rilNrCellIdentity);

  aidlNrCellIdentity.nci = rilNrCellIdentity.nci;
  aidlNrCellIdentity.pci = rilNrCellIdentity.pci;
  aidlNrCellIdentity.tac = rilNrCellIdentity.tac;
  aidlNrCellIdentity.nrarfcn = rilNrCellIdentity.nrarfcn;

  convert(rilNrCellIdentity, aidlNrCellIdentity.operatorNames);

  // Fill band only if its valid
  if (rilNrCellIdentity.band != INT_MAX && rilNrCellIdentity.band != 0) {
    QCRIL_LOG_DEBUG("band - %d", rilNrCellIdentity.band);
    aidlNrCellIdentity.bands.push_back(
        static_cast<aidlnetwork::NgranBands>(rilNrCellIdentity.band));
  }
}

void convert(const RIL_NR_SignalStrength& rilNrSignalStrength,
    aidlnetwork::NrSignalStrength& aidlNrSignalStrength) {
  aidlNrSignalStrength.ssRsrp = rilNrSignalStrength.ssRsrp;
  aidlNrSignalStrength.ssRsrq = rilNrSignalStrength.ssRsrq;
  aidlNrSignalStrength.ssSinr = rilNrSignalStrength.ssSinr;
  aidlNrSignalStrength.csiRsrp = rilNrSignalStrength.csiRsrp;
  aidlNrSignalStrength.csiRsrq = rilNrSignalStrength.csiRsrq;
  aidlNrSignalStrength.csiSinr = rilNrSignalStrength.csiSinr;
  aidlNrSignalStrength.csiCqiTableIndex = INT_MAX;
}

void convert(const RIL_CellInfoNr& rilNrCellInfo,
    aidlnetwork::CellInfoNr& aidlNrCellInfo) {
  convert(rilNrCellInfo.cellIdentityNr, aidlNrCellInfo.cellIdentityNr);
  convert(rilNrCellInfo.signalStrengthNr, aidlNrCellInfo.signalStrengthNr);
}
/*************************************** 5G ***************************************/

/*************************************** TDSCDMA ***************************************/
void convert(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
    aidlnetwork::CellIdentityTdscdma& aidlTdscdmaCellIdentity) {
  getAidlPlmnFromCellIdentity(aidlTdscdmaCellIdentity.mcc,
      aidlTdscdmaCellIdentity.mnc, rilTdscdmaCellIdentity);

  aidlTdscdmaCellIdentity.lac = rilTdscdmaCellIdentity.lac;
  aidlTdscdmaCellIdentity.cid = rilTdscdmaCellIdentity.cid;
  aidlTdscdmaCellIdentity.cpid = rilTdscdmaCellIdentity.cpid;
  aidlTdscdmaCellIdentity.uarfcn = 0;  // default value

  convert(rilTdscdmaCellIdentity, aidlTdscdmaCellIdentity.operatorNames);
}

void convert(const RIL_TD_SCDMA_SignalStrength& rilTdscdmaSignalStrength,
    aidlnetwork::TdscdmaSignalStrength& aidlTdscdmaSignalStrength) {
  aidlTdscdmaSignalStrength.signalStrength = INT_MAX;
  aidlTdscdmaSignalStrength.bitErrorRate = INT_MAX;
  aidlTdscdmaSignalStrength.rscp = rilTdscdmaSignalStrength.rscp;
}

void convert(const RIL_CellInfoTdscdma& rilTdscdmaCellInfo,
    aidlnetwork::CellInfoTdscdma& aidlTdscdmaCellInfo) {
  convert(rilTdscdmaCellInfo.cellIdentityTdscdma, aidlTdscdmaCellInfo.cellIdentityTdscdma);
  convert(rilTdscdmaCellInfo.signalStrengthTdscdma, aidlTdscdmaCellInfo.signalStrengthTdscdma);
}
/*************************************** TDSCDMA ***************************************/

void convert(const RIL_CellInfo_v12& rilCellInfo, aidlnetwork::CellInfo& aidlCellInfo) {
  aidlCellInfo.registered = rilCellInfo.registered;
  aidlCellInfo.connectionStatus =
      static_cast<aidlnetwork::CellConnectionStatus>(rilCellInfo.connStatus);

  switch (rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      aidlnetwork::CellInfoGsm aidlGsmCellInfo {};
      convert(rilCellInfo.CellInfo.gsm, aidlGsmCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::gsm>(
          aidlGsmCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      aidlnetwork::CellInfoWcdma aidlWcdmaCellInfo {};
      convert(rilCellInfo.CellInfo.wcdma, aidlWcdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::wcdma>(
          aidlWcdmaCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      aidlnetwork::CellInfoCdma aidlCdmaCellInfo {};
      convert(rilCellInfo.CellInfo.cdma, aidlCdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::cdma>(
          aidlCdmaCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      aidlnetwork::CellInfoLte aidlLteCellInfo {};
      convert(rilCellInfo.CellInfo.lte, aidlLteCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::lte>(
          aidlLteCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      aidlnetwork::CellInfoNr aidlNrCellInfo {};
      convert(rilCellInfo.CellInfo.nr, aidlNrCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::nr>(
          aidlNrCellInfo);
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      aidlnetwork::CellInfoTdscdma aidlTdscdmaCellInfo {};
      convert(rilCellInfo.CellInfo.tdscdma, aidlTdscdmaCellInfo);
      aidlCellInfo.ratSpecificInfo.set<aidlnetwork::CellInfoRatSpecificInfo::Tag::tdscdma>(
          aidlTdscdmaCellInfo);
      break;
    }

    default: {
      break;
    }
  }
}

void convert(const std::vector<RIL_CellInfo_v12>& rilCellInfos,
    std::vector<aidlnetwork::CellInfo>& aidlCellInfos) {
  size_t num = rilCellInfos.size();
  aidlCellInfos.resize(num);
  for (unsigned int i = 0; i < num; i++) {
    convert(rilCellInfos[i], aidlCellInfos[i]);
  }
}

void convert(const RIL_GW_SignalStrength& rilGsmSignalStrength,
    aidlnetwork::GsmSignalStrength& aidlGsmSignalStrength) {
  aidlGsmSignalStrength.signalStrength = rilGsmSignalStrength.signalStrength;
  aidlGsmSignalStrength.bitErrorRate = rilGsmSignalStrength.bitErrorRate;
  aidlGsmSignalStrength.timingAdvance = INT_MAX;
}

void convert(const RIL_WCDMA_SignalStrength& rilWcdmaSignalStrength,
    aidlnetwork::WcdmaSignalStrength& aidlWcdmaSignalStrength) {
  aidlWcdmaSignalStrength.signalStrength = rilWcdmaSignalStrength.signalStrength;
  aidlWcdmaSignalStrength.bitErrorRate = rilWcdmaSignalStrength.bitErrorRate;
  aidlWcdmaSignalStrength.rscp = rilWcdmaSignalStrength.rscp;
  aidlWcdmaSignalStrength.ecno = rilWcdmaSignalStrength.ecio;
}

void convert(const RIL_SignalStrength& rilSignalStrength,
    aidlnetwork::SignalStrength& aidlSignalStrength) {
  convert(rilSignalStrength.GW_SignalStrength, aidlSignalStrength.gsm);
  convert(rilSignalStrength.CDMA_SignalStrength, aidlSignalStrength.cdma);
  convert(rilSignalStrength.EVDO_SignalStrength, aidlSignalStrength.evdo);
  convert(rilSignalStrength.LTE_SignalStrength, aidlSignalStrength.lte);
  convert(rilSignalStrength.TD_SCDMA_SignalStrength, aidlSignalStrength.tdscdma);
  convert(rilSignalStrength.WCDMA_SignalStrength, aidlSignalStrength.wcdma);
  convert(rilSignalStrength.NR_SignalStrength, aidlSignalStrength.nr);
}

void convert(const RIL_CellIdentity_v16& rilCellIdentity,
    aidlnetwork::CellIdentity& aidlCellIdentity) {
  switch (rilCellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      aidlnetwork::CellIdentityGsm aidlGsmCellIdentity {};
      convert(rilCellIdentity.cellIdentityGsm, aidlGsmCellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::gsm>(aidlGsmCellIdentity);
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      aidlnetwork::CellIdentityWcdma aidlWcdmaCellIdentity {};
      convert(rilCellIdentity.cellIdentityWcdma, aidlWcdmaCellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::wcdma>(aidlWcdmaCellIdentity);
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      aidlnetwork::CellIdentityCdma aidlCdmaCellIdentity {};
      convert(rilCellIdentity.cellIdentityCdma, aidlCdmaCellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::cdma>(aidlCdmaCellIdentity);
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      aidlnetwork::CellIdentityLte aidlLteCellIdentity {};
      convert(rilCellIdentity.cellIdentityLte, aidlLteCellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::lte>(aidlLteCellIdentity);
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      aidlnetwork::CellIdentityTdscdma aidlTdscdmaCellIdentity {};
      convert(rilCellIdentity.cellIdentityTdscdma, aidlTdscdmaCellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::tdscdma>(aidlTdscdmaCellIdentity);
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      aidlnetwork::CellIdentityNr aidlNrcellIdentity {};
      convert(rilCellIdentity.cellIdentityNr, aidlNrcellIdentity);
      aidlCellIdentity.set<aidlnetwork::CellIdentity::Tag::nr>(aidlNrcellIdentity);
      break;
    }

    default: {
      break;
    }
  }
}

std::string getRegisteredPlmnFromCellIdentity(const RIL_CellIdentity_v16& cellIdentity) {
  std::string registeredPlmn;

  switch (cellIdentity.cellInfoType) {
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
      registeredPlmn += cellIdentity.cellIdentityGsm.mcc;
      registeredPlmn += cellIdentity.cellIdentityGsm.mnc;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
      registeredPlmn += cellIdentity.cellIdentityLte.mcc;
      registeredPlmn += cellIdentity.cellIdentityLte.mnc;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
      registeredPlmn += cellIdentity.cellIdentityWcdma.mcc;
      registeredPlmn += cellIdentity.cellIdentityWcdma.mnc;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
      registeredPlmn += cellIdentity.cellIdentityTdscdma.mcc;
      registeredPlmn += cellIdentity.cellIdentityTdscdma.mnc;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NR:
      registeredPlmn += cellIdentity.cellIdentityNr.mcc;
      registeredPlmn += cellIdentity.cellIdentityNr.mnc;
      break;
    default:
      break;
  }

  return registeredPlmn;
}

void convert(const NrVopsInfo& rilNrVopsInfo, aidlnetwork::NrVopsInfo& aidlNrVopsInfo) {
  switch (rilNrVopsInfo.isVopsSupported) {
    case 0:
      aidlNrVopsInfo.vopsSupported = aidlnetwork::NrVopsInfo::VOPS_INDICATOR_VOPS_NOT_SUPPORTED;
      break;
    case 1:
      aidlNrVopsInfo.vopsSupported = aidlnetwork::NrVopsInfo::VOPS_INDICATOR_VOPS_OVER_3GPP;
      break;
    case 2:
      aidlNrVopsInfo.vopsSupported = aidlnetwork::NrVopsInfo::VOPS_INDICATOR_VOPS_OVER_NON_3GPP;
      break;
    default:
      QCRIL_LOG_ERROR("%d is not a recognized value for isVopsSupported", rilNrVopsInfo.isVopsSupported);
      break;
  }

  switch (rilNrVopsInfo.isEmcSupported) {
    case RIL_Nr5gEmcVal::NR5G_EMC_NOT_SUPPORTED:
      aidlNrVopsInfo.emcSupported = aidlnetwork::NrVopsInfo::EMC_INDICATOR_NOT_SUPPORTED;
      break;
    case RIL_Nr5gEmcVal::NR5G_EMC_SUPPORTED_NR:
      aidlNrVopsInfo.emcSupported = aidlnetwork::NrVopsInfo::EMC_INDICATOR_NR_CONNECTED_TO_5GCN;
      break;
    case RIL_Nr5gEmcVal::NR5G_EMC_SUPPORTED_EUTRA:
      aidlNrVopsInfo.emcSupported = aidlnetwork::NrVopsInfo::EMC_INDICATOR_EUTRA_CONNECTED_TO_5GCN;
      break;
    case RIL_Nr5gEmcVal::NR5G_EMC_SUPPORTED_NR_AND_EUTRA:
      aidlNrVopsInfo.emcSupported = aidlnetwork::NrVopsInfo::EMC_INDICATOR_BOTH_NR_EUTRA_CONNECTED_TO_5GCN;
      break;
    default:
      QCRIL_LOG_ERROR("%d is not a recognized value for isEmcSupported", rilNrVopsInfo.isEmcSupported);
      break;
  }

  switch (rilNrVopsInfo.isEmfSupported) {
    case RIL_Nr5gEmfVal::NR5G_EMF_NOT_SUPPORTED:
      aidlNrVopsInfo.emfSupported = aidlnetwork::NrVopsInfo::EMF_INDICATOR_NOT_SUPPORTED;
      break;
    case RIL_Nr5gEmfVal::NR5G_EMF_SUPPORTED_NR:
      aidlNrVopsInfo.emfSupported = aidlnetwork::NrVopsInfo::EMF_INDICATOR_NR_CONNECTED_TO_5GCN;
      break;
    case RIL_Nr5gEmfVal::NR5G_EMF_SUPPORTED_EUTRA:
      aidlNrVopsInfo.emfSupported = aidlnetwork::NrVopsInfo::EMF_INDICATOR_EUTRA_CONNECTED_TO_5GCN;
      break;
    case RIL_Nr5gEmfVal::NR5G_EMF_SUPPORTED_NR_AND_EUTRA:
      aidlNrVopsInfo.emfSupported = aidlnetwork::NrVopsInfo::EMF_INDICATOR_BOTH_NR_EUTRA_CONNECTED_TO_5GCN;
      break;
    default:
      QCRIL_LOG_ERROR("%d is not a recognized value for isEmfSupported", rilNrVopsInfo.isEmfSupported);
      break;
  }
}

void convert(const RIL_DataRegistrationStateResponse& rilDataRegState,
    aidlnetwork::RegStateResult& aidlRegState) {
  aidlRegState.regState = static_cast<aidlnetwork::RegState>(rilDataRegState.regState);
  aidlRegState.rat = static_cast<aidlradio::RadioTechnology>(rilDataRegState.rat);
  aidlRegState.reasonForDenial = static_cast<aidlnetwork::RegistrationFailCause>(rilDataRegState.reasonDataDenied);
  aidlRegState.registeredPlmn = getRegisteredPlmnFromCellIdentity(rilDataRegState.cellIdentity);

  convert(rilDataRegState.cellIdentity, aidlRegState.cellIdentity);

  if (rilDataRegState.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_LTE) {
    aidlnetwork::EutranRegistrationInfo eutranInfo {};

    if (rilDataRegState.lteVopsInfoValid) {
      eutranInfo.lteVopsInfo.isVopsSupported =
          static_cast<bool>(rilDataRegState.lteVopsInfo.isVopsSupported);
      eutranInfo.lteVopsInfo.isEmcBearerSupported =
          static_cast<bool>(rilDataRegState.lteVopsInfo.isEmcBearerSupported);
    }

    if (rilDataRegState.nrIndicatorsValid) {
      eutranInfo.nrIndicators.isEndcAvailable =
          static_cast<bool>(rilDataRegState.nrIndicators.isEndcAvailable);
      eutranInfo.nrIndicators.isDcNrRestricted =
          static_cast<bool>(rilDataRegState.nrIndicators.isDcNrRestricted);
      eutranInfo.nrIndicators.isNrAvailable =
          static_cast<bool>(rilDataRegState.nrIndicators.plmnInfoListR15Available);
    }

    if (rilDataRegState.lteVopsInfoValid || rilDataRegState.nrIndicatorsValid) {
      aidlRegState.accessTechnologySpecificInfo.set<aidlnetwork::AccessTechnologySpecificInfo::Tag::eutranInfo>(
         eutranInfo);
    }
  } else if (rilDataRegState.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_NR) {
    if (rilDataRegState.nrVopsInfoValid) {
      aidlnetwork::NrVopsInfo aildNrVopsInfo {};
      convert(rilDataRegState.nrVopsInfo, aildNrVopsInfo);
      aidlRegState.accessTechnologySpecificInfo.set<aidlnetwork::AccessTechnologySpecificInfo::Tag::ngranNrVopsInfo>(
         aildNrVopsInfo);
    }
  }
}

void convert(const RIL_VoiceRegistrationStateResponse& rilVoiceRegState,
    aidlnetwork::RegStateResult& aidlRegState) {
  aidlRegState.regState = static_cast<aidlnetwork::RegState>(rilVoiceRegState.regState);
  aidlRegState.rat = static_cast<aidlradio::RadioTechnology>(rilVoiceRegState.rat);
  aidlRegState.reasonForDenial = static_cast<aidlnetwork::RegistrationFailCause>(rilVoiceRegState.reasonForDenial);
  aidlRegState.registeredPlmn = getRegisteredPlmnFromCellIdentity(rilVoiceRegState.cellIdentity);

  convert(rilVoiceRegState.cellIdentity, aidlRegState.cellIdentity);

  if (rilVoiceRegState.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
    aidlnetwork::Cdma2000RegistrationInfo cdmaInfo;
    cdmaInfo.cssSupported = static_cast<bool>(rilVoiceRegState.cssSupported);
    cdmaInfo.roamingIndicator = rilVoiceRegState.roamingIndicator;
    cdmaInfo.systemIsInPrl = rilVoiceRegState.systemIsInPrl;
    cdmaInfo.defaultRoamingIndicator = rilVoiceRegState.defaultRoamingIndicator;
    aidlRegState.accessTechnologySpecificInfo.set<aidlnetwork::AccessTechnologySpecificInfo::Tag::cdmaInfo>(
        cdmaInfo);
  } else if (rilVoiceRegState.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_GSM) {
    if (rilVoiceRegState.geranDtmSupported != -1) {
      aidlRegState.accessTechnologySpecificInfo.set<aidlnetwork::AccessTechnologySpecificInfo::Tag::geranDtmSupported>(
          static_cast<bool>(rilVoiceRegState.geranDtmSupported));
    } else {
      aidlRegState.accessTechnologySpecificInfo.set<aidlnetwork::AccessTechnologySpecificInfo::Tag::geranDtmSupported>(
          false);
    }
  }
}

void convert(const qcril::interfaces::RilBarringServiceType& rilBarringServiceType,
    int32_t& aidlBarringServiceType) {
  if (qcril::interfaces::RilBarringServiceType::CS_SERVICE == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_CS_SERVICE;
  } else if (qcril::interfaces::RilBarringServiceType::PS_SERVICE == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_PS_SERVICE;
  } else if (qcril::interfaces::RilBarringServiceType::CS_VOICE == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_CS_VOICE;
  } else if (qcril::interfaces::RilBarringServiceType::MO_SIGNALLING == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_MO_SIGNALLING;
  } else if (qcril::interfaces::RilBarringServiceType::MO_DATA == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_MO_DATA;
  } else if (qcril::interfaces::RilBarringServiceType::CS_FALLBACK == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_CS_FALLBACK;
  } else if (qcril::interfaces::RilBarringServiceType::MMTEL_VOICE == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_MMTEL_VOICE;
  } else if (qcril::interfaces::RilBarringServiceType::MMTEL_VIDEO == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_MMTEL_VIDEO;
  } else if (qcril::interfaces::RilBarringServiceType::EMERGENCY == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_EMERGENCY;
  } else if (qcril::interfaces::RilBarringServiceType::SMS == rilBarringServiceType) {
    aidlBarringServiceType = aidlnetwork::BarringInfo::SERVICE_TYPE_SMS;
  } else {
    QCRIL_LOG_DEBUG("Unrecognized or operator specific barring service type: %d",
        rilBarringServiceType);
    aidlBarringServiceType = static_cast<int32_t>(rilBarringServiceType);
  }
}

void convert(const qcril::interfaces::RilBarringType& rilBarringType,
    int32_t& aidlBarringType) {
  if (qcril::interfaces::RilBarringType::NONE == rilBarringType) {
    aidlBarringType = aidlnetwork::BarringInfo::BARRING_TYPE_NONE;
  } else if (qcril::interfaces::RilBarringType::CONDITIONAL == rilBarringType) {
    aidlBarringType = aidlnetwork::BarringInfo::BARRING_TYPE_CONDITIONAL;
  } else if (qcril::interfaces::RilBarringType::UNCONDITIONAL == rilBarringType) {
    aidlBarringType = aidlnetwork::BarringInfo::BARRING_TYPE_UNCONDITIONAL;
  }
}

void convert(const qcril::interfaces::RILBarringInfo& rilBarringInfo,
    aidlnetwork::BarringInfo& aidlBarringInfo) {
  convert(rilBarringInfo.barringService, aidlBarringInfo.serviceType);
  convert(rilBarringInfo.barringType, aidlBarringInfo.barringType);

  aidlnetwork::BarringTypeSpecificInfo barringTypeSpecificInfo {};
  barringTypeSpecificInfo.factor = rilBarringInfo.typeSpecificInfo.barringFactor;
  barringTypeSpecificInfo.timeSeconds = rilBarringInfo.typeSpecificInfo.barringTimeSeconds;
  barringTypeSpecificInfo.isBarred = rilBarringInfo.typeSpecificInfo.isBarred;
  aidlBarringInfo.barringTypeSpecificInfo = barringTypeSpecificInfo;
}

void convert(const std::vector<qcril::interfaces::RILBarringInfo>& rilBarringInfos,
    std::vector<aidlnetwork::BarringInfo>& aidlBarringInfos) {
  size_t count = rilBarringInfos.size();
  aidlBarringInfos.resize(count);

  QCRIL_LOG_INFO("count: %zu", count);
  for (size_t i = 0; i < count; i++) {
    QCRIL_LOG_INFO(
        "barring info: %zu, serviceType: %d, barringType: %d, barringFactor: %d, "
        "barringTimeSeconds: %d, isBarred: %d",
        i, rilBarringInfos[i].barringService, rilBarringInfos[i].barringType,
        rilBarringInfos[i].typeSpecificInfo.barringFactor,
        rilBarringInfos[i].typeSpecificInfo.barringTimeSeconds,
        rilBarringInfos[i].typeSpecificInfo.isBarred);
    convert(rilBarringInfos[i], aidlBarringInfos[i]);
  }
}

bool checkThresholdAndHysteresis(int32_t hysMs, int32_t hysDb,
    const std::vector<int32_t>& thresholdsDbm) {
  QCRIL_LOG_FUNC_ENTRY();
  bool ret = true;
  // hysteresisDb must be smaller than the smallest threshold delta.
  int size = thresholdsDbm.size();
  int diff = INT_MAX;
  for (int i = 0; i < size - 1; i++) {
    if (abs(thresholdsDbm[i + 1] - thresholdsDbm[i]) < diff) {
      diff = abs(thresholdsDbm[i + 1] - thresholdsDbm[i]);
    }
  }
  if (hysMs < 0 && hysDb < 0 && size == 0) {
    QCRIL_LOG_DEBUG("Invalid arguments");
    ret = false;
  }
  // hysteresisDb value of 0 is valid. It disables hysteresis.
  if((hysDb != 0) && (diff <= hysDb)) {
    ret = false;
    QCRIL_LOG_DEBUG("Invalid arguments passed, hysteresisDb:%d"
      " smaller than smallest threshold delta %d", hysDb, diff);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
}

RIL_Errno sanityCheckSignalStrengthCriteriaParams(
    const std::vector<aidlnetwork::SignalThresholdInfo>& aidlCriteria){
  int size = aidlCriteria.size();
  for (int i = 0; i < size; i++) {
    if(!checkThresholdAndHysteresis(aidlCriteria[i].hysteresisMs,
      aidlCriteria[i].hysteresisDb, aidlCriteria[i].thresholds)) {
        QCRIL_LOG_DEBUG("Invalid args for RAT : %s",
          aidlradio::toString(aidlCriteria[i].ran).c_str());
        return RIL_E_INVALID_ARGUMENTS;
    }
  }
  return RIL_E_SUCCESS;
}

RIL_Errno convertAidlUsageSettingtoRil(const aidlnetwork::UsageSetting& aidlUSetting,
    RIL_UsageSettingMode& rilUSetting) {
  switch(aidlUSetting) {
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

RIL_SignalMeasureType convertAidlSigMTypetoRilSigMType(int32_t Aidl_signalMType){
  switch (Aidl_signalMType) {
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_RSSI:
      return RIL_SignalMeasureType::MT_RSSI;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_RSCP:
      return RIL_SignalMeasureType::MT_RSCP;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_RSRP:
      return RIL_SignalMeasureType::MT_RSRP;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_RSRQ:
      return RIL_SignalMeasureType::MT_RSRQ;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_RSSNR:
      return RIL_SignalMeasureType::MT_RSSNR;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_SSRSRP:
      return RIL_SignalMeasureType::MT_SSRSRP;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_SSRSRQ:
      return RIL_SignalMeasureType::MT_SSRSRQ;
    case aidlnetwork::SignalThresholdInfo::SIGNAL_MEASUREMENT_TYPE_SSSINR:
      return RIL_SignalMeasureType::MT_SSSINR;
    default:
      return RIL_SignalMeasureType::MT_UNKNOWN;
  }
}

std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> fillInSignalStrengthCriteria(
    const std::vector<aidlnetwork::SignalThresholdInfo>& aidlCriteria){
  int size = aidlCriteria.size();
  std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> rilCriteria{};
  for (int i=0; i < size; i++) {
    RIL_RadioAccessNetworks ril_ran = convertAidlRanToRilRan(aidlCriteria[i].ran);
    RIL_SignalMeasureType ril_sigType = convertAidlSigMTypetoRilSigMType(aidlCriteria[i].signalMeasurement);
    rilCriteria.push_back({ril_ran, ril_sigType, aidlCriteria[i].hysteresisMs,
      aidlCriteria[i].hysteresisDb, aidlCriteria[i].thresholds, aidlCriteria[i].isEnabled});
  }
  return rilCriteria;
}

RIL_Errno convertRilUsageSettingtoAidlUsageSetting(
  const RIL_UsageSettingMode& rilMode, aidlnetwork::UsageSetting& aidlMode) {
  switch (rilMode) {
    case RIL_UsageSettingMode::RIL_VOICE_CENTRIC:
      aidlMode = aidlnetwork::UsageSetting::VOICE_CENTRIC;
      return RIL_E_SUCCESS;
    case RIL_UsageSettingMode::RIL_DATA_CENTRIC:
      aidlMode = aidlnetwork::UsageSetting::DATA_CENTRIC;
      return RIL_E_SUCCESS;
    //UKNOWN case not defined in AIDL, so returning INVALID_STATE error
    case RIL_UsageSettingMode::RIL_UNKNOWN_USAGE_SETTING:
      QCRIL_LOG_ERROR(" UKNOWN usage setting received from modem");
      return RIL_E_INVALID_STATE;
    default:
      QCRIL_LOG_ERROR(" Invalid usage setting received from modem");
      return RIL_E_INVALID_STATE;
  }
}

aidlradio::AccessNetwork convertRadioAccessNetworksToAidl(RIL_RadioAccessNetworks in) {
  switch (in) {
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
    default:
      return aidlradio::AccessNetwork::UNKNOWN;
  }
}
RIL_Errno convertRadioAccessSpecifierToAidl(aidlnetwork::RadioAccessSpecifier& out,
                                            const RIL_RadioAccessSpecifier& in) {
  out.accessNetwork = convertRadioAccessNetworksToAidl(in.radio_access_network);
  out.channels.resize(in.channels_length);

  std::vector<aidlnetwork::GeranBands> gbands;
  std::vector<aidlnetwork::UtranBands> ubands;
  std::vector<aidlnetwork::EutranBands> eubands;
  std::vector<aidlnetwork::NgranBands> ngbands;
  switch (in.radio_access_network) {
    case RIL_RadioAccessNetworks::GERAN:
      gbands.resize(in.bands_length);
      for (size_t i = 0; i < in.bands_length; ++i) {
        gbands[i] = static_cast<aidlnetwork::GeranBands>(in.bands.geran_bands[i]);
      }
      out.bands.set<aidlnetwork::RadioAccessSpecifierBands::Tag::geranBands>(std::move(gbands));
      break;
    case RIL_RadioAccessNetworks::UTRAN:
      ubands.resize(in.bands_length);
      for (size_t i = 0; i < in.bands_length; ++i) {
        ubands[i] = static_cast<aidlnetwork::UtranBands>(in.bands.utran_bands[i]);
      }
      out.bands.set<aidlnetwork::RadioAccessSpecifierBands::Tag::utranBands>(std::move(ubands));
      break;
    case RIL_RadioAccessNetworks::EUTRAN:
      eubands.resize(in.bands_length);
      for (size_t i = 0; i < in.bands_length; ++i) {
        eubands[i] = static_cast<aidlnetwork::EutranBands>(in.bands.eutran_bands[i]);
      }
      out.bands.set<aidlnetwork::RadioAccessSpecifierBands::Tag::eutranBands>(std::move(eubands));
      break;
    case RIL_RadioAccessNetworks::NGRAN:
      ngbands.resize(in.bands_length);
      for (size_t i = 0; i < in.bands_length; ++i) {
        ngbands[i] = static_cast<aidlnetwork::NgranBands>(in.bands.eutran_bands[i]);
      }
      out.bands.set<aidlnetwork::RadioAccessSpecifierBands::Tag::ngranBands>(std::move(ngbands));
      break;
    default:
      return RIL_E_INVALID_ARGUMENTS;
  }
  return RIL_E_SUCCESS;
}

RIL_Errno convertRadioAccessSpecifiersToAidl(std::vector<aidlnetwork::RadioAccessSpecifier>& out, const RIL_SysSelChannels& in) {
  for (size_t i = 0; i < in.specifiers_length; i++) {
    aidlnetwork::RadioAccessSpecifier ras{};
    RIL_Errno ret = convertRadioAccessSpecifierToAidl(ras, in.specifiers_latest[i]);
    if (ret == RIL_E_SUCCESS) {
      out.push_back(std::move(ras));
    }
  }
  return RIL_E_SUCCESS;
}

rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork(aidlradio::AccessNetwork ran) {
  rildata::AccessNetwork_t ret;
  switch (ran) {
    case aidlradio::AccessNetwork::GERAN:
      ret = rildata::AccessNetwork_t::GERAN;
      break;
    case aidlradio::AccessNetwork::UTRAN:
      ret = rildata::AccessNetwork_t::UTRAN;
      break;
    case aidlradio::AccessNetwork::EUTRAN:
      ret = rildata::AccessNetwork_t::EUTRAN;
      break;
    case aidlradio::AccessNetwork::CDMA2000:
      ret = rildata::AccessNetwork_t::CDMA;
      break;
    case aidlradio::AccessNetwork::IWLAN:
      ret = rildata::AccessNetwork_t::IWLAN;
      break;
    case aidlradio::AccessNetwork::NGRAN:
      ret = rildata::AccessNetwork_t::NGRAN;
      break;
    case aidlradio::AccessNetwork::UNKNOWN:
    default:
      ret = rildata::AccessNetwork_t::UNKNOWN;
      break;
  }
  return ret;
}

RIL_Errno convertLcResultToRilError(rildata::LinkCapCriteriaResult_t result) {
  RIL_Errno ret = RIL_Errno::RIL_E_SUCCESS;

  switch (result) {
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

aidlradio::RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e) {
  aidlradio::RadioError ret = aidlradio::RadioError::INTERNAL_ERR;
  switch (status) {
    case Message::Callback::Status::FAILURE:
    case Message::Callback::Status::SUCCESS:
      ret = (aidlradio::RadioError)e;
      break;
    case Message::Callback::Status::CANCELLED:
      ret = aidlradio::RadioError::CANCELLED;
      break;
    case Message::Callback::Status::NO_HANDLER_FOUND:
      ret = aidlradio::RadioError::REQUEST_NOT_SUPPORTED;
      break;
    default:
      ret = aidlradio::RadioError::INTERNAL_ERR;
      break;
  }
  return ret;
}


aidlradio::RadioTechnology convertRadioAccessFamilyTypeToRadioTechnology(rildata::RadioAccessFamily_t accessFamily)
{
  aidlradio::RadioTechnology radioTech;
  switch(accessFamily) {
    case rildata::RadioAccessFamily_t::GPRS:
        radioTech = aidlradio::RadioTechnology::GPRS;
        break;
    case rildata::RadioAccessFamily_t::EDGE:
        radioTech = aidlradio::RadioTechnology::EDGE;
        break;
    case rildata::RadioAccessFamily_t::UMTS:
        radioTech = aidlradio::RadioTechnology::UMTS;
        break;
    case rildata::RadioAccessFamily_t::IS95A:
        radioTech = aidlradio::RadioTechnology::IS95A;
        break;
    case rildata::RadioAccessFamily_t::IS95B:
        radioTech = aidlradio::RadioTechnology::IS95B;
        break;
    case rildata::RadioAccessFamily_t::ONE_X_RTT:
        radioTech = aidlradio::RadioTechnology::ONE_X_RTT;
        break;
    case rildata::RadioAccessFamily_t::EVDO_0:
        radioTech = aidlradio::RadioTechnology::EVDO_0;
        break;
    case rildata::RadioAccessFamily_t::EVDO_A:
        radioTech = aidlradio::RadioTechnology::EVDO_A;
        break;
    case rildata::RadioAccessFamily_t::HSDPA:
        radioTech = aidlradio::RadioTechnology::HSDPA;
        break;
    case rildata::RadioAccessFamily_t::HSUPA:
        radioTech = aidlradio::RadioTechnology::HSUPA;
        break;
    case rildata::RadioAccessFamily_t::HSPA:
        radioTech = aidlradio::RadioTechnology::HSPA;
        break;
    case rildata::RadioAccessFamily_t::EVDO_B:
        radioTech = aidlradio::RadioTechnology::EVDO_B;
        break;
    case rildata::RadioAccessFamily_t::EHRPD:
        radioTech = aidlradio::RadioTechnology::EHRPD;
        break;
    case rildata::RadioAccessFamily_t::LTE:
        radioTech = aidlradio::RadioTechnology::LTE;
        break;
    case rildata::RadioAccessFamily_t::HSPAP:
        radioTech = aidlradio::RadioTechnology::HSPAP;
        break;
    case rildata::RadioAccessFamily_t::GSM:
        radioTech = aidlradio::RadioTechnology::GSM;
        break;
    case rildata::RadioAccessFamily_t::TD_SCDMA:
        radioTech = aidlradio::RadioTechnology::TD_SCDMA;
        break;
    case rildata::RadioAccessFamily_t::LTE_CA:
        radioTech = aidlradio::RadioTechnology::LTE_CA;
        break;
    case rildata::RadioAccessFamily_t::NR:
        radioTech = aidlradio::RadioTechnology::NR;
        break;
    default:
        radioTech = aidlradio::RadioTechnology::UNKNOWN;
    }
    return radioTech;
}

RIL_RadioTechnology getRilRadioTechnologyFromRan(aidlradio::AccessNetwork ran) {
  switch (ran) {
    case aidlradio::AccessNetwork::GERAN:
      return RADIO_TECH_GSM;
    case aidlradio::AccessNetwork::UTRAN:
      return RADIO_TECH_UMTS;
    case aidlradio::AccessNetwork::EUTRAN:
      return RADIO_TECH_LTE;
    case aidlradio::AccessNetwork::NGRAN:
      return RADIO_TECH_5G;
    case aidlradio::AccessNetwork::CDMA2000:
      return RADIO_TECH_IS95A;
    default: {
      break;
    }
  }
  return RADIO_TECH_UNKNOWN;
}


}  // namespace utils
}  // namespace network
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
}  // namespace qti
