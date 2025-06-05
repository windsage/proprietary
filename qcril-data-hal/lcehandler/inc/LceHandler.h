/*===========================================================================

  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef LCEHANDLER
#define LCEHANDLER

#include "MessageCommon.h"

#define LQE_SUCCESS              0
#define LQE_FAILURE             -1
#define LQE_NOT_SUPPORTED       -2
#define LQE_INVALID_ARGUMENTS   -3

namespace rildata {

struct LqeParams_t
{
  int ul_report_enabled;
  int dl_report_enabled;
  bool criteria_list_valid;
  std::unordered_map<AccessNetwork_t, LinkCapCriteria_t> criteria_list;
};

class LceHandler {
public:
  LceHandler();
  ~LceHandler(){}
  void Init();
  void Release();
  bool toggleReporting(int enableBit);
  LinkCapCriteriaResult_t setCriteria(LinkCapCriteria_t criteria);
  void handlemodemSSR();
  void deInit();
private:
  bool lqeInited;
  LqeParams_t  lqeParams;
};

}/*namespace rildata */

#endif