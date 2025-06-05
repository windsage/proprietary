/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#ifndef _CONFINEMENT_H_
#define _CONFINEMENT_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"
#include "ITAccessPermissions.h"
#include "object.h"

typedef struct {
  int32_t refs;
  ITAccessPermissions_rules confRules;
} Confinement;

int32_t CConfinement_new(const ITAccessPermissions_rules *userRules,
                         Object *objOut);

Confinement *ConfinementFromObject(Object obj);

#if defined(__cplusplus)
}
#endif

#endif
