/*==========================================================================
Description
 It has implementation for Signaling Power driver class

# Copyright (c) 2024 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.


===========================================================================*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <fcntl.h>
#include <string.h>
#include <asm-generic/ioctls.h>
#include <hidl/HidlSupport.h>
#include <patch_dl_manager.h>
#include <peri_patch_dl_manager.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <termios.h>
#include "notify_signal.h"

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-notify_signal"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-notify_signal"
#endif

// power driver signal Number
#define POWER_DRIVER_SIG SIGIO

// Power driver signal status
#define OOBS_SINGAL         0x00010000
#define POWER_DRIVER_SIGNAL 0x00020000
#define SOC_ACCESS_SIGNAL   0x00040000
#define SIGNAL_STATUS_MASK  0x0000FFFF
#define REQUEST_SOC_ACCESS  0x01
#define RELEASE_SOC_ACCESS  0x02
#define SOC_ACCESS_TIMEOUT 2800
#define SOC_CRASH_WAIT_TIMEOUT 7000

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

NotifySignal * NotifySignal :: instance_ = NULL;
SoCAccessWaitState soc_access_state = ACCESS_GRANTED;

ssr_states ss_ssr_state = UWB_SSR_STATE_IDLE;

std::mutex wait_evt_mutex_;
std::condition_variable wait_evt_cv;

NotifySignal * NotifySignal :: Get()
{
  if (!instance_) {
    instance_ = new NotifySignal();
  }
  return instance_;
}

NotifySignal::NotifySignal()
{
  notifySubsystem = false;
}

NotifySignal::~NotifySignal()
{
 ALOGD("%s: Distructor", __func__);
}

int NotifySignal::GetSubSystemSsrStatus(void) {
  return ss_ssr_state;
}

void NotifySignal::UpdateSoCAccessState(SoCAccessWaitState state)
{
  std::unique_lock<std::mutex> guard(wait_evt_mutex_);
  soc_access_state = state;
  wait_evt_cv.notify_all();
}

void NotifySignal::SoCAccessSigHandler(SoCAccessWaitState access_state)
{
  ALOGD("%s: start %d", __func__, access_state);
  switch(access_state) {
    case ACCESS_GRANTED: {
      ALOGD("%s: ACCESS_GRANTED", __func__);
      UpdateSoCAccessState(ACCESS_GRANTED);
      break;
    } 
  }
  ALOGD("%s: end %d", __func__, access_state);
}

void NotifySignal::PowerDriverSignalHandler(int uwb_ssr_state) {
  switch(ss_ssr_state)
  {
    case UWB_SSR_STATE_IDLE:
      if (uwb_ssr_state == SSR_ON_UWB) {
        ALOGE("%s: Signal status received is SSR_ON_UWB\n", __func__);
        ss_ssr_state = SSR_ON_UWB;
        UpdateSoCAccessState(SSR_ON_OTHER_CLIENT);
        DataHandler *data_handler = DataHandler::Get();
        if (data_handler && data_handler->controller_ != nullptr) {
          UartController * instance = (UartController *)data_handler->controller_;
          Logger::Get()->SetSsrTriggeredFlag();
	  instance->StartSocCrashWaitTimer(UWB_SSR_TIMEOUT);
        }
      }
      else if (uwb_ssr_state == UWB_SSR_COMPLETED) {
        ALOGE("%s: Invalid Signal (UWB_SSR_COMPLETED) status received, Discarded\n",
            __func__);
      }
      break;
    case SSR_ON_UWB:
      if (uwb_ssr_state == SSR_ON_UWB) {
        ALOGE("%s: Invalid Signal (SSR_ON_UWB) status received, Discarded\n",
            __func__);
      }
      else if (uwb_ssr_state == UWB_SSR_COMPLETED) {
        ALOGE("%s: Signal status received is UWB_SSR_COMPLETED\n", __func__);
        ss_ssr_state = UWB_SSR_COMPLETED;
        DataHandler *data_handler = DataHandler::Get();
        if (data_handler && data_handler->controller_ != nullptr) {
          UartController * instance = (UartController *)data_handler->controller_;
          instance->StopSocCrashWaitTimer();
          Logger::Get()->SetasFpissue();
          instance->ReportSocFailure(false, BT_HOST_REASON_PERI_SOC_CRASHED_ON_OTHER_SUB_SYSTEM, true, false);
        }
        UpdateSoCAccessState(SSR_ON_OTHER_CLINET_COMPLETED);
      }
      break;
    case UWB_SSR_COMPLETED:
      if (uwb_ssr_state == SSR_ON_UWB) {
        ALOGE("%s: Discarded the signal(SSR_ON_UWB), as System is already doing Silent Recovery\n",
            __func__);
      }
      else if (uwb_ssr_state == UWB_SSR_COMPLETED) {
        ALOGE("%s: Discarded the signal(UWB_SSR_COMPLETED), as System is already doing Silent Recovery\n",
            __func__);
      }
      break;
  }
}

void NotifySignal::SigIOSignalHandler(int signum, siginfo_t *info, void *unused) {

  int sig_int = 0;
  if (info == NULL){
    ALOGE("%s: Received the NULL in Siginfo Handler", __func__);
    return;
  }

  ALOGV("%s: si_signo: %#x si_errno: %#x si_code: %#x si_int: %#x",
      __func__, info->si_signo, info->si_errno, info->si_code, info->si_int);

  sig_int = info->si_int;

  if (sig_int & POWER_DRIVER_SIGNAL) {
    PowerDriverSignalHandler(sig_int & SIGNAL_STATUS_MASK);
  } else if (sig_int & OOBS_SINGAL) {
#ifdef WCNSS_OBS_ENABLED
    ObsHandler* hObs = ObsHandler::Get();
    if (hObs) {
      hObs->ProcessObsCmd(sig_int & SIGNAL_STATUS_MASK);
      return;
    }
    ALOGE("%s: Oobs Handler destroyed. Discarding the signal\n", __func__);
#else
    ALOGE("%s: Oobs Disabled, Discarding the signal\n", __func__);
#endif
  } else if (sig_int & SOC_ACCESS_SIGNAL) {
    SoCAccessSigHandler((SoCAccessWaitState)(sig_int & SIGNAL_STATUS_MASK)); 
  } else {
    ALOGE("%s: Invalid signal status recvied, discarding it\n", __func__);
  }
}

void NotifySignal::RegSigIOCallBack(void) {
  struct sigaction SigHandler;
  ALOGI("%s: Entry", __func__);
  memset(&SigHandler, 0, sizeof(SigHandler));
  SigHandler.sa_sigaction = SigIOSignalHandler;
  sigemptyset(&SigHandler.sa_mask);
  SigHandler.sa_flags = (SA_SIGINFO | SA_RESTART);
  sigaction(POWER_DRIVER_SIG, &SigHandler, NULL);
  ALOGI("%s: Done", __func__);
}

bool NotifySignal :: RegisterService(void) {
  int status = REGISTER_BT_PID;
  int ret = 0;

  ALOGD("%s with Power Driver", __func__);

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler != NULL) {
    ret = ioctl(data_handler->GetPowerDriverFd(),
                notify_signal_cmd_, (unsigned long)status);
  }

  if (ret < 0) {
    return false;
  }
  return true;
}

bool NotifySignal :: NotifyDriver(int SsrState)
{
  int ret = 0;
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler != NULL) {
    ret = ioctl(data_handler->GetPowerDriverFd(),
                notify_signal_cmd_, (unsigned long)SsrState);
  }

  if (ret < 0) {
    ALOGE("Failed to Notify PowerDriver ret=%d error =(%s)",
      ret, strerror(errno));
	return false;
  }

  ALOGE("%s: Notifying BT_SSR status to PowerDriver successfull\n",
    __func__);

  return true;
}

SoCAccessState NotifySignal::ReleaseSoCAccess(void)
{
  SoCAccessState status = SOC_ACCESS_DISALLOWED;
  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(),
			soc_access_cmd, (unsigned long)RELEASE_SOC_ACCESS);

  if (ret < 0) {
    ALOGE(" ioctl failed to request peri access :%d error =(%s)", ret, strerror(errno));
    status = SOC_ACCESS_DISALLOWED;
  } else if (ret == 2) {
    status = SOC_ACCESS_RELEASED;
  }
  return status;
}

SoCAccessState NotifySignal::RequestSoCAccess(void)
{
  SoCAccessState status = SOC_ACCESS_DISALLOWED;
  /* Reset state to avoid race conditions between waiting thread
   * and signal handler.
   */
  {
    std::unique_lock<std::mutex> guard(wait_evt_mutex_);
    soc_access_state = WAITING_FOR_SOC_ACCESS;
  }
  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(),
		  soc_access_cmd, (unsigned long)REQUEST_SOC_ACCESS);

  if (ret < 0) {
    ALOGE(" ioctl failed to request peri access :%d error =(%s)", ret, strerror(errno));
    status = SOC_ACCESS_DISALLOWED;
  } else if (ret == 0) {
    status = SOC_ACCESS_GRANTED;
  } else if (ret == 1) {
    status = SOC_ACCESS_DENIED;
  }
  return status;
}

bool NotifySignal::WaitForSoCAccess(void)
{
  bool status = false;

  std::unique_lock<std::mutex> guard(wait_evt_mutex_);
  wait_evt_cv.wait_for(guard, std::chrono::milliseconds(SOC_ACCESS_TIMEOUT), []
		      {return soc_access_state;});
  if (soc_access_state == ACCESS_GRANTED) {
    ALOGI("%s: SoC access granted", __func__);
    status = true;
  } else if (soc_access_state == WAITING_FOR_SOC_ACCESS) {
    ALOGI("%s: Timeout triggered", __func__);
  } else if (soc_access_state == SSR_ON_OTHER_CLIENT) {
    ALOGI("%s: SSR_ON_OTHER_CLIENT. increase timeout", __func__);
    wait_evt_cv.wait_for(guard, std::chrono::milliseconds(SOC_CRASH_WAIT_TIMEOUT), []
		      {return soc_access_state;});
    if (soc_access_state == SSR_ON_OTHER_CLINET_COMPLETED) {
      ALOGI("%s: SSR_ON_OTHER_CLINET_COMPLETED", __func__);
    } else if (soc_access_state == SSR_ON_OTHER_CLIENT) {
      ALOGI("%s: Timeout triggered when waiting for SSR_ON_OTHER_CLIENT to complete", __func__);
    }
  }

  return status;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
