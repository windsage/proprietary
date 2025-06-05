/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef WIFISTATS_AIDL_H
#define WIFISTATS_AIDL_H

#include <string>
#include <vector>

#include <android-base/macros.h>

#include <aidl/vendor/qti/hardware/wifi/wifilearner/BnWifiStats.h>
#include <aidl/vendor/qti/hardware/wifi/wifilearner/ChannelSurveyInfo.h>
#include <aidl/vendor/qti/hardware/wifi/wifilearner/IfaceStats.h>
#include <aidl/vendor/qti/hardware/wifi/wifilearner/MacAddress.h>
#include <aidl/vendor/qti/hardware/wifi/wifilearner/WifiLearnerStatus.h>

extern "C"
{
#include "wifilearner.h"
#include "nl_utils.h"
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace wifi {
namespace wifilearner {
namespace implementation {

class WifiStats : public BnWifiStats
{
public:
	WifiStats(struct wifilearner_ctx *wlc);
	~WifiStats() override = default;

	// Aidl methods exposed.
	::ndk::ScopedAStatus getPeerStats(const std::string& iface_name,
					  const MacAddress& peer_mac,
					  WifiLearnerStatus* out_status,
					  IfaceStats* out_stats) override;

	::ndk::ScopedAStatus getSurveyDumpResults(const std::string& iface_name,
						  WifiLearnerStatus* out_status,
						  std::vector<ChannelSurveyInfo>* out_surveyResults) override;

private:
	struct wifilearner_ctx* wlc_;
	DISALLOW_COPY_AND_ASSIGN(WifiStats);
};
}  // namespace implementation
}  // namespace wifilearner
}  // namespace wifi
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // WIFISTATS_AIDL_H
