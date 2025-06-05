/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include  <iomanip>
#include <android-base/stringprintf.h>

#define IPv4_SIZE  (4)
#define IPv6_SIZE  (16)
#define MDNS_SIZE  (16)

/* Periodicity (wlan wake interval) for WHC AP use cases */
#define WLAN_WAKE_INTERVAL_MUSIC 130
#define WLAN_WAKE_INTERVAL_VOICE 30

using android::base::StringPrintf;

typedef struct bdaddr_t {
  uint8_t b[6];

  bool operator==(const bdaddr_t & other) const {
    return b[0] == other.b[0]
        && b[1] == other.b[1]
        && b[2] == other.b[2]
        && b[3] == other.b[3]
        && b[4] == other.b[4]
        && b[5] == other.b[5];
  }

  std::string toString() const {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", b[5],
             b[4], b[3], b[2], b[1], b[0]);
    buf[17] = '\0';

    std::string str(buf);

    return str;
  }

  bool isEmpty() const {
    return (b[0] == 0 &&
            b[1] == 0 &&
            b[2] == 0 &&
            b[3] == 0 &&
            b[4] == 0 &&
            b[5] == 0);
  }

} __attribute__((packed)) bdaddr_t;

typedef struct macaddr_t {
  uint8_t b[6];

  bool operator==(const bdaddr_t & other) const {
    return b[0] == other.b[0]
        && b[1] == other.b[1]
        && b[2] == other.b[2]
        && b[3] == other.b[3]
        && b[4] == other.b[4]
        && b[5] == other.b[5];
  }

  std::string toString() const {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", b[5],
             b[4], b[3], b[2], b[1], b[0]);
    buf[17] = '\0';

    std::string str(buf);

    return str;
  }

  bool isEmpty() const {
    return (b[0] == 0
        && b[1] == 0
        && b[2] == 0
        && b[3] == 0
        && b[4] == 0
        && b[5] == 0);
  }
} __attribute__((packed)) macaddr_t;

/* IP Address type */
typedef enum {
  IPv4 = 0,
  IPv6,
} IpAddrType;

typedef struct ipaddr_t{
  int8_t type;    /* type of IP address to be used */
  uint8_t ipv4[4] = {}; /* IPv4 Address */
  uint8_t ipv6[16] = {}; /* IPv6 Address */

  bool operator==(const ipaddr_t & other) const {
    if (type == IPv4) {
      return ipv4[0] == other.ipv4[0]
          && ipv4[1] == other.ipv4[1]
          && ipv4[2] == other.ipv4[2]
          && ipv4[3] == other.ipv4[3];
    } else if (type == IPv6) {
      return ipv6[0] == other.ipv6[0]
          && ipv6[1] == other.ipv6[1]
          && ipv6[2] == other.ipv6[2]
          && ipv6[3] == other.ipv6[3]
          && ipv6[4] == other.ipv6[4]
          && ipv6[5] == other.ipv6[5]
          && ipv6[6] == other.ipv6[6]
          && ipv6[7] == other.ipv6[7]
          && ipv6[8] == other.ipv6[8]
          && ipv6[9] == other.ipv6[9]
          && ipv6[10] == other.ipv6[10]
          && ipv6[11] == other.ipv6[11]
          && ipv6[12] == other.ipv6[12]
          && ipv6[13] == other.ipv6[13]
          && ipv6[14] == other.ipv6[14]
          && ipv6[15] == other.ipv6[15];
    }
    return false;
  }

  std::string toString() const {
    static char buf[40];

    if (type == IPv4) {
      snprintf(buf, sizeof(buf), "%03d.%03d.%03d.%03d", ipv4[0], \
          ipv4[1], ipv4[2], ipv4[3]);
      buf[16] = '\0';
    } else if (type == IPv6) {
      snprintf(buf, sizeof(buf),
          "%02%x02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", ipv6[0], \
          ipv6[1], ipv6[2], ipv6[3], ipv6[4], ipv6[5], ipv6[6], ipv6[7], ipv6[8], \
          ipv6[9], ipv6[10], ipv6[11], ipv6[12], ipv6[13], ipv6[14], ipv6[15]);
      buf[40] = '\0';
    }

    std::string str(buf);

    return str;
  }

  bool isEmpty() const {
    if (type == IPv4) {
      return (ipv4[0] == 0 && ipv4[1] == 0 && ipv4[2] == 0 && ipv4[3] == 0);
    } else if (type == IPv6) {
      return (ipv6[0] == 0 && ipv6[1] == 0 && ipv6[2] == 0 && ipv6[3] == 0 &&
              ipv6[4] == 0 && ipv6[5] == 0 && ipv6[6] == 0 && ipv6[7] == 0 &&
              ipv6[8] == 0 && ipv6[9] == 0 && ipv6[10] == 0 && ipv6[11] == 0 &&
              ipv6[12] == 0 && ipv6[13] == 0 && ipv6[14] == 0 && ipv6[15] == 0);
    }
    return false;
  }
} __attribute__((packed)) ipaddr_t;

typedef struct mdns_uuid_t {
  uint8_t b[16];

  bool operator==(const mdns_uuid_t & other) const {
    return b[0] == other.b[0]
        && b[1] == other.b[1]
        && b[2] == other.b[2]
        && b[3] == other.b[3]
        && b[4] == other.b[4]
        && b[5] == other.b[5]
        && b[6] == other.b[6]
        && b[7] == other.b[7]
        && b[8] == other.b[8]
        && b[9] == other.b[9]
        && b[10] == other.b[10]
        && b[11] == other.b[11]
        && b[12] == other.b[12]
        && b[13] == other.b[13]
        && b[14] == other.b[14]
        && b[15] == other.b[15];
  }

  /* format 8-4-4-4-12 : big endian */
  std::string toString() const {
    std::stringstream str1;
    std::string result;
    str1 << std::hex << (int)b[0];
    str1 << std::hex << std::setw(2) << (int)b[1];
    str1 << std::hex << std::setw(2) << (int)b[2];
    str1 << std::hex << std::setw(2) << (int)b[3];
    str1 << "-";
    str1 << std::hex << std::setw(2) << (int)b[4];
    str1 << std::hex << std::setw(2) << (int)b[5];
    str1 << "-";
    str1 << std::hex << std::setw(2) << (int)b[6];
    str1 << std::hex << std::setw(2) << (int)b[7];
    str1 << "-";
    str1 << std::hex << std::setw(2) << (int)b[8];
    str1 << std::hex << std::setw(2) << (int)b[9];
    str1 << "-";
    str1 << std::hex << std::setw(2) << (int)b[10];
    str1 << std::hex << std::setw(2) << (int)b[11];
    str1 << std::hex << std::setw(2) << (int)b[12];
    str1 << std::hex << std::setw(2) << (int)b[13];
    str1 << std::hex << std::setw(2) << (int)b[14];
    str1 << std::hex << std::setw(2) << (int)b[15];
    result += str1.str();

    return result;
  }

} __attribute__((packed)) mdns_uuid_t;

typedef enum {
  DEFAULT = 0,
  BR_EDR,
  BT_LE,
  XPAN_AP,
  XPAN_P2P,
  XPAN_AP_PREP,
  NONE = 0x10,
} TransportType;

/* Use Same usecase as WiFi vendor lib */
typedef enum {
  USECASE_XPAN_NONE = 0,
  USECASE_XPAN_LOSSLESS,
  USECASE_XPAN_GAMING,
  USECASE_XPAN_VBC,
  USECASE_XPAN_AP_LOSSLESS = 5,
  USECASE_XPAN_AP_GAMING,
  USECASE_XPAN_LE_VOICE,
  USECASE_XPAN_AP_VOICE
} UseCaseType;

/* Use Same usecase as WiFi vendor lib */
typedef enum {
  QHCI_To_XM = 1,
  XM_To_QHCI,
  CP_To_XM,
} ApiDirection;

/* assigned 20 error reason for every client */
#define XM_KP_ERROR_REASON_OFFSET      0x14

typedef enum {
  XM_SUCCESS = 0,
  XM_XP_FAILED,
  XM_FAILED_STATE_ALREADY_IN_REQUESTED_TRANSPORT,
  XM_FAILED_WRONG_TRANSPORT_TYPE_REQUESTED,
  XM_FAILED_DUE_TO_AUDIO_BEARER_TIMEOUT,
  XM_FAILED_DUE_TO_UNPREPARE_AUDIO_BEARER_TIMEOUT,
  XM_FAILED_TO_ESTABLISH_ACS,
  XM_FAILED_REQUSTED_WRONG_TRANSPORT_TYPE,
  XM_FAILED_REQUSTED_WRONG_BDADDRESS,
  XM_NOTALLOWING_UNPREPARE_AS_STATE_IS_IDLE,
  XM_FAILED_AS_SEAMLESS_SWITCH_IS_NOT_ALLOWED,
  XM_XP_PERFORMED_ON_WRONG_BD_ADDRESS,
  XM_XP_WRONG_TRANSPORT_TYPE_REQ,
  XM_XP_BEARER_PREFERENCE_TIMEOUT,
  XM_XP_WRONG_BDADDRESS,
  XM_WIFI_REJECTED_TRANSPORT_SWITCH,
  XM_FAILED,
  XM_KP_MSG_SUCCESS = XM_KP_ERROR_REASON_OFFSET,
  /* Error while processing the message */
  XM_KP_MSG_FAILED,
  /* Wrong transport type selected by XPAN manager */
  XM_KP_MSG_WRONG_TRANSPORT_TYPE,
  /* Timeout triggered to receive bearer switch indications*/
  XM_KP_MSG_INTERNAL_TIMEOUT,
  /* Failed to Configure HWEP */
  XM_KP_MSG_FAILED_TO_CONFIGURE_HWEP,
  /* Failed to shutdown HWEP */
  XM_KP_MSG_FAILED_TO_SHUTDOWN_HWEP,
  XM_KP_MSG_ERR_WHILE_SHUTING_DOWN_HWEP,
  XM_KP_MSG_INVALID,
  XM_CP_FAILED_TO_SWITCH_BEARER,
  XM_AC_BEARER_PREFERENCE_REJECTED,
  XM_AC_PREPARE_AUDIO_BEARER_FAILED,
} RspStatus;

/* XPAN Earbud Role */
typedef enum {
  ROLE_PRIMARY,
  ROLE_SECONDARY,
  ROLE_INVALID,
} XpanEarbudRole;

/* Bluetooth Bond State */
typedef enum {
  BOND_NONE = 10,
  BOND_BONDING,
  BOND_BONDED,
} BondState;

/* L2CAP Pause/Unpause states */
typedef enum {
  UNPAUSE,
  PAUSE,
} L2capPauseUnpause;

#define FRONT_LEFT 1
#define FRONT_RIGHT 2
typedef struct {
  macaddr_t eb_ap_bssid;
  macaddr_t eb_mac_addr;
  uint32_t eb_audio_loc;
  ipaddr_t eb_ip_addr;
  XpanEarbudRole role;
} __attribute__((packed)) RemoteEbParams;

typedef struct {
  ipaddr_t ipAddr;
  uint8_t audioLocation;
  uint8_t role;
  macaddr_t mac_addr;
  macaddr_t mac_bssid;
} __attribute__((packed)) tXPAN_Eb_Params;

typedef struct {
  bdaddr_t addr;
  macaddr_t mac_bssid;
  uint8_t mdns;
  mdns_uuid_t mdnsUuid;
  uint16_t portL2capTcp;
  uint16_t portUdpAudio;
  uint16_t portUdpReports;
  uint8_t numOfEbs;
  tXPAN_Eb_Params vectorEbParams[2];
} __attribute__((packed)) tXPAN_Remote_Params;

static inline char * BondStateToString(BondState state)
{
  if (state == BOND_NONE)
    return "BOND_NONE";
  else if (state == BOND_BONDING)
    return "BOND_BONDING";
  else if (state == BOND_BONDED)
    return "BOND_BONDED";
  else
    return "INVALID STATE";
}

#define REQUESTOR_EB   0
#define REQUESTOR_WLAN 1
static inline char * StatusToString(RspStatus status)
{
  if (status == XM_SUCCESS)
    return "XM_SUCCESS";
  else if (status == XM_XP_FAILED)
    return "XM_XP_FAILED";
  else if (status == XM_FAILED_STATE_ALREADY_IN_REQUESTED_TRANSPORT)
    return "XM_FAILED_STATE_ALREADY_IN_REQUESTED_TRANSPORT";
  else if (status == XM_FAILED_DUE_TO_AUDIO_BEARER_TIMEOUT)
    return "XM_FAILED_DUE_TO_AUDIO_BEARER_TIMEOUT";
  else if (status == XM_FAILED_DUE_TO_UNPREPARE_AUDIO_BEARER_TIMEOUT)
    return "XM_FAILED_DUE_TO_UNPREPARE_AUDIO_BEARER_TIMEOUT";
  else if (status == XM_FAILED_TO_ESTABLISH_ACS)
    return "XM_FAILED_TO_ESTABLISH_ACS";
  else if (status == XM_FAILED_REQUSTED_WRONG_TRANSPORT_TYPE)
    return "XM_FAILED_REQUSTED_WRONG_TRANSPORT_TYPE";
  else if (status == XM_FAILED)
    return "XM_FAILED";
  else if (status == XM_KP_MSG_SUCCESS)
    return "XM_KP_MSG_SUCCESS";
  else if (status == XM_KP_MSG_FAILED)
    return "XM_KP_MSG_FAILED";
  else if (status == XM_KP_MSG_WRONG_TRANSPORT_TYPE)
    return "XM_KP_MSG_WRONG_TRANSPORT_TYPE";
  else if (status == XM_KP_MSG_INTERNAL_TIMEOUT)
    return "XM_KP_MSG_INTERNAL_TIMEOUT";
  else if (status == XM_KP_MSG_FAILED_TO_CONFIGURE_HWEP)
    return "XM_KP_MSG_FAILED_TO_CONFIGURE_HWEP";
  else if (status == XM_KP_MSG_FAILED_TO_SHUTDOWN_HWEP)
    return "XM_KP_MSG_FAILED_TO_SHUTDOWN_HWEP";
  else if (status == XM_KP_MSG_ERR_WHILE_SHUTING_DOWN_HWEP)
    return "XM_KP_MSG_ERR_WHILE_SHUTING_DOWN_HWEP";
  else if (status == XM_AC_BEARER_PREFERENCE_REJECTED)
    return "XM_AC_BEARER_PREFERENCE_REJECTED";
  else if (status == XM_AC_PREPARE_AUDIO_BEARER_FAILED)
    return "XM_AC_PREPARE_AUDIO_BEARER_FAILED";
  else
   return "INVALID Status";
}

static inline const char *ConvertRawBdaddress(bdaddr_t bdaddr)
{
  return android::base::StringPrintf ("%02X:%02X:%02X:%02X:%02X:%02X", bdaddr.b[5],
    bdaddr.b[4], bdaddr.b[3], bdaddr.b[2], bdaddr.b[1],
    bdaddr.b[0]).c_str();
}

static inline const char *ConvertRawMacaddress(macaddr_t addr)
{
  return android::base::StringPrintf("%02X:%02X:%02X:%02X:%02X:%02X", addr.b[0],
      addr.b[1], addr.b[2], addr.b[3], addr.b[4], addr.b[5]).c_str();
}

static inline char * TransportTypeToString(TransportType type)
{
  if (type == DEFAULT)
    return "DEFAULT";
  if (type == BR_EDR)
    return "BR_EDR";
  if (type == BT_LE)
    return "BT_LE";
  if (type == XPAN_AP)
    return "XPAN_AP";
  if (type == XPAN_P2P)
    return "XPAN_P2P";
  if (type == XPAN_AP_PREP)
    return "XPAN_AP_PREP";
  if (type == NONE)
    return "NONE";
  else
    return "INVALID Transport";
}

static inline char* UseCaseToString(UseCaseType usecase)
{
  if (usecase == USECASE_XPAN_LOSSLESS)
    return "USECASE_XPAN_LOSSLESS";
  else if(usecase == USECASE_XPAN_VBC)
    return "USECASE_XPAN_VBC";
  else if(usecase == USECASE_XPAN_GAMING)
    return "USECASE_XPAN_GAMING";
  else if(usecase == USECASE_XPAN_AP_LOSSLESS)
    return "USECASE_XPAN_AP_LOSSLESS";
  else if(usecase == USECASE_XPAN_AP_GAMING)
    return "USECASE_XPAN_AP_GAMING";
  else if(usecase == USECASE_XPAN_AP_VOICE)
    return "USECASE_XPAN_AP_VOICE";
  else
    return "INVALID USECASE";
}

#define ACTIVE_BDADDR {0x00, 0x00, 0x00, 0x00, 0x00,0x00}

#define SET_BIT(pp ,bit)	((pp) |=  (1<<(bit)))
#define CLEAR_BIT(pp, bit)	((pp) &= ~(1<<(bit)))
#define IS_BIT_SET(pp, bit)	((pp) &   (1<<(bit)))

#define DATA_TO_UINT_64(u, ptr)                                      \
  {                                                                   \
    (u) = (((uint64_t)(*(ptr))) + ((((uint64_t)(*((ptr) + 1)))) << 8) + \
             ((((uint64_t)(*((ptr) + 2)))) << 16) +                     \
             ((((uint64_t)(*((ptr) + 3)))) << 24) +                     \
             ((((uint64_t)(*((ptr) + 4)))) << 32) +                     \
             ((((uint64_t)(*((ptr) + 5)))) << 40) +                     \
             ((((uint64_t)(*((ptr) + 6)))) << 48) +                     \
             ((((uint64_t)(*((ptr) + 7)))) << 56));                     \
    (ptr) += 8;                                                         \
  }

#define DATA_TO_UINT_32(u, ptr)                                       \
{                                                                     \
  (u) = (((uint32_t)(*(ptr))) + ((((uint32_t)(*((ptr) + 1)))) << 8) + \
           ((((uint32_t)(*((ptr) + 2)))) << 16) +                     \
           ((((uint32_t)(*((ptr) + 3)))) << 24));                     \
  (ptr) += 4;                                                         \
}

#define DATA_TO_UINT_16(u, ptr)                                    \
{                                                                 \
  (u) = ((uint16_t)(*(ptr)) + (((uint16_t)(*((ptr) + 1))) << 8)); \
  (ptr) += 2;                                                     \
}

#define DATA_TO_UINT_8(u, ptr) \
{                            \
  (u) = (uint8_t)(*(ptr));    \
  ptr++;                      \
}

#define DATA_TO_UINT8_ARRAY(arr, ptr, length)\
{                                      \
  int k;                               \
  for (k = 0; k < (length); k++)       \
    ((uint8_t*)(arr))[k] = *(ptr)++;       \
}
