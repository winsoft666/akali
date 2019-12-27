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

#include "akali/iocpserver.h"
#if (defined _WIN32 || defined WIN32)
#include "akali/timeutils.h"
#include "akali/logging.h"
#include "akali/overlappedsocket.h"
#include "akali/criticalsection.h"

namespace akali {
IOCPServer::IOCPServer() : start_time_(0) {}

IOCPServer::~IOCPServer() {}

bool IOCPServer::Start(const SocketAddress &addr, int family, int type) {
  socket_ = new OverlappedSocket();
  AKALI_ASSERT(socket_);

  socket_->RegisterDelegate(this);

  if (!socket_->CreateT(family, type)) {
    AKALI_LOG(LS_WARNING) << "CreateT: failed, error=" << socket_->GetError();
    return false;
  }

  if (socket_->Bind(addr) == SOCKET_ERROR) {
    AKALI_LOG(LS_WARNING) << "Bind: failed, error=" << socket_->GetError();
    return false;
  }

  if (socket_->Listen(SOMAXCONN) == SOCKET_ERROR) {
    AKALI_LOG(LS_WARNING) << "Listen: failed, error=" << socket_->GetError();
    return false;
  }

  if (!socket_->Accept()) {
    AKALI_LOG(LS_WARNING) << "Accept: failed, error=" << socket_->GetError();
    return false;
  }

  start_time_ = GetTimeStamp();

  return true;
}

bool IOCPServer::Stop() {
  if (socket_->Close() == SOCKET_ERROR) {
    AKALI_LOG(LS_WARNING) << "Close: failed, error=" << socket_->GetError();
    return false;
  }

  {
    CritScope cs(&crit_);
    for (ClientList::iterator it = client_list_.begin(); it != client_list_.end(); it++) {
      AKALI_ASSERT((*it) != NULL);
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

int64_t IOCPServer::GetStartTime() const { return start_time_; }

void IOCPServer::OnAcceptEvent(OverlappedSocket *socket) {
  AKALI_ASSERT(socket);
  AKALI_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [Connected]";
  {
    CritScope cs(&crit_);
    client_list_.push_back(socket);
  }
}

void IOCPServer::OnReadEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) {
  AKALI_ASSERT(socket);
  AKALI_ASSERT(io_ctx);
  AKALI_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [RECV] "
                   << io_ctx->GetBuffer();
}

void IOCPServer::OnWriteEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) {
  AKALI_ASSERT(socket);
  AKALI_ASSERT(io_ctx);
  AKALI_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString() << "] [SEND] "
                   << io_ctx->GetBuffer();
}

void IOCPServer::OnConnectEvent(OverlappedSocket *socket) {}

void IOCPServer::OnCloseEvent(OverlappedSocket *socket, int error) {
  AKALI_ASSERT(socket);
  AKALI_LOG(LS_INFO) << "[" << socket->GetRemoteAddress().ToString()
                   << "] [Disconnected] error=" << error;
  socket->Close();
  {
    CritScope cs(&crit_);
    client_list_.remove(socket);
  }
  delete socket;
  socket = NULL;
}
} // namespace akali
#endif