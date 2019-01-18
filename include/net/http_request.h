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

#include <string>
#include "ppx_export.h"

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

            int Get(const std::string &url, std::string &response);
            int Post(const std::string &url, const std::string &post_data, std::string &response);
            
            bool IsHttps(const std::string &url);
            void SetCAPath(const std::string &ca_path);

        private:
            int connect_timeout_ms_; // default 1000ms
            int read_timeout_ms_;    // default 1000ms
            std::string ca_path_;

            class HttpRequestImpl;
            HttpRequestImpl* impl_;
        };
    }
}

#endif // !PPX_NET_HTTP_REQUEST_H_