/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <telephony/ril.h>
#include <telephony/RIL_ModuleList_i.hpp>
#include <interfaces/module_config/ModuleConfig.h>
#include <memory>

void rilModuleList_delete(RIL_ModuleList *ml) {
    ml->moduleList = nullptr;
    delete ml;
}
size_t rilModuleList_getCount(RIL_ModuleList *ml) {
    size_t ret = 0;

    if (ml && ml->moduleList) {
        ret = ml->moduleList->size();
    }

    return ret;
}

const char * rilModuleList_getConfigName(RIL_ModuleList * ml) {
    const char *ret = nullptr;
    if (ml && ml->moduleList && ml->moduleList->isConfigValid()) {
        ret = ml->moduleList->getConfig().c_str();
    }
    return ret;
}

const char *rilModuleList_getName(RIL_ModuleList *ml, size_t idx) {
    const char *ret = nullptr;

    if (ml && ml->moduleList && idx < ml->moduleList->size()) {
        ret = (*(ml->moduleList))[idx].c_str();
    }

    return ret;
}
