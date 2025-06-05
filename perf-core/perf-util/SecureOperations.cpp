/******************************************************************************
  @file    SecureOperations.cpp
  @brief   Implementation of secure int operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdint.h>
#include "SecureOperations.h"

uint32_t SecInt::Add(uint32_t lhs, uint32_t rhs) {
    uint32_t ret = 0;
    if (UINT32_MAX - lhs < rhs) {
        ret = UINT32_MAX;
    }
    else {
        ret = lhs + rhs;
    }
    return ret;
}

int32_t SecInt::Add(int32_t lhs, int32_t rhs) {
    int32_t ret = 0;
    if (lhs > 0 && rhs > 0 && (INT32_MAX - lhs < rhs)) {
        ret = INT32_MAX;
    }
    else if (lhs < 0 && rhs < 0 && (lhs < INT32_MIN - rhs)) {
        ret = INT32_MIN;
    }
    else {
        ret = lhs + rhs;
    }
    return ret;
}

uint32_t SecInt::Subtract(uint32_t lhs, uint32_t rhs) {
    uint32_t diff = 0;
    if (lhs < rhs) {
        diff = 0;
    }
    else {
        diff = lhs - rhs;
    }
    return diff;
}

int32_t SecInt::Subtract(int32_t lhs, int32_t rhs) {
    int32_t diff = 0;
    if ((rhs > 0 && lhs < (INT32_MIN + rhs)) ||
        (rhs < 0 && lhs > (INT32_MAX + rhs))) {
        diff = 0;
    }
    else {
        diff = lhs - rhs;
    }
    return diff;
}

uint32_t SecInt::Multiply(uint32_t lhs, uint32_t rhs) {
    uint32_t ret = 0;
    if (rhs != 0 && lhs > (UINT32_MAX/rhs)) {
       ret = UINT32_MAX;
    }
    else {
       ret = lhs * rhs;
    }
    return ret;
}

int32_t SecInt::Multiply(int32_t lhs, int32_t rhs) {
    int32_t ret = 0;

    if (rhs > 0) {
        if ((lhs > INT32_MAX / rhs)) {
            ret = INT32_MAX;
        }
        else if (lhs < INT32_MIN / rhs) {
            ret = INT32_MIN;
        }
    }
    else if (rhs < -1) { //exclude -1 as well min int value divide by -1 leads to overflow as well
        if (lhs > INT32_MAX / rhs) {
            ret = INT32_MAX;
        }
        if (lhs < INT32_MIN / rhs) {
            ret = INT32_MIN;
        }
    }

    if(ret == 0) {
        ret = lhs * rhs;
    }
    return ret;
}

int32_t SecInt::Divide(int32_t lhs, int32_t rhs) {

    int32_t ret = 0;
    if (rhs == 0){
        ret = 0;
    }
    else if (rhs == -1 && lhs == INT32_MIN) {
        ret = INT32_MIN;
    }
    else {
        ret = lhs / rhs;
    }
    return ret;
}