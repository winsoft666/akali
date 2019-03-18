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

#include "net/file_transfer_base.h"
#include "base/timeutils.h"
#include "base/string.h"
#include "base/safe_release_macro.h"
#include <inttypes.h>
#include <algorithm>

namespace ppx {
    namespace net {
		class FileTransferBase::FileTransferBaseImpl {
		public:
			FileTransferBaseImpl() :
				status_(FileTransferBase::Ready),
				finished_thread_num_(0) {

			}

			~FileTransferBaseImpl() {
			}

			
			std::atomic<FileTransferBase::Status> status_;
			FileTransferBase::StatusFunctor status_cb_;
			FileTransferBase::ProgressFunctor progress_cb_;
			std::atomic<size_t> finished_thread_num_;
		};

		FileTransferBase::FileTransferBase() :
			thread_num_(1),
			actual_thread_num_(0),
			start_time_(0L),
			tmp_fileext_(".download"),
			progress_interval_(0L) {
			base_impl_ = new FileTransferBaseImpl();
        }

        FileTransferBase::~FileTransferBase() {
			SAFE_DELETE(base_impl_);
        }

        void FileTransferBase::SetThreadNum(size_t thread_num) {
            if (base_impl_->status_ != Progress && thread_num > 0)
				thread_num_ = thread_num;
        }

        size_t FileTransferBase::GetThreadNum() const {
            return thread_num_;
        }

        void FileTransferBase::SetUrl(const base::StringUTF8 &url) {
            if (base_impl_->status_ != Progress)
				url_ = url;
        }

        base::StringUTF8 FileTransferBase::GetUrl() const {
            return url_;
        }

        void FileTransferBase::SetFileDir(const base::StringUTF8 &filedir) {
            if (base_impl_->status_ == Progress)
                return;

			file_dir_ = filedir;

            if (file_dir_.GetLength() > 0) {
                if (file_dir_[file_dir_.GetLength() - 1] != '\\' || file_dir_[file_dir_.GetLength() - 1] != '/') {
#ifdef _WIN32
					file_dir_ += "\\";
#else
					file_dir_ += "/";
#endif
                }
            }
        }

        base::StringUTF8 FileTransferBase::GetFileDir() const {
            return file_dir_;
        }

        void FileTransferBase::SetFileName(const base::StringUTF8 &filename) {
            if (base_impl_->status_ == Progress)
                return;

			file_name_ = filename;
        }

        base::StringUTF8 FileTransferBase::GetFileName() const {
            return file_name_;
        }

        void FileTransferBase::SetFileExt(const base::StringUTF8 &ext) {
			file_ext_ = ext;
        }

        base::StringUTF8 FileTransferBase::GetFileExt() const {
            return file_ext_;
        }

        void FileTransferBase::SetFileMd5(const base::StringUTF8 &md5) {
            if (base_impl_->status_ == Progress)
                return;
			file_md5_ = md5;
			file_md5_.MakeLower();
        }

        base::StringUTF8 FileTransferBase::GetFileMd5() const {
            return file_md5_;
        }

        void FileTransferBase::GenerateTmpFileName(int64_t filesize) {
            if (file_name_.GetLength() == 0) {
                // "must first set file path"
                return;
            }

            if (file_md5_.GetLength() == 0) {
                char buf[50] = { 0 };
                sprintf_s(buf, "%" PRId64 "", filesize);
				tmp_filename_ = file_name_ + "_" + base::StringUTF8(buf);
            }
            else {
				tmp_filename_ = file_name_ + "_" + file_md5_;
            }
        }

        base::StringUTF8 FileTransferBase::GetTmpFileName() const {
            return tmp_filename_;
        }

        base::StringUTF8 FileTransferBase::GetTmpFileExt() const {
            return tmp_fileext_;
        }

        void FileTransferBase::SetCAPath(const base::StringUTF8 &caPath) {
            if (base_impl_->status_ != Progress) {
				ca_path_ = caPath;
            }
        }

        base::StringUTF8 FileTransferBase::GetCAPath() const {
            return ca_path_;
        }

        void FileTransferBase::SetStatusCallback(const StatusFunctor &functor) {
            if (base_impl_->status_ == Progress)
                return;

			base_impl_->status_cb_ = functor;
        }

        void FileTransferBase::SetProgressCallback(const ProgressFunctor &functor) {
            if (base_impl_->status_ == Progress)
                return;

			base_impl_->progress_cb_ = functor;
        }

        void FileTransferBase::SetProgressInterval(int64_t ms) {
            if (base_impl_->status_ == Progress)
                return;

			progress_interval_ = ms;
        }

        int64_t FileTransferBase::GetProgressInterval() const {
            return progress_interval_;
        }

		FileTransferBase::Status FileTransferBase::GetStatus() const {
			return base_impl_->status_;
		}

		void FileTransferBase::SetStatus(Status s) {
			base_impl_->status_ = s;
		}

		FileTransferBase::StatusFunctor FileTransferBase::GetStatusFunctor() const {
			return base_impl_->status_cb_;
		}

		FileTransferBase::ProgressFunctor FileTransferBase::GetProgressFunctor() const {
			return base_impl_->progress_cb_;
		}

	}
}