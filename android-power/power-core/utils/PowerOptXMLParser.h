/******************************************************************************
  @file    PowerOptXMLParser.h
  @brief   Power XML Parser header

  DESCRIPTION

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef POWEROPTXMLPARSER_H
#define POWEROPTXMLPARSER_H

#include <tinyxml2.h>
#include "PowerOptDebugLog.h"
#include "PowerOptFeatureInfo.h"

#define PERFLOCK_DURATION INT_MAX

class PowerOptXMLParser{
    public:
        PowerOptXMLParser();
        PowerOptXMLParser(std::string xmlFileName);
        PowerOptXMLParser(const char* xmlString);
        virtual ~PowerOptXMLParser();
        PowerOptFeatureInfo getXML();
        std::vector<std::string> parseFeatureConfig(std::string xmlFileName);
        std::string getFeatureLibName();
        int ConvertToFloatArray(const char *str, std::vector<float> &resources);
        int ConvertToFloatArray(const char *str, float *resources, int res_size);

    private:
        PowerOptFeatureInfo currParsedFeatureInfo;
        tinyxml2::XMLDocument mXMLDoc;
        tinyxml2::XMLElement* mXMLRoot;
        tinyxml2::XMLDocument featureXMLDoc;
        tinyxml2::XMLElement* featureXMLRoot;
        std::string getFirstChildText(tinyxml2::XMLNode* elem, std::string name);
        std::string getElementText(tinyxml2::XMLNode* elem);
        int ConvertToIntArray(const char *str, std::vector<int> &resources);

};

#endif /* POWEROPTXMLPARSER_H */