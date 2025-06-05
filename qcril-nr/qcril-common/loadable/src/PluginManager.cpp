/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <qtiril-loadable/PluginManager.h>
#define TAG "RIL-PluginManager"
#include <Logger.h>
#include <memory>

namespace QtiRil {
namespace Loadable {

void PluginRegistry::add(std::string name, std::shared_ptr<LoadableModule> module) {
    modules.insert({0,{name, false, 0, module}});
}

void PluginRegistry::add(std::string name,
        std::shared_ptr<LoadableModule> module,
        bool mandatory,
        size_t prio) {
    modules.insert({prio, {name, mandatory, prio, module}});
}

PluginManager::PluginManager(const PluginRegistry &registry): registry(registry) { }

size_t PluginRegistry::size() {
    return modules.size();
}
void PluginRegistry::foreach(PluginRegistry::foreachcb cb) {
    for (auto entry: modules) {
        std::string name;
        bool mandatory;
        size_t priority;
        std::shared_ptr<LoadableModule> module;
        std::tie(name,mandatory,priority,module) = entry.second;
        cb(name, module, mandatory,priority);
    }
}

void PluginManager::load() {
    QCRIL_HAL_LOG_VERBOSE("Loading plugins...");
    registry.foreach([] (std::string name,
                std::shared_ptr<LoadableModule> module,
                bool mandatory,
                size_t /* prio */) {
            if (module && !module->isLoaded()) {
                const char *cname = name.c_str();
                QCRIL_HAL_LOG_VERBOSE("Loading %s module %s",
                        mandatory ? "mandatory" : "optional",
                        cname);

                auto hndl = module->load();
                if (hndl == nullptr && mandatory) {
                    QCRIL_HAL_LOG_FATAL("Unable to load mandatory module %s", cname);
                    abort();
                }
                QCRIL_HAL_LOG_INFO("%s module %s %sloaded",
                        mandatory ? "Mandatory" : "Optional",
                        cname,
                        (hndl == nullptr ) ? "not " : "" );

            }
    });
}

int PluginManager::init() {
    registry.foreach([] (std::string /* name */,
                std::shared_ptr<LoadableModule> module,
                bool /*mandatory*/,
                size_t /*prio*/) {
                if (module && module->isLoaded()) {
                    module->init();
                }
            }
    );

    return 0;
}

int PluginManager::deinit() {
    registry.foreach([] (std::string /*name*/,
                std::shared_ptr<LoadableModule> module,
                bool /*mandatory*/,
                size_t /*prio*/) {
                if (module && module->isLoaded()) {
                    module->deinit();
                }
            }
    );
    return 0;
}

void PluginManager::unload() {
    registry.foreach([] (std::string /*name*/,
                std::shared_ptr<LoadableModule> module,
                bool /*mandatory*/,
                size_t /*prio*/) {
                if (module && module->isLoaded()) {
                    module->unload();
                }
            }
   );
}


} // Loadable
} // QtiRil
