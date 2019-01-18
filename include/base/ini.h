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


#ifndef PPX_BASE_INI_H_
#define PPX_BASE_INI_H_
#pragma once

#ifdef _WIN32
#include <tchar.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "base/stringencode.h"
#include "base/string.h"
#include "ppx_export.h"

namespace ppx {
    namespace base {
        class PPX_API Ini {
          public:
            Ini();
            virtual ~Ini();

          public:
            void SetIniFilePath(LPCTSTR pszIniFile);
            LPCTSTR GetIniFilePath();
          public:
            bool ReadRect(LPCTSTR pszItem, LPCTSTR pszSubItem, RECT &ValueRect);
            bool ReadSize(LPCTSTR pszItem, LPCTSTR pszSubItem, SIZE &ValueSize);
            bool ReadPoint(LPCTSTR pszItem, LPCTSTR pszSubItem, POINT &ValuePoint);
            bool ReadColor(LPCTSTR pszItem, LPCTSTR pszSubItem, COLORREF &ValueColor);
            UINT ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, INT nDefault);
            bool ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, UINT &ValueInt);
            LPCTSTR ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount);
			bool ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, String &strString);
            bool WriteString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR ValueString);
			bool WriteString(LPCTSTR pszItem, LPCTSTR pszSubItem, const String& ValueString);
            bool WriteInt(LPCTSTR pszItem, LPCTSTR pszSubItem, LONG ValueInt);
            bool WriteRGBColor(LPCTSTR pszItem, LPCTSTR pszSubItem, COLORREF ValueColor);
            bool WritePoint(LPCTSTR pszItem, LPCTSTR pszSubItem, POINT ValuePoint);
            bool WriteSize(LPCTSTR pszItem, LPCTSTR pszSubItem, SIZE ValueSize);
            bool WriteRect(LPCTSTR pszItem, LPCTSTR pszSubItem, RECT ValueRect);
          protected:
            LONG SwitchStringToValue(LPCTSTR &pszSring);
          protected:
            TCHAR m_szIniFile[MAX_PATH];
        };
    }
}
#endif

#endif // !PPX_BASE_INI_H_