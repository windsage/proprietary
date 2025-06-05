/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_RADIO_STABLE_AIDL_SERVICE_UTILS_H_
#define _QTI_RADIO_STABLE_AIDL_SERVICE_UTILS_H_

#include <telephony/ril.h>
#include "interfaces/nas/nas_types.h"
#include "interfaces/dms/dms_types.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/voice/voice.h"
#include "interfaces/dms/QcRilUnsolImeiMessage.h"
#include "framework/Log.h"

#ifndef QMI_RIL_UTF
#include "request/GetDataNrIconTypeMessage.h"
#include "request/SetNrUCIconConfigureMessage.h"
#include "request/RegisterDataDuringVoiceMessage.h"
#include "request/GetSmartTempDdsSwitchCapabilityMessage.h"
#endif
#include "UnSolMessages/DataNrIconTypeIndMessage.h"
#include "UnSolMessages/AutoDdsSwitchControlIndMessage.h"
#include "UnSolMessages/DdsSwitchRecommendationIndMessage.h"
#include "qcril_config.h"

#include <aidl/vendor/qti/hardware/radio/qtiradio/ImeiType.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrIconType.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/EndcStatus.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrConfig.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/CallForwardInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/ImeiInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/McfgRefreshState.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconBandInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconRefreshTime.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconBandwidthInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanRequest.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiCellInfoRatSpecificInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiCellInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/SnpnInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/CagInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/AccessMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/SearchType.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/CiwlanMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/PersoUnlockStatus.h>

#include "interfaces/mbn/mbn.h"

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::qtiradio;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace utils {

#ifndef QMI_RIL_UTF
NrIconType convert_nrIconType_from_response(std::shared_ptr<rildata::NrIconType_t> resp);
NrIconType convert_nrIconType_from_indication(std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg);

bool convertSmartDdsSwitchCapabilityFromResponse(std::shared_ptr<rildata::SmartDdsSwitchCapability_t> resp);
rildata::DdsSwitchUserSelection_t convertUserPeferenceForDataDuringVoiceCall(bool userPreference);
rildata::SubscriptionId_t convertQcRilInstanceId(qcril_instance_id_e_type instanceId);

void convertNrUCIconBandInfo(rildata::NrUCIconBandInfo_t& out,
    const aidlimports::NrUwbIconBandInfo &in);
void convertNrUCIconBandwidthInfo(rildata::NrUCIconBandwidthInfo_t& out,
    const aidlimports::NrUwbIconBandwidthInfo& in);
void convertNrUCIconRefreshTime(rildata::NrUCIconRefreshTime_t& out,
    const aidlimports::NrUwbIconRefreshTime& in);
rildata::NrUCIconRefreshTimer_t convertNrUCIconRefreshTimer(
    const aidlimports::NrUwbIconRefreshTimerType &in);
#endif

EndcStatus convert_endc_status(
    std::shared_ptr<qcril::interfaces::RilQueryEndcStatusResult_t> payload);
NrConfig convert_nr_disable_mode(RIL_NR_DISABLE_MODE mode);
RIL_NR_DISABLE_MODE convert_nr_config(NrConfig config);
bool convertCallForwardInfo(aidlimports::CallForwardInfo& out,
                            const qcril::interfaces::CallForwardInfo& in);
bool convertCallForwardInfoList(std::vector<aidlimports::CallForwardInfo>& out,
                                const std::vector<qcril::interfaces::CallForwardInfo>& in);
qcril::interfaces::FacilityType convertFacilityType(std::string in);
void convert_imei_info(aidlimports::ImeiInfo& out,
    const std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t>& in);
void convert_imei_info(aidlimports::ImeiInfo& out,
    const qcril::interfaces::RilGetImeiInfoResult_t& in);

property_id_type getPropertyId(std::string prop);
McfgRefreshState convertPdcRefreshToAidlMcfgRefresh(qcril::interfaces::PdcRefreshEvent event);

RIL_Errno fillNetworkScanRequest(const aidlimports::QtiNetworkScanRequest& request,
    RIL_NetworkScanRequest& scanRequest);
RIL_Errno rasArgsCheck(const std::vector<aidlimports::RadioAccessSpecifier>& specifiers);
RIL_Errno convertAidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
                                 const aidlimports::RadioAccessSpecifier& ras_from);
void convert(const std::vector<RIL_CellInfo_v12>& rilCellInfos,
    std::vector<aidlimports::QtiCellInfo>& aidlCellInfos);
void convert(const RIL_CellInfo_v12& rilCellInfo, aidlimports::QtiCellInfo& aidlCellInfo);
template <typename T>
bool getAidlPlmnFromCellIdentity(std::string& mcc, std::string& mnc, const T& cellIdentity);
template <typename T>
void convert(const T& rilCellIdentity, aidlimports::OperatorInfo& aidlOperatorInfo);
template <>
bool getAidlPlmnFromCellIdentity<RIL_CellIdentityCdma>(std::string& mcc, std::string& mnc,
    const RIL_CellIdentityCdma& cellIdentity);
RIL_AccessMode convertAccessMode(aidlimports::AccessMode accessMode);
RIL_SearchType convertSearchType(aidlimports::SearchType searchType);
aidlimports::AccessMode convertAccessModeFromRil(RIL_AccessMode ril_access_mode);
RIL_RadioAccessNetworks convertAidlRanToRilRan(aidlimports::AccessNetwork ran);
aidlimports::SignalQuality convertSignalQualityFromRil(RIL_SignalQuality ril_signal_quality);
RIL_RadioTechnology getRilRadioTechnologyFromRan(aidlimports::AccessNetwork ran);
aidlimports::CiwlanMode convertCiwlanModeFromRil(qcril::interfaces::ImsCiWlanMode mode);
aidlimports::PersoUnlockStatus convertRilPersolockStatus(RIL_UIM_Perso_lock_Status status);

/**************************************** GSM  *****************************************/
void convert(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
    aidlimports::CellIdentityGsm& aidlGsmCellIdentity);
void convert(const RIL_GSM_SignalStrength_v12& rilGsmSignalStrength,
    aidlimports::GsmSignalStrength& aidlGsmSignalStrength);
void convert(const RIL_CellInfoGsm_v12& rilGsmCellInfo,
      aidlimports::CellInfoGsm& aidlGsmCellInfo);
/**************************************** GSM  *****************************************/

/**************************************** WCDMA  ***************************************/
void convert(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
    aidlimports::CellIdentityWcdma& aidlWcdmaCellIdentity);
void convert(const RIL_SignalStrengthWcdma& rilWcdmaSignalStrength,
    aidlimports::WcdmaSignalStrength& aidlWcdmaSignalStrength);
void convert(const RIL_CellInfoWcdma_v12& rilWcdmaCellInfo,
    aidlimports::CellInfoWcdma& aidlWcdmaCellInfo);
/**************************************** WCDMA  ***************************************/

/**************************************** CDMA  ****************************************/
void convert(const RIL_CDMA_SignalStrength& rilCdmaSignalStrength,
    aidlimports::CdmaSignalStrength& aidlCdmaSignalStrength);
void convert(const RIL_EVDO_SignalStrength& rilEvdoSignalStrength,
    aidlimports::EvdoSignalStrength& aidlEvdoSignalStrength);
void convert(const RIL_CellInfoCdma& rilCdmaCellInfo,
    aidlimports::CellInfoCdma& aidlCdmaCellInfo);
/**************************************** CDMA  ****************************************/

/**************************************** TDSCDMA  *************************************/
void convert(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
    aidlimports::CellIdentityTdscdma& aidlTdscdmaCellIdentity);
void convert(const RIL_TD_SCDMA_SignalStrength& rilTdscdmaSignalStrength,
    aidlimports::TdscdmaSignalStrength& aidlTdscdmaSignalStrength);
void convert(const RIL_CellInfoTdscdma& rilTdscdmaCellInfo,
    aidlimports::CellInfoTdscdma& aidlTdscdmaCellInfo);
/**************************************** TDSCDMA  *************************************/

/**************************************** LTE  *****************************************/
aidlimports::EutranBands mapEarfcnToEutranBand(int earfcn);
void convert(const RIL_CellInfoLte_v12& rilLteCellInfo,
     aidlimports::CellInfoLte& aidlLteCellInfo);
void convert(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
    aidlimports::CellIdentityLte& aidlLteCellIdentity);
void convert(const RIL_LTE_SignalStrength_v8& rilLteSignalStrength,
    aidlimports::LteSignalStrength& aidlLteSignalStrength);
/**************************************** LTE  *****************************************/

/**************************************** NR  ******************************************/
void convert(const RIL_CellInfoNr& rilNrCellInfo,
    aidlimports::QtiCellInfoNr& aidlNrCellInfo);
void convert(const RIL_CellIdentityNr& rilNrCellIdentity,
    aidlimports::QtiCellIdentityNr& cellIdentityNr);
void convert(const RIL_NR_SignalStrength& rilNrSignalStrength,
  aidlimports::NrSignalStrength& aidlNrSignalStrength);
void convert(const RIL_SnpnInfo& rilSnpnInfo, aidlimports::SnpnInfo& snpnInfo);
void convert(const RIL_CagInfo& rilCagInfo, aidlimports::CagInfo& cagInfo);
/**************************************** NR  ******************************************/
}  // namespace utils
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  //_QTI_RADIO_STABLE_AIDL_SERVICE_UTILS_H_
