#ifndef PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__
#define PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__
#pragma once

#include "ppxbase_export.h"

namespace ppx {
	namespace base {
		class PPXBASE_API InternetAvailabilityChecker {
		public:
			static bool InternetConnectionAvailable(bool &avaliable);
		};
	}
}

#endif // !PPXBAE_INTERNET_AVALIABILITY_CHECKER_H__