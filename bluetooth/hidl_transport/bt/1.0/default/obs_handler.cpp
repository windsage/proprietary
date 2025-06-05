/*==========================================================================
Description
  Wcnss_obs has the state-machine and logic to process GPIO wakeup to excercise
  OOBS (Out-of-band sleep) protocol

# Copyright (c) 2021-2022, 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifdef WCNSS_OBS_ENABLED
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <asm-generic/ioctls.h>
#include <hidl/HidlSupport.h>
#include <signal.h>
#include "obs_handler.h"
#include "hci_uart_transport.h"
#include "uart_controller.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-obs_handler"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-obs_handler"
#endif

#define OBS_HOSTWAKE_SIG SIGIO
#define OBS_CURRENT_TASK _IOW('a','a',int32_t*)

#define TX_IDLE_DELAY           (10)
#define UART_VOTEOFF_DELAY      (100)
#define WAKELOCK_RELEASE_DELAY  (150)
#define CLOCK_OFF               (0)
#define CLOCK_ON                (1)
#define DEV_GPIO_OFF            (2)
#define DEV_GPIO_ON             (3)

// Overall SoC WakeUp timeout is configured 175ms more than
// the timer to detect Rx thread stuck scenario. This is to ensure that
// SoC crash during SoC WakeUp scenario is initiated only if RxThread stuck
// issue is not encountered. If RxThread is stuck, then crash dumps from SoC
// can't be processed.
#define NUM_WACK_RETRANSMIT      ((RX_THREAD_USAGE_TIMEOUT)/(TX_IDLE_DELAY) + 25)
#define DEFAULT_LPM_IDLE_TIMEOUT (1000)
#define SLEEP_IND_WAIT_MS        (1000)

#define UART_SPURIOUS_WAKE_SURPRESS_DELAY      (10*1000)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

std::mutex obs_mutex_;
extern bool soc_need_reload_patch;
ObsHandler *ObsHandler::instance_ = NULL;

ObsHandler *ObsHandler::Init(HciUartTransport* theTransport, HealthInfoLog* theHealthInfo,
           Controller* theController)
{
  if (!instance_) {
    instance_ = new ObsHandler(theTransport, theHealthInfo, theController);
  }
  instance_->InitInt();
  return instance_;
}

void ObsHandler::InitInt()
{
  int status = 0;

  memset(&bt_lpm_, 0, sizeof(bt_lpm_));
  memset(&voteoff_timer_, 0, sizeof(voteoff_timer_));
  memset(&obs_spurious_wake_struct, 0, sizeof(obs_spurious_wake_struct));
  bt_lpm_.timeout_ms = DEFAULT_LPM_IDLE_TIMEOUT;

  tty_fd_ = uart_transport_->GetCtrlFd();
  ALOGD("%s: tty_fd = %d", __func__, tty_fd_);

  /* SoC Rx state is ASLEEP, when soc always ON is enabled.
   * Because during HIDL close, SoC is sent to sleep state.
   * Soc Rx state is AWAKE when SoC is booting up.
   */
  memset(&state_machine_, 0, sizeof(state_machine_));
  if (soc_need_reload_patch) {
    state_machine_.CLK_STATE = CLOCK_OFF;
    state_machine_.rx_obs_state = OBS_RX_ASLEEP;
    SetRxVote(0);
  }
  read_OBS_thread_ = std::thread([this]() {
    this->read_OBS_thread_running_ = true;
    while(read_OBS_thread_running_) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
  if (!read_OBS_thread_.joinable()) {
    read_OBS_thread_running_ = false;
    ALOGE("read OBSthread is not joinable");
  }


  num_try_ = 0;

  struct sigevent se;
  se.sigev_notify_function = VoteOffTimeout;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &voteoff_timer_.timer_id;
  se.sigev_notify_attributes = NULL;

  status = timer_create(CLOCK_MONOTONIC, &se, &voteoff_timer_.timer_id);
  if (status == 0) {
    ALOGV("%s: Vote off timer created", __func__);
    voteoff_timer_.timer_created = true;
  }

  delayed_wakelock = WAKELOCK_RELEASE_DELAY;

  time_last_rx_sleep = 0;
  time_last_tx_packet = 0;
}

ObsHandler * ObsHandler::Get()
{
  return instance_;
}

void ObsHandler::CleanUp(void)
{
  std::unique_lock<std::mutex> guard(obs_mutex_);

  ALOGV("%s", __func__);
  if (instance_) {
    // FIXME
    if (instance_->read_OBS_thread_running_) {
      ALOGD("%s: Wait for OBS_thread stopping", __func__);
      instance_->read_OBS_thread_running_ = false;
      instance_->read_OBS_thread_.join();
    }
    ALOGD("%s: Closing Driver", __func__);
    if (instance_->obs_fd_ >= 0) close(instance_->obs_fd_);
    delete instance_;
    instance_ = NULL;
  }
}

bool ObsHandler::IsEnabled()
{
  char value[PROPERTY_VALUE_MAX] = { '\0' };

#ifdef WCNSS_OBS_ENABLED
  property_get("persist.vendor.service.bdroid.sobs", value, "true");
#else
  property_get("persist.vendor.service.bdroid.sobs", value, "false");
#endif
  return (strcmp(value, "true") == 0) ? true : false;
}

ObsHandler::ObsHandler(HciUartTransport* theTransport,
                      HealthInfoLog* theHealthInfo,
                      Controller* theController)
{
  ALOGD("%s", __func__);
  uart_transport_ = theTransport;
  bt_logger_ = Logger::Get();
  pthread_mutex_init(&sleep_ind_lock_, NULL);
  pthread_cond_init(&sleep_ind_cond_, NULL);
  health_info = theHealthInfo;
  controller = theController;
  state_info_  = BtState::Get();
  state_machine_.timer_created = false;
  voteoff_timer_.timer_created = false;
  obs_spurious_wake_struct.timer_created = false;
  bt_lpm_.timer_created = false;
}

ObsHandler::~ObsHandler()
{
  ALOGI("%s", __func__);
  pthread_mutex_destroy(&sleep_ind_lock_);
  pthread_cond_destroy(&sleep_ind_cond_);

  if (bt_lpm_.timer_created == true) {
    timer_delete(bt_lpm_.timer_id);
    bt_lpm_.timer_created = false;
  }
  if (state_machine_.timer_created == true) {
    timer_delete(state_machine_.timer_id);
    state_machine_.timer_created = false;
  }
  if (voteoff_timer_.timer_created == true) {
    timer_delete(voteoff_timer_.timer_id);
    voteoff_timer_.timer_created = false;
  }
  if (obs_spurious_wake_struct.timer_created == true) {
    timer_delete(obs_spurious_wake_struct.timer_id);
    obs_spurious_wake_struct.timer_created = false;
  }
#ifdef WAKE_LOCK_ENABLED
  struct timeval tv;
  char dst_buff[MAX_BUFF_SIZE];
  gettimeofday(&tv, NULL);
  state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock :: Release in ~ObsHandler destructor");
  state_info_->SetWakeLockRelTimeName(dst_buff);
  Wakelock :: Release();
#endif
}

inline uint8_t ObsHandler::GetTxVote() {
  return state_machine_.tx_vote;
}

inline void ObsHandler::SetTxVote(uint8_t vote_value) {
  state_machine_.tx_vote = vote_value;
  if (health_info)
    health_info->UpdateTxVote(state_machine_.tx_vote);
}

inline uint8_t ObsHandler::GetRxVote() {
  return state_machine_.rx_vote;
}

inline void ObsHandler::SetRxVote(uint8_t vote_value) {
  state_machine_.rx_vote = vote_value;
  if (health_info)
    health_info->UpdateRxVote(state_machine_.rx_vote);
}


bool ObsHandler::IsObsCmd(unsigned char byte)
{
  if (byte == OBS_WAKE_ACK || byte == OBS_WAKE_IND || byte == OBS_SLEEP_IND) {
    return true;
  }
  return false;
}

void ObsHandler::ObsGpioVote(uint8_t vote)
{
  uint8_t new_vote;
  bool DevWakeGPIOOn = false;

  pthread_mutex_lock(&state_machine_.clock_vote_lock);
  uint8_t old_vote = (GetRxVote() | GetTxVote());
  ALOGV("%s", __func__);

  switch (vote) {
    case OBS_TX_VOTE_CLOCK_ON:
      SetTxVote(1);
      new_vote = 1;
      DevWakeGPIOOn = true;
      break;
    case OBS_RX_VOTE_CLOCK_ON:
      SetRxVote(1);
      new_vote = 1;
      break;
    case OBS_TX_VOTE_CLOCK_OFF:
      SetTxVote(0);
      new_vote = GetRxVote() | GetTxVote();
      // if UART CLK needs to be keeped due to OBS_RX_VOTE_CLOCK_ON, only
      // deacrivate GPIO.
      if (new_vote == old_vote) VoteDeactivateGpio();
      break;
    case OBS_RX_VOTE_CLOCK_OFF:
      SetRxVote(0);
      new_vote = GetRxVote() | GetTxVote();
      break;
    default:
      ALOGE("ObsGpioVote: Wrong vote requested!\n");
      pthread_mutex_unlock(&state_machine_.clock_vote_lock);
      return;
  }
  ALOGV("new_vote: (r%d|t%d) ## old-vote: (%d)", GetRxVote(), GetTxVote(), old_vote);

  if (new_vote != old_vote) {
#if 0
      UartController* uart_controller = static_cast<UartController*>(instance_->controller);
      /* UART CLK toggle can corrupts bytes on wire, hence reset the invalid byte counter */
      uart_controller->ResetInvalidByteCounter();
#endif
    if (new_vote) {
      /*vote UART CLK ON using Btpower driver's ioctl() */
      ALOGV("%s: vote for UART/Btpower CLK ON", __func__);
      VoteOnClock();
      state_machine_.CLK_STATE = CLOCK_ON;
#ifdef WAKE_LOCK_ENABLED
      struct timeval tv;
      char dst_buff[MAX_BUFF_SIZE];
      gettimeofday(&tv, NULL);
      state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock::Acquire during vote for UART CLK ON");
      state_info_->SetWakeLockAcqTimeName(dst_buff);
      Wakelock :: Acquire();
#endif
    } else {
      /*vote UART CLK OFF using UART driver's ioctl() */
      ALOGV("%s: vote for UART CLK OFF", __func__);
      VoteOffClock();
      state_machine_.CLK_STATE = CLOCK_OFF;
#ifdef WAKE_LOCK_ENABLED
      struct timeval tv;
      char dst_buff[MAX_BUFF_SIZE];
      gettimeofday(&tv, NULL);
      state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock::ReleaseDelay after UART CLK OFF");
      state_info_->SetWakeLockRelTimeName(dst_buff);
      Wakelock :: ReleaseDelay(delayed_wakelock);
#endif
    }
  }

  // No matter the UART CLK is ON or OFF, Dev WAKEUP GPIO needs to be pulled in
  // this case "OBS_TX_VOTE_CLOCK_ON".
  if (DevWakeGPIOOn) VoteActivateGpio();

  pthread_mutex_unlock(&state_machine_.clock_vote_lock);
}

void ObsHandler::StopAllTimers()
{
  ALOGV("%s", __func__);

  std::unique_lock<std::mutex> guard(obs_mutex_);

  if (!instance_) return;

  StopIdleTimer();

  StopVoteOffTimer();

  return;
}

void ObsHandler::VoteOffTimeout(union sigval /* sig */)
{
  std::unique_lock<std::mutex> guard(obs_mutex_);
  ALOGI("%s: uart serival vote off", __func__);
  if (!instance_) {
    ALOGD("%s: OBS handler has been destroyed ", __func__);
    return;
  }
  instance_->state_machine_.CLK_STATE = CLOCK_OFF;
  instance_->VoteOffClock();
}

void ObsHandler::StopVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  if (voteoff_timer_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop set wack timer", __func__);
    else if (status == 0)
      ALOGV("%s: Wack timer Stopped", __func__);
  }
}

void ObsHandler::StartVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  voteoff_timer_.timeout_ms = UART_VOTEOFF_DELAY;
  ts.it_value.tv_sec = voteoff_timer_.timeout_ms / 1000;
  ts.it_value.tv_nsec = 1000000 * (voteoff_timer_.timeout_ms % 1000);
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
  if (status == -1)
    ALOGE("%s:Failed to set wack timer", __func__);
}

void ObsHandler::obs_spurious_wake_timeout(union sigval /* sig */)
{
#ifdef DETECT_SPURIOUS_WAKE
  std::unique_lock<std::mutex> guard(obs_mutex_);
  ALOGE("%s:expired",__func__);

  if (!instance_) {
    ALOGD("%s: OBS handler has been destroyed ", __func__);
    return;
  }

  UartController* uart_controller = static_cast<UartController*>(instance_->controller);
  uart_controller->SsrCleanup(BT_HOST_REASON_SSR_SPURIOUS_WAKEUP);

  return;
#endif /* DETECT_SPURIOUS_WAKE */
}

void ObsHandler::obs_spurious_wake_timer_stop()
{
#ifdef DETECT_SPURIOUS_WAKE
    int status;
    struct itimerspec ts;

    if(obs_spurious_wake_struct.timer_created == true)
    {
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 0;
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;
        status = timer_settime(obs_spurious_wake_struct.timer_id, 0, &ts, 0);
        if(status == -1)
            ALOGE("%s:Failed to stop set spurious wake timer",__func__);
        else if(status == 0)
            ALOGV("%s: spurious wake timer Stopped",__func__);
     }
#endif /* DETECT_SPURIOUS_WAKE */
}

void ObsHandler::obs_spurious_wake_timer_start()
{
#ifdef DETECT_SPURIOUS_WAKE
    int status;
    struct itimerspec ts;
    struct sigevent se;
    static int spurious_wake_delay = UART_SPURIOUS_WAKE_SURPRESS_DELAY;

    if(obs_spurious_wake_struct.timer_created == false)
    {
        se.sigev_notify_function = obs_spurious_wake_timeout;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = &obs_spurious_wake_struct.timer_id;
        se.sigev_notify_attributes = NULL;

        status = timer_create(CLOCK_MONOTONIC, &se, &obs_spurious_wake_struct.timer_id);
        if (status == 0)
        {
            ALOGD("%s: Spurious wake timer created",__func__);
            obs_spurious_wake_struct.timer_created = true;
        }
    }

    if(obs_spurious_wake_struct.timer_created == true)
    {
        if (spurious_wake_delay != 0) {
            obs_spurious_wake_struct.timeout_ms = spurious_wake_delay;
            ts.it_value.tv_sec = obs_spurious_wake_struct.timeout_ms/1000;
            ts.it_value.tv_nsec = 1000000*(obs_spurious_wake_struct.timeout_ms%1000);
            ts.it_interval.tv_sec = 0;
            ts.it_interval.tv_nsec = 0;

            status = timer_settime(obs_spurious_wake_struct.timer_id, 0, &ts, 0);
            if (status == -1) {
                ALOGE("%s:Failed to set wack timer",__func__);
            }
        }
    }
#endif /* DETECT_SPURIOUS_WAKE */
}

int ObsHandler::DeviceWakeUp()
{

  int status = 0;

  ALOGV("[OBS] %s", __func__);
  pthread_mutex_lock(&state_machine_.hci_tx_obs_lock);

  switch (state_machine_.tx_obs_state) {
    case OBS_TX_ASLEEP:
    {
      ALOGV("%s: TX state ASLEEP, acquire SM lock", __func__);
      StopVoteOffTimer();
      state_machine_.tx_obs_state = OBS_TX_WAKING;
      ALOGV("%s: UART TX Vote ON", __func__);

      // Reused for wakelock
      ObsGpioVote(OBS_TX_VOTE_CLOCK_ON);
      ALOGV("%s: OBS_TX_VOTE_CLOCK_ON ", __func__);

      break;
    }
    case OBS_TX_WAKING:
      ALOGV("%s: SOC is WAKING UP", __func__);
      break;
    case OBS_TX_AWAKE:
      ALOGV("%s: SOC is already AWAKE", __func__);
      break;
  }

  pthread_mutex_unlock(&state_machine_.hci_tx_obs_lock);
  return status;
}


void ObsHandler::ProcessObsCmd(int GPIO_ON)
{

  ALOGV("%s", __func__);

  if (GPIO_ON) {

      ALOGV("%s: Received OBS_HOST_WAKE : ON", __func__);

      StopVoteOffTimer();
      switch (state_machine_.rx_obs_state) {
        case OBS_RX_ASLEEP:
          ALOGV("%s: UART RX Vote ON", __func__);
          ObsGpioVote(OBS_RX_VOTE_CLOCK_ON);
          state_machine_.rx_obs_state = OBS_RX_AWAKE;
          //obs_data = OBS_WAKE_ACK;
          ALOGV("%s: Writing OBS_WAKE_ACK", __func__);

          break;
        case OBS_RX_AWAKE:
          //obs_data = OBS_WAKE_ACK;
          ALOGV("%s: Writing OBS_WAKE_ACK - No Change", __func__);

          break;
      }
  } else {
    ALOGV("%s: Received OBS_HOST_WAKE : OFF", __func__);
    // Host wake up GPIO pulled to low.
     switch (state_machine_.rx_obs_state) {
        case OBS_RX_AWAKE:
          ALOGV("%s: RX path is awake, Vote Off uart", __func__);
          state_machine_.rx_obs_state = OBS_RX_ASLEEP;
          ALOGV("%s: UART RX Vote Off", __func__);
          ObsGpioVote(OBS_RX_VOTE_CLOCK_OFF);
          break;
        case OBS_RX_ASLEEP:
          ALOGV("%s: RX path is asleep", __func__);
          break;
      }
      pthread_mutex_lock(&sleep_ind_lock_);
      sleep_ind_recvd_ = true;
      pthread_cond_broadcast(&sleep_ind_cond_);
      pthread_mutex_unlock(&sleep_ind_lock_);
  }

#if 0
    case OBS_WAKE_ACK: ////0xFC
      switch (state_machine_.tx_obs_state) {
        case OBS_TX_WAKING:
          ALOGV("%s: Received OBS_WAKE_ACK: 0xFC", __func__);
          pthread_mutex_lock(&state_machine_.wack_lock);
          ALOGV("%s: Signal wack_cond_", __func__);
          wack_recvd_ = true;
          //StopWackTimer();
          pthread_cond_signal(&wack_cond_);
          pthread_mutex_unlock(&state_machine_.wack_lock);
          break;
        case OBS_TX_AWAKE:
          ALOGV("%s: TX SM is awake already, stop wack timer if running", __func__);
          //StopWackTimer();
          break;
        default:
          ALOGE("%s: WAKE ACK from SOC, Unexpected TX state", __func__);

          if (pthread_mutex_trylock(&state_machine_.hci_tx_obs_lock) == 0) {
            if (OBS_RX_AWAKE == state_machine_.rx_obs_state) {
              // Stop timers if active

              //ALOGV("%s: RX path is just awake, Vote Off uart again",__func__);
              //Send Sleep Indication to SOC
              //state_info_->AddLogTag(dst_buff, tv, (char *)"Writing FE to SoC");
              //state_info_->SetLastSleepIndFromHost(dst_buff);
              //WriteObsCmd(OBS_SLEEP_IND);
              //state_machine_.rx_obs_state = OBS_RX_ASLEEP;
              //ObsGpioVote(OBS_RX_VOTE_CLOCK_OFF);
            }

            //pthread_mutex_unlock(&state_machine_.hci_tx_obs_lock);
          }
          //break;
      }
      //break;
  }
#endif
}

void ObsHandler::Obs_data_sighandler(int signum, siginfo_t *info, void *unused)
{
  int siginfo_signo = (info ? info->si_signo : -1);
  int siginfo_errno = (info ? info->si_errno : -1);
  int siginfo_code = (info ? info->si_code : -1);
  int siginfo_int = (info ? info->si_int : -1);
  std::unique_lock<std::mutex> guard(obs_mutex_);
  ALOGV("%s: Catching Signal: %d, si_signo: %#x si_errno: %#x si_code: %#x si_int: %#x",
        __func__, signum, siginfo_signo, siginfo_errno, siginfo_code, siginfo_int);
  if (signum == OBS_HOSTWAKE_SIG && instance_) {
    ALOGD("%s: Catching Signal: %d Awaking: %#x", __func__, signum, siginfo_int);
    instance_->ProcessObsCmd(siginfo_int);
  } else {
    ALOGE("%s: Catching Signal: %d Awaking: %#x without ObsHandler......",
          __func__, signum, siginfo_int);
  }
}
void ObsHandler::DeviceSleep(void)
{

  ALOGV("%s: Acquire SM lock", __func__);
  pthread_mutex_lock(&state_machine_.hci_tx_obs_lock);
  switch (state_machine_.tx_obs_state) {
    case OBS_TX_AWAKE:
      if (bt_lpm_.wake_state == WCNSS_OBS_WAKE_ASSERTED) {
        ALOGV("%s: Tx in progress", __func__);
        pthread_mutex_unlock(&state_machine_.hci_tx_obs_lock);
        return;
      }
      ALOGV("%s: TX Awake, Sending SLEEP_IND", __func__);

      state_machine_.tx_obs_state    = OBS_TX_ASLEEP;
      ALOGV("%s: UART TX Vote Off", __func__);
      ObsGpioVote(OBS_TX_VOTE_CLOCK_OFF);
      break;
    case OBS_TX_ASLEEP:
      ALOGV("%s: TX Asleep", __func__);
      break;
    default:
      ALOGE("%s: Invalid TX SM", __func__);
      break;
  }
  pthread_mutex_unlock(&state_machine_.hci_tx_obs_lock);
  return;
}

void ObsHandler::IdleTimeout(union sigval /* sig */)
{
  std::unique_lock<std::mutex> guard(obs_mutex_);
  ALOGV("%s: Deassert SOC", __func__);
  if (!instance_) {
    ALOGD("%s: OBS handler has been destroyed ", __func__);
    return;
  }
  instance_->DeviceSleep();
}

void ObsHandler::StartIdleTimer(void)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == false) {
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &bt_lpm_.timer_id;
    se.sigev_notify_function = (void (*)(union sigval))IdleTimeout;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &bt_lpm_.timer_id);

    if (status == 0)
      bt_lpm_.timer_created = true;
  }

  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = bt_lpm_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (bt_lpm_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s: Failed to set LPM idle timeout", __func__);
  } else {
    ALOGE("%s: LPM idle timer NOT created", __func__);
  }
}

void ObsHandler::StopIdleTimer(void)
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("[STOP] Failed to set LPM idle timeout");
 }
}

void ObsHandler::TransmitDone(uint8_t tx_done)
{
  ALOGV("%s", __func__);
  bt_lpm_.no_tx_data = tx_done;
  if (tx_done == true) {
    ALOGV("%s: tx_done. Start idle timeout", __func__);
    bt_lpm_.wake_state = WCNSS_OBS_WAKE_W4_TIMEOUT;
    StartIdleTimer();
  }
}

void ObsHandler::VoteOnClock()
{
#if 0
  uart_transport_->ClockOperation(USERIAL_OP_CLK_ON);
#endif

  if (instance_->obs_fd_ < 0) {
    ALOGE("%s: btpower device not ready", __func__);
    return;
  }
  int rc = ioctl(instance_->obs_fd_, BT_CMD_OBS_VOTE_CLOCK, (unsigned long)CLOCK_ON);
  if (rc < 0)
  {
    ALOGE("%s: Ioctl OBS_VOTE_CLOCK ON failed", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = false;
  } else {
    ALOGV("%s: Ioctl OBS_VOTE_CLOCK ON Successfully", __func__);
    state_machine_.tx_obs_state = OBS_TX_AWAKE;
    wack_recvd_ = true;
  }
}

void ObsHandler::VoteOffClock()
{
  StopIdleTimer();
#if 0
  uart_transport_->ClockOperation(USERIAL_OP_CLK_OFF);
#endif

  if (instance_->obs_fd_ < 0) {
    ALOGE("%s: btpower device not ready", __func__);
    return;
  }
  int rc = ioctl(instance_->obs_fd_, BT_CMD_OBS_VOTE_CLOCK, (unsigned long)CLOCK_OFF);
  if (rc < 0)
  {
    ALOGE("%s: Ioctl OBS_VOTE_CLOCK OFF failed", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = false;
  } else {
    ALOGV("%s: Ioctl OBS_VOTE_CLOCK OFF Successfully", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = false;
  }
}

void ObsHandler::VoteActivateGpio()
{

  if (instance_->obs_fd_ < 0) {
    ALOGE("%s: btpower device not ready", __func__);
    return;
  }

  int rc = ioctl(instance_->obs_fd_, BT_CMD_OBS_VOTE_CLOCK, (unsigned long)DEV_GPIO_ON);

  if (rc < 0)
  {
    ALOGE("%s: Ioctl OBS_VOTE_CLOCK ON failed", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = false;
  } else {
    ALOGV("%s: Ioctl OBS_VOTE_CLOCK ON Successfully", __func__);
    state_machine_.tx_obs_state = OBS_TX_AWAKE;
    wack_recvd_ = true;
  }
}

void ObsHandler::VoteDeactivateGpio()
{
  if (instance_->obs_fd_ < 0) {
    ALOGE("%s: btpower device not ready", __func__);
    return;
  }

  int rc = ioctl(instance_->obs_fd_, BT_CMD_OBS_VOTE_CLOCK, (unsigned long)DEV_GPIO_OFF);
  if (rc < 0)
  {
    ALOGE("%s: Ioctl OBS_VOTE_CLOCK OFF failed", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = false;
  } else {
    ALOGV("%s: Ioctl OBS_VOTE_CLOCK OFF Successfully", __func__);
    state_machine_.tx_obs_state = OBS_TX_ASLEEP;
    wack_recvd_ = true;
  }
}

int ObsHandler::WakeAssert(void)
{
  int ret = 0;

  ALOGV("[OBS] %s", __func__);
  StopIdleTimer();
  bt_lpm_.wake_state = WCNSS_OBS_WAKE_ASSERTED;
  ret = DeviceWakeUp();

  if (ret != -1) {
    TransmitDone(false);
  }
  return ret;
}

bool ObsHandler::waitUntilSleepIndRecvd(void)
{
  ALOGD("%s, begin", __func__);
  if (state_machine_.rx_obs_state == OBS_RX_ASLEEP) {
    ALOGD("%s, rx is already asleep", __func__);
    return true;
  }

  pthread_mutex_lock(&sleep_ind_lock_);
  sleep_ind_recvd_ = false;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long ns = ts.tv_nsec + 1000000 * (SLEEP_IND_WAIT_MS%1000);
  ts.tv_nsec = ns%1000000000;
  ts.tv_sec += ns/1000000000 + SLEEP_IND_WAIT_MS/1000;
  pthread_cond_timedwait(&sleep_ind_cond_, &sleep_ind_lock_, &ts);
  pthread_mutex_unlock(&sleep_ind_lock_);
  ALOGD("%s, end, sleep_ind_recvd_=%d", __func__, sleep_ind_recvd_);
  return sleep_ind_recvd_;
}
} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
#endif /* WCNSS_OBS_ENABLED */
