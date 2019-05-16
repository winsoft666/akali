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

#ifndef PPX_IPC_H_
#define PPX_IPC_H_
#pragma once
#include "ppx_config.h"
#include "ppx_export.h"
#include <functional>
#include "base/constructormagic.h"

namespace ppx {
    namespace net {
        class PPX_API IPC {
        public:
            IPC();
            virtual ~IPC();

            bool StartListen(const char* ipc_name, unsigned int ipc_name_len, std::function<void(const void* data, unsigned int data_size)> cb);
            void StopListen();

            bool SyncSend(const char* ipc_name, unsigned int ipc_name_len, const void* data, unsigned int data_size);

        private:
            class IPCImpl;
            IPCImpl *impl_;

        private:
            PPX_DISALLOW_COPY_AND_ASSIGN(IPC);
        };
    }
}

#endif // !PPX_IPC_H_