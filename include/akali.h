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

#ifndef AKALI_H__
#define AKALI_H__
#pragma once

#include "akali/base64.h"
#include "akali/scoped_variant.h"
#include "akali/scoped_com_initializer.h"
#include "akali/buffer_queue.h"
#include "akali/byteorder.h"
#include "akali/constructormagic.h"
#include "akali/criticalsection.h"
#include "akali/deprecation.h"
#include "akali/driver_info.h"
#include "akali/endianess_detect.h"
#include "akali/file_info.h"
#include "akali/file_util.h"
#include "akali/ini.h"
#include "akali/md5.h"
#include "akali/memory_pool.hpp"
#include "akali/os_ver.h"
#include "akali/pc_info.h"
#include "akali/process_util.h"
#include "akali/process.h"
#include "akali/registry.h"
#include "akali/string_helper.hpp"
#include "akali/macros.h"
#include "akali/noncopyable.h"
#include "akali/singleton.hpp"
#include "akali/stringencode.h"
#include "akali/timer.h"
#include "akali/timeutils.h"
#include "akali/win_main.h"
#include "akali/win_service_base.h"
#include "akali/win_service_installer.h"
#include "akali/display_monitors.h"
#include "akali/schedule_task.h"
#include "akali/ipc.h"
#include "akali/shortcut.h"
#include "akali/host_resolve.h"
#include "akali/ipaddress.h"
#include "akali/networkprotocoldef.h"
#include "akali/ping.h"
#include "akali/socket.h"
#include "akali/socketaddress.h"
#include "akali/overlappedsocket.h"
#include "akali/iocp_socket.h"
#include "akali/iocpserver.h"
#include "akali/internet_availability_checker.h"
#include "akali/stack_walker.h"
#include "akali/directory_monitor.h"
#include "akali/trace.h"
#include "akali/thread.hpp"
#include "akali/thread_pool.hpp"

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif

#ifndef GHC_USE_STD_FS
#include "akali/filesystem.hpp"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "DbgHelp.lib")
#endif

#endif // !AKALI_H__