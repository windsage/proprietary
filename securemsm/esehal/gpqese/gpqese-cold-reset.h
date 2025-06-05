/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GPQESE_COLD_RESET_H_
#define __GPQESE_COLD_RESET_H_

#define COLD_RESET_IOCTL_FAILURE (-1)

/**
 * @brief ese cold reset function.
 *
 * Sends cold reset ioctl to esepowermanager.
 *
 * @return: 0 - success; -1 - failure.
 */

int ese_cold_reset();

/**
 * @brief ese cold reset protection function.
 *
 * Sends cold reset protection enable or disable
 * ioctl to NFC kernel driver via esepowermanager.
 *
 * @param[in] enable - true, if request for protection enable
 *                   - false otherwise.
 *
 * @return: 0 - success; -1 on failure.
 */

int do_cold_reset_protection(bool enable);

#endif
