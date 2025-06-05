#ifndef COMDEF_H
#define COMDEF_H

/*===========================================================================
Copyright (c) 2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include <stdint.h>

typedef  uint64_t   uint64;
typedef  int64_t    int64;
typedef  uint32_t   uint32;
typedef  uint16_t   uint16;
typedef  uint8_t    uint8;
typedef  int32_t    int32;
typedef  int16_t    int16;
typedef  int8_t     int8;
typedef  signed char byte;

typedef  uint8_t    boolean;

#define word uint32_t

#ifndef NULL
  #define NULL  0
#endif

#if defined __GNUC__
#define PACK(x)       x __attribute__((__packed__))
#endif

#endif // #ifndef COMDEF_H
