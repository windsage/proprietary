/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ESE_UTILS_H_
#define __ESE_UTILS_H_

static char const app_path[] = "/vendor/firmware_mnt/image";

#define TA_APP_PATH         "/vendor/firmware_mnt/image"

#define DEVICE_NODE_NXP     "/dev/nq-nci"
#define DEVICE_NODE_STM     "/dev/st54spi_gpio"

#define TRUSTED_APP_NXP     "eseservice"
#define TRUSTED_APP_STM     "st_eseservice"

#define TRUSTED_APP_UUID_NXP     "05B04A44-BF30-42DF-9E2F-B366B980ED19"
#define TRUSTED_APP_UUID_STM     "FD719D50-FFFB-11EB-9A03-0242AC130003"

#define ESE_HW_NAME_NXP     "NXP"
#define ESE_HW_NAME_STM     "STM"

#define SPI_GPIO_MAGIC_NXP  0xE9
#define SPI_GPIO_MAGIC_STM  0xEB

#define ESE_SET_PWR_NXP     _IOW(SPI_GPIO_MAGIC_NXP, 0x02, unsigned int);
#define ESE_COLD_RESET_NXP  _IOWR(SPI_GPIO_MAGIC_NXP, 0x08, struct ese_ioctl_arg)

#define ESE_SET_PWR_STM     _IOW(SPI_GPIO_MAGIC_STM, 0x02, unsigned int);

#define ESE_SET_PWR_HIGH_NXP    0
#define ESE_SET_PWR_LOW_NXP     1

#define ESE_SET_PWR_HIGH_STM    1
#define ESE_SET_PWR_LOW_STM     0

typedef enum
{
    ESE_VENDOR_UNKNOWN = 0x00,
    ESE_VENDOR_NXP     = 0x01,
    ESE_VENDOR_STM     = 0x02,
} ESE_VENDOR_ID;

/* ESE_COLD_RESET ioctl sub commands, max 8 are supported */
enum ese_cold_reset_sub_cmd {
    ESE_COLD_RESET_DO = 0,
    ESE_COLD_RESET_PROTECT_EN,
    ESE_COLD_RESET_PROTECT_DIS,
};

/* ESE_COLD_RESET ioctl origin */
enum ese_cold_reset_origin {
    ESE_COLD_RESET_ORIGIN_ESE = 0,
    ESE_COLD_RESET_ORIGIN_NFC,
    ESE_COLD_RESET_ORIGIN_NONE = 0xFF,
};

enum ese_ioctl_arg_type {
    ESE_ARG_TYPE_COLD_RESET = 0,
    ESE_ARG_TYPE_ESE_DATA,
};

struct ese_cold_reset_arg {
    __u8 src;
    __u8 sub_cmd;
    __u16 rfu;
};

struct ese_ioctl_arg {
    __u64 buf;
    __u32 buf_size;
    __u8 type;
};

int eseSetPower(bool on);
int eseIoctl(uint64_t ioctlNum, uint8_t subCmd);
uint8_t eseGetVendorId();

#endif


