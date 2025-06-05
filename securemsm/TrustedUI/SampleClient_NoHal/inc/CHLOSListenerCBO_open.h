/**
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include "object.h"
#include <stdint.h>
#include <semaphore.h>
#include "TAVMInterface.h"

/* This function creates  a new IHLOSListenerCBO object. */

int32_t CHLOSListenerCBO_open(Object *obj_out, sem_t *sem, trusted_vm_event_t* vmStatus);


