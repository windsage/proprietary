/******************************************************************************
  @file    SecureOperations.h
  @brief   Implementation of secure int operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __SECUREOPERATIONS__H_
#define __SECUREOPERATIONS__H_

#include <stdint.h>

//secure int operations
class SecInt {

  public:
    static uint32_t Add(uint32_t lhs, uint32_t rhs);
    static int32_t Add(int32_t lhs, int32_t rhs);
    static uint32_t Subtract(uint32_t lhs, uint32_t rhs);
    static int32_t Subtract(int32_t lhs, int32_t rhs);
    static uint32_t Multiply(uint32_t lhs, uint32_t rhs);
    static int32_t Multiply(int32_t lhs, int32_t rhs);
    static int32_t Divide(int32_t lhs, int32_t rhs);

};

#endif
