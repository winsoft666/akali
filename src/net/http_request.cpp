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

#include "net/http_request.h"
#ifndef PPX_NO_HTTP
#include <assert.h>
#include "curl/curl.h"
#include "base/safe_release_macro.h"
#include "base/stringencode.h"

namespace ppx {
    namespace net {

        class HttpRequest::HttpRequestImpl {
        public:
            HttpRequestImpl() : curl_(nullptr) {
                curl_ = curl_easy_init();;
            }

            ~HttpRequestImpl() {
                if (curl_) {
                    curl_easy_cleanup(curl_);
                    curl_ = nullptr;
                }
            }

            CURL* curl_;
            //static ppx::base::CriticalSection cs_;
        };

        //ppx::base::CriticalSection HttpRequest::HttpRequestImpl::cs_;

        HttpRequest::HttpRequest() : 
            connect_timeout_ms_(1000), 
            read_timeout_ms_(1000),
            impl_(nullptr) {
            impl_ = new HttpRequestImpl();
        }

        HttpRequest::~HttpRequest() {
            SAFE_DELETE(impl_);
        }

        void HttpRequest::SetConnectTimeoutMS(int ms) {
            connect_timeout_ms_ = ms;
        }

        int HttpRequest::GetConnectTimeoutMS() const {
            return connect_timeout_ms_;
        }

        void HttpRequest::SetReadTimeoutMS(int ms) {
            read_timeout_ms_ = ms;
        }

        int HttpRequest::GetReadTimeoutMS() const {
            return read_timeout_ms_;
        }

        static size_t WriteCB(char *ptr, size_t size, size_t nmemb, void *userdata) {
			base::BufferQueue *response = (base::BufferQueue *)userdata;
            if (NULL == response || NULL == ptr || 0 == size * nmemb)
                return -1;

            size_t resSize = size * nmemb;
			response->AddToLast(ptr, resSize);
            return resSize;
        }

        int HttpRequest::Get(const base::StringUTF8 &url, base::BufferQueue &response, const std::vector<base::StringUTF8>* const headers /*= NULL*/) {
            assert(impl_);
            assert(impl_->curl_);

            curl_easy_setopt(impl_->curl_, CURLOPT_URL, url.GetDataPointer());
            curl_easy_setopt(impl_->curl_, CURLOPT_READFUNCTION, NULL);
            curl_easy_setopt(impl_->curl_, CURLOPT_WRITEFUNCTION, WriteCB);
            curl_easy_setopt(impl_->curl_, CURLOPT_WRITEDATA, (void*)&response);
            curl_easy_setopt(impl_->curl_, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(impl_->curl_, CURLOPT_TIMEOUT_MS, read_timeout_ms_);
            curl_easy_setopt(impl_->curl_, CURLOPT_CONNECTTIMEOUT_MS, connect_timeout_ms_);
            if (ca_path_.GetLength() == 0) {
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYPEER, false);
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYHOST, false);
            }
            else {
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYPEER, true);
                curl_easy_setopt(impl_->curl_, CURLOPT_CAINFO, ca_path_);
            }

            struct curl_slist *chunk = NULL;

            if (headers) {
                for (size_t i = 0; i < headers->size(); i++) {
                    chunk = curl_slist_append(chunk,headers->at(i).GetDataPointer());
                }
                curl_easy_setopt(impl_->curl_, CURLOPT_HTTPHEADER, chunk);
            }

            CURLcode code = curl_easy_perform(impl_->curl_);

            if(chunk)
                curl_slist_free_all(chunk);

            return (int)code;
        }

        int HttpRequest::Post(const base::StringUTF8 &url, const char* post_data, int post_data_len, base::BufferQueue &response, const std::vector<base::StringUTF8>* const headers /*= NULL*/) {
            assert(impl_);
            assert(impl_->curl_);

            curl_easy_setopt(impl_->curl_, CURLOPT_URL, url.GetDataPointer());
            curl_easy_setopt(impl_->curl_, CURLOPT_POST, 1);
            curl_easy_setopt(impl_->curl_, CURLOPT_POSTFIELDS, post_data);
            curl_easy_setopt(impl_->curl_, CURLOPT_POSTFIELDSIZE, post_data_len);
            curl_easy_setopt(impl_->curl_, CURLOPT_READFUNCTION, NULL);
            curl_easy_setopt(impl_->curl_, CURLOPT_WRITEFUNCTION, WriteCB);
            curl_easy_setopt(impl_->curl_, CURLOPT_WRITEDATA, (void*)&response);
            curl_easy_setopt(impl_->curl_, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(impl_->curl_, CURLOPT_TIMEOUT_MS, read_timeout_ms_);
            curl_easy_setopt(impl_->curl_, CURLOPT_CONNECTTIMEOUT_MS, connect_timeout_ms_);
            if (ca_path_.GetLength() == 0) {
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYPEER, false);
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYHOST, false);
            }
            else {
                curl_easy_setopt(impl_->curl_, CURLOPT_SSL_VERIFYPEER, true);
                curl_easy_setopt(impl_->curl_, CURLOPT_CAINFO, ca_path_);
            }

            struct curl_slist *chunk = NULL;
            if (headers) {
                for (size_t i = 0; i < headers->size(); i++) {
                    chunk = curl_slist_append(chunk, headers->at(i).GetDataPointer());
                }
                curl_easy_setopt(impl_->curl_, CURLOPT_HTTPHEADER, chunk);
            }

            CURLcode code = curl_easy_perform(impl_->curl_);

            return (int)code;
        }

        bool HttpRequest::IsHttps(const base::StringUTF8 &url) {
            if (url.Find("https") == 0)
                return true;
            return false;
        }

        void HttpRequest::SetCAPath(const base::StringUTF8 &ca_path) {
            ca_path_ = ca_path;
        }

    }
}
#endif //!PPX_NO_HTTP