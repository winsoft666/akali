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

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <strsafe.h>
#include "ppxbase/assert.h"
#include <cstring>
#include <cstdlib> // abort()

namespace ppx {
    namespace Internal {
        void HandleAssert(const char *file, int line, const char *function, const char *expression, const char *message, ...) {
            char message_[1024] = {0};
            const char *file_;

            if (message) {
                va_list args;
                va_start(args, message);
                StringCchVPrintfA(message_, 1024, message, args);
                va_end(args);
            }

#if defined(_WIN32)
            file_ = strrchr(file, '\\');
#else
            file_ = strrchr(file, '/');
#endif // #if defined(_WIN32)

            file = file_ ? file_ + 1 : file;

            char buf[1024];
            StringCchPrintfA(buf, 1024, "Assertion failed!\r\n\r\n" \
                             "File: %s\r\nLine: %d\r\nFunction: %s\r\n\r\n" \
                             "Expression: %s\r\n\r\n" \
                             "Message: %s\r\n\r\n" \
                             "(Press Retry to debug the application - JIT must be enabled)\r\n",
                             file, line, function, expression, message_);

            OutputDebugStringA(buf);

            int ret = MessageBoxA(NULL, buf, "PPX Assert", MB_TASKMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE | MB_SETFOREGROUND);

            if (ret == IDIGNORE) {
                return;
            } else if (ret == IDRETRY) {
                PPX_ASSERT_DEBUG_BREAK();
            } else if(ret == IDABORT) {
                _exit(3);
            }
        }
    } // namespace Internal
} // namespace ppx
