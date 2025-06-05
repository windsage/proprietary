/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include "EseUtils/EseUtils.h"
#include <errno.h>

static int  dev_open(uint8_t ese_vendor_id) {
    int dev_node = -1;

    if (ese_vendor_id == ESE_VENDOR_NXP) {
        dev_node = open(DEVICE_NODE_NXP, O_RDWR);
    } else if (ese_vendor_id == ESE_VENDOR_STM) {
        dev_node = open(DEVICE_NODE_STM, O_RDWR);
    } else {
        ALOGE("Unknown eSE vendor ID 0x02x", ese_vendor_id);
        return -1;
    }

    if (dev_node < 0) {
        ALOGE("%s: Failed to open dev node, VendorId 0x%02x. errno %d",
              __func__, ese_vendor_id, errno);
        return -1;
    }

    return dev_node;
}

static void dev_close(int dev_id) {
    close(dev_id);
}

int eseSetPower(bool on) {
    const char* ese_hw_dev;
    unsigned long ESE_SET_PWR;
    unsigned long ESE_SET_PWR_HIGH;
    unsigned long ESE_SET_PWR_LOW;
    int ret = 0;
    int dev_node = -1;
    uint8_t vendor_id;
    vendor_id = eseGetVendorId();

    dev_node = dev_open(vendor_id);
    if (dev_node == -1) {
        return -1;
    }

    if (vendor_id == ESE_VENDOR_NXP) {
        ESE_SET_PWR = ESE_SET_PWR_NXP;
        ESE_SET_PWR_HIGH = ESE_SET_PWR_HIGH_NXP;
        ESE_SET_PWR_LOW = ESE_SET_PWR_LOW_NXP;
    } else {
        ESE_SET_PWR = ESE_SET_PWR_STM;
        ESE_SET_PWR_HIGH = ESE_SET_PWR_HIGH_STM;
        ESE_SET_PWR_LOW = ESE_SET_PWR_LOW_STM;
    }

    if (on) {
        ret = ioctl(dev_node, ESE_SET_PWR, ESE_SET_PWR_HIGH);
    } else {
        ret = ioctl(dev_node, ESE_SET_PWR, ESE_SET_PWR_LOW);
    }

    if (ret)
        ALOGE("eseSetPower ioctl failed");

    dev_close(dev_node);
    return ret;
}

int eseIoctl(uint64_t ioctlNum, uint8_t subCmd) {
    int ret = -1;
    struct ese_ioctl_arg ioctl_arg = {};
    struct ese_cold_reset_arg cold_reset_arg = {};
    int dev_node;
    uint8_t vendor_id = eseGetVendorId();;

    dev_node = dev_open(vendor_id);
    if(dev_node < 0) {
        return -1;
    }

    switch(subCmd) {
        case ESE_COLD_RESET_DO:
        case ESE_COLD_RESET_PROTECT_EN:
        case ESE_COLD_RESET_PROTECT_DIS:
            ioctl_arg.buf = (uintptr_t)&cold_reset_arg;
            ioctl_arg.buf_size = sizeof(cold_reset_arg);
            ioctl_arg.type = ESE_ARG_TYPE_COLD_RESET;
            cold_reset_arg.src = ESE_COLD_RESET_ORIGIN_ESE;
            cold_reset_arg.sub_cmd = subCmd;
            break;
        default:
            ALOGE("%s: invalid eSE ioctl %d",subCmd);
            break;
    }

    ret = ioctl(dev_node, ioctlNum, &ioctl_arg);
    if (ret)
        ALOGE("%s: ioctl failed %d ", __func__, ret);

    dev_close(dev_node);
    return ret;
}

uint8_t eseGetVendorId() {
    uint8_t ese_vendor_id = ESE_VENDOR_UNKNOWN;

    if (access(DEVICE_NODE_NXP, F_OK) == 0) {
        ese_vendor_id = ESE_VENDOR_NXP;
    } else if (access(DEVICE_NODE_STM, F_OK) == 0) {
        ese_vendor_id = ESE_VENDOR_STM;
    } else {
        ALOGE("%s: Unknown eSE HW", __func__);
    }
    return ese_vendor_id;
}
