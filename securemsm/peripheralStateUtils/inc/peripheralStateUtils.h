/**
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PERIPHERALSTATE_UTILS_H__
#define __PERIPHERALSTATE_UTILS_H__

/**
 * These constants define the possible states of a peripheral
 */
typedef enum {
  STATE_SECURE              = 0x01,
  STATE_NONSECURE           = 0x02,
  STATE_EMERGENCY_CALL      = 0x03,
  STATE_PRE_CHANGE          = 0x04,/* Notified before changing state to nonsecure, To enable clocks for AUDIO */
  STATE_POST_CHANGE         = 0x05,/* Notified after changing state to secure, To disable clocks for AUDIO */
  STATE_RESET_CONNECTION    = 0xFF /* When notification connection fails */
} PeripheralStates;

/**
 * These constants define the possible errors to callers.
 */
const int32_t PRPHRL_SUCCESS = 0;
const int32_t PRPHRL_ERROR = -1;
const int32_t PRPHRL_ERROR_NOT_SUPPORTED = -2;

/*
 * @type    Function Pointer
 *
 * @brief   PeripheralStateCB is a function pointer which is a callback function
 *          for state change handler in caller process. When a SecureMode Event
 *          occurs from Peripheral Service in TZ then the handler function
 *          registered is called.
 *
 */
typedef int32_t (*PeripheralStateCB)(const uint32_t peripheral,
                                     const uint8_t state);

/*
 * @func    registerPeripheralCB
 *
 * @params  peripheral - valid peripheral UIDs are available at
 *          CPeripheralAccessControl.h
 *
 * @params  NotifyEvent - Function pointer to call back function which
 *          will be triggered when Peripheral Service in TZ wants to notify
 *          about secure state
 *
 * @brief   Registers the Peripheral callback function to Peripheral Service in
 *          TZ to get the secure state change notifications.
 *
 * @return  CONTEXT upon sucess which will be used to getPeripheralState and
 * deregisterPeripheralCB
 *          NULL upon failure to register with Peripheral Service in TZ
 */
void *registerPeripheralCB(uint32_t peripheral, PeripheralStateCB NotifyEvent);

/*
 * @func    deregisterPeripheralCB
 *
 * @params  *ctx - send the context recieved from registerPeripheralCB
 *
 * @brief   Deregister the callback function registered using
 *          registerPeripheralCB() function.
 *
 * @return  PRPHRL_SUCCESS upon sucess
 *          PRPHRL_ERROR upon failure
 */
int32_t deregisterPeripheralCB(void *ctx);

/*
 * @func    getPeripheralState
 *
 * @params  *ctx - send the context recieved from registerPeripheralCB
 *
 * @brief   get the peripheral state from Peripheral Service in TZ.
 *
 * @return  upon sucess return the STATE of the peripheral.
 *          PRPHRL_ERROR upon failure
 *          PRPHRL_ERROR_NOT_SUPPORTED if Feature or Fuse is not present
 */
int32_t getPeripheralState(void *ctx);

#endif /* __PERIPHERALSTATE_UTILS_H__ */
