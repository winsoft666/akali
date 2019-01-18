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

#ifndef PPX_BASE_CMDLIBE_PARSE_H__
#define PPX_BASE_CMDLIBE_PARSE_H__


#include <string>
#include <map>
#include "ppx_export.h"

namespace ppx
{
    namespace base {
        class PPX_API CmdLineParser {
        public:
            typedef std::map<std::wstring, std::wstring> ValsMap;
            typedef ValsMap::const_iterator ITERPOS;

            CmdLineParser(const std::wstring &cmdline);
            ~CmdLineParser();

            ITERPOS Begin() const;
            ITERPOS End() const;

            bool HasKey(const std::wstring &key) const;

            bool HasVal(const std::wstring &key) const;

            std::wstring GetVal(const std::wstring &key) const;

        private:
            bool Parse(const std::wstring &cmdline);
            ValsMap::const_iterator findKey(const std::wstring &key) const;

            std::wstring   cmdline_;
            ValsMap        value_map_;
        };
    }
}


#endif