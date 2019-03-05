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

#ifndef PPX_NET_GLOBAL_ENV_H_
#define PPX_NET_GLOBAL_ENV_H_
#pragma once

#include "ppx_config.h"

#ifndef PPX_NO_HTTP

#include "ppx_export.h"

namespace ppx {
    namespace net {
        PPX_API void GlobalInit();
        PPX_API void GlobalUnInit();
    }
}

#endif // !PPX_NO_HTTP

#endif // !PPX_NET_GLOBAL_ENV_H_