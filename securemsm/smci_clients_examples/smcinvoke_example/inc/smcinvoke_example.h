/***********************************************************
 Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
#include<stdio.h>
#include<stdint.h>
#include <string.h>

int32_t run_smcinvoke_ta_example(char *appPath, char *appPath2);

static inline int memscpy(void* dest, size_t destLen, const void* src, size_t srcLen) {
    if (srcLen <= destLen) {
        memcpy(dest, src, srcLen);
        return 0;
    } else {
        return -1;
    }
}
