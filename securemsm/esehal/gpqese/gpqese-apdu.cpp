/**
 * Copyright (c) 2018,2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <algorithm>
#include "gpqese-jcop.h"
#include "EseUtils/EseUtils.h"
#include <utils/Log.h>

uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {0x00};
static mChannelInfo_t channelsInfo[MAX_NUM_CHANNELS_ISO];
static int maxChannels = MAX_NUM_CHANNELS_ISO;
static uint8_t cacheAtrApdu[GPQESE_MAX_RAPDU_SIZE] = {0x00};
static uint32_t cacheAtrLen = 0;
bool bATRcached = false;

struct EseSgBuffer tx = {
  .c_base = {}, .isRawTransmit = false,
  .rawTxChNum = 0xFF, .len = 0,
};

struct EseSgBuffer rx = {
  .base = rApdu, .isRawTransmit = false,
  .rawTxChNum = 0xFF,
  .len = GPQESE_MAX_RAPDU_SIZE,
};

bool allChannelsClosed() {
  for (int i = 0; i < maxChannels; i++) {
    if (channelsInfo[i].isOpened == true) {
      ALOGD("Channel number %d still open", i);
      return false;
    }
  }
  return true;
}

void resetChannelState() {
  for (int i = 0; i < maxChannels; i++) {
    if (channelsInfo[i].isOpened == true) {
      ALOGD("Channel number %d marked as closed", i);
      channelsInfo[i].isOpened = false;
    }
  }
}

TEEC_Result ese_init()
{
  TEEC_Result result = TEEC_ERROR_GENERIC;
  int32_t ret;

  if (isSELinkActive())
    return TEEC_SUCCESS;

  result = gpqese_open();
  if (result != TEEC_SUCCESS) {
    ALOGE("gpqese_open failed");
    return result;
  }

  ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_INTERFACE_RESET, 0, &rx, &result);
  if (result != TEEC_SUCCESS || ret < 0) {
    ALOGE("interface reset failed");
    return result;
  }

  if(eseGetVendorId() == ESE_VENDOR_NXP) {
    // store JCOP state
    updateOsMode(rx.base, ret);
  }

  return result;
}

/**
 * @brief find if channel open is allowed
 *
 * This function will check whether maximum number of allowed
 * logical channels are already opened or not.
 *
 * @return true if channel open is allowed otherwise false
 */

static bool isChannelOpenAllowed()
{
  int numOpenLogicalChannels = 0;

  maxChannels = getMaxSupportedChannels();
  ALOGD("max supported channels - %d\n", maxChannels);

  // index starts from 1 as 0 is reserved for basic channel
  for (int i = 1; i < maxChannels; i++) {
    if (channelsInfo[i].isOpened == true)
      numOpenLogicalChannels++;
    else
      break;
  }

  // already opened max supported logical channels ?
  // minus 1 to ignore basic channel
  if (numOpenLogicalChannels >= (maxChannels - 1)) {
    ALOGE("max supported logical channels are already opened");
    return false;
  }
  return true;
}

TEEC_Result openChannel(const std::vector<uint8_t> &aid, uint8_t p2, int *cNumber,
                        uint8_t *respApdu, int *rLen, bool isBasic) {

  uint8_t *tApdu = nullptr;
  int tApduSize = 0;
  TEEC_Result result = TEEC_ERROR_GENERIC;

  if (rLen == nullptr) {
    ALOGE("rLen is null");
    return result;
  } else if (respApdu == nullptr) {
    ALOGE("respApdu is null");
    return result;
  } else if (cNumber == nullptr) {
    ALOGE("cNumber is null");
    return result;
  }

  if (isBasic && channelsInfo[0].isOpened == true) {
    ALOGE("Basic channel is already open");
    return result;
  }

  if (!isBasic && !isChannelOpenAllowed())
    return result;

  //Let's check if the eSE is powered ON, and the TA ready.
  result = gpqese_open();
  if (result != TEEC_SUCCESS) {
    return result;
  }

  if (!isBasic) {
    tx.c_base = MC_OPEN;
    tx.len  = sizeof(MC_OPEN);
    *rLen = gpqese_transceive(&tx, &rx, &result);
    if (result != TEEC_SUCCESS) {
      goto clean_up;
    }
  }

  if ((*rLen == 3) || isBasic) {
    if (((rApdu[1] == 0x90) && (rApdu[2] == 0x00)) || isBasic) {
      /* If AID is not present, Le would be part of SELECT itself and byte for Lc is not required */
      tApduSize = sizeof(SELECT) + aid.size() + ((aid.size() > 0) ? 1 : 0);
      tApdu = new (std::nothrow) uint8_t[tApduSize];
      if (NULL == tApdu) {
        ALOGE("Unable to create tApdu");
        result = TEEC_ERROR_OUT_OF_MEMORY;
        goto clean_up;
      }
      memscpy(tApdu, tApduSize, SELECT, sizeof(SELECT)); //Select command

      if (isBasic) {
        tApdu[0] = 0x00; //Channel Number
      } else {
        tApdu[0] = rApdu[0]; //Channel Number
      }

      tApdu[3] = p2; //p2
      tApdu[4] = aid.size(); //aid.size()
      memscpy(tApdu + 5, tApduSize-5, aid.data(), aid.size()); //AID
      tApdu[tApduSize - 1] = 0x00;
      tx.c_base = tApdu;
      tx.len = tApduSize;
      if ((tApdu[0] < 0) || (tApdu[0] >= maxChannels)) {
        ALOGE("Invalid channel number :  %d", tApdu[0]);
        result = TEEC_ERROR_GENERIC;
        goto clean_up;
      }
      *cNumber = tApdu[0];
      /* As per standard ISO7816-4 Section 5.4.1 */
      if (*cNumber > MAX_NUM_LOGICAL_CH_INTER_INDUSTRY && *cNumber < MAX_NUM_CH_FURTHER_INTER_INDUSTRY) {
        tApdu[0] = 0x40 | (*cNumber - MAX_NUM_CH_INTER_INDUSTRY);
      }
      channelsInfo[*cNumber].isOpened = true;
      *rLen = gpqese_transceive(&tx, &rx, &result);
      delete[] tApdu;
      if ((*rLen > 1) && (result == TEEC_SUCCESS)) {
        memscpy(respApdu, *rLen, rApdu, sizeof(rApdu));

        if (rApdu[*rLen - 2] == 0x69 && rApdu[*rLen - 1] == 0x85) {
          ALOGE("Condition of use not satisfied");
          return  TEEC_ERROR_ITEM_NOT_FOUND;
        } else if ((rApdu[*rLen - 2] == 0x6A && rApdu[*rLen - 1] == 0x82) ||
                   (rApdu[*rLen - 2] == 0x69 && rApdu[*rLen - 1] == 0x99)) {
          ALOGE("Applet can't be selected");
          return TEEC_ERROR_ITEM_NOT_FOUND;
        } else {
          memscpy(channelsInfo[*cNumber].aid, MAX_AID_SIZE, aid.data(), aid.size());
          return TEEC_SUCCESS;
        }
      } else if (result != TEEC_SUCCESS) {
          ALOGE("transceive failed");
          goto clean_up;
      } else {
        result = TEEC_ERROR_GENERIC;
        goto clean_up;
      }
    }
  } else if ((*rLen == 2) && (rApdu[0] == 0x6A && rApdu[1] == 0x81)) {
    result = TEEC_ERROR_OUT_OF_MEMORY; // No more channel available
    goto clean_up;
  } else {
    ALOGE("OpenChannel failed - ret = %d", *rLen);
    goto clean_up;
  }

  return result;
clean_up:
  gpqese_close(result);
  return result;
}

TEEC_Result transmitApdu (const std::vector<uint8_t> &data, bool isRawTransmit,
                          uint32_t rawTxChNum, uint8_t *respApdu, int *rLen) {

  TEEC_Result result = TEEC_ERROR_GENERIC;
  if (respApdu == nullptr) {
    ALOGE("respApdu is null");
    return result;
  } else if (rLen == nullptr) {
    ALOGE("rLen is null");
    return result;
  }
  //Let's check if the eSE is powered ON, and the TA ready.
  result = gpqese_open();

  if (result != TEEC_SUCCESS) {
    return result;
  }

  tx.c_base = data.data();
  tx.isRawTransmit = isRawTransmit;
  tx.rawTxChNum = rawTxChNum;
  tx.len = data.size();
  *rLen = gpqese_transceive(&tx, &rx, &result);

  if (TEEC_SUCCESS != result) {
    ALOGE("transmit failed !");
    return result;
  }

  if (*rLen > 1) {  // minimum rApdu expected is SW
    memscpy(respApdu, *rLen, rApdu, sizeof(rApdu));

    if (memsApducmp(SELECT, sizeof(SELECT), tx.c_base,
                tx.len)) { //check for close basic channel cmd.
      ALOGD("Close basic channel detected");
      channelsInfo[0].isOpened = false;

      if (allChannelsClosed()) {
        return gpqese_close(result);
      }
    }
  }

  return result;
}

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

TEEC_Result gpqese_getATR(uint8_t *respApdu, uint32_t *rLen) {
  TEEC_Result result = TEEC_ERROR_GENERIC;
  int32_t ret;
  if (respApdu == nullptr || rLen == nullptr) {
    ALOGE("Invalid input argument");
    return result;
  };

  // Let's check if the eSE is powered ON, and the TA ready.
  result = gpqese_open();
  if (result != TEEC_SUCCESS) {
    return result;
  }

  if (eseGetVendorId() == ESE_VENDOR_STM) {
    if (bATRcached == false) {
      ALOGD("Sending ATR request to eSE HW");
      ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_INTERFACE_RESET, 0, &rx, &result);
      if (ret < 0) {
        ALOGE("getATR failed");
        goto exit;
      }
      *rLen = ret;
      cacheAtrLen = ret;
      bATRcached = true;
      memscpy(respApdu, *rLen, rApdu, sizeof(rApdu));
      memscpy(cacheAtrApdu, *rLen, rApdu, sizeof(rApdu));
    }
    else {
      ALOGD("Returning the Cached ATR response");
      *rLen = cacheAtrLen;
      memscpy(respApdu, *rLen, cacheAtrApdu, cacheAtrLen);
    }

  }
  else {
    ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_GET_ATR, 0, &rx, &result);
    if (ret < 0) {
      ALOGE("getATR failed");
      goto exit;
    }
    *rLen = ret;
    memscpy(respApdu, *rLen, rApdu, sizeof(rApdu));
  }

exit:
  if (allChannelsClosed()) {
    if (gpqese_close(result) != TEEC_SUCCESS) {
      ALOGE("gpqese_close failed");
      return result;
    }
  }

  return result;
}

TEEC_Result closeGPChannel(uint8_t channelNumber) {
  uint8_t *tApdu = nullptr;
  TEEC_Result result = TEEC_ERROR_GENERIC;
  int tApduSize = 0;
  int ret = -1;

  if ((channelNumber < 0) || (channelNumber >= maxChannels)) {
    ALOGE("Channel number not consistent : %d", channelNumber);
    return result;
  }

  if (channelsInfo[channelNumber].isOpened == false) {
    ALOGE("%s: channel already closed", __func__);
    return result;
  }

  if (channelNumber == 0) {
    tApduSize = sizeof(SELECT);
    tApdu = new (std::nothrow) uint8_t[tApduSize];
    if (NULL == tApdu) {
      ALOGE("Unable to create tApdu");
      return TEEC_ERROR_OUT_OF_MEMORY;
    }
    memscpy(tApdu, tApduSize, SELECT, tApduSize); //Empty Select command
  } else {
    tApduSize = sizeof(MC_CLOSE);
    tApdu = new (std::nothrow) uint8_t[tApduSize];
    if (NULL == tApdu) {
      ALOGE("Unable to create tApdu");
      return TEEC_ERROR_OUT_OF_MEMORY;
    }
    memscpy(tApdu, tApduSize, MC_CLOSE, tApduSize);
    tApdu[0] = channelNumber;
    /* As per standard ISO7816-4 Section 5.4.1 */
    if (channelNumber > MAX_NUM_LOGICAL_CH_INTER_INDUSTRY && channelNumber < MAX_NUM_CH_FURTHER_INTER_INDUSTRY) {
        tApdu[0] = 0x40 | (channelNumber - MAX_NUM_CH_INTER_INDUSTRY);
    }
    tApdu[3] = channelNumber;
  }

  tx.c_base = tApdu;
  tx.len  = tApduSize;
  ret = gpqese_transceive(&tx, &rx, &result);
  delete[] tApdu;
  if ((ret < 1) || (result != TEEC_SUCCESS)) {
    ALOGE("Close Channel failed ");
    return result;
  }
  if ((result == TEEC_SUCCESS) && ((rApdu[ret - 2] == 0x90)
                                   && (rApdu[ret - 1] == 0x00))) {
    channelsInfo[channelNumber].isOpened = false;

    if (allChannelsClosed()) {
      if (gpqese_close(result) == TEEC_SUCCESS) {
        return result;
      }
    }

    return result;
  }

  ALOGE("Close Channel failed");
  return result;
}
