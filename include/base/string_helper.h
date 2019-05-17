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

#ifndef PPX_BASE_STRING_HELPER_H__
#define PPX_BASE_STRING_HELPER_H__
#pragma once

#include <string>
#include <vector>

namespace ppx {
	namespace base {
		std::string StringToLower(const std::string &str);
		std::wstring StringToLower(const std::wstring &str);

		std::string StringToUpper(const std::string &str);
		std::wstring StringToUpper(const std::wstring &str);

		std::vector<std::wstring> StringSplit(const std::wstring &src, const std::wstring& delimiter);
		std::vector<std::string> StringSplit(const std::string &src, const std::string& delimiter);

		int StringFind(const std::string &src, char ch, int iOffset = 0);
		int StringFind(const std::string &src, const std::string & search, int iOffset = 0);
		int StringReverseFind(const std::string &src, char ch, int iOffset = 0);
		int StringReverseFind(const std::string &src, const std::string &search, int iOffset = 0);

		int StringFind(const std::wstring &src, wchar_t ch, int iOffset = 0);
		int StringFind(const std::wstring &src, const std::wstring & search, int iOffset = 0);
		int StringReverseFind(const std::wstring &src, wchar_t ch, int iOffset = 0);
		int StringReverseFind(const std::wstring &src, const std::wstring &search, int iOffset = 0);

		std::string MakeString(const char* pstrFormat, ...);
		std::wstring MakeString(const wchar_t* pstrFormat, ...);
	}
}

#endif // !PPX_BASE_STRING_HELPER_H__