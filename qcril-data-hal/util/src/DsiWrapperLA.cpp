/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "DsiWrapper.h"
#include <iomanip>
#include <sstream>
#include <cutils/properties.h>
#include <utils/SystemClock.h>

#define ADD_ELAPSED_REAL_TIME(time) ((time == -1) ? (MAX_LINX_ADDRESS_TIMER_VAL) : (android::elapsedRealtime() + (time*1000)))

using namespace rildata;

DsiWrapper& DsiWrapper::getInstance()
{
  static DsiWrapperLA dsiWrapper;
  return dsiWrapper;
}

AddrType convertDsiAddrTypeToInternal(dsi_addr_type dsiAddrType) {
  switch (dsiAddrType) {
     case DSI_ADDR_TYPE_DEFAULT: return AddrType::_eDefault;
     case DSI_ADDR_TYPE_CANDIDATE: return AddrType::_eCandidate;
     case DSI_ADDR_TYPE_MULTIHOME: return AddrType::_eMultihome;
     default: return AddrType::_eInvalid;
  }

}

unsigned int DsiWrapperLA::dsiGetAddresses(dsi_hndl_t dsiHandle, bool &v4Connected, bool &v6Connected,
  PduInfo &primPduInfo, PduInfo &secPduInfo) {
  Log::getInstance().d("[DsiWrapper] dsiGetAddresses");

  int rc = 0;
  char DnsAddr[DS_CALL_INFO_ADDR_IPV4V6_MAX_LEN*2+1];
  char tempStr[DS_CALL_INFO_ADDR_IPV4V6_MAX_LEN+1];

  unsigned int count = dsi_get_ip_addr_count_ex(dsiHandle);
  Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr_count = " + std::to_string(count));

  if (count == 0) {
    return 0;
  }

  dsi_addr_info_ex_t *addr_info = nullptr;
  addr_info = (dsi_addr_info_ex_t *)calloc(count, sizeof(dsi_addr_info_ex_t));
  if (addr_info != nullptr) {
    rc = dsi_get_ip_addr_ex(dsiHandle, addr_info, count);
    if (rc!=0) {
      Log::getInstance().d("[DsiWrapper]: couldn't get address info");
    }


    for (unsigned int i=0 ; i<count ; i++) {
      AddrInfo tempAddrInfo = { };
      IpFamilyType_t currIpType = IpFamilyType_t::NotAvailable;
      memset(tempStr, 0, sizeof(tempStr));
      if (addr_info[i].iface_addr_s.valid_addr) {
        if (addr_info[i].iface_addr_s.addr.ss_family == AF_INET) {
          AddressFormatIPv4WithSubnetMask(SASTORAGE_DATA(addr_info[i].iface_addr_s.addr),
            addr_info[i].iface_mask, tempStr, sizeof(tempStr));
          if (addr_info[i].primary) {
            v4Connected = true;
          }
          currIpType = IpFamilyType_t::IPv4;
          if (addr_info[i].address_type != DSI_ADDR_TYPE_INVALID) {
            Log::getInstance().d("[DsiWrapper]: Invalid address type for v4 address");
            continue;
          }
        }
        else if (addr_info[i].iface_addr_s.addr.ss_family == AF_INET6) {
          AddressFormatIPv6WithPrefixLength(SASTORAGE_DATA(addr_info[i].iface_addr_s.addr),
            addr_info[i].iface_mask, tempStr, sizeof(tempStr));
          if (addr_info[i].primary) {
            v6Connected = true;
          }
          currIpType = IpFamilyType_t::IPv6;
          if (addr_info[i].address_type == DSI_ADDR_TYPE_INVALID) {
            Log::getInstance().d("[DsiWrapper]: Invalid address type for v6 address");
            continue;
          }
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid IP address family");
          continue;
        }
        #ifdef QMI_RIL_UTF
        tempAddrInfo.deprecationTime = (addr_info[i].deprecateTime == -1) ? MAX_LINX_ADDRESS_TIMER_VAL:addr_info[i].deprecateTime;
        tempAddrInfo.expirationTime = (addr_info[i].expiryTime == -1) ? MAX_LINX_ADDRESS_TIMER_VAL:addr_info[i].expiryTime;
        #else
        tempAddrInfo.deprecationTime = ADD_ELAPSED_REAL_TIME(addr_info[i].deprecateTime);
        tempAddrInfo.expirationTime = ADD_ELAPSED_REAL_TIME(addr_info[i].expiryTime);
        if(tempAddrInfo.deprecationTime > MAX_LINX_ADDRESS_TIMER_VAL)
          tempAddrInfo.deprecationTime = MAX_LINX_ADDRESS_TIMER_VAL;
        if(tempAddrInfo.expirationTime > MAX_LINX_ADDRESS_TIMER_VAL)
          tempAddrInfo.expirationTime = MAX_LINX_ADDRESS_TIMER_VAL;
        #endif

        //Check if address needs to be marked as deprecated
        if ((addr_info[i].deprecateTime == 0) &&
            (addr_info[i].expiryTime != -1)){
          tempAddrInfo.isDeprecated = true;
        }
        tempAddrInfo.ipAddress = std::string(tempStr);

      }

      memset(tempStr, 0, sizeof(tempStr));
      if (addr_info[i].gtwy_addr_s.valid_addr) {
        if (addr_info[i].gtwy_addr_s.addr.ss_family == AF_INET) {
          AddressFormatIPv4(SASTORAGE_DATA(addr_info[i].gtwy_addr_s.addr), tempStr, sizeof(tempStr));
        }
        else if (addr_info[i].gtwy_addr_s.addr.ss_family == AF_INET6 ) {
          AddressFormatIPv6(SASTORAGE_DATA(addr_info[i].gtwy_addr_s.addr), tempStr, sizeof(tempStr));
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid gateway address family");
          continue;
        }
        tempAddrInfo.gatewayAddress = std::string(tempStr);
      }

      memset(tempStr, 0, sizeof(tempStr));
      memset(DnsAddr, 0, sizeof(DnsAddr));
      if (addr_info[i].dnsp_addr_s.valid_addr) {
        if (addr_info[i].dnsp_addr_s.addr.ss_family == AF_INET ) {
          AddressFormatIPv4(SASTORAGE_DATA(addr_info[i].dnsp_addr_s.addr), tempStr, sizeof(tempStr));
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else if (addr_info[i].dnsp_addr_s.addr.ss_family == AF_INET6 ) {
          AddressFormatIPv6(SASTORAGE_DATA(addr_info[i].dnsp_addr_s.addr), tempStr, sizeof(tempStr));
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid dnsp address family");
          continue;
        }
      }

      if (addr_info[i].dnss_addr_s.valid_addr) {
        memset(tempStr, 0, sizeof(tempStr));
        if (addr_info[i].dnss_addr_s.addr.ss_family == AF_INET ) {
          AddressFormatIPv4(SASTORAGE_DATA(addr_info[i].dnss_addr_s.addr), tempStr, sizeof(tempStr));
          if(strlen(tempStr)) {strlcat(DnsAddr, " ", sizeof(DnsAddr));}
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else if (addr_info[i].dnss_addr_s.addr.ss_family == AF_INET6 ) {
          AddressFormatIPv6(SASTORAGE_DATA(addr_info[i].dnss_addr_s.addr), tempStr, sizeof(tempStr));
          if(strlen(tempStr)) {strlcat(DnsAddr, " ", sizeof(DnsAddr));}
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid dnss address family");
          continue;
        }
      }
      if (addr_info[i].dnsp_addr_s.valid_addr || addr_info[i].dnss_addr_s.valid_addr)
        tempAddrInfo.dnsAddress = std::string(DnsAddr);

      PduInfoByIpFamily tempPduInfo = { };
      //In case of V4 there can only be one address per PDU
      if (currIpType == IpFamilyType_t::IPv6) {
        if (addr_info[i].primary) {
          if (primPduInfo.v6Info) {
              tempPduInfo = *(primPduInfo.v6Info);
          }
        }
        else {
          if (secPduInfo.v6Info) {
              tempPduInfo = *(secPduInfo.v6Info);
          }
        }
      }
      tempPduInfo.addrMap.insert(std::pair<AddrType,AddrInfo>
       (convertDsiAddrTypeToInternal(addr_info[i].address_type), tempAddrInfo));
      if (addr_info[i].primary) {
        if (currIpType == IpFamilyType_t::IPv4) {
          primPduInfo.v4Info = tempPduInfo;
        }
        else if (currIpType == IpFamilyType_t::IPv6) {
          primPduInfo.v6Info = tempPduInfo;
        }
      }
      else {
        if (currIpType == IpFamilyType_t::IPv4) {
          secPduInfo.v4Info = tempPduInfo;
          }
        else if (currIpType == IpFamilyType_t::IPv6) {
          secPduInfo.v6Info = tempPduInfo;
        }
      }
    }

    std::stringstream ss;
    primPduInfo.dump("",ss);
    Log::getInstance().d("[DsiWrapper]: primary pdu info = " + ss.str());
    ss.str("");
    secPduInfo.dump("",ss);
    Log::getInstance().d("[DsiWrapper]: secondary pdu info = " + ss.str());
  }
  else {
    Log::getInstance().d("[DsiWrapper]: Memory calloc failure");
  }

  if (addr_info != nullptr) {
    free(addr_info);
  }

  return count;
}

int DsiWrapperLA::dsiGetPcscfAddresses(dsi_hndl_t dsiHandle, std::string &pcscfAddrV4,
                         std::string &pcscfAddrV6,
                         uint32_t pduId) {
  Log::getInstance().d("[DsiWrapper] dsiGetPcscfAddresses");

  int err_code = DSI_ERROR;
  struct sockaddr_in *sin = NULL;
  struct sockaddr_in6 *sin6 = NULL;
  char addr_buf[50];
  const char *addr_ptr = NULL;
  char fmtstrV4[DS_CALL_INFO_ADDR_PCSCF_MAX_LEN];
  char fmtstrV6[DS_CALL_INFO_ADDR_PCSCF_MAX_LEN];
  size_t fmtstr_size = sizeof(fmtstrV4);

  memset(fmtstrV4, 0, fmtstr_size);
  memset(fmtstrV6, 0, fmtstr_size);
  dsi_pcscf_addr_info_t *pcscf_addr_list = (dsi_pcscf_addr_info_t *)calloc(1, sizeof(dsi_pcscf_addr_info_t));
  if(pcscf_addr_list != nullptr) {
    dsi_iface_ioctl_ex(dsiHandle,
                        DSI_IFACE_IOCTL_EX_GET_PCSCF_SERV_ADDRESS,
                        pcscf_addr_list,
                       &err_code,
                       pduId);
    for (unsigned int i = 0; i < pcscf_addr_list->addr_count; i++) {
      if (pcscf_addr_list->pcscf_address[i].valid_addr != true ||
            (pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET &&
            pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET6))
      {
        Log::getInstance().d("Invalid pcscf address");
        continue;
      }
      if (pcscf_addr_list->pcscf_address[i].addr.ss_family == AF_INET)
      {
        sin = (struct sockaddr_in *)&pcscf_addr_list->pcscf_address[i].addr;
        addr_ptr = inet_ntop(AF_INET, &sin->sin_addr, addr_buf, sizeof(addr_buf));
        if ((NULL == addr_ptr) ||
             (fmtstr_size - strlen(fmtstrV4)) <= (strlen(addr_ptr) + 1)) {
          break;
        }
        if (strlen(fmtstrV4))
        {
          strlcat(fmtstrV4, " ", fmtstr_size);
        }
        strlcat(fmtstrV4, addr_ptr, fmtstr_size);
      }
      else
      {
        sin6 = (struct sockaddr_in6 *)&pcscf_addr_list->pcscf_address[i].addr;
        addr_ptr = inet_ntop(AF_INET6, &sin6->sin6_addr, addr_buf, sizeof(addr_buf));
        if ((NULL == addr_ptr) ||
             (fmtstr_size - strlen(fmtstrV6)) <= (strlen(addr_ptr) + 1)) {
          break;
        }
        if (strlen(fmtstrV6))
        {
          strlcat(fmtstrV6, " ", fmtstr_size);
        }
        strlcat(fmtstrV6, addr_ptr, fmtstr_size);
      }

    }
  }
  else {
    Log::getInstance().d("[DsiWrapper]: Memory calloc failure");
  }

  pcscfAddrV4 = std::string(fmtstrV4);
  pcscfAddrV6 = std::string(fmtstrV6);
  Log::getInstance().d("[DsiWrapper]: pcscf address = " + pcscfAddrV4 + " " + pcscfAddrV6);

  if (pcscf_addr_list != nullptr) {
    free(pcscf_addr_list);
  }

  return 0;
}

int DsiWrapperLA::dsiCheckIfCallExists
(
  dsi_hndl_t dsiHandle,
  std::string apnName,
  wds_apn_type_mask_v01 apnType,
  int ipType,
  std::string &deviceName
)
{
  int rc;
  char deviceNameBuf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1];
  memset(deviceNameBuf, 0, sizeof(deviceNameBuf));

  dsi_matching_call_check_t checkParams;
  checkParams.param_mask = DSI_MATCHING_CALL_PARAMS_APN_NAME |
                           DSI_MATCHING_CALL_PARAMS_APN_TYPE_MASK |
                           DSI_MATCHING_CALL_PARAMS_IP_TYPE;
  strlcpy(checkParams.apn_name, apnName.c_str(), apnName.length()+1);
  checkParams.apn_type_mask = apnType;
  checkParams.ip_type = ipType;

  rc = dsi_check_if_call_exists(dsiHandle, checkParams, deviceNameBuf, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
  if (rc!=0) {
    Log::getInstance().d("[DsiWrapper]: couldn't check existing call");
  }
  deviceName = std::string(deviceNameBuf);

  Log::getInstance().d("[DsiWrapper]: existing call device name = " + deviceName);

  return 0;
}

int DsiWrapperLA::dsiGetCurrentDataBearerTech(dsi_hndl_t dsiHandle, HandoffNetworkType_t &network)
{
  int result = DSI_SUCCESS;
  dsi_data_bearer_tech_t bearerTech = dsi_get_current_data_bearer_tech(dsiHandle);
  switch(bearerTech) {
    case DSI_DATA_BEARER_TECH_UNKNOWN:
      result = DSI_ERROR;
      break;
    case DSI_DATA_BEARER_TECH_3GPP_WLAN:
      network = HandoffNetworkType_t::_eIWLAN;
      break;
    case DSI_DATA_BEARER_TECH_3GPP_CIWLAN:
      network = HandoffNetworkType_t::_eCIWLAN;
      break;
    default:
      network = HandoffNetworkType_t::_eWWAN;
      break;
  }
  Log::getInstance().d("[DsiWrapper]: dsiGetCurrentDataBearerTech() networkType="+
      std::to_string((int)network)+", dsiBearerTech="+std::to_string((int)bearerTech));
  return result;
}

int DsiWrapperLA::dsiSetSSCCapability() {
  int result = dsi_set_ssc_capability(DSI_SSC_MODE_SSC_ONLY);
  if(DSI_SUCCESS != result)
    Log::getInstance().d("dsi_set_ssc_capability failed");
  else
    Log::getInstance().d("dsi_set_ssc_capability successful");
  return result;
}

void DsiWrapperLA::dsiGetMtuByFamily(dsi_hndl_t dsiHandle, dsi_v4_v6_mtu_t &v4_v6_mtu, uint32_t pduId) {
  Log::getInstance().d("[DsiWrapper] dsiGetMtuByFamily");

  int rc;
  if (dataMtu > 0) {
    v4_v6_mtu.v4_mtu = dataMtu;
    v4_v6_mtu.v6_mtu = dataMtu;
  } else {
    rc = dsi_get_link_mtu_by_family_ex(dsiHandle, &v4_v6_mtu, pduId);
    if (rc!=0) {
      Log::getInstance().d("[DsiWrapper]: couldn't get mtu");
    }
  }

  Log::getInstance().d("[DsiWrapper]: v4 mtu = " + std::to_string((int)v4_v6_mtu.v4_mtu) +
                       ", v6 mtu = " + std::to_string((int)v4_v6_mtu.v6_mtu));
  return;
}

std::string DsiWrapperLA::convertToString(dsi_call_param_identifier_t key, dsi_call_param_value_t* value) {
  std::stringstream ss;
  switch (key) {
    case DSI_CALL_INFO_UMTS_PROFILE_IDX:
      ss << "UMTS_PROFILE_IDX";
      break;
    case DSI_CALL_INFO_APN_NAME:
      ss << "APN_NAME";
      break;
    case DSI_CALL_INFO_USERNAME:
      ss << "USERNAME";
      break;
    case DSI_CALL_INFO_PASSWORD:
      ss << "PASSWORD";
      break;
    case DSI_CALL_INFO_AUTH_PREF:
      ss << "AUTH_PREF";
      break;
    case DSI_CALL_INFO_CDMA_PROFILE_IDX:
      ss << "CDMA_PROFILE_IDX";
      break;
    case DSI_CALL_INFO_IP_ADDR:
      ss << "IP_ADDR";
      break;
    case DSI_CALL_INFO_DEVICE_NAME:
      ss << "DEVICE_NAME";
      break;
    case DSI_CALL_INFO_TECH_PREF:
      ss << "TECH_PREF";
      break;
    case DSI_CALL_INFO_CALL_TYPE:
      ss << "CALL_TYPE";
      break;
    case DSI_CALL_INFO_IP_VERSION:
      ss << "IP_VERSION";
      break;
    case DSI_CALL_INFO_EXT_TECH_PREF:
      ss << "EXT_TECH_PREF";
      break;
    case DSI_CALL_INFO_PARTIAL_RETRY:
      ss << "PARTIAL_RETRY";
      break;
    case DSI_CALL_INFO_APP_TYPE:
      ss << "APP_TYPE";
      break;
    case DSI_CALL_INFO_APN_TYPE:
      ss << "APN_TYPE";
      break;
    case DSI_CALL_INFO_APN_TYPE_MASK:
      ss << "APN_TYPE_MASK";
      break;
    case DSI_CALL_INFO_DISALLOW_ROAMING:
      ss << "DISALLOW_ROAMING";
      break;
    case DSI_CALL_INFO_HANDOFF_CONTEXT:
      ss << "HANDOFF_CONTEXT";
      break;
    case DSI_CALL_INFO_CALL_BRINGUP_MODE:
      ss << "CALL_BRINGUP_MODE";
      break;
    case DSI_CALL_INFO_SUBS_ID:
      ss << "SUBS_ID";
      break;
    default:
      ss << std::to_string((int)key);
      break;
  }
  if (value != nullptr) {
    ss << " " << value->num_val;
    if (value->buf_val != nullptr) {
      ss << std::hex << std::setw(2) << std::setfill('0');
      for (int i = 0; i < value->num_val; i++) {
        ss << " 0x" << (int)value->buf_val[i];
      }
    }
  }
  return ss.str();
}
