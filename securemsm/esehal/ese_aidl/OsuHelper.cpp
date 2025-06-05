/**
 * Copyright (c) 2023-24 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/hex.h>
#include <utils/Log.h>

#include <TEE_client_api.h>
#include "gpqese/gpqese-apdu.h"
#include "OsuHalExtn.h"
#include "OsuHelper.h"

using android::base::HexString;

bool isOSUMode(uint8_t operation, const std::vector<uint8_t>& data) {
    OsuHalExtn::OsuApduMode mode;
    uint8_t cApdu[GPQESE_MAX_CAPDU_SIZE] = {};
    uint32_t cmdLen = 0;

    switch (operation) {
        case (BASIC_CHANNEL_OP):
            //Basic channel
            mode = IS_OSU_MODE(data, OsuHalExtn::getInstance().OPENBASIC, NULL);
            if (mode == OsuHalExtn::OSU_PROP_MODE || mode == OsuHalExtn::OSU_BLOCKED_MODE) {
                return true;
            }
            break;

        case (LOGICAL_CHANNEL_OP):
            //Open Logic channel
            if (IS_OSU_MODE(OsuHalExtn::getInstance().OPENLOGICAL)) {
                return true;
            }
            break;
    }
    return false;
}

/*** handleNxpOsuOpenBasicChannel ***/

TEEC_Result handleNxpOsuOpenBasicChannel(const std::vector<uint8_t>& aid, int p2, std::vector<uint8_t>* aidl_return) {
    uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
    int rLen = 0;
    int cNumber = 0;
    std::vector<uint8_t> dummy_aid;
    std::vector<uint8_t> select_response;
    OsuHalExtn::OsuApduMode mode;
    TEEC_Result result = TEEC_SUCCESS;

    mode = IS_OSU_MODE(aid, OsuHalExtn::getInstance().OPENBASIC, NULL);
    if (mode == OsuHalExtn::OSU_PROP_MODE) {
        // start of JCOP OSU update
        // enable cold reset protection
        if (do_cold_reset_protection(true))
        {
            ALOGE("%s: cold reset protection failed", __func__);
            IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
            *aidl_return = select_response;
            return TEEC_SUCCESS;
        }
        // aid coming from OSU framework is not supported by JCOP
        result = openChannel(dummy_aid, p2, &cNumber, rApdu, &rLen, true);
    } else if (mode == OsuHalExtn::OSU_BLOCKED_MODE) {
        ALOGE("%s: Not allowed, OSU update in progress", __func__);
        *aidl_return = select_response;
        return TEEC_SUCCESS;
    }
    if (result == TEEC_SUCCESS) {
        select_response.resize(rLen);
        memscpy(&select_response[0], rLen, rApdu, rLen);
        aidl_return->assign(select_response.begin(), select_response.end());
        LOG(INFO) << __func__ << " sending response: "
            << HexString(select_response.data(), select_response.size());
        if (mode == OsuHalExtn::OSU_PROP_MODE) {
            // update TEE that OSU update started
            if (updateOSUState(OSU_STATE_STARTED)) {
                ALOGE("%s: Unable to notify TEE", __func__);
                IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
                closeGPChannel(0);
                *aidl_return = select_response;
                return TEEC_ERROR_GENERIC;
            }

            ALOGD("%s: send HARD reset, getATR and IFSC", __func__);
            if (resetJcopUpdate()) {
                ALOGE("%s: JCOP reset failed", __func__);
                IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
                closeGPChannel(0);
                return TEEC_ERROR_GENERIC;
            }
        }
        return TEEC_SUCCESS;
    } else if (result == TEEC_ERROR_OUT_OF_MEMORY) {
        ALOGE("%s: - No more channel available", __func__);
    } else if (result == TEEC_ERROR_ITEM_NOT_FOUND) {
        ALOGE("%s: - Applet not found", __func__);
        closeGPChannel(0);
    } else {
        ALOGE("%s: - unexpected error", __func__);
    }
    IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
    return result;
}

TEEC_Result handleNxpOsuTransmit(const std::vector<uint8_t>& data, std::vector<uint8_t>* aidl_return) {
    std::vector<uint8_t> response_apdu;
    std::vector<uint8_t> cmdApdu;
    uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
    uint8_t cApdu[GPQESE_MAX_CAPDU_SIZE] = {};
    uint32_t cmdLen;
    int rLen = 0;
    TEEC_Result result;

    OsuHalExtn::OsuApduMode mode = IS_OSU_MODE(
            data, OsuHalExtn::getInstance().TRANSMIT, &cmdLen, cApdu);

    if (mode == OsuHalExtn::getInstance().OSU_BLOCKED_MODE) {
        // Non-OSU APDU request while update in progress
        ALOGE("%s: Not allowed, OSU update in progress", __func__);
        *aidl_return = response_apdu;
        return TEEC_ERROR_GENERIC;
    } else if ((mode == OsuHalExtn::getInstance().OSU_RST_MODE) ||
            (mode == OsuHalExtn::getInstance().OSU_ACK_SELECT_MODE)) {
        // Request for hard reset or
        // select request with basic channel and aid NULL from OSU jar
        // return dummy success response as this command not sent further
        uint8_t sw[2] = {0x90, 0x00};
        response_apdu.resize(sizeof(sw));
        memscpy(&response_apdu[0], sizeof(sw), sw, sizeof(sw));
        *aidl_return = response_apdu;
        return TEEC_SUCCESS;
    } else if (mode == OsuHalExtn::getInstance().OSU_PROP_MODE) {
        // Prop APDU - not compliant with GP
        cmdApdu.resize(cmdLen);
        memscpy(&cmdApdu[0], cmdLen, cApdu, cmdLen);
        result = transmitApdu(cmdApdu, true, BASIC_CHANNEL_NUM, rApdu, &rLen);
        if (result == TEEC_SUCCESS) {
            response_apdu.resize(rLen);
            memscpy(&response_apdu[0], rLen, rApdu, rLen);
            aidl_return->assign(response_apdu.begin(), response_apdu.end());
            LOG(INFO) << __func__
                << " response: " << HexString(aidl_return->data(), aidl_return->size()) << " ("
                << aidl_return->size() << ")";
            return TEEC_SUCCESS;
        } else {
            ALOGE("Transmit FAILED !");
        }
    } else {
        result = transmitApdu(data, false, INVALID_CHANNEL_NUM, rApdu, &rLen);
        if (result == TEEC_SUCCESS) {
            response_apdu.resize(rLen);
            memscpy(&response_apdu[0], rLen, rApdu, rLen);
            aidl_return->assign(response_apdu.begin(), response_apdu.end());
            LOG(INFO) << __func__
                 << " response: " << HexString(aidl_return->data(), aidl_return->size()) << " ("
                 << aidl_return->size() << ")";
            return TEEC_SUCCESS;
        }
    }
    return TEEC_ERROR_GENERIC;
}
