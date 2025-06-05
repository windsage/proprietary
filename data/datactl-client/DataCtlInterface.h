/*===========================================================================

  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __DATACTLINTERFACE_H__
#define __DATACTLINTERFACE_H__

#include "CommonDataCtlDefs.h"
#include <memory>
#include <string>

namespace datactl {

class DataCtlInterface {
public:
    DataCtlInterface(){}

    virtual ~DataCtlInterface() {}

    virtual void initialize(SubsId subs, logFnPtr logPtr) = 0;

    virtual void controlIWlan(bool enable) = 0;

    virtual void registerDataPPDataInds(dataPPDataCapChangeCb capCb, dataPPDataRecommendationCb recoCb) = 0;

    virtual bool getDataPPDataCapability() = 0;

    virtual DataSubsRecommendation_t getDataPPDataSubsRecommendation() = 0;

    virtual bool updateDataPPDataUIOption(bool option) = 0;

};

} //namespace

#endif /*__DATACTLINTERFACE_H__*/
