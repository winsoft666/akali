#include "gtest/gtest.h"
#include "akali/string_helper.hpp"

TEST(StringTest, Basic) {
  std::string s1 = "abcABC123!@#$%^&*~\r\n";
  std::wstring s2 = L"abcABC123!@#$%^&*~\r\n";
  std::string s3 = "-aa--bb-cc--";
  std::wstring s4 = L"-aa--bb-cc--";
  std::string s5 = "go-c-channel_1-c-channel-ca-v-2.1.1.1.exe";
  std::wstring s6 = L"go-c-channel_1-c-channel-ca-v-2.1.1.1.exe";

  EXPECT_TRUE(akali::StringCaseConvert(s1, akali::EasyCharToLowerA) == "abcabc123!@#$%^&*~\r\n");
  EXPECT_TRUE(akali::StringCaseConvert(s1, akali::EasyCharToUpperA) == "ABCABC123!@#$%^&*~\r\n");
  EXPECT_TRUE(akali::StringCaseConvert(s2, akali::EasyCharToLowerW) == L"abcabc123!@#$%^&*~\r\n");
  EXPECT_TRUE(akali::StringCaseConvert(s2, akali::EasyCharToUpperW) == L"ABCABC123!@#$%^&*~\r\n");

  {
    std::vector<std::string> v = akali::StringSplit<std::string>(s3, "-", true);
    EXPECT_TRUE(v.size() == 7 && v[0] == "" && v[1] == "aa" && v[2] == "" && v[3] == "bb" &&
                v[4] == "cc" && v[5] == "" && v[6] == "");
  }
  {
    std::vector<std::wstring> v = akali::StringSplit<std::wstring>(s4, L"-", true);
    EXPECT_TRUE(v.size() == 7 && v[0] == L"" && v[1] == L"aa" && v[2] == L"" && v[3] == L"bb" &&
                v[4] == L"cc" && v[5] == L"" && v[6] == L"");
  }

  {
    std::vector<std::wstring> v = akali::StringSplit<std::wstring>(s4, L"-", false);
    EXPECT_TRUE(v.size() == 3 && v[0] == L"aa" && v[1] == L"bb" &&
                v[2] == L"cc");
  }

  {
    std::vector<std::string> v = akali::StringSplit<std::string>(s5, "-c-", true);
    EXPECT_TRUE(v.size() == 3 && v[0] == "go" && v[1] == "channel_1" &&
                v[2] == "channel-ca-v-2.1.1.1.exe");
  }

  {
    std::vector<std::wstring> v = akali::StringSplit<std::wstring>(s6, L"-c-", true);
    EXPECT_TRUE(v.size() == 3 && v[0] == L"go" && v[1] == L"channel_1" &&
                v[2] == L"channel-ca-v-2.1.1.1.exe");
  }
}