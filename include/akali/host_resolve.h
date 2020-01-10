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

#ifndef AKALI_HOST_RESOLVE_H_
#define AKALI_HOST_RESOLVE_H_
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include <string>
#include <functional>
#include <thread>
#include <vector>
#include "akali/ipaddress.h"

#pragma comment(lib, "ws2_32.lib")

namespace akali {
class AKALI_API HostResolve {
public:
  HostResolve();
  virtual ~HostResolve();

  bool Resolve(const std::string &host, std::vector<IPAddress> &ip_list);
};
} // namespace akali

#endif
#endif // !AKALI_HOST_RESOLVE_H_
