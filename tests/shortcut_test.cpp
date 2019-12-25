#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <iostream>
#include "gtest/gtest.h"
#include "ppxbase/process_util.h"
#include "ppxbase/shortcut.h"
#include "ppxbase/scoped_com_initializer.h"
#include <strsafe.h>
#include <ShlObj.h>

TEST(ShortcutTest, WebLink) {
  ppx::base::ScopedCOMInitializer com;
  wchar_t szLinkPath[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szLinkPath, CSIDL_COMMON_DESKTOPDIRECTORY, 0);
  StringCchPrintfW(szLinkPath, MAX_PATH, L"%s\\Google:G.lnk", szLinkPath);

  ppx::base::ShortcutProperties sp;
  sp.SetTarget(L"D:\\sourcecode\\XubeiSteamBox\\src\\Win32\\Debug\\Gogo.exe");
  sp.SetIcon(ppx::base::GetCurrentProcessDirectoryW() + L"Google.ico", 0);
  EXPECT_TRUE(
      ppx::base::CreateOrUpdateShortcutLink(szLinkPath, sp, ppx::base::SHORTCUT_CREATE_ALWAYS));
}

#endif