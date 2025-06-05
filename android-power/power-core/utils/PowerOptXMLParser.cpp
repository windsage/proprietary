/******************************************************************************
  @file    PowerOptXMLParser.cpp
  @brief   Power XML Parser

  DESCRIPTION

  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <string>

#include "PowerOptXMLParser.h"
#include "PowerOptFeatureInfo.h"
#include "TargetDifferentiation.h"

#define PROPERTY_VALUE_MAX 256

using namespace tinyxml2;
using namespace std;

PowerOptXMLParser::PowerOptXMLParser(){
    DEBUGV(LOG_XML_PARSER, "Constructor %s", "XML PARSER");
}

PowerOptXMLParser::PowerOptXMLParser(string xmlFileName) :
       mXMLRoot(nullptr) {
    int ret_code = mXMLDoc.LoadFile(xmlFileName.c_str());
    if (ret_code != XML_SUCCESS) {
        /* removing error codes as they are replaced with different api
         * and project is used across new and old android apis. */
        DEBUGE(LOG_XML_PARSER,
                "Unable to load XML file %s : error code: %d",
                xmlFileName.c_str(), ret_code);
    } else {
        mXMLRoot = mXMLDoc.RootElement();
        if (mXMLRoot == NULL) {
            DEBUGE(LOG_XML_PARSER, "Unable to find XMLRoot in %s", xmlFileName.c_str());
        }
    }
}

/* This enables test to specify XML in a string and get it parsed into FeatureInfo */
PowerOptXMLParser::PowerOptXMLParser(const char* xmlString) :
       mXMLRoot(nullptr) {
    int ret_code = mXMLDoc.Parse(xmlString);
    if (ret_code != XML_SUCCESS) {
        DEBUGE(LOG_XML_PARSER,
                "Unable to load XML string %s : error code: %d",
                xmlString, ret_code);
    } else {
        mXMLRoot = mXMLDoc.RootElement();
        if (mXMLRoot == NULL) {
            DEBUGE(LOG_XML_PARSER, "Unable to find XMLRoot in XML String %s", xmlString);
        }
    }
}

PowerOptXMLParser::~PowerOptXMLParser() {
    DEBUGV(LOG_XML_PARSER, "Destructor %s", "XML PARSER");
}

string PowerOptXMLParser::getElementText(XMLNode* elem) {
    string elemText;
    if(elem != NULL) {
        XMLNode* child = elem->FirstChild();
        if(child != NULL) {
            XMLNode* childText = child->ToText();
            if(childText != NULL) {
                elemText = childText->Value();
            }
        }
    }
    DEBUGV(LOG_XML_PARSER, "inside element text %s", elemText.c_str());
    return elemText;
}

string PowerOptXMLParser::getFirstChildText(XMLNode* elem, string name) {
    string childText;
    if(elem != NULL) {
        childText = getElementText(elem->FirstChildElement(name.c_str()));
    }
    DEBUGV(LOG_XML_PARSER, "inside getFirstChild text %s", childText.c_str());
    return childText;
}

PowerOptFeatureInfo PowerOptXMLParser::getXML(){
    char socName[PROPERTY_VALUE_MAX] = {'\0'};
    int ret = TargetDifferentiation::GetSocName(socName);
    if(ret > 0){
        DEBUGV(LOG_XML_PARSER, "Soc name: %s, ret: %d", socName, ret);
    }

    if(mXMLRoot != NULL) {
        /* XML:
        *        <Name>FeatureName</Name>
        */
        currParsedFeatureInfo.name = getFirstChildText(mXMLRoot,NAME);
        /* XML:
        *        <Libname>libgameoptfeature.so</Libname>
        */
        currParsedFeatureInfo.libName = getFirstChildText(mXMLRoot,LIB_NAME);;

        /* XML:
         * <Profiles>
         *     Please keep profile with target at the top.
         *     <Profile target="">
         *         <DEFAULT>resource_hex, val_hex, resource_hex, val_hex</DEFAULT>
         *         <SKEW1>resource_hex, val_hex, resource_hex, val_hex</SKEW1>
         *     </Profile>
         *     <Profile>
         *         <DEFAULT>resource_hex, val_hex, resource_hex, val_hex</DEFAULT>
         *         <SKEW1>resource_hex, val_hex, resource_hex, val_hex</SKEW1>
         *     </Profile>
         * </Profiles>
         */

        XMLElement* profiles = mXMLRoot->FirstChildElement(PROFILES);
        if(profiles != NULL) {
            for(XMLElement* currProfile = profiles->FirstChildElement("Profile"); currProfile != NULL; currProfile = currProfile->NextSiblingElement("Profile")) {
                bool valid_target = false;
                tinyxml2::XMLElement *e = currProfile->ToElement();
                if (e) {
                    const char *xmltarget = e->Attribute("target");
                    if(xmltarget && strstr(xmltarget, socName) != NULL){
                        valid_target = true;
                    }
                    if(valid_target || !xmltarget){
                        DEBUGV(LOG_XML_PARSER, "Parse profile, valid_target: %d", valid_target);
                        for(XMLNode* skew = currProfile->FirstChildElement(); skew != NULL; skew = skew->NextSiblingElement()) {
                            string currProfileKey = skew->Value();
                            string currProfileValue = getElementText(skew);
                            std::vector<int> profile;
                            ConvertToIntArray(currProfileValue.c_str(), profile);
                            currParsedFeatureInfo.profilesMap[currProfileKey].push_back(profile);
                            DEBUGV(LOG_XML_PARSER, "Profile Key %s",currProfileKey.c_str());
                        }
                        break;
                    }
                }
            }
        }

        /* XML:
        *  <Configs>
        *     <Enable>1</Enable>
        *     <MIN_DDR_FREQ>1144</MIN_DDR_FREQ>
        *     <MAX_MIN_DDR_FREQ>2086</MAX_MIN_DDR_FREQ>
        *  </Configs>
        */

        for(XMLElement* currConfigs = mXMLRoot->FirstChildElement(CONFIGS); currConfigs != NULL; currConfigs = currConfigs->NextSiblingElement(CONFIGS)) {
            bool valid_target = false;
            tinyxml2::XMLElement *e = currConfigs->ToElement();
            if (e) {
                const char *xmltarget = e->Attribute("target");
                if(xmltarget && strstr(xmltarget, socName) != NULL){
                    valid_target = true;
                }
                if(valid_target || !xmltarget){
                    DEBUGV(LOG_XML_PARSER, "Parse configs, valid_target: %d", valid_target);
                    for(XMLNode* currConfig = currConfigs->FirstChildElement(); currConfig != NULL; currConfig = currConfig->NextSiblingElement()) {
                        string currConfigKey = currConfig->Value();
                        string currConfigValue = getElementText(currConfig);
                        DEBUGV(LOG_XML_PARSER, "currConfig[%s] = %s",currConfigKey.c_str(),currConfigValue.c_str());
                        currParsedFeatureInfo.configsList[currConfigKey] = currConfigValue;
                    }
                    break;
                }
            }
        }
        /* XML:
        *        <IgnoredApps>
        */
        /* Read Each ignored app from the xml file */
        for(XMLElement* ignoredAppsElem = mXMLRoot->FirstChildElement(IGNORED_APPS); ignoredAppsElem != NULL; ignoredAppsElem = ignoredAppsElem->NextSiblingElement(IGNORED_APPS)) {
            bool valid_target = false;
            tinyxml2::XMLElement *e = ignoredAppsElem->ToElement();
            if (e) {
                const char *xmltarget = e->Attribute("target");
                if(xmltarget && strstr(xmltarget, socName) != NULL){
                    valid_target = true;
                }
                if(valid_target || !xmltarget){
                    DEBUGV(LOG_XML_PARSER, "Parse configs, valid_target: %d", valid_target);
                    for(XMLElement* currApp = ignoredAppsElem->FirstChildElement(); currApp != NULL; currApp = currApp->NextSiblingElement()) {
                        string appName = getElementText(currApp);
                        DEBUGV(LOG_XML_PARSER, "appName = %s", appName.c_str());
                        currParsedFeatureInfo.ignoredAppsList.push_back(appName);
                    }
                    break;
                }
            }
        }
        /* XML:
        *        <ApplieddApps>
        */
        /* Read Each applied app from the xml file */
        XMLElement* appliedAppsElem = mXMLRoot->FirstChildElement(APPLIED_APPS);
        if(appliedAppsElem != NULL) {
            /* XML:
             * <App>
             */
            for(XMLElement* currApp = appliedAppsElem->FirstChildElement(); currApp != NULL; currApp = currApp->NextSiblingElement()) {
                string appName = getElementText(currApp);
                DEBUGV(LOG_XML_PARSER, "appName = %s", appName.c_str());
                currParsedFeatureInfo.appliedAppsList.push_back(appName);
            }
        }

        /* XML:
        * <Trigger>
        *     <AsyncId>resource_hex, resource_hex</AsyncId>
        * </Trigger>
        */
        XMLElement* trigger = mXMLRoot->FirstChildElement(TRIGGER);
        if(trigger != NULL) {
            for(XMLNode* currTrigger = trigger->FirstChildElement(); currTrigger != NULL; currTrigger = currTrigger->NextSiblingElement()) {
                string hint = currTrigger->Value();
                string currHintValue = getElementText(currTrigger);
                ConvertToIntArray(currHintValue.c_str(), currParsedFeatureInfo.hintList);
            }
        }

        DEBUGV(LOG_XML_PARSER, "currFeatureInfo = %s",currParsedFeatureInfo.toString().c_str());
    }
    //else continue
    return currParsedFeatureInfo;
}

int PowerOptXMLParser::ConvertToIntArray(const char *str, vector<int> &resources) {
    int i = 0;
    char *pch = NULL;
    char *end = NULL;
    char *endPtr = NULL;

    DEBUGV(LOG_XML_PARSER, "ConvertToIntArray = %s",str);

    if (NULL == str) {
        return i;
    }

    end = (char *)str + strlen((char *)str);

    do {
        pch = strchr((char *)str, ',');
        resources.push_back(strtol(str, &endPtr, 0));
        str = pch;
        if (NULL != pch) {
            str++;
        }
    } while ((NULL != str) && (str < end));
    return 0;
}

int PowerOptXMLParser::ConvertToFloatArray(const char *str, vector<float> &resources) {
    int i = 0;
    char *pch = NULL;
    char *end = NULL;
    char *endPtr = NULL;

    DEBUGV(LOG_XML_PARSER, "ConvertToFloatArray = %s",str);
    if (NULL == str) { return i; }
    end = (char *)str + strlen((char *)str);
    do {
        pch = strchr((char *)str, ',');
        resources.push_back(strtof(str, &endPtr));
        str = pch;
        if (NULL != pch) {
            str++;
        }
    } while ((NULL != str) && (str < end));
    return 0;
}
int PowerOptXMLParser::ConvertToFloatArray(const char *str, float *resources, int res_size) {
    int i = 0;
    char *pch = NULL;
    char *end = NULL;
    char *endPtr = NULL;

    DEBUGV(LOG_XML_PARSER, "ConvertToFloatArray = %s",str);
    if (NULL == str) { return i; }
    end = (char *)str + strlen((char *)str);
    do {
        pch = strchr((char *)str, ',');
        resources[i++] = strtof(str, &endPtr);
        str = pch;
        if (NULL != pch) {
            str++;
        }
    } while ((NULL != str) && (str < end) && (i < res_size));
    return 0;
}

string PowerOptXMLParser::getFeatureLibName() {
    string currLibName;
    if(mXMLRoot) {
        /* XML:
         *        <Libname>libfeature.so</Libname>
         */
        currLibName = getFirstChildText(mXMLRoot,"Libname");
    } else {
        DEBUGE(LOG_XML_PARSER, "Fatal: No XmlRoot in XML File");
    }
    return currLibName;
}

vector<string> PowerOptXMLParser::parseFeatureConfig(string xmlFileName){
    int ret_code = featureXMLDoc.LoadFile(xmlFileName.c_str());
    vector<string> featureList;
    if (ret_code != XML_SUCCESS) {
        DEBUGE(LOG_XML_PARSER,
                "Unable to load XML file %s : error code: %d",
                xmlFileName.c_str(), ret_code);
    } else {
        featureXMLRoot = featureXMLDoc.RootElement();
        if (featureXMLRoot == NULL) {
            DEBUGE(LOG_XML_PARSER, "Unable to find XMLRoot in %s", xmlFileName.c_str());
        }
        else{
            for(XMLElement* currFeature = featureXMLRoot->FirstChildElement("Feature"); currFeature != NULL; currFeature = currFeature->NextSiblingElement("Feature")) {
                string featureName = getElementText(currFeature->FirstChildElement("Name"));
                string isEnabled = getElementText(currFeature->FirstChildElement("Enable"));
                if(stoi(isEnabled)!= 0){
                    featureList.push_back(featureName);
                }
                DEBUGV(LOG_XML_PARSER, "Current feature %s",featureName.c_str());
            }
        }
    }
    return featureList;
}
