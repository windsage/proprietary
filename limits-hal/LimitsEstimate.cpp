/*
 * Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <limits>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>
#include <hidl/HidlTransportSupport.h>

#include "LimitsEstimate.h"

namespace vendor::qti::hardware::limits::implementation {

void LimitsEstimate::ThermalHalDeathRecipient::serviceDied(uint64_t cookie, const wp<IBase>& who) {
	LOG(INFO) << "ThermalHAL Service died, cookie:" << static_cast<unsigned long long>(cookie)
		<< "who: " << &who << std::endl;
	if (mLimitsEstimate == nullptr)
		return;
	std::lock_guard<std::mutex> _lock(mLimitsEstimate->estimateMutex);
	mLimitsEstimate->ThermalHal = nullptr;
	mLimitsEstimate->getThermalHal();
};

void LimitsEstimate::getThermalHal() {
	if (ThermalHal != nullptr)
		return;
	ThermalHal = IThermal::getService();
	if (ThermalHal == nullptr) {
		LOG(ERROR) << "Thermal HAL service get error" << std::endl;
		return;
	}
	if (lmThermalHalDeathRecipient == nullptr)
		lmThermalHalDeathRecipient = new ThermalHalDeathRecipient(this);

	Return<bool> linked = ThermalHal->linkToDeath(
                    lmThermalHalDeathRecipient, 0xF5FF /* cookie */);
	if (!linked.isOk()) {
		LOG(ERROR) << "Transaction error in linking to ThermalHAL death:"
			<< linked.description();
		ThermalHal = nullptr;
		return;
	} else if (!linked) {
		LOG(ERROR) << "Unable to link to ThermalHal death notifications";
		ThermalHal = nullptr;
		return;
	}
}

LimitsEstimate::LimitsEstimate()
{
	Return<void> ret;
	size_t i;

	pthread_mutex_init(&waitMutex, NULL);
	pthread_cond_init(&waitCond, NULL);
	getThermalHal();
	if (ThermalHal == nullptr)
		return;

	exitMode = pollingMode = false;
	pthread_create(&pollThread, NULL,
			&LimitsEstimate::limitsPoll, (void *)this);
	for (i = 0; i < MAX_TEMP_TYPE; ++i) {
		::android::hardware::thermal::V2_0::TemperatureType tempHalType =
			(::android::hardware::thermal::V2_0::TemperatureType)tempTypeMap[i];
		if ((int)tempHalType > (int)TemperatureType::NPU) {
			/* TO DO: Need to add non thermal hal sensor support */
			LOG(DEBUG) << "Thermal HAL doesn't support this sensor: " <<
			        (int)tempHalType << std::endl;
			continue;
		}
		ret = ThermalHal->getTemperatureThresholds(true, tempHalType,
				std::bind(&LimitsEstimate::thresholdCB, this,
					std::placeholders::_1, std::placeholders::_2));
		if (!ret.isOk())
			LOG(ERROR) << "getTemperatureThresholds() failed. Error:" <<
				ret.description() << std::endl;
	}
}

LimitsEstimate::~LimitsEstimate()
{
	void *data;

	pthread_mutex_lock(&waitMutex);
	pollingMode = false;
	exitMode = true;
	pthread_cond_broadcast(&waitCond);
	pthread_mutex_unlock(&waitMutex);

	pthread_join(pollThread, &data);
	if (ThermalHal) {
		ThermalHal->unlinkToDeath(lmThermalHalDeathRecipient);
		ThermalHal = nullptr;
	}
}

void LimitsEstimate::ClearTempData(void)
{
	size_t i;

	for (i = 0; i < MAX_TEMP_TYPE; ++i)
		temp[i].ClearTemp();
}

void* LimitsEstimate::limitsPoll(void *data)
{
	LimitsEstimate *ptr = (LimitsEstimate *)data;
	struct timespec time_val;
	int ret;

	pthread_mutex_lock(&ptr->waitMutex);
	while (!ptr->exitMode) {
		if (ptr->pollingMode) {
			LOG(DEBUG) << "Thermal poll:" << DEFAULT_POLLING_MSEC << std::endl;
			clock_gettime(CLOCK_REALTIME, &time_val);
			time_val.tv_sec += DEFAULT_POLLING_MSEC / 1000;
			time_val.tv_nsec += (DEFAULT_POLLING_MSEC % 1000) * 1000000;
			ret = pthread_cond_timedwait(&ptr->waitCond, &ptr->waitMutex,
									&time_val);
			if (ret == ETIMEDOUT)
				ptr->estimate();
		} else {
			/* Clear all previous temp samples */
			ptr->ClearTempData();
			LOG(DEBUG) << "Thermal poll service wait" << std::endl;
			pthread_cond_wait(&ptr->waitCond, &ptr->waitMutex);
		}
	}
	pthread_mutex_unlock(&ptr->waitMutex);

	return NULL;
}

void LimitsEstimate::thresholdCB(ThermalStatus status,
		const hidl_vec<TemperatureThreshold>& temps)
{
	int idx = (int)ThrottlingSeverity::NONE;

	if (status.code != ThermalStatusCode::SUCCESS) {
		LOG(ERROR) << "Failed to get temperature threshold from ThermalHAL. Status:"
					<< status.debugMessage << std::endl;
		return;
	}

	for (size_t i = 0; i < temps.size(); ++i) {
		LOG(DEBUG) << "Temp type:" << (static_cast<int>(temps[i].type))
			<< " Sensor:" << temps[i].name
			<< " VR threshold:" << temps[i].vrThrottlingThreshold
			<< std::endl;

		/*
		 * We care about the first throttling threshold and not shutdown
		 * threshold.
		 */
		for (idx = (int)ThrottlingSeverity::NONE;
				idx < (int)ThrottlingSeverity::SHUTDOWN; idx++) {
			if (!isnan(temps[i].hotThrottlingThresholds[idx])) {
				LOG(INFO) << "hot Type: " << idx <<
					" Threshold:" <<
					temps[i].hotThrottlingThresholds[idx]
					<< std::endl;
				switch ((vendor::qti::hardware::limits::V1_2::TemperatureType)temps[i].type) {
				case TemperatureType::CPU:
					temp[CPU_TEMP].UpdateThreshold(
						temps[i].hotThrottlingThresholds[idx]);
					break;
				case TemperatureType::GPU:
					temp[GPU_TEMP].UpdateThreshold(
						temps[i].hotThrottlingThresholds[idx]);
					break;
				case TemperatureType::NPU:
					temp[NSP_TEMP].UpdateThreshold(
						temps[i].hotThrottlingThresholds[idx]);
					break;
				case TemperatureType::BCL_PERCENTAGE:
					temp[SOC_TEMP].UpdateThreshold(
						temps[i].hotThrottlingThresholds[idx]);
					break;
				case TemperatureType::SKIN:
					temp[SKIN_TEMP].UpdateThreshold(
						temps[i].hotThrottlingThresholds[idx]);
					break;
				default:
					continue;
				}
				break;
			}
		}
	}
}

void LimitsEstimate::temperatureCB(ThermalStatus status,
		const hidl_vec<Temperature>& temps)
{
	float tempData = -1;
	int temptype = MAX_TEMP_TYPE;
	std::lock_guard<std::mutex> _lock(estimateMutex);

	LOG(DEBUG) << "enter temperatureCB" << std::endl;
	if (status.code != ThermalStatusCode::SUCCESS) {
		LOG(ERROR) << "Failed to get temperatures from ThermalHAL. Status:"
				<< status.debugMessage << std::endl;
		return;
	}
	for (size_t i = 0; i < temps.size(); ++i) {
		LOG(DEBUG) << "Temp type:" << (static_cast<int>(temps[i].type))
				<< " Sensor:" << temps[i].name
				<< " Temp:" << temps[i].value
				<< " Throttle Status:" << (int)(temps[i].throttlingStatus)
				<< std::endl;
		switch ((vendor::qti::hardware::limits::V1_2::TemperatureType)temps[i].type) {
			case TemperatureType::CPU:
				tempData = std::max(temps[i].value, tempData);
				temptype = CPU_TEMP;
				break;
			case TemperatureType::GPU:
				tempData = std::max(temps[i].value, tempData);
				temptype = GPU_TEMP;
				break;
			case TemperatureType::NPU:
				tempData = std::max(temps[i].value, tempData);
				temptype = NSP_TEMP;
				break;
			case TemperatureType::BCL_PERCENTAGE:
				tempData = std::max(temps[i].value, tempData);
				temptype = SOC_TEMP;
				break;
			case TemperatureType::SKIN:
				tempData = std::max(temps[i].value, tempData);
				temptype = SKIN_TEMP;
				break;
			default:
				continue;
		}
	}
	if (temptype != MAX_TEMP_TYPE) {
		LOG(DEBUG) << "Temp type:" << (static_cast<int>(temps[0].type))
			<< " Max Temp:" << tempData
			<< std::endl;
		temp[temptype].add(tempData);
	}
}

void LimitsEstimate::estimate()
{
	Return<void> ret;
	size_t i;

	if (ThermalHal == nullptr)
		return;
	for (i = 0; i < MAX_TEMP_TYPE; ++i) {
		::android::hardware::thermal::V2_0::TemperatureType tempHalType =
			(::android::hardware::thermal::V2_0::TemperatureType)tempTypeMap[i];
		if ((int)tempHalType > (int)TemperatureType::NPU) {
			/* TO DO: Need to add non thermal hal sensor support */
			LOG(DEBUG) << "Thermal HAL doesn't support this sensor: " <<
			        (int)tempHalType << std::endl;
			continue;
		}
		ret = ThermalHal->getCurrentTemperatures(true, tempHalType,
				std::bind(&LimitsEstimate::temperatureCB, this,
					std::placeholders::_1, std::placeholders::_2));
		if (!ret.isOk())
			LOG(ERROR) << "getCurrentTemperatures() failed. Error:" <<
				ret.description() << std::endl;
	}

}

int LimitsEstimate::getHeadRoom()
{
	int hr = INT_MAX;
	size_t i;

	if (ThermalHal == nullptr) {
		getThermalHal();
		if (ThermalHal == nullptr)
			return -1;
	}
	if (!pollingMode)
		return -1;

	for (i = 0; i < MAX_TEMP_TYPE; ++i) {
		int iphr = temp[i].estimateHeadroom(0);
		LOG(DEBUG) << tempName[i] << " hr:" << iphr << std::endl;
		hr = std::min(hr, iphr);
	}

	LOG(DEBUG)<< "hr: " << hr << std::endl;
	return hr;
}

int LimitsEstimate::getHeadRoom(TemperatureType type, uint32_t sample_count)
{
	int hr = INT_MAX;
	size_t i;

	if (ThermalHal == nullptr) {
		getThermalHal();
		if (ThermalHal == nullptr)
			return -1;
	}
	if (!pollingMode)
		return -1;

	for (i = 0; i < MAX_TEMP_TYPE; ++i) {
		if (tempTypeMap[i] == type) {
			hr = temp[i].estimateHeadroom(sample_count);
			LOG(DEBUG) << tempName[i] << " hr:" << hr << std::endl;
			break;
		}
	}

	return hr;
}

void LimitsEstimate::startEstimate(void)
{
	std::lock_guard<std::mutex> _lock(estimateMutex);

	if (ThermalHal == nullptr) {
		getThermalHal();
		if (ThermalHal == nullptr)
			return;
	}
	pthread_mutex_lock(&waitMutex);
	LOG(INFO) << "Limits HAL estimation start" << std::endl;
	pollingMode = true;
	pthread_cond_broadcast(&waitCond);
	pthread_mutex_unlock(&waitMutex);

	return;
}

void LimitsEstimate::stopEstimate(void)
{
	std::lock_guard<std::mutex> _lock(estimateMutex);

	pthread_mutex_lock(&waitMutex);
	LOG(INFO) << "Limits HAL estimation stop" << std::endl;
	pollingMode = false;
	pthread_cond_broadcast(&waitCond);
	pthread_mutex_unlock(&waitMutex);

	return;
}

}  // namespace vendor::qti::hardware::limits::implementation
