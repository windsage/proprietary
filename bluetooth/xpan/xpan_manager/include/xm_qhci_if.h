/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#ifndef XM_QHCI_IF_H
#define XM_QHCI_IF_H

#pragma once

#include <stdint.h>
#include "xpan_utils.h"
#include "xm_main.h"
#include "xm_ipc_if.h"

namespace xpan {
namespace implementation {

class XMQhciIf
{
 public:
  XMQhciIf();
  ~XMQhciIf();
 bool RemoteSupportsXpan(bdaddr_t, bool);
 bool PrepareAudioBearerReq(bdaddr_t, TransportType,
		 ApiDirection dir = QHCI_To_XM);
 bool UseCaseUpdate(UseCaseType);
 bool UnPrepareAudioBearerReq(bdaddr_t, TransportType);
 bool PrepareAudioBearerRsp(bdaddr_t, RspStatus,
		 ApiDirection dir = QHCI_To_XM);
 bool UnPrepareAudioBearerRsp(bdaddr_t, RspStatus);
 void ProcessMessage(XmIpcEventId, xm_ipc_msg_t *);
 bool BearerSwitchInd(bdaddr_t, TransportType, uint8_t);
 bool DelayReporting(xm_ipc_msg_t * msg);
 bool TransportUpdate(xm_ipc_msg_t * msg);
 bool EncoderLimitUpdate(uint8_t, uint8_t *);
 bool AudioTransportUpdate(bdaddr_t, TransportType);
 UseCaseType GetCurrentUsecase(void);
 bool ConnectLeLinkRsp(bdaddr_t, RspStatus);
 private:
 bool XpTransportEnabled(bdaddr_t, TransportType, bool, uint8_t);
 bool UpdateXpanBondedDevices(uint8_t, bdaddr_t *);
 bool ConnectLeLinkReq(bdaddr_t);
 bool UpdateWifiScanStarted(bdaddr_t, uint8_t);
};
} // namespace implementation
} // namespace xpan

#endif
