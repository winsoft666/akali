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

#include "ppxbase/random.h"
#include <math.h>
#include "ppxbase/timeutils.h"

#pragma warning(disable:4244)

namespace ppx {
    namespace base {

        Random::Random() {
            state_ = GetTimeStamp();
            PPX_ASSERT(state_ != 0x0ull);
        }

        uint32_t Random::Rand(uint32_t t) {
            // Casting the output to 32 bits will give an almost uniform number.
            // Pr[x=0] = (2^32-1) / (2^64-1)
            // Pr[x=k] = 2^32 / (2^64-1) for k!=0
            // Uniform would be Pr[x=k] = 2^32 / 2^64 for all 32-bit integers k.
            uint32_t x = NextOutput();
            // If x / 2^32 is uniform on [0,1), then x / 2^32 * (t+1) is uniform on
            // the interval [0,t+1), so the integer part is uniform on [0,t].
            uint64_t result = x * (static_cast<uint64_t>(t) + 1);
            result >>= 32;
            return result;
        }

        uint32_t Random::Rand(uint32_t low, uint32_t high) {
            PPX_ASSERT(low <= high);
            return Rand(high - low) + low;
        }

        int32_t Random::Rand(int32_t low, int32_t high) {
            PPX_ASSERT(low <= high);
            const int64_t low_i64{ low };
            return static_cast<int32_t>(
                       Rand(static_cast<uint32_t>(high - low_i64)) + low_i64);
        }

        template <>
        float Random::Rand<float>() {
            double result = NextOutput() - 1;
            result = result / 0xFFFFFFFFFFFFFFFEull;
            return static_cast<float>(result);
        }

        template <>
        double Random::Rand<double>() {
            double result = NextOutput() - 1;
            result = result / 0xFFFFFFFFFFFFFFFEull;
            return result;
        }

        template <>
        bool Random::Rand<bool>() {
            return Rand(0, 1) == 1;
        }

        double Random::Gaussian(double mean, double standard_deviation) {
            // Creating a Normal distribution variable from two independent uniform
            // variables based on the Box-Muller transform, which is defined on the
            // interval (0, 1]. Note that we rely on NextOutput to generate integers
            // in the range [1, 2^64-1]. Normally this behavior is a bit frustrating,
            // but here it is exactly what we need.
            const double kPi = 3.14159265358979323846;
            double u1 = static_cast<double>(NextOutput()) / 0xFFFFFFFFFFFFFFFFull;
            double u2 = static_cast<double>(NextOutput()) / 0xFFFFFFFFFFFFFFFFull;
            return mean + standard_deviation * sqrt(-2 * log(u1)) * cos(2 * kPi * u2);
        }

        double Random::Exponential(double lambda) {
            double uniform = Rand<double>();
            return -log(uniform) / lambda;
        }

        uint64_t Random::NextOutput() {
            state_ ^= state_ >> 12;
            state_ ^= state_ << 25;
            state_ ^= state_ >> 27;
            PPX_ASSERT(state_ != 0x0ULL);
            return state_ * 2685821657736338717ull;
        }

    }
}
