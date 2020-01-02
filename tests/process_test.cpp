#include "gtest/gtest.h"
#include "akali/process.hpp"

TEST(ProcessTest, Create1) { 
  akali::Process p(L"C:\\Windows\\System32\\notepad.exe D:\\test.txt", L"C:\\");
  EXPECT_TRUE(p.GetId() != 0);
}

TEST(ProcessTest, Create2) {
  akali::Process p(L"\"D:\\≤‚ ‘ test\\Debug\\Gogo.exe\" -open3rd \"d:\\≤‚ ‘ test\\≤‚ ‘.html\"", L"C:\\");
  EXPECT_TRUE(p.GetId() != 0);
}

TEST(ProcessTest, Kill1) {
  bool ret = akali::Process::Kill(15228);
  EXPECT_TRUE(ret);
}

TEST(ProcessTest, Kill2) {
  bool ret = akali::Process::Kill(L"Dbgview ≤‚ ‘.exe");
  EXPECT_TRUE(ret);
}