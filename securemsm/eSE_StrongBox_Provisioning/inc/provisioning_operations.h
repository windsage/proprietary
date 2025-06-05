/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef PROVISIONING_OPERATIONS_H__
#define PROVISIONING_OPERATIONS_H__

typedef enum OEMProvisioningResult {
  /*
   * Operation executed successfully.
   */
  STATUS_SUCCESS                           = 0x00,

  /*
   * Failure ocuured in sending/receiving APDU to/from eSE
   */
  STATUS_ESE_COMMUNICATION_ERROR           = 0x01,

  /*
   * Failure occured in selecting strongbox applet.
   * Please check if the applet is present on eSE.
   */
  STATUS_ESE_APPLET_NOT_SELECTED           = 0x02,

  /*
   * Failure occured while provisioning base keys on eSE.
   */
  STATUS_ESE_KEYS_PROVISION_FAILED         = 0x03,

  /*
   * Base keys have already been locked on eSE.
   * This error could come in OEMProvisioning_proviosionBaseKeys
   * usecase.
   * This would mean that eSE has been provision locked
   * but failure was observed in locking the keys on TEE during
   * last provision lock procedure.
   *
   * In such cases, please contact the SoC or/and StrongBox vendor
   * for resolution/support.
   */
  STATUS_ESE_STRONGBOX_PROVISION_LOCKED    = 0x04,

  /*
   * Failure occured while locking base keys on eSE
   */
  STATUS_ESE_KEYS_PROVISION_LOCK_FAILED    = 0x05,

  /*
   * Keys derived by TEE and eSE are different.
   * In such cases, please re-execute OEMProvisioning_proviosionBaseKeys
   */
  STATUS_ESE_DERIVED_KEYS_DO_NOT_MATCH     = 0x06,

  /*
   * Other miscellaneous errors related to eSE.
   */
  STATUS_ESE_GENERIC_ERROR                 = 0x07,

  /*
   * Failure occured while establishing connection with TEE services.
   */
  STATUS_TEE_COMMUNICATION_ERROR           = 0x08,

  /*
   * Failure occured while genetrating keys at TEE end
   */
  STATUS_TEE_KEYS_GENERATION_FAILED        = 0x09,

  /*
   * Device in use is not RPMB provisioned.
   * Please try RPMB provisioning on device before further operations.
   */
  STATUS_TEE_RPMB_NOT_PROVISIONED          = 0x0A,

  /*
   * Keys have not been generated yet to save on TEE.
   * Please try generating base keys using
   * OEMProvisioning_proviosionBaseKeys()
   */
  STATUS_TEE_KEYS_NOT_SELECTED             = 0x0B,

  /*
   * Failure occured while provisioning base keys on TEE
   */
  STATUS_TEE_KEYS_PROVISION_FAILED         = 0x0C,

  /*
   * Base keys are provision locked on both TEE and eSE.
   * This status code would be observed if re-provisioing
   * of base keys is attempted on a provisioned locked device.
   */
  STATUS_TEE_DEVICE_PROVISION_LOCKED       = 0x0D,

  /*
   * Failure occured while locking the base keys on TEE.
   * If this status code is received, this would mean
   * that base keys have been locked on eSE but failed on TEE.
   *
   * In such cases, please contact the SoC or/and StrongBox vendor
   * for resolution/support.
   */
  STATUS_TEE_KEYS_PROVISION_LOCK_FAILED    = 0x0E,

  /*
   * Failure occured while generating checksum for device identifiers
   */
  STATUS_TEE_IDP_MAC_CALCULATION_FAILED    = 0x0F,

  /*
   * Other miscellaneous errors related to TEE
   */
  STATUS_TEE_GENERIC_ERROR                 = 0x10,

  /*
   * Other miscellaneous errors not related to eSE and TEE.
   */
  STATUS_GENERIC_ERROR                     = 0xFF
};

/**@brief:  Open a logical channel to select the strongbox applet
 *          for further operations.
 *          User need to ensure that this API is called only once before
 *          performing provisioning operations.
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_openSBLogicalChannel();

/**@brief:  Close the logical channel that was opened earlier for strongbox
 *          provisioning operations.
 *          It is responsibility of the user to call this API after performing
 *          required provisioning operations.
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_closeSBLogicalChannel();

/**@brief:  Initiate process to share base keys between the device and eSE.
 *          User can call this API as many times required till the eSE-StrongBox
 *          is in PROVISIONING mode.
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_proviosionBaseKeys();

/**@brief:  Commit the shared base keys on device and eSE.
 *          Base keys once committed on device and eSE, can't be modified or replaced.
 *          After the base keys are committed on eSE successfully, eSE Strongbox
 *          will transit into ACTIVE mode.
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_commitBaseKeys();

/**@brief:  Reprovision device identifier on the eSE.
 *          Device identifiers would be sent to eSE in CBOR encoded format.
 *
 *          User can call this API as many times as he wants.
 *          This API won't provision device identifiers on eSE if
 *          device identifiers are locked and finalised by user using
 *          OEMProvisioning_finalizeDeviceIdentifiers().
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_reprovisionDeviceIdentifiers();

/**@brief:  Finalize the device identifiers on eSE.
 *          Device identifiers would be sent to eSE in CBOR encoded format.
 *
 *          User must first ensure the device identifiers to be reprovisioned on eSE
 *          are correct using OEMProvisioning_reprovisionDeviceIdentifiers()
 *          before calling this API.
 *
 *          Device identifiers once committed on eSE will no more be replacable.
 *
 * @return  Value STATUS_SUCCESS on success or other value on failure.
 */
int OEMProvisioning_finalizeDeviceIdentifiers();

#endif
