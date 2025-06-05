/******************************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ******************************************************************************/

#include <log/log.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <aidl/vendor/qti/hardware/servicetrackeraidl/BnServicetracker.h>
#include <aidl/vendor/qti/hardware/servicetrackeraidl/Status.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace servicetrackeraidl {
namespace implementation {

using aidl::vendor::qti::hardware::servicetrackeraidl::IServicetracker;
using aidl::vendor::qti::hardware::servicetrackeraidl::Status;
using aidl::vendor::qti::hardware::servicetrackeraidl::ClientConnection;
using aidl::vendor::qti::hardware::servicetrackeraidl::ClientData;
using aidl::vendor::qti::hardware::servicetrackeraidl::ClientRecord;
using aidl::vendor::qti::hardware::servicetrackeraidl::ServiceConnection;
using aidl::vendor::qti::hardware::servicetrackeraidl::ServiceData;
using aidl::vendor::qti::hardware::servicetrackeraidl::ServiceRecord;

using ::ndk::SpAIBinder;
using ::ndk::ScopedAStatus;

struct Servicetracker : public BnServicetracker {

public:
    ScopedAStatus bindService(const ServiceData& in_serviceData, const ClientData& in_clientData) override;
    ScopedAStatus destroyService(const ServiceData& in_serviceData) override;
    ScopedAStatus killProcess(int32_t in_pid) override;
    ScopedAStatus startService(const ServiceData& in_serviceData) override;
    ScopedAStatus unbindService(const ServiceData& in_serviceData, const ClientData& in_clientData) override;
    ScopedAStatus getClientConnections(const std::string& in_clientName, std::vector<ClientConnection>* _aidl_return) override;
    ScopedAStatus getPid(const std::string& in_processName, int32_t* _aidl_return) override;
    ScopedAStatus getPids(const std::vector<std::string>& in_serviceList, std::vector<int32_t>* _aidl_return) override;
    ScopedAStatus getRunningServicePid(std::vector<int32_t>* _aidl_return) override;
    ScopedAStatus getServiceBCount(std::vector<ServiceRecord>* out_bServiceList, int32_t* _aidl_return) override;
    ScopedAStatus getServiceConnections(const std::string& in_serviceName, std::vector<ServiceConnection>* _aidl_return) override;
    ScopedAStatus getclientInfo(const std::string& in_clientName, ClientRecord* _aidl_return) override;
    ScopedAStatus getserviceInfo(const std::string& in_serviceName, ServiceRecord* _aidl_return) override;
    ScopedAStatus isServiceB(const std::string& in_serviceName, bool* _aidl_return) override;
}; // Servicetracker
}  //Implementation
}  // namespace Servicetrackeraidl
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
