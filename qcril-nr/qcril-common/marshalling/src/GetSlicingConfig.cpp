/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <marshal/GetSlicingConfig.h>

template <>
Marshal::Result Marshal::write<rilDnn>(const rilDnn &arg) {
    WRITE_AND_CHECK(arg.dnn);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<rilDnn>(rilDnn &arg) const {
    READ_AND_CHECK(arg.dnn);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<rilDnn>(rilDnn &arg) {
    release(arg.dnn);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<rilRouteSelectorDescriptor>(const rilRouteSelectorDescriptor &arg) {
    WRITE_AND_CHECK(arg.precedence);
    WRITE_AND_CHECK(arg.ipType);
    WRITE_AND_CHECK(arg.sscMode);
    WRITE_AND_CHECK(arg.sliceInfos, arg.sliceInfoSize);
    WRITE_AND_CHECK(arg.dnnList, arg.dnnSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<rilRouteSelectorDescriptor>(rilRouteSelectorDescriptor &arg) const {
    READ_AND_CHECK(arg.precedence);
    READ_AND_CHECK(arg.ipType);
    READ_AND_CHECK(arg.sscMode);
    READ_AND_CHECK(arg.sliceInfos, arg.sliceInfoSize);
    READ_AND_CHECK(arg.dnnList, arg.dnnSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<rilRouteSelectorDescriptor>(rilRouteSelectorDescriptor &arg) {
    release(arg.precedence);
    release(arg.ipType);
    release(arg.sscMode);
    release(arg.sliceInfos, arg.sliceInfoSize);
    release(arg.dnnList, arg.dnnSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<rilUrspRule>(const rilUrspRule &arg) {
    WRITE_AND_CHECK(arg.precedence);
    WRITE_AND_CHECK(arg.tdList, arg.tdSize);
    WRITE_AND_CHECK(arg.routeSelectors, arg.routeSelectorSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<rilUrspRule>(rilUrspRule &arg) const {
    READ_AND_CHECK(arg.precedence);
    READ_AND_CHECK(arg.tdList, arg.tdSize);
    READ_AND_CHECK(arg.routeSelectors, arg.routeSelectorSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<rilUrspRule>(rilUrspRule &arg) {
    release(arg.precedence);
    release(arg.tdList, arg.tdSize);
    release(arg.routeSelectors, arg.routeSelectorSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<rilSlicingConfig>(const rilSlicingConfig &arg) {
    WRITE_AND_CHECK(arg.ursprules, arg.UsrpRuleSize);
    WRITE_AND_CHECK(arg.sliceInfos, arg.sliceInfoSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<rilSlicingConfig>(rilSlicingConfig &arg) const {
    READ_AND_CHECK(arg.ursprules, arg.UsrpRuleSize);
    READ_AND_CHECK(arg.sliceInfos, arg.sliceInfoSize);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<rilSlicingConfig>(rilSlicingConfig &arg) {
    release(arg.ursprules, arg.UsrpRuleSize);
    release(arg.sliceInfos, arg.sliceInfoSize);
    return Result::SUCCESS;
}