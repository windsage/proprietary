/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <Marshal.h>
#include <marshal/RefreshConferenceInfo.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_RefreshConferenceInfo>(const RIL_RefreshConferenceInfo& arg) {
    WRITE_AND_CHECK(arg.conferenceCallState);
    WRITE_AND_CHECK(arg.confInfoUri, arg.confInfoUriLength);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_RefreshConferenceInfo>(RIL_RefreshConferenceInfo &arg) const {
    READ_AND_CHECK(arg.conferenceCallState);
    RUN_AND_CHECK(readAndAlloc(arg.confInfoUri, arg.confInfoUriLength));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_RefreshConferenceInfo>(RIL_RefreshConferenceInfo &arg) {
    release(arg.conferenceCallState);
    release(arg.confInfoUri);
    arg.confInfoUriLength = 0;
    return Marshal::Result::SUCCESS;
}
