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

#ifndef PPX_BASE_EVENT_H_
#define PPX_BASE_EVENT_H_

#include "ppxbase/constructormagic.h"
#include "ppxbase_export.h"
#if defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(POSIX)
#include <pthread.h>
#else
#error "Must define either WIN or POSIX."
#endif

namespace ppx {
    namespace base {
        class PPXBASE_API Event {
        public:
            static const int kForever = -1;

            Event(bool manual_reset, bool initially_signaled);
            ~Event();

            void Set();
            void Reset();

            // Wait for the event to become signaled, for the specified number of
            // |milliseconds|.  To wait indefinetly, pass kForever.
            bool Wait(int milliseconds);

        private:
#if defined(WIN32)
            HANDLE event_handle_;
#elif defined(POSIX)
            pthread_mutex_t event_mutex_;
            pthread_cond_t event_cond_;
            const bool is_manual_reset_;
            bool event_status_;
#endif

            PPX_DISALLOW_IMPLICIT_CONSTRUCTORS(Event);
        };

    }
}
#endif
