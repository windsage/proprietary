/*==========================================================================
Description
  This file taps regular BT UART data (hci command, event, ACL packets)
  and writes into DiagInterface. It also writes the controller logs (controller's
  printf strings, LMP RX and TX data) received from BT SOC over the UART.

# Copyright (c) 2013, 2016-2017, 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc..

===========================================================================*/
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "diag_interface.h"
#ifdef WCNSS_OBS_ENABLED
#include "obs_handler.h"
#else
#include "ibs_handler.h"
#endif
#include <cutils/properties.h>
#include "state_info.h"
#include <unistd.h>
#include "data_handler.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-diag_interface"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-diag_interface"
#endif

#define DIAG_SSR_BT_CMD     0x0007
#define PRINT_BUF_SIZE      (260 * 3 + 2)

/* QXDM ID for LMP packers */
#define LOG_BT_DIAG_LMP_LOG_ID 0x1041
#define LOG_BT_DIAG_LMP_RX_ID  0x1048
#define LOG_BT_DIAG_LMP_TX_ID  0x1049

/* To format LMP logs */
#define LOG_BT_QXDM_PKT_LENGTH_POS    0
#define LOG_BT_QXDM_PKT_LENGTH2_POS   1
#define LOG_BT_QXDM_DEVICE_IDX_POS    2
#define LOG_BT_QXDM_PKT_POS           3

#define LOG_BT_DBG_DEVICE_IDX_POS 0
#define LOG_BT_DBG_PKT_LENGTH_POS 1
#define LOG_BT_DBG_PKT_POS 2

#define DIAG_CMD_SSR_SW_ERR_FAULT           38
#define DIAG_CMD_SSR_SW_EXCPTN_DIV_ZERO     39
#define DIAG_CMD_SSR_SW_EXCPTN_NULL_PTR     40
#define DIAG_CMD_SSR_WATCHDOG               41

#ifdef BT_GANGES_SUPPORTED
/* Peri Diag commands */
#define HCI_PERI_ERROR_FATAL_CMD           42
#define HCI_PERI_EXCEPTION_DIV_ZERO_CMD    43
#define HCI_PERI_EXCEPTION_NULL_PTR_CMD    44
#define HCI_PERI_WDOG_BITE_CMD             45
#endif

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

typedef struct {
  log_hdr_type hdr;
  byte data[1];
} bt_log_pkt;

const uint16_t DiagInterface::log_bt_hdr_size_ = (sizeof(bt_log_pkt) - 1);
bool DiagInterface :: is_cleanup_in_progress_ = false;
bool DiagInterface :: is_forced_ssr_triggered_ = false;
bool DiagInterface :: is_diag_ssr_triggered_ = false;
#ifdef BT_GANGES_SUPPORTED
bool DiagInterface :: is_diag_triggered_for_peri_ = false;
#endif
bool DiagInterface :: isDiagDeinitTimeout = false;
bool DiagInterface :: is_internal_cmd_timeout_ = false;

unsigned int DiagInterface :: cleanup_status = 0;

DiagInterface :: DiagInterface()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_diag_ssr_triggered_ = false;
#ifdef BT_GANGES_SUPPORTED
  is_diag_triggered_for_peri_ = false;
#endif
  is_cleanup_in_progress_ = false;
  is_forced_ssr_triggered_ = false;
  is_internal_cmd_timeout_ = false;
  cleanup_status = 0;
  diag_init_ongoing = false;
  hci_transport_ = NULL;
#ifdef DIAG_ENABLED
  memset (&diag_deinit_timer_state_, 0, sizeof(DiagDeinitTimer));
#endif
}

bool DiagInterface :: Init(HciTransport *hci_transport)
{

#ifdef DIAG_ENABLED
  if (isSsrTriggered()) {
    ALOGD("%s: skipping Diag init as SSR triggerred", __func__);
    return false;
  }
#endif

  hci_transport_ = hci_transport;
  std::unique_lock<std::mutex> guard(diag_mutex);
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  diag_init_ongoing = false;
#ifdef DIAG_ENABLED
  /* Explicitly set init status to false untill Diag_LSM_Init() pass */
  init_status = false;
  diag_deinit_timer_state_.timer_created = false;
  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_DIAGINIT_STUCK, "DIAG INIT");
  BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Pre-Diag_LSM_Init");
  BtState::Get()->SetDiagInitPreTS(dst_buff);
  /* unlocking the diag mutex inorder to avoid blockage for other threads
   * if diag init get stuck.
   */
  guard.unlock();
  diag_init_ongoing = true;
  init_status = Diag_LSM_Init(NULL);
  diag_init_ongoing = false;
  gettimeofday(&tv, NULL);
  BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Post-Diag_LSM_Init");
  BtState::Get()->SetDiagInitPostTS(dst_buff);
  Logger::Get()->CheckAndAddToDelayList(&tv);
  guard.lock();
  if (init_status) {
    ALOGI("%s: Init diag completed", __func__);
  } else {
    ALOGE("%s:Failed to Init diag", __func__);
  }

  return init_status;
#endif

  return true;
}

#ifdef DIAG_ENABLED
bool DiagInterface :: BT_Diag_LSM_DeInit()
{
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  ALOGD("%s", __func__);

  if (init_status) {
   /* In some cases, diag deinit is stuck to return which
    * caused blocking BT clean up during BT off procedure.
    * For this we have started timer to limit the time taken
    * by diag deinit procedure. In case it takes more than
    * 1 sec then we call diag timeout procedure which will
    * Kill/abort the whole HIDL process.
    */
    StartDiagDeinitTimeoutTimer();
    ALOGD("%s: Pre-Diag_LSM_DeInit", __func__);
    gettimeofday(&tv, NULL);
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Pre-Diag_LSM_DeInit");
    BtState::Get()->SetDiagDeInitPreTS(dst_buff);
    Logger::Get()->SetPrimaryCrashReason(BT_HOST_REASON_SOC_DEINIT_STUCK);
    Logger::Get()->SetSecondaryCrashReason(BT_HOST_REASON_DIAGDEINIT_STUCK);
    deinit_status = Diag_LSM_DeInit();
    ALOGD("%s: Post-Diag_LSM_DeInit", __func__);
    gettimeofday(&tv, NULL);
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Post-Diag_LSM_DeInit");
    BtState::Get()->SetDiagDeInitPostTS(dst_buff);
    if (deinit_status)
    {
      init_status = false;
    }
    StopDiagDeinitTimeoutTimer();
  }

  return deinit_status;
}

void DiagInterface::StartDiagDeinitTimeoutTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGI("%s", __func__);
  if (diag_deinit_timer_state_.timer_created == false) {
    se.sigev_notify_function = (void (*)(union sigval))DiagDeinitTimedOut;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr =  NULL;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &diag_deinit_timer_state_.timer_id);
    if (status == 0)
      diag_deinit_timer_state_.timer_created = true;
    else
      ALOGE("%s: Failed to create diagdeinit timeout timer", __func__);
  }

  if (diag_deinit_timer_state_.timer_created == true) {
    diag_deinit_timer_state_.timeout_ms = 1000;
    ts.it_value.tv_sec = diag_deinit_timer_state_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (diag_deinit_timer_state_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(diag_deinit_timer_state_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to set diag deinit timer", __func__);
  }
}

void DiagInterface ::DiagDeinitTimedOut(union sigval  /*sig*/ )
{
  ALOGE("%s",__func__);
  SetDiagDeInitTimeoutTriggered();
#ifdef USER_DEBUG
  if (!DataHandler::CheckSignalCaughtStatus())
  {
    BtState ::Get()->UpdateBqrRieErrCodeAndErrString(
      Logger::Get()->GetSecondaryCrashReasonCode(),
        std::string(Logger::Get()->GetSecondaryCrashReason()));

    Logger::Get()->PrepareDumpProcess();
    Logger::Get()->StoreCrashReason();
    Logger::Get()->CollectDumps(true, true);
    ALOGE("%s: Aborting process as diag deinit is stuck",__func__);
    abort();
  } else {
    // user triggerred reboot, no need to call abort
    ALOGE("%s: killing process as diag deinit is stuck", __func__);
    kill(getpid(), SIGKILL);
  }
#else
  ALOGE("%s: killing process as diag deinit is stuck",__func__);
  kill(getpid(), SIGKILL);
#endif

}
inline void DiagInterface :: SetDiagDeInitTimeoutTriggered()
{
  isDiagDeinitTimeout = true;
}

bool DiagInterface :: GetDiagDeInitTimeoutTriggered()
{
  return isDiagDeinitTimeout;
}


void DiagInterface::StopDiagDeinitTimeoutTimer()
{
  int status;
  struct itimerspec ts;
  ALOGI("%s",__func__);
  if(diag_deinit_timer_state_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(diag_deinit_timer_state_.timer_id, 0, &ts, 0);
    if(status == -1)
      ALOGE("%s:Failed to stop diag deinit timer",__func__);
    else if(status == 0)
      ALOGV("%s: Diag deinit timer Stopped",__func__);
  }
}
#endif

void DiagInterface :: CleanUp()
{
  hci_transport_ = nullptr;
  is_diag_ssr_triggered_ = false;
#ifdef BT_GANGES_SUPPORTED
  is_diag_triggered_for_peri_ = false;
#endif
  is_forced_ssr_triggered_ = false;
  is_internal_cmd_timeout_ = false;
  deinit_status = false;

#ifdef DIAG_ENABLED
  if (init_status) {
    if (BT_Diag_LSM_DeInit()) {
      ALOGD("Diag_LSM_DeInit SUCCEED");
    } else {
      ALOGE("Diag_LSM_DeInit FAILED");
    }
  }
#endif
}

#ifdef DIAG_ENABLED
void DiagInterface :: SendLogs(const uint8_t *buff, int len, int type)
{
  if (buff == NULL)
    return;

  int ret;

  ret = log_status(type);
  if (ret) {
    char timebuff[8];
    bt_log_pkt*  bt_log_pkt_ptr = NULL;

    GetTime(timebuff);

    bt_log_pkt_ptr = (bt_log_pkt*)log_alloc(type, log_bt_hdr_size_ + len);

    if (bt_log_pkt_ptr != NULL) {
      if (type == LOG_BT_DIAG_LMP_RX_ID || type == LOG_BT_DIAG_LMP_TX_ID)
        FormatLmp(bt_log_pkt_ptr->data, (uint8_t*)buff, len);
      else
        memcpy(bt_log_pkt_ptr->data, (void*)buff, len);

      //((log_header_type *)&(bt_log_pkt_ptr->hdr))->ts_lo = *(uint32 *)&timebuf[0];
      //((log_header_type *)&(bt_log_pkt_ptr->hdr))->ts_hi = *(uint32 *)&timebuf[4];

      log_commit(bt_log_pkt_ptr);
    } else {
      ALOGE(": log_alloc returned NULL");
    }
  }
}

void DiagInterface :: GetTime(char *timebuff)
{
  char *temp;
  struct timeval tv;
  uint64_t microseconds;

  gettimeofday(&tv, NULL);

  microseconds = (uint64_t)tv.tv_sec * 1000;
  microseconds += ((uint64_t)tv.tv_usec / 1000);
  microseconds *= 4;
  microseconds = microseconds / 5;
  microseconds = microseconds << 16;

  temp = (char*)(&microseconds);

  for (unsigned long i = 0; i < sizeof(microseconds); i++)
    *(timebuff + i) = *(temp + i);
}

void DiagInterface :: FormatLmp(uint8 *dst, uint8 *src, int len)
{
  dst[LOG_BT_QXDM_PKT_LENGTH_POS] = src[LOG_BT_DBG_PKT_LENGTH_POS];
  dst[LOG_BT_QXDM_PKT_LENGTH2_POS] = 0x0;

  dst[LOG_BT_QXDM_DEVICE_IDX_POS] = src[LOG_BT_DBG_DEVICE_IDX_POS];

  memcpy( (dst + LOG_BT_QXDM_PKT_POS), (src + LOG_BT_DBG_PKT_POS), (len - LOG_BT_QXDM_PKT_POS) );
}

void * DiagInterface :: SsrBtHandler(void *req_pkt, uint16_t pkt_len)
{
  {
    std::unique_lock<std::mutex> guard(diag_mutex);
    if (is_diag_ssr_triggered_ || is_forced_ssr_triggered_ ||
        is_internal_cmd_timeout_) {
      ALOGE("%s: SSR is in progress - returning!", __func__);
      return NULL;
    } else if (is_cleanup_in_progress_) {
      ALOGE("%s: HAL Cleanup in progress - returning!", __func__);
      return NULL;
    }
  }

  void *rsp = NULL;
  unsigned char *pkt_ptr = (unsigned char*)req_pkt + 4;
  int i, retval = 0;
  unsigned short p_len, p_opcode;
#ifdef WCNSS_IBS_ENABLED
  bool ibs_enabled = IbsHandler::IsEnabled();
#else
  bool ibs_enabled = false;
#endif

#ifdef WCNSS_OBS_ENABLED
  bool obs_enabled = ObsHandler::IsEnabled();
#else
  bool obs_enabled = false;
#endif

  char data_buf[PRINT_BUF_SIZE] = { 0, };

  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_BT, DIAG_SSR_BT_CMD, pkt_len);
  if (!rsp) {
    ALOGE("%s: diagpkt_subsys_alloc returned null", __func__);
    return rsp;
  }
#ifdef BT_GANGES_SUPPORTED
  /* handle peri diag cmd */
  if (pkt_ptr[0] == HCI_PACKET_TYPE_PERI_CMD) {
    p_len = *(pkt_ptr + 4); /* VS Command packet length */
    p_opcode = (*(pkt_ptr + 3) << 8) | *(pkt_ptr + 2);

    ALOGI("%s : p_len: %d, pkt_len -8: %d, p_opcode:%.04x",
          __func__, p_len, pkt_len - 9, p_opcode);

    /* Check if packet is valid */
    if (p_len != (pkt_len - 9) || ( p_opcode != HCI_VS_PERI_DEBUG_OPCODE))
      return rsp;

    if ((*(pkt_ptr + 5) < 1) ||(*(pkt_ptr + 5) > 4)) {
      ALOGW("Received Invalid last byte for SSR : %.02x ", *(pkt_ptr + 4));
      return rsp;
    }

    for (i = 0; (i < (p_len + 5) && (i * 3 + 2) < PRINT_BUF_SIZE); i++)
      snprintf((char*)data_buf, PRINT_BUF_SIZE, "%s %.02x ", (char*)data_buf, *(pkt_ptr + i));

    ALOGD("Received Peri data : %s", data_buf);
    {
       std::unique_lock<std::mutex> guard(diag_mutex);
       is_diag_ssr_triggered_ = true;
       is_diag_triggered_for_peri_ = true;
       /* ToDo Can't we have handler pri SSR triggered with a different flag */
       BtState::Get()->SetSSRtrigger(true);

#ifdef WCNSS_IBS_ENABLED
       /* Send VS Command from DiagInterface input */
       /* Send wake up byte to controller */
       if (IbsHandler::Get() && ibs_enabled)
         IbsHandler::Get()->WakeAssert();
#endif

#ifdef WCNSS_OBS_ENABLED
       /* Send wake up byte to controller */
       if (ObsHandler::Get() && obs_enabled)
         ObsHandler::Get()->WakeAssert();
#endif

       if (hci_transport_ != nullptr) {
         retval = write(hci_transport_->GetCtrlFd(), pkt_ptr, (p_len + 5));
         if (retval < 0) {
           ALOGE("%s:error in writing buf: %d: %s", __func__, retval, strerror(errno));
           ResetSsrTriggeredFlag();
         } else {
           Logger::Get()->SetRecoveryStartTime();
         }
       }
     }
  } else {
#endif
     p_len = *(pkt_ptr + 3); /* VS Command packet length */
     p_opcode = (*(pkt_ptr + 2) << 8) | *(pkt_ptr + 1);

     ALOGI("%s : p_len: %d, pkt_len -8: %d, p_opcode:%.04x \n",
           __func__, p_len, pkt_len - 8, p_opcode);

     if (p_len != (pkt_len - 8) || ( p_opcode != 0xFC00 && p_opcode != 0xFC0C))
       return rsp;
   
     for (i = 0; (i < (p_len + 4) && (i * 3 + 2) < PRINT_BUF_SIZE); i++)
       snprintf((char*)data_buf, PRINT_BUF_SIZE, "%s %.02x ", (char*)data_buf, *(pkt_ptr + i));

     ALOGD("Received BT data : %s", data_buf);

     if ((*(pkt_ptr + 4) < DIAG_CMD_SSR_SW_ERR_FAULT) ||
        (*(pkt_ptr + 4) > DIAG_CMD_SSR_WATCHDOG)) {
       ALOGW("Received Invalid last byte for SSR : %.02x ", *(pkt_ptr + 4));
       return rsp;
     }
     {
       std::unique_lock<std::mutex> guard(diag_mutex);
       is_diag_ssr_triggered_ = true;

       /* ToDo Can't we have handler pri SSR triggered with a different flag */
       BtState::Get()->SetSSRtrigger(true);

#ifdef WCNSS_IBS_ENABLED
       /* Send VS Command from DiagInterface input */
       /* Send wake up byte to controller */
       if (IbsHandler::Get() && ibs_enabled)
         IbsHandler::Get()->WakeAssert();
#endif

#ifdef WCNSS_OBS_ENABLED
       /* Send wake up byte to controller */
       if (ObsHandler::Get() && obs_enabled)
         ObsHandler::Get()->WakeAssert();
#endif

       if (hci_transport_ != nullptr) {
         retval = write(hci_transport_->GetCtrlFd(), pkt_ptr, (p_len + 4));
         if (retval < 0) {
           ALOGE("%s:error in writing buf: %d: %s", __func__, retval, strerror(errno));
           ResetSsrTriggeredFlag();
         } else {
           Logger::Get()->SetRecoveryStartTime();
         }
       }
     }
#ifdef BT_GANGES_SUPPORTED
  }
#endif
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler && isDiagSsrTriggered())
    data_handler->StartSocCrashWaitTimer();

  memcpy(rsp, req_pkt, pkt_len);

#ifdef WCNSS_IBS_ENABLED
  if (IbsHandler::Get() && ibs_enabled)
    IbsHandler::Get()->TransmitDone(true);
#endif

#ifdef WCNSS_OBS_ENABLED
   if (ObsHandler::Get() && obs_enabled)
     ObsHandler::Get()->TransmitDone(true);
#endif

  return (void*)rsp;
}
#endif

bool DiagInterface :: isSsrTriggered()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  return (is_diag_ssr_triggered_ || is_forced_ssr_triggered_);
}

bool DiagInterface :: isDiagSsrTriggered()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  return is_diag_ssr_triggered_;
}

#ifdef BT_GANGES_SUPPORTED
bool DiagInterface :: isDiagSsrTriggeredForPeri()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  return is_diag_triggered_for_peri_;
}
#endif

void DiagInterface :: ResetSsrTriggeredFlag()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_diag_ssr_triggered_ = false;
}

bool DiagInterface :: SetSsrTriggeredFlag()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  if (is_diag_ssr_triggered_ == true || is_cleanup_in_progress_ == TRUE
    || is_forced_ssr_triggered_ == true )
    return false;
  is_forced_ssr_triggered_ = true;
  return true;
}

void DiagInterface :: SignalEndofCleanup(ProtocolType ptype)
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_cleanup_in_progress_ = false;
  cleanup_status = (cleanup_status &  (~(0x01 << ptype)));
}

bool DiagInterface :: SignalHALCleanup(ProtocolType ptype)
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  if (is_diag_ssr_triggered_ == true || is_cleanup_in_progress_ == TRUE
    || is_forced_ssr_triggered_ == true )
    return false;
  is_cleanup_in_progress_ = true;
  cleanup_status = (cleanup_status | (0x01 << ptype));
  return true;
}

bool DiagInterface :: GetCleanupStatus(ProtocolType ptype)
{
  bool status;
  std::unique_lock<std::mutex> guard(diag_mutex);
  status = (cleanup_status & (0x01 << ptype));
  return status;
}

#ifdef DIAG_ENABLED
bool DiagInterface :: GetDiagInitStatus()
{
  return init_status;
}
#endif

void DiagInterface :: UnlockDiagMutex()
{
  ALOGD("%s: unlocking diag mutex", __func__);
  diag_mutex.unlock();
}

DiagInterface :: ~DiagInterface()
{
#ifdef DIAG_ENABLED
  if (diag_deinit_timer_state_.timer_created == true) {
    timer_delete(diag_deinit_timer_state_.timer_id);
    diag_deinit_timer_state_.timer_created = false;
  }
#endif
}

void DiagInterface :: ResetForceSsrTriggeredFlag()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_forced_ssr_triggered_ = false;
}

void DiagInterface :: ResetCleanupflag()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_cleanup_in_progress_ = false;
}

void DiagInterface :: SetInternalCmdTimeout()
{
  std::unique_lock<std::mutex> guard(diag_mutex);
  is_internal_cmd_timeout_ = true;
}

bool DiagInterface :: DiagInitOnGoing() {
  return diag_init_ongoing;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
