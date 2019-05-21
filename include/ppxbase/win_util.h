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

#ifndef PPX_BASE_WIN_UTIL_H_SAF234_
#define PPX_BASE_WIN_UTIL_H_SAF234_
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef _WIN32

#ifndef _INC_WINDOWS
#include <Windows.h>
#endif
#include <tchar.h>
#include <strsafe.h>
#pragma warning ( disable : 4995 )
#include <string>
#include <vector>
#include <ctime>
#include <math.h>
#include <limits>
#include <Shlwapi.h>
#include <TlHelp32.h>


#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

namespace ppx {
    namespace base {
        double RoundEx(double v, int digits);

        bool FloatEqual(const float &f1, const float &f2);
        bool FloatNotEqual(const float &f1, const float &f2);
    }
}

#endif

#endif // PPX_BASE_WIN_UTIL_H_SAF234_
