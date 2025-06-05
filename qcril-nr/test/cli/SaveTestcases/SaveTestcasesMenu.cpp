/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <chrono>

#include "SaveTestcasesMenu.hpp"

SaveTestcasesMenu::SaveTestcasesMenu(std::string appName, std::string cursor,
                                     RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
    // Set testcase output dir to default path
    if (rilSession.getTestcaseOutputDir().empty()) {
        Status s = rilSession.setTestcaseOutputDir(TESTCASE_OUT_DEFAULT_DIR);
        if (s == Status::FAILURE) {
            std::cout << "Failed to set testcase output dir to default value." << std::endl;
        }
    }
}


SaveTestcasesMenu::~SaveTestcasesMenu() {
}


void SaveTestcasesMenu::init() {
    std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListSaveTestcasesSubMenu = {
        CREATE_COMMAND(SaveTestcasesMenu::setSaveTestcases, "Enable/Disable Testcase Generation"),
        CREATE_COMMAND(SaveTestcasesMenu::setTestcaseDir, "Change Output Directory"),
    };
    addCommands(commandsListSaveTestcasesSubMenu);
    ConsoleApp::displayMenu();
}


void SaveTestcasesMenu::setSaveTestcases(std::vector<std::string> userInput) {
  std::string saveTestcasesEnabled;
  Status s = Status::FAILURE;
    do {
      std::cin.clear();
      std::cout << "Testcase Generation is currently "
                << ((rilSession.getSaveTestcases()) ? "Enabled" : "Disabled") << ".\n"
                << "Do you want to enable or disable testcase generation?\n"
                << "1. Enable\n"
                << "2. Disable\n"
                << "3. Exit" << std::endl;
      std::cin >> saveTestcasesEnabled;
    } while (std::cin.fail());

    if (saveTestcasesEnabled[0] == '1') {
        s = rilSession.setSaveTestcases(true);
        if (s == Status::FAILURE) {
            std::cout << "Failed to enable testcase generation." << std::endl;
        } else {
            std::cout << "Testcase generation is now enabled." << std::endl;
        }
    }
    else if (saveTestcasesEnabled[0] == '2') {
          s = rilSession.setSaveTestcases(false);
          if (s == Status::FAILURE) {
              std::cout << "Failed to disable testcase generation." << std::endl;
          } else {
            std:: cout << "Testcase generation is now disabled." << std::endl;
          }
    }
    else if  (saveTestcasesEnabled[0] == '3') {
        std::cout << "Testcase generation is still "
                  << ((rilSession.getSaveTestcases()) ? "Enabled" : "Disabled") << "."
                  << std::endl;
        return;
    }
    else {
          std::cout << "Invalid option: " << saveTestcasesEnabled << " Exiting menu." << std::endl;
    }

    return;
}


void SaveTestcasesMenu::setTestcaseDir(std::vector<std::string> userInputs) {
    std::string testcaseDirInput;
    std::string choice;
    Status s = Status::FAILURE;
    std::string currentDir = rilSession.getTestcaseOutputDir();
    if (!currentDir.empty()) {
        std::cout << "Current testcase output directory: " << currentDir << "\n";
    }
    else {
      std::cout << "No testcase output directory has been set.\n";
    }
    do {
      std::cin.clear();
      std::cout << "1. Enter new testcase directory.\n"
                << "2. Exit and keep current directory." << std::endl;
      std::cin >> choice;
    } while (std::cin.fail());

    if (choice[0] == '1') {
        do {
            std::cout << "Enter a new testcase output directory: ";
            std::cin >> testcaseDirInput;
        } while (std::cin.fail());
        s = rilSession.setTestcaseOutputDir(testcaseDirInput.c_str());
        if (s == Status::FAILURE) {
            std::cout << "Failed to set output directory." << std::endl;
        }
        else {
            currentDir = rilSession.getTestcaseOutputDir();
            std::cout << "New testcase output directory is " << currentDir << std::endl;
        }
    }
    return;
}
