/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILSliceInfo.h>

template <>
Marshal::Result Marshal::write<RIL_SliceInfo>(const RIL_SliceInfo &arg) {
    WRITE_AND_CHECK(arg.sst);
    WRITE_AND_CHECK(arg.sliceDifferentiator);
    WRITE_AND_CHECK(arg.mappedHplmnSst);
    WRITE_AND_CHECK(arg.mappedHplmnSD);
    WRITE_AND_CHECK(arg.status);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SliceInfo>(RIL_SliceInfo &arg) const {
    READ_AND_CHECK(arg.sst);
    READ_AND_CHECK(arg.sliceDifferentiator);
    READ_AND_CHECK(arg.mappedHplmnSst);
    READ_AND_CHECK(arg.mappedHplmnSD);
    READ_AND_CHECK(arg.status);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SliceInfo>(RIL_SliceInfo &arg) {
    release(arg.sst);
    release(arg.sliceDifferentiator);
    release(arg.mappedHplmnSst);
    release(arg.mappedHplmnSD);
    release(arg.status);
    return Result::SUCCESS;
}