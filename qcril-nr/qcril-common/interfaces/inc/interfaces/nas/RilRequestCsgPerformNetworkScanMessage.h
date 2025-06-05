/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "telephony/ril.h"
#include "network_access_service_v01.h"

namespace qcril {
namespace interfaces {

struct CsgNetworkInfo_t {
  uint16_t mcc;
  /**<   A 16-bit integer representation of MCC. Range: 0 to 999.
   */

  uint16_t mnc;
  /**<   A 16-bit integer representation of MNC. Range: 0 to 999.
   */

  uint8_t mnc_includes_pcs_digit;
  /**<   MNC PCS digit include status
   */

  nas_csg_list_cat_enum_v01 csg_list_cat;
  /**<   Closed subscriber group category. Values: \n
       - 0 -- NAS_CSG_LIST_CAT_UNKNOWN -- Unknown CSG list \n
       - 1 -- NAS_CSG_LIST_CAT_ALLOWED -- Allowed CSG list \n
       - 2 -- NAS_CSG_LIST_CAT_OPERATOR -- Operator CSG list
  */

  nas_csg_info_type_v01 csg_info;
  /**<   Closed subscriber group information.
   */

  uint16_t getByteStreamLength() {
    return 2 +  // mcc
           2 +  // mnc
           1 +  // mnc_includes_pcs_digit
           4 +  // nas_csg_list_cat_enum_v01
           4 +  // csg_info.id
           1 +  // csg_info.name_len
           csg_info.name_len * 2;
  }

  std::vector<uint8_t> getByteStream() {
    std::vector<uint8_t> buffer;
    buffer.reserve(getByteStreamLength());
    uint8_t* ptr;
    // mcc
    ptr = (uint8_t*)&mcc;
    buffer.insert(buffer.end(), ptr, ptr + 2);

    ptr = (uint8_t*)&mnc;
    buffer.insert(buffer.end(), ptr, ptr + 2);

    ptr = (uint8_t*)&mnc_includes_pcs_digit;
    buffer.insert(buffer.end(), ptr, ptr + 1);

    ptr = (uint8_t*)&csg_list_cat;
    buffer.insert(buffer.end(), ptr, ptr + 4);

    ptr = (uint8_t*)&csg_info.id;
    buffer.insert(buffer.end(), ptr, ptr + 4);

    uint8_t name_len = csg_info.name_len;
    ptr = (uint8_t*)&name_len;
    buffer.insert(buffer.end(), ptr, ptr + 1);

    ptr = (uint8_t*)&csg_info.name;
    buffer.insert(buffer.end(), ptr, ptr + (csg_info.name_len * 2));

    return buffer;
  }
};

struct CsgNetworkSignalStrengthInfo_t {
  uint16_t mcc;
  /**<   A 16-bit integer representation of MCC. Range: 0 to 999.
   */

  uint16_t mnc;
  /**<   A 16-bit integer representation of MNC. Range: 0 to 999.
   */

  uint8_t mnc_includes_pcs_digit;
  /**<   MNC PCS digit include status
   */

  uint32_t csg_id;
  /**<   Closed subscriber group identifier.
   */

  int32_t signal_strength;
  /**<   Signal strength information in dBm.
   */

  uint16_t getByteStreamLength() {
    return 2 +  // mcc
           2 +  // mnc
           1 +  // mnc_includes_pcs_digit
           4 +  // csg_id
           4;   // signal_strength
  }

  std::vector<uint8_t> getByteStream() {
    std::vector<uint8_t> buffer;
    buffer.reserve(getByteStreamLength());
    uint8_t* ptr;
    // mcc
    ptr = (uint8_t*)&mcc;
    buffer.insert(buffer.end(), ptr, ptr + 2);

    ptr = (uint8_t*)&mnc;
    buffer.insert(buffer.end(), ptr, ptr + 2);

    ptr = (uint8_t*)&mnc_includes_pcs_digit;
    buffer.insert(buffer.end(), ptr, ptr + 1);

    ptr = (uint8_t*)&csg_id;
    buffer.insert(buffer.end(), ptr, ptr + 4);

    ptr = (uint8_t*)&signal_strength;
    buffer.insert(buffer.end(), ptr, ptr + 4);

    return buffer;
  }
};

struct CsgPerformNwScanResult_t : public qcril::interfaces::BasePayload {
  static const uint8_t SCAN_RESULT_TYPE = 0x13;
  static const uint8_t CSG_INFO_TYPE = 0x14;
  static const uint8_t CSG_SIG_INFO_TYPE = 0x15;

  /* 0x13 */
  /*  Network Scan Result */
  uint8_t scan_result_valid; /**< Must be set to true if scan_result is being passed */
  uint32_t scan_result;

  /* 0x14+mnc_pcs_bit */
  /*  CSG Information */
  uint8_t csg_info_valid; /**< Must be set to true if csg_info is being passed */
  uint8_t csg_info_len;   /**< Must be set to # of elements in csg_info */
  CsgNetworkInfo_t csg_info[NAS_3GPP_NETWORK_INFO_LIST_MAX_V01];

  /* 0x15+mnc_pcs_bit */
  /*  CSG Signal Strength Information */
  uint8_t csg_sig_info_valid; /**< Must be set to true if csg_sig_info is being passed */
  uint8_t csg_sig_info_len;   /**< Must be set to # of elements in csg_sig_info */
  CsgNetworkSignalStrengthInfo_t csg_sig_info[NAS_3GPP_NETWORK_INFO_LIST_MAX_V01];

  std::vector<uint8_t> toTlv() {
    std::vector<uint8_t> buffer;
    buffer.reserve(sizeof(CsgPerformNwScanResult_t));  // TODO
    uint8_t* ptr;

    if (scan_result_valid) {
      uint8_t type = SCAN_RESULT_TYPE;
      buffer.push_back(type);

      uint16_t length = 4;
      ptr = (uint8_t*)&length;
      buffer.insert(buffer.end(), ptr, ptr + 2);

      ptr = (uint8_t*)&scan_result;
      buffer.insert(buffer.end(), ptr, ptr + 4);
    }
    if (csg_info_valid && csg_info_len) {
      uint8_t type = CSG_INFO_TYPE;
      buffer.push_back(type);

      uint16_t length = csg_info_len ? 1 : 0;
      for (uint16_t i = 0; i < csg_info_len && i < NAS_3GPP_NETWORK_INFO_LIST_MAX_V01; i++) {
        length += csg_info[i].getByteStreamLength();
      }
      ptr = (uint8_t*)&length;
      buffer.insert(buffer.end(), ptr, ptr + 2);

      uint8_t num_instances = csg_info_len;
      buffer.push_back(num_instances);

      for (uint16_t i = 0; i < csg_info_len && i < NAS_3GPP_NETWORK_INFO_LIST_MAX_V01; i++) {
        auto chBuffer = csg_info[i].getByteStream();
        buffer.insert(buffer.end(), chBuffer.begin(), chBuffer.end());
      }
    }
    if (csg_sig_info_valid && csg_sig_info_len) {
      uint8_t type = CSG_SIG_INFO_TYPE;
      buffer.push_back(type);

      uint16_t length = csg_sig_info_len ? 1 : 0;
      for (uint16_t i = 0; i < csg_sig_info_len && i < NAS_3GPP_NETWORK_INFO_LIST_MAX_V01; i++) {
        length += csg_sig_info[i].getByteStreamLength();
      }
      ptr = (uint8_t*)&length;
      buffer.insert(buffer.end(), ptr, ptr + 2);

      uint8_t num_instances = csg_sig_info_len;
      buffer.push_back(num_instances);

      for (uint16_t i = 0; i < csg_sig_info_len && i < NAS_3GPP_NETWORK_INFO_LIST_MAX_V01; i++) {
        auto chBuffer = csg_sig_info[i].getByteStream();
        buffer.insert(buffer.end(), chBuffer.begin(), chBuffer.end());
      }
    }
    return buffer;
  }
};

}  // namespace interfaces
}  // namespace qcril

/**
 * Request to perform CSG network scan.
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::CsgPerformNwScanResult_t>
 **/
class RilRequestCsgPerformNetworkScanMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestCsgPerformNetworkScanMessage> {
 private:
  std::optional<nas_network_type_mask_type_v01> mNetworkType;
  std::optional<nas_nw_scan_type_enum_v01> mScanType;

 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestCsgPerformNetworkScanMessage";

  RilRequestCsgPerformNetworkScanMessage() = delete;
  ~RilRequestCsgPerformNetworkScanMessage() {
  }

  inline RilRequestCsgPerformNetworkScanMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

#define QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE 1
#define QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE 2

#define QCRIL_OEMHOOK_CSG_TAG_NETWORK_TYPE 0x10
#define QCRIL_OEMHOOK_CSG_TAG_SCAN_TYPE 0x11

  RIL_Errno setData(const uint8_t* data, uint32_t dataLen) {
    if (!data) {
      return RIL_E_INVALID_ARGUMENTS;
    }

    RIL_Errno result = RIL_E_SUCCESS;
    uint16_t len = 0;
    for (uint32_t i = 0; i < dataLen;) {
      uint8_t type = *data;
      i += QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE;
      data += QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE;
      switch (type) {
        case QCRIL_OEMHOOK_CSG_TAG_NETWORK_TYPE:
          result = RIL_E_INVALID_ARGUMENTS;
          if (i + QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE <= dataLen) {
            memcpy(&len, data, QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE);
            i += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            data += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            if (len == 1) {
              uint8_t value = *data;
              mNetworkType = static_cast<nas_network_type_mask_type_v01>(value);
              i += 1;
              data += 1;
              result = RIL_E_SUCCESS;
            }
          }
          break;
        case QCRIL_OEMHOOK_CSG_TAG_SCAN_TYPE:
          result = RIL_E_INVALID_ARGUMENTS;
          if (i + QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE <= dataLen) {
            memcpy(&len, data, QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE);
            i += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            data += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            if (len == 1) {
              uint8_t value = *data;
              mScanType = static_cast<nas_nw_scan_type_enum_v01>(value);
              i += 1;
              data += 1;
              result = RIL_E_SUCCESS;
            }
          }
          break;
        default:
          result = RIL_E_INVALID_ARGUMENTS;
          break;
      }
      if (result != RIL_E_SUCCESS) {
        break;
      }
    }
    return result;
  }

  bool hasNetworkType() {
    return mNetworkType ? true : false;
  }

  nas_network_type_mask_type_v01 getNetworkType() {
    return *mNetworkType;
  }
  bool hasScanType() {
    return mScanType ? true : false;
  }
  nas_nw_scan_type_enum_v01 getScanType() {
    return *mScanType;
  }
};
