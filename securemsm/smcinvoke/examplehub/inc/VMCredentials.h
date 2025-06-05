/***********************************************************************
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#pragma once

#include "object.h"

/** Return an object implementing the ICredentials interface, populated
 * with the passed VMUUID.
 * */
int32_t VMCredentials_open(uint8_t const *vmuuid, size_t vmuuidLen, Object *objOut);
