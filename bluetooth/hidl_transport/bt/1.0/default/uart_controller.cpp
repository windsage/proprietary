/*
 * Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2017 The Android Open Source Project
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

#include <hidl/HidlSupport.h>
#include "uart_controller.h"
#include "hci_uart_transport.h"
#ifdef BT_GANGES_SUPPORTED
#include "peri_patch_dl_manager.h"
#endif
#include "power_manager.h"
#include "uart_utils.h"
#include <android-base/logging.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include "state_info.h"

#undef LOG_TAG

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-uart_controller"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-uart_controller"
#endif

#define PROC_PANIC_PATH     "/proc/sysrq-trigger"
#define BT_MAX_HCI_PKT_SIZE (1024 + 76) /* Max pkt size + safe margin */
#define MAX_INVALID_BYTES 2
#define HW_CRASH_TIMEOUT (7000)
#define HW_INIT_CRASH_TIMEOUT (60000)
#define CLIENT_INIT_TIMEOUT (1000)
#define BUGREPORT_WAIT_TIME 1000
#define BQR_CMD_LENGTH  10
#define BQR_ROOT_INFLAMMATION_BIT_POS 4
/* Bluetooth Quality Report OCF */
#define HCI_CONTROLLER_BQR_OPCODE_OCF 0xFD5E
#define MIN_BYTES_FOR_SYNC_CONFIRM (50)
#define WAKE_IND_ACCUMULATE_TIME (500 * 1000) // 500ms
#define MAX_INV_BYTES_LOGGED (20)
#define NUM_MS_FOR_SENDING_CRASH_BYTES (20)
#define MAX_STUCK_TIME  (25*1000) //25 sec

/* Special byte to crash SOC */
#define NMI_INTERRUPT_BYTE  (0xFC)
#define WARM_RESET_BYTE     (0xF8)
//Timer values for crashdump triggered
#define NMI_INTERRUPT_TIMEOUT  (1000) //in ms
#define NMI_INTERRUPT_INIT_FAIL_TIMEOUT (6000) //in ms
#define WARM_RESET_TIMEOUT     (1000)  //in ms
#define WARM_RESET_INIT_FAIL_TIMEOUT     (6000)  //in ms

namespace {

using android::hardware::bluetooth::V1_0::implementation::UartController;

ProtocolType GetProtocol(HciPacketType pkt_type)
{
  ProtocolType type = TYPE_BT;

  switch (pkt_type) {
    case HCI_PACKET_TYPE_COMMAND:
    case HCI_PACKET_TYPE_ACL_DATA:
    case HCI_PACKET_TYPE_SCO_DATA:
    case HCI_PACKET_TYPE_EVENT:
    case HCI_PACKET_TYPE_ISO_DATA:
      type = TYPE_BT;
      break;
    case HCI_PACKET_TYPE_ANT_CTRL:
    case HCI_PACKET_TYPE_ANT_DATA:
      type = TYPE_ANT;
      break;
    case HCI_PACKET_TYPE_FM_CMD:
    case HCI_PACKET_TYPE_FM_EVENT:
      type = TYPE_FM;
      break;
    case HCI_PACKET_TYPE_PERI_CMD:
    case HCI_PACKET_TYPE_PERI_DATA:
    case HCI_PACKET_TYPE_PERI_EVT:
      type = TYPE_PERI;
      break;
    default:
      break;
  }
  return type;
};

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

const int LEN_PACKET_TO_SOC = 1100;
extern bool soc_need_reload_patch;
#ifdef BT_GANGES_SUPPORTED
bool peri_patching_in_progress = false;
bool tme_patching_in_progress = false;
bool cleanup_in_progress = false;
bool notify_fwdwnld_cleanup = false;
#endif
struct power_buff_struct power_buff;
std::mutex rx_thread_timer_mutex_;
std::mutex rx_thread_state_mutex_;
PrimaryReasonCode actual_reason;
std::chrono::time_point<std::chrono::steady_clock> RxTimerStartTs, RxTimerSchedTs;
ThreadTimer UartController::recovery_timer_state_machine_ = {TIMER_NOT_CREATED, 0, 0};
using PacketReadCallback = std::function<void(ProtocolType, HciPacketType, const hidl_vec<uint8_t>*)>;
SocAddOnFeatures_t add_on_features = {0};

#ifdef BT_CP_CONNECTED
extern CoPVerSupported cop_ver_supported;
#endif

static bool is_PwrSrcsLogged = false;
#define BT_INIT_BAUD_RATE 115200
UartController::UartController(BluetoothSocType soc_type)
  : soc_crashed(false), soc_type_(soc_type),
  hci_packetizer_([this](hidl_vec<uint8_t> *data) { OnPacketReady(data); })
{
  prv_reason = BT_HOST_REASON_DEFAULT_NONE;
  init_done_ = false;
  hci_transport_ = nullptr;
  is_cmd_timeout = false;
#ifdef BT_GANGES_SUPPORTED
  notifysignal_ = NotifySignal::Get();
#endif
  /* BQR Root Inflammation Event enabled by default */
  is_bqr_rie_enabled_ = true;
  is_bqr_rie_sent_already = false;

  is_spurious_wake = false;
  is_invalid_pkt_from_soc = false;
  is_soc_wakeup_failure = false;
  force_special_byte_enabled_ = false;
  is_nmi_interrupt_triggered = false;
  is_warm_reset_triggered = false;
  ss_failed_to_notify = false;
  invalid_bytes_counter_ = 0;
  logger_ = Logger::Get();
  state_info_  = BtState::Get();
  sync_bytes_rcvd_ = 0;
  out_of_sync_ = false;
  memset(&rx_timer_state_machine_, 0 , sizeof(ThreadTimer));

  logger_->RegisterCrashDumpCallback([this](bool crash_dump_in_progress) {
    int ssr_level = logger_->ReadSsrLevel();
    if (crash_dump_in_progress) {
      HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
      if (uart_transport && (uart_transport->GetBaudRate() == BT_INIT_BAUD_RATE)) {
        StartSocCrashWaitTimer(SSR_INIT_TIMEOUT);
      } else {
        StartSocCrashWaitTimer(SSR_TIMEOUT);
      }
      //After SOC crashes, it will reset IBS to false
      soc_crashed = true;
    } else {
      StopSocCrashWaitTimer();
      SignalCrashDumpFinish();
      char value[PROPERTY_VALUE_MAX] = {'\0'};
      property_get("persist.vendor.service.bdroid.trigger_crash", value, "3");

      if (((strcmp(value, "3") == 0) && (ssr_level == 1 || ssr_level == 2)) ||
          ((ssr_level == 1 || ssr_level == 2) &&
          (is_cmd_timeout == false) &&
          (is_soc_wakeup_failure == false) &&
          (logger_->host_crash_during_init == false) &&
          (DataHandler::CheckSignalCaughtStatus() == false))) {
        logger_->StoreCrashReason();
        SendBqrRiePacket();
        logger_->CollectDumps(false, true);
        /* call kernel panic to report it to crashscope */
        bt_kernel_panic();
      } else {
         // We only dumps logs when dumped_uart_log is false hence reason is none here.
          ReportSocFailure(true, BT_HOST_REASON_DEFAULT_NONE, true, true);
      }

      is_spurious_wake = false;
      is_cmd_timeout = false;
      is_invalid_pkt_from_soc = false;
      is_soc_wakeup_failure = false;
      is_nmi_interrupt_triggered = false;
      is_warm_reset_triggered = false;
   }
  });

  logger_->RegisterHWEventCallback([this]() {
    /* There are chances of receiving HW error with out any dump or
     * before dump. Will start a timer to wait for the dump to be received
     * or else when timer overflows ReportSoCfailure is triggered.
     */

   HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
   if ((is_nmi_interrupt_triggered || is_warm_reset_triggered)  &&
       (logger_->GetPrimaryReason() == BT_HOST_REASON_INIT_FAILED)) {
     StartSocCrashWaitTimer(HW_INIT_CRASH_TIMEOUT);
   } else if (uart_transport && (uart_transport->GetBaudRate() == BT_INIT_BAUD_RATE)) {
     StartSocCrashWaitTimer(HW_INIT_CRASH_TIMEOUT);
   } else {
     StartSocCrashWaitTimer(HW_CRASH_TIMEOUT);
   }
  });
  //force special byte to be sent to SOC
#ifdef ENABLE_FW_CRASH_DUMP
  const char* default_send_special_byte = "true";
#else
  const char* default_send_special_byte = "false";
#endif

  char value[PROPERTY_VALUE_MAX] = { 0 };
  if (logger_->PropertyGet("vendor.wc_transport.force_special_byte", value,
                    default_send_special_byte)) {
    force_special_byte_enabled_ = (strcmp(value, "false") == 0) ? false : true;
  }

  soc_crash_wait_timer_state_ = TIMER_NOT_CREATED;
}

int UartController::bt_kernel_panic(void)
{
  struct {
    //char SubSystem[10];
    char PrimaryReason[50];
    char SecondaryReason[100];
  } CrashInfo;

  std::string str = Logger::Get()->GetPrimaryReasonString(Logger::Get()->GetPrimaryReason());

  ALOGD("%s: Invoking Kernel Panic.", __func__);

  memset (&CrashInfo, 0 , sizeof(CrashInfo));

  strlcpy (CrashInfo.PrimaryReason, str.c_str(), sizeof(CrashInfo.PrimaryReason));

  strlcpy (CrashInfo.SecondaryReason,
           Logger::Get()->GetSecondaryCrashReason(),
           sizeof(CrashInfo.SecondaryReason));

  ALOGE("%s:PrimaryReason = %s  SecondaryReason = %s\n", __func__, CrashInfo.PrimaryReason, CrashInfo.SecondaryReason);

  int ret = power_manager_.Panic(&CrashInfo);
  if (ret < 0)
    ALOGE("ioctl: kernel panic failed:%d error =(%s)", ret, strerror(errno));

  return ret;
}

PrimaryReasonCode UartController::GetPreviousReason()
{
    return prv_reason;
}

void UartController::WaitforCrashdumpFinish()
{
  if (Logger::Get()->GetPrimaryReason() == BT_HOST_REASON_RX_THREAD_STUCK)
    return;

  if (soc_crash_wait_timer_state_ != TIMER_ACTIVE && Logger::Get()->isSsrTriggered() == false) {
   return;
  }

  if (Logger::Get()->isSsrTriggered() && (soc_crash_wait_timer_state_ != TIMER_ACTIVE)) {
    StartSocCrashWaitTimer(SSR_TIMEOUT);
  }

  ALOGD("%s: Wait for collecting crash dump to finish\n", __func__);
  std::unique_lock<std::mutex> lk(cv_m);
  if (cv.wait_for(lk, std::chrono::seconds(10), [this]{
    return (soc_crash_wait_timer_state_ != TIMER_ACTIVE);})) {
    ALOGD("%s: finished collecting crashdump\n", __func__);
  } else {
    ALOGD("%s: collecting crashdump timed out\n", __func__);
  }
}

void UartController::SignalCrashDumpFinish()
{
  std::lock_guard<std::mutex> lk(cv_m);
  ALOGD("%s: notify the waiting clients \n", __func__);
  cv.notify_all();
}

#ifdef BT_GANGES_SUPPORTED
bool UartController::GetCleanupStatus(void)
{
  return cleanup_in_progress;
}

int UartController::WaitforFwDownloadCmpl()
{
  enum FwDownlodState inital_state = logger_->GetFwDownloadState();
  cleanup_in_progress = true;

  ALOGD("%s : %s", __func__, FwDownlodStateToString(inital_state));
  if (inital_state == FW_DWNLD_SUCCESS)
    return 0;
  else if (inital_state == FW_DWNLD_FAILED)
    return -1;

  std::unique_lock<std::mutex> lk(fwdwld_mtx);
  fwdwld_cv.wait_for(lk, std::chrono::milliseconds(2850), []
		 {return notify_fwdwnld_cleanup;});
  enum FwDownlodState current_state = logger_->GetFwDownloadState();
  ALOGD("%s : %s", __func__, FwDownlodStateToString(current_state));
  if (notify_fwdwnld_cleanup== false || current_state == inital_state) {
    ALOGD("%s: timeout triggered", __func__);
    return -1;
  } else if (current_state == inital_state) {
    ALOGD("%s: there is no change in state", __func__);
    return -1;
  } else if (current_state == DEFAULT_DWNLD_STATE) {
    ALOGD("%s: no need to change soc baudrate", __func__);
    return -1;
  } else if (current_state == PERI_DWNLD_CMPL ||
	     current_state == BT_DWNLD_CMPL || current_state == RX_THREAD_STARTED) {
    if (hci_transport_ == nullptr) {
      ALOGD("%s hci_transport_ is null", __func__);
      return -1;
    }
    HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
    uart_transport->ClockOperation(USERIAL_OP_CLK_ON);
    if (current_state  == RX_THREAD_STARTED) {
      ALOGD("%s: Rx thread is already active", __func__);
      return 0;
    }
    ALOGD("%s: starting Rx thread", __func__);
    // set up the fd watcher now
    int ret;
    ret = fd_watcher_.WatchFdForNonBlockingReads(
              uart_transport->GetCtrlFd(),
              [this](int fd) { OnDataReady(fd); });
    return 0;
  } else if (current_state == RX_THREAD_START_FAILED || current_state == FW_DWNLD_FAILED ||
	     current_state == BT_DWNLD_FAILED || current_state == PERI_DWNLD_FAILED) {
    return -1;
  }

  return -1;
}

void UartController::Signalfwdwnldstate(void)
{
  std::lock_guard<std::mutex> lk(fwdwld_mtx);
  ALOGD("%s: notify the waiting clients \n", __func__);
  notify_fwdwnld_cleanup = true;
  fwdwld_cv.notify_all();
}

void UartController::ResetHostBaudRate(void)
{
  std::unique_lock<std::mutex> guard(controller_mutex);
  if (hci_transport_ != nullptr) {
    ALOGD("%s", __func__);
     HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
     uart_transport->ResetHostBaudRate();
  }
}
#endif

void UartController::HciTransportCleanup(bool wakeup_needed)
{
  std::unique_lock<std::mutex> guard(controller_mutex);
  if (hci_transport_ != nullptr) {
    ALOGD("%s: deleting hci_transport", __func__);
    if (!Logger::Get()->isSsrTriggered())
      (static_cast<HciUartTransport* >(hci_transport_))->Disconnect(soc_need_reload_patch, wakeup_needed);
    else
      /* incase when ssr is triggered we do complete transport cleanup */
      (static_cast<HciUartTransport* >(hci_transport_))->Disconnect(wakeup_needed);
    delete hci_transport_;
    hci_transport_ = nullptr;
  } else {
    ALOGD("%s: Unable to delete hci_transport", __func__);
  }
}

bool UartController::Init(PacketReadCallback pkt_read_cb)
{
  HciUartTransport* uart_transport = nullptr;
#ifdef BT_GANGES_SUPPORTED
  PeriPatchDLManager* peri_patch_dl_manager = nullptr;
#endif
  PatchDLManager* patch_dl_manager = nullptr;
  char skip_patch_download[PROPERTY_VALUE_MAX];
  SocAddOnFeatures_t *temp_add_on_features = NULL;
  char wklk_buff[MAX_BUFF_SIZE];
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  int ret = 0, peri_patch_status;

  if (init_done_) {
    ALOGD("already initialized, return");
    return true;
  }

#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES)
    logger_->SetFwDownloadState(PRE_FW_DWNLD);
#endif

  // Start recovery stuck timer to check every 1 min for stuck scenarios after SSR.
  StartRecoveryStuckTimer();

  ALOGI("soc need reload patch = %d", soc_need_reload_patch);
  read_cb_ = pkt_read_cb;
  power_manager_.Init(soc_type_);
  if (soc_type_ == BT_SOC_CHEROKEE) {
    logger_->UpdateCheBasedIdealDelay();
  }
  if (soc_need_reload_patch) {
    logger_->SetSecondaryCrashReason(BT_HOST_REASON_POWER_ON_REGS_STUCK);
    gettimeofday(&tv, NULL);
    logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_POWER_ON_REGS_STUCK,
                                          "POWER ON REGS");
    // power off the chip first
    power_manager_.SetPower(false);

    // power on the chip using power manager
    power_manager_.SetPower(true);

    gettimeofday(&tv, NULL);
    logger_->CheckAndAddToDelayList(&tv);
  }
#ifdef WAKE_LOCK_ENABLED
  gettimeofday(&time_wk_lockacq_rel_, NULL);
  snprintf(wklk_buff, MAX_BUFF_SIZE, "Wakelock Acquired by UartController %s", __func__);
  state_info_->AddLogTag(dst_buff, time_wk_lockacq_rel_, wklk_buff);
  state_info_->SetWakeLockAcqTimeName(dst_buff);
  Wakelock :: Acquire();
#endif

  if (!(health_info = new (std::nothrow)HealthInfoLog())) {
    logger_->SetInitFailureReason(BT_HOST_REASON_MEMORY_ALLOCATION_FAILED);
    goto error;
  }
  // initialize the HCI transport
  if (!(uart_transport = new (std::nothrow)HciUartTransport(health_info))) {
    logger_->SetInitFailureReason(BT_HOST_REASON_MEMORY_ALLOCATION_FAILED);
    goto error;
  }

  hci_transport_ = static_cast<HciTransport*> (uart_transport);

  logger_->SetSecondaryCrashReason(BT_HOST_REASON_UARTINIT_STUCK);

  gettimeofday(&tv, NULL);
  logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UARTINIT_STUCK,
                                        "UART INIT");
  ret = uart_transport->Init(soc_type_, soc_need_reload_patch);
  gettimeofday(&tv, NULL);
  logger_->CheckAndAddToDelayList(&tv);

  if (!ret) {
    logger_->SetInitFailureReason(BT_HOST_REASON_UART_INIT_FAILED);
    goto error;
  }

  if (soc_type_ == BT_SOC_CHEROKEE && soc_need_reload_patch) {
    /* Check SW_CTRL & save its state for future reference
     * in BT kernel driver.
     * For CHE SW_CTRL is set by SoC after C0 and FC bytes are processed.
     * In case of HSP/HST/Moselle this is not the case.
     * In those SW_CTRL is asserted after toggling the BT_EN pin.
     */
    gettimeofday(&tv, NULL);
    logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_POWER_ON_REGS_STUCK,
                                         "CHECKING SW CTRL");
    power_manager_.CheckSwCtrl();

    gettimeofday(&tv, NULL);
    logger_->CheckAndAddToDelayList(&tv);
  }

  patch_dl_manager = new (std::nothrow)PatchDLManager(soc_type_, uart_transport, &power_manager_);
  if (!patch_dl_manager) {
    logger_->SetInitFailureReason(BT_HOST_REASON_MEMORY_ALLOCATION_FAILED);
    goto error;
  }

  sibs_enabled_ = patch_dl_manager->IsSibsEnabled();
  sobs_enabled_ = patch_dl_manager->IsSobsEnabled();
  ALOGD("%s: sibs_enabled = %d, sobs_enabled = %d \n ", __func__, sibs_enabled_, sobs_enabled_);

#ifdef WCNSS_OBS_ENABLED
  if (sobs_enabled_)
    ObsHandler::Init(uart_transport, health_info, this);
#endif

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_)
    IbsHandler::Init(uart_transport, health_info, this);
#endif

  gettimeofday(&tv, NULL);
  logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK,
                                              "UART CLK ON");
  uart_transport->ClockOperation(USERIAL_OP_CLK_ON);
  gettimeofday(&tv, NULL);
  logger_->CheckAndAddToDelayList(&tv);

#ifdef BT_GANGES_SUPPORTED
  {
    if (soc_type_ == BT_SOC_GANGES && cleanup_in_progress) {
      ALOGD("%s: cleanup is in progress", __func__);
      Signalfwdwnldstate();
      return false;
    }
  }
#endif

#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES) {
    logger_->SetFwDownloadState(PERI_DWNLD_STARTED);
    peri_patch_dl_manager = new (std::nothrow)PeriPatchDLManager(soc_type_, uart_transport);
    if (!peri_patch_dl_manager) {
      logger_->SetInitFailureReason(BT_HOST_REASON_MEMORY_ALLOCATION_FAILED_PERI);
      {
        if (cleanup_in_progress) {
          ALOGD("%s: cleanup is in progress", __func__);
          logger_->SetFwDownloadState(PERI_DWNLD_FAILED);
          Signalfwdwnldstate();
	      return false;
        }
      }
      goto error;
    }

    if (soc_need_reload_patch &&
	((peri_patch_status = peri_patch_dl_manager->PerformChipInit())< 0)) {
      ALOGE("%s: Peripheral Chip Init failed", __func__);
      if (cleanup_in_progress) {
        ALOGD("%s: cleanup is in progress", __func__);
        logger_->SetFwDownloadState(PERI_DWNLD_FAILED);
        Signalfwdwnldstate();
        return false;
      }
      if (peri_patch_status == -2) {
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_PATCHING_FAILED);
        StopRecoveryStuckTimer();
        DataHandler::Get()->StopInitTimer();
        SsrCleanup(BT_HOST_REASON_INIT_FAILED);
        peri_patch_dl_manager->ReadSocDump();
        return init_done_;
      } else {
        // Always turn UART clock off even if BT fail to boot.
        uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
        goto error;
      }
    }

    chipset_peri_ver_ = peri_patch_dl_manager->GetChipVersion();
    peri_patching_in_progress = false;
    {
      if (cleanup_in_progress) {
        ALOGD("%s: cleanup is in progress", __func__);
        logger_->SetFwDownloadState(PERI_DWNLD_CMPL);
        Signalfwdwnldstate();
	return false;
      }
      logger_->SetFwDownloadState(BT_DWNLD_STARTED);
    }
  }
#endif

#ifdef USER_DEBUG
  DataHandler::Get()->SetTransport(uart_transport);
#endif
  //Download the NVM/RAM patch
  if (soc_need_reload_patch) {
    logger_->PropertyGet("vendor.wc_transport.skip_patch_dload", skip_patch_download, "false");
    if (strcmp(skip_patch_download, "true") != 0) {
      int ret = patch_dl_manager->PerformChipInit();
#ifdef BT_GANGES_SUPPORTED
      if (soc_type_ == BT_SOC_GANGES) {
        logger_->SetFwDownloadState(ret < 0 ? BT_DWNLD_FAILED : BT_DWNLD_CMPL);

        if (cleanup_in_progress) {
          ALOGD("%s: cleanup is in progress", __func__);
          Signalfwdwnldstate();
          return false;
        }
      }
#endif
      if (ret < 0) {
        ALOGE("%s: Bluetooth Chip Init failed", __func__);
#ifdef BT_GANGES_SUPPORTED
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_PATCHING_FAILED);
        StopRecoveryStuckTimer();
        DataHandler::Get()->StopInitTimer();
        SsrCleanup(BT_HOST_REASON_INIT_FAILED);
        if (soc_type_ == BT_SOC_GANGES)
          peri_patch_dl_manager->ReadSocDump();
        else
          patch_dl_manager->ReadSocDump();
        // Always turn UART clock off even if BT fail to boot.
        uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
        return init_done_;
#else
        uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
        goto error;
#endif
      }

      temp_add_on_features = patch_dl_manager->GetAddOnFeatureList();

      if (temp_add_on_features != NULL
        && temp_add_on_features->feat_mask_len <= SOC_ADD_ON_FEATURE_MASK_MAX_LENGTH) {
        ALOGD("add on features read true");
        add_on_features.product_id = temp_add_on_features->product_id;
        add_on_features.rsp_version = temp_add_on_features->rsp_version;
        add_on_features.feat_mask_len = temp_add_on_features->feat_mask_len;
        memcpy(add_on_features.features, temp_add_on_features->features,
            temp_add_on_features->feat_mask_len);
      } else {
        ALOGD("add on features read false");
      }
    } else {
      logger_->SetSecondaryCrashReason(BT_HOST_REASON_SETBAUDRATE_CMD_STUCK);
      /* Change baud rate 115.2 kbps to 3Mbps*/
      int ret = patch_dl_manager->SetBaudRateReq();
#ifdef BT_GANGES_SUPPORTED
      {
        if (soc_type_ == BT_SOC_GANGES && cleanup_in_progress) {
          ALOGD("%s: cleanup is in progress", __func__);
	  logger_->SetFwDownloadState(ret == 0 ? BT_DWNLD_CMPL : BT_DWNLD_FAILED);
          Signalfwdwnldstate();
          return false;
        }
	if (soc_type_ == BT_SOC_GANGES)
	  logger_->SetFwDownloadState(STARTING_RX_THREAD);
      }
#endif
      if (ret < 0) {
        ALOGE("%s: Baud rate change failed!", __func__);
        logger_->SetInitFailureReason(BT_HOST_REASON_BAUDRATE_CHANGE_FAILED);
        goto error;
      }
      ALOGI("%s: Baud rate changed successfully ", __func__);
      }
    }

  chipset_ver_ = patch_dl_manager->GetChipVersion();

  // Move Bt SoC to retention mode. SW_CTRL will control the voting now
  if (soc_type_ == BT_SOC_HASTINGS || soc_type_ == BT_SOC_MOSELLE ||
      soc_type_ == BT_SOC_HAMILTON || soc_type_ == BT_SOC_GANGES ||
      soc_type_ == BT_SOC_EVROS ) {
    gettimeofday(&tv, NULL);
    logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_POWER_IOCTL_STUCK,
                                          "POWER ON to RETENTION MODE");
    power_manager_.SetPower(true, true);
    gettimeofday(&tv, NULL);
    logger_->CheckAndAddToDelayList(&tv);
  }

  logger_->SetSecondaryCrashReason(BT_HOST_REASON_RX_THREAD_START_STUCK);
  gettimeofday(&tv, NULL);
  logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_RX_THREAD_START_STUCK,
                                        "RX THREAD start");
  memset(dst_buff, 0, MAX_BUFF_SIZE);
  state_info_->AddLogTag(dst_buff, tv, (char *)"Starting Read thread");
  state_info_->SetReadThreadStartTS(dst_buff);
  // set up the fd watcher now
  ret = fd_watcher_.WatchFdForNonBlockingReads(
              uart_transport->GetCtrlFd(),
                [this](int fd) { OnDataReady(fd); });
  gettimeofday(&tv, NULL);
#ifdef BT_GANGES_SUPPORTED
  {
    if (soc_type_ == BT_SOC_GANGES) {
        logger_->SetFwDownloadState(ret == 0 ? RX_THREAD_STARTED : RX_THREAD_START_FAILED);
      if (cleanup_in_progress) {
        ALOGD("%s: cleanup is in progress", __func__);
        Signalfwdwnldstate();
        return false;
      }
    }
  }
#endif
  if (ret == 0) {
    memset(dst_buff, 0, MAX_BUFF_SIZE);
    state_info_->AddLogTag(dst_buff, tv, (char *)"Read thread was started: SUCCESS");
    state_info_->SetReadThreadStatus(dst_buff);
  } else {
    memset(dst_buff, 0, MAX_BUFF_SIZE);
    state_info_->AddLogTag(dst_buff, tv, (char *)"Read thread FAILED to start");
    state_info_->SetReadThreadStatus(dst_buff);
    logger_->SetInitFailureReason(BT_HOST_REASON_READ_THREAD_START_FAILED);
    goto error;
  }
#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES)
        logger_->SetFwDownloadState(FW_DWNLD_SUCCESS);
#endif
  gettimeofday(&tv, NULL);
  logger_->CheckAndAddToDelayList(&tv);

  /* Turn off the clocks, if SoC always ON is configured.
   * As SoC is sent to sleep state in last close.
   */
#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && !soc_need_reload_patch) {
    gettimeofday(&tv, NULL);
    logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK,
                                                "UART CLK OFF");
    uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
    gettimeofday(&tv, NULL);
    logger_->CheckAndAddToDelayList(&tv);
  }
#endif

#ifdef WAKE_LOCK_ENABLED
  gettimeofday(&tv, NULL);
  memset(dst_buff, 0, MAX_BUFF_SIZE);
  state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock::ReleaseDelay 500ms during UART init");
  state_info_->SetWakeLockRelTimeName(dst_buff);
  Wakelock :: ReleaseDelay(500);
#endif
#ifdef BT_GANGES_SUPPORTED
  if (peri_patch_dl_manager) {
    delete peri_patch_dl_manager;
    peri_patch_dl_manager = nullptr;
  }
#endif
  if (patch_dl_manager) {
    delete patch_dl_manager;
    patch_dl_manager = nullptr;
  }

  /* calling diag LSM init if DIAG_ENABLED is set*/
  Logger::Get()->Init(hci_transport_);

#ifdef ENABLE_HEALTH_TIMER
  if (health_info->HealthStatisticsTimerStart()== TIMER_ACTIVE) {
    memset(dst_buff, 0, MAX_BUFF_SIZE);
    strlcpy(dst_buff, "HealthStatisticsTimerStart started: SUCCESS", MAX_BUFF_SIZE);
    state_info_->SetHelthStatsTimerStatus(dst_buff);
  } else {
    memset(dst_buff, 0, MAX_BUFF_SIZE);
    strlcpy(dst_buff, "HealthStatisticsTimerStart FAILED to Start", MAX_BUFF_SIZE);
    state_info_->SetHelthStatsTimerStatus(dst_buff);
  }
#endif
  init_done_ = true;

  logger_->SetSecondaryCrashReason(BT_SOC_REASON_DEFAULT);
  ALOGD("Init succeded");
  return init_done_;

 error:
#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES)
    logger_->SetFwDownloadState(FW_DWNLD_FAILED);
#endif

  ALOGE("Init failed");
#ifdef BT_GANGES_SUPPORTED
  if (peri_patch_dl_manager) {
    delete peri_patch_dl_manager;
    peri_patch_dl_manager = nullptr;
  }

#endif
  if (patch_dl_manager) {
    delete patch_dl_manager;
    patch_dl_manager = nullptr;
  }

#ifdef WCNSS_OBS_ENABLED
  if (sobs_enabled_ && ObsHandler::Get()) {
    ObsHandler::Get()->CleanUp();
  }
#endif

#ifdef WCNSS_IBS_ENABLED
  {
    std::unique_lock<std::mutex> guard(ibs_clean_up_lock);
    if (sibs_enabled_ && IbsHandler::Get()) {
      IbsHandler::Get()->CleanUp();
    }
  }
#endif

  soc_need_reload_patch = true;
  if ((logger_->isSsrTriggered() == false) && CheckForUartFailureStatus()) {
    LogPwrSrcsUartFlowCtrl();
  }
#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES) {
    if (ret == -1 && notifysignal_) {
      ALOGE("%s: notify about ssr", __func__);
      notifysignal_->NotifyDriver(SSR_ON_BT);
    }
    ResetHostBaudRate();
  }
#endif

  HciTransportCleanup(true);
  // turn off the chip
  power_manager_.SetPower(false);
  power_manager_.Cleanup();
  if (health_info) {
    delete health_info;
    health_info = nullptr;
  }
#ifdef WAKE_LOCK_ENABLED
  gettimeofday(&time_wk_lockacq_rel_, NULL);
  snprintf(wklk_buff, MAX_BUFF_SIZE, "Released by UartController %s", __func__);
  memset(wklk_buff, 0, MAX_BUFF_SIZE);
  memset(dst_buff, 0, MAX_BUFF_SIZE);
  state_info_->AddLogTag(dst_buff, time_wk_lockacq_rel_, wklk_buff);
  state_info_->SetWakeLockRelTimeName(dst_buff);
  Wakelock :: Release();
#endif

  return init_done_;
}

void UartController::StartSocCrashWaitTimer()
{
  StartSocCrashWaitTimer(SSR_TIMEOUT);
}

void UartController::StartSocCrashWaitTimer(int ssrtimeout)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;
  uint32_t timeout_ms;

  ALOGI("%s, time:%d", __func__, ssrtimeout);

  if (soc_crash_wait_timer_state_ == TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))SocCrashWaitTimeout;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &soc_crash_wait_timer_);
    if (status == 0) {
      ALOGV("%s: SoC Crash wait timer created", __func__);
      soc_crash_wait_timer_state_ = TIMER_CREATED;
    } else {
      ALOGE("%s: Error creating timer %d\n", __func__, status);
    }
  }

  if (soc_crash_wait_timer_state_ == TIMER_CREATED || soc_crash_wait_timer_state_ == TIMER_ACTIVE) {
    timeout_ms = ssrtimeout;
    ts.it_value.tv_sec = timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(soc_crash_wait_timer_, 0, &ts, 0);
    if (status < 0)
      ALOGE("%s:Failed to set soc Crash wait timer: %d", __func__, status);
    else {
      ALOGV("%s: time started", __func__);
      soc_crash_wait_timer_state_ = TIMER_ACTIVE;
    }
  }
}

bool UartController::StopSocCrashWaitTimer()
{
  int status;
  struct itimerspec ts;
  bool was_active = false;

  ALOGV("%s", __func__);

  if (soc_crash_wait_timer_state_ == TIMER_ACTIVE) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    was_active = true;
    status = timer_settime(soc_crash_wait_timer_, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop soc Crash wait timer", __func__);
    else if (status == 0) {
      ALOGV("%s: Soc crash timer Stopped", __func__);
      soc_crash_wait_timer_state_ = TIMER_CREATED;
    }
  }
  return was_active;
}

void UartController::CleanupSocCrashWaitTimer()
{
  if (soc_crash_wait_timer_state_ == TIMER_ACTIVE)
    StopSocCrashWaitTimer();
  if (soc_crash_wait_timer_state_ == TIMER_CREATED) {
    timer_delete(soc_crash_wait_timer_);
    soc_crash_wait_timer_state_ = TIMER_NOT_CREATED;
    soc_crash_wait_timer_ = NULL;
  }
}

void UartController::SsrCleanup(PrimaryReasonCode reason)
{
  int retval = -1;

  if (!Logger::Get()->SetSsrTriggeredFlag()) {
    ALOGE("%s: Returning as SSR or cleanup in progress", __func__);
    if (prv_reason == BT_HOST_REASON_DEFAULT_NONE) {
        prv_reason = reason;
    }
    return;
  }

#ifdef BT_GANGES_SUPPORTED
  if (NotifySignal::Get()->GetSubSystemSsrStatus() == SSR_ON_UWB) {
    ALOGD("%s: SSR running on other Sub-System, Currently not allowed in BT", __func__);
    if (prv_reason == BT_HOST_REASON_DEFAULT_NONE) {
        prv_reason = reason;
    }
    return;
  }
#endif

  // Log all power srcs and UART flow ctrl status
  LogPwrSrcsUartFlowCtrl();

  if (power_manager_.isBtEnablePinPulledLow()) {
    logger_->SetSecondaryCrashReason(BT_HOST_REASON_BT_EN_PIN_LOW);
    logger_->SetasFpissue();
#ifndef BT_GANGES_SUPPORTED
  } else {
    if (CheckForUartFailureStatus())
      logger_->SetSecondaryCrashReason(logger_->GetUartErrCode());
  }
#else
  } else if (reason != BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM) {
    if (CheckForUartFailureStatus()){
      logger_->SetSecondaryCrashReason(logger_->GetUartErrCode());
    }
  }

  NotifySignal *notify_signal_ = NotifySignal::Get();
  if (notify_signal_ != NULL) {
    if ((!notify_signal_->notifySubsystem) &&
        (!notify_signal_->GetSubSystemSsrStatus()))
      notify_signal_->notifySubsystem = notify_signal_->NotifyDriver(SSR_ON_BT);
  } else {
    ALOGI("%s: failed to notify driver as NotifySignal handler destroyed\n", __func__);
  }
#endif

  logger_->SetPrimaryCrashReason(reason);

  /*Indicate it by sending special byte */
  if (reason == BT_HOST_REASON_SSR_CMD_TIMEDOUT ||
      reason == BT_HOST_REASON_SSR_SPURIOUS_WAKEUP ||
      reason == BT_HOST_REASON_SSR_INVALID_BYTES_RCVD ||
      reason == BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC ||
      reason == BT_HOST_REASON_SSR_RCVD_LARGE_PKT_FROM_SOC ||
      reason == BT_HOST_REASON_SSR_INTERNAL_CMD_TIMEDOUT ||
      reason == BT_HOST_REASON_SSR_SLEEP_IND_NOT_RCVD ||
      reason == BT_HOST_REASON_INIT_FAILED ||
      reason == BT_HOST_REASON_RX_THREAD_STUCK ||
#ifdef BT_GANGES_SUPPORTED
      reason == BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM ||
#endif
      reason == BT_HOST_REASON_FAILED_TO_SEND_INTERNAL_CMD) {
    if (force_special_byte_enabled_ && !logger_->IsCrashDumpStarted()) {
      if (reason == BT_HOST_REASON_SSR_CMD_TIMEDOUT ||
          reason == BT_HOST_REASON_SSR_INTERNAL_CMD_TIMEDOUT) {
        if (!logger_->isThisFpissue())
          is_cmd_timeout = true;
      } else if (reason == BT_HOST_REASON_SSR_SPURIOUS_WAKEUP) {
        is_spurious_wake = true;
      } else if (reason == BT_HOST_REASON_SSR_INVALID_BYTES_RCVD ||
        reason == BT_HOST_REASON_SSR_RCVD_LARGE_PKT_FROM_SOC) {
        is_invalid_pkt_from_soc = true;
      } else if (reason == BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC) {
        if (!logger_->isThisFpissue())
          is_soc_wakeup_failure = true;
      } else if (reason == BT_HOST_REASON_INIT_FAILED) {
        logger_->host_crash_during_init = true;
        if (logger_->GetSecondaryCrashReasonCode() != BT_HOST_REASON_PATCHING_FAILED) {
          if (logger_->GetUartErrCode() == SOC_REASON_START_TX_IOS_SOC_RFR_HIGH) {
            ALOGD("%s: Converting SOC_REASON_START_TX_IOS_SOC_RFR_HIGH"
                  " to SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT", __func__);
            logger_->SetUartErrCode(SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT);
          }
          // Checking delay list and set appropriate crash reason.
          logger_->CheckDelayListAndSetCrashReason();
          // Send BQR RIE early as crash reason dont depend on FW dump for init stuck case.
          SendBqrRiePacket();
        }
      }

      BtState::Get()->SetSSRtrigger(true);
      if (is_soc_wakeup_failure ||
          reason == BT_HOST_REASON_INIT_FAILED ||
          reason == BT_HOST_REASON_SSR_INVALID_BYTES_RCVD) {
        if(DataHandler::CheckSignalCaughtStatus() == false) {
          char value[PROPERTY_VALUE_MAX] = {'\0'};
          property_get("persist.vendor.service.bdroid.trigger_crash", value, "0");
          // call kernel panic so that all dumps are collected
          if (strcmp(value, "1") == 0) {
            ALOGE("%s: Do kernel panic immediately as property \"trigger_crash\" set to %s",
              __func__, value);
            // Log all power srcs and UART flow ctrl status
            LogPwrSrcsUartFlowCtrl();
            logger_->StoreCrashReason();
            logger_->PrepareDumpProcess();
            // Send BQR RIE before kernel panic
            SendBqrRiePacket();
            logger_->CollectDumps(true, true);
            if (bt_kernel_panic() == 0)
              return;
            else
              ALOGE("%s: Failed to do kernel panic", __func__);
          }
        }
      }

      ALOGD("%s: IsSoCCrashNotNeeded %d", __func__, IsSoCCrashNotNeeded(reason));
      if (IsSoCCrashNotNeeded(reason) || logger_->isThisFpissue()) {
        ReportSocFailure(false, reason, true, false);
      } else {
        // Log all power srcs and UART flow ctrl status
        LogPwrSrcsUartFlowCtrl();
        ALOGD("%s: SSR triggered due to %d sending special buffer", __func__, reason);
        if ((chipset_ver_ == HAMILTON_VER_2_0) ||
            (chipset_ver_ == 0 && soc_type_  == BT_SOC_HAMILTON) ||
            (soc_type_ == BT_SOC_GANGES) || (soc_type_ == BT_SOC_EVROS)) {
          TriggerSocCrashdump(NMI_INTERRUPT_BYTE, reason);
        } else {
          SendSpecialBuffer(reason);
        }
      }
    } else if (!force_special_byte_enabled_) {
      /*  Kill HIDL daemon for graceful recovery after SSR conditions are
       *  triggerred in USER builds, where vendor.wc_transport.force_special_byte
       * flag is set to false.
       */
      ALOGD("%s: SSR triggered due to %d skip sending special buffer",
            __func__, reason);
      ReportSocFailure(false, reason, true, false);
    }
  } else {
    Cleanup();
  }
}

bool UartController::IsSecondaryReasonValid()
{
  if (!strcmp(logger_->GetSecondaryCrashReason(), "Default"))
    return false;
  else
    return true;
}

PrimaryReasonCode UartController::GetPrimaryReason()
{
  return logger_->GetPrimaryReason();
}

void UartController::SocCrashWaitTimeout(union sigval sig)
{
  UartController *uart_controller = static_cast<UartController*>(sig.sival_ptr);
  Logger::is_crash_dump_in_progress_ = false;
  PrimaryReasonCode reason = BT_HOST_REASON_NONE_SOC_WAIT_TIMEOUT;
  if (uart_controller) {
#ifdef BT_GANGES_SUPPORTED
    if (NotifySignal::Get()->GetSubSystemSsrStatus() == SSR_ON_UWB) {
      ALOGE("%s: UWB Sub-system Failed to notify SSR COMPLETE status, Doing silent recovery\n", __func__);
      Logger::Get()->SetasFpissue();
      uart_controller->ss_failed_to_notify = true;
      Logger::Get()->SetPrimaryCrashReason(BT_HOST_REASON_PERI_SUB_SYSTEM_FAILED_UPDATE_SSR_COMPLETE);
    } else {
#endif
      if (uart_controller->is_nmi_interrupt_triggered) {
        ALOGE("%s: NMI interrrupt timed out, trigger warm reset \n", __func__);
        reason = uart_controller->GetPrimaryReason();
        uart_controller->is_nmi_interrupt_triggered = false;
        uart_controller->TriggerSocCrashdump(WARM_RESET_BYTE, reason);
        return;
      }
      if (uart_controller->is_warm_reset_triggered) {
        ALOGE("%s: crash due to warm reset failed\n", __func__);
        uart_controller->is_warm_reset_triggered = false;
      }

      reason = BT_HOST_REASON_NONE_SOC_WAIT_TIMEOUT;

      if (uart_controller->IsSecondaryReasonValid()) {
        ALOGE("%s: crash is decoded but dump is disabled\n", __func__);
        reason = uart_controller->GetPrimaryReason();
      } else if (uart_controller->is_spurious_wake) {
        ALOGE("%s: crashed due to spurious wake\n", __func__);
        reason = BT_HOST_REASON_SPURIOUS_WAKEUP_SOC_WAIT_TIMEOUT;
      } else if (uart_controller->is_invalid_pkt_from_soc) {
        ALOGE("%s: crashed due to invalid bytes received from SoC\n", __func__);
        reason = BT_HOST_REASON_INV_BYTES_SOC_WAIT_TIMEOUT;
      } else if (uart_controller->is_cmd_timeout) {
        ALOGE("%s: crashed due to command timeout\n", __func__);
        reason = BT_HOST_REASON_CMD_TIMEDOUT_SOC_WAIT_TIMEOUT;
      } else if (uart_controller->is_soc_wakeup_failure) {
        ALOGE("%s: crashed due to SoC wakeup failure\n", __func__);
        reason = BT_HOST_REASON_SOC_WAKEUP_FAILED_SOC_WAIT_TIMEOUT;
      } else if (Logger::Get()->isSsrTriggered()) {
        ALOGE("%s: crash due to diag triggered SSR\n", __func__);
        reason = BT_HOST_REASON_SOC_CRASHED_DIAG_SSR_SOC_WAIT_TIMEOUT;
        Logger::Get()->SetPrimaryCrashReason(reason);
      } else {
          /* Ideally we should not reach this part of code, as we should have valid reason
           * from above cases for triggerring SSR and starting CrashTimer
           */
        ALOGE("%s: Crashed due to unknown reason\n", __func__);
      }
#ifdef BT_GANGES_SUPPORTED
    }
#endif
    uart_controller->ReportSocFailure(false, reason, false, true);
  }
}

void UartController::SendBqrRiePacket()
{
  if (is_bqr_rie_sent_already == true) {
    ALOGD("%s: Returning as BQR RIE already sent", __func__);
    return;
  }

  hidl_vec<uint8_t> *bt_packet_ = new hidl_vec<uint8_t>;
  logger_->FrameBqrRieEvent(bt_packet_);

  if (is_bqr_rie_enabled_) {
    /* Crash reason is posted to client */
    if (read_cb_ && logger_->GetClientStatus(TYPE_BT)) {
      ALOGD("%s sending vendor specific crash reason to the client", __func__);
      read_cb_(GetProtocol(HCI_PACKET_TYPE_EVENT), HCI_PACKET_TYPE_EVENT, bt_packet_);
    } else {
      ALOGD("%s: skip sending root inflammation event packet", __func__);
    }
  }
  is_bqr_rie_sent_already = true;
}

void UartController::SendCrashPacket()
{
  hidl_vec<uint8_t> *bt_packet_ = new hidl_vec<uint8_t>;

  /* Crash reason is posted to client if it is active and they
   * don't trigger close call, if close is triggered observed chances
   * of missing the crash data in stack.
   */

  if (read_cb_ && logger_->GetClientStatus(TYPE_BT) &&
      (logger_->GetCleanupStatus(TYPE_BT) == false)) {
    logger_->FrameCrashEvent(bt_packet_);
    ALOGD("%s send crash reasons to the client", __func__);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_EVENT), HCI_PACKET_TYPE_EVENT, bt_packet_);
  } else {
    ALOGD("%s: skip sending crash packet", __func__);
  }
}

void UartController::updateCrashReasonAndDelayList() {
  logger_->StoreCrashReason();
  // Add Delay list info. in state file object.
  logger_->AddDelayListInfo();
}

void UartController::ReportSocFailure(bool dumped_uart_log, PrimaryReasonCode reason,
              bool cleanupSocCrashWaitTimer, bool cleanupIbs)
{
  unsigned char bt_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x10, 0x01, 0x0f };
  unsigned char fm_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x1A, 0x01, 0x0f };
  unsigned char ant_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x1C, 0x01, 0x0f };
  char count[PROPERTY_VALUE_MAX];
  static bool execution_count = false;
  bool IsRxSchedDelay = false;
  bool kill_needed = false;
  bool is_wlan_pulled_low = power_manager_.isBtEnablePinPulledLow();
  int trials = 0;

  /* Only once we need to excute ReportSoCFailure, we have chances
   * of calling ReportSoCFailure again by the timer threads due to
   * timeouts.
   */
  if (execution_count) {
    ALOGW("returing as other thread is in process of execution of %s", __func__);
    return;
  }
#ifdef BT_GANGES_SUPPORTED
  if (reason == BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM)
     logger_->SetPrimaryCrashReason(reason);
#endif

  actual_reason = reason;
  /* If Rx thread might have unblocked, stop closing BT. */
  if (reason == BT_HOST_REASON_RX_THREAD_STUCK) {
    if (GetRxThreadTimerState() != TIMER_OVERFLOW and
        ResetForceSsrTriggeredIfNoCleanup()) {
      ALOGI("%s: Rx thread unblocked returning back from cleanup", __func__);
      return;
    }

    long TsDiff = GetRxThreadSchedTSDiff();
    if (TsDiff > (RX_THREAD_USAGE_TIMEOUT + RX_THREAD_SCHEDULING_DELAY)) {
      IsRxSchedDelay = true;
      ALOGE("%s: Might be LMKD is running in background resulting RxThread Scheduling delay",
            __func__);
    }
    BtState ::Get()->UpdateBqrRieErrCodeAndErrString(reason,
                   std::string(logger_->GetPrimaryReasonString(reason)));
  }

  {
    std::unique_lock<std::mutex> guard(ibs_clean_up_lock);
    if (IbsHandler::Get())
      IbsHandler::Get()->SetCleanupHidlProgress();
  }

  execution_count = true;
  ALOGD("%s: reason %d", __func__, reason);

  /* Dont save crash reason here for host crashes during init.
   * Crash reason is stored at beggining when init timeout is detected.
   */
  if ((logger_->host_crash_during_init == false) && (!is_wlan_pulled_low)) {
    updateCrashReasonAndDelayList();
  } else {

    if (is_wlan_pulled_low)
      updateCrashReasonAndDelayList();

    property_get("persist.vendor.service.bdroid.system_delay_crash_count", count, "0");
    int trials = atoi(count);
    /* Incase of system delay issue, kill only upto certain max continuous count.
     * Check if init stuck happened because of system running slow, if yes then set kill flag.
     */
    if ((trials < MAX_CONTINUOUS_SYSTEM_DELAY_CRASH_COUNT &&
            !logger_->CheckActivityCodeForCrashCmd(logger_->GetSecondaryCrashReasonCode())) || (is_wlan_pulled_low))  {
      kill_needed = true;
      trials++;
      property_set("persist.vendor.service.bdroid.system_delay_crash_count", std::to_string(trials).c_str());
      if (trials > MAX_COUNT_WLAN_BT_EN_PIN_PULLED_DOWN) {
        ALOGD("%s: BT_EN pin pulled low count = %d is crossed threshold count(%d)",
             __func__, trials, MAX_COUNT_WLAN_BT_EN_PIN_PULLED_DOWN);
        kill_needed = false;
      } else {
        ALOGD("%s: BT_EN pin pulled low count = %d ", __func__, trials);
        kill_needed = true;
      }
    } else {
      ALOGE("%s: System delay crash count %d", __func__, trials);
      property_set("persist.vendor.service.bdroid.system_delay_crash_count", "0");
    }
  }

  if (logger_->isThisFpissue() && (!is_wlan_pulled_low)) {
      kill_needed = true;
  }

  /* No point in sending the HW error event to stack if crash
   * is due to Rx thread stuck.
   */
  if (read_cb_ && reason != BT_HOST_REASON_RX_THREAD_STUCK) {
    hidl_vec<uint8_t> *bt_packet_ = new hidl_vec<uint8_t>;
    hidl_vec<uint8_t> *fm_packet_ = new hidl_vec<uint8_t>;
    hidl_vec<uint8_t> *ant_packet_ = new hidl_vec<uint8_t>;
    fm_packet_->resize(LENGTH_HW_ERROR_EVT);
    ant_packet_->resize(LENGTH_HW_ERROR_EVT);
    bt_packet_->resize(LENGTH_HW_ERROR_EVT);
    /* For BT Client only */
    SendCrashPacket();
    // Dont send BQR RIE here as we already sent it before for init stuck cases.
    if (logger_->host_crash_during_init == false)
      SendBqrRiePacket();
    ALOGD("%s send H/W error event to FM/ANT/BT client", __func__);
    memcpy(fm_packet_->data(), fm_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_FM_EVENT), HCI_PACKET_TYPE_FM_EVENT, fm_packet_);

    memcpy(ant_packet_->data(), ant_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_ANT_CTRL), HCI_PACKET_TYPE_ANT_CTRL, ant_packet_);

    memcpy(bt_packet_->data(), bt_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_EVENT), HCI_PACKET_TYPE_EVENT, bt_packet_);
  }

  /* Stop Rx thread, as from this point we don't need any data from SoC. */
  fd_watcher_.StopThreadRoutine();

  /* Don't perform Cleanup of SocCrashTimeout if ReportSoC Failure is
   * triggerred from SocCrashWaitTimeout, which would stall
   * cleanup progress. Subsequent abort/kill will take
   * care of freeing IBS resources.
   */
  if (cleanupSocCrashWaitTimer) {
    ALOGD("%s calling CleanupSocCrashWaitTimer", __func__);
    CleanupSocCrashWaitTimer();
  }

  // Get current thread id.
  std::thread::id current_thread_id = std::this_thread::get_id();

  /* Killing init thread if it is not current thread inorder
   * to prevent ASAN issues that might occur if init thread get unstuck
   * and try to access freed resources.
   */
  if (current_thread_id != DataHandler::Get()->GetInitThreadId()) {
    DataHandler::Get()->KillInitThread();
  }

#ifdef WCNSS_OBS_ENABLED
  if (sobs_enabled_) {
    ObsHandler::Get()->StopAllTimers();
    if (cleanupIbs) {
      ALOGD("%s Cleaning up OBS", __func__);
      ObsHandler::Get()->CleanUp();
    }
  }
#endif

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_) {
     IbsHandler::Get()->StopAllTimers();
     IbsHandler::Get()->DeleteWackTimer();
    /* Don't perform IBS cleanup in below condition(s)
     * as this would kill IBS Timer and will stall further
     * cleanup progress. Subsequent abort/kill will take
     * care of freeing IBS resources.
     */
    if (cleanupIbs) {
      ALOGD("%s Cleaning up IBS", __func__);
      IbsHandler::Get()->CleanUp();
    }
  }
#endif

#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES)
    ResetHostBaudRate();
#endif

  HciTransportCleanup(true);

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: CleanUp();
#endif

#ifdef USER_DEBUG
  bool isDiagSsr =  logger_->isDiagSsrTriggered();
#endif

  /* stack_timeout_triggered stands true during cleanup
   * timeouts in stack. These timeouts may block HIDL
   * Rx thread. No point in logging the dumps in those
   * cases. Also if force reboot happened then also no need
   * to dump logs as issue might be false positive & caused
   * by force reboot.
   */
#ifdef BT_GANGES_SUPPORTED
  if (reason == BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM) {
    ALOGW("%s: Not logging the dumps as crash seen on other(UWB) Sub system\n", __func__);
  } else if ((reason == BT_HOST_REASON_RX_THREAD_STUCK &&
#else
  if ((reason == BT_HOST_REASON_RX_THREAD_STUCK &&
#endif
      (logger_->stack_timeout_triggered || IsRxSchedDelay)) ||
      DataHandler::CheckSignalCaughtStatus() || kill_needed) {
     if (is_wlan_pulled_low) {
       ALOGW("%s: not logging dumps as WLAN pulled BT_EN pin to low, doing silent recovery\n",
        __func__);
     } else if (ss_failed_to_notify) {
       ALOGW("%s: not logging dumps, as UWB didn't notifed the SSR complete status",
        __func__);
     } else {
       ALOGW("%s: not logging dumps as timeout triggered in stack or force reboot happened or System running slow",
        __func__);
     }
  } else if (!dumped_uart_log) {
    logger_->PrepareDumpProcess();
    logger_->CollectDumps(true, true);
#ifdef ENABLE_HEALTH_TIMER
    health_info->ReportHealthInfo();
#endif
  } else {
    logger_->CollectDumps(false, true);
  }

  if (Logger::is_bugreport_triggered_during_crash_dump) {
    pthread_mutex_lock(&Logger::crash_dump_lock);
    ALOGE("%s: now signal and wait for max. 1 sec for bugreport debug method to collect logs",
           __func__);
    pthread_cond_signal(&Logger::crash_dump_cond);
    pthread_mutex_unlock(&Logger::crash_dump_lock);
    std::unique_lock<std::mutex> guard(Logger::bugreport_wait_mutex);
    Logger::bugreport_wait_cv.wait_for(guard, std::chrono::milliseconds(BUGREPORT_WAIT_TIME),
                           []{return !(Logger::is_bugreport_triggered);});
  } else if (Logger::is_bugreport_triggered) {
    ALOGE("%s:  waiting for max. 1 sec for bugreport to collect logs", __func__);
    std::unique_lock<std::mutex> guard(Logger::bugreport_wait_mutex);
    Logger::bugreport_wait_cv.wait_for(guard, std::chrono::milliseconds(BUGREPORT_WAIT_TIME),
                           []{return !(Logger::is_bugreport_triggered);});
  }
  if(DataHandler::CheckSignalCaughtStatus() == false) {
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("persist.vendor.service.bdroid.trigger_crash", value, "0");
    if (strcmp(value, "2") == 0) {
      ALOGE("%s: \"trigger_crash\" property set to %s",
            __func__, value);
      /* Do kernel panic in case of cmd timeout/unable to wake up SoC
       * along with unable collect SoC crash dump partially or
       * completely when ssr level set to 1 or 2.
       */
      if (reason == BT_HOST_REASON_SOC_WAKEUP_FAILED_SOC_WAIT_TIMEOUT
          || reason == BT_HOST_REASON_CMD_TIMEDOUT_SOC_WAIT_TIMEOUT ||
          (force_special_byte_enabled_ &&
          (reason == BT_HOST_REASON_SSR_CMD_TIMEDOUT ||
          ((reason == BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC) &&
          !logger_->isThisFpissue())))) {
        int ssr_level = logger_->ReadSsrLevel();
        // call kernel panic so that all dumps are collected
        if (ssr_level == 1 || ssr_level == 2) {
          ALOGE("%s: Do kernel panic", __func__);
          if (bt_kernel_panic() < 0) {
            ALOGE("%s: Failed to do kernel panic", __func__);
          }
        }
      }
    }
  }

  /* If SSR is triggered due close sequence failure.
   * There could be a chance of power leakage if any of the
   * clients are not starting immediately.
   */
  power_manager_.SetPower(false);
  power_manager_.Cleanup();

#ifdef USER_DEBUG
  if (isDiagSsr) {
    ALOGE("Killing daemon as DIAG SSR is completed !");
    kill(getpid(), SIGKILL);
  } else if (reason == BT_HOST_REASON_RX_THREAD_STUCK) {
    kill_needed = true;

    /* All Rx thread stuck timeouts are not due stack taking more time
     * to process the received packet. Some are due system slowness either
     * LMKD or system was busy in processing high priority tasks or sometimes
     * binder can block Rx thread to return. The below will check and decide
     * the nature of crash. Rx thread crash is triggered when back to back
     * Rx thread timeouts are triggered.
     */
    if (IsRxSchedDelay) {
      ALOGE("Killing daemon as Rxthread timeout callback scheduling is delayed !");
    } else if (logger_->stack_timeout_triggered) {
      ALOGE("Killing daemon as Rxthread is blocked due to stack timeouts");
    } else if (GetRxThreadTimerState() != TIMER_OVERFLOW) {
      logger_->DeleteCurrentDumpedFiles();
      ALOGE("Killing daemon as Rxthread is unblocked !");
    } else {
      property_get("persist.vendor.service.bdroid.rxthread.stuck.count", count, "0");
      int num_of_iterations = atoi(count);
      num_of_iterations++;
      ALOGW("%s: Current Rx thread stuck count %d new count:%d", __func__,
            atoi(count), num_of_iterations);
      if (num_of_iterations < MAX_CONTINUOUS_RXTHREAD_STUCK) {
        logger_->DeleteCurrentDumpedFiles();
        property_set("persist.vendor.service.bdroid.rxthread.stuck.count",
                     std::to_string(num_of_iterations).c_str());
        ALOGE("Killing daemon as Rxthread stuck timeout count is within limit");
      } else {
        ALOGE("%s: Reseting Rxthread stuck timeout prop as count reaches to:%d",
              __func__, num_of_iterations);
        property_set("persist.vendor.service.bdroid.rxthread.stuck.count", "0");
        kill_needed = false;
      }
    }

    if (kill_needed) {
      kill(getpid(), SIGKILL);
    } else {
      ALOGE("Aborting daemon as SSR is completed !");
      abort();
    }
  } else if (DataHandler::CheckSignalCaughtStatus()) {
    // Delete current dumped logs, as issue triggered during reboot.
    logger_->DeleteCurrentDumpedFiles();
    ALOGE("Killing daemon as user triggered forced reboot");
    kill(getpid(), SIGKILL);
  } if (kill_needed) {
    if (is_wlan_pulled_low) {
      ALOGE("Killing daemon as BT_EN pin pull down by WLAN");
#ifdef BT_GANGES_SUPPORTED
    } else if ((reason == BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM) || (ss_failed_to_notify)) {
      ALOGE("Killing daemon as System silent recovery is completed !");
#endif
    } else {
      state_info_->PrintDelayListInfo();
      ALOGE("Killing daemon as system running slow");
    }
    kill(getpid(), SIGKILL);
  } else {
    ALOGE("Aborting daemon as SSR is completed !");
    abort();
  }
#else
  ALOGE("Killing daemon as SSR is completed!");
  kill(getpid(), SIGKILL);
#endif
}

void UartController::SendSpecialBuffer(PrimaryReasonCode reason)
{
  unsigned char buf[LEN_PACKET_TO_SOC];
  int retval;

  memset(buf, 0xfb, LEN_PACKET_TO_SOC);

  ALOGV("%s: sending special bytes to crash SOC", __func__);

  HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
  if (hci_transport_ == nullptr) {
    ALOGE("%s: failed to send special buffer", __func__);
    return;
  }

#ifdef WCNSS_IBS_ENABLED
  IbsHandler* hIbs = IbsHandler::Get();
  if (hIbs)
    hIbs->SerialClockVote(IBS_TX_VOTE_CLOCK_ON);
#endif

  if (reason != BT_HOST_REASON_SSR_INVALID_BYTES_RCVD) {
    retval = uart_transport->UartWrite(buf, LEN_PACKET_TO_SOC);
    if (retval <= 0) {
      ALOGE("%s: Write Err : %d (%s)", __func__, retval, strerror(errno));
      ReportSocFailure(false, BT_HOST_REASON_WRITE_FAIL_SPCL_BUFF_CRASH_SOC, true, false);
    } else {
      StartSocCrashWaitTimer(SSR_TIMEOUT);
    }
  } else {
    // As SoC may be asleep at this time, spread sending of crash bytes over sometime
    ALOGI("%s: Spread out sending of crash bytes over %d ms",  __func__,
           NUM_MS_FOR_SENDING_CRASH_BYTES);
    bool waitForCrashDump = false;
    for(int i = 0; i < NUM_MS_FOR_SENDING_CRASH_BYTES; i++) {
      retval = uart_transport->UartWrite(buf,
                 LEN_PACKET_TO_SOC / NUM_MS_FOR_SENDING_CRASH_BYTES);
      if (retval <= 0) {
        ALOGE("%s: Write Err : %d (%s)", __func__, retval, strerror(errno));
      } else {
        waitForCrashDump = true;
      }
      usleep(1000); // sleep for a millisecond
    }
    if (waitForCrashDump)
      StartSocCrashWaitTimer(SSR_TIMEOUT);
    else
      ReportSocFailure(false, BT_HOST_REASON_WRITE_FAIL_SPCL_BUFF_CRASH_SOC, true, false);
  }
}

int UartController::TriggerSocCrashdump(uint8_t crash_code, PrimaryReasonCode reason)
{
  int flags = 0;
  int retval = -1;
  int baud_rate_set = 0;
  if (hci_transport_ == nullptr) {
    ALOGE("%s: failed to send crashdump request", __func__);
    return -1;
  }

  SecondaryReasonCode sec_reason = logger_->GetSecondaryCrashReasonCode();
  ALOGD("%s: primary  reason : %x", __func__, reason);
  ALOGD("%s: secondary reason : %x", __func__, sec_reason);

  HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
  baud_rate_set = uart_transport->GetBaudRate();

  ALOGD("%s: ## userial_vendor_ioctl: UART Flow OFF ", __func__);
  if ((retval = uart_transport->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-OFF error: 0x%x \n", __func__, retval);
    return retval;
  }

  retval = uart_transport->SetSocToCrash(crash_code);
  if (retval < 0) {
    ALOGE("%s: Write Err : %d (%s)", __func__, retval, strerror(errno));
    ReportSocFailure(false, BT_HOST_REASON_WRITE_FAIL_SPCL_BUFF_CRASH_SOC, true, false);
#ifdef BT_GANGES_SUPPORTED
  } else {
    ALOGI("%s: set local uart baudrate to previous", __func__);
    uart_transport->SetBaudRate(uart_transport->find_baud_rate_code(baud_rate_set));
  }
#else
  } else if (reason == BT_HOST_REASON_INIT_FAILED &&
        (sec_reason == BT_HOST_REASON_GETVER_NO_RSP_RCVD)) {
      ALOGI("%s: set local uart baudrate to 115200 ", __func__);
      uart_transport->SetBaudRate(USERIAL_BAUD_115200);
  } else {
    ALOGI("%s: set local uart baudrate to Max supported ", __func__);
    uart_transport->SetBaudRate(uart_transport->GetMaxBaudrate());
  }
#endif
  uart_transport->GetBaudRate();

  if ((retval = uart_transport->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-ON error: 0x%x \n", __func__, retval);
    return retval;
  }

#ifdef BT_GANGES_SUPPORTED
// TO-DO: set cflag to clear ctsrts bits

  /* notify to Power Driver about BT SSR */
  NotifySignal *notify_signal_ = NotifySignal::Get();
  if (notify_signal_ != NULL) {
    if (!notify_signal_->notifySubsystem)
      notify_signal_->notifySubsystem = notify_signal_->NotifyDriver(SSR_ON_BT);
  } else {
    ALOGI("%s: failed to notify driver as NotifySignal handler destroyed\n", __func__);
  }
#endif

  if(crash_code == NMI_INTERRUPT_BYTE) {
    is_nmi_interrupt_triggered = true;
    if ((reason == BT_HOST_REASON_INIT_FAILED) &&
       ((sec_reason == BT_HOST_REASON_GETVER_NO_RSP_RCVD) ||
       (sec_reason == BT_HOST_REASON_PATCHING_FAILED))) {
      StartSocCrashWaitTimer(NMI_INTERRUPT_INIT_FAIL_TIMEOUT);
    } else
      StartSocCrashWaitTimer(NMI_INTERRUPT_TIMEOUT);
  } else if (crash_code == WARM_RESET_BYTE) {
    is_warm_reset_triggered = true;
    if ((reason == BT_HOST_REASON_INIT_FAILED) &&
       ((sec_reason == BT_HOST_REASON_GETVER_NO_RSP_RCVD) ||
       (sec_reason == BT_HOST_REASON_PATCHING_FAILED))) {
       StartSocCrashWaitTimer(WARM_RESET_INIT_FAIL_TIMEOUT);
    } else
       StartSocCrashWaitTimer(WARM_RESET_TIMEOUT);
  }
  return retval;
}

/**
  * this is to filter out Get Controller Debug Info cmd from stack.
  * we should crash SOC and collect ram dump after receiving it.
  */
bool UartController::command_is_get_dbg_info(const unsigned char* buf, int len)
{
    bool result = true;
    int i;
    const unsigned char get_dbg_info[] = {0x5B, 0xFD};

    ALOGV("%s: lenth = %d ", __func__, len);
    if ( len != 3) {
        result = false;
    }
    else {
        for (i=0; i<(len-1); i++) {
             if(buf[i] != get_dbg_info[i])
                result = false;
        }

        if (result && (buf[i] == 0x01)) {
          logger_->SetasFpissue();
        } else if (result && (buf[i] != 0x00)) {
          result = false;
        }
    }
    return result;
}

/**
  * this is to filter out Enable BQR cmd from stack.
  * we should check bit 4 of BT quality event mask and
  * accordingly enable/disable BQR root inflammation event.
  */
void  UartController::CheckForBQRRootInflammationBit(const unsigned char* buf)
{
    /* BQR cmd format:
       * 1st 2 bytes: BQR cmd Opcode_OCF (0xFD5E)
       * 3rd byte - parameter length
       * 4th byte - BQR report action (Add/Delete/Clear)
       * Next 4 bytes - 32 bit BQR quality event mask (Bit 4 is
       * set for enabling root inflammation event)
       * Next 2 bytes - BQR minimum report interval
       */

    if (buf[0] == (HCI_CONTROLLER_BQR_OPCODE_OCF & 0x00FF)
        && buf[1] == ((HCI_CONTROLLER_BQR_OPCODE_OCF & 0xFF00)>>8)) {
      /* checking for bit 4 of BQR quality event mask */
      if (buf[4] & (1 << BQR_ROOT_INFLAMMATION_BIT_POS)) {
        if (buf[3] == BQR_ACTION_ADD) {
          ALOGD("%s: BQR root inflammation enabled", __func__);
          is_bqr_rie_enabled_ = true;
        } else if (buf[3] == BQR_ACTION_DELETE) {
          ALOGD("%s: BQR root inflammation disabled", __func__);
          is_bqr_rie_enabled_ = false;
        }
      }
      else {
        ALOGD("%s: BQR root inflammation disabled", __func__);
        is_bqr_rie_enabled_ = false;
      }
    }

}


size_t UartController::SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length)
{
  size_t size = 0;
  int ibs_status = 0;
  int obs_status = 0;

#ifdef BT_GANGES_SUPPORTED
  if (notifysignal_->GetSubSystemSsrStatus()) {
    return size;
  }
#endif

  if (Logger::Get()->isSsrTriggered() || (soc_crash_wait_timer_state_== TIMER_ACTIVE)) {
    /* Cleanup will be halted if rx thread unblocked a later point.
     * Check and send the command to the SoC.
     */
    {
      std::unique_lock<std::mutex> guard(rx_thread_timer_mutex_);
      if (actual_reason == BT_HOST_REASON_RX_THREAD_STUCK) {
        if (GetRxThreadTimerState() == TIMER_OVERFLOW) {
           ALOGD("<%s: give up Tx since crash dump has started", __func__);
           return size;
        }
        else {
           actual_reason = BT_HOST_REASON_DEFAULT_NONE;
        }
      } else {
        ALOGD("<%s: give up Tx since crash dump has started", __func__);
        return size;
      }
    }
  }

#ifdef WCNSS_OBS_ENABLED
  if (sobs_enabled_ && !soc_crashed) {
    ObsHandler* hObs = ObsHandler::Get();
    if (!hObs) {
      ALOGW("<%s: Uart socket has been closed", __func__);
      return size;
    } else {
      obs_status = hObs->WakeAssert();
      // Ignore error handling first.
      // perform recovery and dump logs only if SoC not crashed
      /*
      if (!(DataHandler::Get()->CheckSignalCaughtStatus()) && obs_status && !soc_crashed) {
        ALOGE("Initiating crash dump collection as unable wake up SOC");
        SsrCleanup(BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC);
        return size;
      }*/
    }
  }
#endif

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && !soc_crashed) {
    IbsHandler* hIbs = IbsHandler::Get();
    if (!hIbs) {
      ALOGW("<%s: Uart socket has been closed", __func__);
      return size;
    } else {
      ibs_status = hIbs->WakeAssert();
      // perform recovery and dump logs only if SoC not crashed
#ifdef BT_GANGES_SUPPORTED
      if (!(DataHandler::Get()->CheckSignalCaughtStatus()) &&
           ibs_status && !soc_crashed && (!notifysignal_->GetSubSystemSsrStatus())) {
#else
      if (!(DataHandler::Get()->CheckSignalCaughtStatus()) && ibs_status && !soc_crashed) {
#endif
        ALOGE("Initiating crash dump collection as unable wake up SOC");
        SsrCleanup(BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC);
        return size;
      }
    }
  }
#endif

  if (HCI_PACKET_TYPE_COMMAND == packet_type && length == BQR_CMD_LENGTH) {
    CheckForBQRRootInflammationBit(data);
  }
#ifdef USER_DEBUG
  DataHandler *data_handler_ = DataHandler::Get();
  if (data_handler_ != nullptr && data_handler_->command_is_get_rts_status(data, length)) {
    ALOGD("<%s: received Get_RTS_STATUS command", __func__);
    //set UART flow off
    data_handler_->HandleFlowControl(USERIAL_OP_FLOW_OFF);
  }
#endif

  if ((HCI_PACKET_TYPE_COMMAND == packet_type) &&
      command_is_get_dbg_info(data, length)) {
    ALOGD("<%s: received Get_DBG_INFO command", __func__);
    if (is_soc_wakeup_failure == false) {
      if (logger_->stack_timeout_triggered == false)
        SsrCleanup(BT_HOST_REASON_SSR_CMD_TIMEDOUT);
      else
        ALOGE("%s: Discarding command timeout due to stack timeout", __func__);
    }
    else {
      ALOGE("%s: SSR: Unnable to wakeUp SoC, dropping GET_DBG_INFO Command", __func__);
      return size;
    }
  } else if ((ibs_status == 0 || (DataHandler::Get()->CheckSignalCaughtStatus()))
                && hci_transport_ != nullptr) {
    size = hci_transport_->Write(packet_type, data, length);
  } else {
#ifdef WCNSS_OBS_ENABLED
    ALOGE("%s: Dropping packet! packet type = %d obs_status = %d",
          __func__, packet_type, obs_status);
#endif
#ifdef WCNSS_IBS_ENABLED
    ALOGE("%s: Dropping packet! packet type = %d ibs_status = %d",
          __func__, packet_type, ibs_status);
#endif
  }

#ifdef WCNSS_OBS_ENABLED
  //TransmitDone -> StartIdleTimer -> IdleTimeout.
  // -> DeviceSleep -> turn off UART/CLK and pull low BT_WAKE GPIO.
  if (sobs_enabled_ && (obs_status == 0) && !soc_crashed) {
    ObsHandler::Get()->TransmitDone(true);
  }
#endif
#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && (ibs_status == 0) && !soc_crashed) {
    IbsHandler::Get()->TransmitDone(true);
  }
#endif
  return size;
}

bool UartController::IsBqrRieEnabled()
{
  return is_bqr_rie_enabled_;
}

void UartController::UnlockControllerMutex()
{
   ALOGE("%s: unlocking controller_mutex", __func__);
   controller_mutex.unlock();
}

bool UartController::Disconnect()
{
  is_PwrSrcsLogged = false;
  fd_watcher_.StopThreadRoutine();
#ifdef BT_GANGES_SUPPORTED
  if (soc_type_ == BT_SOC_GANGES)
    ResetHostBaudRate();
#endif
  HciTransportCleanup(true);
  // turn off the chip as init sequence is stuck.
  if (soc_need_reload_patch) {
    power_manager_.SetPower(false);
    power_manager_.Cleanup();
  } else {
    power_manager_.CloseFd();
  }

  return true;
}

#ifdef BT_GANGES_SUPPORTED
int UartController::ResetBaudrate(void)
{
  HciPacketType packet_type = HCI_PACKET_TYPE_PERI_CMD;
  const uint8_t data[] = {0x00, 0xf1, 0xff, 0x02, 0x02, 0x00};
  int length = 6;
  int flags;
  int err;

  ALOGI("%s: HCI_SET_BAUDRATE_CMD",  __func__);
  /* Flow off during baudrate change */
  HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && !soc_crashed) {
      IbsHandler* hIbs = IbsHandler::Get();
      if (!hIbs) {
        ALOGE("%s: unable to wakeup soc as hIbs in nullptr", __func__);
	return -1;
      } else {
        int ret = IbsHandler::Get()->WakeAssert();
        if (ret) {
          ALOGE("%s: Unable to wakeup soc", __func__);
	  return -1;
	}
      }
  } else if (soc_crashed) {
    ALOGE("%s: soc crashed", __func__);
    return -1;
  }
#endif
 
  if ((err = uart_transport->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-off error : 0x%x \n", __func__, err);
    return err;
  }

  if (!SendPacket(packet_type, data, length)) {
    ALOGE("Unable to send Activate cmd \n");
    return  -1;
  }

  usleep(20 * 1000);
  tcdrain(uart_transport->GetCtrlFd());
  /* Change Local UART baudrate to high speed UART */
  uart_transport->SetBaudRate(USERIAL_BAUD_115200);
   /* Flow on after changing local uart baudrate */
  if ((err = uart_transport->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
      ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
      return err;
  }
  return 0;
}
#endif

bool UartController::Cleanup(void)
{
  if (!init_done_) {
    ALOGD("UartController:: already closed, return");
    return false;
  }

  /* Rechecking whether the SSR is in progress, before voting down the regs */
  if (Logger::Get()->isSsrTriggered() || (soc_crash_wait_timer_state_== TIMER_ACTIVE)) {
    ALOGW("waiting for crashdump to finish in %s", __func__);
    WaitforCrashdumpFinish();
    return false;
  }

  ALOGD("UartController::Cleanup, soc_need_reload_patch=%d", soc_need_reload_patch);

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_) {
    bool ret;
    if (soc_need_reload_patch && soc_type_ == BT_SOC_CHEROKEE) {
      ret = IbsHandler::Get()->WakeAssert();
      if (ret) {
        ALOGE("Initiating crash dump collection as unable wake up SOC");
        Logger::Get()->SetInternalCmdTimeout();
        Logger::Get()->ResetCleanupflag();
        SsrCleanup(BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC);
        WaitforCrashdumpFinish();
        return false;
      }
    } else {
      IbsHandler::Get()->DeviceSleep();
    }

    ret = IbsHandler::Get()->waitUntilSleepIndRecvd();
    if (!ret) {
      soc_need_reload_patch = true;
      ALOGE("%s:Failed to receive SLEEP IND from SoC", __func__);
      Logger::Get()->SetInternalCmdTimeout();
      Logger::Get()->ResetCleanupflag();
      Logger::Get()->SetSecondaryCrashReason(BT_HOST_REASON_FAILED_TO_RECEIVE_SLEEP_IND);
      SsrCleanup(BT_HOST_REASON_SSR_SLEEP_IND_NOT_RCVD);
      WaitforCrashdumpFinish();
      return false;
    }
  }
#endif
  // stop the fd watcher
  fd_watcher_.StopWatchingFileDescriptors();

  /* Possiblity we are in between dump proces discard dump and stop the timer. */
  CleanupSocCrashWaitTimer();

#ifdef WCNSS_OBS_ENABLED
  if (sobs_enabled_) {
    ObsHandler::Get()->StopAllTimers();
    ObsHandler::Get()->CleanUp();
  }
#endif
#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_) {
    IbsHandler::Get()->StopAllTimers();
    IbsHandler::Get()->CleanUp();
  }
#endif

  // clean up the transport
  HciTransportCleanup(false);

  // turn off the chip

  if (soc_need_reload_patch) {
    power_manager_.SetPower(false);
    power_manager_.Cleanup();
  } else {
    power_manager_.CloseFd();
  }

  // Stop and delete recovery stuck timer as no crash occured.
  CleanupRecoveryStuckTimer();

  // Cleaning Logger which ensures close of diag port
  Logger::Get()->Cleanup();
  #ifdef DIAG_ENABLED
  if (DiagInterface::GetDiagDeInitTimeoutTriggered())
    return false;
  #endif
  delete health_info;

  init_done_ = false;

  return true;
}

void UartController::ResetInvalidByteCounter()
{
    ALOGV("%s", __func__);
    invalid_bytes_counter_ = 0;
}

void UartController::OnPacketReady(hidl_vec<uint8_t> *data)
{
  if(data == nullptr) {
    ALOGE("Error reading data from uart");
    logger_->SetPrimaryCrashReason(BT_HOST_REASON_ERROR_READING_DATA_FROM_UART);
    ReportSocFailure(false, BT_HOST_REASON_ERROR_READING_DATA_FROM_UART, true, true);
    return;
  }

  if (data->size() > BT_MAX_HCI_PKT_SIZE) {
    ALOGE("Received large pkt %zu from soc trigger ssr",
           data->size());
    SsrCleanup(BT_HOST_REASON_SSR_RCVD_LARGE_PKT_FROM_SOC);
    return;
  }

  if (read_cb_) {
    read_cb_(GetProtocol(hci_packet_type_), hci_packet_type_, data);
  }
  // Get ready for the next type byte.
  hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
}

void UartController::OnDataReady(int fd)
{
  if (hci_packet_type_ == HCI_PACKET_TYPE_UNKNOWN) {
    uint8_t buffer[1] = { 0 };
    size_t bytes_read = TEMP_FAILURE_RETRY(read(fd, buffer, 1));
    CHECK(bytes_read == 1);

    hci_packet_type_ = static_cast<HciPacketType>(buffer[0]);

#ifdef WCNSS_IBS_ENABLED
    if (sibs_enabled_) {
      if (out_of_sync_) {
        if (invalid_bytes_counter_ < TX_RX_PKT_ASC_SIZE)
          inv_bytes[invalid_bytes_counter_] = hci_packet_type_;
        invalid_bytes_counter_++;
        if (hci_packet_type_ == IBS_WAKE_IND) {
          sync_bytes_rcvd_++;
          ALOGE("%s: OutOfSync - Sync Byte Rcvd:%d", __func__, sync_bytes_rcvd_);
          if (sync_bytes_rcvd_ == MIN_BYTES_FOR_SYNC_CONFIRM) {
            ALOGI("%s: Synchronization Achieved, # of invalid bytes:%d",
                  __func__, invalid_bytes_counter_ - sync_bytes_rcvd_);
            // log first few invalid bytes
            state_info_->SetInvBytes(inv_bytes, invalid_bytes_counter_);
            out_of_sync_ = false;
            sync_bytes_rcvd_ = 0;
            invalid_bytes_counter_ = 0;
          }
        } else {
          // print first few invalid bytes
          if (invalid_bytes_counter_ < MAX_INV_BYTES_LOGGED) {
            ALOGE("%s: Invalid packet type rcvd 0x%x, invalid_bytes_counter_ = %d",
                 __func__, hci_packet_type_, invalid_bytes_counter_);
          }
          // restart the sync process
          if (sync_bytes_rcvd_ > 0) {
            ALOGE("%s: Again OutOfSync", __func__);
            sync_bytes_rcvd_ = 0;
          }
        }
        hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
        return;
      } else {
        IbsHandler* hIbs = IbsHandler::Get();

        if (hIbs->IsIbsCmd(hci_packet_type_)) {
          hIbs->ProcessIbsCmd( reinterpret_cast <uint8_t *> (&hci_packet_type_));
          hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
          return;
        }
      }
    }
#endif

    if (!IsHciPacketValid(hci_packet_type_)) {
      ALOGE("%s: Invalid packet type rcvd 0x%x, invalid_bytes_counter_ = %d",
        __func__, hci_packet_type_, invalid_bytes_counter_);

      if (invalid_bytes_counter_ < TX_RX_PKT_ASC_SIZE)
        inv_bytes[invalid_bytes_counter_] = hci_packet_type_;

      invalid_bytes_counter_++;
      hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
      if (!is_cmd_timeout &&
          !is_soc_wakeup_failure &&
          (invalid_bytes_counter_ >= MAX_INVALID_BYTES) &&
#ifdef BT_GANGES_SUPPORTED
          (!soc_crashed) && (!notifysignal_->GetSubSystemSsrStatus())) {
#else
          (!soc_crashed)) {
#endif
        ALOGE("%s: Out Of Synchronization", __func__);
        out_of_sync_ = true;
        SsrCleanup(BT_HOST_REASON_SSR_INVALID_BYTES_RCVD);
#ifdef WCNSS_IBS_ENABLED
        if (sibs_enabled_)
          // give some time for WAKE_INDs from SoC as they are used for resync
          usleep(WAKE_IND_ACCUMULATE_TIME);
#endif
      }
    }
  } else {
    hci_packetizer_.OnDataReady(fd, hci_packet_type_);
#ifdef WCNSS_OBS_ENABLED
    if (sobs_enabled_) {
      ObsHandler* hObs = ObsHandler::Get();
      if (hObs) {
        hObs->obs_spurious_wake_timer_start();
      }
    }
#endif
#ifdef WCNSS_IBS_ENABLED
    if (sibs_enabled_) {
      IbsHandler* hIbs = IbsHandler::Get();
      if (hIbs) {
        hIbs->SetRxDataReceived();
        hIbs->ibs_spurious_wake_timer_start();
      }
    }
#endif
  }
}

bool UartController::IsHciPacketValid(HciPacketType type)
{
  bool result = false;

  if (HCI_PACKET_TYPE_EVENT == type || HCI_PACKET_TYPE_ACL_DATA == type ||
      HCI_PACKET_TYPE_SCO_DATA == type || HCI_PACKET_TYPE_COMMAND == type ||
      HCI_PACKET_TYPE_ANT_CTRL == type || HCI_PACKET_TYPE_ANT_DATA == type ||
      HCI_PACKET_TYPE_FM_CMD == type || HCI_PACKET_TYPE_FM_EVENT == type ||
      HCI_PACKET_TYPE_ISO_DATA == type || HCI_PACKET_TYPE_PERI_EVT == type ||
      HCI_PACKET_TYPE_PERI_CMD == type || HCI_PACKET_TYPE_PERI_DATA == type) {
    result = true;
  }

  return result;
}

SocAddOnFeatures_t* UartController:: GetAddOnFeatureList()
{
  if (add_on_features.feat_mask_len > 0) {
    return &add_on_features;
  } else {
    return NULL;
  }
}

UartController :: ~UartController()
{
  ALOGV("%s", __func__);

  StopRxThreadTimer();
  if (GetRxThreadTimerState() == TIMER_CREATED) {
    timer_delete(rx_timer_state_machine_.timer_id);
    SetRxThreadTimerState(TIMER_NOT_CREATED);
  }
}

void UartController::StartRxThreadTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (GetRxThreadTimerState() == TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))RxThreadTimeOut;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &rx_timer_state_machine_.timer_id);
    if (status == 0)
      SetRxThreadTimerState(TIMER_CREATED);
  }

  if ((GetRxThreadTimerState() == TIMER_CREATED) ||
      (GetRxThreadTimerState() == TIMER_OVERFLOW)) {
    rx_timer_state_machine_.timeout_ms = RX_THREAD_USAGE_TIMEOUT;
    ts.it_value.tv_sec = rx_timer_state_machine_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (rx_timer_state_machine_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(rx_timer_state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to set RxThread Usage timer", __func__);
    else
      SetRxThreadTimerState(TIMER_ACTIVE);
  }

  RxTimerStartTs = std::chrono::steady_clock::now();
}

void UartController::RxThreadTimeOut(union sigval sig)
{
  {
    std::unique_lock<std::mutex> guard(rx_thread_state_mutex_);
    UartController *uart_controller = static_cast<UartController*>(sig.sival_ptr);
    uart_controller->rx_timer_state_machine_.timer_state = TIMER_OVERFLOW;
    RxTimerSchedTs = std::chrono::steady_clock::now();
    ALOGE("%s:Rx thread stuck detected and callback scheduled in TS:%0.2lf ms",
          __func__, uart_controller->GetRxThreadSchedTSDiff());
  }

  {
    std::unique_lock<std::mutex> guard(rx_thread_timer_mutex_);
    UartController *uart_controller = static_cast<UartController*>(sig.sival_ptr);
    if (uart_controller->GetRxThreadTimerState() != TIMER_OVERFLOW) {
      ALOGI("%s: Rx thread is unblocked resuming back", __func__);
      return;
    }

    uart_controller->SsrCleanup(BT_HOST_REASON_RX_THREAD_STUCK);
  }
}

void UartController::StopRxThreadTimer()
{
  int status;
  struct itimerspec ts;

  if (GetRxThreadTimerState() != TIMER_NOT_CREATED) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(rx_timer_state_machine_.timer_id, 0, &ts, 0);
    if(status == -1) { 
      ALOGE("%s:Failed to stop Rx thread timer",__func__);
      return;
    }
    ALOGV("%s: Rx thread timer Stopped",__func__);
    SetRxThreadTimerState(TIMER_CREATED);
  }
}

TimerState UartController::GetRxThreadTimerState()
{
  std::unique_lock<std::mutex> guard(rx_thread_state_mutex_);
  return rx_timer_state_machine_.timer_state;
}

void UartController::SetRxThreadTimerState(TimerState state)
{
  std::unique_lock<std::mutex> guard(rx_thread_state_mutex_);
  rx_timer_state_machine_.timer_state = state;
}

double UartController:: GetRxThreadSchedTSDiff()
{
  return (std::chrono::duration_cast<std::chrono::milliseconds>(RxTimerSchedTs -
          RxTimerStartTs).count());
}

void UartController:: LogPwrSrcsUartFlowCtrl() {
  // Capturing pwr srcs and UART CTS status.
  int cts_status = -1;

  if (is_PwrSrcsLogged) {
      ALOGE("%s: Returning as this function\n", __func__);
      return;
  }

  is_PwrSrcsLogged = true;

  if (hci_transport_) {
    char dst_buff[MAX_BUFF_SIZE];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    memset(dst_buff, 0, MAX_BUFF_SIZE);
    state_info_->AddLogTag(dst_buff, tv, (char *)"UART CTS status = ");
    cts_status = (static_cast<HciUartTransport* >(hci_transport_))->GetUartCtsStatus();
    snprintf(dst_buff + strlen(dst_buff), MAX_BUFF_SIZE - strlen(dst_buff),
             "%d", cts_status);
    BtState::Get()->SetCtsState(dst_buff);
  }
  ALOGE("%s: Captured UART CTS: %d", __func__, cts_status);

  memset(&power_buff, 0, sizeof(power_buff));

  int ret = power_manager_.GetAndLogPwrRsrcStates(&power_buff);

  if (ret < 0) {
    ALOGE("%s: Fail to log pwr srcs info in state file", __func__);
    return;
  }

  ALOGE("%s: %s",__func__, power_buff.bt_buff);
  ALOGE("%s",power_buff.uwb_buff);
  ALOGE("%s",power_buff.platform_buff);

  BtState::Get()->SetPowerResourcesState(&power_buff);

}

uint64_t UartController:: GetChipVersion()
{
  return chipset_ver_;
}

#ifdef BT_CP_CONNECTED
CoPVerSupported UartController::GetCoPVersion()
{
  return cop_ver_supported;
}
#endif

void UartController::StartRecoveryStuckTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGD("%s", __func__);
  if (recovery_timer_state_machine_.timer_state == TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))RecoveryStuckTimeout;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &recovery_timer_state_machine_.timer_id);
    if (status == 0) {
      recovery_timer_state_machine_.timer_state = TIMER_CREATED;
      ALOGD("%s:Stuck recovery timer started", __func__);
    } else {
      ALOGE("%s:Failed to create stuck recovery timer", __func__);
    }
  }

  if (recovery_timer_state_machine_.timer_state == TIMER_CREATED) {
    recovery_timer_state_machine_.timeout_ms = MAX_STUCK_TIME;
    ts.it_value.tv_sec = recovery_timer_state_machine_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (recovery_timer_state_machine_.timeout_ms % 1000);
    ts.it_interval.tv_sec = recovery_timer_state_machine_.timeout_ms / 1000;
    ts.it_interval.tv_nsec = 1000000 * (recovery_timer_state_machine_.timeout_ms % 1000);

    status = timer_settime(recovery_timer_state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to set stuck recovery timer", __func__);
    else {
      recovery_timer_state_machine_.timer_state = TIMER_ACTIVE;
      ALOGD("%s: Stuck recovery timer started", __func__);
    }
  }
}

void UartController::RecoveryStuckTimeout(union sigval ins)
{
  struct timeval cur_tv;
  struct timeval ssr_tv;
  static bool is_cleanup_mandatory = false;
  bool cleanup_hidl = false;
  unsigned long long time_1, time_2;

  /* Already enough time was lapsed, Cleanup directly instead
   * of checking timing difference
   */
  if (is_cleanup_mandatory) {
    cleanup_hidl = true;
    gettimeofday(&cur_tv, NULL);
    goto cleanup;
  }

  if (!Logger::Get()->GetSsrTime(&ssr_tv))
    return;

  gettimeofday(&cur_tv, NULL);
  time_1 = ssr_tv.tv_sec*1e3*1ll + ssr_tv.tv_usec*1e-3*1ll;
  time_2 = cur_tv.tv_sec*1e3*1ll + cur_tv.tv_usec*1e-3*1ll;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    is_cleanup_mandatory  = true;
    return;
  }

  if ((time_2 - time_1) >= MAX_STUCK_TIME) {
    cleanup_hidl = true;
  }

cleanup:
  ALOGE("%s: SSR already occured", __func__);

  if (cleanup_hidl) {
    char dst_buff[MAX_BUFF_SIZE];
    BtState::Get()->AddLogTag(dst_buff, cur_tv,
                      (char *) "Abort/Kill as HIDL recovery is stuck");
    BtState::Get()->SetTsStuckTimeout(dst_buff);
    ALOGE("%s: SSR occured atleast %d ms before, collecting logs and aborting/killing",
            __func__, MAX_STUCK_TIME);

    std::unique_lock<std::recursive_mutex> guard(Logger::bugreport_mutex, std::defer_lock);
    /* Try accuring lock if possible.
     * try_lock is needed here as we dont know whether other thread is holding "bugreport mutex"
     * and it is also blocked indefinitely.
     * So its not possible to wait for "bugreport mutex" lock release otherwise this recovery
     * timer thread also may get blocked.
     */
    guard.try_lock();

    // Check if Diag Log API is stuck and update crash reason accordingly.
    if (BtState :: Get()->isDiagLogApiStuck()) {
      Logger::Get()->SetPrimaryCrashReason(BT_HOST_REASON_DIAG_LOG_API_STUCK);
      Logger::Get()->SetSecondaryCrashReason(BT_SOC_REASON_DEFAULT);
      BtState :: Get()->UpdateBqrRieErrCodeAndErrString(BT_HOST_REASON_DIAG_LOG_API_STUCK,
        std::string(Logger::Get()->GetPrimaryReasonString(BT_HOST_REASON_DIAG_LOG_API_STUCK)));
    } else {
      BtState :: Get()->UpdateBqrRieErrCodeAndErrString(Logger::Get()->GetPrimaryReason(),
        std::string(Logger::Get()->GetPrimaryReasonString(
          Logger::Get()->GetPrimaryReason())));
    }

    Logger::Get()->StoreCrashReason();
    Logger::Get()->PrepareDumpProcess();

    Logger::Get()->CollectDumps(true, true);

#ifdef WCNSS_IBS_ENABLED
    UartController* controller_instance = static_cast<UartController *> (ins.sival_ptr);

    if (controller_instance) {
    /* Do power off.
     * There could be a chance of power leakage if any of the
     * clients are not starting immediately.
     */
    controller_instance->power_manager_.SetPower(false);
    controller_instance->power_manager_.Cleanup();
    }
#endif
#if BT_GANGES_SUPPORTED
    {
      UartController* controller_instance = static_cast<UartController *> (ins.sival_ptr);
      if (controller_instance->soc_type_ == BT_SOC_GANGES)
        controller_instance->ResetHostBaudRate();
    }
#endif
#ifdef USER_DEBUG
    if (!DataHandler::CheckSignalCaughtStatus())
    {
      ALOGE("%s: Aborting process to recover stuck",__func__);
      abort();
    } else {
      // user triggerred reboot, no need to call abort
      ALOGE("%s: killing process to recover stuck", __func__);
      kill(getpid(), SIGKILL);
    }
#else
    ALOGE("%s: killing process to recover stuck",__func__);
    kill(getpid(), SIGKILL);
#endif
  }
}

void UartController::StopRecoveryStuckTimer()
{
  int status;
  struct itimerspec ts;

  if (recovery_timer_state_machine_.timer_state != TIMER_NOT_CREATED) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(recovery_timer_state_machine_.timer_id, 0, &ts, 0);
    if(status == -1) {
      ALOGE("%s:Failed to stop stuck recovery timer",__func__);
      return;
    }
    ALOGD("%s: Stuck recovery timer Stopped",__func__);
    recovery_timer_state_machine_.timer_state = TIMER_CREATED;
  }
}

void UartController::CleanupRecoveryStuckTimer()
{
  if (recovery_timer_state_machine_.timer_state == TIMER_ACTIVE)
    StopRecoveryStuckTimer();
  if (recovery_timer_state_machine_.timer_state == TIMER_CREATED) {
    timer_delete(recovery_timer_state_machine_.timer_id);
    recovery_timer_state_machine_.timer_state = TIMER_NOT_CREATED;
    recovery_timer_state_machine_.timer_id = NULL;
  }
}

bool UartController::CheckForUartFailureStatus() {
  SecondaryReasonCode uart_reason;
  HciUartTransport* instance = static_cast<HciUartTransport*>(hci_transport_);
  if (instance) {
    uart_reason = (SecondaryReasonCode)instance->CheckForUartFailureCode();
    if (uart_reason != UART_REASON_DEFAULT) {
      logger_->SetUartErrCode(uart_reason);
      return true;
    }
  }
  return false;
}

bool UartController::IsSoCCrashNotNeeded(PrimaryReasonCode reason) {
  SecondaryReasonCode sec_reason = logger_->GetSecondaryCrashReasonCode();
  return logger_->DiagInitOnGoing() ||
           sec_reason == BT_HOST_REASON_RX_THREAD_START_STUCK ||
             sec_reason == BT_HOST_REASON_SOC_NAME_UNKOWN ||
              !logger_->CrashCmdNeeded() ||
                 DataHandler::CheckSignalCaughtStatus() ||
                   reason == BT_HOST_REASON_RX_THREAD_STUCK ||
                     reason == BT_HOST_REASON_FAILED_TO_SEND_INTERNAL_CMD;
}

void UartController::SetCleanupStatusDuringSSR() {
  std::unique_lock<std::mutex> guard(check_cleanup_mutex_);
  if (Logger::Get()->isSsrTriggered()) {
    cleanup_status_ssr_ = true;
  }
}

bool UartController::CheckCleanupStatusDuringSSR() {
  std::unique_lock<std::mutex> guard(check_cleanup_mutex_);
  return cleanup_status_ssr_;
}

bool UartController::ResetForceSsrTriggeredIfNoCleanup() {
  std::unique_lock<std::mutex> guard(check_cleanup_mutex_);
  if (!cleanup_status_ssr_) {
    logger_->ResetForceSsrTriggeredFlag();
    return true;
  }
  return false;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
