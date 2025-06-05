/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#pragma once
#ifndef MODULE_MENU_INCLUDED
#define MODULE_MENU_INCLUDED

#include <TelSdkConsoleApp.hpp>
#include <vector>
class ModuleMenu: public ConsoleApp {
    public:
        ModuleMenu(std::string appName, std::string cursor, RilApiSession &rilSession);
        ~ModuleMenu();
        void init();
    private:
        RilApiSession& rilSession;
        void getModuleList(std::vector<std::string> userInput);
        void getConfigModuleList(std::vector<std::string> userInput);
        void getConfigList(std::vector<std::string> userInput);
        void getCurrentConfig(std::vector<std::string> userInput);
        void setCurrentConfig(std::vector<std::string> userInput);
        void setCustomConfigModules(std::vector<std::string> userInput);
};

#endif // MODULE_MENU_INCLUDED
