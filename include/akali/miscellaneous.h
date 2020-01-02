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

#ifndef AKALI_MISCELLANEOUS_SAF234_
#define AKALI_MISCELLANEOUS_SAF234_
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if (defined _WIN32 || defined WIN32)

#pragma warning(disable : 4995)
#include <string>
#include "akali_export.h"
#include <windows.h>

struct IPropertyStore;
struct _tagpropertykey;
typedef _tagpropertykey PROPERTYKEY;

#ifdef _X86_
#define DebugBreak() _asm { int 3}
#endif

namespace akali {
AKALI_API double RoundEx(double v, int digits);

AKALI_API bool FloatEqual(const float &f1, const float &f2);
AKALI_API bool FloatNotEqual(const float &f1, const float &f2);

// version format XXXX.XXXX.XXXX.XXXX
AKALI_API int VersionCompare(const std::string &str_ver1, const std::string &str_ver2);

#if (defined _WIN32 || defined WIN32)
// 32 bit bmp
AKALI_API bool FullScreenToFile(const std::wstring &file_path);

// 32 bit bmp
AKALI_API bool WindowToFile(HWND h, const std::wstring &file_path);

AKALI_API void PrintConsoleProcess(double percentage);
#endif

AKALI_API void TraceMsgA(const char *lpFormat, ...);
AKALI_API void TraceMsgW(const wchar_t *lpFormat, ...);
} // namespace akali
#endif
#endif // AKALI_MISCELLANEOUS_SAF234_
