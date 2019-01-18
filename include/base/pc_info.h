/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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
#include "ppx_export.h"

namespace ppx {
    namespace base {
        PPX_API std::string GetLocalIP();
        PPX_API void GetLocalIPList(std::vector<std::string> &ip_list, std::vector<std::string> &gateway_ip_list);
        PPX_API std::string GetMACThroughIP(const std::string &ip);
        PPX_API std::string GetPCName();
        PPX_API void GetCPUId(std::string &cpu_id);
        PPX_API void GetMACList(std::vector<std::string> &mac_list);
        PPX_API std::string GetValidMAC();
		PPX_API int64_t GetValidMacInteger();
    }
}


#endif // !PPX_BASE_PC_INFO_H_