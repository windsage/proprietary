/*===========================================================================

  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __COMMONDATACTLDEFS_H__
#define __COMMONDATACTLDEFS_H__
#include <functional>

namespace datactl {

enum class SubsId : uint32_t {
  PRIMARY,
  SECONDARY,
  TERTIARY
};


enum class RecommendationSubs : uint32_t {
  DDS   = 1,
  NDDS  = 2
};

enum class RecommendationAction : uint32_t {
  DATA_ALLOWED      = 1,
  DATA_NOT_ALLOWED  = 2
};

struct DataSubsRecommendation_t {
  RecommendationAction action;
  RecommendationSubs subs;
};

typedef void (*logFnPtr)(std::string);
typedef void (*dataPPDataCapChangeCb)(bool);
typedef void (*dataPPDataRecommendationCb)(DataSubsRecommendation_t);
extern "C"
{
  void* datactlInit();
}
}//namespace

#endif /*__COMMONDATACTLDEFS_H__*/
