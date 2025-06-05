/**
 * Copyright (c) 2018, 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GPQESE_BE_H_
#define __GPQESE_BE_H_

#include <mutex>
#include <TEE_client_api.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include "gpqese-utils.h"

#define GPQESE_CMD_TRANSCEIVE     0x10000000
#define GPQESE_CMD_OPEN           0x10000001
#define GPQESE_CMD_CLOSE          0x10000002
#define GPQESE_CMD_TRANSCEIVE_RAW 0x10000003
#define GPQESE_CMD_GENERIC        0x20000000 // @see GPQESE_Cmd_Generic_Tag

#define INVALID_CHANNEL_NUM       0xFF
#define BASIC_CHANNEL_NUM         0

#define GPQESE_MAX_CAPDU_SIZE 65546
#define GPQESE_MAX_RAPDU_SIZE 65539

typedef enum
{
  /* S-frame 0xC7 command */
  GPQESE_CMD_GENERIC_TAG_GET_ATR         = 0x00,
  /* S-frame 0xC4 command */
  GPQESE_CMD_GENERIC_TAG_INTERFACE_RESET = 0x01,
  /* S-frame 0xC6 command */
  GPQESE_CMD_GENERIC_TAG_HARD_RESET      = 0x02,
  /* S-frame 0xC1 command */
  GPQESE_CMD_GENERIC_TAG_IFS             = 0x03,
  /* Request to update OSU state */
  GPQESE_CMD_GENERIC_TAG_SET_OSU_STATE   = 0x04,
} GPQESE_Cmd_Generic_Tag;

#ifdef TARGET_USES_GPQTEEC
static const char *teeName = "GPQTEEC";
#else
static const char *teeName = NULL;
#endif

/*
 * UUID for gpqese TA, should match with
 * corresponding value in gptauuid.xml file
 */
static const TEEC_UUID uuidEseGP = {
  0x32552B22, 0x89FE, 0x42B4,
  { 0x8A, 0x45, 0xA0, 0xC4, 0xE2, 0xDB, 0x03, 0x26 }
};

extern bool bATRcached;

/* Scatter-Gather Buffer */
struct EseSgBuffer {
  union {
    uint8_t *base;
    const uint8_t *c_base;
  };
  /*
   * cmdAPDU won't be validated in TEE,
   * should be set only for OSU update
   */
  bool isRawTransmit;
  /*
   * channel number for raw transfer,
   * to be set only if isRawTransmit = true
   */
  uint32_t rawTxChNum;
  uint32_t len;
};

extern struct EseSgBuffer tx;
extern struct EseSgBuffer rx;
extern uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE];

bool isSELinkActive();
void resetChannelState();
int32_t gpqese_generic(GPQESE_Cmd_Generic_Tag reqType,  uint32_t arg,
                           struct EseSgBuffer *rx,
                           TEEC_Result *result);
uint32_t gpqese_transceive(struct EseSgBuffer *tx, struct EseSgBuffer *rx,
                           TEEC_Result *result);
TEEC_Result gpqese_open();
TEEC_Result gpqese_close(TEEC_Result result);

/**
 * @brief Send get ATR request to eSE and read response.
 *
 * This function will send get ATR request to eSE and
 * get the response, having HW specific details i.e vendor id etc
 *
 * @param[in] respApdu - ATR response, rLen - ATR response length.
 *
 * @return TEEC_SUCCESS on success otherwise error code
 */

TEEC_Result gpqese_getATR(uint8_t *respApdu, uint32_t *rspLen);

#endif
