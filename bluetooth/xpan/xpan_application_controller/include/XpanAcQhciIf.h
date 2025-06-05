/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once
#ifndef XPAN_AC_QHCI_IF
#define XPAN_AC_QHCI_IF

#include <stdint.h>
#include "xpan_utils.h"
#include "xpan_ac_int.h"

namespace xpan {
namespace ac {

class XpanAcQhciIf {
 private:
  XpanAcQhciIf();
  ~XpanAcQhciIf();

  static XpanAcQhciIf *sInstance;

 public:
  static XpanAcQhciIf* GetIf();
  static bool Deinitialize();
  bool UpdateLocalVersion(uint8_t version, uint16_t companyId, uint16_t subversion);
  bool UpdateLocalLeFeatures(uint64_t le_features);
  bool CreateConnection (bdaddr_t addr, uint16_t supervision_timeout);
  bool CreateConnectionCancel (bdaddr_t addr);
  bool DisconnectConnection (bdaddr_t addr);
  bool GetRemoteVersion (bdaddr_t addr);
  bool GetRemoteLeFeatures (bdaddr_t addr);
  bool EnableEncrption (bdaddr_t addr, const std::vector<uint8_t>& ltk);
  bool SendAclData (bdaddr_t addr, uint8_t llid, const std::vector<uint8_t>& data);
  bool L2capPauseUnpauseRes (bdaddr_t addr, uint8_t act, uint8_t status);
};

} // namespace ac
} // namespace xpan

#endif