/*
 * Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright 2012 The Android Open Source Project
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <unistd.h>
#include <utils/Log.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <fcntl.h>

#include "state_info.h"
#include "power_manager.h"
#include "data_handler.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-power_manager"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-power_manager"
#endif

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {


#define RFKILL_STATE_PATH  "/sys/class/rfkill/rfkill%d/state"
#define RFKILL_TYPE_PATH  "/sys/class/rfkill/rfkill%d/type"
#define EXTLDO_PATH       "/sys/class/rfkill/rfkill%d/device/extldo"

int power_state_;
typedef enum {
  BT_POWER_OFF = 0,
  BT_POWER_ON = 1,
  BT_POWER_RETENTION = 2,
} BtPowerMode;

const char * PowerManager :: convertStatusToString(enum power_states state) {
  switch (state) {
    case IDLE:
      return "IDLE";
    case BT_ON:
      return "BT_ON";
    case UWB_ON:
      return "UWB_ON";
    case ALL_CLIENTS_ON:
      return "ALL_CLIENTS_ON";
    case POWER_STATE_DISALLOWED:
      return "POWER_STATE_DISALLOWED";
    default:
      return "UNHANDLED STATE";
    }
}

PowerManager :: PowerManager()
{
  rfkill_id_ = -1;
  bt_soc_type_ = BT_SOC_RESERVED;
  ext_ldo_ = false;
  pm_state_ = POWER_MANAGER_OFF;
  memset(ts_sw_ctrl, 0, MAX_TS_SIZE);
  memset(ts_init, 0, MAX_TS_SIZE);
  bt_enable_pin_pulled_low = false;
}

void PowerManager :: Init(BluetoothSocType soc_type)
{
  if (pm_state_ != POWER_MANAGER_OFF)
    return;
  bt_soc_type_ = soc_type;
}

int PowerManager :: SetPower(bool enable, bool retentionMode)
{
  int ret = -1;

  ALOGD("%s: enable: %x", __func__, enable);

  if (bt_soc_type_ == BT_SOC_RESERVED) {
    ALOGE("Power Manager not initialized, Returning!");
    return false;
  }

  pm_state_ = enable ? POWER_MANAGER_TURNING_ON : POWER_MANAGER_TURNING_OFF;

  switch (bt_soc_type_) {
    int rfkill_fd;
    /*power up for smd based chip*/
    case BT_SOC_SMD:
      ret = PowerUpSmd();
      break;

      /*power up for uart based chip*/
    case BT_SOC_ROME:
    case BT_SOC_AR3K:
    case BT_SOC_GENOA:
#ifdef TARGET_BOARD_AUTO
    case BT_SOC_HASTINGS:
#endif
      rfkill_fd = GetRfkillFd();
      if (rfkill_fd < 0)
        return false;

      ret = ControlRfkill(rfkill_fd, enable);
      close(rfkill_fd);
      break;

#ifndef TARGET_BOARD_AUTO
    case BT_SOC_HASTINGS:
#endif
    case BT_SOC_CHEROKEE:
    case BT_SOC_MOSELLE:
    case BT_SOC_EVROS:
    case BT_SOC_HAMILTON:
    case BT_SOC_GANGES:
      ret = PowerUpChip(enable, retentionMode);
      break;

    default:
      ALOGE("%s: unknown soc type %d", __func__, bt_soc_type_);
      break;
  }

  pm_state_ = (enable == false)? POWER_MANAGER_OFF : POWER_MANAGER_ON;

  return ret;
}

void PowerManager :: Cleanup(void)
{
  if (pm_state_ != POWER_MANAGER_OFF)
    return;
  rfkill_id_ = -1;
  ext_ldo_ = false;
  DataHandler::Get()->ClosePowerDriverFd();
}

void PowerManager :: CloseFd(void)
{
  DataHandler::Get()->ClosePowerDriverFd();
}

PowerManagerState PowerManager :: GetState()
{
  return pm_state_;
}

bool PowerManager :: PowerUpSmd()
{
  ALOGE("smd initialization");
  return true;
}

int PowerManager :: GetRfkillFd()
{
  int rfkill_fd;

  rfkill_fd = InitializeRfkill();
  if (rfkill_fd < 0) {
    ALOGE("rfkill initialization failed, returning!");
    Cleanup();
    return -1;
  }

  if (!SetExtLdoStatus()) {
    ALOGE("%s: set ext ldo status fail", __func__);
    Cleanup(rfkill_fd);
    return -1;
  }

  ALOGD("%s: rfkill_fd: %d", __func__, rfkill_fd);

  return rfkill_fd;
}

int PowerManager :: PowerUpChip(bool enable, bool retentionMode)
{
  struct timespec ts;
  int rc;

  ALOGE("%s: enable(%d) retentionMode(%d)\n", __func__, enable, retentionMode);

  BtPowerMode mode = BT_POWER_OFF;
  if ((enable == true) && (retentionMode == true))
    mode = BT_POWER_RETENTION;
  else if (enable == true) {
    mode = BT_POWER_ON;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1) {
      snprintf(ts_init, MAX_TS_SIZE, "%lu:%lu",
          ts.tv_sec, ts.tv_nsec);
    }
  }

  rc = Ioctl(mode);
  ALOGI("%s: return val %s %d", __func__, convertStatusToString((enum power_states)rc), rc);
  if (mode == BT_POWER_ON) {
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1) {
      snprintf(ts_sw_ctrl, MAX_TS_SIZE, "%lu:%lu ",
          ts.tv_sec, ts.tv_nsec);
      power_state_ = rc;
    }
  }

  if (rc < 0) {
    Cleanup();
    return rc;
  }

  return rc;
}

bool PowerManager :: SendSocVersion(unsigned int soc_id)
{
  int err = 0;

  ALOGE("\nbt soc version  (%d)\n", soc_id);

  err = ioctl(DataHandler::Get()->GetPowerDriverFd(),
              soc_version_ctrl_cmd_,(unsigned long)soc_id);

  if (err < 0)
  {
    ALOGE(" ioctl  to send soc version failed :%d error =(%s)", err, strerror(errno));
    return false;
  }
  return true;
}

bool PowerManager :: ControlRfkill(int rfkill_fd, bool enable)
{
  char power_on = enable ? '1' : '0';

  ALOGD("%s: rfkill_fd: %d, enable: %x", __func__, rfkill_fd, enable);

  if (write(rfkill_fd, &power_on, 1) < 0) {
    ALOGE("%s: write rfkill failed: %s (%d)", __func__, strerror(errno), errno);
    Cleanup(rfkill_fd);
    return false;
  }

  return true;
}

int PowerManager :: InitializeRfkill()
{
  int fd, read_size;
  char rfkill_type[64] = {'\0'}, rfkill_state[64] = {'\0'}, type[16] = {'\0'};

  for (int i = 0; rfkill_id_ == -1; i++) {
    snprintf(rfkill_type, sizeof(rfkill_type), (char *)RFKILL_TYPE_PATH, i);
    if ((fd = open(rfkill_type, O_RDONLY)) < 0) {
      ALOGE("open(%s) failed: %s (%d)\n", rfkill_type, strerror(errno), errno);
      return -1;
    }

    read_size = read(fd, &type, sizeof(type));
    close(fd);

    if (read_size >= 9 && !memcmp(type, "bluetooth", 9)) {
      rfkill_id_ = i;
      break;
    }
  }

  snprintf(rfkill_state, sizeof(rfkill_state), (char *)RFKILL_STATE_PATH, rfkill_id_);

  fd = open(rfkill_state, O_RDWR);
  if (fd < 0)
    ALOGE("open(%s) for write failed: %s (%d)", rfkill_state, strerror(errno), errno);

  return fd;
}

bool PowerManager :: SetExtLdoStatus()
{
  int read_size, ldo_fd;
  char curr_ldo_path[64] = {'\0'}, enable_ldo[6] = {'\0'};

  snprintf(curr_ldo_path, sizeof(curr_ldo_path), (char *)EXTLDO_PATH, rfkill_id_);
  if ((ldo_fd = open(curr_ldo_path, O_RDWR)) < 0) {
    ALOGE("open(%s) failed: %s (%d)", curr_ldo_path, strerror(errno), errno);
    return false;
  }

  read_size = read(ldo_fd, &enable_ldo, sizeof(enable_ldo));
  close(ldo_fd);

  if (read_size <= 0) {
    ALOGE("read(%s) failed: %s (%d)", curr_ldo_path, strerror(errno), errno);
    return false;
  }

  if (!memcmp(enable_ldo, "true", 4)) {
    ALOGI("External LDO has been configured");
    ext_ldo_ = true;
  }
  return true;
}

int PowerManager :: Ioctl(int val)
{
  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(),
			power_ctrl_cmd_, (unsigned long)val);

  if (ret < 0)
    ALOGE(" ioctl failed to power control:%d error =(%s)", ret, strerror(errno));

  return ret;
}

int PowerManager :: Panic(void* crashInfo)
{
  ALOGD("%s: Invoking Kernel Panic from BT", __func__);

  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(),
                  kernel_panic_cmd_, crashInfo);

  if (ret < 0)
    ALOGE("ioctl: kernel panic failed:%d error =(%s)", ret, strerror(errno));

  return ret;
}

bool PowerManager :: isBtEnablePinPulledLow()
{
  return bt_enable_pin_pulled_low;
}

void PowerManager :: updateBtEnablePinStatus (int crash_state)
{
  if (!crash_state) {
    bt_enable_pin_pulled_low = true;
    return;
  }
  bt_enable_pin_pulled_low = false;
}

void PowerManager :: UpdatePlatformPowerBuff(char * power_state_buff, int *pwr_state)
{
  struct timespec ts;
  int pos = 0;

  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_SW_CTRL_GPIO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTSWCtrlGpioValue: \t %d \t\t\t %d\n",
        pwr_state[BT_SW_CTRL_GPIO], pwr_state[BT_SW_CTRL_GPIO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_RESET_GPIO] != PWR_SRC_NOT_AVAILABLE) {
    updateBtEnablePinStatus(pwr_state[BT_RESET_GPIO_CURRENT]);
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTResetGpioValue: \t %d \t\t\t %d\n",
            pwr_state[BT_RESET_GPIO], pwr_state[BT_RESET_GPIO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_AON_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddAonLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_AON_LDO],  pwr_state[BT_VDD_AON_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_DIG_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddDigLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_DIG_LDO], pwr_state[BT_VDD_DIG_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA1_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddRfa1LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA1_LDO], pwr_state[BT_VDD_RFA1_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA2_LDO] != PWR_SRC_NOT_AVAILABLE) {
        pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddRfa2LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA2_LDO], pwr_state[BT_VDD_RFA2_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_ASD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddAsdLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_ASD_LDO], pwr_state[BT_VDD_ASD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_XTAL_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddXtalLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_XTAL_LDO], pwr_state[BT_VDD_XTAL_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_PA_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddPALdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_PA_LDO], pwr_state[BT_VDD_PA_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_CORE_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddCoreLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_CORE_LDO], pwr_state[BT_VDD_CORE_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IO_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddIOLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IO_LDO],  pwr_state[BT_VDD_IO_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_LDO], pwr_state[BT_VDD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA_0p8] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddRfa0p8LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFA_0p8], pwr_state[BT_VDD_RFA_0p8_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFACMN] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddRfaCmnLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFACMN], pwr_state[BT_VDD_RFACMN_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IPA_2p2] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddIPA2p2LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IPA_2p2], pwr_state[BT_VDD_IPA_2p2_CURRENT]);
  }

  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_SMPS] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddSmpsValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_SMPS], pwr_state[BT_VDD_SMPS_CURRENT]);
  }
}

void PowerManager :: UpdateBtPowerBuff(char * power_state_buff, int *pwr_state)
{
  struct timespec ts;
  int pos = 0;

  pos += snprintf(power_state_buff, MAX_PWR_RSRC_INFO_SIZE,
      "INIT kernel TS: %s | ", ts_init);
  if (pwr_state[BT_SW_CTRL_GPIO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "SW CTRL kernel TS: %s | ", ts_sw_ctrl);
  }
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1) {
     pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "Crash kernel TS: %lu:%lu", ts.tv_sec, ts.tv_nsec);
  }
  pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\n\t\t\t\tDuring Init\t\tDuring crash\n POWER RESOURCES STATE:\n");
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_AON_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBtVddAonLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_AON_LDO],  pwr_state[BT_VDD_AON_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_DIG_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBtVddDigLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_DIG_LDO], pwr_state[BT_VDD_DIG_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA1_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBtVddRfa1LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA1_LDO], pwr_state[BT_VDD_RFA1_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA2_LDO] != PWR_SRC_NOT_AVAILABLE) {
        pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBtVddRfa2LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA2_LDO], pwr_state[BT_VDD_RFA2_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_ASD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBtVddAsdLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_ASD_LDO], pwr_state[BT_VDD_ASD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_XTAL_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBtVddXtalLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_XTAL_LDO], pwr_state[BT_VDD_XTAL_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_PA_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBtVddPALdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_PA_LDO], pwr_state[BT_VDD_PA_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_CORE_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBtVddCoreLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_CORE_LDO], pwr_state[BT_VDD_CORE_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IO_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBtVddIOLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IO_LDO],  pwr_state[BT_VDD_IO_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBtVddLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_LDO], pwr_state[BT_VDD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA_0p8] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBtVddRfa0p8LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFA_0p8], pwr_state[BT_VDD_RFA_0p8_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFACMN] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBtVddRfaCmnLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFACMN], pwr_state[BT_VDD_RFACMN_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IPA_2p2] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddIPA2p2LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IPA_2p2], pwr_state[BT_VDD_IPA_2p2_CURRENT]);
  }

  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_SMPS] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddSmpsValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_SMPS], pwr_state[BT_VDD_SMPS_CURRENT]);
  }
}

void PowerManager :: UpdateUwbPowerBuff(char * power_state_buff, int *pwr_state)
{
  struct timespec ts;
  int pos = 0;

  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_AON_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddAonLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_AON_LDO],  pwr_state[BT_VDD_AON_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_DIG_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddDigLdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_DIG_LDO], pwr_state[BT_VDD_DIG_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA1_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddRfa1LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA1_LDO], pwr_state[BT_VDD_RFA1_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA2_LDO] != PWR_SRC_NOT_AVAILABLE) {
        pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
            "\tBTVddRfa2LdoValue: \t %d \t\t %d\n",
            pwr_state[BT_VDD_RFA2_LDO], pwr_state[BT_VDD_RFA2_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_ASD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddAsdLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_ASD_LDO], pwr_state[BT_VDD_ASD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_XTAL_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddXtalLdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_XTAL_LDO], pwr_state[BT_VDD_XTAL_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_PA_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
        "\tBTVddPALdoValue: \t %d \t\t %d\n",
        pwr_state[BT_VDD_PA_LDO], pwr_state[BT_VDD_PA_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_CORE_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddCoreLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_CORE_LDO], pwr_state[BT_VDD_CORE_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IO_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddIOLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IO_LDO],  pwr_state[BT_VDD_IO_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_LDO] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_LDO], pwr_state[BT_VDD_LDO_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFA_0p8] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddRfa0p8LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFA_0p8], pwr_state[BT_VDD_RFA_0p8_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_RFACMN] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddRfaCmnLdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_RFACMN], pwr_state[BT_VDD_RFACMN_CURRENT]);
  }
  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_IPA_2p2] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddIPA2p2LdoValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_IPA_2p2], pwr_state[BT_VDD_IPA_2p2_CURRENT]);
  }

  if ((pos >= 0) && (pos < MAX_PWR_RSRC_INFO_SIZE) &&
      pwr_state[BT_VDD_SMPS] != PWR_SRC_NOT_AVAILABLE) {
    pos += snprintf(power_state_buff + pos, MAX_PWR_RSRC_INFO_SIZE - pos,
          "\tBTVddSmpsValue: \t %d \t\t %d\n",
          pwr_state[BT_VDD_SMPS], pwr_state[BT_VDD_SMPS_CURRENT]);
  }
}

int PowerManager :: GetAndLogPwrRsrcStates(struct power_buff_struct *power_buff) {
  struct {
	  int platform_reg[BT_POWER_SRC_SIZE];
	  int bt_reg[BT_POWER_SRC_SIZE];
	  int uwb_reg[BT_POWER_SRC_SIZE];
  } pwr_state;

  memset(&pwr_state, -1 , sizeof(pwr_state));
  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(), get_bt_pwr_srcs_state_cmd_, &pwr_state);

  if (ret < 0) {
    ALOGE("%s: ioctl failed: get_bt_pwr_srcs_state_cmd_ ret(%d) error(%s)",
      __func__, ret, strerror(errno));
    return ret;
  }

  UpdateBtPowerBuff(power_buff->bt_buff, pwr_state.bt_reg);
  UpdateUwbPowerBuff(power_buff->uwb_buff, pwr_state.uwb_reg);
  UpdatePlatformPowerBuff(power_buff->platform_buff, pwr_state.platform_reg);

  return 0;
}

bool PowerManager :: SendIoctlToConfigTCS()
{
  ALOGI("%s:Send IOCTL to enable 2.2v power supply for iPA ", __func__);

  int err = ioctl(DataHandler::Get()->GetPowerDriverFd(),
                  set_radio_config_in_tcs_cmd_, NULL);

  if (err < 0) {
    ALOGE("%s:failed to send ioctl  to configure TCS table :%d error =(%s)",
           __func__, err, strerror(errno));
    return false;
  }
  return true;
}

bool PowerManager :: ExternalldoStatus()
{
  return ext_ldo_;
}

void PowerManager :: Cleanup(int fd)
{
  close(fd);
  Cleanup();
}

void PowerManager :: CheckSwCtrl() {

  struct timespec ts;

  memset(ts_sw_ctrl, 0, MAX_TS_SIZE);
  ALOGD("%s: Check SW_CTRL & save value in kernel", __func__);

  if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1) {
      snprintf(ts_sw_ctrl, MAX_TS_SIZE, "%lu:%lu ",
          ts.tv_sec, ts.tv_nsec);
  }

  int ret = ioctl(DataHandler::Get()->GetPowerDriverFd(),
                  get_bt_sw_ctrl_gpio_cmd_, (unsigned long)NULL);

  if (ret < 0)
    if (errno == EINVAL) {
      ALOGD("%s: ioctl failed to check for SW_CTRL with ret :%d error =(%s)",
        __func__, ret, strerror(errno));
    } else {
      ALOGE("%s: ioctl failed to check for SW_CTRL with ret :%d error =(%s)",
        __func__, ret, strerror(errno));
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
