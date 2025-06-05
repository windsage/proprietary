/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <utils/Log.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "xm_packetizer.h"
#include <android-base/logging.h>
#include <cutils/properties.h>


#include <iostream>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-xmpacketizer"

namespace xpan {
namespace implementation {

XMPacketizer::XMPacketizer()
{
}

XMPacketizer::~XMPacketizer()
{
}

void XMPacketizer::decode_and_dispatch_cp_pkt(uint32_t opcode,
		                              uint32_t len, uint8_t *buf)
{
  int i;
  uint8_t *cmd;
  switch (opcode) {
    case BTM_CP_CTRL_MASTER_CONFIG_RSP: {

      ALOGI("%s: Received master configuration response", __func__);
      CHECK(BTM_CTRL_MASTER_CONFIG_RSP_PKT_LEN == (len + BTM_PKT_HEADER_LEN));

      cmd = (uint8_t *)new uint8_t[len + BTM_PKT_HEADER_LEN];
      UINT32_TO_STREAM(cmd, BTM_KP_CTRL_MASTER_CONFIG_RSP);
      UINT32_TO_STREAM((cmd + BTM_OPCODE_LEN), len);

      for (i = BTM_PKT_HEADER_LEN; i <(len + BTM_PKT_HEADER_LEN); i++)
        cmd[i]= (uint8_t)buf[i- BTM_PKT_HEADER_LEN];

      log_pkt(BTM_CP_CTRL_MASTER_CONFIG_RSP, cmd);
      KernelProxyTransport::Get()->WritetoKpTransport(cmd,
		                   BTM_CTRL_MASTER_CONFIG_RSP_PKT_LEN);
      delete []cmd;
      break;
    } case BTM_CP_CTRL_MASTER_SHUTDOWN_RSP: {
      ALOGI("%s: Received master shutdown response", __func__);
      CHECK(BTM_CTRL_MASTER_SHUTDOWN_RSP_PKT_LEN == (len + BTM_PKT_HEADER_LEN));
      cmd = (uint8_t *)new uint8_t[len + BTM_PKT_HEADER_LEN];
      UINT32_TO_STREAM(cmd, BTM_KP_CTRL_MASTER_SHUTDOWN_RSP);
      UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len);

      for (i = BTM_PKT_HEADER_LEN; i <(len +BTM_PKT_HEADER_LEN); i++)
        cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];
      log_pkt(BTM_CP_CTRL_MASTER_SHUTDOWN_RSP, cmd);
      KernelProxyTransport::Get()->WritetoKpTransport(cmd,
                       BTM_CTRL_MASTER_SHUTDOWN_RSP_PKT_LEN);
      delete []cmd;
      break;
  } case BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP: {
    ALOGI("%s: Received prepare audio bearer switch rsp from cp", __func__);
    CHECK(BTM_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP_LEN == (len + BTM_PKT_HEADER_LEN));
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->CpAudioBearerRsp.eventId = CP_XM_PREPARE_AUDIO_BEARER_RSP;
    msg->CpAudioBearerRsp.current_transport = buf[0];
    msg->CpAudioBearerRsp.status = buf[1];
    XpanManager::Get()->PostMessage(msg);
    break;
  } case BTM_CP_CTRL_DELAY_REPORTING_IND : {
    uint32_t delay_reporting;
    CHECK(BTM_CP_CTRL_DELAY_REPORTING_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT16(delay_reporting, buf);
    ALOGI("%s: Received delay reporting from cp", __func__);
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->DelayReporting.eventId = CP_XM_DELAY_REPORTING;
    msg->DelayReporting.delay_reporting = delay_reporting;
    XpanManager::Get()->PostMessage(msg);
    break;
  } case BTM_CP_CTRL_TRANSPORT_UPDATE_IND : {
    uint8_t transport;
    CHECK(BTM_CP_CTRL_TRANSPORT_UPDATE_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT8(transport, buf);
    ALOGI("%s: Received Transport update from cp with %s", __func__,
          TransportTypeToString((TransportType)transport));
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->TransportUpdate.eventId = CP_XM_TRANSPORT_UPDATE;
    msg->TransportUpdate.transport  = (TransportType)transport;
    XpanManager::Get()->PostMessage(msg);
    break;
  } case BTM_CP_CTRL_BEARER_SWITCH_REQ: {
    uint8_t transport;
    CHECK(BTM_CP_CTRL_BEARER_SWITCH_REQ_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT8(transport, buf);
    ALOGI("%s: Received bearer switch req from cp for %s", __func__,
          TransportTypeToString((TransportType)transport));
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->BearerSwitchReq.eventId = CP_XM_BEARER_SWITCH_REQ;
    msg->BearerSwitchReq.transport  = (TransportType)transport;
    XpanManager::Get()->PostMessage(msg);
    break;
  } case BTM_CP_CTRL_CODEC_DMA_RSP : {
    ALOGI("%s: Received codec DMA Rsp", __func__);
    CHECK(BTM_CP_CONFIG_DMA_RSP_LEN == (len + BTM_PKT_HEADER_LEN));
    cmd = (uint8_t *)new uint8_t[len + BTM_PKT_HEADER_LEN];
    UINT32_TO_STREAM(cmd, BTM_BTFMCODEC_CONFIG_DMA_RSP);
    UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len);

    for (i = BTM_PKT_HEADER_LEN; i <(len +BTM_PKT_HEADER_LEN); i++)
      cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];
    log_pkt(BTM_CP_CTRL_CODEC_DMA_RSP, cmd);
    KernelProxyTransport::Get()->WritetoKpTransport(cmd,
                      BTM_CP_CONFIG_DMA_RSP_LEN);
    delete []cmd;
    break;
  } case BTM_CP_CTRL_ADSP_STATE_IND: {
    uint8_t state;
    CHECK(BTM_CP_ADSP_STATE_IND_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT8(state, buf);
    ALOGI("%s: ADSP is %s", __func__, (state == 1 ? "active": "down"));
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->AdspState.eventId =  CP_XM_ADSP_STATE_IND;
    msg->AdspState.action = (uint32_t)state;
    XpanManager::Get()->PostMessage(msg);
    break;
  } default:
    ALOGE("%s: Opcode %0008x not matching", __func__, opcode);
  }
}

void XMPacketizer::OnDataReady(int fd)
{
  std::stringstream ss;
  uint8_t buffer[MAX_BUF_SIZE] = {0};
  uint32_t len = 0;
  uint32_t opcode;
  int i;
  static bool is_adsp_triggered = false;
  static int glink_fd = GlinkTransport::Get()->GetGlinkFd();

  /* Both KP and Glink drivers are implemented as packet
   * read unlike a byte read in UART. At a time these drivers
   * will fetch the entire packet and copy it to user buffers
   * and then deletes it entires.
   */
  size_t bytes_read = TEMP_FAILURE_RETRY(read(fd, buffer,
                         BTM_MAX_PKT_SIZE));
  if (bytes_read == -1) {
    if (fd == glink_fd && errno == ENETRESET && !is_adsp_triggered) {
      ALOGE("%s glink node is in reset state(%s)", __func__, strerror(errno));
      is_adsp_triggered = true;
    } else if(fd !=glink_fd) {
      ALOGE("%s kp node %d error (%s)", __func__, fd, strerror(errno));
    } else if (errno != ENETRESET) {
      ALOGE("%s glink node error state (%s)", __func__, strerror(errno));
    }
    return ;
  } else if (bytes_read > BTM_PKT_HEADER_LEN) {
    STREAM_TO_OPCODE(opcode, buffer);
    STREAM_TO_LENGTH(len, buffer + BTM_OPCODE_LEN);
    if (len > MAX_BUF_SIZE) {
      ALOGE("%s: received opcode :%04x with invalid len:%d", __func__, opcode, len);
      return;
    }
    ALOGI("%s: received opcode :%04x with len:%d", __func__, opcode, len);
    for (i = BTM_PKT_HEADER_LEN; i < (len  + BTM_PKT_HEADER_LEN); ++i) {
       ss <<  std::uppercase << std::hex << (int)buffer[i] << " ";
    }
    if (ss.str().length() > 0) {
      std::string params = "packet received:\n";
      params += ss.str();
      ALOGE("%s: %s", __func__, params.c_str());
    }
    if (opcode >= BTM_KP_CTRL_OFFSET) 
      decode_and_dispatch_kp_pkt(opcode, len, buffer + BTM_PKT_HEADER_LEN);
    else if (opcode >= BTM_CP_CTRL_OFFSET)
      decode_and_dispatch_cp_pkt(opcode, len, buffer + BTM_PKT_HEADER_LEN);
    else
      ALOGE("%s: opcode :%04x is not handled", __func__, opcode);
  } else {
     if (bytes_read < 0) {
       ALOGE("%s: Error occur while reading packet", __func__);
       return;
     }
     ALOGE("%s: complete packet is not sent, discarding it..", __func__);
     for (i = 0; i < bytes_read ; ++i) {
       ss <<  std::uppercase << std::hex << (int)buffer[i] << " ";
     }
     if (ss.str().length() > 0) {
      std::string params = "discarded packet payload rx:\n";
      params += ss.str();
      ALOGE("%s: %s", __func__,params.c_str());
    }
  }

  if (is_adsp_triggered && fd == glink_fd) {
    ALOGE("%s: Adsp is back online", __func__);
    is_adsp_triggered = false;
  }
}

void XMPacketizer::decode_and_dispatch_kp_pkt(uint32_t opcode,
		                              uint32_t len, uint8_t *buf)
{
  int i;
  uint8_t *cmd;
  switch (opcode) {
    case BTM_KP_CTRL_MASTER_CONFIG_REQ: {

      ALOGI("%s: Received master configuration request", __func__);
      CHECK(BTM_CTRL_MASTER_CONFIG_REQ_PKT_LEN ==
        (len + BTM_PKT_HEADER_LEN + 1));
      cmd = (uint8_t *)new uint8_t[BTM_CTRL_MASTER_CONFIG_REQ_PKT_LEN];
      UINT32_TO_STREAM(cmd, BTM_CP_CTRL_MASTER_CONFIG_REQ);
      UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len + 1);
      for (i = BTM_PKT_HEADER_LEN; i <(len +BTM_PKT_HEADER_LEN); i++)
        cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];
      uint8_t codec_id = (uint8_t)buf[12];
      uint32_t sample_rate = buf[5] | (buf[6] << 8) | (buf[7] << 16) | (buf[8] << 24);
      uint8_t stream_id = buf[0];
      conn_proxy_cop_ver_t cop_ver = conn_proxy_transport_mngr_get_cop_ver(codec_id, sample_rate, stream_id);
      cmd[i] = cop_ver;
      ALOGI("COP VERSION: %d", cop_ver);

      log_pkt(BTM_KP_CTRL_MASTER_CONFIG_REQ, cmd);
      std::stringstream ss;
      for (i = 0; i < BTM_CTRL_MASTER_CONFIG_REQ_PKT_LEN; i++)
         ss <<  std::uppercase << std::hex << (int)cmd[i] << " ";

      if (ss.str().length() > 0) {
        std::string params = "sending packet: ";
        params += ss.str();
        ALOGE("%s: %s", __func__, params.c_str());
      }
      GlinkTransport::Get()->WritetoGlinkCC(cmd,
                    BTM_CTRL_MASTER_CONFIG_REQ_PKT_LEN);
      delete []cmd;
      break;      
    } case BTM_KP_CTRL_MASTER_SHUTDOWN_REQ: {
      ALOGI("%s: Received master shutdown request", __func__);
      CHECK(BTM_CTRL_MASTER_SHUTDOWN_REQ_PKT_LEN == (len + BTM_PKT_HEADER_LEN));

      cmd = (uint8_t *)new uint8_t[len + BTM_PKT_HEADER_LEN];
      UINT32_TO_STREAM(cmd, BTM_CP_CTRL_MASTER_SHUTDOWN_REQ);
      UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len);

      for (i = BTM_PKT_HEADER_LEN; i < (len +BTM_PKT_HEADER_LEN); i++)
        cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];

      log_pkt(BTM_KP_CTRL_MASTER_SHUTDOWN_REQ, cmd);
      GlinkTransport::Get()->WritetoGlinkCC(cmd,
                     BTM_CTRL_MASTER_SHUTDOWN_REQ_PKT_LEN);
      delete []cmd;
      break;      
  } case BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP: {
    ALOGI("%s: Received prepare audio bearer switch rsp from kp", __func__);
    CHECK(BTM_CTRL_PREPARE_AUDIO_BEARER_SWITCH_RSP_LEN == (len + BTM_PKT_HEADER_LEN));
    /* Post message to main thread */
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
    msg->KpAudioBearerRsp.eventId = KP_XM_PREPARE_AUDIO_BEARER_RSP;
    msg->KpAudioBearerRsp.current_transport = buf[BTM_PKT_HEADER_LEN];
    msg->KpAudioBearerRsp.status = buf[BTM_PKT_HEADER_LEN + 1];
    XpanManager::Get()->PostMessage(msg);
    break;
  } case BTM_BTFMCODEC_CONFIG_DMA_REQ: {
    CHECK(BTM_BTFMCODEC_CONFIG_DMA_REQ_LEN == (len + BTM_PKT_HEADER_LEN + 1));
    ALOGI("%s: Received Config DMA Req", __func__);

    cmd = (uint8_t *)new uint8_t[BTM_BTFMCODEC_CONFIG_DMA_REQ_LEN];
    UINT32_TO_STREAM(cmd, BTM_CP_CTRL_CODEC_DMA_REQ);
    UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len + 1);
    for (i = BTM_PKT_HEADER_LEN; i < (len +BTM_PKT_HEADER_LEN); i++)
      cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];

    uint8_t codec_id = (uint8_t)buf[7];
    uint32_t sample_rate = buf[1] | (buf[2] << 8) | (buf[3] << 16) | (buf[4] << 24);
    uint8_t stream_id = buf[0];
    conn_proxy_cop_ver_t cop_ver = conn_proxy_transport_mngr_get_cop_ver(codec_id, sample_rate, stream_id);
    cmd[i] = cop_ver;
    ALOGI("COP VERSION: %d", cop_ver);
    log_pkt(BTM_BTFMCODEC_CONFIG_DMA_REQ, cmd);
    std::stringstream ss;
    for (i = 0; i < BTM_BTFMCODEC_CONFIG_DMA_REQ_LEN; i++)
      ss <<  std::uppercase << std::hex << (int)cmd[i] << " ";

    if (ss.str().length() > 0) {
      std::string params = "sending packet: ";
      params += ss.str();
      ALOGE("%s: %s", __func__, params.c_str());
    }
    GlinkTransport::Get()->WritetoGlinkCC(cmd,
                    BTM_BTFMCODEC_CONFIG_DMA_REQ_LEN);
    delete []cmd;
    break;
  } case BTM_BTFMCODEC_ADSP_STATE_IND: {
    uint32_t action;
    CHECK(BTM_KP_ADSP_STATE_IND_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT32(action, buf);
    ALOGI("%s: Received ADSP state indications with action id:%u", __func__, action);
    if (action == BTM_ADSP_SSR_AFTER_POWERUP) {
      xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
      msg->AdspState.eventId = KP_XM_ADSP_STATE_IND;
      msg->AdspState.action = action;
      XpanManager::Get()->PostMessage(msg);
    } else {
      ALOGI("%s unhandled action id %u", __func__, action);
    }
    break;
  } case BTM_BTFMCODEC_USECASE_START_IND: {
    uint8_t transport;
    CHECK(BTM_USECASE_START_LEN == (len + BTM_PKT_HEADER_LEN));
    STREAM_TO_UINT8(transport, buf);
    ALOGI("%s: Received usecase start indication message %d",
          __func__, transport);
    if (transport == 2) {
      xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
      msg->KpInd.eventId = KP_XM_USECASE_STATE_IND;
      msg->KpInd.transport = transport;
      XpanManager::Get()->PostMessage(msg);
    } else {
      ALOGW("%s: unhandled usecase start ind", __func__);
    }
    break;
  } default:
    ALOGE("%s: Opcode %0008x not matching", __func__, opcode);
  }
}

conn_proxy_cop_ver_t XMPacketizer::conn_proxy_transport_mngr_get_cop_ver(uint8_t codec_id, uint32_t sample_rate, uint8_t stream_id) {
  conn_proxy_cop_ver_t cop_ver = CONN_PROXY_COP_INVALID;
  if (stream_id == SOURCE_ID) {
    cop_ver = conn_proxy_transport_mngr_src_get_cop_ver(codec_id, sample_rate);
  }
  else if (stream_id == SINK_ID) {
    cop_ver = conn_proxy_transport_mngr_sink_get_cop_ver(codec_id, sample_rate);
  }
  return cop_ver;
}

conn_proxy_cop_ver_t XMPacketizer::conn_proxy_transport_mngr_sink_get_cop_ver(uint8_t codec_id, uint32_t sample_rate) {
  conn_proxy_cop_ver_t cop_ver = CONN_PROXY_COP_INVALID;
  std::shared_ptr<XpanManager> xm = XpanManager::Get();
  uint8_t *cop_ver_info = nullptr;
  cop_ver_info = xm->GetCopVersionSupported();

  if (cop_ver_info == nullptr) {
    ALOGE("cop version is null fallback to default");
    return CONN_PROXY_COP_INVALID;
  }
  ALOGI("Sample rate: %d", sample_rate);
  ALOGI("codec id: %d", codec_id);
  for (int i = 0; i<6;i++) {
    ALOGI("%d", cop_ver_info[i]);
  }

  switch (codec_id) {
    case CODEC_ID_SBC:
    case CODEC_ID_AAC:
    case CODEC_ID_LDAC:
    case CODEC_ID_APTX:
    case CODEC_ID_APTX_HD:
    case CODEC_ID_SSC:
    case CODEC_ID_LHDC:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[A2DP_SRC_DEFAULT];
      break;
    }
    case CODEC_ID_APTX_AD:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[A2DP_SRC_APTXAA_LHDC];
      break;
    }
    case CODEC_ID_LC3:
    case CODEC_ID_LC3_SPEECH:
    case CODEC_ID_APTX_R3:
    case CODEC_ID_APTX_R4:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[ISO];
      break;
    }
    case CODEC_ID_NO_CODEC:
    {
      if ((sample_rate == 8000 ) ||
          (sample_rate == 16000 ))
      {
        ALOGI("CODEC_ID_NO_CODEC (SINK) AND SAMPLE RATE 8000 | 16000");
        return CONN_PROXY_COP_NOT_USED;
      }
      else {
        cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[eSCO_SWB];
      }
      break;
    }

    case CODEC_ID_APTX_AD_SPEECH:
    case CODEC_ID_RVP:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[eSCO_SWB];
      break;
    }
  }

  xm->FreeCopVersionSupported();

  return cop_ver;
}

conn_proxy_cop_ver_t XMPacketizer::conn_proxy_transport_mngr_src_get_cop_ver(uint8_t codec_id, uint32_t sample_rate) {
  conn_proxy_cop_ver_t cop_ver = CONN_PROXY_COP_INVALID;
  std::shared_ptr<XpanManager> xm = XpanManager::Get();
  uint8_t *cop_ver_info = nullptr;
  cop_ver_info = xm->GetCopVersionInUse();

  if (cop_ver_info == nullptr) {
    ALOGE("cop version is null fallback to default");
    return CONN_PROXY_COP_INVALID;
  }
  ALOGI("Sample rate: %d", sample_rate);
  ALOGI("codec id: %d", codec_id);

    for (int i = 0; i<6;i++) {
    ALOGI("%d", cop_ver_info[i]);
  }

  switch (codec_id) {
    case CODEC_ID_AAC:
    case CODEC_ID_LDAC:
    case CODEC_ID_SSC:
    case CODEC_ID_LHDC:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[A2DP_SRC_ABR];
      break;
    }
    case CODEC_ID_APTX_AD:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[A2DP_SRC_APTXAA_LHDC];
      break;
    }
    case CODEC_ID_LC3:
    case CODEC_ID_LC3_SPEECH:
    case CODEC_ID_APTX_R3:
    case CODEC_ID_APTX_R4:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[ISO];
      break;
    }
    case CODEC_ID_NO_CODEC:
    {
      if ((sample_rate == 8000 ) ||
          (sample_rate == 16000 ))
      {
        ALOGI("CODEC_ID_NO_CODEC (SOURCE) AND SAMPLE RATE 8000 | 16000");
        return CONN_PROXY_COP_NOT_USED;
      }
      else {
        cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[eSCO_SWB];
      }
      break;
    }

    case CODEC_ID_APTX_AD_SPEECH:
    case CODEC_ID_RVP:
    {
      cop_ver = (conn_proxy_cop_ver_t) cop_ver_info[eSCO_SWB];
      break;
    }
  }

  xm->FreeCopVersionInUse();

  return cop_ver;
}


void XMPacketizer::log_pkt(uint32_t rx_opcode, uint8_t *cmd)
{
  uint32_t opcode;
  uint32_t len;

  STREAM_TO_UINT32(opcode, cmd);
  STREAM_TO_UINT32(len, cmd);
  switch(rx_opcode) {
    case BTM_KP_CTRL_MASTER_CONFIG_REQ: {
      uint8_t stream_id, bit_width, num_channels, channel_num, codec_id;
      uint32_t device_id, sample_rate;
      STREAM_TO_UINT8(stream_id, cmd);
      STREAM_TO_UINT32(device_id, cmd);
      STREAM_TO_UINT32(sample_rate, cmd);
      STREAM_TO_UINT8(bit_width, cmd);
      STREAM_TO_UINT8(num_channels, cmd);
      STREAM_TO_UINT8(channel_num, cmd);
      STREAM_TO_UINT8(codec_id, cmd);
      ALOGD("KP-->CP opcode:%08x len:%08x stream_id:%02x device_id:%08x", opcode,
         len, stream_id, device_id);

      ALOGD("KP-->CP sample_rate:%08x num_channels:%02x channel_num:%02x codec_id:%02x",
         sample_rate, num_channels, channel_num, codec_id);
      break;
    } case BTM_KP_CTRL_MASTER_SHUTDOWN_REQ: {
      uint8_t stream_id;
      STREAM_TO_UINT8(stream_id, cmd);
      ALOGD("KP-->CP opcode:%04x len:%02x stream_id:%02x", opcode, len, stream_id);
      break;
    } case BTM_CP_CTRL_CODEC_DMA_RSP : {
      uint8_t stream_id;
      STREAM_TO_UINT8(stream_id, cmd);
      ALOGD("BTFMCODEC-->CP opcode:%04x len:%02x stream_id:%02x", opcode, len, stream_id);
      break;
    } case BTM_BTFMCODEC_CONFIG_DMA_REQ : {
      uint8_t stream_id, bit_width, num_channels, codec_id, lpaif, inf_idx, active_channel_mask, cop_ver;
      uint32_t sample_rate;

      STREAM_TO_UINT8(stream_id, cmd);
      STREAM_TO_UINT32(sample_rate, cmd);
      STREAM_TO_UINT8(bit_width, cmd);
      STREAM_TO_UINT8(num_channels, cmd);
      STREAM_TO_UINT8(codec_id, cmd);
      STREAM_TO_UINT8(lpaif, cmd);
      STREAM_TO_UINT8(inf_idx, cmd);
      STREAM_TO_UINT8(active_channel_mask, cmd);
      STREAM_TO_UINT8(cop_ver, cmd);

      ALOGD("BTFMCODEC-->CP opcode:%08x len:%08x stream_id:%02x sample_rate:%08x", opcode,
         len, stream_id, sample_rate);
      ALOGD("BTFMCODEC-->CP bit_width:%02x num_channels:%02x codec_id:%02x lpaif:%02x",
         bit_width, num_channels, codec_id, lpaif);
      ALOGD("BTFMCODEC-->CP inf_idx %02x active_channel_mask %02x cop_ver %02x",
        inf_idx, active_channel_mask, cop_ver);
      break;
    } case BTM_CP_CTRL_MASTER_CONFIG_RSP: {
      uint8_t stream_id, status;
      STREAM_TO_UINT8(stream_id, cmd);
      STREAM_TO_UINT8(status, cmd);
      ALOGD("CP-->KP opcode:%04x len:%02x stream_id:%02x status:%02x", opcode,
         len, stream_id, status);
      break;
    } case BTM_CP_CTRL_MASTER_SHUTDOWN_RSP: {
      uint8_t stream_id, status;
      STREAM_TO_UINT8(stream_id, cmd);
      STREAM_TO_UINT8(status, cmd);
      ALOGD("CP-->KP opcode:%04x len:%02x stream_id:%02x status:%02x", opcode,
            len, stream_id, status);
      break;
    } default: {
      ALOGE("%s: this opcode is not handled:%04x", __func__, rx_opcode);
      break;
    }
  }
}

void XMPacketizer::decode_and_dispatch(uint32_t opcode, uint32_t len,
                               uint8_t *buf)
{
  int i;
  uint8_t *cmd;
  std::stringstream ss;
  uint8_t end_point;

  cmd = (uint8_t *)new uint8_t[len + BTM_PKT_HEADER_LEN];

  UINT32_TO_STREAM(cmd, opcode);
  UINT32_TO_STREAM(cmd + BTM_OPCODE_LEN, len);

  if (opcode >= BTM_CP_CTRL_OFFSET && opcode <= BTM_CP_CTRL_MAX_OFFSET) {
    end_point = 1;
    ALOGI("%s:%s", __func__, OpcodeToString(opcode));
  } else if (opcode >= BTM_KP_CTRL_OFFSET && opcode <= BTM_KP_CTRL_MAX_OFFSET) {
    end_point = 2;
    ALOGI("%s:%s", __func__, OpcodeToString(opcode));
  } else {
    ALOGE("%s: incorrect message :%s dropping here with %lu", __func__,
      OpcodeToString(opcode), opcode);
    return;
  }

  for (i = BTM_PKT_HEADER_LEN; i < (len + BTM_PKT_HEADER_LEN); i++)
    cmd[i]= (uint8_t)buf[i - BTM_PKT_HEADER_LEN];

  for (i = 0; i < len + BTM_PKT_HEADER_LEN; i++)
    ss <<  std::uppercase << std::hex << (int)cmd[i] << " ";

  if (ss.str().length() > 0) {
    std::string params = "sending packet:\n";
    params += ss.str();
    ALOGE("%s: %s", __func__, params.c_str());
  }

  if(end_point == 1)
    GlinkTransport::Get()->WritetoGlinkCC(cmd, len+ BTM_PKT_HEADER_LEN);
  else
   KernelProxyTransport::Get()->WritetoKpTransport(cmd, len + BTM_PKT_HEADER_LEN);

  delete []cmd;
}

void XMPacketizer::KpBearerSwitchInd(RspStatus status)
{
  uint8_t cmd;
  cmd = (uint8_t)(status & 0x000000FF);
  decode_and_dispatch(BTM_KP_CTRL_BEARER_SWITCH_IND, 1, &cmd);
}

void XMPacketizer::CpBearerSwitchInd(TransportType type, RspStatus status)
{
  uint8_t cmd[2];
  cmd[0] = (uint8_t) (type & 0x000000FF);
  cmd[1] = (uint8_t) (status & 0x000000FF);
  decode_and_dispatch(BTM_CP_CTRL_BEARER_SWITCH_IND, 2, cmd);
}

void XMPacketizer::BearerSwitchRsp(TransportType type, RspStatus status)
{
  uint8_t cmd[2];
  cmd[0] = (uint8_t) (type & 0x000000FF);
  cmd[1] = (uint8_t) (status & 0x000000FF);

  decode_and_dispatch(BTM_CP_CTRL_BEARER_SWITCH_RSP, 2, cmd);
}

void XMPacketizer::PrepareAudioBearerReqtoCp(TransportType type)
{
  uint8_t cmd = (uint8_t)(type & 0x000000FF);
  decode_and_dispatch(BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ, 1, &cmd);

}

void XMPacketizer::BearerPreferenceInd(TransportType type)
{
  uint8_t cmd;
  cmd = (uint8_t)(type & 0x000000FF);
  decode_and_dispatch(BTM_CP_CTRL_BEARER_PREFERENCE_IND, 1, &cmd);
}

void XMPacketizer::PrepareAudioBearerReqtoKp(TransportType type)
{
  /* updating received transport values to KP
     BR_EDR, BT_LE -> BT = 1
     XPAN_AP, XPAN_P2P, XPAN_AP_PREP -> BTADV = 2
     NONE -> NONE = 3
  */
  uint8_t cmd = 0;

  if (type == BR_EDR || type == BT_LE)
    cmd = 1;
  else if (type == XPAN_AP || type == XPAN_P2P || type == XPAN_AP_PREP)
    cmd = 2;
  else if (type == NONE || type == DEFAULT)
    cmd = 3;

  ALOGI("%s: Transport type cmd to kp = %d ", __func__,cmd);
  decode_and_dispatch(BTM_KP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ, 1, &cmd);
}

void XMPacketizer::ProcessMessage(XmIpcEventId eventId, xm_ipc_msg_t *msg)
{
  switch (eventId) {
    case DH_XM_COP_VER_IND: {
      DhXmCopVerInd CoPInd = msg->CoPInd;
      decode_and_dispatch(BTM_CP_CTRL_COP_VER_IND, CoPInd.len, CoPInd.data);
      break;
    } case QHCI_XM_USECASE_UPDATE: {
      QhciXmUseCase UseCase = msg->UseCase;
      uint8_t cmd;
      cmd = (uint8_t)(UseCase.usecase & 0x000000FF);
      decode_and_dispatch(BTM_CP_CTRL_UPDATE_AUDIO_MODE_IND, 1, &cmd);
      break;
    } case QHCI_XM_PREPARE_AUDIO_BEARER_REQ: {
      QhciXmPrepareAudioBearerReq AudioBearerReq = msg->AudioBearerReq;
      uint8_t cmd;
      cmd = (uint8_t)(AudioBearerReq.type & 0x000000FF);
      decode_and_dispatch(BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ, 1, &cmd);
      PrepareAudioBearerReqtoKp(AudioBearerReq.type);
      break;
    } case QHCI_CP_ENCODER_LIMIT_UPDATE: {
      uint32_t len = (uint32_t)(msg->EncoderLimitParams.num_limit *
		      XM_ENCODER_LIMIT_PARAMS_SIZE + 1);
      uint8_t *cmd = (uint8_t*)new uint8_t[len];
      cmd[0] = (uint8_t) msg->EncoderLimitParams.num_limit;
      memcpy(&cmd[1], msg->EncoderLimitParams.data, (len - 1));
      decode_and_dispatch(BTM_CP_CTRL_ENCODER_LIMTS_IND, len, cmd);
      delete []cmd;
      free(msg->EncoderLimitParams.data);
      break;
    } case XP_XM_HOST_PARAMETERS: {
      uint8_t cmd[8];
      memcpy(cmd, &msg->HostParams.macaddr, sizeof(macaddr_t));
      cmd[6] = (uint8_t)(msg->HostParams.Ethertype & 0xFF);
      cmd[7] = (uint8_t)((msg->HostParams.Ethertype >> 8) & 0x00FF);
      decode_and_dispatch(BTM_CP_CTRL_HOST_PARAMETERS_IND,
                          (sizeof(macaddr_t)+ sizeof(uint16_t)), cmd); 
      break;
    } case XP_XM_TWT_SESSION_EST: {
      int i = 0;
      ALOGE("%s: XP_XM_TWT_SESSION_EST addr %p", __func__, msg->TwtParams.params);
      uint8_t num_devices = msg->TwtParams.num_devices;
      uint8_t len = BTM_XP_TWT_BASE_LEN + (num_devices * BTM_XP_TWT_PAYLOAD_LEN);
      uint8_t *cmd = (uint8_t*)new uint8_t[len];
      uint8_t count = 0;
      XPANTwtSessionParams *params =  msg->TwtParams.params;
      cmd[i++]= num_devices;
      for (; count < num_devices ; count++) {
        /* copy mac addr */
        int j = 0;
        for (; j < 6; j++)
          cmd[i+j] = params[count].mac_addr.b[j];
        i += j;

        UINT32_TO_STREAM(cmd + i, params[count].interval);
        i += sizeof(uint32_t);

        UINT32_TO_STREAM(cmd + i, params[count].peroid);
        i += sizeof(uint32_t);

        UINT32_TO_STREAM(cmd + i, params[count].location);
        i += sizeof(uint32_t);
      }
      ALOGE("%s: len %d and i %d", __func__, len, i);
      decode_and_dispatch(BTM_CP_CTRL_TWT_EST_IND, len , cmd); 
      delete []cmd;
      ALOGE("%s: freeing up the params", __func__);
      free(params);
      ALOGE("%s: freed ", __func__);
      break;
    } case XP_XM_BEARER_PREFERENCE_REQ: {
      uint8_t cmd;
      char value[PROPERTY_VALUE_MAX] = {'\0'};
      cmd = (uint8_t)(msg->BearerPreference.transport & 0x000000FF);
      decode_and_dispatch(BTM_CP_CTRL_BEARER_PREFERENCE_IND, 1, &cmd);
      break;
    } case QHCI_XM_UNPREPARE_AUDIO_BEARER_REQ: {
      QhciXmUnPrepareAudioBearerReq UnPrepareAudioBearerReq = msg->UnPrepareAudioBearerReq;
      uint8_t cmd;
      cmd = (uint8_t)(UnPrepareAudioBearerReq.type & 0x000000FF);
      decode_and_dispatch(BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ, 1, &cmd); 
      PrepareAudioBearerReqtoKp(UnPrepareAudioBearerReq.type);
      break;
    } case XM_CP_LOG_LVL: {
      XmLogLvl Loglvl = msg->Loglvl;
      decode_and_dispatch(BTM_CP_CTRL_DATA_LOG_MASK_IND, Loglvl.len, &Loglvl.data);
      break;
    } case XM_KP_LOG_LVL: {
      XmLogLvl Loglvl = msg->Loglvl;
      decode_and_dispatch(BTM_KP_CTRL_LOG_LVL_IND, Loglvl.len, &Loglvl.data);
      break;
    } case CP_XM_BEARER_SWITCH_REQ: {
      TransportType type = msg->BearerSwitchReq.transport;
      uint8_t cmd;
      cmd = (uint8_t)(type & 0x000000FF);
      decode_and_dispatch(BTM_CP_CTRL_PREPARE_AUDIO_BEARER_SWITCH_REQ, 1, &cmd);
      PrepareAudioBearerReqtoKp(type);
      break;
    } case XAC_CP_BURST_INT_REQ : {
      uint8_t cmd[10];
      memcpy(cmd, &msg->BurstIntReq.macaddr, sizeof(macaddr_t));
      cmd[6] = (uint8_t)(msg->BurstIntReq.bi_voice & 0xFF);
      cmd[7] = (uint8_t)((msg->BurstIntReq.bi_voice >> 8) & 0x00FF);

      cmd[8] = (uint8_t)(msg->BurstIntReq.bi_media & 0xFF);
      cmd[9] = (uint8_t)((msg->BurstIntReq.bi_media >> 8) & 0x00FF);
      decode_and_dispatch(BTM_CP_CTRL_BURST_INTERVAL_REQ,
                          (sizeof(macaddr_t)+ (2 * sizeof(uint16_t))), cmd); 
      break;
    } case XAC_CP_REMOTE_PARAMS_IND: {
      int num_devices = msg->CpRemoteParamsInd.num_devices;
      int type = msg->CpRemoteParamsInd.hs_ip_addr.type;
      int len = ((type == IPv4 ? BTM_XP_REMOTE_IPV4_PARAMS_LEN :
		             BTM_XP_REMOTE_IPV6_PARAMS_LEN) +
	                    (num_devices * (type == IPv4 ? BTM_XP_EBPARAMS_IPV4_PARAMS_LEN :
			     BTM_XP_EBPARAMS_IPV6_PARAMS_LEN)));

      uint8_t *cmd = (uint8_t*)new uint8_t[len];
      CpRemoteApParams *Ind = &msg->CpRemoteParamsInd;
      int i = 0, j = 0;
      ALOGI("%s: XAC_CP_REMOTE_PARAMS_IND total len : %d", __func__, len);

      UINT8_TO_STREAM(cmd + i, Ind->encryption);
      i += sizeof(uint8_t);

      for (; j < 16; j++)
          cmd[i+j] = Ind->psk[j];
      i += j;

      for (j = 0; j < 16; j++)
        cmd[i+j] = Ind->identity[j];
      i += j;


      for (j =0; j < 6; j++)
          cmd[i+j] = Ind->hs_ap_bssid.b[j];
      i += j;

      for (j =0; j < 6; j++)
          cmd[i+j] = Ind->hs_mac_addr.b[j];
      i += j;


      UINT8_TO_STREAM(cmd + i, type);
      i += sizeof(uint8_t);

      if (type == IPv4) {
        for (j = 0; j < IPv4_SIZE; j++)
          cmd[i+j] = Ind->hs_ip_addr.ipv4[j];
        i += IPv4_SIZE;
      } else if (type == IPv6) {
         for (j = 0; j < IPv6_SIZE; j++)
          cmd[i+j] = Ind->hs_ip_addr.ipv6[j];
        i += IPv6_SIZE;
      }

      UINT32_TO_STREAM(cmd + i, Ind->center_freq);
      i += sizeof(uint32_t);
      
      UINT16_TO_STREAM(cmd + i, Ind->remote_udp_port);
      i += sizeof(uint16_t);
      UINT16_TO_STREAM(cmd + i, Ind->rx_udp_port);
      i += sizeof(uint16_t);
      UINT16_TO_STREAM(cmd + i, Ind->time_sync_rx_port);
      i += sizeof(uint16_t);
      UINT16_TO_STREAM(cmd + i, Ind->time_sync_tx_port);
      i += sizeof(uint16_t);
      ALOGI("%s: i:%d line %d", __func__, i, __LINE__);

      UINT16_TO_STREAM(cmd + i, Ind->periodicity);
      i += sizeof(uint16_t);

      UINT8_TO_STREAM(cmd + i, num_devices);
      i += sizeof(uint8_t);
      ALOGI("%s: i:%d line %d", __func__, i, __LINE__);
      if (num_devices) {
        for (int k = 0; k < num_devices; k++) {
          for (j =0; j < 6; j++)
            cmd[i+j] = Ind->EbParams[k].eb_ap_bssid.b[j];
          i += j;

          for (j =0; j < 6; j++)
            cmd[i+j] = Ind->EbParams[k].eb_mac_addr.b[j];
          i += j;

	  UINT32_TO_STREAM(cmd + i, Ind->EbParams[k].eb_audio_loc);
	  i += sizeof(uint32_t);

          if (type == IPv4) {
            for (j = 0; j < IPv4_SIZE; j++)
              cmd[i+j] = Ind->EbParams[k].eb_ip_addr.ipv4[j];
            i += IPv4_SIZE;
          } else if (type == IPv6) {
            for (j = 0; j < IPv6_SIZE; j++)
              cmd[i+j] = Ind->EbParams[k].eb_ip_addr.ipv6[j];
             i += IPv6_SIZE;
          }

	  cmd [i] = (uint8_t)Ind->EbParams[k].role;
	  i += sizeof(uint8_t);
	  ALOGI("%s: i:%d line %d", __func__, i, __LINE__);
        }
      }

      ALOGE("%s: len %d and i %d", __func__, len, i);
      decode_and_dispatch(BTM_CP_CTRL_REMOTE_PARAMS_IND, len , cmd);
      delete []cmd;
      break;
    } default: {
      ALOGI("%s: this :%04x ipc message is not handled", __func__, eventId);
    }
  }
}

} // namespace implementation
} // namespace xpan
