/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#ifndef XM_XPROFILE_IF_H
#define XM_XPROFILE_IF_H

#pragma once

#include <stdint.h>
#include "xpan_utils.h"
#include "xm_main.h"
#include "xm_ipc_if.h"

#ifdef XPAN_ENABLED
#include "xpan_provider_if.h"
#endif

namespace xpan {
namespace implementation {
using namespace std;
class XMXprofileIf
{
  public:
    XMXprofileIf();
    ~XMXprofileIf();
    void ProcessMessage(XmIpcEventId, xm_ipc_msg_t *);
    bool XmXpBearerSwitchInd(bdaddr_t, TransportType, uint8_t);
    void Initialize(void);
    void Deinitialize(void);
    bool BearerPreferenceRsp(bdaddr_t, TransportType, RspStatus, uint8_t);
    bool TransportUpdate(bdaddr_t, TransportType);
    bool XmXpPrepareAudioBearerReq(bdaddr_t, TransportType);
    bool GetTwtPropStatus(void);
    void ReUpdateTwtSessionParams(TwtParameters *);
    bool UseCaseUpdate(UseCaseType);
    bool WifiTransportPreferenceReq(uint8_t);

  private:
    static void TransportEnabled(bdaddr_t, TransportType, bool, uint8_t);
    bool RemoteSupportsXpan(bdaddr_t, bool);
    static void XpXmPrepareAudioBearerRsp(bdaddr_t, uint8_t, uint8_t);
    static void XpXmBearerSwitchInd(bdaddr_t, uint8_t, uint8_t);
    bool WiFiAcsResults(xm_ipc_msg_t *);
    bool WiFiSsrEventInd(xm_ipc_msg_t *);
    static void HostParameters(macaddr_t, uint16_t);
#ifdef XPAN_ENABLED
    static void UpdateTWTSessionParams(uint32_t, uint8_t, uint8_t,
                    std::vector<tXPAN_Twt_Session_Params>);
#endif
    static void XpBearerPreferenceReq(bdaddr_t, uint8_t, uint8_t);
    bool WifiTwtEvent(xm_ipc_msg_t *);
    static void UpdateXpanBondedDevices(uint8_t, bdaddr_t *);
    static void XpSapPowerSave(uint8_t, uint8_t);
    bool WifiSapPowerSaveEvent(xm_ipc_msg_t *);
    static void SapState(uint16_t);
    static void CreateSapInterface(uint8_t);
    static void EnableAcs(std::vector<uint32_t>);
    bool UpdateSapInterface(xm_ipc_msg_t *);
    bool TriggerMdnsQuery(bdaddr_t, bool);
    static void MdnsDiscoveryStatus(bdaddr_t, uint8_t, mdns_uuid_t, uint8_t);
    bool StartFilteredScan(bdaddr_t);
    static void ConnectLeLinkReq(bdaddr_t);
    bool ConnectLeLinkRsp(bdaddr_t, uint8_t);
    static void UpdateRemoteApDetails(tXPAN_Remote_Params);
    static void GetHandSetPortNumberReq(void);
    bool HandSetPortNumberRsp(int, int, int);
    bool RegisterMdnsService(bdaddr_t);
    bool OnCurrentTransportUpdated(bdaddr_t, TransportType);
    static void BondStateUpdate(bdaddr_t, uint8_t);
    static void UpdateLocalApDetails(tXPAN_Local_Params);
    static void WifiTransportPreferenceRsp(uint8_t, uint8_t);
    static void SetApAvailableReq(bdaddr_t, uint32_t);
    static void CancelApAvailableReq(bdaddr_t);
    bool WifiAPAvbRsp(bdaddr_t, uint8_t);
    static void UpdateConnectedEbDetails(uint8_t, macaddr_t, macaddr_t);
    bool ChannelSwitchStarted(xm_ipc_msg_t *);
    static void UpdateWifiScanStarted(bdaddr_t, uint8_t);
};

} // namespace implementation
} // namespace xpan

#endif
