#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <iostream>
#include "gtest/gtest.h"
#include "akali/schedule_task.h"
#include "akali/scoped_com_initializer.h"
#include "akali/process_util.h"
#include <strsafe.h>
#include <ShlObj.h>

TEST(ScheduleTaskTest, test1) {
  akali::ScopedCOMInitializer com;

  akali::ScheduleTask st;
  EXPECT_TRUE(st.NewLoginTriggerTask(TEXT("Test"), TEXT("C:\\Test App.exe"),
                                     TEXT("test parameters"), TEXT("this is a test task"),
                                     TEXT("akali")));

  EXPECT_TRUE(st.IsExist(TEXT("Test")));
  EXPECT_TRUE(st.IsEnable(TEXT("Test")));
  EXPECT_TRUE(st.IsTaskValid(TEXT("Test")));

  TCHAR szBuf[MAX_PATH] = {0};

  {
    ZeroMemory(szBuf, MAX_PATH);
    EXPECT_TRUE(st.GetProgramPath(TEXT("Test"), 1, szBuf));
    EXPECT_TRUE(_tcscmp(szBuf, TEXT("C:\\Test App.exe")) == 0);
  }

  {
    ZeroMemory(szBuf, MAX_PATH);
    EXPECT_TRUE(st.GetParameters(TEXT("Test"), 1, szBuf));
    EXPECT_TRUE(_tcscmp(szBuf, TEXT("test parameters")) == 0);
  }

  EXPECT_TRUE(st.SetEnable(TEXT("Test"), false));
  EXPECT_FALSE(st.IsEnable(TEXT("Test")));

  EXPECT_TRUE(st.Delete(TEXT("Test")));
  EXPECT_FALSE(st.IsExist(TEXT("Test")));
}

#endif