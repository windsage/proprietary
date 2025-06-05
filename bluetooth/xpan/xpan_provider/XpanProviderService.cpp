/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include <android/binder_auto_utils.h>
#include <android-base/logging.h>
#include <vector>
#include "XpanProviderService.h"
#include "xpan_provider_if.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.hardware.bluetooth.xpanprovider"

using aidl::vendor::qti::hardware::bluetooth::xpanprovider::XpanProviderService;
using bluetooth::xpanprovider::XpanProviderIf;

tXPAN_MANAGER_API *xm_intf = NULL;

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth {
namespace xpanprovider {

/* Used to register Xpan Profile Callback */
::ndk::ScopedAStatus XpanProviderService::registerXpanCallbacks(
        const std::shared_ptr<IXpanProviderCallback>& in_cb) {
  ALOGD("%s", __func__);
  xpanProviderCb = in_cb;
  if (xm_intf == NULL && in_cb != NULL) {
    ALOGD("%s XM not ready", __func__);
    std::vector<uint8_t> data;
    data.push_back(VEN_DISABLED);
    sendXpanEvent(data);
    return ::ndk::ScopedAStatus::ok();
  }

  AIBinder_DeathRecipient* deathRecipient = AIBinder_DeathRecipient_new(
      XpanProviderService::clientDeathRecipient);

  if (deathRecipient != NULL && in_cb != NULL) {
    auto status = AIBinder_linkToDeath(in_cb->asBinder().get(), deathRecipient,
                                       reinterpret_cast<void*>(this));
    if (status != STATUS_OK) {
      ALOGE("%s: Failed to register DeathRecipient with error(%d)", __func__, status);
      // no action needed
    }
  } else {
      ALOGE("%s: Failed ", __func__);
  }

  return ::ndk::ScopedAStatus::ok();
}

/* This API receives XPAN command from Xpan Profile and parses it */
::ndk::ScopedAStatus XpanProviderService::sendXpanCommand(
    const std::vector<uint8_t>& cmd) {
  if (cmd.size() < LENGTH_WITHOUT_DATA) {
    ALOGE("%s: Invalid data received.", __func__);
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  uint8_t opcode = (uint8_t)cmd[OPCODE_INDEX];

  std::vector<uint8_t> cmd_data{};
  if (cmd.size() != LENGTH_WITHOUT_DATA) {
    cmd_data.insert(cmd_data.begin(), cmd.begin() + 1, cmd.end());
  }

  switch(opcode) {
    case XPAN_BONDED_DEVICES_CMD:
        updateXpanBondedDevices(cmd_data);
        break;

    case XPAN_UPDATE_TRANSPORT_CMD:
        updateTransport(cmd_data);
        break;

    case XPAN_ENABLE_SAP_ACS_CMD:
        enableSapAcs(cmd_data);
        break;

    case XPAN_UPDATE_BEARER_PREPARED_CMD:
        updateBearerPrepared(cmd_data);
        break;

    case XPAN_UPDATE_TWT_SESSION_PARAMS_CMD:
        updateTwtSessionParams(cmd_data);
        break;

    case XPAN_UPDATE_BEARER_SWITCHED_CMD:
        updateBearerSwitched(cmd_data);
        break;

    case XPAN_UPDATE_HOST_PARAMS_CMD:
        updateHostParams(cmd_data);
        break;

    case XPAN_UPDATE_LOW_POWER_MODE_CMD:
        updateLowPowerMode(cmd_data);
        break;

    case XPAN_UPDATE_SAP_STATE_CMD:
        updateSapState(cmd_data);
        break;

    case XPAN_UPDATE_VBC_PERIODICITY_CMD:
        updateVbcPeriodicity(cmd_data);
        break;

    case XPAN_UPDATE_WIFI_SCAN_STARTED:
        updateWifiScanStarted(cmd_data);
        break;

    case XPAN_CREATE_SAP_INTERFACE_CMD:
        createSapInterface(cmd_data);
        break;

    case XPAN_UPDATE_BEARER_PREFERENCE_CMD:
        updateBearerPreferenceReq(cmd_data);
        break;

    case XPAN_UPDATE_CTS_REQUEST_CMD:
        updateClearToSendReq(cmd_data);
        break;

    case XPAN_UPDATE_AIRPLANE_STATE_CHANGED:
        updateAirplaneModeChanged(cmd_data);
        break;

    case XPAN_WIFI_TRANSPORT_PREFERENCE_RES:
        updateWifiTransportPreference(cmd_data);
        break;
    case CONNECT_LE_LINK:
        connectLeLink(cmd_data);
        break;

    case UPDATE_AP_DETAILS_LOCAL:
        updateApDetailsLocal(cmd_data);
        break;

    case UPDATE_AP_DETAILS_REMOTE:
        updateApDetailsRemote(cmd_data);
        break;

    case GET_PORT_DETAILS:
        getPortDetails(cmd_data);
        break;

    case UPDATE_MDNS_STATUS:
        updateMdnsStatus(cmd_data);
        break;

    case UPDATE_BOND_STATE:
        updateBondState(cmd_data);
        break;

    case UPDATE_MDNS_RECONRD:
        updatemDnsRecord(cmd_data);
        break;

    case SET_AP_AVB_REQ:
        setApAvailableReq(cmd_data);
        break;

     case UPDATE_CONNECTED_EB_DETAILS:
         updateConnectedEbDetails(cmd_data);
         break;

    default:
        ALOGW("%s: Unhandled Command(%02x) Received", __func__, opcode);
  }

  return ::ndk::ScopedAStatus::ok();
}


XpanProviderService::XpanProviderService() {
  XpanProviderIf::Initialize(this);
}

XpanProviderService::~XpanProviderService() {
  xpanProviderCb = NULL;
}

/* Xpan Profile death recipient */
void XpanProviderService::clientDeathRecipient(void* cookie) {
  ALOGD("%s: Xpan Provider Client (Profile) died..", __func__);

  auto* svc = static_cast<XpanProviderService*>(cookie);
  svc->xpanProviderCb = NULL;
}

/* API to receive Bonded XPan Devices from Profile */
void XpanProviderService::updateXpanBondedDevices(const std::vector<uint8_t>& data) {
  uint8_t bytesParsed = 0;

  if (data.empty()) {
    ALOGE("%s: Empty data received for the command", __func__);
    return;
  }

  // expectedLength = numOfDevices (1) + numOfDevices x 6
  uint8_t numOfDevices = data[bytesParsed++];
  uint16_t remLength = numOfDevices * BD_ADDR_LENGTH;

  if (remLength != data.size() - 1) {
    ALOGE("%s:Invalid length(%d) data received", __func__, remLength);
    return;
  }

  bdaddr_t* list = new bdaddr_t[numOfDevices];
  for (int i = 0; i < numOfDevices; i++) {
    std::copy(data.begin() + bytesParsed, data.begin()
              + bytesParsed + BD_ADDR_LENGTH, list[i].b);
    bytesParsed += BD_ADDR_LENGTH;
  }

  ALOGD("%s: numOfDevices = %d", __func__, numOfDevices);
  for (int i = 0; i < numOfDevices; i++) {
    ALOGD("%s: Device %d  %s ", __func__, (i + 1), ConvertRawAddress(list[i]));
  }

  if (xm_intf && xm_intf->update_bonded_dev) {
    ALOGD("%s: xm_intf = %p", __func__, xm_intf);
    xm_intf->update_bonded_dev(numOfDevices, list);
  }
  //TODO: release list
}

/* Used to indicate when a given transport is enabled/disabled in upper layers */
void XpanProviderService::updateTransport(const std::vector<uint8_t>& data) {
  /* expectedLength =  address(6) + transport(1)
                      + isEnabled(1) + reason (1) */
  uint8_t bytesParsed = 0, expectedLength = 9;
  bdaddr_t addr;
  TransportType transport;
  uint8_t reason;
  bool isEnabled;

  if (data.size() != expectedLength) {
    ALOGE("%s:Invalid length(%d) data received", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  transport = static_cast<TransportType>(data[bytesParsed++]);
  isEnabled = (bool)data[bytesParsed++];
  reason = data[bytesParsed];

  ALOGD("%s: transport = %d, isEnabled = %d, reason = %d"
        " address = %s ", __func__
        , transport, isEnabled, reason, ConvertRawAddress(addr));

  if (xm_intf && xm_intf->update_transport) {
    xm_intf->update_transport(addr, transport, isEnabled, reason);
  }
}

/* Used to start ACS algorith for SAP */
void XpanProviderService::enableSapAcs(const std::vector<uint8_t>& data) {
  /* expectedLength = (4*list_size) + freqListSize(1) */
  uint8_t bytesParsed = 0;

  if (data.empty() || (data.size() < sizeof(uint32_t))) {
    ALOGE("%s:Invalid length(%d) data received", __func__, data.size());
    return;
  }

  uint32_t freqListSize = data[bytesParsed++];
  freqListSize |= (data[bytesParsed++] << 8);
  freqListSize |= (data[bytesParsed++] << 16);
  freqListSize |= (data[bytesParsed++] << 24);

  std::vector<uint32_t> freqList;

  if (data.size() - sizeof(uint32_t) != sizeof(uint32_t) * freqListSize) {
    ALOGE("%s: Incorrect frequency list received", __func__);
    return;
  }

  for (int i = 0; i < freqListSize; i++) {
    uint32_t freq = 0;
    freq = freq | (data[bytesParsed++]);
    freq = freq | (data[bytesParsed++] << 8);
    freq = freq | (data[bytesParsed++] << 16);
    freq = freq | (data[bytesParsed++] << 24);
    freqList.push_back(freq);
  }

  ALOGD("%s: freqListSize = %d", __func__, freqListSize);

  if (xm_intf && xm_intf->enable_acs) {
    xm_intf->enable_acs(freqList);
  }
}

/* Received when remote device responds to Bearer Switch request */
void XpanProviderService::updateBearerSwitched(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + bearer(1) + status (1)*/
  uint8_t bytesParsed = 0, expectedLength = 8;
  bdaddr_t addr;
  uint8_t bearer, status;

  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  bearer = data[bytesParsed++];
  status = data[bytesParsed];

  ALOGD("%s: address %s , bearer = %d status = %d", __func__,
        ConvertRawAddress(addr), bearer, status);

  if (xm_intf && xm_intf->update_bearer_switched) {
    xm_intf->update_bearer_switched(addr, bearer, status);
  }
}

/* API used to get Twt Session details with remote device details */
void XpanProviderService::updateTwtSessionParams(const std::vector<uint8_t>& data) {
  /* expectedLength = numOfDevices x sizeof(tXPAN_Twt_Session_Params) */
  uint8_t bytesParsed = 0, numOfDevices = 0, periodicity = 0, minLength;
  uint32_t rightOffset = 0;
  std::vector<tXPAN_Twt_Session_Params> remoteDevicesTwtParams;
  /* minLength = rightoffset(4) + periodicity (1) + numOfDevice(1) */
  minLength = sizeof(uint32_t) + (sizeof(uint8_t) * 2);
  if (data.empty() || (data.size() < minLength)) {
    ALOGE("%s: Incorrect parameters length (%d) required %d ", __func__, data.size(), minLength);
    return;
  }
  rightOffset = (data[bytesParsed]) | (data[bytesParsed + 1] << 8) |
                       (data[bytesParsed + 2] << 16) | (data[bytesParsed + 3] << 24);
  bytesParsed += sizeof(uint32_t);
  periodicity = data[bytesParsed++];
  numOfDevices = data[bytesParsed++];
  ALOGD("%s: numOfDevices (%d), rightOffset, (%d) periodicity (%d)",
      __func__, numOfDevices, rightOffset, periodicity);

  if (numOfDevices != 0
       && (data.size() - 6 != (numOfDevices * sizeof(tXPAN_Twt_Session_Params)))) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  for (int i = 0; i < numOfDevices; i++) {
    tXPAN_Twt_Session_Params twtParams;
    std::copy(data.begin() + bytesParsed,
              data.begin() + bytesParsed + sizeof(macaddr_t),
              twtParams.mac_addr.b);
    bytesParsed += sizeof(macaddr_t);
    twtParams.location = (data[bytesParsed]) | (data[bytesParsed + 1] << 8) |
                         (data[bytesParsed + 2] << 16) | (data[bytesParsed + 3] << 24);
    bytesParsed += sizeof(uint32_t);
    twtParams.isEstablished = data[bytesParsed++];
    twtParams.SI = (data[bytesParsed]) | (data[bytesParsed + 1] << 8) |
         (data[bytesParsed + 2] << 16) | (data[bytesParsed + 3] << 24);
    bytesParsed += sizeof(uint32_t);
    twtParams.SP = (data[bytesParsed]) | (data[bytesParsed + 1] << 8) |
         (data[bytesParsed + 2] << 16) | (data[bytesParsed + 3] << 24);
    bytesParsed += sizeof(uint32_t);

    ALOGD("%s: mac (%s), location = %d, "
          "isEstablished = %d, SI = %d, SP = %d", __func__,
          ConvertRawAddress(twtParams.mac_addr),
          twtParams.location, twtParams.isEstablished, twtParams.SI, twtParams.SP);
    remoteDevicesTwtParams.push_back(twtParams);
  }

  if (xm_intf && xm_intf->update_twt_params) {
    xm_intf->update_twt_params(rightOffset, periodicity, numOfDevices, remoteDevicesTwtParams);
  }
}

/* Indication from Xpan Profile that Bearer has been prepared */
void XpanProviderService::updateBearerPrepared(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + bearer(1) + status(1)*/
  uint8_t bytesParsed = 0, expectedLength = 8;
  bdaddr_t addr;
  uint8_t bearer, status;

  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  bearer = data[bytesParsed++];
  status = data[bytesParsed];

  ALOGD("%s: address(%s), bearer = %d, status = %d", __func__,ConvertRawAddress(addr), bearer, status);

  if (xm_intf && xm_intf->update_bearer_prepared) {
    xm_intf->update_bearer_prepared(addr, bearer, status);
  }
}

/* To update SAP Lower Mode status */
void XpanProviderService::updateLowPowerMode(const std::vector<uint8_t>& data) {
  /* expectedLength = dialog_id(1) + status(1) */
  uint8_t expectedLength = 2, bytesParsed = 0, dialogId, mode;

  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  dialogId = data[bytesParsed++];
  mode = data[bytesParsed];

  if (xm_intf && xm_intf->update_low_power_mode) {
    xm_intf->update_low_power_mode(dialogId, mode);
  }
}

/* To update device SoftAp Mac Address and Ether type */
void XpanProviderService::updateHostParams(const std::vector<uint8_t>& data) {
  /* expectedLength = etherType(2) + macAddr(6)  */
  uint8_t bytesParsed = 0;
  uint16_t etherType = 0;
  macaddr_t macAddr;

  if (data.size() != sizeof(macaddr_t) + sizeof(uint16_t)) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + sizeof(macaddr_t), macAddr.b);
  bytesParsed += sizeof(macaddr_t);
  etherType = (data[bytesParsed]) | (data[bytesParsed + 1] << 8);

  if (xm_intf && xm_intf->update_host_params) {
    xm_intf->update_host_params(macAddr, etherType);
  }
}

/* To update device Device SAP state */
void XpanProviderService::updateSapState(const std::vector<uint8_t>& data) {
  /* expectedLength = state(1) */
  uint8_t bytesParsed = 0;

  if (data.size() !=  sizeof(uint8_t)) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  uint8_t state = (data[bytesParsed]) | (data[bytesParsed + 1] << 8);

  if (xm_intf && xm_intf->update_sap_state) {
    xm_intf->update_sap_state(state);
  }
}

/* To update device Voice back channel periodicity */
void XpanProviderService::updateVbcPeriodicity(const std::vector<uint8_t>& data) {
  /* expectedLength = Periodcity channel(1) + address(6)*/
  uint8_t bytesParsed = 0;
  uint8_t expectedLength = 7;
  bdaddr_t addr;
  if (data.size() !=  expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  uint8_t periodcity = (data[bytesParsed]) | (data[bytesParsed + 1] << 8);
  std::copy(data.begin()+1, data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;

  ALOGD("%s: address( %s ), periodcity = %d, ", __func__,
        ConvertRawAddress(addr), periodcity);

  if (xm_intf && xm_intf->update_vbc_periodcity) {
    xm_intf->update_vbc_periodcity(periodcity, addr);
  }
}

/* To update WHC transition from EBs */
void XpanProviderService::updateWifiScanStarted(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + state(1) */
  uint8_t bytesParsed = 0;
  uint8_t expectedLength = 7;
  bdaddr_t addr;

  if (data.size() !=  expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  uint8_t state = data[bytesParsed];

  ALOGD("%s  %s  %d, ", __func__,  ConvertRawAddress(addr), state);

  if (xm_intf && xm_intf->update_wifi_scan_started) {
    xm_intf->update_wifi_scan_started(addr, state);
  }
}

/* To create SAP Interface for XPAN use case */
void XpanProviderService::createSapInterface(const std::vector<uint8_t>& data) {
  ALOGD("%s", __func__);

  /* expectedLength = state(1) */
  uint8_t bytesParsed = 0;

  if (data.size() !=  sizeof(uint8_t)) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  uint8_t isCreate = data[bytesParsed];

  if (xm_intf && xm_intf->create_sap_interface) {
    xm_intf->create_sap_interface(isCreate);
  }
}

/* Indication from Xpan Profile that Received Bearer preference from Peer */
void XpanProviderService::updateBearerPreferenceReq(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + requestor (1) + bearer(1)*/
  uint8_t bytesParsed = 0, expectedLength = 8;
  bdaddr_t addr;
  uint8_t requestor, bearer;

  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  requestor = data[bytesParsed++];
  bearer = data[bytesParsed];
  ALOGD("%s: address(%02X:%02X:%02X:%02X:%02X:%02X), bearer = %d  requestor = %d ", __func__,
        addr.b[5], addr.b[4], addr.b[3], addr.b[2],
        addr.b[1], addr.b[0], bearer, requestor);

  if (xm_intf && xm_intf->update_bearer_preference_req) {
    xm_intf->update_bearer_preference_req(addr, bearer, requestor);
  }
}

/* Indication from Xpan Profile that Received Clear to send request from Peer */
void XpanProviderService::updateClearToSendReq(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + state(1)*/
  uint8_t bytesParsed = 0, expectedLength = 7;
  bdaddr_t addr;
  uint8_t reqStatus;

  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  reqStatus = data[bytesParsed];

  ALOGD("%s: address(%s), reqStatus = %d ", __func__,
        ConvertRawAddress(addr), reqStatus);

  if (xm_intf && xm_intf->update_clear_to_send_req) {
    xm_intf->update_clear_to_send_req(addr, reqStatus);
  }
}

/* Indication from Xpan Profile that Received Airplande Mode State changed */
void XpanProviderService::updateAirplaneModeChanged(const std::vector<uint8_t>& data) {
  /* expectedLength = state(1)*/
  uint8_t bytesParsed = 0, expectedLength = 1;
  uint8_t state;
  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  state = data[bytesParsed];
  ALOGD("%s:  state (%d)", __func__, state);
  if (xm_intf && xm_intf->airplane_mode_state_changed) {
    xm_intf->airplane_mode_state_changed(state);
  }
}

/* Indication from Xpan Profile that Received Wifi tranport preference response*/
void XpanProviderService::updateWifiTransportPreference(const std::vector<uint8_t>& data) {
  /* expectedLength = bearer(1) + status (1) */
  uint8_t bytesParsed = 0, expectedLength = 2;
  uint8_t bearer, status;
  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  bearer = data[bytesParsed++];
  status = data[bytesParsed];
  ALOGD("%s: bearer (%d) state (%d)", __func__, bearer, status);

  if (xm_intf && xm_intf->wifi_transport_preferece_res) {
    xm_intf->wifi_transport_preferece_res(bearer, status);
  }
}

/* Initiate Le Connection */
void XpanProviderService::connectLeLink(const std::vector<uint8_t>& data){
  /* expectedLength =  address(6)*/
  uint8_t expectedLength = 6;
  bdaddr_t addr;

  if (data.size() != expectedLength) {
    ALOGE("%s:Invalid length(%d) data received", __func__, data.size());
    return;
  }

  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  ALOGD("%s: address = %s ", __func__ , ConvertRawAddress(addr));

  if (xm_intf && xm_intf->connect_le_link) {
    xm_intf->connect_le_link(addr);
  }

}

void copy(const std::vector<uint8_t> data, uint8_t &startOffSet,
		uint8_t stopOffSet, uint8_t dest[]) {
  if (data.size() >= (startOffSet + stopOffSet)) {
    std::copy(data.begin() + startOffSet,
	    (data.begin() + (startOffSet + stopOffSet)), dest);
  } else {
    ALOGE("%s: Incorrect parameters data (%d)  stopOffSet %d stopOffSet %d",
        __func__, data.size(), startOffSet, stopOffSet);
  }
  startOffSet += stopOffSet;
}

/* Update Ap details which MTP connected*/
void XpanProviderService::updateApDetailsLocal(const std::vector<uint8_t>& data){
  /* expectedLength =  bssid (6) + randommac(6) + iptype (1) + ipaddr (4 or 16 ) + uuid local ( 16) + freq (4)*/
  uint8_t bytesParsed = 0, expectedLength = 34;

  if (data.size() < expectedLength) {
    ALOGE("%s:Invalid length(%d) data received", __func__, data.size());
    return;
  }

  tXPAN_Local_Params localParams;
  ALOGD("%s: (%d) ", __func__, data.size());
  copy(data, bytesParsed, MAC_ADDR_LENGTH, localParams.bssid.b);
  copy(data, bytesParsed, MAC_ADDR_LENGTH, localParams.randomMac.b);

  localParams.ipAddr.type = (IpAddrType)data[bytesParsed++];
  uint8_t ipSize = IPV4_ADDR_LENGTH;
  if (localParams.ipAddr.type == IPv6) {
    ipSize = IPV6_ADDR_LENGTH;
    copy(data, bytesParsed, IPV6_ADDR_LENGTH, localParams.ipAddr.ipv6);
  } else {
    copy(data, bytesParsed, IPV4_ADDR_LENGTH, localParams.ipAddr.ipv4);
  }
  copy(data, bytesParsed, MDNS_UUID_LENGTH, localParams.mdnsUuid.b);
  uint32_t freq = 0;
  freq = freq | (data[bytesParsed++]);
  freq = freq | (data[bytesParsed++] << 8);
  freq = freq | (data[bytesParsed++] << 16);
  freq = freq | (data[bytesParsed++] << 24);

  localParams.freq = freq;
  ALOGD("%s: bssid %s randommac %s ipaddr %s mdns %s freq %d", __func__,
      ConvertRawAddress(localParams.bssid),
	  ConvertRawAddress(localParams.randomMac),
	  localParams.ipAddr.toString().c_str(),
	  localParams.mdnsUuid.toString().c_str(), freq);

    if (xm_intf && xm_intf->update_ap_details_local) {
      xm_intf->update_ap_details_local(localParams);
    }
}

/* Update Ap details which Earbud connected*/
void XpanProviderService::updateApDetailsRemote(const std::vector<uint8_t>& data){
/* Minimum expectedLength =  BtAddr(6) + Mdns (1) + Mdns uuid (16) + l2capTcpPort (2) + udpPortAudio(2)
         * + udpPortReports(2) + numdevices (1)*/
  uint8_t bytesParsed = 0, minLength = 30, numOfDevices = 0;
  if (data.empty() || minLength > data.size()) {
    int8_t size = (data.empty()) ? 0 : data.size();
    ALOGE("%s: Invalid size (%d) ", __func__,size);
    return;
  }

  tXPAN_Remote_Params remoteParams;
  copy(data, bytesParsed, BD_ADDR_LENGTH, remoteParams.addr.b);
  remoteParams.mdns = data[bytesParsed++];
  copy(data, bytesParsed, MDNS_UUID_LENGTH, remoteParams.mdnsUuid.b);
  remoteParams.portL2capTcp = ((data[bytesParsed]) | (data[bytesParsed + 1] << 8));
  bytesParsed += 2;
  remoteParams.portUdpAudio = ((data[bytesParsed]) | (data[bytesParsed + 1] << 8));
  bytesParsed += 2;
  remoteParams.portUdpReports = ((data[bytesParsed]) | (data[bytesParsed + 1] << 8));
  bytesParsed += 2;
  numOfDevices = data[bytesParsed++];
  ALOGD("%s bdaddr %s portL2capTcp %d  portUdpAudio %d portUdpReports %d numOfDevices %d",
    __func__, ConvertRawAddress(remoteParams.addr), remoteParams.portL2capTcp,
    remoteParams.portUdpAudio, remoteParams.portUdpReports, numOfDevices);
  remoteParams.numOfEbs = numOfDevices;
  for (int i = 0; i < numOfDevices; i++) {

    tXPAN_Eb_Params ebParams;
    copy(data, bytesParsed, MAC_ADDR_LENGTH, ebParams.mac_bssid.b);
    ebParams.ipAddr.type = (IpAddrType)data[bytesParsed++];
    uint8_t ipSize = IPV4_ADDR_LENGTH;
    if (ebParams.ipAddr.type == IPv6) {
      ipSize = IPV6_ADDR_LENGTH;
      copy(data, bytesParsed, IPV6_ADDR_LENGTH, ebParams.ipAddr.ipv6);
    } else {
      copy(data, bytesParsed, IPV4_ADDR_LENGTH, ebParams.ipAddr.ipv4);
    }

    ebParams.audioLocation = data[bytesParsed++];
    ebParams.role = data[bytesParsed++];
    copy(data, bytesParsed, MAC_ADDR_LENGTH, ebParams.mac_addr.b);
    ALOGD("%s mac_bssid %s ipaddr %s audioLocation %d   role %d mac_addr %s ",
	    __func__, ConvertRawAddress(ebParams.mac_bssid), ebParams.ipAddr.toString().c_str(), ebParams.audioLocation,
		ebParams.role, ConvertRawAddress(ebParams.mac_addr));
    remoteParams.vectorEbParams[i] = ebParams;
  }
  if (xm_intf && xm_intf->update_ap_details_remote) {
    xm_intf->update_ap_details_remote(remoteParams);
  }
}

/* Request L2Cap Tcp Port */
void XpanProviderService::getPortDetails(const std::vector<uint8_t>& data){
  if (!data.empty()) {
    ALOGE("%s: Not Empty ", __func__);
    return;
  }
  ALOGD("%s:", __func__);

  if (xm_intf && xm_intf->get_port_details) {
    xm_intf->get_port_details();
  }
}

/* Update MDNS Status*/
void XpanProviderService::updateMdnsStatus(const std::vector<uint8_t>& data){
  /* expectedLength =  addr (6) + uuid(16) + state(1) + status(1) */
  uint8_t bytesParsed = 0, expectedLength = 24, status = 0, state = 0;
  bdaddr_t addr;
  mdns_uuid_t uuid;

  if (data.size() != expectedLength) {
    ALOGE("%s:Invalid length(%d) data received", __func__, data.size());
    return;
  }
  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  copy(data, bytesParsed, MDNS_UUID_LENGTH, uuid.b);
  state = data[bytesParsed++];
  status = data[bytesParsed];
  ALOGD("%s addr %s uuid %s state %d status %d", __func__,
        ConvertRawAddress(addr), uuid.toString().c_str(), state, status);

  if (xm_intf && xm_intf->update_mdns_status) {
    xm_intf->update_mdns_status(addr, status, uuid, state);
  }
}

/* To Update Bluetooth device Bond state*/
void XpanProviderService::updateBondState(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + state(1) */
  uint8_t bytesParsed = 0;
  uint8_t expectedLength = 7;
  bdaddr_t addr;

  if (data.size() !=  expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  copy(data, bytesParsed, BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  uint8_t state = data[bytesParsed];

  ALOGD("%s: address(%s), state = %d, ", __func__,
        ConvertRawAddress(addr), state);

  if (xm_intf && xm_intf->update_bond_state) {
    xm_intf->update_bond_state(addr, state);
  }
}

/* To Update MDNS record found */
void XpanProviderService::updatemDnsRecord(const std::vector<uint8_t>& data) {
  /* expectedLength = address(6) + state(1) */
	uint8_t expectedLength = 7;
  if (data.size() !=  expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  ALOGD("%s: ", __func__);
}

/* Indication from Xpan Profile that Received Sap Power State  Request*/
void XpanProviderService::setApAvailableReq(const std::vector<uint8_t>& data) {
  /* expectedLength = BluetoothDevice(6) + duration (1) */
  uint8_t bytesParsed = 0, expectedLength = 10;
  uint32_t duatiron;
  bdaddr_t addr;
  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  std::copy(data.begin(), data.begin() + BD_ADDR_LENGTH, addr.b);
  bytesParsed += BD_ADDR_LENGTH;
  duatiron = (data[bytesParsed]) | (data[bytesParsed + 1] << 8) |
                       (data[bytesParsed + 2] << 16) | (data[bytesParsed + 3] << 24);
  ALOGD("%s: (%02X:%02X:%02X:%02X:%02X:%02X), duatiron = %d ", __func__,
        addr.b[5], addr.b[4], addr.b[3], addr.b[2],
        addr.b[1], addr.b[0], duatiron);

  if (xm_intf && xm_intf->set_apavailable_req) {
    xm_intf->set_apavailable_req(addr, duatiron);
  }
}

/* Indication from Xpan Profile that Received Mac connection updated */
void XpanProviderService::updateConnectedEbDetails(const std::vector<uint8_t>& data) {
  /* expectedLength = setid(1) + mac_left (6) + mac_right (6) */
  uint8_t bytesParsed = 0, expectedLength = 13;
  if (data.size() != expectedLength) {
    ALOGE("%s: Incorrect parameters length (%d)", __func__, data.size());
    return;
  }
  uint8_t set_id = data[bytesParsed++];
  macaddr_t left, right;
  uint8_t mac_size = sizeof(macaddr_t);
  std::copy(data.begin() + bytesParsed, data.begin() + bytesParsed + mac_size, left.b);
  bytesParsed += mac_size;
  std::copy((data.begin()+ bytesParsed), (data.begin() + bytesParsed + mac_size), right.b);
  bytesParsed += mac_size;
  ALOGD("%s: set_id %d left (%02X:%02X:%02X:%02X:%02X:%02X), right (%02X:%02X:%02X:%02X:%02X:%02X)",
          __func__, set_id,
          left.b[0], left.b[1], left.b[2], left.b[3], left.b[4], left.b[5],
          right.b[0], right.b[1], right.b[2],right.b[3], right.b[4], right.b[5]);

  if (xm_intf && xm_intf->update_connected_eb_details) {
    xm_intf->update_connected_eb_details(set_id, left, right);
  }
}

/* This API converts Xpan Manager callback to appropriate Event stream */
void XpanProviderService::sendXpanEvent(const std::vector<uint8_t>& data) {
  ALOGD("%s", __func__);

  if (xpanProviderCb == NULL) {
    ALOGE("%s: XpanProvider Client not bound to the service", __func__);
    return;
  }

  xpanProviderCb->xpanEventReceivedCb(data);
}

} // xpanprovider
} // bluetooth
} // hardware
} // qti
} // vendor
} // aidl

namespace {

/* To translate byte stream to bdaddr_t */
static void addBdAddrToEventData(std::vector<uint8_t> &data, bdaddr_t bdAddr) {
  for (int i = 0; i < BD_ADDR_LENGTH; i++) {
    data.push_back(bdAddr.b[i]);
  }
}

/* To translate byte stream to macaddr_t */
static void addMacAddrToEventData(std::vector<uint8_t> &data, macaddr_t macAddr) {
  for (int i = 0; i < BD_ADDR_LENGTH; i++) {
    data.push_back(macAddr.b[i]);
  }
}

/* To translate uint32_t to byte stream */
static void addUint16ToEventData(std::vector<uint8_t> &data, uint16_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
}

/* To translate uint32_t to byte stream */
static void addUint32ToEventData(std::vector<uint8_t> &data, uint32_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
  data.push_back((uint8_t)(0xFF & (val >> 16)));
  data.push_back((uint8_t)(0xFF & (val >> 24)));
}

/* To translate uint64_t to byte stream */
static void addUint64ToEventData(std::vector<uint8_t> &data, uint64_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
  data.push_back((uint8_t)(0xFF & (val >> 16)));
  data.push_back((uint8_t)(0xFF & (val >> 24)));
  data.push_back((uint8_t)(0xFF & (val >> 32)));
  data.push_back((uint8_t)(0xFF & (val >> 40)));
  data.push_back((uint8_t)(0xFF & (val >> 48)));
  data.push_back((uint8_t)(0xFF & (val >> 56)));
}
} // namespace anonymous

namespace bluetooth {
namespace xpanprovider {

class XpanProviderIfImpl;
XpanProviderIfImpl *instance = NULL;

class XpanProviderIfImpl: public XpanProviderIf {
 public:
  XpanProviderService* service_;

  XpanProviderIfImpl(XpanProviderService* service) {
    service_ = service;
  }

  ~XpanProviderIfImpl() {
    service_ = NULL;
    instance = NULL;
  }

/* Callback from XpanManager when xpan supporting device is connected */
  void RegisterXpanManagerIf(tXPAN_MANAGER_API *xmIf) {
    ALOGD("%s", __func__);
    xm_intf = xmIf;
    ALOGD("%s: xm_intf = %p", __func__, xm_intf);
  }

  void DeregisterXpanManagerIf() {
    ALOGD("%s", __func__);
    xm_intf = NULL;
  }

  /* Callback from XpanManager when xpan supporting device is connected */
  void XpanDeviceFoundCb (bdaddr_t bdAddr) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_DEVICES_FOUND_EVT);
    addBdAddrToEventData(data, bdAddr);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  /* Callback from XpanManager when xpan use case is updated */
  void UsecaseUpdateCb (bdaddr_t bdAddr, UseCaseType usecase) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_USECASE_UPDATE_EVT);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(static_cast<uint8_t>(usecase));

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  /* Callback from XpanManager when bearer needs to be prepared in upper layer */
  void PrepareBearerCb (bdaddr_t bdAddr, uint8_t bearer) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_PREPARE_BEARER_EVT);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(bearer);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  /* Callback from XpanManager when Twt Session is established with remote device */
  void TwtSessionEstablishedCb (macaddr_t macAddress, uint32_t sp,
                                uint32_t si, uint8_t eventType) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_TWT_SESSION_ESTABLISHED_EVT);
    addMacAddrToEventData(data, macAddress);
    addUint32ToEventData(data, sp);
    addUint32ToEventData(data, si);
    data.push_back(eventType);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  /* Callback from XpanManager when ACS algorithm is completed */
  void AcsUpdateCb (uint8_t status, uint32_t frequency) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_ACS_UPDATE_EVT);
    data.push_back(status);
    addUint32ToEventData(data, frequency);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void SapLowPowerModeUpdateCb (uint8_t id, uint16_t power_save_bi_multiplier,
                                uint64_t nextTsf) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_SAP_LOW_POW_MODE_UPDATE);
    data.push_back(id);
    addUint16ToEventData(data, power_save_bi_multiplier);
    addUint64ToEventData(data, nextTsf);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void BearerSwitchIndicationCb (bdaddr_t bdAddr, uint8_t bearerType, uint8_t status) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_BEARER_SWITCH_INDICATION);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(bearerType);
    data.push_back(status);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void SapInterfaceCreatedCb (uint8_t state, uint8_t status) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_SAP_INTERFACE_CREATED);
    data.push_back(state);
    data.push_back(status);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void BearerPreferenceResCb (bdaddr_t bdAddr, uint8_t bearer, uint8_t status, uint8_t requestor) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_UPDATE_BEARER_PREFERENCE_RES);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(requestor);
    data.push_back(bearer);
    data.push_back(status);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void SsrWifiCb (uint8_t state) {
    ALOGD("%s", __func__);

    std::vector<uint8_t> data;
    data.push_back(XPAN_SSR_WIFI);
    data.push_back(state);

    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void WifiTransportPreferenceCb (uint8_t bearer) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(XPAN_WIFI_TRANSPORT_PREFERENCE_REQ);
    data.push_back(bearer);

    if (service_) {
        service_->sendXpanEvent(data);
      }
  }

  void LeLinkEstablishedCb(bdaddr_t bdAddr, uint8_t status) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(ESTABLISHED_LE_LINK);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(status);
    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

void PortDetailsCb(uint16_t portL2capTcp, uint16_t portUdpAudio, uint16_t portUdpReports) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(RES_PORT_DETAILS);
    addUint16ToEventData(data, portL2capTcp);
    addUint16ToEventData(data, portUdpAudio);
    addUint16ToEventData(data, portUdpReports);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void MdnsSearchCb(bdaddr_t bdAddr, uint8_t state) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(SEARCH_MDNS);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(state);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void MdnsRegisterUnRegister(bdaddr_t bdAddr, uint8_t state) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(MDNS_REGISTER_UNREGISTER);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(state);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void FilteredScanCb(bdaddr_t bdAddr, uint8_t state) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(START_FILTERED_SCAN);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(state);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void CurrentTransportCb(bdaddr_t bdAddr, uint8_t transport) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(RES_CURRENT_TRANSPORT);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(transport);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void RoleSwitchIndicationCb(bdaddr_t bdAddr, macaddr_t primary, macaddr_t secondary) {
    ALOGD("%s",__func__);
    std::vector<uint8_t> data;
    data.push_back(ROLE_SWITCH_INDICATION);
    addBdAddrToEventData(data, bdAddr);
    addMacAddrToEventData(data, primary);
    addMacAddrToEventData(data, secondary);

    if (service_) {
      service_->sendXpanEvent(data);
    }
}

void WifiAPAvbRsp (bdaddr_t bdAddr, uint8_t state) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(AP_AVB_RES);
    addBdAddrToEventData(data, bdAddr);
    data.push_back(state);
    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

  void SapChannelSwitchStarted(uint32_t freq, uint64_t tsf, uint8_t bw) {
    ALOGD("%s", __func__);
    std::vector<uint8_t> data;
    data.push_back(SAP_CHANNEL_SWITCH_STARTED);
    addUint16ToEventData(data, freq);
    addUint64ToEventData(data, tsf);
    data.push_back(bw);
    if (service_) {
      service_->sendXpanEvent(data);
    }
  }

};
} // xpanprovider
} // namespace

/* Provides XpanProviderIf to Xpan Manager for giving callback to Xpan Profile*/
XpanProviderIf* XpanProviderIf::GetIf() {
  return instance;
}

void XpanProviderIf::Initialize(XpanProviderService* service) {
  ALOGI("%s", __func__);
  instance = new XpanProviderIfImpl(service);
}
