/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef BT_GANGES_SUPPORTED
#pragma once

#include <cutils/properties.h>
#include "data_handler.h"
#include <hidl/HidlSupport.h>
#include <hci_uart_transport.h>
#include <logger.h>
#include "power_manager.h"
#include "patch_dl_manager.h"
#include <stdint.h>

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define HCI_MAX_CMD_SIZE                     260
#define HCI_MAX_EVENT_SIZE                   260
#define PRINT_BUF_SIZE                       ((HCI_MAX_CMD_SIZE * 3) + 2)
#define GET_VERSION_OCF                      0x1E
#define HCI_VENDOR_CMD_OGF                   0x3F

#define EVT_CMD_COMPLETE_SIZE                3
#define EVT_CMD_STATUS                       0x0F
#define EVT_CMD_STATUS_SIZE                  4
#define HCI_EVENT_HDR_SIZE                   2

#define HCI_CMD_IND                          (1)
#define EVENTCODE_OFFSET                     (1)
#define PERI_EVENTCODE_OFFSET                (2)
#define EVT_PLEN                             (2)
#define PLEN                                 (3)
#define CMD_RSP_OFFSET                       (3)
#define RSP_TYPE_OFFSET                      (4)
#define BAUDRATE_RSP_STATUS_OFFSET           (4)
#define CMD_STATUS_OFFSET                    (5)
#define P_CHIP_VER_OFFSET                    (4)
#define P_BUILD_VER_OFFSET                   (6)
#define P_BASE_ADDR_OFFSET                   (8)
#define P_ENTRY_ADDR_OFFSET                  (12)
#define P_LEN_OFFSET                         (16)
#define P_CRC_OFFSET                         (20)
#define P_CONTROL_OFFSET                     (24)
#define PATCH_HDR_LEN                        (28)
#define MAX_DATA_PER_SEGMENT                 (239)
#define VSEVENT_CODE                         (0xFF)
#define HC_VS_MAX_CMD_EVENT                  (0xFF)
#define PATCH_PROD_ID_OFFSET                 (5)
#define PATCH_PATCH_VER_OFFSET               (9)
#define PATCH_ROM_BUILD_VER_OFFSET           (11)
#define PATCH_SOC_VER_OFFSET                 (13)
#define MAX_SIZE_PER_TLV_SEGMENT             (243)

/* Peri Commands */
#define PERI_VSC_SET_BAUDRATE_REQ_LEN                (2)
#define PERI_EDL_PATCH_CONFIG_CMD_LEN                (6)
#define EDL_PATCH_ARBIT_CMD_LEN                      (3)
#define EDL_PATCH_CMD_ARBIT_LEN                      (3)
#define EDL_BOARD_ID_CMD_LEN                         (1)
#define EDL_PATCH_CMD_ACTIVATESS_LEN                 (3)
#define EDL_PATCH_CMD_PERI_RESET_LEN                 (1)
#define HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ        (0x08)
#define HCI_PERI_TLV_DOWNLOAD_REQ                    (0x06)
#define HCI_PERI_TLV_PATCH_CONFIG                    (0x07)
#define HCI_PERI_EDL_PATCH_GETVER                    (0x05)
#define HCI_PERI_CMD_SSID_OFFSET                     (0x04)
#define HCI_PERI_RESET                               (0x03)
#define HCI_PERI_EDL_GET_BUILD_INFO                  (0x09)
#define HCI_PERI_EDL_GET_BOARD_ID                    (0x0A)
#define HCI_PERI_GENERAL_CMD_OCF                     (0x301)
#define HCI_PERI_ACTIVATE_SS                         (0x00)
#define HCI_PERI_EVT_ACTIVATE_NTF_ACTION_OFFSET      (0x06)
#define HCI_VS_GENERAL_OPCODE_PERI                   (0xFFF1)
#define EVT_PERI_CMD_COMPLETE                        (0xFF)
#define HCI_VS_DNLD_OPCODE_PERI                      (0xFFF0)
#define CMD_RSP_SS_ID_OFFSET                         (0x0B)
#define CMD_RSP_STATUS_OFFSET                        (0X09)
#define PERI_MSG_TYPE_CMD_STATUS                     (0X00)
#define PERI_MSG_TYPE_CMD_CMPL			     (0X01)
#define HCI_PERI_SET_BAUDRATE                        (0x02)
#define PERI_PLEN                                    (4)
#define HOST_ID_LEN                                  (1)
#define HCI_VS_PERI_DEBUG_OPCODE                     (0xFFF3)

#define PERI_EDL_PATCH_CMD_LEN           (2)
#define HCI_PERI_CMD_IND_LEN             (2)

/* Payload length offset for peri commands */
#define HCI_PERI_CMD_PLEN_OFFSET             (0x02)
#define HCI_PERI_EVENT_CODE                  (0xF0)
#define PERI_CMD_STATUS_OFFSET               (0x09)
#define HCI_VS_PERI_DEBUG_OPCODE             (0xFFF3)


/* Peri Events */
#define ARBIT_RSP_SSID_OFFSET                    (0x0B)
#define HCI_PERI_EVT_PATCH_STATUS_OFFSET         (0x06)
#define HCI_PERI_EVT_MT_CMD_STATUS               (0x00)
#define HCI_PERI_EVT_MT_CMD_CMPLT                (0x01)
#define HCI_PERI_EVT_MT_SS_ACT_CMPLT             (0x02)
#define HCI_PERI_EVT_MT_SS_PATCH_NTF             (0x03)

/* Event class */
#define HCI_PERI_EVT_CLASS                         (0xF0)
#define HCI_VS_EVENT_CLASS_OFFSET                  (0x03)
#define HCI_VS_EVENT_MT_OFFSET                     (0x04)
#define HCI_VS_EVENT_STATUS_OFFSET                 (0x05)
#define HCI_VS_PATCH_NTF_SSID_OFFSET               (0x06)
#define HCI_PERI_EVT_ACTIVATE_NTF_SSID_OFFSET      (0x05)
#define HCI_PERI_EVT_ACTIVATE_NTF_ACTION_OFFSET    (0x06)

/* Event Message Type */
#define HCI_PERI_EDL_SUBSYSTEM_PATCH_NOTIFICATION (0x10)
#define HCI_PERI_EVT_SS_ACTIVATE_COMPLETE         (0x02)
#define BT_CONTROLLER_STATUS                      (0x00)
#define PERI_EVENTCODE_OFFSET                     (2)

/* Status Codes of HCI CMD execution*/
#define HCI_CMD_SUCCESS                      (0x0)
#define NVM_ACCESS_CODE                      (0x0B)
#define BAUDRATE_CHANGE_SUCCESS              (1)
#define HCI_PERI_EDL_PATCH_SUCCESS           (0x0)
#define HCI_PERI_BUILD_INFO_SUCCESS          (0x0)
#define HCI_PERI_VER_SUCCESS                 (0x0)
#define HCI_PERI_TLV_STATUS                  (0x09)

/* Peri FW path */
#define PERI_FW_REGULAR_PATH     "/vendor/bt_firmware/image/"
#define PERI_RAMPATCH_PATH       "/vendor/bt_firmware/image/"
#define PERI_NVM_PATH            "/vendor/bt_firmware/image/"
#define PERI_FW_QTI_ALT_PATH     "/vendor/bt_firmware/image/"
#define PERI_FW_ALT_PATH         "/vendor/firmware/"

/* TME-L FW path */
#define TMEL_FW_REGULAR_PATH		"/vendor/firmware_mnt/image/"
#define TME_RAMPATCH_PATH		"/vendor/bt_firmware/image/"
#define TME_FW_QTI_ALT_PATH		"/vendor/bt_firmware/image/"
#define TME_FW_ALT_PATH			"/vendor/firmware/"


namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

/******************************************************************************
**  Local type definitions
******************************************************************************/

// GANGES Peri Build details
enum {
  GANGES_PERI_BUILD_VER_0100 = 0x0100,
  GANGES_PERI_BUILD_VER_0200 = 0x0200
};

enum {
  QCA_GANGES_PERI_SOC_ID_0100 = 0x40210100,
  QCA_GANGES_PERI_SOC_ID_0200 = 0x40210200
};

enum {
  GANGES_PERI_VER_1_0   = QCA_BT_VER(QCA_GANGES_PERI_SOC_ID_0100, PROD_ID_GANGES_PERI, GANGES_PERI_BUILD_VER_0100),
  GANGES_PERI_VER_2_0   = QCA_BT_VER(QCA_GANGES_PERI_SOC_ID_0200, PROD_ID_GANGES_PERI, GANGES_PERI_BUILD_VER_0200)
};

class PeriPatchDLManager {
 public:
  PeriPatchDLManager(void);
  PeriPatchDLManager(BluetoothSocType soc_type, HciUartTransport* transport);
  int PerformChipInit();
  int SetBaudRateReq();
  uint64_t GetChipVersion();
  int ActivateSS(byte subsysId, HciActivate action);
  int PeriReset(void);
  void ReadSocDump(void);
  void FramePeriHciPkt(unsigned char *cmd, unsigned short opcode, byte subOpcode, unsigned int p_base_addr, int segtNo, uint8_t size);
  ~PeriPatchDLManager();

 protected:
  int PeriSocInit(void);
  int PeriPatchDownload (void);
  int ProcessPeriEdlPatchEvt(unsigned char *rsp, unsigned char paramlen);
  int DownloadTlvFile();
  int PatchVerReq();
  int PatchConfigReq();
  int Arbitrate();
  int PeriPatchVerReq();
  int SetPeriBaudRateReq();
  int GetVsHciEvent(unsigned char *rsp);
  void FrameHciCmdPkt(unsigned char *cmd, int edl_cmd, unsigned int p_base_addr,
                         int segtNo, int size);
  int GetBuildInfoReq();
  int HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size);
  int ReadVsAsyncEvent(unsigned char* buf, int size, byte msgType);
  int ReadHciEvent(unsigned char* buf, int size);
  int TlvDnldReq(int tlv_size);
  int GetBoardIdReq(void);
  int GetTlvFile(const char *file_path);
  bool ReadTlvInfo();
  int TlvDnldSegment(int index, int seg_size, unsigned char wait_cc_evt);
  int ReadHciPeriEvent(unsigned char* buf, int size);
  void LoadPatchMaptable();
  void FormDefaultPaths();
  void SetPeriRampatchRegularPaths(std::string);
  void SetPeriNVMRegularPaths(std::string);
  int CheckAndSetProperRampatchAltPath();
  void FormRegularPaths(PatchPathManager* info);
  void DownloadAndCmdExeStatus(int err);
  unsigned char Convert2AsciiPeri(unsigned char temp);
  void HandleEdlCmdResEvt(unsigned char subOpcode, unsigned char paramlen, unsigned char* rsp);
  FILE* OpenPatchFile(const char *file_path);
  int ConfigurePeriSoC(void);
#ifdef DEBUG_CRASH_SOC_WHILE_DLD
  int SendCrashCommand();
#endif
  void ReadELFConfig();
  int WakeUpSoC(void);
  int ReleasePeriAccess(void);
  int RequestPeriAccess(void);
  int TmelSocInit(void);
  int TMEPatchVerReq(void);
  int TMEArbitrate(void);
  int ConfigureTMESoC(void);
  int TMEPatchDownload(void);
  void LoadTMELPatchMaptable(void);
  int TMEReset(void);
  bool tmel;
  HciPatchStatus tme_patch_status_;
  int tlv_type_;

 private:
  BluetoothSocType soc_type_;
  void CheckForWakeupMechanism(unsigned char *nvm_byte_ptr, int offset);
  HciUartTransport* uart_transport_;
  int fd_transport_;
  Logger *bt_logger_;
  Logger *tmel_logger_;
  int dnld_fd_in_progress_;
  unsigned char *pdata_buffer_;
  unsigned char tlv_dwn_cfg_;
  unsigned char wait_vsc_evt_;
  bool patch_dnld_pending_;
  unsigned char board_id_[BOARD_ID_LEN];
  std::map<uint64_t, PatchPathManager *> PeriPatchPathInfoMap_;
  std::map<uint64_t, PatchPathManager *> TMEPatchPathInfoMap_;
  char rampatch_file_path[FILE_PATH_LEN] = {'\0'};
  char nvm_file_path[FILE_PATH_LEN] = {'\0'};
  char rampatch_alt_file_path[FILE_PATH_LEN] = {'\0'};
  char nvm_alt_file_path[FILE_PATH_LEN] = {'\0'};
  char elf_config_[PROPERTY_VALUE_MAX] = "no_value";
  bool elf_config_read_ = false;
  HciPatchStatus peri_patch_status_;
  HciActivate peri_activate_action_;
  bool soc_wakeup_in_progress = false;
};


} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
#endif
