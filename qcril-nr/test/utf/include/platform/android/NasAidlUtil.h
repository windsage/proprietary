/*===========================================================================
 *
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#pragma once

#include "ril_utf_ril_api.h"
#include "telephony/ril.h"
#include "aidl/android/hardware/radio/AccessNetwork.h"
#include "aidl/android/hardware/radio/network/RadioAccessSpecifier.h"
#include "aidl/android/hardware/radio/network/SignalStrength.h"
#include "aidl/android/hardware/radio/network/NetworkScanRequest.h"
#include "aidl/android/hardware/radio/network/OperatorInfo.h"
#include "aidl/android/hardware/radio/network/NetworkScanResult.h"
#include "aidl/android/hardware/radio/network/UsageSetting.h"
#include "aidl/vendor/qti/hardware/radio/qtiradioconfig/MsimPreference.h"
#include "aidl/vendor/qti/hardware/radio/qtiradio/NrConfig.h"
#include "aidl/android/hardware/radio/modem/RadioCapability.h"
#include "aidl/android/hardware/radio/modem/DeviceStateType.h"
#include "aidl/android/hardware/radio/modem/ActivityStatsInfo.h"

#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanRequest.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanResult.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NetworkSelectionMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanRequest.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/SetNetworkSelectionMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/AccessMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/CagInfo.h>
#include <aidl/android/hardware/radio/network/BarringInfo.h>



namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}

namespace aidlnetwork {
  using namespace aidl::android::hardware::radio::network;
}

namespace qtiradioaidl {
  using namespace aidl::vendor::qti::hardware::radio::qtiradio;
}

namespace aidlmodem {
  using namespace aidl::android::hardware::radio::modem;
}

namespace aidlqtiradioconfig {
  using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig;
}

std::vector<aidlnetwork::RadioAccessSpecifier> convertAidlRadioAccessSpecifier(
    const uint32_t len, const RIL_RadioAccessSpecifier in[MAX_RADIO_ACCESS_NETWORKS]);

std::vector<qtiradioaidl::RadioAccessSpecifier> convertAidlRadioAccessSpecifierQtiRadio(
    const uint32_t len, const RIL_RadioAccessSpecifier in[MAX_RADIO_ACCESS_NETWORKS]);

aidlradio::AccessNetwork convertRilNetworkTypeToAidl(RIL_RadioAccessNetworks rilType);
qtiradioaidl::AccessNetwork convertRilNetworkTypeToQtiRadioAidl(RIL_RadioAccessNetworks rilType);

void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest& in,
                                     aidlnetwork::NetworkScanRequest& out);

void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest& in,
                                     qtiradioaidl::NetworkScanRequest& out);
void convertNetworkScanRequestToAidl(const RIL_NetworkScanRequest& in,
                                     qtiradioaidl::QtiNetworkScanRequest& out);

ril_request_query_available_networks_resp_t* convertAidlNetworkscanResponseToRil(
    const std::vector<aidlnetwork::OperatorInfo>& networkInfos);

void convertRILDeviceStateToAidl(::aidlmodem::DeviceStateType &in_deviceStateType,
    const RIL_DeviceStateType &deviceState);

void convertNetworkScanResultToRil(
  const ::aidl::android::hardware::radio::network::NetworkScanResult& in,
  RIL_NetworkScanResult &out);

RIL_Errno convertAidlUsageSettingtoRil(const aidlnetwork::UsageSetting& aidlUSetting,
                                       RIL_UsageSettingMode& rilUSetting);

bool convertMsimPreference(const Msim_Preference from, aidlqtiradioconfig::MsimPreference& to);

void convertAidlStrengthToRil(const ::aidlnetwork::SignalStrength from, RIL_SignalStrength* to);

qtiradioaidl::NrConfig convert_nr_config_to_aidl(RIL_NR_DISABLE_MODE mode);

RIL_NR_DISABLE_MODE convert_nr_config_to_ril(qtiradioaidl::NrConfig config);

bool convertToAidlRadioCapability(const RIL_RadioCapability& from, aidlmodem::RadioCapability& to);

bool convertRadioCapabilityToRil(const aidlmodem::RadioCapability& from, RIL_RadioCapability& to);

void convertNetworkScanResultToRil(const qtiradioaidl::QtiNetworkScanResult& in,
                                   RIL_NetworkScanResult& out);

void convertToRil(const std::string& imei, const std::string& imeisv, const std::string& esn,
  const std::string& meid, ril_request_device_identity_type_t* out);

void convertAidlActivityStatsToRil(const aidlmodem::ActivityStatsInfo& from, RIL_ActivityStatsInfo& to);
void convertCellBarringInfoNr(const std::vector<::aidlnetwork::BarringInfo>& in, RIL_CellBarringInfo* out);
