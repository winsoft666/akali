/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_PROCESS_H_
#define AKALI_PROCESS_H_
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include "akali/akali_export.h"
#ifdef AKALI_WIN
#pragma comment(lib, "Psapi.lib")
#else
#include <sys/wait.h>
#endif

namespace akali {
/// Additional parameters to Process constructors.
struct Config {
  /// Buffer size for reading stdout and stderr. Default is 131072 (128 kB).
  std::size_t buffer_size = 131072;
  /// Set to true to inherit file descriptors from parent process. Default is
  /// false. On Windows: has no effect unless read_stdout==nullptr,
  /// read_stderr==nullptr and open_stdin==false.
  bool inherit_file_descriptors = false;
};

/// Platform independent class for creating processes.
/// Note on Windows: it seems not possible to specify which pipes to redirect.
/// Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and
/// open_stdin==false, the stdout, stderr and stdin are sent to the parent
/// process instead.
//
class AKALI_API Process {
 public:
#ifdef AKALI_WIN
  typedef unsigned long id_type;  // Process id type
  typedef void* fd_type;          // File descriptor type
#if (defined UNICODE) || (defined _UNICODE)
  typedef std::wstring string_type;
#else
  typedef std::string string_type;
#endif
#else
  typedef pid_t id_type;
  typedef int fd_type;
  typedef std::string string_type;
#endif
  typedef std::unordered_map<string_type, string_type> environment_type;

 private:
  class Data {
   public:
    Data() noexcept;
    id_type id;
#ifdef AKALI_WIN
    void* handle;
#else
    int exit_status{-1};
#endif
  };

 public:
  /// Starts a process.
  Process(const std::vector<string_type>& arguments,
          const string_type& path = string_type(),
          std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
          std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
          bool open_stdin = false,
          const Config& config = {}) noexcept;

  /// Starts a process.
  Process(const string_type& command,
          const string_type& path = string_type(),
          std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
          std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
          bool open_stdin = false,
          const Config& config = {}) noexcept;

  /// Starts a process with specified environment.
  Process(const std::vector<string_type>& arguments,
          const string_type& path,
          const environment_type& environment,
          std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
          std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
          bool open_stdin = false,
          const Config& config = {}) noexcept;

  /// Starts a process with specified environment.
  Process(const string_type& command,
          const string_type& path,
          const environment_type& environment,
          std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
          std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
          bool open_stdin = false,
          const Config& config = {}) noexcept;

#ifndef AKALI_WIN
  /// Starts a process with the environment of the calling process.
  /// Supported on Unix-like systems only.
  Process(const std::function<void()>& function,
          std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
          std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
          bool open_stdin = false,
          const Config& config = {}) noexcept;
#endif
  ~Process() noexcept;

  /// Get the process id of the started process.
  id_type GetId() const noexcept;

  bool Successed() const noexcept;

  /// Wait until process is finished, and return exit status.
  int GetExitStatus() noexcept;

  /// If process is finished, returns true and sets the exit status. Returns
  /// false otherwise.
  bool TryGetExitStatus(int& exit_status) noexcept;

  /// Write to stdin.
  bool Write(const char* bytes, size_t n);

  /// Write to stdin. Convenience function using write(const char *, size_t).
  bool Write(const std::string& data);

  /// Close stdin. If the process takes parameters from stdin, use this to
  /// notify that all parameters have been sent.
  void CloseStdin() noexcept;

  /// Kill the process. force=true is only supported on Unix-like systems.
  void KillProcessTree(bool force = false) noexcept;

  /// Kill a given process id. Use KillProcessTree(bool force) instead if
  /// possible. force=true is only supported on Unix-like systems.
  static void KillProcessTree(id_type id, bool force = false) noexcept;

  /// Kill a given process id.
  /// force=true is only supported on Unix-like systems.
  static bool Kill(id_type id, bool force = false) noexcept;

#ifdef AKALI_WIN
  /// Kill all process that executed file name is executed_file_name.
  /// force=true is only supported on Unix-like systems.
  /// Return true when all process have been killed.
  /// Return false when have one or more process kill failed.
  static bool Kill(const string_type& executed_file_name, bool force = false) noexcept;

  /// terminate all process that EXE file in "dir" directory.
  static void RecursiveKill(const string_type& dir, bool exclude_self) noexcept;

  static string_type GetSelfPath() noexcept;
  static string_type GetSelfDir() noexcept;

  static string_type GetProcessPath(id_type id) noexcept;
#else
  /// Kill all process that executed file name is executed_file_name.
  /// force=true is only supported on Unix-like systems.
  /// Return true when all process have been killed.
  /// Return false when have one or more process kill failed.
  static bool Kill(const std::string& executed_file_name, bool force = false) noexcept;
  static std::string GetSelfPath() noexcept;
  static std::string GetSelfDir() noexcept;
  static std::string GetProcessPath(id_type id) noexcept;
#endif
 private:
  Data data_;
  bool closed_;
  std::mutex close_mutex_;
  std::function<void(const char* bytes, size_t n)> read_stdout_;
  std::function<void(const char* bytes, size_t n)> read_stderr_;
#ifndef AKALI_WIN
  std::thread stdout_stderr_thread_;
#else
  std::thread stdout_thread_;
  std::thread stderr_thread_;
#endif
  bool open_stdin_;
  std::mutex stdin_mutex_;

  Config config_;

  std::unique_ptr<fd_type> stdout_fd_;
  std::unique_ptr<fd_type> stderr_fd_;
  std::unique_ptr<fd_type> stdin_fd_;

  id_type open(const std::vector<string_type>& arguments,
               const string_type& path,
               const environment_type* environment = nullptr) noexcept;
  id_type open(const string_type& command,
               const string_type& path,
               const environment_type* environment = nullptr) noexcept;
#ifndef AKALI_WIN
  id_type open(const std::function<void()>& function) noexcept;
#endif
  void async_read() noexcept;
  void close_fds() noexcept;
};
}  // namespace akali

#endif  // AKALI_PROCESS_H_
