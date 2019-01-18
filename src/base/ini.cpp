/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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


#include "base/ini.h"
#include <strsafe.h>
#include <assert.h>
#include "base/stringencode.h"

#ifdef _WIN32

namespace ppx {
    namespace base {
        Ini::Ini() {
            ZeroMemory(m_szIniFile, sizeof(m_szIniFile));
        }

        Ini::~Ini() {
        }

        void Ini::SetIniFilePath(LPCTSTR pszIniFile) {
            StringCchCopy(m_szIniFile, MAX_PATH, pszIniFile);
        }

        LPCTSTR Ini::GetIniFilePath() {
            return m_szIniFile;
        }

        UINT Ini::ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, INT nDefault) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            UINT uReadData = GetPrivateProfileInt(pszItem, pszSubItem, nDefault, m_szIniFile);
            return uReadData;
        }

        bool Ini::ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, UINT &ValueInt) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            INT iDefault = 0;
            ValueInt = GetPrivateProfileInt(pszItem, pszSubItem, iDefault, m_szIniFile);
            DWORD dwGLE = GetLastError();

            return (dwGLE == 0);
        }

        LPCTSTR Ini::ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            GetPrivateProfileString(pszItem, pszSubItem, pszDefault, pszString, wMaxCount, m_szIniFile);
            return pszString;
        }

        bool Ini::ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, String &strString) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            bool ret = false;
            int iBufSize = 255;
            TCHAR *pBuf = NULL;
            do 
            {
                pBuf = (TCHAR*)malloc(iBufSize * sizeof(TCHAR));
                DWORD dwRet = GetPrivateProfileString(pszItem, pszSubItem, TEXT("") ,pBuf, iBufSize, m_szIniFile);
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
           
            strString = pBuf;
            free(pBuf);

            return ret;
        }

		bool Ini::WriteInt(LPCTSTR pszItem, LPCTSTR pszSubItem, LONG ValueInt) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0 || !pszItem || !pszSubItem)
                return false;
            TCHAR szValue[50];
            StringCchPrintf(szValue, 50, TEXT("%ld"), ValueInt);
            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteRGBColor(LPCTSTR pszItem, LPCTSTR pszSubItem, COLORREF ValueColor) {
            TCHAR szValue[50];
            StringCchPrintf(szValue, 50, TEXT("%d,%d,%d"), GetRValue(ValueColor), GetGValue(ValueColor), GetBValue(ValueColor));

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WritePoint(LPCTSTR pszItem, LPCTSTR pszSubItem, POINT ValuePoint) {
            TCHAR szValue[50];
            StringCchPrintf(szValue, 50, TEXT("%d,%d"), ValuePoint.x, ValuePoint.y);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteSize(LPCTSTR pszItem, LPCTSTR pszSubItem, SIZE ValueSize) {
            TCHAR szValue[50];
            StringCchPrintf(szValue, 50, TEXT("%d,%d"), ValueSize.cx, ValueSize.cy);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteRect(LPCTSTR pszItem, LPCTSTR pszSubItem, RECT ValueRect) {
            TCHAR szValue[50];
            StringCchPrintf(szValue, 50, TEXT("%d,%d,%d,%d"), ValueRect.left, ValueRect.top, ValueRect.right, ValueRect.bottom);

            return WriteString(pszItem, pszSubItem, szValue);
        }

        bool Ini::WriteString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR ValueString) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0 || ValueString == NULL || !pszItem || !pszSubItem)
                return false;

            return WritePrivateProfileString(pszItem, pszSubItem, ValueString, m_szIniFile) == TRUE;
        }

		bool Ini::WriteString(LPCTSTR pszItem, LPCTSTR pszSubItem, const String& ValueString) {
			return WriteString(pszItem, pszSubItem, ValueString.GetDataPointer());
		}

		bool Ini::ReadRect(LPCTSTR pszItem, LPCTSTR pszSubItem, RECT & ValueRect) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            TCHAR szReadData[64] = TEXT("");
            ZeroMemory(&ValueRect, sizeof(ValueRect));

            GetPrivateProfileString(pszItem, pszSubItem, TEXT(""), szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCTSTR pszString = szReadData;
                ValueRect.left = SwitchStringToValue(pszString);
                ValueRect.top = SwitchStringToValue(pszString);
                ValueRect.right = SwitchStringToValue(pszString);
                ValueRect.bottom = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadSize(LPCTSTR pszItem, LPCTSTR pszSubItem, SIZE & ValueSize) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            TCHAR szReadData[64] = TEXT("");
            ZeroMemory(&ValueSize, sizeof(ValueSize));

            GetPrivateProfileString(pszItem, pszSubItem, TEXT(""), szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCTSTR pszString = szReadData;
                ValueSize.cx = SwitchStringToValue(pszString);
                ValueSize.cy = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadPoint(LPCTSTR pszItem, LPCTSTR pszSubItem, POINT & ValuePoint) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            TCHAR szReadData[64] = TEXT("");
            ZeroMemory(&ValuePoint, sizeof(ValuePoint));

            GetPrivateProfileString(pszItem, pszSubItem, TEXT(""), szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCTSTR pszString = szReadData;
                ValuePoint.x = SwitchStringToValue(pszString);
                ValuePoint.y = SwitchStringToValue(pszString);

                return true;
            }

            return false;
        }

        bool Ini::ReadColor(LPCTSTR pszItem, LPCTSTR pszSubItem, COLORREF & ValueColor) {
            assert(m_szIniFile[0] != 0);
            if (_tcslen(m_szIniFile) == 0)
                return false;
            TCHAR szReadData[64] = TEXT("");
            ZeroMemory(&ValueColor, sizeof(ValueColor));
            GetPrivateProfileString(pszItem, pszSubItem, TEXT(""), szReadData, 64, m_szIniFile);

            if (szReadData[0] != 0) {
                LPCTSTR pszString = szReadData;
                ValueColor = RGB(SwitchStringToValue(pszString), SwitchStringToValue(pszString), SwitchStringToValue(pszString));

                return true;
            }

            return false;
        }

        LONG Ini::SwitchStringToValue(LPCTSTR & pszSring) {
            assert((pszSring != NULL) && (pszSring[0] != 0));
            if ((pszSring == NULL) || (pszSring[0] == 0)) return 0L;

            while (((pszSring[0] > 0) && (pszSring[0] < TEXT('0'))) || (pszSring[0] > TEXT('9'))) 
                pszSring++;

            LONG lValue = 0L;
            while ((pszSring[0] >= TEXT('0')) && (pszSring[0] <= TEXT('9'))) {
                lValue = lValue * 10L + pszSring[0] - TEXT('0');
                ++pszSring;
            }

            return lValue;
        }
    }
}
#endif
