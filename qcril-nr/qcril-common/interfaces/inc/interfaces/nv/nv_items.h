/******************************************************************************
#  Copyright (c) 2008-2013, 2017, 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __NV_ITEMS__H_
#define __NV_ITEMS__H_

#include <stdint.h>
#include <stddef.h>

typedef enum {
  NV_AUTO_ANSWER_I = 74,
  NV_PREF_VOICE_SO_I = 285,
  NV_ROAM_CNT_I = 169,
  NV_AIR_CNT_I = 168,
  NV_MIN1_I = 32,
  NV_MIN2_I = 33,
  NV_IMSI_MCC_I = 176,
  NV_IMSI_11_12_I = 177,
  NV_IMSI_T_S1_I = 262,
  NV_IMSI_T_S2_I = 263,
  NV_IMSI_T_MCC_I = 264,
  NV_IMSI_T_11_12_I = 265,
  NV_IMSI_T_ADDR_NUM_I = 266,
  NV_PCDMACH_I = 20,
  NV_SCDMACH_I = 21,
  NV_HOME_SID_NID_I = 259,
  NV_DIR_NUMBER_I = 178,
  NV_SID_NID_I = 38,
  NV_MOB_CAI_REV_I = 6,
  NV_NAME_NAM_I = 43
} nv_items_enum_type;

/* Up to 2 MINs per NAM allowed */
#define NV_MAX_MINS 2
/* Up to 20 home SID/NID pairs */
#define NV_MAX_HOME_SID_NID 20
/* num digits in dir_number */
#define NV_DIR_NUMB_SIZ 10
/* Max SID+NID */
#define NV_MAX_SID_NID 1
/* With up to 12-letter names */
#define NV_MAX_LTRS 12

/* Type to specify auto answer rings and enable/disable. */
typedef struct __attribute__((__packed__)) {
  /* TRUE if auto answer enabled */
  uint8_t enable;
  /* Number of rings when to answer call */
  uint32_t rings;
} nv_auto_answer_type;

typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint32_t nam;
  /* evrc_capability_enabled */
  uint8_t evrc_capability_enabled;
  /* home_page_voice_so */
  uint32_t home_page_voice_so;
  /* home_orig_voice_so */
  uint32_t home_orig_voice_so;
  /* roam_orig_voice_so */
  uint32_t roam_orig_voice_so;
} nv_pref_voice_so_type;

/* Air time counter */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* count */
  uint32_t cnt;
} nv_call_cnt_type;

/* Type to hold MIN1p for 2 MINs along with the associated NAM id. */
/*
 * The number is 24 bits, per CAI section 2.3.1.
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* MIN1 */
  uint32_t min1[NV_MAX_MINS];
} nv_min1_type;

/* Type to hold MIN2p for 2 MINs along with the associated NAM id. */
/*
 * The number is 10 bits, per CAI section 2.3.1.
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* MIN2 */
  uint16_t min2[NV_MAX_MINS];
} nv_min2_type;

/* Type to hold IMSI MCC , along with the associated NAM id. */
/*
 * The number is 24 bits
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* imsi_mcc */
  uint16_t imsi_mcc;
} nv_imsi_mcc_type;

/* Type to hold IMSI_11_12 for 4 MINs along with the associated NAM id */
/*
 * The number is 8 bits.
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* imsi_11_12 */
  uint8_t imsi_11_12;
} nv_imsi_11_12_type;

/* Type to hold IMSI length along with associated NAM id */
/*
 * The number is 3 bits, per J-STD-008 section 2.3.1.
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* Length of the IMSI for this NAM */
  uint8_t num;
} nv_imsi_addr_num_type;

/* Type to hold CDMA channel and associated NAM. */
/*
 * Value is 11 bits for Primary and Secondary channels,
 * per CAI section 6.1.1.
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* A carrier channel number */
  uint16_t channel_a;
  /* B carrier channel number */
  uint16_t channel_b;
} nv_cdmach_type;

/* Type to hold SID+NID pairs. */
/*
 * The SID is 15 bits, per CAI 2.3.8, and the NID is 16 bits,
 * per CAI section 2.3.10.3.
 */
typedef struct __attribute__((__packed__)) {
  /* 15 bits, per CAI 2.3.8 */
  uint16_t sid;
  /* 16 bits, per CAI section 2.3.10.3 */
  uint16_t nid;
} nv_sid_nid_pair_type;

/* Type to hold 'home' SID+NID pairs for CDMA acquisition */
/*
 * The type also holds NAM id. Note that this item is NOT
 * 'per-MIN'
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* SID+NID pair */
  nv_sid_nid_pair_type pair[NV_MAX_HOME_SID_NID];
} nv_home_sid_nid_type;

/* Type to hold DIR_NUMBER with associated NAM id */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* Directory Number */
  uint8_t dir_number[NV_DIR_NUMB_SIZ];
} nv_dir_number_type;

/* Type to hold SID+NID pairs for CDMA acquisition along with NAM id. */
/*
 * NID is 16 bits, per CAI section 2.3.10.3.  There are up to 4 SID+NID
 * pairs, in descending preferrence (0=first, 3=last).
 */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* SID+NID Pair */
  nv_sid_nid_pair_type pair[NV_MAX_MINS][NV_MAX_SID_NID];
} nv_sid_nid_type;

/* Type to hold the name of each NAM, along with the associated NAM id */
typedef struct __attribute__((__packed__)) {
  /* NAM id 0-N */
  uint8_t nam;
  /* NAM name string */
  uint8_t name[NV_MAX_LTRS];
} nv_name_nam_type;

typedef union __attribute__((__packed__)) {
  nv_auto_answer_type auto_answer;
  nv_pref_voice_so_type pref_voice_so;
  nv_call_cnt_type air_cnt;
  nv_call_cnt_type roam_cnt;
  nv_min1_type min1;
  nv_min2_type min2;
  nv_imsi_mcc_type imsi_mcc;
  nv_imsi_11_12_type imsi_11_12;
  nv_min1_type imsi_t_s1;
  nv_min2_type imsi_t_s2;
  nv_imsi_mcc_type imsi_t_mcc;
  nv_imsi_11_12_type imsi_t_11_12;
  nv_imsi_addr_num_type imsi_t_addr_num;
  nv_cdmach_type pcdmach;
  nv_cdmach_type scdmach;
  nv_home_sid_nid_type home_sid_nid;
  nv_dir_number_type dir_number;
  nv_sid_nid_type sid_nid;
  uint8_t mob_cai_rev;
  nv_name_nam_type name_nam;
} nv_item_type;

bool isValidNvItem(nv_items_enum_type nv_item_id);
size_t getNvItemSize(nv_items_enum_type nv_item_id);
bool isValidNvItemLen(nv_items_enum_type nv_item_id, uint32_t size);
const char* getNvItemName(nv_items_enum_type nv_item_id);

#endif  //__NV_ITEMS__H_

