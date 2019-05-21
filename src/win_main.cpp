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

#include "ppxbase/win_main.h"

#ifdef _WIN32

#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>

namespace ppx {
    namespace base {

        static TCHAR *lstrrchr(LPCTSTR string, int ch) {
            TCHAR *start = (TCHAR *)string;

            while (*string++) ;

            while (--string != start && *string != (TCHAR)ch);

            if (*string == (TCHAR)ch)
                return (TCHAR *)string;

            return NULL;
        }

        void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo) {
            if (!excpInfo) {
                static int iTimes = 0;

                if (iTimes++ > 1)
                    return;

                __try {
                    RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
                } __except (DumpMiniDump(hFile, GetExceptionInformation()),
                            EXCEPTION_CONTINUE_EXECUTION) {
                }
            } else {
                MINIDUMP_EXCEPTION_INFORMATION eInfo;
                eInfo.ThreadId = GetCurrentThreadId();
                eInfo.ExceptionPointers = excpInfo;
                eInfo.ClientPointers = FALSE;

                MiniDumpWriteDump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    hFile,
                    MiniDumpNormal,
                    excpInfo ? &eInfo : NULL,
                    NULL,
                    NULL);
            }
        }

        int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, const TCHAR *szDumpNamePrefix) {
            static bool bFirstTime = true;

            if (!bFirstTime)
                return EXCEPTION_CONTINUE_SEARCH;

            bFirstTime = false;

            // create a unique string.
            //
            TCHAR szLocalTime[50] = { 0 };
            SYSTEMTIME st;
            GetLocalTime(&st);
            StringCchPrintf(szLocalTime, 50, TEXT("%04d%02d%02d.%02d.%02d.%02d.%04d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

            TCHAR szExeDir[MAX_PATH + 1] = { 0 };

            GetModuleFileName(NULL, szExeDir, MAX_PATH);

            if (TCHAR *p = lstrrchr(szExeDir, TEXT('\\'))) {
                *(p + 1) = 0;
            }

            DWORD dwCurrentPID = GetCurrentProcessId();

            TCHAR szDumpFileName[MAX_PATH + 1] = { 0 };
            _stprintf_s(szDumpFileName, MAX_PATH, TEXT("%s%s_%ld_%s.dmp"),
                        szExeDir, szDumpNamePrefix, dwCurrentPID, szLocalTime);

            HANDLE hMiniDumpFile = CreateFile(
                                       szDumpFileName,
                                       GENERIC_WRITE,
                                       0,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                       NULL);

            if (hMiniDumpFile != INVALID_HANDLE_VALUE) {
                DumpMiniDump(hMiniDumpFile, pExceptPtrs);

                CloseHandle(hMiniDumpFile);
                hMiniDumpFile = NULL;
            }

            return EXCEPTION_EXECUTE_HANDLER;
        }
    }
}

#endif