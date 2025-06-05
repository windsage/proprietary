/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <mutex>
#include <atomic>
#include <queue>
#include "xm_glink_transport.h"
#include "xm_kp_transport.h"
#include "xm_async_fd_watcher.h"
#include "xm_ipc_if.h"
#include "xm_state_machine.h"
#include "timer.h"

#define XM_CP_DEFAULT_LOG_LVL         "0x11"
#define XM_BTFMCODEC_DEFAULT_LOG_LVL  "0x03"
#define LOSSLESS_USECASE_MULTIPLIER    1
#define GAMING_VBC_USECASE_MULTIPLIER  4
#define XM_FD_WATCHER_ENABLED          0
#define XM_WIFI_LIB_ENABLED            1
#define XM_XAC_ENABLED                 2

namespace xpan {
namespace implementation {

typedef struct {
  std::mutex xm_lock;
  struct xm_state_machine xm_state;
  XmAudioBearerReq AudioBearerData;
  XmBearerPreferenceReq BearerPreferenceData;
  XmUseCaseReq UseCaseData;
  TwtParameters TwtParams;
  uint8_t adsp_state;
} __attribute__((packed)) XmAppData;

class XpanManager
{
  public:
   XpanManager();
   ~XpanManager();
   int Initialize(bool);
   static std::shared_ptr <XpanManager> Get(void);
   int Deinitialize(bool);
   void NotifyCoPVer(uint8_t, uint8_t *);
   bool PostMessage(xm_ipc_msg_t *);
   void GetMainThreadState(void);
   bool CacheCpMessage(xm_ipc_msg_t *);
   uint8_t* GetCopVersionInUse(void);
   uint8_t* GetCopVersionSupported(void);
   void FreeCopVersionSupported(void);
   void FreeCopVersionInUse(void);
  private:
   void XpanManagerMainThreadRoutine(bool);
   static void usr_handler(int);
   void XmIpcMsgHandler(xm_ipc_msg_t *);
   static void AudioBearerTimeOut(union sigval);
   static void BearerPreferenceTimeOut(union sigval);
   void QhciPrepareAudioBearerReq(xm_ipc_msg_t *);
   void PrepareAudioBearerRsp(XmIpcEventId, xm_ipc_msg_t *);
   void BearerSwitchInd(xm_ipc_msg_t *);
   void QhciUnPrepareAudioBearerReq(xm_ipc_msg_t *);
   void UnPrepareAudioBearerRsp(XmIpcEventId, xm_ipc_msg_t *);
   void ConvertToXmStatus(uint8_t, RspStatus*);
   void NotifyLoglvl(void);
   void QhciUseCaseUpdate(xm_ipc_msg_t *);
   void XmWifiUseCaseUpdate();
   void UpdateStats(bool, uint8_t);
   int GetStatsInterval(void);
   void BearerSwitchReq(xm_ipc_msg_t *);
   void BearerSwitchRsp(XmIpcEventId eventId, xm_ipc_msg_t *);
   void BearerPreferenceReq(xm_ipc_msg_t *);
   void TransportUpdate(xm_ipc_msg_t *msg);
   void GetCurrentUsecase(void);
   void ProcessTwtEvent(xm_ipc_msg_t *);
   void TerminateTwt(void);

   xm_state_machine *xm_get_sm_ptr(void);
   XmAudioBearerReq *xm_get_audiobearer_data(void);
   XmBearerPreferenceReq *xm_get_bearer_preference_data(void);
   XmUseCaseReq *xm_get_usecase_data(void);
   void GetEbDetails(xm_ipc_msg_t *);
   void xm_set_usecase(UseCaseType);
   UseCaseType xm_get_usecase(void);
   void AdspStateUpdate(xm_ipc_msg_t *);
   void CpIpcMsgHandler(xm_ipc_msg_t *);
   void xm_set_adsp_state(uint8_t state);
   uint8_t xm_get_adsp_state(void);
   void AudioTransportUpdate(xm_ipc_msg_t *);
   void CacheTwtParams(xm_ipc_msg_t *);
   TwtParameters *xm_get_twtparams_data(void);
   void UseCaseStartInd(xm_ipc_msg_t *);
   void XmStaWifiUseCaseUpdate(xm_ipc_msg_t *);
   void WifiTransportSwitch(xm_ipc_msg_t *);
   void WifiTransportSwitchReq(uint8_t);
   void WifiBearerIndication(uint8_t, uint8_t);
   void WifiTransportSwitchRsp(XmIpcEventId, xm_ipc_msg_t *);
   void Cp_Cop_Ver_Info_Init(void);

   /* Declared below as static to avoid mutiple references */
   std::mutex xm_wq_mtx;
   std::mutex cp_wq_mtx;
   std::mutex cp_copver_mtx;
   std::queue <xm_ipc_msg_t *> xm_workqueue;
   std::queue <xm_ipc_msg_t *> cp_workqueue;
   std::condition_variable xm_wq_notifier;
   std::atomic_bool is_main_thread_busy;
   std::atomic_bool main_thread_running;
   static std::shared_ptr<XpanManager> main_instance;
   struct alarm_t *audio_bearer;
   struct alarm_t *bearer_preference_req;

   GlinkTransport *glink_transport;
   KernelProxyTransport *kp_transport;
   std::thread main_thread;
   int glink_fd;
   int kp_fd;
   XMAsyncFdWatcher fd_watcher_;
   XmAppData xm_cache;
   uint32_t xm_state;
};

} // namespace implementation
} // namespace xpan
