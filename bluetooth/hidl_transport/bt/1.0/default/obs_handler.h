/*==========================================================================
Description
  Wcnss_obs has the state-machine and logic to OOBS (Software Out-of-band sleep) protocol

# Copyright (c) 2021-2022, 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifdef WCNSS_OBS_ENABLED

#pragma once

#include "hci_uart_transport.h"
#include "wake_lock.h"
#include "logger.h"
#include "controller.h"
#include "state_info.h"

//Below three items are useless in OBS.
#define OBS_WAKE_ACK        0xFC
#define OBS_WAKE_IND        0xFD
#define OBS_SLEEP_IND       0xFE

/* LPM WAKE state */
enum {
  WCNSS_OBS_WAKE_DEASSERTED = 0,              /* initial state */
  WCNSS_OBS_WAKE_W4_TX_DONE,
  WCNSS_OBS_WAKE_W4_TIMEOUT,
  WCNSS_OBS_WAKE_ASSERTED
};
/* low power mode control block */

typedef struct
{
  //uint8_t state;                          // Low power mode state
  uint8_t wake_state;                     // LPM WAKE state
  uint8_t no_tx_data;
  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
} BluetoothOOBSLpm;

/* HCI_OBS transmit side sleep protocol states */
typedef enum {
  OBS_TX_ASLEEP = 0,
  OBS_TX_WAKING,
  OBS_TX_AWAKE,
} tx_obs_states;

/* HCI_OBS receive side sleep protocol states */
typedef enum {
  OBS_RX_ASLEEP = 0,
  OBS_RX_AWAKE,
} rx_obs_states;

typedef enum {
  OBS_TX_VOTE_CLOCK_ON = 0,
  OBS_TX_VOTE_CLOCK_OFF,
  OBS_RX_VOTE_CLOCK_ON,
  OBS_RX_VOTE_CLOCK_OFF,
} obs_clock_state_vote;

typedef struct {
  pthread_mutex_t wack_lock;
  pthread_mutex_t hci_tx_obs_lock;
  pthread_mutex_t clock_vote_lock;
  uint8_t tx_obs_state;
  uint8_t tx_vote;
  uint8_t rx_obs_state;
  uint8_t rx_vote;

  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
  uint8_t CLK_STATE;
} ObsStateMachine;

typedef struct {
  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
  uint8_t clk_vote;
} ObsVoteOffTimer;

#define WAKELOCK_PATH "/sys/power/wake_lock"
#define WAKEUNLOCK_PATH "/sys/power/wake_unlock"
#define BT_CMD_OBS_VOTE_CLOCK	0xbfd1

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

class ObsHandler {
 public:
  static ObsHandler* Init(HciUartTransport* theTransport, HealthInfoLog* theHealthInfo,
          Controller* theController);
  static ObsHandler* Get();
  static void CleanUp();
  static bool IsEnabled();
  ~ObsHandler();
  int WakeAssert(void);
  void DeviceSleep();
  void ProcessObsCmd(int GPIO_ON);
  void TransmitDone(uint8_t tx_done);
  bool IsObsCmd(unsigned char byte);
  bool waitUntilSleepIndRecvd(void);
  void obs_spurious_wake_timer_stop();
  void obs_spurious_wake_timer_start();
  void StopAllTimers();

 private:
  ObsHandler(HciUartTransport* theTransport, HealthInfoLog* theHealthInfo,
       Controller* theController);
  void InitInt ();
  void ObsGpioVote(uint8_t vote);
  //static void WakeRetransTimeout(union sigval sig);
  //void StartWackTimer(void);
  //void StopWackTimer(void);
  static void Obs_data_sighandler(int signum, siginfo_t *info, void *unused);

  void StartVoteOffTimer();
  void StopVoteOffTimer();
  static void VoteOffTimeout(union sigval sig);
  static void IdleTimeout(union sigval sig);
  static void obs_spurious_wake_timeout(union sigval sig);
  //void WriteObsCmd(uint8_t obs_data);
  void VoteOnClock();
  void VoteOffClock();
  void VoteActivateGpio();
  void VoteDeactivateGpio();
  int DeviceWakeUp();
  void StartIdleTimer(void);
  void StopIdleTimer(void);
  inline uint8_t GetTxVote();
  inline uint8_t GetRxVote();
  inline void SetTxVote(uint8_t vote_value);
  inline void SetRxVote(uint8_t vote_value);

  static ObsHandler* instance_;
  int tty_fd_;
  int obs_fd_;
  std::thread read_OBS_thread_;
  bool read_OBS_thread_running_ = false;
  HciUartTransport* uart_transport_;
  ObsStateMachine state_machine_;
  BluetoothOOBSLpm bt_lpm_;
  ObsVoteOffTimer voteoff_timer_;
  ObsVoteOffTimer obs_spurious_wake_struct;
  Logger* bt_logger_;
  pthread_cond_t wack_cond_;
  unsigned char wack_recvd_;
  unsigned int num_try_;
  unsigned int delayed_wakelock;

  unsigned char sleep_ind_recvd_;
  pthread_mutex_t sleep_ind_lock_;
  pthread_cond_t sleep_ind_cond_;

  int time_last_rx_sleep;
  int time_last_tx_packet;
  HealthInfoLog* health_info;
  Controller* controller;
  BtState* state_info_;
  friend void HealthInfoLog::ReportHealthInfo();

};
} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
#endif
