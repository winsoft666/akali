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

#include "ppxbase/process_util.h"

#ifdef _WIN32
#include <tchar.h>
#include <shlobj.h> // for IsUserAnAdmin
#include <Shlwapi.h>
#include <strsafe.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <Psapi.h>
#include <shellapi.h>
#include "ppxbase/safe_release_macro.h"
#include "ppxbase/logging.h"

namespace ppx {
    namespace base {
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
            BOOL  fFound = FALSE;
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
                fFound = (lstrcmpi(pme->szModule, pszModName) == 0) ||
                         (lstrcmpi(pme->szExePath, pszModName) == 0);

                if (fFound)
                    break;
            }

            return fFound;
        }




        BOOL RunAsAdministrator(LPCTSTR szCommand, LPCTSTR szArgs, BOOL bWaitProcess) {
            SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
            sei.lpVerb = TEXT("runas");
            sei.lpFile = szCommand;
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

            if (GetTokenInformation(hToken, TokenElevationType, &elevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize)) {
                if (elevationType == TokenElevationTypeLimited) {
                    byte adminSID[SECURITY_MAX_SID_SIZE];
                    dwSize = sizeof(adminSID);
                    CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);

                    HANDLE hUnfilteredToken = NULL;
                    GetTokenInformation(hToken, TokenLinkedToken, (VOID *)&hUnfilteredToken, sizeof(HANDLE), &dwSize);

                    if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin))
                        bResult = TRUE;

                    CloseHandle(hUnfilteredToken);
                } else {
                    *pIsAdmin = IsUserAnAdmin();
                    bResult = TRUE;
                }
            }

            CloseHandle(hToken);

            return bResult;
        }

        BOOL TerminateProcess(DWORD dwProcessId, DWORD dwExitCode) {
            BOOL bResult = FALSE;

            HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);

            if (h)
                bResult = ::TerminateProcess(h, dwExitCode);

            return bResult;
        }

        BOOL TerminateProcess(LPCTSTR pszExeName, DWORD dwExitCode) {
            BOOL bResult = FALSE;
            ProcessFinder f(TH32CS_SNAPALL, 0);
            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);

            if (f.ProcessFind(pszExeName, &pe)) {
                bResult = TerminateProcess(pe.th32ProcessID, dwExitCode);
            }

            return bResult;
        }

        std::wstring GetCurrentProcessDirectoryW() {
            wchar_t szPath[MAX_PATH] = { 0 };
            GetModuleFileNameW(NULL, szPath, MAX_PATH);
            PathRemoveFileSpecW(szPath);
            PathAddBackslashW(szPath);
            return szPath;
        }

        std::string GetCurrentProcessDirectoryA() {
            char szPath[MAX_PATH] = { 0 };
            GetModuleFileNameA(NULL, szPath, MAX_PATH);
            PathRemoveFileSpecA(szPath);
            PathAddBackslashA(szPath);
            return szPath;
        }

        BOOL MakesureProcessSingleInstance(LPCTSTR pszUniqueName) {
            HANDLE hMutex = CreateEvent(NULL, TRUE, FALSE, pszUniqueName);
            DWORD dwLstErr = GetLastError();
            BOOL bOneInstanceCheck = TRUE;

            if (hMutex) {
                if (dwLstErr == ERROR_ALREADY_EXISTS) {
                    CloseHandle(hMutex);
                    bOneInstanceCheck = FALSE;
                }
            } else {
                if (dwLstErr == ERROR_ACCESS_DENIED)
                    bOneInstanceCheck = FALSE;
            }

            return bOneInstanceCheck;
        }


        BOOL EasyCreateProcess(LPCTSTR szCmdLine, LPPROCESS_INFORMATION lpProcessInfo, BOOL bInheritHandles /*= FALSE*/) {
            TCHAR szCL[MAX_PATH + 1] = { 0 };
            TCHAR szDir[MAX_PATH + 1] = { 0 };
            StringCchCopy(szCL, MAX_PATH, szCmdLine);
            StringCchCopy(szDir, MAX_PATH, szCmdLine);
            if (szDir[0] == L'"') {
                TCHAR *second = _tcschr(szDir + 1, L'"');
                *second = 0;
                StringCchCopy(szDir, MAX_PATH, szDir + 1);
            }
            PathRemoveFileSpec(szDir);
            STARTUPINFO si = { sizeof(si) };
            return CreateProcess(NULL, szCL, NULL, NULL, bInheritHandles, 0, NULL, szDir, &si, lpProcessInfo);
        }

        PPXBASE_API BOOL EasyCreateProcess(const std::wstring &strCmdLine, LPPROCESS_INFORMATION lpProcessInfo, BOOL bInheritHandles /*= FALSE*/) {
            return EasyCreateProcess(strCmdLine.c_str(), lpProcessInfo, bInheritHandles);
        }

		PPXBASE_API BOOL EasyCreateProcessUntilExit(const std::wstring &strCmdLine, DWORD* pExitCode, BOOL bInheritHandles /*= FALSE*/)
		{
			PROCESS_INFORMATION pi;
			BOOL bRet = EasyCreateProcess(strCmdLine.c_str(), &pi, bInheritHandles);
			if (bRet) {
				if (pi.hProcess) {
					WaitForSingleObject(pi.hProcess, INFINITE);
                    if(pExitCode)
					    GetExitCodeProcess(pi.hProcess, pExitCode);
				}
				SAFE_CLOSE(pi.hThread);
				SAFE_CLOSE(pi.hProcess);
			}
			return bRet;
		}

		BOOL CreateUserProcess(PCTSTR pszFilePath) {
            HANDLE hUserTokenDup = NULL;
            HANDLE hPToken = NULL;
            HANDLE hProcess = NULL;


            DWORD active_session_id = WTSGetActiveConsoleSessionId();

            // 查找winlogon.exe进程，判断其是否在当前会话
            ProcessFinder ph(TH32CS_SNAPPROCESS);
            PROCESSENTRY32 pe = { sizeof(pe) };

            if (!ph.ProcessFind(TEXT("winlogon.exe"), &pe)) {
                base::TraceMsgA("CreateUserProcess: not find winlogon.exe \n");
                return FALSE;
            }

            DWORD winlogon_process_id = pe.th32ProcessID;

            if (winlogon_process_id == 0)
                return FALSE;

            DWORD winlogon_session_id = 0;
            ProcessIdToSessionId(winlogon_process_id, &winlogon_session_id);

            if ( winlogon_session_id != active_session_id ) {
                base::TraceMsgA("CreateUserProcess: winlogon_session_id != active_session_id\n");
                return FALSE;
            }


            TOKEN_PRIVILEGES tp;
            LUID luid;
            hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogon_process_id);

            if ( !::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
                                     | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
                                     | TOKEN_READ | TOKEN_WRITE, &hPToken) ) {
                base::TraceMsgA("CreateUserProcess: OpenProcessToken failed, gle=%ld\n", GetLastError());
                return FALSE;
            }

            if ( !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid) ) {
                base::TraceMsgA("CreateUserProcess: LookupPrivilegeValue failed, gle=%ld\n", GetLastError());
                return FALSE;
            }

            tp.PrivilegeCount = 1;
            tp.Privileges[0].Luid = luid;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if ( !DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup) ) {
                base::TraceMsgA("CreateUserProcess: DuplicateTokenEx failed, gle=%ld\n", GetLastError());
                return FALSE;
            }

            //Adjust Token privilege
            if ( !SetTokenInformation(hUserTokenDup, TokenSessionId, (LPVOID)active_session_id, sizeof(DWORD)) ) {
                base::TraceMsgA("CreateUserProcess: SetTokenInformation failed, gle=%ld\n", GetLastError());
                return FALSE;
            }

            if ( !AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL) ) {
                base::TraceMsgA("CreateUserProcess: AdjustTokenPrivileges failed, gle=%ld\n", GetLastError());
                return FALSE;
            }

            if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
                base::TraceMsgA("CreateUserProcess: AdjustTokenPrivileges failed, gle=1300\n");
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
            TCHAR szDesktop[MAX_PATH] = { TEXT("winsta0\\default") };
            si.lpDesktop = szDesktop;
            ZeroMemory(&pi, sizeof(pi));

            TCHAR szTmp[1024] = { 0 };
            StringCchCopy(szTmp, 1024, pszFilePath);
            BOOL ret = CreateProcessAsUserW(
                           hUserTokenDup,            // client's access token
                           NULL,         // file to execute
                           szTmp,              // command line
                           NULL,              // pointer to process SECURITY_ATTRIBUTES
                           NULL,              // pointer to thread SECURITY_ATTRIBUTES
                           FALSE,             // handles are not inheritable
                           dwCreationFlags,  // creation flags
                           pEnv,              // pointer to new environment block
                           NULL,              // name of current directory
                           &si,               // pointer to STARTUPINFO structure
                           &pi                // receives information about new process
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

        PPXBASE_API BOOL UIPIMsgFilter(HWND hWnd, UINT uMessageID, BOOL bAllow) {
            OSVERSIONINFO VersionTmp;
            VersionTmp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
            GetVersionEx(&VersionTmp);
            BOOL res = FALSE;

            if (VersionTmp.dwMajorVersion >= 6) { // vista above.
                BOOL(WINAPI * pfnChangeMessageFilterEx)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
                BOOL(WINAPI * pfnChangeMessageFilter)(UINT, DWORD);

                CHANGEFILTERSTRUCT filterStatus;
                filterStatus.cbSize = sizeof(CHANGEFILTERSTRUCT);

                HINSTANCE hlib = LoadLibrary(_T("user32.dll"));

                if (hlib != NULL) {
                    (FARPROC &)pfnChangeMessageFilterEx = GetProcAddress(hlib, "ChangeWindowMessageFilterEx");

                    if (pfnChangeMessageFilterEx != NULL && hWnd != NULL) {
                        res = pfnChangeMessageFilterEx(hWnd, uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE), &filterStatus);
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

        void RecursiveTerminateProcess(LPCTSTR szAppDir, bool exclude_self) {
            TCHAR szTemp[MAX_PATH] = { 0 };
            StringCchCopy(szTemp, MAX_PATH, szAppDir);

            size_t len = _tcslen(szAppDir);

            if (szTemp[len - 1] != TEXT('\\'))
                _tcscat_s(szTemp, MAX_PATH, TEXT("\\"));

            StringCchCat(szTemp, MAX_PATH, TEXT("*.*"));

            WIN32_FIND_DATA filedata;
            HANDLE fhandle = FindFirstFile(szTemp, &filedata);

            if (fhandle != INVALID_HANDLE_VALUE) {
                if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) && (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
                        TCHAR szTemp[MAX_PATH] = { 0 };
                        StringCchCat(szTemp, MAX_PATH, szAppDir);

                        if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
                            StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

                        _tcscat_s(szTemp, MAX_PATH, filedata.cFileName);
                        RecursiveTerminateProcess(szTemp, exclude_self);
                    }
                } else {
                    TCHAR *p = _tcsrchr(filedata.cFileName, TEXT('.'));

                    if (p) {
                        if (_tcscmp(p, TEXT(".exe")) == 0) {
                            bool teminate = true;
                            if (exclude_self) {
                                TCHAR szSelf[MAX_PATH] = { 0 };
                                GetModuleFileName(NULL, szSelf, MAX_PATH);
                                LPWSTR pSelf = PathFindFileName(szSelf);
                                if (_tcscmp(filedata.cFileName, pSelf) == 0)
                                    teminate = false;
                            }
                            if(teminate)
                                while (TerminateProcess(filedata.cFileName, 333));
                        }
                    }
                }

                while (FindNextFile(fhandle, &filedata) != 0) {
                    if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) && (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
                            TCHAR szTemp[MAX_PATH] = { 0 };
                            StringCchCopy(szTemp, MAX_PATH, szAppDir);

                            if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
                                StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

                            StringCchCat(szTemp, MAX_PATH, filedata.cFileName);
                            RecursiveTerminateProcess(szTemp, exclude_self);
                        }
                    } else {
                        TCHAR *p = _tcsrchr(filedata.cFileName, TEXT('.'));

                        if (p) {
                            if (_tcscmp(p, TEXT(".exe")) == 0) {
                                bool teminate = true;
                                if (exclude_self) {
                                    TCHAR szSelf[MAX_PATH] = { 0 };
                                    GetModuleFileName(NULL, szSelf, MAX_PATH);
                                    LPWSTR pSelf = PathFindFileName(szSelf);
                                    if (_tcscmp(filedata.cFileName, pSelf) == 0)
                                        teminate = false;
                                }
                                if (teminate)
                                    while (TerminateProcess(filedata.cFileName, 333));
                            }
                        }
                    }
                }
            }

            FindClose(fhandle);
        }

        std::wstring GetProcessPath(DWORD dwProcessID) {
            std::wstring strPath;
            wchar_t Filename[MAX_PATH] = { 0 };
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
            if (hProcess == NULL)
                return strPath;
            HMODULE hModule = NULL;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
                if (GetModuleFileNameExW(hProcess, hModule, Filename, MAX_PATH)) {
                    strPath = Filename;
                }
            }
            else {
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, Filename, &size)) {
                    strPath = Filename;
                }
            }
            CloseHandle(hProcess);

            return strPath;
        }
    }
}
#endif