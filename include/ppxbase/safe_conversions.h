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

// Borrowed from Chromium's src/base/numerics/safe_conversions.h.

#ifndef PPX_BASE_SAFE_CONVERSIONS_H_
#define PPX_BASE_SAFE_CONVERSIONS_H_

#include <limits>

#include "ppxbase/assert.h"
#include "ppxbase/safe_conversions_impl.h"

namespace ppx {
    namespace base {
        // Convenience function that returns true if the supplied value is in range
        // for the destination type.
        template <typename Dst, typename Src>
        inline bool IsValueInRangeForNumericType(Src value) {
            return internal::RangeCheck<Dst>(value) == internal::TYPE_VALID;
        }

        // CheckedCast<> is analogous to static_cast<> for
        // numeric types, except that they [D]CHECK that the specified numeric
        // conversion will not overflow or underflow. NaN source will always trigger
        // the [D]CHECK.
        template <typename Dst, typename Src>
        inline Dst CheckedCast(Src value) {
            PPX_ASSERT(IsValueInRangeForNumericType<Dst>(value));
            return static_cast<Dst>(value);
        }

        // SaturatedCast<> is analogous to static_cast<> for numeric types, except
        // that the specified numeric conversion will saturate rather than overflow or
        // underflow. NaN assignment to an integral will trigger a assert condition.
        template <typename Dst, typename Src>
        inline Dst SaturatedCast(Src value) {
            // Optimization for floating point values, which already saturate.
            if (std::numeric_limits<Dst>::is_iec559)
                return static_cast<Dst>(value);

            switch (internal::RangeCheck<Dst>(value)) {
            case internal::TYPE_VALID:
                return static_cast<Dst>(value);

            case internal::TYPE_UNDERFLOW:
                return std::numeric_limits<Dst>::min();

            case internal::TYPE_OVERFLOW:
                return std::numeric_limits<Dst>::max();

                // Should fail only on attempting to assign NaN to a saturated integer.
            case internal::TYPE_INVALID:
                PPX_NOT_REACHED("");
                return std::numeric_limits<Dst>::max();
            }

            PPX_NOT_REACHED("");
            return static_cast<Dst>(value);
        }

    } 
}

#endif  // ! PPX_BASE_SAFE_CONVERSIONS_H_
