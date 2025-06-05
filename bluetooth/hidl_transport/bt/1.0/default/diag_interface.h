/*==========================================================================
Description
  This file taps regular BT UART data (hci command, event, ACL packets)
  and writes into QXDM. It also writes the controller logs (controller's
  printf strings, LMP RX and TX data) received from BT SOC over the UART.

# Copyright (c) 2013, 2016-2017, 2023, Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc..

===========================================================================*/
#pragma once
#include <thread>
#include "log.h"
#include "diag_lsm.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "hci_transport.h"

#ifndef PROTOCOLTYPE
#define PROTOCOLTYPE
enum ProtocolType {
  TYPE_BT,
  TYPE_FM,
  TYPE_ANT,
  TYPE_PERI
};
#endif

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef DIAG_ENABLED
typedef struct {
  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
}DiagDeinitTimer;
#endif

class DiagInterface
{
 private:
  static const uint16_t log_bt_hdr_size_;
  static bool isDiagDeinitTimeout;
  void FormatLmp(uint8_t *, uint8_t *, int);
  HciTransport *hci_transport_;
  void GetTime(char *);
  static bool is_diag_ssr_triggered_;
#ifdef BT_GANGES_SUPPORTED
  static bool is_diag_triggered_for_peri_;
#endif
  static bool is_forced_ssr_triggered_;
  static bool is_cleanup_in_progress_;
  static unsigned int cleanup_status;
  bool init_status = false;
  bool deinit_status = false;
  bool diag_init_ongoing = false;
  std::mutex diag_mutex;
  static bool is_internal_cmd_timeout_;
#ifdef DIAG_ENABLED
  DiagDeinitTimer diag_deinit_timer_state_;
  void StopDiagDeinitTimeoutTimer();
  static void DiagDeinitTimedOut(union sigval  /*sig*/ );
  void StartDiagDeinitTimeoutTimer();
#endif

 public:
  DiagInterface();
  ~DiagInterface();
  bool Init(HciTransport *hci_transport);
  void UnlockDiagMutex();

#ifdef DIAG_ENABLED
  void *SsrBtHandler(void *, uint16);
  void SendLogs(const uint8_t *, int, int);
  bool BT_Diag_LSM_DeInit();
  bool GetDiagInitStatus();
#endif
  static void SetDiagDeInitTimeoutTriggered();
  static bool GetDiagDeInitTimeoutTriggered();
  bool isSsrTriggered();
  bool isDiagSsrTriggered();
#ifdef BT_GANGES_SUPPORTED
  bool isDiagSsrTriggeredForPeri();
#endif
  void ResetSsrTriggeredFlag();
  bool SetSsrTriggeredFlag();
  void CleanUp();
  bool SignalHALCleanup(ProtocolType type);
  void SignalEndofCleanup(ProtocolType type);
  bool GetCleanupStatus(ProtocolType type);
  void ResetForceSsrTriggeredFlag();
  void ResetCleanupflag();
  void SetInternalCmdTimeout();
  bool DiagInitOnGoing();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
