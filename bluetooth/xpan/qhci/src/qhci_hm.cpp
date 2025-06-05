/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <signal.h>
#include "data_handler.h"
#include "qhci_main.h"
#include "qhci_packetizer.h"
#include "qhci_xm_if.h"
#include "qhci_hm.h"
#include "xpan_utils.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
//#include "logger.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_hm"

using ::xpan::implementation::QHci;

namespace xpan {
namespace implementation {

std::vector<xpanDeviceList> xpan_map;
std::vector<bredrDeviceList> bredr_map;

std::shared_ptr<QHciHm> QHciHm::qhci_hm_instance = nullptr;

QHciHm::QHciHm() {
  ALOGD("%s", __func__);
  HandleID = QHCI_XPAN_HANDLE_START;
}

QHciHm::~QHciHm() {
  xpan_map.clear();
  bredr_map.clear();
  ALOGD("%s ", __func__);
}

std::shared_ptr<QHciHm> QHciHm::GetIf() {
  if (!qhci_hm_instance)
    qhci_hm_instance.reset(new QHciHm());
  return qhci_hm_instance;
}

void QHciHm::DeInit() {
  ALOGD("%s", __func__);

  if (qhci_hm_instance) {
    qhci_hm_instance.reset();
    qhci_hm_instance = NULL;
  }
}

uint16_t QHciHm::generateHandle() {
  if (HandleID < QHCI_XPAN_HANDLE_START || HandleID > QHCI_XPAN_HANDLE_END)
  {
    ALOGW("%s: handle 0x%4x not in valid handle range ", __func__, HandleID);
    return 0x00;
  }

  uint16_t new_handle = HandleID;
  HandleID++;

  if (HandleID == QHCI_XPAN_HANDLE_END) {
    ALOGW("%s: handle 0x%4x reached max value of range, reset to start ", __func__, HandleID);
    HandleID = QHCI_XPAN_HANDLE_START;
  }
  return new_handle;
}

bool QHciHm::CmpBDAddrs(bdaddr_t bd_addr1, bdaddr_t bd_addr2) {
  ALOGD("%s Addr %s Addr2  %s", __func__,
         ConvertRawBdaddress(bd_addr1), ConvertRawBdaddress(bd_addr2));

  for (int i = 0; i < 6; i++) {
    if (bd_addr1.b[i] != bd_addr2.b[i]) return false;
  }
  return true;
}

uint16_t QHciHm::bredrConnectionCmplt(uint16_t handle, bdaddr_t bdaddr, uint8_t status) {
  uint16_t new_handle = handle;
  bredrDeviceList new_device;

  for (auto it = xpan_map.begin(); it != xpan_map.end();it++) {
    if (it->stack_handle == handle) {
      new_handle = generateHandle();
      break;
    }
  }

  if (status != HCI_SUCCESS)
    return new_handle;

  ALOGD("%s new device added Addr %s  bredr_handle 0x%04x, stack_handle 0x%04x", __func__,
         ConvertRawBdaddress(bdaddr),handle, new_handle);

  new_device.bdaddr = bdaddr;
  new_device.bredr_handle = handle;
  new_device.stack_handle = new_handle;
  bredr_map.push_back(new_device);
  return new_handle;
}

uint16_t QHciHm::leConnectionCmplt(uint16_t handle, bdaddr_t bdaddr, uint8_t status) {
  uint16_t new_handle = handle;
  xpanDeviceList new_device;

  ALOGD("%s new LE device Addr %s  le_handle 0x%04x", __func__,
         ConvertRawBdaddress(bdaddr),handle);
  new_device.bdaddr = bdaddr;

  for (auto it = xpan_map.begin();it != xpan_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      it->le_handle = handle;
      return it->stack_handle;
    } else if (it->stack_handle == handle) {
      new_handle = generateHandle();
      break;
    }
  }

  if (status != HCI_SUCCESS)
    return new_handle;

  ALOGD("%s new device added Addr %s  le_handle 0x%04x, stack_handle 0x%04x", __func__,
         ConvertRawBdaddress(bdaddr),handle, new_handle);

  new_device.bdaddr = bdaddr;
  new_device.le_handle = handle;
  new_device.stack_handle = new_handle;
  xpan_map.push_back(new_device);
  return new_handle;
}

uint16_t QHciHm::xpanConnectionCmplt(bdaddr_t bdaddr) {
  ALOGD("%s Addr %s ", __func__, ConvertRawBdaddress(bdaddr));
  xpanDeviceList new_device;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      ALOGD("%s Already Known device Addr %s  stack_handle 0x%04x", __func__,
             ConvertRawBdaddress(bdaddr), it->stack_handle);
      it->xpan_handle = it->stack_handle;
      return it->stack_handle;
    }
  }

  new_device.bdaddr = bdaddr;
  new_device.stack_handle = generateHandle();
  new_device.xpan_handle = new_device.stack_handle;
  xpan_map.push_back(new_device);
  return new_device.stack_handle;
}

uint16_t QHciHm::bredrDisconnectionCmplt(uint16_t handle) {
  uint16_t stack_handle = 0x00;

  for (auto it = bredr_map.begin(); it != bredr_map.end(); it++) {
    if (it->bredr_handle == handle) {
      stack_handle = it->stack_handle;
      bredr_map.erase(it);
      return stack_handle;
    }
  }

  return stack_handle;
}

uint16_t QHciHm::leDisconnectionCmplt(uint16_t handle) {
  uint16_t stack_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (it->le_handle == handle) {
      it->le_handle = 0x00;
      stack_handle = it->stack_handle;
      if (it->xpan_handle == 0x00) {
        xpan_map.erase(it);
      }
      return stack_handle;
    }
  }

  return stack_handle;
}

uint16_t QHciHm::xpanDisconnectionCmplt(bdaddr_t bdaddr) {
  uint16_t stack_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      it->xpan_handle = 0x00;
      stack_handle = it->stack_handle;
      if (it->le_handle == 0x00) {
        xpan_map.erase(it);
      }
      return stack_handle;
    }
  }

  return stack_handle;
}

uint16_t QHciHm::GetBredrHandleFromStackHandle(uint16_t stack_handle) {
  uint16_t bredr_handle = 0x00;

  for (auto it = bredr_map.begin(); it != bredr_map.end(); it++) {
    if (it->stack_handle == stack_handle) {
      bredr_handle = it->bredr_handle;
      return bredr_handle;
    }
  }

  if (bredr_handle == 0x00)
    bredr_handle = stack_handle;

  return bredr_handle;
}

uint16_t QHciHm::GetStackHandleFromBredrHandle(uint16_t bredr_handle) {
  uint16_t stack_handle = 0x00;

  for (auto it = bredr_map.begin(); it != bredr_map.end(); it++) {
    if (it->bredr_handle == bredr_handle) {
      stack_handle = it->stack_handle;
      return stack_handle;
    }
  }

  if (stack_handle == 0x00)
    stack_handle = bredr_handle;

  return stack_handle;
}

uint16_t QHciHm::GetXpanHandleFromStackHandle(uint16_t stack_handle) {
  uint16_t xpan_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (it->stack_handle == stack_handle) {
      xpan_handle = it->xpan_handle;
      return xpan_handle;
    }
  }

  if (xpan_handle == 0x00)
    xpan_handle = stack_handle;

  return xpan_handle;
}

uint16_t QHciHm::GetLeHandleFromStackHandle(uint16_t stack_handle) {
   uint16_t le_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (it->stack_handle == stack_handle) {

      if (it->le_handle == 0) return stack_handle;

      le_handle = it->le_handle;
      return le_handle;
    }
  }

  if (le_handle == 0x00)
    le_handle = stack_handle;

  return le_handle;
}

uint16_t QHciHm::GetStackHandleFromLeHandle(uint16_t le_handle) {
  uint16_t stack_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (it->le_handle == le_handle) {
      stack_handle = it->stack_handle;
      return stack_handle;
    }
  }

  if (stack_handle == 0x00)
    stack_handle = le_handle;

  return stack_handle;
}

uint16_t QHciHm::GetStackHandleFromXPANHandle(uint16_t xpan_handle) {
  uint16_t stack_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (it->xpan_handle == xpan_handle) {
      stack_handle = it->stack_handle;
      return stack_handle;
    }
  }

  if (stack_handle == 0x00)
    stack_handle = xpan_handle;

  return stack_handle;
}

uint16_t QHciHm::GetXpanHandleFromBdaddr(bdaddr_t bdaddr) {
  uint16_t xpan_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      xpan_handle = it->xpan_handle;
      return xpan_handle;
    }
  }

  return xpan_handle;
}

uint16_t QHciHm::GetLeHandleFromBdaddr(bdaddr_t bdaddr) {
  uint16_t le_handle =0x00;

  for (auto it = xpan_map.begin(); it != xpan_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      le_handle = it->le_handle;
      return le_handle;
    }
  }

  return le_handle;
}

uint16_t QHciHm::GetBredrHandleFromBdaddr(bdaddr_t bdaddr) {
  uint16_t bredr_handle =0x00;

  for (auto it = bredr_map.begin(); it != bredr_map.end(); it++) {
    if (CmpBDAddrs(it->bdaddr, bdaddr)) {
      bredr_handle = it->bredr_handle;
      return bredr_handle;
    }
  }

  return bredr_handle;
}

uint16_t QHciHm::GetSocHandleFromStackHandle(uint16_t stack_handle) {
  uint16_t handle = 0x00;
  uint16_t xpan_handle = 0;

  handle = GetBredrHandleFromStackHandle(stack_handle);

  xpan_handle = GetXpanHandleFromStackHandle(stack_handle);

  if ((handle == stack_handle) && (xpan_handle != stack_handle)) {
    handle = GetLeHandleFromStackHandle(stack_handle);
  } else if (xpan_handle == stack_handle) {
    handle = xpan_handle;
  }
  //TODO Fetch handle based on QHCI State

  QHCI_CTSM_STATE qhci_state = QHci::Get()->GetQhciTransportState(handle);
  TransportType qhci_link = QHci::Get()->GetQHciActiveLinkTransport(handle);

  if (qhci_state == QHCI_BT_ENABLE_AP_ENABLE) {
    if (qhci_link == BT_LE) {
      ALOGD("%s: Assigning LE Handle", __func__);
      handle = GetLeHandleFromStackHandle(stack_handle);
    } else if (qhci_link == XPAN_AP) {
      ALOGD("%s: Assigning XPAN Handle", __func__);
      handle = GetXpanHandleFromStackHandle(stack_handle);
    }
  }
  ALOGD("%s: Handle %d stack handle %d Xpan_Handle %d ", __func__,
               handle, stack_handle, xpan_handle);

  return handle;
}

uint16_t QHciHm::GetStackHandleFromSocHandle(uint16_t soc_handle) {
  uint16_t handle = 0x00;

  handle = GetStackHandleFromBredrHandle(soc_handle);

  if (handle == soc_handle) {
    handle = GetStackHandleFromLeHandle(soc_handle);
  }

  return handle;
}

} // namespace implementation
} // namespace xpan

