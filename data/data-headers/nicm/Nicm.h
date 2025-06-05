/*===========================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#ifndef _NICM_H_
#define _NICM_H_

#include <sys/socket.h>
#include <linux/if.h>

#define NICM_SUCCESS (0)
#define NICM_FAILURE (-1)

#define CLIENT_CMD_SUCCESS (1)
#define CLIENT_CMD_FAILURE (-1)

#define MAX_APN_NAME_LEN (150)
#define MAX_QMIPRIOD_CMD_LEN  (256)

#define NICM_IF_NAME_MAX_LEN  16

/* Target specific defines */
#ifdef FEATURE_DS_LINUX_ANDROID

#define NICM_CONNECT_PATH "/dev/socket/nicmd/nicm_connect"

#else /* Default to IPQ */

#define NICM_CONNECT_PATH "/var/run/nicm_connect"

#endif  /* FEATURE_DS_LINUX_ANDROID */

#ifdef __cplusplus
  #define C_LINKAGE extern "C"
#else
  #define C_LINKAGE
#endif /* __cplusplus */

#define NICM_IPTABLES_FLAGS " -w "

typedef enum
{
  CLIENT_CMD_INVALID = -1,
  CLIENT_CMD_SYSTEM_READY_REQ,
  CLIENT_CMD_QUERY_ROUTE_LOOKUP,
  CLIENT_CMD_LINK_RELEASE,
  CLIENT_CMD_QUERY_LINK_MTU,
  CLIENT_CMD_QUERY_NETWORK_HANDLE,
  CLIENT_CMD_QUERY_MATCHING_CALL_IFACE,
  CLIENT_CMD_ENABLE_PORT_FORWARDING,
  CLIENT_CMD_DISABLE_PORT_FORWARDING,
  CLIENT_CMD_QUERY_PORT_FORWARDING,
  CLIENT_CMD_SCREEN_OFF,
  CLIENT_CMD_SCREEN_ON,
  CLIENT_CMD_MAX
} NicmClientCommands;

typedef struct
{
  unsigned int mPid;
  unsigned int mTxnId;
  int          mTxnStatus;
} ClientCmdTxn;

typedef enum
{
  INTERFACE_LOOKUP = 1,
  DATAPATH_LOOKUP = 2
} RouteLookupType;

typedef enum
{
  CALL_REQ_MODE_DEFAULT = 0,
  CALL_REQ_MODE_LOW_LATENCY = 1
} CallReqMode;

// NOTE: This structure definition needs to match the one in QmiWdsClient
typedef struct
{
  RouteLookupType mType;
  unsigned long   mParamMask;
  unsigned char   mProfileIndex;
  unsigned char   mProfileIndex2;
  unsigned char   mTechPref;
  int             mExtendedTechPref;
  int             mIpFamilyPref;
  char            mApnName[MAX_APN_NAME_LEN + 1];
  int             mDataCallOrigin;
  CallReqMode     mReqMode;
  int             mBringupByApnName; /*bring up by apn name*/
  int             mBringupByApnType; /*bring up by apn type*/
  int             mApnType;
  uint64_t        mApnTypeMask;
} RouteLookupReqInfo;

typedef struct
{
  unsigned int mIsQmiInstValid;
  unsigned int mIsCompanionIpCall;
  int          mQmiInst;
  int          mLinkId;
  int          mMuxId;
  int          mModem;
  int          mTech;
} RouteLookupRespInfo;

typedef struct
{
  unsigned int mMtu;
  unsigned int mMtu4;
  unsigned int mMtu6;
} MtuInfo;

typedef struct
{
  /*  ip_family to enable or disable forwarding
      For QUERY_PORT_FORWARDING cmd it stores the family
      for which forwarding is enabled */
  int mIpFamily;
} PortForwardingData;

typedef struct
{
  char wlanIfaceName[IFNAMSIZ];
} OemProxyReqInfo;

typedef struct
{
  int oemprxyIfaceId;
} OemProxyRespInfo;

typedef struct
{
  ClientCmdTxn           mTxn;
  NicmClientCommands  mCmd;
  int                    mSubsId;
  int                    mLinkId;
  uint32_t               mPduId;
  RouteLookupReqInfo     mRouteLookupReq;
  RouteLookupRespInfo    mRouteLookupResp;
  PortForwardingData     mPortForwardingData;
  OemProxyReqInfo        mOemProxyReqInfo;
  OemProxyRespInfo       mOemProxyRespInfo;
} ClientCmdInfo;

typedef struct
{
  uint32_t mRmnetFlags;
  uint32_t mDlMinimumPadding;
} VNDFlags;

typedef enum
{
  EVENT_INVALID = -1,
  EVENT_SYSTEM_READY_RESP,
  EVENT_CLIENT_CMD,
  EVENT_IFACE_UP,
  EVENT_IFACE_DOWN,
  EVENT_IFACE_RECONFIGURED,
  EVENT_IFACE_DELADDR,
  EVENT_IFACE_NEWADDR,
  EVENT_MTU_UPDATE,
  EVENT_EXT_IP_CONFIG,
  EVENT_FLOW_ACTIVATED,
  EVENT_FLOW_DELETED,
  EVENT_FLOW_MODIFIED,
  EVENT_FLOW_SUSPENDED,
  EVENT_FLOW_ENABLED,
  EVENT_FLOW_DISABLED,
  Event_PDU_AVAILABLE,
  Event_PDU_PROMOTION,
  Event_PDU_RELEASE,
  Event_PDU_RECONFIGURE,
  EVENT_OEM_PROXY_IFACEUP,
  EVENT_OEM_PROXY_IFACEDOWN,

  /* EV_INTERFACE_IND */
  EVENT_VENDOR_HOOK_NICM_RESTART,/* New Global Event */
  EVENT_VENDOR_HOOK_IFACEUP, /* Dup of EVENT_IFACE_UP */
  EVENT_VENDOR_HOOK_IFACEDOWN, /* Dup of EVENT_IFACE_DOWN */
  EVENT_VENDOR_HOOK_RECONFIG, /* Dup of EVENT_IFACE_RECONFIGURED */
  EVENT_VENDOR_HOOK_MTU_UPDATE, /* Dup of EVENT_MTU_UPDATE */
  EVENT_VENDOR_HOOK_MODEM_IS, /* New Global Event */
  EVENT_VENDOR_HOOK_MODEM_OOS, /* New Global Event */
  EVENT_VENDOR_HOOK_CMD, /* Vendor command */

  EVENT_OP_FAILED,
  EVENT_NICM_RESTART,
  EVENT_MAX
} NicmEvents;

typedef struct
{
  unsigned int mPrefered;
  unsigned int mValid;
  unsigned int mCstamp;
  unsigned int mTstamp;
} CacheInfo;

typedef struct
{
  struct sockaddr_storage  mAddr;
  unsigned int             mMask;
} Addr;

typedef enum
{
  PDU_TYPE_PRIMARY,
  PDU_TYPE_SECONDARY,
  PDU_TYPE_MAX
} PduType;

typedef enum
{
  ADDR_TYPE_DEFAULT,
  ADDR_TYPE_CANDIDATE,
  ADDR_TYPE_MULTIHOME,
  ADDR_TYPE_MAX
} AddrType;

typedef struct
{
  unsigned int mFlags;
  Addr         mAddr;
} AddrInfo;

typedef enum
{
  MSG_TYPE_REQ,
  MSG_TYPE_RESP
} UlPrioMsgType;

typedef struct
{
  UlPrioMsgType type;
  union
  {
    char cmdStr[MAX_QMIPRIOD_CMD_LEN];
    int  cmdResp;
  } data;
} ULPrioInfo;

/* Param Masks for NICM Encoded Messages */
typedef enum
{
  MSG_PARAM_MASK_NONE         = 0x0000,
  MSG_PARAM_MASK_EVENT_INFO   = 0x0001,
  MSG_PARAM_MASK_LINK         = 0x0002,
  MSG_PARAM_MASK_ADDR_INFO    = 0x0004,
  MSG_PARAM_MASK_GTWY_INFO    = 0x0008,
  MSG_PARAM_MASK_DNSP_ADDR    = 0x0010,
  MSG_PARAM_MASK_DNSS_ADDR    = 0x0020,
  MSG_PARAM_MASK_DEV_NAME     = 0x0040,
  MSG_PARAM_MASK_CLIENT_CMD   = 0x0080,
  MSG_PARAM_MASK_MTU          = 0x0100,
  MSG_PARAM_MASK_PDU_ID       = 0x0200,
  MSG_PARAM_MASK_EXT_IP_MASK  = 0x0400,
  MSG_PARAM_MASK_CACHE_INFO   = 0x0800,
  MSG_PARAM_MASK_ADDR_TYPE    = 0x1000,
  MSG_PARAM_MASK_QOS_EVENT_INFO    = 0x2000,
  MSG_PARAM_MASK_OP_STATUS_INFO    = 0x4000,
  MSG_PARAM_MASK_LL_NETWORK_HANDLE = 0x8000,
  MSG_PARAM_MASK_MODEM_SUBS_ID     = 0x10000,
  MSG_PARAM_MASK_NICM_RESTART_INFO = 0x20000,
  MSG_PARAM_MASK_MESSAGE_ID        = 0x40000,
  MSG_PARAM_MASK_MAX
} NicmEventMask;

typedef enum
{
  INTERFACE_INVALID = -1,
  INTERFACE_FORWARD,
  INTERFACE_REVERSE,
  INTERFACE_MAX
} InterfaceLinkType;

typedef struct
{
  unsigned long             mFlowId;
  unsigned char             mFlowType;
} QosEventInfo;

typedef enum
{
  NICM_OP_STATUS_CODE_INVALID = -1,
  NICM_OP_STATUS_CODE_SUCCESS,  /* Default success, most likely will not be seen by clients */
  NICM_OP_STATUS_CODE_FAIL,
  NICM_OP_STATUS_CODE_MAX
} NicmOPStatusCode;

typedef enum
{
  NICM_OP_STATUS_IP_FAMILY_IGNORE,
  NICM_OP_STATUS_IP_FAMILY_V4,
  NICM_OP_STATUS_IP_FAMILY_V6,
  NICM_OP_STATUS_IP_FAMILY_MAX
} NicmOPStatusIPFamily;

typedef struct
{
  NicmOPStatusIPFamily mOPIpfamily;
  NicmOPStatusCode     mOPCode;
} NicmOPStatusInfo;

typedef enum
{
  NICM_POST_RESTART_STATE_INVALID = -1,
  NICM_POST_RESTART_STATE_INIT_START = 0,
  NICM_POST_RESTART_STATE_INIT_ONGOING = 1,
  NICM_POST_RESTART_STATE_INIT_DONE = 2,
  NICM_POST_RESTART_STATE_MAX
} NicmRestartState;

typedef struct
{
  unsigned int             mParamMask;
  InterfaceLinkType        mIfaceLinkType;
  NicmEvents               mEventType;
  int                      mLinkId;
  AddrInfo                 mAddrInfo;
  CacheInfo                mCacheInfo;
  AddrType                 mAddrType;
  Addr                     mGatewayAddr;
  struct sockaddr_storage  mDnsPAddr;
  struct sockaddr_storage  mDnsSAddr;
  char                     mDevName[IFNAMSIZ];
  MtuInfo                  mMtuInfo;
  uint32_t                 mPduId;
  uint32_t                 mExtIpMask;
  ClientCmdInfo            mClientCmdInfo;
  QosEventInfo             mQosEventInfo;
  NicmOPStatusInfo         mNicmOPStatusInfo;
  uint64_t                 mLLNetworkHandle;
  int                      mModemSubsId;
  NicmRestartState         mNicmRestartState;
  uint64_t                 mMessageId;
} NicmEventInfo;

// Callback function exposed to clients to be used for sending events
// This is a C linkage function pointer
typedef void (*ClientCallback)(void *info, void *userdata);

typedef struct NicmClient NicmClient;

C_LINKAGE NicmClient* nicm_client_register(ClientCallback cb_f, void *userdata);
C_LINKAGE void nicm_client_release(NicmClient *clientPtr);
C_LINKAGE int nicm_send_client_cmd(NicmClient *clientPtr,
                                      NicmClientCommands cmd,
                                      ClientCmdInfo *reqInfo,
                                      NicmEventInfo *respInfo);

#endif /* _NICM_H_ */
