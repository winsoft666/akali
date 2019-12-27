#ifndef AKALI_INTERNET_AVALIABILITY_CHECKER_H__
#define AKALI_INTERNET_AVALIABILITY_CHECKER_H__
#pragma once

#include "akali_export.h"

#if (defined _WIN32 || defined WIN32)
namespace akali {
class AKALI_API InternetAvailabilityChecker {
public:
  static bool InternetConnectionAvailable(bool &avaliable);
};
} // namespace akali
#endif

#endif // !AKALI_INTERNET_AVALIABILITY_CHECKER_H__