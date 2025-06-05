/******************************************************************************
#  Copyright (c) 2013-2017, 2020-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_am.c
  @brief   qcril qmi - Audio Management

  DESCRIPTION
    Implements Audio Management APIs.

******************************************************************************/

#define LOG_TAG "RILQ"
#define TAG LOG_TAG

#include <cutils/properties.h>
#include <framework/Log.h>
#include <framework/legacy.h>
#include <WakeLock.h>

#include "modules/ims/ImsServiceStatusInd.h"
#include "qcril_am.h"
#include "ip_multimedia_subsystem_application_v01.h"

#define __STDC_FORMAT_MACROS 1

#include "interfaces/audio/QcRilGetAudioParamSyncMessage.h"
#include "interfaces/audio/QcRilSetAudioParamSyncMessage.h"
#include "interfaces/audio/QcRilQueryAudioParamMessage.h"
#include "interfaces/audio/QcRilSetAudioParamMessage.h"
#include "qcril_config.h"
#include "qcril_qmi_voice.h"
#include "qcril_voice_external.h"
#include "qcril_legacy_apis.h"

extern "C" {
#include "qcrili.h"
}

using namespace qcril::interfaces;

static qtimutex::QtiRecursiveMutex am_state_mutex;
static uint8_t num_of_rilds;
static boolean ignore_ims_wlan;
static boolean is_crs_supported_from_audio_hal;

typedef struct {
    boolean is_valid;
    uint32_t vsid;
} voice_vsid_type;

typedef struct {
    // apply to all rild instances
    voice_vsid_type voice_vsid;
    voice_vsid_type lte_vsid;
    voice_vsid_type wlan_vsid;
    boolean in_lch;

    // apply to primary rild only
    boolean ims_on_wlan;
    call_mode_enum_v02 emergency_rat;
} qcril_am_state_type;

qcril_am_state_type am_state;


/***************************************************************************************************
    @function
    qcril_am_lock - this lock could be recursively calling within a thread
***************************************************************************************************/
static void qcril_am_lock()
{
  QCRIL_LOG_ADDITIONAL("BEFORE LOCK am_state_mutex");
  am_state_mutex.lock();
  QCRIL_LOG_ADDITIONAL("AFTER LOCK am_state_mutex");
}

/***************************************************************************************************
    @function
    qcril_am_unlock
***************************************************************************************************/
static void qcril_am_unlock()
{
  QCRIL_LOG_ADDITIONAL("BEFORE UNLOCK am_state_mutex");
  am_state_mutex.unlock();
  QCRIL_LOG_ADDITIONAL("BEFORE UNLOCK am_state_mutex");
}

/***************************************************************************************************
    @function
    qcril_am_get_audio_vsid

    @implementation detail
    Gets vsid for the voice subsystem as queried.

    @param[in]
        vs_type
            voice subsystem type which is queried

    @param[out]
        vsid
            vsid of the voice subsystem type queried

    @retval
    QCRIL_AM_ERR_NONE if function is successful, QCRIL_AM_ERR_VSID_NOT_AVAILABLE if the vsid
    is not available.
***************************************************************************************************/
static RIL_Errno qcril_am_get_audio_vsid(qcril_am_vs_type vs_type, uint32_t *vsid)
{
    uint32_t voice_modem_vsid[QCRIL_MAX_NUM_VOICE_MODEM] = {0x10C01000, 0x10DC1000};
    RIL_Errno err = RIL_E_SUCCESS;
    int modem_index = 0;

    // TODO: did not find IMS_VSID and VOICE_VSID value in header file
    switch (vs_type)
    {
    case QCRIL_AM_VS_IMS:
        qcril_am_lock();
        if (am_state.lte_vsid.is_valid)
        {
            *vsid = am_state.lte_vsid.vsid;
        }
        else
        {
            *vsid = 0x10C02000; // IMS_VSID
        }
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS_WLAN:
        qcril_am_lock();
        if (am_state.wlan_vsid.is_valid)
        {
            *vsid = am_state.wlan_vsid.vsid;
        }
        else
        {
            *vsid = 0x10002000; // IMS_WLAN_VSID
        }
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_VOICE:
        if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SGLTE))
        {
            int int_config;
            if (qcril_config_get(PERSIST_VENDOR_RADIO_VOICE_MODEM_INDEX, int_config) == E_SUCCESS)
            {
                if ((int_config >= QMI_RIL_ZERO) && (int_config < QCRIL_MAX_NUM_VOICE_MODEM))
                {
                    *vsid = voice_modem_vsid[modem_index];
                }
                else
                {
                    err = RIL_E_GENERIC_FAILURE;
                    QCRIL_LOG_INFO("Invalid modem index!");
                }
            }
            else
            {
                err = RIL_E_GENERIC_FAILURE;
                QCRIL_LOG_ERROR("modem index is not set");
            }
        }
        else
        {
            qcril_am_lock();
            if (am_state.voice_vsid.is_valid)
            {
                *vsid = am_state.voice_vsid.vsid;
            }
            else
            {
                *vsid = 0x10C01000; // VOICE_VSID
            }
            qcril_am_unlock();
        }
        break;
    default:
        err = RIL_E_GENERIC_FAILURE;
    }
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_get_current_ims_vs_type

    @brief
    Retrieves current IMS vs type (ie., LTE or WLAN)
***************************************************************************************************/
qcril_am_vs_type qcril_am_get_current_ims_vs_type()
{
  qcril_am_vs_type vs_type = QCRIL_AM_VS_IMS;

  if (am_state.emergency_rat == CALL_MODE_WLAN_V02 ||
      am_state.emergency_rat == CALL_MODE_C_IWLAN_V02)
  {
    vs_type = QCRIL_AM_VS_IMS_WLAN;
  }
  else if (am_state.emergency_rat == CALL_MODE_LTE_V02 ||
          am_state.emergency_rat == CALL_MODE_NR5G_V02)
  {
    vs_type = QCRIL_AM_VS_IMS;
  }
  else
  {
    if (am_state.ims_on_wlan)
    {
      vs_type = QCRIL_AM_VS_IMS_WLAN;
    }
    else
    {
      vs_type = QCRIL_AM_VS_IMS;
    }
  }

  QCRIL_LOG_INFO("am_state.ims_on_wlan: %d, am_state.emergency_rat = %d, ims_vs_type = %d",
      am_state.ims_on_wlan, am_state.emergency_rat, vs_type);

  return vs_type;
}

/***************************************************************************************************
    @function
    qcril_am_set_emergency_rat

    @brief
    Set emergency_rat based on mode in origination state for emergency calls.
***************************************************************************************************/
void qcril_am_set_emergency_rat(call_mode_enum_v02 rat)
{
  qcril_am_lock();
  am_state.emergency_rat = rat;
  qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_reset_emergency_rat

    @brief
   Reset ims_on_lte after the emergency call ends .
***************************************************************************************************/
void qcril_am_reset_emergency_rat()
{
  qcril_am_lock();
  am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;
  qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_get_audio_call_state

    @implementation detail
    Maps qcril_am call_state to call_state as defined in AudioSystem.h
***************************************************************************************************/
AudioCallState qcril_am_get_audio_call_state(AudioCallState call_state)
{
    AudioCallState ret = qcril::interfaces::AudioCallState::INACTIVE;
    if (call_state != qcril::interfaces::AudioCallState::INVALID)
    {
        ret = call_state;
        if (qcril::interfaces::AudioCallState::ACTIVE == ret)
        {
            qcril_am_lock();
            if (am_state.in_lch)
            {
                ret = qcril::interfaces::AudioCallState::LOCAL_HOLD;
            }
            qcril_am_unlock();
        }
    }
    else
    {
        QCRIL_LOG_DEBUG("invalid call_state value: %d", call_state);
    }
    return ret;
}


/***************************************************************************************************
    @function
    qcril_am_is_active_call

    @implementation detail
    Checks if a call will be considered as an active call from AM perspective. It includes both
    current active call and the call expected to be changed to active by modem.
***************************************************************************************************/
static boolean qcril_am_is_active_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    boolean ret = FALSE;
    if (call_info_entry)
    {
        if ( CALL_STATE_CONVERSATION_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_ORIGINATING_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_ALERTING_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_DISCONNECTING_V02 == call_info_entry->voice_scv_info.call_state ||
             call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL ||
             call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_SWITCHING_CALL_TO_ACTIVE
           )
        {
            ret = TRUE;
        }
    }
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_call

    @implementation detail
    Checks if a call is a hold call.
***************************************************************************************************/
static boolean qcril_am_is_hold_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    boolean ret = FALSE;
    if (call_info_entry)
    {
        if ( CALL_STATE_HOLD_V02 == call_info_entry->voice_scv_info.call_state )
        {
            ret = TRUE;
        }
    }
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_active_ims_call

    @implementation detail
    Checks if a call will be considered as an active ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_active_ims_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_ims_audio(call_info_entry) &&
           (qcril_am_is_active_call(call_info_entry) ||
            (is_crs_supported_from_audio_hal ?
             false : qcril_qmi_voice_has_crs_type_call(call_info_entry)));
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_ims_call

    @implementation detail
    Checks if a call is a hold ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_hold_ims_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_ims_audio(call_info_entry) &&
           qcril_am_is_hold_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_is_active_voice_call

    @implementation detail
    Checks if a call will be considered as an active voice call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_active_voice_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_voice_audio(call_info_entry) &&
           qcril_am_is_active_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_voice_call

    @implementation detail
    Checks if a call is a hold ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_hold_voice_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_voice_audio(call_info_entry) &&
           qcril_am_is_hold_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_set_voice_call_audio_driver_by_call_state
***************************************************************************************************/
static RIL_Errno qcril_am_set_voice_call_audio_driver_by_call_state()
{
    RIL_Errno ret = RIL_E_SUCCESS;
    call_mode_enum_v02 call_mode = CALL_MODE_UNKNOWN_V02;

    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_voice_call, &call_mode))
    {
        QCRIL_LOG_INFO("has active voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                        qcril::interfaces::AudioCallState::ACTIVE, call_mode);
    }
    else if (qcril_qmi_voice_has_specific_call(qcril_am_is_hold_voice_call, &call_mode))
    {
        QCRIL_LOG_INFO("has hold voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                          qcril::interfaces::AudioCallState::HOLD, call_mode);
    }
    else
    {
        QCRIL_LOG_INFO("no active and hold voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                      qcril::interfaces::AudioCallState::INACTIVE, call_mode,
                                      qcril_qmi_voice_has_specific_call(
                                          qcril_qmi_voice_has_crs_type_call, NULL));
    }

    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_set_ims_call_audio_driver_by_call_state
***************************************************************************************************/
static RIL_Errno qcril_am_set_ims_call_audio_driver_by_call_state(boolean ignore_no_calls_case = FALSE)
{
    RIL_Errno ret = RIL_E_SUCCESS;
    call_mode_enum_v02 call_mode = CALL_MODE_UNKNOWN_V02;
    qcril_am_vs_type current_vs_type = qcril_am_get_current_ims_vs_type();

    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, &call_mode))
    {
        if (current_vs_type == QCRIL_AM_VS_IMS_WLAN)
        {
          QCRIL_LOG_INFO("has active ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::ACTIVE,
                                               call_mode);
        }
        else
        {
          QCRIL_LOG_INFO("has active ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::ACTIVE,
                                               call_mode);
        }
    }
    else if (qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, &call_mode))
    {
        if(current_vs_type == QCRIL_AM_VS_IMS_WLAN)
        {
          QCRIL_LOG_INFO("has hold ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::HOLD,
                                               call_mode);
        }
        else
        {
          QCRIL_LOG_INFO("has hold ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::HOLD,
                                               call_mode);
        }
    }
    else if (!ignore_no_calls_case)
    {
          QCRIL_LOG_INFO("no active and hold ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02,
                                               qcril_qmi_voice_has_specific_call(
                                                   qcril_qmi_voice_has_crs_type_call, NULL));

          QCRIL_LOG_INFO("no active and hold ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02,
                                               qcril_qmi_voice_has_specific_call(
                                                   qcril_qmi_voice_has_crs_type_call, NULL));
    }

    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_any_non_inactive_calls_with_vsid
***************************************************************************************************/
static boolean qcril_am_is_any_non_inactive_calls_with_vsid(uint32_t vsid)
{
    boolean ret = FALSE;
    uint vs_type;
    uint32_t audio_vsid;

    for (vs_type = QCRIL_AM_VS_MIN+1; (vs_type < QCRIL_AM_VS_MAX && ret == FALSE); vs_type++)
    {
        if (qcril_am_get_audio_vsid((qcril_am_vs_type)vs_type, &audio_vsid) != RIL_E_SUCCESS)
        {
            break;
        }
        QCRIL_LOG_DEBUG("(vs_type = %d, audio_vsid = %d), queried vsid = %d",
                        vs_type, audio_vsid, vsid);
        if (vsid == audio_vsid)
        {
            switch (vs_type)
            {
                case QCRIL_AM_VS_IMS:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
                    {
                        if(qcril_am_get_current_ims_vs_type() == QCRIL_AM_VS_IMS)
                        {
                            QCRIL_LOG_DEBUG("Has active/hold IMS calls");
                            ret = TRUE;
                        }
                    }
                    break;

                case QCRIL_AM_VS_IMS_WLAN:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
                    {
                        if(qcril_am_get_current_ims_vs_type() == QCRIL_AM_VS_IMS_WLAN)
                        {
                            QCRIL_LOG_DEBUG("Has active/hold WLAN calls");
                            ret = TRUE;
                        }
                    }
                    break;

                case QCRIL_AM_VS_VOICE:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_voice_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_voice_call, NULL))
                    {
                        QCRIL_LOG_DEBUG("Has active/hold VOICE calls");
                        ret = TRUE;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    QCRIL_LOG_DEBUG("ret = %d\n", ret);
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_set_ims_on_wlan_from_handover

    @brief
    Sets IMS service is on wlan or not.
***************************************************************************************************/
static void qcril_am_set_ims_on_wlan_from_handover(imsa_service_rat_enum_v01 target_rat)
{
    if (ignore_ims_wlan)
    {
        QCRIL_LOG_INFO("ignore ims wlan vsid handling");
    }
    else
    {
        qcril_am_lock();
        am_state.ims_on_wlan =
            (target_rat == IMSA_WLAN_V01 || target_rat == IMSA_C_IWLAN_V01) ? TRUE: FALSE;
        QCRIL_LOG_INFO("am_state.ims_on_wlan: %d", am_state.ims_on_wlan);
        qcril_am_unlock();
    }
}

static void qcril_am_handle_audio_rat_change(voice_audio_rat_change_info_ind_msg_v02* ind)
{
    if (ignore_ims_wlan)
    {
        QCRIL_LOG_INFO("ignore ims wlan vsid handling");
    }
    else
    {
        if ( !ind || !ind->rat_info_valid )
            return;
        bool is_rat_wlan  = (ind->rat_info == CALL_MODE_WLAN_V02 ||
                             ind->rat_info == CALL_MODE_C_IWLAN_V02);

        // If audio session info is reported
        // ==> SESSION_START: the current rat is just ind->rat_info
        // ==> SESSION_STOP:
        // 1) The current rat is CALL_MODE_WLAN_V02/CALL_MODE_C_IWLAN_V02 if modem reports CALL_MODE_LTE_V02
        // 2) The current rat is CALL_MODE_LTE_V02 if modem reports CALL_MODE_WLAN_V02
        // 3) What if modem reports other values? And is it possible?
        if ( ind->audio_session_info_valid &&
            ind->audio_session_info == VOICE_AUDIO_PASSIVE_SESSION_STOP_V02 )
        {
            is_rat_wlan = !is_rat_wlan;
        }

        qcril_am_lock();
        am_state.ims_on_wlan = is_rat_wlan;
        QCRIL_LOG_INFO("am_state.ims_on_wlan: %d", am_state.ims_on_wlan);
        qcril_am_unlock();

        // since the call could be in active or hold state, still need to check call state
        qcril_am_set_ims_call_audio_driver_by_call_state();
    }
}

/***************************************************************************************************
    @function
    qcril_am_query_crs_supported_from_audio_hal
***************************************************************************************************/
void qcril_am_query_crs_supported_from_audio_hal() {
    RIL_Errno err = RIL_E_SUCCESS;
    std::shared_ptr<RIL_Errno> respPtr {nullptr};

    auto queryParamMsg = std::make_shared<QcRilQueryAudioParamMessage>(0, "isCRSsupported");
    if (queryParamMsg != nullptr) {
        GenericCallback<QcRilGetAudioParams> cb([](std::shared_ptr<Message> msg,
            Message::Callback::Status status,
            std::shared_ptr<QcRilGetAudioParams> respPtr) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if((status == Message::Callback::Status::SUCCESS) && respPtr) {
                is_crs_supported_from_audio_hal = respPtr->is_crs_supported;
                QCRIL_LOG_INFO("crs is supported from audio : %d", is_crs_supported_from_audio_hal);
            } else {
                QCRIL_LOG_INFO("QcRilGetAudioParamMessage : Failed response or resp NULL");
            }
        });
        queryParamMsg->setCallback(&cb);
        queryParamMsg->dispatch();
    }
}

/***************************************************************************************************
    @function
    qcril_am_pre_init
***************************************************************************************************/
void qcril_am_pre_init()
{
    // initialize am_state.emergency_rat to UNKNOWN
    am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;

    num_of_rilds = qmi_ril_retrieve_number_of_rilds();

    bool bool_config;
    if (qcril_config_get(PERSIST_VENDOR_RADIO_IGNORE_IMS_WLAN, bool_config) == E_SUCCESS)
    {
        ignore_ims_wlan = bool_config;
    }

    //ProcessState::self()->startThreadPool();
}

/***************************************************************************************************
    @function
    qcril_am_set_call_audio_driver

    @implementation detail
    Maps vs_type and call_state to the value defined in AudioSystem.h,
    and calls setParameters API of AudioSystem
***************************************************************************************************/
RIL_Errno qcril_am_set_call_audio_driver
(
    qcril_am_vs_type vs_type,
    AudioCallState call_state,
    call_mode_enum_v02 call_mode,
    bool is_crs_call,
    bool force_inactive_state
)
{
    QCRIL_LOG_FUNC_ENTRY();
    RIL_Errno err = RIL_E_SUCCESS;
    uint32_t audio_vsid;

    do
    {
        QCRIL_LOG_INFO("vs_type: %d, call_state: %d, call_mode: %d",
                                        vs_type, call_state, call_mode);
        if ( vs_type <= QCRIL_AM_VS_MIN || vs_type >= QCRIL_AM_VS_MAX ||
             call_state == qcril::interfaces::AudioCallState::INVALID )
        {
            err = RIL_E_REQUEST_NOT_SUPPORTED;
            break;
        }

        err = qcril_am_get_audio_vsid(vs_type, &audio_vsid);
        if (RIL_E_SUCCESS != err)
        {
            break;
        }

        // In case of single VSID session (same VSID used for Voice/IMS/WLAN),
        // RIL should set call as INACTIVE to audio only if there is no calls
        // active with the VSID. One exception to this case is DSDA
        // case(force_inactive_state will be true) where we have to set audio
        // state as inactive even though currently call active(hold).
        if ((call_state == qcril::interfaces::AudioCallState::INACTIVE) &&
                (!force_inactive_state && qcril_am_is_any_non_inactive_calls_with_vsid(audio_vsid)))
        {
          QCRIL_LOG_INFO("Active calls with VSID %d in progress, do not set INACTIVE",
                         audio_vsid);
          err = RIL_E_CANCELLED;
          break;
        }

        call_state  = qcril_am_get_audio_call_state(call_state);
    } while (FALSE);

    if (RIL_E_SUCCESS == err )
    {
        std::vector<qcril::interfaces::QcRilSetAudioParams>
                audio_params{qcril::interfaces::QcRilSetAudioParams(audio_vsid, call_state,
                        call_mode, is_crs_call)};
        std::shared_ptr<RIL_Errno> respPtr {nullptr};

        //stable AIDL uses an async interface

        auto setParamMsg = std::make_shared<QcRilSetAudioParamMessage>(audio_params);
        if (setParamMsg != nullptr) {
            GenericCallback<RIL_Errno> cb([](std::shared_ptr<Message> /*msg*/,
                Message::Callback::Status status, std::shared_ptr<RIL_Errno> respPtr) {
                if((status == Message::Callback::Status::SUCCESS) && (respPtr != nullptr)) {
                    QCRIL_LOG_INFO("Telephony Audio Manager::setParameters return status: %d", *respPtr);
                } else {
                    QCRIL_LOG_INFO("QcRilSetAudioParamMessage : Failed response or resp NULL");
                }
            });
            setParamMsg->setCallback(&cb);
            setParamMsg->dispatch();
        }

        // HIDL service uses a sync interface

        auto setParamSyncMsg = std::make_shared<QcRilSetAudioParamSyncMessage>(audio_params);
        if (setParamSyncMsg != nullptr){
            auto ret = setParamSyncMsg->dispatchSync(respPtr);
            if((ret == Message::Callback::Status::SUCCESS) && (respPtr != nullptr)) {
                QCRIL_LOG_INFO("AudioSystem::setParameters return status: %d", *respPtr);
            }
            else {
                QCRIL_LOG_INFO("setParameters returned %d", ret);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_state_reset
***************************************************************************************************/
void qcril_am_state_reset()
{
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_lock();
    am_state.ims_on_wlan = FALSE;
    am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;
    qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_handle_event
***************************************************************************************************/
RIL_Errno qcril_am_handle_event(qcril_am_event_type event, const void* data)
{
    RIL_Errno err = RIL_E_SUCCESS;

    const char *EVT_LOG_STR[] = { "EVENT_MIN",
                                  "IMS_ANSWER",
                                  "IMS_ANSWER_FAIL",
                                  "VOICE_ANSWER",
                                  "VOICE_ANSWER_FAIL",
                                  "SWITCH_CALL",
                                  "SWITCH_CALL_FAIL",
                                  "CALL_STATE_CHANGED",
                                  "SRVCC_START",
                                  "SRVCC_COMPLETE",
                                  "SRVCC_FAIL",
                                  "SRVCC_CANCEL",
                                  "DRVCC_START",
                                  "DRVCC_COMPLETE",
                                  "DRVCC_FAIL",
                                  "DRVCC_CANCEL",
                                  "IMS_SRV_CHANGED",
                                  "IMS_HANDOVER",
                                  "AUDIO_RAT_CHANGED",
                                  "LCH",
                                  "UNLCH",
                                  "INTER_RIL_CALL_STATE",
                                  "MEDIA_SERVER_DIED",
                                  "EVENT_MAX" };

    if ( QCRIL_AM_EVENT_MIN <= event && event < sizeof(EVT_LOG_STR)/sizeof(EVT_LOG_STR[0]) )
    {
        QCRIL_LOG_INFO("processing event: %s", EVT_LOG_STR[event]);
    }
    else
    {
        QCRIL_LOG_INFO("processing UNKNOWN event: %d", event);
    }

    switch (event)
    {
    case QCRIL_AM_EVENT_IMS_ANSWER:
        if(am_state.ims_on_wlan)
        {
          QCRIL_LOG_INFO( "Answer IMS call on WLAN");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                    qcril::interfaces::AudioCallState::ACTIVE,
                                    qcril_qmi_voice_get_answer_call_mode());
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
        }
        else
        {
          QCRIL_LOG_INFO( "Answer IMS call");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                    qcril::interfaces::AudioCallState::ACTIVE,
                                    qcril_qmi_voice_get_answer_call_mode());
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
        }
        break;

    case QCRIL_AM_EVENT_VOICE_ANSWER:
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                   qcril::interfaces::AudioCallState::ACTIVE,
                                   qcril_qmi_voice_get_answer_call_mode());
        break;

    case QCRIL_AM_EVENT_IMS_ANSWER_FAIL:
        err = qcril_am_set_ims_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_VOICE_ANSWER_FAIL:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_SWITCH_CALL:
    case QCRIL_AM_EVENT_SWITCH_CALL_FAIL:
    case QCRIL_AM_EVENT_CALL_STATE_CHANGED:
    case QCRIL_AM_EVENT_MEDIA_SERVER_INITIALIZED:
        // Need to set the active VSID first to ensure the existing voice
        // session to continue with the updated VSID if there is any change
        // in VSID(domain) of the call.
        if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
        {
          err = qcril_am_set_ims_call_audio_driver_by_call_state();
          err = qcril_am_set_voice_call_audio_driver_by_call_state();
        }
        else
        {
          err = qcril_am_set_voice_call_audio_driver_by_call_state();
          err = qcril_am_set_ims_call_audio_driver_by_call_state();
        }
        break;

    case QCRIL_AM_EVENT_SRVCC_START:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        break;

    case QCRIL_AM_EVENT_SRVCC_COMPLETE:
        //no action
        break;

    case QCRIL_AM_EVENT_SRVCC_FAIL:
    case QCRIL_AM_EVENT_SRVCC_CANCEL:
        err = qcril_am_set_ims_call_audio_driver_by_call_state();
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_DRVCC_START:
    case QCRIL_AM_EVENT_DRVCC_FAIL:
    case QCRIL_AM_EVENT_DRVCC_CANCEL:
        //no action
        break;

    case QCRIL_AM_EVENT_DRVCC_COMPLETE:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        //no action
        break;

    case QCRIL_AM_EVENT_IMS_HANDOVER:
        if (data) {
          qcril_am_set_ims_on_wlan_from_handover(*(imsa_service_rat_enum_v01*)data);
          // On MultiSim targets, IMS will be running on the SUB which is L capable and
          // set as DDS. Update the call states to Audio only if this SUB is IMS capable.
          if((num_of_rilds == 1) || qcril_qmi_voice_external_is_ims_available())
          {
            qcril_am_set_ims_call_audio_driver_by_call_state(TRUE);
          }
        }
        break;

    case QCRIL_AM_EVENT_AUDIO_RAT_CHANGED:
        qcril_am_handle_audio_rat_change(
                (voice_audio_rat_change_info_ind_msg_v02*)data);
        break;

    case QCRIL_AM_EVENT_RELEASE_AUDIO:
        if(am_state.ims_on_wlan)
        {
          QCRIL_LOG_INFO( "Release Audio for IMS call on WLAN");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                    qcril::interfaces::AudioCallState::INACTIVE,
                                    CALL_MODE_UNKNOWN_V02,
                                    false,
                                    true);
        }
        else
        {
          QCRIL_LOG_INFO( "Release Audio for IMS call");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                    qcril::interfaces::AudioCallState::INACTIVE,
                                    CALL_MODE_UNKNOWN_V02,
                                    false,
                                    true);
        }
        break;

    default:
        QCRIL_LOG_DEBUG("ignore unexpected event");
        err = RIL_E_REQUEST_NOT_SUPPORTED;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_set_vsid
***************************************************************************************************/
RIL_Errno qcril_am_set_vsid(qcril_am_vs_type vs_type, uint32_t vsid)
{
    QCRIL_LOG_INFO("set vs_type: %d to vsid: %u", vs_type, vsid);
    RIL_Errno err = RIL_E_SUCCESS;

    switch (vs_type)
    {
    case QCRIL_AM_VS_VOICE:
        qcril_am_lock();
        am_state.voice_vsid.is_valid = TRUE;
        am_state.voice_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS:
        qcril_am_lock();
        am_state.lte_vsid.is_valid = TRUE;
        am_state.lte_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS_WLAN:
        qcril_am_lock();
        am_state.wlan_vsid.is_valid = TRUE;
        am_state.wlan_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    default:
        err = RIL_E_REQUEST_NOT_SUPPORTED;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}

/* API for testing only */
boolean qcril_am_get_ims_on_wlan()
{
  qcril_am_lock();
  boolean ims_on_wlan = am_state.ims_on_wlan;
  qcril_am_unlock();
  return ims_on_wlan;
}

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_qcril_am_cleanup()
{
  memset(&am_state, 0, sizeof(am_state));
}
#endif
