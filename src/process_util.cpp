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

#include "akali/process_util.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <tchar.h>
#include <shlobj.h> // for IsUserAnAdmin
#include <Shlwapi.h>
#include <strsafe.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <Psapi.h>
#include <shellapi.h>
#include "akali/macros.h"

namespace akali {
ProcessFinder::ProcessFinder(DWORD dwFlags /* = 0*/, DWORD dwProcessID /* = 0*/) {
  m_hSnapShot = INVALID_HANDLE_VALUE;
  if (dwFlags == 0)
    dwFlags = TH32CS_SNAPALL;
  CreateSnapShot(dwFlags, dwProcessID);
}

ProcessFinder::~ProcessFinder() {
  if (m_hSnapShot != INVALID_HANDLE_VALUE) {
    CloseHandle(m_hSnapShot);
    m_hSnapShot = INVALID_HANDLE_VALUE;
  }
}

BOOL ProcessFinder::CreateSnapShot(DWORD dwFlag, DWORD dwProcessID) {
  if (m_hSnapShot != INVALID_HANDLE_VALUE)
    CloseHandle(m_hSnapShot);

  if (dwFlag == 0)
    m_hSnapShot = INVALID_HANDLE_VALUE;
  else
    m_hSnapShot = CreateToolhelp32Snapshot(dwFlag, dwProcessID);

  return (m_hSnapShot != INVALID_HANDLE_VALUE);
}

BOOL ProcessFinder::ProcessFirst(PPROCESSENTRY32 ppe) const {
  BOOL fOk = Process32First(m_hSnapShot, ppe);

  if (fOk && (ppe->th32ProcessID == 0))
    fOk = ProcessNext(ppe); // remove the "[System Process]" (PID = 0)

  return fOk;
}

BOOL ProcessFinder::ProcessNext(PPROCESSENTRY32 ppe) const {
  BOOL fOk = Process32Next(m_hSnapShot, ppe);

  if (fOk && (ppe->th32ProcessID == 0))
    fOk = ProcessNext(ppe); // remove the "[System Process]" (PID = 0)

  return fOk;
}

BOOL ProcessFinder::ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const {
  BOOL fFound = FALSE;

  for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) {
    fFound = (ppe->th32ProcessID == dwProcessId);

    if (fFound)
      break;
  }

  return fFound;
}

BOOL ProcessFinder::ProcessFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf) const {
  BOOL fFound = FALSE;
  DWORD dwCurrentPID = GetCurrentProcessId();

  for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) {
    fFound = lstrcmpi(ppe->szExeFile, pszExeName) == 0;

    if (fFound) {
      if (bExceptSelf) {
        if (ppe->th32ProcessID == dwCurrentPID) {
          fFound = FALSE;
          continue;
        }
      }
      break;
    }
  }

  return fFound;
}

BOOL ProcessFinder::ModuleFirst(PMODULEENTRY32 pme) const {
  return (Module32First(m_hSnapShot, pme));
}

BOOL ProcessFinder::ModuleNext(PMODULEENTRY32 pme) const {
  return (Module32Next(m_hSnapShot, pme));
}

BOOL ProcessFinder::ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {
  BOOL fFound = FALSE;

  for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
    fFound = (pme->modBaseAddr == pvBaseAddr);

    if (fFound)
      break;
  }

  return fFound;
}

BOOL ProcessFinder::ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
  BOOL fFound = FALSE;

  for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
    fFound =
        (lstrcmpi(pme->szModule, pszModName) == 0) || (lstrcmpi(pme->szExePath, pszModName) == 0);

    if (fFound)
      break;
  }

  return fFound;
}

BOOL RunAsAdministrator(LPCTSTR szCommand, LPCTSTR szArgs, LPCTSTR szDir, BOOL bWaitProcess) {
  SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
  sei.lpVerb = TEXT("runas");
  sei.lpFile = szCommand;
  sei.lpDirectory = szDir;
  sei.lpParameters = szArgs;
  sei.nShow = SW_SHOWNORMAL;

  BOOL ret = ShellExecuteEx(&sei);

  if (ret && bWaitProcess)
    WaitForSingleObject(sei.hProcess, INFINITE);

  return ret;
}

BOOL EnablePrivilege(LPCTSTR szPrivilege, BOOL fEnable) {
  BOOL fOk = FALSE;
  HANDLE hToken = NULL;

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
    tp.Privileges->Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    fOk = (GetLastError() == ERROR_SUCCESS);

    CloseHandle(hToken);
  }

  return fOk;
}

BOOL CheckProcessUserIsAdmin(BOOL *pIsAdmin) {
  BOOL bResult = FALSE;
  HANDLE hToken = FALSE;
  DWORD dwSize;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    return FALSE;

  TOKEN_ELEVATION_TYPE elevationType;

  if (GetTokenInformation(hToken, TokenElevationType, &elevationType, sizeof(TOKEN_ELEVATION_TYPE),
                          &dwSize)) {
    if (elevationType == TokenElevationTypeLimited) {
      byte adminSID[SECURITY_MAX_SID_SIZE];
      dwSize = sizeof(adminSID);
      CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);

      HANDLE hUnfilteredToken = NULL;
      GetTokenInformation(hToken, TokenLinkedToken, (VOID *)&hUnfilteredToken, sizeof(HANDLE),
                          &dwSize);

      if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin))
        bResult = TRUE;

      CloseHandle(hUnfilteredToken);
    }
    else {
      *pIsAdmin = IsUserAnAdmin();
      bResult = TRUE;
    }
  }

  CloseHandle(hToken);

  return bResult;
}

BOOL CreateUserProcess(PCTSTR pszFilePath) {
  HANDLE hUserTokenDup = NULL;
  HANDLE hPToken = NULL;
  HANDLE hProcess = NULL;

  DWORD active_session_id = WTSGetActiveConsoleSessionId();

  // 查找winlogon.exe进程，判断其是否在当前会话
  ProcessFinder ph(TH32CS_SNAPPROCESS);
  PROCESSENTRY32 pe = {sizeof(pe)};

  if (!ph.ProcessFind(TEXT("winlogon.exe"), &pe)) {
    return FALSE;
  }

  DWORD winlogon_process_id = pe.th32ProcessID;

  if (winlogon_process_id == 0)
    return FALSE;

  DWORD winlogon_session_id = 0;
  ProcessIdToSessionId(winlogon_process_id, &winlogon_session_id);

  if (winlogon_session_id != active_session_id) {
    return FALSE;
  }

  TOKEN_PRIVILEGES tp;
  LUID luid;
  hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogon_process_id);

  if (!::OpenProcessToken(hProcess,
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_DUPLICATE |
                              TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID | TOKEN_READ |
                              TOKEN_WRITE,
                          &hPToken)) {
    return FALSE;
  }

  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
    return FALSE;
  }

  tp.PrivilegeCount = 1;
  tp.Privileges[0].Luid = luid;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if (!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary,
                        &hUserTokenDup)) {
    return FALSE;
  }

  // Adjust Token privilege
  if (!SetTokenInformation(hUserTokenDup, TokenSessionId, (LPVOID)active_session_id,
                           sizeof(DWORD))) {
    return FALSE;
  }

  if (!AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
                             (PTOKEN_PRIVILEGES)NULL, NULL)) {
    return FALSE;
  }

  if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
    return FALSE;
  }

  DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
  LPVOID pEnv = NULL;

  if (CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE))
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
  else
    pEnv = NULL;

  PROCESS_INFORMATION pi;
  STARTUPINFO si;

  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  TCHAR szDesktop[MAX_PATH] = {TEXT("winsta0\\default")};
  si.lpDesktop = szDesktop;
  ZeroMemory(&pi, sizeof(pi));

  TCHAR szTmp[1024] = {0};
  StringCchCopy(szTmp, 1024, pszFilePath);
  BOOL ret = CreateProcessAsUserW(hUserTokenDup,   // client's access token
                                  NULL,            // file to execute
                                  szTmp,           // command line
                                  NULL,            // pointer to process SECURITY_ATTRIBUTES
                                  NULL,            // pointer to thread SECURITY_ATTRIBUTES
                                  FALSE,           // handles are not inheritable
                                  dwCreationFlags, // creation flags
                                  pEnv,            // pointer to new environment block
                                  NULL,            // name of current directory
                                  &si,             // pointer to STARTUPINFO structure
                                  &pi              // receives information about new process
  );

  if (hProcess)
    CloseHandle(hProcess);

  if (hUserTokenDup)
    CloseHandle(hUserTokenDup);

  if (hPToken)
    CloseHandle(hPToken);

  SAFE_CLOSE(pi.hThread);
  SAFE_CLOSE(pi.hProcess);

  return (TRUE);
}

BOOL UIPIMsgFilter(HWND hWnd, UINT uMessageID, BOOL bAllow) {
  OSVERSIONINFO VersionTmp;
  VersionTmp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&VersionTmp);
  BOOL res = FALSE;

  if (VersionTmp.dwMajorVersion >= 6) { // vista above.
    BOOL(WINAPI * pfnChangeMessageFilterEx)
    (HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
    BOOL(WINAPI * pfnChangeMessageFilter)(UINT, DWORD);

    CHANGEFILTERSTRUCT filterStatus;
    filterStatus.cbSize = sizeof(CHANGEFILTERSTRUCT);

    HINSTANCE hlib = LoadLibrary(_T("user32.dll"));

    if (hlib != NULL) {
      (FARPROC &)pfnChangeMessageFilterEx = GetProcAddress(hlib, "ChangeWindowMessageFilterEx");

      if (pfnChangeMessageFilterEx != NULL && hWnd != NULL) {
        res = pfnChangeMessageFilterEx(hWnd, uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE),
                                       &filterStatus);
      }

      // If failed, try again.
      if (!res) {
        (FARPROC &)pfnChangeMessageFilter = GetProcAddress(hlib, "ChangeWindowMessageFilter");

        if (pfnChangeMessageFilter != NULL) {
          res = pfnChangeMessageFilter(uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE));
        }
      }
    }

    if (hlib != NULL) {
      FreeLibrary(hlib);
    }
  }
  else {
    res = TRUE;
  }

  return res;
}
} // namespace akali
#endif