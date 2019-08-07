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

#include "ppxbase/overlappedsocket.h"
#include "ppxbase/logging.h"
#include <process.h>
#include "ppxbase/timeutils.h"
#include "ppxbase/assert.h"
#include "ppxbase/thread_util.h"

namespace ppx {
    namespace base {

        OverlappedSocket::CompletionIOHandler::CompletionIOHandler(OverlappedSocket* parent, int index) :
            parent_(parent) {

            thread_ = std::thread([this, index]() {
                base::SetCurrentThreadName(("OverlappedSocketWorkThread_" + std::to_string(index)).c_str());
                this->Run();
            });
        }

        OverlappedSocket::CompletionIOHandler::~CompletionIOHandler() {
            if (thread_.joinable())
                thread_.join();
        }

        void OverlappedSocket::CompletionIOHandler::Run() {
            PPX_ASSERT(parent_);
            DWORD transferred_bytes = 0;
            OverlappedSocket *overlapped_socket = NULL;

            OVERLAPPED *overlapped = NULL;
            DWORD gle = 0;

            while (!parent_->exit_.Wait(0)) {
                BOOL ret = GetQueuedCompletionStatus(parent_->iocp_, &transferred_bytes, (PULONG_PTR)&overlapped_socket, &overlapped, INFINITE);

                if (overlapped_socket == EXIT_CODE) {
                    break;
                }

                if (ret == FALSE) {
                    gle = GetLastError();
                    if (gle == WAIT_TIMEOUT) {
                        PPX_NOT_REACHED("");
                        continue;
                    }
                    else if (gle == ERROR_NETNAME_DELETED) {
                        if(parent_ && parent_->delegate_)
                            parent_->delegate_->OnCloseEvent(overlapped_socket, gle);
                        continue;
                    }
                    else {
                        if (parent_ && parent_->delegate_)
                            parent_->delegate_->OnCloseEvent(overlapped_socket, gle);
                        break;
                    }
                }
                else {
                    PER_IO_CONTEXT *io_ctx = CONTAINING_RECORD(overlapped, PER_IO_CONTEXT, overlapped);

                    if ((transferred_bytes == 0) && (io_ctx->operation_type == RECV_POSTED || io_ctx->operation_type == SEND_POSTED)) {
                        if (parent_ && parent_->delegate_)
                            parent_->delegate_->OnCloseEvent(overlapped_socket, gle);
                        continue;
                    }

                    if (io_ctx->operation_type == ACCEPT_POSTED) {
                        sockaddr_storage* ClientAddr = NULL;
                        sockaddr_storage* LocalAddr = NULL;
                        int remoteLen = sizeof(sockaddr_storage);
                        int localLen = sizeof(sockaddr_storage);

                        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms738516(v=vs.85).aspx
                        parent_->getacceptexsockaddrs_fn_(io_ctx->wsa_buffer.buf,
                            0,
                            sizeof(sockaddr_storage) + 16,
                            sizeof(sockaddr_storage) + 16,
                            (LPSOCKADDR*)&LocalAddr,
                            &localLen,
                            (LPSOCKADDR*)&ClientAddr,
                            &remoteLen);

                        SocketAddress remote_addr;
                        SocketAddressFromSockAddrStorage(*ClientAddr, &remote_addr);

                        OverlappedSocket* new_overlapped_socket = new OverlappedSocket();
                        new_overlapped_socket->remote_addr_ = remote_addr;

                        PER_SOCKET_CONTEXT* new_socket_ctx = new PER_SOCKET_CONTEXT();
                        new_socket_ctx->socket = io_ctx->socket;

                        new_overlapped_socket->own_socket_ctx_ = new_socket_ctx;
                        overlapped_socket->Clone(new_overlapped_socket);

                        if (!AssociateDeviceWithCompletionPort(parent_->iocp_, (HANDLE)new_overlapped_socket->own_socket_ctx_->socket, (DWORD)new_overlapped_socket)) {
                            delete new_socket_ctx;
                            new_socket_ctx = NULL;
                        }
                        else {
                            parent_->state_ = Socket::CS_CONNECTED;

                            new_overlapped_socket->connect_time_ = base::GetTimeStamp();
                            // post new accept
                            if (!parent_->PostAccept(io_ctx)) {
                                PPX_NOT_REACHED("");
                            }

                            if (parent_ && parent_->delegate_)
                                parent_->delegate_->OnAcceptEvent(new_overlapped_socket);

                            // post recv
                            PER_IO_CONTEXT *new_io_ctx = new_socket_ctx->GetNewIoContext();
                            new_io_ctx->socket = new_socket_ctx->socket;
                            if (!parent_->PostRecv(new_io_ctx)) {
                                new_socket_ctx->RemoveContext(new_io_ctx);
                            }
                        }
                    }
                    else if (io_ctx->operation_type == RECV_POSTED) {
                        if (parent_->state_ != Socket::CS_CONNECTED) { // connectionless
                            sockaddr_storage* addr = reinterpret_cast<sockaddr_storage*>(&parent_->remote_addrin_);
                            SocketAddressFromSockAddrStorage(*addr, &parent_->remote_addr_);
                        }

                        if (parent_ && parent_->delegate_)
                            parent_->delegate_->OnReadEvent(overlapped_socket, io_ctx);

                        if (!parent_->PostRecv(io_ctx)) {
                            overlapped_socket->own_socket_ctx_->RemoveContext(io_ctx);
                        }
                    }
                    else if (io_ctx->operation_type == SEND_POSTED) {
                        if (parent_ && parent_->delegate_)
                            parent_->delegate_->OnWriteEvent(overlapped_socket, io_ctx);

                        overlapped_socket->own_socket_ctx_->RemoveContext(io_ctx);
                    }
                    else if (io_ctx->operation_type == CONNECT_POSTED) {
                        if (parent_ && parent_->delegate_)
                            parent_->delegate_->OnConnectEvent(overlapped_socket);
                    }
                    else {
                        PPX_NOT_REACHED("");
                    }
                }
            }
        }

        OverlappedSocket::OverlappedSocket() :
            close_error_(0),
            workthread_num_(0),
            own_socket_ctx_(NULL),
            iocp_(INVALID_HANDLE_VALUE),
            exit_(true, false),
            closing_(false),
            connectex_fn_(NULL),
            acceptex_fn_(NULL),
            getacceptexsockaddrs_fn_(NULL)
            {
        }

        OverlappedSocket::~OverlappedSocket() {
            Close();
        }

        bool OverlappedSocket::CreateT(int family, int type) {
            Close();
            int proto = (SOCK_DGRAM == type) ? IPPROTO_UDP : IPPROTO_TCP;
            SOCKET s = ::WSASocket(family, type, proto, nullptr, 0, WSA_FLAG_OVERLAPPED);
            if (s == INVALID_SOCKET) {
                UpdateLastError();
                return false;
            }

            if (!InitIOCP(s)) {
                return false;
            }

            family_ = family;
            type_ = type;

            return true;
        }

        SocketAddress OverlappedSocket::GetLocalAddress() const {
            sockaddr_storage addr = { 0 };
            socklen_t addrlen = sizeof(addr);
            int result = ::getsockname(own_socket_ctx_->socket, reinterpret_cast<sockaddr*>(&addr),
                &addrlen);
            SocketAddress address;
            if (result >= 0) {
                SocketAddressFromSockAddrStorage(addr, &address);
            }
            else {
                PPX_LOG(LS_WARNING) << "GetLocalAddress: unable to get local addr, socket="
                    << own_socket_ctx_->socket;
            }
            return address;
        }

        SocketAddress OverlappedSocket::GetRemoteAddress() const {
            //sockaddr_storage addr = { 0 };
            //socklen_t addrlen = sizeof(addr);
            //int result = ::getpeername(own_socket_ctx_->socket, reinterpret_cast<sockaddr*>(&addr),
            //	&addrlen);
            //SocketAddress address;
            //if (result >= 0) {
            //	SocketAddressFromSockAddrStorage(addr, &address);
            //}
            //else {
            //	PPX_LOG(LS_WARNING)
            //		<< "GetRemoteAddress: unable to get remote addr, socket=" << own_socket_ctx_->socket << ",GLE=" << WSAGetLastError();
            //}
            //return address;
            return remote_addr_;
        }

        int OverlappedSocket::Bind(const SocketAddress & addr) {
            PPX_ASSERT(own_socket_ctx_->socket != INVALID_SOCKET);
            if (own_socket_ctx_->socket == INVALID_SOCKET)
                return SOCKET_ERROR;

            sockaddr_storage saddr;
            size_t len = addr.ToSockAddrStorage(&saddr);
            int err = ::bind(own_socket_ctx_->socket,
                reinterpret_cast<sockaddr*>(&saddr),
                static_cast<int>(len));
            UpdateLastError();
            return err;
        }

        int OverlappedSocket::Connect(const SocketAddress & addr) {
            PPX_ASSERT(own_socket_ctx_ != NULL);
            if (own_socket_ctx_ == NULL) {
                return SOCKET_ERROR;
            }

            PER_IO_CONTEXT* io_ctx = own_socket_ctx_->GetNewIoContext();
            io_ctx->socket = own_socket_ctx_->socket;
            if (!PostConnect(io_ctx, addr)) {
                own_socket_ctx_->RemoveContext(io_ctx);
                return SOCKET_ERROR;
            }
            return 0;
        }

        int OverlappedSocket::Send(const void * buffer, size_t length, PER_IO_CONTEXT* io_ctx /* = NULL*/) {
            PPX_ASSERT(own_socket_ctx_ != NULL);
            if (own_socket_ctx_ == NULL) {
                return SOCKET_ERROR;
            }

            if (!io_ctx) {
                io_ctx = own_socket_ctx_->GetNewIoContext();
                io_ctx->socket = own_socket_ctx_->socket;
            }

            if (!PostSend(io_ctx, buffer, length)) {
                own_socket_ctx_->RemoveContext(io_ctx);
                return SOCKET_ERROR;
            }
            return 0;
        }

        int OverlappedSocket::Listen(int backlog) {
            int err = ::listen(own_socket_ctx_->socket, backlog);
            UpdateLastError();
            if (err == 0)
                state_ = Socket::CS_CONNECTING;
            return err;
        }

        int OverlappedSocket::RecvFrom(const SocketAddress& addr, PER_IO_CONTEXT* io_ctx /*= NULL*/) {
            PPX_ASSERT(own_socket_ctx_ != NULL);
            if (own_socket_ctx_ == NULL) {
                return SOCKET_ERROR;
            }

            io_ctx = own_socket_ctx_->GetNewIoContext();
            io_ctx->socket = own_socket_ctx_->socket;

            if (!PostRecvFrom(io_ctx, addr)) {
                own_socket_ctx_->RemoveContext(io_ctx);
                return SOCKET_ERROR;
            }
            return 0;
        }

        int OverlappedSocket::SendTo(const void* buffer, size_t length, const SocketAddress& addr, PER_IO_CONTEXT* io_ctx /*= NULL*/) {
            PPX_ASSERT(own_socket_ctx_ != NULL);
            if (own_socket_ctx_ == NULL) {
                return SOCKET_ERROR;
            }

            io_ctx = own_socket_ctx_->GetNewIoContext();
            io_ctx->socket = own_socket_ctx_->socket;

            if (!PostSendTo(buffer, length, io_ctx, addr)) {
                own_socket_ctx_->RemoveContext(io_ctx);
                return SOCKET_ERROR;
            }

            return 0;
        }

        bool OverlappedSocket::Accept() {
            int success = 0;
            for (int i = 0; i < workthread_num_; i++) {
                PER_IO_CONTEXT *io_ctx = own_socket_ctx_->GetNewIoContext();
                if (!PostAccept(io_ctx)) {
                    own_socket_ctx_->RemoveContext(io_ctx);
                }
                else {
                    success++;
                }
            }

            return success > 0;
        }

        int OverlappedSocket::Close() {
            int err = 0;

            exit_.Set();

            for (int i = 0; i < workthread_num_; i++) {
                PostQueuedCompletionStatus(iocp_, 0, (DWORD)EXIT_CODE, NULL);
            }

            workthreads_.clear();
            workthread_num_ = 0;

            if (own_socket_ctx_) {
                if (own_socket_ctx_->socket != INVALID_SOCKET) {
                    err = ::closesocket(own_socket_ctx_->socket);
                    own_socket_ctx_->socket = INVALID_SOCKET;
                    closing_ = false;
                    close_error_ = 0;
                    UpdateLastError();
                }

                delete own_socket_ctx_;
                own_socket_ctx_ = NULL;
            }

            if (iocp_ != INVALID_HANDLE_VALUE) {
                CloseHandle(iocp_);
                iocp_ = INVALID_HANDLE_VALUE;
            }

            exit_.Reset();

            connectex_fn_ = NULL;
            acceptex_fn_ = NULL;
            getacceptexsockaddrs_fn_ = NULL;

            addr_.Clear();
            state_ = Socket::CS_CLOSED;
            return err;
        }

        int OverlappedSocket::GetOption(Socket::Option opt, int * value) {
            int slevel;
            int sopt;
            if (TranslateOption(opt, &slevel, &sopt) == -1)
                return -1;

            char* p = reinterpret_cast<char*>(value);
            int optlen = sizeof(value);
            return ::getsockopt(own_socket_ctx_->socket, slevel, sopt, p, &optlen);
        }

        int OverlappedSocket::SetOption(Socket::Option opt, const char* value, int value_len) {
            int slevel;
            int sopt;
            if (TranslateOption(opt, &slevel, &sopt) == -1)
                return -1;

            return ::setsockopt(own_socket_ctx_->socket, slevel, sopt, value, value_len);
        }

        void OverlappedSocket::Clone(OverlappedSocket *socket) {
            socket->family_ = this->family_;
            socket->type_ = this->type_;
        }

        bool OverlappedSocket::InitIOCP(SOCKET s) {
            iocp_ = CreateNewCompletionPort();
            if (iocp_ == INVALID_HANDLE_VALUE) {
                return false;
            }

            exit_.Reset();

            workthread_num_ = GetNumberOfProcesser() * 2;

            for (int i = 0; i < workthread_num_; i++) {
                std::unique_ptr<CompletionIOHandler> handler = std::make_unique<CompletionIOHandler>(this, i);
                workthreads_.push_back(std::move(handler));
            }

            own_socket_ctx_ = new PER_SOCKET_CONTEXT();
            own_socket_ctx_->socket = s;

            if (!AssociateDeviceWithCompletionPort(iocp_, (HANDLE)own_socket_ctx_->socket, (DWORD)this)) {
                Close();
                return false;
            }

            connectex_fn_ = GetConnectExFnPointer(own_socket_ctx_->socket);
            PPX_ASSERT(connectex_fn_);

            acceptex_fn_ = GetAcceptExFnPointer(own_socket_ctx_->socket);
            PPX_ASSERT(acceptex_fn_);

            getacceptexsockaddrs_fn_ = GetAcceptExSockAddrsFnPointer(own_socket_ctx_->socket);
            PPX_ASSERT(getacceptexsockaddrs_fn_);

            return true;
        }

        void OverlappedSocket::UpdateLastError() {
            error_ = WSAGetLastError();
        }

        bool OverlappedSocket::PostAccept(PER_IO_CONTEXT* io_ctx) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            int proto = (SOCK_DGRAM == type_) ? IPPROTO_UDP : IPPROTO_TCP;
            io_ctx->operation_type = ACCEPT_POSTED;
            io_ctx->ResetBuffer();
            io_ctx->socket = WSASocket(family_, type_, proto, NULL, 0, WSA_FLAG_OVERLAPPED);

            if (io_ctx->socket == INVALID_SOCKET) {
                UpdateLastError();
                return false;
            }

            DWORD bytes = 0;
            if (acceptex_fn_(own_socket_ctx_->socket,
                io_ctx->socket,
                io_ctx->wsa_buffer.buf,
                0,
                sizeof(sockaddr_storage) + 16,
                sizeof(sockaddr_storage) + 16,
                &bytes,
                &io_ctx->overlapped) == FALSE) {
                int gle = WSAGetLastError();
                if (gle != WSA_IO_PENDING) {
                    UpdateLastError();
                    return false;
                }
            }

            UpdateLastError();
            return true;
        }

        bool OverlappedSocket::PostRecv(PER_IO_CONTEXT* io_ctx) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            io_ctx->operation_type = RECV_POSTED;
            io_ctx->ResetBuffer();

            DWORD recv_bytes = 0;
            DWORD flags = 0;
            int ret = WSARecv(io_ctx->socket, &io_ctx->wsa_buffer, 1, &recv_bytes, &flags, &io_ctx->overlapped, NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                UpdateLastError();
                return false;
            }

            return true;
        }

        bool OverlappedSocket::PostSend(PER_IO_CONTEXT* io_ctx, const void* msg, size_t msg_len) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            io_ctx->operation_type = SEND_POSTED;
            memcpy(io_ctx->wsa_buffer.buf, msg, msg_len);
            io_ctx->wsa_buffer.len = msg_len;

            DWORD sent_bytes = 0;
            int ret = WSASend(io_ctx->socket, &io_ctx->wsa_buffer, 1, &sent_bytes, 0, &io_ctx->overlapped, NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                UpdateLastError();
                return false;
            }
            UpdateLastError();
            return true;
        }

        bool OverlappedSocket::PostConnect(PER_IO_CONTEXT* io_ctx, const SocketAddress& addr) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            io_ctx->operation_type = CONNECT_POSTED;
            io_ctx->ResetBuffer();

            // ConnectEx requires the socket to be initially bound.
            struct sockaddr_in addr0 = { 0 };
            addr0.sin_family = family_;
            addr0.sin_addr.s_addr = INADDR_ANY;
            addr0.sin_port = 0;
            int ret = bind(io_ctx->socket, (SOCKADDR*)&addr0, sizeof(addr0));
            if (ret != 0) {
                UpdateLastError();
                return false;
            }

            sockaddr_storage saddr;
            size_t len = addr.ToSockAddrStorage(&saddr);

            ret = connectex_fn_(io_ctx->socket,
                reinterpret_cast<const sockaddr*>(&saddr),
                len,
                NULL,
                0,
                NULL,
                &io_ctx->overlapped);
            int gle = WSAGetLastError();
            if (ret == SOCKET_ERROR && gle != WSA_IO_PENDING) {
                UpdateLastError();
                return false;
            }
            UpdateLastError();
            return true;
        }

        bool OverlappedSocket::PostRecvFrom(PER_IO_CONTEXT* io_ctx, const SocketAddress& addr) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            io_ctx->operation_type = RECV_POSTED;

            memset(&remote_addrin_, 0, sizeof(remote_addrin_));
            addr.ToSockAddr(&remote_addrin_);

            int sizein = sizeof(sockaddr);

            DWORD bytes_recv = 0;
            DWORD flags = 0;

            int ret = WSARecvFrom(io_ctx->socket,
                &io_ctx->wsa_buffer,
                1,
                &bytes_recv,
                &flags,
                reinterpret_cast<sockaddr*>(&remote_addrin_),
                &sizein,
                &io_ctx->overlapped,
                NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                UpdateLastError();
                return false;
            }
            UpdateLastError();
            return true;
        }

        bool OverlappedSocket::PostSendTo(const void* buffer, size_t length, PER_IO_CONTEXT* io_ctx, const SocketAddress& addr) {
            PPX_ASSERT(io_ctx);
            if (io_ctx == NULL)
                return false;

            PPX_ASSERT(buffer);
            if (buffer == NULL)
                return false;

            PPX_ASSERT(length > 0);
            if (length <= 0)
                return false;

            io_ctx->operation_type = SEND_POSTED;
            memcpy(io_ctx->wsa_buffer.buf, buffer, length);
            io_ctx->wsa_buffer.len = length;

            sockaddr_in addr_in;
            memset(&addr_in, 0, sizeof(addr_in));
            addr.ToSockAddr(&addr_in);

            int sizeTo = sizeof(sockaddr);

            DWORD bytes_sent = 0;
            DWORD flags = 0;
            int ret = WSASendTo(io_ctx->socket,
                &io_ctx->wsa_buffer,
                1,
                &bytes_sent,
                flags,
                reinterpret_cast<sockaddr*>(&addr_in),
                sizeTo,
                &io_ctx->overlapped,
                NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                UpdateLastError();
                return false;
            }
            UpdateLastError();
            return true;
        }

    }
}