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

#ifndef PPX_BASE_H__
#define PPX_BASE_H__
#pragma once

#include "ppxbase/assert.h"
#include "ppxbase/base64.h"
#include "ppxbase/scoped_ref_ptr.h"
#include "ppxbase/scoped_variant.h"
#include "ppxbase/scoped_propvariant.h"
#include "ppxbase/scoped_com_initializer.h"
#include "ppxbase/buffer_queue.h"
#include "ppxbase/byteorder.h"
#include "ppxbase/cmdline_parse.h"
#include "ppxbase/constructormagic.h"
#include "ppxbase/criticalsection.h"
#include "ppxbase/deprecation.h"
#include "ppxbase/driver_info.h"
#include "ppxbase/event.h"
#include "ppxbase/endianess_detect.h"
#include "ppxbase/flags.h"
#include "ppxbase/file_info.h"
#include "ppxbase/file_util.h"
#include "ppxbase/ini.h"
#include "ppxbase/logging.h"
#include "ppxbase/logging_file_sink_win.h"
#include "ppxbase/macros.h"
#include "ppxbase/md5.h"
#include "ppxbase/memory_pool.hpp"
#include "ppxbase/module_safe_string.h"
#include "ppxbase/os_ver.h"
#include "ppxbase/pc_info.h"
#include "ppxbase/process_util.h"
#include "ppxbase/random.h"
#include "ppxbase/registry.h"
#include "ppxbase/string_helper.h"
#include "ppxbase/safe_release_macro.h"
#include "ppxbase/safe_compare.h"
#include "ppxbase/safe_conversions.h"
#include "ppxbase/noncopyable.h"
#include "ppxbase/singleton.h"
#include "ppxbase/stringencode.h"
#include "ppxbase/callback.h"
#include "ppxbase/bind.h"
#include "ppxbase/thread_util.h"
#include "ppxbase/timer.h"
#include "ppxbase/timeutils.h"
#include "ppxbase/win_main.h"
#include "ppxbase/win_service_base.h"
#include "ppxbase/win_service_installer.h"
#include "ppxbase/display_monitors.h"
#include "ppxbase/schedule_task.h"
#include "ppxbase/ipc.h"
#include "ppxbase/console_print.h"
#include "ppxbase/miscellaneous.h"
#include "ppxbase/shortcut.h"
#include "ppxbase/screenshot.h"
#include "ppxbase/host_resolve.h"
#include "ppxbase/ipaddress.h"
#include "ppxbase/networkprotocoldef.h"
#include "ppxbase/ping.h"
#include "ppxbase/socket.h"
#include "ppxbase/socketaddress.h"
#include "ppxbase/overlappedsocket.h"
#include "ppxbase/iocp.h"
#include "ppxbase/iocpserver.h"
#include "ppxbase/internet_availability_checker.h"
#include "ppxbase/condition_task.h"
#include "ppxbase/stack_walker.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "DbgHelp.lib")

#endif // !PPX_BASE_H__