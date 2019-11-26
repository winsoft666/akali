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

#include "ppxbase/timer.h"

#if (defined _WIN32 || defined WIN32)
namespace ppx {
    namespace base {

        TimerBase::TimerBase() {
            m_hTimer = NULL;
            m_pTimer = NULL;
        }

        TimerBase::~TimerBase() {

        }

        void CALLBACK TimerBase::TimerProc(void *param, BOOLEAN timerCalled) {
            UNREFERENCED_PARAMETER(timerCalled);
            TimerBase *timer = static_cast<TimerBase *>(param);

            timer->OnTimedEvent();
        }

        BOOL TimerBase::Start(DWORD ulInterval,  // ulInterval in ms
                              BOOL bImmediately,
                              BOOL bOnce,
                              ULONG dwFlags /* = WT_EXECUTELONGFUNCTION */) {
            BOOL bRet = FALSE;

            if (!m_hTimer) {
                bRet = CreateTimerQueueTimer(&m_hTimer,
                                             NULL,
                                             TimerProc,
                                             (PVOID)this,
                                             bImmediately ? 0 : ulInterval,
                                             bOnce ? 0 : ulInterval,
                                             dwFlags);
            }

            return bRet;
        }

        void TimerBase::Stop(bool bWait) {
            if (m_hTimer) {
                DeleteTimerQueueTimer(NULL, m_hTimer, bWait ? INVALID_HANDLE_VALUE : NULL);
                m_hTimer = NULL;
            }
        }

        void TimerBase::OnTimedEvent() {
        }
    }
}
#endif