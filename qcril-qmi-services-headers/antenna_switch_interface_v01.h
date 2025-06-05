#ifndef ANTSWITCH_SERVICE_01_H
#define ANTSWITCH_SERVICE_01_H
/**
  @file antenna_switch_interface_v01.h

  @brief This is the public header file which defines the antswitch service Data structures.

  This header file defines the types and structures that were defined in
  antswitch. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2010-2013, 2015, 2017-2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.0/antswitch/api/antenna_switch_interface_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9
   It was generated on: Tue Oct  1 2019 (Spin 0)
   From IDL File: antenna_switch_interface_v01.idl */

/** @defgroup antswitch_qmi_consts Constant values defined in the IDL */
/** @defgroup antswitch_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup antswitch_qmi_enums Enumerated types used in QMI messages */
/** @defgroup antswitch_qmi_messages Structures sent as QMI messages */
/** @defgroup antswitch_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup antswitch_qmi_accessor Accessor for QMI service object */
/** @defgroup antswitch_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "system_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup antswitch_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define ANTSWITCH_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define ANTSWITCH_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define ANTSWITCH_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define ANTSWITCH_V01_MAX_MESSAGE_ID 0x0005
/**
    @}
  */


/** @addtogroup antswitch_qmi_consts
    @{
  */

/**  */
#define ANTSWITCH_MAX_MODEM_ACTIVE_TECHS_V01 32
/**
    @}
  */

/** @addtogroup antswitch_qmi_enums
    @{
  */
typedef enum {
  QMI_ANTSWITCH_CONFIG_TYPE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_ANTSWITCH_CONFIG_DISABLE_V01 = 0, /**<  ANT_SWITCH DISABLE \n  */
  QMI_ANTSWITCH_CONFIG_ENABLE_V01 = 1, /**<  ANT_SWITCH ENABLE \n  */
  QMI_ANTSWITCH_CONFIG_0_V01 = 2, /**<  ANT_SWITCH CONFIG_0 \n  */
  QMI_ANTSWITCH_CONFIG_1_V01 = 3, /**<  ANT_SWITCH CONFIG_1 \n  */
  QMI_ANTSWITCH_CONFIG_2_V01 = 4, /**<  ANT_SWITCH CONFIG_2 \n  */
  QMI_ANTSWITCH_CONFIG_3_V01 = 5, /**<  ANT_SWITCH CONFIG_3 \n  */
  QMI_ANTSWITCH_CONFIG_TYPE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_antswitch_config_type_enum_type_v01;
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Request Message; Sets the specified Ant Switch position. */
typedef struct {

  /* Mandatory */
  /*  ANTSWITCH Config */
  qmi_antswitch_config_type_enum_type_v01 ant_switch_config;
  /**<   Antenna Switch Configs State. \n
 Values: \n
      - QMI_ANTSWITCH_CONFIG_DISABLE (0) --  ANT_SWITCH DISABLE \n
      - QMI_ANTSWITCH_CONFIG_ENABLE (1) --  ANT_SWITCH ENABLE \n
      - QMI_ANTSWITCH_CONFIG_0 (2) --  ANT_SWITCH CONFIG_0 \n
      - QMI_ANTSWITCH_CONFIG_1 (3) --  ANT_SWITCH CONFIG_1 \n
      - QMI_ANTSWITCH_CONFIG_2 (4) --  ANT_SWITCH CONFIG_2 \n
      - QMI_ANTSWITCH_CONFIG_3 (5) --  ANT_SWITCH CONFIG_3 \n
 */
}antswitch_set_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Response Message; Sets the specified Ant Switch position. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
     qmi_error_type  - Error code. Possible error code values are described in
                       the error codes section of each message definition.
    */
}antswitch_set_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_enums
    @{
  */
typedef enum {
  ANTSWITCH_WWAN_TECH_TYPE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LTE_TECH_V01 = 0, /**<  LTE  */
  TDSCDMA_TECH_V01 = 1, /**<  TD-SCDMA \n  */
  GSM_TECH_V01 = 2, /**<  GSM (Instance 1)\n  */
  ONEX_TECH_V01 = 3, /**<  CDMA2000\textsuperscript{\textregistered} 1X \n  */
  HDR_TECH_V01 = 4, /**<  HDR \n  */
  WCDMA_TECH_V01 = 5, /**<  WCDMA (Instance 1)\n  */
  NR5G_TECH_V01 = 6, /**<  NR5G \n  */
  GSM2_TECH_V01 = 7, /**<  GSM (Instance 2)\n  */
  GSM3_TECH_V01 = 8, /**<  GSM (Instance 3)\n  */
  WCDMA2_TECH_V01 = 9, /**<  WCDMA (Instance 2)\n  */
  LTE2_TECH_V01 = 10, /**<  LTE (Instance 2)\n  */
  NR5G2_TECH_V01 = 11, /**<  NR5G (Instance 2)  */
  ANTSWITCH_WWAN_TECH_TYPE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}antswitch_wwan_tech_type_enum_type_v01;
/**
    @}
  */

/** @addtogroup antswitch_qmi_enums
    @{
  */
typedef enum {
  ANTSWITCH_WWAN_TECH_CARRIER_ID_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CARRIER_ID_PCELL_V01 = 0, /**<  PCELL Carrier ID  */
  CARRIER_ID_SCELL0_V01 = 1, /**<  SCELL0 Carrier ID  \n  */
  CARRIER_ID_SCELL1_V01 = 2, /**<  SCELL1 Carrier ID  \n  */
  CARRIER_ID_SCELL2_V01 = 3, /**<  SCELL2 Carrier ID  \n  */
  CARRIER_ID_SCELL3_V01 = 4, /**<  SCELL3 Carrier ID  \n  */
  CARRIER_ID_SCELL4_V01 = 5, /**<  SCELL4 Carrier ID  \n  */
  CARRIER_ID_SCELL5_V01 = 6, /**<  SCELL5 Carrier ID  \n  */
  CARRIER_ID_SCELL6_V01 = 7, /**<  SCELL6 Carrier ID  \n  */
  CARRIER_ID_SCELL7_V01 = 8, /**<  SCELL7 Carrier ID  \n  */
  CARRIER_ID_SCELL8_V01 = 9, /**<  SCELL8 Carrier ID  \n  */
  CARRIER_ID_SCELL9_V01 = 10, /**<  SCELL9 Carrier ID  \n  */
  CARRIER_ID_SCELL10_V01 = 11, /**<  SCELL10 Carrier ID  \n  */
  CARRIER_ID_SCELL11_V01 = 12, /**<  SCELL11 Carrier ID  \n  */
  CARRIER_ID_SCELL12_V01 = 13, /**<  SCELL12 Carrier ID  \n  */
  CARRIER_ID_SCELL13_V01 = 14, /**<  SCELL13 Carrier ID  \n  */
  CARRIER_ID_SCELL14_V01 = 15, /**<  SCELL14 Carrier ID  \n  */
  CARRIER_ID_SCELL15_V01 = 16, /**<  SCELL15 Carrier ID  \n  */
  CARRIER_ID_SCELL16_V01 = 17, /**<  SCELL16 Carrier ID  \n  */
  CARRIER_ID_SCELL17_V01 = 18, /**<  SCELL17 Carrier ID  \n  */
  CARRIER_ID_SCELL18_V01 = 19, /**<  SCELL18 Carrier ID  \n  */
  CARRIER_ID_SCELL19_V01 = 20, /**<  SCELL19 Carrier ID  \n  */
  CARRIER_ID_SCELL20_V01 = 21, /**<  SCELL20 Carrier ID  \n  */
  CARRIER_ID_SCELL21_V01 = 22, /**<  SCELL21 Carrier ID  \n  */
  CARRIER_ID_SCELL22_V01 = 23, /**<  SCELL22 Carrier ID  \n  */
  CARRIER_ID_SCELL23_V01 = 24, /**<  SCELL23 Carrier ID  \n  */
  CARRIER_ID_SCELL24_V01 = 25, /**<  SCELL24 Carrier ID  \n  */
  CARRIER_ID_SCELL25_V01 = 26, /**<  SCELL25 Carrier ID  \n  */
  CARRIER_ID_SCELL26_V01 = 27, /**<  SCELL26 Carrier ID  \n  */
  CARRIER_ID_SCELL27_V01 = 28, /**<  SCELL27 Carrier ID  \n  */
  CARRIER_ID_SCELL28_V01 = 29, /**<  SCELL28 Carrier ID  \n  */
  CARRIER_ID_SCELL29_V01 = 30, /**<  SCELL29 Carrier ID  \n  */
  CARRIER_ID_SCELL30_V01 = 31, /**<  SCELL30 Carrier ID  \n  */
  CARRIER_ID_SCELL31_V01 = 32, /**<  SCELL31 Carrier ID   */
  ANTSWITCH_WWAN_TECH_CARRIER_ID_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}antswitch_wwan_tech_carrier_id_enum_type_v01;
/**
    @}
  */

/** @addtogroup antswitch_qmi_aggregates
    @{
  */
typedef struct {

  antswitch_wwan_tech_type_enum_type_v01 tech_id;
  /**<   Active Tech in Modem */

  antswitch_wwan_tech_carrier_id_enum_type_v01 carrier_id;
  /**<   Carrier (PCC/SCC0/SCC1/SCC2/...) Information in Modem */

  qmi_antswitch_config_type_enum_type_v01 config;
  /**<   Antenna Switch Configs State. */

  system_band_enum_v01 band;
  /**<   Band info for this tech */
}antswitch_config_info_per_tech_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup antswitch_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t mask[14];
  /**<   Bit numbers here correspond to the enum in sys_band_class_e_type.
       Band mask to update config:
     - mask[0]   : sys band class 0 to sys band 63.
     - mask[1]   : sys band class 64 to sys band 127.
     - mask[2]   : sys band class 128 to sys band 191.
     - mask[3]   : sys band class 192 to sys band 255.
     - mask[4]   : sys band class 256 to sys band 319.
     - mask[5]   : sys band class 320 to sys band 383.
     - mask[6]   : sys band class 384 to sys band 447.
     - mask[7]   : sys band class 448 to sys band 511.
     - mask[8]   : sys band class 512 to sys band 575.
     - mask[9]   : sys band class 576 to sys band 639.
     - mask[10]  : sys band class 640 to sys band 703.
     - mask[11]  : sys band class 704 to sys band 767.
     - mask[12]  : sys band class 768 to sys band 831.
     - mask[13]  : sys band class 832 to sys band 895
  */
}antswitch_bands_bitmask_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup antswitch_qmi_enums
    @{
  */
typedef enum {
  ANTSWITCH_SET_CONFIG_OPERATION_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_ANTSWITCH_BAND_CONFIG_SET_V01 = 0, /**<  SET: overwrites the entire bitmask and erases the previous setting. \n  */
  QMI_ANTSWITCH_BAND_CONFIG_ADD_V01 = 1, /**<  AND: ORs the bitmask over into the existing bitmask. \n  */
  QMI_ANTSWITCH_BAND_CONFIG_REMOVE_V01 = 2, /**<  REMOVE: removes any set bit from the mask \n  */
  ANTSWITCH_SET_CONFIG_OPERATION_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}antswitch_set_config_operation_type_v01;
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Request Message; Sets the specified Ant Switch position. */
typedef struct {

  /* Mandatory */
  /*  ANTSWITCH Config */
  qmi_antswitch_config_type_enum_type_v01 ant_switch_config;
  /**<   Antenna Switch Configs State. \n
 Values: \n
      - QMI_ANTSWITCH_CONFIG_DISABLE (0) --  ANT_SWITCH DISABLE \n
      - QMI_ANTSWITCH_CONFIG_ENABLE (1) --  ANT_SWITCH ENABLE \n
      - QMI_ANTSWITCH_CONFIG_0 (2) --  ANT_SWITCH CONFIG_0 \n
      - QMI_ANTSWITCH_CONFIG_1 (3) --  ANT_SWITCH CONFIG_1 \n
      - QMI_ANTSWITCH_CONFIG_2 (4) --  ANT_SWITCH CONFIG_2 \n
      - QMI_ANTSWITCH_CONFIG_3 (5) --  ANT_SWITCH CONFIG_3 \n
 */

  /* Mandatory */
  /*  ANTSWITCH Bands bitmask */
  antswitch_bands_bitmask_type_v01 bands_bitmask;
  /**<   Band mask to set config.
  */

  /* Mandatory */
  /*  ANTSWITCH Reset flag */
  antswitch_set_config_operation_type_v01 operation;
  /**<   Flag to reset previously applied config. \n
 Values: \n
      - QMI_ANTSWITCH_BAND_CONFIG_SET (0) --  SET: overwrites the entire bitmask and erases the previous setting. \n
      - QMI_ANTSWITCH_BAND_CONFIG_ADD (1) --  AND: ORs the bitmask over into the existing bitmask. \n
      - QMI_ANTSWITCH_BAND_CONFIG_REMOVE (2) --  REMOVE: removes any set bit from the mask \n
 */
}antswitch_enh_set_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Response Message; Sets the specified Ant Switch position. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
     qmi_error_type  - Error code. Possible error code values are described in
                       the error codes section of each message definition.
    */
}antswitch_enh_set_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Request Message; Enables/Disables the antenna switch config change indication */
typedef struct {

  /* Optional */
  /*  Antenna switch config indication enable flag */
  uint8_t enable_config_change_ind_valid;  /**< Must be set to true if enable_config_change_ind is being passed */
  uint8_t enable_config_change_ind;
  /**<   Flag to enable/disable antenna switch config change indication.\n
      TRUE: Enable the antenna config change indication\n
      FALSE: Disable the config change indication.
  */
}antswitch_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Response Message; Enables/Disables the antenna switch config change indication */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
     qmi_error_type  - Error code. Possible error code values are described in
                       the error codes section of each message definition.
    */
}antswitch_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_enums
    @{
  */
typedef enum {
  ANT_SWITCH_CLIENT_EXCEP_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  TRM_CLIENT_EXP_TYPE_NONE_V01 = 0,
  TRM_CLIENT_EXP_TYPE_EMBMS1_V01 = 1,
  TRM_CLIENT_EXP_TYPE_EMBMS2_V01 = 2,
  TRM_CLIENT_EXP_TYPE_LTED_V01 = 3,
  TRM_CLIENT_EXP_TYPE_LTECGI_V01 = 4,
  TRM_CLIENT_EXP_TYPE_GPRS_V01 = 5,
  TRM_CLIENT_EXP_TYPE_MAX_V01 = 6,
  ANT_SWITCH_CLIENT_EXCEP_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ant_switch_client_excep_type_v01;
/**
    @}
  */

/** @addtogroup antswitch_qmi_aggregates
    @{
  */
typedef struct {

  antswitch_wwan_tech_type_enum_type_v01 tech_id;
  /**<   Active Tech in Modem */

  antswitch_wwan_tech_carrier_id_enum_type_v01 carrier_id;
  /**<   Carrier (PCC/SCC0/SCC1/SCC2/...) Information in Modem */

  ant_switch_client_excep_type_v01 excep_type;
  /**<   exception type for active client */

  system_band_enum_v01 band;
  /**<   Band info for this tech */

  qmi_antswitch_config_type_enum_type_v01 config;
  /**<   Antenna Switch Configs State. */
}ant_switch_config_change_ind_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Indication Message; Notifies the listener about change in ASDIV config */
typedef struct {

  /* Mandatory */
  /*  Antenna Switch config indicating config change */
  ant_switch_config_change_ind_info_type_v01 new_config_info;
  /**<   \n New antenna switch config state info.
  */

  /* Mandatory */
  /*  Antenna Switch config indicating config change */
  ant_switch_config_change_ind_info_type_v01 old_config_info;
  /**<   \n Old antenna switch config state info.
  */
}antswitch_config_change_ind_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Request Message; Gets the specified Antenna Switch State for different active modem techs. */
typedef struct {

  /* Optional */
  /*  Tech id  */
  uint8_t tech_id_valid;  /**< Must be set to true if tech_id is being passed */
  antswitch_wwan_tech_type_enum_type_v01 tech_id;
  /**<   Tech for which Antenna Switch info is needed.
  */
}antswitch_get_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup antswitch_qmi_messages
    @{
  */
/** Response Message; Gets the specified Antenna Switch State for different active modem techs. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Antenna Switch config for all Modem Active techs  */
  uint8_t antswitch_cfg_valid;  /**< Must be set to true if antswitch_cfg is being passed */
  uint32_t antswitch_cfg_len;  /**< Must be set to # of elements in antswitch_cfg */
  antswitch_config_info_per_tech_type_v01 antswitch_cfg[ANTSWITCH_MAX_MODEM_ACTIVE_TECHS_V01];
  /**<   \n Contains all Active Techs' Antenna Switch Config State.
     Note that whether data is valid or not will be updated by is_valid
     flag in structure.
  */
}antswitch_get_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_ANTSWITCH_CONFIG_CHANGE_IND_V01
//#define REMOVE_QMI_ANTSWITCH_ENABLE_CONFIG_CHANGE_IND_V01
//#define REMOVE_QMI_ANTSWITCH_ENH_SET_ANT_SWITCH_STATE_V01
//#define REMOVE_QMI_ANTSWITCH_GET_ANT_SWITCH_STATE_V01
//#define REMOVE_QMI_ANTSWITCH_SET_ANT_SWITCH_STATE_V01

/*Service Message Definition*/
/** @addtogroup antswitch_qmi_msg_ids
    @{
  */
#define QMI_ANTSWITCH_SET_ANT_SWITCH_STATE_REQ_MSG_V01 0x0001
#define QMI_ANTSWITCH_SET_ANT_SWITCH_STATE_RESP_MSG_V01 0x0001
#define QMI_ANTSWITCH_GET_ANT_SWITCH_STATE_REQ_MSG_V01 0x0002
#define QMI_ANTSWITCH_GET_ANT_SWITCH_STATE_RESP_MSG_V01 0x0002
#define QMI_ANTSWITCH_ENH_SET_ANT_SWITCH_STATE_REQ_MSG_V01 0x0003
#define QMI_ANTSWITCH_ENH_SET_ANT_SWITCH_STATE_RESP_MSG_V01 0x0003
#define QMI_ANTSWITCH_ENABLE_CONFIG_CHANGE_IND_REQ_MSG_V01 0x0004
#define QMI_ANTSWITCH_ENABLE_CONFIG_CHANGE_IND_RESP_MSG_V01 0x0004
#define QMI_ANTSWITCH_CONFIG_CHANGE_IND_V01 0x0005
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro antswitch_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type antswitch_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define antswitch_get_service_object_v01( ) \
          antswitch_get_service_object_internal_v01( \
            ANTSWITCH_V01_IDL_MAJOR_VERS, ANTSWITCH_V01_IDL_MINOR_VERS, \
            ANTSWITCH_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

