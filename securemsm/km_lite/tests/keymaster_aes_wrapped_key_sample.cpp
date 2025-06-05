/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <keymaster_defs.h>
#include <keymaster.h>
#include <keymaster_utils.h>

extern "C" {
#include "sample_utils.h"
}

#define LOGE_PRINT(fmt, ...) \
   do { \
     printf("[%s:%u] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
} while(0)

#define AES_EXPECTED_KEY_EXPORT_SIZE 100

static int __getFileSize(std::string const &filename)
{
    FILE *file  = NULL;
    int size    = -1;
    int ret     = 0;

    file = fopen(filename.c_str(), "r");
    if (file == NULL) {
      LOGE_PRINT("Failed to open file %s: %s (%d)", filename.c_str(), strerror(errno), errno);
      goto ret_handle;
    }

    ret = fseek(file, 0L, SEEK_END);
    if (ret) {
      LOGE_PRINT("Error seeking in file %s: %s (%d)", filename.c_str(), strerror(errno), errno);
      goto ret_handle;
    }

    size = ftell(file);
    if (size == -1) {
      LOGE_PRINT("Error telling size of file %s: %s (%d)", filename.c_str(), strerror(errno), errno);
      goto ret_handle;
    }

ret_handle:
    if (file) {
      fclose(file);
    }
    return size;
}

static int __readFile(std::string const &filename, size_t size, uint8_t *buffer)
{
    FILE *file        = NULL;
    size_t readBytes  = 0;
    int ret           = -1;

    file = fopen(filename.c_str(), "r");
    if (file == NULL) {
      LOGE_PRINT("Failed to open file %s: %s (%d)", filename.c_str(), strerror(errno), errno);
      goto ret_handle;
    }
    readBytes = fread(buffer, 1, size, file);
    if (readBytes != size) {
      LOGE_PRINT("Error reading the file %s: %zu vs %zu bytes: %s (%d)",
        filename.c_str(),
        readBytes,
        size,
        strerror(errno),
        errno);
        goto ret_handle;
    }
    ret = size;

ret_handle:
    if (file) {
      fclose(file);
    }
    return ret;
}


static int __writeFile(std::string const &filename, size_t size, uint8_t *buffer)
{
    FILE *file         = NULL;
    size_t writeBytes  = 0;
    int ret            = -1;

    file = fopen(filename.c_str(), "w");
    if (file == NULL) {
      LOGE_PRINT("Failed to open file %s: %s (%d)", filename.c_str(), strerror(errno), errno);
      goto ret_handle;
    }
    writeBytes = fwrite(buffer, 1, size, file);
    if (writeBytes != size) {
      LOGE_PRINT("Error reading the file %s: %zu vs %zu bytes: %s (%d)",
        filename.c_str(),
        writeBytes,
        size,
        strerror(errno),
        errno);
       goto ret_handle;
     }
     ret = size;

ret_handle:
     if (file) {
       fclose(file);
     }
     return ret;
}

static keymaster_error_t GetAesKeyExportDeUser() {
    keymaster_key_characteristics_t aes_key_characteristics_de0;
    keymaster_key_blob_t aes_key_blob_de0 = {0};
    keymaster_error_t err;
    int ret = 0;

    //Generate DE0 key and save in storage
    keymaster_key_param_t params_de_rollback[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_AES),
            keymaster_param_int(KM_TAG_KEY_SIZE, 256),

            keymaster_param_int(KM_TAG_USER_ID, 400),
            keymaster_param_bool(KM_STORAGE_KEY),
            keymaster_param_bool(KM_TAG_ROLLBACK_RESISTANCE),
            keymaster_param_int(KM_TAG_KEY_TYPE, 0),

            //allow encrypt and decrypt operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_ENCRYPT),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_DECRYPT),

    };
    const keymaster_key_param_set_t param_set_de_rollback = {
            .params = params_de_rollback,
            .length = sizeof(params_de_rollback)/sizeof(params_de_rollback[0]),
    };

    err = km_generate_key(&param_set_de_rollback, &aes_key_blob_de0,
                         &aes_key_characteristics_de0);

    if (err == KM_ERROR_ROLLBACK_RESISTANCE_UNAVAILABLE) {
        printf("Generate key failed with err %d , so try to generate another non-rollback resistant key", err);
        keymaster_key_param_t params_de[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_AES),
            keymaster_param_int(KM_TAG_KEY_SIZE, 256),

            keymaster_param_int(KM_TAG_USER_ID, 400),
            keymaster_param_bool(KM_STORAGE_KEY),
            keymaster_param_int(KM_TAG_KEY_TYPE, 0),

            //allow encrypt and decrypt operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_ENCRYPT),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_DECRYPT),
        };
        const keymaster_key_param_set_t param_set_de = {
            .params = params_de,
            .length = sizeof(params_de)/sizeof(params_de[0]),
        };
        err = km_generate_key(&param_set_de, &aes_key_blob_de0,
                         &aes_key_characteristics_de0);
    }
    if (err != KM_ERROR_OK)
        return err;

    printf("AES key generated successfully with key size of ");
    print_tag_value_from_characterstic(KM_TAG_KEY_SIZE, &aes_key_characteristics_de0);
    printf("\n");

    std::string keyblob_de0_path =  "/data/gen_keyblob_de0";
    ret = __writeFile(keyblob_de0_path, aes_key_blob_de0.key_material_size,  const_cast<uint8_t*>(aes_key_blob_de0.key_material));
    if (ret <= 0) {
        printf("Write File failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }

    keymaster_blob_t exported_key_data_de0 = {0};
    err = km_export_key(KM_KEY_FORMAT_RAW , &aes_key_blob_de0,
             NULL, NULL, &exported_key_data_de0);

    if (err != KM_ERROR_OK)
        return err;

    std::string aes_export_de0_path =  "/data/aes_export_de0";
    ret = __writeFile(aes_export_de0_path, exported_key_data_de0.data_length,  const_cast<uint8_t*>(exported_key_data_de0.data));
    if (ret <= 0) {
        printf("Write File failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }
    printf("aes_key_export_de_user of length %zu is = ", exported_key_data_de0.data_length);
    hexdump(exported_key_data_de0.data, exported_key_data_de0.data_length);
    printf("\n");

    keymaster_free_key_blob(&aes_key_blob_de0);
    keymaster_free_blob(&exported_key_data_de0);
    keymaster_free_characteristics(&aes_key_characteristics_de0);

    printf("Test completed successfully!");
    return KM_ERROR_OK;
}

static keymaster_error_t RebootAndAesKeyExport_DeUser() {
    keymaster_error_t err;
    int ret = 0;
    size_t size = 0;

    keymaster_key_blob_t keyblob_str_de0 = {0};
    std::string keyblob_str_de0_path = "/data/gen_keyblob_de0";
    ret = __getFileSize(keyblob_str_de0_path);
    if (ret <= 0) {
        printf("getFileSize failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }
    size = (size_t)ret;
    keyblob_str_de0.key_material = new uint8_t[size];
    ret = __readFile(keyblob_str_de0_path, size, const_cast<uint8_t*>(keyblob_str_de0.key_material));
    if (ret <= 0) {
        printf("readFile failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }
    keyblob_str_de0.key_material_size = size;
    if (!keyblob_str_de0.key_material_size)
        return KM_ERROR_UNKNOWN_ERROR;

    keymaster_blob_t export_data_de_2= {0};
    err = km_export_key(KM_KEY_FORMAT_RAW , &keyblob_str_de0,
             NULL, NULL, &export_data_de_2);

    if (err != KM_ERROR_OK)
        return err;
    if (AES_EXPECTED_KEY_EXPORT_SIZE != export_data_de_2.data_length)
        return err;

    printf("aes_key_export_de_user of size %zu is = " , export_data_de_2.data_length);
    hexdump(export_data_de_2.data, export_data_de_2.data_length);
    printf("\n");

    std::string exp_path_de2 =  "/data/exported_key_de_2";
    ret = __writeFile(exp_path_de2, export_data_de_2.data_length,  const_cast<uint8_t*>(export_data_de_2.data));
    if (ret <= 0) {
        printf("Write File failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }

    keymaster_blob_t read_de0 = {0};
    std::string read_de0_path = "/data/aes_export_de0";
    ret = __getFileSize(read_de0_path);
    if (ret <= 0) {
        printf("getFileSize failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }
    size = (size_t)ret;
    read_de0.data = new uint8_t[size];
    ret = __readFile(read_de0_path, size, const_cast<uint8_t*>(read_de0.data));
    if (ret <= 0) {
        printf("readFile failed with error %d", ret);
        return KM_ERROR_UNKNOWN_ERROR;
    }
    read_de0.data_length = size;
    if (!read_de0.data_length)
        return KM_ERROR_UNKNOWN_ERROR;
    if (AES_EXPECTED_KEY_EXPORT_SIZE != read_de0.data_length)
        return err;

    //Compare DE0
    //wrapped data should not match
     if(!memcmp(export_data_de_2.data, read_de0.data, AES_EXPECTED_KEY_EXPORT_SIZE)) {
         printf("Wrapped data should not match, testcase failed!");
         return KM_ERROR_UNKNOWN_ERROR;
     }
    printf("Test passed for DE key");
    return KM_ERROR_OK;
}

keymaster_error_t GetAesKeyExportDeUserSuccess(void) {
    keymaster_error_t err = initialize_keymaster();
    if (err != KM_ERROR_OK)
        return err;

    err = GetAesKeyExportDeUser();
    if (err != KM_ERROR_OK)
        return err;

    return KM_ERROR_OK;
}

keymaster_error_t  RebootAndAesKeyExportDeUser(void) {
    keymaster_error_t err = initialize_keymaster();
    if (err != KM_ERROR_OK)
        return err;
    //De user
    err = RebootAndAesKeyExport_DeUser();
    if (err != KM_ERROR_OK)
        return err;

    return KM_ERROR_OK;
}
