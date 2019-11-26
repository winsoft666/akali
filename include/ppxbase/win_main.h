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

#ifndef PPX_BASE_WIN_MAIN_H_
#define PPX_BASE_WIN_MAIN_H_
#pragma once

#if (defined _WIN32 || defined WIN32)
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

        PPXBASE_API int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, const TCHAR *szDumpNamePrefix);
        PPXBASE_API void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo);
    }
}


#define WINMAIN_BEGIN(szDumpNamePrefix) \
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine);\
    LONG WINAPI __96A9695E_UnhandledExceptionHandler( _EXCEPTION_POINTERS *pExceptionInfo ) \
    { \
        OutputDebugString(TEXT("Create a dump file sine an exception occurred in sub-thread.\n")); \
        int iRet = ppx::base::RecordExceptionInfo(pExceptionInfo, szDumpNamePrefix); \
        return iRet; \
    } \
    int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) \
    { \
        UNREFERENCED_PARAMETER(hPrevInstance); \
        UNREFERENCED_PARAMETER(nCmdShow); \
        ::SetUnhandledExceptionFilter( __96A9695E_UnhandledExceptionHandler );\
        int ret = 0;\
        __try\
        {\
            ret = __96A9695E_RUN_WINMAIN_FUNC(hInstance, lpCmdLine);\
        }\
        __except(ppx::base::RecordExceptionInfo(GetExceptionInformation(), szDumpNamePrefix))\
        {\
            OutputDebugString(TEXT("Create a dump file sine an exception occurred in main-thread.\n")); \
        }\
        return ret;\
    }\
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine) \
    {

#define WINMAIN_END }




#define WMAIN_BEGIN(szDumpName) \
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]);\
    LONG WINAPI __96A9695E_UnhandledExceptionHandler( _EXCEPTION_POINTERS *pExceptionInfo ) \
    { \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n")); \
        int iRet = ppx::base::RecordExceptionInfo(pExceptionInfo, szDumpName); \
        return iRet; \
    } \
    int wmain(int argc, wchar_t* argv[])\
    { \
        ::SetUnhandledExceptionFilter( __96A9695E_UnhandledExceptionHandler );\
        int ret = 0;\
        __try\
        {\
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);\
        }\
        __except(ppx::base::RecordExceptionInfo(GetExceptionInformation(), szDumpName))\
        {\
            OutputDebugString(TEXT("Create a dump file since an exception occurred in main-thread.\n")); \
        }\
        return ret;\
    }\
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]) \
    {



#define MAIN_BEGIN(szDumpName) \
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]);\
    LONG WINAPI __96A9695E_UnhandledExceptionHandler( _EXCEPTION_POINTERS *pExceptionInfo ) \
    { \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n")); \
        int iRet = ppx::base::RecordExceptionInfo(pExceptionInfo, szDumpName); \
        return iRet; \
    } \
    int main(int argc, char* argv[])\
    { \
        ::SetUnhandledExceptionFilter( __96A9695E_UnhandledExceptionHandler );\
        int ret = 0;\
        __try\
        {\
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);\
        }\
        __except(ppx::base::RecordExceptionInfo(GetExceptionInformation(), szDumpName))\
        {\
            OutputDebugString(TEXT("Create a dump file since an exception occurred in main-thread.\n")); \
        }\
        return ret;\
    }\
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]) \
    {



#define WMAIN_END }
#define MAIN_END }

#endif

#endif // !PPX_BASE_WIN_MAIN_H_
