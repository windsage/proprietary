/*
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/
#pragma once
#ifndef MODULE_CONFIG_MODULE_LIST_INCLUDED
#define MODULE_CONFIG_MODULE_LIST_INCLUDED

#include <optional>
#include <vector>
#include <string>
namespace ModuleConfig {
class ModuleList {
    private:
        std::optional<std::string> config;
        std::vector<std::string> modules;
    public:
        using container = decltype(modules);
        using iterator = container::iterator;
        using const_iterator = container::const_iterator;
        using reference = container::reference;
        using const_reference = container::const_reference;

        ModuleList():
            config(),
            modules() {}
        ModuleList(const std::string &config):
            config(config),
            modules() {}
        ModuleList(const std::string &config, const std::vector<std::string> &modules):
            config(config),
            modules(modules) {}
        ModuleList(const std::vector<std::string> &modules):
            config(),
            modules(modules) {}
        bool isConfigValid() {
            bool ret = false;

            if (config) {
                ret = (bool)config;
            }

            return ret;
        }
        const std::vector<std::string> &getModules() const {
            return modules;
        }

        void setModules(const std::vector<std::string> &modules) {
            this->modules = modules;
        }

        std::string getConfig() const {
            if (config) {
                return *config;
            }
            return "";
        }
        void setConfig(std::optional<std::string> config) {
            this->config = config;
        }
        std::optional<std::string> getOptConfig() {
            return config;
        }

        template< class... Args >
        reference emplace_back( Args&&... args ) {
            return modules.emplace_back(std::forward<Args>(args)...);
        }

        iterator begin() noexcept {
            auto it = modules.begin();
            return it;
        }

        const_iterator begin() const {
            auto it = modules.cbegin();
            return it;
        }

        const_iterator cbegin() const noexcept {
            return modules.cbegin();
        }
        iterator end() noexcept {
            return modules.end();
        }
        const_iterator end() const noexcept {
            return modules.cend();
        }
        const_iterator cend() const noexcept {
            return modules.cend();
        }
        reference operator[]( std::size_t pos ) {
            return modules[pos];
        }
        const_reference operator[]( std::size_t pos) const {
            return (modules)[pos];
        }
        size_t size() const noexcept {
            return modules.size();
        }
};

}
#endif
