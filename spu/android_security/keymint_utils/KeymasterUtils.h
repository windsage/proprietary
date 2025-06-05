/*
 * Copyright (c) 2017-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SPU_KEYMASTER_UTILS_KEYMASTERUTILS_H_
#define SPU_KEYMASTER_UTILS_KEYMASTERUTILS_H_

#include <hardware/keymaster_defs.h>

#define KEYMASTER_MODULE_API_VERSION_3_0 (3)
#define KEYMASTER_MODULE_API_VERSION_4_0 (4)
#define KEYMASTER_MODULE_HAL_MINOR_VERSION (5)
#define KEYMINT_MODULE_API_VERSION (5)
#define KEYMINT_MODULE_HAL_MINOR_VERSION (3)

// The size of the shared buffer in HLOS
#define QTI_KM_SHARED_BUF_LEN (40 * 1024)

// The size of the shared buffer in SPU
#define SPU_KM_SHARED_BUF_LEN (20 * 1024)

// The max input size of UPDATE command that can be sent via shared buffer
#define SPU_KM_UPDATE_INPUT_BUF_LEN (8 * 1024)

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

struct ctx_t;

class KeymasterUtils {
  public:
    KeymasterUtils(keymaster_security_level_t security_level);
    virtual ~KeymasterUtils();

    static size_t km_memscpy(void* dst, size_t dst_size, const void* src, size_t src_size);

    keymaster_error_t initialize(size_t size);
    uint32_t getKeymasterVersion(void);
    uint32_t getKeymasterAPIVersion(void);
    uint32_t getKeymasterTaMinorVersion(void);
    uint32_t getKeymasterHalMinorVersion(void);
    void* qseecom_dev_init(void);
    keymaster_error_t send_cmd(void* send_buf, uint32_t sbuf_len, void* resp_buf,
                               uint32_t rbuf_len);
    keymaster_error_t spu_init(void);

    keymaster_error_t IsSbKmAvailable(void);
    bool isLegacy();
    bool isOldKeyblob();

    void init(uint32_t cmd_id, keymaster_blob_t* req);

    keymaster_error_t sendCmd(const keymaster_blob_t& req, keymaster_blob_t* rsp);

    void get_cmd_header(uint32_t req_len, void **cmd_hdr,
            uint32_t *cmd_total_len);

    keymaster_error_t save_configure_cmd(const keymaster_blob_t *cmd);
    keymaster_error_t save_device_locked(const keymaster_blob_t *cmd);
    void save_early_boot_ended(void);

private:
    struct ctx_t* ctx;

    void cleanup(void);
    void cleanup_fatal_if(bool cond, const char *log_msg);

    keymaster_error_t spu_hlos_shared_buf_init(void);
    keymaster_error_t spu_tz_shared_buf_init(void);

    keymaster_error_t spu_tz_prepare(void);
    keymaster_error_t spu_tz_send_data(void);
    keymaster_error_t spu_init_impl(void);

    keymaster_error_t spuCopyReq(keymaster_blob_t cmd, size_t *reqSize);
    keymaster_error_t spuCopyRsp(size_t reqSizeSpu, size_t rspSizeFromSpu, keymaster_blob_t *rsp);
    keymaster_error_t callSPU(size_t reqSize, size_t *rspSizeFromSpu);

    keymaster_error_t sendCmd_common(const keymaster_blob_t& cmd, keymaster_blob_t* rsp);

    keymaster_error_t save_cmd(const keymaster_blob_t *src_cmd,
            keymaster_blob_t& target_cmd);

    keymaster_error_t spuSetCmd(uint32_t cmd_id,
            const keymaster_blob_t& src_cmd);

    keymaster_error_t spuGetVersion(void);
    keymaster_error_t spuSetVersion(void);

    keymaster_error_t spuSetConfigure(void);
    keymaster_error_t spuSetEarlyBootEnded(void);
    keymaster_error_t spuSetDeviceLocked(void);
};

}  // namespace keymasterutils
#endif /* SPU_KEYMASTER_UTILS_KEYMASTERUTILS_H_ */
