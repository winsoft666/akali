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

#ifndef AKALI_SCOPED_COM_INITIALIZER_H_
#define AKALI_SCOPED_COM_INITIALIZER_H_
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include <objbase.h>
#include <assert.h>
#include "akali/constructormagic.h"

namespace akali {
// Initializes COM in the constructor (STA or MTA), and uninitializes COM in the
// destructor.
class AKALI_API ScopedCOMInitializer {
 public:
  // Enum value provided to initialize the thread as an MTA instead of STA.
  enum SelectMTA { kMTA };

  // Constructor for STA initialization.
  ScopedCOMInitializer() { Initialize(COINIT_APARTMENTTHREADED); }

  // Constructor for MTA initialization.
  explicit ScopedCOMInitializer(SelectMTA mta) { Initialize(COINIT_MULTITHREADED); }

  ~ScopedCOMInitializer() {
#ifndef NDEBUG
    // Using the windows API directly to avoid dependency on platform_thread.
    assert(GetCurrentThreadId() == thread_id_);
#endif
    if (succeeded())
      CoUninitialize();
  }

  bool succeeded() const { return SUCCEEDED(hr_); }

 private:
  void Initialize(COINIT init) {
#ifndef NDEBUG
    thread_id_ = GetCurrentThreadId();
#endif
    hr_ = CoInitializeEx(NULL, init);
#ifndef NDEBUG
    if (hr_ != S_FALSE)
      assert(("Invalid COM thread model change", RPC_E_CHANGED_MODE != hr_));
#endif
  }

  HRESULT hr_;
#ifndef NDEBUG
  // In debug builds we use this variable to catch a potential bug where a
  // ScopedCOMInitializer instance is deleted on a different thread than it
  // was initially created on.  If that ever happens it can have bad
  // consequences and the cause can be tricky to track down.
  DWORD thread_id_;
#endif
  AKALI_DISALLOW_COPY_AND_ASSIGN(ScopedCOMInitializer);
};

}  // namespace akali
#endif
#endif