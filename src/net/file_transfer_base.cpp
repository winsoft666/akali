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
#include "base/safe_release_macro.h"
#include "base/string_helper.h"
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

        void FileTransferBase::SetUrl(const std::string &url) {
            if (base_impl_->status_ != Progress)
				url_ = url;
        }

        std::string FileTransferBase::GetUrl() const {
            return url_;
        }

        void FileTransferBase::SetFileDir(const std::string &filedir) {
            if (base_impl_->status_ == Progress)
                return;

			file_dir_ = filedir;

            if (file_dir_.length() > 0) {
                if (file_dir_[file_dir_.length() - 1] != '\\' || file_dir_[file_dir_.length() - 1] != '/') {
#ifdef _WIN32
					file_dir_ += "\\";
#else
					file_dir_ += "/";
#endif
                }
            }
        }

        std::string FileTransferBase::GetFileDir() const {
            return file_dir_;
        }

        void FileTransferBase::SetFileName(const std::string &filename) {
            if (base_impl_->status_ == Progress)
                return;

			file_name_ = filename;
        }

        std::string FileTransferBase::GetFileName() const {
            return file_name_;
        }

        void FileTransferBase::SetFileExt(const std::string &ext) {
			file_ext_ = ext;
        }

        std::string FileTransferBase::GetFileExt() const {
            return file_ext_;
        }

        void FileTransferBase::SetFileMd5(const std::string &md5) {
            if (base_impl_->status_ == Progress)
                return;
			file_md5_ = base::StringToLower(md5);
        }

        std::string FileTransferBase::GetFileMd5() const {
            return file_md5_;
        }

        void FileTransferBase::GenerateTmpFileName(int64_t filesize) {
            if (file_name_.length() == 0) {
                // "must first set file path"
                return;
            }

            if (file_md5_.length() == 0) {
                char buf[50] = { 0 };
                sprintf_s(buf, "%" PRId64 "", filesize);
				tmp_filename_ = file_name_ + "_" + std::string(buf);
            }
            else {
				tmp_filename_ = file_name_ + "_" + file_md5_;
            }
        }

        std::string FileTransferBase::GetTmpFileName() const {
            return tmp_filename_;
        }

        std::string FileTransferBase::GetTmpFileExt() const {
            return tmp_fileext_;
        }

        void FileTransferBase::SetCAPath(const std::string &caPath) {
            if (base_impl_->status_ != Progress) {
				ca_path_ = caPath;
            }
        }

        std::string FileTransferBase::GetCAPath() const {
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