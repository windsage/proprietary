/******************************************************************************
#  Copyright (c) 2009-2013, 2017, 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_other.c
  @brief   qcril qmi - misc

  DESCRIPTION
    Handles RIL requests for common software functions an any other
    RIL function that doesn't fall in a different (more specific) category

******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <cutils/properties.h>
#include <cutils/memory.h>
#include <stdarg.h>
#include <simple_list.h>
#ifdef __ANDROID__
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

#ifdef QMI_RIL_UTF
#include <errno.h>
#endif

#include "qcrili.h"
#include "qcril_reqlist.h"
#include "qcril_otheri.h"
#include "qcril_other.h"
#include "modules/nas/qcril_nas_legacy.h"
#include "voice_service_v02.h"
#include "network_access_service_v01.h"
#include "qcril_qmi_client.h"
#include "ip_multimedia_subsystem_presence_v01.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"
#include "request/GoDormantRequestMessage.h"
#include "modules/qmi/QmiStruct.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/voice/VoiceGetConfigMessage.h"
#include "modules/voice/VoiceSetConfigMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"

#include "framework/Log.h"

#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_oemhook_utils.h"
#include "qcril_qmi_err_utils.h"
#include "qcril_legacy_apis.h"

#define TAG "QCRIL_OTHER"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void qcril_other_request_oem_hook_neighboring_cells_info_cb
(
  unsigned int          msg_id,
  std::shared_ptr<void> resp_c_struct,
  unsigned int          resp_c_struct_len,
  void                  *resp_cb_data,
  qmi_client_error_type transp_err
);

void qmi_ril_get_property_value_helper(const char *property_name,
                                       char *property_value,
                                       const char *default_property_value);

RIL_Errno qmi_ril_set_property_value_helper(const char *property_name,
                                                  const char *property_value);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_other_ascii_to_int

===========================================================================*/
/*!
    @brief
    Convert a non NULL terminated string to an integer

    @return
    the integer value of the string.
*/
/*=========================================================================*/
int qcril_other_ascii_to_int(const char* str, int size)
{
  int ret=0;
  char *tmp = (char*)qcril_malloc(size+1);
  if(tmp != NULL)
  {
  memcpy(tmp, str, size);
  tmp[size] = 0;
  ret = atoi(tmp);
  qcril_free(tmp);
  }
  else
  {
      ret = 0;
      QCRIL_LOG_FATAL("CHECK FAILED");
  }
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_other_int_to_ascii

===========================================================================*/
/*!
    @brief
    Convert an integer value to a non NULL terminated string

    @return
    None.
*/
/*=========================================================================*/
void qcril_other_int_to_ascii(char* str, int size, int value)
{
  int i;
  for (i=size-1; i>=0; i--)
  {
    str[i] = value % 10 + '0';
    value /= 10;
  }
}

/*===========================================================================

  FUNCTION:  qcril_other_hex_to_int

===========================================================================*/
/*!
    @brief
    Convert a hexadecimal string to integer

    @return
    None.
*/
/*=========================================================================*/
int qcril_other_hex_to_int(char *hex_string,int *number)
{
    int iter_i=0;
    int temp_digit=0;
    int err=0;
    int len=0;

    if(hex_string && number)
    {
        len = strlen(hex_string);
        *number = 0;
        for(iter_i = 0; iter_i < len; iter_i++)
        {
            if( isdigit(hex_string[iter_i]) )
            {
                temp_digit = hex_string[iter_i]-'0';
            }
            else
            {
                switch( hex_string[iter_i] )
                {
                case 'a':
                case 'A':
                    temp_digit = 10;
                    break;
                case 'b':
                case 'B':
                    temp_digit = 11;
                    break;
                case 'c':
                case 'C':
                    temp_digit = 12;
                    break;
                case 'd':
                case 'D':
                    temp_digit = 13;
                    break;
                case 'e':
                case 'E':
                    temp_digit = 14;
                    break;
                case 'f':
                case 'F':
                    temp_digit = 15;
                    break;
                default:
                    QCRIL_LOG_INFO("Invalid hex character %d", hex_string[iter_i]);
                    err = -1;
                    break;
                }
            }
            if(0 != err)
            {
                *number = 0;
                break;
            }
            *number <<= 4;
            *number += temp_digit;
        }
    }
    else
    {
        QCRIL_LOG_INFO("Null Pointer");
        err = -1;
    }

    return err;
}

/*===========================================================================

  FUNCTION:  qcril_other_is_number_found

===========================================================================*/
/*!
    @brief
    Checks If number is found in the patterns. ',' is the delimiter of the patterns.
    '\0' and ':' can be used to terminate the patterns.


    @return
    Returns TRUE if number is found in the patterns
*/
/*=========================================================================*/
int qcril_other_is_number_found(const char * number, const char *patterns)
{
  int res = FALSE;

  char single_num[ PROPERTY_VALUE_MAX ];

  const char * cur_p;
  char * cur_n;
  char cur_c;

  cur_p = patterns;
  cur_n = single_num;
  do
  {
    cur_c = *cur_p;

    switch ( cur_c )
    {
      case ',':
        cur_p++;    // fallthrourg
        [[clang::fallthrough]];

      case '\0':
      case ':' :
        *cur_n = 0;
        if ( 0 == strcmp( single_num, number ) )
        {
          res = TRUE;
        }
        else
        {
          cur_n = single_num;
        }
        break;

      default:
        *cur_n = cur_c;
        cur_n++;
        cur_p++;
        break;
    }

  } while ( !res && cur_c != '\0' && cur_c != ':' );

  QCRIL_LOG_INFO("qcril_other_is_number_found for %s completed with %d", number, (int) res);

  return res;
} // qcril_other_is_number_found

/*===========================================================================

  FUNCTION:  qcril_other_init

===========================================================================*/
/*!
    @brief
    Initialize the Other subsystem of the RIL.

    @return
    None.
*/
/*=========================================================================*/
void qcril_other_init( void )
{
   return;
} /* qcril_other_init() */


/*===========================================================================

  FUNCTION:  qcril_other_request_oem_hook_strings

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_OEM_HOOK_STRINGS.

    @return
    None.
*/
/*=========================================================================*/
void qcril_other_request_oem_hook_strings
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qcril_instance_id_e_type instance_id;
  qcril_request_resp_params_type resp;

  /*-----------------------------------------------------------------------*/


  instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  QCRIL_NOTUSED( ret_ptr );

  /*-----------------------------------------------------------------------*/

  qcril_default_request_resp_params( instance_id, params_ptr->t, params_ptr->event_id, RIL_E_REQUEST_NOT_SUPPORTED, &resp );
  qcril_send_request_response( & resp );

} /* qcril_other_request_oem_hook_strings() */

/*===========================================================================
  qcril_other_request_oem_hook_go_dormant
  =========================================================================*/
void qcril_other_request_oem_hook_go_dormant
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type *resp = NULL;

  resp = (qcril_request_resp_params_type *) malloc (sizeof(qcril_request_resp_params_type));
  if (resp == nullptr) {
    return;
  }

  memset(resp, 0, sizeof(qcril_request_resp_params_type));
  resp->instance_id = params_ptr->instance_id;
  resp->t = params_ptr->t;
  resp->request_id = params_ptr->event_id;
  resp->request_id_android = params_ptr->event_id_android;
  resp->ril_err_no = RIL_E_GENERIC_FAILURE;

#if !defined(QMI_RIL_UTF) && !defined(RIL_FOR_MDM_DISABLE_DATA)
  std::string devName = std::string ((char*) params_ptr->data, params_ptr->datalen);
  QCRIL_LOG_INFO("goDormant devName %s", devName.c_str());
  auto msg = std::make_shared<rildata::GoDormantRequestMessage>(devName);
  if (msg) {
    GenericCallback<RIL_Errno> cb([resp](std::shared_ptr<Message>,
                                         Message::Callback::Status /*status*/,
                                         std::shared_ptr<RIL_Errno> rsp) -> void {

        if (rsp != nullptr) {
            resp->ril_err_no = *rsp;
        } else {
            QCRIL_LOG_INFO("GoDormantRequest resp is nullptr");
        }
        qcril_send_request_response(resp);
        free(resp);
     });
    msg->setCallback(&cb);
    msg->dispatch();
    ril_req_res = RIL_E_SUCCESS;
  }
#endif

  if (ril_req_res != RIL_E_SUCCESS) {
    qcril_send_request_response(resp);
    free(resp);
  }
  QCRIL_LOG_FUNC_RETURN();
}


/*=========================================================================

  FUNCTION:  qcril_other_request_oem_hook_neighboring_cells_info

===========================================================================*/
/*!
    @brief
    Retrieves the neighboring cells information

    @return
    If success then the neighboring cells information is returned
*/
/*=========================================================================*/
void qcril_other_request_oem_hook_neighboring_cells_info
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
 )
{
    qcril_instance_id_e_type instance_id;
    qcril_request_resp_params_type resp;

    RIL_Errno   ril_req_res = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type qcril_req_info_ptr;
    qmi_client_error_type qmi_client_error;

    nas_get_cell_location_info_resp_msg_v01 *qmi_response;
    qcril_other_get_cell_location_info_resp_msg_v01 *ril_resp_helper_msg = NULL;
    QCRIL_NOTUSED( ret_ptr );



    instance_id = QCRIL_DEFAULT_INSTANCE_ID;


    QCRIL_LOG_FUNC_ENTRY();

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                 QCRIL_EVT_HOOK_NEIGHBOR_CELL_INFO_RCVD,
                                 NULL,
                                 &qcril_req_info_ptr );

    if ( qcril_reqlist_new( instance_id, &qcril_req_info_ptr ) == E_SUCCESS )
    {
        ril_resp_helper_msg = (qcril_other_get_cell_location_info_resp_msg_v01*)
            qcril_malloc( sizeof( *ril_resp_helper_msg ) );

        if ( ril_resp_helper_msg )
        {
            qmi_client_error =  qmi_client_nas_send_async(QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01,
                                                            NULL,
                                                            QMI_RIL_ZERO, // empty payload
                                                            sizeof( *qmi_response ),
                                                            qcril_other_request_oem_hook_neighboring_cells_info_cb,
                                                            (void *)ril_resp_helper_msg);
            ril_req_res = ( QMI_NO_ERR == qmi_client_error ) ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE;
        }

        QCRIL_LOG_INFO( "result=%d",ril_req_res);
        if ( RIL_E_SUCCESS != ril_req_res )
        {
            qcril_default_request_resp_params( instance_id, params_ptr->t, params_ptr->event_id, ril_req_res, &resp );
            qcril_send_request_response( &resp );

            if ( ril_resp_helper_msg )
            {
                qcril_free( ril_resp_helper_msg );
            }
        }
    }
} /* qcril_other_request_oem_hook_neighboring_cells_info */


//===========================================================================
// qcril_other_request_oem_hook_neighboring_cells_info_cb
//===========================================================================
void qcril_other_request_oem_hook_neighboring_cells_info_cb
(
  unsigned int    msg_id,
  std::shared_ptr<void> resp_c_struct,
  unsigned int    resp_c_struct_len,
  void                         *resp_cb_data,
  qmi_client_error_type        transp_err
)
{
    nas_get_cell_location_info_resp_msg_v01 * qmi_response   = (nas_get_cell_location_info_resp_msg_v01 *) resp_c_struct.get();
    qcril_other_get_cell_location_info_resp_msg_v01 *    ril_resp_helper = (qcril_other_get_cell_location_info_resp_msg_v01 *) resp_cb_data;
    RIL_Errno   ril_req_res                                = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type qcril_req_info;
    errno_enum_type           found_qcril_request;
    qcril_request_resp_params_type resp;

    unsigned int iter=0,iter_j=0,iter_k=0;
    QCRIL_NOTUSED(msg_id);
    QCRIL_NOTUSED(resp_c_struct_len);

    found_qcril_request = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                  QCRIL_DEFAULT_MODEM_ID,
                                  QCRIL_EVT_HOOK_NEIGHBOR_CELL_INFO_RCVD,
                                  &qcril_req_info );

    if ( qmi_response && (E_SUCCESS == found_qcril_request) && ril_resp_helper  )
    {
        if ( QMI_NO_ERR == transp_err )
        {
            memset( ril_resp_helper, 0, sizeof(*ril_resp_helper) );
            memcpy( ril_resp_helper, qmi_response, sizeof(*ril_resp_helper));
            iter=0;
            if( ril_resp_helper->geran_info_valid && ril_resp_helper->geran_info.nmr_cell_info_len <= QCRIL_OTHER_NMR_MAX_NUM_V01)
            {
                QCRIL_LOG_INFO("GSM cells");
                for(iter_j=0; iter_j < ril_resp_helper->geran_info.nmr_cell_info_len; iter_j++,iter++)
                {
                    QCRIL_LOG_INFO("cell id %d lac %d",ril_resp_helper->geran_info.nmr_cell_info[iter_j].nmr_cell_id,ril_resp_helper->geran_info.nmr_cell_info[iter_j].nmr_lac);
                }
            }
            if( ril_resp_helper->umts_info_valid && ril_resp_helper->umts_info.umts_monitored_cell_len <= QCRIL_OTHER_UMTS_MAX_MONITORED_CELL_SET_NUM_V01)
            {
                QCRIL_LOG_INFO("UMTS cells");
                for(iter_j=0; iter_j < ril_resp_helper->umts_info.umts_monitored_cell_len; iter_j++,iter++)
                {
                    QCRIL_LOG_INFO("psc %d",ril_resp_helper->umts_info.umts_monitored_cell[iter_j].umts_psc);
                }
            }
            if( ril_resp_helper->lte_intra_valid && ril_resp_helper->lte_intra.cells_len <= QCRIL_OTHER_LTE_NGBR_NUM_CELLS_V01)
            {
                QCRIL_LOG_INFO("INTRA LTE cells");
                QCRIL_LOG_INFO("tac %d",ril_resp_helper->lte_intra.tac);
                for(iter_j=0; iter_j < ril_resp_helper->lte_intra.cells_len; iter_j++,iter++)
                {
                    QCRIL_LOG_INFO("pci %d",ril_resp_helper->lte_intra.cells[iter_j].pci);
                }
            }
            if( ril_resp_helper->lte_inter_valid && ril_resp_helper->lte_inter.freqs_len <= QCRIL_OTHER_LTE_NGBR_NUM_FREQS_V01)
            {
                QCRIL_LOG_INFO("INTER LTE cells");
                for(iter_j=0; iter_j < ril_resp_helper->lte_inter.freqs_len; iter_j++)
                {
                    if( ril_resp_helper->lte_inter.freqs[iter_j].cells_len <= QCRIL_OTHER_LTE_NGBR_NUM_CELLS_V01)
                    {
                        QCRIL_LOG_INFO("earfcn %d",ril_resp_helper->lte_inter.freqs[iter_j].earfcn);
                        for( iter_k=0; iter_k < ril_resp_helper->lte_inter.freqs[iter_j].cells_len; iter_k++,iter++ )
                        {
                            QCRIL_LOG_INFO("pci %d",ril_resp_helper->lte_inter.freqs[iter_j].cells[iter_k].pci);
                        }
                    }
                }
            }
            if( ril_resp_helper->lte_gsm_valid && ril_resp_helper->lte_gsm.freqs_len <= QCRIL_OTHER_LTE_NGBR_GSM_NUM_FREQS_V01)
            {
                QCRIL_LOG_INFO("GSM LTE cells");
                for(iter_j=0; iter_j < ril_resp_helper->lte_gsm.freqs_len; iter_j++)
                {
                    if( ril_resp_helper->lte_gsm.freqs[iter_j].cells_len <= QCRIL_OTHER_LTE_NGBR_GSM_NUM_CELLS_V01)
                    {
                        QCRIL_LOG_INFO("cell res priority %d",ril_resp_helper->lte_gsm.freqs[iter_j].cell_resel_priority);
                        for( iter_k=0; iter_k < ril_resp_helper->lte_gsm.freqs[iter_j].cells_len; iter_k++,iter++ )
                        {
                            QCRIL_LOG_INFO("bsc id %d",ril_resp_helper->lte_gsm.freqs[iter_j].cells[iter_k].bsic_id);
                        }
                    }
                }
            }
            if( ril_resp_helper->lte_wcdma_valid && ril_resp_helper->lte_wcdma.freqs_len <= QCRIL_OTHER_LTE_NGBR_WCDMA_NUM_FREQS_V01)
            {
                QCRIL_LOG_INFO("UMTS LTE cells");
                for(iter_j=0; iter_j < ril_resp_helper->lte_wcdma.freqs_len; iter_j++)
                {
                    if( ril_resp_helper->lte_wcdma.freqs[iter_j].cells_len <= QCRIL_OTHER_LTE_NGBR_WCDMA_NUM_CELLS_V01)
                    {
                        QCRIL_LOG_INFO("uarfcn %d",ril_resp_helper->lte_wcdma.freqs[iter_j].uarfcn);
                        for( iter_k=0; iter_k < ril_resp_helper->lte_wcdma.freqs[iter_j].cells_len; iter_k++,iter++ )
                        {
                            QCRIL_LOG_INFO("psc %d",ril_resp_helper->lte_wcdma.freqs[iter_j].cells[iter_k].psc);
                        }
                    }
                }
            }

            if( iter > 0 )
            {
                QCRIL_LOG_INFO("number of cells %d",iter);
                ril_req_res = RIL_E_SUCCESS;
            }
        }
        qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID, qcril_req_info.t, qcril_req_info.request, ril_req_res , &resp );
        if ( RIL_E_SUCCESS == ril_req_res )
        {
            resp.resp_pkt = (void *) ril_resp_helper;
            resp.resp_len = sizeof( *ril_resp_helper );
        }
        qcril_send_request_response( &resp );
    }

    if ( ril_resp_helper )
    {
        qcril_free( ril_resp_helper );
    }

} // qcril_other_request_oem_hook_neighboring_cells_info_cb

//===========================================================================
//qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object
//===========================================================================
qmi_idl_service_object_type qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object( qmi_ril_oem_hook_qmi_tunneling_service_id_type service_id )
{
    qmi_idl_service_object_type res = NULL;
    (void) service_id;

    switch ( service_id )
    {
    case QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_VT:
        res = ims_qmi_get_service_object_v01();
        break;

    case QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE:
        res = imsp_get_service_object_v01();
        break;

    case QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER:
        res = Qtuner_get_service_object_v01();
        break;

    default:
        res = NULL;
        break;
    }

    return res;
} // qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object

//===========================================================================
//qmi_ril_retrieve_system_time_in_ms
//===========================================================================
uint64_t qmi_ril_retrieve_system_time_in_ms()
{
    uint64_t abs_time;
    struct timespec ts;
    struct timeval tv;

    abs_time = 0;
    memset(&ts,
           0,
           sizeof(ts));
    memset(&tv,
           0,
           sizeof(tv));

    if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_POSIX_CLOCKS))
    {
        clock_gettime(CLOCK_MONOTONIC,
                      &ts);
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec/1000;
    }
    else
    {
        gettimeofday(&tv,
                     NULL);
    }
    abs_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000); //converting system time to ms

    return abs_time;
} //qmi_ril_retrieve_system_time_in_ms

struct property_info
{
    char name[PROPERTY_NAME_MAX];
    char value[PROPERTY_VALUE_MAX];
    int *updated;
};

int qcril_other_check_if_prop_updated
(
 simple_list *list,
 simple_list_iter *it,
 void *value,
 void *userdata
)
{
    qmi_ril_prop_updated_cb cb = (qmi_ril_prop_updated_cb)userdata;
    struct property_info *pi = (struct property_info *) value;
    char new_prop_val[PROPERTY_VALUE_MAX] = {0};

    if (!list || !it || !pi)
    {
        return 1;
    }
    property_get(pi->name, new_prop_val, "");
    if (cb && strncmp(pi->value, new_prop_val, PROPERTY_VALUE_MAX))
    {
        ++(*(pi->updated));
        strlcpy(pi->value, new_prop_val, sizeof(pi->value));
        cb(pi->name, sizeof(pi->name), pi->value, sizeof(pi->value));
    }
    return 0;
}

unsigned int qmi_ril_wait_for_properties(unsigned int state, qmi_ril_prop_updated_cb cb, ...)
{
    simple_list *list = simple_list_new();
    const char *prop_name = NULL;
    va_list args;
    int updated = 0;

    va_start(args, cb);
    for(; ((prop_name = va_arg(args, const char *)) != NULL);)
    {
        struct property_info *pi = (struct property_info *)calloc(1, sizeof(struct property_info));
        if (!pi) continue;
        strlcpy(pi->name, prop_name, sizeof(pi->name));
        property_get(prop_name, pi->value, "");
        pi->updated = &updated;
        simple_list_append(list, pi);
    }
    va_end(args);

    do {
#if !defined(QMI_RIL_UTF) && !defined(RIL_FOR_MDM_LE) 
       state = __system_property_wait_any(state);
#endif
        simple_list_foreach(list, qcril_other_check_if_prop_updated, (void*)cb);
        if (updated) break;
    } while(TRUE);
    return state;
}

/*
 * This function is executed in the event thread to destory one module, where it needs to wait
 * for the completion of its looper thread if it has. Currently it will be called from the msg
 * callback function of "LegacyHandlerMessage", which is handled in looper thread of the module
 * (to be destroyed).The reason why it uses the "event" thread is to avoid waiting for the thread
 * completion while itself is in the same thread, causing the "deadlock".
 */
void qmi_ril_other_cleanup_module_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{
    QCRIL_NOTUSED(ret_ptr);

    if (params_ptr && params_ptr->data) {
      Module* pModule = *((Module**)params_ptr->data);
      if (pModule)
        delete pModule;
    }
}
