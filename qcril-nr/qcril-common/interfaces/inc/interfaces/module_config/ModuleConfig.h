/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <optional>
#include <string>
#include <vector>
#include <framework/SolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/module_config/ModuleList.h>

#ifndef MODULE_MANAGER_GET_MODULE_LIST_MESSAGE_H
#define MODULE_MANAGER_GET_MODULE_LIST_MESSAGE_H

namespace ModuleConfig {

class GetConfigListMessage: public SolicitedMessage<std::vector<std::string>>,
    public add_message_id<GetConfigListMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.module_manager.get_configs";

        GetConfigListMessage():
            SolicitedMessage(get_class_message_id())
        {}

        std::string dump() {
            return "GetConfigListMessage{}";
        }
};

class GetModuleListMessage: public SolicitedMessage<ModuleList>,
    public add_message_id<GetModuleListMessage>
{
    public:
    private:
        std::optional<std::string> configName;
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.module_manager.get_list";
        GetModuleListMessage():
            SolicitedMessage(get_class_message_id())
        { }

        GetModuleListMessage(std::optional<std::string> optConfig):
            SolicitedMessage(get_class_message_id()),
            configName(optConfig)
        { }
        GetModuleListMessage(std::string config):
            SolicitedMessage(get_class_message_id()),
            configName(config)
        {}

        bool hasConfigName() {
            return configName.has_value();
        }
        std::string getConfigName() {
            if (configName) {
                return *configName;
            } else {
                return "";
            }
        }
        std::string dump() {
            return "GetModuleListMessage{}";
        }

};

class GetCurrentConfigMessage: public SolicitedMessage<std::string>,
    public add_message_id<GetCurrentConfigMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.module_manager.get_current_config";
        GetCurrentConfigMessage():
            SolicitedMessage(get_class_message_id())
        {}
        std::string dump() {
            return "GetCurrentConfigMessage{}";
        }
};

class SetCurrentConfigMessage: public SolicitedMessage<void>,
    public add_message_id<SetCurrentConfigMessage>
{
    private:
        std::string config;
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.module_manager.set_current_config";
        SetCurrentConfigMessage(std::string config):
            SolicitedMessage(get_class_message_id()),
            config(config)
        {}

        std::string dump() {
            return "SetCurrentConfigMessage{}";
        }

        std::string getConfig() {
            return config;
        }
};

class SetCustomConfigModules: public SolicitedMessage<void>,
    public add_message_id<SetCustomConfigModules>
{
    private:
        ModuleList modules;
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.module_manager.set_custom_conf_modules";

        using iterator = ModuleList::iterator;
        using const_iterator = ModuleList::const_iterator;
        using reference = ModuleList::reference;
        using const_reference = ModuleList::const_reference;

        SetCustomConfigModules(std::vector<std::string> moduleList):
            SolicitedMessage(get_class_message_id()),
            modules(moduleList)
        {}
        iterator begin() noexcept {
            return modules.begin();
        }
        const_iterator begin() const {
            return modules.begin();
        }
        const_iterator cbegin() const noexcept {
            return modules.cbegin();
        }
        iterator end() noexcept {
            return modules.end();
        }
        const_iterator end() const noexcept {
            return modules.end();
        }
        const_iterator cend() const noexcept {
            return modules.cend();
        }
        reference operator[]( std::size_t pos ) {
            return modules[pos];
        }
        const_reference operator[]( std::size_t pos) const {
            return modules[pos];
        }
        std::string dump() {
            return "SetCustomConfigModules";
        }
};

}
#endif // MODULE_MANAGER_GET_MODULE_LIST_MESSAGE_H
