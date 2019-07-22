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


#include "ppxbase/ini.h"
#include <strsafe.h>
#include <assert.h>
#include "ppxbase/stringencode.h"

#ifdef _WIN32

namespace ppx {
    namespace base {
        Ini::Ini() {
            ZeroMemory(m_szIniFile, sizeof(m_szIniFile));
        }

        Ini::~Ini() {
        }

        void Ini::SetIniFilePath(LPCWSTR pszIniFile) {
            StringCchCopyW(m_szIniFile, MAX_PATH, pszIniFile);
        }

        LPCWSTR Ini::GetIniFilePath() {
            return m_szIniFile;
        }

        bool Ini::ReadInt(LPCWSTR pszItem, LPCWSTR pszSubItem, UINT &ValueInt) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            INT iDefault = 0;
            SetLastError(0);
            UINT ret = GetPrivateProfileIntW(pszItem, pszSubItem, iDefault, m_szIniFile);
            DWORD dwGLE = GetLastError();
			if (dwGLE == 0) {
				ValueInt = ret;
				return true;
			}
			return false;
        }

        LPCWSTR Ini::ReadString(LPCWSTR pszItem, LPCWSTR pszSubItem, LPCWSTR pszDefault, LPTSTR pszString, WORD wMaxCount) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            GetPrivateProfileStringW(pszItem, pszSubItem, pszDefault, pszString, wMaxCount, m_szIniFile);
            return pszString;
        }

        bool Ini::ReadString(LPCWSTR pszItem, LPCWSTR pszSubItem, std::wstring &strString) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            bool ret = false;
            int iBufSize = 255;
            WCHAR *pBuf = NULL;
            do 
            {
                pBuf = (WCHAR*)malloc(iBufSize * sizeof(WCHAR));
				SetLastError(0);
                DWORD dwRet = GetPrivateProfileStringW(pszItem, pszSubItem, L"" ,pBuf, iBufSize, m_szIniFile);
                DWORD dwGLE = GetLastError();
                if (dwRet == 0) {
                    ret = (dwGLE == 0);
                    break;
                }
                else if (dwRet == iBufSize - 1) {
                    free(pBuf);
                    pBuf = NULL;
                    iBufSize += 1024;
                }
                else {
                    ret = true;
                    break;
                }
            } while (true);

			if (ret) {
				strString = pBuf;
			}
            free(pBuf);

            return ret;
        }

		bool Ini::WriteInt(LPCWSTR pszItem, LPCWSTR pszSubItem, LONG ValueInt) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0 || !pszItem || !pszSubItem)
                return false;
            WCHAR szValue[50];
            StringCchPrintfW(szValue, 50, L"%ld", ValueInt);
            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteRGBColor(LPCWSTR pszItem, LPCWSTR pszSubItem, COLORREF ValueColor) {
            WCHAR szValue[50];
            StringCchPrintfW(szValue, 50, L"%d,%d,%d", GetRValue(ValueColor), GetGValue(ValueColor), GetBValue(ValueColor));

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WritePoint(LPCWSTR pszItem, LPCWSTR pszSubItem, POINT ValuePoint) {
            WCHAR szValue[50];
            StringCchPrintfW(szValue, 50, L"%d,%d", ValuePoint.x, ValuePoint.y);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteSize(LPCWSTR pszItem, LPCWSTR pszSubItem, SIZE ValueSize) {
            WCHAR szValue[50];
            StringCchPrintfW(szValue, 50, L"%d,%d", ValueSize.cx, ValueSize.cy);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteRect(LPCWSTR pszItem, LPCWSTR pszSubItem, RECT ValueRect) {
            WCHAR szValue[50];
            StringCchPrintfW(szValue, 50, L"%d,%d,%d,%d", ValueRect.left, ValueRect.top, ValueRect.right, ValueRect.bottom);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteString(LPCWSTR pszItem, LPCWSTR pszSubItem, LPCWSTR ValueString) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0 || ValueString == NULL || !pszItem || !pszSubItem)
                return false;

            return WritePrivateProfileString(pszItem, pszSubItem, ValueString, m_szIniFile) == TRUE;
        }

		bool Ini::WriteString(LPCWSTR pszItem, LPCWSTR pszSubItem, const std::wstring& ValueString) {
			return WriteString(pszItem, pszSubItem, ValueString.c_str());
		}

		bool Ini::ReadRect(LPCWSTR pszItem, LPCWSTR pszSubItem, RECT & ValueRect) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            WCHAR szReadData[64] = L"";
            ZeroMemory(&ValueRect, sizeof(ValueRect));

            GetPrivateProfileStringW(pszItem, pszSubItem, L"", szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCWSTR pszString = szReadData;
                ValueRect.left = SwitchStringToValue(pszString);
                ValueRect.top = SwitchStringToValue(pszString);
                ValueRect.right = SwitchStringToValue(pszString);
                ValueRect.bottom = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadSize(LPCWSTR pszItem, LPCWSTR pszSubItem, SIZE & ValueSize) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            WCHAR szReadData[64] = L"";
            ZeroMemory(&ValueSize, sizeof(ValueSize));

            GetPrivateProfileStringW(pszItem, pszSubItem, L"", szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCWSTR pszString = szReadData;
                ValueSize.cx = SwitchStringToValue(pszString);
                ValueSize.cy = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadPoint(LPCWSTR pszItem, LPCWSTR pszSubItem, POINT & ValuePoint) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            WCHAR szReadData[64] = L"";
            ZeroMemory(&ValuePoint, sizeof(ValuePoint));

            GetPrivateProfileStringW(pszItem, pszSubItem, L"", szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCWSTR pszString = szReadData;
                ValuePoint.x = SwitchStringToValue(pszString);
                ValuePoint.y = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadColor(LPCWSTR pszItem, LPCWSTR pszSubItem, COLORREF & ValueColor) {
            assert(m_szIniFile[0] != 0);
            if (wcslen(m_szIniFile) == 0)
                return false;
            WCHAR szReadData[64] = L"";
            ZeroMemory(&ValueColor, sizeof(ValueColor));
            GetPrivateProfileStringW(pszItem, pszSubItem, L"", szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCWSTR pszString = szReadData;
                ValueColor = RGB(SwitchStringToValue(pszString), SwitchStringToValue(pszString), SwitchStringToValue(pszString));

                return true;
            }

            return false;
        }

        LONG Ini::SwitchStringToValue(LPCWSTR & pszSring) {
            assert((pszSring != NULL) && (pszSring[0] != 0));
            if ((pszSring == NULL) || (pszSring[0] == 0)) return 0L;

            while (((pszSring[0] > 0) && (pszSring[0] < L'0')) || (pszSring[0] > L'9')) 
                pszSring++;

            LONG lValue = 0L;
            while ((pszSring[0] >= L'0') && (pszSring[0] <= L'9')) {
                lValue = lValue * 10L + pszSring[0] - L'0';
                ++pszSring;
            }

            return lValue;
        }
    }
}
#endif
