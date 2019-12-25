#ifndef PPXBASE_THREAD_H__
#define PPXBASE_THREAD_H__
#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "ppxbase/constructormagic.h"
#include "ppxbase/ppxbase_export.h"

namespace ppx {
namespace base {
class PPXBASE_API Thread {
public:
  Thread();
  Thread(const std::string &name);
  virtual ~Thread();

  void SetThreadName(const std::string &name);
  std::string GetThreadName() const;

  long GetThreadId();

  bool Start();
  virtual void Stop(bool wait_until_stopped);
  bool IsRunning() const;

  virtual void Run();

  template <class F, class... Args>
  auto Invoke(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
      std::unique_lock<std::mutex> lock(mutex_);
      work_queue_.emplace([task]() { (*task)(); });
    }
    exit_cond_var_.notify_one();
    return res;
  }

  static void SetCurrentThreadName(const char *name);
  static long GetCurThreadId();

protected:
  std::string thread_name_;
  std::future<void> thread_;
  long thread_id_;
  std::mutex mutex_;
  std::condition_variable exit_cond_var_;
  bool exit_;
  std::queue<std::function<void()>> work_queue_;
  std::atomic_bool running_;
  PPX_DISALLOW_COPY_AND_ASSIGN(Thread);
};

} // namespace base
} // namespace ppx

#endif // !PPXBASE_THREAD_H__