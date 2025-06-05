/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file ril_call.h
 * @brief Contains data structures related to RIL CALL requests
 */

#ifndef __RIL_CALL_H_
#define __RIL_CALL_H_ 1

#include <telephony/ril.h>
#include <telephony/ril_ims.h>

typedef RIL_IMS_SipErrorInfo RIL_SipErrorInfo;

/**
 * Conference Participant Operations
 */
enum RIL_ConfParticipantOperation {
  RIL_CONF_PARTICIPANT_OP_UNKNOWN, /** Unknown */
  RIL_CONF_PARTICIPANT_OP_ADD,     /** Add */
  RIL_CONF_PARTICIPANT_OP_REMOVE,  /** Remove */
};

/**
 * Verstat verification status
 */
enum RIL_VerstatVerificationStatus {
  RIL_VERSTAT_VERIFICATION_STATUS_NONE, /** None */
  RIL_VERSTAT_VERIFICATION_STATUS_PASS, /** Pass */
  RIL_VERSTAT_VERIFICATION_STATUS_FAIL, /** Fail */
};

/**
 * Call Composer priority
 */
enum RIL_CallComposerPriority {
  RIL_CALL_COMPOSER_PRIORITY_INVALID, /** Invalid */
  RIL_CALL_COMPOSER_PRIORITY_URGENT,  /** Urgent */
  RIL_CALL_COMPOSER_PRIORITY_NORMAL,  /** Normal */
};

/**
 * Call Domain.
 */
enum RIL_CallDomain {
  RIL_CALLDOMAIN_UNKNOWN, /**< Unknown */
  RIL_CALLDOMAIN_CS,      /**< Circuit Switched Domain */
  RIL_CALLDOMAIN_PS,      /**< Packet Switched Domain */
  RIL_CALLDOMAIN_AUTOMATIC, /**< Automatic domain - domain for a new call should be selected by modem */
};

/**
 * Call Modified cause.
 */
enum RIL_CallModifiedCause {
  RIL_CMODCAUSE_NONE = 0,                         /**< No call modify cause information \n  */
  RIL_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ = 1,     /**< Upgrade due to a local request \n  */
  RIL_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ = 2,    /**< Upgrade due to a remote request \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ = 3,   /**< Downgrade due to a local request \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ = 4,  /**< Downgrade due to a remote request \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT = 5, /**< Downgrade due to an RTP timeout \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_QOS = 6,         /**< Downgrade due to QOS \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS = 7, /**< Downgrade due to a packet loss \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT = 8,  /**< Downgrade due to low throughput \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION =
      9,                                             /**< Downgrade due to thermal mitigation \n  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC = 10,       /**< Downgrade due to lipsync  */
  RIL_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR = 11, /**< Downgrade due to a general error \n  */
};

/**
 * Verification status
 */
enum RIL_VerificationStatus {
  RIL_VERSTAT_UNKNOWN, /** Unknown */
  RIL_VERSTAT_NONE,    /** Telephone number is not validated. */
  RIL_VERSTAT_PASS,    /** Telephone number validation passed. */
  RIL_VERSTAT_FAIL,    /** Telephone number validation failed. */
};

/**
 * TIR mode
 */
enum RIL_TirMode {
  RIL_TIRMODE_UNKNOWN,
  RIL_TIRMODE_TEMPORARY,
  RIL_TIRMODE_PERMANENT,
};

/**
 * Notification Type of Call Progress Info
 */
enum RIL_CallProgressInfoType {
  RIL_CALL_PROGRESS_INFO_UNKNOWN,         /**< Invalid if CallProgressInfo is not present. */
  RIL_CALL_PROGRESS_INFO_CALL_REJ_Q850,   /**< MO call will be rejected with protocol Q850 error */
  RIL_CALL_PROGRESS_INFO_CALL_WAITING,    /**< There is already an ACTIVE call at remote UE side and
                                                 this call is a WAITING call from remote UE    perspective.    */
  RIL_CALL_PROGRESS_INFO_CALL_FORWARDING, /**< Call forwarding is enabled at remote UE and this
                                                 call will be forwarded from remote UE perspective. */
  RIL_CALL_PROGRESS_INFO_REMOTE_AVAILABLE, /**<  MT call is alerting from remote UE perspective. */
};

enum RIL_MsimAdditionalInfoCode {
  RIL_MSIM_ADDITIONAL_INFO_NONE,
  RIL_MSIM_ADDITIONAL_INFO_CONCURRENT_CALL_NOT_POSSIBLE,
};

/**
 * Codec used for call.
 */
enum RIL_Codec {
  RIL_CODEC_NONE,
  RIL_CODEC_QCELP13K,
  RIL_CODEC_EVRC,
  RIL_CODEC_EVRC_B,
  RIL_CODEC_EVRC_WB,
  RIL_CODEC_EVRC_NW,
  RIL_CODEC_AMR_NB,
  RIL_CODEC_AMR_WB,
  RIL_CODEC_GSM_EFR,
  RIL_CODEC_GSM_FR,
  RIL_CODEC_GSM_HR,
  RIL_CODEC_G711U,
  RIL_CODEC_G723,
  RIL_CODEC_G711A,
  RIL_CODEC_G722,
  RIL_CODEC_G711AB,
  RIL_CODEC_G729,
  RIL_CODEC_EVS_NB,
  RIL_CODEC_EVS_WB,
  RIL_CODEC_EVS_SWB,
  RIL_CODEC_EVS_FB,
};

/**
 * Computed audio quality received based on codec and bitrate.
 */
enum RIL_ComputedAudioQuality {
  RIL_COMPUTED_AUDIO_QUALITY_NONE,
  RIL_COMPUTED_AUDIO_QUALITY_NO_HD,
  RIL_COMPUTED_AUDIO_QUALITY_HD,
  RIL_COMPUTED_AUDIO_QUALITY_HD_PLUS,
};

/**
 * Handover type
 */
enum RIL_HandoverType {
  RIL_HO_START,
  RIL_HO_COMPLETE_SUCCESS,
  RIL_HO_COMPLETE_FAIL,
  RIL_HO_CANCEL,
  RIL_HO_NOT_TRIGGERED,
  RIL_HO_NOT_TRIGGERED_MOBILE_DATA_OFF,
  RIL_HO_INVALID,
  RIL_HO_UNKNOWN,
};

/**
 * Handover extra type
 */
enum RIL_HoExtraType {
  RIL_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL,
  RIL_HO_XT_TYPE_INVALID,
};

/**
 * Service class status
 */
enum RIL_ServiceClassStatus {
  RIL_SERVICE_CLASS_STATUS_DISABLED,
  RIL_SERVICE_CLASS_STATUS_ENABLED,
};

/**
 * Service class provision status
 */
enum RIL_ServiceClassProvisionStatus {
  RIL_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED,
  RIL_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED,
};

/**
 * Supplementary service operation type
 */
enum RIL_SuppSvcOperationType {
  RIL_SUPP_OPERATION_ACTIVATE,
  RIL_SUPP_OPERATION_DEACTIVATE,
  RIL_SUPP_OPERATION_QUERY,
  RIL_SUPP_OPERATION_REGISTER,
  RIL_SUPP_OPERATION_ERASURE,
};

/**
 * Facility type
 */
enum RIL_FacilityType {
  RIL_FACILITY_CLIP,    /* Calling Line Identification Presentation */
  RIL_FACILITY_COLP,    /* Connected Line Identification Presentation */
  RIL_FACILITY_BAOC,    /* Bar All Outgoing Calls */
  RIL_FACILITY_BAOIC,   /* Bar All Outgoing International Calls */
  RIL_FACILITY_BAOICxH, /* Bar all Outgoing International Calls except those
                         * directed to home PLMN country
                         */
  RIL_FACILITY_BAIC,    /* Bar All Incoming Calls */
  RIL_FACILITY_BAICr,   /* Bar All Incoming Calls when Roaming outside
                         * the home PLMN country
                         */
  RIL_FACILITY_BA_ALL,  /* Bar All incoming & outgoing Calls */
  RIL_FACILITY_BA_MO,   /* Bar All Outgoing Calls */
  RIL_FACILITY_BA_MT,   /* Bar All Incming Calls */
  RIL_FACILITY_BS_MT,   /* Bar Incoming Calls from specific DN */
  RIL_FACILITY_BAICa,   /* Bar All Incoming Calls from Anonymous numbers*/
};

/**
 * Ringback tone operation
 */
enum RIL_ToneOperation {
  RIL_TONE_OP_INVALID, /**< Invalid operation */
  RIL_TONE_OP_STOP,    /**< stop play ringback tone. */
  RIL_TONE_OP_START,   /**< start play ringback tone. */
};

/**
 * ECT types
 */
enum RIL_EctType {
  RIL_ECT_TYPE_UNKNOWN,
  RIL_ECT_TYPE_BLIND_TRANSFER,
  RIL_ECT_TYPE_ASSURED_TRANSFER,
  RIL_ECT_TYPE_CONSULTATIVE_TRANSFER,
};

/**
 * IP Presentation
 */
enum RIL_IpPresentation {
  RIL_IP_PRESENTATION_NUM_ALLOWED = 0,
  RIL_IP_PRESENTATION_NUM_RESTRICTED = 1,
  RIL_IP_PRESENTATION_NUM_DEFAULT = 2,
  RIL_IP_PRESENTATION_INVALID = 3,
};

/**
 * IMS Call type
 */
enum RIL_CallType {
  RIL_CALL_TYPE_UNKNOWN = 0,      /** Unknown */
  RIL_CALL_TYPE_VOICE = 1,        /** Voice only call */
  RIL_CALL_TYPE_VT_TX = 2,        /** PS Video telephony call: one way TX video,
                                          two way audio */
  RIL_CALL_TYPE_VT_RX = 3,        /** Video telephony call: one way RX video,
                                          two way audio */
  RIL_CALL_TYPE_VT = 4,           /** Video telephony call: two way video,
                                          two way audio */
  RIL_CALL_TYPE_VT_NODIR = 5,     /** Video telephony call: no direction
                                          two way audio
                                          intermediate state in a video call till
                                          video link is setup */
  RIL_CALL_TYPE_SMS = 6,          /** SMS */
  RIL_CALL_TYPE_UT = 7,           /** UT  - Supplementary services */
  RIL_CALL_TYPE_USSD = 8,         /** USSD */
  RIL_CALL_TYPE_CALLCOMPOSER = 9, /** CALLCOMPOSER */
  RIL_CALL_TYPE_DC = 10,          /** DATA CHANNEL */
};

/**
 * RTT mode
 */
enum RIL_RttMode {
  RIL_RTT_DISABLED = 0, /**< Disabled */
  RIL_RTT_FULL = 1,     /**< Enabled */
};

/**
 * Multi Identity Registration status
 */
enum RIL_MultiIdentityRegistrationStatus {
  RIL_STATUS_UNKNOWN = 0,
  RIL_STATUS_DISABLE = 1,
  RIL_STATUS_ENABLE = 2,
};

/**
 * Multi Identity Line type
 */
enum RIL_MultiIdentityLineType {
  RIL_LINE_TYPE_UNKNOWN = 0,
  RIL_LINE_TYPE_PRIMARY = 1,
  RIL_LINE_TYPE_SECONDARY = 2,
};

/**
 * Call modify causes
 */
enum RIL_CallModifyFailCause {
  RIL_CALL_MODIFY_FAIL_CAUSE_UNKNOWN,
  RIL_CALL_MODIFY_FAIL_CAUSE_SUCCESS,
  RIL_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE,
  RIL_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE,
  RIL_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED,
  RIL_CALL_MODIFY_FAIL_CAUSE_CANCELLED, /* Cancelled */
  RIL_CALL_MODIFY_FAIL_CAUSE_UNUSED,
  RIL_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER,
  RIL_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE,
  RIL_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED,
  RIL_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED,
  RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED,
  RIL_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED,
  RIL_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION,
};

/**
 * TTY Mode types
 */
enum RIL_TtyModeType {
  RIL_TTY_MODE_OFF,
  RIL_TTY_MODE_FULL,
  RIL_TTY_MODE_HCO,
  RIL_TTY_MODE_VCO,
  RIL_TTY_MODE_INVALID,
};

/**
 * Conference call state
 */
enum RIL_ConferenceCallState {
  RIL_CONF_CALL_STATE_UNKNOWN,
  RIL_CONF_CALL_STATE_FOREGROUND, /* Conference call is in forground */
  RIL_CONF_CALL_STATE_BACKGROUND, /* Conference call is in backgroudn */
  RIL_CONF_CALL_STATE_RINGING,    /* Conference call is in ringing state */
};

/**
 * VOICE info type
 */
enum RIL_VoiceInfoType {
  RIL_VOICE_INFO_SILENT, /* Voice Info silent is sent when remote party is silent on a RTT call */
  RIL_VOICE_INFO_SPEECH, /* Voice Info speech is sent when remote party starts speaking on a RTT call */
};

/**
 * VoWIFI call quality
 */
enum RIL_VowifiCallQuality {
  RIL_CALL_QUALITY_UNKNOWN,
  RIL_CALL_QUALITY_NONE,
  RIL_CALL_QUALITY_EXCELLENT,
  RIL_CALL_QUALITY_FAIR,
  RIL_CALL_QUALITY_BAD,
};

/**
 * Conference Participant Status
 */
struct RIL_ParticipantStatusInfo {
  uint32_t callId;
  RIL_ConfParticipantOperation operation;
  uint32_t sipStatus;
  char* participantUri;
  uint8_t hasIsEct;
  uint8_t isEct;
};

/**
 * Deflect call info
 */
struct RIL_DeflectCallInfo {
  uint32_t connIndex;
  char* number;
};

/**
 * Call Forward timer info
 */
struct RIL_CallFwdTimerInfo {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
  uint32_t timezone;
};

/**
 * Call Forward info
 */
struct RIL_CallForwardParams {
  uint32_t status; /*
                    * For RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
                    * status 1 = active, 0 = not active
                    *
                    * For RIL_REQUEST_SET_CALL_FORWARD:
                    * status is:
                    * 0 = disable
                    * 1 = enable
                    * 2 = interrogate
                    * 3 = registeration
                    * 4 = erasure
                    */

  uint32_t reason;       /* from TS 27.007 7.11 "reason" */
  uint32_t serviceClass; /* From 27.007 +CCFC/+CLCK "class".
                            0 means user doesn't input class */
  uint32_t toa;          /* "type" from TS 27.007 7.11 */
  char* number;          /* "number" from TS 27.007 7.11. May be NULL */
  uint32_t timeSeconds;  /* for CF no reply only */
  RIL_CallFwdTimerInfo* callFwdTimerStart;
  RIL_CallFwdTimerInfo* callFwdTimerEnd;
};

/**
 * Query Call Forward Status info
 */
struct RIL_QueryCallForwardStatusInfo {
  size_t callForwardInfoLen;
  RIL_CallForwardParams* callForwardInfo;
  RIL_SipErrorInfo* errorDetails;
};

/**
 * Set call forward status
 */
struct RIL_SetCallForwardStatus {
  uint32_t reason;
  uint8_t status;
  RIL_SipErrorInfo* errorDetails;
};

/**
 * Set call forward status info
 */
struct RIL_SetCallForwardStatusInfo {
  size_t setCallForwardStatusLen;
  RIL_SetCallForwardStatus* setCallForwardStatus;
  RIL_SipErrorInfo* errorDetails;
};

/**
 * Call waiting settings.
 */
struct RIL_CallWaitingSettings {
  uint8_t enabled;
  uint32_t serviceClass;
};

/**
 * Query call waiting response data
 */
struct RIL_QueryCallWaitingResponse {
  RIL_CallWaitingSettings callWaitingSettings;
  RIL_IMS_SipErrorInfo sipError;
};

/**
 * Call Composer location information
 */
struct RIL_CallComposerLocation {
  double radius;    /** Radios; 0 for point location, and -1 if location is not set */
  double latitude;  /** Latitude */
  double longitude; /** Longitude */
};

/**
 * Call Composer information
 */
struct RIL_CallComposerInfo {
  RIL_CallComposerPriority priority;
  char* subject;
  RIL_CallComposerLocation location;
  char* imageUrl;
  char* organization;
};

/**
 * ECNAM Information.
 */
struct RIL_EcnamInfo {
  char* name;
  char* iconUrl;
  char* infoUrl;
  char* cardUrl;
};

/**
 * Auto call rejection information.
 */
struct RIL_AutoCallRejectionInfo {
  RIL_CallType callType;
  RIL_IMS_CallFailCause cause;  // TODO use RIL CallFailCause??
  uint16_t sipErrorCode;
  char* number;
  RIL_VerstatVerificationStatus verificationStatus;
  RIL_CallComposerInfo* ccInfo;
  RIL_EcnamInfo* ecnamInfo;
};

/**
 * Pre alaterting call information.
 */
struct RIL_PreAlertingCallInfo {
  uint32_t callId;
  RIL_CallComposerInfo* ccInfo;
  RIL_EcnamInfo* ecnamInfo;
};

/**
 * Request data for switch waiting or holding and active.
 */
struct RIL_SwitchWaitingOrHoldingAndActive {
  uint8_t hasCallType;
  RIL_CallType callType;
};

/**
 * Request data for hangup
 */
struct RIL_Hangup {
  int32_t connIndex;
  char* connUri;
  uint8_t hasRejectCause;
  RIL_IMS_CallFailCause rejectCause;  // TODO use RIL CallFailCause??
};

/**
 * Call Failcause response data
 */
struct RIL_CallFailCauseResponse {
  RIL_IMS_CallFailCause failCause;  // TODO use RIL CallFailCause??
  uint32_t extendedFailCause;
  char* networkErrorString;
  RIL_IMS_SipErrorInfo* errorDetails;
};

/**
 * Verification status info.
 */
struct RIL_VerstatInfo {
  uint8_t canMarkUnwantedCall; /** Indicates whether the network supports the unwanted call feature.
                                  This value is obtained from the network.  When this value is
                                  enabled, the user gets the option on the UI to reject or end the
                                  received call by selecting the unwanted call option. */
  RIL_VerificationStatus verificationStatus; /** Validation status of the received call. */
};

/**
 * Call Progress Info
 */
struct RIL_CallProgressInfo {
  RIL_CallProgressInfoType type;
  uint16_t reasonCode;
  char* reasonText;
};

/**
 * MultiSIM additional call info
 */
struct RIL_MsimAdditionalCallInfo {
  RIL_MsimAdditionalInfoCode additionalCode;
};

/**
 * Audio quality consits of codec and computed audio quality.
 */
struct RIL_CallAudioQuality {
  RIL_Codec codec;
  RIL_ComputedAudioQuality computedAudioQuality;
};

/**
 * Call Info details
 */
struct RIL_CallInfo {
  RIL_CallState callState;
  uint32_t index;
  uint32_t toa;
  uint8_t isMpty;
  uint8_t isMt;
  uint8_t als;
  uint8_t isVoice;
  uint8_t isVoicePrivacy;
  char* number;
  uint32_t numberPresentation;
  char* name;
  uint32_t namePresentation;
  char* redirNum;
  uint32_t redirNumPresentation;
  RIL_CallType callType;
  RIL_CallDomain callDomain;
  uint32_t callSubState;
  uint8_t isEncrypted;
  uint8_t isCalledPartyRinging;
  uint8_t isVideoConfSupported;
  char* historyInfo;
  uint32_t mediaId;
  RIL_CallModifiedCause causeCode;
  RIL_IMS_RttMode rttMode;
  char* sipAlternateUri;
  size_t localAbilityLen;
  RIL_IMS_ServiceStatusInfo* localAbility;
  size_t peerAbilityLen;
  RIL_IMS_ServiceStatusInfo* peerAbility;
  RIL_CallFailCauseResponse* callFailCauseResponse;
  char* terminatingNumber;
  uint8_t isSecondary;
  RIL_VerstatInfo* verstatInfo;
  RIL_UUS_Info* uusInfo;
  char* displayText;
  char* additionalCallInfo;
  char* childNumber;
  uint32_t emergencyServiceCategory;
  RIL_TirMode tirMode;
  RIL_CallProgressInfo* callProgInfo;
  RIL_MsimAdditionalCallInfo* msimAdditionalCallInfo;
  RIL_CallAudioQuality* audioQuality;
  uint32_t modemCallId;
};

/**
 * Query clip response data.
 */
struct RIL_ClipInfo {
  RIL_ClipStatus clipStatus;
  RIL_IMS_SipErrorInfo* errorDetails;
};

/**
 * Get clir response data.
 */
struct RIL_ClirInfo {
  uint32_t action;
  uint32_t presentation;
};

/**
 * Handover extra information
 */
struct RIL_HoExtra {
  RIL_HoExtraType type;
  char* extraInfo;
};

/**
 * Handover information received as part of handover indication.
 */
struct RIL_HandoverInfo {
  RIL_HandoverType type;
  RIL_RadioTechnology srcTech;
  RIL_RadioTechnology targetTech;
  RIL_HoExtra* hoExtra;
  char* errorCode;
  char* errorMessage;
};

/**
 * Supp service information received as part of RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION.
 */
struct RIL_SuppSvcNotificationInfo {
  uint8_t notificationType; /*
                             * 0 = MO intermediate result code
                             * 1 = MT unsolicited result code
                             */
  uint32_t code;            /* "code1" for MO
                             * "code2" for MT. */
  uint32_t index;           /* CUG index. */
  uint32_t type;            /* "type" (MT only). */
  char* number;             /* "number"
                             * (MT only, may be NULL). */
  uint32_t connId;
  char* historyInfo;
  uint8_t hasHoldTone;
  uint8_t holdTone;
};

/**
 * Call barring numbers information.
 */
struct RIL_CallBarringNumbersInfo {
  RIL_ServiceClassStatus status;
  char* number;
};

/**
 * List of call barring numbers information.
 */
struct RIL_CallBarringNumbersListInfo {
  uint32_t serviceClass;
  size_t callBarringNumbersInfoLen;
  RIL_CallBarringNumbersInfo* callBarringNumbersInfo;
};

/**
 * Used to request supplementary service request.
 */
struct RIL_SuppSvcRequest {
  RIL_SuppSvcOperationType operationType;
  RIL_FacilityType facilityType;
  RIL_CallBarringNumbersListInfo* callBarringNumbersListInfo;
  char* password;
};

/**
 * Response to activate/deactivate/query the supplementary service.
 */
struct RIL_SuppSvcResponse {
  RIL_ServiceClassStatus status;
  RIL_ServiceClassProvisionStatus provisionStatus;
  RIL_FacilityType facilityType;
  size_t callBarringNumbersListInfoLen;
  RIL_CallBarringNumbersListInfo* callBarringNumbersListInfo;
  RIL_IMS_SipErrorInfo* errorDetails;
  uint8_t isPasswordRequired;
};

/**
 * Call forward data to handle multiple call forward parameters.
 */
struct RIL_CallForwardData {
  uint32_t numValidIndexes; /* This gives the number of valid values in cfInfo.
                       For example if voice is forwarded to one number and data
                       is forwarded to a different one then numValidIndexes will be
                       2 indicating total number of valid values in cfInfo.
                       Similarly if all the services are forwarded to the same
                       number then the value of numValidIndexes will be 1. */

  RIL_CallForwardParams cfInfo[NUM_SERVICE_CLASSES]; /* This is the response data
                                                      for SS request to query call
                                                      forward status. see
                                                      RIL_REQUEST_QUERY_CALL_FORWARD_STATUS */
};

/**
 * STK cc supplementary service information received as part of RIL_CALL_UNSOL_ON_SS
 */
struct RIL_StkCcSsInfo {
  RIL_SsServiceType serviceType;
  RIL_SsRequestType requestType;
  RIL_SsTeleserviceType teleserviceType;
  RIL_SuppServiceClass serviceClass;
  RIL_Errno result;

  union {
    uint32_t ssInfo[SS_INFO_MAX]; /* This is the response data for most of the SS GET/SET
                                RIL requests. E.g. RIL_CALL_REQUEST_GET_CLIR returns
                                two ints, so first two values of ssInfo[] will be
                                used for response if serviceType is SS_CLIR and
                                requestType is SS_INTERROGATION */

    RIL_CallForwardData cfData;
  };
};

/**
 * ECT data used to request RIL_CALL_REQUEST_EXPLICIT_CALL_TRANSFER.
 * Optional for CS calls and mandatory for IMS calls.
 */
struct RIL_ExplicitCallTransfer {
  uint8_t isValid;  // 1 indicates the below parameters are valid
  uint32_t callId;
  RIL_EctType ectType;
  char* targetAddress;
  uint32_t targetCallId; // Pass max value of uint32_t if not valid
};

/**
 * COLR Info
 */
struct RIL_ColrInfo {
  RIL_ServiceClassStatus status;
  RIL_ServiceClassProvisionStatus provisionStatus;
  RIL_IpPresentation presentation;
  RIL_IMS_SipErrorInfo* errorDetails;
};

/**
 * Call Details.
 */
struct RIL_CallDetails {
  RIL_CallType callType;
  RIL_CallDomain callDomain;
  RIL_RttMode rttMode;
  uint32_t extrasLength;
  char** extras;
};

/**
 * Multi identify line info
 */
struct RIL_MultiIdentityLineInfo {
  char* msisdn;
  RIL_MultiIdentityRegistrationStatus registrationStatus;
  RIL_MultiIdentityLineType lineType;
};

/**
 * Dial request parameters
 */
struct RIL_DialParams {
  char* address;
  uint32_t clirMode;     /* (same as 'n' paremeter in TS 27.007 7.7 "+CLIR"
                          * clir == 0 on "use subscription default value"
                          * clir == 1 on "CLIR invocation" (restrict CLI presentation)
                          * clir == 2 on "CLIR suppression" (allow CLI presentation)
                          */
  RIL_UUS_Info* uusInfo; /* NULL or Pointer to User-User Signaling Information */
  RIL_IpPresentation presentation;
  bool hasCallDetails;
  RIL_CallDetails callDetails;
  bool hasIsConferenceUri;
  bool isConferenceUri;
  bool hasIsCallPull;
  bool isCallPull;
  bool hasIsEncrypted;
  bool isEncrypted;
  RIL_MultiIdentityLineInfo multiLineInfo;
};

/**
 * Answer request parameters
 */
struct RIL_Answer {
  uint8_t isValid;  // 1 indicates the below parameters are valid
  RIL_CallType callType;
  RIL_IpPresentation presentation;
  RIL_RttMode rttMode;
};

/**
 * Call Modify Info
 */
struct RIL_CallModifyInfo {
  uint32_t callId;
  RIL_CallType callType;
  RIL_CallDomain callDomain;
  uint8_t hasRttMode;
  RIL_RttMode rttMode;
  uint8_t hasCallModifyFailCause;
  RIL_CallModifyFailCause callModifyFailCause;
};

/**
 * TTY notify info.
 */
struct RIL_TtyNotifyInfo {
  RIL_TtyModeType mode;
};

/**
 * Conference status info.
 */
struct RIL_RefreshConferenceInfo {
  RIL_ConferenceCallState conferenceCallState;
  size_t confInfoUriLength;
  uint8_t* confInfoUri;
};

/* RIL CALL Requests */
#define RIL_CALL_REQUEST_BASE 20000

/**
 * RIL_CALL_REQUEST_DIAL
 *
 * Initiate voice call
 *
 * "data" is RIL_DialParams
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_DIAL 20001

/**
 * RIL_CALL_REQUEST_ANSWER
 *
 * Answer incoming call
 *
 * "data" is RIL_Answer
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_ANSWER 20002

/**
 * RIL_CALL_REQUEST_HANGUP
 *
 * Hang up a specific line.
 *
 * "data" is RIL_Hangup
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_STATE
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_CALL_ID
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_HANGUP 20003

/**
 * RIL_CALL_REQUEST_CONFERENCE
 *
 * Conference holding and active (like AT+CHLD=3)

 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  INVALID_CALL_ID
 *  INVALID_ARGUMENTS
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_CONFERENCE 20004

/**
 * RIL_CALL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE
 *
 * Request the radio's system selection module to exit emergency
 * callback mode.  RIL will not respond with SUCCESS until the modem has
 * completely exited from Emergency Callback Mode.
 *
 * "data" is NULL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *  GENERIC_FAILURE
 *
 */
#define RIL_CALL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE 20005

/**
 * RIL_CALL_REQUEST_EXPLICIT_CALL_TRANSFER
 *
 * For transfering the call to a third party.
 * The call can be transferred to third party (Transfer target) by
 * passing the transfer target address (blind or assured transfer)
 * or by passing the call id of the already established call with
 * transfer target (consultative transfer).
 * In success case, the call/calls will be disconnected.
 *
 * "data" is RIL_ExplicitCallTransfer
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_EXPLICIT_CALL_TRANSFER 20006

/**
 * RIL_CALL_REQUEST_DTMF
 *
 * Send a DTMF tone
 *
 * If the implementation is currently playing a tone requested via
 * RIL_CALL_REQUEST_DTMF_START, that tone should be cancelled and the new tone
 * should be played instead
 *
 * "data" is a char containing a single character with one of 12 values: 0-9,*,#
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_CALL_REQUEST_DTMF_STOP, RIL_CALL_REQUEST_DTMF_START
 *
 */
#define RIL_CALL_REQUEST_DTMF 20007

/**
 * RIL_CALL_REQUEST_DTMF_START
 *
 * Start playing a DTMF tone. Continue playing DTMF tone until
 * RIL_CALL_REQUEST_DTMF_STOP is received
 *
 * If a RIL_CALL_REQUEST_DTMF_START is received while a tone is currently playing,
 * it should cancel the previous tone and play the new one.
 *
 * "data" is a char containing a single character with one of 12 values: 0-9,*,#
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_CALL_REQUEST_DTMF, RIL_CALL_REQUEST_DTMF_STOP
 */
#define RIL_CALL_REQUEST_DTMF_START 20008

/**
 * RIL_CALL_REQUEST_DTMF_STOP
 *
 * Stop playing a currently playing DTMF tone.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_CALL_REQUEST_DTMF, RIL_CALL_REQUEST_DTMF_START
 */
#define RIL_CALL_REQUEST_DTMF_STOP 20009

/**
 * RIL_CALL_REQUEST_MODIFY_CALL_INITIATE
 *
 * Request to upgrade/downgrade existing IMS calls
 *
 * "data" is RIL_CallModifyInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_MODIFY_CALL_INITIATE 20010

/**
 * RIL_CALL_REQUEST_MODIFY_CALL_CONFIRM
 *
 * Request to accept the call upgrade of existing IMS calls
 *
 * "data" is RIL_CallModifyInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_MODIFY_CALL_CONFIRM 20011

/**
 * RIL_CALL_REQUEST_QUERY_CLIP
 *
 * Queries the status of the CLIP supplementary service
 * (for MMI code "*#30#")
 *
 * "data" is NULL
 * "response" is RIL_ClipInfo
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_QUERY_CLIP 20012

/**
 * RIL_CALL_REQUEST_GET_CLIR
 *
 * Gets current CLIR status
 *
 * "data" is NULL
 * "response" is RIL_ClirInfo
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_GET_CLIR 20013

/**
 * RIL_CALL_REQUEST_SET_CLIR
 *
 * "data" is int *
 * ((int *)data)[0] is "n" parameter from TS 27.007 7.7
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_CALL_REQUEST_SET_CLIR 20014

/**
 * RIL_CALL_REQUEST_QUERY_CALL_FORWARD_STATUS
 *
 * Requests the call forward settings
 *
 * "data[0]" is reason
 * "data[1]" is serviceClass
 *
 * "response" is RIL_QueryCallForwardStatusInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_QUERY_CALL_FORWARD_STATUS 20015

/**
 * RIL_CALL_REQUEST_SET_CALL_FORWARD
 *
 * Configure call forward rule
 *
 * "data" is RIL_CallForwardParams
 *
 * "response" is RIL_SetCallForwardStatusInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_SET_CALL_FORWARD 20016

/**
 * RIL_CALL_REQUEST_QUERY_CALL_WAITING
 *
 * Query current call waiting state
 *
 * "data" is uint32_t, indicates the service class
 *
 * "response" is RIL_QueryCallWaitingResponse
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_QUERY_CALL_WAITING 20017

/**
 * RIL_CALL_REQUEST_SET_CALL_WAITING
 *
 * Configure current call waiting state
 *
 * "data" is RIL_CallWaitingSettings
 *
 * "response" is RIL_IMS_SipErrorInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_SET_CALL_WAITING 20018

/**
 * RIL_CALL_REQUEST_SET_SUPP_SVC_NOTIFICATION
 *
 * Enables/disables supplementary service related notifications
 * from the network.
 *
 * Notifications are reported via RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION.
 *
 * "data" is boolean, TRUE for enable and FALSE for disable
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  E_GENERIC_FAILURE
 *  E_FDN_CHECK_FAILURE
 *  SIM_BUSY
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_SET_SUPP_SVC_NOTIFICATION 20019

/**
 * RIL_CALL_REQUEST_ADD_PARTICIPANT
 *
 * Request to add a participant to a call (to the current active call or to the held call if there
 * is no active calls).
 *
 * "data" is "char *" containing address
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_ADD_PARTICIPANT 20020

/**
 * RIL_CALL_REQUEST_SUPP_SVC_STATUS
 *
 * Request to activate/deactivate/query the supplementary service
 * This is currently being used for Call Barring, CLIP, COLP
 *
 * "data" is RIL_SuppSvcRequest
 * "response" is RIL_SuppSvcResponse
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_CALL_REQUEST_SUPP_SVC_STATUS 20021

/**
 * RIL_CALL_REQUEST_DEFLECT_CALL
 *
 * Deflect the call to specific number mentioned by user
 *
 * "data" is RIL_DeflectCallInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_DEFLECT_CALL 20022

/**
 * RIL_CALL_REQUEST_GET_COLR
 *
 * Queries COLR info
 *
 * "data" is NULL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_GET_COLR 20023

/**
 * RIL_CALL_REQUEST_SET_COLR
 *
 * Set COLR
 *
 * "data" is RIL_ColrInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_SET_COLR 20024

/**
 * RIL_CALL_REQUEST_HOLD
 *
 * Request to put the active call on HOLD
 *
 * "data" is call id
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_HOLD 20025

/**
 * RIL_CALL_REQUEST_RESUME
 *
 * Request to resume a HELD call.
 *
 * "data" is call id
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_RESUME 20026

/**
 * RIL_CALL_REQUEST_SEND_RTT_MSG
 * MsgType: REQUEST
 *
 * Request to send the RTT message
 *
 * "data" is message char*
 *
 * "response" is null
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_SEND_RTT_MSG 20027

/**
 * RIL_CALL_REQUEST_CANCEL_MODIFY_CALL
 *
 * Cancel call modify request
 *
 * "data" is uint32_t
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_CANCEL_MODIFY_CALL 20028

/**
 * RIL_CALL_REQUEST_HANGUP_WAITING_OR_BACKGROUND
 *
 * Hang up waiting or held.
 *
 * This request will return falure (RIL_E_INVALID_STATE) in case of DSDA mode.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_HANGUP_WAITING_OR_BACKGROUND 20029

/**
 * RIL_CALL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
 *
 * Hang up waiting or held (like AT+CHLD=1)
 *
 * This request will return falure (RIL_E_INVALID_STATE) in case of DSDA mode.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND 20030

/**
 * RIL_CALL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE
 *
 * Switch waiting or holding call and active call (like AT+CHLD=2)
 *
 * State transitions should be is follows:
 *
 * If call 1 is waiting and call 2 is active, then if this re
 *
 *   BEFORE                               AFTER
 * Call 1   Call 2                 Call 1       Call 2
 * ACTIVE   HOLDING                HOLDING     ACTIVE
 * ACTIVE   WAITING                HOLDING     ACTIVE
 * HOLDING  WAITING                HOLDING     ACTIVE
 * ACTIVE   IDLE                   HOLDING     IDLE
 * IDLE     IDLE                   IDLE        IDLE
 *
 * This request will return falure (RIL_E_INVALID_STATE) in case of DSDA mode.
 *
 * "data" is RIL_SwitchWaitingOrHoldingAndActive
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE 20031

/**
 * RIL_CALL_REQUEST_UDUB
 *
 * Send UDUB (user determined used busy) to ringing or waiting call.
 * Applicable only for CS calls., and not applicable for IMS calls.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_RESOURCES
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_UDUB 20032

/**
 * RIL_CALL_REQUEST_LAST_CALL_FAIL_CAUSE
 *
 * Requests the failure cause code for the most recently terminated call
 *
 * "data" is NULL
 * "response" is a const RIL_LastCallFailCauseInfo *
 * RIL_LastCallFailCauseInfo contains LastCallFailCause and vendor cause.
 * The vendor cause code must be used for debugging purpose only.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_LAST_CALL_FAIL_CAUSE 20033

/**
 * RIL_CALL_REQUEST_GET_CURRENT_CALLS
 *
 * Requests current call list
 *
 * "data" is NULL
 *
 * "response" must be a "const RIL_Call **"
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *      (request will be made again in a few hundred msec)
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_GET_CURRENT_CALLS 20034

/**
 * RIL_CALL_REQUEST_SET_TTY_MODE
 *
 * Request to set the TTY mode
 *
 * "data" is int *
 * ((int *)data)[0] is == 0 for TTY off
 * ((int *)data)[0] is == 1 for TTY Full
 * ((int *)data)[0] is == 2 for TTY HCO (hearing carryover)
 * ((int *)data)[0] is == 3 for TTY VCO (voice carryover)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_SET_TTY_MODE 20035

/**
 * RIL_CALL_REQUEST_QUERY_TTY_MODE
 *
 * Request the setting of TTY mode
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is == 0 for TTY off
 * ((int *)response)[0] is == 1 for TTY Full
 * ((int *)response)[0] is == 2 for TTY HCO (hearing carryover)
 * ((int *)response)[0] is == 3 for TTY VCO (voice carryover)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_QUERY_TTY_MODE 20036

/**
 * RIL_CALL_REQUEST_SEND_UI_TTY_MODE
 *
 * Set UI TTY mode
 *
 * "data" is "RIL_TtyNotifyInfo"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_CALL_REQUEST_SEND_UI_TTY_MODE 20037

/**
 * RIL_CALL_REQUEST_SEND_USSD
 *
 * Send a USSD message
 *
 * If a USSD session already exists, the message should be sent in the
 * context of that session. Otherwise, a new session should be created.
 *
 * The network reply should be reported via RIL_CALL_UNSOL_ON_USSD
 *
 * Only one USSD session may exist at a time, and the session is assumed
 * to exist until:
 *   a) The client invokes RIL_CALL_REQUEST_CANCEL_USSD
 *   b) The implementation sends a RIL_CALL_UNSOL_ON_USSD with a type code
 *      of "0" (USSD-Notify/no further action) or "2" (session terminated)
 *
 * "data" is a const char * containing the USSD request in UTF-8 format
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  FDN_CHECK_FAILURE
 *  USSD_MODIFIED_TO_DIAL
 *  USSD_MODIFIED_TO_SS
 *  USSD_MODIFIED_TO_USSD
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  ABORTED
 *  SYSTEM_ERR
 *  INVALID_STATE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_CALL_REQUEST_CANCEL_USSD, RIL_CALL_UNSOL_ON_USSD
 */
#define RIL_CALL_REQUEST_SEND_USSD 20038
/**
 * RIL_CALL_REQUEST_CANCEL_USSD
 *
 * Cancel the current USSD session if one exists
 *
 * "data" is null
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_STATE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_CANCEL_USSD 20039

/**
 * RIL_CALL_REQUEST_SEPARATE_CONNECTION
 *
 * Separate a party from a multiparty call placing the multiparty call
 * (less the specified party) on hold and leaving the specified party
 * as the only other member of the current (active) call
 *
 * Like AT+CHLD=2x
 *
 * See TS 22.084 1.3.8.2 (iii)
 * TS 22.030 6.5.5 "Entering "2X followed by send"
 * TS 27.007 "AT+CHLD=2x"
 *
 * "data" is an int *
 * (int *)data)[0] contains Connection index (value of 'x' in CHLD above) "response" is NULL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_ARGUMENTS
 *  INVALID_STATE
 *  NO_RESOURCES
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  INVALID_STATE
 *  OPERATION_NOT_ALLOWED
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_SEPARATE_CONNECTION 20040

/**
 * RIL_CALL_REQUEST_CHANGE_BARRING_PASSWORD
 *
 * Change call barring facility password
 *
 * "data" is const char **
 *
 * ((const char **)data)[0] = facility string code from TS 27.007 7.4
 * (eg "AO" for BAOC)
 * ((const char **)data)[1] = old password
 * ((const char **)data)[2] = new password
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_CALL_REQUEST_CHANGE_BARRING_PASSWORD 20041

/* RIL CALL Unsolicited Indications */
#define RIL_CALL_UNSOL_BASE 25000

/**
 * RIL_CALL_UNSOL_CALL_STATE_CHANGED
 *
 * Indicate when call state has changed
 *
 * "data" is RIL_CallInfo*
 */
#define RIL_CALL_UNSOL_CALL_STATE_CHANGED 25001

/**
 * RIL_CALL_UNSOL_CALL_RING
 *
 * Ring indication for an incoming call (eg, RING or CRING event).
 * There must be at least one RIL_UNSOL_CALL_RING at the beginning
 * of a call and sending multiple is optional. If the system property
 * ro.telephony.call_ring.multiple is false then the upper layers
 * will generate the multiple events internally. Otherwise the vendor
 * ril must generate multiple RIL_UNSOL_CALL_RING if
 * ro.telephony.call_ring.multiple is true or if it is absent.
 *
 * The rate of these events is controlled by ro.telephony.call_ring.delay
 * and has a default value of 3000 (3 seconds) if absent.
 *
 * "data" is null for GSM / IMS
 * "data" is const RIL_CDMA_SignalInfoRecord * if CDMA
 */
#define RIL_CALL_UNSOL_CALL_RING 25002

/**
 * RIL_CALL_UNSOL_RINGBACK_TONE
 *
 * Indicates that nework doesn't have in-band information, need to play out-band tone.
 *
 * "data" is RIL_ToneOperation
 */
#define RIL_CALL_UNSOL_RINGBACK_TONE 25003

/**
 * RIL_CALL_UNSOL_ON_USSD
 *
 * Called when a new USSD message is received.
 *
 * "data" is a const RIL_UssdModeType *
 *
 * The USSD session is assumed to persist if the type code is "1", otherwise
 * the current session (if any) is assumed to have terminated.
 *
 * ((const char **)data)[1] points to a message string if applicable, which
 * should always be in UTF-8.
 */
#define RIL_CALL_UNSOL_ON_USSD 25004

/**
 * RIL_CALL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
 *
 * Indicates that the radio system selection module has
 * autonomously entered emergency callback mode.
 *
 * "data" is null
 */
#define RIL_CALL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE 25005

/**
 * RIL_CALL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE
 *
 * Called when Emergency Callback Mode Ends
 *
 * Indicates that the radio system selection module has
 * proactively exited emergency callback mode.
 *
 * "data" is null
 */
#define RIL_CALL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE 25006

/**
 * RIL_CALL_UNSOL_MODIFY_CALL
 *
 * Unsol message to notify that an upgrade of a call was triggered from the remote party
 *
 * "data" is RIL_CallModifyInfo
 */
#define RIL_CALL_UNSOL_MODIFY_CALL 25007

/**
 * RIL_CALL_UNSOL_HANDOVER
 *
 * Unsol message to notify that a call has been handed over between technologies
 *
 * "data" is RIL_HandoverInfo
 */
#define RIL_CALL_UNSOL_HANDOVER 25008

/**
 * RIL_CALL_UNSOL_REFRESH_CONF_INFO
 *
 * Unsol message to notify conference status updates
 *
 * "data" is RIL_RefreshConferenceInfo
 */
#define RIL_CALL_UNSOL_REFRESH_CONF_INFO 25009

/**
 * RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION
 *
 * Reports supplementary service related notification from the network.
 *
 * "data" is RIL_SuppSvcNotificationInfo
 */
#define RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION 25010

#define RIL_CALL_UNSOL_TTY_NOTIFICATION 25011

/**
 * RIL_CALL_UNSOL_REFRESH_VICE_INFO
 *
 * Unsol message to notify VICE info.
 *
 * "data" is uint8_t *
 */
#define RIL_CALL_UNSOL_REFRESH_VICE_INFO 25012

/**
 * UNSOL_VOWIFI_CALL_QUALITY
 *
 * Notification for VoWiFi call quality information
 *
 * "response" is message RIL_VowifiCallQuality
 */
#define RIL_CALL_UNSOL_VOWIFI_CALL_QUALITY 25013

/*
 * RIL_CALL_UNSOL_PARTICIPANT_STATUS_INFO
 *
 * Notification for participant status information
 *
 * "data" is RIL_ParticipantStatusInfo
 */
#define RIL_CALL_UNSOL_PARTICIPANT_STATUS_INFO 25014

/**
 * RIL_CALL_UNSOL_RTT_MSG_RECEIVED
 *
 * Notification for received RTT message
 *
 * 'response' is message RttMessage (string)
 */
#define RIL_CALL_UNSOL_RTT_MSG_RECEIVED 25015

/*
 * RIL_CALL_UNSOL_ON_SS
 *
 * Called when SS response is received when DIAL/USSD/SS is changed to SS by
 * call control.
 *
 * "data" is const RIL_StkCcSsInfo
 *
 */
#define RIL_CALL_UNSOL_ON_SS 25016

#define RIL_CALL_UNSOL_AUTO_CALL_REJECTION_IND 25017

/**
 * RIL_CALL_UNSOL_VOICE_INFO
 *
 * Sends updates for the RTT voice info which indicates whether there is speech or silence
 * from remote user
 *
 * "data" is enum RIL_VoiceInfoType
 */
#define RIL_CALL_UNSOL_VOICE_INFO 25018

/**
 * RIL_CALL_UNSOL_INCOMING_CALL_AUTO_REJECTED
 *
 * Notifies clients about a MT call being automatically rejected by the modem.
 *
 * "data" is RIL_AutoCallRejectionInfo
 */
#define RIL_CALL_UNSOL_INCOMING_CALL_AUTO_REJECTED 25019

/**
 * RIL_CALL_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE
 *
 * Indicates to notify pre alerting call information like callcomposer, ecnam.
 *
 * "data" is RIL_PreAlertingCallInfo
 */
#define RIL_CALL_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE 25020

#endif /*__RIL_CALL_H_*/
