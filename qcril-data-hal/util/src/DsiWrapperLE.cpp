/******************************************************************************
#  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "DsiWrapper.h"
#include <iomanip>
#include <sstream>

using namespace rildata;

DsiWrapper& DsiWrapper::getInstance()
{
  static DsiWrapperLE dsiWrapper;
  return dsiWrapper;
}

unsigned int DsiWrapperLE::dsiGetAddresses(dsi_hndl_t dsiHandle, bool &v4Connected, bool &v6Connected,
  PduInfo &primPduInfo, PduInfo &secPduInfo) {
  Log::getInstance().d("[DsiWrapper] dsiGetAddresses");

  int rc = 0;
  unsigned int count;
  char DnsAddr[DS_CALL_INFO_ADDR_IPV4V6_MAX_LEN*2+1];
  char tempStr[DS_CALL_INFO_ADDR_IPV4V6_MAX_LEN+1];

  if (isDsiex2Supported && dsiConnectionMap[dsiHandle].isConnectInfoValid) {
    count = dsiConnectionMap[dsiHandle].connectInfo.addr_count;
  } else {
    count = dsi_get_ip_addr_count(dsiHandle);
  }
  Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr_count = " + std::to_string(count));

  if (count == 0) {
    return 0;
  }

  dsi_addr_info_t *addr_info = nullptr;
  addr_info = (dsi_addr_info_t *)calloc(count, sizeof(dsi_addr_info_t));

  if (addr_info != nullptr) {
    if (isDsiex2Supported && dsiConnectionMap[dsiHandle].isConnectInfoValid) {
      for (int i = 0; i < count; i++) {
        addr_info[i] = dsiConnectionMap[dsiHandle].connectInfo.addr_info[i];
      }
    } else {
      rc = dsi_get_ip_addr(dsiHandle, addr_info, count);
    }
    if (rc!=0) {
      Log::getInstance().d("[DsiWrapper]: couldn't get address info");
    }


    for (unsigned int i= 0 ; i < count ; i++) {
      AddrInfo tempAddrInfo = { };
      IpFamilyType_t currIpType = IpFamilyType_t::NotAvailable;
      memset(tempStr, 0, sizeof(tempStr));
      if (addr_info[i].iface_addr_s.valid_addr) {
        if ((addr_info[i].iface_addr_s.addr.ss_family == AF_INET) ) {
          AddressFormatIPv4WithSubnetMask(SASTORAGE_DATA(addr_info[i].iface_addr_s.addr),
            addr_info[i].iface_mask, tempStr, sizeof(tempStr));
          v4Connected = true;
          currIpType = IpFamilyType_t::IPv4;
          Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr v4: " + std::string(tempStr));
        }
        else if ((addr_info[i].iface_addr_s.addr.ss_family == AF_INET6) ) {
          AddressFormatIPv6WithPrefixLength(SASTORAGE_DATA(addr_info[i].iface_addr_s.addr),
            addr_info[i].iface_mask, tempStr, sizeof(tempStr));
          v6Connected = true;
          currIpType = IpFamilyType_t::IPv6;
          Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr v6: " + std::string(tempStr));
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid IP address family");
          continue;
        }
        tempAddrInfo.ipAddress = std::string(tempStr);
      } else {
        Log::getInstance().d("[DsiWrapper]: iface_addr_s Valid Bit: " + std::to_string(addr_info[i].iface_addr_s.valid_addr));
      }

      memset(tempStr, 0, sizeof(tempStr));
      if (addr_info[i].gtwy_addr_s.valid_addr) {
        if ((addr_info[i].gtwy_addr_s.addr.ss_family == AF_INET) ) {
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
        Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr gateway: " + std::string(tempStr));
      } else {
        Log::getInstance().d("[DsiWrapper]: gtwy_addr_s Valid Bit: " + std::to_string(addr_info[i].gtwy_addr_s.valid_addr));
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
        Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr dnsp: " + std::string(tempStr));
      } else {
        Log::getInstance().d("[DsiWrapper]: dnsp_addr_s Valid Bit: " + std::to_string(addr_info[i].dnsp_addr_s.valid_addr));
      }

      if (addr_info[i].dnss_addr_s.valid_addr) {
        memset(tempStr, 0, sizeof(tempStr));
        if (addr_info[i].dnss_addr_s.addr.ss_family == AF_INET ) {
          AddressFormatIPv4(SASTORAGE_DATA(addr_info[i].dnss_addr_s.addr), tempStr, sizeof(tempStr));
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else if (addr_info[i].dnss_addr_s.addr.ss_family == AF_INET6 ) {
          AddressFormatIPv6(SASTORAGE_DATA(addr_info[i].dnss_addr_s.addr), tempStr, sizeof(tempStr));
          strlcat(DnsAddr, tempStr, sizeof(DnsAddr));
        }
        else {
          Log::getInstance().d("[DsiWrapper]: Invalid dnss address family");
          continue;
        }
        Log::getInstance().d("[DsiWrapper]: dsi_get_ip_addr dnss: " + std::string(tempStr));
      } else {
        Log::getInstance().d("[DsiWrapper]: dnss_addr_s Valid Bit: " + std::to_string(addr_info[i].dnss_addr_s.valid_addr));
      }
      if (addr_info[i].dnsp_addr_s.valid_addr || addr_info[i].dnss_addr_s.valid_addr)
        tempAddrInfo.dnsAddress = std::string(DnsAddr);

      PduInfoByIpFamily tempPduInfo = { };
      if (currIpType == IpFamilyType_t::IPv4) {
        tempPduInfo.addrMap.insert(std::pair<AddrType,AddrInfo>
                                   (AddrType::_eInvalid, tempAddrInfo));
        primPduInfo.v4Info = tempPduInfo;
      }
      else if (currIpType == IpFamilyType_t::IPv6) {
        tempPduInfo.addrMap.insert(std::pair<AddrType,AddrInfo>
                                   (AddrType::_eDefault, tempAddrInfo));
        primPduInfo.v6Info = tempPduInfo;
      }
    }
  }
  else {
    Log::getInstance().d("[DsiWrapper]: Memory calloc failure");
  }

  primPduInfo.pduId = 0;
  std::stringstream ss;
  primPduInfo.dump("",ss);
  Log::getInstance().d("[DsiWrapper]: pdu info = " + ss.str());
  ss.str("");

  if (addr_info != nullptr) {
    free(addr_info);
  }

  return count;
}

int DsiWrapperLE::dsiGetPcscfAddresses(dsi_hndl_t dsiHandle, std::string &pcscfAddrV4,
                         std::string &pcscfAddrV6,
                         uint32_t pduId) {
  Log::getInstance().d("[DsiWrapper] dsiGetPcscfAddresses");

  int rc;
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
    rc = dsi_iface_ioctl(dsiHandle,
                        DSI_IFACE_IOCTL_GET_PCSCF_SERV_ADDRESS,
                        pcscf_addr_list,
                        &err_code);
    for (unsigned int i = 0; i < pcscf_addr_list->addr_count; i++) {
      if (pcscf_addr_list->pcscf_address[i].valid_addr != true ||
            (pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET &&
            pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET6))
      {
        Log::getInstance().d("Invalid pcscf address");
        continue;
      }
      if ((pcscf_addr_list->pcscf_address[i].addr.ss_family == AF_INET) )
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
  Log::getInstance().d("[DsiWrapper]: pcscf addressV4 = " + pcscfAddrV4);
  Log::getInstance().d("[DsiWrapper]: pcscf addressV6 = " + pcscfAddrV6);

  if (pcscf_addr_list != nullptr) {
    free(pcscf_addr_list);
  }

  return 0;
}

int DsiWrapperLE::dsiGetCurrentDataBearerTech(dsi_hndl_t dsiHandle, HandoffNetworkType_t &network)
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
    default:
      network = HandoffNetworkType_t::_eWWAN;
      break;
  }
  Log::getInstance().d("[DsiWrapper]: dsiGetCurrentDataBearerTech() networkType="+
      std::to_string((int)network)+", dsiBearerTech="+std::to_string((int)bearerTech));
  return result;
}

void DsiWrapperLE::dsiGetMtuByFamily(dsi_hndl_t dsiHandle, dsi_v4_v6_mtu_t &v4_v6_mtu, uint32_t pduId) {
  Log::getInstance().d("[DsiWrapper] dsiGetMtuByFamily");

  std::ignore = pduId;
  int rc;
  if (dataMtu > 0) {
    v4_v6_mtu.v4_mtu = dataMtu;
    v4_v6_mtu.v6_mtu = dataMtu;
  } else {
    rc = dsi_get_link_mtu_by_family(dsiHandle, &v4_v6_mtu);
    if (rc!=0) {
      Log::getInstance().d("[DsiWrapper]: couldn't get mtu");
      v4_v6_mtu.v4_mtu = 0;
      v4_v6_mtu.v6_mtu = 0;
      return;
    }
  }
  Log::getInstance().d("[DsiWrapper]: v4 mtu = " + std::to_string((int)v4_v6_mtu.v4_mtu) + ", v6 mtu = " + std::to_string((int)v4_v6_mtu.v6_mtu));
  return;
}

std::string DsiWrapperLE::convertToString(dsi_call_param_identifier_t key, dsi_call_param_value_t* value) {
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
    case DSI_CALL_INFO_SUBS_ID:
      ss << "SUBS_ID";
      break;
    case DSI_CALL_INFO_APN_TYPE_MASK:
      ss << "APN_TYPE_MASK";
      break;
    case DSI_CALL_INFO_CALL_BRINGUP_MODE:
      ss << "CALL_BRINGUP_MODE";
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
