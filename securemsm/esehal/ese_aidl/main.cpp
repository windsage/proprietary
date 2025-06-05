/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/android/hardware/secure_element/BnSecureElement.h>
#include <android-base/hex.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <algorithm>

#include <TEE_client_api.h>
#include "QSEEComAPI.h"
#include "gpqese/gpqese-be.h"
#include "gpqese/gpqese-apdu.h"
#include "EseUtils.h"
#include "OsuHalExtn.h"
#include "OsuHelper.h"

using aidl::android::hardware::secure_element::BnSecureElement;
using aidl::android::hardware::secure_element::ISecureElementCallback;
using aidl::android::hardware::secure_element::LogicalChannelResponse;
using ndk::ScopedAStatus;

class QtiSecureElement : public BnSecureElement {
  public:
    QtiSecureElement() {}

    /*** init ***/

    ScopedAStatus init(const std::shared_ptr<ISecureElementCallback>& client_callback) override {
        LOG(INFO) << __func__ << " callback: " << client_callback.get();
        if (client_callback == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
        }
        client_callback_ = client_callback;

        TEEC_Result result = ese_init();
        if (result == TEEC_SUCCESS) {
            client_callback_->onStateChange(true, "init");
            return ScopedAStatus::ok();
        }
        return ScopedAStatus::fromServiceSpecificError(FAILED);
    }

    /*** getAtr ***/

    ScopedAStatus getAtr(std::vector<uint8_t>* aidl_return) override {
        std::vector<uint8_t> select_response;
        uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
        uint32_t rLen = 0;
        TEEC_Result res_gpese_open;
        TEEC_Result result;
        LOG(INFO) << __func__;
        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        result = gpqese_getATR(rApdu, &rLen);
        if (result == TEEC_SUCCESS) {
            select_response.resize(rLen);
            memscpy(&select_response[0], rLen, rApdu, rLen);
            goto ret_handle;
        } else {
            LOG(INFO) << __func__ << "getATR Failed returning false atr!";
            goto ret_handle;
        }

        ret_handle:
          *aidl_return = select_response;
          return ScopedAStatus::ok();
    }

    /*** reset ***/

    ScopedAStatus reset() override {
        TEEC_Result result = TEEC_SUCCESS;
        LOG(INFO) << __func__;
        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        client_callback_->onStateChange(false, "reset");
        result = gpqese_close(result);
        if(result == TEEC_SUCCESS) {
            if(ese_init() == TEEC_SUCCESS) {
                bATRcached = false;
                client_callback_->onStateChange(true, "reset");
                return ScopedAStatus::ok();
            }
        }

        return ScopedAStatus::fromServiceSpecificError(FAILED);
    }

    /*** isCardPresent ***/

    ScopedAStatus isCardPresent(bool* aidl_return) override {
        LOG(INFO) << __func__;
        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }
        *aidl_return = true;
        return ScopedAStatus::ok();
    }

    /*** openBasicChannel ***/

    ScopedAStatus openBasicChannel(const std::vector<uint8_t>& aid, int8_t p2,
                                   std::vector<uint8_t>* aidl_return) override {
        uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
        int rLen = 0;
        int cNumber = 0;
        TEEC_Result result = TEEC_SUCCESS;
        LOG(INFO) << __func__;

        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        std::vector<uint8_t> select_response;

        if (isOSUMode(BASIC_CHANNEL_OP, aid)) {
            result = handleNxpOsuOpenBasicChannel(aid, p2, aidl_return);
            if (result == TEEC_SUCCESS) {
                *aidl_return = select_response;
                return ScopedAStatus::ok();
            } else {
                LOG(INFO) << __func__ << " handleNxpOsuOpenBasicChannel failed.";
                return ScopedAStatus::fromServiceSpecificError(CHANNEL_NOT_AVAILABLE);
            }
        }

        result = openChannel(aid, p2, &cNumber, rApdu, &rLen, true);
        if (result == TEEC_SUCCESS) {
            select_response.resize(rLen);
            memscpy(&select_response[0], rLen, rApdu, rLen);
        } else if (result == TEEC_ERROR_OUT_OF_MEMORY) {
            return ScopedAStatus::fromServiceSpecificError(CHANNEL_NOT_AVAILABLE);
        } else if (result == TEEC_ERROR_ITEM_NOT_FOUND) {
            closeGPChannel(cNumber);
            return ScopedAStatus::fromServiceSpecificError(NO_SUCH_ELEMENT_ERROR);
        } else {
            LOG(INFO) << __func__ << " openChannel call failed : " << result;
            return ScopedAStatus::fromServiceSpecificError(FAILED);
        }

        *aidl_return = select_response;
        return ScopedAStatus::ok();
    }

    /*** openLogicalChannel ***/

    ScopedAStatus openLogicalChannel(
            const std::vector<uint8_t>& aid, int8_t p2,
            ::aidl::android::hardware::secure_element::LogicalChannelResponse* aidl_return)
            override {
        LOG(INFO) << __func__;

        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        std::vector<uint8_t> select_response;

        uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
        int rLen = 0;
        int cNumber = 0;

        if (isOSUMode(LOGICAL_CHANNEL_OP, aid)) {
            ALOGE("%s: Not allowed, OSU update in progress", __func__);
            return ScopedAStatus::fromServiceSpecificError(FAILED);
        }

        TEEC_Result result = openChannel(aid, p2, &cNumber, rApdu, &rLen, false);
        if (result == TEEC_SUCCESS) {
            select_response.resize(rLen);
            memscpy(&select_response[0], rLen, rApdu, rLen);
        } else if (result == TEEC_ERROR_OUT_OF_MEMORY) {
            LOG(INFO) << __func__ << " Out of memory";
             *aidl_return = LogicalChannelResponse{
                    .channelNumber = static_cast<int8_t>(cNumber),
                    .selectResponse = select_response,
            };
            return ScopedAStatus::fromServiceSpecificError(CHANNEL_NOT_AVAILABLE);
        } else if (result == TEEC_ERROR_ITEM_NOT_FOUND) {
            LOG(INFO) << __func__ << " Item not found";
            *aidl_return = LogicalChannelResponse{
                    .channelNumber = static_cast<int8_t>(cNumber),
                    .selectResponse = select_response,
            };
            closeGPChannel(cNumber);
            return ScopedAStatus::fromServiceSpecificError(NO_SUCH_ELEMENT_ERROR);
        } else {
            LOG(INFO) << __func__ << " openChannel call failed : " << result;
            return ScopedAStatus::fromServiceSpecificError(FAILED);
        }


        *aidl_return = LogicalChannelResponse{
                .channelNumber = static_cast<int8_t>(cNumber),
                .selectResponse = select_response,
        };
        LOG(INFO) << __func__ << "eSE channel : " << cNumber;
        return ScopedAStatus::ok();
    }

    /*** closeChannel ***/

    ScopedAStatus closeChannel(int8_t channel_number) override {
        LOG(INFO) << __func__ << " channel number: " << static_cast<int>(channel_number);
        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }
        // update OSU state
        IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, channel_number);

        TEEC_Result result = closeGPChannel(channel_number);
        if (result == TEEC_SUCCESS) {
            return ScopedAStatus::ok();
        }
        return ScopedAStatus::fromServiceSpecificError(FAILED);
    }

    ScopedAStatus transmit(const std::vector<uint8_t>& data,
                           std::vector<uint8_t>* aidl_return) override {
        LOG(INFO) << __func__;
        if (client_callback_ == nullptr) {
            return ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        std::vector<uint8_t> response_apdu;
        uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
        int rLen = 0;
        TEEC_Result result;

        if (IS_OSU_MODE(OsuHalExtn::GETATR)) {
            result = handleNxpOsuTransmit(data, aidl_return);
            if (result == TEEC_SUCCESS) {
                LOG(INFO) << __func__ << " transmit OSU completed";
                return ScopedAStatus::ok();
            } else {
                LOG(INFO) << __func__ << " transmit OSU failed";
                return ScopedAStatus::fromServiceSpecificError(FAILED);
            }
        }

        result = transmitApdu(data, false, INVALID_CHANNEL_NUM, rApdu, &rLen);
        if (result == TEEC_SUCCESS) {
            response_apdu.resize(rLen);
            memscpy(&response_apdu[0], rLen, rApdu, rLen);
            aidl_return->assign(response_apdu.begin(), response_apdu.end());
            return ScopedAStatus::ok();
        } else {
            ALOGE("Transmit FAILED !");
        }
        return ScopedAStatus::fromServiceSpecificError(FAILED);
    }

  private:
    // OMAPI abstraction.
    std::shared_ptr<ISecureElementCallback> client_callback_{nullptr};
};

static int initEseTrustedApp(void) {
    std::string app_uuid = "";
    std::string app_name = "";
    char ese_hw_name[PROPERTY_VALUE_MAX] = {0};
    int len = 0;
    int ret = 0;
    struct QSEECom_handle* qseeComHandleeSEService = NULL;
    uint8_t ese_vendor_id = eseGetVendorId();

    if (ese_vendor_id == ESE_VENDOR_NXP) {
        app_uuid = TRUSTED_APP_UUID_NXP;
        app_name = TRUSTED_APP_NXP;
    } else if (ese_vendor_id == ESE_VENDOR_STM) {
        app_uuid = TRUSTED_APP_UUID_STM;
	app_name = TRUSTED_APP_STM;
    } else {
        ALOGE("Unknown ese vendor");
        return -1;
    }

    // Power ON eSE for RoT establishment usecase during trusted application loading
    ret = eseSetPower(true);
    if (ret != 0) {
        ALOGE("Failed setEsePower on");
        return -1;
    }

    // to maintain backward compatibility we need to try both UUID and name based TA loading
    ALOGD("loading %s", app_name.c_str());
    ret = QSEECom_start_app(&qseeComHandleeSEService, TA_APP_PATH, app_name.c_str(), 1024);
    if (ret) {
        ALOGD("%s app loading failed. Trying loading with UUID.", app_name.c_str());
        ret = QSEECom_start_app(&qseeComHandleeSEService, TA_APP_PATH, app_uuid.c_str(), 1024);
        if (ret) {
            ALOGD("%s application loading with UUID failed", app_uuid.c_str());
        }
    }

    eseSetPower(false);

    return ret;
}

int main() {
    int ret;
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    ret = initEseTrustedApp();
    if (ret != 0) {
        ALOGE("Failed on initalize eSE TA");
        return EXIT_FAILURE;
    }

    auto se = ndk::SharedRefBase::make<QtiSecureElement>();
    const std::string name = std::string() + BnSecureElement::descriptor + "/eSE1";
    binder_status_t status = AServiceManager_addService(se->asBinder().get(), name.c_str());
    CHECK_EQ(status, STATUS_OK);

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
