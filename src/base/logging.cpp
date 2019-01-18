/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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


#if defined(_WIN32)
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <strsafe.h>
    #if _MSC_VER < 1900
        #define snprintf _snprintf
    #endif
    #undef ERROR  // wingdi.h
#endif

#include <time.h>
#include <limits.h>
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <vector>

#include "base/criticalsection.h"
#include "base/logging.h"
#include "base/stringencode.h"
#include "base/timeutils.h"

namespace ppx {
    namespace base {
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
                    int overflow(int c) override {
                        return c;
                    }
                };
                static NoopStreamBuf noop_buffer;
                static std::ostream noop_stream(&noop_buffer);
                return noop_stream;
            }

            // Global lock for log subsystem, only needed to serialize access to streams_.
            CriticalSection g_log_crit;
        }  // namespace



        bool LogMessage::log_to_stderr_ = true;

        LogMessage::StreamList LogMessage::streams_;

        // Boolean options default to true (1)
        bool LogMessage::thread_ = 1;
        bool LogMessage::timestamp_ = 1;

        LogMessage::LogMessage(const char *file, int line, LoggingSeverity sev, LogErrorContext err_ctx, int err)
            : severity_(sev), is_noop_(IsNoop(sev)) {
            if (is_noop_)
                return;

            if (timestamp_) {
                print_stream_ << "[" << GetLocalTime().ToString(true) << "] ";
            }

            if (thread_) {
#ifdef _WIN32
                DWORD id = GetCurrentThreadId();
                print_stream_ << "[" << std::dec << id << "] ";
#endif
            }

            switch (sev) {
            case ppx::base::LS_SENSITIVE:
                print_stream_ << "[SENSITIVE] ";
                break;
            case ppx::base::LS_VERBOSE:
                print_stream_ << "[VERBOSE] ";
                break;
            case ppx::base::LS_INFO:
                print_stream_ << "[INFO] ";
                break;
            case ppx::base::LS_WARNING:
                print_stream_ << "[WARNING] ";
                break;
            case ppx::base::LS_ERROR:
                print_stream_ << "[ERROR] ";
                break;
            case ppx::base::LS_NONE:
            default:
                break;
            }

            if (file != nullptr)
                print_stream_ << "(" << FilenameFromPath(file) << ":" << line << "): ";

            if (err_ctx != ERRCTX_NONE) {
                std::stringstream tmp;
                char prefix[14] = { 0 };
                sprintf_s(prefix, sizeof(prefix), "[0x%08X]", err);
                tmp << prefix;

                switch (err_ctx) {
                    case ERRCTX_ERRNO:
                        tmp << " " << strerror(err);
                        break;
#ifdef _WIN32

                    case ERRCTX_HRESULT: {
                            char msgbuf[256];
                            DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

                            if (DWORD len = FormatMessageA( flags, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                            msgbuf, sizeof(msgbuf) / sizeof(msgbuf[0]), nullptr)) {
                                while ((len > 0) && isspace(static_cast<unsigned char>(msgbuf[len - 1]))) {
                                    msgbuf[--len] = 0;
                                }

                                tmp << " " << msgbuf;
                            }

                            break;
                        }

#endif  // _WIN32

                    default:
                        break;
                }

                extra_ = tmp.str();
            }
        }

        LogMessage::~LogMessage() {
            if (is_noop_)
                return;

            FinishPrintStream();

            const std::string str = print_stream_.str();

            if (severity_ >= g_dbg_sev) {
                OutputToDebug(str, severity_);
            }

            CritScope cs(&g_log_crit);

            for (auto &kv : streams_) {
                if (severity_ >= kv.second) {
                    kv.first->OnLogMessage(str);
                }
            }
        }

        std::ostream &LogMessage::stream() {
            return is_noop_ ? GetNoopStream() : print_stream_;
        }

        bool LogMessage::Loggable(LoggingSeverity sev) {
            return sev >= g_min_sev;
        }

        int LogMessage::GetMinLogSeverity() {
            return g_min_sev;
        }

        LoggingSeverity LogMessage::GetLogToDebug() {
            return g_dbg_sev;
        }

        int64_t LogMessage::LogStartTime() {
            static const int64_t g_start = GetTimeStamp() / 1000;
            return g_start;
        }

        void LogMessage::LogThreads(bool on) {
            thread_ = on;
        }

        void LogMessage::LogTimestamps(bool on) {
            timestamp_ = on;
        }

        void LogMessage::LogToDebug(LoggingSeverity min_sev) {
            g_dbg_sev = min_sev;
            CritScope cs(&g_log_crit);
            UpdateMinLogSeverity();
        }

        void LogMessage::SetLogToStderr(bool log_to_stderr) {
            log_to_stderr_ = log_to_stderr;
        }

        int LogMessage::GetLogToStream(LogSink *stream) {
            CritScope cs(&g_log_crit);
            LoggingSeverity sev = LS_NONE;

            for (auto &kv : streams_) {
                if (!stream || stream == kv.first) {
                    sev = std::min(sev, kv.second);
                }
            }

            return sev;
        }

        void LogMessage::AddLogToStream(LogSink *stream, LoggingSeverity min_sev) {
            CritScope cs(&g_log_crit);
            streams_.push_back(std::make_pair(stream, min_sev));
            UpdateMinLogSeverity();
        }

        void LogMessage::RemoveLogToStream(LogSink *stream) {
            CritScope cs(&g_log_crit);

            for (StreamList::iterator it = streams_.begin(); it != streams_.end(); ++it) {
                if (stream == it->first) {
                    streams_.erase(it);
                    break;
                }
            }

            UpdateMinLogSeverity();
        }

        void LogMessage::UpdateMinLogSeverity() {
            LoggingSeverity min_sev = g_dbg_sev;

            for (auto &kv : streams_) {
                min_sev = std::min(g_dbg_sev, kv.second);
            }

            g_min_sev = min_sev;
        }

        void LogMessage::OutputToDebug(const std::string &str, LoggingSeverity severity) {
            bool log_to_stderr = log_to_stderr_;
#if defined(_WIN32)
            OutputDebugStringA(str.c_str());

            if (log_to_stderr) {
                if (HANDLE error_handle = ::GetStdHandle(STD_ERROR_HANDLE)) {
                    log_to_stderr = false;
                    DWORD written = 0;
                    ::WriteFile(error_handle, str.data(), static_cast<DWORD>(str.size()), &written, 0);
                }
            }

#endif  // Win32

            if (log_to_stderr) {
                fprintf(stderr, "%s", str.c_str());
                fflush(stderr);
            }
        }

        // static
        bool LogMessage::IsNoop(LoggingSeverity severity) {
            if (severity >= g_dbg_sev)
                return false;

            CritScope cs(&g_log_crit);
            return streams_.size() == 0;
        }

        void LogMessage::FinishPrintStream() {
            if (is_noop_)
                return;

            if (!extra_.empty())
                print_stream_ << " : " << extra_;

            print_stream_ << std::endl;
        }

        //////////////////////////////////////////////////////////////////////

#ifdef _WIN32

        void TraceMsgW(const wchar_t *lpFormat, ...) {
            if (!lpFormat)
                return;

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
        }

        void TraceMsgA(const char *lpFormat, ...) {
            if (!lpFormat)
                return;

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
        }
#endif
    } // namespace base
}  // namespace ppx
