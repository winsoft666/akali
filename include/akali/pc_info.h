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

#ifndef AKALI_PC_INFO_H_
#define AKALI_PC_INFO_H_
#pragma once

#if (defined _WIN32 || defined WIN32)
#include <string>
#include <vector>
#include "akali_export.h"

namespace akali {
AKALI_API std::string GetLocalIP();
AKALI_API void GetLocalIPList(std::vector<std::string> &ip_list,
                                std::vector<std::string> &gateway_ip_list);
AKALI_API std::string GetMACThroughIP(const std::string &ip);
AKALI_API std::string GetPCName();
AKALI_API void GetMACList(std::vector<std::string> &mac_list);
AKALI_API std::string GetValidMAC();
AKALI_API int64_t GetValidMacInteger();
} // namespace akali
#endif
#endif // !AKALI_PC_INFO_H_
