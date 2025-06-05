/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "gpqese-be.h"
#include "gpqese-cold-reset.h"
#include "EseUtils/EseUtils.h"
#include <utils/Log.h>

/**
 * @brief Cold reset ioctl callback function.
 *
 * This function will be called when cold reset ioctl
 * returns from esepowermanager.
 *
 * @param[in] outData output data from esepowermanager.
 *
 * @return void
 */

/**
 * @brief ese cold reset function.
 *
 * Sends cold reset ioctl to NFC kernel driver.
 *
 * @return: 0 - success; -1 - failure.
 */

int ese_cold_reset()
{
    return eseIoctl(ESE_COLD_RESET_NXP, ESE_COLD_RESET_DO);
}

/**
 * @brief ese cold reset protection function.
 *
 * Sends cold reset protection enable or disable
 * ioctl to NFC kernel driver
 *
 * @param[in] enable - true, if request for protection enable
 *                   - false otherwise.
 *
 * @return: 0 - success; -1 on failure.
 */

int do_cold_reset_protection(bool enable) {
    int ret = 0;
    if (enable)
        ret = eseIoctl(ESE_COLD_RESET_NXP, ESE_COLD_RESET_PROTECT_EN);
    else
        ret = eseIoctl(ESE_COLD_RESET_NXP, ESE_COLD_RESET_PROTECT_DIS);

    return ret;
}
