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

#ifndef PPX_NET_FILE_DOWNLOAD_H_
#define PPX_NET_FILE_DOWNLOAD_H_

#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include "net/file_transfer_base.h"
#include "ppx_export.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

// multi-thread download and continue from the point of interruption.
namespace ppx {
    namespace net {
        typedef struct _FileDownloadHeader FileDownloadHeader;
        class PPX_API FileDownload : public FileTransferBase {
        public:
            class FileDownloadWork;
            FileDownload();
            virtual ~FileDownload();

            void TransferProgress(int64_t &total, int64_t& transfered) override;

            bool Start() override;
            void Stop() override;

            void SetInterruptionResuming(bool b);
        protected:
            bool QueryFileSize();
            void WorkLoop();
            void ProgressChange(size_t index);
            bool ReadDownloadHeader();
            bool WriteDownloadHeader();
            bool UpdateDownloadHeader();
            size_t GetDownloadHeaderSize();
            bool CopyDataToFile();
            bool DeleteTmpFile();
            void LastCurlClear();
        private:
            class PrivateData;
            PrivateData* data_;
        };
    }
}

#endif // !PPX_NET_FILE_DOWNLOAD_H_