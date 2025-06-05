/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
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

#include "hidl_impl/1.4/radio_service_utils_1_4.h"
#include "hidl_impl/1.2/radio_service_utils_1_2.h"
#include "hidl_impl/1.1/radio_service_utils_1_1.h"
#include "hidl_impl/1.0/radio_service_utils_1_0.h"

#undef TAG
#define TAG "RILQ"

using namespace std;

namespace android {
namespace hardware {
namespace radio {
namespace utils {

union CellInfoUnion_1_4 {
  V1_2::CellInfoGsm gsm;
  V1_2::CellInfoCdma cdma;
  V1_2::CellInfoWcdma wcdma;
  V1_2::CellInfoTdscdma tdscdma;
  V1_4::CellInfoLte lte;
  V1_4::CellInfoNr nr;

  CellInfoUnion_1_4() {
  }
  ~CellInfoUnion_1_4() {
  }
};

unordered_map<rildata::DataCallFailCause_t, V1_4::DataCallFailCause> dcFailCauseMap = {
    {rildata::DataCallFailCause_t::NONE, V1_4::DataCallFailCause::NONE},
    {rildata::DataCallFailCause_t::OPERATOR_BARRED, V1_4::DataCallFailCause::OPERATOR_BARRED},
    {rildata::DataCallFailCause_t::NAS_SIGNALLING, V1_4::DataCallFailCause::NAS_SIGNALLING},
    {rildata::DataCallFailCause_t::LLC_SNDCP, V1_4::DataCallFailCause::LLC_SNDCP},
    {rildata::DataCallFailCause_t::INSUFFICIENT_RESOURCES, V1_4::DataCallFailCause::INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MISSING_UKNOWN_APN, V1_4::DataCallFailCause::MISSING_UKNOWN_APN},
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_ADDRESS_TYPE, V1_4::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE},
    {rildata::DataCallFailCause_t::USER_AUTHENTICATION, V1_4::DataCallFailCause::USER_AUTHENTICATION},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_GGSN, V1_4::DataCallFailCause::ACTIVATION_REJECT_GGSN},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_UNSPECIFIED, V1_4::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUPPORTED, V1_4::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUBSCRIBED, V1_4::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_OUT_OF_ORDER, V1_4::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER},
    {rildata::DataCallFailCause_t::NSAPI_IN_USE, V1_4::DataCallFailCause::NSAPI_IN_USE},
    {rildata::DataCallFailCause_t::REGULAR_DEACTIVATION, V1_4::DataCallFailCause::REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::QOS_NOT_ACCEPTED, V1_4::DataCallFailCause::QOS_NOT_ACCEPTED},
    {rildata::DataCallFailCause_t::NETWORK_FAILURE, V1_4::DataCallFailCause::NETWORK_FAILURE},
    {rildata::DataCallFailCause_t::UMTS_REACTIVATION_REQ, V1_4::DataCallFailCause::UMTS_REACTIVATION_REQ},
    {rildata::DataCallFailCause_t::FEATURE_NOT_SUPP, V1_4::DataCallFailCause::FEATURE_NOT_SUPP},
    {rildata::DataCallFailCause_t::TFT_SEMANTIC_ERROR, V1_4::DataCallFailCause::TFT_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::TFT_SYTAX_ERROR, V1_4::DataCallFailCause::TFT_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_CONTEXT, V1_4::DataCallFailCause::UNKNOWN_PDP_CONTEXT},
    {rildata::DataCallFailCause_t::FILTER_SEMANTIC_ERROR, V1_4::DataCallFailCause::FILTER_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::FILTER_SYTAX_ERROR, V1_4::DataCallFailCause::FILTER_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::PDP_WITHOUT_ACTIVE_TFT, V1_4::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECTED_BCM_VIOLATION, V1_4::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION},
    {rildata::DataCallFailCause_t::ONLY_IPV4_ALLOWED, V1_4::DataCallFailCause::ONLY_IPV4_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_IPV6_ALLOWED, V1_4::DataCallFailCause::ONLY_IPV6_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_SINGLE_BEARER_ALLOWED, V1_4::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED},
    {rildata::DataCallFailCause_t::ESM_INFO_NOT_RECEIVED, V1_4::DataCallFailCause::ESM_INFO_NOT_RECEIVED},
    {rildata::DataCallFailCause_t::PDN_CONN_DOES_NOT_EXIST, V1_4::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST},
    {rildata::DataCallFailCause_t::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED, V1_4::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::COLLISION_WITH_NETWORK_INITIATED_REQUEST, V1_4::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST},
    {rildata::DataCallFailCause_t::ONLY_IPV4V6_ALLOWED, V1_4::DataCallFailCause::ONLY_IPV4V6_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_NON_IP_ALLOWED, V1_4::DataCallFailCause::ONLY_NON_IP_ALLOWED},
    {rildata::DataCallFailCause_t::UNSUPPORTED_QCI_VALUE, V1_4::DataCallFailCause::UNSUPPORTED_QCI_VALUE},
    {rildata::DataCallFailCause_t::BEARER_HANDLING_NOT_SUPPORTED, V1_4::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::MAX_ACTIVE_PDP_CONTEXT_REACHED, V1_4::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED},
    {rildata::DataCallFailCause_t::UNSUPPORTED_APN_IN_CURRENT_PLMN, V1_4::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN},
    {rildata::DataCallFailCause_t::INVALID_TRANSACTION_ID, V1_4::DataCallFailCause::INVALID_TRANSACTION_ID},
    {rildata::DataCallFailCause_t::MESSAGE_INCORRECT_SEMANTIC, V1_4::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC},
    {rildata::DataCallFailCause_t::INVALID_MANDATORY_INFO, V1_4::DataCallFailCause::INVALID_MANDATORY_INFO},
    {rildata::DataCallFailCause_t::MESSAGE_TYPE_UNSUPPORTED, V1_4::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED},
    {rildata::DataCallFailCause_t::MSG_TYPE_NONCOMPATIBLE_STATE, V1_4::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE},
    {rildata::DataCallFailCause_t::UNKNOWN_INFO_ELEMENT, V1_4::DataCallFailCause::UNKNOWN_INFO_ELEMENT},
    {rildata::DataCallFailCause_t::CONDITIONAL_IE_ERROR, V1_4::DataCallFailCause::CONDITIONAL_IE_ERROR},
    {rildata::DataCallFailCause_t::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE, V1_4::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE},
    {rildata::DataCallFailCause_t::PROTOCOL_ERRORS, V1_4::DataCallFailCause::PROTOCOL_ERRORS},
    {rildata::DataCallFailCause_t::APN_TYPE_CONFLICT, V1_4::DataCallFailCause::APN_TYPE_CONFLICT},
    {rildata::DataCallFailCause_t::INVALID_PCSCF_ADDR, V1_4::DataCallFailCause::INVALID_PCSCF_ADDR},
    {rildata::DataCallFailCause_t::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN, V1_4::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED, V1_4::DataCallFailCause::EMM_ACCESS_BARRED},
    {rildata::DataCallFailCause_t::EMERGENCY_IFACE_ONLY, V1_4::DataCallFailCause::EMERGENCY_IFACE_ONLY},
    {rildata::DataCallFailCause_t::IFACE_MISMATCH, V1_4::DataCallFailCause::IFACE_MISMATCH},
    {rildata::DataCallFailCause_t::COMPANION_IFACE_IN_USE, V1_4::DataCallFailCause::COMPANION_IFACE_IN_USE},
    {rildata::DataCallFailCause_t::IP_ADDRESS_MISMATCH, V1_4::DataCallFailCause::IP_ADDRESS_MISMATCH},
    {rildata::DataCallFailCause_t::IFACE_AND_POL_FAMILY_MISMATCH, V1_4::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED_INFINITE_RETRY, V1_4::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY},
    {rildata::DataCallFailCause_t::AUTH_FAILURE_ON_EMERGENCY_CALL, V1_4::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL},
    {rildata::DataCallFailCause_t::INVALID_DNS_ADDR, V1_4::DataCallFailCause::INVALID_DNS_ADDR},
    {rildata::DataCallFailCause_t::MIP_FA_REASON_UNSPECIFIED, V1_4::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED},
    {rildata::DataCallFailCause_t::MIP_FA_ADMIN_PROHIBITED, V1_4::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED},
    {rildata::DataCallFailCause_t::MIP_FA_INSUFFICIENT_RESOURCES, V1_4::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE, V1_4::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE, V1_4::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_FA_REQUESTED_LIFETIME_TOO_LONG, V1_4::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG},
    {rildata::DataCallFailCause_t::MIP_FA_MALFORMED_REQUEST, V1_4::DataCallFailCause::MIP_FA_MALFORMED_REQUEST},
    {rildata::DataCallFailCause_t::MIP_FA_MALFORMED_REPLY, V1_4::DataCallFailCause::MIP_FA_MALFORMED_REPLY},
    {rildata::DataCallFailCause_t::MIP_FA_ENCAPSULATION_UNAVAILABLE, V1_4::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE, V1_4::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE, V1_4::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY, V1_4::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY},
    {rildata::DataCallFailCause_t::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED, V1_4::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_NAI, V1_4::DataCallFailCause::MIP_FA_MISSING_NAI},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_HOME_AGENT, V1_4::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_HOME_ADDRESS, V1_4::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS},
    {rildata::DataCallFailCause_t::MIP_FA_UNKNOWN_CHALLENGE, V1_4::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_FA_MISSING_CHALLENGE, V1_4::DataCallFailCause::MIP_FA_MISSING_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_FA_STALE_CHALLENGE, V1_4::DataCallFailCause::MIP_FA_STALE_CHALLENGE},
    {rildata::DataCallFailCause_t::MIP_HA_REASON_UNSPECIFIED, V1_4::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED},
    {rildata::DataCallFailCause_t::MIP_HA_ADMIN_PROHIBITED, V1_4::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED},
    {rildata::DataCallFailCause_t::MIP_HA_INSUFFICIENT_RESOURCES, V1_4::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE, V1_4::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE, V1_4::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::MIP_HA_REGISTRATION_ID_MISMATCH, V1_4::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH},
    {rildata::DataCallFailCause_t::MIP_HA_MALFORMED_REQUEST, V1_4::DataCallFailCause::MIP_HA_MALFORMED_REQUEST},
    {rildata::DataCallFailCause_t::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS, V1_4::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS},
    {rildata::DataCallFailCause_t::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE, V1_4::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE},
    {rildata::DataCallFailCause_t::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY, V1_4::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY},
    {rildata::DataCallFailCause_t::MIP_HA_ENCAPSULATION_UNAVAILABLE, V1_4::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE},
    {rildata::DataCallFailCause_t::CLOSE_IN_PROGRESS, V1_4::DataCallFailCause::CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_TERMINATION, V1_4::DataCallFailCause::NETWORK_INITIATED_TERMINATION},
    {rildata::DataCallFailCause_t::MODEM_APP_PREEMPTED, V1_4::DataCallFailCause::MODEM_APP_PREEMPTED},
    {rildata::DataCallFailCause_t::PDN_IPV4_CALL_DISALLOWED, V1_4::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::PDN_IPV4_CALL_THROTTLED, V1_4::DataCallFailCause::PDN_IPV4_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::PDN_IPV6_CALL_DISALLOWED, V1_4::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::PDN_IPV6_CALL_THROTTLED, V1_4::DataCallFailCause::PDN_IPV6_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::MODEM_RESTART, V1_4::DataCallFailCause::MODEM_RESTART},
    {rildata::DataCallFailCause_t::PDP_PPP_NOT_SUPPORTED, V1_4::DataCallFailCause::PDP_PPP_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::UNPREFERRED_RAT, V1_4::DataCallFailCause::UNPREFERRED_RAT},
    {rildata::DataCallFailCause_t::PHYSICAL_LINK_CLOSE_IN_PROGRESS, V1_4::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::APN_PENDING_HANDOVER, V1_4::DataCallFailCause::APN_PENDING_HANDOVER},
    {rildata::DataCallFailCause_t::PROFILE_BEARER_INCOMPATIBLE, V1_4::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE},
    {rildata::DataCallFailCause_t::SIM_CARD_CHANGED, V1_4::DataCallFailCause::SIM_CARD_CHANGED},
    {rildata::DataCallFailCause_t::LOW_POWER_MODE_OR_POWERING_DOWN, V1_4::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN},
    {rildata::DataCallFailCause_t::APN_DISABLED, V1_4::DataCallFailCause::APN_DISABLED},
    {rildata::DataCallFailCause_t::MAX_PPP_INACTIVITY_TIMER_EXPIRED, V1_4::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::IPV6_ADDRESS_TRANSFER_FAILED, V1_4::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED},
    {rildata::DataCallFailCause_t::TRAT_SWAP_FAILED, V1_4::DataCallFailCause::TRAT_SWAP_FAILED},
    {rildata::DataCallFailCause_t::EHRPD_TO_HRPD_FALLBACK, V1_4::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK},
    {rildata::DataCallFailCause_t::MIP_CONFIG_FAILURE, V1_4::DataCallFailCause::MIP_CONFIG_FAILURE},
    {rildata::DataCallFailCause_t::PDN_INACTIVITY_TIMER_EXPIRED, V1_4::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::MAX_IPV4_CONNECTIONS, V1_4::DataCallFailCause::MAX_IPV4_CONNECTIONS},
    {rildata::DataCallFailCause_t::MAX_IPV6_CONNECTIONS, V1_4::DataCallFailCause::MAX_IPV6_CONNECTIONS},
    {rildata::DataCallFailCause_t::APN_MISMATCH, V1_4::DataCallFailCause::APN_MISMATCH},
    {rildata::DataCallFailCause_t::IP_VERSION_MISMATCH, V1_4::DataCallFailCause::IP_VERSION_MISMATCH},
    {rildata::DataCallFailCause_t::DUN_CALL_DISALLOWED, V1_4::DataCallFailCause::DUN_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::INTERNAL_EPC_NONEPC_TRANSITION, V1_4::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION},
    {rildata::DataCallFailCause_t::INTERFACE_IN_USE, V1_4::DataCallFailCause::INTERFACE_IN_USE},
    {rildata::DataCallFailCause_t::APN_DISALLOWED_ON_ROAMING, V1_4::DataCallFailCause::APN_DISALLOWED_ON_ROAMING},
    {rildata::DataCallFailCause_t::APN_PARAMETERS_CHANGED, V1_4::DataCallFailCause::APN_PARAMETERS_CHANGED},
    {rildata::DataCallFailCause_t::NULL_APN_DISALLOWED, V1_4::DataCallFailCause::NULL_APN_DISALLOWED},
    {rildata::DataCallFailCause_t::THERMAL_MITIGATION, V1_4::DataCallFailCause::THERMAL_MITIGATION},
    {rildata::DataCallFailCause_t::DATA_SETTINGS_DISABLED, V1_4::DataCallFailCause::DATA_SETTINGS_DISABLED},
    {rildata::DataCallFailCause_t::DATA_ROAMING_SETTINGS_DISABLED, V1_4::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED},
    {rildata::DataCallFailCause_t::DDS_SWITCHED, V1_4::DataCallFailCause::DDS_SWITCHED},
    {rildata::DataCallFailCause_t::FORBIDDEN_APN_NAME, V1_4::DataCallFailCause::FORBIDDEN_APN_NAME},
    {rildata::DataCallFailCause_t::DDS_SWITCH_IN_PROGRESS, V1_4::DataCallFailCause::DDS_SWITCH_IN_PROGRESS},
    {rildata::DataCallFailCause_t::CALL_DISALLOWED_IN_ROAMING, V1_4::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING},
    {rildata::DataCallFailCause_t::NON_IP_NOT_SUPPORTED, V1_4::DataCallFailCause::NON_IP_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::PDN_NON_IP_CALL_THROTTLED, V1_4::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED},
    {rildata::DataCallFailCause_t::PDN_NON_IP_CALL_DISALLOWED, V1_4::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED},
    {rildata::DataCallFailCause_t::CDMA_LOCK, V1_4::DataCallFailCause::CDMA_LOCK},
    {rildata::DataCallFailCause_t::CDMA_INTERCEPT, V1_4::DataCallFailCause::CDMA_INTERCEPT},
    {rildata::DataCallFailCause_t::CDMA_REORDER, V1_4::DataCallFailCause::CDMA_REORDER},
    {rildata::DataCallFailCause_t::CDMA_RELEASE_DUE_TO_SO_REJECTION, V1_4::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION},
    {rildata::DataCallFailCause_t::CDMA_INCOMING_CALL, V1_4::DataCallFailCause::CDMA_INCOMING_CALL},
    {rildata::DataCallFailCause_t::CDMA_ALERT_STOP, V1_4::DataCallFailCause::CDMA_ALERT_STOP},
    {rildata::DataCallFailCause_t::CHANNEL_ACQUISITION_FAILURE, V1_4::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE},
    {rildata::DataCallFailCause_t::MAX_ACCESS_PROBE, V1_4::DataCallFailCause::MAX_ACCESS_PROBE},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION, V1_4::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION},
    {rildata::DataCallFailCause_t::NO_RESPONSE_FROM_BASE_STATION, V1_4::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION},
    {rildata::DataCallFailCause_t::REJECTED_BY_BASE_STATION, V1_4::DataCallFailCause::REJECTED_BY_BASE_STATION},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICES_INCOMPATIBLE, V1_4::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE},
    {rildata::DataCallFailCause_t::NO_CDMA_SERVICE, V1_4::DataCallFailCause::NO_CDMA_SERVICE},
    {rildata::DataCallFailCause_t::RUIM_NOT_PRESENT, V1_4::DataCallFailCause::RUIM_NOT_PRESENT},
    {rildata::DataCallFailCause_t::CDMA_RETRY_ORDER, V1_4::DataCallFailCause::CDMA_RETRY_ORDER},
    {rildata::DataCallFailCause_t::ACCESS_BLOCK, V1_4::DataCallFailCause::ACCESS_BLOCK},
    {rildata::DataCallFailCause_t::ACCESS_BLOCK_ALL, V1_4::DataCallFailCause::ACCESS_BLOCK_ALL},
    {rildata::DataCallFailCause_t::IS707B_MAX_ACCESS_PROBES, V1_4::DataCallFailCause::IS707B_MAX_ACCESS_PROBES},
    {rildata::DataCallFailCause_t::THERMAL_EMERGENCY, V1_4::DataCallFailCause::THERMAL_EMERGENCY},
    {rildata::DataCallFailCause_t::CONCURRENT_SERVICES_NOT_ALLOWED, V1_4::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::INCOMING_CALL_REJECTED, V1_4::DataCallFailCause::INCOMING_CALL_REJECTED},
    {rildata::DataCallFailCause_t::NO_SERVICE_ON_GATEWAY, V1_4::DataCallFailCause::NO_SERVICE_ON_GATEWAY},
    {rildata::DataCallFailCause_t::NO_GPRS_CONTEXT, V1_4::DataCallFailCause::NO_GPRS_CONTEXT},
    {rildata::DataCallFailCause_t::ILLEGAL_MS, V1_4::DataCallFailCause::ILLEGAL_MS},
    {rildata::DataCallFailCause_t::ILLEGAL_ME, V1_4::DataCallFailCause::ILLEGAL_ME},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED, V1_4::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_NOT_ALLOWED, V1_4::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK, V1_4::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK},
    {rildata::DataCallFailCause_t::IMPLICITLY_DETACHED, V1_4::DataCallFailCause::IMPLICITLY_DETACHED},
    {rildata::DataCallFailCause_t::PLMN_NOT_ALLOWED, V1_4::DataCallFailCause::PLMN_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::LOCATION_AREA_NOT_ALLOWED, V1_4::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN, V1_4::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN},
    {rildata::DataCallFailCause_t::PDP_DUPLICATE, V1_4::DataCallFailCause::PDP_DUPLICATE},
    {rildata::DataCallFailCause_t::UE_RAT_CHANGE, V1_4::DataCallFailCause::UE_RAT_CHANGE},
    {rildata::DataCallFailCause_t::CONGESTION, V1_4::DataCallFailCause::CONGESTION},
    {rildata::DataCallFailCause_t::NO_PDP_CONTEXT_ACTIVATED, V1_4::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED},
    {rildata::DataCallFailCause_t::ACCESS_CLASS_DSAC_REJECTION, V1_4::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION},
    {rildata::DataCallFailCause_t::PDP_ACTIVATE_MAX_RETRY_FAILED, V1_4::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED},
    {rildata::DataCallFailCause_t::RADIO_ACCESS_BEARER_FAILURE, V1_4::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE},
    {rildata::DataCallFailCause_t::ESM_UNKNOWN_EPS_BEARER_CONTEXT, V1_4::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT},
    {rildata::DataCallFailCause_t::DRB_RELEASED_BY_RRC, V1_4::DataCallFailCause::DRB_RELEASED_BY_RRC},
    {rildata::DataCallFailCause_t::CONNECTION_RELEASED, V1_4::DataCallFailCause::CONNECTION_RELEASED},
    {rildata::DataCallFailCause_t::EMM_DETACHED, V1_4::DataCallFailCause::EMM_DETACHED},
    {rildata::DataCallFailCause_t::EMM_ATTACH_FAILED, V1_4::DataCallFailCause::EMM_ATTACH_FAILED},
    {rildata::DataCallFailCause_t::EMM_ATTACH_STARTED, V1_4::DataCallFailCause::EMM_ATTACH_STARTED},
    {rildata::DataCallFailCause_t::LTE_NAS_SERVICE_REQUEST_FAILED, V1_4::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED},
    {rildata::DataCallFailCause_t::DUPLICATE_BEARER_ID, V1_4::DataCallFailCause::DUPLICATE_BEARER_ID},
    {rildata::DataCallFailCause_t::ESM_COLLISION_SCENARIOS, V1_4::DataCallFailCause::ESM_COLLISION_SCENARIOS},
    {rildata::DataCallFailCause_t::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK, V1_4::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK},
    {rildata::DataCallFailCause_t::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER, V1_4::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER},
    {rildata::DataCallFailCause_t::ESM_BAD_OTA_MESSAGE, V1_4::DataCallFailCause::ESM_BAD_OTA_MESSAGE},
    {rildata::DataCallFailCause_t::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL, V1_4::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL},
    {rildata::DataCallFailCause_t::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT, V1_4::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT},
    {rildata::DataCallFailCause_t::DS_EXPLICIT_DEACTIVATION, V1_4::DataCallFailCause::DS_EXPLICIT_DEACTIVATION},
    {rildata::DataCallFailCause_t::ESM_LOCAL_CAUSE_NONE, V1_4::DataCallFailCause::ESM_LOCAL_CAUSE_NONE},
    {rildata::DataCallFailCause_t::LTE_THROTTLING_NOT_REQUIRED, V1_4::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED},
    {rildata::DataCallFailCause_t::ACCESS_CONTROL_LIST_CHECK_FAILURE, V1_4::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE},
    {rildata::DataCallFailCause_t::SERVICE_NOT_ALLOWED_ON_PLMN, V1_4::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN},
    {rildata::DataCallFailCause_t::EMM_T3417_EXPIRED, V1_4::DataCallFailCause::EMM_T3417_EXPIRED},
    {rildata::DataCallFailCause_t::EMM_T3417_EXT_EXPIRED, V1_4::DataCallFailCause::EMM_T3417_EXT_EXPIRED},
    {rildata::DataCallFailCause_t::RRC_UPLINK_DATA_TRANSMISSION_FAILURE, V1_4::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER, V1_4::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER},
    {rildata::DataCallFailCause_t::RRC_UPLINK_CONNECTION_RELEASE, V1_4::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_RADIO_LINK_FAILURE, V1_4::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_UPLINK_ERROR_REQUEST_FROM_NAS, V1_4::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ACCESS_STRATUM_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS, V1_4::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ACCESS_BARRED, V1_4::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CELL_RESELECTION, V1_4::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CONFIG_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_TIMER_EXPIRED, V1_4::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_LINK_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_CELL_NOT_CAMPED, V1_4::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_REJECT_BY_NETWORK, V1_4::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_NORMAL_RELEASE, V1_4::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RADIO_LINK_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_REESTABLISHMENT_FAILURE, V1_4::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER, V1_4::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORT_REQUEST, V1_4::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR, V1_4::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH, V1_4::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH},
    {rildata::DataCallFailCause_t::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH, V1_4::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH},
    {rildata::DataCallFailCause_t::ESM_PROCEDURE_TIME_OUT, V1_4::DataCallFailCause::ESM_PROCEDURE_TIME_OUT},
    {rildata::DataCallFailCause_t::INVALID_CONNECTION_ID, V1_4::DataCallFailCause::INVALID_CONNECTION_ID},
    {rildata::DataCallFailCause_t::MAXIMIUM_NSAPIS_EXCEEDED, V1_4::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED},
    {rildata::DataCallFailCause_t::INVALID_PRIMARY_NSAPI, V1_4::DataCallFailCause::INVALID_PRIMARY_NSAPI},
    {rildata::DataCallFailCause_t::CANNOT_ENCODE_OTA_MESSAGE, V1_4::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE},
    {rildata::DataCallFailCause_t::RADIO_ACCESS_BEARER_SETUP_FAILURE, V1_4::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE},
    {rildata::DataCallFailCause_t::PDP_ESTABLISH_TIMEOUT_EXPIRED, V1_4::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_MODIFY_TIMEOUT_EXPIRED, V1_4::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_INACTIVE_TIMEOUT_EXPIRED, V1_4::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED},
    {rildata::DataCallFailCause_t::PDP_LOWERLAYER_ERROR, V1_4::DataCallFailCause::PDP_LOWERLAYER_ERROR},
    {rildata::DataCallFailCause_t::PDP_MODIFY_COLLISION, V1_4::DataCallFailCause::PDP_MODIFY_COLLISION},
    {rildata::DataCallFailCause_t::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED, V1_4::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED},
    {rildata::DataCallFailCause_t::NAS_REQUEST_REJECTED_BY_NETWORK, V1_4::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_INVALID_REQUEST, V1_4::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED, V1_4::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RF_UNAVAILABLE, V1_4::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE, V1_4::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE, V1_4::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_AFTER_HANDOVER, V1_4::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE, V1_4::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE},
    {rildata::DataCallFailCause_t::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE, V1_4::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE},
    {rildata::DataCallFailCause_t::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER, V1_4::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER},
    {rildata::DataCallFailCause_t::IMEI_NOT_ACCEPTED, V1_4::DataCallFailCause::IMEI_NOT_ACCEPTED},
    {rildata::DataCallFailCause_t::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED, V1_4::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::EPS_SERVICES_NOT_ALLOWED_IN_PLMN, V1_4::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN},
    {rildata::DataCallFailCause_t::MSC_TEMPORARILY_NOT_REACHABLE, V1_4::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE},
    {rildata::DataCallFailCause_t::CS_DOMAIN_NOT_AVAILABLE, V1_4::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE},
    {rildata::DataCallFailCause_t::ESM_FAILURE, V1_4::DataCallFailCause::ESM_FAILURE},
    {rildata::DataCallFailCause_t::MAC_FAILURE, V1_4::DataCallFailCause::MAC_FAILURE},
    {rildata::DataCallFailCause_t::SYNCHRONIZATION_FAILURE, V1_4::DataCallFailCause::SYNCHRONIZATION_FAILURE},
    {rildata::DataCallFailCause_t::UE_SECURITY_CAPABILITIES_MISMATCH, V1_4::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH},
    {rildata::DataCallFailCause_t::SECURITY_MODE_REJECTED, V1_4::DataCallFailCause::SECURITY_MODE_REJECTED},
    {rildata::DataCallFailCause_t::UNACCEPTABLE_NON_EPS_AUTHENTICATION, V1_4::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION},
    {rildata::DataCallFailCause_t::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED, V1_4::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::NO_EPS_BEARER_CONTEXT_ACTIVATED, V1_4::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED},
    {rildata::DataCallFailCause_t::INVALID_EMM_STATE, V1_4::DataCallFailCause::INVALID_EMM_STATE},
    {rildata::DataCallFailCause_t::NAS_LAYER_FAILURE, V1_4::DataCallFailCause::NAS_LAYER_FAILURE},
    {rildata::DataCallFailCause_t::MULTIPLE_PDP_CALL_NOT_ALLOWED, V1_4::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::IRAT_HANDOVER_FAILED, V1_4::DataCallFailCause::IRAT_HANDOVER_FAILED},
    {rildata::DataCallFailCause_t::TEST_LOOPBACK_REGULAR_DEACTIVATION, V1_4::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::LOWER_LAYER_REGISTRATION_FAILURE, V1_4::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE},
    {rildata::DataCallFailCause_t::DATA_PLAN_EXPIRED, V1_4::DataCallFailCause::DATA_PLAN_EXPIRED},
    {rildata::DataCallFailCause_t::UMTS_HANDOVER_TO_IWLAN, V1_4::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN},
    {rildata::DataCallFailCause_t::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY, V1_4::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY},
    {rildata::DataCallFailCause_t::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE, V1_4::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE},
    {rildata::DataCallFailCause_t::EVDO_HDR_CHANGED, V1_4::DataCallFailCause::EVDO_HDR_CHANGED},
    {rildata::DataCallFailCause_t::EVDO_HDR_EXITED, V1_4::DataCallFailCause::EVDO_HDR_EXITED},
    {rildata::DataCallFailCause_t::EVDO_HDR_NO_SESSION, V1_4::DataCallFailCause::EVDO_HDR_NO_SESSION},
    {rildata::DataCallFailCause_t::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL, V1_4::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL},
    {rildata::DataCallFailCause_t::EVDO_HDR_CONNECTION_SETUP_TIMEOUT, V1_4::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT},
    {rildata::DataCallFailCause_t::FAILED_TO_ACQUIRE_COLOCATED_HDR, V1_4::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR},
    {rildata::DataCallFailCause_t::OTASP_COMMIT_IN_PROGRESS, V1_4::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS},
    {rildata::DataCallFailCause_t::NO_HYBRID_HDR_SERVICE, V1_4::DataCallFailCause::NO_HYBRID_HDR_SERVICE},
    {rildata::DataCallFailCause_t::HDR_NO_LOCK_GRANTED, V1_4::DataCallFailCause::HDR_NO_LOCK_GRANTED},
    {rildata::DataCallFailCause_t::DBM_OR_SMS_IN_PROGRESS, V1_4::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS},
    {rildata::DataCallFailCause_t::HDR_FADE, V1_4::DataCallFailCause::HDR_FADE},
    {rildata::DataCallFailCause_t::HDR_ACCESS_FAILURE, V1_4::DataCallFailCause::HDR_ACCESS_FAILURE},
    {rildata::DataCallFailCause_t::UNSUPPORTED_1X_PREV, V1_4::DataCallFailCause::UNSUPPORTED_1X_PREV},
    {rildata::DataCallFailCause_t::LOCAL_END, V1_4::DataCallFailCause::LOCAL_END},
    {rildata::DataCallFailCause_t::NO_SERVICE, V1_4::DataCallFailCause::NO_SERVICE},
    {rildata::DataCallFailCause_t::FADE, V1_4::DataCallFailCause::FADE},
    {rildata::DataCallFailCause_t::NORMAL_RELEASE, V1_4::DataCallFailCause::NORMAL_RELEASE},
    {rildata::DataCallFailCause_t::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS, V1_4::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS},
    {rildata::DataCallFailCause_t::REDIRECTION_OR_HANDOFF_IN_PROGRESS, V1_4::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS},
    {rildata::DataCallFailCause_t::EMERGENCY_MODE, V1_4::DataCallFailCause::EMERGENCY_MODE},
    {rildata::DataCallFailCause_t::PHONE_IN_USE, V1_4::DataCallFailCause::PHONE_IN_USE},
    {rildata::DataCallFailCause_t::INVALID_MODE, V1_4::DataCallFailCause::INVALID_MODE},
    {rildata::DataCallFailCause_t::INVALID_SIM_STATE, V1_4::DataCallFailCause::INVALID_SIM_STATE},
    {rildata::DataCallFailCause_t::NO_COLLOCATED_HDR, V1_4::DataCallFailCause::NO_COLLOCATED_HDR},
    {rildata::DataCallFailCause_t::UE_IS_ENTERING_POWERSAVE_MODE, V1_4::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE},
    {rildata::DataCallFailCause_t::DUAL_SWITCH, V1_4::DataCallFailCause::DUAL_SWITCH},
    {rildata::DataCallFailCause_t::PPP_TIMEOUT, V1_4::DataCallFailCause::PPP_TIMEOUT},
    {rildata::DataCallFailCause_t::PPP_AUTH_FAILURE, V1_4::DataCallFailCause::PPP_AUTH_FAILURE},
    {rildata::DataCallFailCause_t::PPP_OPTION_MISMATCH, V1_4::DataCallFailCause::PPP_OPTION_MISMATCH},
    {rildata::DataCallFailCause_t::PPP_PAP_FAILURE, V1_4::DataCallFailCause::PPP_PAP_FAILURE},
    {rildata::DataCallFailCause_t::PPP_CHAP_FAILURE, V1_4::DataCallFailCause::PPP_CHAP_FAILURE},
    {rildata::DataCallFailCause_t::PPP_CLOSE_IN_PROGRESS, V1_4::DataCallFailCause::PPP_CLOSE_IN_PROGRESS},
    {rildata::DataCallFailCause_t::LIMITED_TO_IPV4, V1_4::DataCallFailCause::LIMITED_TO_IPV4},
    {rildata::DataCallFailCause_t::LIMITED_TO_IPV6, V1_4::DataCallFailCause::LIMITED_TO_IPV6},
    {rildata::DataCallFailCause_t::VSNCP_TIMEOUT, V1_4::DataCallFailCause::VSNCP_TIMEOUT},
    {rildata::DataCallFailCause_t::VSNCP_GEN_ERROR, V1_4::DataCallFailCause::VSNCP_GEN_ERROR},
    {rildata::DataCallFailCause_t::VSNCP_APN_UNATHORIZED, V1_4::DataCallFailCause::VSNCP_APN_UNATHORIZED},
    {rildata::DataCallFailCause_t::VSNCP_PDN_LIMIT_EXCEEDED, V1_4::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED},
    {rildata::DataCallFailCause_t::VSNCP_NO_PDN_GATEWAY_ADDRESS, V1_4::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS},
    {rildata::DataCallFailCause_t::VSNCP_PDN_GATEWAY_UNREACHABLE, V1_4::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE},
    {rildata::DataCallFailCause_t::VSNCP_PDN_GATEWAY_REJECT, V1_4::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT},
    {rildata::DataCallFailCause_t::VSNCP_INSUFFICIENT_PARAMETERS, V1_4::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS},
    {rildata::DataCallFailCause_t::VSNCP_RESOURCE_UNAVAILABLE, V1_4::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE},
    {rildata::DataCallFailCause_t::VSNCP_ADMINISTRATIVELY_PROHIBITED, V1_4::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED},
    {rildata::DataCallFailCause_t::VSNCP_PDN_ID_IN_USE, V1_4::DataCallFailCause::VSNCP_PDN_ID_IN_USE},
    {rildata::DataCallFailCause_t::VSNCP_SUBSCRIBER_LIMITATION, V1_4::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION},
    {rildata::DataCallFailCause_t::VSNCP_PDN_EXISTS_FOR_THIS_APN, V1_4::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN},
    {rildata::DataCallFailCause_t::VSNCP_RECONNECT_NOT_ALLOWED, V1_4::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::IPV6_PREFIX_UNAVAILABLE, V1_4::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE},
    {rildata::DataCallFailCause_t::HANDOFF_PREFERENCE_CHANGED, V1_4::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_1, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_1},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_2, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_2},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_3, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_3},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_4, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_4},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_5, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_5},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_6, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_6},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_7, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_7},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_8, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_8},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_9, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_9},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_10, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_10},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_11, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_11},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_12, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_12},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_13, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_13},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_14, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_14},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_15, V1_4::DataCallFailCause::OEM_DCFAILCAUSE_15},
    {rildata::DataCallFailCause_t::VOICE_REGISTRATION_FAIL, V1_4::DataCallFailCause::VOICE_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::DATA_REGISTRATION_FAIL, V1_4::DataCallFailCause::DATA_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::SIGNAL_LOST, V1_4::DataCallFailCause::SIGNAL_LOST},
    {rildata::DataCallFailCause_t::PREF_RADIO_TECH_CHANGED, V1_4::DataCallFailCause::PREF_RADIO_TECH_CHANGED},
    {rildata::DataCallFailCause_t::RADIO_POWER_OFF, V1_4::DataCallFailCause::RADIO_POWER_OFF},
    {rildata::DataCallFailCause_t::TETHERED_CALL_ACTIVE, V1_4::DataCallFailCause::TETHERED_CALL_ACTIVE},
    {rildata::DataCallFailCause_t::ERROR_UNSPECIFIED, V1_4::DataCallFailCause::ERROR_UNSPECIFIED}
};

uint64_t toRilCategories(const hidl_bitfield<V1_4::EmergencyServiceCategory>& categories) {
  uint64_t rilCategories = (uint64_t)qcril::interfaces::RIL_EccCategoryMask::UNKNOWN;

  if (categories & V1_4::EmergencyServiceCategory::POLICE) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::POLICE;
  }
  if (categories & V1_4::EmergencyServiceCategory::AMBULANCE) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AMBULANCE;
  }
  if (categories & V1_4::EmergencyServiceCategory::FIRE_BRIGADE) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::FIRE_BRIGADE;
  }
  if (categories & V1_4::EmergencyServiceCategory::MARINE_GUARD) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MARINE_GUARD;
  }
  if (categories & V1_4::EmergencyServiceCategory::MOUNTAIN_RESCUE) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MOUNTAIN_RESCUE;
  }
  if (categories & V1_4::EmergencyServiceCategory::MIEC) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MANUAL_ECALL;
  }
  if (categories & V1_4::EmergencyServiceCategory::AIEC) {
    rilCategories |= (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AUTO_ECALL;
  }

  return rilCategories;
}

qcril::interfaces::EmergencyCallRouting toRilRouting(const V1_4::EmergencyCallRouting& routing) {
  qcril::interfaces::EmergencyCallRouting rilRouting =
      qcril::interfaces::EmergencyCallRouting::UNKNOWN;

  if (routing == V1_4::EmergencyCallRouting::EMERGENCY) {
    rilRouting = qcril::interfaces::EmergencyCallRouting::EMERGENCY;
  } else if (routing == V1_4::EmergencyCallRouting::NORMAL) {
    rilRouting = qcril::interfaces::EmergencyCallRouting::NORMAL;
  }
  return rilRouting;
}

void convertToHal(V1_4::NrSignalStrength& out, const RIL_NR_SignalStrength& in) {
  out.ssRsrp = in.ssRsrp;
  out.ssRsrq = in.ssRsrq;
  out.ssSinr = in.ssSinr;
  out.csiRsrp = in.csiRsrp;
  out.csiRsrq = in.csiRsrq;
  out.csiSinr = in.csiSinr;
}

void convertRilSignalStrengthToHal(const RIL_SignalStrength& rilSignalStrength,
                                   V1_4::SignalStrength& signalStrength) {
  convertToHal(signalStrength.gsm, rilSignalStrength.GW_SignalStrength);
  convertToHal(signalStrength.cdma, rilSignalStrength.CDMA_SignalStrength);
  convertToHal(signalStrength.evdo, rilSignalStrength.EVDO_SignalStrength);
  convertToHal(signalStrength.lte, rilSignalStrength.LTE_SignalStrength);
  convertToHal(signalStrength.tdscdma, rilSignalStrength.TD_SCDMA_SignalStrength);
  convertToHal(signalStrength.wcdma, rilSignalStrength.WCDMA_SignalStrength);
  convertToHal(signalStrength.nr, rilSignalStrength.NR_SignalStrength);
}

void fillDataRegistrationStateResponse(V1_4::DataRegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in) {
  fillDataRegistrationStateResponse(out.base, in);
  if (in.lteVopsInfoValid) {
    V1_4::LteVopsInfo lvi;
    lvi.isVopsSupported = static_cast<bool>(in.lteVopsInfo.isVopsSupported);
    lvi.isEmcBearerSupported = static_cast<bool>(in.lteVopsInfo.isEmcBearerSupported);
    out.vopsInfo.lteVopsInfo(std::move(lvi));
  }

  if (in.nrIndicatorsValid) {
    out.nrIndicators.isEndcAvailable = static_cast<bool>(in.nrIndicators.isEndcAvailable);
    out.nrIndicators.isDcNrRestricted = static_cast<bool>(in.nrIndicators.isDcNrRestricted);
    out.nrIndicators.isNrAvailable = static_cast<bool>(in.nrIndicators.plmnInfoListR15Available);
  }
}

uint32_t toHidlCategories(uint32_t categories) {
  uint32_t cat = (uint32_t)V1_4::EmergencyServiceCategory::UNSPECIFIED;
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::POLICE) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::POLICE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AMBULANCE) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::AMBULANCE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::FIRE_BRIGADE) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::FIRE_BRIGADE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MARINE_GUARD) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::MARINE_GUARD;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MOUNTAIN_RESCUE) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::MOUNTAIN_RESCUE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MANUAL_ECALL) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::MIEC;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AUTO_ECALL) {
    cat |= (uint32_t)V1_4::EmergencyServiceCategory::AIEC;
  }
  return cat;
}

uint32_t toHidlSources(uint32_t sources) {
  uint32_t ecc_source = 0;
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::NETWORK_SIGNALING) {
    ecc_source |= (uint32_t)V1_4::EmergencyNumberSource::NETWORK_SIGNALING;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::SIM) {
    ecc_source |= (uint32_t)V1_4::EmergencyNumberSource::SIM;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::MODEM_CONFIG) {
    ecc_source |= (uint32_t)V1_4::EmergencyNumberSource::MODEM_CONFIG;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::DEFAULT) {
    ecc_source |= (uint32_t)V1_4::EmergencyNumberSource::DEFAULT;
  }

  // If there is no ecc source, set it to modem config as this probably from RIL DB.
  if (ecc_source == 0) {
    ecc_source |= (uint32_t)V1_4::EmergencyNumberSource::MODEM_CONFIG;
  }
  return ecc_source;
}

int convertRilEccListToHal(const qcril::interfaces::RIL_EccList& ril_ecc_list,
                           ::android::hardware::hidl_vec<V1_4::EmergencyNumber>& ecc_list) {
  ecc_list.resize(ril_ecc_list.size());
  std::ostringstream logString{};
  for (size_t i = 0; i < ril_ecc_list.size(); i++) {
    ecc_list[i].number = ril_ecc_list[i]->number;
    logString << "number - " << ecc_list[i].number << " | ";
    ecc_list[i].mcc = ril_ecc_list[i]->mcc;
    logString << "mcc - " << ecc_list[i].mcc << " | ";
    ecc_list[i].mnc = ril_ecc_list[i]->mnc;
    logString << "mnc - " << ecc_list[i].mnc << " | ";
    ecc_list[i].categories = toHidlCategories(static_cast<uint32_t>(ril_ecc_list[i]->categories));
    logString << "categories - " << ecc_list[i].categories << " | ";
    ecc_list[i].sources = toHidlSources(static_cast<uint32_t>(ril_ecc_list[i]->sources));
    logString << "sources - " << ecc_list[i].sources << " | ";
    ecc_list[i].urns.resize(ril_ecc_list[i]->urns.size());
    logString << "urn - [";
    for (size_t j = 0; j < ril_ecc_list[i]->urns.size(); j++) {
      ecc_list[i].urns[j] = ril_ecc_list[i]->urns[j];
      logString << ecc_list[i].urns[j] << ",";
    }
    logString << "]";
    QCRIL_LOG_INFO("%s", logString.str().c_str());
    logString.str("");
  }
  return 0;
}

void convertRilCellInfoToHal_1_4(V1_4::CellInfo& hidlCellInfo, const RIL_CellInfo_v12& rilCellInfo) {
  CellInfoUnion_1_4 cellInfo;

  switch (rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      new (&cellInfo.gsm.cellIdentityGsm.base.mcc)
          hidl_string(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc,
                      strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc,
                              sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mcc) - 1));

      new (&cellInfo.gsm.cellIdentityGsm.base.mnc)
          hidl_string(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc,
                      strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc,
                              sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.mnc) - 1));

      cellInfo.gsm.cellIdentityGsm.base.lac = rilCellInfo.CellInfo.gsm.cellIdentityGsm.lac;
      cellInfo.gsm.cellIdentityGsm.base.cid = rilCellInfo.CellInfo.gsm.cellIdentityGsm.cid;
      cellInfo.gsm.cellIdentityGsm.base.arfcn = rilCellInfo.CellInfo.gsm.cellIdentityGsm.arfcn;
      cellInfo.gsm.cellIdentityGsm.base.bsic = rilCellInfo.CellInfo.gsm.cellIdentityGsm.bsic;
      cellInfo.gsm.signalStrengthGsm.signalStrength =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.signalStrength;
      cellInfo.gsm.signalStrengthGsm.bitErrorRate =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.bitErrorRate;
      cellInfo.gsm.signalStrengthGsm.timingAdvance =
          rilCellInfo.CellInfo.gsm.signalStrengthGsm.timingAdvance;

      new (&cellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong) - 1));

      new (&cellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort) - 1));

      hidlCellInfo.info.gsm(std::move(cellInfo.gsm));
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      new (&cellInfo.wcdma.cellIdentityWcdma.base.mcc)
          hidl_string(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc,
                      strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc,
                              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc) - 1));

      new (&cellInfo.wcdma.cellIdentityWcdma.base.mnc)
          hidl_string(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc,
                      strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc,
                              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc) - 1));

      cellInfo.wcdma.cellIdentityWcdma.base.lac = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.lac;
      cellInfo.wcdma.cellIdentityWcdma.base.cid = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.cid;
      cellInfo.wcdma.cellIdentityWcdma.base.psc = rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.psc;
      cellInfo.wcdma.cellIdentityWcdma.base.uarfcn =
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.uarfcn;
      cellInfo.wcdma.signalStrengthWcdma.base.signalStrength =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.signalStrength;
      cellInfo.wcdma.signalStrengthWcdma.base.bitErrorRate =
          rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;

      new (&cellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong) - 1));

      new (&cellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort,
          strnlen(
              rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort,
              sizeof(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort) - 1));

      hidlCellInfo.info.wcdma(std::move(cellInfo.wcdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      cellInfo.cdma.cellIdentityCdma.base.networkId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.networkId;
      cellInfo.cdma.cellIdentityCdma.base.systemId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.systemId;
      cellInfo.cdma.cellIdentityCdma.base.baseStationId =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.basestationId;
      cellInfo.cdma.cellIdentityCdma.base.longitude =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.longitude;
      cellInfo.cdma.cellIdentityCdma.base.latitude =
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.latitude;
      cellInfo.cdma.signalStrengthCdma.dbm = rilCellInfo.CellInfo.cdma.signalStrengthCdma.dbm;
      cellInfo.cdma.signalStrengthCdma.ecio = rilCellInfo.CellInfo.cdma.signalStrengthCdma.ecio;
      cellInfo.cdma.signalStrengthEvdo.dbm = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.dbm;
      cellInfo.cdma.signalStrengthEvdo.ecio = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.ecio;
      cellInfo.cdma.signalStrengthEvdo.signalNoiseRatio =
          rilCellInfo.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;

      new (&cellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaLong) - 1));

      new (&cellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.cdma.cellIdentityCdma.operatorNames.alphaShort) - 1));

      hidlCellInfo.info.cdma(std::move(cellInfo.cdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      new (&cellInfo.lte.base.cellIdentityLte.base.mcc)
          hidl_string(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc,
                      strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc,
                              sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.mcc) - 1));

      new (&cellInfo.lte.base.cellIdentityLte.base.mnc)
          hidl_string(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc,
                      strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc,
                              sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.mnc) - 1));

      cellInfo.lte.base.cellIdentityLte.base.ci = rilCellInfo.CellInfo.lte.cellIdentityLte.ci;
      cellInfo.lte.base.cellIdentityLte.base.pci = rilCellInfo.CellInfo.lte.cellIdentityLte.pci;
      cellInfo.lte.base.cellIdentityLte.base.tac = rilCellInfo.CellInfo.lte.cellIdentityLte.tac;
      cellInfo.lte.base.cellIdentityLte.base.earfcn =
          rilCellInfo.CellInfo.lte.cellIdentityLte.earfcn;
      cellInfo.lte.base.signalStrengthLte.signalStrength =
          rilCellInfo.CellInfo.lte.signalStrengthLte.signalStrength;
      cellInfo.lte.base.signalStrengthLte.rsrp = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrp;
      cellInfo.lte.base.signalStrengthLte.rsrq = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrq;
      cellInfo.lte.base.signalStrengthLte.rssnr = rilCellInfo.CellInfo.lte.signalStrengthLte.rssnr;
      cellInfo.lte.base.signalStrengthLte.cqi = rilCellInfo.CellInfo.lte.signalStrengthLte.cqi;
      cellInfo.lte.base.signalStrengthLte.timingAdvance =
          rilCellInfo.CellInfo.lte.signalStrengthLte.timingAdvance;

      new (&cellInfo.lte.base.cellIdentityLte.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong) - 1));

      new (&cellInfo.lte.base.cellIdentityLte.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort) - 1));

      cellInfo.lte.base.cellIdentityLte.bandwidth =
          rilCellInfo.CellInfo.lte.cellIdentityLte.bandwidth;
      // TODO: Populate ENDC availability information once modem's network scan and cell location
      // info APIs support it
      cellInfo.lte.cellConfig.isEndcAvailable = false;
      hidlCellInfo.info.lte(std::move(cellInfo.lte));
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      new (&cellInfo.tdscdma.cellIdentityTdscdma.base.mcc)
          hidl_string(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                      strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                              sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc) - 1));

      new (&cellInfo.tdscdma.cellIdentityTdscdma.base.mnc)
          hidl_string(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                      strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                              sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc) - 1));

      cellInfo.tdscdma.cellIdentityTdscdma.base.lac =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.lac;
      cellInfo.tdscdma.cellIdentityTdscdma.base.cid =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.cid;
      cellInfo.tdscdma.cellIdentityTdscdma.base.cpid =
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.cpid;

      new (&cellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaLong) -
                      1));

      new (&cellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaShort) -
                      1));

      cellInfo.tdscdma.cellIdentityTdscdma.uarfcn = 0;  // default value
      cellInfo.tdscdma.signalStrengthTdscdma.rscp =
          rilCellInfo.CellInfo.tdscdma.signalStrengthTdscdma.rscp;
      hidlCellInfo.info.tdscdma(std::move(cellInfo.tdscdma));
      break;
    }

    case RIL_CELL_INFO_TYPE_NR: {
      new (&cellInfo.nr.cellidentity.mcc)
          hidl_string(rilCellInfo.CellInfo.nr.cellIdentityNr.mcc,
                      strnlen(rilCellInfo.CellInfo.nr.cellIdentityNr.mcc,
                              sizeof(rilCellInfo.CellInfo.nr.cellIdentityNr.mcc) - 1));

      new (&cellInfo.nr.cellidentity.mnc)
          hidl_string(rilCellInfo.CellInfo.nr.cellIdentityNr.mnc,
                      strnlen(rilCellInfo.CellInfo.nr.cellIdentityNr.mnc,
                              sizeof(rilCellInfo.CellInfo.nr.cellIdentityNr.mnc) - 1));

      cellInfo.nr.cellidentity.nci = rilCellInfo.CellInfo.nr.cellIdentityNr.nci;
      cellInfo.nr.cellidentity.pci = rilCellInfo.CellInfo.nr.cellIdentityNr.pci;
      cellInfo.nr.cellidentity.tac = rilCellInfo.CellInfo.nr.cellIdentityNr.tac;
      cellInfo.nr.cellidentity.nrarfcn = rilCellInfo.CellInfo.nr.cellIdentityNr.nrarfcn;

      new (&cellInfo.nr.cellidentity.operatorNames.alphaLong) hidl_string(
          rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaLong,
          strnlen(rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaLong,
                  sizeof(rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaLong) - 1));

      new (&cellInfo.nr.cellidentity.operatorNames.alphaShort) hidl_string(
          rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaShort,
          strnlen(rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaShort,
                  sizeof(rilCellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaShort) - 1));

      cellInfo.nr.signalStrength.ssRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrp;
      cellInfo.nr.signalStrength.ssRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrq;
      cellInfo.nr.signalStrength.ssSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.ssSinr;
      cellInfo.nr.signalStrength.csiRsrp = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrp;
      cellInfo.nr.signalStrength.csiRsrq = rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrq;
      cellInfo.nr.signalStrength.csiSinr = rilCellInfo.CellInfo.nr.signalStrengthNr.csiSinr;
      hidlCellInfo.info.nr(std::move(cellInfo.nr));
      break;
    }

    default: {
      break;
    }
  }
}

void convertRilCellInfoListToHal_1_4(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                     hidl_vec<V1_4::CellInfo>& records) {
  auto num = rillCellInfo.size();
  records.resize(num);

  for (unsigned int i = 0; i < num; i++) {
    records[i].isRegistered = rillCellInfo[i].registered;
    records[i].connectionStatus =
        static_cast<V1_2::CellConnectionStatus>(rillCellInfo[i].connStatus);
    convertRilCellInfoToHal_1_4(records[i], rillCellInfo[i]);
  }
}

rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil(const V1_4::DataProfileInfo& profile) {
    rildata::DataProfileInfo_t rilProfile = {};
    rilProfile.profileId = (rildata::DataProfileId_t)profile.profileId;
    rilProfile.apn = profile.apn;
    rilProfile.protocol = convertPdpProtocolTypeToString(profile.protocol);
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
    rilProfile.mtu = profile.mtu;
    rilProfile.preferred = profile.preferred;
    rilProfile.persistent = profile.persistent;
    return rilProfile;
}

V1_4::PdpProtocolType convertStringToPdpProtocolType(const string &type) {
    V1_4::PdpProtocolType protocolType = V1_4::PdpProtocolType::UNKNOWN;

    if (type.compare("IP") == 0) {
        protocolType = V1_4::PdpProtocolType::IP;
    }
    else if (type.compare("IPV6") == 0) {
        protocolType = V1_4::PdpProtocolType::IPV6;
    }
    else if (type.compare("IPV4V6") == 0) {
        protocolType = V1_4::PdpProtocolType::IPV4V6;
    }
    else if (type.compare("PPP") == 0) {
        protocolType = V1_4::PdpProtocolType::PPP;
    }
    else if (type.compare("NON_IP") == 0) {
        protocolType = V1_4::PdpProtocolType::NON_IP;
    }
    else if (type.compare("UNSTRUCTURED") == 0) {
        protocolType = V1_4::PdpProtocolType::UNSTRUCTURED;
    }
    else {
        protocolType = V1_4::PdpProtocolType::UNKNOWN;
    }

    return protocolType;
}

vector<hidl_string> convertAddrStringToHidlStringVector(const string &addr) {
    string tmpString;
    char delimiter = 0x20;
    vector<hidl_string> hidlAddressesVector;
    stringstream ssAddresses(addr);
    while(getline(ssAddresses, tmpString, delimiter)) {
        hidlAddressesVector.push_back(tmpString);
    }
    return hidlAddressesVector;
}

V1_4::DataCallFailCause convertDcFailCauseToHidlDcFailCause_1_4(const rildata::DataCallFailCause_t &cause) {
    V1_4::DataCallFailCause hidlCause = V1_4::DataCallFailCause::ERROR_UNSPECIFIED;
    if (dcFailCauseMap.find(cause) != dcFailCauseMap.end()) {
        hidlCause = dcFailCauseMap[cause];
    }
    return hidlCause;
}

V1_4::SetupDataCallResult convertDcResultToHidlDcResult_1_4(const rildata::DataCallResult_t& result) {
    V1_4::SetupDataCallResult dcResult = {};
    dcResult.cause = convertDcFailCauseToHidlDcFailCause_1_4(result.cause);
    dcResult.suggestedRetryTime = result.suggestedRetryTime;
    dcResult.cid = result.cid;
    dcResult.active = (V1_4::DataConnActiveStatus)result.active;
    dcResult.type = convertStringToPdpProtocolType(result.type);
    dcResult.ifname = result.ifname;
    dcResult.addresses = convertAddrStringToHidlStringVector(result.addresses);
    dcResult.dnses = convertAddrStringToHidlStringVector(result.dnses);
    dcResult.gateways = convertAddrStringToHidlStringVector(result.gateways);
    dcResult.pcscf = convertAddrStringToHidlStringVector(result.pcscf);
    dcResult.mtu = result.mtu;
    return dcResult;
}

V1_4::RadioTechnology convertRadioAccessFamilyTypeToRadioTechnology(rildata::RadioAccessFamily_t accessFamily)
{
    V1_4::RadioTechnology radioTech = V1_4::RadioTechnology::UNKNOWN;

    switch(accessFamily) {
        case rildata::RadioAccessFamily_t::GPRS:
            radioTech = V1_4::RadioTechnology::GPRS;
            break;
        case rildata::RadioAccessFamily_t::EDGE:
            radioTech = V1_4::RadioTechnology::EDGE;
            break;
        case rildata::RadioAccessFamily_t::UMTS:
            radioTech = V1_4::RadioTechnology::UMTS;
            break;
        case rildata::RadioAccessFamily_t::IS95A:
            radioTech = V1_4::RadioTechnology::IS95A;
            break;
        case rildata::RadioAccessFamily_t::IS95B:
            radioTech = V1_4::RadioTechnology::IS95B;
            break;
        case rildata::RadioAccessFamily_t::ONE_X_RTT:
            radioTech = V1_4::RadioTechnology::ONE_X_RTT;
            break;
        case rildata::RadioAccessFamily_t::EVDO_0:
            radioTech = V1_4::RadioTechnology::EVDO_0;
            break;
        case rildata::RadioAccessFamily_t::EVDO_A:
            radioTech = V1_4::RadioTechnology::EVDO_A;
            break;
        case rildata::RadioAccessFamily_t::HSDPA:
            radioTech = V1_4::RadioTechnology::HSDPA;
            break;
        case rildata::RadioAccessFamily_t::HSUPA:
            radioTech = V1_4::RadioTechnology::HSUPA;
            break;
        case rildata::RadioAccessFamily_t::HSPA:
            radioTech = V1_4::RadioTechnology::HSPA;
            break;
        case rildata::RadioAccessFamily_t::EVDO_B:
            radioTech = V1_4::RadioTechnology::EVDO_B;
            break;
        case rildata::RadioAccessFamily_t::EHRPD:
            radioTech = V1_4::RadioTechnology::EHRPD;
            break;
        case rildata::RadioAccessFamily_t::LTE:
            radioTech = V1_4::RadioTechnology::LTE;
            break;
        case rildata::RadioAccessFamily_t::HSPAP:
            radioTech = V1_4::RadioTechnology::HSPAP;
            break;
        case rildata::RadioAccessFamily_t::GSM:
            radioTech = V1_4::RadioTechnology::GSM;
            break;
        case rildata::RadioAccessFamily_t::TD_SCDMA:
            radioTech = V1_4::RadioTechnology::TD_SCDMA;
            break;
        case rildata::RadioAccessFamily_t::LTE_CA:
            radioTech = V1_4::RadioTechnology::LTE_CA;
            break;
        case rildata::RadioAccessFamily_t::NR:
            radioTech = V1_4::RadioTechnology::NR;
            break;
        default:
            radioTech = V1_4::RadioTechnology::UNKNOWN;
            break;
    }
    return radioTech;
}

V1_4::FrequencyRange convertFrequencyRangeToHalType(rildata::FrequencyRange_t range)
{
    V1_4::FrequencyRange freqRange = V1_4::FrequencyRange::LOW;
    switch(range) {
        case rildata::FrequencyRange_t::MMWAVE:
            freqRange = V1_4::FrequencyRange::MMWAVE;
            break;
        case rildata::FrequencyRange_t::HIGH:
            freqRange = V1_4::FrequencyRange::HIGH;
            break;
        case rildata::FrequencyRange_t::MID:
            freqRange = V1_4::FrequencyRange::MID;
            break;
        case rildata::FrequencyRange_t::LOW:
            freqRange = V1_4::FrequencyRange::LOW;
            break;
        case rildata::FrequencyRange_t::UNKNOWN:
        default:
            freqRange = (V1_4::FrequencyRange)(rildata::FrequencyRange_t::UNKNOWN);
            break;
    }
    return freqRange;
}

string convertPdpProtocolTypeToString(const V1_4::PdpProtocolType protocol) {
    string protocolType;
    switch(protocol) {
        case V1_4::PdpProtocolType::IP:
        protocolType = string("IP");
        break;

        case V1_4::PdpProtocolType::IPV6:
        protocolType = string("IPV6");
        break;

        case V1_4::PdpProtocolType::IPV4V6:
        protocolType = string("IPV4V6");
        break;

        case V1_4::PdpProtocolType::PPP:
        protocolType = string("PPP");
        break;

        case V1_4::PdpProtocolType::UNKNOWN:
        case V1_4::PdpProtocolType::NON_IP:
        case V1_4::PdpProtocolType::UNSTRUCTURED:
        default:
        protocolType = string("UNKNOWN");
        break;
    }
    return protocolType;
}

rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork_1_4(
    V1_4::AccessNetwork ran) {
    if (ran == V1_4::AccessNetwork::UNKNOWN) {
        return rildata::AccessNetwork_t::UNKNOWN;
    } else {
        return convertHidlAccessNetworkToDataAccessNetwork(
            (V1_2::AccessNetwork)ran);
    }
}

// ***DO NOT DELETE***
// This is to check (in compile time) if enums defined in ril.h match the
// counterpart in HIDL. This makes sure the correctness of static_cast in
// convertRilNetworkTypeBitmapToHidl_1_4 and convertHidlNetworkTypeBitmapToRil_1_4
void checkNetworkTypeBitsMatch_1_4() {
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_UNKNOWN) ==
            V1_4::RadioAccessFamily::UNKNOWN);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_GPRS) ==
            V1_4::RadioAccessFamily::GPRS);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_EDGE) ==
            V1_4::RadioAccessFamily::EDGE);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_UMTS) ==
            V1_4::RadioAccessFamily::UMTS);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_IS95A) ==
            V1_4::RadioAccessFamily::IS95A);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_IS95B) ==
            V1_4::RadioAccessFamily::IS95B);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_1xRTT) ==
            V1_4::RadioAccessFamily::ONE_X_RTT);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_EVDO_0) ==
            V1_4::RadioAccessFamily::EVDO_0);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_EVDO_A) ==
            V1_4::RadioAccessFamily::EVDO_A);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_HSDPA) ==
            V1_4::RadioAccessFamily::HSDPA);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_HSUPA) ==
            V1_4::RadioAccessFamily::HSUPA);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_HSPA) ==
            V1_4::RadioAccessFamily::HSPA);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_EVDO_B) ==
            V1_4::RadioAccessFamily::EVDO_B);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_EHRPD) ==
            V1_4::RadioAccessFamily::EHRPD);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_LTE) ==
            V1_4::RadioAccessFamily::LTE);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_HSPAP) ==
            V1_4::RadioAccessFamily::HSPAP);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_GSM) ==
            V1_4::RadioAccessFamily::GSM);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_TD_SCDMA) ==
            V1_4::RadioAccessFamily::TD_SCDMA);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_LTE_CA) ==
            V1_4::RadioAccessFamily::LTE_CA);
    static_assert(static_cast<V1_4::RadioAccessFamily>(RAF_5G) ==
            V1_4::RadioAccessFamily::NR);
}

hidl_bitfield<V1_4::RadioAccessFamily> convertRilNetworkTypeBitmapToHidl_1_4(int pref) {
    return static_cast<hidl_bitfield<V1_4::RadioAccessFamily>>(pref);
}

int convertHidlNetworkTypeBitmapToRil_1_4(hidl_bitfield<V1_4::RadioAccessFamily> pref) {
    return static_cast<int>(pref);
}

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
