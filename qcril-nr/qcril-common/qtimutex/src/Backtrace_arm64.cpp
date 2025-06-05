/* ============================================================
 *
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
stop conditions of iteration:
   1) array size is reached
   2) fp & 0xf (frame pointer must be aligned with 16 bytes)
   3) fp == 0x00
*/
void get_backtrace(uintptr_t* bt, size_t sz) {
    memset(bt, 0, sizeof(uintptr_t)*sz);
    asm volatile(
        "mov x9, %0 \n"
        "mov x12, %1 \n"
        "mov x10, x29 \n"
    "1:\n"
        "ldp x10, x11, [x10] \n"
        "and x13, x10, 0xf \n"
        "cbnz x13, 2f \n"
        "cbz x10, 2f \n"
        "sub x11, x11, #4 \n"
        /* "xpaci x11 \n" */
        "str x11, [x9], #8 \n"
        "subs x12, x12, #1 \n"
        "b.ne 1b \n"
    "2:\n"
        :
        : "r"(bt), "r"(sz)
        : "memory", "x9","x10","x11","x12","x13");
}
