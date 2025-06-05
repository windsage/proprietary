/*===========================================================================
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include <linux/if.h>
#include "ds_util.h"

#include <string.h>

#define VH_NICM_LOG_ERROR(...)    \
    do{                               \
        ds_log_dflt(__VA_ARGS__);      \
    } while(0);                       \

#define VH_NICM_LOG_MED(...)      \
    do{                               \
        ds_log_med(__VA_ARGS__);      \
    } while(0);                       \

#define VH_NICM_LOG_HIGH(...)     \
    do{                               \
        ds_log_high(__VA_ARGS__);     \
    } while(0);                       \

#ifdef __cplusplus
  #define C_LINKAGE extern "C"
#else
  #define C_LINKAGE
#endif /* __cplusplus */

#define VH_INIT_WAIT_TIME_BEFORE_RETRY 100000 /* usec time interval between each NETLINK req event sent from VH */
#define VH_INIT_MAX_RETRY_COUNT  600 /* max number of retrying ping msgto nicm for its readiness */

#define VH_NICM_SUCCESS 0 /**< Indicates that the operation was successful. */
#define VH_NICM_ERROR -1  /**< Indicates that the operation was not successful. */
//#define boolean int
#define VH_NICM_FALSE 0
#define VH_NICM_TRUE 1
/*---------------------------------------------------------------------------
   Type representing NetLink event indication payload
---------------------------------------------------------------------------*/
#define VH_NICM_EVT_PARAM_NONE         (0x0000)
#define VH_NICM_EVT_PARAM_LINK         (0x0001)
#define VH_NICM_EVT_PARAM_FLOWINFO     (0x0002)
#define VH_NICM_EVT_PARAM_ADDRINFO     (0x0004)
#define VH_NICM_EVT_PARAM_GTWYINFO     (0x0008)
#define VH_NICM_EVT_PARAM_DNSPADDR     (0x0010)
#define VH_NICM_EVT_PARAM_DNSSADDR     (0x0020)
#define VH_NICM_EVT_PARAM_DEVNAME      (0x0040)
#define VH_NICM_EVT_PARAM_USER_CMD     (0x0080)
#define VH_NICM_EVT_PARAM_CMD_DATA     (0x0100)
#define VH_NICM_EVT_PARAM_MTU          (0x0200)
#define VH_NICM_EVT_PARAM_PDU_ID       (0x0400)
#define VH_NICM_EVT_PARAM_ADDR_TYPE    (0x0800)
#define VH_NICM_EVT_PARAM_MASK         ( VH_NICM_EVT_PARAM_LINK      | \
                                        VH_NICM_EVT_PARAM_FLOW      | \
                                        VH_NICM_EVT_PARAM_ADDRINFO  | \
                                        VH_NICM_EVT_PARAM_GTWYINFO  | \
                                        VH_NICM_EVT_PARAM_DNSPINFO  | \
                                        VH_NICM_EVT_PARAM_DNSSINFO  | \
                                        VH_NICM_EVT_PARAM_DEVNAME   | \
                                        VH_NICM_EVT_PARAM_USR_CMD   | \
                                        VH_NICM_EVT_PARAM_MTU       | \
                                        VH_NICM_EVT_PARAM_PDU_ID    | \
                                        VH_NICM_EVT_PARAM_ADDR_TYPE )


/*---------------------------------------------------------------------------
   Type representing enumeration of NetLink event indication messages
---------------------------------------------------------------------------*/
typedef enum {
  VH_EVT_INVALID = 0x0,              /**< Invalid event. */
  /* Update VH_EVT_str array when adding new event to this enum */
  VH_EVT_VENDOR_HOOK_NICM_RESTART,/* New Global Event */
  VH_EVT_VENDOR_HOOK_IFACEUP, /* Dup of VH_EVT_IFACE_UP */
  VH_EVT_VENDOR_HOOK_IFACEDOWN, /* Dup of VH_EVT_IFACE_DOWN */
  VH_EVT_VENDOR_HOOK_RECONFIG, /* Dup of VH_EVT_IFACE_RECONFIGURED */
  VH_EVT_VENDOR_HOOK_MTU_UPDATE, /* Dup of VH_EVT_MTU_UPDATE */
  VH_EVT_VENDOR_HOOK_MODEM_IS, /* New Global Event */
  VH_EVT_VENDOR_HOOK_MODEM_OOS, /* New Global Event */
} VhNicmEvent;

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
} VhNicmLinkId;


typedef struct {
  unsigned int                     param_mask;
  VhNicmEvent                   event;
  VhNicmLinkId                  link;
  char                             dev_name[ IFNAMSIZ ];
  unsigned int                     mtu;
  unsigned int                     mtu4;
  unsigned int                     mtu6;
} VhNicmNLEventInfo;

// Callback function exposed to clients to be used for sending events
// This is a C linkage function pointer
typedef void (*ClientCallback)(void *info, void *userdata);

#define NICM_VH_CMD_DATA_REPORT 100

typedef struct
{
  unsigned int mPid;
  unsigned int mTxnId;
  int          mTxnStatus;
} VhClientCmdTxn;

typedef struct
{
  uint8_t mMuxId;
  uint8_t mType;
  uint8_t mSum;
  uint8_t mValueLen;
  uint8_t mValue[8];
} NicmVhCmdDataReport;

typedef struct
{
  VhClientCmdTxn  mTxn;
  int             mCmd;
  union
  {
    NicmVhCmdDataReport mDataReport;
  };
} NicmVhCmdInfo;

#ifdef __cplusplus
class VhEventClient;
#else
typedef struct VhEventClient VhEventClient;
#endif /* __cplusplus */

C_LINKAGE VhEventClient *vh_event_client_register(ClientCallback vh_f, ClientCallback cb_f, void *userdata);
C_LINKAGE void vh_event_client_release(VhEventClient *clientPtr);
C_LINKAGE int vh_event_send_client_cmd(VhEventClient *clientPtr,
                                       NicmVhCmdInfo *reqInfo,
                                       NicmVhCmdInfo *respInfo);

/*===========================================================================
FUNCTION: VHInitNicmClient
===========================================================================*/
/*!
  @brief
  Initializes NICM client

  @return
  VH_SUCCESS
  VH_ERROR
  */
/*=========================================================================*/
int vhInitVhEventClient(ClientCallback cb_f);

/*===========================================================================
FUNCTION: vhDeInitVhEventClient
===========================================================================*/
/*!
@brief
Releases Nicm client

@return
VH_NICM_SUCCESS
VH_NICM_ERROR

=========================================================================*/
int vhDeInitVhEventClient(void);

/*===========================================================================
FUNCTION: vhQueryDropCount
===========================================================================*/
/*!
@brief
Query IPA drop counts for an RmNet interface. NULL interface name
queries all interfaces.

@return
VH_NICM_SUCCESS
VH_NICM_ERROR

=========================================================================*/
int vhCmdQueryDropCount(const char *ifname,
                        uint32_t *ul_drop_cnt, uint32_t *dl_drop_cnt);
