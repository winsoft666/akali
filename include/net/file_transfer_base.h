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

#ifndef PPX_NET_FILE_TRANSFER_BASE_H__
#define PPX_NET_FILE_TRANSFER_BASE_H__

#include <string>
#include <memory>
#include <functional>
#include <atomic>
#include "ppx_export.h"

namespace ppx {
    namespace net {
        class PPX_API FileTransferBase {
        public:
            typedef enum {
                Ready,
                Progress,
                Finished,
                Failed
            }Status;
            typedef std::function< void(std::string /*filename*/, Status, std::string /*reason*/, int64_t /*used_millsec*/) > StatusFunctor;
            typedef std::function< void(std::string /*filename*/, int64_t /*total*/, int64_t /* download/upload */) > ProgressFunctor;

            FileTransferBase();
            virtual ~FileTransferBase();

            virtual void SetThreadNum(size_t thread_num);
            virtual size_t GetThreadNum() const;

            virtual void SetUrl(const std::string &url);
            virtual std::string GetUrl() const;

            virtual void SetFileDir(const std::string &filedir);
            virtual std::string GetFileDir() const;

            virtual void SetFileName(const std::string &filename);
            virtual std::string GetFileName() const;

            virtual void SetFileExt(const std::string &ext);
            virtual std::string GetFileExt() const;

            virtual void SetFileMd5(const std::string &md5);
            virtual std::string GetFileMd5() const;

            virtual void GenerateTmpFileName(int64_t filesize);
            virtual std::string GetTmpFileName() const;
            virtual std::string GetTmpFileExt() const;

            virtual void SetCAPath(const std::string &caPath);
            virtual std::string GetCAPath() const;

            virtual void SetStatusCallback(const StatusFunctor &functor);
            virtual void SetProgressCallback(const ProgressFunctor &functor);

            virtual void SetProgressInterval(int64_t ms);
            virtual int64_t GetProgressInterval() const;

            virtual void TransferProgress(int64_t &total, int64_t& transfered) = 0;

            virtual bool Start() = 0;
            virtual void Stop() = 0;

        protected:
            size_t thread_num_;
            size_t actual_thread_num_;
            int64_t start_time_;
            std::string ca_path_;
            std::string url_;
            std::string file_dir_;
            std::string file_name_;
            std::string file_ext_;
            std::string tmp_filename_;
            std::string tmp_fileext_;
            std::string file_md5_;
            std::atomic<Status> status_;
            int64_t progress_interval_;
            StatusFunctor status_cb_;
            ProgressFunctor progress_cb_;
            std::atomic<size_t> finished_thread_num_;
        };
    }
}
#endif // !PPX_NET_FILE_TRANSFER_BASE_H__
