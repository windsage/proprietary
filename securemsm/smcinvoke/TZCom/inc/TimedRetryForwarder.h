// Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
/** @cond */

#ifndef TIMEDRETRYFORWARDER_H
#define TIMEDRETRYFORWARDER_H

#include <cstdint>
#include <cstddef>
#include "object.h"

/* TimedRetryForwarder is useful in cases where we can have multiple clients calling into TZ/TA
   concurrently. In such scenarios all the processes except the first will get Object_ERROR_BUSY.
   So, in order to retry these calls from client TimedRetryForwarder is used. */

#define DEFAULT_TIMERETRYFORWARDER_TIMEOUT_MS      2000
#define DEFAULT_TIMERETRYFORWARDER_INTERVAL_MS     30

int32_t TimedRetryForwarder_new(size_t msTimeout, size_t msInterval, OBJECT_CONSUMED Object obj, Object *objOut);

#endif // TIMEDRETRYFORWARDER_H
