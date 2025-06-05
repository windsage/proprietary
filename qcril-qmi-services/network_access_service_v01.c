/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        N E T W O R K _ A C C E S S _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the nas service Data structures.

  Copyright (c) 2006-2023 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  


  $Header: //components/rel/qmimsgs.mpss/6.5/nas/src/network_access_service_v01.c#16 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Thu Oct  5 2023 (Spin 0)
   From IDL File: network_access_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "network_access_service_v01.h"
#include "common_v01.h"
#include "network_access_service_common_v01.h"
#include "voice_nas_services_common_v01.h"


/*Type Definitions*/
static const uint8_t nas_common_network_id_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_network_id_type_v01, mcc),
  NAS_MCC_MNC_MAX_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_network_id_type_v01, mnc),
  NAS_MCC_MNC_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_snpn_id_name_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_id_name_type_v01, nid),
  NAS_SNPN_NID_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_id_name_type_v01, name),
  NAS_SNPN_HRN_MAX_V01,
  QMI_IDL_OFFSET8(nas_snpn_id_name_type_v01, name) - QMI_IDL_OFFSET8(nas_snpn_id_name_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_signal_stregth_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_signal_stregth_indicator_type_v01, report_signal_strength),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_signal_stregth_indicator_type_v01, report_signal_strength_threshold_list),
  NAS_SIG_STRENGTH_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_signal_stregth_indicator_type_v01, report_signal_strength_threshold_list) - QMI_IDL_OFFSET8(nas_signal_stregth_indicator_type_v01, report_signal_strength_threshold_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rssi_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rssi_indicator_type_v01, report_rssi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rssi_indicator_type_v01, rssi_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ecio_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ecio_indicator_type_v01, report_ecio),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ecio_indicator_type_v01, ecio_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_io_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_io_indicator_type_v01, report_io),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_io_indicator_type_v01, io_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_sinr_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sinr_indicator_type_v01, report_sinr),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sinr_indicator_type_v01, sinr_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rsrq_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrq_indicator_type_v01, report_rsrq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrq_indicator_type_v01, rsrq_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ecio_indicator_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ecio_indicator_threshold_type_v01, report_ecio),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ecio_indicator_threshold_type_v01, threshold_list),
  NAS_ECIO_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_ecio_indicator_threshold_type_v01, threshold_list) - QMI_IDL_OFFSET8(nas_ecio_indicator_threshold_type_v01, threshold_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_sinr_indicator_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sinr_indicator_threshold_type_v01, report_sinr),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sinr_indicator_threshold_type_v01, threshold_list),
  NAS_SINR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_sinr_indicator_threshold_type_v01, threshold_list) - QMI_IDL_OFFSET8(nas_sinr_indicator_threshold_type_v01, threshold_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rsrp_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrp_indicator_type_v01, report_lte_rsrp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrp_indicator_type_v01, lte_rsrp_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_snr_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_snr_indicator_type_v01, report_lte_snr),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_snr_indicator_type_v01, lte_snr_delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_supp_ciot_optimizations_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_supp_ciot_optimizations_type_v01, cp_ciot_supported),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_supp_ciot_optimizations_type_v01, up_ciot_supported),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_signal_strength_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_signal_strength_type_v01, sig_strength),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_signal_strength_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rf_band_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_band_info_type_v01, radio_if),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_band_info_type_v01, active_band),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_rf_band_info_type_v01, active_channel),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rf_dedicated_band_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_dedicated_band_info_type_v01, radio_if),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_dedicated_band_info_type_v01, dedicated_band),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rf_bandwidth_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_bandwidth_info_type_v01, radio_if),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rf_bandwidth_info_type_v01, bandwidth),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_registration_reject_reason_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_registration_reject_reason_type_v01, service_domain),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_registration_reject_reason_type_v01, reject_cause),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rssi_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rssi_type_v01, rssi),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rssi_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ecio_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ecio_type_v01, ecio),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ecio_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_error_rate_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_error_rate_type_v01, error_rate),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_error_rate_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rsrq_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrq_type_v01, rsrq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rsrq_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_reg_network_reject_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_reg_network_reject_v01, reg_network_reject),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_reg_network_reject_v01, suppress_sys_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_signal_strength_list_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_signal_strength_list_type_v01, sig_strength),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_signal_strength_list_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_band_pref_mask_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_1_64),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_65_128),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_129_192),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_193_256),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_257_320),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_321_384),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_385_448),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_band_pref_mask_type_v01, bits_449_512),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t lte_band_pref_mask_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(lte_band_pref_mask_ext_type_v01, bits_1_64),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(lte_band_pref_mask_ext_type_v01, bits_65_128),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(lte_band_pref_mask_ext_type_v01, bits_129_192),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(lte_band_pref_mask_ext_type_v01, bits_193_256),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_network_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_info_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_info_type_v01, mobile_network_code),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_info_type_v01, network_status),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(nas_3gpp_network_info_type_v01, network_description),
  NAS_NETWORK_DESCRIPTION_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_network_radio_access_technology_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_network_radio_access_technology_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_network_radio_access_technology_type_v01, mnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_radio_access_technology_type_v01, rat),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_scanned_ciot_lte_op_mode_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_scanned_ciot_lte_op_mode_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_scanned_ciot_lte_op_mode_type_v01, mnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_scanned_ciot_lte_op_mode_type_v01, lte_op_mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_mnc_pcs_digit_include_status_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_mnc_pcs_digit_include_status_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_mnc_pcs_digit_include_status_type_v01, mnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_mnc_pcs_digit_include_status_type_v01, mnc_includes_pcs_digit),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_csg_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_info_type_v01, id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_info_type_v01, name),
  NAS_CSG_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_csg_info_type_v01, name) - QMI_IDL_OFFSET8(nas_csg_info_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cag_id_name_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cag_id_name_type_v01, id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cag_id_name_type_v01, name),
  NAS_CAG_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_cag_id_name_type_v01, name) - QMI_IDL_OFFSET8(nas_cag_id_name_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_fsk_hop_table_entry_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_hop_table_entry_type_v01, delta),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_hop_table_entry_type_v01, freq),
  NAS_FSK_MAX_FREQS_V01,
  QMI_IDL_OFFSET8(nas_fsk_hop_table_entry_type_v01, freq) - QMI_IDL_OFFSET8(nas_fsk_hop_table_entry_type_v01, freq_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_csg_nw_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_info_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_info_type_v01, mnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_info_type_v01, csg_list_cat),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_csg_nw_info_type_v01, csg_info),
  QMI_IDL_TYPE88(0, 30),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cag_nw_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cag_nw_info_type_v01, cag_info_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cag_nw_info_type_v01, cag_in_allowed_list),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cag_nw_info_type_v01, cag_id_name),
  QMI_IDL_TYPE88(0, 31),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_csg_nw_signal_strength_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_signal_strength_info_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_signal_strength_info_type_v01, mnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_signal_strength_info_type_v01, csg_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_signal_strength_info_type_v01, signal_strength),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_pci_scan_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, freq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, cell_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, global_cell_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, plmn),
  NAS_PCI_SCAN_MAX_NUM_PLMN_V01,
  QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, plmn) - QMI_IDL_OFFSET8(nas_pci_scan_cell_info_type_v01, plmn_len),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_pci_scan_signal_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrp_rx0),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrp_rx1),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrq_rx0),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_pci_scan_signal_info_type_v01, rsrq_rx1),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_pci_nw_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_pci_nw_info_type_v01, pci_cell_info),
  NAS_PCI_SCAN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_pci_nw_info_type_v01, pci_cell_info) - QMI_IDL_OFFSET8(nas_pci_nw_info_type_v01, pci_cell_info_len),
  QMI_IDL_TYPE88(0, 36),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_pci_nw_info_type_v01, signal_info),
  QMI_IDL_TYPE88(0, 37),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_geran_cell_search_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, bsic),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, cell_identity),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, cell_is_barred),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, gprs_is_supported),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, plmn),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_search_info_type_v01, rxlev),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cell_search_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, earfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, phy_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, tac),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, global_cell_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, cell_is_barred),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, rssi),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, plmn),
  NAS_PCI_SCAN_MAX_NUM_PLMN_V01,
  QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, plmn) - QMI_IDL_OFFSET8(nas_lte_cell_search_info_type_v01, plmn_len),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_manual_network_register_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_manual_network_register_info_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_manual_network_register_info_type_v01, mobile_network_code),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_manual_network_register_info_type_v01, radio_access_technology),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_serving_system_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, registration_state),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, cs_attach_state),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, ps_attach_state),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, selected_network),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, radio_if),
  NAS_RADIO_IF_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_serving_system_type_v01, radio_if) - QMI_IDL_OFFSET8(nas_serving_system_type_v01, radio_if_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_plmn_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_type_v01, mobile_network_code),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(nas_plmn_type_v01, network_description),
  NAS_NETWORK_DESCRIPTION_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_system_id_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_system_id_type_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_system_id_type_v01, nid),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_base_station_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_base_station_info_type_v01, base_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_base_station_info_type_v01, base_lat),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_base_station_info_type_v01, base_long),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_roaming_indicator_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_roaming_indicator_type_v01, radio_if),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_roaming_indicator_type_v01, roaming_indicator),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_time_zone_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_time_zone_type_v01, lp_sec),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_time_zone_type_v01, ltm_offset),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_time_zone_type_v01, daylt_savings),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ss_detailed_service_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ss_detailed_service_info_type_v01, srv_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ss_detailed_service_info_type_v01, srv_capability),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ss_detailed_service_info_type_v01, hdr_srv_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ss_detailed_service_info_type_v01, hdr_hybrid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ss_detailed_service_info_type_v01, is_sys_forbidden),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_system_id_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_system_id_ext_type_v01, mcc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_system_id_ext_type_v01, imsi_11_12),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_gw_sys_info3_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_gw_sys_info3_type_v01, cs_bar_status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_gw_sys_info3_type_v01, ps_bar_status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_universal_time_and_local_time_zone_3gpp_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, year),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, month),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, day),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, hour),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, minute),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, second),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_universal_time_and_local_time_zone_3gpp_type_v01, time_zone),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp2_home_network_ext_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, mcc_mnc),
  QMI_IDL_TYPE88(3, 0),
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, network_desc_display),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, network_desc_encoding),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, network_desc),
  NAS_NETWORK_DESCRIPTION_MAX_V01,
  QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, network_desc) - QMI_IDL_OFFSET8(nas_3gpp2_home_network_ext_type_v01, network_desc_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_mcs_digit_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_mcs_digit_type_v01, is_3gpp_network),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_mcs_digit_type_v01, mnc_includes_pcs_digit),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_preferred_networks_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_preferred_networks_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_preferred_networks_type_v01, mobile_network_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_preferred_networks_type_v01, radio_access_technology),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_forbidden_networks_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_forbidden_networks_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_forbidden_networks_type_v01, mobile_network_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_accolc_set_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_accolc_set_type_v01, spc),
  NAS_SPC_MAX_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_accolc_set_type_v01, accolc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_registration_parameters_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_registration_parameters_type_v01, reg_home_sid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_registration_parameters_type_v01, reg_foreign_sid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_registration_parameters_type_v01, reg_foreign_nid),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hdr_scp_config_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_scp_config_type_v01, state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_scp_config_type_v01, protocol_mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_scp_config_type_v01, broadcast_mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_scp_config_type_v01, application_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rf_band_info_ext_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_band_info_ext_type_v01, radio_if),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rf_band_info_ext_type_v01, active_band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ext_type_v01, active_channel),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_net_sel_pref_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_net_sel_pref_type_v01, net_sel_pref),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_net_sel_pref_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_net_sel_pref_type_v01, mnc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_csg_nw_iden_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_iden_type_v01, mcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_iden_type_v01, mnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_iden_type_v01, mnc_includes_pcs_digit),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_nw_iden_type_v01, id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_csg_nw_iden_type_v01, rat),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ddtm_preference_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, ddtm_pref),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, ddtm_action),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, so_list_action),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, so),
  NAS_SO_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, so) - QMI_IDL_OFFSET8(nas_ddtm_preference_type_v01, so_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ddtm_settings_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, curr_ddtm_status),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, ddtm_pref),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, ddtm_action),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, so_list_action),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, so),
  NAS_SO_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, so) - QMI_IDL_OFFSET8(nas_ddtm_settings_type_v01, so_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_service_provider_name_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_service_provider_name_type_v01, display_cond),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_service_provider_name_type_v01, spn),
  NAS_SERVICE_PROVIDER_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_service_provider_name_type_v01, spn) - QMI_IDL_OFFSET8(nas_service_provider_name_type_v01, spn_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_operator_plmn_list_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_operator_plmn_list_type_v01, mcc),
  NAS_MCC_MNC_MAX_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_operator_plmn_list_type_v01, mnc),
  NAS_MCC_MNC_MAX_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_operator_plmn_list_type_v01, lac1),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_operator_plmn_list_type_v01, lac2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_operator_plmn_list_type_v01, pnn_rec_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_plmn_network_name_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, coding_scheme),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, ci),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, long_name_spare_bits),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, short_name_spare_bits),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, long_name),
  NAS_LONG_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, long_name) - QMI_IDL_OFFSET8(nas_plmn_network_name_type_v01, long_name_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_plmn_network_name_type_v01, short_name),
  NAS_SHORT_NAME_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_plmn_network_name_type_v01, short_name) - QMI_IDL_OFFSET16RELATIVE(nas_plmn_network_name_type_v01, short_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_tac_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_tac_info_type_v01, tac),
  NAS_NR5G_TAC_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp2_home_sid_nid_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_home_sid_nid_info_type_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_home_sid_nid_info_type_v01, nid),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp2_min_based_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_min_based_info_type_v01, mcc_m),
  NAS_MCC_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_min_based_info_type_v01, imsi_m_11_12),
  NAS_IMSI_11_12_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_min_based_info_type_v01, imsi_m_s1),
  NAS_IMSI_MIN1_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_min_based_info_type_v01, imsi_m_s2),
  NAS_IMSI_MIN2_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp2_true_imsi_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_true_imsi_info_type_v01, mcc_t),
  NAS_MCC_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_true_imsi_info_type_v01, imsi_t_11_12),
  NAS_IMSI_11_12_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_true_imsi_info_type_v01, imsi_t_s1),
  NAS_IMSI_MIN1_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_true_imsi_info_type_v01, imsi_t_s2),
  NAS_IMSI_MIN2_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_true_imsi_info_type_v01, imsi_t_addr_num),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_channel_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_channel_info_type_v01, pri_ch_a),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_channel_info_type_v01, pri_ch_b),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_channel_info_type_v01, sec_ch_a),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_channel_info_type_v01, sec_ch_b),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nmr_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_bsic),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_type_v01, nmr_rx_lev),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_geran_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, bsic),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, timing_advance),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, rx_lev),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, nmr_cell_info),
  NAS_NMR_MAX_NUM_V01,
  QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, nmr_cell_info) - QMI_IDL_OFFSET8(nas_geran_cell_info_type_v01, nmr_cell_info_len),
  QMI_IDL_TYPE88(0, 72),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_monitored_cell_set_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_info_type_v01, umts_uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_info_type_v01, umts_psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_info_type_v01, umts_rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_info_type_v01, umts_ecio),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_geran_nbr_cell_set_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_info_type_v01, geran_arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_info_type_v01, geran_bsic_ncc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_info_type_v01, geran_bsic_bcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_info_type_v01, geran_rssi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_lte_nbr_cell_set_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, earfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, rsrp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, srxlev),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_lte_nbr_cell_set_info_type_v01, cell_is_tdd),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, ecio),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, umts_monitored_cell),
  NAS_UMTS_MAX_MONITORED_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, umts_monitored_cell) - QMI_IDL_OFFSET8(nas_umts_cell_info_type_v01, umts_monitored_cell_len),
  QMI_IDL_TYPE88(0, 74),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_umts_cell_info_type_v01, umts_geran_nbr_cell),
  NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_umts_cell_info_type_v01, umts_geran_nbr_cell) - QMI_IDL_OFFSET16RELATIVE(nas_umts_cell_info_type_v01, umts_geran_nbr_cell_len),
  QMI_IDL_TYPE88(0, 75),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, nid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, base_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, refpn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, base_lat),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_cell_info_type_v01, base_long),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_cell_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_cell_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_cell_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_cell_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_cell_type_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_cell_type_v01, srxlev),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_intra_freq_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, tac),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, global_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, earfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, serving_cell_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, cell_resel_priority),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, s_non_intra_search),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, thresh_serving_low),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, s_intra_search),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, cells),
  NAS_MAX_LTE_NGBR_NUM_CELLS_V01,
  QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, cells) - QMI_IDL_OFFSET8(nas_lte_intra_freq_type_v01, cells_len),
  QMI_IDL_TYPE88(0, 79),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_inter_freq_freqs_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, earfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, threshX_low),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, threshX_high),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, cell_resel_priority),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, cells),
  NAS_MAX_LTE_NGBR_NUM_CELLS_V01,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, cells) - QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_type_v01, cells_len),
  QMI_IDL_TYPE88(0, 79),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_inter_freq_freqs_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, earfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, threshX_low),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, threshX_high),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, cell_resel_priority),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, cells),
  NAS_MAX_LTE_NGBR_NUM_CELLS_V01,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, cells) - QMI_IDL_OFFSET8(nas_lte_inter_freq_freqs_ext_type_v01, cells_len),
  QMI_IDL_TYPE88(0, 79),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_inter_freq_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_type_v01, freqs),
  NAS_MAX_LTE_NGBR_NUM_FREQS_V01,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_type_v01, freqs) - QMI_IDL_OFFSET8(nas_lte_inter_freq_type_v01, freqs_len),
  QMI_IDL_TYPE88(0, 81),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_inter_freq_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_ext_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_ext_type_v01, freqs),
  NAS_MAX_LTE_NGBR_NUM_FREQS_EXT_V01,
  QMI_IDL_OFFSET8(nas_lte_inter_freq_ext_type_v01, freqs) - QMI_IDL_OFFSET8(nas_lte_inter_freq_ext_type_v01, freqs_len),
  QMI_IDL_TYPE88(0, 82),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_gsm_cell_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, band_1900),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, cell_id_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, bsic_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_cell_type_v01, srxlev),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_gsm_freq_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, cell_resel_priority),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, thresh_gsm_high),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, thresh_gsm_low),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, ncc_permitted),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, cells),
  NAS_MAX_LTE_NGBR_GSM_NUM_CELLS_V01,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, cells) - QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_freq_type_v01, cells_len),
  QMI_IDL_TYPE88(0, 85),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_gsm_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_type_v01, freqs),
  NAS_MAX_LTE_NGBR_GSM_NUM_FREQS_V01,
  QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_type_v01, freqs) - QMI_IDL_OFFSET8(nas_lte_ngbr_gsm_type_v01, freqs_len),
  QMI_IDL_TYPE88(0, 86),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_wcdma_cell_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_cell_type_v01, psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_cell_type_v01, cpich_rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_cell_type_v01, cpich_ecno),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_cell_type_v01, srxlev),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_wcdma_freq_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, uarfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, cell_resel_priority),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, thresh_Xhigh),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, thresh_Xlow),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, cells),
  NAS_MAX_LTE_NGBR_WCDMA_NUM_CELLS_V01,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, cells) - QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_freq_type_v01, cells_len),
  QMI_IDL_TYPE88(0, 88),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ngbr_wcdma_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_type_v01, freqs),
  NAS_MAX_LTE_NGBR_WCDMA_NUM_FREQS_V01,
  QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_type_v01, freqs) - QMI_IDL_OFFSET8(nas_lte_ngbr_wcdma_type_v01, freqs_len),
  QMI_IDL_TYPE88(0, 89),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_ngbr_lte_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_ngbr_lte_type_v01, wcdma_rrc_state),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_wcdma_ngbr_lte_type_v01, umts_lte_nbr_cell),
  NAS_UMTS_LTE_MAX_NBR_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET8(nas_wcdma_ngbr_lte_type_v01, umts_lte_nbr_cell) - QMI_IDL_OFFSET8(nas_wcdma_ngbr_lte_type_v01, umts_lte_nbr_cell_len),
  QMI_IDL_TYPE88(0, 76),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rx_power_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_power_type_v01, rx0_agc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_power_type_v01, rx1_agc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_gsm_cell_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_cell_info_ext_type_v01, g_ta),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_cell_info_ext_type_v01, g_bcch),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_cell_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_cell_info_ext_type_v01, w_agc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_cell_info_ext_type_v01, w_txagc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_cell_info_ext_type_v01, w_dl_bler),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_active_set_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_info_type_v01, psc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_info_type_v01, cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_info_type_v01, rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_info_type_v01, ecio),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_info_type_v01, uarfcn),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_active_set_reference_rl_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_reference_rl_type_v01, rac),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_monitored_cell_set_ext_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_ecio),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_squal),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_srxlev),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_rank),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_monitored_cell_set_ext_info_type_v01, umts_set),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_geran_nbr_cell_set_ext_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_ext_info_type_v01, geran_arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_ext_info_type_v01, geran_bsic_ncc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_ext_info_type_v01, geran_bsic_bcc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_ext_info_type_v01, geran_rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_geran_nbr_cell_set_ext_info_type_v01, geran_rank),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_umts_cell_ext_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, ecio),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, squal),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, srxlev),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, umts_monitored_ext_ext_cell),
  NAS_UMTS_MAX_MONITORED_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, umts_monitored_ext_ext_cell) - QMI_IDL_OFFSET8(nas_umts_cell_ext_info_type_v01, umts_monitored_ext_ext_cell_len),
  QMI_IDL_TYPE88(0, 97),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_umts_cell_ext_info_type_v01, umts_geran_ext_nbr_cell),
  NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_umts_cell_ext_info_type_v01, umts_geran_ext_nbr_cell) - QMI_IDL_OFFSET16RELATIVE(nas_umts_cell_ext_info_type_v01, umts_geran_ext_nbr_cell_len),
  QMI_IDL_TYPE88(0, 98),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_active_set_ext_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, psc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, ecio),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, uarfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, sf),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, phy_chan_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, slot_format),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_active_set_ext_info_type_v01, is_compressed_mode_on),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nmr_cell_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_bsic),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_rx_lev),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_c1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_c2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_c31),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nmr_cell_info_ext_type_v01, nmr_c32),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_geran_cell_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, cell_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, bsic),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, timing_advance),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, rx_lev),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, nmr_cell_info),
  NAS_NMR_MAX_NUM_V01,
  QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, nmr_cell_info) - QMI_IDL_OFFSET8(nas_geran_cell_info_ext_type_v01, nmr_cell_info_len),
  QMI_IDL_TYPE88(0, 101),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_geran_eng_mode_scell_config_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_eng_mode_scell_config_type_v01, pbcch_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_eng_mode_scell_config_type_v01, gprs_rxlev_access_min),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_geran_eng_mode_scell_config_type_v01, gprs_ms_txpwr_max_cch),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_serving_cell_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, plmn),
  NAS_PLMN_LEN_V01,

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, nr5g_tac_info),
  QMI_IDL_TYPE88(0, 67),
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, global_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_info_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_eons_plmn_name_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, spn_enc),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, spn),
  NAS_SPN_LEN_MAX_V01,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, spn) - QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, spn_len),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_name_enc),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_name_ci),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_spare_bits),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_name),
  NAS_PLMN_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_name) - QMI_IDL_OFFSET8(nas_3gpp_eons_plmn_name_type_v01, plmn_short_name_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_3gpp_eons_plmn_name_type_v01, plmn_long_name_enc),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_3gpp_eons_plmn_name_type_v01, plmn_long_name_ci),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_3gpp_eons_plmn_name_type_v01, plmn_long_spare_bits),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_3gpp_eons_plmn_name_type_v01, plmn_long_name),
  NAS_PLMN_NAME_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_3gpp_eons_plmn_name_type_v01, plmn_long_name) - QMI_IDL_OFFSET16RELATIVE(nas_3gpp_eons_plmn_name_type_v01, plmn_long_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_display_bit_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_display_bit_type_v01, is_spn_set),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_display_bit_type_v01, is_plmn_set),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lang_plmn_names_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_long_name),
  NAS_ALT_LANG_NAME_LEN_MAX_V01,
  QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_long_name) - QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_long_name_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_short_name),
  NAS_ALT_LANG_NAME_LEN_MAX_V01,
  QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_short_name) - QMI_IDL_OFFSET8(nas_lang_plmn_names_type_v01, plmn_short_name_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_lang_plmn_names_type_v01, lang_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_standby_pref_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_standby_pref_type_v01, standby_pref),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_standby_pref_type_v01, priority_subs),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_standby_pref_type_v01, active_subs),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_standby_pref_type_v01, default_data_subs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_technology_pref_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_technology_pref_type_v01, technology_pref),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_technology_pref_type_v01, duration),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_active_technology_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_active_technology_type_v01, technology_pref),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_active_technology_type_v01, duration),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_julian_time_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, year),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, month),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, day),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, hour),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, minute),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, second),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_julian_time_type_v01, day_of_week),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_reg_reject_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_reg_reject_info_type_v01, reject_srv_domain),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_reg_reject_info_type_v01, rej_cause),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp2_srv_status_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp2_srv_status_info_type_v01, srv_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp2_srv_status_info_type_v01, is_pref_data_path),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_srv_status_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_srv_status_info_type_v01, srv_status),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_3gpp_srv_status_info_type_v01, true_srv_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_srv_status_info_type_v01, is_pref_data_path),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rat_plmn_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rat_plmn_info_type_v01, radio_if),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_rat_plmn_info_type_v01, plmn_id),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_common_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, srv_domain_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, srv_domain),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, srv_capability_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, srv_capability),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, roam_status_valid),

  QMI_IDL_FLAGS_FIRST_EXTENDED | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_FLAGS_ENUM_IS_UNSIGNED,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, roam_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, is_sys_forbidden_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sys_info_type_v01, is_sys_forbidden),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, p_rev_in_use_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, p_rev_in_use),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, bs_p_rev_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, bs_p_rev),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, ccs_supported_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, ccs_supported),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, cdma_sys_id_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, cdma_sys_id),
  QMI_IDL_TYPE88(0, 44),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, bs_info_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, bs_info),
  QMI_IDL_TYPE88(0, 45),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, packet_zone_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, packet_zone),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, network_id_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_only_sys_info_type_v01, network_id),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hdr_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, hdr_personality_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, hdr_personality),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, hdr_active_prot_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, hdr_active_prot),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, is856_sys_id_valid),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_only_sys_info_type_v01, is856_sys_id),
  NAS_IS_856_MAX_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_gsm_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_only_sys_info_type_v01, egprs_supp_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_only_sys_info_type_v01, egprs_supp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_only_sys_info_type_v01, dtm_supp_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_only_sys_info_type_v01, dtm_supp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, hs_call_status_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, hs_call_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, hs_ind_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, hs_ind),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, psc_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_only_sys_info_type_v01, psc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_only_sys_info_type_v01, tac_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_only_sys_info_type_v01, tac),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_only_sys_info_type_v01, tac_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_only_sys_info_type_v01, tac),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_hdr_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_hdr_only_sys_info_type_v01, is_sys_prl_match_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_hdr_only_sys_info_type_v01, is_sys_prl_match),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, lac_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, lac),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, cell_id_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, cell_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, reg_reject_info_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, reg_reject_info),
  QMI_IDL_TYPE88(0, 112),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, network_id_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_3gpp_only_sys_info_type_v01, network_id),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_sys_info_type_v01, cdma_hdr_only_sys_info),
  QMI_IDL_TYPE88(0, 123),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_sys_info_type_v01, cdma_specific_sys_info),
  QMI_IDL_TYPE88(0, 117),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hdr_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_hdr_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_hdr_sys_info_type_v01, cdma_hdr_only_sys_info),
  QMI_IDL_TYPE88(0, 123),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_hdr_sys_info_type_v01, hdr_specific_sys_info),
  QMI_IDL_TYPE88(0, 118),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_gsm_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gsm_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gsm_sys_info_type_v01, threegpp_specific_sys_info),
  QMI_IDL_TYPE88(0, 124),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gsm_sys_info_type_v01, gsm_specific_sys_info),
  QMI_IDL_TYPE88(0, 119),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_wcdma_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_wcdma_sys_info_type_v01, threegpp_specific_sys_info),
  QMI_IDL_TYPE88(0, 124),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_wcdma_sys_info_type_v01, wcdma_specific_sys_info),
  QMI_IDL_TYPE88(0, 120),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_sys_info_type_v01, threegpp_specific_sys_info),
  QMI_IDL_TYPE88(0, 124),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_sys_info_type_v01, lte_specific_sys_info),
  QMI_IDL_TYPE88(0, 121),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_sys_info_type_v01, threegpp_specific_sys_info),
  QMI_IDL_TYPE88(0, 124),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_sys_info_type_v01, nr5g_specific_sys_info),
  QMI_IDL_TYPE88(0, 122),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tdscdma_only_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, hs_call_status_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, hs_call_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, hs_ind_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, hs_ind),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cell_parameter_id_valid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cell_parameter_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cell_broadcast_cap_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cell_broadcast_cap),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cs_bar_status_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cs_bar_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, ps_bar_status_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, ps_bar_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cipher_domain_valid),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_tdscdma_only_sys_info_type_v01, cipher_domain),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tdscdma_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_tdscdma_sys_info_type_v01, common_sys_info),
  QMI_IDL_TYPE88(0, 116),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_tdscdma_sys_info_type_v01, threegpp_specific_sys_info),
  QMI_IDL_TYPE88(0, 124),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_tdscdma_sys_info_type_v01, tdscdma_specific_sys_info),
  QMI_IDL_TYPE88(0, 131),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_sys_info2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_sys_info2_type_v01, geo_sys_idx),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_sys_info2_type_v01, reg_prd),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hdr_sys_info2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_sys_info2_type_v01, geo_sys_idx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_gsm_sys_info2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_sys_info2_type_v01, geo_sys_idx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_gsm_sys_info2_type_v01, cell_broadcast_cap),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_wcdma_sys_info2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_sys_info2_type_v01, geo_sys_idx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wcdma_sys_info2_type_v01, cell_broadcast_cap),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_sys_info2_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sys_info2_type_v01, geo_sys_idx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_emerg_srv_ind_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_emerg_srv_ind_type_v01, emc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_emerg_srv_ind_type_v01, emf),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_common_sig_info_param_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_common_sig_info_param_type_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_common_sig_info_param_type_v01, ecio),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hdr_sig_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_hdr_sig_info_type_v01, common_sig_str),
  QMI_IDL_TYPE88(0, 139),
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_hdr_sig_info_type_v01, sinr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_sig_info_type_v01, io),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_sig_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sig_info_type_v01, rssi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sig_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sig_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sig_info_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tdscdma_sig_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_sig_info_type_v01, rssi),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_sig_info_type_v01, rscp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_sig_info_type_v01, ecio),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tdscdma_sig_info_type_v01, sinr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_sig_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_sig_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_sig_info_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nrdc_sig_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nrdc_sig_info_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nrdc_sig_info_type_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nrdc_sig_info_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_sig_rpt_config_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_sig_rpt_config_type_v01, rpt_rate),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_sig_rpt_config_type_v01, avg_period),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_set_rx_diversity_req_param_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_rx_diversity_req_param_type_v01, radio_if),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_rx_diversity_req_param_type_v01, rx_chain_bitmask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rx_chain_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, is_radio_tuned),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, rx_pwr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, ecio),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, rscp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, rsrp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rx_chain_info_type_v01, phase),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tx_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tx_info_type_v01, is_in_traffic),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tx_info_type_v01, tx_pwr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_akey_with_spc_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_akey_with_spc_type_v01, spc),
  NAS_SPC_MAX_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_akey_with_spc_type_v01, akey),
  QMI_NAS_AKEY_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_spn_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_spn_type_v01, spn_enc),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_spn_type_v01, spn),
  NAS_SPN_LEN_MAX_V01,
  QMI_IDL_OFFSET8(nas_spn_type_v01, spn) - QMI_IDL_OFFSET8(nas_spn_type_v01, spn_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_plmn_name_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_name_enc),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_name_ci),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_spare_bits),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_name),
  NAS_PLMN_NAME_MAX_V01,
  QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_name) - QMI_IDL_OFFSET8(nas_plmn_name_type_v01, plmn_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_bs_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, pilot_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, nid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, base_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, pilot_pn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, pilot_strength),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, base_lat),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, base_long),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_bs_info_type_v01, time_stamp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_position_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_position_info_type_v01, ue_in_idle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_position_info_type_v01, bs),
  NAS_CDMA_POSITION_INFO_MAX_V01,
  QMI_IDL_OFFSET8(nas_cdma_position_info_type_v01, bs) - QMI_IDL_OFFSET8(nas_cdma_position_info_type_v01, bs_len),
  QMI_IDL_TYPE88(0, 152),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_sig_rpt_config_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_sig_rpt_config_type_v01, rpt_rate),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_sig_rpt_config_type_v01, avg_period),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_hysteresis_config_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hysteresis_config_type_v01, signal),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_hysteresis_config_type_v01, delta),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tds_cell_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, plmn),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, uarfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, cell_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, cell_parameter_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, pathloss),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, timing_advance),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tds_cell_info_type_v01, rscp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_tds_nbr_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_tds_nbr_cell_info_type_v01, uarfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_tds_nbr_cell_info_type_v01, cell_parameter_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_tds_nbr_cell_info_type_v01, rscp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cphy_mbsfn_area_signal_strength_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_mbsfn_area_signal_strength_type_v01, area_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_mbsfn_area_signal_strength_type_v01, snr),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_mbsfn_area_signal_strength_type_v01, signal_level),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ims_registration_state_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ims_registration_state_type_v01, call_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ims_registration_state_type_v01, is_registered),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ims_pref_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ims_pref_type_v01, sys_mode),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_ims_pref_type_v01, ims_pref_call_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cdma_avoid_sys_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_sys_info_type_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_sys_info_type_v01, nid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_sys_info_type_v01, mnc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_sys_info_type_v01, mcc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_network_time_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_network_time_type_v01, universal_time),
  QMI_IDL_TYPE88(0, 111),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_time_type_v01, time_zone),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_time_type_v01, daylt_sav_adj),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_network_time_type_v01, radio_if),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_sib16_julian_time_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, year),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, month),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, day),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, hour),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, minute),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, second),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, millisecond),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_julian_time_type_v01, day_of_week),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_embms_tmgi_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_tmgi_type_v01, mrb_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_tmgi_type_v01, session_id_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_tmgi_type_v01, session_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_tmgi_type_v01, tmgi_identifier),
  NAS_TMGI_IDENTIFIER_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_embms_signal_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, area_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, snr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, excess_snr),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, tmgi_info),
  NAS_TMGI_BEARER_INFO_MAX_V01,
  QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, tmgi_info) - QMI_IDL_OFFSET8(nas_lte_embms_signal_type_v01, tmgi_info_len),
  QMI_IDL_TYPE88(0, 164),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cphy_ca_indicator_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_indicator_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_indicator_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_indicator_type_v01, scell_state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cphy_scell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_type_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_type_v01, band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_type_v01, scell_state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cphy_scell_info_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cphy_scell_info_v01, scell_info),
  QMI_IDL_TYPE88(0, 167),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cphy_scell_info_v01, scell_idx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cphy_pcell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_pcell_info_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_pcell_info_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_pcell_info_type_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_cphy_pcell_info_type_v01, band),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cphy_scell_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, scell_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, scell_idx),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_scell_info_ext_type_v01, ul_configured),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_oplmn_entry_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_oplmn_entry_type_v01, plmn),
  3,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_oplmn_entry_type_v01, access_tech),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_oplmn_list_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_oplmn_list_type_v01, list_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_oplmn_list_type_v01, total_list_entries),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_oplmn_list_type_v01, oplmn),
  ((NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01) & 0xFF), ((NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01) >> 8),
  QMI_IDL_OFFSET8(nas_oplmn_list_type_v01, oplmn) - QMI_IDL_OFFSET8(nas_oplmn_list_type_v01, oplmn_len),
  QMI_IDL_TYPE88(0, 171),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_3gpp_network_scan_result_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, mobile_country_code),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, mobile_network_code),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, network_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, rat),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, mnc_includes_pcs_digit),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(nas_3gpp_network_scan_result_type_v01, network_description),
  NAS_NETWORK_DESCRIPTION_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr_pci_nw_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, freq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, phy_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, signal_strength),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, band_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, plmn),
  NAS_NR_PCI_SCAN_MAX_NUM_PLMN_V01,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, plmn) - QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, plmn_len),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, global_cell_id),
  NAS_NR_PCI_SCAN_MAX_NUM_PLMN_V01,
  QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, global_cell_id) - QMI_IDL_OFFSET8(nas_nr_pci_nw_info_type_v01, global_cell_id_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_snpn_id_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_snpn_id_info_type_v01, plmn_id),
  QMI_IDL_TYPE88(2, 0),
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_id_info_type_v01, nid),
  NAS_SNPN_NID_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_snpn_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_snpn_info_type_v01, id),
  QMI_IDL_TYPE88(0, 175),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_info_type_v01, signal_quality),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_info_type_v01, signal_strength),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ssac_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_type_v01, barring_factor_voice),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_type_v01, barring_time_voice),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_type_v01, barring_factor_video),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_type_v01, barring_time_video),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_pmch_datamcs_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_pmch_datamcs_info_type_v01, mbsfn_area_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_pmch_datamcs_info_type_v01, pmch_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_pmch_datamcs_info_type_v01, data_mcs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_mbsfn_area_signal_strength_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_mbsfn_area_signal_strength_info_type_v01, area_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_mbsfn_area_signal_strength_info_type_v01, snr),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_mbsfn_area_signal_strength_info_type_v01, excess_snr),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_mbsfn_area_signal_strength_info_type_v01, signal_level),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_pmch_bler_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_pmch_bler_info_type_v01, pmch_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_pmch_bler_info_type_v01, num_crc_pass),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_pmch_bler_info_type_v01, num_crc_fail),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_ml1_pmch_bler_per_area_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ml1_pmch_bler_per_area_info_type_v01, area_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ml1_pmch_bler_per_area_info_type_v01, pmch_bler),
  NAS_LTE_PMCH_PER_MBSFN_AREA_MAX_V01,
  QMI_IDL_OFFSET8(nas_lte_ml1_pmch_bler_per_area_info_type_v01, pmch_bler) - QMI_IDL_OFFSET8(nas_lte_ml1_pmch_bler_per_area_info_type_v01, pmch_bler_len),
  QMI_IDL_TYPE88(0, 180),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_mtch_info_s_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_mtch_info_s_v01, area_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_mtch_info_s_v01, pmch_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_mtch_info_s_v01, lc_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_mtch_info_s_v01, num_mtch_tb_bytes),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ssac_voice_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_voice_info_type_v01, barring_factor_voice),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_voice_info_type_v01, barring_time_voice),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_sib2_ssac_voice_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sib2_ssac_voice_info_type_v01, sib2_barring_factor_voice),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_sib2_ssac_voice_info_type_v01, sib2_barring_time_voice),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_ssac_video_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_video_info_type_v01, barring_factor_video),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_video_info_type_v01, barring_time_video),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_sib2_ssac_video_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sib2_ssac_video_info_type_v01, sib2_barring_factor_video),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_sib2_ssac_video_info_type_v01, sib2_barring_time_video),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_rrc_ac_barring_params_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rrc_ac_barring_params_type_v01, ac_barring_factor),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rrc_ac_barring_params_type_v01, ac_barring_time),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rrc_ac_barring_params_type_v01, ac_barring_for_special_ac),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_target_frequency_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_target_frequency_info_type_v01, target_freq),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_target_frequency_info_type_v01, target_rat),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cell_lock_entry_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cell_lock_entry_type_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cell_lock_entry_type_v01, freq),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cell_lock_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cell_lock_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_lock_info_type_v01, freq),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_cell_identity_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_cell_identity_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_cell_identity_info_type_v01, scs),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_cell_identity_info_type_v01, arfcn),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_cell_identity_info_type_v01, band),
  QMI_IDL_TYPE88(0, 24),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_frequency_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_frequency_type_v01, arfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_frequency_type_v01, scs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t geran_grr_gcell_identifier_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(geran_grr_gcell_identifier_s_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(geran_grr_gcell_identifier_s_v01, bsic),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(geran_grr_gcell_identifier_s_v01, plmn_id),
  NAS_PLMN_LEN_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(geran_grr_gcell_identifier_s_v01, cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(geran_grr_gcell_identifier_s_v01, location_area_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_rach_fail_ind_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_type_v01, phy_cell_id),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_type_v01, band),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_type_v01, lte_dl_earfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_type_v01, lte_ul_earfcn),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_v2x_multi_pool_status_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_multi_pool_status_type_v01, status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_multi_pool_status_type_v01, cause),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_b2_thresh_adj_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_info_type_v01, rsrq_thresh),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_info_type_v01, rsrp_thresh),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_info_type_v01, thresh1_adj),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_arfcn_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_arfcn_type_v01, arfcn),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_arfcn_type_v01, band),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_earfcn_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_earfcn_type_v01, downlink_only),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_earfcn_type_v01, dl_earfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_earfcn_type_v01, ul_earfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_earfcn_type_v01, bandwidth),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_pps_freq_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_pps_freq_info_type_v01, nr5g_earfcn),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_pps_freq_info_type_v01, nr5g_band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_pps_freq_info_type_v01, nr5g_subcarrier_spacing),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_cell_barring_params_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_params_type_v01, barring_factor),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_params_type_v01, barring_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_access_barring_status_params_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_access_barring_status_params_type_v01, barring_status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_access_barring_status_params_type_v01, barring_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_black_list_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, rat),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, cgi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, threshold_p),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, threshold_q),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, offset_p),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_black_list_type_v01, offset_q),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_slot_cfg_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_slot_cfg_type_v01, num_dl_symbols),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_slot_cfg_type_v01, num_ul_symbols),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_carrier_cfg_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_carrier_cfg_type_v01, slot_cfg),
  NAS_SLOT_CFG_MAX_V01,
  QMI_IDL_OFFSET8(nas_carrier_cfg_type_v01, slot_cfg) - QMI_IDL_OFFSET8(nas_carrier_cfg_type_v01, slot_cfg_len),
  QMI_IDL_TYPE88(0, 203),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_lte_cc_cqi_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cc_cqi_info_type_v01, cw_cqi),
  NAS_LTE_CQI_MAX_NUM_CODEWORDS_V01,
  QMI_IDL_OFFSET8(nas_lte_cc_cqi_info_type_v01, cw_cqi) - QMI_IDL_OFFSET8(nas_lte_cc_cqi_info_type_v01, cw_cqi_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_cc_cqi_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_cc_cqi_info_type_v01, cw_cqi),
  NAS_NR5G_CQI_MAX_NUM_CODEWORDS_V01,
  QMI_IDL_OFFSET8(nas_nr5g_cc_cqi_info_type_v01, cw_cqi) - QMI_IDL_OFFSET8(nas_nr5g_cc_cqi_info_type_v01, cw_cqi_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_rflte_tx_status_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_status),
  NAS_RFLM_LTE_MAX_TX_HANDLES_V01,
  QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_status) - QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_status_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_power_db10),
  NAS_RFLM_LTE_MAX_TX_HANDLES_V01,
  QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_power_db10) - QMI_IDL_OFFSET8(nas_rflte_tx_status_v01, tx_power_db10_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_traffic_profile_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_traffic_profile_info_type_v01, delay),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_traffic_profile_info_type_v01, inter_burst_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_rrc_bwp_config_s_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_config_s_v01, bwp_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_config_s_v01, scs),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_config_s_v01, loc_and_bw),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_rrc_scs_spec_carr_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_scs_spec_carr_s_v01, offset_to_carr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_scs_spec_carr_s_v01, scs),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_scs_spec_carr_s_v01, carrier_bw_prb),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_rrc_serv_cell_config_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, phy_cell_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_abs_freq_ssb),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_abs_freq_point_a),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_band),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_scs_spec_carr_list),
  NAS_NR5G_NUM_SCS_MAX_V01,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_scs_spec_carr_list) - QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_scs_spec_carr_list_len),
  QMI_IDL_TYPE88(0, 210),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_bwp_list),
  NAS_NR5G_MAX_NUM_BANDWIDTH_PARTS_SW_USE_V01,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_bwp_list) - QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, dl_bwp_list_len),
  QMI_IDL_TYPE88(0, 209),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, ul_abs_freq_point_a),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, ul_band),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, ul_scs_spec_carr_list),
  NAS_NR5G_NUM_SCS_MAX_V01,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, ul_scs_spec_carr_list) - QMI_IDL_OFFSET8(nas_nr5g_rrc_serv_cell_config_s_v01, ul_scs_spec_carr_list_len),
  QMI_IDL_TYPE88(0, 210),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_nr5g_rrc_serv_cell_config_s_v01, ul_bwp_list),
  NAS_NR5G_MAX_NUM_BANDWIDTH_PARTS_SW_USE_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_nr5g_rrc_serv_cell_config_s_v01, ul_bwp_list) - QMI_IDL_OFFSET16RELATIVE(nas_nr5g_rrc_serv_cell_config_s_v01, ul_bwp_list_len),
  QMI_IDL_TYPE88(0, 209),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_snpn_id_imsi_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_snpn_id_imsi_type_v01, id),
  QMI_IDL_TYPE88(0, 175),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_snpn_id_imsi_type_v01, imsi),
  NAS_IMSI_LEN_V01,
  QMI_IDL_OFFSET8(nas_snpn_id_imsi_type_v01, imsi) - QMI_IDL_OFFSET8(nas_snpn_id_imsi_type_v01, imsi_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_phy_channel_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_phy_channel_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_phy_channel_info_type_v01, bandwidth),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_rrc_ref_time_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_ref_time_info_s_v01, refDays),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_ref_time_info_s_v01, refSeconds),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_ref_time_info_s_v01, refMilliseconds),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_ref_time_info_s_v01, refTenNanoSeconds),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_freq_info_meas_metrics_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_freq_info_meas_metrics_type_v01, nr5g_earfcn),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_freq_info_meas_metrics_type_v01, nr5g_band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_freq_info_meas_metrics_type_v01, nr5g_subcarrier_spacing),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_freq_info_meas_metrics_type_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_freq_info_meas_metrics_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_serving_cell_freq_info_type_data_v01[] = {
  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_type_v01, band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_type_v01, start_ssb_freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_type_v01, end_ssb_freq),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_unauthorized_freq_type_data_v01[] = {
  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_type_v01, band),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_type_v01, nr5g_neigh_freq_list),
  NAS_NR5G_MAX_NEIGH_FREQS_V01,
  QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_type_v01, nr5g_neigh_freq_list) - QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_type_v01, nr5g_neigh_freq_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_tx_duty_cycle_status_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_type_v01, curr_dl_earfcn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_type_v01, curr_dl_bw),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_type_v01, bw_chunk_list_cycle_bmsk),
  NAS_MAX_TX_DUTY_CYCLE_BW_CHUNK_LIST_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_type_v01, is_suspend),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_v2x_sync_ref_ue_info_s_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_sync_ref_ue_info_s_type_v01, slss_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_sync_ref_ue_info_s_type_v01, in_coverage),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_sync_ref_ue_info_s_type_v01, sync_pattern),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_sync_ref_ue_info_s_type_v01, rsrp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_sync_ref_ue_info_s_type_v01, is_selected),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_v2x_l1_slss_rx_info_s_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_l1_slss_rx_info_s_type_v01, ue_info),
  NAS_V2X_MAX_SLSS_SYNC_REF_UE_V01,
  QMI_IDL_OFFSET8(nas_v2x_l1_slss_rx_info_s_type_v01, ue_info) - QMI_IDL_OFFSET8(nas_v2x_l1_slss_rx_info_s_type_v01, ue_info_len),
  QMI_IDL_TYPE88(0, 219),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_multi_pci_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, pci),
  NAS_MAX_PCI_ENTRIES_PER_FREQ_V01,
  QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, pci) - QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, pci_len),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, scs_mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, arfcn),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_multi_pci_info_type_v01, band),
  QMI_IDL_TYPE88(0, 24),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_nr5g_ca_sig_info_s_type_data_v01[] = {
  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, band),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, scs),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, valid_sig_strength),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_s_type_v01, rsrp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t nas_embms_area_snr_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_area_snr_type_v01, area_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_embms_area_snr_type_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * nas_reset_req_msg is empty
 * static const uint8_t nas_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_abort_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_abort_req_msg_v01, tx_id)
};

static const uint8_t nas_abort_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_abort_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_event_report_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, signal_strength) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, signal_strength_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, signal_strength),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_rf_band_info) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_rf_band_info_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_rf_band_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_reg_reject) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_reg_reject_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_reg_reject),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rssi_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rssi_indicator_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rssi_indicator),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_indicator_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_indicator),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, io_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, io_indicator_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, io_indicator),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_indicator_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_indicator),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_error_rate) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_error_rate_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, report_error_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rsrq_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rsrq_indicator_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, rsrq_indicator),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_threshold_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_threshold_indicator_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, ecio_threshold_indicator),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_threshold_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_threshold_indicator_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, sinr_threshold_indicator),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_snr_delta_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_snr_delta_indicator_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_snr_delta_indicator),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_rsrp_delta_indicator) - QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_rsrp_delta_indicator_valid)),
  0x1C,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_req_msg_v01, lte_rsrp_delta_indicator),
  QMI_IDL_TYPE88(0, 10)
};

static const uint8_t nas_set_event_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_event_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_event_report_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, signal_strength) - QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, signal_strength_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, signal_strength),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, rf_band_info_list) - QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, rf_band_info_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, rf_band_info_list),
  NAS_RADIO_IF_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, rf_band_info_list) - QMI_IDL_OFFSET8(nas_event_report_ind_msg_v01, rf_band_info_list_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, registration_reject_reason) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, registration_reject_reason_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, registration_reject_reason),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rssi) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rssi_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, rssi),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, ecio) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, ecio_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, ecio),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, io) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, io_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, io),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, sinr) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, sinr_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, sinr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, error_rate) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, error_rate_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, error_rate),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rsrq) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rsrq_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, rsrq),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, snr) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, snr_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, snr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rsrp) - QMI_IDL_OFFSET16RELATIVE(nas_event_report_ind_msg_v01, rsrp_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_event_report_ind_msg_v01, rsrp)
};

static const uint8_t nas_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sys_sel_pref) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sys_sel_pref_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sys_sel_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ddtm_events) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ddtm_events_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ddtm_events),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, req_serving_system) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, req_serving_system_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, req_serving_system),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, dual_standby_pref) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, dual_standby_pref_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, dual_standby_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, subscription_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, subscription_info_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, subscription_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_network_time) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_network_time_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_network_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sys_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sys_info_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sys_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sig_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sig_info_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, sig_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, err_rate) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, err_rate_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_uati) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_uati_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_uati),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_session_close) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_session_close_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hdr_session_close),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_managed_roaming) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_managed_roaming_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_managed_roaming),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_current_plmn_name) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_current_plmn_name_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_current_plmn_name),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_status) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_status_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_band_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_band_info_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_band_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, network_reject) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, network_reject_valid)),
  0x21,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, network_reject),
  QMI_IDL_TYPE88(0, 22),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_operator_name_data) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_operator_name_data_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_operator_name_data),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_csp_plmn_mode_bit) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_csp_plmn_mode_bit_valid)),
  0x23,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_csp_plmn_mode_bit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rtre_cfg) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rtre_cfg_valid)),
  0x24,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rtre_cfg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ims_pref_status) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ims_pref_status_valid)),
  0x25,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ims_pref_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_state_ready_status) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_state_ready_status_valid)),
  0x26,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_state_ready_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_sib16_network_time) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_sib16_network_time_valid)),
  0x27,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_sib16_network_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca_valid)),
  0x28,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_subscription_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_subscription_change_valid)),
  0x29,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_subscription_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_info_valid)),
  0x2A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emm_t3402_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emm_t3402_change_valid)),
  0x2B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emm_t3402_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_acb_info_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_acb_info_change_valid)),
  0x2C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_acb_info_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_subs_priority_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_subs_priority_change_valid)),
  0x2D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_subs_priority_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_t3346_timer_status_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_t3346_timer_status_change_valid)),
  0x2E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_t3346_timer_status_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_mode_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_mode_change_valid)),
  0x2F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_mode_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_change_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_change_info_valid)),
  0x30,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_ssac_change_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_manual_scan_fail) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_manual_scan_fail_valid)),
  0x31,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_manual_scan_fail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_expiry_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_expiry_ind_valid)),
  0x32,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_expiry_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emergency_mode_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emergency_mode_status_ind_valid)),
  0x33,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_emergency_mode_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_gcell_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_gcell_info_ind_valid)),
  0x34,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_gcell_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_edrx_change_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_edrx_change_info_ind_valid)),
  0x35,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_edrx_change_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_rach_fail_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_rach_fail_ind_valid)),
  0x36,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_rach_fail_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, lte_rrc_tx_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, lte_rrc_tx_info_ind_valid)),
  0x37,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, lte_rrc_tx_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, nas_sub_blocked_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, nas_sub_blocked_status_ind_valid)),
  0x38,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, nas_sub_blocked_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_search_fail_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_search_fail_ind_valid)),
  0x39,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_search_fail_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_status_ind_valid)),
  0x3A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_arfcn_list_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_arfcn_list_ind_valid)),
  0x3B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_arfcn_list_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_5g_modem_vote_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_5g_modem_vote_ind_valid)),
  0x3C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_5g_modem_vote_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_availability_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_availability_ind_valid)),
  0x3D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_rf_availability_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mm5g_t3502_change) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mm5g_t3502_change_valid)),
  0x3E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mm5g_t3502_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_ready_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_ready_ind_valid)),
  0x3F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_call_ready_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mmtel_resp_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mmtel_resp_ind_valid)),
  0x40,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_mmtel_resp_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_uac_barring_alleviation_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_uac_barring_alleviation_ind_valid)),
  0x41,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_uac_barring_alleviation_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_disable_iccid_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_disable_iccid_ind_valid)),
  0x42,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_disable_iccid_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sfn_threshold_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sfn_threshold_ind_valid)),
  0x43,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_sfn_threshold_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_drx_config_param_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_drx_config_param_ind_valid)),
  0x44,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_drx_config_param_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_high_capability_sub_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_high_capability_sub_ind_valid)),
  0x45,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_high_capability_sub_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_jamming_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_jamming_status_ind_valid)),
  0x46,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_jamming_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_start_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_start_ind_valid)),
  0x47,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_start_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_stop_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_stop_ind_valid)),
  0x48,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_timer_stop_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lost_sync_frame_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lost_sync_frame_ind_valid)),
  0x49,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lost_sync_frame_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_scan_fail_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_scan_fail_ind_valid)),
  0x4A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_scan_fail_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_disable_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_disable_status_ind_valid)),
  0x4B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_disable_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cell_barring_info) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cell_barring_info_valid)),
  0x4C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cell_barring_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_barring_status) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_barring_status_valid)),
  0x4D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_barring_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_panel_pos_change_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_panel_pos_change_ind_valid)),
  0x4E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_panel_pos_change_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_time_sync_pulse_report_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_time_sync_pulse_report_ind_valid)),
  0x4F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_time_sync_pulse_report_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_lost_sync_frame_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_lost_sync_frame_ind_valid)),
  0x50,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_lost_sync_frame_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_wwan_sleep_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_wwan_sleep_info_ind_valid)),
  0x51,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_wwan_sleep_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_conn_reject_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_conn_reject_ind_valid)),
  0x52,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_conn_reject_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_resource_config_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_resource_config_ind_valid)),
  0x53,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_resource_config_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca_aggregated_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca_aggregated_ind_valid)),
  0x54,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_lte_cphy_ca_aggregated_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_freq_out_of_band_range_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_freq_out_of_band_range_ind_valid)),
  0x55,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_freq_out_of_band_range_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_tx_status_report_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_tx_status_report_ind_valid)),
  0x56,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_tx_status_report_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_alleviation_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_alleviation_ind_valid)),
  0x57,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_access_alleviation_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nw_congestion_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nw_congestion_ind_valid)),
  0x58,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nw_congestion_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hst_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hst_info_ind_valid)),
  0x59,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_hst_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_enable_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_enable_ind_valid)),
  0x5A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_data_enable_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_msim_sub_mode_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_msim_sub_mode_ind_valid)),
  0x5B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_msim_sub_mode_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_vonr_not_possible_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_vonr_not_possible_ind_valid)),
  0x5C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_vonr_not_possible_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_bwp_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_bwp_info_ind_valid)),
  0x5D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_bwp_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_utc_time_update_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_utc_time_update_ind_valid)),
  0x5E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_rrc_utc_time_update_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_meas_metrics_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_meas_metrics_ind_valid)),
  0x5F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_meas_metrics_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_mode_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_mode_status_ind_valid)),
  0x60,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_cpe_install_mode_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_sms_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_sms_status_ind_valid)),
  0x61,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_e911_sms_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_serving_cell_freq_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_serving_cell_freq_info_ind_valid)),
  0x62,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_serving_cell_freq_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_unauthorized_freq_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_unauthorized_freq_ind_valid)),
  0x63,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_unauthorized_freq_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_tx_duty_cycle_status_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_tx_duty_cycle_status_ind_valid)),
  0x64,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_tx_duty_cycle_status_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_slss_rx_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_slss_rx_info_ind_valid)),
  0x65,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_v2x_slss_rx_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_ca_sig_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_ca_sig_info_ind_valid)),
  0x66,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_nr5g_ca_sig_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_sig_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_sig_info_ind_valid)),
  0x67,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_embms_sig_info_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_stop_embms_sig_info_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_stop_embms_sig_info_ind_valid)),
  0x68,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_stop_embms_sig_info_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_profile_switch_ind) - QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_profile_switch_ind_valid)),
  0x69,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_indication_register_req_msg_v01, reg_profile_switch_ind)
};

static const uint8_t nas_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_signal_strength_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_req_msg_v01, request_mask) - QMI_IDL_OFFSET8(nas_get_signal_strength_req_msg_v01, request_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_req_msg_v01, request_mask)
};

static const uint8_t nas_get_signal_strength_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength_list) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength_list),
  ((NAS_SIG_STRENGTH_LIST_MAX_V01) & 0xFF), ((NAS_SIG_STRENGTH_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength_list) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, signal_strength_list_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, rssi) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, rssi_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, rssi),
  ((NAS_RSSI_LIST_MAX_V01) & 0xFF), ((NAS_RSSI_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, rssi) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, rssi_len),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, ecio) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, ecio_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, ecio),
  ((NAS_ECIO_LIST_MAX_V01) & 0xFF), ((NAS_ECIO_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, ecio) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, ecio_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, io) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, io_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, io),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, sinr) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, sinr_valid)),
  0x14,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, sinr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, error_rate) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, error_rate_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, error_rate),
  ((NAS_ERROR_RATE_LIST_MAX_V01) & 0xFF), ((NAS_ERROR_RATE_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, error_rate) - QMI_IDL_OFFSET8(nas_get_signal_strength_resp_msg_v01, error_rate_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, rsrq) - QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, rsrq_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_signal_strength_resp_msg_v01, rsrq),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, snr) - QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, snr_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_signal_strength_resp_msg_v01, snr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, lte_rsrp) - QMI_IDL_OFFSET16RELATIVE(nas_get_signal_strength_resp_msg_v01, lte_rsrp_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_signal_strength_resp_msg_v01, lte_rsrp)
};

static const uint8_t nas_perform_network_scan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, network_type) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, network_type_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, network_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_type) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, band_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, band_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, tdscdma_band_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, tdscdma_band_pref_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, tdscdma_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, ciot_lte_op_mode_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, ciot_lte_op_mode_pref_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, ciot_lte_op_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref_valid)),
  0x16,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref_valid)),
  0x17,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref_ext) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref_ext_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_m1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref_ext) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref_ext_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_nb1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_scope) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_scope_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, scan_scope),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref_ext) - QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref_ext_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_req_msg_v01, lte_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_req_msg_v01, lte_nb1_ntn_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_req_msg_v01, lte_nb1_ntn_band_pref_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_req_msg_v01, lte_nb1_ntn_band_pref),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t nas_perform_network_scan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, nas_3gpp_network_info) - QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, nas_3gpp_network_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, nas_3gpp_network_info),
  ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, nas_3gpp_network_info) - QMI_IDL_OFFSET8(nas_perform_network_scan_resp_msg_v01, nas_3gpp_network_info_len),
  QMI_IDL_TYPE88(0, 26),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nas_network_radio_access_technology) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nas_network_radio_access_technology_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, nas_network_radio_access_technology),
  ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nas_network_radio_access_technology) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nas_network_radio_access_technology_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, mnc_includes_pcs_digit_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, mnc_includes_pcs_digit),
  ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, mnc_includes_pcs_digit_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, scan_result) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, scan_result_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, scan_result),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_info_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, csg_info),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_info_len),
  QMI_IDL_TYPE88(0, 33),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_sig_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_sig_info_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, csg_sig_info),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_sig_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, csg_sig_info_len),
  QMI_IDL_TYPE88(0, 35),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nw_name_source_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, nw_name_source),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, nw_name_source_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, pci_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, pci_plmn_info_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, pci_plmn_info),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_op_mode) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_op_mode_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, lte_op_mode),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_op_mode) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_op_mode_len),
  QMI_IDL_TYPE88(0, 28),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, geran_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, geran_cell_search_info_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, geran_cell_search_info),
  NAS_CELL_SEARCH_MAX_NUM_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, geran_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, geran_cell_search_info_len),
  QMI_IDL_TYPE88(0, 39),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_m1_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_m1_cell_search_info_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, lte_m1_cell_search_info),
  NAS_CELL_SEARCH_MAX_NUM_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_m1_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_m1_cell_search_info_len),
  QMI_IDL_TYPE88(0, 40),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_nb1_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_nb1_cell_search_info_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, lte_nb1_cell_search_info),
  NAS_CELL_SEARCH_MAX_NUM_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_nb1_cell_search_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, lte_nb1_cell_search_info_len),
  QMI_IDL_TYPE88(0, 40),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, sib2_upr_layer_ind_value) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, sib2_upr_layer_ind_value_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_network_scan_resp_msg_v01, sib2_upr_layer_ind_value),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, sib2_upr_layer_ind_value) - QMI_IDL_OFFSET16RELATIVE(nas_perform_network_scan_resp_msg_v01, sib2_upr_layer_ind_value_len)
};

static const uint8_t nas_initiate_network_register_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, register_action),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, manual_network_register_info) - QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, manual_network_register_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, manual_network_register_info),
  QMI_IDL_TYPE88(0, 41),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, change_duration) - QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, change_duration_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, change_duration),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, mnc_includes_pcs_digit_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_initiate_network_register_req_msg_v01, mnc_includes_pcs_digit)
};

static const uint8_t nas_initiate_network_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_initiate_network_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_initiate_attach_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_initiate_attach_req_msg_v01, ps_attach_action) - QMI_IDL_OFFSET8(nas_initiate_attach_req_msg_v01, ps_attach_action_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_initiate_attach_req_msg_v01, ps_attach_action)
};

static const uint8_t nas_initiate_attach_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_initiate_attach_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_serving_system_req_msg is empty
 * static const uint8_t nas_get_serving_system_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_serving_system_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_serving_system_resp_msg_v01, serving_system),
  QMI_IDL_TYPE88(0, 42),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, roaming_indicator),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, data_capabilities) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, data_capabilities_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, data_capabilities),
  NAS_DATA_CAPABILITIES_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, data_capabilities) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, data_capabilities_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, current_plmn) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, current_plmn_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, current_plmn),
  QMI_IDL_TYPE88(0, 43),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_system_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_system_id_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, cdma_system_id),
  QMI_IDL_TYPE88(0, 44),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_base_station_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_base_station_info_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, cdma_base_station_info),
  QMI_IDL_TYPE88(0, 45),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, roaming_indicator_list),
  NAS_ROAMING_INDICATOR_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, roaming_indicator_list_len),
  QMI_IDL_TYPE88(0, 46),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, def_roam_ind) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, def_roam_ind_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, def_roam_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, nas_3gpp_time_zone) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, nas_3gpp_time_zone_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, nas_3gpp_time_zone),
  QMI_IDL_TYPE88(0, 47),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, p_rev_in_use) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, p_rev_in_use_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, p_rev_in_use),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, time_zone) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, time_zone_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, time_zone),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, adj) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, adj_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, adj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, lac) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, lac_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, lac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cell_id_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, cell_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, ccs) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, ccs_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, ccs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, prl_ind) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, prl_ind_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, prl_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, dtm_ind) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, dtm_ind_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, dtm_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, detailed_service_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, detailed_service_info_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, detailed_service_info),
  QMI_IDL_TYPE88(0, 48),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_system_id_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, cdma_system_id_ext_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, cdma_system_id_ext),
  QMI_IDL_TYPE88(0, 49),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, hdr_personality) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, hdr_personality_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, hdr_personality),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, tac) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, tac_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, tac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, call_barring_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, call_barring_status_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, call_barring_status),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, umts_psc) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, umts_psc_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, umts_psc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, mnc_includes_pcs_digit_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, mnc_includes_pcs_digit),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, hs_call_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, hs_call_status_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, hs_call_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, nas_3gpp_nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_get_serving_system_resp_msg_v01, nas_3gpp_nw_name_source_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_serving_system_resp_msg_v01, nas_3gpp_nw_name_source)
};

static const uint8_t nas_serving_system_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_serving_system_ind_msg_v01, serving_system),
  QMI_IDL_TYPE88(0, 42),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, roaming_indicator),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, data_capabilities) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, data_capabilities_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, data_capabilities),
  NAS_DATA_CAPABILITIES_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, data_capabilities) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, data_capabilities_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, current_plmn) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, current_plmn_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, current_plmn),
  QMI_IDL_TYPE88(0, 43),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_system_id) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_system_id_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, cdma_system_id),
  QMI_IDL_TYPE88(0, 44),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_base_station_info) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_base_station_info_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, cdma_base_station_info),
  QMI_IDL_TYPE88(0, 45),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator_list) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, roaming_indicator_list),
  NAS_ROAMING_INDICATOR_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator_list) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, roaming_indicator_list_len),
  QMI_IDL_TYPE88(0, 46),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, def_roam_ind) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, def_roam_ind_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, def_roam_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, nas_3gpp_time_zone) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, nas_3gpp_time_zone_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, nas_3gpp_time_zone),
  QMI_IDL_TYPE88(0, 47),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, p_rev_in_use) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, p_rev_in_use_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, p_rev_in_use),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, plmn_description_changed) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, plmn_description_changed_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, plmn_description_changed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, time_zone) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, time_zone_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, time_zone),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, adj) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, adj_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, adj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, universal_time_and_local_time_3gpp_zone) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, universal_time_and_local_time_3gpp_zone_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, universal_time_and_local_time_3gpp_zone),
  QMI_IDL_TYPE88(0, 51),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, lac) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, lac_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, lac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cell_id_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, cell_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, ccs) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, ccs_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, ccs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, prl_ind) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, prl_ind_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, prl_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, dtm_ind) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, dtm_ind_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, dtm_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, detailed_service_info) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, detailed_service_info_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, detailed_service_info),
  QMI_IDL_TYPE88(0, 48),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_system_id_ext) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, cdma_system_id_ext_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, cdma_system_id_ext),
  QMI_IDL_TYPE88(0, 49),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, hdr_personality) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, hdr_personality_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, hdr_personality),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, tac) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, tac_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, tac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, call_barring_status) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, call_barring_status_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, call_barring_status),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, srv_sys_no_change) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, srv_sys_no_change_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, srv_sys_no_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, umts_psc) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, umts_psc_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, umts_psc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, mnc_includes_pcs_digit_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, mnc_includes_pcs_digit),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, hs_call_status) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, hs_call_status_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, hs_call_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, nas_3gpp_nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_serving_system_ind_msg_v01, nas_3gpp_nw_name_source_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_serving_system_ind_msg_v01, nas_3gpp_nw_name_source)
};

/*
 * nas_get_home_network_req_msg is empty
 * static const uint8_t nas_get_home_network_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_home_network_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_home_network_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_home_network_resp_msg_v01, home_network),
  QMI_IDL_TYPE88(0, 43),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, home_system_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, home_system_id_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_home_network_resp_msg_v01, home_system_id),
  QMI_IDL_TYPE88(0, 44),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp2_home_network_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp2_home_network_ext_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_home_network_resp_msg_v01, nas_3gpp2_home_network_ext),
  QMI_IDL_TYPE88(0, 52),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp_mcs_include_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp_mcs_include_digit_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_home_network_resp_msg_v01, nas_3gpp_mcs_include_digit),
  QMI_IDL_TYPE88(0, 53),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp_nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_get_home_network_resp_msg_v01, nas_3gpp_nw_name_source_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_home_network_resp_msg_v01, nas_3gpp_nw_name_source)
};

/*
 * nas_get_preferred_networks_req_msg is empty
 * static const uint8_t nas_get_preferred_networks_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_preferred_networks_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks) - QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks),
  ((NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks) - QMI_IDL_OFFSET8(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_len),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_preferred_networks) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_preferred_networks_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_preferred_networks_resp_msg_v01, static_3gpp_preferred_networks),
  ((NAS_STATIC_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) & 0xFF), ((NAS_STATIC_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_preferred_networks) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_preferred_networks_len),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit),
  NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_mnc_includes_pcs_digit_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_preferred_networks_resp_msg_v01, static_3gpp_mnc_includes_pcs_digit),
  NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, static_3gpp_mnc_includes_pcs_digit_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_ext_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_ext),
  NAS_3GPP_PREFERRED_NETWORKS_EXT_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_preferred_networks_ext_len),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext),
  NAS_3GPP_PREFERRED_NETWORKS_EXT_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_preferred_networks_resp_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext_len),
  QMI_IDL_TYPE88(0, 29)
};

static const uint8_t nas_set_preferred_networks_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks) - QMI_IDL_OFFSET8(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks),
  ((NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks) - QMI_IDL_OFFSET8(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_len),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit),
  NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, clear_prev_preferred_networks) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, clear_prev_preferred_networks_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_preferred_networks_req_msg_v01, clear_prev_preferred_networks),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_ext_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_ext),
  NAS_3GPP_PREFERRED_NETWORKS_EXT_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_preferred_networks_ext_len),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext),
  NAS_3GPP_PREFERRED_NETWORKS_EXT_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_preferred_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_ext_len),
  QMI_IDL_TYPE88(0, 29)
};

static const uint8_t nas_set_preferred_networks_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_preferred_networks_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_sim_overdue_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_sim_overdue_status_req_msg_v01, is_sim_overdue) - QMI_IDL_OFFSET8(nas_set_sim_overdue_status_req_msg_v01, is_sim_overdue_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_sim_overdue_status_req_msg_v01, is_sim_overdue)
};

static const uint8_t nas_set_sim_overdue_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_sim_overdue_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_codebook_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_codebook_config_req_msg_v01, codebook_config_index)
};

static const uint8_t nas_set_codebook_config_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_codebook_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_codebook_config_req_msg is empty
 * static const uint8_t nas_get_codebook_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_codebook_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_codebook_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_codebook_config_rsp_msg_v01, codebook_config_index) - QMI_IDL_OFFSET8(nas_get_codebook_config_rsp_msg_v01, codebook_config_index_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_codebook_config_rsp_msg_v01, codebook_config_index)
};

static const uint8_t nas_set_nr5g_band_priority_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_nr5g_band_priority_req_msg_v01, band_priority_list),
  ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) & 0xFF), ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_set_nr5g_band_priority_req_msg_v01, band_priority_list) - QMI_IDL_OFFSET8(nas_set_nr5g_band_priority_req_msg_v01, band_priority_list_len)
};

static const uint8_t nas_set_nr5g_band_priority_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_band_priority_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nr5g_band_priority_list_req_msg is empty
 * static const uint8_t nas_get_nr5g_band_priority_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_band_priority_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, configured_band_priority_list) - QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, configured_band_priority_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, configured_band_priority_list),
  ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) & 0xFF), ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, configured_band_priority_list) - QMI_IDL_OFFSET8(nas_get_nr5g_band_priority_list_resp_msg_v01, configured_band_priority_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_band_priority_list_resp_msg_v01, supported_band_priority_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_band_priority_list_resp_msg_v01, supported_band_priority_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_nr5g_band_priority_list_resp_msg_v01, supported_band_priority_list),
  ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) & 0xFF), ((NAS_NR5G_BAND_PRIORITY_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_band_priority_list_resp_msg_v01, supported_band_priority_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_band_priority_list_resp_msg_v01, supported_band_priority_list_len)
};

/*
 * nas_get_forbidden_networks_req_msg is empty
 * static const uint8_t nas_get_forbidden_networks_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_forbidden_networks_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, nas_3gpp_forbidden_networks) - QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, nas_3gpp_forbidden_networks_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, nas_3gpp_forbidden_networks),
  ((NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, nas_3gpp_forbidden_networks) - QMI_IDL_OFFSET8(nas_get_forbidden_networks_resp_msg_v01, nas_3gpp_forbidden_networks_len),
  QMI_IDL_TYPE88(0, 55)
};

static const uint8_t nas_set_forbidden_networks_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_forbidden_networks) - QMI_IDL_OFFSET8(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_forbidden_networks_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_forbidden_networks),
  ((NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_forbidden_networks) - QMI_IDL_OFFSET8(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_forbidden_networks_len),
  QMI_IDL_TYPE88(0, 55),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit),
  NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit) - QMI_IDL_OFFSET16RELATIVE(nas_set_forbidden_networks_req_msg_v01, nas_3gpp_mnc_includes_pcs_digit_len)
};

static const uint8_t nas_set_forbidden_networks_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_forbidden_networks_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_accolc_req_msg is empty
 * static const uint8_t nas_get_accolc_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_accolc_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_accolc_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_accolc_resp_msg_v01, accolc)
};

static const uint8_t nas_set_accolc_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_accolc_req_msg_v01, accolc_set),
  QMI_IDL_TYPE88(0, 56)
};

static const uint8_t nas_set_accolc_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_accolc_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_device_config_req_msg is empty
 * static const uint8_t nas_get_device_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_device_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, sci) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, sci_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, sci),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, scm) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, scm_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, scm),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, registration_parameters) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, registration_parameters_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, registration_parameters),
  QMI_IDL_TYPE88(0, 57),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_rev0) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_rev0_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_rev0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, hdr_scp_config) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, hdr_scp_config_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, hdr_scp_config),
  QMI_IDL_TYPE88(0, 58),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, roam_pref) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, roam_pref_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, roam_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_hdrscp_config_at) - QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_hdrscp_config_at_valid)),
  0x17,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_device_config_resp_msg_v01, force_hdrscp_config_at)
};

static const uint8_t nas_set_device_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, spc) - QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, spc_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, spc),
  NAS_SPC_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, force_hdr_rev0) - QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, force_hdr_rev0_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, force_hdr_rev0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, hdr_scp_config) - QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, hdr_scp_config_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, hdr_scp_config),
  QMI_IDL_TYPE88(0, 58),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, roam_pref) - QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, roam_pref_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, roam_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, gpp_wifi_cellular_scan_timer) - QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, gpp_wifi_cellular_scan_timer_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_device_config_req_msg_v01, gpp_wifi_cellular_scan_timer)
};

static const uint8_t nas_set_device_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_device_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_rf_band_info_req_msg is empty
 * static const uint8_t nas_get_rf_band_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_rf_band_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_rf_band_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list),
  NAS_RF_BAND_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list) - QMI_IDL_OFFSET8(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_dedicated_band_info_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_dedicated_band_info_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_rf_band_info_resp_msg_v01, rf_dedicated_band_info_list),
  NAS_RF_BAND_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_dedicated_band_info_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_dedicated_band_info_list_len),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_ext_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_ext),
  NAS_RF_BAND_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, rf_band_info_list_ext_len),
  QMI_IDL_TYPE88(0, 59),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, nas_rf_bandwidth_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, nas_rf_bandwidth_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_rf_band_info_resp_msg_v01, nas_rf_bandwidth_info),
  NAS_RF_BAND_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, nas_rf_bandwidth_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, nas_rf_bandwidth_info_len),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET16RELATIVE(nas_get_rf_band_info_resp_msg_v01, ciot_lte_op_mode_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_rf_band_info_resp_msg_v01, ciot_lte_op_mode)
};

/*
 * nas_get_an_aaa_status_req_msg is empty
 * static const uint8_t nas_get_an_aaa_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_an_aaa_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_an_aaa_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_an_aaa_status_resp_msg_v01, an_aaa_status)
};

static const uint8_t nas_set_system_selection_preference_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, emergency_mode) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, emergency_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, emergency_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mode_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mode_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, band_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, band_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, prl_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, prl_pref_valid)),
  0x13,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, prl_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, roam_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, roam_pref_valid)),
  0x14,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, roam_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, net_sel_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, net_sel_pref_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, net_sel_pref),
  QMI_IDL_TYPE88(0, 60),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, change_duration) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, change_duration_valid)),
  0x17,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, change_duration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mnc_includes_pcs_digit_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, mnc_includes_pcs_digit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, gw_acq_order_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, tdscdma_band_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, tdscdma_band_pref_valid)),
  0x1D,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, tdscdma_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, acq_order_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_reg_restriction) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_reg_restriction_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, srv_reg_restriction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, csg_info) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, csg_info_valid)),
  0x20,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, csg_info),
  QMI_IDL_TYPE88(0, 61),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, usage_setting) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, usage_setting_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, usage_setting),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, rat) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, rat_valid)),
  0x22,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, voice_domain_pref) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, voice_domain_pref_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, voice_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref_ext) - QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref_ext_valid)),
  0x24,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_req_msg_v01, lte_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, force) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, force_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, force),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_lte_op_mode_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_lte_op_mode_pref_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, ciot_lte_op_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, man_ciot_lte_mode) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, man_ciot_lte_mode_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, man_ciot_lte_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_acq_order_valid)),
  0x2A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, ciot_acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, ciot_acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_band_pref_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nr5g_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref_ext_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, lte_m1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref_ext_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, lte_nb1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_disable_mode) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_disable_mode_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nr5g_disable_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_sa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_sa_band_pref_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nr5g_sa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_nsa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nr5g_nsa_band_pref_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nr5g_nsa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, persistent_pref_mask) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, persistent_pref_mask_valid)),
  0x35,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, persistent_pref_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nrdc_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nrdc_band_pref_valid)),
  0x36,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nrdc_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nrdc_mode) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, nrdc_mode_valid)),
  0x37,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, nrdc_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, cag_id) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, cag_id_valid)),
  0x38,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, cag_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, access_mode) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, access_mode_valid)),
  0x39,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, access_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, snpn_nid) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, snpn_nid_valid)),
  0x3A,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, snpn_nid),
  NAS_SNPN_NID_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_ntn_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_set_system_selection_preference_req_msg_v01, lte_nb1_ntn_band_pref_valid)),
  0x3B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_system_selection_preference_req_msg_v01, lte_nb1_ntn_band_pref),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t nas_set_system_selection_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_system_selection_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_system_selection_preference_req_msg is empty
 * static const uint8_t nas_get_system_selection_preference_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_system_selection_preference_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, emergency_mode) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, emergency_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, emergency_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, mode_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, mode_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, prl_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, prl_pref_valid)),
  0x13,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, prl_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, roam_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, roam_pref_valid)),
  0x14,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, roam_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref_ext) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref_ext_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, band_pref_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, net_sel_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, net_sel_pref_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, net_sel_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_domain_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_domain_pref_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, gw_acq_order_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, gw_acq_order_pref_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, gw_acq_order_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, tdscdma_band_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, tdscdma_band_pref_valid)),
  0x1A,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, tdscdma_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, manual_net_sel_plmn) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, manual_net_sel_plmn_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, manual_net_sel_plmn),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, acq_order_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_reg_restriction) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_reg_restriction_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, srv_reg_restriction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, csg_info) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, csg_info_valid)),
  0x1E,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, csg_info),
  QMI_IDL_TYPE88(0, 61),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, usage_setting) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, usage_setting_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, usage_setting),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, voice_domain_pref) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, voice_domain_pref_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, voice_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_disable_cause) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_disable_cause_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_disable_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, rat_disabled_mask) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, rat_disabled_mask_valid)),
  0x22,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, rat_disabled_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_band_pref_ext) - QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_band_pref_ext_valid)),
  0x23,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_system_selection_preference_resp_msg_v01, lte_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_lte_op_mode_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_lte_op_mode_pref_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, ciot_lte_op_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_acq_order_valid)),
  0x27,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, ciot_acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, ciot_acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_band_pref_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nr5g_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref_ext_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, lte_m1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref_ext_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_disable_mode) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_disable_mode_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nr5g_disable_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_sa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_sa_band_pref_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nr5g_sa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_nsa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nr5g_nsa_band_pref_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nr5g_nsa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nrdc_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nrdc_band_pref_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nrdc_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nrdc_mode) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, nrdc_mode_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, nrdc_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, access_mode) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, access_mode_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, access_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, snpn_nid) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, snpn_nid_valid)),
  0x31,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, snpn_nid),
  NAS_SNPN_NID_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_ntn_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_ntn_band_pref_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_system_selection_preference_resp_msg_v01, lte_nb1_ntn_band_pref),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t nas_system_selection_preference_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, emergency_mode) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, emergency_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, emergency_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, mode_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, mode_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, band_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, band_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, prl_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, prl_pref_valid)),
  0x13,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, prl_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, roam_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, roam_pref_valid)),
  0x14,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, roam_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, net_sel_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, net_sel_pref_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, net_sel_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_domain_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_domain_pref_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, gw_acq_order_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, gw_acq_order_pref_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, gw_acq_order_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, tdscdma_band_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, tdscdma_band_pref_valid)),
  0x1A,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, tdscdma_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, manual_net_sel_plmn) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, manual_net_sel_plmn_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, manual_net_sel_plmn),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, acq_order_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, acq_order) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_reg_restriction) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_reg_restriction_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, srv_reg_restriction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, csg_info) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, csg_info_valid)),
  0x1E,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, csg_info),
  QMI_IDL_TYPE88(0, 61),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, usage_setting) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, usage_setting_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, usage_setting),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, voice_domain_pref) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, voice_domain_pref_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, voice_domain_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_disable_cause) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_disable_cause_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_disable_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, rat_disabled_mask) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, rat_disabled_mask_valid)),
  0x22,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, rat_disabled_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref_ext) - QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref_ext_valid)),
  0x23,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_system_selection_preference_ind_msg_v01, lte_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_lte_op_mode_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_lte_op_mode_pref_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, ciot_lte_op_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_acq_order_valid)),
  0x27,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, ciot_acq_order),
  NAS_ACQ_ORDER_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_acq_order) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, ciot_acq_order_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_band_pref_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nr5g_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref_ext_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, lte_m1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref_ext_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, lte_nb1_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_disable_mode) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_disable_mode_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nr5g_disable_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_sa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_sa_band_pref_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nr5g_sa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_nsa_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nr5g_nsa_band_pref_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nr5g_nsa_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nrdc_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nrdc_band_pref_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nrdc_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nrdc_mode) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, nrdc_mode_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, nrdc_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, access_mode) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, access_mode_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, access_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, snpn_nid) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, snpn_nid_valid)),
  0x31,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, snpn_nid),
  NAS_SNPN_NID_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_ntn_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_system_selection_preference_ind_msg_v01, lte_nb1_ntn_band_pref_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_system_selection_preference_ind_msg_v01, lte_nb1_ntn_band_pref),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t nas_set_ddtm_preference_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_ddtm_preference_req_msg_v01, ddtm_preference),
  QMI_IDL_TYPE88(0, 62)
};

static const uint8_t nas_set_ddtm_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_ddtm_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_ddtm_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ddtm_ind_msg_v01, ddtm_settings),
  QMI_IDL_TYPE88(0, 63)
};

/*
 * nas_e911_scan_fail_ind_msg is empty
 * static const uint8_t nas_e911_scan_fail_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_nr5g_freq_out_of_band_range_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_freq_out_of_band_range_ind_msg_v01, nr5g_band) - QMI_IDL_OFFSET8(nas_nr5g_freq_out_of_band_range_ind_msg_v01, nr5g_band_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_nr5g_freq_out_of_band_range_ind_msg_v01, nr5g_band)
};

/*
 * nas_get_operator_name_data_req_msg is empty
 * static const uint8_t nas_get_operator_name_data_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_operator_name_data_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, service_provider_name) - QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, service_provider_name_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, service_provider_name),
  QMI_IDL_TYPE88(0, 64),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, operator_plmn_list) - QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, operator_plmn_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, operator_plmn_list),
  ((NAS_OPERATOR_PLMN_LIST_MAX_V01) & 0xFF), ((NAS_OPERATOR_PLMN_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, operator_plmn_list) - QMI_IDL_OFFSET8(nas_get_operator_name_data_resp_msg_v01, operator_plmn_list_len),
  QMI_IDL_TYPE88(0, 65),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_network_name) - QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_network_name_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_operator_name_data_resp_msg_v01, plmn_network_name),
  NAS_PLMN_NETWORK_NAME_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_network_name) - QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_network_name_len),
  QMI_IDL_TYPE88(0, 66),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_name) - QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, plmn_name_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(nas_get_operator_name_data_resp_msg_v01, plmn_name),
  NAS_PLMN_NAME_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, nitz_information) - QMI_IDL_OFFSET16RELATIVE(nas_get_operator_name_data_resp_msg_v01, nitz_information_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_operator_name_data_resp_msg_v01, nitz_information),
  QMI_IDL_TYPE88(0, 66)
};

static const uint8_t nas_operator_name_data_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, service_provider_name) - QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, service_provider_name_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, service_provider_name),
  QMI_IDL_TYPE88(0, 64),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, operator_plmn_list) - QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, operator_plmn_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, operator_plmn_list),
  ((NAS_OPERATOR_PLMN_LIST_MAX_V01) & 0xFF), ((NAS_OPERATOR_PLMN_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, operator_plmn_list) - QMI_IDL_OFFSET8(nas_operator_name_data_ind_msg_v01, operator_plmn_list_len),
  QMI_IDL_TYPE88(0, 65),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_network_name) - QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_network_name_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_operator_name_data_ind_msg_v01, plmn_network_name),
  NAS_PLMN_NETWORK_NAME_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_network_name) - QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_network_name_len),
  QMI_IDL_TYPE88(0, 66),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_name) - QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, plmn_name_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(nas_operator_name_data_ind_msg_v01, plmn_name),
  NAS_PLMN_NAME_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, nitz_information) - QMI_IDL_OFFSET16RELATIVE(nas_operator_name_data_ind_msg_v01, nitz_information_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_operator_name_data_ind_msg_v01, nitz_information),
  QMI_IDL_TYPE88(0, 66)
};

/*
 * nas_get_csp_plmn_mode_bit_req_msg is empty
 * static const uint8_t nas_get_csp_plmn_mode_bit_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_csp_plmn_mode_bit_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_csp_plmn_mode_bit_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csp_plmn_mode_bit_resp_msg_v01, plmn_mode) - QMI_IDL_OFFSET8(nas_get_csp_plmn_mode_bit_resp_msg_v01, plmn_mode_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_csp_plmn_mode_bit_resp_msg_v01, plmn_mode)
};

static const uint8_t nas_csp_plmn_mode_bit_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csp_plmn_mode_bit_ind_msg_v01, plmn_mode) - QMI_IDL_OFFSET8(nas_csp_plmn_mode_bit_ind_msg_v01, plmn_mode_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_csp_plmn_mode_bit_ind_msg_v01, plmn_mode)
};

static const uint8_t nas_update_akey_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_update_akey_req_msg_v01, akey),
  26
};

static const uint8_t nas_update_akey_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_akey_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_3gpp2_subscription_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_req_msg_v01, nam_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_req_msg_v01, get_3gpp2_info_mask) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_req_msg_v01, get_3gpp2_info_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_req_msg_v01, get_3gpp2_info_mask)
};

static const uint8_t nas_get_3gpp2_subscription_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, nam_name) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, nam_name_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, nam_name),
  NAS_MAX_NAM_NAME_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, nam_name) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, nam_name_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, dir_num) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, dir_num_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, dir_num),
  NAS_MAX_3GPP2_SUBS_INFO_DIR_NUM_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, dir_num) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, dir_num_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_sys_id) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_sys_id_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_sys_id),
  NAS_MAX_3GPP2_HOME_SID_NID_NUM_V01,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_sys_id) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_sys_id_len),
  QMI_IDL_TYPE88(0, 68),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, min_based_info) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, min_based_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, min_based_info),
  QMI_IDL_TYPE88(0, 69),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, true_imsi) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, true_imsi_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, true_imsi),
  QMI_IDL_TYPE88(0, 70),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_channel_info) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_channel_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, cdma_channel_info),
  QMI_IDL_TYPE88(0, 71),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, mdn) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, mdn_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, mdn),
  MDN_MAX_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, mdn) - QMI_IDL_OFFSET8(nas_get_3gpp2_subscription_info_resp_msg_v01, mdn_len)
};

static const uint8_t nas_set_3gpp2_subscription_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, dir_num) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, dir_num_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, dir_num),
  NAS_MAX_3GPP2_SUBS_INFO_DIR_NUM_LEN_V01,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, dir_num) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, dir_num_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_sys_id) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_sys_id_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_sys_id),
  NAS_MAX_3GPP2_HOME_SID_NID_NUM_V01,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_sys_id) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_sys_id_len),
  QMI_IDL_TYPE88(0, 68),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, min_based_info) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, min_based_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, min_based_info),
  QMI_IDL_TYPE88(0, 69),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, true_imsi) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, true_imsi_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, true_imsi),
  QMI_IDL_TYPE88(0, 70),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_channel_info) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_channel_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, cdma_channel_info),
  QMI_IDL_TYPE88(0, 71),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_name) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_name_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_name),
  NAS_MAX_NAM_NAME_LEN_V01,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_name) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, nam_name_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, mdn) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, mdn_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, mdn),
  MDN_MAX_LEN_V01,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, mdn) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, mdn_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, spc) - QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, spc_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_req_msg_v01, spc),
  NAS_SPC_MAX_V01
};

static const uint8_t nas_set_3gpp2_subscription_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_3gpp2_subscription_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_mob_cai_rev_req is empty
 * static const uint8_t nas_get_mob_cai_rev_req_data_v01[] = {
 * };
 */

static const uint8_t nas_get_mob_cai_rev_resp_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_mob_cai_rev_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_mob_cai_rev_resp_v01, cai_rev) - QMI_IDL_OFFSET8(nas_get_mob_cai_rev_resp_v01, cai_rev_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_mob_cai_rev_resp_v01, cai_rev)
};

/*
 * nas_get_rtre_config_req is empty
 * static const uint8_t nas_get_rtre_config_req_data_v01[] = {
 * };
 */

static const uint8_t nas_get_rtre_config_resp_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg) - QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg_pref) - QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg_pref_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_rtre_config_resp_v01, rtre_cfg_pref)
};

static const uint8_t nas_set_rtre_config_req_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_rtre_config_req_v01, rtre_cfg_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_rtre_config_req_v01, spc) - QMI_IDL_OFFSET8(nas_set_rtre_config_req_v01, spc_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_rtre_config_req_v01, spc),
  NAS_SPC_MAX_V01
};

static const uint8_t nas_set_rtre_config_resp_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rtre_config_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_cell_location_info_req_msg is empty
 * static const uint8_t nas_get_cell_location_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cell_location_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, geran_info) - QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, geran_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, geran_info),
  QMI_IDL_TYPE88(0, 73),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, umts_info) - QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, umts_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_location_info_resp_msg_v01, umts_info),
  QMI_IDL_TYPE88(0, 77),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cdma_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cdma_info_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, cdma_info),
  QMI_IDL_TYPE88(0, 78),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_intra) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_intra_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_intra),
  QMI_IDL_TYPE88(0, 80),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_inter),
  QMI_IDL_TYPE88(0, 83),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_gsm) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_gsm_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_gsm),
  QMI_IDL_TYPE88(0, 87),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_wcdma) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_wcdma_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_wcdma),
  QMI_IDL_TYPE88(0, 90),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, umts_cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, umts_cell_id_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, umts_cell_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_lte) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_lte_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, wcdma_lte),
  QMI_IDL_TYPE88(0, 91),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cdma_rx_power) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cdma_rx_power_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, cdma_rx_power),
  QMI_IDL_TYPE88(0, 92),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, hdr_rx_power) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, hdr_rx_power_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, hdr_rx_power),
  QMI_IDL_TYPE88(0, 92),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gsm_info_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gsm_info_ext_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, gsm_info_ext),
  QMI_IDL_TYPE88(0, 93),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_info_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_info_ext_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, wcdma_info_ext),
  QMI_IDL_TYPE88(0, 94),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gncell_bcch) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gncell_bcch_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, gncell_bcch),
  NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gncell_bcch) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, gncell_bcch_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, timing_advance) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, timing_advance_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, timing_advance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cells) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cells_valid)),
  0x1F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, cells),
  NAS_UMTS_MAX_ACTIVE_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cells) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, cells_len),
  QMI_IDL_TYPE88(0, 95),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_reference_rl) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_reference_rl_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_reference_rl),
  QMI_IDL_TYPE88(0, 96),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, geran_info_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, geran_info_ext_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, geran_info_ext),
  QMI_IDL_TYPE88(0, 102),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, umts_ext_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, umts_ext_info_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, umts_ext_info),
  QMI_IDL_TYPE88(0, 99),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_cells) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_cells_valid)),
  0x23,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_cells),
  NAS_UMTS_MAX_ACTIVE_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_cells) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, wcdma_active_set_cells_len),
  QMI_IDL_TYPE88(0, 100),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, scell_geran_config) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, scell_geran_config_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, scell_geran_config),
  QMI_IDL_TYPE88(0, 103),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, current_l1_ts) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, current_l1_ts_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, current_l1_ts),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, doppler_measurement) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, doppler_measurement_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, doppler_measurement),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_intra_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_intra_earfcn_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_intra_earfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_earfcn_valid)),
  0x28,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_inter_earfcn),
  NAS_MAX_LTE_NGBR_NUM_FREQS_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_earfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_earfcn_valid)),
  0x29,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_earfcn),
  NAS_UMTS_LTE_MAX_NBR_CELL_SET_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_earfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_state) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_state_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, emm_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_substate) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_substate_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, emm_substate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_connection_state) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, emm_connection_state_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, emm_connection_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_rrc_state_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_rrc_state_ext_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_rrc_state_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nr5g_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nr5g_arfcn_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, nr5g_arfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nr5g_serving_cell_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nr5g_serving_cell_info_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, nr5g_serving_cell_info),
  QMI_IDL_TYPE88(0, 104),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, lte_inter_ext_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, lte_inter_ext),
  QMI_IDL_TYPE88(0, 84),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nrdc_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, nrdc_arfcn_valid)),
  0x31,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, nrdc_arfcn),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, camped_plmn) - QMI_IDL_OFFSET16RELATIVE(nas_get_cell_location_info_resp_msg_v01, camped_plmn_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_cell_location_info_resp_msg_v01, camped_plmn),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t nas_get_plmn_name_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, plmn),
  QMI_IDL_TYPE88(3, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, suppress_sim_error) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, suppress_sim_error_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, suppress_sim_error),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, mnc_includes_pcs_digit) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, mnc_includes_pcs_digit_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, mnc_includes_pcs_digit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, always_send_plmn_name) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, always_send_plmn_name_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, always_send_plmn_name),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, use_static_table_only) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, use_static_table_only_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, use_static_table_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, csg_id) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, csg_id_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, csg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, rat) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, rat_valid)),
  0x15,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, send_all_information) - QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, send_all_information_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_req_msg_v01, send_all_information)
};

static const uint8_t nas_get_plmn_name_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_plmn_name_resp_msg_v01, eons_plmn_name_3gpp) - QMI_IDL_OFFSET8(nas_get_plmn_name_resp_msg_v01, eons_plmn_name_3gpp_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_plmn_name_resp_msg_v01, eons_plmn_name_3gpp),
  QMI_IDL_TYPE88(0, 105),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, eons_display_bit_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, eons_display_bit_info_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, eons_display_bit_info),
  QMI_IDL_TYPE88(0, 106),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, is_home_network) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, is_home_network_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, is_home_network),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, lang_plmn_names) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, lang_plmn_names_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, lang_plmn_names),
  NAS_ALT_LANG_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, lang_plmn_names) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, lang_plmn_names_len),
  QMI_IDL_TYPE88(0, 107),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, addl_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, addl_info_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, addl_info),
  NAS_PLMN_NAME_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, addl_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, addl_info_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, nw_name_source_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, nw_name_source),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, spn_ext) - QMI_IDL_OFFSET16RELATIVE(nas_get_plmn_name_resp_msg_v01, spn_ext_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_FLAGS_UTF16_STRING,
  QMI_IDL_OFFSET16ARRAY(nas_get_plmn_name_resp_msg_v01, spn_ext),
  NAS_SPN_EXT_LEN_MAX_V01
};

static const uint8_t nas_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_bind_subscription_req_msg_v01, subs_type)
};

static const uint8_t nas_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_dual_standby_pref_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, standby_pref) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, standby_pref_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, standby_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, priority_subs) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, priority_subs_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, priority_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_data_subs) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_data_subs_valid)),
  0x12,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_data_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_voice_subs) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_voice_subs_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, default_voice_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, active_subs_mask) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, active_subs_mask_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, active_subs_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, dds_duration) - QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, dds_duration_valid)),
  0x15,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_req_msg_v01, dds_duration)
};

static const uint8_t nas_set_dual_standby_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_dual_standby_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_dual_standby_pref_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, standby_pref) - QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, standby_pref_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, standby_pref),
  QMI_IDL_TYPE88(0, 108),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, default_voice_subs) - QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, default_voice_subs_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, default_voice_subs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, active_subs_mask) - QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, active_subs_mask_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_dual_standby_pref_ind_msg_v01, active_subs_mask)
};

static const uint8_t nas_set_high_capability_sub_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_high_capability_sub_req_msg_v01, high_capability_sub)
};

static const uint8_t nas_set_high_capability_sub_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_high_capability_sub_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_high_capability_sub_req_msg is empty
 * static const uint8_t nas_get_high_capability_sub_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_high_capability_sub_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_high_capability_sub_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_high_capability_sub_resp_msg_v01, high_capability_sub)
};

static const uint8_t nas_high_capability_sub_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_high_capability_sub_ind_msg_v01, high_capability_sub)
};

static const uint8_t nas_subscription_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_priority_subs) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_priority_subs_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_priority_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_active) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_active_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_active),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_default_data_subs) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_default_data_subs_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, is_default_data_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, voice_system_id) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, voice_system_id_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, lte_voice_system_id) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, lte_voice_system_id_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, lte_voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, wlan_voice_system_id) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, wlan_voice_system_id_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, wlan_voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, dds_type) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, dds_type_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, dds_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, nr5g_voice_system_id) - QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, nr5g_voice_system_id_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_subscription_info_ind_msg_v01, nr5g_voice_system_id)
};

/*
 * nas_get_mode_pref_req_msg is empty
 * static const uint8_t nas_get_mode_pref_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_mode_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx0_mode_pref) - QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx0_mode_pref_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx0_mode_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx1_mode_pref) - QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx1_mode_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx1_mode_pref),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx2_mode_pref) - QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx2_mode_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_mode_pref_resp_msg_v01, idx2_mode_pref)
};

static const uint8_t nas_set_technology_preference_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_technology_preference_req_msg_v01, technology_pref),
  QMI_IDL_TYPE88(0, 109)
};

static const uint8_t nas_set_technology_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_technology_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_technology_preference_req_type is empty
 * static const uint8_t nas_get_technology_preference_req_type_data_v01[] = {
 * };
 */

static const uint8_t nas_get_technology_preference_resp_type_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_technology_preference_resp_type_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_technology_preference_resp_type_v01, active_technology_pref),
  QMI_IDL_TYPE88(0, 110),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_technology_preference_resp_type_v01, persistent_technology_pref) - QMI_IDL_OFFSET8(nas_get_technology_preference_resp_type_v01, persistent_technology_pref_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_technology_preference_resp_type_v01, persistent_technology_pref)
};

/*
 * nas_get_network_system_preference_req is empty
 * static const uint8_t nas_get_network_system_preference_req_data_v01[] = {
 * };
 */

static const uint8_t nas_get_network_system_preference_resp_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_network_system_preference_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_network_system_preference_resp_v01, system_pref)
};

static const uint8_t nas_network_time_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, universal_time),
  QMI_IDL_TYPE88(0, 111),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, time_zone) - QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, time_zone_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, time_zone),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, daylt_sav_adj) - QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, daylt_sav_adj_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, daylt_sav_adj),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, radio_if) - QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, radio_if_valid)),
  0x12,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_network_time_ind_msg_v01, radio_if)
};

/*
 * nas_get_sys_info_req_msg is empty
 * static const uint8_t nas_get_sys_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_sys_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_srv_status_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_srv_status_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_srv_status_info),
  QMI_IDL_TYPE88(0, 113),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_srv_status_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_srv_status_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_srv_status_info),
  QMI_IDL_TYPE88(0, 113),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, gsm_srv_status_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, gsm_srv_status_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, gsm_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, wcdma_srv_status_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, wcdma_srv_status_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, wcdma_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, lte_srv_status_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, lte_srv_status_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, lte_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_sys_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_sys_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, cdma_sys_info),
  QMI_IDL_TYPE88(0, 125),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_sys_info) - QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_sys_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_resp_msg_v01, hdr_sys_info),
  QMI_IDL_TYPE88(0, 126),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_sys_info),
  QMI_IDL_TYPE88(0, 127),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_sys_info),
  QMI_IDL_TYPE88(0, 128),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sys_info_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_sys_info),
  QMI_IDL_TYPE88(0, 129),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_sys_info2_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cdma_sys_info2),
  QMI_IDL_TYPE88(0, 133),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_sys_info2_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, hdr_sys_info2),
  QMI_IDL_TYPE88(0, 134),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info2_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_sys_info2),
  QMI_IDL_TYPE88(0, 135),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info2_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_sys_info2),
  QMI_IDL_TYPE88(0, 136),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sys_info2_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_sys_info2),
  QMI_IDL_TYPE88(0, 137),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info3) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sys_info3_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_sys_info3),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info3) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sys_info3_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_sys_info3),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, voice_support_on_lte) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, voice_support_on_lte_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, voice_support_on_lte),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_cipher_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_cipher_domain_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_cipher_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_cipher_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_cipher_domain_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_cipher_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_srv_status_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_srv_status_info_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tdscdma_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_sys_info_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tdscdma_sys_info),
  QMI_IDL_TYPE88(0, 132),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_embms_coverage) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_embms_coverage_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_embms_coverage),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, sim_rej_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, sim_rej_info_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, sim_rej_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_eutra_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_eutra_status_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_eutra_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_ims_voice_avail) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_ims_voice_avail_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_ims_voice_avail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_voice_status_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_reg_zone) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_reg_zone_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cdma_reg_zone),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_rac) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_rac_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_rac) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_rac_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_mcc_resolved_via_sid_lookup) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_mcc_resolved_via_sid_lookup_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cdma_mcc_resolved_via_sid_lookup),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, srv_reg_restriction) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, srv_reg_restriction_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, srv_reg_restriction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_reg_domain_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tdscdma_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_reg_domain_valid)),
  0x31,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_reg_domain_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_reg_domain_valid)),
  0x33,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_embms_coverage_trace_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_embms_coverage_trace_id_valid)),
  0x34,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_embms_coverage_trace_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_csg_info_valid)),
  0x35,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_csg_info),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_voice_status_valid)),
  0x36,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, hdr_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_sms_status_valid)),
  0x37,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, hdr_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_sms_status_valid)),
  0x38,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_is_eb_supported) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_is_eb_supported_valid)),
  0x39,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_is_eb_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_voice_status_valid)),
  0x3A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, gsm_sms_status_valid)),
  0x3B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, gsm_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_voice_status_valid)),
  0x3C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, wcdma_sms_status_valid)),
  0x3D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, wcdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, emergency_access_barred) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, emergency_access_barred_valid)),
  0x3E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, emergency_access_barred),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_voice_status_valid)),
  0x3F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cdma_sms_status_valid)),
  0x40,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_voice_status_valid)),
  0x41,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tdscdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tdscdma_sms_status_valid)),
  0x42,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tdscdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_csg_info_valid)),
  0x43,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_csg_info),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_cell_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_cell_status_valid)),
  0x44,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_cell_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_subnet_mask_len) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, hdr_subnet_mask_len_valid)),
  0x45,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, hdr_subnet_mask_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, embms_coverage_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, embms_coverage_status_valid)),
  0x46,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, embms_coverage_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tds_rac) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tds_rac_valid)),
  0x47,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tds_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nas_supp_ciot_optimizations) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nas_supp_ciot_optimizations_valid)),
  0x48,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nas_supp_ciot_optimizations),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nas_camped_ciot_lte_op_mode) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nas_camped_ciot_lte_op_mode_valid)),
  0x49,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nas_camped_ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_srv_status_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_srv_status_info_valid)),
  0x4A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_sys_info_valid)),
  0x4B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_sys_info),
  QMI_IDL_TYPE88(0, 130),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_cell_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_cell_status_valid)),
  0x4C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_cell_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cp_sms_service_status) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cp_sms_service_status_valid)),
  0x4D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cp_sms_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, endc_available) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, endc_available_valid)),
  0x4E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, endc_available),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, restrict_dcnr) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, restrict_dcnr_valid)),
  0x4F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, restrict_dcnr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_tac_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_tac_info_valid)),
  0x50,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_tac_info),
  QMI_IDL_TYPE88(0, 67),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tracking_area_is_restricted) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, tracking_area_is_restricted_valid)),
  0x51,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, tracking_area_is_restricted),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, n1_sms_is_registered) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, n1_sms_is_registered_valid)),
  0x52,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, n1_sms_is_registered),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, mpsi_indicator_is_valid) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, mpsi_indicator_is_valid_valid)),
  0x53,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, mpsi_indicator_is_valid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_pci) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_pci_valid)),
  0x54,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_pci),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, plmn_infolist_r15_available) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, plmn_infolist_r15_available_valid)),
  0x55,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, plmn_infolist_r15_available),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_voice_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_voice_domain_valid)),
  0x56,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_voice_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_sms_domain) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_sms_domain_valid)),
  0x57,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_sms_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, voice_support_on_nr5g) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, voice_support_on_nr5g_valid)),
  0x58,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, voice_support_on_nr5g),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_ims_3gpp_voice_avail) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_ims_3gpp_voice_avail_valid)),
  0x59,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_ims_3gpp_voice_avail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_emerg_srv_ind) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_emerg_srv_ind_valid)),
  0x5A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_emerg_srv_ind),
  QMI_IDL_TYPE88(0, 138),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_cs_capability) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, lte_cs_capability_valid)),
  0x5B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, lte_cs_capability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_freq_type) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_freq_type_valid)),
  0x5C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_freq_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_subcarrier_spacing) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_subcarrier_spacing_valid)),
  0x5D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_subcarrier_spacing),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_cell_id_valid)),
  0x5E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_cell_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, primary_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, primary_plmn_info_valid)),
  0x5F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, primary_plmn_info),
  QMI_IDL_TYPE88(0, 115),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nr5g_arfcn_valid)),
  0x60,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nr5g_arfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_pci) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_pci_valid)),
  0x61,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nrdc_pci),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_freq_type) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_freq_type_valid)),
  0x62,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nrdc_freq_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_subcarrier_spacing) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_subcarrier_spacing_valid)),
  0x63,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nrdc_subcarrier_spacing),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, nrdc_arfcn_valid)),
  0x64,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, nrdc_arfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cag_id_name) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, cag_id_name_valid)),
  0x65,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, cag_id_name),
  QMI_IDL_TYPE88(0, 31),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, snpn_id_name) - QMI_IDL_OFFSET16RELATIVE(nas_get_sys_info_resp_msg_v01, snpn_id_name_valid)),
  0x66,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_sys_info_resp_msg_v01, snpn_id_name),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t nas_sys_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_srv_status_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_srv_status_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_srv_status_info),
  QMI_IDL_TYPE88(0, 113),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_srv_status_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_srv_status_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_srv_status_info),
  QMI_IDL_TYPE88(0, 113),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, gsm_srv_status_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, gsm_srv_status_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, gsm_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, wcdma_srv_status_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, wcdma_srv_status_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, wcdma_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, lte_srv_status_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, lte_srv_status_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, lte_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_sys_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_sys_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, cdma_sys_info),
  QMI_IDL_TYPE88(0, 125),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_sys_info) - QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_sys_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sys_info_ind_msg_v01, hdr_sys_info),
  QMI_IDL_TYPE88(0, 126),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_sys_info),
  QMI_IDL_TYPE88(0, 127),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_sys_info),
  QMI_IDL_TYPE88(0, 128),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sys_info_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_sys_info),
  QMI_IDL_TYPE88(0, 129),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_sys_info2_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cdma_sys_info2),
  QMI_IDL_TYPE88(0, 133),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_sys_info2_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, hdr_sys_info2),
  QMI_IDL_TYPE88(0, 134),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info2_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_sys_info2),
  QMI_IDL_TYPE88(0, 135),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info2_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_sys_info2),
  QMI_IDL_TYPE88(0, 136),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sys_info2) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sys_info2_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_sys_info2),
  QMI_IDL_TYPE88(0, 137),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info3) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sys_info3_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_sys_info3),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info3) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sys_info3_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_sys_info3),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_support_on_lte) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_support_on_lte_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, voice_support_on_lte),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_cipher_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_cipher_domain_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_cipher_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_cipher_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_cipher_domain_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_cipher_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, sys_info_no_change) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, sys_info_no_change_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, sys_info_no_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_srv_status_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_srv_status_info_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tdscdma_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_sys_info_valid)),
  0x26,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tdscdma_sys_info),
  QMI_IDL_TYPE88(0, 132),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_embms_coverage) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_embms_coverage_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_embms_coverage),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, sim_rej_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, sim_rej_info_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, sim_rej_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_eutra_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_eutra_status_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_eutra_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_ims_voice_avail) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_ims_voice_avail_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_ims_voice_avail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_voice_status_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_reg_zone) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_reg_zone_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cdma_reg_zone),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_rac) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_rac_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_rac) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_rac_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_mcc_resolved_via_sid_lookup) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_mcc_resolved_via_sid_lookup_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cdma_mcc_resolved_via_sid_lookup),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, srv_reg_restriction) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, srv_reg_restriction_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, srv_reg_restriction),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_reg_domain_valid)),
  0x31,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tdscdma_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_reg_domain_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_reg_domain_valid)),
  0x33,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_reg_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_reg_domain_valid)),
  0x34,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_reg_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_embms_coverage_trace_id) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_embms_coverage_trace_id_valid)),
  0x35,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_embms_coverage_trace_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_csg_info_valid)),
  0x36,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_csg_info),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_voice_status_valid)),
  0x37,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, hdr_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_sms_status_valid)),
  0x38,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, hdr_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_sms_status_valid)),
  0x39,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_is_eb_supported) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_is_eb_supported_valid)),
  0x3A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_is_eb_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_voice_status_valid)),
  0x3B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, gsm_sms_status_valid)),
  0x3C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, gsm_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_voice_status_valid)),
  0x3D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_sms_status_valid)),
  0x3E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, emergency_access_barred) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, emergency_access_barred_valid)),
  0x3F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, emergency_access_barred),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_voice_status_valid)),
  0x40,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cdma_sms_status_valid)),
  0x41,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_voice_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_voice_status_valid)),
  0x42,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tdscdma_voice_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_sms_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tdscdma_sms_status_valid)),
  0x43,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tdscdma_sms_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_csg_info_valid)),
  0x44,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_csg_info),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_cell_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_cell_status_valid)),
  0x45,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_cell_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_subnet_mask_len) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, hdr_subnet_mask_len_valid)),
  0x46,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, hdr_subnet_mask_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, embms_coverage_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, embms_coverage_status_valid)),
  0x47,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, embms_coverage_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, edrx_supported) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, edrx_supported_valid)),
  0x48,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, edrx_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tds_rac) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tds_rac_valid)),
  0x49,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tds_rac),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nas_supp_ciot_optimizations) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nas_supp_ciot_optimizations_valid)),
  0x4A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nas_supp_ciot_optimizations),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nas_camped_ciot_lte_op_mode) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nas_camped_ciot_lte_op_mode_valid)),
  0x4B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nas_camped_ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_srv_status_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_srv_status_info_valid)),
  0x4C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_srv_status_info),
  QMI_IDL_TYPE88(0, 114),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_sys_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_sys_info_valid)),
  0x4D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_sys_info),
  QMI_IDL_TYPE88(0, 130),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_cell_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_cell_status_valid)),
  0x4E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_cell_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cp_sms_service_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cp_sms_service_status_valid)),
  0x4F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cp_sms_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, endc_available) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, endc_available_valid)),
  0x50,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, endc_available),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, restrict_dcnr) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, restrict_dcnr_valid)),
  0x51,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, restrict_dcnr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_tac_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_tac_info_valid)),
  0x52,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_tac_info),
  QMI_IDL_TYPE88(0, 67),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tracking_area_is_restricted) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, tracking_area_is_restricted_valid)),
  0x53,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, tracking_area_is_restricted),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, n1_sms_is_registered) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, n1_sms_is_registered_valid)),
  0x54,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, n1_sms_is_registered),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, mpsi_indicator_is_valid) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, mpsi_indicator_is_valid_valid)),
  0x55,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, mpsi_indicator_is_valid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_pci) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_pci_valid)),
  0x56,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_pci),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, plmn_infolist_r15_available) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, plmn_infolist_r15_available_valid)),
  0x57,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, plmn_infolist_r15_available),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_voice_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_voice_domain_valid)),
  0x58,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_voice_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_sms_domain) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_sms_domain_valid)),
  0x59,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_sms_domain),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_support_on_nr5g) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_support_on_nr5g_valid)),
  0x5A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, voice_support_on_nr5g),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_ims_3gpp_voice_avail) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_ims_3gpp_voice_avail_valid)),
  0x5B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_ims_3gpp_voice_avail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_emerg_srv_ind) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_emerg_srv_ind_valid)),
  0x5C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_emerg_srv_ind),
  QMI_IDL_TYPE88(0, 138),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_cs_capability) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, lte_cs_capability_valid)),
  0x5D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, lte_cs_capability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_freq_type) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_freq_type_valid)),
  0x5E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_freq_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_subcarrier_spacing) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_subcarrier_spacing_valid)),
  0x5F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_subcarrier_spacing),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_cell_id_valid)),
  0x60,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_cell_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, primary_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, primary_plmn_info_valid)),
  0x61,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, primary_plmn_info),
  QMI_IDL_TYPE88(0, 115),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_cs_signalling_bar_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_cs_signalling_bar_status_valid)),
  0x62,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_cs_signalling_bar_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_ps_signalling_bar_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, wcdma_ps_signalling_bar_status_valid)),
  0x63,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, wcdma_ps_signalling_bar_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nr5g_arfcn_valid)),
  0x64,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nr5g_arfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_pci) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_pci_valid)),
  0x65,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nrdc_pci),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_freq_type) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_freq_type_valid)),
  0x66,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nrdc_freq_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_subcarrier_spacing) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_subcarrier_spacing_valid)),
  0x67,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nrdc_subcarrier_spacing),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, nrdc_arfcn_valid)),
  0x68,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, nrdc_arfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cag_id_name) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, cag_id_name_valid)),
  0x69,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, cag_id_name),
  QMI_IDL_TYPE88(0, 31),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, snpn_id_name) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, snpn_id_name_valid)),
  0x6A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, snpn_id_name),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_blocked_status) - QMI_IDL_OFFSET16RELATIVE(nas_sys_info_ind_msg_v01, voice_blocked_status_valid)),
  0x6B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_sys_info_ind_msg_v01, voice_blocked_status)
};

/*
 * nas_get_sig_info_req_msg is empty
 * static const uint8_t nas_get_sig_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_sig_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, cdma_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, cdma_sig_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, cdma_sig_info),
  QMI_IDL_TYPE88(0, 139),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, hdr_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, hdr_sig_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, hdr_sig_info),
  QMI_IDL_TYPE88(0, 140),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, gsm_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, gsm_sig_info_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, gsm_sig_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, wcdma_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, wcdma_sig_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, wcdma_sig_info),
  QMI_IDL_TYPE88(0, 139),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, lte_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, lte_sig_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, lte_sig_info),
  QMI_IDL_TYPE88(0, 141),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, rscp) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, rscp_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, rscp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, tdscdma_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, tdscdma_sig_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, tdscdma_sig_info),
  QMI_IDL_TYPE88(0, 142),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_sig_info_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_sig_info),
  QMI_IDL_TYPE88(0, 143),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_rsrq) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_rsrq_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nr5g_rsrq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, umts_rscp) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, umts_rscp_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, umts_rscp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nrdc_sig_info) - QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nrdc_sig_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sig_info_resp_msg_v01, nrdc_sig_info),
  QMI_IDL_TYPE88(0, 144)
};

static const uint8_t nas_config_sig_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, rssi_threshold_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, ecio_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, ecio_threshold_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, ecio_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, ecio_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, ecio_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, hdr_sinr_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, hdr_sinr_threshold_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, hdr_sinr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, hdr_sinr_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, hdr_sinr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_snr_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_snr_threshold_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_snr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_snr_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_snr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, io_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, io_threshold_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, io_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, io_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, io_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrq_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrq_threshold_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrq_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrq_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrq_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrp_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrp_threshold_list_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrp_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info_req_msg_v01, lte_rsrp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, lte_sig_rpt_config) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, lte_sig_rpt_config_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info_req_msg_v01, lte_sig_rpt_config),
  QMI_IDL_TYPE88(0, 145),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, rscp_threshold_list_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info_req_msg_v01, rscp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, rscp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, tds_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, tds_sinr_threshold_list_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info_req_msg_v01, tds_sinr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, tds_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info_req_msg_v01, tds_sinr_threshold_list_len)
};

static const uint8_t nas_config_sig_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_config_sig_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_sig_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, cdma_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, cdma_sig_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, cdma_sig_info),
  QMI_IDL_TYPE88(0, 139),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, hdr_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, hdr_sig_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, hdr_sig_info),
  QMI_IDL_TYPE88(0, 140),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, gsm_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, gsm_sig_info_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, gsm_sig_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, wcdma_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, wcdma_sig_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, wcdma_sig_info),
  QMI_IDL_TYPE88(0, 139),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, lte_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, lte_sig_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, lte_sig_info),
  QMI_IDL_TYPE88(0, 141),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, rscp) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, rscp_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, rscp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, tdscdma_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, tdscdma_sig_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, tdscdma_sig_info),
  QMI_IDL_TYPE88(0, 142),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_sig_info_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_sig_info),
  QMI_IDL_TYPE88(0, 143),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_rsrq) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_rsrq_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nr5g_rsrq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, umts_rscp) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, umts_rscp_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, umts_rscp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nrdc_sig_info) - QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nrdc_sig_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_sig_info_ind_msg_v01, nrdc_sig_info),
  QMI_IDL_TYPE88(0, 144)
};

/*
 * nas_get_err_rate_req_msg is empty
 * static const uint8_t nas_get_err_rate_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_err_rate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, cdma_frame_err_rate) - QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, cdma_frame_err_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, cdma_frame_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, hdr_packet_err_rate) - QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, hdr_packet_err_rate_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, hdr_packet_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, gsm_bit_err_rate) - QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, gsm_bit_err_rate_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, gsm_bit_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, wcdma_block_err_rate) - QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, wcdma_block_err_rate_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, wcdma_block_err_rate),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, tdscdma_block_err_rate) - QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, tdscdma_block_err_rate_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_err_rate_resp_msg_v01, tdscdma_block_err_rate)
};

static const uint8_t nas_err_rate_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, cdma_frame_err_rate) - QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, cdma_frame_err_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, cdma_frame_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, hdr_packet_err_rate) - QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, hdr_packet_err_rate_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, hdr_packet_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, gsm_bit_err_rate) - QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, gsm_bit_err_rate_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, gsm_bit_err_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, wcdma_block_err_rate) - QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, wcdma_block_err_rate_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, wcdma_block_err_rate),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, tdscdma_block_err_rate) - QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, tdscdma_block_err_rate_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_err_rate_ind_msg_v01, tdscdma_block_err_rate)
};

static const uint8_t nas_hdr_session_close_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_session_close_ind_msg_v01, close_reason)
};

static const uint8_t nas_hdr_uati_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hdr_uati_update_ind_msg_v01, uati),
  QMI_NAS_UATI_LENGTH_V01
};

static const uint8_t nas_get_hdr_subtype_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_subtype_req_msg_v01, protocol)
};

static const uint8_t nas_get_hdr_subtype_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hdr_subtype_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_subtype_resp_msg_v01, subtype) - QMI_IDL_OFFSET8(nas_get_hdr_subtype_resp_msg_v01, subtype_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_subtype_resp_msg_v01, subtype)
};

/*
 * nas_get_hdr_color_code_req_msg is empty
 * static const uint8_t nas_get_hdr_color_code_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_hdr_color_code_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hdr_color_code_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_color_code_resp_msg_v01, color_code) - QMI_IDL_OFFSET8(nas_get_hdr_color_code_resp_msg_v01, color_code_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_color_code_resp_msg_v01, color_code)
};

/*
 * nas_get_current_acq_sys_mode_req_msg is empty
 * static const uint8_t nas_get_current_acq_sys_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_current_acq_sys_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma_evdo) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma_evdo_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, cdma_evdo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, gsm) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, gsm_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, gsm),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, umts) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, umts_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, umts),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, lte) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, lte_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, lte),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, tdscdma) - QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, tdscdma_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_acq_sys_mode_resp_msg_v01, tdscdma)
};

static const uint8_t nas_set_rx_diversity_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rx_diversity_req_msg_v01, req_param),
  QMI_IDL_TYPE88(0, 146)
};

static const uint8_t nas_set_rx_diversity_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rx_diversity_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_tx_rx_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_req_msg_v01, radio_if),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_req_msg_v01, nr5g_type) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_req_msg_v01, nr5g_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_req_msg_v01, nr5g_type)
};

static const uint8_t nas_get_tx_rx_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_0) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_0_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_0),
  QMI_IDL_TYPE88(0, 147),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_1) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_1_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_1),
  QMI_IDL_TYPE88(0, 147),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, tx) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, tx_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, tx),
  QMI_IDL_TYPE88(0, 148),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, downlink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, downlink_mod_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, downlink_mod),
  NAS_LTE_MODULATIONS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, downlink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, downlink_mod_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, uplink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, uplink_mod_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, uplink_mod),
  NAS_LTE_MODULATIONS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, uplink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, uplink_mod_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_2) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_2_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_2),
  QMI_IDL_TYPE88(0, 147),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_3) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_3_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, rx_chain_3),
  QMI_IDL_TYPE88(0, 147),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod),
  NAS_NR5G_MODULATIONS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod),
  NAS_NR5G_MODULATIONS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_extended) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_extended_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_extended),
  NAS_NR5G_MODULATIONS_MAX_EXTENDED_V01,
  QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_extended) - QMI_IDL_OFFSET8(nas_get_tx_rx_info_resp_msg_v01, nr5g_downlink_mod_extended_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_extended) - QMI_IDL_OFFSET16RELATIVE(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_extended_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_extended),
  NAS_NR5G_MODULATIONS_MAX_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_extended) - QMI_IDL_OFFSET16RELATIVE(nas_get_tx_rx_info_resp_msg_v01, nr5g_uplink_mod_extended_len)
};

static const uint8_t nas_update_akey_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_akey_ext_req_msg_v01, akey_with_spc),
  QMI_IDL_TYPE88(0, 149)
};

static const uint8_t nas_update_akey_ext_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_akey_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_managed_roaming_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_managed_roaming_ind_msg_v01, radio_if) - QMI_IDL_OFFSET8(nas_managed_roaming_ind_msg_v01, radio_if_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_managed_roaming_ind_msg_v01, radio_if)
};

/*
 * nas_get_dual_standby_pref_req_msg is empty
 * static const uint8_t nas_get_dual_standby_pref_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_dual_standby_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, standby_pref) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, standby_pref_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, standby_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, priority_subs) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, priority_subs_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, priority_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs_valid)),
  0x12,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_data_subs) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_data_subs_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_data_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_voice_subs) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_voice_subs_valid)),
  0x14,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, default_voice_subs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs_mask) - QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs_mask_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_dual_standby_pref_resp_msg_v01, active_subs_mask)
};

/*
 * nas_detach_lte_req_msg is empty
 * static const uint8_t nas_detach_lte_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_detach_lte_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_detach_lte_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_block_lte_plmn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_abs) - QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_abs_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_abs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_mult) - QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_mult_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_block_lte_plmn_req_msg_v01, blocking_interval_mult)
};

static const uint8_t nas_block_lte_plmn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_block_lte_plmn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_unblock_lte_plmn_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_unblock_lte_plmn_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t nas_unblock_lte_plmn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_unblock_lte_plmn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_reset_lte_plmn_blocking_req_msg is empty
 * static const uint8_t nas_reset_lte_plmn_blocking_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_reset_lte_plmn_blocking_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_reset_lte_plmn_blocking_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_current_plmn_name_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, plmn_id) - QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, plmn_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, plmn_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, spn) - QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, spn_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, spn),
  QMI_IDL_TYPE88(0, 150),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, short_name) - QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, short_name_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_current_plmn_name_ind_msg_v01, short_name),
  QMI_IDL_TYPE88(0, 151),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, long_name) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, long_name_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, long_name),
  QMI_IDL_TYPE88(0, 151),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, csg_id) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, csg_id_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, csg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, eons_display_bit_info) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, eons_display_bit_info_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, eons_display_bit_info),
  QMI_IDL_TYPE88(0, 106),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, is_home_network) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, is_home_network_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, is_home_network),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, rat) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, rat_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, lang_plmn_names) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, lang_plmn_names_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, lang_plmn_names),
  NAS_ALT_LANG_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, lang_plmn_names) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, lang_plmn_names_len),
  QMI_IDL_TYPE88(0, 107),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, addl_info) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, addl_info_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, addl_info),
  NAS_PLMN_NAME_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, addl_info) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, addl_info_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nw_name_source_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, nw_name_source),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, spn_ext) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, spn_ext_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_FLAGS_UTF16_STRING,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, spn_ext),
  NAS_SPN_EXT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nr5g_tac_info) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nr5g_tac_info_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, nr5g_tac_info),
  QMI_IDL_TYPE88(0, 67),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nitz_information) - QMI_IDL_OFFSET16RELATIVE(nas_current_plmn_name_ind_msg_v01, nitz_information_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_current_plmn_name_ind_msg_v01, nitz_information),
  QMI_IDL_TYPE88(0, 66)
};

static const uint8_t nas_config_embms_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_config_embms_req_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_embms_req_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_config_embms_req_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_embms_req_msg_v01, trace_id)
};

static const uint8_t nas_config_embms_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_config_embms_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_embms_resp_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_config_embms_resp_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_embms_resp_msg_v01, trace_id)
};

/*
 * nas_get_embms_status_req_msg is empty
 * static const uint8_t nas_get_embms_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_embms_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, enabled) - QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, trace_id_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_status_resp_msg_v01, trace_id)
};

static const uint8_t nas_embms_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_embms_status_ind_msg_v01, enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_embms_status_ind_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_embms_status_ind_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_embms_status_ind_msg_v01, trace_id)
};

/*
 * nas_get_cdma_position_info_req_msg is empty
 * static const uint8_t nas_get_cdma_position_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cdma_position_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cdma_position_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cdma_position_info_resp_msg_v01, info) - QMI_IDL_OFFSET8(nas_get_cdma_position_info_resp_msg_v01, info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cdma_position_info_resp_msg_v01, info),
  QMI_IDL_TYPE88(0, 153)
};

static const uint8_t nas_rf_band_info_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_band_info),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_dedicated_band_info) - QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_dedicated_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_dedicated_band_info),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_band_info_list_ext) - QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_band_info_list_ext_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, rf_band_info_list_ext),
  QMI_IDL_TYPE88(0, 59),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, nas_rf_bandwidth_info) - QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, nas_rf_bandwidth_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, nas_rf_bandwidth_info),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, ciot_lte_op_mode_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_rf_band_info_ind_msg_v01, ciot_lte_op_mode)
};

/*
 * nas_force_network_search_req_msg is empty
 * static const uint8_t nas_force_network_search_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_force_network_search_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_force_network_search_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_network_reject_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, radio_if),

  0x02,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, reject_srv_domain),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, rej_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, plmn_id) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, plmn_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, plmn_id),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, csg_id) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, csg_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, csg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, ciot_lte_op_mode_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, primary_plmn_id) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, primary_plmn_id_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, primary_plmn_id),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, is_attempt_counter_max_for_rej) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, is_attempt_counter_max_for_rej_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, is_attempt_counter_max_for_rej),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, proc_type) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, proc_type_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, proc_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, cag_id) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, cag_id_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, cag_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, snpn_nid) - QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, snpn_nid_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_network_reject_ind_msg_v01, snpn_nid),
  NAS_SNPN_NID_LEN_V01
};

/*
 * nas_get_managed_roaming_config_req_msg is empty
 * static const uint8_t nas_get_managed_roaming_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_managed_roaming_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_managed_roaming_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_managed_roaming_config_resp_msg_v01, managed_roaming_supported) - QMI_IDL_OFFSET8(nas_get_managed_roaming_config_resp_msg_v01, managed_roaming_supported_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_managed_roaming_config_resp_msg_v01, managed_roaming_supported)
};

static const uint8_t nas_rtre_cfg_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg) - QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg_pref) - QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg_pref_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_rtre_cfg_ind_msg_v01, rtre_cfg_pref)
};

/*
 * nas_get_centralized_eons_support_status_req_msg is empty
 * static const uint8_t nas_get_centralized_eons_support_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_centralized_eons_support_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_centralized_eons_support_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_centralized_eons_support_status_resp_msg_v01, centralized_eons_supported) - QMI_IDL_OFFSET8(nas_get_centralized_eons_support_status_resp_msg_v01, centralized_eons_supported_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_centralized_eons_support_status_resp_msg_v01, centralized_eons_supported)
};

static const uint8_t nas_config_sig_info2_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_threshold_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_delta) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_delta_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_threshold_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_delta) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_delta_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, cdma_ecio_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, hdr_rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, hdr_rssi_threshold_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, hdr_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, hdr_rssi_threshold_list) - QMI_IDL_OFFSET8(nas_config_sig_info2_req_msg_v01, hdr_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_rssi_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_rssi_delta_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_threshold_list_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_ecio_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_ecio_delta_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_ecio_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_threshold_list_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_sinr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_sinr_delta_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_sinr_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_threshold_list_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_io_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_io_delta_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_io_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_threshold_list_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, gsm_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_rssi_delta_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, gsm_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_threshold_list_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rssi_delta_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_threshold_list_valid)),
  0x20,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_ecio_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_ecio_delta_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_ecio_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_threshold_list_valid)),
  0x22,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rssi_delta_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_threshold_list_valid)),
  0x24,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_snr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_snr_delta_valid)),
  0x25,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_snr_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_threshold_list_valid)),
  0x26,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rsrq_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrq_delta_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rsrq_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_threshold_list_valid)),
  0x28,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rsrp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_rsrp_delta_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_rsrp_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_sig_rpt_config) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_sig_rpt_config_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_sig_rpt_config),
  QMI_IDL_TYPE88(0, 145),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_threshold_list_valid)),
  0x2B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_delta_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tdscdma_rscp_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_rssi_threshold_list_valid)),
  0x2D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tds_rssi_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_rssi_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_rssi_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rssi_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_rssi_delta_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tdscdma_rssi_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_ecio_threshold_list_valid)),
  0x2F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tds_ecio_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_ecio_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_ecio_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_ecio_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_ecio_delta_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tdscdma_ecio_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_sinr_threshold_list_valid)),
  0x31,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tds_sinr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_sinr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_sinr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_sinr_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tdscdma_sinr_delta_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tdscdma_sinr_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_threshold_list_valid)),
  0x33,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_snr_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_snr_delta_valid)),
  0x34,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_snr_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_threshold_list_valid)),
  0x35,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_delta_valid)),
  0x36,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_rsrp_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_sig_rpt_config) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_sig_rpt_config_valid)),
  0x37,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_sig_rpt_config),
  QMI_IDL_TYPE88(0, 154),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_threshold_list_valid)),
  0x38,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_delta_valid)),
  0x39,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_rsrq_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_threshold_list_valid)),
  0x3A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_rscp_threshold_list),
  NAS_SIG_STR_THRESHOLD_LIST_MAX2_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_threshold_list) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_threshold_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_delta) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_rscp_delta_valid)),
  0x3B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_rscp_delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_hysteresis_valid)),
  0x3C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, wcdma_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, wcdma_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_hysteresis_valid)),
  0x3D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, gsm_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, gsm_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_hysteresis_valid)),
  0x3E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, lte_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, lte_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_hysteresis_valid)),
  0x3F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, tds_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, tds_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, cdma_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, cdma_hysteresis_valid)),
  0x40,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, cdma_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, cdma_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, cdma_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_hysteresis_valid)),
  0x41,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hdr_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hdr_hysteresis_len),
  QMI_IDL_TYPE88(0, 155),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hysteresis_timer) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, hysteresis_timer_valid)),
  0x42,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, hysteresis_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_hysteresis_valid)),
  0x43,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_config_sig_info2_req_msg_v01, nr5g_hysteresis),
  NAS_SIG_TYPE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_config_sig_info2_req_msg_v01, nr5g_hysteresis_len),
  QMI_IDL_TYPE88(0, 155)
};

static const uint8_t nas_config_sig_info2_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_config_sig_info2_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_tds_cell_and_position_info_req_msg is empty
 * static const uint8_t nas_get_tds_cell_and_position_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_tds_cell_and_position_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_cell_info) - QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_cell_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_cell_info),
  QMI_IDL_TYPE88(0, 156),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_nbr_cell_info) - QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_nbr_cell_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_nbr_cell_info),
  NAS_TDS_MAX_NBR_CELL_NUM_V01,
  QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_nbr_cell_info) - QMI_IDL_OFFSET8(nas_get_tds_cell_and_position_info_resp_msg_v01, tds_nbr_cell_info_len),
  QMI_IDL_TYPE88(0, 157)
};

static const uint8_t nas_set_hplmn_irat_search_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_hplmn_irat_search_timer_req_msg_v01, timer_value)
};

static const uint8_t nas_set_hplmn_irat_search_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_hplmn_irat_search_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_embms_sig_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_req_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_get_embms_sig_req_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_req_msg_v01, trace_id)
};

static const uint8_t nas_get_embms_sig_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, trace_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, sig_list) - QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, sig_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, sig_list),
  NAS_LTE_EMBMS_MAX_MBSFN_AREA_V01,
  QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, sig_list) - QMI_IDL_OFFSET8(nas_get_embms_sig_resp_msg_v01, sig_list_len),
  QMI_IDL_TYPE88(0, 158)
};

static const uint8_t nas_limit_sys_info_ind_reporting_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_limit_sys_info_ind_reporting_req_msg_v01, limit_sys_info_chg_rpt)
};

static const uint8_t nas_limit_sys_info_ind_reporting_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_limit_sys_info_ind_reporting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_sys_info_ind_reporting_limit_req_msg is empty
 * static const uint8_t nas_get_sys_info_ind_reporting_limit_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_sys_info_ind_reporting_limit_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sys_info_ind_reporting_limit_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_sys_info_ind_reporting_limit_resp_msg_v01, limit_sys_info_chg_rpt)
};

static const uint8_t nas_update_ims_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_update_ims_status_req_msg_v01, sys_mode),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_ims_status_req_msg_v01, registration_state),
  NAS_IMS_REG_STATUS_MAX_V01,
  QMI_IDL_OFFSET8(nas_update_ims_status_req_msg_v01, registration_state) - QMI_IDL_OFFSET8(nas_update_ims_status_req_msg_v01, registration_state_len),
  QMI_IDL_TYPE88(0, 159),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, registration_end_cause) - QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, registration_end_cause_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_update_ims_status_req_msg_v01, registration_end_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, retry_timer) - QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, retry_timer_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_update_ims_status_req_msg_v01, retry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, applicable_to_all_plmns) - QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, applicable_to_all_plmns_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_update_ims_status_req_msg_v01, applicable_to_all_plmns),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, applicable_to_all_ps_rats) - QMI_IDL_OFFSET16RELATIVE(nas_update_ims_status_req_msg_v01, applicable_to_all_ps_rats_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_update_ims_status_req_msg_v01, applicable_to_all_ps_rats)
};

static const uint8_t nas_update_ims_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_ims_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_ims_pref_status_req_msg is empty
 * static const uint8_t nas_get_ims_pref_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_ims_pref_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ims_pref_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ims_pref_status_resp_msg_v01, ims_pref) - QMI_IDL_OFFSET8(nas_get_ims_pref_status_resp_msg_v01, ims_pref_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ims_pref_status_resp_msg_v01, ims_pref),
  QMI_IDL_TYPE88(0, 160)
};

static const uint8_t nas_ims_pref_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ims_pref_status_ind_msg_v01, ims_pref),
  QMI_IDL_TYPE88(0, 160),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ims_pref_status_ind_msg_v01, voice_blocked_status) - QMI_IDL_OFFSET8(nas_ims_pref_status_ind_msg_v01, voice_blocked_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ims_pref_status_ind_msg_v01, voice_blocked_status)
};

static const uint8_t nas_config_plmn_name_ind_reporting_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_config_plmn_name_ind_reporting_req_msg_v01, send_all_information)
};

static const uint8_t nas_config_plmn_name_ind_reporting_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_config_plmn_name_ind_reporting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_cdma_avoid_system_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_system_req_msg_v01, avoid_type)
};

static const uint8_t nas_cdma_avoid_system_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cdma_avoid_system_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_cdma_avoid_system_list_req_msg is empty
 * static const uint8_t nas_get_cdma_avoid_system_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cdma_avoid_system_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, nam1_systems) - QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, nam1_systems_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, nam1_systems),
  NAS_MAX_CDMA_SYSTEMS_AVOIDED_V01,
  QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, nam1_systems) - QMI_IDL_OFFSET8(nas_get_cdma_avoid_system_list_resp_msg_v01, nam1_systems_len),
  QMI_IDL_TYPE88(0, 161)
};

static const uint8_t nas_set_hplmn_search_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_hplmn_search_timer_req_msg_v01, timer_value)
};

static const uint8_t nas_set_hplmn_search_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_hplmn_search_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_hplmn_search_timer_req_msg is empty
 * static const uint8_t nas_get_hplmn_search_timer_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_hplmn_search_timer_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hplmn_search_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hplmn_search_timer_resp_msg_v01, timer_value) - QMI_IDL_OFFSET8(nas_get_hplmn_search_timer_resp_msg_v01, timer_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_hplmn_search_timer_resp_msg_v01, timer_value)
};

/*
 * nas_get_scan_config_req_msg is empty
 * static const uint8_t nas_get_scan_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_scan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, scan_counter) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, scan_counter_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, scan_counter),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_min) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_min_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_min),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_inc_type) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_inc_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_inc_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_max) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_max_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, power_save_duration_max),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, hplmn_scan_interval) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, hplmn_scan_interval_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, hplmn_scan_interval),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, ciot_pref_rat_scan_interval) - QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, ciot_pref_rat_scan_interval_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_scan_config_resp_msg_v01, ciot_pref_rat_scan_interval)
};

/*
 * nas_get_current_scan_info_req_msg is empty
 * static const uint8_t nas_get_current_scan_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_current_scan_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_rat) - QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_rat_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_ciot_lte_rat) - QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_ciot_lte_rat_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_camped_ciot_lte_rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_roaming_status) - QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_roaming_status_valid)),
  0x12,
  QMI_IDL_FLAGS_FIRST_EXTENDED |   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_FLAGS_ENUM_IS_UNSIGNED,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_roaming_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_rat) - QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_rat_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_rat),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_ciot_lte_rat) - QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_ciot_lte_rat_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_current_scan_info_resp_msg_v01, current_scanning_ciot_lte_rat)
};

/*
 * nas_get_subscription_info_req_msg is empty
 * static const uint8_t nas_get_subscription_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_subscription_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_priority_subs) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_priority_subs_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_priority_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_active) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_active_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_active),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_default_data_subs) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_default_data_subs_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, is_default_data_subs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, voice_system_id) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, voice_system_id_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, lte_voice_system_id) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, lte_voice_system_id_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, lte_voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, wlan_voice_system_id) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, wlan_voice_system_id_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, wlan_voice_system_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, dds_type) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, dds_type_valid)),
  0x16,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, dds_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, nr5g_voice_system_id) - QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, nr5g_voice_system_id_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_subscription_info_resp_msg_v01, nr5g_voice_system_id)
};

/*
 * nas_get_network_time_req_msg is empty
 * static const uint8_t nas_get_network_time_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_network_time_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp2_time) - QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp2_time_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp2_time),
  QMI_IDL_TYPE88(0, 162),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp_time) - QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp_time_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_network_time_resp_msg_v01, nas_3gpp_time),
  QMI_IDL_TYPE88(0, 162)
};

/*
 * nas_get_lte_sib16_network_time_req_msg is empty
 * static const uint8_t nas_get_lte_sib16_network_time_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_lte_sib16_network_time_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, lte_sib16_acquired) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, lte_sib16_acquired_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, lte_sib16_acquired),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, universal_time) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, universal_time_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, universal_time),
  QMI_IDL_TYPE88(0, 163),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, abs_time) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, abs_time_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, leap_sec) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, leap_sec_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, leap_sec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, time_zone) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, time_zone_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, time_zone),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, daylt_sav_adj) - QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, daylt_sav_adj_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_sib16_network_time_resp_msg_v01, daylt_sav_adj)
};

static const uint8_t nas_lte_sib16_network_time_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, lte_sib16_acquired) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, lte_sib16_acquired_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, lte_sib16_acquired),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, universal_time) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, universal_time_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, universal_time),
  QMI_IDL_TYPE88(0, 163),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, abs_time) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, abs_time_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, leap_sec) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, leap_sec_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, leap_sec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, time_zone) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, time_zone_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, time_zone),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, daylt_sav_adj) - QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, daylt_sav_adj_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_sib16_network_time_ind_msg_v01, daylt_sav_adj)
};

static const uint8_t nas_set_lte_band_priority_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_lte_band_priority_req_msg_v01, band_priority_list),
  NAS_LTE_BAND_PRIORITY_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_set_lte_band_priority_req_msg_v01, band_priority_list) - QMI_IDL_OFFSET8(nas_set_lte_band_priority_req_msg_v01, band_priority_list_len)
};

static const uint8_t nas_set_lte_band_priority_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_lte_band_priority_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_embms_sig_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_ext_req_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_get_embms_sig_ext_req_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_ext_req_msg_v01, trace_id)
};

static const uint8_t nas_get_embms_sig_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, trace_id) - QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, trace_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, trace_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, snr_and_tmgi_list) - QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, snr_and_tmgi_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, snr_and_tmgi_list),
  NAS_LTE_EMBMS_MAX_MBSFN_AREA_V01,
  QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, snr_and_tmgi_list) - QMI_IDL_OFFSET8(nas_get_embms_sig_ext_resp_msg_v01, snr_and_tmgi_list_len),
  QMI_IDL_TYPE88(0, 165)
};

static const uint8_t nas_lte_cphy_ca_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca),
  QMI_IDL_TYPE88(0, 166),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_dl_bandwidth) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_dl_bandwidth_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_info),
  QMI_IDL_TYPE88(0, 167),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_info) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 169),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_idx) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_idx_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_idx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, num_scells_configured) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, num_scells_configured_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, num_scells_configured),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_len),
  QMI_IDL_TYPE88(0, 168),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_freq) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_freq_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, scell_freq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_freq) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_freq_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, pcell_freq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_freq_list) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_freq_list_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_freq_list),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_freq_list) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_freq_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, ul_configured) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, ul_configured_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, ul_configured),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_ul_configured) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_ul_configured_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_ul_configured),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_ul_configured) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_ul_configured_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_ext) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_ext_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_ext),
  NAS_MAX_SCELL_LIST_LEN_EXT_V01,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_ext) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_ind_msg_v01, unchanged_scell_info_list_ext_len),
  QMI_IDL_TYPE88(0, 170)
};

/*
 * nas_get_lte_band_priority_list_req_msg is empty
 * static const uint8_t nas_get_lte_band_priority_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_lte_band_priority_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, configured_band_priority_list) - QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, configured_band_priority_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, configured_band_priority_list),
  NAS_LTE_BAND_PRIORITY_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, configured_band_priority_list) - QMI_IDL_OFFSET8(nas_get_lte_band_priority_list_resp_msg_v01, configured_band_priority_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_lte_band_priority_list_resp_msg_v01, supported_band_priority_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_lte_band_priority_list_resp_msg_v01, supported_band_priority_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(nas_get_lte_band_priority_list_resp_msg_v01, supported_band_priority_list),
  NAS_LTE_BAND_PRIORITY_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_lte_band_priority_list_resp_msg_v01, supported_band_priority_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_lte_band_priority_list_resp_msg_v01, supported_band_priority_list_len)
};

static const uint8_t nas_set_builtin_plmn_list_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_req_msg_v01, oplmn_list) - QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_req_msg_v01, oplmn_list_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_req_msg_v01, oplmn_list),
  QMI_IDL_TYPE88(0, 172),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_builtin_plmn_list_req_msg_v01, ind_token) - QMI_IDL_OFFSET16RELATIVE(nas_set_builtin_plmn_list_req_msg_v01, ind_token_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_builtin_plmn_list_req_msg_v01, ind_token)
};

static const uint8_t nas_set_builtin_plmn_list_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_builtin_plmn_list_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, error),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, ind_token) - QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, ind_token_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, ind_token),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, received_list_entry_count) - QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, received_list_entry_count_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, received_list_entry_count),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, remaining_list_entry_count) - QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, remaining_list_entry_count_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_builtin_plmn_list_ind_msg_v01, remaining_list_entry_count)
};

static const uint8_t nas_perform_incremental_network_scan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, network_type) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, network_type_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, network_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, scan_type) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, scan_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, scan_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, band_pref) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, band_pref_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, tdscdma_band_pref) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, tdscdma_band_pref_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, tdscdma_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, wcdma_arfcn) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, wcdma_arfcn_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, wcdma_arfcn),
  NAS_WCDMA_ARFCN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, wcdma_arfcn) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, wcdma_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_arfcn) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_arfcn_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_arfcn),
  NAS_LTE_ARFCN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_arfcn) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, lte_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, max_search_time) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, max_search_time_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, max_search_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results_periodicity) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results_periodicity_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, incremental_results_periodicity),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, preferred_plmn_list) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, preferred_plmn_list_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, preferred_plmn_list),
  NAS_PREFERRED_PLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, preferred_plmn_list) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_req_msg_v01, preferred_plmn_list_len),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_band_pref) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_band_pref_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, nr5g_band_pref),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_arfcn_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, nr5g_arfcn),
  NAS_NR5G_ARFCN_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr5g_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref_ext) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref_ext_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, lte_band_pref_ext),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity_ext) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity_ext_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, search_periodicity_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, scan_scope) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, scan_scope_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, scan_scope),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, search_type) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, search_type_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, search_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr_red_cap_config_type) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_req_msg_v01, nr_red_cap_config_type_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_req_msg_v01, nr_red_cap_config_type)
};

static const uint8_t nas_perform_incremental_network_scan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_perform_incremental_network_scan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, scan_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, nas_network_scan_info) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, nas_network_scan_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, nas_network_scan_info),
  ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) & 0xFF), ((NAS_3GPP_NETWORK_INFO_LIST_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, nas_network_scan_info) - QMI_IDL_OFFSET8(nas_perform_incremental_network_scan_ind_msg_v01, nas_network_scan_info_len),
  QMI_IDL_TYPE88(0, 173),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, csg_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, csg_info),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, csg_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, csg_info_len),
  QMI_IDL_TYPE88(0, 33),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nw_name_source_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, nw_name_source),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nw_name_source) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nw_name_source_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_quality) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_quality_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, signal_quality),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_quality) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_quality_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_strength) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_strength_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, signal_strength),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_strength) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, signal_strength_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, sib2_upr_layer_ind_value) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, sib2_upr_layer_ind_value_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, sib2_upr_layer_ind_value),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, sib2_upr_layer_ind_value) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, sib2_upr_layer_ind_value_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, lte_pci_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, lte_pci_plmn_info_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, lte_pci_plmn_info),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nr_pci_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nr_pci_plmn_info_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, nr_pci_plmn_info),
  NAS_NR_PCI_SCAN_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nr_pci_plmn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, nr_pci_plmn_info_len),
  QMI_IDL_TYPE88(0, 174),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_only_access) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_only_access_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, cag_only_access),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_only_access) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_only_access_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_info_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, cag_info),
  NAS_3GPP_NETWORK_INFO_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, cag_info_len),
  QMI_IDL_TYPE88(0, 34),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, snpn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, snpn_info_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_perform_incremental_network_scan_ind_msg_v01, snpn_info),
  NAS_MSNPN_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, snpn_info) - QMI_IDL_OFFSET16RELATIVE(nas_perform_incremental_network_scan_ind_msg_v01, snpn_info_len),
  QMI_IDL_TYPE88(0, 176)
};

static const uint8_t nas_set_drx_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_drx_req_msg_v01, drx)
};

static const uint8_t nas_set_drx_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_drx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_drx_req_msg is empty
 * static const uint8_t nas_get_drx_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_drx_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_drx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_drx_resp_msg_v01, drx) - QMI_IDL_OFFSET8(nas_get_drx_resp_msg_v01, drx_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_drx_resp_msg_v01, drx)
};

/*
 * nas_get_negotiated_drx_req_msg is empty
 * static const uint8_t nas_get_negotiated_drx_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_negotiated_drx_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_negotiated_drx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_negotiated_drx_resp_msg_v01, drx_level) - QMI_IDL_OFFSET8(nas_get_negotiated_drx_resp_msg_v01, drx_level_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_negotiated_drx_resp_msg_v01, drx_level)
};

static const uint8_t nas_csg_search_selection_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_timer) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_type) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, search_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, umts_band_pref) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, umts_band_pref_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, umts_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, lte_band_pref_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, csg_cat_list) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, csg_cat_list_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, csg_cat_list),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_type) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_type_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_list) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_list_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_list),
  NAS_CSG_RAT_LIST_MAX_LEN_V01,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_list) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, rat_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_signal_type) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_signal_type_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, sort_signal_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, selection_config_type) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, selection_config_type_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, selection_config_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, network_type) - QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, network_type_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_req_msg_v01, network_type)
};

static const uint8_t nas_csg_search_selection_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_csg_search_selection_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_csg_immediate_search_selection_req_msg is empty
 * static const uint8_t nas_csg_immediate_search_selection_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_csg_immediate_search_selection_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_csg_immediate_search_selection_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_csg_search_selection_configuration_req_msg is empty
 * static const uint8_t nas_get_csg_search_selection_configuration_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_csg_search_selection_configuration_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_timer) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_type) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, search_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, umts_band_pref) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, umts_band_pref_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, umts_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, lte_band_pref) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, lte_band_pref_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, lte_band_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, csg_cat_list) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, csg_cat_list_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, csg_cat_list),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_type) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_type_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_list) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_list_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_list),
  NAS_CSG_RAT_LIST_MAX_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_list) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, rat_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_signal_type) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_signal_type_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, sort_signal_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, selection_config_type) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, selection_config_type_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, selection_config_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, network_type) - QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, network_type_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_csg_search_selection_configuration_resp_msg_v01, network_type)
};

static const uint8_t nas_ssac_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, ssac_info) - QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, ssac_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, ssac_info),
  QMI_IDL_TYPE88(0, 177),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_voice) - QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_voice_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_voice),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_video) - QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_video_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_info_ind_msg_v01, barring_type_video)
};

/*
 * nas_get_lte_embms_info_req_msg is empty
 * static const uint8_t nas_get_lte_embms_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_lte_embms_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, coverage_status) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, coverage_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, coverage_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_data_mcs) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_data_mcs_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_data_mcs),
  NAS_LTE_ACTIVE_PMCH_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_data_mcs) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_data_mcs_len),
  QMI_IDL_TYPE88(0, 178),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, is_active_tmgi_valid) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, is_active_tmgi_valid_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, is_active_tmgi_valid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, mbsfn_area_signal_data) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, mbsfn_area_signal_data_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, mbsfn_area_signal_data),
  NAS_LTE_EMBMS_MBSFN_AREAS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, mbsfn_area_signal_data) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, mbsfn_area_signal_data_len),
  QMI_IDL_TYPE88(0, 179),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_bler_info) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_bler_info_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_bler_info),
  NAS_LTE_EMBMS_MBSFN_AREAS_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_bler_info) - QMI_IDL_OFFSET8(nas_get_lte_embms_info_resp_msg_v01, pmch_bler_info_len),
  QMI_IDL_TYPE88(0, 181),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_lte_embms_info_resp_msg_v01, mtch_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_lte_embms_info_resp_msg_v01, mtch_info_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_lte_embms_info_resp_msg_v01, mtch_info),
  NAS_LTE_EMBMS_ACTIVE_SESSIONS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_lte_embms_info_resp_msg_v01, mtch_info) - QMI_IDL_OFFSET16RELATIVE(nas_get_lte_embms_info_resp_msg_v01, mtch_info_len),
  QMI_IDL_TYPE88(0, 182)
};

static const uint8_t nas_get_serv_cell_sib_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_req_msg_v01, sib_num)
};

static const uint8_t nas_get_serv_cell_sib_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_serv_cell_sib_resp_msg_v01, total_size) - QMI_IDL_OFFSET8(nas_get_serv_cell_sib_resp_msg_v01, total_size_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_resp_msg_v01, total_size)
};

static const uint8_t nas_get_serv_cell_sib_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_ind_msg_v01, sequence),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_ind_msg_v01, sib_pkt),
  ((NAS_GET_SERV_CELL_SIB_MAX_V01) & 0xFF), ((NAS_GET_SERV_CELL_SIB_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_serv_cell_sib_ind_msg_v01, sib_pkt) - QMI_IDL_OFFSET8(nas_get_serv_cell_sib_ind_msg_v01, sib_pkt_len)
};

static const uint8_t nas_ssac_change_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_voice_info) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_voice_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_voice_info),
  QMI_IDL_TYPE88(0, 183),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_video_info) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_video_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, ssac_video_info),
  QMI_IDL_TYPE88(0, 185),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_voice_info) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_voice_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_voice_info),
  QMI_IDL_TYPE88(0, 184),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_video_info) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_video_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, sib2_ssac_video_info),
  QMI_IDL_TYPE88(0, 186),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_voice) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_voice_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_voice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_video) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_video_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, special_ac_barring_video),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_voice) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_voice_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_voice),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_video) - QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_video_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ssac_change_info_ind_msg_v01, barring_type_video)
};

/*
 * nas_get_ssac_info_req_msg is empty
 * static const uint8_t nas_get_ssac_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_ssac_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_voice_info) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_voice_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_voice_info),
  QMI_IDL_TYPE88(0, 183),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_video_info) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_video_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, ssac_video_info),
  QMI_IDL_TYPE88(0, 185),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_voice_info) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_voice_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_voice_info),
  QMI_IDL_TYPE88(0, 184),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_video_info) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_video_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, sib2_ssac_video_info),
  QMI_IDL_TYPE88(0, 186),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_voice) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_voice_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_voice),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_video) - QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_video_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_ssac_info_resp_msg_v01, special_ac_barring_video)
};

static const uint8_t nas_set_periodic_search_allowed_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_periodic_search_allowed_req_msg_v01, allowed)
};

static const uint8_t nas_set_periodic_search_allowed_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_periodic_search_allowed_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_emm_t3402_changed_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_emm_t3402_changed_ind_msg_v01, t3402_timer_val) - QMI_IDL_OFFSET8(nas_emm_t3402_changed_ind_msg_v01, t3402_timer_val_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_emm_t3402_changed_ind_msg_v01, t3402_timer_val)
};

/*
 * nas_get_acb_info_req_msg is empty
 * static const uint8_t nas_get_acb_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_acb_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_for_emergency) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_for_emergency_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_for_emergency),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_signalling) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_signalling_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_signalling),
  QMI_IDL_TYPE88(0, 187),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_data) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_data_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, ac_barring_mo_data),
  QMI_IDL_TYPE88(0, 187),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_voice) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_voice_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_voice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_video) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_video_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_mmtel_video),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_sms) - QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_sms_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_acb_info_resp_msg_v01, skip_ac_barring_sms)
};

static const uint8_t nas_acb_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_for_emergency) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_for_emergency_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_for_emergency),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_signalling) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_signalling_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_signalling),
  QMI_IDL_TYPE88(0, 187),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_data) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_data_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_mo_data),
  QMI_IDL_TYPE88(0, 187),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_csfb) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_csfb_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_csfb),
  QMI_IDL_TYPE88(0, 187),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_emergency) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_emergency_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_emergency),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_signalling) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_signalling_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_signalling),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_data) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_data_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_mo_data),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_csfb) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_csfb_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, ac_barring_type_csfb),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_voice) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_voice_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_voice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_video) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_video_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_mmtel_video),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_sms) - QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_sms_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_acb_info_ind_msg_v01, skip_ac_barring_sms)
};

static const uint8_t nas_set_data_subs_priority_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_data_subs_priority_req_msg_v01, data_subs_priority)
};

static const uint8_t nas_set_data_subs_priority_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_data_subs_priority_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_data_subs_priority_req_msg is empty
 * static const uint8_t nas_get_data_subs_priority_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_data_subs_priority_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_data_subs_priority_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_data_subs_priority_resp_msg_v01, data_subs_priority) - QMI_IDL_OFFSET8(nas_get_data_subs_priority_resp_msg_v01, data_subs_priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_data_subs_priority_resp_msg_v01, data_subs_priority)
};

static const uint8_t nas_data_subs_priority_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_data_subs_priority_ind_msg_v01, data_subs_priority) - QMI_IDL_OFFSET8(nas_data_subs_priority_ind_msg_v01, data_subs_priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_data_subs_priority_ind_msg_v01, data_subs_priority)
};

static const uint8_t nas_avoid_tuneaway_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_avoid_tuneaway_req_msg_v01, trm_priority)
};

static const uint8_t nas_avoid_tuneaway_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_avoid_tuneaway_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_lte_proc_type_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_lte_proc_type_req_msg_v01, proc_type)
};

static const uint8_t nas_set_lte_proc_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_lte_proc_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_mcc_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc) - QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, confidence) - QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, confidence_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, confidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc_status) - QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, mcc_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, border_mcc_list) - QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, border_mcc_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, border_mcc_list),
  NAS_MAX_BORDER_MCC_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, border_mcc_list) - QMI_IDL_OFFSET8(nas_set_mcc_req_msg_v01, border_mcc_list_len)
};

static const uint8_t nas_set_mcc_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_mcc_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_data_roaming_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_data_roaming_req_msg_v01, data_roam_status)
};

static const uint8_t nas_set_data_roaming_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_data_roaming_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_data_roaming_resp_msg_v01, pending_device_reset) - QMI_IDL_OFFSET8(nas_set_data_roaming_resp_msg_v01, pending_device_reset_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_data_roaming_resp_msg_v01, pending_device_reset)
};

/*
 * nas_get_data_roaming_req_msg is empty
 * static const uint8_t nas_get_data_roaming_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_data_roaming_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, data_roam_status) - QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, data_roam_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, data_roam_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, pending_device_reset) - QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, pending_device_reset_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_data_roaming_resp_msg_v01, pending_device_reset)
};

static const uint8_t nas_set_srvcc_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_srvcc_req_msg_v01, srvcc_status)
};

static const uint8_t nas_set_srvcc_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_srvcc_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_bsr_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_bsr_timer_req_msg_v01, bsr_value)
};

static const uint8_t nas_set_bsr_timer_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_bsr_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_bsr_timer_resp_msg_v01, delayed_until_reset) - QMI_IDL_OFFSET8(nas_set_bsr_timer_resp_msg_v01, delayed_until_reset_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_bsr_timer_resp_msg_v01, delayed_until_reset)
};

/*
 * nas_get_bsr_timer_req_msg is empty
 * static const uint8_t nas_get_bsr_timer_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_bsr_timer_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, bsr_value) - QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, bsr_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, bsr_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, delayed_until_reset) - QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, delayed_until_reset_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_bsr_timer_resp_msg_v01, delayed_until_reset)
};

static const uint8_t nas_set_drx_scaling_factor_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, drx_scaling_factor) - QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, drx_scaling_factor_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, drx_scaling_factor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, skip_idle_meas) - QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, skip_idle_meas_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, skip_idle_meas),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, idle_meas_skip_freq) - QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, idle_meas_skip_freq_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, idle_meas_skip_freq),
  NAS_MAX_TARGET_FREQUENCIES_V01,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, idle_meas_skip_freq) - QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_req_msg_v01, idle_meas_skip_freq_len),
  QMI_IDL_TYPE88(0, 188)
};

static const uint8_t nas_set_drx_scaling_factor_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_drx_scaling_factor_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_ssac_hysteresis_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_ssac_hysteresis_timer_req_msg_v01, hysteresis_timer)
};

static const uint8_t nas_set_ssac_hysteresis_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_ssac_hysteresis_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_ssac_hysteresis_timer_req_msg is empty
 * static const uint8_t nas_get_ssac_hysteresis_timer_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_ssac_hysteresis_timer_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ssac_hysteresis_timer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ssac_hysteresis_timer_resp_msg_v01, hysteresis_timer) - QMI_IDL_OFFSET8(nas_get_ssac_hysteresis_timer_resp_msg_v01, hysteresis_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_ssac_hysteresis_timer_resp_msg_v01, hysteresis_timer)
};

/*
 * nas_get_hdr_info_req_msg is empty
 * static const uint8_t nas_get_hdr_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_hdr_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, sector_id) - QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, sector_id_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, sector_id),
  NAS_HDRADDR_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, pilot_pn) - QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, pilot_pn_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, pilot_pn),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, mac_index) - QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, mac_index_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_info_resp_msg_v01, mac_index)
};

/*
 * nas_get_hdr_drc_rate_req_msg is empty
 * static const uint8_t nas_get_hdr_drc_rate_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_hdr_drc_rate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hdr_drc_rate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hdr_drc_rate_resp_msg_v01, drc_rate) - QMI_IDL_OFFSET8(nas_get_hdr_drc_rate_resp_msg_v01, drc_rate_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_hdr_drc_rate_resp_msg_v01, drc_rate)
};

static const uint8_t nas_set_rpm_parameters_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_req_msg_v01, max_resets),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_req_msg_v01, avg_reject_time)
};

static const uint8_t nas_set_rpm_parameters_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_rpm_parameters_req_msg is empty
 * static const uint8_t nas_get_rpm_parameters_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_rpm_parameters_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_resets) - QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_resets_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_resets),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, avg_reject_time) - QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, avg_reject_time_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, avg_reject_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, rpm_state) - QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, rpm_state_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, rpm_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_counter) - QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_counter_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_counter),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_time) - QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_time_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_rpm_parameters_resp_msg_v01, max_event_time)
};

static const uint8_t nas_set_rpm_parameters_2_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_counter) - QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_counter_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_counter),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_time) - QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_time_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_req_msg_v01, max_event_time)
};

static const uint8_t nas_set_rpm_parameters_2_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rpm_parameters_2_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_rpm_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_rpm_state_req_msg_v01, rpm_state)
};

static const uint8_t nas_set_rpm_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_rpm_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_lte_cphy_ca_info_req_msg is empty
 * static const uint8_t nas_get_lte_cphy_ca_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_lte_cphy_ca_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca),
  QMI_IDL_TYPE88(0, 166),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca_dl_bandwidth) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca_dl_bandwidth_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_ca_dl_bandwidth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_info),
  QMI_IDL_TYPE88(0, 167),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_info) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 169),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_idx) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_idx_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, scell_idx),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_len),
  QMI_IDL_TYPE88(0, 168),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_freq) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_freq_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, pcell_freq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_freq_list) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_freq_list_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_freq_list),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_freq_list) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_freq_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, ul_configured) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, ul_configured_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, ul_configured),
  NAS_MAX_SCELL_LIST_LEN_V01,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, ul_configured) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, ul_configured_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_ext) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_ext_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_ext),
  NAS_MAX_SCELL_LIST_LEN_EXT_V01,
  QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_ext) - QMI_IDL_OFFSET8(nas_get_lte_cphy_ca_info_resp_msg_v01, cphy_scell_info_list_ext_len),
  QMI_IDL_TYPE88(0, 170)
};

/*
 * nas_manual_scan_fail_ind_msg is empty
 * static const uint8_t nas_manual_scan_fail_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_set_cell_lock_config_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_cell_lock_config_req_msg_v01, cell_list),
  NAS_MAX_CELL_LOCK_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_set_cell_lock_config_req_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_set_cell_lock_config_req_msg_v01, cell_list_len),
  QMI_IDL_TYPE88(0, 189),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_cell_lock_config_req_msg_v01, enforce) - QMI_IDL_OFFSET16RELATIVE(nas_set_cell_lock_config_req_msg_v01, enforce_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_cell_lock_config_req_msg_v01, enforce)
};

static const uint8_t nas_set_cell_lock_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_cell_lock_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_cell_config_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_cell_config_req_msg_v01, cell_list),
  NAS_MAX_CELL_LOCK_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_set_cell_config_req_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_set_cell_config_req_msg_v01, cell_list_len),
  QMI_IDL_TYPE88(0, 190),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_cell_config_req_msg_v01, enforce) - QMI_IDL_OFFSET16RELATIVE(nas_set_cell_config_req_msg_v01, enforce_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_set_cell_config_req_msg_v01, enforce)
};

static const uint8_t nas_set_cell_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_cell_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_nr5g_cell_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, cell_identity) - QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, cell_identity_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, cell_identity),
  QMI_IDL_TYPE88(0, 191),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, arfcn_list) - QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, arfcn_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, arfcn_list),
  NAS_MAX_NR5G_CONFIG_ARFCN_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, arfcn_list) - QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_req_msg_v01, arfcn_list_len),
  QMI_IDL_TYPE88(0, 192),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_set_nr5g_cell_config_req_msg_v01, multi_pci_list) - QMI_IDL_OFFSET16RELATIVE(nas_set_nr5g_cell_config_req_msg_v01, multi_pci_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_set_nr5g_cell_config_req_msg_v01, multi_pci_list),
  NAS_MAX_PCI_FREQ_LIST_ENTRIES_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_set_nr5g_cell_config_req_msg_v01, multi_pci_list) - QMI_IDL_OFFSET16RELATIVE(nas_set_nr5g_cell_config_req_msg_v01, multi_pci_list_len),
  QMI_IDL_TYPE88(0, 221)
};

static const uint8_t nas_set_nr5g_cell_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_cell_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nr5g_cell_config_req_msg is empty
 * static const uint8_t nas_get_nr5g_cell_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_cell_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, type) - QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, cell_identity) - QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, cell_identity_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, cell_identity),
  QMI_IDL_TYPE88(0, 191),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, arfcn_list) - QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, arfcn_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, arfcn_list),
  NAS_MAX_NR5G_CONFIG_ARFCN_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, arfcn_list) - QMI_IDL_OFFSET8(nas_get_nr5g_cell_config_resp_msg_v01, arfcn_list_len),
  QMI_IDL_TYPE88(0, 192),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_cell_config_resp_msg_v01, multi_pci_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_cell_config_resp_msg_v01, multi_pci_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_nr5g_cell_config_resp_msg_v01, multi_pci_list),
  NAS_MAX_PCI_FREQ_LIST_ENTRIES_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_cell_config_resp_msg_v01, multi_pci_list) - QMI_IDL_OFFSET16RELATIVE(nas_get_nr5g_cell_config_resp_msg_v01, multi_pci_list_len),
  QMI_IDL_TYPE88(0, 221)
};

static const uint8_t nas_lte_ue_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, lte_ue_category) - QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, lte_ue_category_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, lte_ue_category),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, disable_ca) - QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, disable_ca_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, disable_ca),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, mdt_enable) - QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, mdt_enable_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, mdt_enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, gnss_enable) - QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, gnss_enable_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_ue_config_req_msg_v01, gnss_enable)
};

static const uint8_t nas_lte_ue_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_ue_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_timer_expiry_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_timer_expiry_ind_v01, timer_id)
};

static const uint8_t nas_timer_start_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_timer_start_ind_v01, timer_id)
};

static const uint8_t nas_timer_stop_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_timer_stop_ind_v01, timer_id)
};

static const uint8_t nas_emergency_mode_status_ind_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_emergency_mode_status_ind_v01, emergency_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_emergency_mode_status_ind_v01, is_ecbm_required) - QMI_IDL_OFFSET8(nas_emergency_mode_status_ind_v01, is_ecbm_required_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_emergency_mode_status_ind_v01, is_ecbm_required)
};

/*
 * nas_ecall_deregistration_req_msg is empty
 * static const uint8_t nas_ecall_deregistration_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_ecall_deregistration_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ecall_deregistration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_update_ca_band_combo_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, ca_band_combo) - QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, ca_band_combo_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, ca_band_combo),
  ((NAS_CA_BAND_COMBO_MAX_LEN_V01) & 0xFF), ((NAS_CA_BAND_COMBO_MAX_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, ca_band_combo) - QMI_IDL_OFFSET8(nas_update_ca_band_combo_req_msg_v01, ca_band_combo_len)
};

static const uint8_t nas_update_ca_band_combo_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_ca_band_combo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_ca_band_combo_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ca_band_combo_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t nas_get_ca_band_combo_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, ca_band_combo) - QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, ca_band_combo_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, ca_band_combo),
  ((NAS_CA_BAND_COMBO_MAX_LEN_V01) & 0xFF), ((NAS_CA_BAND_COMBO_MAX_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, ca_band_combo) - QMI_IDL_OFFSET8(nas_get_ca_band_combo_resp_msg_v01, ca_band_combo_len)
};

static const uint8_t nas_ecall_timer_restart_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ecall_timer_restart_req_msg_v01, timer_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ecall_timer_restart_req_msg_v01, duration)
};

static const uint8_t nas_ecall_timer_restart_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ecall_timer_restart_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_voice_roaming_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_voice_roaming_req_msg_v01, international_voice_roaming_allowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_voice_roaming_req_msg_v01, domestic_voice_roaming_allowed) - QMI_IDL_OFFSET8(nas_set_voice_roaming_req_msg_v01, domestic_voice_roaming_allowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_voice_roaming_req_msg_v01, domestic_voice_roaming_allowed)
};

static const uint8_t nas_set_voice_roaming_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_voice_roaming_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_voice_roaming_req_msg is empty
 * static const uint8_t nas_get_voice_roaming_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_voice_roaming_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, international_voice_roaming_allowed) - QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, international_voice_roaming_allowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, international_voice_roaming_allowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, domestic_voice_roaming_allowed) - QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, domestic_voice_roaming_allowed_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_voice_roaming_resp_msg_v01, domestic_voice_roaming_allowed)
};

static const uint8_t nas_set_edrx_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_edrx_req_msg_v01, edrx_params)
};

static const uint8_t nas_set_edrx_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_edrx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_edrx_params_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_enabled) - QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_cycle_length) - QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_cycle_length_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_cycle_length),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ptw) - QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ptw_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ptw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_rat_type) - QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_rat_type_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ciot_lte_mode) - QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ciot_lte_mode_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_edrx_params_req_msg_v01, edrx_ciot_lte_mode)
};

static const uint8_t nas_set_edrx_params_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_edrx_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_edrx_params_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_rat_type) - QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_rat_type_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_ciot_lte_mode) - QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_ciot_lte_mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_req_msg_v01, edrx_ciot_lte_mode)
};

static const uint8_t nas_get_edrx_params_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_cycle_length) - QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_cycle_length_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_cycle_length),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ptw) - QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ptw_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ptw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_enabled) - QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_rat_type) - QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_rat_type_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ciot_lte_mode) - QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ciot_lte_mode_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_params_resp_msg_v01, edrx_ciot_lte_mode)
};

/*
 * nas_get_edrx_req_msg is empty
 * static const uint8_t nas_get_edrx_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_edrx_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_edrx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_edrx_resp_msg_v01, edrx_params) - QMI_IDL_OFFSET8(nas_get_edrx_resp_msg_v01, edrx_params_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_edrx_resp_msg_v01, edrx_params)
};

static const uint8_t nas_edrx_change_info_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_enabled) - QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_cycle_length) - QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_cycle_length_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_cycle_length),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ptw) - QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ptw_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ptw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_rat_type) - QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_rat_type_valid)),
  0x13,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ciot_lte_mode) - QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ciot_lte_mode_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_edrx_change_info_ind_v01, edrx_ciot_lte_mode)
};

static const uint8_t nas_set_gnss_session_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_gnss_session_status_req_msg_v01, gnss_session_active)
};

static const uint8_t nas_set_gnss_session_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_gnss_session_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_lte_rrc_tx_info_req_msg is empty
 * static const uint8_t nas_get_lte_rrc_tx_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_lte_rrc_tx_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_is_active) - QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_is_active_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_is_active),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_band) - QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_band_valid)),
  0x11,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_lte_rrc_tx_info_resp_msg_v01, lte_tx_band)
};

static const uint8_t nas_lte_rrc_tx_info_ind_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rrc_tx_info_ind_v01, lte_tx_is_active),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_rrc_tx_info_ind_v01, lte_tx_band) - QMI_IDL_OFFSET8(nas_lte_rrc_tx_info_ind_v01, lte_tx_band_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_lte_rrc_tx_info_ind_v01, lte_tx_band)
};

static const uint8_t nas_gcell_info_ind_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, uid_camped_cell),
  QMI_IDL_TYPE88(0, 193),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, broadcast_si_mask) - QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, broadcast_si_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, broadcast_si_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, si1_cell_allocation) - QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, si1_cell_allocation_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, si1_cell_allocation),
  NAS_MAX_GERAN_CELL_ALLOCATION_V01,
  QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, si1_cell_allocation) - QMI_IDL_OFFSET8(nas_gcell_info_ind_v01, si1_cell_allocation_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, ra_color) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, ra_color_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, ra_color),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, reselection_offset) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, reselection_offset_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, reselection_offset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, t3212) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, t3212_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, t3212),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, rx_lev_min) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, rx_lev_min_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, rx_lev_min),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, si13_position) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, si13_position_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, si13_position),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, MSCR) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, MSCR_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, MSCR),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, mandatory_si_status) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, mandatory_si_status_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, mandatory_si_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, gsm_neighbor_cell_unique_arfcn_list) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, gsm_neighbor_cell_unique_arfcn_list_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, gsm_neighbor_cell_unique_arfcn_list),
  NAS_BA_MAX_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, gsm_neighbor_cell_unique_arfcn_list) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, gsm_neighbor_cell_unique_arfcn_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, cell_reselect_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, cell_reselect_hysteresis_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, cell_reselect_hysteresis),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, ms_txpwr_max_cch) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, ms_txpwr_max_cch_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, ms_txpwr_max_cch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, temporary_offset) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, temporary_offset_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, temporary_offset),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, penalty_time) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_ind_v01, penalty_time_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_ind_v01, penalty_time)
};

/*
 * nas_gcell_info_req_msg is empty
 * static const uint8_t nas_gcell_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_gcell_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, uid_camped_cell),
  QMI_IDL_TYPE88(0, 193),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, broadcast_si_mask) - QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, broadcast_si_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, broadcast_si_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, si1_cell_allocation) - QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, si1_cell_allocation_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, si1_cell_allocation),
  NAS_MAX_GERAN_CELL_ALLOCATION_V01,
  QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, si1_cell_allocation) - QMI_IDL_OFFSET8(nas_gcell_info_resp_msg_v01, si1_cell_allocation_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, ra_color) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, ra_color_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, ra_color),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, reselection_offset) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, reselection_offset_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, reselection_offset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, t3212) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, t3212_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, t3212),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, rx_lev_min) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, rx_lev_min_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, rx_lev_min),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, si13_position) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, si13_position_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, si13_position),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, MSCR) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, MSCR_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, MSCR),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, mandatory_si_status) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, mandatory_si_status_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, mandatory_si_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, gsm_neighbor_cell_unique_arfcn_list) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, gsm_neighbor_cell_unique_arfcn_list_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, gsm_neighbor_cell_unique_arfcn_list),
  NAS_BA_MAX_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, gsm_neighbor_cell_unique_arfcn_list) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, gsm_neighbor_cell_unique_arfcn_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, cell_reselect_hysteresis) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, cell_reselect_hysteresis_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, cell_reselect_hysteresis),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, ms_txpwr_max_cch) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, ms_txpwr_max_cch_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, ms_txpwr_max_cch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, temporary_offset) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, temporary_offset_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, temporary_offset),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, penalty_time) - QMI_IDL_OFFSET16RELATIVE(nas_gcell_info_resp_msg_v01, penalty_time_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_gcell_info_resp_msg_v01, penalty_time)
};

static const uint8_t nas_bar_gcell_req_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_bar_gcell_req_v01, uid_camped_cell),
  QMI_IDL_TYPE88(0, 193),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_bar_gcell_req_v01, bar_time)
};

static const uint8_t nas_bar_gcell_resp_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_bar_gcell_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_abort_scan_req_msg is empty
 * static const uint8_t nas_abort_scan_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_abort_scan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_abort_scan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_lte_cphy_ca_agg_dl_bw_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, num_scells_configured) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, num_scells_configured_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01, num_scells_configured)
};

static const uint8_t nas_lte_rach_fail_ind_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail),
  QMI_IDL_TYPE88(0, 194),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_dl_earfcn) - QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_dl_earfcn_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_dl_earfcn),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_ul_earfcn) - QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_ul_earfcn_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_rach_fail_ind_v01, lte_rach_fail_ul_earfcn)
};

static const uint8_t nas_set_ecid_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_ecid_config_req_msg_v01, ecid_is_enabled) - QMI_IDL_OFFSET8(nas_set_ecid_config_req_msg_v01, ecid_is_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_ecid_config_req_msg_v01, ecid_is_enabled)
};

static const uint8_t nas_set_ecid_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_ecid_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_ecid_config_req_msg is empty
 * static const uint8_t nas_get_ecid_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_ecid_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_ecid_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_ecid_config_resp_msg_v01, ecid_is_enabled) - QMI_IDL_OFFSET8(nas_get_ecid_config_resp_msg_v01, ecid_is_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_ecid_config_resp_msg_v01, ecid_is_enabled)
};

static const uint8_t nas_v2x_start_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_start_req_msg_v01, concurrent_with_WWAN) - QMI_IDL_OFFSET8(nas_v2x_start_req_msg_v01, concurrent_with_WWAN_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_start_req_msg_v01, concurrent_with_WWAN)
};

static const uint8_t nas_v2x_start_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_v2x_stop_req_msg is empty
 * static const uint8_t nas_v2x_stop_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_v2x_stop_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_v2x_status_req_msg is empty
 * static const uint8_t nas_get_v2x_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_v2x_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_multi_pool_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_multi_pool_status_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_multi_pool_status),
  NAS_V2X_RRC_MAX_TX_POOL_LIST_V01,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_multi_pool_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, tx_multi_pool_status_len),
  QMI_IDL_TYPE88(0, 195),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_multi_pool_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_multi_pool_status_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_multi_pool_status),
  NAS_V2X_RRC_MAX_RX_POOL_LIST_V01,
  QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_multi_pool_status) - QMI_IDL_OFFSET8(nas_get_v2x_status_resp_msg_v01, rx_multi_pool_status_len),
  QMI_IDL_TYPE88(0, 195)
};

static const uint8_t nas_v2x_notification_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_cause) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_cause_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_cause) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_cause_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_cause),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_value) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_value_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_multi_pool_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_multi_pool_status_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_multi_pool_status),
  NAS_V2X_RRC_MAX_TX_POOL_LIST_V01,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_multi_pool_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, tx_multi_pool_status_len),
  QMI_IDL_TYPE88(0, 195),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_multi_pool_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_multi_pool_status_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_multi_pool_status),
  NAS_V2X_RRC_MAX_RX_POOL_LIST_V01,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_multi_pool_status) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, rx_multi_pool_status_len),
  QMI_IDL_TYPE88(0, 195),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_multi_pool_value) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_multi_pool_value_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_multi_pool_value),
  NAS_V2X_RRC_MAX_RX_POOL_LIST_V01,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_multi_pool_value) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, cbr_multi_pool_value_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, time_uncertainty) - QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, time_uncertainty_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_notification_ind_v01, time_uncertainty)
};

static const uint8_t nas_sub_blocked_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_sub_blocked_status_ind_msg_v01, is_blocked)
};

/*
 * nas_get_sub_blocked_status_req_msg is empty
 * static const uint8_t nas_get_sub_blocked_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_sub_blocked_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sub_blocked_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_sub_blocked_status_resp_msg_v01, is_blocked) - QMI_IDL_OFFSET8(nas_get_sub_blocked_status_resp_msg_v01, is_blocked_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_sub_blocked_status_resp_msg_v01, is_blocked)
};

static const uint8_t nas_lte_b2_thresh_adj_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_req_msg_v01, config_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_req_msg_v01, lte_b2_thresh_adj_info) - QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_req_msg_v01, lte_b2_thresh_adj_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_req_msg_v01, lte_b2_thresh_adj_info),
  QMI_IDL_TYPE88(0, 196)
};

static const uint8_t nas_lte_b2_thresh_adj_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_resp_msg_v01, config_rsp) - QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_resp_msg_v01, config_rsp_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lte_b2_thresh_adj_config_resp_msg_v01, config_rsp)
};

/*
 * nas_get_arfcn_list_req_msg is empty
 * static const uint8_t nas_get_arfcn_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_arfcn_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_scell_arfcn) - QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_scell_arfcn_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_scell_arfcn),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_nbr_arfcn) - QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_nbr_arfcn_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_nbr_arfcn),
  NAS_MAX_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_nbr_arfcn) - QMI_IDL_OFFSET8(nas_get_arfcn_list_resp_msg_v01, geran_nbr_arfcn_len),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_hopping_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_hopping_arfcn_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, geran_hopping_arfcn),
  NAS_MAX_HOPPING_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_hopping_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_hopping_arfcn_len),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_dedicated_or_pdch_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_dedicated_or_pdch_arfcn_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, geran_dedicated_or_pdch_arfcn),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_scell_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_scell_uarfcn_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, wcdma_scell_uarfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_secondary_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_secondary_uarfcn_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, wcdma_secondary_uarfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_rrc_state_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, wcdma_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_nbr_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_nbr_uarfcn_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, wcdma_nbr_uarfcn),
  NAS_MAX_WCDMA_NBR_UARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_nbr_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, wcdma_nbr_uarfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_scell_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_scell_earfcn_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, lte_scell_earfcn),
  NAS_MAX_LTE_SERVING_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_scell_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_scell_earfcn_len),
  QMI_IDL_TYPE88(0, 198),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_rrc_state_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, lte_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_nbr_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_nbr_earfcn_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, lte_nbr_earfcn),
  NAS_MAX_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_nbr_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, lte_nbr_earfcn_len),
  QMI_IDL_TYPE88(0, 198),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_rrc_state_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, nr5g_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_scell_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_scell_arfcn_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, nr5g_scell_arfcn),
  NAS_MAX_NR5G_SERVING_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_scell_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_scell_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_nbr_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_nbr_arfcn_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, nr5g_nbr_arfcn),
  NAS_MAX_NR5G_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_nbr_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, nr5g_nbr_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count_exceeding_thresh) - QMI_IDL_OFFSET16RELATIVE(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count_exceeding_thresh_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_get_arfcn_list_resp_msg_v01, geran_tx_burst_count_exceeding_thresh)
};

static const uint8_t nas_arfcn_list_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_scell_arfcn) - QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_scell_arfcn_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_scell_arfcn),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_nbr_arfcn) - QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_nbr_arfcn_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_nbr_arfcn),
  NAS_MAX_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_nbr_arfcn) - QMI_IDL_OFFSET8(nas_arfcn_list_ind_v01, geran_nbr_arfcn_len),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_hopping_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_hopping_arfcn_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, geran_hopping_arfcn),
  NAS_MAX_HOPPING_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_hopping_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_hopping_arfcn_len),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_dedicated_or_pdch_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_dedicated_or_pdch_arfcn_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, geran_dedicated_or_pdch_arfcn),
  QMI_IDL_TYPE88(0, 197),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_scell_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_scell_uarfcn_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, wcdma_scell_uarfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_secondary_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_secondary_uarfcn_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, wcdma_secondary_uarfcn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_rrc_state_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, wcdma_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_nbr_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_nbr_uarfcn_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, wcdma_nbr_uarfcn),
  NAS_MAX_WCDMA_NBR_UARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_nbr_uarfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, wcdma_nbr_uarfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_scell_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_scell_earfcn_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, lte_scell_earfcn),
  NAS_MAX_LTE_SERVING_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_scell_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_scell_earfcn_len),
  QMI_IDL_TYPE88(0, 198),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_rrc_state_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, lte_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_nbr_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_nbr_earfcn_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, lte_nbr_earfcn),
  NAS_MAX_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_nbr_earfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, lte_nbr_earfcn_len),
  QMI_IDL_TYPE88(0, 198),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_rrc_state) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_rrc_state_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, nr5g_rrc_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_scell_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_scell_arfcn_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, nr5g_scell_arfcn),
  NAS_MAX_NR5G_SERVING_CELL_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_scell_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_scell_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_nbr_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_nbr_arfcn_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, nr5g_nbr_arfcn),
  NAS_MAX_NR5G_NBR_ARFCN_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_nbr_arfcn) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, nr5g_nbr_arfcn_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_tx_burst_count) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_tx_burst_count_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, geran_tx_burst_count),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_tx_burst_count_exceeding_thresh) - QMI_IDL_OFFSET16RELATIVE(nas_arfcn_list_ind_v01, geran_tx_burst_count_exceeding_thresh_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_arfcn_list_ind_v01, geran_tx_burst_count_exceeding_thresh)
};

static const uint8_t nas_5g_modem_vote_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_5g_modem_vote_ind_v01, vote_ind_for_5g_modem)
};

/*
 * nas_get_rf_availability_req_msg is empty
 * static const uint8_t nas_get_rf_availability_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_rf_availability_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_rf_availability_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_rf_availability_resp_msg_v01, gpp_rf_available) - QMI_IDL_OFFSET8(nas_get_rf_availability_resp_msg_v01, gpp_rf_available_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_rf_availability_resp_msg_v01, gpp_rf_available)
};

static const uint8_t nas_rf_availability_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_rf_availability_ind_msg_v01, gpp_rf_available)
};

/*
 * nas_get_cell_lock_config_req_msg is empty
 * static const uint8_t nas_get_cell_lock_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cell_lock_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, cell_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, cell_list),
  NAS_MAX_CELL_LOCK_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_get_cell_lock_config_resp_msg_v01, cell_list_len),
  QMI_IDL_TYPE88(0, 189)
};

static const uint8_t nas_set_small_bw_earfcn_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_req_msg_v01, small_bw_earfcn) - QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_req_msg_v01, small_bw_earfcn_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_req_msg_v01, small_bw_earfcn),
  NAS_SMALL_BW_EARFCN_MAX_V01,
  QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_req_msg_v01, small_bw_earfcn) - QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_req_msg_v01, small_bw_earfcn_len)
};

static const uint8_t nas_set_small_bw_earfcn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_small_bw_earfcn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_small_bw_earfcn_req_msg is empty
 * static const uint8_t nas_get_small_bw_earfcn_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_small_bw_earfcn_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, small_bw_earfcn) - QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, small_bw_earfcn_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, small_bw_earfcn),
  NAS_SMALL_BW_EARFCN_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, small_bw_earfcn) - QMI_IDL_OFFSET8(nas_get_small_bw_earfcn_resp_msg_v01, small_bw_earfcn_len)
};

/*
 * nas_force_lte_scan_req_msg is empty
 * static const uint8_t nas_force_lte_scan_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_force_lte_scan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_force_lte_scan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_block_nr5g_plmn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_abs) - QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_abs_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_abs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_mult) - QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_mult_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, blocking_interval_mult),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, detach_required) - QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, detach_required_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, detach_required),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, block_specific_plmn_only) - QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, block_specific_plmn_only_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_req_msg_v01, block_specific_plmn_only)
};

static const uint8_t nas_block_nr5g_plmn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_block_nr5g_plmn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_unblock_nr5g_plmn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_unblock_nr5g_plmn_req_msg_v01, plmn),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_unblock_nr5g_plmn_req_msg_v01, unblock_specific_plmn_only) - QMI_IDL_OFFSET8(nas_unblock_nr5g_plmn_req_msg_v01, unblock_specific_plmn_only_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_unblock_nr5g_plmn_req_msg_v01, unblock_specific_plmn_only)
};

static const uint8_t nas_unblock_nr5g_plmn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_unblock_nr5g_plmn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_reset_nr5g_plmn_blocking_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_reset_nr5g_plmn_blocking_req_msg_v01, reset_specific_plmn_only) - QMI_IDL_OFFSET8(nas_reset_nr5g_plmn_blocking_req_msg_v01, reset_specific_plmn_only_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_reset_nr5g_plmn_blocking_req_msg_v01, reset_specific_plmn_only)
};

static const uint8_t nas_reset_nr5g_plmn_blocking_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_reset_nr5g_plmn_blocking_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_mm5g_t3502_changed_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_mm5g_t3502_changed_ind_msg_v01, t3502_timer_val) - QMI_IDL_OFFSET8(nas_mm5g_t3502_changed_ind_msg_v01, t3502_timer_val_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_mm5g_t3502_changed_ind_msg_v01, t3502_timer_val)
};

static const uint8_t nas_set_ims_proc_type_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_ims_proc_type_req_msg_v01, proc_type)
};

static const uint8_t nas_set_ims_proc_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_ims_proc_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_endc_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_endc_config_req_msg_v01, enable_endc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_endc_config_req_msg_v01, immediate_scg_rel) - QMI_IDL_OFFSET8(nas_set_endc_config_req_msg_v01, immediate_scg_rel_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_endc_config_req_msg_v01, immediate_scg_rel)
};

static const uint8_t nas_set_endc_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_endc_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_endc_config_req_msg is empty
 * static const uint8_t nas_get_endc_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_endc_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, enable_endc) - QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, enable_endc_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, enable_endc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, immediate_scg_rel) - QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, immediate_scg_rel_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_endc_config_resp_msg_v01, immediate_scg_rel)
};

static const uint8_t nas_disable_iccid_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_disable_iccid_req_msg_v01, iccid) - QMI_IDL_OFFSET8(nas_disable_iccid_req_msg_v01, iccid_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_disable_iccid_req_msg_v01, iccid),
  NAS_MMGSDI_ICCID_LEN_V01,
  QMI_IDL_OFFSET8(nas_disable_iccid_req_msg_v01, iccid) - QMI_IDL_OFFSET8(nas_disable_iccid_req_msg_v01, iccid_len)
};

static const uint8_t nas_disable_iccid_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_disable_iccid_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_disable_iccid_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_disable_iccid_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(nas_disable_iccid_ind_msg_v01, iccid_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_disable_iccid_ind_msg_v01, iccid),
  NAS_MMGSDI_ICCID_LEN_V01,
  QMI_IDL_OFFSET8(nas_disable_iccid_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(nas_disable_iccid_ind_msg_v01, iccid_len)
};

static const uint8_t nas_get_next_sfn_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_next_sfn_req_msg_v01, radio_access_technology)
};

static const uint8_t nas_get_next_sfn_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_next_sfn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_next_sfn_resp_msg_v01, hsfn_sfn_nbr) - QMI_IDL_OFFSET8(nas_get_next_sfn_resp_msg_v01, hsfn_sfn_nbr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_next_sfn_resp_msg_v01, hsfn_sfn_nbr)
};

static const uint8_t nas_set_sfn_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_set_sfn_config_req_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_sfn_config_req_msg_v01, hsfn_sfn_nbr)
};

static const uint8_t nas_set_sfn_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_sfn_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_sfn_threshold_reached_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_sfn_threshold_reached_ind_msg_v01, hsfn_sfn_nbr)
};

static const uint8_t nas_nr5g_drx_config_param_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_inactivity_timer) - QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_inactivity_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_inactivity_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_long_cycle) - QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_long_cycle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_long_cycle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_short_cycle) - QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_short_cycle_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_drx_config_param_ind_msg_v01, drx_short_cycle)
};

static const uint8_t nas_set_v2x_peak_tx_pwr_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_v2x_peak_tx_pwr_req_msg_v01, global_peak_tx_pwr)
};

static const uint8_t nas_set_v2x_peak_tx_pwr_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_v2x_peak_tx_pwr_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_pps_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, current_panel_pos_index),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, is_pps_needed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, nr5g_pps_freq_info) - QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, nr5g_pps_freq_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, nr5g_pps_freq_info),
  NAS_MAX_NR5G_PPS_FREQ_INFO_LIST_V01,
  QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, nr5g_pps_freq_info) - QMI_IDL_OFFSET8(nas_pps_config_req_msg_v01, nr5g_pps_freq_info_len),
  QMI_IDL_TYPE88(0, 199)
};

static const uint8_t nas_pps_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_pps_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_pps_config_resp_msg_v01, pps_mode) - QMI_IDL_OFFSET8(nas_pps_config_resp_msg_v01, pps_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_pps_config_resp_msg_v01, pps_mode)
};

static const uint8_t nas_panel_pos_change_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_ind_msg_v01, panel_pos_index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_ind_msg_v01, pps_mode)
};

static const uint8_t nas_panel_pos_change_status_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_status_msg_v01, is_panel_pos_changed),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_status_msg_v01, panel_pos_index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_status_msg_v01, pps_mode)
};

static const uint8_t nas_panel_pos_change_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_panel_pos_change_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_get_speff_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_speff_req_msg_v01, is_ant_sel_changed) - QMI_IDL_OFFSET8(nas_get_speff_req_msg_v01, is_ant_sel_changed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_speff_req_msg_v01, is_ant_sel_changed)
};

static const uint8_t nas_get_speff_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_speff_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_speff_resp_msg_v01, speff) - QMI_IDL_OFFSET8(nas_get_speff_resp_msg_v01, speff_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_speff_resp_msg_v01, speff)
};

static const uint8_t nas_get_nr5g_speff_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_speff_req_msg_v01, is_ant_sel_changed) - QMI_IDL_OFFSET8(nas_get_nr5g_speff_req_msg_v01, is_ant_sel_changed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_speff_req_msg_v01, is_ant_sel_changed)
};

static const uint8_t nas_get_nr5g_speff_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_speff_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_speff_resp_msg_v01, speff) - QMI_IDL_OFFSET8(nas_get_nr5g_speff_resp_msg_v01, speff_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_speff_resp_msg_v01, speff)
};

static const uint8_t nas_jamming_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_jamming_status_req_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_req_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_jamming_status_req_msg_v01, ciot_lte_op_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_req_msg_v01, ciot_lte_op_mode)
};

static const uint8_t nas_jamming_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, ciot_lte_op_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, jammer_status) - QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, jammer_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, jammer_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, soft_jammer_flag) - QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, soft_jammer_flag_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_resp_msg_v01, soft_jammer_flag)
};

static const uint8_t nas_jamming_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, ciot_lte_op_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, jammer_status) - QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, jammer_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, jammer_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, soft_jammer_flag) - QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, soft_jammer_flag_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_jamming_status_ind_msg_v01, soft_jammer_flag)
};

/*
 * nas_ce_level_info_req_msg is empty
 * static const uint8_t nas_ce_level_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_ce_level_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, active_rat) - QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, active_rat_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, active_rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ciot_lte_op_mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ce_level) - QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ce_level_valid)),
  0x12,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_ce_level_info_resp_msg_v01, ce_level)
};

static const uint8_t nas_wwan_sleep_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_wwan_sleep_info_ind_msg_v01, sleep_duration)
};

static const uint8_t nas_set_wwan_sleep_threshold_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_wwan_sleep_threshold_req_msg_v01, sleep_duration_threshold)
};

static const uint8_t nas_set_wwan_sleep_threshold_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_wwan_sleep_threshold_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_wwan_sleep_threshold_req_msg is empty
 * static const uint8_t nas_get_wwan_sleep_threshold_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_wwan_sleep_threshold_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_wwan_sleep_threshold_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_wwan_sleep_threshold_resp_msg_v01, sleep_duration_threshold) - QMI_IDL_OFFSET8(nas_get_wwan_sleep_threshold_resp_msg_v01, sleep_duration_threshold_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_wwan_sleep_threshold_resp_msg_v01, sleep_duration_threshold)
};

static const uint8_t nas_fsk_data_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_data_req_msg_v01, data),
  NAS_FSK_MAX_BYTES_TO_SEND_V01,
  QMI_IDL_OFFSET8(nas_fsk_data_req_msg_v01, data) - QMI_IDL_OFFSET8(nas_fsk_data_req_msg_v01, data_len)
};

static const uint8_t nas_fsk_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_fsk_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_fsk_hop_table_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_hop_table_req_msg_v01, index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_fsk_hop_table_req_msg_v01, fsk_hop_table),
  QMI_IDL_TYPE88(0, 32)
};

static const uint8_t nas_set_fsk_hop_table_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_fsk_hop_table_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_fsk_hop_table_req_msg is empty
 * static const uint8_t nas_get_fsk_hop_table_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_fsk_hop_table_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, fsk_hop_table) - QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, fsk_hop_table_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, fsk_hop_table),
  NAS_FSK_MAX_FREQS_V01,
  QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, fsk_hop_table) - QMI_IDL_OFFSET8(nas_get_fsk_hop_table_resp_msg_v01, fsk_hop_table_len),
  QMI_IDL_TYPE88(0, 32)
};

static const uint8_t nas_fsk_start_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, baudrate) - QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, baudrate_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, baudrate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, gaptime) - QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, gaptime_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, gaptime),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, restart_mode) - QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, restart_mode_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_fsk_start_req_msg_v01, restart_mode)
};

static const uint8_t nas_fsk_start_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_fsk_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_fsk_debug_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, freq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, delta) - QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, delta_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, delta),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, baudrate) - QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, baudrate_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_debug_req_msg_v01, baudrate)
};

static const uint8_t nas_set_fsk_debug_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_fsk_debug_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_fsk_debug_req_msg is empty
 * static const uint8_t nas_get_fsk_debug_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_fsk_debug_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, delta) - QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, delta_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, delta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, freq) - QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, freq_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, freq),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, baudrate) - QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, baudrate_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_debug_resp_msg_v01, baudrate)
};

/*
 * nas_fsk_stop_req_msg is empty
 * static const uint8_t nas_fsk_stop_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_fsk_stop_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_fsk_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_fsk_pcl_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_gsm) - QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_gsm_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_gsm),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_dcs) - QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_dcs_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_dcs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_pcs) - QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_pcs_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_fsk_pcl_req_msg_v01, pcl_pcs)
};

static const uint8_t nas_set_fsk_pcl_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_fsk_pcl_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_fsk_pcl_req_msg is empty
 * static const uint8_t nas_get_fsk_pcl_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_fsk_pcl_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_gsm) - QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_gsm_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_gsm),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_dcs) - QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_dcs_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_dcs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_pcs) - QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_pcs_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_fsk_pcl_resp_msg_v01, pcl_pcs)
};

/*
 * nas_get_cell_select_info_req_msg is empty
 * static const uint8_t nas_get_cell_select_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cell_select_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, radio_access_technology) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, radio_access_technology_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, radio_access_technology),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, ciot_lte_op_mode) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, ciot_lte_op_mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, ciot_lte_op_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_rxlevmin) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_rxlevmin_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_rxlevmin),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce1) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce1_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, qrxlevmin_ce1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, rxlev_access_min) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, rxlev_access_min_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, rxlev_access_min),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_hyst) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_hyst_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_hyst),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_qualmin) - QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_qualmin_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_cell_select_info_resp_msg_v01, q_qualmin)
};

static const uint8_t nas_set_sync_pulse_gen_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_sync_pulse_gen_req_msg_v01, start_sfn),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_sync_pulse_gen_req_msg_v01, pulse_period)
};

static const uint8_t nas_set_sync_pulse_gen_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_sync_pulse_gen_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_sync_pulse_gen_req_msg is empty
 * static const uint8_t nas_get_sync_pulse_gen_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_sync_pulse_gen_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_sync_pulse_gen_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_sync_pulse_gen_resp_msg_v01, ta_val),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_sync_pulse_gen_resp_msg_v01, last_sfn)
};

static const uint8_t nas_lost_frame_sync_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_lost_frame_sync_ind_msg_v01, sync_lost_reason)
};

static const uint8_t nas_set_dss_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, dss_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, plmn_list) - QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, plmn_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, plmn_list),
  NAS_DSS_PLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, plmn_list) - QMI_IDL_OFFSET8(nas_set_dss_config_req_msg_v01, plmn_list_len),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t nas_set_dss_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_dss_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_dss_config_req_msg is empty
 * static const uint8_t nas_get_dss_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_dss_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, dss_enabled) - QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, dss_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, dss_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, plmn_list) - QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, plmn_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, plmn_list),
  NAS_DSS_PLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, plmn_list) - QMI_IDL_OFFSET8(nas_get_dss_config_resp_msg_v01, plmn_list_len),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t nas_set_nr5g_full_voice_support_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_full_voice_support_req_msg_v01, nr5g_full_voice_support)
};

static const uint8_t nas_set_nr5g_full_voice_support_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_full_voice_support_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nr5g_full_voice_support_req_msg is empty
 * static const uint8_t nas_get_nr5g_full_voice_support_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_full_voice_support_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_full_voice_support_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_full_voice_support_resp_msg_v01, nr5g_full_voice_support) - QMI_IDL_OFFSET8(nas_get_nr5g_full_voice_support_resp_msg_v01, nr5g_full_voice_support_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_full_voice_support_resp_msg_v01, nr5g_full_voice_support)
};

/*
 * nas_get_nr5g_disable_status_req_msg is empty
 * static const uint8_t nas_get_nr5g_disable_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_disable_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_sa_disabled) - QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_sa_disabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_sa_disabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_nsa_disabled) - QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_nsa_disabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, is_nsa_disabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, sa_disable_reason) - QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, sa_disable_reason_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, sa_disable_reason),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, nsa_disable_reason) - QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, nsa_disable_reason_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_disable_status_resp_msg_v01, nsa_disable_reason)
};

static const uint8_t nas_nr5g_disable_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, is_sa_disabled),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, is_nsa_disabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, sa_disable_reason) - QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, sa_disable_reason_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, sa_disable_reason),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, nsa_disable_reason) - QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, nsa_disable_reason_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_disable_status_ind_msg_v01, nsa_disable_reason)
};

static const uint8_t nas_cell_barring_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, radio_if) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, radio_if_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, radio_if),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_info) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_info),
  QMI_IDL_TYPE88(0, 200),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_type_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, emerg_barring_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_type_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, signalling_barring_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_type_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, data_barring_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_type_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, voice_barring_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_type_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, video_barring_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_type) - QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_type_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cell_barring_info_ind_msg_v01, sms_barring_type)
};

static const uint8_t nas_access_barring_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, radio_if) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, radio_if_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, radio_if),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, emerg_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, emerg_barring_status_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, emerg_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, signalling_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, signalling_barring_status_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, signalling_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, data_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, data_barring_status_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, data_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, voice_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, voice_barring_status_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, voice_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, video_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, video_barring_status_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, video_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, sms_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, sms_barring_status_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, sms_barring_status),
  QMI_IDL_TYPE88(0, 201),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, csfb_barring_status) - QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, csfb_barring_status_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_access_barring_status_ind_msg_v01, csfb_barring_status),
  QMI_IDL_TYPE88(0, 201)
};

static const uint8_t nas_set_nsa_on_ndds_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_nsa_on_ndds_req_msg_v01, enable_nsa_on_ndds)
};

static const uint8_t nas_set_nsa_on_ndds_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nsa_on_ndds_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nsa_on_ndds_req_msg is empty
 * static const uint8_t nas_get_nsa_on_ndds_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nsa_on_ndds_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nsa_on_ndds_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nsa_on_ndds_resp_msg_v01, is_nsa_enabled_on_ndds) - QMI_IDL_OFFSET8(nas_get_nsa_on_ndds_resp_msg_v01, is_nsa_enabled_on_ndds_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nsa_on_ndds_resp_msg_v01, is_nsa_enabled_on_ndds)
};

static const uint8_t nas_set_nr5g_sync_pulse_gen_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, start_sfn) - QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, start_sfn_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, start_sfn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, report_period) - QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, report_period_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, report_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_align_type) - QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_align_type_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_align_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_trigger_action) - QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_trigger_action_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_trigger_action),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_get_cxo_count) - QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_get_cxo_count_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_req_msg_v01, pulse_get_cxo_count)
};

static const uint8_t nas_set_nr5g_sync_pulse_gen_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_sync_pulse_gen_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nr5g_sync_pulse_gen_req_msg is empty
 * static const uint8_t nas_get_nr5g_sync_pulse_gen_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_sync_pulse_gen_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, last_sfn) - QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, last_sfn_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, last_sfn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, pulse_period) - QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, pulse_period_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, pulse_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, report_period) - QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, report_period_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, report_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta) - QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta_offset) - QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta_offset_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_sync_pulse_gen_resp_msg_v01, nta_offset)
};

static const uint8_t nas_nr5g_time_sync_pulse_report_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, gps_time) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, gps_time_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, gps_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, sfn) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, sfn_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, sfn),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta_offset) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta_offset_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, nta_offset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, leapseconds) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, leapseconds_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, leapseconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, utc_time) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, utc_time_valid)),
  0x15,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, utc_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, is_cxo_count_present) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, is_cxo_count_present_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, is_cxo_count_present),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, get_cxo_count) - QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, get_cxo_count_valid)),
  0x17,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_time_sync_pulse_report_ind_msg_v01, get_cxo_count)
};

static const uint8_t nas_nr5g_lost_frame_sync_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_lost_frame_sync_ind_msg_v01, nr5g_sync_lost_reason) - QMI_IDL_OFFSET8(nas_nr5g_lost_frame_sync_ind_msg_v01, nr5g_sync_lost_reason_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_lost_frame_sync_ind_msg_v01, nr5g_sync_lost_reason)
};

static const uint8_t nas_deprioritize_nr5g_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_deprioritize_nr5g_req_msg_v01, deprioritize_nr5g)
};

static const uint8_t nas_deprioritize_nr5g_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_deprioritize_nr5g_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_black_list_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_black_list_req_msg_v01, black_list) - QMI_IDL_OFFSET8(nas_set_black_list_req_msg_v01, black_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_black_list_req_msg_v01, black_list),
  NAS_MAX_BLACK_LIST_SIZE_V01,
  QMI_IDL_OFFSET8(nas_set_black_list_req_msg_v01, black_list) - QMI_IDL_OFFSET8(nas_set_black_list_req_msg_v01, black_list_len),
  QMI_IDL_TYPE88(0, 202)
};

static const uint8_t nas_set_black_list_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_black_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_nr5g_rrc_conn_reject_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, nr_arfcn) - QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, nr_arfcn_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, nr_arfcn),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, phy_cell_id) - QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, phy_cell_id_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_conn_reject_ind_msg_v01, phy_cell_id)
};

static const uint8_t nas_nr5g_resource_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_resource_config_ind_msg_v01, carrier_cfg) - QMI_IDL_OFFSET8(nas_nr5g_resource_config_ind_msg_v01, carrier_cfg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_resource_config_ind_msg_v01, carrier_cfg),
  NAS_CARRIER_CFG_MAX_V01,
  QMI_IDL_OFFSET8(nas_nr5g_resource_config_ind_msg_v01, carrier_cfg) - QMI_IDL_OFFSET8(nas_nr5g_resource_config_ind_msg_v01, carrier_cfg_len),
  QMI_IDL_TYPE88(0, 204)
};

static const uint8_t nas_get_cqi_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cqi_req_msg_v01, sys_mode) - QMI_IDL_OFFSET8(nas_get_cqi_req_msg_v01, sys_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_cqi_req_msg_v01, sys_mode)
};

static const uint8_t nas_get_cqi_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_cc_cqi) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_cc_cqi_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_cc_cqi),
  NAS_LTE_CQI_MAX_NUM_CC_V01,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_cc_cqi) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_cc_cqi_len),
  QMI_IDL_TYPE88(0, 205),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_cc_cqi) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_cc_cqi_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_cc_cqi),
  NAS_NR5G_CQI_MAX_NUM_CC_V01,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_cc_cqi) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_cc_cqi_len),
  QMI_IDL_TYPE88(0, 206),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_rank) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_rank_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_rank),
  NAS_LTE_CQI_MAX_NUM_CC_V01,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_rank) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, lte_rank_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_rank) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_rank_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_rank),
  NAS_NR5G_CQI_MAX_NUM_CC_V01,
  QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_rank) - QMI_IDL_OFFSET8(nas_get_cqi_resp_msg_v01, nr5g_rank_len)
};

static const uint8_t nas_lte_cphy_ca_aggregated_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_lte_cphy_ca_aggregated_ind_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_aggregated_ind_msg_v01, scell_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_aggregated_ind_msg_v01, scell_info),
  NAS_MAX_SCELL_LIST_LEN_EXT_V01,
  QMI_IDL_OFFSET8(nas_lte_cphy_ca_aggregated_ind_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_lte_cphy_ca_aggregated_ind_msg_v01, scell_info_len),
  QMI_IDL_TYPE88(0, 170),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_lte_cphy_ca_aggregated_ind_msg_v01, pcell_info) - QMI_IDL_OFFSET16RELATIVE(nas_lte_cphy_ca_aggregated_ind_msg_v01, pcell_info_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(nas_lte_cphy_ca_aggregated_ind_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 169),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_lte_cphy_ca_aggregated_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth) - QMI_IDL_OFFSET16RELATIVE(nas_lte_cphy_ca_aggregated_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_lte_cphy_ca_aggregated_ind_msg_v01, cphy_ca_aggregated_dl_bandwidth)
};

static const uint8_t nas_v2x_tx_status_report_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_status) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_status_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_status),
  QMI_IDL_TYPE88(0, 207),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, num_rb) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, num_rb_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, num_rb),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, start_rb) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, start_rb_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, start_rb),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, mcs) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, mcs_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, mcs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_num) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_num_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_type) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_type_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, segments_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, src_port) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, src_port_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, src_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_type) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_type_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, tx_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, v2x_l1_sf_time) - QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, v2x_l1_sf_time_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_v2x_tx_status_report_ind_msg_v01, v2x_l1_sf_time)
};

static const uint8_t nas_set_traffic_profile_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_traffic_profile_req_msg_v01, traffic_profile_info),
  NAS_MAX_TRAFFIC_PROFILES_V01,
  QMI_IDL_OFFSET8(nas_set_traffic_profile_req_msg_v01, traffic_profile_info) - QMI_IDL_OFFSET8(nas_set_traffic_profile_req_msg_v01, traffic_profile_info_len),
  QMI_IDL_TYPE88(0, 208)
};

static const uint8_t nas_set_traffic_profile_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_traffic_profile_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_traffic_profile_resp_msg_v01, prohibit_timer) - QMI_IDL_OFFSET8(nas_set_traffic_profile_resp_msg_v01, prohibit_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_traffic_profile_resp_msg_v01, prohibit_timer)
};

static const uint8_t nas_update_app_performance_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, performance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, ul_performance) - QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, ul_performance_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, ul_performance),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, dl_performance) - QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, dl_performance_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_update_app_performance_req_msg_v01, dl_performance)
};

static const uint8_t nas_update_app_performance_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, overall_app_performance_status) - QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, overall_app_performance_status_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, overall_app_performance_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, ul_app_performance_status) - QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, ul_app_performance_status_valid)),
  0x11,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, ul_app_performance_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, dl_app_performance_status) - QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, dl_app_performance_status_valid)),
  0x12,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_update_app_performance_resp_msg_v01, dl_app_performance_status)
};

static const uint8_t nas_set_dc_mute_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_endc) - QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_endc_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_endc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_nrdc) - QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_nrdc_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, mute_nrdc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_endc) - QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_endc_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_endc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_nrdc) - QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_nrdc_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_dc_mute_config_req_msg_v01, immediate_scg_rel_nrdc)
};

static const uint8_t nas_set_dc_mute_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_dc_mute_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_dc_mute_config_req_msg is empty
 * static const uint8_t nas_get_dc_mute_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_dc_mute_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_endc) - QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_endc_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_endc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_nrdc) - QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_nrdc_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, mute_nrdc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_endc) - QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_endc_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_endc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_nrdc) - QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_nrdc_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_dc_mute_config_resp_msg_v01, immediate_scg_rel_nrdc)
};

/*
 * nas_get_hst_info_req_msg is empty
 * static const uint8_t nas_get_hst_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_hst_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_lte) - QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_lte_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_lte),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_nr5g) - QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_nr5g_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_hst_info_resp_msg_v01, hst_nr5g)
};

static const uint8_t nas_hst_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_lte) - QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_lte_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_lte),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_nr5g) - QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_nr5g_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_hst_info_ind_msg_v01, hst_nr5g)
};

static const uint8_t nas_data_enable_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_data_enable_ind_msg_v01, enable_data) - QMI_IDL_OFFSET8(nas_data_enable_ind_msg_v01, enable_data_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_data_enable_ind_msg_v01, enable_data)
};

/*
 * nas_get_msim_sub_mode_req_msg is empty
 * static const uint8_t nas_get_msim_sub_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_msim_sub_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, sub_mode) - QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, sub_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, sub_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, tx_concurrency) - QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, tx_concurrency_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, tx_concurrency),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, dsda_operation_substate) - QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, dsda_operation_substate_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_msim_sub_mode_resp_msg_v01, dsda_operation_substate)
};

static const uint8_t nas_msim_sub_mode_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, sub_mode) - QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, sub_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, sub_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, tx_concurrency) - QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, tx_concurrency_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, tx_concurrency),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, dsda_operation_substate) - QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, dsda_operation_substate_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_msim_sub_mode_ind_msg_v01, dsda_operation_substate)
};

static const uint8_t nas_nr5g_rrc_bwp_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_info_ind_msg_v01, serving_cell_list) - QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_info_ind_msg_v01, serving_cell_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_info_ind_msg_v01, serving_cell_list),
  NAS_NR5G_CPHY_MAX_SERVING_CELL_V01,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_info_ind_msg_v01, serving_cell_list) - QMI_IDL_OFFSET8(nas_nr5g_rrc_bwp_info_ind_msg_v01, serving_cell_list_len),
  QMI_IDL_TYPE88(0, 211)
};

static const uint8_t nas_set_gaming_mode_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_gaming_mode_status_req_msg_v01, gaming_mode_status) - QMI_IDL_OFFSET8(nas_set_gaming_mode_status_req_msg_v01, gaming_mode_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_gaming_mode_status_req_msg_v01, gaming_mode_status)
};

static const uint8_t nas_set_gaming_mode_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_gaming_mode_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_snpn_list_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_snpn_list_req_msg_v01, snpn_list),
  NAS_SNPN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_set_snpn_list_req_msg_v01, snpn_list) - QMI_IDL_OFFSET8(nas_set_snpn_list_req_msg_v01, snpn_list_len),
  QMI_IDL_TYPE88(0, 212)
};

static const uint8_t nas_set_snpn_list_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_snpn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_snpn_list_req_msg is empty
 * static const uint8_t nas_get_snpn_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_snpn_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, snpn_list) - QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, snpn_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, snpn_list),
  NAS_SNPN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, snpn_list) - QMI_IDL_OFFSET8(nas_get_snpn_list_resp_msg_v01, snpn_list_len),
  QMI_IDL_TYPE88(0, 212)
};

/*
 * nas_get_phy_channel_info_req_msg is empty
 * static const uint8_t nas_get_phy_channel_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_phy_channel_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, rat) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, rat_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_info) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 213),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_info),
  NAS_MAX_SCELL_LIST_LEN_EXT_V01,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_info) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_info_len),
  QMI_IDL_TYPE88(0, 213),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_band) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_band_valid)),
  0x13,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, pcell_band),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_band) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_band_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_band),
  NAS_MAX_SCELL_LIST_LEN_EXT_V01,
  QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_band) - QMI_IDL_OFFSET8(nas_get_phy_channel_info_resp_msg_v01, scell_band_len)
};

static const uint8_t nas_nr5g_rrc_utc_time_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, utc_sib9_acquired) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, utc_sib9_acquired_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, utc_sib9_acquired),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, r16_timing_info) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, r16_timing_info_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, r16_timing_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, dayLightSavingTime) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, dayLightSavingTime_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, dayLightSavingTime),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, leapSeconds) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, leapSeconds_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, leapSeconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, localTimeOffset) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, localTimeOffset_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, localTimeOffset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, ref_time_info) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, ref_time_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, ref_time_info),
  QMI_IDL_TYPE88(0, 214),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, uncertainty) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, uncertainty_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, uncertainty),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, timeInfoType) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, timeInfoType_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, timeInfoType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, referenceSFN) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, referenceSFN_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, referenceSFN),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, abs_time) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, abs_time_valid)),
  0x19,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, universal_time) - QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, universal_time_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_rrc_utc_time_update_ind_msg_v01, universal_time),
  QMI_IDL_TYPE88(0, 163)
};

/*
 * nas_get_nr5g_rrc_utc_time_req_msg is empty
 * static const uint8_t nas_get_nr5g_rrc_utc_time_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_rrc_utc_time_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, utc_sib9_acquired) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, utc_sib9_acquired_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, utc_sib9_acquired),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, r16_timing_info) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, r16_timing_info_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, r16_timing_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, dayLightSavingTime) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, dayLightSavingTime_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, dayLightSavingTime),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, leapSeconds) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, leapSeconds_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, leapSeconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, localTimeOffset) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, localTimeOffset_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, localTimeOffset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, ref_time_info) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, ref_time_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, ref_time_info),
  QMI_IDL_TYPE88(0, 214),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, uncertainty) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, uncertainty_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, uncertainty),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, timeInfoType) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, timeInfoType_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, timeInfoType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, referenceSFN) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, referenceSFN_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, referenceSFN),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, abs_time) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, abs_time_valid)),
  0x19,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, universal_time) - QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, universal_time_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_rrc_utc_time_resp_msg_v01, universal_time),
  QMI_IDL_TYPE88(0, 163)
};

static const uint8_t nas_set_msim_preference_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_msim_preference_req_msg_v01, msim_preference)
};

static const uint8_t nas_set_msim_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_msim_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_disable_ul_mimo_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_disable_ul_mimo_req_msg_v01, disable_ul_mimo)
};

static const uint8_t nas_set_disable_ul_mimo_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_disable_ul_mimo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_disable_ul_mimo_req_msg is empty
 * static const uint8_t nas_get_disable_ul_mimo_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_disable_ul_mimo_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_disable_ul_mimo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_disable_ul_mimo_resp_msg_v01, ul_mimo_disabled) - QMI_IDL_OFFSET8(nas_get_disable_ul_mimo_resp_msg_v01, ul_mimo_disabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_disable_ul_mimo_resp_msg_v01, ul_mimo_disabled)
};

static const uint8_t nas_cpe_install_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, inst_mode_status_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, freq_info) - QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, freq_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, freq_info),
  NAS_MAX_NR5G_PPS_FREQ_INFO_LIST_V01,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, freq_info) - QMI_IDL_OFFSET8(nas_cpe_install_mode_req_msg_v01, freq_info_len),
  QMI_IDL_TYPE88(0, 199)
};

static const uint8_t nas_cpe_install_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cpe_install_mode_resp_msg_v01, inst_mode_status) - QMI_IDL_OFFSET8(nas_cpe_install_mode_resp_msg_v01, inst_mode_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_resp_msg_v01, inst_mode_status)
};

static const uint8_t nas_cpe_install_meas_metrics_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cpe_install_meas_metrics_ind_msg_v01, freq_info) - QMI_IDL_OFFSET8(nas_cpe_install_meas_metrics_ind_msg_v01, freq_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_cpe_install_meas_metrics_ind_msg_v01, freq_info),
  NAS_MAX_NR5G_PPS_FREQ_INFO_LIST_V01,
  QMI_IDL_OFFSET8(nas_cpe_install_meas_metrics_ind_msg_v01, freq_info) - QMI_IDL_OFFSET8(nas_cpe_install_meas_metrics_ind_msg_v01, freq_info_len),
  QMI_IDL_TYPE88(0, 215),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(nas_cpe_install_meas_metrics_ind_msg_v01, cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_cpe_install_meas_metrics_ind_msg_v01, cell_id_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(nas_cpe_install_meas_metrics_ind_msg_v01, cell_id),
  NAS_MAX_NR5G_PPS_FREQ_INFO_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(nas_cpe_install_meas_metrics_ind_msg_v01, cell_id) - QMI_IDL_OFFSET16RELATIVE(nas_cpe_install_meas_metrics_ind_msg_v01, cell_id_len)
};

static const uint8_t nas_cpe_install_mode_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_cpe_install_mode_status_ind_msg_v01, inst_mode_status) - QMI_IDL_OFFSET8(nas_cpe_install_mode_status_ind_msg_v01, inst_mode_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_cpe_install_mode_status_ind_msg_v01, inst_mode_status)
};

/*
 * nas_get_fmd_speed_info_req_msg is empty
 * static const uint8_t nas_get_fmd_speed_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_fmd_speed_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, speed) - QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, speed_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, speed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, abs_time_sec) - QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, abs_time_sec_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, abs_time_sec),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, error_cause) - QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, error_cause_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_get_fmd_speed_info_resp_msg_v01, error_cause)
};

static const uint8_t nas_update_channel_lists_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, channel_list_update_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, authorized_channel_list) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, authorized_channel_list_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, authorized_channel_list),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, unauthorized_channel_list) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, unauthorized_channel_list_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, unauthorized_channel_list),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, band) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, band_valid)),
  0x12,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, band),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, graylist) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, graylist_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, graylist),
  NAS_MAX_NR5G_CHANNEL_LISTS_ENTRY_V01,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, graylist) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, graylist_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, psd) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, psd_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, psd),
  NAS_MAX_NR5G_CHANNEL_LISTS_ENTRY_V01,
  QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, psd) - QMI_IDL_OFFSET8(nas_update_channel_lists_req_msg_v01, psd_len)
};

static const uint8_t nas_update_channel_lists_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_channel_lists_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_nr5g_serving_cell_freq_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_ind_msg_v01, serving_cell_freq_info) - QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_ind_msg_v01, serving_cell_freq_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_serving_cell_freq_info_ind_msg_v01, serving_cell_freq_info),
  QMI_IDL_TYPE88(0, 216)
};

static const uint8_t nas_nr5g_unauthorized_freq_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_ind_msg_v01, nr5g_unauthorized_freq) - QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_ind_msg_v01, nr5g_unauthorized_freq_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_unauthorized_freq_ind_msg_v01, nr5g_unauthorized_freq),
  QMI_IDL_TYPE88(0, 217)
};

static const uint8_t nas_nr5g_tx_duty_cycle_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_ind_msg_v01, tx_duty_cycle_status_info) - QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_ind_msg_v01, tx_duty_cycle_status_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_tx_duty_cycle_status_ind_msg_v01, tx_duty_cycle_status_info),
  QMI_IDL_TYPE88(0, 218)
};

/*
 * nas_get_v2x_slss_rx_req_msg is empty
 * static const uint8_t nas_get_v2x_slss_rx_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_v2x_slss_rx_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_v2x_slss_rx_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_v2x_slss_rx_resp_msg_v01, v2x_l1_slss_rx_info) - QMI_IDL_OFFSET8(nas_get_v2x_slss_rx_resp_msg_v01, v2x_l1_slss_rx_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_v2x_slss_rx_resp_msg_v01, v2x_l1_slss_rx_info),
  QMI_IDL_TYPE88(0, 220)
};

static const uint8_t nas_v2x_slss_rx_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_v2x_slss_rx_info_ind_msg_v01, v2x_l1_slss_rx_info) - QMI_IDL_OFFSET8(nas_v2x_slss_rx_info_ind_msg_v01, v2x_l1_slss_rx_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_v2x_slss_rx_info_ind_msg_v01, v2x_l1_slss_rx_info),
  QMI_IDL_TYPE88(0, 220)
};

static const uint8_t nas_set_nr5g_ca_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_set_nr5g_ca_req_msg_v01, nr5g_ca_capability)
};

static const uint8_t nas_set_nr5g_ca_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_nr5g_ca_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * nas_get_nr5g_ca_req_msg is empty
 * static const uint8_t nas_get_nr5g_ca_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_ca_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_ca_resp_msg_v01, nr5g_ca_capability) - QMI_IDL_OFFSET8(nas_get_nr5g_ca_resp_msg_v01, nr5g_ca_capability_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_resp_msg_v01, nr5g_ca_capability)
};

/*
 * nas_get_cell_config_req_msg is empty
 * static const uint8_t nas_get_cell_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_cell_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, cell_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, cell_list),
  NAS_MAX_CELL_LOCK_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, cell_list) - QMI_IDL_OFFSET8(nas_get_cell_config_resp_msg_v01, cell_list_len),
  QMI_IDL_TYPE88(0, 190)
};

static const uint8_t nas_nr5g_ca_sig_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, any_sig_info_configured) - QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, any_sig_info_configured_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, any_sig_info_configured),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, sig_info) - QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, sig_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, sig_info),
  NAS_MAX_NR5G_CA_SIG_INFO_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, sig_info) - QMI_IDL_OFFSET8(nas_nr5g_ca_sig_info_ind_msg_v01, sig_info_len),
  QMI_IDL_TYPE88(0, 222)
};

/*
 * nas_get_nr5g_ca_sig_info_req_msg is empty
 * static const uint8_t nas_get_nr5g_ca_sig_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_get_nr5g_ca_sig_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, any_sig_info_configured) - QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, any_sig_info_configured_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, any_sig_info_configured),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, sig_info) - QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, sig_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, sig_info),
  NAS_MAX_NR5G_CA_SIG_INFO_ENTRIES_V01,
  QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, sig_info) - QMI_IDL_OFFSET8(nas_get_nr5g_ca_sig_info_resp_msg_v01, sig_info_len),
  QMI_IDL_TYPE88(0, 222)
};

static const uint8_t nas_config_embms_sig_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_threshold_list) - QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_threshold_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_threshold_list),
  NAS_EMBMS_SNR_THRESHOLD_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_threshold_list) - QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_threshold_list_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_delta) - QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_delta_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_config_embms_sig_info_req_msg_v01, snr_delta)
};

static const uint8_t nas_config_embms_sig_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_config_embms_sig_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_update_embms_sig_report_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_update_embms_sig_report_req_msg_v01, enable_reporting) - QMI_IDL_OFFSET8(nas_update_embms_sig_report_req_msg_v01, enable_reporting_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_update_embms_sig_report_req_msg_v01, enable_reporting)
};

static const uint8_t nas_update_embms_sig_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_embms_sig_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_embms_sig_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_embms_sig_info_ind_msg_v01, snr_info) - QMI_IDL_OFFSET8(nas_embms_sig_info_ind_msg_v01, snr_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_embms_sig_info_ind_msg_v01, snr_info),
  NAS_EMBMS_SIG_INFO_AREA_LIST_MAX_V01,
  QMI_IDL_OFFSET8(nas_embms_sig_info_ind_msg_v01, snr_info) - QMI_IDL_OFFSET8(nas_embms_sig_info_ind_msg_v01, snr_info_len),
  QMI_IDL_TYPE88(0, 223)
};

/*
 * nas_stop_embms_sig_info_ind_msg is empty
 * static const uint8_t nas_stop_embms_sig_info_ind_msg_data_v01[] = {
 * };
 */

/*
 * nas_trigger_wwan_reset_req_msg is empty
 * static const uint8_t nas_trigger_wwan_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t nas_trigger_wwan_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_trigger_wwan_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_set_red_cap_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(nas_set_red_cap_config_req_msg_v01, redcap_config_type) - QMI_IDL_OFFSET8(nas_set_red_cap_config_req_msg_v01, redcap_config_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_set_red_cap_config_req_msg_v01, redcap_config_type)
};

static const uint8_t nas_set_red_cap_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_set_red_cap_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_update_sleep_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_update_sleep_status_req_msg_v01, disable_sleep)
};

static const uint8_t nas_update_sleep_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_update_sleep_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_notify_cellular_data_disabled_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(nas_notify_cellular_data_disabled_req_msg_v01, is_data_off)
};

static const uint8_t nas_notify_cellular_data_disabled_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_notify_cellular_data_disabled_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t nas_profile_switch_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(nas_profile_switch_ind_msg_v01, switch_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(nas_profile_switch_ind_msg_v01, plmn_id),
  QMI_IDL_TYPE88(0, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  nas_type_table_v01[] = {
  {sizeof(nas_common_network_id_type_v01), nas_common_network_id_type_data_v01},
  {sizeof(nas_snpn_id_name_type_v01), nas_snpn_id_name_type_data_v01},
  {sizeof(nas_signal_stregth_indicator_type_v01), nas_signal_stregth_indicator_type_data_v01},
  {sizeof(nas_rssi_indicator_type_v01), nas_rssi_indicator_type_data_v01},
  {sizeof(nas_ecio_indicator_type_v01), nas_ecio_indicator_type_data_v01},
  {sizeof(nas_io_indicator_type_v01), nas_io_indicator_type_data_v01},
  {sizeof(nas_sinr_indicator_type_v01), nas_sinr_indicator_type_data_v01},
  {sizeof(nas_rsrq_indicator_type_v01), nas_rsrq_indicator_type_data_v01},
  {sizeof(nas_ecio_indicator_threshold_type_v01), nas_ecio_indicator_threshold_type_data_v01},
  {sizeof(nas_sinr_indicator_threshold_type_v01), nas_sinr_indicator_threshold_type_data_v01},
  {sizeof(nas_rsrp_indicator_type_v01), nas_rsrp_indicator_type_data_v01},
  {sizeof(nas_snr_indicator_type_v01), nas_snr_indicator_type_data_v01},
  {sizeof(nas_supp_ciot_optimizations_type_v01), nas_supp_ciot_optimizations_type_data_v01},
  {sizeof(nas_signal_strength_type_v01), nas_signal_strength_type_data_v01},
  {sizeof(nas_rf_band_info_type_v01), nas_rf_band_info_type_data_v01},
  {sizeof(nas_rf_dedicated_band_info_type_v01), nas_rf_dedicated_band_info_type_data_v01},
  {sizeof(nas_rf_bandwidth_info_type_v01), nas_rf_bandwidth_info_type_data_v01},
  {sizeof(nas_registration_reject_reason_type_v01), nas_registration_reject_reason_type_data_v01},
  {sizeof(nas_rssi_type_v01), nas_rssi_type_data_v01},
  {sizeof(nas_ecio_type_v01), nas_ecio_type_data_v01},
  {sizeof(nas_error_rate_type_v01), nas_error_rate_type_data_v01},
  {sizeof(nas_rsrq_type_v01), nas_rsrq_type_data_v01},
  {sizeof(nas_reg_network_reject_v01), nas_reg_network_reject_data_v01},
  {sizeof(nas_signal_strength_list_type_v01), nas_signal_strength_list_type_data_v01},
  {sizeof(nas_nr5g_band_pref_mask_type_v01), nas_nr5g_band_pref_mask_type_data_v01},
  {sizeof(lte_band_pref_mask_ext_type_v01), lte_band_pref_mask_ext_type_data_v01},
  {sizeof(nas_3gpp_network_info_type_v01), nas_3gpp_network_info_type_data_v01},
  {sizeof(nas_network_radio_access_technology_type_v01), nas_network_radio_access_technology_type_data_v01},
  {sizeof(nas_scanned_ciot_lte_op_mode_type_v01), nas_scanned_ciot_lte_op_mode_type_data_v01},
  {sizeof(nas_mnc_pcs_digit_include_status_type_v01), nas_mnc_pcs_digit_include_status_type_data_v01},
  {sizeof(nas_csg_info_type_v01), nas_csg_info_type_data_v01},
  {sizeof(nas_cag_id_name_type_v01), nas_cag_id_name_type_data_v01},
  {sizeof(nas_fsk_hop_table_entry_type_v01), nas_fsk_hop_table_entry_type_data_v01},
  {sizeof(nas_csg_nw_info_type_v01), nas_csg_nw_info_type_data_v01},
  {sizeof(nas_cag_nw_info_type_v01), nas_cag_nw_info_type_data_v01},
  {sizeof(nas_csg_nw_signal_strength_info_type_v01), nas_csg_nw_signal_strength_info_type_data_v01},
  {sizeof(nas_pci_scan_cell_info_type_v01), nas_pci_scan_cell_info_type_data_v01},
  {sizeof(nas_pci_scan_signal_info_type_v01), nas_pci_scan_signal_info_type_data_v01},
  {sizeof(nas_pci_nw_info_type_v01), nas_pci_nw_info_type_data_v01},
  {sizeof(nas_geran_cell_search_info_type_v01), nas_geran_cell_search_info_type_data_v01},
  {sizeof(nas_lte_cell_search_info_type_v01), nas_lte_cell_search_info_type_data_v01},
  {sizeof(nas_manual_network_register_info_type_v01), nas_manual_network_register_info_type_data_v01},
  {sizeof(nas_serving_system_type_v01), nas_serving_system_type_data_v01},
  {sizeof(nas_plmn_type_v01), nas_plmn_type_data_v01},
  {sizeof(nas_cdma_system_id_type_v01), nas_cdma_system_id_type_data_v01},
  {sizeof(nas_cdma_base_station_info_type_v01), nas_cdma_base_station_info_type_data_v01},
  {sizeof(nas_roaming_indicator_type_v01), nas_roaming_indicator_type_data_v01},
  {sizeof(nas_3gpp_time_zone_type_v01), nas_3gpp_time_zone_type_data_v01},
  {sizeof(nas_ss_detailed_service_info_type_v01), nas_ss_detailed_service_info_type_data_v01},
  {sizeof(nas_cdma_system_id_ext_type_v01), nas_cdma_system_id_ext_type_data_v01},
  {sizeof(nas_gw_sys_info3_type_v01), nas_gw_sys_info3_type_data_v01},
  {sizeof(nas_universal_time_and_local_time_zone_3gpp_type_v01), nas_universal_time_and_local_time_zone_3gpp_type_data_v01},
  {sizeof(nas_3gpp2_home_network_ext_type_v01), nas_3gpp2_home_network_ext_type_data_v01},
  {sizeof(nas_3gpp_mcs_digit_type_v01), nas_3gpp_mcs_digit_type_data_v01},
  {sizeof(nas_3gpp_preferred_networks_type_v01), nas_3gpp_preferred_networks_type_data_v01},
  {sizeof(nas_3gpp_forbidden_networks_type_v01), nas_3gpp_forbidden_networks_type_data_v01},
  {sizeof(nas_accolc_set_type_v01), nas_accolc_set_type_data_v01},
  {sizeof(nas_registration_parameters_type_v01), nas_registration_parameters_type_data_v01},
  {sizeof(nas_hdr_scp_config_type_v01), nas_hdr_scp_config_type_data_v01},
  {sizeof(nas_rf_band_info_ext_type_v01), nas_rf_band_info_ext_type_data_v01},
  {sizeof(nas_net_sel_pref_type_v01), nas_net_sel_pref_type_data_v01},
  {sizeof(nas_csg_nw_iden_type_v01), nas_csg_nw_iden_type_data_v01},
  {sizeof(nas_ddtm_preference_type_v01), nas_ddtm_preference_type_data_v01},
  {sizeof(nas_ddtm_settings_type_v01), nas_ddtm_settings_type_data_v01},
  {sizeof(nas_service_provider_name_type_v01), nas_service_provider_name_type_data_v01},
  {sizeof(nas_operator_plmn_list_type_v01), nas_operator_plmn_list_type_data_v01},
  {sizeof(nas_plmn_network_name_type_v01), nas_plmn_network_name_type_data_v01},
  {sizeof(nas_nr5g_tac_info_type_v01), nas_nr5g_tac_info_type_data_v01},
  {sizeof(nas_3gpp2_home_sid_nid_info_type_v01), nas_3gpp2_home_sid_nid_info_type_data_v01},
  {sizeof(nas_3gpp2_min_based_info_type_v01), nas_3gpp2_min_based_info_type_data_v01},
  {sizeof(nas_3gpp2_true_imsi_info_type_v01), nas_3gpp2_true_imsi_info_type_data_v01},
  {sizeof(nas_cdma_channel_info_type_v01), nas_cdma_channel_info_type_data_v01},
  {sizeof(nas_nmr_cell_info_type_v01), nas_nmr_cell_info_type_data_v01},
  {sizeof(nas_geran_cell_info_type_v01), nas_geran_cell_info_type_data_v01},
  {sizeof(nas_umts_monitored_cell_set_info_type_v01), nas_umts_monitored_cell_set_info_type_data_v01},
  {sizeof(nas_umts_geran_nbr_cell_set_info_type_v01), nas_umts_geran_nbr_cell_set_info_type_data_v01},
  {sizeof(nas_umts_lte_nbr_cell_set_info_type_v01), nas_umts_lte_nbr_cell_set_info_type_data_v01},
  {sizeof(nas_umts_cell_info_type_v01), nas_umts_cell_info_type_data_v01},
  {sizeof(nas_cdma_cell_info_type_v01), nas_cdma_cell_info_type_data_v01},
  {sizeof(nas_lte_ngbr_cell_type_v01), nas_lte_ngbr_cell_type_data_v01},
  {sizeof(nas_lte_intra_freq_type_v01), nas_lte_intra_freq_type_data_v01},
  {sizeof(nas_lte_inter_freq_freqs_type_v01), nas_lte_inter_freq_freqs_type_data_v01},
  {sizeof(nas_lte_inter_freq_freqs_ext_type_v01), nas_lte_inter_freq_freqs_ext_type_data_v01},
  {sizeof(nas_lte_inter_freq_type_v01), nas_lte_inter_freq_type_data_v01},
  {sizeof(nas_lte_inter_freq_ext_type_v01), nas_lte_inter_freq_ext_type_data_v01},
  {sizeof(nas_lte_ngbr_gsm_cell_type_v01), nas_lte_ngbr_gsm_cell_type_data_v01},
  {sizeof(nas_lte_ngbr_gsm_freq_type_v01), nas_lte_ngbr_gsm_freq_type_data_v01},
  {sizeof(nas_lte_ngbr_gsm_type_v01), nas_lte_ngbr_gsm_type_data_v01},
  {sizeof(nas_lte_ngbr_wcdma_cell_type_v01), nas_lte_ngbr_wcdma_cell_type_data_v01},
  {sizeof(nas_lte_ngbr_wcdma_freq_type_v01), nas_lte_ngbr_wcdma_freq_type_data_v01},
  {sizeof(nas_lte_ngbr_wcdma_type_v01), nas_lte_ngbr_wcdma_type_data_v01},
  {sizeof(nas_wcdma_ngbr_lte_type_v01), nas_wcdma_ngbr_lte_type_data_v01},
  {sizeof(nas_rx_power_type_v01), nas_rx_power_type_data_v01},
  {sizeof(nas_gsm_cell_info_ext_type_v01), nas_gsm_cell_info_ext_type_data_v01},
  {sizeof(nas_wcdma_cell_info_ext_type_v01), nas_wcdma_cell_info_ext_type_data_v01},
  {sizeof(nas_wcdma_active_set_info_type_v01), nas_wcdma_active_set_info_type_data_v01},
  {sizeof(nas_wcdma_active_set_reference_rl_type_v01), nas_wcdma_active_set_reference_rl_type_data_v01},
  {sizeof(nas_umts_monitored_cell_set_ext_info_type_v01), nas_umts_monitored_cell_set_ext_info_type_data_v01},
  {sizeof(nas_umts_geran_nbr_cell_set_ext_info_type_v01), nas_umts_geran_nbr_cell_set_ext_info_type_data_v01},
  {sizeof(nas_umts_cell_ext_info_type_v01), nas_umts_cell_ext_info_type_data_v01},
  {sizeof(nas_wcdma_active_set_ext_info_type_v01), nas_wcdma_active_set_ext_info_type_data_v01},
  {sizeof(nas_nmr_cell_info_ext_type_v01), nas_nmr_cell_info_ext_type_data_v01},
  {sizeof(nas_geran_cell_info_ext_type_v01), nas_geran_cell_info_ext_type_data_v01},
  {sizeof(nas_geran_eng_mode_scell_config_type_v01), nas_geran_eng_mode_scell_config_type_data_v01},
  {sizeof(nas_nr5g_serving_cell_info_type_v01), nas_nr5g_serving_cell_info_type_data_v01},
  {sizeof(nas_3gpp_eons_plmn_name_type_v01), nas_3gpp_eons_plmn_name_type_data_v01},
  {sizeof(nas_display_bit_type_v01), nas_display_bit_type_data_v01},
  {sizeof(nas_lang_plmn_names_type_v01), nas_lang_plmn_names_type_data_v01},
  {sizeof(nas_standby_pref_type_v01), nas_standby_pref_type_data_v01},
  {sizeof(nas_technology_pref_type_v01), nas_technology_pref_type_data_v01},
  {sizeof(nas_active_technology_type_v01), nas_active_technology_type_data_v01},
  {sizeof(nas_julian_time_type_v01), nas_julian_time_type_data_v01},
  {sizeof(nas_reg_reject_info_type_v01), nas_reg_reject_info_type_data_v01},
  {sizeof(nas_3gpp2_srv_status_info_type_v01), nas_3gpp2_srv_status_info_type_data_v01},
  {sizeof(nas_3gpp_srv_status_info_type_v01), nas_3gpp_srv_status_info_type_data_v01},
  {sizeof(nas_rat_plmn_info_type_v01), nas_rat_plmn_info_type_data_v01},
  {sizeof(nas_common_sys_info_type_v01), nas_common_sys_info_type_data_v01},
  {sizeof(nas_cdma_only_sys_info_type_v01), nas_cdma_only_sys_info_type_data_v01},
  {sizeof(nas_hdr_only_sys_info_type_v01), nas_hdr_only_sys_info_type_data_v01},
  {sizeof(nas_gsm_only_sys_info_type_v01), nas_gsm_only_sys_info_type_data_v01},
  {sizeof(nas_wcdma_only_sys_info_type_v01), nas_wcdma_only_sys_info_type_data_v01},
  {sizeof(nas_lte_only_sys_info_type_v01), nas_lte_only_sys_info_type_data_v01},
  {sizeof(nas_nr5g_only_sys_info_type_v01), nas_nr5g_only_sys_info_type_data_v01},
  {sizeof(nas_cdma_hdr_only_sys_info_type_v01), nas_cdma_hdr_only_sys_info_type_data_v01},
  {sizeof(nas_3gpp_only_sys_info_type_v01), nas_3gpp_only_sys_info_type_data_v01},
  {sizeof(nas_cdma_sys_info_type_v01), nas_cdma_sys_info_type_data_v01},
  {sizeof(nas_hdr_sys_info_type_v01), nas_hdr_sys_info_type_data_v01},
  {sizeof(nas_gsm_sys_info_type_v01), nas_gsm_sys_info_type_data_v01},
  {sizeof(nas_wcdma_sys_info_type_v01), nas_wcdma_sys_info_type_data_v01},
  {sizeof(nas_lte_sys_info_type_v01), nas_lte_sys_info_type_data_v01},
  {sizeof(nas_nr5g_sys_info_type_v01), nas_nr5g_sys_info_type_data_v01},
  {sizeof(nas_tdscdma_only_sys_info_type_v01), nas_tdscdma_only_sys_info_type_data_v01},
  {sizeof(nas_tdscdma_sys_info_type_v01), nas_tdscdma_sys_info_type_data_v01},
  {sizeof(nas_cdma_sys_info2_type_v01), nas_cdma_sys_info2_type_data_v01},
  {sizeof(nas_hdr_sys_info2_type_v01), nas_hdr_sys_info2_type_data_v01},
  {sizeof(nas_gsm_sys_info2_type_v01), nas_gsm_sys_info2_type_data_v01},
  {sizeof(nas_wcdma_sys_info2_type_v01), nas_wcdma_sys_info2_type_data_v01},
  {sizeof(nas_lte_sys_info2_type_v01), nas_lte_sys_info2_type_data_v01},
  {sizeof(nas_nr5g_emerg_srv_ind_type_v01), nas_nr5g_emerg_srv_ind_type_data_v01},
  {sizeof(nas_common_sig_info_param_type_v01), nas_common_sig_info_param_type_data_v01},
  {sizeof(nas_hdr_sig_info_type_v01), nas_hdr_sig_info_type_data_v01},
  {sizeof(nas_lte_sig_info_type_v01), nas_lte_sig_info_type_data_v01},
  {sizeof(nas_tdscdma_sig_info_type_v01), nas_tdscdma_sig_info_type_data_v01},
  {sizeof(nas_nr5g_sig_info_type_v01), nas_nr5g_sig_info_type_data_v01},
  {sizeof(nas_nrdc_sig_info_type_v01), nas_nrdc_sig_info_type_data_v01},
  {sizeof(nas_lte_sig_rpt_config_type_v01), nas_lte_sig_rpt_config_type_data_v01},
  {sizeof(nas_set_rx_diversity_req_param_type_v01), nas_set_rx_diversity_req_param_type_data_v01},
  {sizeof(nas_rx_chain_info_type_v01), nas_rx_chain_info_type_data_v01},
  {sizeof(nas_tx_info_type_v01), nas_tx_info_type_data_v01},
  {sizeof(nas_akey_with_spc_type_v01), nas_akey_with_spc_type_data_v01},
  {sizeof(nas_spn_type_v01), nas_spn_type_data_v01},
  {sizeof(nas_plmn_name_type_v01), nas_plmn_name_type_data_v01},
  {sizeof(nas_cdma_bs_info_type_v01), nas_cdma_bs_info_type_data_v01},
  {sizeof(nas_cdma_position_info_type_v01), nas_cdma_position_info_type_data_v01},
  {sizeof(nas_nr5g_sig_rpt_config_type_v01), nas_nr5g_sig_rpt_config_type_data_v01},
  {sizeof(nas_hysteresis_config_type_v01), nas_hysteresis_config_type_data_v01},
  {sizeof(nas_tds_cell_info_type_v01), nas_tds_cell_info_type_data_v01},
  {sizeof(nas_tds_nbr_cell_info_type_v01), nas_tds_nbr_cell_info_type_data_v01},
  {sizeof(nas_lte_cphy_mbsfn_area_signal_strength_type_v01), nas_lte_cphy_mbsfn_area_signal_strength_type_data_v01},
  {sizeof(nas_ims_registration_state_type_v01), nas_ims_registration_state_type_data_v01},
  {sizeof(nas_ims_pref_type_v01), nas_ims_pref_type_data_v01},
  {sizeof(nas_cdma_avoid_sys_info_type_v01), nas_cdma_avoid_sys_info_type_data_v01},
  {sizeof(nas_network_time_type_v01), nas_network_time_type_data_v01},
  {sizeof(nas_lte_sib16_julian_time_type_v01), nas_lte_sib16_julian_time_type_data_v01},
  {sizeof(nas_embms_tmgi_type_v01), nas_embms_tmgi_type_data_v01},
  {sizeof(nas_lte_embms_signal_type_v01), nas_lte_embms_signal_type_data_v01},
  {sizeof(nas_lte_cphy_ca_indicator_type_v01), nas_lte_cphy_ca_indicator_type_data_v01},
  {sizeof(nas_lte_cphy_scell_info_type_v01), nas_lte_cphy_scell_info_type_data_v01},
  {sizeof(nas_cphy_scell_info_v01), nas_cphy_scell_info_data_v01},
  {sizeof(nas_lte_cphy_pcell_info_type_v01), nas_lte_cphy_pcell_info_type_data_v01},
  {sizeof(nas_lte_cphy_scell_info_ext_type_v01), nas_lte_cphy_scell_info_ext_type_data_v01},
  {sizeof(nas_oplmn_entry_type_v01), nas_oplmn_entry_type_data_v01},
  {sizeof(nas_oplmn_list_type_v01), nas_oplmn_list_type_data_v01},
  {sizeof(nas_3gpp_network_scan_result_type_v01), nas_3gpp_network_scan_result_type_data_v01},
  {sizeof(nas_nr_pci_nw_info_type_v01), nas_nr_pci_nw_info_type_data_v01},
  {sizeof(nas_snpn_id_info_type_v01), nas_snpn_id_info_type_data_v01},
  {sizeof(nas_snpn_info_type_v01), nas_snpn_info_type_data_v01},
  {sizeof(nas_ssac_info_type_v01), nas_ssac_info_type_data_v01},
  {sizeof(nas_pmch_datamcs_info_type_v01), nas_pmch_datamcs_info_type_data_v01},
  {sizeof(nas_lte_mbsfn_area_signal_strength_info_type_v01), nas_lte_mbsfn_area_signal_strength_info_type_data_v01},
  {sizeof(nas_lte_pmch_bler_info_type_v01), nas_lte_pmch_bler_info_type_data_v01},
  {sizeof(nas_lte_ml1_pmch_bler_per_area_info_type_v01), nas_lte_ml1_pmch_bler_per_area_info_type_data_v01},
  {sizeof(nas_mtch_info_s_v01), nas_mtch_info_s_data_v01},
  {sizeof(nas_ssac_voice_info_type_v01), nas_ssac_voice_info_type_data_v01},
  {sizeof(nas_sib2_ssac_voice_info_type_v01), nas_sib2_ssac_voice_info_type_data_v01},
  {sizeof(nas_ssac_video_info_type_v01), nas_ssac_video_info_type_data_v01},
  {sizeof(nas_sib2_ssac_video_info_type_v01), nas_sib2_ssac_video_info_type_data_v01},
  {sizeof(nas_lte_rrc_ac_barring_params_type_v01), nas_lte_rrc_ac_barring_params_type_data_v01},
  {sizeof(nas_target_frequency_info_type_v01), nas_target_frequency_info_type_data_v01},
  {sizeof(nas_cell_lock_entry_type_v01), nas_cell_lock_entry_type_data_v01},
  {sizeof(nas_cell_lock_info_type_v01), nas_cell_lock_info_type_data_v01},
  {sizeof(nas_nr5g_cell_identity_info_type_v01), nas_nr5g_cell_identity_info_type_data_v01},
  {sizeof(nas_nr5g_frequency_type_v01), nas_nr5g_frequency_type_data_v01},
  {sizeof(geran_grr_gcell_identifier_s_v01), geran_grr_gcell_identifier_s_data_v01},
  {sizeof(nas_lte_rach_fail_ind_type_v01), nas_lte_rach_fail_ind_type_data_v01},
  {sizeof(nas_v2x_multi_pool_status_type_v01), nas_v2x_multi_pool_status_type_data_v01},
  {sizeof(nas_lte_b2_thresh_adj_info_type_v01), nas_lte_b2_thresh_adj_info_type_data_v01},
  {sizeof(nas_arfcn_type_v01), nas_arfcn_type_data_v01},
  {sizeof(nas_lte_earfcn_type_v01), nas_lte_earfcn_type_data_v01},
  {sizeof(nas_nr5g_pps_freq_info_type_v01), nas_nr5g_pps_freq_info_type_data_v01},
  {sizeof(nas_cell_barring_params_type_v01), nas_cell_barring_params_type_data_v01},
  {sizeof(nas_access_barring_status_params_type_v01), nas_access_barring_status_params_type_data_v01},
  {sizeof(nas_black_list_type_v01), nas_black_list_type_data_v01},
  {sizeof(nas_slot_cfg_type_v01), nas_slot_cfg_type_data_v01},
  {sizeof(nas_carrier_cfg_type_v01), nas_carrier_cfg_type_data_v01},
  {sizeof(nas_lte_cc_cqi_info_type_v01), nas_lte_cc_cqi_info_type_data_v01},
  {sizeof(nas_nr5g_cc_cqi_info_type_v01), nas_nr5g_cc_cqi_info_type_data_v01},
  {sizeof(nas_rflte_tx_status_v01), nas_rflte_tx_status_data_v01},
  {sizeof(nas_traffic_profile_info_type_v01), nas_traffic_profile_info_type_data_v01},
  {sizeof(nas_nr5g_rrc_bwp_config_s_v01), nas_nr5g_rrc_bwp_config_s_data_v01},
  {sizeof(nas_nr5g_rrc_scs_spec_carr_s_v01), nas_nr5g_rrc_scs_spec_carr_s_data_v01},
  {sizeof(nas_nr5g_rrc_serv_cell_config_s_v01), nas_nr5g_rrc_serv_cell_config_s_data_v01},
  {sizeof(nas_snpn_id_imsi_type_v01), nas_snpn_id_imsi_type_data_v01},
  {sizeof(nas_phy_channel_info_type_v01), nas_phy_channel_info_type_data_v01},
  {sizeof(nas_nr5g_rrc_ref_time_info_s_v01), nas_nr5g_rrc_ref_time_info_s_data_v01},
  {sizeof(nas_nr5g_freq_info_meas_metrics_type_v01), nas_nr5g_freq_info_meas_metrics_type_data_v01},
  {sizeof(nas_nr5g_serving_cell_freq_info_type_v01), nas_nr5g_serving_cell_freq_info_type_data_v01},
  {sizeof(nas_nr5g_unauthorized_freq_type_v01), nas_nr5g_unauthorized_freq_type_data_v01},
  {sizeof(nas_nr5g_tx_duty_cycle_status_type_v01), nas_nr5g_tx_duty_cycle_status_type_data_v01},
  {sizeof(nas_v2x_sync_ref_ue_info_s_type_v01), nas_v2x_sync_ref_ue_info_s_type_data_v01},
  {sizeof(nas_v2x_l1_slss_rx_info_s_type_v01), nas_v2x_l1_slss_rx_info_s_type_data_v01},
  {sizeof(nas_nr5g_multi_pci_info_type_v01), nas_nr5g_multi_pci_info_type_data_v01},
  {sizeof(nas_nr5g_ca_sig_info_s_type_v01), nas_nr5g_ca_sig_info_s_type_data_v01},
  {sizeof(nas_embms_area_snr_type_v01), nas_embms_area_snr_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry nas_message_table_v01[] = {
  {sizeof(nas_reset_req_msg_v01), 0},
  {sizeof(nas_reset_resp_msg_v01), nas_reset_resp_msg_data_v01},
  {sizeof(nas_abort_req_msg_v01), nas_abort_req_msg_data_v01},
  {sizeof(nas_abort_resp_msg_v01), nas_abort_resp_msg_data_v01},
  {sizeof(nas_set_event_report_req_msg_v01), nas_set_event_report_req_msg_data_v01},
  {sizeof(nas_set_event_report_resp_msg_v01), nas_set_event_report_resp_msg_data_v01},
  {sizeof(nas_event_report_ind_msg_v01), nas_event_report_ind_msg_data_v01},
  {sizeof(nas_indication_register_req_msg_v01), nas_indication_register_req_msg_data_v01},
  {sizeof(nas_indication_register_resp_msg_v01), nas_indication_register_resp_msg_data_v01},
  {sizeof(nas_get_signal_strength_req_msg_v01), nas_get_signal_strength_req_msg_data_v01},
  {sizeof(nas_get_signal_strength_resp_msg_v01), nas_get_signal_strength_resp_msg_data_v01},
  {sizeof(nas_perform_network_scan_req_msg_v01), nas_perform_network_scan_req_msg_data_v01},
  {sizeof(nas_perform_network_scan_resp_msg_v01), nas_perform_network_scan_resp_msg_data_v01},
  {sizeof(nas_initiate_network_register_req_msg_v01), nas_initiate_network_register_req_msg_data_v01},
  {sizeof(nas_initiate_network_register_resp_msg_v01), nas_initiate_network_register_resp_msg_data_v01},
  {sizeof(nas_initiate_attach_req_msg_v01), nas_initiate_attach_req_msg_data_v01},
  {sizeof(nas_initiate_attach_resp_msg_v01), nas_initiate_attach_resp_msg_data_v01},
  {sizeof(nas_get_serving_system_req_msg_v01), 0},
  {sizeof(nas_get_serving_system_resp_msg_v01), nas_get_serving_system_resp_msg_data_v01},
  {sizeof(nas_serving_system_ind_msg_v01), nas_serving_system_ind_msg_data_v01},
  {sizeof(nas_get_home_network_req_msg_v01), 0},
  {sizeof(nas_get_home_network_resp_msg_v01), nas_get_home_network_resp_msg_data_v01},
  {sizeof(nas_get_preferred_networks_req_msg_v01), 0},
  {sizeof(nas_get_preferred_networks_resp_msg_v01), nas_get_preferred_networks_resp_msg_data_v01},
  {sizeof(nas_set_preferred_networks_req_msg_v01), nas_set_preferred_networks_req_msg_data_v01},
  {sizeof(nas_set_preferred_networks_resp_msg_v01), nas_set_preferred_networks_resp_msg_data_v01},
  {sizeof(nas_set_sim_overdue_status_req_msg_v01), nas_set_sim_overdue_status_req_msg_data_v01},
  {sizeof(nas_set_sim_overdue_status_resp_msg_v01), nas_set_sim_overdue_status_resp_msg_data_v01},
  {sizeof(nas_set_codebook_config_req_msg_v01), nas_set_codebook_config_req_msg_data_v01},
  {sizeof(nas_set_codebook_config_rsp_msg_v01), nas_set_codebook_config_rsp_msg_data_v01},
  {sizeof(nas_get_codebook_config_req_msg_v01), 0},
  {sizeof(nas_get_codebook_config_rsp_msg_v01), nas_get_codebook_config_rsp_msg_data_v01},
  {sizeof(nas_set_nr5g_band_priority_req_msg_v01), nas_set_nr5g_band_priority_req_msg_data_v01},
  {sizeof(nas_set_nr5g_band_priority_resp_msg_v01), nas_set_nr5g_band_priority_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_band_priority_list_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_band_priority_list_resp_msg_v01), nas_get_nr5g_band_priority_list_resp_msg_data_v01},
  {sizeof(nas_get_forbidden_networks_req_msg_v01), 0},
  {sizeof(nas_get_forbidden_networks_resp_msg_v01), nas_get_forbidden_networks_resp_msg_data_v01},
  {sizeof(nas_set_forbidden_networks_req_msg_v01), nas_set_forbidden_networks_req_msg_data_v01},
  {sizeof(nas_set_forbidden_networks_resp_msg_v01), nas_set_forbidden_networks_resp_msg_data_v01},
  {sizeof(nas_get_accolc_req_msg_v01), 0},
  {sizeof(nas_get_accolc_resp_msg_v01), nas_get_accolc_resp_msg_data_v01},
  {sizeof(nas_set_accolc_req_msg_v01), nas_set_accolc_req_msg_data_v01},
  {sizeof(nas_set_accolc_resp_msg_v01), nas_set_accolc_resp_msg_data_v01},
  {sizeof(nas_get_device_config_req_msg_v01), 0},
  {sizeof(nas_get_device_config_resp_msg_v01), nas_get_device_config_resp_msg_data_v01},
  {sizeof(nas_set_device_config_req_msg_v01), nas_set_device_config_req_msg_data_v01},
  {sizeof(nas_set_device_config_resp_msg_v01), nas_set_device_config_resp_msg_data_v01},
  {sizeof(nas_get_rf_band_info_req_msg_v01), 0},
  {sizeof(nas_get_rf_band_info_resp_msg_v01), nas_get_rf_band_info_resp_msg_data_v01},
  {sizeof(nas_get_an_aaa_status_req_msg_v01), 0},
  {sizeof(nas_get_an_aaa_status_resp_msg_v01), nas_get_an_aaa_status_resp_msg_data_v01},
  {sizeof(nas_set_system_selection_preference_req_msg_v01), nas_set_system_selection_preference_req_msg_data_v01},
  {sizeof(nas_set_system_selection_preference_resp_msg_v01), nas_set_system_selection_preference_resp_msg_data_v01},
  {sizeof(nas_get_system_selection_preference_req_msg_v01), 0},
  {sizeof(nas_get_system_selection_preference_resp_msg_v01), nas_get_system_selection_preference_resp_msg_data_v01},
  {sizeof(nas_system_selection_preference_ind_msg_v01), nas_system_selection_preference_ind_msg_data_v01},
  {sizeof(nas_set_ddtm_preference_req_msg_v01), nas_set_ddtm_preference_req_msg_data_v01},
  {sizeof(nas_set_ddtm_preference_resp_msg_v01), nas_set_ddtm_preference_resp_msg_data_v01},
  {sizeof(nas_ddtm_ind_msg_v01), nas_ddtm_ind_msg_data_v01},
  {sizeof(nas_e911_scan_fail_ind_msg_v01), 0},
  {sizeof(nas_nr5g_freq_out_of_band_range_ind_msg_v01), nas_nr5g_freq_out_of_band_range_ind_msg_data_v01},
  {sizeof(nas_get_operator_name_data_req_msg_v01), 0},
  {sizeof(nas_get_operator_name_data_resp_msg_v01), nas_get_operator_name_data_resp_msg_data_v01},
  {sizeof(nas_operator_name_data_ind_msg_v01), nas_operator_name_data_ind_msg_data_v01},
  {sizeof(nas_get_csp_plmn_mode_bit_req_msg_v01), 0},
  {sizeof(nas_get_csp_plmn_mode_bit_resp_msg_v01), nas_get_csp_plmn_mode_bit_resp_msg_data_v01},
  {sizeof(nas_csp_plmn_mode_bit_ind_msg_v01), nas_csp_plmn_mode_bit_ind_msg_data_v01},
  {sizeof(nas_update_akey_req_msg_v01), nas_update_akey_req_msg_data_v01},
  {sizeof(nas_update_akey_resp_msg_v01), nas_update_akey_resp_msg_data_v01},
  {sizeof(nas_get_3gpp2_subscription_info_req_msg_v01), nas_get_3gpp2_subscription_info_req_msg_data_v01},
  {sizeof(nas_get_3gpp2_subscription_info_resp_msg_v01), nas_get_3gpp2_subscription_info_resp_msg_data_v01},
  {sizeof(nas_set_3gpp2_subscription_info_req_msg_v01), nas_set_3gpp2_subscription_info_req_msg_data_v01},
  {sizeof(nas_set_3gpp2_subscription_info_resp_msg_v01), nas_set_3gpp2_subscription_info_resp_msg_data_v01},
  {sizeof(nas_get_mob_cai_rev_req_v01), 0},
  {sizeof(nas_get_mob_cai_rev_resp_v01), nas_get_mob_cai_rev_resp_data_v01},
  {sizeof(nas_get_rtre_config_req_v01), 0},
  {sizeof(nas_get_rtre_config_resp_v01), nas_get_rtre_config_resp_data_v01},
  {sizeof(nas_set_rtre_config_req_v01), nas_set_rtre_config_req_data_v01},
  {sizeof(nas_set_rtre_config_resp_v01), nas_set_rtre_config_resp_data_v01},
  {sizeof(nas_get_cell_location_info_req_msg_v01), 0},
  {sizeof(nas_get_cell_location_info_resp_msg_v01), nas_get_cell_location_info_resp_msg_data_v01},
  {sizeof(nas_get_plmn_name_req_msg_v01), nas_get_plmn_name_req_msg_data_v01},
  {sizeof(nas_get_plmn_name_resp_msg_v01), nas_get_plmn_name_resp_msg_data_v01},
  {sizeof(nas_bind_subscription_req_msg_v01), nas_bind_subscription_req_msg_data_v01},
  {sizeof(nas_bind_subscription_resp_msg_v01), nas_bind_subscription_resp_msg_data_v01},
  {sizeof(nas_set_dual_standby_pref_req_msg_v01), nas_set_dual_standby_pref_req_msg_data_v01},
  {sizeof(nas_set_dual_standby_pref_resp_msg_v01), nas_set_dual_standby_pref_resp_msg_data_v01},
  {sizeof(nas_dual_standby_pref_ind_msg_v01), nas_dual_standby_pref_ind_msg_data_v01},
  {sizeof(nas_set_high_capability_sub_req_msg_v01), nas_set_high_capability_sub_req_msg_data_v01},
  {sizeof(nas_set_high_capability_sub_resp_msg_v01), nas_set_high_capability_sub_resp_msg_data_v01},
  {sizeof(nas_get_high_capability_sub_req_msg_v01), 0},
  {sizeof(nas_get_high_capability_sub_resp_msg_v01), nas_get_high_capability_sub_resp_msg_data_v01},
  {sizeof(nas_high_capability_sub_ind_msg_v01), nas_high_capability_sub_ind_msg_data_v01},
  {sizeof(nas_subscription_info_ind_msg_v01), nas_subscription_info_ind_msg_data_v01},
  {sizeof(nas_get_mode_pref_req_msg_v01), 0},
  {sizeof(nas_get_mode_pref_resp_msg_v01), nas_get_mode_pref_resp_msg_data_v01},
  {sizeof(nas_set_technology_preference_req_msg_v01), nas_set_technology_preference_req_msg_data_v01},
  {sizeof(nas_set_technology_preference_resp_msg_v01), nas_set_technology_preference_resp_msg_data_v01},
  {sizeof(nas_get_technology_preference_req_type_v01), 0},
  {sizeof(nas_get_technology_preference_resp_type_v01), nas_get_technology_preference_resp_type_data_v01},
  {sizeof(nas_get_network_system_preference_req_v01), 0},
  {sizeof(nas_get_network_system_preference_resp_v01), nas_get_network_system_preference_resp_data_v01},
  {sizeof(nas_network_time_ind_msg_v01), nas_network_time_ind_msg_data_v01},
  {sizeof(nas_get_sys_info_req_msg_v01), 0},
  {sizeof(nas_get_sys_info_resp_msg_v01), nas_get_sys_info_resp_msg_data_v01},
  {sizeof(nas_sys_info_ind_msg_v01), nas_sys_info_ind_msg_data_v01},
  {sizeof(nas_get_sig_info_req_msg_v01), 0},
  {sizeof(nas_get_sig_info_resp_msg_v01), nas_get_sig_info_resp_msg_data_v01},
  {sizeof(nas_config_sig_info_req_msg_v01), nas_config_sig_info_req_msg_data_v01},
  {sizeof(nas_config_sig_info_resp_msg_v01), nas_config_sig_info_resp_msg_data_v01},
  {sizeof(nas_sig_info_ind_msg_v01), nas_sig_info_ind_msg_data_v01},
  {sizeof(nas_get_err_rate_req_msg_v01), 0},
  {sizeof(nas_get_err_rate_resp_msg_v01), nas_get_err_rate_resp_msg_data_v01},
  {sizeof(nas_err_rate_ind_msg_v01), nas_err_rate_ind_msg_data_v01},
  {sizeof(nas_hdr_session_close_ind_msg_v01), nas_hdr_session_close_ind_msg_data_v01},
  {sizeof(nas_hdr_uati_update_ind_msg_v01), nas_hdr_uati_update_ind_msg_data_v01},
  {sizeof(nas_get_hdr_subtype_req_msg_v01), nas_get_hdr_subtype_req_msg_data_v01},
  {sizeof(nas_get_hdr_subtype_resp_msg_v01), nas_get_hdr_subtype_resp_msg_data_v01},
  {sizeof(nas_get_hdr_color_code_req_msg_v01), 0},
  {sizeof(nas_get_hdr_color_code_resp_msg_v01), nas_get_hdr_color_code_resp_msg_data_v01},
  {sizeof(nas_get_current_acq_sys_mode_req_msg_v01), 0},
  {sizeof(nas_get_current_acq_sys_mode_resp_msg_v01), nas_get_current_acq_sys_mode_resp_msg_data_v01},
  {sizeof(nas_set_rx_diversity_req_msg_v01), nas_set_rx_diversity_req_msg_data_v01},
  {sizeof(nas_set_rx_diversity_resp_msg_v01), nas_set_rx_diversity_resp_msg_data_v01},
  {sizeof(nas_get_tx_rx_info_req_msg_v01), nas_get_tx_rx_info_req_msg_data_v01},
  {sizeof(nas_get_tx_rx_info_resp_msg_v01), nas_get_tx_rx_info_resp_msg_data_v01},
  {sizeof(nas_update_akey_ext_req_msg_v01), nas_update_akey_ext_req_msg_data_v01},
  {sizeof(nas_update_akey_ext_resp_msg_v01), nas_update_akey_ext_resp_msg_data_v01},
  {sizeof(nas_managed_roaming_ind_msg_v01), nas_managed_roaming_ind_msg_data_v01},
  {sizeof(nas_get_dual_standby_pref_req_msg_v01), 0},
  {sizeof(nas_get_dual_standby_pref_resp_msg_v01), nas_get_dual_standby_pref_resp_msg_data_v01},
  {sizeof(nas_detach_lte_req_msg_v01), 0},
  {sizeof(nas_detach_lte_resp_msg_v01), nas_detach_lte_resp_msg_data_v01},
  {sizeof(nas_block_lte_plmn_req_msg_v01), nas_block_lte_plmn_req_msg_data_v01},
  {sizeof(nas_block_lte_plmn_resp_msg_v01), nas_block_lte_plmn_resp_msg_data_v01},
  {sizeof(nas_unblock_lte_plmn_req_msg_v01), nas_unblock_lte_plmn_req_msg_data_v01},
  {sizeof(nas_unblock_lte_plmn_resp_msg_v01), nas_unblock_lte_plmn_resp_msg_data_v01},
  {sizeof(nas_reset_lte_plmn_blocking_req_msg_v01), 0},
  {sizeof(nas_reset_lte_plmn_blocking_resp_msg_v01), nas_reset_lte_plmn_blocking_resp_msg_data_v01},
  {sizeof(nas_current_plmn_name_ind_msg_v01), nas_current_plmn_name_ind_msg_data_v01},
  {sizeof(nas_config_embms_req_msg_v01), nas_config_embms_req_msg_data_v01},
  {sizeof(nas_config_embms_resp_msg_v01), nas_config_embms_resp_msg_data_v01},
  {sizeof(nas_get_embms_status_req_msg_v01), 0},
  {sizeof(nas_get_embms_status_resp_msg_v01), nas_get_embms_status_resp_msg_data_v01},
  {sizeof(nas_embms_status_ind_msg_v01), nas_embms_status_ind_msg_data_v01},
  {sizeof(nas_get_cdma_position_info_req_msg_v01), 0},
  {sizeof(nas_get_cdma_position_info_resp_msg_v01), nas_get_cdma_position_info_resp_msg_data_v01},
  {sizeof(nas_rf_band_info_ind_msg_v01), nas_rf_band_info_ind_msg_data_v01},
  {sizeof(nas_force_network_search_req_msg_v01), 0},
  {sizeof(nas_force_network_search_resp_msg_v01), nas_force_network_search_resp_msg_data_v01},
  {sizeof(nas_network_reject_ind_msg_v01), nas_network_reject_ind_msg_data_v01},
  {sizeof(nas_get_managed_roaming_config_req_msg_v01), 0},
  {sizeof(nas_get_managed_roaming_config_resp_msg_v01), nas_get_managed_roaming_config_resp_msg_data_v01},
  {sizeof(nas_rtre_cfg_ind_msg_v01), nas_rtre_cfg_ind_msg_data_v01},
  {sizeof(nas_get_centralized_eons_support_status_req_msg_v01), 0},
  {sizeof(nas_get_centralized_eons_support_status_resp_msg_v01), nas_get_centralized_eons_support_status_resp_msg_data_v01},
  {sizeof(nas_config_sig_info2_req_msg_v01), nas_config_sig_info2_req_msg_data_v01},
  {sizeof(nas_config_sig_info2_resp_msg_v01), nas_config_sig_info2_resp_msg_data_v01},
  {sizeof(nas_get_tds_cell_and_position_info_req_msg_v01), 0},
  {sizeof(nas_get_tds_cell_and_position_info_resp_msg_v01), nas_get_tds_cell_and_position_info_resp_msg_data_v01},
  {sizeof(nas_set_hplmn_irat_search_timer_req_msg_v01), nas_set_hplmn_irat_search_timer_req_msg_data_v01},
  {sizeof(nas_set_hplmn_irat_search_timer_resp_msg_v01), nas_set_hplmn_irat_search_timer_resp_msg_data_v01},
  {sizeof(nas_get_embms_sig_req_msg_v01), nas_get_embms_sig_req_msg_data_v01},
  {sizeof(nas_get_embms_sig_resp_msg_v01), nas_get_embms_sig_resp_msg_data_v01},
  {sizeof(nas_limit_sys_info_ind_reporting_req_msg_v01), nas_limit_sys_info_ind_reporting_req_msg_data_v01},
  {sizeof(nas_limit_sys_info_ind_reporting_resp_msg_v01), nas_limit_sys_info_ind_reporting_resp_msg_data_v01},
  {sizeof(nas_get_sys_info_ind_reporting_limit_req_msg_v01), 0},
  {sizeof(nas_get_sys_info_ind_reporting_limit_resp_msg_v01), nas_get_sys_info_ind_reporting_limit_resp_msg_data_v01},
  {sizeof(nas_update_ims_status_req_msg_v01), nas_update_ims_status_req_msg_data_v01},
  {sizeof(nas_update_ims_status_resp_msg_v01), nas_update_ims_status_resp_msg_data_v01},
  {sizeof(nas_get_ims_pref_status_req_msg_v01), 0},
  {sizeof(nas_get_ims_pref_status_resp_msg_v01), nas_get_ims_pref_status_resp_msg_data_v01},
  {sizeof(nas_ims_pref_status_ind_msg_v01), nas_ims_pref_status_ind_msg_data_v01},
  {sizeof(nas_config_plmn_name_ind_reporting_req_msg_v01), nas_config_plmn_name_ind_reporting_req_msg_data_v01},
  {sizeof(nas_config_plmn_name_ind_reporting_resp_msg_v01), nas_config_plmn_name_ind_reporting_resp_msg_data_v01},
  {sizeof(nas_cdma_avoid_system_req_msg_v01), nas_cdma_avoid_system_req_msg_data_v01},
  {sizeof(nas_cdma_avoid_system_resp_msg_v01), nas_cdma_avoid_system_resp_msg_data_v01},
  {sizeof(nas_get_cdma_avoid_system_list_req_msg_v01), 0},
  {sizeof(nas_get_cdma_avoid_system_list_resp_msg_v01), nas_get_cdma_avoid_system_list_resp_msg_data_v01},
  {sizeof(nas_set_hplmn_search_timer_req_msg_v01), nas_set_hplmn_search_timer_req_msg_data_v01},
  {sizeof(nas_set_hplmn_search_timer_resp_msg_v01), nas_set_hplmn_search_timer_resp_msg_data_v01},
  {sizeof(nas_get_hplmn_search_timer_req_msg_v01), 0},
  {sizeof(nas_get_hplmn_search_timer_resp_msg_v01), nas_get_hplmn_search_timer_resp_msg_data_v01},
  {sizeof(nas_get_scan_config_req_msg_v01), 0},
  {sizeof(nas_get_scan_config_resp_msg_v01), nas_get_scan_config_resp_msg_data_v01},
  {sizeof(nas_get_current_scan_info_req_msg_v01), 0},
  {sizeof(nas_get_current_scan_info_resp_msg_v01), nas_get_current_scan_info_resp_msg_data_v01},
  {sizeof(nas_get_subscription_info_req_msg_v01), 0},
  {sizeof(nas_get_subscription_info_resp_msg_v01), nas_get_subscription_info_resp_msg_data_v01},
  {sizeof(nas_get_network_time_req_msg_v01), 0},
  {sizeof(nas_get_network_time_resp_msg_v01), nas_get_network_time_resp_msg_data_v01},
  {sizeof(nas_get_lte_sib16_network_time_req_msg_v01), 0},
  {sizeof(nas_get_lte_sib16_network_time_resp_msg_v01), nas_get_lte_sib16_network_time_resp_msg_data_v01},
  {sizeof(nas_lte_sib16_network_time_ind_msg_v01), nas_lte_sib16_network_time_ind_msg_data_v01},
  {sizeof(nas_set_lte_band_priority_req_msg_v01), nas_set_lte_band_priority_req_msg_data_v01},
  {sizeof(nas_set_lte_band_priority_resp_msg_v01), nas_set_lte_band_priority_resp_msg_data_v01},
  {sizeof(nas_get_embms_sig_ext_req_msg_v01), nas_get_embms_sig_ext_req_msg_data_v01},
  {sizeof(nas_get_embms_sig_ext_resp_msg_v01), nas_get_embms_sig_ext_resp_msg_data_v01},
  {sizeof(nas_lte_cphy_ca_ind_msg_v01), nas_lte_cphy_ca_ind_msg_data_v01},
  {sizeof(nas_get_lte_band_priority_list_req_msg_v01), 0},
  {sizeof(nas_get_lte_band_priority_list_resp_msg_v01), nas_get_lte_band_priority_list_resp_msg_data_v01},
  {sizeof(nas_set_builtin_plmn_list_req_msg_v01), nas_set_builtin_plmn_list_req_msg_data_v01},
  {sizeof(nas_set_builtin_plmn_list_resp_msg_v01), nas_set_builtin_plmn_list_resp_msg_data_v01},
  {sizeof(nas_set_builtin_plmn_list_ind_msg_v01), nas_set_builtin_plmn_list_ind_msg_data_v01},
  {sizeof(nas_perform_incremental_network_scan_req_msg_v01), nas_perform_incremental_network_scan_req_msg_data_v01},
  {sizeof(nas_perform_incremental_network_scan_resp_msg_v01), nas_perform_incremental_network_scan_resp_msg_data_v01},
  {sizeof(nas_perform_incremental_network_scan_ind_msg_v01), nas_perform_incremental_network_scan_ind_msg_data_v01},
  {sizeof(nas_set_drx_req_msg_v01), nas_set_drx_req_msg_data_v01},
  {sizeof(nas_set_drx_resp_msg_v01), nas_set_drx_resp_msg_data_v01},
  {sizeof(nas_get_drx_req_msg_v01), 0},
  {sizeof(nas_get_drx_resp_msg_v01), nas_get_drx_resp_msg_data_v01},
  {sizeof(nas_get_negotiated_drx_req_msg_v01), 0},
  {sizeof(nas_get_negotiated_drx_resp_msg_v01), nas_get_negotiated_drx_resp_msg_data_v01},
  {sizeof(nas_csg_search_selection_config_req_msg_v01), nas_csg_search_selection_config_req_msg_data_v01},
  {sizeof(nas_csg_search_selection_config_resp_msg_v01), nas_csg_search_selection_config_resp_msg_data_v01},
  {sizeof(nas_csg_immediate_search_selection_req_msg_v01), 0},
  {sizeof(nas_csg_immediate_search_selection_resp_msg_v01), nas_csg_immediate_search_selection_resp_msg_data_v01},
  {sizeof(nas_get_csg_search_selection_configuration_req_msg_v01), 0},
  {sizeof(nas_get_csg_search_selection_configuration_resp_msg_v01), nas_get_csg_search_selection_configuration_resp_msg_data_v01},
  {sizeof(nas_ssac_info_ind_msg_v01), nas_ssac_info_ind_msg_data_v01},
  {sizeof(nas_get_lte_embms_info_req_msg_v01), 0},
  {sizeof(nas_get_lte_embms_info_resp_msg_v01), nas_get_lte_embms_info_resp_msg_data_v01},
  {sizeof(nas_get_serv_cell_sib_req_msg_v01), nas_get_serv_cell_sib_req_msg_data_v01},
  {sizeof(nas_get_serv_cell_sib_resp_msg_v01), nas_get_serv_cell_sib_resp_msg_data_v01},
  {sizeof(nas_get_serv_cell_sib_ind_msg_v01), nas_get_serv_cell_sib_ind_msg_data_v01},
  {sizeof(nas_ssac_change_info_ind_msg_v01), nas_ssac_change_info_ind_msg_data_v01},
  {sizeof(nas_get_ssac_info_req_msg_v01), 0},
  {sizeof(nas_get_ssac_info_resp_msg_v01), nas_get_ssac_info_resp_msg_data_v01},
  {sizeof(nas_set_periodic_search_allowed_req_msg_v01), nas_set_periodic_search_allowed_req_msg_data_v01},
  {sizeof(nas_set_periodic_search_allowed_resp_msg_v01), nas_set_periodic_search_allowed_resp_msg_data_v01},
  {sizeof(nas_emm_t3402_changed_ind_msg_v01), nas_emm_t3402_changed_ind_msg_data_v01},
  {sizeof(nas_get_acb_info_req_msg_v01), 0},
  {sizeof(nas_get_acb_info_resp_msg_v01), nas_get_acb_info_resp_msg_data_v01},
  {sizeof(nas_acb_info_ind_msg_v01), nas_acb_info_ind_msg_data_v01},
  {sizeof(nas_set_data_subs_priority_req_msg_v01), nas_set_data_subs_priority_req_msg_data_v01},
  {sizeof(nas_set_data_subs_priority_resp_msg_v01), nas_set_data_subs_priority_resp_msg_data_v01},
  {sizeof(nas_get_data_subs_priority_req_msg_v01), 0},
  {sizeof(nas_get_data_subs_priority_resp_msg_v01), nas_get_data_subs_priority_resp_msg_data_v01},
  {sizeof(nas_data_subs_priority_ind_msg_v01), nas_data_subs_priority_ind_msg_data_v01},
  {sizeof(nas_avoid_tuneaway_req_msg_v01), nas_avoid_tuneaway_req_msg_data_v01},
  {sizeof(nas_avoid_tuneaway_resp_msg_v01), nas_avoid_tuneaway_resp_msg_data_v01},
  {sizeof(nas_set_lte_proc_type_req_msg_v01), nas_set_lte_proc_type_req_msg_data_v01},
  {sizeof(nas_set_lte_proc_type_resp_msg_v01), nas_set_lte_proc_type_resp_msg_data_v01},
  {sizeof(nas_set_mcc_req_msg_v01), nas_set_mcc_req_msg_data_v01},
  {sizeof(nas_set_mcc_resp_msg_v01), nas_set_mcc_resp_msg_data_v01},
  {sizeof(nas_set_data_roaming_req_msg_v01), nas_set_data_roaming_req_msg_data_v01},
  {sizeof(nas_set_data_roaming_resp_msg_v01), nas_set_data_roaming_resp_msg_data_v01},
  {sizeof(nas_get_data_roaming_req_msg_v01), 0},
  {sizeof(nas_get_data_roaming_resp_msg_v01), nas_get_data_roaming_resp_msg_data_v01},
  {sizeof(nas_set_srvcc_req_msg_v01), nas_set_srvcc_req_msg_data_v01},
  {sizeof(nas_set_srvcc_resp_msg_v01), nas_set_srvcc_resp_msg_data_v01},
  {sizeof(nas_set_bsr_timer_req_msg_v01), nas_set_bsr_timer_req_msg_data_v01},
  {sizeof(nas_set_bsr_timer_resp_msg_v01), nas_set_bsr_timer_resp_msg_data_v01},
  {sizeof(nas_get_bsr_timer_req_msg_v01), 0},
  {sizeof(nas_get_bsr_timer_resp_msg_v01), nas_get_bsr_timer_resp_msg_data_v01},
  {sizeof(nas_set_drx_scaling_factor_req_msg_v01), nas_set_drx_scaling_factor_req_msg_data_v01},
  {sizeof(nas_set_drx_scaling_factor_resp_msg_v01), nas_set_drx_scaling_factor_resp_msg_data_v01},
  {sizeof(nas_set_ssac_hysteresis_timer_req_msg_v01), nas_set_ssac_hysteresis_timer_req_msg_data_v01},
  {sizeof(nas_set_ssac_hysteresis_timer_resp_msg_v01), nas_set_ssac_hysteresis_timer_resp_msg_data_v01},
  {sizeof(nas_get_ssac_hysteresis_timer_req_msg_v01), 0},
  {sizeof(nas_get_ssac_hysteresis_timer_resp_msg_v01), nas_get_ssac_hysteresis_timer_resp_msg_data_v01},
  {sizeof(nas_get_hdr_info_req_msg_v01), 0},
  {sizeof(nas_get_hdr_info_resp_msg_v01), nas_get_hdr_info_resp_msg_data_v01},
  {sizeof(nas_get_hdr_drc_rate_req_msg_v01), 0},
  {sizeof(nas_get_hdr_drc_rate_resp_msg_v01), nas_get_hdr_drc_rate_resp_msg_data_v01},
  {sizeof(nas_set_rpm_parameters_req_msg_v01), nas_set_rpm_parameters_req_msg_data_v01},
  {sizeof(nas_set_rpm_parameters_resp_msg_v01), nas_set_rpm_parameters_resp_msg_data_v01},
  {sizeof(nas_get_rpm_parameters_req_msg_v01), 0},
  {sizeof(nas_get_rpm_parameters_resp_msg_v01), nas_get_rpm_parameters_resp_msg_data_v01},
  {sizeof(nas_set_rpm_parameters_2_req_msg_v01), nas_set_rpm_parameters_2_req_msg_data_v01},
  {sizeof(nas_set_rpm_parameters_2_resp_msg_v01), nas_set_rpm_parameters_2_resp_msg_data_v01},
  {sizeof(nas_set_rpm_state_req_msg_v01), nas_set_rpm_state_req_msg_data_v01},
  {sizeof(nas_set_rpm_state_resp_msg_v01), nas_set_rpm_state_resp_msg_data_v01},
  {sizeof(nas_get_lte_cphy_ca_info_req_msg_v01), 0},
  {sizeof(nas_get_lte_cphy_ca_info_resp_msg_v01), nas_get_lte_cphy_ca_info_resp_msg_data_v01},
  {sizeof(nas_manual_scan_fail_ind_msg_v01), 0},
  {sizeof(nas_set_cell_lock_config_req_msg_v01), nas_set_cell_lock_config_req_msg_data_v01},
  {sizeof(nas_set_cell_lock_config_resp_msg_v01), nas_set_cell_lock_config_resp_msg_data_v01},
  {sizeof(nas_set_cell_config_req_msg_v01), nas_set_cell_config_req_msg_data_v01},
  {sizeof(nas_set_cell_config_resp_msg_v01), nas_set_cell_config_resp_msg_data_v01},
  {sizeof(nas_set_nr5g_cell_config_req_msg_v01), nas_set_nr5g_cell_config_req_msg_data_v01},
  {sizeof(nas_set_nr5g_cell_config_resp_msg_v01), nas_set_nr5g_cell_config_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_cell_config_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_cell_config_resp_msg_v01), nas_get_nr5g_cell_config_resp_msg_data_v01},
  {sizeof(nas_lte_ue_config_req_msg_v01), nas_lte_ue_config_req_msg_data_v01},
  {sizeof(nas_lte_ue_config_resp_msg_v01), nas_lte_ue_config_resp_msg_data_v01},
  {sizeof(nas_timer_expiry_ind_v01), nas_timer_expiry_ind_data_v01},
  {sizeof(nas_timer_start_ind_v01), nas_timer_start_ind_data_v01},
  {sizeof(nas_timer_stop_ind_v01), nas_timer_stop_ind_data_v01},
  {sizeof(nas_emergency_mode_status_ind_v01), nas_emergency_mode_status_ind_data_v01},
  {sizeof(nas_ecall_deregistration_req_msg_v01), 0},
  {sizeof(nas_ecall_deregistration_resp_msg_v01), nas_ecall_deregistration_resp_msg_data_v01},
  {sizeof(nas_update_ca_band_combo_req_msg_v01), nas_update_ca_band_combo_req_msg_data_v01},
  {sizeof(nas_update_ca_band_combo_resp_msg_v01), nas_update_ca_band_combo_resp_msg_data_v01},
  {sizeof(nas_get_ca_band_combo_req_msg_v01), nas_get_ca_band_combo_req_msg_data_v01},
  {sizeof(nas_get_ca_band_combo_resp_msg_v01), nas_get_ca_band_combo_resp_msg_data_v01},
  {sizeof(nas_ecall_timer_restart_req_msg_v01), nas_ecall_timer_restart_req_msg_data_v01},
  {sizeof(nas_ecall_timer_restart_resp_msg_v01), nas_ecall_timer_restart_resp_msg_data_v01},
  {sizeof(nas_set_voice_roaming_req_msg_v01), nas_set_voice_roaming_req_msg_data_v01},
  {sizeof(nas_set_voice_roaming_resp_msg_v01), nas_set_voice_roaming_resp_msg_data_v01},
  {sizeof(nas_get_voice_roaming_req_msg_v01), 0},
  {sizeof(nas_get_voice_roaming_resp_msg_v01), nas_get_voice_roaming_resp_msg_data_v01},
  {sizeof(nas_set_edrx_req_msg_v01), nas_set_edrx_req_msg_data_v01},
  {sizeof(nas_set_edrx_resp_msg_v01), nas_set_edrx_resp_msg_data_v01},
  {sizeof(nas_set_edrx_params_req_msg_v01), nas_set_edrx_params_req_msg_data_v01},
  {sizeof(nas_set_edrx_params_resp_msg_v01), nas_set_edrx_params_resp_msg_data_v01},
  {sizeof(nas_get_edrx_params_req_msg_v01), nas_get_edrx_params_req_msg_data_v01},
  {sizeof(nas_get_edrx_params_resp_msg_v01), nas_get_edrx_params_resp_msg_data_v01},
  {sizeof(nas_get_edrx_req_msg_v01), 0},
  {sizeof(nas_get_edrx_resp_msg_v01), nas_get_edrx_resp_msg_data_v01},
  {sizeof(nas_edrx_change_info_ind_v01), nas_edrx_change_info_ind_data_v01},
  {sizeof(nas_set_gnss_session_status_req_msg_v01), nas_set_gnss_session_status_req_msg_data_v01},
  {sizeof(nas_set_gnss_session_status_resp_msg_v01), nas_set_gnss_session_status_resp_msg_data_v01},
  {sizeof(nas_get_lte_rrc_tx_info_req_msg_v01), 0},
  {sizeof(nas_get_lte_rrc_tx_info_resp_msg_v01), nas_get_lte_rrc_tx_info_resp_msg_data_v01},
  {sizeof(nas_lte_rrc_tx_info_ind_v01), nas_lte_rrc_tx_info_ind_data_v01},
  {sizeof(nas_gcell_info_ind_v01), nas_gcell_info_ind_data_v01},
  {sizeof(nas_gcell_info_req_msg_v01), 0},
  {sizeof(nas_gcell_info_resp_msg_v01), nas_gcell_info_resp_msg_data_v01},
  {sizeof(nas_bar_gcell_req_v01), nas_bar_gcell_req_data_v01},
  {sizeof(nas_bar_gcell_resp_v01), nas_bar_gcell_resp_data_v01},
  {sizeof(nas_abort_scan_req_msg_v01), 0},
  {sizeof(nas_abort_scan_resp_msg_v01), nas_abort_scan_resp_msg_data_v01},
  {sizeof(nas_lte_cphy_ca_agg_dl_bw_ind_msg_v01), nas_lte_cphy_ca_agg_dl_bw_ind_msg_data_v01},
  {sizeof(nas_lte_rach_fail_ind_v01), nas_lte_rach_fail_ind_data_v01},
  {sizeof(nas_set_ecid_config_req_msg_v01), nas_set_ecid_config_req_msg_data_v01},
  {sizeof(nas_set_ecid_config_resp_msg_v01), nas_set_ecid_config_resp_msg_data_v01},
  {sizeof(nas_get_ecid_config_req_msg_v01), 0},
  {sizeof(nas_get_ecid_config_resp_msg_v01), nas_get_ecid_config_resp_msg_data_v01},
  {sizeof(nas_v2x_start_req_msg_v01), nas_v2x_start_req_msg_data_v01},
  {sizeof(nas_v2x_start_resp_msg_v01), nas_v2x_start_resp_msg_data_v01},
  {sizeof(nas_v2x_stop_req_msg_v01), 0},
  {sizeof(nas_v2x_stop_resp_msg_v01), nas_v2x_stop_resp_msg_data_v01},
  {sizeof(nas_get_v2x_status_req_msg_v01), 0},
  {sizeof(nas_get_v2x_status_resp_msg_v01), nas_get_v2x_status_resp_msg_data_v01},
  {sizeof(nas_v2x_notification_ind_v01), nas_v2x_notification_ind_data_v01},
  {sizeof(nas_sub_blocked_status_ind_msg_v01), nas_sub_blocked_status_ind_msg_data_v01},
  {sizeof(nas_get_sub_blocked_status_req_msg_v01), 0},
  {sizeof(nas_get_sub_blocked_status_resp_msg_v01), nas_get_sub_blocked_status_resp_msg_data_v01},
  {sizeof(nas_lte_b2_thresh_adj_config_req_msg_v01), nas_lte_b2_thresh_adj_config_req_msg_data_v01},
  {sizeof(nas_lte_b2_thresh_adj_config_resp_msg_v01), nas_lte_b2_thresh_adj_config_resp_msg_data_v01},
  {sizeof(nas_get_arfcn_list_req_msg_v01), 0},
  {sizeof(nas_get_arfcn_list_resp_msg_v01), nas_get_arfcn_list_resp_msg_data_v01},
  {sizeof(nas_arfcn_list_ind_v01), nas_arfcn_list_ind_data_v01},
  {sizeof(nas_5g_modem_vote_ind_v01), nas_5g_modem_vote_ind_data_v01},
  {sizeof(nas_get_rf_availability_req_msg_v01), 0},
  {sizeof(nas_get_rf_availability_resp_msg_v01), nas_get_rf_availability_resp_msg_data_v01},
  {sizeof(nas_rf_availability_ind_msg_v01), nas_rf_availability_ind_msg_data_v01},
  {sizeof(nas_get_cell_lock_config_req_msg_v01), 0},
  {sizeof(nas_get_cell_lock_config_resp_msg_v01), nas_get_cell_lock_config_resp_msg_data_v01},
  {sizeof(nas_set_small_bw_earfcn_req_msg_v01), nas_set_small_bw_earfcn_req_msg_data_v01},
  {sizeof(nas_set_small_bw_earfcn_resp_msg_v01), nas_set_small_bw_earfcn_resp_msg_data_v01},
  {sizeof(nas_get_small_bw_earfcn_req_msg_v01), 0},
  {sizeof(nas_get_small_bw_earfcn_resp_msg_v01), nas_get_small_bw_earfcn_resp_msg_data_v01},
  {sizeof(nas_force_lte_scan_req_msg_v01), 0},
  {sizeof(nas_force_lte_scan_resp_msg_v01), nas_force_lte_scan_resp_msg_data_v01},
  {sizeof(nas_block_nr5g_plmn_req_msg_v01), nas_block_nr5g_plmn_req_msg_data_v01},
  {sizeof(nas_block_nr5g_plmn_resp_msg_v01), nas_block_nr5g_plmn_resp_msg_data_v01},
  {sizeof(nas_unblock_nr5g_plmn_req_msg_v01), nas_unblock_nr5g_plmn_req_msg_data_v01},
  {sizeof(nas_unblock_nr5g_plmn_resp_msg_v01), nas_unblock_nr5g_plmn_resp_msg_data_v01},
  {sizeof(nas_reset_nr5g_plmn_blocking_req_msg_v01), nas_reset_nr5g_plmn_blocking_req_msg_data_v01},
  {sizeof(nas_reset_nr5g_plmn_blocking_resp_msg_v01), nas_reset_nr5g_plmn_blocking_resp_msg_data_v01},
  {sizeof(nas_mm5g_t3502_changed_ind_msg_v01), nas_mm5g_t3502_changed_ind_msg_data_v01},
  {sizeof(nas_set_ims_proc_type_req_msg_v01), nas_set_ims_proc_type_req_msg_data_v01},
  {sizeof(nas_set_ims_proc_type_resp_msg_v01), nas_set_ims_proc_type_resp_msg_data_v01},
  {sizeof(nas_set_endc_config_req_msg_v01), nas_set_endc_config_req_msg_data_v01},
  {sizeof(nas_set_endc_config_resp_msg_v01), nas_set_endc_config_resp_msg_data_v01},
  {sizeof(nas_get_endc_config_req_msg_v01), 0},
  {sizeof(nas_get_endc_config_resp_msg_v01), nas_get_endc_config_resp_msg_data_v01},
  {sizeof(nas_disable_iccid_req_msg_v01), nas_disable_iccid_req_msg_data_v01},
  {sizeof(nas_disable_iccid_resp_msg_v01), nas_disable_iccid_resp_msg_data_v01},
  {sizeof(nas_disable_iccid_ind_msg_v01), nas_disable_iccid_ind_msg_data_v01},
  {sizeof(nas_get_next_sfn_req_msg_v01), nas_get_next_sfn_req_msg_data_v01},
  {sizeof(nas_get_next_sfn_resp_msg_v01), nas_get_next_sfn_resp_msg_data_v01},
  {sizeof(nas_set_sfn_config_req_msg_v01), nas_set_sfn_config_req_msg_data_v01},
  {sizeof(nas_set_sfn_config_resp_msg_v01), nas_set_sfn_config_resp_msg_data_v01},
  {sizeof(nas_sfn_threshold_reached_ind_msg_v01), nas_sfn_threshold_reached_ind_msg_data_v01},
  {sizeof(nas_nr5g_drx_config_param_ind_msg_v01), nas_nr5g_drx_config_param_ind_msg_data_v01},
  {sizeof(nas_set_v2x_peak_tx_pwr_req_msg_v01), nas_set_v2x_peak_tx_pwr_req_msg_data_v01},
  {sizeof(nas_set_v2x_peak_tx_pwr_resp_msg_v01), nas_set_v2x_peak_tx_pwr_resp_msg_data_v01},
  {sizeof(nas_pps_config_req_msg_v01), nas_pps_config_req_msg_data_v01},
  {sizeof(nas_pps_config_resp_msg_v01), nas_pps_config_resp_msg_data_v01},
  {sizeof(nas_panel_pos_change_ind_msg_v01), nas_panel_pos_change_ind_msg_data_v01},
  {sizeof(nas_panel_pos_change_status_msg_v01), nas_panel_pos_change_status_msg_data_v01},
  {sizeof(nas_panel_pos_change_status_resp_msg_v01), nas_panel_pos_change_status_resp_msg_data_v01},
  {sizeof(nas_get_speff_req_msg_v01), nas_get_speff_req_msg_data_v01},
  {sizeof(nas_get_speff_resp_msg_v01), nas_get_speff_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_speff_req_msg_v01), nas_get_nr5g_speff_req_msg_data_v01},
  {sizeof(nas_get_nr5g_speff_resp_msg_v01), nas_get_nr5g_speff_resp_msg_data_v01},
  {sizeof(nas_jamming_status_req_msg_v01), nas_jamming_status_req_msg_data_v01},
  {sizeof(nas_jamming_status_resp_msg_v01), nas_jamming_status_resp_msg_data_v01},
  {sizeof(nas_jamming_status_ind_msg_v01), nas_jamming_status_ind_msg_data_v01},
  {sizeof(nas_ce_level_info_req_msg_v01), 0},
  {sizeof(nas_ce_level_info_resp_msg_v01), nas_ce_level_info_resp_msg_data_v01},
  {sizeof(nas_wwan_sleep_info_ind_msg_v01), nas_wwan_sleep_info_ind_msg_data_v01},
  {sizeof(nas_set_wwan_sleep_threshold_req_msg_v01), nas_set_wwan_sleep_threshold_req_msg_data_v01},
  {sizeof(nas_set_wwan_sleep_threshold_resp_msg_v01), nas_set_wwan_sleep_threshold_resp_msg_data_v01},
  {sizeof(nas_get_wwan_sleep_threshold_req_msg_v01), 0},
  {sizeof(nas_get_wwan_sleep_threshold_resp_msg_v01), nas_get_wwan_sleep_threshold_resp_msg_data_v01},
  {sizeof(nas_fsk_data_req_msg_v01), nas_fsk_data_req_msg_data_v01},
  {sizeof(nas_fsk_data_resp_msg_v01), nas_fsk_data_resp_msg_data_v01},
  {sizeof(nas_set_fsk_hop_table_req_msg_v01), nas_set_fsk_hop_table_req_msg_data_v01},
  {sizeof(nas_set_fsk_hop_table_resp_msg_v01), nas_set_fsk_hop_table_resp_msg_data_v01},
  {sizeof(nas_get_fsk_hop_table_req_msg_v01), 0},
  {sizeof(nas_get_fsk_hop_table_resp_msg_v01), nas_get_fsk_hop_table_resp_msg_data_v01},
  {sizeof(nas_fsk_start_req_msg_v01), nas_fsk_start_req_msg_data_v01},
  {sizeof(nas_fsk_start_resp_msg_v01), nas_fsk_start_resp_msg_data_v01},
  {sizeof(nas_set_fsk_debug_req_msg_v01), nas_set_fsk_debug_req_msg_data_v01},
  {sizeof(nas_set_fsk_debug_resp_msg_v01), nas_set_fsk_debug_resp_msg_data_v01},
  {sizeof(nas_get_fsk_debug_req_msg_v01), 0},
  {sizeof(nas_get_fsk_debug_resp_msg_v01), nas_get_fsk_debug_resp_msg_data_v01},
  {sizeof(nas_fsk_stop_req_msg_v01), 0},
  {sizeof(nas_fsk_stop_resp_msg_v01), nas_fsk_stop_resp_msg_data_v01},
  {sizeof(nas_set_fsk_pcl_req_msg_v01), nas_set_fsk_pcl_req_msg_data_v01},
  {sizeof(nas_set_fsk_pcl_resp_msg_v01), nas_set_fsk_pcl_resp_msg_data_v01},
  {sizeof(nas_get_fsk_pcl_req_msg_v01), 0},
  {sizeof(nas_get_fsk_pcl_resp_msg_v01), nas_get_fsk_pcl_resp_msg_data_v01},
  {sizeof(nas_get_cell_select_info_req_msg_v01), 0},
  {sizeof(nas_get_cell_select_info_resp_msg_v01), nas_get_cell_select_info_resp_msg_data_v01},
  {sizeof(nas_set_sync_pulse_gen_req_msg_v01), nas_set_sync_pulse_gen_req_msg_data_v01},
  {sizeof(nas_set_sync_pulse_gen_resp_msg_v01), nas_set_sync_pulse_gen_resp_msg_data_v01},
  {sizeof(nas_get_sync_pulse_gen_req_msg_v01), 0},
  {sizeof(nas_get_sync_pulse_gen_resp_msg_v01), nas_get_sync_pulse_gen_resp_msg_data_v01},
  {sizeof(nas_lost_frame_sync_ind_msg_v01), nas_lost_frame_sync_ind_msg_data_v01},
  {sizeof(nas_set_dss_config_req_msg_v01), nas_set_dss_config_req_msg_data_v01},
  {sizeof(nas_set_dss_config_resp_msg_v01), nas_set_dss_config_resp_msg_data_v01},
  {sizeof(nas_get_dss_config_req_msg_v01), 0},
  {sizeof(nas_get_dss_config_resp_msg_v01), nas_get_dss_config_resp_msg_data_v01},
  {sizeof(nas_set_nr5g_full_voice_support_req_msg_v01), nas_set_nr5g_full_voice_support_req_msg_data_v01},
  {sizeof(nas_set_nr5g_full_voice_support_resp_msg_v01), nas_set_nr5g_full_voice_support_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_full_voice_support_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_full_voice_support_resp_msg_v01), nas_get_nr5g_full_voice_support_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_disable_status_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_disable_status_resp_msg_v01), nas_get_nr5g_disable_status_resp_msg_data_v01},
  {sizeof(nas_nr5g_disable_status_ind_msg_v01), nas_nr5g_disable_status_ind_msg_data_v01},
  {sizeof(nas_cell_barring_info_ind_msg_v01), nas_cell_barring_info_ind_msg_data_v01},
  {sizeof(nas_access_barring_status_ind_msg_v01), nas_access_barring_status_ind_msg_data_v01},
  {sizeof(nas_set_nsa_on_ndds_req_msg_v01), nas_set_nsa_on_ndds_req_msg_data_v01},
  {sizeof(nas_set_nsa_on_ndds_resp_msg_v01), nas_set_nsa_on_ndds_resp_msg_data_v01},
  {sizeof(nas_get_nsa_on_ndds_req_msg_v01), 0},
  {sizeof(nas_get_nsa_on_ndds_resp_msg_v01), nas_get_nsa_on_ndds_resp_msg_data_v01},
  {sizeof(nas_set_nr5g_sync_pulse_gen_req_msg_v01), nas_set_nr5g_sync_pulse_gen_req_msg_data_v01},
  {sizeof(nas_set_nr5g_sync_pulse_gen_resp_msg_v01), nas_set_nr5g_sync_pulse_gen_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_sync_pulse_gen_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_sync_pulse_gen_resp_msg_v01), nas_get_nr5g_sync_pulse_gen_resp_msg_data_v01},
  {sizeof(nas_nr5g_time_sync_pulse_report_ind_msg_v01), nas_nr5g_time_sync_pulse_report_ind_msg_data_v01},
  {sizeof(nas_nr5g_lost_frame_sync_ind_msg_v01), nas_nr5g_lost_frame_sync_ind_msg_data_v01},
  {sizeof(nas_deprioritize_nr5g_req_msg_v01), nas_deprioritize_nr5g_req_msg_data_v01},
  {sizeof(nas_deprioritize_nr5g_resp_msg_v01), nas_deprioritize_nr5g_resp_msg_data_v01},
  {sizeof(nas_set_black_list_req_msg_v01), nas_set_black_list_req_msg_data_v01},
  {sizeof(nas_set_black_list_resp_msg_v01), nas_set_black_list_resp_msg_data_v01},
  {sizeof(nas_nr5g_rrc_conn_reject_ind_msg_v01), nas_nr5g_rrc_conn_reject_ind_msg_data_v01},
  {sizeof(nas_nr5g_resource_config_ind_msg_v01), nas_nr5g_resource_config_ind_msg_data_v01},
  {sizeof(nas_get_cqi_req_msg_v01), nas_get_cqi_req_msg_data_v01},
  {sizeof(nas_get_cqi_resp_msg_v01), nas_get_cqi_resp_msg_data_v01},
  {sizeof(nas_lte_cphy_ca_aggregated_ind_msg_v01), nas_lte_cphy_ca_aggregated_ind_msg_data_v01},
  {sizeof(nas_v2x_tx_status_report_ind_msg_v01), nas_v2x_tx_status_report_ind_msg_data_v01},
  {sizeof(nas_set_traffic_profile_req_msg_v01), nas_set_traffic_profile_req_msg_data_v01},
  {sizeof(nas_set_traffic_profile_resp_msg_v01), nas_set_traffic_profile_resp_msg_data_v01},
  {sizeof(nas_update_app_performance_req_msg_v01), nas_update_app_performance_req_msg_data_v01},
  {sizeof(nas_update_app_performance_resp_msg_v01), nas_update_app_performance_resp_msg_data_v01},
  {sizeof(nas_set_dc_mute_config_req_msg_v01), nas_set_dc_mute_config_req_msg_data_v01},
  {sizeof(nas_set_dc_mute_config_resp_msg_v01), nas_set_dc_mute_config_resp_msg_data_v01},
  {sizeof(nas_get_dc_mute_config_req_msg_v01), 0},
  {sizeof(nas_get_dc_mute_config_resp_msg_v01), nas_get_dc_mute_config_resp_msg_data_v01},
  {sizeof(nas_get_hst_info_req_msg_v01), 0},
  {sizeof(nas_get_hst_info_resp_msg_v01), nas_get_hst_info_resp_msg_data_v01},
  {sizeof(nas_hst_info_ind_msg_v01), nas_hst_info_ind_msg_data_v01},
  {sizeof(nas_data_enable_ind_msg_v01), nas_data_enable_ind_msg_data_v01},
  {sizeof(nas_get_msim_sub_mode_req_msg_v01), 0},
  {sizeof(nas_get_msim_sub_mode_resp_msg_v01), nas_get_msim_sub_mode_resp_msg_data_v01},
  {sizeof(nas_msim_sub_mode_ind_msg_v01), nas_msim_sub_mode_ind_msg_data_v01},
  {sizeof(nas_nr5g_rrc_bwp_info_ind_msg_v01), nas_nr5g_rrc_bwp_info_ind_msg_data_v01},
  {sizeof(nas_set_gaming_mode_status_req_msg_v01), nas_set_gaming_mode_status_req_msg_data_v01},
  {sizeof(nas_set_gaming_mode_status_resp_msg_v01), nas_set_gaming_mode_status_resp_msg_data_v01},
  {sizeof(nas_set_snpn_list_req_msg_v01), nas_set_snpn_list_req_msg_data_v01},
  {sizeof(nas_set_snpn_list_resp_msg_v01), nas_set_snpn_list_resp_msg_data_v01},
  {sizeof(nas_get_snpn_list_req_msg_v01), 0},
  {sizeof(nas_get_snpn_list_resp_msg_v01), nas_get_snpn_list_resp_msg_data_v01},
  {sizeof(nas_get_phy_channel_info_req_msg_v01), 0},
  {sizeof(nas_get_phy_channel_info_resp_msg_v01), nas_get_phy_channel_info_resp_msg_data_v01},
  {sizeof(nas_nr5g_rrc_utc_time_update_ind_msg_v01), nas_nr5g_rrc_utc_time_update_ind_msg_data_v01},
  {sizeof(nas_get_nr5g_rrc_utc_time_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_rrc_utc_time_resp_msg_v01), nas_get_nr5g_rrc_utc_time_resp_msg_data_v01},
  {sizeof(nas_set_msim_preference_req_msg_v01), nas_set_msim_preference_req_msg_data_v01},
  {sizeof(nas_set_msim_preference_resp_msg_v01), nas_set_msim_preference_resp_msg_data_v01},
  {sizeof(nas_set_disable_ul_mimo_req_msg_v01), nas_set_disable_ul_mimo_req_msg_data_v01},
  {sizeof(nas_set_disable_ul_mimo_resp_msg_v01), nas_set_disable_ul_mimo_resp_msg_data_v01},
  {sizeof(nas_get_disable_ul_mimo_req_msg_v01), 0},
  {sizeof(nas_get_disable_ul_mimo_resp_msg_v01), nas_get_disable_ul_mimo_resp_msg_data_v01},
  {sizeof(nas_cpe_install_mode_req_msg_v01), nas_cpe_install_mode_req_msg_data_v01},
  {sizeof(nas_cpe_install_mode_resp_msg_v01), nas_cpe_install_mode_resp_msg_data_v01},
  {sizeof(nas_cpe_install_meas_metrics_ind_msg_v01), nas_cpe_install_meas_metrics_ind_msg_data_v01},
  {sizeof(nas_cpe_install_mode_status_ind_msg_v01), nas_cpe_install_mode_status_ind_msg_data_v01},
  {sizeof(nas_get_fmd_speed_info_req_msg_v01), 0},
  {sizeof(nas_get_fmd_speed_info_resp_msg_v01), nas_get_fmd_speed_info_resp_msg_data_v01},
  {sizeof(nas_update_channel_lists_req_msg_v01), nas_update_channel_lists_req_msg_data_v01},
  {sizeof(nas_update_channel_lists_resp_msg_v01), nas_update_channel_lists_resp_msg_data_v01},
  {sizeof(nas_nr5g_serving_cell_freq_info_ind_msg_v01), nas_nr5g_serving_cell_freq_info_ind_msg_data_v01},
  {sizeof(nas_nr5g_unauthorized_freq_ind_msg_v01), nas_nr5g_unauthorized_freq_ind_msg_data_v01},
  {sizeof(nas_nr5g_tx_duty_cycle_status_ind_msg_v01), nas_nr5g_tx_duty_cycle_status_ind_msg_data_v01},
  {sizeof(nas_get_v2x_slss_rx_req_msg_v01), 0},
  {sizeof(nas_get_v2x_slss_rx_resp_msg_v01), nas_get_v2x_slss_rx_resp_msg_data_v01},
  {sizeof(nas_v2x_slss_rx_info_ind_msg_v01), nas_v2x_slss_rx_info_ind_msg_data_v01},
  {sizeof(nas_set_nr5g_ca_req_msg_v01), nas_set_nr5g_ca_req_msg_data_v01},
  {sizeof(nas_set_nr5g_ca_resp_msg_v01), nas_set_nr5g_ca_resp_msg_data_v01},
  {sizeof(nas_get_nr5g_ca_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_ca_resp_msg_v01), nas_get_nr5g_ca_resp_msg_data_v01},
  {sizeof(nas_get_cell_config_req_msg_v01), 0},
  {sizeof(nas_get_cell_config_resp_msg_v01), nas_get_cell_config_resp_msg_data_v01},
  {sizeof(nas_nr5g_ca_sig_info_ind_msg_v01), nas_nr5g_ca_sig_info_ind_msg_data_v01},
  {sizeof(nas_get_nr5g_ca_sig_info_req_msg_v01), 0},
  {sizeof(nas_get_nr5g_ca_sig_info_resp_msg_v01), nas_get_nr5g_ca_sig_info_resp_msg_data_v01},
  {sizeof(nas_config_embms_sig_info_req_msg_v01), nas_config_embms_sig_info_req_msg_data_v01},
  {sizeof(nas_config_embms_sig_info_resp_msg_v01), nas_config_embms_sig_info_resp_msg_data_v01},
  {sizeof(nas_update_embms_sig_report_req_msg_v01), nas_update_embms_sig_report_req_msg_data_v01},
  {sizeof(nas_update_embms_sig_report_resp_msg_v01), nas_update_embms_sig_report_resp_msg_data_v01},
  {sizeof(nas_embms_sig_info_ind_msg_v01), nas_embms_sig_info_ind_msg_data_v01},
  {sizeof(nas_stop_embms_sig_info_ind_msg_v01), 0},
  {sizeof(nas_trigger_wwan_reset_req_msg_v01), 0},
  {sizeof(nas_trigger_wwan_reset_resp_msg_v01), nas_trigger_wwan_reset_resp_msg_data_v01},
  {sizeof(nas_set_red_cap_config_req_msg_v01), nas_set_red_cap_config_req_msg_data_v01},
  {sizeof(nas_set_red_cap_config_resp_msg_v01), nas_set_red_cap_config_resp_msg_data_v01},
  {sizeof(nas_update_sleep_status_req_msg_v01), nas_update_sleep_status_req_msg_data_v01},
  {sizeof(nas_update_sleep_status_resp_msg_v01), nas_update_sleep_status_resp_msg_data_v01},
  {sizeof(nas_notify_cellular_data_disabled_req_msg_v01), nas_notify_cellular_data_disabled_req_msg_data_v01},
  {sizeof(nas_notify_cellular_data_disabled_resp_msg_v01), nas_notify_cellular_data_disabled_resp_msg_data_v01},
  {sizeof(nas_profile_switch_ind_msg_v01), nas_profile_switch_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object nas_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *nas_qmi_idl_type_table_object_referenced_tables_v01[] =
{&nas_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &network_access_service_common_qmi_idl_type_table_object_v01, &voice_nas_services_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object nas_qmi_idl_type_table_object_v01 = {
  sizeof(nas_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(nas_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  nas_type_table_v01,
  nas_message_table_v01,
  nas_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry nas_service_command_messages_v01[] = {
  {QMI_NAS_RESET_REQ_MSG_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_NAS_ABORT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 2), 5},
  {QMI_NAS_SET_EVENT_REPORT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 4), 93},
  {QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 7), 353},
  {QMI_NAS_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_NAS_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_NAS_GET_SIGNAL_STRENGTH_REQ_MSG_V01, QMI_IDL_TYPE16(0, 9), 5},
  {QMI_NAS_PERFORM_NETWORK_SCAN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 11), 224},
  {QMI_NAS_INITIATE_NETWORK_REGISTER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 13), 20},
  {QMI_NAS_INITIATE_ATTACH_REQ_MSG_V01, QMI_IDL_TYPE16(0, 15), 4},
  {QMI_NAS_GET_SERVING_SYSTEM_REQ_MSG_V01, QMI_IDL_TYPE16(0, 17), 0},
  {QMI_NAS_GET_HOME_NETWORK_REQ_MSG_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_NAS_GET_PREFERRED_NETWORKS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 22), 0},
  {QMI_NAS_SET_PREFERRED_NETWORKS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 24), 3156},
  {QMI_NAS_GET_FORBIDDEN_NETWORKS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 36), 0},
  {QMI_NAS_SET_FORBIDDEN_NETWORKS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 38), 329},
  {QMI_NAS_SET_TECHNOLOGY_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 97), 6},
  {QMI_NAS_GET_TECHNOLOGY_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 99), 0},
  {QMI_NAS_GET_ACCOLC_REQ_MSG_V01, QMI_IDL_TYPE16(0, 40), 0},
  {QMI_NAS_SET_ACCOLC_REQ_MSG_V01, QMI_IDL_TYPE16(0, 42), 10},
  {QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 101), 0},
  {QMI_NAS_GET_DEVICE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 44), 0},
  {QMI_NAS_SET_DEVICE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 46), 38},
  {QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 48), 0},
  {QMI_NAS_GET_AN_AAA_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 50), 0},
  {QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 52), 684},
  {QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 54), 0},
  {QMI_NAS_SET_DDTM_PREFERENCE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 57), 72},
  {QMI_NAS_GET_OPERATOR_NAME_DATA_REQ_MSG_V01, QMI_IDL_TYPE16(0, 62), 0},
  {QMI_NAS_GET_CSP_PLMN_MODE_BIT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 65), 0},
  {QMI_NAS_UPDATE_AKEY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 68), 29},
  {QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 70), 11},
  {QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 72), 194},
  {QMI_NAS_GET_MOB_CAI_REV_REQ_MSG_V01, QMI_IDL_TYPE16(0, 74), 0},
  {QMI_NAS_GET_RTRE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 76), 0},
  {QMI_NAS_SET_RTRE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 78), 13},
  {QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 80), 0},
  {QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01, QMI_IDL_TYPE16(0, 82), 38},
  {QMI_NAS_BIND_SUBSCRIPTION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 84), 4},
  {QMI_NAS_GET_MODE_PREF_REQ_MSG_V01, QMI_IDL_TYPE16(0, 95), 0},
  {QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01, QMI_IDL_TYPE16(0, 86), 31},
  {QMI_NAS_GET_SYS_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 104), 0},
  {QMI_NAS_GET_SIG_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 107), 0},
  {QMI_NAS_CONFIG_SIG_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 109), 329},
  {QMI_NAS_GET_ERR_RATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 112), 0},
  {QMI_NAS_GET_HDR_SUBTYPE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 117), 7},
  {QMI_NAS_GET_HDR_COLOR_CODE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 119), 0},
  {QMI_NAS_GET_CURRENT_ACQ_SYS_MODE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 121), 0},
  {QMI_NAS_SET_RX_DIVERSITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 123), 5},
  {QMI_NAS_GET_TX_RX_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 125), 11},
  {QMI_NAS_UPDATE_AKEY_EXT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 127), 35},
  {QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01, QMI_IDL_TYPE16(0, 130), 0},
  {QMI_NAS_DETACH_LTE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 132), 0},
  {QMI_NAS_BLOCK_LTE_PLMN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 134), 22},
  {QMI_NAS_UNBLOCK_LTE_PLMN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 136), 8},
  {QMI_NAS_RESET_LTE_PLMN_BLOCKING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 138), 0},
  {QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 141), 9},
  {QMI_NAS_GET_EMBMS_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 143), 0},
  {QMI_NAS_GET_CDMA_POSITION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 146), 0},
  {QMI_NAS_FORCE_NETWORK_SEARCH_REQ_MSG_V01, QMI_IDL_TYPE16(0, 149), 0},
  {QMI_NAS_GET_MANAGED_ROAMING_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 152), 0},
  {QMI_NAS_GET_CENTRALIZED_EONS_SUPPORT_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 155), 0},
  {QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01, QMI_IDL_TYPE16(0, 157), 2110},
  {QMI_NAS_GET_TDS_CELL_AND_POSITION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 159), 0},
  {QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 161), 5},
  {QMI_NAS_GET_EMBMS_SIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 163), 5},
  {QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 165), 11},
  {QMI_NAS_GET_SYS_INFO_IND_REPORTING_LIMIT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 167), 0},
  {QMI_NAS_UPDATE_IMS_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 169), 350},
  {QMI_NAS_GET_IMS_PREF_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 171), 0},
  {QMI_NAS_CONFIG_PLMN_NAME_IND_REPORTING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 174), 4},
  {QMI_NAS_CDMA_AVOID_SYSTEM_REQ_MSG_V01, QMI_IDL_TYPE16(0, 176), 7},
  {QMI_NAS_GET_CDMA_AVOID_SYSTEM_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 178), 0},
  {QMI_NAS_SET_HPLMN_SEARCH_TIME_REQ_MSG_V01, QMI_IDL_TYPE16(0, 180), 7},
  {QMI_NAS_GET_HPLMN_SEARCH_TIME_REQ_MSG_V01, QMI_IDL_TYPE16(0, 182), 0},
  {QMI_NAS_GET_SUBSCRIPTION_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 188), 0},
  {QMI_NAS_GET_NETWORK_TIME_REQ_MSG_V01, QMI_IDL_TYPE16(0, 190), 0},
  {QMI_NAS_GET_LTE_SIB16_NETWORK_TIME_REQ_MSG_V01, QMI_IDL_TYPE16(0, 192), 0},
  {QMI_NAS_SET_LTE_BAND_PRIORITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 195), 260},
  {QMI_NAS_GET_EMBMS_SIG_EXT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 197), 5},
  {QMI_NAS_GET_LTE_BAND_PRIORITY_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 200), 0},
  {QMI_NAS_SET_BUILTIN_PLMN_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 202), 2520},
  {QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 205), 505},
  {QMI_NAS_SET_DRX_REQ_MSG_V01, QMI_IDL_TYPE16(0, 208), 7},
  {QMI_NAS_GET_DRX_REQ_MSG_V01, QMI_IDL_TYPE16(0, 210), 0},
  {QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 214), 86},
  {QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 216), 0},
  {QMI_NAS_GET_CSG_SEARCH_SELECTION_CONFIGURATION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 218), 0},
  {QMI_NAS_GET_LTE_EMBMS_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 221), 0},
  {QMI_NAS_GET_SERV_CELL_SIB_REQ_MSG_V01, QMI_IDL_TYPE16(0, 223), 4},
  {QMI_NAS_GET_SSAC_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 227), 0},
  {QMI_NAS_SET_PERIODIC_SEARCH_ALLOWED_REQ_MSG_V01, QMI_IDL_TYPE16(0, 229), 4},
  {QMI_NAS_GET_ACB_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 232), 0},
  {QMI_NAS_SET_DATA_SUBS_PRIORITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 235), 7},
  {QMI_NAS_GET_DATA_SUBS_PRIORITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 237), 0},
  {QMI_NAS_AVOID_TUNEAWAY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 240), 4},
  {QMI_NAS_SET_MCC_REQ_MSG_V01, QMI_IDL_TYPE16(0, 244), 55},
  {QMI_NAS_SET_DATA_ROAMING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 246), 7},
  {QMI_NAS_GET_DATA_ROAMING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 248), 0},
  {QMI_NAS_SET_SRVCC_REQ_MSG_V01, QMI_IDL_TYPE16(0, 250), 4},
  {QMI_NAS_SET_BSR_TIMER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 252), 7},
  {QMI_NAS_GET_BSR_REQ_MSG_V01, QMI_IDL_TYPE16(0, 254), 0},
  {QMI_NAS_SET_DRX_SCALING_FACTOR_REQ_MSG_V01, QMI_IDL_TYPE16(0, 256), 56},
  {QMI_NAS_SET_SSAC_HYSTERESIS_TIMER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 258), 5},
  {QMI_NAS_GET_SSAC_HYSTERESIS_TIMER_REQ_MSG_V01, QMI_IDL_TYPE16(0, 260), 0},
  {QMI_NAS_GET_HDR_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 262), 0},
  {QMI_NAS_GET_HDR_DRC_RATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 264), 0},
  {QMI_NAS_SET_RPM_PARAMETERS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 266), 9},
  {QMI_NAS_GET_RPM_PARAMETERS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 268), 0},
  {QMI_NAS_SET_RPM_STATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 272), 4},
  {QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 274), 0},
  {QMI_NAS_GET_NEGOTIATED_DRX_REQ_MSG_V01, QMI_IDL_TYPE16(0, 212), 0},
  {QMI_NAS_SET_CELL_LOCK_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 277), 248},
  {QMI_NAS_LTE_UE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 285), 19},
  {QMI_NAS_ECALL_DEREGISTRATION_REQ_MSG_V01, QMI_IDL_TYPE16(0, 291), 0},
  {QMI_NAS_UPDATE_CA_BAND_COMBO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 293), 2317},
  {QMI_NAS_GET_CA_BAND_COMBO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 295), 8},
  {QMI_NAS_ECALL_TIMER_RESTART_REQ_MSG_V01, QMI_IDL_TYPE16(0, 297), 14},
  {QMI_NAS_SET_VOICE_ROAMING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 299), 8},
  {QMI_NAS_GET_VOICE_ROAMING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 301), 0},
  {QMI_NAS_SET_RPM_PARAMETERS_2_REQ_MSG_V01, QMI_IDL_TYPE16(0, 270), 9},
  {QMI_NAS_SET_EDRX_REQ_MSG_V01, QMI_IDL_TYPE16(0, 303), 4},
  {QMI_NAS_GET_EDRX_REQ_MSG_V01, QMI_IDL_TYPE16(0, 309), 0},
  {QMI_NAS_GCELL_INFO_REQ_V01, QMI_IDL_TYPE16(0, 318), 0},
  {QMI_NAS_BAR_GCELL_REQ_V01, QMI_IDL_TYPE16(0, 320), 20},
  {QMI_NAS_SET_EDRX_PARAMS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 305), 23},
  {QMI_NAS_GET_EDRX_PARAMS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 307), 11},
  {QMI_NAS_ABORT_SCAN_REQ_V01, QMI_IDL_TYPE16(0, 322), 0},
  {QMI_NAS_SET_LTE_PROC_TYPE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 242), 7},
  {QMI_NAS_GET_LTE_RRC_TX_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 314), 0},
  {QMI_NAS_SET_GNSS_SESSION_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 312), 4},
  {QMI_NAS_SET_ECID_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 326), 4},
  {QMI_NAS_GET_ECID_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 328), 0},
  {QMI_NAS_GET_SUB_BLOCKED_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 338), 0},
  {QMI_NAS_LTE_B2_THRESH_ADJ_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 340), 22},
  {QMI_NAS_V2X_START_REQ_MSG_V01, QMI_IDL_TYPE16(0, 330), 4},
  {QMI_NAS_V2X_STOP_REQ_MSG_V01, QMI_IDL_TYPE16(0, 332), 0},
  {QMI_NAS_GET_V2X_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 334), 0},
  {QMI_NAS_GET_ARFCN_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 342), 0},
  {QMI_NAS_GET_RF_AVAILABILITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 346), 0},
  {QMI_NAS_GET_CELL_LOCK_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 349), 0},
  {QMI_NAS_SET_CELL_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 279), 368},
  {QMI_NAS_SET_SMALL_BW_EARFCN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 351), 44},
  {QMI_NAS_GET_SMALL_BW_EARFCN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 353), 0},
  {QMI_NAS_FORCE_LTE_SCAN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 355), 0},
  {QMI_NAS_BLOCK_NR5G_PLMN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 357), 30},
  {QMI_NAS_UNBLOCK_NR5G_PLMN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 359), 12},
  {QMI_NAS_RESET_NR5G_PLMN_BLOCKING_REQ_MSG_V01, QMI_IDL_TYPE16(0, 361), 4},
  {QMI_NAS_SET_IMS_PROC_TYPE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 364), 7},
  {QMI_NAS_SET_ENDC_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 366), 8},
  {QMI_NAS_GET_ENDC_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 368), 0},
  {QMI_NAS_SET_CODEBOOK_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 28), 4},
  {QMI_NAS_GET_CODEBOOK_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 30), 0},
  {QMI_NAS_DISABLE_ICCID_REQ_MSG_V01, QMI_IDL_TYPE16(0, 370), 14},
  {QMI_NAS_SET_NR5G_BAND_PRIORITY_REQ_MSG_V01, QMI_IDL_TYPE16(0, 32), 1029},
  {QMI_NAS_GET_NR5G_BAND_PRIORITY_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_NAS_GET_NEXT_SFN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 373), 4},
  {QMI_NAS_SET_SFN_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 375), 11},
  {QMI_NAS_GET_HIGH_CAPABILITY_SUB_REQ_MSG_V01, QMI_IDL_TYPE16(0, 91), 0},
  {QMI_NAS_SET_HIGH_CAPABILITY_SUB_REQ_MSG_V01, QMI_IDL_TYPE16(0, 89), 4},
  {QMI_NAS_JAMMING_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 390), 11},
  {QMI_NAS_FSK_DATA_REQ_MSG_V01, QMI_IDL_TYPE16(0, 400), 204},
  {QMI_NAS_SET_FSK_HOP_TABLE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 402), 209},
  {QMI_NAS_GET_FSK_HOP_TABLE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 404), 0},
  {QMI_NAS_FSK_START_REQ_MSG_V01, QMI_IDL_TYPE16(0, 406), 13},
  {QMI_NAS_SET_FSK_DEBUG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 408), 15},
  {QMI_NAS_GET_FSK_DEBUG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 410), 0},
  {QMI_NAS_FSK_STOP_REQ_MSG_V01, QMI_IDL_TYPE16(0, 412), 0},
  {QMI_NAS_SET_FSK_PCL_REQ_MSG_V01, QMI_IDL_TYPE16(0, 414), 12},
  {QMI_NAS_GET_FSK_PCL_REQ_MSG_V01, QMI_IDL_TYPE16(0, 416), 0},
  {QMI_NAS_GET_SPEFF_REQ_MSG_V01, QMI_IDL_TYPE16(0, 386), 4},
  {QMI_NAS_GET_SCAN_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 184), 0},
  {QMI_NAS_SET_V2X_PEAK_TX_PWR_REQ_MSG_V01, QMI_IDL_TYPE16(0, 379), 4},
  {QMI_NAS_SET_SYNC_PULSE_GEN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 420), 14},
  {QMI_NAS_GET_SYNC_PULSE_GEN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 422), 0},
  {QMI_NAS_GET_NR5G_SPEFF_REQ_MSG_V01, QMI_IDL_TYPE16(0, 388), 4},
  {QMI_NAS_SET_DSS_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 425), 648},
  {QMI_NAS_GET_DSS_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 427), 0},
  {QMI_NAS_SET_NR5G_FULL_VOICE_SUPPORT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 429), 7},
  {QMI_NAS_GET_NR5G_FULL_VOICE_SUPPORT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 431), 0},
  {QMI_NAS_GET_CURRENT_SCAN_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 186), 0},
  {QMI_NAS_SET_NR5G_CELL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 281), 3132},
  {QMI_NAS_GET_NR5G_CELL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 283), 0},
  {QMI_NAS_GET_NR5G_DISABLE_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 433), 0},
  {QMI_NAS_GET_CE_LEVEL_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 393), 0},
  {QMI_NAS_PPS_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 381), 172},
  {QMI_NAS_PANEL_POS_CHANGE_STATUS_MSG_V01, QMI_IDL_TYPE16(0, 384), 15},
  {QMI_NAS_SET_NSA_ON_NDDS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 438), 4},
  {QMI_NAS_GET_NSA_ON_NDDS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 440), 0},
  {QMI_NAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 442), 33},
  {QMI_NAS_GET_NR5G_SYNC_PULSE_GEN_REQ_MSG_V01, QMI_IDL_TYPE16(0, 444), 0},
  {QMI_NAS_GET_WWAN_SLEEP_THRESHOLD_REQ_MSG_V01, QMI_IDL_TYPE16(0, 398), 0},
  {QMI_NAS_SET_WWAN_SLEEP_THRESHOLD_REQ_MSG_V01, QMI_IDL_TYPE16(0, 396), 7},
  {QMI_NAS_DEPRIORITIZE_NR5G_REQ_MSG_V01, QMI_IDL_TYPE16(0, 448), 4},
  {QMI_NAS_GET_CELL_SELECT_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 418), 0},
  {QMI_NAS_SET_BLACK_LIST_REQ_MSG_V01, QMI_IDL_TYPE16(0, 450), 1156},
  {QMI_NAS_GET_CQI_REQ_MSG_V01, QMI_IDL_TYPE16(0, 454), 7},
  {QMI_NAS_SET_TRAFFIC_PROFILE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 458), 164},
  {QMI_NAS_UPDATE_APP_PERFORMANCE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 460), 21},
  {QMI_NAS_SET_DC_MUTE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 462), 16},
  {QMI_NAS_GET_DC_MUTE_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 464), 0},
  {QMI_NAS_GET_HST_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 466), 0},
  {QMI_NAS_GET_MSIM_SUB_MODE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 470), 0},
  {QMI_NAS_SET_GAMING_MODE_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 474), 7},
  {QMI_NAS_SET_SNPN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 476), 644},
  {QMI_NAS_GET_SNPN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 478), 0},
  {QMI_NAS_SET_SIM_OVERDUE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 26), 4},
  {QMI_NAS_GET_PHY_CHANNEL_INFO_REQ_V01, QMI_IDL_TYPE16(0, 480), 0},
  {QMI_NAS_SET_MSIM_PREFERENCE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 485), 7},
  {QMI_NAS_SET_DISABLE_UL_MIMO_REQ_V01, QMI_IDL_TYPE16(0, 487), 4},
  {QMI_NAS_GET_DISABLE_UL_MIMO_REQ_V01, QMI_IDL_TYPE16(0, 489), 0},
  {QMI_NAS_CPE_INSTALL_MODE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 491), 171},
  {QMI_NAS_GET_NR5G_RRC_UTC_TIME_REQ_V01, QMI_IDL_TYPE16(0, 483), 0},
  {QMI_NAS_GET_FMD_SPEED_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 495), 0},
  {QMI_NAS_UPDATE_CHANNEL_LISTS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 497), 166},
  {QMI_NAS_GET_V2X_SLSS_RX_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 502), 0},
  {QMI_NAS_SET_NR5G_CA_REQ_MSG_V01, QMI_IDL_TYPE16(0, 505), 4},
  {QMI_NAS_GET_NR5G_CA_REQ_MSG_V01, QMI_IDL_TYPE16(0, 507), 0},
  {QMI_NAS_GET_CELL_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 509), 0},
  {QMI_NAS_GET_NR5G_CA_SIG_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 512), 0},
  {QMI_NAS_CONFIG_EMBMS_SIG_INFO_REQ_MSG_V01, QMI_IDL_TYPE16(0, 514), 139},
  {QMI_NAS_UPDATE_EMBMS_SIG_REPORT_REQ_MSG_V01, QMI_IDL_TYPE16(0, 516), 4},
  {QMI_NAS_TRIGGER_WWAN_RESET_REQ_MSG_V01, QMI_IDL_TYPE16(0, 520), 0},
  {QMI_NAS_SET_RED_CAP_CONFIG_REQ_MSG_V01, QMI_IDL_TYPE16(0, 522), 7},
  {QMI_NAS_UPDATE_SLEEP_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 524), 4},
  {QMI_NAS_NOTIFY_CELLULAR_DATA_DISABLED_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 526), 4}
};

static const qmi_idl_service_message_table_entry nas_service_response_messages_v01[] = {
  {QMI_NAS_RESET_RESP_MSG_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_NAS_ABORT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_NAS_SET_EVENT_REPORT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_NAS_INDICATION_REGISTER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_NAS_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_NAS_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_NAS_GET_SIGNAL_STRENGTH_RESP_MSG_V01, QMI_IDL_TYPE16(0, 10), 136},
  {QMI_NAS_PERFORM_NETWORK_SCAN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 12), 17498},
  {QMI_NAS_INITIATE_NETWORK_REGISTER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_NAS_INITIATE_ATTACH_RESP_MSG_V01, QMI_IDL_TYPE16(0, 16), 7},
  {QMI_NAS_GET_SERVING_SYSTEM_RESP_MSG_V01, QMI_IDL_TYPE16(0, 18), 683},
  {QMI_NAS_GET_HOME_NETWORK_RESP_MSG_V01, QMI_IDL_TYPE16(0, 21), 554},
  {QMI_NAS_GET_PREFERRED_NETWORKS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 23), 3833},
  {QMI_NAS_SET_PREFERRED_NETWORKS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_NAS_GET_FORBIDDEN_NETWORKS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 37), 268},
  {QMI_NAS_SET_FORBIDDEN_NETWORKS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 39), 7},
  {QMI_NAS_SET_TECHNOLOGY_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 98), 7},
  {QMI_NAS_GET_TECHNOLOGY_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 100), 18},
  {QMI_NAS_GET_ACCOLC_RESP_MSG_V01, QMI_IDL_TYPE16(0, 41), 11},
  {QMI_NAS_SET_ACCOLC_RESP_MSG_V01, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 102), 11},
  {QMI_NAS_GET_DEVICE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 45), 49},
  {QMI_NAS_SET_DEVICE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 47), 7},
  {QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 49), 350},
  {QMI_NAS_GET_AN_AAA_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 51), 11},
  {QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 53), 7},
  {QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 55), 660},
  {QMI_NAS_SET_DDTM_PREFERENCE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 58), 7},
  {QMI_NAS_GET_OPERATOR_NAME_DATA_RESP_MSG_V01, QMI_IDL_TYPE16(0, 63), 36643},
  {QMI_NAS_GET_CSP_PLMN_MODE_BIT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 66), 11},
  {QMI_NAS_UPDATE_AKEY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 69), 7},
  {QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 71), 188},
  {QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 73), 7},
  {QMI_NAS_GET_MOB_CAI_REV_RESP_MSG_V01, QMI_IDL_TYPE16(0, 75), 11},
  {QMI_NAS_GET_RTRE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 77), 15},
  {QMI_NAS_SET_RTRE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 79), 7},
  {QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 81), 3424},
  {QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01, QMI_IDL_TYPE16(0, 83), 3840},
  {QMI_NAS_BIND_SUBSCRIPTION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 85), 7},
  {QMI_NAS_GET_MODE_PREF_RESP_MSG_V01, QMI_IDL_TYPE16(0, 96), 22},
  {QMI_NAS_DUAL_STANDBY_PREF_RESP_MSG_V01, QMI_IDL_TYPE16(0, 87), 7},
  {QMI_NAS_GET_SYS_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 105), 1173},
  {QMI_NAS_GET_SIG_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 108), 92},
  {QMI_NAS_CONFIG_SIG_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 110), 7},
  {QMI_NAS_GET_ERR_RATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 113), 29},
  {QMI_NAS_GET_HDR_SUBTYPE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 118), 12},
  {QMI_NAS_GET_HDR_COLOR_CODE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 120), 11},
  {QMI_NAS_GET_CURRENT_ACQ_SYS_MODE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 122), 49},
  {QMI_NAS_SET_RX_DIVERSITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 124), 7},
  {QMI_NAS_GET_TX_RX_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 126), 199},
  {QMI_NAS_UPDATE_AKEY_EXT_RESP_V01, QMI_IDL_TYPE16(0, 128), 7},
  {QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01, QMI_IDL_TYPE16(0, 131), 38},
  {QMI_NAS_DETACH_LTE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 133), 7},
  {QMI_NAS_BLOCK_LTE_PLMN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 135), 7},
  {QMI_NAS_UNBLOCK_LTE_PLMN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 137), 7},
  {QMI_NAS_RESET_LTE_PLMN_BLOCKING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 139), 7},
  {QMI_NAS_CONFIG_EMBMS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 142), 12},
  {QMI_NAS_GET_EMBMS_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 144), 16},
  {QMI_NAS_GET_CDMA_POSITION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 147), 312},
  {QMI_NAS_FORCE_NETWORK_SEARCH_RESP_MSG_V01, QMI_IDL_TYPE16(0, 150), 7},
  {QMI_NAS_GET_MANAGED_ROAMING_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 153), 11},
  {QMI_NAS_GET_CENTRALIZED_EONS_SUPPORT_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 156), 11},
  {QMI_NAS_CONFIG_SIG_INFO2_RESP_MSG_V01, QMI_IDL_TYPE16(0, 158), 7},
  {QMI_NAS_GET_TDS_CELL_AND_POSITION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 160), 93},
  {QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 162), 7},
  {QMI_NAS_GET_EMBMS_SIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 164), 64},
  {QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 166), 7},
  {QMI_NAS_GET_SYS_INFO_IND_REPORTING_LIMIT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 168), 18},
  {QMI_NAS_UPDATE_IMS_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 170), 7},
  {QMI_NAS_GET_IMS_PREF_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 172), 19},
  {QMI_NAS_CONFIG_PLMN_NAME_IND_REPORTING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 175), 7},
  {QMI_NAS_CDMA_AVOID_SYSTEM_RESP_MSG_V01, QMI_IDL_TYPE16(0, 177), 7},
  {QMI_NAS_GET_CDMA_AVOID_SYSTEM_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 179), 91},
  {QMI_NAS_SET_HPLMN_SEARCH_TIME_RESP_MSG_V01, QMI_IDL_TYPE16(0, 181), 7},
  {QMI_NAS_GET_HPLMN_SEARCH_TIME_RESP_MSG_V01, QMI_IDL_TYPE16(0, 183), 14},
  {QMI_NAS_GET_SUBSCRIPTION_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 189), 51},
  {QMI_NAS_GET_NETWORK_TIME_RESP_MSG_V01, QMI_IDL_TYPE16(0, 191), 35},
  {QMI_NAS_GET_LTE_SIB16_NETWORK_TIME_RESP_MSG_V01, QMI_IDL_TYPE16(0, 193), 50},
  {QMI_NAS_SET_LTE_BAND_PRIORITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 196), 7},
  {QMI_NAS_GET_EMBMS_SIG_EXT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 198), 2400},
  {QMI_NAS_GET_LTE_BAND_PRIORITY_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 201), 527},
  {QMI_NAS_SET_BUILTIN_PLMN_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 203), 7},
  {QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 206), 7},
  {QMI_NAS_SET_DRX_RESP_MSG_V01, QMI_IDL_TYPE16(0, 209), 7},
  {QMI_NAS_GET_DRX_RESP_MSG_V01, QMI_IDL_TYPE16(0, 211), 14},
  {QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 215), 7},
  {QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 217), 7},
  {QMI_NAS_GET_CSG_SEARCH_SELECTION_CONFIGURATION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 219), 93},
  {QMI_NAS_GET_LTE_EMBMS_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 222), 1655},
  {QMI_NAS_GET_SERV_CELL_SIB_RESP_MSG_V01, QMI_IDL_TYPE16(0, 224), 12},
  {QMI_NAS_GET_SSAC_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 228), 39},
  {QMI_NAS_SET_PERIODIC_SEARCH_ALLOWED_RESP_MSG_V01, QMI_IDL_TYPE16(0, 230), 7},
  {QMI_NAS_GET_ACB_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 233), 37},
  {QMI_NAS_SET_DATA_SUBS_PRIORITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 236), 7},
  {QMI_NAS_GET_DATA_SUBS_PRIORITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 238), 14},
  {QMI_NAS_AVOID_TUNEAWAY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 241), 7},
  {QMI_NAS_SET_MCC_RESP_MSG_V01, QMI_IDL_TYPE16(0, 245), 7},
  {QMI_NAS_SET_DATA_ROAMING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 247), 11},
  {QMI_NAS_GET_DATA_ROAMING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 249), 18},
  {QMI_NAS_SET_SRVCC_RESP_MSG_V01, QMI_IDL_TYPE16(0, 251), 7},
  {QMI_NAS_SET_BSR_TIMER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 253), 11},
  {QMI_NAS_GET_BSR_RESP_MSG_V01, QMI_IDL_TYPE16(0, 255), 18},
  {QMI_NAS_SET_DRX_SCALING_FACTOR_RESP_MSG_V01, QMI_IDL_TYPE16(0, 257), 7},
  {QMI_NAS_SET_SSAC_HYSTERESIS_TIMER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 259), 7},
  {QMI_NAS_GET_SSAC_HYSTERESIS_TIMER_RESP_MSG_V01, QMI_IDL_TYPE16(0, 261), 12},
  {QMI_NAS_GET_HDR_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 263), 36},
  {QMI_NAS_GET_HDR_DRC_RATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 265), 11},
  {QMI_NAS_SET_RPM_PARAMETERS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 267), 7},
  {QMI_NAS_GET_RPM_PARAMETERS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 269), 29},
  {QMI_NAS_SET_RPM_STATE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 273), 7},
  {QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 275), 342},
  {QMI_NAS_GET_NEGOTIATED_DRX_RESP_MSG_V01, QMI_IDL_TYPE16(0, 213), 11},
  {QMI_NAS_SET_CELL_LOCK_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 278), 7},
  {QMI_NAS_LTE_UE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 286), 7},
  {QMI_NAS_ECALL_DEREGISTRATION_RESP_MSG_V01, QMI_IDL_TYPE16(0, 292), 7},
  {QMI_NAS_UPDATE_CA_BAND_COMBO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 294), 7},
  {QMI_NAS_GET_CA_BAND_COMBO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 296), 2316},
  {QMI_NAS_ECALL_TIMER_RESTART_RESP_MSG_V01, QMI_IDL_TYPE16(0, 298), 7},
  {QMI_NAS_SET_VOICE_ROAMING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 300), 7},
  {QMI_NAS_GET_VOICE_ROAMING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 302), 15},
  {QMI_NAS_SET_RPM_PARAMETERS_2_RESP_MSG_V01, QMI_IDL_TYPE16(0, 271), 7},
  {QMI_NAS_SET_EDRX_RESP_MSG_V01, QMI_IDL_TYPE16(0, 304), 7},
  {QMI_NAS_GET_EDRX_RESP_MSG_V01, QMI_IDL_TYPE16(0, 310), 11},
  {QMI_NAS_GCELL_INFO_RESP_V01, QMI_IDL_TYPE16(0, 319), 389},
  {QMI_NAS_BAR_GCELL_RESP_V01, QMI_IDL_TYPE16(0, 321), 7},
  {QMI_NAS_SET_EDRX_PARAMS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 306), 7},
  {QMI_NAS_GET_EDRX_PARAMS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 308), 30},
  {QMI_NAS_ABORT_SCAN_RESP_V01, QMI_IDL_TYPE16(0, 323), 7},
  {QMI_NAS_SET_LTE_PROC_TYPE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 243), 7},
  {QMI_NAS_GET_LTE_RRC_TX_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 315), 16},
  {QMI_NAS_SET_GNSS_SESSION_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 313), 7},
  {QMI_NAS_SET_ECID_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 327), 7},
  {QMI_NAS_GET_ECID_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 329), 11},
  {QMI_NAS_GET_SUB_BLOCKED_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 339), 11},
  {QMI_NAS_LTE_B2_THRESH_ADJ_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 341), 14},
  {QMI_NAS_V2X_START_RESP_MSG_V01, QMI_IDL_TYPE16(0, 331), 7},
  {QMI_NAS_V2X_STOP_RESP_MSG_V01, QMI_IDL_TYPE16(0, 333), 7},
  {QMI_NAS_GET_V2X_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 335), 77},
  {QMI_NAS_GET_ARFCN_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 343), 1316},
  {QMI_NAS_GET_RF_AVAILABILITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 347), 11},
  {QMI_NAS_GET_CELL_LOCK_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 350), 251},
  {QMI_NAS_SET_CELL_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 280), 7},
  {QMI_NAS_SET_SMALL_BW_EARFCN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 352), 7},
  {QMI_NAS_GET_SMALL_BW_EARFCN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 354), 51},
  {QMI_NAS_FORCE_LTE_SCAN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 356), 7},
  {QMI_NAS_BLOCK_NR5G_PLMN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 358), 7},
  {QMI_NAS_UNBLOCK_NR5G_PLMN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 360), 7},
  {QMI_NAS_RESET_NR5G_PLMN_BLOCKING_RESP_MSG_V01, QMI_IDL_TYPE16(0, 362), 7},
  {QMI_NAS_SET_IMS_PROC_TYPE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 365), 7},
  {QMI_NAS_SET_ENDC_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 367), 7},
  {QMI_NAS_GET_ENDC_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 369), 15},
  {QMI_NAS_SET_CODEBOOK_CONFIG_RSP_MSG_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_NAS_GET_CODEBOOK_CONFIG_RSP_MSG_V01, QMI_IDL_TYPE16(0, 31), 11},
  {QMI_NAS_DISABLE_ICCID_RESP_MSG_V01, QMI_IDL_TYPE16(0, 371), 7},
  {QMI_NAS_SET_NR5G_BAND_PRIORITY_RESP_MSG_V01, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_NAS_GET_NR5G_BAND_PRIORITY_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 35), 2065},
  {QMI_NAS_GET_NEXT_SFN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 374), 14},
  {QMI_NAS_SET_SFN_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 376), 7},
  {QMI_NAS_GET_HIGH_CAPABILITY_SUB_RESP_MSG_V01, QMI_IDL_TYPE16(0, 92), 11},
  {QMI_NAS_SET_HIGH_CAPABILITY_SUB_RESP_MSG_V01, QMI_IDL_TYPE16(0, 90), 7},
  {QMI_NAS_JAMMING_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 391), 29},
  {QMI_NAS_FSK_DATA_RESP_MSG_V01, QMI_IDL_TYPE16(0, 401), 7},
  {QMI_NAS_SET_FSK_HOP_TABLE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 403), 7},
  {QMI_NAS_GET_FSK_HOP_TABLE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 405), 10111},
  {QMI_NAS_FSK_START_RESP_MSG_V01, QMI_IDL_TYPE16(0, 407), 7},
  {QMI_NAS_SET_FSK_DEBUG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 409), 7},
  {QMI_NAS_GET_FSK_DEBUG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 411), 22},
  {QMI_NAS_FSK_STOP_RESP_MSG_V01, QMI_IDL_TYPE16(0, 413), 7},
  {QMI_NAS_SET_FSK_PCL_RESP_MSG_V01, QMI_IDL_TYPE16(0, 415), 7},
  {QMI_NAS_GET_FSK_PCL_RESP_MSG_V01, QMI_IDL_TYPE16(0, 417), 19},
  {QMI_NAS_GET_SPEFF_RESP_MSG_V01, QMI_IDL_TYPE16(0, 387), 14},
  {QMI_NAS_GET_SCAN_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 185), 46},
  {QMI_NAS_SET_V2X_PEAK_TX_PWR_RESP_MSG_V01, QMI_IDL_TYPE16(0, 380), 7},
  {QMI_NAS_SET_SYNC_PULSE_GEN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 421), 7},
  {QMI_NAS_GET_SYNC_PULSE_GEN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 423), 21},
  {QMI_NAS_GET_NR5G_SPEFF_RESP_MSG_V01, QMI_IDL_TYPE16(0, 389), 14},
  {QMI_NAS_SET_DSS_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 426), 7},
  {QMI_NAS_GET_DSS_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 428), 655},
  {QMI_NAS_SET_NR5G_FULL_VOICE_SUPPORT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 430), 7},
  {QMI_NAS_GET_NR5G_FULL_VOICE_SUPPORT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 432), 14},
  {QMI_NAS_GET_CURRENT_SCAN_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 187), 33},
  {QMI_NAS_SET_NR5G_CELL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 282), 7},
  {QMI_NAS_GET_NR5G_CELL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 284), 3139},
  {QMI_NAS_GET_NR5G_DISABLE_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 434), 29},
  {QMI_NAS_GET_CE_LEVEL_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 394), 22},
  {QMI_NAS_PPS_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 382), 14},
  {QMI_NAS_PANEL_POS_CHANGE_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 385), 7},
  {QMI_NAS_SET_NSA_ON_NDDS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 439), 7},
  {QMI_NAS_GET_NSA_ON_NDDS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 441), 11},
  {QMI_NAS_SET_NR5G_SYNC_PULSE_GEN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 443), 7},
  {QMI_NAS_GET_NR5G_SYNC_PULSE_GEN_RESP_MSG_V01, QMI_IDL_TYPE16(0, 445), 42},
  {QMI_NAS_GET_WWAN_SLEEP_THRESHOLD_RESP_MSG_V01, QMI_IDL_TYPE16(0, 399), 14},
  {QMI_NAS_SET_WWAN_SLEEP_THRESHOLD_RESP_MSG_V01, QMI_IDL_TYPE16(0, 397), 7},
  {QMI_NAS_DEPRIORITIZE_NR5G_RESP_MSG_V01, QMI_IDL_TYPE16(0, 449), 7},
  {QMI_NAS_GET_CELL_SELECT_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 419), 51},
  {QMI_NAS_SET_BLACK_LIST_RESP_MSG_V01, QMI_IDL_TYPE16(0, 451), 7},
  {QMI_NAS_GET_CQI_RESP_MSG_V01, QMI_IDL_TYPE16(0, 455), 99},
  {QMI_NAS_SET_TRAFFIC_PROFILE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 459), 14},
  {QMI_NAS_UPDATE_APP_PERFORMANCE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 461), 22},
  {QMI_NAS_SET_DC_MUTE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 463), 7},
  {QMI_NAS_GET_DC_MUTE_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 465), 23},
  {QMI_NAS_GET_HST_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 467), 15},
  {QMI_NAS_GET_MSIM_SUB_MODE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 471), 28},
  {QMI_NAS_SET_GAMING_MODE_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 475), 7},
  {QMI_NAS_SET_SNPN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 477), 7},
  {QMI_NAS_GET_SNPN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 479), 651},
  {QMI_NAS_SET_SIM_OVERDUE_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_NAS_GET_PHY_CHANNEL_INFO_RESP_V01, QMI_IDL_TYPE16(0, 481), 135},
  {QMI_NAS_SET_MSIM_PREFERENCE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 486), 7},
  {QMI_NAS_SET_DISABLE_UL_MIMO_RESP_V01, QMI_IDL_TYPE16(0, 488), 7},
  {QMI_NAS_GET_DISABLE_UL_MIMO_RESP_V01, QMI_IDL_TYPE16(0, 490), 11},
  {QMI_NAS_CPE_INSTALL_MODE_RESP_MSG_V01, QMI_IDL_TYPE16(0, 492), 14},
  {QMI_NAS_GET_NR5G_RRC_UTC_TIME_RESP_V01, QMI_IDL_TYPE16(0, 484), 94},
  {QMI_NAS_GET_FMD_SPEED_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 496), 30},
  {QMI_NAS_UPDATE_CHANNEL_LISTS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 498), 7},
  {QMI_NAS_GET_V2X_SLSS_RX_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 503), 155},
  {QMI_NAS_SET_NR5G_CA_RESP_MSG_V01, QMI_IDL_TYPE16(0, 506), 7},
  {QMI_NAS_GET_NR5G_CA_RESP_MSG_V01, QMI_IDL_TYPE16(0, 508), 11},
  {QMI_NAS_GET_CELL_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 510), 371},
  {QMI_NAS_GET_NR5G_CA_SIG_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 513), 255},
  {QMI_NAS_CONFIG_EMBMS_SIG_INFO_RESP_MSG_V01, QMI_IDL_TYPE16(0, 515), 7},
  {QMI_NAS_UPDATE_EMBMS_SIG_REPORT_RESP_MSG_V01, QMI_IDL_TYPE16(0, 517), 7},
  {QMI_NAS_TRIGGER_WWAN_RESET_RESP_MSG_V01, QMI_IDL_TYPE16(0, 521), 7},
  {QMI_NAS_SET_RED_CAP_CONFIG_RESP_MSG_V01, QMI_IDL_TYPE16(0, 523), 7},
  {QMI_NAS_UPDATE_SLEEP_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 525), 7},
  {QMI_NAS_NOTIFY_CELLULAR_DATA_DISABLED_STATUS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 527), 7}
};

static const qmi_idl_service_message_table_entry nas_service_indication_messages_v01[] = {
  {QMI_NAS_EVENT_REPORT_IND_MSG_V01, QMI_IDL_TYPE16(0, 6), 1332},
  {QMI_NAS_SERVING_SYSTEM_IND_MSG_V01, QMI_IDL_TYPE16(0, 19), 695},
  {QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01, QMI_IDL_TYPE16(0, 56), 653},
  {QMI_NAS_DDTM_IND_MSG_V01, QMI_IDL_TYPE16(0, 59), 73},
  {QMI_NAS_OPERATOR_NAME_DATA_IND_MSG_V01, QMI_IDL_TYPE16(0, 64), 36636},
  {QMI_NAS_CSP_PLMN_MODE_BIT_IND_MSG_V01, QMI_IDL_TYPE16(0, 67), 4},
  {QMI_NAS_MANAGED_ROAMING_IND_MSG_V01, QMI_IDL_TYPE16(0, 129), 4},
  {QMI_NAS_DUAL_STANDBY_PREF_IND_MSG_V01, QMI_IDL_TYPE16(0, 88), 22},
  {QMI_NAS_SUBSCRIPTION_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 94), 44},
  {QMI_NAS_NETWORK_TIME_IND_MSG_V01, QMI_IDL_TYPE16(0, 103), 23},
  {QMI_NAS_SYS_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 106), 1186},
  {QMI_NAS_SIG_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 111), 85},
  {QMI_NAS_ERR_RATE_IND_MSG_V01, QMI_IDL_TYPE16(0, 114), 22},
  {QMI_NAS_HDR_SESSION_CLOSE_IND_MSG_V01, QMI_IDL_TYPE16(0, 115), 7},
  {QMI_NAS_HDR_UATI_UPDATE_IND_MSG_V01, QMI_IDL_TYPE16(0, 116), 19},
  {QMI_NAS_CURRENT_PLMN_NAME_IND_V01, QMI_IDL_TYPE16(0, 140), 4383},
  {QMI_NAS_EMBMS_STATUS_IND_V01, QMI_IDL_TYPE16(0, 145), 9},
  {QMI_NAS_RF_BAND_INFO_IND_V01, QMI_IDL_TYPE16(0, 148), 39},
  {QMI_NAS_NETWORK_REJECT_IND_V01, QMI_IDL_TYPE16(0, 151), 69},
  {QMI_NAS_RTRE_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 154), 8},
  {QMI_NAS_IMS_PREF_STATUS_IND_V01, QMI_IDL_TYPE16(0, 173), 16},
  {QMI_NAS_LTE_SIB16_NETWORK_TIME_IND_V01, QMI_IDL_TYPE16(0, 194), 43},
  {QMI_NAS_LTE_CPHY_CA_IND_V01, QMI_IDL_TYPE16(0, 199), 356},
  {QMI_NAS_SET_BUILTIN_PLMN_LIST_IND_MSG_V01, QMI_IDL_TYPE16(0, 204), 26},
  {QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01, QMI_IDL_TYPE16(0, 207), 22429},
  {QMI_NAS_SSAC_INFO_IND_V01, QMI_IDL_TYPE16(0, 220), 23},
  {QMI_NAS_GET_SERV_CELL_SIB_IND_MSG_V01, QMI_IDL_TYPE16(0, 225), 4105},
  {QMI_NAS_SSAC_CHANGE_INFO_IND_V01, QMI_IDL_TYPE16(0, 226), 46},
  {QMI_NAS_EMM_T3402_CHANGED_IND_V01, QMI_IDL_TYPE16(0, 231), 7},
  {QMI_NAS_ACB_INFO_IND_V01, QMI_IDL_TYPE16(0, 234), 65},
  {QMI_NAS_DATA_SUBS_PRIORITY_IND_MSG_V01, QMI_IDL_TYPE16(0, 239), 7},
  {QMI_NAS_MANUAL_SCAN_FAIL_IND_MSG_V01, QMI_IDL_TYPE16(0, 276), 0},
  {QMI_NAS_TIMER_EXPIRY_IND_V01, QMI_IDL_TYPE16(0, 287), 7},
  {QMI_NAS_EMERGENCY_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 290), 11},
  {QMI_NAS_GCELL_INFO_IND_V01, QMI_IDL_TYPE16(0, 317), 382},
  {QMI_NAS_EDRX_CHANGE_INFO_IND_V01, QMI_IDL_TYPE16(0, 311), 23},
  {QMI_NAS_LTE_CPHY_CA_AGG_DL_BW_IND_V01, QMI_IDL_TYPE16(0, 324), 10},
  {QMI_NAS_LTE_RACH_FAIL_IND_V01, QMI_IDL_TYPE16(0, 325), 25},
  {QMI_NAS_LTE_RRC_TX_INFO_IND_V01, QMI_IDL_TYPE16(0, 316), 9},
  {QMI_NAS_SUB_BLOCKED_STATUS_IND_V01, QMI_IDL_TYPE16(0, 337), 4},
  {QMI_NAS_V2X_NOTIFICATION_IND_V01, QMI_IDL_TYPE16(0, 336), 103},
  {QMI_NAS_ARFCN_LIST_IND_V01, QMI_IDL_TYPE16(0, 344), 1309},
  {QMI_NAS_5G_MODEM_VOTE_IND_V01, QMI_IDL_TYPE16(0, 345), 4},
  {QMI_NAS_RF_AVAILABILITY_IND_V01, QMI_IDL_TYPE16(0, 348), 4},
  {QMI_NAS_MM5G_T3502_CHANGED_IND_V01, QMI_IDL_TYPE16(0, 363), 7},
  {QMI_NAS_DISABLE_ICCID_IND_MSG_V01, QMI_IDL_TYPE16(0, 372), 14},
  {QMI_NAS_SFN_THRESHOLD_REACHED_IND_MSG_V01, QMI_IDL_TYPE16(0, 377), 7},
  {QMI_NAS_NR5G_DRX_CONFIG_PARAM_IND_MSG_V01, QMI_IDL_TYPE16(0, 378), 21},
  {QMI_NAS_HIGH_CAPABILITY_SUB_IND_MSG_V01, QMI_IDL_TYPE16(0, 93), 4},
  {QMI_NAS_JAMMING_STATUS_IND_MSG_V01, QMI_IDL_TYPE16(0, 392), 22},
  {QMI_NAS_TIMER_START_IND_V01, QMI_IDL_TYPE16(0, 288), 7},
  {QMI_NAS_TIMER_STOP_IND_V01, QMI_IDL_TYPE16(0, 289), 7},
  {QMI_NAS_LOST_FRAME_SYNC_IND_MSG_V01, QMI_IDL_TYPE16(0, 424), 7},
  {QMI_NAS_E911_SCAN_FAIL_IND_V01, QMI_IDL_TYPE16(0, 60), 0},
  {QMI_NAS_NR5G_DISABLE_STATUS_IND_MSG_V01, QMI_IDL_TYPE16(0, 435), 22},
  {QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 436), 82},
  {QMI_NAS_ACCESS_BARRING_STATUS_IND_MSG_V01, QMI_IDL_TYPE16(0, 437), 60},
  {QMI_NAS_PANEL_POS_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 383), 11},
  {QMI_NAS_NR5G_TIME_SYNC_PULSE_REPORT_IND_MSG_V01, QMI_IDL_TYPE16(0, 446), 62},
  {QMI_NAS_NR5G_LOST_FRAME_SYNC_IND_MSG_V01, QMI_IDL_TYPE16(0, 447), 7},
  {QMI_NAS_WWAN_SLEEP_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 395), 7},
  {QMI_NAS_NR5G_RRC_CONN_REJECT_IND_MSG_V01, QMI_IDL_TYPE16(0, 452), 12},
  {QMI_NAS_NR5G_RESOURCE_CONFIG_IND_MSG_V01, QMI_IDL_TYPE16(0, 453), 2893},
  {QMI_NAS_LTE_CPHY_CA_AGGREGATED_IND_V01, QMI_IDL_TYPE16(0, 456), 202},
  {QMI_NAS_NR5G_FREQ_OUT_OF_BAND_RANGE_IND_MSG_V01, QMI_IDL_TYPE16(0, 61), 5},
  {QMI_NAS_V2X_TX_STATUS_REPORT_IND_V01, QMI_IDL_TYPE16(0, 457), 55},
  {QMI_NAS_HST_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 468), 8},
  {QMI_NAS_DATA_ENABLE_IND_MSG_V01, QMI_IDL_TYPE16(0, 469), 4},
  {QMI_NAS_MSIM_SUB_MODE_IND_MSG_V01, QMI_IDL_TYPE16(0, 472), 21},
  {QMI_NAS_NR5G_RRC_BWP_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 473), 1540},
  {QMI_NAS_NR5G_RRC_UTC_TIME_UPDATE_IND_MSG_V01, QMI_IDL_TYPE16(0, 482), 87},
  {QMI_NAS_CPE_INSTALL_MEAS_METRICS_IND_V01, QMI_IDL_TYPE16(0, 493), 264},
  {QMI_NAS_CPE_INSTALL_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 494), 7},
  {QMI_NAS_NR5G_SERVING_CELL_FREQ_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 499), 13},
  {QMI_NAS_NR5G_UNAUTHORIZED_FREQ_IND_MSG_V01, QMI_IDL_TYPE16(0, 500), 70},
  {QMI_NAS_NR5G_TX_DUTY_CYCLE_STATUS_IND_MSG_V01, QMI_IDL_TYPE16(0, 501), 132},
  {QMI_NAS_V2X_SLSS_RX_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 504), 148},
  {QMI_NAS_NR5G_CA_SIG_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 511), 248},
  {QMI_NAS_EMBMS_SIG_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 518), 44},
  {QMI_NAS_STOP_EMBMS_SIG_INFO_IND_MSG_V01, QMI_IDL_TYPE16(0, 519), 0},
  {QMI_NAS_PROFILE_SWITCH_IND_MSG_V01, QMI_IDL_TYPE16(0, 528), 16}
};

/*Service Object*/
struct qmi_idl_service_object nas_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x03,
  36643,
  { sizeof(nas_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nas_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(nas_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { nas_service_command_messages_v01, nas_service_response_messages_v01, nas_service_indication_messages_v01},
  &nas_qmi_idl_type_table_object_v01,
  0x190,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type nas_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( NAS_V01_IDL_MAJOR_VERS != idl_maj_version || NAS_V01_IDL_MINOR_VERS != idl_min_version
       || NAS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&nas_qmi_idl_service_object_v01;
}

