/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <hidl/HidlSupport.h>
#include "hci_internals.h"

typedef enum {
  BT_SOC_DEFAULT = 0,
  BT_SOC_SMD = BT_SOC_DEFAULT,
  BT_SOC_AR3K,
  BT_SOC_ROME,
  BT_SOC_CHEROKEE,
  BT_SOC_HASTINGS,
  BT_SOC_GENOA,
  BT_SOC_MOSELLE,
  BT_SOC_HAMILTON,
  BT_SOC_GANGES,
  BT_SOC_EVROS,
  /* Add chipset type here */
  BT_SOC_RESERVED
} BluetoothSocType;

/* Peripheral patch status*/
namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

typedef enum {
  HCI_PERI_PATCHING_NOT_DONE = 0,
  HCI_PERI_PATCHING_IN_PROGRESS = 0x0B,
  HCI_PERI_PATCHING_COMPLETE = 0x17,
  HCI_PERI_PATCHING_UNKNOWN = 0xFF,
  
  HCI_TME_PATCHING_NOT_DONE = 0,
  HCI_TME_PATCHING_IN_PROGRESS = 0x0B,
  HCI_TME_PATCHING_COMPLETE = 0x17,
  HCI_TME_PATCHING_UNKNOWN = 0xFF,
  HCI_TMEL_NOT_SUPPORTED = 0x06,
  HCI_TMEL_NOT_PATCHED_BY_OTHER_SS = 0x05,
  
} HciPatchStatus;

/* Activate actions */
enum HciActivate {
  HCI_ACTION_POWER_OFF = 0,
  HCI_ACTION_POWER_ON = 1,
  HCI_REPEATED_ATTEMPTS = 0x17,
  HCI_ACTION_UNKNOWN = 0xFF,
};

class HciTransport {
 public:
  virtual bool Init(BluetoothSocType soc_type) = 0;
  virtual int GetCtrlFd() = 0;
  virtual int GetDataFd() = 0;
  virtual bool CleanUp(void) = 0;
  virtual int Read(unsigned char* buf, size_t len) = 0;
  virtual int Write(HciPacketType type, const uint8_t *buf, int len) = 0;
  virtual ~HciTransport() {};
  virtual void Disconnect(bool) {};

 protected:
  BluetoothSocType soc_type_;
  std::mutex internal_mutex_;
  int ctrl_fd_;
  int data_fd_;
#ifdef QTI_BT_UART_SYSFS_SUPPORTED
  int sysfs_fd_;
#endif //QTI_BT_UART_SYSFS_SUPPORTED
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
