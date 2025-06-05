/*
 * Copyright (c) 2019-2020, 2022, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

/**
 * IMS Menu class provides IMS functionality of the SDK
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "ImsMenu.hpp"
#include "Phone/PhoneMenu.hpp"

using namespace std;

ostream& operator<<(ostream& os, const RIL_IMS_Registration& imsRegInfo);

ImsMenu::ImsMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

ImsMenu::~ImsMenu() {
}

void ImsMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListImsSubMenu = {
    CREATE_COMMAND(ImsMenu::registrationStatus, "Registration_Status"),
    CREATE_COMMAND(ImsMenu::imsDial, "Dial", "Number"),
    CREATE_COMMAND(ImsMenu::acceptCall, "Answer"),
    CREATE_COMMAND(ImsMenu::hangupCall, "Hangup"),
    CREATE_COMMAND(ImsMenu::imsAddParticipant, "Add Participant To Call"),
    CREATE_COMMAND(ImsMenu::imsSetImsConfig, "Set IMS Config"),
    CREATE_COMMAND(ImsMenu::imsGetImsConfig, "Get IMS Config"),
    CREATE_COMMAND(ImsMenu::imsRegistrationChange, "RegistrationChange"),
    CREATE_COMMAND(ImsMenu::imsQueryServiceStatus, "QueryServiceStatus"),
    CREATE_COMMAND(ImsMenu::imsSetServiceStatus, "SetServiceStatus"),
    CREATE_COMMAND(ImsMenu::imsGetImsSubConfig, "GetImsSubConfig"),
    CREATE_COMMAND(ImsMenu::imsDtmf, "imsDtmf"),
    CREATE_COMMAND(ImsMenu::imsDtmfStart, "imsDtmfStart"),
    CREATE_COMMAND(ImsMenu::imsDtmfStop, "imsDtmfStop"),
    CREATE_COMMAND(ImsMenu::imsSendHelloWorldSms, "Send 'hello world' SMS Over IMS"),
    CREATE_COMMAND(ImsMenu::imsSendSms, "Send SMS Over IMS"),
    CREATE_COMMAND(ImsMenu::imsAckSms, "Ack SMS Received Over IMS"),
    CREATE_COMMAND(ImsMenu::imsGetSmsFormat, "Get Current SMS Format"),
    CREATE_COMMAND(ImsMenu::imsSetCallWaiting, "Set IMS Call Waiting"),
    CREATE_COMMAND(ImsMenu::imsQueryCallWaiting, "Query IMS Call Waiting Status"),
    CREATE_COMMAND(ImsMenu::registerForImsIndications, "Register_Indications"),
    CREATE_COMMAND(ImsMenu::imsModifyCallInitiate, "ImsModifyCallInitiate"),
    CREATE_COMMAND(ImsMenu::imsModifyCallConfirm, "ImsModifyCallConfirm"),
    CREATE_COMMAND(ImsMenu::imsCancelModifyCall, "ImsCancelModifyCall"),
    CREATE_COMMAND(ImsMenu::imsRequestConference, "ImsRequestConference"),
    CREATE_COMMAND(ImsMenu::imsRequestHoldCall, "ImsRequestHoldCall"),
    CREATE_COMMAND(ImsMenu::imsRequestResumeCall, "ImsRequestResumeCall"),
    CREATE_COMMAND(ImsMenu::imsRequestExitEmergencyCallbackMode,
                   "ImsRequestExitEmergencyCallbackMode"),
    CREATE_COMMAND(ImsMenu::imsSendRttMessage, "ImsSendRttMessage"),
    CREATE_COMMAND(ImsMenu::imsSendGeolocationInfo, "ImsSendGeolocationInfo"),
    CREATE_COMMAND(ImsMenu::imsGetClir, "GetClir"),
    CREATE_COMMAND(ImsMenu::imsSetClir, "SetClir"),
    CREATE_COMMAND(ImsMenu::imsExplicitCallTransfer, "ExplicitCallTransfer"),
    CREATE_COMMAND(ImsMenu::imsSendUiTtyMode, "SendUiTtyMode"),
    CREATE_COMMAND(ImsMenu::imsDeflectCall, "DeflectCall"),
    CREATE_COMMAND(ImsMenu::imsQueryClip, "QueryClip"),
    CREATE_COMMAND(ImsMenu::imsSetSuppSvcNotification, "SetSuppSvcNotification"),
    CREATE_COMMAND(ImsMenu::imsSetColr, "SetColr"),
    CREATE_COMMAND(ImsMenu::imsGetColr, "GetColr"),
    CREATE_COMMAND(ImsMenu::imsRegisterMultiIdentityLines, "RegisterMultiIdentityLines"),
    CREATE_COMMAND(ImsMenu::imsQueryVirtualLineInfo, "QueryVirtualLineInfo"),
    CREATE_COMMAND(ImsMenu::imsSetCallForwardStatus, "Set IMS Call Forward Status"),
    CREATE_COMMAND(ImsMenu::imsQueryCallForwardStatus, "Query IMS Call Forward STatus"),
    CREATE_COMMAND(ImsMenu::imsQueryMultiSimVoiceCapability, "QueryMultiSimVoiceCapability"),
    CREATE_COMMAND(ImsMenu::imsSuppSvcStatus, "imsSuppSvcStatus"),
    CREATE_COMMAND(ImsMenu::imsSendUssd, "SendUSSD", "Ussd"),
    CREATE_COMMAND(ImsMenu::imsCancelUssd, "CancelUSSD"),
  };

  addCommands(commandsListImsSubMenu);
  ConsoleApp::displayMenu();
}

istream& operator >> (istream &in, RIL_IMS_CallType &callType)
{
    int var;
    cout << "0. RIL_IMS_CALL_TYPE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_CALL_TYPE_VOICE" << std::endl;
    cout << "2. RIL_IMS_CALL_TYPE_VT_TX" << std::endl;
    cout << "3. RIL_IMS_CALL_TYPE_VT_RX" << std::endl;
    cout << "4. RIL_IMS_CALL_TYPE_VT" << std::endl;
    cout << "5. RIL_IMS_CALL_TYPE_VT_NODIR" << std::endl;
    cout << "6. RIL_IMS_CALL_TYPE_SMS" << std::endl;
    cout << "7. RIL_IMS_CALL_TYPE_UT" << std::endl;
    do {
        cout << "Enter Call Type:";
        in >> var;
    } while (var < 0 || var > 7);
    callType = static_cast<RIL_IMS_CallType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_CallDomain &arg)
{
    int var;
    cout << "0. RIL_IMS_CALLDOMAIN_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_CALLDOMAIN_CS" << std::endl;
    cout << "2. RIL_IMS_CALLDOMAIN_PS" << std::endl;
    cout << "3. RIL_IMS_CALLDOMAIN_AUTOMATIC" << std::endl;
    cout << "4. RIL_IMS_CALLDOMAIN_NOT_SET" << std::endl;
    cout << "5. RIL_IMS_CALLDOMAIN_INVALID" << std::endl;
    do {
        cout << "Enter Call Domain:";
        in >> var;
    } while (var < 0 || var > 5);
    arg = static_cast<RIL_IMS_CallDomain>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_IpPresentation &ipPresentation)
{
    int var;
    cout << "0. RIL_IMS_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
    cout << "1. RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
    cout << "2. RIL_IMS_IP_PRESENTATION_NUM_DEFAULT" << std::endl;
    cout << "3. RIL_IMS_IP_PRESENTATION_INVALID" << std::endl;
    do {
        cout << "Enter IP Presentation Type:";
        in >> var;
    } while (var < 0 || var > 3);
    ipPresentation = static_cast<RIL_IMS_IpPresentation>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_RttMode &rttMode)
{
    int var;
    cout << "0. RIL_IMS_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
    cout << "1. RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
    do {
        in >> var;
    } while (var < 0 || var > 1);
    rttMode = static_cast<RIL_IMS_RttMode>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_RegistrationState &state)
{
    int var;
    cout << "0. RIL_IMS_REG_STATE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_REG_STATE_REGISTERED" << std::endl;
    cout << "2. RIL_IMS_REG_STATE_NOT_REGISTERED" << std::endl;
    cout << "3. RIL_IMS_REG_STATE_REGISTERING" << std::endl;
    do {
        in >> var;
    } while (var < 0 || var > 3);
    state = static_cast<RIL_IMS_RegistrationState>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_StatusType &status)
{
    int var;
    cout << "0. RIL_IMS_STATUS_DISABLED" << std::endl;
    cout << "1. RIL_IMS_STATUS_PARTIALLY_ENABLED" << std::endl;
    cout << "2. RIL_IMS_STATUS_ENABLED" << std::endl;
    cout << "3. RIL_IMS_STATUS_NOT_SUPPORTED" << std::endl;
    do {
        cout << "Enter Choice:" << std::endl;
        in >> var;
    } while (var < 0 || var > 3);
    status = static_cast<RIL_IMS_StatusType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_RadioTechnology &tech)
{
    int var;
    cout << "0. RADIO_TECH_UNKNOWN" << std::endl;
    cout << "1. RADIO_TECH_GPRS" << std::endl;
    cout << "2. RADIO_TECH_EDGE" << std::endl;
    cout << "3. RADIO_TECH_UMTS" << std::endl;
    cout << "4. RADIO_TECH_IS95A" << std::endl;
    cout << "5. RADIO_TECH_IS95B" << std::endl;
    cout << "6. RADIO_TECH_1xRTT" << std::endl;
    cout << "7. RADIO_TECH_EVDO_0" << std::endl;
    cout << "8. RADIO_TECH_EVDO_A" << std::endl;
    cout << "9. RADIO_TECH_HSDPA" << std::endl;
    cout << "10. RADIO_TECH_HSUPA" << std::endl;
    cout << "11. RADIO_TECH_HSPA" << std::endl;
    cout << "12. RADIO_TECH_EVDO_B" << std::endl;
    cout << "13. RADIO_TECH_EHRPD" << std::endl;
    cout << "14. RADIO_TECH_LTE" << std::endl;
    cout << "15. RADIO_TECH_HSPAP" << std::endl;
    cout << "16. RADIO_TECH_GSM" << std::endl;
    cout << "17. RADIO_TECH_TD_SCDMA" << std::endl;
    cout << "18. RADIO_TECH_IWLAN" << std::endl;
    cout << "19. RADIO_TECH_LTE_CA" << std::endl;
    cout << "20. RADIO_TECH_5G" << std::endl;
    cout << "21. RADIO_TECH_WIFI" << std::endl;
    cout << "22. RADIO_TECH_ANY" << std::endl;
    cout << "23. RADIO_TECH_AUTO" << std::endl;
    do {
        cout << "Enter Choice:" << std::endl;
        in >> var;
    } while (var < 0 || var > 23);
    tech = static_cast<RIL_RadioTechnology>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Registration &reg)
{
    cout << "Enter State:" << std::endl;
    in >> reg.state;
    return in;
}


istream& operator >> (istream &in, RIL_IMS_AccessTechnologyStatus &accTech)
{
    cout << "Enter Network Mode:" << std::endl;
    in >> accTech.networkMode;
    cout << "Enter Status:" << std::endl;
    in >> accTech.status;
    cout << "Enter Restrict Cause:" << std::endl;
    in >> accTech.restrictCause;
    //cout << "Enter Registration:" << std::endl;
    //in >> in.registration;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ServiceStatusInfo &info)
{
    in >> info.callType;
    in >> info.accTechStatus.networkMode;
    in >> info.accTechStatus.status;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Answer &answer)
{
    in >> answer.callType;
    in >> answer.presentation;
    in >> answer.rttMode;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Hangup &hangup)
{
    cout << "Enter Connection Id:" << std::endl;
    in >> hangup.connIndex;
    return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_CallModifyInfo &arg)
{
  std::cout << "Enter callId:" << std::endl;
  in >> arg.callId;

  in >> arg.callType;
  in >> arg.callDomain;
  arg.hasRttMode = 1;
  in >> arg.rttMode;
  // CallModifyFailCause not set for req
  arg.hasCallModifyFailCause = 0;

  return in;
}
ostream& operator<<(ostream& os, const RIL_AudioQuality quality) {
    os << "RIL Audio Quality : ";
    switch(quality) {
        case RIL_AUDIO_QUAL_UNSPECIFIED:
            os << "RIL_AUDIO_QUAL_UNSPECIFIED";
            break;
        case RIL_AUDIO_QUAL_AMR:
            os << "RIL_AUDIO_QUAL_AMR";
            break;
        case RIL_AUDIO_QUAL_AMR_WB:
            os << "RIL_AUDIO_QUAL_AMR_WB";
            break;
        case RIL_AUDIO_QUAL_GSM_EFR:
            os << "RIL_AUDIO_QUAL_GSM_EFR";
            break;
        case RIL_AUDIO_QUAL_GSM_FR:
            os << "RIL_AUDIO_QUAL_GSM_FR";
            break;
        case RIL_AUDIO_QUAL_GSM_HR:
            os << "RIL_AUDIO_QUAL_GSM_HR";
            break;
        case RIL_AUDIO_QUAL_EVRC:
            os << "RIL_AUDIO_QUAL_EVRC";
            break;
        case RIL_AUDIO_QUAL_EVRC_B:
            os << "RIL_AUDIO_QUAL_EVRC_B";
            break;
        case RIL_AUDIO_QUAL_EVRC_WB:
            os << "RIL_AUDIO_QUAL_EVRC_WB";
            break;
        case RIL_AUDIO_QUAL_EVRC_NW:
            os << "RIL_AUDIO_QUAL_EVRC_NW";
            break;
    }
    return os;
}


ostream& operator<<(ostream& os, const RIL_UUS_Type status) {
    os << "RIL UUS Type: ";
    switch(status) {
        case RIL_UUS_TYPE1_IMPLICIT:
            os << "RIL_UUS_TYPE1_IMPLICIT";
            break;
        case RIL_UUS_TYPE1_REQUIRED:
            os << "RIL_UUS_TYPE1_REQUIRED";
            break;
        case RIL_UUS_TYPE1_NOT_REQUIRED:
            os << "RIL_UUS_TYPE1_NOT_REQUIRED";
            break;
        case RIL_UUS_TYPE2_REQUIRED:
            os << "RIL_UUS_TYPE2_REQUIRED";
            break;
        case RIL_UUS_TYPE2_NOT_REQUIRED:
            os << "RIL_UUS_TYPE2_NOT_REQUIRED";
            break;
        case RIL_UUS_TYPE3_REQUIRED:
            os << "RIL_UUS_TYPE3_REQUIRED";
            break;
        case RIL_UUS_TYPE3_NOT_REQUIRED:
            os << "RIL_UUS_TYPE3_NOT_REQUIRED";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_UUS_DCS dcs) {
    os << "RIL UUS DCS: ";
    switch(dcs) {
        case RIL_UUS_DCS_USP:
            os << "RIL_UUS_DCS_USP";
            break;
        case RIL_UUS_DCS_OSIHLP:
            os << "RIL_UUS_DCS_OSIHLP";
            break;
        case RIL_UUS_DCS_X244:
            os << "RIL_UUS_DCS_X244";
            break;
        case RIL_UUS_DCS_RMCF:
            os << "RIL_UUS_DCS_RMCF";
            break;
        case RIL_UUS_DCS_IA5c:
            os << "RIL_UUS_DCS_IA5c";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_VerificationStatus status) {
    os << "IMS Verficaition Status : ";
    switch(status) {
        case RIL_IMS_VERSTAT_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_VERSTAT_NONE:
            os << "NONE";
            break;
        case RIL_IMS_VERSTAT_PASS:
            os << "PASS";
            break;
        case RIL_IMS_VERSTAT_FAIL:
            os << "FAIL";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_TirMode mode) {
    os << "RIL Tir Mode : ";
    switch(mode) {
        case RIL_IMS_TIRMODE_UNKNOWN:
            os << "RIL_IMS_TIRMODE_UNKNOWN";
            break;
        case RIL_IMS_TIRMODE_TEMPORARY:
            os << "RIL_IMS_TIRMODE_TEMPORARY";
            break;
        case RIL_IMS_TIRMODE_PERMANENT:
            os << "RIL_IMS_TIRMODE_PERMANENT";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallProgressInfoType type) {
    os << "RIL IMS CallPropgressInfoType : ";
    switch(type) {
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE:
            os << "RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN:
            os << "RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN";
            break;
        default:
            os << "<Invalid>";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallState state) {
    os << "CALL STATE : ";
    switch(state) {
        case RIL_IMS_CALLSTATE_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_CALLSTATE_ACTIVE:
            os << "ACTIVE";
            break;
        case RIL_IMS_CALLSTATE_HOLDING:
            os << "HOLDING";
            break;
        case RIL_IMS_CALLSTATE_DIALING:
            os << "DIALING";
            break;
        case RIL_IMS_CALLSTATE_ALERTING:
            os << "ALERTING";
            break;
        case RIL_IMS_CALLSTATE_INCOMING:
            os << "INCOMING";
            break;
        case RIL_IMS_CALLSTATE_WAITING:
            os << "WAITING";
            break;
        case RIL_IMS_CALLSTATE_END:
            os << "END";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ToneOperation op) {
    os << "TONE OP : ";
    switch(op) {
        case RIL_IMS_TONE_OP_INVALID:
            os << "INVALID";
            break;
        case RIL_IMS_TONE_OP_STOP:
            os << "STOP";
            break;
        case RIL_IMS_TONE_OP_START:
            os << "START";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RttMode mode) {
    os << "IMS RTT Mode : ";
    switch(mode) {
        case RIL_IMS_RTT_DISABLED:
            os << "DISABLED";
            break;
        case RIL_IMS_RTT_FULL:
            os << "FULL";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallDomain domain) {
    os << "CALL DOMAIN : ";
    switch(domain) {
        case RIL_IMS_CALLDOMAIN_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_CALLDOMAIN_CS:
            os << "CS";
            break;
        case RIL_IMS_CALLDOMAIN_PS:
            os << "PS";
            break;
        case RIL_IMS_CALLDOMAIN_AUTOMATIC:
            os << "AUTOMATIC";
            break;
        case RIL_IMS_CALLDOMAIN_NOT_SET:
            os << "NOT_SET";
            break;
        case RIL_IMS_CALLDOMAIN_INVALID:
            os << "INVALID";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallModifiedCause cause) {
    os << "CALL MODIFIED CAUSE : ";
    switch(cause) {
        case RIL_IMS_CMODCAUSE_NONE:
            os << "NONE";
            break;
        case RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ:
            os << "UPGRADE_DUE_TO_LOCAL_REQ";
            break;
        case RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ:
            os << "UPGRADE_DUE_TO_REMOTE_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ:
            os << "DOWNGRADE_DUE_TO_LOCAL_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ:
            os << "DOWNGRADE_DUE_TO_REMOTE_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT:
            os << "DOWNGRADE_DUE_TO_RTP_TIMEOUT";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_QOS:
            os << "DOWNGRADE_DUE_TO_QOS";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS:
            os << "DOWNGRADE_DUE_TO_PACKET_LOSS";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT:
            os << "DOWNGRADE_DUE_TO_LOW_THRPUT";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION:
            os << "DOWNGRADE_DUE_TO_THERM_MITIGATION";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC:
            os << "DOWNGRADE_DUE_TO_LIPSYNC";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR:
            os << "DOWNGRADE_DUE_TO_GENERIC_ERROR";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallModifyFailCause cause) {
    os << " Call Modify failure cause: { ";
    switch (cause) {
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNKNOWN:
        os << "CALL_MODIFY_FAIL_CAUSE_UNKNOWN";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS:
        os << "CALL_MODIFY_FAIL_CAUSE_SUCCESS";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE:
        os << "CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE:
        os << "CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED:
        os << "CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_CANCELLED:
        os << "CALL_MODIFY_FAIL_CAUSE_CANCELLED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNUSED:
        os << "CALL_MODIFY_FAIL_CAUSE_UNUSED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER:
        os << "CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE:
        os << "CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED:
        os << "CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED:
        os << "CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED:
        os << "CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED:
        os << "CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED";
        break;
    case RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION:
        os << "CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ConfParticipantOperation op) {
    os << " conference participant op: { ";
    switch (op) {
    case RIL_IMS_CONF_PARTICIPANT_OP_UNKNOWN:
        os << "CONF_PARTICIPANT_OP_UNKNOWN";
        break;
    case RIL_IMS_CONF_PARTICIPANT_OP_ADD:
        os << "CONF_PARTICIPANT_OP_ADD";
        break;
    case RIL_IMS_CONF_PARTICIPANT_OP_REMOVE:
        os << "CONF_PARTICIPANT_OP_REMOVE";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_BlockReasonType reason) {
    os << " block reason type: { ";
    switch (reason) {
    case RIL_IMS_BLOCK_REASON_TYPE_UNKNOWN:
        os << "BLOCK_REASON_TYPE_UNKNOWN";
        break;
    case RIL_IMS_BLOCK_REASON_TYPE_PDP_FAILURE:
        os << "BLOCK_REASON_TYPE_PDP_FAILURE";
        break;
    case RIL_IMS_BLOCK_REASON_TYPE_REGISTRATION_FAILURE:
        os << "BLOCK_REASON_TYPE_REGISTRATION_FAILURE";
        break;
    case RIL_IMS_BLOCK_REASON_TYPE_HANDOVER_FAILURE:
        os << "BLOCK_REASON_TYPE_HANDOVER_FAILURE";
        break;
    case RIL_IMS_BLOCK_REASON_TYPE_OTHER_FAILURE:
        os << "BLOCK_REASON_TYPE_OTHER_FAILURE";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RegFailureReasonType reason) {
    os << " reg fail reson: { ";
    switch (reason) {
    case RIL_IMS_REG_FAILURE_REASON_TYPE_UNKNOWN:
        os << "REG_FAILURE_REASON_TYPE_UNKNOWN";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_UNSPECIFIED:
        os << "REG_FAILURE_REASON_TYPE_UNSPECIFIED";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_MOBILE_IP:
        os << "REG_FAILURE_REASON_TYPE_MOBILE_IP";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_INTERNAL:
        os << "REG_FAILURE_REASON_TYPE_INTERNAL";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED:
        os << "REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_SPEC_DEFINED:
        os << "REG_FAILURE_REASON_TYPE_SPEC_DEFINED";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_PPP:
        os << "REG_FAILURE_REASON_TYPE_PPP";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_EHRPD:
        os << "REG_FAILURE_REASON_TYPE_EHRPD";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_IPV6:
        os << "REG_FAILURE_REASON_TYPE_IPV6";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_IWLAN:
        os << "REG_FAILURE_REASON_TYPE_IWLAN";
        break;
    case RIL_IMS_REG_FAILURE_REASON_TYPE_HANDOFF:
        os << "REG_FAILURE_REASON_TYPE_HANDOFF";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_BlockReasonDetails& details) {
    os << " block reason details: { ";
    os << details.regFailureReasonType;
    if (details.hasRegFailureReason)
        os << "regFailureReason: " << details.regFailureReason;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_BlockStatus status) {
    os << " IMS block status: { ";
    os << status.blockReason;
    if (status.hasBlockReasonDetails)
        os << status.blockReasonDetails;
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MessageType type) {
    os << " IMS message type: { ";
    switch (type) {
    case RIL_IMS_MESSAGE_TYPE_UNKNOWN:
        os << "TYPE_UNKNOWN";
        break;
    case RIL_IMS_MESSAGE_TYPE_NONE:
        os << "TYPE_NONE";
        break;
    case RIL_IMS_MESSAGE_TYPE_VOICE:
        os << "TYPE_VOICE";
        break;
    case RIL_IMS_MESSAGE_TYPE_VIDEO:
        os << "TYPE_VIDEO";
        break;
    case RIL_IMS_MESSAGE_TYPE_FAX:
        os << "TYPE_FAX";
        break;
    case RIL_IMS_MESSAGE_TYPE_PAGER:
        os << "TYPE_PAGER";
        break;
    case RIL_IMS_MESSAGE_TYPE_MULTIMEDIA:
        os << "TYPE_MULTIMEDIA";
        break;
    case RIL_IMS_MESSAGE_TYPE_TEXT:
        os << "TYPE_TEXT";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MessagePriority priority) {
    os << " IMS message priority { ";
    switch (priority) {
    case RIL_IMS_MESSAGE_PRIORITY_UNKNOWN:
        os << "PRIORITY_UNKNOWN";
        break;
    case RIL_IMS_MESSAGE_PRIORITY_LOW:
        os << "PRIORITY_LOW";
        break;
    case RIL_IMS_MESSAGE_PRIORITY_NORMAL:
        os << "PRIORITY_NORMAL";
        break;
    case RIL_IMS_MESSAGE_PRIORITY_URGENT:
        os << "PRIORITY_URGENT";
        break;
    }
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MessageSummary summary) {
    os << " IMS Message summary: { ";
    os << summary.messageType;
    os << " newMessageCount: " << summary.newMessageCount
       << " oldMessageCount: " << summary.oldMessageCount
       << " newUrgentMessageCount: " << summary.newUrgentMessageCount
       << " oldUrgentMessageCount: " << summary.oldUrgentMessageCount;
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MessageDetails details) {
    os << "IMS Message details: { ";
    if (details.toAddress)
        os << "toAddress: " << details.toAddress;
    if (details.fromAddress)
        os << "fromAddress: " << details.fromAddress;
    if (details.subject)
        os << "subject: " << details.subject;
    if (details.date)
        os << "date: " << details.date;
    os << details.priority;
    if (details.messageId)
    os << details.messageType;
    os << " }";
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallType type) {
    os << " call type: ";
    switch(type)
    {
        case RIL_IMS_CALL_TYPE_UNKNOWN:
            os << "RIL_IMS_CALL_TYPE_UNKNOWN";
            break;
        case RIL_IMS_CALL_TYPE_VOICE:
            os << "RIL_IMS_CALL_TYPE_VOICE";
            break;
        case RIL_IMS_CALL_TYPE_VT_TX:
            os << "RIL_IMS_CALL_TYPE_VT_TX";
            break;
        case RIL_IMS_CALL_TYPE_VT_RX:
            os << "RIL_IMS_CALL_TYPE_VT_RX";
            break;
        case RIL_IMS_CALL_TYPE_VT:
            os << "RIL_IMS_CALL_TYPE_VT";
            break;
        case RIL_IMS_CALL_TYPE_VT_NODIR:
            os << "RIL_IMS_CALL_TYPE_VT_NODIR";
            break;
        case RIL_IMS_CALL_TYPE_SMS:
            os << "RIL_IMS_CALL_TYPE_SMS";
            break;
        case RIL_IMS_CALL_TYPE_UT:
            os << "RIL_IMS_CALL_TYPE_UT";
            break;
        case RIL_IMS_CALL_TYPE_USSD:
            os << "RIL_IMS_CALL_TYPE_USSD";
            break;
        case RIL_IMS_CALL_TYPE_CALLCOMPOSER:
            os << "RIL_IMS_CALL_TYPE_CALLCOMPOSER";
            break;
        default:
            os << "RIL_IMS_INVALID_CALL_TYPE";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_StatusType type) {

    os << "IMS STATUS : ";
    switch(type) {
        case RIL_IMS_STATUS_DISABLED:
            os << "DISABLED";
            break;
        case RIL_IMS_STATUS_PARTIALLY_ENABLED:
            os << "PARTIALLY_ENABLED";
            break;
        case RIL_IMS_STATUS_ENABLED:
            os << "ENABLED";
            break;
        case RIL_IMS_STATUS_NOT_SUPPORTED:
            os << "NOT_SUPPORTED";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_AccessTechnologyStatus& status) {
    os << status.networkMode << std::endl;
    os << status.status << std::endl;
    os << "Restrict Cause : " << status.restrictCause << std::endl;
    os << status.registration << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RegistrationState state) {

    os << "IMS Registration State : ";
    switch(state) {
        case RIL_IMS_REG_STATE_UNKNOWN:
            os << "RIL_IMS_REG_STATE_UNKNOWN";
            break;
        case RIL_IMS_REG_STATE_REGISTERED:
            os << "RIL_IMS_REG_STATE_REGISTERED";
            break;
        case RIL_IMS_REG_STATE_NOT_REGISTERED:
            os << "RIL_IMS_REG_STATE_NOT_REGISTERED";
            break;
        case RIL_IMS_REG_STATE_REGISTERING:
            os << "RIL_IMS_REG_STATE_REGISTERING";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_GeoLocationDataStatus state) {
  os << "RIL_IMS_GeoLocationDataStatus : ";
  switch (state) {
    case RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED";
      break;
  }
  return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_Registration& imsRegInfo)
{
    os << imsRegInfo.state << std::endl;
    if (imsRegInfo.state == RIL_IMS_REG_STATE_NOT_REGISTERED) {
        os << "Error Code : " << imsRegInfo.errorCode << std::endl;
        if (imsRegInfo.errorMessage) {
            os << "Error Message : " << imsRegInfo.errorMessage << std::endl;
        }
    }
    os << "Radio Technology : " << getRatFromValue(imsRegInfo.radioTech) << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ServiceStatusInfo* info)
{
    if (info != nullptr) {
        os << "RIL IMS SERVICE STATUS INFO : " << std::endl;
        os << info->callType << std::endl;
        os << info->accTechStatus << std::endl;
        os << info->rttMode << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallFailCauseResponse* info) {
    if (info) {
        os << "IMS CALL FAIL CAUSE RESPONSE :";
        os << "Fail Cause : " << info->failCause << std::endl;
        os << "Extended Fail Cause : " << info->extendedFailCause << std::endl;
        os << "Error Info : ";
        for(size_t i=0; i < info->errorInfoLen; i++) {
            os << info->errorInfo[i] << " ";
        }
        os << std::endl;
        if (info->networkErrorString) {
            os << "Network Error String : " << info->networkErrorString << std::endl;
        }
        os << info->errorDetails << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_UUS_Info* info) {
    os << "RIL UUS Info : " << std::endl;
    os << info->uusType << std::endl;
    os << info->uusDcs << std::endl;
    os << "uusLength : " << info->uusLength << std::endl;
    if(info->uusData && info->uusLength > 0) {
        os << "uusData : " << std::string(info->uusData, info->uusLength) << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_SipErrorInfo* info) {
    os << "IMS SIP ERROR INFO: " << std::endl;
    if(info != nullptr) {
        os << "Error Code : " << info->errorCode << std::endl;
        if(info->errorString != nullptr) {
            os << "Error String : " << info->errorString << std::endl;
        }
    }
    return os;
}

std::ostream& operator<<(ostream& out, const RIL_IMS_SipErrorInfo& sipErrorInfo) {
    out << "== IMS SIP Error Info ==" << std::endl;
    out << "    Error Code : " << sipErrorInfo.errorCode << std::endl;

    out << "    Error String : ";
    if (sipErrorInfo.errorString != nullptr) {
        out << sipErrorInfo.errorString;
    }

    return out;
}

ostream& operator<<(ostream& os, const RIL_IMS_VerstatInfo* info) {
    os << "IMS Verstat Info : " << std::endl;
    os << "canMarkUnwantedCall : " << info->canMarkUnwantedCall << std::endl;
    os << info->verificationStatus << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallProgressInfo* info) {
    os << "IMS Call Progress Info : " << std::endl;
    os << info->type << std::endl;
    os << "reasonCode : " << info->reasonCode << std::endl;
    os << "reasonText : ";
    if (info->reasonText) {
      os << info->reasonText;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_SubConfigInfo& info) {
    os << "IMS Sub Config Info : " << std::endl;
    os << "simultStackCount: " << info.simultStackCount << std::endl;
    os << "imsStackEnabledLen: " << info.imsStackEnabledLen << std::endl;
    os << "simultStack: [ ";
    for(int i=0; i < info.imsStackEnabledLen; i++) {
        os << static_cast<bool>(info.imsStackEnabled[i]) << " ";
    }
    os << "]" << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityRegistrationStatus& type) {
    os << "IMS MultiIdentity Registration Status:" << std::endl;
    switch(type) {
        case RIL_IMS_STATUS_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_STATUS_DISABLE:
            os << "DISABLE";
        break;
        case RIL_IMS_STATUS_ENABLE:
            os << "ENABLE";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityLineType& type) {
    os << "IMS MultiIdentity Line Type: " << std::endl;
    switch(type) {
        case RIL_IMS_LINE_TYPE_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_LINE_TYPE_PRIMARY:
            os << "PRIMARY";
        break;
        case RIL_IMS_LINE_TYPE_SECONDARY:
            os << "SECONDARY";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityLineInfo& info) {
    os << "IMS MultiIdentityLine Info : " << std::endl;
    os << "MSISDN: " << info.msisdn << std::endl;
    os << info.registrationStatus << std::endl;
    os << info.lineType << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HoExtraType& type) {
    os << "IMS Handover Extra Type: " << std::endl;
    switch(type) {
        case RIL_IMS_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL:
            os << "LTE_TO_IWLAN_HO_FAIL";
        break;
        case RIL_IMS_HO_XT_TYPE_INVALID:
            os << "INVALID";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HandoverType& type) {
    os << "IMS Handover Type: " << std::endl;
    switch(type) {
        case RIL_IMS_HO_START:
            os << "START";
        break;
        case RIL_IMS_HO_COMPLETE_SUCCESS:
            os << "COMPLETE_SUCCESS";
        break;
        case RIL_IMS_HO_COMPLETE_FAIL:
            os << "COMPLETE_FAIL";
        break;
        case RIL_IMS_HO_CANCEL:
            os << "CANCEL";
        break;
        case RIL_IMS_HO_NOT_TRIGGERED:
            os << "NOT_TRIGGERED";
        break;
        case RIL_IMS_HO_NOT_TRIGGERED_MOBILE_DATA_OFF:
            os << "NOT_TRIGGERED_MOBILE_DATA_OFF";
        break;
        case RIL_IMS_HO_INVALID:
            os << "INVALID";
        break;
        case RIL_IMS_HO_UNKNOWN:
            os << "UNKNOWN";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HoExtra& extra) {
    os << "IMS HoExtra: " << std::endl;
    os << "Extra Type: " << extra.type << std::endl;
    os << "Extra Info Len:" << extra.extraInfoLen << std::endl;
    for(int i=0; i<extra.extraInfoLen; i++) {
        os << "ExtraInfo[" << i << "]" << extra.extraInfo[i] << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HandoverInfo& info) {
    os << "IMS Handover Info : " << std::endl;
    os << info.type << std::endl;
    os << "Source RAT: " << getRatFromValue(info.srcTech) << std::endl;
    os << "Target RAT: " << getRatFromValue(info.targetTech) << std::endl;
    if (info.hoExtra) {
      os << *(info.hoExtra) << std::endl;
    } else {
      os << "IMS HoExtra: <null>" << std::endl;
    }
    os << "Error Code: " << (info.errorCode ? info.errorCode : "<null>") << std::endl;
    os << "Error Code Message: " << (info.errorMessage ? info.errorMessage : "<null>") << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ConferenceCallState& state) {
    os << "IMS Conference Call State: " << std::endl;
    switch(state) {
        case RIL_IMS_CONF_CALL_STATE_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_CONF_CALL_STATE_FOREGROUND:
            os << "FOREGROUND";
        break;
        case RIL_IMS_CONF_CALL_STATE_BACKGROUND:
            os << "BACKGROUND";
        break;
        case RIL_IMS_CONF_CALL_STATE_RINGING:
            os << "RINGING";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RefreshConferenceInfo& info) {
    os << "IMS Refresh Conference Info : " << std::endl;
    os << info.conferenceCallState << std::endl;
    os << "Conference Uri Len:" << info.confInfoUriLength << std::endl;
    for(int i=0; i<info.confInfoUriLength; i++) {
        os << "confInfoUri[" << i << "]" << info.confInfoUri[i] << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MsimAdditionalInfoCode code) {
    os << "RIL IMS MSIM Additional info code : ";
    switch (code) {
        case RIL_IMS_MSIM_ADDITIONAL_INFO_CONCURRENT_CALL_NOT_POSSIBLE:
            os << "RIL_IMS_MSIM_ADDITIONAL_INFO_CONCURRENT_CALL_NOT_POSSIBLE";
            break;
        case RIL_IMS_MSIM_ADDITIONAL_INFO_NONE:
            os << "RIL_IMS_MSIM_ADDITIONAL_INFO_NONE";
            break;
        default:
            os << "<Invalid>";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MsimAdditionalCallInfo* info) {
    os << "IMS MSIM additional call info : " << std::endl;
    os << "Code : " << info->additionalCode << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_Codec codec) {
  switch (codec) {
    case RIL_IMS_CODEC_NONE:
      os << "NONE";
      break;
    case RIL_IMS_CODEC_QCELP13K:
      os << "QCELP13K";
      break;
    case RIL_IMS_CODEC_EVRC:
      os << "EVRC";
      break;
    case RIL_IMS_CODEC_EVRC_B:
      os << "EVRC_B";
      break;
    case RIL_IMS_CODEC_EVRC_WB:
      os << "EVRC_WB";
      break;
    case RIL_IMS_CODEC_EVRC_NW:
      os << "EVRC_NW";
      break;
    case RIL_IMS_CODEC_AMR_NB:
      os << "AMR_NB";
      break;
    case RIL_IMS_CODEC_AMR_WB:
      os << "AMR_WB";
      break;
    case RIL_IMS_CODEC_GSM_EFR:
      os << "GSM_EFR";
      break;
    case RIL_IMS_CODEC_GSM_FR:
      os << "GSM_FR";
      break;
    case RIL_IMS_CODEC_GSM_HR:
      os << "GSM_HR";
      break;
    case RIL_IMS_CODEC_G711U:
      os << "G711U";
      break;
    case RIL_IMS_CODEC_G723:
      os << "G723";
      break;
    case RIL_IMS_CODEC_G711A:
      os << "G711A";
      break;
    case RIL_IMS_CODEC_G722:
      os << "G722";
      break;
    case RIL_IMS_CODEC_G711AB:
      os << "G711AB";
      break;
    case RIL_IMS_CODEC_G729:
      os << "G729";
      break;
    case RIL_IMS_CODEC_EVS_NB:
      os << "EVS_NB";
      break;
    case RIL_IMS_CODEC_EVS_WB:
      os << "EVS_WB";
      break;
    case RIL_IMS_CODEC_EVS_SWB:
      os << "EVS_SWB";
      break;
    case RIL_IMS_CODEC_EVS_FB:
      os << "EVS_FB";
      break;
    default:
      os << "<invalid>";
      break;
  }
  return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ComputedAudioQuality computedAudioQuality) {
  switch (computedAudioQuality) {
    case RIL_IMS_COMPUTED_AUDIO_QUALITY_NONE:
      os << "NONE";
      break;
    case RIL_IMS_COMPUTED_AUDIO_QUALITY_NO_HD:
      os << "NO_HD";
      break;
    case RIL_IMS_COMPUTED_AUDIO_QUALITY_HD:
      os << "HD";
      break;
    case RIL_IMS_COMPUTED_AUDIO_QUALITY_HD_PLUS:
      os << "HD_PLUS";
      break;
    default:
      os << "<invalid>";
      break;
  }
  return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_AudioQuality* info) {
  os << "IMS Audio Quality : " << std::endl;
  os << "Codec : " << info->codec << std::endl;
  os << "Computed Audio Quality : " << info->computedAudioQuality << std::endl;
  return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallInfo* info) {
    os << "IMS CALL INFO : ";
    if(info)
    {
        os << info->callState << std::endl;
        os << "Call Index : " << info->index << std::endl;
        os << "toa : " << info->toa << std::endl;
        os << "isMpty : " << static_cast<uint32_t>(info->isMpty) << std::endl;
        os << "isMt : " << static_cast<uint32_t>(info->isMt) << std::endl;
        os << "als : " << static_cast<uint32_t>(info->als) << std::endl;
        os << "isVoice : " << static_cast<uint32_t>(info->isVoice) << std::endl;
        os << "isVoicePrivacy : " << static_cast<uint32_t>(info->isVoicePrivacy) << std::endl;
        if(info->number) {
            os << "number : " << info->number << std::endl;
            os << "numberPresentation : " << info->numberPresentation << std::endl;
        }
        if(info->name) {
            os << "name : " << info->name << std::endl;
            os << "namePresentation : " << info->namePresentation << std::endl;
        }
        if(info->redirNum) {
            os << "redirNum : " << info->redirNum << std::endl;
            os << "redirNumPresentation : " << info->redirNumPresentation << std::endl;
        }
        os << info->callType << std::endl;
        os << info->callDomain << std::endl;
        os << "callSubState : " << info->callSubState << std::endl;
        os << "isEncrypted : " << static_cast<uint32_t>(info->isEncrypted) << std::endl;
        os << "isCalledPartyRinging : " << static_cast<uint32_t>(info->isCalledPartyRinging) << std::endl;
        os << "isVideoConfSupported : " << static_cast<uint32_t>(info->isVideoConfSupported) << std::endl;
        if(info->historyInfo) {
            os << "historyInfo : " << info->historyInfo << std::endl;
        }
        os << "mediaId : " << info->mediaId << std::endl;
        os << info->causeCode << std::endl;
        os << info->rttMode << std::endl;
        if(info->sipAlternateUri) {
            os << "sipAlternateUri : " << info->sipAlternateUri << std::endl;
        }
        os << "localAbilityLen" << info->localAbilityLen << std::endl;
        if(info->localAbility) {
            os << info->localAbility << std::endl;
        }
        os << "peerAbilityLen" << info->peerAbilityLen << std::endl;
        if(info->peerAbility) {
            os << info->peerAbility << std::endl;
        }
        os << info->callFailCauseResponse << std::endl;
        if(info->terminatingNumber) {
            os << "terminatingNumber : " << info->terminatingNumber << std::endl;
        }
        os << "isSecondary : " << static_cast<uint32_t>(info->isSecondary) << std::endl;
        if (info->verstatInfo) {
            os << info->verstatInfo << std::endl;
        }
        if (info->uusInfo) {
            os << info->uusInfo << std::endl;
        }
        if (info->audioQuality) {
            os << *(info->audioQuality) << std::endl;
        }
        os << "Codec : " << info->Codec << std::endl;
        if (info->displayText) {
            os << "displayText : " << info->displayText << std::endl;
        }
        if (info->additionalCallInfo) {
            os << "additionalCallInfo : " << info->additionalCallInfo << std::endl;
        }
        if (info->childNumber) {
            os << "childNumber : " << info->childNumber << std::endl;
        }
        os << "emergencyServiceCategory : " << info->emergencyServiceCategory << std::endl;
        os << info->tirMode << std::endl;
        if (info->callProgInfo) {
            os << info->callProgInfo << std::endl;
        }
        if (info->msimAdditionalCallInfo) {
          os << info->msimAdditionalCallInfo << std::endl;
        }
        if (info->imsAudioQuality) {
          os << info->imsAudioQuality << std::endl;
        }
        os << "Call Modem Call Id : " << info->modemCallId << std::endl;
    }

    return os;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_SmsFormat imsSmsFormat) {
    out << "IMS SMS Format: ";
    switch (imsSmsFormat) {
        case RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP:
            out << "3GPP";
            break;
        case RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP2:
            out << "3GPP2";
            break;
        default:
            out << "Unknown";
            break;
    }
    return out;
}

void printSmsPdu(std::ostream& out, const uint8_t* pdu, const uint32_t pduLength) {
    out << std::hex;
    for (uint32_t i = 0; i < pduLength; i++) {
        out << static_cast<unsigned int>(pdu[i]) << " ";
    }
    out << std::dec;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_IncomingSms& incomingImsSms) {
    out << "== Incoming SMS Over IMS ==" << std::endl
        << incomingImsSms.format << std::endl
        << incomingImsSms.verificationStatus << std::endl
        << "    PDU:" << std::endl;
    printSmsPdu(out, incomingImsSms.pdu, incomingImsSms.pduLength);
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_SmsStatusReport& imsSmsStatusReport) {
    out << "== SMS Over IMS Status Report ==" << std::endl
        << imsSmsStatusReport.format << std::endl
        << "    SMS Message Reference: " << imsSmsStatusReport.messageRef << std::endl
        << "    PDU:" << std::endl;
    printSmsPdu(out, imsSmsStatusReport.pdu, imsSmsStatusReport.pduLength);
    return out;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_TtyModeType ttyMode) {
    out << "IMS TTY Mode: ";
    switch (ttyMode) {
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_OFF:
            out << "OFF";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_FULL:
            out << "FULL";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_HCO:
            out << "HCO";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_VCO:
            out << "VCO";
            break;
        default:
            out << "INVALID";
            break;
    }
    return out;
}

// TODO: Come up with a common templated prompt generation and user input
// TODO: extraction function that accepts a prompt, range of valid values,
// TODO: and an exit value. The function returns the extracted value and
// TODO: indicates if the exit value was input.
std::istream& operator >>(std::istream& in, RIL_IMS_SmsMessage& imsSmsMessage) {
    std::string destinationAddress;
    do {
        in.clear();
        std::cout << "Enter the number to send SMS to (-1 - Exit): ";
        in >> destinationAddress;
    } while (in.fail());

    if (destinationAddress == "-1") {
        return in;
    }

    if (destinationAddress.size() > 15) {
        return in;
    }

    if (destinationAddress.size() % 2) {
        destinationAddress.append(1, 'F');
    }

    for (std::string::size_type i = 0; i < destinationAddress.size(); i += 2) {
        std::swap(destinationAddress[i], destinationAddress[i + 1]);
    }

    static uint8_t imsSmsTestPduPart1[] = {1, 0, 11, 145};
    imsSmsTestPduPart1[2] = destinationAddress.size();
    // message body "Hello World"
    static uint8_t imsSmsTestPduPart2[] = {0, 0, 11, 145, 50, 155, 253, 6, 93, 223, 114, 54, 25};

    size_t pduLength = sizeof(imsSmsTestPduPart1) / sizeof(uint8_t) + sizeof(imsSmsTestPduPart2) / sizeof(uint8_t) + destinationAddress.size() / 2;
    imsSmsMessage.pdu = new uint8_t[pduLength];
    if (imsSmsMessage.pdu != nullptr) {
        imsSmsMessage.pduLength = pduLength;
        uint8_t* pduCursor = imsSmsMessage.pdu;
        std::copy(imsSmsTestPduPart1, imsSmsTestPduPart1 + sizeof(imsSmsTestPduPart1) / sizeof(uint8_t), pduCursor);
        pduCursor += sizeof(imsSmsTestPduPart1) / sizeof(uint8_t);

        // append destination number
        for (auto iter = destinationAddress.begin(); iter != destinationAddress.end(); iter += 2) {
            std::istringstream(std::string(iter, iter + 2)) >> std::hex >> *pduCursor;
            pduCursor++;
        }

        std::copy(imsSmsTestPduPart2, imsSmsTestPduPart2 + sizeof(imsSmsTestPduPart2) / sizeof(uint8_t), pduCursor);
    } else {
        imsSmsMessage.pduLength = 0;
    }

    imsSmsMessage.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
    imsSmsMessage.messageRef = 0;
    imsSmsMessage.shallRetry = false;
    imsSmsMessage.smsc = nullptr;
    imsSmsMessage.smscLength = 0;

    return in;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_SmsSendStatus imsSmsSendStatus) {
    out << "IMS SMS Send Status: ";
    switch (imsSmsSendStatus) {
        case RIL_IMS_SMS_SEND_STATUS_OK:
            out << "Success.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR:
            out << "Error. Do not retry sending the messager over CS.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR_RETRY:
            out << "Error. Retry sending message using the message reference.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR_FALLBACK:
            out << "Error. Retry sending message over CS.";
            break;
        default:
            out << "Unknown.";
            break;
    }
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_SendSmsResponse& imsSendSmsResponse) {
    out << "== MO SMS Over IMS Response ==" << std::endl;
    out << "    " << imsSendSmsResponse.status << std::endl;
    out << "    Message Ref: " << imsSendSmsResponse.messageRef << std::endl;
    out << "    RAT: " << imsSendSmsResponse.rat << std::endl;
    return out;
}

std::istream& operator >>(std::istream& in, RIL_IMS_SmsDeliveryStatus& imsSmsDeliveryStatus) {
    int userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the delivery status of the SMS being ACK'd"
            << " (0 - Success, 1 - Error, 2 - Storage Unavailable"
            << ", 3 - SMS Not Supported On Device, -1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1 || userInput > 3);

    imsSmsDeliveryStatus = static_cast<RIL_IMS_SmsDeliveryStatus>(userInput);
    return in;
}

std::istream& operator >>(std::istream& in, RIL_IMS_SmsAck& imsSmsAck) {
    int32_t userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the ID/reference of the SMS message to ACK (-1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1);

    if (userInput == -1) {
        imsSmsAck.messageRef = std::numeric_limits<uint32_t>::max();
        return in;
    } else {
        imsSmsAck.messageRef = userInput;
    }

    in >> imsSmsAck.deliveryStatus;
    return in;
}

std::istream& operator >>(std::istream& in, RIL_IMS_CallWaitingSettings& imsCallwaitingSettings) {
    int32_t userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the service class to enable/disable IMS call waiting on (-1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1);

    if (userInput == -1) {
        imsCallwaitingSettings.serviceClass = std::numeric_limits<uint32_t>::max();
        return in;
    } else {
        imsCallwaitingSettings.serviceClass = userInput;
    }

    do {
        in.clear();
        std::cout << "Do you want to enable or disable call waiting (0 - Disable, !0 - Enable)? ";
        in >> imsCallwaitingSettings.enabled;
    } while (in.fail());

    return in;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_CallWaitingSettings& imsCallwaitingSettings) {
    out << "== IMS Call Waiting Setting ==" << std::endl;
    out << "    Service Class: " << imsCallwaitingSettings.serviceClass << std::endl;
    out << "    State: " << (imsCallwaitingSettings.enabled ? "Enabled" : "Disabled");
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_QueryCallWaitingResponse& imsQueryCallWaitingResp) {
    out << imsQueryCallWaitingResp.sipError << std::endl;
    out << imsQueryCallWaitingResp.callWaitingSettings;
    return out;
}

std::istream& operator >>(std::istream& in, RIL_IMS_ConfigInfo& imsConfigInfo) {
    imsConfigInfo.item = RIL_IMS_ConfigItem::RIL_IMS_CONFIG_ITEM_INVALID;
    imsConfigInfo.hasBoolValue = 0;
    imsConfigInfo.boolValue = 0;
    imsConfigInfo.hasIntValue = 0;
    imsConfigInfo.intValue = 0;
    imsConfigInfo.stringValue = nullptr;
    imsConfigInfo.hasErrorCause = 0;
    imsConfigInfo.errorCause = RIL_IMS_ConfigFailureCause::RIL_IMS_CONFIG_FAILURE_CAUSE_INVALID;

    int32_t configId = -1;
    do {
        in.clear();
        std::cout << "Enter the ID (a value from enum RIL_IMS_REQUEST_SET_IMS_CONFIG) of the IMS config to set (-1 - Exit): ";
        in >> configId;
    } while (in.fail() || configId < -1
            || configId >= static_cast<int32_t>(RIL_IMS_ConfigItem::RIL_IMS_CONFIG_ITEM_INVALID));

    if (configId == -1) {
        return in;
    }

    int32_t configValueType = 0;
    do {
        in.clear();
        std::cout << "Enter the type of value the IMS config requires (0 - boolean, 1 - Integer, 2 - String, -1 - Exit): ";
        in >> configValueType;
    } while (in.fail() || configValueType < -1 || configValueType > 2);

    if (configValueType == -1) {
        return in;
    }

    int32_t configIntegerValue = 0;
    std::string configStringValue;

    if (configValueType <= 1) {
        do {
            in.clear();
            std::cout << "Enter the value for the IMS config (-1 - Exit): ";
            in >> configIntegerValue;
        } while (in.fail() || configIntegerValue < -1);

        if (configIntegerValue == -1) {
            return in;
        }
    } else {
        do {
            in.clear();
            std::cout << "Enter the value for the IMS config (-1 - Exit): ";
            in >> configStringValue;
        } while (in.fail());

        if (configStringValue == "-1") {
            return in;
        }
    }

    imsConfigInfo.item = static_cast<RIL_IMS_ConfigItem>(configId);

    switch(configValueType) {
        case 0:
            imsConfigInfo.hasBoolValue = 1;
            imsConfigInfo.boolValue = static_cast<bool>(configIntegerValue);
            break;
        case 1:
            imsConfigInfo.hasIntValue = 1;
            imsConfigInfo.intValue = configIntegerValue;
            break;
        case 2:
            imsConfigInfo.stringValue = new char[configStringValue.size() + 1];
            if (imsConfigInfo.stringValue != nullptr) {
                std::copy(configStringValue.begin(), configStringValue.end(), imsConfigInfo.stringValue);
                imsConfigInfo.stringValue[configStringValue.size()] = '\0';
            } else {
                return in;
            }
            break;
        default:
            return in;
    }

    return in;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_ConfigInfo& imsConfigInfo) {
    out << "== IMS Config Info ==" << std::endl;
    out << "   Config ID: " << static_cast<int32_t>(imsConfigInfo.item) << std::endl;
    out << "   hasBoolValue: " << imsConfigInfo.hasBoolValue << std::endl;
    out << "   boolValue: " << imsConfigInfo.boolValue << std::endl;
    out << "   hasIntValue: " << imsConfigInfo.hasIntValue << std::endl;
    out << "   intValue: " << imsConfigInfo.intValue << std::endl;
    out << "   stringValue: " << (imsConfigInfo.stringValue ? imsConfigInfo.stringValue : "NULL") << std::endl;
    out << "   hasErrorCause: " << imsConfigInfo.hasErrorCause << std::endl;
    out << "   errorCause: " << static_cast<int32_t>(imsConfigInfo.errorCause) << std::endl;

    return out;
}

void ImsMenu::registrationStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.imsGetRegistrationState(
      [](RIL_Errno err, const RIL_IMS_Registration& info) -> void {
        std::cout << "Got response for registrationStatus request: " << err << std::endl;
        std::cout << info << std::endl;
      });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send registrationStatus" << std::endl;
  }
}

void ImsMenu::imsDial(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IMS_Dial dialParams = {};
  dialParams.address = const_cast<char*>(userInput[1].c_str());

  Status s = rilSession.imsDial(dialParams, [](RIL_Errno err) -> void {
    std::cout << "Got response for Ims dial request: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDial" << std::endl;
  }
}

void ImsMenu::acceptCall(std::vector<std::string> userInput) {
  RIL_IMS_Answer answer {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> answer;
  Status s = rilSession.imsAnswer(answer, [](RIL_Errno err) -> void {
    std::cout << "Got response for Answer request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send acceptCall" << std::endl;
  }
}

void ImsMenu::hangupCall(std::vector<std::string> userInput) {
  RIL_IMS_Hangup hangup {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> hangup;
  Status s = rilSession.imsHangup(hangup, [](RIL_Errno err) -> void {
    std::cout << "Got response for Hangup request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send hangupCall" << std::endl;
  }
}

void ImsMenu::imsAddParticipant(std::vector<std::string> userInput) {
    std::string address;
    std::cout << "Enter the address of the participant to add to the call: ";
    std::cin >> address;
    Status s = rilSession.imsAddParticipant(
        address.c_str(),
        [] (RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
            std::cout << "Add participant error code: " << err << std::endl;
            std::cout << &errorDetails << std::endl;
        }
    );

    if(s != Status::SUCCESS) {
        std::cout << "Failed to send request to add participant to the call." << std::endl;
    }
}

void ImsMenu::imsSetImsConfig(std::vector<std::string> userInput) {
    RIL_IMS_ConfigInfo configInfo;
    std::cin >> configInfo;
    if (configInfo.item == RIL_IMS_ConfigItem::RIL_IMS_CONFIG_ITEM_INVALID) {
        std::cout << "Invalid config." << std::endl;
        return;
    }

    Status s = rilSession.imsSetImsConfig(
        configInfo,
        [] (RIL_Errno err, const RIL_IMS_ConfigInfo& config) -> void {
            std::cout << "Set IMS config error code: " << err << std::endl;
            std::cout << config << std::endl;
        }
    );

    if(s != Status::SUCCESS) {
        std::cout << "Failed to send request to set IMS config." << std::endl;
    }

    if (configInfo.stringValue != nullptr) {
        delete[] configInfo.stringValue;
    }
}

void ImsMenu::imsGetImsConfig(std::vector<std::string> userInput) {
    RIL_IMS_ConfigInfo configInfo;
    std::cin >> configInfo;
    if (configInfo.item == RIL_IMS_ConfigItem::RIL_IMS_CONFIG_ITEM_INVALID) {
        std::cout << "Invalid config." << std::endl;
        return;
    }

    Status s = rilSession.imsGetImsConfig(
        configInfo,
        [] (RIL_Errno err, const RIL_IMS_ConfigInfo& config) -> void {
            std::cout << "Get IMS config error code: " << err << std::endl;
            std::cout << config << std::endl;
        }
    );

    if(s != Status::SUCCESS) {
        std::cout << "Failed to send request to get IMS config." << std::endl;
    }

    if (configInfo.stringValue != nullptr) {
        delete[] configInfo.stringValue;
    }
}

void ImsMenu::imsRegistrationChange(std::vector<std::string> userInput) {
  RIL_IMS_RegistrationState state {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> state;
  Status s = rilSession.imsRegistrationChange(state, [](RIL_Errno err) -> void {
    std::cout << "Got response for RegistrationChange request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsRegistrationChange" << std::endl;
  }
}

void ImsMenu::imsQueryServiceStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsQueryServiceStatus([](RIL_Errno err,
    const size_t count, const RIL_IMS_ServiceStatusInfo** statusInfo) -> void {
    std::cout << "Got response for Query ServiceStatus request." << err << std::endl;
    std::cout << "count: " << count << std::endl;
    if (statusInfo) {
        for(int i = 0; i < count; i++) {
            cout << statusInfo[i];
        }
    }
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsQueryServiceStatus" << std::endl;
  }
}

void ImsMenu::imsSetServiceStatus(std::vector<std::string> userInput) {
  size_t numSsInfos = 1;
  RIL_IMS_ServiceStatusInfo info {};
  RIL_IMS_ServiceStatusInfo **ssInfos = nullptr;
  cin >> info;
  ssInfos = new RIL_IMS_ServiceStatusInfo* [1]();
  if(ssInfos) {
    ssInfos[0] = &info;
  }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetServiceStatus(
    numSsInfos, (const RIL_IMS_ServiceStatusInfo**)ssInfos, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetServiceStatus request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetServiceStatus" << std::endl;
  }
  if (ssInfos) {
    delete[] ssInfos;
  }
}

void ImsMenu::imsGetImsSubConfig(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsGetImsSubConfig([](RIL_Errno err,
    const RIL_IMS_SubConfigInfo& config) -> void {
      std::cout << "Got response for Get Ims Sub Config request." << err << std::endl;
      cout << config << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsGetImsSubConfig" << std::endl;
  }
}

void ImsMenu::imsDtmf(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  char digit;
  std::cout << "Enter digit:";
  std::cin >> digit;
  Status s = rilSession.imsDtmf(digit, [](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmf request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmf" << std::endl;
  }
}

void ImsMenu::imsDtmfStart(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  char digit;
  std::cout << "Enter digit:";
  std::cin >> digit;
  Status s = rilSession.imsDtmfStart(digit, [](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmfStart request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmfStart" << std::endl;
  }
}

void ImsMenu::imsDtmfStop(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsDtmfStop([](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmfStop request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmfStop" << std::endl;
  }
}

void ImsMenu::imsModifyCallInitiate(std::vector<std::string> userInput) {
  RIL_IMS_CallModifyInfo modifyInfo{};
  std::cin >> modifyInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsModifyCallInitiate(modifyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsModifyCallInitiate request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsModifyCallInitiate" << std::endl;
  }
}

void ImsMenu::imsModifyCallConfirm(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_IMS_CallModifyInfo modifyInfo{};
  std::cin >> modifyInfo;
  Status status = rilSession.imsModifyCallConfirm(modifyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsModifyCallConfirm request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsModifyCallConfirm" << std::endl;
  }
}

void ImsMenu::imsCancelModifyCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsCancelModifyCall(callId, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsCancelModifyCall request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsCancelModifyCall" << std::endl;
  }
}

void ImsMenu::imsRequestConference(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsRequestConference(
      [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestConference request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestConference" << std::endl;
  }
}

void ImsMenu::imsRequestHoldCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsRequestHoldCall(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestHoldCall request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestHoldCall" << std::endl;
  }
}

void ImsMenu::imsRequestResumeCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsRequestResumeCall(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestResumeCall request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestResumeCall" << std::endl;
  }
}

void ImsMenu::imsRequestExitEmergencyCallbackMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsRequestExitEmergencyCallbackMode([](RIL_Errno err) -> void {
    std::cout << "Got response for imsRequestExitEmergencyCallbackMode request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestExitEmergencyCallbackMode" << std::endl;
  }
}

void ImsMenu::imsSendRttMessage(std::vector<std::string> userInput) {
  for (auto& arg : userInput) {
    std::cout << __func__ << ":" << arg << std::endl;
  }

  std::string msg;
  std::cout << "Enter rtt message:" << std::endl;
  std::cin >> msg;

  Status s = rilSession.imsSendRttMessage(msg.c_str(), msg.size(), [](RIL_Errno err) -> void {
    std::cout << "Got response for imsSendRttMessage request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendRttMessage" << std::endl;
  }
}

void ImsMenu::imsSendGeolocationInfo(std::vector<std::string> userInput) {
  for (auto& arg : userInput) {
    std::cout << __func__ << ":" << arg << std::endl;
  }

  RIL_IMS_GeolocationInfo geolocationInfo{};

  std::cout << "Enter latitude:" << std::endl;
  std::cin >> geolocationInfo.latitude;
  std::cout << "Enter longitude:" << std::endl;
  std::cin >> geolocationInfo.longitude;

  std::string countryCode;
  std::cout << "Enter country code:" << std::endl;
  std::cin >> countryCode;
  geolocationInfo.countryCode = const_cast<char*>(countryCode.c_str());
  std::string country;
  std::cout << "Enter country:" << std::endl;
  std::cin >> country;
  geolocationInfo.country = const_cast<char*>(country.c_str());
  std::string state;
  std::cout << "Enter state:" << std::endl;
  std::cin >> state;
  geolocationInfo.state = const_cast<char*>(state.c_str());
  std::string city;
  std::cout << "Enter city:" << std::endl;
  std::cin >> city;
  geolocationInfo.city = const_cast<char*>(city.c_str());
  std::string postalCode;
  std::cout << "Enter postal code:" << std::endl;
  std::cin >> postalCode;
  geolocationInfo.postalCode = const_cast<char*>(postalCode.c_str());
  std::string street;
  std::cout << "Enter street:" << std::endl;
  std::cin >> street;
  geolocationInfo.street = const_cast<char*>(street.c_str());
  std::string houseNumber;
  std::cout << "Enter house number:" << std::endl;
  std::cin >> houseNumber;
  geolocationInfo.houseNumber = const_cast<char*>(houseNumber.c_str());

  Status s = rilSession.imsSendGeolocationInfo(geolocationInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsSendGeolocationInfo request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendGeolocationInfo" << std::endl;
  }
}

void ImsMenu::imsSendHelloWorldSms(std::vector<std::string> userInput) {
    RIL_IMS_SmsMessage imsSmsMessage = {};
    std::cin >> imsSmsMessage;

    if (imsSmsMessage.pdu == nullptr) {
        return;
    }

    Status s = rilSession.imsSendSms(
        imsSmsMessage,
        [] (RIL_Errno e, const RIL_IMS_SendSmsResponse& resp) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully sent SMS over IMS." : "Failed to send SMS over IMS.")
                << " Error: " << e << "." << std::endl;
            std::cout << resp;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl << "Failed to issue request to send SMS over IMS."
            << std::endl;
    }

    if (imsSmsMessage.pdu != nullptr) {
        delete[] imsSmsMessage.pdu;
        imsSmsMessage.pdu = nullptr;
        imsSmsMessage.pduLength = 0;
    }
}

void ImsMenu::imsSendSms(std::vector<std::string> userInput) {
    int pduByte = -1;
    std::basic_string<uint8_t> pdu;
    bool exit = false;
    std::istringstream pduStream;
    constexpr size_t maxPduBytes = 256;
    char input[maxPduBytes * 3];

    do {
        pdu.clear();
        std::cout << "Enter upto " << maxPduBytes
            << " PDU bytes separated by space with each byte encoded as ascii hex (100 - Exit): ";

        memset(input, 0, sizeof(input));

        std::cin.clear();
        std::cin.getline(input, sizeof(input));

        if (std::cin.eof()) {
            exit = true;
            break;
        } else if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::istringstream inputStream((std::string(input)));
        pduStream.swap(inputStream);

        do {
            pduByte = -1;
            pduStream >> std::hex >> pduByte;

            if (pduStream.fail()) {
                break;
            } else if (pduByte == 0x100 && pdu.length() == 0) {
                // if 0x100 is the value for the first byte, exit
                exit = true;
                break;
            } else if (pduByte < 0 || pduByte > 255) {
                std::cout << "Value for byte " << (pdu.length() + 1)
                        << " must be between 0 to 255." << std::endl;
                break;
            }

            pdu += static_cast<uint8_t>(pduByte);
        } while (!pduStream.eof());
    } while (!exit && (std::cin.fail() || pduStream.fail() || pduByte < 0 || pduByte > 255));

    if (exit) {
        std::cout << std::endl << "exiting imsSendSms" << std::endl;
        return;
    }

    std::string tempString;
    std::cout << "Enter SMSC Address" << std::endl;
    std::cout << "Provide SMSC address or empty (\"\"), empty is preferred : ";
    std::string smscAddress = "";
    std::cin >> tempString;
    if (!tempString.empty() && tempString != "\"\"") {
        smscAddress = tempString;
    }

    std::cout << "Enter message reference" << std::endl;
    std::cout << "0 - preferred, Provide proper reference number for retry : ";
    int messageRef = 0;
    std::cin >> messageRef;

    std::cout << "Enter isRetry" << std::endl;
    std::cout << "0 - preferred for normal SMS, !0 for retry message : ";
    int isRetry = 0;
    std::cin >> isRetry;
    bool shallRetry = static_cast<bool>(isRetry);

    if (shallRetry && messageRef <= 0) {
        std::cout << std::endl << "Exiting imsSendSms as messageRef count is not correct for retry"
                  << std::endl;
        return;
    }

    RIL_IMS_SmsMessage imsSmsMessage = {};
    imsSmsMessage.pdu = pdu.data();
    imsSmsMessage.pduLength = pdu.length();
    imsSmsMessage.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
    imsSmsMessage.messageRef = messageRef;
    imsSmsMessage.shallRetry = shallRetry;
    imsSmsMessage.smsc = const_cast<char*>(smscAddress.c_str());
    imsSmsMessage.smscLength = smscAddress.length();

    Status s = rilSession.imsSendSms(
        imsSmsMessage,
        [] (RIL_Errno e, const RIL_IMS_SendSmsResponse& resp) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully sent SMS over IMS." : "Failed to send SMS over IMS.")
                << " Error: " << e << "." << std::endl;
            std::cout << resp;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl << "Failed to issue request to send SMS over IMS."
            << std::endl;
    }
}

void ImsMenu::imsAckSms(std::vector<std::string> userInput) {
    RIL_IMS_SmsAck smsAck = {};
    std::cin >> smsAck;

    if (smsAck.messageRef == std::numeric_limits<uint32_t>::max()
            || smsAck.deliveryStatus == -1) {
        return;
    }

    Status s = rilSession.imsAckSms(
        smsAck,
        [] (RIL_Errno e) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully ack'd SMS." : "Failed to ack SMS.")
                << " Error: " << e << "." << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl << "Failed to issue request to ack SMS."
            << std::endl;
    }
}

void ImsMenu::imsGetSmsFormat(std::vector<std::string> userInput) {
    Status s = rilSession.imsGetSmsFormat(
        [] (RIL_Errno e, RIL_IMS_SmsFormat smsFormat) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully got the current SMS format." :
                "Failed to get the current SMS format.")
                << " Error: " << e << ". "
                << smsFormat << "." << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to get the current SMS format."
            << std::endl;
    }
}

void ImsMenu::imsSetCallWaiting(std::vector<std::string> userInput) {
    RIL_IMS_CallWaitingSettings callWaitingSettings {};
    std::cin >> callWaitingSettings;

    if (callWaitingSettings.serviceClass == std::numeric_limits<uint32_t>::max()) {
        return;
    }

    Status s = rilSession.imsSetCallWaiting(
        callWaitingSettings,
        [] (RIL_Errno e, const RIL_IMS_SipErrorInfo sipErrorInfo) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully set IMS call waiting." :
                "Failed to set IMS call waiting.")
                << " Error: " << e << "."
                << std::endl << sipErrorInfo << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to set IMS call waiting."
            << std::endl;
    }
}

void ImsMenu::imsQueryCallWaiting(std::vector<std::string> userInput) {
    int32_t _userInput = -1;
    do {
        std::cin.clear();
        std::cout << "Enter the service class to query IMS call waiting status for (-1 - Exit): ";
        std::cin >> _userInput;
    } while (std::cin.fail() || _userInput < -1);

    if (_userInput == -1) {
        return;
    }

    uint32_t serviceClass = _userInput;
    Status s = rilSession.imsQueryCallWaiting(
        serviceClass,
        [] (RIL_Errno e, const RIL_IMS_QueryCallWaitingResponse& callWaitingResp) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully queried IMS call waiting status." :
                "Failed to query IMS call waiting status.")
                << " Error: " << e << "."
                << std::endl << callWaitingResp << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to query IMS call waiting status."
            << std::endl;
    }
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiSimVoiceCapability& voiceCapability) {
  os << "Multisim Voice Capability: ";
  switch (voiceCapability) {
    case RIL_IMS_NONE:
      os << "NONE";
      break;
    case RIL_IMS_DSSS:
      os << "DSSS";
      break;
    case RIL_IMS_DSDS:
      os << "DSDS";
      break;
    case RIL_IMS_PSEUDO_DSDA:
      os << "PSEUDO_DSDA";
      break;
    case RIL_IMS_DSDA:
      os << "DSDA";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

void ImsMenu::imsQueryMultiSimVoiceCapability(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsQueryMultiSimVoiceCapability(
      [](RIL_Errno err, const RIL_IMS_MultiSimVoiceCapability& voiceCapability) -> void {
        std::cout << "Got response for query multi sim voice capability request. error: " << err
                  << std::endl;
        std::cout << voiceCapability << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsQueryMultiSimVoiceCapability" << std::endl;
  }
}

void ImsMenu::imsSendUssd(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.imsSendUssd(userInput[1], [](RIL_Errno err) -> void {
    std::cout << "Got response for SendUSSD request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "SendUSSD request sent succesfully"
                                       : "Failed to send SendUSSD")
            << std::endl;
}

void ImsMenu::imsCancelUssd(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.imsCancelUssd([](RIL_Errno err) -> void {
    std::cout << "Got response for CancelUSSD request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "CancelUSSD request sent succesfully"
                                       : "Failed to send CancelUSSD")
            << std::endl;
}

std::istream& operator >> (std::istream &in, RIL_IMS_CallFwdTimerInfo &timerInfo) {
  std::cout << "Input Timer Info - " << std::endl;
  std::cout << "Enter year:" << std::endl;
  in >> timerInfo.year;
  std::cout << "Enter month:" << std::endl;
  in >> timerInfo.month;
  std::cout << "Enter day:" << std::endl;
  in >> timerInfo.day;
  std::cout << "Enter hour:" << std::endl;
  in >> timerInfo.hour;
  std::cout << "Enter minute:" << std::endl;
  in >> timerInfo.minute;
  std::cout<< "Enter second:" << std::endl;
  in >> timerInfo.second;
  std::cout<< "Enter timezone:" << std::endl;
  in >> timerInfo.timezone;

  return in;
}

std::ostream& operator <<(std::ostream& os, const RIL_IMS_CallFwdTimerInfo &timerInfo) {
  os << "Date(DD/MM/YYYY) and Time(HH:MM:SS): ";
  os << timerInfo.day << "/" << timerInfo.month << "/" << timerInfo.year << " ";
  os << timerInfo.hour << ":" << timerInfo.minute << ":" << timerInfo.second << std::endl;
  os << "TimeZone: " << timerInfo.timezone << std::endl;

  return os;
}

std::istream& operator >> (std::istream &in, RIL_IMS_CallForwardInfo &info) {
  std::cout << "Input Call Forward Info - " << std::endl;

  std::cout << "0. disable" << std::endl;
  std::cout << "1. enable" << std::endl;
  std::cout << "3. registeration" << std::endl;
  std::cout << "4. erasure" << std::endl;
  std::cout << "Enter status(integer type):" << std::endl;
  in >> info.status;

  std::cout << "0. unconditional" << std::endl;
  std::cout << "1. mobile busy" << std::endl;
  std::cout << "2. no reply" << std::endl;
  std::cout << "3. not reachable" << std::endl;
  std::cout << "4. all call forwarding" << std::endl;
  std::cout << "5. all conditional call forwarding" << std::endl;
  std::cout << "Enter reason(integer type):" << std::endl;
  in >> info.reason;

  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services):" << std::endl;
  in >> info.serviceClass;

  std::cout << "Enter type of number(145 or 129):" << std::endl;
  in >> info.toa;

  std::string num;
  std::cout << "Enter number:" << std::endl;
  in >> num;
  auto len = num.length();
  if (len > 0) {
      info.number = new char[len + 1]();
      if (info.number) {
          strlcpy(info.number, num.c_str(), len + 1);
      } else {
          std::cerr << "failed to allocate number string" << std::endl;
      }
  }

  std::cout<< "Enter timer value in seconds:" << std::endl;
  in >> info.timeSeconds;

  std::cout << "Has call forward timer start? (1 - Yes, 0 - No):" << std::endl;
  uint32_t hasCallForwardTimerStart;

  do {
    in >> hasCallForwardTimerStart;
  } while (hasCallForwardTimerStart != 0 && hasCallForwardTimerStart != 1);

  info.hasCallFwdTimerStart = static_cast<uint8_t>(hasCallForwardTimerStart);
  if (info.hasCallFwdTimerStart) {
    in >> info.callFwdTimerStart;
  }

  std::cout << "Has call forward timer end? (1 - Yes, 0 - No):" << std::endl;
  uint32_t hasCallForwardTimerEnd;

  do {
    in >> hasCallForwardTimerEnd;
  } while (hasCallForwardTimerEnd != 0 && hasCallForwardTimerEnd != 1);

  info.hasCallFwdTimerEnd = static_cast<uint8_t>(hasCallForwardTimerEnd);
  if (info.hasCallFwdTimerEnd) {
    in >> info.callFwdTimerEnd;
  }

  return in;
}

std::ostream& operator <<(std::ostream& os, const RIL_IMS_CallForwardInfo &arg) {
  os << "RIL_IMS_CallForwardInfo :" << std::endl;
  os << "status: " << arg.status << std::endl;
  os << "reason: " << arg.reason << std::endl;
  os << "serviceClass: " << arg.serviceClass << std::endl;
  os << "toa: " << arg.toa << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  os << "timeSeconds: " << arg.timeSeconds << std::endl;

  if (arg.hasCallFwdTimerStart) {
    os << "Start Call Forward Timer Info:" << std::endl;
    os << arg.callFwdTimerStart;
  }

  if (arg.hasCallFwdTimerEnd) {
    os << "End Call Forward Timer Info:" << std::endl;
    os << arg.callFwdTimerEnd;
  }

  return os;
}

std::ostream& operator<<(std::ostream& out, const RIL_IMS_VoiceInfoType data) {
  out << "IMS SMS Send Status: ";
  switch (data) {
    case RIL_IMS_VOICE_INFO_SILENT:
      out << "RIL_IMS_VOICE_INFO_SILENT";
      break;
    case RIL_IMS_VOICE_INFO_SPEECH:
      out << "RIL_IMS_VOICE_INFO_SPEECH";
      break;
    default:
      out << "Unknown.";
      break;
  }
  return out;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_CfData& cfData) {
  os << "RIL_IMS_CfData : " << std::endl;
  os << " numValidIndexes : " << cfData.numValidIndexes << std::endl;
  for (uint32_t i = 0; i < cfData.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    os << " cfInfo[" << i << "] : " << std::endl;
    os << cfData.cfInfo[i] << std::endl;
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_StkCcUnsolSsResponse& suppSvcNotif) {
  os << "RIL_IMS_StkCcUnsolSsResponse : " << std::endl;
  os << " serviceType : " << std::endl << suppSvcNotif.serviceType << std::endl;
  os << " requestType : " << suppSvcNotif.requestType << std::endl;
  os << " teleserviceType : " << suppSvcNotif.teleserviceType << std::endl;
  os << " serviceClass : " << suppSvcNotif.serviceClass << std::endl;
  os << " result : " << suppSvcNotif.result << std::endl;

  if ((suppSvcNotif.requestType == SS_INTERROGATION) &&
      (suppSvcNotif.serviceType == SS_CFU || suppSvcNotif.serviceType == SS_CF_BUSY ||
       suppSvcNotif.serviceType == SS_CF_NO_REPLY ||
       suppSvcNotif.serviceType == SS_CF_NOT_REACHABLE || suppSvcNotif.serviceType == SS_CF_ALL ||
       suppSvcNotif.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    os << " cfData: " << suppSvcNotif.cfData;
  } else {
    for (size_t i = 0; i < SS_INFO_MAX; i++) {
      os << " ssInfo[" << i << "]: " << suppSvcNotif.ssInfo[i] << std::endl;
    }
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_SuppSvcNotification& suppSvcNotif) {
  os << "RIL_IMS_SuppSvcNotification : " << std::endl;
  os << " notificationType : " << suppSvcNotif.notificationType << std::endl;
  os << " code : " << suppSvcNotif.code << std::endl;
  os << " index : " << suppSvcNotif.index << std::endl;
  os << " type : " << suppSvcNotif.type << std::endl;
  std::string number{ "<empty>" };
  if (suppSvcNotif.number) {
    number = suppSvcNotif.number;
  }
  os << " number : " << number << std::endl;
  os << " connId : " << suppSvcNotif.connId << std::endl;
  std::string historyInfo{ "<empty>" };
  if (suppSvcNotif.historyInfo) {
    historyInfo = suppSvcNotif.historyInfo;
  }
  os << " historyInfo : " << historyInfo << std::endl;
  os << " hasHoldTone : " << static_cast<int>(suppSvcNotif.hasHoldTone) << std::endl;
  os << " holdTone : " << static_cast<int>(suppSvcNotif.holdTone) << std::endl;
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_CallComposerLocation& location) {
  os << " radius: " << location.radius;
  os << " latitude: " << location.latitude;
  os << " longitude: " << location.longitude;
  return os;
}

std::ostream& operator<<(std::ostream& out, const RIL_IMS_CallComposerPriority priority) {
  out << "IMS call composer priority: ";
  switch (priority) {
    case RIL_IMS_CALL_COMPOSER_PRIORITY_URGENT:
      out << "RIL_IMS_CALL_COMPOSER_PRIORITY_URGENT";
      break;
    case RIL_IMS_CALL_COMPOSER_PRIORITY_NORMAL:
      out << "RIL_IMS_CALL_COMPOSER_PRIORITY_NORMAL";
      break;
    case RIL_IMS_CALL_COMPOSER_PRIORITY_INVALID:
    default:
      out << "Unknown.";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const RIL_IMS_VerstatVerificationStatus verStatus) {
  out << "IMS call verstat verification status: ";
  switch (verStatus) {
    case RIL_IMS_VERSTAT_VERIFICATION_STATUS_PASS:
      out << "RIL_IMS_VERSTAT_VERIFICATION_STATUS_PASS";
      break;
    case RIL_IMS_VERSTAT_VERIFICATION_STATUS_FAIL:
      out << "RIL_IMS_VERSTAT_VERIFICATION_STATUS_FAIL";
      break;
    case RIL_IMS_VERSTAT_VERIFICATION_STATUS_NONE:
    default:
      out << "Unknown.";
      break;
  }
  return out;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_CallComposerInfo& ccInfo) {
  os << " priority : " << ccInfo.priority;
  if (ccInfo.subject) {
    os << " subject: " << ccInfo.subject;
  }
  if (ccInfo.organization) {
    os << " organization: " << ccInfo.organization;
  }
  os << " location: " << ccInfo.location;
  if (ccInfo.imageUrl) {
    os << " Image url: " << ccInfo.imageUrl;
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_EcnamInfo& ecnamInfo) {
  if (ecnamInfo.name) {
    os << " name: " << ecnamInfo.name;
  }
  if (ecnamInfo.iconUrl) {
    os << " iconUrl: " << ecnamInfo.iconUrl;
  }
  if (ecnamInfo.infoUrl) {
    os << " infoUrl: " << ecnamInfo.infoUrl;
  }
  if (ecnamInfo.cardUrl) {
    os << " cardUrl: " << ecnamInfo.cardUrl;
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_PreAlertingCallInfo& preAlertingCallInfo) {
  os << " CallId: " << preAlertingCallInfo.callId;
  if (preAlertingCallInfo.ccInfo) {
    os << " Callcomposer Info: " << *(preAlertingCallInfo.ccInfo);
  }
  if (preAlertingCallInfo.ecnamInfo) {
    os << " Ecnam Info: " << *(preAlertingCallInfo.ecnamInfo);
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_SrtpEncryptionStatus& srtpInfo) {
  os << " CallId: " << srtpInfo.callId << std::endl;
  os << " SRTP encryption  categories: " << srtpInfo.categories << std::endl;
  if (srtpInfo.categories == RIL_IMS_SRTP_CATEGORIES_UNENCRYPTED) {
    os << " RIL_IMS_SRTP_CATEGORIES_UNENCRYPTED " << std::endl;
  }else {
    if (srtpInfo.categories & RIL_IMS_SRTP_CATEGORIES_VOICE) {
      os << " RIL_IMS_SRTP_CATEGORIES_VOICE " << std::endl;
    }
    if (srtpInfo.categories & RIL_IMS_SRTP_CATEGORIES_VIDEO) {
      os << " RIL_IMS_SRTP_CATEGORIES_VIDEO " << std::endl;
    }
    if (srtpInfo.categories & RIL_IMS_SRTP_CATEGORIES_TEXT) {
      os << " RIL_IMS_SRTP_CATEGORIES_TEXT " << std::endl;
    }
  }
  return os;
}

void ImsMenu::registerForImsIndications(std::vector<std::string> userInput) {
  rilSession.registerImsUnsolCallStateChangedIndicationHandler(
      [](const RIL_IMS_CallInfo* calls, size_t sz) {
        std::cout << "Received ImsUnsolCallStateChanged indication." << std::endl;
        if (calls) {
          for (int i = 0; i < sz; i++) {
            std::cout << &calls[i];
          }
        }
      });

  rilSession.registerImsUnsolCallRingIndicationHandler(
      []() { std::cout << "Received ImsCallUnsolRing." << std::endl; });

  rilSession.registerImsUnsolRingBackToneIndicationHandler(
      [](const RIL_IMS_ToneOperation& imsToneOperation) {
        std::cout << "Received ImsUnsolRingBackTone." << std::endl;
        std::cout << imsToneOperation << std::endl;
      });

  rilSession.registerImsUnsolRegistrationStateIndicationHandler(
      [](const RIL_IMS_Registration& imsRegInfo) {
        std::cout << "Received ImsUnsolRegistrationState." << std::endl;
        std::cout << imsRegInfo << std::endl;
      });

  rilSession.registerImsUnsolServiceStatusIndicationHandler(
      [](const size_t count, const RIL_IMS_ServiceStatusInfo** imsSrvStatusInfo) {
        std::cout << "Received ImsUnsolServiceStatus." << std::endl;
        std::cout << "Received count :" << count << std::endl;
        for (size_t i = 0; i < count; i++) {
          std::cout << imsSrvStatusInfo[i] << std::endl;
        }
      });

  rilSession.registerImsUnsolSubConfigChangeIndicationHandler(
      [](const RIL_IMS_SubConfigInfo& imsSubConfigInfo) {
        std::cout << "Received ImsUnsolSubConfigChange." << std::endl;
        std::cout << imsSubConfigInfo << std::endl;
      });

  rilSession.registerImsRttMessageIndicationHandler([](const std::string& rttMessage) {
    std::cout << "Received rttMessage indication." << std::endl;
    std::cout << " UnsolImsRttMessage = " + rttMessage << std::endl;
  });

  rilSession.registerImsGeolocationIndicationHandler([](double latitude, double longitude) {
    std::cout << "Received geolocation info indication." << std::endl;
    std::cout << " latitude = " + std::to_string(latitude) << std::endl;
    std::cout << " latitude = " + std::to_string(longitude) << std::endl;
  });

  rilSession.registerImsUnsolRetrievingGeoLocationDataStatus(
      [](RIL_IMS_GeoLocationDataStatus status) {
        std::cout << "Received RetrievingGeoLocationDataStatus." << std::endl;
        std::cout << " status = " + std::to_string(status) << std::endl;
      });

  rilSession.registerImsUnsolHandoverIndicationHandler([](const RIL_IMS_HandoverInfo& handoverInfo) {
    std::cout << "Received ImsUnsolHandoverIndication" << std::endl;
    std::cout << handoverInfo << std::endl;
  });

  rilSession.registerImsUnsolRefreshConferenceInfo([](const RIL_IMS_RefreshConferenceInfo& confInfo) {
    std::cout << "Received ImsUnsolRefreshConferenceInfo" << std::endl;
    std::cout << confInfo << std::endl;
  });

  rilSession.registerImsUnsolMultiIdentityPendingInfo(
      []() { std::cout << "Received ImsUnsolMultiIdentityPendingInfo" << std::endl; });

  rilSession.registerImsUnsolMultiIdentityRegistrationStatus(
      [](const RIL_IMS_MultiIdentityLineInfo* info, size_t len) {
        std::cout << "Received ImsUnsolMultiIdentityRegistrationStatus" << std::endl;
        if (info) {
          for (int i = 0; i < len; i++) {
            std::cout << info[i] << std::endl;
          }
        }
      });

  rilSession.registerImsVowifiQualityIndicationHandler(
      [](const RIL_IMS_VowifiCallQuality& vowifiCallQuality) {
        std::cout << "Received vowifi call quality indication." << std::endl;
        std::cout << " vowifi call quality = " + std::to_string(vowifiCallQuality) << std::endl;
      });

  rilSession.registerImsUnsolEnterEcbmIndicationHandler(
      []() { std::cout << "Received ImsUnsolEnterEcbmIndication" << std::endl; });

  rilSession.registerImsUnsolExitEcbmIndicationHandler(
      []() { std::cout << "Received ImsUnsolExitEcbmIndication" << std::endl; });

  rilSession.registerImsUnsolVopsChangedIndicationHandler([](const bool& vopsStatus) {
    std::cout << "Received ImsUnsolVopsChangedIndication." << std::endl;
    std::cout << vopsStatus << std::endl;
  });

  rilSession.registerImsIncomingSmsHandler([](const RIL_IMS_IncomingSms& imsSms) {
    std::cout << "Received RIL_IMS_UNSOL_INCOMING_IMS_SMS" << std::endl;
    std::cout << std::endl << imsSms << std::endl;
  });

  rilSession.registerImsSmsStatusReportHandler([](const RIL_IMS_SmsStatusReport& imsSmsStatusReport) {
    std::cout << "Received RIL_IMS_UNSOL_SMS_STATUS_REPORT" << std::endl;
    std::cout << std::endl << imsSmsStatusReport << std::endl;
  });

  rilSession.registerImsViceInfoHandler([](const std::vector<uint8_t>& viceInfo) {
    std::cout << "Received RIL_IMS_UNSOL_REFRESH_VICE_INFO" << std::endl;
    std::cout << "== IMS VICE Info Notification ==" << std::endl
              << std::string(viceInfo.begin(), viceInfo.end()) << std::endl;
  });

  rilSession.registerImsTtyNotificationHandler([](RIL_IMS_TtyModeType ttyMode) {
    std::cout << "Received RIL_IMS_UNSOL_TTY_NOTIFICATION" << std::endl;
    std::cout << "== IMS TTY Mode Notification ==" << std::endl << ttyMode << std::endl;
  });

  rilSession.registerImsUnsolAutoCallRejectionIndicationHandler(
      [](const RIL_IMS_AutoCallRejectionInfo& rejInfo) {
        std::cout << "Received RIL_IMS_UNSOL_INCOMING_CALL_AUTO_REJECTED" << std::endl;
        std::cout << " call type: " << rejInfo.callType << " failure cause: " << rejInfo.cause
                  << " sip error code: " << rejInfo.sipErrorCode;
        if (rejInfo.number) {
          std::cout << " number: " << rejInfo.number;
        }
        std::cout << " verstat verification status: " << rejInfo.verificationStatus;
        if (rejInfo.ccInfo) {
          std::cout << " Call composer info: " << *(rejInfo.ccInfo);
        }
        if (rejInfo.ecnamInfo) {
          std::cout << " Ecnam info: " << *(rejInfo.ecnamInfo);
        }
        std::cout << std::endl;
      });

  rilSession.registerImsUnsolModifyCallIndicationHandler(
      [](const RIL_IMS_CallModifyInfo& modifyInfo) {
        std::cout << "Received RIL_IMS_UNSOL_MODIFY_CALL" << std::endl;
        std::cout << " call id: " << modifyInfo.callId << modifyInfo.callType
                  << modifyInfo.callDomain;
        if (modifyInfo.hasRttMode) {
          std::cout << modifyInfo.rttMode;
        }
        if (modifyInfo.hasCallModifyFailCause) {
          std::cout << modifyInfo.callModifyFailCause;
        }
        std::cout << std::endl;
      });

  rilSession.registerImsUnsolMessageWaitingIndicationHandler(
      [](const RIL_IMS_MessageWaitingInfo& waitInfo) {
        std::cout << "Received RIL_IMS_UNSOL_MESSAGE_WAITING" << std::endl;
        for (int i = 0; i < waitInfo.messageSummaryLen; i++) {
          std::cout << "idx " << i << " of " << waitInfo.messageSummary[i];
        }
        if (waitInfo.ueAddress) {
          std::cout << " UE address: " << waitInfo.ueAddress;
        }
        for (int i = 0; i < waitInfo.messageDetailsLen; i++) {
          std::cout << "idx " << i << " of " << waitInfo.messageDetails[i];
        }
        std::cout << std::endl;
      });

  rilSession.registerImsUnsolParticipantStatusInfoIndicationHandler(
      [](const RIL_IMS_ParticipantStatusInfo& statusInfo) {
        std::cout << "Received RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO" << std::endl;
        std::cout << " call id: " << statusInfo.callId << statusInfo.operation
                  << " sip status: " << statusInfo.sipStatus;
        if (statusInfo.participantUri) {
            std::cout << " participant URI: " << statusInfo.participantUri;
          }
        if (statusInfo.hasIsEct) std::cout << " isEct: " << statusInfo.isEct;
        std::cout << std::endl;
      });

  rilSession.registerImsUnsolRegistrationBlockStatusIndicationHandler(
      [](const RIL_IMS_RegistrationBlockStatus& blockStatus) {
        std::cout << "Received RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS" << std::endl;
        std::cout << " ";
        if (blockStatus.hasBlockStatusOnWwan) {
          std::cout << "WWAN " << blockStatus.blockStatusOnWwan;
        }
        if (blockStatus.hasBlockStatusOnWlan) {
          std::cout << "WLAN " << blockStatus.blockStatusOnWlan;
        }
        std::cout << std::endl;
      });

  rilSession.registerImsMultiSimVoiceCapabilityChanged(
      [](const RIL_IMS_MultiSimVoiceCapability& voiceCapability) {
        std::cout << "Received RIL_IMS_UNSOL_MULTI_SIM_VOICE_CAPABILITY_CHANGED" << std::endl;
        std::cout << voiceCapability << std::endl;
      });

  rilSession.registerImsUnsolSuppSvcNotificationIndicationHandler(
      [](const RIL_IMS_SuppSvcNotification& suppSvcNotif) {
        std::cout << "Received RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION" << std::endl;
        std::cout << "suppSvcNotification = " << suppSvcNotif << std::endl;
      });

  rilSession.registerImsUnsolOnSsIndicationHandler(
      [](const RIL_IMS_StkCcUnsolSsResponse& stkCcUnsolSs) {
        std::cout << "Received RIL_IMS_UNSOL_ON_SS" << std::endl;
        std::cout << "stkCcUnsolSsResponse = " << stkCcUnsolSs << std::endl;
      });

  rilSession.registerImsUnsolVoiceInfoIndicationHandler([](const RIL_IMS_VoiceInfoType& voiceInfo) {
    std::cout << "Received RIL_IMS_UNSOL_VOICE_INFO" << std::endl;
    std::cout << "voiceInfo = " << voiceInfo << std::endl;
  });
    rilSession.registerImsOnUssdIndicationHandler(
      [](const char& mode, const std::string& message) {
        std::cout << "Received OnUssdIndication." << std::endl;
        std::cout << static_cast<RIL_UssdModeType>(mode);
        std::cout << "message: " << message << std::endl;
      });

  rilSession.registerImsPreAlertingCallInfoAvailable(
      [](const RIL_IMS_PreAlertingCallInfo& preAlertingCallInfo) {
        std::cout << "Received RIL_IMS_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE" << std::endl;
        std::cout << preAlertingCallInfo << std::endl;
      });

  rilSession.registerUnsolSrtpEncryptionStatusIndicationHandler(
      [](const RIL_IMS_SrtpEncryptionStatus& srtpInfo) {
        std::cout << "Received RIL_IMS_UNSOL_SRTP_ENCRYPTION_STATUS" << std::endl;
        std::cout << srtpInfo << std::endl;
      });
}

void ImsMenu::imsGetClir(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsGetClir([](RIL_Errno err, const RIL_IMS_ClirInfo & clirInfo) -> void {
    std::cout << "Got response for GetClir request." << err << std::endl;
    std::cout << "action: " << clirInfo.action << std::endl;
    std::cout << "presentation: " << clirInfo.presentation << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsGetClir" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_SetClirInfo &setClir)
{
    in >> setClir.action;
    return in;
}

void ImsMenu::imsSetClir(std::vector<std::string> userInput) {
  RIL_IMS_SetClirInfo info{};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetClir(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetClir request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetClir" << std::endl;
  }
}

ostream& operator<<(ostream& os, const RIL_IMS_EctType arg) {
    os << "IMS Ect Type : ";
    switch(arg) {
        case RIL_IMS_ECT_TYPE_UNKNOWN:
            os << "RIL_IMS_ECT_TYPE_UNKNOWN";
            break;
        case RIL_IMS_ECT_TYPE_BLIND_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_BLIND_TRANSFER";
            break;
        case RIL_IMS_ECT_TYPE_ASSURED_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_ASSURED_TRANSFER";
            break;
        case RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER";
            break;
    }
    return os;
}

istream& operator >> (istream &in, RIL_IMS_EctType &arg)
{
    int var;
    cout << "0. RIL_IMS_ECT_TYPE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_ECT_TYPE_BLIND_TRANSFER" << std::endl;
    cout << "2. RIL_IMS_ECT_TYPE_ASSURED_TRANSFER" << std::endl;
    cout << "3. RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER" << std::endl;
    do {
        cout << "Enter RIL_IMS_EctType: " << std::endl;
        in >> var;
    } while (var < 0 || var > 3);
    arg = static_cast<RIL_IMS_EctType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ExplicitCallTransfer &explicitCall)
{
  std::cout << "Enter callId : " << std::endl;
  in >> explicitCall.callId;

  cout << " Enter ECT type : " << std::endl;
  cout << "0. RIL_IMS_ECT_TYPE_UNKNOWN" << std::endl;
  cout << "1. RIL_IMS_ECT_TYPE_BLIND_TRANSFER" << std::endl;
  cout << "2. RIL_IMS_ECT_TYPE_ASSURED_TRANSFER" << std::endl;
  cout << "3. RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER" << std::endl;

  int var;
  do {
    cout << "Enter RIL_IMS_EctType : " << std::endl;
    std::cin >> var;
  } while (var < 0 || var > 3);
  explicitCall.ectType = static_cast<RIL_IMS_EctType>(var);

  std::string tempString;
  std::cout << "Enter targetAddress, provide valid target address or empty (\"\") : " << std::endl;
  in >> tempString;
  explicitCall.targetAddress = nullptr;
  if (!tempString.empty() && tempString != "\"\"") {
    explicitCall.targetAddress = new char[tempString.size()+1]();
    tempString.copy(explicitCall.targetAddress, tempString.size());
  }

  std::cout << "Do you want to enter targetCallId, if yes --> 1 and no --> 0" << std::endl;
  int isTargetCallIDRequired;
  std::cin >> isTargetCallIDRequired;

  if (isTargetCallIDRequired) {
    std::cout << "Enter targetCallId : " << std::endl;
    std::cin >> explicitCall.targetCallId;
  } else {
    explicitCall.targetCallId = std::numeric_limits<uint32_t>::max();
  }
  return in;
}

void ImsMenu::imsExplicitCallTransfer(std::vector<std::string> userInput) {
  RIL_IMS_ExplicitCallTransfer info {};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsExplicitCallTransfer(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for ExplicitCallTransfer request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsExplicitCallTransfer" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_TtyModeType &arg)
{
    int var;
    cout << "0. RIL_IMS_TTY_MODE_OFF" << std::endl;
    cout << "1. RIL_IMS_TTY_MODE_FULL" << std::endl;
    cout << "2. RIL_IMS_TTY_MODE_HCO" << std::endl;
    cout << "3. RIL_IMS_TTY_MODE_VCO" << std::endl;
    cout << "4. RIL_IMS_TTY_MODE_INVALID" << std::endl;
    do {
        cout << "Enter RIL_IMS_TtyModeType: " << std::endl;
        in >> var;
    } while (var < 0 || var > 4);
    arg = static_cast<RIL_IMS_TtyModeType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ServiceClassStatus &arg)
{
    int var;
    cout << "0. RIL_IMS_SERVICE_CLASS_STATUS_DISABLED" << std::endl;
    cout << "1. RIL_IMS_SERVICE_CLASS_STATUS_ENABLED" << std::endl;
    do {
        cout << "Enter RIL_IMS_ServiceClassStatus : ";
        in >> var;
    } while (var < 0 || var > 1);
    arg = static_cast<RIL_IMS_ServiceClassStatus>(var);
    return in;
}


istream& operator >> (istream &in, RIL_IMS_TtyNotifyInfo &ttyInfo)
{
    in >> ttyInfo.mode;
    in >> ttyInfo.userData;
    return in;
}

void ImsMenu::imsSendUiTtyMode(std::vector<std::string> userInput) {
  RIL_IMS_TtyNotifyInfo ttyInfo {};
  cin >> ttyInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSendUiTtyMode(ttyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for SendUiTtyMode request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendUiTtyMode" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_DeflectCallInfo &deflectCall)
{
    in >> deflectCall.connIndex;
    std::string num;
    in >> num;
    auto len = num.length();
    if (len > 0) {
        deflectCall.number = new char[len + 1]();
        if (deflectCall.number) {
            strlcpy(deflectCall.number, num.c_str(), len + 1);
        } else {
            std::cerr << "failed to allocate number string" << std::endl;
        }
    }
    return in;
}

void ImsMenu::imsDeflectCall(std::vector<std::string> userInput) {
  RIL_IMS_DeflectCallInfo info {};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsDeflectCall(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for DeflectCall request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsDeflectCall" << std::endl;
  }
}

void ImsMenu::imsQueryClip(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsQueryClip([](RIL_Errno err, const RIL_IMS_ClipInfo & clipInfo) -> void {
    std::cout << "Got response for QueryClip request." << err << std::endl;
    std::cout << "clipStatus: " << clipInfo.clipStatus << std::endl;
    std::cout << "errorDetails: " << clipInfo.errorDetails << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsQueryClip" << std::endl;
  }
}

void ImsMenu::imsSetSuppSvcNotification(std::vector<std::string> userInput) {
  RIL_IMS_ServiceClassStatus info;
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetSuppSvcNotification(info, [](RIL_Errno err, const RIL_IMS_ServiceClassStatus & srvStatus) -> void {
    std::cout << "Got response for SetSuppSvcNotification request." << err << std::endl;
    std::cout << "srvStatus: " << srvStatus << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetSuppSvcNotification" << std::endl;
  }
}

std::istream& operator >> (std::istream &in, RIL_IMS_ServiceClassProvisionStatus &status) {
  std::cout << "provisionStatus: (0 - Not provisioned 1 - Provisioned): " << std::endl;
  uint32_t input;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  status = static_cast<RIL_IMS_ServiceClassProvisionStatus>(input);

  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_SipErrorInfo &info) {
  std::cout << "Input Error Details: " << std::endl;
  uint32_t input;
  in >> input;
  info.errorCode = input;

  std::cout << "any error strings (1 - Yes, 0 - No): " << std::endl;;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  if (input) {
    std::cout << "Input the error string: " << std::endl;
    std::string errorString;
    in >> errorString;
    auto len = errorString.length();
    if (len > 0) {
        info.errorString = new char[len + 1]();
        if (info.errorString) {
            strlcpy(info.errorString, errorString.c_str(), len + 1);
        } else {
            std::cerr << "failed to allocate the error string" << std::endl;
        }
    }
  }
  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_ColrInfo &info) {
  std::cout << "Input Colr Info - " << std::endl;
  in >> info.status;
  in >> info.provisionStatus;
  in >> info.presentation;

  std::cout << "any error details (1 - Yes, 0 - No): " << std::endl;
  uint32_t input;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  if (input) {
    info.errorDetails = new RIL_IMS_SipErrorInfo;
    if (info.errorDetails) {
        in >> *info.errorDetails;
    } else {
        std::cerr << "failed to allocate error details" << std::endl;
    }
  }
  return in;
}

void ImsMenu::imsSetColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IMS_ColrInfo colr{};
  std::cin >> colr;

  Status s = rilSession.imsSetColr(colr,
        [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errInfo) {
            std::cout << "Got response for imsSetColr: " << err << std::endl
                      << &errInfo << std::endl;

        });
  std::cout << ((s == Status::SUCCESS) ? "request imsSetColr sent succesfully"
                                       : "Failed to send imsSetColr")
            << std::endl;
  if (colr.errorDetails) {
    if (colr.errorDetails->errorString)
        delete []colr.errorDetails->errorString;
    delete colr.errorDetails;
  }
}

std::ostream& operator << (ostream& os, const RIL_IMS_ServiceClassStatus status) {
  os << "IMS Service Class status: ";
  switch (status) {
  case RIL_IMS_SERVICE_CLASS_STATUS_DISABLED:
    os << "IMS_SERVICE_CLASS_STATUS_DISABLED";
    break;
  case RIL_IMS_SERVICE_CLASS_STATUS_ENABLED:
    os << "IMS_SERVICE_CLASS_STATUS_ENABLED";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_ServiceClassProvisionStatus status) {
  os << "IMS Service Class Provision status: ";
  switch (status) {
  case RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED:
    os << "IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED";
    break;
  case RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED:
    os << "IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_IpPresentation presentation) {
  os << "IMS Ip Presentation: ";
  switch (presentation) {
  case RIL_IMS_IP_PRESENTATION_NUM_ALLOWED:
    os << "IMS_IP_PRESENTATION_NUM_ALLOWED";
    break;
  case RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED:
    os << "IMS_IP_PRESENTATION_NUM_RESTRICTED";
    break;
  case RIL_IMS_IP_PRESENTATION_NUM_DEFAULT:
    os << "IMS_IP_PRESENTATION_NUM_DEFAULT";
    break;
  case RIL_IMS_IP_PRESENTATION_INVALID:
    os << "IMS_IP_PRESENTATION_INVALID";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_ColrInfo& colr) {
  os << colr.status << std::endl;
  os << colr.provisionStatus << std::endl;
  os << colr.presentation << std::endl;
  if (colr.errorDetails)
    os << colr.errorDetails << std::endl;
  return os;
}

void ImsMenu::imsGetColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.imsGetColr(
        [](RIL_Errno err, const RIL_IMS_ColrInfo& colr) {
            std::cout << "Got response for imsSetColr: " << err << std::endl
                      << colr << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "request imsGetColr sent succesfully"
                                       : "Failed to send imsGetColr")
            << std::endl;
}

std::istream& operator >> (std::istream &in,
        RIL_IMS_MultiIdentityRegistrationStatus &status) {
  uint32_t input;
  std::cout << "0. RIL_IMS_STATUS_UNKNOWN" << std::endl;
  std::cout << "1. RIL_IMS_STATUS_DISABLE" << std::endl;
  std::cout << "2. RIL_IMS_STATUS_ENABLE" << std::endl;
  do {
    std::cout << "Enter MultiIdentityRegStatus: ";
    in >> input;
  } while (input > 2);
  status = static_cast<RIL_IMS_MultiIdentityRegistrationStatus>(input);

  return in;
}

std::istream& operator >> (std::istream &in,
        RIL_IMS_MultiIdentityLineType &lineType) {
  uint32_t input;
  std::cout << "0. RIL_IMS_LINE_TYPE_UNKNOWN" << std::endl;
  std::cout << "1. RIL_IMS_LINE_TYPE_PRIMARY" << std::endl;
  std::cout << "2. RIL_IMS_LINE_TYPE_SECONDARY" << std::endl;
  do {
    std::cout << "Enter MultiIdentityLineType: ";
    in >> input;
  } while (input > 2);
  lineType = static_cast<RIL_IMS_MultiIdentityLineType>(input);

  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_MultiIdentityLineInfo& info) {
  std::string input;
  do {
    std::cout << "Enter msisdn: ";
    in >> input;
  } while (input.empty());
  auto size = input.length();
  info.msisdn = new char[size + 1]();
  if (info.msisdn != nullptr) {
    strlcpy(info.msisdn, input.c_str(), size + 1);
  }
  in >> info.registrationStatus;
  in >> info.lineType;

  return in;
}

void ImsMenu::imsRegisterMultiIdentityLines(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "Please input the number of line info: ";
  uint32_t input;
  do {
    std::cin >> input;
  } while (input == 0);
  size_t size = input;

  Status s = Status::FAILURE;
  auto lineInfo = new RIL_IMS_MultiIdentityLineInfo[size]();
  if (lineInfo != nullptr) {
    for (int i = 0; i < size; i++) {
        std::cout << "line info " << i << ": " << std::endl;
        std::cin >> lineInfo[i];
    }
    s = rilSession.imsRegisterMultiIdentityLines(lineInfo, size,
            [](RIL_Errno err) {
                std::cout << "Got response for imsRegisterMultiIdentityLines: "
                          << err << std::endl;
            });
    for (int i = 0; i < size; i++) {
        if (lineInfo[i].msisdn) delete []lineInfo[i].msisdn;
    }
    delete []lineInfo;
  }

  std::cout << ((s == Status::SUCCESS)
                ? "request imsRegisterMultiIdentityLines sent succesfully"
                : "Failed to send imsRegisterMultiIdentityLines")
            << std::endl;
}

void ImsMenu::imsQueryVirtualLineInfo(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::string input;
  do {
    std::cout << "Enter msisdn: ";
    std::cin >> input;
  } while (input.empty());

  Status s = rilSession.imsQueryVirtualLineInfo(input.c_str(),
        [](RIL_Errno err, const RIL_IMS_QueryVirtualLineInfoResponse& info) {
            std::cout << "Got response for imsQueryVirtualLineInfo: "
                      << err << std::endl
                      << "msisdn: " << (info.msisdn ? info.msisdn : "null") << std::endl
                      << "Number of lines: " << info.numLines << std::endl;
            if (info.virtualLines != nullptr) {
                for (int i = 0; i < info.numLines; i++) {
                    std::cout << "Line " << i << ": " << (info.virtualLines[i] ?
                            info.virtualLines[i] : "null") << std::endl;
                }
            }
        });
  std::cout << ((s == Status::SUCCESS)
                ? "request imsQueryVirtualLineInfo sent succesfully"
                : "Failed to send imsQueryVirtualLineInfo")
            << std::endl;
}

void ImsMenu::imsSetCallForwardStatus(std::vector<std::string> userInput) {
  RIL_IMS_CallForwardInfo callInfo = {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cin.clear();
  cin >> callInfo;

  Status s = rilSession.imsSetCallForwardStatus(callInfo,
      [](RIL_Errno err, const RIL_IMS_SetCallForwardStatusInfo& info) -> void {
    std::cout << "Got response for Ims set Call Forward Status request: " << err << std::endl;
    std::cout << "numSetCallFwdStatus: " << info.setCallForwardStatusLen << std::endl;
    if (info.setCallForwardStatus && (info.setCallForwardStatusLen > 0)) {
      for (int i = 0; i < info.setCallForwardStatusLen; i++) {
          std::cout << "RIL_IMS_SetCallForwardStatus :" << std::endl;
          std::cout << "reason: " << info.setCallForwardStatus[i].reason << std::endl;
          std::cout << "status: " << info.setCallForwardStatus[i].status << std::endl;
          std::cout << "SetCallForwardStatus errorDetails: ";
          std::cout << info.setCallForwardStatus[i].errorDetails << std::endl;
      }
    }
    std::cout << "errorDetails: " << info.errorDetails;
  });

  if(s != Status::SUCCESS) {
    std::cout << "Failed to set Ims Call Forward status" << std::endl;
  }

}

void ImsMenu::imsQueryCallForwardStatus(std::vector<std::string> userInput) {
  RIL_IMS_CallForwardInfo callInfo = {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "Input Call Forward Info - " << std::endl;

  std::cin.clear();
  std::cout << "0. unconditional" << std::endl;
  std::cout << "1. mobile busy" << std::endl;
  std::cout << "2. no reply" << std::endl;
  std::cout << "3. not reachable" << std::endl;
  std::cout << "4. all call forwarding" << std::endl;
  std::cout << "5. all conditional call forwarding" << std::endl;
  std::cout << "Enter reason(integer type):" << std::endl;
  std::cin >> callInfo.reason;

  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services):" << std::endl;
  std::cin >> callInfo.serviceClass;

  Status s = rilSession.imsQueryCallForwardStatus(callInfo,
      [](RIL_Errno err, const RIL_IMS_QueryCallForwardStatusInfo& info) -> void {
    std::cout << "Got response for Ims query Call Forward Status request: " << err << std::endl;
    std::cout << "numCallFwdInfos: " << info.callForwardInfoLen << std::endl;
    if (info.callForwardInfo && (info.callForwardInfoLen > 0)) {
      for (int i = 0; i < info.callForwardInfoLen; i++) {
          std::cout << info.callForwardInfo[i];
      }
    }
    std::cout << "errorDetails: " << info.errorDetails;
  });

  if(s != Status::SUCCESS) {
    std::cout << "Failed to query Ims Call Forward status" << std::endl;
  }
}

istream& operator>>(istream& in, RIL_IMS_SuppSvcOperationType& arg) {
  int var;
  cout << "0. RIL_IMS_SUPP_OPERATION_ACTIVATE" << std::endl;
  cout << "1. RIL_IMS_SUPP_OPERATION_DEACTIVATE" << std::endl;
  cout << "2. RIL_IMS_SUPP_OPERATION_QUERY" << std::endl;
  cout << "3. RIL_IMS_SUPP_OPERATION_REGISTER" << std::endl;
  cout << "4. RIL_IMS_SUPP_OPERATION_ERASURE" << std::endl;
  do {
    cout << "Enter RIL_IMS_SuppSvcOperationType: " << std::endl;
    in >> var;
  } while (var < 0 || var > 4);
  arg = static_cast<RIL_IMS_SuppSvcOperationType>(var);
  return in;
}

istream& operator>>(istream& in, RIL_IMS_FacilityType& arg) {
  int var;
  cout << "0. RIL_IMS_FACILITY_CLIP" << std::endl;
  cout << "1. RIL_IMS_FACILITY_COLP" << std::endl;
  cout << "2. RIL_IMS_FACILITY_BAOC" << std::endl;
  cout << "3. RIL_IMS_FACILITY_BAOIC" << std::endl;
  cout << "4. RIL_IMS_FACILITY_BAOICxH" << std::endl;
  cout << "5. RIL_IMS_FACILITY_BAIC" << std::endl;
  cout << "6. RIL_IMS_FACILITY_BAICr" << std::endl;
  cout << "7. RIL_IMS_FACILITY_BA_ALL" << std::endl;
  cout << "8. RIL_IMS_FACILITY_BA_MO" << std::endl;
  cout << "9. RIL_IMS_FACILITY_BA_MT" << std::endl;
  cout << "10. RIL_IMS_FACILITY_BS_MT" << std::endl;
  cout << "11. RIL_IMS_FACILITY_BAICa" << std::endl;
  do {
    cout << "Enter RIL_IMS_FacilityType: " << std::endl;
    in >> var;
  } while (var < 0 || var > 11);
  arg = static_cast<RIL_IMS_FacilityType>(var);
  return in;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_FacilityType data) {
  os << "RIL_IMS_FacilityType : ";
  switch (data) {
    case RIL_IMS_FACILITY_CLIP:
      os << "RIL_IMS_FACILITY_CLIP";
      break;
    case RIL_IMS_FACILITY_COLP:
      os << "RIL_IMS_FACILITY_COLP";
      break;
    case RIL_IMS_FACILITY_BAOC:
      os << "RIL_IMS_FACILITY_BAOC";
      break;
    case RIL_IMS_FACILITY_BAOIC:
      os << "RIL_IMS_FACILITY_BAOIC";
      break;
    case RIL_IMS_FACILITY_BAOICxH:
      os << "RIL_IMS_FACILITY_BAOICxH";
      break;
    case RIL_IMS_FACILITY_BAIC:
      os << "RIL_IMS_FACILITY_BAIC";
      break;
    case RIL_IMS_FACILITY_BAICr:
      os << "RIL_IMS_FACILITY_BAICr";
      break;
    case RIL_IMS_FACILITY_BA_ALL:
      os << "RIL_IMS_FACILITY_BA_ALL";
      break;
    case RIL_IMS_FACILITY_BA_MO:
      os << "RIL_IMS_FACILITY_BA_MO";
      break;
    case RIL_IMS_FACILITY_BA_MT:
      os << "RIL_IMS_FACILITY_BA_MT";
      break;
    case RIL_IMS_FACILITY_BS_MT:
      os << "RIL_IMS_FACILITY_BS_MT";
      break;
    case RIL_IMS_FACILITY_BAICa:
      os << "RIL_IMS_FACILITY_BAICa";
      break;
  }
  return os;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_CallBarringNumbersInfo& cbNumInfo) {
  os << "RIL_IMS_CallBarringNumbersInfo : ";
  os << "  status : " << cbNumInfo.status << std::endl;
  if (cbNumInfo.number) {
    os << "  number : " << cbNumInfo.number << std::endl;
  }
  return os;
}

std::istream& operator>>(std::istream& in, RIL_IMS_CallBarringNumbersInfo& cbNumInfo) {
  std::cout << "Input RIL_IMS_CallBarringNumbersInfo:" << std::endl;
  in >> cbNumInfo.status;
  std::cout << "Enter number : ";
  std::string number;
  in >> number;
  cbNumInfo.number = nullptr;
  if (!number.empty()) {
    cbNumInfo.number = new char[number.length() + 1];
    if (cbNumInfo.number) {
      strlcpy(cbNumInfo.number, number.c_str(), number.length() + 1);
    }
  }
  return in;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_CallBarringNumbersListInfo& cbNumInfo) {
  os << "RIL_IMS_CallBarringNumbersListInfo : ";
  os << "  serviceClass : " << cbNumInfo.serviceClass << std::endl;
  os << "  callBarringNumbersInfoLen : "
     << static_cast<uint32_t>(cbNumInfo.callBarringNumbersInfoLen) << std::endl;
  if (cbNumInfo.callBarringNumbersInfoLen) {
    for (size_t i = 0; i < cbNumInfo.callBarringNumbersInfoLen; i++) {
      os << "  callBarringNumbersInfo[" << static_cast<int>(i) << "] :" << std::endl;
      os << cbNumInfo.callBarringNumbersInfo[i] << std::endl;
    }
  }
  return os;
}

std::istream& operator>>(std::istream& in, RIL_IMS_CallBarringNumbersListInfo& cbNumInfo) {
  std::cout << "Input RIL_IMS_CallBarringNumbersListInfo:" << std::endl;
  std::cout << "Enter serviceClass : ";
  in >> cbNumInfo.serviceClass;
  std::cout << "Enter callBarringNumbersInfoLen : ";
  in >> cbNumInfo.callBarringNumbersInfoLen;
  if (cbNumInfo.callBarringNumbersInfoLen) {
    cbNumInfo.callBarringNumbersInfo =
        new RIL_IMS_CallBarringNumbersInfo[cbNumInfo.callBarringNumbersInfoLen];
    for (size_t i = 0; i < cbNumInfo.callBarringNumbersInfoLen; i++) {
      std::cout << "Enter callBarringNumbersInfo[" << static_cast<int>(i) << "] : " << std::endl;
      in >> cbNumInfo.callBarringNumbersInfo[i];
    }
  }
  return in;
}

std::istream& operator>>(std::istream& in, RIL_IMS_SuppSvcRequest& suppSvcReq) {
  std::cout << "Input RIL_IMS_SuppSvcRequest:" << std::endl;
  in >> suppSvcReq.operationType;
  in >> suppSvcReq.facilityType;
  suppSvcReq.callBarringNumbersListInfo = new RIL_IMS_CallBarringNumbersListInfo;
  if (suppSvcReq.callBarringNumbersListInfo) {
    in >> *suppSvcReq.callBarringNumbersListInfo;
  }
  std::cout << "Enter password : ";
  std::string password;
  in >> password;
  suppSvcReq.password = nullptr;
  if (!password.empty()) {
    suppSvcReq.password = new char[password.length() + 1];
    if (suppSvcReq.password) {
      strlcpy(suppSvcReq.password, password.c_str(), password.length() + 1);
    }
  }
  return in;
}

std::ostream& operator<<(ostream& os, const RIL_IMS_SuppSvcResponse& suppSvcResp) {
  os << "RIL_IMS_SuppSvcResponse valid for QUERY : " << std::endl;
  os << suppSvcResp.status << std::endl;
  os << suppSvcResp.provisionStatus << std::endl;
  os << suppSvcResp.facilityType << std::endl;
  os << "callBarringNumbersListInfoLen : "
     << static_cast<uint32_t>(suppSvcResp.callBarringNumbersListInfoLen) << std::endl;
  if (suppSvcResp.callBarringNumbersListInfoLen) {
    for (size_t i = 0; i < suppSvcResp.callBarringNumbersListInfoLen; i++) {
      os << "  callBarringNumbersListInfo[" << static_cast<int>(i) << "] : " << std::endl;
      os << suppSvcResp.callBarringNumbersListInfo[i] << std::endl;
    }
  }
  if (suppSvcResp.errorDetails) {
    os << *suppSvcResp.errorDetails << std::endl;
  }
  os << "isPasswordRequired : " << std::to_string(suppSvcResp.isPasswordRequired) << std::endl;

  return os;
}

void ImsMenu::imsSuppSvcStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IMS_SuppSvcRequest suppSvcReq{};
  std::cin >> suppSvcReq;

  Status s = rilSession.imsSuppSvcStatus(
      suppSvcReq, [](RIL_Errno err, const RIL_IMS_SuppSvcResponse& resp) {
        std::cout << "Got response for imsSuppSvcStatus: err=" << err << std::endl;
        std::cout << resp << std::endl;
      });
  std::cout << ((s == Status::SUCCESS) ? "request imsSuppSvcStatus sent succesfully"
                                       : "Failed to send imsSuppSvcStatus")
            << std::endl;
}
