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

#ifndef PPX_BASE_IPC_H_
#define PPX_BASE_IPC_H_
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "ppxbase/constructormagic.h"
#include "ppxbase_export.h"
#include <functional>
#include <string>

namespace ppx {
namespace base {
class PPXBASE_API IPC {
public:
  IPC();
  virtual ~IPC();

  bool StartListen(const std::string &ipc_name,
                   std::function<void(const void *data, unsigned int data_size)> cb);
  void StopListen();

  bool SyncSend(const std::string &target_ipc_name, const void *data, unsigned int data_size,
                unsigned int timeout_ms = 3000);

private:
  class IPCImpl;
  IPCImpl *impl_;

private:
  PPX_DISALLOW_COPY_AND_ASSIGN(IPC);
};
} // namespace base
} // namespace ppx

#endif

#endif // !PPX_BASE_IPC_H_