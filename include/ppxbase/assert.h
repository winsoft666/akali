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

#ifndef PPX_BASE_ASSERT_H__
#define PPX_BASE_ASSERT_H__
#pragma once

#include "ppxbase_export.h"

#if !defined(PPX_ASSERT_ENABLED)
    #if !defined(NDEBUG) // if we are in debug mode
        #define PPX_ASSERT_ENABLED 1 // enable them
    #endif
#endif

#define PPX_ASSERT(...)                    PPX_ASSERT_(__VA_ARGS__)
#define PPX_ASSERT_USED(...)               PPX_ASSERT_USED_(__VA_ARGS__)

#define PPX_NOT_REACHED(...)                  PPX_ASSERT_(false, __VA_ARGS__)

#define PPX_ASSERT_JOIN(lhs, rhs)   PPX_ASSERT_JOIN_(lhs, rhs)
#define PPX_ASSERT_JOIN_(lhs, rhs)  PPX_ASSERT_JOIN__(lhs, rhs)
#define PPX_ASSERT_JOIN__(lhs, rhs) lhs##rhs

#define PPX_ASSERT_FILE __FILE__
#define PPX_ASSERT_LINE __LINE__
#define PPX_ASSERT_FUNCTION __FUNCTION__

#ifdef _MSC_VER
extern void __cdecl __debugbreak(void);
#define PPX_ASSERT_DEBUG_BREAK() __debugbreak()
#endif

#define PPX_ASSERT_NO_MACRO

#define PPX_ASSERT_APPLY_VA_ARGS(M, ...) PPX_ASSERT_APPLY_VA_ARGS_(M, (__VA_ARGS__))
#define PPX_ASSERT_APPLY_VA_ARGS_(M, args) M args

#define PPX_ASSERT_NARG(...) PPX_ASSERT_APPLY_VA_ARGS(PPX_ASSERT_NARG_, PPX_ASSERT_NO_MACRO,##__VA_ARGS__,\
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,\
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, PPX_ASSERT_NO_MACRO)
#define PPX_ASSERT_NARG_( _0, _1, _2, _3, _4, _5, _6, _7, _8,\
                          _9, _10, _11, _12, _13, _14, _15, _16,\
                          _17, _18, _19, _20, _21, _22, _23, _24,\
                          _25, _26, _27, _28, _29, _30, _31, _32, _33, ...) _33

#define PPX_ASSERT_HAS_ONE_ARG(...) PPX_ASSERT_APPLY_VA_ARGS(PPX_ASSERT_NARG_, PPX_ASSERT_NO_MACRO,##__VA_ARGS__,\
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, PPX_ASSERT_NO_MACRO)

#define PPX_ASSERT_LIKELY(arg) (arg)
#define PPX_ASSERT_UNLIKELY(arg) (arg)

#define PPX_ASSERT_UNUSED(expression) (void)(true ? (void)0 : ((void)(expression)))

#define PPX_ASSERT_(...)          PPX_ASSERT_JOIN(PPX_ASSERT_, PPX_ASSERT_HAS_ONE_ARG(__VA_ARGS__))(__VA_ARGS__)
#define PPX_ASSERT_0(...)         PPX_ASSERT_APPLY_VA_ARGS(PPX_ASSERT_2, __VA_ARGS__)
#define PPX_ASSERT_1(expression)  PPX_ASSERT_2(expression, nullptr)

#define PPX_ASSERT_3(expression, ...) \
    do\
    {\
        if (!PPX_ASSERT_LIKELY(expression)) \
            ::ppx::Internal::HandleAssert(PPX_ASSERT_FILE, PPX_ASSERT_LINE, PPX_ASSERT_FUNCTION, #expression, __VA_ARGS__); \
    }\
    while (false)


#define PPX_ASSERT_USED_(...)            PPX_ASSERT_USED_0(PPX_ASSERT_NARG(__VA_ARGS__), __VA_ARGS__)
#define PPX_ASSERT_USED_0(N, ...)        PPX_ASSERT_JOIN(PPX_ASSERT_USED_, N)(__VA_ARGS__)

#define PPX_STATIC_ASSERT(...)           PPX_ASSERT_APPLY_VA_ARGS(PPX_ASSERT_JOIN(PPX_STATIC_ASSERT_, PPX_ASSERT_HAS_ONE_ARG(__VA_ARGS__)), __VA_ARGS__)
#define PPX_STATIC_ASSERT_0(expression, message) static_assert(expression, message)
#define PPX_STATIC_ASSERT_1(expression)  PPX_STATIC_ASSERT_0(expression, #expression)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4548)
#pragma warning(disable: 4710)
#endif
#include <stdexcept>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <utility>

namespace ppx {
    namespace Internal {

        PPXBASE_API void HandleAssert(const char *file, int line, const char *function, const char *expression, const char *message, ...);

        template< typename T>
        class AssertUsedWrapper {
          public:
            AssertUsedWrapper(T &&t);
            ~AssertUsedWrapper() throw();
            operator T();
          private:
            const AssertUsedWrapper &operator = (const AssertUsedWrapper &); // not implemented on purpose (and only VS2013 supports deleted functions)

            T t;
            mutable bool used;

        }; // AssertUsedWrapper<int, T>

        template<typename T>
        inline AssertUsedWrapper<T>::AssertUsedWrapper(T &&_t)
            : t(std::forward<T>(_t)), used(false)
        {}

        template<typename T>
        inline AssertUsedWrapper<T>::operator T() {
            used = true;
            return std::move(t);
        }

        template<typename T>
        inline AssertUsedWrapper< T>::~AssertUsedWrapper() throw() {
            PPX_ASSERT_3(used, "unused value");
        }
    } // namespace Internal
} // namespace ppx


#undef PPX_ASSERT_2
#undef PPX_ASSERT_USED_1


#if PPX_ASSERT_ENABLED
    #define PPX_ASSERT_2(expression, ...) PPX_ASSERT_3(expression, __VA_ARGS__)
    #define PPX_ASSERT_USED_1(type)       ppx::Internal::AssertUsedWrapper<type>

#else

    #define PPX_ASSERT_2(expression, ...) PPX_ASSERT_UNUSED(expression)
    #define PPX_ASSERT_USED_1(type)              type

#endif


#endif // !PPX_BASE_ASSERT_H__