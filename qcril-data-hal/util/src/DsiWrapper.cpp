/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "DsiWrapper.h"
#include <iomanip>
#include <sstream>

bool isDsiex2Supported = false;


using namespace rildata;

void DsiWrapper::dsiQcrilDataInit()
{
  dsi_set_ril_instance(global_instance_id);
  dsi_set_modem_subs_id(global_subs_id);
}

void DsiWrapper::dsiStopDataCall(dsi_hndl_t dsiHandle) {
  std::stringstream ss;
  ss << "[DsiWrapper] dsiStopDataCall hndl=" << (void *)dsiHandle;
  Log::getInstance().d(ss.str());
  dsi_stop_data_call(dsiHandle);
}

int DsiWrapper::dsiStartDataCall(dsi_hndl_t dsiHandle) {
  std::stringstream ss;
  ss << "[DsiWrapper] dsiStartDataCall hndl=" << (void *)dsiHandle;
  Log::getInstance().d(ss.str());
  if (isDsiex2Supported) {
    auto dsiCallparams = dsiStartDataCallMap[dsiHandle];
    size_t n = dsiCallparams.size();
    Log::getInstance().d("[DsiWrapper] dsiStartDataCallParams Size " + std::to_string((int) n));
    int i = 0;
    dsi_call_param_info_t  *params = new (std::nothrow) dsi_call_param_info_t [n] {};
    if (params == nullptr)
      return -1;
    for (auto it: dsiCallparams) {
      params[i].identifier = it.first;
      params[i].value = it.second;
      Log::getInstance().d("[DsiWrapper] dsiStartDataCallParams [" + std::to_string(i) + "]" + convertToString(params[i].identifier, &params[i].value));
      i++;
    }
    int res = dsi_start_data_call_ex (dsiHandle, (int)n, params);
    for (int j = 0; j < n; j++) {
      if (params[j].value.buf_val) {
        delete[] params[j].value.buf_val;
      }
    }
    if (params) {
      delete[] params;
    }
    dsiStartDataCallMap.erase(dsiHandle);
    return res;
  }
  return dsi_start_data_call(dsiHandle);
}

int DsiWrapper::dsiSetDataCallParam(
  dsi_hndl_t hndl,
  dsi_call_param_identifier_t identifier,
  dsi_call_param_value_t *info
) {
  if (info == nullptr) {
    Log::getInstance().d("[DsiWrapper]  dsiSetDataCallParam Invalid param is passed");
    return -1;
  }
  std::stringstream ss;
  ss << (void *)hndl;
  Log::getInstance().d("[DsiWrapper] dsiSetDataCallParam hndl ["+ ss.str() +"] " + convertToString(identifier, info));
  if (isDsiex2Supported) {
    dsi_call_param_value_t callInfo = {};
    callInfo.num_val = info->num_val;
    if (info->buf_val) {
      callInfo.buf_val = new (std::nothrow) char[callInfo.num_val] {};
      if (callInfo.buf_val) {
        memcpy(callInfo.buf_val, info->buf_val, (size_t) callInfo.num_val);
      }
    }
    dsiStartDataCallMap[hndl][identifier] = callInfo;
    Log::getInstance().d("[DsiWrapper]  Entry added in dsiStartDataCallMap");
    return DSI_SUCCESS;
  }
  return dsi_set_data_call_param(hndl, identifier, info);
}

dsi_hndl_t DsiWrapper::dsiGetDataSrvcHndl(
  dsi_net_ev_cb cb_fn,
  void * user_data
) {
  dsi_hndl_t dsiHandle = dsi_get_data_srvc_hndl(cb_fn, user_data);
  std::stringstream ss;
  ss << "[DsiWrapper] dsiGetDataSrvcHndl " << (void *)dsiHandle;
  Log::getInstance().d(ss.str());
  return dsiHandle;
}

dsi_hndl_t DsiWrapper::dsiGetDataSrvcHndl(
  dsi_net_ev_cb_ex user_cb_fn,
  dsi_init_mode_t  mode,
  void *user_data
) {
  dsi_hndl_t dsiHandle = dsi_get_data_srvc_hndl_ex(user_cb_fn, mode, user_data);
  std::stringstream ss;
  ss << "[DsiWrapper] dsiGetDataSrvcHndl ex2 " << (void *)dsiHandle;
  Log::getInstance().d(ss.str());
  return dsiHandle;
}

void DsiWrapper::dsiRelDataSrvcHndl(dsi_hndl_t hndl) {
  std::stringstream ss;
  ss << "[DsiWrapper] dsiRelDataSrvcHndl " << (void *)hndl;
  Log::getInstance().d(ss.str());
  if (isDsiex2Supported) {
    dsi_rel_data_srvc_hndl_ex(hndl);
    dsiConnectionMap.erase(hndl);
  } else {
    dsi_rel_data_srvc_hndl(hndl);
  }
}

int DsiWrapper::dsiGetDeviceName(dsi_hndl_t dsiHandle, std::string &deviceName) {
  Log::getInstance().d("[DsiWrapper] dsiGetDeviceName");

  int rc = 0;
  char deviceNameBuf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1];
  memset(deviceNameBuf, 0, sizeof(deviceNameBuf));

  if (isDsiex2Supported && dsiConnectionMap[dsiHandle].isConnectInfoValid) {
    strlcpy(deviceNameBuf, dsiConnectionMap[dsiHandle].connectInfo.dev_node, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 1);
  } else {
    rc = dsi_get_device_name(dsiHandle, deviceNameBuf, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 1);
  }
  if (rc!=0) {
    Log::getInstance().d("[DsiWrapper]: couldn't get device name");
  }
  deviceName = std::string(deviceNameBuf);

  Log::getInstance().d("[DsiWrapper]: device name = " + deviceName);

  return 0;
}

unsigned int DsiWrapper::dsiGetMtu(dsi_hndl_t dsiHandle) {
  Log::getInstance().d("[DsiWrapper] dsiGetMtu");

  int rc;
  unsigned int mtu;

  if (isDsiex2Supported){
    mtu = dsiConnectionMap[dsiHandle].connectInfo.mtu;
  } else if (dataMtu > 0) {
    mtu = dataMtu;
  }
  else {
    rc = dsi_get_link_mtu(dsiHandle, &mtu);
    if (rc!=0) {
      Log::getInstance().d("[DsiWrapper]: couldn't get mtu");
    }
  }

  Log::getInstance().d("[DsiWrapper]: mtu = " + std::to_string((int)mtu));
  return mtu;
}

dsi_ce_reason_t DsiWrapper::dsiGetVerboseCallEndReason(dsi_hndl_t DsiHandle, std::string ipType) {
  dsi_ip_family_t ipf = DSI_IP_FAMILY_V4;
  if (ipType == "IPV6") {
    ipf = DSI_IP_FAMILY_V6;
  }
  dsi_ce_reason_t dsiEndReason = {};
  /* If dsi_start_data_call_ex fails or DSI_NET_PARTIAL_CONN is received,
     dsi wont send call end reason in the dsiNetcallback.
     So When RIL tries to get call_end_reason, dsi_hndl wont be present in the map
     and we need to query DSI for call end reason.
  */
  if (isDsiex2Supported && (dsiConnectionMap.find(DsiHandle) != dsiConnectionMap.end()) && dsiConnectionMap[DsiHandle].isDisconnectInfoValid) {
    if (ipf == DSI_IP_FAMILY_V6 && dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason6.is_valid) {
      dsiEndReason = dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason6.ce_reason;
    }
    if (ipf == DSI_IP_FAMILY_V4 && dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason4.is_valid) {
      dsiEndReason = dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason4.ce_reason;
    }
  } else {
    dsi_get_call_end_reason(DsiHandle, &dsiEndReason, ipf);
  }
  Log::getInstance().d("[DsiWrapper] dsiGetVerboseCallEndReason type=" + std::to_string(dsiEndReason.reason_type) +
                       " code=" + std::to_string(dsiEndReason.reason_code));
  return dsiEndReason;
}

int DsiWrapper::dsiGetCallEndReason(dsi_hndl_t DsiHandle) {
  Log::getInstance().d("[DsiWrapper] dsiGetCallEndReason");

  int endReason = PDP_FAIL_NONE;
  dsi_ip_family_t ipf = DSI_IP_FAMILY_V4;
  dsi_ce_reason_t dsiEndReason;

  if (isDsiex2Supported && dsiConnectionMap[DsiHandle].isDisconnectInfoValid) {
    if (dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason4.is_valid) {
      dsiEndReason = dsiConnectionMap[DsiHandle].disconnectInfo.ce_reason4.ce_reason;
    }
  } else {
    dsi_get_call_end_reason(DsiHandle, &dsiEndReason, ipf);
  }
  if ( 0 != qcril_data_get_ril_ce_code(&dsiEndReason, &endReason)) {
    endReason = PDP_FAIL_OEM_DCFAILCAUSE_1;
  }

  Log::getInstance().d("[DsiWrapper] endReason = "+std::to_string(endReason));
  return endReason;
}

dsi_call_tech_type DsiWrapper::dsiGetCallTech(dsi_hndl_t DsiHandle) {
  Log::getInstance().d("[DsiWrapper] dsiGetCallTech");

  dsi_call_tech_type callTech = DSI_EXT_TECH_INVALID;
  dsi_get_call_tech(DsiHandle, &callTech);
  return callTech;
}

int DsiWrapper::dsiInit(bool fromSSR, void (* dsi_init_cb_func)( void * ))
{
  int result = DSI_ERROR;
  Log::getInstance().d("[DsiWrapper]:: dsiInit instanceId =" + std::to_string(global_instance_id) + " subs_id =" + std::to_string(global_subs_id));
  dsi_set_modem_subs_id(global_subs_id);

  if(fromSSR)
  {
    result = dsi_init_ex( DSI_MODE_SSR, dsi_init_cb_func, NULL );
    if(DSI_SUCCESS != result)
      Log::getInstance().d("dsi_init_ex trigger failed "+ std::to_string(result));
    else
      Log::getInstance().d("dsi_init_ex triggered Successfully"+ std::to_string(result));
  }
  else
  {
    result = dsi_init_ex( DSI_MODE_GENERAL, dsi_init_cb_func, NULL );
    if(DSI_SUCCESS != result)
      Log::getInstance().d("dsi_init_ex trigger failed"+ std::to_string(result));
    else
      Log::getInstance().d("dsi_init_ex triggered Successfully"+std::to_string(result));
  }
  return result;
}

int DsiWrapper::dsiInit(void (*dsi_init_cb_func_ex2)(dsi_init_mode_t , void* ))
{
  int result = DSI_ERROR;
  dsiStartDataCallMap.clear();
  dsiConnectionMap.clear();
  Log::getInstance().d("[DsiWrapper]:: dsiInit ex2 instanceId =" + std::to_string(global_instance_id) + " subs_id =" + std::to_string(global_subs_id));
  dsi_set_modem_subs_id(global_subs_id);

  result = dsi_init_ex2( DSI_MODE_GENERAL_EX, dsi_init_cb_func_ex2, NULL );
  if(DSI_SUCCESS != result)
    Log::getInstance().d("dsi_init_ex2 trigger failed"+ std::to_string(result));
  else
    Log::getInstance().d("dsi_init_ex2 triggered Successfully"+std::to_string(result));
  return result;
}

int DsiWrapper::dsiRelease()
{
  int result = DSI_ERROR;

  if (isDsiex2Supported) {
    result = dsi_release_ex(DSI_MODE_GENERAL_EX);
  } else {
    result = dsi_release(DSI_MODE_GENERAL);
  }

  if(DSI_SUCCESS != result)
    Log::getInstance().d("dsi_release failed");
  else
    Log::getInstance().d("dsi_release Successful");
  return result;
}

void DsiWrapper::dsiUpdateConnectInfo(dsi_hndl_t handle, rildata::dsiInfo &info)
{
  std::stringstream ss;
  ss << (void *)handle;
  dsiConnectionMap[handle] = info;
  Log::getInstance().d("[DsiWrapper]: dsiUpdateConnectInfo() handle="+
    ss.str() +", Interface_name ="+std::string(info.connectInfo.dev_node));
}

void DsiWrapper::dsiUpdateDisconnectInfo(dsi_hndl_t handle, rildata::dsiInfo &info)
{
  std::stringstream ss;
  ss << (void *)handle;
  dsiConnectionMap[handle] = info;
  Log::getInstance().d("[DsiWrapper]: dsiUpdateDisconnectInfo() handle="+ ss.str());
}

void DsiWrapper::dsiUpdateMtu(dsi_hndl_t handle, uint mtu)
{
  std::stringstream ss;
  ss << (void *)handle;
  dsiConnectionMap[handle].connectInfo.mtu = mtu;
  Log::getInstance().d("[DsiWrapper]: dsiUpdateMtu() handle="+ ss.str() +
              "Updated Mtu" + std::to_string((int)dsiConnectionMap[handle].connectInfo.mtu));
}

void DsiWrapper::dsiUpdateInfo(dsi_hndl_t handle, rildata::dsiInfo &info)
{
  std::stringstream ss;
  ss << (void *)handle;
  Log::getInstance().d("[DsiWrapper]: dsiUpdateInfo() handle="+ ss.str());
  if (info.isDisconnectInfoValid) {
    dsiUpdateDisconnectInfo(handle, info);
  }
  else if (info.isConnectInfoValid) {
    dsiUpdateConnectInfo(handle, info);
  }
  else if (info.isMtuValid) {
    dsiUpdateMtu(handle, info.connectInfo.mtu);
  }
}

void DsiWrapper::dsiEraseConnectionMap(dsi_hndl_t handle)
{
  dsiConnectionMap.erase(handle);
}
