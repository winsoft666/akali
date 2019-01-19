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

#include "net/file_download.h"
#include <algorithm>
#include <atomic>
#include <io.h>
#include "base/safe_release_macro.h"
#include "base/logging.h"
#include "base/timeutils.h"
#include "base/md5.h"
#include "net/internal/scoped_curl.h"
#include "curl\curl.h"
#include "ppx_export.h"

namespace ppx {
    namespace net {
#define FILE_DOWNLOAD_HEADER_SIGN "FSINFO - by jeffery."
#define FILE_DOWNLAD_HEADER_SIZE 20
#pragma pack(1)
        typedef struct _FileSplitInfo {
            int64_t start_pos;
            int64_t end_pos;
            int64_t last_downloaded; // TODO: don't need write to temp file.
            int64_t downloaded;

            _FileSplitInfo() {
                start_pos = 0L;
                end_pos = 0L;
                downloaded = 0L;
                last_downloaded = 0L;
            }
        } FileSplitInfo;

        typedef struct _FileDownloadHeader {
            char flag[FILE_DOWNLAD_HEADER_SIZE]; // FILE_DOWNLOAD_HEADER_SIGN
            size_t split_num;
            std::unique_ptr<FileSplitInfo[]> split_infos;
            _FileDownloadHeader() {
                split_num = 0;
                memset(flag, 0, FILE_DOWNLAD_HEADER_SIZE);
            }
        } FileDownloadHeader;
#pragma pack()

		class FileDownload::PrivateData {
		public:
			PrivateData() :
				is_new_download_(false),
				interruption_resuming_(false),
				file_size_(0L),
				download_header_size_(0),
				last_time_downloaded_(0L),
				actual_interruption_resuming_(false),
#ifdef _WIN32
				file_(INVALID_HANDLE_VALUE),
#else
				file_(0),
#endif
				stop_(false) {
				multi_ = nullptr;
			}

		public:
			CURLM* multi_;
			std::vector<CURL*> easys_;

			bool interruption_resuming_;
			bool actual_interruption_resuming_;
			bool is_new_download_;
			int64_t file_size_;
#ifdef _WIN32
			HANDLE file_;
#else
			int file_;
#endif
			std::mutex file_write_mutex_;
			std::atomic_bool stop_;
			std::thread work_thread_;
			std::unique_ptr<FileDownloadHeader> download_header_;
			size_t download_header_size_;
			int64_t last_time_downloaded_;

			std::vector<FileDownloadWork*> works_;
		};

        class FileDownload::FileDownloadWork {
        public:
            FileDownloadWork(size_t index, FileDownload *parent, int64_t start_pos, int64_t end_pos) :
                work_index_(index),
                parent_(parent),
                start_pos_(start_pos),
                end_pos_(end_pos),
                written_(0L),
                downloaded_(0L),
                total_to_download_(0L),
                buffer_size_(102400),
                buffer_used_(0),
                buffer_(nullptr) {
                buffer_ = (char *)malloc(buffer_size_);
                assert(buffer_);
                assert(parent_);
            }

            ~FileDownloadWork() {
                SAFE_FREE(buffer_);
            }

            int64_t downloaded() {
                return downloaded_;
            }

            int64_t totalToDownload() {
                return total_to_download_;
            }

        public:
            size_t writeData(char *data, size_t size, size_t nmemb) {
                size_t avaliable = size * nmemb;

                if (buffer_used_ + avaliable > buffer_size_) {
                    if (writeAllBuffer() <= 0) {
                        PPX_LOG(LS_WARNING) << "#" << work_index_ << " write file failed";
                        return 0;
                    }

                    if (avaliable > buffer_size_) {
                        SAFE_FREE(buffer_);
                        buffer_size_ = (size_t)(avaliable * 1.5);
                        buffer_ = (char *)malloc(buffer_size_);
                        assert(buffer_);

                        PPX_LOG(LS_INFO) << "#" << work_index_ << " buffer size: " << buffer_size_;
                    }
                }

                if (buffer_) {
                    memcpy(buffer_ + buffer_used_, data, avaliable);
                    buffer_used_ += avaliable;
                }

                return (avaliable);
            }

            int progressFunc(int64_t totalToDownload, int64_t nowDownloaded, int64_t totalToUpLoad, int64_t nowUpLoaded) {
                downloaded_ = (int64_t)nowDownloaded;
                total_to_download_ = (int64_t)totalToDownload;

                //PPX_LOG(LS_INFO) << work_index_ << ":" << downloaded_ << "," << total_to_download_;

                if (downloaded_ > 0 && total_to_download_ > downloaded_) {
                    if (parent_)
                        parent_->ProgressChange(work_index_);
                }

                return 0;
            }

            int64_t writeAllBuffer() {
                std::lock_guard<std::mutex> locker(parent_->data_->file_write_mutex_);
                assert(parent_);

                if (parent_) {
                    LARGE_INTEGER liOffset;
                    BOOL b;
                    DWORD written = 0;

                    liOffset.QuadPart = (start_pos_ + written_ + parent_->data_->download_header_size_);
                    b = SetFilePointerEx(parent_->data_->file_, liOffset, NULL, FILE_BEGIN);
                    if (b) {
                        b = WriteFile(parent_->data_->file_, buffer_, buffer_used_, &written, NULL);
                        assert(b && written == buffer_used_);

                        written_ += written;

                        buffer_used_ -= written;
                    }
                    else {
                        PPX_LOG_GLE(LS_ERROR) << "SetFilePointerEx failed, offset=" << liOffset.QuadPart;
                    }

                    return (int64_t)written;
                }

                return 0;
            }
        private:
            FileDownload *parent_;
            size_t work_index_;
            int64_t start_pos_;
            int64_t end_pos_;
            int64_t written_;
            std::atomic<int64_t> downloaded_;
            std::atomic<int64_t> total_to_download_;
            char *buffer_;
            size_t buffer_size_;
            size_t buffer_used_;
        };

       

        FileDownload::FileDownload() {
            data_ = new PrivateData();
        }

        FileDownload::~FileDownload() {
            Stop();

            if (data_) {
                delete data_;
                data_ = nullptr;
            }
        }

        void FileDownload::TransferProgress(int64_t &total, int64_t &transfered) {
            int64_t total_value = data_->file_size_;
            int64_t transfered_value = data_->last_time_downloaded_;
            for_each(data_->works_.begin(), data_->works_.end(), [&transfered_value, &total_value](FileDownloadWork * work)->void {
                if (work) {
                    transfered_value += work->downloaded();
                }
            });

            transfered = transfered_value;
            total = total_value;
        }

        bool FileDownload::Start() {
            if (GetStatus() == Progress || thread_num_ < 1 ||
                file_dir_.GetLength() == 0 || file_name_.GetLength() == 0 || url_.GetLength() == 0) {
                return false;
            }

            data_->stop_ = false;
			data_->file_size_ = 0L;
			data_->file_ = nullptr;
            actual_thread_num_ = thread_num_;
			data_->actual_interruption_resuming_ = data_->interruption_resuming_;
			data_->last_time_downloaded_ = 0L;
			data_->is_new_download_ = true;

			StatusFunctor sf = GetStatusFunctor();
            if (sf)
				sf(file_name_ + file_ext_, FileTransferBase::Progress, "", 0L);

            start_time_ = ppx::base::GetTimeStamp();

            bool query_size_ret = false;
            query_size_ret = QueryFileSize();
            if (!query_size_ret) {
                query_size_ret = QueryFileSize(); // once again.
            }

            if (!query_size_ret || data_->file_size_ <= 0) {
				data_->actual_interruption_resuming_ = false;
                actual_thread_num_ = 1;
                PPX_LOG(LS_INFO) << "get file size failed, set multi-thread_support = false, interruption_resuming = false";
            }

            GenerateTmpFileName(data_->file_size_);

            // If can't interruption resuming, need choose a temp file that not exist.
            if (!data_->actual_interruption_resuming_) {
                int index = 1;
                base::String name = tmp_filename_;

                while (_access((file_dir_ + name + tmp_fileext_).ToDataA().c_str(), 0) == 0) {
                    name = tmp_filename_ + "(" + std::to_string((_Longlong)index++) + ")";
                }

                tmp_filename_ = name;
            }

#ifdef _WIN32
            // generate temp file
			data_->file_ = CreateFile((file_dir_ + tmp_filename_ + tmp_fileext_).GetDataPointer(),
                GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (data_->file_ == INVALID_HANDLE_VALUE) {
                return false;
            }

            if (data_->actual_interruption_resuming_) {
                LARGE_INTEGER liSize;

                if (!GetFileSizeEx(data_->file_, &liSize)) {
                    CloseHandle(data_->file_);
					data_->file_ = INVALID_HANDLE_VALUE;
                    return false;
                }

                if (liSize.QuadPart > 0) {
                    if (!ReadDownloadHeader()) { //  try to read header into download_header_
                        PPX_LOG(LS_WARNING) << "temp file has exist, but format error";
                    }
                    else {
						data_->is_new_download_ = false; // if read header successful from temp file, is_new_download_ = false.
                        actual_thread_num_ = data_->download_header_->split_num;
                    }
                }

                if (data_->is_new_download_) { // don't read header from temp file.
					data_->download_header_.reset(new FileDownloadHeader());
					data_->download_header_->split_num = actual_thread_num_;
					data_->download_header_->split_infos.reset(new FileSplitInfo[actual_thread_num_]);
                }
            }

			data_->download_header_size_ = GetDownloadHeaderSize();
           
#else
            // TODO
#endif

            PPX_LOG(LS_INFO) << "interruption-resuming support: " << (data_->actual_interruption_resuming_ ? "true" : "false");
            PPX_LOG(LS_INFO) << "download header size: " << data_->download_header_size_;
            PPX_LOG(LS_INFO) << "thread num: " << actual_thread_num_;

            if (data_->work_thread_.joinable())
				data_->work_thread_.join();
			data_->work_thread_ = std::thread(&FileDownload::WorkLoop, this);
            return true;
        }

        void FileDownload::Stop() {
			data_->stop_ = true;

            if (data_->work_thread_.joinable())
				data_->work_thread_.join();
        }

        void FileDownload::SetInterruptionResuming(bool b) {
            if (GetStatus() != Progress)
				data_->interruption_resuming_ = b;
        }

        static size_t callback_4_query_file_size(char *buffer, size_t size, size_t nitems, void *outstream) {
            size_t avaliable = size * nitems;
            return (avaliable);
        }

        int progress_callback_4_download(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
            FileDownload::FileDownloadWork* work = (FileDownload::FileDownloadWork*)p;
            return work->progressFunc(dltotal, dlnow, ultotal, ulnow);
        }

        static size_t write_callback_4_download(char *buffer, size_t size, size_t nitems, void *outstream) {
            FileDownload::FileDownloadWork* work = (FileDownload::FileDownloadWork*)outstream;

            return work->writeData(buffer, size, nitems);
        }

        bool FileDownload::QueryFileSize() {
            ScopedCurl scoped_curl;
            CURL* curl = scoped_curl.GetCurl();

#ifdef _DEBUG
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#else
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
#endif
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, url_.ToDataA().c_str());
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, ca_path_.GetLength() > 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, ca_path_.GetLength() > 0);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // Time-out connect operations after this amount of seconds
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2); // Time-out the read operation after this amount of seconds

            if (ca_path_.GetLength() > 0)
                curl_easy_setopt(curl, CURLOPT_CAINFO, ca_path_.ToDataA().c_str());

            // avoid libcurl failed with "Failed writing body".
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_4_query_file_size);

            CURLcode ret_code = curl_easy_perform(curl);
            if (ret_code != CURLE_OK) {
                PPX_LOG(LS_ERROR) << "[" << url_.ToDataA().c_str() << "] get file size failed, ret_code=" << ret_code;
                return false;
            }

            int http_code = 0;
            ret_code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (ret_code == CURLE_OK) {
                if (http_code != 200) {
                    PPX_LOG(LS_ERROR) << "[" << url_.ToDataA().c_str() << "] get file size failed, http_code=" << http_code;
                    return false;
                }
            }
            else {
                PPX_LOG(LS_ERROR) << "[" << url_.ToDataA().c_str() << "] get file size failed, and get http code failed, ret_code=" << ret_code;
                return false;
            }

            int64_t filesize = 0L;
            ret_code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &filesize);
            if (ret_code != CURLE_OK) {
                PPX_LOG(LS_ERROR) << "[" << url_.ToDataA().c_str() << "] get file size failed, ret_code=" << ret_code;
                return false;
            }

			data_->file_size_ = filesize;

            PPX_LOG(LS_INFO) << "[" << url_.ToDataA().c_str() << "] size: " << data_->file_size_;
            return true;
        }

        void FileDownload::WorkLoop() {
            int64_t per = data_->file_size_ / actual_thread_num_;
            int64_t start_pos = 0, end_pos = 0;
            size_t run_thread_num = 0;

            data_->multi_ = curl_multi_init();

            size_t work_index = 0;
            bool need_break = false;
			StatusFunctor sf = GetStatusFunctor();

            for (size_t i = 0; i < actual_thread_num_; i++) {
                if (!data_->is_new_download_) { // use last time split info
                    start_pos = data_->download_header_->split_infos[i].start_pos + data_->download_header_->split_infos[i].downloaded - 1;
                    if (start_pos < 0)
                        start_pos = 0; // start pos must > 0, because FileDownloadWork will use it to set file write offset.
                    end_pos = data_->download_header_->split_infos[i].end_pos;
					data_->last_time_downloaded_ += data_->download_header_->split_infos[i].downloaded;

                    PPX_LOG(LS_INFO) << "#" << i << ": " << data_->download_header_->split_infos[i].start_pos
                        << " ~ " << data_->download_header_->split_infos[i].end_pos << " , "
                        << data_->download_header_->split_infos[i].downloaded << " downloaded.";

                    if (start_pos == end_pos) {
                        continue;
                    }
                }
                else {
                        start_pos = i * per;

                        if (i == actual_thread_num_ - 1)
                            end_pos = data_->file_size_ - 1;
                        else
                            end_pos = (i + 1) * per - 1;

                        if (end_pos < 0)
                            end_pos = 0;
                    
                    if (data_->download_header_) {
						data_->download_header_->split_infos[i].start_pos = start_pos;
						data_->download_header_->split_infos[i].end_pos = end_pos;
						data_->download_header_->split_infos[i].downloaded = 0L;
                    }
                    PPX_LOG(LS_INFO) << "#" << i << ": " << start_pos << " ~ " << end_pos << " , 0 downloaded.";
                }

                run_thread_num++;

                char range[64] = { 0 };
                if (start_pos >= 0 && end_pos > 0 && end_pos >= start_pos)
                    snprintf(range, sizeof(range), "%lld-%lld", start_pos, end_pos);
                FileDownloadWork *work = new FileDownloadWork(work_index++, this, start_pos, end_pos);
				data_->works_.push_back(work);

                CURL* curl = curl_easy_init();
                data_->easys_.push_back(curl);

#ifdef _DEBUG
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#else
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
#endif
                curl_easy_setopt(curl, CURLOPT_URL, url_.ToDataA().c_str());
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, ca_path_.GetLength() > 0);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, ca_path_.GetLength() > 0);
                if (ca_path_.GetLength() > 0)
                    curl_easy_setopt(curl, CURLOPT_CAINFO, ca_path_.ToDataA().c_str());
                curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 10L);
                curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);

                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
                curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback_4_download);
                curl_easy_setopt(curl, CURLOPT_XFERINFODATA, work);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_4_download);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, work);

                // important option
                if (strlen(range) > 0) {
                    CURLcode err = curl_easy_setopt(curl, CURLOPT_RANGE, range);
                    if (err != CURLE_OK) {
                        PPX_LOG(LS_WARNING) << "curl_easy_setopt CURLOPT_RANGE failed, code=" << err;
                        
                        if (data_->is_new_download_ && // a new download, not resume download.
                            i== 0) { // and error occur on first set CURLOPT_RANGE
                            need_break = true; // we use on thread to download.
                        }
                    }
                }

                CURLMcode m_code = curl_multi_add_handle(data_->multi_, curl);
                if (m_code != CURLE_OK) {
                    PPX_LOG(LS_ERROR) << "curl_multi_add_handle failed, code=" << m_code;
                    LastCurlClear();
                    for_each(data_->works_.begin(), data_->works_.end(), [](FileDownloadWork * work)->void {if (work) { delete work; } });
					data_->works_.clear();
#ifdef _WIN32
                    SAFE_CLOSE_ON_VALID_HANDLE(data_->file_);
#endif

                    if (sf)
						sf(file_name_ + file_ext_, FileTransferBase::Failed, "curl_multi_add_handle failed", 0L);
                    return;
                }

                if(need_break)
                    break;
            }

            if (data_->actual_interruption_resuming_ && data_->is_new_download_) {
                // write new header
                if (!WriteDownloadHeader()) {
                    for_each(data_->easys_.begin(), data_->easys_.end(), [this](CURL * e)->void {
                        if (e) {
                            CURLMcode code = curl_multi_remove_handle(data_->multi_, e);
                            if (code != CURLM_CALL_MULTI_PERFORM && code != CURLM_OK) {
                            }
                        }
                    });

                    curl_multi_cleanup(data_->multi_);

                    for_each(data_->easys_.begin(), data_->easys_.end(), [this](CURL * e)->void { if (e) curl_easy_cleanup(e); });
                    data_->easys_.clear();

                    for_each(data_->works_.begin(), data_->works_.end(), [](FileDownloadWork * work)->void {if (work) {
                        work->writeAllBuffer();
                        delete work;
                    } });
					data_->works_.clear();

                    return;
                }
            }

#ifdef _WIN32
            LARGE_INTEGER liSize;
            liSize.QuadPart = data_->file_size_ + data_->download_header_size_;
            SetFilePointerEx(data_->file_, liSize, NULL, FILE_BEGIN);
            SetEndOfFile(data_->file_);
#else
#endif

            int still_running = 0;
            CURLMcode m_code = curl_multi_perform(data_->multi_, &still_running);


            if (sf)
                sf(file_name_ + file_ext_, FileTransferBase::Progress, "", 0L);

            do {
                if (data_->stop_) {
                    break;
                }

                struct timeval timeout;

                /* set a suitable timeout to play around with */
                timeout.tv_sec = 1;

                timeout.tv_usec = 0;

                long curl_timeo = -1;

                curl_multi_timeout(data_->multi_, &curl_timeo);

                if (curl_timeo > 0) {
                    timeout.tv_sec = curl_timeo / 1000;

                    if (timeout.tv_sec > 1)
                        timeout.tv_sec = 1;
                    else
                        timeout.tv_usec = (curl_timeo % 1000) * 1000;
                }
                else {
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 100 * 1000;
                }

                fd_set fdread;
                fd_set fdwrite;
                fd_set fdexcep;
                int maxfd = -1;

                FD_ZERO(&fdread);
                FD_ZERO(&fdwrite);
                FD_ZERO(&fdexcep);

                /* get file descriptors from the transfers */
                CURLMcode code = curl_multi_fdset(data_->multi_, &fdread, &fdwrite, &fdexcep, &maxfd);
                if (code != CURLM_CALL_MULTI_PERFORM && code != CURLM_OK) {
                    break;
                }


                /* On success the value of maxfd is guaranteed to be >= -1. We call
                select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
                no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
                to sleep 100ms, which is the minimum suggested value in the
                curl_multi_fdset() doc. */

                int rc;

                if (maxfd == -1) {
#ifdef _WIN32
                    Sleep(100);
                    rc = 0;
#else
                    /* Portable sleep for platforms other than Windows. */
                    struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
                    rc = select(0, NULL, NULL, NULL, &wait);
#endif
                }
                else {
                    /* Note that on some platforms 'timeout' may be modified by select().
                    If you need access to the original value save a copy beforehand. */
                    rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
                }

                switch (rc) {
                case -1:
                    break; /* select error */

                case 0: /* timeout */
                default: /* action */
                    curl_multi_perform(data_->multi_, &still_running);
                    break;
                }

#ifdef _WIN32
                // sleep 10ms to reduce cpu usage.
                Sleep(10);
#else
                /* Portable sleep for platforms other than Windows. */
                struct timeval wait = { 0, 10 * 1000 }; /* 100ms */
                rc = select(0, NULL, NULL, NULL, &wait);
#endif
            } while (still_running);

            /* See how the transfers went */
            size_t done_thread = 0;
            CURLMsg * msg; /* for picking up messages with the transfer status */
            int msgsInQueue;
            while ((msg = curl_multi_info_read(data_->multi_, &msgsInQueue)) != NULL) {
                if (msg->msg == CURLMSG_DONE) {
                    if (msg->data.result == CURLE_OK) {
                        done_thread++;
                    }
                }
            }

            LastCurlClear();
            int64_t total_downloaded = 0L;
            for_each(data_->works_.begin(), data_->works_.end(), [&total_downloaded](FileDownloadWork * work)->void {if (work) {
                work->writeAllBuffer();
                total_downloaded += work->downloaded();
                delete work;
            } });
			data_->works_.clear();


            // sometimes get filesize failed, file_size_ is 0;
            // sometimes split failed(such as set CURLOPT_RANGE failed), run_thread_num_ just 1.
            if (done_thread == run_thread_num || total_downloaded == data_->file_size_) { // download complete.
                bool copy_ret = CopyDataToFile();

#ifdef _WIN32
                SAFE_CLOSE_ON_VALID_HANDLE(data_->file_);
#endif
                int64_t used = (ppx::base::GetTimeStamp() - start_time_) / 1000;

                std::string reason;

                if (copy_ret) {
                    bool del_ret = DeleteTmpFile();
                    
                    PPX_LOG(LS_INFO) << "Download [" << (file_name_ + file_ext_).ToDataA().c_str() << "] successful, used " << used << " ms";

                    if (file_md5_.GetLength() > 0) {
                        base::String md5 = ppx::base::GetFileMd5((file_dir_ + file_name_ + file_ext_).ToDataA().c_str());
						md5.MakeLower();
                        if (md5 != file_md5_) {
                            PPX_LOG(LS_ERROR) << "Md5 error, " << file_md5_.ToDataA().c_str() << " != " << md5.ToDataA().c_str();
							SetStatus(FileTransferBase::Failed);
                            reason = "md5 error";
                        }
                        else {
							SetStatus(FileTransferBase::Finished);
                        }
                    }
                    else {
						SetStatus(FileTransferBase::Finished);
                    }
                }
                else {
                    PPX_LOG(LS_INFO) << "Download [" << (file_name_ + file_ext_).ToDataA().c_str() << "] failed (copy data from temp file failed)";
					SetStatus(FileTransferBase::Failed);
                    reason = "copy file failed";
                }

                if (sf) {
					sf(file_name_ + file_ext_, GetStatus(), reason, used);
                }
            }
            else {
                bool b = UpdateDownloadHeader();

#ifdef _WIN32
                SAFE_CLOSE_ON_VALID_HANDLE(data_->file_);
#endif

				SetStatus(FileTransferBase::Ready);

                if (sf) {
                    if (data_->stop_)
						sf(file_name_ + file_ext_, FileTransferBase::Ready, "", 0L);
                    else
						sf(file_name_ + file_ext_, FileTransferBase::Failed, "", 0L);
                }
            }

			data_->download_header_.release();
        }

        void FileDownload::ProgressChange(size_t index) {
            if (data_->download_header_) {
				data_->download_header_->split_infos[index].downloaded =
					data_->download_header_->split_infos[index].last_downloaded + data_->works_[index]->downloaded();
            }

			ProgressFunctor pf = GetProgressFunctor();

            if (pf) {
                static int64_t last_total = 0L;
                static int64_t last_transfer = 0L;
                static int64_t last_time = 0;
                int64_t total = 0L, transfer = 0L;
                TransferProgress(total, transfer);

                if (transfer != last_transfer) {
                    int64_t now_ms = ppx::base::GetTimeStamp() / 1000;

                    if (now_ms - last_time >= progress_interval_ || total == transfer) {
                        last_time = now_ms;
                        last_transfer = transfer;
						pf(file_name_ + file_ext_, total, transfer);
                    }
                }
            }
        }

        bool FileDownload::ReadDownloadHeader() {
            bool ret = false;

            DWORD read = 0;
            BOOL read_ret = FALSE;

			data_->download_header_.reset(new FileDownloadHeader());

            read_ret = ReadFile(data_->file_, &data_->download_header_->flag, sizeof(data_->download_header_->flag), &read, NULL);

            if (!read_ret || read != sizeof(data_->download_header_->flag)) {
				data_->download_header_.release();
                return false;
            }

            if (strcmp(data_->download_header_->flag, FILE_DOWNLOAD_HEADER_SIGN) != 0) {
				data_->download_header_.release();
                return false;
            }

            read_ret = ReadFile(data_->file_, &data_->download_header_->split_num, sizeof(data_->download_header_->split_num), &read, NULL);

            if (!read_ret || read != sizeof(data_->download_header_->split_num)) {
				data_->download_header_.release();
                return false;
            }

            if (data_->download_header_->split_num <= 0) {
				data_->download_header_.release();
                return false;
            }

			data_->download_header_->split_infos.reset(new FileSplitInfo[data_->download_header_->split_num]);

            for (size_t i = 0; i < data_->download_header_->split_num; i++) {
                read_ret = ReadFile(data_->file_, &(data_->download_header_->split_infos[i]), sizeof(FileSplitInfo), &read, NULL);

                if (!read_ret || read != sizeof(FileSplitInfo)) {
					data_->download_header_.release();
                    return false;
                }


                FileSplitInfo *info = &(data_->download_header_->split_infos[i]);
                if (info->start_pos < 0 || info->end_pos <= 0 || info->downloaded < 0 ||
                    (info->start_pos + info->downloaded > info->end_pos + 1)) {
					data_->download_header_.release();
                    return false;
                }
            }

            return true;
        }

        bool FileDownload::WriteDownloadHeader() {
            LARGE_INTEGER liOffset;
            BOOL b;
            DWORD written = 0;

            if (!data_->download_header_)
                return false;

            liOffset.QuadPart = 0;
            b = SetFilePointerEx(data_->file_, liOffset, NULL, FILE_BEGIN);

            if (!b)
                return false;

            b = WriteFile(data_->file_, FILE_DOWNLOAD_HEADER_SIGN, strlen(FILE_DOWNLOAD_HEADER_SIGN), &written, NULL);

            if (!b || written != strlen(FILE_DOWNLOAD_HEADER_SIGN))
                return false;

            b = WriteFile(data_->file_, &data_->download_header_->split_num, sizeof(data_->download_header_->split_num), &written, NULL);

            if (!b || written != sizeof(data_->download_header_->split_num))
                return false;

            for (size_t i = 0; i < data_->download_header_->split_num; i++) {
                b = WriteFile(data_->file_, &(data_->download_header_->split_infos[i]), sizeof(FileSplitInfo), &written, NULL);

                if (!b || written != sizeof(FileSplitInfo))
                    return false;
            }

            return true;
        }

        bool FileDownload::UpdateDownloadHeader() {
            LARGE_INTEGER liOffset;
            BOOL b;
            DWORD written = 0;

            if (!data_->download_header_)
                return false;

            // sign, split_num will not be changed in download progress.

            liOffset.QuadPart = strlen(FILE_DOWNLOAD_HEADER_SIGN) + sizeof(data_->download_header_->split_num);
            b = SetFilePointerEx(data_->file_, liOffset, NULL, FILE_BEGIN);

            if (!b)
                return false;

            PPX_LOG(LS_INFO) << "Download broken:";

            for (size_t i = 0; i < data_->download_header_->split_num; i++) {
                // note: important
				data_->download_header_->split_infos[i].last_downloaded = data_->download_header_->split_infos[i].downloaded;
                b = WriteFile(data_->file_, &(data_->download_header_->split_infos[i]), sizeof(FileSplitInfo), &written, NULL);
                PPX_LOG(LS_INFO) << "#" << i << ": " << data_->download_header_->split_infos[i].start_pos
                    << " ~ " << data_->download_header_->split_infos[i].end_pos
                    << " , " << data_->download_header_->split_infos[i].downloaded << " downloaded";

                if (!b || written != sizeof(FileSplitInfo))
                    return false;
            }

            return true;
        }

        size_t FileDownload::GetDownloadHeaderSize() {
            if (data_->download_header_) {
                return (FILE_DOWNLAD_HEADER_SIZE + sizeof(data_->download_header_->split_num) + data_->download_header_->split_num * sizeof(FileSplitInfo));;
            }

            return 0;
        }

        bool FileDownload::CopyDataToFile() {
            if (data_->file_ == INVALID_HANDLE_VALUE)
                return false;

            int index = 1;
            base::String name = file_name_;

            while (_access((file_dir_ + name + file_ext_).ToDataA().c_str(), 0) == 0) {
                name = file_name_ + "(" + std::to_string((_Longlong)index++) + ")";
            }

            file_name_ = name;

            HANDLE f = CreateFile((file_dir_ + file_name_ + file_ext_).GetDataPointer(),
                GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

            if (f == INVALID_HANDLE_VALUE) {
                return false;
            }

            char *buf = (char *)malloc(1024);

            if (!buf) {
                CloseHandle(f);
                return false;
            }

            DWORD read = 0;
            DWORD written = 0;
            BOOL  b;

            LARGE_INTEGER liOffset;
            liOffset.QuadPart = GetDownloadHeaderSize();

            if (!SetFilePointerEx(data_->file_, liOffset, NULL, FILE_BEGIN)) {
                CloseHandle(f);
                return false;
            }

            while (ReadFile(data_->file_, buf, 1024, &read, NULL) && read > 0) {
                b = WriteFile(f, buf, read, &written, NULL);
                assert(b && read == written);
            }

            free(buf);

            CloseHandle(f);

            return true;
        }

        bool FileDownload::DeleteTmpFile() {
#ifdef _WIN32
            SAFE_CLOSE_ON_VALID_HANDLE(data_->file_);
            return DeleteFile((file_dir_ + tmp_filename_ + tmp_fileext_).GetDataPointer()) == TRUE;
#endif
        }

        void FileDownload::LastCurlClear() {
            if (!data_)
                return;
            for_each(data_->easys_.begin(), data_->easys_.end(), [this](CURL * e)->void {
                if (e) {
                    CURLMcode code = curl_multi_remove_handle(data_->multi_, e);
                    if (code != CURLM_CALL_MULTI_PERFORM && code != CURLM_OK) {
                    }
                }
            });

            curl_multi_cleanup(data_->multi_);

            for_each(data_->easys_.begin(), data_->easys_.end(), [this](CURL * e)->void { if (e) curl_easy_cleanup(e); });
            data_->easys_.clear();

      
        }

    }
}
