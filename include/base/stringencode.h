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

#ifndef PPX_BASE_STRINGENCODE_H_
#define PPX_BASE_STRINGENCODE_H_

#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include "base/string.h"
#include "ppx_export.h"

namespace ppx {

    namespace base {

        // Apply any suitable string transform (including the ones above) to an STL string.
        // Stack-allocated temporary space is used for the transformation, so value and source may refer to the same string.
        typedef size_t(*Transform)(char * buffer, size_t buflen, const char * source, size_t srclen);

        // Return the result of applying transform t to source.
        PPX_API std::string s_transform(const std::string& source, Transform t);


        PPX_API size_t UrlDecode(char * buffer, size_t buflen, const char * source, size_t srclen);
        PPX_API std::string UrlDecode(const std::string& source);
        PPX_API std::string UrlEncode(const std::string &str);

        // Convert an unsigned value from 0 to 15 to the hex character equivalent...
        PPX_API char HexEncode(unsigned char val);
        // ...and vice-versa.
        PPX_API bool HexDecode(char ch, unsigned char* val);

        // hex_encode, but separate each byte representation with a delimiter.
        // delimiter == 0 means no delimiter. If the buffer is too short, we return 0
        PPX_API size_t HexEncodeWithDelimiter(char* buffer, size_t buflen, const char* source, size_t srclen, char delimiter);

        PPX_API std::string HexEncode(const std::string& str);
        PPX_API std::string HexEncode(const char* source, size_t srclen);
        PPX_API std::string HexEncodeWithDelimiter(const char* source, size_t srclen, char delimiter);



        // hex_decode, assuming that there is a delimiter between every byte pair.
        // delimiter == 0 means no delimiter. If the buffer is too short or the data is invalid, we return 0.
        PPX_API size_t HexDecodeWithDelimiter(char* buffer, size_t buflen, const char* source, size_t srclen, char delimiter);

        PPX_API size_t HexDecode(char* buffer, size_t buflen, const std::string& source);
        PPX_API size_t HexDecodeWithDelimiter(char* buffer, size_t buflen, const std::string& source, char delimiter);


#ifdef _WIN32
        // About code_page, see https://docs.microsoft.com/zh-cn/windows/desktop/Intl/code-page-identifiers
        //
        PPX_API StringANSI UnicodeToAnsi(const StringUnicode &str, unsigned int code_page = 0);
        PPX_API StringUnicode AnsiToUnicode(const StringANSI &str, unsigned int code_page = 0);
        PPX_API StringUTF8 UnicodeToUtf8(const StringUnicode &str);
        PPX_API StringUnicode Utf8ToUnicode(const StringUTF8 &str);
        PPX_API StringUTF8 AnsiToUtf8(const StringANSI &str, unsigned int code_page = 0);
        PPX_API StringANSI Utf8ToAnsi(const StringUTF8 &str, unsigned int code_page = 0);

        PPX_API StringUTF8 UnicodeToUtf8BOM(const StringUnicode &str);
        PPX_API StringUTF8 AnsiToUtf8BOM(const StringANSI &str, unsigned int code_page = 0);


#ifdef UNICODE
#define TCHARToAnsi(str) ppx::base::UnicodeToAnsi((str), 0)
#define TCHARToUtf8(str) ppx::base::UnicodeToUtf8((str))
#define AnsiToTCHAR(str) ppx::base::AnsiToUnicode((str), 0)
#define Utf8ToTCHAR(str) ppx::base::Utf8ToUnicode((str))
#define TCHARToUnicode(str) ((std::wstring)(str))
#define UnicodeToTCHAR(str) ((std::wstring)(str))
#else
#define TCHARToAnsi(str) ((std::string)(str))
#define TCHARToUtf8 ppx::base::AnsiToUtf8((str), 0)
#define AnsiToTCHAR(str) ((std::string)(str))
#define Utf8ToTCHAR(str) ppx::base::Utf8ToAnsi((str), 0)
#define TCHARToUnicode(str) ppx::base::AnsiToUnicode((str), 0)
#define UnicodeToTCHAR(str) ppx::base::UnicodeToAnsi((str), 0)
#endif

#endif
    } // namespace base

}  // namespace ppx

#endif  // PPX_BASE_STRINGENCODE_H_
