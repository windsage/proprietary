/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once
#include <map>
#include <mutex>
#include <thread>
#include <cstring>
#include <iostream>
#include "xm_glink_transport.h"
#include "xm_kp_transport.h"
#include "xm_ipc_if.h"
#include "xm_main.h"

#define BTM_MAX_PKT_SIZE				255
#define MAX_BUF_SIZE					BTM_MAX_PKT_SIZE
typedef uint32_t xm_pkt_opcode;
typedef uint32_t xm_pkt_len;

typedef enum conn_proxy_cop_ver {
    CONN_PROXY_COP_INVALID = 0,
    CONN_PROXY_COP_V0,
    CONN_PROXY_COP_V1,
    CONN_PROXY_COP_V2,
    CONN_PROXY_COP_NOT_USED
} conn_proxy_cop_ver_t;

enum CodecID {
    CODEC_ID_SBC       =          0x00,
    CODEC_ID_AAC       =          0x01,
    CODEC_ID_LDAC      =          0x02,
    CODEC_ID_APTX      =          0x03,
    CODEC_ID_APTX_HD   =          0x04,
    CODEC_ID_APTX_AD   =          0x05,
    CODEC_ID_LC3   =              0x06,
    CODEC_ID_APTX_AD_SPEECH   =   0x07,
    CODEC_ID_LC3_SPEECH   =       0x08,
    CODEC_ID_APTX_R3   =          0x09,
    CODEC_ID_APTX_R4   =          0x0A,
    CODEC_ID_RVP   =              0x0B,
    CODEC_ID_SSC   =              0x0C,
    CODEC_ID_LHDC   =             0x0D,
    CODEC_ID_NO_CODEC   =         0x0E
};

enum CopScenario
{
    A2DP_SRC_DEFAULT = 0,
    A2DP_SRC_APTXAA_LHDC,
    A2DP_SRC_ABR,
    A2DP_SINK,
    eSCO_SWB,
    ISO,
};
/* Includes 4 bytes opcode and 4 byte len */
#define BTM_OPCODE_LEN       sizeof(xm_pkt_len)
#define BTM_PKT_HEADER_LEN   (sizeof(xm_pkt_opcode) + sizeof(xm_pkt_opcode))

#define BTM_KP_CTRL_OFFSET                                    0x50000000 
#define BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ           0x50000000
#define BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP           0x50000001
#define BTM_KP_CTRL_MASTER_CONFIG_REQ                         0x50000002
#define BTM_KP_CTRL_MASTER_CONFIG_RSP                         0x50000003
#define BTM_KP_CTRL_MASTER_SHUTDOWN_REQ                       0x50000004
#define BTM_KP_CTRL_MASTER_SHUTDOWN_RSP                       0x50000005

#define BTM_BTFMCODEC_CONFIG_DMA_REQ                          0x58000006
#define BTM_BTFMCODEC_CONFIG_DMA_RSP                          0x58000007
#define BTM_BTFMCODEC_USECASE_START_IND                       0x58000008

/* Indication Messages */
#define BTM_KP_CTRL_BEARER_SWITCH_IND                         0x58000001
#define BTM_KP_CTRL_TRANSPORT_SWITCH_FAILED_IND               0x58000002
#define BTM_KP_CTRL_LOG_LVL_IND                               0x58000004
#define BTM_BTFMCODEC_ADSP_STATE_IND			      0x58000003
#define BTM_KP_CTRL_MAX_OFFSET                                0x5FFFFFFF

/* Indication sent from XM to CP */
#define BTM_CP_CTRL_OFFSET                                    0x20000000
#define BTM_CP_CTRL_BEARER_SWITCH_IND                         0x20000001
#define BTM_CP_CTRL_COP_VER_IND                               0x20000002
#define BTM_CP_CTRL_TWT_EST_IND                               0x20000003
#define BTM_CP_CTRL_HOST_PARAMETERS_IND                       0x20000004
#define BTM_CP_CTRL_BEARER_PREFERENCE_IND                     0x20000005
#define BTM_CP_CTRL_DATA_LOG_MASK_IND                         0x20000006
#define BTM_CP_CTRL_UPDATE_AUDIO_MODE_IND                     0x20000007
#define BTM_CP_CTRL_REMOTE_PARAMS_IND                         0x20000008
#define BTM_CP_CTRL_ENCODER_LIMTS_IND                         0x2000000C
#define BTM_CP_CTRL_ADSP_STATE_IND                            0x21000002

/* Request sent from XM to CP */
#define BTM_CP_CTRL_MASTER_CONFIG_REQ                         0x20001000
#define BTM_CP_CTRL_MASTER_SHUTDOWN_REQ                       0x20001001
#define BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ           0x20001002 
#define BTM_CP_CTRL_CODEC_DMA_REQ                             0x20001003
#define BTM_CP_CTRL_BURST_INTERVAL_REQ                        0x20001004

/* Response sent from XM to CP */
#define BTM_CP_CTRL_BEARER_SWITCH_RSP                         0x20002000
#define BTM_CP_CTRL_BURST_INTERVAL_RSP                        0x20002001

/* Indication sent from CP to XM */
#define BTM_CP_CTRL_DELAY_REPORTING_IND                       0x21000000
#define BTM_CP_CTRL_TRANSPORT_UPDATE_IND                      0x21000001

/* Request sent from CP to XM */
#define BTM_CP_CTRL_BEARER_SWITCH_REQ                         0x21001000

/* Response sent from CP to XM */
#define BTM_CP_CTRL_MASTER_CONFIG_RSP                         0x21002000
#define BTM_CP_CTRL_MASTER_SHUTDOWN_RSP                       0x21002001
#define BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP           0x21002002 
#define BTM_CP_CTRL_CODEC_DMA_RSP                             0x21002003

#define BTM_CP_CTRL_MAX_OFFSET                                0x2FFFFFFF

#define BTM_CTRL_MASTER_CONFIG_REQ_PKT_LEN                    (22)
#define BTM_CTRL_MASTER_CONFIG_RSP_PKT_LEN                    (10)
#define BTM_CTRL_MASTER_SHUTDOWN_REQ_PKT_LEN                  (9)
#define BTM_CTRL_MASTER_SHUTDOWN_RSP_PKT_LEN                  (10)
#define BTM_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP_LEN          (10)
#define BTM_XP_TWT_BASE_LEN                                   (1)
#define BTM_XP_REMOTE_EB_PARAMS_BASE_LEN                      (1)
#define BTM_XP_REMOTE_IPV4_PARAMS_LEN                         (65)
#define BTM_XP_REMOTE_IPV6_PARAMS_LEN                         (BTM_XP_REMOTE_IPV4_PARAMS_LEN + IPv6_SIZE)
#define BTM_XP_EBPARAMS_IPV4_PARAMS_LEN                       (21)
#define BTM_XP_EBPARAMS_IPV6_PARAMS_LEN                       (BTM_XP_EBPARAMS_IPV4_PARAMS_LEN + IPv6_SIZE)
#define BTM_XP_TWT_PAYLOAD_LEN                                (18)
#define BTM_CP_CTRL_DELAY_REPORTING_LEN                       (12)
#define BTM_CP_CTRL_TRANSPORT_UPDATE_LEN                      (9)
#define BTM_CP_CTRL_BEARER_SWITCH_REQ_LEN                     (9)
#define BTM_KP_ADSP_STATE_IND_LEN			      (12)
#define BTM_CP_ADSP_STATE_IND_LEN			      (9)
#define BTM_BTFMCODEC_CONFIG_DMA_REQ_LEN                      (20)
#define BTM_CP_CONFIG_DMA_RSP_LEN                             (10)
#define BTM_USECASE_START_LEN                                 (9)

/* ADSP States */
#define BTM_ADSP_SSR_AFTER_POWERUP			      (1)

/* Cop Version Def */
#define SINK_ID 2
#define SOURCE_ID 1
#define CopV0Support 0x01
#define CopV1Support 0x02
#define CopV2Support 0x04

#define STREAM_TO_UINT8(u8, pp) \
{                               \
  (u8) = (uint8_t)(*(pp));      \
  (pp) += 1;                    \
}

#define STREAM_TO_UINT16(u16, pp)                            \
{                                                            \
  (u16) = ((uint16_t)(*(pp)) + (((uint16_t)(*((pp) + 1))) << 8)); \
  (pp) += 2;                                                     \
}

#define STREAM_TO_UINT32(u32, pp)                                    \
{                                                                   \
  (u32) = (((uint32_t)(*(pp))) + ((((uint32_t)(*((pp) + 1)))) << 8) + \
             ((((uint32_t)(*((pp) + 2)))) << 16) +                   \
             ((((uint32_t)(*((pp) + 3)))) << 24));                   \
  (pp) += 4;                                                       \
}

#define STREAM_TO_OPCODE(u32, pp)                                    \
{                                                                   \
  (u32) = (((uint32_t)(*(pp))) + ((((uint32_t)(*((pp) + 1)))) << 8) + \
             ((((uint32_t)(*((pp) + 2)))) << 16) +                   \
             ((((uint32_t)(*((pp) + 3)))) << 24));                   \
}

#define STREAM_TO_LENGTH(u32, pp)                                    \
{                                                                   \
  (u32) = (((uint32_t)(*(pp))) + ((((uint32_t)(*((pp) + 1)))) << 8) + \
             ((((uint32_t)(*((pp) + 2)))) << 16) +                   \
             ((((uint32_t)(*((pp) + 3)))) << 24));                   \
}

#define UINT32_TO_STREAM(p, u32)     \
{                                  \
  *(p) = (uint8_t)(u32);         \
  *((p)+ 1) = (uint8_t)((u32) >> 8);  \
  *((p)+ 2) = (uint8_t)((u32) >> 16); \
  *((p)+ 3) = (uint8_t)((u32) >> 24); \
}

#define UINT8_TO_STREAM(p, u8)     \
{                                  \
  *(p) = (uint8_t)(u8);         \
}

#define UINT16_TO_STREAM(p, u32)     \
{                                  \
  *(p) = (uint8_t)(u32);         \
  *((p)+ 1) = (uint8_t)((u32) >> 8);  \
}

#define UINT48_TO_STREAM(p, u48)     \
{                                  \
  *(p) = (uint8_t)(u48);         \
  *((p)+ 1) = (uint8_t)((u48) >> 8);  \
  *((p)+ 2) = (uint8_t)((u48) >> 16); \
  *((p)+ 3) = (uint8_t)((u48) >> 24); \
  *((p)+ 4) = (uint8_t)((u48) >> 32);  \
  *((p)+ 5) = (uint8_t)((u48) >> 40); \
}

static inline char *OpcodeToString(uint32_t opcode)
{
  if (opcode == BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP)
    return "Rx prepare audio bearer rsp from KP";
  else if(opcode == BTM_KP_CTRL_BEARER_SWITCH_IND)
    return "Sending bearer switch ind to KP";
  else if (opcode == BTM_CP_CTRL_BEARER_SWITCH_IND)
    return "Sending bearer switch ind to CP";
  else if (opcode == BTM_CP_CTRL_UPDATE_AUDIO_MODE_IND)
    return "Sending current usecase to CP";
  else if (opcode == BTM_CP_CTRL_COP_VER_IND)
    return "Sending CoP version to ";
  else if (opcode == BTM_CP_CTRL_TWT_EST_IND)
    return "Sending TWT establish ind to CP";
  else if (opcode == BTM_CP_CTRL_HOST_PARAMETERS_IND)
    return "Sending host paramters to CP";
  else if (opcode == BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ)
    return "Sending prepare audio bearer req to CP";
  else if (opcode == BTM_CP_CTRL_CODEC_DMA_REQ)
    return "BTM_CP_CTRL_CODEC_DMA_REQ";
  else if (opcode == BTM_CP_CTRL_BURST_INTERVAL_REQ)
    return "BTM_CP_CTRL_BURST_INTERVAL_REQ";
  else if (opcode == BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ)
    return "Sending prepare audio bearer req to KP";
  else if (opcode == BTM_CP_CTRL_DATA_LOG_MASK_IND)
    return "Sending data log mask ind to cp";
  else if (opcode == BTM_CP_CTRL_BEARER_SWITCH_REQ)
    return "BTM_CP_CTRL_BEARER_SWITCH_REQ";
  else if (opcode == BTM_CP_CTRL_BEARER_SWITCH_RSP)
    return "BTM_CP_CTRL_BEARER_SWITCH_RSP";
  else if (opcode == BTM_CP_CTRL_BURST_INTERVAL_RSP)
    return "BTM_CP_CTRL_BURST_INTERVAL_RSP";
  else if (opcode == BTM_CP_CTRL_BEARER_PREFERENCE_IND)
    return "BTM_CP_CTRL_BEARER_PREFERENCE_IND";
  else if (opcode  == BTM_KP_CTRL_MASTER_CONFIG_REQ)
    return "BTM_KP_CTRL_MASTER_CONFIG_REQ";
  else if (opcode  == BTM_KP_CTRL_MASTER_CONFIG_RSP)
    return "BTM_KP_CTRL_MASTER_CONFIG_RSP";
  else if (opcode  == BTM_KP_CTRL_MASTER_SHUTDOWN_REQ)
    return "BTM_KP_CTRL_MASTER_SHUTDOWN_REQ";
  else if (opcode  == BTM_KP_CTRL_MASTER_SHUTDOWN_RSP)
    return "BTM_KP_CTRL_MASTER_SHUTDOWN_RSP";
  else if (opcode  == BTM_KP_CTRL_TRANSPORT_SWITCH_FAILED_IND)
    return "BTM_KP_CTRL_TRANSPORT_SWITCH_FAILED_IND";
  else if (opcode  == BTM_KP_CTRL_LOG_LVL_IND)
    return "BTM_KP_CTRL_LOG_LVL_IND";
  else if (opcode  == BTM_BTFMCODEC_ADSP_STATE_IND)
    return "BTM_BTFMCODEC_ADSP_STATE_IND";
  else if (opcode  == BTM_CP_CTRL_ENCODER_LIMTS_IND)
    return "BTM_CP_CTRL_ENCODER_LIMTS_IND";
  else if (opcode  == BTM_CP_CTRL_ADSP_STATE_IND)
    return "BTM_CP_CTRL_ADSP_STATE_IND";
  else if (opcode  == BTM_CP_CTRL_MASTER_CONFIG_REQ)
    return "BTM_CP_CTRL_MASTER_CONFIG_REQ";
  else if (opcode  == BTM_CP_CTRL_MASTER_SHUTDOWN_REQ)
    return "BTM_CP_CTRL_MASTER_SHUTDOWN_REQ";
  else if (opcode  == BTM_CP_CTRL_DELAY_REPORTING_IND)
    return "BTM_CP_CTRL_DELAY_REPORTING_IND";
  else if (opcode  == BTM_CP_CTRL_TRANSPORT_UPDATE_IND)
    return "BTM_CP_CTRL_TRANSPORT_UPDATE_IND";
   else if (opcode  == BTM_CP_CTRL_MASTER_CONFIG_RSP)
    return "BTM_CP_CTRL_MASTER_CONFIG_RSP";
  else if (opcode  == BTM_CP_CTRL_MASTER_SHUTDOWN_RSP)
    return "BTM_CP_CTRL_MASTER_SHUTDOWN_RSP";
  else if (opcode  == BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP)
    return "BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP";
  else if (opcode  == BTM_CP_CTRL_CODEC_DMA_RSP)
    return "BTM_CP_CTRL_CODEC_DMA_RSP";
  else if (opcode  == BTM_BTFMCODEC_CONFIG_DMA_REQ)
    return "BTM_BTFMCODEC_CONFIG_DMA_REQ";
  else if (opcode  == BTM_BTFMCODEC_CONFIG_DMA_RSP)
    return "BTM_BTFMCODEC_CONFIG_DMA_RSP";
  else if (opcode  == BTM_BTFMCODEC_USECASE_START_IND)
    return "BTM_BTFMCODEC_USECASE_START_IND";
  else if (opcode  == BTM_CP_CTRL_REMOTE_PARAMS_IND)
    return "BTM_CP_CTRL_REMOTE_PARAMS_IND";
  else
    return "INVALID Opcode";
}
namespace xpan {
namespace implementation {

class XMPacketizer
{
 public:
  XMPacketizer();
  ~XMPacketizer();
  void OnDataReady(int);
  void ProcessMessage(XmIpcEventId, xm_ipc_msg_t *);
  void KpBearerSwitchInd(RspStatus);
  void CpBearerSwitchInd(TransportType, RspStatus);
  void BearerSwitchRsp(TransportType, RspStatus);
  void PrepareAudioBearerReqtoCp(TransportType);
  void PrepareAudioBearerReqtoKp(TransportType);
  void BearerPreferenceInd(TransportType);
 private:
  void decode_and_dispatch_kp_pkt(uint32_t, uint32_t, uint8_t *);
  void decode_and_dispatch_cp_pkt(uint32_t, uint32_t, uint8_t *);
  conn_proxy_cop_ver_t conn_proxy_transport_mngr_sink_get_cop_ver(uint8_t, uint32_t);
  conn_proxy_cop_ver_t conn_proxy_transport_mngr_src_get_cop_ver(uint8_t, uint32_t);
  conn_proxy_cop_ver_t conn_proxy_transport_mngr_get_cop_ver(uint8_t, uint32_t, uint8_t);
  void log_pkt(uint32_t, uint8_t *);
  void decode_and_dispatch(uint32_t, uint32_t, uint8_t *);
  bool isStubEnabled();
};

} // namespace implementation
} // namespace xpan
