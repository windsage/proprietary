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
 *  Filename:      hw_rome.c
 *
 *  Description:   Contains controller-specific functions, like
 *                      firmware patch download
 *                      low power mode operations
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "bt_vendor"

#include <sys/socket.h>
//#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
//#include <cutils/properties.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
//#include "bt_hci_bdroid.h"
//#include "bt_vendor_qcom.h"
#include "hci_uart.h"
#include "hw_rome.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/select.h>
#include <byteswap.h>
#include <poll.h>

boolean print_enabled = 0;

boolean    unified_hci= TRUE;


#define BT_VERSION_FILEPATH "/data/misc/bluedroid/bt_fw_version.txt"

#ifdef __cplusplus
}
#endif

#define RESERVED(p)  if(p) ALOGI( "%s: reserved param", __FUNCTION__);

int read_vs_hci_event(int fd, unsigned char* buf, int size);

/******************************************************************************
**  Variables
******************************************************************************/
FILE *file;
unsigned char *phdr_buffer;
unsigned char *pdata_buffer = NULL;
patch_info rampatch_patch_info;
int chipset_ver = 0;
unsigned char gTlv_type;
unsigned char gTlv_dwndCfg;
static unsigned int wipower_flag = 0;
static unsigned int wipower_handoff_ready = 0;
char *rampatch_file_path = NULL;
char *nvm_file_path = NULL;
char *mbn_file_path = NULL;
int enable_scans = 0;
char *fw_su_info = NULL;
unsigned short fw_su_offset =0;
extern char enable_extldo;
unsigned char wait_vsc_evt = TRUE;
boolean patch_dnld_pending = FALSE;
int dnld_fd = -1;

/******************************************************************************
**  Extern variables
******************************************************************************/

/*****************************************************************************
**   Functions
*****************************************************************************/
int do_write(int fd, unsigned char *buf,int len)
{
    int ret = 0;
    int write_offset = 0;
    int write_len = len;
    int j = 0;

    ALOGI("%s\n", __func__);

    ALOGI("Command packet being sent to BT: ");
    for (j=0 ; j< len ; j++)
        ALOGI("0x%x ", buf[j]);
    ALOGI("\n");

    do {
        ret = write(fd,buf+write_offset,write_len);
        if (ret < 0)
        {
            ALOGE("%s, write failed ret = %d err = %s",__func__,ret,strerror(errno));
            return -1;
        } else if (ret == 0) {
            ALOGE("%s, write failed with ret 0 err = %s",__func__,strerror(errno));
            return 0;
        } else {
            if (ret < write_len) {
                ALOGD("%s, Write pending,do write ret = %d err = %s",__func__,ret,
                       strerror(errno));
                write_len = write_len - ret;
                write_offset = ret;
            } else {
                ALOGV("Write successful");
                break;
            }
        }
    } while(1);
    return len;
}


int HandleEdlCmdResEvt(unsigned char subOpcode, unsigned char paramlen,
  unsigned char* rsp)
{
  int err = 0;
  unsigned int soc_id = 0;
  unsigned int productid = 0;
  unsigned short patchversion = 0;
  unsigned short buildversion = 0;
  unsigned int bld_len_offset = 5;
  char build_label[255];
  int build_lbl_len;
  struct timeval tv;
  boolean ret  = false;
  int build_label_prefix;

  switch (subOpcode) {
    case EDL_PATCH_VER_RES_EVT:
    case EDL_APP_VER_RES_EVT:
    {
        productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 3] << 24 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 2] << 16 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 1] << 8 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED]  );
        ALOGI("\t unified Current Product ID\t\t: 0x%08x", productid);

        /* Patch Version indicates FW patch version */
        patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET_UNIFIED + 1] << 8 |
                                             rsp[PATCH_PATCH_VER_OFFSET_UNIFIED] );
        ALOGI("\t unified Current Patch Version\t\t: 0x%04x", patchversion);

        /* ROM Build Version indicates ROM build version like 1.0/1.1/2.0 */
        buildversion =
              (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED + 1] << 8 |
                    rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED] );
        ALOGI("\t unified Current ROM Build Version\t: 0x%04x", buildversion);

        if (paramlen - 10) {
          soc_id =
                (unsigned int)(rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 3] << 24 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 2] << 16 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 1] << 8 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED]  );
          ALOGI("\t unified Current SOC Version\t\t: 0x%08x", soc_id);
          char soc_id_info[255];
          snprintf(soc_id_info, sizeof(soc_id_info),
            "unified Current SOC Version : 0x%08x", soc_id);

        }
      }
      chipset_ver = QCA_BT_VER(soc_id, productid, buildversion);
        ALOGI("\t chipset_ver\t\t: 0x%08x", chipset_ver);

      break;

    case EDL_TVL_DNLD_RES_EVT:
    case EDL_CMD_EXE_STATUS_EVT:
    /* In case of unified HCI cmd, rsp will contain cmd subopcode in this case 0x1E */
    case EDL_PATCH_TLV_REQ_CMD:
      if (unified_hci)
        err = rsp[CMD_STATUS_OFFSET_UNIFIED];
      else
        err = rsp[CMD_STATUS_OFFSET];

      break;
    case EDL_GET_BUILD_INFO:
    case HCI_VS_GET_BUILD_VER_EVT:
      if (unified_hci) {
        bld_len_offset += 3;
      }
      build_lbl_len = rsp[bld_len_offset];
      snprintf(build_label, sizeof(build_label), "BT SoC FW SU Build info: ");
      build_label_prefix = strlen(build_label);
      memcpy(build_label + build_label_prefix, &rsp[bld_len_offset + 1], build_lbl_len);
      *(build_label + build_label_prefix + build_lbl_len) = '\0';

      ALOGI("%s: %s, %d", __func__, build_label, build_lbl_len);

      break;

  }
}

int get_vs_hci_event(unsigned char *rsp)
{
    int err = 0;

    unsigned char EMBEDDED_MODE_CHECK = 0x02;
    FILE *btversionfile = 0;
    unsigned int soc_id = 0;
    unsigned int productid = 0;
    unsigned short patchversion = 0;
    char build_label[255];
    int build_lbl_len;
    unsigned short buildversion = 0;
    unsigned char paramlen = 0;

    unsigned int opcode = 0;
    unsigned char subOpcode = 0;
    unsigned int ocf = 0;
    unsigned int ogf = 0;
    unsigned char status = 0;
    uint8_t baudrate_rsp_status_offset = 0;
    uint8_t addon_features_bitmask_offset = 0;

    if( (rsp[EVENTCODE_OFFSET] == VSEVENT_CODE) || (rsp[EVENTCODE_OFFSET] == EVT_CMD_COMPLETE))
        ALOGI("%s: Received HCI-Vendor Specific event", __FUNCTION__);
    else {
        ALOGI("%s: Failed to receive HCI-Vendor Specific event", __FUNCTION__);
        err = -EIO;
        goto failed;
    }

    paramlen = rsp[EVT_PLEN];
    opcode = rsp[5]<<8 | rsp[4];
    ocf = opcode & 0x03ff;
    ogf = opcode >> 10;
    status = rsp[6];
    subOpcode = rsp[7];
    ALOGV("%s: Opcode: 0x%x", __func__, opcode);
    ALOGV("%s: ocf: 0x%x", __func__, ocf);
    ALOGV("%s: ogf: 0x%x", __func__, ogf);
    ALOGV("%s: Status: 0x%x", __func__, status);
    ALOGV("%s: Sub-Opcode: 0x%x", __func__, subOpcode);
    ALOGV("%s: Parameter Length: 0x%x", __func__, paramlen);

    /* Check the status of the operation */
    switch ( ocf ) {
      case EDL_CMD_REQ_RES_EVT:
        ALOGV("%s: Command Request Response", __func__);
        HandleEdlCmdResEvt(subOpcode, paramlen, rsp);
        break;
      case NVM_ACCESS_CODE:
        ALOGI("%s: NVM Access Code!!!", __func__);
        err = HCI_CMD_SUCCESS;
        break;
      case EDL_SET_BAUDRATE_RSP_EVT:
        baudrate_rsp_status_offset = BAUDRATE_RSP_STATUS_OFFSET;
      /* in case of unified cmd, rsp contains cmd opcode */
      case EDL_SET_BAUDRATE_CMD_OCF:
        /* incase of unified hci we have different offset for baudrate status */
        if (unified_hci)
          baudrate_rsp_status_offset = BAUDRATE_RSP_STATUS_OFFSET_UNIFIED;
        if (rsp[baudrate_rsp_status_offset] != BAUDRATE_CHANGE_SUCCESS) {
          ALOGE("%s: Set Baudrate request failed with status: 0x%x",
                __func__, rsp[baudrate_rsp_status_offset]);
          err = -1;
        }
        break;

      default:
        ALOGE("%s: Not a valid status!!!", __func__);
        err = -1;
        break;
    }



failed:
    return err;
}


/*
 * Read an VS HCI event from the given file descriptor.
 */
int read_vs_hci_event(int fd, unsigned char* buf, int size)
{
    int remain, r;
    int count = 0, i;

    if (size <= 0) {
        ALOGE("Invalid size arguement!");
        return -1;
    }

    ALOGI("%s: Wait for HCI-Vendor Specfic Event from SOC", __FUNCTION__);

    /* The first byte identifies the packet type. For HCI event packets, it
     * should be 0x04, so we read until we get to the 0x04. */
    /* It will keep reading until find 0x04 byte */
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
            if (buf[0] == 0x04)
                    break;
    }
    count++;

    /* The next two bytes are the event code and parameter total length. */
    while (count < 3) {
            r = read(fd, buf + count, 3 - count);
            if ((r <= 0) || ((buf[1] != 0xFF ) && (buf[1] != EVT_CMD_COMPLETE ))) {
                ALOGE("It is not VS event or command complete event !! ret: %d, EVT: %d", r, buf[1]);
                return -1;
            }
            count += r;
    }


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

     /* Check if the set patch command is successful or not */
    if(get_vs_hci_event(buf) != HCI_CMD_SUCCESS)
        return -1;

    return count;
}

int hci_send_vs_cmd(int fd, unsigned char *cmd, unsigned char *rsp, int size)
{
    int ret = 0;

    /* Send the HCI command packet to UART for transmission */
    ret = do_write(fd, cmd, size);
    if (ret != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, ret);
        goto failed;
    }

    if (wait_vsc_evt) {
        /* Check for response from the Controller */
        if (read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE) < 0) {
           ret = -ETIMEDOUT;
           ALOGI("%s: Failed to get HCI-VS Event from SOC", __FUNCTION__);
           goto failed;
        }
        ALOGI("%s: Received HCI-Vendor Specific Event from SOC", __FUNCTION__);
    }

failed:
    return ret;
}

void frame_hci_cmd_pkt(
    unsigned char *cmd,
    int edl_cmd, unsigned int p_base_addr,
    int segtNo, int size
    )
{
    int offset = 0;
    hci_command_hdr *cmd_hdr;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);

    cmd[0]      = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
    cmd_hdr->plen   = size;
    cmd[4]      = edl_cmd;

    switch (edl_cmd)
    {
        case EDL_PATCH_SET_REQ_CMD:
            /* Copy the patch header info as CMD params */
            memcpy(&cmd[5], phdr_buffer, PATCH_HDR_LEN);
            ALOGD("%s: Sending EDL_PATCH_SET_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
                segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_DLD_REQ_CMD:
            offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
            p_base_addr += offset;
            cmd_hdr->plen   = (size + 6);
            cmd[5]  = (size + 4);
            cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
            cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
            cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
            cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
            memcpy(&cmd[10], (pdata_buffer + offset), size);

            ALOGD("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
                __FUNCTION__, size);
            ALOGD("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
                "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
                cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
            break;
        case EDL_PATCH_ATCH_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_RST_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_RESET_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_VER_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_VER_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_TLV_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_TLV_REQ_CMD", __FUNCTION__);
            /* Parameter Total Length */
            cmd[3] = size +2;

            /* TLV Segment Length */
            cmd[5] = size;
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
            offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
            memcpy(&cmd[6], (pdata_buffer + offset), size);
            break;
        case EDL_GET_BUILD_INFO:
            ALOGD("%s: Sending EDL_GET_BUILD_INFO", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
                segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        default:
            ALOGE("%s: Unknown EDL CMD !!!", __FUNCTION__);
    }
}

void rome_extract_patch_header_info(unsigned char *buf)
{
    int index;

    /* Extract patch id */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_id |=
            (LSH(buf[index + P_ID_OFFSET], (index * 8)));

    /* Extract (ROM and BUILD) version information */
    for (index = 0; index < 2; index++)
        rampatch_patch_info.patch_ver.rom_version |=
            (LSH(buf[index + P_ROME_VER_OFFSET], (index * 8)));

    for (index = 0; index < 2; index++)
        rampatch_patch_info.patch_ver.build_version |=
            (LSH(buf[index + P_BUILD_VER_OFFSET], (index * 8)));

    /* Extract patch base and entry addresses */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_base_addr |=
            (LSH(buf[index + P_BASE_ADDR_OFFSET], (index * 8)));

    /* Patch BASE & ENTRY addresses are same */
    rampatch_patch_info.patch_entry_addr = rampatch_patch_info.patch_base_addr;

    /* Extract total length of the patch payload */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_length |=
            (LSH(buf[index + P_LEN_OFFSET], (index * 8)));

    /* Extract the CRC checksum of the patch payload */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_crc |=
            (LSH(buf[index + P_CRC_OFFSET], (index * 8)));

    /* Extract patch control value */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_ctrl |=
            (LSH(buf[index + P_CONTROL_OFFSET], (index * 8)));

    ALOGI("PATCH_ID\t : 0x%x", rampatch_patch_info.patch_id);
    ALOGI("ROM_VERSION\t : 0x%x", rampatch_patch_info.patch_ver.rom_version);
    ALOGI("BUILD_VERSION\t : 0x%x", rampatch_patch_info.patch_ver.build_version);
    ALOGI("PATCH_LENGTH\t : 0x%x", rampatch_patch_info.patch_length);
    ALOGI("PATCH_CRC\t : 0x%x", rampatch_patch_info.patch_crc);
    ALOGI("PATCH_CONTROL\t : 0x%x\n", rampatch_patch_info.patch_ctrl);
    ALOGI("PATCH_BASE_ADDR\t : 0x%x\n", rampatch_patch_info.patch_base_addr);

}

int rome_edl_set_patch_request(int fd)
{
    int size, err;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_SET_REQ_CMD, 0,
        -1, PATCH_HDR_LEN + 1);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to set the patch info to the Controller!");
        goto error;
    }

    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }
    ALOGI("%s: Successfully set patch info on the Controller", __FUNCTION__);
error:
    return err;
}

int rome_edl_patch_download_request(int fd)
{
    int no_of_patch_segment;
    int index = 1, err = 0, size = 0;
    unsigned int p_base_addr;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    no_of_patch_segment = (rampatch_patch_info.patch_length /
        MAX_DATA_PER_SEGMENT);
    ALOGI("%s: %d patch segments to be d'loaded from patch base addr: 0x%x",
        __FUNCTION__, no_of_patch_segment,
    rampatch_patch_info.patch_base_addr);

    /* Initialize the patch base address from the one read from bin file */
    p_base_addr = rampatch_patch_info.patch_base_addr;

    /*
    * Depending upon size of the patch payload, download the patches in
    * segments with a max. size of 239 bytes
    */
    for (index = 1; index <= no_of_patch_segment; index++) {

        ALOGI("%s: Downloading patch segment: %d", __FUNCTION__, index);

        /* Frame the HCI CMD PKT to be sent to Controller*/
        frame_hci_cmd_pkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr,
        index, MAX_DATA_PER_SEGMENT);

        /* Total length of the packet to be sent to the Controller */
        size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

        /* Initialize the RSP packet everytime to 0 */
        memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

        /* Send HCI Command packet to Controller */
        err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
        if ( err != size) {
            ALOGE("Failed to send the patch payload to the Controller!");
            goto error;
        }

        /* Read Command Complete Event */
        err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",
            __FUNCTION__, index);
            goto error;
        }
        ALOGI("%s: Successfully downloaded patch segment: %d",
        __FUNCTION__, index);
    }

    /* Check if any pending patch data to be sent */
    size = (rampatch_patch_info.patch_length < MAX_DATA_PER_SEGMENT) ?
        rampatch_patch_info.patch_length :
        (rampatch_patch_info.patch_length  % MAX_DATA_PER_SEGMENT);

    if (size)
    {
        /* Frame the HCI CMD PKT to be sent to Controller*/
        frame_hci_cmd_pkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr, index, size);

        /* Initialize the RSP packet everytime to 0 */
        memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

        /* Total length of the packet to be sent to the Controller */
        size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

        /* Send HCI Command packet to Controller */
        err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
        if ( err != size) {
            ALOGE("Failed to send the patch payload to the Controller!");
            goto error;
        }

        /* Read Command Complete Event */
        err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",
                __FUNCTION__, index);
            goto error;
        }

        ALOGI("%s: Successfully downloaded patch segment: %d",
        __FUNCTION__, index);
    }

error:
    return err;
}

int rome_attach_rampatch(int fd)
{
    int size, err;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_ATCH_REQ_CMD, 0,
        -1, EDL_PATCH_CMD_LEN);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to attach the patch payload to the Controller!");
        goto error;
    }

    /* Read Command Complete Event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to attach the patch segment(s)", __FUNCTION__);
        goto error;
    }
error:
    return err;
}

int rome_rampatch_reset(int fd)
{
    int size, err = 0, flags;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    struct timespec tm = { 0, 100*1000*1000 }; /* 100 ms */

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_RST_REQ_CMD, 0,
                                        -1, EDL_PATCH_CMD_LEN);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

    /* Send HCI Command packet to Controller */
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        goto error;
    }

    /*
    * Controller doesn't sends any response for the patch reset
    * command. HOST has to wait for 100ms before proceeding.
    */
    nanosleep(&tm, NULL);

error:
    return err;
}

/* This function is called with q_lock held and q is non-NULL */
int rome_get_tlv_file(char *file_path)
{
    FILE * pFile;
    long fileSize;
    int readSize, err = 0, total_segment, remain_size, nvm_length, nvm_index, i;
    unsigned short nvm_tag_len;
    tlv_patch_info *ptlv_header;
    tlv_nvm_hdr *nvm_ptr;
    unsigned char data_buf[PRINT_BUF_SIZE]={0,};
    unsigned char *nvm_byte_ptr;

    ALOGI("File Open (%s)", file_path);
    pFile = fopen ( file_path , "r" );
    if (pFile==NULL) {;
        ALOGE("%s File Open Fail", file_path);
        return -1;
    }

    /* Get File Size */
    fseek (pFile , 0 , SEEK_END);
    fileSize = ftell (pFile);
    rewind (pFile);

    pdata_buffer = (unsigned char*) malloc (sizeof(char)*fileSize);
    if (pdata_buffer == NULL) {
        ALOGE("Allocated Memory failed");
        fclose (pFile);
        return -1;
    }

    /* Copy file into allocated buffer */
    readSize = fread (pdata_buffer,1,fileSize,pFile);

    /* File Close */
    fclose (pFile);

    if (readSize != fileSize) {
        ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)",readSize,fileSize);
        return -1;
    }

    ptlv_header = (tlv_patch_info *) pdata_buffer;

    /* To handle different event between rampatch and NVM */
    gTlv_type = ptlv_header->tlv_type;
    gTlv_dwndCfg = ptlv_header->tlv.patch.dwnd_cfg;

    if(ptlv_header->tlv_type == TLV_TYPE_PATCH){
        ALOGI("\n====================================================");
        ALOGI("\nTLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
        ALOGI("\nLength\t\t\t : %d bytes", (ptlv_header->tlv_length1) |
                                                    (ptlv_header->tlv_length2 << 8) |
                                                    (ptlv_header->tlv_length3 << 16));
        ALOGI("\nTotal Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_data_len);
        ALOGI("\nPatch Data Length\t\t\t : %d bytes",ptlv_header->tlv.patch.tlv_patch_data_len);
        ALOGI("\nSigning Format Version\t : 0x%x", ptlv_header->tlv.patch.sign_ver);
        ALOGI("\nSignature Algorithm\t\t : 0x%x", ptlv_header->tlv.patch.sign_algorithm);
        ALOGI("\nEvent Handling\t\t\t : 0x%x", ptlv_header->tlv.patch.dwnd_cfg);
        ALOGI("\nReserved\t\t\t : 0x%x", ptlv_header->tlv.patch.reserved1);
        ALOGI("\nProduct ID\t\t\t : 0x%04x\n", ptlv_header->tlv.patch.prod_id);
        ALOGI("\nRom Build Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.build_ver);
        ALOGI("\nPatch Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.patch_ver);
        ALOGI("\nReserved\t\t\t : 0x%x\n", ptlv_header->tlv.patch.reserved2);
        ALOGI("\nPatch Entry Address\t\t : 0x%x\n", (ptlv_header->tlv.patch.patch_entry_addr));
        ALOGI("\n====================================================");

    } else if(ptlv_header->tlv_type == TLV_TYPE_NVM) {
        ALOGI("\n====================================================");
        ALOGI("\nTLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
        ALOGI("\nLength\t\t\t : %d bytes",  nvm_length = (ptlv_header->tlv_length1) |
                                                    (ptlv_header->tlv_length2 << 8) |
                                                    (ptlv_header->tlv_length3 << 16));

        if(nvm_length <= 0)
            return readSize;

       for(nvm_byte_ptr=(unsigned char *)(nvm_ptr = &(ptlv_header->tlv.nvm)), nvm_index=0;
             nvm_index < nvm_length ; nvm_ptr = (tlv_nvm_hdr *) nvm_byte_ptr)
       {
            ALOGI("\nTAG ID\t\t\t : %d", nvm_ptr->tag_id);
            ALOGI("\nTAG Length\t\t\t : %d", nvm_tag_len = nvm_ptr->tag_len);
            ALOGI("\nTAG Pointer\t\t\t : %d", nvm_ptr->tag_ptr);
            ALOGI("\nTAG Extended Flag\t\t : %d", nvm_ptr->tag_ex_flag);

            /* Increase nvm_index to NVM data */
            nvm_index+=sizeof(tlv_nvm_hdr);
            nvm_byte_ptr+=sizeof(tlv_nvm_hdr);

            /* Write BD Address */
            /*if(nvm_ptr->tag_id == TAG_NUM_2){
                memcpy(nvm_byte_ptr, q->bdaddr, 6);
                ALOGI("BD Address: %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",
                    *nvm_byte_ptr, *(nvm_byte_ptr+1), *(nvm_byte_ptr+2),
                    *(nvm_byte_ptr+3), *(nvm_byte_ptr+4), *(nvm_byte_ptr+5));
            }*/

            for(i =0;(i<nvm_ptr->tag_len && (i*3 + 2) <PRINT_BUF_SIZE);i++)
                snprintf((char *) data_buf, PRINT_BUF_SIZE, "%s%.02x ", (char *)data_buf, *(nvm_byte_ptr + i));

            ALOGI("\nTAG Data\t\t\t : %s", data_buf);

            /* Clear buffer */
            memset(data_buf, 0x0, PRINT_BUF_SIZE);

            /* increased by tag_len */
            nvm_index+=nvm_ptr->tag_len;
            nvm_byte_ptr +=nvm_ptr->tag_len;
        }

        ALOGI("\n====================================================");

    } else {
        ALOGI("\nTLV Header type is unknown (%d) ", ptlv_header->tlv_type);
        gTlv_type = TLV_TYPE_PATCH;
        gTlv_dwndCfg = ROME_SKIP_EVT_VSE_CC;
    }

    return readSize;
}

int rome_tlv_dnld_segment(int fd, int index, int seg_size, unsigned char wait_cc_evt)
{
    int size=0, err = -1;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    ALOGI("%s: Downloading TLV Patch segment no.%d, size:%d", __FUNCTION__, index, seg_size);

    /* Frame the HCI CMD PKT to be sent to Controller*/
    frame_hci_cmd_pkt(cmd, EDL_PATCH_TLV_REQ_CMD, 0, index, seg_size);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Initialize the RSP packet everytime to 0 */
    memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send the patch payload to the Controller! 0x%x", err);
        return err;
    }

    if(wait_cc_evt) {
        err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",  __FUNCTION__, index);
            return err;
        }
    }

    ALOGI("%s: Successfully downloaded patch segment: %d", __FUNCTION__, index);
    return err;
}

int rome_tlv_dnld_req(int fd, int tlv_size)
{
    int  total_segment, remain_size, i, err = -1;
    unsigned char wait_cc_evt = TRUE;

    total_segment = tlv_size/MAX_SIZE_PER_TLV_SEGMENT;
    remain_size = (tlv_size < MAX_SIZE_PER_TLV_SEGMENT)?\
        tlv_size: (tlv_size%MAX_SIZE_PER_TLV_SEGMENT);

    ALOGI("%s: TLV size: %d, Total Seg num: %d, remain size: %d",
        __FUNCTION__,tlv_size, total_segment, remain_size);

    if (gTlv_type == TLV_TYPE_PATCH) {
       /* Prior to Rome version 3.2(including inital few rampatch release of Rome 3.2), the event
        * handling mechanism is ROME_SKIP_EVT_NONE. After few release of rampatch for Rome 3.2, the
        * mechamism is changed to ROME_SKIP_EVT_VSE_CC. Rest of the mechanism is not used for now
        */
       switch(gTlv_dwndCfg)
       {
           case ROME_SKIP_EVT_NONE:
              wait_vsc_evt = TRUE;
              wait_cc_evt = TRUE;
              ALOGI("Event handling type: ROME_SKIP_EVT_NONE");
              break;
           case ROME_SKIP_EVT_VSE_CC:
              wait_vsc_evt = FALSE;
              wait_cc_evt = FALSE;
              ALOGI("Event handling type: ROME_SKIP_EVT_VSE_CC");
              break;
           /* Not handled for now */
           case ROME_SKIP_EVT_VSE:
           case ROME_SKIP_EVT_CC:
           default:
              ALOGE("Unsupported Event handling: %d", gTlv_dwndCfg);
              break;
       }
    } else {
        wait_vsc_evt = TRUE;
        wait_cc_evt = FALSE;
    }

    for(i=0;i<total_segment ;i++){
        if ((i+1) == total_segment) {
             if ((chipset_ver >= ROME_VER_1_1) && (chipset_ver < ROME_VER_3_2) && (gTlv_type == TLV_TYPE_PATCH)) {
               /* If the Rome version is from 1.1 to 3.1
                * 1. No CCE for the last command segment but all other segment
                * 2. All the command segments get VSE including the last one
                */
                wait_cc_evt = !remain_size ? FALSE: TRUE;
             } else if ((chipset_ver >= ROME_VER_3_2) && (gTlv_type == TLV_TYPE_PATCH)) {
                /* If the Rome version is 3.2
                 * 1. None of the command segments receive CCE
                 * 2. No command segments receive VSE except the last one
                 * 3. If gTlv_dwndCfg is ROME_SKIP_EVT_NONE then the logic is
                 *    same as Rome 2.1, 2.2, 3.0
                 */
                 if (gTlv_dwndCfg == ROME_SKIP_EVT_NONE) {
                    wait_cc_evt = !remain_size ? FALSE: TRUE;
                 } else if (gTlv_dwndCfg == ROME_SKIP_EVT_VSE_CC) {
                    wait_vsc_evt = !remain_size ? TRUE: FALSE;
                 }
             }
        }

        patch_dnld_pending = TRUE;
        if((err = rome_tlv_dnld_segment(fd, i, MAX_SIZE_PER_TLV_SEGMENT, wait_cc_evt )) < 0)
            goto error;
        patch_dnld_pending = FALSE;
    }

    if ((chipset_ver >= ROME_VER_1_1) && (chipset_ver < ROME_VER_3_2) && (gTlv_type == TLV_TYPE_PATCH)) {
       /* If the Rome version is from 1.1 to 3.1
        * 1. No CCE for the last command segment but all other segment
        * 2. All the command segments get VSE including the last one
        */
        wait_cc_evt = remain_size ? FALSE: TRUE;
    } else if ((chipset_ver >= ROME_VER_3_2) && (gTlv_type == TLV_TYPE_PATCH)) {
        /* If the Rome version is 3.2
         * 1. None of the command segments receive CCE
         * 2. No command segments receive VSE except the last one
         * 3. If gTlv_dwndCfg is ROME_SKIP_EVT_NONE then the logic is
         *    same as Rome 2.1, 2.2, 3.0
         */
        if (gTlv_dwndCfg == ROME_SKIP_EVT_NONE) {
           wait_cc_evt = remain_size ? FALSE: TRUE;
        } else if (gTlv_dwndCfg == ROME_SKIP_EVT_VSE_CC) {
           wait_vsc_evt = remain_size ? TRUE: FALSE;
        }
    }
    patch_dnld_pending = TRUE;
    if(remain_size) err =rome_tlv_dnld_segment(fd, i, remain_size, wait_cc_evt);
    patch_dnld_pending = FALSE;
error:
    if(patch_dnld_pending) patch_dnld_pending = FALSE;
    return err;
}

int rome_download_tlv_file(int fd)
{
    int tlv_size, err = -1;

    /* Rampatch TLV file Downloading */
    pdata_buffer = NULL;
#if 0
        if((tlv_size = rome_get_tlv_file(mbn_file_path)) < 0)
            goto error;

        if((err =rome_tlv_dnld_req(fd, tlv_size)) <0 )
            goto error;

        if (pdata_buffer != NULL){
            free (pdata_buffer);
            pdata_buffer = NULL;
        }
#endif

#if 1
    if((tlv_size = rome_get_tlv_file(rampatch_file_path)) < 0)
        goto error;

    if((err =rome_tlv_dnld_req(fd, tlv_size)) <0 )
        goto error;

    if (pdata_buffer != NULL){
        free (pdata_buffer);
        pdata_buffer = NULL;
    }
#endif
nvm_download:
    if(!nvm_file_path) {
        ALOGI("%s: nvm file is not available", __FUNCTION__);
        err = 0; // in case of nvm/rampatch is not available
        goto error;
    }

   /* NVM TLV file Downloading */
    if((tlv_size = rome_get_tlv_file(nvm_file_path)) <= 0)
        goto error;

    if((err =rome_tlv_dnld_req(fd, tlv_size)) <0 )
        goto error;

error:
    if (pdata_buffer != NULL)
        free (pdata_buffer);
    // Sleep for 500 ms to give SoC time to come back to normal mode.
    usleep(1000 * 500);

    return err;
}

int rome_1_0_nvm_tag_dnld(int fd)
{
    int i, size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

#if (NVM_VERSION >= ROME_1_0_100019)
    unsigned char cmds[MAX_TAG_CMD][HCI_MAX_CMD_SIZE] =
    {
        /* Tag 2 */ /* BD Address */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     9,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     2,
            /* Tag Len */      6,
            /* Tag Value */   0x77,0x78,0x23,0x01,0x56,0x22
         },
        /* Tag 6 */ /* Bluetooth Support Features */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     11,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     6,
            /* Tag Len */      8,
            /* Tag Value */   0xFF,0xFE,0x8B,0xFE,0xD8,0x3F,0x5B,0x8B
         },
        /* Tag 17 */ /* HCI Transport Layer Setting */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     11,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     17,
            /* Tag Len */      8,
            /* Tag Value */   0x82,0x01,0x0E,0x08,0x04,0x32,0x0A,0x00
         },
        /* Tag 35 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     58,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     35,
            /* Tag Len */      55,
            /* Tag Value */   0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x58, 0x59,
                                      0x0E, 0x0E, 0x16, 0x16, 0x16, 0x1E, 0x26, 0x5F, 0x2F, 0x5F,
                                      0x0E, 0x0E, 0x16, 0x16, 0x16, 0x1E, 0x26, 0x5F, 0x2F, 0x5F,
                                      0x0C, 0x18, 0x14, 0x24, 0x40, 0x4C, 0x70, 0x80, 0x80, 0x80,
                                      0x0C, 0x18, 0x14, 0x24, 0x40, 0x4C, 0x70, 0x80, 0x80, 0x80,
                                      0x1B, 0x14, 0x01, 0x04, 0x48
         },
        /* Tag 36 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     15,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     36,
            /* Tag Len */      12,
            /* Tag Value */   0x0F,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x04,0x00
         },
        /* Tag 39 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     7,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     39,
            /* Tag Len */      4,
            /* Tag Value */   0x12,0x00,0x00,0x00
         },
        /* Tag 41 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     91,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     41,
            /* Tag Len */      88,
            /* Tag Value */   0x15, 0x00, 0x00, 0x00, 0xF6, 0x02, 0x00, 0x00, 0x76, 0x00,
                                      0x1E, 0x00, 0x29, 0x02, 0x1F, 0x00, 0x61, 0x00, 0x1A, 0x00,
                                      0x76, 0x00, 0x1E, 0x00, 0x7D, 0x00, 0x40, 0x00, 0x91, 0x00,
                                      0x06, 0x00, 0x92, 0x00, 0x03, 0x00, 0xA6, 0x01, 0x50, 0x00,
                                      0xAA, 0x01, 0x15, 0x00, 0xAB, 0x01, 0x0A, 0x00, 0xAC, 0x01,
                                      0x00, 0x00, 0xB0, 0x01, 0xC5, 0x00, 0xB3, 0x01, 0x03, 0x00,
                                      0xB4, 0x01, 0x13, 0x00, 0xB5, 0x01, 0x0C, 0x00, 0xC5, 0x01,
                                      0x0D, 0x00, 0xC6, 0x01, 0x10, 0x00, 0xCA, 0x01, 0x2B, 0x00,
                                      0xCB, 0x01, 0x5F, 0x00, 0xCC, 0x01, 0x48, 0x00
         },
        /* Tag 42 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     63,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     42,
            /* Tag Len */      60,
            /* Tag Value */   0xD7, 0xC0, 0x00, 0x00, 0x8F, 0x5C, 0x02, 0x00, 0x80, 0x47,
                                      0x60, 0x0C, 0x70, 0x4C, 0x00, 0x00, 0x00, 0x01, 0x1F, 0x01,
                                      0x42, 0x01, 0x69, 0x01, 0x95, 0x01, 0xC7, 0x01, 0xFE, 0x01,
                                      0x3D, 0x02, 0x83, 0x02, 0xD1, 0x02, 0x29, 0x03, 0x00, 0x0A,
                                      0x10, 0x00, 0x1F, 0x00, 0x3F, 0x00, 0x7F, 0x00, 0xFD, 0x00,
                                      0xF9, 0x01, 0xF1, 0x03, 0xDE, 0x07, 0x00, 0x00, 0x9A, 0x01
         },
        /* Tag 84 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     153,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     84,
            /* Tag Len */      150,
            /* Tag Value */   0x7C, 0x6A, 0x59, 0x47, 0x19, 0x36, 0x35, 0x25, 0x25, 0x28,
                                      0x2C, 0x2B, 0x2B, 0x28, 0x2C, 0x28, 0x29, 0x28, 0x29, 0x28,
                                      0x29, 0x29, 0x2C, 0x29, 0x2C, 0x29, 0x2C, 0x28, 0x29, 0x28,
                                      0x29, 0x28, 0x29, 0x2A, 0x00, 0x00, 0x2C, 0x2A, 0x2C, 0x18,
                                      0x98, 0x98, 0x98, 0x98, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
                                      0x1E, 0x13, 0x1E, 0x1E, 0x1E, 0x1E, 0x13, 0x13, 0x11, 0x13,
                                      0x1E, 0x1E, 0x13, 0x12, 0x12, 0x12, 0x11, 0x12, 0x1F, 0x12,
                                      0x12, 0x12, 0x10, 0x0C, 0x18, 0x0D, 0x01, 0x01, 0x01, 0x01,
                                      0x01, 0x01, 0x01, 0x0C, 0x01, 0x01, 0x01, 0x01, 0x0D, 0x0D,
                                      0x0E, 0x0D, 0x01, 0x01, 0x0D, 0x0D, 0x0D, 0x0D, 0x0F, 0x0D,
                                      0x10, 0x0D, 0x0D, 0x0D, 0x0D, 0x10, 0x05, 0x10, 0x03, 0x00,
                                      0x7E, 0x7B, 0x7B, 0x72, 0x71, 0x50, 0x50, 0x50, 0x00, 0x40,
                                      0x60, 0x60, 0x30, 0x08, 0x02, 0x0F, 0x00, 0x01, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x08, 0x16, 0x16, 0x08, 0x08, 0x00,
                                      0x00, 0x00, 0x1E, 0x34, 0x2B, 0x1B, 0x23, 0x2B, 0x15, 0x0D
         },
        /* Tag 85 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     119,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     85,
            /* Tag Len */      116,
            /* Tag Value */   0x03, 0x00, 0x38, 0x00, 0x45, 0x77, 0x00, 0xE8, 0x00, 0x59,
                                      0x01, 0xCA, 0x01, 0x3B, 0x02, 0xAC, 0x02, 0x1D, 0x03, 0x8E,
                                      0x03, 0x00, 0x89, 0x01, 0x0E, 0x02, 0x5C, 0x02, 0xD7, 0x02,
                                      0xF8, 0x08, 0x01, 0x00, 0x1F, 0x00, 0x0A, 0x02, 0x55, 0x02,
                                      0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x2A, 0xD7, 0x00, 0x00,
                                      0x00, 0x1E, 0xDE, 0x00, 0x00, 0x00, 0x14, 0x0F, 0x0A, 0x0F,
                                      0x0A, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x04, 0x04, 0x0C, 0x0C,
                                      0x0C, 0x0C, 0x06, 0x06, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02,
                                      0x01, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00,
                                      0x06, 0x0F, 0x14, 0x05, 0x47, 0xCF, 0x77, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0xAC, 0x7C, 0xFF, 0x40, 0x00, 0x00, 0x00,
                                      0x12, 0x04, 0x04, 0x01, 0x04, 0x03
         },
        {TAG_END}
    };
#elif (NVM_VERSION == ROME_1_0_6002)
    unsigned char cmds[MAX_TAG_CMD][HCI_MAX_CMD_SIZE] =
    {
        /* Tag 2 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     9,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     2,
            /* Tag Len */      6,
            /* Tag Value */   0x77,0x78,0x23,0x01,0x56,0x22 /* BD Address */
         },
        /* Tag 6 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     11,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     6,
            /* Tag Len */      8,
            /* Tag Value */   0xFF,0xFE,0x8B,0xFE,0xD8,0x3F,0x5B,0x8B
         },
        /* Tag 17 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     11,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     17,
            /* Tag Len */      8,
            /* Tag Value */   0x82,0x01,0x0E,0x08,0x04,0x32,0x0A,0x00
         },
        /* Tag 36 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     15,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     36,
            /* Tag Len */      12,
            /* Tag Value */   0x0F,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x04,0x00
         },

        /* Tag 39 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     7,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     39,
            /* Tag Len */      4,
            /* Tag Value */   0x12,0x00,0x00,0x00
         },

        /* Tag 41 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     199,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     41,
            /* Tag Len */      196,
            /* Tag Value */   0x30,0x00,0x00,0x00,0xD5,0x00,0x0E,0x00,0xD6,0x00,0x0E,0x00,
                                      0xD7,0x00,0x16,0x00,0xD8,0x00,0x16,0x00,0xD9,0x00,0x16,0x00,
                                      0xDA,0x00,0x1E,0x00,0xDB,0x00,0x26,0x00,0xDC,0x00,0x5F,0x00,
                                      0xDD,0x00,0x2F,0x00,0xDE,0x00,0x5F,0x00,0xE0,0x00,0x0E,0x00,
                                      0xE1,0x00,0x0E,0x00,0xE2,0x00,0x16,0x00,0xE3,0x00,0x16,0x00,
                                      0xE4,0x00,0x16,0x00,0xE5,0x00,0x1E,0x00,0xE6,0x00,0x26,0x00,
                                      0xE7,0x00,0x5F,0x00,0xE8,0x00,0x2F,0x00,0xE9,0x00,0x5F,0x00,
                                      0xEC,0x00,0x0C,0x00,0xED,0x00,0x08,0x00,0xEE,0x00,0x14,0x00,
                                      0xEF,0x00,0x24,0x00,0xF0,0x00,0x40,0x00,0xF1,0x00,0x4C,0x00,
                                      0xF2,0x00,0x70,0x00,0xF3,0x00,0x80,0x00,0xF4,0x00,0x80,0x00,
                                      0xF5,0x00,0x80,0x00,0xF8,0x00,0x0C,0x00,0xF9,0x00,0x18,0x00,
                                      0xFA,0x00,0x14,0x00,0xFB,0x00,0x24,0x00,0xFC,0x00,0x40,0x00,
                                      0xFD,0x00,0x4C,0x00,0xFE,0x00,0x70,0x00,0xFF,0x00,0x80,0x00,
                                      0x00,0x01,0x80,0x00,0x01,0x01,0x80,0x00,0x04,0x01,0x1B,0x00,
                                      0x05,0x01,0x14,0x00,0x06,0x01,0x01,0x00,0x07,0x01,0x04,0x00,
                                      0x08,0x01,0x00,0x00,0x09,0x01,0x00,0x00,0x0A,0x01,0x03,0x00,
                                      0x0B,0x01,0x03,0x00
         },

        /* Tag 44 */
        {  /* Packet Type */HCI_COMMAND_PKT,
            /* Opcode */       0x0b,0xfc,
            /* Total Len */     44,
            /* NVM CMD */    NVM_ACCESS_SET,
            /* Tag Num */     44,
            /* Tag Len */      41,
            /* Tag Value */   0x6F,0x0A,0x00,0x00,0x00,0x00,0x00,0x50,0xFF,0x10,0x02,0x02,
                                      0x01,0x00,0x14,0x01,0x06,0x28,0xA0,0x62,0x03,0x64,0x01,0x01,
                                      0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0xFF,0x10,0x02,0x01,
                                      0x00,0x14,0x01,0x02,0x03
         },
        {TAG_END}
    };
#endif

    ALOGI("%s: Start sending NVM Tags (ver: 0x%x)", __FUNCTION__, (unsigned int) NVM_VERSION);

    for (i=0; (i < MAX_TAG_CMD) && (cmds[i][0] != TAG_END); i++)
    {
        /* Write BD Address */
        /*if(cmds[i][TAG_NUM_OFFSET] == TAG_NUM_2){
            memcpy(&cmds[i][TAG_BDADDR_OFFSET], q->bdaddr, 6);
            ALOGI("BD Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                cmds[i][TAG_BDADDR_OFFSET ], cmds[i][TAG_BDADDR_OFFSET + 1],
                cmds[i][TAG_BDADDR_OFFSET + 2], cmds[i][TAG_BDADDR_OFFSET + 3],
                cmds[i][TAG_BDADDR_OFFSET + 4], cmds[i][TAG_BDADDR_OFFSET + 5]);
        }*/
        size = cmds[i][3] + HCI_COMMAND_HDR_SIZE + 1;
        /* Send HCI Command packet to Controller */
        err = hci_send_vs_cmd(fd, (unsigned char *)&cmds[i][0], rsp, size);
        if ( err != size) {
            ALOGE("Failed to attach the patch payload to the Controller!");
            goto error;
        }

        /* Read Command Complete Event - This is extra routine for ROME 1.0. From ROM 2.0, it should be removed. */
        err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to get patch version(s)", __FUNCTION__);
            goto error;
        }
    }

error:
    return err;
}



int rome_patch_ver_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_VER_REQ_CMD, 0,
    -1, EDL_PATCH_CMD_LEN);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to attach the patch payload to the Controller!");
        goto error;
    }

error:
    return err;

}

int rome_set_baudrate_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_SET_BAUDRATE_CMD_OCF);
    cmd_hdr->plen     = VSC_SET_BAUDRATE_REQ_LEN;
    cmd[4]  = BAUDRATE_3000000;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + VSC_SET_BAUDRATE_REQ_LEN);

    /* Flow off during baudrate change */
    if ((err = userial_vendor_ioctl(USERIAL_OP_FLOW_OFF , &flags)) < 0)
    {
      ALOGE("%s: HW Flow-off error: 0x%x\n", __FUNCTION__, err);
      goto error;
    }

    /* Send the HCI command packet to UART for transmission */
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        goto error;
    }

    /* Change Local UART baudrate to high speed UART */
    userial_vendor_set_baud(USERIAL_BAUD_3M);

    /* Flow on after changing local uart baudrate */
    if ((err = userial_vendor_ioctl(USERIAL_OP_FLOW_ON , &flags)) < 0)
    {
        ALOGE("%s: HW Flow-on error: 0x%x \n", __FUNCTION__, err);
        return err;
    }

    /* Check for response from the Controller */
    if ((err =read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE)) < 0) {
            ALOGE("%s: Failed to get HCI-VS Event from SOC", __FUNCTION__);
            goto error;
    }

    ALOGI("%s: Received HCI-Vendor Specific Event from SOC", __FUNCTION__);

#if 0
    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }
#endif
error:
    return err;

}


int rome_hci_reset_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI RESET ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_RESET;
    cmd_hdr->plen   = 0;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);

    /* Flow off during baudrate change */
    if ((err = userial_vendor_ioctl(USERIAL_OP_FLOW_OFF , &flags)) < 0)
    {
      ALOGE("%s: HW Flow-off error: 0x%x\n", __FUNCTION__, err);
      goto error;
    }

    /* Send the HCI command packet to UART for transmission */
    ALOGI("%s: HCI CMD: 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, cmd[0], cmd[1], cmd[2], cmd[3]);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        goto error;
    }

    /* Change Local UART baudrate to high speed UART */
    userial_vendor_set_baud(USERIAL_BAUD_3M);

    /* Flow on after changing local uart baudrate */
    if ((err = userial_vendor_ioctl(USERIAL_OP_FLOW_ON , &flags)) < 0)
    {
        ALOGE("%s: HW Flow-on error: 0x%x \n", __FUNCTION__, err);
        return err;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}


int rome_hci_reset(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI RESET ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_RESET;
    cmd_hdr->plen   = 0;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int rome_hci_read_local_version(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI Read local version ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_READ_LOCAL_VER_INFO;
    cmd_hdr->plen   = 0;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to read local version from Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int rome_hci_read_local_bd_addr(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI Read local BD Address ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_READ_BD_ADDR;
    cmd_hdr->plen   = 0;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to read local bd address from Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int rome_hci_write_le_scan_enable(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI write LE scan enable ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_ULP_EXT_SCAN_ENABLE;
    cmd_hdr->plen   = 6;
    cmd[4]  = 0x01;
    cmd[5]  = 0x00;
    cmd[6]  = 0x00;
    cmd[7]  = 0x00;
    cmd[8]  = 0x00;
    cmd[9]  = 0x00;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + 6);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to enable scan in Controller", __FUNCTION__);
        goto error;
    }


error:
    return err;

}

int rome_hci_write_le_scan_disable(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI write LE scan disable ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_ULP_EXT_SCAN_ENABLE;
    cmd_hdr->plen   = 6;
    cmd[4]  = 0x00;
    cmd[5]  = 0x00;
    cmd[6]  = 0x00;
    cmd[7]  = 0x00;
    cmd[8]  = 0x00;
    cmd[9]  = 0x00;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + 6);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to disable scan in Controller", __FUNCTION__);
        goto error;
    }


error:
    return err;

}

int rome_hci_write_le_scan_params(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI write LE scan enable ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_ULP_EXT_SCAN_SET_PARAMS;
    cmd_hdr->plen   = 8;
    cmd[4]  = 0x00;
    cmd[5]  = 0x00;
    cmd[6]  = 0x01;
    cmd[7]  = 0x00;
    cmd[8]  = 0x10;
    cmd[9]  = 0x00;
    cmd[10]  = 0x10;
    cmd[11]  = 0x00;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + 8);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to enable scan in Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int rome_hci_write_scan_enable(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI write scan enable ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_WRITE_SCAN_ENABLE;
    cmd_hdr->plen   = 1;
    cmd[4]  = 0x03;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + 1);
    err = do_write(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to enable scan in Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int EdlModeChange(int fd) {
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  hci_command_hdr *cmd_hdr;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = 0xfc69;
  cmd_hdr->plen   = 0x01;
  cmd[4]      = 00;
  /* Total length of the packet to be sent to the Controller */
  size = 5;

  ALOGD("%s: Sending change mode CMD to SOC for going to normal mode", __func__);

  /* Send HCI Command packet to Controller */
  err = do_write(fd, cmd, size);
  if (err != size) {
      ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
      err = -1;
      goto error;
  }
  /* Wait for command complete event */
  err = read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
      ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
      goto error;
  }

  error:
      return err;

  // Sleep for 500 ms to give SoC time to come back to normal mode.
  usleep(1000 * 500);
}
#define PATH_MAX    50

extern char patchFileLoc[PATH_MAX];
extern char nvmFileLoc[PATH_MAX];
extern char mbnFileLoc[PATH_MAX];

int rome_soc_init(int fd, char *bdaddr)
{
    int err = -1, size = 0;
    dnld_fd = fd;
    ALOGI(" %s ", __FUNCTION__);
    RESERVED(bdaddr);
    EdlModeChange(fd);
    /* Send HCI Reset */
    err = rome_hci_reset(fd);
    if ( err <0 ) {
        ALOGE("HCI Reset Failed !!");
        goto error;
    }

download:
#if 0
            /* Change baud rate 115.2 kbps to 3Mbps*/
            err = rome_set_baudrate_req(fd);
            if (err < 0) {
                ALOGE("%s: Baud rate change failed!", __FUNCTION__);
                goto error;
            }
            ALOGI("%s: Baud rate changed successfully ", __FUNCTION__);
#endif



            rampatch_file_path = patchFileLoc;
            nvm_file_path = nvmFileLoc;
            mbn_file_path = mbnFileLoc;
            chipset_ver = ROME_VER_3_2;
            gTlv_dwndCfg = ROME_SKIP_EVT_VSE_CC;

            /* Donwload TLV files (rampatch, NVM) */
            err = rome_download_tlv_file(fd);
            if (err < 0) {
                ALOGE("%s: Download TLV file failed!", __FUNCTION__);
                goto error;
            }
            ALOGI("%s: Download TLV file successfully ", __FUNCTION__);

            /* Send HCI Reset */
            err = rome_hci_reset(fd);
            if ( err <0 ) {
                ALOGE("HCI Reset Failed !!");
                goto error;
            }

            ALOGI("HCI Reset is done\n");

            err = rome_hci_read_local_version(fd);
            if ( err <0 ) {
                ALOGE("HCI Read Local Version Failed !!");
                goto error;
            }

            ALOGI("HCI Read Local Version is done\n");
            err = rome_hci_read_local_bd_addr(fd);
            if ( err <0 ) {
                ALOGE("HCI Read local BD address Failed !!");
                goto error;
            }

            ALOGI("HCI Read local BD address is done\n");

#if 0
            err = rome_hci_write_scan_enable(fd);
            if ( err <0 ) {
                ALOGE("HCI Write scan enable Failed !!");
                goto error;
            }

            ALOGI("HCI Write scan enable is done\n");
#endif

            if(enable_scans)
            {
                err = rome_hci_write_le_scan_params(fd);
                if ( err <0 ) {
                    ALOGE("HCI set LE scan params Failed !!");
                    goto error;
                }

                printf("HCI set LE scan params is done\n");

                err = rome_hci_write_le_scan_enable(fd);
                if ( err <0 ) {
                    ALOGE("HCI set LE scan enable Failed !!");
                    goto error;
                }
                printf("HCI set LE scan enable is done\n");

                // Sleep for 5000 ms
                printf("Wait for 5s\n");
                usleep(1000 * 5000);
                printf("5s wait done. Disable scan now.\n");
                err = rome_hci_write_le_scan_disable(fd);
                if ( err <0 ) {
                    ALOGE("HCI set LE scan disable Failed !!");
                    goto error;
                }
                printf("HCI set LE scan disable is done\n");
            }

#if 0
            break;
        default:
            ALOGI("%s: Detected unknown SoC version: 0x%08x", __FUNCTION__, chipset_ver);
            err = -1;
            break;
    }
#endif
error:
    dnld_fd = -1;
    return err;
}

