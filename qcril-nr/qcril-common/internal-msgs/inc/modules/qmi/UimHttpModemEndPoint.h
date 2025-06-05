/******************************************************************************
#  @file    UimHttpModemEndPoint.h
#  @brief   Base class header file for QMI UIM HTTP module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include <vector>

#include "modules/qmi/ModemEndPoint.h"
#include "framework/Log.h"
#include "framework/legacy.h"

class UimHttpModemEndPoint : public ModemEndPoint
{
 public:
  UimHttpModemEndPoint();

  ~UimHttpModemEndPoint()
  {
      Log::getInstance().d("[UimHttpModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, qcril_instance_id_e_type id,
        GenericCallback<string>* cb);
};
