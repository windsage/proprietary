/*!
  @file
  dsi_netctrl_nicm_helper.c

  @brief
  This file implements routines to interact with nicmd

*/

/*===========================================================================

  Copyright (c) 2010-2015, 2017, 2019-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/restricted/linux/android/ril/qcril_dsi.c $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/10   js      created file

===========================================================================*/

#include "dsi_netctrli.h"
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_mni.h"


dsiNicmLinkMap dsiNicmLinkMapTbl[DSI_MAX_IFACES] =
{
  { NICM_LINK_RMNET_0,   0 },
  { NICM_LINK_RMNET_1,   1 },
  { NICM_LINK_RMNET_2,   2 },
  { NICM_LINK_RMNET_3,   3 },
  { NICM_LINK_RMNET_4,   4 },
  { NICM_LINK_RMNET_5,   5 },
  { NICM_LINK_RMNET_6,   6 },
  { NICM_LINK_RMNET_7,   7 },
  { NICM_LINK_RMNET_8,   8 },
  { NICM_LINK_RMNET_9,   9 },
  { NICM_LINK_RMNET_10, 10 },
  { NICM_LINK_RMNET_11, 11 },
  { NICM_LINK_RMNET_12, 12 },
  { NICM_LINK_RMNET_13, 13 },
  { NICM_LINK_RMNET_14, 14 },
  { NICM_LINK_RMNET_15, 15 },
  { NICM_LINK_RMNET_16, 16 },
  { NICM_LINK_RMNET_17, 17 },
  { NICM_LINK_RMNET_18, 18 },
  { NICM_LINK_RMNET_19, 19 },
  { NICM_LINK_RMNET_20, 20 },
  { NICM_LINK_RMNET_21, 21 },
  { NICM_LINK_RMNET_22, 22 },
  { NICM_LINK_RMNET_23, 23 },
  { NICM_LINK_RMNET_24, 24 },
  { NICM_LINK_RMNET_25, 25 },
  { NICM_LINK_RMNET_26, 26 },
  { NICM_LINK_RMNET_27, 27 },
  { NICM_LINK_RMNET_28, 28 },
  { NICM_LINK_RMNET_29, 29 },
  { NICM_LINK_RMNET_30, 30 },
  { NICM_LINK_RMNET_31, 31 },
  { NICM_LINK_RMNET_32, 32 },
};

#ifdef FEATURE_DATA_IWLAN
dsiNicmLinkMap dsiNicmOemprxyLinkMapTbl[DSI_MAX_IFACES] =
{
  { NICM_LINK_REV_RMNET_0,  34 },
  { NICM_LINK_REV_RMNET_1,  35 },
  { NICM_LINK_REV_RMNET_2,  36 },
  { NICM_LINK_REV_RMNET_3,  37 },
  { NICM_LINK_REV_RMNET_4,  38 },
  { NICM_LINK_REV_RMNET_5,  39 },
  { NICM_LINK_REV_RMNET_6,  40 },
  { NICM_LINK_REV_RMNET_7,  41 },
  { NICM_LINK_REV_RMNET_8,  42 },
  { NICM_LINK_REV_RMNET_9,  43 },
  { NICM_LINK_REV_RMNET_10, 44 },
  { NICM_LINK_REV_RMNET_11, 45 },
  { NICM_LINK_REV_RMNET_12, 46 },
  { NICM_LINK_REV_RMNET_13, 47 },
  { NICM_LINK_REV_RMNET_14, 48 },
  { NICM_LINK_REV_RMNET_15, 49 },
};

#define dsi_link_map_tbl_len(X) (int) (sizeof(X) / sizeof(dsiNicmLinkMap));
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION:  dsiNicmFindIpFamily
===========================================================================*/
/*!
    @brief
    retreive the ip_addr from the event info in the nicm nl message, provided
    the ip addr information is encoded in the message.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsiNicmFindIpFamily
(
  const dsiNicmNLEventInfo *info,
  dsi_ip_family_t              *ipf
)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG("dsiNicmFindIpFamily: ENTRY");

  if (NULL == info || NULL == ipf)
  {
    DSI_LOG_ERROR("dsiNicmFindIpFamily: invalid param(s)");
    goto bail;
  }

  /* If there's an IP addresses included in the message */
  if ((info->param_mask & DSI_NICM_EVT_PARAM_IPADDR) == DSI_NICM_EVT_PARAM_IPADDR)
  {
    if (AF_INET == info->addr_info.addr.ip_addr.ss_family)
    {
      *ipf = DSI_IP_FAMILY_V4;
      ret = DSI_SUCCESS;
    }
    else if (AF_INET6 == info->addr_info.addr.ip_addr.ss_family)
    {
      *ipf = DSI_IP_FAMILY_V6;
      ret = DSI_SUCCESS;
    }
  }

bail:
  DSI_LOG_DEBUG("dsiNicmFindIpFamily: addr=%s, ip_family=%d",
                NULL!=info?DSI_GET_IP_FAMILY(info->addr_info.addr.ip_addr.ss_family):"INVALID",
                (DSI_SUCCESS == ret) ? (int)*ipf : -1);
  return ret;
}

/*===========================================================================
  FUNCTION:  dsiNicmAbortPendingStart
===========================================================================*/
/*!
  @brief
  This function aborts any pending start_nw_if() attempts on the given iface

  @return
  None
*/
/*=========================================================================*/
static void dsiNicmAbortPendingStart
(
  int dsi_iface_id
)
{
  int          i, rc = QMI_INTERNAL_ERR, rc6 = QMI_INTERNAL_ERR;
  int          qmi_err_code=QMI_NO_ERR, qmi_err6_code=QMI_NO_ERR;
  dsi_store_t  *st = NULL;

  for (i = 0; i < DSI_MAX_DATA_CALLS; ++i)
  {
    st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (st != NULL &&
        st->priv.dsi_iface_id == dsi_iface_id &&
        DSI_STATE_CALL_CONNECTING == st->priv.call_state)
    {
      DSI_LOG_DEBUG("pending start_nw_if for qdi_hndl[%d], qdi_call_hndl[%p], iface[%d]",
                    DSI_GET_QDI_HNDL(dsi_iface_id),
                    st->priv.qdi_call_hndl,
                    dsi_iface_id);

      switch(st->priv.ip_version)
      {
        case DSI_IP_VERSION_4:
          if (DSI_INVALID_WDS_TXN != DSI_GET_WDS_V4_TXN(st))
          {
            rc = qdi_wds_abort(DSI_GET_QDI_HNDL(dsi_iface_id),
                               st->priv.qdi_call_hndl,
                               DSI_GET_WDS_V4_TXN(st),
                               &qmi_err_code );
          }
          else
          {
            DSI_LOG_DEBUG("dsiNicmAbortPendingStart: stopping call");
            rc = qdi_wds_stop_nw_if(DSI_GET_QDI_HNDL(dsi_iface_id),
                                    st->priv.qdi_call_hndl,
                                    dsi_qmi_wds_cmd_cb,
                                    QDI_MODE_NOT_SET,
                                    NULL,
                                    &qmi_err_code);
          }
        break;
        case DSI_IP_VERSION_6:
          if (DSI_INVALID_WDS_TXN != DSI_GET_WDS_V6_TXN(st))
          {
            rc = qdi_wds_abort(DSI_GET_QDI_HNDL(dsi_iface_id),
                               st->priv.qdi_call_hndl,
                               DSI_GET_WDS_V6_TXN(st),
                               &qmi_err_code );
          }
          else
          {
            DSI_LOG_DEBUG("dsiNicmAbortPendingStart: stopping call");
            rc = qdi_wds_stop_nw_if(DSI_GET_QDI_HNDL(dsi_iface_id),
                                    st->priv.qdi_call_hndl,
                                    dsi_qmi_wds_cmd_cb,
                                    QDI_MODE_NOT_SET,
                                    NULL,
                                    &qmi_err_code);
          }
        break;
        case DSI_IP_VERSION_4_6:
          if (DSI_INVALID_WDS_TXN != DSI_GET_WDS_V4_TXN(st)||
               DSI_INVALID_WDS_TXN != DSI_GET_WDS_V6_TXN(st))
          {
            rc = qdi_wds_abort(DSI_GET_QDI_HNDL(dsi_iface_id),
                               st->priv.qdi_call_hndl,
                               DSI_GET_WDS_V4_TXN(st),
                               &qmi_err_code );
            rc6 = qdi_wds_abort(DSI_GET_QDI_HNDL(dsi_iface_id),
                               st->priv.qdi_call_hndl,
                               DSI_GET_WDS_V6_TXN(st),
                               &qmi_err6_code );
          }
          else
          {
            DSI_LOG_DEBUG("dsiNicmAbortPendingStart: stopping call");
            rc = qdi_wds_stop_nw_if(DSI_GET_QDI_HNDL(dsi_iface_id),
                                    st->priv.qdi_call_hndl,
                                    dsi_qmi_wds_cmd_cb,
                                    QDI_MODE_NOT_SET,
                                    NULL,
                                    &qmi_err_code);
          }
          rc = rc | rc6;
        break;
      }

      if (rc < 0)
      {
        DSI_LOG_ERROR("dsiNicmAbortPendingStart: failed rc[%d], qmi_err[%d]",
                      rc,
                      qmi_err_code);
      }
      else
      {
        DSI_LOG_DEBUG("dsiNicmAbortPendingStart: successfully aborted rc=0x%x",
                      rc);

        /* release the interface */
        if(DSI_SUCCESS != dsi_link_release(dsi_iface_id))
        {
          DSI_LOG_ERROR("dsi_link_release failed: iface=[%d] " ,
                        dsi_iface_id);
        }
        else
        {
          DSI_LOG_DEBUG("dsi_link_release success: iface=[%d] " ,
                        dsi_iface_id);
        }
      }
    }
  }
}

static void dsiNicmCandidatePromote
(
  dsi_addr_info_ex_t *primary,
  dsi_addr_info_ex_t *candidate,
  dsi_ip_family_t ipf
)
{
  dsi_addr_info_ex_t tmp;

  memset(&tmp, 0, sizeof(tmp));
  ds_memscpy(&tmp, sizeof(tmp), candidate, sizeof(*candidate));
  ds_memscpy(candidate, sizeof(*candidate), primary,
             sizeof(*primary));
  ds_memscpy(primary, sizeof(*primary), &tmp, sizeof(tmp));

  /* Update the metadata */
  candidate->pdu_id = tmp.pdu_id;
  primary->pdu_id = 0;
  if (ipf == DSI_IP_FAMILY_V6)
  {
    candidate->address_type = DSI_ADDR_TYPE_CANDIDATE;
    primary->address_type = DSI_ADDR_TYPE_DEFAULT;
  }
}


static boolean dsiNicmStoreIpMatch
(
  dsi_store_t *dsi_store,
  dsi_ip_family_t ipf
)
{
  /* I'm sorry, but this condition is just too large and clunky to do in a
   * single if statement. Especially EVERYWHERE in the code...
   */
  if (dsi_store->priv.ip_version == DSI_IP_VERSION_4_6)
  {
    return DSI_TRUE;
  }

  if (dsi_store->priv.ip_version == DSI_IP_VERSION_4 && ipf == DSI_IP_FAMILY_V4)
  {
    return DSI_TRUE;
  }

  if (dsi_store->priv.ip_version == DSI_IP_VERSION_6 && ipf == DSI_IP_FAMILY_V6)
  {
    return DSI_TRUE;
  }

  return DSI_FALSE;
}


static int dsiNicmStoreNetReconfig
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  dsi_ip_family_t ipf;
  int i;

  /* Determine the IP family of the received msg */
  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_ERROR("unable to determine IP family");
    return DSI_ERROR;
  }

  /* Fill IP addr */
  dsi_fill_addr_info(dsi_iface_id, ipf, info);
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    dsi_store_t *st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (!st || dsi_iface_id != st->priv.dsi_iface_id ||
        DSI_TRUE != dsiNicmStoreIpMatch(st, ipf))
    {
      continue;
    }

    DSI_LOCK_MUTEX(&(st->priv.mutex));
    DSI_LOG_DEBUG("DSI call state logging: Handle %d, call state %d",
                  st->priv.dsi_iface_id, st->priv.call_state);

    /* If a new call has been attempted (partial retry) move to CONNECTED state */
    if (DSI_STATE_CALL_CONNECTING == st->priv.call_state &&
        DSI_EVT_NET_RECONFIGURED == dsi_event &&
        FALSE == st->priv.partial_retry)
    {
      *event_ret = DSI_EVT_NET_IS_CONN;
    }

    DSI_LOG_DEBUG(" Handle : qmi Instance %d", st->priv.rl_qmi_inst );
    if (st->priv.rl_qmi_inst != DSI_INVALID_QMI_INST &&
        ((ipf == DSI_IP_FAMILY_V4 && DSI_GET_WDS_V4_TXN(st) != DSI_INVALID_WDS_TXN) ||
         (ipf == DSI_IP_FAMILY_V6 && DSI_GET_WDS_V6_TXN(st) != DSI_INVALID_WDS_TXN)))
    {
      if (DSI_STATE_CALL_CONNECTED != st->priv.call_state ||
          DSI_EVT_NET_RECONFIGURED != *event_ret)
      {
        /* Don't send the event if Ansync SNI Resp is not recieved from modem*/
        DSI_LOG_ERROR("Invalid Txn:Dropping %d evt in %d state", dsi_event,
                      st->priv.call_state);
        *event_ret = DSI_EVT_INVALID;
      }
    }
    else if (DSI_STATE_CALL_CONNECTING == st->priv.call_state)
    {
      DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_CONNECTED);
    }
    else if (DSI_STATE_CALL_CONNECTED == st->priv.call_state &&
             DSI_EVT_NET_IS_CONN == dsi_event)
    {
      /* Don't send the event if the call is not yet connected */
      DSI_LOG_ERROR("Dropping %d evt in %d state", dsi_event,
                    st->priv.call_state);
      *event_ret = DSI_EVT_INVALID;
    }

    DSI_UNLOCK_MUTEX(&(st->priv.mutex));
  }

  return DSI_SUCCESS;
}


static int dsiNicmStoreNetNewAddr
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  dsi_addr_type addr_type = DSI_ADDR_TYPE_DEFAULT;
  unsigned int pdu_cnf = PRIMARY_PDU;
  dsi_ip_family_t ipf;
  int i;

  /* If the library using SSC mode 3, this event is deprecated */
  if (DSI_SSC_MODE_SSC_ONLY == dsi_ssc_mode)
  {
    *event_ret = DSI_EVT_PDU_RECONFIGED;
  }

  /* Determine the IP family of the received msg */
  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_ERROR("unable to determine IP family");
    return DSI_ERROR;
  }

  if ((info->param_mask & DSI_NICM_EVT_PARAM_PDU_ID) && info->pdu_id)
  {
    /* Non-zero pdu_id is the Secondary PDU */
    if (DSI_SSC_MODE_SSC_ONLY != dsi_ssc_mode)
    {
      DSI_LOG_INFO("ignoring secondary pdu address event");
      *event_ret = DSI_EVT_INVALID;
      return DSI_SUCCESS;
    }

    pdu_cnf = SECONDARY_PDU;
  }

  if (info->param_mask & DSI_NICM_EVT_PARAM_ADDR_TYPE)
  {
    addr_type = dsiConvertNicmAddrType(info->addr_info.address_type);
  }

  if (!dsi_is_iface_addr_valid_pdu(dsi_iface_id, ipf, pdu_cnf, addr_type))
  {
    /* If we don't already have a valid address for the IP family, store it */
    goto store_addr;
  }

  if (info->param_mask & DSI_NICM_EVT_PARAM_MTU)
  {
    if (DSI_GET_MTU(dsi_iface_id) != info->mtu ||
        DSI_GET_MTU_V4(dsi_iface_id) != info->mtu4 ||
        DSI_GET_MTU_V6(dsi_iface_id) != info->mtu6)
    {
      /* MTU is update. Store it */
      goto store_addr;
    }
  }

  if ((info->param_mask & DSI_NICM_EVT_PARAM_CACHE) == DSI_NICM_EVT_PARAM_CACHE)
  {
    dsi_addr_info_ex_t *addr = dsi_get_addrinfo_ex(dsi_iface_id, ipf, pdu_cnf,
                                                   addr_type);

    if (addr->deprecateTime != info->addr_info.cache_info.prefered ||
        addr->expiryTime != info->addr_info.cache_info.valid)
    {
      /* Lifetimes have updated. Store it */
      goto store_addr;
    }
  }

  /* Don't send the event. Address is already valid and no change detected.
   * We can get this event multiple times from nicm for an IPv6 addr.
   */
  *event_ret = DSI_EVT_INVALID;
  DSI_LOG_DEBUG("ignoring new address on pdu[%d]", pdu_cnf);
  return DSI_SUCCESS;

store_addr:
  DSI_LOG_DEBUG("storing new address for ip family=%d on pdu[%d]", ipf, pdu_cnf);
  dsi_fill_addr_info( dsi_iface_id, ipf, info );

  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    dsi_store_t *st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (!st || dsi_iface_id != st->priv.dsi_iface_id ||
        DSI_TRUE != dsiNicmStoreIpMatch(st, ipf))
    {
      continue;
    }

    DSI_LOCK_MUTEX(&(st->priv.mutex));
    DSI_LOG_DEBUG("DSI call state logging: Handle %d, call state %d",
                  st->priv.dsi_iface_id, st->priv.call_state);

    /* If a new call has been attempted (partial retry) move to CONNECTED state */
    if (DSI_TRUE == st->priv.partial_retry &&
        DSI_STATE_CALL_CONNECTING == st->priv.call_state)
    {
      DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_CONNECTED);
    }

    /* Send the NEWADDR event only if the call is already connected */
    if (DSI_STATE_CALL_CONNECTED != st->priv.call_state)
    {
      *event_ret = DSI_EVT_INVALID;
    }

    DSI_UNLOCK_MUTEX(&(st->priv.mutex));
  }

  return DSI_SUCCESS;
}

static int dsiNicmStoreNetDelAddr
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  dsi_addr_type addr_type = DSI_ADDR_TYPE_DEFAULT;
  unsigned int pdu_cnf = PRIMARY_PDU;
  dsi_addr_info_ex_t *addrinfo;
  dsi_addr_t *addr;
  dsi_ip_family_t ipf;
  int i;

  /* If library is using SSC mode 3, this event is deprecated */
  if (DSI_SSC_MODE_SSC_ONLY == dsi_ssc_mode)
  {
    *event_ret = DSI_EVT_PDU_RECONFIGED;
  }

  /* Determine the IP family of the received msg */
  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_ERROR("unable to determine IP family");
    *event_ret = DSI_EVT_INVALID;
    return DSI_ERROR;
  }

  if ((info->param_mask & DSI_NICM_EVT_PARAM_PDU_ID) && info->pdu_id)
  {
    /* Non-zero pdu_id is the Secondary PDU */
    if (DSI_SSC_MODE_SSC_ONLY != dsi_ssc_mode)
    {
      DSI_LOG_INFO("ignoring secondary pdu address event");
      *event_ret = DSI_EVT_INVALID;
      return DSI_SUCCESS;
    }
    pdu_cnf = SECONDARY_PDU;
  }

  if (info->param_mask & DSI_NICM_EVT_PARAM_ADDR_TYPE)
  {
    addr_type = dsiConvertNicmAddrType(info->addr_info.address_type);
  }

  /* If the address is already invalid, don't send the event */
  if (!dsi_is_iface_addr_valid_pdu(dsi_iface_id, ipf, pdu_cnf, addr_type))
  {
    DSI_LOG_DEBUG("DSI_EVT_NET_DELADDR: address family=%d already invalid for pdu[%d]", ipf, pdu_cnf);
    *event_ret = DSI_EVT_INVALID;
    return DSI_SUCCESS;
  }

  addrinfo = dsi_get_addrinfo_ex(dsi_iface_id, ipf, pdu_cnf, addr_type);
  addr = &addrinfo->iface_addr_s;
  if (memcmp(SASTORAGE_DATA(addr->addr),
             SASTORAGE_DATA(info->addr_info.addr.ip_addr),
             (DSI_IP_FAMILY_V4 == ipf ? DSI_SIZEOF_IPV4_ADDRESS: DSI_SIZEOF_IPV6_ADDRESS)))
  {
    DSI_LOG_ERROR("DSI_EVT_NET_DELADDR: ignoring unexpected address deletion for family=%d on pdu[%d]", ipf, pdu_cnf);
    DSI_INET_NTOP("current addr", &addr->addr);
    DSI_INET_NTOP("notif addr", &info->addr_info.addr.ip_addr);

    /* We might have already promoted the secondary pdu address so check if it matches secondary */
    if (dsi_ssc_mode == DSI_SSC_MODE_SSC_ONLY &&
        pdu_cnf == PRIMARY_PDU && addr_type == DSI_ADDR_TYPE_DEFAULT &&
        dsi_is_iface_addr_valid_pdu(dsi_iface_id, ipf, SECONDARY_PDU, DSI_ADDR_TYPE_CANDIDATE))
    {
      addrinfo = dsi_get_addrinfo_ex(dsi_iface_id, ipf, SECONDARY_PDU, DSI_ADDR_TYPE_CANDIDATE);
      addr = &addrinfo->iface_addr_s;
      if (memcmp(SASTORAGE_DATA(addr->addr),
                 SASTORAGE_DATA(info->addr_info.addr.ip_addr),
                 (DSI_IP_FAMILY_V4 == ipf ? DSI_SIZEOF_IPV4_ADDRESS: DSI_SIZEOF_IPV6_ADDRESS)))
      {
        DSI_LOG_ERROR("DSI_EVT_NET_DELADDR: address also doesn't match secondary candidate. Ignoring.");
        *event_ret = DSI_EVT_INVALID;
        return DSI_SUCCESS;
      }
      else
      {
        DSI_LOG_INFO("DSI_EVT_NET_DELADDR: address matches secondary candidate address");
        pdu_cnf = SECONDARY_PDU;
        addr_type == DSI_ADDR_TYPE_CANDIDATE;
      }
    }
    else
    {
      *event_ret = DSI_EVT_INVALID;
      return DSI_SUCCESS;
    }
  }

  DSI_LOG_ERROR("DSI_EVT_NET_DELADDR: invalidating address family=%d on pdu[%d]", ipf, pdu_cnf);

  /* Invalidate the address */
  addr->valid_addr = DSI_FALSE;

  if (DSI_SSC_MODE_SSC_ONLY == dsi_ssc_mode &&
      DSI_IP_FAMILY_V6 == ipf &&
      DSI_ADDR_TYPE_DEFAULT == addr_type)
  {
    dsi_addr_info_ex_t *candidate = dsi_get_addrinfo_ex(dsi_iface_id, ipf,
                                                        pdu_cnf,
                                                        DSI_ADDR_TYPE_CANDIDATE);

    /* Promote the candidate address over the default */
    dsiNicmCandidatePromote(addrinfo, candidate, ipf);
  }

  /* Don't send this event if we haven't sent the DSI_EVT_NET_IS_CONN event */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    dsi_store_t *st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (!st || dsi_iface_id != st->priv.dsi_iface_id ||
        DSI_TRUE != dsiNicmStoreIpMatch(st, ipf))
    {
      continue;
    }

    DSI_LOCK_MUTEX(&(st->priv.mutex));
    DSI_LOG_DEBUG("DSI call state logging: Handle %d, call state %d",
        st->priv.dsi_iface_id, st->priv.call_state);

    /* Send the DELADDR event only if the call is already connected */
    if (((DSI_STATE_CALL_DISCONNECTING == st->priv.call_state) &&
         (DSI_IP_VERSION_4_6 != st->priv.ip_version)) ||
        ((DSI_IP_VERSION_4 == st->priv.ip_version) &&
         (0 == DSI_GET_V4_COUNT(st->priv.dsi_iface_id))) ||
        ((DSI_IP_VERSION_6 == st->priv.ip_version) &&
         (0 == DSI_GET_V6_COUNT(st->priv.dsi_iface_id))))
    {
      *event_ret = DSI_EVT_NET_NO_NET;
    }
    else if (DSI_STATE_CALL_CONNECTED != st->priv.call_state)
    {
      *event_ret = DSI_EVT_INVALID;
    }

    DSI_UNLOCK_MUTEX(&(st->priv.mutex));
  }

  return DSI_SUCCESS;
}

/*===========================================================================
  FUNCTION:  dsiNicmUpdateCEReason
===========================================================================*/
/*!
    @brief
    Updates the call end reason for internal errors
*/
/*=========================================================================*/
static void dsiNicmUpdateCEReason
(
  dsi_store_t *st,
  dsi_ip_family_t ipf,
  dsiNicmOpStatusInfo op_status
)
{
  uint64_t hash_value = 0;
  if (!st)
  {
    DSI_LOG_ERROR("%s(): bad parameter(s)", __func__);
    return;
  }

  hash_value = dsi_get_hash_from_str_ptr(st);
  if(hash_value <= 0)
  {
    DSI_LOG_ERROR("%s(): No valid hash_value for st [%p]", __func__, st);
    return;
  }

  switch (ipf)
  {
  case DSI_IP_FAMILY_V4:
    st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_code = DSI_CE_REASON_CODE_ERROR;
    st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type = DSI_CE_TYPE_DSI_INTERNAL;

    DSI_LOG_DEBUG("%s(): set call end reason V4 code/type [%d/%d] on handle [%p] hash_value [%p]",
                  __func__, st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_code,
                  st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type, st, hash_value);

    break; /* DSI_IP_FAMILY_V4 */

  case DSI_IP_FAMILY_V6:
    st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_code = DSI_CE_REASON_CODE_ERROR;
    st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_type = DSI_CE_TYPE_DSI_INTERNAL;

    DSI_LOG_DEBUG("%s(): set call end reason V6 code/type [%d/%d] on handle [%p] hash_value [%p]",
                  __func__, st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_code,
                  st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_type, st, hash_value);

    break; /* DSI_IP_FAMILY_V6 */

  default:
    DSI_LOG_ERROR("%s(): invalid ipf [%d]", __func__, ipf);
    break;
  }
}

static int dsiNicmStoreNetNoNet
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  int i;

  if (info->cmd_data.modem_subs_id != DSI_GET_SUBS_ID(dsi_iface_id) )
  {
    if (info->cmd_data.ignore_subs == 1) {
      DSI_LOG_DEBUG("DSI_EVT_NET_NO_NET : ignoring sub mismatch");
    } else {
      DSI_LOG_DEBUG("DSI_EVT_NET_NO_NET : ignoring event as it is for different subs");
      *event_ret = DSI_EVT_INVALID;
      return DSI_SUCCESS;
    }
  }

  /* Abort any pending start_nw_if() requests on this iface */
  dsiNicmAbortPendingStart(dsi_iface_id);

  /* Update the call_state for all active calls on this iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    dsi_store_t *st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (!st || dsi_iface_id != st->priv.dsi_iface_id)
    {
      continue;
    }

    DSI_LOCK_MUTEX(&(st->priv.mutex));

#ifdef FEATURE_UPDATE_FAILURE_CODE
    if (DSI_NICM_OP_STATUS_CODE_SUCCESS != info->op_status.op_code)
    {
      /* If we reached here due to a network connection manager error, we should update the call end reason
         specifically. For regular disconnect or expected call failure scenarios the
         call end reason gets updated elsewhere */
      if (st->priv.ip_version == DSI_IP_VERSION_4_6)
      {
        dsiNicmUpdateCEReason(st, DSI_IP_FAMILY_V4, info->op_status);
        dsiNicmUpdateCEReason(st, DSI_IP_FAMILY_V6, info->op_status);
      }
      else if ((st->priv.ip_version == DSI_IP_VERSION_4)
                && (DSI_NICM_OP_STATUS_IP_FAMILY_V4 == info->op_status.op_ip_family))
      {
        dsiNicmUpdateCEReason(st, DSI_IP_FAMILY_V4, info->op_status);
      }
      else if ((st->priv.ip_version == DSI_IP_VERSION_6)
                && (DSI_NICM_OP_STATUS_IP_FAMILY_V6 == info->op_status.op_ip_family))
      {
        dsiNicmUpdateCEReason(st, DSI_IP_FAMILY_V6, info->op_status);
      }
    }
#endif /* FEATURE_UPDATE_FAILURE_CODE */

    DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_IDLE);
    /* qdi call handle will be freed in dsi_mni_release_client() below */
    st->priv.qdi_call_hndl = QDI_INVALID_CALL_HANDLE;
    DSI_UNLOCK_MUTEX(&(st->priv.mutex));
  }

  dsi_mni_release_client(dsi_iface_id);
  return DSI_SUCCESS;
}

static int dsiNicmStoreQosStatus
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  /* fill event payload */
  dsi_fill_qos_info( dsi_iface_id, info );
  DSI_LOG_DEBUG( "qos flow [0x%08x] activated on iface [%d]",
                 info->flow_info.flow_id, dsi_iface_id );
  return DSI_SUCCESS;
}

static int dsiNicmStoreNetNewMtu
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  if (info->param_mask & DSI_NICM_EVT_PARAM_MTU &&
      ((DSI_GET_MTU(dsi_iface_id) != info->mtu) ||
      (DSI_GET_MTU_V4(dsi_iface_id) != info->mtu4) ||
      (DSI_GET_MTU_V6(dsi_iface_id) != info->mtu6)))
  {
    DSI_SET_MTU(dsi_iface_id, info->mtu);
    DSI_SET_MTU_V4(dsi_iface_id, info->mtu4);
    DSI_SET_MTU_V6(dsi_iface_id, info->mtu6);
  }
  else
  {
    /* No need to send the event if there's no change in MTU */
    *event_ret = DSI_EVT_INVALID;
  }

  return DSI_SUCCESS;
}

static int dsiNicmStorePduAvailable
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  dsi_evt_payload_t *payload;
  dsi_ip_family_t ipf;
  int i;

  if (DSI_SSC_MODE_SSC_ONLY != dsi_ssc_mode)
  {
    /* Client does not support secondary PDU events. Ignore */
    DSI_LOG_DEBUG("Ignoring SSC mode 3 event");
    *event_ret = DSI_EVT_INVALID;
    return DSI_SUCCESS;
  }

  /* Determine the IP family of the received msg */
  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_ERROR("unable to determine IP family");
    return DSI_ERROR;
  }

  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    dsi_store_t *st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;

    if (!st || dsi_iface_id != st->priv.dsi_iface_id ||
        DSI_TRUE != dsiNicmStoreIpMatch(st, ipf))
    {
      continue;
    }

    payload = malloc(sizeof(dsi_evt_payload_t));
    if (NULL == payload)
    {
      DSI_LOG_ERROR("failed to allocate event payload for dsi id[%d]",
                    dsi_iface_id);
      break; /* continue? Do we really need to bail on everyone? */
    }

    memset(payload, 0, sizeof(dsi_evt_payload_t));
    if (DSI_NICM_EVT_PARAM_PDU_ID & info->param_mask)
    {
      payload->pdu_id = info->pdu_id;
    }

    st->ev_payload = payload;
  }

  DSI_LOG_DEBUG("Secondary PDU now available");
  return DSI_SUCCESS;
}

static int dsiNicmStorePduPromotion
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  dsi_addr_info_ex_t *addr_info, *sec_addr_info;
  dsi_ip_family_t ipf;

  if (DSI_SSC_MODE_SSC_ONLY != dsi_ssc_mode)
  {
    /* Client does not support secondary PDU events. Ignore */
    DSI_LOG_DEBUG("Ignoring SSC mode 3 event");
    *event_ret = DSI_EVT_INVALID;
    return DSI_SUCCESS;
  }

  /* Determine the IP family of the received msg */
  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_ERROR("unable to determine IP family");
    return DSI_ERROR;
  }

  DSI_LOG_DEBUG("Secondary PDU promoted to primary");
  /* Secondary Candidate replaces Primary Default */
  addr_info = dsi_get_addrinfo_ex(dsi_iface_id, ipf, PRIMARY_PDU,
                                  DSI_ADDR_TYPE_DEFAULT);
  sec_addr_info = dsi_get_addrinfo_ex(dsi_iface_id, ipf, SECONDARY_PDU,
                                      DSI_ADDR_TYPE_CANDIDATE);
  dsiNicmCandidatePromote(addr_info, sec_addr_info, ipf);

  /* Update the metadata */
  sec_addr_info->primary = DSI_FALSE;
  addr_info->primary = DSI_TRUE;

  return DSI_SUCCESS;
}

static int dsiNicmStorePduRelease
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo *info,
  int *event_ret
)
{
  (void)dsi_iface_id;
  (void)dsi_event;
  (void)info;

  if (DSI_SSC_MODE_SSC_ONLY != dsi_ssc_mode)
  {
    /* Client does not support secondary PDU events. Ignore */
    DSI_LOG_DEBUG("Ignoring SSC mode 3 event");
    *event_ret = DSI_EVT_INVALID;
  }
  else
  {
    DSI_LOG_DEBUG("Secondary PDU released");
  }

  return DSI_SUCCESS;
}

/*===========================================================================
  FUNCTION:  dsiNicmStoreEventData
===========================================================================*/
/*!
    @brief
    stores relevant parts of event data into the internal structure
    associated with the given dsi_iface_id

    @return
    dsi_net_evt_t. Propagate the event to the upper layers only if the return
    value isn't DSI_EVT_INVALID
*/
/*=========================================================================*/
dsi_net_evt_t dsiNicmStoreEventData
(
  int dsi_iface_id,
  int dsi_event,
  const dsiNicmNLEventInfo * info
)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  dsi_net_evt_t   evt_ret = DSI_EVT_INVALID;

  if ((dsi_inited != DSI_TRUE) &&
      (!DSI_IS_ID_VALID_R(dsi_iface_id) && (dsi_event != DSI_EVT_NET_NO_NET)))
  {
    DSI_LOG_ERROR("dsiNicmStoreEventData: bad param");
    return DSI_EVT_INVALID;
  }

  DSI_LOG_DEBUG("dsiNicmStoreEventData: ENTRY");

  ret = DSI_ERROR;
  do
  {
    if (NULL == info)
    {
      DSI_LOG_ERROR("programming err: NULL info ptr rcvd");
      break;
    }

    reti = DSI_SUCCESS;
    evt_ret = dsi_event;

    switch(dsi_event)
    {
      case DSI_EVT_NET_IS_CONN:
      case DSI_EVT_NET_RECONFIGURED:
        reti = dsiNicmStoreNetReconfig(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_NET_NEWADDR:
        reti = dsiNicmStoreNetNewAddr(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_NET_DELADDR:
        reti = dsiNicmStoreNetDelAddr(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_NET_NO_NET:
        reti = dsiNicmStoreNetNoNet(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_QOS_STATUS_IND:
        reti = dsiNicmStoreQosStatus(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_NET_NEWMTU:
        reti = dsiNicmStoreNetNewMtu(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_PDU_AVAILABLE:
        reti = dsiNicmStorePduAvailable(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_PDU_PROMOTION:
        reti = dsiNicmStorePduPromotion(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_PDU_RELEASE:
        reti = dsiNicmStorePduRelease(dsi_iface_id, dsi_event, info, &evt_ret);
        break;
      case DSI_EVT_PDU_RECONFIGED:
        /* This event is never sent directly */
        break;
      default:
        DSI_LOG_DEBUG("programming err: invalid dsi_evt rcvd");
        break;

    }
    if (DSI_ERROR == reti)
    {
      break;
    }
    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG("dsiNicmStoreEventData: EXIT success, evt_ret %d", (int)evt_ret);
  }
  else
  {
    DSI_LOG_ERROR("dsiNicmStoreEventData: EXIT error, evt_ret %d dsi_event %d", (int)evt_ret, dsi_event);
  }

  return evt_ret;
}


/*===========================================================================
  FUNCTION:  dsiNicmMapLink
===========================================================================*/
/*!
    @brief
    maps nicm link to dsi iface id. if a match is found, dsi iface id
    is returned in dsi_iface_id placeholder.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsiNicmMapLink
(
  dsiNicmLinkId nicmLink,
  int * dsi_iface_id
)
{
  int i=0;
  int ret=DSI_ERROR;

  DSI_LOG_DEBUG("dsiNicmMapLink: ENTRY");

  do
  {
    if (NULL == dsi_iface_id)
    {
      DSI_LOG_DEBUG("programming err: NULL dsi_iface_id ptr");
      break;
    }

    /* find the matching dsi iface id */
    for(i=0; i<DSI_MAX_IFACES; i++)
    {
      if (dsiNicmLinkMapTbl[i].nicmLink == nicmLink)
      {
        break;
      }
    }

    if (i == DSI_MAX_IFACES)
    {
      DSI_LOG_ERROR("we don't recognize netlink link [%d]. Check " \
                    "dsiNicmLinkMapTbl", nicmLink);
      break;
    }

    DSI_LOG_DEBUG("nicm link [%d] maps to  dsi iface [%d]",
                  nicmLink, i);

    *dsi_iface_id = i;
    ret = DSI_SUCCESS;
  } while (0);

  DSI_LOG_DEBUG("dsiNicmMapLink: EXIT");

  return ret;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION:  dsiNicmMapReverseLink
===========================================================================*/
/*!
    @brief
    maps reverse link to dsi iface id. if a match is found, dsi iface id
    is returned in dsi_iface_id placeholder.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsiNicmMapReverseLink
(
  dsiNicmLinkId nicmLink,
  int * dsi_iface_id
)
{
  int i=0;
  int ret=DSI_ERROR;

  DSI_LOG_DEBUG("dsiNicmMapReverseLink: ENTRY");

  do
  {
    if (NULL == dsi_iface_id)
    {
      DSI_LOG_DEBUG("programming err: NULL dsi_iface_id ptr");
      break;
    }

    /* find the matching dsi iface id */
    for(i=0; i<DSI_MAX_REV_RMNET_IFACES; i++)
    {
      if (dsiNicmOemprxyLinkMapTbl[i].nicmLink == nicmLink)
      {
        break;
      }
    }

    if (i == DSI_MAX_REV_RMNET_IFACES)
    {
      DSI_LOG_ERROR("we don't recognize netlink link [%d]. Check " \
                    "dsiNicmOemprxyLinkMapTbl", nicmLink);
      break;
    }

    DSI_LOG_DEBUG("nicm link [%d] maps to  dsi iface [%d]",
                  nicmLink, i);

    *dsi_iface_id = i;
    ret = DSI_SUCCESS;
  } while (0);

  DSI_LOG_DEBUG("dsiNicmMapReverseLink: EXIT");

  return ret;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION:  dsiNicmPostEvent
===========================================================================*/
/*!
    @brief
    post the dsi event as a result of receiving
    nicm event
    nicm_link is the link returned by nicm callback
    event is the dsi event to be repoted to the client(s)

    @return
    none
*/
/*=========================================================================*/
void dsiNicmPostEvent
(
  int dsi_iface_id,
  dsi_net_evt_t event,
  const dsiNicmNLEventInfo * info
)
{
  dsi_ce_reason_t ce_reason;
  dsi_store_t * st = NULL;
  int count;
  boolean handle_found = FALSE;
  dsi_ip_family_t              ipf = DSI_NUM_IP_FAMILIES;
  DSI_LOG_DEBUG("posting [%d] for dsi_iface_id [%d]",
                event, dsi_iface_id);

  if (NULL == info)
  {
    DSI_LOG_ERROR("programming err: NULL info ptr rcvd");
    return;
  }

  if (DSI_SUCCESS != dsiNicmFindIpFamily(info, &ipf))
  {
    DSI_LOG_DEBUG("unable to determine IP family; assuming I/F is down");
  }


  /* notify clients that are associated with this dsi iface */
  dsi_notify_users(dsi_iface_id, event,ipf);

  if (event == DSI_EVT_NET_NO_NET)
  {
    /* this would notify the only clients who are
       still attached with this interface */
    DSI_LOG_DEBUG("notify users on interface [%d] of event "
                  "DSI_EVT_NET_NO_NET", dsi_iface_id);

    /* detach interface from this call store handle */
    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == dsi_iface_id)
      {

        DSI_LOG_DEBUG("ipf %d st->priv.ip_version %d",ipf, st->priv.ip_version);

        if( (ipf == DSI_NUM_IP_FAMILIES) ||
             (DSI_IP_VERSION_4_6 == st->priv.ip_version)||
             ( ((DSI_IP_VERSION_4 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V4)) ||
               ((DSI_IP_VERSION_6 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V6)) ) )
        {
          handle_found = TRUE;
          DSI_LOG_DEBUG("handle_found %d",handle_found);
          break;
        }
      }
    }
    if (TRUE == handle_found)
    {
      DSI_LOCK_MUTEX(&(st->priv.mutex));
      dsi_detach_dsi_iface(st);
      DSI_UNLOCK_MUTEX(&(st->priv.mutex));
    }
    else
    {
      DSI_LOG_INFO("No call state for index [%d]", dsi_iface_id);
    }

    /* we determine ce reason from QMI
     * set reason_code to UNKNOWN here */
    ce_reason.reason_code = DSI_CE_REASON_UNKNOWN;
    ce_reason.reason_type = DSI_CE_TYPE_UNINIT;

    /* set this interface free... */
    dsi_release_dsi_iface(dsi_iface_id, &ce_reason,ipf);
  }
}
