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


#include "ppxbase/file_util.h"
#include "ppxbase/stringencode.h"
#include <io.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <strsafe.h>
#include <Shlwapi.h>

namespace ppx {
    namespace base {
        static void AddFile(const wchar_t* szPath, const wchar_t* szDest, WIN32_FIND_DATAW file, int *pIgnoreNum);
        static void FileSearch(const wchar_t* szPath, const wchar_t* szDest, int *pIgnoreNum);

        static void AddFile(const wchar_t* szPath, const wchar_t* szDest, WIN32_FIND_DATAW file, int *pIgnoreNum) {
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (((wcscmp(file.cFileName, L".") != 0) && (wcscmp(file.cFileName, L"..") != 0))) {
                    wchar_t szTemp[MAX_PATH] = { 0 };
                    wcscpy_s(szTemp, MAX_PATH, szPath);
                    if (szTemp[wcslen(szTemp) - 1] != L'\\')
                        wcscat_s(szTemp, MAX_PATH, L"\\");

                    wcscat_s(szTemp, MAX_PATH, file.cFileName);
                    //create directory.
                    wchar_t szDir[MAX_PATH] = { 0 };
                    wcscat_s(szDir, MAX_PATH, szDest);
                    wcscat_s(szDir, MAX_PATH, L"\\");
                    wcscat_s(szDir, MAX_PATH, file.cFileName);
                    if ( _waccess_s(szDir, 0) != 0) {
                        if (!CreateDirectoryW(szDir, NULL)) {
                            if (pIgnoreNum) {
                                *pIgnoreNum += 1;
                            }
                        }
                    }

                    FileSearch(szTemp, szDir, pIgnoreNum);
                }
            }
            else {
                wchar_t szS[MAX_PATH] = { 0 };
                StringCchPrintfW(szS, MAX_PATH, L"%s\\%s", szPath, file.cFileName);
                wchar_t szD[MAX_PATH] = { 0 };
                StringCchPrintfW(szD, MAX_PATH, L"%s\\%s", szDest, file.cFileName);
                if (!CopyFileW(szS, szD, false)) {
                    if (pIgnoreNum) {
                        *pIgnoreNum += 1;
                    }
                }
            }
        }

        static void FileSearch(const wchar_t* szPath, const wchar_t* szDest, int *pIgnoreNum) {
            wchar_t szTemp[MAX_PATH] = { 0 };
            wcscpy_s(szTemp, MAX_PATH, szPath);

            size_t len = wcslen(szPath);
            if (szTemp[len - 1] != '\\')
                wcscat_s(szTemp, MAX_PATH, L"\\");
            wcscat_s(szTemp, MAX_PATH, L"*.*");

            WIN32_FIND_DATAW filedata;
            HANDLE fhandle = FindFirstFileW(szTemp, &filedata);
            if (fhandle != INVALID_HANDLE_VALUE) {
                AddFile(szPath, szDest, filedata, pIgnoreNum);

                while (FindNextFileW(fhandle, &filedata) != 0) {
                    AddFile(szPath, szDest, filedata, pIgnoreNum);
                }
            }
            FindClose(fhandle);
        }

        void CopyDir(const wchar_t* pszSource, const wchar_t* pszDest, bool bCopySource, int *pIgnoreNum) {
            wchar_t szSource[MAX_PATH] = { 0 };
            wchar_t szDest[MAX_PATH] = { 0 };
            wcscpy_s(szSource, MAX_PATH, pszSource);
            wcscpy_s(szDest, MAX_PATH, pszDest);

            if (bCopySource) {
                WIN32_FIND_DATAW filedata;
                HANDLE fhandle = FindFirstFileW(szSource, &filedata);
                size_t len = wcslen(szDest);
                if (szDest[len - 1] != L'\\')
                    wcscat_s(szDest, MAX_PATH, L"\\");
                wcscat_s(szDest, filedata.cFileName);
                CreateDirectoryW(szDest, NULL);
            }

            FileSearch(szSource, szDest, pIgnoreNum);
        }

		PPXBASE_API bool PathIsExists(const wchar_t *pszPath) {
			if (pszPath && _waccess_s(pszPath, 0) == 0)
				return true;
			return false;
		}

		bool DeleteDir(const char* pszDir) {
            if (!pszDir)
                return false;
            bool bRet = false;
            const int kBufSize = MAX_PATH * 4;
            HANDLE              hFind = INVALID_HANDLE_VALUE;
            CHAR                szTemp[kBufSize] = { 0 };
            WIN32_FIND_DATAA    wfd;

            StringCchCopyA(szTemp, kBufSize, pszDir);
            PathAddBackslashA(szTemp);
            StringCchCatA(szTemp, kBufSize, "*.*");

            hFind = FindFirstFileA(szTemp, &wfd);
            if (hFind == INVALID_HANDLE_VALUE) {
                return false;
            }

            do {
                if (lstrcmpiA(wfd.cFileName, ".") && lstrcmpiA(wfd.cFileName, "..")) {
                    StringCchCopyA(szTemp, kBufSize, pszDir);
                    PathAddBackslashA(szTemp);
                    StringCchCatA(szTemp, kBufSize, wfd.cFileName);

                    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        bRet = DeleteDir(szTemp);
                    }
                    else {
                        bRet = (DeleteFileA(szTemp) == TRUE);
                        if (!bRet) {
                            SetFileAttributesA(szTemp, FILE_ATTRIBUTE_NORMAL);
                            bRet = (DeleteFileA(szTemp) == TRUE);
                        }
                    }
                    if (!bRet) {
                        break;
                    }
                }

            } while (FindNextFileA(hFind, &wfd));

            FindClose(hFind);

            if (!bRet) {
                return bRet;
            }

            bRet = (RemoveDirectoryA(pszDir) == TRUE);
            if (!bRet) {
                DWORD dwAttr = GetFileAttributesA(pszDir);
                dwAttr &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);
                SetFileAttributesA(pszDir, dwAttr);
                bRet = (RemoveDirectoryA(pszDir) == TRUE);
            }

            return bRet;
        }

        bool CreateDir(const wchar_t* pszDir) {
            if (!pszDir)
                return false;
            wchar_t* p = NULL;
            wchar_t* szDirBuf = NULL;
            DWORD dwAttributes;
            size_t iLen = wcslen(pszDir);

            __try {
                szDirBuf = (wchar_t*)malloc((iLen + 1) * sizeof(wchar_t));
                if (szDirBuf == NULL)
                    return false;

                StringCchCopyW(szDirBuf, iLen + 1, pszDir);
                p = szDirBuf;

                if ((*p == L'\\') && (*(p + 1) == L'\\')) {
                    p += 2;
                    while (*p && *p != L'\\') p = CharNextW(p);
                    if (*p) p++;
                    while (*p && *p != L'\\') p = CharNextW(p);
                    if (*p) p++;
                }
                else if (*(p + 1) == L':') {
                    p += 2;
                    if (*p && (*p == L'\\')) p++;
                }

                while (*p) {
                    if (*p == L'\\') {
                        *p = L'\0';
                        dwAttributes = GetFileAttributesW(szDirBuf);

                        if (dwAttributes == 0xffffffff) {
                            if (!CreateDirectoryW(szDirBuf, NULL)) {
                                if (GetLastError() != ERROR_ALREADY_EXISTS) {
                                    free(szDirBuf);
                                    return false;
                                }
                            }
                        }
                        else {
                            if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
                                free(szDirBuf);
                                return false;
                            }
                        }

                        *p = L'\\';
                    }

                    p = CharNextW(p);
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                free(szDirBuf);
                return false;
            }

            free(szDirBuf);
            return true;
        }

        bool CreateDir(const char* pszDir) {
            if (!pszDir) return false;
            return CreateDir(base::AnsiToUnicode(pszDir).c_str());
        }
    }
}