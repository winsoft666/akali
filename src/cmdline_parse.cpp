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

#include "ppxbase/cmdline_parse.h"
#include "ppxbase/safe_release_macro.h"
#include <locale>
#include <algorithm>
#include <tchar.h>
#include <map>
#include "ppxbase/string_helper.h"
#include "ppxbase/stringencode.h"

namespace ppx {
    namespace base {

        class CmdLineParser::Impl {
          public:
            Impl() {
                value_map_.clear();
            }

            ~Impl() {
                value_map_.clear();
            }

            ITERPOS findKey(const std::wstring &key) const {
                std::wstring s = StringToLower(key);
                return value_map_.find(s);
            }

            CmdLineParser::ValsMap        value_map_;
        };

        const wchar_t delims[] = L"-/";
        const wchar_t quotes[] = L"\"";
        const wchar_t value_sep[] = L" :"; // don't forget space!!


        CmdLineParser::CmdLineParser(const std::wstring &cmdline) {
            impl_ = new Impl();

            if (cmdline.length() > 0) {
                Parse(cmdline);
            }
        }

        CmdLineParser::~CmdLineParser() {
            SAFE_DELETE(impl_);
        }

        bool CmdLineParser::Parse(const std::wstring &cmdline) {
            const std::wstring sEmpty = L"";
            int nArgs = 0;

            impl_->value_map_.clear();
            cmdline_ = cmdline;

            std::wstring strW = TCHARToUnicode(cmdline_);
            const wchar_t *sCurrent = strW.c_str();

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
                    Key = StringToLower(Key);
                    impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
                    break;
                } else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
                    // cmdline ends with /Key: or a key with no value
                    std::wstring Key(sArg, (int)(sVal - sArg));

                    if (Key.length() > 0) {
                        Key = StringToLower(Key);
                        impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
                    }

                    sCurrent = _wcsinc(sVal);
                    continue;
                } else {
                    // key has value
                    std::wstring Key(sArg, (int)(sVal - sArg));
                    Key = StringToLower(Key);

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

                        if (Key.length() > 0) {
                            impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                        }

                        break;
                    } else {
                        // end quote
                        if (Key.length() > 0) {
                            std::wstring csVal(sQuote, (int)(sEndQuote - sQuote));
                            impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                        }

                        sCurrent = _wcsinc(sEndQuote);
                        continue;
                    }
                }
            }

            return (nArgs > 0);     //TRUE if arguments were found
        }



        bool CmdLineParser::HasKey(const std::wstring &key) const {
            ITERPOS it = impl_->findKey(key);

            if (it == impl_->value_map_.end())
                return false;

            return true;
        }


        bool CmdLineParser::HasVal(const std::wstring &key) const {
            ITERPOS it = impl_->findKey(key);

            if (it == impl_->value_map_.end())
                return false;

            if (it->second.length() == 0)
                return false;

            return true;
        }

        std::wstring CmdLineParser::GetVal(const std::wstring &key) const {
            ITERPOS it = impl_->findKey(key);

            if (it == impl_->value_map_.end())
                return std::wstring();

            return it->second;
        }


        int CmdLineParser::GetParamCount() const {
            return impl_->value_map_.size();
        }

        CmdLineParser::ITERPOS CmdLineParser::Begin() const {
            return impl_->value_map_.begin();
        }

        CmdLineParser::ITERPOS CmdLineParser::End() const {
            return impl_->value_map_.end();
        }
    }
}
