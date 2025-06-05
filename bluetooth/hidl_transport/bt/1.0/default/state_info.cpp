/*==========================================================================
Description
  It has implementation for BtState class

# Copyright (c) 2019-2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <hidl/HidlSupport.h>
#include <fcntl.h>
#include <errno.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <unistd.h>
#include "state_info.h"
#include "logger.h"
#include "ring_buffer.h"
#ifdef WCNSS_OBS_ENABLED
#include "obs_handler.h"
#else
#include "ibs_handler.h"
#endif
#include "wake_lock.h"
#include "health_info_log.h"
#include "data_handler.h"

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-btstateinfo"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-btstateinfo"
#endif

#define TIME_STAMP_LEN 13
#define TAG_STR_LEN    64

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

extern bool soc_need_reload_patch;
BtState * BtState::instance_ = NULL;

BtState * BtState::Get()
{
  if (!instance_) {
    instance_ = new BtState();
  }
  return instance_;
}

BtState::BtState()
{
  memset(&stat_info_obj, 0, sizeof(stat_info_obj));
  strlcpy(stat_info_obj.pri_crash_reason, "No Primary Crash reason set",
          MAX_CRASH_BUFF_SIZE);
  strlcpy(stat_info_obj.sec_crash_reason, "No Secondary Crash reason set",
          MAX_CRASH_BUFF_SIZE);
  strlcpy(stat_info_obj.last_health_stat_info, "Last health stats info: ",
          MAX_STATE_INFO_SIZE);
  strlcpy(stat_info_obj.last_wk_lck_acq_info, "00:00:00:000-Wakelock :: Acquire",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_lck_rel_info, "00:00:00:000-Wakelock :: Release",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_hci_cmd_stack, "00:00:00:000-Last HCI cmd from stack",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ind_from_host, "00:00:00:000-Writing FD to SOC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ack_from_soc, "00:00:00:000-Received FC from SOC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ind_from_soc, "00:00:00:000-Received FD from SOC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ack_from_host, "00:00:00:000-Writing FC to SOC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_sleep_ind_from_host, "00:00:00:000-Writing FE to SoC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_sleep_ind_from_soc, "00:00:00:000-Received FE from SOC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pc_addr, "PC address of FW crash: 0x00", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.build_ver, "SOC build ver: 0x00", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_getver_start_send_cmd, "00:00:00:000-Get Version CMD not sent to SOC"
          , MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_getver_start_read_rsp,
          "00:00:00:000-Get Version CMD rsp not yet read", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_getver_rcvd, "00:00:00:000-Get Version CMD RSP not rcvd from SOC",
          MAX_BUFF_SIZE);
#ifdef BT_GANGES_SUPPORTED
  strlcpy(stat_info_obj.ts_peri_getver_start_send_cmd, "00:00:00:000-Peri Get Version CMD not sent to SOC"
          , MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_getver_start_read_rsp,
          "00:00:00:000-Peri Get Version CMD rsp not yet read", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_getver_rcvd, "00:00:00:000-Peri Get Version CMD RSP not rcvd from SOC",
          MAX_BUFF_SIZE);
#endif
  strlcpy(stat_info_obj.ts_hci_initialize, "00:00:00:000-HCI initialize rcvd from client",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_hci_close, "00:00:00:000-HCI close not rcvd from client",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_last_seq_number, "00:00:00:000-Last sequence num",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_event, "00:00:00:000-Pre stack event callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_event, "00:00:00:000-Post stack event callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_acl, "00:00:00:000--Pre stack ACL callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_acl, "00:00:00:000--Pre stack ACL callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_iso, "00:00:00:000--Pre stack ISO callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_iso, "00:00:00:000--Pre stack ISO callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_select, "00:00:00:000--Pre select callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_select, "00:00:00:000--Pre select callback",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_flow_on, "00:00:00:000-UART Flow On", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_flow_off, "00:00:00:000-UART Flow Off", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_clock_on, "00:00:00:000-UART Clock on", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_clock_off, "00:00:00:000-UART Clock off",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_init_cb_sent, "00:00:00:000-Init callback not sent",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_diag_init, "00:00:00:000:Diag not initialized",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.health_timer_status, "Health timer not started",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.read_thread_start, "Yet to start read thread", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.status_read_thread, "Read thread not started", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_ctrlr_init, "Controller Init not completed",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_tx_pkt_ts, "00:00:00:000-last tx packet", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_rx_pkt_ts, "00:00:00:000-last rx packet", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ssr_rx_pkt_ts, "00:00:00:000-last rx before ssr", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.kernel_time, "TS System 00:00:00:000- TS kernel 000:00000", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_deinit_ts, "00:00:00:000:Pre Diag deinit not started",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_deinit_ts, "00:00:00:000:Post Diag deinit not started",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_init_ts, "00:00:00:000:Pre Diag init not started",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_init_ts, "00:00:00:000:Post Diag init not started",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.fw_dwnld_success, "00:00:00:000:Soc Initialization not completed",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.fw_su_build_info, "BT SoC FW SU Build info: Not Known", MAX_BUFF_SIZE);
#ifdef BT_GANGES_SUPPORTED
  strlcpy(stat_info_obj.peri_fw_dwnld_success, "00:00:00:000:Peri Soc Initialization not completed",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_fw_su_build_info, "Peri SoC FW SU Build info: Not Known", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_soc_id, "Peri SOC Version: Not Known", MAX_BUFF_SIZE);
#endif
  strlcpy(stat_info_obj.bt_soc_id, "unified Current SOC Version: Not Known", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.alwayson_status, "SOC AlwaysOn disabled", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_hci_internal_cmd, "No internal commands sent to SoC",
          MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_hci_internal_rsp,
          "Not rcvd RSP for any internal commands", MAX_BUFF_SIZE);
  //FIX_ME
  //strlcpy(stat_info_obj.power_resources_state,
    //      "BT Power Resources State: Unknown", MAX_PWR_RSRC_INFO_SIZE);
  strlcpy(stat_info_obj.bt_uart_cts,
          "BT UART CTS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_patch_open,
          "Pre Patch open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_patch_open,
          "Post Patch open TS: Unknown", MAX_BUFF_SIZE);
#ifdef BT_GANGES_SUPPORTED
  strlcpy(stat_info_obj.peri_pre_patch_open,
          "Peri Pre Patch open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_patch_open,
          "Pri Post Patch open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_nvm_open,
          "Peri Pre NVM open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_nvm_open,
		  "Peri Post NVM open TS: Unknown", MAX_BUFF_SIZE);
#endif
  strlcpy(stat_info_obj.post_nvm_open,
          "Post NVM open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_nvm_open,
          "Pre NVM open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_sibs_property_read,
          "SIBS property read not read", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_sibs_property_read,
          "Post SIBS property read TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_xmem_patch_open,
          "Pre XMEM Patch open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_xmem_patch_open,
          "Post XMEM Patch open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_xmem_nvm_open,
          "Pre XMEM NVM open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_xmem_nvm_open,
          "Post XMEM NVM open TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_chk_patch_path,
          "Pre Check patch TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_chk_patch_path,
          "Post Check patch TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_version_info,
          "Pre Version info. write TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_version_info,
          "Post Version info. write TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_su_build_info,
          "Pre SU build info. write TS: Unknown", MAX_BUFF_SIZE);
#ifdef BT_GANGES_SUPPORTED
  strlcpy(stat_info_obj.peri_post_chk_patch_path,
          "Peri Post Check patch TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_chk_patch_path,
          "Peri Pre Check patch TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_version_info,
          "Peri Pre Version info. write TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_version_info,
          "Peri Post Version info. write TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_su_build_info,
          "Peri Pre SU build info. write TS: Unknown", MAX_BUFF_SIZE);
  stat_info_obj.peri_fw_su_build_info_set_time = (struct timeval){0, 0};
#endif
  strlcpy(stat_info_obj.post_su_build_info,
          "Post SU build info. write TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_local_addr,
          "Pre Local BD address TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_local_addr,
          "Post Local BD address TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_fetch_vendor_addr,
          "Pre Vendor Local address TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_fetch_vendor_addr,
          "Post Vendor Local address TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_tcs_config,
          "Pre TCS config. TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_tcs_config,
          "Post TCS config. TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.stuck_issue,
          "Health timer Abort TS: Unkown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_rx_acl,
          "Pre diag ACL RX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_rx_acl,
          "Post diag ACL RX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_rx_hci,
          "Pre diag HCI RX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_rx_hci,
          "Post diag HCI RX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_tx_acl,
          "Pre diag ACL TX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_tx_acl,
          "Post diag ACL TX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_tx_hci,
          "Pre diag HCI TX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_tx_hci,
          "Post diag HCI TX TS: Unknown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.uart_ipc_err,
  "UART IPC log Dir open status: Unkown", MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.otp_info_bytes,
          "OTP info. bytes: Unknown", OTP_INFO_BYTES_MAX_SIZE);
  strlcpy(stat_info_obj.bqr_rie_info,
          "BQR RIE info. : Unknown", MAX_CRASH_BUFF_SIZE);
  stat_info_obj.is_xmem_enabled = false;
  stat_info_obj.ssr_trigger = false;
  stat_info_obj.max_diag_rx_hci_delay = 0;
  stat_info_obj.max_diag_rx_acl_delay = 0;
  stat_info_obj.max_diag_tx_hci_delay = 0;
  stat_info_obj.max_diag_tx_acl_delay = 0;
  stat_info_obj.diag_pending_pkt = 0;
  stat_info_obj.fw_su_build_info_set_time = (struct timeval){0};
}

void BtState::Cleanup(void)
{
  if (instance_) {
    delete instance_;
    instance_ = NULL;
  }
}

void BtState::AddLogTag(char* dest_tag_str, struct timeval& time_val, char * tag)
{
  uint32_t w_index = 0;
  memset(dest_tag_str, 0, TAG_STR_LEN);
  add_time_str(dest_tag_str, &time_val);
  w_index = strlen(dest_tag_str);
  snprintf(dest_tag_str+w_index, TAG_STR_LEN - w_index, "-%s", tag);
}

void BtState::SetTsHCIInitClose(HCIStatus hci_status, char *buf) {
  if(hci_status == HCI_INIT) {
    memset(stat_info_obj.ts_hci_initialize, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_hci_initialize, buf, sizeof(stat_info_obj.ts_hci_initialize));
    ALOGV("%s:is %s", __func__, stat_info_obj.ts_hci_initialize);
  } else if(hci_status == HCI_CLOSE) {
    memset(stat_info_obj.ts_hci_close, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_hci_close, buf, sizeof(stat_info_obj.ts_hci_close));
    ALOGV("%s:is %s", __func__, stat_info_obj.ts_hci_close);
 }
}

void BtState::SetDiagInitStatus(char *buf) {
  memset(stat_info_obj.ts_status_diag_init, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_diag_init, buf, sizeof(stat_info_obj.ts_status_diag_init));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_status_diag_init);
}

void BtState::SetHciInternalCmdSent(char *buf) {
  memset(stat_info_obj.ts_hci_internal_cmd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_hci_internal_cmd, buf, sizeof(stat_info_obj.ts_hci_internal_cmd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_hci_internal_cmd);
}

void BtState::SetHciInternalCmdRsp(char *buf) {
  memset(stat_info_obj.ts_hci_internal_rsp, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_hci_internal_rsp, buf, sizeof(stat_info_obj.ts_hci_internal_rsp));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_hci_internal_rsp);
}

void BtState::SetTsCtrlrInitStatus(char *buf) {
  memset(stat_info_obj.ts_status_ctrlr_init, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_ctrlr_init, buf, sizeof(stat_info_obj.ts_status_ctrlr_init));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_status_ctrlr_init);
}

void BtState::SetWakeLockAcqTimeName(char* buf) {
  memset(stat_info_obj.last_wk_lck_acq_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_lck_acq_info, buf, sizeof(stat_info_obj.last_wk_lck_acq_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_wk_lck_acq_info);
}

void BtState::SetHelthStatsTimerStatus(char* buf) {
  memset(stat_info_obj.health_timer_status, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.health_timer_status, buf, sizeof(stat_info_obj.health_timer_status));
  ALOGV("%s:is %s", __func__, stat_info_obj.health_timer_status);
}

void BtState::SetWakeLockRelTimeName(char* buf) {
  memset(stat_info_obj.last_wk_lck_rel_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_lck_rel_info, buf, sizeof(stat_info_obj.last_wk_lck_rel_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_wk_lck_rel_info);
}

void BtState::SetLastHciCmdStack(char* buf) {
  memset(stat_info_obj.last_hci_cmd_stack, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_hci_cmd_stack, buf, sizeof(stat_info_obj.last_hci_cmd_stack));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_hci_cmd_stack);
}

void BtState::SetUARTFlowOn(char *buf) {
  memset(stat_info_obj.ts_uart_flow_on, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_flow_on, buf, sizeof(stat_info_obj.ts_uart_flow_on));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_uart_flow_on);
}

void BtState::SetUARTFlowOff(char *buf) {
  memset(stat_info_obj.ts_uart_flow_off, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_flow_off, buf, sizeof(stat_info_obj.ts_uart_flow_off));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_uart_flow_off);
}

void BtState::SetUARTClockOn(char *buf) {
  memset(stat_info_obj.ts_uart_clock_on, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_clock_on, buf, sizeof(stat_info_obj.ts_uart_clock_on));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_uart_clock_on);
}

void BtState::SetUARTClockOff(char *buf) {
  memset(stat_info_obj.ts_uart_clock_off, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_uart_clock_off, buf, sizeof(stat_info_obj.ts_uart_clock_off));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_uart_clock_off);
}

void BtState::SetTsStatusOfCbSent(char *buf) {
  memset(stat_info_obj.ts_status_init_cb_sent, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_status_init_cb_sent, buf, sizeof(stat_info_obj.ts_status_init_cb_sent));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_status_init_cb_sent);
}
void BtState::SetLastStatsInfo(char *buf) {
  memset(stat_info_obj.last_health_stat_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_health_stat_info, buf, sizeof(stat_info_obj.last_health_stat_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_health_stat_info);
}

void BtState::SetLastWakeIndFromHost(char* buf) {
  memset(stat_info_obj.last_wk_ind_from_host, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ind_from_host, buf, sizeof(stat_info_obj.last_wk_ind_from_host));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_wk_ind_from_host);
}

void BtState::SetLastWakeAckFromHost(char* buf) {
  memset(stat_info_obj.last_wk_ack_from_host, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ack_from_host, buf, sizeof(stat_info_obj.last_wk_ack_from_host));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_wk_ack_from_host);
}

void BtState::SetLastWakeIndFromSoc(char* buf) {
  memset(stat_info_obj.last_wk_ind_from_soc, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ind_from_soc, buf, sizeof(stat_info_obj.last_wk_ind_from_soc));
  ALOGV("%s:is %s", __func__, buf);
}

void BtState::SetLastWakeAckFromSoc(char* buf) {
  memset(stat_info_obj.last_wk_ack_from_soc, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_wk_ack_from_soc, buf, sizeof(stat_info_obj.last_wk_ack_from_soc));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_wk_ack_from_soc);
}

void BtState::SetLastSleepIndFromHost(char* buf) {
  memset(stat_info_obj.last_sleep_ind_from_host, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_sleep_ind_from_host, buf,
          sizeof(stat_info_obj.last_sleep_ind_from_host));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_sleep_ind_from_host);
}

void BtState::SetLastSleepIndFromSoc(char* buf) {
  memset(stat_info_obj.last_sleep_ind_from_soc, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.last_sleep_ind_from_soc, buf, sizeof(stat_info_obj.last_sleep_ind_from_soc));
  ALOGV("%s:is %s", __func__, stat_info_obj.last_sleep_ind_from_soc);
}

void BtState::SetPreStackEvent(char* buf) {
  memset(stat_info_obj.pre_stack_event, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_event, buf, sizeof(stat_info_obj.pre_stack_event));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_stack_event);
}

void BtState::SetPostStackEvent(char* buf) {
  memset(stat_info_obj.post_stack_event, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_event, buf, sizeof(stat_info_obj.post_stack_event));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_stack_event);
}

void BtState::SetPreStackAcl(char* buf) {
  memset(stat_info_obj.pre_stack_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_acl, buf, sizeof(stat_info_obj.pre_stack_acl));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_stack_acl);
}

void BtState::SetPostStackAcl(char* buf) {
  memset(stat_info_obj.post_stack_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_acl, buf, sizeof(stat_info_obj.post_stack_acl));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_stack_acl);
}

void BtState::SetPreStackIso(char* buf) {
  memset(stat_info_obj.pre_stack_iso, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_stack_iso, buf, sizeof(stat_info_obj.pre_stack_iso));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_stack_iso);
}

void BtState::SetPostStackIso(char* buf) {
  memset(stat_info_obj.post_stack_iso, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_stack_iso, buf, sizeof(stat_info_obj.post_stack_iso));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_stack_iso);
}

void BtState::SetPreSelect(char* buf) {
  memset(stat_info_obj.pre_select, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_select, buf, sizeof(stat_info_obj.pre_select));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_select);
}

void BtState::SetPostSelect(char* buf) {
  memset(stat_info_obj.post_select, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_select, buf, sizeof(stat_info_obj.post_select));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_select);
}

void BtState::SetPCAddr(char* buf) {
  memset(stat_info_obj.pc_addr, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pc_addr, buf, sizeof(stat_info_obj.pc_addr));
  ALOGV("%s:is %s", __func__, stat_info_obj.pc_addr);
}

void BtState::SetBuildVersion(char* buf) {
  memset(stat_info_obj.build_ver, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.build_ver, buf, sizeof(stat_info_obj.build_ver));
  ALOGV("%s:is %s", __func__, stat_info_obj.build_ver);
}

void BtState::SetGetVerReqTS(char* buf, int type) {

  if (type == SEND_CMD_TS) {
    memset(stat_info_obj.ts_getver_start_send_cmd, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_getver_start_send_cmd, buf,
            sizeof(stat_info_obj.ts_getver_start_send_cmd));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_getver_start_send_cmd);
  } else {
    memset(stat_info_obj.ts_getver_start_read_rsp, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_getver_start_read_rsp, buf,
            sizeof(stat_info_obj.ts_getver_start_read_rsp));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_getver_start_read_rsp);
  }
}

void BtState::SetGetVerResEvtTS(char* buf) {
  memset(stat_info_obj.ts_getver_rcvd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_getver_rcvd, buf, sizeof(stat_info_obj.ts_getver_rcvd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_getver_rcvd);
}
#ifdef BT_GANGES_SUPPORTED
void BtState::SetPeriGetVerReqTS(char* buf, int type) {
  if (type == SEND_CMD_TS) {
    memset(stat_info_obj.ts_peri_getver_start_send_cmd, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_peri_getver_start_send_cmd, buf,
            sizeof(stat_info_obj.ts_peri_getver_start_send_cmd));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_peri_getver_start_send_cmd);
  } else {
    memset(stat_info_obj.ts_peri_getver_start_read_rsp, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_peri_getver_start_read_rsp, buf,
            sizeof(stat_info_obj.ts_peri_getver_start_read_rsp));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_peri_getver_start_read_rsp);
  }
}

void BtState::SetPeriArbitNtfTS(char* buf) {
  memset(stat_info_obj.ts_peri_arbit_ntf, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_arbit_ntf, buf, sizeof(stat_info_obj.ts_peri_arbit_ntf));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_arbit_ntf);
}

void BtState::SetPeriSocId(char* buf) {
  memset(stat_info_obj.peri_soc_id, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_soc_id, buf, sizeof(stat_info_obj.peri_soc_id));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_soc_id);
}

void BtState::SetPeriGetVerResEvtTS(char* buf) {
  memset(stat_info_obj.ts_peri_getver_rcvd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_getver_rcvd, buf, sizeof(stat_info_obj.ts_peri_getver_rcvd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_getver_rcvd);
}

void BtState::SetPeriArbitReqTS(char* buf) {
  memset(stat_info_obj.ts_peri_arbitration_sent, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_arbitration_sent, buf, sizeof(stat_info_obj.ts_peri_arbitration_sent));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_arbitration_sent);
}

void BtState::SetPeriArbitEvtTS(char* buf) {
  memset(stat_info_obj.ts_peri_arbit_rcvd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_arbit_rcvd, buf, sizeof(stat_info_obj.ts_peri_arbit_rcvd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_arbit_rcvd);
}

void BtState::SetPeriActivateSsReqTS(char* buf, int type) {
  if (type == SEND_CMD_TS) {
    memset(stat_info_obj.ts_peri_getver_start_send_cmd, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_peri_getver_start_send_cmd, buf,
            sizeof(stat_info_obj.ts_peri_getver_start_send_cmd));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_peri_getver_start_send_cmd);
  } else {
    memset(stat_info_obj.ts_peri_getver_start_read_rsp, 0, MAX_BUFF_SIZE);
    strlcpy(stat_info_obj.ts_peri_getver_start_read_rsp, buf,
            sizeof(stat_info_obj.ts_peri_getver_start_read_rsp));
    ALOGV("%s: %s", __func__, stat_info_obj.ts_peri_getver_start_read_rsp);
  }
}

void BtState::SetPrePeriNVMOpenTS(char* buf) {
  memset(stat_info_obj.peri_pre_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_nvm_open, buf, sizeof(stat_info_obj.peri_pre_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_pre_nvm_open);
}

void BtState::SetPostPeriNVMOpenTS(char* buf) {
  memset(stat_info_obj.peri_post_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_nvm_open, buf, sizeof(stat_info_obj.peri_post_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_post_nvm_open);
}

void BtState::SetPeriActivateSsResEvtTS(char* buf) {
  memset(stat_info_obj.ts_peri_getver_rcvd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_getver_rcvd, buf,
          sizeof(stat_info_obj.ts_peri_getver_rcvd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_getver_rcvd);
}

void BtState::SetPrePeriVersioInfoTS(char* buf) {
  memset(stat_info_obj.peri_pre_version_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_version_info, buf, sizeof(stat_info_obj.peri_pre_version_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_pre_version_info);
}

void BtState::SetPostPeriVersioInfoTS(char* buf) {
  memset(stat_info_obj.peri_post_version_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_version_info, buf,
    sizeof(stat_info_obj.peri_post_version_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_post_version_info);
}

void BtState::SetPrePeriSUBuildInfoTS(char* buf) {
  memset(stat_info_obj.peri_pre_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_su_build_info, buf,
    sizeof(stat_info_obj.peri_pre_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_pre_su_build_info);
}

void BtState::SetPostPeriSUBuildInfoTS(char* buf) {
  memset(stat_info_obj.peri_post_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_su_build_info, buf,
    sizeof(stat_info_obj.peri_post_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_post_su_build_info);
}

void BtState::SetPeriFwDwnldSucceed(char *buf) {
  memset(stat_info_obj.peri_fw_dwnld_success, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_fw_dwnld_success, buf, sizeof(stat_info_obj.peri_fw_dwnld_success));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_fw_dwnld_success);
}

void BtState::SetPeriFwSuBuildInfo(char *buf, struct timeval tv) {
  stat_info_obj.peri_fw_su_build_info_set_time = tv;
  memset(stat_info_obj.peri_fw_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_fw_su_build_info, buf, sizeof(stat_info_obj.peri_fw_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_fw_su_build_info);
}

void BtState::SetPeriResetReqTS(char* buf) {
  memset(stat_info_obj.ts_peri_reset_start_send_cmd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_reset_start_send_cmd, buf,
          sizeof(stat_info_obj.ts_peri_reset_start_send_cmd));
  ALOGV("%s: %s", __func__, stat_info_obj.ts_peri_reset_start_send_cmd);
}

void BtState::SetPeriResetResEvtTS(char* buf) {
  memset(stat_info_obj.ts_peri_reset_rcvd, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_peri_reset_rcvd, buf, sizeof(stat_info_obj.ts_peri_getver_rcvd));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_peri_reset_rcvd);
}

void BtState::SetPrePeriPatchOpenTS(char* buf) {
  memset(stat_info_obj.peri_pre_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_pre_patch_open, buf, sizeof(stat_info_obj.peri_pre_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_pre_patch_open);
}

void BtState::SetPostPeriPatchOpenTS(char* buf) {
  memset(stat_info_obj.peri_post_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.peri_post_patch_open, buf, sizeof(stat_info_obj.peri_post_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.peri_post_patch_open);
}
#endif
void BtState::SetLastTxPacket(uint8_t *buf, uint16_t len, HciPacketType type) {
  struct timeval tv;
  ALOGV("%s", __func__);
  gettimeofday(&tv, NULL);
  stat_info_obj.actual_tx_pkt_len = len + 1; //1 for type
  AddLogTag(stat_info_obj.last_tx_pkt_ts, tv, (char *)"Last TX packet and timestamp ");
  stat_info_obj.last_tx_pkt_asc[0] = (char)type;
  stat_info_obj.last_tx_pkt_type = type;
  /*We are restricting size of packet upto 20 bytes due to performance issue*/
  if (len > TX_RX_PKT_ASC_SIZE -1) {
    len = TX_RX_PKT_ASC_SIZE ;
    stat_info_obj.last_tx_pkt_len = len;
    memcpy(&stat_info_obj.last_tx_pkt_asc[1], (char *)buf, len-1);
    ALOGV("Actual TX pkt size %d, Pkt restricted to size %d",
          stat_info_obj.actual_tx_pkt_len, TX_RX_PKT_ASC_SIZE);
  } else {
    stat_info_obj.last_tx_pkt_len = len + 1;
    memcpy(&stat_info_obj.last_tx_pkt_asc[1], (char *)buf, len);
  }
}

void BtState::SetLastRxPacket(uint8_t *buf, uint16_t len, HciPacketType type) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  stat_info_obj.actual_rx_pkt_len = len + 1; //1 for type
  ALOGV("%s", __func__);
  /*We are restricting size of packet upto 20 bytes due to performance issue*/
  if (stat_info_obj.ssr_trigger) {
    AddLogTag(stat_info_obj.ssr_rx_pkt_ts, tv, (char *)"Last RX packet before SSR and timestamp");
    stat_info_obj.ssr_rx_pkt_asc[0] = (char)type;
    stat_info_obj.ssr_rx_pkt_type = type;
    if (len > TX_RX_PKT_ASC_SIZE -1) {
      stat_info_obj.actual_ssr_rx_pkt_len = len;
      stat_info_obj.ssr_rx_pkt_len = TX_RX_PKT_ASC_SIZE;
      memcpy(&stat_info_obj.ssr_rx_pkt_asc[1], (char *)buf, stat_info_obj.ssr_rx_pkt_len-1);
      ALOGV("Actual RX pkt len before SSR %d, Pkt restricted to size %d",
            stat_info_obj.actual_ssr_rx_pkt_len, TX_RX_PKT_ASC_SIZE);
    } else {
      stat_info_obj.ssr_rx_pkt_len = len + 1;
      memcpy(&stat_info_obj.ssr_rx_pkt_asc[1], (char *)buf, len);
    }
    stat_info_obj.ssr_trigger = false;
  }


  AddLogTag(stat_info_obj.last_rx_pkt_ts, tv, (char *)"Last RX packet and timestamp");
  stat_info_obj.last_rx_pkt_asc[0] = (char)type;
  stat_info_obj.last_rx_pkt_type = type;
  if (len > TX_RX_PKT_ASC_SIZE -1) {
    stat_info_obj.actual_rx_pkt_len = len + 1; //1 for type
    len = TX_RX_PKT_ASC_SIZE ;
    stat_info_obj.last_rx_pkt_len = len;
    memcpy(&stat_info_obj.last_rx_pkt_asc[1], (char *)buf, len-1);
    ALOGV("Actual RX pkt size %d, Pkt restricted to size %d",
          stat_info_obj.actual_rx_pkt_len, TX_RX_PKT_ASC_SIZE);
  } else {
    stat_info_obj.last_rx_pkt_len = len + 1; //1 for type
    memcpy(&stat_info_obj.last_rx_pkt_asc[1], (char *)buf, len);
  }
}

void BtState::SetInvBytes(uint8_t *buf, uint16_t len) {
  struct timeval tv;
  ALOGV("%s", __func__);
  gettimeofday(&tv, NULL);
  stat_info_obj.inv_bytes_len = len;
  AddLogTag(stat_info_obj.inv_bytes_ts, tv, (char *)"Invalid bytes and timestamp ");
  /* We are restricting size of packet upto 20 bytes due to performance issue */
  if (len > TX_RX_PKT_ASC_SIZE - 1) {
    len = TX_RX_PKT_ASC_SIZE ;
    ALOGV("Pkt restricted to size %d", TX_RX_PKT_ASC_SIZE);
  }
  stat_info_obj.inv_bytes_len = len;
  memcpy(&stat_info_obj.inv_bytes_asc[0], (char *)buf, len);
}

void BtState::SetSSRtrigger(bool status) {
  stat_info_obj.ssr_trigger =  status;
  ALOGV("%s:is %d", __func__, status);
}

void BtState::SetXmemEnabledFlag() {
  stat_info_obj.is_xmem_enabled =  true;
}

void BtState::SetAlwaysOnEnabled() {
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler && data_handler->IsSocAlwaysOnEnabled())
    strlcpy(stat_info_obj.alwayson_status, "SOC AlwaysOn enabled", MAX_BUFF_SIZE);
  else
    strlcpy(stat_info_obj.alwayson_status, "SOC AlwaysOn disabled", MAX_BUFF_SIZE);
}

void BtState::SetLastSeqTS(char *buf) {
  memset(stat_info_obj.ts_last_seq_number, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.ts_last_seq_number, buf, sizeof(stat_info_obj.ts_last_seq_number));
  ALOGV("%s:is %s", __func__, stat_info_obj.ts_last_seq_number);
}

void BtState::SetReadThreadStatus(char *buf) {
  memset(stat_info_obj.status_read_thread, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.status_read_thread, buf, sizeof(stat_info_obj.status_read_thread));
  ALOGV("%s:is %s", __func__, stat_info_obj.status_read_thread);
}

void BtState::SetReadThreadStartTS(char *buf) {
  memset(stat_info_obj.read_thread_start, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.read_thread_start, buf, sizeof(stat_info_obj.read_thread_start));
  ALOGV("%s:is %s", __func__, stat_info_obj.read_thread_start);
}

void BtState::SetPrimaryCrashReason(char *buf) {
  memset(stat_info_obj.pri_crash_reason, 0, MAX_CRASH_BUFF_SIZE);
  strlcpy(stat_info_obj.pri_crash_reason, buf, sizeof(stat_info_obj.pri_crash_reason));
  ALOGV("%s:is %s", __func__, stat_info_obj.pri_crash_reason);
}

void BtState::SetSecondaryCrashReason(char *buf) {
  memset(stat_info_obj.sec_crash_reason, 0, MAX_CRASH_BUFF_SIZE);
  strlcpy(stat_info_obj.sec_crash_reason, buf, sizeof(stat_info_obj.sec_crash_reason));
  ALOGV("%s:is %s", __func__, stat_info_obj.sec_crash_reason);
}

void BtState::SetCrashTS(char *buf) {
  memset(stat_info_obj.crash_ts, 0, MAX_CRASH_BUFF_SIZE);
  strlcpy(stat_info_obj.crash_ts, buf, sizeof(stat_info_obj.crash_ts));
  ALOGV("%s:is %s", __func__, stat_info_obj.crash_ts);
}

void BtState::SetKernelTimeStamp(char *buf) {
  memset(stat_info_obj.kernel_time, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.kernel_time, buf, sizeof(stat_info_obj.kernel_time));
  ALOGV("%s:is %s", __func__, stat_info_obj.kernel_time);
}

void BtState::SetDiagInitPreTS(char *buf) {
  memset(stat_info_obj.pre_diag_init_ts, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_init_ts, buf, sizeof(stat_info_obj.pre_diag_init_ts));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_init_ts);
}

void BtState::SetDiagInitPostTS(char *buf) {
  memset(stat_info_obj.post_diag_init_ts, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_init_ts, buf, sizeof(stat_info_obj.post_diag_init_ts));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_init_ts);
}

void BtState::SetDiagDeInitPreTS(char *buf) {
  memset(stat_info_obj.pre_diag_deinit_ts, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_deinit_ts, buf, sizeof(stat_info_obj.pre_diag_deinit_ts));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_deinit_ts);
}

void BtState::SetDiagDeInitPostTS(char *buf) {
  memset(stat_info_obj.post_diag_deinit_ts, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_deinit_ts, buf, sizeof(stat_info_obj.post_diag_deinit_ts));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_deinit_ts);
}

void BtState::SetFwDwnldSucceed(char *buf) {
  memset(stat_info_obj.fw_dwnld_success, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.fw_dwnld_success, buf, sizeof(stat_info_obj.fw_dwnld_success));
  ALOGV("%s:is %s", __func__, stat_info_obj.fw_dwnld_success);
}

void BtState::SetFwSuBuildInfo(char *buf, struct timeval tv) {
  stat_info_obj.fw_su_build_info_set_time = tv;
  memset(stat_info_obj.fw_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.fw_su_build_info, buf, sizeof(stat_info_obj.fw_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.fw_su_build_info);
}

void BtState::SetBtSocId(char *buf) {
  memset(stat_info_obj.bt_soc_id, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.bt_soc_id, buf, sizeof(stat_info_obj.bt_soc_id));
  ALOGV("%s:is %s", __func__, stat_info_obj.bt_soc_id);
}

void BtState::SetPowerResourcesState(struct power_buff_struct *buf) {
  memset(&stat_info_obj.power_resources_state, 0, sizeof(stat_info_obj.power_resources_state));
  strlcpy(stat_info_obj.power_resources_state.bt, buf->bt_buff, sizeof(stat_info_obj.power_resources_state.bt));
  strlcpy(stat_info_obj.power_resources_state.uwb, buf->uwb_buff, sizeof(stat_info_obj.power_resources_state.uwb));
  strlcpy(stat_info_obj.power_resources_state.com, buf->platform_buff, sizeof(stat_info_obj.power_resources_state.com));
  ALOGV("%s:is %s", __func__, stat_info_obj.power_resources_state.bt);
  ALOGV("%s:is %s", __func__, stat_info_obj.power_resources_state.uwb);
  ALOGV("%s:is %s", __func__, stat_info_obj.power_resources_state.com);
}

void BtState::SetCtsState(char *buf) {
  memset(stat_info_obj.bt_uart_cts, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.bt_uart_cts, buf, sizeof(stat_info_obj.bt_uart_cts));
  ALOGV("%s:is %s", __func__, stat_info_obj.bt_uart_cts);
}

void BtState::SetPrePatchOpenTS(char* buf) {
  memset(stat_info_obj.pre_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_patch_open, buf, sizeof(stat_info_obj.pre_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_patch_open);
}

void BtState::SetPostPatchOpenTS(char* buf) {
  memset(stat_info_obj.post_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_patch_open, buf, sizeof(stat_info_obj.post_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_patch_open);
}

void BtState::SetPreNVMOpenTS(char* buf) {
  memset(stat_info_obj.pre_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_nvm_open, buf, sizeof(stat_info_obj.pre_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_nvm_open);
}

void BtState::SetPostNVMOpenTS(char* buf) {
  memset(stat_info_obj.post_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_nvm_open, buf, sizeof(stat_info_obj.post_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_nvm_open);
}

void BtState::SetPreXMEMPatchOpenTS(char* buf) {
  memset(stat_info_obj.pre_xmem_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_xmem_patch_open, buf,
    sizeof(stat_info_obj.pre_xmem_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_xmem_patch_open);
}

void BtState::SetPostXMEMPatchOpenTS(char* buf) {
  memset(stat_info_obj.post_xmem_patch_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_xmem_patch_open, buf,
    sizeof(stat_info_obj.post_xmem_patch_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_xmem_patch_open);
}

void BtState::SetPreXmemNVMOpenTS(char* buf) {
  memset(stat_info_obj.pre_xmem_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_xmem_nvm_open, buf,
    sizeof(stat_info_obj.pre_xmem_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_xmem_nvm_open);
}

void BtState::SetPostXmemNVMOpenTS(char* buf) {
  memset(stat_info_obj.post_xmem_nvm_open, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_xmem_nvm_open, buf,
    sizeof(stat_info_obj.post_xmem_nvm_open));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_xmem_nvm_open);
}

void BtState::SetPreReadSibsPropertyTS(char* buf) {
  memset(stat_info_obj.pre_sibs_property_read, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_sibs_property_read, buf,
    sizeof(stat_info_obj.pre_sibs_property_read));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_sibs_property_read);
}

void BtState::SetPostReadSibsPropertyTS(char* buf) {
  memset(stat_info_obj.post_sibs_property_read, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_sibs_property_read, buf,
    sizeof(stat_info_obj.post_sibs_property_read));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_sibs_property_read);
}

void BtState::SetPreTCSConfigIoctlTS(char* buf) {
  memset(stat_info_obj.pre_tcs_config, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_tcs_config, buf,
    sizeof(stat_info_obj.pre_tcs_config));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_tcs_config);
}

void BtState::SetPostTCSConfigIoctlTS(char* buf) {
  memset(stat_info_obj.post_tcs_config, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_tcs_config, buf,
    sizeof(stat_info_obj.post_tcs_config));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_tcs_config);
}

void BtState::SetPreCheckRampatchPathTS(char* buf) {
  memset(stat_info_obj.pre_chk_patch_path, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_chk_patch_path, buf, sizeof(stat_info_obj.pre_chk_patch_path));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_chk_patch_path);
}

void BtState::SetPostCheckRampatchPathTS(char* buf) {
  memset(stat_info_obj.post_chk_patch_path, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_chk_patch_path, buf, sizeof(stat_info_obj.post_chk_patch_path));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_chk_patch_path);
}

void BtState::SetPreVersioInfoTS(char* buf) {
  memset(stat_info_obj.pre_version_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_version_info, buf, sizeof(stat_info_obj.pre_version_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_version_info);
}

void BtState::SetPostVersioInfoTS(char* buf) {
  memset(stat_info_obj.post_version_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_version_info, buf,
    sizeof(stat_info_obj.post_version_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_version_info);
}

void BtState::SetPreSUBuildInfoTS(char* buf) {
  memset(stat_info_obj.pre_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_su_build_info, buf,
    sizeof(stat_info_obj.pre_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_su_build_info);
}

void BtState::SetPostSUBuildInfoTS(char* buf) {
  memset(stat_info_obj.post_su_build_info, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_su_build_info, buf,
    sizeof(stat_info_obj.post_su_build_info));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_su_build_info);
}

void BtState::SetPreGetLocalAddressTS(char* buf) {
  memset(stat_info_obj.pre_local_addr, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_local_addr, buf, sizeof(stat_info_obj.pre_local_addr));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_local_addr);
}

void BtState::SetPostGetLocalAddressTS(char* buf) {
  memset(stat_info_obj.post_local_addr, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_local_addr, buf, sizeof(stat_info_obj.post_local_addr));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_local_addr);
}

void BtState::SetTsStuckTimeout(char* buf) {
  memset(stat_info_obj.stuck_issue, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.stuck_issue, buf, sizeof(stat_info_obj.stuck_issue));
  ALOGD("%s:is %s", __func__, stat_info_obj.stuck_issue);
}

void BtState::SetPreFetchLocalAddressTS(char* buf) {
  memset(stat_info_obj.pre_fetch_vendor_addr, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_fetch_vendor_addr, buf,
    sizeof(stat_info_obj.pre_fetch_vendor_addr));
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_fetch_vendor_addr);
}

void BtState::SetPostFetchLocalAddressTS(char* buf) {
  memset(stat_info_obj.post_fetch_vendor_addr, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_fetch_vendor_addr, buf,
    sizeof(stat_info_obj.post_fetch_vendor_addr));
  ALOGV("%s:is %s", __func__, stat_info_obj.post_fetch_vendor_addr);
}

void BtState::SetPreTsDiagRxAcl(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt++;
  }
  memset(stat_info_obj.pre_diag_rx_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_rx_acl, buf,
    sizeof(stat_info_obj.pre_diag_rx_acl));
  stat_info_obj.diag_rx_acl_start = tv;
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_rx_acl);
}

void BtState::SetPostTsDiagRxAcl(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt--;
  }
  memset(stat_info_obj.post_diag_rx_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_rx_acl, buf,
    sizeof(stat_info_obj.post_diag_rx_acl));
  unsigned long long time_1 = stat_info_obj.diag_rx_acl_start.tv_sec*1e3*1ull +
                                stat_info_obj.diag_rx_acl_start.tv_usec*1e-3*1ull;
  unsigned long long time_2 = tv.tv_sec*1e3*1ull + tv.tv_usec*1e-3*1ull;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    time_2 = time_1;
  }

  if ((time_2 - time_1) > stat_info_obj.max_diag_rx_acl_delay)
    stat_info_obj.max_diag_rx_acl_delay = (time_2 - time_1);
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_rx_acl);
}

void BtState::SetPreTsDiagRxHciEvt(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt++;
  }
  memset(stat_info_obj.pre_diag_rx_hci, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_rx_hci, buf,
    sizeof(stat_info_obj.pre_diag_rx_hci));
  stat_info_obj.diag_rx_hci_start = tv;
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_rx_hci);
}

void BtState::SetPostTsDiagRxHciEvt(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt--;
  }
  memset(stat_info_obj.post_diag_rx_hci, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_rx_hci, buf,
    sizeof(stat_info_obj.post_diag_rx_hci));
  unsigned long long time_1 = stat_info_obj.diag_rx_hci_start.tv_sec*1e3*1ull +
                                stat_info_obj.diag_rx_hci_start.tv_usec*1e-3*1ull;
  unsigned long long time_2 = tv.tv_sec*1e3*1ull + tv.tv_usec*1e-3*1ull;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    time_2 = time_1;
  }

  if ((time_2 - time_1) > stat_info_obj.max_diag_rx_hci_delay)
    stat_info_obj.max_diag_rx_hci_delay = (time_2 - time_1);
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_rx_hci);
}

void BtState::SetPreTsDiagTxAcl(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt++;
  }
  memset(stat_info_obj.pre_diag_tx_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_tx_acl, buf,
    sizeof(stat_info_obj.pre_diag_tx_acl));
  stat_info_obj.diag_tx_acl_start = tv;
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_tx_acl);
}

void BtState::SetPostTsDiagTxAcl(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt--;
  }
  memset(stat_info_obj.post_diag_tx_acl, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_tx_acl, buf,
    sizeof(stat_info_obj.post_diag_tx_acl));
  long long time_1 = stat_info_obj.diag_tx_acl_start.tv_sec*1e3*1ll +
                                stat_info_obj.diag_tx_acl_start.tv_usec*1e-3*1ll;
  long long time_2 = tv.tv_sec*1e3*1ll + tv.tv_usec*1e-3*1ll;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    time_2 = time_1;
  }

  if ((time_2 - time_1) > stat_info_obj.max_diag_tx_acl_delay)
    stat_info_obj.max_diag_tx_acl_delay = (time_2 - time_1);
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_tx_acl);
}

void BtState::SetPreTsDiagTxHciCmd(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt++;
  }
  memset(stat_info_obj.pre_diag_tx_hci, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.pre_diag_tx_hci, buf,
    sizeof(stat_info_obj.pre_diag_tx_hci));
  stat_info_obj.diag_tx_hci_start = tv;
  ALOGV("%s:is %s", __func__, stat_info_obj.pre_diag_tx_hci);
}

void BtState::SetPostTsDiagTxHciCmd(char* buf, struct timeval tv) {
  {
    std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
    stat_info_obj.diag_pending_pkt--;
  }
  memset(stat_info_obj.post_diag_tx_hci, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.post_diag_tx_hci, buf,
    sizeof(stat_info_obj.post_diag_tx_hci));
  long long time_1 = stat_info_obj.diag_tx_hci_start.tv_sec*1e3*1ll +
                                stat_info_obj.diag_tx_hci_start.tv_usec*1e-3*1ll;
  long long time_2 = tv.tv_sec*1e3*1ll + tv.tv_usec*1e-3*1ll;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    time_2 = time_1;
  }

  if ((time_2 - time_1) > stat_info_obj.max_diag_tx_hci_delay)
    stat_info_obj.max_diag_tx_hci_delay = (time_2 - time_1);
  ALOGV("%s:is %s", __func__, stat_info_obj.post_diag_tx_hci);
}

bool BtState::isDiagLogApiStuck() {
  std::unique_lock<std::mutex> lock(diag_log_pending_mutex);
  if (stat_info_obj.diag_pending_pkt > 0) {
    return true;
  }
  return false;
}

void BtState::SetOTPInfoBytes(uint8_t size, char* buf) {
  char hex_buff[3] = "";
  memset(stat_info_obj.otp_info_bytes, 0, OTP_INFO_BYTES_MAX_SIZE);
  if (size == 0) {
    snprintf(stat_info_obj.otp_info_bytes, OTP_INFO_BYTES_MAX_SIZE,
             "HCI_VS_PERSIST_ACCESS_OTP_GET cmd failed %s", buf);
    return;
  }
  snprintf(stat_info_obj.otp_info_bytes, OTP_INFO_BYTES_MAX_SIZE,
           "OTP info. length: %d\nOTP bytes are as follows:\n", size);
  int offset = strlen(stat_info_obj.otp_info_bytes);
  memset(hex_buff, '\0', 3);
  for (int i = 0; i < size && offset < OTP_INFO_BYTES_MAX_SIZE-3; i++) {
    snprintf(hex_buff, 3, "%.2X ", buf[i]);
    stat_info_obj.otp_info_bytes[offset++] = hex_buff[0];
    stat_info_obj.otp_info_bytes[offset++] = hex_buff[1];
    stat_info_obj.otp_info_bytes[offset++] = 32; //space
  }
  stat_info_obj.otp_info_bytes[offset] = '\0';
  ALOGV("%s:is %s", __func__, stat_info_obj.otp_info_bytes);
}

void BtState::UpdateBqrRieErrCodeAndErrString(int crash_code, std::string crash_string) {
  snprintf(stat_info_obj.bqr_rie_info, sizeof(stat_info_obj.bqr_rie_info),
            "BQR RIE Crash Code : 0x%.2X \nBQR RIE Crash String : %s",
              crash_code, crash_string.c_str());
}

void BtState::UpdateBQRVSParams(std::string& params) {
  stat_info_obj.bqr_rie_vs_params = params;
}

void BtState::AddDelayListInfo(std::vector<DelayListElement>& delay_list) {
    ALOGD("%s: Adding Init phase delay list info to state file", __func__);
    stat_info_obj.delay_list_info = "Delay list elements:\n";
    for (DelayListElement& element: delay_list) {
      stat_info_obj.delay_list_info += "  Activity: " + element.activity_info + "\n";
      stat_info_obj.delay_list_info += "    Start time: " + element.start_time;
      stat_info_obj.delay_list_info += "    End time: " + element.end_time;
      stat_info_obj.delay_list_info += "    Extra Delay(ms): " + std::to_string(element.time_diff);
      stat_info_obj.delay_list_info += "    Total Time(ms): " + std::to_string(element.time_taken) + "\n";
    }
}

void BtState::PrintDelayListInfo() {
    ALOGE("%s: %s", __func__, stat_info_obj.delay_list_info.c_str());
}

void BtState::SetFailStatusForIpcLog(char* buf) {
  memset(stat_info_obj.uart_ipc_err, 0, MAX_BUFF_SIZE);
  strlcpy(stat_info_obj.uart_ipc_err, buf, sizeof(stat_info_obj.uart_ipc_err));
  ALOGV("%s:is %s", __func__, stat_info_obj.uart_ipc_err);
}

void BtState::DeleteDumpsIfRequired()
{
  struct DIR* p_dir;
  struct dirent* p_dirent;
  char *first_entry = NULL;
  int count = 0;

  char *look_for = (char *)"ramdump_bt_state";

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }
  while ((p_dirent = readdir(p_dir)) != NULL) {
    if (strstr(p_dirent->d_name, look_for)) {
      count++;
      if(count == 1) {
        first_entry = p_dirent->d_name;
      }
    }
  }
  if (count >= HAL_DUMP_ITERATION_LIMIT)
    DeleteDumps(first_entry);

  closedir(p_dir);
}

void BtState::DeleteDumps(char *first_entry)
{
  struct DIR* p_dir;
  struct dirent* p_dirent;
  int ret = 0;
  char *look_for = (char *)"ramdump_bt_state";
  char timestamp[HAL_DUMP_TIMESTAMP_LENGTH];
  char path[HAL_DUMP_SIZE];
  char oldest_file[HAL_DUMP_FILE_SIZE];

  strlcpy(oldest_file, first_entry, HAL_DUMP_FILE_SIZE);

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }

  while ((p_dirent = readdir(p_dir)) != NULL) {
    if (strstr(p_dirent->d_name, look_for) ) {
        if(strcmp(p_dirent->d_name, oldest_file) < 0) {
        strlcpy(oldest_file, p_dirent->d_name, HAL_DUMP_FILE_SIZE);
      }
    }
  }
  closedir(p_dir);

  strlcpy(timestamp, oldest_file + HAL_DUMP_TIMESTAMP_OFFSET + 1, HAL_DUMP_TIMESTAMP_LENGTH);

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }

  while ((p_dirent = readdir(p_dir)) != NULL) {
   if (strstr(p_dirent->d_name, timestamp)) {
     strlcpy(path, LOG_COLLECTION_DIR, HAL_DUMP_SIZE);
     strlcat(path, p_dirent->d_name, HAL_DUMP_SIZE);
     ALOGV("%s: Deleting oldest dump file: %s", __func__, path);
     ret = remove(path);
     if(ret < 0) ALOGE("%s: Cannot delete file %s", __func__, path);
   }
  }
  closedir(p_dir);
}

void BtState::DumpBtState() {
  int fd;
  char file_name[BT_FILE_NAME_LEN];
  char hex_buff[3] = "";
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  int j =0;
  int ssrlvl;

#ifndef DONT_DELETE_DUMPS_SET
  DeleteDumpsIfRequired();
#endif

  /* Get the exact time stamp of close */
  Logger* logger = Logger::Get();
  logger->GetStateFileName(file_name);
  ssrlvl = logger->ReadSsrLevel();

  fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
            | S_IROTH);
  if (fd < 0) {
     ALOGE("%s: File open (%s) failed: errno: %d", __func__, file_name, errno);
     return;
  }

  ALOGD("%s: Dumping stats into %s", __func__, file_name);

  ALOGD("%s", stat_info_obj.pri_crash_reason);
  write(fd, stat_info_obj.pri_crash_reason, strlen(stat_info_obj.pri_crash_reason));

  write(fd, " \n", 2);
  ALOGD("%s",stat_info_obj.sec_crash_reason);
  write(fd, stat_info_obj.sec_crash_reason, strlen(stat_info_obj.sec_crash_reason));

  write(fd, " \n", 2);
  ALOGD("%s",stat_info_obj.bqr_rie_info);
  write(fd, stat_info_obj.bqr_rie_info, strlen(stat_info_obj.bqr_rie_info));

  if (stat_info_obj.bqr_rie_vs_params.length()) {
    write(fd, " \n", 2);
    ALOGD("%s",stat_info_obj.bqr_rie_vs_params.c_str());
    write(fd, stat_info_obj.bqr_rie_vs_params.c_str(), stat_info_obj.bqr_rie_vs_params.length());
  }

  write(fd, " \n", 2);
  ALOGD("%s",stat_info_obj.crash_ts);
  write(fd, stat_info_obj.crash_ts, strlen(stat_info_obj.crash_ts));

  ALOGD("%s ", stat_info_obj.kernel_time);
  write(fd, stat_info_obj.kernel_time, strlen(stat_info_obj.kernel_time));

  SetAlwaysOnEnabled();
  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.alwayson_status);
  write(fd, stat_info_obj.alwayson_status, strlen(stat_info_obj.alwayson_status));

  snprintf(dst_buff, sizeof(dst_buff), "SSR LVL = %d", ssrlvl);
  write(fd, " \n", 2);
  ALOGD("SSR LVL = %d", ssrlvl);
  write(fd, dst_buff, strlen(dst_buff));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_hci_initialize);
  write(fd, stat_info_obj.ts_hci_initialize, strlen(stat_info_obj.ts_hci_initialize));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pre_sibs_property_read);
  write(fd, stat_info_obj.pre_sibs_property_read, strlen(stat_info_obj.pre_sibs_property_read));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.post_sibs_property_read);
  write(fd, stat_info_obj.post_sibs_property_read, strlen(stat_info_obj.post_sibs_property_read));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_hci_close);
  write(fd, stat_info_obj.ts_hci_close, strlen(stat_info_obj.ts_hci_close));
  if (soc_need_reload_patch) {
    write(fd, " \n", 2);
#ifdef BT_GANGES_SUPPORTED
    ALOGD("%s", stat_info_obj.ts_peri_arbit_ntf);
    write(fd, stat_info_obj.ts_peri_arbit_ntf,
          strlen(stat_info_obj.ts_peri_arbit_ntf));
    write(fd, " \n", 2);
#endif
    ALOGD("%s", stat_info_obj.pre_local_addr);
    write(fd, stat_info_obj.pre_local_addr,
          strlen(stat_info_obj.pre_local_addr));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_local_addr);
    write(fd, stat_info_obj.post_local_addr, strlen(stat_info_obj.post_local_addr));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_fetch_vendor_addr);
    write(fd, stat_info_obj.pre_fetch_vendor_addr,
      strlen(stat_info_obj.pre_fetch_vendor_addr));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_fetch_vendor_addr);
    write(fd, stat_info_obj.post_fetch_vendor_addr,
      strlen(stat_info_obj.post_fetch_vendor_addr));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.ts_getver_start_send_cmd);
    write(fd, stat_info_obj.ts_getver_start_send_cmd,
          strlen(stat_info_obj.ts_getver_start_send_cmd));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.ts_getver_start_read_rsp);
    write(fd, stat_info_obj.ts_getver_start_read_rsp,
          strlen(stat_info_obj.ts_getver_start_read_rsp));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_version_info);
    write(fd, stat_info_obj.pre_version_info,
          strlen(stat_info_obj.pre_version_info));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_version_info);
    write(fd, stat_info_obj.post_version_info, strlen(stat_info_obj.post_version_info));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.ts_getver_rcvd);
    write(fd, stat_info_obj.ts_getver_rcvd, strlen(stat_info_obj.ts_getver_rcvd));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_chk_patch_path);
    write(fd, stat_info_obj.pre_chk_patch_path,
          strlen(stat_info_obj.pre_chk_patch_path));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_chk_patch_path);
    write(fd, stat_info_obj.post_chk_patch_path, strlen(stat_info_obj.post_chk_patch_path));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_patch_open);
    write(fd, stat_info_obj.pre_patch_open,
          strlen(stat_info_obj.pre_patch_open));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_patch_open);
    write(fd, stat_info_obj.post_patch_open, strlen(stat_info_obj.post_patch_open));
    if (stat_info_obj.is_xmem_enabled) {
      write(fd, " \n", 2);
      ALOGD("%s", stat_info_obj.pre_xmem_patch_open);
      write(fd, stat_info_obj.pre_xmem_patch_open,
            strlen(stat_info_obj.pre_xmem_patch_open));

      write(fd, " \n", 2);
      ALOGD("%s", stat_info_obj.post_xmem_patch_open);
      write(fd, stat_info_obj.post_xmem_patch_open, strlen(stat_info_obj.post_xmem_patch_open));

      write(fd, " \n", 2);
      ALOGD("%s", stat_info_obj.pre_xmem_nvm_open);
      write(fd, stat_info_obj.pre_xmem_nvm_open,
            strlen(stat_info_obj.pre_xmem_nvm_open));

      write(fd, " \n", 2);
      ALOGD("%s", stat_info_obj.post_xmem_nvm_open);
      write(fd, stat_info_obj.post_xmem_nvm_open, strlen(stat_info_obj.post_xmem_nvm_open));
    }
    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_nvm_open);
    write(fd, stat_info_obj.pre_nvm_open,
          strlen(stat_info_obj.pre_nvm_open));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_nvm_open);
    write(fd, stat_info_obj.post_nvm_open, strlen(stat_info_obj.post_nvm_open));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_tcs_config);
    write(fd, stat_info_obj.pre_tcs_config, strlen(stat_info_obj.pre_tcs_config));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_tcs_config);
    write(fd, stat_info_obj.post_tcs_config, strlen(stat_info_obj.post_tcs_config));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_su_build_info);
    write(fd, stat_info_obj.pre_su_build_info,
          strlen(stat_info_obj.pre_su_build_info));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_su_build_info);
    write(fd, stat_info_obj.post_su_build_info, strlen(stat_info_obj.post_su_build_info));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.fw_dwnld_success);
    write(fd, stat_info_obj.fw_dwnld_success, strlen(stat_info_obj.fw_dwnld_success));

    write(fd, " \n", 2);
    char tmp_buf1[128];
    add_time_str(tmp_buf1, &stat_info_obj.fw_su_build_info_set_time);
    snprintf(tmp_buf1 + strlen(tmp_buf1), sizeof(tmp_buf1), " %s", stat_info_obj.fw_su_build_info);
    ALOGD("%s", tmp_buf1);
    write(fd, tmp_buf1, strlen(tmp_buf1));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.otp_info_bytes);
    write(fd, stat_info_obj.otp_info_bytes, strlen(stat_info_obj.otp_info_bytes));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.bt_soc_id);
    write(fd, stat_info_obj.bt_soc_id, strlen(stat_info_obj.bt_soc_id));
#ifdef BT_GANGES_SUPPORTED
    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.peri_fw_dwnld_success);
    write(fd, stat_info_obj.peri_fw_dwnld_success, strlen(stat_info_obj.peri_fw_dwnld_success));

    write(fd, " \n", 2);
    char tmp_buf2[128];
    add_time_str(tmp_buf2, &stat_info_obj.peri_fw_su_build_info_set_time);
    snprintf(tmp_buf2 + strlen(tmp_buf2), sizeof(tmp_buf2), " %s", stat_info_obj.peri_fw_su_build_info);
    ALOGD("%s", tmp_buf2);
    write(fd, tmp_buf2, strlen(tmp_buf2));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.peri_soc_id);
    write(fd, stat_info_obj.peri_soc_id, strlen(stat_info_obj.bt_soc_id));
    
    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.peri_pre_patch_open);
    write(fd, stat_info_obj.peri_pre_patch_open,
    strlen(stat_info_obj.peri_pre_patch_open));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.peri_post_patch_open);
    write(fd, stat_info_obj.peri_post_patch_open, strlen(stat_info_obj.peri_post_patch_open));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.ts_peri_arbitration_sent);
    write(fd, stat_info_obj.ts_peri_arbitration_sent,
    strlen(stat_info_obj.ts_peri_arbitration_sent));
#endif
  }

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.power_resources_state.bt);
  ALOGD("%s", stat_info_obj.power_resources_state.uwb);
  ALOGD("%s", stat_info_obj.power_resources_state.com);
  write(fd, stat_info_obj.power_resources_state.bt, strlen(stat_info_obj.power_resources_state.bt));
  write(fd, stat_info_obj.power_resources_state.uwb, strlen(stat_info_obj.power_resources_state.uwb));
  write(fd, stat_info_obj.power_resources_state.com, strlen(stat_info_obj.power_resources_state.com));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.bt_uart_cts);
  write(fd, stat_info_obj.bt_uart_cts,
  strlen(stat_info_obj.bt_uart_cts));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.read_thread_start);
  write(fd, stat_info_obj.read_thread_start, strlen(stat_info_obj.read_thread_start));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.status_read_thread);
  write(fd, stat_info_obj.status_read_thread, strlen(stat_info_obj.status_read_thread));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.pre_diag_init_ts);
  write(fd, stat_info_obj.pre_diag_init_ts, strlen(stat_info_obj.pre_diag_init_ts));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.post_diag_init_ts);
  write(fd, stat_info_obj.post_diag_init_ts, strlen(stat_info_obj.post_diag_init_ts));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_status_diag_init);
  write(fd, stat_info_obj.ts_status_diag_init, strlen(stat_info_obj.ts_status_diag_init));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_status_ctrlr_init);
  write(fd, stat_info_obj.ts_status_ctrlr_init, strlen(stat_info_obj.ts_status_ctrlr_init));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_status_init_cb_sent);
  write(fd, stat_info_obj.ts_status_init_cb_sent, strlen(stat_info_obj.ts_status_init_cb_sent));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_uart_flow_on);
  write(fd, stat_info_obj.ts_uart_flow_on, strlen(stat_info_obj.ts_uart_flow_on));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_uart_flow_off);
  write(fd, stat_info_obj.ts_uart_flow_off, strlen(stat_info_obj.ts_uart_flow_off));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.pre_diag_deinit_ts);
  write(fd, stat_info_obj.pre_diag_deinit_ts, strlen(stat_info_obj.pre_diag_deinit_ts));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.post_diag_deinit_ts);
  write(fd, stat_info_obj.post_diag_deinit_ts, strlen(stat_info_obj.post_diag_deinit_ts));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.health_timer_status);
  write(fd, stat_info_obj.health_timer_status, strlen(stat_info_obj.health_timer_status));

  write(fd, " \n", 2);
  HealthInfoLog::Get()->ReportHealthInfo();
  ALOGD("%s", stat_info_obj.last_health_stat_info);
  write(fd, stat_info_obj.last_health_stat_info, strlen(stat_info_obj.last_health_stat_info));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.ts_hci_internal_cmd);
  write(fd, stat_info_obj.ts_hci_internal_cmd, strlen(stat_info_obj.ts_hci_internal_cmd));

  write(fd, " \n", 2);
  ALOGD("%s ", stat_info_obj.ts_hci_internal_rsp);
  write(fd, stat_info_obj.ts_hci_internal_rsp, strlen(stat_info_obj.ts_hci_internal_rsp));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_hci_cmd_stack);
  write(fd, stat_info_obj.last_hci_cmd_stack, strlen(stat_info_obj.last_hci_cmd_stack));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_last_seq_number);
  write(fd, stat_info_obj.ts_last_seq_number, strlen(stat_info_obj.ts_last_seq_number));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_ind_from_host);
  write(fd, stat_info_obj.last_wk_ind_from_host, strlen(stat_info_obj.last_wk_ind_from_host));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_ack_from_soc);
  write(fd, stat_info_obj.last_wk_ack_from_soc, strlen(stat_info_obj.last_wk_ack_from_soc));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_ind_from_soc);
  write(fd, stat_info_obj.last_wk_ind_from_soc, strlen(stat_info_obj.last_wk_ind_from_soc));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_ack_from_host);
  write(fd, stat_info_obj.last_wk_ack_from_host, strlen(stat_info_obj.last_wk_ack_from_host));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_sleep_ind_from_host);
  write(fd, stat_info_obj.last_sleep_ind_from_host, strlen(stat_info_obj.last_sleep_ind_from_host));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_sleep_ind_from_soc);
  write(fd, stat_info_obj.last_sleep_ind_from_soc, strlen(stat_info_obj.last_sleep_ind_from_soc));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pre_stack_event);
  write(fd, stat_info_obj.pre_stack_event, strlen(stat_info_obj.pre_stack_event));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.post_stack_event);
  write(fd, stat_info_obj.post_stack_event, strlen(stat_info_obj.post_stack_event));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pre_stack_acl);
  write(fd, stat_info_obj.pre_stack_acl, strlen(stat_info_obj.pre_stack_acl));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.post_stack_acl);
  write(fd, stat_info_obj.post_stack_acl, strlen(stat_info_obj.post_stack_acl));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pre_stack_iso);
  write(fd, stat_info_obj.pre_stack_iso, strlen(stat_info_obj.pre_stack_iso));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.post_stack_iso);
  write(fd, stat_info_obj.post_stack_iso, strlen(stat_info_obj.post_stack_iso));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pre_select);
  write(fd, stat_info_obj.pre_select, strlen(stat_info_obj.pre_select));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.post_select);
  write(fd, stat_info_obj.post_select, strlen(stat_info_obj.post_select));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_uart_clock_on);
  write(fd, stat_info_obj.ts_uart_clock_on, strlen(stat_info_obj.ts_uart_clock_on));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ts_uart_clock_off);
  write(fd, stat_info_obj.ts_uart_clock_off, strlen(stat_info_obj.ts_uart_clock_off));

  //last Tx packet
  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_tx_pkt_ts);
  write(fd, stat_info_obj.last_tx_pkt_ts, strlen(stat_info_obj.last_tx_pkt_ts));

  {
    j =0;
    memset(hex_buff, '\0', 3);
    for (int i = 0; i < stat_info_obj.last_tx_pkt_len; i++) {
      snprintf(hex_buff, 3, "%.2X ", stat_info_obj.last_tx_pkt_asc[i]);
      stat_info_obj.last_tx_pkt_hex[j++] = hex_buff[0];
      stat_info_obj.last_tx_pkt_hex[j++] = hex_buff[1];
      stat_info_obj.last_tx_pkt_hex[j++] = 32;
    }
    stat_info_obj.last_tx_pkt_hex [j] = '\0';
    write(fd, " \n", 2);
    write(fd, stat_info_obj.last_tx_pkt_hex, strlen(stat_info_obj.last_tx_pkt_hex));
    ALOGD("%s", stat_info_obj.last_tx_pkt_hex);
    snprintf(dst_buff, sizeof(dst_buff), "Actual last TX pkt len = %d",
             stat_info_obj.actual_tx_pkt_len);
    write(fd, " \n", 2);
    write(fd, dst_buff, strlen(dst_buff));
    ALOGD("Actual last TX pkt len: %d", stat_info_obj.actual_tx_pkt_len);
  }

  //last Rx packet before SSR
  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.ssr_rx_pkt_ts);
  write(fd, stat_info_obj.ssr_rx_pkt_ts, strlen(stat_info_obj.ssr_rx_pkt_ts));

  {
    j =0;
    memset(hex_buff, '\0', 3);
    for (int i = 0; i < stat_info_obj.ssr_rx_pkt_len; i++) {
      snprintf(hex_buff, 3, "%.2X ", stat_info_obj.ssr_rx_pkt_asc[i]);
      stat_info_obj.ssr_rx_pkt_hex[j++] = hex_buff[0];
      stat_info_obj.ssr_rx_pkt_hex[j++] = hex_buff[1];
      stat_info_obj.ssr_rx_pkt_hex[j++] = 32;
    }
    stat_info_obj.ssr_rx_pkt_hex [j] = '\0';
    write(fd, " \n", 2);
    write(fd, stat_info_obj.ssr_rx_pkt_hex, strlen(stat_info_obj.ssr_rx_pkt_hex));
    ALOGD("%s", stat_info_obj.ssr_rx_pkt_hex);
    memset(dst_buff, '\0', MAX_BUFF_SIZE);
    snprintf(dst_buff, sizeof(dst_buff), "Actual RX pkt len before SSR = %d",
             stat_info_obj.actual_ssr_rx_pkt_len);
    write(fd, " \n", 2);
    write(fd, dst_buff, strlen(dst_buff));
    ALOGD("Actual RX pkt len before SSR: %d", stat_info_obj.actual_ssr_rx_pkt_len);
  }

  //last Rx packet
  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_rx_pkt_ts);
  write(fd, stat_info_obj.last_rx_pkt_ts, strlen(stat_info_obj.last_rx_pkt_ts));

  {
    j =0;
    memset(hex_buff, '\0', 3);
    for (int i = 0; i < stat_info_obj.last_rx_pkt_len; i++) {
      snprintf(hex_buff, 3, "%.2X ", stat_info_obj.last_rx_pkt_asc[i]);
      stat_info_obj.last_rx_pkt_hex[j++] = hex_buff[0];
      stat_info_obj.last_rx_pkt_hex[j++] = hex_buff[1];
      stat_info_obj.last_rx_pkt_hex[j++] = 32;
    }
    stat_info_obj.last_rx_pkt_hex [j] = '\0';
    write(fd, " \n", 2);
    write(fd, stat_info_obj.last_rx_pkt_hex, strlen(stat_info_obj.last_rx_pkt_hex));
    ALOGD("%s", stat_info_obj.last_rx_pkt_hex);
    memset(dst_buff, '\0', MAX_BUFF_SIZE);
    snprintf(dst_buff, sizeof(dst_buff), "Actual last RX pkt len = %d",
             stat_info_obj.actual_rx_pkt_len);
    write(fd, " \n", 2);
    write(fd, dst_buff, strlen(dst_buff));
    ALOGD("Actual last RX pkt len: %d", stat_info_obj.actual_rx_pkt_len);
  }

  // Invalid packet
  if (stat_info_obj.inv_bytes_len) {
    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.inv_bytes_ts);
    write(fd, stat_info_obj.inv_bytes_ts, strlen(stat_info_obj.inv_bytes_ts));

    {
      j = 0;
      memset(hex_buff, '\0', 3);
      for (int i = 0; i < stat_info_obj.inv_bytes_len; i++) {
        snprintf(hex_buff, 3, "%.2X ", stat_info_obj.inv_bytes_asc[i]);
        stat_info_obj.inv_bytes_hex[j++] = hex_buff[0];
        stat_info_obj.inv_bytes_hex[j++] = hex_buff[1];
        stat_info_obj.inv_bytes_hex[j++] = 32;
      }
      stat_info_obj.inv_bytes_hex[j] = '\0';
      write(fd, " \n", 2);
      write(fd, stat_info_obj.inv_bytes_hex, strlen(stat_info_obj.inv_bytes_hex));
      ALOGD("%s", stat_info_obj.inv_bytes_hex);
      snprintf(dst_buff, sizeof(dst_buff), "Num Invalid Bytes = %d",
             stat_info_obj.inv_bytes_len);
      write(fd, " \n", 2);
      write(fd, dst_buff, strlen(dst_buff));
      ALOGD("Num Invalid Bytes: %d", stat_info_obj.inv_bytes_len);
    }
  }

#ifdef DIAG_ENABLED
  if (logger->GetDiagInitStatus() && logger->IsSnoopLogEnabled()) {
    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_diag_rx_acl);
    write(fd, stat_info_obj.pre_diag_rx_acl, strlen(stat_info_obj.pre_diag_rx_acl));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_diag_rx_acl);
    write(fd, stat_info_obj.post_diag_rx_acl, strlen(stat_info_obj.post_diag_rx_acl));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_diag_rx_hci);
    write(fd, stat_info_obj.pre_diag_rx_hci, strlen(stat_info_obj.pre_diag_rx_hci));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_diag_rx_hci);
    write(fd, stat_info_obj.post_diag_rx_hci, strlen(stat_info_obj.post_diag_rx_hci));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_diag_tx_acl);
    write(fd, stat_info_obj.pre_diag_tx_acl, strlen(stat_info_obj.pre_diag_tx_acl));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_diag_tx_acl);
    write(fd, stat_info_obj.post_diag_tx_acl, strlen(stat_info_obj.post_diag_tx_acl));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.pre_diag_tx_hci);
    write(fd, stat_info_obj.pre_diag_tx_hci, strlen(stat_info_obj.pre_diag_tx_hci));

    write(fd, " \n", 2);
    ALOGD("%s", stat_info_obj.post_diag_tx_hci);
    write(fd, stat_info_obj.post_diag_tx_hci, strlen(stat_info_obj.post_diag_tx_hci));

    snprintf(dst_buff, sizeof(dst_buff), "Max diag ACL RX logging delay = %d",
              stat_info_obj.max_diag_rx_acl_delay);
    write(fd, " \n", 2);
    ALOGD("Max diag ACL RX delay = %d", stat_info_obj.max_diag_rx_acl_delay);
    write(fd, dst_buff, strlen(dst_buff));

    snprintf(dst_buff, sizeof(dst_buff), "Max diag HCI RX logging delay = %d",
              stat_info_obj.max_diag_rx_hci_delay);
    write(fd, " \n", 2);
    ALOGD("Max diag HCI RX delay = %d", stat_info_obj.max_diag_rx_hci_delay);
    write(fd, dst_buff, strlen(dst_buff));

    snprintf(dst_buff, sizeof(dst_buff), "Max diag ACL TX logging delay = %d",
              stat_info_obj.max_diag_tx_acl_delay);
    write(fd, " \n", 2);
    ALOGD("Max diag ACL TX delay = %d", stat_info_obj.max_diag_tx_acl_delay);
    write(fd, dst_buff, strlen(dst_buff));

    snprintf(dst_buff, sizeof(dst_buff), "Max diag HCI TX logging delay = %d",
              stat_info_obj.max_diag_tx_hci_delay);
    write(fd, " \n", 2);
    ALOGD("Max diag HCI TX delay = %d", stat_info_obj.max_diag_tx_hci_delay);
    write(fd, dst_buff, strlen(dst_buff));
  }
#endif

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.stuck_issue);
  write(fd, stat_info_obj.stuck_issue, strlen(stat_info_obj.stuck_issue));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_lck_acq_info);
  write(fd, stat_info_obj.last_wk_lck_acq_info, strlen(stat_info_obj.last_wk_lck_acq_info));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.last_wk_lck_rel_info);
  write(fd, stat_info_obj.last_wk_lck_rel_info, strlen(stat_info_obj.last_wk_lck_rel_info));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.pc_addr);
  write(fd, stat_info_obj.pc_addr, strlen(stat_info_obj.pc_addr));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.build_ver);
  write(fd, stat_info_obj.build_ver, strlen(stat_info_obj.build_ver));

  write(fd, " \n", 2);
  ALOGD("%s", stat_info_obj.delay_list_info.c_str());
  write(fd, stat_info_obj.delay_list_info.c_str(), stat_info_obj.delay_list_info.length());

  if (fsync(fd) == -1) {
    ALOGE("%s: Error while synchronization of logs in :%s error code:%s",
          __func__, file_name, strerror(errno));
  }
  close(fd);
}
} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
