/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once
#ifndef XPAN_AC_XM_IF
#define XPAN_AC_XM_IF

#include <stdint.h>
#include "xpan_utils.h"
#include "xpan_ac_int.h"

namespace xpan {
namespace ac {

class XpanAcXmIf {
 private:
  XpanAcXmIf();
  ~XpanAcXmIf();

  static XpanAcXmIf *sInstance;

 public:
  static XpanAcXmIf* GetIf();
  bool Initialize();
  bool Deinitialize();
  bool PrepareBearer (bdaddr_t addr, TransportType bearer);
  bool BearerPreference (bdaddr_t addr, TransportType bearer);
  bool UpdateRemoteApParams (tXPAN_Remote_Params params);
  bool InitiateLmpBearerSwitch (bdaddr_t addr, TransportType bearer);
  bool RemoteDisconnectedEvent (bdaddr_t addr, XpanEarbudRole role);
  bool BearerSwitchInd (bdaddr_t addr, TransportType bearer, uint8_t status);
  bool MdnsDiscoveryStatus (bdaddr_t addr, uint8_t status, mdns_uuid_t , uint8_t);
  bool UpdateBondState (bdaddr_t addr, BondState state);
  bool UpdateLocalApDetails (macaddr_t mac, macaddr_t bssid,
                             ipaddr_t ipAddr, mdns_uuid_t uuid, uint32_t freq);
  bool GetLocalListeningPorts ();
  bool UpdateBondedXpanDevices (uint8_t numOfDevices, bdaddr_t devices[]);
};

} // namespace ac
} // namespace xpan

#endif