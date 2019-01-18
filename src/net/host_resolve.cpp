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

#include "net/host_resolve.h"
#include "base/safe_release_macro.h"

namespace ppx {
    namespace net {

        HostResolve::HostResolve() :
            exit_(CreateEvent(NULL, TRUE, FALSE, NULL)) {
            WSADATA wsaData;
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSAStartup(wVersionRequested, &wsaData);

            threads_ = new std::vector<std::thread>();
        }

        HostResolve::~HostResolve() {
            if (exit_)
                SetEvent(exit_);
            for (size_t i = 0; i < (*threads_).size(); i++) {
                if ((*threads_)[i].joinable())
                    (*threads_)[i].join();
            }
            (*threads_).clear();
            SAFE_CLOSE(exit_);
            WSACleanup();

            SAFE_DELETE(threads_);
        }

        bool HostResolve::AsyncResolve(const std::string &host, std::function<void(const std::vector<IPAddress>&)> callback) {
            if (host.length() == 0)
                return false;
            if(threads_)
                (*threads_).push_back(std::thread(&HostResolve::DoResolve, this, host, callback));
            return true;
        }

        void HostResolve::DoResolve(const std::string &host, std::function<void(const std::vector<IPAddress>&)> callback) {
            struct addrinfo* result = nullptr;
            struct addrinfo hints = { 0 };
            hints.ai_family = AF_UNSPEC;

            hints.ai_flags = AI_ADDRCONFIG;
            int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);
            if (ret != 0) {
                return;
            }
            std::vector<IPAddress> ips;

            struct addrinfo* cursor = result;
            bool flag = false;
            for (; cursor; cursor = cursor->ai_next) {
                sockaddr_in *paddr_in = reinterpret_cast<sockaddr_in *>(cursor->ai_addr);

                IPAddress ip(paddr_in->sin_addr);
                ips.push_back(ip);
            }
            freeaddrinfo(result);

            if (callback) {
                callback(ips);
            }
        }

    }
}