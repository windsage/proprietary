/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

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
#include "logger.h"
#include "notify_signal.h"
#ifdef WCNSS_OBS_ENABLED
#include "obs_handler.h"
#else
#include "ibs_handler.h"
#endif
#include <sys/stat.h>
#include <regex>

#define UNUSED(x) (void)(x)
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth@1.1-peri_patch_dl_manager"

#define PERI_VERSION_FILEPATH "/data/vendor/bluetooth/peri_bt_fw_version.txt"
#define TME_VERSION_FILEPATH  "/data/vendor/bluetooth/tme_bt_fw_version.txt"
#define MSB_NIBBLE_MASK 0xF0
#define LSB_NIBBLE_MASK 0x0F
#define WAKEUP_CONFIG_BYTE_CHE_3_AND_LATER   4
#define WAKEUP_CONFIG_MASK  0x03
#define WAKEUP_CONFIG_UART_RxD_CONFIG  1
#define WAKE_IND_RETRIES           10
#define WAKE_IND_TIMEOUT           10

uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud);
int userial_tcio_baud_to_int(uint32_t baud, uint8_t soc_type);

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

extern int power_state_;
#ifdef USER_DEBUG
version_info version_info_ins_peri = {0, 0, "No Build label"};
version_info version_info_ins_tme = {0, 0, "No Build label"};
#endif
unsigned short patchversion = 0;
unsigned int security_version = 0;
unsigned int productid = 0;
unsigned short buildversion = 0;
unsigned int soc_id = 0;
char build_label[255];

bool is_alt_path_for_peri_fw_to_be_used = false;
bool is_alt_path_for_tme_fw_to_be_used = false;
char alt_path_for_peri_fw[PROPERTY_VALUE_MAX] = {'\0'};
char alt_path_for_tme_fw[PROPERTY_VALUE_MAX] = {'\0'};
uint64_t chipset_peri_ver_ = 0;
uint64_t chipset_tme_ver_ = 0;
extern bool peri_patching_in_progress;

extern bool tme_patching_in_progress;

PeriPatchDLManager::PeriPatchDLManager(void)
{
  ALOGI("%s: Calling %s constructor.\n", __func__, __func__);
}

PeriPatchDLManager::PeriPatchDLManager(BluetoothSocType soc_type, HciUartTransport* transport) :
  soc_type_(soc_type), uart_transport_(transport), dnld_fd_in_progress_(-1)
{
  ALOGD("%s", __func__);
  bool tmel = false;
  bt_logger_ = Logger::Get();
  fd_transport_ = uart_transport_->GetCtrlFd();
  /* wait_vsc_evt_ is set to true as peri SoC responds
   * with vendor specific command complete format
   */
  wait_vsc_evt_ = true;
  patch_dnld_pending_ = false;
  elf_config_read_ = false;
  soc_wakeup_in_progress = false;
  LoadPatchMaptable();
  LoadTMELPatchMaptable();
  peri_patch_status_ = HCI_PERI_PATCHING_UNKNOWN;
  peri_activate_action_ = HCI_ACTION_UNKNOWN;
}

PeriPatchDLManager::~PeriPatchDLManager()
{
  patch_dnld_pending_ = false;
  for(auto & element: PeriPatchPathInfoMap_)
    delete element.second;
  PeriPatchPathInfoMap_.clear();
  for(auto & element: TMEPatchPathInfoMap_)
  	delete element.second;
  TMEPatchPathInfoMap_.clear();
}

void PeriPatchDLManager::ReadSocDump()
{
  int tot_len = 0;
  unsigned char buf[HCI_MAX_EVENT_SIZE];

  ALOGE("%s: Start\n", __func__);

  while (1) {

    tot_len = ReadHciPeriEvent(buf, HCI_MAX_EVENT_SIZE);
    if (tot_len < 0) {
      ALOGE("%s: Error while reading the hci event", __func__);
    }

    if (buf[0] == HCI_PACKET_TYPE_EVENT) {
      bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len - 1);
    } else  {
      bt_logger_->ProcessRx(HCI_PACKET_TYPE_PERI_EVT, &buf[1], tot_len - 1);
    }
  }

  ALOGE("%s: Stop\n", __func__);
}

int PeriPatchDLManager::PerformChipInit()
{
  int ret = 0, flags;
  char dst_buff[MAX_BUFF_SIZE] = {0};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW ON");
  /* Workaround UART issue: Make sure RTS is flowed ON in case it
   * was not flowed on during cleanup due to UART issue.
   */
  ret = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags);
  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);

  if (ret < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, ret);
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_HW_FLOW_ON_FAILED);
    return ret;
  }

  ret = RequestPeriAccess();
  if (ret < 0) {
    ALOGE("%s: failed to access to SoC", __func__);
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_ACCESS_DISALLOWED);
    return ret;
  }

  ret = PeriSocInit();
  gettimeofday(&tv, NULL);
  if (ret < 0) {
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Peri SoC initialization failed");
    ALOGE("%s: SoC initialization failed: %d\n", __func__, ret);
    ret = -2;
  } else {
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Peri SoC initialization successful");
  }
  BtState::Get()->SetPeriFwDwnldSucceed(dst_buff);

  ReleasePeriAccess();
  return ret;
}

int PeriPatchDLManager::ActivateSS(byte ssId , HciActivate action)
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  char res_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ALOGI("%s: Sending Activate SS CMD to PeriSoC", __func__);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Sending Activate SS CMD to PeriSoc");
  BtState :: Get()->SetPeriActivateSsReqTS(dst_buff, SEND_CMD_TS);
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK,"SENDING ACTIVATE SS CMD");
  /* Frame the HCI CMD to be sent to the Controller */
  FramePeriHciPkt(cmd,
              HCI_VS_GENERAL_OPCODE_PERI,
              HCI_PERI_ACTIVATE_SS,
              0,
              -1,
              EDL_PATCH_CMD_ACTIVATESS_LEN);

  cmd[6] = BT_SS;
  cmd[7] = action;  // Action
  /* Total length of the packet to be sent to the Controller */
  size = (HCI_PERI_CMD_IND_LEN  + HCI_COMMAND_HDR_SIZE + EDL_PATCH_ARBIT_CMD_LEN);
  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send command payload to the Controller!");
    err = -1;
    goto error;
  }
  if (peri_activate_action_ == HCI_REPEATED_ATTEMPTS) {
    ALOGW("%s: skip waiting for notification event", __func__);
    goto error;
  }
  peri_activate_action_ = HCI_ACTION_UNKNOWN;
  // Wait for activate complete notification
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_BT_ACTIVATE_NTF_STUCK);
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if (err < 0) {
    ALOGE("Failed to read notification!");
    err = -1;
    goto error;
  }
  if(err != -1) action = peri_activate_action_;
  if (action == peri_activate_action_) {
    ALOGI("%s: Activate Command Successful. action(%d)!", __func__, action);
  } else {
    ALOGE("%s: Activate Command Failed. Invalid received-action(%d), expected-action(%d)!",
    __func__,
    peri_activate_action_,
    action);
    err = -1;
  }
  peri_activate_action_ = HCI_ACTION_UNKNOWN;
  gettimeofday(&tv, NULL);
  snprintf(res_buff, sizeof(res_buff), "Peri activate rsp rcvd, num bytes in last rsp = %d", err);
  BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
  BtState :: Get()->SetPeriActivateSsResEvtTS(dst_buff);

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::PeriReset(void)
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  char res_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ALOGD("%s: Sending Peri Reset cmd to Peri SoC", __func__);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Sending Peri Reset cmd to Peri Soc");
  BtState :: Get()->SetPeriResetReqTS(dst_buff);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_RESET_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_RESET_STUCK,
                                            "SENDING PERI RESET CMD");
  FramePeriHciPkt(cmd, HCI_VS_GENERAL_OPCODE_PERI, HCI_PERI_RESET, 0, -1,
                  EDL_PATCH_CMD_PERI_RESET_LEN);
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_PERI_RESET_LEN);
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);

  if ( err != size) {
    ALOGE("Failed to send command payload to the Controller!");
    err = -1;
    goto error;
  }
  gettimeofday(&tv, NULL);
  snprintf(res_buff, sizeof(res_buff), "Peri reset rsp rcvd, num bytes in last rsp = %d", err);
  BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
  BtState :: Get()->SetPeriResetResEvtTS(dst_buff);
  bt_logger_->SetSecondaryCrashReason(PERI_SOC_REASON_DEFAULT);

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::Arbitrate()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  char res_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ALOGD("%s: Sending Arbitration CMD to PeriSoC", __func__);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Sending Arbitration CMD to PeriSoc");
  BtState :: Get()->SetPeriArbitReqTS(dst_buff);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_ARBITRATION_CMD_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_ARBITRATION_CMD_STUCK,
                                            "SENDING PERI ARBITRATION CMD");
  /* Frame the HCI CMD to be sent to the Controller */
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ, 0, -1,
                  EDL_PATCH_CMD_ARBIT_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_ARBIT_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if (err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    err = -1;
    goto error;
  }

  gettimeofday(&tv, NULL);
  snprintf(res_buff, sizeof(res_buff), "Peri SS Arbitration rsp = %d", err);
  dst_buff[0] = '\0';
  BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
  BtState :: Get()->SetPeriArbitEvtTS(dst_buff);
  bt_logger_->SetSecondaryCrashReason(PERI_SOC_REASON_DEFAULT);

error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::PeriPatchVerReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  char res_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Sending Get Version CMD to Peripheral SOC");
  BtState :: Get()->SetPeriGetVerReqTS(dst_buff, SEND_CMD_TS);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_GETVER_SEND_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_GETVER_SEND_STUCK,
                                            "SENDING PERI-GET VERSION CMD");

  /* Frame the HCI CMD to be sent to the Controller */
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_PATCH_GETVER, 0, -1,
                  PERI_EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + PERI_EDL_PATCH_CMD_LEN);
  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);

  if (err != size) {
    ALOGE("%s: Failed to attach the patch payload to the Controller!",__func__);
    err = -1;
    goto error;
  } else if (rsp[CMD_RSP_STATUS_OFFSET] != HCI_PERI_VER_SUCCESS || rsp[CMD_RSP_SS_ID_OFFSET] != PERI_SS) {
    ALOGE("%s: Failed to get valid response : 0x%x" , __func__, err);
    err = -1;
    goto error;
  } else {
    ALOGI("%s: rsp[CMD_RSP_OFFSET] = %x ,  rsp[RSP_TYPE_OFFSET] = %x", __func__, rsp[CMD_RSP_OFFSET], rsp[RSP_TYPE_OFFSET]);
  }

  gettimeofday(&tv, NULL);
  snprintf(res_buff, sizeof(res_buff), "Peri Get Version rsp rcvd, num bytes in last rsp = %d", err);
  BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
  BtState :: Get()->SetPeriGetVerResEvtTS(dst_buff);
  bt_logger_->SetSecondaryCrashReason(PERI_SOC_REASON_DEFAULT);

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::PatchConfigReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_PATCH_CONFIG_CMD_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_PATCH_CONFIG_CMD_STUCK,
                                            "ELF PATCH CONFIG CMD");
  /* Frame the HCI CMD to be sent to the Controller */
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI,
		  HCI_PERI_TLV_PATCH_CONFIG, 0, -1, PERI_EDL_PATCH_CONFIG_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + PERI_EDL_PATCH_CONFIG_CMD_LEN);

  ALOGD("%s: Sending Patch config CMD to SOC", __func__);
  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if (err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    err = -1;
    goto error;
  }

error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::GetBuildInfoReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  ALOGE("%s",__func__);

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_GETBLDINFO_CMD_STUCK,
                                          "BUILD INFO CMD & RSP");

  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_GET_BUILD_INFO, 0, -1,
  PERI_EDL_PATCH_CMD_LEN);

  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + PERI_EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send get build info cmd to the SoC!");
    goto error;
  }

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::GetTlvFile(const char *file_path)
{
  FILE * pFile = NULL;
  int fileSize;
  int readSize;

  if (pFile == NULL) {
    pFile = OpenPatchFile(file_path);
  }

  if( pFile == NULL) {
    return -1;
  }

  /* Get File Size */
  fseek(pFile, 0, SEEK_END);
  fileSize = ftell(pFile);
  rewind(pFile);

  pdata_buffer_ = (unsigned char*)new char[fileSize];
  if (pdata_buffer_ == NULL) {
    ALOGE("Allocated Memory failed");
    fclose(pFile);
    return -1;
  }
  /* Copy file into allocated buffer */
  readSize = fread(pdata_buffer_, 1, fileSize, pFile);

  /* File Close */
  fclose(pFile);

  if (readSize != fileSize) {
    ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)", readSize, fileSize);
    delete []pdata_buffer_;
    return -1;
  }

  if (ReadTlvInfo())
    return readSize;
  else {
    delete []pdata_buffer_;
    return -1;
  }
}

bool PeriPatchDLManager::ReadTlvInfo() {
  int nvm_length, nvm_tot_len, nvm_index, i;
  bool status = false;
  unsigned short nvm_tag_len;
  tlv_patch_info *ptlv_header;
  tlv_nvm_hdr *nvm_ptr;
  unsigned char data_buf[PRINT_BUF_SIZE] = { 0, };
  unsigned char *nvm_byte_ptr;
  ptlv_header = (tlv_patch_info*)pdata_buffer_;

  /* checking for type of patch file */
  if (pdata_buffer_[0] == ELF_FLAG && !memcmp(&pdata_buffer_[1], "ELF", 3)) {
   tlv_type_ = ELF_TYPE_PATCH;
  } else {
    /* To handle different event between rampatch and NVM */
    tlv_type_ = ptlv_header->tlv_type;
    tlv_dwn_cfg_ = ptlv_header->tlv.patch.dwnd_cfg;
  }

  if (tlv_type_ == ELF_TYPE_PATCH) {
    ALOGI("====================================================");
    ALOGI("ELF Type Patch File\t\t\t : 0x%x %c %c %c", pdata_buffer_[0], pdata_buffer_[1],
          pdata_buffer_[2], pdata_buffer_[3]);
    ALOGI("File Class\t\t\t : 0x%x", pdata_buffer_[4]);
    ALOGI("Data Encoding\t\t\t : 0x%x", pdata_buffer_[5]);
    ALOGI("File version\t\t\t : 0x%x", pdata_buffer_[6]);
    ALOGI("====================================================");
    status = true;
  } else if (ptlv_header->tlv_type == TLV_TYPE_PATCH) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    ALOGI("Length\t\t\t : %d bytes", (ptlv_header->tlv_length1) |
          (ptlv_header->tlv_length2 << 8) |
          (ptlv_header->tlv_length3 << 16));
    ALOGI("Total Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_data_len);
    ALOGI("Patch Data Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_patch_data_len);
    ALOGI("Signing Format Version\t : 0x%x", ptlv_header->tlv.patch.sign_ver);
    ALOGI("Signature Algorithm\t\t : 0x%x", ptlv_header->tlv.patch.sign_algorithm);
    ALOGI("Event Handling\t\t\t : 0x%x", ptlv_header->tlv.patch.dwnd_cfg);
    ALOGI("Reserved\t\t\t : 0x%x", ptlv_header->tlv.patch.reserved1);
    ALOGI("Product ID\t\t\t : 0x%04x\n", ptlv_header->tlv.patch.prod_id);
    ALOGI("Rom Build Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.build_ver);
    ALOGI("Patch Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.patch_ver);
    ALOGI("Reserved\t\t\t : 0x%x\n", ptlv_header->tlv.patch.reserved2);
    ALOGI("Patch Entry Address\t\t : 0x%x\n", (ptlv_header->tlv.patch.patch_entry_addr));
    ALOGI("====================================================");
    status = true;
  } else if (ptlv_header->tlv_type >= TLV_TYPE_BT_NVM) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    nvm_tot_len = nvm_length  = (ptlv_header->tlv_length1) |
                                (ptlv_header->tlv_length2 << 8) |
                                (ptlv_header->tlv_length3 << 16);
    ALOGI("Length\t\t\t : %d bytes",  nvm_tot_len);
    ALOGI("====================================================");

    if (nvm_tot_len <= 0)
      return status;

 multi_nvm:
    for (nvm_byte_ptr = (unsigned char*)(nvm_ptr = &(ptlv_header->tlv.nvm)), nvm_index = 0;
         nvm_index < nvm_length; nvm_ptr = (tlv_nvm_hdr*)nvm_byte_ptr) {
      ALOGV("TAG ID\t\t\t : %d", nvm_ptr->tag_id);
      nvm_tag_len = nvm_ptr->tag_len;
      ALOGV("TAG Length\t\t : %d", nvm_tag_len);
      ALOGV("TAG Pointer\t\t : %d", nvm_ptr->tag_ptr);
      ALOGV("TAG Extended Flag\t : %d", nvm_ptr->tag_ex_flag);

      /* Increase nvm_index to NVM data */
      nvm_index += sizeof(tlv_nvm_hdr);
      nvm_byte_ptr += sizeof(tlv_nvm_hdr);

      /* Update Tag#17: HCI UART Settings */
      if (nvm_ptr->tag_id == TAG_NUM_17) {
        uint8_t baudrate = uart_transport_->GetMaxBaudrate();
      
        ALOGI("%s: baudrate %02x", __func__, baudrate);
      
        /* Byte#1: UART baudrate */
        *(nvm_byte_ptr + 1) = baudrate;
      }

      /* IBS Handling */
      if (nvm_ptr->tag_id == 27) {
        if (!IbsHandler::IsEnabled()) {
          /* TxP Sleep Mode: Disable */
          *(nvm_byte_ptr + 1) &= ~0x01;
          ALOGI("%s: SIBS Disable", __func__);
        } else {
          /* TxP Sleep Mode-1:UART_SIBS, 2:USB_SUSPEND, 3: GPIO_OOB, 4: UART_HWIBS */
          *(nvm_byte_ptr + 1) |= 0x01;
          /* Check for wakeup config */
          CheckForWakeupMechanism(nvm_byte_ptr, WAKEUP_CONFIG_BYTE_CHE_3_AND_LATER);
          ALOGI("%s: SIBS Enable", __func__);
        }
      }

      for (i = 0; (i < nvm_ptr->tag_len && (i * 3 + 2) < PRINT_BUF_SIZE); i++)
        snprintf((char*)data_buf, PRINT_BUF_SIZE, "%s%.02x ", (char*)data_buf, *(nvm_byte_ptr + i));

      ALOGV("TAG Data\t\t\t : %s", data_buf);

      /* Clear buffer */
      memset(data_buf, 0x0, PRINT_BUF_SIZE);

      /* increased by tag_len */
      nvm_index += nvm_ptr->tag_len;
      nvm_byte_ptr += nvm_ptr->tag_len;
    }

    nvm_tot_len -= nvm_index;

    if (nvm_tot_len > 4 ) {
      nvm_byte_ptr = (unsigned char*)ptlv_header;
      nvm_byte_ptr += (4 + nvm_index);
      ptlv_header = (tlv_patch_info*)nvm_byte_ptr;
      nvm_tot_len -= 4;
      ALOGI("====================================================");
      ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
      nvm_length = (ptlv_header->tlv_length1) |
                   (ptlv_header->tlv_length2 << 8) |
                   (ptlv_header->tlv_length3 << 16);
      ALOGI("Length\t\t\t : %d bytes",  nvm_length);
      ALOGI("====================================================");
      goto multi_nvm;
    }

    ALOGI("====================================================");
    status = true;

  } else {
    ALOGE("TLV Header type is unknown (%d) ", ptlv_header->tlv_type);
  }

  return status;
}

void PeriPatchDLManager::CheckForWakeupMechanism(unsigned char *nvm_byte_ptr, int offset) {
   // Wakeup source config
   uint8_t wakeup_config = 0;
   // Check for Bit 3:2 for wakeup configuration
   wakeup_config = (*(nvm_byte_ptr + offset) >> 2) & WAKEUP_CONFIG_MASK;
 #ifdef WCNSS_IBS_ENABLED
   IbsHandler* ibs_ins = IbsHandler::Get();
   if (ibs_ins && wakeup_config == WAKEUP_CONFIG_UART_RxD_CONFIG) {
    ibs_ins->SetRxDwakeupEnabled();
   }
 #endif
  ALOGD("%s: Wakeup source config : %u", __func__, wakeup_config);
}

FILE* PeriPatchDLManager::OpenPatchFile(const char *file_path) {
  FILE *pFile = NULL;

  if (!(file_path && (pFile = fopen(file_path, "r" )))) {
    ALOGE("%s File Open Fail %s (%d)", file_path, strerror(errno), errno);
    return NULL;
  } else {
    ALOGI("File open %s succeeded", file_path);
    return pFile;
  }
}

int PeriPatchDLManager::TlvDnldSegment(int index, int seg_size, unsigned char wait_cc_evt)
{
  int size = 0, err = -1;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  ALOGV("%s: Downloading TLV Patch segment no.%d, size:%d", __func__, index, seg_size);
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_TLV_DOWNLOAD_REQ, 0, index, seg_size);
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + cmd[PERI_PLEN]);

  ALOGV("%s: Sending pkt of size %d", __func__, size);
  /* Initialize the RSP packet everytime to 0 */
  memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if (tmel) {
    if (err != size) {
      ALOGE("Failed to send the TME-L patch payload to the Controller! 0x%x", err);
      return err;
     }
    ALOGV("%s: Successfully downloaded TME-L patch segment: %d", __func__, index);
  } else {  	
	  if (err != size) {
    ALOGE("Failed to send the peri patch payload to the Controller! 0x%x", err);
    return err;
	  } else if (peri_patch_status_ == HCI_PERI_PATCHING_NOT_DONE && rsp[11] != PERI_SS) {
    ALOGE("Invalid/wrong Subsystem ID sent in response!");
    err = -1;
  }
  ALOGV("%s: Successfully downloaded peri patch segment: %d", __func__, index);
  }
  
  return err;
}

int PeriPatchDLManager::TlvDnldReq(int tlv_size)
{
  int total_segment, remain_size, i, err = -1;
  unsigned char wait_cc_evt = true;
  bool is_last_seg = false;
  int segment_download_len = MAX_SIZE_PER_TLV_SEGMENT;

  total_segment = tlv_size / MAX_SIZE_PER_TLV_SEGMENT;
  remain_size = (tlv_size < MAX_SIZE_PER_TLV_SEGMENT) ? \
                tlv_size : (tlv_size % MAX_SIZE_PER_TLV_SEGMENT);

  ALOGI("%s: TLV size: %d, Total Seg num: %d, remain size: %d",
        __func__, tlv_size, total_segment, remain_size);

    if (tlv_type_ == TLV_TYPE_PATCH || tlv_type_ == ELF_TYPE_PATCH) {

    switch (tlv_dwn_cfg_) {
      case SKIP_EVT_NONE:
        wait_vsc_evt_ = true;
        wait_cc_evt = true;
        ALOGI("Event handling type: SKIP_EVT_NONE");
        break;
      case SKIP_EVT_VSE_CC:
        wait_vsc_evt_ = false;
        wait_cc_evt = false;
        ALOGI("Event handling type: SKIP_EVT_VSE_CC");
        break;
      /* Not handled for now */
      case SKIP_EVT_VSE:
      case SKIP_EVT_CC:
      default:
        ALOGE("Unsupported Event handling: %d", tlv_dwn_cfg_);
        break;
    }
   } else {
     wait_vsc_evt_ = true;
     wait_cc_evt = true;
   }

  for (i = 0; i <= total_segment && !is_last_seg; i++) {
    /* check for last segment based on remaining size
     * and total number of segments.
     */
    if ((remain_size && i == total_segment) ||
        (!remain_size && (i + 1) == total_segment)) {
      is_last_seg = true;
      // Update segment download len if last segment is being downloaded
      if (remain_size)
        segment_download_len = remain_size;
      ALOGD("%s: Updating seg len to %d as last segment",
        __func__, segment_download_len);
    }

    if ((tlv_type_ == TLV_TYPE_PATCH || tlv_type_ == ELF_TYPE_PATCH) && is_last_seg) {
      /*
       * 1. None of the command segments receive CCE
       * 2. No command segments receive VSE except the last one
       * 3. If tlv_dwn_cfg_ is SKIP_EVT_NONE then wait for VSE and CCE
       * ( except CCE is not received for last segment)
       */
      wait_cc_evt = false;
      wait_vsc_evt_ = true;
    }

    patch_dnld_pending_ = true;
    if ((err = TlvDnldSegment(i, segment_download_len, wait_cc_evt )) < 0) {
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_TLV_DOWNLOAD_FAILED);
      goto error;
    }
    patch_dnld_pending_ = false;
  }

 error:
  if (patch_dnld_pending_)
    patch_dnld_pending_ = false;
  return err;
}

int PeriPatchDLManager::GetBoardIdReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  bool cmd_supported = true;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_GETBOARDID_CMD_STUCK,
                                          "GET BOARDID CMD & RSP");

  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_GET_BOARD_ID, 0, -1,
                  EDL_BOARD_ID_CMD_LEN);
  size = (HCI_PERI_CMD_IND_LEN  + HCI_COMMAND_HDR_SIZE + EDL_BOARD_ID_CMD_LEN);

  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_GET_BOARD_ID command!");
    cmd_supported = false;
  }

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return (cmd_supported == true ? err : -1);
}

int PeriPatchDLManager::DownloadTlvFile()
{
  int tlv_size = -1;
  int err = -1;
  char nvm_file_path_bid[256] = { 0, };
  int nvm_file_path_len = strlen(nvm_file_path);
  int board_id_cmd_status = -1;
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  /* Rampatch TLV file Downloading */
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_RAM_PATCH_READ_STUCK,
                                            "PERI RAM PATCH OPEN & READ");
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading patch file");
  BtState :: Get()->SetPrePeriPatchOpenTS(dst_buff);
  if ((tlv_size = GetTlvFile(rampatch_file_path)) > 0) {
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading patch file");
    BtState :: Get()->SetPostPeriPatchOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
    if (tlv_type_ == ELF_TYPE_PATCH) {
      ReadELFConfig();
      if (strcmp(elf_config_, "no_value") != 0) {
        err = PatchConfigReq();
        if (err < 0) {
          ALOGE("%s: Patch config CMD Failed", __func__);
          bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_PATCH_CONFIG_FAILED);
          delete []pdata_buffer_;
          return err;
        } else {
          unsigned long elf_config;
          /* converting hex in string to real hex number */
          elf_config = strtoul (elf_config_, NULL, 16);
          /* Checking BIT 1:
           * BIT 1: Whether to perform Parallel (1) or Sequential 
           * validation (0)
           */
          if ((elf_config & 2) == 0) {
            if (tlv_dwn_cfg_ == SKIP_EVT_NONE)
              bt_logger_->UpdateElfPatchIdealDelay(tlv_size, false);
            else if (tlv_dwn_cfg_ == SKIP_EVT_VSE_CC)
              bt_logger_->UpdateElfPatchIdealDelay(tlv_size, true);
          }
        }
      } else {
        tlv_dwn_cfg_ = SKIP_EVT_VSE_CC;
      }
    } else {
      bt_logger_->UpdateRamPatchIdealDelay(tlv_size, tlv_dwn_cfg_ == SKIP_EVT_NONE ? false : true);
    }
    bt_logger_->SetSecondaryCrashReason( BT_HOST_REASON_PERI_PATCH_DNLD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_PATCH_DNLD_STUCK,"PERI RAM PATCH DOWNLOAD");
    err = TlvDnldReq(tlv_size);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    delete []pdata_buffer_;
    if (err < 0) {
      return err;
    }
  } else {
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading peri patch file");
    BtState :: Get()->SetPostPatchOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }

  if (tmel) {
    ALOGI("%s: returning from here as tmel doesn't support nvm download", __func__);
    return err;
  }

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_GETBOARDID_CMD_STUCK);
  if ((board_id_cmd_status = GetBoardIdReq()) < 0) {
    ALOGE("%s: failed to get PERI board id(0x%x)", __func__, err);
  }

  /* NVM TLV file Downloading */
  ALOGI("%s: Peripheral NVM Download", __func__);
  tlv_size = -1;
  err = -1;

  ALOGI("%s: nvm file path %s", __func__, nvm_file_path);

  if (board_id_cmd_status  != -1) {
    if (nvm_file_path_len != 0) {
      memcpy(nvm_file_path_bid, nvm_file_path, nvm_file_path_len - 2);
      strlcat(nvm_file_path_bid, (char*)board_id_, sizeof(nvm_file_path_bid));
    }
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_NVM_PATCH_READ_STUCK, "PERI NVM file with BID OPEN & READ");
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading Peri NVM file");
    BtState :: Get()->SetPrePeriNVMOpenTS(dst_buff);
    if ((tlv_size = GetTlvFile(nvm_file_path_bid)) < 0) {
      ALOGI("%s: %s: file doesn't exist, falling back to default file", __func__, nvm_file_path_bid);
      gettimeofday(&tv, NULL);
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading Peri NVM file");
      BtState :: Get()->SetPostPeriNVMOpenTS(dst_buff);
      bt_logger_->CheckAndAddToDelayList(&tv);
    } else {
      gettimeofday(&tv, NULL);
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading Peri NVM file");
      BtState :: Get()->SetPostPeriNVMOpenTS(dst_buff);
      bt_logger_->CheckAndAddToDelayList(&tv);
    }
  }

  if (tlv_size < 0) {
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_NVM_PATCH_READ_STUCK, "PERI NVM file OPEN & READ");
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading Peri NVM file");
    BtState :: Get()->SetPrePeriNVMOpenTS(dst_buff);
    if ((tlv_size = GetTlvFile(nvm_file_path)) < 0) {
      gettimeofday(&tv, NULL);
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading Peri NVM file");
      BtState :: Get()->SetPostPeriNVMOpenTS(dst_buff);
      bt_logger_->CheckAndAddToDelayList(&tv);
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_NVM_FILE_NOT_FOUND);
      return err;
    }
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading Peri NVM file");
    BtState :: Get()->SetPostPeriNVMOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }
  bt_logger_->UpdateNvmPatchIdealDelay(tlv_size);
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_NVM_DNLD_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_NVM_DNLD_STUCK, "Peri NVM file download");
  err = TlvDnldReq(tlv_size);
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  delete []pdata_buffer_;
  return err;
}

uint64_t PeriPatchDLManager :: GetChipVersion()
{
  return chipset_peri_ver_;
}

int PeriPatchDLManager::PeriPatchDownload (void)
{
  int err =0;
  PatchPathManager* info = NULL;
  struct timeval tv;

  /* form peri paths */
  auto itr = PeriPatchPathInfoMap_.find(chipset_peri_ver_);
  if (itr != PeriPatchPathInfoMap_.end()) {
    info = itr->second;
  }

  if (info && !info->GetChipName().empty()) {
    FormRegularPaths(info);
  } else {
    FormDefaultPaths();
  }

  ALOGD("%s: Ram patch file path %s", __func__, rampatch_file_path);
  ALOGD("%s: NVM file path %s", __func__, nvm_file_path);

  err = DownloadTlvFile();
  if (err < 0) {
    ALOGE("%s: Download TLV file failed!", __func__);
    return err;
  }

  ALOGI("%s: Download TLV file successfully ", __func__);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_GETBLDINFO_CMD_STUCK);
  if ((err = GetBuildInfoReq()) < 0) {
    ALOGE("%s: Fail to get FW SU Build info (0x%x)", __func__, err);
    dnld_fd_in_progress_ = -1;
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED);
    return err;
  }

  return err;
}

int PeriPatchDLManager::ConfigurePeriSoC(void)
{
  int err = -1;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE];
  char res_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  switch (peri_patch_status_) {
    case HCI_PERI_PATCHING_NOT_DONE : {
      ALOGI("%s: Starting Peripheral patching as it is unpatched", __func__);
      peri_patching_in_progress = true;
      if((err = PeriPatchDownload()) < 0) {
        ALOGE("%s: Peri Patch Download failed(0x%x)", __func__, err);
        return err;
      }
      bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_RESET_STUCK);
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_RESET_STUCK,
                    "HCI PERI RESET CMD & RSP");
      err = PeriReset();
      gettimeofday(&tv, NULL);
      bt_logger_->CheckAndAddToDelayList(&tv);

      if ( err < 0 ) {
        ALOGE("HCI PERI Reset Failed !!");
        bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_RESET_CMD_FAILED);
      }
      bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_ARBITRATION_NTF_STUCK);
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (peri_patch_status_ == HCI_PERI_PATCHING_COMPLETE)
        ALOGD("%s: Patch Ntf success , peripheral is patched", __func__);
      else {
        ALOGE("%s: Patch Ntf failed , peripheral is not patched", __func__);
        err = -1;
        return err;
      }
      bt_logger_->SetSecondaryCrashReason(PERI_SOC_REASON_DEFAULT);
      break;
    }

    case HCI_PERI_PATCHING_IN_PROGRESS :
    {
      ALOGI("%s: Peripheral patching is in progress by other SS", __func__);
      dst_buff[0] = '\0';
      res_buff[0] = '\0';
      bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_ARBITRATION_NTF_STUCK);
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (peri_patch_status_ == HCI_PERI_PATCHING_COMPLETE) {
        ALOGD("%s: Patch Ntf success , peripheral is patched", __func__);
        snprintf(res_buff, sizeof(res_buff), "Peri Patching Success NTF= %d", err);
      } else {
        ALOGE("%s: Patch Ntf failed , peripheral is not patched", __func__);
        snprintf(res_buff, sizeof(res_buff), "Peri Patching Failed NTF= %d", err);
      }
      BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
      BtState :: Get()->SetPeriArbitNtfTS(dst_buff);
      bt_logger_->SetSecondaryCrashReason(PERI_SOC_REASON_DEFAULT);
      break;
     }
     case HCI_PERI_PATCHING_COMPLETE :
       ALOGI("%s: Peripheral core is already patched by other Subsystem", __func__);
       err = 0;
       break;
     default:
     {
       ALOGE("%s: Wrong Peripheral core patching status recieved \n", __func__);
       break;
     }
  }
  
  /* Writing into File */
  FILE *periversionfile = 0;
  char peri_dst_buff[MAX_BUFF_SIZE];
   
  /* patch version writing */
  if (version_info_ins_peri.chipset_ver != chipset_peri_ver_ ||
      version_info_ins_peri.patchversion != patchversion) {
    ALOGD("%s: Writing version info. in %s", __func__, PERI_VERSION_FILEPATH);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                               "Writing peri version info. in file");
    BtState :: Get()->AddLogTag(peri_dst_buff, tv, (char *)"Writing peri version info. in file");
    BtState :: Get()->SetPrePeriVersioInfoTS(peri_dst_buff);
    if (NULL != (periversionfile = fopen(PERI_VERSION_FILEPATH, "wb"))) {
      version_info_ins_peri.chipset_ver = chipset_peri_ver_;
      version_info_ins_peri.patchversion = patchversion;
      fprintf(periversionfile, "Peripheral Controller Product ID    : 0x%08x\n", productid);
      fprintf(periversionfile, "Peripheral Controller Patch Version : 0x%04x\n", patchversion);
      fprintf(periversionfile, "Peripheral Controller Build Version : 0x%04x\n", buildversion);
      fprintf(periversionfile, "Peripheral Controller SOC Version   : 0x%08x\n", soc_id);
      //fclose(periversionfile);
    } else {
      ALOGE("Failed to dump Peri SOC version info. Errno:%d", errno);
    }
    ALOGD("%s: Done writing peri version info. in %s", __func__, PERI_VERSION_FILEPATH);
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(peri_dst_buff, tv, (char *)"Done writing version info. in file");
    BtState :: Get()->SetPostPeriVersioInfoTS(peri_dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }
  
  /* Writing Build info */
  if (strcmp(version_info_ins_peri.build_label, build_label) != 0) {
    ALOGD("%s: Writing SU build info. in %s", __func__, PERI_VERSION_FILEPATH);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_FILE_SYSTEM_CALL_STUCK,
                                                  "Writing Controller SU Build info.");
    BtState :: Get()->AddLogTag(peri_dst_buff, tv, (char *)"Writing Controller SU Build info.");
    BtState :: Get()->SetPreSUBuildInfoTS(peri_dst_buff);
    if (periversionfile) {
      snprintf(version_info_ins_peri.build_label, sizeof(version_info_ins_peri.build_label),
               "%s", build_label);
      fprintf(periversionfile, "Bluetooth Contoller SU Build info  : %s\n", build_label);
      fclose(periversionfile);
    } else {
      ALOGE("Failed to dump  FW SU build info. Errno:%d", errno);
    }
    ALOGD("%s: Done Writing SU build info. in %s", __func__, PERI_VERSION_FILEPATH);
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(peri_dst_buff, tv, (char *)"Done writing Controller SU Build info.");
    BtState :: Get()->SetPostPeriSUBuildInfoTS(peri_dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }

  return err;
}

int PeriPatchDLManager::WakeUpSoC(void)
{
  struct timeval timeout;
  struct timeval* timeout_ptr = NULL;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  uint8_t wakeup_byte = 0xfd;
  int i, ret, err = -1;
  fd_set read_fds;
  int max_fd;

  std::chrono::milliseconds timeout_ms_;
  FD_ZERO(&read_fds);
  FD_SET(fd_transport_, &read_fds);

  /* This flag will decide whether this WAKE ACK received
   * from controller as part of WAKE UP sequence during BT SoC
   * boot.
   */
  soc_wakeup_in_progress = true;
  timeout_ms_ = std::chrono::milliseconds(WAKE_IND_TIMEOUT);

  for (i = 0; i < WAKE_IND_RETRIES; i++) {
    ALOGI("%s: Writing %d WAKE_IND", __func__, i);
    ret = uart_transport_->UartWrite(&wakeup_byte, 1);
    if (ret != 1) {
      ALOGE("%s: Failed to send WAKE_IND: %d", __func__, err);
      break;
    }
    if (timeout_ms_ > std::chrono::milliseconds(0)) {
      timeout.tv_sec = timeout_ms_.count() / 1000;
      timeout.tv_usec = (timeout_ms_.count() % 1000) * 1000;
      timeout_ptr = &timeout;
    }
    max_fd = fd_transport_+1;

#ifdef DUMP_RINGBUF_LOG
    Logger::Get()->UpdateRxEventTag(RX_PRE_SELECT_CALL_BACK);
#endif
    int retval = select(max_fd, &read_fds, NULL, NULL, timeout_ptr);
#ifdef DUMP_RINGBUF_LOG
    Logger::Get()->UpdateRxEventTag(RX_POST_SELECT_CALL_BACK);
#endif
    // There was some error.
    if (retval < 0) continue;
    if (FD_ISSET(fd_transport_, &read_fds)) {
      if (ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
        ALOGE("%s: failed to read WAKE_ACK", __func__);
        break;
      } else {
      ALOGI("%s: SoC wokeup Initiating FW sequence", __func__);
      err = 0;
      break;
      }
    }
  }

  soc_wakeup_in_progress = false;
  /* On Ganges 1.0 SoC will not respond when BT is first client
   * accesing peri. override this error to start FW download.
   */
  if (i  == WAKE_IND_RETRIES)
    err = 0;
  return err;
}

int PeriPatchDLManager::ReleasePeriAccess(void)
{
  NotifySignal *notifysignal_  = NotifySignal::Get();
  SoCAccessState state;

  ALOGI("%s", __func__);
  state = notifysignal_->ReleaseSoCAccess();
  if (state == SOC_ACCESS_RELEASED)
   return 0;
  else
   return -1;
}

int PeriPatchDLManager::RequestPeriAccess(void)
{
  NotifySignal *notifysignal_  = NotifySignal::Get();
  SoCAccessState state;

  ALOGI("%s", __func__);
  state = notifysignal_->RequestSoCAccess();
  if (state == SOC_ACCESS_DENIED) {
    ALOGI("%s: SoC access denied. waiting for soc access", __func__);
    bool status;
    status = notifysignal_->WaitForSoCAccess();
    if (status) {
      ALOGI("%s soc access granted", __func__);
    } else {
      ALOGE("%s soc access not granted", __func__);
      return -1;
    }
  } else if (state == SOC_ACCESS_DISALLOWED) {
    ALOGE("%s soc access disallowed", __func__);
    return -1;
  } else if (state == SOC_ACCESS_GRANTED) {
      ALOGI("%s soc access granted", __func__);
  } else {
      ALOGE("%s: invalid state", __func__);
      return -1;
  }

  return 0;
}

int PeriPatchDLManager::PeriSocInit(void)
{
  int err = -1;
  PatchPathManager* info = NULL;
  struct timeval tv;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE];
  char res_buff[MAX_BUFF_SIZE];
  ALOGI(" %s ", __func__);

  dnld_fd_in_progress_ = fd_transport_;

  /* Wake assert */
  err = WakeUpSoC();
  if (err < 0) {
    ALOGE("%s: failed to wakeup soc", __func__);
    return -1;
  }

  /* if previous BT OFF is not properly closed UART baudrate is
   * set to 8Mbps. In this case BT will send deactivate command
   * to SoC before sending any other commands.
   */
  int baud_rate_set = uart_transport_->GetBaudRate();
  if (power_state_ == ALL_CLIENTS_ON && baud_rate_set == UART_BAUDRATE_8_M) {
    ALOGI("%s: BD is 8Mbps resetting BT SoC ", __func__);
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK,
                                            "HCI ACTIVATE SS CMD & RSP");
    err = ActivateSS(BT_SS, HCI_ACTION_POWER_OFF);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    if (err < 0) {
      ALOGE("De Activate BT Failed !!");
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_BT_ACTIVATE_CMD_FAILED);
      return err;
    }
  }

  /*send the get version*/
  if ((err = PeriPatchVerReq()) < 0) {
    ALOGE("%s: Fail to get chipset Version (0x%x)", __func__, err);
    dnld_fd_in_progress_ = -1;
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_GETVER_CMD_FAILED);
    return err;
  }

  /* Checking Baud rate */
  baud_rate_set = uart_transport_->GetBaudRate();
  if (baud_rate_set == UART_BAUDRATE_8_M) {
    ALOGI("%s: Baud rate is already set to 8 Mbps", __func__);
  } else {
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PERI_SETBAUDRATE_CMD_STUCK);
    /* Change baud rate 115.2 kbps to 8Mbps */
    err = SetPeriBaudRateReq();
    if (err < 0) {
      ALOGE("%s: Baud rate change failed!", __func__);
      dnld_fd_in_progress_ = -1;
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_PERI_SETBAUD_CMD_FAILED);
      return err;
    }
  }

  /* Send arbitration request */
  if((err = Arbitrate()) < 0) {
    ALOGE("%s: Arbitration failed(0x%x)", __func__, err);
    return err;
  }

  if ((err = ConfigurePeriSoC()) < 0) {
    ALOGE("%s: failed to download fw(0x%x)", __func__, err);
    return err;
  }

  /* Wake assert */
  err = WakeUpSoC();
  if (err < 0) {
    ALOGE("%s: failed to wakeup soc", __func__);
    return -1;
  }

    err = TmelSocInit();
    if (err < 0) {
      ALOGE("%s: failed to patch tmel", __func__);
      return -1;
    }

  if(peri_patch_status_ == HCI_PERI_PATCHING_COMPLETE) {
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_BT_ACTIVATE_CMD_STUCK,
                                            "HCI ACTIVATE SS CMD & RSP");
    err = ActivateSS(BT_SS, HCI_ACTION_POWER_ON);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    if ( err < 0 ) {
      ALOGE("Activate BT Failed !!");
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_BT_ACTIVATE_CMD_FAILED);
    }
    ALOGD("%s: Peripheral patching completed", __func__);
   }

  dnld_fd_in_progress_ = -1;
  return err;
}

int PeriPatchDLManager::SetPeriBaudRateReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;
  uint8_t bt_baud_rate = uart_transport_->GetMaxBaudrate();
  struct timeval tv;

  if (bt_baud_rate == USERIAL_BAUD_8M) {
    ALOGI("%s: Setting baud rate at 8 Mbps", __func__);
  }

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_SETBAUDRATE_CMD_STUCK,
                                            "SETBAUDRATE CMD & RSP");

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  cmd_hdr = (hci_command_hdr*)(cmd + 2);
  cmd[0]  = HCI_PERI_COMMAND_PKT;
  cmd[1]  = BT_HOST_ID;
  cmd_hdr->opcode = HCI_VS_GENERAL_OPCODE_PERI;
  cmd_hdr->plen = PERI_VSC_SET_BAUDRATE_REQ_LEN;
  cmd[5]  = HCI_PERI_SET_BAUDRATE;
  cmd[6]  = bt_baud_rate;

  /* Total length of the packet to be sent to the Controller */
  size = 7;
  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW OFF");

  /* Flow off during baudrate change */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-off error: 0x%x\n", __func__, err);
    goto error;
  }

  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);

  bt_logger_->ProcessTx(HCI_PACKET_TYPE_PERI_CMD, &cmd[1], size - 1);
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    err  = -1;
    goto error;
  }

  usleep(20 * 1000);
  tcdrain(fd_transport_);
  /* Change Local UART baudrate to high speed UART */
  uart_transport_->SetBaudRate(bt_baud_rate);
  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW ON");
  /* Flow on after changing local uart baudrate */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    goto error;
  }
  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Baud rate rsp failed!", __func__);
    goto error;
  }

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

void PeriPatchDLManager::SetPeriRampatchRegularPaths(std::string tlv_name)
{
  snprintf(rampatch_file_path, sizeof(rampatch_file_path), "%s%s%s",
           (tmel == false ? PERI_RAMPATCH_PATH : TMEL_FW_REGULAR_PATH),
	   alt_path_for_peri_fw, tlv_name.c_str());
}

void PeriPatchDLManager::SetPeriNVMRegularPaths(std::string nvm_name)
{
  snprintf(nvm_file_path, sizeof(nvm_file_path), "%s%s%s", PERI_NVM_PATH,
           alt_path_for_peri_fw, nvm_name.c_str());
}

void PeriPatchDLManager::FormRegularPaths(PatchPathManager * info)
{
  bool valid = false;
  int prop_size;

  if (!is_alt_path_for_peri_fw_to_be_used) {
    /* Property value should be of format "btfw_x" where x is alphanumeric
     * and x can have max size of 2 and min size of 1 char(s).
     */
    ALOGI("%s: Getting value of persist.vendor.bluetooth.alt_path_for_fw",
           __func__);

    bt_logger_->PropertyGet("persist.vendor.bluetooth.alt_path_for_fw", alt_path_for_peri_fw, "");
    is_alt_path_for_peri_fw_to_be_used = true;
    prop_size = strlen(alt_path_for_peri_fw);
    if (prop_size == 6 || prop_size == 7) {
      ALOGI("%s: persist.vendor.bluetooth.alt_path_for_fw is set to %s",
             __func__, alt_path_for_peri_fw);
      valid = std::regex_match(alt_path_for_peri_fw,
                     std::regex("(btfw_(([a-z1-9][a-z1-9])|[a-z1-9]))"));
      if (valid)
        strlcat(alt_path_for_peri_fw, "/", PROPERTY_VALUE_MAX);
    }
  }

  if (!valid) {
    if (prop_size == 0) {
      ALOGI("%s: persist.vendor.bluetooth.alt_path_for_fw not set!", __func__);
    } else {
      ALOGE("%s: persist.vendor.bluetooth.alt_path_for_fw "
            "property value = (%s) have invalid format", __func__, alt_path_for_peri_fw);
      ALOGE("%s: Falling back to default path", __func__);
      is_alt_path_for_peri_fw_to_be_used = false;
      memset(alt_path_for_peri_fw, '\0', sizeof(alt_path_for_peri_fw));
    }
  }

  SetPeriRampatchRegularPaths(info->GetTLVName());
  if (!tmel) {
    SetPeriNVMRegularPaths(info->GetNVMName());
  }

}

void PeriPatchDLManager::FormDefaultPaths()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PERI_FILE_SYSTEM_CALL_STUCK,
                                            "Setting default Rampatch and NVM path");
  strlcpy(rampatch_file_path, TEMP_RAMPATCH_TLV_0_0_0_PATH, sizeof(rampatch_file_path));
  strlcpy(nvm_file_path, TEMP_NVM_TLV_0_0_0_PATH, sizeof(nvm_file_path));

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
}

int PeriPatchDLManager::ReadHciPeriEvent(unsigned char* buf, int size)
{
  int retval;
  unsigned char protocol_byte;
  unsigned char hdr[PERI_EVT_HDR_SIZE];
  unsigned char packet_len;
  unsigned short tot_len;
  unsigned char host_id_byte;
  unsigned char* tmp_buf = NULL;

  ALOGV("%s:", __func__);

  do {

    retval = uart_transport_->Read(&protocol_byte, 1);
    if (retval < 0) {
      ALOGE("%s: read error", __func__);
      return -1;
    }

    if (protocol_byte == LOG_BT_EVT_PACKET_TYPE) {
 
      retval = uart_transport_->Read(hdr, BT_EVT_HDR_SIZE);
      if (retval < 0) {
        ALOGE("%s:error in reading hdr: %d", __func__, retval);
        return -1;
      }

      ALOGV("%s: read scucesssssfully HDR", __func__);
      packet_len = hdr[BT_EVT_HDR_LEN_OFFSET];
      ALOGV("%s: packet_len: %d\n", __func__, packet_len);
      buf[0] = protocol_byte;
      memcpy(buf + 1, hdr, BT_EVT_HDR_SIZE);

      retval = uart_transport_->Read(buf + BT_EVT_HDR_SIZE + 1, packet_len);
      if (retval < 0) {
        ALOGE("%s:error in reading buf: %d", __func__, retval);
        retval = -1;
        return retval;
      }

      tot_len = packet_len + BT_EVT_HDR_SIZE + 1;
      ALOGV("%s: read scucesssssfully payload: tot_len: %d", __func__, tot_len);
      break;
    } else if (protocol_byte == LOG_PERI_EVT_PACKET_TYPE) {

      retval = uart_transport_->Read(&host_id_byte, 1);
      if (retval < 0) {
        ALOGE("%s:error in reading host id byte: %d", __func__, retval);
        return -1;
      }

      if (host_id_byte != BT_HOST_ID) {
        ALOGE("%s:Received invalid host id byte: %d", __func__, host_id_byte);
        return -1;
      }

      retval = uart_transport_->Read(hdr, PERI_EVT_HDR_SIZE);
      if (retval < 0) {
        ALOGE("%s:error in reading peri hdr: %d", __func__, retval);
        return -1;
      }

      ALOGV("read scucesssssfully Peri-HDR");
      packet_len = hdr[PERI_EVT_HDR_LEN_OFFSET];
      ALOGV("Peri packet_len: %d\n", packet_len);
      buf[0] = protocol_byte;
      buf[1] = host_id_byte;
      memcpy(buf + HCI_PERI_CMD_IND_LEN, hdr, PERI_EVT_HDR_SIZE);

      retval = uart_transport_->Read(buf + PERI_EVT_HDR_SIZE + 2, packet_len);
      if (retval < 0) {
        ALOGE("%s:error in reading peri buf: %d", __func__, retval);
        retval = -1;
        return retval;
      }

      tot_len = packet_len + PERI_EVT_HDR_SIZE + 2;
      ALOGV("read successfully Peri payload: tot_len: %d", tot_len);
      break;
    } else if (protocol_byte == LOG_BT_ACL_PACKET_TYPE) {
      /* Check and parse ACL data if received during init phase to prevent init failures */
      retval = uart_transport_->Read(buf, BT_ACL_HDR_SIZE);
      if (retval < 0) {
        ALOGE("%s: read error", __func__);
        return -1;
      }

      PTR_TO_UINT16(tot_len, &buf[BT_ACL_HDR_LEN_OFFSET]);
      tmp_buf = new (std::nothrow)unsigned char[tot_len + BT_ACL_HDR_SIZE];
      if (tmp_buf == NULL) {
        ALOGE("%s: Heap memory allocation failed ", __func__);
        return -1;
      }
      memcpy(tmp_buf, buf, BT_ACL_HDR_SIZE);
      retval = uart_transport_->Read(tmp_buf + BT_ACL_HDR_SIZE, tot_len);
      if (retval < 0) {
        ALOGE("%s: read error", __func__);
        if (tmp_buf != NULL) {
          delete [] tmp_buf;
          tmp_buf = NULL;
        }
        return -1;
      }

      ALOGD("%s: Number of bytes of ACL data(header + payload) read = %d",
            __func__, tot_len + BT_ACL_HDR_SIZE);

      bt_logger_->ProcessRx(HCI_PACKET_TYPE_ACL_DATA, tmp_buf, tot_len + BT_ACL_HDR_SIZE);
      if (tmp_buf != NULL) {
        delete [] tmp_buf;
        tmp_buf = NULL;
      }
    } else if (protocol_byte == 0xFD) {
      ALOGI("%s: Got FD , responding with FC", __func__);
      uint8_t wake_byte = 0xFC;
      uart_transport_->UartWrite(&wake_byte, 1);
    } else if (protocol_byte == 0xFE) {
      ALOGI("%s: Sleep Byte Recieved ", __func__);
    } else if (protocol_byte == 0xFC) {
        if (soc_wakeup_in_progress) {
          tot_len = 1;
          buf[0] = protocol_byte;
          ALOGI("%s: WAKE_ACK Received as part of SoC Wakeup", __func__);
      break;
    } else {
          ALOGW("%s: unexpected WAKE_ACK Received", __func__);
    }
    } else {
      ALOGE("%s: Got an invalid proto byte: %d", __func__, protocol_byte);
    }
  } while (1);

  return tot_len;
}

int PeriPatchDLManager::ReadHciEvent(unsigned char* buf, int size)
{
  bool collecting_ram_dump = false;
  bool collecting_peri_ram_dump = false;
  int tot_len;
  bool read_again = false;
  ALOGE("%s", __func__);

  do {
    read_again = false;
    tot_len = ReadHciPeriEvent(buf, size);
    if (tot_len < 0) {
      ALOGE("%s: Error while reading the hci event", __func__);
      return -1;
    }

    std::stringstream ss;
    for (int i = 0; i < tot_len && i <= 30; ++i) {
      ss <<  std::uppercase << std::hex << (int)buf[i] << " ";
    }

    if (ss.str().length() > 0) {
      std::string params = "packet received:\n";
      params += ss.str();
      ALOGI("%s: %s", __func__, params.c_str());
    }

    if (buf[0] == HCI_PACKET_TYPE_EVENT) {
      bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len - 1);
    } else if (buf[0] == HCI_PACKET_TYPE_PERI_EVT)  {
      bt_logger_->ProcessRx(HCI_PACKET_TYPE_PERI_EVT, &buf[1], tot_len - 1);
    }

    if (tot_len == 1 && buf[0] == 0xFC)
      return tot_len;

    if(buf[0] == HCI_PACKET_TYPE_PERI_EVT && buf[2] == PERI_EVT_CMD_COMPLETE) {
      if(buf[4] == HCI_PERI_EVT_CLASS && buf[5] == HCI_PERI_EVT_MT_SS_ACT_CMPLT) {
        ALOGD("%s: Activate_complete_rsp async recvd \n", __func__);
        if (ReadVsAsyncEvent(buf, size, HCI_PERI_EVT_MT_SS_ACT_CMPLT) != HCI_CMD_SUCCESS)
          return -1;
       } else if(buf[4] == HCI_PERI_EVT_CLASS &&
            (buf[5] == LOG_HCI_PERI_CRASH_DUMP_MEMDUMP ||
            buf[5] == LOG_HCI_PERI_CRASH_DUMP_INFORMATION ||
            buf[5] == LOG_HCI_PERI_BUFF_DUMP)) {
          if (!collecting_peri_ram_dump) {
            ALOGE("%s: It is RAMDUMP OR HCI_BUFFER_DUMP, keep looping to get RAMDUMP events", __func__);
            collecting_peri_ram_dump = true;
          } else {
            collecting_peri_ram_dump = bt_logger_->IsCrashDumpStarted();
            // crash dump finished.
            if (collecting_peri_ram_dump == false) {
              ALOGE("%s: crash dump finished in between!", __func__);
              return -1;
            }
          }
       } else if(buf[4] == HCI_PERI_EVT_CLASS && buf[5] == HCI_PERI_EVT_MT_SS_PATCH_NTF) {
         ALOGD("%s: Patch Notification recvd \n", __func__);
         if (ReadVsAsyncEvent(buf, size, HCI_PERI_EVT_MT_SS_PATCH_NTF) != HCI_CMD_SUCCESS)
           return -1;
       } else if(buf[5] == PERI_MSG_TYPE_CMD_STATUS) {
         ALOGD("%s: Activate_status_rsp recvd \n", __func__);
         if (GetVsHciEvent(buf) != HCI_CMD_SUCCESS)
           return -1;
       } else if(buf[5] == PERI_MSG_TYPE_CMD_CMPL) {
         ALOGD("%s: Peri Expected CC", __func__);
         if (GetVsHciEvent(buf) != HCI_CMD_SUCCESS)
           return -1;
       }
    } else if ((buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) &&
               (buf[3] == LOG_BT_CONTROLLER_LOG) &&
               (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP || buf[4] == LOG_BT_MESSAGE_TYPE_HW_ERR)) {
      if (!collecting_ram_dump) {
        ALOGE("%s: It is RAMDUMP OR HCI_BUFFER_DUMP, keep looping to get RAMDUMP events", __func__);
        collecting_ram_dump = true;
      } else {
        collecting_ram_dump = bt_logger_->IsCrashDumpStarted();
        // crash dump finished.
        if (collecting_ram_dump == false) {
          ALOGE("%s: crash dump finished in between!", __func__);
          return -1;
        }
      }
    } else if ((buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) && (buf[PERI_EVENTCODE_OFFSET] == VSEVENT_CODE) &&
        buf[4] != LOG_BT_MESSAGE_TYPE_HW_ERR &&
        buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
      ALOGE("%s: Unexpected event recieved rather than CC", __func__);
      return 0;
    } else if (buf[0] == HCI_PACKET_TYPE_EVENT) {
      ALOGD("%s: BT Evt with type %02x", __func__, buf[1]);
      if (buf[1] != BT_HW_ERR_EVT)
        read_again = true; 
    } else {
      ALOGE("%s: Unexpected event : protocol byte: %d", __func__, buf[1]);
      tot_len = -1;
    }
  }while (collecting_ram_dump || collecting_peri_ram_dump || read_again);

  return tot_len;
}

int PeriPatchDLManager::ReadVsAsyncEvent(unsigned char* buf, int size, byte msgType)
{
  int err = 0;
  byte evtCode = 0;
  byte ssId = 0;
  byte status = 0;

  if (buf[PERI_EVENTCODE_OFFSET] == VSEVENT_CODE)
    ALOGD("%s: Received HCI-Vendor Specific event", __func__);
  else {
    ALOGE("%s: Failed to receive HCI-Vendor Specific event", __func__);
    err = -EIO;
    goto failed;
  }

  evtCode = buf[PERI_EVENTCODE_OFFSET];

  /* Check the status of the operation */
  switch (msgType) {
    case HCI_PERI_EVT_MT_SS_PATCH_NTF:
      status = buf[6];
      ssId = buf[7];
	  if(tmel) {
	      if (ssId != TME_SS) {
	        ALOGE("%s: TME-L Patching Failed. Wrong/invalid Subsystem Id recived, SS_ID:%d",
	                       __func__, ssId);
	        err = -1;
	      } else if(status == HCI_PERI_EDL_PATCH_SUCCESS) {
		  	if (ssId == TME_SS) {
	        	tme_patch_status_ = HCI_PERI_PATCHING_COMPLETE;
				ALOGI("%s: TME-L Patching Completed. Status:%d, PatchStatus;%d, SS-Id:%d",
						__func__, status, ssId);
		  	} else {
				peri_patch_status_ = HCI_PERI_PATCHING_COMPLETE; 
	        	ALOGI("%s: Peri Patching Completed. Status:%d, PatchStatus;%d, SS-Id:%d",
						__func__, status, peri_patch_status_, ssId);
		  	}
	      } else {
	        ALOGE("%s: TME-L Patching Failed. Status:%d, PatchStatus;%d, SS_ID:%d",
	                        __func__, status, tme_patch_status_, ssId);
	        err = -1;
	      }
	  } else {
      if (ssId != PERI_SS) {
        ALOGE("%s: Peri Patching Failed. Wrong/invalid Subsystem Id recived, SS_ID:%d",
                       __func__, ssId);
        err = -1;
      } else if(status == HCI_PERI_EDL_PATCH_SUCCESS) {
		  	if (ssId == PERI_SS) {
        peri_patch_status_ = HCI_PERI_PATCHING_COMPLETE;
        ALOGI("%s: Peri Patching Completed. Status:%d, PatchStatus;%d, SS-Id:%d",
                        __func__, status, peri_patch_status_, ssId);
      } else {
				tme_patch_status_ = HCI_PERI_PATCHING_COMPLETE;
	        	ALOGI("%s: TME-L Patching Completed. Status:%d, PatchStatus;%d, SS-Id:%d",
	        			__func__, status, tme_patch_status_, ssId);
			}
     	} else {
        ALOGE("%s: Peri Patching Failed. Status:%d, PatchStatus;%d, SS_ID:%d",
                        __func__, status, peri_patch_status_, ssId);
        err = -1;
      }
	  }
      break;
    case HCI_PERI_EVT_MT_SS_ACT_CMPLT:
      ssId = buf[6];
      peri_activate_action_ = (HciActivate)buf[7];
      ALOGD("%s: Peri Activate SS done. SsId:%d, Action:%d", __func__, ssId,
                         peri_activate_action_);
      break;
    }

failed:
  return err;
}

int PeriPatchDLManager::ProcessPeriEdlPatchEvt(unsigned char *rsp, unsigned char paramlen)
{
  int err = 0;
  struct timeval tv;
  char core[10];
#ifdef USER_DEBUG
  FILE *periversionfile = 0;
  char dst_buff[MAX_BUFF_SIZE];
#endif

  if (tmel)
    strlcpy(core, "TMEL ", sizeof(core));
  else
    strlcpy(core, "Peri ", sizeof(core));

  productid    = (rsp[12] | (rsp[13]<<8) | (rsp[14]<<16) | (rsp[15]<<24));
  patchversion = (rsp[16] | (rsp[17] << 8)) ;
  buildversion = (rsp[18] | (rsp[19] << 8)) ;

  ALOGI("\t%s Product ID\t\t: 0x%08x\n", core, productid);
  ALOGI("\t%s Patch Version\t\t: 0x%04x\n", core, patchversion);
  ALOGI("\t%s Build Version\t\t: 0x%04x\n", core, buildversion);

  if (paramlen - 10) {
    soc_id = (rsp[20] | (rsp[21]<<8) | (rsp[22]<<16) | (rsp[23]<<24));
    char soc_id_info[255];
    ALOGI("\t%s SOC Version\t\t: 0x%08x", core, soc_id);
    if (tmel) {
      snprintf(soc_id_info, sizeof(soc_id_info), "TMEL Current SOC Version : 0x%08x", soc_id);
      /* To Do TMEL SoC Id to state file*/
    } else {
      snprintf(soc_id_info, sizeof(soc_id_info), "Peripheral Current SOC Version : 0x%08x", soc_id);
      BtState::Get()->SetPeriSocId(soc_id_info);
    }
  }
  security_version = rsp[24];
  if (tmel) {
    chipset_tme_ver_ =  QCA_BT_VER(soc_id, productid, buildversion);
  } else {
    chipset_peri_ver_ = QCA_BT_VER(soc_id, productid, buildversion);
  }

  ALOGI("\t%s: Chipset Version (0x%16llx)", __func__,
        tmel == true ? chipset_tme_ver_: chipset_peri_ver_);
  ALOGI("%s Current Security Version\t\t: 0x%x\n", core, security_version);
  return err;
}

int PeriPatchDLManager::GetVsHciEvent(unsigned char *rsp)
{
  int err = 0;
  unsigned char paramlen = 0;
  unsigned int opcode = 0;
  unsigned char subOpcode = 0;
  unsigned int ocf = 0;
  unsigned char status = 0;
  unsigned char ssId = 0;

  if (rsp[5] == PERI_MSG_TYPE_CMD_STATUS) {
     opcode = rsp[9]<<8 | rsp[8];
     ocf = opcode & 0x03ff;
     status = rsp[6];
  } else if (rsp[5] == PERI_MSG_TYPE_CMD_CMPL) {
     opcode = rsp[8]<<8 | rsp[7];
     ocf = opcode & 0x03ff;
     status = rsp[9];
     subOpcode = rsp[10];
     ALOGV("%s: Subopcode:0x%x", __func__, subOpcode);
  }

  ALOGV("%s: Opcode: 0x%x", __func__, opcode);
  ALOGV("%s: ocf: 0x%x", __func__, ocf);
  ALOGV("%s: Status:0x%x", __func__, status);

  /* Check peri events */
  if(opcode == HCI_VS_DNLD_OPCODE_PERI) {
    switch (subOpcode) {
      case HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ:
      {
        ssId = rsp[ARBIT_RSP_SSID_OFFSET];
        if (tmel) {
	  if (ssId != TME_SS ) {
	    ALOGE("Invalid subsystem ID recieved! 0x%x", ssId);
	    err = -1;
	  } else {
             tme_patch_status_ = (HciPatchStatus)rsp[9];
	     ALOGD("%s: SS_ID: 0x%x", __func__, ssId);
	     if (tme_patch_status_ == HCI_TME_PATCHING_NOT_DONE) {
	       ALOGD("%s: TME-L patching is not done", __func__);
	        } else if (tme_patch_status_ == HCI_TME_PATCHING_COMPLETE) {
	       ALOGD("%s: TME-L patching is completed = 0x%x", __func__, tme_patch_status_);
	        } else if (tme_patch_status_ == HCI_TMEL_NOT_PATCHED_BY_OTHER_SS ||
                        tme_patch_status_ == HCI_TMEL_NOT_SUPPORTED) {
              /* The response HCI_TMEL_NOT_PATCHED_BY_OTHER_SS and HCI_TMEL_NOT_SUPPORTED,
                 UWB patched but did not patched the TME-L and TME-L is not supported, BT INIT
                 should continue by skipping the TME-L patching.
                 So for both the conditions, BT INIT should continue like the case where TME-L
                 successfully patched.
			  */
	          ALOGD("%s: TME-L is not supported or didn't patched by other SS = 0x%x", __func__,
	                tme_patch_status_);
	     } else {
	       ALOGD("%s: Unknown TMEL-patch status = 0x%x", __func__, tme_patch_status_);
	     }
	     ALOGD("%s: Arbitration completed", __func__);
	  }
        } else {
          if (ssId != PERI_SS ) {
            ALOGE("Invalid subsystem ID recieved! 0x%x", ssId);
            err = -1;
          } else {
            peri_patch_status_ = (HciPatchStatus)rsp[9];
            ALOGD("%s: SS_ID: 0x%x", __func__, ssId);
            if (peri_patch_status_ == HCI_PERI_PATCHING_NOT_DONE) {
              ALOGD("%s: Peripheral patching is not done", __func__);
            } else if (peri_patch_status_ == HCI_PERI_PATCHING_IN_PROGRESS) {
              ALOGD("%s: Peripheral patching is in progress = 0x%x", __func__, peri_patch_status_);
            } else if (peri_patch_status_ == HCI_PERI_PATCHING_COMPLETE) {
              ALOGD("%s: Peripheral patching is completed = 0x%x", __func__, peri_patch_status_);
            } else {
              ALOGD("%s: Unknown peri-patch status = 0x%x", __func__, peri_patch_status_);
            }
            ALOGD("%s: Arbitration completed", __func__);
          }
        }
      }
      break;
      case HCI_PERI_EDL_PATCH_GETVER:
      {
        ssId = rsp[11];
        paramlen = rsp[3];
        ProcessPeriEdlPatchEvt(rsp, paramlen);
      }
      break;
      case HCI_PERI_TLV_DOWNLOAD_REQ:
      {
        ssId = rsp[11];
        paramlen = rsp[3];
        HandleEdlCmdResEvt(subOpcode, paramlen, rsp);

        if(rsp[9] != HCI_CMD_SUCCESS) {
          ALOGE("Download TLV request failed with status: 0x%x" , rsp[HCI_PERI_TLV_STATUS]);
          err = -1;
          return err;
        } else {
          ALOGV("%s: TLV Download Complete", __func__);
        }
      }
      break;
      case HCI_PERI_TLV_PATCH_CONFIG:
      {
        err = rsp[CMD_STATUS_OFFSET_UNIFIED];
        if (err == HCI_CMD_SUCCESS) {
          ALOGD("%s: patch config cmd completed successfully", __func__);
        } else {
          ALOGE("%s: patch config cmd failed with HCI err code 0x%x", __func__, err);
        }
      }
      break;
      case HCI_PERI_EDL_GET_BOARD_ID:
      {
        HandleEdlCmdResEvt(subOpcode, paramlen, rsp);
        ALOGV("%s: HCI_PERI_EDL_GET_BOARD_ID Response rced", __func__);
      }
      break;
      case HCI_PERI_EDL_GET_BUILD_INFO:
      {
        ALOGV("%s: HCI_PERI_EDL_GET_BUILD_INFO Response rced", __func__);
        HandleEdlCmdResEvt(subOpcode, paramlen, rsp);
      }
      break;
      default:
        ALOGE("%s: Unknown Peri General subOpcode received!!!", __func__);
      break;
    }
    return 0;
  } else if (opcode == HCI_VS_GENERAL_OPCODE_PERI) {
    switch(subOpcode) {
      case HCI_PERI_SET_BAUDRATE:
        ALOGD("%s: HCI_PERI_SET_BAUDRATE Response recived", __func__);
        if (rsp[9] != BAUDRATE_CHANGE_SUCCESS) {
          ALOGE("%s: Set Baudrate request failed with status: 0x%x", __func__, rsp[9]);
          err = -1;
        }
        break;
      case HCI_PERI_ACTIVATE_SS:
	peri_activate_action_ = (HciActivate)status;
        ALOGD("%s: HCI_PERI_ACTIVATE_SS Response received", __func__);
        break;
      case HCI_PERI_RESET:
        ALOGD("%s: HCI_PERI_RESET Response rced", __func__);
        if (status != HCI_CMD_SUCCESS) {
          ALOGE("%s: Peri Reset Command Failed", __func__);
          err = -EIO;
          goto failed;
        }
        break;
      default:
        ALOGE("%s: Unknown Peri General subOpcode !!!", __func__);
      return 0;
    }
    return 0;
  }
 failed:
  return err;
}

void PeriPatchDLManager::HandleEdlCmdResEvt(unsigned char subOpcode, unsigned char paramlen, unsigned char* rsp)
{
  int err = 0;
  unsigned int soc_id = 0;
  unsigned int productid = 0;
  unsigned short patchversion = 0;
  unsigned short buildversion = 0;
  unsigned int bld_len_offset = 0x0C;
  int build_lbl_len;
  struct timeval tv;
#ifdef USER_DEBUG
  FILE *periversionfile = 0;
  char dst_buff[MAX_BUFF_SIZE];
#endif
  bool ret  = false;
  int build_label_prefix;

  switch (subOpcode) {
    case HCI_PERI_TLV_DOWNLOAD_REQ:
      err = rsp[PERI_CMD_STATUS_OFFSET];
      DownloadAndCmdExeStatus(err);
      break;
    case HCI_PERI_EDL_GET_BUILD_INFO:
      build_lbl_len = rsp[bld_len_offset];
      snprintf(build_label, sizeof(build_label), "BT SoC FW SU Build info: ");
      build_label_prefix = strlen(build_label);
      memcpy(build_label + build_label_prefix, &rsp[bld_len_offset + 1], build_lbl_len);
      *(build_label + build_label_prefix + build_lbl_len) = '\0';
      gettimeofday(&tv, NULL);
      BtState::Get()->SetPeriFwSuBuildInfo(build_label, tv);

      ALOGI("%s: %s, %d", __func__, build_label, build_lbl_len);

/*
#ifdef USER_DEBUG
    
#endif
*/
      break;
    case HCI_PERI_EDL_GET_BOARD_ID:
    {
      uint8_t msbBoardId = 0;
      uint8_t lsbBoardId = 0;
      uint8_t boardIdLen = 0;
      board_id_[0] = '\0';
      paramlen = (uint8_t)rsp[3];
      if (paramlen < 8) {
        ALOGE("%s: Invalid Param Len in BoardId rsp:%d!!", __func__, paramlen);
        break;
      }
      boardIdLen = (uint8_t)rsp[0x0B];
      msbBoardId = (uint8_t)rsp[0x0C];
      lsbBoardId = (uint8_t)rsp[0x0D];

      if (boardIdLen != 2) {
        ALOGE("%s: Invalid Board Id Len %d!!", __func__, boardIdLen);
        break;
      }
      ALOGI("%s: Board Id %x %x!!", __func__, msbBoardId, lsbBoardId);
      if (msbBoardId == 0x00) {
        board_id_[0] = Convert2AsciiPeri((lsbBoardId & MSB_NIBBLE_MASK) >> 4);
        board_id_[1] = Convert2AsciiPeri(lsbBoardId & LSB_NIBBLE_MASK);
        board_id_[2] = '\0';
      } else {
        board_id_[0] = Convert2AsciiPeri((msbBoardId & MSB_NIBBLE_MASK) >> 4);
        board_id_[1] = Convert2AsciiPeri(msbBoardId & LSB_NIBBLE_MASK);
        board_id_[2] = Convert2AsciiPeri((lsbBoardId & MSB_NIBBLE_MASK) >> 4);
        board_id_[3] = Convert2AsciiPeri(lsbBoardId & LSB_NIBBLE_MASK);
        board_id_[4] = '\0';
      }
    }
    break;
  }
}

void PeriPatchDLManager::DownloadAndCmdExeStatus(int err) {
  switch (err) {
    case HCI_CMD_SUCCESS:
      ALOGV("%s: Download Packet successfully!", __func__);
      break;
    case LENGTH_ERROR:
      ALOGE("%s: Invalid patch length argument passed for EDL PATCH "
                    "SET REQ cmd, (0x%x)", __func__, err);
      break;
    case VERSION_ERROR:
      ALOGE("%s: Invalid patch version argument passed for EDL PATCH "
                    "SET REQ cmd, (0x%x)", __func__, err);
      break;
    case SIGNATURE_ERROR:
      ALOGE("%s: SIGNATURE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NO_MEMORY_ERROR:
      ALOGE("%s: NO_MEMORY_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NVM_TAG_ERROR:
      ALOGE("%s: NVM_TAG_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NVM_LENGTH_ERROR:
      ALOGE("%s: NVM_LENGTH_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case BT_MODE_ERRO:
      ALOGE("%s: BT_MODE_ERRO (0x%x) detected!!!", __func__, err);
      break;
    case UNKOWN_ERROR:
      ALOGE("%s: UNKOWN_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case TYPE_ERROR:
      ALOGE("%s: TYPE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case IMAGE_TYPE_ERROR:
      ALOGE("%s: IMAGE_TYPE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case CRC_ERROR:
      ALOGE("%s: CRC_ERROR (0x%x) detected", __func__, err);
      break;
    case ID_ERROR:
      ALOGE("%s: ID_ERROR (0x%x) detected", __func__, err);
      break;
    case WRONG_VER_ERROR:
      ALOGE("%s: WRONG_VER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case SMALL_VER_ERROR:
      ALOGE("%s: SMALL_VER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case REGION_ERROR:
      ALOGE("%s: REGION_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NO_PATCH_ERROR:
      ALOGE("%s: NO_PATCH_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case WRONG_ANTI_ROLLBACK_ERROR:
      ALOGE("%s: WRONG_ANTI_ROLLBACK_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case OTP_WR_ERROR:
      ALOGE("%s: OTP_WR_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_PATCH_PER_OTP_ERROR:
      ALOGE("%s: INVALID_PATCH_PER_OTP_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_PATCH_HEADER_ERROR:
      ALOGE("%s: INVALID_PATCH_HEADER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_RSA_KEY_ERROR:
      ALOGE("%s: INVALID_RSA_KEY_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_RSA_SIGNATURE_ERROR:
      ALOGE("%s: INVALID_RSA_SIGNATURE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_SERIAL_NUMBER_ERROR:
      ALOGE("%s: INVALID_SERIAL_NUMBER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    default:
      ALOGE("%s: Undefined error (0x%x)", __func__, err);
      break;
  }
}

int PeriPatchDLManager::HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;
  hci_command_hdr* cmd_hdr;

  bt_logger_->ProcessTx(HCI_PACKET_TYPE_PERI_CMD, &cmd[1], size - 1);

  /* Send the HCI command packet to UART for transmission */
  ret = uart_transport_->UartWrite(cmd, size);
  if (ret != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
    goto failed;
  }
  /* check cmd size , ocf and subopcode for peri */
  cmd_hdr = (hci_command_hdr*)(cmd);

  if (wait_vsc_evt_) {
    if (ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      ret = -ETIMEDOUT;
      ALOGE("%s: Failed to get ReadHciEvent Event from SOC", __func__);
      goto failed;
    }
    ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
  }

 failed:
  return ret;
}

void PeriPatchDLManager::FramePeriHciPkt(
  unsigned char *cmd,
  unsigned short opcode,
  byte subOpcode,
  unsigned int p_base_addr,
  int segtNo,
  uint8_t size
)
{
  int offset = 0, add_size =0;
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  cmd_hdr = (hci_command_hdr*)(cmd+2);

  cmd[0] = HCI_PERI_COMMAND_PKT;
  cmd[1] = BT_HOST_ID;
  cmd_hdr->opcode = opcode;
  cmd_hdr->plen = size;
  cmd[5] = subOpcode;
  if (!tmel) {
    cmd[6] = PERI_SS;
  } else {
    cmd[6] = TME_SS;
  }

  switch(cmd_hdr->opcode) {
    case HCI_VS_DNLD_OPCODE_PERI:
      switch (subOpcode) {
        case HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ:
          cmd[7] = 0; // Timer Disabled
          if (!tmel)
            ALOGD("%s: Sending HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ", __func__);
		      else
		  	    ALOGD("%s: Sending HCI_TMEL_PATCH_SS_ARBITRATION_REQ", __func__);
        break;
        case HCI_PERI_EDL_PATCH_GETVER:
          if (!tmel)
            ALOGD("%s: Sending HCI_PERI_EDL_PATCH_GETVER", __func__);
          else
            ALOGD("%s: Sending HCI_TMEL_PATCH_GETVER", __func__);
        break;
        case HCI_PERI_TLV_DOWNLOAD_REQ:
          if (!tmel)
            ALOGV("%s: Sending HCI_PERI_TLV_DOWNLOAD_REQ", __func__);
          else
            ALOGV("%s: Sending HCI_TMEL_TLV_DOWNLOAD_REQ", __func__);
          cmd[7] = size;
          cmd[4] = size + 3;
          offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
          memcpy(&cmd[8], (pdata_buffer_ + offset), size);
        break;
        case HCI_PERI_EDL_GET_BUILD_INFO:
          ALOGD("%s: Sending HCI_PERI_EDL_GET_BUILD_INFO", __func__);
        break;
        case HCI_PERI_EDL_GET_BOARD_ID:
          ALOGD("%s: Sending HCI_PERI_EDL_GET_BOARD_ID", __func__);
        break;
        case HCI_PERI_TLV_PATCH_CONFIG:
         /* patch configuration */
         if (strcmp(elf_config_, "no_value") != 0) {
           unsigned long ul;
           /* converting hex in string to real hex number */
           ul = strtoul (elf_config_, NULL, 16);
           /* patch config size is 4 */
           memcpy(&cmd[6], &ul, sizeof(uint32_t));
           /* Checking BIT 0 and BIT 1:
            * 1) BIT 0: Whether SoC needs to respond for every
            *     download command (0) or only at the end (1).
            * 2) BIT 1: Whether to perform Parallel (1) or Sequential
            *     validation (0)
            */
          if ((cmd[6] & 1) == 0) {
            tlv_dwn_cfg_ = SKIP_EVT_NONE;
          } else {
            tlv_dwn_cfg_ = SKIP_EVT_VSE_CC;
          }
          if ((cmd[6] & 2) == 0) {
            ALOGI("%s: Sequential validation enabled by SoC", __func__);
          } else {
            ALOGI("%s: Parallel validation enabled by SoC", __func__);
          }

          ALOGD("%s: Sending HCI_PERI_TLV_PATCH_CONFIG", __func__);
        }
        break;
        default:
          ALOGE("%s: Unknown Peri EDL subOpcode !!!", __func__);
        break;
       };
    break;
    case HCI_VS_GENERAL_OPCODE_PERI:
      switch(subOpcode) {
        case HCI_PERI_ACTIVATE_SS:
          ALOGD("%s: Sending HCI_PERI_ACTIVATE_SS", __func__);
          break;
        case HCI_PERI_RESET:
          ALOGD("%s: Sending HCI_PERI_RESET", __func__);
          break;
        default:
          ALOGE("%s: Unknown Peri General subOpcode !!!", __func__);
          break;
      }
      break;
     default:
      ALOGE("%s: Unknown Peri Opcode !!!", __func__);
     break;
  }
}

void PeriPatchDLManager::LoadTMELPatchMaptable() {
  TMEPatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GANGES_PERI_VER_1_0,
  								new PatchPathManager("GAN_PERI1_0", "tmel_peach_10.elf", "")));
}

void PeriPatchDLManager::LoadPatchMaptable() {
  PeriPatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GANGES_PERI_VER_1_0,
                        new PatchPathManager("GAN_PERI1_0", "gngperifw10.tlv", "gngperinv10.bin")));
  PeriPatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GANGES_PERI_VER_2_0,
                        new PatchPathManager("GAN_PERI2_0", "gngperifw20.tlv", "gngperinv20.bin")));
}

void PeriPatchDLManager::ReadELFConfig() {
  if (!elf_config_read_) {
    const char* default_value = "no_value";
    //Enable sequential download by default for moselle.
    bt_logger_->PropertyGet("persist.vendor.peri.patch_config", elf_config_, default_value);
    ALOGI("%s: ELF patch config: %s", __func__, elf_config_);
    elf_config_read_ = true;
  }
}

unsigned char PeriPatchDLManager::Convert2AsciiPeri(unsigned char temp)
{
  unsigned char n = temp;
  if (n <= 9)
    n = n + 0x30;
  else
    n = n + 0x57;
  return n;
}

int PeriPatchDLManager::TMEArbitrate()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ALOGD("%s: Sending Arbitration CMD to TME-L", __func__);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_TME_ARBITRATION_CMD_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_TME_ARBITRATION_CMD_STUCK, "SENDING TME-L ARBITRATION CMD");
  
  /* Frame the HCI CMD to be sent to the controller */
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_PATCH_SS_ARBITRATION_REQ, 0, -1, EDL_PATCH_CMD_ARBIT_LEN);

  /* Total length of the packet to be send to the controller */
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_ARBIT_LEN);

  /* Send HCI Command packet to Controller */
  if ((err = HciSendVsCmd((unsigned char*)cmd, rsp, size)) != size) {
	ALOGE("%s: Failed to attach the patch payload to the controller!", __func__);
	err = -1;
	goto error;
  }

  gettimeofday(&tv, NULL);
  bt_logger_->SetSecondaryCrashReason(TME_SOC_REASON_DEFAULT);

error:
   gettimeofday(&tv, NULL);
   bt_logger_->CheckAndAddToDelayList(&tv);
   return err;
}

int PeriPatchDLManager::TMEPatchVerReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  gettimeofday(&tv, NULL);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_TME_GETVER_SEND_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_TME_GETVER_SEND_STUCK,
											"SENDING TMEL-GET VERSION CMD");

  /* Frame the HCI CMD to be send to the controller */
  FramePeriHciPkt(cmd, HCI_VS_DNLD_OPCODE_PERI, HCI_PERI_EDL_PATCH_GETVER,
					0, -1, PERI_EDL_PATCH_CMD_LEN);

  /* Total length of the packet to send to the controller */
  size = (HCI_PERI_CMD_IND_LEN + HCI_COMMAND_HDR_SIZE + PERI_EDL_PATCH_CMD_LEN);
  /* Send HCI Command packet to controller */
  err = HciSendVsCmd((unsigned char *)cmd, rsp, size);

  if (err != size) {
	ALOGE("%s: Failed to attach the patch payload to the controller!", __func__);
	err = -1;
	goto error;
  } else if (rsp[CMD_RSP_STATUS_OFFSET] != HCI_PERI_VER_SUCCESS || rsp[CMD_RSP_SS_ID_OFFSET] != TME_SS) {
	ALOGE("%s: Failed to get valid response : 0x%x ", __func__, err);
	err = -1;
	goto error;
  } else {
	ALOGI("%s: rsp[CMD_RSP_OFFSET] = %x, rsp[RSP_TYPE_OFFSET] = %x", __func__, rsp[CMD_RSP_OFFSET], rsp[RSP_TYPE_OFFSET]);
  }

  error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

int PeriPatchDLManager::TmelSocInit(void)
{
  int err = -1;

  tmel = true;
  ALOGE("%s", __func__);

  /* Send arbitration request */
  if((err = TMEArbitrate()) < 0) {
    ALOGE("%s : Arbitration failed(0x%x)", __func__, err);
    return err;
  }
  else if (tme_patch_status_ == HCI_TMEL_NOT_SUPPORTED ||
            tme_patch_status_ == HCI_TMEL_NOT_PATCHED_BY_OTHER_SS) {
   /* There are two cases as below, where BT should not patch the TME-L
      based on the arbitration response from TME-L.

      1) If arbitration response shows that TME-L is not supported on target SoC
         then we must return without TME-L patching. (0x06)
      2) If before BT turn on, UWB INIT is done but didn't patched the TME-L, then
         due to memory constraints, BT also must not patch the TME-L. (0x05)

      Above both the cases, BT INIT should continue without fail.
   */
   ALOGE("%s: TME-L is not supported or didn't patched by other SS", __func__);
   err = 0;
   tmel = false;
   return err;
  }

    /* send the get version */
    if((err = TMEPatchVerReq()) < 0) {
      ALOGE("%s : Fail to get the chipset version (0x%x)", __func__, err);
      dnld_fd_in_progress_ = -1;
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_TME_GETVER_CMD_FAILED);
      return err;
    }

  /* Configure the TME-L SoC */
  if((err = ConfigureTMESoC()) < 0) {
    ALOGE("%s : failed to download fw(0x%X)", __func__, err);
    return err;
  }

  tmel = false;
  error:
  return err;
}

int PeriPatchDLManager::TMEPatchDownload(void)
{
  int err = 0;
  PatchPathManager *info = NULL;
  struct timeval tv;

  /* form TME-L paths */
  auto itr = TMEPatchPathInfoMap_.find(chipset_tme_ver_);
  if (itr != TMEPatchPathInfoMap_.end())
	info = itr->second;

  if (info && !info->GetChipName().empty())
	FormRegularPaths(info);
  else
	FormDefaultPaths();

  ALOGD("%s: Ram patch file path %s", __func__, rampatch_file_path);

  err = DownloadTlvFile();
  if (err < 0) {
	ALOGE("%s: Download TLV file failed.", __func__);
	return err;
  }

  ALOGE("%s: Download TLV file successfully.", __func__);

  return err;
}

int PeriPatchDLManager::ConfigureTMESoC(void)
{
  int err = -1;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char res_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  switch (tme_patch_status_) {
    case HCI_TME_PATCHING_NOT_DONE : {
      ALOGI("%s : Starting TME-L patching as it is unpatched", __func__); 
      tme_patching_in_progress = true;
      if ((err = TMEPatchDownload()) < 0) {
        ALOGE("%s: TME-L Patch Donwload Failed(0x%X)", __func__, err);
        return err;
      }

      /* Read patch Notification */
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (tme_patch_status_ == HCI_TME_PATCHING_COMPLETE)
        ALOGD("%s: Patch Ntf success, TME-L is patched", __func__);
      else {
        ALOGE("%s: Patch Ntf failed, TME-L is not patched", __func__);
        err = -1;
        return err;
      }
      bt_logger_->SetSecondaryCrashReason(TME_SOC_REASON_DEFAULT);
    break;
    } case HCI_TME_PATCHING_IN_PROGRESS : {
      ALOGI("%s: TME-L patching is in progress by other SS.", __func__);
      bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_TME_ARBITRATION_NTF_STUCK);
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (tme_patch_status_ == HCI_TME_PATCHING_COMPLETE) {
        ALOGD("%s: Patch Ntf success, TME-L is patched", __func__);
        snprintf(res_buff, sizeof(res_buff), "TME-L Patching Success NTF= %d", err);
      } else {
        ALOGE("%s: Patch Ntf failed, TME-L is not patched", __func__);
        snprintf(res_buff, sizeof(res_buff), "TME-L Patching Failed NTF= %d", err);
      }
      bt_logger_->SetSecondaryCrashReason(TME_SOC_REASON_DEFAULT);
    break;
    } case HCI_TME_PATCHING_COMPLETE: {
      ALOGE("%s: TME-L is already patched by other SS", __func__);
      err = 0;
    break;
    }
	default:{
      ALOGE("%s: Wrong TME-L patching status received.\n", __func__);
      break;
    }
  }

	/* Writing into File */
	FILE *tmeversionfile = 0;
	char tme_dst_buff[MAX_BUFF_SIZE];

#ifdef USER_DEBUG
	/* patch version writing */
	if (version_info_ins_tme.chipset_ver != chipset_tme_ver_ ||
		  version_info_ins_tme.patchversion != patchversion) {
		ALOGD("%s: Writing Version Info. in %s", __func__, TME_VERSION_FILEPATH);
		gettimeofday(&tv, NULL);
		bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_TME_FILE_SYSTEM_CALL_STUCK,
												"Writing TME-L version info. in file");
		if (NULL != (tmeversionfile = fopen(TME_VERSION_FILEPATH, "wb"))) {
			version_info_ins_tme.chipset_ver = chipset_tme_ver_;
			version_info_ins_tme.patchversion = patchversion;
			fprintf(tmeversionfile, "TME-L Product Id	: 0x%08x\n", productid);
			fprintf(tmeversionfile, "TME-L Patch Version: 0x%04x\n", patchversion);
			fprintf(tmeversionfile, "TME-L Build Version: 0x%04x\n", buildversion);
			fprintf(tmeversionfile, "TME-L SoC Version	: 0x%08x\n", soc_id);
			//fclose(tmeversionfile);
		} else {
			ALOGE("Failed to dump the TME-L SoC Version Info. Errno:%d", errno);
		}
		ALOGI("%s: Done writing TME-L Version Info. in %s", __func__, TME_VERSION_FILEPATH);
		gettimeofday(&tv, NULL);
		bt_logger_->CheckAndAddToDelayList(&tv);
	}
	/* build version writing */
	if (strcmp(version_info_ins_tme.build_label, build_label) != 0) {
		ALOGD("%s: Writing SU build info. in %s", __func__, TME_VERSION_FILEPATH);
		gettimeofday(&tv, NULL);
		bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_TME_FILE_SYSTEM_CALL_STUCK,
													"Writing Controller SU Build info.");
		if (tmeversionfile) {
			snprintf(version_info_ins_tme.build_label, sizeof(version_info_ins_tme.build_label),
						"%s", build_label);
			fprintf(tmeversionfile, "Bluetooth Controller SU Build info : %s\n", build_label);
			fclose(tmeversionfile);
		} else {
			ALOGE("Failed to dump FW SU Build info. Errno: %d", errno);
		}
		ALOGE("%s: Done writing SU build info. in %s", __func__, TME_VERSION_FILEPATH);
		gettimeofday(&tv, NULL);
		bt_logger_->CheckAndAddToDelayList(&tv);
	}
#endif
	return err;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
