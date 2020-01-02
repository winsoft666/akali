#include "akali/process.hpp"
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__NT__)

#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <set>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iosfwd>
#include <limits.h>

// clang-format off
#if (defined UNICODE) || (defined _UNICODE)
#define __T(x) L ## x
#else
#define __T(x) x
#endif
#define _T(x) __T(x)
// clang-format on

namespace akali {
Process::Data::Data() noexcept
    : id(-1) {}

Process::Process(const std::function<void()> &function,
                 std::function<void(const char *, size_t)> read_stdout,
                 std::function<void(const char *, size_t)> read_stderr, bool open_stdin,
                 const Config &config) noexcept
    : closed_(true)
    , read_stdout_(std::move(read_stdout))
    , read_stderr_(std::move(read_stderr))
    , open_stdin_(open_stdin)
    , config_(config) {
  open(function);
  async_read();
}

Process::id_type Process::open(const std::function<void()> &function) noexcept {
  if (open_stdin_)
    stdin_fd_ = std::unique_ptr<fd_type>(new fd_type);
  if (read_stdout_)
    stdout_fd_ = std::unique_ptr<fd_type>(new fd_type);
  if (read_stderr_)
    stderr_fd_ = std::unique_ptr<fd_type>(new fd_type);

  int stdin_p[2], stdout_p[2], stderr_p[2];

  if (stdin_fd_ && pipe(stdin_p) != 0)
    return -1;
  if (stdout_fd_ && pipe(stdout_p) != 0) {
    if (stdin_fd_) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    return -1;
  }
  if (stderr_fd_ && pipe(stderr_p) != 0) {
    if (stdin_fd_) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if (stdout_fd_) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    return -1;
  }

  id_type pid = fork();

  if (pid < 0) {
    if (stdin_fd_) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if (stdout_fd_) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    if (stderr_fd_) {
      close(stderr_p[0]);
      close(stderr_p[1]);
    }
    return pid;
  }
  else if (pid == 0) {
    if (stdin_fd_)
      dup2(stdin_p[0], 0);
    if (stdout_fd_)
      dup2(stdout_p[1], 1);
    if (stderr_fd_)
      dup2(stderr_p[1], 2);
    if (stdin_fd_) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if (stdout_fd_) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    if (stderr_fd_) {
      close(stderr_p[0]);
      close(stderr_p[1]);
    }

    if (!config_.inherit_file_descriptors) {
      // Optimization on some systems: using 8 * 1024 (Debian's default
      // _SC_OPEN_MAX) as fd_max limit
      int fd_max = std::min(8192,
                            static_cast<int>(sysconf(_SC_OPEN_MAX))); // Truncation is safe
      if (fd_max < 0)
        fd_max = 8192;
      for (int fd = 3; fd < fd_max; fd++)
        close(fd);
    }

    setpgid(0, 0);
    // TODO: See here on how to emulate tty for colors:
    // http://stackoverflow.com/questions/1401002/trick-an-application-into-thinking-its-stdin-is-interactive-not-a-pipe
    // TODO: One solution is: echo "command;exit"|script -q /dev/null

    if (function)
      function();

    _exit(EXIT_FAILURE);
  }

  if (stdin_fd_)
    close(stdin_p[0]);
  if (stdout_fd_)
    close(stdout_p[1]);
  if (stderr_fd_)
    close(stderr_p[1]);

  if (stdin_fd_)
    *stdin_fd_ = stdin_p[1];
  if (stdout_fd_)
    *stdout_fd_ = stdout_p[0];
  if (stderr_fd_)
    *stderr_fd_ = stderr_p[0];

  closed_ = false;
  data_.id = pid;
  return pid;
}

Process::id_type Process::open(const std::vector<string_type> &arguments, const string_type &path,
                               const environment_type *environment) noexcept {
  return open([&arguments, &path, &environment] {
    if (arguments.empty())
      exit(127);

    std::vector<const char *> argv_ptrs;
    argv_ptrs.reserve(arguments.size() + 1);
    for (auto &argument : arguments)
      argv_ptrs.emplace_back(argument.c_str());
    argv_ptrs.emplace_back(nullptr);

    if (!path.empty()) {
      if (chdir(path.c_str()) != 0)
        exit(1);
    }

    if (!environment)
      execv(arguments[0].c_str(), const_cast<char *const *>(argv_ptrs.data()));
    else {
      std::vector<std::string> env_strs;
      std::vector<const char *> env_ptrs;
      env_strs.reserve(environment->size());
      env_ptrs.reserve(environment->size() + 1);
      for (const auto &e : *environment) {
        env_strs.emplace_back(e.first + '=' + e.second);
        env_ptrs.emplace_back(env_strs.back().c_str());
      }
      env_ptrs.emplace_back(nullptr);

      execve(arguments[0].c_str(), const_cast<char *const *>(argv_ptrs.data()),
             const_cast<char *const *>(env_ptrs.data()));
    }
  });
}

Process::id_type Process::open(const std::string &command, const std::string &path,
                               const environment_type *environment) noexcept {
  return open([&command, &path, &environment] {
    if (!path.empty()) {
      if (chdir(path.c_str()) != 0)
        exit(1);
    }

    if (!environment)
      execl("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr);
    else {
      std::vector<std::string> env_strs;
      std::vector<const char *> env_ptrs;
      env_strs.reserve(environment->size());
      env_ptrs.reserve(environment->size() + 1);
      for (const auto &e : *environment) {
        env_strs.emplace_back(e.first + '=' + e.second);
        env_ptrs.emplace_back(env_strs.back().c_str());
      }
      env_ptrs.emplace_back(nullptr);
      execle("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr, env_ptrs.data());
    }
  });
}

void Process::async_read() noexcept {
  if (data_.id <= 0 || (!stdout_fd_ && !stderr_fd_))
    return;

  stdout_stderr_thread_ = std::thread([this] {
    std::vector<pollfd> pollfds;
    std::bitset<2> fd_is_stdout;
    if (stdout_fd_) {
      fd_is_stdout.set(pollfds.size());
      pollfds.emplace_back();
      pollfds.back().fd = fcntl(*stdout_fd_, F_SETFL, fcntl(*stdout_fd_, F_GETFL) | O_NONBLOCK) == 0
                              ? *stdout_fd_
                              : -1;
      pollfds.back().events = POLLIN;
    }
    if (stderr_fd_) {
      pollfds.emplace_back();
      pollfds.back().fd = fcntl(*stderr_fd_, F_SETFL, fcntl(*stderr_fd_, F_GETFL) | O_NONBLOCK) == 0
                              ? *stderr_fd_
                              : -1;
      pollfds.back().events = POLLIN;
    }
    auto buffer = std::unique_ptr<char[]>(new char[config_.buffer_size]);
    bool any_open = !pollfds.empty();
    while (any_open && (poll(pollfds.data(), pollfds.size(), -1) > 0 || errno == EINTR)) {
      any_open = false;
      for (size_t i = 0; i < pollfds.size(); ++i) {
        if (pollfds[i].fd >= 0) {
          if (pollfds[i].revents & POLLIN) {
            const ssize_t n = read(pollfds[i].fd, buffer.get(), config_.buffer_size);
            if (n > 0) {
              if (fd_is_stdout[i])
                read_stdout_(buffer.get(), static_cast<size_t>(n));
              else
                read_stderr_(buffer.get(), static_cast<size_t>(n));
            }
            else if (n < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
              pollfds[i].fd = -1;
              continue;
            }
          }
          if (pollfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            pollfds[i].fd = -1;
            continue;
          }
          any_open = true;
        }
      }
    }
  });
}

int Process::GetExitStatus() noexcept {
  if (data_.id <= 0)
    return -1;

  int exit_status;
  id_type p;
  do {
    p = waitpid(data_.id, &exit_status, 0);
  } while (p < 0 && errno == EINTR);

  if (p < 0 && errno == ECHILD) {
    // PID doesn't exist anymore, return previously sampled exit status (or
    // -1)
    return data_.exit_status;
  }
  else {
    // store exit status for future calls
    if (exit_status >= 256)
      exit_status = exit_status >> 8;
    data_.exit_status = exit_status;
  }

  {
    std::lock_guard<std::mutex> lock(close_mutex_);
    closed_ = true;
  }
  close_fds();

  return exit_status;
}

bool Process::TryGetExitStatus(int &exit_status) noexcept {
  if (data_.id <= 0)
    return false;

  const id_type p = waitpid(data_.id, &exit_status, WNOHANG);
  if (p < 0 && errno == ECHILD) {
    // PID doesn't exist anymore, set previously sampled exit status (or -1)
    exit_status = data_.exit_status;
    return true;
  }
  else if (p <= 0) {
    // Process still running (p==0) or error
    return false;
  }
  else {
    // store exit status for future calls
    if (exit_status >= 256)
      exit_status = exit_status >> 8;
    data_.exit_status = exit_status;
  }

  {
    std::lock_guard<std::mutex> lock(close_mutex_);
    closed_ = true;
  }
  close_fds();

  return true;
}

void Process::close_fds() noexcept {
  if (stdout_stderr_thread_.joinable())
    stdout_stderr_thread_.join();

  if (stdin_fd_)
    CloseStdin();
  if (stdout_fd_) {
    if (data_.id > 0)
      close(*stdout_fd_);
    stdout_fd_.reset();
  }
  if (stderr_fd_) {
    if (data_.id > 0)
      close(*stderr_fd_);
    stderr_fd_.reset();
  }
}

bool Process::Write(const char *bytes, size_t n) {
  if (!open_stdin_)
    throw std::invalid_argument("Can't write to an unopened stdin pipe. Please set open_stdin=true "
                                "when constructing the process.");

  std::lock_guard<std::mutex> lock(stdin_mutex_);
  if (stdin_fd_) {
    if (::write(*stdin_fd_, bytes, n) >= 0) {
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

void Process::CloseStdin() noexcept {
  std::lock_guard<std::mutex> lock(stdin_mutex_);
  if (stdin_fd_) {
    if (data_.id > 0)
      close(*stdin_fd_);
    stdin_fd_.reset();
  }
}

void Process::KillProcessTree(bool force) noexcept {
  std::lock_guard<std::mutex> lock(close_mutex_);
  if (data_.id > 0 && !closed_) {
    if (force)
      ::kill(-data_.id, SIGTERM);
    else
      ::kill(-data_.id, SIGINT);
  }
}

void Process::KillProcessTree(id_type id, bool force) noexcept {
  if (id <= 0)
    return;

  if (force)
    ::kill(-id, SIGTERM);
  else
    ::kill(-id, SIGINT);
}

bool Process::Kill(id_type id, bool force) noexcept {
  if (force)
    return ::kill(id, SIGTERM) == 0;
  return ::kill(id, SIGINT) == 0;
}

bool Process::Kill(const std::string &executed_file_name, bool force) noexcept {
  if (executed_file_name.length() == 0)
    return false;

  bool ret = true;

  int pid = -1;

  // Open the /proc directory
  DIR *dp = opendir("/proc");
  if (dp != NULL) {
    // Enumerate all entries in directory until process found
    struct dirent *dirp;
    while (pid < 0 && (dirp = readdir(dp))) {
      // Skip non-numeric entries
      int id = atoi(dirp->d_name);
      if (id > 0) {
        // Read contents of virtual /proc/{pid}/cmdline file

        std::string cmdPath = std::string("/proc/") + dirp->d_name + std::string("/cmdline");
        std::ifstream cmdFile(cmdPath.c_str());

        std::string cmdLine;
        std::getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
          // Keep first cmdline item which contains the program path
          size_t pos = cmdLine.find('\0');
          if (pos != string_type::npos)
            cmdLine = cmdLine.substr(0, pos);
          // Keep program name only, removing the path
          pos = cmdLine.rfind('/');
          if (pos != std::string::npos)
            cmdLine = cmdLine.substr(pos + 1);
          // Compare against requested process name
          if (executed_file_name == cmdLine) {
            if (::kill(id, force ? SIGTERM : SIGINT) != 0)
              ret = false;
          }
        }
      }
    }
  }

  closedir(dp);

  return ret;
}

std::string Process::GetSelfPath() noexcept {
  char buf[PATH_MAX] = {0};
  ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len == -1) {
    return "";
  }
  return std::string(buf);
}

std::string Process::GetSelfDir() noexcept {
  char buf[PATH_MAX] = {0};
  ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len == -1) {
    return "";
  }
  std::string path = buf;
  std::string::size_type pos = path.find_last_of("/");
  if (pos != std::string::npos) {
    path = path.substr(0, pos + 1);
  }
  return path;
}
} // namespace akali
#endif