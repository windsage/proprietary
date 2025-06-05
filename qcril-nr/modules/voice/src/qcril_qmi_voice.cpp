/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_voice.cpp
  @brief   qcril qmi - voice

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI VOICE.

******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <QtiMutex.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include "telephony/ril.h"
#include "IxErrno.h"
#include "qcrili.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_voice.h"
#include "qcril_qmi_client.h"
#include "qcril_cm_ss.h"
#include "qcril_cmi.h"
#include "qcril_qmi_err_utils.h"
#include "qcril_memory_management.h"
#include "qcril_qmi_pil_monitor.h"
#include "qcril_pd_notifier.h"
#include "qcril_other.h"
#include "qcril_db.h"
#include "qcril_config.h"

// required for glibc compile
#include <limits.h>

#ifdef QMI_RIL_UTF
#include <netdb.h>
#endif

#include <memory>
#include "modules/qmi/QmiStruct.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/voice/VoiceDialSetUpInd.h"
#include "modules/pbm/RilNumberIsEmergency.h"
#include "modules/pbm/NetworkDetectedEccNumberInfoMessage.h"
#include "modules/pbm/GetNetworkDetectedEccNumberInfoMessage.h"
#include "framework/PolicyManager.h"
#include "WakeLock.h"
#include "VoiceModule.h"

#include <interfaces/ims/QcRilUnsolImsRadioStateIndication.h>
#include <interfaces/ims/QcRilUnsolImsPreAlertingCallInfo.h>
#include <interfaces/voice/QcRilRequestConferenceMessage.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallConfirmMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallInitiateMessage.h>
#include <interfaces/voice/QcRilRequestImsSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSendDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolAutoCallRejectionMessage.h>
#include <interfaces/voice/QcRilUnsolCallRingingMessage.h>
#include <interfaces/voice/QcRilUnsolCallStateChangeMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaOtaProvisionStatusMessage.h>
#include <interfaces/voice/QcRilUnsolConfParticipantStatusInfoMessage.h>
#include <interfaces/voice/QcRilUnsolDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolExtBurstIntlMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsModifyCallMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>
#include <interfaces/voice/QcRilUnsolImsTtyNotificationMessage.h>
#include <interfaces/voice/QcRilUnsolImsViceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolNssReleaseMessage.h>
#include <interfaces/voice/QcRilUnsolOnUssdMessage.h>
#include <interfaces/voice/QcRilUnsolOnSipDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolRingbackToneMessage.h>
#include <interfaces/voice/QcRilUnsolSpeechCodecInfoMessage.h>
#include <interfaces/voice/QcRilUnsolSrvccStatusMessage.h>
#include <interfaces/voice/QcRilUnsolSuppSvcErrorCodeMessage.h>
#include <interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h>
#include <interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h>
#include <interfaces/voice/QcRilVoiceNotifyEmergencyCallEndMessage.h>
#include <interfaces/voice/QcRilUnsolVoiceAllCsCallsEndedMessage.h>
#include <interfaces/sms/RilUnsolStkCCAlphaNotifyMessage.h>

#include "framework/Log.h"
#include "VoiceRequestPendingDialMessage.h"
#include "qcril_legacy_apis.h"
#include "misc.h"

#include "interfaces/voice/QcRilVoiceNotifyCallInProcessMessage.h"
#include "interfaces/voice/QcRilUnsolVoiceNumberOfCsCallChangedMessage.h"

#include "interfaces/nas/QcRilGetPowerOptEnabledSyncMessage.h"
#include "interfaces/nas/QcRilGetUIStatusSyncMessage.h"
#include "interfaces/nas/QcRilGetCurrentMccMncSyncMessage.h"
#include <inttypes.h>

#include "qcril_voice_external.h"

/*===========================================================================

                    INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define TAG  "RILQ"
#define QCRIL_QMI_VOICE_CALLED_PARTY_BCD_NO_LENGTH       41

#define QCRIL_QMI_VOICE_SS_TA_UNKNOWN       129 /* 0x80|CM_TON_UNKNOWN      |CM_NPI_ISDN */
#define QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL 145 /* 0x80|CM_TON_INTERNATIONAL|CM_NPI_ISDN */
#define QCRIL_QMI_VOICE_SS_TA_INTER_PREFIX  '+' /* ETSI international call dial prefix */

#define QCRIL_QMI_VOICE_CLASS_NONE              0X00
#define QCRIL_QMI_VOICE_CLASS_VOICE             0X01
#define QCRIL_QMI_VOICE_CLASS_DATA              0X02
#define QCRIL_QMI_VOICE_CLASS_FAX               0X04
#define QCRIL_QMI_VOICE_CLASS_SMS               0X08
#define QCRIL_QMI_VOICE_CLASS_DATACIRCUITSYNC   0X10
#define QCRIL_QMI_VOICE_CLASS_DATACIRCUITASYNC  0X20
#define QCRIL_QMI_VOICE_CLASS_PACKETACCESS      0X40
#define QCRIL_QMI_VOICE_CLASS_PADACCESS         0X80

#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL        ( QCRIL_QMI_VOICE_CLASS_VOICE | QCRIL_QMI_VOICE_CLASS_FAX | QCRIL_QMI_VOICE_CLASS_SMS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_TELEPHONY  ( QCRIL_QMI_VOICE_CLASS_VOICE  )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_DATA   ( QCRIL_QMI_VOICE_CLASS_FAX | QCRIL_QMI_VOICE_CLASS_SMS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_FAX        ( QCRIL_QMI_VOICE_CLASS_FAX )
#define QCRIL_QMI_VOICE_TELESEFRVICE_SMS        ( QCRIL_QMI_VOICE_CLASS_SMS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_EXCEPT_SMS  ( QCRIL_QMI_VOICE_CLASS_VOICE | QCRIL_QMI_VOICE_CLASS_SMS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_BEARER_SVC  ( QCRIL_QMI_VOICE_CLASS_DATACIRCUITSYNC | QCRIL_QMI_VOICE_CLASS_DATACIRCUITASYNC )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_ASYNC_SVC   ( QCRIL_QMI_VOICE_CLASS_DATACIRCUITASYNC | QCRIL_QMI_VOICE_CLASS_PADACCESS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_SYNC_SVC    ( QCRIL_QMI_VOICE_CLASS_DATACIRCUITSYNC | QCRIL_QMI_VOICE_CLASS_PACKETACCESS )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_DATA_CIRCUIT_SYNC_SVC    ( QCRIL_QMI_VOICE_CLASS_DATACIRCUITSYNC )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_DATA_CIRCUIT_ASYNC_SVC    ( QCRIL_QMI_VOICE_CLASS_DATACIRCUITASYNC )
#define QCRIL_QMI_VOICE_TELESEFRVICE_TELEPHONY_AND_ALL_SYNC_SVC    ( QCRIL_QMI_VOICE_CLASS_VOICE | QCRIL_QMI_VOICE_CLASS_DATACIRCUITSYNC )
#define QCRIL_QMI_VOICE_TELESEFRVICE_ALL_GPRS_BEARER_SVC    ( QCRIL_QMI_VOICE_CLASS_PADACCESS )

#define QCRIL_QMI_VOICE_SERVICE_STATUS_INACTIVE    0x00
#define QCRIL_QMI_VOICE_SERVICE_STATUS_ACTIVE      0x01

#define QCRIL_QMI_VOICE_RIL_CF_STATUS_NOT_ACTIVE    0
#define QCRIL_QMI_VOICE_RIL_CF_STATUS_ACTIVE        1

#define QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER_PREFIX  '+'
#define QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER         145
#define QCRIL_QMI_VOICE_DOMESTIC_NUMBER              129

#define PEND_EMER_LOCK()        pending_emergency_lock_mutex.lock()
#define PEND_EMER_UNLOCK()      pending_emergency_lock_mutex.unlock()

#define QMI_RIL_SYS_PROP_DIAG_LOGGING           "vendor.radio.voice_diag_logging"
#define QMI_RIL_SYS_PROP_DIAG_LOGGING_LENGTH    (4)
#define QCRIL_DIAG_PATH                         "/vendor/bin/diag_mdlog"
#define QCRIL_DIAG_CMD_LENGTH                   (512)
#define QCRIL_DIAG_APQ_MASK_NAME                "/vendor/etc/diag/voice-call-min-log.cfg"
#define QCRIL_DIAG_MDM_MASK_NAME                "/vendor/etc/diag/mdm/voice-call-min-log.cfg"
#define QCRIL_DIAG_LOG_LOCATION DATA_PARTITION_ROOT "diag_logs"

#define AUDIO_PD_SERVICE_NAME                   "avs/audio"
#define AUDIO_PD_DOMAIN_NAME                    "msm/adsp/audio_pd"

#define QCRIL_NUM_1X_WAIT_WAKE_LOCK_NAME        "qcril_num_1x_wait"
#define QCRIL_NUM_1X_WAIT_WAKE_LOCK_PROP        "vendor.radio.num_1x_wait.wakelock"

// local struct definition
typedef struct
{
    bool                        voice_state_valid;
    bool                        voice_radio_tech_valid;
    bool                        status_overview_valid;
    bool                        voice_rte_valid;
    bool                        data_rte_valid;
    bool                        ims_rte_valid;
    int                         voice_state;
    RIL_RadioTechnology         voice_radio_tech;
    uint32_t                    status_overview;
    qmi_ril_nw_reg_rte_type     voice_rte;
    qmi_ril_nw_reg_rte_type     data_rte;
    qmi_ril_nw_reg_rte_type     ims_rte;
} qcril_qmi_voice_nas_reported_info_type;

/*===========================================================================

                    QMI VOICE GLOBALS

===========================================================================*/

static qcril_qmi_voice_info_type  qcril_qmi_voice_info;

static qtimutex::QtiRecursiveMutex pending_emergency_lock_mutex;
static qcril_qmi_pending_call_info_type *qcril_qmi_pending_emergency_call_info = nullptr;
static qcril_qmi_pending_call_info_type *qcril_qmi_pending_wps_call_info = nullptr;

static qmi_ril_gen_operational_status_type voice_operational_status =
        QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN;
static qtimutex::QtiRecursiveMutex voice_operational_status_mutex;

static qcril_qmi_voice_voip_overview_type qmi_voice_voip_overview;

// STK CC info
qcril_qmi_voice_stk_cc_info_type              stk_cc_info;

call_type_enum_v02 qcril_qmi_voice_cdma_call_type_to_be_considered;

static int diag_odl_logging_in_progress = FALSE;

static qtimutex::QtiSharedMutex diag_odl_logging_in_progress_mutex;

static boolean disabled_screen_off_ind = FALSE;

static boolean ims_client_connected = FALSE;

WakeLock voiceNum1xWaitWakeLock(QCRIL_NUM_1X_WAIT_WAKE_LOCK_NAME);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
static std::string hide_phone_number(const char* in)
{
  if (!in) return "";
  std::string tmp = in;
  if (tmp.length() > QCRIL_SHOW_PHONE_NUMBER_MAX) {
    std::string lastDigits = tmp.substr(tmp.length() - QCRIL_SHOW_PHONE_NUMBER_MAX);
    std::string numStr = QCRIL_HIDE_PHONE_NUMBER_STR + lastDigits;
    return numStr;
  } else {
    return "****";
  }
}

static void qcril_qmi_voice_handle_audio_pd_state_changed
(
   qcril_qmi_pd_entry *entry,
   qcril_qmi_pd_state state,
   void *userdata
);
static int qcril_stop_diag_log(void);

static void qcril_qmi_voice_invalid_last_call_failure_cause();
static void qcril_qmi_voice_respond_ril_last_call_failure_request();
static int qcril_qmi_voice_nas_control_is_call_mode_reported_voice_radio_tech_different(call_mode_enum_v02 call_mode);
static RIL_RadioTechnology qcril_qmi_voice_nas_control_get_reported_voice_radio_tech();
static unsigned int qcril_qmi_voice_convert_call_mode_to_radio_tech_family(call_mode_enum_v02 call_mode);
static void qcril_qmi_voice_auto_answer_timeout_handler( void *param );
static void qmi_ril_voice_drop_homeless_incall_reqs( void );
static void qmi_ril_voice_cleanup_reqs_after_call_completion(void);
uint32_t qcril_qmi_voice_call_num_copy_with_toa_check(char *src, uint32_t src_size, char* dest,
                                                      uint32_t dest_buffer_size, voice_num_type_enum_v02 num_type);

static void qcril_qmi_voice_create_emer_voice_entry(qcril_qmi_voice_emer_voice_feature_info_type * emer_voice_number, voice_remote_party_number2_type_v02 * remote_party_number);
static int qcril_qmi_voice_is_emer_voice_entry_valid(qcril_qmi_voice_emer_voice_feature_info_type * emer_voice_number);
static bool qcril_qmi_voice_get_cdma_emergency_number(std::string& number,     /* out */
                                                      int& numberPresentation, /* out */
                                                      int& toa                 /* out */
);

static int qcril_qmi_voice_is_cdma_voice_emergency_calls_present(qcril_qmi_voice_voip_call_info_entry_type **cdma_voice_call_info_entry,
                                                                 qcril_qmi_voice_voip_call_info_entry_type **cdma_no_srv_emer_call_info_entry);
static void qcril_qmi_voice_respond_pending_hangup_ril_response(uint8_t call_id);
static void qmi_ril_voice_pending_1x_num_timeout(void * param);
static void qcril_qmi_voice_cancel_num_1x_wait_timer();
static void qcril_qmi_voice_setup_num_1x_wait_timer();

std::string qcril_qmi_voice_map_ril_reason_to_str(qcril::interfaces::CallFailCause reason);
static void qcril_qmi_voice_handle_new_last_call_failure_cause
(
 call_end_reason_enum_v02 qmi_reason,
 qcril::interfaces::CallFailCause ril_reason,
 std::string reason_str,
 qcril_qmi_voice_voip_call_info_entry_type *call_obj
);
static void qcril_qmi_voice_set_last_call_fail_request_timeout();
static void qcril_qmi_voice_last_call_fail_request_timeout_handler(void *param);
static void qcril_qmi_voice_consider_shadow_remote_number_cpy_creation( qcril_qmi_voice_voip_call_info_entry_type* entry );

static void qmi_ril_succeed_on_pending_hangup_req_on_no_calls_left(void);

static void qcril_qmi_voice_transfer_sim_ucs2_alpha_to_std_ucs2_alpha(const voice_alpha_ident_type_v02 *sim_alpha, voice_alpha_ident_type_v02 *std_alpha);

static void qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl( void );

static void qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl( void );

static void qmi_ril_voice_ended_call_obj_phase_out(uint8_t qmi_call_id);

static boolean qcril_qmi_voice_call_to_ims(const qcril_qmi_voice_voip_call_info_entry_type * call_info_entry);

static void qmi_ril_voice_ims_command_oversight_timeout_handler(uint16_t token);

static boolean qmi_ril_voice_is_calls_supressed_by_pil_vcl();
static boolean qmi_ril_voice_is_audio_inactive_vcl();
#if !defined (QMI_RIL_UTF) && defined RIL_WEARABLES_TARGET_AON
static void qcril_qmi_voice_handle_pil_state_changed(const qcril_qmi_pil_state* cur_state);
#endif
static void qcril_qmi_voice_hangup_all_non_emergency_calls_vcl();
static void qcril_qmi_voice_hangup_all_calls_vcl(boolean (*exception_filter)(const qcril_qmi_voice_voip_call_info_entry_type*), boolean qmi_async);

static boolean qcril_qmi_voice_get_atel_call_type_info
(
   call_type_enum_v02 call_type,
   boolean video_attrib_valid,
   voice_call_attribute_type_mask_v02 video_attrib,
   boolean audio_attrib_valid,
   voice_call_attribute_type_mask_v02 audio_attrib,
   boolean attrib_status_valid,
   voice_call_attrib_status_enum_v02 attrib_status,
   boolean call_info_elab_valid,
   qcril_qmi_voice_voip_call_info_elaboration_type call_info_elab,
   qcril::interfaces::CallType cached_call_type,
   qcril::interfaces::CallType &callType,
   qcril::interfaces::CallDomain &callDomain,
   uint32_t &callSubState
);

static RIL_Errno qcril_qmi_voice_stk_ss_resp_handle
(
 uint16_t msg_token_id,
 qmi_response_type_v01* resp,
 uint8_t alpha_ident_valid,
 voice_alpha_ident_type_v02* alpha_ident,
 uint8_t call_id_valid,
 uint8_t call_id,
 uint8_t cc_sups_result_valid,
 voice_cc_sups_result_type_v02* cc_sups_result,
 uint8_t cc_result_type_valid,
 voice_cc_result_type_enum_v02* cc_result_type
);

static void qcril_qmi_voice_reset_info_xml(qcril_qmi_voice_info_ind_xml_type_e_type xml_type);

static void qcril_qmi_voice_set_audio_call_type
(
    const voice_call_info2_type_v02* iter_call_info,
    qcril_qmi_voice_voip_call_info_entry_type *call_info_entry
);

static void qcril_qmi_voice_send_hangup_on_call
(
    int conn_index
);

static int qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust( qcril_qmi_voice_voip_call_info_elaboration_type elaboration_single_input,
                                                                          qcril_qmi_voice_voip_call_info_elaboration_type * elaboration_single_adjusted );

static void qcril_qmi_voice_voip_reset_answered_call_type
(
 qcril_qmi_voice_voip_call_info_entry_type *call_info,
 voice_modified_ind_msg_v02                *modify_ind_ptr
);

static void qcril_qmi_voice_send_simulated_SRVCC_ind(void);

static void qcril_qmi_voice_update_audio_state_vcl(void);
static void qcril_qmi_voice_send_ims_unsol_resp_handover(RIL_SrvccState ril_srvccstate);
static void qcril_qmi_voice_send_unsol_ussd(qcril::interfaces::UssdDomain domain,
    qcril::interfaces::UssdModeType mode,
    std::string ussdMessage,
    std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails);
static boolean qcril_qmi_voice_cancel_pending_wps_call(bool send_response);
static boolean qcril_qmi_voice_cancel_pending_emergency_call(bool send_response);
static void qcril_qmi_voice_voip_cleanup_all_call_info_entries(void);


template <typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
static inline T get_next_sequence_number(T value)
{
    return (value == std::numeric_limits<T>::max()) ? 0 : value + 1;
}

static inline void set_audio_released(bool value) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    voiceModule->setIsAudioReleased(value);
  }
  else {
    QCRIL_LOG_ERROR("getVoiceModule() failed, so isAudioReleased was not changed.");
  }
}

//===========================================================================
// qcril_qmi_voice_init_pending_calls
//===========================================================================
boolean qcril_qmi_voice_init_pending_calls()
{
  // Clear wps pending dial request
  qcril_qmi_voice_cancel_pending_wps_call(true);
  // Clear pending emergency call request
  qcril_qmi_voice_cancel_pending_emergency_call(true);
  return TRUE;
}

//===========================================================================
// qcril_qmi_voice_pre_init
//===========================================================================
RIL_Errno qcril_qmi_voice_pre_init(void)
{
  QCRIL_LOG_FUNC_ENTRY();

#ifdef RIL_FOR_MDM_LE
  ims_client_connected = TRUE;
#endif
  QCRIL_LOG_FUNC_RETURN();

  return RIL_E_SUCCESS;
} // qcril_qmi_voice_pre_init

//===========================================================================
// get_voice_modem_endpoint
//===========================================================================
std::shared_ptr<VoiceModemEndPoint> get_voice_modem_endpoint
(
  void
)
{
  return ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();
} // get_voice_modem_endpoint

//===========================================================================
// qmi_client_voice_send_async
//===========================================================================
qmi_client_error_type qmi_client_voice_send_async
(
 unsigned long                   msg_id,
 void                            *req_ptr,
 int                             req_struct_len,
 int                             resp_struct_len,
 qmiAsyncCbType                  resp_cb,
 void                            *resp_cb_data
)
{
  qmi_client_error_type rc = QMI_INTERNAL_ERR;

  QCRIL_LOG_FUNC_ENTRY();

  Module* module = nullptr;
  if (getVoiceModule() != nullptr) {
    module = getVoiceModule().get();
  }
  rc = get_voice_modem_endpoint()->sendRawAsync(msg_id,
      req_ptr,
      req_struct_len,
      resp_struct_len,
      resp_cb,
      resp_cb_data,
      module);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
  return rc;
}

static bool getVoiceModuleFeature(VoiceFeatureType feature) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    return voiceModule->getFeature(feature);
  }
  return false;
}

static std::shared_ptr<Message> extractVoiceModulePendingMessage(message_id_ref messageId) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    return voiceModule->getPendingMessageList().extract(messageId);
  }
  return nullptr;
}

static std::shared_ptr<Message> findVoiceModulePendingMessage(message_id_ref messageId) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    return voiceModule->getPendingMessageList().find(messageId);
  }
  return nullptr;
}

static std::shared_ptr<Message> findVoiceModulePendingMessage(uint16_t token) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    return voiceModule->getPendingMessageList().find(token);
  }
  return nullptr;
}

static bool eraseVoiceModulePendingMessage(std::shared_ptr<Message> message) {
  auto voiceModule = getVoiceModule();
  if (voiceModule) {
    return voiceModule->getPendingMessageList().erase(message);
  }
  return false;
}

//===========================================================================
// qcril_qmi_voice_ind_registrations
//===========================================================================
void qcril_qmi_voice_ind_registrations
(
  void
)
{
  voice_indication_register_req_msg_v02  indication_req;
  voice_indication_register_resp_msg_v02 indication_resp_msg;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));

  indication_req.reg_voice_privacy_events_valid = TRUE;
  indication_req.reg_voice_privacy_events = 0x01;

  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("Voice_privacy events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("Voice_privacy events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));

  indication_req.ext_brst_intl_events_valid = TRUE;
  indication_req.ext_brst_intl_events = 0x01;

  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("Extended_burst events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("Extended_burst events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));

  indication_req.speech_events_valid = TRUE;
  indication_req.speech_events = 0x01;

  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("Speech events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("Speech events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.handover_events_valid = TRUE;
  indication_req.handover_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("Handover events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("Handover events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.conference_events_valid = TRUE;
  indication_req.conference_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("Conference events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("Conference events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.tty_info_events_valid = TRUE;
  indication_req.tty_info_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
    QCRIL_LOG_INFO("tty_info events indication register failed!");
  }
  else
  {
    QCRIL_LOG_INFO("tty_info events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.cc_result_events_valid = TRUE;
  indication_req.cc_result_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("cc_result events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("cc_result events registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.additional_call_info_events_valid = TRUE;
  indication_req.additional_call_info_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("additional_call_info events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("additional_call_info events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.audio_rat_change_events_valid = TRUE;
  indication_req.audio_rat_change_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("audio_rat_change events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("audio_rat_change events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.vice_dialog_event_valid = TRUE;
  indication_req.vice_dialog_event = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("vice_dialog events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("vice_dialog events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.conf_participants_events_valid = TRUE;
  indication_req.conf_participants_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("conf_participants_events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("conf_participants_events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.sups_events_valid = TRUE;
  indication_req.sups_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("sups_events indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("sups_events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.auto_rejected_incoming_call_end_event_valid = TRUE;
  indication_req.auto_rejected_incoming_call_end_event = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("auto_rejected_incoming_call_end_event indication register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("auto_rejected_incoming_call_end_event registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.sip_dtmf_event_valid = TRUE;
  indication_req.sip_dtmf_event = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("sip_dtmf_event register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("sip_dtmf_event registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.notify_clir_mode_events_valid = TRUE;
  indication_req.notify_clir_mode_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("notify_clir_mode_events register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("notify_clir_mode_events registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.network_detected_ecc_number_event_valid = TRUE;
  indication_req.network_detected_ecc_number_event = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                  &indication_req,
                                  sizeof(indication_req),
                                  &indication_resp_msg,
                                  sizeof(indication_resp_msg)
                                  ) != QMI_NO_ERR )
  {
     QCRIL_LOG_INFO("network_detected_ecc_number_event register failed!");
  }
  else
  {
     QCRIL_LOG_INFO("network_detected_ecc_number_event registration error code: %d",
                    indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.reg_dtmf_events_valid = TRUE;
  indication_req.reg_dtmf_events = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(
          QMI_VOICE_INDICATION_REGISTER_REQ_V02, &indication_req, sizeof(indication_req),
          &indication_resp_msg, sizeof(indication_resp_msg)) != QMI_NO_ERR) {
    QCRIL_LOG_INFO("dtmf_event register failed!");
  } else {
    QCRIL_LOG_INFO("dtmf_event registration error code: %d", indication_resp_msg.resp.error);
  }

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));
  indication_req.srtp_status_info_event_valid = TRUE;
  indication_req.srtp_status_info_event = 0x01;
  if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                              &indication_req,
                                              sizeof(indication_req),
                                              &indication_resp_msg,
                                              sizeof(indication_resp_msg)) != QMI_NO_ERR) {
    QCRIL_LOG_INFO("srtp_status_info_event register failed!");
  } else {
    QCRIL_LOG_INFO("srtp_status_info_event registration error code: %d",
                   indication_resp_msg.resp.error);
  }

  disabled_screen_off_ind = FALSE;

  QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_voice_ind_registrations

uint8_t qcril_qmi_voice_is_set_all_call_forward_supported()
{
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  qmi_get_supported_msgs_resp_v01 qmi_resp;
  uint8_t ret = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&qmi_resp, 0x0, sizeof(qmi_resp));
  qmi_client_error = get_voice_modem_endpoint()->sendRawSync(
      QMI_VOICE_GET_SUPPORTED_MSGS_REQ_V02,
      NULL,
      0,
      &qmi_resp,
      sizeof(qmi_resp));

  QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error);

  if (qmi_client_error == QMI_NO_ERR)
  {
    if (qmi_resp.supported_msgs_valid)
    {
      int message_id = QMI_VOICE_SET_ALL_CALL_FWD_SUPS_REQ_V02;
      uint32_t index = message_id/8;
      uint8_t bit_position = message_id - index * 8;
      uint8_t bit_position_mask = 0x01 << bit_position;

      if (index < qmi_resp.supported_msgs_len)
      {
        ret = ((qmi_resp.supported_msgs[index] & bit_position_mask) == bit_position_mask);
      }
    }
  }
  QCRIL_LOG_INFO("ret = %d", ret);

  return ret;
}

bool qcril_qmi_voice_is_ue_based_clir_operation_mode()
{
  voice_get_sups_operation_mode_req_msg_v02 qmi_req;
  voice_get_sups_operation_mode_resp_msg_v02 qmi_resp;
  bool is_ue_based_clir = true;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&qmi_req, 0x0, sizeof(qmi_req));
  memset(&qmi_resp, 0x0, sizeof(qmi_resp));
  qmi_req.reason = VOICE_REASON_CLIR_V02;
  qmi_client_error_type qmi_client_error = get_voice_modem_endpoint()->sendRawSync(
      QMI_VOICE_GET_SUPS_OPERATION_MODE_REQ_V02,
      &qmi_req,
      sizeof(qmi_req),
      &qmi_resp,
      sizeof(qmi_resp));

  QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error);

  if (qmi_client_error == QMI_NO_ERR)
  {
    QCRIL_LOG_INFO("qmi_resp.sups_operation_mode %d, %d",
                   (int) qmi_resp.sups_operation_mode_valid, (int) qmi_resp.sups_operation_mode);
    if (qmi_resp.sups_operation_mode_valid)
    {
      if (qmi_resp.sups_operation_mode == UE_BASED_SUPS_V02)
      {
        is_ue_based_clir = true;
      }
      else if (qmi_resp.sups_operation_mode == NETWORK_BASED_SUPS_V02)
      {
        is_ue_based_clir = false;
      }
    }
  }
  QCRIL_LOG_INFO("is_ue_based_clir = %d", is_ue_based_clir);

  return is_ue_based_clir;
}


//===========================================================================
// qcril_qmi_voice_is_active_ims_call
//===========================================================================
static boolean qcril_qmi_voice_is_active_ims_call
(
 const qcril_qmi_voice_voip_call_info_entry_type * call_info_entry
)
{
  boolean ret = FALSE;

  if (call_info_entry)
  {
    ret = (qcril_qmi_voice_call_to_ims(call_info_entry) &&
            (call_info_entry->voice_scv_info.call_state != CALL_STATE_END_V02));
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)ret);
  return ret;
} // qcril_qmi_voice_is_active_ims_call

//===========================================================================
// qcril_qmi_voice_enable_voice_indications
//===========================================================================
void qcril_qmi_voice_enable_voice_indications
(
  boolean enable
)
{
  boolean need_to_register = FALSE;
  voice_indication_register_req_msg_v02  indication_req;
  voice_indication_register_resp_msg_v02 indication_resp_msg;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO("disabled_screen_off_ind = %d\n", disabled_screen_off_ind);

  if (enable == FALSE)
  {
    // De-register for the indications if PS calls are not present
    if (!disabled_screen_off_ind)
    {
      need_to_register = !qcril_qmi_voice_has_specific_call(qcril_qmi_voice_is_active_ims_call, NULL);
      disabled_screen_off_ind = need_to_register;
    }
  }
  else
  {
    // Register again only if previously de-registered the indications
    need_to_register = disabled_screen_off_ind;
    disabled_screen_off_ind = FALSE;
  }

  QCRIL_LOG_INFO("enable = %d, need_to_register = %d, disabled_screen_off_ind = %d\n",
                 enable, need_to_register, disabled_screen_off_ind);

  if (need_to_register)
  {
    memset(&indication_req, 0, sizeof(indication_req));
    memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));

    indication_req.handover_events_valid = TRUE;
    indication_req.handover_events = enable;

    if (get_voice_modem_endpoint()->sendRawSync(QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                    &indication_req,
                                    sizeof(indication_req),
                                    &indication_resp_msg,
                                    sizeof(indication_resp_msg)) != QMI_NO_ERR)
    {
      QCRIL_LOG_INFO("Indication register failed!");
    }
    else
    {
      QCRIL_LOG_INFO("Events registration error code: %d", indication_resp_msg.resp.error);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_enable_voice_indications


void *qcril_qmi_audio_pd_init_thread(void *arg)
{
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(arg);
    QCRIL_LOG_DEBUG("Initializing pd_handle");
    qcril_qmi_voice_info.pd_info.handle =
    qcril_qmi_pd_notifier_new(AUDIO_PD_SERVICE_NAME, "QCRIL_VOICE");

    if (qcril_qmi_voice_info.pd_info.handle &&
        qcril_pd_notifier_available(qcril_qmi_voice_info.pd_info.handle))
    {
      QCRIL_LOG_DEBUG("pd_notifier is available");
      qcril_pd_notifier_register_for(qcril_qmi_voice_info.pd_info.handle, AUDIO_PD_DOMAIN_NAME, qcril_qmi_voice_handle_audio_pd_state_changed, NULL);
      qcril_pd_notifier_start_listening(qcril_qmi_voice_info.pd_info.handle, AUDIO_PD_DOMAIN_NAME);
    }
    else
    {
      QCRIL_LOG_DEBUG("pd_notifier is unavailable");
      qcril_qmi_voice_info.pd_info.state = QCRIL_QMI_PD_UNUSED;
    }

    QCRIL_LOG_DEBUG("Locking overview");
    qcril_qmi_voice_voip_lock_overview();
    QCRIL_LOG_DEBUG("Updating audio state");
    qcril_qmi_voice_update_audio_state_vcl();
    qcril_qmi_voice_voip_unlock_overview();
    QCRIL_LOG_DEBUG("Unlocked overview");

    QCRIL_LOG_FUNC_RETURN();
    return NULL;
}
//===========================================================================
// qcril_qmi_audio_pd_init
//===========================================================================
RIL_Errno qcril_qmi_audio_pd_init
(
)
{
    int rc = -1;
    RIL_Errno ret = RIL_E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    if(!qcril_qmi_voice_info.pd_info.pd_init_thread_valid)
    {
        qcril_qmi_voice_info.pd_info.state = QCRIL_QMI_PD_UNKNOWN;

        rc = pthread_create(&qcril_qmi_voice_info.pd_info.pd_init_thread,
                                NULL, qcril_qmi_audio_pd_init_thread, NULL);
        if(!rc)
        {
            qcril_qmi_voice_info.pd_info.pd_init_thread_valid =TRUE;
        }
        else
        {
            QCRIL_LOG_ERROR("Unable to create Audio pd thread");
            ret = RIL_E_GENERIC_FAILURE;
        }
    }
    else
    {
        QCRIL_LOG_INFO("Audio pd thread already created. Skipping.");
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
} // qcril_qmi_audio_pd_init

//===========================================================================
// qcril_qmi_voice_init
//===========================================================================
RIL_Errno qcril_qmi_voice_init
(
  void
)
{
  QCRIL_LOG_FUNC_ENTRY();

  if (get_voice_modem_endpoint() == nullptr) {
    return RIL_E_RADIO_NOT_AVAILABLE;
  }

  qcril_qmi_voice_ims_cleanup_unknown_calls();

  qcril_qmi_voice_ind_registrations();
  /* Default CLIR */
  qcril_qmi_voice_info.clir = ( uint8_t ) QCRIL_QMI_VOICE_SS_CLIR_PRESENTATION_INDICATOR;

  /* Use saved CLIR setting if available */
  static const property_id_type id_mapping[] = {PERSIST_VENDOR_RADIO_CLIR0,
        PERSIST_VENDOR_RADIO_CLIR1 };
  static constexpr auto id_mapping_size = sizeof(id_mapping)/sizeof(id_mapping[0]);
  auto inst_id = static_cast<uint8_t>(qmi_ril_get_process_instance_id());
  int int_config;
  if (inst_id <= id_mapping_size && qcril_config_get(
            id_mapping[inst_id], int_config) == E_SUCCESS)
  {
      if (int_config >= 0 && int_config <= QCRIL_QMI_VOICE_SS_CLIR_SUPPRESSION_OPTION)
          qcril_qmi_voice_info.clir = int_config;
      else
          QCRIL_LOG_ERROR( "QCRIL QMI VOICE Invalid saved CLIR %ld, use default", int_config);
  }
  QCRIL_LOG_DEBUG( "CLIR=%d", qcril_qmi_voice_info.clir );

  /* Default Auto Answer timerID */
  qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;
  qmi_voice_voip_overview.num_1x_wait_timer_id = TimeKeeper::no_timer;

  qcril_qmi_voice_reset_stk_cc();

  qcril_qmi_voice_reset_info_xml(QCRIL_QMI_VOICE_CONFERENCE_INFO_IND_XML);
  qcril_qmi_voice_reset_info_xml(QCRIL_QMI_VOICE_DIALOG_INFO_IND_XML);

  qcril_qmi_voice_cdma_call_type_to_be_considered = CALL_TYPE_VOICE_V02;
  if (qcril_config_get(PERSIST_VENDOR_RADIO_CALL_TYPE, int_config) == E_SUCCESS)
  {
    if (int_config == 0)
        qcril_qmi_voice_cdma_call_type_to_be_considered = CALL_TYPE_EMERGENCY_V02;
  }
  QCRIL_LOG_DEBUG("qcril_qmi_voice_cdma_call_type_to_be_considered=%d",
      qcril_qmi_voice_cdma_call_type_to_be_considered);

  // Check if num_1x_wait_timer_wakelock_is_acquired is already acquired
  // and release if required.
  qmi_ril_get_property_value_from_boolean(QCRIL_NUM_1X_WAIT_WAKE_LOCK_PROP,
          &qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired, FALSE);
  qcril_qmi_voice_cancel_num_1x_wait_timer();

  qcril_qmi_voice_init_pending_calls();

  qcril_qmi_voice_reset_ussd_power_opt_buffer();

  qcril_qmi_voice_info.send_vice_unsol_on_socket_connect = FALSE;

  // Initialize PIL Monitor and Audio PD notifier.
  qcril_qmi_voice_info.pil_state.state = QCRIL_QMI_PIL_STATE_UNKNOWN;
#if !defined (QMI_RIL_UTF) && defined RIL_WEARABLES_TARGET_AON
  QCRIL_LOG_DEBUG("Slate based wearable");
  qcril_qmi_pil_register_for_state_change(qcril_qmi_voice_handle_pil_state_changed);
  qcril_qmi_pil_init_monitor();
  qcril_qmi_voice_info.pil_state = *(qcril_qmi_pil_get_pil_state());
#endif
  qcril_qmi_voice_info.ussd_request_domain = qcril::interfaces::UssdDomain::UNKNOWN;

  QCRIL_LOG_DEBUG("Initializing audio pd");
  qcril_qmi_audio_pd_init();

  QCRIL_LOG_FUNC_RETURN();
  return RIL_E_SUCCESS;
} // qcril_qmi_voice_init

//===========================================================================
// qcril_qmi_voice_voip_cleanup_all_call_info_entries
//===========================================================================
void qcril_qmi_voice_voip_cleanup_all_call_info_entries(void)
{
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;
  boolean unsol_resp = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  // clean call objects, will have to extend

  qcril_qmi_voice_voip_lock_overview();
  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while (NULL != call_info_entry)
  {
      if(qcril_qmi_voice_call_to_ims(call_info_entry))
      {
         unsol_resp = TRUE;
      }
      call_info_entry->voice_scv_info.call_state = CALL_STATE_END_V02;
      qcril_qmi_voice_handle_new_last_call_failure_cause((call_end_reason_enum_v02)0,
          qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR,
          qcril_qmi_voice_map_ril_reason_to_str(qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR),
          call_info_entry);

      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }
  qcril_qmi_voice_voip_unlock_overview();
  if(unsol_resp)
  {
      qcril_qmi_voice_send_unsol_call_state_changed();
  }
  qcril_qmi_voice_voip_lock_overview();
  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while (NULL != call_info_entry)
  {
      qcril_qmi_voice_voip_destroy_call_info_entry( call_info_entry );
      // Destroying call info entry will change the enumerated list so use _first function to
      // make sure that all the call info entries are destroyed properly.
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  }
  qcril_qmi_voice_voip_unlock_overview();

  QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
  if (!qcril_stop_diag_log())
  {
      QCRIL_LOG_INFO("qxdm logging disabled successfully");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_voip_cleanup_all_call_info_entries

//===========================================================================
// qcril_qmi_voice_cleanup
//===========================================================================
void qcril_qmi_voice_cleanup(void)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_voice_voip_cleanup_all_call_info_entries();
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_cleanup

int qcril_qmi_voice_get_cached_clir()
{
  return qcril_qmi_voice_info.clir;
}
uint8_t qcril_qmi_voice_get_ussd_user_action_required()
{
  return qcril_qmi_voice_info.ussd_user_action_required;
}
void qcril_qmi_voice_set_ussd_request_domain(bool is_ims)
{
  qcril_qmi_voice_info.ussd_request_domain = is_ims ?
                                             qcril::interfaces::UssdDomain::IMS :
                                             qcril::interfaces::UssdDomain::CS;
}

//===========================================================================
// qcril_qmi_voice_call_to_atel
//===========================================================================
boolean qcril_qmi_voice_call_to_atel
(
 const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry
)
{
  boolean ret = FALSE;

  if (call_info_entry)
  {
    ret = (QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN & call_info_entry->elaboration);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)ret);
  return ret;
} // qcril_qmi_voice_call_to_atel

//===========================================================================
// qcril_qmi_voice_call_to_ims
//===========================================================================
boolean qcril_qmi_voice_call_to_ims
(
 const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry
)
{
  boolean ret = FALSE;

  if (call_info_entry)
  {
    ret = !qcril_qmi_voice_call_to_atel(call_info_entry);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)ret);
  return ret;
} // qcril_qmi_voice_call_to_ims

//===========================================================================
// qcril_qmi_voice_is_call_has_ims_audio
//===========================================================================
boolean qcril_qmi_voice_is_call_has_ims_audio
(
 const qcril_qmi_voice_voip_call_info_entry_type * call_info_entry
)
{
  boolean ret = FALSE;
  boolean is_cs = FALSE;

  if (call_info_entry)
  {
    is_cs = ((call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN) ||
        (call_info_entry->audio_call_type == QMI_RIL_VOICE_IMS_AUDIO_CALL_TYPE_VOICE)) ?
      TRUE : FALSE;
    QCRIL_LOG_INFO("is cs call: %d", is_cs);
    ret = !is_cs && !(call_info_entry->srvcc_in_progress);
  }
  return ret;
} // qcril_qmi_voice_is_call_has_ims_audio

//===========================================================================
// qcril_qmi_voice_is_call_has_voice_audio
//===========================================================================
boolean qcril_qmi_voice_is_call_has_voice_audio
(
 const qcril_qmi_voice_voip_call_info_entry_type * call_info_entry
)
{
  boolean ret = FALSE;
  if (call_info_entry)
  {
    ret = call_info_entry->srvcc_in_progress ||
      !qcril_qmi_voice_is_call_has_ims_audio(call_info_entry);
  }
  return ret;
} // qcril_qmi_voice_is_call_has_voice_audio

//===========================================================================
// qcril_qmi_voice_get_answer_am_event
//===========================================================================
qcril_am_event_type qcril_qmi_voice_get_answer_am_event
(
 const qcril_qmi_voice_voip_call_info_entry_type * call_info_entry
)
{
  qcril_am_event_type am_event = QCRIL_AM_EVENT_INVALID;
  if (call_info_entry)
  {
    if (qcril_qmi_voice_is_call_has_ims_audio(call_info_entry))
    {
      am_event = QCRIL_AM_EVENT_IMS_ANSWER;
    }
    else
    {
      am_event = QCRIL_AM_EVENT_VOICE_ANSWER;
    }
  }
  return am_event;
} // qcril_qmi_voice_get_answer_am_event

//===========================================================================
// qcril_qmi_voice_get_answer_call_mode
//===========================================================================
call_mode_enum_v02 qcril_qmi_voice_get_answer_call_mode()
{
  boolean ret = FALSE;
  call_mode_enum_v02 call_mode = CALL_MODE_UNKNOWN_V02;

  qcril_qmi_voice_voip_lock_overview();
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry =
    qcril_qmi_voice_voip_call_info_entries_enum_first();

  while (NULL != call_info_entry)
  {
    if(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL)
    {
      call_mode = call_info_entry->voice_scv_info.mode;
      break;
    }
    ret = TRUE;
    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }
  qcril_qmi_voice_voip_unlock_overview();

  return call_mode;
} // qcril_qmi_voice_get_answer_call_mode

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_lookup_command_name

===========================================================================*/
/*!
    @brief
    Lookup state name.

    @return
    A pointer to the state name.
*/
/*=========================================================================*/
static const char *qcril_qmi_voice_lookup_command_name
(
  unsigned long msg
)
{
  switch( msg )
  {
    case QMI_VOICE_DIAL_CALL_RESP_V02:
      return "DIAL CALL RESP";
    case QMI_VOICE_ANSWER_CALL_RESP_V02:
      return "ANSWER CALL RESP";
    case QMI_VOICE_END_CALL_RESP_V02:
      return "END CALL RESP";
    case QMI_VOICE_BURST_DTMF_RESP_V02:
      return "BURST DTMF RESP";
    case QMI_VOICE_START_CONT_DTMF_RESP_V02:
      return "START CONT DTMF RESP";
    case QMI_VOICE_STOP_CONT_DTMF_RESP_V02:
      return "STOP CONT DTMF RESP";
    case QMI_VOICE_SEND_FLASH_RESP_V02:
      return "SEND FLASH RESP";
    case QMI_VOICE_INDICATION_REGISTER_RESP_V02:
      return "INDICATION REGISTER RESP";
    case QMI_VOICE_MANAGE_CALLS_RESP_V02:
      return "MNG CALLS RESP";
    case QMI_VOICE_SET_SUPS_SERVICE_RSEP_V02:
      return "SET SUPS RESP";
    case QMI_VOICE_GET_CLIR_RESP_V02:
      return "GET CLIR RESP";
    case QMI_VOICE_GET_CLIP_RESP_V02:
      return "GET CLIP RESP";
    case QMI_VOICE_GET_COLP_RESP_V02:
      return "GET COLP RESP";
    case QMI_VOICE_SET_CALL_BARRING_PASSWORD_RESP_V02:
      return "SET CALL BARRING PASSWORD RESP";
    case QMI_VOICE_GET_CALL_WAITING_RESP_V02:
      return "GET CALL WAITING RESP";
    case QMI_VOICE_GET_CALL_BARRING_RESP_V02:
      return "GET CALL BARRING RESP";
    case QMI_VOICE_GET_CALL_FORWARDING_RESP_V02:
      return "GET CALL FORWARDING RESP";
    case QMI_VOICE_ORIG_USSD_RESP_V02:
      return "ORIG USSD RESP";
    case QMI_VOICE_ANSWER_USSD_RESP_V02:
      return "ANSWER USSD RESP";
    case QMI_VOICE_CANCEL_USSD_RESP_V02:
      return "CANCEL USSD RESP";
    case QMI_VOICE_MANAGE_IP_CALLS_RESP_V02:
      return "MNG IP CALLS RESP";
    case QMI_VOICE_SETUP_ANSWER_RESP_V02:
      return "SETUP ANSWER RESP";
    case QMI_VOICE_GET_COLR_RESP_V02:
      return "GET COLR RESP";
    case QMI_VOICE_IMS_CALL_CANCEL_RESP_V02:
      return "IMS CALL CANCEL RESP";
    default:
      return "Unknown";
  } /* end switch */
} /* qcril_qmi_voice_lookup_command_name */


/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_qmi_cfw_reason_to_ril_reason

===========================================================================*/
/*!
    @brief
    Maps qmi CFW Reason to QCRIL CFW Reason.

    @return
    None.
*/
/*=========================================================================*/

uint8_t qcril_qmi_voice_map_qmi_cfw_reason_to_ril_reason(int qmi_cfw_reason)
{
  uint8_t ril_reason=0;

  switch(qmi_cfw_reason)
  {
    case VOICE_REASON_FWD_UNCONDITIONAL_V02 :
     ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_UNCOND;
     break;

    case VOICE_REASON_FWD_MOBILEBUSY_V02:
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_BUSY ;
      break;

    case VOICE_REASON_FWD_NOREPLY_V02 :
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_NOREPLY;
      break;

    case VOICE_REASON_FWD_UNREACHABLE_V02:
      ril_reason = (uint8_t) QCRIL_QMI_VOICE_CCFC_REASON_NOTREACH ;
      break;

    case VOICE_REASON_FWD_ALLFORWARDING_V02:
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_ALLCALL ;
      break;

    case VOICE_REASON_FWD_ALLCONDITIONAL_V02:
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_ALLCOND ;
      break;

    case VOICE_REASON_FWD_NOT_LOGGED_IN_V02:
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_NOTLOGIN;
      break;

    default:
      /* Fail to add entry to ReqList */
      QCRIL_LOG_ERROR("qmi_reason : %d",qmi_cfw_reason);
      ril_reason = (uint8_t)QCRIL_QMI_VOICE_CCFC_REASON_UNCOND;
      break;
  }

  return ril_reason;
}/*qcril_qmi_voice_map_qmi_cfw_reason_to_ril_reason*/

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_ril_reason_to_qmi_cfw_reason

===========================================================================*/
/*!
    @brief
    Maps QCRIL CFW Reson to qmi CFW Reason

    @return
    None.
*/
/*=========================================================================*/
bool qcril_qmi_voice_map_ril_reason_to_qmi_cfw_reason(voice_reason_enum_v02 &qmi_reason, int ril_reason)
{
  bool result = true;
  switch(ril_reason)
  {
    case QCRIL_QMI_VOICE_CCFC_REASON_UNCOND:
      qmi_reason = VOICE_REASON_FWD_UNCONDITIONAL_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_BUSY:
      qmi_reason = VOICE_REASON_FWD_MOBILEBUSY_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_NOREPLY:
      qmi_reason = VOICE_REASON_FWD_NOREPLY_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_NOTREACH:
      qmi_reason = VOICE_REASON_FWD_UNREACHABLE_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_ALLCALL:
      qmi_reason = VOICE_REASON_FWD_ALLFORWARDING_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_ALLCOND:
      qmi_reason = VOICE_REASON_FWD_ALLCONDITIONAL_V02;
      break;

    case QCRIL_QMI_VOICE_CCFC_REASON_NOTLOGIN:
      qmi_reason = VOICE_REASON_FWD_NOT_LOGGED_IN_V02;
      break;

    default:
      qmi_reason = VOICE_REASON_ENUM_MAX_ENUM_VAL_V02;
      result = false;
      break;
  }

  QCRIL_LOG_DEBUG("Mapped ril_reason %d to qmi_reason %d", ril_reason, qmi_reason);

  return result;
}/*qcril_qmi_voice_map_ril_reason_to_qmi_cfw_reason*/

bool qcril_qmi_voice_map_ril_status_to_qmi_cfw_status(voice_service_enum_v02 &out, uint32_t in) {
  bool result = true;
  switch (in) {
    case QCRIL_QMI_VOICE_MODE_DISABLE:
      out = VOICE_SERVICE_DEACTIVATE_V02;
      break;
    case QCRIL_QMI_VOICE_MODE_ENABLE:
      out = VOICE_SERVICE_ACTIVATE_V02;
      break;
    case QCRIL_QMI_VOICE_MODE_REG:
      out = VOICE_SERVICE_REGISTER_V02;
      break;
    case QCRIL_QMI_VOICE_MODE_ERASURE:
      out = VOICE_SERVICE_ERASE_V02;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_qmi_to_ril_name_pi

===========================================================================*/
/*!
    @brief
    Maps qmi name presentation enum to RIL pi values.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_voice_map_qmi_to_ril_name_pi
(
  pi_name_enum_v02  qmi_name_pi
)
{
  int ril_name_pi = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;

  switch(qmi_name_pi)
  {
    case PRESENTATION_NAME_PRESENTATION_ALLOWED_V02:
      ril_name_pi = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;
      break;
    case PRESENTATION_NAME_PRESENTATION_RESTRICTED_V02:
      ril_name_pi = QCRIL_QMI_VOICE_RIL_PI_RESTRICTED;
      break;
    case PRESENTATION_NAME_UNAVAILABLE_V02:
      ril_name_pi = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
      break;
    case PRESENTATION_NAME_NAME_PRESENTATION_RESTRICTED_V02:
      ril_name_pi = QCRIL_QMI_VOICE_RIL_PI_RESTRICTED;
      break;
    default:
      QCRIL_LOG_INFO("Invalid name presentation %d",qmi_name_pi);
      break;
  }
  return ril_name_pi;
}/* qcril_qmi_voice_map_qmi_to_ril_name_pi */

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_qmi_to_ril_last_call_failure_cause

===========================================================================*/
/*!
    @brief
    Maps qmi last call failure cause enum to RIL last call failure cause values.

    @return
    RIL last call failure cause values.
*/
/*=========================================================================*/
qcril::interfaces::CallFailCause qcril_qmi_voice_map_qmi_to_ril_last_call_failure_cause
(
  call_end_reason_enum_v02  reason,
  uint8_t                   raw_code_valid,
  uint8_t                   raw_code,
  uint8_t                   sip_code_valid,
  uint16_t                  sip_code,
  uint8_t                   end_reason_text_valid,
  char                     *end_reason_text,
  std::string              &fail_cause_str /*!< Output parameter */
)
{
  const char *cause_str = NULL;
  qcril::interfaces::CallFailCause ret = qcril::interfaces::CallFailCause::UNKNOWN;
  switch (reason)
  {
    case CALL_END_CAUSE_CLIENT_END_V02:
      if (!cause_str) cause_str = "Normal call clearing; Client ended the call";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_REL_NORMAL_V02:
    case CALL_END_CAUSE_NETWORK_END_V02:
    case CALL_END_CAUSE_NORMAL_CALL_CLEARING_V02:
      if (!cause_str) cause_str = "Normal call clearing";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_RINGING_RINGBACK_TIMEOUT_V02:
      if (!cause_str) cause_str = "Call terminated due to ringing/ringback timeout";
      ret = qcril::interfaces::CallFailCause::NORMAL;
      break;

    case CALL_END_CAUSE_IMSI_UNKNOWN_IN_VLR_V02:
      cause_str = "IMSI unknown in VLR";
      ret = qcril::interfaces::CallFailCause::IMSI_UNKNOWN_IN_VLR;
      break;

    case CALL_END_CAUSE_IMEI_NOT_ACCEPTED_V02:
      cause_str = "IMEI not accepted";
      ret = qcril::interfaces::CallFailCause::IMEI_NOT_ACCEPTED;
      break;

    case CALL_END_CAUSE_INCOM_REJ_V02:
      if (!cause_str) cause_str = "User busy; client rejected incoming call";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_SETUP_REJ_V02:
      if (!cause_str) cause_str = "User busy; client rejected setup indication";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_USER_BUSY_V02:
    case CALL_END_CAUSE_USSD_BUSY_V02:
    case CALL_END_CAUSE_REJECTED_BY_USER_V02:
      if (!cause_str) cause_str = "User busy";
      ret = qcril::interfaces::CallFailCause::BUSY;
      break;

    case CALL_END_CAUSE_MAX_PS_CALLS_V02:
      if (!cause_str) cause_str = "Resources are not available to handle new MO/MT PS call";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_HOLD_RESUME_FAILED_V02:
      if (!cause_str) cause_str = "Resume failed";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_HOLD_RESUME_CANCELED_V02:
      if (!cause_str) cause_str = "Resume canceled";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_REINVITE_COLLISION_V02:
      if (!cause_str) cause_str = "Re-invite collision";
      ret = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
      break;

    case CALL_END_CAUSE_NETWORK_CONGESTION_V02:
    case CALL_END_CAUSE_RESOURCES_NOT_AVAILABLE_V02:
    case CALL_END_CAUSE_NO_RESOURCES_V02:
    case CALL_END_CAUSE_NO_CIRCUIT_OR_CHANNEL_AVAILABLE_V02:
      if (!cause_str) cause_str = "No circuit/channel available";
      ret = qcril::interfaces::CallFailCause::CONGESTION;
      break;

    case CALL_END_CAUSE_NO_FUNDS_V02:
      if (!cause_str) cause_str = "ACM limit exceeded, no funds";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_ACM_LIMIT_EXCEEDED_V02:
      if (!cause_str) cause_str = "ACM equal to or greater than ACMmax";
      ret = qcril::interfaces::CallFailCause::ACM_LIMIT_EXCEEDED;
      break;

    case CALL_END_CAUSE_CDMA_LOCK_V02:
      cause_str = "CDMA locked";
      ret = qcril::interfaces::CallFailCause::CDMA_LOCKED_UNTIL_POWER_CYCLE;
      break;

    case CALL_END_CAUSE_INTERCEPT_V02:
    case CALL_END_CAUSE_1XCSFB_FAIL_CALL_REL_INTERCEPT_ORDER_V02:
      cause_str = "CDMA intercept";
      ret = qcril::interfaces::CallFailCause::CDMA_INTERCEPT;
      break;

    case CALL_END_CAUSE_REORDER_V02:
    case CALL_END_CAUSE_1XCSFB_FAIL_CALL_REL_REORDER_V02:
      cause_str = "CDMA reorder";
      ret = qcril::interfaces::CallFailCause::CDMA_REORDER;
      break;

    case CALL_END_CAUSE_REL_SO_REJ_V02:
    case CALL_END_CAUSE_1XCSFB_FAIL_CALL_REL_SO_REJ_V02:
      cause_str = "CDMA so reject";
      ret = qcril::interfaces::CallFailCause::CDMA_SO_REJECT;
      break;

    case CALL_END_CAUSE_RETRY_ORDER_V02:
      cause_str = "CDMA retry order";
      ret = qcril::interfaces::CallFailCause::CDMA_RETRY_ORDER;
      break;

    case CALL_END_CAUSE_ACC_FAIL_V02:
    case CALL_END_CAUSE_IS707B_MAX_ACC_V02:
      if (!cause_str) cause_str = "CDMA access failure";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_MAX_ACCESS_PROBE_V02:
      if (!cause_str) cause_str = "CDMA access failure; Maximum access probes transmitted";
      ret = qcril::interfaces::CallFailCause::CDMA_ACCESS_FAILURE;
      break;

    case CALL_END_CAUSE_INCOM_CALL_V02:
      cause_str = "CDMA preemted";
      ret = qcril::interfaces::CallFailCause::CDMA_PREEMPTED;
      break;

    case CALL_END_CAUSE_EMERGENCY_FLASHED_V02:
      cause_str = "CDMA not emergency";
      ret = qcril::interfaces::CallFailCause::CDMA_NOT_EMERGENCY;
      break;

    case CALL_END_CAUSE_ACCESS_BLOCK_ALL_V02:
    case CALL_END_CAUSE_ACCESS_BLOCK_V02:
      if (!cause_str) cause_str = "CDMA access blocked";
      [[clang::fallthrough]];
    case CALL_END_CAUSE_PSIST_N_V02:
      if (!cause_str) cause_str = "CDMA access blocked; Persistence test failure";
      ret = qcril::interfaces::CallFailCause::CDMA_ACCESS_BLOCKED;
      break;

    case CALL_END_CAUSE_UNASSIGNED_NUMBER_V02:
      cause_str = "Unassigned (unallocated) number";
      ret = qcril::interfaces::CallFailCause::UNOBTAINABLE_NUMBER;
      break;

    case CALL_END_CAUSE_PROTOCOL_Q850_ERROR_V02:
      if (end_reason_text_valid)
      {
        cause_str = end_reason_text;
      }
      else
      {
        cause_str = "Sip Q850 error";
      }
      if (sip_code_valid)
      {
        ret = static_cast<qcril::interfaces::CallFailCause>(sip_code);
      }
      else
      {
        ret = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
      }
      break;

    case CALL_END_CAUSE_PEER_NOT_REACHABLE_V02:
      cause_str = "Peer not reachable";
      ret= qcril::interfaces::CallFailCause::SUBSCRIBER_ABSENT;
      break;

    case CALL_END_CAUSE_NO_ROUTE_TO_DESTINATION_V02:
      cause_str = "No route to destination";
      ret = qcril::interfaces::CallFailCause::NO_ROUTE_TO_DESTINATION;
      break;

    case CALL_END_CAUSE_CHANNEL_UNACCEPTABLE_V02:
      cause_str = "Channel unacceptable";
      ret = qcril::interfaces::CallFailCause::CHANNEL_UNACCEPTABLE;
      break;

    case CALL_END_CAUSE_OPERATOR_DETERMINED_BARRING_V02:
      cause_str = "Operator determined Barring";
      ret = qcril::interfaces::CallFailCause::OPERATOR_DETERMINED_BARRING;
      break;

    case CALL_END_CAUSE_NO_USER_RESPONDING_V02:
      cause_str = "No user responding";
      ret = qcril::interfaces::CallFailCause::NO_USER_RESPONDING;
      break;

    case CALL_END_CAUSE_USER_ALERTING_NO_ANSWER_V02:
      cause_str = "User alerting, no answer";
      ret = qcril::interfaces::CallFailCause::NO_ANSWER_FROM_USER;
      break;

    case CALL_END_CAUSE_CALL_REJECTED_V02:
      cause_str = "Call rejected";
      ret = qcril::interfaces::CallFailCause::CALL_REJECTED;
      break;

    case CALL_END_CAUSE_NUMBER_CHANGED_V02:
      cause_str = "Number changed";
      ret = qcril::interfaces::CallFailCause::NUMBER_CHANGED;
      break;

    case CALL_END_CAUSE_PREEMPTION_V02:
      cause_str = "Pre-emption";
      ret = qcril::interfaces::CallFailCause::PREEMPTION;
      break;

    case CALL_END_CAUSE_DESTINATION_OUT_OF_ORDER_V02:
      cause_str = "Destination out of order";
      ret = qcril::interfaces::CallFailCause::DESTINATION_OUT_OF_ORDER;
      break;

    case CALL_END_CAUSE_INVALID_NUMBER_FORMAT_V02:
      cause_str = "Invalid number format (incomplete number)";
      ret = qcril::interfaces::CallFailCause::INVALID_NUMBER_FORMAT;
      break;

    case CALL_END_CAUSE_FACILITY_REJECTED_V02:
      cause_str = "Facility rejected";
      ret = qcril::interfaces::CallFailCause::FACILITY_REJECTED;
      break;

    case CALL_END_CAUSE_RESP_TO_STATUS_ENQUIRY_V02:
      cause_str = "Response to STATUS ENQUIRY";
      ret = qcril::interfaces::CallFailCause::RESP_TO_STATUS_ENQUIRY;
      break;

    case CALL_END_CAUSE_NORMAL_UNSPECIFIED_V02:
      cause_str = "Normal, unspecified";
      if (!raw_code_valid)
      {
        ret = qcril::interfaces::CallFailCause::NORMAL_UNSPECIFIED;
      }
      else
      {
        ret = static_cast<qcril::interfaces::CallFailCause>(raw_code);
        // no macro defined for this particular value
        if (raw_code == 26)
        {
            cause_str = "Non-selected user clearing";
        }
      }
      break;

    case CALL_END_CAUSE_NETWORK_OUT_OF_ORDER_V02:
    case CALL_END_CAUSE_SERVICE_TEMPORARILY_OUT_OF_ORDER_V02:
      cause_str = "Network out of order";
      ret = qcril::interfaces::CallFailCause::NETWORK_OUT_OF_ORDER;
      break;

    case CALL_END_CAUSE_TEMPORARY_FAILURE_V02:
      cause_str = "Temporary failure";
      ret = qcril::interfaces::CallFailCause::TEMPORARY_FAILURE;
      break;

    case CALL_END_CAUSE_SWITCHING_EQUIPMENT_CONGESTION_V02:
      cause_str = "Switching equipment congestion";
      ret = qcril::interfaces::CallFailCause::SWITCHING_EQUIPMENT_CONGESTION;
      break;

    case CALL_END_CAUSE_ACCESS_INFORMATION_DISCARDED_V02:
      cause_str = "Access information discarded";
      ret = qcril::interfaces::CallFailCause::ACCESS_INFORMATION_DISCARDED;
      break;

    case CALL_END_CAUSE_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE_V02:
      cause_str = "Requested circuit/channel not available";
      ret = qcril::interfaces::CallFailCause::REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE;
      break;

    case CALL_END_CAUSE_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED_V02:
      cause_str = "Resource unavailable, unspecified";
      ret = qcril::interfaces::CallFailCause::RESOURCES_UNAVAILABLE_OR_UNSPECIFIED;
      break;

    case CALL_END_CAUSE_QOS_UNAVAILABLE_V02:
      cause_str = "Quality of service unavailable";
      ret = qcril::interfaces::CallFailCause::QOS_UNAVAILABLE;
      break;

    case CALL_END_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED_V02:
      cause_str = "Requested facility not subscribed";
      ret = qcril::interfaces::CallFailCause::REQUESTED_FACILITY_NOT_SUBSCRIBED;
      break;

    case CALL_END_CAUSE_INCOMING_CALLS_BARRED_WITHIN_CUG_V02:
      cause_str = "Incoming calls barred within the CUG";
      ret = qcril::interfaces::CallFailCause::INCOMING_CALLS_BARRED_WITHIN_CUG;
      break;

    case CALL_END_CAUSE_BEARER_CAPABILITY_NOT_AUTH_V02:
      cause_str = "Bearer capability not authorized";
      ret = qcril::interfaces::CallFailCause::BEARER_CAPABILITY_NOT_AUTHORIZED;
      break;

    case CALL_END_CAUSE_BEARER_CAPABILITY_UNAVAILABLE_V02:
      cause_str = "Bearer capability not presently available";
      ret = qcril::interfaces::CallFailCause::BEARER_CAPABILITY_UNAVAILABLE;
      break;

    case CALL_END_CAUSE_SERVICE_OPTION_NOT_AVAILABLE_V02:
      cause_str = "Service or option not available, unspecified";
      ret = qcril::interfaces::CallFailCause::SERVICE_OPTION_NOT_AVAILABLE;
      break;

    case CALL_END_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED_V02:
      cause_str = "Bearer service not implemented";
      ret = qcril::interfaces::CallFailCause::BEARER_SERVICE_NOT_IMPLEMENTED;
      break;

    case CALL_END_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED_V02:
      cause_str = "Requested facility not implemented";
      ret = qcril::interfaces::CallFailCause::REQUESTED_FACILITY_NOT_IMPLEMENTED;
      break;

    case CALL_END_CAUSE_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE_V02:
      cause_str = "Only restricted digital information bearer capability is available";
      ret = qcril::interfaces::CallFailCause::ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE;
      break;

    case CALL_END_CAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED_V02:
      cause_str = "Service or option not implemented, unspecified";
      ret = qcril::interfaces::CallFailCause::SERVICE_OR_OPTION_NOT_IMPLEMENTED;
      break;

    case CALL_END_CAUSE_INVALID_TRANSACTION_IDENTIFIER_V02:
      cause_str = "Invalid transaction identifier value";
      ret = qcril::interfaces::CallFailCause::INVALID_TRANSACTION_IDENTIFIER;
      break;

    case CALL_END_CAUSE_USER_NOT_MEMBER_OF_CUG_V02:
      cause_str = "User not member of CUG";
      ret = qcril::interfaces::CallFailCause::USER_NOT_MEMBER_OF_CUG;
      break;

    case CALL_END_CAUSE_INCOMPATIBLE_DESTINATION_V02:
      cause_str = "Incompatible destination";
      ret = qcril::interfaces::CallFailCause::INCOMPATIBLE_DESTINATION;
      break;

    case CALL_END_CAUSE_INVALID_TRANSIT_NW_SELECTION_V02:
      cause_str = "Invalid transit network selection";
      ret = qcril::interfaces::CallFailCause::INVALID_TRANSIT_NW_SELECTION;
      break;

    case CALL_END_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE_V02:
      cause_str = "Semantically incorrect message";
      ret = qcril::interfaces::CallFailCause::SEMANTICALLY_INCORRECT_MESSAGE;
      break;

    case CALL_END_CAUSE_INVALID_MANDATORY_INFORMATION_V02:
      cause_str = "Invalid mandatory information";
      ret = qcril::interfaces::CallFailCause::INVALID_MANDATORY_INFORMATION;
      break;

    case CALL_END_CAUSE_MESSAGE_TYPE_NON_IMPLEMENTED_V02:
      cause_str = "Message type non-existent or not implemented";
      ret = qcril::interfaces::CallFailCause::MESSAGE_TYPE_NON_IMPLEMENTED;
      break;

    case CALL_END_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE_V02:
      cause_str = "Message type not compatible with protocol state";
      ret = qcril::interfaces::CallFailCause::MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE;
      break;

    case CALL_END_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_V02:
      cause_str = "Information element non-existent or not implemented";
      ret = qcril::interfaces::CallFailCause::INFORMATION_ELEMENT_NON_EXISTENT;
      break;

    case CALL_END_CAUSE_CONDITONAL_IE_ERROR_V02:
      cause_str = "Conditional IE error";
      ret = qcril::interfaces::CallFailCause::CONDITIONAL_IE_ERROR;
      break;

    case CALL_END_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE_V02:
      cause_str = "Message not compatible with protocol state";
      ret = qcril::interfaces::CallFailCause::MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE;
      break;

    case CALL_END_CAUSE_RECOVERY_ON_TIMER_EXPIRED_V02:
      cause_str = "Recovery on timer expiry";
      ret = qcril::interfaces::CallFailCause::RECOVERY_ON_TIMER_EXPIRED;
      break;

    case CALL_END_CAUSE_PROTOCOL_ERROR_UNSPECIFIED_V02:
      cause_str = "Protocol error, unspecified";
      ret = qcril::interfaces::CallFailCause::PROTOCOL_ERROR_UNSPECIFIED;
      break;

    case CALL_END_CAUSE_INTERWORKING_UNSPECIFIED_V02:
      cause_str = "Interworking, unspecified";
      ret = qcril::interfaces::CallFailCause::INTERWORKING_UNSPECIFIED;
      break;

    // DSDS: To enable emergency redial. Android Telephony
    // shall redial emergency call on the other sub on receiving
    // the fail cause. This cause is specific to DSDS and not
    // per spec. Telephony has locally defined the same cause codes.
    case CALL_END_CAUSE_TEMP_REDIAL_ALLOWED_V02:
      if (!cause_str) cause_str = "Temp redial allowed";
      ret = qcril::interfaces::CallFailCause::EMERGENCY_TEMP_FAILURE;
      break;

    case CALL_END_CAUSE_PERM_REDIAL_NOT_NEEDED_V02:
      if (!cause_str) cause_str = "Perm redial not needed";
      ret = qcril::interfaces::CallFailCause::EMERGENCY_PERM_FAILURE;
      break;

    case CALL_END_CAUSE_OFFLINE_V02:
      cause_str = "Radio is offline";
      ret = qcril::interfaces::CallFailCause::RADIO_OFF;
      break;

    case CALL_END_CAUSE_NO_SRV_V02:
    case CALL_END_CAUSE_NO_GW_SRV_V02:
    case CALL_END_CAUSE_NO_FULL_SRV_V02:
    case CALL_END_CAUSE_NO_CDMA_SRV_V02:
    case CALL_END_CAUSE_NO_CELL_AVAILABLE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED_V02:
      cause_str = "Radio is out of service";
      ret = qcril::interfaces::CallFailCause::OUT_OF_SERVICE;
      break;

    case CALL_END_CAUSE_UIM_NOT_PRESENT_V02:
    case CALL_END_CAUSE_INVALID_SIM_V02:
      cause_str = "No valid SIM present";
      ret = qcril::interfaces::CallFailCause::NO_VALID_SIM;
      break;

    case CALL_END_CAUSE_INCOMPATIBLE_V02:
    case CALL_END_CAUSE_ALREADY_IN_TC_V02:
    case CALL_END_CAUSE_USER_CALL_ORIG_DURING_GPS_V02:
    case CALL_END_CAUSE_USER_CALL_ORIG_DURING_SMS_V02:
    case CALL_END_CAUSE_USER_CALL_ORIG_DURING_DATA_V02:
    case CALL_END_CAUSE_TRM_REQ_FAIL_V02:
    case CALL_END_CAUSE_CALL_CANNOT_BE_IDENTIFIED_V02:
    case CALL_END_CAUSE_INCORRECT_SEMANTICS_IN_MESSAGE_V02:
    case CALL_END_CAUSE_MANDATORY_INFORMATION_INVALID_V02:
    case CALL_END_CAUSE_WRONG_STATE_V02:
    case CALL_END_CAUSE_INVALID_USER_DATA_V02:
    case CALL_END_CAUSE_CNM_MM_REL_PENDING_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_LOW_LEVEL_IMMED_RETRY_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_ABORT_RADIO_UNAVAILABLE_V02:
    case CALL_END_CAUSE_THERMAL_EMERGENCY_V02:
    case CALL_END_CAUSE_INTERNAL_ERROR_V02:
      cause_str = "Radio internal error";
      ret = qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR;
      break;

    case CALL_END_CAUSE_CCS_NOT_SUPPORTED_BY_BS_V02:
    case CALL_END_CAUSE_REJECTED_BY_BS_V02:
    case CALL_END_CAUSE_ACC_FAIL_REJ_ORD_V02:
    case CALL_END_CAUSE_ACC_FAIL_RETRY_ORD_V02:
    case CALL_END_CAUSE_UNKNOWN_SUBSCRIBER_V02:
    case CALL_END_CAUSE_ILLEGAL_SUBSCRIBER_V02:
    case CALL_END_CAUSE_BEARER_SERVICE_NOT_PROVISIONED_V02:
    case CALL_END_CAUSE_TELE_SERVICE_NOT_PROVISIONED_V02:
    case CALL_END_CAUSE_ILLEGAL_EQUIPMENT_V02:
    case CALL_END_CAUSE_ILLEGAL_SS_OPERATION_V02:
    case CALL_END_CAUSE_SS_ERROR_STATUS_V02:
    case CALL_END_CAUSE_SS_NOT_AVAILABLE_V02:
    case CALL_END_CAUSE_SS_SUBSCRIPTION_VIOLATION_V02:
    case CALL_END_CAUSE_SS_INCOMPATIBILITY_V02:
    case CALL_END_CAUSE_FACILITY_NOT_SUPPORTED_V02:
    case CALL_END_CAUSE_ABSENT_SUBSCRIBER_V02:
    case CALL_END_CAUSE_SHORT_TERM_DENIAL_V02:
    case CALL_END_CAUSE_LONG_TERM_DENIAL_V02:
    case CALL_END_CAUSE_SYSTEM_FAILURE_V02:
    case CALL_END_CAUSE_REJECTED_BY_NETWORK_V02:
    case CALL_END_CAUSE_IMSI_UNKNOWN_IN_HLR_V02:
    case CALL_END_CAUSE_ILLEGAL_MS_V02:
    case CALL_END_CAUSE_ILLEGAL_ME_V02:
    case CALL_END_CAUSE_PLMN_NOT_ALLOWED_V02:
    case CALL_END_CAUSE_LOCATION_AREA_NOT_ALLOWED_V02:
    case CALL_END_CAUSE_ROAMING_NOT_ALLOWED_IN_THIS_LOCATION_AREA_V02:
    case CALL_END_CAUSE_NO_SUITABLE_CELLS_IN_LOCATION_AREA_V02:
    case CALL_END_CAUSE_NETWORK_FAILURE_V02:
    case CALL_END_CAUSE_MAC_FAILURE_V02:
    case CALL_END_CAUSE_SYNCH_FAILURE_V02:
    case CALL_END_CAUSE_GSM_AUTHENTICATION_UNACCEPTABLE_V02:
    case CALL_END_CAUSE_SERVICE_NOT_SUBSCRIBED_V02:
    case CALL_END_CAUSE_ABORT_MSG_RECEIVED_V02:
    case CALL_END_CAUSE_SERVICE_OPTION_NOT_SUPPORTED_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_CONN_REJECT_V02:
    case CALL_END_CAUSE_UNAUTHORIZED_V02:
    case CALL_END_CAUSE_PAYMENT_REQUIRED_V02:
    case CALL_END_CAUSE_EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT_V02:
    case CALL_END_CAUSE_EMM_REJ_SERVICE_REQ_FAILURE_CS_DOMAIN_NOT_AVAILABLE_V02:
    case CALL_END_CAUSE_EMM_REJ_V02:
      cause_str = "Explicit network reject";
      ret = qcril::interfaces::CallFailCause::NETWORK_REJECT;
      break;

    case CALL_END_CAUSE_NO_RESPONSE_FROM_BS_V02:
    case CALL_END_CAUSE_TIMEOUT_T42_V02:
    case CALL_END_CAUSE_TIMEOUT_T40_V02:
    case CALL_END_CAUSE_T50_EXP_V02:
    case CALL_END_CAUSE_T51_EXP_V02:
    case CALL_END_CAUSE_RL_ACK_TIMEOUT_V02:
    case CALL_END_CAUSE_BAD_FL_V02:
    case CALL_END_CAUSE_TIMEOUT_T41_V02:
    case CALL_END_CAUSE_TIMER_T3230_EXPIRED_V02:
    case CALL_END_CAUSE_TIMER_T303_EXPIRED_V02:
    case CALL_END_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW_V02:
    case CALL_END_CAUSE_EMM_REJ_TIMER_T3417_EXT_EXP_V02:
    case CALL_END_CAUSE_EMM_REJ_TIMER_T3417_EXP_V02:
      cause_str = "No response from network";
      ret = qcril::interfaces::CallFailCause::NETWORK_RESP_TIMEOUT;
      break;

    case CALL_END_CAUSE_CALL_BARRED_V02:
      cause_str = "Call is barred";
      ret = qcril::interfaces::CallFailCause::CALL_BARRED;
      break;

    case CALL_END_CAUSE_ACCESS_STRATUM_FAILURE_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_RR_RANDOM_ACCESS_FAILURE_V02:
    case CALL_END_CAUSE_ESR_FAILURE_V02:
    case CALL_END_CAUSE_CS_ACQ_FAILURE_V02:
      cause_str = "Radio access failure";
      ret = qcril::interfaces::CallFailCause::RADIO_ACCESS_FAILURE;
      break;

    case CALL_END_CAUSE_ACCESS_CLASS_BLOCKED_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_V02:
    case CALL_END_CAUSE_SSAC_REJECT_V02:
      cause_str = "Access class blocked";
      ret = qcril::interfaces::CallFailCause::ACCESS_CLASS_BLOCKED;
      break;

    case CALL_END_CAUSE_FADE_V02:
    case CALL_END_CAUSE_RADIO_LINK_LOST_V02:
      cause_str = "Radio link lost";
      ret = qcril::interfaces::CallFailCause::RADIO_LINK_LOST;
      break;

    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_RR_REL_IND_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_RRC_REL_IND_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_NORMAL_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE_V02:
      cause_str = "RRC connection release, normal";
      ret = qcril::interfaces::CallFailCause::RADIO_RELEASE_NORMAL;
      break;

    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_RRC_CLOSE_SESSION_IND_V02:
    case CALL_END_CAUSE_ACCESS_STRATUM_REJ_RRC_OPEN_SESSION_FAILURE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_CRE_FAILURE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_ABORTED_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS_V02:
      cause_str = "RRC connection release, abnormal";
      ret = qcril::interfaces::CallFailCause::RADIO_RELEASE_ABNORMAL;
      break;

    case CALL_END_CAUSE_AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN_V02:
      cause_str = "Radio uplink failure";
      ret = qcril::interfaces::CallFailCause::RADIO_UPLINK_FAILURE;
      break;

    case CALL_END_CAUSE_AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_REL_RLF_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_RADIO_LINK_FAILURE_V02:
    case CALL_END_CAUSE_CONNECTION_FAILURE_V02:
    case CALL_END_CAUSE_RLF_DURING_CC_DISCONNECT_V02:
      cause_str = "Radio link failure";
      ret = qcril::interfaces::CallFailCause::RADIO_LINK_FAILURE;
      break;

    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED_V02:
    case CALL_END_CAUSE_AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE_V02:
    case CALL_END_CAUSE_CONNECTION_EST_FAILURE_V02:
      cause_str = "Radio setup failure";
      ret = qcril::interfaces::CallFailCause::RADIO_SETUP_FAILURE;
      break;

    case CALL_END_CAUSE_AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH_V02:
    case CALL_END_CAUSE_AS_REJ_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH_V02:
    case CALL_END_CAUSE_PDN_DISCONNECTED_V02:
      cause_str = "Explicit Network Detach";
      ret = qcril::interfaces::CallFailCause::NETWORK_DETACH;
      break;

    default:
      cause_str = "Error unspecified";
      ret = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
      break;
  } /* end switch */
  fail_cause_str = cause_str;
  QCRIL_LOG_ESSENTIAL("map qmi reason: %d to ril reason: %d, ril reason str: %s",
      reason, ret, cause_str);

  return ret;
}/* qcril_qmi_voice_map_qmi_to_ril_last_call_failure_cause */

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_ril_reason_to_str

===========================================================================*/
/*!
    @brief
    Maps last call failure cause enum to string

    @return
    String corresponds to RIL last call failure cause
*/
/*=========================================================================*/
std::string qcril_qmi_voice_map_ril_reason_to_str
(
 qcril::interfaces::CallFailCause reason
)
{
  std::string cause_str;
  switch ( reason )
  {
    case qcril::interfaces::CallFailCause::FDN_BLOCKED:
      cause_str = "FDN Blocked";
      break;

    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL:
      cause_str = "Call Control; dial modified to dial";
      break;

    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_USSD:
      cause_str = "Call Control; dial modified to USSD";
      break;

    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_SS:
      cause_str = "Call Control; dial modified to SS";
      break;

    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO:
      cause_str = "Call Control; dial modified to dial video";
      break;

    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL:
      cause_str = "Call Control; dial video modified to dial";
      break;

    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      cause_str = "Call Control; dial video modified to dial video";
      break;

    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD:
      cause_str = "Call Control; dial video modified to USSD";
      break;

    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS:
      cause_str = "Call Control; dial video modified to SS";
      break;

    case qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR:
      cause_str = "Radio internal error";
      break;

    case qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED:
    default:
      cause_str = "Error unspecified";
      break;
  }

  QCRIL_LOG_ESSENTIAL("ril reason str: %s\n", cause_str.c_str());
  return cause_str;
}

//===========================================================================
// qcril_qmi_voice_last_call_failure_cause_lock
//===========================================================================
static inline void qcril_qmi_voice_last_call_failure_cause_lock()
{
  qcril_qmi_voice_info.last_call_failure_cause.call_failure_cause_lock_mutex.lock();
} // qcril_qmi_voice_last_call_failure_cause_lock

//===========================================================================
// qcril_qmi_voice_last_call_failure_cause_unlock
//===========================================================================
static inline void qcril_qmi_voice_last_call_failure_cause_unlock()
{
  qcril_qmi_voice_info.last_call_failure_cause.call_failure_cause_lock_mutex.unlock();
} // qcril_qmi_voice_last_call_failure_cause_unlock

//===========================================================================
// qcril_qmi_voice_info_lock
//===========================================================================
static inline void qcril_qmi_voice_info_lock()
{
  qcril_qmi_voice_info.voice_info_lock_mutex.lock();
} // qcril_qmi_voice_info_lock

//===========================================================================
// qcril_qmi_voice_info_unlock
//===========================================================================
static inline void qcril_qmi_voice_info_unlock()
{
  qcril_qmi_voice_info.voice_info_lock_mutex.unlock();
} // qcril_qmi_voice_info_unlock

//===========================================================================
// qcril_qmi_voice_diag_odl_lock
//===========================================================================
static inline void qcril_qmi_voice_diag_odl_lock()
{
  diag_odl_logging_in_progress_mutex.lock();
} // qcril_qmi_voice_diag_odl_lock

//===========================================================================
// qcril_qmi_voice_diag_odl_unlock
//===========================================================================
static inline void qcril_qmi_voice_diag_odl_unlock()
{
  diag_odl_logging_in_progress_mutex.unlock();
} // qcril_qmi_voice_diag_odl_unlock

//===========================================================================
// convertCallState
//===========================================================================
qcril::interfaces::CallState convertCallState
(
 RIL_CallState in
)
{
  switch (in) {
    case RIL_CALL_ACTIVE:
      return qcril::interfaces::CallState::ACTIVE;
    case RIL_CALL_HOLDING:
      return qcril::interfaces::CallState::HOLDING;
    case RIL_CALL_DIALING:
      return qcril::interfaces::CallState::DIALING;
    case RIL_CALL_ALERTING:
      return qcril::interfaces::CallState::ALERTING;
    case RIL_CALL_INCOMING:
      return qcril::interfaces::CallState::INCOMING;
    case RIL_CALL_WAITING:
      return qcril::interfaces::CallState::WAITING;
    case RIL_CALL_END:
      return qcril::interfaces::CallState::END;
    default:
      return qcril::interfaces::CallState::UNKNOWN;
  }
} /* convertCallState */

//===========================================================================
// qcril_qmi_ims_map_ril_failcause_to_ims_failcause
//===========================================================================
qcril::interfaces::CallFailCause
qcril_qmi_ims_map_ril_failcause_to_ims_failcause(
    qcril::interfaces::CallFailCause ril_failcause,
    call_end_reason_enum_v02 ims_extended_error_code,
    uint8_t sip_error_code_valid,
    uint16_t sip_error_code
)
{
  qcril::interfaces::CallFailCause ret = qcril::interfaces::CallFailCause::UNKNOWN;

  switch ( ims_extended_error_code )
  {
    case CALL_END_CAUSE_MULTIPLE_CHOICES_V02:
    case CALL_END_CAUSE_MOVED_PERMANENTLY_V02:
    case CALL_END_CAUSE_MOVED_TEMPORARILY_V02:
    case CALL_END_CAUSE_USE_PROXY_V02:
    case CALL_END_CAUSE_ALTERNATE_SERVICE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REDIRECTED;
      break;

    case CALL_END_CAUSE_BAD_REQ_WAIT_INVITE_V02:
    case CALL_END_CAUSE_BAD_REQ_WAIT_REINVITE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_BAD_REQUEST;
      break;

    case CALL_END_CAUSE_SIP_403_FORBIDDEN_V02:
      ret = qcril::interfaces::CallFailCause::SIP_FORBIDDEN;
      break;

    case CALL_END_CAUSE_INVALID_REMOTE_URI_V02:
      ret = qcril::interfaces::CallFailCause::SIP_NOT_FOUND;
      break;

    case CALL_END_CAUSE_UNSUPPORTED_URI_SCHEME_V02:
    case CALL_END_CAUSE_REMOTE_UNSUPP_MEDIA_TYPE_V02:
    case CALL_END_CAUSE_BAD_EXTENSION_V02:
      ret = qcril::interfaces::CallFailCause::SIP_NOT_SUPPORTED;
      break;

    case CALL_END_CAUSE_NETWORK_NO_RESP_TIME_OUT_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REQUEST_TIMEOUT;
      break;

    case CALL_END_CAUSE_PEER_NOT_REACHABLE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE;
      break;

    case CALL_END_CAUSE_ADDRESS_INCOMPLETE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_BAD_ADDRESS;
      break;

    case CALL_END_CAUSE_USER_BUSY_V02:
    case CALL_END_CAUSE_BUSY_EVERYWHERE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_BUSY;
      break;

    case CALL_END_CAUSE_REQUEST_TERMINATED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REQUEST_CANCELLED;
      break;

    case CALL_END_CAUSE_NOT_ACCEPTABLE_V02:
    case CALL_END_CAUSE_NOT_ACCEPTABLE_HERE_V02:
    case CALL_END_CAUSE_SESS_DESCR_NOT_ACCEPTABLE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_NOT_ACCEPTABLE;
      break;

    case CALL_END_CAUSE_GONE_V02:
    case CALL_END_CAUSE_DOES_NOT_EXIST_ANYWHERE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_NOT_REACHABLE;
      break;

    case CALL_END_CAUSE_SERVER_INTERNAL_ERROR_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_INTERNAL_ERROR;
      break;

    case CALL_END_CAUSE_NO_NETWORK_RESP_V02:
    case CALL_END_CAUSE_SIP_503_SERVER_UNAVAILABLE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVICE_UNAVAILABLE;
      break;

    case CALL_END_CAUSE_SERVER_TIME_OUT_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_TIMEOUT;
      break;

    case CALL_END_CAUSE_CALL_REJECTED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_USER_REJECTED;
      break;

    case CALL_END_CAUSE_ANSWERED_ELSEWHERE_V02:
    case CALL_END_CAUSE_CALL_DEFLECTED_V02:
      ret = qcril::interfaces::CallFailCause::ANSWERED_ELSEWHERE;
      break;

    case CALL_END_CAUSE_LOW_BATTERY_V02:
      ret = qcril::interfaces::CallFailCause::LOW_BATTERY;
      break;

    case CALL_END_CAUSE_FALLBACK_TO_CS_V02:
      ret = qcril::interfaces::CallFailCause::CS_RETRY_REQUIRED;
      break;

    case CALL_END_CAUSE_NOT_IMPLEMENTED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED;
      break;

    case CALL_END_CAUSE_BAD_GATEWAY_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_BAD_GATEWAY;
      break;

    case CALL_END_CAUSE_VERSION_NOT_SUPPORTED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED;
      break;

    case CALL_END_CAUSE_MESSAGE_TOO_LARGE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE;
      break;

    case CALL_END_CAUSE_PRECONDITION_FAILURE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE;
      break;

    case CALL_END_CAUSE_HO_NOT_FEASIBLE_V02:
      ret = qcril::interfaces::CallFailCause::HO_NOT_FEASIBLE;
      break;

    case CALL_END_CAUSE_CALL_PULLED_V02:
      ret = qcril::interfaces::CallFailCause::CAUSE_CALL_PULLED;
      break;

    case CALL_END_CAUSE_CALL_PULL_OUT_OF_SYNC_V02:
      ret = qcril::interfaces::CallFailCause::PULL_OUT_OF_SYNC;
      break;

    case CALL_END_CAUSE_HOLD_RESUME_FAILED_V02:
      ret = qcril::interfaces::CallFailCause::HOLD_RESUME_FAILED;
      break;

    case CALL_END_CAUSE_HOLD_RESUME_CANCELED_V02:
      ret = qcril::interfaces::CallFailCause::HOLD_RESUME_CANCELED;
      break;

    case CALL_END_CAUSE_REINVITE_COLLISION_V02:
      ret = qcril::interfaces::CallFailCause::HOLD_REINVITE_COLLISION;
      break;

    case CALL_END_CAUSE_SSAC_REJECT_V02:
      ret = qcril::interfaces::CallFailCause::ACCESS_CLASS_BLOCKED;
      break;

    case CALL_END_CAUSE_ALTERNATE_EMERGENCY_CALL_V02:
      ret = qcril::interfaces::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL;
      break;

    case CALL_END_CAUSE_CODEC_ERROR_V02:
      ret = qcril::interfaces::CallFailCause::MEDIA_INIT_FAILED;
      break;

    case CALL_END_CAUSE_RTP_RTCP_TIMEOUT_V02:
      ret = qcril::interfaces::CallFailCause::MEDIA_NO_DATA;
      break;

    case CALL_END_CAUSE_UNSUPPORTED_SDP_V02:
    case CALL_END_CAUSE_RTP_FAILURE_V02:
      ret = qcril::interfaces::CallFailCause::MEDIA_UNSPECIFIED_ERROR;
      break;

    case CALL_END_CAUSE_CSFB_NOT_FEASIBLE_IN_ROAM_CS_NW_V02:
      ret = qcril::interfaces::CallFailCause::NO_CSFB_IN_CS_ROAM;
      break;

    case CALL_END_CAUSE_SRV_NOT_REGISTERED_V02:
      ret = qcril::interfaces::CallFailCause::SRV_NOT_REGISTERED;
      break;
    case CALL_END_CAUSE_CALL_TYPE_NOT_ALLOWED_V02:
      ret = qcril::interfaces::CallFailCause::CALL_TYPE_NOT_ALLOWED;
      break;
    case CALL_END_CAUSE_EMRG_CALL_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::EMRG_CALL_ONGOING;
      break;
    case CALL_END_CAUSE_CALL_SETUP_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::CALL_SETUP_ONGOING;
      break;
    case CALL_END_CAUSE_MAX_CALL_LIMIT_REACHED_V02:
      ret = qcril::interfaces::CallFailCause::MAX_CALL_LIMIT_REACHED;
      break;
    case CALL_END_CAUSE_UNSUPPORTED_SIP_HDRS_V02:
      ret = qcril::interfaces::CallFailCause::UNSUPPORTED_SIP_HDRS;
      break;
    case CALL_END_CAUSE_CALL_TRANSFER_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::CALL_TRANSFER_ONGOING;
      break;
    case CALL_END_CAUSE_PRACK_TIMEOUT_V02:
      ret = qcril::interfaces::CallFailCause::PRACK_TIMEOUT;
      break;
    case CALL_END_CAUSE_QOS_FAILURE_V02:
      ret = qcril::interfaces::CallFailCause::QOS_FAILURE;
      break;
    case CALL_END_CAUSE_ONGOING_HANDOVER_V02:
      ret = qcril::interfaces::CallFailCause::ONGOING_HANDOVER;
      break;
    case CALL_END_CAUSE_VT_WITH_TTY_NOT_ALLOWED_V02:
      ret = qcril::interfaces::CallFailCause::VT_WITH_TTY_NOT_ALLOWED;
      break;
    case CALL_END_CAUSE_CALL_UPGRADE_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::CALL_UPGRADE_ONGOING;
      break;
    case CALL_END_CAUSE_CONFERENCE_WITH_TTY_NOT_ALLOWED_V02:
      ret = qcril::interfaces::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED;
      break;
    case CALL_END_CAUSE_CALL_CONFERENCE_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::CALL_CONFERENCE_ONGOING;
      break;
    case CALL_END_CAUSE_VT_WITH_AVPF_NOT_ALLOWED_V02:
      ret = qcril::interfaces::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED;
      break;
    case CALL_END_CAUSE_ENCRYPTION_CALL_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::ENCRYPTION_CALL_ONGOING;
      break;
    case CALL_END_CAUSE_INCOMING_REJ_CAUSE_CALL_ONGOING_CW_DISABLED_V02:
      ret = qcril::interfaces::CallFailCause::CALL_ONGOING_CW_DISABLED;
      break;
    case CALL_END_CAUSE_INCOMING_REJ_CAUSE_CALL_ON_OTHER_SUB_V02:
      ret = qcril::interfaces::CallFailCause::CALL_ON_OTHER_SUB;
      break;
    case CALL_END_CAUSE_INCOMING_REJ_CAUSE_1X_COLLISION_V02:
      ret = qcril::interfaces::CallFailCause::ONE_X_COLLISION;
      break;
    case CALL_END_CAUSE_INCOM_REJ_CAUSE_UI_NOT_READY_V02:
      ret = qcril::interfaces::CallFailCause::UI_NOT_READY;
      break;
    case CALL_END_CAUSE_CS_CALL_ONGOING_V02:
      ret = qcril::interfaces::CallFailCause::CS_CALL_ONGOING;
      break;
    case CALL_END_CAUSE_REJECTED_ELSEWHERE_V02:
      ret = qcril::interfaces::CallFailCause::REJECTED_ELSEWHERE;
      break;
    case CALL_END_CAUSE_UPGRADE_DOWNGRADE_REJ_V02:
      ret = qcril::interfaces::CallFailCause::USER_REJECTED_SESSION_MODIFICATION;
      break;
    case CALL_END_CAUSE_UPGRADE_DOWNGRADE_CANCELLED_V02:
      ret = qcril::interfaces::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION;
      break;
    case CALL_END_CAUSE_UPGRADE_DOWNGRADE_FAILED_V02:
      ret = qcril::interfaces::CallFailCause::SESSION_MODIFICATION_FAILED;
      break;
    case CALL_END_CAUSE_METHOD_NOT_ALLOWED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_METHOD_NOT_ALLOWED;
      break;
    case CALL_END_CAUSE_PROXY_AUTHENTICATION_REQUIRED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED;
      break;
    case CALL_END_CAUSE_REQUEST_ENTITY_TOO_LARGE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE;
      break;
    case CALL_END_CAUSE_REQUEST_URI_TOO_LARGE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REQUEST_URI_TOO_LARGE;
      break;
    case CALL_END_CAUSE_EXTENSION_REQUIRED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_EXTENSION_REQUIRED;
      break;
    case CALL_END_CAUSE_INTERVAL_TOO_BRIEF_V02:
      ret = qcril::interfaces::CallFailCause::SIP_INTERVAL_TOO_BRIEF;
      break;
    case CALL_END_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST_V02:
      ret = qcril::interfaces::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
      break;
    case CALL_END_CAUSE_LOOP_DETECTED_V02:
      ret = qcril::interfaces::CallFailCause::SIP_LOOP_DETECTED;
      break;
    case CALL_END_CAUSE_TOO_MANY_HOPS_V02:
      ret = qcril::interfaces::CallFailCause::SIP_TOO_MANY_HOPS;
      break;
    case CALL_END_CAUSE_AMBIGUOUS_V02:
      ret = qcril::interfaces::CallFailCause::SIP_AMBIGUOUS;
      break;
    case CALL_END_CAUSE_REQUEST_PENDING_V02:
      ret = qcril::interfaces::CallFailCause::SIP_REQUEST_PENDING;
      break;
    case CALL_END_CAUSE_UNDECIPHERABLE_V02:
      ret = qcril::interfaces::CallFailCause::SIP_UNDECIPHERABLE;
      break;
    case CALL_END_CAUSE_DEAD_BATTERY_V02:
      ret = qcril::interfaces::CallFailCause::LOW_BATTERY;
      break;
    case CALL_END_CAUSE_RETRY_ON_IMS_WITHOUT_RTT_V02:
      ret = qcril::interfaces::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT;
      break;
    case CALL_END_CAUSE_PROTOCOL_Q850_ERROR_V02:
      if (sip_error_code_valid && sip_error_code == SIP_TEMPORARILY_UNAVAILABLE)
      {
        ret = qcril::interfaces::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE;
      }
      else
      {
        ret = ril_failcause;
      }
      break;
    case CALL_END_CAUSE_DSDA_CONCURRENT_CALL_NOT_POSSIBLE_V02:
      ret = qcril::interfaces::CallFailCause::DSDA_CONCURRENT_CALL_NOT_POSSIBLE;
      break;
    case CALL_END_CAUSE_EPSFB_FAILURE_V02:
      ret = qcril::interfaces::CallFailCause::EPSFB_FAILURE;
      break;
    case CALL_END_CAUSE_TWAIT_EXPIRED_V02:
      ret = qcril::interfaces::CallFailCause::TWAIT_EXPIRED;
      break;
    case CALL_END_CAUSE_TCP_CONNECTION_REQ_V02:
      ret = qcril::interfaces::CallFailCause::TCP_CONNECTION_REQ;
      break;

    default:
      ret = ril_failcause;
      break;
  }

  QCRIL_LOG_INFO("CallFailCause %d with extended error code %d mapped to %s",
                 ril_failcause, ims_extended_error_code, qcril::interfaces::toString(ret).c_str());
  return ret;
} /* qcril_qmi_ims_map_ril_failcause_to_ims_failcause */

//===========================================================================
// qcril_qmi_ims_map_qcril_rtt_mode_to_ims
// ===========================================================================
qcril::interfaces::RttMode qcril_qmi_ims_map_qcril_rtt_mode_to_ims
(
 rtt_mode_type_v02 in
)
{
  qcril::interfaces::RttMode ret = qcril::interfaces::RttMode::UNKNOWN;
  switch (in)
  {
    case RTT_MODE_FULL_V02:
      ret = qcril::interfaces::RttMode::FULL;
      break;
    case RTT_MODE_DISABLED_V02:
      ret = qcril::interfaces::RttMode::DISABLED;
      break;
    default:
      ret = qcril::interfaces::RttMode::UNKNOWN;
      break;
  }
  return ret;
} /* qcril_qmi_ims_map_qcril_rtt_mode_to_ims */

//===========================================================================
// qcril_qmi_ims_map_call_mode_to_ims
//===========================================================================
RIL_RadioTechnology qcril_qmi_ims_map_call_mode_to_ims
(
 call_mode_enum_v02 call_mode
)
{
  RIL_RadioTechnology ims_rat;

  switch(call_mode)
  {
    case CALL_MODE_LTE_V02:
      ims_rat = RADIO_TECH_LTE;
      break;
    case CALL_MODE_WLAN_V02:
      ims_rat = RADIO_TECH_IWLAN;
      break;
    case CALL_MODE_NR5G_V02:
      ims_rat = RADIO_TECH_5G;
      break;
    case CALL_MODE_C_IWLAN_V02:
      ims_rat = RADIO_TECH_C_IWLAN;
      break;
    default:
      ims_rat = RADIO_TECH_UNKNOWN;
      break;
  }

  return ims_rat;
}  /* qcril_qmi_ims_map_call_mode_to_ims */

//===========================================================================
// qcril_qmi_map_verstat_verification_state_to_ims
//===========================================================================
qcril::interfaces::VerificationStatus qcril_qmi_map_verstat_verification_state_to_ims
(
 mt_ims_verstat_enum_v02 verification_state
)
{
  qcril::interfaces::VerificationStatus ret = qcril::interfaces::VerificationStatus::NONE;
  switch (verification_state) {
    case MT_IMS_TN_VALIDATION_PASS_V02:
      ret = qcril::interfaces::VerificationStatus::PASS;
      break;
    case MT_IMS_TN_VALIDATION_FAIL_V02:
      ret = qcril::interfaces::VerificationStatus::FAIL;
      break;
    default:
      ret = qcril::interfaces::VerificationStatus::NONE;
      break;
  }
  return ret;
}  /* qcril_qmi_map_verstat_verification_state_to_ims */

//===========================================================================
// qcril_qmi_map_verstat_info_to_ims
//===========================================================================
std::shared_ptr<qcril::interfaces::VerstatInfo> qcril_qmi_map_verstat_info_to_ims
(
 const voice_MT_call_received_verstat_type_v02 &in
)
{
  std::shared_ptr<qcril::interfaces::VerstatInfo> out =
      std::make_shared<qcril::interfaces::VerstatInfo>();
  if (out) {
    //if (in.network_supports_unwanted_call)
    out->setCanMarkUnwantedCall(in.network_supports_unwanted_call);
    out->setVerificationStatus(
        qcril_qmi_map_verstat_verification_state_to_ims(in.mt_call_verstat_info));
  }
  return out;
} /* qcril_qmi_map_verstat_info_to_ims */

qcril::interfaces::TirMode qcril_qmi_map_tir_mode_to_ims
(
 const sups_mode_enum_v02 &in
)
{
  switch (in)
    {
      case SUPS_MODE_TEMPORARY_V02:
        return qcril::interfaces::TirMode::TEMPORARY;
      case SUPS_MODE_PERMANENT_V02:
        return qcril::interfaces::TirMode::PERMANENT;
      default:
        return qcril::interfaces::TirMode::UNKNOWN;
    }
}

qcril::interfaces::CrsType qcril_qmi_map_crs_type_to_ims
(
 const voice_call_crs_type_enum_v02 &in
 )
{
  switch (in)
  {
    case VOICE_CALL_CRS_TYPE_AUDIO_V02:
      return qcril::interfaces::CrsType::AUDIO;
    case VOICE_CALL_CRS_TYPE_VIDEO_ONLY_V02:
      return qcril::interfaces::CrsType::VIDEO;
    case VOICE_CALL_CRS_TYPE_VIDEO_V02:
      return qcril::interfaces::CrsType::VIDEO_AND_AUDIO;
    default:
      return qcril::interfaces::CrsType::INVALID;
  }
}

qcril::interfaces::CallModifiedCause qcril_qmi_map_call_modified_cause_to_ims
(
 const voice_call_modified_cause_enum_v02 &in
)
{
  switch (in)
  {
    case VOICE_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ_V02:
      return qcril::interfaces::CallModifiedCause::UPGRADE_DUE_TO_LOCAL_REQ;
    case VOICE_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ_V02:
      return qcril::interfaces::CallModifiedCause::UPGRADE_DUE_TO_REMOTE_REQ;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LOCAL_REQ;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_REMOTE_REQ;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_RTP_TIMEOUT;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_QOS_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_QOS;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_PACKET_LOSS;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LOW_THRPUT;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_THERM_MITIGATION;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LIPSYNC;
    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR_V02:
      return qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_GENERIC_ERROR;
    case VOICE_CALL_MODIFIED_CAUSE_NONE_V02:
    default:
      return qcril::interfaces::CallModifiedCause::NONE;
  }
}

qcril::interfaces::CallProgressInfoType qcril_qmi_map_call_prog_info_notif_type
(
 const voice_call_prog_info_notif_type_enum_v02 &in
 )
{
  switch (in)
  {
    case VOICE_CALL_PROG_INFO_NOTIF_TYPE_CALL_REJ_Q850_V02:
      return qcril::interfaces::CallProgressInfoType::CALL_REJ_Q850;
    case VOICE_CALL_PROG_INFO_NOTIF_TYPE_CALL_WAITING_V02:
      return qcril::interfaces::CallProgressInfoType::CALL_WAITING;
    case VOICE_CALL_PROG_INFO_NOTIF_TYPE_CALL_FORWARDING_V02:
      return qcril::interfaces::CallProgressInfoType::CALL_FORWARDING;
    case VOICE_CALL_PROG_INFO_NOTIF_TYPE_REMOTE_AVAILABLE_V02:
      return qcril::interfaces::CallProgressInfoType::REMOTE_AVAILABLE;
    default:
      return qcril::interfaces::CallProgressInfoType::UNKNOWN;
  }
}

qcril::interfaces::MsimAdditionalInfoCode qcril_qmi_map_msim_additional_info_code(
    const voice_msim_additional_call_info_enum_v02& in) {
  switch (in) {
    case VOICE_MSIM_ADDL_INFO_CONCURRENT_CALL_NOT_POSSIBLE_V02:
      return qcril::interfaces::MsimAdditionalInfoCode::CONCURRENT_CALL_NOT_POSSIBLE;
    case VOICE_MSIM_ADDL_INFO_NONE_V02:
    default:
      return qcril::interfaces::MsimAdditionalInfoCode::NONE;
  }
}

//===========================================================================
// qcril_qmi_ims_translate_ril_callcapabilities_to_ServiceStatusInfo
//===========================================================================
std::vector<qcril::interfaces::ServiceStatusInfo>
  qcril_qmi_ims_translate_ril_callcapabilities_to_ServiceStatusInfo
(
 const bool ril_data_valid,
 const voice_ip_call_capabilities_info_type_v02 &ril_data,
 const bool rtt_mode_valid,
 const rtt_mode_type_v02 rtt_mode,
 qcril::interfaces::CallType current_call_type,
 call_mode_enum_v02 call_mode
)
{
  std::vector<qcril::interfaces::ServiceStatusInfo> ims_data;
  RIL_RadioTechnology networkmode = qcril_qmi_ims_map_call_mode_to_ims(call_mode);
  boolean call_type_found = FALSE;
  qcril::interfaces::ServiceStatusInfo info = {};

  if (ril_data_valid)
  {
    info = {};
    // CallType VOICE
    if ((VOICE_CALL_ATTRIB_TX_V02 == ril_data.audio_attrib) ||
        (VOICE_CALL_ATTRIB_RX_V02 == ril_data.audio_attrib) ||
        ((VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02) == ril_data.audio_attrib))
    {
      info.setCallType(qcril::interfaces::CallType::VOICE);
      info.setStatusType(qcril::interfaces::StatusType::ENABLED);
    }
    else
    {
      info.setCallType(qcril::interfaces::CallType::VOICE);
      info.setStatusType(qcril::interfaces::StatusType::DISABLED);
    }
    if (rtt_mode_valid)
    {
      info.setRttMode(qcril_qmi_ims_map_qcril_rtt_mode_to_ims(rtt_mode));
    }
    info.setRestrictCause(ril_data.audio_cause);
    if (current_call_type == info.getCallType())
    {
      std::shared_ptr<qcril::interfaces::AccessTechnologyStatus> accessTechStatus =
          std::make_shared<qcril::interfaces::AccessTechnologyStatus>();
      if (accessTechStatus) {
        accessTechStatus->setStatusType(info.getStatusType());
        accessTechStatus->setNetworkMode(networkmode);
        accessTechStatus->setRestrictCause(ril_data.audio_cause);
        info.setAccessTechnologyStatus(accessTechStatus);
      }
      call_type_found = TRUE;
    }
    ims_data.push_back(info);

    // CallType VT
    info = {};
    if (VOICE_CALL_ATTRIB_TX_V02 == ril_data.video_attrib)
    {
      info.setCallType(qcril::interfaces::CallType::VT_TX);
      info.setStatusType(qcril::interfaces::StatusType::PARTIALLY_ENABLED);
    }
    else if (VOICE_CALL_ATTRIB_RX_V02 == ril_data.video_attrib)
    {
      info.setCallType(qcril::interfaces::CallType::VT_RX);
      info.setStatusType(qcril::interfaces::StatusType::PARTIALLY_ENABLED);
    }
    else if ((VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02) == ril_data.video_attrib)
    {
      info.setCallType(qcril::interfaces::CallType::VT);
      info.setStatusType(qcril::interfaces::StatusType::ENABLED);
    }
    else
    {
      info.setCallType(qcril::interfaces::CallType::VT);
      info.setStatusType(qcril::interfaces::StatusType::DISABLED);
    }
    if (rtt_mode_valid)
    {
      info.setRttMode(qcril_qmi_ims_map_qcril_rtt_mode_to_ims(rtt_mode));
    }
    info.setRestrictCause(ril_data.video_cause);
    if (current_call_type == info.getCallType())
    {
      std::shared_ptr<qcril::interfaces::AccessTechnologyStatus> accessTechStatus =
          std::make_shared<qcril::interfaces::AccessTechnologyStatus>();
      if (accessTechStatus) {
        accessTechStatus->setStatusType(info.getStatusType());
        accessTechStatus->setNetworkMode(networkmode);
        accessTechStatus->setRestrictCause(ril_data.video_cause);
        info.setAccessTechnologyStatus(accessTechStatus);
      }
      call_type_found = TRUE;
    }
    ims_data.push_back(info);
  }
  if (current_call_type != qcril::interfaces::CallType::UNKNOWN &&
      !call_type_found)
  {
    info = {};
    info.setCallType(current_call_type);
    info.setStatusType(qcril::interfaces::StatusType::ENABLED);
    std::shared_ptr<qcril::interfaces::AccessTechnologyStatus> accessTechStatus =
        std::make_shared<qcril::interfaces::AccessTechnologyStatus>();
    if (accessTechStatus) {
      accessTechStatus->setStatusType(qcril::interfaces::StatusType::ENABLED);
      accessTechStatus->setNetworkMode(networkmode);
      accessTechStatus->setRestrictCause(0);
      info.setAccessTechnologyStatus(accessTechStatus);
    }
    ims_data.push_back(info);
  }

  return ims_data;
} /* qcril_qmi_ims_translate_ril_callcapabilities_to_ServiceStatusInfo */

//===========================================================================
// convert_dial_call_req_number
//===========================================================================
RIL_Errno convert_dial_call_req_number
(
 const string &number,
 string &calling_number,
 string &sip_uri_overflow,
 string &sub_address
)
{
  RIL_Errno result = RIL_E_INTERNAL_GENERIC_FAILURE;

  do {
    if (number.size() > 0) {
      std::string::size_type subAddressPosition = std::string::npos;
      if (getVoiceModuleFeature(VoiceFeatureType::SUBADDRESS_SUPPORT_AMP)) {
        subAddressPosition = number.find("&");
      }
      if (subAddressPosition == std::string::npos) {
        if (getVoiceModuleFeature(VoiceFeatureType::SUBADDRESS_SUPPORT) &&
            number[0] != '*') {
          subAddressPosition = number.find("*");
        }
      }
      if (subAddressPosition != std::string::npos) {
        sub_address = number.substr(subAddressPosition + 1);
        if (getVoiceModuleFeature(VoiceFeatureType::SUBADDRESS_IA5_ID_SUPPORT)) {
#define QCRIL_QMI_VOICE_SUBADDRESS_IA5_IDENTIFIER_STR "P" // 0x50
          sub_address = QCRIL_QMI_VOICE_SUBADDRESS_IA5_IDENTIFIER_STR + sub_address;
        }
        if (sub_address.size() > QMI_VOICE_SUBADDRESS_LEN_MAX_V02 - 1) {
          result = RIL_E_INTERNAL_INVALID_ARGUMENTS;
          break;
        }
      }
      calling_number = number;
      if (subAddressPosition != std::string::npos) {
        calling_number = number.substr(0, subAddressPosition);
      }
      if (calling_number.size() > QMI_VOICE_NUMBER_MAX_V02) {
        if (number[0] == '+') {
          sip_uri_overflow = calling_number.substr(QMI_VOICE_NUMBER_MAX_V02);
          calling_number = calling_number.substr(0, QMI_VOICE_NUMBER_MAX_V02);
        } else {
          sip_uri_overflow = calling_number.substr(QMI_VOICE_NUMBER_MAX_V02-1);
          calling_number = calling_number.substr(0, QMI_VOICE_NUMBER_MAX_V02-1);
        }
        if (sip_uri_overflow.size() > QMI_VOICE_SIP_URI_OVERFLOW_MAX_V02) {
          result = RIL_E_INTERNAL_INVALID_ARGUMENTS;
          break;
        }
      }
      result = RIL_E_SUCCESS;
    } else {
      result = RIL_E_INTERNAL_INVALID_ARGUMENTS;
    }
  } while (FALSE);
  return result;
}

//===========================================================================
// convert_dial_call_req_clir_type
//===========================================================================
uint8_t convert_dial_call_req_clir_type
(
 const int &in,
 clir_type_enum_v02 &out
)
{
  uint8_t result = TRUE;

  if (in == QCRIL_QMI_VOICE_SS_CLIR_INVOCATION_OPTION) {
    out = CLIR_INVOCATION_V02;
  } else if (in == QCRIL_QMI_VOICE_SS_CLIR_SUPPRESSION_OPTION) {
    out = CLIR_SUPPRESSION_V02;
  } else {
    result = FALSE;
  }
  return result;
}

//===========================================================================
// convert_dial_call_req_uss
//===========================================================================
uint8_t convert_dial_call_req_uss
(
 const qcril::interfaces::UusInfo &in,
 voice_uus_type_v02 &out
)
{
  if (in.hasData() && !in.getData().empty()) {
    out.uus_data_len = std::min(in.getData().size(), sizeof(out.uus_data));
    memcpy(out.uus_data, in.getData().c_str(), out.uus_data_len);
  }

  switch (in.getType()) {
  case RIL_UUS_TYPE1_IMPLICIT:
    out.uus_type = UUS_TYPE1_IMPLICIT_V02;
    break;

  case RIL_UUS_TYPE1_REQUIRED:
    out.uus_type = UUS_TYPE1_REQUIRED_V02;
    break;

  case RIL_UUS_TYPE1_NOT_REQUIRED:
    out.uus_type = UUS_TYPE1_NOT_REQUIRED_V02;
    break;

  case RIL_UUS_TYPE2_REQUIRED:
    out.uus_type = UUS_TYPE2_REQUIRED_V02;
    break;

  case RIL_UUS_TYPE2_NOT_REQUIRED:
    out.uus_type = UUS_TYPE2_NOT_REQUIRED_V02;
    break;

  case RIL_UUS_TYPE3_REQUIRED:
    out.uus_type = UUS_TYPE3_REQUIRED_V02;
    break;

  case RIL_UUS_TYPE3_NOT_REQUIRED:
    out.uus_type = UUS_TYPE3_NOT_REQUIRED_V02;
    break;

  default:
    out.uus_type = UUS_TYPE_DATA_V02;
    break;
  }
  switch (in.getDcs()) {
  case RIL_UUS_DCS_USP:
    out.uus_dcs = UUS_DCS_USP_V02;
    break;

  case RIL_UUS_DCS_OSIHLP:
    out.uus_dcs = UUS_DCS_OHLP_V02;
    break;

  case RIL_UUS_DCS_X244:
    out.uus_dcs = UUS_DCS_X244_V02;
    break;

  case RIL_UUS_DCS_IA5c:
    out.uus_dcs = UUS_DCS_IA5_V02;
    break;

  case RIL_UUS_DCS_RMCF: // todo: mapping
  default:
    break;
  }

  QCRIL_LOG_INFO("..  UUS info sent type %d, dcs %d, length %d", out.uus_type,
                 out.uus_dcs, out.uus_data_len);
  return TRUE;
}

//===========================================================================
// qcril_qmi_voice_map_qmi_to_ril_num_pi
//===========================================================================
int qcril_qmi_voice_map_qmi_to_ril_num_pi
(
  pi_num_enum_v02  qmi_num_pi
)
{
  int ril_num_pi = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;

  switch(qmi_num_pi)
  {
    case PRESENTATION_NUM_ALLOWED_V02:
      ril_num_pi = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;
      break;
    case PRESENTATION_NUM_RESTRICTED_V02:
      ril_num_pi = QCRIL_QMI_VOICE_RIL_PI_RESTRICTED;
      break;
    case PRESENTATION_NUM_RESERVED_V02:               // fallthough
    case PRESENTATION_NUM_NUM_UNAVAILABLE_V02:
      ril_num_pi = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
      break;
    case PRESENTATION_NUM_PAYPHONE_V02:
      ril_num_pi = QCRIL_QMI_VOICE_RIL_PI_PAYPHONE;
      break;
    default:
      QCRIL_LOG_INFO("Invalid num presentation %d",qmi_num_pi);
      break;
  }
  return ril_num_pi;
}/* qcril_qmi_voice_map_qmi_to_ril_num_pi */

//===========================================================================
// qcril_qmi_voice_map_qmi_to_ril_provision_status
//===========================================================================
boolean qcril_qmi_voice_map_qmi_to_ril_provision_status
(
 provision_status_enum_v02 qmi_provision_status,
 int *ril_provision_status
)
{
  boolean result = FALSE;

  if (ril_provision_status)
  {
    result = TRUE;
    switch(qmi_provision_status)
    {
      case PROVISION_STATUS_NOT_PROVISIONED_V02 :
        *ril_provision_status = QCRIL_QMI_VOICE_CLIR_SRV_NOT_PROVISIONED;
        break;
      case PROVISION_STATUS_PRESENTATION_ALLOWED_V02 :
        *ril_provision_status = QCRIL_QMI_VOICE_CLIR_SRV_PRESENTATION_ALLOWED;
        break;
      case PROVISION_STATUS_PROVISIONED_PERMANENT_V02 :
        *ril_provision_status = QCRIL_QMI_VOICE_CLIR_SRV_PROVISIONED_PERMANENT;
        break;
      case PROVISION_STATUS_PRESENTATION_RESTRICTED_V02:
        *ril_provision_status = QCRIL_QMI_VOICE_CLIR_SRV_PRESENTATION_RESTRICTED;
        break;
      default :
        QCRIL_LOG_INFO("Invalid provision status %d", qmi_provision_status);
        result = FALSE;
        break;
    }
  }

  return result;
}  /* qcril_qmi_voice_map_qmi_to_ril_provision_status */

//===========================================================================
// convertConferenceCallState
//===========================================================================
qcril::interfaces::ConferenceCallState convertConferenceCallState
(
 call_state_enum_v02 in
)
{
  qcril::interfaces::ConferenceCallState result = qcril::interfaces::ConferenceCallState::UNKNOWN;
  switch (in) {
    case CALL_STATE_ORIGINATING_V02:
    case CALL_STATE_CC_IN_PROGRESS_V02:
    case CALL_STATE_ALERTING_V02:
    case CALL_STATE_CONVERSATION_V02:
      result = qcril::interfaces::ConferenceCallState::FOREGROUND;
      break;

    case CALL_STATE_HOLD_V02:
      result = qcril::interfaces::ConferenceCallState::BACKGROUND;
      break;

    case CALL_STATE_INCOMING_V02:
    case CALL_STATE_WAITING_V02:
      result = qcril::interfaces::ConferenceCallState::RINGING;
      break;

    default:
      result = qcril::interfaces::ConferenceCallState::UNKNOWN;
      break;
  }
  return result;
} /* convertConferenceCallState */

//===========================================================================
// convertConferenceParticipantOperation
//===========================================================================
qcril::interfaces::ConfParticipantOperation convertConferenceParticipantOperation
(
 const conf_participant_operation_enum_v02 &in
)
{
  qcril::interfaces::ConfParticipantOperation result =
      qcril::interfaces::ConfParticipantOperation::UNKNOWN;
  switch (in) {
    case VOICE_CONF_PARTICIPANT_ADD_V02:
      result = qcril::interfaces::ConfParticipantOperation::ADD;
      break;

    case VOICE_CONF_PARTICIPANT_REMOVE_V02:
      result = qcril::interfaces::ConfParticipantOperation::REMOVE;
      break;

    default:
      result = qcril::interfaces::ConfParticipantOperation::UNKNOWN;
      break;
  }
  return result;
}  /* convertConferenceParticipantOperation */

uint8_t convert_presentation_to_qmi
(
 const qcril::interfaces::Presentation &in,
 ip_pi_enum_v02 &out
)
{
  uint8_t ret = FALSE;
  switch (in) {
    case qcril::interfaces::Presentation::ALLOWED:
      out = IP_PRESENTATION_NUM_ALLOWED_V02;
      ret = TRUE;
      break;
    case qcril::interfaces::Presentation::RESTRICTED:
      out = IP_PRESENTATION_NUM_RESTRICTED_V02;
      ret = TRUE;
      break;
    case qcril::interfaces::Presentation::DEFAULT:
    case qcril::interfaces::Presentation::UNKNOWN:
    default:
      ret = FALSE;
      break;
  }
  return ret;
}

uint8_t convert_rtt_mode_to_qmi
(
 const qcril::interfaces::RttMode &in,
 rtt_mode_type_v02 &out
)
{
  uint8_t ret = FALSE;
  switch (in) {
    case qcril::interfaces::RttMode::FULL:
      out = RTT_MODE_FULL_V02;
      ret = TRUE;
      break;
    case qcril::interfaces::RttMode::DISABLED:
      out = RTT_MODE_DISABLED_V02;
      ret = TRUE;
      break;
    case qcril::interfaces::RttMode::UNKNOWN:
    default:
      out = RTT_MODE_DISABLED_V02;
      ret = FALSE;
      break;
  }
  return ret;
}

uint8_t convert_call_mode_to_qmi
(
 const RIL_RadioTechnology &in,
 call_mode_enum_v02 &out
)
{
  uint8_t ret = FALSE;
  switch (in) {
    case RADIO_TECH_LTE:
      out = CALL_MODE_LTE_V02;
      ret = TRUE;
      break;
    case RADIO_TECH_WIFI:
    case RADIO_TECH_IWLAN:
      out = CALL_MODE_WLAN_V02;
      ret = TRUE;
      break;
    case RADIO_TECH_C_IWLAN:
      out = CALL_MODE_C_IWLAN_V02;
      break;
    default:
      ret = FALSE;
      break;
  }
  return ret;
}

uint8_t convert_call_fail_reason_to_qmi
(
 const qcril::interfaces::CallFailCause &in,
 call_end_reason_enum_v02 &out
)
{
  uint8_t ret = FALSE;
  switch (in) {
    case qcril::interfaces::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT:
      out = CALL_END_CAUSE_RETRY_ON_IMS_WITHOUT_RTT_V02;
      ret = TRUE;
      break;
    default:
      ret = FALSE;
      break;
  }
  return ret;
}


uint8_t convert_call_info_to_qmi
(
 const qcril::interfaces::CallType ril_callType,
 const qcril::interfaces::CallDomain ril_callDomain,
 const bool is_emergency_ip,
 call_type_enum_v02 &call_type,
 uint8_t &audio_attrib_valid,
 voice_call_attribute_type_mask_v02 &audio_attrib,
 uint8_t &video_attrib_valid,
 voice_call_attribute_type_mask_v02 &video_attrib
)
{
  uint8_t result = TRUE;

  switch (ril_callType) {
    case qcril::interfaces::CallType::VOICE:
      if (ril_callDomain == qcril::interfaces::CallDomain::PS) {
        call_type = (is_emergency_ip ? CALL_TYPE_EMERGENCY_IP_V02 : CALL_TYPE_VOICE_IP_V02);
      } else {
        call_type = CALL_TYPE_VOICE_V02;
      }

      audio_attrib_valid = TRUE;
      audio_attrib = (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02);
      video_attrib_valid = TRUE;
      video_attrib = 0;
      break;

    case qcril::interfaces::CallType::VT_RX:
      /* Video is receive only */
      if (ril_callDomain == qcril::interfaces::CallDomain::PS ||
          ril_callDomain == qcril::interfaces::CallDomain::AUTOMATIC) {
        audio_attrib_valid = TRUE;
        audio_attrib = (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02);
        video_attrib_valid = TRUE;
        video_attrib = VOICE_CALL_ATTRIB_RX_V02;
        call_type = (is_emergency_ip ? CALL_TYPE_EMERGENCY_VT_V02 : CALL_TYPE_VT_V02);
      } else {
        /* other domain values are not supported currently */
        result = FALSE;
      }
      break;

    case qcril::interfaces::CallType::VT_TX:
      /* Video is transmit only */
      if (ril_callDomain == qcril::interfaces::CallDomain::PS ||
          ril_callDomain == qcril::interfaces::CallDomain::AUTOMATIC) {
        audio_attrib_valid = TRUE;
        audio_attrib = (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02);
        video_attrib_valid = TRUE;
        video_attrib = VOICE_CALL_ATTRIB_TX_V02;
        call_type = (is_emergency_ip ? CALL_TYPE_EMERGENCY_VT_V02 : CALL_TYPE_VT_V02);
      } else {
        /* other domain values are not supported currently */
        result = FALSE;
      }
      break;

    case qcril::interfaces::CallType::VT:
      /* Video is transmit only */
      if (ril_callDomain == qcril::interfaces::CallDomain::PS ||
          ril_callDomain == qcril::interfaces::CallDomain::AUTOMATIC) {
        audio_attrib_valid = TRUE;
        audio_attrib = VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02;
        video_attrib_valid = TRUE;
        video_attrib = VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02;
        call_type = (is_emergency_ip ? CALL_TYPE_EMERGENCY_VT_V02 : CALL_TYPE_VT_V02);
      } else {
        /* other domain values are not supported currently */
        result = FALSE;
      }
      break;

    case qcril::interfaces::CallType::VT_NODIR:
      if (ril_callDomain == qcril::interfaces::CallDomain::PS ||
          ril_callDomain == qcril::interfaces::CallDomain::AUTOMATIC) {
        audio_attrib_valid = TRUE;
        audio_attrib = VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02;
        video_attrib_valid = TRUE;
        video_attrib = 0;
        call_type = (is_emergency_ip ? CALL_TYPE_EMERGENCY_VT_V02 : CALL_TYPE_VT_V02);
      }
      break;

    default:
      result = FALSE;
      break;
  }

  QCRIL_LOG_DEBUG("result = %d, modem call type = %d, ril call type = %d, ril call domain = %d",
                  result, call_type, ril_callType, ril_callDomain);
  QCRIL_LOG_DEBUG("audio_attrib_valid = %d, audio_attrib = %d", audio_attrib_valid, audio_attrib);
  QCRIL_LOG_DEBUG("video_attrib_valid = %d, video_attrib = %d", video_attrib_valid, video_attrib);

  return result;
}

bool qcril_qmi_ims_map_ims_failcause_qmi
(
 const qcril::interfaces::CallFailCause ril_failcause,
 voice_reject_cause_enum_v02 &qmi_failcause
)
{
  switch (ril_failcause) {
    case qcril::interfaces::CallFailCause::BLACKLISTED_CALL_ID:
      qmi_failcause = VOICE_REJECT_CAUSE_BLACKLISTED_CALL_ID_V02;
      break;

      // Modem expects user busy, when the user rejects the call
      // Reason for inverted mapping.
    case qcril::interfaces::CallFailCause::USER_BUSY:
      qmi_failcause = VOICE_REJECT_CAUSE_USER_REJECT_V02;
      break;

    case qcril::interfaces::CallFailCause::USER_REJECT:
      qmi_failcause = VOICE_REJECT_CAUSE_USER_BUSY_V02;
      break;

    case qcril::interfaces::CallFailCause::LOW_BATTERY:
      qmi_failcause = VOICE_REJECT_CAUSE_LOW_BATTERY_V02;
      break;

    case qcril::interfaces::CallFailCause::SIP_USER_MARKED_UNWANTED:
      qmi_failcause = VOICE_REJECT_CAUSE_UNWANTED_CALL_V02;
      break;

    default:
      return false;
  }
  return true;
}

bool qcril_qmi_voice_map_ril_ect_type_to_qmi_ect_type
(
 voip_ect_type_enum_v02 &out,
 qcril::interfaces::EctType in
)
{
  bool result = true;
  switch (in) {
    case qcril::interfaces::EctType::BLIND_TRANSFER:
      out = ECT_TYPE_BLIND_TRANSFER_V02;
      break;
    case qcril::interfaces::EctType::ASSURED_TRANSFER:
      out = ECT_TYPE_ASSURED_TRANSFER_V02;
      break;
    case qcril::interfaces::EctType::CONSULTATIVE_TRANSFER:
      out = ECT_TYPE_CONSULTATIVE_TRANSFER_V02;
      break;
    case qcril::interfaces::EctType::UNKNOWN:
    default:
      result = false;
      break;
  }
  return result;
}

bool qcril_qmi_voice_map_ril_tty_mode_to_qmi_tty_mode
(
 tty_mode_enum_v02 &out,
 qcril::interfaces::TtyMode in
)
{
  bool result = true;
  switch (in) {
    case qcril::interfaces::TtyMode::FULL:
      out = TTY_MODE_FULL_V02;
      break;
    case qcril::interfaces::TtyMode::HCO:
      out = TTY_MODE_HCO_V02;
      break;
    case qcril::interfaces::TtyMode::VCO:
      out = TTY_MODE_VCO_V02;
      break;
    case qcril::interfaces::TtyMode::MODE_OFF:
      out = TTY_MODE_OFF_V02;
      break;
    case qcril::interfaces::TtyMode::UNKNOWN:
    default:
      result = false;
      break;
  }
  return result;
}

call_composer_importance_enum_v02 convert_priority_to_qmi
(
 const qcril::interfaces::Priority &in
)
{
  switch (in) {
    case qcril::interfaces::Priority::URGENT:
      return CALL_COMP_IMP_URGENT_V02;
    default:
      return CALL_COMP_IMP_NORMAL_V02;
  }
}

boolean qcril_qmi_voice_translate_ril_callfwdtimerinfo_to_voice_time_type
(
 voice_time_type_v02 &out,
 const std::shared_ptr<qcril::interfaces::CallFwdTimerInfo> in
)
{
  boolean result = FALSE;
  if (in != nullptr) {
    if (in->hasYear()) {
      out.year = in->getYear();
    }
    if (in->hasMonth()) {
      out.month = in->getMonth();
    }
    if (in->hasDay()) {
      out.day = in->getDay();
    }
    if (in->hasHour()) {
      out.hour = in->getHour();
    }
    if (in->hasMinute()) {
      out.minute = in->getMinute();
    }
    if (in->hasSecond()) {
      out.second = in->getSecond();
    }
    if (in->hasTimezone()) {
      out.time_zone = in->getTimezone();
    }
    result = TRUE;
  }
  return result;
}

boolean qcril_qmi_voice_translate_voice_time_type_to_ril_callfwdtimerinfo
(
 std::shared_ptr<qcril::interfaces::CallFwdTimerInfo> out,
 const voice_time_type_v02 &in
)
{
  boolean result = FALSE;

  if (out != nullptr) {
    out->setYear(in.year);
    out->setMonth(in.month);
    out->setDay(in.day);
    out->setHour(in.hour);
    out->setMinute(in.minute);
    out->setSecond(in.second);
    out->setTimezone(in.time_zone);
    result = TRUE;
  }

  return result;
}

std::shared_ptr<qcril::interfaces::SipErrorInfo> buildSipErrorInfo
(
 uint8_t sipErrorCodeValid,
 uint16_t sipErrorCode,
 uint32_t failureCauseDescriptionLen,
 uint16_t *failureCauseDescription
)
{
  char sipErrUtf8Str[QCRIL_QMI_VOICE_MAX_SUPS_FAILURE_STR_LEN];
  int utf8Len = 0;
  std::shared_ptr<qcril::interfaces::SipErrorInfo> out =
      std::make_shared<qcril::interfaces::SipErrorInfo>();

  if (out != nullptr) {
    if (sipErrorCodeValid == TRUE) {
      out->setErrorCode(sipErrorCode);
    }

    if (failureCauseDescriptionLen > 0) {
      memset(sipErrUtf8Str, 0x0, sizeof(sipErrUtf8Str));
      utf8Len = qcril_cm_ss_convert_ucs2_to_utf8((char *)failureCauseDescription,
                                                 failureCauseDescriptionLen * 2, sipErrUtf8Str,
                                                 sizeof(sipErrUtf8Str));
      utf8Len = utf8Len + 1;
      if (utf8Len > (QCRIL_QMI_VOICE_MAX_SUPS_FAILURE_STR_LEN)) {
        QCRIL_LOG_ERROR("Length exceeds maximum sip error size");
        utf8Len = (int)(QCRIL_QMI_VOICE_MAX_SUPS_FAILURE_STR_LEN);
        sipErrUtf8Str[utf8Len - 1] = '\0';
      }
      QCRIL_LOG_ERROR("QCRIL QMI VOICE CALL FORWARD RESP len =%d, failure_description=%s", utf8Len,
                      sipErrUtf8Str);
      out->setErrorString(sipErrUtf8Str);
    }
  }
  return out;
}

qcril::interfaces::ServiceClassStatus mapActiveStatus
(
 active_status_enum_v02 in
)
{
  switch (in) {
    case ACTIVE_STATUS_INACTIVE_V02:
      return qcril::interfaces::ServiceClassStatus::DISABLED;
    case ACTIVE_STATUS_ACTIVE_V02:
      return qcril::interfaces::ServiceClassStatus::ENABLED;
    default:
      return qcril::interfaces::ServiceClassStatus::UNKNOWN;
  }
}

qcril::interfaces::ServiceClassProvisionStatus mapProvisionStatus
(
 provision_status_enum_v02 in
)
{
  switch (in) {
    case PROVISION_STATUS_NOT_PROVISIONED_V02:
      return qcril::interfaces::ServiceClassProvisionStatus::NOT_PROVISIONED;
    case PROVISION_STATUS_PROVISIONED_PERMANENT_V02: /* Consider PERMANENT as PROVISIONED */
    case PROVISION_STATUS_PRESENTATION_ALLOWED_V02:
      return qcril::interfaces::ServiceClassProvisionStatus::PROVISIONED;
    default:
      return qcril::interfaces::ServiceClassProvisionStatus::UNKNOWN;
  }
}

qcril::interfaces::ClipStatus mapProvisionActiveStatusToClipStatus
(
 provision_status_enum_v02 provision,
 active_status_enum_v02 active,
 sups_domain_enum_v02 domain
)
{
  switch (provision) {
    case PROVISION_STATUS_NOT_PROVISIONED_V02:
      return qcril::interfaces::ClipStatus::NOT_PROVISIONED;
    case PROVISION_STATUS_PROVISIONED_PERMANENT_V02:
      if (domain == SUPS_DOMAIN_PS_V02) {
        if (active == ACTIVE_STATUS_ACTIVE_V02) {
          return qcril::interfaces::ClipStatus::PROVISIONED;
        } else {
          return qcril::interfaces::ClipStatus::NOT_PROVISIONED;
        }
      }
      return qcril::interfaces::ClipStatus::PROVISIONED;
    case PROVISION_STATUS_PRESENTATION_ALLOWED_V02:
    case PROVISION_STATUS_PRESENTATION_RESTRICTED_V02:
    default:
      return qcril::interfaces::ClipStatus::UNKNOWN;
  }
}

qcril::interfaces::Presentation mapColrPi
(
 pi_colr_enum_v02 in
)
{
  switch (in) {
    case COLR_PRESENTATION_NOT_RESTRICTED_V02:
      return qcril::interfaces::Presentation::ALLOWED;
    case COLR_PRESENTATION_RESTRICTED_V02:
      return qcril::interfaces::Presentation::RESTRICTED;
    default:
      return qcril::interfaces::Presentation::UNKNOWN;
  }
}

bool mapFacilityType
(
 voice_reason_enum_v02 &out,
 qcril::interfaces::FacilityType in
)
{
  bool result = true;
  switch (in) {
    case qcril::interfaces::FacilityType::CLIP:
      out = VOICE_REASON_CLIP_V02;
      break;
    case qcril::interfaces::FacilityType::COLP:
      out = VOICE_REASON_COLP_V02;
      break;
    case qcril::interfaces::FacilityType::BAOC:
      out = VOICE_REASON_BARR_ALLOUTGOING_V02;
      break;
    case qcril::interfaces::FacilityType::BAOIC:
      out = VOICE_REASON_BARR_OUTGOINGINT_V02;
      break;
    case qcril::interfaces::FacilityType::BAOICxH:
      out = VOICE_REASON_BARR_OUTGOINGINTEXTOHOME_V02;
      break;
    case qcril::interfaces::FacilityType::BAIC:
      out = VOICE_REASON_BARR_ALLINCOMING_V02;
      break;
    case qcril::interfaces::FacilityType::BAICr:
      out = VOICE_REASON_BARR_INCOMINGROAMING_V02;
      break;
    case qcril::interfaces::FacilityType::BA_ALL:
      out = VOICE_REASON_BARR_ALLBARRING_V02;
      break;
    case qcril::interfaces::FacilityType::BA_MO:
      out = VOICE_REASON_BARR_ALLOUTGOINGBARRING_V02;
      break;
    case qcril::interfaces::FacilityType::BA_MT:
      out = VOICE_REASON_BARR_ALLINCOMINGBARRING_V02;
      break;
    case qcril::interfaces::FacilityType::BS_MT:
      out = VOICE_REASON_BARR_INCOMING_NUMBER_V02;
      break;
    case qcril::interfaces::FacilityType::BAICa:
      out = VOICE_REASON_BARR_INCOMING_ANONYMOUS_V02;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

bool mapFacilityType
(
 qcril::interfaces::FacilityType &out,
 voice_reason_enum_v02 in
)
{
  bool result = true;
  switch (in) {
    case VOICE_REASON_CLIP_V02:
      out = qcril::interfaces::FacilityType::CLIP;
      break;
    case VOICE_REASON_COLP_V02:
      out = qcril::interfaces::FacilityType::COLP;
      break;
    case VOICE_REASON_BARR_ALLOUTGOING_V02:
      out = qcril::interfaces::FacilityType::BAOC;
      break;
    case VOICE_REASON_BARR_OUTGOINGINT_V02:
      out = qcril::interfaces::FacilityType::BAOIC;
      break;
    case VOICE_REASON_BARR_OUTGOINGINTEXTOHOME_V02:
      out = qcril::interfaces::FacilityType::BAOICxH;
      break;
    case VOICE_REASON_BARR_ALLINCOMING_V02:
      out = qcril::interfaces::FacilityType::BAIC;
      break;
    case VOICE_REASON_BARR_INCOMINGROAMING_V02:
      out = qcril::interfaces::FacilityType::BAICr;
      break;
    case VOICE_REASON_BARR_ALLBARRING_V02:
      out = qcril::interfaces::FacilityType::BA_ALL;
      break;
    case VOICE_REASON_BARR_ALLOUTGOINGBARRING_V02:
      out = qcril::interfaces::FacilityType::BA_MO;
      break;
    case VOICE_REASON_BARR_ALLINCOMINGBARRING_V02:
      out = qcril::interfaces::FacilityType::BA_MT;
      break;
    case VOICE_REASON_BARR_INCOMING_NUMBER_V02:
      out = qcril::interfaces::FacilityType::BS_MT;
      break;
    case VOICE_REASON_BARR_INCOMING_ANONYMOUS_V02:
      out = qcril::interfaces::FacilityType::BAICa;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

std::shared_ptr<qcril::interfaces::ColrInfo> buildColrInfo
(
 const voice_get_colr_resp_msg_v02 *const qmiResp
)
{
  QCRIL_LOG_FUNC_ENTRY();

  auto out = std::make_shared<qcril::interfaces::ColrInfo>();

  if (out != nullptr) {
    if (qmiResp->colr_response_valid) {
      QCRIL_LOG_INFO("COLR RESP with status %d and provisionStatus %d",
                     (int)qmiResp->colr_response.active_status,
                     (int)qmiResp->colr_response.provision_status);

      out->setStatus(mapActiveStatus(qmiResp->colr_response.active_status));
      out->setProvisionStatus(mapProvisionStatus(qmiResp->colr_response.provision_status));
    }

    if (qmiResp->colr_pi_valid) {
      out->setPresentation(mapColrPi(qmiResp->colr_pi));
    }

    if (qmiResp->sip_error_code_valid || qmiResp->failure_cause_description_valid) {
      auto errorDetails = buildSipErrorInfo(
          qmiResp->sip_error_code_valid, qmiResp->sip_error_code,
          qmiResp->failure_cause_description_valid ? qmiResp->failure_cause_description_len : 0,
          (uint16_t *)&(qmiResp->failure_cause_description));
      out->setErrorDetails(errorDetails);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return out;
}


//===========================================================================
// qcril_qmi_voice_pend_emergency_call
//===========================================================================
boolean qcril_qmi_voice_pend_emergency_call
(
 uint16_t req_id,
 voice_dial_call_req_msg_v02 &dial_call_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler,
 qcril_qmi_voice_voip_call_info_elaboration_type elaboration
)
{
  PEND_EMER_LOCK();
  qcril_qmi_pending_emergency_call_info = new qcril_qmi_pending_call_info_type(
      req_id, dial_call_req, responseCb, commandOversightCompletionHandler, elaboration);
  PEND_EMER_UNLOCK();
  return TRUE;
}  // qcril_qmi_voice_pend_emergency_call

//===========================================================================
// qcril_qmi_voice_cancel_pending_emergency_call
//===========================================================================
boolean qcril_qmi_voice_cancel_pending_emergency_call(bool send_response)
{
  boolean is_pending_ecall_clearing_status = FALSE;

  QCRIL_LOG_FUNC_ENTRY();
  PEND_EMER_LOCK();
  if (qcril_qmi_pending_emergency_call_info != nullptr)
  {
    QCRIL_LOG_DEBUG("cancel pending emergency call...");
    is_pending_ecall_clearing_status = TRUE;
    if (send_response &&
        qcril_qmi_pending_emergency_call_info->commandOversightCompletionHandler)
    {
      CommonVoiceResponseData respData = {qcril_qmi_pending_emergency_call_info->req_id,
                                          RIL_E_INTERNAL_GENERIC_FAILURE, nullptr};
      qcril_qmi_pending_emergency_call_info->commandOversightCompletionHandler(&respData);
    }
    delete qcril_qmi_pending_emergency_call_info;
    qcril_qmi_pending_emergency_call_info = nullptr;
  }
  PEND_EMER_UNLOCK();
  QCRIL_LOG_FUNC_RETURN_WITH_RET(is_pending_ecall_clearing_status);
  return is_pending_ecall_clearing_status;
} /* qcril_qmi_voice_cancel_pending_emergency_call */

//===========================================================================
// qcril_qmi_voice_trigger_possible_pending_emergency_call
//===========================================================================
void qcril_qmi_voice_trigger_possible_pending_emergency_call()
{
  PEND_EMER_LOCK();
  if (qcril_qmi_pending_emergency_call_info != nullptr)
  {
    QCRIL_LOG_DEBUG("trigger pending emergency call...");
    auto msg = std::make_shared<VoiceRequestPendingDialMessage>(
        *qcril_qmi_pending_emergency_call_info);
    Dispatcher::getInstance().dispatch(msg);

    delete qcril_qmi_pending_emergency_call_info;
    qcril_qmi_pending_emergency_call_info = nullptr;
  }
  PEND_EMER_UNLOCK();
} // qcril_qmi_voice_trigger_possible_pending_emergency_call

//===========================================================================
// qcril_qmi_voice_pending_wps_calls
//===========================================================================
boolean qcril_qmi_voice_pending_wps_calls
(
 uint16_t req_id,
 voice_dial_call_req_msg_v02 &dial_call_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler,
 qcril_qmi_voice_voip_call_info_elaboration_type elaboration
)
{
  qcril_qmi_pending_wps_call_info = new qcril_qmi_pending_call_info_type(
      req_id, dial_call_req, responseCb, commandOversightCompletionHandler, elaboration);
  return TRUE;
} // qcril_qmi_voice_pending_wps_calls

//===========================================================================
// qcril_qmi_voice_trigger_possible_pending_wps_call
//===========================================================================
void qcril_qmi_voice_trigger_possible_pending_wps_call()
{
  if (qcril_qmi_pending_wps_call_info != nullptr)
  {
    QCRIL_LOG_DEBUG("trigger pending wps call...");
    auto msg = std::make_shared<VoiceRequestPendingDialMessage>(*qcril_qmi_pending_wps_call_info);
    Dispatcher::getInstance().dispatch(msg);
    delete qcril_qmi_pending_wps_call_info;
    qcril_qmi_pending_wps_call_info = nullptr;
  }
} // qcril_qmi_voice_trigger_possible_pending_wps_call

//===========================================================================
// qcril_qmi_voice_cancel_pending_wps_call
//===========================================================================
boolean qcril_qmi_voice_cancel_pending_wps_call(bool send_response)
{
  boolean status = FALSE;

  QCRIL_LOG_FUNC_ENTRY();
  if (qcril_qmi_pending_wps_call_info != nullptr)
  {
    QCRIL_LOG_DEBUG("cancel pending wps call...");
    status = TRUE;

    if (send_response &&
        qcril_qmi_pending_wps_call_info->commandOversightCompletionHandler)
    {
      CommonVoiceResponseData respData = {qcril_qmi_pending_wps_call_info->req_id,
                                          RIL_E_INTERNAL_GENERIC_FAILURE, nullptr};
      qcril_qmi_pending_wps_call_info->commandOversightCompletionHandler(&respData);
    }
    delete qcril_qmi_pending_wps_call_info;
    qcril_qmi_pending_wps_call_info = nullptr;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(status);
  return status;
} /* qcril_qmi_voice_cancel_pending_wps_call */

boolean qcril_qmi_voice_cancel_pending_call(bool send_response)
{
  auto pendingMsg =
        findVoiceModulePendingMessage(QcRilRequestDialMessage::get_class_message_id());
  if(pendingMsg) {
    auto msg(std::static_pointer_cast<QcRilRequestDialMessage>(pendingMsg));
    if(msg && msg->getPendingMessageState() == PendingMessageState::AWAITING_PRECONDITIONS) {
      eraseVoiceModulePendingMessage(pendingMsg);
      auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_INTERNAL_GENERIC_FAILURE,
        nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      return TRUE;
    }
  }
  boolean ret = qcril_qmi_voice_cancel_pending_emergency_call(send_response);
  if (!ret)
  {
    ret = qcril_qmi_voice_cancel_pending_wps_call(send_response);
  } else {
    // Notify to DMS that pending call is cancelled. If emergency call is dialled
    // when APM is ON, modem will enter emergency scan mode and wait for dial request
    // forever, DMS Module need to inform modem that DIAL request is cancelled (This is
    // achieved by sending ONLINE request to modem without emergency flag).
    auto ind_msg_ptr =
      std::make_shared<VoiceDialSetUpInd>();
    if (ind_msg_ptr != nullptr)
    {
      ind_msg_ptr->setStatus(RIL_E_CANCELLED);
      ind_msg_ptr->broadcast();
    }
  }
  return ret;
}


//===========================================================================
// qcril_qmi_voice_store_last_call_failure_cause_ex
//===========================================================================
void qcril_qmi_voice_store_last_call_failure_cause_ex
(
 qcril::interfaces::CallFailCause reason,
 std::string reason_str
)
{
  QCRIL_LOG_INFO("store ril reason: %d", reason);
  qcril_qmi_voice_last_call_failure_cause_lock();
  qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_valid = TRUE;
  qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause = reason;
  qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_str = reason_str;
  qcril_qmi_voice_last_call_failure_cause_unlock();
} // qcril_qmi_voice_store_last_call_failure_cause_ex

//===========================================================================
// qcril_qmi_voice_invalid_last_call_failure_cause
//===========================================================================
void qcril_qmi_voice_invalid_last_call_failure_cause()
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_voice_last_call_failure_cause_lock();
  qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_valid = FALSE;
  qcril_qmi_voice_last_call_failure_cause_unlock();
} // qcril_qmi_voice_invalid_last_call_failure_cause

//===========================================================================
// qcril_qmi_voice_set_last_call_fail_request_timeout
//===========================================================================
void qcril_qmi_voice_set_last_call_fail_request_timeout()
{
  QCRIL_LOG_FUNC_ENTRY();

  auto timer_id = TimeKeeper::getInstance().set_timer(
            qcril_qmi_voice_last_call_fail_request_timeout_handler,
            nullptr,
            3000); /* 3 seconds */

  qcril_qmi_voice_last_call_failure_cause_lock();
  qcril_qmi_voice_info.last_call_failure_cause.pending_req = TRUE;
  qcril_qmi_voice_info.last_call_failure_cause.pending_request_timeout_timer_id = timer_id;
  qcril_qmi_voice_last_call_failure_cause_unlock();

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_set_last_call_fail_request_timeout

//===========================================================================
// qcril_qmi_voice_last_call_fail_request_timeout_handler
//===========================================================================
void qcril_qmi_voice_last_call_fail_request_timeout_handler(void * param)
{
  boolean response_sent = FALSE;

  QCRIL_NOTUSED( param );

  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_voice_last_call_failure_cause_lock();
  if (qcril_qmi_voice_info.last_call_failure_cause.pending_req)
  {
    /* Lookup the LastCallFailCause request */
    auto pendingMsg = extractVoiceModulePendingMessage(
        QcRilRequestLastCallFailCauseMessage::get_class_message_id());
    if (pendingMsg != nullptr)
    {
      auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
      auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_INTERNAL_ERR, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      response_sent = TRUE;
    }
    qcril_qmi_voice_info.last_call_failure_cause.pending_req = FALSE;
  }
  qcril_qmi_voice_last_call_failure_cause_unlock();

  if (response_sent)
  {
    qcril_qmi_voice_voip_lock_overview();
    qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl();
    qcril_qmi_voice_voip_unlock_overview();

    if (qcril_qmi_voice_voip_call_info_entries_is_empty())
    {
        QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
        if (!qcril_stop_diag_log())
        {
            QCRIL_LOG_INFO("qxdm logging disabled successfully");
        }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_last_call_fail_request_timeout_handler

//===========================================================================
// qcril_qmi_voice_respond_ril_last_call_failure_request
//===========================================================================
void qcril_qmi_voice_respond_ril_last_call_failure_request()
{
  int call_obj_review_needed = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_voice_last_call_failure_cause_lock();

  if ( TRUE == qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_valid )
  {
    // find all pending request if we have and send the resoponse
    QCRIL_LOG_INFO(
        "last_call_failure_cause is valid. Will send the response if there is any pending ril "
        "request.");
    /* Lookup the LastCallFailCause request */
    auto pendingMsg = extractVoiceModulePendingMessage(
        QcRilRequestLastCallFailCauseMessage::get_class_message_id());
    if (pendingMsg != nullptr)
    {
      call_obj_review_needed = TRUE;

      QCRIL_LOG_DEBUG("Reply to RIL --> Last call fail cause : %d",
                      qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause);

      auto callFailCause = std::make_shared<qcril::interfaces::LastCallFailCauseInfo>();
      if (callFailCause)
      {
        callFailCause->setCallFailCause(
              qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause);
        if (!qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_str.empty())
        {
          callFailCause->setCallFailCauseDescription(
              qcril_qmi_voice_info.last_call_failure_cause.last_call_failure_cause_str);
        }

        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        auto respPayload =
          std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_SUCCESS, callFailCause);
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      }
      qcril_qmi_voice_stk_cc_relay_alpha_if_necessary( QCRIL_DEFAULT_INSTANCE_ID, FALSE );
    }

    if ( call_obj_review_needed )
    {
      qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl();
    }

    if ( qcril_qmi_voice_info.last_call_failure_cause.pending_req )
    {
      auto& fc = qcril_qmi_voice_info.last_call_failure_cause;
      fc.pending_req = FALSE;
      TimeKeeper::getInstance().clear_timer(fc.pending_request_timeout_timer_id);
      fc.pending_request_timeout_timer_id = TimeKeeper::no_timer;
    }
  }
  else
  {
    QCRIL_LOG_INFO(
        "last_call_failure_cause is not valid. The response will be delayed after we get the valid "
        "value.");
  }
  qcril_qmi_voice_last_call_failure_cause_unlock();

  if (qcril_qmi_voice_voip_call_info_entries_is_empty())
  {
      QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
      if (!qcril_stop_diag_log())
      {
          QCRIL_LOG_INFO("qxdm logging disabled successfully");
      }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_respond_ril_last_call_failure_request

//===========================================================================
// qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl
//===========================================================================
void qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl( void )
{
  qcril_qmi_voice_voip_call_info_entry_type* call_info = NULL;

  int                       cleanup_evaluation_needed;

  QCRIL_LOG_FUNC_ENTRY();

  cleanup_evaluation_needed = FALSE;

  call_info = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while ( NULL != call_info )
  {
    if ( VOICE_INVALID_CALL_ID != call_info->android_call_id &&
         CALL_STATE_END_V02 == call_info->voice_scv_info.call_state &&
         !( call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LAST_CALL_FAILURE_REPORTED )
       )
    {
      call_info->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LAST_CALL_FAILURE_REPORTED;

      cleanup_evaluation_needed = TRUE;
    }
    call_info = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }

  if ( cleanup_evaluation_needed )
  {
    qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl();
  }
  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_review_call_objs_after_last_call_failure_response_vcl

//===========================================================================
// qcril_qmi_voice_handle_new_last_call_failure_cause
//===========================================================================
static void qcril_qmi_voice_handle_new_last_call_failure_cause
(
 call_end_reason_enum_v02 qmi_reason,
 qcril::interfaces::CallFailCause ril_reason,
 std::string reason_str,
 qcril_qmi_voice_voip_call_info_entry_type *call_obj
)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_voice_store_last_call_failure_cause_ex(ril_reason, reason_str);
  qcril_qmi_voice_respond_ril_last_call_failure_request();
  if (call_obj)
  {
     call_obj->lcf_valid = TRUE;
     call_obj->lcf = ril_reason;
     call_obj->lcf_extended_codes = qmi_reason;
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_handle_new_last_call_failure_cause

//===========================================================================
// qcril_qmi_voice_handle_new_last_call_failure_cause
//===========================================================================
static void qcril_qmi_voice_handle_new_last_call_failure_cause
(
 call_end_reason_enum_v02 reason,
 uint8_t raw_code_valid,
 uint8_t raw_code,
 uint8_t sip_code_valid,
 uint16_t sip_code,
 uint8_t end_reason_text_valid,
 char *end_reason_text,
 qcril_qmi_voice_voip_call_info_entry_type *call_obj
)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril::interfaces::CallFailCause ril_reason = qcril::interfaces::CallFailCause::UNKNOWN;
  std::string reason_str;

  ril_reason = qcril_qmi_voice_map_qmi_to_ril_last_call_failure_cause(
          reason,
          raw_code_valid,
          raw_code,
          sip_code_valid,
          sip_code,
          end_reason_text_valid,
          end_reason_text,
          reason_str);

  qcril_qmi_voice_handle_new_last_call_failure_cause(reason, ril_reason, reason_str, call_obj);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_handle_new_last_call_failure_cause

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_send_unsol_call_state_changed

===========================================================================*/
/*!
    @brief
    Send call state changed indication

    @return
    none
*/
/*=========================================================================*/
void qcril_qmi_voice_send_unsol_call_state_changed()
{
  QCRIL_LOG_FUNC_ENTRY();
  std::vector<qcril::interfaces::CallInfo> calls;
  RIL_Errno ret =
      qcril_qmi_voice_request_get_current_calls(qcril::interfaces::CallDomain::AUTOMATIC, calls);
  if (ret == RIL_E_SUCCESS)
  {
    auto msg = std::make_shared<QcRilUnsolCallStateChangeMessage>();
    if (msg != nullptr)
    {
      msg->setCallInfo(calls);
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_send_unsol_call_state_changed


/*=========================================================================
  FUNCTION:  qcril_qmi_voice_make_incoming_call_ring
===========================================================================*/
void qcril_qmi_voice_make_incoming_call_ring
(
  std::shared_ptr<VoiceMakeIncomingCallRingMessage> ring_msg
)
{
  qcril_qmi_voice_voip_call_info_entry_type*    call_info = NULL;
  int                                           need_ring_to_ui;
  int                                           need_reiterate;
  int                                           is_1x;
  int                                           is_ims_call;

  QCRIL_LOG_FUNC_ENTRY();

  need_ring_to_ui = FALSE;
  need_reiterate  = FALSE;
  is_1x           = FALSE;
  qcril_qmi_voice_voip_lock_overview();

  call_info = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING, TRUE );
  QCRIL_LOG_INFO(".. call obj %p", call_info );
  if ( call_info )
  {
    qcril_qmi_voice_voip_call_info_dump( call_info );

    call_info->ringing_time_id = TimeKeeper::no_timer;

    is_1x = (CALL_MODE_CDMA_V02 == call_info->voice_scv_info.mode) ? TRUE : FALSE;

    is_ims_call = qcril_qmi_voice_call_to_ims(call_info);

    if ( call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_RING_DONE || !is_1x )
    {
      call_info->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_RING_DONE;
      need_ring_to_ui = TRUE;
    }

    if ( !is_1x && (call_info->voice_scv_info.call_state == CALL_STATE_SETUP_V02) ) // no follow up will be needed for 1x or non-setup call
    {
      need_reiterate = TRUE;
    }

    if ( !need_reiterate )
    {
      call_info->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING;
    }
    else
    {
      call_info->elaboration |= QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID;
      call_info->ringing_time_id = TimeKeeper::getInstance().set_timer(
            [](void* /*userdata*/) {
                auto msg = std::make_shared<VoiceMakeIncomingCallRingMessage>();
                Dispatcher::getInstance().dispatch(msg);
            },
            nullptr,
            3000 /* 3 seconds */
            );
    }
  }

  qcril_qmi_voice_voip_unlock_overview();

  QCRIL_LOG_INFO(".. need ring ui %d, need reiterate %d", (int) need_ring_to_ui, (int) need_reiterate );
  if ( need_ring_to_ui )
  {
    std::optional<RIL_CDMA_SignalInfoRecord> cdma_info = ring_msg->getCdmaSignalInfoRecord();
    auto msg = std::make_shared<QcRilUnsolCallRingingMessage>();
    if (msg != nullptr) {
      if ( is_1x && cdma_info )
      {
        const auto& info = *cdma_info;
        QCRIL_LOG_INFO("signal_info signal_type: %d, alert_pitch: %d, signal: %d",
                      info.signalType, info.alertPitch, info.signal);

        auto cdmaSigInfoRecord = std::make_shared<RIL_CDMA_SignalInfoRecord>();
        if (cdmaSigInfoRecord)
        {
          cdmaSigInfoRecord->signalType = info.signalType;
          cdmaSigInfoRecord->alertPitch = info.alertPitch;
          cdmaSigInfoRecord->signal = info.signal;
          msg->setCdmaSignalInfoRecord(cdmaSigInfoRecord);
        }
      }
      msg->setIsIms(is_ims_call ? true : false);
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_voice_make_incoming_call_ring */

//===========================================================================
// qmi_ril_succeed_on_pending_hangup_req_on_no_calls_left
//===========================================================================
void qmi_ril_succeed_on_pending_hangup_req_on_no_calls_left(void)
{
  auto pendingMsg =
      findVoiceModulePendingMessage(QcRilRequestHangupMessage::get_class_message_id());
  if (pendingMsg == nullptr)
  {
    pendingMsg = findVoiceModulePendingMessage(
        QcRilRequestHangupWaitingOrBackgroundMessage::get_class_message_id());
  }
  if (pendingMsg == nullptr)
  {
    pendingMsg = findVoiceModulePendingMessage(
        QcRilRequestHangupForegroundResumeBackgroundMessage::get_class_message_id());
  }
  if (pendingMsg) {
    auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_SUCCESS, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
  QCRIL_LOG_FUNC_RETURN();
}  // qmi_ril_succeed_on_pending_hangup_req_on_no_calls_left

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_otasp_status_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_QMI_VOICE_OTASP_STATUS_IND

    @return
    RIL_CDMA_OTA_ProvisionStatus.
*/
/*=========================================================================*/
void qcril_qmi_voice_otasp_status_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t /*ind_data_len*/
)
{
  voice_otasp_status_ind_msg_v02* otasp_status_ind_ptr = NULL;
  RIL_CDMA_OTA_ProvisionStatus response;

  if( ind_data_ptr != NULL )
  {
      otasp_status_ind_ptr = (voice_otasp_status_ind_msg_v02*)ind_data_ptr;

      QCRIL_LOG_DEBUG( "QCRIL_EVT_QMI_VOICE_OTASP_STATUS_IND status = %d for conn id %d",
                           otasp_status_ind_ptr->otasp_status_info.otasp_status,
                           otasp_status_ind_ptr->otasp_status_info.call_id);

      response = (RIL_CDMA_OTA_ProvisionStatus)otasp_status_ind_ptr->otasp_status_info.otasp_status;

      auto msg = std::make_shared<QcRilUnsolCdmaOtaProvisionStatusMessage>();
      if (msg != nullptr)
      {
        msg->setStatus(response);
        Dispatcher::getInstance().dispatchSync(msg);
      }
  }

} /* qcril_qmi_voice_otasp_status_ind_hdlr */


/*=========================================================================
  FUNCTION:  qcril_qmi_voice_privacy_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_VOICE_PRIVACY_IND.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_privacy_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_privacy_ind_msg_v02* privacy_ind_ptr = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;
  QCRIL_NOTUSED(ind_data_len);

  if( ind_data_ptr != NULL )
  {
      privacy_ind_ptr = (voice_privacy_ind_msg_v02*)ind_data_ptr;

      QCRIL_LOG_INFO("Privacy indication received with privacy %d for conn id %d",
                                   privacy_ind_ptr->voice_privacy_info.voice_privacy,
                                   privacy_ind_ptr->voice_privacy_info.call_id);

      qcril_qmi_voice_voip_lock_overview();
      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id( privacy_ind_ptr->voice_privacy_info.call_id );
      if ( call_info_entry )
      {
        call_info_entry->voice_svc_voice_privacy = privacy_ind_ptr->voice_privacy_info.voice_privacy;
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VOICE_PRIVACY_VALID;
        QCRIL_LOG_INFO( ".. caching voice privacy" );
      }
      qcril_qmi_voice_voip_unlock_overview();

      if (call_info_entry)
      {
        qcril_qmi_voice_send_unsol_call_state_changed();
      }
  }

}/* qcril_qmi_voice_privacy_ind_hdlr */

//===========================================================================
// qcril_qmi_voice_uus_ind_hdlr
//===========================================================================
void qcril_qmi_voice_uus_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_uus_ind_msg_v02* uus_ind_ptr = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;
  QCRIL_NOTUSED(ind_data_len);

  if( ind_data_ptr != NULL )
  {
      uus_ind_ptr = (voice_uus_ind_msg_v02*)ind_data_ptr;

      QCRIL_LOG_INFO("UUS indication received for call id %d with type %d, dcs %d, data len %d ",
                                   (int)uus_ind_ptr->uus_information.call_id,
                                   (int)uus_ind_ptr->uus_information.uus_type,
                                   (int)uus_ind_ptr->uus_information.uus_dcs,
                                   (int)uus_ind_ptr->uus_information.uus_data_len
                     );

      qcril_qmi_voice_voip_lock_overview();
      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id( uus_ind_ptr->uus_information.call_id );
      if ( call_info_entry )
      {
        qcril_qmi_voice_voip_update_call_info_uus(
                                                  call_info_entry,
                                                  uus_ind_ptr->uus_information.uus_type,
                                                  uus_ind_ptr->uus_information.uus_dcs,
                                                  uus_ind_ptr->uus_information.uus_data_len,
                                                  uus_ind_ptr->uus_information.uus_data
                                                 );
      }
      qcril_qmi_voice_voip_unlock_overview();
  }

} // qcril_qmi_voice_uus_ind_hdlr

//===========================================================================
// qcril_qmi_voice_respond_pending_hangup_ril_response
//===========================================================================
void qcril_qmi_voice_respond_pending_hangup_ril_response(uint8_t call_id)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("call id = %d", call_id);

  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;

  memset(&oversight_event_params, 0, sizeof(oversight_event_params));
  oversight_event_params.new_call_state = CALL_STATE_END_V02;
  oversight_event_params.locator.qmi_call_id = call_id;
  qmi_ril_voice_ims_command_oversight_handle_event(
      QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_IND,
      QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID, &oversight_event_params);

  memset(&oversight_event_params, 0, sizeof(oversight_event_params));
  oversight_event_params.locator.qmi_call_id = call_id;
  qmi_ril_voice_ims_command_oversight_handle_event(
      QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_CALL_ENDED,
      QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID, &oversight_event_params);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_respond_pending_hangup_ril_response

//===========================================================================
// qcril_qmi_voice_setup_num_1x_wait_timer
//===========================================================================
static void qcril_qmi_voice_setup_num_1x_wait_timer()
{
  QCRIL_LOG_FUNC_ENTRY();
  if (TimeKeeper::no_timer != qmi_voice_voip_overview.num_1x_wait_timer_id)
  {
    TimeKeeper::getInstance().clear_timer(qmi_voice_voip_overview.num_1x_wait_timer_id);
    qmi_voice_voip_overview.num_1x_wait_timer_id = TimeKeeper::no_timer;
  }

  auto num_1x_wait_timer_id = TimeKeeper::getInstance().set_timer(
              qmi_ril_voice_pending_1x_num_timeout,
              nullptr,
              1000); /* 1 second */
  qmi_voice_voip_overview.num_1x_wait_timer_id = num_1x_wait_timer_id;

  QCRIL_LOG_INFO("num_1x_wait_timer_wakelock_is_acquired = %d",
          qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired);
  // Acquire wake lock
  if (qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired == FALSE)
  {
    voiceNum1xWaitWakeLock.acquire();
    qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired = TRUE;
    qmi_ril_set_property_value_to_string(QCRIL_NUM_1X_WAIT_WAKE_LOCK_PROP, "true");
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_setup_num_1x_wait_timer

//===========================================================================
// qcril_qmi_voice_cancel_num_1x_wait_timer
//===========================================================================
static void qcril_qmi_voice_cancel_num_1x_wait_timer()
{
  QCRIL_LOG_FUNC_ENTRY();
  if (TimeKeeper::no_timer != qmi_voice_voip_overview.num_1x_wait_timer_id)
  {
    TimeKeeper::getInstance ().clear_timer(qmi_voice_voip_overview.num_1x_wait_timer_id);
    qmi_voice_voip_overview.num_1x_wait_timer_id = TimeKeeper::no_timer;
  }

  // Release wake lock
  QCRIL_LOG_INFO("num_1x_wait_timer_wakelock_is_acquired = %d",
          qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired);
  if (qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired == TRUE)
  {
    voiceNum1xWaitWakeLock.release();
    qmi_voice_voip_overview.num_1x_wait_timer_wakelock_is_acquired = FALSE;
    qmi_ril_set_property_value_to_string(QCRIL_NUM_1X_WAIT_WAKE_LOCK_PROP, "false");
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_cancel_num_1x_wait_timer

//===========================================================================
// qmi_ril_voice_pending_1x_num_timeout
//===========================================================================
void qmi_ril_voice_pending_1x_num_timeout(void * param)
{
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    qcril_qmi_voice_voip_lock_overview();

    qcril_qmi_voice_cancel_num_1x_wait_timer();

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration ( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING, TRUE );
    if ( call_info_entry )
    {
      call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;

      qcril_qmi_voice_send_unsol_call_state_changed();
    }

    qcril_qmi_voice_voip_unlock_overview();

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_pending_1x_num_timeout

//===========================================================================
// qcril_qmi_voice_set_ps_cs_call_elab_vcl
//===========================================================================
void qcril_qmi_voice_set_ps_cs_call_elab_vcl(const voice_call_info2_type_v02* iter_call_info, qcril_qmi_voice_voip_call_info_entry_type *call_info_entry)
{
   call_mode_enum_v02 call_mode;
   call_type_enum_v02 call_type;

   if (NULL == call_info_entry)
   {
      QCRIL_LOG_ERROR("call_info_entry is NULL");
   }
   else
   {
      if (NULL != iter_call_info)
      {
         call_type = iter_call_info->call_type;
         call_mode = iter_call_info->mode;
      }
      else
      {
         call_type = call_info_entry->voice_scv_info.call_type;
         call_mode = call_info_entry->voice_scv_info.mode;
      }

      if ((CALL_TYPE_VOICE_IP_V02 == call_type ||
           CALL_TYPE_VT_V02 == call_type ||
           CALL_TYPE_EMERGENCY_IP_V02 == call_type ||
           CALL_TYPE_EMERGENCY_VT_V02 == call_type) &&
          ims_client_connected)
      {
         // Set the call as PS_DOMAIN only if the call is already not resolved as CS_DOMAIN.
         // This is to avoid call transition from CS to PS
         if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN) &&
              CALL_MODE_NO_SRV_V02 != call_mode)
         {
            // if the call type is a PS call, and we get a determined call_mode, set it as a PS call
            QCRIL_LOG_INFO("set the call as a PS call");
            call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
            call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
         }
      }
      else if (CALL_MODE_NO_SRV_V02 != call_mode &&
               CALL_MODE_UNKNOWN_V02 != call_mode &&
               CALL_MODE_LTE_V02 != call_mode &&
               CALL_MODE_NR5G_V02 != call_mode)
      {
         // if the call type is a CS call and the call_mode is determined(not no_srv), set it as a CS call
         QCRIL_LOG_INFO("set the call as a CS call");
         call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
         call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
         call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
      }
   }
} // qcril_qmi_voice_set_ps_cs_call_elab_vcl

void qcril_qmi_voice_set_domain_elab_from_call_type
(
  call_type_enum_v02 call_type,
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry
)
{
    if ((CALL_TYPE_VOICE_IP_V02 == call_type ||
         CALL_TYPE_VT_V02 == call_type ||
         CALL_TYPE_EMERGENCY_IP_V02 == call_type ||
         CALL_TYPE_EMERGENCY_VT_V02 == call_type) &&
        ims_client_connected)
    {
        // Set the call as PS_DOMAIN only if the call is already not resolved as CS_DOMAIN.
        // This is to avoid call transition from CS to PS
        if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN)) {
           QCRIL_LOG_INFO("set the call as a PS call");
           call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
           call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
           call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
        }
    }
    else
    {
        QCRIL_LOG_INFO("set the call as a CS call");
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
    }
}

//===========================================================================
// qcril_qmi_voice_auto_answer_if_needed
//===========================================================================
void qcril_qmi_voice_auto_answer_if_needed()
{
  long int                                    answer_ms = 0;
  int int_config;
  if (qcril_config_get(PERSIST_VENDOR_RADIO_AUTOANSWER_MS, int_config) == E_SUCCESS)
  {
     answer_ms = int_config;
  }
  QCRIL_LOG_INFO("QCRIL_QMI_AUTO_ANSWER %d", answer_ms);
  if( answer_ms > 0 )
  {
    if (TimeKeeper::no_timer != qmi_voice_voip_overview.auto_answer_timer_id )
    {
      QCRIL_LOG_INFO("Cancel Auto Answer timed callback");
      TimeKeeper::getInstance().clear_timer(qmi_voice_voip_overview.auto_answer_timer_id);
      qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;
    }
    QCRIL_LOG_INFO("Auto answer timeout %d ms", answer_ms);
    qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::getInstance().set_timer(
            qcril_qmi_voice_auto_answer_timeout_handler,
            nullptr,
            answer_ms);
  }
  else
  {
    QCRIL_LOG_INFO("Auto answer disabled!");
  }
} // qcril_qmi_voice_auto_answer_if_needed

//===========================================================================
// qcril_qmi_voice_is_qmi_call_emergency
//===========================================================================
boolean qcril_qmi_voice_is_qmi_call_emergency(const voice_call_info2_type_v02* iter_call_info)
{
   return (CALL_TYPE_EMERGENCY_V02 == iter_call_info->call_type ||
           CALL_TYPE_EMERGENCY_IP_V02 == iter_call_info->call_type ||
           CALL_TYPE_EMERGENCY_VT_V02 == iter_call_info->call_type);
} // qcril_qmi_voice_is_qmi_call_emergency

//===========================================================================
// qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received
//===========================================================================
void qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received(qcril_qmi_voice_voip_call_info_entry_type *call_info_entry)
{
   if (call_info_entry)
   {
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED)
      {
         call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED;
         call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;
      }
   }
   else
   {
      QCRIL_LOG_ERROR("call_info_entry is NULL");
   }
} // qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received

//===========================================================================
// qcril_start_diag_log
//===========================================================================
int qcril_start_diag_log(void)
{
    int   rc = 0;
    char  diag_cmd[QCRIL_DIAG_CMD_LENGTH] = {0};

    bool bool_config, diag_logging = false;
    if (qcril_config_get(PERSIST_VENDOR_RADIO_VOICE_DIAG_LOGGING, bool_config) == E_SUCCESS)
    {
        diag_logging = bool_config;
    }
    if (!diag_logging)
    {
        return rc;
    }
    else
    {
        qcril_qmi_voice_diag_odl_lock();

        if (diag_odl_logging_in_progress == FALSE)
        {
            diag_odl_logging_in_progress = TRUE;
            qcril_qmi_voice_diag_odl_unlock();
            snprintf(diag_cmd, QCRIL_DIAG_CMD_LENGTH,
                    "%s -p -f %s -m %s -o %s &",
                    QCRIL_DIAG_PATH, QCRIL_DIAG_APQ_MASK_NAME,
                    QCRIL_DIAG_MDM_MASK_NAME,
                    QCRIL_DIAG_LOG_LOCATION);
            rc = system(diag_cmd);
            QCRIL_LOG_INFO("diag logging intialization status %d\n", rc);
            if (rc != 0)
            {
                qcril_qmi_voice_diag_odl_lock();
                diag_odl_logging_in_progress = FALSE;
                qcril_qmi_voice_diag_odl_unlock();
            }
        }
        else
        {
            qcril_qmi_voice_diag_odl_unlock();
            QCRIL_LOG_INFO("diag logging already in progress\n");
        }

        return rc;
    }
}

//===========================================================================
// qcril_stop_diag_log
//===========================================================================
int qcril_stop_diag_log(void)
{
    int rc = 0;
    char diag_cmd[QCRIL_DIAG_CMD_LENGTH] = {0};
    int tmp_qcril_qmi_voice_diag_odl_lock;

    qcril_qmi_voice_diag_odl_lock();
    tmp_qcril_qmi_voice_diag_odl_lock = diag_odl_logging_in_progress;
    qcril_qmi_voice_diag_odl_unlock();

    do {

        if (tmp_qcril_qmi_voice_diag_odl_lock == FALSE)
        {
            break;
        }

        snprintf(diag_cmd, QCRIL_DIAG_CMD_LENGTH,
                "%s -k", QCRIL_DIAG_PATH);
        rc = system(diag_cmd);
        if (rc == 0)
        {
            qcril_qmi_voice_diag_odl_lock();
            diag_odl_logging_in_progress = FALSE;
            qcril_qmi_voice_diag_odl_unlock();
            QCRIL_LOG_INFO("diag logging disabled");
        }

    } while (0);

    return rc;
}

//===========================================================================
// get_network_detected_ecc_number_info
//===========================================================================
static std::shared_ptr<PbmNetworkDetectedEccNumber> get_network_detected_ecc_number_info()
{
  std::shared_ptr<PbmNetworkDetectedEccNumber> respPtr = nullptr;
  auto getNwDetectedEcc = std::make_shared<GetNetworkDetectedEccNumberInfoMessage>();
  if (getNwDetectedEcc)
  {
    if (getNwDetectedEcc->dispatchSync(respPtr) != Message::Callback::Status::SUCCESS)
    {
      respPtr = nullptr;
    }
  }
  return respPtr;
} // get_network_detected_ecc_number_info

//===========================================================================
// reset_network_detected_ecc_if_required
//===========================================================================
static void reset_network_detected_ecc_if_required(bool force_reset = false)
{
  if (!force_reset)
  {
    // Reset the network detected ecc number details if required.
    auto nwDetectedEcc = get_network_detected_ecc_number_info();
    if (nwDetectedEcc)
    {
      force_reset = true;
    }
  }
  if (force_reset)
  {
      auto nwDetectedEccMsg = std::make_shared<NetworkDetectedEccNumberInfoMessage>(
          NetworkDetectedEccNumberInfoMessage::Action::RESET);
      Dispatcher::getInstance().dispatchSync(nwDetectedEccMsg);
  }
} // reset_network_detected_ecc_if_required

//===========================================================================
// is_end_reason_to_reset_network_detected_ecc_info
//===========================================================================
bool is_end_reason_to_reset_network_detected_ecc_info(call_end_reason_enum_v02 reason)
{
  // Reset the NetworkDetectedEccNumberInfo if the call is not ended with
  // TEMP_REDIAL_ALLOWED / PERM_REDIAL_NOT_NEEDED / ALTERNATE_EMERGENCY_CALL
  if ((reason != CALL_END_CAUSE_TEMP_REDIAL_ALLOWED_V02) &&
      (reason != CALL_END_CAUSE_PERM_REDIAL_NOT_NEEDED_V02) &&
      (reason != CALL_END_CAUSE_ALTERNATE_EMERGENCY_CALL_V02))
  {
    return true;
  }
  return false;
}



//===========================================================================
// qcril_qmi_voice_create_call_info_from_srvcc_parent_call_info
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_create_call_info_from_srvcc_parent_call_info
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02 *call_status_ind_ptr
)
{
  voice_srvcc_parent_call_id_type_v02 *srvcc_parent_call_info = NULL;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  qcril_qmi_voice_voip_call_info_entry_type *parent_call_info_entry = NULL;
  unsigned int j;

  QCRIL_LOG_FUNC_ENTRY();
  if (call_status_ind_ptr)
  {
    // Mid Call SRVCC
    srvcc_parent_call_info = NULL;
    if (call_status_ind_ptr->srvcc_parent_call_info_valid)
    {
      for (j = 0; (j < call_status_ind_ptr->srvcc_parent_call_info_len &&
            j < QMI_VOICE_SRVCC_PARENT_CALL_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind_ptr->srvcc_parent_call_info[j].call_id == call_id)
        {
          srvcc_parent_call_info = &call_status_ind_ptr->srvcc_parent_call_info[j];
          break;
        }
      }
    }
  }

  if ((NULL != srvcc_parent_call_info))
  {
    // This is the case of conference call SRVCC
    // Create call_info_entry with new call_id.
    call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
        srvcc_parent_call_info->call_id,
        INVALID_MEDIA_ID,
        TRUE,
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID,
        NULL);

    if ((NULL != call_info_entry) && (srvcc_parent_call_info->is_parent_id_cleared == TRUE))
    {
      //Delete the call_info_entry with parent call_id
      parent_call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(
          srvcc_parent_call_info->parent_call_id);
      if (parent_call_info_entry)
      {
        parent_call_info_entry->elaboration |=
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_END_AFTER_SRVCC;
      }
      else
      {
        QCRIL_LOG_INFO( "Call info with parent_call_id doesn't exist... " );
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_create_call_info_from_is_srvcc
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_create_call_info_from_is_srvcc
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02 *call_status_ind_ptr
)
{
  voice_is_srvcc_call_with_id_type_v02 *is_srvcc = NULL;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  unsigned int j;

  QCRIL_LOG_FUNC_ENTRY();

  if (call_status_ind_ptr)
  {
    // SRVCC Call
    is_srvcc = NULL;
    if (call_status_ind_ptr->is_srvcc_valid)
    {
      for (j = 0; (j < call_status_ind_ptr->is_srvcc_len &&
            j < QMI_VOICE_IS_SRVCC_CALL_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind_ptr->is_srvcc[j].call_id == call_id)
        {
          is_srvcc = &call_status_ind_ptr->is_srvcc[j];
          break;
        }
      }
    }
  }

  if (is_srvcc && is_srvcc->is_srvcc_call)
  {
    // fresh creation of call_info_entry in case of SRVCC call from 3rd party
    call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
        is_srvcc->call_id,
        INVALID_MEDIA_ID,
        TRUE,
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID,
        NULL);
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_get_call_info
//===========================================================================
voice_call_info2_type_v02 *
qcril_qmi_voice_all_call_status_ind_get_call_info
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02 *call_status_ind_ptr
 )
{
  voice_call_info2_type_v02 *call_info = NULL;
  unsigned int j;

  if (call_status_ind_ptr)
  {
    for (j = 0; (j < call_status_ind_ptr->call_info_len &&
          j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
    {
      if (call_status_ind_ptr->call_info[j].call_id == call_id)
      {
        call_info = &call_status_ind_ptr->call_info[j];
        break;
      }
    }
  }
  return call_info;
}

//===========================================================================
// qcril_qmi_voice_notify_unsol_ringback_tone
//===========================================================================
void qcril_qmi_voice_notify_unsol_ringback_tone
(
 uint8_t call_id,
 alerting_type_enum_v02 alerting_type,
 boolean is_ims
)
{
  int local_ringback_payload = FALSE;
  boolean notify_ind = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  if (alerting_type == ALERTING_REMOTE_V02 &&
      !qcril_qmi_voice_info.last_call_is_local_ringback)
  {
    QCRIL_LOG_INFO("Ringback Tone started with QMI id %d", call_id);
    local_ringback_payload = TRUE;
    qcril_qmi_voice_info.last_call_is_local_ringback = TRUE;
    qcril_qmi_voice_info.last_local_ringback_call_id = call_id;
    notify_ind = TRUE;
  }
  else if (qcril_qmi_voice_info.last_call_is_local_ringback &&
      (qcril_qmi_voice_info.last_local_ringback_call_id == call_id) &&
      (alerting_type == ALERTING_LOCAL_V02))
  {
    QCRIL_LOG_INFO("Ringback Tone stopped with QMI id %d", call_id);
    local_ringback_payload = FALSE;
    qcril_qmi_voice_info.last_call_is_local_ringback = FALSE;
    qcril_qmi_voice_info.last_local_ringback_call_id = VOICE_INVALID_CALL_ID;
    notify_ind = TRUE;
  }

  if (notify_ind)
  {
    auto msg = std::make_shared<QcRilUnsolRingbackToneMessage>();
    if (msg != nullptr)
    {
      msg->setIsIms(is_ims);
      msg->setRingBackToneOperation(local_ringback_payload
                                        ? qcril::interfaces::RingBackToneOperation::START
                                        : qcril::interfaces::RingBackToneOperation::STOP);
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_pre_alerting_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_all_call_status_ind_pre_alerting_hdlr(
    uint8_t call_id, voice_all_call_status_ind_msg_v02* call_status_ind_ptr) {
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = nullptr;
  voice_call_info2_type_v02* call_info = nullptr;

  QCRIL_LOG_FUNC_ENTRY();
  if (!call_status_ind_ptr) {
    return nullptr;
  }

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_info) {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (!call_info_entry) {
      call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
          call_id, INVALID_MEDIA_ID, TRUE, QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE, nullptr);
    }
  }

  if (!call_info_entry) {
    return nullptr;
  }

  qcril::interfaces::PreAlertingCallInfo preAlertingInfo{};
  preAlertingInfo.setCallId(call_info_entry->android_call_id);

  bool has_composerInfo = false;
  qcril::interfaces::CallComposerInfo info{};
  info.setId(call_info_entry->android_call_id);
  if (call_status_ind_ptr->call_composer_importance_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_importance_len; i++) {
      const auto& ci = call_status_ind_ptr->call_composer_importance[i];
      if (ci.call_id == call_id) {
        auto value = ci.importance;
        if (value == CALL_COMP_IMP_NORMAL_V02) {
          info.setPriority(qcril::interfaces::Priority::NORMAL);
        } else if (value == CALL_COMP_IMP_URGENT_V02) {
          info.setPriority(qcril::interfaces::Priority::URGENT);
        }
        has_composerInfo = true;
        break;
      }
    }
  }
  if (call_status_ind_ptr->call_composer_subject_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_subject_len; i++) {
      const auto& cs = call_status_ind_ptr->call_composer_subject[i];
      if (cs.call_id == call_id) {
        if (cs.subject_len <= QMI_VOICE_CALL_COMP_SUBJECT_MAX_LEN_V02) {
          std::vector<uint16_t> subject(cs.subject, cs.subject + cs.subject_len);
          info.setSubject(std::move(subject));
        }
        has_composerInfo = true;
        break;
      }
    }
  }
  if (call_status_ind_ptr->call_composer_organization_header_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_organization_header_len; i++) {
      const auto& co = call_status_ind_ptr->call_composer_organization_header[i];
      if (co.call_id == call_id) {
        if (co.org_header_len <= QMI_VOICE_CALL_COMP_ORG_HDR_MAX_LEN_V02) {
          std::vector<uint16_t> organization(co.org_header, co.org_header + co.org_header_len);
          info.setOrganization(std::move(organization));
        }
        has_composerInfo = true;
        break;
      }
    }
  }
  if (call_status_ind_ptr->call_composer_picture_url_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_picture_url_len; i++) {
      const auto& cp = call_status_ind_ptr->call_composer_picture_url[i];
      if (cp.call_id == call_id) {
        info.setImageUrl(cp.picture_url);
        has_composerInfo = true;
        break;
      }
    }
  }

  qcril::interfaces::Location location;
  location.setRadius(qcril::interfaces::Location::LOCATION_NOT_SET);
  if (call_status_ind_ptr->call_composer_circle_location_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_circle_location_len; i++) {
      const auto& ccl = call_status_ind_ptr->call_composer_circle_location[i];
      if (ccl.call_id == call_id) {
        location.setRadius(ccl.call_composer_circle_location.radius);
        location.setLatitude(ccl.call_composer_circle_location.coordinates.latitude);
        location.setLongitude(ccl.call_composer_circle_location.coordinates.longitude);
        has_composerInfo = true;
        break;
      }
    }
  } else if (call_status_ind_ptr->call_composer_point_location_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_composer_point_location_len; i++) {
      const auto& cpl = call_status_ind_ptr->call_composer_point_location[i];
      if (cpl.call_id == call_id) {
        location.setRadius(qcril::interfaces::Location::POINT_LOCATION);
        location.setLatitude(cpl.point_location.coordinates.latitude);
        location.setLongitude(cpl.point_location.coordinates.longitude);
        has_composerInfo = true;
        break;
      }
    }
  }
  info.setLocation(location);
  if (has_composerInfo) {
    preAlertingInfo.setCallComposerInfo(info);
  }

  bool has_ecnamInfo = false;
  qcril::interfaces::EcnamInfo ecnamInfo{};
  if (call_status_ind_ptr->ip_caller_name_valid) {
    for (int i = 0; i < call_status_ind_ptr->ip_caller_name_len; i++) {
      const auto& name = call_status_ind_ptr->ip_caller_name[i];
      if (name.call_id == call_id) {
        uint32_t caller_name_len = 0;
        char ip_caller_name[(QMI_VOICE_IP_CALLER_NAME_MAX_LEN_V02 * 2)] = "\0";
        memset( &ip_caller_name, 0, sizeof(ip_caller_name) );
        caller_name_len = qcril_cm_ss_convert_ucs2_to_utf8((const char*)name.ip_caller_name,
                                                           name.ip_caller_name_len * 2,
                                                           ip_caller_name, sizeof(ip_caller_name));
        if (caller_name_len > 0) {
          ecnamInfo.setName(ip_caller_name);
          has_ecnamInfo = true;
          break;
        }
      }
    }
  }
  if (call_status_ind_ptr->call_ecnam_icon_url_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_ecnam_icon_url_len; i++) {
      const auto& ecnamIcon = call_status_ind_ptr->call_ecnam_icon_url[i];
      if (ecnamIcon.call_id == call_id) {
        ecnamInfo.setIconUrl(ecnamIcon.ecnam_icon_url);
        has_ecnamInfo = true;
        break;
      }
    }
  }
  if (call_status_ind_ptr->call_ecnam_info_url_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_ecnam_info_url_len; i++) {
      const auto& ecnamInfoUrl = call_status_ind_ptr->call_ecnam_info_url[i];
      if (ecnamInfoUrl.call_id == call_id) {
        ecnamInfo.setInfoUrl(ecnamInfoUrl.ecnam_info_url);
        has_ecnamInfo = true;
        break;
      }
    }
  }
  if (call_status_ind_ptr->call_ecnam_card_url_valid) {
    for (int i = 0; i < call_status_ind_ptr->call_ecnam_card_url_len; i++) {
      const auto& ecnamCard = call_status_ind_ptr->call_ecnam_card_url[i];
      if (ecnamCard.call_id == call_id) {
        ecnamInfo.setCardUrl(ecnamCard.ecnam_card_url);
        has_ecnamInfo = true;
        break;
      }
    }
  }

  if (has_ecnamInfo) {
    preAlertingInfo.setEcnamInfo(ecnamInfo);
  }

  qcril::interfaces::DataChannelInfo dcCallInfo{};
  if (call_status_ind_ptr->dc_call_info_valid) {
    for (int i = 0; i < call_status_ind_ptr->dc_call_info_len; i++) {
      if (call_status_ind_ptr->dc_call_info[i].call_id == call_id) {
        dcCallInfo.setModemCallId(call_status_ind_ptr->dc_call_info[i].call_id);
        dcCallInfo.setIsDcCall(call_status_ind_ptr->dc_call_info[i].is_dc_call);
        preAlertingInfo.setDataChannelInfo(dcCallInfo);
        break;
      }
    }
  }

  auto msg = std::make_shared<QcRilUnsolImsPreAlertingCallInfo>(preAlertingInfo);
  Dispatcher::getInstance().dispatchSync(msg);
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_incoming_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_incoming_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr,
 boolean is_voice_rte_lte
)
{
  int rc = 0;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  // Reset the network detected ecc number details if required.
  reset_network_detected_ecc_if_required();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);

  if (call_status_ind_ptr && call_info)
  {
    rc = qcril_start_diag_log();
    QCRIL_LOG_INFO("MT call: start diag logging : %s\n", (rc == 0 ? "SUCCESS" : "FAILURE"));

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (!call_info_entry)
    {
      // fresh
      call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
          call_id,
          INVALID_MEDIA_ID,
          TRUE,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING,
          NULL );
    }
    else
    {
      // what we got?
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING;
      qcril_qmi_voice_voip_call_info_dump(call_info_entry);
    }

    if (call_info_entry)
    {
      if (CALL_STATE_SETUP_V02 == call_info_entry->voice_scv_info.call_state &&
          call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID)
      {
        TimeKeeper::getInstance().clear_timer(call_info_entry->ringing_time_id);
        call_info_entry->ringing_time_id = TimeKeeper::no_timer;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING;
      }

      if (CALL_TYPE_VOICE_IP_V02 == call_info->call_type ||
          CALL_TYPE_VT_V02 == call_info->call_type)
      {
        auto msg = std::make_shared<QcRilUnsolCallRingingMessage>();
        if (msg != nullptr) {
          msg->setIsIms(true);
          Dispatcher::getInstance().dispatchSync(msg);
        }
      }

      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_INCOMING);
      qcril_qmi_voice_consider_shadow_remote_number_cpy_creation(call_info_entry);
      qcril_qmi_voice_set_domain_elab_from_call_type(call_info->call_type, call_info_entry);
      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);
      qcril_qmi_voice_set_call_reason(call_id, call_status_ind_ptr, call_info_entry);

      if (is_voice_rte_lte &&
          (CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type))
      {
        // 1x CSFB MT call
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1x_CSFB_CALL;
      }
      else if (CALL_MODE_NO_SRV_V02 == call_info_entry->voice_scv_info.mode &&
          CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type)
      {
        // 1x or GW CSFB MT call
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1x_CSFB_CALL;
      }

      switch (call_info->call_type)
      {
        case CALL_TYPE_VOICE_V02:
        case CALL_TYPE_VOICE_FORCED_V02:
          if (CALL_MODE_CDMA_V02 == call_info->mode &&
              (!(call_info_entry->elaboration &
                 QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID)))
          {
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;
            qcril_qmi_voice_setup_num_1x_wait_timer();
          }
          break;

        default:
          // nothing
          break;
      }
    }
    // Answer call if auto answer is enabled
    qcril_qmi_voice_auto_answer_if_needed();
  }
  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_alerting_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_alerting_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);

  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (call_info_entry)
    {
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_ALERTING);

      if (CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_IP_V02 == call_info_entry->voice_scv_info.call_type)
      {
        if (call_info_entry->voice_scv_info.mode == CALL_MODE_LTE_V02 ||
            call_info_entry->voice_scv_info.mode == CALL_MODE_WLAN_V02 ||
            call_info_entry->voice_scv_info.mode == CALL_MODE_C_IWLAN_V02 ||
            call_info_entry->voice_scv_info.mode == CALL_MODE_NR5G_V02)
        {
          qcril_am_set_emergency_rat(call_info_entry->voice_scv_info.mode);
        }
      }

      if (call_info_entry &&
          ((call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID) &&
           call_info_entry->is_srvcc.is_srvcc_call))
      {
        /* Call domain changes are not expected in normal scenario and even
           if it changes we have to handle them only in conversation state.
           This is due to the limitation in telephony that CS->PS transition
           is not handled. So we need evaluate CS/PS call elaboration only in
           case of SRVCC where call domain is changed intentionally to CS*/
        qcril_qmi_voice_set_ps_cs_call_elab_vcl(call_info, call_info_entry);
      }
      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);
    }

    /* Ringback tone handling */
    if (call_info_entry &&
        (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALERTING_TYPE_VALID))
    {
      qcril_qmi_voice_notify_unsol_ringback_tone(call_id,
          call_info_entry->voice_svc_alerting_type.alerting_type,
          qcril_qmi_voice_call_to_ims(call_info_entry));
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_conversation_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_conversation_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  // Reset the network detected ecc number details if required
  reset_network_detected_ecc_if_required();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (call_info_entry == NULL)
    {
      call_info_entry = qcril_qmi_voice_create_call_info_from_srvcc_parent_call_info(
          call_id, call_status_ind_ptr);
    }
    if (call_info_entry == NULL)
    {
      call_info_entry = qcril_qmi_voice_create_call_info_from_is_srvcc(
          call_id, call_status_ind_ptr);
    }

    if (call_info_entry)
    {
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING)
      {
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;
        qcril_qmi_voice_cancel_num_1x_wait_timer();
      }

      switch (call_info_entry->voice_scv_info.call_state) // check previous state
      {
        case CALL_STATE_ORIGINATING_V02:
        case CALL_STATE_CONVERSATION_V02:
        case CALL_STATE_CC_IN_PROGRESS_V02:
        case CALL_STATE_ALERTING_V02:
          // no action
          break;

        default:
          qmi_ril_voice_drop_homeless_incall_reqs();
          break;
      }

      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_ACTIVE);
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_GOT_CONNECTED;
      qcril_qmi_voice_set_ps_cs_call_elab_vcl(call_info, call_info_entry);

      if (CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_IP_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_VT_V02 == call_info_entry->voice_scv_info.call_type)
      {
        qcril_am_set_emergency_rat(call_info_entry->voice_scv_info.mode);
      }

      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);

      // Reset the PENDING_INCOMING and ANSWERING_CALL elabs
      call_info_entry->elaboration &= ~(QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING |
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL);
    }

    /* Ringback tone handling */
    if (call_info_entry &&
        (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALERTING_TYPE_VALID))
    {
      qcril_qmi_voice_notify_unsol_ringback_tone(call_id,
          call_info_entry->voice_svc_alerting_type.alerting_type,
          qcril_qmi_voice_call_to_ims(call_info_entry));
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_end_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_end_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;
  char end_reason_text[QCRIL_QMI_VOICE_FAIL_CAUSE_STR_LEN] = {0};

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);

    // Cancel auto answer timed callback if previous state is INCOMING
    if (call_info_entry)
    {
      if ((CALL_STATE_INCOMING_V02 == call_info_entry->voice_scv_info.call_state) &&
          (CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type))
      {
        if (TimeKeeper::no_timer != qmi_voice_voip_overview.auto_answer_timer_id)
        {
          QCRIL_LOG_INFO("Cancel Auto Answer timed callback");
          TimeKeeper::getInstance ().clear_timer(
              qmi_voice_voip_overview.auto_answer_timer_id);
          qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;
        }
      }
    }

    if (call_info_entry)
    {
      qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received(call_info_entry);
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
                                                             TRUE, RIL_CALL_END);

      QCRIL_LOG_INFO("call mode %d, call type %d, call got connected %d",
          (int)call_info->mode,
          (int)call_info->call_type,
          (int)(QMI_RIL_ZERO != (call_info_entry->elaboration &
                                 QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_GOT_CONNECTED)));

      if (CALL_TYPE_EMERGENCY_V02 == call_info->call_type ||
          CALL_TYPE_EMERGENCY_IP_V02 == call_info->call_type ||
          CALL_TYPE_EMERGENCY_VT_V02 == call_info->call_type)
      {
        EmerCallEndType call_end_info;
        call_end_info.is_eme_ip_call =
          (CALL_TYPE_EMERGENCY_IP_V02 == call_info->call_type ||
           CALL_TYPE_EMERGENCY_VT_V02 == call_info->call_type);
        call_end_info.is_eme_call_connected =
          !!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_GOT_CONNECTED);

        auto msg = std::make_shared<QcRilVoiceNotifyEmergencyCallEndMessage>(call_end_info);
        Dispatcher::getInstance().dispatch(msg);

        qcril_am_reset_emergency_rat();
      }

      if (!qcril_qmi_voice_call_to_ims(call_info_entry))
      {
        call_info_entry->call_obj_phase_out_timer_id =
            TimeKeeper::getInstance().set_timer(
                [call_id = call_info_entry->android_call_id] (void* /*userdata*/) {
                    qmi_ril_voice_ended_call_obj_phase_out(call_id);
                },
                nullptr,
                3000); // 3 seconds
      }
    }

    // Ringback tone handling
    if (call_info_entry &&
        (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALERTING_TYPE_VALID))
    {
      qcril_qmi_voice_notify_unsol_ringback_tone(call_id,
          call_info_entry->voice_svc_alerting_type.alerting_type,
          qcril_qmi_voice_call_to_ims(call_info_entry));
    }

    // Reset the NetworkDetectedEccNumberInfo if the call is not ended with
    // TEMP_REDIAL_ALLOWED / PERM_REDIAL_NOT_NEEDED / ALTERNATE_EMERGENCY_CALL
    if (!call_status_ind_ptr->call_end_reason_valid ||
        is_end_reason_to_reset_network_detected_ecc_info(
          call_status_ind_ptr->call_end_reason[0].call_end_reason))
    {
      reset_network_detected_ecc_if_required();
    }

    // Last call failure cause
    if (call_status_ind_ptr->call_end_reason_valid)
    {
      QCRIL_LOG_INFO("call failure cause %d",
          call_status_ind_ptr->call_end_reason[0].call_end_reason);

      if (call_info_entry &&
          (call_status_ind_ptr->call_end_reason[0].call_end_reason ==
           CALL_END_CAUSE_ALTERNATE_EMERGENCY_CALL_V02))
      {
        if (call_status_ind_ptr->emerg_srv_categ_valid)
        {
          unsigned n_calls = call_status_ind_ptr->emerg_srv_categ_len;
          for (unsigned j = 0; (j < n_calls && j < QMI_VOICE_EMERG_SRV_CATEG_ARRAY_MAX_V02); j++)
          {
            if (call_status_ind_ptr->emerg_srv_categ[j].call_id == call_id)
            {
              call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERG_SRV_CATEG_VALID;
              call_info_entry->emerg_srv_categ =
                    call_status_ind_ptr->emerg_srv_categ[j].emerg_srv_categ;
              break;
            }
          }
        }
      }
      if (call_status_ind_ptr->raw_call_end_cause_code_valid)
      {
        QCRIL_LOG_INFO("raw call end cause code: %d",
            call_status_ind_ptr->raw_call_end_cause_code[0].raw_call_end_cause_code);
      }
      if (call_status_ind_ptr->sip_error_code_valid)
      {
        QCRIL_LOG_INFO("sip error code: %d",
            call_status_ind_ptr->sip_error_code[0].sip_error_code);
      }
      if (call_status_ind_ptr->end_reason_text_valid)
      {
        qcril_cm_ss_convert_ucs2_to_utf8(
            (char *)call_status_ind_ptr->end_reason_text[0].end_reason_text,
            call_status_ind_ptr->end_reason_text[0].end_reason_text_len*2,
            end_reason_text,
            sizeof(end_reason_text));
      }
      qcril_qmi_voice_handle_new_last_call_failure_cause(
          call_status_ind_ptr->call_end_reason[0].call_end_reason,
          call_status_ind_ptr->raw_call_end_cause_code_valid,
          call_status_ind_ptr->raw_call_end_cause_code[0].raw_call_end_cause_code,
          call_status_ind_ptr->sip_error_code_valid,
          call_status_ind_ptr->sip_error_code[0].sip_error_code,
          call_status_ind_ptr->end_reason_text_valid,
          end_reason_text,
          call_info_entry);
    }

    if (call_info_entry)
    {
      // qmi call id will no longer be valid for this call object
      call_info_entry->qmi_call_id = VOICE_INVALID_CALL_ID;
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_originating_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_originating_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr,
 boolean is_voice_rte_lte
)
{
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    // update call info entry
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (!call_info_entry)
    {
      // ghost call
      call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
          call_id,
          INVALID_MEDIA_ID,
          TRUE,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE,
          NULL );
      if (call_info_entry)
      {
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PHANTOM_CALL;

        if ((CALL_MODE_LTE_V02 == call_info->mode ||
              CALL_MODE_NR5G_V02 == call_info->mode ||
              CALL_TYPE_VOICE_IP_V02 == call_info->call_type ||
              CALL_TYPE_VT_V02 == call_info->call_type ||
              CALL_TYPE_EMERGENCY_IP_V02 == call_info->call_type ||
              CALL_TYPE_EMERGENCY_VT_V02 == call_info->call_type) &&
            ims_client_connected)
        {
          //Even though call mode is LTE, there may be a chance call gets connected on
          //CS domain. So better to declare elab as AUTO to handle transition scenario.
          QCRIL_LOG_INFO("Set call domain as AUTO");
          call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
          call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
        }
        else
        {
          QCRIL_LOG_INFO("set the call as a CS call");
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
          call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
          call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
        }
      }
    }
    else
    {
      qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received(call_info_entry);
    }

    if (call_info_entry)
    {
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_DIALING);
      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);
      qcril_qmi_voice_consider_shadow_remote_number_cpy_creation(call_info_entry);

      // related eme from non oos or 1xcsfb
      if (CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_IP_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_VT_V02 == call_info_entry->voice_scv_info.call_type)
      {
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERGENCY_CALL;
        if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING))
        {
          // related emergency call from non oos
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
        }
        qcril_am_set_emergency_rat(call_info_entry->voice_scv_info.mode);
      }
      else
      {
        // non emergency
        if (is_voice_rte_lte &&
            (CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type))
        {
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1x_CSFB_CALL;
          if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING))
          {
            // 1x CSFB call
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
          }
        }
        else if ((CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type ||
              CALL_TYPE_STD_OTASP_V02 == call_info_entry->voice_scv_info.call_type ||
              CALL_TYPE_NON_STD_OTASP_V02 == call_info_entry->voice_scv_info.call_type) &&
            (CALL_MODE_NO_SRV_V02 == call_info_entry->voice_scv_info.mode))
        {
          // call type is voice or OTASP, but call mode is not yet known -> we may not identify
          // if calls' RAT matches to current voice RAT, and that will hide call from UI
          // mark call as "DIAL_FROM_OOS" to reveal the call info to UI
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_DIAL_FROM_OOS;
          if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING))
          {
            // also set extended dialing flag, this enabling RIL reports DIALING state
            // at least once, that way ensuring call info is rendered if we change RAT
            // during call setup
            // call originated from unknown RAT
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
          }
        }
      }
    }
    if (qcril_qmi_voice_is_stk_cc_in_progress() && stk_cc_info.call_id_info == call_info->call_id)
    {
      // stk cc for this call
      qcril_qmi_voice_stk_cc_relay_alpha_if_necessary(instance_id, FALSE);
      qcril_qmi_voice_reset_stk_cc();
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_disconnecting_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_disconnecting_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (call_info_entry)
    {
      qcril_qmi_voice_voip_change_call_elab_when_first_call_ind_received(call_info_entry);
      switch (call_info_entry->voice_scv_info.call_state) // check previous state
      {
        case CALL_STATE_ORIGINATING_V02:
        case CALL_STATE_CONVERSATION_V02:
        case CALL_STATE_CC_IN_PROGRESS_V02:
        case CALL_STATE_ALERTING_V02:
          qmi_ril_voice_drop_homeless_incall_reqs();
          break;

        case CALL_STATE_INCOMING_V02:
          if ((CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type) &&
              (TimeKeeper::no_timer != qmi_voice_voip_overview.auto_answer_timer_id))
          {
            QCRIL_LOG_INFO("Cancel Auto Answer timed callback");
            TimeKeeper::getInstance().clear_timer(
                    qmi_voice_voip_overview.auto_answer_timer_id);
            qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;
          }
          break;

        default:
          // no action
          break;
      }
      if (CALL_STATE_SETUP_V02 == call_info_entry->voice_scv_info.call_state &&
          call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID)
      {
        TimeKeeper::getInstance().clear_timer(call_info_entry->ringing_time_id);
        call_info_entry->ringing_time_id = TimeKeeper::no_timer;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID;
      }
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          FALSE, (RIL_CallState)0);
    }
    qcril_qmi_voice_invalid_last_call_failure_cause();
    /* Ringback tone handling */
    if (call_info_entry &&
        (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALERTING_TYPE_VALID))
    {
      qcril_qmi_voice_notify_unsol_ringback_tone(call_id,
          call_info_entry->voice_svc_alerting_type.alerting_type,
          qcril_qmi_voice_call_to_ims(call_info_entry));
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_waiting_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_waiting_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;
  int rc = 0;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    rc = qcril_start_diag_log();
    QCRIL_LOG_INFO("MT call: start diag logging : %s\n", (rc == 0 ? "SUCCESS" : "FAILURE"));

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (!call_info_entry)
    {
      // fresh
      call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
          call_id,
          INVALID_MEDIA_ID,
          TRUE,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING,
          NULL );
    }
    else
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING;
      qcril_qmi_voice_voip_call_info_dump(call_info_entry);
    }
    if (call_info_entry)
    {
      if (CALL_STATE_SETUP_V02 == call_info_entry->voice_scv_info.call_state &&
          call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID)
      {
        TimeKeeper::getInstance().clear_timer(call_info_entry->ringing_time_id);
        call_info_entry->ringing_time_id = TimeKeeper::no_timer;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_RINING_TIME_ID_VALID;
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING;
      }

      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_WAITING);
      qcril_qmi_voice_consider_shadow_remote_number_cpy_creation(call_info_entry);
      qcril_qmi_voice_set_ps_cs_call_elab_vcl(call_info, call_info_entry);
      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);
      qcril_qmi_voice_set_call_reason(call_id, call_status_ind_ptr, call_info_entry);
    }
    if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_KDDI_HOLD_ANSWER))
    {
      auto hdlrMsg = std::make_shared<FlashActivateHoldAnswerMessage>(call_info->call_id);
      hdlrMsg->dispatch();
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_hold_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_hold_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id );
    if (call_info_entry == NULL)
    {
      call_info_entry = qcril_qmi_voice_create_call_info_from_srvcc_parent_call_info(
          call_id, call_status_ind_ptr);
    }
    if (call_info_entry)
    {
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          TRUE, RIL_CALL_HOLDING);
      qcril_qmi_voice_set_ps_cs_call_elab_vcl(call_info, call_info_entry);
      qcril_qmi_voice_set_audio_call_type(call_info, call_info_entry);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_cc_in_progress_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_cc_in_progress_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP, TRUE);
    if (call_info_entry)
    {
      call_info_entry->qmi_call_id = call_id;
    }
    else
    {
      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    }
    if (call_info_entry)
    {
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          FALSE, (RIL_CallState)0);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

//===========================================================================
// qcril_qmi_voice_all_call_status_ind_setup_hdlr
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type *
qcril_qmi_voice_all_call_status_ind_setup_hdlr
(
 uint8_t call_id,
 voice_all_call_status_ind_msg_v02* call_status_ind_ptr
)
{
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  voice_call_info2_type_v02 *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  call_info = qcril_qmi_voice_all_call_status_ind_get_call_info(call_id, call_status_ind_ptr);
  if (call_status_ind_ptr && call_info)
  {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(call_id);
    if (!call_info_entry)
    {
      // fresh
      call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
          call_id,
          INVALID_MEDIA_ID,
          TRUE,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE,
          NULL);
    }
    else
    {
      // what we got?
      qcril_qmi_voice_voip_call_info_dump(call_info_entry);
    }
    if (call_info_entry)
    {
      qcril_qmi_voice_set_domain_elab_from_call_type(call_info->call_type, call_info_entry);
      qcril_qmi_voice_voip_update_call_info_entry_mainstream(call_info_entry, call_status_ind_ptr,
          FALSE, (RIL_CallState)0);

      if (!(call_info_entry->elaboration &
            QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING))
      {
        // got to initiate ringing
        call_info_entry->elaboration |=
          (QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING |
           QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_RING_DONE);
        QCRIL_LOG_INFO( "launch ringer" );
        auto msg = std::make_shared<VoiceMakeIncomingCallRingMessage>();
        Dispatcher::getInstance().dispatch(msg);
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
  return call_info_entry;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_all_call_status_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_ALL_CALL_STATUS_IND_V02.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_all_call_status_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_all_call_status_ind_msg_v02* call_status_ind_ptr = NULL;
  unsigned int i;
  uint32_t nof_ims_calls = 0;
  uint32_t nof_atel_calls = 0;
  boolean  hangup_call = FALSE;
  QCRIL_NOTUSED(ind_data_len);

  /* variables defined to track modem initiated silent redial */
  boolean is_request_on_autodomain = FALSE;
  boolean is_call_on_cs_domain     = FALSE;
  boolean is_silent_redialed       = FALSE;

  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;
  voice_call_info2_type_v02*                  iter_call_info = NULL;
  int                                         calls_nof_remaining;
  int                                         calls_iter;
  int                                         is_deviant_call;
  bool                                        is_mode_resonable;

  int                                         imperative_report;
  int                                         post_call_cleanup_may_be_necessary;

  qmi_ril_nw_reg_rte_type                     current_voice_rte;
  int                                         reported_voice_radio_tech;

  qcril_qmi_voice_voip_call_info_entry_type * cdma_voice_call_info_entry = NULL;

  qcril_qmi_voice_voip_current_call_summary_type
                                              calls_summary_beginning;
  qcril_qmi_voice_voip_current_call_summary_type
                                              calls_summary_end;

  char                                        log_essence[ QCRIL_MAX_LOG_MSG_SIZE ];
  char                                        log_addon[ QCRIL_MAX_LOG_MSG_SIZE ];

  int rc = 0;

  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;

  bool is_alerting_data_snapshot = false;

  QCRIL_LOG_FUNC_ENTRY();

  memset( &calls_summary_beginning, 0, sizeof( calls_summary_beginning ) );
  memset( &calls_summary_end, 0, sizeof( calls_summary_end ) );

  snprintf( log_essence, QCRIL_MAX_LOG_MSG_SIZE,
            "RILVIMS: update" );

  if( ind_data_ptr != NULL )
  {
      call_status_ind_ptr = (voice_all_call_status_ind_msg_v02*)ind_data_ptr;

      post_call_cleanup_may_be_necessary = FALSE;

      snprintf( log_addon, QCRIL_MAX_LOG_MSG_SIZE,
                " %d calls", call_status_ind_ptr->call_info_len );
      strlcat( log_essence, log_addon, sizeof( log_essence ) );

      auto query_tech = qcril_qmi_voice_external_get_voice_radio_tech();
      current_voice_rte = query_tech.currentVoiceRadioTech;
      reported_voice_radio_tech = query_tech.reportedVoiceRadioTech;

      cdma_voice_call_info_entry = NULL;

      qcril_qmi_voice_voip_lock_overview();

      qcril_qmi_voice_voip_generate_summary( &calls_summary_beginning );

      for (i = 0; (i < call_status_ind_ptr->call_info_len && i < QMI_VOICE_CALL_INFO_MAX_V02); i++)
      {
        // call info
        iter_call_info = &call_status_ind_ptr->call_info[i];
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(
            iter_call_info->call_id);
        if (call_info_entry)
        {
          snprintf(log_addon, QCRIL_MAX_LOG_MSG_SIZE,
                    "[a-c-id: %d, q-c-id %d, c-s %d -> %d, c-t: %d / %d, c-m: %d / %d]",
                        (int)call_info_entry->android_call_id,
                        (int)iter_call_info->call_id,
                        (int)call_info_entry->voice_scv_info.call_state,
                        (int)iter_call_info->call_state,
                        (int)call_info_entry->voice_scv_info.call_type,
                        (int)iter_call_info->call_type,
                        (int)call_info_entry->voice_scv_info.mode,
                        (int)iter_call_info->mode);
        }
        else
        {
          snprintf(log_addon, QCRIL_MAX_LOG_MSG_SIZE,
              "[uncached q-c-id: %d, c-s: %d, c-t: %d, c-m: %d]",
              (int)iter_call_info->call_id,
              (int)iter_call_info->call_state,
              (int)iter_call_info->call_type,
              (int)iter_call_info->mode);
        }
        strlcat(log_essence, log_addon, sizeof(log_essence));

        call_info_entry = NULL;

        switch (iter_call_info->call_state)
        {
          case CALL_STATE_PRE_ALERTING_V02:
            QCRIL_LOG_ESSENTIAL("call state PRE_ALERTING for conn id %d", iter_call_info->call_id);
            // Call state changed with PRE ALERTING will not be informed to telephony.
            // Call state with PRE ALERTING will be blocked as we are setting elobration as
            // QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE when call_info_entry is created.
            call_info_entry = qcril_qmi_voice_all_call_status_ind_pre_alerting_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            break;  // end of case CALL_STATE_PRE_ALERTING_V02

          case CALL_STATE_INCOMING_V02:
            QCRIL_LOG_ESSENTIAL("call state INCOMING for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_incoming_hdlr(
                iter_call_info->call_id, call_status_ind_ptr,
                (QMI_RIL_RTE_SUB_LTE == current_voice_rte ||
                 RADIO_TECH_LTE == reported_voice_radio_tech));
            break; // end of case CALL_STATE_INCOMING_V02

          case CALL_STATE_ALERTING_V02:
            QCRIL_LOG_ESSENTIAL("call state ALERTING for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_alerting_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            if (call_info_entry &&
                (!((call_info_entry->elaboration &
                    QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID) &&
                   call_info_entry->is_srvcc.is_srvcc_call) &&
                 ((CALL_MODE_LTE_V02 != call_info_entry->voice_scv_info.mode) ||
                  (CALL_MODE_NR5G_V02 != call_info_entry->voice_scv_info.mode) ||
                  (CALL_MODE_WLAN_V02 != call_info_entry->voice_scv_info.mode) ||
                  (CALL_MODE_C_IWLAN_V02 != call_info_entry->voice_scv_info.mode) ||
                  (CALL_MODE_NO_SRV_V02 != call_info_entry->voice_scv_info.mode) ||
                  (CALL_MODE_UNKNOWN_V02 != call_info_entry->voice_scv_info.mode))))
            {
              is_alerting_data_snapshot = true;
            }
            break; // end of case CALL_STATE_ALERTING_V02

          case CALL_STATE_CONVERSATION_V02:
            QCRIL_LOG_ESSENTIAL("call state CONVERSATION for conn id %d", iter_call_info->call_id);

            call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(
                iter_call_info->call_id);
            if (call_info_entry)
            {
              // set the domain flag before ELA is reset by call_type
              is_request_on_autodomain = (call_info_entry->elaboration &
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN) ? TRUE : FALSE;
            }

            call_info_entry = qcril_qmi_voice_all_call_status_ind_conversation_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);

            if (call_info_entry)
            {
              // set the calltype flag after ELA is reset by call_type
              is_call_on_cs_domain = (call_info_entry->elaboration &
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN) ? TRUE : FALSE;
            }
            is_silent_redialed = is_request_on_autodomain && is_call_on_cs_domain;
            QCRIL_LOG_INFO("is_request_on_autodomain:%d is_call_on_cs_domain:%d is_silent_redialed:%d",
                is_request_on_autodomain, is_call_on_cs_domain, is_silent_redialed);
            break; // end of case CALL_STATE_CONVERSATION_V02

          case CALL_STATE_END_V02:
            QCRIL_LOG_ESSENTIAL("call state END for conn id %d", iter_call_info->call_id);
            post_call_cleanup_may_be_necessary = TRUE;
            call_info_entry = qcril_qmi_voice_all_call_status_ind_end_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            break; // end of case CALL_STATE_END_V02

          case CALL_STATE_ORIGINATING_V02:
            QCRIL_LOG_ESSENTIAL("call state ORIGINATING for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_originating_hdlr(
                iter_call_info->call_id, call_status_ind_ptr,
                (QMI_RIL_RTE_SUB_LTE == current_voice_rte ||
                 RADIO_TECH_LTE == reported_voice_radio_tech));
            break; // end of case CALL_STATE_ORIGINATING_V02

          case CALL_STATE_DISCONNECTING_V02:
            QCRIL_LOG_ESSENTIAL("call state DISCONNECTING for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_disconnecting_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            break; // end of case CALL_STATE_DISCONNECTING_V02

          case CALL_STATE_WAITING_V02:
            QCRIL_LOG_ESSENTIAL("call state WAITING for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_waiting_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            break; // end of case CALL_STATE_WAITING_V02

          case CALL_STATE_HOLD_V02:
            QCRIL_LOG_ESSENTIAL("call state HOLD for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_hold_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);
            break; // end of case CALL_STATE_HOLD_V02

          case CALL_STATE_CC_IN_PROGRESS_V02:
            QCRIL_LOG_ESSENTIAL("call state CC IN PROGRESS for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_cc_in_progress_hdlr(
                iter_call_info->call_id, call_status_ind_ptr);

            if (call_info_entry &&
                (call_info_entry->elaboration &
                 QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HANGUP_AFTER_VALID_QMI_ID))
            {
              call_info_entry->elaboration &=
                (~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HANGUP_AFTER_VALID_QMI_ID);
              hangup_call = TRUE;
              QCRIL_LOG_INFO("Call needs to be hung up");
            }
            break; // end of case CALL_STATE_CC_IN_PROGRESS_V02

          case CALL_STATE_SETUP_V02:
            QCRIL_LOG_ESSENTIAL("call state SETUP for conn id %d", iter_call_info->call_id);
            call_info_entry = qcril_qmi_voice_all_call_status_ind_setup_hdlr(
                iter_call_info->call_id,
                call_status_ind_ptr);
            break; // end of case CALL_STATE_SETUP_V02

          default:
            QCRIL_LOG_ESSENTIAL("unexpected call state(%d)  for conn id %d",
                iter_call_info->call_state, iter_call_info->call_id);
            call_info_entry = NULL;
            break;
        } // switch ( iter_call_info->call_state )

        // eme oos - extended dialing
        if ( NULL != call_info_entry )
        {
          switch ( iter_call_info->call_state )
          {
            case CALL_STATE_CONVERSATION_V02: // fallthrough
            case CALL_STATE_DISCONNECTING_V02:
            case CALL_STATE_ALERTING_V02:
            case CALL_STATE_END_V02:
              if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING &&
                   !( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING_ENDING ) )
              {
                call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING_ENDING;
                call_info_entry->elaboration &= ~( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING_ENDING | QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING );
              }
              break;

            default:
              // nothing
              break;
          }
        }
        //Evaluating calls that need to be skipped for now
        if (iter_call_info->call_state == CALL_STATE_CC_IN_PROGRESS_V02 ||
                (iter_call_info->call_type == CALL_TYPE_VOICE_IP_V02 ||
                 iter_call_info->call_type == CALL_TYPE_VT_V02 ||
                 iter_call_info->call_type == CALL_TYPE_EMERGENCY_IP_V02 ||
                 iter_call_info->call_type == CALL_TYPE_EMERGENCY_VT_V02))
        {
           is_deviant_call = FALSE;
        }
        else
        {
           is_deviant_call = qcril_qmi_voice_nas_control_is_call_mode_reported_voice_radio_tech_different( iter_call_info->mode );
        }
        QCRIL_LOG_INFO(".. is deviant call pre %d ", (int) is_deviant_call );
        if ( is_deviant_call )
        {
          switch ( iter_call_info->call_state )
          {
            case CALL_STATE_ORIGINATING_V02:
            case CALL_STATE_INCOMING_V02:
            case CALL_STATE_CC_IN_PROGRESS_V02:
            case CALL_STATE_SETUP_V02:
              // keep is_deviant_call as is
              break;

            default: // do not allow hiding call for post connecetd call states
              is_deviant_call = FALSE;
              break;
          }
        }
        is_mode_resonable = qcril_qmi_voice_external_is_call_mode_reasonable_based_on_devcfg(iter_call_info->mode);
        QCRIL_LOG_INFO(".. is deviant call final %d, is mode reasonable %d", (int) is_deviant_call, (int)is_mode_resonable );


        if ( call_info_entry )
        {
          if ( call_info_entry->elaboration &
               (QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EME_FROM_OOS |
                QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERGENCY_CALL |
                QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1x_CSFB_CALL |
                QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_DIAL_FROM_OOS) )
          {
            imperative_report = TRUE;
          }
          else
          {
            imperative_report = FALSE;
          }
        }
        else
        {
          imperative_report = FALSE;
        }
        QCRIL_LOG_INFO(".. imperative report %d ", (int) imperative_report );

        if ( !( CALL_STATE_SETUP_V02 == iter_call_info->call_state ||
                CALL_STATE_DISCONNECTING_V02 == iter_call_info->call_state ||
                CALL_STATE_PRE_ALERTING_V02 == iter_call_info->call_state ||
                ( is_deviant_call && !imperative_report && is_mode_resonable )
              )
           )
        {  // need to report the given call at this time
           if ( qcril_qmi_voice_call_to_ims(call_info_entry) )
           {
             nof_ims_calls++;
           }

           if ( qcril_qmi_voice_call_to_atel(call_info_entry) )
           {
             nof_atel_calls++;
           }
        }

        if ( call_info_entry )
        {
          if ( is_deviant_call && is_mode_resonable && !imperative_report )
          {
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE;
            QCRIL_LOG_INFO("skipping call with android call id %d / qmi call id %d as call mode does not match with the current voice radio tech",
                           call_info_entry->android_call_id,
                           call_info_entry->qmi_call_id );
          }
          else
          {
            if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE )
            { // "shadowing" no longer needed
              call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE;
            }
          }
        }

        // oversight update -- start
        memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
        oversight_event_params.new_call_state      = iter_call_info->call_state;
        oversight_event_params.locator.qmi_call_id = iter_call_info->call_id;
        (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_IND, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID, &oversight_event_params );

        if ( NULL != call_info_entry )
        {
          if ( VOICE_INVALID_CALL_ID != call_info_entry->android_call_id )
          {
            memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
            oversight_event_params.new_call_state             = iter_call_info->call_state;
            oversight_event_params.locator.android_call_id    = call_info_entry->android_call_id;
            (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_IND, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID, &oversight_event_params );
          }

          memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
          oversight_event_params.new_call_state               = iter_call_info->call_state;
          oversight_event_params.locator.elaboration_pattern  = call_info_entry->elaboration;
          (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_IND, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN, &oversight_event_params );
        }

        if ( CALL_STATE_END_V02 == iter_call_info->call_state )
        {
          memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
          oversight_event_params.locator.qmi_call_id = iter_call_info->call_id;
          (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_CALL_ENDED, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID, &oversight_event_params );

          if ( NULL != call_info_entry )
          {
            if ( VOICE_INVALID_CALL_ID != call_info_entry->android_call_id )
            {
              memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
              oversight_event_params.locator.android_call_id   = call_info_entry->android_call_id;
              (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_CALL_ENDED, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID, &oversight_event_params );
            }

            memset( &oversight_event_params, 0, sizeof( oversight_event_params ) );
            oversight_event_params.locator.elaboration_pattern  = call_info_entry->elaboration;
            (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_CALL_ENDED, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN, &oversight_event_params );
          }
        }
        // oversight update -- end
      } // for ( i = 0; i < call_status_ind_ptr->call_info_len; i++ )

      //Destroy the call info objects for the calls ended as part of srvcc
      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_END_AFTER_SRVCC, TRUE );
      if ( NULL != call_info_entry )
      {
        qcril_qmi_voice_voip_destroy_call_info_entry( call_info_entry );
      }

      // dump call objs to log
      calls_nof_remaining = 0;
      calls_iter          = 0;
      QCRIL_LOG_INFO( "-- final call dump start --" );
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
      while ( NULL != call_info_entry )
      {
        calls_nof_remaining++;
        call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
      }
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
      while ( NULL != call_info_entry )
      {
        calls_iter++;
        QCRIL_LOG_INFO( "- dumping call %d out of %d", calls_iter, calls_nof_remaining );
        qcril_qmi_voice_voip_call_info_dump( call_info_entry );
        call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
      }
      QCRIL_LOG_INFO( "-- final call dump end --" );

      if( TRUE == qcril_qmi_voice_is_cdma_voice_emergency_calls_present(&cdma_voice_call_info_entry, NULL) )
      {
        if( NULL != cdma_voice_call_info_entry )
        {
          cdma_voice_call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_REPORT_CACHED_RP_NUMBER;
        }
        QCRIL_LOG_INFO( "flagged cached RP num %p", cdma_voice_call_info_entry );
      }

      qcril_qmi_voice_voip_generate_summary( &calls_summary_end );
      if (qmi_ril_voice_is_audio_inactive_vcl())
      {
         qcril_qmi_voice_hangup_all_non_emergency_calls_vcl();
      }

      if (qcril_qmi_pending_wps_call_info && calls_summary_end.nof_voip_calls == QMI_RIL_ZERO)
      {
        qcril_qmi_voice_trigger_possible_pending_wps_call();
      }

      qcril_qmi_voice_voip_unlock_overview();

      // post cleanup
      if (post_call_cleanup_may_be_necessary &&
          calls_summary_end.nof_calls_overall == QMI_RIL_ZERO)
      {
        qmi_ril_succeed_on_pending_hangup_req_on_no_calls_left();
        qmi_ril_voice_cleanup_reqs_after_call_completion();
      }

      auto voiceModule = getVoiceModule();
      if (voiceModule && voiceModule->setAudioStateBeforeCallStatusReport()) {
        qcril_am_handle_event(QCRIL_AM_EVENT_CALL_STATE_CHANGED, NULL);
      }

      if (nof_atel_calls)
      {
        // send SRVCC ind to RILD if IMS dial request if silent redialed
        if (is_silent_redialed)
        {
          QCRIL_LOG_ERROR("modem silent redialed, sending SRVCC IND");
          // send HANDOVER STARTED and COMPLETED
          qcril_qmi_voice_send_simulated_SRVCC_ind();
        }
      }
      if (nof_ims_calls || nof_atel_calls)
      {
        qcril_qmi_voice_send_unsol_call_state_changed();
      }

      if ( calls_summary_end.nof_voice_calls != calls_summary_beginning.nof_voice_calls )
      {
        auto msg = std::make_shared<QcRilUnsolVoiceNumberOfCsCallChangedMessage>(
                calls_summary_end.nof_voice_calls);
        Dispatcher::getInstance().broadcast(msg);
      }

      if (hangup_call)
      {
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
                                            QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP,
                                            TRUE );
        if (call_info_entry)
        {
            qcril_qmi_voice_send_hangup_on_call(call_info_entry->android_call_id);
        }
      }
      if (voiceModule && voiceModule->setAudioStateAfterCallStatusReport()) {
        qcril_am_handle_event(QCRIL_AM_EVENT_CALL_STATE_CHANGED, NULL);
      }
  }

  auto ipcCallStatMsg = std::make_shared<IpcCallStatusMessage>(qmi_ril_get_process_instance_id());
  if (ipcCallStatMsg) {
    ipcCallStatMsg->setTotalNumberOfCalls(calls_summary_end.nof_calls_overall);
    ipcCallStatMsg->broadcast();
  }

  // Reset isAudioReleased to false when all calls have ended to avoid stale value.
  if (calls_summary_end.nof_calls_overall == 0) {
    set_audio_released(false);
  }

  // Register or deregister for the qmi voice indicatons.
  boolean enable_ind = qcril_qmi_voice_has_specific_call(qcril_qmi_voice_is_active_ims_call, NULL);
  if (enable_ind || qcril_qmi_voice_external_is_screen_off())
  {
      qcril_qmi_voice_enable_voice_indications(enable_ind);
  }

  if (qcril_qmi_voice_voip_call_info_entries_is_empty())
  {
      QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
      rc = qcril_stop_diag_log();
      if (rc == 0)
      {
          QCRIL_LOG_INFO("qxdm logging disabled successfully");
      }
  }

  auto msg = std::make_shared<QcRilVoiceNotifyCallInProcessMessage>();
  if (msg != nullptr) {
    msg->setAlertingCsCall(is_alerting_data_snapshot);
    Dispatcher::getInstance().broadcast(msg);
  } else {
    QCRIL_LOG_ERROR("failed to allocate message QcRilVoiceNotifyCallInProcessMessage");
  }

  QCRIL_LOG_ESSENTIAL ( "%s", log_essence );

  QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_voice_all_call_status_ind_hdlr

//===========================================================================
// qcril_qmi_voice_is_cdma_voice_emergency_calls_present
//
//
// Passes back the pointer to the voice call info entry
// returns TRUE if a emergency call has been dialled on top of a cdma voice call
//===========================================================================
int qcril_qmi_voice_is_cdma_voice_emergency_calls_present(qcril_qmi_voice_voip_call_info_entry_type **cdma_voice_call_info_entry,
                                                                 qcril_qmi_voice_voip_call_info_entry_type **cdma_no_srv_emer_call_info_entry)
{
  int ret;
  int nof_calls;
  int nof_1x_voice_calls;
  int nof_1x_no_srv_emergency_calls;
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  ret = FALSE;
  nof_calls = QMI_RIL_ZERO;
  nof_1x_voice_calls = QMI_RIL_ZERO;
  nof_1x_no_srv_emergency_calls = QMI_RIL_ZERO;
  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();

  while ( NULL != call_info_entry )
  {
    if ( call_info_entry->android_call_id != VOICE_INVALID_CALL_ID &&
         call_info_entry->voice_scv_info.call_state != CALL_STATE_END_V02 )
    {
      nof_calls++;
      if ( ( CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type ||
             ( CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type &&
               (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_REPORT_CACHED_RP_NUMBER) ) ) &&
           CALL_MODE_CDMA_V02 == call_info_entry->voice_scv_info.mode )
      {
        nof_1x_voice_calls++;
        if( cdma_voice_call_info_entry )
        {
          *cdma_voice_call_info_entry = call_info_entry;
        }
      }
      else if ( CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type &&
                ( CALL_MODE_CDMA_V02 == call_info_entry->voice_scv_info.mode ||
                  CALL_MODE_NO_SRV_V02 == call_info_entry->voice_scv_info.mode ) )
      {
        nof_1x_no_srv_emergency_calls++;
        if( cdma_no_srv_emer_call_info_entry )
        {
          *cdma_no_srv_emer_call_info_entry = call_info_entry;
        }
      }
    }
    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }
  QCRIL_LOG_INFO( "nof calls %d, nof cdma voice calls %d, nof 1x/no_srv emergency calls %d", nof_calls, nof_1x_voice_calls, nof_1x_no_srv_emergency_calls );

  if ( 2 == nof_calls &&
       1 == nof_1x_voice_calls &&
       1 == nof_1x_no_srv_emergency_calls )
  {
    ret= TRUE;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
} //qcril_qmi_voice_is_cdma_voice_emergency_calls_present

//===========================================================================
// qmi_ril_voice_ended_call_obj_phaseout
//===========================================================================
void qmi_ril_voice_ended_call_obj_phase_out(uint8_t call_id)
{
    qcril_qmi_voice_voip_call_info_entry_type* call_info = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_voice_voip_lock_overview();

    call_info = qcril_qmi_voice_voip_call_info_entries_enum_first();
    while ( NULL != call_info )
    {
      if (VOICE_INVALID_CALL_ID != call_info->android_call_id &&
          CALL_STATE_END_V02 == call_info->voice_scv_info.call_state &&
          call_id == call_info->android_call_id)
      {
        // timeout on keeping call obj, delete it now
        qcril_qmi_voice_voip_destroy_call_info_entry( call_info );
      }

      call_info = qcril_qmi_voice_voip_call_info_entries_enum_next();
    }

    qcril_qmi_voice_voip_unlock_overview();

    if (qcril_qmi_voice_voip_call_info_entries_is_empty())
    {
        QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
        if (!qcril_stop_diag_log())
        {
            QCRIL_LOG_INFO("qxdm logging disabled successfully");
        }
    }

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_ended_call_obj_phaseout

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_modified_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_MODIFIED_IND_V02.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_modified_ind_hdlr
(
 void   *data_ptr,
 uint32_t data_len
)
{
  voice_modified_ind_msg_v02 *modify_ind_ptr = NULL;
  qcril_qmi_voice_voip_call_info_entry_type *call_info = NULL;
  voice_all_call_status_ind_msg_v02          call_status_ind;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(data_len);

  memset(&call_status_ind, 0, sizeof(call_status_ind));

  if (data_ptr != NULL)
  {
    modify_ind_ptr = (voice_modified_ind_msg_v02 *)data_ptr;

    call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(modify_ind_ptr->call_id);

    if (call_info != NULL)
    {
      /* udpate the call type */
      if (modify_ind_ptr->call_type_valid)
      {
        call_info->voice_scv_info.call_type = modify_ind_ptr->call_type;
      }

      if (modify_ind_ptr->audio_attrib_valid)
      {
        call_status_ind.audio_attrib_valid = TRUE;
        call_status_ind.audio_attrib_len = 1;
        call_status_ind.audio_attrib[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.audio_attrib[0].call_attributes = modify_ind_ptr->audio_attrib;
      }

      if (modify_ind_ptr->video_attrib_valid)
      {
        call_status_ind.video_attrib_valid = TRUE;
        call_status_ind.video_attrib_len = 1;
        call_status_ind.video_attrib[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.video_attrib[0].call_attributes = modify_ind_ptr->video_attrib;
      }

      if (modify_ind_ptr->call_attrib_status_valid)
      {
        call_status_ind.call_attrib_status_valid = TRUE;
        call_status_ind.call_attrib_status_len = 1;
        call_status_ind.call_attrib_status[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.call_attrib_status[0].call_attrib_status =
          modify_ind_ptr->call_attrib_status;
      }
      // Local Call Capabilities
      if (modify_ind_ptr->local_call_capabilities_valid)
      {
        call_status_ind.local_call_capabilities_info_valid = TRUE;
        call_status_ind.local_call_capabilities_info_len = 1;
        call_status_ind.local_call_capabilities_info[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.local_call_capabilities_info[0].audio_attrib =
          modify_ind_ptr->local_call_capabilities.audio_attrib;
        call_status_ind.local_call_capabilities_info[0].audio_cause  =
          modify_ind_ptr->local_call_capabilities.audio_cause;
        call_status_ind.local_call_capabilities_info[0].video_attrib =
          modify_ind_ptr->local_call_capabilities.video_attrib;
        call_status_ind.local_call_capabilities_info[0].video_cause  =
          modify_ind_ptr->local_call_capabilities.video_cause;
      }
      // Peer Call Capabilities
      if( modify_ind_ptr->peer_call_capabilities_valid)
      {
        call_status_ind.peer_call_capabilities_info_valid = TRUE;
        call_status_ind.peer_call_capabilities_info_len = 1;
        call_status_ind.peer_call_capabilities_info[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.peer_call_capabilities_info[0].audio_attrib =
          modify_ind_ptr->peer_call_capabilities.audio_attrib;
        call_status_ind.peer_call_capabilities_info[0].audio_cause  =
          modify_ind_ptr->peer_call_capabilities.audio_cause;
        call_status_ind.peer_call_capabilities_info[0].video_attrib =
          modify_ind_ptr->peer_call_capabilities.video_attrib;
        call_status_ind.peer_call_capabilities_info[0].video_cause  =
          modify_ind_ptr->peer_call_capabilities.video_cause;
      }
      //RTT mode check
      if (modify_ind_ptr->rtt_mode_valid  == TRUE)
      {
        call_status_ind.rtt_mode_valid = TRUE;
        call_status_ind.rtt_mode_len = 1;
        call_status_ind.rtt_mode[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.rtt_mode[0].rtt_mode = modify_ind_ptr->rtt_mode;
        QCRIL_LOG_DEBUG("modify ind rtt mode = %d", modify_ind_ptr->rtt_mode);
      }

      //RTT capabilities check
      if (modify_ind_ptr->rtt_capabilities_valid == TRUE )
      {
        call_status_ind.rtt_capabilities_info_valid = TRUE;
        call_status_ind.rtt_capabilities_info_len = 1;
        call_status_ind.rtt_capabilities_info[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.rtt_capabilities_info[0].rtt_capabilities.local_rtt_cap =
          modify_ind_ptr->rtt_capabilities.local_rtt_cap;
        QCRIL_LOG_DEBUG("local rtt capability = %d", modify_ind_ptr->rtt_capabilities.local_rtt_cap);
        call_status_ind.rtt_capabilities_info[0].rtt_capabilities.peer_rtt_cap  =
          modify_ind_ptr->rtt_capabilities.peer_rtt_cap;
        QCRIL_LOG_DEBUG("peer rtt capability = %d", modify_ind_ptr->rtt_capabilities.peer_rtt_cap);
      }

      //Vos Support check
      if (modify_ind_ptr->vos_support_valid == TRUE )
      {
        call_status_ind.vos_support_valid = TRUE;
        call_status_ind.vos_support_len = 1;
        call_status_ind.vos_support[0].call_id = modify_ind_ptr->call_id;
        call_status_ind.vos_support[0].vos_support = modify_ind_ptr->vos_support;
        QCRIL_LOG_DEBUG("modify ind vos support = %d", modify_ind_ptr->vos_support);
      }

      /* update the audio/video parameters */
      qcril_qmi_voice_voip_update_call_info_entry_mainstream (call_info, &call_status_ind,
          FALSE, call_info->ril_call_state);

      if(modify_ind_ptr->call_modified_cause_valid)
      {
        qcril_qmi_voice_call_info_entry_set_call_modified_cause(call_info,
            modify_ind_ptr->call_modified_cause);
      }

      // Reset the answered_call_type flag
      qcril_qmi_voice_voip_reset_answered_call_type(call_info, modify_ind_ptr);

      /* check for modify initiate pending request */
      std::shared_ptr<Message> pendingMsg = findVoiceModulePendingMessage(
          QcRilRequestImsModifyCallInitiateMessage::get_class_message_id());
      /* now check for modify confirm pending request as both of them wait for same indication */
      if (pendingMsg == nullptr)
      {
        pendingMsg = findVoiceModulePendingMessage(
            QcRilRequestImsModifyCallConfirmMessage::get_class_message_id());
      }

      if (pendingMsg != nullptr)
      {
        /* modify was initiated/accepted(upgrade) by user, send the response now */
        auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
        if (msg->getPendingMessageState() == PendingMessageState ::AWAITING_INDICATION)
        {
          eraseVoiceModulePendingMessage(pendingMsg);
          if (pendingMsg)
          {
            auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(
                (modify_ind_ptr->failure_cause_valid ? RIL_E_GENERIC_FAILURE : RIL_E_SUCCESS),
                nullptr);
            msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
          }
        }
      }
      else
      {
        if (modify_ind_ptr->failure_cause_valid && modify_ind_ptr->failure_cause)
        {
          auto msg = std::make_shared<QcRilUnsolImsModifyCallMessage>();

          if (call_info->elaboration &
              QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MODIFY_CONFIRM_PENDING)
          {
            if (msg != nullptr) {
              msg->setCallIndex(call_info->android_call_id);
              msg->setCallType(call_info->to_modify_call_type);
              msg->setCallDomain(call_info->to_modify_call_domain);
              msg->setCallModifyFailCause(qcril::interfaces::CallModifyFailCause::CANCELLED);
              Dispatcher::getInstance().dispatchSync(msg);
            }
            call_info->elaboration &=
              ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MODIFY_CONFIRM_PENDING;
          }
          else
          {
            QCRIL_LOG_DEBUG("QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MODIFY_CONFIRM_PENDING is not set");
          }
        }
      }

      qcril_qmi_voice_send_unsol_call_state_changed();
    }
    else
    {
      QCRIL_LOG_DEBUG("could not find call-id = %d, ignoring modify ind", modify_ind_ptr->call_id);
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("received null data, ignoring modify ind");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_modified_ind_hdlr


/*=========================================================================
  FUNCTION:  qcril_qmi_voice_modify_accept_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_MODIFY_ACCEPT_IND_V02.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_modify_accept_ind_hdlr
(
  void   *data_ptr,
  uint32_t data_len
)
{
  voice_modify_accept_ind_msg_v02 *modify_ind_ptr = NULL;
  boolean                          result = false;
  qcril_qmi_voice_voip_call_info_entry_type *call_info = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(data_len);

  if (data_ptr != NULL)
  {
    modify_ind_ptr = (voice_modify_accept_ind_msg_v02 *)data_ptr;

    call_info = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(modify_ind_ptr->call_id);

    if (call_info != NULL)
    {
      qcril::interfaces::CallType callType;
      qcril::interfaces::CallDomain callDomain;
      uint32_t callSubState;

      result = qcril_qmi_voice_get_atel_call_type_info(modify_ind_ptr->call_type,
          modify_ind_ptr->video_attrib_valid,
          modify_ind_ptr->video_attrib,
          modify_ind_ptr->audio_attrib_valid,
          modify_ind_ptr->audio_attrib,
          FALSE,
          VOICE_CALL_ATTRIB_STATUS_OK_V02,
          FALSE,
          0,
          qcril::interfaces::CallType::UNKNOWN,
          callType,
          callDomain,
          callSubState);

      if (result)
      {
        auto msg = std::make_shared<QcRilUnsolImsModifyCallMessage>();
        if (msg != nullptr) {
          msg->setCallIndex(call_info->android_call_id);
          msg->setCallType(callType);
          msg->setCallDomain(callDomain);
          if (modify_ind_ptr->rtt_mode_valid)
          {
            msg->setRttMode(qcril_qmi_ims_map_qcril_rtt_mode_to_ims(modify_ind_ptr->rtt_mode));
          }
          if (modify_ind_ptr->vos_support_valid)
          {
            msg->setVosSupport(modify_ind_ptr->vos_support);
          }
          Dispatcher::getInstance().dispatchSync(msg);
        }
        call_info->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MODIFY_CONFIRM_PENDING;
        call_info->to_modify_call_domain = callDomain;
        call_info->to_modify_call_type = callType;
      }
      else
      {
        QCRIL_LOG_DEBUG("could not convert modem call type to atel call type");
      }
    }
    else
    {
      QCRIL_LOG_DEBUG("could not find call-id = %d, ignoring modify accept ind",
          modify_ind_ptr->call_id);
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("received null data, ignoring modify accept ind");
  }

  QCRIL_LOG_FUNC_RETURN();
}

qcril::interfaces::ComputedAudioQuality qcril_qmi_map_computed_audio_quality(
    const voice_audio_quality_enum_v02 audioQuality) {
  switch (audioQuality) {
    case VOICE_AUDIO_QUALITY_NO_HD_V02:
      return qcril::interfaces::ComputedAudioQuality::NO_HD;
    case VOICE_AUDIO_QUALITY_HD_V02:
      return qcril::interfaces::ComputedAudioQuality::HD;
    case VOICE_AUDIO_QUALITY_HD_PLUS_V02:
      return qcril::interfaces::ComputedAudioQuality::HD_PLUS;
    default:
      return qcril::interfaces::ComputedAudioQuality::NONE;
  }
}

/*===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_SPEECH_CODEC_INFO_IND_V02.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_speech_codec_info_ind_hdlr
(
  void   *data_ptr,
  uint32_t data_len
)
{
  voice_speech_codec_info_ind_msg_v02* speech_codec_ptr;
  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED(data_len);

  if( data_ptr )
  {
     speech_codec_ptr = (voice_speech_codec_info_ind_msg_v02*) data_ptr;

     // add codec info to the associated call
     if (speech_codec_ptr->call_id_valid &&
         (speech_codec_ptr->speech_codec_valid || speech_codec_ptr->audio_quality_valid)) {
       qcril_qmi_voice_voip_lock_overview();
       qcril_qmi_voice_voip_call_info_entry_type* call_info_entry =
           qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(speech_codec_ptr->call_id);
       if (NULL == call_info_entry) {
         QCRIL_LOG_ERROR("did not find the call with qmi id: %d", speech_codec_ptr->call_id);
       } else {
         if (speech_codec_ptr->speech_codec_valid) {
           call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CODEC_VALID;
           call_info_entry->codec = speech_codec_ptr->speech_codec;
         }

         qcril_qmi_voice_voip_call_info_set_single_elaboration(
             call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_COMPUTED_AUDIO_QUALITY_VALID,
             FALSE);
         if (speech_codec_ptr->audio_quality_valid) {
           qcril_qmi_voice_voip_call_info_set_single_elaboration(
               call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_COMPUTED_AUDIO_QUALITY_VALID,
               TRUE);
           call_info_entry->computed_audio_quality =
               qcril_qmi_map_computed_audio_quality(speech_codec_ptr->audio_quality);
         }
       }
       qcril_qmi_voice_voip_unlock_overview();

       if (call_info_entry) {
         qcril_qmi_voice_send_unsol_call_state_changed();
       }

       if (speech_codec_ptr->speech_codec_valid &&
           getVoiceModuleFeature(VoiceFeatureType::REPORT_SPEECH_CODEC)) {
         if (call_info_entry) {
           auto msg = std::make_shared<QcRilUnsolSpeechCodecInfoMessage>(
               call_info_entry->android_call_id, speech_codec_ptr->speech_codec,
               (speech_codec_ptr->network_mode_valid ? speech_codec_ptr->network_mode
                                                     : VOICE_NETWORK_MODE_NONE_V02));
           if (msg != nullptr) {
             Dispatcher::getInstance().dispatchSync(msg);
           }
         }
       }
     }
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_nas_control_is_call_mode_reported_voice_radio_tech_different

===========================================================================*/
/*!
    @brief
    Compares call_mode to reported voice radio tech and
    returns TRUE if they do not belong to the same radio_technology family

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_voice_nas_control_is_call_mode_reported_voice_radio_tech_different(call_mode_enum_v02 call_mode)
{
  unsigned int res = FALSE;
  unsigned int call_radio_tech = RADIO_TECH_UNKNOWN;
  unsigned int old_call_radio_tech = RADIO_TECH_UNKNOWN;
  unsigned int old_call_radio_tech_family = RADIO_TECH_UNKNOWN;

  call_radio_tech = qcril_qmi_voice_convert_call_mode_to_radio_tech_family(call_mode);

  old_call_radio_tech = qcril_qmi_voice_nas_control_get_reported_voice_radio_tech();
  old_call_radio_tech_family = qcril_qmi_convert_radio_tech_to_radio_tech_family(old_call_radio_tech);

  if (
      ( call_radio_tech != old_call_radio_tech_family ) &&
      ( RADIO_TECH_UNKNOWN != old_call_radio_tech )
     )
  {
    res = TRUE;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}/* qcril_qmi_voice_nas_control_is_call_mode_reported_voice_radio_tech_different */


/*=========================================================================
  FUNCTION:  qcril_qmi_voice_convert_call_mode_to_radio_tech_family

===========================================================================*/
/*!
    @brief
    Converts the reported call mode to voice radio tech family


    @return
    Voice radio tech family.
*/
/*=========================================================================*/
unsigned int qcril_qmi_voice_convert_call_mode_to_radio_tech_family(call_mode_enum_v02 call_mode)
{
  unsigned int call_radio_tech = RADIO_TECH_UNKNOWN;

  QCRIL_LOG_INFO("entered call_mode %d", (int) call_mode);

  switch( call_mode )
  {
    case CALL_MODE_CDMA_V02:
      call_radio_tech = RADIO_TECH_3GPP2;
      break;

    case CALL_MODE_GSM_V02:
    case CALL_MODE_UMTS_V02:
    case CALL_MODE_TDS_V02:
    case CALL_MODE_LTE_V02:
    case CALL_MODE_NR5G_V02:
      call_radio_tech = RADIO_TECH_3GPP;
      break;

    default:
      call_radio_tech = RADIO_TECH_UNKNOWN;
      break;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(call_radio_tech);
  return call_radio_tech;
}/* qcril_qmi_voice_convert_call_mode_to_radio_tech_family */

//===========================================================================
//qcril_qmi_voice_nas_control_get_reported_voice_radio_tech
//===========================================================================
RIL_RadioTechnology qcril_qmi_voice_nas_control_get_reported_voice_radio_tech()
{

    QCRIL_LOG_FUNC_ENTRY();

    auto query_tech = qcril_qmi_voice_external_get_voice_radio_tech();

    QCRIL_LOG_INFO("completed with voice_radio_tech %d",
            query_tech.reportedVoiceRadioTech);

    return query_tech.reportedVoiceRadioTech;
} //qcril_qmi_voice_nas_control_get_reported_voice_radio_tech

//===========================================================================
//qcril_qmi_voice_nas_control_process_calls_pending_for_right_voice_rte
//===========================================================================
void qcril_qmi_voice_nas_control_process_calls_pending_for_right_voice_rte(void *)
{
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;
  unsigned int call_radio_tech = RADIO_TECH_UNKNOWN;
  unsigned int call_radio_tech_family = RADIO_TECH_UNKNOWN;
  int res = FALSE;

  QCRIL_LOG_FUNC_ENTRY();
  call_radio_tech = qcril_qmi_voice_nas_control_get_reported_voice_radio_tech();
  call_radio_tech_family = qcril_qmi_convert_radio_tech_to_radio_tech_family(call_radio_tech);

  qcril_qmi_voice_voip_lock_overview();
  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while ( NULL != call_info_entry )
  {
    qcril_qmi_voice_voip_call_info_dump( call_info_entry );
    if( (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE) &&
        (call_radio_tech_family == qcril_qmi_voice_convert_call_mode_to_radio_tech_family(call_info_entry->voice_scv_info.mode)) )
    {
      call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE; //Resetting the WAITING_FOR_MATCHING_VOICE_RTE bit
      res = TRUE;
      QCRIL_LOG_INFO("Resuming android call id %d as call mode matches with the current voice radio tech",call_info_entry->android_call_id);
    }
    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }
  qcril_qmi_voice_voip_unlock_overview();

  if( res )
  {
    qcril_qmi_voice_send_unsol_call_state_changed();
  }

  QCRIL_LOG_FUNC_RETURN();

} //qcril_qmi_voice_nas_control_process_calls_pending_for_right_voice_rte

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_sups_notification_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_SUPS_NOTIFICATION_IND.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_sups_notification_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_sups_notification_ind_msg_v02* sups_notify_ind;
  RIL_SuppSvcNotification response;
  boolean success = FALSE;
  char number[ 2 * QCRIL_QMI_VOICE_CALLED_PARTY_BCD_NO_LENGTH ];
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;
  char ip_hist_info_utf8_str[QCRIL_QMI_VOICE_MAX_IP_HISTORY_INFO_LEN*2];
  int  utf8_len = 0;
  QCRIL_NOTUSED(ind_data_len);

  auto msg = std::make_shared<QcRilUnsolSuppSvcNotificationMessage>();

  if( ind_data_ptr != NULL )
  {
    sups_notify_ind = (voice_sups_notification_ind_msg_v02*)ind_data_ptr;

    memset( ( void* )&response, 0, sizeof( response ) );
    memset( ( void* )&number, 0, sizeof( number ) );
    /* Add event processing here */
    switch(sups_notify_ind->notification_info.notification_type)
    {
      case NOTIFICATION_TYPE_UNCOND_CALL_FORWARD_ACTIVE_V02:
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_UNCOND_FWD_ACTIVE;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_COND_CALL_FORWARD_ACTIVE_V02:
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_COND_FWD_ACTIVE;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_OUTGOING_CALL_IS_FORWARDED_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_CALL_FORWARDED;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_OUTGOING_CALL_IS_WAITING_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_CALL_WAITING;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_OUTGOING_CALLS_BARRED_V02:
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_OUTGOING_CALLS_BARRED;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_CALLS_BARRED_V02:
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_INCOMING_CALLS_BARRED;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_CLIR_SUPPRSESION_REJECTED_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_CLIR_SUPPRESSION_REJ;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_FORWARDED_CALL_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_FORWARDED_CALL;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_OUTGOING_CUG_CALL_V02:
        if(sups_notify_ind->index_valid)
        {
          response.index = sups_notify_ind->index;
          if(msg) {
            msg->setIndex(sups_notify_ind->index);
          }
        }
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_CUG_CALL;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_CUG_CALL_V02:
        if(sups_notify_ind->index_valid)
        {
          response.index = sups_notify_ind->index;
          if(msg) {
            msg->setIndex(sups_notify_ind->index);
          }
        }
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_CUG_CALL;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_CALL_IS_ON_HOLD_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_CALL_HOLD;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_CALL_IS_RETRIEVED_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_CALL_RETRIEVED;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_CALL_IS_FORWARDED_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_ADDITIONAL_INCOM_CALL_FWD;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_CALL_IS_ECT_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        if(sups_notify_ind->ect_number_valid == TRUE)
        {
          if(sups_notify_ind->ect_number.ect_call_state == ECT_CALL_STATE_ALERTING_V02)
          {
            response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_ECT_CALL_REMOTE_PTY_ALERT;
          }
          else if(sups_notify_ind->ect_number.ect_call_state == ECT_CALL_STATE_ACTIVE_V02)
          {
            response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_ECT_CALL_REMOTE_PTY_CONNECTED;
            if(sups_notify_ind->ect_number.number_len != 0 )
            {
              memset(number,0,sizeof(number));
              if( sups_notify_ind->ect_number.number_len < sizeof(number) )
              {
                memcpy(number,sups_notify_ind->ect_number.number,sups_notify_ind->ect_number.number_len);
              }
              else
              {
                memcpy(number,sups_notify_ind->ect_number.number,(sizeof(number)-1));
              }

              /* Set Type based on '+' prefix */
              response.type = ( QCRIL_QMI_VOICE_SS_TA_INTER_PREFIX == number[ 0 ] ) ? QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL : QCRIL_QMI_VOICE_SS_TA_UNKNOWN;
              /* number[0] contains type and the rest contains the number in ASCII */
              if ( response.type == QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL )
              {
                response.number = (char * )&number[ 1 ];
              }
              else
              {
                response.number = (char * )&number[ 0 ];
              }
              if (msg) {
                msg->setType(response.type);
              }
              QCRIL_LOG_DEBUG( "ECT notification has number = %s",response.number);
            }
          }
          else
          {
            QCRIL_LOG_DEBUG( "Invalid ECT notification call state  = %d",sups_notify_ind->ect_number.ect_call_state);
          }

        }
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_OUTGOING_CALL_IS_DEFLECTED_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MO_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSI_CALL_DEFLECTED;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_INCOMING_DEFLECTED_CALL_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_DEFLECTED_CALL;
        success = TRUE;
        break;
      case NOTIFICATION_TYPE_CALL_IS_IN_MPTY_V02:
        response.number = NULL;
        response.notificationType = (int) QCRIL_QMI_VOICE_SS_MT_NOTIFICATION;
        response.code = (int) QCRIL_QMI_VOICE_SS_CSSU_MPTY_CALL;
        success = TRUE;
        break;
      default :
        QCRIL_LOG_ERROR( "Invalid sups notification type recieved = %d",sups_notify_ind->notification_info.notification_type);
        break;
    }

    if ( success )
    {
      QCRIL_LOG_DEBUG( "QCRIL_EVT_CM_CALL_ORIG_FWD_STATUS notification type %d, response code %d",
                       response.notificationType, response.code);

      /* Call related notifications to RIL */
      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id( sups_notify_ind->notification_info.call_id );
      if (msg != nullptr)
      {
        if (qcril_qmi_voice_call_to_ims(call_info_entry)) {
          msg->setIsIms(true);
        }

        if (response.notificationType == 0 /*QCRIL_QMI_VOICE_SS_MT_NOTIFICATION*/) {
          msg->setNotificationType(qcril::interfaces::NotificationType::MO);
        } else if (response.notificationType == 1 /*QCRIL_QMI_VOICE_SS_MT_NOTIFICATION*/) {
          msg->setNotificationType(qcril::interfaces::NotificationType::MT);
        }
        msg->setCode(response.code);
        if (response.number != NULL) {
          msg->setNumber(response.number);
        }


        if (call_info_entry)
        {
          msg->setConnectionId(call_info_entry->android_call_id);
        }

        if(sups_notify_ind->ip_forward_hist_info_valid)
        {
          //Convert UTF16 to UTF8 string
          utf8_len = qcril_cm_ss_convert_ucs2_to_utf8(
              (char *) sups_notify_ind->ip_forward_hist_info,
              sups_notify_ind->ip_forward_hist_info_len * 2,
              ip_hist_info_utf8_str,
              sizeof(ip_hist_info_utf8_str) );

          QCRIL_LOG_DEBUG ("ip_hist_info (utf8 data) : %s", ip_hist_info_utf8_str);
          if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_IP_HISTORY_INFO_LEN * 2 ) )
          {
            QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_IP_HISTORY_INFO_LEN");
            utf8_len = (int) (QCRIL_QMI_VOICE_MAX_IP_HISTORY_INFO_LEN*2);
            ip_hist_info_utf8_str[ utf8_len - 1] = '\0';
          }
          msg->setHistoryInfo(ip_hist_info_utf8_str);
        }

        if (response.code ==  QCRIL_QMI_VOICE_SS_CSSU_CALL_HOLD)
        {
          QCRIL_LOG_DEBUG ("media_direction_hold_valid = %d, media_direction_hold = 0x%x",
              sups_notify_ind->media_direction_hold_valid,
              sups_notify_ind->media_direction_hold);

          if(sups_notify_ind->media_direction_hold_valid)
          {
            if (sups_notify_ind->media_direction_hold == 0)  // media direction NONE
            {
              msg->setHoldTone(true);
            }
            else
            {
              msg->setHoldTone(false);
            }
          }
        }
        Dispatcher::getInstance().dispatchSync(msg);
      }
    }
  }

}/* qcril_qmi_voice_sups_notification_ind_hdlr */

//===========================================================================
// qcril_qmi_voice_call_num_copy_with_toa_check
//===========================================================================
uint32_t qcril_qmi_voice_call_num_copy_with_toa_check(char *src, uint32_t src_size, char* dest,
                                                      uint32_t dest_buffer_size, voice_num_type_enum_v02 num_type)
{
  uint32_t ret_size = 0;
  int offset = 0;

  if ( NULL == src || NULL == dest || src_size + 1 >= dest_buffer_size )
  {
    QCRIL_LOG_ERROR("function paramenter incorrect");
  }
  else
  {
    if ( QMI_VOICE_NUM_TYPE_INTERNATIONAL_V02 != num_type )
    {
      ret_size = src_size;
      memcpy(dest, src, src_size);
    }
    else
    {
      if ( QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX == src[0] )
      {
        ret_size = src_size;
        memcpy(dest, src, src_size);
      }
      else
      {
        if (src_size > 1 && src[0] == '0' && src[1] == '0')
        {
          QCRIL_LOG_INFO("Removing 00 prefix");
          offset = 2;
          src_size-=2; //src_size can not turn negative since we have already checked "if (src_size > 1" above
        }
        ret_size = src_size + 1;
        dest[0] = QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX;
        memcpy(dest+1, src + offset, src_size);
      }
    }
    dest [ret_size] = 0;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET( ret_size );

  return ret_size;
} // qcril_qmi_voice_call_num_copy_with_toa_check

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_info_rec_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_VOICE_INFO_REC_IND.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_info_rec_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  RIL_CDMA_CallWaiting            call_waiting;
  RIL_CDMA_InformationRecords     info_recs;
  RIL_CDMA_SignalInfoRecord       signal_info_rec;
  voice_info_rec_ind_msg_v02 *    info_rec_ind;
  unsigned int                    idx = 0;
  uint8_t                         display_tag;
  uint8_t                         display_len;
  boolean                         call_is_in_call_waiting_state = FALSE;
  boolean                         ignore_caller_name = FALSE;
  unsigned int                    buf_len = 0;
  struct
  {
    char buf[QMI_VOICE_CALLER_ID_MAX_V02+1];
    char pi;
    int  number_type;
    int  number_plan;
  } number;
  char                            name[QMI_VOICE_CALLER_NAME_MAX_V02+1];
  boolean                         name_changed = FALSE,
                                  number_changed = FALSE;
  unsigned int                    iter_idx;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;

  int                             need_reset_1x_num_wait_tmr;
  static const unsigned int max_ril_cdma_info_recs = sizeof(RIL_CDMA_InformationRecords::infoRec) / sizeof(RIL_CDMA_InformationRecords::infoRec[0]);

  QCRIL_NOTUSED(ind_data_len);

  QCRIL_LOG_FUNC_ENTRY();

  need_reset_1x_num_wait_tmr = FALSE;

  if( ind_data_ptr != NULL )
  {
    info_rec_ind = (voice_info_rec_ind_msg_v02*)ind_data_ptr;
    memset( &info_recs, 0, sizeof( info_recs ) );
    memset( &number, 0, sizeof(number) );
    memset( &name, 0, sizeof(name) );
    memset( &signal_info_rec, 0, sizeof(signal_info_rec) );


    QCRIL_LOG_INFO(".. call_waiting_valid, state %d, %d", (int)info_rec_ind->call_waiting_valid, (int)info_rec_ind->call_waiting );
    if (info_rec_ind->call_waiting_valid)
    {
      call_is_in_call_waiting_state = (info_rec_ind->call_waiting == CALL_WAITING_NEW_CALL_V02) ?
                                       TRUE: FALSE;
    }
    QCRIL_LOG_INFO(".. call in waiting state org %d", (int)call_is_in_call_waiting_state );

    QCRIL_LOG_INFO(".. caller_id_info_valid %d", (int)info_rec_ind->caller_id_info_valid );
    QCRIL_LOG_INFO(".. calling_party_info_valid %d", (int)info_rec_ind->calling_party_info_valid );
    if( info_rec_ind->caller_id_info_valid && info_rec_ind->calling_party_info_valid )
    {
      QCRIL_LOG_INFO("caller_id caller_id_len: %d, pi: %d", info_rec_ind->caller_id_info.caller_id_len, info_rec_ind->caller_id_info.pi);
      QCRIL_LOG_INFO("calling_party_info num_type: %d, num_plan: %d, si: %d",
                     info_rec_ind->calling_party_info.num_type, info_rec_ind->calling_party_info.num_plan,
                     info_rec_ind->calling_party_info.si);
      if (idx < max_ril_cdma_info_recs)
      {
        info_recs.infoRec[idx].name = RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC;

        info_recs.infoRec[idx].rec.number.len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->caller_id_info.caller_id,
                                                                                             info_rec_ind->caller_id_info.caller_id_len,
                                                                                             info_recs.infoRec[idx].rec.number.buf,
                                                                                             sizeof(info_recs.infoRec[idx].rec.number.buf),
                                                                                             info_rec_ind->calling_party_info.num_type);

        info_recs.infoRec[idx].rec.number.pi = (char)info_rec_ind->caller_id_info.pi;
        info_recs.infoRec[idx].rec.number.number_type = info_rec_ind->calling_party_info.num_type;
        info_recs.infoRec[idx].rec.number.number_plan = info_rec_ind->calling_party_info.num_plan;
        info_recs.infoRec[idx].rec.number.si = info_rec_ind->calling_party_info.si;
        idx++;
      }

      qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->caller_id_info.caller_id,
                                                   info_rec_ind->caller_id_info.caller_id_len,
                                                   number.buf,
                                                   sizeof(number.buf),
                                                   info_rec_ind->calling_party_info.num_type);
      number.pi = (char)info_rec_ind->caller_id_info.pi;
      number.number_type = info_rec_ind->calling_party_info.num_type;
      number.number_plan = info_rec_ind->calling_party_info.num_plan;
      number_changed = TRUE;
    }
    else if ( info_rec_ind->caller_id_info_valid && !info_rec_ind->calling_party_info_valid )
    {
      QCRIL_LOG_INFO("caller_id caller_id_len: %d, pi: %d", info_rec_ind->caller_id_info.caller_id_len, info_rec_ind->caller_id_info.pi);
      if (idx < max_ril_cdma_info_recs)
      {
        info_recs.infoRec[idx].name = RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC;
        info_recs.infoRec[idx].rec.number.len = std::min(sizeof(info_recs.infoRec[idx].rec.number.buf),
                                                         (size_t)info_rec_ind->caller_id_info.caller_id_len);
        memcpy(info_recs.infoRec[idx].rec.number.buf,
               info_rec_ind->caller_id_info.caller_id,
               info_recs.infoRec[idx].rec.number.len);
        info_recs.infoRec[idx].rec.number.pi = (char)info_rec_ind->caller_id_info.pi;
        info_recs.infoRec[idx].rec.number.number_type = QMI_VOICE_NUM_TYPE_UNKNOWN_V02;
        info_recs.infoRec[idx].rec.number.number_plan = QMI_VOICE_NUM_PLAN_UNKNOWN_V02;
        info_recs.infoRec[idx].rec.number.si = QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED_V02;
        idx++;
      }

      buf_len = std::min(sizeof(number.buf) - 1, (size_t)info_rec_ind->caller_id_info.caller_id_len);
      memcpy(number.buf, info_rec_ind->caller_id_info.caller_id, buf_len);
      number.buf[buf_len] = '\0';

      number.pi = (char)info_rec_ind->caller_id_info.pi;
      number.number_type = QMI_VOICE_NUM_TYPE_UNKNOWN_V02;
      number.number_plan = QMI_VOICE_NUM_PLAN_UNKNOWN_V02;
      number_changed = TRUE;
    }
    else if ( info_rec_ind->calling_party_info_valid )
    {
      QCRIL_LOG_INFO("calling_party_info num_len: %d, pi: %d, num_type: %d, num_plan: %d, si: %d",
                     info_rec_ind->calling_party_info.num_len, info_rec_ind->calling_party_info.pi,
                     info_rec_ind->calling_party_info.num_type, info_rec_ind->calling_party_info.num_plan,
                     info_rec_ind->calling_party_info.si);

      if (idx < max_ril_cdma_info_recs)
      {
        info_recs.infoRec[idx].name = RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC;
        info_recs.infoRec[idx].rec.number.len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->calling_party_info.num,
                                                                                             info_rec_ind->calling_party_info.num_len,
                                                                                             info_recs.infoRec[idx].rec.number.buf,
                                                                                             sizeof(info_recs.infoRec[idx].rec.number.buf),
                                                                                             info_rec_ind->calling_party_info.num_type);
        info_recs.infoRec[idx].rec.number.pi = (char)info_rec_ind->calling_party_info.pi;
        info_recs.infoRec[idx].rec.number.number_type = info_rec_ind->calling_party_info.num_type;
        info_recs.infoRec[idx].rec.number.number_plan = info_rec_ind->calling_party_info.num_plan;
        info_recs.infoRec[idx].rec.number.si = info_rec_ind->calling_party_info.si;
        idx++;
      }

      qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->calling_party_info.num,
                                                   info_rec_ind->calling_party_info.num_len,
                                                   number.buf,
                                                   sizeof(number.buf),
                                                   info_rec_ind->calling_party_info.num_type);
      number.pi = (char)info_rec_ind->calling_party_info.pi;
      number.number_type = info_rec_ind->calling_party_info.num_type;
      number.number_plan = info_rec_ind->calling_party_info.num_plan;
      number_changed = TRUE;
    }

    QCRIL_LOG_INFO(".. display_buffer_valid %d", (int)info_rec_ind->display_buffer_valid );
    if ( info_rec_ind->display_buffer_valid && idx < max_ril_cdma_info_recs )
    {
      info_recs.infoRec[idx].name = RIL_CDMA_DISPLAY_INFO_REC;
      info_recs.infoRec[idx].rec.display.alpha_len = std::min(sizeof(info_recs.infoRec[idx].rec.display.alpha_buf),
                                                              (size_t)strlen(info_rec_ind->display_buffer));
      QCRIL_LOG_INFO("display_buffer length: %d", info_recs.infoRec[idx].rec.display.alpha_len);
      memcpy(info_recs.infoRec[idx].rec.display.alpha_buf,
             info_rec_ind->display_buffer,
             info_recs.infoRec[idx].rec.display.alpha_len);
      idx++;
    }

    QCRIL_LOG_INFO(".. ext_display_record_valid %d", (int)info_rec_ind->ext_display_record_valid );
    QCRIL_LOG_INFO(".. ext_display_buffer_valid %d", (int)info_rec_ind->ext_display_buffer_valid );
    if ( info_rec_ind->ext_display_record_valid )
    {
      display_tag = info_rec_ind->ext_display_record.ext_display_info[0];
      display_len = info_rec_ind->ext_display_record.ext_display_info[1];
      bool is_0x9e_not_treat_as_name = getVoiceModuleFeature(VoiceFeatureType::IS_0x9E_NOT_TREAT_AS_NAME);
      QCRIL_LOG_INFO("ext_display_record display_type: %d, ext_display_info_len: %d, is_0x9e_not_treat_as_name: %d",
                     display_tag, display_len, is_0x9e_not_treat_as_name);
      if ( display_tag == 0x8D || display_tag == 0x8F || (display_tag == 0x9E && !is_0x9e_not_treat_as_name))
      {
        memcpy(name, info_rec_ind->ext_display_record.ext_display_info+2, display_len);
        name_changed = TRUE;
      }
      else
      {
        if (idx < max_ril_cdma_info_recs)
        {
          info_recs.infoRec[idx].name = RIL_CDMA_EXTENDED_DISPLAY_INFO_REC;
          info_recs.infoRec[idx].rec.display.alpha_len = std::min(sizeof(info_recs.infoRec[idx].rec.display.alpha_buf),
              (size_t)info_rec_ind->ext_display_record.ext_display_info_len);
          memcpy(info_recs.infoRec[idx].rec.display.alpha_buf,
                 info_rec_ind->ext_display_record.ext_display_info,
                 info_recs.infoRec[idx].rec.display.alpha_len);
          idx++;
        }
        if ( display_tag == 0x9E && is_0x9e_not_treat_as_name )
        {
          ignore_caller_name = TRUE;
        }
      }
    }
    else if(info_rec_ind->ext_display_buffer_valid && idx < max_ril_cdma_info_recs)
    {
      info_recs.infoRec[idx].name = RIL_CDMA_EXTENDED_DISPLAY_INFO_REC;
      info_recs.infoRec[idx].rec.display.alpha_len = std::min(sizeof(info_recs.infoRec[idx].rec.display.alpha_buf),
          (size_t)strlen(info_rec_ind->ext_display_buffer));
      QCRIL_LOG_INFO("ext_display_buffer length: %d", info_recs.infoRec[idx].rec.display.alpha_len);
      memcpy(info_recs.infoRec[idx].rec.display.alpha_buf,
             info_rec_ind->ext_display_buffer,
             info_recs.infoRec[idx].rec.display.alpha_len);
      idx++;
    }

    QCRIL_LOG_INFO(".. caller_name_valid %d", (int)info_rec_ind->caller_name_valid );
    QCRIL_LOG_INFO(".. name_changed %d", (int)name_changed);
    if(info_rec_ind->caller_name_valid && !name_changed && !ignore_caller_name)
    {
      buf_len = strlen(info_rec_ind->ext_display_buffer);
      QCRIL_LOG_INFO("caller_name length: %d", buf_len);
      memcpy(name, info_rec_ind->caller_name, buf_len);
      name_changed = TRUE;
    }

    QCRIL_LOG_INFO(".. audio_control_valid %d", (int)info_rec_ind->audio_control_valid );
    if(info_rec_ind->audio_control_valid && idx < max_ril_cdma_info_recs)
    {
      QCRIL_LOG_INFO("audio control downlink: %d, uplink: %d",
                     info_rec_ind->audio_control.down_link, info_rec_ind->audio_control.up_link);
      info_recs.infoRec[idx].name = RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC;
      info_recs.infoRec[idx].rec.audioCtrl.downLink = info_rec_ind->audio_control.down_link;
      info_recs.infoRec[idx].rec.audioCtrl.upLink = info_rec_ind->audio_control.up_link;
      idx++;
    }

    QCRIL_LOG_INFO(".. clir_cause_valid %d", (int)info_rec_ind->clir_cause_valid );
    if (info_rec_ind->clir_cause_valid)
    {
      if (idx < max_ril_cdma_info_recs)
      {
        QCRIL_LOG_INFO("clir_cause: %d", info_rec_ind->clir_cause);
        info_recs.infoRec[idx].name = RIL_CDMA_T53_CLIR_INFO_REC;
        info_recs.infoRec[idx].rec.clir.cause = (char)info_rec_ind->clir_cause;
        idx++;
      }
      number_changed = TRUE;
    }

    QCRIL_LOG_INFO(".. nss_release_valid %d", (int)info_rec_ind->nss_release_valid);
    if (info_rec_ind->nss_release_valid)
    {
      auto msg = std::make_shared<QcRilUnsolNssReleaseMessage>(info_rec_ind->call_id,
                                                               info_rec_ind->nss_release);
      if (msg != nullptr)
      {
        Dispatcher::getInstance().dispatchSync(msg);
      }

      if (idx < max_ril_cdma_info_recs)
      {
        info_recs.infoRec[idx].name = RIL_CDMA_T53_RELEASE_INFO_REC;
        idx++;
      }
    }

    QCRIL_LOG_INFO(".. redirecting_num_info_valid %d", (int)info_rec_ind->redirecting_num_info_valid);
    if (info_rec_ind->redirecting_num_info_valid && idx < max_ril_cdma_info_recs)
    {
      QCRIL_LOG_INFO("redirecting reason: %d, pi: %d, si: %d, num_plan: %d, num_type: %d, len: %d",
                     info_rec_ind->redirecting_num_info.reason, info_rec_ind->redirecting_num_info.pi,
                     info_rec_ind->redirecting_num_info.si, info_rec_ind->redirecting_num_info.num_plan,
                     info_rec_ind->redirecting_num_info.num_type, info_rec_ind->redirecting_num_info.num_len);

      info_recs.infoRec[idx].name = RIL_CDMA_REDIRECTING_NUMBER_INFO_REC;
      info_recs.infoRec[idx].rec.redir.redirectingReason = (RIL_CDMA_RedirectingReason)info_rec_ind->redirecting_num_info.reason;
      info_recs.infoRec[idx].rec.redir.redirectingNumber.pi = info_rec_ind->redirecting_num_info.pi;
      info_recs.infoRec[idx].rec.redir.redirectingNumber.si = info_rec_ind->redirecting_num_info.si;
      info_recs.infoRec[idx].rec.redir.redirectingNumber.number_plan = info_rec_ind->redirecting_num_info.num_plan;
      info_recs.infoRec[idx].rec.redir.redirectingNumber.number_type = info_rec_ind->redirecting_num_info.num_type;
      info_recs.infoRec[idx].rec.redir.redirectingNumber.len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->redirecting_num_info.num,
                                                                                                            info_rec_ind->redirecting_num_info.num_len,
                                                                                                            info_recs.infoRec[idx].rec.redir.redirectingNumber.buf,
                                                                                                            sizeof(info_recs.infoRec[idx].rec.redir.redirectingNumber.buf),
                                                                                                            info_rec_ind->redirecting_num_info.num_type);
      idx++;
    }

    QCRIL_LOG_INFO(".. line_control_valid %d", (int)info_rec_ind->line_control_valid);
    if ( info_rec_ind->line_control_valid && idx < max_ril_cdma_info_recs)
    {
      info_recs.infoRec[idx].name = RIL_CDMA_LINE_CONTROL_INFO_REC;
      info_recs.infoRec[idx].rec.lineCtrl.lineCtrlPolarityIncluded = (char) info_rec_ind->line_control.polarity_included;
      info_recs.infoRec[idx].rec.lineCtrl.lineCtrlToggle = info_rec_ind->line_control.toggle_mode;
      info_recs.infoRec[idx].rec.lineCtrl.lineCtrlReverse = info_rec_ind->line_control.reverse_polarity;
      info_recs.infoRec[idx].rec.lineCtrl.lineCtrlPowerDenial = info_rec_ind->line_control.power_denial_time;
      idx++;
    }

    QCRIL_LOG_INFO(".. conn_num_info_valid %d", (int)info_rec_ind->conn_num_info_valid);
    if (info_rec_ind->conn_num_info_valid && idx < max_ril_cdma_info_recs)
    {
      info_recs.infoRec[idx].name = RIL_CDMA_CONNECTED_NUMBER_INFO_REC;
      info_recs.infoRec[idx].rec.number.number_plan = info_rec_ind->conn_num_info.num_plan;
      info_recs.infoRec[idx].rec.number.number_type = info_rec_ind->conn_num_info.num_type;
      info_recs.infoRec[idx].rec.number.pi = info_rec_ind->conn_num_info.pi;
      info_recs.infoRec[idx].rec.number.si = info_rec_ind->conn_num_info.si;
      info_recs.infoRec[idx].rec.number.len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->conn_num_info.num,
                                                                                           info_rec_ind->conn_num_info.num_len,
                                                                                           info_recs.infoRec[idx].rec.number.buf,
                                                                                           sizeof(info_recs.infoRec[idx].rec.number.buf),
                                                                                           info_rec_ind->conn_num_info.num_type);
      idx++;
    }

    QCRIL_LOG_INFO(".. called_party_info_valid %d", (int)info_rec_ind->called_party_info_valid);
    if (info_rec_ind->called_party_info_valid && idx < max_ril_cdma_info_recs)
    {
      info_recs.infoRec[idx].name = RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC;
      info_recs.infoRec[idx].rec.number.number_plan = info_rec_ind->called_party_info.num_plan;
      info_recs.infoRec[idx].rec.number.number_type = info_rec_ind->called_party_info.num_type;
      info_recs.infoRec[idx].rec.number.pi = info_rec_ind->called_party_info.pi;
      info_recs.infoRec[idx].rec.number.si = info_rec_ind->called_party_info.si;
      info_recs.infoRec[idx].rec.number.len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->called_party_info.num,
                                                                                           info_rec_ind->called_party_info.num_len,
                                                                                           info_recs.infoRec[idx].rec.number.buf,
                                                                                           sizeof(info_recs.infoRec[idx].rec.number.buf),
                                                                                           info_rec_ind->called_party_info.num_type);
      idx++;
    }

    QCRIL_LOG_INFO(".. signal_info_valid %d", (int)info_rec_ind->signal_info_valid );
    if( info_rec_ind->signal_info_valid )
    {
      signal_info_rec.isPresent = TRUE;
      signal_info_rec.signalType = (char)info_rec_ind->signal_info.signal_type;
      signal_info_rec.alertPitch = (char)info_rec_ind->signal_info.alert_pitch;
      signal_info_rec.signal = (char)info_rec_ind->signal_info.signal;
    }

    QCRIL_LOG_INFO(".. is waiting state %d, name changed %d, number changed %d  ",
        (int)call_is_in_call_waiting_state, (int)name_changed, (int)number_changed );

    if( call_is_in_call_waiting_state )
    {
      /* Fill in the Call Waiting information */
      call_waiting.number = number.buf;
      call_waiting.numberPresentation = number.pi;
      call_waiting.name = name;
      call_waiting.signalInfoRecord = signal_info_rec;
      call_waiting.number_type = number.number_type;
      call_waiting.number_plan = number.number_plan;

      QCRIL_LOG_INFO( "call is in waiting state. Number : %s; Number Presentation: %d; "
                      "Name : %s; Signal Info Rec.isPresent : %d; "
                      "Number type : %d; Number plan : %d;",
                      call_waiting.number,
                      call_waiting.numberPresentation,
                      call_waiting.name,
                      call_waiting.signalInfoRecord.isPresent,
                      call_waiting.number_type,
                      call_waiting.number_plan
                    );

      auto msg = std::make_shared<QcRilUnsolCdmaCallWaitingMessage>();
      if (msg != nullptr)
      {
        if (call_waiting.number)
        {
          msg->setNumber(call_waiting.number);
        }
        msg->setNumberPresentation(call_waiting.numberPresentation);
        if (call_waiting.name)
        {
          msg->setName(call_waiting.name);
        }
        auto sigInfoRec = std::make_shared<RIL_CDMA_SignalInfoRecord>();
        if (sigInfoRec)
        {
          sigInfoRec->isPresent = call_waiting.signalInfoRecord.isPresent;
          sigInfoRec->signalType = call_waiting.signalInfoRecord.signalType;
          sigInfoRec->alertPitch = call_waiting.signalInfoRecord.alertPitch;
          sigInfoRec->signal = call_waiting.signalInfoRecord.signal;
        }
        msg->setSignalInfoRecord(sigInfoRec);
        msg->setNumberType(call_waiting.number_type);
        msg->setNumberPlan(call_waiting.number_plan);
        Dispatcher::getInstance().dispatchSync(msg);
      }
      if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_KDDI_HOLD_ANSWER))
      {
        auto hdlrMsg = std::make_shared<FlashActivateHoldAnswerMessage>(info_rec_ind->call_id);
        hdlrMsg->dispatch();
      }
    }
    else
    {
      qcril_qmi_voice_voip_lock_overview();

      if ( info_rec_ind->caller_id_info_valid || info_rec_ind->caller_name_valid || info_rec_ind->calling_party_info_valid || info_rec_ind->clir_cause_valid)
      {
        //  workaround for unknown incoming call number in 3gpp2
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id( info_rec_ind->call_id );
        if ( NULL == call_info_entry && info_rec_ind->call_id != 254 )
        { // fresh, this should be an incoming call and the info_rec_ind comes before all_call_status_ind
          // Create a call entry only if call_id is not 254 which is used by modem to indicate a unknown call type.
          call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
                  info_rec_ind->call_id,
                  INVALID_MEDIA_ID,
                  TRUE,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING,
                  NULL );
          if(call_info_entry)
          {
            call_info_entry->ril_call_state = RIL_CALL_INCOMING;
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RIL_CALL_STATE_VALID;
          }
        }
        else
        { // what we got?
          qcril_qmi_voice_voip_call_info_dump( call_info_entry );
        }

        if ( call_info_entry )
        {
          if ( info_rec_ind->caller_id_info_valid )
          {
            call_info_entry->voice_svc_remote_party_number.call_id = info_rec_ind->call_id;
            call_info_entry->voice_svc_remote_party_number.number_pi = info_rec_ind->caller_id_info.pi;

            QCRIL_LOG_INFO(".. store caller id in call obj" );

            uint32_t number_len = qcril_qmi_voice_call_num_copy_with_toa_check(info_rec_ind->caller_id_info.caller_id,
                                                                               info_rec_ind->caller_id_info.caller_id_len,
                                                                               call_info_entry->voice_svc_remote_party_number.number,
                                                                               QMI_VOICE_NUMBER_MAX_V02,
                                                                               (info_rec_ind->calling_party_info_valid) ? info_rec_ind->calling_party_info.num_type : QMI_VOICE_NUM_TYPE_UNKNOWN_V02);
            call_info_entry->voice_svc_remote_party_number.number_len = number_len;

            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;

            qcril_qmi_voice_consider_shadow_remote_number_cpy_creation( call_info_entry );

            if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING )
            {
              call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;
              need_reset_1x_num_wait_tmr = TRUE;
            }

          }
          else if ( info_rec_ind->calling_party_info_valid )
          {
            call_info_entry->voice_svc_remote_party_number.call_id = info_rec_ind->call_id;
            call_info_entry->voice_svc_remote_party_number.number_pi = info_rec_ind->calling_party_info.pi;

            QCRIL_LOG_INFO(".. store calling_party_info in call obj" );

            uint32_t number_len = qcril_qmi_voice_call_num_copy_with_toa_check(
                                  info_rec_ind->calling_party_info.num,
                                  info_rec_ind->calling_party_info.num_len,
                                  call_info_entry->voice_svc_remote_party_number.number,
                                  QMI_VOICE_NUMBER_MAX_V02,
                                  info_rec_ind->calling_party_info.num_type);
            call_info_entry->voice_svc_remote_party_number.number_len = number_len;

            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;

            qcril_qmi_voice_consider_shadow_remote_number_cpy_creation (call_info_entry );

            if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING )
            {
              call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;
              need_reset_1x_num_wait_tmr = TRUE;
            }

          }

          if ( info_rec_ind->caller_name_valid && !ignore_caller_name)
          {
            QCRIL_LOG_INFO(".. store caller name in call obj" );

            call_info_entry->voice_svc_remote_party_name.call_id = info_rec_ind->call_id;
            call_info_entry->voice_svc_remote_party_name.name_pi = PRESENTATION_NAME_PRESENTATION_ALLOWED_V02; // set to Allowed Presentation
            // convert ascii to utf8
            call_info_entry->voice_svc_remote_party_name.name_len = qcril_cm_ss_ascii_to_utf8((unsigned char*) info_rec_ind->caller_name,
                                                                                              strlen(info_rec_ind->caller_name),
                                                                                              call_info_entry->voice_svc_remote_party_name.name,
                                                                                              sizeof(call_info_entry->voice_svc_remote_party_name.name));
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NAME_VALID;
          }

          if ( info_rec_ind->clir_cause_valid )
          {
            if((!info_rec_ind->calling_party_info_valid) && (info_rec_ind->clir_cause == 0))
            {
              call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_NUM_UNAVAILABLE_V02; // default value
            }
            else
            {
              switch((uint8_t)info_rec_ind->clir_cause)
              {
                case QMI_VOICE_CLIR_CAUSE_NO_CAUSE_V02:
                  call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_ALLOWED_V02;
                  break;
                case QMI_VOICE_CLIR_CAUSE_REJECTED_BY_USER_V02:
                  call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_RESTRICTED_V02;
                  break;
                case QMI_VOICE_CLIR_CAUSE_COIN_LINE_V02:
                  call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_PAYPHONE_V02;
                  break;
                default:
                  call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_NUM_UNAVAILABLE_V02;
                  break;
               }
              QCRIL_LOG_INFO("Mapped Clir=%d, PI=%d",info_rec_ind->clir_cause,call_info_entry->voice_svc_remote_party_number.number_pi );
            }
            call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;

            qcril_qmi_voice_consider_shadow_remote_number_cpy_creation( call_info_entry );

            if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING )
            {
              call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING;
              need_reset_1x_num_wait_tmr = TRUE;
            }

            QCRIL_LOG_INFO( "clir_cause_valid voice_svc_remote_party_number.number_pi = %d",
                            call_info_entry->voice_svc_remote_party_number.number_pi);
          }

          qcril_qmi_voice_voip_call_info_dump( call_info_entry );
        }
        else
        {
          QCRIL_LOG_ERROR("call_info_entry is NULL");
        }
      }
      else
      {
        QCRIL_LOG_INFO("both caller_id_info and caller_name are not valid");
      }

      qcril_qmi_voice_voip_unlock_overview();
    }

    QCRIL_LOG_INFO(".. signal_info_valid %d", (int)info_rec_ind->signal_info_valid );
    if( info_rec_ind->signal_info_valid )
    {
      QCRIL_LOG_INFO("signal_info signal_type: %d, alert_pitch: %d, signal: %d", info_rec_ind->signal_info.signal_type,
                     info_rec_ind->signal_info.alert_pitch, info_rec_ind->signal_info.signal);

      qcril_qmi_voice_voip_lock_overview();

      if ( call_is_in_call_waiting_state )
      { // there must be call waiting obj already
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state( RIL_CALL_WAITING );
      }
      else
      { // expecting call obj in coming state
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state( RIL_CALL_INCOMING );
      }
      QCRIL_LOG_INFO(".. call obj %p", call_info_entry );
      if ( call_info_entry )
      {
        qcril_qmi_voice_voip_call_info_dump( call_info_entry );
        call_info_entry->elaboration |= ( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING | QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_RING_DONE );

        QCRIL_LOG_INFO(".. launch ringer" );
        auto msg = std::make_shared<VoiceMakeIncomingCallRingMessage>(signal_info_rec);
        if (msg)
        {
          Dispatcher::getInstance().dispatch(msg);
        }
        else
        { // rollback
          call_info_entry->elaboration &= ~( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NEED_FOR_RING_PENDING | QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_RING_DONE );
        }
      }
      else if (idx < max_ril_cdma_info_recs)
      { // neither a waiting call nor an incoming call
        info_recs.infoRec[idx].name = RIL_CDMA_SIGNAL_INFO_REC;
        info_recs.infoRec[idx].rec.signal.isPresent = TRUE;
        info_recs.infoRec[idx].rec.signal.signalType = (char)info_rec_ind->signal_info.signal_type;
        info_recs.infoRec[idx].rec.signal.alertPitch = (char)info_rec_ind->signal_info.alert_pitch;
        info_recs.infoRec[idx].rec.signal.signal = (char)info_rec_ind->signal_info.signal;
        idx++;
      }
      qcril_qmi_voice_voip_unlock_overview();
    }

    info_recs.numberOfInfoRecs = idx;
    QCRIL_LOG_INFO( ".. number of info recs to be sent in event is %d", info_recs.numberOfInfoRecs);
    for (iter_idx = 0; iter_idx < info_recs.numberOfInfoRecs; iter_idx++)
    {
      QCRIL_LOG_INFO( ".. .. rec# %d, name %d ", iter_idx, (int)info_recs.infoRec[ iter_idx ].name );
    }

    if (number_changed || name_changed)
    {
      qcril_qmi_voice_send_unsol_call_state_changed();
    }

    if ( idx > 0 )
    {
      auto msg = std::make_shared<QcRilUnsolCdmaInfoRecordMessage>();
      auto records = std::make_shared<RIL_CDMA_InformationRecords>(info_recs);
      msg->setCdmaInfoRecords(records);
      Dispatcher::getInstance().dispatchSync(msg);
    }

    if ( need_reset_1x_num_wait_tmr )
    {
      qcril_qmi_voice_voip_lock_overview();
      qcril_qmi_voice_cancel_num_1x_wait_timer();
      qcril_qmi_voice_voip_unlock_overview();
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_info_rec_ind_hdlr

//===========================================================================
// qcril_qmi_voice_map_ril_error_to_stk_cc_modification_type
//===========================================================================
qcril_qmi_voice_stk_cc_modification_e_type
qcril_qmi_voice_map_ril_error_to_stk_cc_modification_type(RIL_Errno ril_err)
{
  qcril_qmi_voice_stk_cc_modification_e_type ret = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE;

  switch (ril_err)
  {
    case RIL_E_DIAL_MODIFIED_TO_DIAL:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_TO_DIAL;
      break;
    case RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_TO_DIAL_VIDEO;
      break;
    case RIL_E_DIAL_MODIFIED_TO_USSD:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_TO_USSD;
      break;
    case RIL_E_DIAL_MODIFIED_TO_SS:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_TO_SS;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_VIDEO_TO_DIAL;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_VIDEO_TO_DIAL_VIDEO;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_VIDEO_TO_USSD;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_SS:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_DIAL_VIDEO_TO_SS;
      break;
    case RIL_E_SS_MODIFIED_TO_DIAL:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_SS_TO_DIAL;
      break;
    case RIL_E_SS_MODIFIED_TO_DIAL_VIDEO:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_SS_TO_DIAL_VIDEO;
      break;
    case RIL_E_SS_MODIFIED_TO_USSD:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_SS_TO_USSD;
      break;
    case RIL_E_SS_MODIFIED_TO_SS:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_SS_TO_SS;
      break;
    case RIL_E_USSD_MODIFIED_TO_DIAL:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_USSD_TO_DIAL;
      break;
    case RIL_E_USSD_MODIFIED_TO_DIAL_VIDEO:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_USSD_TO_DIAL_VIDEO;
      break;
    case RIL_E_USSD_MODIFIED_TO_USSD:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_USSD_TO_USSD;
      break;
    case RIL_E_USSD_MODIFIED_TO_SS:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_USSD_TO_SS;
      break;
    default:
      ret  = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE;
      break;
  }
  return ret;
}

//===========================================================================
// qcril_qmi_voice_map_ril_error_to_stk_cc_call_fail_cause
//===========================================================================
qcril::interfaces::CallFailCause
qcril_qmi_voice_map_ril_error_to_stk_cc_call_fail_cause(RIL_Errno ril_err)
{
  qcril::interfaces::CallFailCause ret  = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;

  switch (ril_err)
  {
    case RIL_E_DIAL_MODIFIED_TO_DIAL:
      ret  = qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL;
      break;
    case RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO:
      ret  = qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO;
      break;
    case RIL_E_DIAL_MODIFIED_TO_USSD:
      ret  = qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_USSD;
      break;
    case RIL_E_DIAL_MODIFIED_TO_SS:
      ret  = qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_SS;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL:
      ret  = qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      ret  = qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD:
      ret  = qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD;
      break;
    case RIL_E_DIAL_VIDEO_MODIFIED_TO_SS:
      ret  = qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS;
      break;
    default:
      ret  = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
      break;
  }
  return ret;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_dial_call_resp_hdlr

===========================================================================*/
/*!
    @brief
    Handle DIAL_CALL_RESP.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_dial_call_resp_hdlr
(
 uint16_t req_id,
 voice_dial_call_resp_msg_v02 *dial_call_resp,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01  qmi_error;
  boolean destroy_call_info_entry = FALSE;

  int                            enforcing_stk_cc_emulation = FALSE;
  int                            ims_stk_cc_supported = TRUE;
  voice_alpha_ident_type_v02     emulation_transcoding_step1;
  char                           emulation_transcoding_final_buf[QMI_VOICE_ALPHA_TEXT_MAX_V02 + 2];
  int                            emulation_name_len;
  uint8_t                        original_android_call_id = VOICE_INVALID_CALL_ID;

  qcril::interfaces::CallFailCause last_call_fail_cause =
    qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
  qcril_qmi_voice_stk_cc_modification_e_type stk_cc_modification;

  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;


  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_exec_event;

  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry_final;


  qcril_qmi_voice_voip_lock_overview();

  call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
      QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP, TRUE );

  QCRIL_LOG_INFO( "call_info_entry %p", call_info_entry );

  if (dial_call_resp != NULL && NULL != call_info_entry)
  {
    qmi_result = dial_call_resp->resp.result;
    qmi_error = dial_call_resp->resp.error;

    qmi_ril_err_context_e_type err_ctx_type = QCRIL_QMI_ERR_CTX_DIAL_TXN;
    if (call_info_entry->voice_scv_info.call_type == CALL_TYPE_VT_V02 ||
        call_info_entry->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_VT_V02)
    {
      err_ctx_type = QCRIL_QMI_ERR_CTX_DIAL_VIDEO_TXN;
    }

    original_android_call_id = call_info_entry->android_call_id;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(QMI_NO_ERR,
                                                                        &dial_call_resp->resp,
                                                                        err_ctx_type,
                                                                        dial_call_resp);
    if(qmi_error == QMI_ERR_INVALID_ID_V01 && ril_err == RIL_E_MODEM_ERR)
    {
      //handle specific QMI error
      ril_err = RIL_E_INVALID_CALL_ID;
    }


    QCRIL_LOG_INFO("DIAL CALL RESP : ril_err=%d, dial_call_resp_result=%d,"
                   " dial_call_resp_error=%d, cc_sups_result_valid=%d,"
                   " cc_result_type_valid=%d, cc_result_type=%d ",
                   (int)ril_err,
                   (int)dial_call_resp->resp.result,
                   (int)dial_call_resp->resp.error,
                   (int)dial_call_resp->cc_sups_result_valid,
                   (int)dial_call_resp->cc_result_type_valid,
                   (int)dial_call_resp->cc_result_type);

    switch ( (int)ril_err )
    {
      case RIL_E_SUCCESS:
      case RIL_E_DIAL_MODIFIED_TO_DIAL:
      case RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO:
      case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL:
      case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
        call_info_entry->qmi_call_id = dial_call_resp->call_id;

        if(dial_call_resp->media_id_valid)
        {
          call_info_entry->media_id = dial_call_resp->media_id;
        }
        else
        {
          call_info_entry->media_id = INVALID_MEDIA_ID;
        }
        QCRIL_LOG_INFO( "call qmi id recorded %d", (int)call_info_entry->qmi_call_id );
        break;

      default:
        qcril_qmi_voice_respond_pending_hangup_ril_response(call_info_entry->qmi_call_id);
        destroy_call_info_entry = TRUE;
        break;
    }

    // compatibility checks: what is platform does not support STK CC
    enforcing_stk_cc_emulation = FALSE;
    if (qcril_qmi_voice_call_to_ims (call_info_entry) && !ims_stk_cc_supported)
    { // IMS case
      // IMS currently does not support STK CC, need so see if need to setup overlays for STK CC'ed IMS call
       if ( RIL_E_DIAL_MODIFIED_TO_DIAL == ril_err )
       {
          enforcing_stk_cc_emulation = TRUE;
          qcril_qmi_voice_voip_call_info_set_single_elaboration( call_info_entry,
              QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_STK_CC_EMULATED_OVERLAY, TRUE  );
          QCRIL_LOG_INFO( ".. establishing STK CC overlay for IMS MO call, android call id %d",
                         call_info_entry->android_call_id );

          ril_err                    = RIL_E_SUCCESS; // we pretend the life is good
       }
    } // end IMS case

    if( RIL_E_SUCCESS != ril_err && dial_call_resp->cc_sups_result_valid )
    {
      QCRIL_LOG_INFO("Error Details : cc_sups_result_reason=%d, cc_sups_result_service=%d",
                      dial_call_resp->cc_sups_result.reason,
                      dial_call_resp->cc_sups_result.service_type);

    }
    stk_cc_modification = qcril_qmi_voice_map_ril_error_to_stk_cc_modification_type(ril_err);
    last_call_fail_cause = qcril_qmi_voice_map_ril_error_to_stk_cc_call_fail_cause(ril_err);

    if ( !enforcing_stk_cc_emulation )
    { // default case: STK CC is support by Android framework
       if (last_call_fail_cause != qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED)
       {  // STK CC session started
            qcril_qmi_voice_reset_stk_cc();

            stk_cc_info.modification                      = stk_cc_modification;
            stk_cc_info.is_alpha_relayed                  = FALSE;

            auto pendingMsg = findVoiceModulePendingMessage(req_id);
            if (pendingMsg) {
              auto msg = std::static_pointer_cast<QcRilRequestMessage>(pendingMsg);
              if (msg->isImsRequest()) {
                stk_cc_info.is_ims_request = TRUE;
              }
            }

            if ( dial_call_resp->call_id_valid )
            {
              stk_cc_info.call_id_info      = dial_call_resp->call_id;
            }

            if ( dial_call_resp->cc_sups_result_valid )
            {
              stk_cc_info.ss_ussd_info = dial_call_resp->cc_sups_result;
            }

            if ( dial_call_resp->alpha_ident_valid )
            {
              if ( ALPHA_DCS_UCS2_V02 == dial_call_resp->alpha_ident.alpha_dcs )
              {
                qcril_qmi_voice_transfer_sim_ucs2_alpha_to_std_ucs2_alpha(&dial_call_resp->alpha_ident, &stk_cc_info.alpha_ident);
              }
              else
              {
                stk_cc_info.alpha_ident = dial_call_resp->alpha_ident;
              }
            }
            else
            {
              memset( &stk_cc_info.alpha_ident, 0, sizeof( stk_cc_info.alpha_ident ) );
            }
       }
    } // if (!enforcing_stk_cc_emulation)
    else
    { // enforcing STK CC emulation as STK CC is not supported by Android framework: need to store alpha as overlay name

       memset( emulation_transcoding_final_buf, 0 , sizeof(emulation_transcoding_final_buf) );

       if ( dial_call_resp->alpha_ident_valid )
       {
         // make use after converting to utf8
         // step 1 out of 2: to ucs2
         if ( ALPHA_DCS_UCS2_V02 == dial_call_resp->alpha_ident.alpha_dcs )
         {
            qcril_qmi_voice_transfer_sim_ucs2_alpha_to_std_ucs2_alpha( &dial_call_resp->alpha_ident, &emulation_transcoding_step1 );
         }
         else
         {
            emulation_transcoding_step1 = dial_call_resp->alpha_ident;
         }
         // step 2 out of 2: to utf8
         switch ( emulation_transcoding_step1.alpha_dcs )
         {
            case ALPHA_DCS_GSM_V02:
              if  (stk_cc_info.alpha_ident.alpha_text_len < QMI_VOICE_ALPHA_TEXT_MAX_V02)
              {
                  qcril_cm_ss_convert_gsm8bit_alpha_string_to_utf8( (char*) emulation_transcoding_step1.alpha_text,
                                                                    emulation_transcoding_step1.alpha_text_len,
                                                                    emulation_transcoding_final_buf,
                                                                    sizeof(emulation_transcoding_final_buf));
              }
             break;

            case ALPHA_DCS_UCS2_V02:
               qcril_cm_ss_convert_ucs2_to_utf8( (char *) emulation_transcoding_step1.alpha_text,
                                                          emulation_transcoding_step1.alpha_text_len * 2,
                                                          emulation_transcoding_final_buf,
                                                          sizeof(emulation_transcoding_final_buf));
             break;

            default:
               // nothing
               break;
         } // switch ( emulation_transcoding_step1.alpha_dcs )

         if ( *emulation_transcoding_final_buf )
         {
            if ( NULL != call_info_entry->overlayed_name_storage_for_emulated_stk_cc )
            {
               qcril_free( call_info_entry->overlayed_name_storage_for_emulated_stk_cc );
            }
            emulation_name_len = strlen( emulation_transcoding_final_buf );
            call_info_entry->overlayed_name_storage_for_emulated_stk_cc = (char*)qcril_malloc( emulation_name_len + 1 );
            if ( NULL != call_info_entry->overlayed_name_storage_for_emulated_stk_cc )
            {
               strlcpy( call_info_entry->overlayed_name_storage_for_emulated_stk_cc, emulation_transcoding_final_buf, emulation_name_len + 1 );

               QCRIL_LOG_INFO( ".. established STK CC emulated overlay name %s, android call id %d",
                                    call_info_entry->overlayed_name_storage_for_emulated_stk_cc,
                                    call_info_entry->android_call_id );
            } // if ( NULL != call_info_entry->overlayed_name_storage_for_emulated_stk_cc )
         } // if ( *emulation_transcoding_final_buf )
       } // if ( dial_call_resp->alpha_ident_valid )
    } // else-if: enforcing STK CC emulation as STK CC is not supported by Android framework: need to store alpha as overlay name

    QCRIL_LOG_INFO( "DIAL CALL RESP COMPLETE received with result %d for call id %d", (int)ril_err, (int)dial_call_resp->call_id );

    qcril_qmi_voice_stk_cc_dump();
  }
  else
  {
    ril_err = call_info_entry ? RIL_E_SYSTEM_ERR : RIL_E_INVALID_STATE;
  }

  if ( RIL_E_FDN_CHECK_FAILURE == ril_err )
  {
    qcril_qmi_voice_handle_new_last_call_failure_cause((call_end_reason_enum_v02)0,
        qcril::interfaces::CallFailCause::FDN_BLOCKED,
        qcril_qmi_voice_map_ril_reason_to_str(qcril::interfaces::CallFailCause::FDN_BLOCKED),
        call_info_entry);
  }
  else
  {
    if (last_call_fail_cause == qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED &&
        dial_call_resp && dial_call_resp->end_reason_valid)
    {
      qcril_qmi_voice_handle_new_last_call_failure_cause(dial_call_resp->end_reason,
            FALSE, 0, FALSE, 0, FALSE, NULL, call_info_entry);
    }
    else
    {
      qcril_qmi_voice_handle_new_last_call_failure_cause((call_end_reason_enum_v02)0,
          last_call_fail_cause,
          qcril_qmi_voice_map_ril_reason_to_str(last_call_fail_cause),
          call_info_entry);
    }
  }

  memset( &oversight_exec_event, 0, sizeof( oversight_exec_event ) );
  oversight_exec_event.locator.elaboration_pattern = QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP;

  if ( NULL != call_info_entry )
  { // dial failed or dial modified by STK CC to SS or USSD
    if (RIL_E_SUCCESS != ril_err &&
        (RIL_E_DIAL_MODIFIED_TO_DIAL != ril_err &&
         RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO != ril_err &&
         RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL != ril_err &&
         RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO != ril_err))
     {
       // Reset the NetworkDetectedEccNumberInfo if the call is not ended with
       // TEMP_REDIAL_ALLOWED / PERM_REDIAL_NOT_NEEDED / ALTERNATE_EMERGENCY_CALL
       if (dial_call_resp &&
           (!dial_call_resp->end_reason_valid ||
            is_end_reason_to_reset_network_detected_ecc_info(dial_call_resp->end_reason)))
       {
         reset_network_detected_ecc_if_required();
       }

       if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING)
       {
           call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
       }
       oversight_exec_event.successful_response_payload = ril_err;
       oversight_exec_event.successful_response_payload_len = sizeof(ril_err);
       qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_ABANDON,
                                                         QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN,
                                                         &oversight_exec_event );
       destroy_call_info_entry = FALSE;
     }
     else
     {
       oversight_exec_event.successful_response_payload = ril_err;
       oversight_exec_event.successful_response_payload_len = sizeof(ril_err);
       qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS,
                                                         QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN,
                                                         &oversight_exec_event );
     }
  } // NULL != call_info_entry

  if ( !enforcing_stk_cc_emulation && NULL != dial_call_resp && VOICE_INVALID_CALL_ID != original_android_call_id )
  { // if no STK CC emulation, do cleanup of whatever overlay preserved till now
     call_info_entry_final = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id ( original_android_call_id );
     if ( NULL != call_info_entry_final )
     {
        if ( NULL != call_info_entry_final->overlayed_number_storage_for_emulated_stk_cc )
        {
           qcril_free( call_info_entry_final->overlayed_number_storage_for_emulated_stk_cc );
           call_info_entry_final->overlayed_number_storage_for_emulated_stk_cc = NULL;
        }
        if ( NULL != call_info_entry_final->overlayed_name_storage_for_emulated_stk_cc )
        {
           qcril_free( call_info_entry_final->overlayed_name_storage_for_emulated_stk_cc );
           call_info_entry_final->overlayed_name_storage_for_emulated_stk_cc = NULL;
        }
     }
  } // if ( !enforcing_stk_cc_emulation && NULL != dial_call_resp && VOICE_INVALID_CALL_ID != original_android_call_id )

  // Invoke the callback function
  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, ril_err, dial_call_resp};
    responseCb(&respData);
  }

  if ( destroy_call_info_entry && NULL != dial_call_resp && VOICE_INVALID_CALL_ID != original_android_call_id )
  {
      call_info_entry_final = qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id ( original_android_call_id );
      qcril_qmi_voice_voip_destroy_call_info_entry( call_info_entry_final );
  } // if ( destroy_call_info_entry && NULL != dial_call_resp && VOICE_INVALID_CALL_ID != original_android_call_id )

  qcril_qmi_voice_voip_unlock_overview();

  if (qcril_qmi_voice_voip_call_info_entries_is_empty())
  {
      QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
      if (!qcril_stop_diag_log())
      {
          QCRIL_LOG_INFO("qxdm logging disabled successfully");
      }
  }

  QCRIL_LOG_FUNC_RETURN();

} /* qcril_qmi_voice_dial_call_resp_hdlr */

//===========================================================================
// qcril_qmi_voice_answer_call_resp_hdlr
//===========================================================================
void qcril_qmi_voice_answer_call_resp_hdlr
(
 uint16_t req_id,
 voice_answer_call_resp_msg_v02 *ans_call_resp,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01 qmi_result = QMI_RESULT_FAILURE_V01;
  qmi_error_type_v01 qmi_error = QMI_ERR_INTERNAL_V01;

  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = nullptr;
  int covered_by_oversight_handling;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL != ans_call_resp)
  {
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &ans_call_resp->resp);
    // TODO debug
    // ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( transp_err, &ans_call_resp->resp );
    // QCRIL_LOG_INFO( ".. transp err %d, resp err %d, ril err %d", (int)transp_err, (int)ans_call_resp->resp.error, (int)ril_err );
    QCRIL_LOG_INFO(".. call id valid %d, call id %d", (int)ans_call_resp->call_id_valid,
                   (int)ans_call_resp->call_id);

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING, TRUE);
    QCRIL_LOG_INFO(".. call info entry %p", call_info_entry);

    qmi_result = ans_call_resp->resp.result;
    qmi_error = ans_call_resp->resp.error;
    covered_by_oversight_handling = FALSE;
    if (ril_err == RIL_E_MODEM_ERR && qmi_error == QMI_ERR_INVALID_ID_V01)
    {
      // handle specific error case
      ril_err = RIL_E_INVALID_CALL_ID;
    }
  }
  qcril_qmi_voice_voip_lock_overview();
  command_oversight = qmi_ril_voice_ims_find_command_oversight_by_token(req_id);
  if (NULL != command_oversight)
  {
    memset(&oversight_event_params, 0, sizeof(oversight_event_params));
    oversight_event_params.locator.command_oversight = command_oversight;
    oversight_event_params.successful_response_payload = ril_err;
    oversight_event_params.successful_response_payload_len = sizeof(ril_err);
    covered_by_oversight_handling = qmi_ril_voice_ims_command_oversight_handle_event(
        (QMI_RESULT_SUCCESS_V01 == qmi_result && QMI_ERR_NONE_V01 == qmi_error)
            ? QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS
            : QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_FAILURE,
        QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_event_params);
  }  // if (NULL != command_oversight)
  qcril_qmi_voice_voip_unlock_overview();

  if ((NULL == command_oversight) || (!covered_by_oversight_handling))
  {
    if (NULL != call_info_entry)
    {
      call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL;
      if (RIL_E_SUCCESS == ril_err)
      {
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING;
      }
    }
  }

  // Invoke the callback function
  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, ril_err, ans_call_resp};
    responseCb(&respData);
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_voice_answer_call_resp_hdlr

//===========================================================================
// qcril_qmi_voice_setup_answer_resp_hdlr
//===========================================================================
void qcril_qmi_voice_setup_answer_resp_hdlr
(
 uint16_t req_id,
 voice_setup_answer_resp_msg_v02 *setup_answer_resp_ptr,
 CommonVoiceResponseCallback responseCb
)
{
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = NULL;
  int covered_by_oversight_handling = FALSE;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (setup_answer_resp_ptr)
  {
    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR,
                                                                     &setup_answer_resp_ptr->resp);
    QCRIL_LOG_INFO("QMI result %d, QMI error %d, RIL result %d", setup_answer_resp_ptr->resp.result,
                   setup_answer_resp_ptr->resp.error, result);

    qcril_qmi_voice_voip_lock_overview();
    command_oversight = qmi_ril_voice_ims_find_command_oversight_by_token(req_id);
    if (NULL != command_oversight)
    {
      memset(&oversight_event_params, 0, sizeof(oversight_event_params));
      oversight_event_params.locator.command_oversight = command_oversight;
      covered_by_oversight_handling = qmi_ril_voice_ims_command_oversight_handle_event(
          (RIL_E_SUCCESS == result) ? QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS
                                    : QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_FAILURE,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_event_params);
    }
    qcril_qmi_voice_voip_unlock_overview();
  }

  // Invoke the callback function
  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, result, setup_answer_resp_ptr};
    responseCb(&respData);
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_end_call_resp_hdlr

===========================================================================*/
/*!
    @brief
    Handle END_CALL_RESP.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_end_call_resp_hdlr
(
 uint16_t req_id,
 voice_end_call_resp_msg_v02 *end_call_resp,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_SUCCESS;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01 qmi_error;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = nullptr;
  int covered_by_oversight_handling = FALSE;
  boolean send_response = true;

  QCRIL_LOG_FUNC_ENTRY();

  if (end_call_resp != nullptr) {
    qmi_result = end_call_resp->resp.result;
    qmi_error = end_call_resp->resp.error;

    if (qmi_result == QMI_RESULT_SUCCESS_V01) {
      QCRIL_LOG_INFO("END CALL RESP SUCCESS received with call id %d", end_call_resp->call_id);
      ril_err = RIL_E_SUCCESS;
    } else {
      QCRIL_LOG_INFO("END CALL RESP FAILURE received with error %d", qmi_error);

      ril_err = qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error);
      if(ril_err == RIL_E_MODEM_ERR && qmi_error == QMI_ERR_INVALID_ID_V01)
      {
        //handle specific error case
        ril_err = RIL_E_INVALID_CALL_ID;
      }

      if (qmi_error == QMI_ERR_MALFORMED_MSG_V01)
      {
        auto pendingMsg = findVoiceModulePendingMessage(req_id);
        if (pendingMsg && (pendingMsg->get_message_id() ==
                                 QcRilRequestHangupMessage::get_class_message_id()))
        {
          auto msg(std::static_pointer_cast<QcRilRequestHangupMessage>(pendingMsg));
          if (!msg->isHangupRetryAttempted())
          {
            voice_end_call_req_msg_v02 end_call_req = {};
            if (msg->hasCallIndex()) {
              qcril_qmi_voice_voip_call_info_entry_type *call_info_entry =
                qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id(msg->getCallIndex());
              if (call_info_entry)
              {
                end_call_req.call_id = call_info_entry->qmi_call_id;
                RIL_Errno ret = qcril_qmi_voice_process_end_call_req(
                    req_id, end_call_req, responseCb, nullptr);
                if (ret == RIL_E_SUCCESS)
                {
                  send_response = false;
                }
              }
            }
            msg->setHangupRetryAttempted();
          }
        }
      }
    }
  }
  else
  {
    ril_err = RIL_E_SYSTEM_ERR;
  }

  if (send_response) {
    command_oversight = qmi_ril_voice_ims_find_command_oversight_by_token(req_id);
    if (command_oversight != NULL)
    {
      memset(&oversight_event_params, 0, sizeof(oversight_event_params));
      oversight_event_params.locator.command_oversight = command_oversight;
      oversight_event_params.successful_response_payload = ril_err;
      oversight_event_params.successful_response_payload_len = sizeof(ril_err);
      covered_by_oversight_handling = qmi_ril_voice_ims_command_oversight_handle_event(
          ril_err == RIL_E_SUCCESS ? QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS
          : QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_FAILURE,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_event_params);
    }

    // Invoke the callback function
    if (responseCb)
    {
      CommonVoiceResponseData respData = {req_id, ril_err, end_call_resp};
      responseCb(&respData);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_voice_end_call_resp_hdlr */

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_ussd_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle USSD Indications.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_ussd_ind_hdlr
(
const voice_ussd_ind_msg_v02 &ussd_ind_msg
)
{
  char ussd_utf8_str[QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2];
  qcril::interfaces::UssdModeType type_code = qcril::interfaces::UssdModeType::UNKNOWN;
  int utf8_len = 0;
  uint8_t uss_dcs=QCRIL_QMI_VOICE_USSD_DCS_UNSPECIFIED;
  const voice_ussd_ind_msg_v02 *ussd_ind = &ussd_ind_msg;
  further_user_action_enum_v02 notification_type;
  boolean success=TRUE;
  int i=0;
  std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
  qcril::interfaces::UssdDomain domain = qcril::interfaces::UssdDomain::CS;

  QCRIL_LOG_FUNC_ENTRY();

  if ( ussd_ind )
  {
    notification_type = ussd_ind->notification_type;
    if (ussd_ind->ussd_domain_route_valid) {
      domain = qcril_qmi_voice_map_qmi_to_ril_ussd_domain(ussd_ind->ussd_domain_route);
    }
    memset( ussd_utf8_str, '\0', sizeof( ussd_utf8_str ) );

    switch(notification_type)
    {
      case FURTHER_USER_ACTION_REQUIRED_V02:
        qcril_qmi_voice_info.ussd_user_action_required = TRUE;
        break;
      case FURTHER_USER_ACTION_NOT_REQUIRED_V02 :
        qcril_qmi_voice_info.ussd_user_action_required = FALSE;
        break;
      default :
        break;
    }

    if( TRUE == ussd_ind->uss_info_valid || TRUE == ussd_ind->uss_info_utf16_valid )
    {
      if ( TRUE == ussd_ind->uss_info_utf16_valid ) // using uss_info_utf16 instead of uss_info if it is available
      {
        utf8_len = qcril_cm_ss_convert_ucs2_to_utf8( (char *) ussd_ind->uss_info_utf16, ussd_ind->uss_info_utf16_len * 2, ussd_utf8_str, sizeof(ussd_utf8_str) );

        for(i=0 ; i< utf8_len ; i++ )
        {
          QCRIL_LOG_DEBUG ("utf8 data bytes : %x ", ussd_utf8_str[ i ]);
        }
        if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
        {
          QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
          utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
          ussd_utf8_str[ utf8_len - 1] = '\0';
        }
      }
      else
      {
        switch(ussd_ind->uss_info.uss_dcs)
        {
          case USS_DCS_ASCII_V02 :
            if( ussd_ind->uss_info.uss_data_len < QMI_VOICE_USS_DATA_MAX_V02 )
            {
              qcril_cm_ss_ascii_to_utf8((unsigned char *)ussd_ind->uss_info.uss_data, ussd_ind->uss_info.uss_data_len,
                                      ussd_utf8_str, sizeof(ussd_utf8_str));
            }
            break;
          case USS_DCS_8BIT_V02 :
            uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_8_BIT;
            utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8( uss_dcs,
                                                                ussd_ind->uss_info.uss_data,
                                                                ussd_ind->uss_info.uss_data_len,
                                                                ussd_utf8_str,
                                                                sizeof(ussd_utf8_str));
            if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
            {
              QCRIL_LOG_ERROR ( "ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR" );
              utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
              ussd_utf8_str[ utf8_len - 1] = '\0';
            }
            break;
          case USS_DCS_UCS2_V02 :
            uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_UCS2;
            utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8( uss_dcs,
                                                                ussd_ind->uss_info.uss_data,
                                                                ussd_ind->uss_info.uss_data_len,
                                                                ussd_utf8_str,
                                                                sizeof(ussd_utf8_str));
            if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
            {
              QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
              utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
              ussd_utf8_str[ utf8_len - 1] = '\0';
            }
            break;
          default :
            QCRIL_LOG_ERROR ("Invalid USSD dcs : %d", ussd_ind->uss_info.uss_dcs );
            success = FALSE;
            break;
        }
      }

      if(success == TRUE)
      {
        if(notification_type == FURTHER_USER_ACTION_REQUIRED_V02)
        {
          type_code = qcril::interfaces::UssdModeType::REQUEST;
        }
        else
        {
          type_code = qcril::interfaces::UssdModeType::NOTIFY;
        }
      }
    }
    else
    {
      domain = qcril::interfaces::UssdDomain::UNKNOWN;
      type_code = qcril::interfaces::UssdModeType::NOTIFY;
      ussd_utf8_str[0] = '\0';
      QCRIL_LOG_ERROR("Received USSD Indication with no USSD string");
    }
    if (ussd_ind->sip_error_code_valid ||
        ussd_ind->ussd_error_description_valid)
    {
      errorDetails = buildSipErrorInfo(
          ussd_ind->sip_error_code_valid, ussd_ind->sip_error_code,
          ussd_ind->ussd_error_description_valid
              ? (QMI_VOICE_USS_DATA_MAX_V02 + 1) : 0,
          (uint16_t *)&(ussd_ind->ussd_error_description));
    }
    qcril_qmi_voice_send_unsol_ussd(domain, type_code, ussd_utf8_str, errorDetails);
  }

  QCRIL_LOG_FUNC_RETURN();

}/*qcril_qmi_voice_ussd_ind_hdlr*/

/*=========================================================================
  qcril_qmi_voice_ussd_ind_hdlr_wrapper
===========================================================================*/
void qcril_qmi_voice_ussd_ind_hdlr_wrapper(const voice_ussd_ind_msg_v02 &ussd_ind)
{
  auto power_opt_enabled = qcril_qmi_voice_external_query_is_power_opt_enabled();
  if (power_opt_enabled)
  {
    auto atel_ui_status = qcril_qmi_voice_external_get_ui_status();
    if (atel_ui_status)
    {
      qcril_qmi_voice_ussd_ind_hdlr(ussd_ind);
    }
    else
    {
      qcril_qmi_voice_update_ussd_power_opt_buffer(ussd_ind);
    }
  }
  else
  {
    qcril_qmi_voice_ussd_ind_hdlr(ussd_ind);
  }
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_ussd_release_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handles USSD Release indications

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_ussd_release_ind_hdlr
(
void *ind_data_ptr,
uint32_t ind_data_len
)
{
  voice_ussd_release_ind_msg_v02 *ussd_release_ind;
  qcril::interfaces::UssdDomain domain = qcril::interfaces::UssdDomain::CS;
  QCRIL_NOTUSED(ind_data_len);

  QCRIL_LOG_FUNC_ENTRY();

  if( ind_data_ptr != NULL )
  {
    ussd_release_ind = (voice_ussd_release_ind_msg_v02*)ind_data_ptr;
    if( ussd_release_ind->ussd_domain_route_valid ) {
      domain = qcril_qmi_voice_map_qmi_to_ril_ussd_domain(ussd_release_ind->ussd_domain_route);
    }
  }
  // Return from pending message list
  /* Lookup the hangup request */
  auto pendingMsg = extractVoiceModulePendingMessage(
        QcRilRequestSendUssdMessage::get_class_message_id());
  if (pendingMsg != nullptr)
  {
    QCRIL_LOG_INFO("cleaning the uss_cnf after receiving release_uss_ind");
    /* send RIL_E_GENERIC_FAILURE response */
    auto msg(std::static_pointer_cast<QcRilRequestMessage>(pendingMsg));
    auto respPayload =
      std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_GENERIC_FAILURE, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }

  qcril_qmi_voice_info.ussd_user_action_required = FALSE;

  QCRIL_LOG_DEBUG ("USSD Release triggered, Sending ABORT in case if any pending transaction exists");
  qcril_qmi_voice_send_unsol_ussd(domain, qcril::interfaces::UssdModeType::NOTIFY, "", nullptr);

  qcril_qmi_voice_info.ussd_request_domain = qcril::interfaces::UssdDomain::UNKNOWN;

  QCRIL_LOG_FUNC_RETURN();
}/*qcril_qmi_voice_ussd_release_ind_hdlr*/

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_map_qmi_to_ril_ussd_domain

===========================================================================*/
/*!
    @brief
    Maps qmi USSD domain enum to RIL USSD domain values.

    @return
    RIL USSD domain values.
*/
/*=========================================================================*/
qcril::interfaces::UssdDomain qcril_qmi_voice_map_qmi_to_ril_ussd_domain
(
  sups_domain_enum_v02 qmi_ussd_domain
)
{
  switch(qmi_ussd_domain)
  {
    case SUPS_DOMAIN_CS_V02:
      return qcril::interfaces::UssdDomain::CS;
    case SUPS_DOMAIN_PS_V02:
      return qcril::interfaces::UssdDomain::IMS;
    default:
      return qcril::interfaces::UssdDomain::UNKNOWN;
  }
}/*qcril_qmi_voice_map_qmi_to_ril_ussd_domain*/

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_command_cb

===========================================================================*/
/*!
    @brief
    Common Callback for the QMI voice commands.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_command_cb
(
  unsigned int                 msg_id,
  std::shared_ptr<void>        /*resp_c_struct*/,
  unsigned int                 /*resp_c_struct_len*/,
  void                       * /*resp_cb_data*/,
  qmi_client_error_type        /*transp_err*/
)
{
  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO("msg_id %.2x (%s)", msg_id, qcril_qmi_voice_lookup_command_name(msg_id));

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_voice_command_cb */

qcril::interfaces::DtmfEvent convertDtmfEvent(dtmf_event_enum_v02 in) {
  qcril::interfaces::DtmfEvent ret = qcril::interfaces::DtmfEvent::UNKNOWN;
  switch(in)
  {
    case DTMF_EVENT_FWD_BURST_V02:
      ret = qcril::interfaces::DtmfEvent::FWD_BURST;
      break;
    case DTMF_EVENT_FWD_START_CONT_V02:
      ret = qcril::interfaces::DtmfEvent::FWD_START_CONT;
      break;
    case DTMF_EVENT_FWD_STOP_CONT_V02:
      ret = qcril::interfaces::DtmfEvent::FWD_STOP_CONT;
      break;
    case DTMF_EVENT_IP_INCOMING_DTMF_START_V02:
      ret = qcril::interfaces::DtmfEvent::IP_INCOMING_DTMF_START;
      break;
    case DTMF_EVENT_IP_INCOMING_DTMF_STOP_V02:
      ret = qcril::interfaces::DtmfEvent::IP_INCOMING_DTMF_STOP;
      break;
    default:
      break;
  }
  return ret;
}

qcril::interfaces::DtmfOnLength convertOnLength(dtmf_onlength_enum_v02 in)
{
  qcril::interfaces::DtmfOnLength ret = qcril::interfaces::DtmfOnLength::UNKNOWN;
  switch(in)
  {
    case DTMF_ONLENGTH_95MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_95MS;
      break;
    case DTMF_ONLENGTH_150MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_150MS;
      break;
    case DTMF_ONLENGTH_200MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_200MS;
      break;
    case DTMF_ONLENGTH_250MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_250MS;
      break;
    case DTMF_ONLENGTH_300MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_300MS;
      break;
    case DTMF_ONLENGTH_350MS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_350MS;
      break;
    case DTMF_ONLENGTH_SMS_V02:
      ret = qcril::interfaces::DtmfOnLength::ONLENGTH_SMS;
      break;
    default:
      break;
  }
  return ret;
}

qcril::interfaces::DtmfOffLength convertOffLength(dtmf_offlength_enum_v02 in) {
  qcril::interfaces::DtmfOffLength ret = qcril::interfaces::DtmfOffLength::UNKNOWN;
  switch(in)
  {
    case DTMF_OFFLENGTH_60MS_V02:
      ret = qcril::interfaces::DtmfOffLength::OFFLENGTH_60MS;
      break;
    case DTMF_OFFLENGTH_100MS_V02:
      ret = qcril::interfaces::DtmfOffLength::OFFLENGTH_100MS;
      break;
    case DTMF_OFFLENGTH_150MS_V02:
      ret = qcril::interfaces::DtmfOffLength::OFFLENGTH_150MS;
      break;
    case DTMF_OFFLENGTH_200MS_V02:
      ret = qcril::interfaces::DtmfOffLength::OFFLENGTH_200MS;
      break;
    default:
      break;
  }
  return ret;
}

/*=========================================================================
  FUNCTION: qcril_qmi_voice_dtmf_ind_hdlr

    @brief
    Handle QMI_VOICE_DTMF_IND_V02.

    @return
    None.
=========================================================================*/
void qcril_qmi_voice_dtmf_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t /*ind_data_len*/
)
{
  if (ind_data_ptr != NULL)
  {
    voice_dtmf_ind_msg_v02 *dtmf_ind = (voice_dtmf_ind_msg_v02*) ind_data_ptr;
    qcril::interfaces::DtmfEvent dtmfEvent = convertDtmfEvent(dtmf_ind->dtmf_info.dtmf_event);

    if (dtmfEvent != qcril::interfaces::DtmfEvent::UNKNOWN)
    {
      auto msg = std::make_shared<QcRilUnsolDtmfMessage>(
          dtmf_ind->dtmf_info.call_id,
          dtmfEvent,
          dtmf_ind->dtmf_info.digit_buffer);
      if (msg)
      {
        if (dtmf_ind->on_length_valid)
        {
          msg->setOnLength(convertOnLength(dtmf_ind->on_length));
        }
        if (dtmf_ind->off_length_valid)
        {
          msg->setOffLength(convertOffLength(dtmf_ind->off_length));
        }
        Dispatcher::getInstance().dispatchSync(msg);
      }
    }
  }
}

/*=========================================================================
  FUNCTION: qcril_qmi_voice_ext_brst_intl_ind_hdlr

    @brief
    Handle QMI_VOICE_EXT_BRST_INTL_IND_V02.

    @return
    None.
=========================================================================*/
void qcril_qmi_voice_ext_brst_intl_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t /*ind_data_len*/
)
{
  voice_ext_brst_intl_ind_msg_v02* ext_brst_intl_ind;

  QCRIL_LOG_FUNC_ENTRY();

  if( ind_data_ptr != NULL )
  {
    ext_brst_intl_ind = (voice_ext_brst_intl_ind_msg_v02*) ind_data_ptr;

    auto msg = std::make_shared<QcRilUnsolExtBurstIntlMessage>(
        ext_brst_intl_ind->ext_burst_data.mcc,
        ext_brst_intl_ind->ext_burst_data.db_subtype,
        ext_brst_intl_ind->ext_burst_data.chg_ind,
        ext_brst_intl_ind->ext_burst_data.sub_unit,
        ext_brst_intl_ind->ext_burst_data.unit);
    if (msg)
    {
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_voice_ext_brst_intl_ind_hdlr

//===========================================================================
// qcril_qmi_voice_mark_calls_srvcc_in_progress
//===========================================================================
void qcril_qmi_voice_mark_calls_srvcc_in_progress()
{
    qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

    qcril_qmi_voice_voip_lock_overview();
    iter = qmi_voice_voip_overview.call_info_root;
    while ( iter != NULL  )
    {
        if ( !(iter->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN) )
        {
            iter->srvcc_in_progress = TRUE;
        }
        iter = iter->next;
    }
    qcril_qmi_voice_voip_unlock_overview();
} // qcril_qmi_voice_mark_calls_srvcc_in_progress

//===========================================================================
// qcril_qmi_voice_unmark_calls_srvcc_in_progress
//===========================================================================
void qcril_qmi_voice_unmark_calls_srvcc_in_progress()
{
    qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

    qcril_qmi_voice_voip_lock_overview();
    iter = qmi_voice_voip_overview.call_info_root;
    while ( iter != NULL  )
    {
        iter->srvcc_in_progress = FALSE;
        iter = iter->next;
    }
    qcril_qmi_voice_voip_unlock_overview();
} // qcril_qmi_voice_unmark_calls_srvcc_in_progress

//===========================================================================
// qcril_qmi_voice_reset_all_calls_from_auto_to_cs_domain_elab
//===========================================================================
void qcril_qmi_voice_reset_all_calls_from_auto_to_cs_domain_elab()
{
    qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_voice_voip_lock_overview();
    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
    while ( call_info_entry != NULL  )
    {
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN;
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
        call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
    }
    qcril_qmi_voice_voip_unlock_overview();

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_reset_all_calls_from_auto_to_cs_domain_elab

//===========================================================================
// qcril_qmi_voice_handover_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_handover_info_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_handover_ind_msg_v02   *qmi_handover_ind_msg_ptr;
  RIL_Errno                     ret_val = RIL_E_GENERIC_FAILURE;
  RIL_SrvccState                ril_srvccstate = HANDOVER_STARTED;

  QCRIL_LOG_FUNC_ENTRY();

  if (ind_data_ptr != NULL && ind_data_len != 0)
  {
    qmi_handover_ind_msg_ptr = (voice_handover_ind_msg_v02*) ind_data_ptr;

    QCRIL_LOG_INFO("qmi handover ind ho_type_valid: %d, ho_type: %d",
        qmi_handover_ind_msg_ptr->ho_type_valid, qmi_handover_ind_msg_ptr->ho_type);

    // Need to send the UNSOL_SRVCC_STATE_NOTIFY only
    // in case of handover type is SRVCC L_2_G/L_2_W
    // and DRVCC cases for WIFI_2_C/WIFI_2_GW
    if (qmi_handover_ind_msg_ptr->ho_type_valid)
    {
      switch (qmi_handover_ind_msg_ptr->ho_type)
      {
        case VOICE_HO_SRVCC_L_2_G_V02:
        case VOICE_HO_SRVCC_L_2_W_V02:
        case VOICE_HO_DRVCC_WIFI_2_C_V02:
        case VOICE_HO_DRVCC_WIFI_2_GW_V02:
          ret_val = RIL_E_SUCCESS;
          break;

        default:
          break;
      }
    }

    if (ret_val == RIL_E_SUCCESS)
    {
      QCRIL_LOG_INFO("qmi handover ind ho_state: %d", qmi_handover_ind_msg_ptr->ho_state);
      auto voiceModule = getVoiceModule();
      switch (qmi_handover_ind_msg_ptr->ho_state)
      {
        case VOICE_HANDOVER_START_V02:
          ril_srvccstate = HANDOVER_STARTED;
          qcril_qmi_voice_mark_calls_srvcc_in_progress();
          if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_G_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_W_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_SRVCC_START, NULL);
          }
          else if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_C_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_GW_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_DRVCC_START, NULL);
          }
          break;
        case VOICE_HANDOVER_FAIL_V02:
          ril_srvccstate = HANDOVER_FAILED;
          qcril_qmi_voice_unmark_calls_srvcc_in_progress();
          if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_G_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_W_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_SRVCC_FAIL, NULL);
          }
          else if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_C_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_GW_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_DRVCC_FAIL, NULL);
          }
          break;
        case VOICE_HANDOVER_COMPLETE_V02:
          ril_srvccstate = HANDOVER_COMPLETED;
          qcril_qmi_voice_reset_all_calls_from_auto_to_cs_domain_elab();
          if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_G_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_W_V02)
          {
            // Check for DSDS transition mode
            if (voiceModule && voiceModule->callOnBothSubs()) {
              // End IMS calls on other sub.
              qcril_instance_id_e_type instance;
              instance = qmi_ril_get_process_instance_id();
              auto ipc = std::make_shared<IpcDisconnectCallsMessage>(instance);
              ipc->broadcast();
            }
            qcril_am_handle_event(QCRIL_AM_EVENT_SRVCC_COMPLETE, NULL);
          }
          else if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_C_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_GW_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_DRVCC_COMPLETE, NULL);
          }
          if (qcril_qmi_voice_external_is_screen_off())
          {
            qcril_qmi_voice_enable_voice_indications(FALSE);
          }
          break;
        case VOICE_HANDOVER_CANCEL_V02:
          ril_srvccstate = HANDOVER_CANCELED;
          qcril_qmi_voice_unmark_calls_srvcc_in_progress();
          if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_G_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_SRVCC_L_2_W_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_SRVCC_CANCEL, NULL);
          }
          else if(qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_C_V02 ||
              qmi_handover_ind_msg_ptr->ho_type == VOICE_HO_DRVCC_WIFI_2_GW_V02)
          {
            qcril_am_handle_event(QCRIL_AM_EVENT_DRVCC_CANCEL, NULL);
          }
          break;
        default:
          ret_val = RIL_E_GENERIC_FAILURE;
          break;
      }/* switch */

      if (RIL_E_SUCCESS == ret_val)
      {
        qcril_qmi_voice_send_ims_unsol_resp_handover(ril_srvccstate);
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_handover_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_reset_info_xml
//===========================================================================
void qcril_qmi_voice_reset_info_xml(qcril_qmi_voice_info_ind_xml_type_e_type xml_type)
{
   QCRIL_LOG_FUNC_ENTRY();
   qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].last_sequence_number =
        std::numeric_limits<decltype(
        qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].last_sequence_number)>::max();
   qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].total_size = 0;
   qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].filled_size = 0;

   if (qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].buffer)
   {
      qcril_free(qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].buffer);
      qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].buffer = NULL;
   }

   qcril_qmi_voice_info.qmi_info_ind_xml[xml_type].call_id_valid = FALSE;
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_reset_info_xml

//===========================================================================
// qcril_qmi_voice_conference_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_conference_info_ind_hdlr
(
   void *ind_data_ptr,
   uint32_t ind_data_len
)
{
   voice_conference_info_ind_msg_v02* qmi_ind_msg_ptr;

   QCRIL_LOG_FUNC_ENTRY();

   do
   {
      if (NULL == ind_data_ptr || 0 == ind_data_len)
      {
         QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
         break;
      }
      qmi_ind_msg_ptr = (voice_conference_info_ind_msg_v02*) ind_data_ptr;
      QCRIL_LOG_INFO("sequence: %d, total_size_valid: %d, total_size: %d, conference_xml_len: %d",
                     qmi_ind_msg_ptr->sequence,
                     qmi_ind_msg_ptr->total_size_valid,
                     qmi_ind_msg_ptr->total_size,
                     qmi_ind_msg_ptr->conference_xml_len);

      qcril_qmi_voice_process_qmi_info_ind(qmi_ind_msg_ptr->sequence,
                                           qmi_ind_msg_ptr->total_size_valid,
                                           qmi_ind_msg_ptr->total_size,
                                           qmi_ind_msg_ptr->conference_xml_len,
                                           qmi_ind_msg_ptr->conference_xml,
                                           qmi_ind_msg_ptr->call_id_valid,
                                           qmi_ind_msg_ptr->call_id,
                                           QCRIL_QMI_VOICE_CONFERENCE_INFO_IND_XML);

   } while (FALSE);

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_conference_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_dialog_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_dialog_info_ind_hdlr
(
   void *ind_data_ptr,
   uint32_t ind_data_len
)
{
   vice_dialog_info_ind_msg_v02* qmi_ind_msg_ptr;

   QCRIL_LOG_FUNC_ENTRY();

   do
   {
      if (NULL == ind_data_ptr || 0 == ind_data_len)
      {
         QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
         break;
      }
      qmi_ind_msg_ptr = (vice_dialog_info_ind_msg_v02*) ind_data_ptr;
      QCRIL_LOG_INFO("sequence: %d, total_size_valid: %d, total_size: %d, dialog_xml_len: %d",
                     qmi_ind_msg_ptr->sequence,
                     qmi_ind_msg_ptr->total_size_valid,
                     qmi_ind_msg_ptr->total_size,
                     qmi_ind_msg_ptr->vice_dialog_xml_len);

      qcril_qmi_voice_process_qmi_info_ind(qmi_ind_msg_ptr->sequence,
                                           qmi_ind_msg_ptr->total_size_valid,
                                           qmi_ind_msg_ptr->total_size,
                                           qmi_ind_msg_ptr->vice_dialog_xml_len,
                                           qmi_ind_msg_ptr->vice_dialog_xml,
                                           FALSE,
                                           INVALID_NEGATIVE_ONE,
                                           QCRIL_QMI_VOICE_DIALOG_INFO_IND_XML);

   } while (FALSE);

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_dialog_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_process_qmi_info_ind
//===========================================================================
void qcril_qmi_voice_process_qmi_info_ind(uint32_t sequence,
                                          uint8_t total_size_valid,
                                          uint32_t total_size,
                                          uint32_t xml_len,
                                          uint8_t* xml,
                                          uint8_t call_id_valid,
                                          uint8_t call_id,
                                          qcril_qmi_voice_info_ind_xml_type_e_type xml_type)
{
   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_voice_info_lock();

   qcril_qmi_voice_info_ind_xml_type* current_xml;
   current_xml = &qcril_qmi_voice_info.qmi_info_ind_xml[xml_type];

   do
   {
      if (0 == sequence)
      {
         if (total_size_valid)
         {
            qcril_qmi_voice_reset_info_xml(xml_type);
            current_xml->total_size = total_size;
            current_xml->buffer = (uint8_t *)qcril_malloc(current_xml->total_size);
            if (NULL == current_xml->buffer)
            {
               QCRIL_LOG_ERROR("malloc failed");
               qcril_qmi_voice_reset_info_xml(xml_type);
               break;
            }
         }
         else
         {
            QCRIL_LOG_ERROR("no total size in the first sequence indication");
            break;
         }
      }

      if (current_xml->filled_size + xml_len > current_xml->total_size)
      {
         QCRIL_LOG_ERROR("filled_size (%d) + new conference_xml_len (%d) > total_size (%d)",
                         current_xml->filled_size, xml_len, current_xml->total_size);
         qcril_qmi_voice_reset_info_xml(xml_type);
         break;
      }

      if (sequence != get_next_sequence_number(current_xml->last_sequence_number))
      {
         QCRIL_LOG_ERROR("sequence out of order! new msg seq#: %d, last_seq#: %d",
                         sequence, current_xml->last_sequence_number);
         qcril_qmi_voice_reset_info_xml(xml_type);
         break;
      }

      if (NULL == current_xml->buffer)
      {
         QCRIL_LOG_ERROR("qcril_qmi_voice_info.conf_xml.buffer is NULL");
         break;
      }

      memcpy( &(current_xml->buffer[current_xml->filled_size]), xml, xml_len );
      current_xml->filled_size += xml_len;
      current_xml->last_sequence_number = sequence;

      if (call_id_valid)
      {
        current_xml->call_id_valid = TRUE;
        current_xml->call_id = call_id;
      }

      if (current_xml->filled_size == current_xml->total_size)
      {
         if(xml_type == QCRIL_QMI_VOICE_CONFERENCE_INFO_IND_XML)
         {
           auto msg = std::make_shared<QcRilUnsolImsConferenceInfoMessage>();
           if (msg != nullptr) {
             std::vector<uint8_t> conf_info_uri(current_xml->total_size);
             for (uint32_t  i = 0; i < current_xml->total_size; i++) {
               conf_info_uri[i] = current_xml->buffer[i];
             }
             msg->setConferenceInfoUri(conf_info_uri);
             if (current_xml->call_id_valid)
             {
               qcril_qmi_voice_voip_call_info_entry_type *call_info_entry =
                 qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(current_xml->call_id);

               if (call_info_entry)
               {
                 msg->setConferenceCallState(
                     convertConferenceCallState(call_info_entry->voice_scv_info.call_state));
               }
             }
             Dispatcher::getInstance().dispatchSync(msg);
           }

           qcril_qmi_voice_send_unsol_call_state_changed();
           qcril_qmi_voice_reset_info_xml(xml_type);
         }
         if(xml_type == QCRIL_QMI_VOICE_DIALOG_INFO_IND_XML)
         {
           qcril_qmi_voice_send_vice_dialog_info_unsol(current_xml->buffer,
                                                       current_xml->total_size);
           qcril_qmi_voice_info.send_vice_unsol_on_socket_connect = TRUE;
         }
      }
      else if(xml_type == QCRIL_QMI_VOICE_DIALOG_INFO_IND_XML)
      {
        qcril_qmi_voice_info.send_vice_unsol_on_socket_connect = FALSE;
      }

   } while (FALSE);

   qcril_qmi_voice_info_unlock();

   QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_send_vice_dialog_info_unsol
//===========================================================================
void qcril_qmi_voice_send_vice_dialog_info_unsol(uint8_t *buffer, uint32_t size)
{
  if (buffer && size)
  {
    auto msg = std::make_shared<QcRilUnsolImsViceInfoMessage>();
    if (msg != nullptr)
    {
      std::vector<uint8_t> vice_info_uri(size);
      for (uint32_t i = 0; i < size; i++)
      {
        vice_info_uri[i] = buffer[i];
      }
      msg->setViceInfoUri(vice_info_uri);
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }
}

//===========================================================================
// qcril_qmi_voice_send_unsol_vice_dialog_refresh_info_helper
//===========================================================================
void qcril_qmi_voice_ims_send_unsol_vice_dialog_refresh_info_helper(void)
{
   qcril_qmi_voice_info_ind_xml_type* vice_dialog_xml;

   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_voice_info_lock();

   if(TRUE == qcril_qmi_voice_info.send_vice_unsol_on_socket_connect)
   {
      vice_dialog_xml =
              &qcril_qmi_voice_info.qmi_info_ind_xml[QCRIL_QMI_VOICE_DIALOG_INFO_IND_XML];

      qcril_qmi_voice_send_vice_dialog_info_unsol(vice_dialog_xml->buffer,
                                                  vice_dialog_xml->total_size);
   }

   qcril_qmi_voice_info_unlock();

   QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_tty_ind_hdlr
//===========================================================================
void qcril_qmi_voice_tty_ind_hdlr
(
   void *ind_data_ptr,
   uint32_t ind_data_len
)
{
   voice_tty_ind_msg_v02* qmi_ind_msg_ptr;

   QCRIL_LOG_FUNC_ENTRY();

   do
   {
      if (NULL == ind_data_ptr || 0 == ind_data_len)
      {
         QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
         break;
      }

      qmi_ind_msg_ptr = (voice_tty_ind_msg_v02*) ind_data_ptr;
      QCRIL_LOG_INFO("tty_mode: %d", qmi_ind_msg_ptr->tty_mode);

      qcril::interfaces::TtyMode mode = qcril::interfaces::TtyMode::UNKNOWN;
      switch(qmi_ind_msg_ptr->tty_mode)
      {
         case TTY_MODE_FULL_V02:
            mode = qcril::interfaces::TtyMode::FULL;
            break;

         case TTY_MODE_VCO_V02:
            mode = qcril::interfaces::TtyMode::VCO;
            break;

         case TTY_MODE_HCO_V02:
            mode = qcril::interfaces::TtyMode::HCO;
            break;

         case TTY_MODE_OFF_V02:
            mode = qcril::interfaces::TtyMode::MODE_OFF;
            break;

         default:
            mode = qcril::interfaces::TtyMode::MODE_OFF;
            break;
      }

      auto msg = std::make_shared<QcRilUnsolImsTtyNotificationMessage>();
      if (msg != nullptr) {
        msg->setTtyMode(mode);
        Dispatcher::getInstance().dispatchSync(msg);
      }
   } while (FALSE);

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_tty_ind_hdlr

/*=========================================================================
  FUNCTION: qcril_qmi_voice_call_control_result_info_ind_hdlr

    @brief
    Handle QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02.

    @return
    None.
=========================================================================*/
void qcril_qmi_voice_call_control_result_info_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
   voice_call_control_result_info_ind_msg_v02* qmi_ind_msg_ptr;
   qcril_instance_id_e_type instance_id;
   char buf_str[QMI_VOICE_CC_ALPHA_TEXT_MAX_V02 + 2];

   qcril_qmi_voice_voip_call_info_entry_type * call_entry;
   int                                         name_len;
   int                                         call_entry_is_already_emulating;

   QCRIL_LOG_FUNC_ENTRY();
   instance_id = qmi_ril_get_process_instance_id();
   memset(buf_str, 0 , QMI_VOICE_CC_ALPHA_TEXT_MAX_V02 + 2);

   if( ind_data_ptr != NULL && ind_data_len != 0 )
   {
       qmi_ind_msg_ptr = (voice_call_control_result_info_ind_msg_v02*) ind_data_ptr;

        if( ( VOICE_CC_ALPHA_NOT_PRESENT_V02 == qmi_ind_msg_ptr->alpha_presence ) ||
            ( VOICE_CC_ALPHA_NULL_V02 == qmi_ind_msg_ptr->alpha_presence )
          )
        {
            QCRIL_LOG_INFO("Either Alhpa is absent in cc result or Alpha is present but length is zero");
        }
        else
        {
            if( TRUE == qmi_ind_msg_ptr->alpha_text_gsm8_valid )
            {
                QCRIL_LOG_INFO("Alpha text message is present in gsm8 bit format");
                if(qmi_ind_msg_ptr->alpha_text_gsm8_len < QMI_VOICE_CC_ALPHA_TEXT_MAX_V02 )
                    qcril_cm_ss_convert_gsm8bit_alpha_string_to_utf8( (char*) qmi_ind_msg_ptr->alpha_text_gsm8,
                                                                qmi_ind_msg_ptr->alpha_text_gsm8_len,
                                                                buf_str, sizeof(buf_str) );
            }
            else
            {
                QCRIL_LOG_INFO("Alpha text message is present in UTF16 format");
                qcril_cm_ss_convert_ucs2_to_utf8( (char *) qmi_ind_msg_ptr->alpha_text_utf16,
                                                             qmi_ind_msg_ptr->alpha_text_utf16_len * 2,
                                                           buf_str,
                                                           sizeof(buf_str) );
            }
        }
   }
   else
   {
       QCRIL_LOG_ERROR("ind_data_ptr is NULL");
   }

   if ( *buf_str )
   { // something to relay
      call_entry_is_already_emulating = FALSE;

      call_entry = qcril_qmi_voice_voip_find_call_info_entry_by_single_elaboration_extended(
              QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_STK_CC_EMULATED_OVERLAY, TRUE );
      if ( NULL != call_entry )
      {
         call_entry_is_already_emulating = TRUE;
      }
      QCRIL_LOG_INFO( ".. stk cc emulate check, obj %x, already emulating %d",
                     (intptr_t)call_entry, call_entry_is_already_emulating );

      if ( NULL != call_entry )
      {  // STK CC emulation: keep alpha as overlay "name"

         if ( NULL != call_entry->overlayed_name_storage_for_emulated_stk_cc )
         {
            qcril_free( call_entry->overlayed_name_storage_for_emulated_stk_cc );
         }
         name_len = strlen( buf_str );
         call_entry->overlayed_name_storage_for_emulated_stk_cc = (char*)qcril_malloc( name_len + 1 );
         if ( NULL != call_entry->overlayed_name_storage_for_emulated_stk_cc )
         {
            strlcpy( call_entry->overlayed_name_storage_for_emulated_stk_cc, buf_str, name_len + 1 );
         }
         QCRIL_LOG_INFO( ".. setting overlay name %s for android call id %d under ongoing == %d emulation",
                           call_entry->overlayed_name_storage_for_emulated_stk_cc,
                           call_entry->android_call_id,
                           call_entry_is_already_emulating );

         if ( call_entry_is_already_emulating )
         {
           // if QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_STK_CC_EMULATED_OVERLAY is set,
           // that means DIAL RESP already received and we should update call list
           qcril_qmi_voice_send_unsol_call_state_changed();
         }
      } // if ( NULL != call_entry ) i.e. no overlay, no STK CC emulation
      else
      {
         // no STK CC emulation
         auto msg = std::make_shared<RilUnsolStkCCAlphaNotifyMessage>(
             std::string(static_cast<const char*>(buf_str)));
         Dispatcher::getInstance().dispatchSync(msg);
      }
   }

    QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_voice_call_control_result_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_reset_additional_call_info
//===========================================================================
void qcril_qmi_voice_reset_additional_call_info
(
    qcril_qmi_voice_voip_call_info_entry_type *entry
)
{
   QCRIL_LOG_FUNC_ENTRY();

   if (entry)
   {
      entry->additional_call_info.last_sequence_number = std::numeric_limits<
            decltype(entry->additional_call_info.last_sequence_number)>::max();
      entry->additional_call_info.total_size = 0;
      entry->additional_call_info.filled_size = 0;

      if (entry->additional_call_info.buffer)
      {
         qcril_free(entry->additional_call_info.buffer);
         entry->additional_call_info.buffer = NULL;
      }
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_reset_additional_call_info

//===========================================================================
// qcril_qmi_voice_is_additional_call_info_available
//===========================================================================
boolean qcril_qmi_voice_is_additional_call_info_available
(
    const qcril_qmi_voice_voip_call_info_entry_type *entry
)
{
   boolean add_info_present = FALSE;

   QCRIL_LOG_FUNC_ENTRY();

   if (entry)
   {
      if (entry->additional_call_info.is_add_info_present &&
          (entry->additional_call_info.total_size > 0)&&
          (entry->additional_call_info.filled_size ==
           entry->additional_call_info.total_size))
      {
         add_info_present = TRUE;
      }
   }

   QCRIL_LOG_FUNC_RETURN_WITH_RET(add_info_present);

   return add_info_present;
} // qcril_qmi_voice_is_additional_call_info_available


//===========================================================================
// qcril_qmi_voice_additional_call_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_additional_call_info_ind_hdlr
(
   void *ind_data_ptr,
   uint32_t ind_data_len
)
{
   voice_additional_call_info_ind_msg_v02    *qmi_ind_msg_ptr = NULL;
   voice_additional_call_info_type_v02       *add_call_info   = NULL;
   qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;

   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_voice_info_lock();

   do
   {
      if (NULL == ind_data_ptr || 0 == ind_data_len)
      {
         QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
         break;
      }
      qmi_ind_msg_ptr = (voice_additional_call_info_ind_msg_v02*) ind_data_ptr;
      if (qmi_ind_msg_ptr->extension_header_info_valid)
      {
         add_call_info   = &(qmi_ind_msg_ptr->extension_header_info);
      }
      else
      {
         QCRIL_LOG_ERROR("extension_header_info is not valid");
         break;
      }
      QCRIL_LOG_INFO("call_id: %d, sequence: %d, total_size: %d, additional_call_info_len: %d",
              qmi_ind_msg_ptr->call_id, add_call_info->sequence,
              add_call_info->total_size, add_call_info->additional_call_info_len);

      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(
              qmi_ind_msg_ptr->call_id);
      if (NULL == call_info_entry)
      {
         QCRIL_LOG_ERROR("Unable to find call info entry for call_id: %d",
                 qmi_ind_msg_ptr->call_id);
         break;
      }

      if (0 == add_call_info->sequence)
      {
         qcril_qmi_voice_reset_additional_call_info(call_info_entry);
         call_info_entry->additional_call_info.total_size = add_call_info->total_size;
         call_info_entry->additional_call_info.buffer     =
               (uint8_t*)qcril_malloc(call_info_entry->additional_call_info.total_size);
         if (NULL == call_info_entry->additional_call_info.buffer)
         {
            QCRIL_LOG_ERROR("malloc failed");
            qcril_qmi_voice_reset_additional_call_info(call_info_entry);
            break;
         }
      }

      if ((call_info_entry->additional_call_info.filled_size +
            add_call_info->additional_call_info_len) >
          call_info_entry->additional_call_info.total_size)
      {
         QCRIL_LOG_ERROR("filled_size (%d) + new additional_call_info_len (%d) > total_size (%d)",
                         call_info_entry->additional_call_info.filled_size,
                         add_call_info->additional_call_info_len,
                         call_info_entry->additional_call_info.total_size);
         qcril_qmi_voice_reset_additional_call_info(call_info_entry);
         break;
      }

      if (add_call_info->sequence != get_next_sequence_number(
            call_info_entry->additional_call_info.last_sequence_number))
      {
         QCRIL_LOG_ERROR("sequence out of order! new msg seq#: %d, last_seq#: %d",
                         add_call_info->sequence,
                         call_info_entry->additional_call_info.last_sequence_number);
         qcril_qmi_voice_reset_additional_call_info(call_info_entry);
         break;
      }

      if (NULL == call_info_entry->additional_call_info.buffer)
      {
         QCRIL_LOG_ERROR("call_info_entry->additional_call_info.buffer is NULL");
         break;
      }

      memcpy(&(call_info_entry->additional_call_info.buffer[
                    call_info_entry->additional_call_info.filled_size]),
              add_call_info->additional_call_info, add_call_info->additional_call_info_len);
      call_info_entry->additional_call_info.filled_size += add_call_info->additional_call_info_len;
      call_info_entry->additional_call_info.last_sequence_number = add_call_info->sequence;
      if (call_info_entry->additional_call_info.filled_size ==
          call_info_entry->additional_call_info.total_size)
      {
        qcril_qmi_voice_send_unsol_call_state_changed();
      }
   } while (FALSE);

   qcril_qmi_voice_info_unlock();

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_additional_call_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_audio_rat_change_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_audio_rat_change_info_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_audio_rat_change_info_ind_msg_v02 *qmi_ind_msg_ptr = NULL;
  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( NULL == ind_data_ptr || 0 == ind_data_len )
    {
      QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
      break;
    }
    qmi_ind_msg_ptr = (voice_audio_rat_change_info_ind_msg_v02*)ind_data_ptr;

    qcril_am_handle_event(QCRIL_AM_EVENT_AUDIO_RAT_CHANGED, qmi_ind_msg_ptr);
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_audio_rat_change_info_ind_hdlr

//===========================================================================
// qcril_qmi_voice_conf_participant_status_info_ind_hdlr
//===========================================================================
void qcril_qmi_voice_conf_participant_status_info_ind_hdlr
(
  void *ind_data_ptr,
  uint32_t ind_data_len
)
{
  voice_conf_participant_status_info_ind_msg_v02 *qmi_ind_msg_ptr = NULL;
  qcril_qmi_voice_voip_call_info_entry_type      *call_info_entry = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if( ind_data_ptr == NULL || ind_data_len == 0 )
    {
      QCRIL_LOG_ERROR("ind_data_ptr is NULL or ind_data_len is 0");
      break;
    }

    qmi_ind_msg_ptr = (voice_conf_participant_status_info_ind_msg_v02 *) ind_data_ptr;

    QCRIL_LOG_DEBUG("call_id = %d, participant_uri = %s",
        qmi_ind_msg_ptr->call_id, qmi_ind_msg_ptr->participant_uri);

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(
        qmi_ind_msg_ptr->call_id);

    if (call_info_entry == NULL)
    {
      QCRIL_LOG_ERROR("Unable to find call info entry for call_id: %d", qmi_ind_msg_ptr->call_id);
      break;
    }

    auto msg = std::make_shared<QcRilUnsolConfParticipantStatusInfoMessage>();
    if (msg != nullptr)
    {
      msg->setCallId(call_info_entry->android_call_id);
      msg->setParticipantUri(qmi_ind_msg_ptr->participant_uri);

      if (qmi_ind_msg_ptr->op_status_valid)
      {
        msg->setOperation(
            convertConferenceParticipantOperation(qmi_ind_msg_ptr->op_status.operation));
        msg->setSipStatus(qmi_ind_msg_ptr->op_status.sip_status);
      }

      if (qmi_ind_msg_ptr->is_qmi_voice_transfer_valid)
      {
        msg->setIsEct(qmi_ind_msg_ptr->is_qmi_voice_transfer);
      }
      Dispatcher::getInstance().dispatchSync(msg);
    }
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_conf_participant_status_info_ind_hdlr

//===========================================================================
// map_qmi_call_type_to_ril_type
//===========================================================================
qcril::interfaces::CallType map_qmi_call_type_to_ril_type(call_type_enum_v02 call_type) {
  switch (call_type) {
    case CALL_TYPE_VT_V02:
    case CALL_TYPE_EMERGENCY_VT_V02:
    case CALL_TYPE_VIDEOSHARE_V02:
      return qcril::interfaces::CallType::VT;
    default:
      return qcril::interfaces::CallType::VOICE;
  }
} // map_qmi_call_type_to_ril_type

//===========================================================================
// qcril_qmi_voice_auto_call_rejection_ind_hdlr
//===========================================================================
void qcril_qmi_voice_auto_call_rejection_ind_hdlr
(
    void * auto_reject_data_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();
  do {
    auto qmi_auto_reject_info = static_cast<
        auto_rejected_incoming_call_end_ind_msg_v02*>(auto_reject_data_ptr);
    if (!qmi_auto_reject_info) {
      QCRIL_LOG_ERROR("qmi_auto_reject_info is NULL. returning");
      break;
    }

    bool has_composerInfo = false;
    qcril::interfaces::CallComposerInfo info{};
    if (qmi_auto_reject_info->call_composer_importance_valid)
    {
        auto value = qmi_auto_reject_info->call_composer_importance;
        if (value == CALL_COMP_IMP_NORMAL_V02)
            info.setPriority(qcril::interfaces::Priority::NORMAL);
        else if (value == CALL_COMP_IMP_URGENT_V02)
            info.setPriority(qcril::interfaces::Priority::URGENT);
        has_composerInfo = true;
    }

    if (qmi_auto_reject_info->call_composer_subject_valid)
    {
        if (qmi_auto_reject_info->call_composer_subject_len <=
                QMI_VOICE_CALL_COMP_SUBJECT_MAX_LEN_V02)
        {
            std::vector<uint16_t> subject(
                    qmi_auto_reject_info->call_composer_subject,
                    qmi_auto_reject_info->call_composer_subject +
                    qmi_auto_reject_info->call_composer_subject_len);
            info.setSubject(std::move(subject));
            has_composerInfo = true;
        }
    }

    if (qmi_auto_reject_info->call_composer_organization_header_valid)
    {
        if (qmi_auto_reject_info->call_composer_organization_header_len <=
                QMI_VOICE_CALL_COMP_ORG_HDR_MAX_LEN_V02)
        {
            std::vector<uint16_t> organization(
                    qmi_auto_reject_info->call_composer_organization_header,
                    qmi_auto_reject_info->call_composer_organization_header +
                    qmi_auto_reject_info->call_composer_organization_header_len);
            info.setOrganization(std::move(organization));
            has_composerInfo = true;
        }
    }

    if (qmi_auto_reject_info->call_composer_picture_url_valid)
    {
        info.setImageUrl(qmi_auto_reject_info->call_composer_picture_url);
        has_composerInfo = true;
    }

    qcril::interfaces::Location location;
    location.setRadius(qcril::interfaces::Location::LOCATION_NOT_SET);
    if (qmi_auto_reject_info->call_composer_circle_location_valid)
    {
        const auto& cl = qmi_auto_reject_info->call_composer_circle_location;
        location.setRadius(cl.radius);
        location.setLatitude(cl.coordinates.latitude);
        location.setLongitude(cl.coordinates.longitude);
        has_composerInfo = true;
    }
    else if (qmi_auto_reject_info->call_composer_point_location_valid)
    {
        const auto& pl = qmi_auto_reject_info->call_composer_point_location;
        location.setRadius(qcril::interfaces::Location::POINT_LOCATION);
        location.setLatitude(pl.coordinates.latitude);
        location.setLongitude(pl.coordinates.longitude);
        has_composerInfo = true;
    }
    info.setLocation(location);

    bool has_ecnamInfo = false;
    qcril::interfaces::EcnamInfo ecnamInfo{};
    if (qmi_auto_reject_info->ip_caller_name_valid) {
      if (qmi_auto_reject_info->ip_caller_name_len <= QMI_VOICE_IP_CALLER_NAME_MAX_LEN_V02) {
        uint32_t caller_name_len = 0;
        char ip_caller_name[(QMI_VOICE_IP_CALLER_NAME_MAX_LEN_V02 * 2)] = "\0";
        memset( &ip_caller_name, 0, sizeof(ip_caller_name) );
        caller_name_len = qcril_cm_ss_convert_ucs2_to_utf8(
            (const char*)qmi_auto_reject_info->ip_caller_name,
            qmi_auto_reject_info->ip_caller_name_len * 2, ip_caller_name, sizeof(ip_caller_name));
        if (caller_name_len > 0) {
          ecnamInfo.setName(ip_caller_name);
          has_ecnamInfo = true;
        }
      }
    }

    if (qmi_auto_reject_info->call_ecnam_icon_url_valid) {
      ecnamInfo.setIconUrl(qmi_auto_reject_info->call_ecnam_icon_url);
      has_ecnamInfo = true;
    }

    if (qmi_auto_reject_info->call_ecnam_info_url_valid) {
      ecnamInfo.setInfoUrl(qmi_auto_reject_info->call_ecnam_info_url);
      has_ecnamInfo = true;
    }

    if (qmi_auto_reject_info->call_ecnam_card_url_valid) {
      ecnamInfo.setCardUrl(qmi_auto_reject_info->call_ecnam_card_url);
      has_ecnamInfo = true;
    }

    auto msg = std::make_shared<QcRilUnsolAutoCallRejectionMessage>();
    if (msg != nullptr)
    {
      msg->setCallType(map_qmi_call_type_to_ril_type(qmi_auto_reject_info->call_type));
      msg->setCallFailCause(qcril_qmi_ims_map_ril_failcause_to_ims_failcause(
                 qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED,
                 qmi_auto_reject_info->call_end_reason,
                 TRUE,
                 qmi_auto_reject_info->sip_error_code));
      msg->setSipErrorCode(qmi_auto_reject_info->sip_error_code);
      if ((qmi_auto_reject_info->num_valid) && (qmi_auto_reject_info->num_len > 0)) {
        msg->setNumber(qmi_auto_reject_info->num);
      }
      if (has_composerInfo)
      {
        msg->setComposerInfo(info);
      }
      if (qmi_auto_reject_info->mt_call_verstat_info_valid) {
        msg->setVerificationStatus(qcril_qmi_map_verstat_verification_state_to_ims(
                qmi_auto_reject_info->mt_call_verstat_info));
      }
      if (has_ecnamInfo) {
        msg->setEcnamInfo(ecnamInfo);
      }
      if (qmi_auto_reject_info->is_dc_call_valid) {
        msg->setIsDcCall(qmi_auto_reject_info->is_dc_call);
      }
      if (qmi_auto_reject_info->call_reason_valid && qmi_auto_reject_info->call_reason_len) {
        char *call_reason = qcril_qmi_voice_convert_call_reason(
                qmi_auto_reject_info->call_reason, qmi_auto_reject_info->call_reason_len);
        QCRIL_LOG_DEBUG("call reason %s", call_reason);
        msg->setCallReason(std::string(call_reason));
        delete[] call_reason;
      }
      Dispatcher::getInstance().dispatchSync(msg);
    }
  } while (FALSE);
  QCRIL_LOG_FUNC_RETURN();
}//qcril_qmi_voice_auto_call_rejection_ind_hdlr

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_sip_dtmf_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handles sip dtmf indications

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_sip_dtmf_ind_hdlr
(
    void *ind_data_ptr
)
{
  voice_sip_dtmf_ind_msg_v02 *sip_dtmf_ind = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  if( ind_data_ptr != NULL )
  {
    sip_dtmf_ind = (voice_sip_dtmf_ind_msg_v02*)ind_data_ptr;
  }

  auto msg = std::make_shared<QcRilUnsolOnSipDtmfMessage>();
  if (sip_dtmf_ind != NULL && msg != nullptr)
  {
    if (sip_dtmf_ind->dtmf_info_valid)
    {
      msg->setMessage(sip_dtmf_ind->dtmf_info.dtmf_string);
    }
    Dispatcher::getInstance().dispatchSync(msg);
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION: qcril_qmi_voice_srtp_status_ind_hdlr

    @brief
    Handle QMI_VOICE_SRTP_STATUS_IND_V02

    @return
    None.
=========================================================================*/
void qcril_qmi_voice_srtp_status_ind_hdlr(const voice_srtp_status_ind_msg_v02& srtp_ind)
{
  QCRIL_LOG_FUNC_ENTRY();
  uint8_t call_id = VOICE_INVALID_CALL_ID;
  qcril_qmi_voice_voip_lock_overview();
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry =
      qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id(srtp_ind.srtp_status_info.call_id);
  if (call_info_entry) {
    call_id = call_info_entry->android_call_id;
  }
  qcril_qmi_voice_voip_unlock_overview();
  if (call_id != VOICE_INVALID_CALL_ID) {
    auto msg = std::make_shared<QcRilUnsolImsSrtpEncryptionStatus>(
        call_id,
        static_cast<bool>(srtp_ind.srtp_status_info.audio_srtp),
        static_cast<bool>(srtp_ind.srtp_status_info.video_srtp),
        static_cast<bool>(srtp_ind.srtp_status_info.text_srtp));
    if (msg != nullptr) {
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_unsol_ind_cb_helper

===========================================================================*/
/*!
    @brief
    Handle QMI indication

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_unsol_ind_cb_helper
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
)
{
  QCRIL_LOG_FUNC_ENTRY();

  switch(msg_id)
  {
  case QMI_VOICE_ALL_CALL_STATUS_IND_V02:
    qcril_qmi_voice_all_call_status_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_OTASP_STATUS_IND_V02:
    qcril_qmi_voice_otasp_status_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_PRIVACY_IND_V02:
    qcril_qmi_voice_privacy_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_SUPS_NOTIFICATION_IND_V02:
    qcril_qmi_voice_sups_notification_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_INFO_REC_IND_V02:
    qcril_qmi_voice_info_rec_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_USSD_IND_V02:
    if (decoded_payload && decoded_payload_len) {
      voice_ussd_ind_msg_v02* ussd_ind = reinterpret_cast<voice_ussd_ind_msg_v02*>(decoded_payload);
      if (ussd_ind) {
        qcril_qmi_voice_ussd_ind_hdlr_wrapper(*ussd_ind);
      }
    }
    break;

  case QMI_VOICE_USSD_RELEASE_IND_V02:
    qcril_qmi_voice_ussd_release_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_SUPS_IND_V02:
    qcril_qmi_voice_stk_cc_handle_voice_sups_ind( (voice_sups_ind_msg_v02*) decoded_payload );
    break;

  case QMI_VOICE_DTMF_IND_V02:
    qcril_qmi_voice_dtmf_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_EXT_BRST_INTL_IND_V02:
    qcril_qmi_voice_ext_brst_intl_ind_hdlr(decoded_payload, decoded_payload_len);
    break;

  case QMI_VOICE_UUS_IND_V02:
    qcril_qmi_voice_uus_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_MODIFIED_IND_V02:
    qcril_qmi_voice_modified_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_MODIFY_ACCEPT_IND_V02:
    qcril_qmi_voice_modify_accept_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_SPEECH_CODEC_INFO_IND_V02:
    qcril_qmi_voice_speech_codec_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_HANDOVER_IND_V02:
    qcril_qmi_voice_handover_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_CONFERENCE_INFO_IND_V02:
    qcril_qmi_voice_conference_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_VICE_DIALOG_INFO_IND_V02:
    qcril_qmi_voice_dialog_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_TTY_IND_V02:
    qcril_qmi_voice_tty_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02:
    qcril_qmi_voice_call_control_result_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_ADDITIONAL_CALL_INFO_IND_V02:
    qcril_qmi_voice_additional_call_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_AUDIO_RAT_CHANGE_INFO_IND_V02:
    qcril_qmi_voice_audio_rat_change_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_IND_V02:
    qcril_qmi_voice_conf_participant_status_info_ind_hdlr( decoded_payload, decoded_payload_len );
    break;

  case QMI_AUTO_REJECTED_INCOMING_CALL_END_IND_V02:
    qcril_qmi_voice_auto_call_rejection_ind_hdlr(decoded_payload);
    break;

  case QMI_VOICE_SIP_DTMF_IND_V02:
    qcril_qmi_voice_sip_dtmf_ind_hdlr(decoded_payload);
    break;

  case QMI_VOICE_SRTP_STATUS_IND_V02:
    if (decoded_payload && decoded_payload_len) {
      if (decoded_payload_len == sizeof(voice_srtp_status_ind_msg_v02)) {
        voice_srtp_status_ind_msg_v02* srtp_status =
            reinterpret_cast<voice_srtp_status_ind_msg_v02*>(decoded_payload);
        if (srtp_status) {
          qcril_qmi_voice_srtp_status_ind_hdlr(*srtp_status);
        }
      }
    }
    break;

  default:
    QCRIL_LOG_INFO("Unknown QMI VOICE indication %d", msg_id);
    break;
  }

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_voice_unsol_ind_cb_helper */

void qcril_qmi_voice_manage_calls_resp_hdlr
(
 uint16_t req_id,
 voice_manage_calls_resp_msg_v02 *manage_calls_resp,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_SUCCESS;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01 qmi_error;

  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_event_params;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight;
  int covered_by_oversight_handling;

  if (manage_calls_resp != nullptr)
  {
    qmi_result = manage_calls_resp->resp.result;
    qmi_error = manage_calls_resp->resp.error;

    if (qmi_result != QMI_RESULT_SUCCESS_V01)
    {
      ril_err = qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error);
      if (ril_err == RIL_E_MODEM_ERR && qmi_error == QMI_ERR_INVALID_ID_V01)
      {
        // handle specific error
        ril_err = RIL_E_INVALID_CALL_ID;
      }
    }
    QCRIL_LOG_INFO("QCRIL QMI VOICE MNG CALLS RESP: %s, ril_err = %d",
                   (ril_err == RIL_E_SUCCESS) ? "SUCCESS" : "FAILURE", ril_err);

    covered_by_oversight_handling = FALSE;
    qcril_qmi_voice_voip_lock_overview();
    command_oversight = qmi_ril_voice_ims_find_command_oversight_by_token(req_id);
    if (command_oversight != NULL)
    {
      memset(&oversight_event_params, 0, sizeof(oversight_event_params));
      oversight_event_params.locator.command_oversight = command_oversight;
      covered_by_oversight_handling = qmi_ril_voice_ims_command_oversight_handle_event(
          (QMI_RESULT_SUCCESS_V01 == qmi_result && QMI_ERR_NONE_V01 == qmi_error)
              ? QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS
              : QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_FAILURE,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_event_params);
    }
    qcril_qmi_voice_voip_unlock_overview();

    if (!covered_by_oversight_handling) {
        if (manage_calls_resp->failure_cause_valid == TRUE) {
          /* Send UNSOL msg with SS error code first */
          qcril_qmi_send_ss_failure_cause_oem_hook_unsol_resp(manage_calls_resp->failure_cause,
                                                              VOICE_INVALID_CALL_ID);
        }
    }

    // Invoke the callback function
    if (responseCb)
    {
      CommonVoiceResponseData respData = {req_id, ril_err, manage_calls_resp};
      responseCb(&respData);
    }
  }
}

uint32_t qcril_qmi_voice_convert_qmi_to_ril_call_forwarding_info
(
 int reason,
 uint32_t qmi_call_forwarding_info_len,
 voice_get_call_forwarding_info_type_v02 *qmi_call_forwarding_info,
 uint32_t ril_call_forwarding_info_len,
 RIL_CallForwardInfo *ril_call_forwarding_info
)
{
  size_t i = 0;
  uint32_t num_of_instances = 0;
  int instance_merged = FALSE;
  size_t interm_num_len = 0;
  RIL_CallForwardInfo *ril_buf_slot = nullptr;

  QCRIL_LOG_FUNC_ENTRY();
  do {
    if (qmi_call_forwarding_info == nullptr || ril_call_forwarding_info == nullptr)
    {
      QCRIL_LOG_INFO("Invalid parameters");
      break;
    }
    voice_get_call_forwarding_info_type_v02 *qmi_cf_info = qmi_call_forwarding_info;

    QCRIL_LOG_INFO("cf info len %d", (int)qmi_call_forwarding_info_len);

    for (i = 0; (i < qmi_call_forwarding_info_len && i < ril_call_forwarding_info_len); i++)
    {
      // Try to merge this instance with previous instances
      instance_merged = FALSE;
      qmi_cf_info = qmi_call_forwarding_info + i;
      for (uint32_t j = 0; j < num_of_instances; j++)
      {
        ril_buf_slot = ril_call_forwarding_info + j;
        if (ril_buf_slot->number != NULL)
        {
          interm_num_len = strlen(ril_buf_slot->number);
        }
        else
        {
          interm_num_len = QMI_RIL_ZERO;
        }
        if ((interm_num_len == qmi_cf_info->number_len) &&
            (QMI_RIL_ZERO == qmi_cf_info->number_len ||
             strncmp(qmi_cf_info->number, ril_buf_slot->number,
                     qmi_cf_info->number_len) == 0) &&
            (qmi_cf_info->service_status == ril_buf_slot->status) &&
            (qmi_cf_info->no_reply_timer == ril_buf_slot->timeSeconds))
        {
          // Number, status and timer are same, merge the serviceClass
          ril_buf_slot->serviceClass |= qmi_cf_info->service_class;
          instance_merged = TRUE;
          std::string numStr = PII(ril_buf_slot->number,
                                hide_phone_number(ril_buf_slot->number));
          QCRIL_LOG_INFO("service class 0x%x is merged with %d",
                         qmi_cf_info->service_class, j);
          QCRIL_LOG_INFO("(updated) instance %d, status = %d, serviceClass = %d, reason = %d, "
                         ", number = %s, toa = %d, timeSeconds = %d",
                         j, ril_buf_slot->status, ril_buf_slot->serviceClass,
                         ril_buf_slot->reason, numStr.c_str(),
                           ril_buf_slot->toa, ril_buf_slot->timeSeconds);
        }
      }

      if (!instance_merged)
      {
        RIL_CallForwardInfo *ril_cf_info = ril_call_forwarding_info + num_of_instances;

        // We can't merge it. So we get a new RIL_CallForwardInfo record.
        ril_cf_info->status = qmi_cf_info->service_status;

        ril_cf_info->serviceClass = qmi_cf_info->service_class;
        if (QMI_RIL_ZERO == qmi_cf_info->service_class)
        {
          ril_cf_info->serviceClass = QMI_RIL_FF;
          QCRIL_LOG_INFO("serviceClass adjusted to 0xFF for single instance");
        }
        ril_cf_info->reason = reason;
        interm_num_len = qmi_cf_info->number_len;
        if (interm_num_len > QMI_RIL_ZERO)
        {
          char *resp_num_ptr = (char *)qcril_malloc(interm_num_len + 1);
          if (resp_num_ptr)
          {
            memcpy(resp_num_ptr, qmi_cf_info->number, interm_num_len);
            resp_num_ptr[interm_num_len] = QMI_RIL_ZERO;

            ril_cf_info->toa = (QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX == *resp_num_ptr)
                                   ? QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL
                                   : QCRIL_QMI_VOICE_SS_TA_UNKNOWN;
            ril_cf_info->number = resp_num_ptr;
          }
          else
          {
            QCRIL_LOG_ERROR("out of memory");
            break;
          }
        }

        if (qmi_cf_info->no_reply_timer != QMI_RIL_ZERO)
        {
          ril_cf_info->timeSeconds = qmi_cf_info->no_reply_timer;
        }
        std::string numStr = PII(ril_cf_info->number,
                               hide_phone_number(ril_cf_info->number));
        QCRIL_LOG_INFO("instance %d, status = %d, serviceClass = %d, reason = %d, "
                       ", number = %s, toa = %d, timeSeconds = %d",
                       num_of_instances, ril_cf_info->status, ril_cf_info->serviceClass,
                       ril_cf_info->reason, numStr.c_str(), ril_cf_info->toa,
                       ril_cf_info->timeSeconds);
        num_of_instances++;
      }
    }
    if (num_of_instances == 0)
    {
      QCRIL_LOG_INFO("No numbers; set false");
      num_of_instances = 1;
      ril_call_forwarding_info->status = FALSE;
      ril_call_forwarding_info->reason = reason;
      /* if call forwarding is interrogated for specific service class, include only that service
       * class in response */
      if (qmi_call_forwarding_info->service_class != 0)
      {
        ril_call_forwarding_info->serviceClass = qmi_call_forwarding_info->service_class;
      }
      else
      {
        ril_call_forwarding_info->serviceClass = 0xff;
      }
    }
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(num_of_instances);

  return num_of_instances;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_orig_ussd_resp_hdlr

===========================================================================*/
/*!
    @brief
    Handle Orig USSD RESP.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_orig_ussd_resp_hdlr
(
  uint16_t req_id,
  voice_orig_ussd_resp_msg_v02 *orig_ussd_resp,
  CommonVoiceResponseCallback responseCb
)
{
  qcril_instance_id_e_type instance_id;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01  qmi_error;
  char ussd_utf8_str[QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2];
  qcril::interfaces::UssdModeType type_code = qcril::interfaces::UssdModeType::UNKNOWN;
  int utf8_len =0;
//  qcril_request_resp_params_type resp;
  uint8_t uss_dcs=QCRIL_QMI_VOICE_USSD_DCS_UNSPECIFIED;
  boolean success = TRUE;
  RIL_Errno ril_err;
  qcril_qmi_voice_stk_cc_modification_e_type stk_cc_modification;
  int i=0;
  qcril::interfaces::UssdDomain domain = qcril::interfaces::UssdDomain::CS;
  std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
//  bool send_response = false;

//  qcril_request_resp_params_type ril_response;

  instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  do
  {
    if(orig_ussd_resp)
    {
      memset( ussd_utf8_str, '\0', sizeof( ussd_utf8_str ) );

      qmi_result = orig_ussd_resp->resp.result;
      qmi_error = orig_ussd_resp->resp.error;
      QCRIL_LOG_INFO("QCRIL QMI VOICE Orig USSD Response qmi_error : %d", qmi_error );

      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex( QMI_NO_ERR,
                                                                            &orig_ussd_resp->resp,
                                                                            QCRIL_QMI_ERR_CTX_SEND_USSD_TXN,
                                                                            orig_ussd_resp );

      QCRIL_LOG_INFO("ORIG USSD RESP : ril_err=%d, orig_ussd_resp_result=%d, orig_ussd_resp_error=%d, cc_sups_result_valid=%d, cc_result_type_valid=%d, cc_result_type=%d",
                     (int)ril_err,
                     (int)orig_ussd_resp->resp.result,
                     (int)orig_ussd_resp->resp.error,
                     (int)orig_ussd_resp->cc_sups_result_valid,
                     (int)orig_ussd_resp->cc_result_type_valid,
                     (int)orig_ussd_resp->cc_result_type
                     );

      if( orig_ussd_resp->ussd_domain_route_valid ) {
        domain = qcril_qmi_voice_map_qmi_to_ril_ussd_domain(orig_ussd_resp->ussd_domain_route);
      }

      if( ril_err && orig_ussd_resp->cc_sups_result_valid)
      {
        QCRIL_LOG_INFO("Error Details : cc_sups_result_reason=%d, cc_sups_result_service=%d",orig_ussd_resp->cc_sups_result.reason,
                       orig_ussd_resp->cc_sups_result.service_type);
      }

      stk_cc_modification = qcril_qmi_voice_map_ril_error_to_stk_cc_modification_type(ril_err);

      if (ril_err == RIL_E_CANCELLED)
      {
        // Send success to telephony in case of CANCEL. This is to avoid error pop up
        // due to telephony design limitation when cancelling the multi USSD session.
        QCRIL_LOG_INFO("ril_err(%d) convert to RIL_E_SUCCESS", ril_err);
        ril_err = RIL_E_SUCCESS;
      }

      // QMI_VOICE can send valid Alpha in case of QMI_ERR_CARD_CALL_CONTROL_FAILED.
      // Handling Alpha for both success and failure cases.
      if ( orig_ussd_resp->alpha_id_valid )
      {
        stk_cc_info.alpha_ident = orig_ussd_resp->alpha_id;
        // Alpha id is present; return success.
        ril_err = RIL_E_SUCCESS;
      }
      else
      {
        memset( &stk_cc_info.alpha_ident, 0, sizeof( stk_cc_info.alpha_ident ) );
      }

      if ( QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE != stk_cc_modification )
      { // STK CC session started
        qcril_qmi_voice_reset_stk_cc();

        stk_cc_info.modification                      = stk_cc_modification;
        stk_cc_info.is_alpha_relayed                  = FALSE;

        if ( orig_ussd_resp->call_id_valid )
        {
          stk_cc_info.call_id_info = orig_ussd_resp->call_id;
        }

        if ( orig_ussd_resp->cc_sups_result_valid )
        {
          stk_cc_info.ss_ussd_info = orig_ussd_resp->cc_sups_result;
        }

        QCRIL_LOG_INFO( "org req altered. ril_err: %d, call_id: %d", (int)ril_err, (int)orig_ussd_resp->call_id );

        qcril_qmi_voice_stk_cc_dump();

        if (responseCb)
        {
          CommonVoiceResponseData respData = {req_id, ril_err, orig_ussd_resp};
          responseCb(&respData);
        }
      }
      else
      {
        if (orig_ussd_resp->sip_error_code_valid ||
            orig_ussd_resp->ussd_error_description_valid)
          {
            errorDetails = buildSipErrorInfo(
              orig_ussd_resp->sip_error_code_valid, orig_ussd_resp->sip_error_code,
              orig_ussd_resp->ussd_error_description_valid
                  ? (QMI_VOICE_USS_DATA_MAX_V02 + 1) : 0,
              (uint16_t *)&(orig_ussd_resp->ussd_error_description));
        }
        if(orig_ussd_resp->failure_cause_valid == TRUE)
        {
          /* Send UNSOL msg with SS error code first */
          qcril_qmi_send_ss_failure_cause_oem_hook_unsol_resp ( orig_ussd_resp->failure_cause,
              (orig_ussd_resp->call_id_valid ? orig_ussd_resp->call_id : VOICE_INVALID_CALL_ID) );
          if( (orig_ussd_resp->failure_cause == QMI_FAILURE_CAUSE_FACILITY_REJECTED_V02) ||
              (orig_ussd_resp->failure_cause == QMI_FAILURE_CAUSE_REJECTED_BY_NETWORK_V02 ) )
          {
            if (responseCb)
            {
              CommonVoiceResponseData respData = {req_id, RIL_E_ABORTED, orig_ussd_resp};
              responseCb(&respData);
            }
            if (errorDetails) {
              qcril_qmi_voice_send_unsol_ussd(qcril::interfaces::UssdDomain::UNKNOWN,
                                              qcril::interfaces::UssdModeType::NOTIFY,
                                              "", errorDetails);
            }
            break;
          }
        }

        if (responseCb)
        {
          CommonVoiceResponseData respData = {req_id, ril_err, orig_ussd_resp};
          responseCb(&respData);
        }
        if (ril_err != RIL_E_SUCCESS)
        {
          /* Reset the user action flag, since the session failed */
          qcril_qmi_voice_info.ussd_user_action_required = FALSE;
          QCRIL_LOG_ERROR("Failure response for SEND_USSD!! : ril_err=%d\n", ril_err);
          if (errorDetails) {
            qcril_qmi_voice_send_unsol_ussd(qcril::interfaces::UssdDomain::UNKNOWN,
                                            qcril::interfaces::UssdModeType::NOTIFY,
                                            "", errorDetails);
          }
          break;
        }

        /* send RIL_UNSOL_STK_CC_ALPHA_NOTIFY to Telephony if Alpha is valid */
        qcril_qmi_voice_stk_cc_relay_alpha_if_necessary(QCRIL_DEFAULT_INSTANCE_ID, TRUE);

        if( (qmi_result == QMI_RESULT_SUCCESS_V01) &&
            ( (orig_ussd_resp->uss_info_valid == TRUE) || (orig_ussd_resp->uss_info_utf16_valid == TRUE) )
          )
        {
          if ( orig_ussd_resp->uss_info_utf16_valid == TRUE ) // using uss_info_utf16 instead of uss_info if it is available
          {
            QCRIL_LOG_ERROR ("USSD Orig resp, utf16 len=%d", orig_ussd_resp->uss_info_utf16_len );

            utf8_len = qcril_cm_ss_convert_ucs2_to_utf8( (char *) orig_ussd_resp->uss_info_utf16, orig_ussd_resp->uss_info_utf16_len * 2, ussd_utf8_str, sizeof(ussd_utf8_str) );
            for(i=0 ; i< utf8_len ; i++ )
            {
              QCRIL_LOG_DEBUG ("utf8 data bytes : %x\n", ussd_utf8_str[ i ]);
            }
            if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
            {
              QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
              utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
              ussd_utf8_str[ utf8_len - 1] = '\0';
            }
          }
          else
          {
            QCRIL_LOG_ERROR ("USSD Orig resp dcs=%d , len=%d", orig_ussd_resp->uss_info.uss_dcs,orig_ussd_resp->uss_info.uss_data_len );
            switch(orig_ussd_resp->uss_info.uss_dcs)
            {
              case USS_DCS_ASCII_V02 :
                utf8_len = qcril_cm_ss_ascii_to_utf8((unsigned char *)orig_ussd_resp->uss_info.uss_data, orig_ussd_resp->uss_info.uss_data_len,
                                                     ussd_utf8_str, sizeof(ussd_utf8_str));
                break;
              case USS_DCS_8BIT_V02 :
                uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_8_BIT;
                utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8( uss_dcs,
                                                                    orig_ussd_resp->uss_info.uss_data,
                                                                    orig_ussd_resp->uss_info.uss_data_len,
                                                                    ussd_utf8_str,
                                                                    sizeof(ussd_utf8_str) );
                if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
                {
                  QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
                  utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
                  ussd_utf8_str[ utf8_len - 1] = '\0';
                }
                break;
              case USS_DCS_UCS2_V02 :
                uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_UCS2;
                utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8( uss_dcs,
                                                                    orig_ussd_resp->uss_info.uss_data,
                                                                    orig_ussd_resp->uss_info.uss_data_len,
                                                                    ussd_utf8_str,
                                                                    sizeof(ussd_utf8_str));
                if ( utf8_len > ( QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2 ) )
                {
                  QCRIL_LOG_ERROR ("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
                  utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
                  ussd_utf8_str[ utf8_len - 1] = '\0';
                }
                break;
              default :
                QCRIL_LOG_ERROR ("Invalid USSD dcs : %d", orig_ussd_resp->uss_info.uss_dcs );
                success = FALSE;
                break;
            }
          }

          if ( success )
          {
            type_code = qcril::interfaces::UssdModeType::NOTIFY;
            if ( orig_ussd_resp->uss_info.uss_data_len > 0)
            {
              //response_buff[ 1 ] = ussd_utf8_str;
            }
            else
            {
              ussd_utf8_str[0] = '\0';
            }
            /* Sending the response received from the network for the USSD request */
            QCRIL_LOG_DEBUG ("USSD Conf Success, data_len : %d", orig_ussd_resp->uss_info.uss_data_len);
            QCRIL_LOG_DEBUG ("USSD : type_code=%d, response_buff=%s, strlen=%d", type_code, ussd_utf8_str, strlen(ussd_utf8_str));
          }
          else
          {
             QCRIL_LOG_DEBUG ("USSD abort");
             type_code = qcril::interfaces::UssdModeType::NW_RELEASE;
             ussd_utf8_str[0] = '\0';
          }
          qcril_qmi_voice_send_unsol_ussd(domain, type_code, ussd_utf8_str, nullptr);
        }
        else
        {
          /* sending the unsol indication so that RIL can close the USSD session */
          if ( (orig_ussd_resp->failure_cause_valid == TRUE) &&
             (orig_ussd_resp->failure_cause == QMI_FAILURE_CAUSE_FACILITY_NOT_SUPPORTED_V02) ) /*facilityNotSupported*/
          {
            type_code = qcril::interfaces::UssdModeType::NOT_SUPPORTED;
          }
          else if (qmi_error == QMI_ERR_CARD_CALL_CONTROL_FAILED_V01)
          {
            type_code = qcril::interfaces::UssdModeType::LOCAL_CLIENT;
          }
          else if (qmi_error == QMI_ERR_ABORTED_V01)
          {
            // User cancelled the on going ussd session.
            // Not required to send the UNSOL_ON_USSD
            break;
          }
          else
          {
            type_code = qcril::interfaces::UssdModeType::NW_RELEASE;
          }
          QCRIL_LOG_DEBUG ("USSD Failure: type_code=%d",type_code);
          qcril_qmi_voice_send_unsol_ussd(domain, type_code, "", errorDetails);
        }
      }
    }
    else
    {
      if (responseCb)
      {
        CommonVoiceResponseData respData = {req_id, RIL_E_SYSTEM_ERR, orig_ussd_resp};
        responseCb(&respData);
      }
    }
  }while(0);
  qcril_qmi_voice_info.ussd_request_domain = qcril::interfaces::UssdDomain::UNKNOWN;
}/* qcril_qmi_voice_orig_ussd_resp_hdlr */

/*=========================================================================
  FUNCTION: qcril_qmi_voice_is_wps_call
===========================================================================*/
boolean qcril_qmi_voice_is_wps_call(const char* voice_dial_address)
{
  const char* WPS_CALL_PREFIX = "*272";
  return strncmp(voice_dial_address, WPS_CALL_PREFIX, strlen(WPS_CALL_PREFIX)) == 0;
} // qcril_qmi_voice_is_wps_call


//===========================================================================
// qcril_qmi_voice_is_non_std_otasp
//===========================================================================
boolean qcril_qmi_voice_is_non_std_otasp(const char * number_to_check)
{
#define NON_STD_OTASP_NUMBER "*228"
  return strncmp(number_to_check, NON_STD_OTASP_NUMBER, strlen(NON_STD_OTASP_NUMBER)) == 0;
} // qcril_qmi_voice_is_non_std_otasp


/*=========================================================================
  FUNCTION: qcril_qmi_voice_wps_call_over_cs
===========================================================================*/
boolean qcril_qmi_voice_wps_call_over_cs()
{
  auto msg = std::make_shared<QcRilGetCurrentMccMncSyncMessage>(true, true);
  assert(msg != nullptr);
  std::shared_ptr<QcRilGetMccMncType> shared_result;
  auto res = msg->dispatchSync(shared_result);
  if (res == Message::Callback::Status::SUCCESS && shared_result != nullptr)
  {
    if (shared_result->success) {
        QCRIL_LOG_INFO("qcril_qmi_voice_wps_call_over_cs: mcc %s, mnc %s",
                shared_result->mcc.c_str(), shared_result->mnc.c_str());
        return qcril_db_query_wps_call_over_cs(shared_result->mcc.c_str(),
                shared_result->mnc.c_str());
    }
  }

  return false;
} // qcril_qmi_voice_wps_call_over_cs

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_answer_ussd_resp_hdlr

===========================================================================*/
/*!
    @brief
    Handle Answer USSD RESP.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_answer_ussd_resp_hdlr
(
  uint16_t req_id,
  voice_answer_ussd_resp_msg_v02 *ans_ussd_resp,
  CommonVoiceResponseCallback responseCb
)
{
  qcril_instance_id_e_type instance_id;
  RIL_Errno ril_err;
//  qcril_request_resp_params_type resp;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01  qmi_error;


  instance_id = QCRIL_DEFAULT_INSTANCE_ID;


  if (ans_ussd_resp)
  {
  qmi_result = ans_ussd_resp->resp.result;
  qmi_error = ans_ussd_resp->resp.error;
  QCRIL_LOG_INFO("QCRIL QMI VOICE Answer USSD qmi_error : %d", qmi_error );

  if(qmi_result == QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_INFO("QCRIL QMI VOICE Answer USSD Success: user_act_req = %d",qcril_qmi_voice_info.ussd_user_action_required);
    qcril_qmi_voice_info.ussd_user_action_required = FALSE;
    ril_err = RIL_E_SUCCESS;
  }
  else
  {
    QCRIL_LOG_INFO("QCRIL QMI VOICE Answer USSD Failure: user_act_req = %d",qcril_qmi_voice_info.ussd_user_action_required);
    qcril_qmi_voice_info.ussd_user_action_required = FALSE;
    ril_err = qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error);
  }
  }
  else
  {
    ril_err = RIL_E_SYSTEM_ERR;
  }
  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, ril_err, ans_ussd_resp};
    responseCb(&respData);
  }
}/* qcril_qmi_voice_answer_ussd_resp_hdlr */

/*=========================================================================
  FUNCTION:  qcril_qmi_voice_cancel_ussd_resp_hdlr

===========================================================================*/
/*!
    @brief
    Handle Cancel USSD RESP.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_voice_cancel_ussd_resp_hdlr
(
  uint16_t req_id,
  voice_cancel_ussd_resp_msg_v02 *cancel_ussd_resp,
  CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01  qmi_error;


  if (cancel_ussd_resp)
  {
  qmi_result = cancel_ussd_resp->resp.result;
  qmi_error = cancel_ussd_resp->resp.error;
  QCRIL_LOG_INFO("QCRIL QMI VOICE Cancel USSD qmi_error : %d", qmi_error );

  if(qmi_result == QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_INFO("QCRIL QMI VOICE Cancel USSD Success: user_act_req = %d",qcril_qmi_voice_info.ussd_user_action_required);
    qcril_qmi_voice_info.ussd_user_action_required = FALSE;
    ril_err = RIL_E_SUCCESS;
  }
  else
  {
    QCRIL_LOG_INFO("QCRIL QMI VOICE Cancel USSD Failure: user_act_req = %d",qcril_qmi_voice_info.ussd_user_action_required);
    qcril_qmi_voice_info.ussd_user_action_required = FALSE;
    ril_err = qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error);
    /* Send FAILURE response */
  }
  }
  else
  {
    ril_err = RIL_E_SYSTEM_ERR;
  }
  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, ril_err, cancel_ussd_resp};
    responseCb(&respData);
  }
}/*qcril_qmi_voice_cancel_ussd_resp_hdlr*/

/*===========================================================================
  FUNCTION:  qcril_qmi_voice_get_emergency_flags_for_hlos_num
 *=========================================================================*/
RIL_Errno qcril_qmi_voice_get_emergency_flags_for_hlos_num
(
    std::shared_ptr<QcRilRequestDialMessage> dialMsg,
    char * /*number*/,
    int& /*is_emergency_flag*/,
    int& is_fake_ecc_flag,
    int& ril_enforce_ecc_flag
)
{
    RIL_Errno res = RIL_E_SUCCESS;
    QCRIL_LOG_FUNC_ENTRY();

    if (dialMsg != nullptr) {
        auto isTesting = dialMsg->hasIsForEccTesting() ? dialMsg->getIsForEccTesting() : false;
        auto routing = dialMsg->hasRouting() ? dialMsg->getRouting()
                                             : qcril::interfaces::EmergencyCallRouting::UNKNOWN;

        // If isTesting flag set, ignore everything else and mark as fake ecc
        if (isTesting) {
            is_fake_ecc_flag = TRUE;
            ril_enforce_ecc_flag = FALSE;
        } else if (routing == qcril::interfaces::EmergencyCallRouting::NORMAL) {
            is_fake_ecc_flag = TRUE;
            ril_enforce_ecc_flag = FALSE;
        } else if (routing == qcril::interfaces::EmergencyCallRouting::EMERGENCY) {
            is_fake_ecc_flag = FALSE;
            ril_enforce_ecc_flag = TRUE;
        } else if (routing == qcril::interfaces::EmergencyCallRouting::UNKNOWN) {
            is_fake_ecc_flag = FALSE;
            ril_enforce_ecc_flag = TRUE;
        }
    }
    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================
  FUNCTION:  qcril_qmi_voice_get_emergency_flags_for_ril_db_num
 *=========================================================================*/
RIL_Errno qcril_qmi_voice_get_emergency_flags_for_ril_db_num
(
    std::shared_ptr<QcRilRequestDialMessage> dialMsg,
    char *number,
    int& is_emergency_flag,
    int& is_fake_ecc_flag,
    int& ril_enforce_ecc_flag
)
{
    RIL_Errno res = RIL_E_SUCCESS;
    QCRIL_LOG_FUNC_ENTRY();

    if (dialMsg != nullptr) {
        auto isIntentionEcc = dialMsg->hasIsIntentionEcc() ? dialMsg->getIsIntentionEcc() : false;

        // RIL emergency number, check if this is fake emergency number
        auto is_display_ecc_num =
            qcril_qmi_voice_external_ecc_number_for_display_only(number);

        // If fake emergency number, no need to check further, break from this condition.
        if (is_display_ecc_num) {
            is_fake_ecc_flag = TRUE;
            ril_enforce_ecc_flag = FALSE;
            QCRIL_LOG_DEBUG("Display only emergency number");
        } else {
            // Check if this number is populated by RIL using nw mcc when network mcc, card mcc differs.
            auto is_nw_ecc_num =
                qcril_qmi_voice_external_designated_number_nw_mcc_roaming_emergency(number);
            if (is_nw_ecc_num) {
                if (isIntentionEcc) {
                    is_fake_ecc_flag = FALSE;
                    ril_enforce_ecc_flag = TRUE;
                } else {
                    // This is a special case, mark this as normal call.
                    is_emergency_flag = FALSE;
                    is_fake_ecc_flag = FALSE;
                    ril_enforce_ecc_flag = FALSE;
                }
            } else {
                is_fake_ecc_flag = FALSE;
                ril_enforce_ecc_flag = TRUE;
            }
        }
    }
    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================
  FUNCTION:  qcril_qmi_voice_get_emergency_flags_for_ril_pbm_num
 *=========================================================================*/
RIL_Errno qcril_qmi_voice_get_emergency_flags_for_pbm_num
(
    std::shared_ptr<QcRilRequestDialMessage> /*dialMsg*/,
    char * /*number*/,
    int& /*is_emergency_flag*/,
    int& is_fake_ecc_flag,
    int& ril_enforce_ecc_flag
)
{
    RIL_Errno res = RIL_E_SUCCESS;
    QCRIL_LOG_FUNC_ENTRY();

    // If this is ecc received from PBM, do not enforce as emergency
    // as modem should be able to resolve call.
    is_fake_ecc_flag = FALSE;
    ril_enforce_ecc_flag = FALSE;

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================
  FUNCTION:  qcril_qmi_voice_get_emergency_flags
 *=========================================================================*/
RIL_Errno qcril_qmi_voice_get_emergency_flags
(
    uint16_t req_id,
    char *number,
    int& is_emergency_flag,
    int& is_fake_ecc_flag,
    int& ril_enforce_ecc_flag
)
{
    RIL_Errno res = RIL_E_SUCCESS;
    enum { SOURCE_UNKNOWN, SOURCE_HLOS,SOURCE_RIL_DB,SOURCE_MODEM };
    int source = SOURCE_UNKNOWN;

    auto pendingMsg = findVoiceModulePendingMessage(req_id);
    do {

        if (pendingMsg == nullptr) {
            QCRIL_LOG_ERROR("dial request not found");
            res = RIL_E_GENERIC_FAILURE;
            break;
        }

        std::shared_ptr<QcRilRequestDialMessage> dialMsg =
            std::static_pointer_cast<QcRilRequestDialMessage>(pendingMsg);
        if (dialMsg == nullptr) {
            QCRIL_LOG_ERROR("failed to type cast to dial message");
            res = RIL_E_GENERIC_FAILURE;
            break;
        }

        auto isEmergency = dialMsg->hasIsEmergency() ? dialMsg->getIsEmergency() : false;
        auto isEmerNumMsg = std::make_shared<RilNumberIsEmergency>();
        std::shared_ptr<bool> respPtr = nullptr;
        bool is_from_ecc_list = false;
        if (isEmerNumMsg != nullptr) {
            isEmerNumMsg->setNumber(number);
            if (isEmerNumMsg->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS) {
                if (respPtr != nullptr) {
                is_from_ecc_list = *respPtr;
                }
            }
        }
        if (isEmergency || is_from_ecc_list) {
            is_emergency_flag = TRUE;
        } else {
            is_emergency_flag = FALSE;
            QCRIL_LOG_ESSENTIAL("Not an emergency number");
            break;
        }

        // Determine source of emergency number
        if ( !is_from_ecc_list ) {
            source = SOURCE_HLOS;
        } else {
            auto is_ril_ecc_num =
                qcril_qmi_voice_external_designated_number_enforcable_ecc_number(number);
            if (is_ril_ecc_num) {
                source = SOURCE_RIL_DB;
            } else {
                source = SOURCE_MODEM;
            }
        }

        switch (source) {
            case SOURCE_HLOS:
                qcril_qmi_voice_get_emergency_flags_for_hlos_num(dialMsg,
                    number, is_emergency_flag, is_fake_ecc_flag, ril_enforce_ecc_flag);
            break;

            case SOURCE_MODEM:
                qcril_qmi_voice_get_emergency_flags_for_pbm_num(dialMsg,
                    number, is_emergency_flag, is_fake_ecc_flag, ril_enforce_ecc_flag);
            break;

            case SOURCE_RIL_DB:
                qcril_qmi_voice_get_emergency_flags_for_ril_db_num(dialMsg,
                    number, is_emergency_flag, is_fake_ecc_flag, ril_enforce_ecc_flag);
            break;
        }
    } while(FALSE);

    QCRIL_LOG_ESSENTIAL("emer - %d, fake - %d, enforce - %d",
        is_emergency_flag, is_fake_ecc_flag, ril_enforce_ecc_flag);
    return res;
}

//===========================================================================
// qcril_qmi_voice_check_if_release_audio_possible
//===========================================================================
bool qcril_qmi_voice_check_if_release_audio_possible()
{
  bool ret = true;
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;

  int nof_inactive_calls = 0;
  int nof_calls =0 ;

  qcril_qmi_voice_voip_lock_overview();
  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while ( NULL != call_info_entry )
  {
    if ((VOICE_INVALID_CALL_ID != call_info_entry->android_call_id) &&
        ((VOICE_INVALID_CALL_ID != call_info_entry->qmi_call_id) ||
          (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED)))
    {
      nof_calls++;
      // Incoming call can be considered as inactive for audio release to handle MT + MT
      // or HOLD, MT + MT etc combinations
      if ( CALL_STATE_HOLD_V02 == call_info_entry->voice_scv_info.call_state ||
           CALL_STATE_INCOMING_V02 == call_info_entry->voice_scv_info.call_state ||
           CALL_STATE_WAITING_V02 == call_info_entry->voice_scv_info.call_state )
      {
        nof_inactive_calls++;
      }
    }
    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }
  qcril_qmi_voice_voip_unlock_overview();
  QCRIL_LOG_INFO(".. total calls - %d, num of inactive calls - %d", nof_calls, nof_inactive_calls);
  // It is possible that hold call is disconnected by other party. In these scenarios, there
  // might not be any calls. If there are calls, all calls state should be hold.
  if(nof_calls !=0 && nof_inactive_calls != nof_calls) {
    ret = false;
  }
  return ret;
} // qcril_qmi_voice_check_if_release_audio_possible



//===========================================================================
// qcril_qmi_voice_process_release_audio_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_release_audio_req
(
 uint16_t req_id,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    // If there are any active calls, we should send failure to other RIL
    if(!qcril_qmi_voice_check_if_release_audio_possible()) {
      break;
    }
    qcril_am_handle_event(QCRIL_AM_EVENT_RELEASE_AUDIO, NULL);
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_RELEASE_AUDIO_REQ_V02,
        nullptr,
        0,
        sizeof(voice_release_audio_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_release_audio_resp_msg_v02 *qmi_response =
              (voice_release_audio_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_release_audio_req */

/*===========================================================================
  FUNCTION:  qcril_qmi_voice_process_dial_call_req
 *=========================================================================*/
RIL_Errno qcril_qmi_voice_process_dial_call_req
(
 uint16_t req_id,
 voice_dial_call_req_msg_v02 &dial_call_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler,
 qcril_qmi_voice_voip_call_info_elaboration_type elaboration
)
{
  RIL_Errno call_setup_result = RIL_E_SUCCESS;
  int is_emergency_call = FALSE;
  int enforce_emergency = FALSE;
  int is_emer_num_to_ims_addr = FALSE;
  boolean dial_pended = FALSE;
  unsigned int escv_type = 0;
  boolean is_alt_emer_retry = FALSE;
  uint32_t nw_reg_status_overview;
  int rc = 0;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = NULL;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_cmd_params;
  uint32_t user_data;
  qcril_qmi_voice_voip_current_call_summary_type call_summary;
  qcril::interfaces::CallFailCause last_call_fail_cause = qcril::interfaces::CallFailCause::NORMAL;
  qcril_qmi_voice_emer_num_ims_addr_info_type emer_num_ims_addr_info;

  /*-----------------------------------------------------------------------*/
  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (!dial_call_req.conf_uri_list_valid)
    {
      int is_number_emergency = FALSE;
      int is_fake_emergency_num = FALSE;

      qcril_qmi_voice_get_emergency_flags(req_id, dial_call_req.calling_number,
        is_number_emergency, is_fake_emergency_num, enforce_emergency);

      if (TRUE == is_fake_emergency_num && qcril_qmi_voice_external_in_apm_leave_window())
      {
        // As this is for fake emergency number and APM has been turned off for emergency,
        // inform modem to come out of emergency scan mode and acquire normal service.
        auto ind_msg_ptr = std::make_shared<VoiceDialSetUpInd>();
        if (ind_msg_ptr != nullptr){
          ind_msg_ptr->setStatus(RIL_E_CANCELLED);
          ind_msg_ptr->broadcast();
        }

        if (!qcril_qmi_voice_external_has_normal_voice_call_capability())
        {
          dial_pended = qcril_qmi_voice_pend_emergency_call(req_id, dial_call_req, responseCb,
              commandOversightCompletionHandler, elaboration);
          // double check in case we miss the notification
          if (qcril_qmi_voice_external_has_normal_voice_call_capability())
          {
            qcril_qmi_voice_trigger_possible_pending_emergency_call();
          }
        }
      }
      if (TRUE == is_number_emergency &&
          FALSE == is_fake_emergency_num)
      {
        is_emergency_call = TRUE;
        if (enforce_emergency)
        {
          escv_type = qcril_qmi_voice_external_get_escv_type(dial_call_req.calling_number);
        }

        //IMS DIAL request - Convert the emergency number to IMS address If needed
        if ((elaboration & (QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN |
                            QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN)) &&
            (TRUE == enforce_emergency))
        {
          strlcpy(emer_num_ims_addr_info.emergency_number,
              dial_call_req.calling_number,
              sizeof(emer_num_ims_addr_info.emergency_number));

          auto dial_address = qcril_qmi_voice_external_convert_emergency_number_to_ims_address(
                dial_call_req.calling_number);
          strlcpy(emer_num_ims_addr_info.ims_address,
              dial_address.c_str(),
              sizeof(emer_num_ims_addr_info.ims_address));
          if (dial_address.compare(dial_call_req.calling_number))
          {
            is_emer_num_to_ims_addr = TRUE;
            strlcpy(dial_call_req.calling_number,
                dial_address.c_str(),
                sizeof(dial_call_req.calling_number));
          }
        }
      }
      auto nwDetectedEcc = get_network_detected_ecc_number_info();
      if (nwDetectedEcc)
      {
        if (nwDetectedEcc->number == dial_call_req.calling_number)
        {
          QCRIL_LOG_ESSENTIAL("Network detected emergency call retry case");
          is_alt_emer_retry = TRUE;
          is_emergency_call = TRUE;
          enforce_emergency = TRUE;
          escv_type = nwDetectedEcc->emerg_srv_categ;
          auto urn = nwDetectedEcc->urn;
          if (urn.size() && urn.size() < QMI_VOICE_EMERG_SERVICE_URN_MAX_LEN_V02)
          {
            dial_call_req.emerg_service_urn_valid = TRUE;
            dial_call_req.emerg_service_urn_len = urn.size();
            std::copy(urn.begin(), urn.end(), dial_call_req.emerg_service_urn);
          }
        }
        else
        {
          reset_network_detected_ecc_if_required(true);
        }
      }
    }

    /*-----------------------------------------------------------------------*/
    QCRIL_LOG_INFO("Starting MO voice call: requesting qxdm logging\n");
    rc = qcril_start_diag_log();
    if (rc == 0)
    {
      QCRIL_LOG_INFO("logging started successfully\n");
    }

    QCRIL_LOG_ESSENTIAL(".. is_emergency %d", is_emergency_call);
    QCRIL_LOG_ESSENTIAL(".. enforce_emergency %d", enforce_emergency);
    QCRIL_LOG_ESSENTIAL(".. is_emer_num_to_ims_addr %d", is_emer_num_to_ims_addr);
    QCRIL_LOG_ESSENTIAL(".. dial_pended %d", dial_pended);

#if 0
    // --FR47445-TODO--:
    if (TRUE == is_emergency_call)
    {
      call_setup_result = qcril_qmi_nas_voice_move_device_to_online_for_emer_call_conditionally();
      if (RIL_E_SUCCESS != call_setup_result)
      {
        QCRIL_LOG_DEBUG(".. unable to move device to online for emergency call");
        call_setup_result = RIL_E_INTERNAL_INVALID_STATE;
        break;
      }
    }
#endif

    nw_reg_status_overview = qcril_qmi_voice_external_get_reg_status_overview();
    QCRIL_LOG_ESSENTIAL(".. nw reg status overview %d", (int)nw_reg_status_overview);

    boolean is_wps_call_over_cs = qcril_qmi_voice_is_wps_call(dial_call_req.calling_number) &&
                                     qcril_qmi_voice_wps_call_over_cs();
    QCRIL_LOG_ESSENTIAL(".. is_wps_call_over_cs: %d", is_wps_call_over_cs);

    unsigned int call_radio_tech = qcril_qmi_voice_nas_control_get_reported_voice_radio_tech();
    unsigned int call_radio_tech_family =
      qcril_qmi_convert_radio_tech_to_radio_tech_family(call_radio_tech);
    boolean is_non_std_otasp = qcril_qmi_voice_is_non_std_otasp(dial_call_req.calling_number) &&
                                    (RADIO_TECH_3GPP2 == call_radio_tech_family);

    QCRIL_LOG_ESSENTIAL(".. is_non_std_otasp %d", is_non_std_otasp);

    qcril_qmi_voice_voip_lock_overview();

    if (dial_pended)
    {
      // Do nothing now
      call_setup_result = RIL_E_SUCCESS;
      break;
    }

    if (qmi_ril_voice_is_audio_inactive_vcl() && !is_emergency_call)
    {
      QCRIL_LOG_ESSENTIAL(".. audio is inactive; do not allow normal voice calls");
      call_setup_result = RIL_E_INTERNAL_INVALID_STATE;
      break;
    }

    qcril_qmi_voice_voip_generate_summary(&call_summary);

    if (call_summary.nof_voip_calls > 0 && is_wps_call_over_cs)
    {
      qcril_qmi_voice_hangup_all_calls_vcl(qcril_qmi_voice_call_to_atel, TRUE);
      // Do nothing now
      call_setup_result = RIL_E_SUCCESS;
      // Store the wps call info, and trigger the wps call after all VoIP calls are ended.
      qcril_qmi_voice_pending_wps_calls(req_id, dial_call_req, responseCb,
                                        commandOversightCompletionHandler, elaboration);
      break;
    }

    call_info_entry = qcril_qmi_voice_voip_create_call_info_entry(
        VOICE_INVALID_CALL_ID,
        INVALID_MEDIA_ID,
        TRUE,
        (elaboration | QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP),
        &call_setup_result);
    if (call_info_entry == NULL)
    {
      QCRIL_LOG_ESSENTIAL(".. unable to create call info entry");
      call_setup_result = RIL_E_INTERNAL_NO_MEMORY;
      break;
    }

    call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED;
    call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RIL_CALL_STATE_VALID;
    call_info_entry->ril_call_state = RIL_CALL_DIALING;
    if (is_alt_emer_retry)
    {
      qcril_qmi_voice_voip_call_info_set_single_elaboration(call_info_entry,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_NW_DETECTED_EME_CALL, TRUE);
    }

    if (!dial_call_req.conf_uri_list_valid)
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;
      call_info_entry->voice_svc_remote_party_number.number_pi = PRESENTATION_NUM_ALLOWED_V02;
      call_info_entry->voice_svc_remote_party_number.number_len =
        std::min(sizeof(call_info_entry->voice_svc_remote_party_number.number),
          strlen(dial_call_req.calling_number));
      memcpy(call_info_entry->voice_svc_remote_party_number.number,
          dial_call_req.calling_number,
          call_info_entry->voice_svc_remote_party_number.number_len);
      call_info_entry->voice_scv_info.is_mpty = FALSE;
    }
    else
    {
      call_info_entry->voice_scv_info.is_mpty = TRUE;
    }
    call_info_entry->voice_scv_info.direction = CALL_DIRECTION_MO_V02;
    call_info_entry->voice_scv_info.als = ALS_LINE1_V02;

    if( TRUE == is_emer_num_to_ims_addr )
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMER_NUM_TO_IMS_ADDR;
      memcpy(&call_info_entry->emer_num_ims_addr_info,
          &emer_num_ims_addr_info,
          sizeof(call_info_entry->emer_num_ims_addr_info));
    }

    command_oversight = qmi_ril_voice_ims_create_command_oversight(req_id, nullptr, 72,
                                commandOversightCompletionHandler);
    if (command_oversight == NULL)
    {
      QCRIL_LOG_ESSENTIAL(".. unable to create command oversight");
      call_setup_result = RIL_E_INTERNAL_NO_MEMORY;
      break;
    }

    qmi_ril_voice_ims_command_oversight_add_call_link(command_oversight,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP,
          VOICE_INVALID_CALL_ID,
          CALL_STATE_ORIGINATING_V02);
    memset(&oversight_cmd_params, 0, sizeof(oversight_cmd_params));
    oversight_cmd_params.locator.command_oversight = command_oversight;
    qmi_ril_voice_ims_command_oversight_handle_event(
          QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ,
          &oversight_cmd_params);

    if (is_emergency_call)
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERGENCY_CALL;
    }

    if (is_emergency_call && ((nw_reg_status_overview & QMI_RIL_NW_REG_VOICE_CALLS_AVAILABLE) &&
                              !(nw_reg_status_overview & QMI_RIL_NW_REG_FULL_SERVICE)))
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EME_FROM_OOS |
                                      QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
    }

    // non emergency dialed from OOS (specific to certain customer extensions)
    if (!is_emergency_call && !(nw_reg_status_overview & QMI_RIL_NW_REG_FULL_SERVICE))
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_DIAL_FROM_OOS |
                                      QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING;
    }
    std::string numStr =  PII(dial_call_req.calling_number,
                            hide_phone_number(dial_call_req.calling_number));
    QCRIL_LOG_INFO(".. Number sent %s", numStr.c_str());

    // store STK CC emulation overlay number. we will free it at DIAL_RESP if not needed
    size_t overlay_dial_num_len = strlen(dial_call_req.calling_number);
    call_info_entry->overlayed_number_storage_for_emulated_stk_cc =
        (char *)qcril_malloc(overlay_dial_num_len + 1);
    if (NULL != call_info_entry->overlayed_number_storage_for_emulated_stk_cc)
    {
      strlcpy(call_info_entry->overlayed_number_storage_for_emulated_stk_cc,
              dial_call_req.calling_number, overlay_dial_num_len + 1);
    }

    if (is_non_std_otasp)
    {
      dial_call_req.call_type_valid = TRUE;
      dial_call_req.call_type       = CALL_TYPE_NON_STD_OTASP_V02;
      //call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN;
    }

    if (is_wps_call_over_cs)
    {
      dial_call_req.call_type = CALL_TYPE_VOICE_V02;
      dial_call_req.service_type_valid = TRUE;
      dial_call_req.service_type = VOICE_DIAL_CALL_SRV_TYPE_CS_ONLY_V02;
    }

    if (CALL_TYPE_VT_V02 == dial_call_req.call_type ||
        CALL_TYPE_EMERGENCY_VT_V02 == dial_call_req.call_type)
    {
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUDIO_ATTR_VALID;
      call_info_entry->voice_audio_attrib.call_attributes =
          VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02;
      call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VIDEO_ATTR_VALID;
      call_info_entry->voice_video_attrib.call_attributes =
          VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02;
    }

    if (enforce_emergency)
    {
      dial_call_req.call_type_valid = TRUE;
      dial_call_req.call_type =
          (dial_call_req.call_type == CALL_TYPE_VT_V02 ? CALL_TYPE_EMERGENCY_VT_V02
                                                        : CALL_TYPE_EMERGENCY_V02);
      if (escv_type > 0)
      {
        dial_call_req.emer_cat_valid = TRUE;
        dial_call_req.emer_cat = escv_type;
      }
    }
    if (!(getVoiceModuleFeature(VoiceFeatureType::ENCRYPTED_CALLS_SUPPORT)))
    {
      dial_call_req.is_secure_call_valid = FALSE;
      dial_call_req.is_secure_call = FALSE;
    }

    QCRIL_LOG_DEBUG(".. final elaboration %x, %x hex",
        (uint32_t)(call_info_entry->elaboration >> 32),(uint32_t)call_info_entry->elaboration );
    QCRIL_LOG_DEBUG(".. call type set %d emer cat %x",
        (int)dial_call_req.call_type, dial_call_req.emer_cat);

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);
    qmi_client_error_type qmi_client_error = qmi_client_voice_send_async(
        QMI_VOICE_DIAL_CALL_REQ_V02,
        &dial_call_req,
        sizeof(voice_dial_call_req_msg_v02),
        sizeof(voice_dial_call_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_dial_call_resp_msg_v02 *qmi_response = nullptr;
          if (transp_err == QMI_NO_ERR) {
            qmi_response = (voice_dial_call_resp_msg_v02 *)(resp_c_struct.get());
          }
          qcril_qmi_voice_dial_call_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);
    call_setup_result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                        qmi_client_error, NULL);
  } while (FALSE);

  if (RIL_E_SUCCESS != call_setup_result)
  {
    // rollback
    QCRIL_LOG_INFO(".. rolling back with %d", (int) call_setup_result);
    last_call_fail_cause = qcril::interfaces::CallFailCause::NORMAL;

    if (call_info_entry)
    {
      qcril_qmi_voice_voip_destroy_call_info_entry(call_info_entry);
      call_info_entry = nullptr;
    }
    if (command_oversight)
    {
      qmi_ril_voice_ims_destroy_command_oversight(command_oversight);
    }
  }

  if(RIL_E_SUCCESS == call_setup_result && dial_pended != TRUE)
  {
    /* This unsol is to indicate DmsModule to cancel radio power wait
     * for dial request timer. Send this msgs only if DIAL request is success and dial
     * is not pending.
     */
    auto ind_msg_ptr =
         std::make_shared<VoiceDialSetUpInd>();
    if (ind_msg_ptr != nullptr)
    {
        ind_msg_ptr->setStatus(RIL_E_SUCCESS);
        ind_msg_ptr->broadcast();
    }
  }
  qcril_qmi_voice_voip_unlock_overview();

  if (qcril_qmi_voice_voip_call_info_entries_is_empty())
  {
      QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
      if (!qcril_stop_diag_log())
      {
          QCRIL_LOG_INFO("qxdm logging disabled successfully");
      }
  }

  qcril_qmi_voice_handle_new_last_call_failure_cause((call_end_reason_enum_v02)0,
      last_call_fail_cause, qcril_qmi_voice_map_ril_reason_to_str(last_call_fail_cause),
      call_info_entry);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(call_setup_result);

  return call_setup_result;
} /* qcril_qmi_voice_process_dial_call_req */

//===========================================================================
// qcril_qmi_voice_process_answer_call_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_answer_call_req
(
 uint16_t req_id,
 voice_answer_call_req_msg_v02 &ans_call_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = nullptr;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_cmd_params;
  call_state_enum_v02 target_call_state;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING, TRUE);
    if (NULL == call_info_entry)
    {
      QCRIL_LOG_INFO(".. pending incoming call record entry not found");
      ril_err = RIL_E_INVALID_STATE;
      break;
    }
    // Cancel if there is any auto answer timed callback
    if (TimeKeeper::no_timer == qmi_voice_voip_overview.auto_answer_timer_id)
    {
      QCRIL_LOG_INFO(".. Cancel Auto answer timed callback");
      TimeKeeper::getInstance().clear_timer(qmi_voice_voip_overview.auto_answer_timer_id);
      qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                                        req_id);

    command_oversight = qmi_ril_voice_ims_create_command_oversight(
        req_id, nullptr, 10, commandOversightCompletionHandler);
    if (NULL != command_oversight)
    {
      target_call_state = CALL_STATE_CONVERSATION_V02;
      if (ans_call_req.reject_call_valid && ans_call_req.reject_call)
      {
        target_call_state = CALL_STATE_END_V02;
      }
      qmi_ril_voice_ims_command_oversight_add_call_link(
          command_oversight, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE, call_info_entry->qmi_call_id, target_call_state);

      memset(&oversight_cmd_params, 0, sizeof(oversight_cmd_params));
      oversight_cmd_params.locator.command_oversight = command_oversight;

      qmi_ril_voice_ims_command_oversight_handle_event(
          QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_cmd_params);
    }

    // Send QMI VOICE ANSWER CALL REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_ANSWER_CALL_REQ_V02,
        &ans_call_req,
        sizeof(voice_answer_call_req_msg_v02),
        sizeof(voice_answer_call_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_answer_call_resp_msg_v02 *qmi_response = nullptr;
          if (transp_err == QMI_NO_ERR) {
            qmi_response = (voice_answer_call_resp_msg_v02 *)(resp_c_struct.get());
          }
          qcril_qmi_voice_answer_call_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);

    if (!(ans_call_req.reject_call_valid && ans_call_req.reject_call)) {
      if (call_info_entry) {
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL;
      }
    }
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_answer_call_req */

//===========================================================================
// qcril_qmi_voice_process_end_call_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_end_call_req
(
 uint16_t req_id,
 voice_end_call_req_msg_v02 &end_call_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler
)
{
  uint32_t user_data;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = NULL;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_cmd_params;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (commandOversightCompletionHandler != nullptr)
    {
      command_oversight = qmi_ril_voice_ims_create_command_oversight(
          req_id, nullptr, 72, commandOversightCompletionHandler);
      if (command_oversight == NULL)
      {
        ril_err = RIL_E_NO_MEMORY;
        break;
      }
    }

    qmi_ril_voice_ims_command_oversight_add_call_link(
        command_oversight, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID,
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE, end_call_req.call_id, CALL_STATE_END_V02);

    memset(&oversight_cmd_params, 0, sizeof(oversight_cmd_params));
    oversight_cmd_params.locator.command_oversight = command_oversight;

    qmi_ril_voice_ims_command_oversight_handle_event(
        QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND,
        QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_cmd_params);

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_END_CALL_REQ_V02,
        &end_call_req,
        sizeof(voice_end_call_req_msg_v02),
        sizeof(voice_end_call_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          //RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_end_call_resp_msg_v02 *qmi_response = nullptr;
          if (transp_err == QMI_NO_ERR) {
            qmi_response = (voice_end_call_resp_msg_v02 *)(resp_c_struct.get());
          }
          qcril_qmi_voice_end_call_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    if (command_oversight)
    {
      qmi_ril_voice_ims_destroy_command_oversight(command_oversight);
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_end_call_req */

//===========================================================================
// qcril_qmi_voice_process_setup_answer_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_setup_answer_req
(
 uint16_t req_id,
 voice_setup_answer_req_msg_v02 &setup_answer_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;
  qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
  qmi_ril_voice_ims_command_exec_oversight_type *command_oversight = nullptr;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_cmd_params;
  call_state_enum_v02 target_call_state;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state(
        CALL_STATE_SETUP_V02);
    if (call_info_entry == nullptr)
    {
      QCRIL_LOG_INFO("Setup calls not found");
      ril_err = RIL_E_INVALID_STATE;
      break;
    }
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                                        req_id);
    command_oversight = qmi_ril_voice_ims_create_command_oversight(
        req_id, nullptr, 90, commandOversightCompletionHandler);
    if (command_oversight != nullptr)
    {
      target_call_state = CALL_STATE_INCOMING_V02;
      if (setup_answer_req.reject_setup_valid && setup_answer_req.reject_setup)
      {
        target_call_state = CALL_STATE_END_V02;
      }
      qmi_ril_voice_ims_command_oversight_add_call_link(
          command_oversight, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE, call_info_entry->qmi_call_id, target_call_state);

      memset(&oversight_cmd_params, 0, sizeof(oversight_cmd_params));
      oversight_cmd_params.locator.command_oversight = command_oversight;

      qmi_ril_voice_ims_command_oversight_handle_event(
          QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND,
          QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &oversight_cmd_params);
    }

    // Send QMI VOICE SETUP ANSWER REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SETUP_ANSWER_REQ_V02,
        &setup_answer_req,
        sizeof(voice_setup_answer_req_msg_v02),
        sizeof(voice_setup_answer_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_setup_answer_resp_msg_v02 *qmi_response = nullptr;
          if (transp_err == QMI_NO_ERR) {
            qmi_response = (voice_setup_answer_resp_msg_v02 *)(resp_c_struct.get());
          }
          qcril_qmi_voice_setup_answer_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_setup_answer_req */

/*===========================================================================
  qcril_qmi_voice_process_start_cont_dtmf_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_start_cont_dtmf_req
(
 uint16_t req_id,
 voice_start_cont_dtmf_req_msg_v02 &start_cont_dtmf_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE IMS_CALL_CANCEL_REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_START_CONT_DTMF_REQ_V02,
        &start_cont_dtmf_req,
        sizeof(voice_start_cont_dtmf_req_msg_v02),
        sizeof(voice_start_cont_dtmf_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_start_cont_dtmf_resp_msg_v02 *qmi_response =
              (voice_start_cont_dtmf_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_start_cont_dtmf_req */

/*===========================================================================
  qcril_qmi_voice_process_stop_cont_dtmf_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_stop_cont_dtmf_req
(
 uint16_t req_id,
 voice_stop_cont_dtmf_req_msg_v02 &stop_cont_dtmf_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE IMS_CALL_CANCEL_REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_STOP_CONT_DTMF_REQ_V02,
        &stop_cont_dtmf_req,
        sizeof(voice_stop_cont_dtmf_req_msg_v02),
        sizeof(voice_stop_cont_dtmf_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_stop_cont_dtmf_resp_msg_v02 *qmi_response =
              (voice_stop_cont_dtmf_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_stop_cont_dtmf_req */

/*===========================================================================
  qcril_qmi_voice_process_burst_dtmf_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_burst_dtmf_req
(
 uint16_t req_id,
 voice_burst_dtmf_req_msg_v02 &burst_dtmf_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI_VOICE_BURST_DTMF_REQ_V02
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_BURST_DTMF_REQ_V02,
        &burst_dtmf_req,
        sizeof(voice_burst_dtmf_req_msg_v02),
        sizeof(voice_burst_dtmf_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_burst_dtmf_resp_msg_v02 *qmi_response =
              (voice_burst_dtmf_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_burst_dtmf_req */

/*===========================================================================
  qcril_qmi_voice_process_ims_call_cancel_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_ims_call_cancel_req
(
 uint16_t req_id,
 voice_ims_call_cancel_req_msg_v02 &call_cancel_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE IMS_CALL_CANCEL_REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_IMS_CALL_CANCEL_REQ_V02,
        &call_cancel_req,
        sizeof(voice_ims_call_cancel_req_msg_v02),
        sizeof(voice_ims_call_cancel_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_ims_call_cancel_resp_msg_v02 *qmi_response =
              (voice_ims_call_cancel_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_ims_call_cancel_req */

/*===========================================================================
  qcril_qmi_voice_process_send_flash_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_send_flash_req
(
 uint16_t req_id,
 voice_send_flash_req_msg_v02 &qmi_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE IMS_CALL_CANCEL_REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SEND_FLASH_REQ_V02,
        &qmi_req,
        sizeof(voice_send_flash_req_msg_v02),
        sizeof(voice_send_flash_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_send_flash_resp_msg_v02 *qmi_response =
              (voice_send_flash_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_send_flash_req */

/*===========================================================================
  qcril_qmi_voice_process_get_config_req
=========================================================================*/
RIL_Errno qcril_qmi_voice_process_get_config_req
(
 uint16_t req_id,
 voice_get_config_req_msg_v02 &get_config_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI_VOICE_GET_CONFIG_REQ_V02
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CONFIG_REQ_V02,
        &get_config_req,
        sizeof(voice_get_config_req_msg_v02),
        sizeof(voice_get_config_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_config_resp_msg_v02 *qmi_response =
              (voice_get_config_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_config_req */

/*===========================================================================
  qcril_qmi_voice_process_set_preferred_privacy_req
  =========================================================================*/
RIL_Errno qcril_qmi_voice_process_set_preferred_privacy_req
(
 uint16_t req_id,
 voice_set_preferred_privacy_req_msg_v02 &set_preferred_privacy_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI_VOICE_GET_CONFIG_REQ_V02
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SET_PREFERRED_PRIVACY_REQ_V02,
        &set_preferred_privacy_req,
        sizeof(voice_set_preferred_privacy_req_msg_v02),
        sizeof(voice_set_preferred_privacy_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_set_preferred_privacy_resp_msg_v02 *qmi_response =
              (voice_set_preferred_privacy_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_preferred_privacy_req */

//===========================================================================
// qcril_qmi_voice_request_last_call_fail_cause
//===========================================================================
void qcril_qmi_voice_request_last_call_fail_cause()
{
  qcril_qmi_voice_set_last_call_fail_request_timeout();

  qcril_qmi_voice_voip_lock_overview();
  qcril_qmi_voice_respond_ril_last_call_failure_request();
  qcril_qmi_voice_voip_unlock_overview();
} // qcril_qmi_voice_request_last_call_fail_cause

//===========================================================================
// qcril_qmi_voice_process_set_sups_service_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_set_sups_service_req
(
 uint16_t req_id,
 voice_set_sups_service_req_msg_v02 &set_sups_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE SET SUPS SERVICE REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SET_SUPS_SERVICE_REQ_V02,
        &set_sups_req,
        sizeof(voice_set_sups_service_req_msg_v02),
        sizeof(voice_set_sups_service_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_set_sups_service_resp_msg_v02 *qmi_response =
              (voice_set_sups_service_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response)
          {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_ident_valid,
                &qmi_response->alpha_ident,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb)
          {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_sups_service_req */

//===========================================================================
// qcril_qmi_voice_process_set_all_call_fwd_sups_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_set_all_call_fwd_sups_req
(
 uint16_t req_id,
 voice_set_all_call_fwd_sups_req_msg_v02 &set_all_call_fwd_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE SET ALL CALL FWD SUPS REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SET_ALL_CALL_FWD_SUPS_REQ_V02,
        &set_all_call_fwd_req,
        sizeof(voice_set_all_call_fwd_sups_req_msg_v02),
        sizeof(voice_set_all_call_fwd_sups_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_set_all_call_fwd_sups_resp_msg_v02 *qmi_response =
              (voice_set_all_call_fwd_sups_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response)
          {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_ident_valid,
                &qmi_response->alpha_ident,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb)
          {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_all_call_fwd_sups_req */

//===========================================================================
// qcril_qmi_voice_process_get_clip_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_clip_req
(
 uint16_t req_id,
 voice_get_clip_req_msg_v02 &/*clip_req*/,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CLIP_REQ_V02,
        NULL,
        0,
        sizeof(voice_get_clip_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_clip_resp_msg_v02 *qmi_response =
              (voice_get_clip_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_clip_req */

//===========================================================================
// qcril_qmi_voice_process_get_colp_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_colp_req
(
 uint16_t req_id,
 voice_get_colp_req_msg_v02 &/*colp_req*/,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_COLP_REQ_V02,
        NULL,
        0,
        sizeof(voice_get_colp_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_colp_resp_msg_v02 *qmi_response =
              (voice_get_colp_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_colp_req */

//===========================================================================
// qcril_qmi_voice_process_get_clir_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_clir_req
(
 uint16_t req_id,
 voice_get_clir_req_msg_v02 &/*clir_req*/,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CLIR_REQ_V02,
        NULL,
        0,
        sizeof(voice_get_clir_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_clir_resp_msg_v02 *qmi_response =
              (voice_get_clir_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_clir_req */

//===========================================================================
// qcril_qmi_voice_process_set_call_barring_password_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_set_call_barring_password_req
(
 uint16_t req_id,
 voice_set_call_barring_password_req_msg_v02 &set_cb_pwd_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_CALL_WAITING
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SET_CALL_BARRING_PASSWORD_REQ_V02,
        &set_cb_pwd_req,
        sizeof(voice_set_call_barring_password_req_msg_v02),
        sizeof(voice_set_call_barring_password_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_set_call_barring_password_resp_msg_v02 *qmi_response =
              (voice_set_call_barring_password_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_call_barring_password_req */


//===========================================================================
// qcril_qmi_voice_process_get_call_forwarding_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_call_forwarding_req
(
 uint16_t req_id,
 voice_get_call_forwarding_req_msg_v02 &get_cf_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_COLR_REG
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CALL_FORWARDING_REQ_V02,
        &get_cf_req,
        sizeof(voice_get_call_forwarding_req_msg_v02),
        sizeof(voice_get_call_forwarding_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_call_forwarding_resp_msg_v02 *qmi_response =
              (voice_get_call_forwarding_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_call_forwarding_req */

//===========================================================================
// qcril_qmi_voice_process_orig_ussd_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_orig_ussd_req
(
 uint16_t req_id,
 voice_orig_ussd_req_msg_v02 &orig_ussd_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_COLR_REG
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_ORIG_USSD_REQ_V02,
        &orig_ussd_req,
        sizeof(voice_orig_ussd_req_msg_v02),
        sizeof(voice_orig_ussd_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          (void)transp_err;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_orig_ussd_resp_msg_v02 *qmi_response =
              (voice_orig_ussd_resp_msg_v02 *)(resp_c_struct.get());
          qcril_qmi_voice_orig_ussd_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_orig_ussd_req */

//===========================================================================
// qcril_qmi_voice_process_answer_ussd_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_answer_ussd_req
(
 uint16_t req_id,
 voice_answer_ussd_req_msg_v02 &answer_ussd_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_COLR_REG
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_ANSWER_USSD_REQ_V02,
        &answer_ussd_req,
        sizeof(voice_answer_ussd_req_msg_v02),
        sizeof(voice_answer_ussd_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          (void)transp_err;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_answer_ussd_resp_msg_v02 *qmi_response =
              (voice_answer_ussd_resp_msg_v02 *)(resp_c_struct.get());
          qcril_qmi_voice_answer_ussd_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_answer_ussd_req */

//===========================================================================
// qcril_qmi_voice_process_cancel_ussd_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_cancel_ussd_req
(
 uint16_t req_id,
 voice_cancel_ussd_req_msg_v02 &/*cancel_ussd_req*/,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_COLR_REG
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_CANCEL_USSD_REQ_V02,
        nullptr,
        0,
        sizeof(voice_cancel_ussd_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          (void)transp_err;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_cancel_ussd_resp_msg_v02 *qmi_response =
              (voice_cancel_ussd_resp_msg_v02 *)(resp_c_struct.get());
          qcril_qmi_voice_cancel_ussd_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_cancel_ussd_req */

//===========================================================================
// qcril_qmi_voice_process_manage_calls_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_manage_calls_req
(
 uint16_t req_id,
 voice_manage_calls_req_msg_v02 &manage_calls_req,
 CommonVoiceResponseCallback responseCb,
 CommonVoiceResponseCallback commandOversightCompletionHandler
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (manage_calls_req.sups_type == SUPS_TYPE_RELEASE_HELD_OR_WAITING_V02) {
      qcril_qmi_voice_voip_lock_overview();
      qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
      qcril_qmi_voice_voip_call_info_entry_type *call_info_iter =
          qcril_qmi_voice_voip_call_info_entries_enum_first();
      while (NULL != call_info_iter) {
        if (VOICE_INVALID_CALL_ID != call_info_iter->android_call_id) {
          if ((CALL_STATE_WAITING_V02 == call_info_iter->voice_scv_info.call_state) ||
              (CALL_STATE_INCOMING_V02 == call_info_iter->voice_scv_info.call_state)) {
            call_info_entry = call_info_iter;
            break;
          }
          if (CALL_STATE_HOLD_V02 == call_info_iter->voice_scv_info.call_state) {
            call_info_entry = call_info_iter;
          }
        }
        call_info_iter = qcril_qmi_voice_voip_call_info_entries_enum_next();
      }
      qcril_qmi_voice_voip_unlock_overview();

      if (!call_info_entry) {
        ril_err = RIL_E_INVALID_STATE;
        break;
      }
      qmi_ril_voice_ims_command_exec_oversight_type *command_oversight =
          qmi_ril_voice_ims_create_command_oversight(req_id, nullptr, 72,
                                                     commandOversightCompletionHandler);
      if (NULL != command_oversight) {
        qmi_ril_voice_ims_command_oversight_add_call_link(
            command_oversight, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE, call_info_entry->qmi_call_id,
            CALL_STATE_END_V02);

        qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type oversight_cmd_params;
        memset(&oversight_cmd_params, 0, sizeof(oversight_cmd_params));
        oversight_cmd_params.locator.command_oversight = command_oversight;

        qmi_ril_voice_ims_command_oversight_handle_event(
            QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND,
            QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ,
            &oversight_cmd_params);
      }
    }
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_MANAGE_CALLS_REQ_V02,
        &manage_calls_req,
        sizeof(voice_manage_calls_req_msg_v02),
        sizeof(voice_manage_calls_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_manage_calls_resp_msg_v02 *qmi_response = nullptr;
          if (transp_err == QMI_NO_ERR) {
            qmi_response = (voice_manage_calls_resp_msg_v02 *)(resp_c_struct.get());
          }
          qcril_qmi_voice_manage_calls_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_manage_calls_req */


//===========================================================================
// qcril_qmi_voice_process_manage_ip_calls_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_manage_ip_calls_req
(
 uint16_t req_id,
 voice_manage_ip_calls_req_msg_v02 &manage_ip_calls_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_MANAGE_IP_CALLS_REQ_V02,
        &manage_ip_calls_req,
        sizeof(voice_manage_ip_calls_req_msg_v02),
        sizeof(voice_manage_ip_calls_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_manage_ip_calls_resp_msg_v02 *qmi_response =
              (voice_manage_ip_calls_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_MNG_IP_TXN, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_manage_ip_calls_req */


//===========================================================================
// qcril_qmi_voice_process_set_config_req_sync
//===========================================================================
RIL_Errno qcril_qmi_voice_process_set_config_req_sync
(
 uint16_t req_id,
 voice_set_config_req_msg_v02 &set_config_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  voice_set_config_resp_msg_v02 set_config_resp = {};

  QCRIL_LOG_FUNC_ENTRY();

  do {
    qmi_client_error_type transp_err = get_voice_modem_endpoint()->sendRawSync(
        QMI_VOICE_SET_CONFIG_REQ_V02,
        &set_config_req,
        sizeof(voice_set_config_req_msg_v02),
        (void *)(&set_config_resp),
        sizeof(voice_set_config_resp_msg_v02));
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
        transp_err, (qmi_response_type_v01 *)(&set_config_resp));

    QCRIL_LOG_INFO(".. qmi req got  %d", (int)ril_err);

    if (RIL_E_SUCCESS == ril_err) {
      if (responseCb) {
        CommonVoiceResponseData respData = {req_id, ril_err, &set_config_resp};
        responseCb(&respData);
      }
    }
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_config_req_sync */

//===========================================================================
// qcril_qmi_voice_process_get_call_waiting_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_call_waiting_req
(
 uint16_t req_id,
 voice_get_call_waiting_req_msg_v02 &get_cw_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_CALL_WAITING
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CALL_WAITING_REQ_V02,
        &get_cw_req,
        sizeof(voice_get_call_waiting_req_msg_v02),
        sizeof(voice_get_call_waiting_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_call_waiting_resp_msg_v02 *qmi_response =
              (voice_get_call_waiting_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_call_waiting_req */

//===========================================================================
// qcril_qmi_voice_process_get_colr_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_colr_req
(
 uint16_t req_id,
 voice_get_colr_req_msg_v02 &/*colr_req*/,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    // Send QMI VOICE GET_COLR_REG
    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_COLR_REQ_V02,
        NULL,
        0,
        sizeof(voice_get_colr_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_colr_resp_msg_v02 *qmi_response =
              (voice_get_colr_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_colr_req */

//===========================================================================
// qcril_qmi_voice_process_set_clir_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_set_clir_req
(
 uint32_t clir_n_param
)
{
  RIL_Errno ril_err = RIL_E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_DEBUG("SET_CLIR input = %d", clir_n_param);

  if ((clir_n_param == QCRIL_QMI_VOICE_SS_CLIR_PRESENTATION_INDICATOR) ||
      (clir_n_param == QCRIL_QMI_VOICE_SS_CLIR_INVOCATION_OPTION) ||
      (clir_n_param == QCRIL_QMI_VOICE_SS_CLIR_SUPPRESSION_OPTION)) {
    qcril_qmi_voice_info.clir = clir_n_param;

    /* Save CLIR setting to system property */
    static const property_id_type id_mapping[] = {PERSIST_VENDOR_RADIO_CLIR0,
            PERSIST_VENDOR_RADIO_CLIR1};
    static constexpr auto id_mapping_size = sizeof(id_mapping)/sizeof(id_mapping[0]);
    auto inst_id = static_cast<uint8_t>(qmi_ril_get_process_instance_id());
    if (inst_id <= id_mapping_size && qcril_config_set(
            id_mapping[inst_id], qcril_qmi_voice_info.clir) == E_SUCCESS)
    {
        QCRIL_LOG_DEBUG("Successfully save CLIR = %d", qcril_qmi_voice_info.clir);
    }
    else
    {
        QCRIL_LOG_ERROR("Failed to save CLIR = %d", qcril_qmi_voice_info.clir);
    }
  } else {
    QCRIL_LOG_ERROR("invalid params");
    ril_err = RIL_E_INVALID_ARGUMENTS;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_set_clir_req */


//===========================================================================
// qcril_qmi_voice_process_get_call_barring_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_get_call_barring_req
(
 uint16_t req_id,
 voice_get_call_barring_req_msg_v02 &get_cb_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_GET_CALL_BARRING_REQ_V02,
        &get_cb_req,
        sizeof(voice_get_call_barring_req_msg_v02),
        sizeof(voice_get_call_barring_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_get_call_barring_resp_msg_v02 *qmi_response =
              (voice_get_call_barring_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_voice_stk_ss_resp_handle(
                req_id,
                &qmi_response->resp,
                qmi_response->alpha_id_valid,
                &qmi_response->alpha_id,
                qmi_response->call_id_valid,
                qmi_response->call_id,
                qmi_response->cc_sups_result_valid,
                &qmi_response->cc_sups_result,
                qmi_response->cc_result_type_valid,
                &qmi_response->cc_result_type);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_get_call_barring_req */

//===========================================================================
// qcril_qmi_voice_process_indication_register_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_indication_register_req
(
 uint16_t req_id,
 voice_indication_register_req_msg_v02 &indication_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_INDICATION_REGISTER_REQ_V02,
        &indication_req,
        sizeof(voice_indication_register_req_msg_v02),
        sizeof(voice_indication_register_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_indication_register_resp_msg_v02 *qmi_response =
              (voice_indication_register_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
} /* qcril_qmi_voice_process_indication_register_req */


//===========================================================================
// qcril_qmi_voice_get_current_call_name
//===========================================================================
RIL_Errno qcril_qmi_voice_get_current_call_name
(
 const qcril_qmi_voice_voip_call_info_entry_type *const call_info_entry, // in
 std::string                                     &name,                  // out
 int                                             &name_presentation       // out
)
{
  RIL_Errno result            = RIL_E_SUCCESS;
  uint32_t    caller_name_len = 0;
  const char *caller_name_ptr = NULL;
  int         name_pi         = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
  char        ip_caller_name[(QMI_VOICE_IP_CALLER_NAME_MAX_LEN_V02 * 2)] = "\0";

  QCRIL_LOG_INFO("Enter");

  do {
    if (!call_info_entry)
    {
      QCRIL_LOG_INFO("Invalid parameters!");
      result = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    name_presentation = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;

    // ------------------------------------------------------------------------------------------
    // 1. Caller Name for IP Call - IMS specific and goes to name field and has the high priority
    // If IP call and ip_caller_name valid, then set the caller name in 'name' field
    // ------------------------------------------------------------------------------------------
    if ((call_info_entry->voice_scv_info.call_type == CALL_TYPE_VOICE_IP_V02 ||
         call_info_entry->voice_scv_info.call_type == CALL_TYPE_VT_V02 ||
         call_info_entry->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_IP_V02 ||
         call_info_entry->voice_scv_info.call_type == CALL_TYPE_EMERGENCY_VT_V02) &&
        (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IP_CALLER_NAME_VALID))
    {
      QCRIL_LOG_INFO("ip caller name len: %d, caller name:",
                     call_info_entry->ip_caller_name.ip_caller_name_len);

      // ip caller name TLV is valid. Set the name field
      caller_name_len = qcril_cm_ss_convert_ucs2_to_utf8(
                                       (const char *)call_info_entry->ip_caller_name.ip_caller_name,
                                       call_info_entry->ip_caller_name.ip_caller_name_len*2,
                                       ip_caller_name,
                                       sizeof(ip_caller_name));

      if (caller_name_len > 0)
      {
        caller_name_ptr = ip_caller_name;
        name_pi = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;
        break;
      }
    }

    if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN)
    {
    // ------------------------------------------------------------------------------------------
    // 2. Connected Party Number Extension
    // ------------------------------------------------------------------------------------------
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CONN_PARTY_IP_NUM_VALID)
      {
        caller_name_len = strlen(call_info_entry->voice_svc_conn_party_ip_num.conn_ip_num);
        caller_name_ptr = call_info_entry->voice_svc_conn_party_ip_num.conn_ip_num;
        name_pi = qcril_qmi_voice_map_qmi_to_ril_name_pi((pi_name_enum_v02)
                call_info_entry->voice_svc_conn_party_ip_num.conn_ip_num_pi);
        break;
      }

    // ------------------------------------------------------------------------------------------
    // 3. Connected Party Number
    // ------------------------------------------------------------------------------------------
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CONN_PARTY_NUM_VALID)
      {
        caller_name_len = call_info_entry->voice_svc_conn_party_num.conn_num_len;
        caller_name_ptr = call_info_entry->voice_svc_conn_party_num.conn_num;
        name_pi = qcril_qmi_voice_map_qmi_to_ril_name_pi((pi_name_enum_v02)
                call_info_entry->voice_svc_conn_party_num.conn_num_pi);
        break;
      }
    }

    // ------------------------------------------------------------------------------------------
    // 4. Remote Party Name
    // ------------------------------------------------------------------------------------------
    if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NAME_VALID)
    {
      QCRIL_LOG_INFO("remote name len %d, str %s",
              call_info_entry->voice_svc_remote_party_name.name_len,
              call_info_entry->voice_svc_remote_party_name.name);

      if (*(call_info_entry->voice_svc_remote_party_name.name) &&
              (call_info_entry->voice_svc_remote_party_name.name_len <
               QCRIL_QMI_VOICE_INTERCODING_BUF_LEN))
      {
        caller_name_len = call_info_entry->voice_svc_remote_party_name.name_len;
        caller_name_ptr = call_info_entry->voice_svc_remote_party_name.name;
        name_pi = qcril_qmi_voice_map_qmi_to_ril_name_pi(
                call_info_entry->voice_svc_remote_party_name.name_pi);
        break;
      }
      else
      {
        QCRIL_LOG_ERROR("remote party name is null, or "
                "remote party name len >= QCRIL_QMI_VOICE_INTERCODING_BUF_LEN");
      }
    }
  } while (FALSE);

  if (caller_name_ptr)
  {
    name = caller_name_ptr;
    name_presentation = name_pi;
    QCRIL_LOG_INFO("name = %s, name_pi = %d", name.c_str(), name_pi);
  }
  else
  {
    result = RIL_E_GENERIC_FAILURE;
  }
  QCRIL_LOG_INFO("Exit: result = %d", result);

  return result;
}

//===========================================================================
// qcril_qmi_voice_get_current_call_number
//===========================================================================
RIL_Errno qcril_qmi_voice_get_current_call_number
(
 const qcril_qmi_voice_voip_call_info_entry_type *const call_info_entry, // in
 std::string                                     &number,                // out
 int                                             &number_presentation,   // out
 std::string                                     &redir_number,          // out
 int                                             &redir_number_presentation //out
)
{
  RIL_Errno result           = RIL_E_SUCCESS;
  uint32_t number_len        = 0;
  const char    *caller_number_ptr = NULL;
  int      number_pi         = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
  char     rp_number[QMI_VOICE_SIP_URI_MAX_V02+QMI_VOICE_NUMBER_MAX_V02+1];

  QCRIL_LOG_INFO("Enter");

  do {
    if (!call_info_entry)
    {
      QCRIL_LOG_INFO("Invalid parameters!");
      result = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    number_presentation = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;

    // ------------------------------------------------------------------------------------------
    // 1. Connected Party Number
    // ------------------------------------------------------------------------------------------
    if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CONN_PARTY_NUM_VALID)
    {
      number_len = call_info_entry->voice_svc_conn_party_num.conn_num_len;
      caller_number_ptr = call_info_entry->voice_svc_conn_party_num.conn_num;
      number_pi = qcril_qmi_voice_map_qmi_to_ril_num_pi(
              call_info_entry->voice_svc_conn_party_num.conn_num_pi);
      break;
    }
    // ------------------------------------------------------------------------------------------
    // 2. Remote Party Number / Remote Party Number Extn
    // ------------------------------------------------------------------------------------------
    if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID ||
        call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_IP_NUMBER_VALID)
    {
      pi_num_enum_v02 rp_number_pi;
      memset(rp_number, 0, sizeof(rp_number));

      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_IP_NUMBER_VALID)
      {
        rp_number_pi = call_info_entry->voice_svc_remote_party_ip_number.ip_num_pi;
        number_len = strlen(call_info_entry->voice_svc_remote_party_ip_number.ip_num);
        memcpy(rp_number, call_info_entry->voice_svc_remote_party_ip_number.ip_num, number_len);
      }
      else
      {
        rp_number_pi = call_info_entry->voice_svc_remote_party_number.number_pi;
        number_len = call_info_entry->voice_svc_remote_party_number.number_len;
        strlcpy(rp_number,
                call_info_entry->voice_svc_remote_party_number.number,
                sizeof(rp_number));
      }

      if((QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMER_NUM_TO_IMS_ADDR & call_info_entry->elaboration) &&
         (CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_IP_V02 == call_info_entry->voice_scv_info.call_type ||
          CALL_TYPE_EMERGENCY_VT_V02 == call_info_entry->voice_scv_info.call_type))
      {
        //convert the ims emergency address back to corresponding emergency number If needed
        if(!strcmp(call_info_entry->emer_num_ims_addr_info.ims_address, rp_number))
        {
          strlcpy(rp_number,
                  call_info_entry->emer_num_ims_addr_info.emergency_number,
                  sizeof(rp_number));
          number_len = strlen(rp_number);
          rp_number_pi = PRESENTATION_NUM_ALLOWED_V02;
        }
      }

    // ------------------------------------------------------------------------------------------
    // 3. Redirecting Party Num
    // ------------------------------------------------------------------------------------------
      if (getVoiceModuleFeature(VoiceFeatureType::REDIR_PARTY_NUM_SUPPORT) &&
              call_info_entry->voice_svc_redirecting_party_num.num_len > 0)
      {
        rp_number[number_len] = '&';
        memcpy(&rp_number[number_len+1],
                call_info_entry->voice_svc_redirecting_party_num.num,
                call_info_entry->voice_svc_redirecting_party_num.num_len + 1);
        number_len += (call_info_entry->voice_svc_redirecting_party_num.num_len + 1);
      }
      caller_number_ptr = rp_number;
      number_pi = qcril_qmi_voice_map_qmi_to_ril_num_pi(rp_number_pi);

      if ((call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_REDIRECTING_PARTY_NUM_VALID)
            && (call_info_entry->voice_svc_redirecting_party_num.num_len > 0))
      {
        redir_number = std::string(call_info_entry->voice_svc_redirecting_party_num.num,
                                   call_info_entry->voice_svc_redirecting_party_num.num_len);
        redir_number_presentation = qcril_qmi_voice_map_qmi_to_ril_num_pi(
            call_info_entry->voice_svc_redirecting_party_num.num_pi);
      }
    }
  } while (FALSE);

  if (caller_number_ptr)
  {
    number = caller_number_ptr;
    number_presentation = number_pi;
    std::string numStr = PII(number,hide_phone_number(number.c_str()));
    QCRIL_LOG_INFO("number = %s, number_pi = %d", numStr.c_str(), number_pi);
  }
  else
  {
    result = RIL_E_GENERIC_FAILURE;
  }
  QCRIL_LOG_INFO("Exit: result = %d", result);

  return result;
}

RIL_AudioQuality convertToRil(voice_speech_codec_enum_v02 speechCodec)
{
    RIL_AudioQuality ret = RIL_AUDIO_QUAL_UNSPECIFIED;
    switch(speechCodec) {
        case VOICE_SPEECH_CODEC_AMR_NB_V02:
            ret = RIL_AUDIO_QUAL_AMR;
            break;
        case VOICE_SPEECH_CODEC_AMR_WB_V02:
            ret = RIL_AUDIO_QUAL_AMR_WB;
            break;
        case VOICE_SPEECH_CODEC_GSM_EFR_V02:
            ret = RIL_AUDIO_QUAL_GSM_EFR;
            break;
        case VOICE_SPEECH_CODEC_GSM_FR_V02:
            ret = RIL_AUDIO_QUAL_GSM_FR;
            break;
        case VOICE_SPEECH_CODEC_GSM_HR_V02:
            ret = RIL_AUDIO_QUAL_GSM_HR;
            break;
        case VOICE_SPEECH_CODEC_EVRC_V02:
            ret = RIL_AUDIO_QUAL_EVRC;
            break;
        case VOICE_SPEECH_CODEC_EVRC_B_V02:
            ret = RIL_AUDIO_QUAL_EVRC_B;
            break;
        case VOICE_SPEECH_CODEC_EVRC_WB_V02:
            ret = RIL_AUDIO_QUAL_EVRC_WB;
            break;
        case VOICE_SPEECH_CODEC_EVRC_NW_V02:
            ret = RIL_AUDIO_QUAL_EVRC_NW;
            break;
        default:
            ret = RIL_AUDIO_QUAL_UNSPECIFIED;
    }
    return ret;
}

qcril::interfaces::AudioCodec qcril_qmi_map_audio_codec(voice_speech_codec_enum_v02 speechCodec) {
  switch (speechCodec) {
    case VOICE_SPEECH_CODEC_QCELP13K_V02:
      return qcril::interfaces::AudioCodec::QCELP13K;
    case VOICE_SPEECH_CODEC_EVRC_V02:
      return qcril::interfaces::AudioCodec::EVRC;
    case VOICE_SPEECH_CODEC_EVRC_B_V02:
      return qcril::interfaces::AudioCodec::EVRC_B;
    case VOICE_SPEECH_CODEC_EVRC_WB_V02:
      return qcril::interfaces::AudioCodec::EVRC_WB;
    case VOICE_SPEECH_CODEC_EVRC_NW_V02:
      return qcril::interfaces::AudioCodec::EVRC_NW;
    case VOICE_SPEECH_CODEC_AMR_NB_V02:
      return qcril::interfaces::AudioCodec::AMR_NB;
    case VOICE_SPEECH_CODEC_AMR_WB_V02:
      return qcril::interfaces::AudioCodec::AMR_WB;
    case VOICE_SPEECH_CODEC_GSM_EFR_V02:
      return qcril::interfaces::AudioCodec::GSM_EFR;
    case VOICE_SPEECH_CODEC_GSM_FR_V02:
      return qcril::interfaces::AudioCodec::GSM_FR;
    case VOICE_SPEECH_CODEC_GSM_HR_V02:
      return qcril::interfaces::AudioCodec::GSM_HR;
    case VOICE_SPEECH_CODEC_G711U_V02:
      return qcril::interfaces::AudioCodec::G711U;
    case VOICE_SPEECH_CODEC_G723_V02:
      return qcril::interfaces::AudioCodec::G723;
    case VOICE_SPEECH_CODEC_G711A_V02:
      return qcril::interfaces::AudioCodec::G711A;
    case VOICE_SPEECH_CODEC_G722_V02:
      return qcril::interfaces::AudioCodec::G722;
    case VOICE_SPEECH_CODEC_G711AB_V02:
      return qcril::interfaces::AudioCodec::G711AB;
    case VOICE_SPEECH_CODEC_G729_V02:
      return qcril::interfaces::AudioCodec::G729;
    case VOICE_SPEECH_CODEC_EVS_NB_V02:
      return qcril::interfaces::AudioCodec::EVS_NB;
    case VOICE_SPEECH_CODEC_EVS_WB_V02:
      return qcril::interfaces::AudioCodec::EVS_WB;
    case VOICE_SPEECH_CODEC_EVS_SWB_V02:
      return qcril::interfaces::AudioCodec::EVS_SWB;
    case VOICE_SPEECH_CODEC_EVS_FB_V02:
      return qcril::interfaces::AudioCodec::EVS_FB;
    case VOICE_SPEECH_CODEC_NONE_V02:
    default:
      return qcril::interfaces::AudioCodec::NONE;
  }
}

//===========================================================================
// qcril_qmi_voice_gather_current_call_information
//===========================================================================
RIL_Errno qcril_qmi_voice_gather_current_call_information(
    qcril::interfaces::CallInfo& callInfo,                                 /* out */
    const qcril_qmi_voice_voip_call_info_entry_type* const call_info_entry /* in */
)
{
  QCRIL_LOG_FUNC_ENTRY();
  if (!call_info_entry)
  {
    QCRIL_LOG_ERROR("Null pointer: call_info_entry %p", call_info_entry);
    return RIL_E_INVALID_ARGUMENTS;
  }

  // call state
  RIL_CallState call_state = call_info_entry->ril_call_state;
  if (call_info_entry->voice_scv_info.call_state == CALL_STATE_END_V02)
  {
    call_state = RIL_CALL_END;
  }
  else if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EXTENDED_DIALING)
  {
    call_state = RIL_CALL_DIALING;
  }
  callInfo.setCallState(convertCallState(call_state));
  // call id
  callInfo.setIndex(call_info_entry->android_call_id);
  // Number & Number Presentation
  std::string number;
  int numberPresentation = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
  std::string redirNum;
  int redirNumPresentation = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
  qcril_qmi_voice_get_current_call_number(
      call_info_entry, number, numberPresentation, redirNum, redirNumPresentation);
  callInfo.setNumber(number);
  callInfo.setNumberPresentation(numberPresentation);
  callInfo.setRedirNum(redirNum);
  callInfo.setRedirNumPresentation(redirNumPresentation);
  // TOA
  if (number.length() > 1)
  {
    callInfo.setToa((number[0] == QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER_PREFIX)
                        ? QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER
                        : QCRIL_QMI_VOICE_DOMESTIC_NUMBER);
  }
  // Name & Name Presentation
  std::string name;
  int namePresentation = QCRIL_QMI_VOICE_RIL_PI_UNKNOWN;
  qcril_qmi_voice_get_current_call_name(call_info_entry, name, namePresentation);
  callInfo.setName(name);
  callInfo.setNamePresentation(namePresentation);
  // is multiparty
  callInfo.setIsMpty(call_info_entry->voice_scv_info.is_mpty);
  // is mobile terminated
  callInfo.setIsMt(call_info_entry->voice_scv_info.direction == CALL_DIRECTION_MT_V02);
  // ALS
  callInfo.setAls(call_info_entry->voice_scv_info.als == ALS_LINE2_V02);
  // privacy
  callInfo.setIsVoice(false);
  if (call_info_entry->voice_scv_info.call_type != CALL_TYPE_SUPS_V02)
  {
    callInfo.setIsVoice(true);
    if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VOICE_PRIVACY_VALID)
    {
      callInfo.setIsVoicePrivacy(call_info_entry->voice_svc_voice_privacy ==
                                 VOICE_PRIVACY_ENHANCED_V02);
    }
  }
  // Call details
  qcril::interfaces::CallType call_type = qcril::interfaces::CallType::UNKNOWN;
  qcril::interfaces::CallDomain call_domain = qcril::interfaces::CallDomain::UNKNOWN;
  uint32_t call_sub_state = 0;
  boolean ret = qcril_qmi_voice_get_atel_call_type_info(
      call_info_entry->voice_scv_info.call_type,
      (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VIDEO_ATTR_VALID ? TRUE
                                                                                         : FALSE),
      call_info_entry->voice_video_attrib.call_attributes,
      (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUDIO_ATTR_VALID ? TRUE
                                                                                         : FALSE),
      call_info_entry->voice_audio_attrib.call_attributes,
      (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ATTR_VALID ? TRUE
                                                                                        : FALSE),
      call_info_entry->call_attrib_status.call_attrib_status,
      TRUE,
      call_info_entry->elaboration,
      call_info_entry->answered_call_type,
      call_type,
      call_domain,
      call_sub_state);
  callInfo.setCallType(call_type);
  callInfo.setCallDomain(call_domain);
  callInfo.setCallSubState(call_sub_state);
  // uus
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_UUS_VALID)
  {
    auto uusInfo = std::make_shared<qcril::interfaces::UusInfo>();
    if (uusInfo)
    {
      uusInfo->setType(static_cast<RIL_UUS_Type>(call_info_entry->voice_svc_uus.uus_type));
      uusInfo->setDcs(static_cast<RIL_UUS_DCS>(call_info_entry->voice_svc_uus.uus_dcs));
      uusInfo->setData(
          std::string(reinterpret_cast<const char*>(call_info_entry->voice_svc_uus.uus_data),
                      call_info_entry->voice_svc_uus.uus_data_len));
    }
    callInfo.setUusInfo(uusInfo);
  }
  // codec
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CODEC_VALID)
  {
    callInfo.setAudioQuality(convertToRil(call_info_entry->codec));
  }
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CODEC_VALID)
  {
    callInfo.setCodec(qcril_qmi_map_audio_codec(call_info_entry->codec));
  }
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_COMPUTED_AUDIO_QUALITY_VALID))
  {
    callInfo.setComputedAudioQuality(call_info_entry->computed_audio_quality);
  }
  // LocalAbility
  std::vector<qcril::interfaces::ServiceStatusInfo> localAbility =
      qcril_qmi_ims_translate_ril_callcapabilities_to_ServiceStatusInfo(
          (call_info_entry->elaboration &
           QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LOCAL_CALL_CAPBILITIES_VALID),
          call_info_entry->local_call_capabilities_info,
          (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_CAP_INFO_VALID),
          call_info_entry->rtt_capabilities_info.rtt_capabilities.local_rtt_cap,
          callInfo.getCallType(),
          call_info_entry->voice_scv_info.mode);
  callInfo.setLocalAbility(localAbility);
  // PeerAbility
  if ((call_info_entry->elaboration &
       QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PEER_CALL_CAPBILITIES_VALID) ||
      (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_CAP_INFO_VALID))
  {
    std::vector<qcril::interfaces::ServiceStatusInfo> peerAbility =
        qcril_qmi_ims_translate_ril_callcapabilities_to_ServiceStatusInfo(
            (call_info_entry->elaboration &
             QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PEER_CALL_CAPBILITIES_VALID),
            call_info_entry->peer_call_capabilities_info,
            (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_CAP_INFO_VALID),
            call_info_entry->rtt_capabilities_info.rtt_capabilities.peer_rtt_cap,
            (call_info_entry->elaboration &
             QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PEER_CALL_CAPBILITIES_VALID)
                ? qcril::interfaces::CallType::UNKNOWN
                : callInfo.getCallType(),
            CALL_MODE_NO_SRV_V02);
    callInfo.setPeerAbility(peerAbility);
  }
  // RttMode
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_MODE_VALID)
  {
    callInfo.setRttMode(
        qcril_qmi_ims_map_qcril_rtt_mode_to_ims(call_info_entry->rtt_mode.rtt_mode));
  }
  // Media id
  callInfo.setMediaId(call_info_entry->media_id);
  // Encryption
  callInfo.setIsEncrypted(call_info_entry->is_secure_call);
  // History info
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HISTORY_INFO_VALID)
  {
    char history_info_utf8[QMI_VOICE_IP_HIST_INFO_MAX_LEN_V02 * 2] = "\0";
    qcril_cm_ss_convert_ucs2_to_utf8((const char*)call_info_entry->history_info.ip_hist_info,
                                     call_info_entry->history_info.ip_hist_info_len * 2,
                                     history_info_utf8,
                                     sizeof(history_info_utf8));
    callInfo.setHistoryInfo(history_info_utf8);
  }
  // Diversion info
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_DIVERSION_INFO_VALID))
  {
    char diversion_info_utf8[QMI_VOICE_IP_DIVERSION_INFO_MAX_LEN_V02 * 2] = "\0";
    qcril_cm_ss_convert_ucs2_to_utf8(
        (const char*)call_info_entry->diversion_info.ip_diversion_info,
        call_info_entry->diversion_info.ip_diversion_info_len * 2,
        diversion_info_utf8,
        sizeof(diversion_info_utf8));
    callInfo.setDiversionInfo(diversion_info_utf8);
  }
  // ChildNumber
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_CHILD_NUMBER_VALID)
  {
    callInfo.setChildNumber(call_info_entry->child_number.number);
  }
  // DisplayText
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_DISPLAY_TEXT_VALID)
  {
    char display_text[(QMI_VOICE_DISPLAY_TEXT_MAX_LEN_V02 * 2)] = "\0";
    qcril_cm_ss_convert_ucs2_to_utf8((const char*)call_info_entry->display_text.display_text,
                                     call_info_entry->display_text.display_text_len * 2,
                                     display_text,
                                     sizeof(display_text));
    callInfo.setDisplayText(display_text);
  }
  // AdditionalCallInfo
  if (qcril_qmi_voice_is_additional_call_info_available(call_info_entry))
  {
    std::string additional_call_info((const char*)call_info_entry->additional_call_info.buffer,
                                     call_info_entry->additional_call_info.total_size);
    callInfo.setAdditionalCallInfo(additional_call_info);
  }
  // EmergencyServiceCategory
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERG_SRV_CATEG_VALID)
  {
    callInfo.setEmergencyServiceCategory(call_info_entry->emerg_srv_categ);
  }
  // lcf
  if (call_info_entry->lcf_valid)
  {
    auto failCause = std::make_shared<qcril::interfaces::CallFailCauseResponse>();
    if (failCause)
    {
      // sip error code
      bool sip_error_code_valid =
          (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_SIP_ERROR_CODE_VALID);
      uint32_t sip_error_code = 0;
      if (sip_error_code_valid)
      {
        sip_error_code = call_info_entry->sip_error_code.sip_error_code;
      }
      failCause->setFailCause(
          qcril_qmi_ims_map_ril_failcause_to_ims_failcause(call_info_entry->lcf,
                                                           call_info_entry->lcf_extended_codes,
                                                           sip_error_code_valid,
                                                           sip_error_code));
      failCause->setExtendedFailCause(call_info_entry->lcf_extended_codes);
      // end_reason_text
      std::string end_reason{};
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_END_REASON_TEXT_VALID)
      {
        char end_reason_text[QMI_VOICE_END_REASON_TEXT_MAX_LEN_V02 * 2];
        QCRIL_LOG_INFO("end_reason_text_len: %d, end_reason_text (UTF-16):",
                       call_info_entry->end_reason_text.end_reason_text_len);
        auto end_reason_text_len = qcril_cm_ss_convert_ucs2_to_utf8(
            (const char*)call_info_entry->end_reason_text.end_reason_text,
            call_info_entry->end_reason_text.end_reason_text_len * 2,
            end_reason_text,
            sizeof(end_reason_text));
        if (end_reason_text_len > 0)
        {
          QCRIL_LOG_INFO("end_reason_text (UTF-8): %s", end_reason_text);
          end_reason = end_reason_text;
        }
      }
      if (!end_reason.empty())
      {
        failCause->setNetworkErrorString(end_reason);
      }
      if (sip_error_code_valid || !end_reason.empty())
      {
        auto errorDetails = std::make_shared<qcril::interfaces::SipErrorInfo>();
        if (errorDetails)
        {
          if (sip_error_code_valid)
          {
            errorDetails->setErrorCode(sip_error_code);
          }
          if (!end_reason.empty())
          {
            errorDetails->setErrorString(end_reason);
          }
          failCause->setErrorDetails(errorDetails);
        }
      }
      callInfo.setCallFailCauseResponse(failCause);
    }
  }
  // sip alternate uri
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALTERNATE_SIP_URI_VALID)
  {
    callInfo.setSipAlternateUri(call_info_entry->alternate_sip_uri.sip_uri);
  }
  // Call Modified Cause
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MOD_CAUSE_VALID)
  {
    QCRIL_LOG_INFO("call_modified_cause = %d\n", call_info_entry->call_modified_cause);
    callInfo.setCauseCode(
        qcril_qmi_map_call_modified_cause_to_ims(call_info_entry->call_modified_cause));
  }
  // Is called party ringing at remote end
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_CALLED_PARTY_RING_VALID)
  {
    QCRIL_LOG_INFO("is_called_party_ringing = %d\n",
                   call_info_entry->is_called_party_ringing.is_called_party_ringing);
    callInfo.setIsCalledPartyRinging(
        call_info_entry->is_called_party_ringing.is_called_party_ringing);
  }
  // Remote Party Conf Capability
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_CONF_CAP_VALID)
  {
    QCRIL_LOG_INFO("remote_party_conf_capability->conf_is_enabled = %d\n",
                   call_info_entry->remote_party_conf_capability.conf_is_enabled);
    callInfo.setIsVideoConfSupported(
        call_info_entry->remote_party_conf_capability.conf_is_enabled);
  }
  // Verstat info
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VERSTAT_INFO_VALID)
  {
    QCRIL_LOG_INFO(
        "verstat_info->can_mark_unwanted_call = %d, verstat_info->validataion_status = %d\n",
        call_info_entry->verstat_info.network_supports_unwanted_call,
        call_info_entry->verstat_info.mt_call_verstat_info);
    auto verStatInfo = qcril_qmi_map_verstat_info_to_ims(call_info_entry->verstat_info);
    callInfo.setVerstatInfo(verStatInfo);
  }
  // terminating number info
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TERMINATING_NUM_VALID)
  {
    callInfo.setTerminatingNumber(call_info_entry->terminating_num);
    callInfo.setIsSecondary(call_info_entry->is_secondary);
    QCRIL_LOG_INFO("terminating_num = %s, is_secondary = %d\n",
                   call_info_entry->terminating_num,
                   call_info_entry->is_secondary);
  }
  // Tir mode only for MT calls
  if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TIR_MODE_VALID)
  {
    callInfo.setTirMode(qcril_qmi_map_tir_mode_to_ims(call_info_entry->tir_mode));
    QCRIL_LOG_INFO("tir_mode = %d", call_info_entry->tir_mode);
  }
  // Silent ui for CRS calls
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_SILENT_UI_VALID))
  {
    callInfo.setSilentUi(call_info_entry->silent_ui);
    QCRIL_LOG_INFO("silent_ui = %d", call_info_entry->silent_ui);
  }
  // Original call type for CRS calls
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_ORIGINAL_CALL_TYPE_VALID))
  {
    callInfo.setOriginalCallType(
        map_qmi_call_type_to_ril_type(call_info_entry->original_call_type));
    QCRIL_LOG_INFO("original_call_type = %d", call_info_entry->original_call_type);
  }
  // CRS type for CRS calls
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_CRS_TYPE_VALID))
  {
    callInfo.setCrsType(qcril_qmi_map_crs_type_to_ims(call_info_entry->crs_type));
    QCRIL_LOG_INFO("crs_type = %d", call_info_entry->crs_type);
  }
  // Call progress notification info
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_PROG_INFO_NOTIF_VALID))
  {
    qcril::interfaces::CallProgressInfo prog_info_notif{};
    prog_info_notif.setType(qcril_qmi_map_call_prog_info_notif_type(
                                      call_info_entry->prog_info_notif.notif_type));
    prog_info_notif.setReasonCode(call_info_entry->prog_info_notif.notif_reason);
    prog_info_notif.setReasonText(call_info_entry->prog_info_notif.notif_text);

    callInfo.setProgressInfo(prog_info_notif);
    QCRIL_LOG_INFO("prog_info_notif = %s",
                   qcril::interfaces::toString(prog_info_notif).c_str());
  }
  // Msim additional call information
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_MSIM_ADDITIONAL_CALL_INFO_VALID))
  {
    callInfo.setMsimAdditionalCallInfo(call_info_entry->msim_additional_call_info);
    QCRIL_LOG_INFO(
        "msim_additional_call_info = %s",
        qcril::interfaces::toString(call_info_entry->msim_additional_call_info).c_str());
  }
  // modem call id
  callInfo.setModemCallId(call_info_entry->qmi_call_id);
  // Vos support
  if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_VOS_SUPPORT_VALID))
  {
    callInfo.setVosSupport(call_info_entry->vos_support);
    QCRIL_LOG_INFO("vos_support = %d", call_info_entry->vos_support);
  }

  if (call_info_entry->call_reason != nullptr)
  {
    callInfo.setCallReason(std::string(call_info_entry->call_reason));
  }
  QCRIL_LOG_FUNC_RETURN();
  return RIL_E_SUCCESS;
}  // qcril_qmi_voice_gather_current_call_information

//===========================================================================
// qcril_qmi_get_call_list_to_send
//===========================================================================
RIL_Errno qcril_qmi_get_call_list_to_send
(
   qcril::interfaces::CallDomain callDomain, /* in */
   std::vector<qcril::interfaces::CallInfo> &calls  /* out */
)
{
  RIL_Errno ril_req_res = RIL_E_SUCCESS;
  int need_consider_voice_call_obj_cleanup = FALSE;
  uint32_t num_of_calls = 0;
  int   log_nof_skipped_calls = 0;
  char  log_essence[ QCRIL_MAX_LOG_MSG_SIZE ];
  char  log_addon[ QCRIL_MAX_LOG_MSG_SIZE ];

  QCRIL_LOG_FUNC_ENTRY();

  snprintf(log_essence, QCRIL_MAX_LOG_MSG_SIZE, "RILVI: calls rep:");

  qcril_qmi_voice_voip_lock_overview();

  QCRIL_LOG_INFO( "iteration through call objects start" );
  qcril_qmi_voice_voip_call_info_entry_type* call_info_entry =
            qcril_qmi_voice_voip_call_info_entries_enum_first();
  while (call_info_entry)
  {
    qcril_qmi_voice_voip_call_info_dump(call_info_entry);
    bool report_call = false;
    do {
      // Call without valid android_call_id
      if (call_info_entry->android_call_id == VOICE_INVALID_CALL_ID)
      {
        QCRIL_LOG_INFO("No valid android call id");
        break;
      }
      // Call without a valid qmi call ID
      if (!((call_info_entry->qmi_call_id != VOICE_INVALID_CALL_ID) ||
            (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED) ||
            (call_info_entry->voice_scv_info.call_state == CALL_STATE_END_V02 &&
             !(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ENDED_REPORTED))))
      {
        QCRIL_LOG_INFO("No valid QMI call id");
        break;
      }
      // ENDed call
      if (call_info_entry->voice_scv_info.call_state == CALL_STATE_END_V02 &&
          (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ENDED_REPORTED))
      {
        QCRIL_LOG_INFO("ENDed call");
        break;
      }
      // SETUP call
      if (call_info_entry->voice_scv_info.call_state == CALL_STATE_SETUP_V02)
      {
        QCRIL_LOG_INFO("SETUP call");
        break;
      }
      // OTAPA call
      if (call_info_entry->voice_scv_info.call_type == CALL_TYPE_OTAPA_V02)
      {
        QCRIL_LOG_INFO("OTAPA call");
        break;
      }
      // Call mode matches with reported voice radio tech
      // Emergency call originated from limited service or emergency call or 1x CSFB call
      if ((call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_WAITING_FOR_MATCHING_VOICE_RTE) &&
          (call_info_entry->elaboration & (QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EME_FROM_OOS |
                                           QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_EMERGENCY_CALL |
                                           QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1x_CSFB_CALL)))
      {
        QCRIL_LOG_INFO("Waiting for matching VOICE RTE");
        break;
      }
      if (!(call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RIL_CALL_STATE_VALID))
      {
        QCRIL_LOG_INFO("RIL CALL STATE not valid");
        break;
      }
      if (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_1X_REMOTE_NUM_PENDING)
      {
        QCRIL_LOG_INFO("Remote number pending");
        break;
      }
      if ((call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PHANTOM_CALL) &&
          (call_info_entry->voice_scv_info.call_state == CALL_STATE_ORIGINATING_V02 ||
           call_info_entry->voice_scv_info.call_state == CALL_STATE_ALERTING_V02) &&
          (qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state(RIL_CALL_ACTIVE) != NULL))
      {
        QCRIL_LOG_INFO("Phantom call in ORIG/ALERT state");
        break;
      }
      report_call = true;
    } while (FALSE);

    if (report_call)
    {
      if (callDomain == qcril::interfaces::CallDomain::AUTOMATIC ||
          (callDomain == qcril::interfaces::CallDomain::PS &&
           qcril_qmi_voice_call_to_ims(call_info_entry)) ||
          (callDomain == qcril::interfaces::CallDomain::CS &&
           qcril_qmi_voice_call_to_atel(call_info_entry)))
      {
        num_of_calls++;
        snprintf(log_addon, QCRIL_MAX_LOG_MSG_SIZE,
                 "[qmi call id %d, android call id %d, qmi call state %d]",
                 call_info_entry->qmi_call_id,
                 call_info_entry->android_call_id,
                 call_info_entry->voice_scv_info.call_state);
        strlcat(log_essence, log_addon, sizeof(log_essence));
        qcril::interfaces::CallInfo callInfo{};
        if (qcril_qmi_voice_gather_current_call_information(callInfo, call_info_entry) != RIL_E_SUCCESS)
        {
          ril_req_res = RIL_E_INTERNAL_ERR;
          break;
        }
        call_info_entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MOD_CAUSE_VALID;
        call_info_entry->lcf_valid = FALSE;

        calls.push_back(std::move(callInfo));
      }
    }
    else
    {
      // call not reported
      log_nof_skipped_calls++;
    }

    if (CALL_STATE_END_V02 == call_info_entry->voice_scv_info.call_state &&
        VOICE_INVALID_CALL_ID != call_info_entry->android_call_id)
    {
      if (callDomain == qcril::interfaces::CallDomain::AUTOMATIC ||
          (callDomain == qcril::interfaces::CallDomain::PS && qcril_qmi_voice_call_to_ims(call_info_entry)) ||
          (callDomain == qcril::interfaces::CallDomain::CS && qcril_qmi_voice_call_to_atel(call_info_entry)))
      {
        // Need to add this to destroy the call object.
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ENDED_REPORTED;

        // - For IMS call, failure is sent along with
        //   UNSOL_RESPONSE_CALL_STATE_CHANGED. so set the elab here.
        // - For CS call, call is cleared after LAST_CALL_FAILURE
        //   req is handled, elab is set by then
        if (qcril_qmi_voice_call_to_ims(call_info_entry))
        {
          call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LAST_CALL_FAILURE_REPORTED;
        }
        need_consider_voice_call_obj_cleanup = TRUE;
      }
    }

    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
  }

  if (calls.size() == 1)
  {
    std::string number;
    int numberPresentation;
    int toa = QCRIL_QMI_VOICE_DOMESTIC_NUMBER;
    if (qcril_qmi_voice_get_cdma_emergency_number(number, numberPresentation, toa)) {
      calls[0].setNumber(number);
      calls[0].setNumberPresentation(numberPresentation);
      calls[0].setToa(toa);
    }
  }

  if (need_consider_voice_call_obj_cleanup)
  {
    qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl();
  }

  qcril_qmi_voice_voip_unlock_overview();

  if (qcril_qmi_voice_voip_call_info_entries_is_empty())
  {
      QCRIL_LOG_INFO("Terminating MO call, request to stop DIAG logging");
      if (!qcril_stop_diag_log())
      {
          QCRIL_LOG_INFO("qxdm logging disabled successfully");
      }
  }

  if (callDomain == qcril::interfaces::CallDomain::CS)
  {
     if( !num_of_calls )
     {
       auto msg = std::make_shared<QcRilUnsolVoiceAllCsCallsEndedMessage>();
       Dispatcher::getInstance().dispatch(msg);
     }
  }

  snprintf(log_addon, QCRIL_MAX_LOG_MSG_SIZE,
           " reported/skipped %d/%d calls",
           num_of_calls, log_nof_skipped_calls);
  strlcat(log_essence, log_addon, sizeof(log_essence));

  QCRIL_LOG_ESSENTIAL("%s", log_essence);
  return ril_req_res;
} // qcril_qmi_get_call_list_to_send

//===========================================================================
// qcril_qmi_voice_request_get_current_calls
//===========================================================================
RIL_Errno qcril_qmi_voice_request_get_current_calls
(
 qcril::interfaces::CallDomain callDomain,
 std::vector<qcril::interfaces::CallInfo> &calls
)
{
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno ril_req_res = qcril_qmi_get_call_list_to_send(callDomain, calls);
  if (ril_req_res == RIL_E_SUCCESS)
  {
    QCRIL_LOG_INFO("Reply to RIL --> Number of calls : %d", calls.size());
    uint32_t i = 0;
    for (auto &call : calls)
    {
      QCRIL_LOG_INFO("Reply to RIL --> call[%d]: state %s, index %d, callType %s, callDomain %s"
                     ", toa %d, isMpty %d, isMT %d",
                     i,
                     qcril::interfaces::toString(call.getCallState()).c_str(),
                     call.getIndex(),
                     qcril::interfaces::toString(call.getCallType()).c_str(),
                     qcril::interfaces::toString(call.getCallDomain()).c_str(),
                     call.getToa(),
                     call.getIsMpty(),
                     call.getIsMt());
      std::string numStr = PII(call.getNumber(),
                             hide_phone_number(call.getNumber().c_str()));
      std::string redirNumStr = PII(call.getRedirNum(),
                                  hide_phone_number(call.getRedirNum().c_str()));
      QCRIL_LOG_INFO("...num %s, num presentation %d, name %s, name presentation %d "
                     "redirected number %s,  redirected number presentation %d",
                     numStr.c_str(),
                     call.getNumberPresentation(),
                     call.getName().c_str(),
                     call.getNamePresentation(),
                     redirNumStr.c_str(),
                     call.getRedirNumPresentation());
      i++;
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return ril_req_res;
} // qcril_qmi_voice_request_get_current_calls

//===========================================================================
//qcril_qmi_voice_create_emer_voice_entry
//===========================================================================
 void qcril_qmi_voice_create_emer_voice_entry(qcril_qmi_voice_emer_voice_feature_info_type * emer_voice_number, voice_remote_party_number2_type_v02 * remote_party_number)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("entry %p, remote svc entry %p", emer_voice_number, remote_party_number);
  if( emer_voice_number && remote_party_number)
  {
    emer_voice_number->is_valid = TRUE;
    if ( remote_party_number->number[0] == QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER_PREFIX )
    {
      emer_voice_number->toa = QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER;
    }
    else
    {
      emer_voice_number->toa = QCRIL_QMI_VOICE_DOMESTIC_NUMBER;
    }
    emer_voice_number->number = (char*)qcril_malloc( remote_party_number->number_len + 1 );
    if ( emer_voice_number->number )
    {
      memcpy( emer_voice_number->number, remote_party_number->number, remote_party_number->number_len + 1 );
      emer_voice_number->numberPresentation = qcril_qmi_voice_map_qmi_to_ril_num_pi( remote_party_number->number_pi );
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_voice_create_emer_voice_entry

//===========================================================================
//qcril_qmi_voice_is_emer_voice_entry_valid
//===========================================================================
int qcril_qmi_voice_is_emer_voice_entry_valid(qcril_qmi_voice_emer_voice_feature_info_type * emer_voice_number)
{
  int ret = FALSE;

  QCRIL_LOG_FUNC_ENTRY();
  if( emer_voice_number && TRUE == emer_voice_number->is_valid )
  {
    ret = TRUE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
} //qcril_qmi_voice_is_emer_voice_entry_valid

//===========================================================================
// qcril_qmi_voice_get_cdma_emergency_number
//===========================================================================
bool qcril_qmi_voice_get_cdma_emergency_number(std::string& number,     /* out */
                                               int& numberPresentation, /* out */
                                               int& toa                 /* out */
)
{
  qcril_qmi_voice_voip_call_info_entry_type* cdma_voice_call_info_entry = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* cdma_no_srv_emer_call_info_entry = NULL;
  qcril_qmi_voice_emer_voice_feature_info_type* temp_ptr = NULL;
  int is_cdma_voice_emergency_calls_present = FALSE;
  bool ret = false;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO("call type to be considered %d", qcril_qmi_voice_cdma_call_type_to_be_considered);
  is_cdma_voice_emergency_calls_present = qcril_qmi_voice_is_cdma_voice_emergency_calls_present(
      &cdma_voice_call_info_entry, &cdma_no_srv_emer_call_info_entry);

  if (CALL_TYPE_EMERGENCY_V02 == qcril_qmi_voice_cdma_call_type_to_be_considered) {
    if (NULL != cdma_no_srv_emer_call_info_entry &&
        VOICE_LOWEST_CALL_ID < cdma_no_srv_emer_call_info_entry->voice_scv_info.call_id &&
        // mode of the emergency call would be either CDMA or NO_SRV
        // In case of NO_SRV we conclude that it is a CDMA emergency + voice call scenario by
        // checking If there is a CDMA voice call present
        (CALL_MODE_CDMA_V02 == cdma_no_srv_emer_call_info_entry->voice_scv_info.mode ||
         TRUE == is_cdma_voice_emergency_calls_present) &&
        CALL_TYPE_EMERGENCY_V02 == cdma_no_srv_emer_call_info_entry->voice_scv_info.call_type &&
        // Should not be considering the ENDed call's number as we not going to destroy and not
        // report this call to Atel QMI Voice replaces the voice call's number with a emergency
        // number by the time emergency call is ENDed
        CALL_STATE_END_V02 != cdma_no_srv_emer_call_info_entry->voice_scv_info.call_state) {
      temp_ptr = &(cdma_no_srv_emer_call_info_entry->emer_voice_number);
    }
  } else {
    if (NULL != cdma_voice_call_info_entry &&
        (CALL_MODE_CDMA_V02 == cdma_voice_call_info_entry->voice_scv_info.mode) &&
        (cdma_voice_call_info_entry->elaboration &
         QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_REPORT_CACHED_RP_NUMBER)) {
      temp_ptr = &(cdma_voice_call_info_entry->emer_voice_number);
    }
  }

  if (temp_ptr) {
    QCRIL_LOG_INFO("Found call entry - valid %d, number %s, presentation %d, toa %d",
                   temp_ptr->is_valid,
                   temp_ptr->number,
                   temp_ptr->numberPresentation,
                   temp_ptr->toa);
    if (temp_ptr->is_valid) {
      ret = true;
      if (temp_ptr->number[0] == QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER_PREFIX) {
        toa = QCRIL_QMI_VOICE_INTERNATIONAL_NUMBER;
      } else {
        toa = QCRIL_QMI_VOICE_DOMESTIC_NUMBER;
      }
      number = temp_ptr->number;
      numberPresentation = temp_ptr->numberPresentation;
    } else {  // should not be ending up here
      QCRIL_LOG_FATAL("Invalid state in cdma call handling");
    }
  } else {
    QCRIL_LOG_INFO("ril call entry unchanged");
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
}  // qcril_qmi_voice_get_cdma_emergency_number

//===========================================================================
// qcril_qmi_voice_nas_control_is_any_calls_present
//===========================================================================
boolean qcril_qmi_voice_nas_control_is_any_calls_present()
{
  QCRIL_LOG_FUNC_ENTRY();
  boolean result = FALSE;
  qcril_qmi_voice_voip_current_call_summary_type calls_summary;
  memset( &calls_summary, 0, sizeof( calls_summary ) );

  qcril_qmi_voice_voip_lock_overview();
  qcril_qmi_voice_voip_generate_summary( &calls_summary );
  if (calls_summary.nof_calls_overall)
  {
    result = TRUE;
  }
  qcril_qmi_voice_voip_unlock_overview();
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int) result);
  return result;
} //qcril_qmi_voice_nas_control_is_any_calls_present

//===========================================================================
// qcril_qmi_voice_auto_answer_timeout_handler
//===========================================================================
void qcril_qmi_voice_auto_answer_timeout_handler( void * param )
{
  voice_answer_call_req_msg_v02               ans_call_req_msg;
  qcril_qmi_voice_voip_call_info_entry_type   *call_info_entry = NULL;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED( param );

  do
  {
    qcril_qmi_voice_voip_lock_overview();
    qmi_voice_voip_overview.auto_answer_timer_id = TimeKeeper::no_timer;

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
        QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PENDING_INCOMING, TRUE);
    if ( NULL == call_info_entry ){
      QCRIL_LOG_INFO(".. pending incoming call record entry not found");
      break;
    }

    memset( &ans_call_req_msg, 0, sizeof(ans_call_req_msg ) );
    ans_call_req_msg.call_id = call_info_entry->qmi_call_id;
    QCRIL_LOG_INFO(".. call id qmi %d", (int) ans_call_req_msg.call_id );

    if ( call_info_entry->qmi_call_id == VOICE_INVALID_CALL_ID )
    {
      QCRIL_LOG_INFO(".. Invalid Call id!");
      break;
    }

    ans_call_req_msg.call_type_valid = TRUE;
    ans_call_req_msg.call_type = call_info_entry->voice_scv_info.call_type;

    //Fill call type details for VT & VOIP calls from the call_info_entry
    if(call_info_entry->voice_scv_info.call_type == CALL_TYPE_VT_V02)
    {
      ans_call_req_msg.audio_attrib_valid = TRUE;
      ans_call_req_msg.audio_attrib = (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02);

      ans_call_req_msg.video_attrib_valid = TRUE;
      ans_call_req_msg.video_attrib = (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02);
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
        QCRIL_REQ_ID_INTERNAL);
    // Send QMI VOICE ANSWER CALL REQ
    qmi_client_error_type client_err = qmi_client_voice_send_async(
                                 QMI_VOICE_ANSWER_CALL_REQ_V02,
                                 &ans_call_req_msg,
                                 sizeof(ans_call_req_msg),
                                 sizeof(voice_answer_call_resp_msg_v02),
                                 qcril_qmi_voice_command_cb,
                                 (void*)(uintptr_t)user_data);
    if ( client_err )
    {
      QCRIL_LOG_INFO(".. failed to post qmi answer message");
    }
    else
    {
        call_info_entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL;
        qcril_am_handle_event(qcril_qmi_voice_get_answer_am_event(call_info_entry), NULL);
    }
  } while (FALSE);

  qcril_qmi_voice_voip_unlock_overview();
  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_qmi_process_hangup_on_call_being_setup

===========================================================================*/
/*!
    @brief
    Retrieve call id for MO call for which RIL has not sent response back
    to RIL client.

    Update call info elaboration if need to wait for CC_IN_PROGRESS

    @return
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_process_hangup_on_call_being_setup
(
    int *conn_index
)
{
    qcril_qmi_voice_voip_call_info_entry_type *call_info_entry = NULL;
    RIL_Errno                                  ril_err         = RIL_E_GENERIC_FAILURE;

    call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration(
                                QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_MO_CALL_BEING_SETUP,
                                TRUE );
    QCRIL_LOG_INFO( "call_info_entry %p", call_info_entry );

    if (call_info_entry)
    {
      QCRIL_LOG_INFO( "qmi call id %d android call id %d",
                      call_info_entry->qmi_call_id,
                      call_info_entry->android_call_id);
      if (VOICE_INVALID_CALL_ID == call_info_entry->qmi_call_id)
      {
        call_info_entry->elaboration |=
                          QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HANGUP_AFTER_VALID_QMI_ID;
        ril_err = RIL_E_SUCCESS;
      }
      else
      {
        *conn_index = call_info_entry->android_call_id;
        ril_err = RIL_E_SUCCESS;
      }
    }
    else
    {
      // Telephony will use HANGUP -1 if user tries to hangup the call
      // before the GET_CURRENT_CALLS response received eventhough the
      // RIL have already sent the DIAL response.
      // In this cases, RIL would be alread cleared the ELA_MO_CALL_BEING_SETUP
      // and thus it will not find any valid call_info_entry and send failure
      // back to telephony.
      // Add support to handle the HANGUP -1 for disconnecting the call in
      // DIALING/ALERTING/ACTIVE state.
      QCRIL_LOG_INFO("call_info_entry with ELA_MO_CALL_BEING_SETUP not found. "
                     "Check if any calls in DIALING/ALERTING/ACTIVE state");

      call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state(
                                RIL_CALL_DIALING);
      if (!call_info_entry)
      {
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state(
                                  RIL_CALL_ALERTING);
      }
      if (!call_info_entry)
      {
        call_info_entry = qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state(
                                  RIL_CALL_ACTIVE);
      }

      // Found a DIALING/ACTIVE call
      if (call_info_entry &&
              call_info_entry->qmi_call_id != VOICE_INVALID_CALL_ID)
      {
        QCRIL_LOG_INFO("qmi call id %d android call id %d ril call state %d",
                       call_info_entry->qmi_call_id,
                       call_info_entry->android_call_id,
                       call_info_entry->ril_call_state);
        *conn_index = call_info_entry->android_call_id;
        ril_err = RIL_E_SUCCESS;
      }
      else
      {
        QCRIL_LOG_ERROR("Failed to find call entry, aborting!");
      }
    }

    return ril_err;
}

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_send_hangup_on_call

===========================================================================*/
/*!
    @brief
    Send hangup request for pending call

    @return
*/
/*=========================================================================*/
void qcril_qmi_voice_send_hangup_on_call
(
    int conn_index
)
{
    QCRIL_LOG_FUNC_ENTRY();

    /* Lookup the hangup request */
    auto pendingMsg = extractVoiceModulePendingMessage(
            QcRilRequestHangupMessage::get_class_message_id());
    if (pendingMsg != nullptr)
    {
      std::shared_ptr<QcRilRequestHangupMessage> hangupMsg =
          std::static_pointer_cast<QcRilRequestHangupMessage>(pendingMsg);
      if (hangupMsg) {
        hangupMsg->setCallIndex(conn_index);
      }
    }

    auto voiceModule = getVoiceModule();
    if (pendingMsg && voiceModule)
    {
      voiceModule->dispatchSync(pendingMsg);
    }

    QCRIL_LOG_FUNC_RETURN();
}


//===========================================================================
// ****************************** STK CC ***********************************
//===========================================================================

//===========================================================================
// qcril_qmi_voice_is_stk_cc_in_progress
//===========================================================================
int qcril_qmi_voice_is_stk_cc_in_progress(void)
{
  return ( QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE == stk_cc_info.modification ) ? FALSE : TRUE;
} // qcril_qmi_voice_is_stk_cc_in_progress

//===========================================================================
// qcril_qmi_voice_is_stk_cc_in_progress
//===========================================================================
void qcril_qmi_voice_reset_stk_cc(void)
{
  stk_cc_info.modification = QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE;
  stk_cc_info.is_alpha_relayed = FALSE;
  stk_cc_info.is_ims_request = FALSE;
  stk_cc_info.call_id_info = QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID;
} // qcril_qmi_voice_reset_stk_cc

//===========================================================================
// qcril_qmi_voice_stk_cc_relay_alpha_if_necessary
//===========================================================================
void qcril_qmi_voice_stk_cc_relay_alpha_if_necessary(qcril_instance_id_e_type instance_id, boolean send_unsol_unconditional)
{
  char buf_str[QMI_VOICE_ALPHA_TEXT_MAX_V02 + 2];

  memset(buf_str, 0 , QMI_VOICE_ALPHA_TEXT_MAX_V02 + 2);

  QCRIL_LOG_DEBUG("instance_id = %d, send unsol unconditional = %d", instance_id, send_unsol_unconditional);

  if ( ( qcril_qmi_voice_is_stk_cc_in_progress() && !stk_cc_info.is_alpha_relayed ) ||
       ( send_unsol_unconditional ) )
  {
    if ( stk_cc_info.alpha_ident.alpha_text_len > 0 )
    {
      switch ( stk_cc_info.alpha_ident.alpha_dcs )
      {
        case ALPHA_DCS_GSM_V02:
           if(stk_cc_info.alpha_ident.alpha_text_len < QMI_VOICE_ALPHA_TEXT_MAX_V02)
               qcril_cm_ss_convert_gsm8bit_alpha_string_to_utf8( (char*) stk_cc_info.alpha_ident.alpha_text,
                                                                stk_cc_info.alpha_ident.alpha_text_len,
                                                                buf_str,
                                                                sizeof(buf_str));
          break;

        case ALPHA_DCS_UCS2_V02:
        {
          size_t decoded_buf_max_len = stk_cc_info.alpha_ident.alpha_text_len / 2;
          uint16_t * decoded_buf = (uint16_t*)qcril_malloc(decoded_buf_max_len * 2);
          if (decoded_buf) {
            size_t actual_len = 0;
            auto res = qcril_cm_ss_decode_ucs2(stk_cc_info.alpha_ident.alpha_text,
                                    stk_cc_info.alpha_ident.alpha_text_len,
                                    decoded_buf, decoded_buf_max_len, actual_len);
            if (res)
            {
                qcril_cm_ss_convert_ucs2_to_utf8((char*)decoded_buf, actual_len * 2,
                                              buf_str, sizeof(buf_str));
            }
            qcril_free(decoded_buf);
          }
          break;
        }

        default:
          buf_str[0] = 0;
          break;
      }

      if ( *buf_str )
      {
        auto msg = std::make_shared<RilUnsolStkCCAlphaNotifyMessage>(
            std::string(static_cast<const char*>(buf_str)));
        Dispatcher::getInstance().dispatchSync(msg);
      }

      // RIL_UNSOL_STK_CC_ALPHA_NOTIFY
      stk_cc_info.is_alpha_relayed = TRUE;
    }
  }
} // qcril_qmi_voice_stk_cc_relay_alpha_if_necessary

//===========================================================================
// qcril_qmi_voice_map_qmi_ss_service_type_to_ril_ss_request_type
//===========================================================================
boolean qcril_qmi_voice_map_qmi_ss_service_type_to_ril_ss_request_type
(
 service_type_enum_v02 qmi_type,
 RIL_SsRequestType *ril_type
)
{
  boolean result = FALSE;

  if (ril_type)
  {
    result = TRUE;
    switch (qmi_type)
    {
      case SERVICE_TYPE_ACTIVATE_V02:
        *ril_type = SS_ACTIVATION;
        break;

      case SERVICE_TYPE_DEACTIVATE_V02:
        *ril_type = SS_DEACTIVATION;
        break;

      case SERVICE_TYPE_REGISTER_V02:
        *ril_type = SS_REGISTRATION;
        break;

      case SERVICE_TYPE_ERASE_V02:
        *ril_type = SS_ERASURE;
        break;

      case SERVICE_TYPE_INTERROGATE_V02:
        *ril_type = SS_INTERROGATION;
        break;

        // qmi_todo: why VOICE_CC_SUPS_RESULT_SERVICE_TYPE_REGISTER_PASSWORD_V02 ??
      default:
        result = FALSE;
        *ril_type = SS_ERASURE; // some default value as we may refer to this value for logging
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_voice_map_qmi_ss_reason_to_ril_ss_service_type
//===========================================================================
boolean qcril_qmi_voice_map_qmi_ss_reason_to_ril_ss_service_type
(
 voice_sups_ind_reason_enum_v02 qmi_reason,
 int *ril_type
)
{
  boolean result = FALSE;

  if (ril_type)
  {
    result = TRUE;
    switch (qmi_reason)
    {
      case VOICE_SUPS_IND_REASON_FWD_UNCONDITIONAL_V02:
        *ril_type = SS_CFU;
        break;

      case VOICE_SUPS_IND_REASON_FWD_MOBILEBUSY_V02:
        *ril_type = SS_CF_BUSY;
        break;

      case VOICE_SUPS_IND_REASON_FWD_NOREPLY_V02:
        *ril_type = SS_CF_NO_REPLY;
        break;

      case VOICE_SUPS_IND_REASON_FWD_UNREACHABLE_V02:
        *ril_type = SS_CF_NOT_REACHABLE;
        break;

      case VOICE_SUPS_IND_REASON_FWD_ALLFORWARDING_V02:
        *ril_type = SS_CF_ALL;
        break;

      case VOICE_SUPS_IND_REASON_FWD_ALLCONDITIONAL_V02:
        *ril_type = SS_CF_ALL_CONDITIONAL;
        break;

      case VOICE_SUPS_IND_REASON_BARR_ALLOUTGOING_V02:
        *ril_type = SS_BAOC;
        break;

      case VOICE_SUPS_IND_REASON_BARR_OUTGOINGINT_V02:
        *ril_type = SS_BAOIC;
        break;

      case VOICE_SUPS_IND_REASON_BARR_OUTGOINGINTEXTOHOME_V02:
        *ril_type = SS_BAOIC_EXC_HOME;
        break;

      case VOICE_SUPS_IND_REASON_BARR_ALLINCOMING_V02:
        *ril_type = SS_BAIC;
        break;

      case VOICE_SUPS_IND_REASON_BARR_INCOMINGROAMING_V02:
        *ril_type = SS_BAIC_ROAMING;
        break;

      case VOICE_SUPS_IND_REASON_BARR_ALLBARRING_V02:
        *ril_type = SS_ALL_BARRING;
        break;

      case VOICE_SUPS_IND_REASON_BARR_ALLOUTGOINGBARRING_V02:
        *ril_type = SS_OUTGOING_BARRING;
        break;

      case VOICE_SUPS_IND_REASON_BARR_ALLINCOMINGBARRING_V02:
        *ril_type = SS_INCOMING_BARRING;
        break;

      case VOICE_SUPS_IND_REASON_CALLWAITING_V02:
        *ril_type = SS_WAIT;
        break;

      case VOICE_SUPS_IND_REASON_CLIP_V02:
        *ril_type = SS_CLIP;
        break;

      case VOICE_SUPS_IND_REASON_CLIR_V02:
        *ril_type = SS_CLIR;
        break;

      case VOICE_SUPS_IND_REASON_COLP_V02:
        *ril_type = SS_COLP;
        break;

      case VOICE_SUPS_IND_REASON_COLR_V02:
        *ril_type = SS_COLR;
        break;

      case VOICE_SUPS_IND_REASON_BARR_INCOMINGNUMBER_V02:
        *ril_type = SS_INCOMING_BARRING_DN;
        break;

      case VOICE_SUPS_IND_REASON_BARR_INCOMINGANONYMOUS_V02:
        *ril_type = SS_INCOMING_BARRING_ANONYMOUS;
        break;

      default:
        result = FALSE;
        *ril_type = SS_CFU; // some default value as we may refer to this value for logging
        break;
    }
  }

  return result;
}

//===========================================================================
// qcril_qmi_voice_stk_cc_convert_qmi_svc_class_to_ril_teleservice
//===========================================================================
RIL_SsTeleserviceType qcril_qmi_voice_stk_cc_convert_qmi_svc_class_to_ril_teleservice( uint8_t service_class )
{
  RIL_SsTeleserviceType ss_teleservice_type;

  switch ( service_class )
  {
    case QCRIL_QMI_VOICE_TELESEFRVICE_ALL:
      ss_teleservice_type = SS_ALL_TELESEVICES;
      break;

    case QCRIL_QMI_VOICE_TELESEFRVICE_TELEPHONY:
      ss_teleservice_type = SS_TELEPHONY;
      break;

    case QCRIL_QMI_VOICE_TELESEFRVICE_ALL_DATA:
      ss_teleservice_type = SS_ALL_DATA_TELESERVICES;
      break;

    case QCRIL_QMI_VOICE_TELESEFRVICE_SMS:
      ss_teleservice_type = SS_SMS_SERVICES;
      break;

    case QCRIL_QMI_VOICE_TELESEFRVICE_ALL_EXCEPT_SMS:
      ss_teleservice_type = SS_ALL_TELESERVICES_EXCEPT_SMS;
      break;

    case QCRIL_QMI_VOICE_TELESEFRVICE_ALL_BEARER_SVC:
       ss_teleservice_type = SS_ALL_TELE_AND_BEARER_SERVICES;
       break;

    default: // something that we do not support
      ss_teleservice_type = SS_ALL_TELE_AND_BEARER_SERVICES;
      break;
  }

  return ss_teleservice_type;
} // qcril_qmi_voice_stk_cc_convert_qmi_svc_class_to_ril_teleservice

//===========================================================================
// qcril_qmi_voice_convert_sups_ind_call_forwarding_info_to_ril_cfdata
//===========================================================================
boolean qcril_qmi_voice_convert_sups_ind_call_forwarding_info_to_ril_cfdata
(
  voice_sups_ind_reason_enum_v02 reason,
  voice_get_call_forwarding_info_type_v02 *qmi_cf_info,
  int qmi_cf_info_len,
  std::vector<qcril::interfaces::CallForwardInfo> &cf_data
)
{
  int idx;
  int ril_cf_reason = qcril_qmi_voice_map_qmi_cfw_reason_to_ril_reason(reason);

  for (idx = 0; idx < qmi_cf_info_len; idx++)
  {
    qcril::interfaces::CallForwardInfo cfInfo = {};
    cfInfo.setStatus((qmi_cf_info[idx].service_status == QCRIL_QMI_VOICE_SERVICE_STATUS_ACTIVE)
                         ? QCRIL_QMI_VOICE_RIL_CF_STATUS_ACTIVE
                         : QCRIL_QMI_VOICE_RIL_CF_STATUS_NOT_ACTIVE);
    cfInfo.setReason(ril_cf_reason);
    cfInfo.setServiceClass(qmi_cf_info[idx].service_class);
    cfInfo.setNumber(qmi_cf_info[idx].number);  // 1st is char*, 2nd is char[]
    cfInfo.setToa((QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX == *qmi_cf_info[idx].number)
                      ? QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL
                      : QCRIL_QMI_VOICE_SS_TA_UNKNOWN);
    cfInfo.setTimeSeconds(qmi_cf_info[idx].no_reply_timer);
    cf_data.push_back(cfInfo);
  }

  return TRUE;
}

//===========================================================================
// qcril_qmi_voice_stk_cc_notify_unsol_on_ussd_from_sups_ind
//===========================================================================
void qcril_qmi_voice_stk_cc_notify_unsol_on_ussd_from_sups_ind
(
 voice_sups_ind_msg_v02 *sups_ind_msg
)
{
  uint8_t uss_dcs  = QCRIL_QMI_VOICE_USSD_DCS_UNSPECIFIED;
  boolean success  = FALSE;
  int     utf8_len = 0;
  char    ussd_utf8_str[QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2];
  qcril::interfaces::UssdModeType type_code = qcril::interfaces::UssdModeType::UNKNOWN;
  qcril::interfaces::UssdDomain domain = qcril::interfaces::UssdDomain::CS;
  QCRIL_LOG_FUNC_ENTRY();

  if (sups_ind_msg != NULL)
  {
    QCRIL_LOG_INFO("..sups_ind uss_info_valid: %d, uss_info_utf16_valid: %d",
        sups_ind_msg->uss_info_valid, sups_ind_msg->uss_info_utf16_valid);

    memset(ussd_utf8_str, '\0', sizeof(ussd_utf8_str));

    // uss_info_utf16 has high priority.
    if (sups_ind_msg->uss_info_utf16_valid)
    {
      utf8_len = qcril_cm_ss_convert_ucs2_to_utf8((char *)sups_ind_msg->uss_info_utf16,
          sups_ind_msg->uss_info_utf16_len * 2, ussd_utf8_str, sizeof(ussd_utf8_str));

      if (utf8_len > (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2))
      {
        QCRIL_LOG_ERROR("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
        utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
        ussd_utf8_str[utf8_len - 1] = '\0';
      }
      success = TRUE;
    }
    else if (sups_ind_msg->uss_info_valid)
    {
      QCRIL_LOG_INFO("..sups_ind uss_dcs %d", (int) sups_ind_msg->uss_info.uss_dcs);
      switch (sups_ind_msg->uss_info.uss_dcs)
      {
        case USS_DCS_ASCII_V02 :
          utf8_len = qcril_cm_ss_ascii_to_utf8((unsigned char *)sups_ind_msg->uss_info.uss_data,
              sups_ind_msg->uss_info.uss_data_len, ussd_utf8_str, sizeof(ussd_utf8_str));
          success = TRUE;
          break;

        case USS_DCS_8BIT_V02 :
          uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_8_BIT;
          utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8(uss_dcs,
              sups_ind_msg->uss_info.uss_data, sups_ind_msg->uss_info.uss_data_len,
              ussd_utf8_str, sizeof(ussd_utf8_str));
          if (utf8_len > (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2))
          {
            QCRIL_LOG_ERROR("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
            utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
            ussd_utf8_str[utf8_len - 1] = '\0';
          }
          success = TRUE;
          break;

        case USS_DCS_UCS2_V02 :
          uss_dcs = QCRIL_QMI_VOICE_USSD_DCS_UCS2;
          utf8_len = qcril_cm_ss_convert_ussd_string_to_utf8(uss_dcs,
              sups_ind_msg->uss_info.uss_data, sups_ind_msg->uss_info.uss_data_len,
              ussd_utf8_str, sizeof(ussd_utf8_str));
          if (utf8_len > (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR * 2))
          {
            QCRIL_LOG_ERROR("ascii_len exceeds QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR");
            utf8_len = (int) (QCRIL_QMI_VOICE_MAX_MT_USSD_CHAR*2);
            ussd_utf8_str[utf8_len - 1] = '\0';
          }
          success = TRUE;
          break;

        default :
          QCRIL_LOG_ERROR("Invalid USSD dcs : %d", sups_ind_msg->uss_info.uss_dcs);
          break;
      }
    }

    if (success)
    {
      type_code = qcril::interfaces::UssdModeType::NOTIFY;
      if (sups_ind_msg->uss_info.uss_data_len > 0)
      {
        //response_buff[1] = ussd_utf8_str;
      }
      else
      {
        ussd_utf8_str[0] = '\0';
      }

      // Sending the response received from the network for the USSD request
      QCRIL_LOG_DEBUG("USSD Conf Success, data_len : %d", sups_ind_msg->uss_info.uss_data_len);
      QCRIL_LOG_DEBUG("USSD : type_code=%d, response_buff=%s strlen=%d", type_code, ussd_utf8_str, strlen(ussd_utf8_str));
    }
    else
    {
      // sending the unsol indication so that RIL can close the USSD session
      if ((sups_ind_msg->failure_cause_valid == TRUE) &&
          (sups_ind_msg->failure_cause == QMI_FAILURE_CAUSE_FACILITY_NOT_SUPPORTED_V02))
      {
        type_code = qcril::interfaces::UssdModeType::NOT_SUPPORTED;
      }
      else
      {
        type_code = qcril::interfaces::UssdModeType::NW_RELEASE;
      }
      ussd_utf8_str[0] = '\0';
      QCRIL_LOG_DEBUG("STK CC USSD Failure: type_code=%d",type_code);
    }
    if( sups_ind_msg->ussd_domain_route_valid ) {
      domain = qcril_qmi_voice_map_qmi_to_ril_ussd_domain(sups_ind_msg->ussd_domain_route);
    }
    qcril_qmi_voice_send_unsol_ussd(domain, type_code, ussd_utf8_str, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_stk_cc_notify_unsol_on_ss_from_sups_ind
//===========================================================================
void qcril_qmi_voice_stk_cc_notify_unsol_on_ss_from_sups_ind
(
 voice_sups_ind_msg_v02 *sups_ind_msg
)
{
  boolean           success = FALSE;
  boolean           ss_service_type_available = FALSE;
  int               ss_service_type = SS_CFU;
  boolean           ss_request_type_available = FALSE;
  RIL_SsRequestType ss_request_type = SS_ERASURE;
  boolean           ss_service_class_available = FALSE;
  int               ss_service_class = 0;

  QCRIL_LOG_FUNC_ENTRY();

  auto msg = std::make_shared<QcRilUnsolSupplementaryServiceMessage>();

  if (sups_ind_msg && msg)
  {
    msg->setIsIms(stk_cc_info.is_ims_request ? true : false);

    QCRIL_LOG_INFO(".. ss reason %d, %d", (int) sups_ind_msg->reason_valid,
        (int) sups_ind_msg->reason);
    QCRIL_LOG_INFO(".. ss service class %d, %d", (int) sups_ind_msg->service_class_valid,
        (int) sups_ind_msg->service_class);

    // service type
    ss_request_type_available = qcril_qmi_voice_map_qmi_ss_service_type_to_ril_ss_request_type(
        sups_ind_msg->supplementary_service_info.service_type, &ss_request_type);

    QCRIL_LOG_INFO(".. ss_request_type %d, %d", (int) ss_request_type_available,
        (int) ss_request_type);

    // request type
    if (sups_ind_msg->reason_valid)
    {
      ss_service_type_available = qcril_qmi_voice_map_qmi_ss_reason_to_ril_ss_service_type(
          sups_ind_msg->reason, &ss_service_type);
    }
    QCRIL_LOG_INFO(".. ss_service_type %d, %d", (int) ss_service_type_available,
        (int) ss_service_type);

    // teleservice type
    if (sups_ind_msg->service_class_valid)
    {
      ss_service_class_available = TRUE;
      ss_service_class = sups_ind_msg->service_class;
    }
    else
    {
      ss_service_class_available = FALSE;
      ss_service_class           = 0; // per ril.h - 0 means user doesn't input class
    }
    QCRIL_LOG_INFO(".. ss_service_class %d, %d", (int) ss_service_class_available,
        (int) ss_service_class );

    // Video service class (CLASS_DATACIRCUITSYNC & CLASS_PACKETACCESS)
    if (ss_service_class & 0x50)
    {
      msg->setIsIms(true);
    }

    // build response
    msg->setServiceType((RIL_SsServiceType)ss_service_type);
    msg->setRequestType(ss_request_type);
    msg->setServiceClass(ss_service_class);

    if (ss_request_type_available && ss_service_type_available)
    {
      switch ( ss_request_type )
      {
        case SS_ACTIVATION:
        case SS_DEACTIVATION:
        case SS_REGISTRATION:
        case SS_ERASURE:
          // -- ALL SS request types except INTERROGATION -- start
          // -- ALL SS request types except INTERROGATION -- end
          switch (ss_service_type)
          {
            case SS_CFU:
            case SS_CF_BUSY:
            case SS_CF_NO_REPLY:
            case SS_CF_NOT_REACHABLE:
            case SS_CF_ALL:
            case SS_CF_ALL_CONDITIONAL:
            case SS_CLIP:
            case SS_CLIR:
            case SS_COLP:
            case SS_COLR:
            case SS_WAIT:
            case SS_BAOC:
            case SS_BAOIC:
            case SS_BAOIC_EXC_HOME:
            case SS_BAIC:
            case SS_BAIC_ROAMING:
            case SS_ALL_BARRING:
            case SS_OUTGOING_BARRING:
            case SS_INCOMING_BARRING:
              break;
            default:
              // failure case
              break;
          }
          break;

        case SS_INTERROGATION:
          {
          std::vector<int> ssInfo;
          switch (ss_service_type)
          {
            case SS_CLIP:
              if (sups_ind_msg->clip_status_valid)
              {
                success = TRUE;
                ssInfo.push_back(sups_ind_msg->clip_status.provision_status);
                if ((sups_ind_msg->clip_status.active_status == 0) &&
                    (sups_ind_msg->clip_status.provision_status == 0))
                {
                  ssInfo.push_back(QCRIL_QMI_VOICE_CLIR_SRV_NO_NETWORK);
                }
                msg->setSuppSrvInfoList(ssInfo);
              }
              QCRIL_LOG_INFO(".. ss_clip %d, %d", (int)success, (int)ssInfo[0]);
              break;

            case SS_CLIR:
              if (sups_ind_msg->clir_status_valid)
              {
                if (sups_ind_msg->failure_cause_valid)
                {
                  ssInfo.push_back((int)QCRIL_QMI_VOICE_CLIR_SRV_NO_NETWORK);
                  ssInfo.push_back(QCRIL_QMI_VOICE_SS_CLIR_PRESENTATION_INDICATOR);
                }
                else
                {
                  ssInfo.push_back(qcril_qmi_voice_info.clir);
                  int ril_provision_status = 0;
                  success = qcril_qmi_voice_map_qmi_to_ril_provision_status(
                      sups_ind_msg->clir_status.provision_status, &ril_provision_status);
                  ssInfo.push_back(ril_provision_status);
                }
                msg->setSuppSrvInfoList(ssInfo);
              }
              break;

            case SS_COLP:
              if (sups_ind_msg->colp_status_valid)
              {
                success = TRUE;
                ssInfo.push_back(sups_ind_msg->colp_status.provision_status);
                if ((sups_ind_msg->colp_status.active_status == 0) &&
                    (sups_ind_msg->colp_status.provision_status == 0))
                {
                  ssInfo.push_back(QCRIL_QMI_VOICE_CLIR_SRV_NO_NETWORK);
                }
                msg->setSuppSrvInfoList(ssInfo);
              }
              QCRIL_LOG_INFO(".. ss_colp %d, %d", (int)success, (int)ssInfo[0]);
              break;

            case SS_COLR:
              if (sups_ind_msg->colr_status_valid)
              {
                success = TRUE;
                ssInfo.push_back(sups_ind_msg->colr_status.provision_status);
                if ((sups_ind_msg->colr_status.active_status == 0) &&
                    (sups_ind_msg->colr_status.provision_status == 0))
                {
                  ssInfo.push_back(QCRIL_QMI_VOICE_CLIR_SRV_NO_NETWORK);
                }
                msg->setSuppSrvInfoList(ssInfo);
              }
              QCRIL_LOG_INFO(".. ss_colr %d, %d", (int)success, (int)ssInfo[0]);
              break;

            case SS_WAIT:
              if (sups_ind_msg->service_class_valid )
              {
                if (sups_ind_msg->service_class == 0x00) // we love magic numbers
                {
                  ssInfo.push_back(FALSE);
                }
                else
                {
                  ssInfo.push_back(TRUE);
                }
                ssInfo.push_back(sups_ind_msg->service_class);
                msg->setSuppSrvInfoList(ssInfo);
              }
              else
              {
                success = FALSE;
              }
              break;

            case SS_BAOC:
            case SS_BAOIC:
            case SS_BAOIC_EXC_HOME:
            case SS_BAIC:
            case SS_BAIC_ROAMING:
            case SS_ALL_BARRING:
            case SS_OUTGOING_BARRING:
            case SS_INCOMING_BARRING:
              if (sups_ind_msg->service_class_valid)
              {
                ssInfo.push_back(sups_ind_msg->service_class);
                msg->setSuppSrvInfoList(ssInfo);
              }
              break;

            case SS_INCOMING_BARRING_DN:
            case SS_INCOMING_BARRING_ANONYMOUS:
              if (sups_ind_msg->barred_numbers_list_valid &&
                  sups_ind_msg->barred_numbers_list_len > 0)
              {
                std::vector<std::string> cbNumList;
                for (uint32_t i = 0; i < sups_ind_msg->barred_numbers_list_len; i++)
                {
                  std::string num = sups_ind_msg->barred_numbers_list[i].barred_number;
                  cbNumList.push_back(num);
                }
                msg->setBarredNumberList(cbNumList);
                msg->setIsIms(true);
              }
              break;

            case SS_CFU:
            case SS_CF_BUSY:
            case SS_CF_NO_REPLY:
            case SS_CF_NOT_REACHABLE:
            case SS_CF_ALL:
            case SS_CF_ALL_CONDITIONAL:
            default:
              // Call Forward cases
              if ( sups_ind_msg->call_forwarding_info_valid &&
                  sups_ind_msg->call_forwarding_info_len > 0 &&
                  sups_ind_msg->call_forwarding_info_len < NUM_SERVICE_CLASSES)
              {
                std::vector<qcril::interfaces::CallForwardInfo> cfData;
                success = qcril_qmi_voice_convert_sups_ind_call_forwarding_info_to_ril_cfdata(
                    sups_ind_msg->reason,
                    sups_ind_msg->call_forwarding_info,
                    sups_ind_msg->call_forwarding_info_len,
                    cfData);
                msg->setCallForwardInfoList(cfData);
              }
              break;
          }
          }
          // -- INTERROGATION - end
          break;

        default:
          break;
      }
      Dispatcher::getInstance().dispatchSync(msg);
    }
    else
    {  // unknown request type
      QCRIL_LOG_ERROR("ss_request_type / ss_service_type not available");
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_stk_cc_handle_voice_sups_ind
//===========================================================================
void qcril_qmi_voice_stk_cc_handle_voice_sups_ind(voice_sups_ind_msg_v02* sups_ind_msg)
{
  int is_stk_cc_in_progress = qcril_qmi_voice_is_stk_cc_in_progress();
  voice_cc_sups_result_service_type_enum_v02 service_type = stk_cc_info.ss_ussd_info.service_type;

  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_voice_stk_cc_dump();

  if (sups_ind_msg != NULL)
  {
    QCRIL_LOG_INFO("..is_stk_cc_in_progress = %d", is_stk_cc_in_progress);
    QCRIL_LOG_INFO("..sups_ind call_id %d, %d",
        (int) sups_ind_msg->call_id_valid, (int) sups_ind_msg->call_id);
    QCRIL_LOG_INFO("..sups_ind data_source %d, %d",
        (int) sups_ind_msg->data_source_valid, (int) sups_ind_msg->data_source);
    QCRIL_LOG_INFO(".. service type, cc_mod %d, %d",
        (int) sups_ind_msg->supplementary_service_info.service_type,
        (int) sups_ind_msg->supplementary_service_info.is_modified_by_call_control);

    if (!is_stk_cc_in_progress)
    {
      // For proactive cases, we should just use the service type in the qmi message
      service_type = (voice_cc_sups_result_service_type_enum_v02)
        sups_ind_msg->supplementary_service_info.service_type;
    }
    QCRIL_LOG_INFO( "..sups_ind service_type %d", (int) service_type);

    if ((sups_ind_msg->data_source_valid &&
          // response from network
          (sups_ind_msg->data_source == VOICE_SUPS_DATA_SOURCE_NETWORK_V02)) &&
        !(sups_ind_msg->alpha_ident_valid &&
          (QCRIL_QMI_VOICE_ALPHA_LENGTH_IN_NULL_CASE == sups_ind_msg->alpha_ident.alpha_text_len)))
    {
      if (((!is_stk_cc_in_progress &&
              sups_ind_msg->supplementary_service_info.service_type == SERVICE_TYPE_USSD_V02 &&
              getVoiceModuleFeature(VoiceFeatureType::PROCESS_SUPS_IND)) || // for proactive cases
            (is_stk_cc_in_progress &&
             ((sups_ind_msg->call_id_valid && sups_ind_msg->call_id == stk_cc_info.call_id_info) ||
              (sups_ind_msg->supplementary_service_info.is_modified_by_call_control)))))
      {
        if (VOICE_CC_SUPS_RESULT_SERVICE_TYPE_USSD_V02 == service_type)
        {
          qcril_qmi_voice_stk_cc_notify_unsol_on_ussd_from_sups_ind(sups_ind_msg);
        }
        else
        {
          qcril_qmi_voice_stk_cc_notify_unsol_on_ss_from_sups_ind(sups_ind_msg);
        }
      }
    }

    if (((sups_ind_msg->data_source_valid &&
            sups_ind_msg->data_source == VOICE_SUPS_DATA_SOURCE_MS_V02) ||
          !sups_ind_msg->data_source_valid) &&
        sups_ind_msg->alpha_ident_valid)
    {
      /* this should cover the following scenarios.
         1) User initiated requests modified by STK CC.
         2) STK initiated requests modified by STK CC. */
      if (ALPHA_DCS_UCS2_V02 == sups_ind_msg->alpha_ident.alpha_dcs)
      {
        qcril_qmi_voice_transfer_sim_ucs2_alpha_to_std_ucs2_alpha(&sups_ind_msg->alpha_ident,
            &stk_cc_info.alpha_ident);
      }
      else
      {
        stk_cc_info.alpha_ident = sups_ind_msg->alpha_ident;
      }
      qcril_qmi_voice_stk_cc_relay_alpha_if_necessary(QCRIL_DEFAULT_INSTANCE_ID, TRUE);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_stk_cc_handle_voice_sups_ind

//===========================================================================
// qcril_qmi_voice_transfer_sim_alpha_to_std_alpha
//===========================================================================
void qcril_qmi_voice_transfer_sim_ucs2_alpha_to_std_ucs2_alpha(const voice_alpha_ident_type_v02 *sim_alpha, voice_alpha_ident_type_v02 *std_alpha)
{
  uint8_t idx;
  uint8_t coding_scheme;
  uint8_t num_of_char;
  uint16_t base_val;
  uint16_t ucs2_val;

  QCRIL_LOG_FUNC_ENTRY();

  memset(std_alpha, 0, sizeof(*std_alpha));

  do
  {
    if ( sim_alpha->alpha_text_len > 0)
    {
      coding_scheme = sim_alpha->alpha_text[0];
    }
    else
    {
      QCRIL_LOG_ERROR("alpha_text_len is 0");
      break;
    }
    QCRIL_LOG_INFO("coding scheme %x", (int)coding_scheme);

    idx = 1;
    switch (coding_scheme)
    {
      case 0x80:
        while ( (uint32_t)(idx+1) < sim_alpha->alpha_text_len )
        {
          std_alpha->alpha_text[idx-1] = sim_alpha->alpha_text[idx+1];
          std_alpha->alpha_text[idx] = sim_alpha->alpha_text[idx];
          idx += 2;
        }

        if ( (uint32_t)(idx+1) == sim_alpha->alpha_text_len && 0 != sim_alpha->alpha_text[idx] )
        {
          QCRIL_LOG_ERROR("an unexpected extra non-zero byte in source alpha buffer");
        }

        std_alpha->alpha_dcs = ALPHA_DCS_UCS2_V02;
        std_alpha->alpha_text_len = idx-1;
        break;

      case 0x81:
        if ( sim_alpha->alpha_text_len < 3 )
        {
          QCRIL_LOG_ERROR("sim_alpha->alpha_text_len (%d) less than 3", sim_alpha->alpha_text_len);
        }
        else
        {
          num_of_char = sim_alpha->alpha_text[1];
          base_val = sim_alpha->alpha_text[2];
          base_val <<= 7;
          idx = 3;

          if ( idx + num_of_char > sim_alpha->alpha_text_len )
          {
            QCRIL_LOG_DEBUG("num_of_char > sim_alpha->alpha_text_len - 3");
            num_of_char = sim_alpha->alpha_text_len - idx;
          }

          if (num_of_char * 2 > QMI_VOICE_ALPHA_TEXT_MAX_V02)
          {
            QCRIL_LOG_DEBUG("num_of_char * 2 > QMI_VOICE_ALPHA_TEXT_MAX_V02");
            num_of_char = QMI_VOICE_ALPHA_TEXT_MAX_V02 / 2;
          }

          int i;
          for ( i = 0; i< num_of_char && i < (QMI_VOICE_ALPHA_TEXT_MAX_V02 / 2); i++, idx++ )
          {
            ucs2_val = sim_alpha->alpha_text[idx];

            if ( ucs2_val >= 0x80 )
            {
              ucs2_val &= 0x7F;
              ucs2_val |= base_val;
            }

            std_alpha->alpha_text[2*i]   = (uint8_t) (ucs2_val);
            std_alpha->alpha_text[2*i+1] = (uint8_t) (ucs2_val >> 8);
          }
          std_alpha->alpha_dcs = ALPHA_DCS_UCS2_V02;
          std_alpha->alpha_text_len = num_of_char * 2;
        }

        break;

      case 0x82:
        if ( sim_alpha->alpha_text_len < 4 )
        {
          QCRIL_LOG_DEBUG("sim_alpha->alpha_text_len (%d) less than 4", sim_alpha->alpha_text_len);
        }
        else
        {
          num_of_char = sim_alpha->alpha_text[1];
          base_val = sim_alpha->alpha_text[2];
          base_val <<= 8;
          base_val += sim_alpha->alpha_text[3];
          idx = 4;

          if ( idx + num_of_char > sim_alpha->alpha_text_len )
          {
            QCRIL_LOG_DEBUG("num_of_char > sim_alpha->alpha_text_len - 4");
            num_of_char = sim_alpha->alpha_text_len - idx;
          }

          if (num_of_char * 2 > QMI_VOICE_ALPHA_TEXT_MAX_V02)
          {
            QCRIL_LOG_DEBUG("num_of_char * 2 > QMI_VOICE_ALPHA_TEXT_MAX_V02");
            num_of_char = QMI_VOICE_ALPHA_TEXT_MAX_V02 / 2;
          }

          int i;
          for ( i = 0; i< num_of_char && i < (QMI_VOICE_ALPHA_TEXT_MAX_V02 / 2); i++, idx++ )
          {
            ucs2_val = sim_alpha->alpha_text[idx];

            if ( ucs2_val >= 0x80 )
            {
              ucs2_val &= 0x7F;
              ucs2_val += base_val;
            }

            std_alpha->alpha_text[2*i]   = (uint8_t) (ucs2_val);
            std_alpha->alpha_text[2*i+1] = (uint8_t) (ucs2_val >> 8);
          }
          std_alpha->alpha_dcs = ALPHA_DCS_UCS2_V02;
          std_alpha->alpha_text_len = num_of_char * 2;
        }

        break;

      default:
        QCRIL_LOG_ERROR("unknown SIM coding scheme");
    }

  } while ( FALSE );

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_transfer_sim_alpha_to_std_alpha

//===========================================================================
// qcril_qmi_voice_stk_cc_dump
//===========================================================================
void qcril_qmi_voice_stk_cc_dump(void)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("... modification %d, call_id %d, is_ims_request %d ",
      (int) stk_cc_info.modification, (int) stk_cc_info.call_id_info,
      (int) stk_cc_info.is_ims_request);
  QCRIL_LOG_INFO("... ss svc type %d, ss reason %d ", (int) stk_cc_info.ss_ussd_info.service_type,
      (int) stk_cc_info.ss_ussd_info.reason);
  QCRIL_LOG_INFO("... is_alpha_relayed %d ", (int) stk_cc_info.is_alpha_relayed);
} // qcril_qmi_voice_stk_cc_dump

//===========================================================================
// qcril_qmi_voice_stk_ss_resp_handle
//===========================================================================
RIL_Errno qcril_qmi_voice_stk_ss_resp_handle
(
 uint16_t msg_token_id,
 qmi_response_type_v01* resp,
 uint8_t alpha_ident_valid,
 voice_alpha_ident_type_v02* alpha_ident,
 uint8_t call_id_valid,
 uint8_t call_id,
 uint8_t cc_sups_result_valid,
 voice_cc_sups_result_type_v02* cc_sups_result,
 uint8_t cc_result_type_valid,
 voice_cc_result_type_enum_v02* cc_result_type
)
{
  RIL_Errno ril_err;
  qcril_qmi_voice_stk_cc_modification_e_type stk_cc_modification;
//  qcril_request_resp_params_type ril_response;
  qmi_ril_err_ctx_ss_resp_data_type ss_op_info;

  QCRIL_LOG_INFO( "cidv %d, ccrv %d, ccr %p", (int)call_id_valid, (int)cc_sups_result_valid, cc_sups_result );

  memset( &ss_op_info, 0, sizeof( ss_op_info ));
  ss_op_info.cc_sups_result_valid = cc_sups_result_valid;
  ss_op_info.cc_sups_result       = cc_sups_result;
  ss_op_info.cc_result_type_valid = cc_result_type_valid;
  ss_op_info.cc_result_type       = cc_result_type;

  ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex( QMI_NO_ERR,
                                                                        resp,
                                                                        QCRIL_QMI_ERR_CTX_SEND_SS_TXN,
                                                                        (void*)&ss_op_info );
  QCRIL_LOG_INFO("STK SS RESP : ril_err=%d, result=%d, error=%d", ril_err,resp->result, resp->error);


  stk_cc_modification = qcril_qmi_voice_map_ril_error_to_stk_cc_modification_type(ril_err);

  if ( QCRIL_QMI_VOICE_STK_CC_MODIFICATION_NONE != stk_cc_modification )
  { // STK CC session started
    qcril_qmi_voice_reset_stk_cc();

    stk_cc_info.modification                      = stk_cc_modification;
    stk_cc_info.is_alpha_relayed                  = FALSE;

    if ( call_id_valid )
    {
      stk_cc_info.call_id_info = call_id;
    }

    if ( cc_sups_result_valid )
    {
      stk_cc_info.ss_ussd_info = *cc_sups_result;
    }

    if ( alpha_ident_valid )
    {
      stk_cc_info.alpha_ident = *alpha_ident;
    }
    else
    {
      memset( &stk_cc_info.alpha_ident, 0, sizeof( stk_cc_info.alpha_ident ) );
    }

    // msg_token_id
    auto pendingMsg = findVoiceModulePendingMessage(msg_token_id);
    if (pendingMsg)
    {
      auto msg = std::static_pointer_cast<QcRilRequestMessage>(pendingMsg);
      if (msg->isImsRequest()) {
        stk_cc_info.is_ims_request = TRUE;
      }
    }

    QCRIL_LOG_INFO( "org req altered ril_err %d, call_id %d ", (int)ril_err, (int)call_id );

    qcril_qmi_voice_stk_cc_dump();
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);

  return ril_err;
} // qcril_qmi_voice_stk_ss_resp_handle

//===========================================================================
// qmi_ril_voice_is_under_any_voice_calls
//===========================================================================
int qmi_ril_voice_is_under_any_voice_calls(void)
{
  int                                         res;
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  res = FALSE;

  qcril_qmi_voice_voip_lock_overview();

  call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
  while ( NULL != call_info_entry && !res )
  {
    if ( VOICE_INVALID_CALL_ID != call_info_entry->android_call_id &&
         ( CALL_TYPE_VOICE_V02 == call_info_entry->voice_scv_info.call_type ||
           CALL_TYPE_EMERGENCY_V02 == call_info_entry->voice_scv_info.call_type )
       )
    {
      res = TRUE;
    }
    else
    {
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
    }
  }

  qcril_qmi_voice_voip_unlock_overview();

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;
} // qmi_ril_voice_is_under_any_voice_calls

// ---------------------------------------------- VOIP utilities -------------------------------------------------------------------
//===========================================================================
// qcril_qmi_voice_voip_create_call_info_entry
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_create_call_info_entry(
        uint8_t call_qmi_id,
        int32_t call_media_id,
        int need_allocate_call_android_id,
        qcril_qmi_voice_voip_call_info_elaboration_type initial_elaboration,
        RIL_Errno *ril_err )
{
  uint8_t call_android_id = VOICE_INVALID_CALL_ID;
  qcril_qmi_voice_voip_call_info_entry_type* res =
        (qcril_qmi_voice_voip_call_info_entry_type*)qcril_malloc( sizeof( *res ) );
  RIL_Errno id_alloc_res = RIL_E_SUCCESS;
  int   finalized_ok;

  if ( res )
  {
    finalized_ok = FALSE;
    memset( res, 0, sizeof( *res ) );
    res->ringing_time_id = TimeKeeper::no_timer;
    res->call_obj_phase_out_timer_id = TimeKeeper::no_timer;

    do
    {
      res->elaboration = initial_elaboration;

      if ( need_allocate_call_android_id )
      {
        id_alloc_res = qcril_qmi_voice_voip_allocate_call_android_id ( &call_android_id );
        if ( RIL_E_SUCCESS != id_alloc_res )
            break;
      }

      res->android_call_id = call_android_id;

      res->qmi_call_id = call_qmi_id;

      res->media_id = call_media_id;

      res->next = qmi_voice_voip_overview.call_info_root;
      qmi_voice_voip_overview.call_info_root = res;

      finalized_ok = TRUE;
    } while (FALSE);

    if ( !finalized_ok )
    { // rollback
      qcril_free( res );
      res = NULL;
      id_alloc_res = RIL_E_NO_RESOURCES;
    }
  }
  else
  {
    id_alloc_res = RIL_E_NO_MEMORY;
  }

  QCRIL_LOG_ESSENTIAL("Created call info entry %p with call android id %d, qmi id %d, media id %d",
                        res, (int) call_android_id, call_qmi_id, call_media_id);
  if(ril_err != NULL)
  {
    *ril_err = id_alloc_res;
  }

  return res;
} // qcril_qmi_voice_voip_create_call_info_entry

//===========================================================================
// qcril_qmi_voice_voip_destroy_call_info_entry
//===========================================================================
void qcril_qmi_voice_voip_destroy_call_info_entry( qcril_qmi_voice_voip_call_info_entry_type* entry )
{
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* prev = NULL;
  int found;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO(  ".. entry %p", entry );

  if ( entry )
  {

    iter = qmi_voice_voip_overview.call_info_root;
    prev = NULL;
    found = FALSE;

    while ( iter != NULL && !found )
    {
      if ( iter == entry )
      {
        found = TRUE;
      }
      else
      {
        prev = iter;
        iter = iter->next;
      }
    }
    QCRIL_LOG_INFO( ".. entry found ok %d", found );
    if ( found )
    {
      if ( entry == qmi_voice_voip_overview.call_info_enumeration_current )
      {
        qmi_voice_voip_overview.call_info_enumeration_current = entry->next;
      }

      if ( NULL == prev )
      { // first
        qmi_voice_voip_overview.call_info_root = entry->next;
      }
      else
      {
        prev->next = entry->next;
      }

      // destroy
      if ( !(entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_GOLDEN) )
      {
        //destroy the cdma emergency voice call handling entries
        if( entry->emer_voice_number.number )
        {
          qcril_free(entry->emer_voice_number.number);
        }

        // destroy phaseout timer if needed
        if (TimeKeeper::no_timer != entry->call_obj_phase_out_timer_id)
        {
          TimeKeeper::getInstance ().clear_timer(entry->call_obj_phase_out_timer_id);
          entry->call_obj_phase_out_timer_id = TimeKeeper::no_timer;
        }

        // destroy additional call info buffer
        if( entry->additional_call_info.buffer )
        {
          qcril_free(entry->additional_call_info.buffer);
        }

        // destroy STK CC emulation overlays
        if ( entry->overlayed_number_storage_for_emulated_stk_cc )
        {
          qcril_free( entry->overlayed_number_storage_for_emulated_stk_cc );
        }
        if ( entry->overlayed_name_storage_for_emulated_stk_cc )
        {
          qcril_free( entry->overlayed_name_storage_for_emulated_stk_cc );
        }

        if (entry->terminating_num)
        {
          qcril_free(entry->terminating_num);
        }
        if (entry->call_reason)
        {
          qcril_free(entry->call_reason);
        }
        qcril_free( entry );
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_voip_destroy_call_info_entry

//===========================================================================
// qcril_qmi_voice_voip_allocate_call_android_id
//===========================================================================
RIL_Errno qcril_qmi_voice_voip_allocate_call_android_id( uint8_t* new_call_android_id )
{
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  uint8_t call_id;
  uint8_t generated_call_id;
  int generated;
  int found;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  generated_call_id = VOICE_INVALID_CALL_ID;

  if ( new_call_android_id )
  {
    generated = FALSE;
    for ( call_id = VOICE_LOWEST_CALL_ID; call_id <= VOICE_HIGHEST_CALL_ID && !generated; call_id++ )
    {
      iter = qmi_voice_voip_overview.call_info_root;
      found = FALSE;
      while ( iter != NULL && !found )
      {
        if ( call_id == iter->android_call_id )
        {
          found = TRUE;
          break;
        }
        iter = iter->next;
      }
      if ( !found )
      {
        generated_call_id = call_id;
        generated = TRUE;
      }
    }

    if ( generated )
    {
      res = RIL_E_SUCCESS;
    }

    *new_call_android_id = generated_call_id;
  }

  QCRIL_LOG_INFO( "returns res %d and id %d", (int) res, (int)generated_call_id );

  return res;
} // qcril_qmi_voice_voip_allocate_call_android_id

//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id( uint8_t call_qmi_id )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  QCRIL_LOG_INFO( "entry with id %d", (int)call_qmi_id );
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( call_qmi_id == iter->qmi_call_id )
    {
        res = iter;
    }
    else
    {
        iter = iter->next;
    }
  }


  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_call_qmi_id

//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id( uint8_t call_andoid_id )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  QCRIL_LOG_INFO( "entry with id %d", (int)call_andoid_id );
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( call_andoid_id == iter->android_call_id )
    {
      res = iter;
      break;
    }
    iter = iter->next;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_call_android_id

//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state( RIL_CallState ril_call_state )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  QCRIL_LOG_INFO( "entry with ril call state %d", (int)ril_call_state );
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( ( iter->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RIL_CALL_STATE_VALID ) && ( ril_call_state == iter->ril_call_state ) )
    {
        res = iter;
    }
    else
    {
        iter = iter->next;
    }
  }


  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_andoid_call_state
//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_elaboration
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_elaboration( qcril_qmi_voice_voip_call_info_elaboration_type elaboration_pattern, int pattern_present )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_call_info_elaboration_type expected_after_application;

  QCRIL_LOG_INFO( "entry with elaboration %x, %x hex", (uint32_t)(elaboration_pattern >> 32),(uint32_t)elaboration_pattern );

  if ( pattern_present )
  {
    expected_after_application  = elaboration_pattern;
  }
  else
  {
    expected_after_application  = QMI_RIL_ZERO;
  }

  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( expected_after_application == (elaboration_pattern & iter->elaboration) ) // all bits
    {
      res = iter;
      break;
    }
    iter = iter->next;
  }

  if ( NULL != res )
  {
    QCRIL_LOG_INFO( ".. found %p - with call android id %d, call qmi id %d", res, (int)res->android_call_id, (int)res->qmi_call_id );
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_elaboration
//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_elaboration_any_subset
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_elaboration_any_subset( qcril_qmi_voice_voip_call_info_elaboration_type elaboration_pattern )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  QCRIL_LOG_INFO( "entry with pattern %d", (int)elaboration_pattern );


  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( elaboration_pattern & iter->elaboration ) // any of requested bits
    {
        res = iter;
    }
    else
    {
        iter = iter->next;
    }
  }

  if ( NULL != res )
  {
    QCRIL_LOG_INFO( ".. found %p - with call android id %d, call qmi id %d", res, (int)res->android_call_id, (int)res->qmi_call_id );
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_elaboration_any_subset

//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_single_elaboration_extended
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_single_elaboration_extended( qcril_qmi_voice_voip_call_info_elaboration_type elaboration_single,
                                                                                                                     int single_present )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_call_info_elaboration_type expected_after_application;

  int                                             is_extended;
  qcril_qmi_voice_voip_call_info_elaboration_type extended_ela_value;

  extended_ela_value = 0;
  is_extended = qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust( elaboration_single, &extended_ela_value );

  if ( single_present )
  {
     if ( is_extended )
     {
        expected_after_application = extended_ela_value;
     }
     else
     {
       expected_after_application  = elaboration_single;
     }
  }
  else
  {
    expected_after_application  = QMI_RIL_ZERO;
  }

  QCRIL_LOG_INFO( "lookup %x, %x hex, is extended %d", (uint32_t)(elaboration_single >> 32),(uint32_t)elaboration_single, is_extended );

  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if (  ( !is_extended && ( expected_after_application == ( elaboration_single & iter->elaboration ) ) )
        ||
        ( is_extended && ( expected_after_application == ( extended_ela_value & iter->elaboration_extended )  ) )
       )
    {
      res = iter;
      break;
    }
    iter = iter->next;
  }

  if ( NULL != res )
  {
    QCRIL_LOG_INFO( ".. found %p - with call android id %d, call qmi id %d", res, (int)res->android_call_id, (int)res->qmi_call_id );
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_single_elaboration_extended


//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_conn_uri
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_conn_uri( const char *conn_uri )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  QCRIL_LOG_INFO( "entry with conn_uri %s", conn_uri);
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( !strcmp( conn_uri, iter->voice_svc_remote_party_number.number ) )
    {
      res = iter;
      break;
    }
    iter = iter->next;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_conn_uri

//===========================================================================
// qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state( call_state_enum_v02 qmi_call_state )
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;
  qcril_qmi_voice_voip_call_info_entry_type* iter;

  QCRIL_LOG_INFO( "seeking entry with qmi call state %d", (int)qmi_call_state );
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL && NULL == res )
  {
    if ( qmi_call_state == iter->voice_scv_info.call_state )
    {
        res = iter;
    }
    else
    {
        iter = iter->next;
    }
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state

//===========================================================================
// qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust
//
// we check if elaboration_single_input belongs to original 64 bit or to extended range
// in later case we provide via elaboration_single_adjusted a bit mask for extended elabotaion check
//===========================================================================
int qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust( qcril_qmi_voice_voip_call_info_elaboration_type elaboration_single_input,
                                                                   qcril_qmi_voice_voip_call_info_elaboration_type * elaboration_single_adjusted )
{
   int res;
   if ( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NONE == elaboration_single_input )
   { // original elaboration range
      res = FALSE;
   }
   else
   if ( QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_FIRST == elaboration_single_input ||
        (0 == (elaboration_single_input & 1) )
      )
   { // in initial elaboration all values are even except QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_FIRST
      res = FALSE;
   }
   else
   {
      res = TRUE;
   }
   if ( res && NULL != elaboration_single_adjusted )
   {
      *elaboration_single_adjusted = elaboration_single_input ^ 1;
   }

   return res;
} // qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust

//===========================================================================
// qcril_qmi_voice_voip_call_info_is_single_elaboration_set
//===========================================================================
int qcril_qmi_voice_voip_call_info_is_single_elaboration_set( const qcril_qmi_voice_voip_call_info_entry_type* entry, qcril_qmi_voice_voip_call_info_elaboration_type single_elaboration )
{
   int                                             res = 0;
   int                                             is_extended;
   qcril_qmi_voice_voip_call_info_elaboration_type single_elaboration_extended_adjusted;

   if ( NULL != entry )
   {
      single_elaboration_extended_adjusted = 0;
      is_extended = qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust( single_elaboration, &single_elaboration_extended_adjusted );

      if (!is_extended)
      {
         res = ( 0 != ( entry->elaboration & single_elaboration ) );
      }
      else
      {
         res = ( 0 != ( entry->elaboration_extended & single_elaboration_extended_adjusted ) );
      }
   }

   return res;
} // qcril_qmi_voice_voip_call_info_is_single_elaboration_set
//===========================================================================
// qcril_qmi_voice_voip_call_info_set_single_elaboration
//===========================================================================
void qcril_qmi_voice_voip_call_info_set_single_elaboration( qcril_qmi_voice_voip_call_info_entry_type* entry, qcril_qmi_voice_voip_call_info_elaboration_type single_elaboration, int is_set )
{
   int                                             is_extended;
   qcril_qmi_voice_voip_call_info_elaboration_type single_elaboration_extended_adjusted;

   if ( NULL != entry )
   {
      single_elaboration_extended_adjusted = 0;
      is_extended = qcril_qmi_voice_voip_is_elaboration_extended_check_and_adjust( single_elaboration, &single_elaboration_extended_adjusted );

      if ( !is_extended )
      {
         if ( is_set )
         {
            entry->elaboration |= single_elaboration;
         }
         else
         {
            entry->elaboration &= ( 0xFFFFFFFFFFFFFFFFUL ^ single_elaboration );
         }
      }
      else
      {
         if ( is_set )
         {
            entry->elaboration_extended |= single_elaboration_extended_adjusted;
         }
         else
         {
            entry->elaboration_extended &= ( 0xFFFFFFFFFFFFFFFFUL ^ single_elaboration_extended_adjusted );
         }
      }
   }

} // qcril_qmi_voice_voip_call_info_set_single_elaboration


//===========================================================================
// qcril_qmi_voice_consider_shadow_remote_number_cpy_creation
//===========================================================================
void qcril_qmi_voice_consider_shadow_remote_number_cpy_creation( qcril_qmi_voice_voip_call_info_entry_type* entry )
{
  QCRIL_LOG_INFO( "param %p", entry );

  if ( NULL != entry )
  {
    if ( ( CALL_TYPE_VOICE_V02 == entry->voice_scv_info.call_type ||
         CALL_TYPE_EMERGENCY_V02 == entry->voice_scv_info.call_type ) &&
         entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID &&
         !qcril_qmi_voice_is_emer_voice_entry_valid(&entry->emer_voice_number) )
    {
      QCRIL_LOG_INFO( "shadow created for voice/emergency call" );
      qcril_qmi_voice_create_emer_voice_entry(&entry->emer_voice_number, &entry->voice_svc_remote_party_number);
    }
  }
} // qcril_qmi_voice_consider_shadow_remote_number_cpy_creation

//===========================================================================
// qcril_qmi_voice_call_info_entry_set_call_modified_cause
//===========================================================================
void qcril_qmi_voice_call_info_entry_set_call_modified_cause
(
 qcril_qmi_voice_voip_call_info_entry_type *entry,
 voice_call_modified_cause_enum_v02         call_modified_cause
)
{
  if (entry)
  {
    QCRIL_LOG_INFO("Set call_modified_cause: %d for call info entry: %p"
                   " (call android id %d, call qmi id %d)",
                   call_modified_cause, entry,
                   (int)entry->android_call_id, (int)entry->qmi_call_id);

    entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_MOD_CAUSE_VALID;
    entry->call_modified_cause       = call_modified_cause;
  }
}

//===========================================================================
// qcril_qmi_voice_voip_update_call_info_entry_mainstream
//===========================================================================
void qcril_qmi_voice_voip_update_call_info_entry_mainstream
(
 qcril_qmi_voice_voip_call_info_entry_type* entry,
 voice_all_call_status_ind_msg_v02* call_status_ind,
 uint8_t ril_call_state_valid,
 RIL_CallState ril_call_state
)
{
  char alpha_remote_name[QMI_VOICE_CALLER_NAME_MAX_V02+1] = {0};
  int alpha_remote_name_len = 0;
  uint8_t remote_party_name_len = 0;
  unsigned int j = 0;
  uint8_t qmi_call_id;
  boolean need_update_remote_party_details = FALSE;

  QCRIL_LOG_INFO("param %p", entry);
  if (entry && call_status_ind)
  {
    QCRIL_LOG_INFO( "call android id %d, call qmi id %d",
        (int)entry->android_call_id, (int)entry->qmi_call_id);
    qmi_call_id = entry->qmi_call_id;

    // call_info
    for (j = 0; (j < call_status_ind->call_info_len && j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
    {
      if (call_status_ind->call_info[j].call_id == qmi_call_id)
      {
        entry->voice_scv_info = call_status_ind->call_info[j];
        QCRIL_LOG_INFO( ".. call state %d, call type %d, call mode %d",
            (int)entry->voice_scv_info.call_state, (int)entry->voice_scv_info.call_type,
            (int)entry->voice_scv_info.mode );

        // Update the remote party number/ip number if the call is in incoming state.
        if ((entry->voice_scv_info.call_state == CALL_STATE_INCOMING_V02) ||
            (entry->voice_scv_info.call_state == CALL_STATE_WAITING_V02) ||
            (entry->voice_scv_info.call_state == CALL_STATE_SETUP_V02))
        {
          need_update_remote_party_details =  TRUE;
        }
        break;
      }
    }

    if (ril_call_state_valid)
    {
      entry->ril_call_state = ril_call_state;
      entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RIL_CALL_STATE_VALID;
    }

    /* If the alpha identifier is provided by the UICC, this is an indication that the terminal should not give any information
       to the user on the changes made by the UICC to the initial user request. So, We have to ignore modified number in remote
       party field if Alpha text is valid.
       */
    if (call_status_ind->ip_num_info_valid)
    {
      for (j = 0; (j < call_status_ind->ip_num_info_len &&
            j < QMI_VOICE_REMOTE_PARTY_NUMBER_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->ip_num_info[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_IP_NUMBER_VALID;
          if (need_update_remote_party_details ||
              (!call_status_ind->alpha_id_valid &&
               !(getVoiceModuleFeature(VoiceFeatureType::IS_ORIGINAL_NUMBER_DISPLAY))))
          {
            entry->voice_svc_remote_party_ip_number = call_status_ind->ip_num_info[j];
          }
          break;
        }
      }
    }

    //  Is Connected Number ECT
    if (call_status_ind->is_connected_number_ECT_valid)
    {
      for (j = 0; (j < call_status_ind->is_connected_number_ECT_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->is_connected_number_ECT[j].call_id == qmi_call_id)
        {
          // Modify remote party number if it is modified due to call transfer
          // in CONVERSATION/HOLD states.
          if (call_status_ind->is_connected_number_ECT[j].is_connected_number_ECT &&
              ((entry->voice_scv_info.call_state == CALL_STATE_CONVERSATION_V02) ||
               (entry->voice_scv_info.call_state == CALL_STATE_HOLD_V02)))
          {
            need_update_remote_party_details = TRUE;
          }
          break;
        }
      }
    }

    // remote party number
    if (call_status_ind->remote_party_number_valid)
    {
      for (j = 0; (j < call_status_ind->remote_party_number_len &&
            j < QMI_VOICE_REMOTE_PARTY_NUMBER_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->remote_party_number[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NUMBER_VALID;
          if (need_update_remote_party_details ||
              (!call_status_ind->alpha_id_valid &&
               !(getVoiceModuleFeature(VoiceFeatureType::IS_ORIGINAL_NUMBER_DISPLAY))))
          {
            entry->voice_svc_remote_party_number = call_status_ind->remote_party_number[j];
          }
          break;
        }
      }
    }

    if (call_status_ind->remote_party_name_valid)
    {
      for (j = 0; (j < call_status_ind->remote_party_name_len &&
            j < QMI_VOICE_REMOTE_PARTY_NAME_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->remote_party_name[j].call_id == qmi_call_id)
        {
          voice_remote_party_name2_type_v02 *remote_party_name =
            &call_status_ind->remote_party_name[j];

          entry->voice_svc_remote_party_name.call_id = remote_party_name->call_id;
          entry->voice_svc_remote_party_name.name_pi = remote_party_name->name_pi;

          *(entry->voice_svc_remote_party_name.name) = 0;

          QCRIL_LOG_INFO("remote party name - coding scheme %d, len %d",
              remote_party_name->coding_scheme, remote_party_name->name_len);

          remote_party_name_len = ((remote_party_name->name_len > QMI_VOICE_CALLER_NAME_MAX_V02) ?
              QMI_VOICE_CALLER_NAME_MAX_V02 : remote_party_name->name_len);

          if (entry->voice_scv_info.mode == CALL_MODE_CDMA_V02)
          {
            // convert ascii to utf8
            QCRIL_LOG_INFO("remote party name - call mode CDMA; convert ascii to utf8\n");
            entry->voice_svc_remote_party_name.name_len =
              qcril_cm_ss_ascii_to_utf8((unsigned char*) remote_party_name->name,
                  remote_party_name_len,
                  entry->voice_svc_remote_party_name.name,
                  sizeof(entry->voice_svc_remote_party_name.name));
          }
          else
          {
            entry->voice_svc_remote_party_name.name_len =
              qcril_cm_ss_convert_ussd_string_to_utf8(remote_party_name->coding_scheme,
                  (uint8_t *)remote_party_name->name,
                  remote_party_name_len,
                  entry->voice_svc_remote_party_name.name,
                  sizeof(entry->voice_svc_remote_party_name.name));
          }

          if ((!*entry->voice_svc_remote_party_name.name) ||
              (entry->voice_svc_remote_party_name.name_len >=
               sizeof(entry->voice_svc_remote_party_name.name)))
          {
            QCRIL_LOG_ERROR("Invalid conversion results, remote name");
          }
          else
          {
            entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NAME_VALID;
          }
          break;
        }
      }
    }

    if (call_status_ind->alerting_type_valid)
    {
      for (j = 0; (j < call_status_ind->alerting_type_len &&
            j < QMI_VOICE_ALERTING_TYPE_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->alerting_type[j].call_id == qmi_call_id)
        {
          entry->voice_svc_alerting_type = call_status_ind->alerting_type[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALERTING_TYPE_VALID;
          break;
        }
      }
    }

    if (call_status_ind->alpha_id_valid)
    {
      for (j = 0; (j < call_status_ind->alpha_id_len &&
            j < QMI_VOICE_ALPHA_IDENT_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->alpha_id[j].call_id == qmi_call_id)
        {
          voice_alpha_ident_with_id_type_v02 *alpha_id = &call_status_ind->alpha_id[j];

          alpha_id->alpha_text_len = (alpha_id->alpha_text_len>QMI_VOICE_ALPHA_TEXT_MAX_V02) ?
            QMI_VOICE_ALPHA_TEXT_MAX_V02 : alpha_id->alpha_text_len;

          if (!call_status_ind->remote_party_name_valid)
          {
            if (ALPHA_DCS_GSM_V02 == alpha_id->alpha_dcs)
            {
              alpha_remote_name_len = qcril_cm_ss_convert_gsm8bit_alpha_string_to_utf8(
                  (char *)alpha_id->alpha_text,
                  alpha_id->alpha_text_len,
                  alpha_remote_name,
                  sizeof(alpha_remote_name));
            }
            else
            {
              alpha_remote_name_len = qcril_cm_ss_convert_ucs2_to_utf8(
                  (char *)alpha_id->alpha_text,
                  alpha_id->alpha_text_len * 2,
                  alpha_remote_name,
                  sizeof(alpha_remote_name));
            }

            entry->voice_svc_remote_party_name.call_id = entry->qmi_call_id;
            entry->voice_svc_remote_party_name.name_pi = PRESENTATION_NAME_PRESENTATION_ALLOWED_V02;
            entry->voice_svc_remote_party_name.name_len = alpha_remote_name_len;
            strlcpy(entry->voice_svc_remote_party_name.name,
                alpha_remote_name, sizeof(entry->voice_svc_remote_party_name.name));
            entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_NAME_VALID;

            QCRIL_LOG_ESSENTIAL("Alpha string %s", entry->voice_svc_remote_party_name.name);
          }
          break;
        }
      }
    }
    if (call_status_ind->conn_ip_num_info_valid)
    {
      for (j = 0; (j < call_status_ind->conn_ip_num_info_len &&
            j < QMI_VOICE_CONNECTED_PARTY_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->conn_ip_num_info[j].call_id == qmi_call_id)
        {
          entry->voice_svc_conn_party_ip_num = call_status_ind->conn_ip_num_info[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CONN_PARTY_IP_NUM_VALID;
          break;
        }
      }
    }
    if (call_status_ind->conn_party_num_valid)
    {
      for (j = 0; (j < call_status_ind->conn_party_num_len &&
            j < QMI_VOICE_CONNECTED_PARTY_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->conn_party_num[j].call_id == qmi_call_id)
        {
          entry->voice_svc_conn_party_num = call_status_ind->conn_party_num[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CONN_PARTY_NUM_VALID;
          break;
        }
      }
    }
    if (call_status_ind->redirecting_party_num_valid)
    {
      for (j = 0; (j < call_status_ind->redirecting_party_num_len &&
            j < QMI_VOICE_REDIRECTING_PARTY_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->redirecting_party_num[j].call_id == qmi_call_id)
        {
          entry->voice_svc_redirecting_party_num = call_status_ind->redirecting_party_num[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_REDIRECTING_PARTY_NUM_VALID;
          break;
        }
      }
    }
    if (call_status_ind->audio_attrib_valid)
    {
      for (j = 0; (j < call_status_ind->audio_attrib_len &&
            j < QMI_VOICE_CALL_ATTRIBUTES_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->audio_attrib[j].call_id == qmi_call_id)
        {
          entry->voice_audio_attrib = call_status_ind->audio_attrib[j];
          entry->elaboration |=  QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUDIO_ATTR_VALID;
          break;
        }
      }
    }
    if (call_status_ind->video_attrib_valid)
    {
      for (j = 0; (j < call_status_ind->video_attrib_len &&
            j < QMI_VOICE_CALL_ATTRIBUTES_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->video_attrib[j].call_id == qmi_call_id)
        {
          entry->voice_video_attrib = call_status_ind->video_attrib[j];
          entry->elaboration |=  QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VIDEO_ATTR_VALID;
          break;
        }
      }
    }
    if (call_status_ind->call_attrib_status_valid)
    {
      for (j = 0; (j < call_status_ind->call_attrib_status_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->call_attrib_status[j].call_id == qmi_call_id)
        {
          entry->call_attrib_status = call_status_ind->call_attrib_status[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ATTR_VALID;
          break;
        }
      }
    }
    if (call_status_ind->rtt_mode_valid)
    {
      for (j = 0; (j < call_status_ind->rtt_mode_len &&
            j < QMI_VOICE_RTT_MODE_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->rtt_mode[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_MODE_VALID;
          entry->rtt_mode = call_status_ind->rtt_mode[j];
          break;
        }
      }
    }
    if (call_status_ind->rtt_capabilities_info_valid)
    {
      for (j = 0; (j < call_status_ind->rtt_capabilities_info_len  &&
            j < QMI_VOICE_RTT_MODE_ARRAY_MAX_V02); j++)
      {
        if ( call_status_ind->rtt_capabilities_info[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_RTT_CAP_INFO_VALID;
          entry->rtt_capabilities_info = call_status_ind->rtt_capabilities_info[j];
          break;
        }
      }
    }
    if (call_status_ind->is_srvcc_valid)
    {
      for (j = 0; (j < call_status_ind->is_srvcc_len &&
            j < QMI_VOICE_IS_SRVCC_CALL_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->is_srvcc[j].call_id == qmi_call_id)
        {
          if (TRUE == call_status_ind->is_srvcc[j].is_srvcc_call)
          {
            entry->is_srvcc = call_status_ind->is_srvcc[j];
            entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID;
            entry->srvcc_in_progress = FALSE;
          }
          break;
        }
      }
    }

    /* In case of SRVCC as call type get modified to voice, reset voip mask */
    if ((entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_SRVCC_VALID) &&
        (entry->voice_scv_info.call_type == CALL_TYPE_VOICE_V02))
    {
      if (entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN)
      {
        entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN;
        QCRIL_LOG_DEBUG("resetting the voip mask as call got modified to voice");
      }
    }

    if (call_status_ind->local_call_capabilities_info_valid)
    {
      for (j = 0; (j < call_status_ind->local_call_capabilities_info_len &&
            j < QMI_VOICE_CALL_CAPABILITIES_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->local_call_capabilities_info[j].call_id == qmi_call_id)
        {
          entry->local_call_capabilities_info = call_status_ind->local_call_capabilities_info[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LOCAL_CALL_CAPBILITIES_VALID;
          break;
        }
      }
    }
    if (call_status_ind->peer_call_capabilities_info_valid)
    {
      for (j = 0; (j < call_status_ind->peer_call_capabilities_info_len &&
            j < QMI_VOICE_CALL_CAPABILITIES_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->peer_call_capabilities_info[j].call_id == qmi_call_id)
        {
          entry->peer_call_capabilities_info = call_status_ind->peer_call_capabilities_info[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PEER_CALL_CAPBILITIES_VALID;
          break;
        }
      }
    }
    if (call_status_ind->child_number_valid)
    {
      for (j = 0; (j < call_status_ind->child_number_len &&
            j < QMI_VOICE_CHILD_NUMBER_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->child_number[j].call_id == qmi_call_id)
        {
          entry->child_number = call_status_ind->child_number[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_CHILD_NUMBER_VALID;
          break;
        }
      }
    }
    if (call_status_ind->display_text_valid)
    {
      for (j = 0; (j < call_status_ind->display_text_len &&
            j < QMI_VOICE_DISPLAY_TEXT_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->display_text[j].call_id == qmi_call_id)
        {
          entry->display_text = call_status_ind->display_text[j];
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_DISPLAY_TEXT_VALID;
          break;
        }
      }
    }
    if (call_status_ind->is_add_info_present_valid)
    {
      for (j = 0; (j < call_status_ind->is_add_info_present_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->is_add_info_present[j].call_id == qmi_call_id)
        {
          entry->additional_call_info.is_add_info_present =
            call_status_ind->is_add_info_present[j].is_add_info_present;
          break;
        }
      }
    }
    if (call_status_ind->ip_caller_name_valid)
    {
      for (j = 0; (j < call_status_ind->ip_caller_name_len &&
            j < QMI_VOICE_IP_CALLER_NAME_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->ip_caller_name[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IP_CALLER_NAME_VALID;
          entry->ip_caller_name = call_status_ind->ip_caller_name[j];
          break;
        }
      }
    }
    if (call_status_ind->end_reason_text_valid)
    {
      for (j = 0; (j < call_status_ind->end_reason_text_len &&
            j < QMI_VOICE_IP_END_REASON_TEXT_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->end_reason_text[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_END_REASON_TEXT_VALID;
          entry->end_reason_text = call_status_ind->end_reason_text[j];
          break;
        }
      }
    }
    if (call_status_ind->orig_fail_reason_valid)
    {
      for (j = 0; (j < call_status_ind->orig_fail_reason_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->orig_fail_reason[j].call_id == qmi_call_id)
        {
          entry->lcf_valid = TRUE;
          entry->lcf = qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
          entry->lcf_extended_codes = call_status_ind->orig_fail_reason[j].orig_fail_reason;
          QCRIL_LOG_INFO(".. orig_fail_reason 0x%x",
              call_status_ind->orig_fail_reason[j].orig_fail_reason);
          break;
        }
      }
    }
    if (call_status_ind->is_secure_call_valid)
    {
      for (j = 0; (j < call_status_ind->is_secure_call_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->is_secure_call[j].call_id == qmi_call_id)
        {
          entry->is_secure_call = call_status_ind->is_secure_call[j].is_secure_call;
          break;
        }
      }
    }
    entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_CALLED_PARTY_RING_VALID;
    if (call_status_ind->is_called_party_ringing_valid)
    {
      for (j = 0; (j < call_status_ind->is_called_party_ringing_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->is_called_party_ringing[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_IS_CALLED_PARTY_RING_VALID;
          entry->is_called_party_ringing = call_status_ind->is_called_party_ringing[j];
          QCRIL_LOG_INFO(".. is_called_party_ringing %d",
              call_status_ind->is_called_party_ringing[j].is_called_party_ringing);
          break;
        }
      }
    }
    if (call_status_ind->sip_error_code_valid)
    {
      for (j = 0; (j < call_status_ind->sip_error_code_len &&
            j < QMI_VOICE_IP_END_SIP_CODE_ARRAY_MAX_V02); j++)
      {
        if ( call_status_ind->sip_error_code[ j ].call_id == qmi_call_id )
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_SIP_ERROR_CODE_VALID;
          entry->sip_error_code = call_status_ind->sip_error_code[ j ];
          break;
        }
      }
    }
    if (call_status_ind->alternate_sip_uris_valid)
    {
      for (j = 0; (j < call_status_ind->alternate_sip_uris_len &&
            j < QMI_VOICE_ALTERNATE_SIP_URI_ARRAY_MAX_V02); j++)
      {
        if ( call_status_ind->alternate_sip_uris[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ALTERNATE_SIP_URI_VALID;
          entry->alternate_sip_uri = call_status_ind->alternate_sip_uris[j];
          break;
        }
      }
    }
    entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HISTORY_INFO_VALID;
    if (call_status_ind->ip_hist_info_valid)
    {
      for (j = 0; (j < call_status_ind->ip_hist_info_len &&
            j < QMI_VOICE_IP_HIST_INFO_ARRAY_MAX_V02); j++)
      {
        if ( call_status_ind->ip_hist_info[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_HISTORY_INFO_VALID;
          entry->history_info = call_status_ind->ip_hist_info[j];
          break;
        }
      }
    }
    if (call_status_ind->remote_party_conf_capability_valid)
    {
      for (j = 0; (j < call_status_ind->remote_party_conf_capability_len &&
            j < QMI_VOICE_REMOTE_PARTY_CONF_CAP_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->remote_party_conf_capability[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_R_PARTY_CONF_CAP_VALID;
          entry->remote_party_conf_capability = call_status_ind->remote_party_conf_capability[j];
          break;
        }
      }
    }
    if (call_status_ind->media_id_valid)
    {
      for (j = 0; (j < call_status_ind->media_id_len &&
            j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->media_id[j].call_id == qmi_call_id)
        {
          entry->media_id = call_status_ind->media_id[j].media_id;
          break;
        }
      }
    }

    entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VERSTAT_INFO_VALID;
    if (call_status_ind->mt_call_verstat_valid == 1)
    {
      unsigned n_calls = call_status_ind->mt_call_verstat_len;
      for (j = 0; (j < n_calls && j < QMI_VOICE_CALL_INFO_MAX_V02); j++)
      {
        if (call_status_ind->mt_call_verstat[j].call_id == qmi_call_id)
        {
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_VERSTAT_INFO_VALID;
          entry->verstat_info = call_status_ind->mt_call_verstat[j].
                  verstat_info;
          break;
        }
      }
    }

    //Terminating number for MT calls only
    entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TERMINATING_NUM_VALID;
    entry->is_secondary = false;
    if(call_status_ind->terminating_num_valid == 1) {
      unsigned n_calls = call_status_ind->terminating_num_len;
      for (j = 0; (j < n_calls && j < QMI_VOICE_TERMINATING_ARRAY_MAX_V02); j++)
      {
        if (call_status_ind->terminating_num[j].call_id == qmi_call_id)
        {
          int length = strlen(call_status_ind->terminating_num[j].num);
          entry->terminating_num = (char*)qcril_malloc(length + 1);
          if (NULL == entry->terminating_num) {
            QCRIL_LOG_ERROR("malloc failed for entry->terminating_num[call id = %d]", qmi_call_id);
            break;
          }
          strlcpy(entry->terminating_num, call_status_ind->terminating_num[j].num, length + 1);
          //Assumption : there can be only one secondary call supported
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TERMINATING_NUM_VALID;
          if (call_status_ind->is_secondary_valid) {
              entry->is_secondary = call_status_ind->is_secondary;
          }
        }
      }
    }

    //tir mode for MT calls only
    entry->elaboration &= ~QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TIR_MODE_VALID;
    if(call_status_ind->tir_mode_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->tir_mode_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->tir_mode[idxCall].call_id == qmi_call_id)
        {
          entry->tir_mode = call_status_ind->tir_mode[idxCall].tir_mode;
          entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_TIR_MODE_VALID;
          break;
        }
      }
    }

    //silent ui for CRS calls
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_SILENT_UI_VALID, FALSE);
    if(call_status_ind->silent_ui_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->silent_ui_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->silent_ui[idxCall].call_id == qmi_call_id)
        {
          entry->silent_ui = call_status_ind->silent_ui[idxCall].silent_ui;
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_SILENT_UI_VALID, TRUE);
          break;
        }
      }
    }

    //original call type for CRS calls
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_ORIGINAL_CALL_TYPE_VALID, FALSE);
    if(call_status_ind->original_call_type_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->original_call_type_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->original_call_type[idxCall].call_id == qmi_call_id)
        {
          entry->original_call_type = call_status_ind->original_call_type[idxCall].original_call_type;
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_ORIGINAL_CALL_TYPE_VALID, TRUE);
          break;

        }
      }
    }

    //crs type for CRS calls
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_CRS_TYPE_VALID, FALSE);
    if(call_status_ind->crs_type_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->crs_type_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->crs_type[idxCall].call_id == qmi_call_id)
        {
          entry->crs_type = call_status_ind->crs_type[idxCall].crs_type;
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_CRS_TYPE_VALID, TRUE);
          break;
        }
      }
    }

    // Call progress notification info
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_PROG_INFO_NOTIF_VALID, FALSE);
    if (call_status_ind->prog_info_notif_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->prog_info_notif_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->prog_info_notif[idxCall].call_id == qmi_call_id)
        {
          entry->prog_info_notif = call_status_ind->prog_info_notif[idxCall];
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_PROG_INFO_NOTIF_VALID, TRUE);
          break;
        }
      }
    }

    // Diversion info
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_DIVERSION_INFO_VALID, FALSE);
    if (call_status_ind->ip_diversion_info_valid)
    {
      for (j = 0; (j < call_status_ind->ip_diversion_info_len &&
            j < QMI_VOICE_IP_DIVERSION_INFO_ARRAY_MAX_V02); j++)
      {
        if ( call_status_ind->ip_diversion_info[j].call_id == qmi_call_id)
        {
          entry->diversion_info = call_status_ind->ip_diversion_info[j];
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_DIVERSION_INFO_VALID, TRUE);
          break;
        }
      }
    }

    // Msim additional call information
    qcril_qmi_voice_voip_call_info_set_single_elaboration(
        entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_MSIM_ADDITIONAL_CALL_INFO_VALID, FALSE);
    if (call_status_ind->msim_additional_call_info_valid) {
      const size_t max_calls = std::min<size_t>(call_status_ind->msim_additional_call_info_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (j = 0; j < max_calls; j++) {
        if (call_status_ind->msim_additional_call_info[j].call_id == qmi_call_id) {
          entry->msim_additional_call_info.setAdditionalCode(qcril_qmi_map_msim_additional_info_code(
              call_status_ind->msim_additional_call_info[j].call_info));
          qcril_qmi_voice_voip_call_info_set_single_elaboration(
              entry, QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_MSIM_ADDITIONAL_CALL_INFO_VALID, TRUE);
          break;
        }
      }
    }

    // Vos support
    qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
            QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_VOS_SUPPORT_VALID, FALSE);
    if(call_status_ind->vos_support_valid)
    {
      const size_t max_calls = std::min<size_t>(call_status_ind->vos_support_len,
            QMI_VOICE_CALL_INFO_MAX_V02);
      for (size_t idxCall = 0; idxCall < max_calls; idxCall++)
      {
        if (call_status_ind->vos_support[idxCall].call_id == qmi_call_id)
        {
          entry->vos_support = call_status_ind->vos_support[idxCall].vos_support;
          qcril_qmi_voice_voip_call_info_set_single_elaboration(entry,
                  QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_VOS_SUPPORT_VALID, TRUE);
          break;
        }
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_voip_update_call_info_entry_mainstream
//===========================================================================
// qcril_qmi_voice_voip_update_call_info_uus
//===========================================================================
void qcril_qmi_voice_voip_update_call_info_uus(qcril_qmi_voice_voip_call_info_entry_type* entry,
                                               uus_type_enum_v02 uus_type,
                                               uus_dcs_enum_v02 uus_dcs,
                                               uint32_t uus_data_len,
                                               uint8_t *uus_data
                                                 )
{
  QCRIL_LOG_INFO( "param %p", entry );
  if ( entry )
  {
    entry->voice_svc_uus.uus_type = uus_type;
    entry->voice_svc_uus.uus_dcs  = uus_dcs;
    entry->voice_svc_uus.uus_data_len  = uus_data_len;
    memcpy( entry->voice_svc_uus.uus_data, uus_data, MIN( QMI_VOICE_UUS_DATA_MAX_V02, uus_data_len ) );
    entry->voice_svc_uus.uus_data_len  = uus_data_len;

    entry->elaboration |= QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_UUS_VALID;
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_voip_update_call_info_uus

//===========================================================================
// qcril_qmi_voice_voip_lock_overview
//===========================================================================
void qcril_qmi_voice_voip_lock_overview()
{
  qmi_voice_voip_overview.overview_lock_mutex.lock();
} // qcril_qmi_voice_voip_lock_overview
//===========================================================================
// qcril_qmi_voice_voip_unlock_overview
//===========================================================================
void qcril_qmi_voice_voip_unlock_overview()
{
  qmi_voice_voip_overview.overview_lock_mutex.unlock();
} // qcril_qmi_voice_voip_unlock_overview
//===========================================================================
// qcril_qmi_voice_voip_mark_all_with
//===========================================================================
void qcril_qmi_voice_voip_mark_all_with(qcril_qmi_voice_voip_call_info_elaboration_type elaboration_set)
{
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;

  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL  )
  {
    iter->elaboration |= elaboration_set;
    iter = iter->next;
  }
} // qcril_qmi_voice_voip_mark_all_with
//===========================================================================
// qcril_qmi_voice_voip_unmark_all_with
//===========================================================================
void qcril_qmi_voice_voip_unmark_all_with(qcril_qmi_voice_voip_call_info_elaboration_type elaboration_set)
{
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_call_info_elaboration_type application_set;

  application_set = ~elaboration_set;
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL  )
  {
    iter->elaboration &= application_set;
    iter = iter->next;
  }
} // qcril_qmi_voice_voip_unmark_all_with

//===========================================================================
// qcril_qmi_voice_voip_unmark_with_specified_call_state
//===========================================================================
void qcril_qmi_voice_voip_unmark_with_specified_call_state
(
  qcril_qmi_voice_voip_call_info_elaboration_type elaboration_set,
  call_state_enum_v02 state
)
{
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_call_info_elaboration_type application_set;

  application_set = ~elaboration_set;
  iter = qmi_voice_voip_overview.call_info_root;
  while ( iter != NULL  )
  {
    if ( state == iter->voice_scv_info.call_state )
    {
      iter->elaboration &= application_set;
    }
    iter = iter->next;
  }
} // qcril_qmi_voice_voip_unmark_with_specified_call_state

//===========================================================================
// qcril_qmi_voice_voip_mark_with_specified_call_state
//===========================================================================
void qcril_qmi_voice_voip_mark_with_specified_call_state
(
  qcril_qmi_voice_voip_call_info_elaboration_type elaboration_set,
  call_state_enum_v02 state
)
{
  qcril_qmi_voice_voip_call_info_entry_type* iter = qmi_voice_voip_overview.call_info_root;

  while ( iter != NULL  )
  {
    if ( state == iter->voice_scv_info.call_state )
    {
      iter->elaboration |= elaboration_set;
    }
    iter = iter->next;
  }
} // qcril_qmi_voice_voip_mark_with_specified_call_state

//===========================================================================
// qcril_qmi_voice_voip_call_info_entries_is_empty
//===========================================================================
boolean qcril_qmi_voice_voip_call_info_entries_is_empty(void)
{
  return !qmi_voice_voip_overview.call_info_root;
} // qcril_qmi_voice_voip_call_info_entries_is_empty

//===========================================================================
// qcril_qmi_voice_voip_call_info_entries_enum_first
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_call_info_entries_enum_first(void)
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;

  res = qmi_voice_voip_overview.call_info_root;
  qmi_voice_voip_overview.call_info_enumeration_current = res;

  return res;
} //
//===========================================================================
// qcril_qmi_voice_voip_call_info_entries_enum_next
//===========================================================================
qcril_qmi_voice_voip_call_info_entry_type* qcril_qmi_voice_voip_call_info_entries_enum_next(void)
{
  qcril_qmi_voice_voip_call_info_entry_type* res = NULL;

  res = NULL;

  if ( NULL != qmi_voice_voip_overview.call_info_enumeration_current )
  {
    res = qmi_voice_voip_overview.call_info_enumeration_current->next;
    qmi_voice_voip_overview.call_info_enumeration_current = res;
  }

  return res;
}

//===========================================================================
// qcril_qmi_voice_voip_generate_summary
//===========================================================================
void qcril_qmi_voice_voip_generate_summary( qcril_qmi_voice_voip_current_call_summary_type * summary )
{
  qcril_qmi_voice_voip_call_info_entry_type * call_info_entry = NULL;

  int nof_3gpp2_calls;
  int nof_3gpp_calls;
  int nof_active_calls;
  int nof_voip_calls;
  int nof_calls;

  if ( summary )
  {
    memset( summary, 0, sizeof( *summary ) );

    nof_3gpp2_calls  = 0;
    nof_3gpp_calls   = 0;
    nof_active_calls = 0;
    nof_voip_calls   = 0;
    nof_calls        = 0;

    call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
    while ( NULL != call_info_entry )
    {
      if ((VOICE_INVALID_CALL_ID != call_info_entry->android_call_id) &&
          ((VOICE_INVALID_CALL_ID != call_info_entry->qmi_call_id) ||
            (call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_NO_QMI_ID_RECEIVED)))
      {
        // skip shadow calls from CM space
        nof_calls++;
        if ( call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN )
        {
          nof_voip_calls++;
        }
        else if ( CALL_MODE_CDMA_V02 == call_info_entry->voice_scv_info.mode )
        {
          nof_3gpp2_calls++;
        }
        else
        {
          nof_3gpp_calls++;
        }

        if ( CALL_STATE_CONVERSATION_V02 == call_info_entry->voice_scv_info.call_state )
        {
          nof_active_calls++;
        }
        summary->active_or_single_call = call_info_entry;
      }
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
    }

    summary->nof_calls_overall      = nof_calls;
    summary->nof_voip_calls         = nof_voip_calls;
    summary->nof_3gpp2_calls        = nof_3gpp2_calls;
    summary->nof_3gpp_calls         = nof_3gpp_calls;
    summary->nof_active_calls       = nof_active_calls;
    summary->nof_voice_calls        = nof_3gpp2_calls + nof_3gpp_calls;
  }
} // qcril_qmi_voice_voip_generate_summary

//===========================================================================
// qcril_qmi_voice_voip_call_info_dump
//===========================================================================
void qcril_qmi_voice_voip_call_info_dump(const qcril_qmi_voice_voip_call_info_entry_type *const call_info_entry)
{
  QCRIL_LOG_INFO( "param %p", call_info_entry );
  if ( call_info_entry )
  {
    QCRIL_LOG_DEBUG( ".. call android id %d, call qmi id %d, elaboration %x, %x hex",
        (int)call_info_entry->android_call_id,
        (int)call_info_entry->qmi_call_id,
        (uint32_t) (call_info_entry->elaboration >> 32), (uint32_t)call_info_entry->elaboration);
    QCRIL_LOG_DEBUG( ".. call state %d, call type %d, call mode %d",
        (int)call_info_entry->voice_scv_info.call_state,
        (int)call_info_entry->voice_scv_info.call_type,
        (int)call_info_entry->voice_scv_info.mode );
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_voip_call_info_dump

//===========================================================================
// qmi_ril_voice_drop_homeless_incall_reqs
//===========================================================================
void qmi_ril_voice_drop_homeless_incall_reqs( void )
{
  QCRIL_LOG_FUNC_ENTRY();

  auto restriction = PolicyManager::getInstance().getMessageRestriction(
      QcRilRequestSendDtmfMessage::MESSAGE_NAME);
  restriction->abandonPendingMessages();

  restriction = PolicyManager::getInstance().getMessageRestriction(
      QcRilRequestImsSendDtmfMessage::MESSAGE_NAME);
  restriction->abandonPendingMessages();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_drop_homeless_incall_reqs

//===========================================================================
// qmi_ril_voice_cleanup_reqs_after_call_completion
//===========================================================================
void qmi_ril_voice_cleanup_reqs_after_call_completion(void)
{
  QCRIL_LOG_FUNC_ENTRY();

  auto restriction = PolicyManager::getInstance().getMessageRestriction(
      QcRilRequestSendDtmfMessage::MESSAGE_NAME);
  restriction->abandonPendingMessages();

  restriction = PolicyManager::getInstance().getMessageRestriction(
      QcRilRequestImsSendDtmfMessage::MESSAGE_NAME);
  restriction->abandonPendingMessages();

  restriction = PolicyManager::getInstance().getMessageRestriction(
      QcRilRequestConferenceMessage::MESSAGE_NAME);
  restriction->abandonPendingMessages();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_cleanup_reqs_after_call_completion

/*===========================================================================

  FUNCTION: qcril_qmi_voice_get_atel_call_type_info

===========================================================================*/
/*!
    @brief
    fetch RIL call type information from call type, video attribute (if available)
    audio attribute (if available) and call elaboration (if available).

    @return
    TRUE, if call type can be derived.
    FALSE, otherwise.
*/
/*=========================================================================*/
boolean qcril_qmi_voice_get_atel_call_type_info
(
 call_type_enum_v02 call_type,
 boolean video_attrib_valid,
 voice_call_attribute_type_mask_v02 video_attrib,
 boolean audio_attrib_valid,
 voice_call_attribute_type_mask_v02 audio_attrib,
 boolean attrib_status_valid,
 voice_call_attrib_status_enum_v02 attrib_status,
 boolean call_info_elab_valid,
 qcril_qmi_voice_voip_call_info_elaboration_type call_info_elab,
 qcril::interfaces::CallType cached_call_type,
 qcril::interfaces::CallType &callType,
 qcril::interfaces::CallDomain &callDomain,
 uint32_t &callSubState
)
{
  QCRIL_LOG_FUNC_ENTRY();
  boolean ret = TRUE;

    QCRIL_LOG_INFO( "QMI call_type: %d, video_attrib_valid: %d, video_attrib: %d, "
        "audio_attrib_valid: %d, audio_attrib: %d, "
        "call_attrib_valid: %d, call_attrib:%d, "
        "call_info_elab_valid: %d, elaboration %x, %x hex",
        call_type, video_attrib_valid, (int)video_attrib,
        audio_attrib_valid, audio_attrib,
        attrib_status_valid, attrib_status,
        call_info_elab_valid,
        (uint32_t)(call_info_elab >> 32), (uint32_t)call_info_elab );

    callSubState = (uint32_t)qcril::interfaces::CallSubState::UNDEFINED;

    if ((CALL_TYPE_VT_V02 == call_type) || (CALL_TYPE_EMERGENCY_VT_V02 == call_type))
    {
      if (FALSE == video_attrib_valid)
      {
        callType = qcril::interfaces::CallType::VT;
        callDomain = qcril::interfaces::CallDomain::PS;
      }
      else
      {
        /* based on call attributes determine video call type */
        if (VOICE_CALL_ATTRIB_TX_V02 == video_attrib)
        {
          callType = qcril::interfaces::CallType::VT_TX;
          callDomain = qcril::interfaces::CallDomain::PS;
        }
        else if (VOICE_CALL_ATTRIB_RX_V02 == video_attrib)
        {
          callType = qcril::interfaces::CallType::VT_RX;
          callDomain = qcril::interfaces::CallDomain::PS;
        }
        else if ((VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02) == video_attrib)
        {
          callType = qcril::interfaces::CallType::VT;
          callDomain = qcril::interfaces::CallDomain::PS;
        }
        else if (TRUE == audio_attrib_valid && 0 == audio_attrib && 0 == video_attrib)
        {
          callSubState = ((uint32_t)qcril::interfaces::CallSubState::AUDIO_CONNECTED_SUSPENDED |
                          ((uint32_t)qcril::interfaces::CallSubState::VIDEO_CONNECTED_SUSPENDED));
          callType = (cached_call_type != qcril::interfaces::CallType::UNKNOWN)
                         ? cached_call_type
                         : qcril::interfaces::CallType::VT;
          callDomain = qcril::interfaces::CallDomain::PS;
        }
        else if (0 == video_attrib && attrib_status_valid == TRUE)
        {
          callType = qcril::interfaces::CallType::VT_NODIR;
          callDomain = qcril::interfaces::CallDomain::PS;
          switch (attrib_status)
          {
            case VOICE_CALL_ATTRIB_STATUS_RETRY_NEEDED_V02:
              callSubState = (uint32_t)qcril::interfaces::CallSubState::AVP_RETRY;
              break;
            case VOICE_CALL_ATTRIB_STATUS_MEDIA_PAUSED_V02:
              callSubState = (uint32_t)qcril::interfaces::CallSubState::MEDIA_PAUSED;
              break;
            case VOICE_CALL_ATTRIB_STATUS_OK_V02:
              callSubState = (uint32_t)qcril::interfaces::CallSubState::UNDEFINED;
              break;
            default:
              break;
          }
        }
        else
        {
          ret = FALSE;
        }
      }
      if (call_type == CALL_TYPE_EMERGENCY_VT_V02 &&
          call_info_elab & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN)
      {
        callDomain = qcril::interfaces::CallDomain::AUTOMATIC;
      }
    }
    else // not a VT call
    {
      callType = qcril::interfaces::CallType::VOICE;
      if (TRUE == audio_attrib_valid && 0 == audio_attrib)
      {
        callSubState = (uint32_t)qcril::interfaces::CallSubState::AUDIO_CONNECTED_SUSPENDED;
      }

      boolean call_domain_set = FALSE;
      if (call_info_elab_valid)
      {
        call_domain_set = TRUE;
        if (call_info_elab & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CS_DOMAIN)
        {
          callDomain = qcril::interfaces::CallDomain::CS;
        }
        else if (call_info_elab & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_PS_DOMAIN)
        {
          callDomain = qcril::interfaces::CallDomain::PS;
        }
        else if (call_info_elab & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_AUTO_DOMAIN)
        {
          callDomain = qcril::interfaces::CallDomain::AUTOMATIC;
        }
        else
        {
          QCRIL_LOG_DEBUG("did not set call domain in elaboration.");
          call_domain_set = FALSE;
        }
      }
      if (!call_domain_set)
      {
        switch (call_type)
        {
          case CALL_TYPE_EMERGENCY_IP_V02:
          case CALL_TYPE_VOICE_IP_V02:
            callDomain = qcril::interfaces::CallDomain::PS;
            break;

          default:
            /* fall back to default voice case */
            callDomain = qcril::interfaces::CallDomain::CS;
            break;
        }
      }
    } // end of "not a VT call"

  QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
  return ret;
} // qcril_qmi_voice_get_atel_call_type_info

/*===========================================================================

  FUNCTION: qcril_qmi_voice_match_modem_call_type

===========================================================================*/
/*!
    @brief
    compare the modem call types provided.

    @return
    TRUE, if call type has matched.
    FALSE, otherwise.
*/
/*=========================================================================*/
bool qcril_qmi_voice_match_modem_call_type
(
   call_type_enum_v02                    call_type1,
   bool                                  audio_attrib_valid1,
   voice_call_attribute_type_mask_v02    audio_attrib1,
   bool                                  video_attrib_valid1,
   voice_call_attribute_type_mask_v02    video_attrib1,
   bool                                  rtt_mode_valid1,
   rtt_mode_type_v02                     rtt_mode1,
   call_type_enum_v02                    call_type2,
   bool                                  audio_attrib_valid2,
   voice_call_attribute_type_mask_v02    audio_attrib2,
   bool                                  video_attrib_valid2,
   voice_call_attribute_type_mask_v02    video_attrib2,
   bool                                  rtt_mode_valid2,
   rtt_mode_type_v02                     rtt_mode2
)
{
   bool result = true;

   QCRIL_LOG_DEBUG("call_type1 = %d, audio_attrrib = %d/%d, video_attrib = %d/%d, rtt_mode = %d/%d",
                   call_type1, audio_attrib_valid1, (int)audio_attrib1, video_attrib_valid1,
                   (int)video_attrib1, rtt_mode_valid1, rtt_mode1);

   QCRIL_LOG_DEBUG("call_type2 = %d, audio_attrrib = %d/%d, video_attrib = %d/%d, rtt_mode = %d/%d",
                   call_type2, audio_attrib_valid2, (int)audio_attrib2, video_attrib_valid2,
                   (int)video_attrib2, rtt_mode_valid2, rtt_mode2);

   do {
     if (call_type1 != call_type2) {
       result = false;
       break;
     }
     if ((rtt_mode_valid1 != rtt_mode_valid2) ||
         (rtt_mode_valid1 && (rtt_mode1 != rtt_mode2))) {
       result = false;
       break;
     }
     // Check video/audio attribute only for VT
     if (call_type1 == CALL_TYPE_VT_V02 || call_type1 == CALL_TYPE_EMERGENCY_VT_V02) {
       if ((audio_attrib_valid1 != audio_attrib_valid2) ||
           (audio_attrib_valid1 && (audio_attrib1 != audio_attrib2))) {
         result = false;
         break;
       }
       if ((video_attrib_valid1 != video_attrib_valid2) ||
           (video_attrib_valid1 && (video_attrib1 != video_attrib2))) {
         result = false;
         break;
       }
     }
   } while (0);

   QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
   return result;
}

//===========================================================================
// qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl
//===========================================================================
void qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl( void )
{
    qcril_qmi_voice_voip_call_info_entry_type* call_info = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    call_info = qcril_qmi_voice_voip_call_info_entries_enum_first();
    while ( NULL != call_info )
    {
      if ( VOICE_INVALID_CALL_ID != call_info->android_call_id &&
           CALL_STATE_END_V02 == call_info->voice_scv_info.call_state &&
           call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_CALL_ENDED_REPORTED &&
           call_info->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_LAST_CALL_FAILURE_REPORTED
         )
      {
        qcril_qmi_voice_voip_destroy_call_info_entry( call_info);
      }

      call_info = qcril_qmi_voice_voip_call_info_entries_enum_next();
    }

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_evaluate_voice_call_obj_cleanup_vcl

//===========================================================================
// qcril_qmi_voice_handle_audio_pd_state_changed
//===========================================================================
void qcril_qmi_voice_handle_audio_pd_state_changed
(
   qcril_qmi_pd_entry *entry,
   qcril_qmi_pd_state state,
   void *userdata)
{
   QCRIL_LOG_FUNC_ENTRY();
   QCRIL_NOTUSED(entry);
   QCRIL_NOTUSED(userdata);
   qcril_qmi_voice_voip_lock_overview();
   qcril_qmi_voice_info.pd_info.state = state;
   qcril_qmi_voice_update_audio_state_vcl();
   qcril_qmi_voice_voip_unlock_overview();
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_handle_audio_pd_state_changed

#if !defined (QMI_RIL_UTF) && defined RIL_WEARABLES_TARGET_AON
//===========================================================================
// qcril_qmi_voice_handle_pil_state_changed
//===========================================================================
void qcril_qmi_voice_handle_pil_state_changed(const qcril_qmi_pil_state* cur_state)
{
   QCRIL_LOG_FUNC_ENTRY();
   qcril_qmi_voice_voip_lock_overview();
   qcril_qmi_voice_info.pil_state.state = cur_state->state;
   qcril_qmi_voice_update_audio_state_vcl();
   qcril_qmi_voice_voip_unlock_overview();
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_handle_pil_state_changed
#endif

boolean qmi_ril_voice_is_audio_inactive_vcl()
{
  bool feature_fake_audio_up = getVoiceModuleFeature(VoiceFeatureType::FAKE_AUDIO_UP);
  boolean ret = !feature_fake_audio_up &&
                (qcril_qmi_voice_info.audio_info.state != QCRIL_QMI_AUDIO_STATE_UP);
  QCRIL_LOG_DEBUG("[%d, %d] ret = %d", feature_fake_audio_up,
                  qcril_qmi_voice_info.audio_info.state, ret);
  return ret;
}

boolean qmi_ril_voice_is_calls_supressed_by_audio_pd_vcl(void)
{
  boolean ret;
  QCRIL_LOG_FUNC_ENTRY();
  ret = qcril_qmi_voice_info.pd_info.state != QCRIL_QMI_PD_UP &&
        qcril_qmi_voice_info.pd_info.state != QCRIL_QMI_PD_UNUSED;
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
  return ret;
}

void qcril_qmi_voice_update_audio_state_vcl(void)
{
   bool audio_active;
   QCRIL_LOG_FUNC_ENTRY();
   audio_active = TRUE;
   audio_active &= !qmi_ril_voice_is_calls_supressed_by_pil_vcl();
   audio_active &= !qmi_ril_voice_is_calls_supressed_by_audio_pd_vcl();

   qcril_qmi_voice_info.audio_info.state = audio_active ?
           QCRIL_QMI_AUDIO_STATE_UP :
           QCRIL_QMI_AUDIO_STATE_DOWN;

   if (qmi_ril_voice_is_audio_inactive_vcl())
   {
      qcril_qmi_voice_hangup_all_non_emergency_calls_vcl();
   }

   QCRIL_LOG_FUNC_RETURN();
}

boolean qcril_qmi_voice_is_emergency_call(const qcril_qmi_voice_voip_call_info_entry_type* call)
{
    return call && qcril_qmi_voice_is_qmi_call_emergency(&call->voice_scv_info);
}

//===========================================================================
// qcril_qmi_voice_hangup_all_non_emergency_calls_vcl
//===========================================================================
void qcril_qmi_voice_hangup_all_non_emergency_calls_vcl()
{
    qcril_qmi_voice_hangup_all_calls_vcl(qcril_qmi_voice_is_emergency_call, TRUE);
} // qcril_qmi_voice_hangup_all_non_emergency_calls_vcl

//===========================================================================
// qcril_qmi_voice_hangup_all_calls_vcl
//===========================================================================
void qcril_qmi_voice_hangup_all_calls_vcl(boolean (*exception_filter)(const qcril_qmi_voice_voip_call_info_entry_type*), boolean qmi_async)
{
   qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;
   voice_end_call_req_msg_v02   call_end_req_msg;

   QCRIL_LOG_FUNC_ENTRY();

   call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_first();
   while (NULL != call_info_entry)
   {
      if ( CALL_STATE_DISCONNECTING_V02 != call_info_entry->voice_scv_info.call_state &&
           CALL_STATE_END_V02 != call_info_entry->voice_scv_info.call_state )
      {
         if (exception_filter && exception_filter(call_info_entry))
         {
            call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
            continue;
         }

         memset(&call_end_req_msg, 0, sizeof(call_end_req_msg));
         call_end_req_msg.call_id = call_info_entry->qmi_call_id;
         QCRIL_LOG_INFO("end call with qmi id %d", call_end_req_msg.call_id);
         if (qmi_async)
         {
           qmi_client_error_type qmi_client_error = qmi_client_voice_send_async(
                                       QMI_VOICE_END_CALL_REQ_V02,
                                       &call_end_req_msg,
                                       sizeof(voice_end_call_req_msg_v02),
                                       sizeof(voice_end_call_resp_msg_v02),
                                       qcril_qmi_voice_command_cb,
                                       nullptr);
           QCRIL_LOG_INFO("end call QMI send returns: %d", qmi_client_error);
         }
         else
         {
           voice_end_call_resp_msg_v02 call_end_resp_msg;
           qmi_client_error_type qmi_client_error = get_voice_modem_endpoint()->sendRawSync(
                                       QMI_VOICE_END_CALL_REQ_V02,
                                       &call_end_req_msg,
                                       sizeof(voice_end_call_req_msg_v02),
                                       &call_end_resp_msg,
                                       sizeof(voice_end_call_resp_msg_v02));
           QCRIL_LOG_INFO("end call QMI send returns: %d", qmi_client_error);
         }
      }
      call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_hangup_all_calls_vcl

//===========================================================================
// qmi_ril_voice_is_calls_supressed_by_pil_vcl
//===========================================================================
boolean qmi_ril_voice_is_calls_supressed_by_pil_vcl()
{
   QCRIL_LOG_FUNC_ENTRY();
   boolean ret = (QCRIL_QMI_PIL_STATE_OFFLINE == qcril_qmi_voice_info.pil_state.state);
   QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
   return ret;
} // qmi_ril_voice_is_calls_supressed_by_pil_vcl

//===========================================================================
// qcril_qmi_voice_ims_send_unsol_radio_state_change
//===========================================================================
void qcril_qmi_voice_ims_send_unsol_radio_state_change()
{
  RIL_RadioState        radio_state;
  qcril::interfaces::RadioState radioState = qcril::interfaces::RadioState::STATE_UNKNOWN;
  QCRIL_LOG_FUNC_ENTRY();

  radio_state = qcril_qmi_voice_external_get_radio_state();
  QCRIL_LOG_DEBUG("radio_state: %s(%d)", qcril_log_ril_radio_state_to_str(radio_state), radio_state);

  switch (radio_state)
  {
    case RADIO_STATE_UNAVAILABLE:
      radioState = qcril::interfaces::RadioState::STATE_UNAVAILABLE;
      break;

    case RADIO_STATE_OFF:
      radioState = qcril::interfaces::RadioState::STATE_OFF;
      break;

    default:
      radioState = qcril::interfaces::RadioState::STATE_ON;
      break;
  }

  QCRIL_LOG_DEBUG("Modem_state is : %d", radioState);
  auto msg = std::make_shared<QcRilUnsolImsRadioStateIndication>(radioState);
  if (msg != nullptr)
  {
    Dispatcher::getInstance().dispatchSync(msg);
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_voice_ims_cleanup_unknown_calls
//===========================================================================
void qcril_qmi_voice_ims_cleanup_unknown_calls() {
  // Clear the local cache
  qcril_qmi_voice_voip_cleanup_all_call_info_entries();

  // Send END_ALL_CALLS to modem to ensure all the on going calls and ENDed properly.
#ifndef QMI_RIL_UTF
  voice_manage_ip_calls_req_msg_v02 manage_ip_calls_req = { .sups_type =
                                                                VOIP_SUPS_TYPE_END_ALL_CALLS_V02 };
  voice_manage_ip_calls_resp_msg_v02 manage_ip_calls_resp = {};
  qmi_client_error_type transp_err = get_voice_modem_endpoint()->sendRawSync(
      QMI_VOICE_MANAGE_IP_CALLS_REQ_V02, &manage_ip_calls_req,
      sizeof(voice_manage_ip_calls_req_msg_v02), &manage_ip_calls_resp,
      sizeof(voice_manage_ip_calls_resp_msg_v02));
  RIL_Errno ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
      transp_err, &(manage_ip_calls_resp.resp));
  QCRIL_LOG_INFO("QMI req got : %d", ril_err);
#endif
}  // qcril_qmi_voice_ims_cleanup_unknown_calls

//===========================================================================
// qcril_qmi_voice_ims_client_connected
//===========================================================================
void qcril_qmi_voice_ims_client_connected()
{
  QCRIL_LOG_FUNC_ENTRY();
  ims_client_connected = TRUE;
  if (qcril_qmi_voice_has_specific_call(qcril_qmi_voice_call_to_ims, NULL)) {
    qcril_qmi_voice_ims_cleanup_unknown_calls();
  }
  qcril_qmi_voice_ims_send_unsol_radio_state_change();
  qcril_qmi_voice_ims_send_unsol_vice_dialog_refresh_info_helper();
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_ims_client_connected

//===========================================================================
// qcril_qmi_send_ss_failure_cause_oem_hook_unsol_resp
//===========================================================================
void qcril_qmi_send_ss_failure_cause_oem_hook_unsol_resp
(
  qmi_sups_errors_enum_v02 sups_failure_cause,
  uint8_t call_id
)
{
  auto msg = std::make_shared<QcRilUnsolSuppSvcErrorCodeMessage>(call_id, sups_failure_cause);
  if (msg)
  {
    Dispatcher::getInstance().dispatchSync(msg);
  }
}

//===========================================================================
// qmi_ril_voice_ims_command_oversight_timeout_handler
//===========================================================================
void qmi_ril_voice_ims_command_oversight_timeout_handler(uint16_t token)
{
  qmi_ril_voice_ims_command_exec_oversight_type* command_oversight;
  qmi_ril_voice_ims_command_exec_oversight_type* iter;
  qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type
                                                  event_params;

  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_voice_voip_lock_overview();

  // find expired
  command_oversight = NULL;
  iter = qmi_ril_voice_ims_command_oversight_first();
  while ( NULL != iter && NULL == command_oversight )
  {
    if (token == iter->token)
    {
      command_oversight = iter;
    }
    else
    {
      iter = qmi_ril_voice_ims_command_oversight_next();
    }
  } // while ( NULL != iter && NULL == command_oversight )

  if ( NULL != command_oversight )
  {
    command_oversight->timeout_control_timer_id = TimeKeeper::no_timer;

    memset( &event_params, 0, sizeof( event_params ) );
    event_params.locator.command_oversight = command_oversight;

    (void)qmi_ril_voice_ims_command_oversight_handle_event( QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_TIME_OUT, QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ, &event_params );
  }

  qcril_qmi_voice_voip_unlock_overview();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_voice_ims_command_oversight_timeout_handler

//===========================================================================
// qmi_ril_voice_ims_create_command_oversight
//===========================================================================
qmi_ril_voice_ims_command_exec_oversight_type *
qmi_ril_voice_ims_create_command_oversight
(
 uint16_t token,
 qcril_evt_e_type request_id,
 int launch_timout_control,
 CommonVoiceResponseCallback commandOversightCompletionHandler
)
{
  qmi_ril_voice_ims_command_exec_oversight_type* command_oversight =
        new qmi_ril_voice_ims_command_exec_oversight_type();

  if ( NULL != command_oversight )
  {
    command_oversight->token              = token;
    command_oversight->android_request_id = request_id;
    command_oversight->completion_action = commandOversightCompletionHandler;
    command_oversight->timeout_control_timer_id = TimeKeeper::no_timer;

    if ( launch_timout_control )
    {
      command_oversight->timeout_control_timer_id =
            TimeKeeper::getInstance().set_timer(
                [token] (void *userdata) {
                    (void)userdata;
                    qmi_ril_voice_ims_command_oversight_timeout_handler(token);
                },
                nullptr,
                launch_timout_control * 1000);
    }
    QCRIL_LOG_DEBUG( "cmd oversight created obj %p android req %d, token 0x%x",
                     command_oversight, request_id, token);

    command_oversight->next                           = qmi_voice_voip_overview.command_exec_oversight_root;
    qmi_voice_voip_overview.command_exec_oversight_root = command_oversight;
  }
  else
  {
    QCRIL_LOG_ERROR("cmd oversight FAILED to create obj: no resources");
  }

  qmi_ril_voice_ims_command_oversight_dump( command_oversight );

  return command_oversight;
} // qmi_ril_voice_ims_create_command_oversight

//===========================================================================
// qmi_ril_voice_ims_destroy_command_oversight
//===========================================================================
void qmi_ril_voice_ims_destroy_command_oversight( qmi_ril_voice_ims_command_exec_oversight_type* command_oversight )
{
  qmi_ril_voice_ims_command_exec_oversight_type** cur_p;
  qmi_ril_voice_ims_command_exec_oversight_type*  cur;
  if ( NULL != command_oversight )
  {
    if (TimeKeeper::no_timer != command_oversight->timeout_control_timer_id )
    {
      TimeKeeper::getInstance().clear_timer(command_oversight->timeout_control_timer_id);
      command_oversight->timeout_control_timer_id = TimeKeeper::no_timer;
    }

    cur_p = &qmi_voice_voip_overview.command_exec_oversight_root;
    while ( NULL != ( cur = *cur_p ) && ( command_oversight != cur ) )
    {
      cur_p = &cur->next;
    }
    if ( NULL != cur )
    {
      *cur_p = cur->next;
    }

    if ( command_oversight == qmi_voice_voip_overview.command_exec_oversight_current )
    {
      qmi_voice_voip_overview.command_exec_oversight_current = command_oversight->next;
    }
    QCRIL_LOG_DEBUG("Destroyed cmd oversight obj %p", command_oversight);

    delete command_oversight;
  }
} // qmi_ril_voice_ims_destroy_command_oversight

//===========================================================================
// qmi_ril_voice_ims_command_oversight_add_call_link
//===========================================================================
void qmi_ril_voice_ims_command_oversight_add_call_link(  qmi_ril_voice_ims_command_exec_oversight_type*     command_oversight,
                                                         qmi_ril_voice_ims_command_exec_oversight_call_obj_linkage_e_type link_type,
                                                         qcril_qmi_voice_voip_call_info_elaboration_type    elaboration_pattern,
                                                         int                                                call_id,
                                                         call_state_enum_v02                                target_call_state
                                                       )
{
  qmi_ril_voice_ims_command_exec_oversight_link_type * link;

  if ( NULL != command_oversight && ( command_oversight->nof_impacted < QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_MAX_NOF_CALLS ) )
  {
    link = &command_oversight->impacted [ command_oversight->nof_impacted ];
    command_oversight->nof_impacted++;

    link->linkage_type = link_type;
    switch ( link_type )
    {
      case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_NONE:
        // no action
        break;

      case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID:
        link->linkage.qmi_call_id = call_id;
        break;

      case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID:
        link->linkage.android_call_id = call_id;
        break;

      case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN:
        link->linkage.elaboration_pattern                               = elaboration_pattern;
        break;

      default:
        break;
    }
    link->target_call_state = target_call_state;
  }

  qmi_ril_voice_ims_command_oversight_dump( command_oversight );
} // qmi_ril_voice_ims_command_oversight_add_call_link


//===========================================================================
// qmi_ril_voice_ims_command_oversight_first
//===========================================================================
qmi_ril_voice_ims_command_exec_oversight_type* qmi_ril_voice_ims_command_oversight_first()
{
  qmi_ril_voice_ims_command_exec_oversight_type*  command_oversight;

  command_oversight                                       = qmi_voice_voip_overview.command_exec_oversight_root;
  qmi_voice_voip_overview.command_exec_oversight_current  = qmi_voice_voip_overview.command_exec_oversight_root;

  return command_oversight;
}  // qmi_ril_voice_ims_command_oversight_first
//===========================================================================
// qmi_ril_voice_ims_command_oversight_next
//===========================================================================
qmi_ril_voice_ims_command_exec_oversight_type* qmi_ril_voice_ims_command_oversight_next()
{
  qmi_ril_voice_ims_command_exec_oversight_type*  command_oversight;

  command_oversight = qmi_voice_voip_overview.command_exec_oversight_current;
  if ( NULL != qmi_voice_voip_overview.command_exec_oversight_current )
  {
    qmi_voice_voip_overview.command_exec_oversight_current = qmi_voice_voip_overview.command_exec_oversight_current->next;
    command_oversight                                      = qmi_voice_voip_overview.command_exec_oversight_current;
  }
  else
  {
    command_oversight = NULL;
  }
  return command_oversight;
} // qmi_ril_voice_ims_command_oversight_next

//===========================================================================
// qmi_ril_voice_ims_find_command_oversight_by_token
//===========================================================================
qmi_ril_voice_ims_command_exec_oversight_type* qmi_ril_voice_ims_find_command_oversight_by_token( uint16_t token )
{
  qmi_ril_voice_ims_command_exec_oversight_type*  command_oversight;
  qmi_ril_voice_ims_command_exec_oversight_type*  iter;

  command_oversight = NULL;

  iter = qmi_ril_voice_ims_command_oversight_first();
  while ( ( NULL != iter ) && ( NULL == command_oversight ) )
  {
    if ( token == iter->token )
    {
      command_oversight = iter;
    }
    else
    {
      iter = qmi_ril_voice_ims_command_oversight_next();
    }
  }

  return command_oversight;
} // qmi_ril_voice_ims_find_command_oversight_by_token

//===========================================================================
// qmi_ril_voice_ims_command_oversight_handle_event
//===========================================================================
int qmi_ril_voice_ims_command_oversight_handle_event(   qmi_ril_voice_ims_command_exec_intermediates_event_e_type event,
                                                         qmi_ril_voice_ims_command_exec_oversight_call_obj_linkage_e_type link_type,
                                                         qmi_ril_voice_ims_command_exec_oversight_handle_event_params_type* params
                                                       )
{
  qmi_ril_voice_ims_command_exec_oversight_type*              iter;
  int                                                         idx;
  qmi_ril_voice_ims_command_exec_oversight_link_type *        link;
  int                                                         need_to_conclude_cmd;
  int                                                         is_match;
  qmi_ril_voice_ims_command_exec_intermediates_state_e_type   new_link_state;
  int                                                         need_set_new_link_state;
  int                                                         any_link_changed;
  qmi_ril_voice_ims_command_exec_intermediates_state_e_type   generalized_completion_state;
  RIL_Errno                                                   ril_req_res = RIL_E_INTERNAL_ERR;
//  qcril_reqlist_public_type                                   atel_request;
//  IxErrnoType                                                 atel_request_find_res;
  int                                                         any_action_performed;
  qcril_qmi_voice_voip_call_info_entry_type*                  call_entry;

  QCRIL_LOG_FUNC_ENTRY();

  any_action_performed = FALSE;

  iter = qmi_ril_voice_ims_command_oversight_first();
  while ( NULL != iter )
  {
    qmi_ril_voice_ims_command_oversight_dump( iter );

    link                 = iter->impacted;
    any_link_changed     = FALSE;
    for ( idx = 0; idx < iter->nof_impacted; idx++ )
    {
      is_match = FALSE;
      switch ( link_type )
      {
        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID:
          if ( QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID == link->linkage_type && link->linkage.qmi_call_id == params->locator.qmi_call_id )
          {
            is_match = TRUE;
          }
          break;

        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID:
          if ( QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID == link->linkage_type && link->linkage.android_call_id == params->locator.android_call_id )
          {
            is_match = TRUE;
          }
          break;

        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN:
          if ( QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN == link->linkage_type &&
               ( link->linkage.elaboration_pattern & params->locator.elaboration_pattern ) == link->linkage.elaboration_pattern
             )
          {
            call_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration( link->linkage.elaboration_pattern, TRUE );
            if ( NULL != call_entry )
            {
              is_match = TRUE;
            }
          }
          break;

        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_SPECIFIC_OVERSIGHT_OBJ:
          if ( params->locator.command_oversight == iter )
          {
            is_match = TRUE;
          }
          break;

        default:
          // no action
          break;
      } // switch ( link_type )

      need_set_new_link_state = FALSE;
      new_link_state          = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_NONE;
      if ( is_match )
      { // found record, absorb event
        switch ( event )
        {
          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_COMMENCE_AWAIT_RESP_IND:
            new_link_state          = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_AWAITING_RESP_AND_IND;
            need_set_new_link_state = TRUE;
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_SUCCESS:
            switch ( link->exec_state )
            {
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_AWAITING_RESP_AND_IND:
                new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_RESP_AWAITING_IND;
                need_set_new_link_state = TRUE;
                break;

              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AWAITING_RESP:
                new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION;
                need_set_new_link_state = TRUE;
                break;

              default:
                break;
            }
            if ( 0 == iter->successful_response_payload_len && params->successful_response_payload_len > 0 && need_set_new_link_state )
            {
              iter->successful_response_payload_len = params->successful_response_payload_len;
              memcpy( (void *)&iter->successful_response_payload, (void *)&params->successful_response_payload, params->successful_response_payload_len );
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_RESP_FAILURE:
            switch ( link->exec_state )
            {
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_AWAITING_RESP_AND_IND:
                new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION;
                need_set_new_link_state = TRUE;
                break;

              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AWAITING_RESP:
                new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION;
                need_set_new_link_state = TRUE;
                break;

              default:
                break;
            }
            if ( 0 == iter->successful_response_payload_len && params->successful_response_payload_len > 0 && need_set_new_link_state )
            {
              iter->successful_response_payload_len = params->successful_response_payload_len;
              memcpy( (void *)&iter->successful_response_payload, (void *)&params->successful_response_payload, params->successful_response_payload_len );
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_RECEIVED_IND:
            if ( link->target_call_state == params->new_call_state )
            {
              switch ( link->exec_state )
              {
                case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_AWAITING_RESP_AND_IND:
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AWAITING_RESP;
                  need_set_new_link_state = TRUE;
                  break;

                case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_RESP_AWAITING_IND:
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION;
                  need_set_new_link_state = TRUE;
                  break;

                default:
                  break;
              } // ( link->exec_state )
            } // if ( link->target_call_state == params->new_call_state )
            if ( 0 == iter->successful_response_payload_len && params->successful_response_payload_len > 0 && need_set_new_link_state )
            {
              iter->successful_response_payload_len = params->successful_response_payload_len;
              memcpy( (void *)&iter->successful_response_payload, (void *)&params->successful_response_payload, params->successful_response_payload_len );
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_CALL_ENDED:
            if ( CALL_STATE_END_V02 == link->target_call_state )
            {
              switch ( link->exec_state )
              {
                case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_AWAITING_RESP_AND_IND:
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AWAITING_RESP;
                  need_set_new_link_state = TRUE;
                  break;

                case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_RESP_AWAITING_IND:
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION;
                  need_set_new_link_state = TRUE;
                  break;

                default:
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION;
                  need_set_new_link_state = TRUE;
                  break;
              }
            }
            else
            {
              new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION;
              need_set_new_link_state = TRUE;
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_TIME_OUT:
            switch ( link->exec_state )
            {
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION:               // fallthrough
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION:    // fallthrough
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION:
                // no change
                break;

              default:
                if ( iter == params->locator.command_oversight )
                {
                  new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_TIMEOUT_READY_FOR_COMPLETION;
                  need_set_new_link_state = TRUE;
                }
                break;
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_ABANDON:
            new_link_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION;
            need_set_new_link_state = TRUE;
            if ( 0 == iter->successful_response_payload_len && params->successful_response_payload_len > 0 && need_set_new_link_state )
            {
              iter->successful_response_payload_len = params->successful_response_payload_len;
              memcpy( (void *)&iter->successful_response_payload, (void *)&params->successful_response_payload, params->successful_response_payload_len );
            }
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_EVENT_NONE:                // fallthrough
          default:
            // no action
            break;
        } // switch ( event )
        if ( need_set_new_link_state )
        {
          link->exec_state      = new_link_state;
          any_link_changed      = TRUE;
          any_action_performed  = TRUE;
        }
      } // if ( is_match ) - call links match with params
      link++;
    } // for ( idx = 0; idx < iter->nof_impacted; idx++ )

    need_to_conclude_cmd = FALSE;
    if ( any_link_changed )
    { // assess if all links awaiting for conclusion
      need_to_conclude_cmd = TRUE;
      generalized_completion_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION;
      link = iter->impacted;
      // get to one result cause or abandon conclusion
      for ( idx = 0; idx < iter->nof_impacted ; idx++ )
      {
        switch ( link->exec_state )
        {
          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION:
            // we may conclude, keeping need_to_conclude_cmd set
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION:
            generalized_completion_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION;
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_TIMEOUT_READY_FOR_COMPLETION:
            generalized_completion_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_TIMEOUT_READY_FOR_COMPLETION;
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION:
            switch ( generalized_completion_state )
            {
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_TIMEOUT_READY_FOR_COMPLETION:                  // fallthrough
              case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION:
                // no change
                break;

              default:
                generalized_completion_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION;
                break;
            }
            // we may conclude, keeping need_to_conclude_cmd set
            break;

          case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION:
            generalized_completion_state = QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION;
            need_to_conclude_cmd = TRUE;
            break;

          default:
            if ( QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION != generalized_completion_state )
            {
              need_to_conclude_cmd = FALSE; // not ready for conclusion
            }
            break;
        }
        link++;
      } // for ( idx = 0; idx < iter->nof_impacted && need_to_conclude_cmd ; idx++ )
      if ( need_to_conclude_cmd )
      { // do conclude - need to respond and conclude the pending oversight
        //if ( QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION != generalized_completion_state )
        {
          switch ( generalized_completion_state )
          {
            case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_TIMEOUT_READY_FOR_COMPLETION:
              ril_req_res = RIL_E_INTERNAL_ERR;
              break;

            case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_FAILURE_RESP_READY_FOR_COMPLETION:
              if (iter->successful_response_payload_len > 0)
              {
                ril_req_res = (RIL_Errno)iter->successful_response_payload;
              }
              else
              {
                ril_req_res = RIL_E_INTERNAL_ERR;
              }
              break;

            case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_CALL_ENDED_READY_FOR_COMPLETION:
            case QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_RECEIVED_IND_AND_RESP_READY_FOR_COMPLETION:
            default:
              if (iter->successful_response_payload_len > 0)
              {
                ril_req_res = (RIL_Errno)iter->successful_response_payload;
              }
              else
              {
                ril_req_res = RIL_E_SUCCESS;
              }
              break;
          }

        } // if ( QMI_RIL_VOICE_IMS_EXEC_INTERMED_STATE_ABANDONED_READY_FOR_COMPLETION != generalized_completion_state )

        if (iter->completion_action)
        {
          CommonVoiceResponseData respData = {iter->token, ril_req_res, iter};
          iter->completion_action(&respData);
        }

        // reset elaboration from call obj
        link                 = iter->impacted;
        for ( idx = 0; idx < iter->nof_impacted; idx++ )
        {
          if ( QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN == link->linkage_type )
          {
            call_entry = qcril_qmi_voice_voip_find_call_info_entry_by_elaboration( link->linkage.elaboration_pattern, TRUE );
            if ( NULL != call_entry )
            {
              call_entry->elaboration &= ~link->linkage.elaboration_pattern;
            }
          }
          link++;
        } // for ( idx = 0; idx < iter->nof_impacted; idx++ )

        qmi_ril_voice_ims_destroy_command_oversight( iter );
      } // if ( need_to_conclude_cmd )

    } // if ( any_link_changed )

    if ( !need_to_conclude_cmd )
    {
      qmi_ril_voice_ims_command_oversight_dump( iter );
    }
    else
    {
      QCRIL_LOG_INFO("command concluded");
    }

    iter = qmi_ril_voice_ims_command_oversight_next();

  } // while ( NULL != iter ), iteration through oversight command objects

  QCRIL_LOG_FUNC_RETURN_WITH_RET( any_action_performed );

  return any_action_performed;
} // qmi_ril_voice_ims_command_oversight_handle_event

//===========================================================================
// qmi_ril_voice_ims_command_oversight_dump
//===========================================================================
void qmi_ril_voice_ims_command_oversight_dump( qmi_ril_voice_ims_command_exec_oversight_type* command_oversight )
{
  int                                                   idx;
  qmi_ril_voice_ims_command_exec_oversight_link_type*   link;
  char                                                  link_info_buf[ QCRIL_MAX_LOG_MSG_SIZE ];

  if ( NULL != command_oversight )
  {
    QCRIL_LOG_INFO("-------------------------");
    QCRIL_LOG_INFO("oversight obj android request id %d, token id %" PRId32 ", nof_impacted %d",
                   command_oversight->android_request_id,
                   command_oversight->token,
                   command_oversight->nof_impacted );

    link = command_oversight->impacted;
    for ( idx = 0; idx < command_oversight->nof_impacted; idx++ )
    {
      switch ( link->linkage_type )
      {
        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_QMI_CALL_ID:
          snprintf( link_info_buf, QCRIL_MAX_LOG_MSG_SIZE, "qmi call id %d", link->linkage.qmi_call_id );
          break;

        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ANDROID_CALL_ID:
          snprintf( link_info_buf, QCRIL_MAX_LOG_MSG_SIZE, "atel call id %d", link->linkage.android_call_id );
          break;

        case QMI_RIL_VOICE_IMS_EXEC_OVERSIGHT_LINKAGE_ELABORATION_PATTERN:
          snprintf( link_info_buf, QCRIL_MAX_LOG_MSG_SIZE, "elaboration %x-%x", (unsigned int)(link->linkage.elaboration_pattern >> 32), (unsigned int)(link->linkage.elaboration_pattern & 0xFFFFFFUL) );
          break;

        default:
          snprintf( link_info_buf, QCRIL_MAX_LOG_MSG_SIZE, "no info" );
          break;
      }
      QCRIL_LOG_INFO(".link#%d, state %d, %s, exp c-state %d",
                     idx,
                     link->exec_state,
                     link_info_buf,
                     link->target_call_state
                     );

      link++;
    }
    QCRIL_LOG_INFO("-------------------------");
  }
} // qmi_ril_voice_ims_command_oversight_dump

boolean qcril_qmi_voice_has_specific_call(qcril_qmi_voice_call_filter filter, call_mode_enum_v02 *call_mode)
{
    boolean ret = FALSE;

    if (filter)
    {
        qcril_qmi_voice_voip_lock_overview();
        qcril_qmi_voice_voip_call_info_entry_type *call_info_entry =
            qcril_qmi_voice_voip_call_info_entries_enum_first();

        while ( NULL != call_info_entry )
        {
            if (filter(call_info_entry))
            {
                if(call_mode != NULL)
                {
                    *call_mode = call_info_entry->voice_scv_info.mode;
                }
                ret = TRUE;
                break;
            }
            call_info_entry = qcril_qmi_voice_voip_call_info_entries_enum_next();
        }
        qcril_qmi_voice_voip_unlock_overview();
    }

    return ret;
}

//===========================================================================
// qcril_qmi_voice_has_crs_type_call
//===========================================================================
boolean qcril_qmi_voice_has_crs_type_call(
        const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
   boolean ret = FALSE;

   if (qcril_qmi_voice_voip_call_info_is_single_elaboration_set(
          call_info_entry,
          QCRIL_QMI_VOICE_VOIP_CALLINFO_EXT_ELA_IS_CRS_TYPE_VALID))
   {
    if(call_info_entry->crs_type == VOICE_CALL_CRS_TYPE_VIDEO_V02
         || call_info_entry->crs_type == VOICE_CALL_CRS_TYPE_AUDIO_V02)
      {
         ret = TRUE;
      }
   }
   return ret;
} // qcril_qmi_voice_has_crs_type_call

//===========================================================================
// qcril_qmi_voice_set_audio_call_type
//===========================================================================
void qcril_qmi_voice_set_audio_call_type(const voice_call_info2_type_v02* iter_call_info, qcril_qmi_voice_voip_call_info_entry_type *call_info_entry)
{
   call_mode_enum_v02 call_mode;
   call_type_enum_v02 call_type;

   if (NULL == call_info_entry)
   {
      QCRIL_LOG_ERROR("call_info_entry is NULL");
   }
   else
   {
      if (NULL != iter_call_info)
      {
         call_type = iter_call_info->call_type;
         call_mode = iter_call_info->mode;
      }
      else
      {
         call_type = call_info_entry->voice_scv_info.call_type;
         call_mode = call_info_entry->voice_scv_info.mode;
      }

      switch(call_mode)
      {
        case CALL_MODE_NO_SRV_V02:
        case CALL_MODE_UNKNOWN_V02:
          switch(call_type)
          {
            case CALL_TYPE_VOICE_IP_V02:
            case CALL_TYPE_VT_V02:
            case CALL_TYPE_EMERGENCY_IP_V02:
            case CALL_TYPE_EMERGENCY_VT_V02:
              QCRIL_LOG_INFO("Set audio call_type as IMS");
              call_info_entry->audio_call_type = QMI_RIL_VOICE_IMS_AUDIO_CALL_TYPE_IMS;
              break;

            default:
              QCRIL_LOG_INFO("Set audio call_type as VOICE");
              call_info_entry->audio_call_type = QMI_RIL_VOICE_IMS_AUDIO_CALL_TYPE_VOICE;
              break;
          }
          break;

        case CALL_MODE_LTE_V02:
        case CALL_MODE_NR5G_V02:
        case CALL_MODE_WLAN_V02:
        case CALL_MODE_C_IWLAN_V02:
          QCRIL_LOG_INFO("Set audio call_type as IMS");
          call_info_entry->audio_call_type = QMI_RIL_VOICE_IMS_AUDIO_CALL_TYPE_IMS;
          break;

        default:
          QCRIL_LOG_INFO("Set audio call_type as VOICE");
          call_info_entry->audio_call_type = QMI_RIL_VOICE_IMS_AUDIO_CALL_TYPE_VOICE;
          break;
      }
   }
} /* qcril_qmi_voice_set_audio_call_type */

//===========================================================================
// qcril_qmi_voice_get_current_setup_calls
//===========================================================================
void qcril_qmi_voice_get_current_setup_calls
(
 std::shared_ptr<QcRilRequestGetCurrentSetupCallsMessage> msg
)
{
    std::shared_ptr<qcril::interfaces::GetCurrentSetupCallsRespData> payload_ptr = NULL;
    qcril_qmi_voice_voip_call_info_entry_type* call_info_entry = NULL;
    boolean is_call_present = FALSE;
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril::interfaces::CallInfo callInfo{};

    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_voice_voip_lock_overview();
    call_info_entry =
        qcril_qmi_voice_voip_find_call_info_entry_by_qmi_call_state(CALL_STATE_SETUP_V02);
    do
    {
        if (NULL == call_info_entry)
        {
            result = RIL_E_SUCCESS;
            break;
        }
        is_call_present = TRUE;
        payload_ptr = std::make_shared<qcril::interfaces::GetCurrentSetupCallsRespData>();
        if (NULL == payload_ptr)
        {
            break;
        }
        if (qcril_qmi_voice_gather_current_call_information(callInfo, call_info_entry) != RIL_E_SUCCESS)
        {
            break;
        }
        payload_ptr->setIndex(callInfo.getIndex());
        payload_ptr->setToa(callInfo.getToa());
        payload_ptr->setAls(callInfo.getAls());
        payload_ptr->setIsVoice(callInfo.getIsVoice());
        if (callInfo.hasNumber() && !callInfo.getNumber().empty()) {
          payload_ptr->setNumber(callInfo.getNumber());
        }
        payload_ptr->setNumberPresentation(callInfo.getNumberPresentation());
        if (callInfo.hasName() && !callInfo.getName().empty()) {
          payload_ptr->setName(callInfo.getName());
        }
        payload_ptr->setNamePresentation(callInfo.getNamePresentation());
        if (callInfo.hasRedirNum() && !callInfo.getRedirNum().empty()) {
          payload_ptr->setRedirNum(callInfo.getRedirNum());
        }
        payload_ptr->setRedirNumPresentation(callInfo.getRedirNumPresentation());

        result = RIL_E_SUCCESS;
    } while (FALSE);

    qcril_qmi_voice_voip_unlock_overview();

    if (RIL_E_SUCCESS == result)
    {
        QCRIL_LOG_INFO ("Reply to OEM --> is_call_present %d", is_call_present);
        if (is_call_present)
        {
            QCRIL_LOG_INFO ("Reply to OEM --> index %d, toa %d, als %d, isVoice %d",
                           payload_ptr->getIndex(),
                           payload_ptr->getToa(),
                           payload_ptr->getAls(),
                           payload_ptr->getIsVoice());
            std::string numStr = PII(payload_ptr->getNumber(),
                                   hide_phone_number(payload_ptr->getNumber().c_str()));
            std::string redirNumStr =  PII(payload_ptr->getRedirNum(),
                                         hide_phone_number(payload_ptr->getRedirNum().c_str()));
            QCRIL_LOG_INFO ("...num %s, num presentation %d, name %s, name presentation %d"
                            "redirected number %s,  redirected number presentation %d",
                           numStr.c_str(),
                           payload_ptr->getNumberPresentation(),
                           payload_ptr->getName().c_str(),
                           payload_ptr->getNamePresentation(),
                           redirNumStr.c_str(),
                           payload_ptr->getRedirNumPresentation());
        }
    }

    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(result, payload_ptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_get_current_setup_calls

//===========================================================================
// qcril_qmi_voice_voip_reset_answered_call_type
//===========================================================================
void qcril_qmi_voice_voip_reset_answered_call_type
(
 qcril_qmi_voice_voip_call_info_entry_type *call_info,
 voice_modified_ind_msg_v02                *modify_ind_ptr
)
{
  if(call_info != NULL && call_info->answered_call_type != qcril::interfaces::CallType::UNKNOWN)
  {
    if (call_info->voice_scv_info.call_type == CALL_TYPE_VT_V02)
    {
      if ((modify_ind_ptr->audio_attrib_valid && 0 != modify_ind_ptr->audio_attrib) &&
          (modify_ind_ptr->video_attrib_valid && 0 != modify_ind_ptr->video_attrib))
      {
        call_info->answered_call_type = qcril::interfaces::CallType::UNKNOWN;
      }
    }
    else if (call_info->voice_scv_info.call_type == CALL_TYPE_VOICE_IP_V02)
    {
      if (modify_ind_ptr->audio_attrib_valid && 0 != modify_ind_ptr->audio_attrib)
      {
        call_info->answered_call_type = qcril::interfaces::CallType::UNKNOWN;
      }
    }
  }
} /* qcril_qmi_voice_voip_reset_answered_call_type */

//===========================================================================
// qcril_qmi_voice_send_simulated_SRVCC_ind
//===========================================================================
void qcril_qmi_voice_send_simulated_SRVCC_ind(void)
{
  QCRIL_LOG_FUNC_ENTRY();

  RIL_SrvccState ril_srvccstate = HANDOVER_STARTED;
  qcril_qmi_voice_send_ims_unsol_resp_handover(ril_srvccstate);

  ril_srvccstate = HANDOVER_COMPLETED;
  qcril_qmi_voice_send_ims_unsol_resp_handover(ril_srvccstate);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_voice_send_simulated_SRVCC_ind


/*=========================================================================
  FUNCTION:  qcril_qmi_voice_send_ims_unsol_resp_handover

===========================================================================*/
/*!
    @brief
    Sends UNSOL_RESPONSE_HANDOVER on IMS socket.

    @return
    None.
*/
/*=========================================================================*/
static void qcril_qmi_voice_send_ims_unsol_resp_handover(RIL_SrvccState ril_srvccstate)
{
  qcril::interfaces::HandoverType type = qcril::interfaces::HandoverType::UNKNOWN;
  switch (ril_srvccstate)
  {
    case HANDOVER_STARTED:
      type = qcril::interfaces::HandoverType::START;
      break;
    case HANDOVER_COMPLETED:
      type = qcril::interfaces::HandoverType::COMPLETE_SUCCESS;
      break;
    case HANDOVER_FAILED:
      type = qcril::interfaces::HandoverType::COMPLETE_FAIL;
      break;
    case HANDOVER_CANCELED:
      type = qcril::interfaces::HandoverType::CANCEL;
      break;
    default:
      break;
  }

  if (type != qcril::interfaces::HandoverType::UNKNOWN)
  {
    auto srvccMsg = std::make_shared<QcRilUnsolSrvccStatusMessage>();
    if (srvccMsg != nullptr)
    {
      srvccMsg->setState(ril_srvccstate);
      Dispatcher::getInstance().dispatchSync(srvccMsg);
    }

    auto msg = std::make_shared<QcRilUnsolImsHandoverMessage>();
    if (msg != nullptr)
    {
      msg->setHandoverType(type);
      Dispatcher::getInstance().dispatchSync(msg);
    }
  }
}

static void qcril_qmi_voice_send_unsol_ussd
(
 qcril::interfaces::UssdDomain domain,
 qcril::interfaces::UssdModeType mode,
 std::string ussdMessage,
 std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails
)
{
  auto msg = std::make_shared<QcRilUnsolOnUssdMessage>();
  if (msg != nullptr)
  {
    if (domain != qcril::interfaces::UssdDomain::UNKNOWN)
    {
      if (qcril_qmi_voice_info.ussd_request_domain !=
             qcril::interfaces::UssdDomain::UNKNOWN) {
        msg->setDomain(qcril_qmi_voice_info.ussd_request_domain);
      } else {
        msg->setDomain(domain);
      }
    }
    if (mode != qcril::interfaces::UssdModeType::UNKNOWN)
    {
      msg->setMode(mode);
    }
    if (!ussdMessage.empty())
    {
      msg->setMessage(ussdMessage);
    }
    if (errorDetails != nullptr)
    {
      msg->setErrorDetails(errorDetails);
    }
    Dispatcher::getInstance().dispatchSync(msg);
  }
}

void qcril_qmi_voice_reset_ussd_power_opt_buffer()
{
    QCRIL_LOG_INFO("Reset USSD Power Opt buffer");
    qcril_qmi_voice_info.ussd_info_buffer_for_opt.clear();
    qcril_qmi_voice_info.ussd_info_buffer_for_opt.shrink_to_fit();
}

void qcril_qmi_voice_update_ussd_power_opt_buffer(const voice_ussd_ind_msg_v02 &ind_data)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_voice_info.ussd_info_buffer_for_opt.push_back(ind_data);
  QCRIL_LOG_INFO("USSD Power Opt buffer length %d",
                 qcril_qmi_voice_info.ussd_info_buffer_for_opt.size());
  QCRIL_LOG_FUNC_RETURN();
}

void qcril_qmi_voice_notify_ussd_power_opt_to_atel()
{
  QCRIL_LOG_INFO("USSD Power Opt buffer length %d",
                 qcril_qmi_voice_info.ussd_info_buffer_for_opt.size());
  for(const auto &ind : qcril_qmi_voice_info.ussd_info_buffer_for_opt)
  {
    qcril_qmi_voice_ussd_ind_hdlr(ind);
  }
  qcril_qmi_voice_reset_ussd_power_opt_buffer();
}

//===========================================================================
// qcril_qmi_voice_sip_dtmf_resp_hdlr
//===========================================================================
void qcril_qmi_voice_sip_dtmf_resp_hdlr
(
  uint16_t req_id,
  voice_sip_dtmf_resp_msg_v02 *sip_dtmf_resp,
  CommonVoiceResponseCallback responseCb
)
{
  qcril_instance_id_e_type instance_id;
  RIL_Errno ril_err;
  qmi_result_type_v01 qmi_result;
  qmi_error_type_v01  qmi_error;

  instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  if (sip_dtmf_resp)
  {
    qmi_result = sip_dtmf_resp->resp.result;
    qmi_error = sip_dtmf_resp->resp.error;
    QCRIL_LOG_INFO("QCRIL QMI VOICE send sip dtmf qmi_error : %d", qmi_error );
    ril_err = qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error);
  }
  else
  {
    ril_err = RIL_E_SYSTEM_ERR;
  }

  if (responseCb)
  {
    CommonVoiceResponseData respData = {req_id, ril_err, sip_dtmf_resp};
    responseCb(&respData);
  }
}

//===========================================================================
// qcril_qmi_voice_process_sip_dtmf_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_sip_dtmf_req
(
 uint16_t req_id,
 voice_sip_dtmf_req_msg_v02 &sip_dtmf_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SIP_DTMF_REQ_V02,
        &sip_dtmf_req,
        sizeof(voice_sip_dtmf_req_msg_v02),
        sizeof(voice_sip_dtmf_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          (void)transp_err;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_sip_dtmf_resp_msg_v02 *qmi_response =
              (voice_sip_dtmf_resp_msg_v02 *)(resp_c_struct.get());
          qcril_qmi_voice_sip_dtmf_resp_hdlr(req_id, qmi_response, responseCb);
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
}

//===========================================================================
// qcril_qmi_voice_process_send_vos_support_status_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_send_vos_support_status_req
(
 uint16_t req_id,
 voice_sip_vos_capability_ack_req_msg_v02 &vos_capability_ack_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SIP_VOS_CAPABILITY_ACK_REQ_V02,
        &vos_capability_ack_req,
        sizeof(voice_sip_vos_capability_ack_req_msg_v02),
        sizeof(voice_sip_vos_capability_ack_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_sip_vos_capability_ack_resp_msg_v02 *qmi_response =
              (voice_sip_vos_capability_ack_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
}

//===========================================================================
// qcril_qmi_voice_process_send_vos_action_info_req
//===========================================================================
RIL_Errno qcril_qmi_voice_process_send_vos_action_info_req
(
 uint16_t req_id,
 voice_sip_vos_action_req_msg_v02 &vos_action_req,
 CommonVoiceResponseCallback responseCb
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32_t user_data;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error_type client_err = qmi_client_voice_send_async(
        QMI_VOICE_SIP_VOS_ACTION_REQ_V02,
        &vos_action_req,
        sizeof(voice_sip_vos_action_req_msg_v02),
        sizeof(voice_sip_vos_action_resp_msg_v02),
        [responseCb](unsigned int msg_id, std::shared_ptr<void> resp_c_struct,
                     unsigned int resp_c_struct_len, void *resp_cb_data,
                     qmi_client_error_type transp_err) -> void {
          (void)msg_id;
          (void)resp_c_struct_len;
          RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
          uint32_t user_data = (uint32_t)(uintptr_t)resp_cb_data;
          uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
          voice_sip_vos_action_resp_msg_v02 *qmi_response =
              (voice_sip_vos_action_resp_msg_v02 *)(resp_c_struct.get());

          if (transp_err == QMI_NO_ERR && qmi_response) {
            ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
                QMI_NO_ERR, &qmi_response->resp, QCRIL_QMI_ERR_CTX_NONE, qmi_response);
            QCRIL_LOG_INFO(".. ril res %d, qmi res %d", (int)ril_req_res,
                           (int)qmi_response->resp.error);
          }
          if (responseCb) {
            CommonVoiceResponseData respData = {req_id, ril_req_res, qmi_response};
            responseCb(&respData);
          }
        },
        (void *)(uintptr_t)user_data);
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(client_err, NULL);
    QCRIL_LOG_INFO("client_err = %d, ril_err = %d", client_err, ril_err);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ril_err);
  return ril_err;
}

//===========================================================================
// qcril_qmi_voice_set_call_reason
//
// @description Set the call reason in RIL received from QMI
//
// @param call_id
// @param call_status_ind_ptr
// @param call_info_entry
//===========================================================================
void qcril_qmi_voice_set_call_reason
(
 const uint8_t call_id,
 const voice_all_call_status_ind_msg_v02* call_status_ind_ptr,
 qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
  if (!call_status_ind_ptr || !call_info_entry || !call_status_ind_ptr->call_reason_valid) {
    return;
  }
  for (int i = 0; i < call_status_ind_ptr->call_reason_len; i++) {
    const auto& reason = call_status_ind_ptr->call_reason[i];
    if (reason.call_id != call_id) {
      continue;
    }
    int length = reason.call_reason_len;
    QCRIL_LOG_DEBUG("call reason length %d", length);
    char *call_reason = qcril_qmi_voice_convert_call_reason((uint16_t*)reason.call_reason,
            length);
    if (call_info_entry->call_reason) qcril_free(call_info_entry->call_reason);
    call_info_entry->call_reason = (char*)qcril_malloc(strlen(call_reason) + 1);
    if (!call_info_entry->call_reason)
    {
        QCRIL_LOG_ERROR("malloc failed for entry->call_reason[call id = %d]", call_id);
        break;
    }
    strlcpy(call_info_entry->call_reason, call_reason, strlen(call_reason) + 1);
    delete[] call_reason;
    break;
  }
}

//===========================================================================
// qcril_qmi_voice_convert_call_reason
//
// @description Convert call reason to UTF8
//
// @param reason - call reason sent from QMI
// @param length - the length of the call reason from QMI
//
// @output call_reason - the converted call reason in UTF8
//===========================================================================
char* qcril_qmi_voice_convert_call_reason
(
 const uint16_t reason[QMI_VOICE_CALL_REASON_MAX_LEN_V02],
 const uint32_t length)
{
  uint32_t call_reason_len = 0;
  char *call_reason = new char[(QMI_VOICE_CALL_REASON_MAX_LEN_V02 * 2)];
  if (call_reason == nullptr) {
    QCRIL_LOG_ERROR("memory allocation failed for call_reason");
    return call_reason;
  }
  memset(call_reason, 0, (QMI_VOICE_CALL_REASON_MAX_LEN_V02 * 2));
  call_reason_len = qcril_cm_ss_convert_ucs2_to_utf8((const char*)reason,
          length * 2, call_reason, (QMI_VOICE_CALL_REASON_MAX_LEN_V02 * 2));
  QCRIL_LOG_DEBUG("call reason %s", call_reason);
  return call_reason;
}

void qcril_qmi_voice_set_operational_status(
    qmi_ril_gen_operational_status_type status)
{
  voice_operational_status_mutex.lock();
  voice_operational_status = status;
  voice_operational_status_mutex.unlock();
}

qmi_ril_gen_operational_status_type
qcril_qmi_voice_get_operational_status()
{
  voice_operational_status_mutex.lock();
  auto res = voice_operational_status;
  voice_operational_status_mutex.unlock();

  return res;
}

void insert_qmi_call_ids(std::vector<uint8_t> &out) {
  qcril_qmi_voice_voip_call_info_entry_type* iter = NULL;
  qcril_qmi_voice_voip_lock_overview();
  iter = qmi_voice_voip_overview.call_info_root;
  while (iter) {
    out.push_back(iter->qmi_call_id);
    iter = iter->next;
  }
  qcril_qmi_voice_voip_unlock_overview();
}


#ifdef QMI_RIL_UTF
void qcril_qmi_hal_voice_module_cleanup()
{
    getVoiceModule()->cleanup();
    qcril_qmi_voice_cleanup();

    if (qcril_qmi_voice_info.pd_info.handle &&
        qcril_pd_notifier_available(qcril_qmi_voice_info.pd_info.handle))
    {
        qcril_pd_notifier_stop_listening_all(qcril_qmi_voice_info.pd_info.handle);
        qcril_pd_notifier_unregister_for_all(qcril_qmi_voice_info.pd_info.handle);
    }
    qcril_qmi_pd_notifier_release(qcril_qmi_voice_info.pd_info.handle);
    qcril_qmi_voice_info.pd_info.handle = nullptr;

    diag_odl_logging_in_progress = FALSE;
    disabled_screen_off_ind = FALSE;
}
#endif
