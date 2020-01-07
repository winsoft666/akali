#include "gtest/gtest.h"
#include "akali/process.hpp"

TEST(ProcessTest, Create1) { 
  akali::Process p(L"C:\\Windows\\System32\\notepad.exe D:\\test.txt", L"C:\\");
  EXPECT_TRUE(p.GetId() != 0);
}