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

#ifndef PPX_BASE_HOST_RESOLVE_H_
#define PPX_BASE_HOST_RESOLVE_H_
#pragma once

#ifdef _WIN32
#include <string>
#include <functional>
#include <thread>
#include <vector>
#include "ppxbase/ipaddress.h"
#include "ppxbase_export.h"

#pragma comment(lib, "ws2_32.lib")

namespace ppx {
    namespace base {
        class PPXBASE_API HostResolve {
          public:
            HostResolve();
            virtual~HostResolve();

            bool Resolve(const std::string &host, std::vector<IPAddress> &ip_list);
        };
    }
}

#endif

#endif // !PPX_BASE_HOST_RESOLVE_H_