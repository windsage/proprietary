/******************************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/
#include "Servicetracker.h"
#include <map>
#include <vector>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android-base/logging.h>
#include <cutils/properties.h>
#include <aidl/vendor/qti/hardware/servicetrackeraidl/Status.h>

namespace aidl{
namespace vendor {
namespace qti {
namespace hardware {
namespace servicetrackeraidl {
namespace implementation {

using ::aidl::vendor::qti::hardware::servicetrackeraidl::Status;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ClientData;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ServiceData;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ClientConnection;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ServiceConnection;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ClientRecord;
using ::aidl::vendor::qti::hardware::servicetrackeraidl::ServiceRecord;

struct service_record {
    std::string packageName;
    std::string processName;
    int32_t pid;
    bool serviceB;
    std::vector<ServiceConnection> conn;
    double lastActivity;
};

struct client_record {
    std::string processName;
    int32_t pid;
    std::vector<ClientConnection> conn;
};

//maps to store services and clients details
std::map<std::string, service_record> mServices;
std::map<std::string, client_record> mClients;

// used for synchronized access of map
std::mutex mLock;

//private APIs
void createServiceData(ServiceData,service_record*);
void createClientData(ClientData,client_record*);
void addServiceConnection(ClientData,service_record*);
void addClientConnection(ServiceData,client_record*);
void removeServiceConnection(std::string,service_record*);
void removeClientConnection(std::string,client_record*);

// Methods from ::vendor::qti::hardware::servicetracker::V1_0::IServicetracker follow.
ScopedAStatus Servicetracker::startService(const ServiceData& serviceData) {
    std::lock_guard<std::mutex> lock(mLock);
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceData.processName);
    if (itr != mServices.end()) {
        itr->second.lastActivity = serviceData.lastActivity;
        itr->second.serviceB = serviceData.serviceB;
        itr->second.pid = serviceData.pid;
        itr->second.processName = serviceData.processName;
    } else {
        service_record s;
        createServiceData(serviceData,&s);
        mServices.insert({serviceData.processName,s});
    }
   return ScopedAStatus::ok();
}

void createServiceData(ServiceData s_data, service_record *s_record) {
    s_record->packageName = s_data.packageName;
    s_record->processName = s_data.processName;
    s_record->pid = s_data.pid;
    s_record->lastActivity = s_data.lastActivity;
    s_record->serviceB = s_data.serviceB;
}

void createClientData(ClientData c_data, client_record *c_record) {
    c_record->processName= c_data.processName;
    c_record->pid = c_data.pid;
}

ScopedAStatus Servicetracker::bindService(const ServiceData& serviceData, const ClientData& clientData) {
    std::lock_guard<std::mutex> lock(mLock);
    LOG(INFO) << "bindService is called for service : " << serviceData.processName << " and for client " << clientData.processName;
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceData.processName);
    if (itr != mServices.end()) {
        itr->second.lastActivity = serviceData.lastActivity;
        itr->second.serviceB = serviceData.serviceB;
        addServiceConnection(clientData,&itr->second);
    } else {
        service_record s;
        createServiceData(serviceData,&s);
        addServiceConnection(clientData,&s);
        mServices.insert({serviceData.processName,s});
    }
    std::map<std::string, client_record>::iterator itr_2 = mClients.find(clientData.processName);
    if (itr_2 != mClients.end()) {
        addClientConnection(serviceData,&itr_2->second);
    } else {
        client_record c;
        createClientData(clientData,&c);
        addClientConnection(serviceData,&c);
        mClients.insert({clientData.processName,c});
    }
    return ScopedAStatus::ok();
}

void addClientConnection(ServiceData s_data,client_record *c_record) {
    bool flag = false;
    for (auto it = c_record->conn.begin(); it != c_record->conn.end(); it++) {
        if (it->serviceName == s_data.processName) {
            flag=true;
            it->count +=1;
            break;
        }
    }
    if (!flag) {
        //createconnection
        c_record->conn.resize(c_record->conn.size() + 1);
        auto it = c_record->conn.end() -1;
        it->serviceName = s_data.processName;
        it->servicePid = s_data.pid;
        it->count = 1;
    }
    LOG(INFO) << "total connections for client : "  << c_record->processName << "are :" << c_record->conn.size();
}

void addServiceConnection(ClientData c_data, service_record *s_record) {
    bool flag = false;
    for(auto it = s_record->conn.begin(); it != s_record->conn.end(); it++) {
        if (it->clientPid == c_data.pid) {
            flag = true;
            it->count +=1;
            break;
        }
    }
    if (!flag) {
        //createconnection
        s_record->conn.resize(s_record->conn.size() + 1);
        auto itr = s_record->conn.end() -1;
        itr->clientName = c_data.processName;
        itr->clientPid = c_data.pid;
        itr->count = 1;
    }
    LOG(INFO) << "total connections for service : " << s_record->processName << "are :" << s_record->conn.size();
}

ScopedAStatus Servicetracker::unbindService(const ServiceData& serviceData, const ClientData& clientData) {
    std::lock_guard<std::mutex> lock(mLock);
    LOG(INFO) << "unbindService is called for service : " << serviceData.processName << " and for client " << clientData.processName;
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceData.processName);
    if (itr != mServices.end()) {
        //only update the needed info and remove  the connection
        itr->second.lastActivity = serviceData.lastActivity;
        itr->second.serviceB = serviceData.serviceB;
        removeServiceConnection(clientData.processName,&itr->second);
    } else {
        //error this should not happen
        LOG(INFO) << "unbind service " << serviceData.processName << "not found";
    }
    std::map<std::string, client_record>::iterator itr_2 = mClients.find(clientData.processName);
    if (itr_2 != mClients.end()) {
        //remove the connection
        removeClientConnection(serviceData.processName,&itr_2->second);
    } else {
        //error this should not happen
        LOG(INFO) << "unbind client " << clientData.processName << "not  found";
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::destroyService(const ServiceData& serviceData) {
    std::lock_guard<std::mutex> lock(mLock);
    LOG(INFO) << "destroyService is called for service : " << serviceData.processName ;
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceData.processName);
    if (itr != mServices.end()) {
        for(auto it = ((itr->second).conn.begin()); it != ((itr->second).conn.end()); ++it) {
            std::map<std::string, client_record>::iterator itr_2 = mClients.find(it->clientName);
            if (itr_2 != mClients.end()) {
                removeClientConnection(serviceData.processName,&itr_2->second);
            }
        }
    //remove service entry from map
    mServices.erase(itr);
    } else {
     //error this should not happen
        LOG(INFO) << "destroy service " << serviceData.processName << "not  found";
    }
    return ScopedAStatus::ok();
}

void removeServiceConnection(std::string c_name, service_record *s_record) {
    for(auto it = s_record->conn.begin(); it != s_record->conn.end(); it++) {
        if (it->clientName == c_name) {
            it->count -=1;
            //remove the element if count is 0 AND BREAK
            if (it->count ==0) {
                s_record->conn.erase(it);
                LOG(INFO) << "size of service connections for service: " << s_record->processName << "after removal is " << s_record->conn.size();
                break;
            }
        }
    }
}

void removeClientConnection(std::string s_name, client_record *c_record){
    for(auto it = c_record->conn.begin(); it != c_record->conn.end(); ++it) {
        if (it->serviceName == s_name) {
            it->count -=1;
            //remove the element if count is 0 AND BREAK
            if (it->count ==0) {
                c_record->conn.erase(it);
                LOG(INFO) << "size of client connections for client: " << c_record->processName << "after removal is " << c_record->conn.size();
                break;
            }
        }
    }
}
ScopedAStatus Servicetracker::killProcess(int32_t pid) {
    //lets say it is a service than check its connection and for each connection remove the corresponding connection entry from its clients
    std::lock_guard<std::mutex> lock(mLock);
    LOG(INFO) << "killProcess is called for pid : " << pid ;
    for(auto itr = mServices.begin(); itr != mServices.end(); itr++) {
        if (itr->second.pid == pid){
            LOG(INFO) << "process with pid " << pid << " is service" ;
            for(auto it = ((itr->second).conn.begin()); it != ((itr->second).conn.end()); ++it) {
                std::map<std::string, client_record>::iterator itr_2 = mClients.find(it->clientName);
                if (itr_2 != mClients.end()) {
                    removeClientConnection(itr->second.processName,&itr_2->second);
                }
            }
        }
    }
    for(auto itr = mClients.begin(); itr != mClients.end();) {
        if (itr->second.pid == pid) {
            LOG(INFO) << "process with pid " << pid << " is client" ;
            for(auto it = ((itr->second).conn.begin()); it != ((itr->second).conn.end()); ++it) {
                std::map<std::string, service_record>::iterator itr_2 = mServices.find(it->serviceName);
                if (itr_2 != mServices.end()) {
                    removeServiceConnection(itr->second.processName,&itr_2->second);
                }
            }
            //remove client entry from list
            LOG(INFO) << "removing client with pid " <<itr->second.pid << "process name" << itr->second.processName  ;
            itr = mClients.erase(itr);
        } else {
            ++itr;
        }
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getclientInfo(const std::string& clientName, ClientRecord* ret) {
    LOG(INFO) << "getclientInfo called for client: " << clientName;
    std::lock_guard<std::mutex> lock(mLock);
    std::map<std::string, client_record>::iterator itr = mClients.find(clientName);
    ClientRecord c;
    if (itr != mClients.end()) {
        c.processName = itr->second.processName;
        c.pid = itr->second.pid;
        c.conn = (std::vector<ClientConnection>)(itr->second.conn);
        *ret = c;
    } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getserviceInfo(const std::string& serviceName, ServiceRecord* ret) {
    LOG(INFO) << "getserviceInfo called for service: " << serviceName;
    std::lock_guard<std::mutex> lock(mLock);
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceName);
    ServiceRecord s;
    if (itr != mServices.end()) {
        s.packageName = itr->second.packageName;
        s.processName = itr->second.processName;
        s.pid = itr->second.pid;
        s.serviceB = itr->second.serviceB;
        s.lastActivity = itr->second.lastActivity;
        s.conn = (std::vector<ServiceConnection>)(itr->second.conn);
        *ret = s;
    } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getServiceConnections(const std::string& serviceName, std::vector<ServiceConnection>* ret) {
    LOG(INFO) << "getServiceConnections called for service: " << serviceName;
    std::lock_guard<std::mutex> lock(mLock);
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceName);
    std::vector<ServiceConnection> s_conn;
    if (itr != mServices.end()) {
        s_conn = (std::vector<ServiceConnection>)(itr->second.conn);
        *ret = s_conn;
        } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getClientConnections(const std::string& clientName, std::vector<ClientConnection>* ret) {
    LOG(INFO) << "getClientConnections called for client: " << clientName;
    std::lock_guard<std::mutex> lock(mLock);
    std::map<std::string, client_record>::iterator itr = mClients.find(clientName);
    std::vector<ClientConnection> c_conn;
    if (itr != mClients.end()) {
        c_conn = (std::vector<ClientConnection>)(itr->second.conn);
        *ret = c_conn;
    } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getPid(const std::string& processName, int32_t* ret) {
    LOG(INFO) << "getPid called for process: " << processName;
    std::lock_guard<std::mutex> lock(mLock);
    int32_t pid = -1;
    std::map<std::string, service_record>::iterator itr = mServices.find(processName);
    if (itr != mServices.end()) {
        pid = itr->second.pid;
        *ret = pid;
    } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::getPids(const std::vector<std::string>& serviceList, std::vector<int32_t>* ret) {
    std::lock_guard<std::mutex> lock(mLock);
    std::vector<int32_t>servicePids;
    for(auto it = serviceList.begin(); it != serviceList.end(); ++it) {
        std::map<std::string, service_record>::iterator itr = mServices.find(*it);
        if (itr != mServices.end()) {
            servicePids.push_back(itr->second.pid);
        } else {
            servicePids.push_back(-1);
        }
    }
    *ret = servicePids;
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker::isServiceB(const std::string& serviceName, bool* ret) {
    std::lock_guard<std::mutex> lock(mLock);
    bool serviceB = false;
    std::map<std::string, service_record>::iterator itr = mServices.find(serviceName);
    if (itr != mServices.end()) {
        serviceB = itr->second.serviceB;
        *ret = serviceB;
    } else {
        return ScopedAStatus::fromServiceSpecificError((uint32_t) Status::ERROR_NOT_AVAILABLE);
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Servicetracker:: getServiceBCount(std::vector<ServiceRecord>* out_bServiceList, int32_t* ret) {
    std::lock_guard<std::mutex> lock(mLock);
    std::vector<ServiceRecord> bServiceList;
    int32_t count = 0;
    for(auto it = mServices.begin(); it != mServices.end(); ++it) {
        if (it->second.serviceB) {
            ServiceRecord s;
            s.packageName = it->second.packageName;
            s.processName = it->second.processName;
            s.pid = it->second.pid;
            s.serviceB = it->second.serviceB;
            s.lastActivity = it->second.lastActivity;
            s.conn = (std::vector<ServiceConnection>)(it->second.conn);
            bServiceList.push_back(s);
            count +=1;
        }
    }
    *out_bServiceList = bServiceList;
    *ret = count;
    return ScopedAStatus::ok();
}


// Methods from ::vendor::qti::hardware::servicetracker::V1_1::IServicetracker follow.
ScopedAStatus Servicetracker::getRunningServicePid(std::vector<int32_t>* ret) {
    std::lock_guard<std::mutex> lock(mLock);
    std::vector<int32_t>runningPids;
    for(auto it = mServices.begin(); it != mServices.end(); ++it) {
        if (it->second.pid != -1) {
            runningPids.push_back(it->second.pid);
        }
    }
    *ret = runningPids;
    return ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace servicetracker
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
