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

#ifndef PPX_BASE_OS_VER_H__
#define PPX_BASE_OS_VER_H__
#pragma once

#if (defined _WIN32 || defined WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <string>
#include "ppxbase_export.h"

#pragma warning(disable : 4996)

namespace ppx {
namespace base {
#if (defined _WIN32 || defined WIN32)
enum OSVer {
  WIN_2000,
  WIN_XP,
  WIN_XP_PRO,
  WIN_2003,
  WIN_2003_R2,
  WIN_HOME,
  WIN_VISTA,
  WIN_2008,
  WIN_2008_R2,
  WIN_7,
  WIN_2012,
  WIN_8,
  WIN_2012_R2,
  WIN_8_1,
  WIN_2016,
  WIN_10,     // Threshold 1: Version 1507, Build 10240.
  WIN_10_TH2, // Threshold 2: Version 1511, Build 10586.
  WIN_10_RS1, // Redstone 1: Version 1607, Build 14393.
  WIN_10_RS2, // Redstone 2: Version 1703, Build 15063.
  WIN_10_RS3, // Redstone 3: Version 1709, Build 16299.
  WIN_LAST    // Indicates error condition.
};

class PPXBASE_API OSVersion {
public:
  OSVersion();

  // 2003R2, 2003, XP ....
  //
  bool AtMostWin2003R2(void);

  // Vista, 7, 8 ....
  //
  bool AtLeastWinVista(void);
  void GetMajorMinorBuild(DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuild);
  bool IsWinXPProX64();
  OSVer GetOsVersion(void);
  std::string GetOsVerSummary(void);
  static bool IsWin64();

private:
  void ____Init();
  OSVERSIONINFOEXW osvi;
};
#endif
} // namespace base
} // namespace ppx
#endif
#endif // !PPX_BASE_OS_VER_H__
