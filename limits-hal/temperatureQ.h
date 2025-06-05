/*
 * Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TEMPERATURE_Q_H__
#define TEMPERATURE_Q_H__

#include <vector>
#include <android-base/logging.h>

#define TEMP_HIST_SIZE 180
#define DEFAULT_SAMPLE_COUNT 30
#define DEFAULT_POLLING_MSEC 1000

#define ZERO_SLOPE_THRESHOLD 0.002
#define ZERO_SLOPE 0.018
#define DEFAULT_HEADROOM_MSEC 600000
#define PLATFORM_SLOPE 0.018

namespace vendor::qti::hardware::limits::implementation {

class tempQ {
public:
	tempQ(float thresh = 95);
	void UpdateThreshold(float val);
	void add(float val);
	int estimateHeadroom(uint32_t sample_count);
	void ClearTemp(void);

private:
	int hr;
	float threshold;
	std::vector<float> temp;
	std::mutex headRoomMutex;
};

} // namespace vendor::qti::hardware::limits::implementation

#endif // TEMPERATURE_Q_H__
