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

#ifndef AKALI_ASSERT_H__
#define AKALI_ASSERT_H__
#pragma once

#include "akali_export.h"

#if !defined(AKALI_ASSERT_ENABLED)
#if !defined(NDEBUG)         // if we are in debug mode
#define AKALI_ASSERT_ENABLED 1 // enable them
#endif
#endif

#define AKALI_ASSERT(...) AKALI_ASSERT_(__VA_ARGS__)
#define AKALI_ASSERT_USED(...) AKALI_ASSERT_USED_(__VA_ARGS__)

#define AKALI_NOT_REACHED(...) AKALI_ASSERT_(false, __VA_ARGS__)

#define AKALI_ASSERT_JOIN(lhs, rhs) AKALI_ASSERT_JOIN_(lhs, rhs)
#define AKALI_ASSERT_JOIN_(lhs, rhs) AKALI_ASSERT_JOIN__(lhs, rhs)
#define AKALI_ASSERT_JOIN__(lhs, rhs) lhs##rhs

#define AKALI_ASSERT_FILE __FILE__
#define AKALI_ASSERT_LINE __LINE__
#define AKALI_ASSERT_FUNCTION __FUNCTION__

#ifdef _MSC_VER
extern void __cdecl __debugbreak(void);
#define AKALI_ASSERT_DEBUG_BREAK() __debugbreak()
#endif

#define AKALI_ASSERT_NO_MACRO

#define AKALI_ASSERT_APPLY_VA_ARGS(M, ...) AKALI_ASSERT_APPLY_VA_ARGS_(M, (__VA_ARGS__))
#define AKALI_ASSERT_APPLY_VA_ARGS_(M, args) M args

#define AKALI_ASSERT_NARG(...)                                                                       \
  AKALI_ASSERT_APPLY_VA_ARGS(AKALI_ASSERT_NARG_, AKALI_ASSERT_NO_MACRO, ##__VA_ARGS__, 32, 31, 30, 29,   \
                           28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
                           10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, AKALI_ASSERT_NO_MACRO)
#define AKALI_ASSERT_NARG_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15,     \
                         _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,     \
                         _30, _31, _32, _33, ...)                                                  \
  _33

#define AKALI_ASSERT_HAS_ONE_ARG(...)                                                                \
  AKALI_ASSERT_APPLY_VA_ARGS(AKALI_ASSERT_NARG_, AKALI_ASSERT_NO_MACRO, ##__VA_ARGS__, 0, 0, 0, 0, 0, 0, \
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                           0, 1, 0, AKALI_ASSERT_NO_MACRO)

#define AKALI_ASSERT_LIKELY(arg) (arg)
#define AKALI_ASSERT_UNLIKELY(arg) (arg)

#define AKALI_ASSERT_UNUSED(expression) (void)(true ? (void)0 : ((void)(expression)))

#define AKALI_ASSERT_(...)                                                                           \
  AKALI_ASSERT_JOIN(AKALI_ASSERT_, AKALI_ASSERT_HAS_ONE_ARG(__VA_ARGS__))(__VA_ARGS__)
#define AKALI_ASSERT_0(...) AKALI_ASSERT_APPLY_VA_ARGS(AKALI_ASSERT_2, __VA_ARGS__)
#define AKALI_ASSERT_1(expression) AKALI_ASSERT_2(expression, nullptr)

#define AKALI_ASSERT_3(expression, ...)                                                              \
  do {                                                                                             \
    if (!AKALI_ASSERT_LIKELY(expression))                                                            \
      Internal::HandleAssert(AKALI_ASSERT_FILE, AKALI_ASSERT_LINE, AKALI_ASSERT_FUNCTION,         \
                                    #expression, __VA_ARGS__);                                     \
  } while (false)

#define AKALI_ASSERT_USED_(...) AKALI_ASSERT_USED_0(AKALI_ASSERT_NARG(__VA_ARGS__), __VA_ARGS__)
#define AKALI_ASSERT_USED_0(N, ...) AKALI_ASSERT_JOIN(AKALI_ASSERT_USED_, N)(__VA_ARGS__)

#define AKALI_STATIC_ASSERT(...)                                                                     \
  AKALI_ASSERT_APPLY_VA_ARGS(                                                                        \
      AKALI_ASSERT_JOIN(AKALI_STATIC_ASSERT_, AKALI_ASSERT_HAS_ONE_ARG(__VA_ARGS__)), __VA_ARGS__)
#define AKALI_STATIC_ASSERT_0(expression, message) static_assert(expression, message)
#define AKALI_STATIC_ASSERT_1(expression) AKALI_STATIC_ASSERT_0(expression, #expression)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4548)
#pragma warning(disable : 4710)
#endif
#include <stdexcept>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <utility>

namespace akali {
namespace Internal {

AKALI_API void HandleAssert(const char *file, int line, const char *function,
                              const char *expression, const char *message, ...);

template <typename T> class AssertUsedWrapper {
public:
  AssertUsedWrapper(T &&t);
  ~AssertUsedWrapper() throw();
  operator T();

private:
  const AssertUsedWrapper &
  operator=(const AssertUsedWrapper
                &); // not implemented on purpose (and only VS2013 supports deleted functions)

  T t;
  mutable bool used;

}; // AssertUsedWrapper<int, T>

template <typename T>
inline AssertUsedWrapper<T>::AssertUsedWrapper(T &&_t) : t(std::forward<T>(_t)), used(false) {}

template <typename T> inline AssertUsedWrapper<T>::operator T() {
  used = true;
  return std::move(t);
}

template <typename T> inline AssertUsedWrapper<T>::~AssertUsedWrapper() throw() {
  AKALI_ASSERT_3(used, "unused value");
}
} // namespace Internal
} // namespace akali

#undef AKALI_ASSERT_2
#undef AKALI_ASSERT_USED_1

#if AKALI_ASSERT_ENABLED
#define AKALI_ASSERT_2(expression, ...) AKALI_ASSERT_3(expression, __VA_ARGS__)
#define AKALI_ASSERT_USED_1(type) Internal::AssertUsedWrapper<type>

#else

#define AKALI_ASSERT_2(expression, ...) AKALI_ASSERT_UNUSED(expression)
#define AKALI_ASSERT_USED_1(type) type

#endif

#endif // !AKALI_ASSERT_H__