/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <dlfcn.h>
#include <fcntl.h>
#include <map>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

#include "InstallKeybox.h"
#include "keymaster_tags.h"
#include <VerifyAttestation.h>

#ifdef ANDROID
 #include <ion/ion.h>
 #endif

using namespace std;

namespace keymasterdevice {

InstallKeybox::InstallKeybox():
        mKeymasterTzUtils(new KeymasterTzUtils()),
        mProvisioningClient(new KeymasterKeyProvision(keymaster::KM_SECURITY_LEVEL_STRONGBOX)) {
    keymaster_error_t ret = mProvisioningClient->KeyMasterProvisionInit();
    if (ret != KM_ERROR_OK) {
        cerr << "Failed to init keymaster: " << ret << endl;
        exit(-1);
    }
}

string InstallKeybox::getAppName(void) {
    return "spu_install_keybox";
}

int InstallKeybox::validateProvisionedKeys(bool provisionDeviceId) {
    VerifyAttestation dev(provisionDeviceId, mBrand, mDevice, mProduct, mSerialNum, mManufacturer,
                          mModel, mImei, mMeid);
    keymaster_error_t ret = dev.RsaAttestation();
    if (ret != KM_ERROR_OK) {
        cerr << "RsaAttestation failed: " << ret << endl;
        return -1;
    }

    ret = dev.EcAttestation();
    if (ret != KM_ERROR_OK) {
        cerr << "EcAttestation failed: " << ret << endl;
        return -1;
    }

    return lockKeybox();
}

InstallKeybox::~InstallKeybox() {
    mProvisioningClient.reset();
    mKeymasterTzUtils.reset();
}

};  // end of namespace keymasterdevice.

using namespace keymasterdevice;

typedef enum {
    ARG_PROGRAM_NAME = 0,
    ARG_DEV_ID,
    ARG_PROVISION_DEV_ID,
    MANDATORY_ARGS_NUM, // Counts the number of mandatory arguments
    ARG_ATT_KEY_FILENAME = MANDATORY_ARGS_NUM,
    ARGS_MAX_NUM // Counts the maximal number of arguments
};

void printUsage(void) {
    cout << "Usage: "
            << InstallKeybox::getAppName() << " Device_ID Provision_device_ID(true/false) [Keybox_filename]" << endl;
    cout << "For example," << endl;
    cout << "To provision device ID and attestation keys: "
            << InstallKeybox::getAppName() << " devID_0001 true keybox.xml" << endl;
    cout << "To provision device ID only: "
            << InstallKeybox::getAppName() << " devID_0001 true" << endl;
    cout << "To provision attestation keys only: "
            << InstallKeybox::getAppName() << " devID_0001 false keybox.xml" << endl;
}

int main(int argc, char *argv[])
{
    int ret = Object_OK;

    if (argc < MANDATORY_ARGS_NUM || argc > ARGS_MAX_NUM) {
        printUsage();
        return -1;
    }

    std::string deviceId(argv[ARG_DEV_ID]);
    bool provisionDeviceId = false;
    if (!memcmp(argv[ARG_PROVISION_DEV_ID], "true", strlen("true")))
        provisionDeviceId = true;
    else if (!memcmp(argv[ARG_PROVISION_DEV_ID], "false", strlen("false")))
        provisionDeviceId = false;
    else {
        printUsage();
        return -1;
    }

    InstallKeybox installer;

    //Provision device IDs
    if(provisionDeviceId) {
        ret = installer.provisionDeviceIds();
        if(ret != KM_ERROR_OK) {
            cerr << "Failed to provision device IDs: " << ret << endl;
            return ret;
        }
    }

    // Provision attest keys and validate them
    if (argc >= ARG_ATT_KEY_FILENAME + 1) {
        ret = installer.provisionKeybox(argv[ARG_ATT_KEY_FILENAME], deviceId);
        if(ret != KM_ERROR_OK) {
            cerr << "Failed to provision keybox id: " << ret << endl;
            return ret;
        }

        ret = installer.validateProvisionedKeys(provisionDeviceId);
        if(ret != KM_ERROR_OK) {
            cerr << "Failed to validate provisioned keys: " << ret << endl;
            return ret;
        }
    }

    cout << "Provisioning succeeded!" << endl;
    return 0;
}
