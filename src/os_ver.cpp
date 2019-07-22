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


#include "ppxbase/os_ver.h"
#ifdef _WIN32
#include <strsafe.h>

/*
Operating system                            dwMajorVersion   dwMinorVersion              Other
Windows 10 Technical Preview                     10               0                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server Technical Preview                 10               0                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8.1                                      6                3                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012 R2                           6                3                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8                                        6                2                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012                              6                2                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 7                                        6                1                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2008 R2                           6                1                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Server 2008                              6                0                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Vista                                    6                0                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2003 R2                           5                2                  GetSystemMetrics(SM_SERVERR2) != 0
Windows Home Server                              5                2                  OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
Windows Server 2003                              5                2                  GetSystemMetrics(SM_SERVERR2) == 0
Windows XP Professional x64 Edition              5                2                  (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
Windows XP                                       5                1                  Not applicable
Windows 2000                                     5                0                  Not applicable
*/
#define VER_SUITE_WH_SERVER                 0x00008000

#endif


namespace ppx {
	namespace base {
#ifdef _WIN32
		OSVersion::OSVersion() {
			memset(&osvi, 0, sizeof(OSVERSIONINFOEXW));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		}

		// 2003R2, 2003, XP ....
		//
		bool OSVersion::AtMostWin2003R2(void) {
			____Init();
			return (osvi.dwMajorVersion <= 5 && osvi.dwMinorVersion <= 2);
		}

		// Vista, 7, 8 ....
		//
		bool OSVersion::AtLeastWinVista(void) {
			____Init();
			return (osvi.dwMajorVersion >= 6);
		}

		void OSVersion::GetMajorMinorBuild(DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuild) {
			____Init();
			dwMajor = osvi.dwMajorVersion;
			dwMinor = osvi.dwMinorVersion;
			dwBuild = osvi.dwBuildNumber;
		}


		bool OSVersion::IsWinXPProX64() {
			____Init();
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);

			return (osvi.dwMajorVersion == 5 &&
				osvi.dwMinorVersion == 2 &&
				osvi.wProductType == VER_NT_WORKSTATION &&
				sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
		}


		OSVer OSVersion::GetOsVersion(void) {
			____Init();

			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				return WIN_2000;
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				return WIN_XP;
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
				if (IsWinXPProX64())
					return WIN_XP_PRO;
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					return WIN_2003;
				else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
					return WIN_HOME;
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					return WIN_2003_R2;
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return WIN_VISTA;
				else if (osvi.wProductType != VER_NT_WORKSTATION)
					return WIN_2008;
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return WIN_7;
				else if (osvi.wProductType != VER_NT_WORKSTATION)
					return WIN_2008_R2;
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return WIN_8;
				else if (osvi.wProductType != VER_NT_WORKSTATION)
					return WIN_2012;
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return WIN_8_1;
				else if (osvi.wProductType != VER_NT_WORKSTATION)
					return WIN_2012_R2;
			}
			else if (osvi.dwMajorVersion == 10) {
                if (osvi.wProductType == VER_NT_WORKSTATION) {
                    if (osvi.dwBuildNumber < 10586) {
                        return WIN_10;
                    }
                    else if (osvi.dwBuildNumber < 14393) {
                        return WIN_10_TH2;
                    }
                    else if (osvi.dwBuildNumber < 15063) {
                        return WIN_10_RS1;
                    }
                    else if (osvi.dwBuildNumber < 16299) {
                        return WIN_10_RS2;
                    }
                    else {
                        return WIN_10_RS3;
                    }
                }
                else if (osvi.wProductType != VER_NT_WORKSTATION) {
                    return WIN_2016;
                }
			}

			return WIN_LAST;
		}

		std::string OSVersion::GetOsVerSummary(void) {
			____Init();
			char szBuf[100] = { 0 };
			StringCchPrintfA(szBuf, 100, "build:%d.%d.%d type:%d", osvi.dwMajorVersion,
				osvi.dwMinorVersion,
				osvi.dwBuildNumber,
				osvi.wProductType
			);
			return szBuf;
		}


		void OSVersion::____Init() {
			if (osvi.dwMajorVersion == 0) {
				GetVersionEx((LPOSVERSIONINFO)&osvi);
			}
		}

		bool OSVersion::IsWin64() {
			typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
			static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
			BOOL bIsWow64 = FALSE;

			if (NULL == fnIsWow64Process) {
				HMODULE h = GetModuleHandleW(L"kernel32");
				if (h)
					fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(h, "IsWow64Process");
			}

			if (NULL != fnIsWow64Process) {
				fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
			}

			return bIsWow64 == 1;
		}
#endif
	}
}