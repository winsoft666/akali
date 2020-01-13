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

#ifndef AKALI_DIRECTORY_MONITOR_H__
#define AKALI_DIRECTORY_MONITOR_H__
#pragma once
#include "akali/akali_export.h"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <functional>
#include <future>
#include <vector>

namespace akali {
class AKALI_API DirectoryMonitor {
 public:
  typedef std::function<void(DWORD /*action*/, std::wstring /*path*/)> ChangeNotify;
  DirectoryMonitor();
  virtual ~DirectoryMonitor();

  bool StartMonitor(const std::wstring& direcotry,
                    bool watch_sub_dir,
                    DWORD filter,
                    ChangeNotify cn);
  void StopMonitor();

 private:
  HANDLE directory_;
  OVERLAPPED overlapped_;
  std::future<void> monitor_task_;
};
}  // namespace akali
#endif

#endif  // !AKALI_DIRECTORY_MONITOR_H__
