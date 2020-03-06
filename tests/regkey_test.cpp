#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "akali/registry.h"
#include "gtest/gtest.h"
#include <iostream>
#include <strsafe.h>
#include <windows.h>

using namespace akali;

TEST(RegKeyTest, ReadStr) {
  RegKey regkey(HKEY_CURRENT_USER,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders");
  HRESULT hr = regkey.Open(KEY_ALL_ACCESS | KEY_WOW64_64KEY, false);
  EXPECT_TRUE(hr == S_OK);
  if (S_OK == hr) {
    std::wstring strFolder;
    EXPECT_TRUE(regkey.GetExpandSZValue(L"Favorites", true, strFolder) == S_OK);
  }
}

#endif