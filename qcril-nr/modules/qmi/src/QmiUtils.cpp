
/******************************************************************************
#  Copyright (c) 2014, 2017, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    QmiUtils.cpp
  @brief   Provides interface to functions where conditional platform
           dependency is there.

******************************************************************************/
#include <sys/ioctl.h>

#define TAG "RILQ"

#include "framework/Log.h"
#include "framework/TimeKeeper.h"
#include "qcrili.h"
#include "qcril_qmi_client.h"
#include "qcril_memory_management.h"
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
#include "sys_health_mon.h"
#endif
#endif


typedef struct {
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
    TimeKeeper::timer_id timer_id;
#endif
#endif
    qmi_client_recv_msg_async_cb resp_cb;
    void    *cb_data;
} async_data_type;

//===========================================================================
// generic_async_qmi_cb
//===========================================================================
static void generic_async_qmi_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                         *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                         *resp_cb_data,
  qmi_client_error_type        transp_err
)
{
  async_data_type *generic_cb_data;

  generic_cb_data = (async_data_type *)resp_cb_data;
  /* cancel the timer */
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
  if (generic_cb_data->timer_id != TimeKeeper::no_timer) {
      TimeKeeper::getInstance ().clear_timer(generic_cb_data->timer_id);
      generic_cb_data->timer_id = TimeKeeper::no_timer;
  }
#endif
#endif
  /* call the user-set callback func with user-set cb_data */
  generic_cb_data->resp_cb( user_handle,
                            msg_id,
                            resp_c_struct,
                            resp_c_struct_len,
                            generic_cb_data->cb_data,
                            transp_err );

  qcril_free(generic_cb_data);
}

/*
 * This function is to send qmi async message with
 * system health monitor check
 */
qmi_client_error_type
qmi_client_send_msg_async_with_shm
(
    qmi_client_type                 user_handle,
    unsigned long                   msg_id,
    void                            *req_c_struct,
    int                             req_c_struct_len,
    void                            *resp_c_struct,
    int                             resp_c_struct_len,
    qmi_client_recv_msg_async_cb    resp_cb,
    void                            *resp_cb_data,
    qmi_txn_handle                  *txn_handle
)
{
    async_data_type *generic_cb_data;
    qmi_client_error_type rc;

    QCRIL_LOG_ESSENTIAL("msgId = %d", msg_id);
    /* build generic_cb_data */
    generic_cb_data = qcril_malloc2(generic_cb_data);
    if (generic_cb_data == NULL)
    {
        QCRIL_LOG_ERROR("Failed to allocate generic cb data");
        return QMI_INTERNAL_ERR;
    }
    generic_cb_data->resp_cb = resp_cb;
    generic_cb_data->cb_data = resp_cb_data;
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
    generic_cb_data->timer_id = TimeKeeper::no_timer;

    auto timer_id = TimeKeeper::getInstance().set_timer(
            [generic_cb_data](void*) {
                int ret;
                generic_cb_data->timer_id = TimeKeeper::no_timer;
                QCRIL_LOG_ESSENTIAL("send msg async timeout");
                /* since async callback does not come in time, check system health */
                ret = check_system_health();
                if (ret < 0) {
                    QCRIL_LOG_ESSENTIAL("QCRIL_WARNING:IO: Check system health returned %d", ret);
                }
            },
            nullptr,
            12000);
    generic_cb_data->timer_id = timer_id;
#endif
#endif
    /* call the qmi async send */
    rc = qmi_client_send_msg_async( user_handle,
                                    msg_id,
                                    req_c_struct,
                                    req_c_struct_len,
                                    resp_c_struct,
                                    resp_c_struct_len,
                                    generic_async_qmi_cb,
                                    generic_cb_data,
                                    txn_handle);
    if (rc != QMI_NO_ERR) {
        /* cancel the timer */
        QCRIL_LOG_ESSENTIAL("send msg async failed");
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
        if (generic_cb_data->timer_id != TimeKeeper::no_timer) {
            TimeKeeper::getInstance ().clear_timer(generic_cb_data->timer_id);
            generic_cb_data->timer_id = TimeKeeper::no_timer;
        }
#endif
#endif
        qcril_free(generic_cb_data);
    }

    return rc;
}

/*
 * This function is to send qmi sync message with
 * system health monitor check
 */
qmi_client_error_type
qmi_client_send_msg_sync_with_shm
(
    qmi_client_type    user_handle,
    int                msg_id,
    void               *req_c_struct,
    int                req_c_struct_len,
    void               *resp_c_struct,
    int                resp_c_struct_len,
    int                timeout_msecs
)
{
    qmi_client_error_type rc;
    int ret;
    QCRIL_LOG_ESSENTIAL("msgId = %d", msg_id);
    rc = qmi_client_send_msg_sync(user_handle,
                                  msg_id,
                                  req_c_struct,
                                  req_c_struct_len,
                                  resp_c_struct,
                                  resp_c_struct_len,
                                  timeout_msecs);
    if (rc == QMI_TIMEOUT_ERR)
    {
    QCRIL_LOG_ESSENTIAL("send msg sync timeout");

#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
        /* sync send timeout, and check system health */
        ret = check_system_health();
        if (ret < 0) {
            QCRIL_LOG_ESSENTIAL("QCRIL_WARNING:IO: Check system health returned %d", ret);
        }
#endif
#endif

    }
    return rc;
}
