/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef KEYMASTER_UTILS_KEYMASTERUTILS_H_
#define KEYMASTER_UTILS_KEYMASTERUTILS_H_

#include <QSEEComAPI.h>
#include <hardware/keymaster_defs.h>
#if ENABLE_SPU
#include <linux/msm_ion.h>
#endif
#if ENABLE_SMCINVOKE_KM
#include<object.h>
#endif

#define KEYMASTER_MODULE_API_VERSION_3_0 (3)
#define KEYMASTER_MODULE_API_VERSION_4_0 (4)
#define KEYMASTER_MODULE_HAL_MINOR_VERSION (5)
#define QTI_KM_SHARED_BUF_LEN (40 * 1024)
#define QTI_KM_INPUT_BUF_LEN (16 * 1024)
#define QTI_KM_SPU_INPUT_BUF_LEN (8 * 1024)
#define SPU_BUF_SIZE (20 * 1024)

namespace keymasterutils {

#define GUARD(code)     \
    do {                \
        ret = (code);   \
        if (ret != 0) { \
            return ret; \
        };              \
    } while (0)

#define GUARD_MSG(code)     \
    do {                \
        ret = (code);   \
        if (ret != 0) { \
            ALOGE("%s", __func__);               \
            ALOGE("ret: %d", ret);              \
            return ret; \
        };              \
    } while (0)

#define GUARD_BAIL(code, status)                 \
    do {                                         \
        if ((code) || (status != KM_ERROR_OK)) { \
            ALOGE("%s", __func__);               \
            ALOGE("ret: %d", code);              \
            ALOGE("resp->status: %d", status);   \
            if (ret)                             \
                return ret;                      \
            else                                 \
                ret = (keymaster_error_t)status; \
            return ret;                          \
        }                                        \
    } while (0)

#if ENABLE_SPU

typedef struct {
    int32_t ion_fd;
    int32_t ifd_data_fd;
    uint32_t sbuf_len;
} sp_km_ion_info;

#endif

typedef struct {
    keymaster_security_level_t level;
    uint32_t major_version = 0;
    uint32_t minor_version = 0;
    uint32_t ta_major_version = 0;
    uint32_t ta_minor_version = 0;
    // struct QSEECom_handle *qseecom = NULL;
    void* qseecom_handle = NULL;
#if ENABLE_SMCINVOKE_KM
    Object appObj = Object_NULL;
    void* km_ta_buf = NULL;
#endif
#if ENABLE_SPU
    sp_km_ion_info ion_info;
    uint32_t spu_initialized = 0;
    uint32_t shared_buf_allocated = 0;
    uint32_t spu_available;
#endif
} ctx_t;

class KeymasterUtils {
  public:
    KeymasterUtils(keymaster_security_level_t security_level);
    virtual ~KeymasterUtils();

    size_t km_memscpy(void* dst, size_t dst_size, const void* src, size_t src_size);

    keymaster_error_t initialize(size_t size);
    uint32_t getKeymasterVersion(void);
    uint32_t getKeymasterAPIVersion(void);
    uint32_t getKeymasterTaMinorVersion(void);
    uint32_t getKeymasterHalMinorVersion(void);
    void* qseecom_dev_init(void);
    keymaster_error_t send_cmd(void* send_buf, uint32_t sbuf_len, void* resp_buf,
                               uint32_t rbuf_len);
    void IsTeeKmAvailable(void);
    bool isLegacy();
    bool isOldKeyblob();
    void init(uint32_t cmd_id, keymaster_blob_t* req);
    keymaster_error_t sendCmd(const keymaster_blob_t& req, keymaster_blob_t* rsp);
    keymaster_error_t spu_init(void);
#if ENABLE_SPU
    keymaster_error_t IsSbKmAvailable(void);
    keymaster_error_t spu_ssr_reset_flags(void);
#endif
  private:
    ctx_t* ctx;
};

}  // namespace keymasterutils
#endif /* KEYMASTER_UTILS_KEYMASTERUTILS_H_ */
