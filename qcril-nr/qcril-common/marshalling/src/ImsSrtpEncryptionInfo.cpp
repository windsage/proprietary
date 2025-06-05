/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsSrtpEncryptionInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SrtpEncryptionStatus>(const RIL_IMS_SrtpEncryptionStatus& arg) {
    WRITE_AND_CHECK(arg.callId);
    WRITE_AND_CHECK(arg.categories);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SrtpEncryptionStatus>(RIL_IMS_SrtpEncryptionStatus& arg) const {
    READ_AND_CHECK(arg.callId);
    READ_AND_CHECK(arg.categories);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SrtpEncryptionStatus>(RIL_IMS_SrtpEncryptionStatus& arg) {
    release(arg.callId);
    release(arg.categories);
    return Marshal::Result::SUCCESS;
}
