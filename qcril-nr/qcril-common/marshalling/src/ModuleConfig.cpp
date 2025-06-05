/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <marshal/ModuleConfig.hpp>
#include <Logger.h>
template <>
Marshal::Result Marshal::write<ModuleConfig::ModuleList>(const ModuleConfig::ModuleList& arg) {
#ifndef TAG
    const char *TAG = "Marshal::write<ModuleList>";
#endif
    QCRIL_HAL_LOG_DEBUG("Entry Size: %zu. Available: %zu", dataSize(), dataAvail());
    std::optional<std::string> config = arg.getConfig();
    std::vector<std::string> modules = arg.getModules();
    WRITE_AND_CHECK(config);
    QCRIL_HAL_LOG_DEBUG("After config: %zu. Available: %zu", dataSize(), dataAvail());
    WRITE_AND_CHECK(modules);
    QCRIL_HAL_LOG_DEBUG("Exit Size: %zu. Available: %zu", dataSize(), dataAvail());
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<ModuleConfig::ModuleList>(ModuleConfig::ModuleList& arg) const {
#ifndef TAG
    const char *TAG = "Marshal::read<ModuleList>";
#endif
    QCRIL_HAL_LOG_DEBUG("Entry Size: %zu. Available: %zu", dataSize(), dataAvail());
    std::optional<std::string> config;
    READ_AND_CHECK(config);
    arg.setConfig(config);
    QCRIL_HAL_LOG_DEBUG("After config: %zu. Available: %zu", dataSize(), dataAvail());

    std::vector<std::string> modules;
    READ_AND_CHECK(modules);
    QCRIL_HAL_LOG_DEBUG("After modules: %zu. Available: %zu", dataSize(), dataAvail());
    arg.setModules(modules);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_ModuleList>(const RIL_ModuleList& arg) {
    WRITE_AND_CHECK(arg.moduleList);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ModuleList>(RIL_ModuleList& arg) const {
    READ_AND_CHECK(arg.moduleList);
    return Result::SUCCESS;
}

