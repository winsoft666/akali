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

#ifndef PPX_BASE_SAFE_STRING_H_
#define PPX_BASE_SAFE_STRING_H_
#pragma once

#include <string>
#include "ppxbase_export.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace ppx {
    namespace base {

        template <typename T>
        class vm_allocator : public std::allocator<T> {
        public:
            typedef size_t size_type;
            typedef T* pointer;
            typedef const T* const_pointer;

            template<typename _Tp1>
            struct rebind {
                typedef vm_allocator<_Tp1> other;
            };

            pointer allocate(size_type n, const void *hint = 0) {
                UNREFERENCED_PARAMETER(hint);
                LPVOID buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, n * sizeof(T));
                return (pointer)buffer;
            }

            void deallocate(pointer p, size_type n) {
                UNREFERENCED_PARAMETER(n);
                if (p) {
                    HeapFree(GetProcessHeap(), 0, p);
                }
            }

            vm_allocator() throw() : std::allocator<T>() {
            }

            vm_allocator(const vm_allocator &a) throw() : std::allocator<T>(a) {
            }

            template <class U>
            vm_allocator(const vm_allocator<U> &a) throw() : std::allocator<T>(a) {
            }

            ~vm_allocator() throw() {
            }
        };

        typedef std::basic_string<char, std::char_traits<char>, vm_allocator<char> > ModuleSafeString;

        #define ModuleSafeVector(T) std::vector<T, ppx::base::vm_allocator<T> >
    }
}
#else
typedef std::string ModuleSafeString;
#define ModuleSafeVector(T) std::vector<T>
#endif
#endif