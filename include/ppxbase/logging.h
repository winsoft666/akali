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

#ifndef PPX_BASE_LOGGING_H_
#define PPX_BASE_LOGGING_H_
#pragma once

#include <errno.h>
#include <list>
#include <sstream>
#include <string>
#include <utility>
#include "constructormagic.h"
#include "deprecation.h"
#include "stringencode.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        enum LoggingSeverity {
            LS_SENSITIVE,
            LS_VERBOSE,
            LS_INFO,
            LS_WARNING,
            LS_ERROR,
            LS_NONE
        };

        enum LogErrorContext {
            ERRCTX_NONE,
            ERRCTX_ERRNO,     // System-local errno
            ERRCTX_HRESULT,   // Windows HRESULT
            ERRCTX_OSSTATUS   // MacOS OSStatus
        };

        // Virtual sink interface that can receive log messages.
        class PPXBASE_API LogSink {
        public:
            LogSink() {}
            virtual ~LogSink() {}
            virtual void OnLogMessage(const std::string& message) = 0;
        };


        class PPXBASE_API LogMessage {
        public:
            LogMessage(const char* file, int line, LoggingSeverity sev, LogErrorContext err_ctx = ERRCTX_NONE, int err = 0);

            ~LogMessage();

            static bool Loggable(LoggingSeverity sev);
            std::ostream& stream();

            static int64_t LogStartTime();

            static void LogThreads(bool on = true);

            static void LogTimestamps(bool on = true);

            // These are the available logging channels
            //  Debug: Debug console on Windows, otherwise stderr
            static void LogToDebug(LoggingSeverity min_sev);
            static LoggingSeverity GetLogToDebug();

            // Sets whether logs will be directed to stderr in debug mode.
            static void SetLogToStderr(bool log_to_stderr);

            static int GetLogToStream(LogSink* stream = nullptr);
            static void AddLogToStream(LogSink* stream, LoggingSeverity min_sev);
            static void RemoveLogToStream(LogSink* stream);

            static int GetMinLogSeverity();

        private:
            typedef std::pair<LogSink*, LoggingSeverity> StreamAndSeverity;
            typedef std::list<StreamAndSeverity> StreamList;

            // Updates min_sev_ appropriately when debug sinks change.
            static void UpdateMinLogSeverity();

            static void OutputToDebug(const std::string& msg, LoggingSeverity severity);

            // Checks the current global debug severity and if the |streams_| collection
            // is empty. If |severity| is smaller than the global severity and if the
            // |streams_| collection is empty, the LogMessage will be considered a noop
            // LogMessage.
            static bool IsNoop(LoggingSeverity severity);

            // Called from the dtor (or from a test) to append optional extra error
            // information to the log stream and a newline character.
            void FinishPrintStream();

			class Impl;
			Impl* impl_;



            PPX_DISALLOW_COPY_AND_ASSIGN(LogMessage);
        };

        // This class is used to explicitly ignore values in the conditional logging macros. 
		// This avoids compiler warnings like "value computed is not used" and "statement has no effect".
        class PPXBASE_API LogMessageVoidify {
        public:
            LogMessageVoidify() {}
            // This has to be an operator with a precedence lower than << but higher than ?:
            void operator&(std::ostream&) {}
        };




#define PPX_LOG_SEVERITY_PRECONDITION(sev)	\
	!(ppx::base::LogMessage::Loggable(sev)) ? (void) 0 : ppx::base::LogMessageVoidify() &

#define PPX_LOG(sev) \
	PPX_LOG_SEVERITY_PRECONDITION(ppx::base::sev) ppx::base::LogMessage(__FILE__, __LINE__, ppx::base::sev).stream()

#define PPX_LOG_E(sev, ctx, err, ...) \
	PPX_LOG_SEVERITY_PRECONDITION(ppx::base::sev) \
    ppx::base::LogMessage(__FILE__, __LINE__, ppx::base::sev, ppx::base:: ## ctx, err , ##__VA_ARGS__).stream()


#define PPX_LOG_ERRNO_EX(sev, err)      PPX_LOG_E(sev, ERRCTX_ERRNO, err)
#define PPX_LOG_ERRNO(sev)              PPX_LOG_ERRNO_EX(sev, errno)

#if defined(_WIN32)
#define PPX_LOG_GLE_EX(sev, err)        PPX_LOG_E(sev, ERRCTX_HRESULT, err)
#define PPX_LOG_GLE(sev)                PPX_LOG_GLE_EX(sev, GetLastError())
#endif  // _WIN32



#ifdef _WIN32
		PPXBASE_API void TraceMsgW(const wchar_t *lpFormat, ...);
		PPXBASE_API void TraceMsgA(const char *lpFormat, ...);

#if (defined UNICODE) || (defined _UNICODE)
#define TraceMsg ppx::base::TraceMsgW
#else
#define TraceMsg ppx::base::TraceMsgA
#endif
#endif


    } // namespace base
}  // namespace ppx

#endif