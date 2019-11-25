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


#include "ppxbase/logging_file_sink_win.h"
#ifdef _WIN32
#include <strsafe.h>
#pragma warning(disable:4995)
#pragma warning(disable:4996)

namespace ppx {
    namespace base {

        const int STLOG_MAX_FILE_NUM = 3;
        const long STLOG_FILE_MAX_SIZE_IN_BYTE = 20971520; // 20MB
        const int STLOG_CHECK_PRECISION = 8000;
        const int STLOG_MAXINDENT = 100;

        LoggingFileSinkWin::LoggingFileSinkWin()
            : m_bIsStarted(FALSE), m_lWriteCount(0), m_iLogFileIndex(0) {
            ::InitializeCriticalSection(&m_Crit4File);
            ::InitializeCriticalSection(&m_Crit4Toggle);
            m_dwTLSIndex = TlsAlloc();
        }

        LoggingFileSinkWin::~LoggingFileSinkWin() {
            if (m_bIsStarted)
                Stop();

            ::DeleteCriticalSection(&m_Crit4File);
            ::DeleteCriticalSection(&m_Crit4Toggle);
            TlsFree(m_dwTLSIndex);
        }

        void LoggingFileSinkWin::OnLogMessage(const std::string &message) {
            LoggingFileSinkWin *sink = GetSink();

            if (sink) {
                sink->Write("%s", message.c_str());
            }
        }

        LoggingFileSinkWin *LoggingFileSinkWin::GetSink() {
            static LoggingFileSinkWin LogFile;

            if (!LogFile.m_bIsStarted) {
                LogFile.Start();
            } else {
                LogFile.TryToggleFile();
            }

            return &LogFile;
        }

        void LoggingFileSinkWin::WriteData(LPCVOID pBytes, DWORD dwSize) {
            if (m_hFile == INVALID_HANDLE_VALUE)         return;

            Write("*** Binary data (%d bytes) ***", dwSize);
            DWORD dwWrittenBytes = 0;
            DWORD dwTmp = 0;
            ::SetFilePointer(m_hFile, 0, 0, FILE_END);
            ::WriteFile(m_hFile, pBytes, dwSize, &dwWrittenBytes, NULL);

            ::WriteFile(m_hFile, "\r\n", 2, &dwTmp, NULL);
            Write("*** End binary data (written %d bytes) ***", dwWrittenBytes);
            ::FlushFileBuffers(m_hFile);
        }

        void LoggingFileSinkWin::Write(LPCSTR szFormat, ...) {
            const unsigned int iBufIncrCount = 512;

            if (m_bIsStarted) {
                unsigned int iMsgBufCount = 0;
                char *pMsgBuffer = NULL;
                va_list args;
                va_start(args, szFormat);

                HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

                while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
                    iMsgBufCount += iBufIncrCount;

                    if (pMsgBuffer) {
                        free(pMsgBuffer);
                        pMsgBuffer = NULL;
                    }

                    pMsgBuffer = (char *)malloc(iMsgBufCount * sizeof(char));

                    if (!pMsgBuffer) {
                        break;
                    }

                    hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, szFormat, args);
                }

                va_end(args);

                if (hr != S_OK) {
                    if (pMsgBuffer) {
                        free(pMsgBuffer);
                        pMsgBuffer = NULL;
                    }

                    return;
                }

                ::EnterCriticalSection(&m_Crit4File);
                char szIndent[STLOG_MAXINDENT];
                memset(szIndent, ' ', sizeof(szIndent));

                int nIndent = (int)TlsGetValue(m_dwTLSIndex);
                *(szIndent + nIndent) = 0;

                char *pOutputBuf = NULL;
                unsigned int iOutputBufCount = 0;
                hr = STRSAFE_E_INSUFFICIENT_BUFFER;

                while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
                    iOutputBufCount += iBufIncrCount;

                    if (pOutputBuf) {
                        free(pOutputBuf);
                        pOutputBuf = NULL;
                    }

                    pOutputBuf = (char *)malloc(iOutputBufCount * sizeof(char));

                    if (!pOutputBuf) {
                        break;
                    }

                    hr = StringCchPrintfA(pOutputBuf, iOutputBufCount, "%s%s", szIndent, pMsgBuffer);
                }

                if (pMsgBuffer)                  free(pMsgBuffer);

                if (hr != S_OK) {
                    if (pOutputBuf)              free(pOutputBuf);

                    ::LeaveCriticalSection(&m_Crit4File);
                    return;
                }

                DWORD dwWrittenBytes;
                size_t nBytes = strlen(pOutputBuf);
                ::SetFilePointer(m_hFile, 0, 0, FILE_END);
                ::WriteFile(m_hFile, pOutputBuf, nBytes, &dwWrittenBytes, NULL);
                ::FlushFileBuffers(m_hFile);

                if (pOutputBuf)                           free(pOutputBuf);

                ::LeaveCriticalSection(&m_Crit4File);
            }
        }

        void LoggingFileSinkWin::Start() {
            TlsSetValue(m_dwTLSIndex, 0);

            TCHAR wszFileName[MAX_PATH] = { 0 };
            GetLogFileName(wszFileName);
            Start(wszFileName);
        }

        void LoggingFileSinkWin::Start(LPCTSTR szFilePath) {
            if (m_bIsStarted) return;

            ::EnterCriticalSection(&m_Crit4File);
            m_hFile = ::CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                   OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

            if (m_hFile != INVALID_HANDLE_VALUE) {
                ::SetFilePointer(m_hFile, 0, 0, FILE_END);
                m_bIsStarted = TRUE;
                char szExecutable[MAX_PATH] = { 0 };
                GetModuleFileNameA(NULL, szExecutable, MAX_PATH);
                DWORD dwProcID = GetCurrentProcessId();
                SYSTEMTIME st;
                GetLocalTime(&st);
                Write("\r\n\r\n================================================================================\r\n");
                Write("Log is started on %04u-%02u-%02u, at %02u:%02u:%02u:%03u, executable: %s (PID: 0x%08x), compile time : %s %s.\r\n",
                      st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szExecutable, dwProcID,
                      __DATE__, __TIME__);
            }

            ::LeaveCriticalSection(&m_Crit4File);
        }

        void LoggingFileSinkWin::Stop() {
            ::EnterCriticalSection(&m_Crit4File);

            if (m_bIsStarted) {
                Write("Log finished.\r\n");
                ::CloseHandle(m_hFile);
                m_hFile = INVALID_HANDLE_VALUE;
                m_bIsStarted = FALSE;
            }

            ::LeaveCriticalSection(&m_Crit4File);
        }

        void LoggingFileSinkWin::TryToggleFile() {
            InterlockedExchangeAdd(&m_lWriteCount, 1);

            if (InterlockedExchangeAdd(&m_lWriteCount, 0) < STLOG_CHECK_PRECISION) {
                return;
            }

            ::EnterCriticalSection(&m_Crit4Toggle);

            if (InterlockedExchangeAdd(&m_lWriteCount, 0) < STLOG_CHECK_PRECISION) {
                ::LeaveCriticalSection(&m_Crit4Toggle);
                return;
            }

            ::EnterCriticalSection(&m_Crit4File);

            bool bToggle = false;

            if (m_hFile) {
                LARGE_INTEGER li;
                GetFileSizeEx(m_hFile, &li);

                if (li.QuadPart > STLOG_FILE_MAX_SIZE_IN_BYTE) {
                    bToggle = true;
                }
            }

            if (bToggle) {
                if (m_hFile != INVALID_HANDLE_VALUE) {
                    ::FlushFileBuffers(m_hFile);
                    ::CloseHandle(m_hFile);
                    m_hFile = INVALID_HANDLE_VALUE;
                }

                TCHAR wszExecutableFilePath[MAX_PATH] = { 0 };
                ::GetModuleFileName(NULL, wszExecutableFilePath, MAX_PATH);
                TCHAR *wszLastDot = ::_tcsrchr(wszExecutableFilePath, _T('.'));

                int nNowNumber = m_iLogFileIndex + 1;

                if (nNowNumber > STLOG_MAX_FILE_NUM) {
                    m_iLogFileIndex = 1;
                    ::_tcscpy(wszLastDot, TEXT("_log01.txt"));
                } else {
                    m_iLogFileIndex = nNowNumber;
                    StringCchPrintf(wszLastDot, MAX_PATH, _T("_log%02d.txt"), nNowNumber);
                }

                DeleteFile(wszExecutableFilePath);
                m_hFile = ::CreateFile(wszExecutableFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                       OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            }

            ::LeaveCriticalSection(&m_Crit4File);
            InterlockedExchange(&m_lWriteCount, 0);
            ::LeaveCriticalSection(&m_Crit4Toggle);
        }

        BOOL LoggingFileSinkWin::GetTimeString(LPSTR szString, int nStringSize) {
            SYSTEMTIME st;
            GetLocalTime(&st);
            HRESULT hr = StringCchPrintfA(szString, nStringSize, "%04d/%02d/%02d %02u:%02u:%02u:%03u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
            return (hr == S_OK || hr == STRSAFE_E_INSUFFICIENT_BUFFER);
        }

        DWORD LoggingFileSinkWin::GetIndent() {
            return (DWORD)TlsGetValue(m_dwTLSIndex);
        }

        void LoggingFileSinkWin::SetIndent(DWORD dwIndent) {
            TlsSetValue(m_dwTLSIndex, (LPVOID)dwIndent);
        }

        void LoggingFileSinkWin::IncrIndent() {
            DWORD dwIndent = (DWORD)TlsGetValue(m_dwTLSIndex);
            dwIndent += 2;
            TlsSetValue(m_dwTLSIndex, (LPVOID)dwIndent);

        }

        void LoggingFileSinkWin::DecrIndent() {
            DWORD dwIndent = (DWORD)TlsGetValue(m_dwTLSIndex);
            dwIndent -= 2;
            TlsSetValue(m_dwTLSIndex, (LPVOID)dwIndent);
        }

        void LoggingFileSinkWin::GetLogFileName(LPTSTR szFileName) {
            TCHAR wszExecutableFilePath[MAX_PATH] = { 0 };
            ::GetModuleFileName(NULL, wszExecutableFilePath, MAX_PATH);
            TCHAR *wszExecutableFileName = wszExecutableFilePath;
            TCHAR *wszLastDot = ::_tcsrchr(wszExecutableFileName, _T('.'));

            int nNowMaxNumber = 1;
            TCHAR wszTemp[MAX_PATH];

            for (;; nNowMaxNumber++) {
                StringCchPrintf(wszTemp, MAX_PATH, _T("_log%02d.txt"), nNowMaxNumber);
                ::_tcscpy(wszLastDot, wszTemp);

                if (0xFFFFFFFF == ::GetFileAttributes(wszExecutableFileName)) {
                    nNowMaxNumber--;
                    break;
                }
            }

            int nNowNumber = 0;

            if (nNowMaxNumber != 0) {
                StringCchPrintf(wszTemp, MAX_PATH, _T("_log%02d.txt"), nNowMaxNumber);
                ::_tcscpy(wszLastDot, wszTemp);
                WIN32_FILE_ATTRIBUTE_DATA fad;
                memset(&fad, 0, sizeof(fad));

                if (::GetFileAttributesEx(wszExecutableFileName, GetFileExInfoStandard, &fad)) {
                    ULONGLONG llFileSize = fad.nFileSizeHigh;
                    llFileSize <<= sizeof(DWORD) * 8;
                    llFileSize += fad.nFileSizeLow;

                    if (llFileSize < STLOG_FILE_MAX_SIZE_IN_BYTE) {
                        nNowNumber = nNowMaxNumber;
                    } else {
                        nNowNumber = nNowMaxNumber + 1;
                    }
                }
            } else {
                nNowNumber = 1;
            }

            if (nNowNumber > STLOG_MAX_FILE_NUM) {
                m_iLogFileIndex = 1;
                ::_tcscpy(wszLastDot, TEXT("_log01.txt"));
                DeleteFile(wszExecutableFileName);
            } else {
                m_iLogFileIndex = nNowNumber;
                StringCchPrintf(wszTemp, MAX_PATH, _T("_log%02d.txt"), nNowNumber);
                ::_tcscpy(wszLastDot, wszTemp);
            }

            ::_tcscpy(szFileName, wszExecutableFileName);
        }
    }
}

#endif