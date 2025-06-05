/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstdio>
#include <cinttypes>
#include <string>
#include <dirent.h>
#include <unordered_map>
#include <fstream>

#include <android-base/logging.h>
#include "LimitsCommon.h"

#define RETRY_CT		3
#define MAX_LENGTH		50
#define MAX_PATH		(256)
#define THERMAL_SYSFS		"/sys/class/thermal/"
#define TZ_TYPE			"type"
#define CDEV_DIR_NAME		"cooling_device"
#define CDEV_DIR_FMT		"cooling_device%d"
#define CDEV_CUR_STATE_PATH	"/sys/class/thermal/cooling_device%d/cur_state"
#define CDEV_MAX_STATE_PATH	"/sys/class/thermal/cooling_device%d/max_state"

namespace vendor::qti::hardware::limits::implementation {

using ::vendor::qti::hardware::limits::V1_1::MitigationType;
using ::vendor::qti::hardware::limits::V1_1::Mitigation;

static std::unordered_map<std::string, MitigationType> cdev_map = {
	{"display-fps", MitigationType::DISPLAY_FPS},
	{"panel-backlight", MitigationType::DISPLAY_BRIGHTNESS},
	{"camera", MitigationType::CAMERA},
};

static int readLineFromFile(std::string_view path, std::string& out)
{
	char *fgets_ret;
	FILE *fd;
	int rv;
	char buf[MAX_LENGTH];

	out.clear();

	fd = fopen(std::string(path).c_str(), "r");
	if (fd == NULL) {
		LOG(ERROR) << "Path:" << std::string(path) << " file open error.err:"
			<< strerror(errno) << std::endl;
		return errno;
	}

	fgets_ret = fgets(buf, MAX_LENGTH, fd);
	if (NULL != fgets_ret) {
		rv = (int)strlen(buf);
		out.append(buf, rv);
	} else {
		rv = ferror(fd);
	}

	fclose(fd);
	out.erase(std::remove(out.begin(), out.end(), '\n'), out.end());
	LOG(DEBUG) << "Path:" << std::string(path) << " Val:" << out << std::endl;

	return rv;
}

LimitsCommon::LimitsCommon(const notifyCB& cb):
	lm(std::bind(&LimitsCommon::cdevNotify, this,
				std::placeholders::_1,
				std::placeholders::_2)),
	notify(cb)
{
}

int LimitsCommon::readFromFile(std::string_view path, std::string& out)
{
	return readLineFromFile(path, out);
}

void LimitsCommon::cdevNotify(int cdevn, int state)
{
	if (cdev.find(cdevn) == cdev.end())
		return;

	cdev[cdevn].data.currentLevel = state;
	notify(cdev[cdevn].data);
}

int LimitsCommon::initCdev()
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	int cdevn = 0;
	char name[MAX_PATH] = {0};
	char cwd[MAX_PATH] = {0};
	int ret = 0;

	if (!getcwd(cwd, sizeof(cwd)))
		return 0;

	/* Change dir to read the entries. Doesnt work otherwise */
	ret = chdir(THERMAL_SYSFS);
	if (ret) {
		LOG(ERROR) << "Unable to change to " << THERMAL_SYSFS << std::endl;
		return 0;
	}
	tdir = opendir(THERMAL_SYSFS);
	if (!tdir) {
		LOG(ERROR) << "Unable to open " << THERMAL_SYSFS << std::endl;
		return 0;
	}

	while ((tdirent = readdir(tdir))) {
		std::string buf;
		struct dirent *tzdirent;
		cdevData cdevInst;

		if (strncmp(tdirent->d_name, CDEV_DIR_NAME,
			strlen(CDEV_DIR_NAME)) != 0)
			continue;

		snprintf(name, MAX_PATH, "%s%s/%s", THERMAL_SYSFS,
				tdirent->d_name, TZ_TYPE);
		ret = readLineFromFile(std::string_view(name), buf);
		if (ret <= 0) {
			LOG(ERROR) <<
				"init_cdev: cdev type read error for cdev:" <<
				tdirent->d_name << std::endl;
		}
		if (cdev_map.find(buf) == cdev_map.end())
			continue;
		sscanf(tdirent->d_name, CDEV_DIR_FMT, &cdevn);
		LOG(DEBUG) << "cdev: " << buf <<
			" found at cdev number: " << cdevn << std::endl;
		cdevInst.data.name = buf;
		cdevInst.data.type = cdev_map[buf];
		cdevInst.cdevn = cdevn;
		read_cdev_state(cdevInst);
		cdev[cdevn] = cdevInst;
	}

	closedir(tdir);
	/* Restore current working dir */
	ret = chdir(cwd);
	lm.start();

	return cdev.size();
}

int LimitsCommon::read_cdev_state(cdevData& cdev)
{
	char file_name[MAX_PATH];
	std::string buf;
	int ret = 0, ct = 0;
	bool read_ok = true;

	do {
		snprintf(file_name, sizeof(file_name), CDEV_CUR_STATE_PATH,
			cdev.cdevn);
		ret = readLineFromFile(std::string(file_name), buf);
		if (ret <= 0) {
			LOG(ERROR) << "Cdev state read error:"<< ret <<
			" for cdev: " << cdev.data.name;
			return -1;
		}
		try {
			cdev.data.currentLevel = std::stoi(buf, nullptr, 0);
			snprintf(file_name, sizeof(file_name),
				CDEV_MAX_STATE_PATH, cdev.cdevn);
			ret = readLineFromFile(std::string(file_name), buf);
			if (ret <= 0) {
				LOG(ERROR) << "Cdev max state read error:"<< ret <<
				" for cdev: " << cdev.data.name;
				return -1;
			}
			cdev.data.maxLevel = std::stoi(buf, nullptr, 0);
			read_ok = true;
		}
		catch (std::exception &err) {
			LOG(ERROR) << "cdev state stoi error:" << err.what()
				<< " buf:" << buf;
			ct++;
			read_ok = false;
		}
	} while (!read_ok && ct < RETRY_CT);
	LOG(DEBUG) << "cdev Name:" << cdev.data.name << ". cur state:" <<
		cdev.data.currentLevel << " Max:" <<
		cdev.data.maxLevel << std::endl;

	return 0;
}

std::vector<Mitigation> LimitsCommon::getMitigation(MitigationType type)
{
	std::vector<Mitigation> ret;

	for (auto it: cdev) {
		if (it.second.data.type == type)
			ret.push_back(it.second.data);
	}

	return ret;
}
}  // namespace vendor::qti::hardware::limits::implementation
