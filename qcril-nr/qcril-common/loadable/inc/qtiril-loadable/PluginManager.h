/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#ifndef QTIRIL_LOADABLE_PLUGIN_REGISTRY_INCLUDED
#define QTIRIL_LOADABLE_PLUGIN_REGISTRY_INCLUDED

#include <qtiril-loadable/QtiRilLoadable.h>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <tuple>

namespace QtiRil {
namespace Loadable {

class PluginRegistry {
    private:
        using EntryType = std::tuple<std::string, bool, size_t, std::shared_ptr<LoadableModule>>;
        std::multimap<size_t, EntryType> modules;
    public:
        void add(std::string name,
                std::shared_ptr<LoadableModule> module);
        void add(std::string name,
                std::shared_ptr<LoadableModule> module,
                bool mandatory,
                size_t prio);
        using foreachcb = std::function<void(std::string,
                std::shared_ptr<LoadableModule>,
                bool,
                size_t)>;
        void foreach(foreachcb cb);
        size_t size();
};


class PluginManager {
    private:
        PluginRegistry registry;
    public:
        PluginManager(const PluginRegistry &registry);
        void load();
        int init();
        int deinit();
        void unload();
        static const PluginRegistry &loadPluginRegistry();
};

}
}

#endif
