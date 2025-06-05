/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef KEYMASTER_TZ_UTILS_KEYMASTERUTILS_H_
#define KEYMASTER_TZ_UTILS_KEYMASTERUTILS_H_

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
#include "KeymasterUtils.h"
#include <hardware/keymaster_defs.h>
#include "object.h"

using namespace std;

namespace keymasterutils {

class KeymasterTzUtils {
  public:
    KeymasterTzUtils();
    virtual ~KeymasterTzUtils();

    typedef enum {
        TZ_KEY_ID_DEVICE_IDS        = 1,
        TZ_KEY_ID_ATTASTATION_KEY   = 2,
    } tz_key_id_enum;

    keymaster_error_t tzServiceWrite(tz_key_id_enum key_id, const void *buffer,
            size_t size);
    keymaster_error_t tzServiceRead(tz_key_id_enum key_id, void *buffer,
            size_t *size);
    keymaster_error_t tzServiceLock(tz_key_id_enum key_id);
    keymaster_error_t tzServiceDelete(tz_key_id_enum key_id);
    keymaster_error_t tzSpuKmPrepare(int32_t ion_fd, bool &ion_already_exists);
    keymaster_error_t tzSpuKmInitData(void);
    keymaster_error_t tzSpuKmProvisionDeviceId(const void *request,
            size_t request_size);
    keymaster_error_t tzSpuKmProvisionAttestationKeys(const void *request,
            size_t request_size);

  private:
    struct ion_handle {
        int32_t ion_dev_fd;
        int32_t ifd_data_fd;
        uint32_t sbuf_len;
        void* ion_sbuffer;
    };

    Object mAppObj;
    Object mClientEnv;
    Object mAppController;
    Object mAppLoader;
    Object mAppClientObj;

    keymaster_error_t init(void);
    bool isInitialized(void) const;
    keymaster_error_t tzServiceLoad(void);
    void tzServiceUnload(void);
    int getFileSize(std::string const &filename);
    int readImageFileToBuffer(std::string const &filename, size_t &size,
            uint8_t *buffer);
    int loadTzApp(std::string const &path);
    int smcinvokeIonMemalloc(uint32_t size, struct ion_handle *outHandle);
    int smcinvokeIonMemfree(struct ion_handle* handle);
    keymaster_error_t allocIonMemObj(struct ion_handle *handle, uint32_t size,
            Object *memObj);
    keymaster_error_t getIonMemObj(int32_t ion_fd, Object *memObj);
    keymaster_error_t keyIdToKeyTag(tz_key_id_enum key_id, uint32_t *key_tag);
};

}  // namespace keymasterutils
#endif /* KEYMASTER_TZ_UTILS_KEYMASTERUTILS_H_ */
