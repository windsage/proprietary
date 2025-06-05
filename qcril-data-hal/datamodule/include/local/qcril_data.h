/******************************************************************************
  ---------------------------------------------------------------------------
  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_DATA_H
#define QCRIL_DATA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "assert.h"
#include <telephony/ril.h>
#include <framework/legacy.h>
#include <framework/Log.h>
#include "DataCommon.h"
#include "data_system_determination_v01.h"
#include "wireless_data_service_v01.h"
#include "modules/android/version.h"
#include "dsi_netctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern wds_bind_subscription_enum_v01 global_subs_id;
extern int global_instance_id;
extern boolean global_qcril_clat_supported;
extern unsigned int dataMtu;
extern int dsi_set_ril_instance(int instance);

typedef struct qcril_data_ce_map_s
{
  int ril_data_ce_code;
  int qmi_data_ce_code;
  dsi_ce_reason_type_t reason_category;
} qcril_data_ce_map;

#define PDP_FAIL_ACTIVATION_NOT_ALLOWED       (7)
#define QCRIL_TRUE  "true"
#define QCRIL_FALSE "false"
#define LOCK_MUTEX(mutex)                                 \
        if( pthread_mutex_lock( &mutex ) < 0) {      \
          Log::getInstance().d("LOCK MUTEX FAILED!!!"); \
          return;                                    \
        }

#define UNLOCK_MUTEX(mutex)                                  \
        if( pthread_mutex_unlock( &mutex ) < 0) {       \
          Log::getInstance().d("UNLOCK MUTEX FAILED!!!");  \
          return;                                       \
        }

#define QCRIL_DATA_PROPERTY_TIMEOUT_DEFAULT  (155)

#define MAX_CONCURRENT_UMTS_DATA_CALLS (DSI_MAX_DATA_CALLS) /* bounded as [0, 255] */

#define DS_RIL_REQ_INVALID             (0)

#if defined(RIL_FOR_MDM_LE) || defined(QCRIL_DATA_OFFTARGET) || defined(QMI_RIL_UTF)
  #define SA_FAMILY(addr)         (addr).sa_family
  #define SA_DATA(addr)           (addr).sa_data
  #define SASTORAGE_FAMILY(addr)  (addr).ss_family
  #define SASTORAGE_DATA(addr)    (addr).__ss_padding
  #define PTHREAD_MUTEX_RECURSIVE  PTHREAD_MUTEX_RECURSIVE_NP
#else
  #define SA_FAMILY(addr)         (addr).sa_family
  #define SA_DATA(addr)           (addr).sa_data
  #define SASTORAGE_FAMILY(addr)  (addr).ss_family
  #define SASTORAGE_DATA(addr)    (addr).__data
#endif

#define QCRIL_DATA_IP_FAMILY_V4   "IP"
#define QCRIL_DATA_IP_FAMILY_V6   "IPV6"
#define QCRIL_DATA_IP_FAMILY_V4V6 "IPV4V6"
#define QCRIL_DATA_IPV4           (4)
#define QCRIL_DATA_IPV6           (6)
#define QCRIL_DATA_EMBMS_TMGI_LEN (6)

/* RIL API call states */
#define CALL_INACTIVE                (0)
#define CALL_ACTIVE_PHYSLINK_DOWN    (1)
#define CALL_ACTIVE_PHYSLINK_UP      (2)


#define DS_CALL_INFO_APN_MAX_LEN  (QMI_WDS_APN_NAME_MAX_V01)
#define DS_CALL_INFO_ADDR_BUF_SIZE     (16)
/* Format: xxx.xxx.xxx.xxx/yy */
#define DS_CALL_INFO_ADDR_IPV4_MAX_LEN (18)
/* Format: xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx/yyy */
#define DS_CALL_INFO_ADDR_IPV6_MAX_LEN (43)

#define DS_CALL_INFO_ADDR_IPV4V6_MAX_LEN (DS_CALL_INFO_ADDR_IPV4_MAX_LEN +  \
                                          DS_CALL_INFO_ADDR_IPV6_MAX_LEN + 1)
                                          /* IPv6 & IPv6 with space delimiter */
#define DS_CALL_INFO_IP_ADDR_MAX_LEN   (DS_CALL_INFO_ADDR_IPV6_MAX_LEN)

#define DS_CALL_INFO_ADDR_PCSCF_MAX_LEN (200)

#define QCRIL_DATA_TECHNOLOGY_MAX_LEN  4
#define QCRIL_DATA_AUTH_PREF_MAX_LEN   2
#define QCRIL_PROFILE_ID_STR_MAX       4
#define QCRIL_IP_FAMILY_STR_MAX        sizeof(QCRIL_DATA_IP_FAMILY_V4V6)
#define QCRIL_APN_TYPES_STR_MAX        16
#define QCRIL_ROAMING_ALLOWED_STR_MAX  4
#define QCRIL_ACCESS_NETWORK_STR_MAX   2
#define QCRIL_REASON_STR_MAX           2

#define QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_CDMA_1X_EVDO(t) ((t == DSI_DATA_BEARER_TECH_CDMA_1X)   || \
                                                            (t == DSI_DATA_BEARER_TECH_EVDO_REV0) || \
                                                            (t == DSI_DATA_BEARER_TECH_EVDO_REVA) || \
                                                            (t == DSI_DATA_BEARER_TECH_EVDO_REVB) || \
                                                            (t == DSI_DATA_BEARER_TECH_HRPD))

#define QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP(t)  ((t == DSI_DATA_BEARER_TECH_WCDMA)         || \
                                                     (t == DSI_DATA_BEARER_TECH_GPRS)          || \
                                                     (t == DSI_DATA_BEARER_TECH_HSDPA)         || \
                                                     (t == DSI_DATA_BEARER_TECH_HSUPA)         || \
                                                     (t == DSI_DATA_BEARER_TECH_EDGE)          || \
                                                     (t == DSI_DATA_BEARER_TECH_LTE)           || \
                                                     (t == DSI_DATA_BEARER_TECH_HSDPA_PLUS)    || \
                                                     (t == DSI_DATA_BEARER_TECH_DC_HSDPA_PLUS) || \
                                                     (t == DSI_DATA_BEARER_TECH_HSPA)          || \
                                                     (t == DSI_DATA_BEARER_TECH_64_QAM)        || \
                                                     (t == DSI_DATA_BEARER_TECH_TDSCDMA)       || \
                                                     (t == DSI_DATA_BEARER_TECH_GSM)           || \
                                                     (t == DSI_DATA_BEARER_TECH_3GPP_WLAN))

#define QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP2(t)  (QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_CDMA_1X_EVDO(t) || \
                                                      (t == DSI_DATA_BEARER_TECH_EHRPD)                  || \
                                                      (t == DSI_DATA_BEARER_TECH_3GPP2_WLAN))

#define QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP_EHRPD(t)  (QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP(t) || \
                                                          (t == DSI_DATA_BEARER_TECH_EHRPD))

/*Function Definitions */

void qcril_data_set_pref_data_tech
(
  int instance_id,
  qcril_data_pref_data_tech_e_type pref_data_tech
);
void qcril_data_set_dsd_sys_status
(
  dsd_system_status_ind_msg_v01 *dsd_system_status
);
void qcril_data_set_data_sys_status
(
  qcril_instance_id_e_type          instance_id,
  wds_data_system_status_type_v01 * data_sys_status
);
void qcril_data_new_dds_info
(
  DDSSubIdInfo sub_info
);
void qcril_data_dds_status_followup
(
  DDSStatusInfo * status
);
void qcril_data_bearer_type_info_ind_update
(
  int32_t cid,
  wds_data_bearer_type_info_v01 *bearerInfo
);
int qcril_data_get_ril_ce_code
(
  dsi_ce_reason_t * dsi_ce_reason,
  int * ril_ce_code
);
int qcril_data_get_ifindex
(
  const char * dev,
  int * ifindex
);
#ifndef RIL_FOR_LOW_RAM
void qcril_data_bearer_type_info_response_update
(
  int32_t cid,
  char * apn,
  char * ifaceName,
  wds_get_data_bearer_type_resp_msg_v01 *resp
);
#endif

#ifdef __cplusplus
}
#endif
#endif
