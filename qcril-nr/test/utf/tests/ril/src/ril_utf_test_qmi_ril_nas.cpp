/******************************************************************************
#  Copyright (c) 2014, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

  @file    ril_utf_test_qmi_ril_nas.cpp
  @brief   RIL UTF test case for NAS

  DESCRIPTION
  All NAS related test case definitions should be in this file
  ---------------------------------------------------------------------------
******************************************************************************/
#include "ril_utf_test_qmi_ril.h"
#include "modules/nas/qcril_qmi_nas2.h"

/**==
@testname: ril_utf_test_oem_request_cdma_set_subscription_source
@description: Test case to set cdma subs source with Valid spc data
==**/
test_result ril_utf_test_oem_request_cdma_set_subscription_source_testcase_1(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  /* utility function to set CDMA subscription source with valid SPC maximum of length 6 */
  util_ril_utf_test_cdma_set_subscription_source_with_spc(t, OEM_node, QMI_node, SUBSCRIPTION_SOURCE_RUIM_SIM, "11111", 6, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_oem_request_cdma_set_subscription_source
@description: Test case to set cdma subs source with Invalid spc data
==**/
test_result ril_utf_test_oem_request_cdma_set_subscription_source_testcase_2(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  /* utility function to set CDMA subscription source with invalid SPC length  */
  util_ril_utf_test_cdma_set_subscription_source_with_spc(t, OEM_node, QMI_node, SUBSCRIPTION_SOURCE_RUIM_SIM, "111111", 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_GENERIC_FAILURE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_set_subscription_source_1
@description: This test verify SUCCESS case of CDMA_SET_SUBSCRIPTION_SOURCE when no spc present and CDMA subscription source is RUIM SIM
==**/
test_result ril_utf_test_cdma_set_subscription_source_1( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  int sub_source;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  sub_source = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;
  res = RIL_E_SUCCESS;

  /* utility function to set CDMA subscription source without SPC when source is RUIM_SIM  */
  util_ril_utf_test_cdma_set_subscription_source(t, RIL_node, QMI_node, sub_source, "11111", 6, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_set_subscription_source_2
@description: This test verify SUCCESS case of CDMA_SET_SUBSCRIPTION_SOURCE when no spc present and CDMA subscription source is NV
==**/
test_result ril_utf_test_cdma_set_subscription_source_2( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  int sub_source;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  sub_source = CDMA_SUBSCRIPTION_SOURCE_NV;
  res = RIL_E_SUCCESS;

  /* utility function to set CDMA subscription source without SPC when source is NV  */
  util_ril_utf_test_cdma_set_subscription_source(t, RIL_node, QMI_node, sub_source,"11111", 6, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_set_subscription_source_3
@description: This test verify FAILURE case of CDMA_SET_SUBSCRIPTION_SOURCE when no spc present
              and CDMA subscription source is RUIM SIM
==**/
test_result ril_utf_test_cdma_set_subscription_source_3( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  int sub_source;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  sub_source = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;
  res = RIL_E_GENERIC_FAILURE;

  /* utility function to set CDMA subscription source without SPC when source is RUIM_SIM but MODEM returns FAILURE  */
  util_ril_utf_test_cdma_set_subscription_source(t, RIL_node, QMI_node, sub_source, "11111", 6, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_set_subscription_source_4
@description: This test verify FAILURE case of CDMA_SET_SUBSCRIPTION_SOURCE when no spc present
              and CDMA subscription source is NV
==**/
test_result ril_utf_test_cdma_set_subscription_source_4( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  int sub_source;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  sub_source = CDMA_SUBSCRIPTION_SOURCE_NV;
  res = RIL_E_GENERIC_FAILURE;

  /* utility function to set CDMA subscription source without SPC when source is NV but MODEM returns FAILURE  */
  util_ril_utf_test_cdma_set_subscription_source(t, RIL_node, QMI_node, sub_source, "11111", 6, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}


/**==
@testname: ril_utf_test_cdma_get_subscription_source_1
@description: This test verify SUCCESS case of CDMA_GET_SUBSCRIPTION_SOURCE when CDMA subscription source is RUIM SIM
==**/
test_result ril_utf_test_cdma_get_subscription_source_1( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_rtre_cfg_enum_v01 rtre_cfg_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  rtre_cfg_pref = NAS_RTRE_CFG_RUIM_ONLY_V01;
  res = RIL_E_SUCCESS;

  /* utility function to get CDMA subscription source without SPC when source is RUIM_SIM */
  util_ril_utf_test_cdma_get_subscription_source(t, RIL_node, QMI_node, rtre_cfg_pref, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_get_subscription_source_2
@description: This test verify SUCCESS case of CDMA_GET_SUBSCRIPTION_SOURCE when CDMA subscription source is NV
==**/
test_result ril_utf_test_cdma_get_subscription_source_2( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_rtre_cfg_enum_v01 rtre_cfg_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;

   return ret;
  }
  rtre_cfg_pref = NAS_RTRE_CFG_INTERNAL_SETTINGS_ONLY_V01;
  res = RIL_E_SUCCESS;

  /* utility function to get CDMA subscription source without SPC when source is NV */
  util_ril_utf_test_cdma_get_subscription_source(t, RIL_node, QMI_node, rtre_cfg_pref, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_get_subscription_source_3
@description: This test verify FAILURE case of CDMA_GET_SUBSCRIPTION_SOURCE when CDMA subscription source is RUIM SIM
==**/
test_result ril_utf_test_cdma_get_subscription_source_3( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;

  AndroidTelTestnode RIL_node;
  nas_rtre_cfg_enum_v01 rtre_cfg_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  rtre_cfg_pref = NAS_RTRE_CFG_RUIM_ONLY_V01;
  res = RIL_E_GENERIC_FAILURE;

  util_ril_utf_test_cdma_get_subscription_source(t, RIL_node, QMI_node, rtre_cfg_pref, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_get_subscription_source_4
@description: This test verify FAILURE case of CDMA_GET_SUBSCRIPTION_SOURCE when CDMA subscription source is other than RUIM/NV
==**/
test_result ril_utf_test_cdma_get_subscription_source_4( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_rtre_cfg_enum_v01 rtre_cfg_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  rtre_cfg_pref = NAS_RTRE_CFG_GSM_ON_1X_V01;
  res = RIL_E_SUCCESS;

  util_ril_utf_test_cdma_get_subscription_source(t, RIL_node, QMI_node, rtre_cfg_pref, res);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_cdma_set_roaming_preference_1
@description: This test verify FAILURE case of CDMA_SET_ROAMING_PREFERENCE
==**/
test_result ril_utf_test_cdma_set_roaming_preference_1( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  ril_cdma_roaming_preference_enum_type roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = Home_Networks_only;
  res = RIL_E_MODEM_ERR;
  util_ril_utf_test_cdma_set_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_set_roaming_preference_2
@description: This test verify SUCCESS case of CDMA_SET_ROAMING_PREFERENCE with roam_pref Home_Networks_only
==**/
test_result ril_utf_test_cdma_set_roaming_preference_2( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  ril_cdma_roaming_preference_enum_type roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = Home_Networks_only;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_set_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_set_roaming_preference_3
@description: This test verify SUCCESS case of CDMA_SET_ROAMING_PREFERENCE with roam_pref Roaming_on_Affiliated_networks
==**/
test_result ril_utf_test_cdma_set_roaming_preference_3( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  ril_cdma_roaming_preference_enum_type roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = Roaming_on_Affiliated_networks;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_set_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_set_roaming_preference_4
@description: This test verify SUCCESS case of CDMA_SET_ROAMING_PREFERENCE with roam_pref Roaming_on_Any_Network
==**/
test_result ril_utf_test_cdma_set_roaming_preference_4( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  ril_cdma_roaming_preference_enum_type roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = Roaming_on_Any_Network ;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_set_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_query_roaming_preference_1
@description: This test verify FAILURE case of CDMA_QUERY_ROAMING_PREFERENCE
==**/
test_result ril_utf_test_cdma_query_roaming_preference_1( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_roam_pref_enum_v01 roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = NAS_ROAMING_PREF_OFF_V01;
  res = RIL_E_MODEM_ERR;
  util_ril_utf_test_cdma_query_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_query_roaming_preference_2
@description: This test verify SUCCESS case of cdma_query_roaming_preference with roam_pref NAS_ROAMING_PREF_OFF_V01
==**/
test_result ril_utf_test_cdma_query_roaming_preference_2( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_roam_pref_enum_v01 roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = NAS_ROAMING_PREF_OFF_V01;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_query_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_query_roaming_preference_3
@description: This test verify SUCCESS case of cdma_query_roaming_preference with roam_pref NAS_ROAMING_PREF_NOT_FLASING_V01
==**/
test_result ril_utf_test_cdma_query_roaming_preference_3( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_roam_pref_enum_v01 roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = NAS_ROAMING_PREF_NOT_FLASING_V01;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_query_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_cdma_query_roaming_preference_4
@description: This test verify SUCCESS case of cdma_query_roaming_preference with roam_pref NAS_ROAMING_PREF_ANY_V01
==**/
test_result ril_utf_test_cdma_query_roaming_preference_4( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  nas_roam_pref_enum_v01 roam_pref;
  int res;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("cdma_preference");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }
  roam_pref = NAS_ROAMING_PREF_ANY_V01;
  res = RIL_E_SUCCESS;
  util_ril_utf_test_cdma_query_roaming_preference(t, RIL_node, QMI_node, roam_pref, res);
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_get_preferred_network_type,
@description: This testcase is to get the preferred network type from MODEM in SUCCESS case
==**/
test_result ril_utf_test_get_preferred_network_type(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_get_pref_network_type(t, RIL_node, QMI_node, QMI_NAS_RAT_MODE_PREF_CDMA, RIL_E_SUCCESS, 0, FALSE, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_get_preferred_network_type_failure,
@description: This testcase is to get the preferred network type from MODEM in SUCCESS case
==**/
test_result ril_utf_test_get_preferred_network_type_failure(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

#ifdef QMI_RIL_HAL_UTF
  int android_resp = RIL_E_MODEM_ERR;
#else
  int android_resp = RIL_E_SYSTEM_ERR;
#endif

  util_get_pref_network_type(t, RIL_node, QMI_node, QMI_NAS_RAT_MODE_PREF_CDMA, android_resp, QMI_ERR_INTERNAL_V01, FALSE, 0);
  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_preferred_network_type_resp_failure
@description: This test verify that SET_PREFERRED_NETWORK_TYPE response got Failed.
==**/
test_result ril_utf_test_set_preferred_network_type_resp_failure( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, QMI_ERR_INTERNAL_V01, RIL_E_MODEM_ERR, 0, FALSE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_preferred_network_type_without_ind
@description: This test verify that SET_PREFERRED_NETWORK_TYPE response got SUCCESS, but RIL
did not got Indication. So, RIL will send Failure to ATEL.
==**/
test_result ril_utf_test_set_preferred_network_type_without_ind( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_UNKNOWN, 0, RIL_E_GENERIC_FAILURE, 60005, FALSE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}
/**==
@testname: ril_utf_test_consecutive_set_preferred_network_type_testcase_1
@description: This test verify the consecutive SET_PREFERRED_NETWORK_TYPE request came from Telephony
and before sending response to Telephony  and also after getting response from Modem, RIL will wait till the indication
came from modem for the request.
==**/
test_result ril_utf_test_consecutive_set_preferred_network_type_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  char tags[MAX_TAG_SIZE];

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  RIL_PreferredNetworkType* mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA;
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Android_Mode_Pref = %d", QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA);
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_OPERATING_MODE_REQ_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_OPERATING_MODE_RESP_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_resp_msg();
  QMI_node.set_qmi_dms_get_operating_mode_resp_operating_mode(DMS_OP_MODE_ONLINE_V01);
  QMI_node.set_qmi_dms_get_operating_mode_resp_result(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE, 0, 0);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  QMI_node.add_callflow_tag(tags);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA;
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Android_Mode_Pref = %d", QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA);
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5001);
  t.inject(RIL_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.expect(RIL_node);

  // Expect: QMI_DMS_GET_OPERATING_MODE_REQ_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_OPERATING_MODE_RESP_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_resp_msg();
  QMI_node.set_qmi_dms_get_operating_mode_resp_operating_mode(DMS_OP_MODE_ONLINE_V01);
  QMI_node.set_qmi_dms_get_operating_mode_resp_result(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE, 0, 0);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.expect(QMI_node);

 // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5001);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_consecutive_set_preferred_network_type_testcase_2
@description: This test verify the consecutive SET_PREFERRED_NETWORK_TYPE request came from Telephony
and before sending response to Telephony  and also before getting response from Modem,
RIL will wait till the indication came from modem for the request.
==**/
test_result ril_utf_test_consecutive_set_preferred_network_type_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  char tags[MAX_TAG_SIZE];

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  RIL_PreferredNetworkType* mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA;
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Android_Mode_Pref = %d", QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA);
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_OPERATING_MODE_REQ_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_OPERATING_MODE_RESP_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_resp_msg();
  QMI_node.set_qmi_dms_get_operating_mode_resp_operating_mode(DMS_OP_MODE_ONLINE_V01);
  QMI_node.set_qmi_dms_get_operating_mode_resp_result(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE, 0, 0);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  QMI_node.add_callflow_tag(tags);
  t.expect(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA;
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Android_Mode_Pref = %d", QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA);
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5001);
  t.inject(RIL_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.expect(RIL_node);

  // Expect: QMI_DMS_GET_OPERATING_MODE_REQ_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_OPERATING_MODE_RESP_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_resp_msg();
  QMI_node.set_qmi_dms_get_operating_mode_resp_operating_mode(DMS_OP_MODE_ONLINE_V01);
  QMI_node.set_qmi_dms_get_operating_mode_resp_result(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE, 0, 0);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Success");
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5001);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_preferred_network_type_sdrm_testcase
@description: This test verify the SET_PREFERRED_NETWORK_TYPE request in sdrm mode
==**/
test_result ril_utf_test_set_preferred_network_type_sdrm_testcase( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  char tags[MAX_TAG_SIZE];

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("pref_network_type");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Modem_Mode_Pref = %d", QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  QMI_node.add_callflow_tag(tags);
  t.inject(QMI_node);

  // Inject: QMI_DMS_EVENT_REPORT_IND_V01
  QMI_node.update_default_qmi_dms_event_report_ind_v01();
  QMI_node.set_qmi_dms_event_report_ind_v01_operating_mode(DMS_OP_MODE_SDRM_V01);
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  RIL_PreferredNetworkType* mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA;
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Android_Mode_Pref = %d", QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA);
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_OPERATING_MODE_REQ_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_OPERATING_MODE_RESP_V01
  QMI_node.update_default_qmi_dms_get_operating_mode_resp_msg();
  QMI_node.set_qmi_dms_get_operating_mode_resp_operating_mode(DMS_OP_MODE_SDRM_V01);
  QMI_node.set_qmi_dms_get_operating_mode_resp_result(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_STATE);
  memset(tags,'\0', MAX_TAG_SIZE);
  snprintf(tags, MAX_TAG_SIZE, "Response Invalid State");
  RIL_node.add_callflow_tag(tags);
  RIL_node.set_token_id(5000);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}


/**==
@testname: ril_utf_test_voice_radio_tech_testcase_1
@description: This test verify voice_radio_change happen due to SET_PREFERRED_NETWORK_TYPE request came from Telephony.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  t.clear_db_before_execution();

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_2
@description: This test is to verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS and mode_pref is LTE,
VRT will be sent as RADIO_TECH_LTE.
Later wcdma got in service, so voice_radio_tech will be sent with VRT as RADIO_TECH_UMTS.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_3
@description: This test verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS, mode_pref is QMI_NAS_RAT_MODE_PREF_CDMA_HRPD.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_4
@description: This test is to verify the voice_radio_tech sent as RADIO_TECH_IS95A when CDMA got in service.
Later device got out of service, voice_radio_tech sent as RADIO_TECH_UMTS when Voice_RTE and DATA_RTE is OOS, mode_pref is GWL.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  //util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_5
@description: This test is to verify that RIL should not send voice_radio_tech when Voice_RTE and DATA_RTE is OOS and mode_pref is GLOBAL.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, FALSE, RADIO_TECH_UNKNOWN, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_6
@description: This test is to verify that RIL should send voice_radio_tech when Voice_RTE is 1x.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, FALSE, RADIO_TECH_UNKNOWN, FALSE);

  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_7
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is G+W+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as 3GPP.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, TRUE, TRUE, "123", "07", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_8
@description: This test is to verify that RIL should send voice_radio_tech based on LTE_VOICE_STATUS/LTE_SMS_STATUS in Global mode
in case of domain is CS_PS. When lte_voice_status/lte_sms_status came as None, RIL will report VRT as RADIO_TECH_LTE.
As soon as lte_voice_status/lte_sms_status came as 1X, RIL will report VRT as RADIO_TECH_IS95A.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "08", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, FALSE, TRUE, "123", "08", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_9
@description: This test is to verify that RIL should send voice_radio_tech in SGLTE case.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_9( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_in_sglte_case(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, TRUE, TRUE, FALSE);

  util_voice_radio_tech_in_sglte_case(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, TRUE, FALSE, TRUE, FALSE, FALSE);

  util_voice_radio_tech_in_sglte_case(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, FALSE, FALSE, TRUE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_10
@description: This test is to verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS and mode_pref is LTE,
VRT will be sent as RADIO_TECH_LTE.
Later GSM got in service, so voice_radio_tech will be sent with VRT as RADIO_TECH_GPRS/RADIO_TECH_EDGE.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_10( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_11
@description: This test is to verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS and mode_pref is LTE,
VRT will be sent as RADIO_TECH_LTE.
Later TDSCDMA got in service, so voice_radio_tech will be sent with VRT as RADIO_TECH_TD_SCDMA.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_11( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_12
@description: This test is to verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS and mode_pref is LTE,
VRT will be sent as RADIO_TECH_LTE.
Later GSM is in Limited service, so voice_radio_tech will be sent with VRT as RADIO_TECH_GPRS/RADIO_TECH_EDGE.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_12( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_13
@description: This test is to verify the voice_radio_tech calculation when Voice_RTE and DATA_RTE is OOS and mode_pref is LTE,
VRT will be sent as RADIO_TECH_LTE.
Later TDSCDMA is in Limited service, so voice_radio_tech will be sent with VRT as RADIO_TECH_TD_SCDMA.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_13( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_14
@description: This test is to verify the voice_radio_tech sent as RADIO_TECH_GPRS when GSM is in Limited service.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_14( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);


  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_15
@description: This test is to verify that RIL should send voice_radio_tech in SVLTE case.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_15( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_in_svlte_case(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, TRUE, FALSE, FALSE);

  util_voice_radio_tech_in_svlte_case(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, TRUE, FALSE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_in_svlte_case(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, FALSE, FALSE, TRUE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_16
@description: This test is to verify that RIL should send voice_radio_tech in SVDO case when 1X is in service.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_16( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_in_svdo_case(t, QMI_node, RIL_node, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, NAS_SYS_PERSONALITY_HRPD_V01, TRUE, NAS_SYS_ACTIVE_PROT_HDR_REL0_V01, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_17
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is C+E+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as 3GPP.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_17( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "17", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, FALSE, TRUE, "123", "17", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_18
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is C+E+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as IMS.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_18( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "18", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_IMS_V01, FALSE, TRUE, "123", "18", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_19
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is C+E+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as 1x.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_19( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "19", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, FALSE, TRUE, "123", "19", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_20
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is G+W+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as 1x.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_20( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "20", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, TRUE, TRUE, "123", "20", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_21
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is G+W+L.
Although RIL got Lte_voice_status/Lte_Sms_Status as IMS.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_21( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "21", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_IMS_V01, TRUE, TRUE, "123", "21", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_22
@description: This test is to verify that RIL should send voice_radio_tech based on LTE_VOICE_STATUS/LTE_SMS_STATUS in Global mode
in case of domain is CS_PS. When lte_voice_status/lte_sms_status came as None, RIL will report VRT as RADIO_TECH_LTE.
As soon as lte_voice_status/lte_sms_status came as 3GPP, RIL will report VRT as RADIO_TECH_UMTS.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_22( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "22", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, FALSE, TRUE, "123", "22", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_23
@description: This test is to verify that RIL should send voice_radio_tech based on LTE_VOICE_STATUS/LTE_SMS_STATUS in Global mode
in case of domain is CS_PS. When lte_voice_status/lte_sms_status came as None, RIL will report VRT as RADIO_TECH_LTE.
As soon as lte_voice_status/lte_sms_status came as IMS, RIL will report VRT as RADIO_TECH_LTE if Reported VRT is NOT LTE.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_23( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "23", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_IMS_V01, FALSE, TRUE, "123", "23", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_24
@description: This test is to verify that RIL should send voice_radio_tech based on LTE_VOICE_STATUS/LTE_SMS_STATUS in Global mode
in case of domain is CS_PS. When lte_voice_status/lte_sms_status came as None, RIL will report VRT as RADIO_TECH_LTE.
As soon as lte_voice_status/lte_sms_status came as IMS, RIL will report VRT as RADIO_TECH_LTE if Reported VRT is NOT LTE.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_24( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "24", TRUE, TRUE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_3GPP_V01, FALSE, TRUE, "123", "24", FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, FALSE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_IMS_V01, FALSE, TRUE, "123", "24", FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_25
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is C+E+L.
Although RIL got LTE in service from DSD_IND.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_25( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, 0ull, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_26
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is G+W+L.
Although RIL got LTE in service from DSD_IND.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_26( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, 0ull, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_27
@description: This test is to verify that RIL should send voice_radio_tech as LTE when mode_pref is Global mode,
since RIL got LTE in service from DSD_IND.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_27( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, 0ull, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

#ifdef RIL_UTF_L_MASTER
/**==
@testname: ril_utf_test_voice_radio_tech_testcase_28_1
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is 3GPP, registerd for data only, and LTE in service with no voice
==**/
test_result ril_utf_test_voice_radio_tech_testcase_28_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "281", "28", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_28_2
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is 3GPP, registerd for data only, and LTE in service with voice in 3gpp
==**/
test_result ril_utf_test_voice_radio_tech_testcase_28_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "282", "28", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_28_3
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is 3GPP, registerd for data only, and LTE in service with voice in 3gpp2 with voice in 3gpp2 with voice in 3gpp2 with voice in 3gpp2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_28_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "283", "28", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_28_4
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is 3GPP, registerd for data only, and LTE in service with voice in 3gpp2 with voice in 3gpp2 with voice in 3gpp2 with voice in ims
==**/
test_result ril_utf_test_voice_radio_tech_testcase_28_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "284", "28", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_29_1
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is 3GPP2, registerd for data only, and LTE in service with no voice
==**/
test_result ril_utf_test_voice_radio_tech_testcase_29_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "291", "29", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_29_2
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is 3GPP2, registerd for data only, and LTE in service with voice in 3gpp
==**/
test_result ril_utf_test_voice_radio_tech_testcase_29_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "292", "29", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_29_3
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is 3GPP2, registerd for data only, and LTE in service with voice in 3gpp2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_29_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "293", "29", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_29_4
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is 3GPP2, registerd for data only, and LTE in service with voice in ims
==**/
test_result ril_utf_test_voice_radio_tech_testcase_29_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CDMA_EVDO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "294", "29", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_30
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is global
              after RIL got Lte_voice_status/Lte_Sms_Status as 3GPP
==**/
test_result ril_utf_test_voice_radio_tech_testcase_30( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_3GPP_V01, NAS_SMS_STATUS_1X_V01, TRUE, TRUE, "123", "30", TRUE, TRUE);

  DB_node.utf_db_query_operator_type("123", "30", "3gpp");
  t.expect(DB_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_31
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP2 when mode_pref is global
              after RIL got Lte_voice_status/Lte_Sms_Status as 3GPP2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_31( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_3GPP_V01, TRUE, TRUE, "123", "31", TRUE, TRUE);

  DB_node.utf_db_query_operator_type("123", "31", "3gpp2");
  t.expect(DB_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_32_1
@description: This test is to verify that RIL should send voice_radio_tech 3GPP and learn about that operator type when mode_pref is global, 3GPP in service and registered for data only
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP
==**/
test_result ril_utf_test_voice_radio_tech_testcase_32_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "321", "32", 0);
  DB_node.utf_db_query_operator_type("321", "32", "3gpp");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "321", "32", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_32_2
@description: This test is to verify that RIL should send voice_radio_tech 3GPP and learn about that operator type when mode_pref is global, 3GPP in service and registered for data only
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP
==**/
test_result ril_utf_test_voice_radio_tech_testcase_32_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "322", "32", 0);
  DB_node.utf_db_query_operator_type("322", "32", "3gpp");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_IMS_V01, NAS_SMS_STATUS_IMS_V01, TRUE, TRUE, "322", "32", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_33_1
@description: This test is to verify that RIL should send voice_radio_tech 3GPP2 and learn about that operator type when mode_pref is global, 3GPP2 in service and registered for data only
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_33_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "331", "33", TRUE);

  DB_node.utf_db_query_operator_type("331", "33", "3gpp2");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "331", "33", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_33_2
@description: This test is to verify that RIL should send voice_radio_tech 3GPP2 and learn about that operator type when mode_pref is global, 3GPP2 in service and registered for data only
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_33_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "332", "33", TRUE);

  DB_node.utf_db_query_operator_type("332", "33", "3gpp2");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "332", "33", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_34
@description: This test is to verify that RIL should send voice_radio_tech LTE when mode_pref is global
              after RIL got Lte_voice_status/Lte_Sms_Status as none and NW operator type is unknown
==**/
test_result ril_utf_test_voice_radio_tech_testcase_34( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "34", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_35
@description: This test is to verify that RIL should send voice_radio_tech LTE when mode_pref is LTE_only and only registered for data
==**/
test_result ril_utf_test_voice_radio_tech_testcase_35( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_1X_V01, NAS_SMS_STATUS_1X_V01, TRUE, TRUE, "123", "35", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_36
@description: This test is to verify that RIL should send voice_radio_tech 3GPP and learn about that operator type when mode_pref is global, 3GPP in service and registered for voice
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP
==**/
test_result ril_utf_test_voice_radio_tech_testcase_36( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "36", 0);
  DB_node.utf_db_query_operator_type("123", "36", "3gpp");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "36", TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_37
@description: This test is to verify that RIL should send voice_radio_tech belongs to 3GPP when mode_pref is 3GPP and LTE in service
==**/
test_result ril_utf_test_voice_radio_tech_testcase_37( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "37", TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_38
@description: This test is to verify that RIL should send voice_radio_tech 3GPP2 and learn about that operator type when mode_pref is global, 3GPP2 in service and registered for voice
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP2
==**/
test_result ril_utf_test_voice_radio_tech_testcase_38( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "38", TRUE);

  DB_node.utf_db_query_operator_type("123", "38", "3gpp2");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "38", TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_39
@description: This test is to verify the voice_radio_tech calculation when LTE is in service and mode_pref is LTE only
              VRT will be sent as RADIO_TECH_LTE.
==**/
test_result ril_utf_test_voice_radio_tech_testcase_39( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "39", TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}


/**==
@testname: ril_utf_test_voice_radio_tech_testcase_40
@description: This test is to verify that RIL should send voice_radio_tech 3GPP and learn about that operator type when mode_pref is global, 3GPP in service and registered for voice
              Then RIL should send voice_radio_tech 3GPP2 and learn about that operator type when mode_pref is global, 3GPP2 in service and registered for voice
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as LTE since the operator type get from db is BOTH
==**/
test_result ril_utf_test_voice_radio_tech_testcase_40( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "40", 0);
  DB_node.utf_db_query_operator_type("123", "40", "3gpp");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_cdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_IS95A, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "40", TRUE);
  DB_node.utf_db_query_operator_type("123", "40", "both");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_set_pref_net_type(t, RIL_node, QMI_node, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "40", TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_41
@description: test case checking if is_ril_vrte_learning_enabled is manually set to true
==**/
test_result ril_utf_test_voice_radio_tech_testcase_41( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  t.clear_db_before_execution();

  // update DB property
  db_property_set("is_ril_vrte_learning_enabled", "1");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "41", 0);
  DB_node.utf_db_query_operator_type("123", "41", "3gpp");
  t.expect(DB_node);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_42
@description: test case checking if is_ril_vrte_learning_enabled is off
==**/
test_result ril_utf_test_voice_radio_tech_testcase_42( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  DBTestnode DB_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  t.clear_db_before_execution();

  // update DB property
  db_property_set("is_ril_vrte_learning_enabled", "0");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "42", 0);
  DB_node.utf_db_query_operator_type("123", "42", "");
  t.expect(DB_node);

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_43
@description: This test is to verify that RIL should send voice_radio_tech 3GPP and learn about that operator type when mode_pref is global, 3GPP in service and registered for voice
              After that when LTE is in service RIL should retrive the vrt from db according to mcc and mnc and report vrt as 3GPP
==**/
test_result ril_utf_test_voice_radio_tech_testcase_43( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  DBTestnode DB_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  t.clear_db_before_execution();

  db_property_set("persist.vendor.radio.snapshot_enabled", "1");
  db_property_set("persist.vendor.radio.snapshot_timer", "22");

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, "123", "43", 0);
  DB_node.utf_db_query_operator_type("123", "43", "3gpp");
  t.expect(DB_node);

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UMTS, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "43", TRUE, TRUE, TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, 0ull, TRUE, TRUE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);

  // DSD IND
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_NULL_BEARER_V01, 0ull, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_44
@description: This test is to verify that RIL should send voice_radio_tech 3GPP when voice and data is OOS, mode pref is global, sim app is 3GPP and rtre_config is not NV
==**/
test_result ril_utf_test_voice_radio_tech_testcase_44( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // set vrt to 3gpp first
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set sim app to 3gpp
  util_ril_utf_init_3gpp_sim_app(t, QMI_node, RIL_node, UIM_APP_STATE_DETECTED_V01);

  // set vrt to lte
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set mode_pref to global, expecting vrt as 3gpp
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, 1);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_45
@description: This test is to verify that RIL should send voice_radio_tech 3GPP2 when voice and data is OOS, mode pref is global, sim app is 3GPP2 and rtre_config is not NV
==**/
test_result ril_utf_test_voice_radio_tech_testcase_45( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // set vrt to 3gpp2 first
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);

  // set sim app to 3gpp
  util_ril_utf_init_3gpp2_sim_app(t, QMI_node, RIL_node);

  // set vrt to lte
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set mode_pref to global, expecting vrt as 3gpp2
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, 1);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_46
@description: This test is to verify that RIL should send voice_radio_tech 3GPP when voice and data is OOS, mode pref is global
              sim app is 3gpp, rtre_config is NV and sim pin is locked
==**/
test_result ril_utf_test_voice_radio_tech_testcase_46( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // set vrt to 3gpp first
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set sim app to 3gpp and set pin lock to true
  util_ril_utf_init_3gpp_sim_app(t, QMI_node, RIL_node, UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01);

  /*
  t.set_start();

  // Expect: QMI_DMS_UIM_GET_ICCID_REQ_V01
  QMI_node.update_default_qmi_dms_get_iccid_req();
  t.expect(QMI_node);

  // Inject: QMI_DMS_UIM_GET_ICCID_RESP_V01
  char* uim_id = QMI_node.update_default_qmi_dms_get_iccid_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  char tmp_uim_id[30] = {0x38,0x39,0x30,0x30,0x33,0x31,0x30,0x30,0x30,0x38,0x35,0x38,0x36,0x35,0x31,0x33,0x31,0x31,0x34,0x36};
  memcpy(uim_id, tmp_uim_id, 20);
  t.inject(QMI_node);

  // Expect: QMI_UIM_READ_TRANSPARENT_REQ_V01
  QMI_node.update_default_qmi_uim_read_transparent_reg();
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_UIM_READ_TRANSPARENT_RESP_V01
  QMI_node.update_default_qmi_uim_read_transparent_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  t.set_end();
  */

  // set rtre_cfg to NV
  QMI_node.update_default_qmi_nas_rtre_config_ind_msg();
  QMI_node.set_qmi_nas_rtre_config_ind_rtre_cfg(NAS_RTRE_CFG_INTERNAL_SETTINGS_ONLY_V01);
  t.inject(QMI_node);

  RIL_node.update_default_ril_unsol_cdma_subscription_source_changed(1);
  t.expect(RIL_node);

  // set vrt to lte
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set mode_pref to global, expecting vrt as 3gpp
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, 1);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_radio_tech_testcase_47
@description: This test is to verify that RIL should send voice_radio_tech 3GPP when voice and data is OOS, mode pref is global, sim app is 3GPP and rtre_config is not NV
==**/
test_result ril_utf_test_voice_radio_tech_testcase_47( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // set vrt to 3gpp first
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UMTS, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set sim app to 3gpp
  util_ril_utf_init_3gpp_sim_app(t, QMI_node, RIL_node, UIM_APP_STATE_DETECTED_V01);

  // set rtre_cfg to NV
  QMI_node.update_default_qmi_nas_rtre_config_ind_msg();
  QMI_node.set_qmi_nas_rtre_config_ind_rtre_cfg(NAS_RTRE_CFG_INTERNAL_SETTINGS_ONLY_V01);
  t.inject(QMI_node);

  RIL_node.update_default_ril_unsol_cdma_subscription_source_changed(1);
  t.expect(RIL_node);

  // set vrt to lte
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // set mode_pref to global, expecting vrt as unknown (previous vrte)
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE, 1);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}
#endif

/**==
@testname: ril_utf_test_voice_registration_state_testcase_1
@description: This test is to verify the voice_registration_state request failure case.
==**/
test_result ril_utf_test_voice_registration_state_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_failure(t, QMI_node, RIL_node, QMI_ERR_INTERNAL_V01, RIL_E_MODEM_ERR);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_2
@description: This test is to verify the "voice_registration_state" for WCDMA in service with No Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_3
@description: This test is to verify the "voice_registration_state" for WCDMA in service with Temporary Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, 1, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_4
@description: This test is to verify the "voice_registration_state" for WCDMA in service with Permanent Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTRATION_DENIED, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_5
@description: This test is to verify the "voice_registration_state" for WCDMA in service with Roaming state.
==**/
test_result ril_utf_test_voice_registration_state_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_6
@description: This test is to verify the "voice_registration_state" for WCDMA in Limited service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_7
@description: This test is to verify the "voice_registration_state" for GSM in service with No Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_8
@description: This test is to verify the "voice_registration_state" for GSM in service with Temporary Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, 1, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_9
@description: This test is to verify the "voice_registration_state" for GSM in service with Permanent Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_9( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTRATION_DENIED, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_10
@description: This test is to verify the "voice_registration_state" for GSM in service with Roaming state.
==**/
test_result ril_utf_test_voice_registration_state_testcase_10( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_11
@description: This test is to verify the "voice_registration_state" for GSM in Limited service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_11( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_gsm_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_GPRS, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_12
@description: This test is to verify the "voice_registration_state" for TDSCDMA in service with No Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_12( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);
  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_13
@description: This test is to verify the "voice_registration_state" for TDSCDMA in service with Temporary Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_13( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, 1);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_14
@description: This test is to verify the "voice_registration_state" for TDSCDMA in service with Permanent Rejection.
==**/
test_result ril_utf_test_voice_registration_state_testcase_14( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTRATION_DENIED, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_15
@description: This test is to verify the "voice_registration_state" for TDSCDMA in service with Roaming state.
==**/
test_result ril_utf_test_voice_registration_state_testcase_15( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_16
@description: This test is to verify the "voice_registration_state" for TDSCDMA in Limited service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_16( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE);

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_17
@description: This test is to verify the "voice_registration_state" for LTE in Service with CS domain.
==**/
test_result ril_utf_test_voice_registration_state_testcase_17( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_18
@description: This test is to verify the "voice_registration_state" for LTE in Service with CS domain in Roaming case.
==**/
test_result ril_utf_test_voice_registration_state_testcase_18( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_19
@description: This test is to verify the "voice_registration_state" for LTE in Service with CS domain in Temporary Rejection case.
==**/
test_result ril_utf_test_voice_registration_state_testcase_19( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, 1, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_20
@description: This test is to verify the "voice_registration_state" for LTE in Service with CS domain in Permanent Rejection case.
==**/
test_result ril_utf_test_voice_registration_state_testcase_20( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTRATION_DENIED, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_21
@description: This test is to verify the "voice_registration_state" for LTE in Limited Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_21( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_22
@description: This test is to verify the "voice_registration_state" for LTE in Service with PS domain, but Voice_on_Lte is available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_22( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_23
@description: This test is to verify the "voice_registration_state" for LTE in Service with PS domain in Roaming case, but Voice_on_Lte is available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_23( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, TRUE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_24
@description: This test is to verify the "voice_registration_state" for LTE in Service with PS domain in Temporary Rejection case, but Voice_on_Lte is available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_24( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, TRUE, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, 1, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_25
@description: This test is to verify the "voice_registration_state" for LTE in Service with PS domain in Permanent Rejection case, but Voice_on_Lte is available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_25( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTRATION_DENIED, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, TRUE, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_26
@description: This test is to verify the "voice_registration_state" for LTE in Limited Service, but Voice_on_Lte is available..
==**/
test_result ril_utf_test_voice_registration_state_testcase_26( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, TRUE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_27
@description: This test is to verify the "voice_registration_state" in SGLTE case when both GSM and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_27( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_28
@description: This test is to verify the "voice_registration_state" in SGLTE case when Permanent Rejection happen for GSM RAT and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_28( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_CS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_29
@description: This test is to verify the "voice_registration_state" in SGLTE case when GSM is in Limited Service and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_29( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_30
@description: This test is to verify the "voice_registration_state" in SGLTE case when GSM is in Roaming state and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_30( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_31
@description: This test is to verify the "voice_registration_state" when CDMA is in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_31( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_32
@description: This test is to verify the "voice_registration_state" when CDMA is in Roaming state.
==**/
test_result ril_utf_test_voice_registration_state_testcase_32( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_ON_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_33
@description: This test is to verify the "voice_registration_state" when Device is OOS.
==**/
test_result ril_utf_test_voice_registration_state_testcase_33( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_in_oos( t, QMI_node, RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_34
@description: This test is to verify the "voice_registration_state" for WCDMA in service with Roaming status as 76 and property
"persist.vendor.radio.eri64_as_home" is enable.
==**/
test_result ril_utf_test_voice_registration_state_testcase_34( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.eri64_as_home","1");
  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, (nas_roam_status_enum_type_v01)76, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_35
@description: This test is to verify the "voice_registration_state" for WCDMA in service with Roaming status as 76 and property
"persist.vendor.radio.eri64_as_home" is NOT enable.
==**/
test_result ril_utf_test_voice_registration_state_testcase_35( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.eri64_as_home","0");
  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, (nas_roam_status_enum_type_v01)76, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_36
@description: This test is to verify the "voice_registration_state" in SVLTE case when both 1X and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_36( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_svlte_case(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_37
@description: This test is to verify the "voice_registration_state" in SVLTE case when 1X is NOT in Service and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_37( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_svlte_case(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_38
@description: This test is to verify the "voice_registration_state" in SVLTE case when 1X is in Roaming state and LTE in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_38( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");

  util_voice_registration_state_in_svlte_case(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_ON_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_39
@description: This test is to verify the "voice_registration_state" in SVDO case when both 1X and HDR in Service.
==**/
test_result ril_utf_test_voice_registration_state_testcase_39( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_in_svdo_case(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, NAS_SYS_PERSONALITY_HRPD_V01, TRUE, NAS_SYS_ACTIVE_PROT_HDR_REL0_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_40
@description: This test is to verify the "voice_registration_state" in SVDO case when both 1X and HDR in Service, But 1X in Roaming.
==**/
test_result ril_utf_test_voice_registration_state_testcase_40( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_in_svdo_case(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_ON_V01, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, NAS_SYS_PERSONALITY_HRPD_V01, TRUE, NAS_SYS_ACTIVE_PROT_HDR_REL0_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_41
@description: This test is to verify the "voice_registration_state" for WCDMA in service with
SRV_STATUS as FULL_SRV, TRUE_SRV_STATUS as LIMITED_REGIONAL and DOMAIN as CS_ONLY.
==**/
test_result ril_utf_test_voice_registration_state_testcase_41( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_42
@description: This test is to verify the "voice_registration_state" for WCDMA in service with
SRV_STATUS as FULL_SRV, TRUE_SRV_STATUS as LIMITED_REGIONAL and DOMAIN as CAMPED. Also,
Property "persist.vendor.radio.ignore_dom_time" is NOT set.
==**/
test_result ril_utf_test_voice_registration_state_testcase_42( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_43
@description: This test is to verify the "voice_registration_state" for WCDMA in service with
SRV_STATUS as FULL_SRV, TRUE_SRV_STATUS as LIMITED_REGIONAL and DOMAIN as CAMPED. Also,
Property "persist.vendor.radio.ignore_dom_time" is set.
==**/
test_result ril_utf_test_voice_registration_state_testcase_43( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.ignore_dom_time","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL, false);

  // Expect: RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
  RIL_node.update_default_ril_unsol_response_voice_network_state_changed(); // This UNSOL will get once "ignore_dom_time" expires.
  t.expect(RIL_node);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL, false);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_44
@description: This test is to verify the "voice_registration_state" for WCDMA in service with
SRV_STATUS as LIMITED, TRUE_SRV_STATUS as LIMITED_REGIONAL and DOMAIN as CAMPED. Also,
Property "persist.vendor.radio.ignore_dom_time" is NOT set.
==**/
test_result ril_utf_test_voice_registration_state_testcase_44( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_45
@description: This test is to verify the "voice_registration_state" for WCDMA in service with
SRV_STATUS as FULL_SRV, TRUE_SRV_STATUS as LIMITED_REGIONAL and DOMAIN as CAMPED. Also,
Property "persist.vendor.radio.ignore_dom_time" is set.
==**/
test_result ril_utf_test_voice_registration_state_testcase_45( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.ignore_dom_time","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_46
@description: This test is to verify the "voice_registration_state" for LTE in full service with primary plmn info available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_46( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", "124", "67", FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_1
@description: This test is to verify the Data_registration_state request failure case.
==**/
test_result ril_utf_test_data_registration_state_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, 0ull, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_LTE, FALSE, FALSE);
  util_data_registration_state_failure(t, QMI_node, RIL_node, QMI_ERR_INTERNAL_V01, RIL_E_MODEM_ERR);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_2
@description: This test is to verify the "Data_registration_state" when both QMI_NAS and DSD reported WCDMA in service with No Rejection.
==**/
test_result ril_utf_test_data_registration_state_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WCDMA_V01, QMI_DSD_3GPP_SO_MASK_WCDMA_V01, TRUE, FALSE, TRUE, TRUE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_wcdma_success(t, QMI_node, RIL_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_3
@description: This test is to verify the "Data_registration_state" when both QMI_NAS and DSD reported WCDMA in service with Temporary Rejection.
==**/
test_result ril_utf_test_data_registration_state_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WCDMA_V01, QMI_DSD_3GPP_SO_MASK_WCDMA_V01, TRUE, FALSE, TRUE, TRUE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_wcdma_success(t, QMI_node, RIL_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_PS_ONLY_V01, 1, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_4
@description: This test is to verify the "Data_registration_state" when both QMI_NAS and DSD reported WCDMA in service with Permanent Rejection.
==**/
test_result ril_utf_test_data_registration_state_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WCDMA_V01, QMI_DSD_3GPP_SO_MASK_WCDMA_V01, TRUE, FALSE, TRUE, TRUE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_wcdma_success(t, QMI_node, RIL_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, TRUE, SYS_SRV_DOMAIN_PS_ONLY_V01, IMSI_UNKNOWN_IN_HLR, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_5
@description: This test is to verify the "Data_registration_state" when both QMI_NAS and DSD reported WCDMA in service with Roaming state.
==**/
test_result ril_utf_test_data_registration_state_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WCDMA_V01, QMI_DSD_3GPP_SO_MASK_WCDMA_V01, TRUE, FALSE, TRUE, TRUE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_wcdma_success(t, QMI_node, RIL_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_6
@description: This test is to verify the "Data_registration_state" when DSD reported WCDMA and
QMI_NAS reported WCDMA in limited service.
==**/
test_result ril_utf_test_data_registration_state_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WCDMA_V01, QMI_DSD_3GPP_SO_MASK_WCDMA_V01, TRUE, FALSE, TRUE, TRUE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_wcdma_success(t, QMI_node, RIL_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_7
@description: This test is to verify the "Data_registration_state" when DSD reported LTE and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);
  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_8
@description: This test is to verify the "Data_registration_state" when DSD reported LTE as Preffered along with WLAN
if property "persist.vendor.radio.data_con_rprt" is FALSE then RIL will report LTE as DATA_REG_STATE without looking for IMS reg state or wfc_call_preference.
==**/
test_result ril_utf_test_data_registration_state_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, TRUE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_9
@description: This test is to verify the "Data_registration_state" when DSD reported LTE as Preffered along with WLAN
if property "persist.vendor.radio.data_con_rprt" is TRUE but IMS is NOT registered then RIL will report IWLAN as DATA_REG_STATE
looking for WFC_CALL_PREFERENCE through IMSS.
==**/
test_result ril_utf_test_data_registration_state_testcase_9( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.data_con_rprt", "true");
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, TRUE, TRUE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_IWLAN, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_10
@description: This test is to verify the "Data_registration_state" when DSD reported IWLAN as Preffered RAT
if property "persist.vendor.radio.data_con_rprt" is FALSE then RIL will report IWLAN as DATA_REG_STATE
without looking for WFC_CALL_PREFERENCE through IMSS or IMS reg state.
==**/
test_result ril_utf_test_data_registration_state_testcase_10( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.data_con_rprt", "false");
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_WLAN_V01, DSD_SYS_RAT_EX_3GPP_WLAN_V01, QMI_DSD_SO_MASK_UNSPECIFIED_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_IWLAN, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_11
@description: This test is to verify the "Data_registration_state" when DSD reported LTE as Preffered along with WLAN
if property "persist.vendor.radio.data_con_rprt" is FALSE but IMS is registered over WLAN then RIL will report LTE as DATA_REG_STATE
==**/
test_result ril_utf_test_data_registration_state_testcase_11( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.data_con_rprt", "false");
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_imsa_registration_status(t, IMS_node, QMI_node, TRUE, FALSE, 0, TRUE, IMSA_STATUS_REGISTERED_V01, FALSE, NULL, TRUE, ims_Registration_RegState_REGISTERED);
  util_ril_utf_imsa_service_status(t, IMS_node, QMI_node, IMSA_FULL_SERVICE_V01, IMSA_FULL_SERVICE_V01, IMSA_FULL_SERVICE_V01, IMSA_WLAN_V01, IMSA_WLAN_V01, IMSA_WLAN_V01);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, TRUE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_12
@description: This test is to verify the "Data_registration_state" when DSD reported LTE as Preffered along with WLAN
if property "persist.vendor.radio.data_con_rprt" is TRUE but IMS is registered over WLAN then RIL will report IWLAN as DATA_REG_STATE
==**/
test_result ril_utf_test_data_registration_state_testcase_12( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.data_con_rprt", "true");
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_imsa_registration_status(t, IMS_node, QMI_node, TRUE, FALSE, 0, TRUE, IMSA_STATUS_REGISTERED_V01, FALSE, NULL, TRUE, ims_Registration_RegState_REGISTERED);
  util_ril_utf_imsa_service_status(t, IMS_node, QMI_node, IMSA_FULL_SERVICE_V01, IMSA_FULL_SERVICE_V01, IMSA_FULL_SERVICE_V01, IMSA_WLAN_V01, IMSA_WLAN_V01, IMSA_WLAN_V01);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, TRUE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_IWLAN, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_13
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as LTE_CA_DL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_13( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_CA_DL_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_14
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as LTE_CA_UL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_14( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, QMI_DSD_3GPP_SO_MASK_LTE_CA_UL_V01, TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_15
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as FDD_LTE+LTE_CA_DL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_15( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, (QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01|QMI_DSD_3GPP_SO_MASK_LTE_CA_DL_V01), TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_16
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as FDD_LTE+LTE_CA_UL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_16( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, (QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01|QMI_DSD_3GPP_SO_MASK_LTE_CA_UL_V01), TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_17
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as TDD_LTE+LTE_CA_DL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_17( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, (QMI_DSD_3GPP_SO_MASK_LTE_TDD_V01|QMI_DSD_3GPP_SO_MASK_LTE_CA_DL_V01), TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_18
@description: This test is to verify the "Data_registration_state" when DSD reported LTE with SO_MASK as TDD_LTE+LTE_CA_UL and QMI_NAS reported LTE in service.
==**/
test_result ril_utf_test_data_registration_state_testcase_18( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_LTE, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, TRUE, TRUE, "123", "07", TRUE, TRUE);
  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_LTE_V01, (QMI_DSD_3GPP_SO_MASK_LTE_TDD_V01|QMI_DSD_3GPP_SO_MASK_LTE_CA_UL_V01), TRUE, FALSE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);
  util_data_registration_state_for_lte_success(t, QMI_node, RIL_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_data_registration_state_testcase_19
@description: This test is to verify the "Data_registration_state" QMI_NAS reported TDSCDMA limited service.
==**/
test_result ril_utf_test_data_registration_state_testcase_19( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Data_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_CDMA_EVDO_AUTO, RADIO_TECH_IS95A, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, TRUE);
  util_voice_radio_tech_tdscdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_TD_SCDMA, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE);

  // Inject: RIL_REQUEST_DATA_REGISTRATION_STATE
  RIL_node.update_default_ril_request_data_registration_state();
  t.inject(RIL_node);

  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg((qmi_result_type_v01)0, (qmi_error_type_v01)0);
  t.inject(QMI_node);

  //Expect: RIL_REQUEST_DATA_REGISTRATION_STATE_RESP
  RIL_DataRegistrationStateResponse* reg_state_resp = RIL_node.update_default_ril_request_data_registration_state_resp();
  reg_state_resp->regState = (RIL_RegState)RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV;
  reg_state_resp->rat = (RIL_RadioTechnology)RADIO_TECH_TD_SCDMA;
  t.expect(RIL_node);

  // Expect: RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
  RIL_node.update_default_ril_unsol_response_voice_network_state_changed();
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_1
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got Failed from Modem side.
*/
test_result ril_utf_test_query_network_scan_testcase_1( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, RIL_E_MODEM_ERR, 0, FALSE, FALSE, FALSE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_2
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got SUSSESS from Modem side, but with empty response data.
*/
test_result ril_utf_test_query_network_scan_testcase_2( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_MODEM_ERR, 0, FALSE, FALSE, FALSE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_3
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got SUSSESS from Modem side, but with valid response data without RAT.
*/
test_result ril_utf_test_query_network_scan_testcase_3( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS, 2, FALSE, FALSE, FALSE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_4
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got SUSSESS from Modem side, but with valid response data with RAT in Legacy case.
*/
test_result ril_utf_test_query_network_scan_testcase_4( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  property_set("persist.vendor.radio.rat_on", "legacy");

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS, 2, TRUE, FALSE, FALSE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_5
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got SUSSESS from Modem side, but with valid response data with RAT in Combine case.
*/
test_result ril_utf_test_query_network_scan_testcase_5( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  property_set("persist.vendor.radio.rat_on", "combine");

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS, 2, FALSE, TRUE, FALSE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_query_network_scan_testcase_6
@description: This test verify RIL_REQUEST_QUERY_AVAILABLE_NETWORKS when request
got SUSSESS from Modem side with same plmn having different nw_status, but with valid response data without RAT.
*/
test_result ril_utf_test_query_network_scan_testcase_6( user_specified_attrib_t attr )
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("nw_scan");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_ril_utf_network_scan( t, RIL_node, QMI_node, 7, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS, 2, FALSE, FALSE, TRUE );

  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_1
@description: This test is to verify the "Operator_Name" for CDMA RAT from Static Table.
==**/
test_result ril_utf_test_operator_name_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "310", "00", 331, 1, "Test PLMN 1-2", "Test1-2", FALSE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_2
@description: This test is to verify the "Operator_Name" for CDMA RAT from "3GPP2_SUBSCRIPTION_INFO_REQ"
if Operator name is not present in static table and request to modem got failed and property "persist.vendor.radio.fill_eons"
is NOT set.
==**/
test_result ril_utf_test_operator_name_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "456", "78", 331, 1, NULL, NULL, TRUE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_3
@description: This test is to verify the "Operator_Name" for CDMA RAT from "3GPP2_SUBSCRIPTION_INFO_REQ"
if Operator name is not present in static table and request to modem got failed and property
"persist.vendor.radio.fill_eons" is set.
==**/
test_result ril_utf_test_operator_name_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "456", "78", 331, 1, NULL, NULL, TRUE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_4
@description: This test is to verify the "Operator_Name" for CDMA RAT from "3GPP2_SUBSCRIPTION_INFO_REQ"
if Operator name is not present in static table and request to modem got success, also property
"persist.vendor.radio.fill_eons" is set.
==**/
test_result ril_utf_test_operator_name_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "456", "78", 331, 1, "Vodafone", NULL, TRUE, QMI_RESULT_SUCCESS_V01, (qmi_error_type_v01)0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_5
@description: This test is to verify the "Operator_Name" for CDMA RAT from "3GPP2_SUBSCRIPTION_INFO_REQ"
if Operator name is not present in static table and request to modem got success, also property
"persist.vendor.radio.fill_eons" is NOT set.
==**/
test_result ril_utf_test_operator_name_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "456", "78", 331, 1, "Vodafone", NULL, TRUE, QMI_RESULT_SUCCESS_V01, (qmi_error_type_v01)0, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_6
@description: This test is to verify the "Operator_Name" when Device is OOS.
==**/
test_result ril_utf_test_operator_name_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_to_get_operator_name_oos_limited_case(t, QMI_node, RIL_node, FALSE, NULL, NULL, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_7
@description: This test is to verify the "Operator_Name" when GSM is in Limited Service
and Property "persist.vendor.env.spec" is not set to "ChinaTelecom".
==**/
test_result ril_utf_test_operator_name_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "001", "01");

  util_to_get_operator_name_oos_limited_case(t, QMI_node, RIL_node, FALSE, NULL, NULL, NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_8
@description: This test is to verify the "Operator_Name" when GSM is in Limited Service
and Property "persist.vendor.env.spec" is set to "ChinaTelecom", Operator name present in
Static Table.
==**/
test_result ril_utf_test_operator_name_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.env.spec","ChinaTelecom");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "001", "01");

  util_to_get_operator_name_oos_limited_case(t, QMI_node, RIL_node, TRUE, "001", "01", "Test PLMN 1-1", "Test1-1");

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_9
@description: This test is to verify the "Operator_Name" when GSM is in Limited Service
and Property "persist.vendor.env.spec" is set to "ChinaTelecom", Operator name not available
in Static Table.
==**/
test_result ril_utf_test_operator_name_testcase_9( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.env.spec","ChinaTelecom");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "101", "01");

  util_to_get_operator_name_oos_limited_case(t, QMI_node, RIL_node, TRUE, "101", "01", NULL, NULL);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_10
@description: This test is to verify the "Operator_Name" for CDMA RAT from Static Table, where mcc=""
and mnc="".
==**/
test_result ril_utf_test_operator_name_testcase_10( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_to_get_operator_name_3gpp2(t, QMI_node, RIL_node, "", "", 4107, 1, "Sprint", "Sprint", FALSE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_11
@description: This test is to verify the "Operator_Name" when Device is in UMTS network and
CSG_ID info is not available. Also, property "persist.vendor.radio.csg_info_avlbl" and
"persist.vendor.radio.fill_eons" is NOT set. Also, RIL Query modem for EONS op name default
and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_11( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","0");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", FALSE, 0, 0, NULL);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_UMTS_V01, "Vodafone", NULL, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_12
@description: This test is to verify the "Operator_Name" when Device is in UMTS network and
CSG_ID info is not available. Also, property "persist.vendor.radio.csg_info_avlbl" and
"persist.vendor.radio.fill_eons" is set. Also, RIL Query modem for EONS op name default
and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_12( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", FALSE, 0, 0, NULL);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_UMTS_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_13
@description: This test is to verify the "Operator_Name" when Device is in UMTS network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is NOT set and
propert "persist.vendor.radio.fill_eons" is set. Also, RIL Query modem for EONS op name default
and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_13( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","0");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_UMTS_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_14
@description: This test is to verify the "Operator_Name" when Device is in UMTS network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" and
"persist.vendor.radio.fill_eons" is set. Also, RIL Query modem for EONS op name default
and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_14( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_15
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is NOT available. Also, property "persist.vendor.radio.csg_info_avlbl" is NOT set. Also, RIL Query modem for EONS op name default
and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_15( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","0");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_16
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is NOT available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op name default and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_16( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_17
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is NOT set.
Also, RIL Query modem for EONS op name default and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_17( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","0");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_18
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op name default and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_18( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_19
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" and
"persist.vendor.radio.fill_eons" are set. In this RIL didn't got valid operator name
through GET_PLMN_REQ and no corresponding PLMN name available in STATIC table.
==**/
test_result ril_utf_test_operator_name_testcase_19( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, NULL, NULL, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_20
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set and
"persist.vendor.radio.fill_eons" is NOT set. In this RIL didn't got valid operator name
through GET_PLMN_REQ and no corresponding PLMN name available in STATIC table.
==**/
test_result ril_utf_test_operator_name_testcase_20( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, NULL, NULL, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_21
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
In this RIL didn't got valid operator name through GET_PLMN_REQ and corresponding
PLMN name available in STATIC table.
==**/
test_result ril_utf_test_operator_name_testcase_21( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "001", "01", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "001", "01", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Test PLMN 1-1", "Test1-1", FALSE, TRUE, FALSE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_22
@description: This test is to verify the "Operator_Name" when Device is in LTE network
and RIL got Operator Name through QMI_NAS_CURRENT_PLMN_NAME_IND_V01.
==**/
test_result ril_utf_test_operator_name_testcase_22( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.radio.sglte_csfb", "true");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_update_cache_current_plmn_ind(t, QMI_node, RIL_node, TRUE, 123, 45, FALSE, TRUE, (nas_coding_scheme_enum_v01)0, 8, "spn Name", TRUE, (nas_coding_scheme_enum_v01)0, 15, "plmn Short Name", TRUE, (nas_coding_scheme_enum_v01)0, 14, "plmn Long Name");
  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", 0, 0, (nas_radio_if_enum_v01)0, "plmn Long Name", "plmn Short Name", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_23
@description: This test is to verify the "Operator_Name" when Device is in LTE network
and RIL got Operator Name through QMI_NAS_CURRENT_PLMN_NAME_IND_V01. In this case, SPN
Name will be preferred over PLMN Name.
==**/
test_result ril_utf_test_operator_name_testcase_23( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.radio.sglte_csfb", "true");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");
  db_property_set("persist.vendor.radio.prefer_spn", "1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_update_cache_current_plmn_ind(t, QMI_node, RIL_node, TRUE, 123, 45, FALSE, TRUE, (nas_coding_scheme_enum_v01)0, 8, "spn Name", TRUE, (nas_coding_scheme_enum_v01)0, 15, "plmn Short Name", TRUE, (nas_coding_scheme_enum_v01)0, 14, "plmn Long Name");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", 0, 0, (nas_radio_if_enum_v01)0, "spn Name", "spn Name", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_24
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op PLMN Name and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_24( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_25
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op PLMN Name and got only Long PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_25( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_26
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op PLMN Name and got both Long and short PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_26( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Long PLMN Name", "Short PLMN Name", FALSE, FALSE, FALSE, TRUE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_27
@description: This test is to verify the "Operator_Name" when Device is in LTE network and
CSG_ID info is available. Also, property "persist.vendor.radio.csg_info_avlbl" is set.
Also, RIL Query modem for EONS op SE Table and got both Long and short PLMN name.
==**/
test_result ril_utf_test_operator_name_testcase_27( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", NULL, NULL, TRUE, 22, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", TRUE, 22, NAS_RADIO_IF_LTE_V01, "Long PLMN Name", "Short PLMN Name", FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_28
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on EONS Domain i.e. CS domain.
RIL will query for GSM RAT.
==**/
test_result ril_utf_test_operator_name_testcase_28( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","false");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, "GSM Long PLMN Name", "GSM Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_29
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on EONS Domain i.e. PS domain.
RIL will query for LTE RAT.
==**/
test_result ril_utf_test_operator_name_testcase_29( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","ps");
  db_property_set("persist.vendor.radio.sglte.eons_roam","false");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "456", "78", FALSE, 0, NAS_RADIO_IF_LTE_V01, "LTE Long PLMN Name", "LTE Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_30
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since LTE
is in Roaming, so RIL will query for LTE RAT.
==**/
test_result ril_utf_test_operator_name_testcase_30( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","ps");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "456", "78", FALSE, 0, NAS_RADIO_IF_LTE_V01, "LTE Long PLMN Name", "LTE Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_31
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT. Although EONS_DOMAIN is PS.
==**/
test_result ril_utf_test_operator_name_testcase_31( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","ps");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, "GSM Long PLMN Name", "GSM Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_32
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since LTE
is in Roaming, so RIL will query for LTE RAT. Although EONS_DOMAIN is CS.
==**/
test_result ril_utf_test_operator_name_testcase_32( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "456", "78", FALSE, 0, NAS_RADIO_IF_LTE_V01, "LTE Long PLMN Name", "LTE Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_33
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT.
==**/
test_result ril_utf_test_operator_name_testcase_33( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, "GSM Long PLMN Name", "GSM Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_34
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT. But RIL got only Short PLMN Name
and property "persist.vendor.radio.fill_eons" is NOT set.
==**/
test_result ril_utf_test_operator_name_testcase_34( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");
  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, NULL, "GSM Short PLMN Name", FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_35
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT. But RIL got only Short PLMN Name
and property "persist.vendor.radio.fill_eons" is set.
==**/
test_result ril_utf_test_operator_name_testcase_35( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");
  db_property_set("persist.vendor.radio.fill_eons", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, NULL, "GSM Short PLMN Name", TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_36
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT. But RIL got only Long PLMN Name
and property "persist.vendor.radio.fill_eons" is NOT set.
==**/
test_result ril_utf_test_operator_name_testcase_36( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");
  db_property_set("persist.vendor.radio.fill_eons", "0");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, "GSM Long PLMN Name", NULL, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_37
@description: This test is to verify the "Operator_Name" in case of SGLTE.
In this case, we will send Operator Name based on DOMAIN ROAM. Since GSM
is in Roaming, so RIL will query for GSM RAT. But RIL got only Long PLMN Name
and property "persist.vendor.radio.fill_eons" is set.
==**/
test_result ril_utf_test_operator_name_testcase_37( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","sglte");
  db_property_set("persist.vendor.radio.sglte.eons_domain","cs");
  db_property_set("persist.vendor.radio.sglte.eons_roam","true");
  db_property_set("persist.vendor.radio.fill_eons", "1");

  util_voice_registration_state_in_sglte_case(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_PS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "123", "45", TRUE, "456", "78");

  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0, NAS_RADIO_IF_GSM_V01, "GSM Long PLMN Name", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_1
@description: This test is to verify the Signal Strength when UMTS is in Home Network
==**/
test_result ril_utf_test_signal_strength_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_2
@description: This test is to verify the Signal Strength when UMTS is in Roaming Network
==**/
test_result ril_utf_test_signal_strength_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -112,2, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_3
@description: This test is to verify the Signal Strength when UMTS is in Limited Network,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -114,2, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_4
@description: This test is to verify the Signal Strength when UMTS is in Limited Service,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -116,2, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_5
@description: This test is to verify the Signal Strength when UMTS is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -118,2, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_6
@description: This test is to verify the Signal Strength when UMTS is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -120,2, TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_7
@description: This test is to verify the Signal Strength when GSM is in Home Network
==**/
test_result ril_utf_test_signal_strength_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -110, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_8
@description: This test is to verify the Signal Strength when GSM is in Roaming Network
==**/
test_result ril_utf_test_signal_strength_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -112, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_9
@description: This test is to verify the Signal Strength when GSM is in Limited Network,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_9( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -114, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_10
@description: This test is to verify the Signal Strength when GSM is in Limited Service,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_10( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -116, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_11
@description: This test is to verify the Signal Strength when GSM is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_11( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -118, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_12
@description: This test is to verify the Signal Strength when GSM is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_12( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL);

  util_ril_utf_gsm_signal_strength(t, RIL_node, QMI_node, -120, TRUE, FALSE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_13
@description: This test is to verify the Signal Strength when TDSCDMA is in Home Network
==**/
test_result ril_utf_test_signal_strength_testcase_13( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -110, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_14
@description: This test is to verify the Signal Strength when TDSCDMA is in Roaming Network
==**/
test_result ril_utf_test_signal_strength_testcase_14( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_TD_SCDMA, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -112, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_15
@description: This test is to verify the Signal Strength when TDSCDMA is in Limited Network,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_15( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -114, TRUE, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_16
@description: This test is to verify the Signal Strength when TDSCDMA is in Limited Service,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_16( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -116, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_17
@description: This test is to verify the Signal Strength when TDSCDMA is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_17( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -118, FALSE, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_18
@description: This test is to verify the Signal Strength when TDSCDMA is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_18( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_tdscdma_success(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0);

  util_ril_utf_tdscdma_signal_strength(t, RIL_node, QMI_node, -120, TRUE, FALSE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_19
@description: This test is to verify the Signal Strength when CDMA is in Home Network
==**/
test_result ril_utf_test_signal_strength_testcase_19( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_ril_utf_cdma_signal_strength(t, RIL_node, QMI_node, -110, -10, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_20
@description: This test is to verify the Signal Strength when CDMA is in Roaming Network
==**/
test_result ril_utf_test_signal_strength_testcase_20( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_IS95A, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_ON_V01);

  util_ril_utf_cdma_signal_strength(t, RIL_node, QMI_node, -112, -12, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_21
@description: This test is to verify the Signal Strength when CDMA is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_21( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_ril_utf_cdma_signal_strength(t, RIL_node, QMI_node, -114, -14, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_22
@description: This test is to verify the Signal Strength when CDMA is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_22( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_cdma(t, QMI_node, RIL_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01);

  util_ril_utf_cdma_signal_strength(t, RIL_node, QMI_node, -116, -16, TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_23
@description: This test is to verify the Signal Strength when LTE is in Home Network
==**/
test_result ril_utf_test_signal_strength_testcase_23( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -110, -10, -20, 10, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_24
@description: This test is to verify the Signal Strength when LTE is in Roaming Network
==**/
test_result ril_utf_test_signal_strength_testcase_24( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_LTE, RIL_VAL_REG_REGISTERED_ROAMING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_ON_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -112, -12, -22, 12, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_25
@description: This test is to verify the Signal Strength when LTE is in Limited Network,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_25( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");
  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -114, -14, -24, 14, TRUE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_26
@description: This test is to verify the Signal Strength when LTE is in Limited Service,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_26( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_vrte_ltd", "1");
  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING_LTD_SRV, NAS_SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_STATUS_LIMITED_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -116, -16, -26, 16, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_27
@description: This test is to verify the Signal Strength when LTE is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to not set, so RIL not send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_27( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","0");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -118, -18, -28, 18, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_28
@description: This test is to verify the Signal Strength when LTE is in OOS,
but property "persist.vendor.radio.3gpp_drte_ltd" to set, so RIL will send UNSOL_SIGNAL_STRENGTH to ATeL.
==**/
test_result ril_utf_test_signal_strength_testcase_28( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Radio_Tech");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_STATUS_NO_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);

  util_ril_utf_lte_signal_strength(t, RIL_node, QMI_node, -120, -20, -30, 20, TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_29
@description: This test is to verify the Signal Strength when
Domain is Camped, and both property "persist.vendor.radio.ignore_dom_time"
and "persist.vendor.radio.3gpp_drte_ltd" are NOT set.
==**/
test_result ril_utf_test_signal_strength_testcase_29( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, FALSE, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_30
@description: This test is to verify the Signal Strength when
Domain is Camped, and property "persist.vendor.radio.ignore_dom_time"
is NOT set and property "persist.vendor.radio.3gpp_drte_ltd" is set.
==**/
test_result ril_utf_test_signal_strength_testcase_30( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_signal_strength_testcase_31
@description: This test is to verify the Signal Strength when
Domain is Camped, during "ignore_dom_time" and after timer
expires.
==**/
test_result ril_utf_test_signal_strength_testcase_31( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.ignore_dom_time","1");
  db_property_set("persist.vendor.radio.3gpp_drte_ltd","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, TRUE, FALSE);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL, false);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, TRUE, FALSE);

  // Expect: RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
  RIL_node.update_default_ril_unsol_response_voice_network_state_changed(); // This UNSOL will get once "ignore_dom_time" expires.
  t.expect(RIL_node);

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UNKNOWN, RIL_VAL_REG_NOT_REGISTERED_SEARCHING, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_LIMITED_REGIONAL_V01, SYS_SRV_DOMAIN_CAMPED_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, FALSE, NULL, NULL, FALSE, 0, 0, NULL, false);

  util_ril_utf_wcdma_signal_strength(t, RIL_node, QMI_node, -110,2, TRUE, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_multisim_config_testcase_1
@description: This test is to verify whether the multisim property "persist.vendor.radio.multisim.config"
is changing dynamically or not. i.e. "from DSDS to DSDA" and vice_versa.
==**/
test_result ril_utf_test_multisim_config_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("MSIM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_ril_utf_set_multisim_prop ( t, RIL_node, QMI_node, 2, 1, 1, 2);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_uicc_subscription_testcase_1
@description: This test is to verify SET_UICC_SUBSCRIPTION request
is to return success without doing anything, if property "ro.baseband"is set to DSDA2.
==**/
test_result ril_utf_test_set_uicc_subscription_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("MSIM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","dsda2");

  util_ril_utf_set_uicc_subs ( t, RIL_node, QMI_node, 1, 0, RIL_UICC_SUBSCRIPTION_ACTIVATE, RIL_SUBSCRIPTION_1, TRUE, FALSE, RIL_E_SUCCESS, (qmi_result_type_v01)0, (qmi_error_type_v01)0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_uicc_subscription_testcase_2
@description: This test is to verify SET_UICC_SUBSCRIPTION request
is to return Failure, if property "ro.baseband"is Not set to DSDA2 and RIL got
Failure response for request GET_MODE_PREF_REQ.
==**/
test_result ril_utf_test_set_uicc_subscription_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("MSIM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","dsda");

  util_ril_utf_set_uicc_subs ( t, RIL_node, QMI_node, 1, 0, RIL_UICC_SUBSCRIPTION_ACTIVATE, RIL_SUBSCRIPTION_1, FALSE, FALSE, RIL_E_GENERIC_FAILURE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_uicc_subscription_testcase_3
@description: This test is to verify SET_UICC_SUBSCRIPTION request
is to return Success, if property "ro.baseband"is Not set to DSDA2 and RIL got
Success response for request GET_MODE_PREF_REQ.
==**/
test_result ril_utf_test_set_uicc_subscription_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("MSIM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("ro.baseband","dsda");

  util_ril_utf_set_multisim_prop ( t, RIL_node, QMI_node, 2, 1, 1, 2);
  util_ril_utf_set_uicc_subs ( t, RIL_node, QMI_node, 1, 0, RIL_UICC_SUBSCRIPTION_ACTIVATE, RIL_SUBSCRIPTION_1, FALSE, TRUE, RIL_E_SUCCESS, QMI_RESULT_SUCCESS_V01, (qmi_error_type_v01)0);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_1
@description: This test is to verify IMS_REGISTRATION_STATE should return Failure
with Empty Payload,if Transport Network registration status is not valid.
==**/
test_result ril_utf_test_ims_registration_state_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
#ifdef QMI_RIL_HAL_UTF
  int atel_status = RIL_E_MODEM_ERR;
#else
  int atel_status = RIL_E_SUCCESS;
#endif
  util_sms_over_ims_reg_state(t, RIL_node, QMI_node, FALSE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, FALSE, WMS_TRANSPORT_NW_REG_STATUS_FAILED_V01, FALSE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, FALSE, FALSE, FALSE, (wms_transport_type_enum_v01)0, (wms_transport_capability_enum_v01)0, TRUE, FALSE, RADIO_TECH_3GPP, atel_status);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_2
@description: This test is to verify IMS_REGISTRATION_STATE should return Success
with Default response Payload,if Transport Network registration status is valid,
but Transport Network registration status is NOT FULL Service .
==**/
test_result ril_utf_test_ims_registration_state_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node, FALSE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_IN_PROGRESS_V01, FALSE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, FALSE, FALSE, FALSE, (wms_transport_type_enum_v01)0, (wms_transport_capability_enum_v01)0, FALSE, FALSE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_3
@description: This test is to verify IMS_REGISTRATION_STATE should return Failure
with Empty Payload,if Transport Network registration status is valid and
Transport Network registration status is FULL Service, but got Failure Response
for TRANSPORT_LAYER_INFO_REQ from modem.
==**/
test_result ril_utf_test_ims_registration_state_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

#ifdef QMI_RIL_HAL_UTF
  int atel_status = RIL_E_MODEM_ERR;
#else
  int atel_status = RIL_E_SUCCESS;
#endif

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node, FALSE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, FALSE, FALSE, FALSE, (wms_transport_type_enum_v01)0, (wms_transport_capability_enum_v01)0, TRUE, FALSE, RADIO_TECH_3GPP, atel_status);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_4
@description: This test is to verify IMS_REGISTRATION_STATE should return Failure
with Empty Payload,if RIL got Failure Response for TRANSPORT_NW_REG_INFO_REQ from modem.
==**/
test_result ril_utf_test_ims_registration_state_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

#ifdef QMI_RIL_HAL_UTF
  int atel_status = RIL_E_MODEM_ERR;
#else
  int atel_status = RIL_E_SUCCESS;
#endif

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node, FALSE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, FALSE, WMS_TRANSPORT_NW_REG_STATUS_FAILED_V01, FALSE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, FALSE, FALSE, FALSE, (wms_transport_type_enum_v01)0, (wms_transport_capability_enum_v01)0, TRUE, FALSE, RADIO_TECH_3GPP, atel_status);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_5
@description: This test is to verify IMS_REGISTRATION_STATE should return Success
with Default Response Payload,if RIL got FULL service in TRANSPORT_NW_REG_INFO_REQ
from modem. But, RIL got IMS NOT Registered in GET_TRANSPORT_LAYER_INFO_REQ.
==**/
test_result ril_utf_test_ims_registration_state_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, 0, FALSE, (wms_transport_type_enum_v01)0, (wms_transport_capability_enum_v01)0, FALSE, FALSE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_6
@description: This test is to verify IMS_REGISTRATION_STATE should return Success
and mention that IMS got registered over 3GPP, if RIL got FULL service in TRANSPORT_NW_REG_INFO_REQ
from modem and also RIL got IMS Registered having GW Capability in GET_TRANSPORT_LAYER_INFO_REQ.
==**/
test_result ril_utf_test_ims_registration_state_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_GW_V01, FALSE, TRUE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_registration_state_testcase_7
@description: This test is to verify IMS_REGISTRATION_STATE should return Success
and mention that IMS got registered over 3GPP2, if RIL got FULL service in TRANSPORT_NW_REG_INFO_REQ
from modem and also RIL got IMS Registered having CDMA Capability in GET_TRANSPORT_LAYER_INFO_REQ.
==**/
test_result ril_utf_test_ims_registration_state_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_CDMA_V01, FALSE, TRUE, RADIO_TECH_3GPP2, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_1
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP
and WMS service is ready over 3GPP2. Although WMS service is not available
on 3GPP but still RIL should process SMS over IMS since IMS is registered.
==**/
test_result ril_utf_test_ims_send_sms_testcase_1( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP2_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_GW_V01, FALSE, TRUE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SUCCESS, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, 0x01, FALSE, (wms_message_delivery_failure_type_enum_v01)0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_2
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP
and WMS service is ready over 3GPP2. Although WMS service is not available
on 3GPP but still RIL should process SMS over IMS since IMS is registered.
But RIL got PERMANENT Delivery Failure, so RIL will send Generic Failure
to Telephony and Telephony should NOT Retry.
==**/
test_result ril_utf_test_ims_send_sms_testcase_2( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP2_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_GW_V01, FALSE, TRUE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_MODEM_ERR, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_PERMANENT_V01, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_3
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP
and WMS service is ready over 3GPP2. Although WMS service is not available
on 3GPP but still RIL should process SMS over IMS since IMS is registered.
But RIL got Temporary Delivery Failure, so RIL will send RETRY Failure
to Telephony and Telephony should Retry. Since, it is a retry, so RIL will
check for WMS service availability over IMS Tech and return Failure since
WMS is not available on 3GPP Tech.
==**/
test_result ril_utf_test_ims_send_sms_testcase_3( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP2_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_GW_V01, FALSE, TRUE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SMS_SEND_FAIL_RETRY, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_TEMPORARY_V01, TRUE);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, TRUE, 0x01, FALSE, RIL_E_INVALID_STATE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, 0, 0, (wms_message_delivery_failure_type_enum_v01)0, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_4
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP
and WMS service is ready over 3GPP. First SMS over IMS request, RIL
should process over IMS. And RIL got Temporary Delivery Failure,
so RIL will send RETRY Failure to Telephony and Telephony should Retry.
==**/
test_result ril_utf_test_ims_send_sms_testcase_4( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_GW_V01, FALSE, TRUE, RADIO_TECH_3GPP, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SMS_SEND_FAIL_RETRY, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_TEMPORARY_V01, TRUE);

  util_sms_over_ims_send_sms_3gpp(t, RIL_node, QMI_node, TRUE, 0x01, FALSE, RIL_E_SUCCESS, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, 0x01, FALSE, (wms_message_delivery_failure_type_enum_v01)0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_5
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP2
and WMS service is ready over 3GPP. Although WMS service is not available
on 3GPP2 but still RIL should process SMS over IMS since IMS is registered.
==**/
test_result ril_utf_test_ims_send_sms_testcase_5( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_CDMA_V01, FALSE, TRUE, RADIO_TECH_3GPP2, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SUCCESS, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, 0x01, FALSE, (wms_message_delivery_failure_type_enum_v01)0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_6
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP2
and WMS service is ready over 3GPP. Although WMS service is not available
on 3GPP2 but still RIL should process SMS over IMS since IMS is registered.
But RIL got PERMANENT Delivery Failure, so RIL will send Generic Failure
to Telephony and Telephony should NOT Retry.
==**/
test_result ril_utf_test_ims_send_sms_testcase_6( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_CDMA_V01, FALSE, TRUE, RADIO_TECH_3GPP2, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_MODEM_ERR, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_PERMANENT_V01, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_7
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP2
and WMS service is ready over 3GPP. Although WMS service is not available
on 3GPP2 but still RIL should process SMS over IMS since IMS is registered.
But RIL got Temporary Delivery Failure, so RIL will send RETRY Failure
to Telephony and Telephony should Retry. Since, it is a retry, so RIL will
check for WMS service availability over IMS Tech and return Failure since
WMS is not available on 3GPP2 Tech.
==**/
test_result ril_utf_test_ims_send_sms_testcase_7( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_CDMA_V01, FALSE, TRUE, RADIO_TECH_3GPP2, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SMS_SEND_FAIL_RETRY, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_TEMPORARY_V01, TRUE);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, TRUE, 0x01, FALSE, RIL_E_INVALID_STATE, (qmi_result_type_v01)0, (qmi_error_type_v01)0, 0, 0, (wms_message_delivery_failure_type_enum_v01)0, FALSE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_ims_send_sms_testcase_8
@description: This test is to verify IMS_SEND_SMS when IMS_tech is 3GPP2
and WMS service is ready over 3GPP2. First SMS over IMS request, RIL should
process over IMS. And if RIL got Temporary Delivery Failure, so RIL will
send RETRY Failure to Telephony and Telephony should Retry.
==**/
test_result ril_utf_test_ims_send_sms_testcase_8( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("WMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_wms_service_ready(t, RIL_node, OEM_node, QMI_node, WMS_SERVICE_READY_STATUS_3GPP2_V01);

  util_sms_over_ims_reg_state(t, RIL_node, QMI_node,TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, WMS_TRANSPORT_NW_REG_STATUS_FULL_SERVICE_V01, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, TRUE, TRUE, TRUE, WMS_TRANSPORT_TYPE_IMS_V01, WMS_TRANSPORT_CAP_CDMA_V01, FALSE, TRUE, RADIO_TECH_3GPP2, RIL_E_SUCCESS);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, FALSE, 0x00, TRUE, RIL_E_SMS_SEND_FAIL_RETRY, QMI_RESULT_FAILURE_V01, QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01, 0x01, TRUE, WMS_MESSAGE_DELIVERY_FAILURE_TEMPORARY_V01, TRUE);

  util_sms_over_ims_send_sms_3gpp2(t, RIL_node, QMI_node, TRUE, 0x01, FALSE, RIL_E_SUCCESS, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, 0x01, FALSE, (wms_message_delivery_failure_type_enum_v01)0, TRUE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_lte_tune_away_testcase_1
@description: Test case to set lte tune away
==**/
test_result ril_utf_test_set_lte_tune_away_testcase_1(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_tuneaway_0", "0");

  /* utility function to set lte tune away */
  util_ril_utf_test_set_lte_tune_away(t, OEM_node, QMI_node, 1, TRUE, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, RIL_E_GENERIC_FAILURE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_lte_tune_away_testcase_2
@description: Test case to set lte tune away
==**/
test_result ril_utf_test_set_lte_tune_away_testcase_2(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_tuneaway_0", "0");

  /* utility function to set lte tune away */
  util_ril_utf_test_set_lte_tune_away(t, OEM_node, QMI_node, 1, TRUE, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_lte_tune_away_testcase_3
@description: Test case to set lte tune away
==**/
test_result ril_utf_test_set_lte_tune_away_testcase_3(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_tuneaway_0", "0");

  /* utility function to set lte tune away */
  util_ril_utf_test_set_lte_tune_away(t, OEM_node, QMI_node, 2, FALSE, 0, 0, RIL_E_GENERIC_FAILURE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_lte_tune_away_testcase_4
@description: Test case to set lte tune away
==**/
test_result ril_utf_test_set_lte_tune_away_testcase_4(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEM");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.lte_tuneaway_0", "1");

  /* utility function to set lte tune away */
  util_ril_utf_test_set_lte_tune_away(t, OEM_node, QMI_node, 1, FALSE, 0, 0, RIL_E_SUCCESS);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_detach_then_attach
@description: Test case to initialize lte network and simulate the scenario where APN is detached and then attached
==**/
test_result ril_utf_test_lte_apn_detach_then_attach(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_no_reattach_needed
@description: Test case to simulate the sceanrio that no apn detach/attach is needed because of lte not ready
==**/
test_result ril_utf_test_lte_apn_no_reattach_needed(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_NO_SRV_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_detach_error
@description: Test case to simulate the sceanrio that apn detatch fails
==**/
test_result ril_utf_test_lte_apn_detach_error (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_attach_error
@description: Test case to simulate the sceanrio that apn attatch fails
==**/
test_result ril_utf_test_lte_apn_attach_error (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_detach_no_service_domain
@description: Test case to initialize lte network in PS domain and simulate the scenario where APN
              is detached but no service domain is defined
==**/
test_result ril_utf_test_lte_apn_lte_ps_mode_detach_no_service_domain(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_PS_ONLY_V01);
  // Detatch APN without service domain indication
  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_DETACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENE_IND
  // DO not set sercive domain here
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  t.inject(QMI_node);

  // Attach APN with PS only srv domain
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_PS_ONLY_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_detach_two_indications
@description: Test case to initialize lte network and simulate the scenario where APN detach has
              two indications, first one is CS_PS mode and second one is CS mode
==**/
test_result ril_utf_test_lte_apn_detach_two_indications(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  // Detach with two indications
  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_DETACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENE_IND
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_srv_domain_pref(QMI_SRV_DOMAIN_PREF_CS_PS_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENE_IND
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_srv_domain_pref(QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  t.inject(QMI_node);

  // Attach
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_multiple_set_init_attach_apn_requests
@description: Test case to initialize lte network and simulate the scenario where new init APN
              attach request is sent in the middle of the detach/attach process
==**/
test_result ril_utf_test_lte_apn_multiple_set_init_attach_apn_requests(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  RIL_node.set_token_id(6000);
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  t.set_start();

//  util_ril_utf_get_device_cap_info(t, QMI_node, RIL_node, OEM_node, 1,1, TRUE);

  // Detach
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);

  t.set_end();

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelesstwo.com", "IP");
  RIL_node.set_token_id(6001);
  t.inject(RIL_node);

  // Attach
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // First RIL request finished
  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  RIL_node.set_token_id(6000);
  t.expect(RIL_node);

  // Start handling second RIL request
  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // Second RIL request finished
  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  RIL_node.set_token_id(6001);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_detach_no_indication
@description: Test case to simulate the sceanrio that apn detatch doesn't return an indicaiton
==**/
test_result ril_utf_test_lte_apn_detach_no_indication (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_DETACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Timeout
  t.sleep(80000);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_attach_no_indication
@description: Test case to simulate the sceanrio that apn attatch doesn't return an indicaiton
==**/
test_result ril_utf_test_lte_apn_attach_no_indication (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  // Detach
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);

  // Attach
  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Timeout
  t.sleep(80000);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}


/**==
@testname: ril_utf_test_lte_apn_detach_delayed_indication
@description: Test case to simulate the sceanrio that apn detatch successful indication is delayed
==**/
test_result ril_utf_test_lte_apn_detach_delayed_indication (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_DETACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Timeout
  t.sleep(80000);

  // Delayed indication
  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENE_IND
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_srv_domain_pref(QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_lte_apn_attach_delayed_indication
@description: Test case to simulate the sceanrio that apn atatch successful indication is delayed
==**/
test_result ril_utf_test_lte_apn_attach_delayed_indication (user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("wirelessone.com", "IP");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);

  // Detach
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);

  // Attach
  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_REQ
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_srv_domain_pref(QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENE_RESP
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Timeout
  t.sleep(80000);

  // Delayed indication
  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENE_IND
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_srv_domain_pref(QMI_SRV_DOMAIN_PREF_CS_PS_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}


/**==
@testname: ril_utf_test_lte_apn_empty_apn_name_gerrit_1075551
@description: Test case to initialize lte network and simulate the scenario where APN is detached and then attached
==**/
test_result ril_utf_test_lte_apn_empty_apn_name_gerrit_1075551(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("lte_apn_init_attach");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_ONLY, RADIO_TECH_LTE, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  // Inject: RIL_REQUEST_SET_INITIAL_ATTACH_APN_REQ
  RIL_node.update_default_ril_request_set_initial_attach_apn();
  RIL_node.update_ril_request_set_initial_attach_apn_address_and_protocol("", "");
  t.inject(RIL_node);

  util_ril_utf_get_sys_info_for_lte_attached(t, QMI_node, SYS_SRV_DOMAIN_CS_PS_V01);
  util_ril_utf_detach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_ONLY_V01);
  util_ril_utf_attach_apn(t, QMI_node, QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01, QMI_SRV_DOMAIN_PREF_CS_PS_V01);

  // Expect: RIL_REQUEST_SET_INITIAL_ATTACH_APN_RESP
  RIL_node.update_default_ril_request_set_initial_attach_apn_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

#ifdef RIL_UTF_L_MASTER
/**==
@testname: ril_utf_test_mbn_success
@description: This test case simulates the basic successful scenario for hw/sw mbn loading
==**/
test_result ril_utf_test_mbn_msim_success(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.clear_db_before_execution();
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_success_with_iccid_only
@description: This test case simulates the basic successful scenario for hw/sw mbn loading, but retrieve mbn from iccid since mcc/mnc is absent
==**/
test_result ril_utf_test_mbn_msim_success_with_iccid_only(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("8900310","null","null",0);
  t.set_mbn_sim_info("8900310","null","null",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_no_sim_001
@description: verify boot up with no sim and no hw mbn init needed
==**/
test_result ril_utf_test_mbn_no_sim_without_hw_mbn_init(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("absent","","",0);
  t.set_mbn_sim_info("absent","","",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_ril_utf_init_no_sim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, DEFAULT_BOOT, FALSE, FALSE);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, DEFAULT_BOOT, FALSE, FALSE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_no_sim_002
@description: verify boot up with no sim and hw mbn init is needed
==**/
test_result ril_utf_test_mbn_no_sim_with_hw_mbn_init(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();
  t.set_mbn_sim_info("absent","","",0);
  t.set_mbn_sim_info("absent","","",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_ril_utf_init_no_sim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, FALSE, FALSE);

  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, FALSE, FALSE);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_one_sim_001
@description:
==**/
test_result ril_utf_test_mbn_one_sim_001(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  db_property_set("persist.vendor.radio.stack_id_0", "0");
  db_property_set("persist.vendor.radio.hw_mbn_update", "1");
  db_property_set("persist.vendor.radio.sw_mbn_update", "1");
  db_property_set("persist.vendor.radio.hw_mbn_loaded", "1");
  db_property_set("persist.vendor.radio.sw_mbn_loaded", "1");

  ril_utf_bootup_helper(t, PDC_BOOT);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
/**==
@testname: ril_utf_test_mbn_msim_with_card_0_active_only
@description:
==**/
test_result ril_utf_test_mbn_msim_with_only_card_0_active(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("8900310","480","480",0);
  t.set_mbn_sim_info("absent","","",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.radio.multisim.config", "dsds");
  property_set("ro.baseband","dsda");
  db_property_set("persist.vendor.radio.stack_id_0", "0");
  db_property_set("persist.vendor.radio.hw_mbn_update", "1");
  db_property_set("persist.vendor.radio.sw_mbn_update", "1");
  db_property_set("persist.vendor.radio.hw_mbn_loaded", "1");
  db_property_set("persist.vendor.radio.sw_mbn_loaded", "1");

  //ril_utf_msim_with_only_card_0_active_bootup_helper_1(t, PDC_BOOT);
  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, FALSE);

  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  util_ril_utf_init_sw_mbn(t, 0);

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, (const uint8_t*)"pending_config_id_0", PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);
  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0, 1);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);
  //ril_utf_msim_with_only_card_0_active_bootup_helper_2(t);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_with_card_1_active_only
@description:
==**/
test_result ril_utf_test_mbn_msim_with_only_card_1_active(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("absent","","",0);
  t.set_mbn_sim_info("8900310","480","480",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  property_set("persist.vendor.radio.multisim.config", "dsds");
  property_set("ro.baseband","dsda");
  db_property_set("persist.vendor.radio.iccid_1", "89003100085865131146");
  db_property_set("persist.vendor.radio.stack_id_1", "1");
  db_property_set("persist.vendor.radio.hw_mbn_update", "1");
  db_property_set("persist.vendor.radio.sw_mbn_update", "1");
  db_property_set("persist.vendor.radio.hw_mbn_loaded", "1");
  db_property_set("persist.vendor.radio.sw_mbn_loaded", "1");

  //ril_utf_msim_with_card_1_only_bootup_helper_1(t, DEFAULT_BOOT);
  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, FALSE, TRUE);

  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, FALSE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_hw_get_config_fail
@description: hw get selected config fail, continue with load config
==**/
test_result ril_utf_test_mbn_hw_get_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  t.set_start();
  // Expect: QMI_PDC_GET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_get_selected_config_req();
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_get_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  const uint32_t hw_config_len_1 = 8724;

  util_ril_utf_qmi_pdc_load_config(t, (char *)hw_config_id_1, hw_config_id_1, hw_config_len_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, hw_config_id_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0, 1);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_hw_get_config_ind_with_no_config_id
@description: hw get selected config indication has no config_id, continue with load config
==**/
test_result ril_utf_test_mbn_hw_get_config_ind_with_no_config_id(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  t.set_start();
  // Expect: QMI_PDC_GET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_get_selected_config_req();
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_get_selected_config_resp();
  t.inject(QMI_node);

  // Inject: QMI_PDC_GET_SELECTED_CONFIG_IND
  QMI_node.update_default_qmi_pdc_get_selected_config_ind();
  t.inject(QMI_node);
  t.set_end();

  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  const uint32_t hw_config_len_1 = 8724;

  util_ril_utf_qmi_pdc_load_config(t, (char *)hw_config_id_1, hw_config_id_1, hw_config_len_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, hw_config_id_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0, 1);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_hw_deactivate_config_fail
@description: hw deactivate config fail, continue with sw mbn init
==**/
test_result ril_utf_test_mbn_hw_deactivate_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  uint8_t *hw_config_id_0 = (uint8_t *)"dummy/config/id";
  util_ril_utf_qmi_pdc_get_selected_config(t, hw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_DEACTIVATE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_deactivate_config_req();
  t.expect(QMI_node);

  // Inject: QMI_PDC_DEACTIVATE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_deactivate_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_hw_delete_config_fail
@description: hw delete config fail, continue with load config
==**/
test_result ril_utf_test_mbn_hw_delete_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  uint8_t *hw_config_id_0 = (uint8_t *)"dummy/config/id";
  uint32_t hw_config_id_len_0 = strlen((char *)hw_config_id_0);

  util_ril_utf_qmi_pdc_get_selected_config(t, hw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  t.set_start();
  QMI_node.update_default_qmi_pdc_delete_config_req();
  QMI_node.set_qmi_pdc_delete_config_req_config_id(hw_config_id_0, hw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_DELETE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_delete_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  const uint32_t hw_config_len_1 = 8724;

  util_ril_utf_qmi_pdc_load_config(t, (char *)hw_config_id_1, hw_config_id_1, hw_config_len_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, hw_config_id_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0, 1);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_hw_load_config_fail
description: hw load config fail, continue with sw mbn init
==**/
test_result ril_utf_test_mbn_hw_load_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  uint8_t *hw_config_id_0 = (uint8_t *)"dummy/config/id";

  util_ril_utf_qmi_pdc_get_selected_config(t, hw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, hw_config_id_0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);


  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  uint32_t hw_config_id_len_1 = strlen((char *)hw_config_id_1);
  const uint32_t hw_config_len_1 = 8724;
  const uint32_t pdc_frame_size = 900;
  FILE *mbn_file_ptr;
  uint8_t config_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  int config_frame_len = 0;
  int read_success = 0;
  mbn_file_ptr = fopen((char *)hw_config_id_1, "rb");

  t.set_start();
  // Expect: QMI_PDC_LOAD_CONFIG_REQ
  memset(config_frame, 0, sizeof(config_frame));
  fseek(mbn_file_ptr, 0, SEEK_SET);
  read_success = fread(&config_frame, pdc_frame_size, 1, mbn_file_ptr);
  if (!read_success) // Reached EoF
  {
    config_frame_len = hw_config_len_1;
  }
  else
  {
    config_frame_len = pdc_frame_size;
  }
  QMI_node.update_default_qmi_pdc_load_config_req();
  QMI_node.set_qmi_pdc_load_config_req_config_id(hw_config_id_1, hw_config_id_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_size(hw_config_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_frame(config_frame, config_frame_len);
  t.expect(QMI_node);

  // Inject: QMI_PDC_LOAD_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_load_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();
  fclose(mbn_file_ptr);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
testname: ril_utf_test_mbn_hw_set_config_fail
description: hw set config fail, continue with sw mbn init
==**/
test_result ril_utf_test_mbn_hw_set_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  uint8_t *hw_config_id_0 = (uint8_t *)"dummy/config/id";

  util_ril_utf_qmi_pdc_get_selected_config(t, hw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, hw_config_id_0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  uint32_t hw_config_id_len_1 = strlen((char *)hw_config_id_1);
  const uint32_t hw_config_len_1 = 8724;

  util_ril_utf_qmi_pdc_load_config(t, (char *)hw_config_id_1, hw_config_id_1, hw_config_len_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  t.set_start();
  // Expect: QMI_PDC_SET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_set_selected_config_req();
  QMI_node.set_qmi_pdc_set_selected_config_req_config_id(hw_config_id_1, hw_config_id_len_1);
  t.expect(QMI_node);

  // Inject: QMI_PDC_SET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_set_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
testname: ril_utf_test_mbn_hw_activate_config_fail
description: hw activate config fail, continue with sw mbn init
==**/
test_result ril_utf_test_mbn_hw_activate_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  // HW MBN
  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  uint8_t *hw_config_id_0 = (uint8_t *)"dummy/config/id";

  util_ril_utf_qmi_pdc_get_selected_config(t, hw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, hw_config_id_0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  uint8_t *hw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/MSM8994/LA_DS/mcfg_hw.mbn";
  const uint32_t hw_config_len_1 = 8724;
  util_ril_utf_qmi_pdc_load_config(t, (char *)hw_config_id_1, hw_config_id_1, hw_config_len_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, hw_config_id_1, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_ACTIVATE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_activate_config_req();
  t.expect(QMI_node);

  // Inject: QMI_PDC_ACTIVATE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_activate_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_get_config_fail
@description: 1st sw get selected config fail, continue with delete config
==**/
test_result ril_utf_test_mbn_1st_sw_get_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  t.set_start();
  // Expect: QMI_PDC_GET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_get_selected_config_req();
  QMI_node.set_qmi_pdc_get_selected_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_selected_config_req_subscription_id(0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_get_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_get_config_info_fail
@description: 1st sw get config info fail, continue with delete config
==**/
test_result ril_utf_test_mbn_1st_sw_get_config_info_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_GET_CONFIG_INFO_REQ
  QMI_node.update_default_qmi_pdc_get_config_info_req();
  QMI_node.set_qmi_pdc_get_config_info_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_config_info_req_config_id(sw_config_id_0, sw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_RESP
  QMI_node.update_default_qmi_pdc_get_config_info_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_version
@description: 1st sw get config info ind returns different version, continue with deactivate config
==**/
test_result ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_version(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_GET_CONFIG_INFO_REQ
  QMI_node.update_default_qmi_pdc_get_config_info_req();
  QMI_node.set_qmi_pdc_get_config_info_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_config_info_req_config_id(sw_config_id_0, sw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_RESP
  QMI_node.update_default_qmi_pdc_get_config_info_resp();
  t.inject(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_IND
  pdc_get_config_info_ind_msg_v01* ptr = QMI_node.update_default_qmi_pdc_get_config_info_ind();
  ptr->config_version_valid = 1;
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_minor_version
@description: 1st sw get config info ind returns same major version but different minor version, continue with deactivate config
==**/
test_result ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_minor_version(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_GET_CONFIG_INFO_REQ
  QMI_node.update_default_qmi_pdc_get_config_info_req();
  QMI_node.set_qmi_pdc_get_config_info_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_config_info_req_config_id(sw_config_id_0, sw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_RESP
  QMI_node.update_default_qmi_pdc_get_config_info_resp();
  t.inject(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_IND
  pdc_get_config_info_ind_msg_v01* ptr = QMI_node.update_default_qmi_pdc_get_config_info_ind();
  ptr->config_version_valid = 1;
  ptr->config_version = 0x04011909;
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_same_version
@description: 1st sw get config info ind returns same version, continue with 2nd sw mbn init
==**/
test_result ril_utf_test_mbn_1st_sw_get_config_info_ind_with_different_same_version(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_GET_CONFIG_INFO_REQ
  QMI_node.update_default_qmi_pdc_get_config_info_req();
  QMI_node.set_qmi_pdc_get_config_info_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_config_info_req_config_id(sw_config_id_0, sw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_RESP
  QMI_node.update_default_qmi_pdc_get_config_info_resp();
  t.inject(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_IND
  pdc_get_config_info_ind_msg_v01* ptr = QMI_node.update_default_qmi_pdc_get_config_info_ind();
  ptr->config_version_valid = 1;
  ptr->config_version = 0x04011910;
  t.inject(QMI_node);
  t.set_end();

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_deactivate_config_fail
@description: 1st sw deactivatition fail, continue with 2nd sw mbn init
==**/
test_result ril_utf_test_mbn_1st_sw_deactivate_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_2 = 20664;
  uint8_t *sw_config_id_2 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  uint32_t sw_config_id_len_2 = strlen((char *)sw_config_id_2);
  char *sw_config_file_2 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t sw_config_id_3[100];

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_1, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_1);

  t.set_start();
  // Expect: QMI_PDC_DEACTIVATE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_deactivate_config_req();
  QMI_node.set_qmi_pdc_deactivate_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_deactivate_config_req_subscription_id(0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_DEACTIVATE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_deactivate_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);
  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);
  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_delete_config(t, (uint8_t *)sw_config_id_2, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_config_file_2, sw_config_id_2, sw_config_len_2, PDC_CONFIG_TYPE_MODEM_SW_V01);
  util_ril_utf_qmi_pdc_set_selected_config(t, (uint8_t *)sw_config_id_2, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);


  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_1, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_2, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1, 1);
  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);
  t.set_end();

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_2nd_delete_config_fail
@description: 1st sw 2nd delete config fail, continue with load config
==**/
test_result ril_utf_test_mbn_1st_sw_2nd_delete_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  t.set_start();
  // Expect: QMI_PDC_DELETE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_delete_config_req();
  QMI_node.set_qmi_pdc_delete_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_delete_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  t.expect(QMI_node);

  // Inject: QMI_PDC_DELETE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_delete_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_load_config_fail
@description: 1st sw load config fail, continue with 2nd sw mbn init
==**/
test_result ril_utf_test_mbn_1st_sw_load_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  //util_ril_utf_qmi_pdc_load_config(t, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);
  const uint32_t pdc_frame_size = 900;
  FILE *mbn_file_ptr;
  uint8_t config_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  int config_frame_len = 0;
  int read_success = 0;
  mbn_file_ptr = fopen(sw_file_1, "rb");

  t.set_start();
  // Expect: QMI_PDC_LOAD_CONFIG_REQ
  memset(config_frame, 0, sizeof(config_frame));
  fseek(mbn_file_ptr, 0, SEEK_SET);
  read_success = fread(&config_frame, pdc_frame_size, 1, mbn_file_ptr);
  if (!read_success) // Reached EoF
  {
    config_frame_len = sw_config_len_1;
  }
  else
  {
    config_frame_len = pdc_frame_size;
  }
  QMI_node.update_default_qmi_pdc_load_config_req();
  QMI_node.set_qmi_pdc_load_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_load_config_req_config_size(sw_config_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_frame(config_frame, config_frame_len);
  t.expect(QMI_node);

  // Inject: QMI_PDC_LOAD_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_load_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_1st_sw_set_config_fail
@description: 1st sw set config fail, continue with 2nd sw mbn init
==**/
test_result ril_utf_test_mbn_1st_sw_set_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn0";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  t.set_start();
  // Expect: QMI_PDC_SET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_set_selected_config_req();
  QMI_node.set_qmi_pdc_set_selected_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_set_selected_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_set_selected_config_req_subscription_id(0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_SET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_set_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_get_config_fail
@description: 2nd sw get selected config fail, continue with delete config
==**/
test_result ril_utf_test_mbn_2nd_sw_get_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;
  t.set_start();
  // Expect: QMI_PDC_GET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_get_selected_config_req();
  QMI_node.set_qmi_pdc_get_selected_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_selected_config_req_subscription_id(1);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_get_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1, 2);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_get_config_info_fail
@description: 2nd sw get config info fail, continue with delete config
==**/
test_result ril_utf_test_mbn_2nd_sw_get_config_info_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  t.set_start();
  // Expect: QMI_PDC_GET_CONFIG_INFO_REQ
  QMI_node.update_default_qmi_pdc_get_config_info_req();
  QMI_node.set_qmi_pdc_get_config_info_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_get_config_info_req_config_id(sw_config_id_0, sw_config_id_len_0);
  t.expect(QMI_node);

  // Inject: QMI_PDC_GET_CONFIG_INFO_RESP
  QMI_node.update_default_qmi_pdc_get_config_info_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1, 2);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_deactivate_config_fail
@description: 2nd sw deactivatie config fail, continue with activate mbn
==**/
test_result ril_utf_test_mbn_2nd_sw_deactivate_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint32_t sw_config_id_len_0 = strlen((char *)sw_config_id_0);
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  t.set_start();
  // Expect: QMI_PDC_DEACTIVATE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_deactivate_config_req();
  QMI_node.set_qmi_pdc_deactivate_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_deactivate_config_req_subscription_id(1);
  t.expect(QMI_node);

  // Inject: QMI_PDC_DEACTIVATE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_deactivate_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  t.set_end();

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_0, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0, 1);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_2nd_delete_config_fail
@description: 2nd sw 2nd delete config fail, continue with load config
==**/
test_result ril_utf_test_mbn_2nd_sw_2nd_delete_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  t.set_start();
  // Expect: QMI_PDC_DELETE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_delete_config_req();
  QMI_node.set_qmi_pdc_delete_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_delete_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  t.expect(QMI_node);

  // Inject: QMI_PDC_DELETE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_delete_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1, 2);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_load_config_fail
@description: 2nd sw load config fail, continue with activate mbn
==**/
test_result ril_utf_test_mbn_2nd_sw_load_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  //util_ril_utf_qmi_pdc_load_config(t, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);
  const uint32_t pdc_frame_size = 900;
  FILE *mbn_file_ptr;
  char config_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  int config_frame_len = 0;
  int read_success = 0;
  mbn_file_ptr = fopen(sw_file_1, "rb");

  t.set_start();
  // Expect: QMI_PDC_LOAD_CONFIG_REQ
  memset(config_frame, 0, sizeof(config_frame));
  fseek(mbn_file_ptr, 0, SEEK_SET);
  read_success = fread(&config_frame, pdc_frame_size, 1, mbn_file_ptr);
  if (!read_success) // Reached EoF
  {
    config_frame_len = sw_config_len_1;
  }
  else
  {
    config_frame_len = pdc_frame_size;
  }
  QMI_node.update_default_qmi_pdc_load_config_req();
  QMI_node.set_qmi_pdc_load_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_load_config_req_config_size(sw_config_len_1);
  QMI_node.set_qmi_pdc_load_config_req_config_frame((uint8_t *)config_frame, config_frame_len);
  t.expect(QMI_node);

  // Inject: QMI_PDC_LOAD_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_load_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  t.set_end();

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0, 1);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_set_config_fail
@description: 2nd sw set config fail, continue with activate mbn
==**/
test_result ril_utf_test_mbn_2nd_sw_set_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  t.set_start();
  // Expect: QMI_PDC_SET_SELECTED_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_set_selected_config_req();
  QMI_node.set_qmi_pdc_set_selected_config_req_config_id(sw_config_id_1, sw_config_id_len_1);
  QMI_node.set_qmi_pdc_set_selected_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.set_qmi_pdc_set_selected_config_req_subscription_id(1);
  t.expect(QMI_node);

  // Inject: QMI_PDC_SET_SELECTED_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_set_selected_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  t.set_end();

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_activate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0, 1);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_2nd_sw_activate_config_fail
@description: 2nd sw activate config fail, continue with remaining of bootup
==**/
test_result ril_utf_test_mbn_2nd_sw_activate_config_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  uint8_t *sw_config_id_0 = (uint8_t *)"data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn";
  uint8_t *sw_config_id_1 = (uint8_t *)"data/modem_config/mcfg_sw/generic/common/W_One/Commercial/mcfg_sw.mbn1";
  uint32_t sw_config_id_len_1 = strlen((char *)sw_config_id_1);
  const uint32_t sw_config_len_1 = 20664;

  util_ril_utf_qmi_pdc_get_selected_config(t, sw_config_id_0, NULL, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_config_info(t, sw_config_id_0);

  util_ril_utf_qmi_pdc_deactivate_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_delete_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_load_config(t, sw_file_1, sw_config_id_1, sw_config_len_1, PDC_CONFIG_TYPE_MODEM_SW_V01);

  util_ril_utf_qmi_pdc_set_selected_config(t, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);
  util_ril_utf_qmi_pdc_get_selected_config(t, NULL, sw_config_id_1, PDC_CONFIG_TYPE_MODEM_SW_V01, 1);

  t.set_start();
  // Expect: QMI_PDC_ACTIVATE_CONFIG_REQ
  QMI_node.update_default_qmi_pdc_activate_config_req();
  QMI_node.update_qmi_pdc_activate_config_req_config_type(PDC_CONFIG_TYPE_MODEM_SW_V01);
  QMI_node.update_qmi_pdc_activate_config_req_subscription_id(1);
  t.expect(QMI_node);

  // Inject: QMI_PDC_ACTIVATE_CONFIG_RESP
  QMI_node.update_default_qmi_pdc_activate_config_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_no_mbn_config
@description: This test case simulates the scenario for hw/sw mbn loading without mbn config folder
==**/
test_result ril_utf_test_mbn_no_mbn_config(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  char* cmd1 = "mv data/modem_config data/modem_config_1";
  system(cmd1);

  // Execute the test case
  t.execute();

  char* cmd2 = "mv data/modem_config_1 data/modem_config";
  system(cmd2);

  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_no_hw_mbn_file
@description: This test case simulates the scenario for hw/sw mbn loading without hw mbn config folder
==**/
test_result ril_utf_test_mbn_no_hw_mbn_file(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  char* cmd1 = "mv data/modem_config/mcfg_hw data/modem_config/mcfg_hw_1";
  system(cmd1);

  // Execute the test case
  t.execute();

  char* cmd2 = "mv data/modem_config/mcfg_hw_1 data/modem_config/mcfg_hw";
  system(cmd2);

  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_no_sw_mbn_file
@description: This test case simulates the scenario for hw/sw mbn loading without sw mbn config folder
==**/
test_result ril_utf_test_mbn_no_sw_mbn_file(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.reset_mbn_sim_info();

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  util_ril_utf_init_hw_mbn(t, 2);

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  util_ril_utf_qmi_pdc_cleanup_config(t, PDC_CONFIG_TYPE_MODEM_SW_V01, 0);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  char* cmd1 = "mv data/modem_config/mcfg_sw data/modem_config/mcfg_sw_1";
  system(cmd1);

  // Execute the test case
  t.execute();

  char* cmd2 = "mv data/modem_config/mcfg_sw_1 data/modem_config/mcfg_sw";
  system(cmd2);

  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_copy_not_complete
@description: This test case simulates the scenario where mbn file copy is not completed
==**/
test_result ril_utf_test_mbn_copy_not_complete(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  char* cmd1 = "cp data/vendor/misc/radio/copy_complete data/vendor/radio/copy_complete_1";
  char* cmd2 = "cp data/vendor/misc/radio/copy_not_complete data/vendor/radio/copy_complete";
  system(cmd1);
  system(cmd2);

  // Execute the test case
  t.execute();

  char* cmd3 = "mv data/vendor/misc/radio/copy_complete_1 data/vendor/radio/copy_complete";
  system(cmd3);

  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_11_digit_iin
@description: This test case simulates mbn loading for 11 digit IIN for Google NOVA
==**/
test_result ril_utf_test_mbn_nova_sim_with_11_digit_iin(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("89012609711","null","null",0);
  t.set_mbn_sim_info("89012609711","null","null",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn1";
  const uint32_t sw_config_len_1 = 26256;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_nova_sim_with_mcc_mnc
@description: This test case simulates mbn loading for mcc/mnc for Google NOVA
==**/
test_result ril_utf_test_mbn_nova_sim_with_mcc_mnc(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("89012609712","310","260",0);
  t.set_mbn_sim_info("89012609712","310","260",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(1);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/Nova/mcfg_sw.mbn1";
  const uint32_t sw_config_len_1 = 26256;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_wild
@description: This test case simulates the scenario that wild mbn should be selected
==**/
test_result ril_utf_test_mbn_msim_wild(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("1234567","null","null",0);
  t.set_mbn_sim_info("1234567","null","null",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/common/ROW/Gen_3GPP/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/generic/common/ROW/Gen_3GPP/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/generic/common/ROW/Gen_3GPP/mcfg_sw.mbn1";
  const uint32_t sw_config_len_1 = 22880;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_mcc_mnc_list
@description: This test case simulates the scenario that mbn is selected based on mcc mnc list
==**/
test_result ril_utf_test_mbn_msim_mcc_mnc_list(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("8901410","311","180",0);
  t.set_mbn_sim_info("8901410","310","980",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/generic/NA/ATT/VoLTE/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/generic/NA/ATT/VoLTE/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/NA/ATT/VoLTE/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 26812;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_capability_with_mcc_mnc
@description: This test case simulates the scenario that mbn is selected based on mcc mnc and capability
==**/
test_result ril_utf_test_mbn_msim_capability_with_mcc_mnc(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("000000","460","07",0);
  t.set_mbn_sim_info("000000","454","13",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 31876;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_capability_with_iccid
@description: This test case simulates the scenario that mbn is selected based on iccid and capability
==**/
test_result ril_utf_test_mbn_msim_capability_with_iccid(user_specified_attrib_t attr)
{
  Testcase t;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("898600","null","null",0);
  t.set_mbn_sim_info("898600","null","null",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char* sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char* sw_config_id_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn0";
  char* sw_config_id_2 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn1";
  char* sw_file_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 31876;

  util_ril_utf_qmi_pdc_disable_modem_update(t);

  // First RILD
  util_ril_utf_init_sw_mbn(t, 0, sw_config_id_0, sw_file_1, sw_config_id_1, sw_config_len_1);

  // Second RILD
  util_ril_utf_init_sw_mbn(t, 1, sw_config_id_0, sw_file_1, sw_config_id_2, sw_config_len_1);

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}

/**==
@testname: ril_utf_test_mbn_msim_disable_modem_update_fail
@description: This test case simulates the scenario that disabling modem mbn auto selection fails
@             No sw_mbn_update should happen
==**/
test_result ril_utf_test_mbn_msim_disable_modem_update_fail(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;

  t.set_test_id(__func__);
  t.set_test_description("PDC Test case");
  t.set_test_attribute("PDC");
  t.set_boot_case(qmi_ril_msim_dummy_bootup);
  t.set_mbn_sim_info("898600","460","0",0);
  t.set_mbn_sim_info("898600","454","13",1);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }
  util_ril_utf_init_msim_mbn_properties(0);

  ril_utf_msim_bootup_helper_1(t, PDC_BOOT, TRUE, TRUE);

  // HW_MBN
  util_ril_utf_init_hw_mbn(t, 2);

  char *sw_config_id_0 = "data/modem_config/mcfg_hw/generic/common/Default/Default/mcfg_sw.mbn";
  char *sw_config_id_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn0";
  char *sw_config_id_2 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn1";
  char *sw_file_1 = "data/modem_config/mcfg_sw/generic/China/CMCC/Commercial/Volte_OpenMkt/mcfg_sw.mbn";
  const uint32_t sw_config_len_1 = 31876;

  t.set_start();
  // Expect: QMI_PDC_SET_FEATURE_REQ_V01
  QMI_node.update_default_qmi_pdc_set_feature_req(0xFFFFFFFF);
  QMI_node.set_default_qmi_pdc_set_feature_req_selection_mode(PDC_SELECTION_MODE_DISABLED_V01);
  t.expect(QMI_node);

  // Inject: QMI_PDC_SET_FEATURE_RESP_V01
  QMI_node.update_default_qmi_pdc_set_feature_resp();
  t.inject(QMI_node);

  // Inject: QMI_PDC_SET_FEATURE_IND_V01
  QMI_node.update_default_qmi_pdc_set_feature_ind(QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);
  t.set_end();

  ril_utf_msim_bootup_helper_2(t, PDC_BOOT, TRUE, TRUE);

  // Start oem threads for oem socket
  t.add_func_exec(utf_start_oem_threads);

  // Start the ims threads for ims
  t.add_func_exec(utf_start_ims_threads);

  // Expect: ims_MsgId_UNSOL_RADIO_STATE_CHANGED
  IMS_node.update_default_ims_unsol_radio_state_changed(ims_RadioState_RADIO_STATE_ON);
  t.expect(IMS_node);

  // Execute the test case
  t.execute();
  t.print_summary();
  return t.get_test_result();
}
#endif

/**==
@testname: ril_utf_test_sglte_property
@description: test if property sglte_target is correctly set after msim init
==**/
test_result ril_utf_test_sglte_property( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  PropertyTestnode Property_node;

  t.set_boot_case(qmi_ril_msim_bootup_primary_002);

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Property");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  Property_node.utf_property_update_property("abc.123", "1");
  t.inject(Property_node);

  Property_node.utf_property_update_property("abc.123", "1");
  t.expect(Property_node);

  Property_node.utf_property_update_property("persist.vendor.radio.sglte_target", "1");
  t.expect(Property_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_gerrit_1270060_001
@description: In case IWLAN do not report PS restricted status.
==**/
test_result ril_utf_test_gerrit_1270060_001( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  OEMTestnode OEM_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);

  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);

  util_voice_radio_tech_lte_in_service(t, QMI_node, RIL_node, IMS_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, NAS_LTE_VOICE_STATUS_NO_VOICE_V01, NAS_SMS_STATUS_NO_SMS_V01, FALSE, TRUE, "123", "45", TRUE, TRUE);

  util_ril_utf_dsd_ind_info(t, RIL_node, OEM_node, QMI_node, TRUE, 1, DSD_SYS_NETWORK_3GPP_V01, DSD_SYS_RAT_EX_3GPP_WLAN_V01 , 0ull, TRUE, TRUE, TRUE, FALSE, RADIO_TECH_UNKNOWN, FALSE, FALSE);

  //Inject: QMI_NAS_SYS_INFO_IND_MSG_V01
  QMI_node.update_default_sys_info_ind_msg();
  nas_3gpp2_srv_status_info_type_v01* cdma_srv_status_info_ind = QMI_node.set_qmi_nas_sys_info_ind_cdma_srv_status_info();
  cdma_srv_status_info_ind->srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
  cdma_srv_status_info_ind->is_pref_data_path = 0;
  nas_3gpp_srv_status_info_type_v01* gsm_srv_status_info = QMI_node.set_qmi_nas_sys_info_ind_gsm_srv_status_info();
  gsm_srv_status_info->srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
  gsm_srv_status_info->true_srv_status = SYS_SRV_STATUS_NO_SRV_V01;
  gsm_srv_status_info->is_pref_data_path = 0;
  nas_3gpp_srv_status_info_type_v01* wcdma_srv_status_info = QMI_node.set_qmi_nas_sys_info_ind_wcdma_srv_status_info();
  wcdma_srv_status_info->srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
  wcdma_srv_status_info->true_srv_status = SYS_SRV_STATUS_NO_SRV_V01;
  wcdma_srv_status_info->is_pref_data_path = 0;
  nas_3gpp_srv_status_info_type_v01 *lte_srv_status_info = QMI_node.set_qmi_nas_sys_info_ind_lte_srv_status_info();
  lte_srv_status_info->srv_status = NAS_SYS_SRV_STATUS_SRV_V01;
  lte_srv_status_info->true_srv_status = SYS_SRV_STATUS_SRV_V01;
  lte_srv_status_info->is_pref_data_path = 0;
  nas_lte_sys_info_type_v01 *lte_sys_info = QMI_node.set_qmi_nas_sys_info_ind_lte_sys_info();
  lte_sys_info->common_sys_info.srv_domain_valid = TRUE;
  lte_sys_info->common_sys_info.srv_domain = SYS_SRV_DOMAIN_PS_ONLY_V01;
  lte_sys_info->common_sys_info.srv_capability_valid = TRUE;
  lte_sys_info->common_sys_info.srv_capability = SYS_SRV_DOMAIN_CS_PS_V01;
  lte_sys_info->common_sys_info.roam_status_valid = TRUE;
  lte_sys_info->common_sys_info.roam_status = NAS_SYS_ROAM_STATUS_OFF_V01;
  lte_sys_info->threegpp_specific_sys_info.network_id_valid = TRUE;
  strcpy(lte_sys_info->threegpp_specific_sys_info.network_id.mcc,"123");
  strcpy(lte_sys_info->threegpp_specific_sys_info.network_id.mnc,"45");
  QMI_node.set_qmi_nas_sys_info_ind_lte_voice_status(NAS_LTE_VOICE_STATUS_3GPP_V01 );
  QMI_node.set_qmi_nas_sys_info_ind_lte_sms_status(NAS_SMS_STATUS_3GPP_V01 );
  QMI_node.set_qmi_nas_sys_info_ind_lte_cell_status(NAS_CELL_ACCESS_EMERGENCY_ONLY_V01);
  QMI_node.set_qmi_nas_sys_info_ind_voice_support_on_lte(TRUE);
  t.inject(QMI_node);

  t.set_start();

#if 0 // enable this once the indication is added to IMS HIDL
  IMS_node.update_default_ims_unsol_vops_changed(TRUE);
  t.expect(IMS_node);
#endif

  RIL_node.update_default_ril_unsol_restricted_state_changed(2);
  t.expect(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  RIL_node.update_default_ril_unsol_response_voice_network_state_changed();
  t.expect(RIL_node);

  t.set_end();

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_38
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC" and Short_name="MCCMNC", then RIL will report Operaotr_name to Telephony
as Long_name="MCC" and Short_name="MCCMNC".
==**/
test_result ril_utf_test_operator_name_testcase_38( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206", "206 01", "206", "206 01", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_39
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC" and Short_name="MCC+MNC", then RIL will report Operaotr_name to Telephony
as Long_name="MCC" and Short_name="MCC+MNC".
==**/
test_result ril_utf_test_operator_name_testcase_39( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206", "206+01", "206", "206+01", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_40
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCCMNC" and Short_name="MCC", then RIL will report Operaotr_name to Telephony
as Long_name="MCCMNC" and Short_name="MCC".
==**/
test_result ril_utf_test_operator_name_testcase_40( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206 01", "206", "206 01", "206", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_41
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC+MNC" and Short_name="MCC", then RIL will report Operaotr_name to Telephony
as Long_name="MCC+MNC" and Short_name="MCC".
==**/
test_result ril_utf_test_operator_name_testcase_41( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206+01", "206", "206+01", "206", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_42
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC+MNC" and Short_name="", then RIL will look into data_base and found corresponding
Operator_name and report it to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_42( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206+01", "", "Belgacom Mobile", "Proximus", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_43
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC+MNC" and Short_name="   ", then RIL will look into data_base and found corresponding
Operator_name and report it to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_43( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206+01", "   ", "Belgacom Mobile", "Proximus", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_44
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="" and Short_name="MCC+MNC", then RIL will look into data_base and found corresponding
Operator_name and report it to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_44( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "", "206+01", "Belgacom Mobile", "Proximus", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_45
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="   " and Short_name="MCC+MNC", then RIL will look into data_base and found corresponding
Operator_name and report it to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_45( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "   ", "206+01", "Belgacom Mobile", "Proximus", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_46
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC+MNC" and Short_name="MCC+MNC", then RIL will look into data_base and found corresponding
Operator_name and report it to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_46( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "01", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "01", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206+01", "206+01", "Belgacom Mobile", "Proximus", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_47
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCCMNC" and Short_name="MCCMNC", then RIL will look into data_base and  didn't
found corresponding Operator_name and report what modem reported to Telephony.
==**/
test_result ril_utf_test_operator_name_testcase_47( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "02", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "02", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "20602", "20602", "20602", "20602", TRUE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_48
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="" and Short_name="", then RIL will look into data_base and  didn't
found corresponding Operator_name and report "mccmnc" as the operator long name
and property "persist.vendor.radio.fill_eons" is "true", so short_name=long_name.
==**/
test_result ril_utf_test_operator_name_testcase_48( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "02", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "02", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "", "", "20602", "20602", TRUE, FALSE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_49
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="" and Short_name="", then RIL will look into data_base and  didn't
found corresponding Operator_name and report "mccmnc" as the operator long name
and property "persist.vendor.radio.fill_eons" is "false", so short_name=long_name.
==**/
test_result ril_utf_test_operator_name_testcase_49( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "02", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "02", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "", "", "20602", "", FALSE, FALSE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_operator_name_testcase_50
@description: This test is to verify the "Operator_Name" for UMTS and modem reported
Long_name="MCC" and Short_name="MNC", then RIL will report operator long name
as "MCC" and Short_name="MNC".
==**/
test_result ril_utf_test_operator_name_testcase_50( user_specified_attrib_t attr )
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Operator_Name");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "0");
  db_property_set("persist.vendor.radio.csg_info_avlbl","1");

  util_voice_registration_state_for_wcdma_success(t, QMI_node, RIL_node, OEM_node, RADIO_TECH_UMTS, RIL_VAL_REG_REGISTERED_HOME_NET, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_ONLY_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, SYS_SRV_DOMAIN_NO_SRV_V01, 0, TRUE, "206", "02", TRUE, 21, 0, NULL);

  util_to_get_operator_name_3gpp_oprtr_mcc_mnc(t, QMI_node, RIL_node, "206", "02", TRUE, 21, NAS_RADIO_IF_UMTS_V01, "206", "02", "206", "02", FALSE, TRUE, FALSE, FALSE);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_enable_5g_inclusive_01( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("5g_enable");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_CDMA_HRPD_LTE);
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
  RIL_PreferredNetworkType* mode_pref = RIL_node.update_default_ril_request_set_preferred_network_type();
  *mode_pref = QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA;
  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE, 0, 0);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE_RESP
  RIL_node.update_default_ril_request_set_preferred_network_type_resp();
  t.expect(RIL_node);

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_ENABLE_5G
  RIL_node.update_default_enable_5g();
  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  QMI_node.set_qmi_nas_system_selection_preference_req_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE | QMI_NAS_RAT_MODE_PREF_NR5G, 0, 0);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_ENABLE_5G_RESP
  RIL_node.update_default_enable_5g_resp();
  t.expect(RIL_node);

  //Inject:QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
//  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
 // QMI_node.set_qmi_nas_system_selection_preference_ind_mode_pref(QMI_NAS_RAT_MODE_PREF_GSM_UMTS_LTE | QMI_NAS_RAT_MODE_PREF_NR5G);
 // t.inject(QMI_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_enable_modem_on( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  //property_set("persist.radio.multisim.config", "dsds");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("enableModem");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_ENABLE_MODEM
  int *ptr = RIL_node.update_default_enable_modem();
  RIL_node.fill_enable_modem(ptr, true);
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_dual_standby_pref_resp_active_subs_mask(QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_REQ
  QMI_node.update_default_qmi_nas_dual_standby_pref_req_msg();
  QMI_node.update_qmi_nas_dual_standby_pref_active_subs_mask(QMI_NAS_ACTIVE_SUB_PRIMARY_V01|
          QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_RESP
  QMI_node.update_default_qmi_nas_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  QMI_node.update_default_qmi_nas_dual_standby_pref_ind_msg();
  QMI_node.set_qmi_nas_standby_pref_ind_active_subs_mask(QMI_NAS_ACTIVE_SUB_PRIMARY_V01|
                    QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_ENABLE_MODEM
  RIL_node.update_default_enable_modem_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_enable_modem_off( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  //property_set("persist.radio.multisim.config", "dsds");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("enableModem");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_ENABLE_MODEM
  int *ptr = RIL_node.update_default_enable_modem();
  RIL_node.fill_enable_modem(ptr, false);
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_dual_standby_pref_resp_active_subs_mask(QMI_NAS_ACTIVE_SUB_PRIMARY_V01 |
          QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_REQ
  QMI_node.update_default_qmi_nas_dual_standby_pref_req_msg();
  QMI_node.update_qmi_nas_dual_standby_pref_active_subs_mask(QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_RESP
  QMI_node.update_default_qmi_nas_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  QMI_node.update_default_qmi_nas_dual_standby_pref_ind_msg();
  QMI_node.set_qmi_nas_standby_pref_ind_active_subs_mask(QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_ENABLE_MODEM
  RIL_node.update_default_enable_modem_resp(RIL_E_SUCCESS);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_enable_modem_timeout( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  //property_set("persist.radio.multisim.config", "dsds");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("enableModem");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_ENABLE_MODEM
  int *ptr = RIL_node.update_default_enable_modem();
  RIL_node.fill_enable_modem(ptr, false);
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_dual_standby_pref_resp_active_subs_mask(QMI_NAS_ACTIVE_SUB_PRIMARY_V01 |
          QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_REQ
  QMI_node.update_default_qmi_nas_dual_standby_pref_req_msg();
  QMI_node.update_qmi_nas_dual_standby_pref_active_subs_mask(QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_DUAL_STANDBY_PREF_REQ_MSG_V01_RESP
  QMI_node.update_default_qmi_nas_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_ENABLE_MODEM
  RIL_node.update_default_enable_modem_resp(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_modem_stack_status_on( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("enableModem");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_GET_MODEM_STACK_STATUS
  RIL_node.update_default_get_modem_stack_status();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_dual_standby_pref_resp_active_subs_mask(QMI_NAS_ACTIVE_SUB_PRIMARY_V01 |
          QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_MODEM_STACK_STATUS
  RIL_node.update_default_get_modem_stack_status_resp(true);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_modem_stack_status_off( user_specified_attrib_t attr )
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  db_property_set("persist.vendor.radio.limit_sys_info", "1");

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("enableModem");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  //////////////////////////////////////////////////////
  // inject: RIL_REQUEST_GET_MODEM_STACK_STATUS
  RIL_node.update_default_get_modem_stack_status();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_DUAL_STANDBY_PREF_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_DUAL_STANDBY_PREF_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_dual_standby_pref_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_dual_standby_pref_resp_active_subs_mask(QMI_NAS_ACTIVE_SUB_SECONDARY_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_MODEM_STACK_STATUS
  RIL_node.update_default_get_modem_stack_status_resp(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_set_unsol_cell_info_list_rate_001
@description: set_unsol_cell_info_list_rate
==**/
test_result ril_utf_test_set_unsol_cell_info_list_rate_001(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE
  ril_request_unsol_cell_info_rate_t* set_req = RIL_node.update_default_ril_request_set_unsol_cell_info_list_rate();
  set_req->rate = 100;
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE (Resp)
  RIL_node.update_default_ril_request_set_unsol_cell_info_list_rate_resp();
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_unsol_cell_info_list_rate_002(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE
  ril_request_unsol_cell_info_rate_t* set_req =
      RIL_node.update_default_ril_request_set_unsol_cell_info_list_rate();
  set_req->rate = -1;
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE (Resp)
  RIL_node.update_default_ril_request_set_unsol_cell_info_list_rate_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_001(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_ONE_SHOT;
  req->specifiers_length = 0;  // invalid param
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_ONE_SHOT;
  req->specifiers_length = MAX_RADIO_ACCESS_NETWORKS+1;  // 9 - invalid param
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // INVALID interval

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 4;  // invalid value: Valid ranges : 5-300
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 301;  // invalid value: Valid ranges : 5-300
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // INVALID maxSearchTime

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 100;  // valid; Valid ranges : 5-300
  req->maxSearchTime = 20;  // invalid; valid ranges : 60-3600
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 100;        // valid; Valid ranges : 5-300
  req->maxSearchTime = 3601;  // invalid; valid ranges : 60-3600
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);


  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = MAX_RADIO_ACCESS_NETWORKS + 1;  // Invalid > MAX_RADIO_ACCESS_NETWORKS;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 4;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->specifiers[3].radio_access_network = (RIL_RadioAccessNetworks)100;  // Invalid RAN
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[0].bands_length = MAX_BANDS + 2; // Invalid > MAX_BANDS; 8
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = UTRAN;
  req->specifiers[0].bands_length = MAX_BANDS + 2; // Invalid > MAX_BANDS; 8
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = EUTRAN;
  req->specifiers[0].bands_length = MAX_BANDS + 2;  // Invalid > MAX_BANDS; 8
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = EUTRAN;
  req->specifiers[0].channels_length = 33;  // Invalid > MAX_CHANNELS; 32
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // SUCCESS case

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(NAS_NETWORK_TYPE_GSM_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);


  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_002(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = GERAN;
  req->interval = 10;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  req->incrementalResults = 1;  // Incremental results are reported
  req->incrementalResultsPeriodicity = 2;
  req->mccMncLength = 2;
  strlcpy(req->mccMncs[0], "40412", strlen("40412")+1);
  strlcpy(req->mccMncs[1], "405025", strlen("405025")+1);
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(NAS_NETWORK_TYPE_GSM_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(10);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(1);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results_periodicity(2);
  nas_plmn_id_ext_type_v01 mcc_list[2] = { {.mcc=404, .mnc=12, .mnc_includes_pcs_digit=FALSE},
                                           {.mcc=405, .mnc=25, .mnc_includes_pcs_digit=TRUE} };
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_preferred_plmn_list(2, mcc_list);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_003(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  struct NetworkScanRequset {
    RIL_NetworkScanRequest rilReq;
    nas_perform_incremental_network_scan_req_msg_v01 qmiReq;
    nas_perform_incremental_network_scan_resp_msg_v01 qmiResp;
    NetworkScanRequset(
        std::function<void(RIL_NetworkScanRequest &)> getImsReq,
        std::function<void(nas_perform_incremental_network_scan_req_msg_v01 &)> getQmiReq,
        std::function<void(nas_perform_incremental_network_scan_resp_msg_v01 &)> getQmiResp) {
      memset(&rilReq, 0x00, sizeof(rilReq));
      rilReq.type = RIL_PERIODIC;
      rilReq.specifiers_length = 1;
      rilReq.specifiers[0].radio_access_network = GERAN;
      rilReq.interval = 10;  // in seconds
      rilReq.maxSearchTime = 60;  // in seconds
      rilReq.incrementalResults = 1;  // Incremental results are reported
      rilReq.incrementalResultsPeriodicity = 2;
      getImsReq(rilReq);

      memset(&qmiReq, 0x00, sizeof(qmiReq));
      qmiReq.scan_type_valid = TRUE;
      qmiReq.scan_type = NAS_SCAN_TYPE_MODE_PREF_V01;
      qmiReq.search_periodicity_valid = TRUE;
      qmiReq.search_periodicity = 10;
      qmiReq.max_search_time_valid = TRUE;
      qmiReq.max_search_time = 60;
      qmiReq.incremental_results_valid = TRUE;
      qmiReq.incremental_results = 1;
      qmiReq.incremental_results_periodicity_valid = TRUE;
      qmiReq.incremental_results_periodicity = 2;
      getQmiReq(qmiReq);

      memset(&qmiResp, 0x00, sizeof(qmiResp));
      getQmiResp(qmiResp);
      qmiResp.resp.result = QMI_RESULT_SUCCESS_V01;
      qmiResp.resp.error = QMI_ERR_NONE_V01;
    }
  };

  std::vector<NetworkScanRequset> testReqVector {
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = GERAN;
        req.specifiers[0].bands_length = 8;
        RIL_GeranBands band = GERAN_BAND_T380;
        for (uint32_t i = 0; i < 8; i++) {  //1-8
          req.specifiers[0].bands.geran_bands[i] = band;
          band = (RIL_GeranBands)(band+1);
        }
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_GSM_ONLY_V01;
        req.band_pref_valid = TRUE;
        req.band_pref = (QMI_NAS_GSM_BAND_450_V01|
                         QMI_NAS_GSM_BAND_480_V01|
                         QMI_NAS_GSM_BAND_750_V01|
                         QMI_NAS_GSM_BAND_850_V01);
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = GERAN;
        req.specifiers[0].bands_length = 8;
        RIL_GeranBands band = GERAN_BAND_T810;
        for (uint32_t i = 0; i < 8; i++) {  //8-14
          req.specifiers[0].bands.geran_bands[i] = band;
          band = (RIL_GeranBands)(band+1);
        }
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_GSM_ONLY_V01;
        req.band_pref_valid = TRUE;
        req.band_pref = (QMI_NAS_GSM_BAND_850_V01|
                         QMI_NAS_P_GSM_900_BAND_V01|
                         QMI_NAS_E_GSM_900_BAND_V01|
                         QMI_NAS_GSM_BAND_RAILWAYS_900_BAND_V01|
                         QMI_NAS_GSM_BAND_PCS_1900_BAND_V01);
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = UTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_UtranBands band = UTRAN_BAND_1;
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.utran_bands[i] = band;
          band = (RIL_UtranBands)(band+1);
        }
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_WCDMA_ONLY_V01|NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01;
        req.band_pref_valid = TRUE;
        req.band_pref = (QMI_NAS_WCDMA_EU_J_CH_IMT_2100_BAND_V01|
                         QMI_NAS_WCDMA_US_PCS_1900_BAND_V01|
                         QMI_NAS_EU_CH_DCS_1800_BAND_V01|
                         QMI_NAS_WCDMA_US_1700_BAND_V01|
                         QMI_NAS_WCDMA_US_850_BAND_V01|
                         QMI_NAS_WCDMA_EU_2600_BAND_V01|
                         QMI_NAS_WCDMA_EU_J_900_BAND_V01);
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = UTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_UtranBands band = UTRAN_BAND_9;
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.utran_bands[i] = band;
          band = (RIL_UtranBands)(band+1);
        }
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_WCDMA_ONLY_V01|NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01;
        req.band_pref_valid = TRUE;
        req.band_pref = (QMI_NAS_WCDMA_J_1700_BAND_V01);
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = UTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_UtranBands band = UTRAN_BAND_19;
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.utran_bands[i] = band;
          band = (RIL_UtranBands)(band+1);
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_WCDMA_ONLY_V01|NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01;
        req.band_pref_valid = TRUE;
        req.band_pref = (QMI_NAS_WCDMA_JAPAN_800_BAND_V01);
        req.wcdma_arfcn_valid = TRUE;
        req.wcdma_arfcn_len = 2;
        req.wcdma_arfcn[0] = 100;
        req.wcdma_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = EUTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_EutranBands bands[8] = {EUTRAN_BAND_1, EUTRAN_BAND_2, EUTRAN_BAND_3, EUTRAN_BAND_4,
                                   EUTRAN_BAND_5, EUTRAN_BAND_6, EUTRAN_BAND_7, EUTRAN_BAND_8};
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.eutran_bands[i] = bands[i];
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_LTE_ONLY_V01;
        req.lte_band_pref_valid = TRUE;
        req.lte_band_pref = (E_UTRA_OPERATING_BAND_1_V01|E_UTRA_OPERATING_BAND_2_V01|
                         E_UTRA_OPERATING_BAND_3_V01|E_UTRA_OPERATING_BAND_4_V01|
                         E_UTRA_OPERATING_BAND_5_V01|E_UTRA_OPERATING_BAND_6_V01|
                         E_UTRA_OPERATING_BAND_7_V01|E_UTRA_OPERATING_BAND_8_V01);
        req.lte_arfcn_valid = TRUE;
        req.lte_arfcn_len = 2;
        req.lte_arfcn[0] = 100;
        req.lte_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = EUTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_EutranBands bands[8] = {EUTRAN_BAND_9, EUTRAN_BAND_10, EUTRAN_BAND_11, EUTRAN_BAND_12,
                                   EUTRAN_BAND_13, EUTRAN_BAND_14, EUTRAN_BAND_17, EUTRAN_BAND_18};
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.eutran_bands[i] = bands[i];
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_LTE_ONLY_V01;
        req.lte_band_pref_valid = TRUE;
        req.lte_band_pref = (E_UTRA_OPERATING_BAND_18_V01|E_UTRA_OPERATING_BAND_17_V01|
                         E_UTRA_OPERATING_BAND_14_V01|E_UTRA_OPERATING_BAND_13_V01|
                         E_UTRA_OPERATING_BAND_12_V01|E_UTRA_OPERATING_BAND_11_V01|
                         E_UTRA_OPERATING_BAND_10_V01|E_UTRA_OPERATING_BAND_9_V01);
        req.lte_arfcn_valid = TRUE;
        req.lte_arfcn_len = 2;
        req.lte_arfcn[0] = 100;
        req.lte_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = EUTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_EutranBands bands[8] = {EUTRAN_BAND_19, EUTRAN_BAND_20, EUTRAN_BAND_21, EUTRAN_BAND_23,
                                   EUTRAN_BAND_24, EUTRAN_BAND_25, EUTRAN_BAND_26, EUTRAN_BAND_28};
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.eutran_bands[i] = bands[i];
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_LTE_ONLY_V01;
        req.lte_band_pref_valid = TRUE;
        req.lte_band_pref = (E_UTRA_OPERATING_BAND_28_V01|E_UTRA_OPERATING_BAND_26_V01|
                         E_UTRA_OPERATING_BAND_25_V01|E_UTRA_OPERATING_BAND_24_V01|
                         E_UTRA_OPERATING_BAND_23_V01|E_UTRA_OPERATING_BAND_21_V01|
                         E_UTRA_OPERATING_BAND_20_V01|E_UTRA_OPERATING_BAND_19_V01);
        req.lte_arfcn_valid = TRUE;
        req.lte_arfcn_len = 2;
        req.lte_arfcn[0] = 100;
        req.lte_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = EUTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_EutranBands bands[8] = {EUTRAN_BAND_30, EUTRAN_BAND_33, EUTRAN_BAND_34, EUTRAN_BAND_35,
                                   EUTRAN_BAND_36, EUTRAN_BAND_37, EUTRAN_BAND_38, EUTRAN_BAND_39};
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.eutran_bands[i] = bands[i];
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_LTE_ONLY_V01;
        req.lte_band_pref_valid = TRUE;
        req.lte_band_pref = (E_UTRA_OPERATING_BAND_30_V01|E_UTRA_OPERATING_BAND_33_V01|
                         E_UTRA_OPERATING_BAND_34_V01|E_UTRA_OPERATING_BAND_35_V01|
                         E_UTRA_OPERATING_BAND_36_V01|E_UTRA_OPERATING_BAND_37_V01|
                         E_UTRA_OPERATING_BAND_38_V01|E_UTRA_OPERATING_BAND_39_V01);
        req.lte_arfcn_valid = TRUE;
        req.lte_arfcn_len = 2;
        req.lte_arfcn[0] = 100;
        req.lte_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
     NetworkScanRequset(
      [](RIL_NetworkScanRequest &req) -> void {
        req.specifiers[0].radio_access_network = EUTRAN;
        req.specifiers[0].bands_length = 8;
        RIL_EutranBands bands[8] = {EUTRAN_BAND_40, EUTRAN_BAND_41, EUTRAN_BAND_42, EUTRAN_BAND_43,
                                   EUTRAN_BAND_46, EUTRAN_BAND_22, EUTRAN_BAND_66, EUTRAN_BAND_68};
        for (uint32_t i = 0; i < 8; i++) {
          req.specifiers[0].bands.eutran_bands[i] = bands[i];
        }
        req.specifiers[0].channels_length = 2;
        req.specifiers[0].channels[0] = 100;
        req.specifiers[0].channels[1] = 200;
      },
      [](nas_perform_incremental_network_scan_req_msg_v01 &req) -> void {
        req.network_type_valid = TRUE;
        req.network_type = NAS_NETWORK_TYPE_LTE_ONLY_V01;
        req.lte_band_pref_valid = TRUE;
        req.lte_band_pref = (E_UTRA_OPERATING_BAND_40_V01|E_UTRA_OPERATING_BAND_41_V01|
                         E_UTRA_OPERATING_BAND_42_V01|E_UTRA_OPERATING_BAND_43_V01|
                         E_UTRA_OPERATING_BAND_46_V01);
        req.lte_arfcn_valid = TRUE;
        req.lte_arfcn_len = 2;
        req.lte_arfcn[0] = 100;
        req.lte_arfcn[1] = 200;
      },
      [](nas_perform_incremental_network_scan_resp_msg_v01 &resp) -> void {
      }),
  };

  for (auto testReq : testReqVector) {
    // Inject: RIL_REQUEST_START_NETWORK_SCAN
    RIL_node.update_default_ril_request_start_network_scan(&testReq.rilReq);
    t.inject(RIL_node);

    QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req(&testReq.qmiReq);
    t.expect(QMI_node);

    // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
    t.inject(QMI_node);

    // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
    RIL_node.update_default_ril_request_start_network_scan_resp();
    t.expect(RIL_node);
  }

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_004(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_STOP_NETWORK_SCAN
  RIL_node.update_default_ril_request_stop_network_scan();
  t.inject(RIL_node);

  // Expect: QMI_NAS_ABORT_SCAN_REQ_V01
  QMI_node.update_default_qmi_nas_abort_scan_req();
  t.expect(QMI_node);

  // Inject: QMI_NAS_ABORT_SCAN_REQ_V01
  QMI_node.update_default_qmi_nas_abort_scan_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_STOP_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_stop_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  // inject: RIL_REQUEST_STOP_NETWORK_SCAN
  RIL_node.update_default_ril_request_stop_network_scan();
  t.inject(RIL_node);

  // Expect: QMI_NAS_ABORT_SCAN_REQ_V01
  QMI_node.update_default_qmi_nas_abort_scan_req();
  t.expect(QMI_node);

  // Inject: QMI_NAS_ABORT_SCAN_REQ_V01
  QMI_node.update_default_qmi_nas_abort_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_STOP_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_stop_network_scan_resp();
  t.expect(RIL_node);


  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_005(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 3;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_GSM_ONLY_V01|
      NAS_NETWORK_TYPE_WCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);


  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_PARTIAL_V01);
  nas_3gpp_network_scan_result_type_v01 *nas_network_scan_info = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(1);
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 8;  // LTE
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel")+1);
  int32_t val[] = {64};
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_LTE_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult *scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = PARTIAL;
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_network_scan_info = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(4);
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 8;  // LTE
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel")+1);
  nas_network_scan_info[1].mobile_country_code = 404;
  nas_network_scan_info[1].mobile_network_code = 11;
  nas_network_scan_info[1].network_status = 154;
  nas_network_scan_info[1].rat = 4; // GERAN
  nas_network_scan_info[1].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[1].network_description, "404 11", strlen("404 11")+1);
  nas_network_scan_info[2].mobile_country_code = 405;
  nas_network_scan_info[2].mobile_network_code = 872;
  nas_network_scan_info[2].network_status = 154;
  nas_network_scan_info[2].rat = 5;  // UMTS
  nas_network_scan_info[2].mnc_includes_pcs_digit = true;
  strlcpy(nas_network_scan_info[2].network_description, "JIO", strlen("JIO")+1);
  nas_network_scan_info[3].mobile_country_code = 405;
  nas_network_scan_info[3].mobile_network_code = 5;
  nas_network_scan_info[3].network_status = 154;
  nas_network_scan_info[3].rat = 9;  // TDS-CDMA
  nas_network_scan_info[3].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[3].network_description, "Reliance", strlen("Reliance")+1);
  int32_t val_4[] = {64, 71, 51, 57};
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(4, val_4);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_LTE_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 11);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_GSM_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 6;
  strlcpy(eons_plmn_name->plmn_short_name, "404 11", strlen("404 11")+1);
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 6;
  strlcpy(eons_plmn_name->plmn_long_name, "404 11", strlen("404 11")+1);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(405, 872);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_UMTS_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 3;
  strlcpy(eons_plmn_name->plmn_short_name, "JIO", strlen("JIO")+1);
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 7;
  strlcpy(eons_plmn_name->plmn_long_name, "IND-JIO", strlen("IND-JIO")+1);
  t.inject(QMI_node);


  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(405, 5);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_TDSCDMA_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 8;
  strlcpy(eons_plmn_name->plmn_short_name, "Reliance", strlen("Reliance")+1);
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 8;
  strlcpy(eons_plmn_name->plmn_long_name, "Reliance", strlen("Reliance")+1);
  t.inject(QMI_node);


  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  t.expect(RIL_node);



  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_006(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 3;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_GSM_ONLY_V01|
      NAS_NETWORK_TYPE_WCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);


  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_PARTIAL_PERIODIC_V01);
  nas_3gpp_network_scan_result_type_v01 *nas_network_scan_info = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(1);
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 8;  // LTE
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel")+1);
  int32_t val[] = {64};
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_LTE_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult *scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = PARTIAL;
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_ABORT_V01);
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->error = RIL_E_ABORTED;
  t.expect(RIL_node);



  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_network_scan_007(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }


  // NAS_SCAN_STATUS_ABORT_V01

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 3;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_GSM_ONLY_V01|
      NAS_NETWORK_TYPE_WCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_ABORT_V01);
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult *scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->error = RIL_E_ABORTED;
  t.expect(RIL_node);


  // NAS_SCAN_STATUS_REJ_IN_RLF_V01

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 3;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_GSM_ONLY_V01|
      NAS_NETWORK_TYPE_WCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_REJ_IN_RLF_V01);
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->error = RIL_E_ABORTED;
  t.expect(RIL_node);

  // NAS_SCAN_STATUS_INCREMENT_ERROR_V01

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 3;
  req->specifiers[0].radio_access_network = GERAN;
  req->specifiers[1].radio_access_network = UTRAN;
  req->specifiers[2].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_GSM_ONLY_V01|
      NAS_NETWORK_TYPE_WCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01|
      NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_INCREMENT_ERROR_V01);
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->error = RIL_E_ABORTED;
  t.expect(RIL_node);


  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

// NR only scan without any bands
test_result ril_ut_test_network_scan_008(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = NGRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_NR5G_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_PLMN_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_3gpp_network_scan_result_type_v01 *nas_network_scan_info =
    QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(1);
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 12;  // NR5G
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel")+1);
  int32_t val[] = {64};
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult *scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();

}

// NR only scan with bands
test_result ril_ut_test_network_scan_009(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = NGRAN;
  req->specifiers[0].bands_length = 1;
  req->specifiers[0].bands.ngran_bands[0] = NGRAN_BAND_5;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_NR5G_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_MODE_PREF_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);

  nas_nr5g_band_pref_mask_type_v01 band {};
  band.bits_1_64 |= (1ULL << ( NGRAN_BAND_5 - 1));
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_nr5g_band_pref(band);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  t.expect(RIL_node);


  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_3gpp_network_scan_result_type_v01 *nas_network_scan_info = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(1);
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 12;  // NR5G
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel")+1);
  int32_t val[] = {64};
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult *scanResult = RIL_node.update_default_ril_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();

}

// Scan with invalid bands
test_result ril_ut_test_network_scan_010(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest *req = RIL_node.update_default_ril_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = EUTRAN;
  req->interval = 5;  // in seconds
  req->maxSearchTime = 60;  // in seconds

  // Pass invalid bands
  req->specifiers[0].bands_length = 8;
  RIL_EutranBands bands[8] = {EUTRAN_BAND_70, EUTRAN_BAND_27, EUTRAN_BAND_31, EUTRAN_BAND_44,
    EUTRAN_BAND_45, EUTRAN_BAND_47, EUTRAN_BAND_48, EUTRAN_BAND_65};
  for (uint32_t i = 0; i < 8; i++) {
    req->specifiers[0].bands.eutran_bands[i] = bands[i];
  }

  t.inject(RIL_node);

  // Expect: RIL_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_request_start_network_scan_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_ut_test_network_scan_011
@description: CAG info in scan results.
==**/
test_result ril_ut_test_network_scan_011(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Precondition: Registered to NR5G with CAG cell
  nas_cag_id_name_type_v01 cag_id_name{};
  cag_id_name.id = 505;
  cag_id_name.name_len = 3;
  cag_id_name.name[0] = 'a';
  cag_id_name.name[1] = 'b';
  cag_id_name.name[2] = 'c';
  cag_id_name.name[3] = 0;

  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "404",
                                         "10",
                                         "404",
                                         "10",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         &cag_id_name,
                                         nullptr);

  // inject: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest* req = RIL_node.update_default_ril_qti_radio_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = NGRAN;
  req->specifiers[0].bands_length = 1;
  req->specifiers[0].bands.ngran_bands[0] = NGRAN_BAND_5;
  req->interval = 5;        // in seconds
  req->maxSearchTime = 60;  // in seconds
  req->accessMode = RIL_ACCESS_MODE_PLMN;
  req->searchType = RIL_SEARCH_TYPE_PLMN_AND_CAG;
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_NR5G_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_MODE_PREF_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  // QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_type(
      NAS_SEARCH_TYPE_PLMN_AND_CAG_V01);

  nas_nr5g_band_pref_mask_type_v01 band{};
  band.bits_1_64 |= (1ULL << (NGRAN_BAND_5 - 1));
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_nr5g_band_pref(band);
  // TODO
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01,
                                                                        QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_qti_radio_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_3gpp_network_scan_result_type_v01* nas_network_scan_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(3);

  // only PLMN
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 12;  // NR5G
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);
  int32_t val[] = { 64 };
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);

  // PLMN+CAG with same MCCMNC
  nas_network_scan_info[1].mobile_country_code = 404;
  nas_network_scan_info[1].mobile_network_code = 10;
  nas_network_scan_info[1].network_status = 101;
  nas_network_scan_info[1].rat = 12;  // NR5G
  nas_network_scan_info[1].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);

  // PLMN
  nas_network_scan_info[2].mobile_country_code = 310;
  nas_network_scan_info[2].mobile_network_code = 1;
  nas_network_scan_info[2].network_status = 170;
  nas_network_scan_info[2].rat = 12;  // NR5G
  nas_network_scan_info[2].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "310 01", strlen("310 01") + 1);

  // CAG Info
  nas_cag_nw_info_type_v01* cag_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info_cag_info(3);
  cag_info[0].cag_info_valid = false;
  cag_info[0].cag_in_allowed_list = false;
  cag_info[0].cag_id_name.id = 0;
  cag_info[0].cag_id_name.name_len = 0;  // NR5G

  cag_info[1].cag_info_valid = true;
  cag_info[1].cag_in_allowed_list = false;
  cag_info[1].cag_id_name.id = 505;
  cag_info[1].cag_id_name.name_len = 1;  // NR5G
  // strlcpy(cag_info[1].cag_id_name.name, "abc", strlen("abc")+1);

  cag_info[2].cag_info_valid = false;
  cag_info[2].cag_in_allowed_list = false;
  cag_info[2].cag_id_name.id = 0;
  cag_info[2].cag_id_name.name_len = 0;  // NR5G

  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 1);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "test", strlen("test") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult* scanResult =
      RIL_node.update_default_ril_qti_radio_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->network_infos_length = 3;
  scanResult->network_infos =
      qcril_malloc2(scanResult->network_infos, scanResult->network_infos_length);
  scanResult->network_infos[0].registered = FALSE;
  scanResult->network_infos[0].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc));

  scanResult->network_infos[1].registered = TRUE;
  scanResult->network_infos[1].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc));
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info_valid = TRUE;
  // scanResult->network_infos[1].cag_info.cagName;
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info.cagId = 505;

  scanResult->network_infos[2].registered = FALSE;
  scanResult->network_infos[2].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc,
          "310",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc,
          "01",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc));

  t.expect(RIL_node);

  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_ut_test_network_scan_012
@description: CAG info in scan results.

Test Data :
1. PLMN 40410, cag only access is TRUE
2. CAG Cell  40410, with CAG Id 505, CAG only access is FALSE
3. PLMN 31001 , Normal PLMN
==**/
test_result ril_ut_test_network_scan_012(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Precondition: Registered to NR5G with CAG cell
  nas_cag_id_name_type_v01 cag_id_name{};
  cag_id_name.id = 505;
  cag_id_name.name_len = 3;
  cag_id_name.name[0] = 'a';
  cag_id_name.name[1] = 'b';
  cag_id_name.name[2] = 'c';
  cag_id_name.name[3] = 0;

  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "404",
                                         "10",
                                         "404",
                                         "10",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         &cag_id_name,
                                         nullptr);

  // inject: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest* req = RIL_node.update_default_ril_qti_radio_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = NGRAN;
  req->specifiers[0].bands_length = 1;
  req->specifiers[0].bands.ngran_bands[0] = NGRAN_BAND_5;
  req->interval = 5;        // in seconds
  req->maxSearchTime = 60;  // in seconds
  req->accessMode = RIL_ACCESS_MODE_PLMN;
  req->searchType = RIL_SEARCH_TYPE_PLMN_AND_CAG;
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_NR5G_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_MODE_PREF_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  // QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_type(
      NAS_SEARCH_TYPE_PLMN_AND_CAG_V01);

  nas_nr5g_band_pref_mask_type_v01 band{};
  band.bits_1_64 |= (1ULL << (NGRAN_BAND_5 - 1));
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_nr5g_band_pref(band);
  // TODO
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01,
                                                                        QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_qti_radio_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_3gpp_network_scan_result_type_v01* nas_network_scan_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(3);

  // only PLMN
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 12;  // NR5G
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);
  int32_t val[] = { 64 };
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);

  // PLMN+CAG with same MCCMNC
  nas_network_scan_info[1].mobile_country_code = 404;
  nas_network_scan_info[1].mobile_network_code = 10;
  nas_network_scan_info[1].network_status = 101;
  nas_network_scan_info[1].rat = 12;  // NR5G
  nas_network_scan_info[1].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);

  // PLMN
  nas_network_scan_info[2].mobile_country_code = 310;
  nas_network_scan_info[2].mobile_network_code = 1;
  nas_network_scan_info[2].network_status = 170;
  nas_network_scan_info[2].rat = 12;  // NR5G
  nas_network_scan_info[2].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "310 01", strlen("310 01") + 1);

  // CAG Info
  nas_cag_nw_info_type_v01* cag_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info_cag_info(3);
  cag_info[0].cag_info_valid = false;
  cag_info[0].cag_in_allowed_list = false;
  cag_info[0].cag_id_name.id = 0;
  cag_info[0].cag_id_name.name_len = 0;  // NR5G

  cag_info[1].cag_info_valid = true;
  cag_info[1].cag_in_allowed_list = false;
  cag_info[1].cag_id_name.id = 505;
  cag_info[1].cag_id_name.name_len = 1;  // NR5G
  // strlcpy(cag_info[1].cag_id_name.name, "abc", strlen("abc")+1);

  cag_info[2].cag_info_valid = false;
  cag_info[2].cag_in_allowed_list = false;
  cag_info[2].cag_id_name.id = 0;
  cag_info[2].cag_id_name.name_len = 0;  // NR5G

  // CAG only Access
  uint8_t* cag_only_access = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_cag_only_access(3);
  cag_only_access[0] = true;
  cag_only_access[1] = false;
  cag_only_access[2] = false;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 1);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "test", strlen("test") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult* scanResult =
      RIL_node.update_default_ril_qti_radio_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->network_infos_length = 3;
  scanResult->network_infos =
      qcril_malloc2(scanResult->network_infos, scanResult->network_infos_length);
  scanResult->network_infos[0].registered = FALSE;
  scanResult->network_infos[0].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc));

  scanResult->network_infos[1].registered = TRUE;
  scanResult->network_infos[1].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc));
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info_valid = TRUE;
  // scanResult->network_infos[1].cag_info.cagName;
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info.cagId = 505;

  scanResult->network_infos[2].registered = FALSE;
  scanResult->network_infos[2].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc,
          "310",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc,
          "01",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc));

  t.expect(RIL_node);


  t.execute();

  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_ut_test_network_scan_013
@description: CAG info in scan results.
Network Scan Test to check 2 scan results connected(Normal PLMN + CAG cell),
when Normal PLMN and CAG PLMN have same MccMnc and sys info reports 5G in service but
no CAG cells registered.

cag_only_access is false for all cag cells.

Normal PLMN : 40410
CAG Cell : 40410

Device is not registered with any CAG cell.
==**/
test_result ril_ut_test_network_scan_013(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Precondition: Registered to NR5G with CAG cell
  nas_cag_id_name_type_v01 cag_id_name{};
  cag_id_name.id = 505;
  cag_id_name.name_len = 3;
  cag_id_name.name[0] = 'a';
  cag_id_name.name[1] = 'b';
  cag_id_name.name[2] = 'c';
  cag_id_name.name[3] = 0;

  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "404",
                                         "10",
                                         "404",
                                         "10",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         nullptr,//&cag_id_name,
                                         nullptr);

  // inject: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN
  RIL_NetworkScanRequest* req = RIL_node.update_default_ril_qti_radio_request_start_network_scan();
  req->type = RIL_PERIODIC;
  req->specifiers_length = 1;
  req->specifiers[0].radio_access_network = NGRAN;
  req->specifiers[0].bands_length = 1;
  req->specifiers[0].bands.ngran_bands[0] = NGRAN_BAND_5;
  req->interval = 5;        // in seconds
  req->maxSearchTime = 60;  // in seconds
  req->accessMode = RIL_ACCESS_MODE_PLMN;
  req->searchType = RIL_SEARCH_TYPE_PLMN_AND_CAG;
  t.inject(RIL_node);

  // Expect: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_req();
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_network_type(
      NAS_NETWORK_TYPE_NR5G_ONLY_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_scan_type(NAS_SCAN_TYPE_MODE_PREF_V01);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_periodicity(5);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_max_search_time(60);
  // QMI_node.set_qmi_nas_perform_incremental_network_scan_req_incremental_results(FALSE);
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_search_type(
      NAS_SEARCH_TYPE_PLMN_AND_CAG_V01);

  nas_nr5g_band_pref_mask_type_v01 band{};
  band.bits_1_64 |= (1ULL << (NGRAN_BAND_5 - 1));
  QMI_node.set_qmi_nas_perform_incremental_network_scan_req_nr5g_band_pref(band);
  // TODO
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_resp(QMI_RESULT_SUCCESS_V01,
                                                                        QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN (resp)
  RIL_node.update_default_ril_qti_radio_request_start_network_scan_resp();
  t.expect(RIL_node);

  // Inject: QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01
  QMI_node.update_default_qmi_nas_perform_incremental_network_scan_ind(NAS_SCAN_STATUS_COMPLETE_V01);
  nas_3gpp_network_scan_result_type_v01* nas_network_scan_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info(3);

  // only PLMN
  nas_network_scan_info[0].mobile_country_code = 404;
  nas_network_scan_info[0].mobile_network_code = 10;
  nas_network_scan_info[0].network_status = 101;
  nas_network_scan_info[0].rat = 12;  // NR5G
  nas_network_scan_info[0].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);
  int32_t val[] = { 64 };
  QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_signal_strength(1, val);

  // PLMN+CAG with same MCCMNC
  nas_network_scan_info[1].mobile_country_code = 404;
  nas_network_scan_info[1].mobile_network_code = 10;
  nas_network_scan_info[1].network_status = 101;
  nas_network_scan_info[1].rat = 12;  // NR5G
  nas_network_scan_info[1].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "airtel", strlen("airtel") + 1);

  // PLMN
  nas_network_scan_info[2].mobile_country_code = 310;
  nas_network_scan_info[2].mobile_network_code = 1;
  nas_network_scan_info[2].network_status = 170;
  nas_network_scan_info[2].rat = 12;  // NR5G
  nas_network_scan_info[2].mnc_includes_pcs_digit = false;
  strlcpy(nas_network_scan_info[0].network_description, "310 01", strlen("310 01") + 1);

  // CAG Info
  nas_cag_nw_info_type_v01* cag_info =
      QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_nas_network_scan_info_cag_info(3);
  cag_info[0].cag_info_valid = false;
  cag_info[0].cag_in_allowed_list = false;
  cag_info[0].cag_id_name.id = 0;
  cag_info[0].cag_id_name.name_len = 0;  // NR5G

  cag_info[1].cag_info_valid = true;
  cag_info[1].cag_in_allowed_list = false;
  cag_info[1].cag_id_name.id = 505;
  cag_info[1].cag_id_name.name_len = 1;  // NR5G
  // strlcpy(cag_info[1].cag_id_name.name, "abc", strlen("abc")+1);

  cag_info[2].cag_info_valid = false;
  cag_info[2].cag_in_allowed_list = false;
  cag_info[2].cag_id_name.id = 0;
  cag_info[2].cag_id_name.name_len = 0;  // NR5G

  // CAG only Access
  uint8_t* cag_only_access = QMI_node.set_qmi_nas_perform_incremental_network_scan_ind_cag_only_access(3);
  cag_only_access[0] = false;
  cag_only_access[1] = false;
  cag_only_access[2] = false;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(404, 10);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "airtel", strlen("airtel") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 1);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_NR5G_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "test", strlen("test") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_QTI_RADIO_UNSOL_NETWORK_SCAN_RESULT
  RIL_NetworkScanResult* scanResult =
      RIL_node.update_default_ril_qti_radio_unsol_network_scan_result();
  scanResult->status = COMPLETE;
  scanResult->network_infos_length = 3;
  scanResult->network_infos =
      qcril_malloc2(scanResult->network_infos, scanResult->network_infos_length);
  scanResult->network_infos[0].registered = TRUE;
  scanResult->network_infos[0].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[0].CellInfo.nr.cellIdentityNr.mnc));

  scanResult->network_infos[1].registered = FALSE;
  scanResult->network_infos[1].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc,
          "404",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc,
          "10",
          sizeof(scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.mnc));
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info_valid = TRUE;
  // scanResult->network_infos[1].cag_info.cagName;
  scanResult->network_infos[1].CellInfo.nr.cellIdentityNr.cag_info.cagId = 505;

  scanResult->network_infos[2].registered = FALSE;
  scanResult->network_infos[2].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc,
          "310",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mcc));
  strlcpy(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc,
          "01",
          sizeof(scanResult->network_infos[2].CellInfo.nr.cellIdentityNr.mnc));

  t.expect(RIL_node);


  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_neighboring_cell_ids_001(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }


  // inject: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS
  RIL_node.update_default_ril_request_get_neighboring_cell_ids();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS (resp)
  RIL_NeighboringCell** neighCells = RIL_node.update_default_ril_request_get_neighboring_cell_ids_resp(0);
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);



  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_neighboring_cell_ids_002(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // UMTS case

  // inject: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS
  RIL_node.update_default_ril_request_get_neighboring_cell_ids();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_umts_cell_info_type_v01 *umts_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_umts_info();
  umts_info->cell_id = 45035;
  umts_info->plmn[0] = 0;
  umts_info->plmn[1] = 0;
  umts_info->plmn[2] = 0;
  umts_info->lac = 5304;
  umts_info->uarfcn = 10657;
  umts_info->psc = 410;
  umts_info->rscp = -71;
  umts_info->ecio = -2;
  umts_info->umts_monitored_cell_len = 2;
  umts_info->umts_monitored_cell[0].umts_uarfcn = 10657;
  umts_info->umts_monitored_cell[0].umts_psc = 410;
  umts_info->umts_monitored_cell[0].umts_rscp = -71;
  umts_info->umts_monitored_cell[0].umts_ecio = -2;
  umts_info->umts_monitored_cell[1].umts_uarfcn = 10657;
  umts_info->umts_monitored_cell[1].umts_psc = 410;
  umts_info->umts_monitored_cell[1].umts_rscp = -71;
  umts_info->umts_monitored_cell[1].umts_ecio = -2;
  umts_info->umts_geran_nbr_cell_len = 2;
  umts_info->umts_geran_nbr_cell[0].geran_arfcn = 56;
  umts_info->umts_geran_nbr_cell[0].geran_bsic_ncc = 255;
  umts_info->umts_geran_nbr_cell[0].geran_bsic_bcc = 255;
  umts_info->umts_geran_nbr_cell[0].geran_rssi = -98;
  umts_info->umts_geran_nbr_cell[1].geran_arfcn = 39;
  umts_info->umts_geran_nbr_cell[1].geran_bsic_ncc = 255;
  umts_info->umts_geran_nbr_cell[1].geran_bsic_bcc = 255;
  umts_info->umts_geran_nbr_cell[1].geran_rssi = -102;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS (resp)
  RIL_NeighboringCell** neighCells = RIL_node.update_default_ril_request_get_neighboring_cell_ids_resp(2);
  neighCells[0]->cid = "0000019a";  // umts_monitored_cell[0].umts_psc in %08x format
  neighCells[0]->rssi = -71;        // umts_monitored_cell[0].umts_rscp
  neighCells[1]->cid = "0000019a";  // umts_monitored_cell[1].umts_psc in %08x format
  neighCells[1]->rssi = -71;        // umts_monitored_cell[1].umts_rscp
  //RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_neighboring_cell_ids_003(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // GSM case

#if 0
  util_voice_registration_state_for_gsm_success(t, QMI_node, RIL_node,
      RADIO_TECH_GSM, RIL_VAL_REG_REGISTERED_ROAMING,
      NAS_SYS_SRV_STATUS_SRV_V01,
      SYS_SRV_STATUS_SRV_V01,
      SYS_SRV_DOMAIN_CS_ONLY_V01,
      NAS_SYS_ROAM_STATUS_ON_V01,
      FALSE, SYS_SRV_DOMAIN_NO_SRV_V01,
      0, FALSE, NULL, NULL);
#endif

  // inject: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS
  RIL_node.update_default_ril_request_get_neighboring_cell_ids();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_geran_cell_info_type_v01 *geran_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_geran_info();
  geran_info->cell_id = 20332;
  geran_info->plmn[0] = 0;
  geran_info->plmn[1] = 0;
  geran_info->plmn[2] = 0;
  geran_info->lac = 106;
  geran_info->arfcn = 109;
  geran_info->bsic = 23;
  geran_info->timing_advance = 4294967295;
  geran_info->rx_lev = 45;
  geran_info->nmr_cell_info_len = 3;
  geran_info->nmr_cell_info[0].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[0].nmr_plmn = { (null), (null), (null) }
  geran_info->nmr_cell_info[0].nmr_lac = 0;
  geran_info->nmr_cell_info[0].nmr_arfcn = 109;
  geran_info->nmr_cell_info[0].nmr_bsic = 23;
  geran_info->nmr_cell_info[0].nmr_rx_lev = 46;
  geran_info->nmr_cell_info[1].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[1].nmr_plmn = { (null), (null), (null) };
  geran_info->nmr_cell_info[1].nmr_lac = 0;
  geran_info->nmr_cell_info[1].nmr_arfcn = 104;
  geran_info->nmr_cell_info[1].nmr_bsic = 23;
  geran_info->nmr_cell_info[1].nmr_rx_lev = 37;
  geran_info->nmr_cell_info[2].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[2].nmr_plmn = { (null), (null), (null) };
  geran_info->nmr_cell_info[2].nmr_lac = 0;
  geran_info->nmr_cell_info[2].nmr_arfcn = 111;
  geran_info->nmr_cell_info[2].nmr_bsic = 39;
  geran_info->nmr_cell_info[2].nmr_rx_lev = 28;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS (resp)
  RIL_NeighboringCell** neighCells = RIL_node.update_default_ril_request_get_neighboring_cell_ids_resp(3);
//  neighCells[0]->cid = "0000019a";
//  neighCells[0]->rssi = -71;
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_neighboring_cell_ids_004(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // LTE case

#if 0
  util_voice_registration_state_for_lte(t, QMI_node, RIL_node, OEM_node,
      RADIO_TECH_UNKNOWN,
      RIL_VAL_REG_NOT_REGISTERED_SEARCHING,
      NAS_SYS_SRV_STATUS_NO_SRV_V01,
      SYS_SRV_STATUS_NO_SRV_V01,
      SYS_SRV_DOMAIN_CS_ONLY_V01,
      NAS_SYS_ROAM_STATUS_OFF_V01,
      FALSE, FALSE,
      SYS_SRV_DOMAIN_NO_SRV_V01,
      0, FALSE, NULL, NULL, NULL, NULL, FALSE, 0, 0, NULL, FALSE, NAS_TRI_FALSE_V01);
#endif

  // inject: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS
  RIL_node.update_default_ril_request_get_neighboring_cell_ids();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_NEIGHBORING_CELL_IDS (resp)
  RIL_NeighboringCell** neighCells = RIL_node.update_default_ril_request_get_neighboring_cell_ids_resp(3);
//  neighCells[0]->cid = "0000019a";
//  neighCells[0]->rssi = -71;
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_001(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(3);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_002(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(0);
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_003(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_umts_cell_info_type_v01 *umts_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_umts_info();
  umts_info->cell_id = 45035;
  umts_info->plmn[0] = 19;  // 0x13
  umts_info->plmn[1] = 240; // 0xF0
  umts_info->plmn[2] = 0;
  umts_info->lac = 5304;
  umts_info->uarfcn = 10657;
  umts_info->psc = 410;
  umts_info->rscp = -71;
  umts_info->ecio = -2;
  umts_info->umts_monitored_cell_len = 2;
  umts_info->umts_monitored_cell[0].umts_uarfcn = 10657;
  umts_info->umts_monitored_cell[0].umts_psc = 410;
  umts_info->umts_monitored_cell[0].umts_rscp = -71;
  umts_info->umts_monitored_cell[0].umts_ecio = -2;
  umts_info->umts_monitored_cell[1].umts_uarfcn = 10657;
  umts_info->umts_monitored_cell[1].umts_psc = 410;
  umts_info->umts_monitored_cell[1].umts_rscp = -71;
  umts_info->umts_monitored_cell[1].umts_ecio = -2;
  umts_info->umts_geran_nbr_cell_len = 2;
  umts_info->umts_geran_nbr_cell[0].geran_arfcn = 56;
  umts_info->umts_geran_nbr_cell[0].geran_bsic_ncc = 255;
  umts_info->umts_geran_nbr_cell[0].geran_bsic_bcc = 255;
  umts_info->umts_geran_nbr_cell[0].geran_rssi = -98;
  umts_info->umts_geran_nbr_cell[1].geran_arfcn = 39;
  umts_info->umts_geran_nbr_cell[1].geran_bsic_ncc = 255;
  umts_info->umts_geran_nbr_cell[1].geran_bsic_bcc = 255;
  umts_info->umts_geran_nbr_cell[1].geran_rssi = -102;
  t.inject(QMI_node);


  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);



  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(3);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_004(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_geran_cell_info_type_v01 *geran_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_geran_info();
  geran_info->cell_id = 20332;
  geran_info->plmn[0] = 0x31;
  geran_info->plmn[1] = 0x00;
  geran_info->plmn[2] = 0x00;
  geran_info->lac = 106;
  geran_info->arfcn = 109;
  geran_info->bsic = 23;
  geran_info->timing_advance = 4294967295;
  geran_info->rx_lev = 45;
  geran_info->nmr_cell_info_len = 3;
  geran_info->nmr_cell_info[0].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[0].nmr_plmn = { (null), (null), (null) }
  geran_info->nmr_cell_info[0].nmr_lac = 0;
  geran_info->nmr_cell_info[0].nmr_arfcn = 109;
  geran_info->nmr_cell_info[0].nmr_bsic = 23;
  geran_info->nmr_cell_info[0].nmr_rx_lev = 46;
  geran_info->nmr_cell_info[1].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[1].nmr_plmn = { (null), (null), (null) };
  geran_info->nmr_cell_info[1].nmr_lac = 0;
  geran_info->nmr_cell_info[1].nmr_arfcn = 104;
  geran_info->nmr_cell_info[1].nmr_bsic = 23;
  geran_info->nmr_cell_info[1].nmr_rx_lev = 37;
  geran_info->nmr_cell_info[2].nmr_cell_id = 4294967295;
  //geran_info->nmr_cell_info[2].nmr_plmn = { (null), (null), (null) };
  geran_info->nmr_cell_info[2].nmr_lac = 0;
  geran_info->nmr_cell_info[2].nmr_arfcn = 111;
  geran_info->nmr_cell_info[2].nmr_bsic = 39;
  geran_info->nmr_cell_info[2].nmr_rx_lev = 28;
  t.inject(QMI_node);


  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);




  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(3);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_005(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_cdma_cell_info_type_v01 *cdma_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_cdma_info();
  cdma_info->sid = 4;
  cdma_info->nid = 54;
  cdma_info->base_id = 7762;
  cdma_info->refpn = 164;
  cdma_info->base_lat = 0;
  cdma_info->base_long = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_006(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01 *lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01 *lte_inter = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01 *lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01 *lte_wcdma = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);


    // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
    nas_get_lte_cphy_ca_info_resp_msg_v01 *get_cphy_ca_info_resp =
          QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
    get_cphy_ca_info_resp->pcell_info_valid = TRUE;
    get_cphy_ca_info_resp->pcell_info.pci = 354;
    get_cphy_ca_info_resp->pcell_info.freq = 39348;
    get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_100_V01;
    get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
    get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_100_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state = NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
    t.inject(QMI_node);

    // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
    nas_get_rf_band_info_resp_msg_v01 *get_rf_band_info_resp =
          QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
    get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
    get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
    get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
    get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_100_V01;
    get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
    get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
    get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
    get_rf_band_info_resp->rf_band_info_list_ext[0].active_band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
    get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
    t.inject(QMI_node);

    // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
    lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
    lte_intra->ue_in_idle = 0;
    lte_intra->plmn[0] = 19;
    lte_intra->plmn[1] = 240;
    lte_intra->plmn[2] = 0;
    lte_intra->tac = 3;
    lte_intra->global_cell_id = 21505;
    lte_intra->earfcn = 39348;
    lte_intra->serving_cell_id = 354;
    lte_intra->cell_resel_priority = 0;
    lte_intra->s_non_intra_search = 0;
    lte_intra->thresh_serving_low = 0;
    lte_intra->s_intra_search = 0;
    lte_intra->cells_len = 0;
    t.inject(QMI_node);

    // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
    QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
    QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
    QMI_node.set_expect_skip_verification_status(false);
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
    nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
    eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->spn_len = 6;
    strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne")+1);
    eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
    eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
    eons_plmn_name->plmn_short_name_len = 0;
    eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
    eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
    eons_plmn_name->plmn_long_name_len = 0;
    t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_007(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_get_cell_location_info_resp_nr5g_arfcn(504990);
  nas_nr5g_serving_cell_info_type_v01 *nr5g_info = QMI_node.set_qmi_nas_get_cell_location_info_resp_nr5g_serving_cell_info();
  nr5g_info->plmn[0] = 100;
  nr5g_info->plmn[1] = 240;
  nr5g_info->plmn[2] = 4;
  nr5g_info->nr5g_tac_info.tac[0] = 22;
  nr5g_info->nr5g_tac_info.tac[1] = 24;
  nr5g_info->nr5g_tac_info.tac[2] = 10;
  nr5g_info->global_cell_id = 13206302722;
  nr5g_info->pci = 863;
  nr5g_info->rsrq = -110;
  nr5g_info->rsrp = -750;
  nr5g_info->snr = 290;
  t.inject(QMI_node);


    // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(450, 0);
    QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
    QMI_node.set_qmi_nas_get_plmn_name_req_rat(NAS_RADIO_IF_LTE_V01);
    QMI_node.set_expect_skip_verification_status(false);
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
    nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name = QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
    eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->spn_len = 6;
    strlcpy(eons_plmn_name->spn, "CMCC", strlen("CMCC")+1);
    eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
    eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
    eons_plmn_name->plmn_short_name_len = 0;
    eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
    eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
    eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
    eons_plmn_name->plmn_long_name_len = 0;
    t.inject(QMI_node);


    // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
    nas_get_lte_cphy_ca_info_resp_msg_v01 *get_cphy_ca_info_resp =
          QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
    get_cphy_ca_info_resp->pcell_info_valid = TRUE;
    get_cphy_ca_info_resp->pcell_info.pci = 354;
    get_cphy_ca_info_resp->pcell_info.freq = 39348;
    get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_100_V01;
    get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
    get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_100_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state = NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
    get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
    t.inject(QMI_node);

    // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
    nas_get_rf_band_info_resp_msg_v01 *get_rf_band_info_resp =
          QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
    get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
    get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
    get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_NR5G_V01;
    get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_NR5G_BW_NRB_100_V01;
    get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
    get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
    get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_NR5G_V01;
    get_rf_band_info_resp->rf_band_info_list_ext[0].active_band = NAS_ACTIVE_BAND_NR5G_BAND_41_V01;
    get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 504990;
    t.inject(QMI_node);

    // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
    t.inject(QMI_node);


  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  cellInfos[0].cellInfoType = RIL_CELL_INFO_TYPE_NR;
  //RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_008(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01* lte_intra =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01* lte_inter =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01* lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01* lte_wcdma =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
  nas_get_lte_cphy_ca_info_resp_msg_v01* get_cphy_ca_info_resp =
      QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
  get_cphy_ca_info_resp->pcell_info_valid = TRUE;
  get_cphy_ca_info_resp->pcell_info.pci = 354;
  get_cphy_ca_info_resp->pcell_info.freq = 39348;
  get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_75_V01;
  get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
  get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth =
      NAS_LTE_CPHY_CA_BW_NRB_75_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state =
      NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
  nas_get_rf_band_info_resp_msg_v01* get_rf_band_info_resp =
      QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
  get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
  get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_75_V01;
  get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
  get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
  get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_009(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01* lte_intra =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01* lte_inter =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01* lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01* lte_wcdma =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
  nas_get_lte_cphy_ca_info_resp_msg_v01* get_cphy_ca_info_resp =
      QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
  get_cphy_ca_info_resp->pcell_info_valid = TRUE;
  get_cphy_ca_info_resp->pcell_info.pci = 354;
  get_cphy_ca_info_resp->pcell_info.freq = 39348;
  get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_50_V01;
  get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
  get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth =
      NAS_LTE_CPHY_CA_BW_NRB_50_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state =
      NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
  nas_get_rf_band_info_resp_msg_v01* get_rf_band_info_resp =
      QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
  get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
  get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_50_V01;
  get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
  get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
  get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_010(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01* lte_intra =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01* lte_inter =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01* lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01* lte_wcdma =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
  nas_get_lte_cphy_ca_info_resp_msg_v01* get_cphy_ca_info_resp =
      QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
  get_cphy_ca_info_resp->pcell_info_valid = TRUE;
  get_cphy_ca_info_resp->pcell_info.pci = 354;
  get_cphy_ca_info_resp->pcell_info.freq = 39348;
  get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_25_V01;
  get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
  get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth =
      NAS_LTE_CPHY_CA_BW_NRB_25_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state =
      NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
  nas_get_rf_band_info_resp_msg_v01* get_rf_band_info_resp =
      QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
  get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
  get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_25_V01;
  get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
  get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
  get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_011(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01* lte_intra =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01* lte_inter =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01* lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01* lte_wcdma =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
  nas_get_lte_cphy_ca_info_resp_msg_v01* get_cphy_ca_info_resp =
      QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
  get_cphy_ca_info_resp->pcell_info_valid = TRUE;
  get_cphy_ca_info_resp->pcell_info.pci = 354;
  get_cphy_ca_info_resp->pcell_info.freq = 39348;
  get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_15_V01;
  get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
  get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth =
      NAS_LTE_CPHY_CA_BW_NRB_15_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state =
      NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
  nas_get_rf_band_info_resp_msg_v01* get_rf_band_info_resp =
      QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
  get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
  get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_15_V01;
  get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
  get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
  get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_ut_test_get_cell_info_list_012(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  OEMTestnode OEM_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // inject: RIL_REQUEST_GET_CELL_INFO_LIST
  RIL_node.update_default_ril_request_get_cell_info_list();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  nas_lte_intra_freq_type_v01* lte_intra =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 2;
  lte_intra->cells[0].pci = 354;  // Serving cell information
  lte_intra->cells[0].rsrq = -68;
  lte_intra->cells[0].rsrp = -952;
  lte_intra->cells[0].rssi = -692;
  lte_intra->cells[0].srxlev = 0;
  lte_intra->cells[1].pci = 350;  // Neighbouring Cell information
  lte_intra->cells[1].rsrq = -68;
  lte_intra->cells[1].rsrp = -952;
  lte_intra->cells[1].rssi = -692;
  lte_intra->cells[1].srxlev = 0;
  nas_lte_inter_freq_type_v01* lte_inter =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_inter();
  lte_inter->ue_in_idle = 0;
  lte_inter->freqs_len = 2;
  lte_inter->freqs[0].earfcn = 39546;
  lte_inter->freqs[0].threshX_low = 0;
  lte_inter->freqs[0].threshX_high = 0;
  lte_inter->freqs[0].cell_resel_priority = 0;
  lte_inter->freqs[0].cells_len = 1;
  lte_inter->freqs[0].cells[0].pci = 355;
  lte_inter->freqs[0].cells[0].rsrq = -85;
  lte_inter->freqs[0].cells[0].rsrp = -1110;
  lte_inter->freqs[0].cells[0].rssi = -945;
  lte_inter->freqs[0].cells[0].srxlev = 0;
  lte_inter->freqs[1].earfcn = 39150;
  lte_inter->freqs[1].threshX_low = 0;
  lte_inter->freqs[1].threshX_high = 0;
  lte_inter->freqs[1].cell_resel_priority = 0;
  lte_inter->freqs[1].cells_len = 2;
  lte_inter->freqs[1].cells[0].pci = 353;
  lte_inter->freqs[1].cells[0].rsrq = -101;
  lte_inter->freqs[1].cells[0].rsrp = -1018;
  lte_inter->freqs[1].cells[0].rssi = -839;
  lte_inter->freqs[1].cells[0].srxlev = 0;
  lte_inter->freqs[1].cells[1].pci = 47;
  lte_inter->freqs[1].cells[1].rsrq = -166;
  lte_inter->freqs[1].cells[1].rsrp = -1108;
  lte_inter->freqs[1].cells[1].rssi = -839;
  lte_inter->freqs[1].cells[1].srxlev = 0;
  nas_lte_ngbr_gsm_type_v01* lte_gsm = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_gsm();
  lte_gsm->ue_in_idle = 0;
  lte_gsm->freqs_len = 1;
  lte_gsm->freqs[0].cell_resel_priority = 1;  // TODO: add other fields
  lte_gsm->freqs[0].cells_len = 1;
  lte_gsm->freqs[0].cells[0].arfcn = 109;
  lte_gsm->freqs[0].cells[0].cell_id_valid = 1;
  lte_gsm->freqs[0].cells[0].bsic_id = 23;
  lte_gsm->freqs[0].cells[0].rssi = -839;
  nas_lte_ngbr_wcdma_type_v01* lte_wcdma =
      QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_wcdma();
  lte_wcdma->ue_in_idle = 0;
  lte_wcdma->freqs_len = 1;
  lte_wcdma->freqs[0].uarfcn = 10657;  // TODO add other fields
  lte_wcdma->freqs[0].cells_len = 1;
  lte_wcdma->freqs[0].cells[0].psc = 410;
  lte_wcdma->freqs[0].cells[0].cpich_rscp = -71;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_LTE_CPHY_CA_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_lte_cphy_ca_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_LTE_CPHY_CA_INFO_RESP_MSG_V01
  nas_get_lte_cphy_ca_info_resp_msg_v01* get_cphy_ca_info_resp =
      QMI_node.update_default_nas_get_lte_cphy_ca_info_resp_msg();
  get_cphy_ca_info_resp->pcell_info_valid = TRUE;
  get_cphy_ca_info_resp->pcell_info.pci = 354;
  get_cphy_ca_info_resp->pcell_info.freq = 39348;
  get_cphy_ca_info_resp->pcell_info.cphy_ca_dl_bandwidth = NAS_LTE_CPHY_CA_BW_NRB_6_V01;
  get_cphy_ca_info_resp->pcell_info.band = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list_valid = TRUE;
  get_cphy_ca_info_resp->cphy_scell_info_list_len = 1;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.pci = 353;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.freq = 39150;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.cphy_ca_dl_bandwidth =
      NAS_LTE_CPHY_CA_BW_NRB_6_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_info.scell_state =
      NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01;
  get_cphy_ca_info_resp->cphy_scell_info_list[0].scell_idx = 1;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_rf_band_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01
  nas_get_rf_band_info_resp_msg_v01* get_rf_band_info_resp =
      QMI_node.update_default_qmi_nas_get_rf_band_info_resp_msg();
  get_rf_band_info_resp->nas_rf_bandwidth_info_valid = TRUE;
  get_rf_band_info_resp->nas_rf_bandwidth_info_len = 1;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->nas_rf_bandwidth_info[0].bandwidth = NAS_LTE_BW_NRB_6_V01;
  get_rf_band_info_resp->rf_band_info_list_ext_valid = TRUE;
  get_rf_band_info_resp->rf_band_info_list_ext_len = 1;
  get_rf_band_info_resp->rf_band_info_list_ext[0].radio_if = NAS_RADIO_IF_LTE_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_band =
      NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
  get_rf_band_info_resp->rf_band_info_list_ext[0].active_channel = 39348;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_CELL_LOCATION_INFO_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_cell_location_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                  QMI_ERR_NONE_V01);
  lte_intra = QMI_node.set_qmi_nas_get_cell_location_info_resp_lte_intra();
  lte_intra->ue_in_idle = 0;
  lte_intra->plmn[0] = 19;
  lte_intra->plmn[1] = 240;
  lte_intra->plmn[2] = 0;
  lte_intra->tac = 3;
  lte_intra->global_cell_id = 21505;
  lte_intra->earfcn = 39348;
  lte_intra->serving_cell_id = 354;
  lte_intra->cell_resel_priority = 0;
  lte_intra->s_non_intra_search = 0;
  lte_intra->thresh_serving_low = 0;
  lte_intra->s_intra_search = 0;
  lte_intra->cells_len = 0;
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_plmn_name_req_msg(310, 0);
  QMI_node.set_qmi_nas_get_plmn_name_req_suppress_sim_error(TRUE);
  QMI_node.set_qmi_nas_get_plmn_name_req_rat((nas_radio_if_enum_v01)0x05);
  QMI_node.set_expect_skip_verification_status(false);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_PLMN_NAME_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_plmn_name_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  nas_3gpp_eons_plmn_name_type_v01* eons_plmn_name =
      QMI_node.set_qmi_nas_get_plmn_name_resp_eons_plmn_name_3gpp();
  eons_plmn_name->spn_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->spn_len = 6;
  strlcpy(eons_plmn_name->spn, "WirelessOne", strlen("WirelessOne") + 1);
  eons_plmn_name->plmn_short_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_short_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_short_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_short_name_len = 0;
  eons_plmn_name->plmn_long_name_enc = NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  eons_plmn_name->plmn_long_name_ci = NAS_COUNTRY_INITIALS_DO_NOT_ADD_V01;
  eons_plmn_name->plmn_long_spare_bits = NAS_SPARE_BITS_UNKNOWN_V01;
  eons_plmn_name->plmn_long_name_len = 0;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_CELL_INFO_LIST (resp)
  RIL_CellInfo_v12* cellInfos = RIL_node.update_default_ril_request_get_cell_info_list_resp(1);
  RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_system_selection_channels_001(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject : RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS
  RIL_node.update_default_ril_request_get_system_selection_channels();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_V01
  QMI_node.update_default_qmi_nas_get_system_selection_preference_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_system_selection_preference_resp_msg();
  // Set band_pref
  nas_band_pref_mask_type_v01 band_pref{};
  band_pref |= QMI_NAS_GSM_DCS_1800_BAND_V01;
  band_pref |= QMI_NAS_E_GSM_900_BAND_V01;
  band_pref |= QMI_NAS_P_GSM_900_BAND_V01;
  band_pref |= QMI_NAS_GSM_BAND_450_V01;
  band_pref |= QMI_NAS_GSM_BAND_480_V01;
  band_pref |= QMI_NAS_GSM_BAND_750_V01;
  band_pref |= QMI_NAS_GSM_BAND_850_V01;
  band_pref |= QMI_NAS_GSM_BAND_RAILWAYS_900_BAND_V01;
  band_pref |= QMI_NAS_GSM_BAND_PCS_1900_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_EU_J_CH_IMT_2100_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_US_PCS_1900_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_US_1700_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_JAPAN_800_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_EU_2600_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_EU_J_900_BAND_V01;
  band_pref |= QMI_NAS_WCDMA_J_1700_BAND_V01;
  QMI_node.set_qmi_nas_get_system_selection_preference_resp_band_pref(band_pref);
  // Set band_pref_ext
  lte_band_pref_mask_ext_type_v01 lte_band_pref_ext{};
  lte_band_pref_ext.bits_1_64 |= 1ULL;
  lte_band_pref_ext.bits_1_64 |= 1ULL << 1;
  lte_band_pref_ext.bits_1_64 |= 1ULL << 2;
  lte_band_pref_ext.bits_65_128 |= 1ULL;
  lte_band_pref_ext.bits_65_128 |= 1ULL << 1;
  lte_band_pref_ext.bits_65_128 |= 1ULL << 2;
  lte_band_pref_ext.bits_193_256 |= 1ULL << (250 - 193);
  QMI_node.set_qmi_nas_get_system_selection_preference_resp_lte_band_pref_ext(lte_band_pref_ext);

  // Set tdscdma_band_pref
  nas_nr5g_band_pref_mask_type_v01 nr5g_band_pref{};
  nr5g_band_pref.bits_1_64 |= 1ULL;
  nr5g_band_pref.bits_1_64 |= 1ULL << 1;
  nr5g_band_pref.bits_1_64 |= 1ULL << 2;
  nr5g_band_pref.bits_65_128 |= 1ULL;
  nr5g_band_pref.bits_65_128 |= 1ULL << 1;
  nr5g_band_pref.bits_65_128 |= 1ULL << 5;
  nr5g_band_pref.bits_257_320 |= 1ULL;
  nr5g_band_pref.bits_257_320 |= 1ULL << 1;
  nr5g_band_pref.bits_257_320 |= 1ULL << 2;
  QMI_node.set_qmi_nas_get_system_selection_preference_resp_nr5g_band_pref(nr5g_band_pref);
  QMI_node.set_qmi_nas_get_system_selection_preference_resp_result(QMI_RESULT_SUCCESS_V01,
                                                                   QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_SysSelChannels* ptr = RIL_node.update_default_ril_request_get_system_selection_channels_resp();
  ptr->specifiers_length = 4;
  ptr->specifiers[0].radio_access_network = GERAN;
  ptr->specifiers[0].bands_length = 9;
  ptr->specifiers[0].bands.geran_bands[0] = GERAN_BAND_DCS1800;
  ptr->specifiers[0].bands.geran_bands[1] = GERAN_BAND_E900;
  ptr->specifiers[0].bands.geran_bands[2] = GERAN_BAND_P900;
  ptr->specifiers[0].bands.geran_bands[3] = GERAN_BAND_450;
  ptr->specifiers[0].bands.geran_bands[4] = GERAN_BAND_480;
  ptr->specifiers[0].bands.geran_bands[5] = GERAN_BAND_750;
  ptr->specifiers[0].bands.geran_bands[6] = GERAN_BAND_850;
  ptr->specifiers[0].bands.geran_bands[7] = GERAN_BAND_R900;
  ptr->specifiers[0].bands.geran_bands[8] = GERAN_BAND_PCS1900;

  ptr->specifiers[1].radio_access_network = UTRAN;
  ptr->specifiers[1].bands_length = 7;
  ptr->specifiers[1].bands.utran_bands[0] = UTRAN_BAND_1;
  ptr->specifiers[1].bands.utran_bands[1] = UTRAN_BAND_2;
  ptr->specifiers[1].bands.utran_bands[2] = UTRAN_BAND_4;
  ptr->specifiers[1].bands.utran_bands[3] = UTRAN_BAND_19;
  ptr->specifiers[1].bands.utran_bands[4] = UTRAN_BAND_7;
  ptr->specifiers[1].bands.utran_bands[5] = UTRAN_BAND_8;
  ptr->specifiers[1].bands.utran_bands[6] = UTRAN_BAND_9;

  ptr->specifiers[2].radio_access_network = EUTRAN;
  ptr->specifiers[2].bands_length = 7;
  ptr->specifiers[2].bands.eutran_bands[0] = EUTRAN_BAND_1;
  ptr->specifiers[2].bands.eutran_bands[1] = EUTRAN_BAND_2;
  ptr->specifiers[2].bands.eutran_bands[2] = EUTRAN_BAND_3;
  ptr->specifiers[2].bands.eutran_bands[3] = EUTRAN_BAND_65;
  ptr->specifiers[2].bands.eutran_bands[4] = EUTRAN_BAND_66;
  ptr->specifiers[2].bands.eutran_bands[5] = EUTRAN_BAND_67;
  ptr->specifiers[2].bands.eutran_bands[6] = EUTRAN_BAND_250;

  ptr->specifiers[3].radio_access_network = NGRAN;
  ptr->specifiers[3].bands_length = 9;
  ptr->specifiers[3].bands.ngran_bands[0] = NGRAN_BAND_1;
  ptr->specifiers[3].bands.ngran_bands[1] = NGRAN_BAND_2;
  ptr->specifiers[3].bands.ngran_bands[2] = NGRAN_BAND_3;
  ptr->specifiers[3].bands.ngran_bands[3] = NGRAN_BAND_65;
  ptr->specifiers[3].bands.ngran_bands[4] = NGRAN_BAND_66;
  ptr->specifiers[3].bands.ngran_bands[5] = NGRAN_BAND_70;
  ptr->specifiers[3].bands.ngran_bands[6] = NGRAN_BAND_257;
  ptr->specifiers[3].bands.ngran_bands[7] = NGRAN_BAND_258;
  ptr->specifiers[3].bands.ngran_bands[8] = NGRAN_BAND_259;

  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_system_selection_channels_001(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  ril_request_set_sys_channels_t* req =
      RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = false;
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_EX_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_EX_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  QMI_node.update_default_qmi_dms_band_capability_ex_ind();
  QMI_node.set_qmi_dms_band_capability_ex_ind();
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  nas_band_pref_mask_type_v01 *qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = 0;
  lte_band_pref_mask_ext_type_v01 *lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 1ULL; // band 1
  lte_band->bits_65_128 = 1ULL; //band 65
  lte_band->bits_129_192 = 1ULL; //band 129
  lte_band->bits_193_256 = 0;
  nas_nr5g_band_pref_mask_type_v01 *nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;
  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  nas_system_selection_preference_ind_msg_v01* qmiPayload =
      QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  //Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 0;
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_EX_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_EX_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  QMI_node.update_default_qmi_dms_band_capability_ex_ind();
  QMI_node.set_qmi_dms_band_capability_ex_ind();
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = 0;
  lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 1ULL;
  lte_band->bits_65_128 = 1ULL;
  lte_band->bits_129_192 = 1ULL;
  lte_band->bits_193_256 = 0;
  nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;
  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  qmiPayload = QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = MAX_RADIO_ACCESS_SPECIFIERS + 1;  // invalid param
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 1;
  req->specifiers_list.specifiers[0].channels_length = MAX_CHANNELS + 1;  // 33 - invalid param
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 1;
  req->specifiers_list.specifiers[0].radio_access_network = GERAN;
  req->specifiers_list.specifiers[0].bands_length = MAX_BANDS + 1;  // 9 - invalid param
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  RIL_node.set_ril_error_value(RIL_E_INVALID_ARGUMENTS);
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 4;
  req->specifiers_list.specifiers[0].radio_access_network = GERAN;
  req->specifiers_list.specifiers[0].bands_length = 2;
  req->specifiers_list.specifiers[0].bands.geran_bands[0] = GERAN_BAND_450;
  req->specifiers_list.specifiers[0].bands.geran_bands[1] = GERAN_BAND_R900;

  req->specifiers_list.specifiers[1].radio_access_network = UTRAN;
  req->specifiers_list.specifiers[1].bands_length = 4;
  req->specifiers_list.specifiers[1].bands.utran_bands[0] = UTRAN_BAND_2;
  req->specifiers_list.specifiers[1].bands.utran_bands[1] = UTRAN_BAND_5;
  req->specifiers_list.specifiers[1].bands.utran_bands[2] = UTRAN_BAND_9;
  req->specifiers_list.specifiers[1].bands.utran_bands[3] = UTRAN_BAND_19;

  req->specifiers_list.specifiers[2].radio_access_network = EUTRAN;
  req->specifiers_list.specifiers[2].bands_length = 5;
  req->specifiers_list.specifiers[2].bands.eutran_bands[0] = EUTRAN_BAND_1;
  req->specifiers_list.specifiers[2].bands.eutran_bands[1] = EUTRAN_BAND_3;
  req->specifiers_list.specifiers[2].bands.eutran_bands[2] = EUTRAN_BAND_66;
  req->specifiers_list.specifiers[2].bands.eutran_bands[3] = EUTRAN_BAND_127;
  req->specifiers_list.specifiers[2].bands.eutran_bands[4] = EUTRAN_BAND_250;

  req->specifiers_list.specifiers[3].radio_access_network = NGRAN;
  req->specifiers_list.specifiers[3].bands_length = 7;
  req->specifiers_list.specifiers[3].bands.ngran_bands[0] = NGRAN_BAND_12;
  req->specifiers_list.specifiers[3].bands.ngran_bands[1] = NGRAN_BAND_25;
  req->specifiers_list.specifiers[3].bands.ngran_bands[2] = NGRAN_BAND_48;
  req->specifiers_list.specifiers[3].bands.ngran_bands[3] = NGRAN_BAND_65;
  req->specifiers_list.specifiers[3].bands.ngran_bands[4] = NGRAN_BAND_71;
  req->specifiers_list.specifiers[3].bands.ngran_bands[5] = NGRAN_BAND_82;
  req->specifiers_list.specifiers[3].bands.ngran_bands[6] = NGRAN_BAND_260;

  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = QMI_NAS_GSM_BAND_450_V01 | QMI_NAS_GSM_BAND_RAILWAYS_900_BAND_V01;
  *qmi_band |= QMI_NAS_WCDMA_US_PCS_1900_BAND_V01 | QMI_NAS_WCDMA_US_850_BAND_V01 |
               QMI_NAS_WCDMA_J_1700_BAND_V01 | QMI_NAS_WCDMA_JAPAN_800_BAND_V01;

  lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = (1ULL << (EUTRAN_BAND_1 - 1)) | (1ULL << (EUTRAN_BAND_3 - 1));
  lte_band->bits_65_128 = (1ULL << (EUTRAN_BAND_66 - 65)) | (1ULL << (EUTRAN_BAND_127 - 65));
  lte_band->bits_129_192 = 0;
  lte_band->bits_193_256 = 1ULL << (EUTRAN_BAND_250 - 193);

  nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 =
      (1ULL << (NGRAN_BAND_12 - 1)) | (1ULL << (NGRAN_BAND_25 - 1)) | (1ULL << (NGRAN_BAND_48 - 1));
  nr5g_band->bits_65_128 = (1ULL << (NGRAN_BAND_65 - 65)) | (1ULL << (NGRAN_BAND_71 - 65)) |
                           (1ULL << (NGRAN_BAND_82 - 65));
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = (1ULL << (NGRAN_BAND_260 - 257));
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;
  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);

  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  qmiPayload = QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_system_selection_channels_002(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  ril_request_set_sys_channels_t* req =
      RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 2;
  req->specifiers_list.specifiers[0].radio_access_network = GERAN;
  req->specifiers_list.specifiers[0].bands_length = 7;
  req->specifiers_list.specifiers[0].bands.geran_bands[0] = GERAN_BAND_480;
  req->specifiers_list.specifiers[0].bands.geran_bands[1] = GERAN_BAND_T380;
  req->specifiers_list.specifiers[0].bands.geran_bands[2] = GERAN_BAND_750;
  req->specifiers_list.specifiers[0].bands.geran_bands[3] = GERAN_BAND_850;
  req->specifiers_list.specifiers[0].bands.geran_bands[4] = GERAN_BAND_P900;
  req->specifiers_list.specifiers[0].bands.geran_bands[5] = GERAN_BAND_T410;
  req->specifiers_list.specifiers[0].bands.geran_bands[6] = GERAN_BAND_710;

  req->specifiers_list.specifiers[1].radio_access_network = UTRAN;
  req->specifiers_list.specifiers[1].bands_length = 8;
  req->specifiers_list.specifiers[1].bands.utran_bands[0] = UTRAN_BAND_1;
  req->specifiers_list.specifiers[1].bands.utran_bands[1] = UTRAN_BAND_3;
  req->specifiers_list.specifiers[1].bands.utran_bands[2] = UTRAN_BAND_4;
  req->specifiers_list.specifiers[1].bands.utran_bands[3] = UTRAN_BAND_6;
  req->specifiers_list.specifiers[1].bands.utran_bands[4] = UTRAN_BAND_10;
  req->specifiers_list.specifiers[1].bands.utran_bands[5] = UTRAN_BAND_11;
  req->specifiers_list.specifiers[1].bands.utran_bands[6] = UTRAN_BAND_12;
  req->specifiers_list.specifiers[1].bands.utran_bands[7] = UTRAN_BAND_13;

  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  nas_band_pref_mask_type_v01* qmi_band =
      QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = QMI_NAS_GSM_BAND_480_V01 | QMI_NAS_GSM_BAND_750_V01 | QMI_NAS_GSM_BAND_850_V01 |
              QMI_NAS_P_GSM_900_BAND_V01;
  *qmi_band |= QMI_NAS_WCDMA_EU_J_CH_IMT_2100_BAND_V01 | QMI_NAS_EU_CH_DCS_1800_BAND_V01 |
               QMI_NAS_WCDMA_US_1700_BAND_V01;

  lte_band_pref_mask_ext_type_v01* lte_band =
      QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 0;
  lte_band->bits_65_128 = 0;
  lte_band->bits_129_192 = 0;
  lte_band->bits_193_256 = 0;
  nas_nr5g_band_pref_mask_type_v01* nr5g_band =
      QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;

  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);

  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  nas_system_selection_preference_ind_msg_v01* qmiPayload =
      QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = true;
  req->specifiers_list.specifiers_length = 2;
  req->specifiers_list.specifiers[0].radio_access_network = GERAN;
  req->specifiers_list.specifiers[0].bands_length = 5;
  req->specifiers_list.specifiers[0].bands.geran_bands[0] = GERAN_BAND_E900;
  req->specifiers_list.specifiers[0].bands.geran_bands[1] = GERAN_BAND_PCS1900;
  req->specifiers_list.specifiers[0].bands.geran_bands[2] = GERAN_BAND_T810;
  req->specifiers_list.specifiers[0].bands.geran_bands[3] = GERAN_BAND_DCS1800;
  req->specifiers_list.specifiers[0].bands.geran_bands[4] = GERAN_BAND_ER900;

  req->specifiers_list.specifiers[1].radio_access_network = UTRAN;
  req->specifiers_list.specifiers[1].bands_length = 8;
  req->specifiers_list.specifiers[1].bands.utran_bands[0] = UTRAN_BAND_7;
  req->specifiers_list.specifiers[1].bands.utran_bands[1] = UTRAN_BAND_8;
  req->specifiers_list.specifiers[1].bands.utran_bands[2] = UTRAN_BAND_14;
  req->specifiers_list.specifiers[1].bands.utran_bands[3] = UTRAN_BAND_20;
  req->specifiers_list.specifiers[1].bands.utran_bands[4] = UTRAN_BAND_21;
  req->specifiers_list.specifiers[1].bands.utran_bands[5] = UTRAN_BAND_22;
  req->specifiers_list.specifiers[1].bands.utran_bands[6] = UTRAN_BAND_25;
  req->specifiers_list.specifiers[1].bands.utran_bands[7] = UTRAN_BAND_26;

  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = QMI_NAS_E_GSM_900_BAND_V01 | QMI_NAS_GSM_BAND_PCS_1900_BAND_V01;
  *qmi_band |= QMI_NAS_WCDMA_EU_2600_BAND_V01 | QMI_NAS_WCDMA_EU_J_900_BAND_V01;

  lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 0;
  lte_band->bits_65_128 = 0;
  lte_band->bits_129_192 = 0;
  lte_band->bits_193_256 = 0;
  nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;

  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);

  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  qmiPayload = QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_system_selection_channels_003(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  ril_request_set_sys_channels_t *req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = false;
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_EX_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_EX_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_NOT_SUPPORTED_V01);
  t.inject(QMI_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01,
          0);
  QMI_node.set_qmi_dms_get_band_capability_resp_supported_lte_bands();
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  nas_band_pref_mask_type_v01 *qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = 0;
  lte_band_pref_mask_ext_type_v01 *lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 1ULL; // band 1
  lte_band->bits_65_128 = 1ULL; //band 65
  lte_band->bits_129_192 = 1ULL; //band 129
  lte_band->bits_193_256 = 0;
  nas_nr5g_band_pref_mask_type_v01 *nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;
  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  nas_system_selection_preference_ind_msg_v01* qmiPayload = QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  qmiPayload->band_pref_valid = true;
  qmiPayload->lte_band_pref_ext_valid = true;
  qmiPayload->nr5g_sa_band_pref_valid = true;
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_system_selection_channels_004(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  ril_request_set_sys_channels_t *req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = false;
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_EX_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_EX_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_NOT_SUPPORTED_V01);
  t.inject(QMI_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01,
          0);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_set_system_selection_channels_005(user_specified_attrib_t attr)
{
  test_result ret;
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__func__);
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
   test_result ret;
   return ret;
  }

  // inject: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
  ril_request_set_sys_channels_t *req = RIL_node.update_default_ril_request_set_system_selection_channels();
  req->specifyChannels = false;
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_EX_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_EX_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_ex_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_NOT_SUPPORTED_V01);
  t.inject(QMI_node);

  // Expect: QMI_DMS_GET_BAND_CAPABILITY_REQ_V01
  QMI_node.update_default_qmi_dms_get_band_capability_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_BAND_CAPABILITY_RESP_V01
  QMI_node.update_default_qmi_dms_get_band_capability_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01,
          0);
  QMI_node.set_qmi_dms_get_band_capability_resp_supported_lte_bands();
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  nas_band_pref_mask_type_v01 *qmi_band = QMI_node.set_qmi_nas_system_selection_preference_req_band_pref();
  *qmi_band = 0;
  lte_band_pref_mask_ext_type_v01 *lte_band = QMI_node.set_qmi_nas_system_selection_preference_req_lte_band_pref();
  lte_band->bits_1_64 = 1ULL; // band 1
  lte_band->bits_65_128 = 1ULL; //band 65
  lte_band->bits_129_192 = 1ULL; //band 129
  lte_band->bits_193_256 = 0;
  nas_nr5g_band_pref_mask_type_v01 *nr5g_band = QMI_node.set_qmi_nas_system_selection_preference_req_nr5g_band_pref();
  nr5g_band->bits_1_64 = 0;
  nr5g_band->bits_65_128 = 0;
  nr5g_band->bits_129_192 = 0;
  nr5g_band->bits_193_256 = 0;
  nr5g_band->bits_257_320 = 0;
  nr5g_band->bits_321_384 = 0;
  nr5g_band->bits_385_448 = 0;
  nr5g_band->bits_449_512 = 0;
  QMI_node.set_qmi_nas_system_selection_preference_req_change_duration(NAS_PERMANENT_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_FAILURE_V01, QMI_ERR_GENERAL_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS (resp)
  RIL_node.update_default_ril_request_set_system_selection_channels_resp();
  RIL_node.set_ril_error_value(RIL_E_MODEM_ERR);
  t.expect(RIL_node);

  // Execute the test case
  t.execute();

  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_radio_power_with_device_state_ready(user_specified_attrib_t attr)
{
  Testcase t;
  AndroidTelTestnode RIL_node;

  t.set_boot_case(qmi_ril_test_bootup_002);
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("RadioPower");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_NV_RESET_CONFIG
  RIL_node.update_default_ril_request_nv_reset_config_with_type(0);  // RELOAD
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_NV_RESET_CONFIG (resp)
  RIL_node.update_default_ril_request_nv_reset_config_resp();
  RIL_node.set_ril_error_value(RIL_E_REQUEST_NOT_SUPPORTED);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_radio_power_with_RF_failure(user_specified_attrib_t attr)
{
  Testcase t;
  AndroidTelTestnode RIL_node;

  t.set_boot_case(qmi_ril_test_bootup_003);
  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("RadioPower");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_NV_RESET_CONFIG
  RIL_node.update_default_ril_request_nv_reset_config_with_type(0);  // RELOAD
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_NV_RESET_CONFIG (resp)
  RIL_node.update_default_ril_request_nv_reset_config_resp();
  RIL_node.set_ril_error_value(RIL_E_REQUEST_NOT_SUPPORTED);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_qcril_req_hook_get_csg_id_001
@description:
==**/
test_result ril_utf_test_qcril_req_hook_get_csg_id_001(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEMHOOK");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  db_property_set("persist.vendor.radio.fill_eons", "1");
  db_property_set("persist.vendor.radio.csg_info_avlbl", "0");

  util_voice_registration_state_for_lte(t,
                                        QMI_node,
                                        RIL_node,
                                        OEM_node,
                                        RADIO_TECH_LTE,
                                        RIL_VAL_REG_REGISTERED_HOME_NET,
                                        NAS_SYS_SRV_STATUS_SRV_V01,
                                        SYS_SRV_STATUS_SRV_V01,
                                        SYS_SRV_DOMAIN_CS_ONLY_V01,
                                        NAS_SYS_ROAM_STATUS_OFF_V01,
                                        FALSE,
                                        FALSE,
                                        SYS_SRV_DOMAIN_NO_SRV_V01,
                                        0,
                                        TRUE,
                                        "123",
                                        "45",
                                        NULL,
                                        NULL,
                                        TRUE,
                                        22,
                                        0,
                                        NULL,
                                        FALSE,
                                        NAS_TRI_FALSE_V01);

  //  util_to_get_operator_name_3gpp(t, QMI_node, RIL_node, "123", "45", FALSE, 0,
  //  NAS_RADIO_IF_LTE_V01, "Vodafone", NULL, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);

  OEM_node.update_default_qcril_req_hook_get_csg_id_req();
  t.inject(OEM_node);

  OEM_node.update_default_qcril_req_hook_get_csg_id_resp(22);
  t.expect(OEM_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_qcril_req_hook_preferred_network_band_pref_001
@description:
==**/
test_result ril_utf_test_qcril_req_hook_preferred_network_band_pref_001(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEMHOOK");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  OEM_node.update_default_qcril_req_hook_get_preferred_network_band_pref_req(2);  // LTE_BAND
  t.inject(OEM_node);

  OEM_node.update_default_qcril_req_hook_get_preferred_network_band_pref_resp(0);  // NONE
  t.expect(OEM_node);

  OEM_node.update_default_qcril_req_hook_set_preferred_network_band_pref_req(1);  // LTE_FULL
  t.inject(OEM_node);

  OEM_node.update_default_qcril_req_hook_set_preferred_network_band_pref_resp();
  t.expect(OEM_node);

  OEM_node.update_default_qcril_req_hook_get_preferred_network_band_pref_req(2);  // LTE_BAND
  t.inject(OEM_node);

  OEM_node.update_default_qcril_req_hook_get_preferred_network_band_pref_resp(0);  // NONE
  t.expect(OEM_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_qcril_req_hook_set_ui_ready_001
@description:
==**/
test_result ril_utf_test_qcril_req_hook_set_ui_ready_001(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEMHOOK");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  OEM_node.update_default_qcril_req_hook_set_atel_ui_status_req(1);
  t.inject(OEM_node);

  OEM_node.update_default_qcril_req_hook_set_atel_ui_status_resp();
  t.expect(OEM_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_qcril_req_hook_csg_perform_nw_scan_001
@description:
==**/
test_result ril_utf_test_qcril_req_hook_csg_perform_nw_scan_001(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEMHOOK");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  uint8_t reqData[100];
  reqData[0] = 0x10;  // network type
  reqData[1] = 0x01;
  reqData[2] = 0x00;
  reqData[3] = 0x04;  // NAS_NETWORK_TYPE_LTE_ONLY_V01
  reqData[4] = 0x11;  // scan type
  reqData[5] = 0x01;  // len
  reqData[6] = 0x00;  // len
  reqData[7] = 0x01;  // NAS_SCAN_TYPE_CSG_V01
  OEM_node.update_default_qcril_req_hook_csg_perform_nw_scan_req(reqData, 8);
  t.inject(OEM_node);

  // Expect: QMI_NAS_PERFORM_NETWORK_SCAN_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_perfrom_network_scan_req_msg();
  QMI_node.set_qmi_nas_perform_network_scan_req_msg_network_type(NAS_NETWORK_TYPE_LTE_ONLY_V01);
  QMI_node.set_qmi_nas_perform_network_scan_req_msg_scan_type(NAS_SCAN_TYPE_CSG_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_PERFORM_NETWORK_SCAN_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_perform_network_scan_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                QMI_ERR_NONE_V01);
  QMI_node.set_qmi_nas_scan_result_tlv_network_scan_resp_msg(NAS_SCAN_SUCCESS_V01);
  uint16_t name[3];
  name[0] = 'a';
  name[1] = 'b';
  name[2] = 'c';
  QMI_node.set_qmi_nas_csg_nw_info_tlv_network_scan_resp_msg(
      0x10, 0x11, NAS_CSG_LIST_CAT_ALLOWED_V01, 0x12, 3, name);
  QMI_node.set_qmi_nas_csg_sig_info_tlv_network_scan_resp_msg(0x10, 0x11, 0x12, 0x5);
  t.inject(QMI_node);

  uint8_t respData[100];
  respData[0] = 0x13;  // SCAN_RESULT_TYPE
  respData[1] = 0x04;  // len 4
  respData[2] = 0x00;  // len 4
  respData[3] = 0x00;
  respData[4] = 0x00;
  respData[5] = 0x00;
  respData[6] = 0x00;

  respData[7] = 0x14;   // CSG_INFO_TYPE
  respData[8] = 0x15;   // len 21
  respData[9] = 0x00;   // len
  respData[10] = 0x01;  // number of instances
  respData[11] = 0x10;  // mcc 0x10
  respData[12] = 0x00;  // mcc
  respData[13] = 0x11;  // mnc 0x11
  respData[14] = 0x00;  // mnc
  respData[15] = 0x00;  // mnc_includes_pcs_digit 0
  respData[16] = 0x01;  // csg_list_cat 0x01
  respData[17] = 0x00;  // csg_list_cat
  respData[18] = 0x00;  // csg_list_cat
  respData[19] = 0x00;  // csg_list_cat
  respData[20] = 0x12;  // csg_info.id 0x12
  respData[21] = 0x00;  // csg_info.id
  respData[22] = 0x00;  // csg_info.id
  respData[23] = 0x00;  // csg_info.id
  respData[24] = 0x03;  // csg_info.name_len 0x03
  respData[25] = 0x61;  // csg_info.name 'a'
  respData[26] = 0x00;  // csg_info.name
  respData[27] = 0x62;  // csg_info.name 'b'
  respData[28] = 0x00;  // csg_info.name
  respData[29] = 0x63;  // csg_info.name 'c'
  respData[30] = 0x00;  // csg_info.name

  respData[31] = 0x15;  // CSG_SIG_INFO_TYPE
  respData[32] = 0x0E;  // len 14
  respData[33] = 0x00;  // len
  respData[34] = 0x01;  // number of instances
  respData[35] = 0x10;  // mcc 0x10
  respData[36] = 0x00;  // mcc
  respData[37] = 0x11;  // mnc 0x11
  respData[38] = 0x00;  // mnc
  respData[39] = 0x00;  // mnc_includes_pcs_digit 0
  respData[40] = 0x12;  // csg_id 0x12
  respData[41] = 0x00;  // csg_id
  respData[42] = 0x00;  // csg_id
  respData[43] = 0x00;  // csg_id
  respData[44] = 0x05;  // signal_strength 0x05
  respData[45] = 0x00;  // signal_strength
  respData[46] = 0x00;  // signal_strength
  respData[47] = 0x00;  // signal_strength

  OEM_node.update_default_qcril_req_hook_csg_perform_nw_scan_resp(respData, 48);
  t.expect(OEM_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_qcril_req_hook_csg_set_sys_sel_pref_001
@description:
==**/
test_result ril_utf_test_qcril_req_hook_csg_set_sys_sel_pref_001(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("OEMHOOK");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF
  uint8_t reqData[100];
  reqData[0] = 0x20;   // QCRIL_OEMHOOK_CSG_TAG_CSG_INFO
  reqData[1] = 0x0A;   // len 10
  reqData[2] = 0x00;   // len
  reqData[3] = 0x10;   // mcc 0x10
  reqData[4] = 0x00;   // mcc
  reqData[5] = 0x11;   // mnc 0x11
  reqData[6] = 0x00;   // mnc
  reqData[7] = 0x01;   // mnc_includes_pcs_digit
  reqData[8] = 0x12;   // id 0x12
  reqData[9] = 0x00;   // id
  reqData[10] = 0x00;  // id
  reqData[11] = 0x00;  // id
  reqData[12] = 0x08;  // rat 0x08 RADIO_IF_LTE
  OEM_node.update_default_qcril_req_hook_csg_set_sys_sel_pref_req(reqData, 13);
  t.inject(OEM_node);

  // Expect: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_system_selection_preference_req_msg();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_get_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                           QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  nas_csg_nw_iden_type_v01* csg_info =
      QMI_node.set_qmi_nas_system_selection_preference_req_csg_info();
  csg_info->mcc = 0x10;
  csg_info->mnc = 0x11;
  csg_info->mnc_includes_pcs_digit = 0x01;
  csg_info->id = 0x12;
  csg_info->rat = NAS_RADIO_IF_LTE_V01;
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01
  QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
  nas_csg_nw_iden_type_v01* ind_csg_info =
      QMI_node.set_qmi_nas_system_selection_preference_ind_csg_info();
  ind_csg_info->mcc = 0x10;
  ind_csg_info->mnc = 0x11;
  ind_csg_info->mnc_includes_pcs_digit = 0x01;
  ind_csg_info->id = 0x12;
  ind_csg_info->rat = NAS_RADIO_IF_LTE_V01;
  t.inject(QMI_node);

  // Expect: QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF (Resp)
  OEM_node.update_default_qcril_req_hook_csg_set_sys_sel_pref_resp();
  t.expect(OEM_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_imei_001(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_attribute("DMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_GET_IMEI
  RIL_node.update_default_ril_request_get_imei();
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_IMEI_INFO_REQ_V01
  QMI_node.update_default_qmi_dms_get_imei_info();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_IMEI_INFO_RESP_V01
  QMI_node.update_default_qmi_dms_get_imei_info_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_DMS_IMEI_INFO_IND_V01
  uint8_t imei[4] = { '9', '9', '9', '\0' };
  dms_imei_info_ind_msg_v01* ind_payload =
      QMI_node.update_default_qmi_dms_imei_info_ind(imei, 3, DMS_PRIMARY_IMEI_V01);
  ind_payload->tx_id_valid = TRUE;
  ind_payload->tx_id = 0;
  t.inject(QMI_node);

  // Expect: QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01
  QMI_node.update_default_qmi_dms_get_device_serial_numbers_req();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_RESP_V01
  QMI_node.update_deafult_qmi_dms_get_device_serial_numbers_resp_msg(
          QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  char *esn    = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_esn();
  char *imeival   = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_imei();
  char *imeid  = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_meid();
  char *imeisv = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_imeisv_svn();
  strlcpy(esn,"12345678", QMI_DMS_ESN_MAX_V01);
  strlcpy(imeival,"999", QMI_DMS_IMEI_MAX_V01);
  strlcpy(imeid,"98765432100000", QMI_DMS_MEID_MAX_V01);
  strlcpy(imeisv,"01", QMI_DMS_IMEISV_MAX_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_IMEI
  RIL_node.update_default_ril_request_get_imei_resp(imei, 3, ImeiType::IMEI_TYPE_PRIMARY);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_imei_002(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_attribute("DMS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_GET_IMEI
  RIL_node.update_default_ril_request_get_imei();
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_IMEI_INFO_REQ_V01
  QMI_node.update_default_qmi_dms_get_imei_info();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_IMEI_INFO_RESP_V01
  QMI_node.update_default_qmi_dms_get_imei_info_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Inject: QMI_DMS_IMEI_INFO_IND_V01
  uint8_t imei[4] = { '9', '9', '9', '\0' };
  dms_imei_info_ind_msg_v01* ind_payload =
      QMI_node.update_default_qmi_dms_imei_info_ind(imei, 3, DMS_SECONDARY_IMEI_V01);
  ind_payload->tx_id_valid = TRUE;
  ind_payload->tx_id = 0;
  t.inject(QMI_node);

  // Expect: QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01
  QMI_node.update_default_qmi_dms_get_device_serial_numbers_req();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_RESP_V01
  QMI_node.update_deafult_qmi_dms_get_device_serial_numbers_resp_msg(
          QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  char *esn    = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_esn();
  char *imeival   = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_imei();
  char *imeid  = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_meid();
  char *imeisv = QMI_node.set_qmi_dms_get_device_serial_numbers_resp_imeisv_svn();
  strlcpy(esn,"12345678", QMI_DMS_ESN_MAX_V01);
  strlcpy(imeival,"999", QMI_DMS_IMEI_MAX_V01);
  strlcpy(imeid,"98765432100000", QMI_DMS_MEID_MAX_V01);
  strlcpy(imeisv,"01", QMI_DMS_IMEISV_MAX_V01);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_IMEI
  RIL_node.update_default_ril_request_get_imei_resp(imei, 3, ImeiType::IMEI_TYPE_SECONDARY);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}
test_result ril_utf_test_get_cdma_subscription(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("GET_CDMA_SUB");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_CDMA_SUBSCRIPTION
  RIL_node.update_default_ril_request_get_cdma_sub();
  t.inject(RIL_node);

  // Expect: QMI_DMS_GET_CURRENT_PRL_INFO_REQ_V01
  QMI_node.update_default_qmi_dms_get_current_prl_info_req_v01();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_CURRENT_PRL_INFO_RESP_V01
  QMI_node.update_default_qmi_dms_get_current_prl_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                QMI_ERR_NONE_V01);
  QMI_node.set_qmi_dms_get_current_prl_info_resp_prl_version(60);
  t.inject(QMI_node);

  // Expect: QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_get_3gpp2_subscription_info_req_msg(0xFF);
  QMI_node.set_qmi_nas_get_3gpp2_subscription_info_req_get_3gpp2_info_mask(
      QMI_NAS_GET_3GPP2_SUBS_INFO_MDN_V01 | QMI_NAS_GET_3GPP2_SUBS_INFO_HOME_SID_IND_V01 |
      QMI_NAS_GET_3GPP2_SUBS_INFO_MIN_BASED_IMSI_V01);
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_RESP_MSG_V01
  QMI_node.upate_default_qmi_nas_get_3gpp2_subscription_info_resp_msg(QMI_RESULT_SUCCESS_V01,
                                                                      QMI_ERR_NONE_V01);

  // Setup Payload

  // NAM name
  QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_nam_name("nom");
  // Directory Number
  snprintf(QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_dir_num(3), 3, "12");
  // Home SID/NID info
  nas_3gpp2_home_sid_nid_info_type_v01* sid_info =
      QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_cdma_sys_id(3);
  sid_info->sid = 101;
  sid_info->nid = 102;

  // // MIN-based IMSI
  nas_3gpp2_min_based_info_type_v01* min_based =
      QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_min_based_info();
  snprintf(min_based->mcc_m, 4, "000");
  snprintf(min_based->imsi_m_11_12, 3, "00");
  snprintf(min_based->imsi_m_s1, 8, "0123456");
  snprintf(min_based->imsi_m_s2, 4, "012");

  // True IMSI
  nas_3gpp2_true_imsi_info_type_v01 true_imsi =
      QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_true_imsi();
  snprintf(true_imsi.mcc_t, 4, "000");
  snprintf(true_imsi.imsi_t_11_12, 3, "00");
  snprintf(true_imsi.imsi_t_s1, 8, "0123456");
  snprintf(true_imsi.imsi_t_s2, 4, "012");
  true_imsi.imsi_t_addr_num = 12;

  // CDMA Channel
  nas_cdma_channel_info_type_v01 cdma_channel =
      QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_cdma_channel_info();
  cdma_channel.pri_ch_a = 1;
  cdma_channel.pri_ch_b = 1;
  cdma_channel.sec_ch_a = 1;
  cdma_channel.sec_ch_b = 1;

  // MDN
  snprintf(QMI_node.set_qmi_nas_get_3gpp2_subscription_info_resp_mdn(4), 4, "123");

  t.inject(QMI_node);

  // Expect: RIL_REQUEST_CDMA_SUBSCRIPTION
  char mdn[15];
  snprintf(mdn, 15, "123");
  qcril::interfaces::RilGetCdmaSubscriptionResult_t* resp =
      RIL_node.update_default_ril_request_get_cdma_sub_resp(RIL_E_SUCCESS);
  resp->mdn = std::string(mdn, 15);
  resp->hSid = "101,0,0";
  resp->hNid = "102,0,0";
  resp->min = "0120123456";
  resp->prl = "60";
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_uicc_app_status(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  OEMTestnode OEM_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_description("GET_UICC_APP_STATUS");
  t.set_test_attribute("NAS");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_SET_UICC_SUBSCRIPTION
  RIL_SelectUiccSub* uicc_subs_data = RIL_node.update_default_ril_request_set_uicc_subscription();
  RIL_node.set_ril_request_set_uicc_subscription_app_index(1);
  RIL_node.set_ril_request_set_uicc_subscription_slot(0);
  RIL_node.set_ril_request_set_uicc_subscription_act_status(RIL_UICC_SUBSCRIPTION_ACTIVATE);
  RIL_node.set_ril_request_set_uicc_subscription_sub_type(RIL_SUBSCRIPTION_1);
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_SET_UICC_SUBSCRIPTION
  RIL_node.update_default_ril_request_set_uicc_subscription_resp();
  RIL_node.set_ril_error_value(RIL_E_REQUEST_NOT_SUPPORTED);
  t.expect(RIL_node);

  // Inject: RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT
  RIL_node.update_default_ril_request_get_uicc_applications_enablement();
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT
  RIL_node.update_default_ril_request_get_uicc_applications_enablement_resp(false,
                                                                            RIL_E_GENERIC_FAILURE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_voice_radio_tech(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  IMSTestnode IMS_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("GET_VOICE_RADIO_TECH");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_VOICE_RADIO_TECH
  RIL_node.update_default_ril_request_voice_radio_tech();
  t.inject(RIL_node);

  // Expect: RIL_REQUEST_VOICE_RADIO_TECH
  RIL_RadioTechnology* resp = RIL_node.update_default_ril_request_voice_radio_tech_resp();
  *resp = RIL_RadioTechnology::RADIO_TECH_UMTS;
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

/**==
@testname: ril_utf_test_voice_registration_state_testcase_47
@description: This test is to verify the "voice_registration_state" for LTE in full service with
primary plmn info available.
==**/
test_result ril_utf_test_voice_registration_state_testcase_47(user_specified_attrib_t attr)
{
  test_result ret;

  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("Voice_Registration_State");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  nas_cag_id_name_type_v01 cag_id_name{};
  cag_id_name.id = 505;
  cag_id_name.name_len = 3;
  cag_id_name.name[0] = 'a';
  cag_id_name.name[1] = 'b';
  cag_id_name.name[2] = 'c';
  cag_id_name.name[3] = 0;

  nas_snpn_id_name_type_v01 snpn_id_name{};
  snpn_id_name.nid[0] = 1;
  snpn_id_name.nid[1] = 2;
  snpn_id_name.nid[2] = 3;
  snpn_id_name.nid[3] = 4;
  snpn_id_name.nid[4] = 5;
  snpn_id_name.nid[5] = 6;
  snpn_id_name.name_len = 3;
  snpn_id_name.name[0] = 'A';
  snpn_id_name.name[1] = 'B';
  snpn_id_name.name[2] = 'C';
  snpn_id_name.name[3] = 0;

  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "123",
                                         "45",
                                         "124",
                                         "67",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         &cag_id_name,
                                         nullptr);  //&snpn_id_name

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

void util_test_set_nr_config(Testcase& t,
                             AndroidTelTestnode& RIL_node,
                             QMIRILTestnode& QMI_node,
                             RIL_NR_DISABLE_MODE ril_mode,
                             qmi_result_type_v01 result,
                             qmi_error_type_v01 error,
                             RIL_Errno err)
{
  nas_nr5g_disable_mode_enum_type_v01 nas_mode;

  switch (ril_mode) {
    case NR_DISABLE_MODE_NONE:
      nas_mode = NAS_NR5G_DISABLE_MODE_NONE_V01;
      break;
    case NR_DISABLE_MODE_SA:
      nas_mode = NAS_NR5G_DISABLE_MODE_SA_V01;
      break;
    case NR_DISABLE_MODE_NSA:
      nas_mode = NAS_NR5G_DISABLE_MODE_NSA_V01;
      break;
    default:
      nas_mode = NAS_NR5G_DISABLE_MODE_NONE_V01;
      break;
  }

  // Inject: RIL_REQUEST_SET_NR_DISABLE_MODE
  RIL_node.update_default_ril_request_set_nr_disable_mode(ril_mode);
  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
  nas_set_system_selection_preference_req_msg_v01* pl =
      QMI_node.update_default_qmi_nas_set_system_selection_preference_req_msg();
  pl->nr5g_disable_mode_valid = TRUE;
  pl->nr5g_disable_mode = nas_mode;
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_set_system_selection_preference_resp_msg(result, error);
  t.inject(QMI_node);

  if (result == QMI_RESULT_SUCCESS_V01) {  // Success case means wait for indication
    nas_system_selection_preference_ind_msg_v01* qmiIndPayload =
        QMI_node.update_default_qmi_nas_system_selection_preference_ind_msg();
    qmiIndPayload->nr5g_disable_mode_valid = TRUE;
    qmiIndPayload->nr5g_disable_mode = nas_mode;
    t.inject(QMI_node);
  }

  // Expect: RIL_REQUEST_SET_NR_DISABLE_MODE
  RIL_node.update_default_ril_request_set_nr_disable_mode_resp(1);  // Always responds with success
  RIL_node.set_ril_error_value(err);
  t.expect(RIL_node);
}

void util_test_query_nr_config(Testcase& t,
                               AndroidTelTestnode& RIL_node,
                               RIL_NR_DISABLE_MODE ril_mode,
                               bool do_fetch)
{
  // Inject: RIL_REQUEST_QUERY_NR_DISABLE_MODE
  RIL_node.update_default_ril_request_query_nr_disable_mode();
  t.inject(RIL_node);

  if (do_fetch) {
    QMIRILTestnode QMI_node;
    nas_nr5g_disable_mode_enum_type_v01 nas_mode;

    switch (ril_mode) {
      case NR_DISABLE_MODE_NONE:
        nas_mode = NAS_NR5G_DISABLE_MODE_NONE_V01;
        break;
      case NR_DISABLE_MODE_SA:
        nas_mode = NAS_NR5G_DISABLE_MODE_SA_V01;
        break;
      case NR_DISABLE_MODE_NSA:
        nas_mode = NAS_NR5G_DISABLE_MODE_NSA_V01;
        break;
      default:
        nas_mode = NAS_NR5G_DISABLE_MODE_NONE_V01;
        break;
    }

    // Expect: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01
    QMI_node.update_default_qmi_nas_get_system_selection_preference_req_msg();
    t.expect(QMI_node);

    // Inject: QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP_MSG_V01
    QMI_node.update_default_qmi_nas_get_system_selection_preference_resp_msg();
    QMI_node.set_qmi_nas_get_system_selection_preference_resp_nr5g_disable_mode(nas_mode);
    t.inject(RIL_node);
  }

  // Expect: RIL_REQUEST_QUERY_NR_DISABLE_MODE
  RIL_node.update_default_ril_request_query_nr_disable_mode_resp(ril_mode);
  t.expect(RIL_node);
}

test_result ril_utf_test_default_get_nr_config(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("TEST_GET_NR_CONFIG");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_test_query_nr_config(t, RIL_node, NR_DISABLE_MODE_NONE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_default_set_and_get_all_nr_config(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("TEST_GET_SET_NR_CONFIG");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Check default is NR_DISABLE_MODE_NONE
  util_test_query_nr_config(t, RIL_node, NR_DISABLE_MODE_NONE);

  // Now set to NR_DISABLE_MODE_SA
  util_test_set_nr_config(t, RIL_node, QMI_node, NR_DISABLE_MODE_SA);

  // Verify properly set
  util_test_query_nr_config(t, RIL_node, NR_DISABLE_MODE_SA);

  // Now set to NR_DISABLE_MODE_NSA
  util_test_set_nr_config(t, RIL_node, QMI_node, NR_DISABLE_MODE_NSA);

  // Verify properly Set
  util_test_query_nr_config(t, RIL_node, NR_DISABLE_MODE_NSA);

  // Set back to default NR_DISABLE_MODE_NONE
  util_test_set_nr_config(t, RIL_node, QMI_node, NR_DISABLE_MODE_NONE);

  // Verify properly Set
  util_test_query_nr_config(t, RIL_node, NR_DISABLE_MODE_NONE);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_default_fail_set_nr_config(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("TEST_FAIL_SET_NR_CFG");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Vector mapping QMI errors to the RIL errors they will return
  std::vector<std::pair<qmi_error_type_v01, RIL_Errno>> qmiErrors = {
    { QMI_ERR_NO_MEMORY_V01, RIL_E_NO_MEMORY },
    { QMI_ERR_INVALID_QMI_CMD_V01, RIL_E_REQUEST_NOT_SUPPORTED },
    { QMI_ERR_MALFORMED_MSG_V01, RIL_E_INTERNAL_ERR },
    { QMI_ERR_NOT_PROVISIONED_V01, RIL_E_NOT_PROVISIONED },
    { QMI_ERR_INVALID_ARG_V01, RIL_E_INVALID_ARGUMENTS },
    { QMI_ERR_OP_DEVICE_UNSUPPORTED_V01, RIL_E_REQUEST_NOT_SUPPORTED },
    { QMI_ERR_INFO_UNAVAILABLE_V01, RIL_E_NOT_PROVISIONED },
    { QMI_ERR_DEVICE_NOT_READY_V01, RIL_E_INVALID_MODEM_STATE },
    { QMI_ERR_ENCODING_V01, RIL_E_ENCODING_ERR },
    { QMI_ERR_SMSC_ADDR_V01, RIL_E_INVALID_SMSC_ADDRESS },
    { QMI_ERR_DEVICE_STORAGE_FULL_V01, RIL_E_NO_RESOURCES },
    { QMI_ERR_NETWORK_NOT_READY_V01, RIL_E_NETWORK_NOT_READY },
    { QMI_ERR_NO_ENTRY_V01, RIL_E_NO_SUCH_ENTRY },
    { QMI_ERR_DEVICE_IN_USE_V01, RIL_E_DEVICE_IN_USE },
    { QMI_ERR_INCOMPATIBLE_STATE_V01, RIL_E_INVALID_MODEM_STATE },
    { QMI_ERR_NOT_SUPPORTED_V01, RIL_E_MODEM_ERR },
    { QMI_ERR_NO_SUBSCRIPTION_V01, RIL_E_NO_SUBSCRIPTION },
    { QMI_ERR_HARDWARE_RESTRICTED_V01, RIL_E_SYSTEM_ERR }
  };

  // "Attempt" to set and receive error from modem
  util_test_set_nr_config(t,
                          RIL_node,
                          QMI_node,
                          NR_DISABLE_MODE_NSA,
                          QMI_RESULT_FAILURE_V01,
                          QMI_ERR_INTERNAL_V01,
                          RIL_E_MODEM_ERR);

  // Loop through and cover currently uncovered QMI Error paths.
  // NOTE: This could be attached to any request, but attached here for
  // As it's already covering one possible error path.
  for (int i = 0; i < qmiErrors.size(); ++i) {
    std::pair<qmi_error_type_v01, RIL_Errno> errPair = qmiErrors[i];
    util_test_set_nr_config(t,
                            RIL_node,
                            QMI_node,
                            NR_DISABLE_MODE_NSA,
                            QMI_RESULT_FAILURE_V01,
                            errPair.first,
                            errPair.second);
  }

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

void util_test_enable_endc(Testcase &t, AndroidTelTestnode& RIL_node, QMIRILTestnode& QMI_node,
  bool enable, RIL_Errno err, qmi_result_type_v01 result, qmi_error_type_v01 error)
{
  // Inject: RIL_REQUEST_ENABLE_ENDC
  RIL_node.update_ril_request_enable_endc(enable);
  t.inject(RIL_node);

  // Expect: QMI_NAS_SET_ENDC_CONFIG_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_req_set_endc_config(enable ? 0x01:0x00);
  t.expect(QMI_node);

  // Inject: QMI_NAS_SET_ENDC_CONFIG_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_req_set_endc_config_resp(result, error);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_ENABLE_ENDC
  RIL_node.update_ril_request_enable_endc_resp(err);
  t.expect(RIL_node);
}

void util_test_query_endc(Testcase &t, AndroidTelTestnode& RIL_node, QMIRILTestnode& QMI_node,
  uint8_t qmi_endc, endc_status ril_endc, RIL_Errno err, qmi_result_type_v01 result,
  qmi_error_type_v01 error) {

  // Inject: RIL_REQUEST_QUERY_ENDC_STATUS
  RIL_node.update_ril_request_query_endc();
  t.inject(RIL_node);

  // Expect: QMI_NAS_GET_ENDC_CONFIG_REQ_MSG_V01
  QMI_node.update_default_qmi_nas_req_get_endc_config();
  t.expect(QMI_node);

  // Inject: QMI_NAS_GET_ENDC_CONFIG_RESP_MSG_V01
  QMI_node.update_default_qmi_nas_req_get_endc_config_resp(qmi_endc, result, error);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_QUERY_ENDC_STATUS
  RIL_node.update_ril_request_query_endc_resp(ril_endc, err);
  t.expect(RIL_node);
}

test_result ril_utf_test_enable_and_query_endc_001(user_specified_attrib_t attr) {
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("ENDC_SET_GET_001");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_test_enable_endc(t, RIL_node, QMI_node, true);
  util_test_query_endc(t, RIL_node, QMI_node, 1, ENDC_STATUS_ENABLED);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_enable_and_query_endc_002(user_specified_attrib_t attr) {
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("ENDC_SET_GET_002");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_test_enable_endc(t, RIL_node, QMI_node, false);
  util_test_query_endc(t, RIL_node, QMI_node, 0, ENDC_STATUS_DISABLED);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_enable_and_query_endc_003(user_specified_attrib_t attr) {
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("ENDC_SET_GET_003");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  util_test_enable_endc(t, RIL_node, QMI_node, false);
  util_test_query_endc(t, RIL_node, QMI_node, 2, ENDC_STATUS_DISABLED, RIL_E_INVALID_RESPONSE); // 2 is invalid. Expect error. 

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_enhanced_radio_capability(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("ENH_RADCAP");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: RIL_REQUEST_GET_ENHANCED_RADIO_CAPABILITY
  RIL_node.update_default_ril_request_get_enhanced_radio_cap();
  t.inject(RIL_node);

  // Expect QMI_DMS_GET_DEVICE_CAP_REQ_V01
  QMI_node.update_default_qmi_dms_get_device_cap_req_v01();
  t.expect(QMI_node);

  // Inject: QMI_DMS_GET_DEVICE_CAP_RESP_V01
  QMI_node.update_default_qmi_dms_get_device_cap_resp_msg(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  QMI_node.set_default_qmi_dms_get_device_cap_resp_v01_device_capabilities();
  dms_multisim_capability_type_v01 msim_cap;
  msim_cap.subscription_config_list_len = 1;
  msim_cap.subscription_config_list[0].subscription_list_len = 1;
  msim_cap.subscription_config_list[0].subscription_list[0] =
      ((dms_subs_capability_mask_type_v01)0x00000020ull);  // LTE
  QMI_node.set_default_qmi_dms_get_device_cap_resp_v01_multisim_capability(msim_cap);
  t.inject(QMI_node);

  // Expect: RIL_REQUEST_GET_ENHANCED_RADIO_CAPABILITY
  RIL_node.update_default_ril_request_get_enhanced_radio_cap_resp(ENHANCED_RAF_LTE);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_cell_barring_info_001(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("CELL_BARRING_INFO");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Enable WCDMA
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t, RIL_node, QMI_node, QCRIL_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, RADIO_TECH_UNKNOWN, 0, RIL_E_SUCCESS, 0, TRUE, FALSE, FALSE, FALSE);
  util_voice_radio_tech_wcdma_in_service(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN, NAS_SYS_SRV_STATUS_SRV_V01, SYS_SRV_STATUS_SRV_V01, SYS_SRV_DOMAIN_CS_PS_V01, NAS_SYS_ROAM_STATUS_OFF_V01, FALSE, NULL, NULL, 1);

  // Inject: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info();
  t.inject(RIL_node);

  RIL_CellBarringInfo barring_resp_info;
  memset(&barring_resp_info, 0x00, sizeof(barring_resp_info));
  barring_resp_info.barringInfo[0].barringService = CS_SERVICE;
  barring_resp_info.barringInfo[0].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[1].barringService = PS_SERVICE;
  barring_resp_info.barringInfo[1].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[2].barringService = CS_VOICE;
  barring_resp_info.barringInfo[2].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[3].barringService = EMERGENCY;
  barring_resp_info.barringInfo[3].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[4].barringService = BARRING_SMS;
  barring_resp_info.barringInfo[4].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.isBarred = 0;
  // Expect: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info_resp(barring_resp_info);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_cell_barring_info_002(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("CELL_BARRING_INFO");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Enable LTE
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t,
                         RIL_node,
                         QMI_node,
                         QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA,
                         RADIO_TECH_UNKNOWN,
                         0,
                         RIL_E_SUCCESS,
                         0,
                         TRUE,
                         FALSE,
                         FALSE,
                         FALSE);
  util_voice_radio_tech_lte_in_service(t,
                                       QMI_node,
                                       RIL_node,
                                       IMS_node,
                                       TRUE,
                                       RADIO_TECH_UNKNOWN,
                                       NAS_SYS_SRV_STATUS_SRV_V01,
                                       SYS_SRV_STATUS_SRV_V01,
                                       SYS_SRV_DOMAIN_CS_PS_V01,
                                       NAS_SYS_ROAM_STATUS_OFF_V01,
                                       NAS_LTE_VOICE_STATUS_NO_VOICE_V01,
                                       NAS_SMS_STATUS_NO_SMS_V01,
                                       FALSE,
                                       TRUE,
                                       "123",
                                       "45",
                                       TRUE,
                                       TRUE);

  // Inject: QMI_NAS_ACB_INFO_IND_V01
  nas_acb_info_ind_msg_v01* acb_info = QMI_node.update_acb_info_ind_msg();
  acb_info->ac_barring_for_emergency_valid = TRUE;
  acb_info->ac_barring_for_emergency = TRUE;
  acb_info->ac_barring_mo_signalling_valid = TRUE;
  acb_info->ac_barring_mo_signalling.ac_barring_factor = 0;
  acb_info->ac_barring_mo_signalling.ac_barring_time = 0;
  acb_info->ac_barring_mo_signalling.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_mo_data_valid = TRUE;
  acb_info->ac_barring_mo_data.ac_barring_factor = 0;
  acb_info->ac_barring_mo_data.ac_barring_time = 0;
  acb_info->ac_barring_mo_data.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_csfb_valid = TRUE;
  acb_info->ac_barring_csfb.ac_barring_factor = 0;
  acb_info->ac_barring_csfb.ac_barring_time = 0;
  acb_info->ac_barring_csfb.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_type_mo_signalling_valid = TRUE;
  acb_info->ac_barring_type_mo_signalling = NAS_CELL_BARRING_CONDITIONAL_V01;
  acb_info->ac_barring_type_mo_data_valid = TRUE;
  acb_info->ac_barring_type_mo_data = NAS_CELL_BARRING_CONDITIONAL_V01;
  acb_info->ac_barring_type_csfb_valid = TRUE;
  acb_info->ac_barring_type_csfb = NAS_CELL_BARRING_CONDITIONAL_V01;
  acb_info->ac_barring_type_emergency_valid = TRUE;
  acb_info->ac_barring_type_emergency = NAS_CELL_BARRING_CONDITIONAL_V01;

  acb_info->skip_ac_barring_mmtel_voice_valid = TRUE;
  acb_info->skip_ac_barring_mmtel_voice = TRUE;
  acb_info->skip_ac_barring_mmtel_video_valid = TRUE;
  acb_info->skip_ac_barring_mmtel_video = TRUE;
  acb_info->skip_ac_barring_sms_valid = TRUE;
  acb_info->skip_ac_barring_sms = TRUE;
  t.inject(QMI_node);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_ssac_change_info_ind_msg_v01* ssac_info = QMI_node.update_ssac_change_info_ind_msg();
  ssac_info->ssac_voice_info_valid = TRUE;
  ssac_info->ssac_voice_info.barring_factor_voice = 0;
  ssac_info->ssac_voice_info.barring_time_voice = 0;
  ssac_info->ssac_video_info_valid = TRUE;
  ssac_info->ssac_video_info.barring_factor_video = 0;
  ssac_info->ssac_video_info.barring_time_video = 0;
  ssac_info->barring_type_voice_valid = TRUE;
  ssac_info->barring_type_voice = NAS_CELL_BARRING_CONDITIONAL_V01;
  ssac_info->barring_type_video_valid = TRUE;
  ssac_info->barring_type_video = NAS_CELL_BARRING_CONDITIONAL_V01;
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info();
  t.inject(RIL_node);

  RIL_CellBarringInfo barring_resp_info;
  memset(&barring_resp_info, 0x00, sizeof(barring_resp_info));
  barring_resp_info.barringInfo[0].barringService = MO_SIGNALLING;
  barring_resp_info.barringInfo[0].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[1].barringService = MO_DATA;
  barring_resp_info.barringInfo[1].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[2].barringService = CS_FALLBACK;
  barring_resp_info.barringInfo[2].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[3].barringService = MMTEL_VOICE;
  barring_resp_info.barringInfo[3].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[4].barringService = MMTEL_VIDEO;
  barring_resp_info.barringInfo[4].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[5].barringService = EMERGENCY;
  barring_resp_info.barringInfo[5].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[6].barringService = BARRING_SMS;
  barring_resp_info.barringInfo[6].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.isBarred = 0;
  // Expect: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info_resp(barring_resp_info);
  // RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_cell_barring_info_003(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("CELL_BARRING_INFO");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Enable LTE
  util_voice_radio_tech_oos(t, QMI_node, RIL_node, TRUE, RADIO_TECH_UNKNOWN);
  util_set_pref_net_type(t,
                         RIL_node,
                         QMI_node,
                         QCRIL_PREF_NET_TYPE_LTE_GSM_WCDMA,
                         RADIO_TECH_UNKNOWN,
                         0,
                         RIL_E_SUCCESS,
                         0,
                         TRUE,
                         FALSE,
                         FALSE,
                         FALSE);
  util_voice_radio_tech_lte_in_service(t,
                                       QMI_node,
                                       RIL_node,
                                       IMS_node,
                                       TRUE,
                                       RADIO_TECH_UNKNOWN,
                                       NAS_SYS_SRV_STATUS_SRV_V01,
                                       SYS_SRV_STATUS_SRV_V01,
                                       SYS_SRV_DOMAIN_CS_PS_V01,
                                       NAS_SYS_ROAM_STATUS_OFF_V01,
                                       NAS_LTE_VOICE_STATUS_NO_VOICE_V01,
                                       NAS_SMS_STATUS_NO_SMS_V01,
                                       FALSE,
                                       TRUE,
                                       "123",
                                       "45",
                                       TRUE,
                                       TRUE);

  // Inject: QMI_NAS_ACB_INFO_IND_V01
  nas_acb_info_ind_msg_v01* acb_info = QMI_node.update_acb_info_ind_msg();
  acb_info->ac_barring_for_emergency_valid = TRUE;
  acb_info->ac_barring_for_emergency = TRUE;
  acb_info->ac_barring_mo_signalling_valid = TRUE;
  acb_info->ac_barring_mo_signalling.ac_barring_factor = 0;
  acb_info->ac_barring_mo_signalling.ac_barring_time = 0;
  acb_info->ac_barring_mo_signalling.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_mo_data_valid = TRUE;
  acb_info->ac_barring_mo_data.ac_barring_factor = 0;
  acb_info->ac_barring_mo_data.ac_barring_time = 0;
  acb_info->ac_barring_mo_data.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_csfb_valid = TRUE;
  acb_info->ac_barring_csfb.ac_barring_factor = 0;
  acb_info->ac_barring_csfb.ac_barring_time = 0;
  acb_info->ac_barring_csfb.ac_barring_for_special_ac = 0;
  acb_info->ac_barring_type_mo_signalling_valid = TRUE;
  acb_info->ac_barring_type_mo_signalling = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  acb_info->ac_barring_type_mo_data_valid = TRUE;
  acb_info->ac_barring_type_mo_data = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  acb_info->ac_barring_type_csfb_valid = TRUE;
  acb_info->ac_barring_type_csfb = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  acb_info->ac_barring_type_emergency_valid = TRUE;
  acb_info->ac_barring_type_emergency = NAS_CELL_BARRING_UNCONDITIONAL_V01;

  acb_info->skip_ac_barring_mmtel_voice_valid = TRUE;
  acb_info->skip_ac_barring_mmtel_voice = TRUE;
  acb_info->skip_ac_barring_mmtel_video_valid = TRUE;
  acb_info->skip_ac_barring_mmtel_video = TRUE;
  acb_info->skip_ac_barring_sms_valid = TRUE;
  acb_info->skip_ac_barring_sms = TRUE;
  t.inject(QMI_node);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_ssac_change_info_ind_msg_v01* ssac_info = QMI_node.update_ssac_change_info_ind_msg();
  ssac_info->ssac_voice_info_valid = TRUE;
  ssac_info->ssac_voice_info.barring_factor_voice = 0;
  ssac_info->ssac_voice_info.barring_time_voice = 0;
  ssac_info->ssac_video_info_valid = TRUE;
  ssac_info->ssac_video_info.barring_factor_video = 0;
  ssac_info->ssac_video_info.barring_time_video = 0;
  ssac_info->barring_type_voice_valid = TRUE;
  ssac_info->barring_type_voice = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  ssac_info->barring_type_video_valid = TRUE;
  ssac_info->barring_type_video = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info();
  t.inject(RIL_node);

  RIL_CellBarringInfo barring_resp_info;
  memset(&barring_resp_info, 0x00, sizeof(barring_resp_info));
  barring_resp_info.barringInfo[0].barringService = MO_SIGNALLING;
  barring_resp_info.barringInfo[0].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[1].barringService = MO_DATA;
  barring_resp_info.barringInfo[1].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[2].barringService = CS_FALLBACK;
  barring_resp_info.barringInfo[2].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[3].barringService = MMTEL_VOICE;
  barring_resp_info.barringInfo[3].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[4].barringService = MMTEL_VIDEO;
  barring_resp_info.barringInfo[4].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[5].barringService = EMERGENCY;
  barring_resp_info.barringInfo[5].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[6].barringService = BARRING_SMS;
  barring_resp_info.barringInfo[6].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.isBarred = 0;
  // Expect: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info_resp(barring_resp_info);
  // RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_cell_barring_info_004(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("CELL_BARRING_INFO");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Enable NR5G
  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "404",
                                         "10",
                                         "404",
                                         "10",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         nullptr,
                                         nullptr);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_cell_barring_info_ind_msg_v01* barring_info = QMI_node.update_cell_barring_info_ind_msg();
  barring_info->radio_if_valid = TRUE;
  barring_info->radio_if = NAS_RADIO_IF_NR5G_V01;
  barring_info->emerg_barring_info_valid = TRUE;
  barring_info->emerg_barring_info.barring_factor = 0;
  barring_info->emerg_barring_info.barring_time = 0;
  barring_info->signalling_barring_info_valid = TRUE;
  barring_info->signalling_barring_info.barring_factor = 0;
  barring_info->signalling_barring_info.barring_time = 0;
  barring_info->data_barring_info_valid = TRUE;
  barring_info->data_barring_info.barring_factor = 0;
  barring_info->data_barring_info.barring_time = 0;
  barring_info->voice_barring_info_valid = TRUE;
  barring_info->voice_barring_info.barring_factor = 0;
  barring_info->voice_barring_info.barring_time = 0;
  barring_info->video_barring_info_valid = TRUE;
  barring_info->video_barring_info.barring_factor = 0;
  barring_info->video_barring_info.barring_time = 0;
  barring_info->sms_barring_info_valid = TRUE;
  barring_info->sms_barring_info.barring_factor = 0;
  barring_info->sms_barring_info.barring_time = 0;
  barring_info->emerg_barring_type_valid = TRUE;
  barring_info->emerg_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  barring_info->signalling_barring_type_valid = TRUE;
  barring_info->signalling_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  barring_info->data_barring_type_valid = TRUE;
  barring_info->data_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  barring_info->voice_barring_type_valid = TRUE;
  barring_info->voice_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  barring_info->video_barring_type_valid = TRUE;
  barring_info->video_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  barring_info->sms_barring_type_valid = TRUE;
  barring_info->sms_barring_type = NAS_CELL_BARRING_CONDITIONAL_V01;
  t.inject(QMI_node);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_access_barring_status_ind_msg_v01* barring_status =
      QMI_node.update_access_barring_status_ind_msg();
  barring_status->radio_if_valid = TRUE;
  barring_status->radio_if = NAS_RADIO_IF_NR5G_V01;
  barring_status->emerg_barring_status_valid = TRUE;
  barring_status->emerg_barring_status.barring_status = 0;
  barring_status->emerg_barring_status.barring_time = 0;
  barring_status->signalling_barring_status_valid = TRUE;
  barring_status->signalling_barring_status.barring_status = 0;
  barring_status->signalling_barring_status.barring_time = 0;
  barring_status->data_barring_status_valid = TRUE;
  barring_status->data_barring_status.barring_status = 0;
  barring_status->data_barring_status.barring_time = 0;
  barring_status->voice_barring_status_valid = TRUE;
  barring_status->voice_barring_status.barring_status = 0;
  barring_status->voice_barring_status.barring_time = 0;
  barring_status->video_barring_status_valid = TRUE;
  barring_status->video_barring_status.barring_status = 0;
  barring_status->video_barring_status.barring_time = 0;
  barring_status->sms_barring_status_valid = TRUE;
  barring_status->sms_barring_status.barring_status = 0;
  barring_status->sms_barring_status.barring_time = 0;
  barring_status->csfb_barring_status_valid = TRUE;
  barring_status->csfb_barring_status.barring_status = 0;
  barring_status->csfb_barring_status.barring_time = 0;
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info();
  t.inject(RIL_node);

  RIL_CellBarringInfo barring_resp_info;
  memset(&barring_resp_info, 0x00, sizeof(barring_resp_info));
  barring_resp_info.barringInfo[0].barringService = MO_SIGNALLING;
  barring_resp_info.barringInfo[0].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[1].barringService = MO_DATA;
  barring_resp_info.barringInfo[1].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[2].barringService = CS_FALLBACK;
  barring_resp_info.barringInfo[2].barringType = BARRING_NONE;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[3].barringService = MMTEL_VOICE;
  barring_resp_info.barringInfo[3].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[4].barringService = MMTEL_VIDEO;
  barring_resp_info.barringInfo[4].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[5].barringService = EMERGENCY;
  barring_resp_info.barringInfo[5].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[6].barringService = BARRING_SMS;
  barring_resp_info.barringInfo[6].barringType = CONDITIONAL;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.isBarred = 0;

  // Expect: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info_resp(barring_resp_info);
  // RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}

test_result ril_utf_test_get_cell_barring_info_005(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;

  t.set_test_id(__FUNCTION__);
  t.set_test_attribute("NAS");
  t.set_test_description("CELL_BARRING_INFO");
  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Enable NR5G
  util_voice_registration_state_for_nr5g(t,
                                         QMI_node,
                                         RIL_node,
                                         RADIO_TECH_5G,
                                         RIL_VAL_REG_REGISTERED_HOME_NET,
                                         NAS_SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_STATUS_SRV_V01,
                                         SYS_SRV_DOMAIN_PS_ONLY_V01,
                                         NAS_SYS_ROAM_STATUS_OFF_V01,
                                         FALSE,
                                         SYS_SRV_DOMAIN_NO_SRV_V01,
                                         0,
                                         TRUE,
                                         "404",
                                         "10",
                                         "404",
                                         "10",
                                         TRUE,
                                         TRUE,
                                         NAS_NR5G_VOICE_DOMAIN_IMS_V01,
                                         nullptr,
                                         nullptr);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_cell_barring_info_ind_msg_v01* barring_info = QMI_node.update_cell_barring_info_ind_msg();
  barring_info->radio_if_valid = TRUE;
  barring_info->radio_if = NAS_RADIO_IF_NR5G_V01;
  barring_info->emerg_barring_info_valid = TRUE;
  barring_info->emerg_barring_info.barring_factor = 0;
  barring_info->emerg_barring_info.barring_time = 0;
  barring_info->signalling_barring_info_valid = TRUE;
  barring_info->signalling_barring_info.barring_factor = 0;
  barring_info->signalling_barring_info.barring_time = 0;
  barring_info->data_barring_info_valid = TRUE;
  barring_info->data_barring_info.barring_factor = 0;
  barring_info->data_barring_info.barring_time = 0;
  barring_info->voice_barring_info_valid = TRUE;
  barring_info->voice_barring_info.barring_factor = 0;
  barring_info->voice_barring_info.barring_time = 0;
  barring_info->video_barring_info_valid = TRUE;
  barring_info->video_barring_info.barring_factor = 0;
  barring_info->video_barring_info.barring_time = 0;
  barring_info->sms_barring_info_valid = TRUE;
  barring_info->sms_barring_info.barring_factor = 0;
  barring_info->sms_barring_info.barring_time = 0;
  barring_info->emerg_barring_type_valid = TRUE;
  barring_info->emerg_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  barring_info->signalling_barring_type_valid = TRUE;
  barring_info->signalling_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  barring_info->data_barring_type_valid = TRUE;
  barring_info->data_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  barring_info->voice_barring_type_valid = TRUE;
  barring_info->voice_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  barring_info->video_barring_type_valid = TRUE;
  barring_info->video_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  barring_info->sms_barring_type_valid = TRUE;
  barring_info->sms_barring_type = NAS_CELL_BARRING_UNCONDITIONAL_V01;
  t.inject(QMI_node);

  // Inject: QMI_NAS_CELL_BARRING_INFO_IND_MSG_V01
  nas_access_barring_status_ind_msg_v01* barring_status =
      QMI_node.update_access_barring_status_ind_msg();
  barring_status->radio_if_valid = TRUE;
  barring_status->radio_if = NAS_RADIO_IF_NR5G_V01;
  barring_status->emerg_barring_status_valid = TRUE;
  barring_status->emerg_barring_status.barring_status = 0;
  barring_status->emerg_barring_status.barring_time = 0;
  barring_status->signalling_barring_status_valid = TRUE;
  barring_status->signalling_barring_status.barring_status = 0;
  barring_status->signalling_barring_status.barring_time = 0;
  barring_status->data_barring_status_valid = TRUE;
  barring_status->data_barring_status.barring_status = 0;
  barring_status->data_barring_status.barring_time = 0;
  barring_status->voice_barring_status_valid = TRUE;
  barring_status->voice_barring_status.barring_status = 0;
  barring_status->voice_barring_status.barring_time = 0;
  barring_status->video_barring_status_valid = TRUE;
  barring_status->video_barring_status.barring_status = 0;
  barring_status->video_barring_status.barring_time = 0;
  barring_status->sms_barring_status_valid = TRUE;
  barring_status->sms_barring_status.barring_status = 0;
  barring_status->sms_barring_status.barring_time = 0;
  barring_status->csfb_barring_status_valid = TRUE;
  barring_status->csfb_barring_status.barring_status = 0;
  barring_status->csfb_barring_status.barring_time = 0;
  t.inject(QMI_node);

  // Inject: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info();
  t.inject(RIL_node);

  RIL_CellBarringInfo barring_resp_info;
  memset(&barring_resp_info, 0x00, sizeof(barring_resp_info));
  barring_resp_info.barringInfo[0].barringService = MO_SIGNALLING;
  barring_resp_info.barringInfo[0].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[0].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[1].barringService = MO_DATA;
  barring_resp_info.barringInfo[1].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[1].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[2].barringService = CS_FALLBACK;
  barring_resp_info.barringInfo[2].barringType = BARRING_NONE;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[2].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[3].barringService = MMTEL_VOICE;
  barring_resp_info.barringInfo[3].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[3].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[4].barringService = MMTEL_VIDEO;
  barring_resp_info.barringInfo[4].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[4].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[5].barringService = EMERGENCY;
  barring_resp_info.barringInfo[5].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[5].typeSpecificInfo.isBarred = 0;

  barring_resp_info.barringInfo[6].barringService = BARRING_SMS;
  barring_resp_info.barringInfo[6].barringType = UNCONDITIONAL;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringFactor = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.barringTimeSeconds = 0;
  barring_resp_info.barringInfo[6].typeSpecificInfo.isBarred = 0;

  // Expect: RIL_REQUEST_GET_CELL_BARRING_INFO
  RIL_node.update_get_cell_barring_info_resp(barring_resp_info);
  // RIL_node.set_expect_skip_verification_status(false);
  t.expect(RIL_node);

  t.execute();
  t.print_summary();

  return t.get_test_result();
}
