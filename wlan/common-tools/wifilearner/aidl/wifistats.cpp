/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/hardware/wifi/wifilearner/ChannelSurveyInfoMask.h>
#include <aidl/vendor/qti/hardware/wifi/wifilearner/IfaceStatsInfoMask.h>

#include "wifistats.h"

extern "C"
{

}

namespace {

using aidl::vendor::qti::hardware::wifi::wifilearner::MacAddress;
using aidl::vendor::qti::hardware::wifi::wifilearner::IfaceStats;
using aidl::vendor::qti::hardware::wifi::wifilearner::WifiLearnerStatus;
using aidl::vendor::qti::hardware::wifi::wifilearner::WifiLearnerStatusCode;

}  // namespace

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace wifi {
namespace wifilearner {
namespace implementation {

WifiStats::WifiStats(struct wifilearner_ctx* wlc) : wlc_(wlc)
{}

static void copyStats(IfaceStats *stats, station_info *data)
{
	if (data->filled & NL_STATS_HAS_TX_RATE) {
		stats->txRate = data->current_tx_rate;
		stats->rateMask |= (long) IfaceStatsInfoMask::STATS_HAS_TX_RATE;
	}
	if (data->filled & NL_STATS_HAS_RX_RATE) {
		stats->rxRate = data->current_rx_rate;
		stats->rateMask |= (long) IfaceStatsInfoMask::STATS_HAS_RX_RATE;
	}
}

::ndk::ScopedAStatus WifiStats::getPeerStats(const std::string& iface_name,
					     const MacAddress& peer_mac,
					     WifiLearnerStatus* out_status,
					     IfaceStats* out_stats)
{
	WifiLearnerStatus status;
	station_info sta_data;
	int ret;

	IfaceStats peer_stats = {};
	std::array<uint8_t, 6> mac = peer_mac.data;
	memset(&status, 0, sizeof(status));
	memset(&sta_data, 0, sizeof(sta_data));

	ret = get_iface_stats(wlc_, iface_name.c_str(), mac.data(), &sta_data);
	if (ret) {
		status.code = WifiLearnerStatusCode::FAILURE_UNKNOWN;
		return ndk::ScopedAStatus::fromServiceSpecificError(
			static_cast<int32_t>(status.code));
	}
	copyStats(&peer_stats, &sta_data);
	out_stats = &peer_stats;
	out_status = &status;

	return ::ndk::ScopedAStatus::ok();
}

static void copySurveyInfo(ChannelSurveyInfo *info, survey_info *data, uint32_t data_index)
{
	info->freq = data->freq[data_index];

	if (data->filled[data_index] & NL_SURVEY_HAS_CHAN_TIME) {
		info->channelTime = data->time[data_index];
		info->chanMask |= (int) ChannelSurveyInfoMask::SURVEY_HAS_CHAN_TIME;
	}
	if (data->filled[data_index] & NL_SURVEY_HAS_CHAN_TIME_BUSY) {
		info->channelTimeBusy = data->time_busy[data_index];
		info->chanMask |= (int) ChannelSurveyInfoMask::SURVEY_HAS_CHAN_TIME_BUSY;
	}
}

::ndk::ScopedAStatus WifiStats::getSurveyDumpResults(const std::string& iface_name,
						     WifiLearnerStatus* out_status,
						     std::vector<ChannelSurveyInfo>* out_surveyResults)
{
	std::vector<ChannelSurveyInfo> survey_results;
	survey_info survey_data;
	int ret;
	WifiLearnerStatus status;

	memset(&status, 0, sizeof(status));
	status.debugMessage = "";

	ret = get_survey_dump_results(wlc_, iface_name.c_str(), &survey_data);
	if (ret) {
		status.code = WifiLearnerStatusCode::FAILURE_UNKNOWN;
		return ndk::ScopedAStatus::fromServiceSpecificError(
			static_cast<int32_t>(status.code));
	}

	survey_results.resize(survey_data.count);
	ChannelSurveyInfo defaultSurvey = {};
	std::fill(survey_results.begin(), survey_results.end(), defaultSurvey);
	for (uint32_t i = 0; i < survey_data.count; i++)
		copySurveyInfo(&survey_results[i], &survey_data, i);

	out_status = &status;
	out_surveyResults = &survey_results;

	return ::ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace wifilearner
}  // namespace wifi
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  //namespace aidl
