/*!
  @file
  dsi_netctrl_init.c

  @brief
  implements dsi_netctrl initialization

*/

/*===========================================================================

  Copyright (c) 2010-2015, 2019, 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/10   js      created

===========================================================================*/
#include "dsi_netctrli.h"
#include "dsi_netctrl_mni.h"
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl.h"
#include "dsi_netctrl_cb_thrd.h"

#include <pthread.h>

char l2s_proc_name[50];
int  l2s_pid;

#define DSI_INIT_WAIT_TIME_BEFORE_RETRY 100000 /* usec
                                                  time interval between each
                                                  NETLINK req event sent from
                                                  dsi_netctrl
                                                */
#define DSI_INIT_MAX_RETRY_COUNT 36000 /* max number of retrying ping msg
                                         to netmgr for its readiness */

NicmClient *nicm_clnt_hndl = NULL;
int is_nicm_client_init = DSI_FALSE;

/*===========================================================================
  FUNCTION:  dsiNicmCb
===========================================================================*/
/*!
    @brief
    Callback function registered with NICM

    @return
    void
*/
/*=========================================================================*/
void dsiNicmCb(void *info, void *userdata)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t *cmd_buf = NULL;

  DSI_LOG_VERBOSE("%s", ">>>dsi_nicm_cb ENTRY");

  do
  {
    ret = DSI_ERROR;

    if (NULL == info)
    {
      DSI_LOG_FATAL("%s", "*** NULL info rcvd ***");
      break;
    }


    cmd_buf = (dsi_netctrl_cb_cmd_t *) dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
      break;
    }

    /* Set internal structures, Set Opaque pointer towards dsi */
    cmd_buf->cmd_data.data_union.nicm_cb_info.eventInfo = setNicmInfo(info);
    cmd_buf->cmd_data.data_union.nicm_cb_info.data = userdata;

    if(!(cmd_buf->cmd_data.data_union.nicm_cb_info.eventInfo))
    {
      DSI_LOG_ERROR("%s(): failed to set pointer!", __func__);
      free(cmd_buf);
      return;
    }

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsiProcessNicmEvents;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue", &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_ERROR == ret)
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_nicm_cb EXIT with err");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_nicm_cb EXIT with suc");
  }

  return;
}

/*===========================================================================
FUNCTION:  dsiInitNicmClient
===========================================================================*/
/*!
  @brief
  Initializes NICM client

  @return
  DSI_SUCCESS
  DSI_ERROR
  */
/*=========================================================================*/
int dsiInitNicmClient(void)
{
    DSI_LOG_DEBUG("%s(): Initializing NICM client for DSI", __func__);
    if (DSI_FALSE == is_nicm_client_init)
    {
        nicm_clnt_hndl = nicm_client_register(dsiNicmCb, NULL);
        if (NULL == nicm_clnt_hndl)
        {
            DSI_LOG_ERROR("%s(): failed to register client!", __func__);
            return DSI_ERROR;
        }

        is_nicm_client_init = TRUE;
    }

    return DSI_SUCCESS;
}

/*===========================================================================
FUNCTION: dsiDeInitNicmClient

===========================================================================*/
/*!
@brief
Releases NICM client

@return
DSI_SUCCESS
DSI_ERROR

=========================================================================*/
int dsiDeInitNicmClient(void)
{
  DSI_LOG_DEBUG("%s(): de-initializing NICM client for DSI", __func__);
  if (DSI_TRUE == is_nicm_client_init)
  {
    nicm_client_release(nicm_clnt_hndl);
    nicm_clnt_hndl = NULL;
    is_nicm_client_init = FALSE;
  }

  return DSI_SUCCESS;
}
/*===========================================================================
FUNCTION:  dsiInitQueryNicm
===========================================================================*/
/*!
@brief
Seperate thread to query nw mgr for readiness status. It updates
    dsi_inited accordingly.
    Repeat query periodically if dsi_inited is not TRUE

    @return
    None
*/
/*=========================================================================*/
void dsiInitQueryNicm(void)
{
  unsigned int i = 0;

  /* dsi_inited will be updated once resp nl msg is received */
  while(dsi_inited != DSI_TRUE &&     /* check if dsi_inited has been updated */
        dsi_released != DSI_TRUE &&   /* if dsi_release has been called we need
                                         to exit this thread*/
        i < DSI_INIT_MAX_RETRY_COUNT )/* stop retrying after reaching max trial number */
  {
    i++;
    DSI_LOG_ERROR("dsi_init_query_nicm %d time", i);
    if( NICM_SUCCESS == dsiInitQueryNicmWrapper(nicm_clnt_hndl))
    {
      DSI_LOG_DEBUG("%s", "update dsi_inited to TRUE\n");
      set_dsi_init_state(DSI_TRUE);
    }
    else if (dsi_released == TRUE)
    {
      break;
    }
    else
    {
      usleep(DSI_INIT_WAIT_TIME_BEFORE_RETRY);
    }
  }
  DSI_LOG_DEBUG("%s: exit ping thread",__func__);

}

/*===========================================================================
  FUNCTION:  dsi_ping_thread
===========================================================================*/
/*!
    @brief
    used to start thread for query netmgr readiness status.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_init_ping_thread(void)
{
  int rc = DSI_ERROR;

  DSI_LOCK_MUTEX(&dsi_ping_tid_mutex);
  dsi_ping_tid = 0;

  if(pthread_create(&dsi_ping_tid, NULL, (void *) &dsiInitQueryNicm, NULL))
  {
    DSI_LOG_ERROR("%s", "failed to create dsi_ping_thread\n");
    goto bail;
  }

  DSI_LOG_DEBUG("%s: dsi_ping_tid [%lu]", __func__, (long unsigned int) dsi_ping_tid);
  rc = DSI_SUCCESS;

bail:
  DSI_UNLOCK_MUTEX(&dsi_ping_tid_mutex);
  return rc;
}

/* once test mode is supported, populate this vtbl
   with test functions */
dsi_mni_vtable_t dsi_netctrl_mni_vtbl;

static int __dsi_init_ex(void *mode_ptr)
{
  int mode = 0;
  int ret = DSI_SUCCESS;
  int rc = DSI_ERROR;

  /* most likely static analysis checkers will complain if this check is not present */
  if (!mode_ptr)
  {
    DSI_LOG_ERROR("%s(): mode_ptr is NULL, using DSI_MODE_GENERAL", __func__);
  }
  else
  {
    mode = *((int *)mode_ptr);
    free(mode_ptr);
  }

  DSI_GLOBAL_LOCK;

  if (dsi_inited == DSI_TRUE)
  {
    DSI_LOG_ERROR("%s(): dsi already inited", __func__);
    DSI_GLOBAL_UNLOCK;
    return DSI_EINITED;
  }
  else if (dsi_inited == DSI_IN_PROGRESS)
  {
    DSI_LOG_ERROR("%s(): dsi init in progress", __func__);
    DSI_GLOBAL_UNLOCK;
    return DSI_EINTERNAL;
  }

  set_dsi_init_state(DSI_IN_PROGRESS);

  DSI_L2S_SETUP();
  DSI_L2S_ENTRY();
  DSI_LOG_DEBUG("%s(): ENTRY", __func__);
  DSI_LOG_DEBUG("%s(): RIL Instance %d", __func__, dsi_get_ril_instance());

  /* no more do-while(0) */
  switch(mode)
  {
  case DSI_MODE_TEST:
    DSI_LOG_ERROR("%s(): not supported test mode", __func__);
    break;
  case DSI_MODE_GENERAL:
    DSI_LOG_DEBUG("%s(): initializing dsi_netctrl in general mode", __func__);
    /* if no vtbl is provided, it's inited in general mode
     * by default */
    if (DSI_SUCCESS != dsi_init_internal(NULL, FALSE, 1))
    {
      ret = DSI_ERROR;
      DSI_LOG_ERROR("%s(): dsi_init_internal failed", __func__);
    }

    rc = dsiInitNicmClient();

    if (DSI_SUCCESS != rc)
    {
      ret = DSI_ERROR;
      DSI_LOG_ERROR("%s(): dsi init client failed", __func__);
    }
    break;
  case DSI_MODE_SSR:
    DSI_LOG_DEBUG("%s(): initializing dsi_netctrl in SSR mode", __func__);
    if (DSI_SUCCESS != dsi_init_internal(NULL, TRUE, 1))
    {
      ret = DSI_ERROR;
      DSI_LOG_ERROR("%s(): dsi_init_internal failed", __func__);
    }

    rc = dsiInitNicmClient();

    if (DSI_SUCCESS != rc)
    {
      ret = DSI_ERROR;
      DSI_LOG_ERROR("%s(): dsi init client failed", __func__);
    }
    break;
  default:
    ret = DSI_ERROR;
    DSI_LOG_ERROR("%s(): not supported default mode", __func__);
    break;
  }

  /* start the thread for send/recv netmgr ready state */
  if (DSI_SUCCESS == ret)
  {
    dsi_released = DSI_FALSE;
    ret = dsi_init_ping_thread();
  }

  if (ret == DSI_SUCCESS)
  {
    DSI_LOG_DEBUG("%s(): EXIT with success", __func__);
  }
  else
  {
    DSI_LOG_DEBUG("%s(): EXIT with error", __func__);
    /* if there is some true internal error, clients have no way to recover
     * have to force a restart
     */
    if (!dsi_mni_get_dsi_release_invoked())
    {
      DSI_LOG_DEBUG("%s(): initialization encountered errors", __func__);
      abort();
    }
  }

  DSI_GLOBAL_UNLOCK;

  DSI_L2S_EXIT_WITH_STATUS();
  /* no one cares about ret anyway */
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_init
===========================================================================*/
/*!
    @brief
    used to initialize dsi netctrl module.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_init(int mode)
{
  if(dsi_inited == DSI_TRUE)
  {
    DSI_LOG_ERROR("%s","dsi_init: dsi already inited");
    return DSI_EINITED;
  }

  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  int rc;

  DSI_GLOBAL_LOCK;

  DSI_L2S_SETUP();
  DSI_L2S_ENTRY();
  DSI_LOG_DEBUG( "%s", "dsi_init: ENTRY" );
  DSI_LOG_DEBUG( "dsi_init: RIL instance %d", dsi_get_ril_instance() );

    /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    reti = DSI_SUCCESS;
    switch(mode)
    {
    case DSI_MODE_TEST:
      DSI_LOG_ERROR("%s","not supported test mode");
      break;
    case DSI_MODE_GENERAL:
      DSI_LOG_DEBUG("%s","initializing dsi_netctrl in general mode");
      /* if no vtbl is provided, it's inited in general mode
         by default */
      if (DSI_SUCCESS != dsi_init_internal(NULL, FALSE, 0))
      {
        reti = DSI_ERROR;
        DSI_LOG_ERROR("%s","dsi_init_internal failed");
      }

      rc = dsiInitNicmClient();

      if (DSI_SUCCESS != rc)
      {
        reti = DSI_ERROR;
        DSI_LOG_ERROR("%s","dsi init client failed");
      }
      break;
    case DSI_MODE_SSR:
      DSI_LOG_DEBUG("%s","initializing dsi_netctrl in SSR mode");
      if (DSI_SUCCESS != dsi_init_internal(NULL, TRUE, 0))
      {
        reti = DSI_ERROR;
	DSI_LOG_ERROR("%s","dsi_init_internal failed");
      }

      rc = dsiInitNicmClient();

      if (DSI_SUCCESS != rc)
      {
        reti = DSI_ERROR;
        DSI_LOG_ERROR("%s","dsi init client failed");
      }
      break;
    default:
      reti = DSI_ERROR;
      DSI_LOG_ERROR("%s","not supported default mode");
      break;
    }
    if (reti == DSI_ERROR)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  /* start the thread for send/recv netmgr ready state */
  if (DSI_SUCCESS == ret)
  {
    dsi_released = DSI_FALSE;
    ret = dsi_init_ping_thread();
  }

  if (ret == DSI_SUCCESS)
  {
    DSI_LOG_DEBUG( "%s", "dsi_init: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_init: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  DSI_L2S_EXIT_WITH_STATUS();
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_init_ex2
===========================================================================*/
/*!
    @brief
    Dummy dsi_init_ex2 function.
*/
/*=========================================================================*/
extern int dsi_init_ex2
(
  dsi_init_mode_t      mode,
  dsi_init_cb_func_ex2 cb_func,
  void                 *dsi_init_cb_data
)
{
  (void) mode;
  (void) cb_func;
  (void) dsi_init_cb_data;
  return DSI_ERROR;
}

/*===========================================================================
  FUNCTION:  dsi_release_ex
===========================================================================*/
/*!
    @brief
    Dummy dsi_release_ex function.
*/
/*=========================================================================*/
extern int dsi_release_ex(dsi_init_mode_t mode)
{
  (void) mode;
  return DSI_ERROR;
}

/*===========================================================================
  FUNCTION:  dsi_get_data_srvc_hndl_ex
===========================================================================*/
/*!
    @brief
    Dummy dsi_get_data_srvc_hndl_ex.
*/
/*=========================================================================*/
extern dsi_hndl_t dsi_get_data_srvc_hndl_ex
(
  dsi_net_ev_cb_ex user_cb_fn,
  dsi_init_mode_t  mode,
  void             *user_data
)
{
  (void) user_cb_fn;
  (void) mode;
  (void) user_data;
  return NULL;
}

/*===========================================================================
  FUNCTION: dsi_rel_data_srvc_hndl_ex
===========================================================================*/
/*!
    @brief
    Dummy dsi_rel_data_srvc_hndl_ex
*/
/*=========================================================================*/
extern void dsi_rel_data_srvc_hndl_ex(dsi_hndl_t hndl)
{
  (void) hndl;
}
/*===========================================================================
  FUNCTION:  dsi_start_data_call_ex
===========================================================================*/
/*!
    @brief
    Dummy dsi_start_data_call_ex.
*/
/*=========================================================================*/
extern int dsi_start_data_call_ex
(
  dsi_hndl_t             hndl,
  int                    num_params,  /* Number of params */
  dsi_call_param_info_t  *params      /* Array of call params */
)
{
  (void) hndl;
  (void) num_params;
  (void) params;
  return DSI_ERROR;
}

/*===========================================================================
  FUNCTION: dsi_get_ip_version
===========================================================================*/
/*!
    @brief
    Dummy dsi_get_ip_version.
*/
/*=========================================================================*/
int dsi_get_ip_version(dsi_hndl_t hndl)
{
  (void) hndl;
  return DSI_ERROR;
}

/*===========================================================================
  FUNCTION:  dsi_init_ex
===========================================================================*/
/*!
    @brief
    used to initialize dsi netctrl module and executing the callback, given as an argument.

    param [in] init mode
    param [in] dsi_init_cb_func
    param [in] dsi_init_cb_data

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_init_ex
(
  int mode,
  void (* dsi_init_cb_func)( void * ),
  void *dsi_init_cb_data

)
{
  pthread_t dsi_init_ex_pt;
  int *mode_ptr = NULL;
  pthread_attr_t dsi_init_ex_pt_a;

  DSI_LOG_DEBUG("%s(): ENTRY", __func__);

  /* clear state for new intialization */
  dsi_mni_set_dsi_release_invoked(DSI_MNI_SET_RELEASE_CLEARED);

  mode_ptr = calloc(1, sizeof(*mode_ptr));
  if (!mode_ptr)
  {
    DSI_LOG_ERROR("%s(): Unable to allocate mode_ptr", __func__);
    abort();
  }

  /* this seems to be needed to avoid pthread_join which would block here
   * failure to do that could cause zombie threads
   */
  if (pthread_attr_init(&dsi_init_ex_pt_a))
  {
    DSI_LOG_ERROR("%s(): Unable to init __dsi_init_ex thread attribute", __func__);
    abort();
  }

  if (pthread_attr_setdetachstate(&dsi_init_ex_pt_a, PTHREAD_CREATE_DETACHED))
  {
    DSI_LOG_ERROR("%s(): Unable to set __dsi_init_ex thread as PTHREAD_CREATE_DETACHED",
                  __func__);
    abort();
  }

  dsi_init_cb_info.cb_func = dsi_init_cb_func;
  dsi_init_cb_info.cb_data = dsi_init_cb_data;

  *mode_ptr = mode;

  /* wonder if dsi_init_ex_pt_a needs to be from heap
   * address of mode cannot be passed as an argument to pthread_create as dsi_init_ex could
   * have finished executing when __dsi_init_ex is invoked and the memory of mode in stack
   * would be out of scope
   */
  if (pthread_create(&dsi_init_ex_pt, &dsi_init_ex_pt_a, (void *)__dsi_init_ex, (void *)mode_ptr) != 0)
  {
    DSI_LOG_ERROR("%s(): Unable to create __dsi_init_ex thread", __func__);
    abort();
  }
  DSI_LOG_DEBUG("%s(): Created __dsi_init_ex thread", __func__);

  DSI_LOG_DEBUG("%s(): EXIT", __func__);

  return DSI_SUCCESS;
}

/*===========================================================================
  FUNCTION:  dsi_release
===========================================================================*/
/** @ingroup dsi_release

    Clean-up the DSI_NetCtrl library.

    @return
    DSI_SUCCESS -- Cleanup was successful. \n
    DSI_ERROR -- Cleanup failed.

    @dependencies
    None.
*/
/*=========================================================================*/
int dsi_release(int mode)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;

 if(dsi_released == DSI_TRUE)
 {
   DSI_LOG_ERROR("dsi_released: dsi already released.");
   return DSI_ERELEASED;
 }

  /* if __dsi_init_ex thread is still executing, it means that DSI_GLOBAL_LOCK is already
   * acquired there. Set the release state prior to getting the lock.
   */
  dsi_mni_set_dsi_release_invoked(DSI_MNI_SET_RELEASE_INVOKED);

  /* this will block till qmi times out and the __dsi_init_ex thread realizes that
   * dsi_release has been called
   */
  DSI_GLOBAL_LOCK;

  DSI_L2S_ENTRY();
  DSI_LOG_DEBUG( "dsi_release: ENTRY" );
  DSI_LOG_DEBUG( "dsi_release: RIL instance %d", dsi_get_ril_instance() );

    /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;
    dsi_released = DSI_TRUE;
    /* Join ping thread when we call dsi_release */
    dsi_netctrl_join_ping_thread();

    reti = DSI_SUCCESS;
    switch(mode)
    {
    case DSI_MODE_TEST:
      DSI_LOG_ERROR("not supported test mode");
      break;
    case DSI_MODE_GENERAL:
      DSI_LOG_DEBUG("releasing dsi_netctrl in general mode");

      dsi_release_internal();
      break;
    default:
      reti = DSI_ERROR;
      DSI_LOG_ERROR("not supported default mode");
      break;
    }
    if (reti == DSI_ERROR)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_SUCCESS)
  {
    dsi_inited = DSI_FALSE;
    DSI_LOG_DEBUG( "dsi_release: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "dsi_release: EXIT with err" );
  }

  DSI_L2S_EXIT_WITH_STATUS();

  DSI_GLOBAL_UNLOCK;

  return ret;
}

