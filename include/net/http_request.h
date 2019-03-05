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

            int Get(const base::String &url, base::BufferQueue &response);
            int Post(const base::String &url, const base::String &post_data, base::BufferQueue &response);
            
            bool IsHttps(const base::String &url);
            void SetCAPath(const base::String &ca_path);

        private:
            int connect_timeout_ms_; // default 1000ms
            int read_timeout_ms_;    // default 1000ms
            base::String ca_path_;

            class HttpRequestImpl;
            HttpRequestImpl* impl_;
        };
    }
}

#endif //!PPX_NO_HTTP

#endif // !PPX_NET_HTTP_REQUEST_H_