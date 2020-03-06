#include "gtest/gtest.h"
#include "akali/process.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
TEST(WinProcessTest, Create1) { 
  akali::Process p(L"C:\\Windows\\System32\\notepad.exe C:\\test.txt", L"C:\\");
  EXPECT_TRUE(p.GetId() != 0);
  EXPECT_TRUE(p.Successed());
  EXPECT_TRUE(p.Kill(p.GetId()));

}
#else
TEST(UnixProcessTest, Create1) {
  akali::Process p(L"top");
  EXPECT_TRUE(p.GetId() != 0);
  EXPECT_TRUE(p.Successed());
  EXPECT_TRUE(p.Kill(p.GetId()));
}
#endif