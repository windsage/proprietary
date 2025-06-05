/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "RadioDataServiceImpl.h"
#include "RadioServiceUtils.h"

namespace utils {

std::unordered_map<rildata::DataCallFailCause_t, ::aidl::android::hardware::radio::data::DataCallFailCause> dcFailCauseMap = {
    {rildata::DataCallFailCause_t::NONE, DataCallFailCause::NONE},
    {rildata::DataCallFailCause_t::OPERATOR_BARRED, DataCallFailCause::OPERATOR_BARRED},
    {rildata::DataCallFailCause_t::NAS_SIGNALLING, DataCallFailCause::NAS_SIGNALLING},
    {rildata::DataCallFailCause_t::LLC_SNDCP, DataCallFailCause::LLC_SNDCP},
    {rildata::DataCallFailCause_t::INSUFFICIENT_RESOURCES, DataCallFailCause::INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MISSING_UKNOWN_APN, DataCallFailCause::MISSING_UNKNOWN_APN },
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_ADDRESS_TYPE, DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE},
    {rildata::DataCallFailCause_t::USER_AUTHENTICATION, DataCallFailCause::USER_AUTHENTICATION},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_GGSN, DataCallFailCause::ACTIVATION_REJECT_GGSN},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_UNSPECIFIED, DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUPPORTED, DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUBSCRIBED, DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_OUT_OF_ORDER, DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER},
    {rildata::DataCallFailCause_t::NSAPI_IN_USE, DataCallFailCause::NSAPI_IN_USE},
    {rildata::DataCallFailCause_t::REGULAR_DEACTIVATION, DataCallFailCause::REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::QOS_NOT_ACCEPTED, DataCallFailCause::QOS_NOT_ACCEPTED},
    {rildata::DataCallFailCause_t::NETWORK_FAILURE, DataCallFailCause::NETWORK_FAILURE},
    {rildata::DataCallFailCause_t::UMTS_REACTIVATION_REQ, DataCallFailCause::UMTS_REACTIVATION_REQ},
    {rildata::DataCallFailCause_t::FEATURE_NOT_SUPP, DataCallFailCause::FEATURE_NOT_SUPP},
    {rildata::DataCallFailCause_t::TFT_SEMANTIC_ERROR, DataCallFailCause::TFT_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::TFT_SYTAX_ERROR, DataCallFailCause::TFT_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_CONTEXT, DataCallFailCause::UNKNOWN_PDP_CONTEXT},
    {rildata::DataCallFailCause_t::FILTER_SEMANTIC_ERROR, DataCallFailCause::FILTER_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::FILTER_SYTAX_ERROR, DataCallFailCause::FILTER_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::PDP_WITHOUT_ACTIVE_TFT, DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECTED_BCM_VIOLATION, DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION},
    {rildata::DataCallFailCause_t::ONLY_IPV4_ALLOWED, DataCallFailCause::ONLY_IPV4_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_IPV6_ALLOWED, DataCallFailCause::ONLY_IPV6_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_SINGLE_BEARER_ALLOWED, DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED},
    {rildata::DataCallFailCause_t::ESM_INFO_NOT_RECEIVED, DataCallFailCause::ESM_INFO_NOT_RECEIVED},
    {rildata::DataCallFailCause_t::PDN_CONN_DOES_NOT_EXIST, DataCallFailCause::PDN_CONN_DOES_NOT_EXIST},
    {rildata::DataCallFailCause_t::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED, DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::COLLISION_WITH_NETWORK_INITIATED_REQUEST, DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST},
    {rildata::DataCallFailCause_t::ONLY_IPV4V6_ALLOWED, DataCallFailCause::ONLY_IPV4V6_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_NON_IP_ALLOWED, DataCallFailCause::ONLY_NON_IP_ALLOWED},
    {rildata::DataCallFailCause_t::UNSUPPORTED_QCI_VALUE, DataCallFailCause::UNSUPPORTED_QCI_VALUE},
    {rildata::DataCallFailCause_t::BEARER_HANDLING_NOT_SUPPORTED, DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::MAX_ACTIVE_PDP_CONTEXT_REACHED, DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED},
    {rildata::DataCallFailCause_t::UNSUPPORTED_APN_IN_CURRENT_PLMN, DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN},
    {rildata::DataCallFailCause_t::INVALID_TRANSACTION_ID, DataCallFailCause::INVALID_TRANSACTION_ID},
    {rildata::DataCallFailCause_t::MESSAGE_INCORRECT_SEMANTIC, DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC},
    {rildata::DataCallFailCause_t::INVALID_MANDATORY_INFO, DataCallFailCause::INVALID_MANDATORY_INFO},
    {rildata::DataCallFailCause_t::MESSAGE_TYPE_UNSUPPORTED, DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED},
    {rildata::DataCallFailCause_t::MSG_TYPE_NONCOMPATIBLE_STATE, DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE},
    {rildata::DataCallFailCause_t::UNKNOWN_INFO_ELEMENT, DataCallFailCause::UNKNOWN_INFO_ELEMENT},
    {rildata::DataCallFailCause_t::CONDITIONAL_IE_ERROR, DataCallFailCause::CONDITIONAL_IE_ERROR},
    {rildata::DataCallFailCause_t::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE, DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE},
    {rildata::DataCallFailCause_t::PROTOCOL_ERRORS, DataCallFailCause::PROTOCOL_ERRORS},
    {rildata::DataCallFailCause_t::APN_TYPE_CONFLICT, DataCallFailCause::APN_TYPE_CONFLICT},
    {rildata::DataCallFailCause_t::INVALID_PCSCF_ADDR, DataCallFailCause::INVALID_PCSCF_ADDR},
    {rildata::DataCallFailCause_t::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN, DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED, DataCallFailCause::EMM_ACCESS_BARRED},
    {rildata::DataCallFailCause_t::EMERGENCY_IFACE_ONLY, DataCallFailCause::EMERGENCY_IFACE_ONLY},
    {rildata::DataCallFailCause_t::IFACE_MISMATCH, DataCallFailCause::IFACE_MISMATCH},
    {rildata::DataCallFailCause_t::COMPANION_IFACE_IN_USE, DataCallFailCause::COMPANION_IFACE_IN_USE},
    {rildata::DataCallFailCause_t::IP_ADDRESS_MISMATCH, DataCallFailCause::IP_ADDRESS_MISMATCH},
    {rildata::DataCallFailCause_t::IFACE_AND_POL_FAMILY_MISMATCH, DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED_INFINITE_RETRY, DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY},
    {rildata::DataCallFailCause_t::AUTH_FAILURE_ON_EMERGENCY_CALL, DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL},
    {rildata::DataCallFailCause_t::INVALID_DNS_ADDR, DataCallFailCause::INVALID_DNS_ADDR},
    {rildata::DataCallFailCause_t::MIP_FA_REASON_UNSPECIFIED, DataCallFailCause::MIP_FA_REASON_UNSPECIFIED},
    {rildata::DataCallFailCause_t::MIP_FA_ADMIN_PROHIBITED, DataCallFailCause::MIP_FA_ADMIN_PROHIBITED},
    {rildata::DataCallFailCause_t::MIP_FA_INSUFFICIENT_RESOURCES, DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE, DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE, DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_FA_REQUESTED_LIFETIME_TOO_LONG, DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG},
    {rildata::DataCallFailCause_t::MIP_FA_MALFORMED_REQUEST, DataCallFailCause::MIP_FA_MALFORMED_REQUEST},
    {rildata::DataCallFailCause_t::MIP_FA_MALFORMED_REPLY, DataCallFailCause::MIP_FA_MALFORMED_REPLY},
    {rildata::DataCallFailCause_t::MIP_FA_ENCAPSULATION_UNAVAILABLE, DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE, DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE, DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY, DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY},
    {rildata::DataCallFailCause_t::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED, DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_NAI, DataCallFailCause::MIP_FA_MISSING_NAI},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_HOME_AGENT, DataCallFailCause::MIP_FA_MISSING_HOME_AGENT},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_HOME_ADDRESS, DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS},
    {rildata::DataCallFailCause_t::MIP_FA_UNKNOWN_CHALLENGE, DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_CHALLENGE, DataCallFailCause::MIP_FA_MISSING_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_FA_STALE_CHALLENGE, DataCallFailCause::MIP_FA_STALE_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_HA_REASON_UNSPECIFIED, DataCallFailCause::MIP_HA_REASON_UNSPECIFIED},
    {rildata::DataCallFailCause_t::MIP_HA_ADMIN_PROHIBITED, DataCallFailCause::MIP_HA_ADMIN_PROHIBITED},
    {rildata::DataCallFailCause_t::MIP_HA_INSUFFICIENT_RESOURCES, DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE, DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE, DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_HA_REGISTRATION_ID_MISMATCH, DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH},
    {rildata::DataCallFailCause_t::MIP_HA_MALFORMED_REQUEST, DataCallFailCause::MIP_HA_MALFORMED_REQUEST},
    {rildata::DataCallFailCause_t::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS, DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS},
    {rildata::DataCallFailCause_t::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE, DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY, DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY},
    {rildata::DataCallFailCause_t::MIP_HA_ENCAPSULATION_UNAVAILABLE, DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::CLOSE_IN_PROGRESS, DataCallFailCause::CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_TERMINATION, DataCallFailCause::NETWORK_INITIATED_TERMINATION},
    {rildata::DataCallFailCause_t::MODEM_APP_PREEMPTED, DataCallFailCause::MODEM_APP_PREEMPTED},
    {rildata::DataCallFailCause_t::PDN_IPV4_CALL_DISALLOWED, DataCallFailCause::PDN_IPV4_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::PDN_IPV4_CALL_THROTTLED, DataCallFailCause::PDN_IPV4_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::PDN_IPV6_CALL_DISALLOWED, DataCallFailCause::PDN_IPV6_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::PDN_IPV6_CALL_THROTTLED, DataCallFailCause::PDN_IPV6_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::MODEM_RESTART, DataCallFailCause::MODEM_RESTART},
    {rildata::DataCallFailCause_t::PDP_PPP_NOT_SUPPORTED, DataCallFailCause::PDP_PPP_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::UNPREFERRED_RAT, DataCallFailCause::UNPREFERRED_RAT},
    {rildata::DataCallFailCause_t::PHYSICAL_LINK_CLOSE_IN_PROGRESS, DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::APN_PENDING_HANDOVER, DataCallFailCause::APN_PENDING_HANDOVER},
    {rildata::DataCallFailCause_t::PROFILE_BEARER_INCOMPATIBLE, DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE},
    {rildata::DataCallFailCause_t::SIM_CARD_CHANGED, DataCallFailCause::SIM_CARD_CHANGED},
    {rildata::DataCallFailCause_t::LOW_POWER_MODE_OR_POWERING_DOWN, DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN},
    {rildata::DataCallFailCause_t::APN_DISABLED, DataCallFailCause::APN_DISABLED},
    {rildata::DataCallFailCause_t::MAX_PPP_INACTIVITY_TIMER_EXPIRED, DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::IPV6_ADDRESS_TRANSFER_FAILED, DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED},
    {rildata::DataCallFailCause_t::TRAT_SWAP_FAILED, DataCallFailCause::TRAT_SWAP_FAILED},
    {rildata::DataCallFailCause_t::EHRPD_TO_HRPD_FALLBACK, DataCallFailCause::EHRPD_TO_HRPD_FALLBACK},
    {rildata::DataCallFailCause_t::MIP_CONFIG_FAILURE, DataCallFailCause::MIP_CONFIG_FAILURE},
    {rildata::DataCallFailCause_t::PDN_INACTIVITY_TIMER_EXPIRED, DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::MAX_IPV4_CONNECTIONS, DataCallFailCause::MAX_IPV4_CONNECTIONS},
    {rildata::DataCallFailCause_t::MAX_IPV6_CONNECTIONS, DataCallFailCause::MAX_IPV6_CONNECTIONS},
    {rildata::DataCallFailCause_t::APN_MISMATCH, DataCallFailCause::APN_MISMATCH},
    {rildata::DataCallFailCause_t::IP_VERSION_MISMATCH, DataCallFailCause::IP_VERSION_MISMATCH},
    {rildata::DataCallFailCause_t::DUN_CALL_DISALLOWED, DataCallFailCause::DUN_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::INTERNAL_EPC_NONEPC_TRANSITION, DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION},
    {rildata::DataCallFailCause_t::INTERFACE_IN_USE, DataCallFailCause::INTERFACE_IN_USE},
    {rildata::DataCallFailCause_t::APN_DISALLOWED_ON_ROAMING, DataCallFailCause::APN_DISALLOWED_ON_ROAMING},
    {rildata::DataCallFailCause_t::APN_PARAMETERS_CHANGED, DataCallFailCause::APN_PARAMETERS_CHANGED},
    {rildata::DataCallFailCause_t::NULL_APN_DISALLOWED, DataCallFailCause::NULL_APN_DISALLOWED},
    {rildata::DataCallFailCause_t::THERMAL_MITIGATION, DataCallFailCause::THERMAL_MITIGATION},
    {rildata::DataCallFailCause_t::DATA_SETTINGS_DISABLED, DataCallFailCause::DATA_SETTINGS_DISABLED},
    {rildata::DataCallFailCause_t::DATA_ROAMING_SETTINGS_DISABLED, DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED},
    {rildata::DataCallFailCause_t::DDS_SWITCHED, DataCallFailCause::DDS_SWITCHED},
    {rildata::DataCallFailCause_t::FORBIDDEN_APN_NAME, DataCallFailCause::FORBIDDEN_APN_NAME},
    {rildata::DataCallFailCause_t::DDS_SWITCH_IN_PROGRESS, DataCallFailCause::DDS_SWITCH_IN_PROGRESS},
    {rildata::DataCallFailCause_t::CALL_DISALLOWED_IN_ROAMING, DataCallFailCause::CALL_DISALLOWED_IN_ROAMING},
    {rildata::DataCallFailCause_t::NON_IP_NOT_SUPPORTED, DataCallFailCause::NON_IP_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::PDN_NON_IP_CALL_THROTTLED, DataCallFailCause::PDN_NON_IP_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::PDN_NON_IP_CALL_DISALLOWED, DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::CDMA_LOCK, DataCallFailCause::CDMA_LOCK},
    {rildata::DataCallFailCause_t::CDMA_INTERCEPT, DataCallFailCause::CDMA_INTERCEPT},
    {rildata::DataCallFailCause_t::CDMA_REORDER, DataCallFailCause::CDMA_REORDER},
    {rildata::DataCallFailCause_t::CDMA_RELEASE_DUE_TO_SO_REJECTION, DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION},
    {rildata::DataCallFailCause_t::CDMA_INCOMING_CALL, DataCallFailCause::CDMA_INCOMING_CALL},
    {rildata::DataCallFailCause_t::CDMA_ALERT_STOP, DataCallFailCause::CDMA_ALERT_STOP},
    {rildata::DataCallFailCause_t::CHANNEL_ACQUISITION_FAILURE, DataCallFailCause::CHANNEL_ACQUISITION_FAILURE},
    {rildata::DataCallFailCause_t::MAX_ACCESS_PROBE, DataCallFailCause::MAX_ACCESS_PROBE},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION, DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION},
    {rildata::DataCallFailCause_t::NO_RESPONSE_FROM_BASE_STATION, DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION},
    {rildata::DataCallFailCause_t::REJECTED_BY_BASE_STATION, DataCallFailCause::REJECTED_BY_BASE_STATION},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICES_INCOMPATIBLE, DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE},
    {rildata::DataCallFailCause_t::NO_CDMA_SERVICE, DataCallFailCause::NO_CDMA_SERVICE},
    {rildata::DataCallFailCause_t::RUIM_NOT_PRESENT, DataCallFailCause::RUIM_NOT_PRESENT},
    {rildata::DataCallFailCause_t::CDMA_RETRY_ORDER, DataCallFailCause::CDMA_RETRY_ORDER},
    {rildata::DataCallFailCause_t::ACCESS_BLOCK, DataCallFailCause::ACCESS_BLOCK},
    {rildata::DataCallFailCause_t::ACCESS_BLOCK_ALL, DataCallFailCause::ACCESS_BLOCK_ALL},
    {rildata::DataCallFailCause_t::IS707B_MAX_ACCESS_PROBES, DataCallFailCause::IS707B_MAX_ACCESS_PROBES},
    {rildata::DataCallFailCause_t::THERMAL_EMERGENCY, DataCallFailCause::THERMAL_EMERGENCY},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICES_NOT_ALLOWED, DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::INCOMING_CALL_REJECTED, DataCallFailCause::INCOMING_CALL_REJECTED},
    {rildata::DataCallFailCause_t::NO_SERVICE_ON_GATEWAY, DataCallFailCause::NO_SERVICE_ON_GATEWAY},
    {rildata::DataCallFailCause_t::NO_GPRS_CONTEXT, DataCallFailCause::NO_GPRS_CONTEXT},
    {rildata::DataCallFailCause_t::ILLEGAL_MS, DataCallFailCause::ILLEGAL_MS},
    {rildata::DataCallFailCause_t::ILLEGAL_ME, DataCallFailCause::ILLEGAL_ME},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED, DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_NOT_ALLOWED, DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK, DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK},
    {rildata::DataCallFailCause_t::IMPLICITLY_DETACHED, DataCallFailCause::IMPLICITLY_DETACHED},
    {rildata::DataCallFailCause_t::PLMN_NOT_ALLOWED, DataCallFailCause::PLMN_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::LOCATION_AREA_NOT_ALLOWED, DataCallFailCause::LOCATION_AREA_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN, DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN},
    {rildata::DataCallFailCause_t::PDP_DUPLICATE, DataCallFailCause::PDP_DUPLICATE},
    {rildata::DataCallFailCause_t::UE_RAT_CHANGE, DataCallFailCause::UE_RAT_CHANGE},
    {rildata::DataCallFailCause_t::CONGESTION, DataCallFailCause::CONGESTION},
    {rildata::DataCallFailCause_t::NO_PDP_CONTEXT_ACTIVATED, DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED},
    {rildata::DataCallFailCause_t::ACCESS_CLASS_DSAC_REJECTION, DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION},
    {rildata::DataCallFailCause_t::PDP_ACTIVATE_MAX_RETRY_FAILED, DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED},
    {rildata::DataCallFailCause_t::RADIO_ACCESS_BEARER_FAILURE, DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE},
    {rildata::DataCallFailCause_t::ESM_UNKNOWN_EPS_BEARER_CONTEXT, DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT},
    {rildata::DataCallFailCause_t::DRB_RELEASED_BY_RRC, DataCallFailCause::DRB_RELEASED_BY_RRC},
    {rildata::DataCallFailCause_t::CONNECTION_RELEASED, DataCallFailCause::CONNECTION_RELEASED},
    {rildata::DataCallFailCause_t::EMM_DETACHED, DataCallFailCause::EMM_DETACHED},
    {rildata::DataCallFailCause_t::EMM_ATTACH_FAILED, DataCallFailCause::EMM_ATTACH_FAILED},
    {rildata::DataCallFailCause_t::EMM_ATTACH_STARTED, DataCallFailCause::EMM_ATTACH_STARTED},
    {rildata::DataCallFailCause_t::LTE_NAS_SERVICE_REQUEST_FAILED, DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED},
    {rildata::DataCallFailCause_t::DUPLICATE_BEARER_ID, DataCallFailCause::DUPLICATE_BEARER_ID},
    {rildata::DataCallFailCause_t::ESM_COLLISION_SCENARIOS, DataCallFailCause::ESM_COLLISION_SCENARIOS},
    {rildata::DataCallFailCause_t::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK, DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK},
    {rildata::DataCallFailCause_t::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER, DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER},
    {rildata::DataCallFailCause_t::ESM_BAD_OTA_MESSAGE, DataCallFailCause::ESM_BAD_OTA_MESSAGE},
    {rildata::DataCallFailCause_t::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL, DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL},
    {rildata::DataCallFailCause_t::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT, DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT},
    {rildata::DataCallFailCause_t::DS_EXPLICIT_DEACTIVATION, DataCallFailCause::DS_EXPLICIT_DEACTIVATION},
    {rildata::DataCallFailCause_t::ESM_LOCAL_CAUSE_NONE, DataCallFailCause::ESM_LOCAL_CAUSE_NONE},
    {rildata::DataCallFailCause_t::LTE_THROTTLING_NOT_REQUIRED, DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED},
    {rildata::DataCallFailCause_t::ACCESS_CONTROL_LIST_CHECK_FAILURE, DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE},
    {rildata::DataCallFailCause_t::SERVICE_NOT_ALLOWED_ON_PLMN, DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN},
    {rildata::DataCallFailCause_t::EMM_T3417_EXPIRED, DataCallFailCause::EMM_T3417_EXPIRED},
    {rildata::DataCallFailCause_t::EMM_T3417_EXT_EXPIRED, DataCallFailCause::EMM_T3417_EXT_EXPIRED},
    {rildata::DataCallFailCause_t::RRC_UPLINK_DATA_TRANSMISSION_FAILURE, DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER, DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER},
    {rildata::DataCallFailCause_t::RRC_UPLINK_CONNECTION_RELEASE, DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_RADIO_LINK_FAILURE, DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_ERROR_REQUEST_FROM_NAS, DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ACCESS_STRATUM_FAILURE, DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS, DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ACCESS_BARRED, DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CELL_RESELECTION, DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CONFIG_FAILURE, DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_TIMER_EXPIRED, DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_LINK_FAILURE, DataCallFailCause::RRC_CONNECTION_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CELL_NOT_CAMPED, DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE, DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_REJECT_BY_NETWORK, DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_NORMAL_RELEASE, DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RADIO_LINK_FAILURE, DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_REESTABLISHMENT_FAILURE, DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER, DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORT_REQUEST, DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR, DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH, DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH, DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH},
    {rildata::DataCallFailCause_t::ESM_PROCEDURE_TIME_OUT, DataCallFailCause::ESM_PROCEDURE_TIME_OUT},
    {rildata::DataCallFailCause_t::INVALID_CONNECTION_ID, DataCallFailCause::INVALID_CONNECTION_ID},
    {rildata::DataCallFailCause_t::MAXIMIUM_NSAPIS_EXCEEDED, DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED},
    {rildata::DataCallFailCause_t::INVALID_PRIMARY_NSAPI, DataCallFailCause::INVALID_PRIMARY_NSAPI},
    {rildata::DataCallFailCause_t::CANNOT_ENCODE_OTA_MESSAGE, DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE},
    {rildata::DataCallFailCause_t::RADIO_ACCESS_BEARER_SETUP_FAILURE, DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE},
    {rildata::DataCallFailCause_t::PDP_ESTABLISH_TIMEOUT_EXPIRED, DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_MODIFY_TIMEOUT_EXPIRED, DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_INACTIVE_TIMEOUT_EXPIRED, DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_LOWERLAYER_ERROR, DataCallFailCause::PDP_LOWERLAYER_ERROR},
    {rildata::DataCallFailCause_t::PDP_MODIFY_COLLISION, DataCallFailCause::PDP_MODIFY_COLLISION},
    {rildata::DataCallFailCause_t::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED, DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED},
    {rildata::DataCallFailCause_t::NAS_REQUEST_REJECTED_BY_NETWORK, DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_INVALID_REQUEST, DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED, DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RF_UNAVAILABLE, DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE, DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE, DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_AFTER_HANDOVER, DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE, DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE, DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE},
    {rildata::DataCallFailCause_t::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER, DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER},
    {rildata::DataCallFailCause_t::IMEI_NOT_ACCEPTED, DataCallFailCause::IMEI_NOT_ACCEPTED},
    {rildata::DataCallFailCause_t::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED, DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::EPS_SERVICES_NOT_ALLOWED_IN_PLMN, DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN},
    {rildata::DataCallFailCause_t::MSC_TEMPORARILY_NOT_REACHABLE, DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE},
    {rildata::DataCallFailCause_t::CS_DOMAIN_NOT_AVAILABLE, DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE},
    {rildata::DataCallFailCause_t::ESM_FAILURE, DataCallFailCause::ESM_FAILURE},
    {rildata::DataCallFailCause_t::MAC_FAILURE, DataCallFailCause::MAC_FAILURE},
    {rildata::DataCallFailCause_t::SYNCHRONIZATION_FAILURE, DataCallFailCause::SYNCHRONIZATION_FAILURE},
    {rildata::DataCallFailCause_t::UE_SECURITY_CAPABILITIES_MISMATCH, DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH},
    {rildata::DataCallFailCause_t::SECURITY_MODE_REJECTED, DataCallFailCause::SECURITY_MODE_REJECTED},
    {rildata::DataCallFailCause_t::UNACCEPTABLE_NON_EPS_AUTHENTICATION, DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION},
    {rildata::DataCallFailCause_t::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED, DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::NO_EPS_BEARER_CONTEXT_ACTIVATED, DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED},
    {rildata::DataCallFailCause_t::INVALID_EMM_STATE, DataCallFailCause::INVALID_EMM_STATE},
    {rildata::DataCallFailCause_t::NAS_LAYER_FAILURE, DataCallFailCause::NAS_LAYER_FAILURE},
    {rildata::DataCallFailCause_t::MULTIPLE_PDP_CALL_NOT_ALLOWED, DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::EMBMS_NOT_ENABLED, DataCallFailCause::EMBMS_NOT_ENABLED},
    {rildata::DataCallFailCause_t::IRAT_HANDOVER_FAILED, DataCallFailCause::IRAT_HANDOVER_FAILED},
    {rildata::DataCallFailCause_t::EMBMS_REGULAR_DEACTIVATION, DataCallFailCause::EMBMS_REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::TEST_LOOPBACK_REGULAR_DEACTIVATION, DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::LOWER_LAYER_REGISTRATION_FAILURE, DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE},
    {rildata::DataCallFailCause_t::DATA_PLAN_EXPIRED, DataCallFailCause::DATA_PLAN_EXPIRED},
    {rildata::DataCallFailCause_t::UMTS_HANDOVER_TO_IWLAN, DataCallFailCause::UMTS_HANDOVER_TO_IWLAN},
    {rildata::DataCallFailCause_t::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY, DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY},
    {rildata::DataCallFailCause_t::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE, DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::EVDO_HDR_CHANGED, DataCallFailCause::EVDO_HDR_CHANGED},
    {rildata::DataCallFailCause_t::EVDO_HDR_EXITED, DataCallFailCause::EVDO_HDR_EXITED},
    {rildata::DataCallFailCause_t::EVDO_HDR_NO_SESSION, DataCallFailCause::EVDO_HDR_NO_SESSION},
    {rildata::DataCallFailCause_t::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL, DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL},
    {rildata::DataCallFailCause_t::EVDO_HDR_CONNECTION_SETUP_TIMEOUT, DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT},
    {rildata::DataCallFailCause_t::FAILED_TO_ACQUIRE_COLOCATED_HDR, DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR},
    {rildata::DataCallFailCause_t::OTASP_COMMIT_IN_PROGRESS, DataCallFailCause::OTASP_COMMIT_IN_PROGRESS},
    {rildata::DataCallFailCause_t::NO_HYBRID_HDR_SERVICE, DataCallFailCause::NO_HYBRID_HDR_SERVICE},
    {rildata::DataCallFailCause_t::HDR_NO_LOCK_GRANTED, DataCallFailCause::HDR_NO_LOCK_GRANTED},
    {rildata::DataCallFailCause_t::DBM_OR_SMS_IN_PROGRESS, DataCallFailCause::DBM_OR_SMS_IN_PROGRESS},
    {rildata::DataCallFailCause_t::HDR_FADE, DataCallFailCause::HDR_FADE},
    {rildata::DataCallFailCause_t::HDR_ACCESS_FAILURE, DataCallFailCause::HDR_ACCESS_FAILURE},
    {rildata::DataCallFailCause_t::UNSUPPORTED_1X_PREV, DataCallFailCause::UNSUPPORTED_1X_PREV},
    {rildata::DataCallFailCause_t::LOCAL_END, DataCallFailCause::LOCAL_END},
    {rildata::DataCallFailCause_t::NO_SERVICE, DataCallFailCause::NO_SERVICE},
    {rildata::DataCallFailCause_t::FADE, DataCallFailCause::FADE},
    {rildata::DataCallFailCause_t::NORMAL_RELEASE, DataCallFailCause::NORMAL_RELEASE},
    {rildata::DataCallFailCause_t::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS, DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS},
    {rildata::DataCallFailCause_t::REDIRECTION_OR_HANDOFF_IN_PROGRESS, DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS},
    {rildata::DataCallFailCause_t::EMERGENCY_MODE, DataCallFailCause::EMERGENCY_MODE},
    {rildata::DataCallFailCause_t::PHONE_IN_USE, DataCallFailCause::PHONE_IN_USE},
    {rildata::DataCallFailCause_t::INVALID_MODE, DataCallFailCause::INVALID_MODE},
    {rildata::DataCallFailCause_t::INVALID_SIM_STATE, DataCallFailCause::INVALID_SIM_STATE},
    {rildata::DataCallFailCause_t::NO_COLLOCATED_HDR, DataCallFailCause::NO_COLLOCATED_HDR},
    {rildata::DataCallFailCause_t::UE_IS_ENTERING_POWERSAVE_MODE, DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE},
    {rildata::DataCallFailCause_t::DUAL_SWITCH, DataCallFailCause::DUAL_SWITCH},
    {rildata::DataCallFailCause_t::PPP_TIMEOUT, DataCallFailCause::PPP_TIMEOUT},
    {rildata::DataCallFailCause_t::PPP_AUTH_FAILURE, DataCallFailCause::PPP_AUTH_FAILURE},
    {rildata::DataCallFailCause_t::PPP_OPTION_MISMATCH, DataCallFailCause::PPP_OPTION_MISMATCH},
    {rildata::DataCallFailCause_t::PPP_PAP_FAILURE, DataCallFailCause::PPP_PAP_FAILURE},
    {rildata::DataCallFailCause_t::PPP_CHAP_FAILURE, DataCallFailCause::PPP_CHAP_FAILURE},
    {rildata::DataCallFailCause_t::PPP_CLOSE_IN_PROGRESS, DataCallFailCause::PPP_CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::LIMITED_TO_IPV4, DataCallFailCause::LIMITED_TO_IPV4},
    {rildata::DataCallFailCause_t::LIMITED_TO_IPV6, DataCallFailCause::LIMITED_TO_IPV6},
    {rildata::DataCallFailCause_t::VSNCP_TIMEOUT, DataCallFailCause::VSNCP_TIMEOUT},
    {rildata::DataCallFailCause_t::VSNCP_GEN_ERROR, DataCallFailCause::VSNCP_GEN_ERROR},
    {rildata::DataCallFailCause_t::VSNCP_APN_UNATHORIZED, DataCallFailCause::VSNCP_APN_UNAUTHORIZED},
    {rildata::DataCallFailCause_t::VSNCP_PDN_LIMIT_EXCEEDED, DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED},
    {rildata::DataCallFailCause_t::VSNCP_NO_PDN_GATEWAY_ADDRESS, DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS},
    {rildata::DataCallFailCause_t::VSNCP_PDN_GATEWAY_UNREACHABLE, DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE},
    {rildata::DataCallFailCause_t::VSNCP_PDN_GATEWAY_REJECT, DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT},
    {rildata::DataCallFailCause_t::VSNCP_INSUFFICIENT_PARAMETERS, DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS},
    {rildata::DataCallFailCause_t::VSNCP_RESOURCE_UNAVAILABLE, DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE},
    {rildata::DataCallFailCause_t::VSNCP_ADMINISTRATIVELY_PROHIBITED, DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED},
    {rildata::DataCallFailCause_t::VSNCP_PDN_ID_IN_USE, DataCallFailCause::VSNCP_PDN_ID_IN_USE},
    {rildata::DataCallFailCause_t::VSNCP_SUBSCRIBER_LIMITATION, DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION},
    {rildata::DataCallFailCause_t::VSNCP_PDN_EXISTS_FOR_THIS_APN, DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN},
    {rildata::DataCallFailCause_t::VSNCP_RECONNECT_NOT_ALLOWED, DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::IPV6_PREFIX_UNAVAILABLE, DataCallFailCause::IPV6_PREFIX_UNAVAILABLE},
    {rildata::DataCallFailCause_t::HANDOFF_PREFERENCE_CHANGED, DataCallFailCause::HANDOFF_PREFERENCE_CHANGED},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_1, DataCallFailCause::OEM_DCFAILCAUSE_1},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_2, DataCallFailCause::OEM_DCFAILCAUSE_2},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_3, DataCallFailCause::OEM_DCFAILCAUSE_3},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_4, DataCallFailCause::OEM_DCFAILCAUSE_4},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_5, DataCallFailCause::OEM_DCFAILCAUSE_5},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_6, DataCallFailCause::OEM_DCFAILCAUSE_6},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_7, DataCallFailCause::OEM_DCFAILCAUSE_7},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_8, DataCallFailCause::OEM_DCFAILCAUSE_8},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_9, DataCallFailCause::OEM_DCFAILCAUSE_9},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_10, DataCallFailCause::OEM_DCFAILCAUSE_10},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_11, DataCallFailCause::OEM_DCFAILCAUSE_11},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_12, DataCallFailCause::OEM_DCFAILCAUSE_12},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_13, DataCallFailCause::OEM_DCFAILCAUSE_13},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_14, DataCallFailCause::OEM_DCFAILCAUSE_14},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_15, DataCallFailCause::OEM_DCFAILCAUSE_15},
    {rildata::DataCallFailCause_t::VOICE_REGISTRATION_FAIL, DataCallFailCause::VOICE_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::DATA_REGISTRATION_FAIL, DataCallFailCause::DATA_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::SIGNAL_LOST, DataCallFailCause::SIGNAL_LOST},
    {rildata::DataCallFailCause_t::PREF_RADIO_TECH_CHANGED, DataCallFailCause::PREF_RADIO_TECH_CHANGED},
    {rildata::DataCallFailCause_t::RADIO_POWER_OFF, DataCallFailCause::RADIO_POWER_OFF},
    {rildata::DataCallFailCause_t::TETHERED_CALL_ACTIVE, DataCallFailCause::TETHERED_CALL_ACTIVE},
    {rildata::DataCallFailCause_t::ERROR_UNSPECIFIED, DataCallFailCause::ERROR_UNSPECIFIED},
    {rildata::DataCallFailCause_t::SLICE_REJECTED, DataCallFailCause::SLICE_REJECTED},
    {rildata::DataCallFailCause_t::MATCH_ALL_RULE_NOT_ALLOWED, DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::ALL_MATCHING_RULES_FAILED, DataCallFailCause::ALL_MATCHING_RULES_FAILED}
};

DataCallFailCause convertDcFailCauseToAidlDcFailCause(const rildata::DataCallFailCause_t &cause) {
   DataCallFailCause aidlCause = DataCallFailCause::ERROR_UNSPECIFIED;
    if (dcFailCauseMap.find(cause) != dcFailCauseMap.end()) {
        aidlCause = dcFailCauseMap[cause];
    }
    return aidlCause;
}

PdpProtocolType convertStringToPdpProtocolType(const string &type) {
    PdpProtocolType protocolType = PdpProtocolType::UNKNOWN;

    if (type.compare("IP") == 0) {
        protocolType = PdpProtocolType::IP;
    }
    else if (type.compare("IPV6") == 0) {
        protocolType = PdpProtocolType::IPV6;
    }
    else if (type.compare("IPV4V6") == 0) {
        protocolType = PdpProtocolType::IPV4V6;
    }
    else if (type.compare("PPP") == 0) {
        protocolType = PdpProtocolType::PPP;
    }
    else if (type.compare("NON_IP") == 0) {
        protocolType = PdpProtocolType::NON_IP;
    }
    else if (type.compare("UNSTRUCTURED") == 0) {
        protocolType = PdpProtocolType::UNSTRUCTURED;
    }
    else {
        protocolType = PdpProtocolType::UNKNOWN;
    }

    return protocolType;
}

PdpProtocolType convertToPdpProtocolType(const string protocol) {
    string protocolType;
    if(protocol == "IP")
        return PdpProtocolType::IP;
    else if(protocol == "IPV6")
        return PdpProtocolType::IPV6;
    else if(protocol == "IPV4V6")
        return PdpProtocolType::IPV4V6;
    else if(protocol == "PPP")
        return PdpProtocolType::PPP;
    else
        return PdpProtocolType::UNKNOWN;
}

vector<LinkAddress> convertLinkAddressToAidlVector(vector<rildata::LinkAddress_t> in_addresses) {
   vector<LinkAddress> aidlAddressesVector;
   for (rildata::LinkAddress_t addr : in_addresses) {
        LinkAddress aidlAddr = {
            .address = addr.address,
            .addressProperties = (int)addr.properties,
            .deprecationTime = (long)addr.deprecationTime,
            .expirationTime = (long)addr.expirationTime,
        };
        aidlAddressesVector.push_back(aidlAddr);
    }
    return aidlAddressesVector;
}

void convertQosFilters(QosFilter& dcQosFilter, const rildata::QosFilter_t& result) {
  unsigned int localAddressLen = result.localAddresses.size();
  if(localAddressLen > 0) {
    dcQosFilter.localAddresses.resize(localAddressLen);
    for(int i =0; i< localAddressLen; i++) {
      dcQosFilter.localAddresses[i]=result.localAddresses[i];
    }
  }

  unsigned int remoteAddressesLen = result.remoteAddresses.size();
  if(remoteAddressesLen > 0) {
    dcQosFilter.remoteAddresses.resize(remoteAddressesLen);
    for(int i =0; i< remoteAddressesLen; i++) {
      dcQosFilter.remoteAddresses[i]=result.remoteAddresses[i];
    }
  }
  if(result.localPort != std::nullopt) {
    PortRange range;
    range.start = result.localPort->start;
    range.end = result.localPort->end;
    dcQosFilter.localPort = range;
  }
  if(result.remotePort != std::nullopt) {
    PortRange range;
    range.start = result.remotePort->start;
    range.end = result.remotePort->end;
    dcQosFilter.remotePort = range;
  }
  dcQosFilter.protocol = (uint8_t)result.protocol;
  if(result.tos.value != std::nullopt) {
    QosFilterTypeOfService tos;
    tos = (int8_t)(*result.tos.value);
    dcQosFilter.tos = tos;
  }

  if(result.flowLabel.value != std::nullopt) {
    QosFilterIpv6FlowLabel flowLabel;
    flowLabel = (int)(*result.flowLabel.value);
    dcQosFilter.flowLabel = flowLabel;
  }

  if(result.spi.value != std::nullopt) {
    QosFilterIpsecSpi spi;
    spi = (int)(*result.spi.value);
    dcQosFilter.spi = spi;
  }

  dcQosFilter.direction = (uint8_t)result.direction;
  dcQosFilter.precedence= result.precedence;
}

void convertQosSession(QosSession& dcQosSession, const rildata::QosSession_t& result) {
  dcQosSession.qosSessionId = result.qosSessionId;
  if(result.qos.epsQos != std::nullopt) {
    EpsQos eps;
    eps.qci = result.qos.epsQos->qci;
    eps.downlink.maxBitrateKbps = result.qos.epsQos->downlink.maxBitrateKbps;
    eps.downlink.guaranteedBitrateKbps = result.qos.epsQos->downlink.guaranteedBitrateKbps;
    eps.uplink.maxBitrateKbps = result.qos.epsQos->uplink.maxBitrateKbps;
    eps.uplink.guaranteedBitrateKbps = result.qos.epsQos->uplink.guaranteedBitrateKbps;
    dcQosSession.qos = eps;
  }
  if(result.qos.nrQos != std::nullopt) {
    NrQos nr;
    nr.downlink.maxBitrateKbps = result.qos.nrQos->downlink.maxBitrateKbps;
    nr.downlink.guaranteedBitrateKbps = result.qos.nrQos->downlink.guaranteedBitrateKbps;
    nr.uplink.maxBitrateKbps = result.qos.nrQos->uplink.maxBitrateKbps;
    nr.uplink.guaranteedBitrateKbps = result.qos.nrQos->uplink.guaranteedBitrateKbps;
    nr.qfi = result.qos.nrQos->qfi;
    nr.averagingWindowMs = result.qos.nrQos->averagingWindowMs;
    dcQosSession.qos = nr;
  }

  unsigned int qosFilterLen = result.qosFilters.size();
  if(qosFilterLen > 0 ) {
    dcQosSession.qosFilters.resize(qosFilterLen);
    for(int j=0; j< qosFilterLen; j++) {
      convertQosFilters(dcQosSession.qosFilters[j], result.qosFilters[j]);
    }
  }
}

void convertTrafficDescriptor(TrafficDescriptor& trafficDescriptor, const rildata::TrafficDescriptor_t& result)
{
  if(result.dnn.has_value())
    trafficDescriptor.dnn = result.dnn.value();

  if (result.osAppId.has_value()) {
    OsAppId id;
    id.osAppId = result.osAppId.value();
    trafficDescriptor.osAppId = id;
  }
}

void convertFromAidlTrafficDescriptor(std::optional<rildata::TrafficDescriptor_t>& out_td, const std::optional<::aidl::android::hardware::radio::data::TrafficDescriptor>& in_td) {
  if (in_td.has_value()) {
    convertFromAidlTrafficDescriptor(out_td, *in_td);
  }
}

void convertFromAidlTrafficDescriptor(std::optional<rildata::TrafficDescriptor_t>& out_td, const ::aidl::android::hardware::radio::data::TrafficDescriptor& in_td) {
  rildata::TrafficDescriptor_t td;
  convertFromAidlTrafficDescriptor(td, in_td);
  out_td = td;
}

void convertFromAidlTrafficDescriptor(rildata::TrafficDescriptor_t& out_td, const ::aidl::android::hardware::radio::data::TrafficDescriptor& in_td)
{
  if (in_td.dnn.has_value()) {
    out_td.dnn = in_td.dnn;
  }
  if(in_td.osAppId.has_value()) {
    std::vector<uint8_t> osAppId (in_td.osAppId.value().osAppId.begin(),
                                  in_td.osAppId.value().osAppId.end());
    out_td.osAppId = osAppId;
  }
}

vector<string> convertAddrStringToAidlStringVector(const string &addr) {
    string tmpString;
    char delimiter = 0x20;
    vector<string> aidlAddressesVector;
    stringstream ssAddresses(addr);
    while(getline(ssAddresses, tmpString, delimiter)) {
        aidlAddressesVector.push_back(tmpString);
    }
    return aidlAddressesVector;
}

SetupDataCallResult convertDcResultToAidlDcResult(const rildata::DataCallResult_t& result) {
    SetupDataCallResult dcResult = {};
    dcResult.cause = convertDcFailCauseToAidlDcFailCause(result.cause);
    dcResult.suggestedRetryTime = result.suggestedRetryTime;
    dcResult.cid = result.cid;
    dcResult.active = result.active;
    dcResult.type = convertStringToPdpProtocolType(result.type);
    dcResult.ifname = result.ifname;
    dcResult.addresses = convertLinkAddressToAidlVector(result.linkAddresses);
    dcResult.dnses = convertAddrStringToAidlStringVector(result.dnses);
    dcResult.gateways = convertAddrStringToAidlStringVector(result.gateways);
    dcResult.pcscf = convertAddrStringToAidlStringVector(result.pcscf);
    dcResult.mtuV4 = result.mtuV4;
    dcResult.mtuV6 = result.mtuV6;
    dcResult.handoverFailureMode = (uint8_t)result.handoverFailureMode;
    unsigned int qosSessionLen = result.qosSessions.size();
    if(qosSessionLen > 0) {
      dcResult.qosSessions.resize(qosSessionLen);
      for(int i = 0; i< qosSessionLen; i++)
      {
        convertQosSession(dcResult.qosSessions[i], result.qosSessions[i]);
      }
    }
    unsigned int tdSize = result.trafficDescriptors.size();
    if(tdSize > 0) {
      dcResult.trafficDescriptors.resize(tdSize);
      for(int i = 0; i< tdSize; i++) {
        convertTrafficDescriptor(dcResult.trafficDescriptors[i], result.trafficDescriptors[i]);
      }
    }

    return dcResult;
}

rildata::AccessNetwork_t convertAidlAccessNetworkToDataAccessNetwork(AccessNetwork ran) {
    rildata::AccessNetwork_t ret;
    switch(ran) {
        case AccessNetwork::GERAN:
            ret = rildata::AccessNetwork_t::GERAN;
            break;
        case AccessNetwork::UTRAN:
            ret = rildata::AccessNetwork_t::UTRAN;
            break;
        case AccessNetwork::EUTRAN:
            ret = rildata::AccessNetwork_t::EUTRAN;
            break;
        case AccessNetwork::CDMA2000:
            ret = rildata::AccessNetwork_t::CDMA;
            break;
        case AccessNetwork::IWLAN:
            ret = rildata::AccessNetwork_t::IWLAN;
            break;
        case AccessNetwork::NGRAN:
            ret = rildata::AccessNetwork_t::NGRAN;
            break;
        default:
            ret = rildata::AccessNetwork_t::UNKNOWN;
            break;
    }
    return ret;
}

string convertPdpProtocolTypeToString(const PdpProtocolType protocol) {
    string protocolType;
    switch(protocol) {
        case PdpProtocolType::IP:
        protocolType = string("IP");
        break;

        case PdpProtocolType::IPV6:
        protocolType = string("IPV6");
        break;

        case PdpProtocolType::IPV4V6:
        protocolType = string("IPV4V6");
        break;

        case PdpProtocolType::PPP:
        protocolType = string("PPP");
        break;

        case PdpProtocolType::UNKNOWN:
        case PdpProtocolType::NON_IP:
        case PdpProtocolType::UNSTRUCTURED:
        default:
        protocolType = string("UNKNOWN");
        break;
    }
    return protocolType;
}

rildata::DataProfileInfo_t convertAidlDataProfileInfoToRil(const DataProfileInfo& profile) {
    rildata::DataProfileInfo_t rilProfile = {};
    rilProfile.profileId = (rildata::DataProfileId_t)profile.profileId;
    rilProfile.apn = profile.apn;
    rilProfile.protocol = convertPdpProtocolTypeToString(profile.protocol);
    rilProfile.homeProtocol = convertPdpProtocolTypeToString(profile.protocol);
    rilProfile.roamingProtocol = convertPdpProtocolTypeToString(profile.roamingProtocol);
    rilProfile.authType = (rildata::ApnAuthType_t)profile.authType;
    rilProfile.username = profile.user;
    rilProfile.password = profile.password;
    rilProfile.dataProfileInfoType = (rildata::DataProfileInfoType_t)profile.type,
    rilProfile.maxConnsTime = profile.maxConnsTime;
    rilProfile.maxConns = profile.maxConns;
    rilProfile.waitTime = profile.waitTime;
    rilProfile.enableProfile = profile.enabled;
    rilProfile.supportedApnTypesBitmap = (rildata::ApnTypes_t)profile.supportedApnTypesBitmap;
    rilProfile.bearerBitmap = (rildata::RadioAccessFamily_t)profile.bearerBitmap;
    rilProfile.mtuV4 = profile.mtuV4;
    rilProfile.mtuV6 = profile.mtuV6;
    rilProfile.preferred = profile.preferred;
    rilProfile.persistent = profile.persistent;
    convertFromAidlTrafficDescriptor(rilProfile.trafficDescriptor, profile.trafficDescriptor);
    return rilProfile;
}

DataProfileInfo convertToAidlDataProfileInfo(rildata::DataProfileInfo_t in_profile) {
    DataProfileInfo out_profile;
    out_profile.profileId = (int)in_profile.profileId;
    out_profile.apn = in_profile.apn;
    out_profile.protocol = convertToPdpProtocolType(in_profile.homeProtocol);
    out_profile.roamingProtocol = convertToPdpProtocolType(in_profile.roamingProtocol);
    out_profile.authType = (ApnAuthType)in_profile.authType;
    out_profile.user = in_profile.username;
    out_profile.password = in_profile.password;
    out_profile.type = (int)in_profile.dataProfileInfoType;
    out_profile.maxConnsTime = in_profile.maxConnsTime;
    out_profile.maxConns = in_profile.maxConns;
    out_profile.waitTime = in_profile.waitTime;
    out_profile.enabled = in_profile.enableProfile;
    out_profile.supportedApnTypesBitmap = (int)in_profile.supportedApnTypesBitmap;
    out_profile.bearerBitmap = (int)in_profile.bearerBitmap;
    out_profile.mtuV4 = in_profile.mtuV4;
    out_profile.mtuV6 = in_profile.mtuV6;
    out_profile.preferred = in_profile.preferred;
    out_profile.persistent = in_profile.persistent;
    if(in_profile.trafficDescriptor.has_value())
    {
        TrafficDescriptor td;
        convertTrafficDescriptor(td, *in_profile.trafficDescriptor);
        out_profile.trafficDescriptor = td;
    }
    return out_profile;
}

RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e) {
  RadioError ret = RadioError::INTERNAL_ERR;
  switch (status) {
    case Message::Callback::Status::FAILURE:
    case Message::Callback::Status::SUCCESS:
      ret = (RadioError)e;
      break;
    case Message::Callback::Status::CANCELLED:
      ret = RadioError::CANCELLED;
      break;
    case Message::Callback::Status::NO_HANDLER_FOUND:
      ret = RadioError::REQUEST_NOT_SUPPORTED;
      break;
    default:
      ret = RadioError::INTERNAL_ERR;
      break;
  }
  return ret;
}

rildata::DataThrottleAction_t convertAidlDataThrottleActionToRil (const DataThrottlingAction& dataThrottlingAction) {
  rildata::DataThrottleAction_t action;
  switch (dataThrottlingAction) {
      case DataThrottlingAction::NO_DATA_THROTTLING:
          action = rildata::DataThrottleAction_t::NoDataThrottle;
          break;
      case DataThrottlingAction::THROTTLE_SECONDARY_CARRIER:
          action = rildata::DataThrottleAction_t::ThrottleSecondaryCarrier;
          break;
      case DataThrottlingAction::THROTTLE_ANCHOR_CARRIER:
          action = rildata::DataThrottleAction_t::ThrottleAnchorCarrier;
          break;
      case DataThrottlingAction::HOLD:
      default:
          action = rildata::DataThrottleAction_t::Hold;
          break;
  }
  return action;
}

void convertRouteSelectionDescriptor(RouteSelectionDescriptor out, rildata::RouteSelectionDescriptor_t in)
{
    out.precedence = in.precedence;
    out.sessionType = (PdpProtocolType)in.sessionType;
    out.sscMode = in.sscMode;
    out.dnn = in.dnn;
    int i =0;
    for(auto it = in.sliceInfo.begin(); it != in.sliceInfo.end(); it++)
    {
      out.sliceInfo[i].sliceServiceType = (uint8_t)((*it).sliceServiceType);
      out.sliceInfo[i].sliceDifferentiator = ((*it).sliceDifferentiator);
      out.sliceInfo[i].mappedHplmnSst = (uint8_t)((*it).mappedHplmnSst);
      out.sliceInfo[i].mappedHplmnSd = ((*it).mappedHplmnSd);
      out.sliceInfo[i].status = (uint8_t)((*it).status);
      i++;
    }
}

SlicingConfig convertToAidlSlicingConfig(rildata::SlicingConfig_t slicingConfig)
{
    SlicingConfig config;
    for(auto it : slicingConfig.urspRules)
    {
      UrspRule rule;
      rule.precedence = (it).precedence;
      for(auto td : (it).trafficDescriptors)
      {
        TrafficDescriptor trafficDesc;
        convertTrafficDescriptor(trafficDesc, (td));
        rule.trafficDescriptors.push_back(trafficDesc);
      }
      for(auto rd : (it).routeSelectionDescriptors)
      {
        RouteSelectionDescriptor routeDesc;
        convertRouteSelectionDescriptor(routeDesc, (rd));
        rule.routeSelectionDescriptor.push_back(routeDesc);
      }
      config.urspRules.push_back(rule);
    }

    for(auto it : slicingConfig.sliceInfos)
    {
      SliceInfo slice;
      slice.sliceServiceType = (uint8_t)((it).sliceServiceType);
      slice.sliceDifferentiator = (it).sliceDifferentiator;
      slice.mappedHplmnSst = (uint8_t)((it).mappedHplmnSst);
      slice.mappedHplmnSd = (it).mappedHplmnSd;
      slice.status = (uint8_t)((it).status);
      config.sliceInfo.push_back(slice);
    }

    return config;
}

RadioError convertMsgToRadioError(Message::Callback::Status &status, rildata::ResponseError_t &respErr) {
    RadioError ret = RadioError::INTERNAL_ERR;
    if (status == Message::Callback::Status::SUCCESS) {
      switch (respErr) {
        case rildata::ResponseError_t::NO_ERROR: ret = RadioError::NONE; break;
        case rildata::ResponseError_t::NOT_SUPPORTED: ret = RadioError::REQUEST_NOT_SUPPORTED; break;
        case rildata::ResponseError_t::INVALID_ARGUMENT: ret = RadioError::INVALID_ARGUMENTS; break;
        default: break;
      }
    }
    return ret;
}

}//namespace utils
