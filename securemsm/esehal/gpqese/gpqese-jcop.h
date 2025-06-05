/**
 * Copyright (c) 2021,2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GPQESE_JCOP_H_
#define __GPQESE_JCOP_H_

#include "gpqese-apdu.h"

/*
 * OS version id of new JCOP
 * supporting OS Type property
 */
#define SN110_OS_VERSION_ID      0x11
#define SN220_OS_VERSION_ID      0x20
#define ATR_OS_TYPE_OFF          20
#define ATR_OS_VERSION_OFF       4

/*
 * ATR RSP length of new JCOP OS
 * supporting OsMode property
 */
#define ATR_RSP_LEN              21

// including basic channel
#define MAX_NUM_CHANNELS_DEFAULT 4
#define MAX_NUM_CHANNELS_SN220   5
#define MAX_NUM_CHANNELS_ST54X   4
#define MAX_NUM_CHANNELS_ISO     20

typedef enum
{
  JCOP_UNKNOWN_MODE,
  // normal mode
  JCOP_NORMAL_MODE,
  // either update in progress or teardown case
  JCOP_UPDATE_MODE
} JcopMode_t;

/*
 * OSU state
 */
typedef enum
{
  OSU_STATE_NORMAL,
  OSU_STATE_STARTED,
  OSU_STATE_FINISHED
} OsuState_t;

/**
 * @brief find max number of supported channels
 *
 * This function will return number of maximum channels
 * supported based on NFC HW.
 *
 * @return number of max supported channels
 */

int getMaxSupportedChannels();

/**
 * @brief Store current mode of JCOP.
 *
 * This function will store current mode of JCOP.
 *
 * @param[in] getATR - ATR response, len - ATR response length.
 *
 * @return void
 */

void updateOsMode(uint8_t *getATR, int32_t len);

/**
 * @brief Check if JCOP is in update mode.
 *
 * This function will check if currently JCOP is in update mode.
 *
 * @return true if JCOP is in update mode otherwise false
 */

bool isJcopUpdateMode();

/**
 * @brief Notify TEE about OSU state.
 *
 * This function will notify TEE whenever update started or finished.
 *
 * @param[in] osuState - OsuState_t.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int updateOSUState(OsuState_t osuState);

/**
 * @brief reset JCOP.
 *
 * This function will do eSE hard reset, read ATR response
 * and update IFSC & IFSD to max supported value.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int resetJcopUpdate();

/**
 * @brief Transmit APDU.
 *
 * This function will transmit APDU for boot time OSU update use case.
 *
 * @param[in] tx_buff - CAPDU, tx_len - CAPDU length
 *            respApdu - RAPDU, rx_len - MAX RAPDU length
 *            isRawTransmit - CAPDU validation will be skipped if set to true
 *            rx_len_out - Actual RAPDU length.
 *
 * @return void
 */

void prepare_and_do_transceive(const uint8_t *tx_buff, int32_t tx_len, uint8_t *respApdu, int32_t rx_len, bool isRawTransmit, int32_t *rx_len_out);

#endif
