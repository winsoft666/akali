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

#ifndef PPX_BASE_PC_INFO_H_
#define PPX_BASE_PC_INFO_H_
#pragma once

#include <string>
#include <vector>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        PPXBASE_API std::string GetLocalIP();
        PPXBASE_API void GetLocalIPList(std::vector<std::string> &ip_list, std::vector<std::string> &gateway_ip_list);
        PPXBASE_API std::string GetMACThroughIP(const std::string &ip);
        PPXBASE_API std::string GetPCName();
        PPXBASE_API void GetCPUId(std::string &cpu_id);
        PPXBASE_API void GetMACList(std::vector<std::string> &mac_list);
        PPXBASE_API std::string GetValidMAC();
        PPXBASE_API int64_t GetValidMacInteger();
    }
}


#endif // !PPX_BASE_PC_INFO_H_