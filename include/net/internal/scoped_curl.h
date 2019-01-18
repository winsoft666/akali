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

#ifndef PPX_NET_SCOPED_CURL_H_
#define PPX_NET_SCOPED_CURL_H_
#pragma once

#include "curl/curl.h"

class ScopedCurl {
public:
    ScopedCurl() {
        curl_ = curl_easy_init();
    }

    ~ScopedCurl() {
        curl_easy_cleanup(curl_);
    }

    CURL* GetCurl() {
        return curl_;
    }

private:
    CURL* curl_;
};

#endif // !PPX_NET_SCOPED_CURL_H_