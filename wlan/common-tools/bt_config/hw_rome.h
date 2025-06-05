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

#ifndef HW_ROME_H
#define HW_ROME_H


/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define HCI_MAX_CMD_SIZE        260
#define HCI_MAX_EVENT_SIZE     260
#define PRINT_BUF_SIZE              ((HCI_MAX_CMD_SIZE * 3) + 2)

#define HCI_CHG_BAUD_CMD_OCF        0x0C
#define HCI_VENDOR_CMD_OGF             0x3F
#define WRITE_BDADDR_CMD_LEN        14
#define WRITE_BAUD_CMD_LEN             6
#define MAX_CMD_LEN                    WRITE_BDADDR_CMD_LEN
#define GET_VERSION_OCF            0x1E

#define PS_HDR_LEN                         4
#define HCI_VENDOR_CMD_OGF      0x3F
#define HCI_PS_CMD_OCF                0x0B

#define HCI_COMMAND_HDR_SIZE        3
#define EVT_CMD_COMPLETE_SIZE       3
#define EVT_CMD_STATUS                     0x0F
#define EVT_CMD_STATUS_SIZE           4
#define HCI_EVENT_HDR_SIZE              2
#define HCI_EV_SUCCESS                      0x00
/* HCI Socket options */
#define HCI_DATA_DIR            1
#define HCI_FILTER                  2
#define HCI_TIME_STAMP        3

#define P_ID_OFFSET                                     (0)
#define HCI_CMD_IND                                   (1)
#define EVENTCODE_OFFSET                      (1)
#define EVT_PLEN                                             (2)
#define PLEN                                                       (3)
#define CMD_RSP_OFFSET                             (3)
#define RSP_TYPE_OFFSET                            (4)
#define BAUDRATE_RSP_STATUS_OFFSET    (4)
#define CMD_STATUS_OFFSET                      (5)
#define P_ROME_VER_OFFSET                       (4)
#define P_BUILD_VER_OFFSET                      (6)
#define P_BASE_ADDR_OFFSET                     (8)
#define P_ENTRY_ADDR_OFFSET                   (12)
#define P_LEN_OFFSET                                   (16)
#define P_CRC_OFFSET                                  (20)
#define P_CONTROL_OFFSET                          (24)
#define PATCH_HDR_LEN                               (28)
#define MAX_DATA_PER_SEGMENT                (239)
#define VSEVENT_CODE                                 (0xFF)
#define HC_VS_MAX_CMD_EVENT                 (0xFF)
#define PATCH_PROD_ID_OFFSET                (5)
#define PATCH_PATCH_VER_OFFSET            (9)
#define PATCH_ROM_BUILD_VER_OFFSET       (11)
#define PATCH_SOC_VER_OFFSET             (13)
#define MAX_SIZE_PER_TLV_SEGMENT        (243)
/* Unified VSC offset*/
#define PATCH_PROD_ID_OFFSET_UNIFIED         (9)
#define PATCH_PATCH_VER_OFFSET_UNIFIED       (13)
#define PATCH_ROM_BUILD_VER_OFFSET_UNIFIED   (15)
#define PATCH_SOC_VER_OFFSET_UNIFIED         (17)
#define BAUDRATE_RSP_STATUS_OFFSET_UNIFIED   (6)
#define CMD_STATUS_OFFSET_UNIFIED            (6)
#define UNIFIED_HCI_CODE                     (0x01)
#define UNIFIED_HCI_CC_MIN_LENGTH            (6)
#define HCI_VS_WIPOWER_CMD_OPCODE            (0xFC1F)
#define HCI_VS_GET_VER_CMD_OPCODE            (0xFC00)

/* chipset id(64 bit)= (((Soc ID) << 32) | ((Product ID &0xffff) <<16)
                        | (Build version)& 0xffff)
    Create a 64bit unique chipset ver by storing 2 bytes of build version in LSB
    , next to it store 2 bytes of product id and 4 bytes SOC ID in MSB then ORing
    them for uniqueness */

 #define QCA_BT_VER(s, p, b) (((uint64_t)(s) << 32) | ((uint64_t)(p & 0xffff) << 16) \
                             | ((uint64_t)(b & 0xffff)))

/* VS Opcode */
#define HCI_PATCH_CMD_OCF                       (0)
#define EDL_SET_BAUDRATE_CMD_OCF        (0x48)
#define EDL_WIPOWER_VS_CMD_OCF          (0x1f)
#define HCI_VS_GET_ADDON_FEATURES_SUPPORT   (0x1d)

/* VS Commands */
#define VSC_SET_BAUDRATE_REQ_LEN        (1)
#define EDL_PATCH_CMD_LEN	                       (1)
#define EDL_PATCH_CMD_REQ_LEN               (1)
#define EDL_WIP_QUERY_CHARGING_STATUS_LEN   (0x01)
#define EDL_WIP_START_HANDOFF_TO_HOST_LEN   (0x01)
#define EDL_PATCH_DLD_REQ_CMD               (0x01)
#define EDL_PATCH_RST_REQ_CMD               (0x05)
#define EDL_PATCH_SET_REQ_CMD               (0x16)
#define EDL_PATCH_ATCH_REQ_CMD            (0x17)
#define EDL_PATCH_VER_REQ_CMD               (0x19)
#define EDL_GET_BUILD_INFO                  (0x20)
#define EDL_PATCH_TLV_REQ_CMD               (0x1E)
#define EDL_WIP_QUERY_CHARGING_STATUS_CMD   (0x1D)
#define EDL_WIP_START_HANDOFF_TO_HOST_CMD   (0x1E)

/* VS Event */
#define EDL_CMD_REQ_RES_EVT                 (0x00)
#define EDL_CMD_EXE_STATUS_EVT           (0x00)
#define EDL_SET_BAUDRATE_RSP_EVT       (0x92)
#define EDL_PATCH_VER_RES_EVT             (0x19)
#define EDL_TVL_DNLD_RES_EVT                (0x04)
#define EDL_APP_VER_RES_EVT                  (0x02)
#define EDL_WIP_QUERY_CHARGING_STATUS_EVT    (0x18)
#define EDL_WIP_START_HANDOFF_TO_HOST_EVENT  (0x19)
#define HCI_VS_GET_ADDON_FEATURES_EVENT      (0x1B)
#define HCI_VS_GET_BUILD_VER_EVT             (0x05)
#define HCI_VS_STRAY_EVT                (0x17)

/* Status Codes of HCI CMD execution*/
#define HCI_CMD_SUCCESS                     (0x0)
#define PATCH_LEN_ERROR                       (0x1)
#define PATCH_VER_ERROR                       (0x2)
#define PATCH_CRC_ERROR                     (0x3)
#define PATCH_NOT_FOUND                      (0x4)
#define TLV_TYPE_ERROR                         (0x10)
#define NVM_ACCESS_CODE                     (0x0B)
#define BAUDRATE_CHANGE_SUCCESS   (1)

/* Wipower status codes */
#define WIPOWER_IN_EMBEDDED_MODE 0x01
#define NON_WIPOWER_MODE 0x02

/* mask to validate support for wipower */
#define ADDON_FEATURES_EVT_WIPOWER_MASK      (0x01)

/* TLV_TYPE */
#define TLV_TYPE_PATCH                  (1)
#define TLV_TYPE_NVM                      (2)

/* NVM */
#define MAX_TAG_CMD                 30
#define TAG_END                           0xFF
#define NVM_ACCESS_SET            0x01
#define TAG_NUM_OFFSET             5
#define TAG_NUM_2                       2
#define TAG_BDADDR_OFFSET     7

/* NVM Tags specifically used for ROME 1.0 */
#define ROME_1_0_100022_1       0x101000221
#define ROME_1_0_100019           0x101000190
#define ROME_1_0_6002               0x100600200

/* Default NVM Version setting for ROME 1.0 */
#define NVM_VERSION                  ROME_1_0_100022_1


#define LSH(val, n)     ((uint32_t)(val) << (n))
#define EXTRACT_BYTE(val, pos)      (char) (((val) >> (8 * (pos))) & 0xFF)
#define CALC_SEG_SIZE(len, max)   ((plen) % (max))?((plen/max)+1) : ((plen) / (max))

#define ROME_3_1_FW_SU  "bprm.cnss.3.1"
#define ROME_3_2_FW_SU  "btfwp.cnss.3.2"

/* Rome 3.1 FW SU release has been branched from rome 3.0 SU 224
    So, rome 3.1 formula is SU = patch version -(0xE0 = 224) - 0x111 -1
*/
#define ROME_3_1_FW_SW_OFFSET   0x01F2

/* Rome 3.2 FW SU formula is SU = patch version - 0x111 -1 */
#define ROME_3_2_FW_SW_OFFSET   0x0112

/* This header value in rampatch file decides event handling mechanism in the HOST */
#define ROME_SKIP_EVT_NONE     0x00
#define ROME_SKIP_EVT_VSE      0x01
#define ROME_SKIP_EVT_CC       0x02
#define ROME_SKIP_EVT_VSE_CC   0x03

/* Packet types */
#define LOG_BT_CMD_PACKET_TYPE     0x01
#define LOG_BT_ACL_PACKET_TYPE     0x02
#define LOG_BT_SCO_PACKET_TYPE     0x03
#define LOG_BT_EVT_PACKET_TYPE     0x04
#define LOG_ANT_CTL_PACKET_TYPE    0x0c
#define LOG_ANT_DATA_PACKET_TYPE   0x0e
#define LOG_BT_EVT_VENDOR_SPECIFIC 0xFF
#define LOG_BT_EVT_CMD_CMPLT 0x0E
#define BT_ACL_HDR_SIZE 4
#define BT_ACL_HDR_LEN_OFFSET 2

#define PTR_TO_UINT16(u16, p) ((u16) = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)))

/* Message type of the log from controller */
#define LOG_BT_CONTROLLER_LOG        0x01
#define LOG_BT_MESSAGE_TYPE_VSTR     0x02
#define LOG_BT_MESSAGE_TYPE_PACKET   0x05
#define LOG_BT_MESSAGE_TYPE_MEM_DUMP 0x08
#define LOG_BT_MESSAGE_TYPE_HW_ERR   0x09
#define LOG_BT_HCI_BUFFER_DUMP       0x0A

/* Offset related to different crash dumps */
#define MINI_DUMP_ARM_REG_SIZE_OFFSET   10
#define MINI_DUMP_CALL_STK_SIZE_OFFSET  87
#define HCI_BUFFER_DUMP_SIZE_OFFSET     4
#define DUMP_PKT_SIZE_OFFSET            1

#define MAX_BQR_VS_PARAMS_SIZE          251
#define BQR_RIE_VS_PARAM_COUNT_OFFSET   6
#define BQR_RIE_VS_ERR_CODE_OFFSET      5
#define BQR_RIE_VSP_META_DATA_SIZE      2
#define MAX_BQR_RIE_PKT_SIZE            257

/* Sub log ID for the message type PACKET */
#define LOG_BT_HCI_CMD   0
#define LOG_BT_HCI_EVENT 1

#define LOG_BT_RX_LMP_PDU      18
#define LOG_BT_TX_LMP_PDU      19
#define LOG_BT_RX_LE_CTRL_PDU  20
#define LOG_BT_TX_LE_CTRL_PDU  21
#define LOG_BT_TX_LE_CONN_MNGR 22

#define LOG_BT_LINK_MANAGER_STATE    0x80
#define LOG_BT_CONN_MANAGER_STATE    0x81
#define LOG_BT_SECURITY_STATE        0x82
#define LOG_BT_LE_CONN_MANAGER_STATE 0x83
#define LOG_BT_LE_CHANNEL_MAP_STATE  0x84
#define LOG_BT_LE_ENCRYPTION_STATE   0x85

/******************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    unsigned short rom_version;
    unsigned short build_version;
} __attribute__ ((packed)) patch_version;

typedef struct {
    unsigned int patch_id;
    patch_version patch_ver;
    unsigned int patch_base_addr;
    unsigned int patch_entry_addr;
    unsigned short patch_length;
    int patch_crc;
    unsigned short patch_ctrl;
} __attribute__ ((packed)) patch_info;

typedef struct {
    unsigned int  tlv_data_len;
    unsigned int  tlv_patch_data_len;
    unsigned char sign_ver;
    unsigned char sign_algorithm;
    unsigned char dwnd_cfg;
    unsigned char reserved1;
    unsigned short prod_id;
    unsigned short build_ver;
    unsigned short patch_ver;
    unsigned short reserved2;
    unsigned int patch_entry_addr;
} __attribute__ ((packed)) tlv_patch_hdr;

typedef struct {
    unsigned short tag_id;
    unsigned short tag_len;
    unsigned int tag_ptr;
    unsigned int tag_ex_flag;
} __attribute__ ((packed)) tlv_nvm_hdr;

typedef struct {
    unsigned char tlv_type;
    unsigned char tlv_length1;
    unsigned char tlv_length2;
    unsigned char tlv_length3;

    union{
        tlv_patch_hdr patch;
        tlv_nvm_hdr nvm;
    }tlv;
} __attribute__ ((packed)) tlv_patch_info;


enum{
    BAUDRATE_115200     = 0x00,
    BAUDRATE_57600      = 0x01,
    BAUDRATE_38400      = 0x02,
    BAUDRATE_19200      = 0x03,
    BAUDRATE_9600       = 0x04,
    BAUDRATE_230400     = 0x05,
    BAUDRATE_250000     = 0x06,
    BAUDRATE_460800     = 0x07,
    BAUDRATE_500000     = 0x08,
    BAUDRATE_720000     = 0x09,
    BAUDRATE_921600     = 0x0A,
    BAUDRATE_1000000   = 0x0B,
    BAUDRATE_1250000   = 0x0C,
    BAUDRATE_2000000   = 0x0D,
    BAUDRATE_3000000   = 0x0E,
    BAUDRATE_4000000   = 0x0F,
    BAUDRATE_1600000   = 0x10,
    BAUDRATE_3200000   = 0x11,
    BAUDRATE_3500000   = 0x12,
    BAUDRATE_AUTO        = 0xFE,
    BAUDRATE_Reserved  = 0xFF
};

enum{
    ROME_PATCH_VER_0100 = 0x0100,
    ROME_PATCH_VER_0101 = 0x0101,
    ROME_PATCH_VER_0200 = 0x0200,
    ROME_PATCH_VER_0300 = 0x0300,
    ROME_PATCH_VER_0302 = 0x0302
 };

enum{
    ROME_SOC_ID_00 = 0x00000000,
    ROME_SOC_ID_11 = 0x00000011,
    ROME_SOC_ID_22 = 0x00000022,
    ROME_SOC_ID_44 = 0x00000044
};

enum{
    ROME_VER_UNKNOWN = 0,
    ROME_VER_1_0 = ((ROME_PATCH_VER_0100 << 16 ) | ROME_SOC_ID_00 ),
    ROME_VER_1_1 = ((ROME_PATCH_VER_0101 << 16 ) | ROME_SOC_ID_00 ),
    ROME_VER_1_3 = ((ROME_PATCH_VER_0200 << 16 ) | ROME_SOC_ID_00 ),
    ROME_VER_2_1 = ((ROME_PATCH_VER_0200 << 16 ) | ROME_SOC_ID_11 ),
    ROME_VER_3_0 = ((ROME_PATCH_VER_0300 << 16 ) | ROME_SOC_ID_22 ),
    ROME_VER_3_2 = ((ROME_PATCH_VER_0302 << 16 ) | ROME_SOC_ID_44 )
};

extern char *rampatch_file_path;
extern char *nvm_file_path;
extern char *mbn_file_path;

//declarations
int rome_soc_init(int fd, char *bdaddr);
#endif /* HW_ROME_H */
