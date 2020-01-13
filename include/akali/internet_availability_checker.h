#ifndef AKALI_INTERNET_AVALIABILITY_CHECKER_H__
#define AKALI_INTERNET_AVALIABILITY_CHECKER_H__
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
namespace akali {
class AKALI_API InternetAvailabilityChecker {
 public:
  static bool InternetConnectionAvailable(bool& avaliable);
};
}  // namespace akali
#endif

#endif  // !AKALI_INTERNET_AVALIABILITY_CHECKER_H__