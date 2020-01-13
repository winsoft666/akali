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

namespace akali {
Process::Process(const std::vector<string_type>& arguments,
                 const string_type& path,
                 std::function<void(const char* bytes, size_t n)> read_stdout,
                 std::function<void(const char* bytes, size_t n)> read_stderr,
                 bool open_stdin,
                 const Config& config) noexcept
    : closed_(true)
    , read_stdout_(std::move(read_stdout))
    , read_stderr_(std::move(read_stderr))
    , open_stdin_(open_stdin)
    , config_(config) {
  open(arguments, path);
  async_read();
}

Process::Process(const string_type& command,
                 const string_type& path,
                 std::function<void(const char* bytes, size_t n)> read_stdout,
                 std::function<void(const char* bytes, size_t n)> read_stderr,
                 bool open_stdin,
                 const Config& config) noexcept
    : closed_(true)
    , read_stdout_(std::move(read_stdout))
    , read_stderr_(std::move(read_stderr))
    , open_stdin_(open_stdin)
    , config_(config) {
  open(command, path);
  async_read();
}

Process::Process(const std::vector<string_type>& arguments,
                 const string_type& path,
                 const environment_type& environment,
                 std::function<void(const char* bytes, size_t n)> read_stdout,
                 std::function<void(const char* bytes, size_t n)> read_stderr,
                 bool open_stdin,
                 const Config& config) noexcept
    : closed_(true)
    , read_stdout_(std::move(read_stdout))
    , read_stderr_(std::move(read_stderr))
    , open_stdin_(open_stdin)
    , config_(config) {
  open(arguments, path, &environment);
  async_read();
}

Process::Process(const string_type& command,
                 const string_type& path,
                 const environment_type& environment,
                 std::function<void(const char* bytes, size_t n)> read_stdout,
                 std::function<void(const char* bytes, size_t n)> read_stderr,
                 bool open_stdin,
                 const Config& config) noexcept
    : closed_(true)
    , read_stdout_(std::move(read_stdout))
    , read_stderr_(std::move(read_stderr))
    , open_stdin_(open_stdin)
    , config_(config) {
  open(command, path, &environment);
  async_read();
}

Process::~Process() noexcept {
  close_fds();
}

Process::id_type Process::GetId() const noexcept {
  return data_.id;
}

bool Process::Write(const std::string& data) {
  return Write(data.c_str(), data.size());
}
}  // namespace akali