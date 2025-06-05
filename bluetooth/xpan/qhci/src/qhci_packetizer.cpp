/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qhci_packetizer.h"
#include <string.h>
#include <thread>
#include <unistd.h>
#include <android-base/logging.h>
#include <errno.h>
#include <utils/Log.h>
#include <thread>
#include "qhci_main.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.qti@1.0-QHciPacketizer"

using ::xpan::implementation::QHciPacketizer;

namespace xpan {
namespace implementation {

QHciPacketizer::QHciPacketizer()
{
}

QHciPacketizer::~QHciPacketizer()
{
}

void QHciPacketizer::ProcessMessage(qhci_msg_t *msg) {
  QHciEventId eventId = msg->eventId;
  switch (eventId) {
    case QHCI_QLL_CMD_REQ: {
      ALOGI("%s: QHCI_QLL_CMD_REQ ", __func__);
      //free(msg);
      break;
    } default:
      ALOGI("%s: this :%04x message is not handled", __func__, eventId);
  }
}

QHCI_BT_HDR* QHciPacketizer::QHciMakePacket(size_t data_size) {
  QHCI_BT_HDR* ret = (QHCI_BT_HDR*) malloc(sizeof(QHCI_BT_HDR) + data_size);

  ret->qhci_event = 0;
  ret->qhci_offset = 0;
  ret->qhci_layer_specific = 0;
  ret->qhci_len = data_size;

  return ret;
}

QHCI_BT_HDR* QHciPacketizer::QHciMakeCommand(uint16_t opcode, size_t parameter_size,
                                      uint8_t** stream_out) {
  QHCI_BT_HDR* packet = QHciMakePacket(QHCI_COMMAND_PREAMBLE_SIZE + parameter_size);

  uint8_t* stream = packet->qhci_data;

  //convert below into macros
  *(stream)++ = (uint8_t)(opcode);
  *(stream)++ = (uint8_t)((opcode) >> 8);

  *(stream)++ = (uint8_t)(parameter_size);
  
  if (stream_out != NULL) *stream_out = stream;

  return packet;
}

QHCI_BT_HDR* QHciPacketizer::QHciMakeQbceSetHostFeature(uint8_t bit_position,
                                                                   uint8_t bit_value) {
  uint8_t* stream;
  const uint8_t parameter_size = 3;
  QHCI_BT_HDR* packet = QHciMakeCommand(QHCI_VS_QBCE, parameter_size, &stream);

  //convert below into macros
  *(stream)++ = (uint8_t)QBCE_QHCI_QLE_SET_HOST_FEATURE;
  *(stream)++ = (uint8_t)bit_position;
  *(stream)++ = (uint8_t)bit_value;

  return packet;
}

} // namespace implementation
} // namespace xpan
