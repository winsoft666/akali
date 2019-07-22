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
#ifndef PPXBASE_SCREENSHOT_H_
#define PPXBASE_SCREENSHOT_H_
#pragma once

#include <string>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include "ppxbase_export.h"

namespace ppx {
	namespace base {
		class PPXBASE_API Screenshot {
		public:
			// 32 bit bmp
			static bool FullScreenToFile(const std::wstring &file_path);

			// 32 bit bmp
			static bool WindowToFile(HWND h, const std::wstring &file_path);
			
		};
	}
}

#endif