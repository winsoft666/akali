#include <iostream>
#include "gtest/gtest.h"
#include "ppxbase/thread.h"

TEST(ThreadTest, test1) {
  long call_tid = ppx::base::Thread::GetCurThreadId();

  std::string str_thread_name = "test-thread1";
  ppx::base::Thread t(str_thread_name);
  EXPECT_TRUE(str_thread_name == t.GetThreadName());
  EXPECT_TRUE(t.Start());

  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  EXPECT_TRUE(t.IsRunning());

  long sub_tid = t.GetThreadId();

  EXPECT_TRUE(sub_tid != 0);
  EXPECT_TRUE(call_tid != sub_tid);

  bool task_done = false;
  auto res = t.Invoke([sub_tid, &task_done]() {
    long cur_tid = ppx::base::Thread::GetCurThreadId();
    EXPECT_TRUE(sub_tid == cur_tid);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    task_done = true;

    return 5;
  });

  int ret_val = res.get();
  EXPECT_TRUE(task_done);
  EXPECT_EQ(ret_val, 5);

  t.Stop(true);

  EXPECT_FALSE(t.IsRunning());
}