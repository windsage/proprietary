/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "spu_keymaster_utils.h"

using namespace keymasterutils;

bool SpuKeymasterUtils::is_spu_gk_supported(void) {
#if ENABLE_SPU_GK
    return true;
#else
    return false;
#endif
}
