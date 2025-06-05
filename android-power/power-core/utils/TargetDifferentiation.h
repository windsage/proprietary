/******************************************************************************
  @file    TargetDifferentiation.h
  @brief   Interface for target differentiation for feature obfuscation

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include<string>
#ifndef TARGET_DIFFERENTIATION_H
#define TARGET_DIFFERENTIATION_H

class TargetDifferentiation {
  public:
    enum features {
        VideoPowerOptFeature = 1
    };

    static std::string encode(const std::string in);
    static std::string decode(const std::string in);
    static bool IsFeatureEnabled(std::string feature);
    static int GetSocName(char* name);

};

#endif // TARGET_DIFFERENTIATION_H