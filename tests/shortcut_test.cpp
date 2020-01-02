#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <iostream>
#include "gtest/gtest.h"
#include "akali/process.hpp"
#include "akali/shortcut.h"
#include "akali/scoped_com_initializer.h"
#include <strsafe.h>
#include <ShlObj.h>

TEST(ShortcutTest, WebLink) {
  akali::ScopedCOMInitializer com;
  wchar_t szLinkPath[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szLinkPath, CSIDL_COMMON_DESKTOPDIRECTORY, 0);
  StringCchPrintfW(szLinkPath, MAX_PATH, L"%s\\Google:G.lnk", szLinkPath);

  akali::ShortcutProperties sp;
  sp.SetTarget(L"D:\\sourcecode\\XubeiSteamBox\\src\\Win32\\Debug\\Gogo.exe");
  sp.SetIcon(akali::Process::GetSelfDir() + L"Google.ico", 0);
  EXPECT_TRUE(
      akali::CreateOrUpdateShortcutLink(szLinkPath, sp, akali::SHORTCUT_CREATE_ALWAYS));
}

#endif