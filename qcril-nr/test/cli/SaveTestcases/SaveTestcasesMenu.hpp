/*
* Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef SAVETESTCASESMENU_HPP
#define SAVETESTCASESMENU_HPP

#include <string>
#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

#define TESTCASE_OUT_DEFAULT_DIR "/data/vendor/qcrild/testcases/"

class SaveTestcasesMenu : public ConsoleApp {
    public:
        SaveTestcasesMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
        ~SaveTestcasesMenu();
        void init();
        void setTestcaseDir(std::vector<std::string> userInput);
        void setSaveTestcases(std::vector<std::string> userInput);

    private:
        RilApiSession& rilSession;

};


#endif // SAVETESTCASESMENU_HPP
