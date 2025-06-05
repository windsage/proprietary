/**
 * Copyright (c) 2018,2021-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "gpqese-be.h"
#include "gpqese-cold-reset.h"
#include "EseUtils/EseUtils.h"
#include <utils/Log.h>
#include <cutils/properties.h>

static TEEC_Result result = TEEC_SUCCESS;
static TEEC_Context context;
static TEEC_Session session;
static TEEC_Operation operation;
static TEEC_SharedMemory input;
static TEEC_SharedMemory output;
static bool contextInitialised;
static bool sessionOpen;
static bool iseSEGPOpen;
static bool iseSEPowered;
static uint32_t resultOrigin;
static std::mutex m;
bool DBG = false;

bool isSELinkActive() {
  return (contextInitialised && sessionOpen && iseSEGPOpen);
}

static int nativeeSEPowerOn () {
    return (eseSetPower(true) == 0);
}

static int nativeeSEPowerOff() {
    return (eseSetPower(false) == 0);
}

/**@brief:  Hard reset eSE by toggling SPI pins.
 *
 * @return  status code 0 on success, else -1 otherwise.
 */
static int resetSE() {
  int ret = -1;

  if (!nativeeSEPowerOff()) {
    ALOGE("Failure to power off the eSE");
    return ret;
  }
  iseSEPowered = false;

  if (!nativeeSEPowerOn()) {
    ALOGE("Failure to power on the eSE");
    return ret;
  }
  iseSEPowered = true;

  return 0;
}

static TEEC_Result do_close(TEEC_Result result) {

  TEEC_Result partialResult = result;

  if (partialResult != TEEC_SUCCESS) {
    ALOGE("ESE ERROR MESSAGE - let's close everything : 0x%08X", result);
  }

  if (partialResult == TEEC_ERROR_WTX_TIMEOUT) {
    if(eseGetVendorId() == ESE_VENDOR_STM) {
      ALOGE("%s Not able to recover from WTX, do interface reset", __func__);
      int32_t ret = gpqese_generic(GPQESE_CMD_GENERIC_TAG_INTERFACE_RESET, 0, &rx, &result);
      if (result != TEEC_SUCCESS || ret < 0) {
        ALOGE("%s interface reset failed", __func__);
        // We assume that eSE is in mute state, so we reset eSE.
        if (resetSE()) {
          ALOGE("Failure in recovering eSE !");
        }
      }
    } else {
      ALOGE("%s Not able to recover from WTX, calling ese_cold_reset", __func__);
      if (ese_cold_reset()) {
        ALOGE("ese_cold_reset failed !");
      }
    }
  }

  operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE,
                                          TEEC_NONE);
  result = TEEC_InvokeCommand(&session, GPQESE_CMD_CLOSE, &operation,
                              &resultOrigin);

  if ((result != TEEC_SUCCESS) && (partialResult == TEEC_SUCCESS)) {
    // We only want to log the error if we're in a "normal close".
    ALOGE("ESE ERROR MESSAGE  0x%08X", result);
  }
  else {
    // GPQESE_CMD_CLOSE is returned as success
    // client TA closed all open channels
    // reset channel state in HAL to be in sync with TA
    resetChannelState();
  }

  if (sessionOpen) {
    TEEC_CloseSession(&session);
  }

  if (contextInitialised) {
    TEEC_FinalizeContext(&context);
  }

  if (!nativeeSEPowerOff()) {
    ALOGE("Failure to power off the eSE");
  } else {
    iseSEPowered = false;
  }

  iseSEGPOpen = false;
  sessionOpen = false;
  contextInitialised = false;
  return result;
}

TEEC_Result gpqese_close(TEEC_Result result) {
  std::lock_guard<std::mutex> lock(m);

  if (contextInitialised || sessionOpen || iseSEGPOpen) {
    return do_close(result);
  }
  ALOGE("Session already closed, ignore close request");
  return TEEC_SUCCESS;
}

TEEC_Result gpqese_open() {
  std::lock_guard<std::mutex> lock(m);

  TEEC_Result ret = TEEC_SUCCESS;
  DBG = property_get_int32("ro.debuggable", 0);

  if (isSELinkActive()) {
    return TEEC_SUCCESS;
  }

  if (!contextInitialised) {
    /* Create the Context */
    result = TEEC_InitializeContext(teeName, &context);

    if (result != TEEC_SUCCESS) {
      ALOGE("TEEC_InitializeContext() ERROR MESSAGE  0x%08X", result);
      return result;
    }

    contextInitialised = true;
  }

  if (!nativeeSEPowerOn()) {
    ALOGE("eSE not powered on");
    iseSEPowered = false;
    return result;
  }
  iseSEPowered = true;

  if (!sessionOpen) {
    /* Open a session with the GP TA */
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE,
                                            TEEC_NONE);
    result = TEEC_OpenSession(&context, &session, &uuidEseGP, TEEC_LOGIN_USER, NULL,
                              &operation, &resultOrigin);

    if (result != TEEC_SUCCESS) {
      ALOGE("TEEC_OpenSession() failed : 0x%08X", result);
      return do_close(result);
    }

    sessionOpen = true;
  }

  if (!iseSEGPOpen) {
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE,
                                            TEEC_NONE);
    result = TEEC_InvokeCommand(&session, GPQESE_CMD_OPEN, &operation,
                                &resultOrigin);

    if (result != TEEC_SUCCESS) {
      ALOGE("GPQESE_CMD_OPEN failed : 0x%08X", result);
      ret = do_close(result);
      if (ret != TEEC_SUCCESS) {
        ALOGE("do_close failed : 0x%08X", ret);
      }
      return result;
    }

    iseSEGPOpen = true;
  }

  return result;
}

uint32_t gpqese_transceive(struct EseSgBuffer *tx, struct EseSgBuffer *rx,
                           TEEC_Result *result) {
  uint32_t ret = -1;
  uint32_t commandID;
  std::lock_guard<std::mutex> lock(m);

  if (tx == nullptr) {
    ALOGE("tx buffer is null");
    return ret;
  } else if (rx == nullptr) {
    ALOGE("rx buffer is null");
    return ret;
  } else if (result == nullptr) {
    ALOGE("result is null");
    return ret;
  } else if (tx->len > GPQESE_MAX_CAPDU_SIZE) {
    ALOGE("APDU too long %6" PRIu32" max is %6" PRIu32"", tx->len,
        GPQESE_MAX_CAPDU_SIZE);
    return ret;
  }

  if (!contextInitialised || !sessionOpen || !iseSEGPOpen || !iseSEPowered) {
    ALOGE("Transceive failed : Context initialized : %d, Session open : %d, eSE open %d, eSEPowerState : %d",
          contextInitialised, sessionOpen, iseSEGPOpen, iseSEPowered);
    return ret;
  }

  print_text("tApdu to be sent :", tx->c_base, tx->len);

  //INPUT MANAGEMENT
  if (tx->isRawTransmit) {
    operation.paramTypes = TEEC_PARAM_TYPES(
                           TEEC_MEMREF_PARTIAL_INPUT,  // Input buffer
                           TEEC_MEMREF_PARTIAL_OUTPUT, // Output Buffer
                           TEEC_VALUE_INPUT,
                           TEEC_NONE);

    operation.params[2].value.a = tx->rawTxChNum;
    commandID = GPQESE_CMD_TRANSCEIVE_RAW;
  } else {
    operation.paramTypes = TEEC_PARAM_TYPES(
                           TEEC_MEMREF_PARTIAL_INPUT,  // Input buffer
                           TEEC_MEMREF_PARTIAL_OUTPUT, // Output Buffer
                           TEEC_NONE,
                           TEEC_NONE);
    commandID = GPQESE_CMD_TRANSCEIVE;
  }

  input.size = tx->len;
  input.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  *result = TEEC_AllocateSharedMemory(&context, &input);

  if (*result != TEEC_SUCCESS) {
    ALOGE("AllocateSharedMemory failed() returned 0x%08X", *result);
    if (do_close(*result) != TEEC_SUCCESS) {
      ALOGE("do_close() failed");
    }
    return ret;
  }

  memscpy(input.buffer, input.size, (void *) &tx->c_base[0], tx->len);
  operation.params[0].memref.parent = &input;
  operation.params[0].memref.offset = 0;
  operation.params[0].memref.size = tx->len;


  //OUTPUT MANAGEMENT
  output.size = GPQESE_MAX_RAPDU_SIZE;
  output.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  *result = TEEC_AllocateSharedMemory(&context, &output);

  if (*result != TEEC_SUCCESS) {
    ALOGE("AllocateSharedMemory failed() returned 0x%08X", *result);
    TEEC_ReleaseSharedMemory(&input);
    if (do_close(*result) != TEEC_SUCCESS) {
      ALOGE("do_close() failed");
    }
    return ret;
  }

  memset(output.buffer, 0x0, output.size);
  operation.params[1].memref.parent = &output;
  operation.params[1].memref.offset = 0;
  operation.params[1].memref.size = output.size;
  *result = TEEC_InvokeCommand(&session, commandID, &operation,
                               &resultOrigin);

  if (*result != TEEC_SUCCESS) {
    ALOGE("Transceive failed() returned 0x%08X", *result);
    TEEC_ReleaseSharedMemory(&input);
    TEEC_ReleaseSharedMemory(&output);
    if ((*result == TEEC_ERROR_TARGET_DEAD) || (*result == TEEC_ERROR_WTX_TIMEOUT)) {
      if (do_close(*result) != TEEC_SUCCESS) {
        ALOGE("do_close() failed");
      }
    }
    return ret;
  }

  uint32_t size_read = (uint32_t)operation.params[1].memref.size;
  memscpy(rx->base, rx->len, (uint8_t *)output.buffer, size_read);

  print_text("rApdu received :", rx->base, size_read);

  TEEC_ReleaseSharedMemory(&input);
  TEEC_ReleaseSharedMemory(&output);
  return size_read;
}

int32_t gpqese_generic(GPQESE_Cmd_Generic_Tag reqType, uint32_t arg,
                           struct EseSgBuffer *rx,
                           TEEC_Result *result) {

  int32_t ret = -1;
  std::lock_guard<std::mutex> lock(m);

  //check for valid reqType
  if ((reqType != GPQESE_CMD_GENERIC_TAG_GET_ATR) &&
      (reqType != GPQESE_CMD_GENERIC_TAG_INTERFACE_RESET) &&
      (reqType != GPQESE_CMD_GENERIC_TAG_HARD_RESET) &&
      (reqType != GPQESE_CMD_GENERIC_TAG_SET_OSU_STATE) &&
      (reqType != GPQESE_CMD_GENERIC_TAG_IFS)) {
    ALOGE("Invalid reqType for generic command");
    return ret;
  }

  operation.paramTypes = TEEC_PARAM_TYPES(
                           TEEC_VALUE_INPUT,  // Input value
                           TEEC_MEMREF_PARTIAL_OUTPUT, // Output Buffer
                           TEEC_NONE,
                           TEEC_NONE);
  //INPUT MANAGEMENT
  operation.params[0].value.a = reqType;
  operation.params[0].value.b = arg;

  //OUTPUT MANAGEMENT
  output.size = GPQESE_MAX_RAPDU_SIZE;
  output.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  *result = TEEC_AllocateSharedMemory(&context, &output);

  if (*result != TEEC_SUCCESS) {
    ALOGE("AllocateSharedMemory failed() returned 0x%08X", *result);
    return ret;
  }

  memset(output.buffer, 0x0, output.size);
  operation.params[1].memref.parent = &output;
  operation.params[1].memref.offset = 0;
  operation.params[1].memref.size = output.size;
  *result = TEEC_InvokeCommand(&session, GPQESE_CMD_GENERIC, &operation,
                               &resultOrigin);

  if (*result != TEEC_SUCCESS) {
    ALOGE("Transceive failed() returned 0x%08X", *result);
    TEEC_ReleaseSharedMemory(&output);
    return ret;
  }
  uint32_t size_read = (uint32_t)operation.params[1].memref.size;
  memscpy(rx->base, rx->len, (uint8_t *)output.buffer, size_read);

  print_text("rApdu received :", rx->base, size_read);
  TEEC_ReleaseSharedMemory(&output);
  return size_read;
}
