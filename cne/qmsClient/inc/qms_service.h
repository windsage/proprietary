/**
  @file
  qms_service.h

  @brief
  This file provides an interface to interact with underlying qms service plane

*/

/*===========================================================================

  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#ifndef QMS_SERVICE_H
#define QMS_SERVICE_H

/*---------------------------------------------------------------------------
                           DECLARATIONS
---------------------------------------------------------------------------*/
typedef enum
{
  BC_0 = 0,
  BC_1 = 1,
  BC_3 = 3,
  BC_4 = 4,
  BC_5 = 5,
  BC_6 = 6,
  BC_7 = 7,
  BC_8 = 8,
  BC_9 = 9,
  BC_10 = 10,
  BC_11 = 11,
  BC_12 = 12,
  BC_13 = 13,
  BC_14 = 14,
  BC_15 = 15,
  BC_16 = 16,
  BC_17 = 17,
  BC_18 = 18,
  BC_19 = 19,
  GSM_450 = 40,
  GSM_480 = 41,
  GSM_750 = 42,
  GSM_850 = 43,
  GSM_900_EXTENDED = 44,
  GSM_900_PRIMARY  = 45,
  GSM_900_RAILWAYS = 46,
  GSM_1800 = 47,
  GSM_1900 = 48,
  WCDMA_2100     = 80,
  WCDMA_PCS_1900 = 81,
  WCDMA_DCS_1800 = 82,
  WCDMA_1700_US  = 83,
  WCDMA_850      = 84,
  WCDMA_800      = 85,
  WCDMA_2600     = 86,
  WCDMA_900      = 87,
  WCDMA_1700_JAPAN = 88,
  WCDMA_1500_JAPAN = 90,
  WCDMA_850_JAPAN          = 91,
  E_UTRA_OPERATING_BAND_1  = 120,
  E_UTRA_OPERATING_BAND_2  = 121,
  E_UTRA_OPERATING_BAND_3  = 122,
  E_UTRA_OPERATING_BAND_4  = 123,
  E_UTRA_OPERATING_BAND_5  = 124,
  E_UTRA_OPERATING_BAND_6  = 125,
  E_UTRA_OPERATING_BAND_7  = 126,
  E_UTRA_OPERATING_BAND_8  = 127,
  E_UTRA_OPERATING_BAND_9  = 128,
  E_UTRA_OPERATING_BAND_10 = 129,
  E_UTRA_OPERATING_BAND_11 = 130,
  E_UTRA_OPERATING_BAND_12 = 131,
  E_UTRA_OPERATING_BAND_13 = 132,
  E_UTRA_OPERATING_BAND_14 = 133,
  E_UTRA_OPERATING_BAND_17 = 134,
  E_UTRA_OPERATING_BAND_33 = 135,
  E_UTRA_OPERATING_BAND_34 = 136,
  E_UTRA_OPERATING_BAND_35 = 137,
  E_UTRA_OPERATING_BAND_36 = 138,
  E_UTRA_OPERATING_BAND_37 = 139,
  E_UTRA_OPERATING_BAND_38 = 140,
  E_UTRA_OPERATING_BAND_39 = 141,
  E_UTRA_OPERATING_BAND_40 = 142,
  E_UTRA_OPERATING_BAND_18 = 143,
  E_UTRA_OPERATING_BAND_19 = 144,
  E_UTRA_OPERATING_BAND_20 = 145,
  E_UTRA_OPERATING_BAND_21 = 146,
  E_UTRA_OPERATING_BAND_24 = 147,
  E_UTRA_OPERATING_BAND_25 = 148,
  E_UTRA_OPERATING_BAND_41 = 149,
  E_UTRA_OPERATING_BAND_42 = 150,
  E_UTRA_OPERATING_BAND_43 = 151,
  E_UTRA_OPERATING_BAND_23 = 152,
  E_UTRA_OPERATING_BAND_26 = 153,
  E_UTRA_OPERATING_BAND_32 = 154,
  E_UTRA_OPERATING_BAND_125 = 155,
  E_UTRA_OPERATING_BAND_126 = 156,
  E_UTRA_OPERATING_BAND_127 = 157,
  E_UTRA_OPERATING_BAND_28 = 158,
  E_UTRA_OPERATING_BAND_29 = 159,
  E_UTRA_OPERATING_BAND_30 = 160,
  E_UTRA_OPERATING_BAND_66 = 161,
  E_UTRA_OPERATING_BAND_250 = 162,
  E_UTRA_OPERATING_BAND_46 = 163,
  E_UTRA_OPERATING_BAND_27 = 164,
  E_UTRA_OPERATING_BAND_31 = 165,
  E_UTRA_OPERATING_BAND_71 = 166,
  E_UTRA_OPERATING_BAND_47 = 167,
  E_UTRA_OPERATING_BAND_48 = 168,
  E_UTRA_OPERATING_BAND_67 = 169,
  E_UTRA_OPERATING_BAND_68 = 170,
  E_UTRA_OPERATING_BAND_49 = 171,
  E_UTRA_OPERATING_BAND_85 = 172,
  E_UTRA_OPERATING_BAND_72 = 173,
  E_UTRA_OPERATING_BAND_73 = 174,
  E_UTRA_OPERATING_BAND_86 = 175,
  E_UTRA_OPERATING_BAND_53 = 176,
  E_UTRA_OPERATING_BAND_87 = 177,
  E_UTRA_OPERATING_BAND_88 = 178,
  E_UTRA_OPERATING_BAND_70 = 179,
  TDSCDMA_BAND_A = 200,
  TDSCDMA_BAND_B = 201,
  TDSCDMA_BAND_C = 202,
  TDSCDMA_BAND_D = 203,
  TDSCDMA_BAND_E = 204,
  TDSCDMA_BAND_F = 205,
  NR5G_BAND_1 = 250,
  NR5G_BAND_2 = 251,
  NR5G_BAND_3 = 252,
  NR5G_BAND_5 = 253,
  NR5G_BAND_7 = 254,
  NR5G_BAND_8 = 255,
  NR5G_BAND_20 = 256,
  NR5G_BAND_28 = 257,
  NR5G_BAND_38 = 258,
  NR5G_BAND_41 = 259,
  NR5G_BAND_50 = 260,
  NR5G_BAND_51 = 261,
  NR5G_BAND_66 = 262,
  NR5G_BAND_70 = 263,
  NR5G_BAND_71 = 264,
  NR5G_BAND_74 = 265,
  NR5G_BAND_75 = 266,
  NR5G_BAND_76 = 267,
  NR5G_BAND_77 = 268,
  NR5G_BAND_78 = 269,
  NR5G_BAND_79 = 270,
  NR5G_BAND_80 = 271,
  NR5G_BAND_81 = 272,
  NR5G_BAND_82 = 273,
  NR5G_BAND_83 = 274,
  NR5G_BAND_84 = 275,
  NR5G_BAND_85 = 276,
  NR5G_BAND_257 = 277,
  NR5G_BAND_258 = 278,
  NR5G_BAND_259 = 279,
  NR5G_BAND_260 = 280,
  NR5G_BAND_261 = 281,
  NR5G_BAND_12 = 282,
  NR5G_BAND_25 = 283,
  NR5G_BAND_34 = 284,
  NR5G_BAND_39 = 285,
  NR5G_BAND_40 = 286,
  NR5G_BAND_65 = 287,
  NR5G_BAND_86 = 288,
  NR5G_BAND_48 = 289,
  NR5G_BAND_14 = 290,
  NR5G_BAND_13 = 291,
  NR5G_BAND_18 = 292,
  NR5G_BAND_26 = 293,
  NR5G_BAND_30 = 294,
  NR5G_BAND_29 = 295,
  NR5G_BAND_53 = 296,
  NR5G_BAND_46 = 297,
  NR5G_BAND_91 = 298,
  NR5G_BAND_92 = 299,
  NR5G_BAND_93 = 300,
  NR5G_BAND_94 = 301
} active_band_e;

typedef enum
{
  UPLINK = 0,
  DOWNLINK = 1
} traffic_direction_e;

typedef enum
{
  /* Invalid Status */
  INVALID = -1,
  /* No error in the request */
  SUCCESS = 0,
  /* Invalid Parameter[s] */
  INVALID_ARG = 1,
  /* Unexpected error occurred during processing */
  INTERNAL_ERR = 2,
  /* Unsupported command */
  NOT_SUPPORTED = 3,
  /* Request is issued on unexpected nDDS */
  INVALID_OPERATION = 4,
  /* Response to client register when the service is up */
  SERVICE_UP = 5,
  /* Response to client register when the service is down */
  SERVICE_DOWN = 6,
  /* No sim available on which the parameters are configured */
  INVALID_SLOT = 7
} status_e;

typedef enum
{
    SLOT_FIRST = 0,
    SLOT_SECOND = 1,
    SLOT_THIRD = 2
} slot_id_e;

typedef enum
{
  /* NR5G subcarrier spacing 15 KHz */
  NR5G_SCS_15 = 0,
  /* NR5G subcarrier spacing 30 KHz */
  NR5G_SCS_30 = 1,
  /* NR5G subcarrier spacing 60 KHz */
  NR5G_SCS_60 = 2,
  /* NR5G subcarrier spacing 120 KHz */
  NR5G_SCS_120 = 3,
  /* NR5G subcarrier spacing 240 KHz */
  NR5G_SCS_240 = 4
} nr5g_scs_type_e;

typedef enum
{
  /** Unspecified */
  UNSPECIFIED = 0,
  /** Audio conversation */
  CONV_AUDIO = 1,
  /** Video conversation */
  CONV_VIDEO = 2,
  /** Audio streaming */
  STREAMING_AUDIO = 3,
  /** Video streaming */
  STREAMING_VIDEO = 4,
  /** Gaming */
  TYPE_GAMING = 5,
  /** Web browsing */
  WEB_BROWSING = 6,
  /** File transfer */
  FILE_TRANSFER = 7
} application_type_e;

typedef enum
{
  /** Slow scan */
  SCAN_STATE_SLOW = 0,
  /** Normal scan */
  SCAN_STATE_NORMAL = 1,
} scan_state_e;

typedef enum
{
  /** Allow Data */
  DATA_ALLOWED = 1,
  /** Do Not Allow Data */
  DATA_NOT_ALLOWED = 2
}data_recommendation_action_e;


typedef enum
{
  /** DDS Sub */
  DDS = 1,
  /** NDDS Sub */
  NDDS = 2
}data_recommendation_subscription_e;

typedef enum {
    /** Dual SIM Dual Standby submode */
    SUBMODE_INFO_SUBMODE_DSDS = 0,
    /** Dual SIM Dual Active submode */
    SUBMODE_INFO_SUBMODE_DSDA = 1
} submode_info_submode_e;

typedef enum {
    /** DSDA Invalid */
    SUBMODE_INFO_TX_CONCURRENCY_INVALID = -1,
    /** TX sharing */
    SUBMODE_INFO_TX_CONCURRENCY_DSDA_FULL_CONCURRENCY = 0,
    /** Full concurrency */
    SUBMODE_INFO_TX_CONCURRENCY_DSDA_TX_SHARING = 1
} submode_info_tx_concurrency_e;

typedef struct
{
  /**
   * Mandatory parameter.
   * Indicates the MCC value.
   * A 16-bit integer representation of MCC. Range: 0 to 999.
   */
  unsigned short mcc;
  /**
   * Mandatory parameter.
   * Indicates the MNC value.
   * A 16-bit integer representation of MNC. Range: 0 to 999.
   */
  unsigned short mnc;
  /**
   * Mandatory parameter.
   * Indicates the MNC PCS digit include status or not.
   * TRUE if MNC is a three-digit value --> 90 corresponds to an MNC value of 090.
   * FALSE if MNC is a two-digit value --> 90 corresponds to an MNC value of 90.
   */
  bool mnc_includes_pcs_digit;
} plmn_id_s;

typedef struct {
  /**
   * Mandatory parameter.
   * Indicates the PLMN ID.
   */
  plmn_id_s plmn_id;
  /**
   * Mandatory parameter.
   * Indicates the band of serving cell.
   */
  active_band_e active_band;
  /**
   * Mandatory parameter.
   * Indicates the Absolute RF channel number.
   */
  unsigned int arfcn;
  /**
   * Mandatory parameter.
   * Indicates the Physical cell ID.
   */
  unsigned short pci;
  /**
   * Mandatory parameter.
   * Bitmask indicating the dubious cell cause code
   *
   *  Bit 0 --> DUBIOUS_CELL_CAUSE_CEF
   *  Bit 1 --> DUBIOUS_CELL_CAUSE_RLF
   *  Bit 2 --> DUBIOUS_CELL_CAUSE_PING_PONG
   *  Bit 3 --> DUBIOUS_CELL_CAUSE_LOW_DATA_RATE
   *  Bit 4 --> DUBIOUS_CELL_CAUSE_LOW_DATA_RATE_IMS
   *  No bits set --> Cell will be ignored
   *
   *   1 (0001) :  DUBIOUS_CELL_CAUSE_CEF
   *   3 (0011) :  DUBIOUS_CELL_CAUSE_CEF | DUBIOUS_CELL_CAUSE_RLF
   *  14 (1110) :  DUBIOUS_CELL_CAUSE_RLF | DUBIOUS_CELL_CAUSE_PING_PONG | DUBIOUS_CELL_CAUSE_LOW_DATA_RATE
   */
  unsigned int cause_code_bit_mask;
} dubious_cell_info_s;

typedef struct {

  /**
   * Mandatory parameter.
   */
  dubious_cell_info_s cell_info;
  /**
   * Mandatory parameter.
   * Indicates the Global cell ID.
   */
  unsigned int cgi;
} lte_dubious_cell_info_s;

typedef struct {
  /**
   * Mandatory parameter.
   */
  dubious_cell_info_s cell_info;
  /**
   * Mandatory parameter.
   * Indicates the Global cell ID.
   */
  unsigned long long cgi;
  /**
   * Mandatory parameter for NR 5G cells when init is set to TRUE
   * Indicates the Subcarrier spacing
   */
  nr5g_scs_type_e nr_scs_type;
} nr_dubious_cell_info_s;

typedef struct {
  /**
   * Mandatory parameter.
   * Indicates the direction of data stall.
   */
  traffic_direction_e traffic_direction;
  /**
   * Mandatory parameter.
   * Indicates whether data is stalled or not.
   */
  bool data_stall_status;
  /**
   * Optional parameter.
   * Indicates the type of application for which the data stall
   * information is sent.
   * Must be -1 in case if no valid value is being set
   */
  application_type_e app_type;
  /**
   * Optional parameter.
   * Indicates the expected number of downlink IP packets to receive
   * in the specified measurement interval.
   * Must be -1 in case if no valid value is being set
   */
  int expected_ip_packet_count;
  /**
   * Optional parameter.
   * Indicates the expected downlink data rate (in Kbps) to receive
   * in the specified measurement interval.
   * Must be -1 in case if no valid value is being set
   */
  int expected_dataRate;
  /**
   * Optional parameter.
   * Indicates the number of downlink IP packets lost in the specified
   * measurement interval.
   * Must be -1 in case if no valid value is being set
   */
  int lost_ip_packet_count;
  /**
   * Optional parameter.
   * Indicates the measurement interval in milliseconds.
   * Must be -1 in case if no valid value is being set
   */
  int measurement_interval;
} data_stall_params_s;


typedef struct
{
  /**
   * Mandatory parameter.
   * Indicates whether the recommendation action is allowed
   * or not allowed.
   */
  data_recommendation_action_e action;
  /**
   * Mandatory parameter.
   * Indicates the recommendation subscription is on DDS/NDDS.
   */
  data_recommendation_subscription_e subscription;
}data_recommendation_s;

typedef struct {
  /**
   * Mandatory parameter.
   * Indicates the Multi-SIM submode.
   * or not allowed.
   */
  submode_info_submode_e submode;
  /**
   * Optional parameter.
   * Indicates the DSDA Tx Concurrency.
   * Must be SUBMODE_INFO_TX_CONCURRENCY_INVALID if no valid value is being set.
   */
  submode_info_tx_concurrency_e tx_concurrency;
} submode_info_s;

/*===========================================================================
  FUNCTION:  service_status_cb
===========================================================================*/
/** @ingroup service_status_cb

    Callback function to send the service status to the client

    @param[in] status, service status (up / down)

    @return none

    @dependencies
    This API will be triggered when there is a change in service status.
*/
/*=========================================================================*/
typedef void (*service_status_cb)(status_e status);

/*===========================================================================
  FUNCTION:  status_ev_cb
===========================================================================*/
/** @ingroup status_ev_cb

    Callback function prototype to receive the status with transaction id.

    @param[in] transaction_id, id of the transaction
    @param[in] status, error or success status of transaction

    @return none

    @dependencies
    This API will be triggered once the transaction is complete with the service.
*/
/*=========================================================================*/
typedef void (*status_ev_cb)(int transaction_id, status_e status);

/*===========================================================================
  FUNCTION:  dppd_capability_status_ev_cb
===========================================================================*/
/** @ingroup dppd_capability_status_ev_cb

    Callback function prototype for data++data get capability status

    @param[in] transaction_id, id of the transaction
    @param[in] status, error or success status of transaction
    @param[in] capability , data++data capability

    @return none

    @dependencies
    This API will be triggered once the transaction is complete with the service.
*/
/*=========================================================================*/
typedef void (*dppd_capability_status_ev_cb)(int transaction_id,
                 status_e status,bool *capability);

/*===========================================================================
  FUNCTION:  dppd_recommendation_status_ev_cb
===========================================================================*/
/** @ingroup dppd_recommendation_status_ev_cb

    Callback function prototype for data++data get recommendation status

    @param[in] transaction_id, id of the transaction
    @param[in] status, error or success status of transaction
    @param[in] recommendation , data++data recommendation

    @return none

    @dependencies
    This API will be triggered once the transaction is complete with the service.
*/
/*=========================================================================*/
typedef void (*dppd_recommendation_status_ev_cb)(int transaction_id,
                 status_e status,data_recommendation_s *recommendation);

/*===========================================================================
  FUNCTION:  multi_sim_submode_info_status_ev_cb
===========================================================================*/
/** @ingroup multi_sim_submode_info_status_ev_cb

    Callback function prototype for get Multi-SIM submode info

    @param[in] transaction_id, id of the transaction
    @param[in] status, error or success status of transaction
    @param[in] submode_info , Multi-SIM submode and tx concurrency state.

    @return none

    @dependencies
    This API will be triggered once the transaction is complete with the service.
*/
/*=========================================================================*/
typedef void (*multi_sim_submode_info_status_ev_cb)(int transaction_id, status_e status,
                                                    const submode_info_s *submode_info);

/*===========================================================================
  FUNCTION:  dppd_ui_status_ev_cb
===========================================================================*/
/** @ingroup dppd_ui_status_ev_cb

    Callback function prototype for data++data UI update status

    @param[in] transaction_id, id of the transaction
    @param[in] status, error or success status of transaction
    @param[in] result , 0->success or 1->failure

    @return none

    @dependencies
    This API will be triggered once the transaction is complete with the service.
*/
/*=========================================================================*/
typedef void (*dppd_ui_status_ev_cb)(int transaction_id,
                 status_e status,bool *result);

/*===========================================================================
  FUNCTION:  dppd_capability_change_cb
===========================================================================*/
/** @ingroup dppd_capability_change_cb

    Callback function prototype for receiving data++data capability
    change.

    @param[in] capability , 0 -- Not supported
                            1 -- Supported
    @return none

    @dependencies
    This API will be triggered whenever there is a dual data capability change
*/
/*=========================================================================*/
typedef void (*dppd_capability_change_cb)(bool capability);


/*===========================================================================
  FUNCTION:  dppd_recommendation_change_cb
===========================================================================*/
/** @ingroup dppd_recommendation_change_cb

    Callback function prototype for receiving data++data recommendation
    change.

    @param[in] recommendation ,Data subscription recommendation of an application.
    @return none

    @dependencies
    This API will be triggered whenever there is a data subscription recommendation change
*/
/*=========================================================================*/
typedef void (*dppd_recommendation_change_cb)(data_recommendation_s recommendation);

/*===========================================================================
  FUNCTION:  multi_sim_submode_info_change_cb
===========================================================================*/
/** @ingroup multi_sim_submode_info_change_cb

    Callback function prototype for receiving Multi-SIM submode info
    change.

    @param[in] submode_info , Multi-SIM submode and tx concurrency state.
    @return none

    @dependencies
    This API will be triggered whenever there is a submode info change
*/
/*=========================================================================*/
typedef void (*multi_sim_submode_info_change_cb)(const submode_info_s *submode_info);

/*===========================================================================
  FUNCTION:  register_qms_client
===========================================================================*/
/** @ingroup register_qms_client

    Registers the client handle with the service

    @param[in] user_cb_fn, callback function over which response will be received

    @return none

    @dependencies SERVER_UP / DOWN will be received in the status of the
    cb_fn depending on the server / Modem status.
    Client must register the client first before triggering the other APIs.
*/
/*=========================================================================*/
extern void register_qms_client(service_status_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  set_data_stall_parameters
===========================================================================*/
/** @ingroup set_data_stall_parameters

    Configures data stall parameters specific to an application

    @param[in] slot_id, SlotId in which the params need to be configured
    @param[in] params, Data Stall Parameters of an application.
    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    Optional parameters in data_stall_params_t must be set to -1 if a valid
    value is not set.
    cb_fn will be triggered with INVALID_SLOT status if slot_id is invalid.
    cb_fn will be triggered with INVALID_ARG status if any of the parameter
    in params is invalid.
*/
/*=========================================================================*/
extern int set_data_stall_parameters(slot_id_e slot_id, data_stall_params_s *params,
        status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  set_l2nr_selection
===========================================================================*/
/** @ingroup set_l2nr_selection

    Sends the L2NR selection request to Modem.

    @param[in] slot_id, SlotId in which the params need to be configured
    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    cb_fn will be triggered with INVALID_SLOT status if slot_id is invalid.
*/
/*=========================================================================*/
extern int set_l2nr_selection(slot_id_e slot_id, status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  set_lte_dubious_cell_array
===========================================================================*/
/** @ingroup set_lte_dubious_cell_array

    Configures the LTE Dubious Cell List.

    @param[in] slot_id, SlotId in which the params need to be configured
    @param[in] lte_array_size, Size of the Array
    @param[in] lte_dubious_array[], LTE Dubious Array
    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    MAX entries allowed are 32 in array and rest of them will be truncated.
    cb_fn will be triggered with INVALID_SLOT status if slot_id is invalid.
    cb_fn will be triggered with INVALID_ARG status if any of the parameter
    in lte_dubious_array is invalid.
*/
/*=========================================================================*/
extern int set_lte_dubious_cell_array(slot_id_e slot_id, unsigned short lte_array_size,
        lte_dubious_cell_info_s* lte_dubious_array, status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  set_nr_dubious_cell_array
===========================================================================*/
/** @ingroup set_nr_dubious_cell_array

    Configures the NR Dubious Cell List.

    @param[in] slot_id, SlotId in which the params need to be configured
    @param[in] nr_array_size, Size of the Array
    @param[in] nr_dubious_array[], NR Dubious Array
    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    MAX entries allowed are 32 in array and rest of them will be truncated.
    cb_fn will be triggered with INVALID_SLOT status if slot_id is invalid.
    cb_fn will be triggered with INVALID_ARG status if any of the parameter
    in nr_dubious_array is invalid.
*/
/*=========================================================================*/
extern int set_nr_dubious_cell_array(slot_id_e slot_id, unsigned short nr_array_size,
        nr_dubious_cell_info_s* nr_dubious_array, status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  register_dppd_indication
===========================================================================*/
/** @ingroup register_dppd_indication

    Register for data++data indications.

    @param[in] capability_cb, callback function over which data++data
               capability change indication will be received
    @param[in] recommendation_cb, callback function over which data++data
               recommendation change indication will be received

    @return

    @dependencies
    callback functions should be passed to receive capability change or
    recommendation change indications
*/
/*=========================================================================*/
extern void register_dppd_indication(dppd_capability_change_cb capability_cb,
                        dppd_recommendation_change_cb recommendation_cb);

/*===========================================================================
  FUNCTION:  get_dppd_capability
===========================================================================*/
/** @ingroup get_dppd_capability

    Get for data++data capability.

    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    callback function should be passed to receive the data++data capability
*/
/*=========================================================================*/
extern int get_dppd_capability(dppd_capability_status_ev_cb user_cb_fn );

/*===========================================================================
  FUNCTION:  get_dppd_recommendation
===========================================================================*/
/** @ingroup get_dppd_recommendation

    Get for daya++data subscription recommendation.

    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    callback function should be passed to receive the data++data recommendation
*/
/*=========================================================================*/
extern int get_dppd_recommendation(dppd_recommendation_status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  update_dppd_ui_option
===========================================================================*/
/** @ingroup update_dppd_ui_option

    Update data++data UI option.

    @param[in] option,  0: Disable data++data
                        1: Enable data++data

    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    callback function should be passed to receive the ui update status
*/
/*=========================================================================*/
extern int update_dppd_ui_option(bool option,dppd_ui_status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  set_out_of_service_state
===========================================================================*/
/** @ingroup set_out_of_service_state

    Sets the out of service scan state. Clients can set the out of services
    scan state to SLOW or NORMAL to impact modem behavior for power save.

    @param[in] slot_id, SlotId in which the params need to be configured
    @param[in] scan_state, Scan state
    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction
*/
/*=========================================================================*/
extern int set_out_of_service_state(slot_id_e slot_id, scan_state_e scan_state,
                                    status_ev_cb user_cb_fn);

/*===========================================================================
  FUNCTION:  register_multi_sim_submode_indication
===========================================================================*/
/** @ingroup register_multi_sim_submode_indication

    Register for Multi-SIM submode info indications.

    @param[in] submode_info_cb, callback function over which Multi-SIM
               submode info change indication will be received

    @return

    @dependencies
    callback function should be passed to receive Multi-SIM submode info change indications
*/
/*=========================================================================*/
extern void register_multi_sim_submode_indication(multi_sim_submode_info_change_cb submode_info_cb);

/*===========================================================================
  FUNCTION:  get_multi_sim_submode_info
===========================================================================*/
/** @ingroup get_multi_sim_submode_info

    Get for Multi-SIM submode info.

    @param[in] user_cb_fn, callback function over which response will be received

    @return transaction_id, id of the transaction

    @dependencies
    callback function should be passed to receive the Multi-SIM submode info
*/
/*=========================================================================*/
extern int get_multi_sim_submode_info(multi_sim_submode_info_status_ev_cb user_cb_fn);

#endif /* QMS_SERVICE_H */

#ifdef __cplusplus
}
#endif
