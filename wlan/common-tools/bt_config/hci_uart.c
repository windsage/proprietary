/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Copyright 2012 The Android Open Source Project
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

/******************************************************************************
 *
 *  Filename:      userial_vendor.c
 *
 *  Description:   Contains vendor-specific userial functions
 *
 ******************************************************************************/

#define LOG_TAG "bt_vendor"

//#include <utils/Log.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
//#include "bt_vendor_qcom.h"
#include "hci_uart.h"
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/limits.h>
#include <sys/select.h>
#include <byteswap.h>
#include <poll.h>
#include "hw_rome.h"
/******************************************************************************
**  Constants & Macros
******************************************************************************/
#ifndef VNDUSERIAL_DBG
#define VNDUSERIAL_DBG TRUE
#endif

#if (VNDUSERIAL_DBG == TRUE)
#define VNDUSERIALDBG(param, ...) {ALOGI(param, ## __VA_ARGS__);}
#else
#define VNDUSERIALDBG(param, ...) {}
#endif

#define RESERVED(p)  if(p) ALOGI( "%s: reserved param", __FUNCTION__);

/******************************************************************************
**  Global variables
******************************************************************************/
vnd_userial_cb_t vnd_userial;

/*****************************************************************************
**   Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        userial_to_tcio_baud
**
** Description     helper function converts USERIAL baud rates into TCIO
**                  conforming baud rates
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud)
{
    if (cfg_baud == USERIAL_BAUD_115200)
        *baud = B115200;
    else if (cfg_baud == USERIAL_BAUD_4M)
        *baud = B4000000;
    else if (cfg_baud == USERIAL_BAUD_3M)
        *baud = B3000000;
    else if (cfg_baud == USERIAL_BAUD_2M)
        *baud = B2000000;
    else if (cfg_baud == USERIAL_BAUD_1M)
        *baud = B1000000;
    else if (cfg_baud == USERIAL_BAUD_921600)
        *baud = B921600;
    else if (cfg_baud == USERIAL_BAUD_460800)
        *baud = B460800;
    else if (cfg_baud == USERIAL_BAUD_230400)
        *baud = B230400;
    else if (cfg_baud == USERIAL_BAUD_57600)
        *baud = B57600;
    else if (cfg_baud == USERIAL_BAUD_19200)
        *baud = B19200;
    else if (cfg_baud == USERIAL_BAUD_9600)
        *baud = B9600;
    else if (cfg_baud == USERIAL_BAUD_1200)
        *baud = B1200;
    else if (cfg_baud == USERIAL_BAUD_600)
        *baud = B600;
    else
    {
        ALOGE( "userial vendor open: unsupported baud idx %i", cfg_baud);
        *baud = B115200;
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function        userial_to_baud_tcio
**
** Description     helper function converts TCIO baud rate into integer
**
** Returns         uint32_t
**
*******************************************************************************/
int userial_tcio_baud_to_int(uint32_t baud)
{
    int baud_rate =0;

    switch (baud)
    {
        case B600:
            baud_rate = 600;
            break;
        case B1200:
            baud_rate = 1200;
            break;
        case B9600:
            baud_rate = 9600;
            break;
        case B19200:
            baud_rate = 19200;
            break;
        case B57600:
            baud_rate = 57600;
            break;
        case B115200:
            baud_rate = 115200;
            break;
        case B230400:
            baud_rate = 230400;
            break;
        case B460800:
            baud_rate = 460800;
            break;
        case B921600:
            baud_rate = 921600;
            break;
        case B1000000:
            baud_rate = 1000000;
            break;
        case B2000000:
            baud_rate = 2000000;
            break;
        case B3000000:
            baud_rate = 3000000;
            break;
        case B4000000:
            baud_rate = 4000000;
            break;
        default:
            ALOGE( "%s: unsupported baud %d", __FUNCTION__, baud);
            break;
    }

    ALOGI( "%s: Current Baudrate = %d bps", __FUNCTION__, baud_rate);
    return baud_rate;
}


#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
/*******************************************************************************
**
** Function        userial_ioctl_init_bt_wake
**
** Description     helper function to set the open state of the bt_wake if ioctl
**                  is used. it should not hurt in the rfkill case but it might
**                  be better to compile it out.
**
** Returns         none
**
*******************************************************************************/
void userial_ioctl_init_bt_wake(int fd)
{
    uint32_t bt_wake_state;

    /* assert BT_WAKE through ioctl */
    ioctl(fd, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
    ioctl(fd, USERIAL_IOCTL_BT_WAKE_GET_ST, &bt_wake_state);
    VNDUSERIALDBG("userial_ioctl_init_bt_wake read back BT_WAKE state=%i", \
               bt_wake_state);
}
#endif // (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)


/*****************************************************************************
**   Userial Vendor API Functions
*****************************************************************************/


/*******************************************************************************
**
** Function        userial_vendor_set_baud
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_baud(uint8_t userial_baud)
{
    uint32_t tcio_baud;

    VNDUSERIALDBG("## userial_vendor_set_baud: %d", userial_baud);

    userial_to_tcio_baud(userial_baud, &tcio_baud);

    cfsetospeed(&vnd_userial.termios, tcio_baud);
    cfsetispeed(&vnd_userial.termios, tcio_baud);
    tcsetattr(vnd_userial.fd, TCSADRAIN, &vnd_userial.termios); /* don't change speed until last write done */
//    tcflush(vnd_userial.fd, TCIOFLUSH);
}

/*******************************************************************************
**
** Function        userial_vendor_get_baud
**
** Description     Get current baud rate
**
** Returns         int
**
*******************************************************************************/
int userial_vendor_get_baud(void)
{
    if (vnd_userial.fd == -1)
    {
        ALOGE( "%s: uart port(%s) has not been opened", __FUNCTION__, BT_HS_UART_DEVICE );
        return -1;
    }

    return userial_tcio_baud_to_int(cfgetispeed(&vnd_userial.termios));
}

/*******************************************************************************
**
** Function        userial_vendor_ioctl
**
** Description     ioctl inteface
**
** Returns         None
**
*******************************************************************************/
int userial_vendor_ioctl(userial_vendor_ioctl_op_t op, int *p_data)
{
    int err = -1;

    switch(op)
    {
#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
        case USERIAL_OP_ASSERT_BT_WAKE:
            VNDUSERIALDBG("## userial_vendor_ioctl: Asserting BT_Wake ##");
            err = ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
            break;

        case USERIAL_OP_DEASSERT_BT_WAKE:
            VNDUSERIALDBG("## userial_vendor_ioctl: De-asserting BT_Wake ##");
            err = ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_DEASSERT, NULL);
            break;

        case USERIAL_OP_GET_BT_WAKE_STATE:
            err = ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_GET_ST, p_data);
            break;
#endif  //  (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
        case USERIAL_OP_FLOW_ON:
            ALOGI("## userial_vendor_ioctl: UART Flow On ");
            *p_data |=TIOCM_RTS;
            err = ioctl(vnd_userial.fd, TIOCMSET, p_data);
            break;

        case USERIAL_OP_FLOW_OFF:
            ALOGI("## userial_vendor_ioctl: UART Flow Off ");
            ioctl(vnd_userial.fd, TIOCMGET, p_data);
            *p_data &= ~TIOCM_RTS;
            err = ioctl(vnd_userial.fd, TIOCMSET, p_data);
            break;

        default:
            break;
    }

    return err;
}


/*******************************************************************************
**
** Function        read_hci_event
**
** Description     Read HCI event during vendor initialization
**
** Returns         int: size to read
**
*******************************************************************************/
int read_hci_event(int fd, unsigned char* buf, int size)
{
    int remain, r;
    int count = 0;
    int tot_len;
    unsigned char* tmp_buf = NULL;

    if (size <= 0) {
        ALOGE("Invalid size arguement!");
        return -1;
    }

    ALOGI("%s: Wait for Command Compete Event from SOC", __FUNCTION__);

    /* The first byte identifies the packet type. For HCI event packets, it
     * should be 0x04, so we read until we get to the 0x04. */
    while (1) {
            r = read(fd, buf, 1);
            if (r <= 0)
            {
                            ALOGI("%s: Nothing received over UART. Wait and try again.", __FUNCTION__);
                            usleep(1000 * 500);
                            r = read(fd, buf, 1);
                            if (r <= 0)
                            {
                                return -1;
                            }
                        }
            else
                {
                    ALOGI("0x%x ", buf[0]);
                }

            if (buf[0] == 0x04)
                    break;
            else if (buf[0] == LOG_BT_ACL_PACKET_TYPE)
            {
                /* Check and parse ACL data if received during init phase to prevent init failures */
                r = read(fd,buf, BT_ACL_HDR_SIZE);
                if (r <= 0)
                {
                    ALOGE("%s: read error", __func__);
                    return -1;
                }

                PTR_TO_UINT16(tot_len, &buf[BT_ACL_HDR_LEN_OFFSET]);
                tmp_buf = (unsigned char *)malloc(sizeof(unsigned char) *(tot_len + BT_ACL_HDR_SIZE));
                if (tmp_buf == NULL) {
                    ALOGE("%s: Heap memory allocation failed ", __func__);
                    return -1;
                  }
                memcpy(tmp_buf, buf, BT_ACL_HDR_SIZE);
                r = read(fd, tmp_buf + BT_ACL_HDR_SIZE, tot_len);
                if (r < 0) {
                    ALOGE("%s: read error", __func__);
                    if (tmp_buf != NULL) {
              free(tmp_buf);
              tmp_buf = NULL;
            }
            return -1;
          }

          ALOGD("%s: Number of bytes of ACL data(header + payload) read = %d",
                __func__, tot_len + BT_ACL_HDR_SIZE);


        } else {
          ALOGI("%s: Got an invalid proto byte: %d", __func__, buf[0]);
        }

    }
    count++;
    ALOGI("%s: event code", __FUNCTION__);

    /* The next two bytes are the event code and parameter total length. */
    while (count < 3) {
            r = read(fd, buf + count, 3 - count);
            if (r <= 0)
                    return -1;
            count += r;
    }
    ALOGI("%s: params", __FUNCTION__);

    /* Now we read the parameters. */
    if (buf[2] < (size - 3))
            remain = buf[2];
    else
            remain = size - 3;

    while ((count - 3) < remain) {
            r = read(fd, buf + count, remain - (count - 3));
            if (r <= 0)
                    return -1;
            count += r;
    }

    {
        int j = 0;
        printf("\nread_vs_hci_event: Event received: ");
        for (j=0 ; j< count ; j++)
            printf("0x%x ", buf[j]);
        printf("\n");
    }

    return count;
}

#if 0
int userial_clock_operation(int fd, int cmd)
{
    int ret = 0;

    switch (cmd)
    {
        case USERIAL_OP_CLK_ON:
        case USERIAL_OP_CLK_OFF:
             ioctl(fd, cmd);
             break;
        case USERIAL_OP_CLK_STATE:
             ret = ioctl(fd, cmd);
             break;
    }

    return ret;
}
#endif
