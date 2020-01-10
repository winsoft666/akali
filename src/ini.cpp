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

#include "akali/ini.h"
#ifdef AKALI_WIN
#include <strsafe.h>
#include <assert.h>

namespace akali {
Ini::Ini() noexcept {}

Ini::Ini(const std::wstring &file_path) noexcept
    : ini_file_path_(file_path) {}

Ini::~Ini() noexcept {}

void Ini::SetIniFilePath(const std::wstring &file_path) noexcept { ini_file_path_ = file_path; }

std::wstring Ini::GetIniFilePath() const noexcept { return ini_file_path_; }

bool Ini::ReadInt(const std::wstring &item, const std::wstring &sub_item, UINT *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;
  INT iDefault = 0;
  SetLastError(0);
  UINT ret =
      GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), iDefault, ini_file_path_.c_str());
  DWORD dwGLE = GetLastError();
  if (dwGLE == 0) {
    if (result)
      *result = ret;
    return true;
  }
  return false;
}

UINT Ini::ReadInt(const std::wstring &item, const std::wstring &sub_item,
                  UINT default_value) noexcept {
  if (ini_file_path_.length() == 0)
    return default_value;

  SetLastError(0);
  return GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), default_value,
                               ini_file_path_.c_str());
}

std::wstring Ini::ReadString(const std::wstring &item, const std::wstring &sub_item,
                             const std::wstring &default_value) noexcept {
  if (ini_file_path_.length() == 0)
    return default_value;

  std::vector<wchar_t> buf(1024, 0);
  DWORD read = GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), default_value.c_str(),
                                        buf.data(), 1024, ini_file_path_.c_str());

  std::wstring result;
  result.assign(buf.data(), read);

  return result;
}

bool Ini::ReadString(const std::wstring &item, const std::wstring &sub_item,
                     std::wstring *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;

  bool ret = false;
  int iBufSize = 255;
  WCHAR *pBuf = NULL;
  do {
    pBuf = (WCHAR *)malloc(iBufSize * sizeof(WCHAR));
    memset(pBuf, 0, iBufSize * sizeof(WCHAR));
    SetLastError(0);
    DWORD dwRet = GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", pBuf, iBufSize,
                                           ini_file_path_.c_str());
    DWORD dwGLE = GetLastError();
    if (dwRet == 0) {
      ret = (dwGLE == 0);
      break;
    }
    else if (dwRet == iBufSize - 1) {
      free(pBuf);
      pBuf = NULL;
      iBufSize += 1024;
    }
    else {
      ret = true;
      break;
    }
  } while (true);

  if (ret) {
    if (result)
      *result = pBuf;
  }
  free(pBuf);

  return ret;
}

bool Ini::WriteInt(const std::wstring &item, const std::wstring &sub_item, LONG value) noexcept {
  if (ini_file_path_.length() == 0)
    return false;

  WCHAR szValue[50];
  StringCchPrintfW(szValue, 50, L"%ld", value);
  return WriteString(item, sub_item, szValue);
}

bool Ini::WriteColor(const std::wstring &item, const std::wstring &sub_item,
                     COLORREF value) noexcept {
  WCHAR szValue[60];
  StringCchPrintfW(szValue, 60, L"%d,%d,%d,%d", (BYTE)(value >> 24), (BYTE)(value >> 16),
                   (BYTE)(value >> 8), (BYTE)value);

  return WriteString(item, sub_item, szValue);
}

bool Ini::WritePoint(const std::wstring &item, const std::wstring &sub_item, POINT value) noexcept {
  WCHAR szValue[50];
  StringCchPrintfW(szValue, 50, L"%d,%d", value.x, value.y);

  return WriteString(item, sub_item, szValue);
}

bool Ini::WriteSize(const std::wstring &item, const std::wstring &sub_item, SIZE value) noexcept {
  WCHAR szValue[50];
  StringCchPrintfW(szValue, 50, L"%d,%d", value.cx, value.cy);

  return WriteString(item, sub_item, szValue);
}

bool Ini::WriteRect(const std::wstring &item, const std::wstring &sub_item, RECT value) noexcept {
  WCHAR szValue[50];
  StringCchPrintfW(szValue, 50, L"%d,%d,%d,%d", value.left, value.top, value.right, value.bottom);

  return WriteString(item, sub_item, szValue);
}

bool Ini::WriteString(const std::wstring &item, const std::wstring &sub_item,
                      const std::wstring &value) noexcept {
  if (ini_file_path_.length() == 0)
    return false;

  return !!WritePrivateProfileString(item.c_str(), sub_item.c_str(), value.c_str(),
                                     ini_file_path_.c_str());
}

bool Ini::ReadRect(const std::wstring &item, const std::wstring &sub_item, RECT *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;

  WCHAR szReadData[64] = L"";
  ZeroMemory(result, sizeof(result));

  GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                           ini_file_path_.c_str());

  if (szReadData[0] != 0) {
    LPCWSTR pszString = szReadData;
    if (result) {
      result->left = SwitchStringToValue(pszString);
      result->top = SwitchStringToValue(pszString);
      result->right = SwitchStringToValue(pszString);
      result->bottom = SwitchStringToValue(pszString);
    }
    return true;
  }

  return false;
}

bool Ini::ReadSize(const std::wstring &item, const std::wstring &sub_item, SIZE *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;
  WCHAR szReadData[64] = L"";
  ZeroMemory(result, sizeof(result));

  GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                           ini_file_path_.c_str());

  if (szReadData[0] != 0) {
    LPCWSTR pszString = szReadData;
    if (result) {
      result->cx = SwitchStringToValue(pszString);
      result->cy = SwitchStringToValue(pszString);
    }
    return true;
  }

  return false;
}

bool Ini::ReadPoint(const std::wstring &item, const std::wstring &sub_item,
                    POINT *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;
  WCHAR szReadData[64] = L"";
  ZeroMemory(result, sizeof(result));

  GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                           ini_file_path_.c_str());

  if (szReadData[0] != 0) {
    LPCWSTR pszString = szReadData;
    if (result) {
      result->x = SwitchStringToValue(pszString);
      result->y = SwitchStringToValue(pszString);
    }

    return true;
  }

  return false;
}

bool Ini::ReadColor(const std::wstring &item, const std::wstring &sub_item,
                    COLORREF *result) noexcept {
  if (ini_file_path_.length() == 0)
    return false;
  WCHAR szReadData[64] = L"";
  ZeroMemory(result, sizeof(result));
  GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                           ini_file_path_.c_str());

  if (szReadData[0] != 0) {
    LPCWSTR pszString = szReadData;
    if (result)
      *result = SwitchStringToValue(pszString) << 24 | SwitchStringToValue(pszString) << 16 |
                SwitchStringToValue(pszString) << 8 | SwitchStringToValue(pszString);

    return true;
  }

  return false;
}

LONG Ini::SwitchStringToValue(LPCWSTR &pszSring) noexcept {
  assert((pszSring != NULL) && (pszSring[0] != 0));
  if ((pszSring == NULL) || (pszSring[0] == 0))
    return 0L;

  while (((pszSring[0] > 0) && (pszSring[0] < L'0')) || (pszSring[0] > L'9'))
    pszSring++;

  LONG lValue = 0L;
  while ((pszSring[0] >= L'0') && (pszSring[0] <= L'9')) {
    lValue = lValue * 10L + pszSring[0] - L'0';
    ++pszSring;
  }

  return lValue;
}
} // namespace akali
#endif
