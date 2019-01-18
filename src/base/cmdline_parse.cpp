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

#include "base/cmdline_parse.h"
#include <locale>
#include <algorithm>
#include <tchar.h>

namespace ppx {
    namespace base {
        const wchar_t delims[] = L"-/";
        const wchar_t quotes[] = L"\"";
        const wchar_t value_sep[] = L" :"; // don't forget space!!


        CmdLineParser::CmdLineParser(const std::wstring &cmdline) {
            if (cmdline.length() > 0) {
                Parse(cmdline);
            }
        }

        CmdLineParser::~CmdLineParser() {
            value_map_.clear();
        }

        bool CmdLineParser::Parse(const std::wstring &cmdline) {
            const std::wstring sEmpty = L"";
            int nArgs = 0;

            value_map_.clear();
            cmdline_ = cmdline;

            const wchar_t *sCurrent = cmdline_.c_str();

            for (;;) {
                if (sCurrent[0] == L'\0')
                    break;

                // 查找任一分隔符
                const wchar_t *sArg = wcspbrk(sCurrent, delims);

                if (!sArg)
                    break;

                sArg = _wcsinc(sArg); // 字符指针sArg向后移动一个字符

                if (sArg[0] == L'\0')
                    break; // ends with delim

                const wchar_t *sVal = wcspbrk(sArg, value_sep);

                if (sVal == NULL) {
                    std::wstring Key(sArg);
                    std::transform(Key.begin(), Key.end(), Key.begin(), ::tolower);
                    value_map_.insert(ValsMap::value_type(Key, sEmpty));
                    break;
                } else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
                    // cmdline ends with /Key: or a key with no value
                    std::wstring Key(sArg, (int)(sVal - sArg));

                    if (!Key.empty()) {
                        std::transform(Key.begin(), Key.end(), Key.begin(), ::tolower);
                        value_map_.insert(ValsMap::value_type(Key, sEmpty));
                    }

                    sCurrent = _wcsinc(sVal);
                    continue;
                } else {
                    // key has value
                    std::wstring Key(sArg, (int)(sVal - sArg));
                    std::transform(Key.begin(), Key.end(), Key.begin(), ::tolower);

                    sVal = _wcsinc(sVal);

                    const wchar_t *sQuote = wcspbrk(sVal, quotes);
                    const wchar_t *sEndQuote = NULL;

                    if (sQuote == sVal) {
                        // string with quotes (defined in quotes, e.g. '")
                        sQuote = _wcsinc(sVal);
                        sEndQuote = wcspbrk(sQuote, quotes);
                    } else {
                        sQuote = sVal;
                        sEndQuote = wcschr(sQuote, L' ');
                    }

                    if (sEndQuote == NULL) {
                        // no end quotes or terminating space, take the rest of the string to its end
                        std::wstring csVal(sQuote);

                        if (!Key.empty()) {
                            value_map_.insert(ValsMap::value_type(Key, csVal));
                        }

                        break;
                    } else {
                        // end quote
                        if (!Key.empty()) {
                            std::wstring csVal(sQuote, (int)(sEndQuote - sQuote));
                            value_map_.insert(ValsMap::value_type(Key, csVal));
                        }

                        sCurrent = _wcsinc(sEndQuote);
                        continue;
                    }
                }
            }

            return (nArgs > 0);     //TRUE if arguments were found
        }

        CmdLineParser::ValsMap::const_iterator CmdLineParser::findKey(const std::wstring &key) const {
            std::wstring s = key;
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            return value_map_.find(s);
        }

        bool CmdLineParser::HasKey(const std::wstring &key) const {
            ValsMap::const_iterator it = findKey(key);

            if (it == value_map_.end())
                return false;

            return true;
        }


        bool CmdLineParser::HasVal(const std::wstring &key) const {
            ValsMap::const_iterator it = findKey(key);

            if (it == value_map_.end())
                return false;

            if (it->second.empty())
                return false;

            return true;
        }

        std::wstring CmdLineParser::GetVal(const std::wstring &key) const {
            ValsMap::const_iterator it = findKey(key);

            if (it == value_map_.end())
                return 0;

            return it->second;
        }


        CmdLineParser::ITERPOS CmdLineParser::Begin() const {
            return value_map_.begin();
        }

        CmdLineParser::ITERPOS CmdLineParser::End() const {
            return value_map_.end();
        }
    }
}
