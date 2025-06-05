/******************************************************************************
#  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/**
 * @file RilApiSession.hpp
 * @brief Contains the RIL Client APIs.
 */

#pragma once

#include <memory>
#include <mutex>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <telephony/ril_call.h>
#include <Status.hpp>
#include <RequestManager.hpp>
#include <telephony/RIL_ModuleList_i.hpp>

/**
 * Enum RilInstance
 */
enum class RilInstance {
    FIRST = 0,    /**< First instance/subscription */
    SECOND = 1,   /**< Second instance/subscription */
    MIN = FIRST,  /**< Min value */
    MAX = SECOND, /**< Min value */
};

// TODO: Define response and indication callbacks in different namespaces

/**
 * @brief Generic Callback function
 *
 * @retval void
 */
typedef std::function<void()> VoidIndicationHandler;

/**
 * @brief Generic Callback function notified char strings
 *
 * @param data   data in char string format.
 * @retval void
 */
typedef std::function<void(const char *)> CharStarIndicationHandler;
typedef std::function<void(int)> IntIndicationHandler;
typedef std::function<void(RIL_Errno)> EmptyCallback;

/**
 * @brief Callback function for @ref dial request.
 *
 * @param errorCode   - Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DialCallback;

/**
 * @brief Callback function for @ref getCellInfo request.
 *
 * @param errorCode           Error code @ref RIL_Errno
 * @param cellInfoList        Cell info list
 *                            Refer @ref RIL_CellInfo_v12
 * @param cellInfoListLen     Length of Cell info list
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_CellInfo_v12[], size_t)> CellInfoCallback;

/**
 * @brief Indication callback function for @ref registerSignalStrengthIndicationHandler
 *
 * @param signalStrength        Signal strength
 *                              Refer @ref RIL_SignalStrength
 * @retval void
 */
typedef std::function<void(const RIL_SignalStrength&)> SignalStrengthIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCellInfoIndicationHandler
 *
 * @param cellInfoList        Cell info list
 *                            Refer @ref RIL_CellInfo_v12
 * @param cellInfoListLen     Length of Cell info list
 */
typedef std::function<void(const RIL_CellInfo_v12[], size_t)> CellInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaNewSmsIndicationHandler.
 * The callback function will be invoked when new CDMA SMS is received
 *
 * @param message             CDMA SMS message
 *                            Refer @ref RIL_CDMA_SMS_Message
 */
typedef std::function<void(const RIL_CDMA_SMS_Message&)> CdmaNewSmsIndicationHandler;

/**
 * @brief Callback function
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> VoidResponseHandler;

/**
 * @brief Callback function
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param data            Data
 * @param dataLen         Length of the data
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler;

/**
 * @brief Callback function for @ref getVoiceRegStatus request.
 *
 * @param errorCode        Error code @ref RIL_Errno
 * @param voiceRegState    Voice registration state.
 *                         Refer @ref RIL_VoiceRegistrationStateResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_VoiceRegistrationStateResponse)> GetVoiceRegCallback;

/**
 * @brief Callback function for @ref getDataRegStatus request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param dataRegState    Data registration state.
 *                        Refer @ref RIL_DataRegistrationStateResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_DataRegistrationStateResponse)> GetDataRegCallback;

/**
 * @brief Callback function for @ref setPreferredNetworkType request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetNetworkTypeCallback;

/**
 * @brief Callback function for @ref getPreferredNetworkType request.
 *
 * @param errorCode          Error code @ref RIL_Errno
 * @param prefNetworkType    Preferred network type.  Refer @ref RIL_PreferredNetworkType
 * @retval void
 */
typedef std::function<void(RIL_Errno, int)> GetNetworkTypeCallback;

/**
 * @brief Callback function for @ref getCurrentCalls request.
 *
 * @param errorCode         Error code @ref RIL_Errno
 * @param numCalls          Length of the RIL_Call array
 * @param callList          RIL_Call array
 *                          Refer @ref RIL_Call
 * @retval void
 */
typedef std::function<void(RIL_Errno, const size_t numCalls, const RIL_Call **)> GetCurrentCallsCallback;

/**
 * @brief Callback function for @ref answer request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> AnswerCallback;

/**
 * @brief Callback function for @ref conference request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ConferenceCallback;

/**
 * @brief Callback function for @ref switchWaitingOrHoldingAndActive request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SwitchWaitingOrHoldingAndActiveCallback;

/**
 * @brief Callback function for @ref udub request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> UdubCallback;

/**
 * @brief Callback function for @ref hangupWaitingOrBackground request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> HangupWaitingOrBackgroundCallback;

/**
 * @brief Callback function for @ref hangupForegroundResumeBackground request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> HangupForegroundResumeBackgroundCallback;

/**
 * @brief Callback function for @ref separateConnection request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SeparateConnectionCallback;

/**
 * @brief Callback function for @ref queryAvailableBandMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param bandsList       Array of RIL_RadioBandMode's. int[0] is the size of array
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int *)> QueryAvailableBandModeCallback;

/**
 * @brief Callback function for @ref getUsageSetting request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param settingMode     int value representing  RIL_UsageSettingMode
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int)> GetUsageSettingCallback;

/**
 * @brief Callback function for @ref getSignalStrength request.
 *
 * @param errorCode         Error code @ref RIL_Errno
 * @param signalStrendth    Signal strength.
 *                          Refer @ref RIL_SignalStrength
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_SignalStrength *)> GetSignalStrengthCallback;

/**
 * @brief Callback function for @ref radioPower request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> RadioPowerCallback;
typedef std::function<void(RIL_Errno)> AllowDataCallback;

/**
 * @brief Callback function for @ref enableSim request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> EnableSimCallback;

/**
 * @brief Callback function for @ref getEnableSimStatus request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param status          SIM enablement status
 * @retval void
 */
typedef std::function<void(RIL_Errno, int)> GetEnableSimStatusCallback;

/**
 * @brief Callback function for @ref setLocationUpdates request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetLocationUpdatesCallback;

/**
 * @brief Callback function for @ref getActivityInfo request.
 *
 * @param errorCode            Error code @ref RIL_Errno
 * @param activityStatInfo     Activity stat info.
 *                             Refer @ref RIL_ActivityStatsInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_ActivityStatsInfo)> GetActivityInfoCallback;

/**
 * @brief Callback function for @ref VoiceRadioTech request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param radioTec        Voice radio tech.
 *                        Refer @ref RIL_RadioTechnology
 * @retval void
 */
typedef std::function<void(RIL_Errno, int)> VoiceRadioTechCallback;

/**
 * @brief Callback function for @ref deviceIdentity request.
 *
 * @param errorCode     Error code @ref RIL_Errno
 * @param response      Array of strings.
 *                      <br>response[0] is IMEI if GSM subscription is available.
 *                      <br>response[1] is IMEISV if GSM subscription is available.
 *                      <br>response[2] is ESN if CDMA subscription is available.
 *                      <br>response[3] is MEID if CDMA subscription is available.
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char **)> DeviceIdentityCallback;

/**
 * @brief Callback function for @ref operatorName request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param response        Response containing strings
 *                        <br>response[0] is long alpha ONS or EONS or NULL if unregistered
 *                        <br>response[1] is short alpha ONS or EONS or NULL if unregistered
 *                        <br>response[2] is 5 or 6 digit numeric code (MCC + MNC) or NULL if unregistered
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char **)> OperatorNameCallback;

/**
 * @brief Callback function for @ref QueryNetworkSelectionMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param mode            <br>0 for automatic selection.
 *                        <br>1 for manual selection.
 * @retval void
 */
typedef std::function<void(RIL_Errno, int)> QueryNetworkSelectionModeCallback;

/**
 * @brief Callback function for @ref getSystemSelectionChannels request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param sysSelChannels  System selection channels
 *                        <br>@ref RIL_SysSelChannels
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_SysSelChannels&)> GetSystemSelectionChannelsCallback;

/**
 * @brief Callback function for @ref setNetworkSelectionAutomatic request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetNetworkSelectionAutomaticCallback;

/**
 * @brief Callback function for @ref setNetworkSelectionManual request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetNetworkSelectionManualCallback;

/**
 * @brief Callback function for @ref basebandVersion request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param baseband        Baseband version in string format
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char *)> BasebandVersionCallback;

/**
 * @brief Callback function for @ref setRadioCapability request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetRadioCapabilityCallback;

/**
 * @brief Callback function for @ref getRadioCapability request.
 *
 * @param errorCode          Error code @ref RIL_Errno
 * @param radioCapability    Radio capability.
 *                           Refer @ref RIL_RadioCapability&
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_RadioCapability&)> GetRadioCapabilityCallback;

/**
 * @brief Indication callback function for @ref registerRadioCapabilityChangedIndHandler.
 * The callback function will be notified when @ref setRadioCapability completes.
 *
 * @param capability          Phone radio capability
 *                            Refer @ref RIL_RadioCapability
 */
typedef std::function<void(const RIL_RadioCapability&)> RadioCapabilityIndHandler;

/**
 * @brief Callback function for @ref shutDown request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ShutDownCallback;

/**
 * @brief Callback function for @ref exitEmergencyCbMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ExitEmergencyCbModeCallback;

/**
 * @brief Callback function for @ref setBandMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetBandModeCallback;

/**
 * @brief Callback function for @ref setUsageSetting request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetUsageSettingCallback;

/**
 * @brief Callback function for @ref hangup request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> HangupCallback;

/**
 * @brief Callback function for @ref sendUssd request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SendUssdCallback;

/**
 * @brief Callback function for @ref cancelUssd request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> CancelUssdCallback;

/**
 * @brief Callback function for @ref dtmf request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DtmfCallback;

/**
 * @brief Callback function for @ref dtmfStart request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DtmfStartCallback;

/**
 * @brief Callback function for @ref dtmfStop request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DtmfStopCallback;

/**
 * @brief Callback function for @ref lastCallFailCause request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param failCause       Failure reason of the recently ended call.
 *                        Refer @ref RIL_LastCallFailCauseInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_LastCallFailCauseInfo&)> LastCallFailCauseCallback;

/**
 * @brief Callback function for @ref sendSms and @ref sendCdmaSms requests.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param smsResponse     Send sms response.  Refer @ref RIL_SMS_Response
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_SMS_Response&)> SendSmsCallback;

/**
 * @brief Callback function for @ref ackSms request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> AckSmsCallback;

/**
 * @brief Callback function for @ref getSmscAddress request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param smscAddress     SMSC address
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char* addr)> GetSmscAddressCallback;

/**
 * @brief Callback function for @ref setSmscAddress request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetSmscAddressCallback;

/**
 * @brief Callback function for @ref queryCallForwardStatus request.
 *
 * @param errorCode          Error code @ref RIL_Errno
 * @param numCallFwdInfos    Number of call forward info entries
 * @param callFwdInfoList    Array of RIL_CallForwardInfo.
 *                           Refer @ref RIL_CallForwardInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const size_t numCallFwdInfos, const RIL_CallForwardInfo**)>
    QueryCallForwardStatusCallback;

/**
 * @brief Callback function for @ref setCallForward request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetCallForwardStatusCallback;

/**
 * @brief Callback function for @ref queryCallWaiting request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param enabled         Enable status
 * @param serviceClass    Service class
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int enabled, const int serviceClass)>
    QueryCallWaitingCallback;

/**
 * @brief Callback function for @ref setCallWaiting request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetCallCallWaitingCallback;

/**
 * @brief Callback function for @ref changeBarringPassword request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ChangeBarringPasswordCallback;

/**
 * @brief Callback function for @ref queryClip request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param status          CLIP status
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int status)> QueryClipCallback;

/**
 * @brief Callback function for @ref setSuppSvcNotification request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetSuppSvcNotificationCallback;

/**
 * @brief Indication callback function for @ref registerIncoming3GppSmsIndicationHandler.
 * The callback function will be invoked when new SMS is received
 *
 * @param message             SMS PDU
 * @param message_len         Length of the SMS PDU
 */
typedef std::function<void(const uint8_t[], size_t)> Incoming3GppSmsIndicationHandler;

/**
 * @brief Indication callback function for @ref registerEnableSimStatusIndicationHandler.
 * The callback function will be invoked to notify the sim enablement status
 *
 * @param status             Status
 */
typedef std::function<void(const bool)> EnableSimStatusIndicationHandler;

/**
 * @brief Indication callback function for @ref
 * registerCdmaSubscriptionSourceChangedIndicationHandler.
 * The callback function will be invoked when CDMA subscription source changed.
 *
 * @param source              Cdma subscription source
 *                            Refer @ref RIL_CdmaSubscriptionSource.
 */
typedef std::function<void(const RIL_CdmaSubscriptionSource)> CdmaSubscriptionSourceChangedIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaNewSmsIndicationHandler.
 * The callback function will be invoked when new Broadcast SMS is received.
 *
 * @param message             CDMA SMS message
 *                            Refer @ref RIL_CDMA_SMS_Message
 */
typedef std::function<void(const std::vector<uint8_t>&)> BroadcastSmsIndicationHandler;

/**
 * @brief Indication callback function for @ref registerModemRestartIndicationHandler.
 * The callback function will be invoked when there is a modem reset.
 *
 * @param reason              Reason text
 */
typedef std::function<void(const std::string&)> ModemRestartIndicationHandler;

/**
 * @brief Indication callback function for @ref registerRilServerReadyIndicationHandler.
 * The callback function will be invoked when the RIL server is ready.
 *
 * @param version             Version
 */
typedef std::function<void(int)> RilServerReadyIndicationHandler;

/**
 * @brief Indication callback function for @ref registerNetworksScanIndicationHandler.
 * The callback function will be invoked to notify the incremental result for the network scan
 * which is started by @ref startNetworkScan, notifies the results, status, or errors.
 *
 * @param scanResult          Network scan result
 *                            Refer @ref RIL_NetworkScanResult
 */
typedef std::function<void(const RIL_NetworkScanResult&)> NetworkScanIndicationHandler;

/**
 * @brief Indication callback function for @ref registerEmergencyListIndicationHandler.
 * The callback function will be invoked to notify the current list of emergency numbers.
 *
 * @param emergencyList       Emergency number details
 *                            Refer @ref RIL_EmergencyList
 */
typedef std::function<void(const RIL_EmergencyList&)> EmergencyListIndicationHandler;

/**
 * @brief Callback function for @ref writeSmsToSim request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno, int32_t)> WriteSmsToSimCallback;

/**
 * @brief Callback function for @ref deleteSmsOnSim request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DeleteSmsOnSimCallback;

/**
 * @brief Callback function for @ref reportSmsMemoryStatus request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ReportSmsMemoryStatusCallback;

/**
 * @brief Callback function for @ref setCdmaSubscriptionSource request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetCdmaSubscriptionSourceCallback;

/**
 * @brief Callback function for @ref getCdmaSubscriptionSource request.
 *
 * @param errorCode                 Error code @ref RIL_Errno
 * @param cdmaSubscriptionSource    CDMA subscription source
 *                                     Refer @ref RIL_CdmaSubscriptionSource
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_CdmaSubscriptionSource)> GetCdmaSubscriptionSourceCallback;

/**
 * @brief Callback function for @ref setCdmaRoamingPreference request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetCdmaRoamingPreferenceCallback;

/**
 * @brief Callback function for @ref getCdmaRoamingPreference request.
 *
 * @param errorCode                Error code @ref RIL_Errno
 * @param cdmaRoamingPreference    CDMA Roaming preference
 *                                 Refer @ref RIL_CdmaRoamingPreference
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_CdmaRoamingPreference)> GetCdmaRoamingPreferenceCallback;

/**
 * @brief Callback function for @ref GsmGetBroadcastSmsConfig request.
 *
 * @param errorCode           Error code @ref RIL_Errno
 * @param OnfigInfo           GSM Broadcast Sms Config Info list.
 *                            Refer @ref RIL_GSM_BroadcastSmsConfigInfo
 * @param configInfoLength    Size of the GSM Broadcast Sms Config Info list
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_GSM_BroadcastSmsConfigInfo *, size_t)> GsmGetBroadcastSmsConfigCallback;

/**
 * @brief Callback function for @ref setSignalStrengthReportingCriteria request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetSignalStrengthReportingCriteriaCallback;

/**
 * @brief Callback function for @ref oemhookRaw request.
 *
 * @param errorCode         Error code @ref RIL_Errno
 * @param responseData      Response data
 * @param reponseDataLen    Response data length
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char *, size_t)> OemhookRawCallback;

/**
 * @brief Indication callback function for @ref registerOemhookIndicationHandler.
 * The callback function will be invoked when there is a oemhook indication.
 *
 * @param data             Oemhook data
 * @param dataLen          Length of oemhook data.
 * @retval void
 */
typedef std::function<void(const char *, size_t)> OemhookIndicationHandler;

// IMS Requests
/**
 * Callback function for imsGetRegistrationState
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              IMS registration info.  Refer @ref RIL_IMS_Registration
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_Registration&)> ImsGetRegistrationStateCallback;

/**
 * Callback function for imsQueryServiceStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param dataLen           Length of IMS Service status info list.
 * @param data              IMS Service status info list.  Refer @ref RIL_IMS_ServiceStatusInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const size_t, const RIL_IMS_ServiceStatusInfo**)>
    ImsQueryServiceStatusCallback;

/**
 * Callback function for imsGetImsSubConfig
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              IMS Sub config info.  Refer @ref RIL_IMS_SubConfigInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SubConfigInfo&)> ImsGetImsSubConfigCallback;

/**
 * Callback function for imsRegistrationChange
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsRegistrationChangeCallback;

/**
 * Callback function for imsSetServiceStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsSetServiceStatusCallback;

/**
 * Callback function for imsDial
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsDialCallback;

/**
 * Callback function for imsAnswer
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsAnswerCallback;

/**
 * Callback function for imsHangup
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsHangupCallback;

/**
 * Callback function for imsModifyCallInitiate
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsModifyCallInitiateCallback;

/**
 * Callback function for imsModifyCallConfirm
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsModifyCallConfirmCallback;

/**
 * Callback function for imsCancelModifyCall
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsCancelModifyCallCallback;

/**
 * Callback function for imsAddParticipant
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details  Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsAddParticipantCallback;

/**
 * Callback function for imsSetImsConfig
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              IMS Config info.  Refer @ref RIL_IMS_ConfigInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ConfigInfo&)> ImsSetImsConfigCallback;

/**
 * Callback function for imsGetImsConfig
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              IMS Config info.  Refer @ref RIL_IMS_ConfigInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ConfigInfo&)> ImsGetImsConfigCallback;

/**
 * Callback function for imsQueryCallForwardStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Call forward status details.
 *                          Refer @ref RIL_IMS_QueryCallForwardStatusInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryCallForwardStatusInfo&)>
    ImsQueryCallForwardStatusCallback;

/**
 * Callback function for imsSetCallForwardStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Set call forward status info.
 *                          Refer @ref RIL_IMS_SetCallForwardStatusInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SetCallForwardStatusInfo&)>
    ImsSetCallForwardStatusCallback;

/**
 * Callback function for imsExplicitCallTransfer
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsExplicitCallTransferCallback;

/**
 * Callback function for imsGetClir
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              CLIR info.  Refer @ref RIL_IMS_ClirInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ClirInfo&)> ImsGetClirCallback;

/**
 * Callback function for imsQueryClip
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              CLIP info.  Refer @ref RIL_IMS_ClipInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ClipInfo&)> ImsQueryClipCallback;

/**
 * Callback function for imsSetSuppSvcNotification
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Service class status.  Refer @ref RIL_IMS_ServiceClassStatus
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ServiceClassStatus&)>
    ImsSetSuppSvcNotificationCallback;

/**
 * Callback function for imsSetClir
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsSetClirCallback;

/**
 * Callback function for imsDeflectCall
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsDeflectCallCallback;

/**
 * Callback function for imsSendUiTtyMode
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsSendUiTtyModeCallback;

/**
 * Callback function for imsSuppSvcStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Status of supplementary service request.
 *                          Refer @ref RIL_IMS_SuppSvcResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SuppSvcResponse&)> ImsSuppSvcStatusCallback;

/**
 * Callback function for imsGetColr
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              COLR info.
 *                          Refer @ref RIL_IMS_SuppSvcResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_ColrInfo&)> ImsGetColrCallback;

/**
 * Callback function for getColr
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              COLR info.
 *                          Refer @ref RIL_ColrInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_ColrInfo&)> VoiceGetColrCallback;


/**
 * Callback function for imsSetColr
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details.
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsSetColrCallback;

/**
 * Callback function for imsRegisterMultiIdentityLines
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsRegisterMultiIdentityLinesCallback;

/**
 * Callback function for imsQueryVirtualLineInfo
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Virtual lines info.  Refer @ref RIL_IMS_QueryVirtualLineInfoResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryVirtualLineInfoResponse&)>
        ImsQueryVirtualLineInfoCallback;

/**
 * Callback function for imsSendSms
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Send SMS response information.  Refer @ref RIL_IMS_SendSmsResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SendSmsResponse&)> ImsSendSmsCallback;

/**
 * Callback function for imsAckSms
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsAckSmsCallback;

/**
 * Callback function for imsGetSmsFormat
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              SMS format.  Refer @ref RIL_IMS_SmsFormat
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_IMS_SmsFormat)> ImsGetSmsFormatCallback;

/**
 * Callback function for imsSetCallWaiting
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details.
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsSetCallWaitingCallback;

/**
 * Callback function for imsQueryCallWaiting
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Call waiting information
 *                          Refer @ref RIL_IMS_QueryCallWaitingResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryCallWaitingResponse&)> ImsQueryCallWaitingCallback;


/**
 * @brief Callback function for @ref ackCdmaSms request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> AckCdmaSmsCallback;

/**
 * @brief Callback function for @ref getCdmaSmsBroadcastConfig request.
 *
 * @param errorCode            Error code @ref RIL_Errno
 * @param configInfoList       Array of @ref RIL_CDMA_BroadcastSmsConfigInfo
 * @param configInfoListLen    Size of the array
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_CDMA_BroadcastSmsConfigInfo[], size_t)> CdmaGetBroadcastSmsConfigCallback;

/**
 * @brief Callback function for @ref setCdmaSmsBroadcastConfig request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> CdmaSetBroadcastSmsConfigCallback;

/**
 * @brief Callback function for @ref setCdmaSmsBroadcastActivation request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> CdmaSmsBroadcastActivationCallback;

/**
 * @brief Callback function for @ref writeCdmaSmsToRuim request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param recordNumber    recordNumber
 * @retval void
 */
typedef std::function<void(RIL_Errno, int32_t recordNumber)> WriteCdmaSmsToSimCallback;

/**
 * @brief Callback function for @ref deleteCdmaSmsOnRuim request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> DeleteCdmaSmsOnRuimCallback;

/**
 * @brief Indication callback function for @ref registerRingBackToneIndicationHandler.
 * The indication callback will be invoked to notify if the UE need to play the ringback tone.
 *
 * @param playRingbackTone    Ringback tone status.
 *                            0 for stop play ringback tone.
 *                            1 for start play ringback tone.
 */
typedef std::function<void(const int&)> RingBackToneIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsNetworkStateChangeIndicationHandler.
 * The callback function will be called when IMS registration state has changed.
 * To get IMS registration state and IMS SMS format, callee needs to invoke the
 * @ref getImsRegState
 */
typedef std::function<void()> ImsNetworkStateChangeIndicationHandler;

/**
 * @brief Indication callback function for @ref registerVoiceNetworkStateIndicationHandler.
 * The callback function will be called when the voice network state changed.
 */
typedef std::function<void()> VoiceNetworkStateIndicationHandler;

/**
 * @brief Indication callback function for @ref registerVoiceRadioTechIndicationHandler.
 * The callback function will be invoked to indicates that voice technology has changed.
 * And will contains new radio technology.
 *
 * @param voiceRat            Voice radio technology
 *                            Refer @ref RIL_RadioTechnology
 */
typedef std::function<void(int)> VoiceRadioTechIndicationHandler;

/**
 * @brief Indication callback function for @ref registerNitzTimeIndicationHandler.
 * The callback function will be invoked when radio has received a NITZ time message
 *
 * @param nitz                NITZ time message in the form "yy/mm/dd,hh:mm:ss(+/-)tz,dt"
 */
typedef std::function<void(const char*)> NitzTimeIndicationHandler;

/**
 * @brief Indication callback function for @ref registerRadioStateIndicationHandler.
 * The callback function will be invoked when the radio state changes.
 *
 * @param state               Radio state.
 *                            Refer @ref RIL_RadioState
 */
typedef std::function<void(RIL_RadioState)> RadioStateIndicationHandler;

/**
 * @brief Indication callback function for @ref registerRestrictedStateChangedIndicationHandler.
 * The callback function will be invoked when the restricted state changes (eg, for Domain
 * Specific Access Control).
 *
 * @param state               Contains a bitmask of RIL_RESTRICTED_STATE_* values.
 */
typedef std::function<void(const int&)> RestrictedStateChangedIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCallRingIndicationHandler.
 * The callback function will be invoked to notify ring indication for an incoming call.
 *
 * @param record              CDMA signal info record.  This parameter is only applicable in case of
 *                            3GPP2.
 *                            Refer @ref RIL_CDMA_SignalInfoRecord
 */
typedef std::function<void(const RIL_CDMA_SignalInfoRecord *)> CallRingIndicationHandler;

/**
 * @brief Indication callback function for @ref registerOnUssdIndicationHandler.
 * The callback function will be invoked when a new USSD message is received.
 *
 * @param mode                USSD Mode.  Refer @ref RIL_UssdModeType
 * @param message             USSD message
 */
typedef std::function<void(const char& mode, const std::string& message)> OnUssdIndicationHandler;

/**
 * @brief Indication callback function for @ref registerSuppSvcNotificationIndicationHandler.
 * The callback function will be invoked to report supplementary service related
 * notification from the network.
 *
 * @param suppSvc             Supplementary service notification information.
 *                            Refer @ref RIL_SuppSvcNotification
 */
typedef std::function<void(const RIL_SuppSvcNotification& suppSvc)>
    SuppSvcNotificationIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaCallWaitingIndicationHandler.
 * The callback function will be invoked when CDMA radio receives a call waiting indication.
 *
 * @param callWaitingRecord   CDMA call waiting record
 *                            Refer @ref RIL_CDMA_CallWaiting_v6
 */
typedef std::function<void(const RIL_CDMA_CallWaiting_v6& callWaitingRecord)>
    CdmaCallWaitingIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaOtaProvisionStatusIndicationHandler.
 * The callback function will be invoked when CDMA radio receives an update of the progress of an
 * OTASP/OTAPA call.
 *
 * @param status              CDMA OTA provision status
 *                            Refer @ref RIL_CDMA_OTA_ProvisionStatus
 */
typedef std::function<void(const RIL_CDMA_OTA_ProvisionStatus& status)>
    CdmaOtaProvisionStatusIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaInfoRecIndicationHandler.
 * The callback function will be invoked when CDMA radio receives one or more info recs.
 *
 * @param records             CDMA info records
 *                            Refer @ref RIL_CDMA_InformationRecords
 */
typedef std::function<void(const RIL_CDMA_InformationRecords& records)> CdmaInfoRecIndicationHandler;

/**
 * @brief Indication callback function for @ref registerOnSupplementaryServiceIndicationHandler.
 * The callback function will be invokved when SS response is received when DIAL/USSD/SS
 * is changed to SS by call control.
 *
 * @param ss                  Supplementary service info
 *                            Refer @ref RIL_StkCcUnsolSsResponse
 * @retval void
 */
typedef std::function<void(const RIL_StkCcUnsolSsResponse& ss)> OnSupplementaryServiceIndicationHandler;

/**
 * @brief Indication callback function for @ref registerOnSupplementaryServiceIndicationHandler.
 * The callback function will be invokved when SS response is received when DIAL/USSD/SS
 * is changed to SS by call control.
 *
 * @param ss                  Supplementary service info
 *                            Refer @ref RIL_StkCcSsInfo
 * @retval void
 */
typedef std::function<void(const RIL_StkCcSsInfo& ss)> OnSupplementaryServiceIndicationHdlr;

/**
 * @brief Indication callback function for @ref registerSrvccStatusIndicationHandler.
 * The callback function will be invoked when Single Radio Voice Call Continuity(SRVCC)
 * progress state has changed
 *
 * @param state               SRVCC state
 *                            Refer @ref RIL_SrvccState
 * @retval void
 */
typedef std::function<void(const RIL_SrvccState& state)> SrvccStatusIndicationHandler;

/**
 * @brief Callback function for @ref explicitCallTransfer request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ExplicitCallTransferCallback;

/**
 * @brief Callback function for @ref setMute request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetMuteCallback;

/**
 * @brief Callback function for @ref getMute request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param mute            Mute status
 * @retval void
 */
typedef std::function<void(RIL_Errno, int)> GetMuteCallback;

/**
 * @brief Callback function for @ref setClir request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetClirCallback;

/**
 * @brief Callback function for @ref getClir request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param n               n parameter
 * @param m               m parameter
 * @retval void
 */
typedef std::function<void(RIL_Errno, int, int)> GetClirCallback;

/**
 * @brief Callback function for @ref imsSendRttMessage request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SendRttCallback;

/**
 * @brief Callback function for @ref imsSendGeolocationInfo request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> GeolocationInfoCallback;

/**
 * @brief Callback function for @ref setUnsolCellInfoListRate request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetUnsolCellInfoListRateCallback;

/**
 * @brief Indication callback function for @ref registerUnsolCallStateChangeIndicationHandler.
 * The callback function will be invoked  when there is a call state change.
 * Application need to call the @ref getCurrentCalls to get the currnet call list.
 */
typedef std::function<void()> CallStateChangeIndicationHandler;

/**
 * @brief Indication callback function for @ref registerEmergencyCbModeIndHandler.
 * The callback function will be invoked to indicates that the radio system selection
 * module has entered to or exited from emergency callback mode.
 */
typedef std::function<void(RIL_EmergencyCbMode)> EmergencyCbModeIndHandler;

/**
 * @brief Indication callback function for @ref registerEmergencyCallbackModeIndHandler.
 * The callback function will be invoked to indicates that the radio system selection
 * module has entered to or exited from emergency callback mode.
 */
typedef std::function<void(RIL_EmergencyCbMode)> EmergencyCallbackModeIndHandler;

/**
 * @brief Indication callback function for @ref registerUiccSubStatusIndHandler.
 * The callback function will be invoked to notify the subscription readiness.
 *
 * @param status              Subscription status.
 *                            <br>0 for Subscription Deactivated
 *                            <br>1 for Subscription Activated
 */
typedef std::function<void(int)> UiccSubStatusChangedHandler;

/**
 * @brief Indication callback function for @ref registerCdmaPrlChangedIndHandler.
 * The callback function willbe invoked when PRL (preferred roaming list) changes.
 *
 * @param version             PRL version
 */
typedef std::function<void(int)> CdmaPrlChangedHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolRegistrationStateIndicationHandler.
 * The callback function will be invoked when Single Radio Voice Call Continuity(SRVCC)
 * progress state has changed
 *
 * @param registration        IMS registration info.
 *                            Refer @ref RIL_IMS_Registration
 * @retval void
 */
typedef std::function<void(const RIL_IMS_Registration&)> ImsRegistrationStateIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolServiceStatusIndicationHandler.
 * The callback function will be invoked to notify the change in IMS service status.
 *
 * @param length              Length of IMS Service status info list.
 * @param status              IMS Service status info list.
 *                            Refer @ref RIL_IMS_ServiceStatusInfo
 * @retval void
 */
typedef std::function<void(const size_t, const RIL_IMS_ServiceStatusInfo**)>
    ImsServiceStatusIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolSubConfigChangeIndicationHandler.
 * The callback function will be invoked to notify the change in IMS subscription
 * configuration
 *
 * @param state               IM subscription configuration info.
 *                            Refer @ref RIL_IMS_SubConfigInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_SubConfigInfo&)> ImsSubConfigChangeIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsRttMessageIndicationHandler.
 * The callback function will be invoked when a RTT message received.
 *
 * @param message             RTT message
 * @retval void
 */
typedef std::function<void(const std::string&)> ImsRttMessageIndicationHandler;

/**
 * @brief Indication callback function for @ref registerRttMessageIndicationHandler.
 * The callback function will be invoked when a RTT message received.
 *
 * @param message             RTT message
 * @retval void
 */
typedef std::function<void(const std::string&)> RttMessageIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsGeolocationIndicationHandler.
 * The callback function will be invoked when the lower layers request for the geo location
 * address information. The 'request' will contain the latitude (double), longitude (double)
 * and application is expected to resolve this to geo location address information and set
 * back to the lower layers using @ref imsSendGeolocationInfo.
 *
 * @param latitude            latitude
 * @param longitude           longitude
 * @retval void
 */
typedef std::function<void(double, double)> ImsGeolocationIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsVowifiQualityIndicationHandler.
 * The callback function will be invoked to notify the VoWiFi call quality information.
 *
 * @param quality             VoWIFI call quality.
 *                            Refer @ref RIL_IMS_VowifiCallQuality
 * @retval void
 */
typedef std::function<void(const RIL_IMS_VowifiCallQuality &)> ImsVowifiQualityIndicationHandler;

/**
 * @brief Indication callback function for @ref registerVowifiQualityIndicationHandler.
 * The callback function will be invoked to notify the VoWiFi call quality information.
 *
 * @param quality             VoWIFI call quality.
 *                            Refer @ref RIL_VowifiCallQuality
 * @retval void
 */
typedef std::function<void(const RIL_VowifiCallQuality &)> VowifiQualityIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolCallRingIndicationHandler.
 * The callback function will be invoked to notify ring indication for an incoming call on IMS.
 *
 * @retval void
 */
typedef std::function<void()> ImsCallRingIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolRingBackToneIndicationHandler.
 * The indication callback will be invoked to notify if the UE need to play the ringback tone.
 *
 * @param toneOperation       Ringback tone status.
 *                            Refer @ref RIL_IMS_ToneOperation
 * @retval void
 */
typedef std::function<void(const RIL_IMS_ToneOperation&)> ImsRingBackToneIndicationHandler;

/**
 * @brief Indication callback function for @ref registerRingBackToneIndicationHdlr.
 * The indication callback will be invoked to notify if the UE need to play the ringback tone.
 *
 * @param toneOperation       Ringback tone status.
 *                            Refer @ref RIL_ToneOperation
 * @retval void
 */
typedef std::function<void(const RIL_ToneOperation&)> RingBackToneIndicationHdlr;

/**
 * @brief Indication callback function for @ref registerImsUnsolCallStateChangedIndicationHandler.
 * The callback function will be invoked when call state has changed
 *
 * @param callInfos           Call info list
 *                            Refer @ref RIL_IMS_CallInfo
 * @param count               Length of call info list.
 * @retval void
 */
typedef std::function<void(RIL_IMS_CallInfo*, size_t count)> ImsCallStateChangedIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolEnterEcbmIndicationHandler.
 * The callback function will be invoked to indicates that the radio system selection
 * module has entered to emergency callback mode.
 *
 * @retval void
 */
typedef std::function<void()> ImsEnterEcbmIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolExitEcbmIndicationHandler.
 * The callback function will be invoked to indicates that the radio system selection
 * module has exited from emergency callback mode.
 *
 * @retval void
 */
typedef std::function<void()> ImsExitEcbmIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolModifyCallIndicationHandler.
 * The callback function will be invoked to notify that an upgrade of a call was triggered
 * from the remote party
 *
 * @param callModifyInfo      Call modify info.
 *                            Refer @ref RIL_IMS_CallModifyInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_CallModifyInfo&)> ImsModifyCallIndicationHandler;

/**
 * @brief Indication callback function for @ref registerUnsolModifyCallIndicationHandler.
 * The callback function will be invoked to notify that an upgrade of a call was triggered
 * from the remote party
 *
 * @param callModifyInfo      Call modify info.
 *                            Refer @ref RIL_CallModifyInfo
 * @retval void
 */
typedef std::function<void(const RIL_CallModifyInfo&)> ModifyCallIndicationHandler;
/**
 * @brief Indication callback function for @ref registerImsUnsolMessageWaitingIndicationHandler.
 * The callback function will be invoked to notify clients about message waiting indication
 *
 * @param msgWaitingInfo      Message waiting info.
 *                            Refer @ref RIL_IMS_MessageWaitingInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_MessageWaitingInfo&)> ImsMessageWaitingIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolSuppSvcNotificationIndicationHandler.
 * The callback function will be invoked to notify supplementary service related
 * notification from the network.
 *
 * @param suppSvc             Supplementary service notification info.
 *                            Refer @ref RIL_IMS_SuppSvcNotification
 * @retval void
 */
typedef std::function<void(const RIL_IMS_SuppSvcNotification&)> ImsSuppSvcNotificationIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolOnSsIndicationHandler.
 * The callback function will be invoked when SS response is received when DIAL/USSD/SS is
 * changed to SS by call control.
 *
 * @param ssIndInfo           Supplementary service info.
 *                            Refer @ref RIL_IMS_StkCcUnsolSsResponse
 * @retval void
 */
typedef std::function<void(const RIL_IMS_StkCcUnsolSsResponse&)> ImsOnSsIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolVopsChangedIndicationHandler.
 * The callback function will be invoked to notify the clients about vops change indication
 *
 * @param state               VoPS state
 * @retval void
 */
typedef std::function<void(const bool)> ImsVopsChangedIndicationHandler;

/**
 * @brief Indication callback function for @ref
 * registerImsUnsolParticipantStatusInfoIndicationHandler.
 * The callback function will be invoked to notify the status of the participants in the
 * conference call.
 *
 * @param participantInfo     Participant status info.
 *                            Refer @ref RIL_IMS_ParticipantStatusInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_ParticipantStatusInfo&)>
    ImsParticipantStatusInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref
 * registerImsUnsolRegistrationBlockStatusIndicationHandler.
 * The callback function will be invoked to notify the permanent ims registration failure.
 * Permanent registration failure happens when modem has exhausted all its
 * retries. At this point modem will stop attempting to register until user
 * toggles the registration state or network reports favorable conditions for
 *
 * @param blockStatus         Registration block status.
 *                            Refer @ref RIL_IMS_RegistrationBlockStatus
 * @retval void
 */
typedef std::function<void(const RIL_IMS_RegistrationBlockStatus&)>
    ImsRegistrationBlockStatusIndicationHandler;

/**
 * @brief Indication callback function for @ref
 * registerImsUnsolWfcRoamingModeConfigSupportIndicationHandler.
 * The callback function will be invoked to notify if modem supports WFC roaming mode
 * configuration
 *
 * @param supported           supported flag
 * @retval void
 */
typedef std::function<void(const uint8_t&)> ImsWfcRoamingModeConfigSupportIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolUssdFailedIndicationHandler.
 * The callback function will be invoked to notify USSD failure indicaiton over IMS.
 *
 * @param ussdInfo            USSD info.
 *                            Refer @ref RIL_IMS_UssdInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_UssdInfo&)> ImsUssdFailedIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolVoiceInfoIndicationHandler.
 * The callback function will be invoked to sends updates for the RTT voice info which
 * indicates whether there is speech or silence from remote user
 *
 * @param voiceInfo           RTT voice info
 *                            Refer @ref RIL_IMS_VoiceInfoType
 * @retval void
 */
typedef std::function<void(const RIL_IMS_VoiceInfoType&)> ImsVoiceInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref registerUnsolVoiceInfoIndicationHandler.
 * The callback function will be invoked to sends updates for the RTT voice info which
 * indicates whether there is speech or silence from remote user
 *
 * @param voiceInfo           RTT voice info
 *                            Refer @ref RIL_VoiceInfoType
 * @retval void
 */
typedef std::function<void(const RIL_VoiceInfoType&)> VoiceInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolAutoCallRejectionIndicationHandler.
 * The callback function will be invoked to notify the auto call reject notifications.
 *
 * @param voiceInfo           RTT voice info
 *                            Refer @ref RIL_IMS_VoiceInfoType
 * @retval void
 */
typedef std::function<void(const RIL_IMS_AutoCallRejectionInfo&)>
    ImsVoiceAutoCallRejectionIndicationHandler;

/**
 * Callback function for imsRequestConference
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestConferenceCallback;

/**
 * Callback function for imsRequestHoldCall
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestHoldCallback;

/**
 * Callback function for imsRequestResumeCall
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestResumeCallback;

/**
 * SipErrorInfoResponseHandler is a generic callback
 * This can be used with any of the API which returns only SipErrorInfo
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> SipErrorInfoResponseHandler;

/**
 * Callback function for imsRequestExitEmergencyCallbackMode
 * @param errorCode         Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsExitECBMCallback;

/**
 * @brief Indication callback function for @ref registerImsIncomingSmsHandler.
 * The callback function will be invoked when new incoming SMS is recieved.
 *
 * @param message             Incoming SMS
 *                            Refer @ref RIL_IMS_IncomingSms
 * @retval void
 */
typedef std::function<void(const RIL_IMS_IncomingSms&)> ImsIncomingSmsHandler;

/**
 * @brief Indication callback function for @ref registerImsSmsStatusReportHandler.
 * The callback function will be invoked when new SMS status report is recieved.
 *
 * @param smsStatusReport     SMS status report.
 *                            Refer @ref RIL_IMS_SmsStatusReport
 * @retval void
 */
typedef std::function<void(const RIL_IMS_SmsStatusReport&)> ImsSmsStatusReportHandler;

/**
 * @brief Indication callback function for @ref registerImsViceInfoHandler.
 * The callback function will be invoked to notify VICE info indication.
 *
 * @param viceInfo            VICE info.
 * @retval void
 */
typedef std::function<void(const std::vector<uint8_t>&)> ImsViceInfoHandler;

/**
 * @brief Indication callback function for @ref registerViceInfoHandler.
 * The callback function will be invoked to notify VICE info indication.
 *
 * @param viceInfo            VICE info.
 * @retval void
 */
typedef std::function<void(const std::vector<uint8_t>&)> ViceInfoHandler;

/**
 * @brief Indication callback function for @ref registerImsTtyNotificationHandler.
 * The callback function will be invoked to notify information about TTY
 *
 * @param mode                TTY mode
 *                            Refer @ref RIL_IMS_TtyModeType
 * @retval void
 */
typedef std::function<void(RIL_IMS_TtyModeType)> ImsTtyNotificationHandler;

/**
 * @brief Indication callback function for @ref registerTtyNotificationHandler.
 * The callback function will be invoked to notify information about TTY
 *
 * @param mode                TTY mode
 *                            Refer @ref RIL_TtyModeType
 * @retval void
 */
typedef std::function<void(RIL_TtyModeType)> TtyNotificationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolHandoverIndicationHandler.
 * The callback function will be invoked to notify that a call has been handed over
 * between technologies
 *
 * @param handoverInfo        Handover info.
 *                            Refer @ref RIL_IMS_HandoverInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_HandoverInfo&)> ImsHandoverIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolRefreshConferenceInfo.
 * The callback function will be invoked to notify the conference status updates
 *
 * @param confInfo            Conference info.
 *                            Refer @ref RIL_IMS_RefreshConferenceInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_RefreshConferenceInfo&)>
    ImsRefreshConferenceInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref registerUnsolRefreshConferenceInfo.
 * The callback function will be invoked to notify the conference status updates
 *
 * @param confInfo            Conference info.
 *                            Refer @ref RIL_RefreshConferenceInfo
 * @retval void
 */
typedef std::function<void(const RIL_RefreshConferenceInfo&)>
    RefreshConferenceInfoIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolMultiIdentityRegistrationStatus.
 * The callback function will be invoked to notify the registration status of
 * Multi-line identity
 *
 * @param lineInfo            Multi idendity line info
 *                            Refer @ref RIL_IMS_MultiIdentityLineInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_MultiIdentityLineInfo*, size_t)>
    MultiIdentityRegistrationStatusHandler;

/**
 * @brief Indication callback function for @ref registerImsUnsolRetrievingGeoLocationDataStatus.
 * The callback function will be invoked to indicates geo location retrival status in modem.
 * Modem needs geo location to register IMS over standalone WIFI.
 *
 * @param status              Geo location retrival status
 *                            Refer @ref RIL_IMS_GeoLocationDataStatus
 * @retval void
 */
typedef std::function<void(const RIL_IMS_GeoLocationDataStatus&)>
    RetrievingGeoLocationDataStatusHandler;

/**
 * @brief Callback function for @ref imsQueryMultiSimVoiceCapability request.
 * The callback function will be invoked to indicates the multi sim capability of dual sims
 *
 * @param errorCode         Error code @ref RIL_Errno
 * @param voiceCapability   Voice capability on multi sim devices.
 *                            Refer @ref RIL_IMS_MultiSimVoiceCapability
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_MultiSimVoiceCapability&)>
    ImsQueryMultiSimVoiceCapabilityCallback;

/**
 * @brief Indication callback function for @ref registerImsMultiSimVoiceCapabilityChanged.
 * The callback function will be invoked to indicate change in voice capability on multi sim case.
 *
 * @param voiceCapability   Voice capability on multi sim devices.
 *                            Refer @ref RIL_IMS_MultiSimVoiceCapability
 * @retval void
 */
typedef std::function<void(const RIL_IMS_MultiSimVoiceCapability&)>
    ImsMultiSimVoiceCapabilityIndicationHandler;

/**
 * @brief Indication callback function for @ref registerImsPreAlertingCallInfoAvailable.
 * The callback function will be invoked to indicate pre alerting call info from modem.
 *
 * @param status              Pre alerting call info
 *                            Refer @ref RIL_IMS_PreAlertingCallInfo
 * @retval void
 */
typedef std::function<void(const RIL_IMS_PreAlertingCallInfo&)> ImsPreAlertingCallInfoAvailableHandler;

/**
 * @brief Callback function for @ref sendUssd request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsSendUssdCallback;

/**
 * @brief Callback function for @ref cancelUssd request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> ImsCancelUssdCallback;

/**
 * @brief Indication callback function for @ref registerImsOnUssdIndicationHandler.
 * The callback function will be invoked when a new USSD message is received.
 *
 * @param mode                USSD Mode.  Refer @ref RIL_UssdModeType
 * @param message             USSD message
 */
typedef std::function<void(const char& mode, const std::string& message)> ImsOnUssdIndicationHandler;

typedef std::function<void(RIL_Errno, const Status, const RIL_Data_Call_Response_v11*)> SetupDataCallCallback;
typedef std::function<void(RIL_Errno)> DeactivateDataCallCallback;
typedef std::function<void(const RIL_Data_Call_Response_v11[], size_t)> DataCallListChangedIndicationHandler;
typedef std::function<void(RIL_Errno)> SetInitialAttachApnCallback ;
typedef std::function<void(RIL_Errno)> SetDataProfileCallback ;
typedef std::function<void(const RIL_Errno, const RIL_Data_Call_Response_v11*, const size_t)> GetDataCallListCallback;
typedef std::function<void(const RIL_Errno)> SetDataThrottlingCallback;
typedef std::function<void(const RIL_Errno)> GetRilDataDumpCallback;
typedef std::function<void(RIL_Errno, const RIL_LceStatusInfo* )> StartLceDataCallback;
typedef std::function<void(RIL_Errno, const RIL_LceStatusInfo* )> StopLceDataCallback;
typedef std::function<void(RIL_Errno, const RIL_LceDataInfo* )> PullLceDataCallback;
typedef std::function<void(RIL_Errno, const int*)> SetLinkCapFilterCallback;
typedef std::function<void(const RIL_PCO_Data)> RILPCODataChangeIndicationHandler;
typedef std::function<void(const RIL_LinkCapacityEstimate)> RILLCEDataChangeIndicationHandler;
typedef std::function<void()> CarrierInfoForImsiHandler;
typedef std::function<void(const Status, const uint32_t, const RIL_KeepaliveStatusCode)> RILDataRadioKeepAliveHandler;
typedef std::function<void(const Status, const std::vector<RIL_PhysicalChannelConfig> )> PhysicalConfigStructHandlerHandler;
typedef std::function<void(const Status, const five_g_icon_type )> RILDataNrIconChangeIndicationHandler;
typedef std::function<void(RIL_Errno, const RIL_LinkCapCriteriaResult*)> SetLinkCapRptCriteriaCallback;
typedef std::function<void(RIL_Errno, const Status, const five_g_icon_type*)> GetDataNrIconTypeCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*)> SetPreferredDataModemCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*, const uint32_t*, const RIL_KeepaliveStatus_t*)> StartKeepAliveCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*)> StopKeepAliveCallback;
typedef std::function<void(RIL_Errno, const Status, const int*)> RegisterDataUnsolIndicationCallback;
typedef std::function<void(const RIL_Errno, const Status, const RIL_Errno*)> CarrierInfoForImsiEncryptionCallback;
typedef std::function<void(const RIL_Errno, const Status, const rilSlicingConfig*)> GetSlicingConfigCallback;

/**
 * @brief Indication callback function for SlicingConfigChanged
 * The callback function will be invoked when slicing config will be changed at modem
 *
 * @param SlicingConfig (type @ref rilSlicingConfig)
 */
typedef std::function<void(const rilSlicingConfig&)> SlicingConfigChangedIndicationHandler;
/**
 * @brief Indication callback function for UnthrottleApn
 * The callback function will be invoked when an apn will be unthrottled
 *
 * @param ProfileInfo (type @ref RIL_DataProfileInfo)
 */
typedef std::function<void(const RIL_DataProfileInfo&)> UnthrottleApnTdIndicationHandler;
/**
 * @brief Indication callback function for UnthrottleApn
 * The callback function will be invoked when an apn will be unthrottled
 *
 * @param apn name (type string)
 */
typedef std::function<void(const std::string&)> UnthrottleApnIndicationHandler;
/**
 * @brief Indication callback function for @ref registerNewSmsOnSimIndicationHandler.
 * The callback function will be invoked when new SMS has been stored on SIM card
 *
 * @param index               The slot index on the SIM that contains the new message
 */
typedef std::function<void(const int&)> NewSmsOnSimIndicationHandler;

/**
 * @brief Indication callback function for @ref registerSimSmsStorageFullIndicationHandler.
 * The callback function will be invoked to indicates that SMS storage on the SIM is full.
 * Sent when the network attempts to deliver a new SMS message.
 * Messages cannot be saved on the SIM until space is freed.
 * In particular, incoming Class 2 messages cannot be stored.
 */
typedef std::function<void()> SimSmsStorageFullIndicationHandler;

/**
 * @brief Indication callback function for @ref registerNewSmsStatusReportIndicationHandler.
 * The callback function will be invoked when new SMS status report is received.
 *
 * @param message             PDU of an SMS-STATUS-REPORT as an ascii string of hex digits.
 */
typedef std::function<void(const char*)> NewSmsStatusReportIndicationHandler;

/**
 * @brief Indication callback function for @ref registerCdmaSmsRuimStorageFullIndicationHandler.
 * The callback function will be invoked to indicates that SMS storage on the RUIM is full.
 * Messages cannot be saved on the RUIM until space is freed.
 *
 * @param message             CDMA SMS message
 *                            Refer @ref RIL_CDMA_SMS_Message
 */
typedef std::function<void()> CdmaSmsRuimStorageFullIndicationHandler;

/**
 * @brief Callback function for @ref queryAvailableNetworks request.
 *
 * @param errorCode        Error code @ref RIL_Errno
 * @param operator info    Vector of RIL_OperatorInfo.
 *                         Refer @ref RIL_OperatorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const std::vector<RIL_OperatorInfo>&)> QueryAvailableNetworksCallback;

/* QCRIL_UIM Request Message Callbacks */

typedef std::function<void(RIL_Errno, const char *)> GetImsiReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> IOReqCallback;
typedef std::function<void(RIL_Errno, const RIL_CardStatus_v6*)> GetSimStatusReqCallback;
typedef std::function<void(RIL_Errno, const int *, size_t len)> OpenChannelReqCallback;
typedef std::function<void(RIL_Errno)> CloseChannelReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> TransmitApduBasicReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> TransmitApduChannelReqCallback;
typedef std::function<void(RIL_Errno, int *)> ChangePinReqCallback;
typedef std::function<void(RIL_Errno, int *)> ChangePin2ReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPukReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPuk2ReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPinReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPin2ReqCallback;
typedef std::function<void(RIL_Errno, int32_t *)> QueryFacilityLockReqCallback;
typedef std::function<void(RIL_Errno, int *)> SetFacilityLockReqCallback;
typedef std::function<void(RIL_Errno, const char *)> IsimAuthenticationReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> StkSendEnvelopeWithStatusReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> AuthenticationReqCallback;

/**
 * @brief Callback function for @ref setTtyMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetTtyModeCallBack;

/**
 * @brief Callback function for @ref getTtyMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param ttyMode         TTY mode
 *                        <br>0 for TTY off.
 *                        <br>1 for TTY Full.
 *                        <br>2 for TTY HCO (hearing carryover).
 *                        <br>3 for TTY VCO (voice carryover).
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int&)> GetTtyModeCallBack;

/**
 * @brief Callback function for @ref setCdmaVoicePrefMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SetCdmaVoicePrefModeCallBack;

/**
 * @brief Callback function for @ref getCdmaVoicePrefMode request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param mode     Voice privacy mode
 *                   <br>0 for Standard Privacy Mode (Public Long Code Mask)
 *                   <br>1 for Enhanced Privacy Mode (Private Long Code Mask)
 * @retval void
 */
typedef std::function<void(RIL_Errno, const int&)> GetCdmaVoicePrefModeCallBack;

/**
 * @brief Callback function for @ref sendCdmaFlash request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SendCdmaFlashCallBack;

/**
 * @brief Callback function for @ref sendCdmaBurstDtmf request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @retval void
 */
typedef std::function<void(RIL_Errno)> SendCdmaBurstDtmfCallBack;

/**
 * @brief Callback function for @ref getImsRegState request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param registered      Registration state.
 *                        <br>false - Not registered
 *                        <br>true - Registered
 * @param techFamily      IMS SMS format.
 *                        Refer @ref RIL_RadioTechnologyFamily
 * @retval void
 */
typedef std::function<void(RIL_Errno, const bool&, const RIL_RadioTechnologyFamily&)> GetImsRegStateCallBack;


/**
 * @brief Callback function for @ref getCdmaSubscription request.
 *
 * @param errorCode           Error code @ref RIL_Errno
 * @param subscriptionInfo    Subscription info
 *                            <br>subscriptionInfo[0] is MDN if CDMA subscription is available.
 *                            <br>subscriptionInfo[1] is a comma separated list of H_SID (Home SID) if
 *                              CDMA subscription is available, in decimal format.
 *                            <br>subscriptionInfo[2] is a comma separated list of H_NID (Home NID) if
 *                              CDMA subscription is available, in decimal format.
 *                            <br>subscriptionInfo[3] is MIN (10 digits, MIN2+MIN1) if CDMA subscription
 *                              is available.
 *                            <br>subscriptionInfo[4] is PRL version if CDMA subscription is available.
 * @retval void
 */
typedef std::function<void(RIL_Errno, const char*[5])> GetCdmaSubInfoCallback;

/**
 * @brief Callback function for @ref GetAdnRecord request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param adnCountInfo    ADN count info.
 *                        Refer @ref RIL_AdnCountInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, RIL_AdnCountInfo *)> GetAdnRecordCallback;
typedef std::function<void(RIL_Errno, RIL_ModuleList *module_list)> GetModuleListCallback;
typedef std::function<void(RIL_Errno, const std::string &config)> GetCurrentConfigCallback;
typedef std::function<void(RIL_Errno, const std::vector<std::string> &configList)> GetConfigListCallback;
typedef std::function<void(const RIL_SimRefreshResponse_v7 *)> SimRefreshIndicationHandler;

/**
*  @brief  Callback function for @ref SendLpaUserRequest request.
*
*  @param RIL_Errno   Error code @ref RIL_Errno
* @param LpaUserReponse Refer #ref UimLpaUserResponse
* @retval void
*/
typedef std::function<void(RIL_Errno, const UimLpaUserResponse *)>lpauserresponseCallback;

/**
*  @brief  Callback function for @ref SendHttpTxnCmpltedRequest request.
*
*  @param RIL_Errno   Error code @ref RIL_Errno
* @param token
* @retval void
*/
typedef std::function<void(RIL_Errno, int32_t)>UimLpaHttpTxnCompletedResponse;

/**
*  @brief  Callback function for @ref registerAddProfProgressIndicationHandler.
*
*  @param AddProfileIndication payload   @ref UimLpaAddProfProgressInd
* @retval void
*/
typedef std::function<void(const UimLpaAddProfProgressInd *)> AddProfProgressIndicationHandler;

/**
*  @brief  Callback function for @ref registerAddProfProgressIndicationHandler.
*
*  @param AddProfileIndication payload   @ref UimLpaAddProfProgressInd
* @retval void
*/
typedef std::function<void(const UimLpaHttpTxnIndype *)> LpaHttpTxnIndicationHandler;

/**
 * @brief Callback function for @ref queryCallWaiting request.
 *
 * @param errorCode       Error code @ref RIL_Errno
 * @param data            Call waiting information
 *                        Refer @ref RIL_QueryCallWaitingResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_QueryCallWaitingResponse&)> QueryCallWaitingCb;

/**
 * Callback function for setCallWaiting
 * @param errorCode         Error code @ref RIL_Errno
 * @param errorDetails      Error details.
 *                          Refer @ref RIL_IMS_SipErrorInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> SetCallWaitingCb;

/**
 * Callback function for queryCallForwardStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Call forward status details.
 *                          Refer @ref RIL_QueryCallForwardStatusInfo
 */
typedef std::function<void(RIL_Errno, const RIL_QueryCallForwardStatusInfo&)> QueryCallForwardStatusCb;

/**
 * Callback function for setCallForward
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Set call forward status info.
 *                          Refer @ref RIL_IMS_SetCallForwardStatusInfo
 */
typedef std::function<void(RIL_Errno, const RIL_SetCallForwardStatusInfo&)> SetCallForwardStatusCb;

/**
 * Callback function for queryClip
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              CLIP info.  Refer @ref RIL_ClipInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_ClipInfo&)> QueryClipCb;

/**
 * Callback function for getClir
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              CLIR info.  Refer @ref RIL_ClirInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_ClirInfo&)> GetClirCb;

/**
 * Callback function for suppSvcStatus
 * @param errorCode         Error code @ref RIL_Errno
 * @param data              Status of supplementary service request.
 *                          Refer @ref RIL_SuppSvcResponse
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_SuppSvcResponse&)> SuppSvcStatusCallback;

/**
 * @brief Callback function for @ref getCurrentCalls request.
 *
 * @param errorCode         Error code @ref RIL_Errno
 * @param callList          Call info list
 *                          Refer @ref RIL_CallInfo
 * @retval void
 */
typedef std::function<void(RIL_Errno, const std::vector<RIL_CallInfo> callList)> GetCurrentCallsCb;

/**
 * @brief Callback function for @ref getImei request
 * @param errorCode         Error code @ref RIL_Errno
 * @param imeiInfo          Imei Information Structure
 *                          Refer to @ref RIL_ImeiInfo
 *
 * @retval void
 */
typedef std::function<void(RIL_Errno, const RIL_ImeiInfo)> GetImeiCb;

/**
 * @brief Indication callback function for @ref
 * registerImsUnsolParticipantStatusInfoIndicationHandler.
 * The callback function will be invoked to notify the status of the participants in the
 * conference call.
 *
 * @param participantInfo     Participant status info.
 *                            Refer @ref RIL_IMS_ParticipantStatusInfo
 * @retval void
 */
using ParticipantStatusInfoIndHandler = std::function<void(const RIL_ParticipantStatusInfo&)>;

/**
 * @brief Indication callback function for @ref registerUnsolIncomingCallAutoRejectedIndHandler.
 * The callback function will be invoked to notify the auto call reject notifications.
 *
 * @param voiceInfo           RTT voice info
 *                            Refer @ref RIL_IMS_VoiceInfoType
 * @retval void
 */
typedef std::function<void(const RIL_AutoCallRejectionInfo&)> IncomingCallAutoRejectedIndHandler;

/**
 * @brief Indication callback function for @ref registerUnsolPreAlertingCallInfoAvailableIndHandler.
 * The callback function will be invoked to indicate pre alerting call info from modem.
 *
 * @param status              Pre alerting call info
 *                            Refer @ref RIL_PreAlertingCallInfo
 * @retval void
 */
typedef std::function<void(const RIL_PreAlertingCallInfo&)> PreAlertingCallInfoAvailableIndHandler;

/**
 * @brief Indication callback function for @ref registerOnUssdIndicationHandler.
 * The callback function will be invoked when a new USSD message is received.
 *
 * @param mode                USSD Mode.  Refer @ref RIL_UssdModeType
 * @param message             USSD message
 */
typedef std::function<void(const RIL_UssdModeType& mode, const std::string& message)> OnUssdIndicationHdlr;

/**
 * @brief Indication callback function for @ref registerUnsolHandoverIndicationHandler.
 * The callback function will be invoked to notify that a call has been handed over
 * between technologies
 *
 * @param handoverInfo        Handover info.
 *                            Refer @ref RIL_HandoverInfo
 * @retval void
 */
typedef std::function<void(const RIL_HandoverInfo&)> HandoverIndicationHandler;

/**
 * @brief Indication callback function for @ref registerSuppSvcNotificationIndicationHandler.
 * The callback function will be invoked to notify supplementary service related
 * notification from the network.
 *
 * @param suppSvc             Supplementary service notification info.
 *                            Refer @ref RIL_SuppSvcNotification
 * @retval void
 */
typedef std::function<void(const RIL_SuppSvcNotificationInfo&)> SuppSvcNotificationIndicationHdlr;

/**
 * @brief Indication callback function for @ref registerUnsolCallStateChangedIndicationHandler.
 * The callback function will be invoked when call state has changed
 *
 * @param callList            Call info list
 *                            Refer @ref RIL_CallInfo
 */
typedef std::function<void(const std::vector<RIL_CallInfo> callList)> CallStateChangedIndicationHandler;

/**
 * @brief Indication callback function for @ref registerUnsolSrtpEncryptionStatusIndicationHandler.
 * The callback function will be invoked to indicate secure real-time transport protocol(SRTP)
 * encryption info from modem.
 *
 * @param srtpInfo            Secure real-time transport protocol(SRTP) status info.
 *                            Refer @ref RIL_IMS_SrtpEncryptionStatus
 */
typedef std::function<void(const RIL_IMS_SrtpEncryptionStatus&)> SrtpEncryptionStatusIndicationHandler;

/**
 * @brief RilApiSession maintains state associated with an API session opened by a client of RIL.
 */
class RilApiSession {
    public:
        /**
         * @brief Constructor to create a RilApiSession using unix socket.
         *
         * @param socketPath  socket address
         */
        RilApiSession(const std::string& socketPath);

        /**
         * @brief Constructor to create a RilApiSession using tcp socket.
         *
         * @param ipAddress  IP adress
         * @param tcpPort    Port number
         */
        RilApiSession(const std::string& ipAddress, const unsigned short tcpPort);

        ~RilApiSession();

        /**
         * @brief Initialize RIL API session.
         *
         * @param errorCallback  Error call back
         * @retval Returns status of initialize.
         *         SUCCESS or FAILURE
         */
        Status initialize(const RequestManager::ErrorCallback& errorCallback);

        /**
         * @brief Re-initialize RIL API session.
         *
         * @param errorCallback  Error call back
         * @retval Returns status of initialize.
         *         SUCCESS or FAILURE
         */
        Status reinitialize(const RequestManager::ErrorCallback& errorCallback);

        /**
         * @brief deinitialize RIL API session.
         *
         * @retval Returns status of initialize.
         *         SUCCESS or FAILURE
         */
        Status deinitialize();

        Status createUnixSocket();
        Status createTcpIpSocket();

        #ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
        /**
         * @brief Set testcaseOutDir
         *
         * @param path   Path to output directory that testcases will be written to.
         * @retval Returns status of setter SUCCESS or FAILURE. On FAILURE, no path was set.
         *
         */
        Status setTestcaseOutputDir(std::string path);

       /**
        * @brief Get testcaseCaseOutputDir
        *
        * @retval Returns testcaseOutputDir
        */
       std::string getTestcaseOutputDir();

        /**
         * @brief Enable or disable testcase generation.
         *
         * @param value   true enables testcase generation. false disables it.
         * @retval Returns status of setter SUCCESS or FAILURE.
         */
        Status setSaveTestcases(bool value);

        /**
         * @brief Getter for saveTestcases.
         *
         * @retval True if testcase generation is enabled, false otherwise.
         */
        bool getSaveTestcases();
        #endif // QCRIL_BUILD_AFL_TESTCASE_GEN

        /**
         * @brief Register handler for unsol indications
         *
         * @retval Returns status of initialize.
         *         SUCCESS or FAILURE
         */
        Status registerIndicationHandler(int32_t indicationId, const RequestManager::GenericIndicationHandler& indicationHandler);

        /**
         * @brief Initiate a voice call.
         *
         * This API is deprecated; use @ref dial(const RIL_DialParams& dialParams, const
         * VoidResponseHandler& cb).
         *
         * @param dialParams  Dial parameters contains number, clir and UUS info.
         * @param cb          Callback to get the response of the dial request.
         *                    Refer @ref DialCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status dial(const RIL_Dial& dialParams, const DialCallback& cb);

        /**
         * @brief Request all of the current cell information known to the radio. The radio
         * must a list of all current cells, including the neighboring cells. If for a particular
         * cell information isn't known then the appropriate unknown value will be returned.
         *
         * @param cb          Callback to get the response of the request.
         *                    Refer @ref CellInfoCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCellInfo(const CellInfoCallback& cb);

        /**
         * @brief Register for Signal Strength change indications
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          @ref SignalStrengthIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerSignalStrengthIndicationHandler(const SignalStrengthIndicationHandler& indicationHandler);

        /**
         * @brief Register for CellInfo change indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          @ref CellInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCellInfoIndicationHandler(const CellInfoIndicationHandler& indicationHandler);

        /**
         * @brief Register for new CDMA SMS indications.  The indicationHandler will be invoked
         * when a new CDMA SMS is received.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          @ref CdmaNewSmsIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaNewSmsIndicationHandler(const CdmaNewSmsIndicationHandler& indicationHandler);

        /**
         * @brief Requests to set the preferred network type for searching and registering
         * (CS/PS domain, RAT, and operation mode)
         *
         * @param nwType      Preferred network type. Refer @ref RIL_PreferredNetworkType
         * @param cb          Callback to get the response of the request.
         *                    Refer @ref SetNetworkTypeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setPreferredNetworkType(int nwType, const SetNetworkTypeCallback &cb);

        /**
         * @brief Query the preferred network type (CS/PS domain, RAT, and operation mode)
         * for searching and registering
         *
         * @param cb          Callback to get the response of the request.
         *                    Refer @ref GetNetworkTypeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getPreferredNetworkType(const GetNetworkTypeCallback &cb);

        /**
         * @brief Request list of current calls
         *
         * This API is deprecated; use the below getCurrentCalls API with GetCurrentCallsCb
         *
         * @param cb Callback to get the response of the getCurrentCalls request.
         *           Refer @ref GetCurrentCallsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCurrentCalls(const GetCurrentCallsCallback& cb);

        /**
         * @brief Request list of current calls
         *
         * @param cb Callback to get the response of the getCurrentCalls request.
         *           Refer @ref GetCurrentCallsCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCurrentCalls(const GetCurrentCallsCb& cb);

        /**
         * Request the IMEI of the device
         * @param cb Callback to get the response of the getImei request.
         *           Refer to @ref GetImeiCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getImei(const GetImeiCb& cb);

        /**
         * @brief Answer an INCOMING call
         *
         * This API is deprecated; use @ref answer(const RIL_Answer& answer, const
         * VoidResponseHandler& cb)
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref AnswerCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status answer(const AnswerCallback& cb);

        /**
         * @brief Make conference with holding and active calls
         *
         * This API is deprecated; use @ref conference(const SipErrorInfoResponseHandler& cb);
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref ConferenceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status conference(const ConferenceCallback& cb);

        /**
         * @brief Switch waiting or holding call and active call
         * This API is deprecated; use the below switchWaitingOrHoldingAndActive API with extended
         * paramters.
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref SwitchWaitingOrHoldingAndActiveCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status switchWaitingOrHoldingAndActive(const SwitchWaitingOrHoldingAndActiveCallback& cb);

        /**
         * @brief Switch waiting or holding call and active call.
         *
         * @param reqData Contains the @ref RIL_SwitchWaitingOrHoldingAndActive
         * @param cb Callback to get the response of the the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status switchWaitingOrHoldingAndActive(RIL_SwitchWaitingOrHoldingAndActive reqData,
            const VoidResponseHandler& cb);

        /**
         * @brief Send UDUB (user determined used busy) to ringing or waiting call
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref UdubCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status udub(const UdubCallback& cb);

        /**
         * @brief Hang up waiting or held call
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref HangupWaitingOrBackgroundCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status hangupWaitingOrBackground(const HangupWaitingOrBackgroundCallback& cb);

        /**
         * @brief Hang up the active call and resume the waiting or held call
         *
         * @param cb Callback to get the response of the the request.
         *           Refer @ref HangupForegroundResumeBackgroundCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status hangupForegroundResumeBackground(const HangupForegroundResumeBackgroundCallback& cb);

        /**
         * @brief Separate a party from a multiparty call placing the multiparty call
         * (less the specified party) on hold and leaving the specified party
         * as the only other member of the current (active) call
         *
         * @param callId Call Id
         * @param cb     Callback to get the response of the the request.
         *               Refer @ref SeparateConnectionCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status separateConnection(int callId, const SeparateConnectionCallback& cb);

        /**
         * @brief Set the phones radio capability.
         *
         * @param radioCap Radio capability. Refer @ref RIL_RadioCapability
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetRadioCapabilityCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setRadioCapability(const RIL_RadioCapability &radioCap, const SetRadioCapabilityCallback& cb);

        /**
         * @brief Get phone radio capablility.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetRadioCapabilityCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getRadioCapability(const GetRadioCapabilityCallback& cb);

        /**
         * @brief Inform the device is shutting down.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref ShutDownCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status shutDown(const ShutDownCallback& cb);

        /**
         * @brief Request to exit emergency callback mode.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref ExitEmergencyCbModeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status exitEmergencyCbMode(const ExitEmergencyCbModeCallback& cb);

        /**
         * @brief Set a specified band for RF configuration.
         *
         * @param bandMode Band Preference. Refer @ref RIL_RadioBandMode
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetBandModeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setBandMode(const int& bandMode, const SetBandModeCallback& cb);

        /**
         * @brief Query the list of band mode supported by RF.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref QueryAvailableBandModeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryAvailableBandMode(const QueryAvailableBandModeCallback& cb);

         /**
         * @brief Set a usage setting preference for the current SUB.
         *
         * @param settingMode usage preference. Refer @ref RIL_UsageSettingMode
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetDeviceUsageSetting
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setUsageSetting(const int settingMode, const SetUsageSettingCallback &cb);

         /**
         * @brief Get the usage setting preference for the current SUB.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetUsageSettingCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getUsageSetting(const GetUsageSettingCallback& cb);

        /**
         * @brief Requests current signal strength
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetSignalStrengthCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getSignalStrength(const GetSignalStrengthCallback& cb);

        /**
         * @brief Query the radio technology type (3GPP/3GPP2) used for voice.
         * Query is valid only when radio state is not RADIO_STATE_UNAVAILABLE
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoiceRadioTechCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status VoiceRadioTech(const VoiceRadioTechCallback &cb);

        /**
         * @brief Request current registration state
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetVoiceRegCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getVoiceRegStatus(const GetVoiceRegCallback& cb);

        /**
         * @brief Request current DATA registration state
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetDataRegCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getDataRegStatus(const GetDataRegCallback& cb);

        /**
         * @brief Toggle radio on and off (for "airplane" mode)
         *
         * @param on       Radio on state
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref RadioPowerCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status radioPower(const bool& on, const RadioPowerCallback& cb);

        /**
         * @brief Toggle radio on and off (for "airplane" mode)
         *
         * @param req      Radi power parameters
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref RadioPowerCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status radioPower(const RIL_RadioPowerReq &req, const RadioPowerCallback& cb);

        /**
         * @brief Enable SIM card
         *
         * @param on       on state
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref EnableSimCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status enableSim(const bool& on, const EnableSimCallback& cb);

        /**
         * @brief Get SIM card enable/disable status
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetEnableSimStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getEnableSimStatus(const GetEnableSimStatusCallback& cb);

        /**
         * @brief Tells the modem whether data calls are allowed or not
         *
         * @param on       on status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref AllowDataCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status allowData(const bool& on, const AllowDataCallback& cb);

        /**
         * @brief Enables/disables network state change notifications due to changes in
         * LAC and/or CID (for GSM) or BID/SID/NID/latitude/longitude (for CDMA).
         *
         * @param on       on status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetLocationUpdatesCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setLocationUpdates(const bool& on, const SetLocationUpdatesCallback& cb);

        /**
         * @brief Get modem activity information for power consumption estimation.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetActivityInfoCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getActivityInfo(const GetActivityInfoCallback& cb);

        /**
         * @brief Get the device ESN / MEID / IMEI / IMEISV.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref DeviceIdentityCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status deviceIdentity(const DeviceIdentityCallback &cb);

        /**
         * @brief Request current operator ONS or EONS
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref OperatorNameCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status operatorName(const OperatorNameCallback& cb);

        /**
         * @brief Set the unsolicited response filter.
         * <br>This is used to prevent unnecessary application processor wake up for power saving
         * purposes by suppressing the unsolicited responses in certain scenarios.
         *
         * @param filter   Bitmask of @ref RIL_UnsolicitedResponseFilter
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setIndicationFilter(int32_t filter, const VoidResponseHandler &cb);

        /**
         * @brief Query current network selectin mode
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref QueryNetworkSelectionModeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status QueryNetworkSelectionMode(const QueryNetworkSelectionModeCallback &cb);

        /**
         * @brief Request to specify that the network should be selected automatically
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetNetworkSelectionAutomaticCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setNetworkSelectionAutomatic(const SetNetworkSelectionAutomaticCallback &cb);

        /**
         * @brief Request to specify that the network should be selected automatically
         *
         * @param accessMode    AccessMode of the Network either PLMN or SNPN
         * @param cb            Callback to get the response of the the request.
         *                      Refer @ref SetNetworkSelectionAutomaticCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setNetworkSelectionAutomatic(const RIL_AccessMode& accessMode,
                                            const SetNetworkSelectionAutomaticCallback& cb);
        /**
         * @brief Request to manually select a specified network.
         *
         * @param mcc      MCC of the network to select
         * @param mnc      MNC of the network to select
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetNetworkSelectionManualCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setNetworkSelectionManual(std::string mcc, std::string mnc, const SetNetworkSelectionAutomaticCallback &cb);

        /**
         * @brief Request to manually select a specified network.
         *
         * @param req      Manual Network scan parameter.
         *                 Refer to @ref RIL_SetNetworkSelectionManualParam
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetNetworkSelectionManualCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setNetworkSelectionManual(const RIL_SetNetworkSelectionManualParam& req,
                                         const SetNetworkSelectionAutomaticCallback& cb);
        /**
         * @brief Request to get the baseband version
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref BasebandVersionCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status basebandVersion(const BasebandVersionCallback &cb);

        /**
         * @brief Request to hangup a specific call
         * This API is deprecated; use the below hangup API with extended paramters.
         *
         * @param connId   Connnection id
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref HangupCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status hangup(const int& connid, const HangupCallback& cb);

        /**
         * @brief Request to hangup a specific call
         *
         * @param hangupReq Hangup request containing connIndex, conference uri.
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref HangupCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status hangup(const RIL_Hangup& hangup, const HangupCallback& cb);

        /**
         * @brief Send a USSD message.
         *
         * <br>If a USSD session already exists, the message will be sent in the
         * context of that session. Otherwise, a new session should be created.
         * <br>The network reply will be reported via the indication callback registered with
         * the @ref registerOnUssdIndicationHandler.
         *
         * @param ussd     USSD request in UTF-8 format
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SendUssdCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendUssd(const std::string& ussd, const SendUssdCallback& cb);

        /**
         * @brief Cancel the current USSD session if one exists
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref CancelUssdCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status cancelUssd(const CancelUssdCallback& cb);

        /**
         * @brief Send a DTMF tone
         *
         * @param dtmf     A single character with one of 12 values: 0-9,*,#
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref DtmfCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status dtmf(const char& dtmf, const DtmfCallback& cb);

        /**
         * @brief Request to start playing a DTMF tone. Continue playing DTMF tone until
         * @ref dtmfStop is received
         *
         * @param dtmf     A single character with one of 12 values: 0-9,*,#
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref DtmfStartCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status dtmfStart(const char& dtmf, const DtmfStartCallback& cb);

        /**
         * @brief Request to stop playing a currently playing DTMF tone.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref DtmfStopCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status dtmfStop(const DtmfStopCallback& cb);

        /**
         * @brief Requests the failure cause code for the most recently terminated call
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref LastCallFailCauseCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status lastCallFailCause(const LastCallFailCauseCallback& cb);

        /**
         * @brief Connects the two calls and disconnects the subscriber from both calls.
         *
         * This API is deprecated; use explicitCallTransfer(
         * const RIL_ExplicitCallTransfer& explicitCall, const VoidResponseHandler& cb);
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref OperatorNameCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status explicitCallTransfer(const ExplicitCallTransferCallback& cb);

        /**
         * @brief Sets current mute settings
         *
         * @param mute     mute status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetMuteCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setMute(const bool& mute, const SetMuteCallback& cb);

        /**
         * @brief Queries the current state of the uplink mute setting
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetMuteCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getMute(const GetMuteCallback& cb);

        /**
         * @brief Request to set CLIR
         *
         * @param clir     clir value
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetClirCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setClir(const int* clir, const SetClirCallback& cb);

        /**
         * @brief Request to send the RTT message
         *
         * This API is deprecated, use @ref sendRttMessage
         *
         * @param msg      RTT message
         * @param len      Length of RTT message
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SendRttCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSendRttMessage(const char* msg, size_t len, const SendRttCallback& cb);

        /**
         * @brief Request to send the RTT message
         *
         * @param msg      RTT message
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendRttMessage(const std::string msg, const VoidResponseHandler& cb);

        /**
         * @brief Request to send GeoLocation Address info.
         *
         * @param geolocationInfo      Geo location info
         * @param cb                   Callback to get the response of the the request.
         *                             Refer @ref GeolocationInfoCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSendGeolocationInfo( const RIL_IMS_GeolocationInfo& geolocationInfo,
                                       const GeolocationInfoCallback& cb);

        /**
         * @brief Register for incoming RTT message
         *
         * This API is deprecated, use @ref registerRttMessageIndicationHandler instead
         *
         * The callback function will be invoked when a RTT message received.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsRttMessageIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsRttMessageIndicationHandler(const
                    ImsRttMessageIndicationHandler& indicationHandler);

        /**
         * @brief Register for incoming RTT message
         * The callback function will be invoked when a RTT message received.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RttMessageIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRttMessageIndicationHandler(const
                    RttMessageIndicationHandler& indicationHandler);
        /**
         * @brief Register for IMS geolocation request indication.
         * The callback function will be invoked when the lower layers request for the geo location
         * address information. The 'request' will contain the latitude (double), longitude (double)
         * and application is expected to resolve this to geo location address information and set
         * back to the lower layers using @ref imsSendGeolocationInfo.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsGeolocationIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsGeolocationIndicationHandler(const
                    ImsGeolocationIndicationHandler& indicationHandler);

        /**
         * @brief Register for geo location retrival status indication.
         * The callback function will be invoked to indicates geo location retrival status in modem.
         * Modem needs geo location to register IMS over standalone WIFI.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RetrievingGeoLocationDataStatusHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolRetrievingGeoLocationDataStatus(
            const RetrievingGeoLocationDataStatusHandler& indicationHandler);

        /**
         * @brief Register for VoWiFi call quality information indication.
         *
         * This API is deprecated, use @ref registerVowifiQualityIndicationHandler instead
         *
         * The callback function will be invoked to notify the VoWiFi call quality information.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsVowifiQualityIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsVowifiQualityIndicationHandler(const
                    ImsVowifiQualityIndicationHandler& indicationHandler);

        /**
         * @brief Register for VoWiFi call quality information indication.
         * The callback function will be invoked to notify the VoWiFi call quality information.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref VowifiQualityIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerVowifiQualityIndicationHandler(const
                    VowifiQualityIndicationHandler& indicationHandler);

        /**
         * @brief Request to query current CLIR
         *
         * This API is deprecated; use getClir(const GetClirCb& cb);
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetClirCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getClir(const GetClirCallback& cb);

        /**
         * @brief Request to query the status of call forwarding supplementary service.
         * This API is deprecated; use the below API instead.
         * Status queryCallForwardStatus(const uint32_t reason,
         *                               const uint32_t serviceClass,
         *                               const QueryCallForwardStatusCb& cb);
         *
         * @param callFwdInfo Call forward info request parameters. Refer @ref RIL_CallForwardInfo
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref QueryCallForwardStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryCallForwardStatus(const RIL_CallForwardInfo& callFwdInfo,
                                      const QueryCallForwardStatusCallback& cb);

        /**
         * @brief Request to set the call forwading status
         * This API is deprecated; use the below API instead.
         * Status setCallForward(const RIL_CallForwardParams& callFwdInfo,
         *                       const SetCallForwardStatusCb& cb);
         *
         * @param callFwdInfo Call forward info request parameters. Refer @ref RIL_CallForwardInfo
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref SetCallForwardStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCallForward(const RIL_CallForwardInfo& callFwdInfo,
                              const SetCallForwardStatusCallback& cb);

        /**
         * @brief Request to query the status of call forwarding supplementary service.
         *
         * @param reason       Call forward reason
         * @param serviceClass Service Class
         * @param cb           Callback to get the response of the the request.
         *                     Refer @ref QueryCallForwardStatusCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryCallForwardStatus(const uint32_t reason,
                                      const uint32_t serviceClass,
                                      const QueryCallForwardStatusCb& cb);

        /**
         * @brief Request to set the call forwading status
         *
         * @param callFwdInfo Call forward info request parameters. Refer @ref RIL_CallForwardInfo
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref SetCallForwardStatusCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCallForward(const RIL_CallForwardParams& callFwdInfo,
                              const SetCallForwardStatusCb& cb);

        /**
         * @brief Request to query current call waiting state
         *
         * @param serviceClass Service class to query
         * @param cb           Callback to get the response of the the request.
         *                     Refer @ref QueryCallWaitingCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryCallWaiting(const int& serviceClass, const QueryCallWaitingCallback& cb);

        /**
         * @brief Configure current call waiting state
         *
         * @param enabled      Enable status
         * @param serviceClass Bitmask of the service classes to modify
         * @param cb           Callback to get the response of the the request.
         *                     Refer @ref SetCallCallWaitingCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCallWaiting(const int& enabled, const int& serviceClass,
                              const SetCallCallWaitingCallback& cb);

        /**
         * @brief Request to change call barring facility password
         *
         * @param facility    Facility string
         * @param oldPassword Old password
         * @param newPassword New password
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref ChangeBarringPasswordCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status changeBarringPassword(const std::string& facility, const std::string& oldPassword,
                                     const std::string& newPassword,
                                     const ChangeBarringPasswordCallback& cb);

        /**
         * @brief Request to query the status of the CLIP supplementary service
         * This API is deprecated; use queryClip(const QueryClipCb& cb);
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref QueryClipCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryClip(const QueryClipCallback& cb);

        /**
         * @brief Enables/disables supplementary service related notifications from the network.
         * This API is deprecated; use setSuppSvcNotification(const bool enabled, const
         * VoidResponseHandler& cb);
         *
         * @param enabled  Enable status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetSuppSvcNotificationCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setSuppSvcNotification(const int& enabled, const SetSuppSvcNotificationCallback& cb);

        /**
         * @brief Sets the minimum time between when Cell Info Indication should be invoked.
         * A value of 0, means invoke Cell Info Indication when any of the reported
         * information changes. Setting the value to INT_MAX(0x7fffffff) means never issue
         * a Cell Info Indication. Refer @ref registerCellInfoIndicationHandler
         *
         * @param mRate    Minimum time in milliseconds
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref OperatorNameCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setUnsolCellInfoListRate(const int32_t& mRate, const SetUnsolCellInfoListRateCallback& cb);

        /**
         * @brief Send an SMS message
         *
         * @param message      SMS pdu
         * @param smscAddress  SMSC address
         * @param expectMore   Expect more messages
         * @param cb           Callback to get the response of the the request.
         *                     Refer @ref SendSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendSms(const std::string& message, const std::string& smscAddress, bool expectMore, const SendSmsCallback& cb);

        /**
         * @brief Acknowledge successful or failed receipt of SMS previously indicated
         * via the indication cb registered with @ref registerIncoming3GppSmsIndicationHandler
         *
         * @param ack      Acknowledgement @ref Acknowledge
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref AckSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status ackSms(const RIL_GsmSmsAck ack, const AckSmsCallback& cb);

        /**
         * @brief Queries the default Short Message Service Center address on the device.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetSmscAddressCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getSmscAddress(const GetSmscAddressCallback& cb);

        /**
         * @brief Sets the default Short Message Service Center address on the device.
         *
         * @param addr     SMSC address
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref OperatorNameCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setSmscAddress(const std::string& addr, const SetSmscAddressCallback& cb);

        /**
         * @brief Send a CDMA SMS message
         *
         * @param smsParams CDMA SMS parameter. Refer @ref RIL_CDMA_SMS_Message
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SendSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendCdmaSms(const RIL_CDMA_SMS_Message& smsParams, const SendSmsCallback& cb);

        /**
         * @brief Acknowledge the success or failure in the receipt of SMS
         * previously indicated via the indication cb registered with
         * @ref registerCdmaNewSmsIndicationHandler
         *
         * @param ackParams Acknowledgement parameters.  Refer @ref RIL_CDMA_SMS_Ack
         * @param cb        Callback to get the response of the the request.
         *                  Refer @ref AckCdmaSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status ackCdmaSms(const RIL_CDMA_SMS_Ack& ackParams, const AckCdmaSmsCallback& cb);

        /**
         * @brief Request the setting of CDMA Broadcast SMS config
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref CdmaGetBroadcastSmsConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCdmaSmsBroadcastConfig(const CdmaGetBroadcastSmsConfigCallback& cb);

        /**
         * @brief Set CDMA Broadcast SMS config
         *
         * @param cfgParams Array of CDMA broadcast configuration.
         *                  Refer @ref RIL_CDMA_BroadcastSmsConfigInfo
         * @param size      size of the cfgParams array
         * @param cb        Callback to get the response of the the request.
         *                  Refer @ref CdmaSetBroadcastSmsConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCdmaSmsBroadcastConfig(const RIL_CDMA_BroadcastSmsConfigInfo cfgParams[], size_t size, const CdmaSetBroadcastSmsConfigCallback& cb);

        /**
         * @brief Enable or disable the reception of CDMA Broadcast SMS
         *
         * @param value    Enable status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref CdmaSmsBroadcastActivationCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCdmaSmsBroadcastActivation(const int32_t value, const CdmaSmsBroadcastActivationCallback& cb);

        /**
         * @brief Stores a CDMA SMS message to RUIM memory.
         *
         * @param arg      CDMA SMS message.  Refer @ref RIL_CDMA_SMS_WriteArgs
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref WriteCdmaSmsToSimCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status writeCdmaSmsToRuim(const RIL_CDMA_SMS_WriteArgs arg, const WriteCdmaSmsToSimCallback& cb);

        /**
         * @brief Deletes a CDMA SMS message from RUIM memory.
         *
         * @param index    Record index of the message to delete
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref DeleteCdmaSmsOnRuimCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status deleteCdmaSmsOnRuim(int32_t index, const DeleteCdmaSmsOnRuimCallback& cb);

        /**
         * @brief Request the device MDN / H_SID / H_NID.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetCdmaSubInfoCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCdmaSubscription(const GetCdmaSubInfoCallback& cb);

        /**
         * @brief Stores a SMS message to SIM memory.
         *
         * @param pdu         SMS pdu
         * @param smscAddress SMSC address
         * @param status      status
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref WriteSmsToSimCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status writeSmsToSim(const std::string& pdu, const std::string& smscAddress, const int status, const WriteSmsToSimCallback& cb);

        /**
         * @brief Deletes a SMS message from SIM memory.
         *
         * @param recordIndex Record index of the message to delete
         * @param cb          Callback to get the response of the the request.
         *                    Refer @ref DeleteSmsOnSimCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status deleteSmsOnSim(int32_t recordIndex, const DeleteSmsOnSimCallback& cb);

        /**
         * @brief Indicates whether there is storage available for new SMS messages.
         *
         * @param storageSpaceAvailable Memory availability
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref ReportSmsMemoryStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status reportSmsMemoryStatus(bool storageSpaceAvailable, const ReportSmsMemoryStatusCallback& cb);

        /**
         * @brief Request to set the location where the CDMA subscription shall be retrieved
         *
         * @param subscriptionSource CDMA subscription source. Refer @ref RIL_CdmaSubscriptionSource
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetCdmaSubscriptionSourceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCdmaSubscriptionSource(RIL_CdmaSubscriptionSource subscriptionSource, const SetCdmaSubscriptionSourceCallback& cb);

        /**
         * @brief Request to query the location where the CDMA subscription shall be retrieved
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetCdmaSubscriptionSourceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCdmaSubscriptionSource(const GetCdmaSubscriptionSourceCallback& cb);

        /**
         * @brief Request to set the roaming preferences in CDMA
         *
         * @param roamingPref CDMA roaming preference.  Refer @ref RIL_CdmaRoamingPreference
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetCdmaRoamingPreferenceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCdmaRoamingPreference(RIL_CdmaRoamingPreference roamingPref, const SetCdmaRoamingPreferenceCallback& cb);

        /**
         * @brief Request the actual setting of the roaming preferences in CDMA in the modem
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetCdmaRoamingPreferenceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCdmaRoamingPreference(const GetCdmaRoamingPreferenceCallback& cb);

        /**
         * @brief Sets the signal strength reporting criteria.
         * The resulting reporting criteria are the AND of all the supplied criteria.
         *
         * @param criteria Signal strength reporting criteria.
         *                 Refer @ref RIL_SignalStrengthReportingCriteria
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetSignalStrengthReportingCriteriaCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setSignalStrengthReportingCriteria(const RIL_SignalStrengthReportingCriteria& criteria,
                const SetSignalStrengthReportingCriteriaCallback& cb);

        /**
         * @brief This is to send OEM-specific requests.
         *
         * @param data     data
         * @param dataLen  data legnth
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref OemhookRawCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status oemhookRaw(const char *data, size_t dataLen, const OemhookRawCallback& cb);

        /**
         * @brief Register for call state change indications.  The indicationHandler will be invoked
         * when there is a call state change.  Application need to call the @ref getCurrentCalls to
         * get the currnet call list.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolCallStateChangeIndicationHandler(const CallStateChangeIndicationHandler& indicationHandler);

        /**
         * @brief Register for call state changed indications.  The indicationHandler will be invoked
         * when there is a call state change.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CallStateChangedIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolCallStateChangedIndicationHandler(const CallStateChangedIndicationHandler& indicationHandler);

        /**
         * @brief Register for ringback tone indication.
         * The indication callback will be invoked to notify if the UE need to play the ringback
         * tone.
         *
         * This API is deprecated; use registerRingBackToneIndicationHdlr(const
         * RingBackToneIndicationHdlr& indicationHandler);
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RingBackToneIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRingBackToneIndicationHandler(const RingBackToneIndicationHandler& indicationHandler);

        /**
         * @brief Register for IMS registration state change indication.
         * The callback function will be called when IMS registration state has changed.
         * To get IMS registration state and IMS SMS format, callee needs to invoke the
         * @ref getImsRegState
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsNetworkStateChangeIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsNetworkStateChangeIndicationHandler(const ImsNetworkStateChangeIndicationHandler& indicationHandler);

        /**
         * @brief Register for voice network state change indication.
         * The callback function will be called when the voice network state changed.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref VoiceNetworkStateIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerVoiceNetworkStateIndicationHandler(const VoiceNetworkStateIndicationHandler& indicationHandler);

        /**
         * @brief Register for voice radio technology change indication.
         * The callback function will be invoked to indicates that voice technology has changed.
         * And will contains new radio technology.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref VoiceRadioTechIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerVoiceRadioTechIndicationHandler(const VoiceRadioTechIndicationHandler& indicationHandler);

        /**
         * @brief Register for the NITZ time indication.
         * The callback function will be invoked when radio has received a NITZ time message
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref NitzTimeIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerNitzTimeIndicationHandler(const NitzTimeIndicationHandler& indicationHandler);

        /**
         * @brief Register for radio state change indication.
         * The callback function will be invoked when there is change in the radio state.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RadioStateIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRadioStateIndicationHandler(const RadioStateIndicationHandler& indicationHandler);

        /**
         * @brief Register for radio capability change.
         * The callback function will be invoked when the restricted state changes (eg, for Domain
         * Specific Access Control).
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RestrictedStateChangedIndicationHandler.
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRestrictedStateChangedIndicationHandler(
                const RestrictedStateChangedIndicationHandler& indicationHandler);

        /**
         * @brief Register for call ring indication.
         * The callback function will be invoked to notify ring indication for an incoming call.
         *
         * This API is deprecated; use @ref registerCallRingIndicationHdlr instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CallRingIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCallRingIndicationHandler(const CallRingIndicationHandler& indicationHandler);

        /**
         * @brief Register for emergency callback mode indication
         * The callback function will be invoked to indicates that the radio system selection
         * module has autonomously entered to emergency callback mode.
         *
         * This API is deprecated; Use @ref registerEmergencyCallbackModeIndHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref EmergencyCbModeIndHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerEmergencyCbModeIndHandler(const EmergencyCbModeIndHandler& indicationHandler);

        /**
         * @brief Register for emergency callback mode indication
         * The callback function will be invoked to indicates that the radio system selection
         * module has autonomously entered to emergency callback mode.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref EmergencyCallbackModeIndHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerEmergencyCallbackModeIndHandler(
            const EmergencyCallbackModeIndHandler& indicationHandler);

        /**
         * @brief Register for UICC subscription status change indication.
         * The callback function will be invoked to notify the subscription readiness.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref UiccSubStatusChangedHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUiccSubStatusIndHandler(const UiccSubStatusChangedHandler& indicationHandler);

        /**
         * @brief Register for CDMA PLR (preferred roaming list) change indication.
         * The callback function willbe invoked when PRL (preferred roaming list) changes.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaPrlChangedHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaPrlChangedIndHandler(const CdmaPrlChangedHandler& indicationHandler);

        /**
         * @brief Register for radio capability change.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RadioCapabilityIndHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRadioCapabilityChangedIndHandler(
            const RadioCapabilityIndHandler& indicationHandler);
        /**
        *  @brief Registering Indication handler for Session End indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void()> VoidIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkUnsolSessionEnd(
                const VoidIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Proactive command indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const char *)> CharStarIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkUnsolProactiveCommand(
                const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for EventNotifications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const char *)> CharStarIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkEventNotify(
                const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Call Setup notification from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(int)> IntIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkCallSetup(
                const IntIndicationHandler &indicationHandler);

        /**
         * @brief Register for USSD indication.
         * The callback function will be invoked when a new USSD message is received.
         * This API is deprecated; Use @ref registerUnsolOnUssdIndicationHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref OnUssdIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerOnUssdIndicationHandler(const OnUssdIndicationHandler& indicationHandler);

        /**
         * @brief Register for USSD indication.
         * The callback function will be invoked when a new USSD message is received.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref OnUssdIndicationHdlr
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolOnUssdIndicationHandler(const OnUssdIndicationHdlr& indHandler);

        /**
         * @brief Register for supplementary service notification.
         *
         * This API is deprecated; use registerSuppSvcNotificationIndicationHandler(
         * const SuppSvcNotificationIndicationHdlr& indicationHandler);
         *
         * The callback function will be invoked to report supplementary service related
         * notification from the network.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref SuppSvcNotificationIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerSuppSvcNotificationIndicationHandler(
            const SuppSvcNotificationIndicationHandler& indicationHandler);

        /**
         * @brief Register for CDMA call waiting indication.
         * The callback function will be invoked when CDMA radio receives a call waiting indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaCallWaitingIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaCallWaitingIndicationHandler(
            const CdmaCallWaitingIndicationHandler& indicationHandler);

        /**
         * @brief Register for CDMA OTA provision status indication.
         * The callback function will be invoked when CDMA radio receives an update of the
         * progress of an OTASP/OTAPA call.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaOtaProvisionStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaOtaProvisionStatusIndicationHandler(
            const CdmaOtaProvisionStatusIndicationHandler& indicationHandler);

        /**
         * @brief Register for CDMA info records indication.
         * The callback function will be invoked when CDMA radio receives one or more info recs.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaInfoRecIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaInfoRecIndicationHandler(
            const CdmaInfoRecIndicationHandler& indicationHandler);

        /**
         * @brief Register for supplementary service indication.
         * The callback function will be invokved when SS response is received when DIAL/USSD/SS
         * is changed to SS by call control.
         *
         * This API is deprecated; use registerOnSupplementaryServiceIndicationHandler(const
         * OnSupplementaryServiceIndicationHdlr& indicationHandler);
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref OnSupplementaryServiceIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerOnSupplementaryServiceIndicationHandler(
            const OnSupplementaryServiceIndicationHandler& indicationHandler);

        /**
         * @brief Register for SRVCC state change indication.
         *
         * This API is deprecated; use @ref registerUnsolHandoverIndicationHandler instead.
         *
         * The callback function will be invoked when Single Radio Voice Call Continuity(SRVCC)
         * progress state has changed
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref SrvccStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerSrvccStatusIndicationHandler(
            const SrvccStatusIndicationHandler& indicationHandler);

        /**
         * @brief Register for new incoming 3gpp SMS.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref Incoming3GppSmsIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerIncoming3GppSmsIndicationHandler(
            const Incoming3GppSmsIndicationHandler& indicationHandler);

        /**
         * @brief Register for sim enablement status indicaiton
         * The callback function will be invoked to notify the sim enablement status
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref EnableSimStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerEnableSimStatusIndicationHandler(
            const EnableSimStatusIndicationHandler& indicationHandler);

        /**
         * @brief Register for CDMA subscription source change.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaSubscriptionSourceChangedIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaSubscriptionSourceChangedIndicationHandler(
            const CdmaSubscriptionSourceChangedIndicationHandler& indicationHandler);

        /**
         * @brief Register for new Broadcast SMS.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref BroadcastSmsIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerBroadcastSmsIndicationHandler(
            const BroadcastSmsIndicationHandler& indicationHandler);

        /**
         * @brief Register for modem reset indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ModemRestartIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerModemRestartIndicationHandler(
            const ModemRestartIndicationHandler& indicationHandler);

        /**
         * @brief Register for RIL server ready indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RilServerReadyIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRilServerReadyIndicationHandler(
            const RilServerReadyIndicationHandler& indicationHandler);

        /**
         * @brief Register for oemhook indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref OemhookIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerOemhookIndicationHandler(
            const OemhookIndicationHandler& indicationHandler);
        /**
         * @brief
         * SetupDataCall API is used to bring up data call.
         * @note This is not the preferred API for use. Use the other SetupDataCall API provided in this file.
         *
         * @param accessNetwork This is an enum of type #RIL_RadioAccessNetworks which specifies the preferred network access type.
         * @param dataProfile This is a struct of type RIL_DataProfileInfo which specifies the preferred data profile.
         * @param roamingAllowed This is a boolean variable which specifies if roaming is allowed or not.
         * @param reason This is an enum of type #RIL_RadioDataRequestReasons which specifies the reason for data call bringup request.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for Setupdatacall.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_Data_Call_Response_v11 *)> SetupDataCallCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call bring up is SUCCESS/FAILURE would be inidcated in the setupdatacall Callback.
         *
         *
        */
        Status setupDataCall(const RIL_RadioAccessNetworks accessNetwork, const RIL_DataProfileInfo& dataProfile,  const bool roamingAllowed, const RIL_RadioDataRequestReasons reason,const SetupDataCallCallback& cb);
        /**
         * @brief
         * SetupDataCall API is used to bring up data call.
         * @note This is the preferred API for use.
         *
         * @param callParams This is a struct of type RIL_SetUpDataCallParams with all the necessary parameters to bring up the data call.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for Setupdatacall.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_Data_Call_Response_v11 *)> SetupDataCallCallback.
         *
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call bring up is SUCCESS/FAILURE would be inidcated in the setupdatacall Callback.
         *
         *
        */
        Status setupDataCall(const RIL_SetUpDataCallParams &callParams, const SetupDataCallCallback& cb);
        /**
         * @brief
         * SetupDataCall API is used to bring up data call with additional parameters.
         * @note This API is only supported on Android
         *
         * @param callParams This is a struct of type RIL_SetUpDataCallParams_Ursp with all the necessary parameters to bring up the data call.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for Setupdatacall.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_Data_Call_Response_v11 *)> SetupDataCallCallback.
         * The response callback can contain additional parameters - expiration/deprecation time, mtuV4/mtuV6, traffic descriptors,
         * slicing info, and handover failure mode.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call bring up is SUCCESS/FAILURE would be indicated in the setupdatacall Callback.
         *
         *
        */
        Status setupDataCall(const RIL_SetUpDataCallParams_Ursp &callParams, const SetupDataCallCallback& cb);
        /**
         * @brief
         * DeactivateDatacall API. This Api is used to tear down / deactivate a data call.
         *
         * @param cid Call id is an int32_t variable. Every call which is successfully brought up is associated with a cid which indicates call identifier..
         * @param reason reason is a boolean variable. True indicates Radio Shutdown requested and False indicates no specific reason.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for DeactivateDatacall.
         * Prototype of the callback: std::fustd::function<void(#RIL_Errno)> DeactivateDataCallCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call tear down / deactivation is SUCCESS/FAILURE would be inidcated in the Deactivatedatacall Callback.
         *
         *
        */
        Status deactivateDataCall(const int32_t cid, const bool reason, const DeactivateDataCallCallback& cb);
        /**
         * @brief
         * DataCallListChanged Indication Registration API. Client needs to register for the API to receive any DataCallListChanged
         * Indication. If there in any change, ex: ip address update, MTU update, change in DNS address etc. in the data call list,
         * an indication will be received via the callback.
         * The list is the complete list of current data contexts including new contexts that have been activated.
         * A data call is only removed from this list when the client sends a deactivateDataCall request or the radio is powered off/on
         * modem/network deactivates the call.
         *
         * @note It is advisable to register for this indication before SetupDataCall request
         * @param indicationHandler This a function pointer of type DataCallListChangedIndicationHandler.
         * Prototype of the handler: std::function<void(const #RIL_Data_Call_Response_v11 [], size_t)> DataCallListChangedIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerDataCallListChangedIndicationHandler(const DataCallListChangedIndicationHandler& indicationHandler);
        /**
         * @brief
         * SetInitialAttachApn API is used to set the apn with which initial attach is requested by client.
         * If the Profile is present on modem with same apn, then the non matched parameters are updated.
         * If there is no match with same apn, a new profile is created on modem & a request is sent to modem to trigger a Detach and re-attach with the updated APN.
         * @note Only LTE RAT requires the Inital Attach. Its not applicable for other RATs.
         * @param dataProfile A struct of type RIL_DataProfileInfo which provides the profile for the intital attach.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetInitalAttach.
         * Prototype of the callback: std::function<void(#RIL_Errno)> SetInitialAttachApnCallback .
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Initial Attach is SUCCESS/FAILURE would be inidcated in the Initial Attach Callback.
         *
         *
        */
        Status setInitialAttachApn(const RIL_DataProfileInfo& dataProfile, const SetInitialAttachApnCallback &cb);
        /**
         * @brief
         * Set Data Profile API. This Api is used to Set Data profiles on Modem.
         * If the Profile is present on modem with same apn, then the non matched parameters are updated.
         * If there is no match with same apn, a new profile is created on modem.
         * Some situations for client invoking the API : addition of an APN, SIM enablement, powerup etc.
         * @param dataProfile An array of struct of type RIL_DataProfileInfo which provides the list of profiles.
         * @param size unint32_t type variable which specifies the total number of profiles.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetDataProfile.
         * Prototype of the callback: std::function<void( #RIL_Errno)> SetDataProfileCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetDataProfile is SUCCESS/FAILURE would be inidcated in the SetDataProfile Callback.
         *
         *
        */
        Status setDataProfile(const RIL_DataProfileInfo* dataProfile, const uint32_t size, const SetDataProfileCallback &cb);
        /**
         * @brief
         * Get Data Call List API. This API is used to get list of active Data Calls.
         * Returns the data call list.
         * An entry is added when a setupdatacall request is issued.
         * An entry is removed when deactivatedataCall request is sent from client or when call is teared down from modem/network.
         *
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for GetDataCallList.
         * Prototype of the callback: std::function<void(const #RIL_Errno, const #RIL_Data_Call_Response_v11 *, const size_t)> GetDataCallListCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * GetDataCallList SUCCESS/FAILURE would be inidcated in the GetDataCallList Callback.
         *
         *
        */
        Status getDataCallList(const GetDataCallListCallback &cb);
        /**
         * @brief
         * SetDataThrottling API.
         * @note This API is only supported on Android
         *
         * @param action enum of type #RIL_DataThrottlingAction which specifies the throttling action
         * @param completionDurationMs time in which the throttling action has to be achieved
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetDataThrottling.
         * Prototype of the callback: function<void(const #RIL_Errno)> SetDataThrottlingCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetDataThrottling SUCCESS/FAILURE would be indicated in the SetDataThrottlingCallback.
         *
         *
        */
        Status setDataThrottling(RIL_DataThrottlingAction action, int64_t completionDurationMillis, const SetDataThrottlingCallback& cb);
        Status StartLceData(const int32_t interval, const int32_t mode, const StartLceDataCallback &cb);
        Status StopLceData(const StopLceDataCallback &cb);
        Status PullLceData(const PullLceDataCallback &cb);
        /**
         * @brief
         * SetLinkCapFilter API. Link Capacity Indication is turned on/off using this API.
         *
         * @param enable_bit enum of type #RIL_ReportFilter which specifies the enable bit
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetLinkCapFilter.
         * Prototype of the callback: std::function<void( #RIL_Errno, const int*)> SetLinkCapFilterCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetLinkCapFilter SUCCESS/FAILURE would be inidcated in the SetLinkCapFilter Callback.
         *
         *
        */
        Status SetLinkCapFilter(const RIL_ReportFilter enable_bit, const SetLinkCapFilterCallback &cb);
        /**
         * @brief
         * SetLinkCapRptCriteria API. Setting Link Reporting Criteria is set via this API.
         *
         * @param criteria A sturct of type RIL_LinkCapCriteria which is used to pass the link report criteria.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetLinkCapFilter.
         * Prototype of the callback: std::function<void( #RIL_Errno, const #RIL_LinkCapCriteriaResult *)> SetLinkCapRptCriteriaCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetLinkCapRptCriteria SUCCESS/FAILURE would be inidcated in the SetLinkCapRptCriteria Callback.
         *
         *
        */
        Status SetLinkCapRptCriteria(const RIL_LinkCapCriteria &criteria, const SetLinkCapRptCriteriaCallback &cb);
        /**
         * @brief
         * GetDataNrIconType API. This API helps in getting NR/5G icon type.
         *
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for GetDataNrIconType.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #five_g_icon_type *)> GetDataNrIconTypeCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * GetDataNrIconType SUCCESS/FAILURE would be inidcated in the GetDataNrIconTypeCallback.
         *
         *
        */
        Status GetDataNrIconType(const GetDataNrIconTypeCallback &cb);
        /**
         * @brief
         * Pco Data Change Indication Registration API. Client needs to register for the API to receive any RILPCODataChanged
         * Indication. If there in any change in the Protocol Config Option, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILPCODataChangeIndicationHandler.
         * Prototype of the handler: std::function<void(const #RIL_PCO_Data)> RILPCODataChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerPcoDataChangeHandler(const RILPCODataChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * LCE Data Change Indication Registration API. Client needs to register for the API to receive any indication for change in LCE Data.
         * If there in any change in the Link Capacity, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILLCEDataChangeIndicationHandler.
         * Prototype of the handler: std::functistd::function<void(const #RIL_LinkCapacityEstimate)> RILLCEDataChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerLCEDataChangeHandler(const RILLCEDataChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * Data NrIcon Change Indication Registration API. Client needs to register for the API to receive any indication for change in DataNrIcon Type.
         * If there in any change in the NR/5g icon type, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILDataNrIconChangeIndicationHandler.
         * Prototype of the handler: std::function<void(const Status, const #five_g_icon_type )> RILDataNrIconChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerDataNrIconChangeHandler(const RILDataNrIconChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * CarrierInfoForImsi Indication Registration API. Client needs to register for the API to receive any indication for change in CarrierInfo for IMSI.
         * If there in any change in the Carrier Info for IMSI, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type CarrierInfoForImsiHandler.
         * Prototype of the handler: std::function<void()> CarrierInfoForImsiHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerCarrierInfoForImsiHandler(const CarrierInfoForImsiHandler &indicationHandler);
        /**
         * @brief
         * SetPreferredDataModem API. This API is used to set preferred SIM for Data Usage.
         *
         * @param modemId int32_t variable which specifies preferred SIM for Data Usage.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetPreferredDataModem.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> SetPreferredDataModemCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetPreferredDataModem SUCCESS/FAILURE would be inidcated in the SetPreferredDataModem Callback.
         *
         *
        */
        Status SetPreferredDataModem(const int32_t modemId, const SetPreferredDataModemCallback &cb);
        /**
         * @brief
         * RegisterDataUnsolIndication API. This API is used to Register for UI Change Indication.
         *
         * @param Filter int32_t type variable. Filter determines whether to register/deregister(1/0) a particular Unsol Indication.
         *        1st bit - UI INFO Indication
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StopKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> RegisterForUIChangeIndCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * RegisterDataUnsolIndication SUCCESS/FAILURE would be inidcated in the RegisterDataUnsolIndication Callback.
         *
         *
        */
        Status registerDataUnsolIndication(const int32_t filter, const RegisterDataUnsolIndicationCallback &cb);
        /**
         * @brief
         * StartKeepAlive API. This API is used to start Keep Alive. Keep Alive is a feature to
         * prevent automatic termination of connection. In specific intervals, a data packet is sent
         * to the server to keep the connection established.
         *
         * @param request A struct of type RIL_KeepaliveRequest to provide source/ destination IP/port and desired interval in ms.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StartKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *, const uint32_t*, const #RIL_KeepaliveStatus_t *)> StartKeepAliveCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * StartKeepAlive SUCCESS/FAILURE would be inidcated in the StartKeepAlive Callback.
         *
         *
        */
        Status StartKeepAlive(const RIL_KeepaliveRequest request, const StartKeepAliveCallback &cb);
        /**
         * @brief
         * Radio Keep Alive Indication Registration API. Client needs to register for the API to receive any indication for change in Radio KeepAlive Status.
         * If there in any change in the KeepAlive Status, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILDataRadioKeepAliveHandler.
         * Prototype of the handler: std::function<void(const Status, const uint32_t, const #RIL_KeepaliveStatusCode)> RILDataRadioKeepAliveHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         */
        Status registerRadioKeepAliveHandler(const RILDataRadioKeepAliveHandler &indicationHandler);
        /**
         * @brief
         * Physical Channel Config Indication Registration API. Client needs to register for the API to receive any indication for change in Physical Config.
         * If there in any change in the Physical Channel Config, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type PhysicalConfigStructHandlerHandler.
         * Prototype of the handler: std::function<void(const Status, const std::vector< #RIL_PhysicalChannelConfig > )> PhysicalConfigStructHandlerHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerPhysicalConfigStructHandler(const PhysicalConfigStructHandlerHandler &indicationHandler);
        /**
         * @brief
         * StopKeepAlive API. This API is used to stop a keep alive connection.
         *
         * @param handle int32_t type variable. Keep Alive handle which needs to be stopped.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StopKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> StopKeepAliveCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * StopKeepAlive SUCCESS/FAILURE would be inidcated in the StopKeepAlive Callback.
         *
         *
        */
        Status StopKeepAlive(const int32_t handle, const StopKeepAliveCallback &cb);
        /**
         * @brief
         * CarrierInfoForImsiEncryption API. This API is used to set carrier info for "IMSI" Encrytion.
         *
         * @param carrier A struct RIL_CarrierInfoForImsiEncryption for passing all information required for carrier encryption.
         * @param type an enum of type #RIL_PublicKeyType which specifies the type of public key.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for CarrierInfoForImsiEncryption.
         * Prototype of the callback: std::function<void(const #RIL_Errno, const Status, const #RIL_Errno *)> CarrierInfoForImsiEncryptionCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * CarrierInfoForImsiEncryption SUCCESS/FAILURE would be inidcated in the CarrierInfoForImsiEncryption Callback.
         *
         *
        */
        Status CarrierInfoForImsiEncryption(const RIL_CarrierInfoForImsiEncryption carrier, const RIL_PublicKeyType type,
        const CarrierInfoForImsiEncryptionCallback &cb);

        /**
         * @brief
         * GetSlicingConfig API. This API is used to get Slicing Config Info from Modem
         *
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for GetSlicingConfig.
         * Prototype of the callback: std::function<void(const #RIL_Errno, const Status, const #rilSlicingConfig *)> GetSlicingConfigCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * GetSlicingConfig SUCCESS/FAILURE would be inidcated in the GetSlicingConfig Callback.
         *
         *
        */
        Status GetSlicingConfig(const GetSlicingConfigCallback &cb);

        /**
         * @brief
         * SlicingConfigChanged Indication Registration API. This indication is used to notify clients when slicing
         * config is changed
         *
         * @param indicationHandler This a function pointer of type SlicingConfigChangedIndicationHandler.
         * Prototype of the handler: std::function<void(#rilSlicingConfig&)> SlicingConfigChangedIndicationHandler.
         * The handler parameter is a string representing the apn which is no longer throttled.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerSlicingConfigChangedIndicationHandler(const SlicingConfigChangedIndicationHandler& indicationHandler);

        /**
         * @brief
         * UnthrottleApn Indication Registration API. This indication is used to notify clients when an apn
         * from a previous setupDataCall request is no longer throttled.
         *
         * @param indicationHandler This a function pointer of type UnthrottleApnIndicationHandler.
         * Prototype of the handler: std::function<void(std::string&)> UnthrottleApnIndicationHandler.
         * The handler parameter is a string representing the apn which is no longer throttled.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerUnthrottleApnIndicationHandler(const UnthrottleApnIndicationHandler& indicationHandler);
        /**
         * @brief
         * UnthrottleApn Indication Registration API. This indication is used to notify clients when an apn
         * from a previous setupDataCall request is no longer throttled.
         *
         * @param indicationHandler This a function pointer of type UnthrottleApnTdIndicationHandler.
         * Prototype of the handler: std::function<void(const RIL_DataProfileInfo&)> UnthrottleApnTdIndicationHandler.
         * The handler parameter is a string representing the apn which is no longer throttled.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerUnthrottleApnTdIndicationHandler(const UnthrottleApnTdIndicationHandler& indicationHandler);

        Status captureRilDataDump(const GetRilDataDumpCallback &cb);

        /**
         * @brief Request to set the TTY mode
         *
         * @param mode     TTY mode.
         *                 0 for TTY off.
         *                 1 for TTY Full.
         *                 2 for TTY HCO (hearing carryover).
         *                 3 for TTY VCO (voice carryover).
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetTtyModeCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setTtyMode(const int mode, const SetTtyModeCallBack& cb);

        /**
         * @brief Request the setting of TTY mode
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetTtyModeCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getTtyMode(const GetTtyModeCallBack& cb);

        /**
         * @brief Request to set the preferred voice privacy mode used in voice scrambling
         *
         * @param mode     Voice privacy mode
         *                   0 for Standard Privacy Mode (Public Long Code Mask)
         *                   1 for Enhanced Privacy Mode (Private Long Code Mask)
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SetCdmaVoicePrefModeCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCdmaVoicePrefMode(const int mode, const SetCdmaVoicePrefModeCallBack& cb);

        /**
         * @brief Request the setting of preferred voice privacy mode
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetCdmaVoicePrefModeCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getCdmaVoicePrefMode(const GetCdmaVoicePrefModeCallBack& cb);

        /**
         * @brief Send CDMA FLASH
         *
         * @param flash    FLASH string
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SendCdmaFlashCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendCdmaFlash(const std::string& flash, const SendCdmaFlashCallBack& cb);

        /**
         * @brief Send DTMF string
         *
         * @param dtmf     DTMF string
         * @param on       DTMF ON length in milliseconds, or 0 to use default
         * @param off      DTMF OFF length in milliseconds, or 0 to use default
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref SendCdmaBurstDtmfCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendCdmaBurstDtmf(const std::string& dtmf, int32_t on, int32_t off,
                                 const SendCdmaBurstDtmfCallBack& cb);

        /**
         * @brief Request current IMS registration state
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetImsRegStateCallBack
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getImsRegState(const GetImsRegStateCallBack& cb);


        /**
         * @brief Register for new SMS on SIM indication.
         * The callback function will be invoked when new SMS has been stored on SIM card
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref NewSmsOnSimIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerNewSmsOnSimIndicationHandler(
            const NewSmsOnSimIndicationHandler& indicationHandler);

        /**
         * @brief Register for SIM SMS storage full indication.
         * The callback function will be invoked to indicates that SMS storage on the SIM is full.
         * Sent when the network attempts to deliver a new SMS message.
         * Messages cannot be saved on the SIM until space is freed.
         * In particular, incoming Class 2 messages cannot be stored.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref SimSmsStorageFullIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerSimSmsStorageFullIndicationHandler(
            const SimSmsStorageFullIndicationHandler& indicationHandler);

        /**
         * @brief Register for new SMS status report indication.
         * The callback function will be invoked when new SMS status report is received.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref NewSmsStatusReportIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerNewSmsStatusReportIndicationHandler(
            const NewSmsStatusReportIndicationHandler& indicationHandler);

        /**
         * @brief Register for SMS storage on RUIM full indication.
         * The callback function will be invoked to indicates that SMS storage on the RUIM is full.
         * Messages cannot be saved on the RUIM until space is freed.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CdmaSmsRuimStorageFullIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCdmaSmsRuimStorageFullIndicationHandler(
            const CdmaSmsRuimStorageFullIndicationHandler& indicationHandler);

        /**
         * @brief Scans for available networks
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref QueryAvailableNetworksCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryAvailableNetworks(const QueryAvailableNetworksCallback& cb);

        /**
         * @brief Request to start a network scan with specified radio access networks
         * with frequency bands and/or channels
         *
         * @param req      Network scan request parameters.  Refer @ref RIL_NetworkScanRequest
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status startNetworkScan(const RIL_NetworkScanRequest& req, const VoidResponseHandler& cb);

        /**
         * @brief Request to stops an ongoing network scan
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status stopNetworkScan(const VoidResponseHandler& cb);

        /**
         * @brief Request to get which bands the modem's background scan is acting on.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetSystemSelectionChannelsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getSystemSelectionChannels(const GetSystemSelectionChannelsCallback& cb);

        /**
         * @brief Set System Selection parameters
         *
         * @param req      System selection parameter.  Refer @ref RIL_SysSelChannels
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setSystemSelectionChannels(const RIL_SysSelChannels& req, const VoidResponseHandler& cb);

        /**
         * @brief Register for network scan indications.
         * The callback function will be invoked to notify the incremental result for
         * the network scan which is started by @ref startNetworkScan, notifies the
         * results, status or errors.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref NetworkScanIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerNetworksScanIndicationHandler(
            const NetworkScanIndicationHandler& indicationHandler);

        /**
         * @brief Register for emergency number list change.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref EmergencyListIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerEmergencyListIndicationHandler(
            const EmergencyListIndicationHandler& indicationHandler);

        /**
         * @brief Request the setting of GSM/WCDMA Cell Broadcast SMS config.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GsmGetBroadcastSmsConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status GsmGetBroadcastSmsConfig(const GsmGetBroadcastSmsConfigCallback& cb);

        /**
         * @brief Set GSM/WCDMA Cell Broadcast SMS config
         *
         * @param config   Array of configs. Refer @ref RIL_GSM_BroadcastSmsConfigInfo
         * @param len      Length of config array
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status GsmSetBroadcastSMSConfig(RIL_GSM_BroadcastSmsConfigInfo *config, size_t len,
                const VoidResponseHandler &cb);

        /**
         * @brief Enable or disable the reception of GSM/WCDMA Cell Broadcast SMS
         *
         * @param disable  Indicates activate or turn off reception of GSM/WCDMA Cell broadcast SMS
         *                  0 - Activate, 1 - Turn off
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status GsmSmsBroadcastActivation(uint32_t disable, const VoidResponseHandler &cb);
        /**
         * @brief Indication Handler to handle Stk Service Running indication
         *
         * @param cb Callback to Handle Indication
         *           Callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
         *
         *  @retval SUCCESS/FAILURE
         *  @note   SUCCESS If request is Successfully placed into the RIL.
         *          FAILURE if request was not placed into the RIL.
         */
        Status ReportStkServiceIsRunning(const VoidResponseHandler &cb);
       /**
        *  @brief Request to get ATR from card
        *
        *  @param slot  physical slot-id of the card
        *
        *  @param cb Callback to handle the response for request
        *            callback prototype(typedef std::function<void(#RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status GetAtr(uint8_t slot, const CharStarSizeResponseHandler &cb);
        /**
        *  @brief Handling Call Setup Request from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkHandleCallSetupRequestedFromSim(int32_t accept, const VoidResponseHandler &cb);
        /**
        *  @brief Sending Envelope command to Upper layers
        *
        *  @param command Conatins Envelope Data
        *
        *  @param size  Length of the Envelope command
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkSendEnvelopeCommand(const char *command, size_t size, const CharStarSizeResponseHandler &cb);
        /**
        *  @brief Sending Terminal Response to Card
        *
        *  @param terminalResponse Conatins Terminal Response data
        *
        *  @param size  Length of the Terminal Response
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkSendTerminalResponse(const char *terminalResponse, size_t size, const VoidResponseHandler &cb);

        // IMS APIs
        /**
         * @brief Request current IMS registration state
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetRegistrationStateCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetRegistrationState(const ImsGetRegistrationStateCallback& cb);

        /**
         * @brief Request to get the status of IMS Services
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryServiceStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryServiceStatus(const ImsQueryServiceStatusCallback& cb);

        /**
         * @brief Request current IMS Registration/Deregistration
         *
         * @param state   IMS registration state.
         *                Refer @ref RIL_IMS_RegistrationState
         * @param cb      Callback to get the response of the request.
         *                Refer @ref ImsRegistrationChangeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRegistrationChange(const RIL_IMS_RegistrationState& state,
                                     const ImsRegistrationChangeCallback& cb);

        /**
         * @brief Request to set the status of an IMS Service
         *
         * @param serviceStatusInfoLen  Length of the service status info list
         * @param serviceStatusInfoList Service status info list
         *           Refer @ref RIL_IMS_ServiceStatusInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetServiceStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetServiceStatus(const size_t, const RIL_IMS_ServiceStatusInfo **,
                                   const ImsSetServiceStatusCallback& cb);

        /**
         * @brief Get IMS Subscription specific configuration
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetImsSubConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetImsSubConfig(const ImsGetImsSubConfigCallback& cb);

        /**
         * @brief Initiate voice call
         *
         * This API is deprecated; use @ref dial(const RIL_DialParams& dialParams, const
         * VoidResponseHandler& cb).
         *
         * @param dial Dial info.  Refer @ref RIL_IMS_Dial
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsDialCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsDial(const RIL_IMS_Dial& dial, const ImsDialCallback& cb);

        /**
         * @brief Answer incoming call
         *
         * This API is deprecated; use @ref answer(const RIL_Answer& answer, const
         * VoidResponseHandler& cb)
         *
         * @param answer Answer info.  Refer @ref RIL_IMS_Answer
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsAnswerCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsAnswer(const RIL_IMS_Answer& answer, const ImsAnswerCallback& cb);

        /**
         * @brief Hang up a specific line
         *
         * @param hangup Hang up info.  Refer @ref RIL_IMS_Hangup.
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsHangupCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsHangup(const RIL_IMS_Hangup& hangup, const ImsHangupCallback& cb);

        /**
         * @brief  Send a DTMF tone.
         *
         * @param digit     A single character with one of 12 values: 0-9,*,#
         * @param cb        Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsDtmf(char digit, const VoidResponseHandler &cb);

        /**
         * @brief Start playing a DTMF tone. Continue playing DTMF tone until
         *        @ref imsDtmfStop is received.
         *
         * @param digit     A single character with one of 12 values: 0-9,*,#
         * @param cb        Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsDtmfStart(char digit, const VoidResponseHandler &cb);

         /**
         * @brief Stop playing a currently playing DTMF tone.
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsDtmfStop(const VoidResponseHandler &cb);

         /**
         * @brief Request to upgrade/downgrade existing IMS calls
         *
         * This API is deprecated; use @ref modifyCallInitiate instead.
         *
         * @param modifyInfo Modify call info. Refer @ref RIL_IMS_CallModifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsModifyCallInitiateCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsModifyCallInitiate(const RIL_IMS_CallModifyInfo& modifyInfo,
                                     const ImsModifyCallInitiateCallback& cb);

         /**
         * @brief Request to upgrade/downgrade existing IMS calls
         *
         * @param modifyInfo Modify call info. Refer @ref RIL_CallModifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref modifyCallInitiateCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status modifyCallInitiate(const RIL_CallModifyInfo& modifyInfo, const VoidResponseHandler& cb);
         /**
         * @brief Request to accept the call upgrade of existing IMS calls
         *
         * This API is deprecated, use @ref modifyCallConfirm instead
         *
         * @param modifyInfo Modify call info. Refer @ref RIL_IMS_CallModifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsModifyCallConfirmCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsModifyCallConfirm(const RIL_IMS_CallModifyInfo& modifyInfo,
                                    const ImsModifyCallConfirmCallback& cb);
         /**
         * @brief Request to accept the call upgrade of existing IMS calls
         *
         * @param modifyInfo Modify call info. Refer @ref RIL_CallModifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status modifyCallConfirm(const RIL_CallModifyInfo& modifyInfo, const VoidResponseHandler& cb);

         /**
         * @brief Cancel call modify request
         *
         * This API is deprecated; use @ref cancelModifyCall instead.
         *
         * @param callId Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsCancelModifyCallCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsCancelModifyCall(const uint32_t& callId, const ImsCancelModifyCallCallback& cb);

         /**
         * @brief Cancel call modify request
         *
         * This API is deprecated; use @ref cancelModifyCall instead.
         *
         * @param callId Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status cancelModifyCall(const uint32_t& callId, const VoidResponseHandler& cb);
         /**
         * @brief Request to add a participant to a call (to the current active call or
         *        to the held call if there
         * This API is deprecated; use @ref addParticipant instead.
         *
         * @param address Address
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsAddParticipantCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsAddParticipant(const char* address, const ImsAddParticipantCallback& cb);

         /**
         * @brief Request to add a participant to a call (to the current active call or
         *        to the held call if there
         *
         * @param address Address
         * @param cb Callback to get the response of the request.
         *           Refer @ref SipErrorInfoResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status addParticipant(const std::string address, const SipErrorInfoResponseHandler& cb);

         /**
         * @brief Sets value for Ims Config item
         *
         * @param config IMS Config info. Refer @ref RIL_IMS_ConfigInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetImsConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetImsConfig(const RIL_IMS_ConfigInfo& config, const ImsSetImsConfigCallback& cb);

         /**
         * @brief Gets value for Ims Config item
         *
         * @param config IMS Config info. Refer @ref RIL_IMS_ConfigInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetImsConfigCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetImsConfig(const RIL_IMS_ConfigInfo& config, const ImsGetImsConfigCallback& cb);

         /**
         * @brief Requests the call forward settings
         * This API is deprecated; use the below API instead.
         * Status queryCallForwardStatus(const uint32_t reason,
         *                               const uint32_t serviceClass,
         *                               const QueryCallForwardStatusCb& cb);
         *
         * @param cfInfo  Call Forward request info.  Refer @ref RIL_IMS_CallForwardInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryCallForwardStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                         const ImsQueryCallForwardStatusCallback& cb);

         /**
         * @brief Configure call forward rule
         * This API is deprecated; use the below API instead.
         * Status setCallForward(const RIL_CallForwardParams& callFwdInfo,
         *                       const SetCallForwardStatusCb& cb);
         *
         * @param cfInfo Call forward info to set.  Refer @ref RIL_IMS_CallForwardInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetCallForwardStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                       const ImsSetCallForwardStatusCallback& cb);

        /**
         * @brief For transfering the call to a third party.
         * The call can be transferred to third party (Transfer target) by
         * passing the transfer target address (blind or assured transfer)
         * or by passing the call id of the already established call with
         * transfer target (consultative transfer).
         * In success case, the call/calls will be disconnected.
         *
         * This API is deprecated; use explicitCallTransfer(
         * const RIL_ExplicitCallTransfer& explicitCall, const VoidResponseHandler& cb);
         *
         * @param explicitCall Explicit call transfer info.
         *                     Refer @ref RIL_IMS_ExplicitCallTransfer
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsExplicitCallTransferCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsExplicitCallTransfer(const RIL_IMS_ExplicitCallTransfer& explicitCall,
                                       const ImsExplicitCallTransferCallback& cb);

        /**
         * @brief Gets current CLIR status
         *
         * This API is deprecated; use getClir(const GetClirCb& cb);
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetClirCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetClir(const ImsGetClirCallback& cb);

        /**
         * @brief Sets the Clir parameter
         *
         * This API is deprecated; use setClir(const int* clir, const SetClirCallback& cb);
         *
         * @param clirInfo  CLIR info
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetClirCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetClir(const RIL_IMS_SetClirInfo& clirInfo,
                          const ImsSetClirCallback& cb);

        /**
         * @brief Queries the status of the CLIP supplementary service
         * (for MMI code "*#30#")
         * This API is deprecated; use queryClip(const QueryClipCb& cb);
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryClipCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryClip(const ImsQueryClipCallback& cb);

        /**
         * @brief Enables/disables supplementary service related notifications
         * from the network.
         * This API is deprecated; use setSuppSvcNotification(const bool enabled, const
         * VoidResponseHandler& cb);
         *
         * @param serviceStatus service status.  Refer @ref RIL_IMS_ServiceClassStatus
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetSuppSvcNotificationCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetSuppSvcNotification(const RIL_IMS_ServiceClassStatus& serviceStatus,
                                         const ImsSetSuppSvcNotificationCallback& cb);

         /**
         * @brief deflect the call to specific number mentioned by user
         * This API is deprecated; use @ref deflectCall instead.
         *
         * @param deflectCall  Deflect call request details.  Refer @ref RIL_IMS_DeflectCallInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsDeflectCallCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsDeflectCall(const RIL_IMS_DeflectCallInfo& deflectCall,
                              const ImsDeflectCallCallback& cb);

         /**
         * @brief Request to set UI TTY Setting
         *
         * This API is deprecated; use @ref sendUiTtyMode instead.
         *
         * @param ttyInfo  TTY info.  Refer @ref RIL_IMS_TtyNotifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSendUiTtyModeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSendUiTtyMode(const RIL_IMS_TtyNotifyInfo& ttyInfo,
                                const ImsSendUiTtyModeCallback& cb);

         /**
         * @param ttyInfo  TTY info.  Refer @ref RIL_TtyNotifyInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status sendUiTtyMode(const RIL_TtyModeType& ttyMode,
                                const VoidResponseHandler &cb);
        /**
         * @brief Request to activate/deactivate/query the supplementary service of an IMS Service
         *
         * This API is deprecated; use @ref suppSvcStatus instead.
         *
         * @param suppSvc Supplementary service request.  Refer @ref RIL_IMS_SuppSvcRequest
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSuppSvcStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSuppSvcStatus(const RIL_IMS_SuppSvcRequest& suppSvc,
                                const ImsSuppSvcStatusCallback& cb);

        /**
         * @brief
         * Conference holding and active calls
         *
         * This API is deprecated; use @ref conference(const SipErrorInfoResponseHandler& cb);
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsRequestConferenceCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRequestConference(const ImsRequestConferenceCallback& cb);

         /**
         * @brief
         * Request to put the active call on HOLD
         *
         * This API is deprecated; use @ref hold instead
         *
         * @param callId  Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsRequestHoldCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRequestHoldCall(uint32_t callId, const ImsRequestHoldCallback& cb);

         /**
         * @brief
         * Request to put the active call on HOLD
         *
         * @param callId  Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref SipErrorInfoResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status hold(uint32_t callId, const SipErrorInfoResponseHandler& cb);

         /**
         * @brief
         * Request to resume a HELD call.
         *
         * This API is deprecated; use @ref resume instead
         *
         * @param callId Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsRequestResumeCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRequestResumeCall(uint32_t callId, const ImsRequestResumeCallback& cb);

         /**
         * @brief
         * Request to resume a HELD call.
         *
         * @param callId Call Index
         * @param cb Callback to get the response of the request.
         *           Refer @ref SipErrorInfoResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status resume(uint32_t callId, const SipErrorInfoResponseHandler& cb);

         /**
         * @brief Request to exit emergency callback mode.
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsExitECBMCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRequestExitEmergencyCallbackMode(const ImsExitECBMCallback& cb);

         /**
         * @brief set COLR
         *
         * This API is deprecated; use @ref setColr instead
         *
         * @param colr COLR info.  Refer @ref RIL_IMS_ColrInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetColrCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetColr(const RIL_IMS_ColrInfo& colr, const ImsSetColrCallback& cb);

         /**
         * @brief Set COLR
         *
         * @param colr COLR info.  Refer @ref RIL_IpPresentation
         * @param cb Callback to get the response of the request.
         *           Refer @ref SipErrorInfoResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setColr(const RIL_IpPresentation& colr, const SipErrorInfoResponseHandler& cb);

         /**
         * @brief Queries COLR info.
         *
         * This API is deprecated; use @ref getColr instead
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetColrCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetColr(const ImsGetColrCallback& cb);

         /**
         * @brief Queries COLR info.
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoiceGetColrCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getColr(const VoiceGetColrCallback& cb);
         /**
         * @brief Request to register the Multi Line Identidy lines.
         *
         * @param lineInfo Line info list.
         * @param size     Length of Line info list.
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsRegisterMultiIdentityLinesCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsRegisterMultiIdentityLines(const RIL_IMS_MultiIdentityLineInfo lineInfo[],
                size_t size, const ImsRegisterMultiIdentityLinesCallback& cb);

         /**
         * @brief Quries virtual lines info.
         *
         * @param msisdn  msisdn
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryVirtualLineInfoCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryVirtualLineInfo(const char* msisdn,
                const ImsQueryVirtualLineInfoCallback& cb);

         /**
         * @brief Send SMS
         *
         * @param smsMessage  SMS message.  Refer @ref RIL_IMS_SmsMessage
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSendSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSendSms(const RIL_IMS_SmsMessage& smsMessage, const ImsSendSmsCallback& cb);

         /**
         * @brief Acknowledge successful or failed receipt of SMS previously indicated
         * via the indication cb registered with @ref registerImsIncomingSmsHandler
         *
         * @param smsAck SMS ack.  Refer @ref RIL_IMS_SmsAck
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsAckSmsCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsAckSms(const RIL_IMS_SmsAck& smsAck, const ImsAckSmsCallback& cb);

         /**
         * @brief Get SMS format.
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsGetSmsFormatCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsGetSmsFormat(const ImsGetSmsFormatCallback& cb);

         /**
         * @brief Configure current call waiting state
         *
         * @param callWaitingSettings Call waiting settings.
         *                            Refer @ref RIL_IMS_CallWaitingSettings
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsSetCallWaitingCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSetCallWaiting(const RIL_IMS_CallWaitingSettings& callWaitingSettings, const ImsSetCallWaitingCallback& cb);

         /**
         * @brief Query current call waiting state
         *
         * @param serviceClass Service class
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryCallWaitingCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryCallWaiting(uint32_t serviceClass, const ImsQueryCallWaitingCallback& cb);

         /**
         * @brief Retrieves voice capability on multi sims.
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref ImsQueryMultiSimVoiceCapabilityCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsQueryMultiSimVoiceCapability(const ImsQueryMultiSimVoiceCapabilityCallback& cb);

        /**
         * @brief Register for IMS registration state change indication.
         * The callback function will be invoked to notify the change in IMS registration state.
         * Registration failures reported through this API are temporary.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsRegistrationStateIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolRegistrationStateIndicationHandler(
            const ImsRegistrationStateIndicationHandler& indicationHandler);

        /**
         * @brief Register for IMS service status change indication.
         * The callback function will be invoked to notify the change in IMS service status.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsServiceStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolServiceStatusIndicationHandler(
            const ImsServiceStatusIndicationHandler& indicationHandler);

        /**
         * @brief Register for IMS subscription configuration change indication.
         * The callback function will be invoked to notify the change in IMS subscription
         * configuration
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsSubConfigChangeIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolSubConfigChangeIndicationHandler(
            const ImsSubConfigChangeIndicationHandler& indicationHandler);

        /**
         * @brief Register for IMS registration state change indication.
         * The callback function will be invoked to notify ring indication
         * for an incoming call on IMS.
         *
         * This API is deprecated; use @ref registerCallRingIndicationHdlr instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsCallRingIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolCallRingIndicationHandler(
            const ImsCallRingIndicationHandler& indicationHandler);

        /**
         * @brief Register for ring back tone indication.
         * The indication callback will be invoked to notify if the UE need
         * to play the ringback tone.
         *
         * This API is deprecated; use registerRingBackToneIndicationHdlr(const
         * RingBackToneIndicationHdlr& indicationHandler);
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsRingBackToneIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolRingBackToneIndicationHandler(
            const ImsRingBackToneIndicationHandler& indicationHandler);

        /**
         * @brief Register for IMS call state change indication.
         * The callback function will be invoked when call state has changed
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsCallStateChangedIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolCallStateChangedIndicationHandler(
            const ImsCallStateChangedIndicationHandler& indicationHandler);

        /**
         * @brief Register for enter emergency callback mode indication.
         * The callback function will be invoked to indicates that the radio system selection
         * module has entered to emergency callback mode.
         *
         * This API is deprecated; Use @ref registerEmergencyCallbackModeIndHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsEnterEcbmIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolEnterEcbmIndicationHandler(
            const ImsEnterEcbmIndicationHandler& indicationHandler);

        /**
         * @brief Register for emergency callback mode exit indication.
         * The callback function will be invoked to indicates that the radio system selection
         * module has exited from emergency callback mode.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsExitEcbmIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolExitEcbmIndicationHandler(
            const ImsExitEcbmIndicationHandler& indicationHandler);

        /**
         * @brief Register for modify call indication.
         *
         * This API is deprecated, use @ref registerUnsolModifyCallIndicationHandler instead
         *
         * The callback function will be invoked to notify that an upgrade of a call was triggered
         * from the remote party
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsModifyCallIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolModifyCallIndicationHandler(
            const ImsModifyCallIndicationHandler& indicationHandler);

        /**
         * @brief Register for modify call indication.
         * The callback function will be invoked to notify that an upgrade of a call was triggered
         * from the remote party
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ModifyCallIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolModifyCallIndicationHandler(
            const ModifyCallIndicationHandler& indicationHandler);

        /**
         * @brief Register for Message waiting indication on IMS.
         * The callback function will be invoked to notify clients about message waiting indication
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsMessageWaitingIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolMessageWaitingIndicationHandler(
            const ImsMessageWaitingIndicationHandler& indicationHandler);

        /**
         * @brief Register for supplementary service related notification from the network.
         *
         * This API is deprecated; use registerSuppSvcNotificationIndicationHandler(
         * const SuppSvcNotificationIndicationHdlr& indicationHandler);
         *
         * The callback function will be invoked to notify supplementary service related
         * notification from the network.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsSuppSvcNotificationIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolSuppSvcNotificationIndicationHandler(
            const ImsSuppSvcNotificationIndicationHandler& indicationHandler);

        /**
         * @brief Register for unsol on SS indication.
         * The callback function will be invoked when SS response is received when DIAL/USSD/SS is
         * changed to SS by call control.
         *
         * This API is deprecated; use registerOnSupplementaryServiceIndicationHandler(const
         * OnSupplementaryServiceIndicationHdlr& indicationHandler);
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsOnSsIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolOnSsIndicationHandler(
            const ImsOnSsIndicationHandler& indicationHandler);

        /**
         * @brief Register for VoPS state change indication.
         * The callback function will be invoked to notify the clients about vops change indication
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsVopsChangedIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolVopsChangedIndicationHandler(
            const ImsVopsChangedIndicationHandler& indicationHandler);

        /**
         * @brief Register for conference participant status information.
         * The callback function will be invoked to notify the status of the participants in the
         * conference call.
         * This API is deprecated; use @ref registerUnsolParticipantStatusInfoIndicationHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsParticipantStatusInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolParticipantStatusInfoIndicationHandler(
            const ImsParticipantStatusInfoIndicationHandler& indicationHandler);
        /**
         * @brief Register for IMS registration block status indication.
         * The callback function will be invoked to notify the permanent ims registration failure.
         * Permanent registration failure happens when modem has exhausted all its
         * retries. At this point modem will stop attempting to register until user
         * toggles the registration state or network reports favorable conditions for
         * registration.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsRegistrationBlockStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolRegistrationBlockStatusIndicationHandler(
            const ImsRegistrationBlockStatusIndicationHandler& indicationHandler);
        /**
         * @brief Register for WFC roaming mode configuration support indication.
         * The callback function will be invoked to notify if modem supports WFC roaming mode
         * configuration
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsWfcRoamingModeConfigSupportIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolWfcRoamingModeConfigSupportIndicationHandler(
            const ImsWfcRoamingModeConfigSupportIndicationHandler& indicationHandler);
        /**
         * @brief Register for USSD failure indication.
         * The callback function will be invoked to notify USSD failure indicaiton over IMS.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsUssdFailedIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolUssdFailedIndicationHandler(
            const ImsUssdFailedIndicationHandler& indicationHandler);
        /**
         * @brief Register for RTT voice info indication.
         *
         * This API is deprecated, use @ref registerUnsolVoiceInfoIndicationHandler instead
         *
         * The callback function will be invoked to sends updates for the RTT voice info which
         * indicates whether there is speech or silence from remote user
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsVoiceInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolVoiceInfoIndicationHandler(
            const ImsVoiceInfoIndicationHandler& indicationHandler);

        /**
         * @brief Register for RTT voice info indication.
         * The callback function will be invoked to sends updates for the RTT voice info which
         * indicates whether there is speech or silence from remote user
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref VoiceInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolVoiceInfoIndicationHandler(
            const VoiceInfoIndicationHandler& indicationHandler);

        /**
         * @brief Register for Auto call reject indication.
         * The callback function will be invoked to notify the auto call reject notifications.
         * This API is deprecated; use @ref registerUnsolIncomingCallAutoRejectedIndHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsVoiceAutoCallRejectionIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolAutoCallRejectionIndicationHandler(
            const ImsVoiceAutoCallRejectionIndicationHandler& indicationHandler);

        /**
         * @brief Register for incoming SMS indication.
         * The callback function will be invoked when new incoming SMS is recieved.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsIncomingSmsHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsIncomingSmsHandler(const ImsIncomingSmsHandler& handler);
        /**
         * @brief Register for SMS status report.
         * The callback function will be invoked when new SMS status report is recieved.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsSmsStatusReportHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsSmsStatusReportHandler(const ImsSmsStatusReportHandler& handler);
        /**
         * @brief Register for VICE info indication.
         *
         * This API is deprecated, use @ref registerViceInfoHandler instead
         *
         * The callback function will be invoked to notify VICE info indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsViceInfoHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsViceInfoHandler(const ImsViceInfoHandler& handler);

        /**
         * @brief Register for VICE info indication.
         * The callback function will be invoked to notify VICE info indication.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ViceInfoHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerViceInfoHandler(const ViceInfoHandler& handler);
        /**
         * @brief Register for TTY notification.
         * The callback function will be invoked to notify information about TTY
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsTtyNotificationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsTtyNotificationHandler(const ImsTtyNotificationHandler& handler);

         /**
         * @brief Register for TTY notification.
         * The callback function will be invoked to notify information about TTY
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref TtyNotificationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerTtyNotificationHandler(const TtyNotificationHandler& handler);

        /**
         * @brief Register for hand over indication.
         *
         * This API is deprecated; use @ref registerUnsolHandoverIndicationHandler instead.
         *
         * The callback function will be invoked to notify that a call has been handed over
         * between technologies
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsHandoverIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolHandoverIndicationHandler(
            const ImsHandoverIndicationHandler& indicationHandler);

        /**
         * @brief Register for conference status update indicaiton.
         *
         * This API is deprecated; use @ref registerUnsolRefreshConferenceInfo instead.
         *
         * The callback function will be invoked to notify the conference status updates
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsRefreshConferenceInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolRefreshConferenceInfo(
            const ImsRefreshConferenceInfoIndicationHandler& indicationHandler);

        /**
         * @brief Register for conference status update indicaiton.
         * The callback function will be invoked to notify the conference status updates
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RefreshConferenceInfoIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolRefreshConferenceInfo(
            const RefreshConferenceInfoIndicationHandler& indicationHandler);

        /**
         * @brief Register for multi idendity pending info indication.
         * The callback function will be invoked to notify the multi idendity pending info
         * indication
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref VoidIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolMultiIdentityPendingInfo(
            const VoidIndicationHandler &indicationHandler);

        /**
         * @brief Register for multi identity registration status indication.
         * The callback function will be invoked to notify the registration status of
         * Multi-line identity
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref MultiIdentityRegistrationStatusHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsUnsolMultiIdentityRegistrationStatus(
            const MultiIdentityRegistrationStatusHandler indicationHandler);

        /**
         * @brief Register for voice capability change on multi sim case.
         * The callback function will be invoked to notify the voice capability
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsMultiSimVoiceCapabilityIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsMultiSimVoiceCapabilityChanged(
            const ImsMultiSimVoiceCapabilityIndicationHandler& indicationHandler);

        /**
         * @brief Register for pre alerting call info indication.
         * The callback function will be invoked to notify the pre alerting call information.
         * This API is deprecated; use @ref registerUnsolPreAlertingCallInfoAvailableIndHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @refImsPreAlertingCallInfoAvailableHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsPreAlertingCallInfoAvailable(
            const ImsPreAlertingCallInfoAvailableHandler& indicationHandler);

        /**
         * @brief Register for USSD indication.
         * The callback function will be invoked when a new USSD message is received.
         * This API is deprecated; Use @ref registerUnsolOnUssdIndicationHandler instead.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ImsOnUssdIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerImsOnUssdIndicationHandler(const ImsOnUssdIndicationHandler& indicationHandler);
        /* QCRIL_UIM Request Message APIs */
        /**
        *  @brief Request IMSI value from the card
        *
        *  @param aid  Aid related to Specific application in the card.
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(callback typedef std::function<void(#RIL_Errno, const char *)> GetImsiReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simGetImsiReq(const char *aid, const GetImsiReqCallback& cb);
        /**
        *  @brief Perform SIMIO Operations in card
        *
        *  @param RIL_SIM_IO_v6  Structure to hold below APDU parameters
        *                        Command.fileid,path,P1,P2,P3,
        *                        Pin2,data,aidPtr
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> IOReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simIOReq(const RIL_SIM_IO_v6& reqParams, const IOReqCallback& cb);
        /**
        *  @brief GetSimStatus Request to retrieve card info from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *)> GetImsiReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simGetSimStatusReq(const GetSimStatusReqCallback& cb);
        /**
        *  @brief simOpenChannelReq to open a logical channel from card
        *
        *  @param RIL_OpenChannelParams Structure to hold Open channel parameter AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno, const int *, size_t len)> OpenChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simOpenChannelReq(const RIL_OpenChannelParams& reqParams, const OpenChannelReqCallback& cb);
        /**
        *  @brief CloseChannel Request to clsoe already opened Logical channel
        *
        *  @param SessionId  Logical Session id
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno)> CloseChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simCloseChannelReq(const int sessionId, const CloseChannelReqCallback& cb);
        /**
        *  @brief TransmitApduBasic  Request to Send a stream of bytes to card
        *
        *  @param RIL_SIM_APDU  Structure to hold APDU parameters
        *                       Session-id,cla,instruction,p1,p2,p3,data
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> TransmitApduBasicReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simTransmitApduBasicReq(const RIL_SIM_APDU& reqParams, const TransmitApduBasicReqCallback& cb);
        /**
        *  @brief TransmitApduChannel  Request to Send a stream of bytes to card
        *
        *  @param RIL_SIM_APDU  Structure to hold APDU parameters
        *                       Session-id,cla,instruction,p1,p2,p3,data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> TransmitApduChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simTransmitApduChannelReq(const RIL_SIM_APDU& reqParams, const TransmitApduChannelReqCallback& cb);
        /**
        *  @brief ChangePin Request to change pin in the card.
        *
        *  @param reqParams  Array of pointer to hold Old pin,Newpin and AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> ChangePinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simChangePinReq(const char ** reqParams, const ChangePinReqCallback& cb);
        /**
        *  @brief ChangePin2 Request to change pin in the card.
        *
        *  @param reqParams  Pointer to hold Old pin,Newpin and AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> ChangePin2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simChangePin2Req(const char ** reqParams, const ChangePin2ReqCallback& cb);
        /**
        *  @brief EnterPuk Request to Enter Puk code to unblock the card.
        *
        *  @param reqParams  Pointer to hold Puk code, Newpin, AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPukReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPukReq(const char ** reqParams, const EnterPukReqCallback& cb);
        /**
        *  @brief EnterPuk2 Request to Enter Puk code to unblock the card.
        *
        *  @param reqParams  Pointer to hold Puk code,Newpin and AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPuk2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPuk2Req(const char ** reqParams, const EnterPuk2ReqCallback& cb);
        /**
        *  @brief EnterPin Request to Verify PIN in the card.
        *
        *  @param reqParams  Pointer to hold Pin and AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPinReq(const char ** reqParams, const EnterPinReqCallback& cb);
        /**
        *  @brief EnterPin2 Request to Verify Pin2 in the card.
        *
        *  @param reqParams  Pointer to hold Pin2,AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPin2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPin2Req(const char ** reqParams, const EnterPin2ReqCallback& cb);
        /**
        *  @brief QueryFacilitylock  Request to query FDN or PIN from card..
        *
        *  @param reqParams  Pointer to hold Password,AID,and Facility
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno, int32_t *)> QueryFacilityLockReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simQueryFacilityLockReq(const char ** reqParams, const QueryFacilityLockReqCallback& cb);
        /**
        *  @brief SetFacilitylock  Request to Set Pin or FDN to the card
        *
        *  @param reqParams  Pointer to hold Password,AID,lockstatus and Facility info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, int *)> SetFacilityLockReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simSetFacilityLockReq(const char ** reqParams, const SetFacilityLockReqCallback& cb);
        /**
        *  @brief IsimAuthenticate  Request to Perform Authentication Procedure with card
        *
        *  @param reqParams  Pointer to hold the Card Challenge Data
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *)> IsimAuthenticationReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simIsimAuthenticationReq(const char * reqParams, const IsimAuthenticationReqCallback& cb);
        /**
        *  @brief simStkSendEnvelopeWithStatus Request to send Envelope command to the card
        *
        *  @param reqParams  Pointer to hold Envelope data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> StkSendEnvelopeWithStatusReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simStkSendEnvelopeWithStatusReq(const char * reqParams, const StkSendEnvelopeWithStatusReqCallback& cb);
        /**
        *  @brief simAuthenticate  Request to Perform Authentication Procedure with card
        *
        *  @param reqParams  Pointer to hold the Card Challenge Data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> AuthenticationReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simAuthenticationReq(const RIL_SimAuthentication &reqParams, const AuthenticationReqCallback& cb);
        /**
        *  @brief EnterNetworkDepersonalization  to send Depersonalization Request to card
        *
        *  @param reqParams  Pointer to hold the NetPin Value
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status EnterNetworkDepersonalization(const char **reqParams, const EnterPinReqCallback &cb);

        /**
         * @brief
         * Get ADN record
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetAdnRecordCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status GetAdnRecord(const GetAdnRecordCallback &cb);

        /**
         * @brief
         * Update PBM ADN record
         *
         * @param record   ADN record info. Refer @ref RIL_AdnRecordInfo
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref GetAdnRecordCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status UpdateAdnRecord(const RIL_AdnRecordInfo *record, const VoidResponseHandler &cb);

        /**
        *  @brief Register for ADN init complete
        *
        *  @param cb   Callback to get the response of the request.
        *              Refer @ref VoidIndicationHandler.
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerAdnInitDone(
            const VoidIndicationHandler indicationHandler);

        /**
        *  @brief Register for ADN records change indication
        *
        *  @param cb   Callback to get the response of the request.
        *              Refer @ref VoidIndicationHandler.
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerAdnRecords(
            const VoidIndicationHandler indicationHandler);

        /**
        *  @brief Registering Indication handler for card_status_change indications
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void()> VoidIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerSimStatusChanged(
            const VoidIndicationHandler &indicationHandler);

        /**
        *  @brief Register for STK CC alpha notify indicaiton.
        *  The callback function will be called when there is an ALPHA from UICC during Call Control.
        *
        *  @param cb   Callback to get the response of the request.
        *              Refer @ref CharStarIndicationHandler.
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkCcAlphaNotify(
            const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Refresh indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const #RIL_SimRefreshResponse_v7 *)> SimRefreshIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerSimRefresh(
            const SimRefreshIndicationHandler &indicationHandler);

       /**
         * @brief Send a USSD message.
         *
         * <br>If a USSD session already exists, the message will be sent in the
         * context of that session. Otherwise, a new session should be created.
         * <br>The network reply will be reported via the indication callback registered with
         * the @ref registerImsOnUssdIndicationHandler.
         *
         * This API is deprecated; use @ref sendUssd instead.
         *
         * @param ussd     USSD request in UTF-8 format
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref ImsSendUssdCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsSendUssd(const std::string& ussd, const ImsSendUssdCallback& cb);

        /**
         * @brief Cancel the current USSD session if one exists
         *
         * This API is deprecated; use @ref cancelUssd instead.
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref ImsCancelUssdCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status imsCancelUssd(const ImsCancelUssdCallback& cb);

        /**
         * @brief Send Lpa request
         *
         * @param token    token which will be used to differentiate requests
         *
         * @param UimLpaUserReq  Structure to hold the request and its necessary parameters
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref lpauserresponseCallback
         *
         */

         Status SendLpaUserRequest(int32_t token,const UimLpaUserReq& reqparams , const lpauserresponseCallback &cb);

        /**
         * @brief Send Lpahttp request
         *
         * @param UimLpaHttpTrnsReq  Structure to hold the http request and its pay
         *                           load
         *
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref UimLpaHttpTxnCompletedResponse
         *
         */

        Status SendHttpTxnCmpltedRequest(UimLpaHttpTrnsReq &reqparams,const UimLpaHttpTxnCompletedResponse &cb );

        /**
         * @brief Register Addprofile indication
         *
         * @param indicationHandler  Handler to handle response of addprogress
         *                           indication
         *
         */

        Status registerAddProfProgressIndicationHandler(const AddProfProgressIndicationHandler &indicationHandler);

        /**
         * @brief Register HttpTxn indication
         *
         * @param indicationHandler  Handler to handle response of HttpTxn
         *                           indication
         *
         */

        Status registerLpaHttpsTxnIndicationHandler(const LpaHttpTxnIndicationHandler &indicationHandler);

        /**
         * @brief Request to query current call waiting state
         *
         * @param serviceClass Service class to query
         * @param cb           Callback to get the response of the the request.
         *                     Refer @ref QueryCallWaitingCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryCallWaiting(const uint32_t serviceClass, const QueryCallWaitingCb& cb);

        /**
         * @brief Configure current call waiting state
         *
         * @param reqData Call waiting settings.
         *                Refer @ref RIL_CallWaitingSettings
         * @param cb Callback to get the response of the request.
         *           Refer @ref SetCallWaitingCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setCallWaiting(const RIL_CallWaitingSettings& reqData, const SetCallWaitingCb& cb);

         /**
         * @brief Deflect the call to specific number mentioned by user
         *
         * @param reqData  Deflect call request details.  Refer @ref RIL_DeflectCallInfo
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status deflectCall(const RIL_DeflectCallInfo& reqData, const VoidResponseHandler& cb);

        /**
         * @brief Queries the status of the CLIP supplementary service
         * (for MMI code "*#30#")
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref QueryClipCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status queryClip(const QueryClipCb& cb);

        /**
         * @brief Gets current CLIR status
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref GetClirCb
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status getClir(const GetClirCb& cb);

        /**
         * @brief Enables/disables supplementary service related notifications
         * from the network.
         *
         * @param enabled  Enable status
         * @param cb       Callback to get the response of the the request.
         *                 Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status setSuppSvcNotification(const bool enabled, const VoidResponseHandler& cb);

        /**
         * @brief Request to activate/deactivate/query the supplementary service (call barring,
         * CLIP, COLP.)
         *
         * @param suppSvc Supplementary service request.  Refer @ref RIL_SuppSvcRequest
         * @param cb Callback to get the response of the request.
         *           Refer @ref SuppSvcStatusCallback
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status suppSvcStatus(const RIL_SuppSvcRequest& suppSvc, const SuppSvcStatusCallback& cb);

        /**
         * @brief Connects the two calls and disconnects the subscriber from both calls.
         *
         * The call can be transferred to third party (Transfer target) by
         * passing the transfer target address (blind or assured transfer)
         * or by passing the call id of the already established call with
         * transfer target (consultative transfer).
         * In success case, the call/calls will be disconnected.
         *
         * @param explicitCall Explicit call transfer info, optional for CS calls.
         *                     Refer @ref RIL_ExplicitCallTransfer
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status explicitCallTransfer(const RIL_ExplicitCallTransfer& explicitCall,
                                    const VoidResponseHandler& cb);

        /**
         * @brief Make conference with holding and active calls
         *
         * @param cb Callback to get the response of the request.
         *           Refer @ref SipErrorInfoResponseHandler
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status conference(const SipErrorInfoResponseHandler& cb);

        /**
         * @brief Initiate voice call
         *
         * @param dialParams Dial info. Refer @ref RIL_DialParams
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandle
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status dial(const RIL_DialParams& dialParams, const VoidResponseHandler& cb);

        /**
         * @brief Answer incoming call
         *
         * @param answer Answer info.  Refer @ref RIL_Answer
         * @param cb Callback to get the response of the request.
         *           Refer @ref VoidResponseHandle
         *
         * @retval Returns status of the request.
         *         SUCCESS if the request is succesfully placed to RIL.
         *         FAILURE if there is any failure
         *         The response will be notified using the callback in case of SUCCESS.
         */
        Status answer(const RIL_Answer& answer, const VoidResponseHandler& cb);

        /**
         * @brief Register for conference participant status information.
         * The callback function will be invoked to notify the status of the participants in the
         * conference call.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref ParticipantStatusInfoIndHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolParticipantStatusInfoIndicationHandler(
            const ParticipantStatusInfoIndHandler& indicationHandler);

        /**
         * @brief Register for Auto call reject indication.
         * The callback function will be invoked to notify the auto call reject notifications.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref IncomingCallAutoRejectedIndHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolIncomingCallAutoRejectedIndHandler(
            const IncomingCallAutoRejectedIndHandler& indicationHandler);

        /**
         * @brief Register for pre alerting call info indication.
         * The callback function will be invoked to notify the pre alerting call information.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @refImsPreAlertingCallInfoAvailableHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolPreAlertingCallInfoAvailableIndHandler(
            const PreAlertingCallInfoAvailableIndHandler& indicationHandler);

        /**
         * @brief Register for hand over indication.
         * The callback function will be invoked to notify that a call has been handed over
         * between technologies
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref HandoverIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolHandoverIndicationHandler(
            const HandoverIndicationHandler& indicationHandler);

        /**
         * @brief Register for secure real-time transport protocol(SRTP) status info
         * encryption indication.
         * The callback function will be invoked to indicates the secure real-time transport
         * protocol(SRTP) encryption categories.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref SrtpEncryptionStatusIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerUnsolSrtpEncryptionStatusIndicationHandler(
            const SrtpEncryptionStatusIndicationHandler& indicationHandler);

        /**
         * @brief Register for supplementary service related notification from the network.
         * The callback function will be invoked to notify supplementary service related
         * notification from the network.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref SuppSvcNotificationIndicationHdlr
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerSuppSvcNotificationIndicationHandler(
            const SuppSvcNotificationIndicationHdlr& indicationHandler);

        /**
         * @brief Register for call ring indication.
         * The callback function will be invoked to notify ring indication for an incoming call.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref CallRingIndicationHandler
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerCallRingIndicationHdlr(const CallRingIndicationHandler& indicationHandler);

        /**
         * @brief Register for ring back tone indication.
         * The indication callback will be invoked to notify if the UE need
         * to play the ringback tone.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref RingBackToneIndicationHdlr
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerRingBackToneIndicationHdlr(
            const RingBackToneIndicationHdlr& indicationHandler);

        /**
         * @brief Register for supplementary service indication.
         * The callback function will be invokved when SS response is received when DIAL/USSD/SS
         * is changed to SS by call control.
         *
         * @param indicationHandler Callback to get the response of the the request.
         *                          Refer @ref OnSupplementaryServiceIndicationHdlr
         *
         * @retval Returns status of the request. @ref Status
         */
        Status registerOnSupplementaryServiceIndicationHandler(
            const OnSupplementaryServiceIndicationHdlr& indicationHandler);

        /**
         * @brief Get the list of modules
         * @param config_name The name of the configuration to use.
         *     If null, the full list of available modules will be returned.
         *
         * @param cb Callback to get the response
         */
        Status getModuleList(
            const char *config_name,
            const GetModuleListCallback &cb);
        Status getCurrentConfig(const GetCurrentConfigCallback &cb);
        Status setCurrentConfig(const std::string &config,
                const VoidResponseHandler &cb);
        Status getConfigList( const GetConfigListCallback &cb);
        Status setCustomConfigModules(const std::vector<std::string> moduleList,
                const VoidResponseHandler &cb);
       private:
        const std::string socketPath;
        const std::string ipAddress;
        const unsigned short tcpPort = 50000;
        int socketFd = -1;
        std::mutex socketFdMutex;
        std::mutex reqMgrMutex;

#ifdef QCRIL_ENABLE_AFL
    public:
        std::unique_ptr<RequestManager> reqMgr;
#else
    private:
        std::unique_ptr<RequestManager> reqMgr;
#endif
};

