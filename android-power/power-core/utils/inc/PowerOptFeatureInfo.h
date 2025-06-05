/******************************************************************************
 *   @file    PowerOptFeatureInfo.h
 *   @brief   Feature Info
 *
 *   DESCRIPTION
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef POWEROPTFEATUREINFO_H
#define POWEROPTFEATUREINFO_H

#include <vector>
#include <unordered_map>

#define LOG_XML_PARSER "XMLParserLog"
#define NAME "Name"
#define ENABLED "Enable"
#define LIB_NAME "Libname"
#define CONFIGS "Configs"
#define PROFILES "Profiles"
#define IGNORED_APPS "IgnoredApps"
#define APPLIED_APPS "AppliedApps"
#define TRIGGER "Trigger"

struct PowerOptFeatureInfo {
    std::string name; /**< Name of the feature */
    std::string libName;
    std::vector<std::vector<int> > profiles;  /**< List of profiles, obsolete */
    std::unordered_map<std::string, std::vector<std::vector<int> > > profilesMap;  /**< Map of profiles */
    std::unordered_map<std::string, std::string> configsList; /**< Map of key value pairs passed as config to the Feature */
    std::vector<std::string> ignoredAppsList; /**< List of ignored apps */
    std::vector<std::string> appliedAppsList; /**< List of applied apps */
    std::vector<int> hintList;

    std::string toString() {
        std::string retvalue;
        retvalue += "FeatureInfo: featureName = "+ name;
        retvalue += " Libname -"+ libName;
        return retvalue;
    }
};
#endif /* POWEROPTFEATUREINFO_H */