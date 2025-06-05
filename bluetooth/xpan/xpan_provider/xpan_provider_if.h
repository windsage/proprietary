/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef XPAN_PROVIDER_IF_H
#define XPAN_PROVIDER_IF_H

#include "XpanProviderService.h"
#include "xpan_utils.h"

/* XPAN Provider Commands Opcodes */
enum {
  XPAN_BONDED_DEVICES_CMD,
  XPAN_UPDATE_TRANSPORT_CMD,
  XPAN_ENABLE_SAP_ACS_CMD,
  XPAN_UPDATE_BEARER_PREPARED_CMD,
  XPAN_UPDATE_TWT_SESSION_PARAMS_CMD,
  XPAN_UPDATE_BEARER_SWITCHED_CMD,
  XPAN_UPDATE_HOST_PARAMS_CMD,
  XPAN_UPDATE_LOW_POWER_MODE_CMD,
  XPAN_UPDATE_SAP_STATE_CMD,
  XPAN_UPDATE_VBC_PERIODICITY_CMD,
  XPAN_CREATE_SAP_INTERFACE_CMD,
  XPAN_UPDATE_WIFI_SCAN_STARTED,
  XPAN_UPDATE_BEARER_PREFERENCE_CMD,
  XPAN_UPDATE_CTS_REQUEST_CMD,
  XPAN_UPDATE_AIRPLANE_STATE_CHANGED,
  XPAN_WIFI_TRANSPORT_PREFERENCE_RES,
  CONNECT_LE_LINK,
  UPDATE_AP_DETAILS_LOCAL,
  UPDATE_AP_DETAILS_REMOTE,
  GET_PORT_DETAILS,
  UPDATE_MDNS_STATUS,
  UPDATE_BOND_STATE,
  UPDATE_MDNS_RECONRD,
  SET_AP_AVB_REQ,
  UPDATE_CONNECTED_EB_DETAILS,
};

/* XPAN Provider Callback Events Opcodes */
enum {
  XPAN_DEVICES_FOUND_EVT,
  XPAN_USECASE_UPDATE_EVT,
  XPAN_ACS_UPDATE_EVT,
  XPAN_PREPARE_BEARER_EVT,
  XPAN_TWT_SESSION_ESTABLISHED_EVT,
  XPAN_SAP_LOW_POW_MODE_UPDATE,
  XPAN_BEARER_SWITCH_INDICATION,
  XPAN_SAP_INTERFACE_CREATED,
  XPAN_UPDATE_BEARER_PREFERENCE_RES,
  XPAN_SSR_WIFI,
  XPAN_WIFI_TRANSPORT_PREFERENCE_REQ,
  SEARCH_MDNS,
  MDNS_REGISTER_UNREGISTER,
  START_FILTERED_SCAN,
  ESTABLISHED_LE_LINK,
  RES_PORT_DETAILS,
  RES_CURRENT_TRANSPORT,
  ROLE_SWITCH_INDICATION,
  AP_AVB_RES,
  SAP_CHANNEL_SWITCH_STARTED,
  VEN_DISABLED,
};

typedef struct {
  macaddr_t mac_addr;
  uint32_t SI;
  uint32_t SP;
  uint32_t location;
  bool isEstablished;
} __attribute__((packed)) tXPAN_Twt_Session_Params;

typedef struct {
  macaddr_t bssid;
  macaddr_t randomMac;
  ipaddr_t ipAddr;
  mdns_uuid_t mdnsUuid;
  uint32_t freq;
} __attribute__((packed)) tXPAN_Local_Params;

static inline char *ConvertRawAddress(bdaddr_t addr)
{
  static char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr.b[5],
    addr.b[4], addr.b[3], addr.b[2], addr.b[1],
    addr.b[0]);
  buf[17] = '\0';
  return buf;
}

static inline char *ConvertRawAddress(macaddr_t addr)
{
  static char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr.b[5],
    addr.b[4], addr.b[3], addr.b[2], addr.b[1],
    addr.b[0]);
  buf[17] = '\0';
  return buf;
}

/* Bluetooth Address length */
#define BD_ADDR_LENGTH 6

/* MAC Address length */
#define MAC_ADDR_LENGTH 6

/* IPV4 Address length */
#define IPV4_ADDR_LENGTH 4

/* IPV6 Address length */
#define IPV6_ADDR_LENGTH 16

/* MDNS UUID length */
#define MDNS_UUID_LENGTH 16

/* Opcode index */
#define OPCODE_INDEX 0

#define LENGTH_WITHOUT_DATA 1

/* To update bonded xpan devices */
typedef void (XM_UpdateXpanBondedDevices) (uint8_t numOfDevices, bdaddr_t devices[]);

/* Used to indicate when a given transport is enabled/disabled in upper layers */
typedef void (XM_UpdateTransport) (bdaddr_t addr, TransportType transport,
              bool isEnabled, uint8_t reason);

/* Used to start ACS algorith for SAP */
typedef void (XM_EnableSapAcs) (std::vector<uint32_t> freqList);

/* Received when remote device responds to Bearer Switch request */
typedef void (XM_UpdateBearerSwitched) (bdaddr_t addr, uint8_t bearer,
                                        uint8_t status);

/* API used to get Twt Session details with remote device details */
typedef void (XM_UpdateTwtSessionParams) (uint32_t rightOffset, uint8_t periodicity,
                                          uint8_t num_devices,
                                          std::vector<tXPAN_Twt_Session_Params> twt_params);

/* Indication from Xpan Profile that Bearer has been prepared */
typedef void (XM_UpdateBearerPrepared) (bdaddr_t addr, uint8_t bearer,
                                        uint8_t status);

/* To update SAP Lower Mode status */
typedef void (XM_UpdateLowPowerMode) (uint8_t dialogId, uint8_t mode);

/* To update Host Params */
typedef void (XM_UpdateHostParams) (macaddr_t mac, uint16_t etherType);

/* To update SAP state */
typedef void (XM_UpdateSapState) (uint16_t sapState);

/* To update Voice back channel periodicity */
typedef void (XM_UpdateVbcPeriodicity) (uint16_t periodicity, bdaddr_t addr);

/* To update WHC transition from EBs */
typedef void (XM_UpdateWifiScanStarted) (bdaddr_t addr, uint8_t state);

/* To create SAP interface */
typedef void (XM_CreateSapInterface) (uint8_t state);

/* Received Bearer preference indication from peer */
typedef void (XM_BearerPreferenceReq) (bdaddr_t addr, uint8_t bearer, uint8_t requestor);

/* Received Clear to send indication from peer */
typedef void (XM_ClearToSendReq) (bdaddr_t addr, uint8_t req);

/* Airplane Mode State changed */
typedef void (XM_AirplaneModeStateChanged) (uint8_t state);

/* Wifi Transport bearer switch response */
typedef void (XM_WifiTransportPreferenceRsp) (uint8_t bearer, uint8_t reponse);

/* Established LE Link */
typedef void XM_ConnectLeLink(bdaddr_t addr);

/* Updated Connected Access point Details */
typedef void XM_UpdateApDetailsLocal(tXPAN_Local_Params local_params);

/* Updated Remote Ap details */
typedef void XM_UpdateApDetailsRemote(tXPAN_Remote_Params remote_params);

/* Request L2cap TCP and UDP Port */
typedef void XM_GetPortDetails();

/* Update MDNS State */
typedef void XM_UpdateMdnsStatus(bdaddr_t, uint8_t, mdns_uuid_t, uint8_t);

/* Update Bonded State */
typedef void XM_UpdateBondState(bdaddr_t, uint8_t);

/* Set Ap available request */
typedef void (XM_SetApAvailableReq) (bdaddr_t addr, uint32_t duration);

/* Update eb Connection details */
typedef void (XM_UpdateConnectedEbDetails) (uint8_t set_id, macaddr_t left, macaddr_t right);

/* Xpan Manager interface API's */
typedef struct {
  XM_UpdateXpanBondedDevices* update_bonded_dev = NULL;
  XM_UpdateTransport* update_transport = NULL;
  XM_EnableSapAcs* enable_acs = NULL;
  XM_UpdateBearerSwitched* update_bearer_switched = NULL;
  XM_UpdateTwtSessionParams* update_twt_params = NULL;
  XM_UpdateBearerPrepared* update_bearer_prepared = NULL;
  XM_UpdateLowPowerMode* update_low_power_mode = NULL;
  XM_UpdateHostParams* update_host_params = NULL;
  XM_UpdateSapState* update_sap_state = NULL;
  XM_UpdateVbcPeriodicity* update_vbc_periodcity = NULL;
  XM_CreateSapInterface* create_sap_interface = NULL;
  XM_BearerPreferenceReq* update_bearer_preference_req = NULL;
  XM_ClearToSendReq* update_clear_to_send_req = NULL;
  XM_AirplaneModeStateChanged* airplane_mode_state_changed = NULL;
  XM_ConnectLeLink* connect_le_link = NULL;
  XM_UpdateApDetailsLocal* update_ap_details_local = NULL;
  XM_UpdateApDetailsRemote* update_ap_details_remote = NULL;
  XM_GetPortDetails* get_port_details = NULL;
  XM_UpdateMdnsStatus* update_mdns_status = NULL;
  XM_UpdateBondState* update_bond_state = NULL;
  XM_WifiTransportPreferenceRsp* wifi_transport_preferece_res = NULL;
  XM_SetApAvailableReq* set_apavailable_req = NULL;
  XM_UpdateConnectedEbDetails* update_connected_eb_details = NULL;
  XM_UpdateWifiScanStarted* update_wifi_scan_started = NULL;
} tXPAN_MANAGER_API;

namespace bluetooth {
namespace xpanprovider {

class XpanProviderIf {
 public:
  virtual ~XpanProviderIf() = default;;
  static void Initialize(
      aidl::vendor::qti::hardware::bluetooth::xpanprovider::XpanProviderService* service);
  static XpanProviderIf* GetIf();
  virtual void RegisterXpanManagerIf (tXPAN_MANAGER_API *xmIf) = 0;
  virtual void DeregisterXpanManagerIf () = 0;
  virtual void XpanDeviceFoundCb (bdaddr_t bdAddr) = 0;
  virtual void UsecaseUpdateCb (bdaddr_t bdAddr, UseCaseType usecase) = 0;
  virtual void PrepareBearerCb (bdaddr_t bdAddr, uint8_t bearer) = 0;
  virtual void TwtSessionEstablishedCb (macaddr_t macAddress, uint32_t sp,
                                        uint32_t si, uint8_t eventType) = 0;
  virtual void AcsUpdateCb (uint8_t status, uint32_t frequency) = 0;
  virtual void SapLowPowerModeUpdateCb (uint8_t id, uint16_t power_save_bi_multiplier,
                                        uint64_t nextTsf) = 0;
  virtual void BearerSwitchIndicationCb (bdaddr_t bdAddr, uint8_t bearerType, uint8_t status) = 0;
  virtual void SapInterfaceCreatedCb (uint8_t state, uint8_t status) = 0;
  virtual void BearerPreferenceResCb (bdaddr_t bdAddr, uint8_t bearer, uint8_t status,
                                                uint8_t requestor) = 0;
  virtual void SsrWifiCb (uint8_t state) = 0;
  virtual void WifiTransportPreferenceCb (uint8_t state) = 0;
  virtual void LeLinkEstablishedCb(bdaddr_t bdAddr, uint8_t status) = 0;
  virtual void PortDetailsCb(uint16_t portL2capTcp, uint16_t portUdpAudio, uint16_t portUdpReports) = 0;
  virtual void MdnsSearchCb(bdaddr_t bdAddr, uint8_t state) = 0;
  virtual void MdnsRegisterUnRegister(bdaddr_t bdAddr, uint8_t state) = 0;
  virtual void FilteredScanCb(bdaddr_t bdAddr, uint8_t state) = 0;
  virtual void CurrentTransportCb(bdaddr_t bdAddr, uint8_t transport) = 0;
  virtual void RoleSwitchIndicationCb(bdaddr_t bdAddr, macaddr_t primary, macaddr_t secondary) = 0;

  virtual void WifiAPAvbRsp (bdaddr_t bdAddr, uint8_t state) = 0;
  virtual void SapChannelSwitchStarted(uint32_t freq, uint64_t tsf, uint8_t bw) = 0;
};

} // namespace bluetooth
} // namespace xpanprovider

#endif
