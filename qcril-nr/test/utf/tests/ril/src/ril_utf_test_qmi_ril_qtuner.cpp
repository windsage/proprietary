/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include "ril_utf_test_qmi_ril.h"

//==================================================
//  ril_utf_test_qmi_qtuner_set_rfm_scenario_req
//  Description:
//   OEMHOOK request: QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01
//==================================================
test_result ril_utf_test_qmi_qtuner_set_rfm_scenario_req(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  OEMTestnode OEM_node;

  t.set_test_description("QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01");
  t.set_test_attribute("NAS");
  t.set_test_id(__func__);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01
  Qtuner_set_scenario_req_v01 *set_scenarios = OEM_node.update_default_qmi_qtuner_set_rfm_scenario_req();
  set_scenarios->scenarios_len = 2;
  set_scenarios->scenarios[0] = 2;
  set_scenarios->scenarios[1] = 3;
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01
  uint32_t scenarios[2];
  scenarios[0] = 2;
  scenarios[1] = 3;
  QMI_node.update_default_qmi_rfrpe_set_rfm_scenario_req(2, scenarios);
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01
  QMI_node.update_default_qmi_rfrpe_set_rfm_scenario_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01
  OEM_node.update_default_qmi_qtuner_set_rfm_scenario_resp(RIL_E_SUCCESS);
  t.expect(OEM_node);

  // Inject: QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01
  set_scenarios = OEM_node.update_default_qmi_qtuner_set_rfm_scenario_req();
  set_scenarios->scenarios_len = 2;
  set_scenarios->scenarios[0] = 2;
  set_scenarios->scenarios[1] = 3;
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01
  QMI_node.update_default_qmi_rfrpe_set_rfm_scenario_req(2, scenarios);
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_SET_RFM_SCENARIO_RESP_V01
  QMI_node.update_default_qmi_rfrpe_set_rfm_scenario_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01
  OEM_node.update_default_qmi_qtuner_set_rfm_scenario_resp(RIL_E_MODEM_ERR);
  t.expect(OEM_node);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

//==================================================
//  ril_utf_test_qmi_qtuner_get_rfm_scenario_req
//  Description:
//   OEMHOOK request: QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01
//==================================================
test_result ril_utf_test_qmi_qtuner_get_rfm_scenario_req(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  OEMTestnode OEM_node;

  t.set_test_description("QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01");
  t.set_test_attribute("NAS");
  t.set_test_id(__func__);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01
  OEM_node.update_default_qmi_qtuner_get_rfm_scenario_req();
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_GET_RFM_SCENARIO_REQ_V01
  QMI_node.update_default_qmi_rfrpe_get_rfm_scenario_req();
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_GET_RFM_SCENARIO_RESP_V01
  rfrpe_get_rfm_scenarios_resp_v01 *get_req = QMI_node.update_default_qmi_rfrpe_get_rfm_scenario_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  get_req->active_scenarios_valid = TRUE;
  get_req->active_scenarios_len = 2;
  get_req->active_scenarios[0] = 5;
  get_req->active_scenarios[1] = 6;
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01
  Qtuner_get_rfm_scenarios_resp_v01 *get_resp = OEM_node.update_default_qmi_qtuner_get_rfm_scenario_resp(RIL_E_SUCCESS);
  get_resp->active_scenarios_valid = TRUE;
  get_resp->active_scenarios_len = 2;
  get_resp->active_scenarios[0] = 5;
  get_resp->active_scenarios[1] = 6;
  t.expect(OEM_node);

  // Inject: QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01
  OEM_node.update_default_qmi_qtuner_get_rfm_scenario_req();
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_GET_RFM_SCENARIO_REQ_V01
  QMI_node.update_default_qmi_rfrpe_get_rfm_scenario_req();
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_GET_RFM_SCENARIO_RESP_V01
  QMI_node.update_default_qmi_rfrpe_get_rfm_scenario_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01
  OEM_node.update_default_qmi_qtuner_get_rfm_scenario_resp(RIL_E_MODEM_ERR);
  t.expect(OEM_node);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}

//==================================================
//  ril_utf_test_qmi_qtuner_get_provisioned_table_revision_req
//  Description:
//   OEMHOOK request: QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01
//==================================================
test_result ril_utf_test_qmi_qtuner_get_provisioned_table_revision_req(user_specified_attrib_t attr)
{
  Testcase t;
  QMIRILTestnode QMI_node;
  AndroidTelTestnode RIL_node;
  IMSTestnode IMS_node;
  OEMTestnode OEM_node;

  t.set_test_description("QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01");
  t.set_test_attribute("NAS");
  t.set_test_id(__func__);

  if (!t.isAttributeMatch(attr)) {
    test_result ret;
    return ret;
  }

  // Inject: QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01
  OEM_node.update_default_qmi_qtuner_get_provisioned_table_revision_req();
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01
  QMI_node.update_default_qmi_rfrpe_get_provisioned_table_revision_req();
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_RESP_V01
  rfrpe_get_provisioned_table_revision_resp_v01 *get_req = QMI_node.update_default_qmi_rfrpe_get_provisioned_table_revision_resp(QMI_RESULT_SUCCESS_V01, QMI_ERR_NONE_V01);
  get_req->provisioned_table_revision_valid = TRUE;
  get_req->provisioned_table_revision = 10;
  get_req->provisioned_table_OEM_valid = TRUE;
  get_req->provisioned_table_OEM_len = 2;
  get_req->provisioned_table_OEM[0] = 5;
  get_req->provisioned_table_OEM[1] = 6;
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01
  Qtuner_get_provisioned_table_revision_resp_v01 *get_resp = OEM_node.update_default_qmi_qtuner_get_provisioned_table_revision_resp(RIL_E_SUCCESS);
  get_resp->provisioned_table_revision_valid = TRUE;
  get_resp->provisioned_table_revision = 10;
  get_resp->provisioned_table_OEM_valid = TRUE;
  get_resp->provisioned_table_OEM_len = 2;
  get_resp->provisioned_table_OEM[0] = 5;
  get_resp->provisioned_table_OEM[1] = 6;
  t.expect(OEM_node);

  // Inject: QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01
  OEM_node.update_default_qmi_qtuner_get_provisioned_table_revision_req();
  t.inject(OEM_node);

  // Expect: QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01
  QMI_node.update_default_qmi_rfrpe_get_provisioned_table_revision_req();
  t.expect(QMI_node);

  // Inject: QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_RESP_V01
  QMI_node.update_default_qmi_rfrpe_get_provisioned_table_revision_resp(QMI_RESULT_FAILURE_V01, QMI_ERR_INTERNAL_V01);
  t.inject(QMI_node);

  // Expect: QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01
  OEM_node.update_default_qmi_qtuner_get_provisioned_table_revision_resp(RIL_E_MODEM_ERR);
  t.expect(OEM_node);


  t.execute();
  t.print_summary();

  return t.get_test_result();
}
