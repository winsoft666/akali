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

#include "net/file_transfer_base.h"
#include "base/timeutils.h"
#include <inttypes.h>
#include <algorithm>

namespace ppx {
    namespace net {
        FileTransferBase::FileTransferBase() :
            thread_num_(1),
            actual_thread_num_(0),
            status_(Ready),
            finished_thread_num_(0),
            start_time_(0L),
            tmp_fileext_(".download"),
            progress_interval_(0L) {
        }

        FileTransferBase::~FileTransferBase() {

        }

        void FileTransferBase::SetThreadNum(size_t thread_num) {
            if (status_ != Progress && thread_num > 0)
                thread_num_ = thread_num;
        }

        size_t FileTransferBase::GetThreadNum() const {
            return thread_num_;
        }

        void FileTransferBase::SetUrl(const std::string &url) {
            if (status_ != Progress)
                url_ = url;
        }

        std::string FileTransferBase::GetUrl() const {
            return url_;
        }

        void FileTransferBase::SetFileDir(const std::string &filedir) {
            if (status_ == Progress)
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
            if (status_ == Progress)
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
            if (status_ == Progress)
                return;
            file_md5_ = md5;
            std::transform(file_md5_.begin(), file_md5_.end(), file_md5_.begin(), ::tolower);
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
            if (status_ != Progress) {
                ca_path_ = caPath;
            }
        }

        std::string FileTransferBase::GetCAPath() const {
            return ca_path_;
        }

        void FileTransferBase::SetStatusCallback(const StatusFunctor &functor) {
            if (status_ == Progress)
                return;

            status_cb_ = functor;
        }

        void FileTransferBase::SetProgressCallback(const ProgressFunctor &functor) {
            if (status_ == Progress)
                return;

            progress_cb_ = functor;
        }

        void FileTransferBase::SetProgressInterval(int64_t ms) {
            if (status_ == Progress)
                return;

            progress_interval_ = ms;
        }

        int64_t FileTransferBase::GetProgressInterval() const {
            return progress_interval_;
        }
    }
}