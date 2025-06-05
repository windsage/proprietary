/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_QTI_VENDOR_LIMITS_H
#define ANDROID_QTI_VENDOR_LIMITS_H


#include <vendor/qti/hardware/limits/1.2/ILimits.h>
#include <vendor/qti/hardware/limits/1.1/ILimitsCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "LimitsEstimate.h"
#include "LimitsCommon.h"

namespace vendor::qti::hardware::limits::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;
using ::vendor::qti::hardware::limits::V1_2::TemperatureType;
using ::vendor::qti::hardware::limits::V1_2::ILimits;
using ::vendor::qti::hardware::limits::V1_1::MitigationType;
using ::vendor::qti::hardware::limits::V1_1::Mitigation;
using ::vendor::qti::hardware::limits::V1_1::ILimitsCallback;

struct CallbackSetting {
	sp<ILimitsCallback> callback;
	MitigationType type;

	CallbackSetting(sp<ILimitsCallback> callback, MitigationType type)
			: callback(callback), type(type) {}
};

class Limits : public ILimits {
public:
	Limits();
	~Limits() = default;

    // Methods from ::vendor::qti::hardware::limits::V1_0::ILimits follow.
    Return<void> getThermalHeadRoom(getThermalHeadRoom_cb _hidl_cb) override;
    Return<void> stopHeadRoomCalculation() override;
    Return<void> startHeadRoomCalculation() override;

	// Methods from ::vendor::qti::hardware::limits::V1_1::ILimits follow.
	Return<void> registerMitigationCallback(MitigationType type,
			const sp<ILimitsCallback> &callback,
			registerMitigationCallback_cb _hidl_cb);
	Return<void> unregisterMitigationCallback(
			const sp<ILimitsCallback> &callback,
			unregisterMitigationCallback_cb _hidl_cb);

	void sendThrottlingChangeCB(const Mitigation &m);

    // Methods from ::vendor::qti::hardware::limits::V1_2::ILimits follow.
    Return<void> getCurrentThermalHeadRoom(TemperatureType type,
		    uint32_t sample_count, int reserved,
		    getThermalHeadRoom_cb _hidl_cb) override;
private:
	std::mutex cb_mutex;
	std::vector<CallbackSetting> cb;
	LimitsEstimate le;
	LimitsCommon lc;

};

}  // namespace vendor::qti::hardware::limits::implementation

#endif  // ANDROID_QTI_VENDOR_LIMITS_H
