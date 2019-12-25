#include "ppxbase/thread.h"
#if (defined _WIN32 || defined WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

namespace ppx {
namespace base {

Thread::Thread() : thread_id_(0), exit_(false) { running_.store(false); }

Thread::Thread(const std::string &name) : thread_id_(0), exit_(false), thread_name_(name) {
  running_.store(false);
}

Thread::~Thread() { Stop(true); }

void Thread::SetThreadName(const std::string &name) { thread_name_ = name; }

std::string Thread::GetThreadName() const { return thread_name_; }

long Thread::GetThreadId() { return thread_id_; }

bool Thread::Start() {
  if (thread_.valid()) {
    if (thread_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
      return false;
    }
  }
  thread_ = std::async(std::launch::async, &Thread::Run, this);
  return true;
}

void Thread::Stop(bool wait_until_stopped) {
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

bool Thread::IsRunning() const { return running_.load(); }

void Thread::Run() {
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

void Thread::SetCurrentThreadName(const char *name) {
#if defined(_WIN32)
  struct {
    DWORD dwType;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags;
  } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};

  __try {
    ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(DWORD),
                     reinterpret_cast<ULONG_PTR *>(&threadname_info));
  } __except (EXCEPTION_EXECUTE_HANDLER) { // NOLINT
  }

#else
  prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name)); // NOLINT
#endif
}

long Thread::GetCurThreadId() {
#if (defined WIN32 || defined _WIN32)
  return GetCurrentThreadId();
#else
  return static_cast<long>(syscall(__NR_gettid));
#endif
}

} // namespace base
} // namespace ppx