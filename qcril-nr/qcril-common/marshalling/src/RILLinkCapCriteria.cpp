/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILLinkCapCriteria.h>

template <>
Marshal::Result Marshal::write<RIL_LinkCapCriteria>(const RIL_LinkCapCriteria &arg) {
    WRITE_AND_CHECK(arg.hysteresisMs);
    WRITE_AND_CHECK(arg.hysteresisDlKbps);
    WRITE_AND_CHECK(arg.hysteresisUlKbps);
    WRITE_AND_CHECK(arg.thresholdsDownLength);
    if (arg.thresholdsDownlinkKbps != NULL) {
        for (int i = 0; i < arg.thresholdsDownLength; i++) {
            WRITE_AND_CHECK(arg.thresholdsDownlinkKbps[i]);
        }
    }
    WRITE_AND_CHECK(arg.thresholdsUpLength);
    if (arg.thresholdsUplinkKbps != NULL) {
        for (int i = 0; i < arg.thresholdsUpLength; i++) {
            WRITE_AND_CHECK(arg.thresholdsUplinkKbps[i]);
        }
    }
    WRITE_AND_CHECK(static_cast<uint8_t>(arg.ran));
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LinkCapCriteria>(RIL_LinkCapCriteria &arg) const {
    uint8_t val;
    size_t downLength, upLength;
    Marshal::Result ret = Result::SUCCESS;
    READ_AND_CHECK(arg.hysteresisMs);
    READ_AND_CHECK(arg.hysteresisDlKbps);
    READ_AND_CHECK(arg.hysteresisUlKbps);
    RUN_AND_CHECK(readAndAlloc(arg.thresholdsDownlinkKbps,
                               downLength));
    arg.thresholdsDownLength = downLength;
    /* Don't use RUN_AND_CHECK here because we need
     * to free arg.thresholdsDownlinkKbps on
     * failure before returning.
     */
    ret = readAndAlloc(arg.thresholdsUplinkKbps,
                       upLength);
    arg.thresholdsUpLength = upLength;
    if (ret != Result::SUCCESS) {
        // Free Downlinkdata if it was allocated before this.
        if (arg.thresholdsDownlinkKbps) {
            release(arg.thresholdsDownlinkKbps, arg.thresholdsDownLength);
        }
        return ret;
    }
    READ_AND_CHECK(val);
    arg.ran = static_cast<RIL_RAN>(val);
    return ret;
}

template <>
Marshal::Result Marshal::release<RIL_LinkCapCriteria>(RIL_LinkCapCriteria &arg)
{
    release(arg.hysteresisMs);
    release(arg.hysteresisDlKbps);
    release(arg.hysteresisUlKbps);
    release(arg.thresholdsDownlinkKbps, arg.thresholdsDownLength);
    release(arg.thresholdsUplinkKbps, arg.thresholdsUpLength);
    release(arg.ran);
    return Result::SUCCESS;
}
