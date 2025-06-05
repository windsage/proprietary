/*
 * Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "temperatureQ.h"
#include <android-base/logging.h>

namespace vendor::qti::hardware::limits::implementation {

tempQ::tempQ(float thresh) {
	threshold = thresh;
	hr = DEFAULT_HEADROOM_MSEC;
}

void tempQ::UpdateThreshold(float val) {
	std::lock_guard<std::mutex> _lock(headRoomMutex);
	threshold = val;
}

void tempQ::add(float val)
{
	std::lock_guard<std::mutex> _lock(headRoomMutex);
	std::vector<float>::iterator it = temp.begin();

	if (temp.size() == TEMP_HIST_SIZE)
		temp.pop_back();
	temp.insert(it, val);
}

void tempQ::ClearTemp(void)
{
	std::lock_guard<std::mutex> _lock(headRoomMutex);

	if (!temp.empty())
		temp.clear();
}

int tempQ::estimateHeadroom(uint32_t sample_count)
{
	int i = 0, tmp = 0;
	double x = 0, x2 = 0, y = 0, xy = 0, b1, b2;
	std::lock_guard<std::mutex> _lock(headRoomMutex);

	/*
	 * Support for 1.0 API and in case client doesn't mention any sample count,
	 * use default sample count.
	 */
	if (!sample_count)
		sample_count = DEFAULT_SAMPLE_COUNT;

	if (sample_count > TEMP_HIST_SIZE ||
		temp.size() < sample_count)
		return DEFAULT_HEADROOM_MSEC;

	if (temp.front() >= threshold) {
		hr = 0;
		return hr;
	}
	/*
	 * Algorithm reference:
	 * https://www.codesansar.com/numerical-methods/linear-regression-method-algorithm.htm
	 */
	for (i = 0; i < sample_count; i++) {
		LOG(DEBUG) << "i:" << i << " temp:" << temp[sample_count - i - 1] << " sample_count:"
			<< sample_count << std::endl;
		x += (i + 1);
		x2 += ((i + 1) * (i + 1));
		y += temp[sample_count - i - 1];
		xy += (i + 1) * temp[sample_count - i - 1];
	}
	/* y = b1 * x + b2 */
	b1 = (sample_count * xy - x * y) / (sample_count * x2 - x * x);
	b2 = (y - b1 * x) / sample_count;

	LOG(DEBUG) << "x:" << x << " x2:" << x2 << " y:" << y << " xy:" << xy << std::endl;
	LOG(DEBUG) << "b1:" << b1 << " b2:"<< b2 << " thresh:" << threshold << std::endl;

	if (b1 <= ZERO_SLOPE_THRESHOLD && b1 >= -ZERO_SLOPE_THRESHOLD) {
		b1 = ZERO_SLOPE;
	} else if (b1 < 0) {
		b1 = PLATFORM_SLOPE;
		b2 = temp[0]; //curr temp
	}

	tmp = (threshold - b2) / b1;
	/* Check for possible overflow */
	if (tmp <= (DEFAULT_HEADROOM_MSEC / DEFAULT_POLLING_MSEC))
		hr = tmp * DEFAULT_POLLING_MSEC;
	else
		hr = DEFAULT_HEADROOM_MSEC;

end:
	LOG(DEBUG) << "hr: " << hr << std::endl;
	return hr;
}

}  // namespace vendor::qti::hardware::limits::implementation
