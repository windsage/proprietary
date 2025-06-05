/*
 * Copyright (c) 2017 - 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include <thread>
#include "controller.h"
#include "uart_controller.h"
//#include "patch_dl_manager.h"
#include "mct_controller.h"
#include "wake_lock.h"
#include "diag_interface.h"
#ifdef BT_GANGES_SUPPORTED
#include "notify_signal.h"
#endif
#include <mutex>
#ifdef BT_SECURE_PERIPHERAL_ENABLED
extern "C" {
#include "CPeripheralAccessControl.h"
#include "IPeripheralState.h"
#include "peripheralStateUtils.h"
}
#endif

#define OPCODE_LEN (7)
#define HCICMD_BUFF_SIZE (64)
#define TIME_STAMP_LEN (13)
#define HCI_WRITE_BD_ADDRESS_LENGTH 9
#define HCI_WRITE_BD_ADDRESS_OFFSET 3
#define MIN_CLIENTS_ACTIVE 1
#define HOST_ADD_ON_FEATURE_MASK_MAX_LENGTH 0xFF
#define PCS_NOTIFICATION_SIGNAL  (0xFF)

#define QSH_BLE_SNOOP_ENABLE_PROPERTY "persist.vendor.service.bt.qsh_ble.enablesnoop"
#define HCI_VS_OFFLOAD_HOST_COMM_CMD_OCF 0x3C
#define HCI_VS_OFFLOAD_HOST_SET_CONFIG_SUB_OPCODE 0
#define HCI_VS_OFFLOAD_HOST_MAX_CONFIG_LEN 0x18
#define HCI_VS_OFFLOAD_HOST_BTSNOOP_BIT_MASK 0x01
#ifdef BT_GANGES_SUPPORTED
// Peri commands
#define HCI_VS_GENERAL_OPCODE_PERI         (0xFFF1)
#define HCI_PERI_EVT_SS_ACTIVATE_COMPLETE  (0x02)
#endif
typedef struct {
  uint8_t feat_mask_len;
  uint8_t features[HOST_ADD_ON_FEATURE_MASK_MAX_LENGTH];
} HostAddOnFeatures_t;



#ifdef USER_DEBUG
#define COMMAND_RESET_UART_FLOW_ON \
  { 0x00, 0xFD, 0x00 }
#define COMMAND_GET_CTS_STATUS \
  { 0x10, 0xFD, 0x00 }
#define FRAME_GET_CTS_SOC_STATUS \
  { 0x0e, 0x06, 0x01, 0x0c, 0xfc, 0x00, 0x3a }
#define COMMAND_GET_SOC_CTS_STATUS \
  { 0x0c, 0xfc, 0x01, 0x39 }
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif
namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using InitializeCallback = std::function<void(bool success)>;

enum InitStatusType {
  INIT_STATUS_IDLE = 0,
  INIT_STATUS_INITIALIZING = 1,
  INIT_STATUS_FAILED = 2,
  INIT_STATUS_SUCCESS = 3
};

enum HciCommand {
  HCI_RESET_CMD = 1,
  SOC_PRE_SHUTDOWN_CMD,
  HCI_WRITE_BD_ADDRESS,
  HCI_SET_OFFLOAD_HOST_CONFIG_CMD,
  HCI_ACTIVATE_SS_CMD,
  HCI_SET_BAUDRATE_CMD
};

typedef struct {
  ProtocolType type;
  bool is_pending_init_cb;
  InitializeCallback init_cb;
  DataReadCallback data_read_cb;
} ProtocolCallbacksType;

typedef struct {
  char hcicmd_timestamp[HCICMD_BUFF_SIZE];
  char opcode[OPCODE_LEN];
} HciCmdTimestamp;
// it is the interface to the all external class like BT and FM and ANT.

typedef struct {
  TimerState timer_state;
  timer_t timer_id;
} InitTimer;

class DataHandler {
 public:
  // Function gives the Power driver file descriptor
  int GetPowerDriverFd(void);
  // Function Close the Power driver file descriptor
  void ClosePowerDriverFd(void);
  // with this APIs respective modules will register with their dataread cllback.
  // callback implementaiton varies from BT to FM.
  static bool Init(ProtocolType type, InitializeCallback init_cb,
      DataReadCallback data_read_cb);
  static void CleanUp (ProtocolType type);
  static DataHandler* Get();
  BluetoothSocType GetSocType();
  bool IsSocAlwaysOnEnabled();
  static int32_t NotifyEvent(const uint32_t peripheral, const uint8_t state);

  /*Sends command and wait for event*/
  bool sendCommandWait(HciCommand cmd, ProtocolType type);

  // this is used to send the actual packet.
  size_t SendData(ProtocolType type, HciPacketType packet_type, const uint8_t *data, size_t length);
  struct timeval time_hci_cmd_arrived_;
  HciCmdTimestamp last_hci_cmd_timestamp_;

  void AddHciCommandTag(char* dest_tag_str,  struct timeval& time_val, char * opcode);
  static bool CheckSignalCaughtStatus();
  void SetSignalCaught();
  bool SendBTSarData(const uint8_t *data, size_t length, DataReadCallback event_cb);
  bool SendBtTpiData(const uint8_t *data, size_t length, DataReadCallback event_cb);
  void registerTpiAsyncEventCb(DataReadCallback event_cb);
  void unRegisterTpiAsyncEventCb();
  uint16_t cmd_opcode;
  uint8_t  cmd_subopcode;
  SocAddOnFeatures_t* GetSoCAddOnFeatures();
  HostAddOnFeatures_t* GetHostAddOnFeatures();
  uint64_t GetChipVersion();
  unsigned int GetRxthreadStatus(ProtocolType);
  unsigned int GetClientStatus(ProtocolType);
  void SetRxthreadStatus(bool, ProtocolType);
  void SetClientStatus(bool, ProtocolType);
  void StartSocCrashWaitTimer();
  void SendBqrRiePacket();
  int IsXMEMEnabled();
  void StopInitTimer();
  int GetInitStatus();
  Controller* GetControllerRef();
#ifdef XPAN_SUPPORTED
  void QHciInitialized(bool);
  void OnPacketReadyFromQHci(HciPacketType type,
          const hidl_vec<uint8_t>*hidl_data, bool from_soc);
  void UpdateRingBufferFromQHci
          (HciPacketType packet_type, const uint8_t *data, int length);

#endif

#ifdef USER_DEBUG
  void SetTransport(HciUartTransport *);
  int  HandleFlowControl(userial_vendor_ioctl_op_t op);
  void SendCTSStatusToClient(unsigned char status);
  int  GetCTSLineStatus();
  bool command_is_get_rts_status(const unsigned char* buf, int len);
  bool command_is_get_cts_status(const unsigned char* buf, int len);
  bool command_is_reset_uart_flow(const unsigned char* buf, int len);
#endif
  void StackTimeoutTriggered();
  void LogPwrSrcsUartFlowCtrl();
  bool CheckForUartFailureStatus();
  void KillInitThread();
  std::thread::id GetInitThreadId();
  void XMInitialized(bool);
  Controller *controller_ = nullptr;

 private:
  DataHandler();
  static void usr1_handler(int s);
  static void usr2_handler(int);
  bool Open(ProtocolType type, InitializeCallback init_cb, DataReadCallback data_read_cb);
  bool Close(ProtocolType type);
  static int data_service_setup_sighandler(void);
  static void data_service_sighandler(int signum);
  BluetoothSocType GetSocTypeInt();
  bool isProtocolInitialized(ProtocolType type);
  void OnPacketReady(ProtocolType , HciPacketType, const hidl_vec<uint8_t>*, bool from_soc = true);
  void InternalOnPacketReady(ProtocolType , HciPacketType,
                             const hidl_vec<uint8_t>*, bool from_soc = true);
  bool isAwaitedEvent(const uint8_t *buff, uint16_t len);
  bool UpdatePowerDriverFd(void);
  int pwr_drv_Fd;
#ifdef BT_GANGES_SUPPORTED
  bool CheckPeripheralCoreActivityCode(SecondaryReasonCode activity_code);
  bool isAwaitedPeriEvent(const uint8_t *buff, uint16_t len);
  bool isAwaitedPeriNtf(const uint8_t *buff, uint16_t len);
  bool isAwaitedPeriBDEvt(const uint8_t *, uint16_t);
  bool ChangeSoCbd(void);
  bool DeactivatePeri(void);
#endif
  void UpdateRingBuffer(HciPacketType packet_type, const uint8_t *data, int length);
  bool isBTSarEvent(HciPacketType type, const uint8_t* data);
  bool isBtTpiEvent(HciPacketType type, const uint8_t* data);
  bool isBtTpiAsyncEvent(HciPacketType type, const uint8_t* data);
  bool isBtTpiAsyncVSEvent(HciPacketType type, const uint8_t* data);
  bool isProtocolAdded(ProtocolType type);
  static void InitTimeOut(union sigval);
  static void InitiateShutdown(union sigval);
  void StartInitTimer();
  static void StartShutdownTimer();
  TimerState GetInitTimerState();
  void SetInitTimerState(TimerState);
  void SetAppropriateLog(HciCommand, bool, ProtocolType);
  void SetHostAddOnFeatures(uint64_t chip_ver);
  void SetScramblingFeature(uint64_t chip_ver);
  const char * convertChipVerToStr(uint64_t chip_ver);
#ifdef BT_CP_CONNECTED
  bool GetXMState(void);
  bool UpdateCopVersion(CoPVerSupported);
#endif
  void SetOffloadHostConfig(ProtocolType);
  bool MakeHciCmdForSetOffloadHostConfig(uint8_t *cmd, size_t *len);

  virtual ~DataHandler() = default;
#ifdef XPAN_SUPPORTED
  bool GetQHciState(void);
#endif
  static bool caught_signal;
  DiagInterface diag_interface_;
#ifdef USER_DEBUG
  HciUartTransport *uart_transport_;
#endif
  Logger *logger_;
#ifdef BT_GANGES_SUPPORTED
  NotifySignal *notifysignal_;
#endif
  std::mutex internal_mutex_;
  static int init_status_;
  std::thread init_thread_;
  BluetoothSocType soc_type_;
  std::map<ProtocolType , ProtocolCallbacksType *> protocol_info_;
  DataReadCallback btsar_event_cb = nullptr;
  DataReadCallback bttpi_event_cb = nullptr;
  DataReadCallback bttpi_asyncevent_cb = nullptr;
  double GetInitCloseTSDiff();
  InitTimer init_timer_ = {TIMER_NOT_CREATED, 0};
  static std::mutex init_timer_mutex_;
  bool is_xmem_read_;
  int xmem_prop_val_;
  HostAddOnFeatures_t host_add_on_features;
  std::thread::id init_thread_id;
  bool is_init_thread_killed;
  std::thread prop_reset_thread;
  bool is_last_client_cleanup_in_progress;
#ifdef XPAN_SUPPORTED
  bool qhci_initialized;
#endif
  std::mutex property_reset_mutex_;
#ifdef BT_SECURE_PERIPHERAL_ENABLED
  static uint8_t currSecureState;
  static void *periContext;
  static timer_t shutdown_timer_id;
#endif
  static bool secureEvent;
#ifdef BT_CP_CONNECTED
  bool xpan_manager_initialized;
#endif

  static bool offload_host_config_sent;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
