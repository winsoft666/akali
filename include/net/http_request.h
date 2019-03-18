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

#ifndef PPX_NET_HTTP_REQUEST_H_
#define PPX_NET_HTTP_REQUEST_H_
#pragma once
#include "ppx_config.h"

#ifndef PPX_NO_HTTP
#include <string>
#include "ppx_export.h"
#include "base/string.h"
#include "base/buffer_queue.h"

namespace ppx {
    namespace net {
        class PPX_API HttpRequest {
        public:
            HttpRequest();
            virtual~HttpRequest();

            void SetConnectTimeoutMS(int ms);
            int GetConnectTimeoutMS() const;

            void SetReadTimeoutMS(int ms);
            int GetReadTimeoutMS() const;

            // headers string sample:
            //
            // Remove a header curl would otherwise add by itself
            //  "Accept:"
            //
            // Add a custom header 
            // "Another: yes"
            //
            // Modify a header curl otherwise adds differently
            // "Host: example.com"
            //
            // Add a header with "blank" contents to the right of the colon. Note that we're then using a semicolon in the string we pass to curl! */
            // "X-silly-header;"
            //
            int Get(const base::StringUTF8 &url, base::BufferQueue &response, const std::vector<base::StringUTF8>* const headers = NULL);
            int Post(const base::StringUTF8 &url, const char* post_data, int post_data_len, base::BufferQueue &response, const std::vector<base::StringUTF8>* const headers = NULL);
            
            bool IsHttps(const base::StringUTF8 &url);
            void SetCAPath(const base::StringUTF8 &ca_path);

        private:
            int connect_timeout_ms_; // default 1000ms
            int read_timeout_ms_;    // default 1000ms
            base::StringUTF8 ca_path_;

            class HttpRequestImpl;
            HttpRequestImpl* impl_;
        };
    }
}

#endif //!PPX_NO_HTTP

#endif // !PPX_NET_HTTP_REQUEST_H_