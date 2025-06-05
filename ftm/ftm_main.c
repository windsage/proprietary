/*
 Copyright (c) 2010-2017, 2021-2024 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*==========================================================================

                     FTM Main Task Source File

Description
  Unit test component file for regsitering the routines to Diag library
  for BT and FTM commands

===========================================================================*/

/*===========================================================================

                         Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/10   rakeshk  Created a source file to implement routines for
                    registering the callback routines for FM and BT FTM
                    packets
07/06/10   rakeshk  changed the name of FM common header file in inclusion
07/07/10   rakeshk  Removed the sleep and wake in the main thread loop
===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <net/if.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <dirent.h>
#include <pwd.h>
#include <stdbool.h>
#ifndef CONFIG_WLAN_QCLINUX
#ifndef WIN_AP_HOST
#include <cutils/sched_policy.h>
#include <cutils/properties.h>
#endif
#endif
#include <ctype.h>
#include <getopt.h>
#ifndef ANDROID
#include <sys/socket.h>
#include <linux/un.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#define ERROR 0
#ifdef CONFIG_FTM_BT
static const int get_soc_id_cmd = 0xbfaf;
#endif
#endif
#if !defined(WIN_AP_HOST) && defined(CONFIG_FTM_BT)
#ifndef USE_LIBBT_VENDOR
#include "hidl_client.h"
#endif
#endif
/* Diag related header files */
#include "event.h"
#include "msg.h"
#include "log.h"
#include "diag_lsm.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diag.h"

#if defined(CONFIG_FTM_BT) || defined(CONFIG_FTM_FM)
pthread_mutex_t fm_event_lock;
pthread_cond_t fm_event_cond;
#endif

#ifdef WIN_AP_HOST_OPEN
#include "signal.h"
#include "libtcmd.h"
#endif
#include "ftm_dbg.h"
#include "ftm_wlan.h"
#include "ftm_common.h"
#include "ftm_ant_common.h"
#ifndef WIN_AP_HOST
#ifdef CONFIG_FTM_BT
#include "ftm_bt_hci_pfal.h"
#endif
#ifdef CONFIG_FTM_FM
#include "ftm_fm_pfal.h"
#endif

#include "ftm_nfc.h"
#endif
#ifdef USE_GLIB
#include <glib.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif

#ifdef IPQ_AP_HOST_IOT
#include <semaphore.h>
#include "ftm_iot.h"
#include "signal.h"
#ifdef IPQ_AP_HOST_IOT_QCA402X
#include "diag_api.h"
#endif /* Including IPQ-QCA402x specific headerfiles */
#endif
#ifdef WIN_AP_HOST
#include "ftm_wlan_win.h"
#endif

#ifdef CONFIG_FTM_UWB
#include "ftm_uwb.h"
#endif

extern void hidl_client_close();
int boardtype = 8660;
int first_ant_command;

int fm_passthrough = 0;
int sibs_enabled = 0;
static char *progname = NULL;
bool block_bt = false;
bool block_uwb = false;
unsigned int g_dbg_level = FTM_DBG_DEFAULT;

/* Sempahore Timeout Period set for 5 seconds */
int user_sem_wait_timeout = 5;

#ifdef IPQ_AP_HOST_IOT
int interface = 0;
int thread_stop = 0;
sem_t iot_sem;
sem_t iot_sem_async;
#endif

#ifdef WIN_AP_HOST
struct ftm_config ftm_cfg;
#endif

#define SHOW_PRIO 1
#define SHOW_TIME 2
#define SHOW_POLICY 4
#define SHOW_CPU  8
#define SHOW_MACLABEL 16
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*a))

#ifndef WIN_AP_HOST
#ifndef ANDROID
#define SOCKETNAME  "/data/misc/bluetooth/btprop"
static int sk;
#endif
#endif

#ifdef CONFIG_FTM_BT
/* Semaphore to monitor the completion of
* the queued command before sending down the
* next HCI cmd
*/
sem_t semaphore_cmd_complete;
/* Semaphore to monitor whether a command
* is queued before proceeding to dequeue
* the HCI packet
*/
sem_t semaphore_cmd_queued;


int soc_type;

/* Diag pkt table for BT */
static const diagpkt_user_table_entry_type bt_ftm_diag_func_table[] =
{
  {FTM_BT_CMD_CODE, FTM_BT_CMD_CODE , bt_ftm_diag_dispatch},
};
#endif /* CONFIG_FTM_BT */



#ifdef CONFIG_FTM_UWB

#define UWB_CMD_TIME_OUT_IN_SECONDS    5
bool uwb_ftm_enabled = FALSE;

/* Callback declaration for UWB FTM packet processing */
void *uwb_ftm_diag_dispatch(void *req_pkt, uint16 pkt_len);

/* Semaphore to monitor the completion of
* the queued command before sending down the
* next uwb cmd.
*/
sem_t semaphore_uwb_cmd_complete;
/* Semaphore to monitor whether a command
* is queued before proceeding to dequeue
* the uwb packet
*/
sem_t semaphore_uwb_cmd_queued;

/* Diag pkt table for UWB */
static const diagpkt_user_table_entry_type uwb_ftm_diag_func_table[] =
{
  {FTM_UWB_CMD_CODE, FTM_UWB_CMD_CODE, uwb_ftm_diag_dispatch},
};
#endif /* CONFIG_FTM_UWB */

#ifdef DEBUG
void current_time()
{
    struct timeval curTime;
    time_t now;
    int milli, usec;
    struct tm timeinfo;
    char buffer[TIMESTAMP_BUFLEN] = "";
    char currentTime[TIME_BUFLEN] = "";
    gettimeofday(&curTime, NULL);
    now   = curTime.tv_sec;
    milli = curTime.tv_usec / 1000;
    usec  = curTime.tv_usec;

    localtime_r(&now, &timeinfo);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    snprintf(currentTime, sizeof(currentTime), "%s:%03d:%06d", buffer, milli, usec);

    DPRINTF(FTM_DBG_TRACE, "Time:%s\n", currentTime);

}
#endif /* DEBUG */



#ifdef CONFIG_FTM_FM
/* Callback declaration for BT FTM packet processing */
void *fm_ftm_diag_dispatch (void *req_pkt, uint16 pkt_len);

/* Diag pkt table for FM */
static const diagpkt_user_table_entry_type fm_ftm_diag_func_table[] =
{
  {FTM_FM_CMD_CODE, FTM_FM_CMD_CODE, fm_ftm_diag_dispatch},
};
#endif /* CONFIG_FTM_FM */

#ifdef CONFIG_FTM_ANT
/* Callback declaration for ANT FTM packet processing */
void *ant_ftm_diag_dispatch(void *req_pkt, uint16 pkt_len);

/*Diag pkt table for ANT */
static const diagpkt_user_table_entry_type ant_ftm_diag_func_table[] =
{
  {FTM_ANT_CMD_CODE, FTM_ANT_CMD_CODE, ant_ftm_diag_dispatch}
};
#endif /* CONFIG_FTM_ANT */

#ifdef CONFIG_FTM_WLAN
/* Callback declaration for WLAN FTM packet processing */
void * wlan_ftm_diag_dispatch (void *req_pkt,
  uint16 pkt_len);

/* Diag pkt table for WLAN */
static const diagpkt_user_table_entry_type wlan_ftm_diag_func_table[] =
{
  {FTM_WLAN_CMD_CODE, FTM_WLAN_CMD_CODE, wlan_ftm_diag_dispatch}
};
#endif

#ifdef IPQ_AP_HOST_IOT
/*IPQ-QCA402x specific diag API handler*/
void *iotd_desc;
#ifdef IPQ_AP_HOST_IOT_IPQ
int btss_desc;
extern int BAUD_RATE;
extern int btss_init();
extern void btss_deinit();
#endif /* IPQ50XX, IPQ95XX*/
/* Callback declaration for IPQ-QCA402x FTM packet processing */
void *iot_ftm_diag_dispatch (void *req_pkt,
   uint16 pkt_len);


 /* Diag pkt table for IPQ-QCA402X */
 static const diagpkt_user_table_entry_type iot_ftm_diag_func_table[] =
 {
   {MFG_CMD_ID_BLE_HCI, MFG_CMD_ID_BLE_HCI, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_I15P4_HMI, MFG_CMD_ID_I15P4_HMI, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_WRITE_BYTE, MFG_CMD_ID_OTP_WRITE_BYTE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_READ_BYTE, MFG_CMD_ID_OTP_READ_BYTE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_INVALID, MFG_CMD_ID_OTP_INVALID, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_SET_BITS, MFG_CMD_ID_OTP_SET_BITS, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_TLV_INIT, MFG_CMD_ID_OTP_TLV_INIT, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_TLV_READ, MFG_CMD_ID_OTP_TLV_READ, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_TLV_WRITE, MFG_CMD_ID_OTP_TLV_WRITE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_TLV_STATUS, MFG_CMD_ID_OTP_TLV_STATUS, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_OTP_TLV_DELETE, MFG_CMD_ID_OTP_TLV_DELETE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_INVALID, MFG_CMD_ID_FS_INVALID, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_READ, MFG_CMD_ID_FS_READ, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_WRITE, MFG_CMD_ID_FS_WRITE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_DELETE, MFG_CMD_ID_FS_DELETE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_LIST_SETUP, MFG_CMD_ID_FS_LIST_SETUP, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_LIST_NEXT, MFG_CMD_ID_FS_LIST_NEXT, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_MOUNT, MFG_CMD_ID_FS_MOUNT, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_FS_UNMOUNT, MFG_CMD_ID_FS_UNMOUNT, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_REBOOT, MFG_CMD_ID_MISC_REBOOT, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_ADDR_READ, MFG_CMD_ID_MISC_ADDR_READ, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_ADDR_WRITE, MFG_CMD_ID_MISC_ADDR_WRITE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_HWSS_DONE, MFG_CMD_ID_MISC_HWSS_DONE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_XTAL_CAP_SET, MFG_CMD_ID_MISC_XTAL_CAP_SET, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_PART_SZ_GET, MFG_CMD_ID_MISC_PART_SZ_GET, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_PROG_MODE, MFG_CMD_ID_MISC_PROG_MODE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_MISC_FWUP, MFG_CMD_ID_MISC_FWUP, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_INVALID, MFG_CMD_ID_RAWFLASH_INVALID, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_CLEAR_BITS, MFG_CMD_ID_RAWFLASH_CLEAR_BITS, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_READ, MFG_CMD_ID_RAWFLASH_READ, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_WRITE, MFG_CMD_ID_RAWFLASH_WRITE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_ERASE, MFG_CMD_ID_RAWFLASH_ERASE, iot_ftm_diag_dispatch},
   {MFG_CMD_ID_RAWFLASH_DISABLE_MFG, MFG_CMD_ID_RAWFLASH_DISABLE_MFG, iot_ftm_diag_dispatch},
 };

#endif /*ifdef IPQ_AP_HOST_IOT*/
#ifdef CONFIG_FTM_NFC
/* Callback declaration for NFC FTM packet processing */
void *nfc_ftm_diag_dispatch (void *req_pkt, uint16 pkt_len);

/*Diag pkt table for NFC */
static const diagpkt_user_table_entry_type nfc_ftm_diag_func_table[] =
{
  {FTM_NFC_CMD_CODE, FTM_NFC_CMD_CODE, nfc_ftm_diag_dispatch}
};
#endif /* CONFIG_FTM_NFC */
#ifdef WIN_AP_HOST_OPEN
static void signal_handler(int32_t signum)
{
	printf("FTM Daemon Exit called signal %d\n", signum );
	tcmd_tx_stop();
        exit(0);
}
#endif

#ifdef IPQ_AP_HOST_IOT
static void ftm_signal_handler(int32_t signum)
{
    printf("FTM Daemon Exit called signal %d\n", signum );

#ifdef IPQ_AP_HOST_IOT_IPQ50XX
    btss_deinit();
#endif
    exit(0);
}
#endif

#ifdef CONFIG_FTM_BT
#ifndef WIN_AP_HOST
#ifndef ANDROID
/* set up Socket connection to btproperty */
int prop_get(const char *key, char *value, const char *default_value)
{
    char prop_string[200];
    int ret, bytes_read = 0, i = 0;

    if(default_value == NULL) {
	    ALOGI("prop_get: invalid default_value");
	    return 1;
    }
    snprintf(prop_string, sizeof(prop_string), "get_property %s,", key);
    ret = send(sk, prop_string, strlen(prop_string), 0);
    memset(value, 0, sizeof(value));
    do
    {
        bytes_read = recv(sk, &value[i], 1, 0);
        if (bytes_read == 1)
        {
            if (value[i] == ',')
            {
                value[i] = '\0';
                break;
            }
            i++;
        }
    } while(1);
    ALOGI("property_get_bt: key(%s) has value: %s", key, value);
    if (bytes_read) {
        return 0;
    } else {
        strlcpy(value, default_value, strlen(default_value) + 1);
        return 1;
    }
}

int prop_set(const char *key, const char *value)
{
    char prop_string[200];
    int ret;
    snprintf(prop_string, sizeof(prop_string), "set_property %s %s,", key, value);
    ALOGI("property_set_bt: setting key(%s) to value: %s\n", key, value);
    ret = send(sk, prop_string, strlen(prop_string), 0);
    return 0;
}

#ifdef USE_LIBBT_VENDOR
int bt_property_init(void)
{
	int len;    /* length of sockaddr */
	struct sockaddr_un name;
	if( (sk = socket(AF_UNIX, SOCK_STREAM, 0) ) < 0) {
		perror("socket");
		return -1;
	}
	/*Create the address of the server.*/
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_UNIX;
	strlcpy(name.sun_path, SOCKETNAME, sizeof(name.sun_path));
	ALOGI("connecting to %s, fd = %d", SOCKETNAME, sk);
	len = sizeof(name.sun_family) + strlen(name.sun_path);
	/*Connect to the server.*/
	if (connect(sk, (struct sockaddr *) &name, len) < 0){
		perror("connect");
		return -1;
	}
#if (BT_SOC_TYPE_ROME || BT_SOC_TYPE_CHEROKEE)
        prop_set("persist.vendor.qcom.bluetooth.soc", "rome");
        ALOGE("bt_property_init ftm_main.c rome");
#endif
	return 0;
}
#endif
#endif

/* Get Bluetooth SoC type from system setting */
static int get_bt_soc_type(void)
{
    int ret = 0;
    char bt_soc_type[PROPERTY_VALUE_MAX];
    ALOGI("bt-hci: get_bt_soc_type");
#ifndef ANDROID
    int fd_btdev;
    char ret_string[100] = "SoC_NAME_UNKOWN";
    char soc_name[PROPERTY_VALUE_MAX];
    char compatable_chipset_id[32] = {'\0'};
    fd_btdev = open("/dev/btpower", O_RDWR | O_NONBLOCK);
    if (fd_btdev < ERROR) {
        ALOGE("\nfailed to open bt device error = (%s)\n", strerror(errno));
        return 0;
     }
     ret = ioctl(fd_btdev, get_soc_id_cmd, compatable_chipset_id);
     close(fd_btdev);
    if (strstr(compatable_chipset_id, "wcn3990")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "cherokee");
              property_set("vendor.qcom.bluetooth.soc", "cherokee");
              ALOGE("bt_property_init cherokee");
    }
// defined for OpenWrt SPs
#ifdef OWRT_BUILD
    else if (strstr(compatable_chipset_id, "qca6174")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "rome");
              property_set("vendor.qcom.bluetooth.soc", "rome");
              ALOGE("bt_property_init rome");
    }
    else if (strstr(compatable_chipset_id, "qca6490")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastingsprime");
              property_set("vendor.qcom.bluetooth.soc", "hastingsprime");
              ALOGE("bt_property_init hastingsprime");
    }
#else
    else if (strstr(compatable_chipset_id, "qca6490")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastings");
              property_set("vendor.qcom.bluetooth.soc", "hastings");
              ALOGE("bt_property_init hastings");
    }
#endif
    else if (strstr(compatable_chipset_id, "wcn6750")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "moselle");
              property_set("vendor.qcom.bluetooth.soc", "moselle");
              ALOGE("bt_property_init moselle");
    }
    else if (strstr(compatable_chipset_id, "qca6490")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "pronto");
              property_set("vendor.qcom.bluetooth.soc", "pronto");
              ALOGE("bt_property_init pronto");
    }
    else if (strstr(compatable_chipset_id, "kiwi") ||
             strstr(compatable_chipset_id, "wcn7850")) {
              ret = property_set("persist.vendor.qcom.bluetooth.soc", "hamilton");
              property_set("vendor.qcom.bluetooth.soc", "hamilton");
              ALOGE("bt_property_init hamilton");
    }
    else if (strstr(compatable_chipset_id, "peach") ||
	     strstr(compatable_chipset_id, "wcn78xx")) {
	      ret = property_set("persist.vendor.qcom.bluetooth.soc", "ganges");
              property_set("vendor.qcom.bluetooth.soc", "ganges");
              ALOGE("bt_property_init ganges");
    }
    else if (strstr(compatable_chipset_id, "wcn7750")) {
	      ret = property_set("persist.vendor.qcom.bluetooth.soc", "orne");
              property_set("vendor.qcom.bluetooth.soc", "orne");
              ALOGE("bt_property_init ganges");
    }
    else if (strstr(compatable_chipset_id, "wcn6450")) {
	      ret = property_set("persist.vendor.qcom.bluetooth.soc", "evros");
              property_set("vendor.qcom.bluetooth.soc", "evros");
              ALOGE("bt_property_init evros");
    }
#if (BT_SOC_TYPE_CHEROKEE)
    property_set("persist.vendor.qcom.bluetooth.soc", "cherokee");
    ALOGE("get_bt_soc_type ftm_main.c cherokee");
#else
#if (BT_SOC_TYPE_ROME)
#ifdef USE_LIBBT_VENDOR
        if (bt_property_init() < 0) {
                ALOGE("%s: Failed to get soc type", __func__);
                ret = BT_SOC_DEFAULT;
                return ret;
         }
        ret = !prop_get("persist.vendor.qcom.bluetooth.soc", bt_soc_type, "rome");
        ALOGE("get_bt_soc_type ftm_main.c rome");
#else
        property_set("vendor.qcom.bluetooth.soc", "rome");
        ALOGE("get_bt_soc_type ftm_main.c rome");
#endif
#endif
#endif
#if (BT_SOC_TYPE_HASTINGS)
    property_set("vendor.qcom.bluetooth.soc", "hastings");
#endif
#endif
#ifndef USE_LIBBT_VENDOR
    ret = property_get("persist.vendor.qcom.bluetooth.soc", bt_soc_type, NULL);
#endif
#ifndef ANDROID
#if (PLATFORM_QRB5165)
    ret = property_get("vendor.qcom.bluetooth.soc", bt_soc_type, NULL);
#endif
#endif
    if (ret != 0) {
        ALOGI("soc_type  set to %s\n", bt_soc_type);
        if (!strncasecmp(bt_soc_type, "rome", sizeof("rome"))) {
            return BT_SOC_ROME;
        }
        else if (!strncasecmp(bt_soc_type, "cherokee", sizeof("cherokee"))) {
            return BT_SOC_CHEROKEE;
        }
        else if (!strncasecmp(bt_soc_type, "pronto", sizeof("pronto"))) {
            return BT_SOC_DEFAULT;
        }
        else if (!strncasecmp(bt_soc_type, "ath3k", sizeof("ath3k"))) {
            return BT_SOC_AR3K;
        }
        else if (!strncasecmp(bt_soc_type, "hastings", sizeof("hastings"))) {
            return BT_SOC_HASTINGS;
        }
#ifdef OWRT_BUILD
        else if (!strncasecmp(bt_soc_type, "hastingsprime", sizeof("hastingsprime"))) {
            return BT_SOC_HASTINGS;
        }
#endif
        else if (!strncasecmp(bt_soc_type, "genoa", sizeof("genoa"))) {
            return BT_SOC_GENOA;
        }
        else if (!strncasecmp(bt_soc_type, "moselle", sizeof("moselle"))) {
            return BT_SOC_MOSELLE;
        }
        else if (!strncasecmp(bt_soc_type, "hamilton", sizeof("hamilton"))) {
            return BT_SOC_HAMILTON;
        }
        else if (!strncasecmp(bt_soc_type, "slate", sizeof("slate"))) {
            return BT_SOC_SLATE;
        }
        else if (!strncasecmp(bt_soc_type, "ganges", sizeof("ganges"))) {
            return BT_SOC_GANGES;
        }
        else if (!strncasecmp(bt_soc_type, "orne", sizeof("orne"))) {
            return BT_SOC_ORNE;
        }
        else if (!strncasecmp(bt_soc_type, "evros", sizeof("evros"))) {
            return BT_SOC_EVROS;
        }
        else {
            ALOGI("vendor.qcom.bluetooth.soc not set, so using default.\n");
            return BT_SOC_DEFAULT;
        }
    }
    else {
        ALOGE("%s: Failed to get soc type", __FUNCTION__);
        ret = BT_SOC_DEFAULT;
    }
    return ret;
}
#endif //ifndef WIN_AP_HOST
#endif //ifdef CONFIG_FTM_BT

/*===========================================================================
FUNCTION  ioctl_write

DESCRIPTION
  Helper function to package the ioctl messages and send it to the I2C driver

DEPENDENCIES
  NIL

RETURN VALUE
  -1 in failure,positive or zero in success

SIDE EFFECTS
  None

===========================================================================*/

static int ioctl_readwrite(int fd, struct i2c_msg *msgs, int nmsgs)
{
  struct i2c_rdwr_ioctl_data msgset =
  {
    .msgs = msgs,
    .nmsgs = nmsgs,
  };

  if ((fd < 0) || (NULL == msgs) || (nmsgs <= 0))
  {
    return -1;
  }

  if (ioctl(fd, I2C_RDWR, &msgset) < 0)
  {
    return -1;
  }
  return 0;
}
/*===========================================================================
FUNCTION  i2c_write

DESCRIPTION
  Helper function to construct the I@C request to be sent to the FM I2C
  driver

DEPENDENCIES
  NIL

RETURN VALUE
  -1 in failure,positive or zero in success

SIDE EFFECTS
  None

===========================================================================*/
int i2c_write
(
int fd,
unsigned char offset,
const unsigned char* buf,
unsigned char len,
unsigned int slave_addr
)
{
  unsigned char offset_data[((1 + len) * sizeof(unsigned char))];
  struct i2c_msg msgs[] =
  {
    [0] = {
            .addr = slave_addr,
            .flags = 0,
            .buf = (void *)offset_data,
            .len = (1 + len) * sizeof(*offset_data),
           },
 };

 offset_data[0] = offset;
 memcpy(offset_data + 1, buf, len);

 return ioctl_readwrite(fd, msgs, ARRAY_SIZE(msgs));
}

/*===========================================================================
FUNCTION  i2c_read

DESCRIPTION
  Helper function to construct the I2C request to read data from the FM I2C
  driver

DEPENDENCIES
  NIL

RETURN VALUE
  -1 in failure,positive or zero in success

SIDE EFFECTS
  None

===========================================================================*/
int i2c_read
(
int fd,
unsigned char offset,
const unsigned char* buf,
unsigned char len,
unsigned int slave_addr
)
{
  unsigned char offset_data[] = {offset};
  struct i2c_msg msgs[] =
  {
    [0] = {
            .addr = slave_addr,
            .flags = 0,
            .buf = (void *)offset_data,
            .len = sizeof(*offset_data),
           },
    [1] = {
              .addr = slave_addr,
              .flags = I2C_M_RD,
              .buf = (void *)buf,
              .len = len,
           },
 };

 return ioctl_readwrite(fd, msgs, ARRAY_SIZE(msgs));
}

#ifdef CONFIG_FTM_FM
/*=========================================================================
FUNCTION   fm_ftm_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the FTM FM layer for further
  processing

DEPENDENCIES
  NIL

RETURN VALUE
  pointer to FTM FM Response packet

SIDE EFFECTS
  None

===========================================================================*/
void *fm_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
 void *rsp = NULL;

 DPRINTF(FTM_DBG_TRACE, "FM I2C Send Response = %d\n",pkt_len);

 // Allocate the same length as the request.
 rsp = ftm_fm_dispatch(req_pkt,pkt_len);
 return rsp;
}
#endif /* CONFIG_FTM_FM */

#ifdef CONFIG_FTM_ANT
/*===========================================================================
FUNCTION   ant_ftm_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the FTM ANT layer for further
  processing
DEPENDENCIES
 NIL

RETURN VALUE
  pointer to FTM ANT  Response packet

SIDE EFFECTS
  None

===========================================================================*/
void *ant_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
 void *rsp = NULL;

 DPRINTF(FTM_DBG_TRACE, "ANT diag dispatch send response = %d\n", pkt_len);

// Allocate the same length as the request.
 rsp = ftm_ant_dispatch(req_pkt,pkt_len);
 return rsp;
}
#endif /* CONFIG_FTM_ANT */

#ifdef CONFIG_FTM_BT
/*===========================================================================
FUNCTION   bt_ftm_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the FTM BT layer for further
  processing

DEPENDENCIES
  NIL

RETURN VALUE
  pointer to FTM BT Response packet

SIDE EFFECTS
  None

===========================================================================*/
void *bt_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
  void *rsp = NULL;
  boolean status = TRUE;

  DPRINTF(FTM_DBG_TRACE, "Send Response = %d\n",pkt_len);

  // Allocate the same length as the request.
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_FTM, FTM_BT_CMD_CODE, pkt_len);

  if (rsp != NULL)
  {
    memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
  }
  /* Spurious incoming request packets are occasionally received
   * by DIAG_SUBSYS_FTM which needs to be ignored and accordingly responded.
   * TODO: Reason for these spurious incoming request packets is yet to be
   *       found, though its always found to be corresponding to this majic
   *       length of 65532.
   */
  if (pkt_len == 65532)
  {
    printf("\nIgnore spurious DIAG packet processing & respond immediately");
  }
  else
  {

    DPRINTF(FTM_DBG_TRACE, "Insert BT packet = %d\n", pkt_len);

    /* add the BT packet into the Cmd Queue
     * and notify the main thread its queued
     */
    status = qinsert_cmd((ftm_bt_pkt_type *)req_pkt);
    if(status == TRUE)
      sem_post(&semaphore_cmd_queued);

    DPRINTF(FTM_DBG_TRACE, "Insert BT packet done\n");

  }
  return (rsp);
}
#endif /* CONFIG_FTM_BT */

#ifdef CONFIG_FTM_WLAN
/*===========================================================================
FUNCTION   wlan_ftm_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the FTM WLAN layer for further
  processing

DEPENDENCIES
  NIL

RETURN VALUE
  pointer to FTM WLAN Response packet

SIDE EFFECTS
  None

===========================================================================*/

void * wlan_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
    void *rsp = NULL;

    DPRINTF(FTM_DBG_TRACE, "WLAN Send Response = %d\n", pkt_len);

    rsp = ftm_wlan_dispatch(req_pkt, pkt_len);

    return rsp;
}
#endif /* CONFIG_FTM_WLAN */

#ifdef IPQ_AP_HOST_IOT

void *iot_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
    void *rsp = NULL;
    rsp = ftm_iot_dispatch(req_pkt, pkt_len ,iotd_desc);
    return rsp;
}

#endif /*IPQ_AP_HOST_IOT*/
#ifdef CONFIG_FTM_NFC
/*===========================================================================
FUNCTION   nfcs_ftm_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the FTM NFC layer for further
  processing
DEPENDENCIES
 NIL

RETURN VALUE


SIDE EFFECTS


===========================================================================*/
void *nfc_ftm_diag_dispatch
(
  void *req_pkt,
  uint16 pkt_len
)
{
    void *rsp = NULL;
    boolean status = TRUE;

    DPRINTF(FTM_DBG_TRACE, " NFC Send Response = %d\n",pkt_len);

    /*now send the incoming nfc diag command packet to the nfc ftm layer to
      get it processed*/
    rsp = ftm_nfc_dispatch(req_pkt, pkt_len);

    /* send same response as recieved back*/
    return rsp;
}
#endif /* CONFIG_FTM_NFC */

static void usage(void)
{
    fprintf(stderr, "\nusage: %s [options] \n"
            "   -n, --nodaemon      do not run as a daemon\n"
            "   -p, --passthrough   FM passthrough mode \n"
            "   -d                  show more debug messages (-dd for even more)\n"
            "   -n -t               Tag read test\n"
            "   -n -t -d            eSE DWP test\n"
            "   -s                  do not intialize BT handlers to block BT command\n"
            "   -q, --sem-timeout\n"
            "       set the semaphore wait timeout to value other than default (5)\n"
            "   -r, --baud-rate     set the baudrate based on QCC710 chip version\n"
#ifdef CONFIG_FTM_BT
            "   -b, --board-type    Board Type\n"
#endif
#ifdef CONFIG_FTM_WLAN
            "   -i <wlan interface>\n"
            "       --interface=<wlan interface>\n"
            "                       wlan adapter name (wlan, eth, etc.) default wlan\n"
#endif
#ifdef CONFIG_FTM_NFC
            "   -f                  nfc firmware download\n"
            "   -t                  nfc test\n"
#endif
#ifdef WIN_AP_HOST
            "   -c                  FTM config file path\n"
#endif
#ifdef CONFIG_FTM_UWB
            "   -w                  uwb test\n"
#endif
            "       --help          display this help and exit\n"
            , progname);
    exit(EXIT_FAILURE);
}

#ifdef WIN_AP_HOST
int parse_ftm_cfg_file(FILE *fp, char *board_name)
{
    char buf[64] = {0};
    char *ret = NULL;
    size_t len = 0;
    char *endptr = NULL;
    int i = 0;

    while (fgets(buf, sizeof(buf), fp)) {
        len = strlen(buf);
        if (!len)
            continue;

        buf[len - 1] = '\0';
        endptr = buf;

        if (endptr && (*endptr == '#' || *endptr == ' '))
            continue;

        /* ap-al02-c4,0x0001,1,0x1000,0x32000 */
        ret = strtok_r(endptr, ",", &endptr);
        if (!ret)
            continue;
        if (strcmp(board_name, ret))
            continue;

        ret = strtok_r(endptr, ",", &endptr);
        if (ret) {
            ftm_cfg.board_id[i] = (uint32_t)strtol(ret, &ret, 16);
        } else {
            DPRINTF(FTM_DBG_ERROR, "Invalid board id\n");
            exit(EXIT_FAILURE);
        }

        ret = strtok_r(endptr, ",", &endptr);
        if (ret) {
            ftm_cfg.slot_id[i] = (uint8_t)strtol(ret, &ret, 10);
        } else {
            DPRINTF(FTM_DBG_ERROR, "Invalid slot ID\n");
            exit(EXIT_FAILURE);
        }

        ret = strtok_r(endptr, ",", &endptr);
        if (ret) {
            ftm_cfg.slot_offset[i] = (uint32_t)strtol(ret, &ret, 10);
        } else {
            DPRINTF(FTM_DBG_ERROR, "Invalid slot offset\n");
            exit(EXIT_FAILURE);
        }

        ret = strtok_r(endptr, ",", &endptr);
        if (ret) {
            ftm_cfg.slot_size[i] = (uint32_t)strtol(ret, &ret, 10);
        } else {
            DPRINTF(FTM_DBG_ERROR, "Invalid slot size\n");
            exit(EXIT_FAILURE);
        }
        i++;
    }

    ftm_cfg.total_num_slots = i;
    if (ftm_cfg.total_num_slots <= 0) // Incase no info is present in the conf file we return -1 and throw error.
	return -1;
    else
	return 0;
}
#endif


#ifdef CONFIG_FTM_UWB

/*===========================================================================
FUNCTION   uwb_ftm_diag_dispatch

DESCRIPTION
  Processes uwb request packet and stores it in a uwb cmd queue to send it to
  uwb chip for processing.

DEPENDENCIES
  NIL

RETURN VALUE
  pointer to FTM uwb Response packet

SIDE EFFECTS
  None

===========================================================================*/
void *uwb_ftm_diag_dispatch(void *req_pkt, uint16 pkt_len)
{
    void *rsp = NULL;
    boolean status = TRUE;

    if ((pkt_len == 0) || (req_pkt == NULL))
    {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: Incorrect uwb diag packet pkt_len:%d\n",pkt_len);
        return rsp;
    }

    DPRINTF(FTM_DBG_TRACE, "UWB FTM: uwb cmd len = %d\n",pkt_len);

    // Allocate the same length as the request.
    rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_FTM, FTM_UWB_CMD_CODE, pkt_len);

    if (rsp != NULL)
    {
        memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
    }

    /* add the uwb packet into the Cmd Queue
     * and notify the main thread its queued
    */
    status = uwb_qinsert_cmd((ftm_uwb_pkt_type *)req_pkt);
    if (status == TRUE)
    {
        sem_post(&semaphore_uwb_cmd_queued);
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb packet queued\n");
    }
    else
    {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb packet could not be queued\n");
    }
    return (rsp);
}

/*===========================================================================
FUNCTION   uwb_ftm_start

DESCRIPTION
  Processes the uwb request packet and sends it to the FTM uwb layer for further
  processing.
DEPENDENCIES
 NIL

RETURN VALUE
Nil

SIDE EFFECTS
None

===========================================================================*/
void uwb_ftm_start(void)
{
    uwb_ftm_enabled = FALSE;
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: Initialised the UWB FTM cmd queue handlers \n");

    if (sem_init(&semaphore_uwb_cmd_complete,0, 1) != 0)
    {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: semaphore_uwb_cmd_complete failed to initialise \n");
        return;
    }
    if (sem_init(&semaphore_uwb_cmd_queued,0,0) != 0)
    {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: semaphore_uwb_cmd_queued failed to initialise \n");
        return;
    }

    if (!block_uwb)
    {
        DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_FTM, uwb_ftm_diag_func_table);
        do
        {
            struct timespec ts;
            int sem_status;
            /* We have the freedom to send the first request without wating
             * for a command complete
             */
            DPRINTF(FTM_DBG_TRACE,"UWB FTM: Wait on cmd complete from the previous command \n");
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            {
                DPRINTF(FTM_DBG_TRACE,"UWB FTM: get clock_gettime error \n");
                break;
            }
            ts.tv_sec += UWB_CMD_TIME_OUT_IN_SECONDS;
            /*we wait for 5 secs for a command already queued for transmision*/
            sem_status = sem_timedwait(&semaphore_uwb_cmd_complete,&ts);
            if (sem_status == -1)
            {
                DPRINTF(FTM_DBG_TRACE,"UWB FTM: Command complete timed out \n");
                ftm_uwb_err_timedout();
            }

            DPRINTF(FTM_DBG_TRACE,"UWB FTM: Waiting on next Cmd to be queued \n");
            sem_wait(&semaphore_uwb_cmd_queued);
            uwb_dequeue_send();
        }
        while(1);
        sem_destroy(&semaphore_uwb_cmd_complete);
        sem_destroy(&semaphore_uwb_cmd_queued);
    }
    else
    {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: UWB is blocked to send any command with option 's' \n");
    }
}
#endif /*CONFIG_FTM_UWB*/

#ifdef WIN_AP_HOST
void ftm_cfg_default_file(char *Board_Name)
{
    FILE *fp = NULL;
    fp = fopen(FTM_CFG_FILE_PATH, "r");
    if (!fp) {
	DPRINTF(FTM_DBG_ERROR, "ftm.conf open failed %s or -c option is not supported for this platform.\n", FTM_CFG_FILE_PATH);
	exit(EXIT_FAILURE);
    } else {
	if (parse_ftm_cfg_file(fp, Board_Name) < 0) {
	    DPRINTF(FTM_DBG_ERROR, "No info on the Board_name present in the default %s file.\n", FTM_CFG_FILE_PATH);
	    fclose(fp);
	} else {
	    fclose(fp);
	}
    }
}
#endif

/*===========================================================================
FUNCTION   main

DESCRIPTION
  Initialises the Diag library and registers the PKT table for FM and BT
  and daemonises

DEPENDENCIES
  NIL

RETURN VALUE
  NIL, Error in the event buffer will mean a NULL App version and Zero HW
  version

SIDE EFFECTS
  None

===========================================================================*/

int main(int argc, char *argv[])
{
    int c;
    int daemonize = 1;
#ifdef WIN_AP_HOST
    FILE *fp = NULL;
    FILE *board_name_path = NULL;
    char buf[BOARD_NAME_LEN] = {0};
    char *ret = NULL;
    char *board_name = NULL;
    bool flag = false;
    char *ftm_conf_user = NULL;
#endif
    static struct option options[] =
    {
        {"help", no_argument, NULL, 'h'},
#ifdef CONFIG_FTM_WLAN
        {"interface", required_argument, NULL, 'i'},
#endif
#ifdef CONFIG_FTM_BT
        {"board-type", required_argument, NULL, 'b'},
#endif
#ifdef CONFIG_FTM_NFC
        {"firmware-download", no_argument, NULL, 'f'},
        {"nfc-test", no_argument, NULL, 't'},
#endif
#ifdef CONFIG_FTM_UWB
        {"uwb-test", no_argument, NULL, 'w'},
#endif
        {"nodaemon", no_argument, NULL, 'n'},
        {"block-bt", no_argument, NULL, 's'},
        {"semaphore-timeout",required_argument,NULL,'q'},
        {"baud-rate",required_argument,NULL,'r'},
        {"passthrough", no_argument, NULL, 'p'},
        {"uart-baudrate", no_argument, NULL, 'u'},
        {"sibs", no_argument, NULL, 's'},
#ifdef WIN_AP_HOST
        {"ftm-config", required_argument, NULL, 'c'},
#endif
        {0, 0, 0, 0}
    };
    block_bt = false;
    block_uwb = false;

    progname = argv[0];

    while (1)
    {
#if defined(ANDROID)
        c = getopt_long(argc, argv, "hdi:npb:ftws", options, NULL);
#elif defined(WIN_AP_HOST)
	c = getopt_long(argc, argv, "hdiq:npb:ftwc:", options, NULL);
#else
        c = getopt_long(argc, argv, "hdi:nb:f", options, NULL);
#endif
        if (c < 0)
            break;

        switch (c)
        {
#ifdef CONFIG_FTM_WLAN
        case 'i':
            strlcpy(g_ifname, optarg, IFNAMSIZ);
            break;
#endif
        case 'n':
            daemonize = 0;
            break;
        case 's':
            block_bt = true;
            block_uwb = true;
	    break;
	case 'q':
            user_sem_wait_timeout = atoi(optarg);

            break;
#ifdef IPQ_AP_HOST_IOT_IPQ
        case 'r':
            BAUD_RATE = atoi(optarg);
            break;
#endif
        case 'p':
            fm_passthrough = 1;
            daemonize = 0;
#ifndef WIN_AP_HOST
            property_set("wc_transport.skip_patch_dload" , "true");
#endif
            break;
        case 'd':
#ifdef DEBUG
            g_dbg_level = g_dbg_level << 1 | 0x1;
#else
            printf("Debugging disabled, please build with -DDEBUG option\n");
            exit(EXIT_FAILURE);
#endif
            break;

#ifdef CONFIG_FTM_BT
        case 'b':
            boardtype = atoi(optarg);
            break;
#endif

#ifdef CONFIG_FTM_NFC
        case 'f':
            ftm_nfc_dispatch_nq_fwdl();
            break;
        case 't':
            ftm_nfc_dispatch_nq_test( argc, argv);
            break;
#endif
#ifdef WIN_AP_HOST
        case 'c':
            flag = true;
	    ftm_conf_user = optarg;

            board_name_path = fopen(BOARD_NAME_PATH, "r");
            if (!board_name_path) {
                DPRINTF(FTM_DBG_ERROR, "Unable to open board_name path %s.\n", BOARD_NAME_PATH);
                exit(EXIT_FAILURE);
            }

            ret = fgets(buf, sizeof(buf), board_name_path);
            if (!ret) {
                DPRINTF(FTM_DBG_ERROR, "Unable to read the board_name path.\n");
                fclose(board_name_path);
                exit(EXIT_FAILURE);
            }
            buf[strlen(buf) - 1] = '\0';
            fclose(board_name_path);

            board_name = strstr(buf, "ap");
            if (!board_name) {
                board_name = strstr(buf, "db");
                if (!board_name) {
                   board_name = strstr(buf, "rdp");
                   if (!board_name) {
                       board_name = strstr(buf, "tb");
                       if (!board_name) {
                           DPRINTF(FTM_DBG_ERROR, "Unable to get board_name details.\n");
                           exit(EXIT_FAILURE);
                       }
                   }
                }
            }

	    if(ftm_conf_user != NULL)
	    {
		fp = fopen(ftm_conf_user, "r");
		if (!fp) {
		    DPRINTF(FTM_DBG_ERROR, "user input conf file %s cant be opened\n", ftm_conf_user);
		    exit(EXIT_FAILURE);
		} else {
		    if (parse_ftm_cfg_file(fp, board_name) < 0) {
			DPRINTF(FTM_DBG_ERROR, "parsing user conf file : %s failed. No valid entries for the board found. Proceeding with ftm.conf from default path %s.\n", ftm_conf_user, FTM_CFG_FILE_PATH);
			fclose(fp);
			ftm_cfg_default_file(board_name);
		    }
		}
	    } else {
		ftm_cfg_default_file(board_name);
	    }
            break;
#endif

#ifdef CONFIG_FTM_UWB
        case 'w':
            uwb_ftm_enabled = true;
            break;
#endif

        case 'h':
        default:
            usage();
            break;
        }
    }

#ifdef WIN_AP_HOST
    if (access(FTM_CFG_FILE_PATH, F_OK) == 0 && flag == false) {
        DPRINTF(FTM_DBG_ERROR, "FTM Daemon: main() failed\n");
        DPRINTF(FTM_DBG_ERROR, "-c option should be given for this platform.\n");
        usage();
        exit(EXIT_FAILURE);
    }
#endif

    if (optind < argc)
        usage();

    if (daemonize && daemon(0, 0))
    {
        perror("daemon");
        exit(EXIT_FAILURE);
    }

    DPRINTF(FTM_DBG_TRACE, "FTM Daemon calling LSM init\n");

    if (!Diag_LSM_Init(NULL))
    {
        DPRINTF(FTM_DBG_ERROR, "FTM Daemon: Diag_LSM_Init() failed\n");
        exit(EXIT_FAILURE);
    }

    DPRINTF(FTM_DBG_TRACE, "FTMDaemon: Diag_LSM_Init succesful\n");

#ifdef CONFIG_FTM_BT
#ifndef WIN_AP_HOST
    soc_type = get_bt_soc_type();
#endif
#endif

#ifdef CONFIG_FTM_FM
    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, fm_ftm_diag_func_table);
#endif

#ifdef CONFIG_FTM_WLAN
    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, wlan_ftm_diag_func_table);
#ifdef WIN_AP_HOST_OPEN
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif
#endif

#ifdef IPQ_AP_HOST_IOT
    struct sigaction ftmSigAction;
    ftmSigAction.sa_handler = ftm_signal_handler;
    sigemptyset(&ftmSigAction.sa_mask);
    ftmSigAction.sa_flags = SA_RESTART;

    sigaction(SIGINT, &ftmSigAction, NULL);
    sigaction(SIGTERM, &ftmSigAction, NULL);
#endif /* IPQ_AP_HOST_IOT */

#ifdef IPQ_AP_HOST_IOT
    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, iot_ftm_diag_func_table);
    pthread_t *iot_thr = NULL;
    iotd_desc = NULL;

#ifdef IPQ_AP_HOST_IOT_QCA402X
    iotd_desc = diag_open("/iotdiag");
#endif /* IPQ_AP_HOST_IOT_QCA402X */
#ifdef IPQ_AP_HOST_IOT_IPQ
    btss_desc = -1;
    btss_desc = btss_init();
    if(btss_desc < 0)
    {
        DPRINTF(FTM_DBG_ERROR, "FTMd : Unable to fetch BT Devnode Descriptor \n");
    }
    else
    {
        iotd_desc = (void *)(&btss_desc);
    }
#endif /* IPQ50XX, IPQ95XX */
    if (iotd_desc != NULL) {
#ifdef IPQ_AP_HOST_IOT_QCA402X
        DPRINTF(FTM_DBG_ERROR, "FTMd : FTMd has registered a handle with IOTD \n");
#endif /* IPQ_AP_HOST_IOT_QCA402X */
        sem_init(&iot_sem, 0, 0);
        sem_init(&iot_sem_async, 0, 1);

        iot_thr = (pthread_t *)malloc( sizeof(*iot_thr));
        if (!iot_thr) {
            DPRINTF(FTM_DBG_ERROR, "Could not create thread for async log packets \n");
        }
        else
        {
#ifdef IPQ_AP_HOST_IOT_QCA402X
            pthread_create(iot_thr, NULL, (void*)iot_thr_func_qca402x, (void *)iotd_desc);
#endif
#ifdef IPQ_AP_HOST_IOT_IPQ
            pthread_create(iot_thr, NULL, (void*)iot_thr_func_ipq, (void *)iotd_desc);
#endif /* IPQ50XX, IPQ95XX */
        }
    }

    if (iotd_desc == NULL)
    {
        DPRINTF(FTM_DBG_ERROR, "FTMd : IOT Daemon has not been enabled \n");
    }

#endif /* IPQ_AP_HOST_IOT */

#ifdef CONFIG_FTM_ANT
    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, ant_ftm_diag_func_table);
#endif

#ifdef CONFIG_FTM_NFC
    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, nfc_ftm_diag_func_table);
#endif

#ifdef CONFIG_FTM_UWB
    if (uwb_ftm_enabled == TRUE)
    {
        uwb_ftm_start();
    }
#endif

#ifdef CONFIG_FTM_BT
    if (!block_bt)
    {
        DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_FTM, bt_ftm_diag_func_table);
    }
    else
    {
        ALOGI("BT is blocked to send any command with option 's'");
    }
    sem_init(&semaphore_cmd_complete,0, 1);
    sem_init(&semaphore_cmd_queued,0,0);
    first_ant_command = 0;

    DPRINTF(FTM_DBG_TRACE, "Initialised the BT FTM cmd queue handlers \n");

    do
    {
        struct timespec ts;
        int sem_status;
        /* We have the freedom to send the first request without wating
         * for a command complete
         */
        DPRINTF(FTM_DBG_TRACE,
                "Wait on cmd complete from the previous command\n");
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            printf("get clock_gettime error");
        ts.tv_sec += 5;
        /*we wait for 5 secs for a command already queued for
         * transmision
         */
        sem_status = sem_timedwait(&semaphore_cmd_complete,&ts);
        if(sem_status == -1)
        {
            printf("Command complete timed out\n");
            ftm_bt_err_timedout();
        }

        DPRINTF(FTM_DBG_TRACE, "Waiting on next Cmd to be queued\n");

        sem_wait(&semaphore_cmd_queued);
        dequeue_send();
    }
    while(1);
#else /* CONFIG_FTM_BT */
#ifdef CONFIG_FTM_FM
    pthread_cond_init(&fm_event_cond, NULL);
    pthread_mutex_init(&fm_event_lock, NULL);
#endif
    while (1);
#endif

    DPRINTF(FTM_DBG_TRACE, "\nFTMDaemon Deinit the LSM\n");
#ifdef CONFIG_FTM_FM
    pthread_cond_destroy(&fm_event_cond);
    pthread_mutex_destroy(&fm_event_lock);
#endif

#ifdef IPQ_AP_HOST_IOT
    if (iotd_desc != NULL) {
        thread_stop = 1;
        pthread_join(*iot_thr, NULL);
        free(iot_thr);
#ifdef IPQ_AP_HOST_IOT_IPQ
        btss_deinit();
#endif/* IPQ50XX, IPQ95XX */
    }
#endif /* IPQ_AP_HOST_IOT */
#ifdef CONFIG_FTM_BT
    hidl_client_close();
#endif
    /* Clean up before exiting */
    Diag_LSM_DeInit();

    exit(EXIT_SUCCESS);
}
