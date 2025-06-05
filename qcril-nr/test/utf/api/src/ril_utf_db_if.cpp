/******************************************************************************
#  Copyright (c) 2013 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

  @file    ril_utf_db_if.cpp
  @brief   sqlite3 db API's for test case dev

  DESCRIPTION

  API's for querying and modifying sqlite3 db used in RIL UTF tests.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "ril_utf_db_if.h"
using namespace std;

//==================================================
// Private DB Interface
//==================================================
void ril_utf_db_if::set_mcc(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->mcc = (char*)malloc(strlen(str)+1);
  strcpy(db_node->mcc, str);
}

void ril_utf_db_if::set_mnc(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->mnc = (char*)malloc(strlen(str)+1);
  strcpy(db_node->mnc, str);
}

void ril_utf_db_if::set_emergency_num(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->emergency_num = (char*)malloc(strlen(str)+1);
  strcpy(db_node->emergency_num , str);
}

void ril_utf_db_if::set_service_state(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->service_state = (char*)malloc(strlen(str)+1);
  strcpy(db_node->service_state , str);
}

void ril_utf_db_if::set_service(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->service = (char*)malloc(strlen(str)+1);
  strcpy(db_node->service, str);
}

void ril_utf_db_if::set_ims_address(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->ims_address = (char*)malloc(strlen(str)+1);
  strcpy(db_node->ims_address, str);
}

void ril_utf_db_if::set_iin(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->iin = (char*)malloc(strlen(str)+1);
  strcpy(db_node->iin, str);
}

void ril_utf_db_if::set_roam(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->roam = (char*)malloc(strlen(str)+1);
  strcpy(db_node->roam, str);
}

void ril_utf_db_if::set_property_name(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->property_name = (char*)malloc(strlen(str)+1);
  strcpy(db_node->property_name, str);
}

void ril_utf_db_if::set_value(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->value = (char*)malloc(strlen(str)+1);
  strcpy(db_node->value, str);
}

void ril_utf_db_if::set_source(qmi_ril_custom_emergency_numbers_source_type val)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->source = val;
}

void ril_utf_db_if::set_operator_type(const char* str)
{
  db_node_t* db_node = (db_node_t*) payload;
  int i;
  for (i = 0; i < QCRIL_DB_MAX_OPERATOR_TYPE_LEN; ++i)
  {
    if (str[i])
    {
      db_node->operator_type[i] = str[i];
    }
    else
    {
      break;
    }
  }
}

void ril_utf_db_if::set_iccid(const char *str)
{
  db_node_t* db_node = (db_node_t*) payload;
  size_t iccid_len = strlen(str) + 1;
  db_node->iccid = (char*)malloc(iccid_len);
  strlcpy(db_node->iccid, str, iccid_len);
}

void ril_utf_db_if::set_clir_pref(uint32_t clir)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->clir_pref = clir;
}

void ril_utf_db_if::set_iccid_found(bool found)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->iccid_found = found;
}


void ril_utf_db_if::set_clir_pref_entry_count(int32_t count)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->clir_pref_entry_count = count;
}

void ril_utf_db_if::set_clir_pref_entry_age(int32_t age)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->clir_pref_entry_age = age;
}

void ril_utf_db_if::set_action(ril_utf_db_action_t val)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->action = val;
}

void ril_utf_db_if::set_expected_ret(int ret)
{
  db_node_t* db_node = (db_node_t*) payload;
  db_node->expected_ret = ret;
}

void ril_utf_db_if::update_default_db_request()
{
  payload = malloc(sizeof(db_node_t));
  payload_len = sizeof(db_node_t);
  memset(payload, 0x0, payload_len);
  payload_type = ril_utf_db_request;
}

void ril_utf_db_if::update_default_db_response()
{
  payload = malloc(sizeof(db_node_t));
  payload_len = sizeof(db_node_t);
  memset(payload, 0x0, payload_len);
  payload_type = ril_utf_db_response;
}

//========================================================
//  Public DB Interface
//========================================================
void ril_utf_db_if::utf_db_is_mcc_part_of_emergency_numbers_table
(
    qmi_ril_custom_emergency_numbers_source_type source,
    const char *mcc,
    const char *emergency_num,
    int expected_ret
)
{
  update_default_db_response();
  set_source(source);
  set_mcc(mcc);
  set_emergency_num(emergency_num);
  set_action(UTF_DB_IS_MCC_PART_OF_EMERGENCY_NUMBER_TABLE);
  set_expected_ret(expected_ret);
}

void ril_utf_db_if::utf_db_query_operator_type
(
    const char *mcc,
    const char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
)
{
  update_default_db_response();
  set_mcc(mcc);
  set_mnc(mnc);
  if (operator_type != "")
  {
    set_operator_type(operator_type);
  }
  set_action(UTF_DB_QUERY_OPERATOR_TYPE);
}

void ril_utf_db_if::utf_db_insert_operator_type
(
    const char *mcc,
    const char *mnc,
    const char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN],
    int expected_ret
)
{
  update_default_db_request();
  set_mcc(mcc);
  set_mnc(mnc);
  set_operator_type(operator_type);
  set_action(UTF_DB_INSERT_OPERATOR_TYPE);
  set_expected_ret(expected_ret);
}

void ril_utf_db_if::utf_db_insert_properties_table
(
    const char *property_name,
    const char *value,
    int expected_ret
)
{
  update_default_db_request();
  set_property_name(property_name);
  set_value(value);
  set_action(UTF_DB_INSERT_PROPERTIES_TABLE);
  set_expected_ret(expected_ret);
}

void ril_utf_db_if::clear_internals()
{
  if (payload) {
    db_node_t* db_node = (db_node_t*)payload;
    if (db_node->property_name) {
      free(db_node->property_name);
      db_node->property_name = NULL;
    }
    if (db_node->value) {
      free(db_node->value);
      db_node->value = NULL;
    }
    if (db_node->roam) {
      free(db_node->roam);
      db_node->roam = NULL;
    }
    if (db_node->iin) {
      free(db_node->iin);
      db_node->iin = NULL;
    }
    if (db_node->ims_address) {
      free(db_node->ims_address);
      db_node->ims_address = NULL;
    }
    if (db_node->service) {
      free(db_node->service);
      db_node->service = NULL;
    }
    if (db_node->service_state) {
      free(db_node->service_state);
      db_node->service_state = NULL;
    }
    if (db_node->emergency_num) {
      free(db_node->emergency_num);
      db_node->emergency_num = NULL;
    }
    if (db_node->mnc) {
      free(db_node->mnc);
      db_node->mnc = NULL;
    }
    if (db_node->mcc) {
      free(db_node->mcc);
      db_node->mcc = NULL;
    }
  }
}

void ril_utf_db_if::utf_db_query_clir_pref
(
    const char *iccid,
    uint32_t clir_pref,
    bool found,
    int expected_ret
)
{
  update_default_db_response();
  set_iccid(iccid);
  set_clir_pref(clir_pref);
  set_iccid_found(found);
  set_action(UTF_DB_QUERY_CLIR_PREF);
  set_expected_ret(expected_ret);
}
void ril_utf_db_if::utf_db_query_clir_pref_entry_count
(
    int32_t entry_count,
    int expected_ret
)
{
  update_default_db_response();
  set_clir_pref_entry_count(entry_count);
  set_action(UTF_DB_QUERY_CLIR_PREF_ENTRY_COUNT);
  set_expected_ret(expected_ret);
}
void ril_utf_db_if::utf_db_query_clir_pref_oldest_entry
(
    const char *oldest,
    int expected_ret
)
{
    update_default_db_response();
    set_iccid(oldest);
    set_action(UTF_DB_QUERY_OLDEST_CLIR_PREF);
    set_expected_ret(expected_ret);
}
void ril_utf_db_if::utf_db_insert_clir_pref
(
  const char *iccid,
  uint32_t clir_pref,
  int32_t entry_age,
  int expected_ret
)
{
  update_default_db_request();
  set_iccid(iccid);
  set_clir_pref(clir_pref);
  set_clir_pref_entry_age(entry_age);
  set_action(UTF_DB_INSERT_CLIR_PREF);
  set_expected_ret(expected_ret);
}
void ril_utf_db_if::utf_db_update_clir_pref
(
  const char *iccid,
  uint32_t clir_pref,
  int expected_ret
)
{
  update_default_db_request();
  set_iccid(iccid);
  set_clir_pref(clir_pref);
  set_action(UTF_DB_UPDATE_CLIR_PREF);
  set_expected_ret(expected_ret);
}

void ril_utf_db_if::utf_db_delete_clir_pref
(
  const char *iccid,
  int expected_ret
)
{
  update_default_db_request();
  set_iccid(iccid);
  set_action(UTF_DB_DELETE_CLIR_PREF);
  set_expected_ret(expected_ret);
}

void ril_utf_db_if::utf_db_clear_iccid_clir_pref_table
(
  int expected_ret
)
{
  update_default_db_request();
  set_action(UTF_DB_CLEAR_ICCID_CLIR_PREF_TABLE);
  set_expected_ret(expected_ret);
}
