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

#ifndef PPX_BASE_MISCELLANEOUS_SAF234_
#define PPX_BASE_MISCELLANEOUS_SAF234_
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef _WIN32


#pragma warning ( disable : 4995 )
#include <string>
#include "ppxbase_export.h"
#include <windows.h>

struct IPropertyStore;
struct _tagpropertykey;
typedef _tagpropertykey PROPERTYKEY;

#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

namespace ppx {
    namespace base {
		PPXBASE_API double RoundEx(double v, int digits);

		PPXBASE_API bool FloatEqual(const float &f1, const float &f2);
		PPXBASE_API bool FloatNotEqual(const float &f1, const float &f2);

		// version format XXXX.XXXX.XXXX.XXXX
		PPXBASE_API int VersionCompare(const std::string &str_ver1, const std::string &str_ver2);
    }
}

#endif

#endif // PPX_BASE_MISCELLANEOUS_SAF234_
