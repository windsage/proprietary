/*===========================================================================

  Copyright (c) 2018-2020, 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef DSIWRAPPER
#define DSIWRAPPER

#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dsi_netctrl.h"
#include <framework/Log.h>
#include "qcril_data.h"
#include "MessageCommon.h"
#include "event/RilEventDataCallback.h"

#define DS_CALL_INFO_ADDR_PCSCF_MAX_LEN (200)

#define AddressFormatIPv4WithSubnetMask(data,mask,str,len)    \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d/%d%c",                             \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        mask, 0 );

#define AddressFormatIPv6WithPrefixLength(data,mask,str,len) \
        (void)snprintf( str, len,                                            \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x/%d%c",  \
                        ((char*)data)[ 0],((char*)data)[ 1],                 \
                        ((char*)data)[ 2],((char*)data)[ 3],                 \
                        ((char*)data)[ 4],((char*)data)[ 5],                 \
                        ((char*)data)[ 6],((char*)data)[ 7],                 \
                        ((char*)data)[ 8],((char*)data)[ 9],                 \
                        ((char*)data)[10],((char*)data)[11],                 \
                        ((char*)data)[12],((char*)data)[13],                 \
                        ((char*)data)[14],((char*)data)[15], mask, 0 );

#define AddressFormatIPv4(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d%c",                                \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3], 0 );

#define AddressFormatIPv6(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"  \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x%c",\
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        ((char*)data)[ 4],((char*)data)[ 5],            \
                        ((char*)data)[ 6],((char*)data)[ 7],            \
                        ((char*)data)[ 8],((char*)data)[ 9],            \
                        ((char*)data)[10],((char*)data)[11],            \
                        ((char*)data)[12],((char*)data)[13],            \
                        ((char*)data)[14],((char*)data)[15], 0 );


extern bool isDsiex2Supported;

class DsiWrapper {
  private:
    void dsiUpdateConnectInfo(dsi_hndl_t handle, rildata::dsiInfo &info);
    void dsiUpdateDisconnectInfo(dsi_hndl_t handle, rildata::dsiInfo &info);
    void dsiUpdateMtu(dsi_hndl_t handle, uint mtu);
  protected:
    std::unordered_map<dsi_hndl_t, rildata::dsiInfo> dsiConnectionMap;
    std::unordered_map<dsi_hndl_t, std::unordered_map<dsi_call_param_identifier_t, dsi_call_param_value_t>> dsiStartDataCallMap;
  public:
    DsiWrapper() {}
    virtual ~DsiWrapper() {}
    DsiWrapper(DsiWrapper const &) = delete;             // Copy construct
    DsiWrapper(DsiWrapper &&) = delete;                  // Move construct
    DsiWrapper &operator=(DsiWrapper const &) = delete;  // Copy assign
    DsiWrapper &operator=(DsiWrapper &&) = delete;       // Move assign
    static DsiWrapper &getInstance();

    /**Functions Common for class classes**/
    virtual int dsiInit(bool fromSSR, void (* dsi_init_cb_func)( void * ));
    virtual int dsiInit(void (*dsi_init_cb_func_ex2)(dsi_init_mode_t , void* ));
    virtual void dsiQcrilDataInit();
    virtual int dsiRelease();
    virtual void dsiStopDataCall(dsi_hndl_t dsiHandle);
    virtual int dsiStartDataCall(dsi_hndl_t dsiHandle);
    virtual int dsiSetDataCallParam(dsi_hndl_t hndl,
                                    dsi_call_param_identifier_t identifier,
                                    dsi_call_param_value_t *info);
    virtual dsi_hndl_t dsiGetDataSrvcHndl(dsi_net_ev_cb cb_fn,
                                          void * user_data);
    virtual dsi_hndl_t dsiGetDataSrvcHndl(dsi_net_ev_cb_ex user_cb_fn,
                                          dsi_init_mode_t  mode,
                                          void *user_data);
    virtual void dsiUpdateInfo(dsi_hndl_t handle, rildata::dsiInfo &info);
    virtual void dsiEraseConnectionMap(dsi_hndl_t handle);
    virtual void dsiRelDataSrvcHndl(dsi_hndl_t hndl);
    virtual int dsiGetDeviceName(dsi_hndl_t DsiHandle, std::string &deviceName);
    virtual unsigned int dsiGetMtu(dsi_hndl_t DsiHandle);
    virtual int dsiGetCallEndReason(dsi_hndl_t DsiHandle);
    virtual dsi_ce_reason_t dsiGetVerboseCallEndReason(dsi_hndl_t DsiHandle,
                                                       std::string ipType);
    virtual dsi_call_tech_type dsiGetCallTech(dsi_hndl_t DsiHandle);


    /**Functions Defined Separately for child classes*/
    virtual unsigned int dsiGetAddresses(dsi_hndl_t,
                                         bool &,
                                         bool &,
                                         rildata::PduInfo &,
                                         rildata::PduInfo &) {return 0;}
    virtual int dsiGetPcscfAddresses(dsi_hndl_t,
                                     std::string &,
                                     std::string &,
                                     uint32_t) {return 0;}
    virtual int dsiCheckIfCallExists(dsi_hndl_t,
                                     std::string,
                                     wds_apn_type_mask_v01,
                                     int,
                                     std::string &) {return 1;}
    virtual int dsiGetCurrentDataBearerTech(dsi_hndl_t,
                                            rildata::HandoffNetworkType_t &) {return 1;}
    virtual int dsiSetSSCCapability() {return DSI_SUCCESS;}
    virtual void dsiGetMtuByFamily(dsi_hndl_t,
                                   dsi_v4_v6_mtu_t &,
                                   uint32_t) {}
    virtual std::string convertToString(dsi_call_param_identifier_t,
                                        dsi_call_param_value_t*) {return "";}
};

class DsiWrapperLA: public DsiWrapper {
  public:
    DsiWrapperLA() {}
    virtual ~DsiWrapperLA() {}
    virtual unsigned int dsiGetAddresses(dsi_hndl_t DsiHandle,
                                         bool &v4Connected,
                                         bool &v6Connected,
                                         rildata::PduInfo &primPduInfo,
                                         rildata::PduInfo &secPduInfo);

    virtual int dsiGetPcscfAddresses(dsi_hndl_t DsiHandle,
                                     std::string &pcscfAddressesV4,
                                     std::string &pcscfAddressesV6,
                                     uint32_t pduId);

    virtual int dsiCheckIfCallExists(dsi_hndl_t,
                                     std::string,
                                     wds_apn_type_mask_v01,
                                     int,
                                     std::string &);
    virtual int dsiGetCurrentDataBearerTech(dsi_hndl_t,
                                            rildata::HandoffNetworkType_t &);
    virtual int dsiSetSSCCapability();
    virtual void dsiGetMtuByFamily(dsi_hndl_t,
                                   dsi_v4_v6_mtu_t &,
                                   uint32_t);
    virtual std::string convertToString(dsi_call_param_identifier_t,
                                        dsi_call_param_value_t*);
};

class DsiWrapperLE: public DsiWrapper {
  public:
    DsiWrapperLE() {}
    virtual ~DsiWrapperLE() {}
    virtual unsigned int dsiGetAddresses(dsi_hndl_t DsiHandle,
                                         bool &v4Connected,
                                         bool &v6Connected,
                                         rildata::PduInfo &primPduInfo,
                                         rildata::PduInfo &secPduInfo);

    virtual int dsiGetPcscfAddresses(dsi_hndl_t DsiHandle,
                                     std::string &pcscfAddressesV4,
                                     std::string &pcscfAddressesV6,
                                     uint32_t pduId);
    virtual int dsiGetCurrentDataBearerTech(dsi_hndl_t,
                                            rildata::HandoffNetworkType_t &);
    virtual void dsiGetMtuByFamily(dsi_hndl_t,
                                   dsi_v4_v6_mtu_t &,
                                   uint32_t);
    virtual std::string convertToString(dsi_call_param_identifier_t,
                                        dsi_call_param_value_t*);
};

#endif
