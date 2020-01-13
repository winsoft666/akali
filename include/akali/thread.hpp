#ifndef AKALI_THREAD_H__
#define AKALI_THREAD_H__
#pragma once

#include "akali/akali_export.h"

#if defined AKALI_WIN || defined AKALI_LINUX
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif (defined AKALI_LINUX)
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include "akali/constructormagic.h"

namespace akali {
class Thread {
 public:
  Thread() : thread_id_(0), exit_(false) { running_.store(false); }
  Thread(const std::string& name) : thread_id_(0), exit_(false), thread_name_(name) {
    running_.store(false);
  }
  virtual ~Thread() { Stop(true); }

  void SetThreadName(const std::string& name) { thread_name_ = name; }
  std::string GetThreadName() const { return thread_name_; }

  long GetThreadId() { return thread_id_; }

  bool Start() {
    if (thread_.valid()) {
      if (thread_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
        return false;
      }
    }
    thread_ = std::async(std::launch::async, &Thread::Run, this);
    return true;
  }

  virtual void Stop(bool wait_until_stopped) {
    {
      std::lock_guard<std::mutex> lg(mutex_);
      exit_ = true;
    }
    exit_cond_var_.notify_one();

    if (wait_until_stopped) {
      if (thread_.valid())
        thread_.wait();
    }
  }
  bool IsRunning() const { return running_.load(); }

  virtual void Run() {
    running_.store(true);

    SetCurrentThreadName(thread_name_.c_str());
    thread_id_ = Thread::GetCurThreadId();
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lg(mutex_);
        exit_cond_var_.wait(lg, [this]() { return (exit_ || !work_queue_.empty()); });
        if (exit_) {
          running_.store(false);
          return;
        }
        task = std::move(work_queue_.front());
        work_queue_.pop();
      }

      task();
    }
  }

  template <class F, class... Args>
  auto Invoke(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
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

  static void SetCurrentThreadName(const char* name) {
#ifdef AKALI_WIN
    struct {
      DWORD dwType;
      LPCSTR szName;
      DWORD dwThreadID;
      DWORD dwFlags;
    } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};

    __try {
      ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(DWORD),
                       reinterpret_cast<ULONG_PTR*>(&threadname_info));
    } __except (EXCEPTION_EXECUTE_HANDLER) {  // NOLINT
    }
#elif defined AKALI_LINUX
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#endif
  }

  static long GetCurThreadId() {
#ifdef AKALI_WIN
    return GetCurrentThreadId();
#elif defined AKALI_LINUX
    return static_cast<long>(syscall(__NR_gettid));
#endif
  }

 protected:
  std::string thread_name_;
  std::future<void> thread_;
  long thread_id_;
  std::mutex mutex_;
  std::condition_variable exit_cond_var_;
  bool exit_;
  std::queue<std::function<void()>> work_queue_;
  std::atomic_bool running_;
  AKALI_DISALLOW_COPY_AND_ASSIGN(Thread);
};
}  // namespace akali
#endif
#endif  // !AKALI_THREAD_H__
