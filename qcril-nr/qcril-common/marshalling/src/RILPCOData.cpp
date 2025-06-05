/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILPCOData.h>

template <>
Marshal::Result Marshal::write<RIL_PCO_Data>(const RIL_PCO_Data &arg) {
    WRITE_AND_CHECK(arg.cid);
    WRITE_AND_CHECK(arg.bearer_proto);
    WRITE_AND_CHECK(arg.pco_id);
    WRITE_AND_CHECK(arg.contents, arg.contents_length);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PCO_Data>(RIL_PCO_Data &arg) const {
    READ_AND_CHECK(arg.cid);
    READ_AND_CHECK(arg.bearer_proto);
    READ_AND_CHECK(arg.pco_id);
    size_t length = 0;
    RUN_AND_CHECK(readAndAlloc(arg.contents, length));
    auto res = Result::SUCCESS;
    if (length <= std::numeric_limits<int>::max()) {
        arg.contents_length = static_cast<int>(length);
    } else {
        assert(false);
        arg.contents_length = 0;
        res = Result::OVERFLOWED;
    }
    return res;
}

template <>
Marshal::Result Marshal::release<RIL_PCO_Data>(RIL_PCO_Data &arg)
{
    release(arg.cid);
    release(arg.bearer_proto);
    release(arg.pco_id);
    release(arg.contents, arg.contents_length);
    return Result::SUCCESS;
}