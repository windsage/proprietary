/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __COMMONDEFS_H__
#define __COMMONDEFS_H__
#include "comdef.h"
#include <functional>

/**
 *  @file CommonDefs.h
 */

namespace qdp {

/**
 * @brief
 * Profile id of the modem profile
 */
using ProfileId = int32_t;
/**
 * @brief
 * Definition of callback function that will get invoked when
 * IProfileKey is no longer valid
 */
using ExpirationListener = std::function<void()>;

/**
 * @brief
 * TechType
 */

enum class TechType : uint32_t {
  THREE_GPP, /**< 3GPP */
  THREE_GPP2, /**< 3GPP2 */
};

/**
 * @brief
 * IPType
 * It can be used as a bitmask
 */
enum class IPType : uint32_t {
  UNKNOWN   = 0,            /**< Unknown */
  V4        = 1 << 0,       /**< IPV4 */
  V6        = 1 << 1,       /**< IPV6 */
  V4V6      = (V4 | V6),    /**< IPV4V6 */
};

/**
 * @brief
 * AuthType
 * It can be used as a bitmask
 */
enum class AuthType : uint32_t {
  NONE      = 0,              /**< None */
  PAP       = 1 << 0,         /**< PAP */
  CHAP      = 1 << 1,         /**< CHAP */
  PAP_CHAP  = (PAP | CHAP),   /**< PAP_CHAP */
};

/**
 * @brief
 * ApnTypes
 * It can be used as a bitmask
 */
enum class ApnTypes : uint32_t {
  NONE      = 0,          /**< None */
  DEFAULT   = 1 << 0,     /**< Default */
  MMS       = 1 << 1,     /**< MMS */
  SUPL      = 1 << 2,     /**< SUPL */
  DUN       = 1 << 3,     /**< DUN */
  HIPRI     = 1 << 4,     /**< HIPRI */
  FOTA      = 1 << 5,     /**< FOTA */
  IMS       = 1 << 6,     /**< IMS */
  CBS       = 1 << 7,     /**< CBS */
  IA        = 1 << 8,     /**< IA */
  EMERGENCY = 1 << 9,     /**< EMERGENCY */
  MCX       = 1 << 10,    /**< MCX */
  XCAP      = 1 << 11,    /**< XCAP */
  MAX,                    /**< Max value */
};

/**
 * @brief
 * ApnClass
 */
enum class ApnClass : uint32_t {
  IMS = 1,     /**< IMS */
  ADMN = 2,    /**< ADMN */
  INET = 3,    /**< INET */
  APP = 4,     /**< APP */
};

/**
 * @brief
 * FilterMatchType
 * Additional criteria that can be specified in
 * FilterProfileParams for the filter and match operations. It
 * can be used as a bitmask
 */
enum class FilterMatchType : uint32_t {
  None                     = 0,           /**< None */
  HomeIPSubset             = 1 << 0,      /**< Home IP Subset : client home IP is subset*/
  RoamIPSubset             = 1 << 1,      /**< Roam IP Subset : client roam IP is subset */
  ApnNameSubset            = 1 << 2,      /**< Apn Name Subset : client APN name is subset */
  ApnTypeBitmaskSubset     = 1 << 3,      /**< APN type bitmask subset : client APN type bitmask is subset */
  HomeIPSuperset           = 1 << 4,      /**< Home IP Superset : client home IP is superset */
  RoamIPSuperset           = 1 << 5,      /**< Roam IP Superset : client roam IP is superset */
};

/**
 * @brief
 * PDCRefreshStatus
 */
enum class PDCRefreshStatus : uint8_t {
  REFRESH_START    = 0,
  REFRESH_COMPLETE = 1,
};

} //namespace

#endif /*__COMMONDEFS_H__*/
