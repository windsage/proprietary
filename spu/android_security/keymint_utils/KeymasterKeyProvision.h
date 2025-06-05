/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef KEYMASTERKEYPROVISION_H_
#define KEYMASTERKEYPROVISION_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <string>

#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>

#include <KeymasterBuffer.h>
#include <KeymasterUtils.h>

using namespace keymasterutils;
using namespace std;

namespace keymasterdevice {

#define MAX_KEYS_IN_KEYBOX 2
#define KM_MAX_SUBS 2

typedef struct {
    keymaster_algorithm_t algo;
    keymaster_key_blob_t key;
    keymaster_cert_chain_t cert_chain;
} keybox_t;

typedef struct {
    keymaster_blob_t deviceId;
    keybox_t keybox[MAX_KEYS_IN_KEYBOX];
} km_install_keybox_t;

typedef struct {
    size_t dev_id_count;
    string dev_id[KM_MAX_SUBS];
} keymaster_device_id_t;

class KeymasterKeyProvision {
  public:
    KeymasterKeyProvision(keymaster_security_level_t security_level)
        : utils(new KeymasterUtils(security_level)), msecurityLevel(security_level) {}
    virtual ~KeymasterKeyProvision() {
        utils.reset();
    }

    keymaster_error_t KeyMasterProvisionInit(void);

    keymaster_error_t KeyMasterProvisionDeviceId(string Brand, string Device,
            string Product, string SerialNum, string Manufacturer, string Model,
            keymaster_device_id_t Imei, keymaster_device_id_t Meid,
            uint8_t *prov_cmd_buffer_out, uint32_t *prov_cmd_buffer_len_out);

    keymaster_error_t KeyMasterInstallKeybox(km_install_keybox_t* keybox,
            uint8_t *prov_cmd_buffer_out,
            uint32_t *prov_cmd_buffer_len_out);

    keymaster_error_t SpuInit(void);

  private:
    void CopyCmd(uint32_t req_len, uint8_t *cmd_copy_out,
                 uint32_t *cmd_copy_len_out);

    std::unique_ptr<KeymasterUtils> utils;
    keymaster_security_level_t msecurityLevel;
};

}  // namespace keymasterdevice

#endif  // KEYMASTERKEYPROVISION_H_
