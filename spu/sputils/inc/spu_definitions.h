/*!
 * Copyright (c) 2015-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <stdint.h>

/**
 * \file  spu_definitions.h
 * \brief Secure Processor Unit (SPU) Definitions Helper Header File
 *
 * \note  This header file contains helper definitions for SPU Android (HLOS) user space developers
 *        whether using libspcom or SPU HAL service (vendor.qti.spu@2.0 and up).
 *
 * A detailed documentation for developers can be found in Secure Processor Unit Enablement User
 * Guide.
 *
 * Both libspcom and SPU HAL service return error codes from errno.h.
 */

/**
 * \def      SPCOM_MAX_MESSAGE_SIZE
 * \brief    Max SPCom message buffer size
 *
 * Larger data can be shared by:
 * 1. Using DMA buffer when using libspcom or ISPUSharedBuffer when using SPU HAL interface.
 * 2. Divide original buffer into smaller blocks and send each block separately.
 */
#define SPCOM_MAX_MESSAGE_SIZE 268

/**
 * \def      SPCOM_CHANNEL_NAME_SIZE
 * \brief    Maximum size including string null terminator for channel names
 *
 * Match GLINK_NAME_SIZE
 */
#define SPCOM_CHANNEL_NAME_SIZE 32

/**
 * \def      SYSDATA_GET_SENSORS_DATA_BUF_SIZE
 * \brief    Maximum size needed for sensors information
 *
 */
#define SYSDATA_GET_SENSORS_DATA_BUF_SIZE 256

/**
 * \def      SP_SYSPARAM_LIFECYCLE_STATE_WRITE_MAGIC
 * \brief    Used as second argument to set SPU lifecycle state using sysparam write
 *
 */
#define SP_SYSPARAM_LIFECYCLE_STATE_WRITE_MAGIC 0xED56F451

/**
 * \def      SP_SYSPARAM_TRCR_STATE_ENABLE_MAGIC
 * \brief    Used as second argument to enable SPU TRCR using sysparam write
 *
 */
#define SP_SYSPARAM_TRCR_STATE_ENABLE_MAGIC 0xD0B05E26

/**
 * \def      SP_DEVICE_ISK_ID_SIZE_BYTES
 * \brief    Size of device ISK ID
 *
 */
#define SP_DEVICE_ISK_ID_SIZE_BYTES 0x7

/**
 * \enum     spu_hal_event_id_enum
 * \brief    SPU HAL event ID to identify the type of event being notified by SPU HAL server
 *
 * Event ID passed when ISPUNotifier::callback is called on SPU HAL client side to identify the
 * type of SPU event being notified by SPU HAL server
 */
typedef enum : uint32_t {
    SPU_HAL_EVENT_SSR = 1, ///< SPU SSR event occurred
} spu_hal_event_id_enum;

/**
 * \enum     spu_image_type_enum
 * \brief    SPU image type returned from ISPUManager::getImageType
 *
 * Indicated the type of image to load.
 */
typedef enum : uint32_t {
    SPU_IMAGE_TYPE_INVALID = 0, ///< Image type cannot be determined
    SPU_IMAGE_TYPE_TEST    = 1, ///< SPU test image: load *t.sig apps
    SPU_IMAGE_TYPE_PROD    = 2, ///< SPU prod image: load *p.sig apps
} spu_image_type_enum;

/**
 * \enum     sp_sysparam_id_enum
 * \brief    A List of predefined SPU system parameters
 */
typedef enum : uint32_t {
    SP_SYSPARAM_ID_FIRST                                = 0,
    SP_SYSPARAM_ID_BUILD_VERSION                        = 0,  ///< Read-Only
    SP_SYSPARAM_ID_BUILD_VERSION_HASH                   = 1,  ///< Read-Only
    SP_SYSPARAM_ID_ARI_CONFIG                           = 2,  ///< Write-Only, reboot is needed after activating ARI
    SP_SYSPARAM_ID_APP_BUILD_VERSION                    = 3,  ///< Read-Only
    SP_SYSPARAM_ID_APP_BUILD_VERSION_HASH               = 4,  ///< Read-Only
    SP_SYSPARAM_ID_HW_VERSION                           = 5,  ///< Read-Only
    SP_SYSPARAM_ID_FIPS_OVERRIDE                        = 6,  ///< Write-Only
    SP_SYSPARAM_ID_END_ARI_TEST_MODE                    = 7,  ///< Write-Only
    SP_SYSPARAM_ID_SET_ARI_TEST_MODE_THRESHOLD          = 8,  ///< Write-Only
                                                             /*!< Set test mode threshold
                                                              *   When calling sysparam write: var1 = percent number, var2 = adding 0.5%
                                                              *   For example 1.5 is 1 1 */
    SP_SYSPARAM_ID_GET_ARI_LIFECYCLE_STATE              = 9,  ///< Read-Only
    SP_SYSPARAM_ID_ARI_SUSPEND                          = 10, ///< Write-Only
    SP_SYSPARAM_ID_ARI_RESUME                           = 11, ///< Write-Only
    SP_SYSPARAM_ID_GET_ARI_SUSPENSION_STATE             = 12, ///< Read-Only, reboot is needed after resuming from suspend mode
    SP_SYSPARAM_ID_GET_NVM_OPEN_RESULT                  = 13, ///< Read-Only
    SP_SYSPARAM_ID_GET_ARI_MUTE_STATE                   = 14, ///< Read-Only
    SP_SYSPARAM_ID_GET_ARI_TEST_MODE_THRESHOLD          = 15, ///< Read-Only
                                                            /*!< Get ARI test mode threshold
                                                             * The value in the 16 upper bits is the threshold percent
                                                             * The value in the 16 lower bits is 1 if 0.5% is added or 0 if not
                                                             * For example 2.5% is 0x00020001 */
    SP_PBL_VERSION                                      = 16, ///< Read-Only
    SP_LIFECYCLE_STATE                                  = 17, ///< Read-Write
    SP_RESERVED_18                                      = 18, ///< Internal
    SP_RESERVED_19                                      = 19, ///< Internal
    SP_RESERVED_20                                      = 20, ///< Internal
    SP_RESERVED_21                                      = 21, ///< Internal
    SP_SYSPARAM_ID_GET_MROM_VERSION                     = 22, ///< Read-Only
    SP_SYSPARAM_ID_GET_FOUNDRY_ID                       = 23, ///< Read-Only
    SP_SYSPARAM_ID_GET_AR_VERSION_INFO                  = 24, ///< Read-Only
    SP_SYSPARAM_ID_GET_TRCR_STATUS                      = 25, ///< Read-Only
    SP_SYSPARAM_ID_GET_SOC_HW_VERSION                   = 26, ///< Read-Only
    SP_SYSPARAM_ID_GET_NON_CRITICAL_EVENT_COUNTER       = 27, ///< Read-Only
    SP_SYSPARAM_ID_ENABLE_TRCR                          = 28, ///< Write-Only
    SP_SYSPARAM_ID_IS_TRCR_ENABLED                      = 29, ///< Read-Only
    SP_SYSPARAM_ID_KPI_CONFIGURATION                    = 30, ///< Read-Write
    SP_SYSPARAM_ID_IS_ARI_TEST_MODE_THRESHOLD_REACHED   = 31, ///< Read-Only
} sp_sysparam_id_enum;

/**
 * \enum     sp_sysdata_id_enum
 * \brief    Predefined SPU system data ID to get mismatch data
 */
typedef enum : uint32_t {
    SP_SYSDATA_ID_GET_MISMATCH_DATA, ///< Read-Only
    SP_SYSDATA_ID_GET_SENSORS_DATA, ///< Read-Only
    SP_SYSDATA_ID_GET_DEVICE_ISK_ID, ///< Read-Only
    SP_SYSDATA_ID_GET_TRCR_HISTORY, ///< Read-Only
} sp_sysdata_id_enum;

/**
 * \def     TRCR history source
 * \brief   Params to send with SYSDATA_GET_TRCR_HISTORY
 */
#define TRCR_HISTORY_SRC_QFPROM 0
#define TRCR_HISTORY_SRC_NVM    1

/**
 * \def      SYSDATA_GET_MISMATCH_DATA_BUF_SIZE
 * \brief    Used as the buffer's size needed to get ARC mismatch data using sysdata
 */
#define SYSDATA_GET_MISMATCH_DATA_BUF_SIZE 0x500

/**
 * \def      SYSDATA_GET_TRCR_HISTORY_BUF_SIZE
 * \brief    Used as the buffer's size needed to get TRCR history using sysdata
 */
#define SYSDATA_GET_TRCR_HISTORY_BUF_SIZE 0x60

/**
 * \enum     sp_sysparam_lifecycle_write_event_t
 * \brief    Identifies the SPU lIfecycle state to set
 *           Used as first argument to set SPU lifecycle state using sysparam write
 */
typedef enum : uint32_t {
    SP_SYSPARAM_LIFECYCLE_WRITE_EVENT_READY_TO_UNLOCKED = 0x4F6F7073,
    SP_SYSPARAM_LIFECYCLE_WRITE_EVENT_SECURED = 0x87FA4C25,
} sp_sysparam_lifecycle_write_event_t;

/**
 * \enum     sp_sysparam_ariconfig_t
 */
typedef enum : uint32_t {
    SP_SYSPARAM_ARI_ACTIVATE = 1,
} sp_sysparam_ariconfig_t;

/**
 * \enum    sp_sysparam_fipsconfig_t
 */
typedef enum : uint32_t {
    SP_SYSPARAM_FIPS_OVERRIDE_ENABLED = 1,
} sp_sysparam_fipsconfig_t;

/**
 * \enum    sp_sysparam_trcr_enable_config_t
 */
typedef enum : uint32_t {
    SP_SYSPARAM_TRCR_ENABLED = 1,
} sp_sysparam_trcr_enable_config_t;

/**
 * \enum     nvm_ari_lifecycle_state
 * \brief    Return value of sysparam read SP_SYSPARAM_ID_GET_ARI_LIFECYCLE_STATE
 */
typedef enum : uint32_t {
    ARI_LIFECYCLE_UNINITIALIZED = 0x0,
    ARI_LIFECYCLE_TEST_MODE     = 0x4,
    ARI_LIFECYCLE_OPERATIONAL   = 0x6,
    ARI_LIFECYCLE_EXHAUSTED     = 0x7,
    ARI_LIFECYCLE_MUTED         = 0xE,
    ARI_LIFECYCLE_INVALID       = 0xFFFF,
} nvm_ari_lifecycle_state;

/**
 * \enum     nvm_ari_suspension_state
 * \brief    Return value of sysparam read SP_SYSPARAM_ID_GET_ARI_SUSPENSION_STATE
 */
typedef enum : uint32_t {
    NVM_ARI_NORMAL             = 0,
    NVM_ARI_SUSPENSION_PENDING = 1,
    NVM_ARI_SUSPENDED          = 2,
} nvm_ari_suspension_state;

/**
 * \enum     spu_lifecycle_state
 * \brief    Return value for sysparam read SP_LIFECYCLE_STATE
 */
typedef enum : uint32_t {
    SPU_LIFECYCLE_READY      = 0x1,
    SPU_LIFECYCLE_SECURED    = 0x3,
    SPU_LIFECYCLE_UNLOCKED   = 0x7,
    SPU_LIFECYCLE_TERMINATED = 0xF,
} spu_lifecycle_state;

/**
 * \enum    nvm_open_result
 * \brief   Return value of to sysparam read SP_SYSPARAM_ID_GET_NVM_OPEN_RESULT
 *
 * The result of segDB open_partition
 */
typedef enum : uint32_t {
    NVM_OPEN_RESULT_SUCCESS              = 0x0,
    NVM_OPEN_RESULT_CREATED_NOT_FOUND    = 0x1,
    NVM_OPEN_RESULT_CREATED_ARC_MISMATCH = 0x2,
    NVM_OPEN_RESULT_CREATED_CE           = 0x3,
    NVM_OPEN_RESULT_CREATED_OTHER        = 0x4,
} nvm_open_result;

#pragma pack(push, 1)

/**
 * \struct    por_data_struct
 * \brief     por_data field in sp_health_status_data struct
 */
typedef struct {
  uint8_t por_counter;
  uint8_t normal_boot_count_since_por;
  uint16_t commit_count_since_por;
} por_data_struct;

/**
 * \struct    ari_mismatch_data_struct
 * \brief     mismatch_data field in sp_health_status_data struct
 */
typedef struct {
  uint8_t current_boot_mismatch;
  uint8_t por_boot_mismatch_num;
  uint8_t normal_boot_mismatch_num;
  uint8_t fuse_count_left_by_granularity;
} ari_mismatch_data_struct;

/**
 * \struct    sp_health_status_data
 * \brief     return struct of SPU check health status
 */
typedef struct {
    uint32_t ari_lifecycle_state;
    uint32_t reserved1;
    uint32_t fips_enabled;
    uint32_t fips_self_test_passed;
    uint32_t sensors_calibrated;
    uint32_t reserved2;
    uint16_t por_indicator;
    uint16_t nvm_result;
    por_data_struct por_data;
    uint32_t aux_data0;
    uint32_t aux_data1;
    uint32_t aux_data2;
    uint32_t aux_data3;
    ari_mismatch_data_struct mismatch_data;
    uint32_t ari_fuse_gauge;
    uint32_t iar_status;
} sp_health_status_data;

/**
 * \struct    device_isk_id_struct
 * \brief     ISK ID of the device
 */
typedef struct {
  uint8_t info[SP_DEVICE_ISK_ID_SIZE_BYTES];
} device_isk_id_struct;

#pragma pack(pop)
