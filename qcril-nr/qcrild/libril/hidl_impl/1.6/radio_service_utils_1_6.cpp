/******************************************************************************
#  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "hidl_impl/1.2/radio_service_utils_1_2.h"
#include "hidl_impl/1.4/radio_service_utils_1_4.h"
#include "hidl_impl/1.5/radio_service_utils_1_5.h"
#include "hidl_impl/1.6/radio_service_utils_1_6.h"
#include "hidl_impl/1.0/radio_service_utils_1_0.h"
#include "RadioServiceModule.h"
#include <optional>
#undef TAG
#define TAG "RILQ"

using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

namespace android {
namespace hardware {
namespace radio {
namespace utils {

void convertCellInfoRilToHidl(const RIL_CellInfo_v12& rilCellInfo, V1_6::CellInfo& hidlCellInfo) {
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
      V1_6::CellInfoLte cellInfoLte{};
      convertLteCellIdentityRilToHidl(rilCellInfo.CellInfo.lte.cellIdentityLte,
                                      cellInfoLte.cellIdentityLte);
      cellInfoLte.signalStrengthLte.base.signalStrength =
          rilCellInfo.CellInfo.lte.signalStrengthLte.signalStrength;
      cellInfoLte.signalStrengthLte.base.rsrp = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrp;
      cellInfoLte.signalStrengthLte.base.rsrq = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrq;
      cellInfoLte.signalStrengthLte.base.rssnr = rilCellInfo.CellInfo.lte.signalStrengthLte.rssnr;
      cellInfoLte.signalStrengthLte.base.cqi = rilCellInfo.CellInfo.lte.signalStrengthLte.cqi;
      cellInfoLte.signalStrengthLte.base.timingAdvance =
          rilCellInfo.CellInfo.lte.signalStrengthLte.timingAdvance;
      hidlCellInfo.ratSpecificInfo.lte(std::move(cellInfoLte));
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      V1_6::CellInfoNr cellInfoNr{};
      convertNrCellIdentityRilToHidl(rilCellInfo.CellInfo.nr.cellIdentityNr,
                                     cellInfoNr.cellIdentityNr);
      cellInfoNr.signalStrengthNr.base.ssRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrp;
      cellInfoNr.signalStrengthNr.base.ssRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrq;
      cellInfoNr.signalStrengthNr.base.ssSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.ssSinr;
      cellInfoNr.signalStrengthNr.base.csiRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrp;
      cellInfoNr.signalStrengthNr.base.csiRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrq;
      cellInfoNr.signalStrengthNr.base.csiSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.csiSinr;
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
                            hidl_vec<V1_6::CellInfo>& records) {
  auto num = rilCellInfo.size();
  records.resize(num);

  for (unsigned int i = 0; i < num; i++) {
    records[i].registered = rilCellInfo[i].registered;
    records[i].connectionStatus = (V1_2::CellConnectionStatus)rilCellInfo[i].connStatus;
    convertCellInfoRilToHidl(rilCellInfo[i], records[i]);
  }
}

V1_6::DataCallFailCause convertDcFailCauseToHidlDcFailCause_1_6(const rildata::DataCallFailCause_t &cause) {
    // TODO: add 1.6 specific errors
    switch (cause) {
      case rildata::DataCallFailCause_t::SLICE_REJECTED:
        return V1_6::DataCallFailCause::SLICE_REJECTED;
      case rildata::DataCallFailCause_t::MATCH_ALL_RULE_NOT_ALLOWED:
        return V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED;
      case rildata::DataCallFailCause_t::ALL_MATCHING_RULES_FAILED:
        return V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED;
    default:
        return static_cast<V1_6::DataCallFailCause>(convertDcFailCauseToHidlDcFailCause_1_4(cause));
    }
}

void convertQosSession(V1_6::QosSession& dcQosSession, const rildata::QosSession_t& result) {
  dcQosSession.qosSessionId = result.qosSessionId;
  if(result.qos.epsQos != std::nullopt) {
    V1_6::EpsQos eps;
    eps.qci = result.qos.epsQos->qci;
    eps.downlink.maxBitrateKbps = result.qos.epsQos->downlink.maxBitrateKbps;
    eps.downlink.guaranteedBitrateKbps = result.qos.epsQos->downlink.guaranteedBitrateKbps;
    eps.uplink.maxBitrateKbps = result.qos.epsQos->uplink.maxBitrateKbps;
    eps.uplink.guaranteedBitrateKbps = result.qos.epsQos->uplink.guaranteedBitrateKbps;
    dcQosSession.qos.eps(eps);
  }
  if(result.qos.nrQos != std::nullopt) {
    V1_6::NrQos nr;
    nr.fiveQi = result.qos.nrQos->fiveQi;
    nr.downlink.maxBitrateKbps = result.qos.nrQos->downlink.maxBitrateKbps;
    nr.downlink.guaranteedBitrateKbps = result.qos.nrQos->downlink.guaranteedBitrateKbps;
    nr.uplink.maxBitrateKbps = result.qos.nrQos->uplink.maxBitrateKbps;
    nr.uplink.guaranteedBitrateKbps = result.qos.nrQos->uplink.guaranteedBitrateKbps;
    nr.qfi = result.qos.nrQos->qfi;
    nr.averagingWindowMs = result.qos.nrQos->averagingWindowMs;
    dcQosSession.qos.nr(nr);
  }

  unsigned int qosFilterLen = result.qosFilters.size();
  if(qosFilterLen > 0 ) {
    dcQosSession.qosFilters.resize(qosFilterLen);
    for(int j=0; j< qosFilterLen; j++) {
      convertQosFilters(dcQosSession.qosFilters[j], result.qosFilters[j]);
    }
  }
}

void convertQosFilters(V1_6::QosFilter& dcQosFilter, const rildata::QosFilter_t& result) {
  unsigned int localAddressLen = result.localAddresses.size();
  if(localAddressLen > 0) {
    dcQosFilter.localAddresses.resize(localAddressLen);
    for(int i =0; i< localAddressLen; i++) {
      dcQosFilter.localAddresses[i]=result.localAddresses[i];
    }
  }

  unsigned int remoteAddressesLen = result.remoteAddresses.size();
  if(remoteAddressesLen > 0) {
    dcQosFilter.remoteAddresses.resize(remoteAddressesLen);
    for(int i =0; i< remoteAddressesLen; i++) {
      dcQosFilter.remoteAddresses[i]=result.remoteAddresses[i];
    }
  }
  if(result.localPort != std::nullopt) {
    V1_6::PortRange range;
    range.start = result.localPort->start;
    range.end = result.localPort->end;
    dcQosFilter.localPort.range(range);
  }
  if(result.remotePort != std::nullopt) {
    V1_6::PortRange range;
    range.start = result.remotePort->start;
    range.end = result.remotePort->end;
    dcQosFilter.remotePort.range(range);
  }
  dcQosFilter.protocol = (V1_6::QosProtocol)result.protocol;
  if(result.tos.value != std::nullopt) {
    dcQosFilter.tos.value(*result.tos.value);
  }

  if(result.flowLabel.value != std::nullopt) {
    dcQosFilter.flowLabel.value(*result.flowLabel.value);
  }

  if(result.spi.value != std::nullopt) {
    dcQosFilter.spi.value(*result.spi.value);
  }

  dcQosFilter.direction = (V1_6::QosFilterDirection )result.direction;
  dcQosFilter.precedence= result.precedence;
}

void convertTrafficDescriptor(V1_6::TrafficDescriptor& trafficDescriptor, const rildata::TrafficDescriptor_t& result)
{
  if (result.dnn.has_value()) {
    trafficDescriptor.dnn.value(result.dnn.value());
  }
  if (result.osAppId.has_value()) {
    V1_6::OsAppId id;
    id.osAppId = result.osAppId.value();
    trafficDescriptor.osAppId.value(id);
  }
}

V1_6::SetupDataCallResult convertDcResultToHidlDcResult_1_6(const rildata::DataCallResult_t& result) {
    V1_6::SetupDataCallResult dcResult = {};
    dcResult.cause = convertDcFailCauseToHidlDcFailCause_1_6(result.cause);
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
    dcResult.handoverFailureMode = (V1_6::HandoverFailureMode)result.handoverFailureMode;
    unsigned int qosSessionLen = result.qosSessions.size();
    if(qosSessionLen > 0) {
      dcResult.qosSessions.resize(qosSessionLen);
      for(int i = 0; i< qosSessionLen; i++)
      {
        convertQosSession(dcResult.qosSessions[i], result.qosSessions[i]);
      }

    }
    unsigned int tdSize = result.trafficDescriptors.size();
    if(tdSize > 0) {
      dcResult.trafficDescriptors.resize(tdSize);
      for(int i = 0; i< tdSize; i++) {
        convertTrafficDescriptor(dcResult.trafficDescriptors[i], result.trafficDescriptors[i]);
      }
    }

    return dcResult;
}

void fillVoiceRegistrationStateResponse(V1_6::RegStateResult& out,
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
  } else if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_GSM) {
    if (in.geranDtmSupported != -1) {
      out.accessTechnologySpecificInfo.geranDtmSupported(static_cast<bool>(in.geranDtmSupported));
    } else {
      out.accessTechnologySpecificInfo.geranDtmSupported(false);
    }
  } else {
    out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
                                     string(in.cellIdentity.cellIdentityGsm.mnc));
  }
}

void fillDataRegistrationStateResponse(V1_6::RegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in) {
  out.regState = static_cast<V1_0::RegState>(in.regState);
  out.rat = static_cast<V1_4::RadioTechnology>(in.rat);

  if (in.cellIdentity.cellInfoType != RIL_CELL_INFO_TYPE_CDMA) {
    out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
                                     string(in.cellIdentity.cellIdentityGsm.mnc));
  }

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
  else if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_NR) {
    if (in.nrVopsInfoValid) {
      V1_6::NrVopsInfo info;
      info.vopsSupported = static_cast<V1_6::VopsIndicator>(in.nrVopsInfo.isVopsSupported);
      info.emcSupported = static_cast<V1_6::EmcIndicator>(in.nrVopsInfo.isEmcSupported);
      info.emfSupported = static_cast<V1_6::EmfIndicator>(in.nrVopsInfo.isEmfSupported);
      out.accessTechnologySpecificInfo.ngranNrVopsInfo(std::move(info));
    }
  }
}

int convertToHidl(V1_6::Call& out, const qcril::interfaces::CallInfo& in) {
  convertToHidl(out.base, in);
  if (in.hasRedirNum()) {
    out.forwardedNumber = in.getRedirNum();
  }
  return 0;
}

int convertToHal(V1_6::NrSignalStrength& out, const RIL_NR_SignalStrength& in) {
  convertToHal(out.base, in);
  out.csiCqiTableIndex = INT_MAX;
  return 0;
}

int convertToHal(V1_6::LteSignalStrength& out, RIL_LTE_SignalStrength_v8 in) {
  convertToHal(out.base, in);
  out.cqiTableIndex = INT_MAX;
  return 0;
}

void convertRilSignalStrengthToHal(const RIL_SignalStrength& rilSignalStrength,
        V1_6::SignalStrength& signalStrength) {
  convertToHal(signalStrength.gsm, rilSignalStrength.GW_SignalStrength);
  convertToHal(signalStrength.cdma, rilSignalStrength.CDMA_SignalStrength);
  convertToHal(signalStrength.evdo, rilSignalStrength.EVDO_SignalStrength);
  convertToHal(signalStrength.lte, rilSignalStrength.LTE_SignalStrength);
  convertToHal(signalStrength.tdscdma, rilSignalStrength.TD_SCDMA_SignalStrength);
  convertToHal(signalStrength.wcdma, rilSignalStrength.WCDMA_SignalStrength);
  convertToHal(signalStrength.nr, rilSignalStrength.NR_SignalStrength);
}

rildata::DataThrottleAction_t convertHidlDataThrottleActionToRil
                              (const V1_6::DataThrottlingAction& dataThrottlingAction) {
  rildata::DataThrottleAction_t action;
  switch (dataThrottlingAction) {
      case V1_6::DataThrottlingAction::NO_DATA_THROTTLING:
          action = rildata::DataThrottleAction_t::NoDataThrottle;
          break;
      case V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER:
          action = rildata::DataThrottleAction_t::ThrottleSecondaryCarrier;
          break;
      case V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER:
          action = rildata::DataThrottleAction_t::ThrottleAnchorCarrier;
          break;
      case V1_6::DataThrottlingAction::HOLD:
      default:
          action = rildata::DataThrottleAction_t::Hold;
          break;
  }
  return action;
}

void convertRilPbCapacity(const std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> &in,
        V1_6::PhonebookCapacity &out) {
    out.maxAdnRecords = in->max_adn_num;
    out.usedAdnRecords = in->valid_adn_num;
    out.maxEmailRecords = in->max_email_num;
    out.usedEmailRecords = in->valid_email_num;
    out.maxAdditionalNumberRecords = in->max_ad_num;
    out.usedAdditionalNumberRecords = in->valid_ad_num;
    out.maxNameLen = in->max_name_len;
    out.maxNumberLen = in->max_number_len;
    out.maxEmailLen = in->max_email_len;
    out.maxAdditionalNumberLen = in->max_anr_len;
}

void convertHidlPhonebookRecords(const V1_6::PhonebookRecordInfo& in, qcril::interfaces::AdnRecordInfo& out) {
    uint16_t email_index, ad_index;
    out.record_id = in.recordId;
    out.name = in.name;
    out.number = in.number;
    out.email_elements = in.emails.size();
    for(email_index = 0; email_index < in.emails.size(); email_index++) {
        out.email[email_index] = in.emails[email_index];
    }
    out.anr_elements = in.additionalNumbers.size();
    for(ad_index = 0; ad_index < in.additionalNumbers.size(); ad_index++) {
        out.ad_number[ad_index] = in.additionalNumbers[ad_index];
    }
}

void convertRilPhonebookRecords(const qcril::interfaces::AdnRecords* in, hidl_vec<V1_6::PhonebookRecordInfo> &out) {
    uint16_t index, email_index, ad_index;
    out.resize(in->record_elements);
    for (index = 0; index < in->record_elements; index++) {
        const qcril::interfaces::AdnRecordInfo* adnRecordInfo = &in->adn_record_info[index];
        if (adnRecordInfo != nullptr) {
            out[index].recordId = adnRecordInfo->record_id;
            out[index].name = adnRecordInfo->name;
            out[index].number = adnRecordInfo->number;
            if (adnRecordInfo->email_elements > 0)
            {
                out[index].emails.resize(adnRecordInfo->email_elements);
                for (email_index = 0; email_index < adnRecordInfo->email_elements; email_index++)
                {
                    out[index].emails[email_index] = adnRecordInfo->email[email_index];
                }
            }

            if (adnRecordInfo->anr_elements > 0)
            {
                out[index].additionalNumbers.resize(adnRecordInfo->anr_elements);
                for (ad_index = 0; ad_index < adnRecordInfo->anr_elements; ad_index++)
                {
                    out[index].additionalNumbers[ad_index] = adnRecordInfo->ad_number[ad_index];
                }
            }
        }

    }
}

V1_6::PbReceivedStatus convertRilPbReceivedStatus(int seq_num) {
    V1_6::PbReceivedStatus status = V1_6::PbReceivedStatus::PB_RECEIVED_ERROR;
    if (seq_num == 65535) {
        status = V1_6::PbReceivedStatus::PB_RECEIVED_FINAL;
    } else if (seq_num > 0) {
        status = V1_6::PbReceivedStatus::PB_RECEIVED_OK;
    }
    return status;
}
}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
