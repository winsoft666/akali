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

#ifndef RTC_BASE_OVERLAPPED_SOCKET_H_
#define RTC_BASE_OVERLAPPED_SOCKET_H_

#include <memory>
#include <thread>
#include "ppxbase/event.h"
#include "ppxbase/iocp.h"
#include "ppxbase/socket.h"
#include "ppxbase/socketaddress.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API OverlappedSocket: public Socket {
        public:
            class OverlappedSocketDelegate {
            public:
                virtual void OnAcceptEvent(OverlappedSocket *socket) = 0;
                virtual void OnReadEvent(OverlappedSocket *socket, const PER_IO_CONTEXT* io_ctx) = 0;
                virtual void OnWriteEvent(OverlappedSocket *socket, const PER_IO_CONTEXT* io_ctx) = 0;
                virtual void OnConnectEvent(OverlappedSocket *socket) = 0;
                virtual void OnCloseEvent(OverlappedSocket *socket, int close_code) = 0;
            };
            OverlappedSocket();
            virtual ~OverlappedSocket();

            bool CreateT(int family, int type);

            SocketAddress GetLocalAddress() const;
            SocketAddress GetRemoteAddress() const;
            int Bind(const SocketAddress& addr);
            int Connect(const SocketAddress& addr);
            int Send(const void* buffer, size_t length, PER_IO_CONTEXT* io_ctx = NULL);
            int Listen(int backlog = SOMAXCONN);

            int RecvFrom(const SocketAddress& addr, PER_IO_CONTEXT* io_ctx = NULL);
            int SendTo(const void* buffer, size_t length, const SocketAddress& addr, PER_IO_CONTEXT* io_ctx = NULL);

            bool Accept();
            int Close();

            int GetOption(Socket::Option opt, int* value);
            int SetOption(Socket::Option opt, const char* value, int value_len);

            void Clone(OverlappedSocket *socket);

            void RegisterDelegate(OverlappedSocketDelegate* delegate) {
                delegate_ = delegate;
            }
        public:
            class CompletionIOHandler {
            public:
                CompletionIOHandler(OverlappedSocket* parent, int index);
                ~CompletionIOHandler();
                void Run();
            private:
                OverlappedSocket* parent_;
                std::thread thread_;
            };

        private:
            bool InitIOCP(SOCKET s);
            void UpdateLastError();
            bool PostAccept(PER_IO_CONTEXT* io_ctx);
            bool PostRecv(PER_IO_CONTEXT* io_ctx);
            bool PostSend(PER_IO_CONTEXT* io_ctx, const void* msg, size_t msg_len);
            bool PostConnect(PER_IO_CONTEXT* io_ctx, const SocketAddress& addr);
            bool PostRecvFrom(PER_IO_CONTEXT* io_ctx, const SocketAddress& addr);
            bool PostSendTo(const void* buffer, size_t length, PER_IO_CONTEXT* io_ctx, const SocketAddress& addr);
        private:
            base::Event exit_;
            HANDLE iocp_;
            int workthread_num_;
            std::vector<std::unique_ptr<CompletionIOHandler>> workthreads_;

            PER_SOCKET_CONTEXT* own_socket_ctx_;
            LPFN_CONNECTEX connectex_fn_;
            LPFN_ACCEPTEX acceptex_fn_;
            LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_fn_;

            
            SocketAddress addr_;         // address that we connected to (see DoConnect)
            SocketAddress remote_addr_;
            sockaddr_in remote_addrin_; // for overlapped udp

            bool closing_;
            int close_error_;

            OverlappedSocketDelegate *delegate_;
        };
    }
}

#endif
