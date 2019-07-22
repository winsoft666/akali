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

#ifndef PPX_BASE_TIMEUTILS_H_
#define PPX_BASE_TIMEUTILS_H_

#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include "ppxbase_export.h"

namespace ppx {
	namespace base {
        class PPXBASE_API Time {
        public:
            Time();
            std::string ToString(
                bool mill_precision = false, 
                bool micro_precision = false, 
                bool nano_precision = false);
        public:
            int year;
            int month;
            int day;
            int hour;
            int minute;
            int second;
            int milliseconds;
            int microseconds;
            int nanoseconds;
        };


		static const int64_t kNumMillisecsPerSec = INT64_C(1000);
		static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
		static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

		static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
		static const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec;
		static const int64_t kNumNanosecsPerMicrosec = kNumNanosecsPerSec / kNumMicrosecsPerSec;

        
        // Windows: precision is milliseconds
		PPXBASE_API Time GetLocalTime();

        // Windows: precision is milliseconds
		PPXBASE_API Time GetUTCTime();

        // 从1970-01-01 00:00:00到当前格林威治时间（UTC）所经过的微妙数
		PPXBASE_API long long GetTimeStamp();
#if defined(_WIN32)
		PPXBASE_API Time FILETIMEToUTC(FILETIME ft);
#endif
		PPXBASE_API long long UTCToTimeStamp(Time t);

		class PPXBASE_API TimerMeter {
		public:
			TimerMeter() {
				lStartTime_ = std::clock();
			}

			void Restart() {
				lStartTime_ = std::clock();
			}

            // ms
			long Elapsed() const {
				return std::clock() - lStartTime_;
			}

			long ElapsedMax() const {
				return (std::numeric_limits<std::clock_t>::max)()
					- lStartTime_;
			}

			long ElapsedMin() const {
				return 1L;
			}

		private:
			std::clock_t lStartTime_;
		};
	}
}  // namespace ppx

#endif  // PPX_BASE_TIMEUTILS_H_
