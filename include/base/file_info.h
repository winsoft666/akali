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


#ifndef __FILE_INFO_SDFSDR54_H__
#define __FILE_INFO_SDFSDR54_H__
#pragma once

#ifdef _WIN32
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include "base/string.h"
#include "base/timeutils.h"
#include "ppx_export.h"

namespace ppx
{
    namespace base {
        class PPX_API FileInfo {
        public:
            FileInfo();
            ~FileInfo();

            bool Create(HMODULE hModule = NULL);
            bool Create(const String &strFileName);

        public:
            WORD GetFileVersion(int nIndex) const;
            WORD GetProductVersion(int nIndex) const;
            DWORD GetFileFlagsMask() const;
            DWORD GetFileFlags() const;
            DWORD GetFileOs() const;
            DWORD GetFileType() const;
            DWORD GetFileSubtype() const;

            String GetCompanyName();
            String GetFileDescription();
            String GetFileVersion();
            String GetFileVersionEx();
            String GetInternalName();
            String GetLegalCopyright();
            String GetOriginalFileName();
            String GetProductName();
            String GetProductVersion();
            String GetComments();
            String GetLegalTrademarks();
            String GetPrivateBuild();
            String GetSpecialBuild();

            // Windows的文件时间为一个64位整数（用FILETIME结构体存储）,它记录从1601-1-1 00:00:00到当前格林威治时间（UTC）所经过的100纳秒(ns)数
            // See: https ://blog.csdn.net/china_jeffery/article/details/78409614 
            //
            FILETIME GetCreationTime() const;
            FILETIME GetLastAccessTime() const;
            FILETIME GetLastWriteTime() const;

        private:
            virtual void Reset();
            bool GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, BOOL bPrimaryEnough = FALSE);

        private:
            VS_FIXEDFILEINFO m_FileInfo;

            String m_strCompanyName;
            String m_strFileDescription;
            String m_strFileVersion;
            String m_strInternalName;
            String m_strLegalCopyright;
            String m_strOriginalFileName;
            String m_strProductName;
            String m_strProductVersion;
            String m_strComments;
            String m_strLegalTrademarks;
            String m_strPrivateBuild;
            String m_strSpecialBuild;

            FILETIME     m_ftCreationTime;
            FILETIME     m_ftLastAccessTime;
            FILETIME     m_ftLastWriteTime;
        };
    }
}
#endif

#endif // ! __FILE_INFO_SDFSDR54_H__