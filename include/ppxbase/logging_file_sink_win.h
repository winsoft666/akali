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

#ifndef PPX_BASE_LOGGING_FILE_SINK_WIN_H__
#define PPX_BASE_LOGGING_FILE_SINK_WIN_H__
#pragma once

#include <tchar.h>
#include <wtypes.h>
#include "ppxbase/logging.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API LoggingFileSinkWin : public LogSink {
          public:
            static LoggingFileSinkWin *GetSink();
          protected:
            LoggingFileSinkWin();
            virtual ~LoggingFileSinkWin();
            void OnLogMessage(const std::string &message) override;

            void WriteData(LPCVOID pBytes, DWORD dwSize);
            void Write(LPCSTR szFormat, ...);
            void Start();
            void Start(LPCTSTR szFilePath);
            void Stop();
            void TryToggleFile();
            BOOL GetTimeString(LPSTR szString, int nStringSize);
            DWORD GetIndent();
            void SetIndent(DWORD dwIndent);
            void IncrIndent();
            void DecrIndent();
            void GetLogFileName(LPTSTR szFileName);
          private:
            HANDLE m_hFile;
            CRITICAL_SECTION m_Crit4File;
            CRITICAL_SECTION m_Crit4Toggle;
            BOOL m_bIsStarted;
            DWORD m_dwTLSIndex;
            int m_iLogFileIndex;
            LONG m_lWriteCount;
        };
    }
}

#endif
