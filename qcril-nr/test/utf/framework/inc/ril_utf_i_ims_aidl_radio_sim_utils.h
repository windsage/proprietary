/******************************************************************************
#  Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "imsIF.pb.h"
#include "ril_utf_ims_if.h"
#include <framework/legacy.h>
#include <qcrili.h>

#include "ril_utf_i_ims_aidl_radio_sim_utils.h"
#include <aidl/vendor/qti/hardware/radio/ims/EcnamInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ErrorCode.h>
#include <aidl/vendor/qti/hardware/radio/ims/TtyMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClirInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ColrInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallState.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfigItem.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/HandoverInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/DialRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/AnswerRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/HangupRequestInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallForwardInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallModifyInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CiWlanNotificationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/IncomingSms.h>
#include <aidl/vendor/qti/hardware/radio/ims/ToneOperation.h>
#include <aidl/vendor/qti/hardware/radio/ims/ImsSubConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendStatusReport.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendResponse.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsDeliverStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallComposerInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/DtmfInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyCallRoute.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyDialRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/AutoCallRejectionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegistrationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/GeoLocationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/DeflectRequestInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfigFailureCause.h>
#include <aidl/vendor/qti/hardware/radio/ims/MediaConfig.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiSimVoiceCapability.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceClassStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/StkCcUnsolSsResult.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiIdentityLineInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ParticipantStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/PreAlertingCallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/NotificationType.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppServiceNotification.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppServiceStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppServiceStatusRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallForwardStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ExplicitCallTransferInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceClassProvisionStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegistrationBlockStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/Coordinate2D.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosActionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosMoveInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosTouchInfo.h>

namespace imsaidl = ::aidl::vendor::qti::hardware::radio::ims;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace aidl {
namespace utils {
    void convertToAidl(const RIL_IMS_SMS_Message& in,
        imsaidl::SmsSendRequest& out);
    void convertToAidl(const ims_Dial& in,
        imsaidl::DialRequest& out);
    void convertToAidl(const ims_Dial& in,
        imsaidl::EmergencyDialRequest& out);
    void convertToAidl(const ims_Answer& in,
        imsaidl::AnswerRequest& out);
    void convertToAidl(const ims_Hangup& in,
        imsaidl::HangupRequestInfo& out);
    ims_Error convertFromAidl(
        const imsaidl::SmsSendResponse& in);
    ims_Error convertFromAidl(
        const imsaidl::SmsSendResponse& in, ims_SmsSendResponse &out);
    ims_Error convertFromAidl(
        const imsaidl::IncomingSms& in,
        utf_ims_mt_sms_t& out);
    ims_Error convertFromAidl(
        const imsaidl::SmsSendStatusReport& in,
        utf_ims_sms_status_report_t& out);
    ims_Error convertFromAidl(
        const imsaidl::RegistrationInfo& in,
        ims_Registration& out);
    void convertFromAidl(
        const std::vector<
          imsaidl::ServiceStatusInfo>& srvStatusList,
        ims_SrvStatusList& out);
    void convertToAidl(
        const ims_CallForwardInfoList_CallForwardInfo** in,
        std::vector<imsaidl::CallForwardInfo>& out);
    void convertToAidl(
        const ims_CallForwardInfoList_CallForwardInfo& in,
        imsaidl::CallForwardInfo& out);
    void convertToAidl(
        const ims_ExplicitCallTransfer &in,
        imsaidl::ExplicitCallTransferInfo &out);
    void convertToAidl(
        const ims_DeflectCall&in,
        imsaidl::DeflectRequestInfo& out);
    void convertToAidl(
        const ims_Registration_RegState& in,
        imsaidl::RegState& out);
    void convertToAidl(
        const ims_GeoLocationInfo& in,
        imsaidl::GeoLocationInfo& out);
    void convertToAidl(
        const ims_Clir& in,
        imsaidl::ClirInfo& out);
    void convertToAidl(
        const ims_Colr& in,
        imsaidl::ColrInfo& out);
    void convertToAidl(
        const ims_SuppSvcRequest& in,
        imsaidl::SuppServiceStatusRequest& out);
    void convertToAidl(const ims_Tty_Mode_Type& in,
        imsaidl::TtyMode& out);
    void convertToAidl(const ims_ConfigMsg& in,
        imsaidl::ConfigInfo& out);
    void convertToAidl(const ims_ConfigItem& in,
        imsaidl::ConfigItem& out);
    void convertToAidl(const ims_CallModify &in,
        imsaidl::CallModifyInfo& out);
    void convertToAidl(const ims_MultiIdentityStatus& in,
        std::vector<imsaidl::MultiIdentityLineInfo>& out);
    void convertToAidl(const int& in,
        imsaidl::SmsDeliverStatus& out);

    void convertFromAidl(const imsaidl::SipErrorInfo& in,
        ims_SipErrorInfo& out);
    void convertFromAidl(const imsaidl::ConfigFailureCause& in,
        ims_ConfigFailureCause& out);
    void convertFromAidl(const imsaidl::ConfigInfo& in,
        ims_ConfigMsg& out);
    void convertFromAidl(const imsaidl::SuppServiceStatus &in,
        ims_SuppSvcResponse &out);
    void convertFromAidl(const imsaidl::ClirInfo &in,
        ims_Clir &out);
    void convertFromAidl(const imsaidl::CallForwardInfo& in,
        ims_CallForwardInfoList_CallForwardInfo& out);
    void convertFromAidl(const imsaidl::ServiceClassStatus& in,
        ims_ServiceClassStatus& out);
    void convertFromAidl(const imsaidl::CallForwardStatusInfo &in,
        ims_CallForwardStatusInfo &out);
    void convertFromAidl(const imsaidl::ImsSubConfigInfo &in,
        ims_ImsSubConfig &out);
    void convertFromAidl(const imsaidl::TtyMode& in,
        ims_Tty_Mode_Type& out);
    void convertFromAidl(const imsaidl::CallModifyInfo& out,
        ims_CallModify &in);
    void convertFromAidl(const imsaidl::ToneOperation &in,
        ims_RingBackTone_ToneFlag& out);
    void convertFromAidl(const imsaidl::HandoverInfo &in,
        ims_Handover &out);
    void convertFromAidl(const std::vector<imsaidl::CallInfo> &in,
        ims_CallList &out);
    void convertFromAidl(const imsaidl::SuppServiceNotification &in,
        ims_SuppSvcNotification &out);
    void convertFromAidl(const imsaidl::ParticipantStatusInfo &in,
        ims_ParticipantStatusInfo &out);
    void convertFromAidl(const imsaidl::RegistrationBlockStatusInfo& in,
        ims_RegistrationBlockStatus& out);
    void convertFromAidl(const std::vector<imsaidl::MultiIdentityLineInfo> &in,
        ims_MultiIdentityStatus &out);
    void convertFromAidl(const imsaidl::AutoCallRejectionInfo& in,
        ims_AutoCallRejectionInfo& out);
    void convertFromAidl(const imsaidl::CallComposerInfo& in,
        ims_CallComposer& out);
    void convertFromAidl(const imsaidl::ColrInfo& in,
        ims_Colr& out);
    ims_Error convertFromAidl(const imsaidl::ErrorCode& in);
    void convertFromAidl(const imsaidl::MultiSimVoiceCapability& in,
                         ims_MultiSimVoiceCapability& out);
    void convertFromAidl(const imsaidl::PreAlertingCallInfo& in, ims_PreAlertingCallInfo& out);
    void convertFromAidl(const imsaidl::CiWlanNotificationInfo& in,
        ims_CiWlanNotificationInfo& out);
    void convertFromAidl(const imsaidl::EcnamInfo& in, ims_EcnamInfo& out);
    imsaidl::VosActionInfo convertToAidl(const ims_VosActionInfo &in);
    imsaidl::VosMoveInfo convertToAidl(const ims_VosMoveInfo &in);
    imsaidl::VosTouchInfo convertToAidl(const ims_VosTouchInfo &in);
    imsaidl::Coordinate2D convertToAidl(const ims_Coordinate2D &in);
} // namespace utils
} // namespace aidl
} // namespace ims
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
