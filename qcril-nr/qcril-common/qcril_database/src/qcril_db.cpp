/******************************************************************************
#  Copyright (c) 2013, 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_db.c
  @brief   Provides interface to communicate with qcril db tables

  DESCRIPTION
    Initialize sqlite db
    Create qcril db tables
    Provides interface to query db tables

******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#ifndef __USE_GNU
#define __USE_GNU /* Required for non-standard scandirat function */
#endif
#define TAG "RILQ-DB"
#include "qcril_db.h"
#include "DbState.h"
#include "qcril_file_utils.h"
#include "qcril_config.h"
#include <cutils/properties.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mman.h>
#include <array>
#include "framework/Log.h"
#include "framework/legacy.h"
#include "qcril_memory_management.h"

extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);

#ifdef QMI_RIL_UTF
#define QCRIL_DATABASE_NAME "./qcrilNr.db"
#define QCRIL_BACKUP_DATABASE_NAME "./qcrilNr_backup.db"
#define UPGRADE_SCRIPTS_PATH   "./upgrade"
#define QCRIL_PREBUILT_DB_NAME "./qcrilNr_prebuilt.db"
#else
#define QCRIL_DATABASE_NAME DATA_PARTITION_ROOT "radio/qcrilNr.db"
#define QCRIL_BACKUP_DATABASE_NAME DATA_PARTITION_ROOT "radio/qcrilNr_backup.db"
#define QCRIL_PREBUILT_DB_NAME DATA_PARTITION_ROOT "radio/qcrilNr_prebuilt.db"

#ifndef RIL_FOR_MDM_LE
#define UPGRADE_SCRIPTS_PATH   "/vendor/etc/qcril_database/upgrade"
#else
#define UPGRADE_SCRIPTS_PATH   "/etc/radio/qcril_database/upgrade"
#endif // RIL_FOR_MDM_LE

#endif // QMI_RIL_UTF

#define UPGRADE_SCRIPTS_CONFIG_PATH UPGRADE_SCRIPTS_PATH"/config"
#define UPGRADE_SCRIPTS_OTHER_PATH UPGRADE_SCRIPTS_PATH"/other"

#define MAX_RETRY               (10)
#define QCRIL_DB_SLEEP_TIME     (500000)  // in micro seconds.
#define QCRIL_DATA_DIR_CHECK_INTERVAL     (1) // in seconds
#define QCRIL_DATA_DIR_CHECK_TIME_LIMIT   (12) // in seconds

#define QCRIL_DB_PROPERTY_VALUE_MAX   (200)
#define QCRIL_MBN_HW_STR_MAX_SIZE     (750)

#define QCRIL_DB_MIGRATION_PROPERTY_NAME "dbmigration"

#define TEL_DB_ICCID_COL_NAME            "icc_id"
#define TEL_DB_SUBSTATE_COL_NAME         "sub_state"
#define TEL_DB_SIMINFO_TABLE_NAME        "siminfo"

#define IIN_11_LEN              (12)
#define IIN_7_LEN               (8)
#define IIN_6_LEN               (7)

/* QCRIL DB handle */
sqlite3* qcril_db_handle     = NULL;

/* Telephony DB handle */
sqlite3* qcril_tel_db_handle = NULL;

const char    *qcril_db     = "qcrilNr.db";
const char    *qcril_tel_db = "telephony.db";

extern "C" size_t strlcpy(char *d, const char *s, size_t sz);
typedef enum
{
    QCRIL_DB_TABLE_FIRST = 0,
    QCRIL_DB_TABLE_OPERATOR_TYPE = QCRIL_DB_TABLE_FIRST,
    QCRIL_DB_SW_MBN_FILE_TYPE,
    QCRIL_DB_SW_MBN_IIN_TYPE,
    QCRIL_DB_SW_MBN_MCC_MNC_TYPE,
    QCRIL_DB_HW_MBN_FILE_TYPE,
    QCRIL_DB_SIG_CONFIG_TYPE,
    QCRIL_DB_MANUAL_PROV_TYPE,
    QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE,
    QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE,
    QCRIL_DB_MODULES,
    QCRIL_DB_ICCID_CLIR_PREF_TYPE,
    QCRIL_DB_TABLE_MAX
} qcril_db_table_type;

typedef struct qcril_db_table_info {
    const char *table_name;
    const char *create_stmt;
    uint8_t drop_during_bootup;
} qcril_db_table_info;

typedef struct qcril_db_escv_in_out {
    const char *mnc;
    int escv_type;
} qcril_db_escv_in_out;


#define QCRIL_PROPERTIES_TABLE_NAME "qcril_properties_table"

/* Statement to create qcril db tables */
#define QCRIL_CREATE_EMRGENCY_TABLE    \
            "create table if not exists %s" \
            "(MCC TEXT, NUMBER TEXT, IMS_ADDRESS TEXT, SERVICE TEXT, PRIMARY KEY(MCC,NUMBER))"

/* Statement to create qcril db escv iin table */
#define QCRIL_DB_CREATE_ESCV_IIN_TABLE  \
            "create table if not exists %s" \
            "(IIN TEXT, NUMBER TEXT, ESCV INTEGER, ROAM TEXT, PRIMARY KEY(IIN,NUMBER,ROAM))"

/* Statement to create qcril db escv nw table */
#define QCRIL_DB_CREATE_ESCV_NW_TABLE  \
            "create table if not exists %s"\
            "(MCC TEXT, MNC TEXT, NUMBER TEXT, ESCV INTEGER, PRIMARY KEY(MCC,NUMBER, ESCV))"

/* Statement to create qcril db operator type table */
#define QCRIL_DB_CREATE_OPERATOR_TYPE_TABLE  \
            "create table if not exists %s"\
            "(MCC TEXT, MNC TEXT, TYPE TEXT, PRIMARY KEY(MCC,MNC))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_FILES_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT PRIMARY KEY, MCFG_VERSION_FAMILY INTEGER, "\
            "MCFG_VERSION_OEM INTEGER, MCFG_VERSION_CARRIER INTEGER, "\
            "MCFG_VERSION_MINOR INTEGER, CARRIER_NAME TEXT, "\
            "QC_VERSION_FAMILY INTEGER, QC_VERSION_OEM INTEGER, "\
            "QC_VERSION_CARRIER INTEGER, QC_VERSION_MINOR INTEGER, "\
            "CONFIG_ID TEXT)"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_MCC_MNC_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT , MCC TEXT, MNC TEXT, "\
            "VOLTE_INFO TEXT, MKT_INFO TEXT, LAB_INFO TEXT, "\
            "PRIMARY KEY(FILE, MCC, MNC))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_IIN_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT , MCFG_IIN TEXT, MCFG_LONG_IIN TEXT, "\
            "VOLTE_INFO TEXT, MKT_INFO TEXT, LAB_INFO TEXT, "\
            "PRIMARY KEY(FILE, MCFG_IIN, MCFG_LONG_IIN))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_HW_MBN_FILES_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT PRIMARY KEY, MCFG_VERSION_FAMILY INTEGER, "\
            "MCFG_VERSION_OEM INTEGER, MCFG_VERSION_CARRIER INTEGER, "\
            "MCFG_VERSION_MINOR INTEGER, HW_NAME TEXT, "\
            "QC_VERSION_FAMILY INTEGER, QC_VERSION_OEM INTEGER, "\
            "QC_VERSION_CARRIER INTEGER, QC_VERSION_MINOR INTEGER, "\
            "SHORT_NAME TEXT, CONFIG_ID TEXT)"


/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_SIG_CONFIG_TABLE  \
            "create table if not exists %s "\
            "(SIG_CONFIG_TYPE TEXT, DELTA TEXT, PRIMARY KEY(SIG_CONFIG_TYPE))"

/* Statement to create qcril db tables */
#define QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE    \
            "create table if not exists %s" \
            "(MCC TEXT, MNC TEXT, NUMBER TEXT, IMS_ADDRESS TEXT, SERVICE TEXT, PRIMARY KEY(MCC,NUMBER,MNC))"


/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_PROV_TABLE  \
            "create table if not exists %s "\
            "(ICCID TEXT, USER_PREF INTEGER, PRIMARY KEY(ICCID))"

/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_MBN_IMSI_EXCEPTION_TABLE  \
            "create table if not exists %s "\
            "(MCC TEXT, MNC TEXT, CARRIER_PREF INTEGER, PRIMARY KEY(MCC,MNC))"

/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_MBN_ICCID_EXCEPTION_TABLE  \
            "create table if not exists %s "\
            "(ICCID TEXT, CARRIER_PREF INTEGER, PRIMARY KEY(ICCID))"

#define QCRIL_DB_CREATE_ICCID_CLIR_PREF_TABLE \
            "create table if not exists %s"\
            "(ICCID TEXT, CLIR_PREF INTEGER, ENTRY_AGE INTEGER, PRIMARY KEY(ICCID))"

#define QCRIL_DB_CREATE_MODULES_TABLES \
    "CREATE TABLE IF NOT EXISTS %s (ROWID  INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, MODULE TEXT, MANDATORY BOOLEAN, INTERNAL BOOLEAN, PRIO INTEGER);" \
    "CREATE UNIQUE INDEX IF NOT EXISTS IDX_MODULES_MODULE ON MODULES (MODULE);" \
    "CREATE UNIQUE INDEX IF NOT EXISTS IDX_MODULES_PRIO ON MODULES (PRIO);" \
    "CREATE TABLE IF NOT EXISTS CONFIGS (ROWID INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, NAME TEXT, DESCRIPTION TEXT);" \
    "CREATE TABLE IF NOT EXISTS CONFIG_MODULES ( ROWID INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, CONFIG_ID INTEGER, MODULE_ID INTEGER, FOREIGN KEY (CONFIG_ID) REFERENCES CONFIGS(ROWID), FOREIGN KEY (MODULE_ID) REFERENCES MODULES(ROWID));" \

/* Table containing qcril db emergency table names */
static qcril_db_table_info qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX] =
{
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_NONE] =
                 {NULL, NULL, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC]      =
                 {  "qcril_emergency_source_mcc_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC]    =
                 { "qcril_emergency_source_voice_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_HARD_MCC] =
                 {"qcril_emergency_source_hard_mcc_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_HARD] =
                 {NULL, NULL, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_NW]       =
                 { "qcril_emergency_source_nw_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_IIN] =
                 { "qcril_emergency_source_escv_iin_table", QCRIL_DB_CREATE_ESCV_IIN_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_NW]  =
                 { "qcril_emergency_source_escv_nw_table", QCRIL_DB_CREATE_ESCV_NW_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC]  =
                 { "qcril_emergency_source_mcc_mnc_table", QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC]  =
                 { "qcril_emergency_source_voice_mcc_mnc_table", QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE, 0 },
};

/* Table containing qcril db table names */
static qcril_db_table_info qcril_db_tables[QCRIL_DB_TABLE_MAX] =
{
    [QCRIL_DB_TABLE_OPERATOR_TYPE]  =
                 { "qcril_operator_type_table", QCRIL_DB_CREATE_OPERATOR_TYPE_TABLE, 0},
    [QCRIL_DB_SW_MBN_FILE_TYPE]  =
                 { "qcril_sw_mbn_file_type_table", QCRIL_DB_CREATE_SW_MBN_FILES_TABLE, 1},
    [QCRIL_DB_SW_MBN_IIN_TYPE]  =
                 { "qcril_sw_mbn_iin_table", QCRIL_DB_CREATE_SW_MBN_IIN_TABLE, 1},
    [QCRIL_DB_SW_MBN_MCC_MNC_TYPE]  =
                 { "qcril_sw_mbn_mcc_mnc_table", QCRIL_DB_CREATE_SW_MBN_MCC_MNC_TABLE, 1},
    [QCRIL_DB_HW_MBN_FILE_TYPE]  =
                 { "qcril_hw_mbn_file_type_table", QCRIL_DB_CREATE_HW_MBN_FILES_TABLE, 1},
    [QCRIL_DB_SIG_CONFIG_TYPE]  =
                 { "qcril_sig_config_table", QCRIL_DB_CREATE_SIG_CONFIG_TABLE, 0},
    [QCRIL_DB_MANUAL_PROV_TYPE]  =
                 { "qcril_manual_prov_table", QCRIL_DB_CREATE_PROV_TABLE, 0},
    [QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE]  =
                 { "qcril_mbn_imsi_exception_table", QCRIL_DB_CREATE_MBN_IMSI_EXCEPTION_TABLE, 0},
    [QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE]  =
                 { "qcril_mbn_iccid_exception_table", QCRIL_DB_CREATE_MBN_ICCID_EXCEPTION_TABLE, 0},
    [QCRIL_DB_MODULES]  =
                 { "MODULES", QCRIL_DB_CREATE_MODULES_TABLES, 0},
    [QCRIL_DB_ICCID_CLIR_PREF_TYPE] =
                {"qcril_iccid_clir_pref_table", QCRIL_DB_CREATE_ICCID_CLIR_PREF_TABLE, 0},
};

#define RESERVED_TO_STORE_LENGTH 4

/* Query statement to query emergency number */
static const char* qcril_db_query_number_and_mcc_stmt =
                      "select NUMBER from %s where MCC='%s' and NUMBER='%s'";

/* Query statement to query emergency number*/
static const char* qcril_db_query_number_and_mcc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and NUMBER='%s' and SERVICE='%s'";

/* Query statement to query emergency number and mcc */
static const char* qcril_db_query_emergency_number_stmt =
                      "select NUMBER from %s where MCC='%s'";

/* Query statement to query emergency number */
static const char* qcril_db_query_number_from_mcc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and SERVICE='%s'";

/* Query statement to query ims_address from mcc and emergency number */
static const char* qcril_db_query_ims_address_from_mcc_number_stmt =
                      "select IMS_ADDRESS from %s where MCC='%s' and NUMBER='%s'";

/* Emergency numbers retrieved */
static char qcril_db_emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN] = {0};

/* Query statement to query emergency number escv type using iin */
static const char* qcril_db_query_escv_iin_stmt =
                      "select ESCV from %s where IIN='%s' and NUMBER='%s' and ROAM='%s'";

/* Query statement to query emergency number escv type using mcc and mnc */
static const char* qcril_db_query_escv_nw_stmt   =
                      "select ESCV, MNC from %s where MCC='%s' and NUMBER='%s'";

/* Query statement to query operator type using mcc and mnc */
static const char* qcril_db_query_operator_type_stmt =
                      "select TYPE from %s where MCC='%s' and MNC='%s'";

/* Insert statement to insert operator type, mcc and mnc */
static const char* qcril_db_insert_operator_type_stmt =
                      "insert into %s values('%s', '%s', '%s')";

/* Update statement to update operator type, mcc and mnc */
static const char* qcril_db_update_operator_type_stmt =
                      "update %s set TYPE='%s' where MCC='%s' and MNC='%s'";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_insert_sw_mbn_file_stmt =
                      "insert into %s(FILE) values('%s')";

/* update statement to update string column to mbn file row*/
static const char* qcril_db_update_sw_mbn_file_str_stmt =
                      "update %s set %s='%s' where FILE='%s'";

/* update statement to update int column to mbn file row*/
static const char* qcril_db_update_sw_mbn_file_int_stmt =
                      "update %s set %s='%d' where FILE='%s'";

/* Query statement to query from mbn file table*/
#if 0
static char* qcril_db_query_sw_mbn_file_stmt =
                      "select %s from %s where %s='%s'";
#endif

/* Insert statement to insert mbn file row*/
static const char* qcril_db_insert_hw_mbn_file_stmt =
                      "insert into %s(FILE) values('%s')";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_update_hw_mbn_file_str_stmt =
                      "update %s set %s='%s' where FILE='%s'";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_update_hw_mbn_file_int_stmt =
                      "update %s set %s='%d' where FILE='%s'";

/* Query statement to query from mbn file table based upon two arguments */
// static char* qcril_db_query_mbn_file_stmt_two_args =
//               "select %s from %s where %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon three int arguments */
static const char* qcril_db_query_mbn_file_stmt_three_int_args =
                      "select %s from %s where %s='%d' and %s='%d' and %s='%d'";

/* Query statement to query from mbn file table based upon three arguments */
static const char* qcril_db_query_mbn_file_stmt_three_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon four arguments */
static const char* qcril_db_query_mbn_file_stmt_four_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon five arguments */
static const char* qcril_db_query_mbn_file_stmt_five_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table*/
static const char* qcril_db_query_hw_mbn_file_stmt =
                      "select %s from %s where %s='%s'";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_insert_sw_mbn_iin_stmt =
                      "insert into %s(FILE, MCFG_IIN, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s')";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_insert_sw_mbn_long_iin_stmt =
                      "insert into %s(FILE, MCFG_LONG_IIN, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s')";

/* Insert statement to insert mbn file row*/
static const char* qcril_db_insert_sw_mbn_mcc_mnc_stmt =
                      "insert into %s(FILE, MCC, MNC, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s', '%s')";

/* Query statement from mbn file iin row*/
static const char* qcril_db_query_sw_mbn_str_stmt =
                      "select %s from %s where %s='%s'";

/* Query statement from sig config*/
static const char* qcril_db_query_sig_config_stmt =
                      "select DELTA from %s where SIG_CONFIG_TYPE='%s'";

/* Query user preference from manual prov table statement*/
static const char* qcril_db_query_user_pref_stmt =
                      "select USER_PREF from %s where ICCID='%s'";

/* Query statement to insert new entry in manual prov table */
static const char* qcril_db_insert_new_iccid_stmt =
                      "insert into %s values('%s', '%d')";

/* update user preference statement */
static const char* qcril_db_update_user_pref_str_stmt =
                      "update %s set USER_PREF='%d' where ICCID='%s'";

/* Query statement to query emergency number */
#if 0
static char* qcril_db_query_emergency_matching_number_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and NUMBER='%s'";
#endif

/* Query statement to query emergency present or not */
static const char* qcril_db_query_number_and_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and NUMBER='%s'";

/* Query statement to query emergency number and mcc */
static const char* qcril_db_query_emergency_matching_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s'";

/* Query statement to query emergency number */
static const char* qcril_db_query_number_from_mcc_mnc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and SERVICE='%s'";

/* Check if table exists statement */
static const char* qcril_db_check_table_str_stmt =
                      "select COUNT(*) from sqlite_master where type = '%s' and name = '%s'";

/* Insert statement to insert mbn imsi exception row*/
static const char* qcril_db_insert_mbn_imsi_exception_stmt =
                      "insert into %s(MCC, MNC, CARRIER_PREF) "\
                      "values('%s', '%s', '%d')";

/* Update carrier pref for mbn imsi exception */
static const char* qcril_db_update_mbn_imsi_exception_stmt =
                      "update %s set CARRIER_PREF='%d' where MCC='%s' and MNC='%s'";

/* Query statement to query carrier pref for mbn imsi exception */
static const char* qcril_db_query_carrier_pref_from_mbn_imsi_exception_stmt =
                      "select CARRIER_PREF from %s where MCC='%s' and MNC='%s'";

/* Insert statement to insert mbn iccid exception row*/
static const char* qcril_db_insert_mbn_iccid_exception_stmt =
                      "insert into %s(ICCID, CARRIER_PREF) "\
                      "values('%s', '%d')";

/* Update carrier pref for mbn iccid exception */
static const char* qcril_db_update_mbn_iccid_exception_stmt =
                      "update %s set CARRIER_PREF='%d' where ICCID='%s'";

/* Query statement to query carrier pref for mbn iccid exception */
static const char* qcril_db_query_carrier_pref_from_mbn_iccid_exception_stmt =
                      "select CARRIER_PREF from %s where ICCID='%s'";

/* Query to determine if a WPS call should be placed over CS
   for a given operator */
static const char* qcril_db_query_wps_call_over_cs_stmt =
                       "SELECT COUNT(WPS_OVER_CS) FROM qcril_operator_specific_config "\
                       "WHERE MCC='%s' AND MNC='%s' AND WPS_OVER_CS=1";

/* Query to determine if the force on dc of sms should be enabled
   for a given operator */
static const char* qcril_db_query_force_on_dc_stmt =
                       "SELECT COUNT(FORCE_ON_DC) FROM qcril_cdma_operator_specific_config "\
                       "WHERE MCC='%s' AND MNC='%s' AND FORCE_ON_DC=1";

/* Query to get CLIR preference for an ICCID */
static const char* qcril_db_query_iccid_clir_pref_stmt =
                        "SELECT CLIR_PREF FROM %s WHERE ICCID='%s'";

/* Query to get the oldest entry (i.e. highest ENTRY_AGE) */
static const char* qcril_db_query_iccid_clir_pref_oldest_entry_stmt =
                        "SELECT ICCID FROM %s ORDER BY ENTRY_AGE DESC "\
                        "LIMIT 1";

/* Query to get the number of entries in a table. */
static const char* qcril_db_query_get_entry_count =
                        "SELECT COUNT(*) FROM %s";

/* Insert ICCID CLIR prefence */
static const char* qcril_db_insert_iccid_clir_pref_stmt =
                        "INSERT INTO %s(ICCID, CLIR_PREF, ENTRY_AGE) "\
                        "VALUES('%s', %d, %d)";

/* Update ICCID CLIR Preference */
static const char* qcril_db_update_iccid_clir_pref_stmt =
                        "UPDATE %s SET CLIR_PREF=%d WHERE ICCID='%s'";

/* Update ICCID CLIR Preference Entry Age for all entries except a specific one */
static const char* qcril_db_update_iccid_clir_pref_age_stmt =
                        "UPDATE %s SET ENTRY_AGE = ENTRY_AGE + 1 WHERE ICCID != '%s'";

static const char* qcril_db_delete_iccid_clir_pref_stmt =
                        "DELETE FROM %s WHERE ICCID='%s'";

#ifdef QMI_RIL_UTF
static const char* qcril_db_clear_iccid_clir_pref_table_stmt =
                        "DELETE FROM %s";
#endif // QMI_RIL_UTF

/* Statement to perform Integrity check */
static const char* qcril_db_check_integrity_stmt = "PRAGMA main.integrity_check";

static const char *qcril_db_begin_txn_stmt = "BEGIN";
static const char *qcril_db_commit_txn_stmt = "COMMIT";
static const char *qcril_db_abort_txn_stmt = "ROLLBACK";

/* Emergency numbers retrieved */
static int qcril_db_emergency_numbers_escv_type = 0;
static int qcril_db_query_result = 0;

static int qcril_db_query_wps_call_over_cs_callback
(
    void   *wps_call_over_cs,
    int     num_columns,
    char  **column_values,
    char  **column_names
);

static int qcril_db_upgrade(const char* path, double cur_version, double new_version);

static int qcril_db_check_integrity(sqlite3* db_handle);

/*===========================================================================

  FUNCTION qcril_db_ready

===========================================================================*/
/*!
    @brief
    check if qcril db is ready

    @return
    true or false
*/
/*=========================================================================*/
bool qcril_db_ready()
{
    return (qcril_db_handle != nullptr);
}

/*===========================================================================

  FUNCTION qcril_db_busy_handler

===========================================================================*/
/*!
    @brief
    database busy handler

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_busy_handler(void *ptr, int count)
{
    int ret = 0;
    QCRIL_LOG_INFO("db - %s count - %d", (char*)ptr, count);

    if ( count < MAX_RETRY )
    {
        usleep(QCRIL_DB_SLEEP_TIME);
        ret = 1;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION qcril_db_atel_database_busy_handler

===========================================================================*/
/*!
    @brief
    database busy handler

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_atel_database_busy_handler(void *ptr, int count)
{
    int ret = 0;
    QCRIL_LOG_INFO("db - %s count - %d", (char*)ptr, count);

    if ( count < MAX_RETRY )
    {
        usleep(QCRIL_DB_SLEEP_TIME);
        ret = 1;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

int qcril_db_begin(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_begin_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_begin_txn_stmt);
        QCRIL_LOG_ERROR("Could not start transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

int qcril_db_commit(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_commit_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_commit_txn_stmt);
        QCRIL_LOG_ERROR("Could not commit transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

int qcril_db_abort(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_abort_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_abort_txn_stmt);
        QCRIL_LOG_ERROR("Could not abort transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION qcril_db_check_if_table_exists

===========================================================================*/
/*!
    @brief
    Check if qcril.db has table or not.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_check_if_table_exists(const char *tname)
{
    char    stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret = 0;
    int     res = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !tname )
        {
            QCRIL_LOG_WARN("Invalid table name - null");
            break;
        }

        snprintf( stmt,
                  sizeof(stmt),
                  qcril_db_check_table_str_stmt,
                  "table",
                  tname );

        /* Query database for if table exist.if ret = 0,
        ** table doesn't exist. if ret != 0 table exist.*/
        if (SQLITE_OK !=
                 (res = qcril_db_sqlite3_exec(
                          stmt, qcril_db_table_query_callback_integer, &ret)))
        {
            QCRIL_LOG_ERROR("stmt %s", stmt);
            QCRIL_LOG_ERROR("Could not check if table exist %d", res);
        }
        else
        {
            res = ( ret == 0 ) ? FALSE : TRUE;
        }

    }while(FALSE);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_retrieve_emergency_num_callback

===========================================================================*/
/*!
    @brief
    Retireves emergency number from db output.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_retrieve_emergency_num_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int     tmp_len = 0;
    char   *ptr;
    uint32_t len;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
        len     = *((uint32_t*)data);
        ptr     = (char*)data + (RESERVED_TO_STORE_LENGTH + len);
        if (argc == 1 && (len < QCRIL_MAX_EMERGENCY_NUMBERS_LEN) && argv[0])
        {
            if (len != 0)
            {
                tmp_len = snprintf(ptr,
                            (QCRIL_MAX_EMERGENCY_NUMBERS_LEN - len), "%s", ",");
                len = len + tmp_len;
                ptr = ptr + tmp_len;
            }

            tmp_len = snprintf(ptr,
                         (QCRIL_MAX_EMERGENCY_NUMBERS_LEN - len), "%s", argv[0]);

            len = len + tmp_len;
            *((int*)data) = len;
        }
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_num_and_mcc_callback

===========================================================================*/
/*!
    @brief
    checks existence of emergency number and mcc in a table.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_num_and_mcc_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{

    QCRIL_NOTUSED(azColName);

    if ((argc > 0) && strlen(argv[0]) > 0 && data)
    {
        *(int*)data = 1;
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_escv_callback

===========================================================================*/
/*!
    @brief
    checks existence of escv row in a table.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_escv_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = -1;
    qcril_db_escv_in_out *result = static_cast<qcril_db_escv_in_out *>(data);
    int escv = 0;

    QCRIL_NOTUSED(azColName);

    if (result)
    {
        escv = atoi(argv[0]);
        if (argc == 1)
        {
            QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
            if (escv >= 0)
            {
                result->escv_type  = escv;
                ret = 0;
            }
        }
        else if (argc == 2)
        {
            QCRIL_LOG_INFO("argc %d argv[0] %s argv[1] %s",
                                 argc, argv[0] ? argv[0] : "null",
                                 argv[1] ? argv[1] : "null");
            if (escv >= 0)
            {
                if ((result->mnc) && (argv[1]))
                {
                    if (!strcmp(result->mnc, argv[1]))
                    {
                        result->escv_type  = escv;
                    }
                }
                else
                {
                    result->escv_type  = escv;
                }
                ret = 0;
             }
         }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback

===========================================================================*/
/*!
    @brief
    Retireves ims_address from mcc and emergency number from db output.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    char   *ptr;
    int     len;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
        ptr = (char*)data;
        *ptr = 0;

        if (argc == 1 && argv[0])
        {
            len = strlen(argv[0]);
            if (len > 0 && len < QCRIL_MAX_IMS_ADDRESS_LEN)
            {
                snprintf(ptr,
                         QCRIL_MAX_IMS_ADDRESS_LEN,
                         "%s",
                         argv[0]);
            }
        }
    }

    return 0;
}

/*===========================================================================

  FUNCTION qcril_db_check_prebuilt_and_wait

============================================================================*/
/*!
    @brief
    Mostly, this function will return "qcril.db" string directly at first check.
    Two exception here:
    1) For the very first boot, it will copy prebuilt db file to "qcril.db".
    2) For disk encryption, it will copy prebuilt db file to "qcril.db"

    @return
    E_SUCCESS: qcril.db exists, or succceed to copy from prebuilt db file
    E_FAILURE: no qcril.db file. RIL needs to create it by itself

    TODO: need to add "version check" support, which is very useful for OTA
    update
*/
/*==========================================================================*/
static int qcril_db_check_prebuilt_and_wait
(
    int id
)
{
    int retries = 0;
    int ret = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();

    // first check if qcril.db is available for R & W
    do
    {
        if ( !access( QCRIL_DATABASE_NAME, R_OK | W_OK ) )
        {
            QCRIL_LOG_INFO("%s already available", QCRIL_DATABASE_NAME);
            return E_SUCCESS;
        }
        else if(id == QCRIL_DEFAULT_INSTANCE_ID)
        {
            /* Primary RIL instance, so allow to create DB */
            break;
        }

        //RIL2/3 will wait here for DB to be created.
        retries++;
        QCRIL_LOG_INFO("Waiting for DB creation. Retry: %d",retries);
        usleep(QCRIL_DB_SLEEP_TIME);
    } while(retries <= MAX_RETRY);

    if(id == QCRIL_DEFAULT_INSTANCE_ID)
    {
        // Expected db file does not exist in data partition
        // Check if prebuilt db file is available
        ret = E_FAILURE;
        retries = 0;
        do {
            if ( !access( QCRIL_PREBUILT_DB_NAME, R_OK ) )
            {
                ret = E_SUCCESS;
                break;
            }
            QCRIL_LOG_INFO("%s is not available to RIL, reason: %s. Retry: %d",
                QCRIL_PREBUILT_DB_NAME, strerror(errno), retries);
            retries++;
            usleep(QCRIL_DB_SLEEP_TIME);
        } while (retries <= MAX_RETRY);

        if ( ret == E_SUCCESS )
        {
            QCRIL_LOG_INFO("Prebuilt db available. Copy to %s",QCRIL_DATABASE_NAME);

            //when copying db reset db_upgrade property so that upgrade will
            //happen again after device exit encryption
            QCRIL_LOG_INFO("Setting the db state to %d", DbState::State::INIT);
            DbState::getInstance()->setDbState(DbState::State::INIT);

            // do copy work
            ret = qcril_file_copy_file(QCRIL_PREBUILT_DB_NAME,
                    QCRIL_DATABASE_NAME);
            if (ret)
            {
                QCRIL_LOG_ERROR("Failed to copy %s to %s",
                    QCRIL_PREBUILT_DB_NAME, QCRIL_DATABASE_NAME);
                ret = E_FAILURE;
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return ret;
}

void qcril_db_cleanup_databases()
{
    std::array<const char*, 2> file_names = { QCRIL_BACKUP_DATABASE_NAME,
            QCRIL_DATABASE_NAME };
    for (const auto& file_name : file_names)
    {
        int ret = unlink(file_name);
        if (ret == -1)
        {
            QCRIL_LOG_ERROR("Failed to delete file %s, errno %d.",
                file_name, errno);
        }
    }
}

int qcril_db_copy_from_backup_or_prebuilt()
{
    const char* src_db = nullptr;
    sqlite3 *backup_handle = nullptr;
    sqlite3 *prebuilt_handle = nullptr;

    // check integrity of backup database
    auto rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                              &backup_handle,
                              SQLITE_OPEN_READONLY, NULL);
    if (rc == SQLITE_OK && qcril_db_check_integrity(backup_handle) == TRUE)
    {
        src_db = QCRIL_BACKUP_DATABASE_NAME;
    }
    else
    {
        // check integrity of prebuilt database
        rc = sqlite3_open_v2(QCRIL_PREBUILT_DB_NAME,
                             &prebuilt_handle,
                             SQLITE_OPEN_READONLY, NULL);
        if (rc == SQLITE_OK && qcril_db_check_integrity(prebuilt_handle) == TRUE)
        {
            src_db = QCRIL_PREBUILT_DB_NAME;
        }
    }

    if (backup_handle)
    {
        (void)sqlite3_close(backup_handle);
    }
    if (prebuilt_handle)
    {
        (void)sqlite3_close(prebuilt_handle);
    }

    if (src_db)
    {
        return (qcril_file_copy_file(src_db, QCRIL_DATABASE_NAME) ?
            E_FAILURE : E_SUCCESS);
    }
    else
    {
        return E_FAILURE;
    }
}

/*===========================================================================

  FUNCTION  qcril_db_update_cur_modem_version

===========================================================================*/
/*!
    @brief
    Update modem version as current version.

    @return
    none
*/
/*=========================================================================*/
void qcril_db_update_cur_modem_version
(
    void
)
{
    char cur_ver_info[PROPERTY_VALUE_MAX]     = {0};

    qcril_file_read_string_from_file(QCRIL_DB_MCFG_VER_INFO,
                                     cur_ver_info, sizeof(cur_ver_info) - 1,
                                     sizeof(cur_ver_info) - 1);
    std::string prev_ver_info;
    (void)qcril_config_get(MCFG_VER_INFO, prev_ver_info);
    if (prev_ver_info.compare(cur_ver_info))
    {
        QCRIL_LOG_DEBUG("updating cur version");
        qcril_config_set(MCFG_VER_INFO, cur_ver_info);
    }

    return;
}

/*===========================================================================

  FUNCTION  qcril_db_is_modem_image_updated

===========================================================================*/
/*!
    @brief
    Check if modem image is udated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_modem_image_updated
(
    void
)
{
    boolean is_ver_updated    = FALSE;
    char cur_ver_info[PROPERTY_VALUE_MAX]     = {0};

    qcril_file_read_string_from_file(QCRIL_DB_MCFG_VER_INFO,
                                     cur_ver_info, sizeof(cur_ver_info) - 1,
                                     sizeof(cur_ver_info) - 1);
    std::string prev_ver_info;
    (void)qcril_config_get(MCFG_VER_INFO, prev_ver_info);
    if (prev_ver_info.compare(cur_ver_info))
    {
        QCRIL_LOG_DEBUG("version info updated");
        is_ver_updated = TRUE;
    }

    QCRIL_LOG_DEBUG("prev_ver_info: %s, cur_ver_info: %s", prev_ver_info.c_str(), cur_ver_info);
    return is_ver_updated;
}

/*===========================================================================

  FUNCTION  qcril_db_is_sw_mbn_reevaluation_needed

===========================================================================*/
/*!
    @brief
    Check and update if software mcfg tables needs to be re-evaluated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_sw_mbn_reevaluation_needed
(
    void
)
{
    boolean is_ver_updated    = FALSE;

    std::string sw_loaded;
    (void)qcril_config_get(SW_MBN_LOADED, sw_loaded);
    QCRIL_LOG_DEBUG("is sw db loaded = %d", sw_loaded.c_str());

    auto is_sw_db_loaded = sw_loaded.compare("1");
    if (is_sw_db_loaded)
    {
        is_ver_updated = qcril_db_is_modem_image_updated();
        if (is_ver_updated)
        {
            qcril_config_set(SW_MBN_LOADED, std::to_string(0));
        }
    }

    return (!is_sw_db_loaded || is_ver_updated);
}

/*===========================================================================

  FUNCTION  qcril_db_is_hw_mbn_reevaluation_needed

===========================================================================*/
/*!
    @brief
    Check and update if hardware mcfg tables needs to be re-evaluated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_hw_mbn_reevaluation_needed
(
    void
)
{
    boolean is_ver_updated    = FALSE;

    std::string hw_loaded;
    (void)qcril_config_get(HW_MBN_LOADED, hw_loaded);
    QCRIL_LOG_DEBUG("is hw db loaded = %s", hw_loaded.c_str());

    auto is_hw_db_loaded = hw_loaded.compare("1");
    if (is_hw_db_loaded)
    {
        is_ver_updated = qcril_db_is_modem_image_updated();
        if (is_ver_updated)
        {
            qcril_config_set(HW_MBN_LOADED, std::to_string(0));
        }
    }

    return (!is_hw_db_loaded || is_ver_updated);
}

/*===========================================================================

  FUNCTION  qcril_db_evaluate_drop

===========================================================================*/
/*!
    @brief
    Check and update if table needs to be dropped.

    @return
    none
*/
/*=========================================================================*/
void qcril_db_evaluate_drop
(
    void
)
{
    if (!qcril_db_is_hw_mbn_reevaluation_needed())
    {
        qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].drop_during_bootup = 0;
    }

    if (!qcril_db_is_sw_mbn_reevaluation_needed())
    {
        qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].drop_during_bootup = 0;
        qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].drop_during_bootup = 0;
        qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].drop_during_bootup = 0;
    }
}

/*===========================================================================

  FUNCTION  qcril_db_upgrade_get_version

===========================================================================*/
/*!
    @brief
    Get the current version of the db from the properties table

    @return the version or -1 if not found
    None
*/
/*=========================================================================*/
double qcril_db_upgrade_get_version(uint8_t vendor)
{
    const char get_version_sql[] =
        "SELECT DEF_VAL from " QCRIL_PROPERTIES_TABLE_NAME " where PROPERTY = 'qcrildb_version'";
    double version = -1;
    int rc = -1;
    sqlite3 *db_handle = NULL;
    sqlite3_stmt *stmt = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (vendor)
        {
            rc = sqlite3_open_v2("file:" QCRIL_PREBUILT_DB_NAME "?immutable=1", &db_handle,
                                 SQLITE_OPEN_READONLY | SQLITE_OPEN_URI, NULL);
            if (SQLITE_OK != rc)
            {
                QCRIL_LOG_ERROR("Failed to open " QCRIL_PREBUILT_DB_NAME ": %d\n", rc);
                break;
            }
        }
        else
        {
            db_handle = qcril_db_handle;
        }

        rc = sqlite3_prepare_v2(db_handle,
                get_version_sql,
                sizeof(get_version_sql),
                &stmt,
                NULL);

        if (rc != SQLITE_OK)
        {
            QCRIL_LOG_ERROR("Unable to prepare query %s", get_version_sql);
            break;
        }

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW)
        {
            break;
        }

        version = sqlite3_column_double(stmt, 0);

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW)
        {
            QCRIL_LOG_WARN("Unexpected multiple results for query %s. Using the first only", get_version_sql);
        }
        sqlite3_finalize(stmt);

    } while (0);

    if (vendor) {
        // db_handle could be NULL if the vendor DB failed to open.
        // However, calling sqlite3_close() with a NULL pointer
        // argument is a harmless no-op.
        sqlite3_close(db_handle);
    }

    return version;
}

/**
  * qcril_db_upgrade_version_and_name: Obtain the version and name from
  *         an upgrade file name, which should have the format
  *         <version>_<name>.sql
  *
  * NULL can be passed to either version or name, or even to both,
  * in which case only a check will be performed on whether the name
  * confirms to the specified format.
  *
  * @param filename The file name to parse
  * @param version Pointer to int where to store the version.
  * @param name Pointer to char * where to store the name (sans the .sql portion)
  *
  * @return Non-zero on failure.
  */
int qcril_db_upgrade_version_and_name(const char *filename, double *version, char **name)
{
    int ret = 0;
    double local_ver;
    char *local_name = NULL;
    char extn[5] = {0,0,0,0,0};
    do
    {
        if (!filename)
        {
            QCRIL_LOG_ERROR("File name not provided");
            break;
        }
        ret = sscanf(filename, "%04lf_%m[^.]%4s", &local_ver, &local_name, extn);
        QCRIL_LOG_DEBUG("File %s. ver: %lf. name: %s, extn: %s", filename, local_ver, local_name, extn);

        if (ret != 3 || strncmp(extn, ".sql", 4))
        {
            /*Shouldn't free the string when not parsed correctly*/
            ret = 0; /* Covers the case when ret=3 but not a .sql file */
            break;
        }
        if (version)
        {
            *version = local_ver;
        }
        if (name)
        {
            *name = local_name;
        }
        else
        {
            qcril_free(local_name);
        }
    } while (0);

    return 3 - ret;
}

int qcril_upgrade_file_sort(const struct dirent **a, const struct dirent **b)
{
    double va = -1, vb = -1;
    char *namea = NULL, *nameb = NULL;
    int ret = 0;
    int rca = 0;
    int rcb = 0;

    rca = qcril_db_upgrade_version_and_name(a[0]->d_name, &va, &namea);
    rcb = qcril_db_upgrade_version_and_name(b[0]->d_name, &vb, &nameb);
    ret = (va == vb) ? 0 : ((va > vb) ? 1 : -1);
    if (!ret && !rca && !rcb)
    {
        ret = strcmp(namea, nameb);
    }
    if (namea) qcril_free(namea);
    if (nameb) qcril_free(nameb);

    return ret;
}
int qcril_file_filter(const struct dirent *de)
{
    int ret = 0;
    if (de->d_type == DT_REG &&
                    (qcril_db_upgrade_version_and_name(de->d_name, NULL, NULL) == 0))
    {
        ret = 1;
    }
    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_upgrade

===========================================================================*/
/*!
    @brief
    Perform an upgrade by applying the necessary upgrade scripts

    @return
    Non zero on failure
*/
/*=========================================================================*/
int qcril_db_upgrade(const char* path, double cur_version, double new_version)
{
    int rc = SQLITE_OK;
    int num_files = 0;
    struct dirent **file_names = NULL;
    int dirfd = -1;
    char error[256];

    QCRIL_LOG_FUNC_ENTRY();
    memset(error, 0, sizeof(error));

    do {
        dirfd = open(path, O_RDONLY|O_DIRECTORY);
        if (dirfd < 0)
        {
            (void)strerror_r(errno, error, sizeof(error));
            QCRIL_LOG_ERROR("Unable to open %s as a folder: %s",
                    path, error);
            rc = -1;
            break;
        }

#ifndef RIL_FOR_OPENWRT
        num_files = scandirat(dirfd, ".",
#else
        num_files = scandir(path,
#endif
                &file_names,
                qcril_file_filter,
                qcril_upgrade_file_sort);

        if (num_files < 0)
        {
            rc = -1;
            QCRIL_LOG_ERROR("Unable to retrieve files from %s", path);
            break;
        }

        for( int i = 0; i < num_files; i++)
        {
            size_t size = 0;
            void *address;
            char *errmsg = NULL;
            double ver = 0;

            qcril_db_upgrade_version_and_name(file_names[i]->d_name, &ver, NULL);
            if (ver <= cur_version) continue;
            if (ver > new_version) break;

            QCRIL_LOG_INFO("Updating database by using %s", file_names[i]->d_name);
            address = qcril_file_open_mmap_at_for_read(
                    dirfd,
                    file_names[i]->d_name,
                    &size);
            if (address == MAP_FAILED)
            {
                QCRIL_LOG_ERROR("Ignoring file %s", file_names[i]->d_name);
                continue;
            }

            rc = sqlite3_exec(
                    qcril_db_handle,
                    (const char *)address,
                    NULL,
                    NULL,
                    &errmsg);
            if (rc != SQLITE_OK)
            {
                QCRIL_LOG_ERROR("Error executing upgrade step %s: %s",
                        file_names[i]->d_name,
                        errmsg);
                sqlite3_free(errmsg);
                break;
            }
            QCRIL_LOG_INFO("Successful upgrade step %s",file_names[i]->d_name);
            free(file_names[i]);
        }
        close(dirfd);
    } while(0);
    free(file_names);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

int qcril_db_prepare_stmt(std::string query, bool persist, sqlite3_stmt *&out) {
    int rc = SQLITE_ERROR;

    if (qcril_db_handle != nullptr) {
        int flags = 0;
        if (persist) {
            flags |= SQLITE_PREPARE_PERSISTENT;
        }
        rc = sqlite3_prepare_v3(qcril_db_handle,
                query.c_str(),
                query.length(),
                flags,
                &out,
                nullptr);
        if (rc) {
            QCRIL_LOG_ERROR("Error %d (error_code: %d, extended: %d) while preparing statement: %s.",
                rc, sqlite3_errcode(qcril_db_handle), sqlite3_extended_errcode(qcril_db_handle),
                sqlite3_errmsg(qcril_db_handle));
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

int qcril_db_finalize_stmt(sqlite3_stmt *&stmt) {
    int rc = SQLITE_ERROR;
    QCRIL_LOG_FUNC_ENTRY();
    if (qcril_db_handle != nullptr) {
        rc = sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

const char *qcril_db_errmsg() {
    const char *err = nullptr;
    QCRIL_LOG_FUNC_ENTRY();
    if (qcril_db_handle != nullptr) {
        err = sqlite3_errmsg(qcril_db_handle);
    }
    return err;
}

int qcril_db_open_handle(sqlite3 *&handle)
{
    int retry_count = 0;
    int res = SQLITE_ERROR;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        /* open qcril DB */
        if (SQLITE_OK !=
                 (res = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                         &handle,
                          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)))
        {
            QCRIL_LOG_ERROR("Failed to open qcril db %d\n", res);
            usleep(QCRIL_DB_SLEEP_TIME);
            retry_count++;
        }
        else
        {
            QCRIL_LOG_DEBUG("qcril db open successful\n");
            break;
        }
        QCRIL_LOG_DEBUG("db open retry - %d", retry_count);
    } while(retry_count < MAX_RETRY);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

int qcril_db_open() {
    return qcril_db_open_handle(qcril_db_handle);
}

/*===========================================================================

  FUNCTION  qcril_db_check_and_upgrade

===========================================================================*/
/*!
    @brief
    Compare the current db version with the one in the vendor partition
    and upgrade if necessary

    @return
    None
*/
/*=========================================================================*/
bool qcril_db_check_and_upgrade
(
)
{
#define MAX_RETRY_TIMES 10
    const char presql[] = "BEGIN EXCLUSIVE TRANSACTION;";
    int rc = -1;
    char *error = NULL;
    bool db_upgraded = false;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        int ntimes = 0;
        // in case of other error other than busy, return immediately
        while ((rc = sqlite3_exec(qcril_db_handle, presql, NULL, NULL,
                &error)) == SQLITE_BUSY && ++ntimes <= MAX_RETRY_TIMES)
        {
            // usleep(QCRIL_DB_SLEEP_TIME);
            if (!error) continue;
            QCRIL_LOG_ERROR("Unable to begin exclusive transaction for " QCRIL_DATABASE_NAME ": %s", error);
            sqlite3_free(error);
            error = NULL;
        }
        if (rc != SQLITE_OK) return db_upgraded;

        auto vnd_version = qcril_db_upgrade_get_version(TRUE);
        auto local_version = qcril_db_upgrade_get_version(FALSE);

        QCRIL_LOG_DEBUG("Vendor version: %f. Local version: %f",
                vnd_version,
                local_version);
        if (vnd_version < 0) {
            QCRIL_LOG_ERROR("Unable to find vendor version. Not upgrading");
            break;
        }

        if (local_version < 0) local_version = 0;

        if (local_version < vnd_version)
        {
            QCRIL_LOG_INFO("Performing upgrade");
            rc = qcril_db_upgrade(UPGRADE_SCRIPTS_OTHER_PATH, local_version, vnd_version);
            if (rc != SQLITE_OK) break;
            rc = qcril_db_upgrade(UPGRADE_SCRIPTS_CONFIG_PATH, local_version, vnd_version);
            if (rc != SQLITE_OK) break;
            db_upgraded = true;
        }
        else if (local_version > vnd_version)
        {
            QCRIL_LOG_INFO("Downgrading by removing the existing db");
            sqlite3_close(qcril_db_handle);
            unlink(QCRIL_DATABASE_NAME);
            rc = qcril_db_open();
            if (rc != SQLITE_OK) {
                QCRIL_LOG_ERROR("Unable to reopen database after unlinking");
            }
            return db_upgraded;
        }
    } while(0);

    const char* postsql = (rc != SQLITE_OK) ? "ROLLBACK;" : "COMMIT TRANSACTION;";
    rc = sqlite3_exec(qcril_db_handle, postsql, NULL, NULL, &error);
    if (rc != SQLITE_OK)
    {
        QCRIL_LOG_ERROR("Unable to set locking mode to NORMAL for " QCRIL_DATABASE_NAME ": %s", error);
        sqlite3_free(error);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(db_upgraded);
    return db_upgraded;
} /* qcril_db_check_and_upgrade */

/*===========================================================================

  FUNCTION  qcril_db_init

===========================================================================*/
/*!
    @brief
    Initialize qcril db and tables.

    @return
    0 if function is successful.
*/
/*=========================================================================*/

int qcril_db_init
(
    int id
)
{
    #ifdef USE_QCRIL_NR
    return SQLITE_OK;
    #endif
    int     res     = SQLITE_OK;
    char   *zErrMsg = NULL;

    char    create_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     i;
    const char *drop_stmt_fmt = "drop table if exists '%s'";
    char    drop_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    res = sqlite3_threadsafe();
    QCRIL_LOG_INFO("Sqlite threadsafe: %d\n", res);

    qcril_db_check_prebuilt_and_wait(id);

    /* initialize sqlite in serialized mode */
    if(SQLITE_OK != (res = sqlite3_config(SQLITE_CONFIG_SERIALIZED)))
    {
        QCRIL_LOG_ERROR("Failed to configure sqlite3 in serialized mode: %d\n", res);
    }

    /* initialize sqlite engine */
    if (SQLITE_OK != (res = sqlite3_initialize()))
    {
        QCRIL_LOG_ERROR("Failed to initialize sqlite3: %d\n", res);
    }
    else
    {
        if ( id == QCRIL_DEFAULT_INSTANCE_ID )
        {
            auto rc = qcril_db_open();
            if (rc == SQLITE_OK)
            {
                // check integrity and restore using sqlite backup interface
                rc = qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);
                if (rc != SQLITE_OK)
                {
                    (void)sqlite3_close(qcril_db_handle);
                    // copying from from "file level"
                    if (qcril_db_copy_from_backup_or_prebuilt() == E_SUCCESS)
                    {
                        rc = qcril_db_open();
                    }
                }
            }

            if (rc != SQLITE_OK)
            {
                QCRIL_LOG_ERROR("FATAL: failed to open ril database. Cleanup local"
                    "created databases, and all RIL settings are going to reset");
                // clean up the database qcril created
                qcril_db_cleanup_databases();
                assert(0);
            }

            /* for allowing multiple RILDs to access database,
            ** set busy handler. This will call qcril_db_busy_handler
            ** and will retry till qcril_db_busy_handler returns 0.*/
            QCRIL_LOG_INFO("register qcril db busy handler");
            sqlite3_busy_handler(qcril_db_handle, qcril_db_busy_handler, const_cast<void *>(static_cast<const void *>(qcril_db)));

            qcril_db_check_and_upgrade();
            qcril_db_evaluate_drop();
            /* Initialize provisioning table */

            for (i = QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC;
                 i < (QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX); i++)
            {
                if (qcril_db_emergency_number_tables[i].table_name &&
                     qcril_db_emergency_number_tables[i].create_stmt)
                {
                    snprintf(create_stmt, sizeof(create_stmt),
                             qcril_db_emergency_number_tables[i].create_stmt,
                             qcril_db_emergency_number_tables[i].table_name);

                    /* create qcril DB tables */
                    if (SQLITE_OK !=
                             (res = sqlite3_exec(qcril_db_handle,
                                     create_stmt, NULL, NULL, &zErrMsg)))
                    {
                        if (zErrMsg)
                        {
                            QCRIL_LOG_ERROR("stmt %s", create_stmt);
                            QCRIL_LOG_ERROR("Could not create table %d %s",
                                             res, zErrMsg);
                            sqlite3_free(zErrMsg);
                        }
                    }
                }
                memset(create_stmt,0,sizeof(create_stmt));
            }

            for (i = QCRIL_DB_TABLE_FIRST;
                 i < QCRIL_DB_TABLE_MAX; i++)
            {
                if (qcril_db_tables[i].table_name &&
                     qcril_db_tables[i].create_stmt)
                {
#ifndef QMI_RIL_UTF
                    if (qcril_db_tables[i].drop_during_bootup)
                    {
                        QCRIL_LOG_DEBUG("dropping %d", i);
                        snprintf(drop_stmt, sizeof(drop_stmt),
                                 drop_stmt_fmt,
                                 qcril_db_tables[i].table_name);

                        if (SQLITE_OK !=
                        (res = sqlite3_exec(qcril_db_handle,
                                drop_stmt, NULL, NULL, &zErrMsg)))
                        {
                            if (zErrMsg)
                            {
                                QCRIL_LOG_ERROR("stmt %s", drop_stmt);
                                QCRIL_LOG_ERROR("Could not drop table %d %s",
                                                 res, zErrMsg);
                                sqlite3_free(zErrMsg);
                            }
                        }
                    }
#endif

                    snprintf(create_stmt, sizeof(create_stmt),
                             qcril_db_tables[i].create_stmt,
                             qcril_db_tables[i].table_name);

                    /* create qcril DB tables */
                    if (SQLITE_OK !=
                             (res = sqlite3_exec(qcril_db_handle,
                                     create_stmt, NULL, NULL, &zErrMsg)))
                    {
                        if (zErrMsg)
                        {
                            QCRIL_LOG_ERROR("stmt %s", create_stmt);
                            QCRIL_LOG_ERROR("Could not create table %d %s",
                                             res, zErrMsg);
                            sqlite3_free(zErrMsg);
                        }
                    }
                }

                memset(create_stmt,0,sizeof(create_stmt));
                memset(drop_stmt, 0, sizeof(drop_stmt));
            }

            qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);

            DbState::getInstance()->setDbState(DbState::State::READY);
        }
        else
        {
            QCRIL_LOG_INFO("Wait for db init done");
            /* RIL[1,2] will wait for RIL0 to upgrade qcril database. */
            DbState::getInstance()->waitForDbState(DbState::State::READY, 100);
            QCRIL_LOG_INFO("Get confirmation that db init is done");
            res = qcril_db_open();
            if (res == SQLITE_OK)
            {
                QCRIL_LOG_INFO("register qcril db busy handler");
                sqlite3_busy_handler(qcril_db_handle, qcril_db_busy_handler,
                        const_cast<void *>(static_cast<const void *>(qcril_db)));
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc existence in db and retireves emergency number from db.

    @return
    1 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_mcc_part_of_emergency_numbers_table
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    char *emergency_num
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    char    emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN + RESERVED_TO_STORE_LENGTH] = {0};

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s MNC %s", source, mcc? mcc: "null", mnc? mnc: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc &&
         qcril_db_emergency_number_tables[source].table_name   &&
         emergency_num)
    {
        if ( is_mcc_valid && is_mnc_valid && mcc && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_emergency_matching_mcc_mnc_stmt,
                     qcril_db_emergency_number_tables[source].table_name, mcc, mnc);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_emergency_number_stmt,
                     qcril_db_emergency_number_tables[source].table_name, mcc);
        }

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_emergency_num_callback,
                                  emergency_numbers)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            if ( *((int*)emergency_numbers) > 0 )
            {
                res = TRUE;
                strlcpy(emergency_num, emergency_numbers + RESERVED_TO_STORE_LENGTH,
                         QCRIL_MAX_EMERGENCY_NUMBERS_LEN);
                QCRIL_LOG_INFO("Emergency numbers %s", emergency_num);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state

===========================================================================*/
/*!
    @brief
    Checks for mcc & service state existence in db and retireves
    emergency number from db.

    @return
    1 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    const char *service,
    char *emergency_num
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    char    emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN + RESERVED_TO_STORE_LENGTH] = {0};

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s MNC %s", source, mcc? mcc: "null", mnc? mnc: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc &&
         service && qcril_db_emergency_number_tables[source].table_name
         && emergency_num)
    {
        if ( is_mnc_valid && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_from_mcc_mnc_and_service_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     mnc,
                     service);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_from_mcc_and_service_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     service);
        }

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_emergency_num_callback,
                                  emergency_numbers)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            if ( *((int*)emergency_numbers) > 0 )
            {
                res = TRUE;
                strlcpy(emergency_num, emergency_numbers + RESERVED_TO_STORE_LENGTH,
                         QCRIL_MAX_EMERGENCY_NUMBERS_LEN);
                QCRIL_LOG_INFO("Emergency numbers %s", emergency_num);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_number_from_emergency_table

===========================================================================*/
/*!
    @brief
    Checks whether number present in table as per query

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_query_number_from_emergency_table
(
    char *query,
    int  *is_num_present
)
{
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();
    if (!qcril_db_handle)
        return ret;

    if ( query && is_num_present )
    {
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                         qcril_db_check_num_and_mcc_callback,
                                         is_num_present)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_is_number_mcc_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc and number existence in db

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_number_mcc_part_of_emergency_numbers_table
(
    const char *emergency_num,
    int  is_mcc_valid,
    const char *mcc,
    int  is_mnc_valid,
    const char *mnc,
    qmi_ril_custom_emergency_numbers_source_type source
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc && emergency_num &&
         qcril_db_emergency_number_tables[source].table_name)
    {
        QCRIL_LOG_INFO("Source %d MCC %s MNC %s emergency num %s", source, mcc, mnc? mnc: "null",emergency_num);
        memset(qcril_db_emergency_numbers, 0, sizeof(qcril_db_emergency_numbers));

        if ( is_mnc_valid && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_and_mcc_mnc_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     mnc,
                     emergency_num);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_and_mcc_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     emergency_num);
        }

        qcril_db_query_number_from_emergency_table(query,&res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_is_number_mcc_part_of_emergency_numbers_table_with_service_state

===========================================================================*/
/*!
    @brief
    Checks for mcc and number existence based on service state

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_number_mcc_part_of_emergency_numbers_table_with_service_state
(
    const char *emergency_num,
    const char *mcc,
    qmi_ril_custom_emergency_numbers_source_type source,
    const char *service
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         mcc && emergency_num &&
         qcril_db_emergency_number_tables[source].table_name)
    {
        QCRIL_LOG_INFO("Source %d MCC %s emergency num %s", source, mcc, emergency_num);

        snprintf(query, sizeof(query),
                 qcril_db_query_number_and_mcc_and_service_stmt,
                 qcril_db_emergency_number_tables[source].table_name,
                 mcc,
                 emergency_num,
                 service);

        qcril_db_query_number_from_emergency_table(query,&res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}



/*===========================================================================

  FUNCTION  qcril_db_is_ims_address_for_mcc_emergency_number_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc and emergency number existence in db and retrieves
    corresponding ims_address (if present) from db.

    @return
    TRUE if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_ims_address_for_mcc_emergency_number_part_of_emergency_numbers_table
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    char *emergency_num,
    char *ims_address
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    QCRIL_NOTUSED(is_mnc_valid);
    QCRIL_NOTUSED(mnc);

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s emergency_num %s",
                   source,
                   mcc? mcc: "null",
                   emergency_num? emergency_num: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc && emergency_num && ims_address &&
         qcril_db_emergency_number_tables[source].table_name)
    {

        snprintf(query, sizeof(query),
                 qcril_db_query_ims_address_from_mcc_number_stmt,
                 qcril_db_emergency_number_tables[source].table_name,
                 mcc,
                 emergency_num);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback,
                                  ims_address)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            QCRIL_LOG_INFO("ims_address %s", PII(ims_address,"<hidden>"));
            if(strlen(ims_address))
            {
                res = TRUE;
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_escv_type

===========================================================================*/
/*!
    @brief
    Query ESCV type nased upon iin or (mcc, mnc)

    @return
    escv type
*/
/*=========================================================================*/
int qcril_db_query_escv_type
(
    const char *emergency_num,
    const char *iin,
    const char *mcc,
    const char *mnc,
    const char *roam
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = 0;
    int     ret     = SQLITE_OK;
    qcril_db_escv_in_out result;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&result, 0, sizeof(result));
    QCRIL_LOG_INFO(" emergency_num %s iin %s mcc %s mnc %s roam %s",
                     emergency_num? emergency_num : "null",
                     iin? iin : "null",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     roam? roam : "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!emergency_num )
        {
            break;
        }

        if ( iin && roam )
        {
            snprintf(query, sizeof(query),
               qcril_db_query_escv_iin_stmt,
               qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_IIN].table_name,
               iin,
               emergency_num,
               roam);
            result.escv_type = -1;
        }
        else if ( mcc )
        {
            snprintf(query, sizeof(query),
               qcril_db_query_escv_nw_stmt,
               qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_NW].table_name,
               mcc,
               emergency_num);

            if (mnc)
            {
                result.mnc = mnc;
            }
        }
        else
        {
            break;
        }

        QCRIL_LOG_INFO(" Query %s", query);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                         qcril_db_check_escv_callback,
                                         &result)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            res = result.escv_type;
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}


/*===========================================================================

  FUNCTION  qcril_db_query_operator_type_callback

===========================================================================*/
/*!
    @brief
    retrieve operator type.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_operator_type_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            strlcpy(static_cast<char *>(data), argv[0], QCRIL_DB_MAX_OPERATOR_TYPE_LEN);
        }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_query_operator_type

===========================================================================*/
/*!
    @brief
    Query operator type based upon (mcc, mnc)

    @output
    string 3gpp or 3gpp2
*/
/*=========================================================================*/
void qcril_db_query_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(query, sizeof(query),
           qcril_db_query_operator_type_stmt,
           qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
           mcc,
           mnc);

        QCRIL_LOG_INFO(" Query: %s", query);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                        query,
                                        qcril_db_query_operator_type_callback,
                                        operator_type)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }

    } while (0);

    return;
}

/*===========================================================================

  FUNCTION  qcril_db_update_operator_type

===========================================================================*/
/*!
    @brief
    Update operator type, mcc and mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
)
{
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s, operator type %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     operator_type? operator_type: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 qcril_db_update_operator_type_stmt,
                 qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
                 operator_type, mcc, mnc);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_operator_type

===========================================================================*/
/*!
    @brief
    Insert operator type, mcc and mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s, operator type %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     operator_type? operator_type: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_operator_type_stmt,
                 qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
                 mcc, mnc, operator_type);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                    insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file

===========================================================================*/
/*!
    @brief
    Query mbn file based upon query string

    @output
    None
*/
/*=========================================================================*/
void qcril_db_query_mbn_file
(
    char            *query_string,
    sqlite3_callback  call_back,
    unsigned char   *output
)
{
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!query_string || !call_back || !output)
        {
            break;
        }

        QCRIL_LOG_INFO(" Query: %s", query_string);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                   query_string, call_back, output)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }

    } while (0);

    return;
}

/*===========================================================================

  FUNCTION  qcril_db_update_hw_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update hardware mbn file with integer value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_hw_mbn_file_with_int
(
    const char *file,
    const char *type,
    int   value
)
{
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %d",
                     type? type : "null", value);
    do {
        if (!(type && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_int(qcril_db_update_hw_mbn_file_int_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_hw_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update hardware mbn file with string value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_hw_mbn_file_with_str
(
    const char *file,
    const char *type,
    const char *value
)
{
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %s",
                     type? type : "null",
                     value? value : "null");
    do {

        if (!(type && value && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_hw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_hw_mbn_file

===========================================================================*/
/*!
    @brief
    Insert hardware mbn file to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_hw_mbn_file
(
    const char *file
)
{
    IxErrnoType     res     = E_FAILURE;
    char            *new_file = NULL;
    char            *short_name = NULL;
    const char      *type = "SHORT_NAME";
    int             slash_start_pos = 0;
    int             slash_end_pos = 0;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    do {

        if (!file)
        {
            break;
        }


        res = qcril_db_insert_mbn_file(qcril_db_insert_hw_mbn_file_stmt,
                             qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                             file);
        if (res)
        {
            break;
        }
        // Find the last '/'
        slash_end_pos = strrchr(file, '/') - file + 1;
        new_file = static_cast<char *>(malloc(slash_end_pos*sizeof(char)));
        if(NULL != new_file)
        {
            memset(new_file, 0, slash_end_pos);
            memcpy(new_file, file, slash_end_pos - 1);
            QCRIL_LOG_INFO("new_file: %s", new_file);

            // Find the second to the last '/' and get the folder name before *.mbn
            slash_start_pos = strrchr(new_file, '/') - new_file + 1;
            short_name = static_cast<char *>(malloc((slash_end_pos - slash_start_pos)*sizeof(char)));
            if(NULL != short_name)
            {
                memset(short_name, 0, slash_end_pos - slash_start_pos);
                memcpy(short_name, file + slash_start_pos, slash_end_pos - slash_start_pos - 1);
                QCRIL_LOG_INFO("short_name: %s", short_name);
            }
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_hw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, short_name);
        if(NULL != new_file)
        {
            free(new_file);
        }
        if(NULL != short_name)
        {
            free(short_name);
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_sw_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update software mbn file with integer value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_sw_mbn_file_with_int
(
    const char *file,
    const char *type,
    int   value
)
{
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %d",
                     type? type : "null",
                     value);
    do {

        if (!(type && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_int(qcril_db_update_sw_mbn_file_int_stmt,
                                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_sw_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update software mbn file with string value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_sw_mbn_file_with_str
(
    char *file,
    char *type,
    char *value
)
{
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %s",
                     type? type : "null",
                     value? value : "null");
    do {

        if (!(type && value && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_sw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_file

===========================================================================*/
/*!
    @brief
    Insert software mbn file to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_sw_mbn_file
(
    const char *file
)
{
    IxErrnoType     res     = E_FAILURE;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    do {

        if (!file)
        {
            break;
        }

        res = qcril_db_insert_mbn_file(qcril_db_insert_sw_mbn_file_stmt,
                             qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                             file);

    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file

===========================================================================*/
/*!
    @brief
    Update mbn file in db using update statement

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_mbn_file
(
   const char* update_stmt
)
{
    IxErrnoType res     = E_FAILURE;
    int         ret     = SQLITE_OK;

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!update_stmt)
        {
            break;
        }
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not update %d", ret);
            break;
        }
        res = E_SUCCESS;
    }
    while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update mbn file in db by filling values with int

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_mbn_file_with_int
(
    const char *update_stmt_fmt,
    const char *table_name,
    const char *file_name,
    const char *type,
    int value
)
{
    char        update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, file_name : %s",
                     type? type : "null",
                     file_name? file_name : "null");
    do {

        if (!(type && file_name && update_stmt_fmt && table_name))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 update_stmt_fmt,
                 table_name,
                 type, value, file_name);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        res = qcril_db_update_mbn_file(update_stmt);

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update mbn file in db by filling values with string

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_update_mbn_file_with_str
(
    const char *update_stmt_fmt,
    const char *table_name,
    const char *file_name,
    const char *type,
    const char *value
)
{
    char        update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, file_name : %s",
                     type? type : "null",
                     file_name? file_name : "null");
    do {

        if (!(type && value && file_name && update_stmt_fmt && table_name))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 update_stmt_fmt,
                 table_name,
                 type, value, file_name);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        res = qcril_db_update_mbn_file(update_stmt);

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_mbn_file

===========================================================================*/
/*!
    @brief
    Insert mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_mbn_file
(
    const char *insert_stmt_fmt,
    const char *table_name,
    const char *file
)
{
    char            insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType     res     = E_FAILURE;
    int             ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    do {

        if (!qcril_db_handle)
        {
            break;
        }
        if (!file || !insert_stmt_fmt || !table_name)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 insert_stmt_fmt,
                 table_name,
                 file);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_int_callback

===========================================================================*/
/*!
    @brief
    Call back for querying integer value from mbn

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_mbn_int_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(uint32_t*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %x", *(uint32_t*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}
/*===========================================================================

  FUNCTION  qcril_db_query_mbn_string_callback

===========================================================================*/
/*!
    @brief
    Call back for querying string value from mbn

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_mbn_string_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;
    int len = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            len = strlen(argv[0]);
            if (len >= QCRIL_MBN_HW_STR_MAX_SIZE)
            {
                QCRIL_LOG_ERROR("Unexpected length");
            }
            else
            {
                strlcpy(static_cast<char *>(data), argv[0], QCRIL_MBN_HW_STR_MAX_SIZE);
            }
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_hw_type

===========================================================================*/
/*!
    @brief
    Query mbn file name from hw type

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_query_mbn_file_name_from_hw_type
(
    char   **mbn_file_name,
    const char   *device_type
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_ERROR("device_type: %s", device_type? device_type: "null");

        if (!mbn_file_name || !device_type)
        {
            res = E_FAILURE;
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_hw_mbn_file_stmt,
                 "FILE",
                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                 "SHORT_NAME",
                 device_type);

        *mbn_file_name = static_cast<char *>(malloc(QCRIL_MBN_HW_STR_MAX_SIZE*sizeof(char)));
        *mbn_file_name[0] = 0;
        qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                (unsigned char *)*mbn_file_name);

        QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION qcril_db_query_sw_mbn_file_with_mcc_mnc

===========================================================================*/
/*!
    @brief
    Query mbn file with mcc and mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_query_sw_mbn_file_with_mcc_mnc
(
    char       **mbn_file_name,
    const char *mcc,
    const char *mnc,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        QCRIL_LOG_INFO("mcc %s", mcc? mcc: "null");
        QCRIL_LOG_INFO("mnc %s", mnc? mnc: "null");
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !mcc || !mnc || !volte_info || !mkt_info || !lab_info)
        {
            res = E_FAILURE;
            break;
        }

        *mbn_file_name = static_cast<char *>(malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char)));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on mcc + mnc + volte_info + mkt_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_five_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                         "MCC", mcc, "MNC", mnc, "VOLTE_INFO", volte_info,
                         "MKT_INFO", mkt_info, "LAB_INFO", lab_info);

                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on mcc + mnc only
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                         "MCC", mcc, "MNC", mnc, "MKT_INFO", mkt_info, "LAB_INFO", lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_sw_mbn_int_from_file

===========================================================================*/
/*!
    @brief
    Query integer value from software mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_query_sw_mbn_int_from_file
(
    const char  *mbn_file_name,
    const char  *type,
    uint32_t    *value
)
{
  char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
  IxErrnoType res = E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    if (!mbn_file_name || !type || !value)
    {
        res = E_FAILURE;
        break;
    }

    snprintf(query_stmt, sizeof(query_stmt),
             qcril_db_query_sw_mbn_str_stmt,
             type,
             qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
             "FILE",
             mbn_file_name);

    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_int_callback,
                            (unsigned char *)value);

    QCRIL_LOG_INFO("value %d", *value);
  } while (0);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

int qcril_db_query_sw_mbn_file_from_version_type
(
    uint32_t    family,
    uint32_t    carrier,
    uint32_t    minor,
    char        **mbn_file_name
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!mbn_file_name)
        {
            res = -1;
            break;
        }

        snprintf(query_stmt, QCRIL_DB_MAX_STMT_LEN,
                 qcril_db_query_mbn_file_stmt_three_int_args,
                 "FILE",
                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                 "MCFG_VERSION_FAMILY",
                 family,
                 "MCFG_VERSION_CARRIER",
                 carrier,
                 "MCFG_VERSION_MINOR",
                 minor);
        *mbn_file_name = static_cast<char *>(malloc(QCRIL_MBN_HW_STR_MAX_SIZE*sizeof(char)));
        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                    (unsigned char *)*mbn_file_name);
        }
        else
        {
            res = -1;
            QCRIL_LOG_ERROR("failed to allocate memory for mbn file name");
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_iin

===========================================================================*/
/*!
    @brief
    Query mbn file name from iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_query_mbn_file_name_from_iin
(
    char       **mbn_file_name,
    const char *iin,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    iin_6_digit[IIN_6_LEN] = {0};
    char    iin_7_digit[IIN_7_LEN] = {0};

    IxErrnoType res = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_INFO("iin %s", iin? iin: "null");
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !iin || !volte_info || !mkt_info || !lab_info)
        {
            res = E_FAILURE;
            break;
        }

        *mbn_file_name = static_cast<char *>(malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char)));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            strlcpy(iin_7_digit, iin, sizeof(iin_7_digit));
            strlcpy(iin_6_digit, iin, sizeof(iin_6_digit));

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on 7-digit_iin + volte_info + mkt_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         iin_7_digit,
                         "VOLTE_INFO",
                         volte_info,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on 7-digit_iin only
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_three_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         iin_7_digit,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            if (!strlen(*mbn_file_name))
            {
                if (strlen(volte_info) || strlen(mkt_info))
                {
                // Query base on 6-digit_iin + volte_info + mkt_info
                    snprintf(query_stmt, sizeof(query_stmt),
                             qcril_db_query_mbn_file_stmt_four_args,
                             "FILE",
                             qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                             "MCFG_IIN",
                             iin_6_digit,
                             "VOLTE_INFO",
                             volte_info,
                             "MKT_INFO",
                             mkt_info,
                             "LAB_INFO",
                             lab_info);
                    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                            (unsigned char *)*mbn_file_name);
                }

                // Query base on 6-digit_iin only
                if (!strlen(*mbn_file_name))
                {
                    snprintf(query_stmt, sizeof(query_stmt),
                             qcril_db_query_mbn_file_stmt_three_args,
                             "FILE",
                             qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                             "MCFG_IIN",
                             iin_6_digit,
                             "MKT_INFO",
                             mkt_info,
                             "LAB_INFO",
                             lab_info);
                    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                            (unsigned char *)*mbn_file_name);
                }
            }

            // Query based on wild
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_sw_mbn_str_stmt,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         "wild");
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_long_iin

===========================================================================*/
/*!
    @brief
    Query mbn file name from 11-bit iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_query_mbn_file_name_from_long_iin
(
    char       **mbn_file_name,
    const char *long_iin,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    iin_11_digit[IIN_11_LEN] = {0};

    IxErrnoType res = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_INFO("long_iin %s", long_iin? long_iin: "null");
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !long_iin || !volte_info || !mkt_info || !lab_info)
        {
            res = E_FAILURE;
            break;
        }
        *mbn_file_name = static_cast<char *>(malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char)));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            strlcpy(iin_11_digit, long_iin, sizeof(iin_11_digit));

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on long_iin + volte_info + mkt_info + lab_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_LONG_IIN",
                         iin_11_digit,
                         "VOLTE_INFO",
                         volte_info,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on long_iin only (mkt_info and lab_info will always have value in DB)
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_three_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_LONG_IIN",
                         iin_11_digit,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);

            }
            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}
/*===========================================================================

  FUNCTION  qcril_db_insert_using_stmt

===========================================================================*/
/*!
    @brief
    Insert value into DB using insert statement

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static IxErrnoType qcril_db_insert_using_stmt(const char* insert_stmt)
{
    IxErrnoType res     = E_FAILURE;
    int         ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!insert_stmt)
        {
            QCRIL_LOG_ERROR("Invalid insert_stmt");
            break;
        }

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_iin

===========================================================================*/
/*!
    @brief
    Insert software mbn iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_sw_mbn_iin
(
    const char *file,
    const char *iin,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char        insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    QCRIL_LOG_INFO(" iin: %s", iin? iin: "null");
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!file || !iin || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_iin_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                 file, iin, volte_info, mkt_info, lab_info);


        res = qcril_db_insert_using_stmt(insert_stmt);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_long_iin

===========================================================================*/
/*!
    @brief
    Insert software mbn long iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_sw_mbn_long_iin
(
    const char *file,
    const char *long_iin,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char        insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    QCRIL_LOG_INFO(" long iin: %s", long_iin? long_iin: "null");
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {

        if (!file || !long_iin || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_long_iin_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                 file, long_iin, volte_info, mkt_info, lab_info);

    } while (0);

    res = qcril_db_insert_using_stmt(insert_stmt);
    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_mcc_mnc

===========================================================================*/
/*!
    @brief
    Insert software mbn mcc_mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_db_insert_sw_mbn_mcc_mnc
(
    const char *file,
    const char *mcc,
    const char *mnc,
    const char *volte_info,
    const char *mkt_info,
    const char *lab_info
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    IxErrnoType res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    QCRIL_LOG_INFO(" mcc: %s", mcc? mcc: "null");
    QCRIL_LOG_INFO(" mnc: %s", mnc? mnc: "null");
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {

        if (!file || !mcc || !mnc || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_mcc_mnc_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                 file, mcc, mnc, volte_info, mkt_info, lab_info);

        res = qcril_db_insert_using_stmt(insert_stmt);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_sig_config_callback

===========================================================================*/
/*!
    @brief
    Call back for querying sig config

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_sig_config_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(uint16_t*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %d", *(uint16_t*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_query_sig_config

===========================================================================*/
/*!
    @brief
    Query sig config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_sig_config
(
    const char  *sig_config_type,
    uint16_t    *delta
)
{
    int     res     = 0;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            res = -1;
            break;
        }
        if (!sig_config_type || !delta)
        {
            res = -1;
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_sig_config_stmt,
                 qcril_db_tables[QCRIL_DB_SIG_CONFIG_TYPE].table_name,
                 sig_config_type);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if (SQLITE_OK != (res = qcril_db_sqlite3_exec(
                                             query_stmt,
                                             qcril_db_query_sig_config_callback,
                                             delta)))
        {
            QCRIL_LOG_ERROR("Could not query %d", res);
        }

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/***************** All manual provisioning related functions start *********/

/*===========================================================================

  FUNCTION  qcril_db_insert_new_iccid_into_prov_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_iccid_into_prov_table
(
    char *iccid,
    int  user_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s user_pref - %d", iccid? PII(iccid,"<hidden>"): "null", user_pref);
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_new_iccid_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 iccid,
                 user_pref);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                   insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_user_pref_prov_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_user_pref_prov_table
(
    char *iccid,
    int  user_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s user_pref - %d", iccid? PII(iccid,"<hidden>"): "null", user_pref);
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_user_pref_str_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 user_pref,
                 iccid);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", PII(insert_stmt,"<hidden>"));

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                     insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_table_query_callback_integer

===========================================================================*/
/*!
    @brief
    Call back for querying user preference

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_table_query_callback_integer
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(int*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %d", *(int*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_query_user_pref_from_prov_table

===========================================================================*/
/*!
    @brief
    Query user preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_user_pref_from_prov_table
(
    char        *iccid,
    int         *user_pref
)
{
    int     res     = E_FAILURE;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!iccid || !user_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_user_pref_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 iccid);

        QCRIL_LOG_INFO(" Query: %s", PII(query_stmt,"<hidden>"));

        if(SQLITE_OK != (res = qcril_db_sqlite3_exec(
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             user_pref)))
        {
            QCRIL_LOG_ERROR("Could not insert after restore %d", res);
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_new_mcc_mnc_into_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Insert new mcc,mnc entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_mcc_mnc_into_mbn_imsi_exception_table
(
    char *mcc,
    char *mnc,
    int  carrier_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s mnc: %s carrier_pref - %d",
                            mcc? mcc: "null",
                            mnc? mnc: "null", carrier_pref);
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !mcc || !mnc )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 mcc, mnc, carrier_pref);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }

            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_carrier_pref_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Update carrier pref in existing mcc,mnc entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_carrier_pref_mbn_imsi_exception_table
(
    char *mcc,
    char *mnc,
    int  carrier_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s mnc: %s carrier_pref - %d",
                            mcc? mcc: "null",
                            mnc? mnc: "null", carrier_pref);
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !mcc || !mnc )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 carrier_pref, mcc, mnc);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_carrier_pref_from_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Query carrier preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_carrier_pref_from_mbn_imsi_exception_table
(
    char        *mcc,
    char        *mnc,
    int         *carrier_pref
)
{
    int     res     = E_FAILURE;
    char   *zErrMsg = NULL;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!mcc || !mnc || !carrier_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_carrier_pref_from_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 mcc, mnc);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if (SQLITE_OK != (res = sqlite3_exec(qcril_db_handle,
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             carrier_pref,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", res, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_new_mcc_mnc_into_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_mcc_mnc_into_mbn_iccid_exception_table
(
    char *iccid,
    int  carrier_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s carrier_pref - %d",
                            iccid? PII(iccid,"<hidden>"): "null", carrier_pref);
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 iccid, carrier_pref);

        QCRIL_LOG_INFO(" insert stmt: %s", PII(insert_stmt,"<hidden>"));

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }

            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_update_carrier_pref_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Update carrier pref in existing iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_carrier_pref_mbn_iccid_exception_table
(
    char *iccid,
    int  carrier_pref
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s carrier_pref - %d",
                            iccid? PII(iccid,"<hidden>"): "null", carrier_pref);
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 carrier_pref, iccid);

        QCRIL_LOG_INFO(" insert stmt: %s", PII(insert_stmt,"<hidden>"));

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_carrier_pref_from_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Query carrier preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_carrier_pref_from_mbn_iccid_exception_table
(
    char        *iccid,
    int         *carrier_pref
)
{
    int     res     = E_FAILURE;
    char   *zErrMsg = NULL;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!iccid || !carrier_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_carrier_pref_from_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE].table_name,
                 iccid);

        QCRIL_LOG_INFO(" Query: %s", PII(query_stmt,"<hidden>"));

        if (SQLITE_OK != (res = sqlite3_exec(qcril_db_handle,
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             carrier_pref,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", res, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_wps_call_over_cs_callback

===========================================================================*/
/*!
    @brief
    Callback used to process the results of the query issued in
    qcril_db_query_wps_call_over_cs.

    @return
    0
*/
/*=========================================================================*/
static int qcril_db_query_wps_call_over_cs_callback
(
    void   *wps_call_over_cs,
    int     num_columns,
    char  **column_values,
    char  **column_names
)
{
    QCRIL_NOTUSED(column_names);

    if (wps_call_over_cs && num_columns > 0 && atoi(column_values[0]) > 0)
    {
        *((boolean *) wps_call_over_cs) = true;
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_query_wps_call_over_cs

===========================================================================*/
/*!
    @brief
    Query if a WPS call should be placed over CS for an operator using
    MCC and MNC of the operator.

    @return
    true or false
*/
/*=========================================================================*/
boolean qcril_db_query_wps_call_over_cs
(
    const char *mcc,
    const char *mnc
)
{
    boolean    wps_call_over_cs = false;
    int        sqlite_exec_rc = SQLITE_ERROR;
    char      *zErrMsg = NULL;
    char       query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle || !mcc || !mnc)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_wps_call_over_cs_stmt, mcc, mnc);

        QCRIL_LOG_INFO("Query: %s", query_stmt);

        if (SQLITE_OK != (sqlite_exec_rc = sqlite3_exec(qcril_db_handle,
                                                        query_stmt,
                                                        qcril_db_query_wps_call_over_cs_callback,
                                                        &wps_call_over_cs,
                                                        &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", sqlite_exec_rc, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(wps_call_over_cs);
    return wps_call_over_cs;
}

/*===========================================================================

  FUNCTION  qcril_db_query_force_on_dc_callback

===========================================================================*/
/*!
    @brief
    Callback used to process the results of the query issued in
    qcril_db_query_force_on_dc.

    @return
    0
*/
/*=========================================================================*/
static int qcril_db_query_force_on_dc_callback
(
    void   *force_on_dc,
    int     num_columns,
    char  **column_values,
    char  **column_names
)
{
    QCRIL_NOTUSED(column_names);

    if (force_on_dc && num_columns > 0 && atoi(column_values[0]) > 0)
    {
        *((boolean *) force_on_dc) = true;
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_query_force_on_dc

===========================================================================*/
/*!
    @brief
    Query if the force on dc of sms should be enabled for an operator using
    MCC and MNC of the operator.

    @return
    true or false
*/
/*=========================================================================*/
boolean qcril_db_query_force_on_dc
(
    const char *mcc,
    const char *mnc
)
{
    boolean    force_on_dc = false;
    int        sqlite_exec_rc = SQLITE_ERROR;
    char      *zErrMsg = NULL;
    char       query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    if ((qcril_db_handle != NULL) && (mcc != NULL) && (mnc != NULL))
    {
        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_force_on_dc_stmt, mcc, mnc);

        QCRIL_LOG_INFO("Query: %s", query_stmt);

        if (SQLITE_OK != (sqlite_exec_rc = sqlite3_exec(qcril_db_handle,
                                                        query_stmt,
                                                        qcril_db_query_force_on_dc_callback,
                                                        &force_on_dc,
                                                        &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", sqlite_exec_rc, zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(force_on_dc);
    return force_on_dc;
}

/*===========================================================================

  FUNCTION  qcril_db_table_query_callback_iccid_clir_pref

===========================================================================*/
/*!
    @brief
    Callback used to process the results of the query issued in
    qcril_db_query_iccid_clir_pref.

    @return
    0
*/
/*=========================================================================*/
int qcril_db_query_iccid_clir_pref_callback(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;
    uint32_t clir_pref;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        if (argc == 1 && argv[0] && strnlen(argv[0], 1) > 0)
        {
            /* Check that the database doesn't return some weird value,
             * as CLIR settings can only be 0, 1, or 2.
             * It can't be negative because stroul returns an unsigned value,
             * so we just need to check that it doesn't exceed 2.
            */
            clir_pref = (uint32_t)strtoul(argv[0], nullptr, 0);
            if (clir_pref > 2) {
                QCRIL_LOG_DEBUG("Invalid CLIR pref in database: %u. Using 0 (default) instead.",
                                clir_pref);
                clir_pref = 0;
            }
            ((clir_pref_query_result_t *)(data))->clir_pref = clir_pref;
            ((clir_pref_query_result_t *)(data))->found = true;
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
            ((clir_pref_query_result_t *)(data))->clir_pref = 0;
            ((clir_pref_query_result_t *)(data))->found = false;
        }
    }

    return ret;

}

/*===========================================================================
    FUNCTION qcril_db_query_iccid_clir_pref
===========================================================================*/
/*!
    @brief
    Queries for a given ICCID's CLIR preference in DB.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_iccid_clir_pref(const char *iccid, uint32_t *clir_pref, bool *found) {

    char query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;
    clir_pref_query_result_t query_result = {
        0,      // clir_pref
        false, //found
    };


    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle || !iccid || !clir_pref || !found) {
            break;
        }
        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_iccid_clir_pref_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name,
                 iccid);

        QCRIL_LOG_DEBUG("Query: %s", PII(query_stmt,"<hidden>"));
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                                query_stmt,
                                                qcril_db_query_iccid_clir_pref_callback,
                                                &query_result)))
        {
            QCRIL_LOG_DEBUG("Could not query ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
            *clir_pref = query_result.clir_pref;
            *found = query_result.found;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================
    FUNCTION qcril_db_query_iccid_clir_pref_entry_count
===========================================================================*/
/*!
    @brief
    Queries for the number of entries in the ICCID CLIR Preference table.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_iccid_clir_pref_entry_count(int32_t *count) {

    char query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;


    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle) {
            break;
        }
        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_get_entry_count,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name);

        QCRIL_LOG_DEBUG("Query: %s", query_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                                query_stmt,
                                                qcril_db_table_query_callback_integer,
                                                count)))
        {
            QCRIL_LOG_DEBUG("Could not query ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================

  FUNCTION  qcril_db_query_iccid_clir_pref_oldest_entry_callback

===========================================================================*/
/*!
    @brief
    Call back for querying the iccid of the oldest entry.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_iccid_clir_pref_oldest_entry_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;
    int len = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            len = strlen(argv[0]);
            if (len >= QCRIL_ICCID_STR_MAX_SIZE)
            {
                QCRIL_LOG_ERROR("Unexpected length");
            }
            else
            {
                strlcpy(static_cast<char *>(data), argv[0], QCRIL_ICCID_STR_MAX_SIZE);
            }
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}



/*===========================================================================
    FUNCTION qcril_db_query_iccid_clir_pref_oldest_entry
===========================================================================*/
/*!
    @brief
    Queries for the ICCID of oldest ICCID CLIR preference entry in DB.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_iccid_clir_pref_oldest_entry(char *oldest) {

    char query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle) {
            break;
        }
        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_iccid_clir_pref_oldest_entry_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name);

        QCRIL_LOG_DEBUG("Query: %s", query_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                                query_stmt,
                                                qcril_db_query_iccid_clir_pref_oldest_entry_callback,
                                                oldest)))
        {
            QCRIL_LOG_DEBUG("Could not query ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}


/*===========================================================================
    FUNCTION qcril_db_insert_iccid_clir_pref
===========================================================================*/
/*!
    @brief
    Inserts a new CLIR preference entry for an ICCID.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_iccid_clir_pref(const char *iccid, uint32_t clir_pref, int32_t entry_age) {

    char insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    do {
        if (!qcril_db_handle || !iccid) {
            break;
        }
        size_t iccid_len = strnlen(iccid, QCRIL_ICCID_STR_MAX_SIZE + 1);
        // Make sure iccid has a valid length.
        if (iccid_len > QCRIL_ICCID_STR_MAX_SIZE) {
            QCRIL_LOG_DEBUG("iccid %s has an invalid length of %d.", PII(iccid,"<hidden>"), iccid_len);
            break;
        }
        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_iccid_clir_pref_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name,
                 iccid, clir_pref, entry_age);

        //QCRIL_LOG_DEBUG("Insert statement: %s", insert_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_DEBUG("Could not insert entry into ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================
    FUNCTION qcril_db_update_iccid_clir_pref
===========================================================================*/
/*!
    @brief
    Updates an existing CLIR preference entry for an ICCID.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_iccid_clir_pref(const char *iccid, uint32_t clir_pref) {

    char update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    do {
        if (!qcril_db_handle || !iccid) {
            break;
        }
        snprintf(update_stmt, sizeof(update_stmt),
                 qcril_db_update_iccid_clir_pref_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name,
                 clir_pref, iccid);

        //QCRIL_LOG_DEBUG("Update statement: %s", update_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_DEBUG("Could not update entry in ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================
    FUNCTION qcril_db_update_iccid_clir_pref_ages
===========================================================================*/
/*!
    @brief
    Increments the age of all entries expect the one
    specified by the iccid argument.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_iccid_clir_pref_ages(const char *iccid) {

    char update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    do {
        if (!qcril_db_handle || !iccid) {
            break;
        }
        snprintf(update_stmt, sizeof(update_stmt),
                 qcril_db_update_iccid_clir_pref_age_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name,
                 iccid);

        //QCRIL_LOG_DEBUG("Update statement: %s", update_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_DEBUG("Could not update the age for entries in ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================
    FUNCTION qcril_db_delete_iccid_clir_pref
===========================================================================*/
/*!
    @brief
    Delete an CLIR preference entry for an ICCID.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_delete_iccid_clir_pref(const char *iccid) {

    char delete_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    do {
        if (!qcril_db_handle || !iccid) {
            break;
        }
        snprintf(delete_stmt, sizeof(delete_stmt),
                 qcril_db_delete_iccid_clir_pref_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name,
                 iccid);

        //QCRIL_LOG_DEBUG("Delete statement: %s", delete_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(delete_stmt, NULL, NULL)))
        {
            QCRIL_LOG_DEBUG("Could not delete entry in ICCID CLIR Table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}

/*===========================================================================
  FUNCTION qcrild_db_clear_iccid_clir_pref_table
===========================================================================*/
/*!
    @brief
    Delete all CLIR preference entries in the table.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
#ifdef QMI_RIL_UTF
int qcril_db_clear_iccid_clir_pref_table() {

    char delete_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int ret = SQLITE_OK;
    int res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    do {
        if (!qcril_db_handle) {
            break;
        }
        snprintf(delete_stmt, sizeof(delete_stmt),
                 qcril_db_clear_iccid_clir_pref_table_stmt,
                 qcril_db_tables[QCRIL_DB_ICCID_CLIR_PREF_TYPE].table_name
                 );

        QCRIL_LOG_DEBUG("Delete statement: %s", delete_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(delete_stmt, NULL, NULL)))
        {
            QCRIL_LOG_DEBUG("Could not delete entries from the ICCID CLIR pref table: %d", ret);
        }
        else {
            res = E_SUCCESS;
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;

}
#endif // QMI_RIL_UTF

/*===========================================================================

  FUNCTION  qcril_db_reset_cleanup

===========================================================================*/
/*!
    @brief
    Reset all global vars and release database

    @return
    0 on success
*/
/*=========================================================================*/

int qcril_db_reset_cleanup()
{
  int ret = SQLITE_ERROR;

  if (qcril_db_handle)
    ret = sqlite3_close(qcril_db_handle);
  sqlite3_shutdown();
  qcril_db_emergency_numbers_escv_type = 0;
  qcril_db_query_result = 0;


  if (ret != SQLITE_OK)
  {
    return -1;
  }

  return 0;

}

/*===========================================================================

  FUNCTION  qcril_db_sqlite3_exec

===========================================================================*/
/*!
    @brief
    Execute SQL command and retry DB operation after restoring
    the DB from backup, if operation failed.

    @return
    SQLITE_OK on success
*/
/*=========================================================================*/

int qcril_db_sqlite3_exec
(
    const char *sql_stmt,                             /* SQL to be evaluated */
    int        (*callback)(void*,int,char**,char**),  /* Callback function */
    void       *arg                                   /* 1st argument to callback */
)
{
  char *zErrMsg = NULL;
  int rc = SQLITE_OK;

  QCRIL_LOG_FUNC_ENTRY();

  if (SQLITE_OK != (rc = sqlite3_exec(qcril_db_handle,
                                       sql_stmt, callback,
                                       arg, &zErrMsg)))
  {
    if (zErrMsg)
    {
      QCRIL_LOG_ERROR("sql_stmt %s", sql_stmt);
      QCRIL_LOG_ERROR("Operation failed %d %s", rc, zErrMsg);
      sqlite3_free(zErrMsg);
    }

    if(rc == SQLITE_CORRUPT)
    {
      qcril_db_recovery(QCRIL_DB_RECOVERY_RESTORE);
      if (SQLITE_OK != (rc = sqlite3_exec(qcril_db_handle,
                                           sql_stmt, callback,
                                           arg, &zErrMsg)))
      {
        if (zErrMsg)
        {
          QCRIL_LOG_ERROR("sql_stmt %s", sql_stmt);
          QCRIL_LOG_ERROR("Operation failed after restore %d %s", rc, zErrMsg);
          sqlite3_free(zErrMsg);
        }
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
  return rc;
}

/*===========================================================================

  FUNCTION  qcril_db_check_integrity_callback

===========================================================================*/
/*!
    @brief
    checks integrity of db callback.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_integrity_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(azColName);

    QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
    if ((argc > 0) && argv[0] && strlen(argv[0]) > 0 && data)
    {
        if((argc == 1) && (strncmp(argv[0], "ok", strlen(argv[0])) == 0))
        {
            /* DB passed integrity check */
            *(int*)data = TRUE;
        }
        else
        {
            *(int*)data = FALSE;
        }
    }
    QCRIL_LOG_FUNC_RETURN();

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_integrity

===========================================================================*/
/*!
    @brief
    Check and return if DB passed Inegrity check or not.

    @return
    TRUE(1) if Inegrity check is pass
    FALSE(0) else
*/
/*=========================================================================*/

int qcril_db_check_integrity(sqlite3* db_handle)
{
    char *zErrMsg = NULL;
    int rc = SQLITE_OK;
    int integrity_check = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK != (rc = sqlite3_exec(db_handle,
                                        qcril_db_check_integrity_stmt,
                                        qcril_db_check_integrity_callback,
                                        &integrity_check, &zErrMsg)))
    {
      if (zErrMsg)
      {
        QCRIL_LOG_ERROR("sql_stmt %s", qcril_db_check_integrity_stmt);
        QCRIL_LOG_ERROR("Operation failed %d %s", rc, zErrMsg);
        sqlite3_free(zErrMsg);
      }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(integrity_check);
    return integrity_check;

}

/*===========================================================================

  FUNCTION  qcril_db_recovery

===========================================================================*/
/*!
    @brief
    Either a backup or restore will be done based on input param

    @return
    SQLITE_OK on success
*/
/*=========================================================================*/

int qcril_db_recovery(qcril_db_recovery_type isRecovery)
{
  int rc = SQLITE_OK;
  int valid_integrity = TRUE;
  sqlite3_backup *qcril_backup_handle = NULL;
  sqlite3 *qcril_src_handle = NULL;
  sqlite3 *qcril_dst_handle = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    switch (isRecovery)
    {
      case QCRIL_DB_RECOVERY_BACKUP:
        QCRIL_LOG_DEBUG("Backup QCRIL database");
        /* First check if the current database is not corrupt */
        valid_integrity = qcril_db_check_integrity(qcril_db_handle);
        QCRIL_LOG_DEBUG("Integrity of current DB: %d", valid_integrity);
        if(TRUE != valid_integrity)
        {
            /* Do not perform backup */
            rc = SQLITE_CORRUPT;
            QCRIL_LOG_DEBUG("Skip backing up of corrupt DB");
            break;
        }
        rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                             &qcril_src_handle,
                             SQLITE_OPEN_READONLY, NULL);
        if (rc == SQLITE_OK)
            rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                                 &qcril_dst_handle,
                                 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        break;

      case QCRIL_DB_RECOVERY_RESTORE:
        QCRIL_LOG_DEBUG("Restore QCRIL database");
        rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                             &qcril_dst_handle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (rc == SQLITE_OK)
        {
            rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                                 &qcril_src_handle,
                                 SQLITE_OPEN_READONLY, NULL);
            // in case that BACKUP database is also corrupted, which should not happen (most cases
            // it may be caused by memory corruption like double free in ril code), PREBUILT_DB which
            // is RO in file system will be used
            if (rc != SQLITE_OK || qcril_db_check_integrity(qcril_src_handle) != TRUE)
            {
                QCRIL_LOG_DEBUG("issues in backup database...restore from prebuilt one");
                if (qcril_src_handle) (void)sqlite3_close(qcril_src_handle);
                rc = sqlite3_open_v2(QCRIL_PREBUILT_DB_NAME,
                                     &qcril_src_handle,
                                     SQLITE_OPEN_READONLY, NULL);
            }
        }
        break;

      case QCRIL_DB_RECOVERY_CHECK_AND_RESTORE:
        QCRIL_LOG_DEBUG("Check Integrity and Restore QCRIL database");
        valid_integrity = qcril_db_check_integrity(qcril_db_handle);
        QCRIL_LOG_DEBUG("Integrity of current DB: %d", valid_integrity);
        if(TRUE != valid_integrity)
        {
          /* Perform restore */
          rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                               &qcril_dst_handle,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
          if(rc != SQLITE_OK)
          {
            QCRIL_LOG_DEBUG("Failed to open qcril.db: %d", rc);
          }
          else
          {
            rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                                 &qcril_src_handle,
                                 SQLITE_OPEN_READONLY, NULL);
            if (rc != SQLITE_OK || qcril_db_check_integrity(qcril_src_handle) != TRUE)
            {
                QCRIL_LOG_DEBUG("issues in backup database...restore from prebuilt one");
                if (qcril_src_handle) (void)sqlite3_close(qcril_src_handle);
                rc = sqlite3_open_v2(QCRIL_PREBUILT_DB_NAME,
                                     &qcril_src_handle,
                                     SQLITE_OPEN_READONLY, NULL);
            }
          }
        }
        break;

      default:
        QCRIL_LOG_DEBUG("Invalid otion for recovery: %d", isRecovery);
        rc = SQLITE_ERROR;
        break;
    }

    if(rc != SQLITE_OK || !qcril_src_handle || !qcril_dst_handle) break;

    qcril_backup_handle = sqlite3_backup_init(qcril_dst_handle, "main",
                                              qcril_src_handle, "main");
    if( qcril_backup_handle )
    {
      QCRIL_LOG_DEBUG("database copying...");
      (void)sqlite3_backup_step(qcril_backup_handle, -1);
      (void)sqlite3_backup_finish(qcril_backup_handle);
    }
    rc = sqlite3_errcode(qcril_dst_handle);
  } while(FALSE);

  if(TRUE != valid_integrity && isRecovery == QCRIL_DB_RECOVERY_BACKUP)
  {
    /* Backup was requested but current db is corrupt. So instead RECOVER the DB */
    QCRIL_LOG_DEBUG("Trigger Restore instead of Backup");
    qcril_db_recovery(QCRIL_DB_RECOVERY_RESTORE);
  }

  /* Close QCRIL database */
  if(qcril_src_handle)
  {
    (void)sqlite3_close(qcril_src_handle);
  }
  if(qcril_dst_handle)
  {
    (void)sqlite3_close(qcril_dst_handle);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
  return rc;
}
