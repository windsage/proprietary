/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QCRIL_LEGACY_APIS_H
#define _QCRIL_LEGACY_APIS_H

#define QMI_RIL_SYS_PROP_NAME_BASEBAND "ro.baseband"
#define PROPERTY_NAME_MAX 80

#include "common_v01.h"
#include "telephony/ril.h"
#include <framework/legacy.h>

#define QCRIL_MAX_MODEM_ID ( (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SVLTE2) || qmi_ril_is_feature_supported(QMI_RIL_FEATURE_FUSION_CSFB) ) ? 2 : 1)

void qmi_ril_reset_multi_sim_ftr_info();
int qmi_ril_is_feature_supported(int feature);
int qmi_ril_is_multi_sim_feature_supported();
uint32_t qmi_ril_get_sim_slot(void);
void qmi_ril_set_sim_slot(uint32_t slot);
void qmi_ril_reset_baseband_rat_option();

#ifdef __cplusplus
extern "C" {
#endif
void qmi_ril_clear_thread_name(pthread_t thread_id);
int qmi_ril_get_thread_name(pthread_t thread_id, char* thread_name);
void qmi_ril_set_thread_name(pthread_t thread_id, const char* thread_name);
qcril_instance_id_e_type qmi_ril_get_process_instance_id(void);
void qmi_ril_set_process_instance_id(qcril_instance_id_e_type instance_id);
#ifdef __cplusplus
}
#endif

/***************************************************************************************************
    @function
    qmi_ril_get_property_value_from_string

    @brief
    Reads system property to retrieve the value when the type of the value is string.

    @param[in]
        property_name
            pointer to the name of property that needs to be read
        default_property_value
            pointer to the string that needs to be returned in case the read fails

    @param[out]
        property_value
            pointer to the variable that would hold the read property value

    @retval
    none
***************************************************************************************************/
void qmi_ril_get_property_value_from_string(const char* property_name, char* property_value,
                                            const char* default_property_value);

/***************************************************************************************************
    @function
    qmi_ril_get_property_value_from_integer

    @brief
    Reads system property to retrieve the value when the type of the value is integer.

    @param[in]
        property_name
            pointer to the name of property that needs to be read
        default_property_value
            value of interger that needs to be returned in case the read fails

    @param[out]
        property_value
            pointer to the variable that would hold the read property value

    @retval
    none
***************************************************************************************************/
void qmi_ril_get_property_value_from_integer(const char* property_name, int* property_value,
                                             int default_property_value);

/***************************************************************************************************
    @function
        qmi_ril_get_property_value_from_boolean

    @brief
        Reads system property to retrieve the value when the type of the value is boolean.

    @param[in]
        property_name
            pointer to the name of property that needs to be read
        default_property_value
            value of boolean that needs to be returned in case the read fails

    @param[out]
        property_value
            pointer to the variable that would hold the read property value

    @retval
        none
***************************************************************************************************/
void qmi_ril_get_property_value_from_boolean(const char* property_name, boolean* property_value,
                                             boolean default_property_value);

/***************************************************************************************************
    @function
    qmi_ril_set_property_value_to_string

    @brief
    Writes a value to a system property when the type of the value is string.

    @param[in]
        property_name
            pointer to the name of property that needs to be updated
        property_value
            pointer to the string that needs to be used for setting the value

    @param[out]
        none

    @retval
    E_SUCCESS If set operation is successful, appropriate error code otherwise
***************************************************************************************************/
RIL_Errno qmi_ril_set_property_value_to_string(const char* property_name,
                                               const char* property_value);

/***************************************************************************************************
    @function
    qmi_ril_set_property_value_to_integer

    @brief
    Writes a value to a system property when the type of the value is integer.

    @param[in]
        property_name
            pointer to the name of property that needs to be updated
        property_value
            value of the integer that needs to be used for setting the value

    @param[out]
        none

    @retval
    E_SUCCESS If set operation is successful, appropriate error code otherwise
***************************************************************************************************/
RIL_Errno qmi_ril_set_property_value_to_integer(const char* property_name, int property_value);

RIL_Errno qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error_type_v01 qmi_err);
int qmi_ril_retrieve_number_of_rilds();

const char* qcril_log_ril_radio_state_to_str(RIL_RadioState radio_state);

#endif
