/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * RILCallMenu class provides call functionalitites
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "RILCallMenu.hpp"
#include "Phone/PhoneMenu.hpp"
#include "Ims/ImsMenu.hpp"

using namespace std;

RILCallMenu::RILCallMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession)
{
}

RILCallMenu::~RILCallMenu()
{
}

void RILCallMenu::init()
{
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListImsSubMenu = {
    CREATE_COMMAND(RILCallMenu::registerForIndications, "registerForIndications"),
    CREATE_COMMAND(RILCallMenu::dial, "dial", "number"),
    CREATE_COMMAND(RILCallMenu::acceptCall, "acceptCall"),
    CREATE_COMMAND(RILCallMenu::hangup, "hangup"),
    CREATE_COMMAND(RILCallMenu::getCurrentCalls, "getCurrentCalls"),
    CREATE_COMMAND(RILCallMenu::hold, "hold"),
    CREATE_COMMAND(RILCallMenu::resume, "resume"),
    CREATE_COMMAND(RILCallMenu::hangupWaitingOrBackground, "hangupWaitingOrBackground"),
    CREATE_COMMAND(RILCallMenu::hangupForegroundResumeBackground, "hangupForegroundResumeBackground"),
    CREATE_COMMAND(RILCallMenu::udub, "udub"),
    CREATE_COMMAND(RILCallMenu::lastCallFailCause, "lastCallFailCause"),
    CREATE_COMMAND(RILCallMenu::switchWaitingOrHoldingAndActive, "switchWaitingOrHoldingAndActive"),
    CREATE_COMMAND(RILCallMenu::deflectCall, "deflectCall"),
    CREATE_COMMAND(RILCallMenu::explicitCallTransfer, "explicitCallTransfer"),
    CREATE_COMMAND(RILCallMenu::conference, "conference"),
    CREATE_COMMAND(RILCallMenu::separateConnection, "SeparateConnection", "call_index"),
    CREATE_COMMAND(RILCallMenu::addParticipant, "addParticipant"),
    CREATE_COMMAND(RILCallMenu::modifyCallInitiate, "modifyCallInitiate"),
    CREATE_COMMAND(RILCallMenu::modifyCallConfirm, "modifyCallConfirm"),
    CREATE_COMMAND(RILCallMenu::cancelModifyCall, "cancelModifyCall"),
    CREATE_COMMAND(RILCallMenu::sendRttMessage, "sendRttMessage"),
    CREATE_COMMAND(RILCallMenu::exitEmergencyCallbackMode, "exitEmergencyCallbackMode"),
    CREATE_COMMAND(RILCallMenu::playDtmfTone, "playDtmfTone", "number * #"),
    CREATE_COMMAND(RILCallMenu::startDtmfTone, "startDtmfTone", "number * #"),
    CREATE_COMMAND(RILCallMenu::stopDtmfTone, "stopDtmfTone"),
    CREATE_COMMAND(RILCallMenu::sendUssd, "sendUssd"),
    CREATE_COMMAND(RILCallMenu::cancelUssd, "cancelUssd"),
    CREATE_COMMAND(RILCallMenu::setCallForward, "setCallForward"),
    CREATE_COMMAND(RILCallMenu::queryCallForwardStatus, "queryCallForwardStatus"),
    CREATE_COMMAND(RILCallMenu::setCallWaiting, "setCallWaiting"),
    CREATE_COMMAND(RILCallMenu::queryCallWaiting, "queryCallWaiting"),
    CREATE_COMMAND(RILCallMenu::changeBarringPassword, "changeBarringPassword"),
    CREATE_COMMAND(RILCallMenu::queryClip, "queryClip"),
    CREATE_COMMAND(RILCallMenu::getClir, "getClir"),
    CREATE_COMMAND(RILCallMenu::setClir, "setClir"),
    CREATE_COMMAND(RILCallMenu::getColr, "getColr"),
    CREATE_COMMAND(RILCallMenu::setColr, "setColr"),
    CREATE_COMMAND(RILCallMenu::suppSvcStatus, "suppSvcStatus"),
    CREATE_COMMAND(RILCallMenu::setTtyMode, "setTtyMode"),
    CREATE_COMMAND(RILCallMenu::getTtyMode, "getTtyMode"),
    CREATE_COMMAND(RILCallMenu::sendUiTtyMode, "sendUiTtyMode"),
    CREATE_COMMAND(RILCallMenu::setSuppSvcNotification, "setSuppSvcNotification"),
  };

  addCommands(commandsListImsSubMenu);
  ConsoleApp::displayMenu();
}

void readFromUserDeflectCallInfo(RIL_DeflectCallInfo& o)
{
  std::cout << "Enter connIndex : ";
  std::cin >> o.connIndex;
  std::string num;
  std::cout << "Enter number : ";
  std::cin >> num;
  auto len = num.length();
  if (len > 0) {
    o.number = new char[len + 1]();
    if (o.number) {
      strlcpy(o.number, num.c_str(), len + 1);
    } else {
      std::cerr << "failed to allocate number string" << std::endl;
    }
  }
}

std::string toString(RIL_ConfParticipantOperation o)
{
  if (o == RIL_CONF_PARTICIPANT_OP_UNKNOWN) {
    return "RIL_CONF_PARTICIPANT_OP_UNKNOWN";
  }
  if (o == RIL_CONF_PARTICIPANT_OP_ADD) {
    return "RIL_CONF_PARTICIPANT_OP_ADD";
  }
  if (o == RIL_CONF_PARTICIPANT_OP_REMOVE) {
    return "RIL_CONF_PARTICIPANT_OP_REMOVE";
  }
  return "<INVALID>";
}

std::string toString(RIL_VerstatVerificationStatus o)
{
  if (o == RIL_VERSTAT_VERIFICATION_STATUS_NONE) {
    return "RIL_VERSTAT_VERIFICATION_STATUS_NONE";
  }
  if (o == RIL_VERSTAT_VERIFICATION_STATUS_PASS) {
    return "RIL_VERSTAT_VERIFICATION_STATUS_PASS";
  }
  if (o == RIL_VERSTAT_VERIFICATION_STATUS_FAIL) {
    return "RIL_VERSTAT_VERIFICATION_STATUS_FAIL";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallComposerPriority o)
{
  if (o == RIL_CALL_COMPOSER_PRIORITY_INVALID) {
    return "RIL_CALL_COMPOSER_PRIORITY_INVALID";
  }
  if (o == RIL_CALL_COMPOSER_PRIORITY_URGENT) {
    return "RIL_CALL_COMPOSER_PRIORITY_URGENT";
  }
  if (o == RIL_CALL_COMPOSER_PRIORITY_NORMAL) {
    return "RIL_CALL_COMPOSER_PRIORITY_NORMAL";
  }
  return "<INVALID>";
}

std::string toString(RIL_UssdModeType o)
{
  if (o == RIL_USSD_NOTIFY) {
    return "RIL_USSD_NOTIFY";
  }
  if (o == RIL_USSD_REQUEST) {
    return "RIL_USSD_REQUEST";
  }
  if (o == RIL_USSD_NW_RELEASE) {
    return "RIL_USSD_NW_RELEASE";
  }
  if (o == RIL_USSD_LOCAL_CLIENT) {
    return "RIL_USSD_LOCAL_CLIENT";
  }
  if (o == RIL_USSD_NOT_SUPPORTED) {
    return "RIL_USSD_NOT_SUPPORTED";
  }
  if (o == RIL_USSD_NW_TIMEOUT) {
    return "RIL_USSD_NW_TIMEOUT";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallState o)
{
  if (o == RIL_CALL_ACTIVE) {
    return "RIL_CALL_ACTIVE";
  }
  if (o == RIL_CALL_HOLDING) {
    return "RIL_CALL_HOLDING";
  }
  if (o == RIL_CALL_DIALING) {
    return "RIL_CALL_DIALING";
  }
  if (o == RIL_CALL_ALERTING) {
    return "RIL_CALL_ALERTING";
  }
  if (o == RIL_CALL_INCOMING) {
    return "RIL_CALL_INCOMING";
  }
  if (o == RIL_CALL_WAITING) {
    return "RIL_CALL_WAITING";
  }
  if (o == RIL_CALL_END) {
    return "RIL_CALL_END";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallType o)
{
  if (o == RIL_CALL_TYPE_UNKNOWN) {
    return "RIL_CALL_TYPE_UNKNOWN";
  }
  if (o == RIL_CALL_TYPE_VOICE) {
    return "RIL_CALL_TYPE_VOICE";
  }
  if (o == RIL_CALL_TYPE_VT_TX) {
    return "RIL_CALL_TYPE_VT_TX";
  }
  if (o == RIL_CALL_TYPE_VT_RX) {
    return "RIL_CALL_TYPE_VT_RX";
  }
  if (o == RIL_CALL_TYPE_VT) {
    return "RIL_CALL_TYPE_VT";
  }
  if (o == RIL_CALL_TYPE_VT_NODIR) {
    return "RIL_CALL_TYPE_VT_NODIR";
  }
  if (o == RIL_CALL_TYPE_SMS) {
    return "RIL_CALL_TYPE_SMS";
  }
  if (o == RIL_CALL_TYPE_UT) {
    return "RIL_CALL_TYPE_UT";
  }
  if (o == RIL_CALL_TYPE_USSD) {
    return "RIL_CALL_TYPE_USSD";
  }
  if (o == RIL_CALL_TYPE_CALLCOMPOSER) {
    return "RIL_CALL_TYPE_CALLCOMPOSER";
  }
  if (o == RIL_CALL_TYPE_DC) {
    return "RIL_CALL_TYPE_DC";
  }
  return "<INVALID>";
}

std::string toString(RIL_IMS_CallType o)
{
  if (o == RIL_IMS_CALL_TYPE_UNKNOWN) {
    return "RIL_IMS_CALL_TYPE_UNKNOWN";
  }
  if (o == RIL_IMS_CALL_TYPE_VOICE) {
    return "RIL_IMS_CALL_TYPE_VOICE";
  }
  if (o == RIL_IMS_CALL_TYPE_VT_TX) {
    return "RIL_IMS_CALL_TYPE_VT_TX";
  }
  if (o == RIL_IMS_CALL_TYPE_VT_RX) {
    return "RIL_IMS_CALL_TYPE_VT_RX";
  }
  if (o == RIL_IMS_CALL_TYPE_VT) {
    return "RIL_IMS_CALL_TYPE_VT";
  }
  if (o == RIL_IMS_CALL_TYPE_VT_NODIR) {
    return "RIL_IMS_CALL_TYPE_VT_NODIR";
  }
  if (o == RIL_IMS_CALL_TYPE_SMS) {
    return "RIL_IMS_CALL_TYPE_SMS";
  }
  if (o == RIL_IMS_CALL_TYPE_UT) {
    return "RIL_IMS_CALL_TYPE_UT";
  }
  if (o == RIL_IMS_CALL_TYPE_USSD) {
    return "RIL_IMS_CALL_TYPE_USSD";
  }
  if (o == RIL_IMS_CALL_TYPE_CALLCOMPOSER) {
    return "RIL_IMS_CALL_TYPE_CALLCOMPOSER";
  }
  if (o == RIL_IMS_CALL_TYPE_DC) {
    return "RIL_IMS_CALL_TYPE_DC";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallDomain o)
{
  if (o == RIL_CALLDOMAIN_UNKNOWN) {
    return "RIL_CALLDOMAIN_UNKNOWN";
  }
  if (o == RIL_CALLDOMAIN_CS) {
    return "RIL_CALLDOMAIN_CS";
  }
  if (o == RIL_CALLDOMAIN_PS) {
    return "RIL_CALLDOMAIN_PS";
  }
  if (o == RIL_CALLDOMAIN_AUTOMATIC) {
    return "RIL_CALLDOMAIN_AUTOMATIC";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallModifiedCause o)
{
  if (o == RIL_CMODCAUSE_NONE) {
    return "RIL_CMODCAUSE_NONE";
  }
  if (o == RIL_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ) {
    return "RIL_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ";
  }
  if (o == RIL_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ) {
    return "RIL_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_QOS) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_QOS";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC";
  }
  if (o == RIL_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR) {
    return "RIL_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR";
  }
  return "<INVALID>";
}

std::string toString(RIL_IMS_RttMode o)
{
  if (o == RIL_IMS_RTT_DISABLED) {
    return "RIL_IMS_RTT_DISABLED";
  }
  if (o == RIL_IMS_RTT_FULL) {
    return "RIL_IMS_RTT_FULL";
  }
  return "<INVALID>";
}

std::string toString(RIL_VerificationStatus o)
{
  if (o == RIL_VERSTAT_UNKNOWN) {
    return "RIL_VERSTAT_UNKNOWN";
  }
  if (o == RIL_VERSTAT_NONE) {
    return "RIL_VERSTAT_NONE";
  }
  if (o == RIL_VERSTAT_PASS) {
    return "RIL_VERSTAT_PASS";
  }
  if (o == RIL_VERSTAT_FAIL) {
    return "RIL_VERSTAT_FAIL";
  }
  return "<INVALID>";
}

std::string toString(RIL_TirMode o)
{
  if (o == RIL_TIRMODE_UNKNOWN) {
    return "RIL_TIRMODE_UNKNOWN";
  }
  if (o == RIL_TIRMODE_TEMPORARY) {
    return "RIL_TIRMODE_TEMPORARY";
  }
  if (o == RIL_TIRMODE_PERMANENT) {
    return "RIL_TIRMODE_PERMANENT";
  }
  return "<INVALID>";
}

std::string toString(RIL_CallProgressInfoType o)
{
  if (o == RIL_CALL_PROGRESS_INFO_UNKNOWN) {
    return "RIL_CALL_PROGRESS_INFO_UNKNOWN";
  }
  if (o == RIL_CALL_PROGRESS_INFO_CALL_REJ_Q850) {
    return "RIL_CALL_PROGRESS_INFO_CALL_REJ_Q850";
  }
  if (o == RIL_CALL_PROGRESS_INFO_CALL_WAITING) {
    return "RIL_CALL_PROGRESS_INFO_CALL_WAITING";
  }
  if (o == RIL_CALL_PROGRESS_INFO_CALL_FORWARDING) {
    return "RIL_CALL_PROGRESS_INFO_CALL_FORWARDING";
  }
  if (o == RIL_CALL_PROGRESS_INFO_REMOTE_AVAILABLE) {
    return "RIL_CALL_PROGRESS_INFO_REMOTE_AVAILABLE";
  }
  return "<INVALID>";
}

std::string toString(RIL_MsimAdditionalInfoCode o)
{
  if (o == RIL_MSIM_ADDITIONAL_INFO_NONE) {
    return "RIL_MSIM_ADDITIONAL_INFO_NONE";
  }
  if (o == RIL_MSIM_ADDITIONAL_INFO_CONCURRENT_CALL_NOT_POSSIBLE) {
    return "RIL_MSIM_ADDITIONAL_INFO_CONCURRENT_CALL_NOT_POSSIBLE";
  }
  return "<INVALID>";
}

std::string toString(RIL_Codec o)
{
  if (o == RIL_CODEC_NONE) {
    return "RIL_CODEC_NONE";
  }
  if (o == RIL_CODEC_QCELP13K) {
    return "RIL_CODEC_QCELP13K";
  }
  if (o == RIL_CODEC_EVRC) {
    return "RIL_CODEC_EVRC";
  }
  if (o == RIL_CODEC_EVRC_B) {
    return "RIL_CODEC_EVRC_B";
  }
  if (o == RIL_CODEC_EVRC_WB) {
    return "RIL_CODEC_EVRC_WB";
  }
  if (o == RIL_CODEC_EVRC_NW) {
    return "RIL_CODEC_EVRC_NW";
  }
  if (o == RIL_CODEC_AMR_NB) {
    return "RIL_CODEC_AMR_NB";
  }
  if (o == RIL_CODEC_AMR_WB) {
    return "RIL_CODEC_AMR_WB";
  }
  if (o == RIL_CODEC_GSM_EFR) {
    return "RIL_CODEC_GSM_EFR";
  }
  if (o == RIL_CODEC_GSM_FR) {
    return "RIL_CODEC_GSM_FR";
  }
  if (o == RIL_CODEC_GSM_HR) {
    return "RIL_CODEC_GSM_HR";
  }
  if (o == RIL_CODEC_G711U) {
    return "RIL_CODEC_G711U";
  }
  if (o == RIL_CODEC_G723) {
    return "RIL_CODEC_G723";
  }
  if (o == RIL_CODEC_G711A) {
    return "RIL_CODEC_G711A";
  }
  if (o == RIL_CODEC_G722) {
    return "RIL_CODEC_G722";
  }
  if (o == RIL_CODEC_G711AB) {
    return "RIL_CODEC_G711AB";
  }
  if (o == RIL_CODEC_G729) {
    return "RIL_CODEC_G729";
  }
  if (o == RIL_CODEC_EVS_NB) {
    return "RIL_CODEC_EVS_NB";
  }
  if (o == RIL_CODEC_EVS_WB) {
    return "RIL_CODEC_EVS_WB";
  }
  if (o == RIL_CODEC_EVS_SWB) {
    return "RIL_CODEC_EVS_SWB";
  }
  if (o == RIL_CODEC_EVS_FB) {
    return "RIL_CODEC_EVS_FB";
  }
  return "<INVALID>";
}

std::string toString(RIL_ComputedAudioQuality o)
{
  if (o == RIL_COMPUTED_AUDIO_QUALITY_NONE) {
    return "RIL_COMPUTED_AUDIO_QUALITY_NONE";
  }
  if (o == RIL_COMPUTED_AUDIO_QUALITY_NO_HD) {
    return "RIL_COMPUTED_AUDIO_QUALITY_NO_HD";
  }
  if (o == RIL_COMPUTED_AUDIO_QUALITY_HD) {
    return "RIL_COMPUTED_AUDIO_QUALITY_HD";
  }
  if (o == RIL_COMPUTED_AUDIO_QUALITY_HD_PLUS) {
    return "RIL_COMPUTED_AUDIO_QUALITY_HD_PLUS";
  }
  return "<INVALID>";
}

std::string toString(RIL_RadioTechnology o)
{
  if (o == RADIO_TECH_UNKNOWN) {
    return "RADIO_TECH_UNKNOWN";
  }
  if (o == RADIO_TECH_GPRS) {
    return "RADIO_TECH_GPRS";
  }
  if (o == RADIO_TECH_EDGE) {
    return "RADIO_TECH_EDGE";
  }
  if (o == RADIO_TECH_UMTS) {
    return "RADIO_TECH_UMTS";
  }
  if (o == RADIO_TECH_IS95A) {
    return "RADIO_TECH_IS95A";
  }
  if (o == RADIO_TECH_IS95B) {
    return "RADIO_TECH_IS95B";
  }
  if (o == RADIO_TECH_1xRTT) {
    return "RADIO_TECH_1xRTT";
  }
  if (o == RADIO_TECH_EVDO_0) {
    return "RADIO_TECH_EVDO_0";
  }
  if (o == RADIO_TECH_EVDO_A) {
    return "RADIO_TECH_EVDO_A";
  }
  if (o == RADIO_TECH_HSDPA) {
    return "RADIO_TECH_HSDPA";
  }
  if (o == RADIO_TECH_HSUPA) {
    return "RADIO_TECH_HSUPA";
  }
  if (o == RADIO_TECH_HSPA) {
    return "RADIO_TECH_HSPA";
  }
  if (o == RADIO_TECH_EVDO_B) {
    return "RADIO_TECH_EVDO_B";
  }
  if (o == RADIO_TECH_EHRPD) {
    return "RADIO_TECH_EHRPD";
  }
  if (o == RADIO_TECH_LTE) {
    return "RADIO_TECH_LTE";
  }
  if (o == RADIO_TECH_HSPAP) {
    return "RADIO_TECH_HSPAP";
  }
  if (o == RADIO_TECH_GSM) {
    return "RADIO_TECH_GSM";
  }
  if (o == RADIO_TECH_TD_SCDMA) {
    return "RADIO_TECH_TD_SCDMA";
  }
  if (o == RADIO_TECH_IWLAN) {
    return "RADIO_TECH_IWLAN";
  }
  if (o == RADIO_TECH_LTE_CA) {
    return "RADIO_TECH_LTE_CA";
  }
  if (o == RADIO_TECH_5G) {
    return "RADIO_TECH_5G";
  }
  if (o == RADIO_TECH_WIFI) {
    return "RADIO_TECH_WIFI";
  }
  if (o == RADIO_TECH_ANY) {
    return "RADIO_TECH_ANY";
  }
  if (o == RADIO_TECH_AUTO) {
    return "RADIO_TECH_AUTO";
  }
  if (o == RADIO_TECH_C_IWLAN) {
    return "RADIO_TECH_C_IWLAN";
  }
  return "<INVALID>";
}

std::string toString(RIL_IMS_StatusType o)
{
  if (o == RIL_IMS_STATUS_DISABLED) {
    return "RIL_IMS_STATUS_DISABLED";
  }
  if (o == RIL_IMS_STATUS_PARTIALLY_ENABLED) {
    return "RIL_IMS_STATUS_PARTIALLY_ENABLED";
  }
  if (o == RIL_IMS_STATUS_ENABLED) {
    return "RIL_IMS_STATUS_ENABLED";
  }
  if (o == RIL_IMS_STATUS_NOT_SUPPORTED) {
    return "RIL_IMS_STATUS_NOT_SUPPORTED";
  }
  return "<INVALID>";
}

std::string toString(RIL_IMS_RegistrationState o)
{
  if (o == RIL_IMS_REG_STATE_UNKNOWN) {
    return "RIL_IMS_REG_STATE_UNKNOWN";
  }
  if (o == RIL_IMS_REG_STATE_REGISTERED) {
    return "RIL_IMS_REG_STATE_REGISTERED";
  }
  if (o == RIL_IMS_REG_STATE_NOT_REGISTERED) {
    return "RIL_IMS_REG_STATE_NOT_REGISTERED";
  }
  if (o == RIL_IMS_REG_STATE_REGISTERING) {
    return "RIL_IMS_REG_STATE_REGISTERING";
  }
  return "<INVALID>";
}

std::string toString(const char *o, std::string padding = "") {
  std::ostringstream os;
  os << padding << (o ? o : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_CallFailCauseResponse *o, std::string padding = "")
{
  if (!o){
    return "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_CallFailCauseResponse  ==" << std::endl;
  os << padding << " failCause: " << o->failCause << std::endl;
  os << padding << " extendedFailCause: " << o->extendedFailCause << std::endl;
  os << padding << " networkErrorString: " << (o->networkErrorString ? o->networkErrorString : "<null>") << std::endl;
  if (o->errorDetails) {
    os << padding << " errorDetails: " << *o->errorDetails << std::endl;
  }
  return os.str();
}

std::string toString(const RIL_VerstatInfo *o, std::string padding = "")
{
  if (!o){
    return padding + "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_VerstatInfo  ==" << std::endl;
  os << padding << " canMarkUnwantedCall: " << static_cast<int>(o->canMarkUnwantedCall) << std::endl;
  os << padding << " verificationStatus: " << toString(o->verificationStatus) << std::endl;
  return os.str();
}

std::string toString(const RIL_UUS_Info *o, std::string padding = "")
{
  if (!o){
    return padding + "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_UUS_Info  ==" << std::endl;
  os << padding << " uusType: " << static_cast<int>(o->uusType) << std::endl;
  os << padding << " uusDcs: " << static_cast<int>(o->uusDcs) << std::endl;
  os << padding << " uusLength: " << static_cast<int>(o->uusLength) << std::endl;
  os << padding << " uusData: " << (o->uusLength && o->uusData ? o->uusData : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_CallProgressInfo *o, std::string padding = "")
{
  if (!o){
    return padding + "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_CallProgressInfo  ==" << std::endl;
  os << padding << " type: " << toString(o->type) << std::endl;
  os << padding << " reasonCode: " << static_cast<int>(o->reasonCode) << std::endl;
  os << padding << " reasonText: " << (o->reasonText ? o->reasonText : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_MsimAdditionalCallInfo *o, std::string padding = "")
{
  if (!o){
    return padding + "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_MsimAdditionalCallInfo  ==" << std::endl;
  os << padding << " additionalCode: " << toString(o->additionalCode) << std::endl;
  return os.str();
}

std::string toString(const RIL_CallAudioQuality *o, std::string padding = "")
{
  if (!o){
    return padding + "<null>\n";
  }
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_CallAudioQuality  ==" << std::endl;
  os << padding << " codec: " << toString(o->codec) << std::endl;
  os << padding << " computedAudioQuality: " << toString(o->computedAudioQuality) << std::endl;
  return os.str();
}

std::string toString(const RIL_IMS_Registration& o, std::string padding = "")
{
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_IMS_Registration  ==" << std::endl;
  os << padding << " state: " << toString(o.state) << std::endl;
  os << padding << " errorCode: " << o.errorCode << std::endl;
  os << padding << " errorMessage: " << toString(o.errorMessage);
  os << padding << " radioTech: " << toString(o.radioTech) << std::endl;
  os << padding << " pAssociatedUris: " << toString(o.pAssociatedUris);
  return os.str();
}

std::string toString(const RIL_IMS_AccessTechnologyStatus& o, std::string padding = "")
{
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_IMS_AccessTechnologyStatus  ==" << std::endl;
  os << padding << " networkMode: " << o.networkMode << std::endl;
  os << padding << " status: " << toString(o.status) << std::endl;
  os << padding << " restrictCause: " << o.restrictCause << std::endl;
  os << padding << " registration: " << toString(o.registration, padding+" ");
  return os.str();
}

std::string toString(const RIL_IMS_ServiceStatusInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << std::endl;
  os << padding << "== RIL_IMS_ServiceStatusInfo  ==" << std::endl;
  os << padding << " callType: " << toString(o.callType) << std::endl;
  os << padding << " accTechStatus: " << toString(o.accTechStatus, padding+" ");
  os << padding << " rttMode: " << toString(o.rttMode) << std::endl;
  return os.str();
}

std::string toString(const RIL_IMS_SipErrorInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_IMS_SipErrorInfo  ==" << std::endl;
  os << padding << " errorCode: " << o.errorCode << std::endl;
  os << padding << " errorString: " << (o.errorString ? o.errorString : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_ParticipantStatusInfo& partiStatusInfo)
{
  std::ostringstream os;
  os << "== RIL_ParticipantStatusInfo  ==" << std::endl;
  os << " callId: " << partiStatusInfo.callId << std::endl;
  os << " operation: " << toString(partiStatusInfo.operation) << std::endl;
  os << " sipStatus: " << partiStatusInfo.sipStatus << std::endl;
  os << " participantUri: "
     << (partiStatusInfo.participantUri ? partiStatusInfo.participantUri : "null") << std::endl;
  os << " hasIsEct: " << static_cast<uint32_t>(partiStatusInfo.hasIsEct) << std::endl;
  os << " isEct: " << static_cast<uint32_t>(partiStatusInfo.isEct) << std::endl;
  return os.str();
}

std::string toString(const RIL_CallComposerLocation& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallComposerLocation  ==" << std::endl;
  os << padding << " radius: " << o.radius << std::endl;
  os << padding << " latitude: " << o.latitude << std::endl;
  os << padding << " longitude: " << o.longitude << std::endl;
  return os.str();
}

std::string toString(const RIL_CallComposerInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallComposerInfo  ==" << std::endl;
  os << padding << " priority: " << toString(o.priority) << std::endl;
  os << padding << " subject: " << (o.subject ? o.subject : "null") << std::endl;
  os << padding << " location:\n" << toString(o.location, padding + " ") << std::endl;
  os << padding << " imageUrl: " << (o.imageUrl ? o.imageUrl : "null") << std::endl;
  os << padding << " organization: " << (o.organization ? o.organization : "null") << std::endl;
  return os.str();
}

std::string toString(const RIL_EcnamInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_EcnamInfo  ==" << std::endl;
  os << padding << " name: " << (o.name ? o.name : "null") << std::endl;
  os << padding << " iconUrl: " << (o.iconUrl ? o.iconUrl : "null") << std::endl;
  os << padding << " infoUrl: " << (o.infoUrl ? o.infoUrl : "null") << std::endl;
  os << padding << " cardUrl: " << (o.cardUrl ? o.cardUrl : "null") << std::endl;
  return os.str();
}

std::string toString(const RIL_AutoCallRejectionInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_AutoCallRejectionInfo  ==" << std::endl;
  os << padding << " callType: " << o.callType << std::endl;
  os << padding << " cause: " << o.cause << std::endl;
  os << padding << " sipErrorCode: " << o.sipErrorCode << std::endl;
  os << padding << " number: " << (o.number ? o.number : "null") << std::endl;
  os << padding << " verificationStatus: " << toString(o.verificationStatus) << std::endl;
  os << padding << " ccInfo: \n"
     << (o.ccInfo ? toString(*o.ccInfo, padding + " ") : "null") << std::endl;
  os << padding << " ecnamInfo: \n"
     << (o.ecnamInfo ? toString(*o.ecnamInfo, padding + " ") : "null") << std::endl;
  return os.str();
}

std::string toString(const RIL_PreAlertingCallInfo& o, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_PreAlertingCallInfo  ==" << std::endl;
  os << padding << " callId: " << o.callId << std::endl;
  os << padding << " ccInfo: \n"
     << (o.ccInfo ? toString(*o.ccInfo, padding + " ") : "null") << std::endl;
  os << padding << " ecnamInfo: \n"
     << (o.ecnamInfo ? toString(*o.ecnamInfo, padding + " ") : "null") << std::endl;
  return os.str();
}

void readCallWaitingSettingsFromUser(RIL_CallWaitingSettings& cwSettings)
{
  int32_t userInput = -1;
  do {
    std::cin.clear();
    std::cout << "1. voice (telephony)" << std::endl;
    std::cout << "2. data" << std::endl;
    std::cout << "4. fax" << std::endl;
    std::cout << "8. short message service" << std::endl;
    std::cout << "16. data circuit sync" << std::endl;
    std::cout << "32. data circuit async" << std::endl;
    std::cout << "64. dedicated packet access" << std::endl;
    std::cout << "128. dedicated PAD access" << std::endl;
    std::cout << "Enter serviceClass(sum of integers representing services): ";
    std::cin >> userInput;
  } while (std::cin.fail() || userInput < -1);

  if (userInput == -1) {
    cwSettings.serviceClass = std::numeric_limits<uint32_t>::max();
  } else {
    cwSettings.serviceClass = userInput;
  }

  do {
    std::cin.clear();
    std::cout << "Do you want to enable or disable call waiting (0 - Disable, !0 - Enable)? : ";
    std::cin >> userInput;
    cwSettings.enabled = userInput;
  } while (std::cin.fail());
}

std::string toString(const RIL_CallFwdTimerInfo& val, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallFwdTimerInfo ==" << std::endl;
  os << padding << " year: " << val.year << std::endl;
  os << padding << " month: " << val.month << std::endl;
  os << padding << " day: " << val.day << std::endl;
  os << padding << " hour: " << val.hour << std::endl;
  os << padding << " minute: " << val.minute << std::endl;
  os << padding << " second: " << val.second << std::endl;
  os << padding << " timezone: " << val.timezone << std::endl;

  return os.str();
}
std::string toString(const RIL_CallForwardParams& val, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallForwardParams ==" << std::endl;
  os << padding << " status: " << val.status << std::endl;
  os << padding << " reason: " << val.reason << std::endl;
  os << padding << " serviceClass: " << val.serviceClass << std::endl;
  os << padding << " toa: " << val.toa << std::endl;
  os << padding << " number: " << (val.number ? val.number : "<null>") << std::endl;
  os << padding << " timeSeconds: " << val.timeSeconds << std::endl;
  os << padding << " callFwdTimerStart:\n"
     << (val.callFwdTimerStart ? toString(*val.callFwdTimerStart, padding + " ")
                               : (padding + " <null>"))
     << std::endl;
  os << padding << " callFwdTimerEnd:\n"
     << (val.callFwdTimerEnd ? toString(*val.callFwdTimerEnd, padding + " ") : (padding + " <null>"))
     << std::endl;
  return os.str();
}

std::string toString(const RIL_QueryCallForwardStatusInfo& data, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_QueryCallForwardStatusInfo ==" << std::endl;
  os << padding << " callForwardInfoLen: " << data.callForwardInfoLen << std::endl;
  for (size_t i = 0; i < data.callForwardInfoLen; i++) {
    os << padding << " callForwardInfo[" << i << "]:\n"
       << toString(data.callForwardInfo[i], " ") << std::endl;
  }
  os << padding << " errorDetails:\n"
     << (data.errorDetails ? toString(*data.errorDetails, padding + " ") : (padding + " <null>"))
     << std::endl;
  return os.str();
}

void readFromUserCallFwdTimerInfo(RIL_CallFwdTimerInfo& val)
{
  std::cout << "Input RIL_CallFwdTimerInfo" << std::endl;
  std::cout << "Enter year: ";
  std::cin >> val.year;
  std::cout << "Enter month: ";
  std::cin >> val.month;
  std::cout << "Enter day: ";
  std::cin >> val.day;
  std::cout << "Enter hour: ";
  std::cin >> val.hour;
  std::cout << "Enter minute: ";
  std::cin >> val.minute;
  std::cout << "Enter second: ";
  std::cin >> val.second;
  std::cout << "Enter timezone: ";
  std::cin >> val.timezone;
}

void readFromUserCallForwardPrefInfo(RIL_CallForwardParams& val)
{
  int32_t userInput = -1;
  std::cout << "Call Forward Pref Info" << std::endl;
  std::cout << "0. disable" << std::endl;
  std::cout << "1. enable" << std::endl;
  std::cout << "3. registeration" << std::endl;
  std::cout << "4. erasure" << std::endl;
  std::cout << "Enter status: ";
  std::cin >> userInput;
  val.status = userInput;

  std::cout << "0. unconditional" << std::endl;
  std::cout << "1. mobile busy" << std::endl;
  std::cout << "2. no reply" << std::endl;
  std::cout << "3. not reachable" << std::endl;
  std::cout << "4. all call forwarding" << std::endl;
  std::cout << "5. all conditional call forwarding" << std::endl;
  std::cout << "Enter reason (integer type): ";
  std::cin >> userInput;
  val.reason = userInput;

  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services): ";
  std::cin >> userInput;
  val.serviceClass = userInput;

  std::cout << "Enter type of number(145 or 129): ";
  std::cin >> userInput;
  val.toa = userInput;

  std::string num;
  std::cout << "Enter number: ";
  std::cin >> num;
  auto len = num.length();
  if (len > 0) {
    val.number = new char[len + 1]();
    if (val.number) {
      strlcpy(val.number, num.c_str(), len + 1);
    } else {
      std::cerr << "failed to allocate number string" << std::endl;
    }
  }

  std::cout << "Enter timer value in seconds: ";
  std::cin >> userInput;
  val.timeSeconds = userInput;

  std::cout << "Has call forward timer start? (1 - Yes, 0 - No): ";
  int32_t hasCallForwardTimerStart;

  do {
    std::cin >> hasCallForwardTimerStart;
  } while (hasCallForwardTimerStart != 0 && hasCallForwardTimerStart != 1);

  if (hasCallForwardTimerStart) {
    val.callFwdTimerStart = new RIL_CallFwdTimerInfo{};
    readFromUserCallFwdTimerInfo(*val.callFwdTimerStart);
  }

  std::cout << "Has call forward timer end? (1 - Yes, 0 - No): ";
  uint32_t hasCallForwardTimerEnd;

  do {
    std::cin >> hasCallForwardTimerEnd;
  } while (hasCallForwardTimerEnd != 0 && hasCallForwardTimerEnd != 1);

  if (hasCallForwardTimerEnd) {
    val.callFwdTimerEnd = new RIL_CallFwdTimerInfo{};
    readFromUserCallFwdTimerInfo(*val.callFwdTimerEnd);
  }
}

void readFromUserSwitchWaitingOrHoldingAndActive(RIL_SwitchWaitingOrHoldingAndActive& val)
{
  int32_t userInput = -1;
  do {
    std::cin.clear();
    std::cout << "Enter hasCallType (0 for no or 1 for yes): ";
    std::cin >> userInput;
  } while (std::cin.fail());

  if (userInput == 0) {
    val.hasCallType = 0;
  } else {
    val.hasCallType = 1;
    std::cin.clear();
    std::cin >> userInput;
    val.callType = static_cast<RIL_CallType>(userInput);
  }
}

void readFromUserHangup(RIL_Hangup& val)
{
  int32_t userInput = -1;
  do {
    std::cin.clear();
    std::cout << "Enter connIndex : ";
    std::cin >> userInput;
  } while (std::cin.fail());

  val.connIndex = userInput;

  std::cout << "Is connUri valid (0 for no or 1 for yes): ";
  std::cin >> userInput;
  if (userInput) {
    std::string tempString;
    std::cout << "Enter connUri : ";
    std::cin >> tempString;
    if (!tempString.empty()) {
      val.connUri = new char[tempString.size()+1]();
      tempString.copy(val.connUri, tempString.size());
    } else {
      val.connUri = nullptr;
    }
  }

  std::cin.clear();
  std::cout << "Enter hasRejectCause (0 for no or 1 for yes): ";
  std::cin >> userInput;

  if (userInput == 0) {
    val.hasRejectCause = 0;
  } else {
    val.hasRejectCause = 1;
    std::cin.clear();
    std::cout << "Enter rejectCause : ";
    std::cin >> userInput;
    val.rejectCause = static_cast<RIL_IMS_CallFailCause>(userInput);
  }
}

std::string toString(const RIL_CallWaitingSettings& cwSettings, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallWaitingSettings ==" << std::endl;
  os << padding << " Service Class: " << cwSettings.serviceClass << std::endl;
  os << padding << " State: " << (cwSettings.enabled ? "Enabled" : "Disabled");
  return os.str();
}

std::string toString(const RIL_QueryCallWaitingResponse& queryCwResp)
{
  std::ostringstream os;
  os << "== RIL_QueryCallWaitingResponse ==" << std::endl;
  os << " callWaitingSettings:\n" << toString(queryCwResp.callWaitingSettings, "    ") << std::endl;
  os << " sipError:" << std::endl;
  os << queryCwResp.sipError << std::endl;
  return os.str();
}

std::string toString(const RIL_SetCallForwardStatus& val, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_SetCallForwardStatus ==" << std::endl;
  os << padding << " reason: " << val.reason << std::endl;
  os << padding << " status: " << static_cast<uint32_t>(val.status) << std::endl;
  os << padding << " errorDetails:\n"
     << (val.errorDetails ? toString(*val.errorDetails, padding + " ") : (padding + " <null>"))
     << std::endl;
  return os.str();
}

std::string toString(const RIL_SetCallForwardStatusInfo& val, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_SetCallForwardStatusInfo ==" << std::endl;
  os << padding << " setCallForwardStatusLen: " << val.setCallForwardStatusLen << std::endl;
  if (val.setCallForwardStatusLen && val.setCallForwardStatus) {
    for (uint32_t i = 0; i < val.setCallForwardStatusLen; i++) {
      os << padding << " setCallForwardStatus:\n"
         << toString(val.setCallForwardStatus[i], padding + " ") << std::endl;
    }
  }
  os << padding << " errorDetails:\n"
     << (val.errorDetails ? toString(*val.errorDetails, padding + " ") : (padding + " <null>"))
     << std::endl;
  return os.str();
}

std::string toString(const RIL_LastCallFailCauseInfo& lcfInfo, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_LastCallFailCauseInfo ==" << std::endl;
  // os << padding << " cause_code: " << toString(lcfInfo.cause_code, " ") << std::endl;
  os << padding << " cause_code: " << lcfInfo.cause_code << std::endl;
  os << padding << " vendor_cause: " << (lcfInfo.vendor_cause ? lcfInfo.vendor_cause : "<null>")
     << std::endl;
  return os.str();
}

std::string toString(const RIL_CallInfo& callInfo, std::string padding = "")
{
  std::ostringstream os;
  os << padding << "== RIL_CallInfo ==" << std::endl;
  os << padding << " callState: " << toString(callInfo.callState) << std::endl;
  os << padding << " index: " << callInfo.index << std::endl;
  os << padding << " toa: " << callInfo.toa
                << ", isMpty: " << static_cast<int>(callInfo.isMpty)
                << ", isMt: " << static_cast<int>(callInfo.isMt)
                << ", als: " << static_cast<int>(callInfo.als)
                << ", isVoice: " << static_cast<int>(callInfo.isVoice)
                << ", isVoicePrivacy: " << static_cast<int>(callInfo.isVoicePrivacy) << std::endl;
  os << padding << " numberPresentation: " << callInfo.numberPresentation
                << ", number: " << (callInfo.number ? callInfo.number : "<null>") << std::endl;
  os << padding << " namePresentation: " << callInfo.namePresentation
                << ", name: " << (callInfo.name ? callInfo.name : "<null>") << std::endl;
  os << padding << " redirNumPresentation: " << callInfo.redirNumPresentation
                << ", redirNum: " << (callInfo.redirNum ? callInfo.redirNum : "<null>") << std::endl;
  os << padding << " callType: " << toString(callInfo.callType) << std::endl;
  os << padding << " callDomain: " << toString(callInfo.callDomain) << std::endl;
  os << padding << " callSubState: " << callInfo.callSubState << std::endl;
  os << padding << " isEncrypted: " << static_cast<int>(callInfo.isEncrypted) << std::endl;
  os << padding << " isCalledPartyRinging: " << static_cast<int>(callInfo.isCalledPartyRinging) << std::endl;
  os << padding << " isVideoConfSupported: " << static_cast<int>(callInfo.isVideoConfSupported) << std::endl;
  os << padding << " historyInfo: " << (callInfo.historyInfo ? callInfo.historyInfo : "<null>") << std::endl;
  os << padding << " mediaId: " << callInfo.mediaId << std::endl;
  os << padding << " causeCode: " << toString(callInfo.causeCode) << std::endl;
  os << padding << " rttMode: " << toString(callInfo.rttMode) << std::endl;
  os << padding << " sipAlternateUri: " << (callInfo.sipAlternateUri ? callInfo.sipAlternateUri : "<null>") << std::endl;
  os << padding << " localAbilityLen: " << callInfo.localAbilityLen << std::endl;
  if (callInfo.localAbilityLen && callInfo.localAbility) {
    for (int i = 0; i < callInfo.localAbilityLen; i++) {
      os << padding << " localAbility[" << i << "]:" << toString(callInfo.localAbility[i], padding+" ");
    }
  }
  os << padding << " peerAbilityLen: " << callInfo.peerAbilityLen << std::endl;
  if (callInfo.peerAbilityLen && callInfo.peerAbility) {
    for (int i = 0; i < callInfo.peerAbilityLen; i++) {
      os << padding << " peerAbility[" << i << "]:" << toString(callInfo.peerAbility[i], padding+" ");
    }
  }
  os << padding << " callFailCauseResponse: " << toString(callInfo.callFailCauseResponse, padding+" ");
  os << padding << " terminatingNumber: " << toString(callInfo.terminatingNumber);
  os << padding << " isSecondary: " << static_cast<int>(callInfo.isSecondary) << std::endl;
  os << padding << " verstatInfo: " << toString(callInfo.verstatInfo, padding+" ");
  os << padding << " uusInfo: " << toString(callInfo.uusInfo, padding+" ");
  os << padding << " displayText: " << toString(callInfo.displayText);
  os << padding << " additionalCallInfo: " << toString(callInfo.additionalCallInfo);
  os << padding << " childNumber: " << toString(callInfo.childNumber);
  os << padding << " emergencyServiceCategory: " << callInfo.emergencyServiceCategory << std::endl;
  os << padding << " tirMode: " << toString(callInfo.tirMode) << std::endl;
  os << padding << " callProgInfo: " << toString(callInfo.callProgInfo, padding+" ");
  os << padding << " msimAdditionalCallInfo: " << toString(callInfo.msimAdditionalCallInfo, padding+" ");
  os << padding << " audioQuality: " << toString(callInfo.audioQuality, padding+" ");
  os << padding << " modemCallId: " << callInfo.modemCallId << std::endl;

  return os.str();
}

void RILCallMenu::switchWaitingOrHoldingAndActive(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_SwitchWaitingOrHoldingAndActive reqData{};
  readFromUserSwitchWaitingOrHoldingAndActive(reqData);
  Status s = rilSession.switchWaitingOrHoldingAndActive(
      reqData,
      [](RIL_Errno e)
      {
        std::cout << std::endl
                  << "switchWaitingOrHoldingAndActive returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });

  if (s != Status::SUCCESS) {
    std::cout << std::endl
              << "switchWaitingOrHoldingAndActive: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::hangup(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_Hangup hangupReq{};
  readFromUserHangup(hangupReq);
  Status s = rilSession.hangup(hangupReq,
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "hangup returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "hangup: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::udub(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.udub(
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "udub returns " << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "udub: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::hangupWaitingOrBackground(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.hangupWaitingOrBackground(
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "hangupWaitingOrBackground returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << std::endl << "hangupWaitingOrBackground: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::hangupForegroundResumeBackground(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.hangupForegroundResumeBackground(
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "hangupForegroundResumeBackground returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << std::endl
              << "hangupForegroundResumeBackground: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::deflectCall(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_DeflectCallInfo reqData{};
  readFromUserDeflectCallInfo(reqData);
  Status s = rilSession.deflectCall(reqData,
                                    [](RIL_Errno e)
                                    {
                                      std::cout << std::endl
                                                << "deflectCall returns "
                                                << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                                << std::endl;
                                      std::cout << "Error: " << e << "." << std::endl;
                                    });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "deflectCall: Failed to issue request." << std::endl;
  }
}

std::string toString(const RIL_HoExtraType type)
{
  switch (type) {
    case RIL_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL:
      return "LTE_TO_IWLAN_HO_FAIL";
    case RIL_HO_XT_TYPE_INVALID:
      return "INVALID";
  }
  return "INVALID";
}

std::string toString(const RIL_HandoverType type)
{
  switch (type) {
    case RIL_HO_START:
      return "START";
    case RIL_HO_COMPLETE_SUCCESS:
      return "COMPLETE_SUCCESS";
    case RIL_HO_COMPLETE_FAIL:
      return "COMPLETE_FAIL";
    case RIL_HO_CANCEL:
      return "CANCEL";
    case RIL_HO_NOT_TRIGGERED:
      return "NOT_TRIGGERED";
    case RIL_HO_NOT_TRIGGERED_MOBILE_DATA_OFF:
      return "NOT_TRIGGERED_MOBILE_DATA_OFF";
    case RIL_HO_INVALID:
      return "INVALID";
    case RIL_HO_UNKNOWN:
      return "UNKNOWN";
  }
  return "INVALID";
}

std::string toString(const RIL_RttMode rttmode)
{
  switch(rttmode) {
    case RIL_RTT_DISABLED:
      return "RIL_RTT_DISABLED";
    case RIL_RTT_FULL:
      return "RIL_RTT_FULL";
  }
  return "INVALID";
}

std::string toString(const RIL_CallModifyFailCause failcause)
{
  switch(failcause) {
    case RIL_CALL_MODIFY_FAIL_CAUSE_UNKNOWN:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_UNKNOWN";
    case RIL_CALL_MODIFY_FAIL_CAUSE_SUCCESS:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_SUCCESS";
    case RIL_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE";
    case RIL_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE";
    case RIL_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_CANCELLED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_CANCELLED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_UNUSED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_UNUSED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER";
    case RIL_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE";
    case RIL_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED";
    case RIL_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION:
      return "RIL_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION";
  }
  return "INVALID";
}

std::string toString(const RIL_TtyModeType mode)
{
  switch(mode) {
    case RIL_TTY_MODE_OFF:
      return "RIL_TTY_MODE_OFF";
    case RIL_TTY_MODE_FULL:
      return "RIL_TTY_MODE_FULL";
    case RIL_TTY_MODE_HCO:
      return "RIL_TTY_MODE_HCO";
    case RIL_TTY_MODE_VCO:
      return "RIL_TTY_MODE_VCO";
    case RIL_TTY_MODE_INVALID:
      return "RIL_TTY_MODE_INVALID";
  }
  return "INVALID";
}

std::string toString(const RIL_VoiceInfoType infotype)
{
  switch(infotype) {
    case RIL_VOICE_INFO_SILENT:
      return "RIL_VOICE_INFO_SILENT";
    case RIL_VOICE_INFO_SPEECH:
      return "RIL_VOICE_INFO_SPEECH";
  }
  return "INVALID";
}

std::string toString(const RIL_HoExtra& extra)
{
  std::ostringstream os;
  os << "     Extra Type: " << toString(extra.type) << std::endl;
  os << "     ExtraInfo: " << (extra.extraInfo ? extra.extraInfo : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_HandoverInfo& info)
{
  std::ostringstream os;
  os << "== Handover Info : == " << std::endl;
  os << "   Type : " << toString(info.type) << std::endl;
  os << "   Source RAT: " << getRatFromValue(info.srcTech) << std::endl;
  os << "   Target RAT: " << getRatFromValue(info.targetTech) << std::endl;

  if (info.hoExtra) {
    os << "   HoExtra: " << std::endl;
    os << toString(*(info.hoExtra)) << std::endl;
  } else {
    os << "   HoExtra: <null>" << std::endl;
  }
  os << "   Error Code: " << (info.errorCode ? info.errorCode : "<null>") << std::endl;
  os << "   Error Code Message: " << (info.errorMessage ? info.errorMessage : "<null>") << std::endl;
  return os.str();
}

std::string toString(const RIL_SuppSvcNotificationInfo& suppSvcNotif)
{
  std::ostringstream os;
  os << "== RIL_SuppSvcNotificationInfo : == " << std::endl;
  os << "   notificationType : " << static_cast<int>(suppSvcNotif.notificationType) << std::endl;
  os << "   code : " << suppSvcNotif.code << std::endl;
  os << "   index : " << suppSvcNotif.index << std::endl;
  os << "   type : " << suppSvcNotif.type << std::endl;
  std::string number{ "<empty>" };
  if (suppSvcNotif.number) {
    number = suppSvcNotif.number;
  }
  os << "   number : " << number << std::endl;
  os << "   connId : " << suppSvcNotif.connId << std::endl;
  std::string historyInfo{ "<empty>" };
  if (suppSvcNotif.historyInfo) {
    historyInfo = suppSvcNotif.historyInfo;
  }
  os << "   historyInfo : " << historyInfo << std::endl;
  os << "   hasHoldTone : " << static_cast<int>(suppSvcNotif.hasHoldTone) << std::endl;
  os << "   holdTone : " << static_cast<int>(suppSvcNotif.holdTone) << std::endl;
  return os.str();
}

std::string toString(const RIL_RefreshConferenceInfo& info) {
  std::ostringstream os;
  os << "Refresh Conference Info : " << std::endl;
  os << info.conferenceCallState << std::endl;
  os << "Conference Uri Len:" << info.confInfoUriLength << std::endl;
  for(int i=0; i<info.confInfoUriLength; i++) {
    os << "confInfoUri[" << i << "]" << info.confInfoUri[i] << std::endl;
  }
  return os.str();
}

std::string toString(const RIL_ToneOperation op)
{
  switch (op) {
    case RIL_TONE_OP_INVALID:
      return "INVALID";
    case RIL_TONE_OP_STOP:
      return "STOP";
    case RIL_TONE_OP_START:
      return "START";
  }
  return "INVALID";
}

std::string toString(const RIL_CDMA_SignalInfoRecord& arg)
{
  std::ostringstream os;
  os << "RIL_CDMA_SignalInfoRecord : " << std::endl;
  os << "isPresent: " << (int)arg.isPresent << std::endl;
  os << "signalType: " << (int)arg.signalType << std::endl;
  os << "alertPitch: " << (int)arg.alertPitch << std::endl;
  os << "signal: " << (int)arg.signal << std::endl;
  return os.str();
}

std::string toString(const RIL_SsRequestType& arg)
{
  switch (arg) {
    case SS_ACTIVATION:
      return "SS_ACTIVATION";
    case SS_DEACTIVATION:
      return "SS_DEACTIVATION";
    case SS_INTERROGATION:
      return "SS_INTERROGATION";
    case SS_REGISTRATION:
      return "SS_REGISTRATION";
    case SS_ERASURE:
      return "SS_ERASURE";
  }
  return "INVALID";
}

std::string toString(const RIL_SsTeleserviceType& arg)
{
  switch (arg) {
    case SS_ALL_TELE_AND_BEARER_SERVICES:
      return "SS_ALL_TELE_AND_BEARER_SERVICES";
    case SS_ALL_TELESEVICES:
      return "SS_ALL_TELESEVICES";
    case SS_TELEPHONY:
      return "SS_TELEPHONY";
    case SS_ALL_DATA_TELESERVICES:
      return "SS_ALL_DATA_TELESERVICES";
    case SS_SMS_SERVICES:
      return "SS_SMS_SERVICES";
    case SS_ALL_TELESERVICES_EXCEPT_SMS:
      return "SS_ALL_TELESERVICES_EXCEPT_SMS";
  }
  return "INVALID";
}

std::string toString(const RIL_SsServiceType& arg)
{
  switch (arg) {
    case SS_CFU:
      return "SS_CFU";
    case SS_CF_BUSY:
      return "SS_CF_BUSY";
    case SS_CF_NO_REPLY:
      return "SS_CF_NO_REPLY";
    case SS_CF_NOT_REACHABLE:
      return "SS_CF_NOT_REACHABLE";
    case SS_CF_ALL:
      return "SS_CF_ALL";
    case SS_CF_ALL_CONDITIONAL:
      return "SS_CF_ALL_CONDITIONAL";
    case SS_CLIP:
      return "SS_CLIP";
    case SS_CLIR:
      return "SS_CLIR";
    case SS_COLP:
      return "SS_COLP";
    case SS_COLR:
      return "SS_COLR";
    case SS_WAIT:
      return "SS_WAIT";
    case SS_BAOC:
      return "SS_BAOC";
    case SS_BAOIC:
      return "SS_BAOIC";
    case SS_BAOIC_EXC_HOME:
      return "SS_BAOIC_EXC_HOME";
    case SS_BAIC:
      return "SS_BAIC";
    case SS_BAIC_ROAMING:
      return "SS_BAIC_ROAMING";
    case SS_ALL_BARRING:
      return "SS_ALL_BARRING";
    case SS_OUTGOING_BARRING:
      return "SS_OUTGOING_BARRING";
    case SS_INCOMING_BARRING:
      return "SS_INCOMING_BARRING";
    case SS_INCOMING_BARRING_DN:
      return "SS_INCOMING_BARRING_DN";
    case SS_INCOMING_BARRING_ANONYMOUS:
      return "SS_INCOMING_BARRING_ANONYMOUS";
  }
  return "INVALID";
}

std::string toString(const RIL_SuppServiceClass& arg)
{
  switch (arg) {
    case SS_VOICE:
      return "SS_VOICE";
    case SS_DATA:
      return "SS_DATA";
    case SS_FAX:
      return "SS_FAX";
    case SS_SMS:
      return "SS_SMS";
    case SS_DATA_SYNC:
      return "SS_DATA_SYNC";
    case SS_DATA_ASYNC:
      return "SS_DATA_ASYNC";
    case SS_PACKET:
      return "SS_PACKET";
    case SS_PAD:
      return "SS_PAD";
    case SS_NONE:
      return "SS_NONE";
  }
  return "INVALID";
}

std::string toString(const RIL_CallForwardData& cfData)
{
  std::ostringstream os;
  os << "  RIL_CallForwardData: " << std::endl;
  os << "  numValidIndexes : " << cfData.numValidIndexes << std::endl;
  for (uint32_t i = 0; i < cfData.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    os << "  cfInfo[" << i << "] : " << std::endl;
    os << toString(cfData.cfInfo[i]) << std::endl;
  }
  return os.str();
}

std::string toString(const RIL_StkCcSsInfo& suppSvcNotif)
{
  std::ostringstream os;
  os << "RIL_StkCcSsInfo: " << std::endl;
  os << " serviceType : " << toString(suppSvcNotif.serviceType) << std::endl;
  os << " requestType : " << toString(suppSvcNotif.requestType) << std::endl;
  os << " teleserviceType : " << toString(suppSvcNotif.teleserviceType) << std::endl;
  os << " serviceClass : " << toString(suppSvcNotif.serviceClass) << std::endl;
  os << " result : " << suppSvcNotif.result << std::endl;

  if ((suppSvcNotif.requestType == SS_INTERROGATION) &&
      (suppSvcNotif.serviceType == SS_CFU || suppSvcNotif.serviceType == SS_CF_BUSY ||
       suppSvcNotif.serviceType == SS_CF_NO_REPLY ||
       suppSvcNotif.serviceType == SS_CF_NOT_REACHABLE || suppSvcNotif.serviceType == SS_CF_ALL ||
       suppSvcNotif.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    os << " cfData: " << toString(suppSvcNotif.cfData);
  } else {
    for (size_t i = 0; i < SS_INFO_MAX; i++) {
      os << " ssInfo[" << i << "]: " << suppSvcNotif.ssInfo[i] << std::endl;
    }
  }
  return os.str();
}

void RILCallMenu::getCurrentCalls(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.getCurrentCalls(
                                    [](RIL_Errno e, const std::vector<RIL_CallInfo> callList)
                                    {
                                      std::cout << std::endl
                                                << "getCurrentCalls returns "
                                                << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                                << std::endl;
                                      std::cout << "Error: " << e << "." << std::endl;
                                      std::cout << "callList.size: " << callList.size() << std::endl;
                                      std::cout << "callList:"  << std::endl;
                                      int i = 0;
                                      for (auto &callInfo : callList) {
                                        std::cout << "callInfo:[" << i << "]\n" << toString(callInfo, " ") << std::endl;
                                        i++;
                                      }
                                    });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "getCurrentCalls: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::registerForIndications(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  rilSession.registerUnsolCallStateChangedIndicationHandler(
      [](const std::vector<RIL_CallInfo> callList)
      {
        std::cout << "UnsolCallStateChanged indication:" << std::endl;
        std::cout << "callList.size: " << callList.size() << std::endl;
        std::cout << "callList:"  << std::endl;
        int i = 0;
        for (auto &callInfo : callList) {
          std::cout << "callInfo[" << i << "]:\n" << toString(callInfo, " ") << std::endl;
          i++;
        }
      });
  rilSession.registerUnsolParticipantStatusInfoIndicationHandler(
      [](const RIL_ParticipantStatusInfo& partiStatusInfo)
      {
        std::cout << "UnsolParticipantStatusInfo indication:" << std::endl
                  << "partiStatusInfo : " << std::endl
                  << toString(partiStatusInfo) << std::endl;
      });
  rilSession.registerUnsolIncomingCallAutoRejectedIndHandler(
      [](const RIL_AutoCallRejectionInfo& autoCallRejInfo)
      {
        std::cout << "UnsolIncomingCallAutoRejected indication:" << std::endl
                  << "autoCallRejInfo : " << std::endl
                  << toString(autoCallRejInfo) << std::endl;
      });
  rilSession.registerUnsolPreAlertingCallInfoAvailableIndHandler(
      [](const RIL_PreAlertingCallInfo& preAlertInfo)
      {
        std::cout << "UnsolPreAlertingCallInfoAvailable indication:" << std::endl
                  << "preAlertInfo : " << std::endl
                  << toString(preAlertInfo) << std::endl;
      });
  rilSession.registerUnsolOnUssdIndicationHandler(
      [](const RIL_UssdModeType& mode, const std::string& message)
      {
        std::cout << "UnsolOnUssd indication:" << std::endl;
        std::cout << "mode : " << toString(mode) << std::endl;
        std::cout << "message : " << message << std::endl;
      });
  rilSession.registerUnsolHandoverIndicationHandler(
      [](const RIL_HandoverInfo& handoverInfo)
      {
        std::cout << "UnsolHandover indication" << std::endl;
        std::cout << toString(handoverInfo) << std::endl;
      });
  rilSession.registerSuppSvcNotificationIndicationHandler(
      [](const RIL_SuppSvcNotificationInfo& suppSvcNotif)
      {
        std::cout << "SuppSvcNotification indication" << std::endl;
        std::cout << toString(suppSvcNotif) << std::endl;
      });
  rilSession.registerUnsolModifyCallIndicationHandler(
      [](const RIL_CallModifyInfo& modifyInfo) {
        std::cout << "Received RIL_CALL_UNSOL_MODIFY_CALL" << std::endl;
        std::cout << " call id: " << modifyInfo.callId << std::endl;
        std::cout << " callType: " << toString(modifyInfo.callType) << std::endl;
        std::cout << " callDomain: " << toString(modifyInfo.callDomain) <<std::endl;
        if (modifyInfo.hasRttMode) {
          std::cout << " rttMode: " << toString(modifyInfo.rttMode) << std::endl;
        }
        if (modifyInfo.hasCallModifyFailCause) {
          std::cout << "callModifyFailCause: " <<toString(modifyInfo.callModifyFailCause);
        }
        std::cout << std::endl;
      });
  rilSession.registerTtyNotificationHandler([](RIL_TtyModeType ttyMode) {
    std::cout << "Received RIL_CALL_UNSOL_TTY_NOTIFICATION" << std::endl;
    std::cout << "== TTY Mode Notification ==" << std::endl << toString(ttyMode) << std::endl;
  });
  rilSession.registerUnsolRefreshConferenceInfo([](const RIL_RefreshConferenceInfo& confInfo) {
    std::cout << "Received UnsolRefreshConferenceInfo" << std::endl;
    std::cout << toString(confInfo) << std::endl;
  });
  rilSession.registerUnsolVoiceInfoIndicationHandler([](const RIL_VoiceInfoType& voiceInfo) {
    std::cout << "Received RIL_CALL_UNSOL_VOICE_INFO" << std::endl;
    std::cout << "voiceInfo = " << toString(voiceInfo) << std::endl;
  });
  rilSession.registerViceInfoHandler([](const std::vector<uint8_t>& viceInfo) {
    std::cout << "Received RIL_CALL_UNSOL_REFRESH_VICE_INFO" << std::endl;
    std::cout << "== CALL VICE Info Notification ==" << std::endl
              << std::string(viceInfo.begin(), viceInfo.end()) << std::endl;
  });
  rilSession.registerVowifiQualityIndicationHandler(
      [](const RIL_VowifiCallQuality& vowifiCallQuality) {
        std::cout << "Received vowifi call quality indication." << std::endl;
        std::cout << " vowifi call quality = " + std::to_string(vowifiCallQuality) << std::endl;
  });
  rilSession.registerRttMessageIndicationHandler([](const std::string& rttMessage) {
    std::cout << "Received rttMessage indication." << std::endl;
    std::cout << " UnsolRttMessage = " + rttMessage << std::endl;
  });
  rilSession.registerCallRingIndicationHdlr(
      [](const RIL_CDMA_SignalInfoRecord* record)
      {
        std::cout << "Received CallRingIndication:" << std::endl;
        if (record) {
          std::cout << toString(*record);
        }
      });
  rilSession.registerRingBackToneIndicationHdlr(
      [](const RIL_ToneOperation& toneOperation)
      {
        std::cout << "Received UnsolRingBackTone:" << std::endl;
        std::cout << "play tone: " << toString(toneOperation) << std::endl;
      });
  rilSession.registerOnSupplementaryServiceIndicationHandler(
      [](const RIL_StkCcSsInfo& stkCcSs)
      {
        std::cout << "Received UnsolOnSs:" << std::endl;
        std::cout << "stkCcSsResponse = " << toString(stkCcSs) << std::endl;
      });
  rilSession.registerEmergencyCallbackModeIndHandler(
      [](RIL_EmergencyCbMode mode)
      {
        std::cout << "EmergencyCbMode indication:" << std::endl;
        std::cout << "mode = ";
        if (mode == RIL_EMERGENCY_CALLBACK_MODE_EXIT) {
          std::cout << "EXIT" << std::endl;
        } else if (mode == RIL_EMERGENCY_CALLBACK_MODE_ENTER) {
          std::cout << "ENTER" << std::endl;
        }
      });
}

void RILCallMenu::setCallForward(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_CallForwardParams cfPrefInfo{};
  readFromUserCallForwardPrefInfo(cfPrefInfo);
  Status s = rilSession.setCallForward(
      cfPrefInfo,
      [](RIL_Errno e, const RIL_SetCallForwardStatusInfo& setCfStatus)
      {
        std::cout << std::endl
                  << "setCallForward returns " << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << e << "." << std::endl
                  << "setCfStatus: " << std::endl
                  << toString(setCfStatus) << std::endl;
      });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "setCallForward: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::queryCallForwardStatus(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    std::cin.clear();
    std::cout << "0. unconditional" << std::endl;
    std::cout << "1. mobile busy" << std::endl;
    std::cout << "2. no reply" << std::endl;
    std::cout << "3. not reachable" << std::endl;
    std::cout << "4. all call forwarding" << std::endl;
    std::cout << "5. all conditional call forwarding" << std::endl;
    std::cout << "Enter reason (integer type): ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);

  if (_userInput == -1) {
    return;
  }

  uint32_t reason = _userInput;

  do {
    std::cin.clear();
    std::cout << "1. voice (telephony)" << std::endl;
    std::cout << "2. data" << std::endl;
    std::cout << "4. fax" << std::endl;
    std::cout << "8. short message service" << std::endl;
    std::cout << "16. data circuit sync" << std::endl;
    std::cout << "32. data circuit async" << std::endl;
    std::cout << "64. dedicated packet access" << std::endl;
    std::cout << "128. dedicated PAD access" << std::endl;
    std::cout << "Enter serviceClass(sum of integers representing services): ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);

  if (_userInput == -1) {
    return;
  }
  uint32_t serviceClass = _userInput;

  Status s = rilSession.queryCallForwardStatus(
      reason,
      serviceClass,
      [](RIL_Errno e, const RIL_QueryCallForwardStatusInfo& cfStatusInfo)
      {
        std::cout << std::endl
                  << "queryCallForwardStatus returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl
                  << "cfStatusInfo: " << std::endl
                  << toString(cfStatusInfo) << std::endl;
      });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "queryCallForwardStatus: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::setCallWaiting(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_CallWaitingSettings cwSettings{};
  readCallWaitingSettingsFromUser(cwSettings);
  if (cwSettings.serviceClass == std::numeric_limits<uint32_t>::max()) {
    return;
  }

  Status s = rilSession.setCallWaiting(cwSettings,
                                       [](RIL_Errno e, const RIL_IMS_SipErrorInfo& sipErrorInfo)
                                       {
                                         std::cout << std::endl
                                                   << "setCallWaiting returns "
                                                   << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                                   << std::endl;
                                         std::cout << "Error: " << e << "." << std::endl
                                                   << "SipErrorInfo: " << sipErrorInfo << std::endl;
                                       });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "setCallWaiting: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::queryCallWaiting(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    std::cin.clear();
    std::cout << "1. voice (telephony)" << std::endl;
    std::cout << "2. data" << std::endl;
    std::cout << "4. fax" << std::endl;
    std::cout << "8. short message service" << std::endl;
    std::cout << "16. data circuit sync" << std::endl;
    std::cout << "32. data circuit async" << std::endl;
    std::cout << "64. dedicated packet access" << std::endl;
    std::cout << "128. dedicated PAD access" << std::endl;
    std::cout << "Enter serviceClass(sum of integers representing services): ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);

  if (_userInput == -1) {
    return;
  }

  uint32_t serviceClass = _userInput;
  Status s = rilSession.queryCallWaiting(
      serviceClass,
      [](RIL_Errno e, const RIL_QueryCallWaitingResponse& callWaitingResp)
      {
        std::cout << std::endl
                  << "queryCallWaiting returns " << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << e << "." << std::endl
                  << "QueryCallWaitingResponse:\n"
                  << toString(callWaitingResp) << std::endl;
      });

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "queryCallWaiting: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::lastCallFailCause(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.lastCallFailCause(
      [](RIL_Errno e, const RIL_LastCallFailCauseInfo& cause) -> void
      {
        std::cout << std::endl
                  << "lastCallFailCause returns " << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << e << "." << std::endl
                  << "cause: " << toString(cause) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "lastCallFailCause: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::sendUssd(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::string ussdData;
  std::cin.clear();
  std::cout << "Enter ussd : ";
  std::cin >> ussdData;

  Status s = rilSession.sendUssd(ussdData,
                                 [](RIL_Errno e) -> void
                                 {
                                   std::cout << std::endl
                                             << "sendUssd returns "
                                             << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                             << std::endl;
                                   std::cout << "Error: " << e << "." << std::endl;
                                 });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "sendUssd: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::cancelUssd(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.cancelUssd(
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "cancelUssd returns " << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "cancelUssd: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::changeBarringPassword(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::string facility;
  std::string oldPassword;
  std::string newPassword;
  std::cout << "AO. BAOC (Barr All Outgoing Calls)" << std::endl;
  std::cout << "OI. BOIC (Barr Outgoing International Calls)" << std::endl;
  std::cout << "OX. BOIC exHC (Barr Outgoing International Calls except to Home Country)"
            << std::endl;
  std::cout << "AI. BAIC (Barr All Incoming Calls)" << std::endl;
  std::cout << "IR. BIC Roam (Barr Incoming Calls when Roaming outside the home country)"
            << std::endl;
  std::cout << "AB. All Barring services" << std::endl;
  std::cout << "AG. All outGoing barring services" << std::endl;
  std::cout << "AC. All inComing barring services" << std::endl;
  std::cout << "Enter Facility: " << std::endl;
  std::cin >> facility;
  std::cout << "Enter OldPassword: " << std::endl;
  std::cin >> oldPassword;
  std::cout << "Enter NewPassword: " << std::endl;
  std::cin >> newPassword;

  Status s = rilSession.changeBarringPassword(
      facility,
      oldPassword,
      newPassword,
      [](RIL_Errno e) -> void
      {
        std::cout << std::endl
                  << "changeBarringPassword returns "
                  << ((e == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << e << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "changeBarringPassword: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::exitEmergencyCallbackMode(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.exitEmergencyCbMode(
      [](RIL_Errno err) -> void
      {
        std::cout << std::endl
                  << "exitEmergencyCbMode returns "
                  << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "exitEmergencyCbMode: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::queryClip(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.queryClip(
      [](RIL_Errno err, const RIL_ClipInfo& clipInfo) -> void
      {
        std::cout << std::endl
                  << "queryClip returns " << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
        std::cout << "queryClip response: clipStatus: " << clipInfo.clipStatus << std::endl;
        std::cout << "errorDetails:" << std::endl
                  << (clipInfo.errorDetails ? toString(*clipInfo.errorDetails, " ") : (" <null>"))
                  << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "queryClip: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::getClir(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.getClir(
      [](RIL_Errno err, const RIL_ClirInfo& clirInfo) -> void
      {
        std::cout << std::endl
                  << "getClir returns " << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
        std::cout << "getClir response: action: " << clirInfo.action << std::endl;
        std::cout << "presentation: " << clirInfo.presentation << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "getClir: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::setClir(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int type;
  std::cout << "Select one of the below options" << std::endl;
  std::cout << "0 - Subscription Default" << std::endl;
  std::cout << "1 - CLIR invocation" << std::endl;
  std::cout << "2 - CLIR Suppression" << std::endl;
  std::cout << "Enter Choice:";
  std::cin >> type;
  Status s = rilSession.setClir(&type,
                                [](RIL_Errno err) -> void
                                {
                                  std::cout << std::endl
                                            << "setClir returns "
                                            << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                            << std::endl;
                                  std::cout << "Error: " << err << "." << std::endl;
                                });
  if (s != Status::SUCCESS) {
    std::cout << "setClir: Failed to issue request." << std::endl;
  }
}

std::string toString(const RIL_ServiceClassStatus& status)
{
  switch (status) {
    case RIL_SERVICE_CLASS_STATUS_DISABLED:
      return "SERVICE_CLASS_STATUS_DISABLED";
    case RIL_SERVICE_CLASS_STATUS_ENABLED:
      return "SERVICE_CLASS_STATUS_ENABLED";
  }
  return "INVALID";
}

void readServiceClassStatus(RIL_ServiceClassStatus& status)
{
  int32_t serviceClass;
  std::cout << "Select one of the below options" << std::endl;
  std::cout << "0. RIL_SERVICE_CLASS_STATUS_DISABLED" << std::endl;
  std::cout << "1. RIL_SERVICE_CLASS_STATUS_ENABLED" << std::endl;
  do {
    cout << "Enter RIL_ServiceClassStatus : ";
    std::cin >> serviceClass;
  } while (serviceClass < 0 || serviceClass > 1);
  status = static_cast<RIL_ServiceClassStatus>(serviceClass);
}

void RILCallMenu::setSuppSvcNotification(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  bool enabled;
  std::cout << "Enter enabled (1 for TRUE, 0 for FALSE): " << std::endl;
  std::cin >> enabled;

  Status s = rilSession.setSuppSvcNotification(
      enabled,
      [](RIL_Errno err) -> void
      {
        std::cout << std::endl
                  << "setSuppSvcNotification returns "
                  << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "setSuppSvcNotification: Failed to issue request." << std::endl;
  }
}

void readSuppSvcOperationType(RIL_SuppSvcOperationType& arg)
{
  int var;
  std::cout << "0. RIL_SUPP_OPERATION_ACTIVATE" << std::endl;
  std::cout << "1. RIL_SUPP_OPERATION_DEACTIVATE" << std::endl;
  std::cout << "2. RIL_SUPP_OPERATION_QUERY" << std::endl;
  std::cout << "3. RIL_SUPP_OPERATION_REGISTER" << std::endl;
  std::cout << "4. RIL_SUPP_OPERATION_ERASURE" << std::endl;
  do {
    std::cout << "Enter RIL_SuppSvcOperationType: " << std::endl;
    std::cin >> var;
  } while (var < 0 || var > 4);
  arg = static_cast<RIL_SuppSvcOperationType>(var);
}

void readSuppSvcFacilityType(RIL_FacilityType& arg)
{
  int var;
  std::cout << "0. RIL_FACILITY_CLIP" << std::endl;
  std::cout << "1. RIL_FACILITY_COLP" << std::endl;
  std::cout << "2. RIL_FACILITY_BAOC" << std::endl;
  std::cout << "3. RIL_FACILITY_BAOIC" << std::endl;
  std::cout << "4. RIL_FACILITY_BAOICxH" << std::endl;
  std::cout << "5. RIL_FACILITY_BAIC" << std::endl;
  std::cout << "6. RIL_FACILITY_BAICr" << std::endl;
  std::cout << "7. RIL_FACILITY_BA_ALL" << std::endl;
  std::cout << "8. RIL_FACILITY_BA_MO" << std::endl;
  std::cout << "9. RIL_FACILITY_BA_MT" << std::endl;
  std::cout << "10. RIL_FACILITY_BS_MT" << std::endl;
  std::cout << "11. RIL_FACILITY_BAICa" << std::endl;
  do {
    std::cout << "Enter RIL_FacilityType: " << std::endl;
    std::cin >> var;
  } while (var < 0 || var > 11);
  arg = static_cast<RIL_FacilityType>(var);
}

void readCallBarringSettingsFromUser(RIL_CallBarringNumbersListInfo& cbNumInfo)
{
  std::cout << "Input RIL_CallBarringNumbersListInfo:" << std::endl;
  std::cout << "Enter serviceClass : ";
  std::cin >> cbNumInfo.serviceClass;
  std::cout << "Enter callBarringNumbersInfoLen : ";
  std::cin >> cbNumInfo.callBarringNumbersInfoLen;
  if (cbNumInfo.callBarringNumbersInfoLen) {
    cbNumInfo.callBarringNumbersInfo =
        new RIL_CallBarringNumbersInfo[cbNumInfo.callBarringNumbersInfoLen];
    for (size_t i = 0; i < cbNumInfo.callBarringNumbersInfoLen; i++) {
      std::cout << "Enter callBarringNumbersInfo[" << static_cast<int>(i) << "] : " << std::endl;
      std::cout << "Input RIL_CallBarringNumbersInfo:" << std::endl;
      readServiceClassStatus(cbNumInfo.callBarringNumbersInfo[i].status);
      std::cout << "Enter number : ";
      std::string number;
      std::cin >> number;
      cbNumInfo.callBarringNumbersInfo[i].number = nullptr;
      if (!number.empty()) {
        cbNumInfo.callBarringNumbersInfo[i].number = new char[number.length() + 1];
        if (cbNumInfo.callBarringNumbersInfo[i].number) {
          strlcpy(cbNumInfo.callBarringNumbersInfo[i].number, number.c_str(), number.length() + 1);
        }
      }
    }
  }
}

void readSuppSvcRequestFromUser(RIL_SuppSvcRequest& suppSvcReq)
{
  std::cout << "Input RIL_SuppSvcRequest:" << std::endl;
  std::cin.clear();
  readSuppSvcOperationType(suppSvcReq.operationType);
  readSuppSvcFacilityType(suppSvcReq.facilityType);
  suppSvcReq.callBarringNumbersListInfo = new RIL_CallBarringNumbersListInfo;
  if (suppSvcReq.callBarringNumbersListInfo) {
    readCallBarringSettingsFromUser(*suppSvcReq.callBarringNumbersListInfo);
  }

  std::cout << "Enter password : ";
  std::string password;
  std::cin >> password;
  suppSvcReq.password = nullptr;
  if (!password.empty()) {
    suppSvcReq.password = new char[password.length() + 1];
    if (suppSvcReq.password) {
      strlcpy(suppSvcReq.password, password.c_str(), password.length() + 1);
    }
  }
}

std::string toString(const RIL_FacilityType data)
{
  switch (data) {
    case RIL_FACILITY_CLIP:
      return "RIL_FACILITY_CLIP";
    case RIL_FACILITY_COLP:
      return "RIL_FACILITY_COLP";
    case RIL_FACILITY_BAOC:
      return "RIL_FACILITY_BAOC";
    case RIL_FACILITY_BAOIC:
      return "RIL_FACILITY_BAOIC";
    case RIL_FACILITY_BAOICxH:
      return "RIL_FACILITY_BAOICxH";
    case RIL_FACILITY_BAIC:
      return "RIL_FACILITY_BAIC";
    case RIL_FACILITY_BAICr:
      return "RIL_FACILITY_BAICr";
    case RIL_FACILITY_BA_ALL:
      return "RIL_FACILITY_BA_ALL";
    case RIL_FACILITY_BA_MO:
      return "RIL_FACILITY_BA_MO";
    case RIL_FACILITY_BA_MT:
      return "RIL_FACILITY_BA_MT";
    case RIL_FACILITY_BS_MT:
      return "RIL_FACILITY_BS_MT";
    case RIL_FACILITY_BAICa:
      return "RIL_FACILITY_BAICa";
  }
  return "INVALID";
}

std::string toString(const RIL_ServiceClassProvisionStatus status)
{
  switch (status) {
    case RIL_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED:
      return "RIL_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED";
    case RIL_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED:
      return "RIL_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED";
  }
  return "INVALID";
}

std::string toString(const RIL_CallBarringNumbersInfo& cbNumInfo)
{
  std::ostringstream os;
  os << "  RIL_CallBarringNumbersInfo : ";
  os << "    status : " << toString(cbNumInfo.status) << std::endl;
  if (cbNumInfo.number) {
    os << "    number : " << cbNumInfo.number << std::endl;
  }
  return os.str();
}

std::string toString(const RIL_CallBarringNumbersListInfo& cbNumInfo)
{
  std::ostringstream os;
  os << "RIL_CallBarringNumbersListInfo : ";
  os << "  serviceClass : " << cbNumInfo.serviceClass << std::endl;
  os << "  callBarringNumbersInfoLen : "
     << static_cast<uint32_t>(cbNumInfo.callBarringNumbersInfoLen) << std::endl;
  if (cbNumInfo.callBarringNumbersInfoLen) {
    for (size_t i = 0; i < cbNumInfo.callBarringNumbersInfoLen; i++) {
      os << "  callBarringNumbersInfo[" << static_cast<int>(i) << "] :" << std::endl;
      os << toString(cbNumInfo.callBarringNumbersInfo[i]) << std::endl;
    }
  }
  return os.str();
}

std::string toString(const RIL_SuppSvcResponse& suppSvcResp)
{
  std::ostringstream os;
  os << "RIL_SuppSvcResponse valid for QUERY : " << std::endl;
  os << "Service class status : " << toString(suppSvcResp.status) << std::endl;
  os << "Provision status: " << toString(suppSvcResp.provisionStatus) << std::endl;
  os << "Facility type : " << toString(suppSvcResp.facilityType) << std::endl;
  os << "callBarringNumbersListInfoLen : "
     << static_cast<uint32_t>(suppSvcResp.callBarringNumbersListInfoLen) << std::endl;
  if (suppSvcResp.callBarringNumbersListInfoLen) {
    for (size_t i = 0; i < suppSvcResp.callBarringNumbersListInfoLen; i++) {
      os << "  callBarringNumbersListInfo[" << static_cast<int>(i) << "] : " << std::endl;
      os << toString(suppSvcResp.callBarringNumbersListInfo[i]) << std::endl;
    }
  }
  if (suppSvcResp.errorDetails) {
    os << *suppSvcResp.errorDetails << std::endl;
  }
  os << "isPasswordRequired: " << suppSvcResp.isPasswordRequired << std::endl;
  return os.str();
}

void RILCallMenu::suppSvcStatus(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_SuppSvcRequest suppSvcReq{};
  readSuppSvcRequestFromUser(suppSvcReq);

  Status s = rilSession.suppSvcStatus(
      suppSvcReq,
      [](RIL_Errno err, const RIL_SuppSvcResponse& resp)
      {
        std::cout << std::endl;
        std::cout << "suppSvcStatus returns " << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
        std::cout << "suppSvcStatus response: " << toString(resp) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "suppSvcStatus: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::separateConnection(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }

  auto connid = std::stoi(userInput[1]);
  Status status = rilSession.separateConnection(
      connid,
      [](RIL_Errno err) -> void
      {
        std::cout << "separateConnection returns "
                  << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << "separateConnection: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::setTtyMode(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int mode;
  std::cout << "Enter TTY mode:" << std::endl;
  std::cin >> mode;
  Status s = rilSession.setTtyMode(mode,
                                   [](RIL_Errno err) -> void
                                   {
                                     std::cout << "setTtyMode returns "
                                               << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                               << std::endl;
                                     std::cout << "Error: " << err << "." << std::endl;
                                   });
  if (s != Status::SUCCESS) {
    std::cout << "setTtyMode: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::getTtyMode(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getTtyMode(
      [](RIL_Errno err, int mode) -> void
      {
        std::cout << "getTtyMode returns " << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
        std::cout << "TTY mode: " << mode << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "getTtyMode: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::hold(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId: ";
  std::cin >> callId;
  Status status = rilSession.hold(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << std::endl
               << "hold returns "
               << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
               << std::endl;
        std::cout << "errorDetails: " << errorDetails<< std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << std::endl << "hold: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::resume(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId: ";
  std::cin >> callId;
  Status status = rilSession.resume(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
         std::cout << std::endl
               << "resume returns "
               << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
               << std::endl;
         std::cout << "errorDetails: " << errorDetails<<std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << std::endl << "resume: Failed to issue request." << std::endl;
  }
}

std::string toString(const RIL_IpPresentation presentation) {
  std::ostringstream os;
  switch (presentation) {
  case RIL_IP_PRESENTATION_NUM_ALLOWED:
    os << "IP_PRESENTATION_NUM_ALLOWED";
    break;
  case RIL_IP_PRESENTATION_NUM_RESTRICTED:
    os << "IP_PRESENTATION_NUM_RESTRICTED";
    break;
  case RIL_IP_PRESENTATION_NUM_DEFAULT:
    os << "IP_PRESENTATION_NUM_DEFAULT";
    break;
  case RIL_IP_PRESENTATION_INVALID:
    os << "IP_PRESENTATION_INVALID";
    break;
  }
  return os.str();
}

std::string toString(const RIL_ColrInfo& colr) {
  std::ostringstream os;
  os << "Service class status : " << toString(colr.status) << std::endl;
  os << "Provision status: " << toString(colr.provisionStatus) << std::endl;
  os << "Presentation: "<< toString(colr.presentation) << std::endl;
  if (colr.errorDetails)
    os << colr.errorDetails << std::endl;
  return os.str();
}

void RILCallMenu::getColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getColr(
        [](RIL_Errno err, const RIL_ColrInfo& colr) {
           std::cout << std::endl
                     << "getColr returns "
                     << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                     << toString(colr)
                     << std::endl;
        });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "getColr: Failed to issue request." << std::endl;
  }
}

void readServiceClassProvisionStatus(RIL_ServiceClassProvisionStatus &status) {
  std::cout << "provisionStatus: (0 - Not provisioned 1 - Provisioned): " << std::endl;
  uint32_t input;
  do {
    cin >> input;
  } while (input != 0 && input != 1);
  status = static_cast<RIL_ServiceClassProvisionStatus>(input);
}

void readIpPresentation(RIL_IpPresentation &ipPresentation)
{
    int var;
    cout << "0. RIL_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
    cout << "1. RIL_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
    cout << "2. RIL_IP_PRESENTATION_NUM_DEFAULT" << std::endl;
    cout << "3. RIL_IP_PRESENTATION_INVALID" << std::endl;
    do {
        cout << "Enter IP Presentation Type:";
        cin >> var;
    } while (var < 0 || var > 3);
    ipPresentation = static_cast<RIL_IpPresentation>(var);
}

void RILCallMenu::setColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IpPresentation colr;
  readIpPresentation(colr);

  Status s = rilSession.setColr(colr,
        [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errInfo) {
           std::cout << std::endl
                     << "setColr returns "
                     << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                     << std::endl;
           std::cout << "errorDetails: " << errInfo<<std::endl;
        });
  if (s != Status::SUCCESS) {
    std::cout << std::endl << "setColr: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::addParticipant(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    std::string address;
    std::cout << "Enter the address of the participant to add to the call: ";
    std::cin >> address;
    Status s = rilSession.addParticipant(
        address.c_str(),
        [] (RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
            std::cout << std::endl
                      << "addParticipant returns "
                      << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                      << std::endl;
            std::cout << "errorDetails: " << errorDetails<< std::endl;
        }
    );

    if(s != Status::SUCCESS) {
        std::cout << "addParticipant: Failed to issue request"<< std::endl;

    }
}

void readCallType(RIL_CallType &callType)
{
    int var;
    cout << "0. RIL_CALL_TYPE_UNKNOWN" << std::endl;
    cout << "1. RIL_CALL_TYPE_VOICE" << std::endl;
    cout << "2. RIL_CALL_TYPE_VT_TX" << std::endl;
    cout << "3. RIL_CALL_TYPE_VT_RX" << std::endl;
    cout << "4. RIL_CALL_TYPE_VT" << std::endl;
    cout << "5. RIL_CALL_TYPE_VT_NODIR" << std::endl;
    cout << "6. RIL_CALL_TYPE_SMS" << std::endl;
    cout << "7. RIL_CALL_TYPE_UT" << std::endl;
    do {
        cout << "Enter Call Type:";
        cin >> var;
    } while (var < 0 || var > 7);
    callType = static_cast<RIL_CallType>(var);
}

void readCallDomain(RIL_CallDomain &arg)
{
    int var;
    cout << "0. RIL_CALLDOMAIN_UNKNOWN" << std::endl;
    cout << "1. RIL_CALLDOMAIN_CS" << std::endl;
    cout << "2. RIL_CALLDOMAIN_PS" << std::endl;
    cout << "3. RIL_CALLDOMAIN_AUTOMATIC" << std::endl;
    cout << "4. RIL_CALLDOMAIN_NOT_SET" << std::endl;
    cout << "5. RIL_CALLDOMAIN_INVALID" << std::endl;
    do {
        cout << "Enter Call Domain:";
        cin >> var;
    } while (var < 0 || var > 5);
    arg = static_cast<RIL_CallDomain>(var);
}


void readRttMode(RIL_RttMode &rttMode)
{
    int var;
    cout << "0. RIL_RTT_DISABLED" << std::endl;
    cout << "1. RIL_RTT_FULL" << std::endl;
    do {
        cin >> var;
    } while (var < 0 || var > 1);
    rttMode = static_cast<RIL_RttMode>(var);
}

void readCallModifyInfo(RIL_CallModifyInfo &arg)
{
  std::cout << "Enter callId:" << std::endl;
  cin >> arg.callId;
  readCallType(arg.callType);
  readCallDomain(arg.callDomain);
  arg.hasRttMode = 1;
  readRttMode(arg.rttMode);
  // CallModifyFailCause not set for req
  arg.hasCallModifyFailCause = 0;
}

void RILCallMenu::modifyCallInitiate(std::vector<std::string> userInput) {
  RIL_CallModifyInfo modifyInfo{};
  readCallModifyInfo(modifyInfo);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.modifyCallInitiate(modifyInfo, [](RIL_Errno err) -> void {
       std::cout << std::endl
                 << "modifyCallInitiate returns "
                 << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                 << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "modifyCallInitiate returns: Failed to issue request" << std::endl;
  }
}

void RILCallMenu::modifyCallConfirm(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_CallModifyInfo modifyInfo{};
  readCallModifyInfo(modifyInfo);
  Status status = rilSession.modifyCallConfirm(modifyInfo, [](RIL_Errno err) -> void {
       std::cout << std::endl
                 << "modifyCallConfirm returns "
                 << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                 << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "modifyCallConfirm returns: Failed to issue request" << std::endl;
  }
}

void RILCallMenu::cancelModifyCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.cancelModifyCall(callId, [](RIL_Errno err) -> void {
       std::cout << std::endl
                 << "cancelModifyCall returns "
                 << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                 << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "cancelModifyCall returns: Failed to issue request" << std::endl;
  }
}

void readTtyModeType(RIL_TtyModeType &arg)
{
    int var;
    cout << "0. RIL_TTY_MODE_OFF" << std::endl;
    cout << "1. RIL_TTY_MODE_FULL" << std::endl;
    cout << "2. RIL_TTY_MODE_HCO" << std::endl;
    cout << "3. RIL_TTY_MODE_VCO" << std::endl;
    cout << "4. RIL_TTY_MODE_INVALID" << std::endl;
    do {
        cout << "Enter RIL_TtyModeType: " << std::endl;
        cin >> var;
    } while (var < 0 || var > 4);
    arg = static_cast<RIL_TtyModeType>(var);
}

void RILCallMenu::sendUiTtyMode(std::vector<std::string> userInput) {
  RIL_TtyModeType ttyMode;
  readTtyModeType(ttyMode);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.sendUiTtyMode(ttyMode, [](RIL_Errno err) -> void {
       std::cout << std::endl
                 << "sendUiTtyMode returns "
                 << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                 << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "sendUiTtyMode returns: Failed to issue request" << std::endl;
  }
}


void RILCallMenu::sendRttMessage(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::string msg;
  std::cout << "Enter rtt message:" << std::endl;
  std::cin >> msg;

  Status s = rilSession.sendRttMessage(msg, [](RIL_Errno err) -> void {
  std::cout << std::endl
            << "sendRttMessage returns "
            << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
            << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "sendRttMessage returns: Failed to issue request" << std::endl;
  }
}

void RILCallMenu::playDtmfTone(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2 || userInput[1].empty()) {
    std::cout << "Incorrect arguments. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.dtmf(userInput[1][0],
                             [](RIL_Errno err) -> void
                             {
                               std::cout << "playDtmfTone returns"
                                         << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                         << std::endl;
                               std::cout << "Error: " << err << "." << std::endl;
                             });
  if (s != Status::SUCCESS) {
    std::cout << "playDtmfTone: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::startDtmfTone(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2 || userInput[1].empty()) {
    std::cout << "Incorrect arguments. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.dtmfStart(userInput[1][0],
                                  [](RIL_Errno err) -> void
                                  {
                                    std::cout << "startDtmfTone returns"
                                              << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                              << std::endl;
                                    std::cout << "Error: " << err << "." << std::endl;
                                  });
  if (s != Status::SUCCESS) {
    std::cout << "startDtmfTone: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::stopDtmfTone(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.dtmfStop(
      [](RIL_Errno err) -> void
      {
        std::cout << "stopDtmfTone returns" << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "stopDtmfTone: Failed to issue request." << std::endl;
  }
}

void readExplicitCallTransferInfo(RIL_ExplicitCallTransfer& explicitCall)
{
  std::cout << "Enter explicit call transfer parameters : " << std::endl;
  std::cout << "For CS calls enter 0, for IMS calls enter 1" << std::endl;
  int isValid;
  std::cin >> isValid;

  explicitCall.isValid = static_cast<int8_t>(isValid);
  if (!explicitCall.isValid) {
    return;
  }

  std::cout << "Enter callId : " << std::endl;
  std::cin >> explicitCall.callId;

  cout << " Enter ECT type : " << std::endl;
  cout << "0. RIL_ECT_TYPE_UNKNOWN" << std::endl;
  cout << "1. RIL_ECT_TYPE_BLIND_TRANSFER" << std::endl;
  cout << "2. RIL_ECT_TYPE_ASSURED_TRANSFER" << std::endl;
  cout << "3. RIL_ECT_TYPE_CONSULTATIVE_TRANSFER" << std::endl;

  int var;
  do {
    cout << "Enter RIL_EctType : " << std::endl;
    std::cin >> var;
  } while (var < 0 || var > 3);
  explicitCall.ectType = static_cast<RIL_EctType>(var);

  std::string tempString;
  std::cout << "Enter targetAddress, provide valid target address or empty (\"\") : " << std::endl;
  std::cin >> tempString;
  explicitCall.targetAddress = nullptr;
  if (!tempString.empty() && tempString != "\"\"") {
    explicitCall.targetAddress = new char[tempString.size() + 1]();
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
  return;
}

void RILCallMenu::explicitCallTransfer(std::vector<std::string> userInput)
{
  RIL_ExplicitCallTransfer info{};
  readExplicitCallTransferInfo(info);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.explicitCallTransfer(
      info,
      [](RIL_Errno err) -> void
      {
        std::cout << "explicitCallTransfer returns"
                  << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "explicitCallTransfer: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::conference(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.conference(
      [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void
      {
        std::cout << "conference returns" << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                  << std::endl;
        std::cout << "Error: " << err << "." << std::endl;
        std::cout << "errorDetails: " << errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "conference: Failed to issue request." << std::endl;
  }
}

void RILCallMenu::dial(std::vector<std::string> userInput)
{
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_DialParams dialParams = {};
  dialParams.address = const_cast<char*>(userInput[1].c_str());
  dialParams.clirMode = 0;
  dialParams.uusInfo = nullptr;

  cout << " Enter domain to place call : " << std::endl;
  cout << "1. RIL_DOMAIN_CS" << std::endl;
  cout << "2. RIL_DOMAIN_PS" << std::endl;
  cout << "3. RIL_DOMAIN_AUTOMATIC" << std::endl;

  int domain;
  std::cin >> domain;
  dialParams.callDetails.callDomain = static_cast<RIL_CallDomain>(domain);

  Status s = rilSession.dial(dialParams,
                             [](RIL_Errno err) -> void
                             {
                               std::cout << "dial returns"
                                         << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                         << std::endl;
                               std::cout << "Error: " << err << "." << std::endl;
                             });
  if (s != Status::SUCCESS) {
    std::cout << "dial: Failed to issue request." << std::endl;
  }
}

void readAnswerParams(RIL_Answer& answer)
{
  std::cout << "Enter answer parameters: " << std::endl;
  std::cout << "For CS calls enter 0, for IMS calls enter 1" << std::endl;
  int isValid;
  std::cin >> isValid;

  answer.isValid = static_cast<int8_t>(isValid);
  if (!answer.isValid) {
    return;
  }

  int callType;
  std::cout << "Enter callType to accept call: " << std::endl;
  cout << "1. RIL_CALL_TYPE_VOICE" << std::endl;
  cout << "2. RIL_CALL_TYPE_VT_TX" << std::endl;
  cout << "3. RIL_CALL_TYPE_VT_RX" << std::endl;
  cout << "4. RIL_CALL_TYPE_VT" << std::endl;
  std::cin >> callType;
  answer.callType = static_cast<RIL_CallType>(callType);

  int presentation;
  std::cout << "Enter presentation: " << std::endl;
  cout << "0. RIL_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
  cout << "1. RIL_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
  cout << "2. RIL_IP_PRESENTATION_NUM_DEFAULT" << std::endl;
  std::cin >> presentation;
  answer.presentation = static_cast<RIL_IpPresentation>(presentation);

  int rttMode;
  std::cout << "Enter RTT mode: " << std::endl;
  cout << "0. RIL_RTT_DISABLED" << std::endl;
  cout << "1. RIL_RTT_FULL" << std::endl;
  std::cin >> rttMode;
  answer.rttMode = static_cast<RIL_RttMode>(rttMode);
}

void RILCallMenu::acceptCall(std::vector<std::string> userInput)
{
  RIL_Answer answer{};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  readAnswerParams(answer);
  Status s = rilSession.answer(answer,
                               [](RIL_Errno err) -> void
                               {
                                 std::cout << "acceptCall returns"
                                           << ((err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE")
                                           << std::endl;
                                 std::cout << "Error: " << err << "." << std::endl;
                               });
  if (s != Status::SUCCESS) {
    std::cout << "acceptCall: Failed to issue request." << std::endl;
  }
}
