/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef LIMITS_COMMON_H__
#define LIMITS_COMMON_H__

#include <unordered_map>
#include <vector>
#include <vendor/qti/hardware/limits/1.1/ILimits.h>
#include "LimitsNetlink.h"

namespace vendor::qti::hardware::limits::implementation {

using ::vendor::qti::hardware::limits::V1_1::MitigationType;
using ::vendor::qti::hardware::limits::V1_1::Mitigation;
using notifyCB = std::function<void(const Mitigation)>;

struct cdevData {
	Mitigation data;
	int cdevn;
};

class LimitsCommon {
	public:
		LimitsCommon(const notifyCB& cb);
		~LimitsCommon() = default;

		int initCdev();

		void cdevNotify(int cdevn, int state);
		std::vector<Mitigation> getMitigation(MitigationType type);
	private:
		int readFromFile(std::string_view path, std::string& out);
		int read_cdev_state(cdevData& cdev);
		std::unordered_map<int, cdevData> cdev;
		LimitsNetlink lm;
		notifyCB notify;
};

}  // namespace vendor::qti::hardware::limits::implementation

#endif  // LIMITS_COMMON_H__
