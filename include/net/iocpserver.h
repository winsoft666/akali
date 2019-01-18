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

#ifndef PPX_NET_IOCP_SERVER_H_
#define PPX_NET_IOCP_SERVER_H_

#include <list>
#include "net/iocp.h"
#include "ppx_export.h"
#include "base/criticalsection.h"
#include "net/socketaddress.h"
#include "net/overlappedsocket.h"

namespace ppx {
    namespace net {
        typedef std::list<OverlappedSocket*> ClientList;

        class PPX_API IOCPServer : public OverlappedSocket::OverlappedSocketDelegate {
        public:
            void OnAcceptEvent(OverlappedSocket *socket) override;
            void OnReadEvent(OverlappedSocket *socket, const PER_IO_CONTEXT* io_ctx) override;
            void OnWriteEvent(OverlappedSocket *socket, const PER_IO_CONTEXT* io_ctx) override;
            void OnConnectEvent(OverlappedSocket *socket) override;
            void OnCloseEvent(OverlappedSocket *socket, int close_code) override;

            IOCPServer();
            virtual ~IOCPServer();

            bool Start(const SocketAddress &addr, int family, int type);
            bool Stop();

            int64_t GetStartTime() const;

        private:
            int64_t start_time_;

            OverlappedSocket* socket_;

            ClientList client_list_;
            base::CriticalSection crit_;  // CriticalSection for client_list_
        };
    }
}

#endif // PPX_NET_IOCP_SERVER_H_