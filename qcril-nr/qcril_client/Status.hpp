/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/**
 * @file Status.hpp
 * @brief Defines data types used in the RIL Client Library
 */

#pragma once

/**
 * Defines all the status codes that all APIs can return
 */
enum class Status {
    SUCCESS = 0,           /**< API processing is successful */
    FAILURE,               /**< API processing failure.*/
    SOCKET_FAILURE,        /**< Socket failure.*/
    SOCKET_WRITE_FAILURE,  /**< Socket write failure.*/
};
