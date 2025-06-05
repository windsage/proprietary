/*
 * Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_HARDWARE_FACTORY_AIDL_IFACTORY_H
#define VENDOR_QTI_HARDWARE_FACTORY_AIDL_IFACTORY_H
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <aidl/vendor/qti/hardware/factory/BnFactory.h>
#include <hardware/hardware.h>
#include <cutils/properties.h>
#include <vector>

#define MMID_SERVICE_STATUS "init.svc.vendor.mmid"
#define PACK_SIZE 1024

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace factory {

using ::aidl::vendor::qti::hardware::factory::IFactory;
using ::aidl::vendor::qti::hardware::factory::FactoryResult;
using ::aidl::vendor::qti::hardware::factory::IResultType;
using ::aidl::vendor::qti::hardware::factory::WriteFileReq;
using ::aidl::vendor::qti::hardware::factory::ReadFileResult;
using ::aidl::vendor::qti::hardware::factory::ReadFileReq;

//using ::android::hardware::Return;
//using ::android::hardware::Void;
using ::ndk::ScopedAStatus;

using ::std::string;
using ::std::vector;

class Factory : public BnFactory {

public:
    Factory();
    ~Factory();

    ScopedAStatus runApp(const std::string& name,const std::string& params, bool isStart, FactoryResult* _aidl_return) override;
    ScopedAStatus getSmbStatus(FactoryResult* _aidl_return) override;
    ScopedAStatus enterShipMode(bool* _aidl_return) override;
    ScopedAStatus chargerEnable(bool enable, bool* _aidl_return) override;
    ScopedAStatus wifiEnable(bool enable, bool* _aidl_return) override;

    ScopedAStatus eraseAllFiles(const std::string& path, IResultType* _aidl_return) override;
    ScopedAStatus dirListFileNameToFile(const std::string& path, const std::string& name, IResultType* _aidl_return) override;
    ScopedAStatus readFile(const std::string& path, const ReadFileReq& req, ReadFileResult* _aidl_return) override;
    ScopedAStatus writeFile(const std::string& path, const WriteFileReq& req, IResultType* _aidl_return) override;
    ScopedAStatus delegate(const std::string& cmd,const std::string& value, FactoryResult* _aidl_return) override;

private:
    FactoryResult set_prop(const char* prop, const char* value);
    FactoryResult get_prop(const char* prop);
    vector<string> split_string(const std::string& str, const std::string& delim);
};

}  // namespace factory
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif // VENDOR_QTI_HARDWARE_FACTORY_AIDL_IFACTORY_H
