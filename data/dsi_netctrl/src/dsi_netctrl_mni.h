/*!
  @file
  dsi_netctrl_mni.h

  @brief
  Exports dsi_netctrl modem network interface functions

*/

/*===========================================================================
  Copyright (c) 2010-2013, 2020-2021 Qualcomm Technologies, Inc.
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
#ifndef _DSI_NETCTRL_MNI_
#define _DSI_NETCTRL_MNI_

/* inits modem network interface(s) */
extern int dsi_mni_init(boolean from_ssr, int retry_qmi);

extern void dsi_mni_cleanup(boolean from_ssr);

/* init mni client */
int dsi_mni_init_client(int conn_id);

/* starts modem network interface */
extern int dsi_mni_start(int i, dsi_store_t * st);

/* starts extended modem network interface */
extern int dsi_mni_init_extended(dsi_store_t * st);

/* stops modem network interface */
extern int dsi_mni_stop(int i, dsi_store_t * st);

/* aborts previous start request */
extern int dsi_mni_abort_start(int i, dsi_store_t * st);

/* looks up the appropriate modem network interface */
extern int dsi_mni_look_up(dsi_store_t * st_hndl, int * pi);

/* register for handoff indications */
extern int dsi_mni_reg_unreg_handoff_ind(int pi, dsi_store_t * st_hndl, boolean reg_unreg);

/* register WDS indication */
extern int dsi_mni_register_embms_ind(int pi, dsi_store_t * st_hndl);

/* activate EMBMS TMGI */
extern int dsi_mni_embms_tmgi_activate(int pi, dsi_store_t *st_hndl);

/* deactivate EMBMS TMGI */
extern int dsi_mni_embms_tmgi_deactivate(int pi, dsi_store_t *st_hndl);

/* activate and deactivate EMBMS TMGI at the same time */
extern int dsi_mni_embms_tmgi_activate_deactivate(int pi, dsi_store_t *st_hndl);

/* query active/available EMBMS TMGI list */
extern int dsi_mni_embms_tmgi_list_query(int pi, dsi_store_t *st_hndl);

/* Content desc update EMBMS TMGI */
extern int dsi_mni_embms_tmgi_content_desc_update(int pi, dsi_store_t *st_hndl);

/* Send service interested EMBMS TMGI list */
extern int dsi_mni_embms_tmgi_svc_interest_list(int i, dsi_store_t * st_hndl);

/* can be called to release qmi client */
extern void dsi_mni_release_client(int conn_id);

/* Retrieve Tuneaway info */
extern int dsi_mni_embms_get_tuneaway_info(dsi_store_t * st_hndl, unsigned int txn_id);

/* Callback to client when iface is up/down and wds is ready */
void dsi_oemproxy_common_ind(dsi_net_evt_t ind, int iface_id);

/* states required when using dsi_init_ex and dsi_release */
#define DSI_MNI_SET_RELEASE_INVOKED (1)
#define DSI_MNI_SET_RELEASE_CLEARED (0)

void dsi_mni_set_dsi_release_invoked(int val);

int dsi_mni_get_dsi_release_invoked(void);

/* Function to check for matching calls */
extern int dsi_mni_check_matching_call(dsi_store_t *st,
                                       dsi_matching_call_check_t *call_params,
                                       char *device_name,
                                       int device_name_len);
#endif /* _DSI_NETCTRL_MNI_ */
