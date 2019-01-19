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

#ifndef PPX_BASE_CMDLIBE_PARSE_H__
#define PPX_BASE_CMDLIBE_PARSE_H__


#include "ppx_export.h"
#include "base/string.h"
#include <map>

namespace ppx
{
    namespace base {

        class PPX_API CmdLineParser {
        public:
			typedef std::map<String, String> ValsMap;
			typedef ValsMap::const_iterator ITERPOS;

            explicit CmdLineParser(const String &cmdline);
            ~CmdLineParser();

            ITERPOS Begin() const;
            ITERPOS End() const;

            bool HasKey(const String &key) const;

            bool HasVal(const String &key) const;

            String GetVal(const String &key) const;

        private:
            bool Parse(const String &cmdline);

            String         cmdline_;
			class Impl;
			Impl*		   impl_;
        };
    }
}


#endif