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

#ifndef AKALI_LOGGING_H_
#define AKALI_LOGGING_H_
#pragma once

#include <errno.h>
#include <list>
#include <sstream>
#include <string>
#include <utility>
#include "constructormagic.h"
#include "deprecation.h"
#include "stringencode.h"
#include "akali_export.h"

namespace akali {
enum LoggingSeverity { LS_SENSITIVE, LS_VERBOSE, LS_INFO, LS_WARNING, LS_ERROR, LS_NONE };

enum LogErrorContext {
  ERRCTX_NONE,
  ERRCTX_ERRNO,   // System-local errno
  ERRCTX_HRESULT, // Windows HRESULT
  ERRCTX_OSSTATUS // MacOS OSStatus
};

// Virtual sink interface that can receive log messages.
class AKALI_API LogSink {
public:
  LogSink() {}
  virtual ~LogSink() {}
  virtual void OnLogMessage(const std::string &message) = 0;
};

class AKALI_API LogMessage {
public:
  LogMessage(const char *file, int line, LoggingSeverity sev, LogErrorContext err_ctx = ERRCTX_NONE,
             int err = 0);

  ~LogMessage();

  static bool Loggable(LoggingSeverity sev);
  std::ostream &stream();

  static int64_t LogStartTime();

  static void LogThreads(bool on = true);

  static void LogTimestamps(bool on = true);

  // These are the available logging channels
  //  Debug: Debug console on Windows, otherwise stderr
  static void LogToDebug(LoggingSeverity min_sev);
  static LoggingSeverity GetLogToDebug();

  // Sets whether logs will be directed to stderr in debug mode.
  static void SetLogToStderr(bool log_to_stderr);

  static int GetLogToStream(LogSink *stream = nullptr);
  static void AddLogToStream(LogSink *stream, LoggingSeverity min_sev);
  static void RemoveLogToStream(LogSink *stream);

  static int GetMinLogSeverity();

private:
  typedef std::pair<LogSink *, LoggingSeverity> StreamAndSeverity;
  typedef std::list<StreamAndSeverity> StreamList;

  // Updates min_sev_ appropriately when debug sinks change.
  static void UpdateMinLogSeverity();

  static void OutputToDebug(const std::string &msg, LoggingSeverity severity);

  // Checks the current global debug severity and if the |streams_| collection
  // is empty. If |severity| is smaller than the global severity and if the
  // |streams_| collection is empty, the LogMessage will be considered a noop
  // LogMessage.
  static bool IsNoop(LoggingSeverity severity);

  // Called from the dtor (or from a test) to append optional extra error
  // information to the log stream and a newline character.
  void FinishPrintStream();

  class Impl;
  Impl *impl_;

  AKALI_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

// This class is used to explicitly ignore values in the conditional logging macros.
// This avoids compiler warnings like "value computed is not used" and "statement has no effect".
class AKALI_API LogMessageVoidify {
public:
  LogMessageVoidify() {}
  // This has to be an operator with a precedence lower than << but higher than ?:
  void operator&(std::ostream &) {}
};

#define AKALI_LOG_SEVERITY_PRECONDITION(sev)                                                         \
  !(akali::LogMessage::Loggable(sev)) ? (void)0 : akali::LogMessageVoidify() &

#define AKALI_LOG(sev)                                                                               \
  AKALI_LOG_SEVERITY_PRECONDITION(akali::sev)                                                    \
  akali::LogMessage(__FILE__, __LINE__, akali::sev).stream()

#define AKALI_LOG_E(sev, ctx, err, ...)                                                              \
  AKALI_LOG_SEVERITY_PRECONDITION(akali::sev)                                                    \
  akali::LogMessage(__FILE__, __LINE__, akali::sev, akali::##ctx, err, ##__VA_ARGS__)  \
      .stream()

#define AKALI_LOG_ERRNO_EX(sev, err) AKALI_LOG_E(sev, ERRCTX_ERRNO, err)
#define AKALI_LOG_ERRNO(sev) AKALI_LOG_ERRNO_EX(sev, errno)

#if (defined _WIN32 || defined WIN32)
#define AKALI_LOG_GLE_EX(sev, err) AKALI_LOG_E(sev, ERRCTX_HRESULT, err)
#define AKALI_LOG_GLE(sev) AKALI_LOG_GLE_EX(sev, GetLastError())
#endif

AKALI_API void TraceMsgW(const wchar_t *lpFormat, ...);
AKALI_API void TraceMsgA(const char *lpFormat, ...);

#if (defined UNICODE) || (defined _UNICODE)
#define TraceMsg akali::TraceMsgW
#else
#define TraceMsg akali::TraceMsgA
#endif
} // namespace akali
#endif