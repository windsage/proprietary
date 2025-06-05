/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RadioAccessSpecifier.h>

template <>
Marshal::Result Marshal::write<RIL_RadioAccessSpecifier>(const RIL_RadioAccessSpecifier& arg) {
    WRITE_AND_CHECK(arg.radio_access_network);
    // If bands_length exceeds the maximum, return failure.
    if (arg.bands_length > MAX_BANDS) {
        return Result::FAILURE;
    }
    WRITE_AND_CHECK(arg.bands_length);
    for(uint32_t i=0; i < arg.bands_length; i++) {
        switch(arg.radio_access_network) {
            case GERAN:
                WRITE_AND_CHECK(arg.bands.geran_bands[i]);
                break;
            case UTRAN:
                WRITE_AND_CHECK(arg.bands.utran_bands[i]);
                break;
            case EUTRAN:
                WRITE_AND_CHECK(arg.bands.eutran_bands[i]);
                break;
            case NGRAN:
                WRITE_AND_CHECK(arg.bands.ngran_bands[i]);
                break;
            default:
                break;
        }
    }
    // If channels_length exceeds the maximum, return failure.
    if (arg.channels_length > MAX_CHANNELS) {
        return Result::FAILURE;
    }
    WRITE_AND_CHECK(arg.channels_length);
    for(uint32_t i=0; i < arg.channels_length; i++) {
        WRITE_AND_CHECK(arg.channels[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_RadioAccessSpecifier>(RIL_RadioAccessSpecifier& arg) const {
    READ_AND_CHECK(arg.radio_access_network);
    READ_AND_CHECK(arg.bands_length);
    // If bands_length exceeds the maximum, return failure.
    if (arg.bands_length > MAX_BANDS) {
        return Result::FAILURE;
    }
    for(uint32_t i=0; i < arg.bands_length; i++) {
        switch(arg.radio_access_network) {
            case GERAN:
                READ_AND_CHECK(arg.bands.geran_bands[i]);
                break;
            case UTRAN:
                READ_AND_CHECK(arg.bands.utran_bands[i]);
                break;
            case EUTRAN:
                READ_AND_CHECK(arg.bands.eutran_bands[i]);
                break;
            case NGRAN:
                READ_AND_CHECK(arg.bands.ngran_bands[i]);
                break;
            default:
                break;
        }
    }
    READ_AND_CHECK(arg.channels_length);
    // If channels_length exceeds the maximum, return failure.
    if (arg.channels_length > MAX_CHANNELS) {
        return Result::FAILURE;
    }
    for(uint32_t i=0; i < arg.channels_length; i++) {
        READ_AND_CHECK(arg.channels[i]);
    }
    return Result::SUCCESS;
}
