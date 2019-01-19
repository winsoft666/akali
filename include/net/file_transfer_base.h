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
            typedef std::function< void(base::String /*filename*/, Status, base::String /*reason*/, int64_t /*used_millsec*/) > StatusFunctor;
            typedef std::function< void(base::String /*filename*/, int64_t /*total*/, int64_t /* download/upload */) > ProgressFunctor;

            FileTransferBase();
            virtual ~FileTransferBase();

            virtual void SetThreadNum(size_t thread_num);
            virtual size_t GetThreadNum() const;

            virtual void SetUrl(const base::String &url);
            virtual base::String GetUrl() const;

            virtual void SetFileDir(const base::String &filedir);
            virtual base::String GetFileDir() const;

            virtual void SetFileName(const base::String &filename);
            virtual base::String GetFileName() const;

            virtual void SetFileExt(const base::String &ext);
            virtual base::String GetFileExt() const;

            virtual void SetFileMd5(const base::String &md5);
            virtual base::String GetFileMd5() const;

            virtual void GenerateTmpFileName(int64_t filesize);
            virtual base::String GetTmpFileName() const;
            virtual base::String GetTmpFileExt() const;

            virtual void SetCAPath(const base::String &caPath);
            virtual base::String GetCAPath() const;

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
			base::String ca_path_;
			base::String url_;
			base::String file_dir_;
			base::String file_name_;
			base::String file_ext_;
			base::String tmp_filename_;
			base::String tmp_fileext_;
			base::String file_md5_;
			int64_t progress_interval_;
        };
    }
}
#endif // !PPX_NET_FILE_TRANSFER_BASE_H__
