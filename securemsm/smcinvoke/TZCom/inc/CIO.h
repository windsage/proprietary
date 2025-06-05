/********************************************************************
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef __CIO_H
#define __CIO_H

#include <stdint.h>
#include <stddef.h>

#include "object.h"

int32_t CIO_open(const void* cred_buffer,
                  size_t cred_buffer_len,
                  Object* objOut);

#endif // __CIO_H
