/*
 *  Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef XM_XAC_IF_H
#define XM_XAC_IF_H

#pragma once

#include <stdint.h>
#include "xpan_utils.h"
#include "xm_main.h"
#include "xm_ipc_if.h"

namespace xpan {
namespace implementation {

class XMXacIf
{
 public:
   XMXacIf();
   ~XMXacIf();
   static XMXacIf *GetIf(void);
   bool Initialize(void);
   void Deinitialize(void);
   void ProcessMessage(XmIpcEventId, xm_ipc_msg_t *);
   bool TriggerMdnsQuery(bdaddr_t, bool);
   bool BurstIntervalReq(macaddr_t, uint16_t, uint16_t);
   bool PrepareAudioBearerReq(bdaddr_t, TransportType);
   bool BearerPreferenceReq(bdaddr_t, TransportType);
   bool InitiateLmpBearerSwitch(bdaddr_t, TransportType);
   bool BearerSwitchInd(bdaddr_t, TransportType, RspStatus);
   bool UpdateApDetails(uint8_t);
   bool UpdateRemoteEbDetails(bool encryption, uint8_t *psk, uint8_t *identity,
                            macaddr_t hs_ap_bssid, macaddr_t hs_mac_addr,
                            ipaddr_t hs_ip_addr,
                            uint32_t center_freq,
                            uint16_t time_sync_tx_port,
                            uint16_t time_sync_rx_port,
                            uint16_t remote_udp_port,
                            uint16_t rx_udp_port,
			    std::vector<RemoteEbParams> EbParams);
   bool PrepareAudioBearerRsp(bdaddr_t, RspStatus);
   bool BearerPreferenceRsp(bdaddr_t, TransportType, RspStatus);
   bool RegisterMdnsService(bdaddr_t);
   bool OnCurrentTransportUpdated(bdaddr_t, TransportType);
   bool HandSetPortNumberRsp(int, int, int);
   bool UpdateHsApBssidChanged (macaddr_t);
   bool StartFilteredScan(bdaddr_t);
 private:
   bool UpdateXpanBondedDevices(uint8_t, bdaddr_t *);
   bool MdnsDiscoveryStatus(bdaddr_t, uint8_t , mdns_uuid_t, uint8_t);
   bool UpdateRemoteApDetails(tXPAN_Remote_Params);
   bool GetHandSetPortNumberReq(void);
   bool BondStateUpdate(bdaddr_t, BondState);
   bool UpdateLocalApDetails(macaddr_t, macaddr_t, ipaddr_t, mdns_uuid_t, uint32_t);
   static XMXacIf *instance_;
};
} // namespace implementation
} // namespace xpan

#endif //XM_XAC_IF_H
