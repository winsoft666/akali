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

#include <stdint.h>

#if defined(POSIX)
#include <sys/time.h>
#if defined(_MAC)
#include <mach/mach_time.h>
#endif
#endif

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>
#include <sys/timeb.h>
#include <strsafe.h>
#endif
#include "ppxbase/timeutils.h"

namespace ppx {
	namespace base {

        ppx::base::Time GetLocalTime() {
            Time t;
            SYSTEMTIME st;
            GetLocalTime(&st);

            t.year = st.wYear;
            t.month = st.wMonth;
            t.day = st.wDay;
            t.hour = st.wHour;
            t.minute = st.wMinute;
            t.second = st.wSecond;
            t.milliseconds = st.wMilliseconds;

            return t;
        }

        ppx::base::Time GetUTCTime() {
            Time t;
            SYSTEMTIME st;
            GetSystemTime(&st);

            t.year = st.wYear;
            t.month = st.wMonth;
            t.day = st.wDay;
            t.hour = st.wHour;
            t.minute = st.wMinute;
            t.second = st.wSecond;
            t.milliseconds = st.wMilliseconds;

            return t;
        }

        long long GetTimeStamp() {
            union {
                long long ns100;
                FILETIME ft;
            } fileTime;
            GetSystemTimeAsFileTime(&fileTime.ft);

            // lNowMicroMS中存储着从1970-01-01 00:00:00到当前格林威治时间（UTC）所经过的微妙数
            // 116444736000000000是从1601年1月1日00:00:00:000到1970年1月1日00:00:00:000所经过的100纳秒数
            long long lNowMicroMS = (long long)((fileTime.ns100 - 116444736000000000LL) / 10LL);

            return lNowMicroMS;
        }

#if defined(_WIN32)
        ppx::base::Time FILETIMEToUTC(FILETIME ft) {
            SYSTEMTIME st;
            Time t;
            if (FileTimeToSystemTime(&ft, &st)) {
                t.year = st.wYear;
                t.month = st.wMonth;
                t.day = st.wDay;
                t.hour = st.wHour;
                t.minute = st.wMinute;
                t.second = st.wSecond;
                t.milliseconds = st.wMilliseconds;
            }

            return t;
        }
#endif

        long long UTCToTimeStamp(Time t) {

            struct tm tmUTC;
            tmUTC.tm_year = t.year - 1900; // 减去1900
            tmUTC.tm_mon = t.month - 1; // 减去1
            tmUTC.tm_mday = t.day;
            tmUTC.tm_hour = t.hour;
            tmUTC.tm_min = t.minute;
            tmUTC.tm_sec = t.second;

            time_t timastampSec = mktime(&tmUTC);

            return (long long)timastampSec * 1000000;
        }

        Time::Time() {
            year = 0;
            month = 0;
            day = 0;
            minute = 0;
            second = 0;
            milliseconds = 0;
            microseconds = 0;
            nanoseconds = 0;
        }

        std::string Time::ToString(
            bool mill_precision,
            bool micro_precision,
            bool nano_precision) {
            char szString[512];
            if (nano_precision) {
                StringCchPrintfA(szString, 512,
                    "%04d/%02d/%02d %02u:%02u:%02u:%03u:%03u:%03u",
                    year, month, day, hour, minute, second, milliseconds, microseconds, nanoseconds
                );
            }
            else if (micro_precision) {
                StringCchPrintfA(szString, 512,
                    "%04d/%02d/%02d %02u:%02u:%02u:%03u:%03u",
                    year, month, day, hour, minute, second, milliseconds, microseconds 
                );
            }
            else if (mill_precision) {
                StringCchPrintfA(szString, 512,
                    "%04d/%02d/%02d %02u:%02u:%02u:%03u",
                    year, month, day, hour, minute, second, milliseconds
                );
            }
            else {
                StringCchPrintfA(szString, 512,
                    "%04d/%02d/%02d %02u:%02u:%02u",
                    year, month, day, hour, minute, second
                );
            }

            return szString;
        }

    }
} // namespace ppx
