/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <Modules/ModuleMenu.hpp>
#include <telephony/ril.h>

ModuleMenu::ModuleMenu(std::string appName, std::string cursor, RilApiSession &rilSession):
    ConsoleApp(appName, cursor),
    rilSession(rilSession)
{ }

ModuleMenu::~ModuleMenu() {
}

void ModuleMenu::init() {
    std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListModuleSubMenu = {
        CREATE_COMMAND(ModuleMenu::getConfigModuleList, "get_config_module_list", "config"),
        CREATE_COMMAND(ModuleMenu::getModuleList, "get_module_list"),
        CREATE_COMMAND(ModuleMenu::getConfigList, "get_config_list"),
        CREATE_COMMAND(ModuleMenu::getCurrentConfig, "get_current_config"),
        CREATE_COMMAND(ModuleMenu::setCurrentConfig, "set_current_config", "config"),
        CREATE_COMMAND(ModuleMenu::setCustomConfigModules, "set_custom_config_modules"),
    };
    addCommands(commandsListModuleSubMenu);
    ConsoleApp::displayMenu();
}

void ModuleMenu::getModuleList(std::vector<std::string> userInput) {
    Status s = rilSession.getModuleList(nullptr,
            [] (RIL_Errno e, RIL_ModuleList *ml) {
                if (e == RIL_E_SUCCESS) {
                    std::cout << "Full list of modules (" << rilModuleList_getCount(ml) << "): " << std::endl;
                    for (size_t i = 0; i <  rilModuleList_getCount(ml); i++ ) {
                        std::cout << "  " << rilModuleList_getName(ml, i) << std::endl;
                    }
                } else {
                    std::cout << "Error while getting module list: " << e << std::endl;
                }
            }
    );
}
void ModuleMenu::getConfigModuleList(std::vector<std::string> userInput) {
    std::string config;
    const char *config_name = nullptr;
    if (userInput.size() >= 2 && userInput[1].size() > 0) {
        config = userInput[1];
        config_name = config.c_str();
    }
    Status s = rilSession.getModuleList(config_name,
            [config_name, config] (RIL_Errno e, RIL_ModuleList *ml) {
                if (e == RIL_E_SUCCESS) {
                    if (config_name) std::cout <<
                        "Modules for config: '" <<
                        config <<
                        "' (" <<
                        rilModuleList_getCount(ml) <<
                        ") " <<
                        std::endl;
                    for (size_t i = 0; i <  rilModuleList_getCount(ml); i++ ) {
                        std::cout << "  " << rilModuleList_getName(ml, i) << std::endl;
                    }
                } else {
                    std::cout << "Error while getting module list: " << e << std::endl;
                }
            }
    );
}
void ModuleMenu::getConfigList(std::vector<std::string> userInput) {
    Status s = rilSession.getConfigList(
            [] (RIL_Errno e, std::vector<std::string> configList) {
                if (e == RIL_E_SUCCESS) {
                    for (auto cfg: configList) {
                        std::cout << "  " << cfg << std::endl;
                    }
                } else {
                    std::cout << "Error while getting the config list: " << e << std::endl;
                }
            }
    );
}

void ModuleMenu::getCurrentConfig(std::vector<std::string> userInput) {
    rilSession.getCurrentConfig(
            [] (RIL_Errno e, std::string currentConfig) {
                if (e == RIL_E_SUCCESS) {
                    std::cout << "Current config: '" << currentConfig << "'" << std::endl;
                }
            }
    );
}

void ModuleMenu::setCurrentConfig(std::vector<std::string> userInput) {
    std::string currentConfig;
    if (userInput.size() < 2 || userInput[1].size() < 1) {
        std::cout << "Please provide the name of the configuration to set" << std::endl;
        return;
    }
    currentConfig = userInput[1];
    rilSession.setCurrentConfig( currentConfig,
            [currentConfig] (RIL_Errno e) {
                if (e == RIL_E_SUCCESS) {
                    std::cout << "Success" << std::endl;
                } else {
                    std::cout << "Error setting currentConfig to " << currentConfig << ": " << e << std::endl;
                }
            }
    );
}

void ModuleMenu::setCustomConfigModules(std::vector<std::string> userInput) {
    std::vector<std::string> moduleList;
    std::cout << "Please type the list of modules to use for the Custom configuration" << std::endl;
    std::cout << "Enter one per line. An empty line means the end of the list." << std::endl;
    std::cout << "To cancel enter the word \"Cancel\" (without the double quotes)" << std::endl;
    std::string line;

    do {
        std::getline(std::cin, line);
        if (line.size() > 0 && line.compare("Cancel") != 0) {
            moduleList.push_back(line);
        }
    } while (line.size() != 0 && line.compare("Cancel") != 0);
    if (line.compare("Cancel") != 0 && moduleList.size() > 0) {
        rilSession.setCustomConfigModules( moduleList,
                [] (RIL_Errno e) {
                    if ( e == RIL_E_SUCCESS) {
                        std::cout << "Success" << std::endl;
                    } else {
                        std::cout << "Error setting list of modules for \"Custom\" config: " << e << std::endl;
                    }
                }
        );
    }
}

