#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <iostream>
#include "gtest/gtest.h"
#include "akali/process.hpp"
#include "akali/shortcut.h"
#include "akali/filesystem.hpp"
#include "akali/scoped_com_initializer.h"
#include <strsafe.h>
#include <ShlObj.h>

TEST(ShortcutTest, WebLink) {
  akali::ScopedCOMInitializer com;
  wchar_t szLinkPath[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szLinkPath, CSIDL_DESKTOPDIRECTORY, 0);
  StringCchPrintfW(szLinkPath, MAX_PATH, L"%s\\Google.lnk", szLinkPath);

  akali::ShortcutProperties sp;
  sp.SetTarget(L"https://www.google.com");
  std::wstring ico_path = akali::filesystem::current_path().wstring() + L"\\Google.ico";
  if (akali::filesystem::exists(ico_path))
    sp.SetIcon(ico_path, 0);
  EXPECT_TRUE(
      akali::CreateOrUpdateShortcutLink(szLinkPath, sp, akali::SHORTCUT_CREATE_ALWAYS));
  EXPECT_TRUE(akali::filesystem::exists(szLinkPath));
}

#endif