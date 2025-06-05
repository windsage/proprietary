/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
SOTER Client app.
*********************************************************************/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/8/18     dy      Add hidl based client for decoupled soter
11/30/15   dy      Add soter client to support ATTK provision
===========================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <utils/Log.h>
#include <sys/mman.h>
#include <getopt.h>
#include <dlfcn.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <limits.h>
#include <aidl/vendor/qti/hardware/soter/SoterErrorCode.h>
#include <aidl/vendor/qti/hardware/soter/SoterBufferReturn.h>
#include <aidl/vendor/qti/hardware/soter/ISoter.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.hardware.soter-provision"
//#define ALOGD(...) do {} while (0)  //Comment this line to enable ALOGD
//#define ALOGV(...) do {} while (0)  //Comment this line to enable ALOGV

using ::ndk::SpAIBinder;
using ISoterAidl = ::aidl::vendor::qti::hardware::soter::ISoter;
using ::aidl::vendor::qti::hardware::soter::SoterBufferReturn;
using ::aidl::vendor::qti::hardware::soter::SoterInitReturn;
using SoterErrorCodeAidl = ::aidl::vendor::qti::hardware::soter::SoterErrorCode;

#define ATTK_COPY_NUM_DEFAULT          3
#define CMD_GENERATE_ATTK_KEY_PAIR     1
#define CMD_VERIFY_ATTK_KEY_PAIR       2
#define CMD_EXPORT_ATTK_PUBLIC_KEY     3
#define CMD_GET_DEVICE_ID              4

using namespace std;

static std::shared_ptr<ISoterAidl> soterAidl = nullptr;

static void qsc_usage(void)
{

    printf("*************************************************************\n");
    printf("************           SOTER CLIENT              ************\n");
    printf("*************************************************************\n");
    printf("\n"
        "Usage: soter_client [CMD]\n"
        "CMD can be:\n"
        " \t\t 1: Generate ATTK key pair\n"
        " \t\t 2: Verify ATTK key pair\n"
        " \t\t 3: Export ATTK public key \n"
        " \t\t 4: Get device ID \n"
        "---------------------------------------------------------\n\n\n");
}

void serviceDiedAidl(void* cookie) {
    ALOGI("soter just died");
    soterAidl = nullptr;
}

std::shared_ptr<ISoterAidl> getSoterAidl() {
    if(soterAidl == nullptr) {
        const std::string instance = std::string() + ISoterAidl::descriptor + "/default";
        auto soterBinder = ::ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str()));
        if (soterBinder.get() == nullptr) {
            ALOGE("soter service doesn't exist");
            return soterAidl;
        }
        auto deathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(AIBinder_DeathRecipient_new(&serviceDiedAidl));
        auto status = ::ndk::ScopedAStatus::fromStatus(AIBinder_linkToDeath(soterBinder.get(), deathRecipient.get(), (void*)serviceDiedAidl));
        if (!status.isOk()) {
            ALOGE("linking soter service to death failed: %d: %s", status.getStatus(), status.getMessage());
            return soterAidl;
        }
        soterAidl = ISoterAidl::fromBinder(soterBinder);
        if (soterAidl == nullptr) {
            ALOGE("soter service doesn't exist");
            return soterAidl;
        }
        ALOGE("soter service linked to death!!");
    }
    return soterAidl;
}

uint32_t GetDeviceId(void)
{
    std::vector<uint8_t> deviceId;
    size_t deviceIdLength;
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK);
    SoterBufferReturn tmpBuf;
    SoterErrorCodeAidl tmpRetCode;

    ALOGI("Trying to connect to service ISoter aidl");
    std::shared_ptr<ISoterAidl> soterAidl_ = getSoterAidl();
    if (soterAidl_.get() == nullptr) {
        ALOGE("Failed to get Soter service");
        return static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OTHERS);
    }

    ALOGI("calling Soter service api : getDeviceId()");
    auto rc = soterAidl_->getDeviceId(&tmpBuf, &tmpRetCode);
    ret = static_cast<uint32_t>(tmpRetCode);
    deviceIdLength = tmpBuf.dataLength;
    deviceId = tmpBuf.data;
    if (!rc.isOk() || (ret != static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK))) {
        ALOGE("Failed to get device id, error %d", ret);
        printf("fail %d\n", ret);
    } else {
        for(auto p = deviceId.begin(); p!=deviceId.end(); ++p)
            printf("%02x",*p);
        printf("\n");
    }
    return ret;
}

uint32_t VerifyAttkKeyPair(void)
{
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR);
    SoterErrorCodeAidl tmpRetCode;

    ALOGI("Trying to connect to service ISoter aidl");
    std::shared_ptr<ISoterAidl> soterAidl_ = getSoterAidl();
    if (soterAidl_.get() == nullptr) {
        ALOGE("Failed to get Soter service");
        return static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OTHERS);
    }

    ALOGI("calling Soter service api : verifyAttkKeyPair()");
    auto rc = soterAidl_->verifyAttkKeyPair(&tmpRetCode);
    ret = static_cast<uint32_t>(tmpRetCode);
    if (!rc.isOk() || ret != static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK)) {
        ALOGE("Failed to VerifyAttkKeyPair, error %d", ret);
        printf("fail %d\n", ret);
    } else
        printf("pass\n");

    return ret;
}

uint32_t ExportAttkKeyPair(void)
{
    uint32_t PubKeyLen;
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR);
    std::vector<uint8_t> pubKeyData;
    SoterBufferReturn tmpBuf;
    SoterErrorCodeAidl tmpRetCode;

    ALOGI("Trying to connect to service ISoter aidl");
    std::shared_ptr<ISoterAidl> soterAidl_ = getSoterAidl();
    if (soterAidl_.get() == nullptr) {
        ALOGE("Failed to get Soter service");
        return static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OTHERS);
    }

    ALOGI("calling Soter service api : exportAttkPublicKey()");
    auto rc = soterAidl_->exportAttkPublicKey(&tmpBuf, &tmpRetCode);
    ret = static_cast<uint32_t>(tmpRetCode);
    PubKeyLen = tmpBuf.dataLength;
    pubKeyData = tmpBuf.data;
    if (!rc.isOk() || (ret != static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK))) {
        ALOGE("Failed to export attk, error %d", ret);
        printf("fail %d\n", ret);
    } else {
        printf("PubKeyLen %d\n", PubKeyLen);
        std::string str(pubKeyData.begin(), pubKeyData.end());
        std::cout << str;
    }

    return ret;
}

uint32_t GenerateAttkKeyPair(void)
{
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR);
    uint8_t copy_num = ATTK_COPY_NUM_DEFAULT;
    SoterErrorCodeAidl status = SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR;

    ALOGI("Trying to connect to service ISoter aidl");
    std::shared_ptr<ISoterAidl> soterAidl_ = getSoterAidl();
    if (soterAidl_.get() == nullptr) {
        ALOGE("Failed to get Soter service");
        return static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OTHERS);
    }

    ALOGI("calling Soter service api : generateAttkKeyPair()");
    auto rc = soterAidl_->generateAttkKeyPair(copy_num, &status);
    ret = static_cast<uint32_t>(status);
    if (!rc.isOk() || (ret != static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK))) {
        ALOGE("Failed to generate attk, error %d", ret);
        printf("fail %d\n", ret);
    } else
        printf("pass\n");

    return ret;
}

uint32_t RunSoterCmd(char *argv[])
{
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR);

    if(argv == NULL) {
        ALOGE("No arguments to process, exiting!");
        return static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OTHERS);
    }
    /* CMD_ID */
    switch(atoi(argv[1])) {
    case CMD_GENERATE_ATTK_KEY_PAIR:
        ret = GenerateAttkKeyPair();
        break;
    case CMD_VERIFY_ATTK_KEY_PAIR:
        ret = VerifyAttkKeyPair();
        break;
    case CMD_EXPORT_ATTK_PUBLIC_KEY:
        ret = ExportAttkKeyPair();
        break;
    case CMD_GET_DEVICE_ID:
        ret = GetDeviceId();
        break;
    default:
        ALOGD("command not supported.");
        qsc_usage();
        break;
    }

    return ret;
}

int main(int argc, char **argv)
{
    uint32_t ret = static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_UNKNOWN_ERROR);

    if( !argv || (argc != 2) || (!strcmp(argv[1],"h")))
        qsc_usage();
    else {
        ret = RunSoterCmd(argv);
        if (ret != static_cast<uint32_t>(SoterErrorCodeAidl::SOTER_ERROR_OK))
            ALOGE("qsc_run_soter_cmd failed,ret=%d", ret);
    }
    return static_cast<int>(ret);
}
