/******************************************************************************
#  Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/common.h"
#include <string>
#include <vector>

typedef enum
{
    QMI_RIL_RTE_NONE = 0,
    QMI_RIL_RTE_FIRST,
    QMI_RIL_RTE_1x = QMI_RIL_RTE_FIRST,
    QMI_RIL_RTE_GSM,
    QMI_RIL_RTE_WCDMA,
    QMI_RIL_RTE_TDSCDMA,
    QMI_RIL_RTE_SUB_LTE,
    QMI_RIL_RTE_SUB_DO,
    QMI_RIL_RTE_SUB_NR5G,
    QMI_RIL_RTE_LAST = QMI_RIL_RTE_SUB_NR5G,
    QMI_RIL_RTE_CAP
} qmi_ril_nw_reg_rte_type;

typedef enum
{
    QMI_RIL_RTE_KIND_NONE = 0,
    QMI_RIL_RTE_KIND_VOICE,
    QMI_RIL_RTE_KIND_DATA,
    QMI_RIL_RTE_KIND_IMS,
    QMI_RIL_RTE_KIND_CAP
} qmi_ril_nw_reg_rte_kind_type;

typedef enum
{
    QMI_RIL_RAT_CONFIDENCE_UNKNOWN = 0,
    QMI_RIL_RAT_CONFIDENCE_FULL_SVC,
    QMI_RIL_RAT_CONFIDENCE_LTD_SVC,
    QMI_RIL_RAT_CONFIDENCE_ANTICIPATED,
    QMI_RIL_RAT_CONFIDENCE_NO_SVC
} qmi_ril_nw_reg_rat_confidence_tag_type;

typedef enum
{
    QMI_RIL_NW_REG_FULL_SERVICE             = 1,
    QMI_RIL_NW_REG_LIMITED_SERVICE          = 2,
    QMI_RIL_NW_REG_VOICE_CALLS_AVAILABLE    = 4,
    QMI_RIL_NW_REG_VOIP_CALLS_AVAILABLE     = 8
} qmi_ril_nw_reg_status_overview_item_type;

typedef enum
{
  FTM_SUBSCRIPTION_INVALID = -1,
  FTM_SUBSCRIPTION_CDMA,
  FTM_SUBSCRIPTION_GSM,
  FTM_SUBSCRIPTION_WCDMA
}oem_hook_ftm_subscription_source_e_type;

struct ftm_subs_status
{
  oem_hook_ftm_subscription_source_e_type subs_type;
  int is_enable;
};

struct cdma_ftm_data
{
  uint32_t srv_status;
  uint32_t srv_domain;
  uint32_t system_mode;
  uint32_t roam_status;
  uint32_t mcc;
  uint32_t mnc;
  int8_t rssi;
  int16_t ecio;
  float cdma_1x_rx0_agc;
  float cdma_1x_rx1_agc;
  float cdma_evdo_rx0_agc;
  float cdma_evdo_rx1_agc;
};

struct gsm_neigh_cell_info
{
  uint32_t cell_id;
  uint16_t bcch;
  uint8_t bsic;
  uint16_t rx_level;
};

struct gsm_ftm_data
{
  uint32_t srv_status;
  uint32_t srv_domain;
  uint32_t system_mode;
  uint32_t roam_status;
  uint32_t mcc;
  uint32_t mnc;
  uint16_t lac;
  int8_t rssi;
  uint16_t bcch;
  uint8_t bsic;
  uint16_t rx_level;
  uint16_t rx_qual_full;
  uint16_t rx_qual_sub;
  uint16_t ta;
  uint32_t no_of_neigh_cell_info_len;
  struct gsm_neigh_cell_info gsm_neigh_cell[6];
};

struct wcdma_neigh_cell_info
{
  uint32_t cell_id;
  uint16_t uarfcn;
  uint16_t psc;
  int16_t rscp;
  int16_t ecio;
};

struct wcdma_ftm_data
{
  uint32_t srv_status;
  uint32_t srv_domain;
  uint32_t system_mode;
  uint32_t roam_status;
  uint32_t mcc;
  uint32_t mnc;
  uint16_t lac;
  uint16_t bler;
  int16_t ecio;
  int16_t rscp;
  float rx_agc;
  float tx_agc;
  uint16_t uarfcn;
  uint16_t psc;
  uint32_t no_of_neigh_cell_info_len;
  struct wcdma_neigh_cell_info wcdma_neigh_cell[6];
};

namespace qcril {
namespace interfaces {

enum class NasSettingResultCode {
    SUCCESS = RIL_E_SUCCESS,
    FAILURE = RIL_E_INTERNAL_ERR,
};

struct RilGetVoiceRegResult_t : public qcril::interfaces::BasePayload {
  RIL_VoiceRegistrationStateResponse respData;
};

struct RilGetDataRegResult_t : public qcril::interfaces::BasePayload {
  RIL_DataRegistrationStateResponse respData;
};

struct RilGetSysSelResult_t : public qcril::interfaces::BasePayload {
  RIL_SysSelChannels respData;
};

struct RilGetOperatorResult_t: public qcril::interfaces::BasePayload {
  std::string longName;
  std::string shortName;
  std::string numeric;
  template<typename T1, typename T2, typename T3>
  inline RilGetOperatorResult_t(T1&& _longName, T2&& _shortName, T3&& _numeric):
        longName(std::forward<T1>(_longName)), shortName(std::forward<T2>(_shortName)),
        numeric(std::forward<T3>(_numeric)) {}
};

struct RilGetSelectModeResult_t: public qcril::interfaces::BasePayload {
  bool bManual;
  RIL_AccessMode accessMode = RIL_ACCESS_MODE_INVALID;
  explicit inline RilGetSelectModeResult_t(bool manual, RIL_AccessMode accessMode):
        bManual(manual), accessMode(accessMode) {}
};

enum class NetworkStatus {UNKNOWN, AVAILABLE, CURRENT, FORBIDDEN};
struct NetworkInfo {
  std::string alphaLong;
  std::string alphaShort;
  std::string operatorNumeric;
  NetworkStatus status;
};
struct RilGetAvailNetworkResult_t: public qcril::interfaces::BasePayload {
  std::vector<NetworkInfo> info;
  template<typename T>
  inline RilGetAvailNetworkResult_t(T _info):
        info(std::forward<T>(_info)) {}
};

struct RilGetModePrefResult_t: public qcril::interfaces::BasePayload {
  int pref;
  inline RilGetModePrefResult_t(int value): pref(value) {}
};

struct RilGetSignalStrengthResult_t: public qcril::interfaces::BasePayload {
  RIL_SignalStrength respData;
};

struct RilGetVoiceTechResult_t: public qcril::interfaces::BasePayload {
  int rat;
  inline RilGetVoiceTechResult_t(int value): rat(value) {}
};

struct RilGetRadioAccessFamilyResult_t: public qcril::interfaces::BasePayload {
  int32_t radioAccessFamily;
  inline RilGetRadioAccessFamilyResult_t(int32_t raf): radioAccessFamily(raf) {}
};

struct RilNeighboringCell_t {
  std::string cid;
  int rssi;
};

struct RilGetNeighborCellIdResult_t: public qcril::interfaces::BasePayload {
  std::vector<RilNeighboringCell_t> cellList;
  template<typename T>
  inline RilGetNeighborCellIdResult_t(T list):
        cellList(std::forward<T>(list)) {}
};

struct RilGetCdmaSubscriptionResult_t: public qcril::interfaces::BasePayload {
  std::string mdn;
  std::string hSid;
  std::string hNid;
  std::string min;
  std::string prl;

  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  inline RilGetCdmaSubscriptionResult_t(T1 _mdn, T2 _hSid, T3 _hNid, T4 _min, T5 _prl):
    mdn(std::forward<T1>(_mdn)), hSid(std::forward<T2>(_hSid)),
    hNid(std::forward<T3>(_hNid)), min(std::forward<T4>(_min)),
    prl(std::forward<T5>(_prl)) {}
};

struct RilQueryAvailBandsResult_t: public qcril::interfaces::BasePayload {
  std::vector<int> bandList;
  template<typename T>
  inline RilQueryAvailBandsResult_t(T list):
    bandList(std::forward<T>(list)) {}
};

struct RilGetUsageSettingResult_t: public qcril::interfaces::BasePayload {
  RIL_UsageSettingMode mode;
  inline RilGetUsageSettingResult_t(RIL_UsageSettingMode inMode):
    mode(inMode) {}
};

struct RilRadioCapResult_t: public qcril::interfaces::BasePayload {
  RIL_RadioCapability respData;
};

struct RilGetModemStackResult_t: public qcril::interfaces::BasePayload {
    bool state;
    inline RilGetModemStackResult_t(bool stackstate): state(stackstate) {}
};

struct RilGetUiccAppStatusResult_t: public qcril::interfaces::BasePayload {
    bool state;
    inline RilGetUiccAppStatusResult_t(bool enable): state(enable) {}
};

enum class RIL_EccNumberSourceMask {
    DEFAULT = 1 << 0,
    NETWORK_SIGNALING = 1 << 1,
    SIM = 1 << 2,
    MODEM_CONFIG = 1 << 3
};

struct RIL_EmergencyNumber_t: public qcril::interfaces::BasePayload {
    std::string number;
    std::string mcc;
    std::string mnc;
    uint64_t categories;
    vector<std::string> urns;
    RIL_EccNumberSourceMask sources;
};

typedef std::vector<std::shared_ptr<qcril::interfaces::RIL_EmergencyNumber_t>> RIL_EccList;
typedef std::unordered_map<std::string, std::shared_ptr<RIL_EmergencyNumber_t>> RIL_EccMapType;

typedef struct {
    uint8_t modemId;
} RIL_ModemInfo;

typedef struct {
    uint8_t maxActiveData;
    uint8_t maxActiveInternetData;
    bool isInternetLingeringSupported;
    std::vector<RIL_ModemInfo> logicalModemList;
} RIL_PhoneCapability;

struct RilPhoneCapabilityResult_t: public qcril::interfaces::BasePayload {
    RIL_PhoneCapability phoneCap;
    inline RilPhoneCapabilityResult_t(RIL_PhoneCapability pc): phoneCap(pc) {}
};

struct RilQueryEndcStatusResult_t: public qcril::interfaces::BasePayload {
  endc_status status;
  inline RilQueryEndcStatusResult_t(endc_status _status): status(_status) {}
};

enum class ServiceDomain {
    UNSPECIFIED = 0,
    CS_DOMAIN = 1 /* (1 << 0) */,
    PS_DOMAIN = 2 /* (1 << 1) */,
};

struct RilNwRegistrationRejectInfo_t: public qcril::interfaces::BasePayload {
  bool causeCode_valid;
  int32_t causeCode;
  bool additionalCauseCode_valid;
  uint64_t domain;
  int32_t additionalCauseCode;
  bool choosenPlmn_valid;
  char choosenPlmn[MAX_MCC_MNC_LEN+1];
  bool rilCellIdentity_valid;
  RIL_CellIdentity_v16 rilCellIdentity;
};

struct RilQueryCdmaRoamingPrefResult_t : public qcril::interfaces::BasePayload {
  int mPrefType;
  inline RilQueryCdmaRoamingPrefResult_t(int value): mPrefType(value) {}
};

struct RilGetCdmaSubscriptionSourceResult_t : public qcril::interfaces::BasePayload {
  RIL_CdmaSubscriptionSource mSource;
  inline RilGetCdmaSubscriptionSourceResult_t(RIL_CdmaSubscriptionSource value): mSource(value) {}
};

enum class RilBarringServiceType {
    /** Applicable to UTRAN */
    /** Barring for all CS services, including registration */
    CS_SERVICE,
    /** Barring for all PS services, including registration */
    PS_SERVICE,
    /** Barring for mobile-originated circuit-switched voice calls */
    CS_VOICE,

    /** Applicable to EUTRAN, NGRAN */
    /** Barring for mobile-originated signalling for any purpose */
    MO_SIGNALLING,
    /** Barring for mobile-originated internet or other interactive data */
    MO_DATA,
    /** Barring for circuit-switched fallback calling */
    CS_FALLBACK,
    /** Barring for IMS voice calling */
    MMTEL_VOICE,
    /** Barring for IMS video calling */
    MMTEL_VIDEO,

    /** Applicable to UTRAN, EUTRAN, NGRAN */
    /** Barring for emergency services, either CS or emergency MMTEL */
    EMERGENCY,
    /** Barring for short message services */
    SMS,

    /** Operator-specific barring codes; applicable to NGRAN */
    OPERATOR_1 = 1001,
    OPERATOR_2 = 1002,
    OPERATOR_3 = 1003,
    OPERATOR_4 = 1004,
    OPERATOR_5 = 1005,
    OPERATOR_6 = 1006,
    OPERATOR_7 = 1007,
    OPERATOR_8 = 1008,
    OPERATOR_9 = 1009,
    OPERATOR_10 = 1010,
    OPERATOR_11 = 1011,
    OPERATOR_12 = 1012,
    OPERATOR_13 = 1013,
    OPERATOR_14 = 1014,
    OPERATOR_15 = 1015,
    OPERATOR_16 = 1016,
    OPERATOR_17 = 1017,
    OPERATOR_18 = 1018,
    OPERATOR_19 = 1019,
    OPERATOR_20 = 1020,
    OPERATOR_21 = 1021,
    OPERATOR_22 = 1022,
    OPERATOR_23 = 1023,
    OPERATOR_24 = 1024,
    OPERATOR_25 = 1025,
    OPERATOR_26 = 1026,
    OPERATOR_27 = 1027,
    OPERATOR_28 = 1028,
    OPERATOR_29 = 1029,
    OPERATOR_30 = 1030,
    OPERATOR_31 = 1031,
    OPERATOR_32 = 1032,
};

enum class RilBarringType {
    /** Device is not barred for the given service */
    NONE,
    /** Device may be barred based on time and probability factors */
    CONDITIONAL,
    /* Device is unconditionally barred */
    UNCONDITIONAL,
};

struct RilBarringTypeSpecificInfo {
    /** The barring factor as a percentage 0-100 */
    int32_t barringFactor;

    /** The number of seconds between re-evaluations of barring */
    int32_t barringTimeSeconds;

    /**
     * Indicates whether barring is currently being applied.
     *
     * <p>True if the UE applies barring to a conditionally barred
     * service based on the conditional barring parameters.
     *
     * <p>False if the service is conditionally barred but barring
     * is not currently applied, which could be due to either the
     * barring criteria not having been evaluated (if the UE has not
     * attempted to use the service) or due to the criteria being
     * evaluated and the UE being permitted to use the service
     * despite conditional barring.
     */
    bool isBarred;
};

struct RILBarringInfo {
    RilBarringServiceType barringService;
    RilBarringType barringType;
    RilBarringTypeSpecificInfo typeSpecificInfo;
};

struct RilCellBarringInfo_t: public qcril::interfaces::BasePayload {
  std::vector<RILBarringInfo> barring_info;
  bool rilCellIdentity_valid;
  RIL_CellIdentity_v16 rilCellIdentity;
};

struct RilUnsolBarringFilterResult_t: public qcril::interfaces::BasePayload {
  int32_t mIndicationFilter;
  inline RilUnsolBarringFilterResult_t(int32_t value): mIndicationFilter(value) {}
};

struct RilQueryNrDisableModeResult_t: public qcril::interfaces::BasePayload {
  RIL_NR_DISABLE_MODE mode;
  inline RilQueryNrDisableModeResult_t(RIL_NR_DISABLE_MODE _mode): mode(_mode) {}
};

struct RilPlmnIdInfo {
    uint16_t mcc;
    uint16_t mnc;
    uint8_t mnc_includes_pcs_digit;
    inline RilPlmnIdInfo(uint16_t _mcc, uint16_t _mnc, uint8_t _pcs_digit):
        mcc(_mcc), mnc(_mnc), mnc_includes_pcs_digit(_pcs_digit) {}
};

struct RilQueryVonrStatusResult_t: public qcril::interfaces::BasePayload {
  bool status;
  inline RilQueryVonrStatusResult_t(bool enable): status(enable) {}
};

enum class RilBandPrefType {
  NONE = 0,
  LTE_FULL,
  TDD_LTE,
  FDD_LTE,
};

enum class RilRatBandType {
  NONE = 1,
  LTE_BAND,
};

struct RILSubProvStatus_t: public qcril::interfaces::BasePayload{
  int32_t  user_preference;      // User SIM provisioning preference 0 - Deactivate, 1 - Activate
  int32_t  current_sub_preference;   // Current SIM provisioning status  0 - Deactivate, 1 - Activate
  inline RILSubProvStatus_t(int32_t user_pref, int32_t sub_pref):
   user_preference(user_pref), current_sub_preference(sub_pref) {};
};

struct RILSetSubProvState_t: public qcril::interfaces::BasePayload{
  int32_t  act_status;      // User SIM provisioning preference 0 - Deactivate, 1 - Activate
  int32_t  slot_id;   // Current SIM provisioning status  0 - Deactivate, 1 - Activate
  inline RILSetSubProvState_t(int32_t status, int32_t id):
   act_status(status), slot_id(id) {};
};

struct RilIccidResult_t: public qcril::interfaces::BasePayload {
  std::string mData;
  inline RilIccidResult_t(const char *iccid):
    mData(iccid) {};
};

struct SignalStrengthCriteriaEntry
{
    RIL_RadioAccessNetworks ran;
    RIL_SignalMeasureType type;
    int32_t hysteresisMs;
    int32_t hysteresisDb; // hysteresis values around thresholds
    std::vector<int32_t> thresholds;
    bool isEnabled;
};

static inline std::string toString(RIL_RadioAccessNetworks ran) {
    std::string ret = "<Unknown>";
    switch(ran) {
        case GERAN:
            ret = "GERAN";
            break;
        case UTRAN:
            ret = "UTRAN";
            break;
        case EUTRAN:
            ret = "EUTRAN";
            break;
        case CDMA:
            ret = "CDMA";
            break;
        case IWLAN:
            ret = "IWLAN";
            break;
        case NGRAN:
            ret = "NGRAN";
            break;
        default:
            break;
    }
    return ret;
}

}  // namespace interfaces
}  // namespace qcril
