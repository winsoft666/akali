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

#ifndef AKALI_PROCESS_UTIL_H_
#define AKALI_PROCESS_UTIL_H_
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <TlHelp32.h>

#include <string>

#include "akali/deprecation.h"
#include "akali_export.h"

#pragma comment(lib, "Userenv.lib")

namespace akali {
class AKALI_API ProcessFinder {
public:
  // If use default param, CreateToolhelp32Snapshot With dwFlags =
  // TH32CS_SNAPALL, th32ProcessID = 0
  //
  ProcessFinder(DWORD dwFlags = 0, DWORD dwProcessID = 0);
  ~ProcessFinder();
  BOOL CreateSnapShot(DWORD dwFlag, DWORD dwProcessID);
  BOOL ProcessFirst(PPROCESSENTRY32 ppe) const;
  BOOL ProcessNext(PPROCESSENTRY32 ppe) const;
  // Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
  BOOL ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const;
  BOOL ProcessFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf = false) const;
  BOOL ModuleFirst(PMODULEENTRY32 pme) const;
  BOOL ModuleNext(PMODULEENTRY32 pme) const;
  BOOL ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const;
  BOOL ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const;

private:
  HANDLE m_hSnapShot;
};

AKALI_API BOOL RunAsAdministrator(LPCTSTR szCommand, LPCTSTR szArgs, LPCTSTR szDir,
                                    BOOL bWaitProcess);
AKALI_API BOOL EnablePrivilege(LPCTSTR szPrivilege, BOOL fEnable);
AKALI_API BOOL CheckProcessUserIsAdmin(BOOL *pIsAdmin);

AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process::Kill instead.")
BOOL TerminateProcess(DWORD dwProcessId, DWORD dwExitCode);

AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process::Kill instead.")
BOOL TerminateProcess(LPCTSTR pszExeName, DWORD dwExitCode);

// terminate all process that EXE file in szAppDir directory.
AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process::RecursiveKill "
               "instead.")
void RecursiveTerminateProcess(LPCTSTR szAppDir, bool exclude_self);

AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process::GetSelfDir "
                 "instead.")
std::wstring GetCurrentProcessDirectoryW();

AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process::GetSelfDir "
                 "instead.")
std::string GetCurrentProcessDirectoryA();
#if defined(UNICODE) || defined(_UNICODE)
#define GetCurrentProcessDirectory akali::GetCurrentProcessDirectoryW
#else
#define GetCurrentProcessDirectory akali::GetCurrentProcessDirectoryA
#endif

AKALI_API BOOL MakesureProcessSingleInstance(LPCTSTR pszUniqueName);

// 如果路径中含有空格，必须使用双引号引用
//
AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process instead.")
BOOL EasyCreateProcess(LPCTSTR szCmdLine, LPPROCESS_INFORMATION lpProcessInfo,
                                   BOOL bInheritHandles = FALSE);
AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process instead.")
BOOL EasyCreateProcess(const std::wstring &strCmdLine,
                                   LPPROCESS_INFORMATION lpProcessInfo,
                                   BOOL bInheritHandles = FALSE);
AKALI_API
AKALI_DEPRECATED("This function will be removed. Consider using Process instead.")
BOOL EasyCreateProcessUntilExit(const std::wstring &strCmdLine, DWORD *pExitCode,
                                BOOL bInheritHandles = FALSE);
AKALI_API BOOL CreateUserProcess(PCTSTR pszFilePath);

// Please see: https://blog.csdn.net/china_jeffery/article/details/88225810
//
AKALI_API BOOL UIPIMsgFilter(HWND hWnd, UINT uMessageID, BOOL bAllow);
} // namespace akali
#endif
#endif // AKALI_WIN_PROCESS_H_
