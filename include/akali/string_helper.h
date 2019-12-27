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

#ifndef AKALI_STRING_HELPER_H__
#define AKALI_STRING_HELPER_H__
#pragma once

#include <string>
#include <vector>
#include "akali_export.h"

namespace akali {
AKALI_API std::string StringToLower(const std::string &str);
AKALI_API std::wstring StringToLower(const std::wstring &str);

AKALI_API std::string StringToUpper(const std::string &str);
AKALI_API std::wstring StringToUpper(const std::wstring &str);

AKALI_API std::vector<std::wstring> StringSplit(const std::wstring &src,
                                                  const std::wstring &delimiter);
AKALI_API std::vector<std::string> StringSplit(const std::string &src,
                                                 const std::string &delimiter);

AKALI_API int StringFind(const std::string &src, char ch, int iOffset = 0);
AKALI_API int StringFind(const std::string &src, const std::string &search, int iOffset = 0);
AKALI_API int StringReverseFind(const std::string &src, char ch, int iOffset = 0);
AKALI_API int StringReverseFind(const std::string &src, const std::string &search,
                                  int iOffset = 0);

AKALI_API int StringFind(const std::wstring &src, wchar_t ch, int iOffset = 0);
AKALI_API int StringFind(const std::wstring &src, const std::wstring &search, int iOffset = 0);
AKALI_API int StringReverseFind(const std::wstring &src, wchar_t ch, int iOffset = 0);
AKALI_API int StringReverseFind(const std::wstring &src, const std::wstring &search,
                                  int iOffset = 0);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
AKALI_API std::string MakeString(const char *pstrFormat, ...);
AKALI_API std::wstring MakeString(const wchar_t *pstrFormat, ...);
#endif
} // namespace akali
#endif // !AKALI_STRING_HELPER_H__
