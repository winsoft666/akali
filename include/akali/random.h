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

#ifndef AKALI_RANDOM_H_
#define AKALI_RANDOM_H_
#pragma once
#include <limits>
#include "akali_export.h"
#include "akali/assert.h"
#include "akali/constructormagic.h"

namespace akali {
class AKALI_API Random {
public:
  Random();

  // Return pseudo-random integer of the specified type.
  // We need to limit the size to 32 bits to keep the output close to uniform.
  template <typename T> T Rand() {
    static_assert(std::numeric_limits<T>::is_integer && std::numeric_limits<T>::radix == 2 &&
                      std::numeric_limits<T>::digits <= 32,
                  "Rand is only supported for built-in integer types that are "
                  "32 bits or smaller.");
    return static_cast<T>(NextOutput());
  }

  // Uniformly distributed pseudo-random number in the interval [0, t].
  uint32_t Rand(uint32_t t);

  // Uniformly distributed pseudo-random number in the interval [low, high].
  uint32_t Rand(uint32_t low, uint32_t high);

  // Uniformly distributed pseudo-random number in the interval [low, high].
  int32_t Rand(int32_t low, int32_t high);

  // Normal Distribution.
  double Gaussian(double mean, double standard_deviation);

  // Exponential Distribution.
  double Exponential(double lambda);

private:
  // Outputs a nonzero 64-bit random number.
  uint64_t NextOutput();

  uint64_t state_;

  AKALI_DISALLOW_COPY_AND_ASSIGN(Random);
};
} // namespace akali
#endif
