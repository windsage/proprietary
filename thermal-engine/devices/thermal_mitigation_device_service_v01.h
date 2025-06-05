#ifndef TMD_SERVICE_01_H
#define TMD_SERVICE_01_H
/**
  @file thermal_mitigation_device_service_v01.h

  @brief This is the public header file which defines the tmd service Data structures.

  This header file defines the types and structures that were defined in
  tmd. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2011, 2022 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.2/tmd/api/thermal_mitigation_device_service_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9
   It was generated on: Wed Nov 24 2021 (Spin 0)
   From IDL File: thermal_mitigation_device_service_v01.idl */

/** @defgroup tmd_qmi_consts Constant values defined in the IDL */
/** @defgroup tmd_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup tmd_qmi_enums Enumerated types used in QMI messages */
/** @defgroup tmd_qmi_messages Structures sent as QMI messages */
/** @defgroup tmd_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup tmd_qmi_accessor Accessor for QMI service object */
/** @defgroup tmd_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup tmd_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define TMD_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define TMD_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define TMD_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define TMD_V01_MAX_MESSAGE_ID 0x0025
/**
    @}
  */


/** @addtogroup tmd_qmi_consts
    @{
  */
#define QMI_TMD_MITIGATION_DEV_LIST_MAX_V01 32
#define QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01 32
#define QMI_TMD_MITIGATION_DEV_LIST_EXT01_MAX_V01 64
/**
    @}
  */

/** @addtogroup tmd_qmi_aggregates
    @{
  */
typedef struct {

  char mitigation_dev_id[QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01 + 1];
  /**<   Mitigation device ID.*/
}tmd_mitigation_dev_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup tmd_qmi_aggregates
    @{
  */
typedef struct {

  tmd_mitigation_dev_id_type_v01 mitigation_dev_id;
  /**<   Mitigation device ID.*/

  uint8_t max_mitigation_level;
  /**<   Maximum valid mitigation level. Valid range: 0 to max_mitigation_level.*/
}tmd_mitigation_dev_list_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Request Message; Gets the list of mitigation devices from the remote endpoint. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}tmd_get_mitigation_device_list_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Response Message; Gets the list of mitigation devices from the remote endpoint. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Mitigation Device List */
  uint8_t mitigation_device_list_valid;  /**< Must be set to true if mitigation_device_list is being passed */
  uint32_t mitigation_device_list_len;  /**< Must be set to # of elements in mitigation_device_list */
  tmd_mitigation_dev_list_type_v01 mitigation_device_list[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];

  /* Optional */
  uint8_t mitigation_device_list_ext01_valid;  /**< Must be set to true if mitigation_device_list_ext01 is being passed */
  uint32_t mitigation_device_list_ext01_len;  /**< Must be set to # of elements in mitigation_device_list_ext01 */
  tmd_mitigation_dev_list_type_v01 mitigation_device_list_ext01[QMI_TMD_MITIGATION_DEV_LIST_EXT01_MAX_V01];
}tmd_get_mitigation_device_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Request Message; Sets the thermal mitigation level for the specified mitigation
           device. */
typedef struct {

  /* Mandatory */
  /*  Mitigation Device ID */
  tmd_mitigation_dev_id_type_v01 mitigation_dev_id;

  /* Mandatory */
  /*  Thermal Mitigation Level */
  uint8_t mitigation_level;
  /**<   The thermal mitigation level to set. No mitigation is defined as zero.
      Each increment is predefined to have increasing mitigative
      effect. The per-device maximum valid level is obtained from
      GET_MITIGATION_DEVICE_LIST.
   */
}tmd_set_mitigation_level_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Response Message; Sets the thermal mitigation level for the specified mitigation
           device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}tmd_set_mitigation_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Request Message; Gets the thermal mitigation level for the mitigation device. */
typedef struct {

  /* Mandatory */
  /*  Mitigation Device */
  tmd_mitigation_dev_id_type_v01 mitigation_device;
}tmd_get_mitigation_level_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Response Message; Gets the thermal mitigation level for the mitigation device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Current Thermal Mitigation Level */
  uint8_t current_mitigation_level_valid;  /**< Must be set to true if current_mitigation_level is being passed */
  uint8_t current_mitigation_level;
  /**<   Current thermal mitigation level.
   */

  /* Optional */
  /*  Requested Thermal Mitigation Level */
  uint8_t requested_mitigation_level_valid;  /**< Must be set to true if requested_mitigation_level is being passed */
  uint8_t requested_mitigation_level;
  /**<   The requested thermal mitigation level from the client. The
      default is zero if the client has not previously set the
      mitigation level.
   */
}tmd_get_mitigation_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Request Message; Registers for notification of mitigation device level changes. */
typedef struct {

  /* Mandatory */
  /*  Mitigation Device */
  tmd_mitigation_dev_id_type_v01 mitigation_device;
}tmd_register_notification_mitigation_level_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Response Message; Registers for notification of mitigation device level changes. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}tmd_register_notification_mitigation_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Request Message; Deregisters the notification for mitigation device level changes.  */
typedef struct {

  /* Mandatory */
  /*  Mitigation Device */
  tmd_mitigation_dev_id_type_v01 mitigation_device;
}tmd_deregister_notification_mitigation_level_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Response Message; Deregisters the notification for mitigation device level changes.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}tmd_deregister_notification_mitigation_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup tmd_qmi_messages
    @{
  */
/** Indication Message; Indicates that a mitigation level report has been received. */
typedef struct {

  /* Mandatory */
  /*  Mitigation Device */
  tmd_mitigation_dev_id_type_v01 mitigation_device;

  /* Mandatory */
  /*  Current Thermal Mitigation Level */
  uint8_t current_mitigation_level;
  /**<   Current thermal mitigation level. */
}tmd_mitigation_level_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_V01
//#define REMOVE_QMI_TMD_GET_MITIGATION_DEVICE_LIST_V01
//#define REMOVE_QMI_TMD_GET_MITIGATION_LEVEL_V01
//#define REMOVE_QMI_TMD_GET_SUPPORTED_FIELDS_V01
//#define REMOVE_QMI_TMD_GET_SUPPORTED_MSGS_V01
//#define REMOVE_QMI_TMD_MITIGATION_LEVEL_REPORT_IND_V01
//#define REMOVE_QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_V01
//#define REMOVE_QMI_TMD_SET_MITIGATION_LEVEL_V01

/*Service Message Definition*/
/** @addtogroup tmd_qmi_msg_ids
    @{
  */
#define QMI_TMD_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_TMD_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_TMD_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_TMD_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01 0x0020
#define QMI_TMD_GET_MITIGATION_DEVICE_LIST_RESP_V01 0x0020
#define QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01 0x0021
#define QMI_TMD_SET_MITIGATION_LEVEL_RESP_V01 0x0021
#define QMI_TMD_GET_MITIGATION_LEVEL_REQ_V01 0x0022
#define QMI_TMD_GET_MITIGATION_LEVEL_RESP_V01 0x0022
#define QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01 0x0023
#define QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_RESP_V01 0x0023
#define QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01 0x0024
#define QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_RESP_V01 0x0024
#define QMI_TMD_MITIGATION_LEVEL_REPORT_IND_V01 0x0025
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro tmd_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type tmd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define tmd_get_service_object_v01( ) \
          tmd_get_service_object_internal_v01( \
            TMD_V01_IDL_MAJOR_VERS, TMD_V01_IDL_MINOR_VERS, \
            TMD_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

