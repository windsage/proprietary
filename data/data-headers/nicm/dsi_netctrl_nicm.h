/*===========================================================================
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "ds_util.h"
#include "Nicm.h"
#include "dsi_nicm_internal.h"
#include <string.h>

#define DSI_NICM_DIAG_LOG_MSG_SIZE 512

#define DSI_NICM_LOG_ERROR(...)                                 \
  do {                                                             \
    char buf[DSI_NICM_DIAG_LOG_MSG_SIZE];                       \
    snprintf(buf, DSI_NICM_DIAG_LOG_MSG_SIZE, __VA_ARGS__);     \
    MSG_SPRINTF_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,           \
                   "%s[%d] %s",l2s_proc_name,l2s_pid,buf);         \
  } while(0);                                                      \

#define DSI_NICM_LOG_MED(...)                                   \
  do {                                                             \
    char buf[DSI_NICM_DIAG_LOG_MSG_SIZE];                       \
    snprintf(buf, DSI_NICM_DIAG_LOG_MSG_SIZE, __VA_ARGS__);     \
    MSG_SPRINTF_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED,             \
                   "%s[%d] %s",l2s_proc_name,l2s_pid,buf);         \
  } while(0);                                                      \

#define DSI_NICM_LOG_HIGH(...)                                  \
  do {                                                             \
    char buf[DSI_NICM_DIAG_LOG_MSG_SIZE];                       \
    snprintf(buf, DSI_NICM_DIAG_LOG_MSG_SIZE, __VA_ARGS__);     \
    MSG_SPRINTF_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH,            \
                   "%s[%d] %s",l2s_proc_name,l2s_pid,buf);         \
  } while(0);                                                      \

#define DSI_INIT_WAIT_TIME_BEFORE_RETRY 100000 /* usec time interval between each NETLINK req event sent from dsi_netctrl */
#define DSI_INIT_MAX_RETRY_COUNT  600 /* max number of retrying ping msgto nicm for its readiness */

#define DSI_NICM_SUCCESS 0 /**< Indicates that the operation was successful. */
#define DSI_NICM_ERROR -1  /**< Indicates that the operation was not successful. */

/*---------------------------------------------------------------------------
   Type representing NetLink event indication payload
---------------------------------------------------------------------------*/
#define DSI_NICM_EVT_PARAM_NONE         (0x0000)
#define DSI_NICM_EVT_PARAM_LINK         (0x0001)
#define DSI_NICM_EVT_PARAM_FLOWINFO     (0x0002)
#define DSI_NICM_EVT_PARAM_ADDRINFO     (0x0004)
#define DSI_NICM_EVT_PARAM_GTWYINFO     (0x0008)
#define DSI_NICM_EVT_PARAM_DNSPADDR     (0x0010)
#define DSI_NICM_EVT_PARAM_DNSSADDR     (0x0020)
#define DSI_NICM_EVT_PARAM_DEVNAME      (0x0040)
#define DSI_NICM_EVT_PARAM_USER_CMD     (0x0080)
#define DSI_NICM_EVT_PARAM_CMD_DATA     (0x0100)
#define DSI_NICM_EVT_PARAM_MTU          (0x0200)
#define DSI_NICM_EVT_PARAM_PDU_ID       (0x0400)
#define DSI_NICM_EVT_PARAM_ADDR_TYPE    (0x0800)
#define DSI_NICM_EVT_PARAM_MASK         ( DSI_NICM_EVT_PARAM_LINK      | \
                                        DSI_NICM_EVT_PARAM_FLOW      | \
                                        DSI_NICM_EVT_PARAM_ADDRINFO  | \
                                        DSI_NICM_EVT_PARAM_GTWYINFO  | \
                                        DSI_NICM_EVT_PARAM_DNSPINFO  | \
                                        DSI_NICM_EVT_PARAM_DNSSINFO  | \
                                        DSI_NICM_EVT_PARAM_DEVNAME   | \
                                        DSI_NICM_EVT_PARAM_USR_CMD   | \
                                        DSI_NICM_EVT_PARAM_MTU       | \
                                        DSI_NICM_EVT_PARAM_PDU_ID    | \
                                        DSI_NICM_EVT_PARAM_ADDR_TYPE )

/* WARNING: These constants contain multiple bits. The ONLY proper
 * way to check them is as follows:
 * if ((bits & PARAM_MASK) == PARAM_MASK)
 * otherwise you'll get false positives. You have been warned...
 */
#define DSI_NICM_EVT_PARAM_IPADDR       (0x1000 | DSI_NICM_EVT_PARAM_ADDRINFO)
#define DSI_NICM_EVT_PARAM_CACHE        (0x2000 | DSI_NICM_EVT_PARAM_ADDRINFO)
#define DSI_NICM_EVT_PARAM_OP_STATUS    (0x4000)

#define DSI_NICM_QMI_WDS_APN_NAME_MAX_V01 150

/*---------------------------------------------------------------------------
   Dsi Nicm user commands
---------------------------------------------------------------------------*/
#define DSI_NICM_CMD_SCREEN_OFF 0
#define DSI_NICM_CMD_SCREEN_ON  1
#define DSI_NICM_CMD_ENABLE_PORT_FORWARDING  2
#define DSI_NICM_CMD_DISABLE_PORT_FORWARDING 3
#define DSI_NICM_CMD_QUERY_PORT_FORWARDING   4
#define DSI_NICM_CMD_QUERY_ROUTE_LOOK_UP     5
#define DSI_NICM_CMD_LINK_RELEASE            6
#define DSI_NICM_CMD_QUERY_NETWORK_HANDLE    7
#define DSI_NICM_CMD_QUERY_LINK_MTU          8
#define DSI_NICM_CMD_QUERY_LINK_IFACE_MTU_ONLY 9
#define DSI_NICM_CMD_QUERY_OEMPROXY_DATAPATH_IFACE 10
#define DSI_NICM_CMD_QUERY_MATCHING_CALL_IFACE 11
#define DSI_NICM_CMD_QUERY_LINK_MTU_PDU_ID   12

/*---------------------------------------------------------------------------
   Type representing enumeration of NetLink event indication messages
---------------------------------------------------------------------------*/
typedef enum {
  DSI_NICM_INVALID_EV,         /* Internal value                       */
  DSI_NICM_UP_EV,              /* Network interface entered UP state   */
  DSI_NICM_DOWN_EV,            /* Network interface entered DOWN state */
  DSI_NICM_RECONFIGURED_EV,    /* Network interface reconfigured       */
  DSI_NICM_FLOW_ACTIVATED_EV,  /* QoS flow entered ACTIVATED state     */
  DSI_NICM_FLOW_DELETED_EV,    /* QoS flow entered DELETED state       */
  DSI_NICM_FLOW_MODIFIED_EV,   /* QoS flow entered MODIFIED state      */
  DSI_NICM_FLOW_SUSPENDED_EV,  /* QoS flow entered SUSPENDED state     */
  DSI_NICM_FLOW_ENABLED_EV,    /* QoS flow datapath enabled            */
  DSI_NICM_FLOW_DISABLED_EV,   /* QoS flow datapath disabled           */
  DSI_NICM_RESET_EV,           /* Process satte reset occurred         */
  DSI_NICM_NEWADDR_EV,         /* Network interface address updated    */
  DSI_NICM_DELADDR_EV,         /* Network interface address removed    */
  DSI_NICM_READY_REQ,                /* DSI_NICM readiness query               */
  DSI_NICM_READY_RESP,               /* DSI_NICM readiness response            */
  DSI_NICM_USER_CMD,                 /* DSI_NICM user generated command        */
  DSI_NICM_MTU_UPDATE_EV,      /* Network interface MTU updated        */
  DSI_NICM_OEMPRXY_IFACE_UP,   /* OEMPrxy interface entered UP state   */
  DSI_NICM_OEMPRXY_IFACE_DOWN, /* OEMPrxy interface entered DOWN state */
  DSI_NICM_PDU_AVAILABLE_EV,
  DSI_NICM_PDU_PROMOTION_EV,
  DSI_NICM_PDU_RELEASE_EV,
  DSI_NICM_PDU_RECONFIG_EV,
  DSI_NICM_OP_FAILED_EV,       /* Indicate DSI_NICM failures             */
  DSI_NICM_MAX_EV              /* Internal value                       */
} dsiNicmNLEvent;

/*---------------------------------------------------------------------------
   Type representing enumeration of network link/interface identifiers
---------------------------------------------------------------------------*/
typedef enum {
  /* Forward Rmnet ports */
  NICM_LINK_RMNET_0,
  NICM_LINK_RMNET_1,
  NICM_LINK_RMNET_2,
  NICM_LINK_RMNET_3,
  NICM_LINK_RMNET_4,
  NICM_LINK_RMNET_5,
  NICM_LINK_RMNET_6,
  NICM_LINK_RMNET_7,
  NICM_LINK_RMNET_8,
  NICM_LINK_RMNET_9,
  NICM_LINK_RMNET_10,
  NICM_LINK_RMNET_11,
  NICM_LINK_RMNET_12,
  NICM_LINK_RMNET_13,
  NICM_LINK_RMNET_14,
  NICM_LINK_RMNET_15,
  NICM_LINK_RMNET_16,

  /* Forward Rmnet MDM ports */
  NICM_LINK_RMNET_17,
  NICM_LINK_RMNET_18,
  NICM_LINK_RMNET_19,
  NICM_LINK_RMNET_20,
  NICM_LINK_RMNET_21,
  NICM_LINK_RMNET_22,
  NICM_LINK_RMNET_23,
  NICM_LINK_RMNET_24,
  NICM_LINK_RMNET_25,
  NICM_LINK_RMNET_26,
  NICM_LINK_RMNET_27,
  NICM_LINK_RMNET_28,
  NICM_LINK_RMNET_29,
  NICM_LINK_RMNET_30,
  NICM_LINK_RMNET_31,
  NICM_LINK_RMNET_32,
  NICM_LINK_RMNET_33,

#ifdef FEATURE_DATA_IWLAN
  /* Reverse Rmnet MSM ports */
  NICM_LINK_REV_RMNET_0,
  NICM_LINK_REV_RMNET_1,
  NICM_LINK_REV_RMNET_2,
  NICM_LINK_REV_RMNET_3,
  NICM_LINK_REV_RMNET_4,
  NICM_LINK_REV_RMNET_5,
  NICM_LINK_REV_RMNET_6,
  NICM_LINK_REV_RMNET_7,
  NICM_LINK_REV_RMNET_8,
  NICM_LINK_REV_RMNET_9,
  NICM_LINK_REV_RMNET_10,
  NICM_LINK_REV_RMNET_11,
  NICM_LINK_REV_RMNET_12,
  NICM_LINK_REV_RMNET_13,
  NICM_LINK_REV_RMNET_14,
  NICM_LINK_REV_RMNET_15,

  /* Reverse Rmnet MDM ports */
  NICM_LINK_REV_RMNET_16,
  NICM_LINK_REV_RMNET_17,
  NICM_LINK_REV_RMNET_18,
  NICM_LINK_REV_RMNET_19,
  NICM_LINK_REV_RMNET_20,
  NICM_LINK_REV_RMNET_21,
  NICM_LINK_REV_RMNET_22,
  NICM_LINK_REV_RMNET_23,
  NICM_LINK_REV_RMNET_24,
  NICM_LINK_REV_RMNET_25,
  NICM_LINK_REV_RMNET_26,
  NICM_LINK_REV_RMNET_27,
  NICM_LINK_REV_RMNET_28,
  NICM_LINK_REV_RMNET_29,
  NICM_LINK_REV_RMNET_30,
  NICM_LINK_REV_RMNET_31,
#endif /* FEATURE_DATA_IWLAN */

  NICM_LINK_MAX,
  NICM_LINK_NONE,                     /* Used for link-independent event */
  NICM_LINK_FORCE_32_BIT_SIGNED = 0x7FFFFFFF        /* Force to int size */
} dsiNicmLinkId;

/* netmgr link to dsi iface id mapping */
typedef struct
{
  dsiNicmLinkId    nicmLink;
  int                 dsi_iface_id;
} dsiNicmLinkMap;

extern dsiNicmLinkMap dsiNicmLinkMapTbl[];

typedef struct {
  unsigned int                     flow_id;
  char                             flow_type;
} dsiNicmNLFlowInfo;

/* Must match layout of ifa_cacheinfo */
typedef struct {
  unsigned int   prefered;
  unsigned int   valid;
  unsigned int   cstamp;
  unsigned int   tstamp;
} dsiNicmNLCacheInfo;

typedef enum {
  DSI_NICM_ADDR_TYPE_DEFAULT,
  DSI_NICM_ADDR_TYPE_CANDIDATE,
  DSI_NICM_ADDR_TYPE_MULTIHOME,
  DSI_NICM_ADDR_TYPE_MAX,
} dsiNicmAddrType;

typedef struct {
  struct ds_sockaddr_storage        ip_addr;
  unsigned int                   mask;
} dsiNicmNLAddr;

typedef struct {
  unsigned int                   flags;
  dsiNicmNLAddr               addr;
  dsiNicmNLCacheInfo          cache_info;
  uint8_t                        ext_ip;
  dsiNicmAddrType               address_type;
} dsiNicmNLAddrInfo;

typedef enum
{
  DSI_NICM_INTERFACE_LOOKUP  = 1,
  DSI_NICM_DATAPATH_LOOKUP   = 2
}dsiNicmQmiWdsRouteLookupType;

typedef struct
{
  /* Bitmask which indicates which of the below
  ** parameters has been set
  */
  /*Mandatory only valid for route lookup*/
  dsiNicmQmiWdsRouteLookupType      route_lookup;
  unsigned long                        params_mask;
  /* Parameters, more will be added later */
  unsigned char                        profile_index;
  unsigned char                        profile_index_3gpp2;
  unsigned char                        tech_pref;
  /*xtended_tech_pref is wds_technology_name_enum_v01*/
  int                                  xtended_tech_pref;
  /*ip_family_pref is wds_ip_family_preference_enum_v01*/
  int                                  ip_family_pref;
  /*DSI_NICM_QMI_WDS_APN_NAME_MAX_V01 is same as QMI_WDS_APN_NAME_MAX_V01*/
  char                                 apn_name[DSI_NICM_QMI_WDS_APN_NAME_MAX_V01 + 1];
  /*data_call_origin is wds_call_type_enum_v01*/
  int                                  data_call_origin;
  int                                  bringup_by_apn_name; /* Call bringup mode */
  int                                  bringup_by_apn_type; /* Call bringup mode */
  int                                  apn_type;            /* APN type */
  uint64_t                             apn_type_mask;       /* APN type mask */
} dsiNicmQmiWdsRouteLookupParam;

typedef struct
{
  unsigned int is_qmi_inst_valid;
  unsigned int is_companion_ip;
  int qmi_inst;
  int link;
  int pmodem;
  int ptech;
} dsiNicmQmiWdsRouteLookupResp;

typedef struct
{
  char device_name[IFNAMSIZ + 1];
  int  device_name_valid;
} dsiNicmQmiMatchingCallCheckResp;

typedef struct
{
  unsigned int v4_mtu;
  unsigned int v6_mtu;
} dsiNicmV4V6Mtu;

typedef struct
{
  char wlan_iface_name[IFNAMSIZ];
} dsiNicmOemProxyReqParams;

typedef struct
{
  int oemprxy_iface_id;
} dsiNicmOemProxyRespParam;

typedef struct
{
  unsigned int pid;
  unsigned int txn_id;
  int txn_status;
}dsiNicmUsrCmdTxn;

typedef enum
{
  DSI_NICM_POST_RESTART_STATE_INVALID = -1,
  DSI_NICM_POST_RESTART_STATE_INIT_START = 0,
  DSI_NICM_POST_RESTART_STATE_INIT_ONGOING = 1,
  DSI_NICM_POST_RESTART_STATE_INIT_DONE = 2,
  DSI_NICM_POST_RESTART_STATE_MAX
} dsiNicmRestartState;

typedef struct {
  dsiNicmUsrCmdTxn txn;
  unsigned int cmd_id;

  union{
    struct {
      /* ip_family to enable or disable forwarding
         For QUERY_PORT_FORWARDING cmd it stores the family
              for which forwarding is enabled */
      int ip_family;
    }port_forwarding_data;
    unsigned int iface_mtu;
    uint64_t network_handle;
    dsiNicmV4V6Mtu v4_v6_mtu;
    dsiNicmOemProxyRespParam netmgr_oemprxy_resp_params;
  }data;

  unsigned int                          link;
  unsigned int                          pdu_id;
  dsiNicmAddrType                    address_type;
  dsiNicmQmiWdsRouteLookupParam      rl_params;
  dsiNicmQmiWdsRouteLookupResp       rl_resp;
  int                                   app_type;
  int                                   modem_subs_id;
  uint8_t                               ignore_subs;
  dsiNicmRestartState                nicm_restart_state;
  dsiNicmOemProxyReqParams           netmgr_oemprxy_req_params;
  dsiNicmQmiMatchingCallCheckResp    matching_call_check_resp;
}dsiNicmUserCmdData;

/* For now we have defined the status as a simple success / failure
   If we want to add further specific error codes that need to
   be handled differently we can expand the enum list */
typedef enum
{
  DSI_NICM_OP_STATUS_CODE_INVALID = -1,
  DSI_NICM_OP_STATUS_CODE_SUCCESS,  /* Default success, most likely will not be seen by clients */
  DSI_NICM_OP_STATUS_CODE_FAIL,
  DSI_NICM_OP_STATUS_CODE_MAX
} dsiNicmOpStatusCode;

typedef enum
{
  DSI_NICM_OP_STATUS_IP_FAMILY_IGNORE,
  DSI_NICM_OP_STATUS_IP_FAMILY_V4,
  DSI_NICM_OP_STATUS_IP_FAMILY_V6,
  DSI_NICM_OP_STATUS_IP_FAMILY_MAX
} dsiNicmOpStatusIpFamily;

typedef struct
{
  dsiNicmOpStatusIpFamily op_ip_family;
  dsiNicmOpStatusCode op_code;
} dsiNicmOpStatusInfo;

typedef struct {
  unsigned int                param_mask;
  dsiNicmNLEvent           event;
  dsiNicmLinkId            link;
  dsiNicmNLFlowInfo        flow_info;
  dsiNicmNLAddrInfo        addr_info;
  dsiNicmNLAddr            gtwy_info;
  struct ds_sockaddr_storage  dnsp_addr;
  struct ds_sockaddr_storage  dnss_addr;
  char                        dev_name[ IFNAMSIZ ];
  unsigned int                mtu;
  unsigned int                mtu4;
  unsigned int                mtu6;
  unsigned int                user_cmd;
  unsigned int                pdu_id;
  dsiNicmUserCmdData       cmd_data;
  dsiNicmOpStatusInfo      op_status;
} dsiNicmNLEventInfo;

/*===========================================================================
FUNCTION: dsiInitNicmClient
===========================================================================*/
/*!
  @brief
  Initializes NICM client

  @return
  DSI_SUCCESS
  DSI_ERROR
  */
/*=========================================================================*/
extern int dsiInitNicmClient(void);

/*===========================================================================
FUNCTION: dsiDeInitNicmClient

===========================================================================*/
/*!
@brief
Releases Nicm client

@return
DSI_SUCCESS
DSI_ERROR

=========================================================================*/
extern int dsiDeInitNicmClient(void);

/*===========================================================================
FUNCTION:  dsiInitQueryNicmWrapper
===========================================================================*/
/*!
@brief
  wrapper function to check if nicm is inited or not.
    @return
    NICM_SUCCESS
    NICM_FALIURE
*/
/*=========================================================================*/
extern int dsiInitQueryNicmWrapper(NicmClient *nicm_clnt_hndl);

/*===========================================================================
  FUNCTION:  dsiNicmCb
===========================================================================*/
/*!
    @brief
    Callback function registered with NICM

    @return
    void
*/
/*=========================================================================*/
extern void dsiNicmCb(void *info, void *userdata);

/*===========================================================================
  FUNCTION:  dsiTranslateNicmEvent
===========================================================================*/
/*!
    @brief
    Translate NICM events into DSI
*/
/*=========================================================================*/
extern int dsiTranslateNicmEvent
(
  NicmEventInfo *eventInfo,
  dsiNicmNLEventInfo *info,
  int *dsi_event
);

/*===========================================================================
  FUNCTION:  dsiTranslateRouteLookupRequest
===========================================================================*/
/*!
    @brief
    Translate routelookup params to nicm qtructures
*/
/*=========================================================================*/
extern int dsiTranslateRouteLookupRequest
(
  int modemSubsId,
  dsi_qmi_wds_route_look_up_params_type *dsi_rl_params,
  ClientCmdInfo                         *nicm_req
);

/*===========================================================================
  FUNCTION:  dsiNicmRouteLookup
===========================================================================*/
/*!
    @brief
    Send Route lookup query to Nicm
*/
/*=========================================================================*/
extern int dsiNicmRouteLookup
(
  NicmClient *nicm_clnt_hndl,
  int modemSubsId,
  dsiNicmNLEventInfo *resp_info,
  dsi_qmi_wds_route_look_up_params_type* route_lookup_params,
  int userCmd
);

extern void *setNicmInfo(void *info);

/*===========================================================================
  FUNCTION:  dsiNicmQueryLLNetworkHndl
===========================================================================*/
/*!
    @brief
    Query link-local network handle
*/
/*=========================================================================*/
extern int dsiNicmQueryLLNetworkHndl
(
  NicmClient *nicm_clnt_hndl,
  uint64_t *networkHndl
);

/*===========================================================================
  FUNCTION:  dsiNicmGetLinkMtu
===========================================================================*/
/*!
    @brief
    Query link MTU
*/
/*=========================================================================*/
int dsiNicmGetLinkMtu
(
  NicmClient *nicm_clnt_hndl,
  int linkId,
  uint32_t pduId,
  MtuInfo *mtuInfo
);

int dsiNicmLinkRelease
(
  NicmClient *nicm_clnt_hndl,
  int link
);

/*===========================================================================
  FUNCTION:  dsiNicmProcessPFRequest
===========================================================================*/
/*!
    @brief
    Send port forwading request to Nicm
*/
/*=========================================================================*/
int dsiNicmProcessPFRequest
(
  NicmClient           *nicm_clnt_hndl,
  int                     cmd,
  int                     ipFamily,
  dsiNicmNLEventInfo  *resp_info
);

/*===========================================================================
  FUNCTION:  dsiNicmOEMProxy
===========================================================================*/
/*!
    @brief
    Send OEM Proxy request to Nicm
*/
/*=========================================================================*/
int dsiNicmOEMProxy
(
  NicmClient          *nicm_clnt_hndl,
  char                   *wlanIfaceName,
  dsiNicmNLEventInfo *resp_info
);

/*===========================================================================
  FUNCTION:  dsiNicmProcessScreenState
===========================================================================*/
/*!
    @brief
    Set screen to ON/OFF
*/
/*=========================================================================*/
int dsiNicmProcessScreenState
(
  NicmClient          *nicm_clnt_hndl,
  int                    cmd
);

static int dsiNicmStoreNetReconfig
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
);

dsi_net_evt_t dsiNicmStoreEventData
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo * info
);

int dsiNicmMapLink
(
  dsiNicmLinkId nicmLink,
  int * dsi_iface_id
);

int dsiNicmMapReverseLink
(
  dsiNicmLinkId nicmLink,
  int * dsi_iface_id
);

void dsiNicmPostEvent
(
  int dsi_iface_id,
  dsi_net_evt_t event,
  const dsiNicmNLEventInfo * info
);
