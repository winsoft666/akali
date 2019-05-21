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

#ifndef PPX_BASE_CRITICAL_SECTION_H__
#define PPX_BASE_CRITICAL_SECTION_H__
#pragma once
#include "constructormagic.h"
#include "ppx_export.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace ppx {
    namespace base {
        class PPX_API CriticalSection {
          public:
            CriticalSection();
            ~CriticalSection();
            void Enter() const;
            void Leave() const;
            bool TryEnter() const;
          private:
            CriticalSection(const CriticalSection &refCritSec);
            CriticalSection &operator=(const CriticalSection &refCritSec);
            mutable CRITICAL_SECTION crit_;
        };

        class PPX_API CritScope {
          public:
            explicit CritScope(const CriticalSection *pCS);
            ~CritScope();
          private:
            const CriticalSection *const crit_;
            PPX_DISALLOW_COPY_AND_ASSIGN(CritScope);
        };
    }
}
#endif

#endif // !PPX_BASE_CRITICAL_SECTION_H__
