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

#if (defined _WIN32 || defined WIN32)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <strsafe.h>
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#undef ERROR // wingdi.h
#endif

#include <string.h>
#include <time.h>
#include <limits.h>
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <vector>
#include <mutex>
#include <stdarg.h>
#include "akali/logging.h"
#include "akali/stringencode.h"
#include "akali/timeutils.h"
#include "akali/safe_release_macro.h"

#pragma warning(disable : 4995)

namespace akali {
namespace {
#if !defined(NDEBUG)
static LoggingSeverity g_min_sev = LS_SENSITIVE;
static LoggingSeverity g_dbg_sev = LS_SENSITIVE;
#else
static LoggingSeverity g_min_sev = LS_SENSITIVE;
static LoggingSeverity g_dbg_sev = LS_SENSITIVE;
#endif

// Return the filename portion of the string (that following the last slash).
const char *FilenameFromPath(const char *file) {
  const char *end1 = ::strrchr(file, '/');
  const char *end2 = ::strrchr(file, '\\');

  if (!end1 && !end2)
    return file;
  else
    return (end1 > end2) ? end1 + 1 : end2 + 1;
}

std::ostream &GetNoopStream() {
  class NoopStreamBuf : public std::streambuf {
  public:
    int overflow(int c) override { return c; }
  };
  static NoopStreamBuf noop_buffer;
  static std::ostream noop_stream(&noop_buffer);
  return noop_stream;
}

// Global lock for log subsystem, only needed to serialize access to streams_.
std::mutex g_log_crit;
} // namespace

class LogMessage::Impl {
public:
  Impl() { is_noop_ = false; }

  ~Impl() {}

  // The ostream that buffers the formatted message before output
  std::ostringstream print_stream_;

  // The severity level of this message
  LoggingSeverity severity_;

  // String data generated in the constructor, that should be appended to the message before output.
  std::string extra_;

  bool is_noop_;

  // The output streams and their associated severities
  static StreamList streams_;

  // Flags for formatting options
  static bool thread_;
  static bool timestamp_;

  // Determines if logs will be directed to stderr in debug mode.
  static bool log_to_stderr_;
};

bool LogMessage::Impl::log_to_stderr_ = true;

LogMessage::StreamList LogMessage::Impl::streams_;

// Boolean options default to true (1)
bool LogMessage::Impl::thread_ = 1;
bool LogMessage::Impl::timestamp_ = 1;

LogMessage::LogMessage(const char *file, int line, LoggingSeverity sev, LogErrorContext err_ctx,
                       int err) {
  impl_ = new Impl();

  impl_->severity_ = sev;
  impl_->is_noop_ = IsNoop(sev);

  if (impl_->is_noop_)
    return;

  if (impl_->timestamp_) {
    impl_->print_stream_ << "[" << GetLocalTime().ToString(true) << "] ";
  }

  if (impl_->thread_) {
#if (defined _WIN32 || defined WIN32)
    DWORD id = GetCurrentThreadId();
    impl_->print_stream_ << "[" << std::dec << id << "] ";
#endif
  }

  switch (sev) {
  case akali::LS_SENSITIVE:
    impl_->print_stream_ << "[SENSITIVE] ";
    break;
  case akali::LS_VERBOSE:
    impl_->print_stream_ << "[VERBOSE] ";
    break;
  case akali::LS_INFO:
    impl_->print_stream_ << "[INFO] ";
    break;
  case akali::LS_WARNING:
    impl_->print_stream_ << "[WARNING] ";
    break;
  case akali::LS_ERROR:
    impl_->print_stream_ << "[ERROR] ";
    break;
  case akali::LS_NONE:
  default:
    break;
  }

#if (defined DEBUG || defined _DEBUG)
  if (file != nullptr)
    impl_->print_stream_ << "(" << FilenameFromPath(file) << ":" << line << "): ";
#endif

  if (err_ctx != ERRCTX_NONE) {
    std::stringstream tmp;
    char prefix[14] = {0};
    snprintf(prefix, sizeof(prefix), "[0x%08X]", err);
    tmp << prefix;
    char err_buf[100] = {0};

    switch (err_ctx) {
    case ERRCTX_ERRNO:
#if (defined _WIN32 || defined WIN32)
      strerror_s(err_buf, 100, err);
#else
      strerror_r(err, err_buf, 100);
#endif
      tmp << " " << err_buf;
      break;
#if (defined _WIN32 || defined WIN32)

    case ERRCTX_HRESULT: {
      char msgbuf[256];
      DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

      if (DWORD len = FormatMessageA(flags, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                     msgbuf, sizeof(msgbuf) / sizeof(msgbuf[0]), nullptr)) {
        while ((len > 0) && isspace(static_cast<unsigned char>(msgbuf[len - 1]))) {
          msgbuf[--len] = 0;
        }

        tmp << " " << msgbuf;
      }

      break;
    }

#endif

    default:
      break;
    }

    impl_->extra_ = tmp.str();
  }
}

LogMessage::~LogMessage() {
  if (impl_->is_noop_)
    return;

  FinishPrintStream();

  const std::string str = impl_->print_stream_.str();

  if (impl_->severity_ >= g_dbg_sev) {
    OutputToDebug(str, impl_->severity_);
  }

  std::lock_guard<std::mutex> lg(g_log_crit);

  for (auto &kv : impl_->streams_) {
    if (impl_->severity_ >= kv.second) {
      kv.first->OnLogMessage(str);
    }
  }

  SAFE_DELETE(impl_);
}

std::ostream &LogMessage::stream() {
  return impl_->is_noop_ ? GetNoopStream() : impl_->print_stream_;
}

bool LogMessage::Loggable(LoggingSeverity sev) { return sev >= g_min_sev; }

int LogMessage::GetMinLogSeverity() { return g_min_sev; }

LoggingSeverity LogMessage::GetLogToDebug() { return g_dbg_sev; }

int64_t LogMessage::LogStartTime() {
  static const int64_t g_start = GetTimeStamp() / 1000;
  return g_start;
}

void LogMessage::LogThreads(bool on) { Impl::thread_ = on; }

void LogMessage::LogTimestamps(bool on) { Impl::timestamp_ = on; }

void LogMessage::LogToDebug(LoggingSeverity min_sev) {
  g_dbg_sev = min_sev;
  std::lock_guard<std::mutex> lg(g_log_crit);
  UpdateMinLogSeverity();
}

void LogMessage::SetLogToStderr(bool log_to_stderr) { Impl::log_to_stderr_ = log_to_stderr; }

int LogMessage::GetLogToStream(LogSink *stream) {
  std::lock_guard<std::mutex> lg(g_log_crit);
  LoggingSeverity sev = LS_NONE;

  for (auto &kv : Impl::streams_) {
    if (!stream || stream == kv.first) {
      sev = std::min(sev, kv.second);
    }
  }

  return sev;
}

void LogMessage::AddLogToStream(LogSink *stream, LoggingSeverity min_sev) {
  std::lock_guard<std::mutex> lg(g_log_crit);
  Impl::streams_.push_back(std::make_pair(stream, min_sev));
  UpdateMinLogSeverity();
}

void LogMessage::RemoveLogToStream(LogSink *stream) {
  std::lock_guard<std::mutex> lg(g_log_crit);

  for (StreamList::iterator it = Impl::streams_.begin(); it != Impl::streams_.end(); ++it) {
    if (stream == it->first) {
      Impl::streams_.erase(it);
      break;
    }
  }

  UpdateMinLogSeverity();
}

void LogMessage::UpdateMinLogSeverity() {
  LoggingSeverity min_sev = g_dbg_sev;

  for (auto &kv : Impl::streams_) {
    min_sev = std::min(g_dbg_sev, kv.second);
  }

  g_min_sev = min_sev;
}

void LogMessage::OutputToDebug(const std::string &str, LoggingSeverity severity) {
  bool log_to_stderr = Impl::log_to_stderr_;
#if (defined _WIN32 || defined WIN32)
  OutputDebugStringA(str.c_str());

  if (log_to_stderr) {
    if (HANDLE error_handle = ::GetStdHandle(STD_ERROR_HANDLE)) {
      log_to_stderr = false;
      DWORD written = 0;
      ::WriteFile(error_handle, str.data(), static_cast<DWORD>(str.size()), &written, 0);
    }
  }

#endif // Win32

  if (log_to_stderr) {
    fprintf(stderr, "%s", str.c_str());
    fflush(stderr);
  }
}

// static
bool LogMessage::IsNoop(LoggingSeverity severity) {
  if (severity >= g_dbg_sev)
    return false;

  std::lock_guard<std::mutex> lg(g_log_crit);
  return Impl::streams_.size() == 0;
}

void LogMessage::FinishPrintStream() {
  if (impl_->is_noop_)
    return;

  if (!impl_->extra_.empty())
    impl_->print_stream_ << " : " << impl_->extra_;

  impl_->print_stream_ << "\r\n";
}

//////////////////////////////////////////////////////////////////////

void TraceMsgW(const wchar_t *lpFormat, ...) {
  if (!lpFormat)
    return;

#ifdef WIN32
  wchar_t *pMsgBuffer = NULL;
  unsigned int iMsgBufCount = 0;

  va_list arglist;
  va_start(arglist, lpFormat);
  HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

  while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
    iMsgBufCount += 1024;

    if (pMsgBuffer) {
      free(pMsgBuffer);
      pMsgBuffer = NULL;
    }

    pMsgBuffer = (wchar_t *)malloc(iMsgBufCount * sizeof(wchar_t));

    if (!pMsgBuffer) {
      break;
    }

    hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
  }

  va_end(arglist);

  if (hr == S_OK) {
    if (pMsgBuffer)
      OutputDebugStringW(pMsgBuffer);
  }

  if (pMsgBuffer) {
    free(pMsgBuffer);
    pMsgBuffer = NULL;
  }
#else
  wchar_t msgBuf[1024] = {0};
  va_list arglist;
  va_start(arglist, lpFormat);
  int err = vswprintf(msgBuf, 1024, lpFormat, arglist);
  va_end(arglist);
  printf("%ls\n", msgBuf);
#endif
}

void TraceMsgA(const char *lpFormat, ...) {
  if (!lpFormat)
    return;

#ifdef WIN32
  char *pMsgBuffer = NULL;
  unsigned int iMsgBufCount = 0;

  va_list arglist;
  va_start(arglist, lpFormat);
  HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

  while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
    iMsgBufCount += 1024;

    if (pMsgBuffer) {
      free(pMsgBuffer);
      pMsgBuffer = NULL;
    }

    pMsgBuffer = (char *)malloc(iMsgBufCount * sizeof(char));

    if (!pMsgBuffer) {
      break;
    }

    hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
  }

  va_end(arglist);

  if (hr == S_OK) {
    if (pMsgBuffer)
      OutputDebugStringA(pMsgBuffer);
  }

  if (pMsgBuffer) {
    free(pMsgBuffer);
    pMsgBuffer = NULL;
  }
#else
  char msgBuf[1024] = {0};
  va_list arglist;
  va_start(arglist, lpFormat);
  int err = vsnprintf(msgBuf, 1024, lpFormat, arglist);
  va_end(arglist);
  printf("%s\n", msgBuf);
#endif
}
} // namespace akali