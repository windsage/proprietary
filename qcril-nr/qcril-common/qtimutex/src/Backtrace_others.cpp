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

/* default implementation */
void get_backtrace(uintptr_t* bt, size_t sz) {
    memset(bt, 0, sizeof(uintptr_t)*sz);
    if (sz > 0) {
        bt[0] = (uintptr_t)__builtin_return_address(0);
    }
#if 0
    if (sz > 1) {
        bt[1] = (uintptr_t)__builtin_return_address(1);
    }
    if (sz > 2) {
        bt[2] = (uintptr_t)__builtin_return_address(2);
    }
#endif
}
