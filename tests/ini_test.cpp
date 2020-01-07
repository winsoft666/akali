#include "gtest/gtest.h"
#include "akali/ini.h"
#include <akali/process.hpp>
#include "akali/process_util.h"

TEST(IniTest, Basic) {
  std::wstring ini_path = akali::Process::GetSelfDir() + L"test ≤‚ ‘.ini";
  akali::Ini ini(ini_path);

  std::wstring str_result;
  EXPECT_TRUE(ini.WriteString(L"app", L"str", L"≤‚ ‘ test"));
  EXPECT_TRUE(ini.ReadString(L"app", L"str", &str_result));
  EXPECT_TRUE(str_result == L"≤‚ ‘ test");
  EXPECT_TRUE(ini.ReadString(L"app", L"str", L"") == L"≤‚ ‘ test");

  EXPECT_TRUE(ini.WriteString(L"app", L"str", L"  hello ƒ„∫√ƒ„∫√ƒ„∫√  "));
  EXPECT_TRUE(ini.ReadString(L"app", L"str", &str_result));
  EXPECT_TRUE(str_result == L"hello ƒ„∫√ƒ„∫√ƒ„∫√");
  EXPECT_TRUE(ini.ReadString(L"app", L"str", L"XXX") == L"hello ƒ„∫√ƒ„∫√ƒ„∫√");

  EXPECT_FALSE(ini.ReadString(L"app", L"str_non", &str_result));
  EXPECT_TRUE(ini.ReadString(L"app", L"str_non", L"XXX") == L"XXX");

  UINT i_result = 0;
  EXPECT_TRUE(ini.WriteInt(L"app", L"int", 1234567));
  EXPECT_TRUE(ini.ReadInt(L"app", L"int", &i_result));
  EXPECT_TRUE(i_result == 1234567);
  EXPECT_TRUE(ini.ReadInt(L"app", L"int", 123) == 1234567);

  EXPECT_FALSE(ini.ReadInt(L"app", L"int_non", &i_result));
  EXPECT_TRUE(ini.ReadInt(L"app", L"int_non", 123) == 123);
}

TEST(IniTest, Advance) {
  std::wstring ini_path = akali::Process::GetSelfDir() + L"test ≤‚ ‘.ini";
  akali::Ini ini(ini_path);

  RECT rc = {1, 2, 3, 4};
  RECT rc_result = {0, 0, 0, 0};

  EXPECT_TRUE(ini.WriteRect(L"app", L"rect", rc));
  EXPECT_TRUE(ini.ReadRect(L"app", L"rect", &rc_result));
  EXPECT_TRUE(rc_result.left == 1 && rc_result.top == 2 && rc_result.right == 3 &&
              rc_result.bottom == 4);

  POINT point = {1, 2};
  POINT point_result = {0, 0};

  EXPECT_TRUE(ini.WritePoint(L"app", L"point", point));
  EXPECT_TRUE(ini.ReadPoint(L"app", L"point", &point_result));
  EXPECT_TRUE(point_result.x == 1 && point_result.y == 2);

  SIZE sz = {1, 2};
  SIZE sz_result = {0, 0};

  EXPECT_TRUE(ini.WriteSize(L"app", L"size", sz));
  EXPECT_TRUE(ini.ReadSize(L"app", L"size", &sz_result));
  EXPECT_TRUE(sz_result.cx == 1 && sz_result.cy == 2);

  COLORREF clr = 0x11223344;
  COLORREF clr_result = 0;

  EXPECT_TRUE(ini.WriteColor(L"app", L"color", clr));
  EXPECT_TRUE(ini.ReadColor(L"app", L"color", &clr_result));
  EXPECT_TRUE(clr_result == 0x11223344);
}