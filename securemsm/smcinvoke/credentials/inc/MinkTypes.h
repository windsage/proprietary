/***********************************************************************
 * Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#ifndef _MINK_TYPES_H_
#define _MINK_TYPES_H_

/**
@file mink_types.h
@brief Provides mink common type definitions
*/

#include <stdint.h>

typedef struct {
    uint8_t val[32];
} DistId;  // Distinguished Identifier

typedef struct {
    uint8_t val[32];
} SHA256Hash;  // SHA256 hash

#endif /* _MINK_TYPES_H_ */
