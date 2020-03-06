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

#include "akali/process.h"
#ifdef AKALI_WIN
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <cstring>
#include <stdexcept>
#include <Psapi.h>

namespace akali {
Process::Data::Data() noexcept : id(0), handle(NULL) {}

bool Process::Successed() const noexcept {
  return (data_.id > 0);
}

// Simple HANDLE wrapper to close it automatically from the destructor.
class Handle {
 public:
  Handle() noexcept : handle(INVALID_HANDLE_VALUE) {}
  ~Handle() noexcept { close(); }
  void close() noexcept {
    if (handle != INVALID_HANDLE_VALUE)
      CloseHandle(handle);
  }
  HANDLE detach() noexcept {
    HANDLE old_handle = handle;
    handle = INVALID_HANDLE_VALUE;
    return old_handle;
  }
  operator HANDLE() const noexcept { return handle; }
  HANDLE* operator&() noexcept { return &handle; }

 private:
  HANDLE handle;
};

// Based on the discussion thread:
// https://www.reddit.com/r/cpp/comments/3vpjqg/a_new_platform_independent_process_library_for_c11/cxq1wsj
std::mutex create_process_mutex;

Process::id_type Process::open(const std::vector<string_type>& arguments,
                               const string_type& path,
                               const environment_type* environment) noexcept {
  string_type command;
  for (auto& argument : arguments)
#if (defined UNICODE) || (defined _UNICODE)
    command += (command.empty() ? L"" : L" ") + argument;
#else
    command += (command.empty() ? "" : " ") + argument;
#endif
  return open(command, path, environment);
}

// Based on the example at
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx.
Process::id_type Process::open(const string_type& command,
                               const string_type& path,
                               const environment_type* environment) noexcept {
  if (open_stdin_)
    stdin_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));
  if (read_stdout_)
    stdout_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));
  if (read_stderr_)
    stderr_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));

  Handle stdin_rd_p;
  Handle stdin_wr_p;
  Handle stdout_rd_p;
  Handle stdout_wr_p;
  Handle stderr_rd_p;
  Handle stderr_wr_p;

  SECURITY_ATTRIBUTES security_attributes;

  security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  security_attributes.bInheritHandle = TRUE;
  security_attributes.lpSecurityDescriptor = nullptr;

  std::lock_guard<std::mutex> lock(create_process_mutex);
  if (stdin_fd_) {
    if (!CreatePipe(&stdin_rd_p, &stdin_wr_p, &security_attributes, 0) ||
        !SetHandleInformation(stdin_wr_p, HANDLE_FLAG_INHERIT, 0))
      return 0;
  }
  if (stdout_fd_) {
    if (!CreatePipe(&stdout_rd_p, &stdout_wr_p, &security_attributes, 0) ||
        !SetHandleInformation(stdout_rd_p, HANDLE_FLAG_INHERIT, 0)) {
      return 0;
    }
  }
  if (stderr_fd_) {
    if (!CreatePipe(&stderr_rd_p, &stderr_wr_p, &security_attributes, 0) ||
        !SetHandleInformation(stderr_rd_p, HANDLE_FLAG_INHERIT, 0)) {
      return 0;
    }
  }

  PROCESS_INFORMATION process_info;
  STARTUPINFO startup_info;

  ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

  ZeroMemory(&startup_info, sizeof(STARTUPINFO));
  startup_info.cb = sizeof(STARTUPINFO);
  startup_info.hStdInput = stdin_rd_p;
  startup_info.hStdOutput = stdout_wr_p;
  startup_info.hStdError = stderr_wr_p;
  if (stdin_fd_ || stdout_fd_ || stderr_fd_)
    startup_info.dwFlags |= STARTF_USESTDHANDLES;

  auto process_command = command;
#ifdef MSYS_PROCESS_USE_SH
  size_t pos = 0;
  while ((pos = process_command.find('\\', pos)) != string_type::npos) {
    process_command.replace(pos, 1, "\\\\\\\\");
    pos += 4;
  }
  pos = 0;
  while ((pos = process_command.find('\"', pos)) != string_type::npos) {
    process_command.replace(pos, 1, "\\\"");
    pos += 2;
  }
  process_command.insert(0, "sh -c \"");
  process_command += "\"";
#endif

  string_type environment_str;
  if (environment) {
#if (defined UNICODE) || (defined _UNICODE)
    for (const auto& e : *environment)
      environment_str += e.first + L'=' + e.second + L'\0';
    environment_str += L'\0';
#else
    for (const auto& e : *environment)
      environment_str += e.first + '=' + e.second + '\0';
    environment_str += '\0';
#endif
  }
  BOOL bSuccess = CreateProcess(
      nullptr, process_command.empty() ? nullptr : &process_command[0], nullptr, nullptr,
      stdin_fd_ || stdout_fd_ || stderr_fd_ ||
          config_.inherit_file_descriptors,  // Cannot be false when stdout,
                                             // stderr or stdin is used
      stdin_fd_ || stdout_fd_ || stderr_fd_ ? CREATE_NO_WINDOW
                                            : 0,  // CREATE_NO_WINDOW cannot be used when stdout or
                                                  // stderr is redirected to parent process
      environment_str.empty() ? nullptr : &environment_str[0],
      path.empty() ? nullptr : path.c_str(), &startup_info, &process_info);

  if (!bSuccess)
    return 0;
  else
    CloseHandle(process_info.hThread);

  if (stdin_fd_)
    *stdin_fd_ = stdin_wr_p.detach();
  if (stdout_fd_)
    *stdout_fd_ = stdout_rd_p.detach();
  if (stderr_fd_)
    *stderr_fd_ = stderr_rd_p.detach();

  closed_ = false;
  data_.id = process_info.dwProcessId;
  data_.handle = process_info.hProcess;
  return process_info.dwProcessId;
}

void Process::async_read() noexcept {
  if (data_.id == 0)
    return;

  if (stdout_fd_) {
    stdout_thread_ = std::thread([this]() {
      DWORD n;
      std::unique_ptr<char[]> buffer(new char[config_.buffer_size]);
      for (;;) {
        BOOL bSuccess = ReadFile(*stdout_fd_, static_cast<CHAR*>(buffer.get()),
                                 static_cast<DWORD>(config_.buffer_size), &n, nullptr);
        if (!bSuccess || n == 0)
          break;
        read_stdout_(buffer.get(), static_cast<size_t>(n));
      }
    });
  }
  if (stderr_fd_) {
    stderr_thread_ = std::thread([this]() {
      DWORD n;
      std::unique_ptr<char[]> buffer(new char[config_.buffer_size]);
      for (;;) {
        BOOL bSuccess = ReadFile(*stderr_fd_, static_cast<CHAR*>(buffer.get()),
                                 static_cast<DWORD>(config_.buffer_size), &n, nullptr);
        if (!bSuccess || n == 0)
          break;
        read_stderr_(buffer.get(), static_cast<size_t>(n));
      }
    });
  }
}

int Process::GetExitStatus() noexcept {
  if (data_.id == 0)
    return -1;

  DWORD exit_status;
  WaitForSingleObject(data_.handle, INFINITE);
  if (!GetExitCodeProcess(data_.handle, &exit_status))
    exit_status = -1;
  {
    std::lock_guard<std::mutex> lock(close_mutex_);
    CloseHandle(data_.handle);
    closed_ = true;
  }
  close_fds();

  return static_cast<int>(exit_status);
}

bool Process::TryGetExitStatus(int& exit_status) noexcept {
  if (data_.id == 0)
    return false;

  DWORD wait_status = WaitForSingleObject(data_.handle, 0);

  if (wait_status == WAIT_TIMEOUT)
    return false;

  DWORD exit_status_win;
  if (!GetExitCodeProcess(data_.handle, &exit_status_win))
    exit_status_win = -1;
  {
    std::lock_guard<std::mutex> lock(close_mutex_);
    CloseHandle(data_.handle);
    closed_ = true;
  }
  close_fds();

  exit_status = static_cast<int>(exit_status_win);
  return true;
}

void Process::close_fds() noexcept {
  if (stdout_thread_.joinable())
    stdout_thread_.join();
  if (stderr_thread_.joinable())
    stderr_thread_.join();

  if (stdin_fd_)
    CloseStdin();
  if (stdout_fd_) {
    if (*stdout_fd_ != NULL)
      CloseHandle(*stdout_fd_);
    stdout_fd_.reset();
  }
  if (stderr_fd_) {
    if (*stderr_fd_ != NULL)
      CloseHandle(*stderr_fd_);
    stderr_fd_.reset();
  }
}

bool Process::Write(const char* bytes, size_t n) {
  if (!open_stdin_)
    throw std::invalid_argument(
        "Can't write to an unopened stdin pipe. Please set open_stdin=true "
        "when constructing the process.");

  std::lock_guard<std::mutex> lock(stdin_mutex_);
  if (stdin_fd_) {
    DWORD written;
    BOOL bSuccess = WriteFile(*stdin_fd_, bytes, static_cast<DWORD>(n), &written, nullptr);
    if (!bSuccess || written == 0) {
      return false;
    }
    else {
      return true;
    }
  }
  return false;
}

void Process::CloseStdin() noexcept {
  std::lock_guard<std::mutex> lock(stdin_mutex_);
  if (stdin_fd_) {
    if (*stdin_fd_ != NULL)
      CloseHandle(*stdin_fd_);
    stdin_fd_.reset();
  }
}

// Based on http://stackoverflow.com/a/1173396
void Process::KillProcessTree(bool /*force*/) noexcept {
  std::lock_guard<std::mutex> lock(close_mutex_);
  if (data_.id > 0 && !closed_) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot) {
      PROCESSENTRY32 process;
      ZeroMemory(&process, sizeof(process));
      process.dwSize = sizeof(process);
      if (Process32First(snapshot, &process)) {
        do {
          if (process.th32ParentProcessID == data_.id) {
            HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
            if (process_handle) {
              TerminateProcess(process_handle, 2);
              CloseHandle(process_handle);
            }
          }
        } while (Process32Next(snapshot, &process));
      }
      CloseHandle(snapshot);
    }
    TerminateProcess(data_.handle, 2);
  }
}

// Based on http://stackoverflow.com/a/1173396
void Process::KillProcessTree(id_type id, bool /*force*/) noexcept {
  if (id == 0)
    return;

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot) {
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
      do {
        if (process.th32ParentProcessID == id) {
          HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
          if (process_handle) {
            TerminateProcess(process_handle, 2);
            CloseHandle(process_handle);
          }
        }
      } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);
  }
  HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, id);
  if (process_handle)
    TerminateProcess(process_handle, 2);
}

bool Process::Kill(id_type id, bool /*force*/) noexcept {
  if (id == 0)
    return false;

  HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, id);
  if (!process_handle)
    return false;
  return !!TerminateProcess(process_handle, 2);
}

bool Process::Kill(const string_type& executed_file_name, bool /*force*/) noexcept {
  if (executed_file_name.length() == 0)
    return false;

  bool ret = false;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (snapshot) {
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
      do {
        if (lstrcmpi(process.szExeFile, executed_file_name.c_str()) == 0) {
          HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
          if (process_handle) {
            if (::TerminateProcess(process_handle, 2))
              ret = true;
            CloseHandle(process_handle);
          }
        }
      } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);
  }
  return ret;
}

void Process::RecursiveKill(const string_type& dir, bool exclude_self) noexcept {
  size_t len = dir.length();
  TCHAR szTemp[MAX_PATH] = {0};

  StringCchCopy(szTemp, MAX_PATH, dir.c_str());

  if (szTemp[len - 1] != TEXT('\\'))
    _tcscat_s(szTemp, MAX_PATH, TEXT("\\"));

  StringCchCat(szTemp, MAX_PATH, TEXT("*.*"));

  WIN32_FIND_DATA filedata;
  HANDLE fhandle = FindFirstFile(szTemp, &filedata);

  if (fhandle != INVALID_HANDLE_VALUE) {
    if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) &&
           (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
        TCHAR szTemp[MAX_PATH] = {0};
        StringCchCat(szTemp, MAX_PATH, dir.c_str());

        if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
          StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

        _tcscat_s(szTemp, MAX_PATH, filedata.cFileName);
        RecursiveKill(szTemp, exclude_self);
      }
    }
    else {
      TCHAR* p = _tcsrchr(filedata.cFileName, TEXT('.'));

      if (p) {
        if (_tcscmp(p, TEXT(".exe")) == 0) {
          bool teminate = true;
          if (exclude_self) {
            TCHAR szSelf[MAX_PATH] = {0};
            GetModuleFileName(NULL, szSelf, MAX_PATH);
            LPWSTR pSelf = PathFindFileName(szSelf);
            if (_tcscmp(filedata.cFileName, pSelf) == 0)
              teminate = false;
          }
          if (teminate) {
            int tryTimes = 0;
            while (Process::Kill(filedata.cFileName, 333) && ++tryTimes < 20);
          }
        }
      }
    }

    while (FindNextFile(fhandle, &filedata) != 0) {
      if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) &&
             (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
          TCHAR szTemp[MAX_PATH] = {0};
          StringCchCopy(szTemp, MAX_PATH, dir.c_str());

          if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
            StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

          StringCchCat(szTemp, MAX_PATH, filedata.cFileName);
          RecursiveKill(szTemp, exclude_self);
        }
      }
      else {
        TCHAR* p = _tcsrchr(filedata.cFileName, TEXT('.'));

        if (p) {
          if (_tcscmp(p, TEXT(".exe")) == 0) {
            bool teminate = true;
            if (exclude_self) {
              TCHAR szSelf[MAX_PATH] = {0};
              GetModuleFileName(NULL, szSelf, MAX_PATH);
              LPWSTR pSelf = PathFindFileName(szSelf);
              if (_tcscmp(filedata.cFileName, pSelf) == 0)
                teminate = false;
            }
            if (teminate) {
              int tryTimes = 0;
              while (Process::Kill(filedata.cFileName, 2) && ++tryTimes < 20);
            }
          }
        }
      }
    }
  }

  FindClose(fhandle);
}

Process::string_type Process::GetSelfPath() noexcept {
  TCHAR szPath[MAX_PATH] = {0};
  GetModuleFileName(NULL, szPath, MAX_PATH);
  return string_type(szPath);
}

Process::string_type Process::GetSelfDir() noexcept {
  TCHAR szPath[MAX_PATH] = {0};
  GetModuleFileName(NULL, szPath, MAX_PATH);
  PathRemoveFileSpec(szPath);
  PathAddBackslash(szPath);
  return string_type(szPath);
}

Process::string_type Process::GetProcessPath(Process::id_type id) noexcept {
  Process::string_type strPath;
  TCHAR Filename[MAX_PATH] = {0};
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id);
  if (hProcess == NULL)
    return strPath;
  HMODULE hModule = NULL;
  DWORD cbNeeded;
  if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
    if (GetModuleFileNameEx(hProcess, hModule, Filename, MAX_PATH)) {
      strPath = Filename;
    }
  }
  else {
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageName(hProcess, 0, Filename, &size)) {
      strPath = Filename;
    }
  }
  CloseHandle(hProcess);

  return strPath;
}
}  // namespace akali
#endif