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

#ifndef AKALI_TIMER_H_
#define AKALI_TIMER_H_
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <functional>

namespace akali {
class AKALI_API TimerBase {
 public:
  TimerBase();
  virtual ~TimerBase();
  static void CALLBACK TimerProc(void* param, BOOLEAN timerCalled);

  // About dwFlags, see:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485(v=vs.85).aspx
  //
  BOOL Start(DWORD ulInterval,  // ulInterval in ms
             BOOL bImmediately,
             BOOL bOnce,
             ULONG dwFlags = WT_EXECUTELONGFUNCTION);
  void Stop(bool bWait);
  virtual void OnTimedEvent();

 private:
  HANDLE m_hTimer;
  PTP_TIMER m_pTimer;
};

template <class T>
class TTimer : public TimerBase {
 public:
  typedef private void (T::*POnTimer)(void);

  TTimer() {
    m_pClass = NULL;
    m_pfnOnTimer = NULL;
  }

  void SetTimedEvent(T* pClass, POnTimer pFunc) {
    m_pClass = pClass;
    m_pfnOnTimer = pFunc;
  }

 protected:
  void OnTimedEvent() override {
    if (m_pfnOnTimer && m_pClass) {
      (m_pClass->*m_pfnOnTimer)();
    }
  }

 private:
  T* m_pClass;
  POnTimer m_pfnOnTimer;
};

class AKALI_API Timer : public TimerBase {
 public:
  typedef std::function<void()> FN_CB;
  Timer() {}

  Timer(FN_CB cb) { SetTimedEvent(cb); }

  void SetTimedEvent(FN_CB cb) { m_cb = cb; }

 protected:
  void OnTimedEvent() override {
    if (m_cb) {
      m_cb();
    }
  }

 private:
  FN_CB m_cb;
};
}  // namespace akali
#endif
#endif  // !AKALI_WIN_TIMER_H_
