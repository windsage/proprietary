/**
 * Copyright (c) 2018,2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GPQESE_API_H_
#define __GPQESE_API_H_

#include "interface.h"

/**
 * @brief Initialize eSE.
 *
 * This function will initialize eSE and do open basic channel
 * for boot time OSU update use case.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int ese_open();

/**
 * @brief De-initialize eSE.
 *
 * This function will de-initialize eSE and do close basic channel
 * for boot time OSU update use case.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int ese_close();

#endif /* __GPQESE_API_H_ */
