#ifndef PPXBASE_DIRECTORY_MONITOR_H__
#define PPXBASE_DIRECTORY_MONITOR_H__
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <functional>
#include <future>
#include <vector>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API DirectoryMonitor {
          public:
            typedef std::function<void(DWORD /*action*/, std::wstring /*path*/)> ChangeNotify;
            DirectoryMonitor();

            virtual ~DirectoryMonitor();

            bool StartMonitor(const std::wstring &direcotry, bool watch_sub_dir,
                              DWORD filter, ChangeNotify cn);

            void StopMonitor();

          private:
            HANDLE directory_;
            OVERLAPPED	overlapped_;
            std::future<void> monitor_task_;
        };
    }
}

#endif // !PPXBASE_DIRECTORY_MONITOR_H__