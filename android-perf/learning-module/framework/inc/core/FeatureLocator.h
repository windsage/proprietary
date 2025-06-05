/******************************************************************************
  *   @file    FeatureLocator.h
  *   @brief   FeatureLocator Class defined here
  *
  *   DESCRIPTION
  *      FeatureLocator Class provides the implemetatation for to
  *      support accessing other features from different modules
  *
  *
  *---------------------------------------------------------------------------
  *    Copyright (c) 2020 Qualcomm Technologies, Inc.
  *    All Rights Reserved.
  *    Confidential and Proprietary - Qualcomm Technologies, Inc.
  *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef FEATURELOCATOR_H
#define FEATURELOCATOR_H

#include <string>
#include <vector>

/*
* This file Shouldn't be modified. Otherwise no-ship
* code has be recompiled because of the change.
*/

struct featureData {
    int data;
};

//fwd declaration
class Feature;

class FeatureLocator {
private:
    // ctor, copy ctor, assignment overloading
    FeatureLocator() {}
    FeatureLocator(FeatureLocator const& oh);
    FeatureLocator& operator=(FeatureLocator const& oh);

    std::vector<Feature*> mFeatureList;
    Feature *lookup(std::string& featureName);
    static FeatureLocator mInstance;

public:
    static FeatureLocator &getInstance() {
        return mInstance;
    }

    int getData(std::string& featureName, std::string& appName, featureData &data, int type, int duration);
    int getData(std::string& featureName, std::string& appName, featureData &data, int type);
    int getData(std::string& featureName, int hintID, char** ptr);
    int setData(std::string& featureName, std::string& appName, std::vector<int> data);
    int setData(std::string& featureName , int setDataType , std::vector<int> &data);
    void registerFeatureList(std::vector<Feature*>);
    bool isLocatable(std::string& featureName) { return lookup(featureName) ? true: false; };
};

#endif /* FEATURELOCATOR_H */
