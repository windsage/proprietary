/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _BTTPI_H_
#define _BTTPI_H_


#include <aidl/vendor/qti/hardware/bttpi/BnBtTpi.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpiStatusCb.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpiStatusRspCb.h>
#include <aidl/vendor/qti/hardware/bttpi/BtTpiState.h>
#include <mutex>
#include <queue>
#include <vector>
#include <hidl/HidlSupport.h>
#include "controller.h"
#include "data_handler.h"
#include <poll.h>

using ::aidl::vendor::qti::hardware::bttpi::BnBtTpi;
using ::aidl::vendor::qti::hardware::bttpi::BtTpiState;
using ::aidl::vendor::qti::hardware::bttpi::IBtTpiStatusCb;
using ::aidl::vendor::qti::hardware::bttpi::IBtTpiEventsCb;
using ::aidl::vendor::qti::hardware::bttpi::IBtTpiStatusRspCb;
using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using android::hardware::bluetooth::V1_0::implementation::DataHandler;

// TPI Commands
const uint8_t HCI_VS_STX_BT_CFG_IND             = 0x1B;
const uint8_t HCI_VS_STX_BT_SAR_TABLE_IND       = 0x1C;
const uint8_t HCI_VS_STX_BT_NE_LIMIT_UPDATE_IND = 0x1D;
const uint8_t HCI_VS_STX_AIRPLANE_MODE_STATUS   = 0x1E;
const uint8_t HCI_VS_STX_BT_ENABLE_IND          = 0x20;
const uint8_t HCI_VS_STX_ACTIVE_DSI_IND         = 0x21;
const uint8_t HCI_VS_STX_WLAN_STATE_IND         = 0x23;
const uint8_t HCI_VS_STX_FEATURE_SUPPORT_REQ    = 0x25;

const uint8_t HCI_VS_STX_BT_INACTIVE_LIMIT_UPDATE_IND  = 0x26;

const uint8_t HCI_STX_FRAGMENT_HDR_SIZE         = 0x05;
const uint8_t HCI_STX_ASYNC_EVT_DATA_OFFSET     = 0x07;
const uint8_t HCI_STX_ASYNC_VSE_DATA_OFFSET     = 0x04;
// TPI Async Events
const uint8_t HCI_VS_COEX_STX_BT_PWR_REPORT_IND = 0x1B;
const uint8_t HCI_VS_COEX_STX_BT_MAX_PWR_IND    = 0x1C;
const uint8_t HCI_VS_COEX_STX_BT_STATE_IND      = 0x1D;

// TPI command status
const uint8_t TPI_CMD_STATUS_SUCCESS        = 0x00;
const uint8_t TPI_CMD_STATUS_FAILURE        = 0x01;
const uint8_t TPI_CMD_STATUS_BUSY_TRY_LATER = 0x02;
const uint8_t TPI_CMD_STATUS_BTSOC_TIMEOUT  = 0x03;

// TPI max data in a blob
const uint8_t TPI_CMD_MAX_BLOB_SIZE         = 246;

// Timer Value
const uint32_t BTTPI_TIMER_VALUE_MS         = 2500;

class BtTpi : public BnBtTpi
{
private:
  static std::shared_ptr<IBtTpiEventsCb> eventCb;
  static std::shared_ptr<IBtTpiStatusCb> statusCb;
  static std::shared_ptr<IBtTpiStatusRspCb> statusRspCb;
  static bool is_timer_created;
  static timer_t bttpi_timer;
  void sendDataToController(std::vector<uint8_t>& data, int8_t* retValPtr);
  void sendFragment(int8_t* retValPtr);
  static void BtTpiTimerExpired();
  void BtTpiTimerStart();
  static void BtTpiTimerStop();
  static void BtTpiTimerCleanup();
  void BtTpiTxThreadRoutine();
  static bool isCmdPending;

  static uint8_t pendingReq;
  static uint8_t nextSeqNum;
  static uint16_t remainingLen;
  static uint8_t totalFragments;
  static std::vector<uint8_t> reqParams;
  static std::vector<uint8_t> rspEvent;
  static AIBinder_DeathRecipient* deathObj;
  static int read_fd_, write_fd_;
  static bool getCmdPendingStatus();
  static void setCmdPendingStatus(bool status);
  static void ClosePipe(void);
  static int Notifythread(uint8_t);
public:
  BtTpi();
  virtual ~BtTpi();

  ::ndk::ScopedAStatus setTpiState(BtTpiState in_state, const std::shared_ptr<IBtTpiStatusCb>& in_statusCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus setActiveDSI(int32_t in_dsi, const std::shared_ptr<IBtTpiStatusCb>& in_statusCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus setAirplaneMode(bool in_airPlaneMode, const std::shared_ptr<IBtTpiStatusCb>& in_statusCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus setWiFiState(bool in_wifiState, const std::shared_ptr<IBtTpiStatusCb>& in_statusCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus setTpiParams(int32_t in_reqType, const std::vector<uint8_t>& in_reqParams, const std::shared_ptr<IBtTpiStatusCb>& in_statusCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus registerTpiEventsCallback(const std::shared_ptr<IBtTpiEventsCb>& in_eventCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus UnRegisterTpiEventsCallback(int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus getTpiVersion(int8_t in_version, const std::shared_ptr<IBtTpiStatusRspCb>& in_statusRspCb, int8_t* _aidl_return) override;
  ::ndk::ScopedAStatus setTpiParamsRsp(int32_t in_reqType, const std::vector<uint8_t>& in_reqParams, const std::shared_ptr<IBtTpiStatusRspCb>& in_statusRspCb, int8_t* _aidl_return) override;

  static void deathRecipient(void* cookie);
};

#endif // _BTTPI_H_
