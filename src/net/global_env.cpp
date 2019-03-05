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

#include "net/global_env.h"
#ifndef PPX_NO_HTTP
#include "curl/curl.h"

namespace ppx {
    namespace net {

        void GlobalInit() {
            curl_global_init(CURL_GLOBAL_ALL);
        }

        void GlobalUnInit() {
            curl_global_cleanup();
        }
    }
}
#endif //!PPX_NO_HTTP