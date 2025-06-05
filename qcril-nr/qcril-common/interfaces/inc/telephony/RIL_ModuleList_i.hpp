/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef RIL_MODULELIST_I_INCLUDED
#define RIL_MODULELIST_I_INCLUDED

#include <memory>
#include <interfaces/module_config/ModuleList.h>
struct RIL_ModuleList {
    std::shared_ptr<ModuleConfig::ModuleList> moduleList;
};


#endif // RIL_MODULELIST_I_INCLUDED

