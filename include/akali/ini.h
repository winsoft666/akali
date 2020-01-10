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

#ifndef AKALI_INI_H_
#define AKALI_INI_H_
#pragma once

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include <tchar.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <vector>

namespace akali {
/// Ini file need saved as ANSI encoding.
/// Ini will trim spaces around string value.
///
class AKALI_API Ini {
public:
  Ini() noexcept;
  Ini(const std::wstring &file_path) noexcept;
  ~Ini() noexcept;

public:
  void SetIniFilePath(const std::wstring &file_path) noexcept;
  std::wstring GetIniFilePath() const noexcept;

public:
  bool ReadRect(const std::wstring &item, const std::wstring &sub_item, RECT *result) noexcept;
  bool ReadSize(const std::wstring &item, const std::wstring &sub_item, SIZE *result) noexcept;
  bool ReadPoint(const std::wstring &item, const std::wstring &sub_item, POINT *result) noexcept;
  bool ReadColor(const std::wstring &item, const std::wstring &sub_item, COLORREF *result) noexcept;

  bool ReadInt(const std::wstring &item, const std::wstring &sub_item, UINT *result) noexcept;
  UINT ReadInt(const std::wstring &item, const std::wstring &sub_item, UINT default_value) noexcept;

  std::wstring ReadString(const std::wstring &item, const std::wstring &sub_item,
                          const std::wstring &default_value) noexcept;
  bool ReadString(const std::wstring &item, const std::wstring &sub_item,
                  std::wstring *result) noexcept;

  bool WriteString(const std::wstring &item, const std::wstring &sub_item,
                   const std::wstring &value) noexcept;
  bool WriteInt(const std::wstring &item, const std::wstring &sub_item, LONG value) noexcept;
  bool WriteColor(const std::wstring &item, const std::wstring &sub_item,
                     COLORREF value) noexcept;
  bool WritePoint(const std::wstring &item, const std::wstring &sub_item, POINT value) noexcept;
  bool WriteSize(const std::wstring &item, const std::wstring &sub_item, SIZE value) noexcept;
  bool WriteRect(const std::wstring &item, const std::wstring &sub_item, RECT value) noexcept;

protected:
  LONG SwitchStringToValue(LPCWSTR &pszSring) noexcept;

protected:
  std::wstring ini_file_path_;
};
} // namespace akali
#endif
#endif // !AKALI_INI_H_
