#include "akali/process.hpp"

namespace akali {
Process::Process(const std::vector<string_type> &arguments, const string_type &path,
                 std::function<void(const char *bytes, size_t n)> read_stdout,
                 std::function<void(const char *bytes, size_t n)> read_stderr, bool open_stdin,
                 const Config &config) noexcept
    : closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)),
      open_stdin_(open_stdin), config_(config) {
  open(arguments, path);
  async_read();
}

Process::Process(const string_type &command, const string_type &path,
                 std::function<void(const char *bytes, size_t n)> read_stdout,
                 std::function<void(const char *bytes, size_t n)> read_stderr, bool open_stdin,
                 const Config &config) noexcept
    : closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)),
      open_stdin_(open_stdin), config_(config) {
  open(command, path);
  async_read();
}

Process::Process(const std::vector<string_type> &arguments, const string_type &path,
                 const environment_type &environment,
                 std::function<void(const char *bytes, size_t n)> read_stdout,
                 std::function<void(const char *bytes, size_t n)> read_stderr, bool open_stdin,
                 const Config &config) noexcept
    : closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)),
      open_stdin_(open_stdin), config_(config) {
  open(arguments, path, &environment);
  async_read();
}

Process::Process(const string_type &command, const string_type &path,
                 const environment_type &environment,
                 std::function<void(const char *bytes, size_t n)> read_stdout,
                 std::function<void(const char *bytes, size_t n)> read_stderr, bool open_stdin,
                 const Config &config) noexcept
    : closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)),
      open_stdin_(open_stdin), config_(config) {
  open(command, path, &environment);
  async_read();
}

Process::~Process() noexcept { close_fds(); }

Process::id_type Process::GetId() const noexcept { return data_.id; }

bool Process::Write(const std::string &data) { return Write(data.c_str(), data.size()); }
} // namespace akali