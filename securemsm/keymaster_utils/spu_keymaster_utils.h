/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SPU_KEYMASTER_UTILS_H_
#define _SPU_KEYMASTER_UTILS_H_

#include <hardware/keymaster_defs.h>

namespace keymasterutils {

class SpuKeymasterUtils {
public:
    static bool is_spu_gk_supported(void);
};

} // namespace keymasterutils
#endif /* _SPU_KEYMASTER_UTILS_H_ */
