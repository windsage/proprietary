/*==========================================================================

# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <unistd.h>
#include <utils/Log.h>
#include <sys/ioctl.h>
#include <cutils/sockets.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <string>
#include <string.h>
#define LOG_TAG "bluetooth_transport_properties"

static const int get_soc_id_cmd = 0xbfaf;
static const int set_cp_status  = 0xbfaf;

#define BT_KP_TRANSPORT_DEVICE     "/dev/btfmcodec_dev"

std::string setVendorPropertiesDefault() {
  int ret = 0;
  std::string ret_string = "SoC_NAME_UNKOWN";
  char soc_name[PROPERTY_VALUE_MAX];
  char compatable_chipset_id[32] = {'\0'};
  int  fd_btdev;
  ret = property_get("persist.vendor.qcom.bluetooth.soc", soc_name, "");
  ALOGD("%s property_get return value %d", __func__, ret);
  if (ret == 0) {
    fd_btdev = open("/dev/btpower", O_RDWR | O_NONBLOCK);
    if (fd_btdev < 0) {
      ALOGE("\nfailed to open bt device error = (%s)\n", strerror(errno));
      return ret_string;
    }
    ret = ioctl(fd_btdev, get_soc_id_cmd, compatable_chipset_id);
    close(fd_btdev);
    if (ret < 0) {
      ALOGE("%s: ioctl failed error = %d\n",__func__, ret);
      return ret_string;
    } else {
      ALOGD("%s:Soc version recevied : %s\n", __func__, compatable_chipset_id);
    }
    if (strstr(compatable_chipset_id, "qca6490")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastings");
      ret_string = "hastings";
    } else if (strstr(compatable_chipset_id, "qca6390")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastings");
      ret_string = "hastings";
    } else if (strstr(compatable_chipset_id, "kiwi") ||
               strstr(compatable_chipset_id, "wcn7850")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "hamilton");
      ret_string = "hamilton";
    } else if (strstr(compatable_chipset_id, "peach")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "ganges");
      ret_string = "ganges";
    } else if (strstr(compatable_chipset_id, "wcn3990")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "cherokee");
      ret_string = "cherokee";
    } else if (strstr(compatable_chipset_id, "wcn6750")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "moselle");
      ret_string = "moselle";
    } else if (strstr(compatable_chipset_id, "wcn6450")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "evros");
      ret_string = "evros";
    }

    if (ret < 0) {
      ALOGE("%s: set soc name property failed error = %d\n",__func__, ret);
    }
  } else {
    ALOGD("%s:vendor properties set : soc name  : %s\n", __func__, soc_name);
    ret_string = std::string(soc_name);
  }
  return ret_string;
}

void isCpsupported(void)
{
  char value[PROPERTY_VALUE_MAX];
  bool is_cp_supported = false;
  int ret;
  int fd;

  ret = property_get("persist.vendor.qcom.bluetooth.is_cp_supported", value, "true");
  if (strcmp(value, "true") == 0) {
    ALOGD("%s: Connectivity Proxy supported on this platform", __func__);
    is_cp_supported = true;
  }

  fd = open(BT_KP_TRANSPORT_DEVICE, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    ALOGE("%s: unable to dev node %s", __func__, BT_KP_TRANSPORT_DEVICE);
    return;
  }
  ret = ioctl(fd, set_cp_status, (is_cp_supported == true? 1: 0));

  if (ret < 0) {
    ALOGE("%s: Failed to update CP status to bfmcodec driver", __func__);
  }

  close(fd);
}
