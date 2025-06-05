/*==============================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "QmsSdkCommon.h"

#define RFID_MAX_FREQ 8
#define RFID_MAX_COMM_STD_BASED_TABLE 16
#define RFID_MAX_UHF_RF_TABLE 32
#define RFID_CHANNEL_INDEX_MAX_LENGTH 16
#define RFID_MAX_COMMON_TAG_MASK_SIZE 8
#define RFID_MAX_COMMON_FILTER 4
#define RFID_MAX_TAG_MASK_SIZE 128
#define RFID_MAX_TAG_DATA_SIZE 128
#define RFID_MAX_WRITE_DATA_SIZE 2048
#define RFID_MAX_BLOCK_MASK 128
#define RFID_MAX_CRYPTO_MESSAGE 128
#define RFID_MAX_LOCK_PARAM_SIZE 5
#define RFID_MAX_ACCESS_FILTERS 4
#define RFID_MAX_ACCESS_CMDS 10 // Practical limit should be 4
#define RFID_MAX_READ_DATA_SIZE 2048
#define RFID_MAX_PERMA_LOCK_SIZE 128
#define RFID_MAX_AUTH_RESP_SIZE 1024
#define RFID_MAX_EPC_DATA_SIZE 16
#define RFID_MAX_INVENTORY_SCAN_TAG_SIZE 10

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                           ENUMS
---------------------------------------------------------------------------*/

typedef enum {
    RFID_COM_STANDARD_UNSPECIFIED = 0,
    RFID_COM_STANDARD_FCC_15_247,
    RFID_COM_STANDARD_ETSI_302_208,
    RFID_COM_STANDARD_CHINA_920_925,
} rfid_com_standard_e;

typedef enum {
    RFID_DR_8 = 0,
    RFID_DR_64_3
} rfid_dr_e;

typedef enum {
    RFID_SYMBOL_M1 = 0,
    RFID_SYMBOL_M2,
    RFID_SYMBOL_M4,
    RFID_SYMBOL_M8,
} rfid_symbol_rep_e;

// Forward Link Mod
typedef enum {
    RFID_FLM_PR_ASK = 0,
    RFID_FLM_SSB_ASK,
    RFID_FLM_DSB_ASK
} rfid_flm_e;

// Spectral Mask Indicator
typedef enum {
    RFID_SMI_UNKNOWN = 0,
    RFID_SMI_SINGLE_READER,
    RFID_SMI_MULTI_READER,
    RFID_SMI_DENSE
} rfid_smi_e;

typedef enum {
    RFID_ANTENNA_1 = 0,
    RFID_ANTENNA_2,
} rfid_antenna_id_e;

typedef enum {
    RFID_MB_RESERVED = 0,
    RFID_MB_EPC,
    RFID_MB_TID,
    RFID_MB_USER
} rfid_memory_bank_e;

typedef enum {
    RFID_TRUNC_UNSPECIFIED = 0,
    RFID_TRUNC_NONE,
    RFID_TRUNC_TRUNCATE
} rfid_truncate_e;

typedef enum {
    RFID_UNAWARE_ACTION_SELECT_UNSELECT = 0,
    RFID_UNAWARE_ACTION_SELECT_NOOP,
    RFID_UNAWARE_ACTION_NOOP_UNSELECT,
    RFID_UNAWARE_ACTION_UNSELECT_NOOP,
    RFID_UNAWARE_ACTION_UNSELECT_SELECT,
    RFID_UNAWARE_ACTION_NOOP_SELECT
} rfid_unaware_action_e;

typedef enum {
    RFID_SELECT_TARGET_S0 = 0,
    RFID_SELECT_TARGET_S1,
    RFID_SELECT_TARGET_S2,
    RFID_SELECT_TARGET_S3,
    RFID_SELECT_TARGET_SL
} rfid_select_target_e;

typedef enum {
    RFID_SELECT_ACTION_ASSERT_SL_A_B = 0,
    RFID_SELECT_ACTION_ASSERT_SL_A,
    RFID_SELECT_ACTION_ASSERT_SL_B,
    RFID_SELECT_ACTION_NEGATE_SL_MATCH,
    RFID_SELECT_ACTION_DEASSERT_SL_B_A,
    RFID_SELECT_ACTION_DEASSERT_SL_B,
    RFID_SELECT_ACTION_DEASSERT_SL_A,
    RFID_SELECT_ACTION_NEGATE_SL_MISMATCH
} rfid_select_action_e;

typedef enum {
    RFID_SESSION_S0 = 0,
    RFID_SESSION_S1,
    RFID_SESSION_S2,
    RFID_SESSION_S3
} rfid_session_e;

typedef enum {
    RFID_TARGET_A = 0,
    RFID_TARGET_B
} rfid_target_e;

typedef enum {
    RFID_START_TRIGGER_IMMEDIATE = 0,
    RFID_START_TRIGGER_PERIODIC,
    RFID_STOP_TRIGGER_UI,
    RFID_STOP_TRIGGER_DURATION,
    RFID_STOP_TRIGGER_TAG_OBSERVATION
} rfid_trigger_type_e;

typedef enum {
    RFID_REPORT_TRIGGER_N_TAG = 0,
    RFID_REPORT_TRIGGER_N_MSEC,
    RFID_REPORT_TRIGGER_N_INVENTORY_ROUNDS
} rfid_report_trigger_e;

typedef enum {
    RFID_N_OR_TIMEOUT = 0,
    RFID_NO_MORE_OR_TIMEOUT,
    RFID_ATTEMPTS_OR_TIMEOUT,
    RFID_UNIQUE_N_OR_TIMEOUT,
    RFID_NO_UNIQUE_OR_TIMEOUT,
} rfid_tag_oberservation_trigger_e;

typedef enum {
    RFID_ACCESS_ACTION_START = 0,
    RFID_ACCESS_ACTION_STOP,
} rfid_access_action_e;

typedef enum {
    RFID_PRIV_UNLOCK = 0,
    RFID_PRIV_PERMA_UNLOCK,
    RFID_PRIV_RW,
    RFID_PRIV_PERMA_LOCK,
} rfid_privilege_e;

typedef enum {
    RFID_LOCK_DATA_FIELD_KILL_PWD = 0,
    RFID_LOCK_DATA_FIELD_ACCESS_PWD,
    RFID_LOCK_DATA_FIELD_EPC_MEM,
    RFID_LOCK_DATA_FIELD_TID_MEM,
    RFID_LOCK_DATA_FIELD_USER_MEM
} rfid_lock_data_field_e;

typedef enum {
    RFID_TID_HIDE_NONE = 0,
    RFID_TID_HIDE_SOME,
    RFID_TID_HIDE
} rfid_tid_e;

typedef enum {
    RFID_RANGE_NORMAL = 0,
    RFID_RANGE_TEMP_TOGGLE,
    RFID_RANGE_REDUCED
} rfid_range_e;

typedef enum {
    RFID_OPCODE_NONE = 0,
    RFID_OPCODE_READ,
    RFID_OPCODE_WRITE,
    RFID_OPCODE_KILL,
    RFID_OPCODE_LOCK,
    RFID_OPCODE_BLOCK_ERASE,
    RFID_OPCODE_BLOCK_WRITE,
    RFID_OPCODE_BLOCK_PERMA_LOCK,
    RFID_OPCODE_GET_BLOCK_PERMA_LOCK,
    RFID_OPCODE_UNTRACEABLE,
    RFID_OPCODE_AUTHENTICATE,
    RFID_OPCODE_READ_BUFFER
} rfid_opcode_e;

typedef enum {
    RFID_ACCESS_OTHER_ERROR = 0,
    RFID_ACCESS_NOT_SUPPORTED,
    RFID_ACCESS_INSUFFICIENT_PRIVILEGE,
    RFID_ACCESS_TAG_MEMORY_OVERRUN,
    RFID_ACCESS_TAG_MEMORY_LOCKED,
    RFID_ACCESS_CRYPTO_SUITE_ERROR,
    RFID_ACCESS_COMMAND_NOT_ENCAPSULATED,
    RFID_ACCESS_RESPONSE_BUFFER_OVERFLOW,
    RFID_ACCESS_SECURITY_TIMEOUT,
    RFID_ACCESS_INSUFFICIENT_POWER,
    RFID_ACCESS_NON_SPECIFIC_TAG_ERROR,
    RFID_ACCESS_NON_SPECIFIC_READER_ERROR,
    RFID_ACCESS_NO_RESPONSE_FROM_TAG,
    RFID_ACCESS_INCORRECT_PASSWORD_ERROR,
    RFID_ACCESS_ZERO_KILL_PASSWORD_ERROR,
    RFID_ACCESS_IN_PROCESS_ERROR_STORED_NO_LENGTH,
    RFID_ACCESS_IN_PROCESS_ERROR_STORED_WITH_LENGTH,
    RFID_ACCESS_IN_PROCESS_ERROR_SEND_NO_LENGTH,
    RFID_ACCESS_IN_PROCESS_ERROR_SEND_WITH_LENGTH,
    RFID_ACCESS_IN_PROCESS_STILL_WORKING,
    RFID_ACCESS_IN_PROCESS_SUCCESS_STORED_NO_LENGTH,
    RFID_ACCESS_IN_PROCESS_SUCCESS_STORED_WITH_LENGTH,
    RFID_ACCESS_IN_PROCESS_SUCCESS_SEND_NO_LENGTH,
    RFID_ACCESS_IN_PROCESS_SUCCESS_SEND_WITH_LENGTH,
    RFID_ACCESS_SUCCESS
} rfid_access_result_e;

typedef enum {
    RFID_SCAN_RESULT_COMPLETE_WITH_RESULT = 0,
    RFID_SCAN_RESULT_COMPLETE_NO_RESULT,
    RFID_SCAN_RESULT_COMPLETE_PARTIAL
} rfid_scan_result_e;

typedef enum {
    RFID_SCAN_STATUS_STARTED = 0,
    RFID_SCAN_STATUS_FAILED,
    RFID_SCAN_STATUS_STOPPED_HLOS,
    RFID_SCAN_STATUS_STOPPED_TIMEOUT,
    RFID_SCAN_STATUS_STOPPED_TAG_OBSERVATION,
    RFID_SCAN_STATUS_STOPPED_PROCEDURE_DONE,
    RFID_SCAN_STATUS_STOPPED_DEACTIVATED,
    RFID_SCAN_STATUS_PAUSED_INVALID,
    RFID_SCAN_STATUS_PAUSED_LTA,
    RFID_SCAN_STATUS_PAUSED_INACTIVITY,
    RFID_SCAN_STATUS_PAUSED_VOWIFI,
    RFID_SCAN_STATUS_RESUMED,
} rfid_scan_status_e;

typedef enum {
    RFID_CONFIG_STATUS_SUCCESS = 0,
    RFID_CONFIG_STATUS_FAILURE,
} rfid_config_status_e;

typedef enum {
    RFID_SERVICE_ACTIVATED = 0,
    RFID_SERVICE_DEACTIVATED_HLOS,
    RFID_SERVICE_DEACTIVATED_INACTIVITY,
    RFID_SERVICE_DEACTIVATED_EMERGENCY_CALL,
    RFID_SERVICE_DEACTIVATED_LPM,
    RFID_SERVICE_DEACTIVATED_THERMAL_EMERG,
    RFID_SERVICE_DEACTIVATED_MCC_MISMATCH,
    RFID_SERVICE_DEACTIVATED_VOWIFI,
    RFID_SERVICE_DEACTIVATED_OTHERS,
} rfid_service_status_e;

typedef enum {
    RFID_MODE_RFID_WWAN = 0,
    RFID_MODE_RFID,
} rfid_mode_type_e;

typedef enum {
    RFID_SEL_SL_ALL = 0,
    RFID_SEL_SL_DESELECT,
    RFID_SEL_SL_SELECT,
} rfid_sel_sl_e;

typedef enum {
    RFID_MODE_STATUS_SUCCESS = 0,
    RFID_MODE_STATUS_FAILURE,
} rfid_mode_status_e;

typedef enum {
    RFID_ACTION_SCAN = 0,
    RFID_ACTION_SEARCH,
} rfid_action_type_e;

typedef enum {
    RFID_TAG_RESULT = 0,
    RFID_READ_RESULT,
    RFID_WRITE_RESULT,
    RFID_AUTH_RESULT,
    RFID_OTHER_RESULT,
    RFID_SCAN_RESULT,
    RFID_STATUS_RESULT,
} rfid_result_type_e;

typedef enum {
    RFID_ACCESS_CMD_READ = 0,
    RFID_ACCESS_CMD_WRITE,
    RFID_ACCESS_CMD_AUTH,
    RFID_ACCESS_CMD_OTHER,
} rfid_access_cmd_type_e;

/*---------------------------------------------------------------------------
                           STRUCTURES
---------------------------------------------------------------------------*/
typedef struct {
    bool        support_block_erase;
    bool        support_block_write;
    bool        support_block_perma_lock;
    bool        support_umi_method2;
    bool        support_xpc;
    bool        support_untraceable;
    bool        support_challenge;
    bool        support_authenticate;
    bool        support_aut_comm;
    bool        support_secure_comm;
    bool        support_key_update;
    bool        support_tag_privilege;
    bool        support_read_buffer;
    bool        support_extended_pwr_on;
    bool        support_tag_inv_state_aware;
    uint32_t    max_num_select_filters;
    uint32_t    max_num_access_seq;
    bool        support_nxp_cmd;
} rfid_air_proto_cap_s;

typedef struct {
    rfid_com_standard_e     com_standard;
    bool                    hopping_enabled;
    uint32_t                freq_size; //same for start_freq, step_freq, num_freq
    uint32_t                start_freq[RFID_MAX_FREQ];
    uint32_t                step_freq[RFID_MAX_FREQ];
    uint32_t                num_freq[RFID_MAX_FREQ];
    uint32_t                supported_rf_mode_tbl_mask;
    int                     max_allowed_trans_pwr;
} rfid_comm_std_based_cap_s;

typedef struct {
    uint32_t            rf_mode_identifier;
    rfid_dr_e           dr;
    uint32_t            bdr;
    rfid_symbol_rep_e   m;
    rfid_flm_e          fwd_link_mod;
    uint32_t            pie;
    uint32_t            min_tari;
    uint32_t            max_tari;
    uint32_t            step_tari;
    rfid_smi_e          spectral_mask;
    bool                certified;
} rfid_uhf_rf_mode_s;

typedef struct {
    int                         transmit_min_power;
    uint32_t                    transmit_power_step;
    int                         transmit_max_power;
    rfid_comm_std_based_cap_s   comm_std_table[RFID_MAX_COMM_STD_BASED_TABLE]; // Support 16 but max of 3 for now. No frag required.
    uint32_t                    comm_std_table_size;
    rfid_uhf_rf_mode_s          uhf_rf_table[RFID_MAX_UHF_RF_TABLE]; // Support 32 but max 10 for now. No frag required.
    uint32_t                    uhf_rf_table_size;
} rfid_regulation_cap_s;

typedef struct {
    uint32_t            mcc;
    uint32_t            comm_std_tbl_index;
    rfid_antenna_id_e   antenna_id;
    int                 tx_power;
    uint32_t            hop_table_index;
    uint32_t            channel_index[RFID_CHANNEL_INDEX_MAX_LENGTH];
    uint8_t             channel_index_size;
    uint32_t            rf_mode_index;
    uint32_t            tari;
} rfid_rf_config_s;

typedef struct {
    rfid_memory_bank_e      mb;
    uint32_t                msb_ptr;
    uint32_t                tag_mask_bit_count;
    uint32_t                tag_mask[RFID_MAX_COMMON_TAG_MASK_SIZE]; // to_compare?
    int                     tag_mask_size; // All size variables should be same type
    rfid_truncate_e         truncate;
    rfid_unaware_action_e   state_unaware_action;
    rfid_select_target_e    target;
    rfid_select_action_e    action;
} rfid_common_config_filter_s;

typedef struct {
    rfid_session_e  session;
    uint32_t        tag_population;
    uint32_t        tag_transit_time;
    rfid_target_e   target_query;
    rfid_sel_sl_e   sel;
} rfid_common_config_singulation_s;

typedef struct {
    rfid_trigger_type_e              start_trigger;
    uint32_t                         time;
    uint32_t                         offset;
    uint32_t                         period;
    uint32_t                         on_duration;
    rfid_trigger_type_e              stop_trigger;
    uint32_t                         timeout;
    rfid_tag_oberservation_trigger_e type;
    uint32_t                         num_tags;
    uint32_t                         num_attempts;
    uint32_t                         idle_time;
} rfid_common_config_trigger_s;

typedef struct {
    rfid_report_trigger_e        report_trigger;
    uint32_t                     num_trigger_event;
    bool                         enable_peak_rssi;
    bool                         enable_first_seen_timestamp;
    bool                         enable_last_seen_timestamp;
    bool                         enable_tag_seen_count;
    bool                         enable_pc;
    bool                         enable_crc;
    bool                         enable_xpc;
} rfid_common_config_report_s;

typedef struct {
    rfid_antenna_id_e                   antenna_id;
    bool                                tag_inventory_state_aware;
    rfid_common_config_filter_s         filter[RFID_MAX_COMMON_FILTER];
    uint32_t                            filter_size;
    rfid_common_config_singulation_s    singulation;
    rfid_common_config_trigger_s        trigger;
    rfid_common_config_report_s         report;
} rfid_common_config_s;

typedef struct {
    rfid_memory_bank_e      mb;
    uint32_t                msb_ptr;
    uint32_t                tag_mask_bit_count;
    uint32_t                tag_mask[RFID_MAX_TAG_MASK_SIZE];
    int                     tag_mask_size;
    uint32_t                tag_data[RFID_MAX_TAG_DATA_SIZE];
    int                     tag_data_size;
    bool                    match;
} rfid_access_filter_s;

typedef struct {
    rfid_opcode_e          opcode;
    rfid_memory_bank_e     mb;
    uint32_t               word_ptr;
    uint32_t               data_count;
    uint32_t               access_password;
} rfid_read_access_config_s;

typedef struct {
    rfid_opcode_e          opcode;
    rfid_memory_bank_e     mb;
    uint32_t               word_ptr;
    uint32_t               word_count;
    uint32_t               access_password;
    uint32_t               write_data[RFID_MAX_WRITE_DATA_SIZE];
    int                    write_data_size;
} rfid_write_access_config_s;

typedef struct {
    bool                   send; // If false store. rename
    bool                   include_length;
    uint32_t               crypto_suite;
    uint32_t               crypto_message_bit_count;
    uint32_t               crypto_message[RFID_MAX_CRYPTO_MESSAGE];
    int                    crypto_message_size;
    uint32_t               resp_len; // resp_len?
} rfid_auth_access_config_s;

typedef struct {
    rfid_privilege_e       privilege;
    rfid_lock_data_field_e data_field;
} rfid_access_lock_param_s;

typedef struct {
    bool                   ubit;
    uint32_t               epc;
    rfid_tid_e             tid;
    bool                   hide_user;
    rfid_range_e           range;
} rfid_untraceable_param_s;

typedef struct {
    rfid_opcode_e               opcode;
    rfid_memory_bank_e          mb;
    int                         word_ptr;
    uint32_t                    word_count;
    uint32_t                    access_password;
    uint32_t                    kill_password;
    rfid_access_lock_param_s    lock_param[RFID_MAX_LOCK_PARAM_SIZE];
    int                         lock_param_size;
    uint32_t                    block_mask_bit_count;
    uint32_t                    block_mask[RFID_MAX_BLOCK_MASK];
    int                         block_mask_size;
    uint32_t                    block_range;
    rfid_untraceable_param_s    untraceable_param;
} rfid_other_access_config_s;

typedef struct {
    rfid_access_cmd_type_e type;
    union {
        rfid_read_access_config_s       read;
        rfid_write_access_config_s      write;
        rfid_auth_access_config_s       auth;
        rfid_other_access_config_s      other;
    } cmd;
} rfid_access_cmd_s;

typedef struct {
    bool                    is_multi_tag;
    rfid_antenna_id_e       antenna_id;                 // Should not need valid flag for enum
    uint32_t                filter_size;
    rfid_access_filter_s    filter[RFID_MAX_ACCESS_FILTERS];
} rfid_set_access_param_s;

typedef struct {
    rfid_set_access_param_s     access_params;
    uint32_t                    cmd_size;
    rfid_access_cmd_s           cmd[RFID_MAX_ACCESS_CMDS];
} rfid_access_config_s;

typedef struct {
    uint32_t    epc_data_bit_count;
    uint32_t    epc_data[RFID_MAX_EPC_DATA_SIZE];
    int         epc_data_size;
    bool        peak_rssi_valid;
    int         peak_rssi;
    int         phase;
    uint32_t    channel_idx;
    uint64_t    first_seen_timestamp; // May change storage type
    uint64_t    last_seen_timestamp; // May change storage type
    uint32_t    tag_seen_count;
    bool        pc_valid;
    uint32_t    pc;
    bool        xpc_1_valid;
    uint32_t    xpc_w1;
    bool        xpc_2_valid;
    uint32_t    xpc_w2;
    bool        crc_valid;
    uint32_t    crc;
} rfid_tag_info_s;

typedef struct {
    rfid_scan_result_e      access_result;
    rfid_tag_info_s         tag_info;
    uint32_t                num_access_result;
} rfid_tag_access_result_ind_s;

typedef struct {
    rfid_opcode_e           opcode;
    rfid_access_result_e    access_result;
    uint32_t                read_data_bit_count;
    uint32_t                read_data[RFID_MAX_READ_DATA_SIZE];
    int                     read_data_size;
} rfid_read_access_result_ind_s;

typedef struct {
    rfid_opcode_e           opcode;
    rfid_access_result_e    access_result;
    uint32_t                num_words_written;
} rfid_write_access_result_ind_s;

typedef struct {
    rfid_access_result_e    access_result;
    uint32_t                auth_resp_bit_count;
    uint32_t                auth_resp[RFID_MAX_AUTH_RESP_SIZE];
    int                     auth_resp_size;
} rfid_auth_access_result_ind_s;

typedef struct {
    rfid_opcode_e           opcode;
    rfid_access_result_e    access_result;
    uint32_t                perma_lock_status_bit_count;
    uint32_t                perma_lock_status[RFID_MAX_PERMA_LOCK_SIZE];
    int                     perma_lock_status_size;
} rfid_other_access_result_ind_s;

typedef struct {
    rfid_scan_result_e      status;
    uint32_t                tag_info_size;
    rfid_tag_info_s         tag_info[RFID_MAX_INVENTORY_SCAN_TAG_SIZE];
} rfid_inventory_scan_result_ind_s;

typedef struct {
    rfid_scan_status_e      status;
    // Used for both scan and access. Provides updates for start/stop/pause/resume.
} rfid_status_ind_s;

typedef struct {
    rfid_result_type_e type;
    union {
        rfid_status_ind_s                   status;
        rfid_tag_access_result_ind_s        tag;
        rfid_read_access_result_ind_s       read;
        rfid_write_access_result_ind_s      write;
        rfid_auth_access_result_ind_s       auth;
        rfid_other_access_result_ind_s      other;
        rfid_inventory_scan_result_ind_s    scan;
    } result;
} rfid_scan_result_ind_s;

/*---------------------------------------------------------------------------
                           CALLBACKS/INDICATIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION:  rfid_cap_cb
===========================================================================*/
/** @ingroup rfid_cap_cb

    Callback function to send the rfid capabilities to client. Capabilities are static on modem
    and should never be updated.

    @param[in] txnId, transaction ID used to correlate this callback to the request.
    @param[in] air_cap, supported air capabilities by modem.
    @param[in] reg_cap, supported regulation capabilities by modem.

    @return none

    @dependencies This API will be triggered in response to qms_rfid_get_cap().
*/
/*=========================================================================*/
typedef void (*rfid_cap_cb)(int txnId, rfid_air_proto_cap_s air_cap, rfid_regulation_cap_s reg_cap);


/*===========================================================================
  FUNCTION:  rfid_service_ind
===========================================================================*/
/** @ingroup rfid_service_ind

    Indication function to send the service stack status to client. Deactiviate status
    will always be sent back regardless if the deactivate request was from modem or user.

    @param[in] status, new service stack status.

    @return none

    @dependencies Only called after indication is registered by qms_rfid_activate(). This API will
    be triggered when there is a change the service stack status.
*/
/*=========================================================================*/
typedef void (*rfid_service_ind)(rfid_service_status_e status);

/*===========================================================================
  FUNCTION:  rfid_set_config_cb
===========================================================================*/
/** @ingroup rfid_set_config_cb

    Callback providing results of rf and common configuration after being set by
    qms_rfid_set_config().

    @param[in] txnId, transaction ID used to correlate this callback to the request.
    @param[in] rf, status of RF config.
    @param[in] common, status of common config.

    @return none

    @dependencies This api will be triggered in response to qms_rfid_set_config() after applying
                  the provide configurations.
*/
/*=========================================================================*/
typedef void (*rfid_set_config_cb)(int txnId, rfid_config_status_e rf, rfid_config_status_e common);

/*===========================================================================
  FUNCTION:  rfid_scan_result_ind
===========================================================================*/
/** @ingroup rfid_scan_result_ind

    Indication function with updates on the latest scan result. Before the first scan
    SCAN_STATUS_STARTED will be returned in scan_result_s. After the last scan similarly
    SCAN_STATUS_STOPPED will be returned.

    @param[in] scan, information about the current scan, error in scanning, or if scanning has
                     been paused or resumed.

    @return none

    @dependencies This API will be triggered periodically while a scan is ongoing.
*/
/*=========================================================================*/
typedef void (*rfid_scan_result_ind)(rfid_scan_result_ind_s scan);

/*===========================================================================
  FUNCTION:  rfid_set_mode_cb
===========================================================================*/
/** @ingroup rfid_set_mode_cb

    Callback in response to qms_rfid_set_mode() to indicate the result of setting the new mode.

    @param[in] txnId, transaction ID used to correlate this callback to the request.
    @param[in] status, status indicating the success or failure of attempting to update the status.

    @return none

    @dependencies This API will be triggered in response to qms_rfid_set_mode().
*/
/*=========================================================================*/
typedef void (*rfid_set_mode_cb)(int txnId, rfid_mode_status_e status);



/*---------------------------------------------------------------------------
                           FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION:  qms_rfid_enable
===========================================================================*/
/** @ingroup qms_rfid_enable

    Initializes the RFID service with QMS and registers the service status indication function.
    This function is required to be called before any other APIs in RFID.

    @param[in] ind_fn, indication function over which service status updates will be sent.

    @return status_e::SUCCESS or error.

    @dependencies none
*/
/*=========================================================================*/
extern int qms_rfid_enable(service_status_ind ind_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_disable
===========================================================================*/
/** @ingroup qms_rfid_disable

    Cleans up the connection to QMS and disables RFID service. After this API is called
    no other APIs in RFID can be called until qms_rfid_enable() is called again.

    @param[in] none

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable() first.
*/
/*=========================================================================*/
extern int qms_rfid_disable();

/*===========================================================================
  FUNCTION:  qms_rfid_get_cap
===========================================================================*/
/** @ingroup qms_rfid_get_cap

    Requests for specific capability information. Requested capability types will be returned
    in the provided callback function.

    @param[in] rfid_cap_cb, callback to provide the capabilities.

    @return transaction ID or error from status_e.

    @dependencies RFID service must be initialized via qms_rfid_enable() first. Expecation is that
                  this API is only called after license valid is indicated via service_status_ind.
*/
/*=========================================================================*/
extern int qms_rfid_get_cap(rfid_cap_cb cb_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_activate
===========================================================================*/
/** @ingroup qms_rfid_activate

    Requests to activate the RFID service. The service stack status will be notified in the
    provided indication callback function.

    @param[in] ind_fn, indication function that will provide updates on when the service stack
                       moves between ACTIVATED and DEACTIVATED.

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable() first.
*/
/*=========================================================================*/
extern int qms_rfid_activate(rfid_service_ind ind_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_deactivate
===========================================================================*/
/** @ingroup qms_rfid_deactivate

    Deactiavtes the RFID service if already activated, otherwise is a no-op.

    @param[in] none

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable() and previously activated
                  via qms_rfid_activate().
*/
/*=========================================================================*/
extern int qms_rfid_deactivate();

/*===========================================================================
  FUNCTION:  qms_rfid_set_config
===========================================================================*/
/** @ingroup qms_rfid_set_config

    Requests for specific capability information. Requested capability types will be returned
    in the provided callback function.

    @param[in] rfid_rf_config_s,     RF configuration, may be null if user only wants to update common
                                configuration.
    @param[in] rfid_common_config_s, Common configuration, may be null if user only wants to update rf
                                configuration.
    @param[in] rfid_set_config_cb,   callback to indicate success or failure of updating provided
                                configurations.

    @return transaction ID or error from status_e.

    @dependencies RFID service must be initialized via qms_rfid_enable().
*/
/*=========================================================================*/
extern int qms_rfid_set_config(rfid_rf_config_s rf, rfid_common_config_s common, rfid_set_config_cb cb_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_inventory_start
===========================================================================*/
/** @ingroup qms_rfid_inventory_start

    Starts the specified inventory action on the provided antenna. Results will be passed back
    via the provided scan indication function.

    @param[in] action, enum corresponding to the action that inventory start should take.
    @param[in] antenna_id, antenna on which the inventory start should run on.
    @param[in] ind_fn, indication function which will provide results from inventory start.

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable().
*/
/*=========================================================================*/
extern int qms_rfid_inventory_start(rfid_action_type_e action, rfid_antenna_id_e antenna_id, rfid_scan_result_ind ind_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_inventory_stop
===========================================================================*/
/** @ingroup qms_rfid_inventory_stop

    Stops ths inventory action previously started via qms_rfid_inventory_start().

    @param[in] none

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable(). Can only be called after
                  qms_rfid_inventory_start().
*/
/*=========================================================================*/
extern int qms_rfid_inventory_stop();

/*===========================================================================
  FUNCTION:  qms_rfid_access_start
===========================================================================*/
/** @ingroup qms_rfid_access_start

    Starts the access scan based on the provided configuration. Activate must be called before.
    Indications will provide continuous updates of the scan results

    @param[in] config, configuration information for the requested access command.
    @param[in] ind_fn, Indication function which will provide results from access start.

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable().
*/
/*=========================================================================*/
extern int qms_rfid_access_start(rfid_access_config_s config, rfid_scan_result_ind ind_fn);

/*===========================================================================
  FUNCTION:  qms_rfid_access_stop
===========================================================================*/
/** @ingroup qms_rfid_access_stop

    Stops any previously called qms_rfid_acess_start().

    @param[in] none

    @return status_e::SUCCESS or error.

    @dependencies RFID service must be initialized via qms_rfid_enable(). Can only be called after
                  qms_rfid_access_start().
*/
/*=========================================================================*/
extern int qms_rfid_access_stop();

/*===========================================================================
  FUNCTION:  qms_rfid_set_mode
===========================================================================*/
/** @ingroup qms_rfid_set_mode

    Requests for the new mode to be set on the RFID service.

    @param[in] mode, new mode to set on the RFID service.
    @param[in] uninterupted_rfid, Boolean indicating if uninterrupted RFID mode should be
                                  enabled or not. Only applicable when mode is RFID+WWAN
    @param[in] cb_fn, callback to indicate the result of this request.

    @return transaction ID or error from status_e.

    @dependencies RFID service must be initialized via qms_rfid_enable().
*/
/*=========================================================================*/
extern int qms_rfid_set_mode(rfid_mode_type_e mode, bool uninterupted_rfid, rfid_set_mode_cb cb_fn);

#ifdef __cplusplus
} // Extern "C"
#endif