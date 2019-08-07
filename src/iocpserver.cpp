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

#include "ppxbase/iocpserver.h"
#include "ppxbase/timeutils.h"
#include "ppxbase/logging.h"
#include "ppxbase/overlappedsocket.h"
#include "ppxbase/criticalsection.h"

namespace ppx {
    namespace base {
        IOCPServer::IOCPServer()
            : start_time_(0) {
        }

        IOCPServer::~IOCPServer() {

        }

        bool IOCPServer::Start(const SocketAddress & addr, int family, int type) {
            socket_ = new OverlappedSocket();
            PPX_ASSERT(socket_);

            socket_->RegisterDelegate(this);

            if (!socket_->CreateT(family, type)) {
                PPX_LOG(LS_WARNING) << "CreateT: failed, error=" << socket_->GetError();
                return false;
            }

            if (socket_->Bind(addr) == SOCKET_ERROR) {
                PPX_LOG(LS_WARNING) << "Bind: failed, error=" << socket_->GetError();
                return false;
            }

            if (socket_->Listen(SOMAXCONN) == SOCKET_ERROR) {
                PPX_LOG(LS_WARNING) << "Listen: failed, error=" << socket_->GetError();
                return false;
            }

            if (!socket_->Accept()) {
                PPX_LOG(LS_WARNING) << "Accept: failed, error=" << socket_->GetError();
                return false;
            }

            start_time_ = base::GetTimeStamp();

            return true;
        }

        bool IOCPServer::Stop() {
            if (socket_->Close() == SOCKET_ERROR) {
                PPX_LOG(LS_WARNING) << "Close: failed, error=" << socket_->GetError();
                return false;
            }

            {
                base::CritScope cs(&crit_);
                for (ClientList::iterator it = client_list_.begin(); it != client_list_.end(); it++) {
                    PPX_ASSERT((*it) != NULL);
                    (*it)->Close();
                    delete (*it);
                }

                client_list_.clear();
            }
            start_time_ = 0;

            delete socket_;
            socket_ = NULL;

            return true;
        }

        int64_t IOCPServer::GetStartTime() const {
            return start_time_;
        }

        void IOCPServer::OnAcceptEvent(OverlappedSocket * socket) {
            PPX_ASSERT(socket);
            PPX_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [Connected]";
            {
                base::CritScope cs(&crit_);
                client_list_.push_back(socket);
            }
        }

        void IOCPServer::OnReadEvent(OverlappedSocket * socket, const PER_IO_CONTEXT * io_ctx) {
            PPX_ASSERT(socket);
            PPX_ASSERT(io_ctx);
            PPX_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [RECV] " << io_ctx->GetBuffer();
        }

        void IOCPServer::OnWriteEvent(OverlappedSocket * socket, const PER_IO_CONTEXT * io_ctx) {
            PPX_ASSERT(socket);
            PPX_ASSERT(io_ctx);
            PPX_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [SEND] " << io_ctx->GetBuffer();
        }

        void IOCPServer::OnConnectEvent(OverlappedSocket *socket) {

        }

        void IOCPServer::OnCloseEvent(OverlappedSocket * socket, int error) {
            PPX_ASSERT(socket);
            PPX_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [Disconnected] error=" << error;
            socket->Close();
            {
                base::CritScope cs(&crit_);
                client_list_.remove(socket);
            }
            delete socket;
            socket = NULL;
        }

    }
}