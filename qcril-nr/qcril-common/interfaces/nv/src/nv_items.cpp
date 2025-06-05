/******************************************************************************
#  Copyright (c) 2009-2013, 2017, 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "interfaces/nv/nv_items.h"

typedef struct
{
  uint32_t nv_item;
  size_t nv_item_size;
  size_t nv_item_offset;
  const char * name;
} qcril_other_nv_table_entry_type;


#define FSIZ( type, field ) sizeof( ((type *) 0)->field )
#define FPOS offsetof

#define QCRIL_OTHER_NV_SO( item )       FSIZ( nv_item_type, item ), FPOS( nv_item_type, item )
#define QCRIL_OTHER_NUM_OF_NV_ITEMS     (sizeof( qcril_other_nv_table ) / sizeof( qcril_other_nv_table_entry_type ))

static const qcril_other_nv_table_entry_type qcril_other_nv_table[] =
{
  { NV_AUTO_ANSWER_I,           QCRIL_OTHER_NV_SO( auto_answer ),         "NV_AUTO_ANSWER_I" },
  { NV_PREF_VOICE_SO_I,         QCRIL_OTHER_NV_SO( pref_voice_so ),       "NV_PREF_VOICE_SO_I" },
  { NV_ROAM_CNT_I,              QCRIL_OTHER_NV_SO( roam_cnt ),            "NV_ROAM_CNT_I" },
  { NV_AIR_CNT_I,               QCRIL_OTHER_NV_SO( air_cnt ),             "NV_AIR_CNT_I" },
  { NV_MIN1_I,                  QCRIL_OTHER_NV_SO( min1 ),                "NV_MIN1_I" },
  { NV_MIN2_I,                  QCRIL_OTHER_NV_SO( min2 ),                "NV_MIN2_I" },
  { NV_IMSI_11_12_I,            QCRIL_OTHER_NV_SO( imsi_11_12 ),          "NV_IMSI_11_12_I" },
  { NV_IMSI_MCC_I,              QCRIL_OTHER_NV_SO( imsi_mcc ),            "NV_IMSI_MCC_I" },
  { NV_IMSI_T_MCC_I,            QCRIL_OTHER_NV_SO( imsi_t_mcc ),          "NV_IMSI_T_MCC_I" },
  { NV_IMSI_T_11_12_I,          QCRIL_OTHER_NV_SO( imsi_t_11_12 ),        "NV_IMSI_T_11_12_I" },
  { NV_IMSI_T_S1_I,             QCRIL_OTHER_NV_SO( imsi_t_s1 ),           "NV_IMSI_T_S1_I" },
  { NV_IMSI_T_S2_I,             QCRIL_OTHER_NV_SO( imsi_t_s2 ),           "NV_IMSI_T_S2_I" },
  { NV_IMSI_T_ADDR_NUM_I,       QCRIL_OTHER_NV_SO( imsi_t_addr_num ),     "NV_IMSI_T_ADDR_NUM_I" },
  { NV_PCDMACH_I,               QCRIL_OTHER_NV_SO( pcdmach ),             "NV_PCDMACH_I" },
  { NV_SCDMACH_I,               QCRIL_OTHER_NV_SO( scdmach ),             "NV_SCDMACH_I" },
  { NV_HOME_SID_NID_I,          QCRIL_OTHER_NV_SO( home_sid_nid ),        "NV_HOME_SID_NID_I" },
  { NV_DIR_NUMBER_I,            QCRIL_OTHER_NV_SO( dir_number ),          "NV_DIR_NUMBER_I" },
  { NV_SID_NID_I,               QCRIL_OTHER_NV_SO( sid_nid ),             "NV_SID_NID_I" },
  { NV_MOB_CAI_REV_I,           QCRIL_OTHER_NV_SO( mob_cai_rev ),         "NV_MOB_CAI_REV_I" },
  { NV_NAME_NAM_I,              QCRIL_OTHER_NV_SO( name_nam ),            "NV_NAME_NAM_I" },
};

bool isValidNvItem(nv_items_enum_type nv_item_id) {
  for (size_t index = 0; index < QCRIL_OTHER_NUM_OF_NV_ITEMS; index++) {
    if (qcril_other_nv_table[index].nv_item == nv_item_id) {
      return true;
    }
  }
  return false;
}

size_t getNvItemSize(nv_items_enum_type nv_item_id) {
  for (size_t index = 0; index < QCRIL_OTHER_NUM_OF_NV_ITEMS; index++) {
    if (qcril_other_nv_table[index].nv_item == nv_item_id) {
      return qcril_other_nv_table[index].nv_item_size;
    }
  }
  return 0;
}

bool isValidNvItemLen(nv_items_enum_type nv_item_id, uint32_t size) {
  for (size_t index = 0; index < QCRIL_OTHER_NUM_OF_NV_ITEMS; index++) {
    if (qcril_other_nv_table[index].nv_item == nv_item_id &&
        qcril_other_nv_table[index].nv_item_size == size) {
      return true;
    }
  }
  return false;
}

const char *getNvItemName(nv_items_enum_type nv_item_id) {
  for (size_t index = 0; index < QCRIL_OTHER_NUM_OF_NV_ITEMS; index++) {
    if (qcril_other_nv_table[index].nv_item == nv_item_id) {
      return qcril_other_nv_table[index].name;
    }
  }
  return "<invalid>";
}
