#ifndef PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__
#define PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__
#pragma once

#include "ppxbase_export.h"

#if (defined _WIN32 || defined WIN32)
namespace ppx {
namespace base {
class PPXBASE_API InternetAvailabilityChecker {
public:
  static bool InternetConnectionAvailable(bool &avaliable);
};
} // namespace base
} // namespace ppx
#endif

#endif // !PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__