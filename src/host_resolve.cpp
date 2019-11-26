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

#include "ppxbase/host_resolve.h"
#if (defined _WIN32 || defined WIN32)
#include "ppxbase/safe_release_macro.h"

namespace ppx {
    namespace base {

        HostResolve::HostResolve() {
            WSADATA wsaData;
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSAStartup(wVersionRequested, &wsaData);
        }

        HostResolve::~HostResolve() {
            WSACleanup();
        }

        bool HostResolve::Resolve(const std::string &host, std::vector<IPAddress> &ip_list) {
            if (host.length() == 0)
                return false;

            struct addrinfo *result = nullptr;
            struct addrinfo hints = { 0 };
            hints.ai_family = AF_UNSPEC;

            hints.ai_flags = AI_ADDRCONFIG;
            int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);
            if (ret != 0) {
                return false;
            }

            struct addrinfo *cursor = result;
            bool flag = false;
            for (; cursor; cursor = cursor->ai_next) {
                sockaddr_in *paddr_in = reinterpret_cast<sockaddr_in *>(cursor->ai_addr);

                IPAddress ip(paddr_in->sin_addr);

                bool found = false;
                for (auto it : ip_list) {
                    if (it == ip) {
                        found = true;
                        break;
                    }
                }
                if(!found)
                    ip_list.push_back(ip);
            }
            freeaddrinfo(result);

            return true;
        }

    }
}

#endif