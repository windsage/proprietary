/**
 * Copyright (c) 2019,2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "gpqese-jcop.h"
#include "EseUtils/EseUtils.h"
#include <utils/Log.h>

static uint8_t vendorId = 0xFF;
static uint8_t currJcopMode = JCOP_UNKNOWN_MODE;

/**
 * @brief find max number of supported channels
 *
 * This function will return number of maximum channels
 * supported based on NFC HW.
 *
 * @return number of max supported channels
 */

int getMaxSupportedChannels()
{
  if (eseGetVendorId() == ESE_VENDOR_NXP) {
    if (vendorId != 0xFF && vendorId >= SN220_OS_VERSION_ID)
      return MAX_NUM_CHANNELS_SN220;
    else
      return MAX_NUM_CHANNELS_DEFAULT;
  }
  else if (eseGetVendorId() == ESE_VENDOR_STM)
    return  MAX_NUM_CHANNELS_ST54X;

  return MAX_NUM_CHANNELS_ISO;
}

/**
 * @brief Read current mode of JCOP.
 *
 * This function will read current mode of JCOP if supported,
 * otherwise it will return JCOP_UNKNOWN_MODE.
 *
 * @return Mode as defined in JcopMode_t
 */

static uint8_t getOsMode()
{
  ALOGD("%s current OSU mode : %d", __func__, currJcopMode);
  return currJcopMode;
}

/**
 * @brief Store current mode of JCOP.
 *
 * This function will store current mode of JCOP.
 *
 * @param[in] getATR - ATR response, len - ATR response length.
 *
 * @return void
 */

void updateOsMode(uint8_t *getATR, int32_t len)
{
  vendorId = getATR[ATR_OS_VERSION_OFF];

  if (len >= ATR_RSP_LEN && vendorId >= SN110_OS_VERSION_ID)
    currJcopMode = getATR[ATR_OS_TYPE_OFF];
}

/**
 * @brief Check if JCOP is in update mode.
 *
 * This function will check if currently JCOP is in update mode.
 *
 * @return true if JCOP is in update mode otherwise false
 */

bool isJcopUpdateMode()
{
  return (getOsMode() == JCOP_UPDATE_MODE);
}

/**
 * @brief Notify TEE about OSU state.
 *
 * This function will notify TEE whenever update started or finished.
 *
 * @param[in] osuState - OsuState_t.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int updateOSUState(OsuState_t osuState)
{
  TEEC_Result result = TEEC_ERROR_GENERIC;
  int32_t ret;

  ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_SET_OSU_STATE, osuState, &rx, &result);
  if (result != TEEC_SUCCESS || ret < 0) {
    ALOGE("Unable to notify TEE of OSU state");
    return STATUS_FAILURE;
  }
  return STATUS_SUCCESS;
}

/**
 * @brief reset JCOP.
 *
 * This function will do eSE hard reset, read ATR response
 * and update IFSC & IFSD to max supported value.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int resetJcopUpdate()
{
  TEEC_Result result = TEEC_ERROR_GENERIC;
  int32_t ret;

  ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_HARD_RESET, 0, &rx, &result);
  if (result != TEEC_SUCCESS || ret < 0) {
    ALOGE("HARD_RESET failed");
    return STATUS_FAILURE;
  }

  ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_GET_ATR, 0, &rx, &result);
  if (result != TEEC_SUCCESS || ret < 0) {
    ALOGE("getATR failed");
    return STATUS_FAILURE;
  }

  updateOsMode(rx.base, ret);
  ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_IFS, 0, &rx, &result);
  if (result != TEEC_SUCCESS || ret < 0) {
    ALOGE("set IFS failed");
    return STATUS_FAILURE;
  }

  return STATUS_SUCCESS;
}

/**
 * @brief Initialize eSE.
 *
 * This function will initialize eSE and do open basic channel
 * for boot time OSU update use case.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int ese_open()
{
  TEEC_Result result = TEEC_ERROR_GENERIC;
  const std::vector<uint8_t> aid;
  int rLen = 0;
  int cNumber = 0;

  result = gpqese_open();
  if (result != TEEC_SUCCESS) {
    ALOGE("gpqese_open failed");
    return STATUS_FAILURE;
  }

  //open basic channel for JCOP use case
  result = openChannel(aid, 0, &cNumber, rApdu, &rLen, true);
  if (result != TEEC_SUCCESS) {
    ALOGE("open basic channel failed");
    result = gpqese_close(result);
    if (result != TEEC_SUCCESS)
      ALOGE("gpqese_close failed");
    return STATUS_FAILURE;
  }
  return STATUS_SUCCESS;
}

/**
 * @brief De-initialize eSE.
 *
 * This function will de-initialize eSE and do close basic channel
 * for boot time OSU update use case.
 *
 * @return 0 - SUCCESS, 1 - FAILURE
 */

int ese_close()
{
  TEEC_Result result = TEEC_ERROR_GENERIC;

  /*
   * close BasicChannel, it was opened in ese_open()
   * channel close will take care of gpqese_close as well
   */
  result = closeGPChannel(0);
  if (result != TEEC_SUCCESS) {
    ALOGE("close BasicChannel failed");
    return STATUS_FAILURE;
  }
  return STATUS_SUCCESS;
}

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

void prepare_and_do_transceive(const uint8_t *tx_buff, int32_t tx_len, uint8_t *respApdu, int32_t rx_len, bool isRawTransmit, int32_t *rx_len_out)
{
  TEEC_Result result = TEEC_ERROR_GENERIC;

  if (!tx_buff || !respApdu || (rx_len > GPQESE_MAX_RAPDU_SIZE))
  {
    ALOGE("Invalid input !");
    return;
  }

  tx.c_base = tx_buff;
  tx.isRawTransmit = isRawTransmit;
  tx.len = tx_len;

  if (isRawTransmit)
    tx.rawTxChNum = BASIC_CHANNEL_NUM;
  else
    tx.rawTxChNum = INVALID_CHANNEL_NUM;

  *rx_len_out = gpqese_transceive(&tx, &rx, &result);
  if ((*rx_len_out > 1) && (result == TEEC_SUCCESS)) {
    //rx_len - Max size of receive buffer
    if (*rx_len_out < rx_len)
      memscpy(respApdu, *rx_len_out, rApdu, GPQESE_MAX_RAPDU_SIZE);
    else
      memscpy(respApdu, rx_len, rApdu, GPQESE_MAX_RAPDU_SIZE);
  }
}
