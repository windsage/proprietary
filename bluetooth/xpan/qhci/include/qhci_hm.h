/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#ifndef QHCI_HM_H
#define QHCI_HM_H

#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <string>
#include "qhci_packetizer.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "qhci_xm_if.h"
#include "xpan_utils.h"


#define QHCI_XPAN_HANDLE_START 0x0040
#define QHCI_XPAN_HANDLE_END 0x0EFF

namespace xpan {
namespace implementation {

typedef struct {
  uint16_t stack_handle;
  uint16_t le_handle;
  uint16_t xpan_handle;
  bdaddr_t bdaddr;
} xpanDeviceList;

typedef struct {
  uint16_t stack_handle;
  uint16_t bredr_handle;
  bdaddr_t bdaddr;
} bredrDeviceList;

class QHciHm {
  public:
    static std::shared_ptr<QHciHm> GetIf();
    QHciHm();
    ~QHciHm();
    void DeInit();

    friend class QHci;

    uint16_t bredrConnectionCmplt(uint16_t handle, bdaddr_t bdaddr, uint8_t status);
    uint16_t leConnectionCmplt(uint16_t handle, bdaddr_t bdaddr, uint8_t status);
    uint16_t xpanConnectionCmplt(bdaddr_t bdaddr);
    uint16_t bredrDisconnectionCmplt(uint16_t handle);
    uint16_t leDisconnectionCmplt(uint16_t handle);
    uint16_t xpanDisconnectionCmplt(bdaddr_t bdaddr);
    uint16_t GetBredrHandleFromStackHandle(uint16_t stack_handle);
    uint16_t GetStackHandleFromBredrHandle(uint16_t stack_handle);
    uint16_t GetXpanHandleFromStackHandle(uint16_t stack_handle);
    uint16_t GetLeHandleFromStackHandle(uint16_t stack_handle);
    uint16_t GetStackHandleFromLeHandle(uint16_t le_handle);
    uint16_t GetStackHandleFromXPANHandle(uint16_t xpan_handle);
    uint16_t GetXpanHandleFromBdaddr(bdaddr_t bdaddr);
    uint16_t GetLeHandleFromBdaddr(bdaddr_t bdaddr);
    uint16_t GetBredrHandleFromBdaddr(bdaddr_t bdaddr);
    uint16_t GetSocHandleFromStackHandle(uint16_t stack_handle);
    uint16_t GetStackHandleFromSocHandle(uint16_t soc_handle);

  private:
    uint16_t HandleID;
    uint16_t generateHandle();
    bool CmpBDAddrs(bdaddr_t bd_addr1, bdaddr_t bd_addr2);

  private:
    static std::shared_ptr<QHciHm> qhci_hm_instance;
};

} // namespace implementation
} // namespace xpan
#endif

