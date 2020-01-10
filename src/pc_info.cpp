/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#include "akali/pc_info.h"
#ifdef AKALI_WIN
#include <WinSock2.h>
#include <Iphlpapi.h>
#include <thread>
#include "akali/string_helper.hpp"

namespace akali {
std::string GetLocalIP() {
  WSADATA wsaData;
  int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

  if (ret != 0)
    return "";

  char hostname[256] = {0};
  ret = gethostname(hostname, sizeof(hostname));

  if (ret == SOCKET_ERROR)
    return "";

  HOSTENT *host = gethostbyname(hostname);

  if (host == NULL)
    return "";

  char ip[30] = {0};
  in_addr *addr = (in_addr *)*host->h_addr_list;
  strcpy_s(ip, 30, inet_ntoa(addr[0]));

  WSACleanup();

  return ip;
}

void GetLocalIPList(std::vector<std::string> &ip_list, std::vector<std::string> &gateway_ip_list) {
  unsigned long stSize = 0;
  IP_ADAPTER_INFO *pIpAdapterInfo = NULL;
  IP_ADAPTER_INFO *pIpAdapterInfoBak = NULL;
  int nRel = GetAdaptersInfo(NULL, &stSize);
  if (ERROR_BUFFER_OVERFLOW == nRel) {
    pIpAdapterInfo = (PIP_ADAPTER_INFO)malloc(stSize);
    pIpAdapterInfoBak = pIpAdapterInfo;
    nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
  }

  if (ERROR_SUCCESS == nRel) {
    while (pIpAdapterInfo) {
      IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
      do {
        std::string ip;
        ip = pIpAddrString->IpAddress.String;
        ip_list.push_back(ip);

        pIpAddrString = pIpAddrString->Next;
      } while (pIpAddrString);

      IP_ADDR_STRING *pIpAddrString2 = &(pIpAdapterInfo->GatewayList);
      do {
        std::string ip;
        ip = pIpAddrString2->IpAddress.String;
        gateway_ip_list.push_back(ip);

        pIpAddrString2 = pIpAddrString2->Next;
      } while (pIpAddrString2);

      pIpAdapterInfo = pIpAdapterInfo->Next;
    }
  }

  if (pIpAdapterInfoBak) {
    free(pIpAdapterInfoBak);
  }
}

std::string GetMACThroughIP(const std::string &ip) {
  if (ip.length() == 0)
    return "";

  std::string ret;

  std::thread t = std::thread([&]() {
    unsigned char mac[6];
    ULONG MacLen = 6;
    ULONG DestIP = inet_addr(ip.c_str());

    memset(&mac, 0xff, sizeof(mac));
    char szMac[32] = {0};
    DWORD dwRet = SendARP(DestIP, (ULONG)NULL, (PULONG)mac, (PULONG)&MacLen);
    if (dwRet == NO_ERROR) {
      sprintf_s(szMac, 32, "%02x-%02x-%02x-%02x-%02x-%02x", (unsigned int)mac[0],
                (unsigned int)mac[1], (unsigned int)mac[2], (unsigned int)mac[3],
                (unsigned int)mac[4], (unsigned int)mac[5]);

      ret = szMac;
    }
  });

  if (t.joinable()) {
    t.join();
  }

  return ret;
}

std::string GetPCName() {
  std::string pc_name;
  DWORD dwLen = MAX_COMPUTERNAME_LENGTH + 1;
  char szBuf[MAX_COMPUTERNAME_LENGTH + 1] = {0};
  if (GetComputerNameA(szBuf, &dwLen)) {
    pc_name = szBuf;
  }

  return pc_name;
}

void GetMACList(std::vector<std::string> &mac_list) {
  ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
  PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);

  if (pAddresses == NULL)
    return;

  // Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
  if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
    free(pAddresses);
    pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);

    if (pAddresses == NULL)
      return;
  }

  if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR) {
    // If successful, output some information from the data we received
    for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL;
         pCurrAddresses = pCurrAddresses->Next) {
      // 确保MAC地址的长度为 00-00-00-00-00-00
      if (pCurrAddresses->PhysicalAddressLength != 6)
        continue;
      char acMAC[32] = {0};
      sprintf_s(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X", int(pCurrAddresses->PhysicalAddress[0]),
                int(pCurrAddresses->PhysicalAddress[1]), int(pCurrAddresses->PhysicalAddress[2]),
                int(pCurrAddresses->PhysicalAddress[3]), int(pCurrAddresses->PhysicalAddress[4]),
                int(pCurrAddresses->PhysicalAddress[5]));

      mac_list.push_back(acMAC);
    }
  }

  free(pAddresses);
}

std::string GetValidMAC() {
  std::string mac_ret;
  unsigned long stSize = 0;
  IP_ADAPTER_INFO *pIpAdapterInfo = NULL;
  IP_ADAPTER_INFO *pIpAdapterInfoBak = NULL;
  int nRel = GetAdaptersInfo(NULL, &stSize);
  if (ERROR_BUFFER_OVERFLOW == nRel) {
    pIpAdapterInfo = (PIP_ADAPTER_INFO)malloc(stSize);
    pIpAdapterInfoBak = pIpAdapterInfo;
    nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
  }

  if (ERROR_SUCCESS == nRel) {
    while (pIpAdapterInfo) {
      if (pIpAdapterInfo->Type != MIB_IF_TYPE_ETHERNET && pIpAdapterInfo->Type != 71) {
        pIpAdapterInfo = pIpAdapterInfo->Next;
        continue;
      }

      std::string ip;
      IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
      do {
        std::string tmp = pIpAddrString->IpAddress.String;
        if (tmp.length() > 0 && tmp != "0.0.0.0") {
          ip = tmp;
          break;
        }

        pIpAddrString = pIpAddrString->Next;
      } while (pIpAddrString);

      if (ip.length() > 0) {
        char acMAC[32] = {0};
        sprintf_s(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X", int(pIpAdapterInfo->Address[0]),
                  int(pIpAdapterInfo->Address[1]), int(pIpAdapterInfo->Address[2]),
                  int(pIpAdapterInfo->Address[3]), int(pIpAdapterInfo->Address[4]),
                  int(pIpAdapterInfo->Address[5]));

        mac_ret = acMAC;
        break;
      }

      pIpAdapterInfo = pIpAdapterInfo->Next;
    }
  }

  if (pIpAdapterInfoBak) {
    free(pIpAdapterInfoBak);
  }

  return mac_ret;
}

AKALI_API int64_t GetValidMacInteger() {
  std::string strMac = GetValidMAC();
  if (strMac.length() == 0)
    return 0;

  int64_t ret = 0L;
  std::vector<std::string> mac_values;
  mac_values = StringSplit<std::string>(strMac, "-");

  for (size_t i = 0; i < mac_values.size(); i++) {
    int64_t l = strtol(mac_values[i].c_str(), NULL, 16) * (long long)pow(10, i * 3);
    ret += l;
  }

  return ret;
}
} // namespace akali
#endif