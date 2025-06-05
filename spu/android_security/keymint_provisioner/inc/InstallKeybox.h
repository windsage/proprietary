/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef SPU_KEYMINT_INSTALLKEYBOX_H
#define SPU_KEYMINT_INSTALLKEYBOX_H

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <limits.h>
#include <utils/Log.h>

// required for glibc compile
#include <KeymasterKeyProvision.h>
#include "KeymasterTzUtils.h"

using namespace keymasterdevice;
using namespace keymasterutils;
using namespace std;

namespace keymasterdevice {

enum keybox_format { FORMAT_NONE, PEM };

typedef enum {
    ERROR_OK = 0,
    GENERIC_FAILURE = -2000,
    ATTESTATION_VERIFICATION_FAILURE = -2001,
    DEVICE_ID_NOT_FOUND = -2002,
} keybox_error;

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "SPU_INSTALL_KEYBOX"
#endif

struct smcinvoke_priv_handle {
     int32_t ion_dev_fd;
     int32_t ifd_data_fd;
     uint32_t sbuf_len;
     unsigned char* ion_sbuffer;
 };

class InstallKeybox {
  public:
    InstallKeybox();
    ~InstallKeybox();

    static string getAppName(void);

    int provisionDeviceIds();
    int provisionKeybox(string filename, string deviceId);
    int validateDeviceIds(uint8_t* buffer, size_t bufferLen);
    int validateProvisionedKeys(bool provisionDeviceId);

  private:
    int consumeTag(ifstream& inFile, const string& tag);
    string consumeElement(ifstream& inFile, const string& startTag);
    int processOneXmlKeybox(ifstream& inFile);
    int provisionDeviceID();
    int processOneXmlKeybox(ifstream& inFile, string deviceId);
    int processDeviceID(ifstream& inFile, keymaster_blob_t* deviceId);
    int processKeyAlgo(ifstream& inFile, keymaster_algorithm_t* algorithm);
    int processKeyFormat(ifstream& inFile);
    int processCertFormat(ifstream& inFile);
    int processPrivKey(ifstream& inFile, keymaster_key_blob_t* privKey);
    int processCertificate(ifstream& inFile, keymaster_blob_t* certificate);
    int cleanKeyBox(km_install_keybox_t* keyboxCtx);
    int lockKeybox(void);

    string mBrand;
    string mDevice;
    string mProduct;
    string mSerialNum;
    string mManufacturer;
    string mModel;
    keymaster_device_id_t mImei;
    keymaster_device_id_t mMeid;
    unique_ptr<KeymasterTzUtils> mKeymasterTzUtils;
    unique_ptr<KeymasterKeyProvision> mProvisioningClient;
};
};  // namespace keymasterdevice

#endif // SPU_KEYMINT_INSTALLKEYBOX_H
