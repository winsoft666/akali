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

#ifndef PPX_BASE_IOCP_SERVER_H_
#define PPX_BASE_IOCP_SERVER_H_

#include <list>
#include "ppxbase/iocp_socket.h"
#include "ppxbase_export.h"
#include "ppxbase/criticalsection.h"
#include "ppxbase/socketaddress.h"
#include "ppxbase/overlappedsocket.h"

namespace ppx {
    namespace base {
        typedef std::list<OverlappedSocket *> ClientList;

        class PPXBASE_API IOCPServer : public OverlappedSocket::OverlappedSocketDelegate {
          public:
            void OnAcceptEvent(OverlappedSocket *socket) override;
            void OnReadEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) override;
            void OnWriteEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) override;
            void OnConnectEvent(OverlappedSocket *socket) override;
            void OnCloseEvent(OverlappedSocket *socket, int close_code) override;

            IOCPServer();
            virtual ~IOCPServer();

            bool Start(const SocketAddress &addr, int family, int type);
            bool Stop();

            int64_t GetStartTime() const;

          private:
            int64_t start_time_;

            OverlappedSocket *socket_;

            ClientList client_list_;
            base::CriticalSection crit_;  // CriticalSection for client_list_
        };
    }
}

#endif // PPX_BASE_IOCP_SERVER_H_
