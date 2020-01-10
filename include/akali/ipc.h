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

#ifndef AKALI_IPC_H_
#define AKALI_IPC_H_
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include "akali/constructormagic.h"
#include <functional>
#include <string>

namespace akali {
class AKALI_API IPC {
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
  AKALI_DISALLOW_COPY_AND_ASSIGN(IPC);
};
} // namespace akali
#endif
#endif // !AKALI_IPC_H_
