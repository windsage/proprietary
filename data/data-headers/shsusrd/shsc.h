/******************************************************************************

                             S H S C . H

******************************************************************************/

/******************************************************************************

  @file    shsc.h
  @brief   shsusrd client header file

  DESCRIPTION
  Header file for shsusrd client.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __SHSC_H
#define __SHSC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * shsc status code
 */
#define SHSC_SUCCESS (0)
#define SHSC_ERR_INTERNAL (-1)
#define SHSC_ERR_INIT (-2)
#define SHSC_ERR_SYNC (-3)
#define SHSC_ERR_INVALID_PARAMS (-4)
#define SHSC_ERR_NO_MEM (-5)
#define SHSC_ERR_SEND (-6)
#define SHSC_ERR_RECV (-7)
#define SHSC_ERR_RECV_TIMEOUT (-8)

/*
 * Internal message indicating connection reset
 * Client callback invoked with NULL msg
 */
#define SHSC_MSG_RESET 1

/*
 * shsc callback function type.
 * Callback is invoked from a seperate thread and
 * the memory of msg_buf will be freed after callback
 */
typedef void (*shsc_cb)(uint32_t msg_id, void *msg_buf, uint32_t msg_buf_len,
			void *cb_data);

/**
 * shsc_init - initialize the shs client.
 * Only one client is supported per process.
 * @cb: callback function
 * @cb_data: callback data
 *
 * returns 0 on success, error code otherwise.
 */
int shsc_init(shsc_cb cb, void *cb_data);

/**
 * shsc_exit - shs client exit.
 * All information associated with the client will be removed.
 */
void shsc_exit(void);

/**
 * shsc_send_msg - send request message synchronously
 * @msg_id: msg id
 * @req: request buf pointer
 * @req_len: request buf len
 * @resp: response buf pointer
 * @resp_len: response buf len
 *
 * return 0 on success. error code otherwise.
 */
int shsc_send_msg(uint32_t msg_id, void *req, uint32_t req_len, void *resp,
		  uint32_t resp_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SHSC_H */
