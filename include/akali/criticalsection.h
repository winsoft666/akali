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

#ifndef AKALI_CRITICAL_SECTION_H__
#define AKALI_CRITICAL_SECTION_H__
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "constructormagic.h"

namespace akali {
class AKALI_API CriticalSection {
public:
  CriticalSection();
  ~CriticalSection();
  void Enter() const;
  void Leave() const;
  bool TryEnter() const;

private:
  CriticalSection(const CriticalSection &refCritSec);
  CriticalSection &operator=(const CriticalSection &refCritSec);
  mutable CRITICAL_SECTION crit_;
};

class AKALI_API CritScope {
public:
  explicit CritScope(const CriticalSection *pCS);
  ~CritScope();

private:
  const CriticalSection *const crit_;
  AKALI_DISALLOW_COPY_AND_ASSIGN(CritScope);
};
} // namespace akali
#endif
#endif // !AKALI_CRITICAL_SECTION_H__
