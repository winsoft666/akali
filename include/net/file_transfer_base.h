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
#include "base/string.h"

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
            typedef std::function< void(base::StringUTF8 /*filename*/, Status, base::StringUTF8 /*reason*/, int64_t /*used_millsec*/) > StatusFunctor;
            typedef std::function< void(base::StringUTF8 /*filename*/, int64_t /*total*/, int64_t /* download/upload */) > ProgressFunctor;

            FileTransferBase();
            virtual ~FileTransferBase();

            virtual void SetThreadNum(size_t thread_num);
            virtual size_t GetThreadNum() const;

            virtual void SetUrl(const base::StringUTF8 &url);
            virtual base::StringUTF8 GetUrl() const;

            virtual void SetFileDir(const base::StringUTF8 &filedir);
            virtual base::StringUTF8 GetFileDir() const;

            virtual void SetFileName(const base::StringUTF8 &filename);
            virtual base::StringUTF8 GetFileName() const;

            virtual void SetFileExt(const base::StringUTF8 &ext);
            virtual base::StringUTF8 GetFileExt() const;

            virtual void SetFileMd5(const base::StringUTF8 &md5);
            virtual base::StringUTF8 GetFileMd5() const;

            virtual void GenerateTmpFileName(int64_t filesize);
            virtual base::StringUTF8 GetTmpFileName() const;
            virtual base::StringUTF8 GetTmpFileExt() const;

            virtual void SetCAPath(const base::StringUTF8 &caPath);
            virtual base::StringUTF8 GetCAPath() const;

            virtual void SetStatusCallback(const StatusFunctor &functor);
            virtual void SetProgressCallback(const ProgressFunctor &functor);

            virtual void SetProgressInterval(int64_t ms);
            virtual int64_t GetProgressInterval() const;

            virtual void TransferProgress(int64_t &total, int64_t& transfered) = 0;

            virtual bool Start() = 0;
            virtual void Stop() = 0;

			Status GetStatus() const;
			void SetStatus(Status s);
			StatusFunctor GetStatusFunctor() const;
			ProgressFunctor GetProgressFunctor() const;
        protected:
			class FileTransferBaseImpl;
			FileTransferBaseImpl* base_impl_;

			size_t thread_num_;
			size_t actual_thread_num_;
			int64_t start_time_;
			base::StringUTF8 ca_path_;
			base::StringUTF8 url_;
			base::StringUTF8 file_dir_;
			base::StringUTF8 file_name_;
			base::StringUTF8 file_ext_;
			base::StringUTF8 tmp_filename_;
			base::StringUTF8 tmp_fileext_;
			base::StringUTF8 file_md5_;
			int64_t progress_interval_;
        };
    }
}
#endif // !PPX_NET_FILE_TRANSFER_BASE_H__
