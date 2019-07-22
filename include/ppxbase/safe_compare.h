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


// This file defines six constexpr functions:
//
//   ppx::SafeEq  // ==
//   ppx::SafeNe  // !=
//   ppx::SafeLt  // <
//   ppx::SafeLe  // <=
//   ppx::SafeGt  // >
//   ppx::SafeGe  // >=
//
// They each accept two arguments of arbitrary types, and in almost all cases,
// they simply call the appropriate comparison operator. However, if both
// arguments are integers, they don't compare them using C++'s quirky rules,
// but instead adhere to the true mathematical definitions. It is as if the
// arguments were first converted to infinite-range signed integers, and then
// compared, although of course nothing expensive like that actually takes
// place. In practice, for signed/signed and unsigned/unsigned comparisons and
// some mixed-signed comparisons with a compile-time constant, the overhead is
// zero; in the remaining cases, it is just a few machine instructions (no
// branches).

#ifndef PPX_BASE_SAFE_COMPARE_H_
#define PPX_BASE_SAFE_COMPARE_H_
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include <utility>
#include "ppxbase/template_util.h"

namespace ppx {
    namespace base {
        namespace safe_cmp_impl {

            template <size_t N>
            struct LargerIntImpl : std::false_type {};
            template <>
            struct LargerIntImpl<sizeof(int8_t)> : std::true_type {
                using type = int16_t;
            };
            template <>
            struct LargerIntImpl<sizeof(int16_t)> : std::true_type {
                using type = int32_t;
            };
            template <>
            struct LargerIntImpl<sizeof(int32_t)> : std::true_type {
                using type = int64_t;
            };

            // LargerInt<T1, T2>::value is true iff there's a signed type that's larger
            // than T1 (and no larger than the larger of T2 and int*, for performance
            // reasons); and if there is such a type, LargerInt<T1, T2>::type is an alias
            // for it.
            template <typename T1, typename T2>
            struct LargerInt
                : LargerIntImpl < sizeof(T1) < sizeof(T2) || sizeof(T1) < sizeof(int *)
                ? sizeof(T1)
                : 0 > {};

            template <typename T>
            constexpr typename std::make_unsigned<T>::type MakeUnsigned(T a) {
                return static_cast<typename std::make_unsigned<T>::type>(a);
            }

            // Overload for when both T1 and T2 have the same signedness.
            template <typename Op,
                typename T1,
                typename T2,
                typename std::enable_if<std::is_signed<T1>::value ==
                std::is_signed<T2>::value>::type * = nullptr>
                constexpr bool Cmp(T1 a, T2 b) {
                return Op::Op(a, b);
            }

            // Overload for signed - unsigned comparison that can be promoted to a bigger
            // signed type.
            template < typename Op,
                typename T1,
                typename T2,
                typename std::enable_if < std::is_signed<T1>::value &&
                std::is_unsigned<T2>::value &&
                LargerInt<T2, T1>::value >::type * = nullptr >
                constexpr bool Cmp(T1 a, T2 b) {
                return Op::Op(a, static_cast<typename LargerInt<T2, T1>::type>(b));
            }

            // Overload for unsigned - signed comparison that can be promoted to a bigger
            // signed type.
            template < typename Op,
                typename T1,
                typename T2,
                typename std::enable_if < std::is_unsigned<T1>::value &&
                std::is_signed<T2>::value &&
                LargerInt<T1, T2>::value >::type * = nullptr >
                constexpr bool Cmp(T1 a, T2 b) {
                return Op::Op(static_cast<typename LargerInt<T1, T2>::type>(a), b);
            }

            // Overload for signed - unsigned comparison that can't be promoted to a bigger
            // signed type.
            template < typename Op,
                typename T1,
                typename T2,
                typename std::enable_if < std::is_signed<T1>::value &&
                std::is_unsigned<T2>::value &&
                !LargerInt<T2, T1>::value >::type * = nullptr >
                constexpr bool Cmp(T1 a, T2 b) {
                return a < 0 ? Op::Op(-1, 0) : Op::Op(safe_cmp_impl::MakeUnsigned(a), b);
            }

            // Overload for unsigned - signed comparison that can't be promoted to a bigger
            // signed type.
            template < typename Op,
                typename T1,
                typename T2,
                typename std::enable_if < std::is_unsigned<T1>::value &&
                std::is_signed<T2>::value &&
                !LargerInt<T1, T2>::value >::type * = nullptr >
                constexpr bool Cmp(T1 a, T2 b) {
                return b < 0 ? Op::Op(0, -1) : Op::Op(a, safe_cmp_impl::MakeUnsigned(b));
            }

#define PPX_SAFECMP_MAKE_OP(name, op)      \
    struct name {                            \
        template <typename T1, typename T2>    \
        static constexpr bool Op(T1 a, T2 b) { \
            return a op b;                       \
        }                                      \
    };
            PPX_SAFECMP_MAKE_OP(EqOp, == )
                PPX_SAFECMP_MAKE_OP(NeOp, != )
                PPX_SAFECMP_MAKE_OP(LtOp, < )
                PPX_SAFECMP_MAKE_OP(LeOp, <= )
                PPX_SAFECMP_MAKE_OP(GtOp, > )
                PPX_SAFECMP_MAKE_OP(GeOp, >= )
#undef PPX_SAFECMP_MAKE_OP

        }  // namespace safe_cmp_impl




#define PPX_SAFECMP_MAKE_FUN(name)                                            \
    template <typename T1, typename T2>                                         \
    constexpr                                                                   \
    typename std::enable_if<ppx::base::IsIntlike<T1>::value && ppx::base::IsIntlike<T2>::value,   \
    bool>::type Safe##name(T1 a, T2 b) {            \
        /* Unary plus here turns enums into real integral types. */               \
        return ppx::base::safe_cmp_impl::Cmp<ppx::base::safe_cmp_impl::name##Op>(+a, +b);               \
    }                                                                           \
    template <typename T1, typename T2>                                         \
    constexpr                                                                   \
    typename std::enable_if<!ppx::base::IsIntlike<T1>::value || !ppx::base::IsIntlike<T2>::value, \
    bool>::type Safe##name(const T1& a,             \
                           const T2& b) {           \
        return ppx::base::safe_cmp_impl::name##Op::Op(a, b);                                 \
    }
    PPX_SAFECMP_MAKE_FUN(Eq)
    PPX_SAFECMP_MAKE_FUN(Ne)
    PPX_SAFECMP_MAKE_FUN(Lt)
    PPX_SAFECMP_MAKE_FUN(Le)
    PPX_SAFECMP_MAKE_FUN(Gt)
    PPX_SAFECMP_MAKE_FUN(Ge)
#undef PPX_SAFECMP_MAKE_FUN
    }
}

#endif
