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

#include "ppxbase/thread_util.h"

namespace ppx {
    namespace base {
        void SetCurrentThreadName(const char *name) {
#if defined(_WIN32)
            struct {
                DWORD dwType;
                LPCSTR szName;
                DWORD dwThreadID;
                DWORD dwFlags;
            } threadname_info = { 0x1000, name, static_cast<DWORD>(-1), 0 };

            __try {
                ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(DWORD),
                                 reinterpret_cast<ULONG_PTR *>(&threadname_info));
            } __except (EXCEPTION_EXECUTE_HANDLER) { // NOLINT
            }

#elif defined(LINUX) || defined(ANDROID)
            prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#elif defined(MAC) || defined(IOS)
            pthread_setname_np(name);
#endif
        }
    }
}