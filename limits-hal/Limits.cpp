/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "Limits.h"
#include "LimitsEstimate.h"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

namespace vendor::qti::hardware::limits::implementation {

using ::android::hardware::interfacesEqual;

template <typename A>
Return<void> exit_hal(A _cb, std::string_view _msg) {
	ThermalStatus _status;

	_status.code = ThermalStatusCode::FAILURE;
	_status.debugMessage = _msg.data();
	LOG(ERROR) << _msg;
	_cb(_status);

	return Void();
}

Limits::Limits(): le(),
	lc(std::bind(&Limits::sendThrottlingChangeCB, this,
				std::placeholders::_1))
{
	int ret = 0;
	ret = lc.initCdev();
	if (!ret)
		LOG(ERROR) << "Cdev init failed" << std::endl;
}

// Methods from ::vendor::qti::hardware::limits::V1_0::ILimits follow.
Return<void> Limits::getThermalHeadRoom(getThermalHeadRoom_cb _hidl_cb) {
	float hr = 0;
	ThermalStatus status;
	status.code = ThermalStatusCode::SUCCESS;

	hr = le.getHeadRoom();
	if (hr < 0.0) {
		status.code = ThermalStatusCode::FAILURE;
		status.debugMessage = "Error getting Head Room.";
		LOG(ERROR) << "Error getting Head Room.";
	}

	_hidl_cb(status, hr);

    return Void();
}

Return<void> Limits::startHeadRoomCalculation() {

	le.startEstimate();

    return Void();
}

Return<void> Limits::stopHeadRoomCalculation() {

	le.stopEstimate();

    return Void();
}

// Methods from ::vendor::qti::hardware::limits::V1_1::ILimits follow.
Return<void> Limits::registerMitigationCallback(MitigationType type,
			const sp<ILimitsCallback> &callback,
			registerMitigationCallback_cb _hidl_cb)
{
	ThermalStatus status;
	std::lock_guard<std::mutex> _lock(cb_mutex);

        status.code = ThermalStatusCode::SUCCESS;
	if (callback == nullptr)
		return exit_hal(_hidl_cb, "Invalid nullptr callback");
	if (type == MitigationType::UNKNOWN)
		return exit_hal(_hidl_cb, "Invalid Mitigation type");

	for (CallbackSetting _cb: cb) {
		if (interfacesEqual(_cb.callback, callback))
			return exit_hal(_hidl_cb,
				"Same callback interface registered already");
	}
	cb.push_back(CallbackSetting(callback, type));
	LOG(DEBUG) << "Mitigation callback has been registered. Type:"
		<< (int)type;
	for (Mitigation m: lc.getMitigation(type))
		callback->mitigationLevel(m);

	_hidl_cb(status);
	return Void();
}

Return<void> Limits::unregisterMitigationCallback(
		const sp<ILimitsCallback> &callback,
		unregisterMitigationCallback_cb _hidl_cb)
{
	ThermalStatus status;
	bool removed = false;
	std::lock_guard<std::mutex> _lock(cb_mutex);
	std::vector<CallbackSetting>::iterator it;

        status.code = ThermalStatusCode::SUCCESS;
	if (callback == nullptr)
		return exit_hal(_hidl_cb, "Invalid nullptr callback");

	for (it = cb.begin(); it != cb.end(); it++) {
		if (interfacesEqual(it->callback, callback)) {
			LOG(DEBUG) << "callback unregistered. Type: "
				<< (int)it->type;
			cb.erase(it);
			removed = true;
			break;
		}
	}
	if (!removed)
		return exit_hal(_hidl_cb, "The callback was not registered before");
	_hidl_cb(status);
	return Void();
}

void Limits::sendThrottlingChangeCB(const Mitigation &m)
{
	std::lock_guard<std::mutex> _lock(cb_mutex);
	std::vector<CallbackSetting>::iterator it;

	LOG(DEBUG) << "Cooling device Name: " << m.name <<
		" type: " << (int)m.type << " cur_level: " << m.currentLevel <<
		" max_level: " << m.maxLevel;
	it = cb.begin();
	while (it != cb.end()) {
		if (it->type == m.type) {
			Return<void> ret = it->callback->mitigationLevel(m);
			if (!ret.isOk()) {
				LOG(ERROR) << "Notify callback execution error. Removing";
				it = cb.erase(it);
				continue;
			}
		}
		it++;
	}
}

// Methods from ::vendor::qti::hardware::limits::V1_2::ILimits follow.
Return<void> Limits::getCurrentThermalHeadRoom(TemperatureType type,
		uint32_t sample_count, int reserved, getThermalHeadRoom_cb _hidl_cb) {
	float hr = 0;
	ThermalStatus status;
	status.code = ThermalStatusCode::SUCCESS;

	hr = le.getHeadRoom(type, sample_count);
	if (hr < 0.0) {
		status.code = ThermalStatusCode::FAILURE;
		status.debugMessage = "Error getting Head Room.";
		LOG(ERROR) << "Error getting Head Room.";
	}

	_hidl_cb(status, hr);

    return Void();
}

}  // namespace vendor::qti::hardware::limits::implementation
