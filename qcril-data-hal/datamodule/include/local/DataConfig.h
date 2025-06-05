/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATACONFIG
#define DATACONFIG

#include "framework/Log.h"
#ifdef RIL_FOR_MDM_LE

#include "qcril_config.h"
#else
  #include <cutils/properties.h>
#endif

#define TAG "DATACONFIG"

#ifdef __cplusplus
#include <string>
#include <list>

using namespace std;

namespace rildata {

#ifdef RIL_FOR_MDM_LE
  #include <unordered_map>
  void propertySetMap
  (
     string s1,
     property_id p1
  );
#endif

string qcril_get_property_value
(
   string name,
   string defaultValue
);
string readProperty
(
   string id,
   string propValue
);

struct SimInfo
{
  string mcc;
  string mnc;
};

const std::list<SimInfo> simInfoList = {
  {"405","840"},{"405","854"},{"405","855"},{"405","856"},{"405","857"},{"405","858"},{"405","859"},{"405","860"},{"405","861"},
  {"405","862"},{"405","863"},{"405","864"},{"405","865"},{"405","866"},{"405","867"},{"405","868"},{"405","869"},{"405","870"},
  {"405","871"},{"405","872"},{"405","873"},{"405","874"}
};
bool isProfileUpdateCarrierSet(string mcc, string mnc);

} /* namespace rildata */
#else

char* readProperty
(
  char id[],
  char* propValue,
  char defaultValue[]
);

#endif

#endif