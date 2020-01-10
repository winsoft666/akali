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
#ifdef AKALI_WIN
#include "akali/timeutils.h"
#include "akali/overlappedsocket.h"
#include "akali/criticalsection.h"

namespace akali {
IOCPServer::IOCPServer() : start_time_(0) {}

IOCPServer::~IOCPServer() {}

bool IOCPServer::Start(const SocketAddress &addr, int family, int type) {
  socket_ = new OverlappedSocket();
  assert(socket_);

  socket_->RegisterDelegate(this);

  if (!socket_->CreateT(family, type)) {
    return false;
  }

  if (socket_->Bind(addr) == SOCKET_ERROR) {
    return false;
  }

  if (socket_->Listen(SOMAXCONN) == SOCKET_ERROR) {
    return false;
  }

  if (!socket_->Accept()) {
    return false;
  }

  start_time_ = GetTimeStamp();

  return true;
}

bool IOCPServer::Stop() {
  if (socket_->Close() == SOCKET_ERROR) {
    return false;
  }

  {
    CritScope cs(&crit_);
    for (ClientList::iterator it = client_list_.begin(); it != client_list_.end(); it++) {
      assert((*it) != NULL);
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
  assert(socket);
  {
    CritScope cs(&crit_);
    client_list_.push_back(socket);
  }
}

void IOCPServer::OnReadEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) {
  assert(socket);
  assert(io_ctx);
}

void IOCPServer::OnWriteEvent(OverlappedSocket *socket, const PER_IO_CONTEXT *io_ctx) {
  assert(socket);
  assert(io_ctx);
}

void IOCPServer::OnConnectEvent(OverlappedSocket *socket) {}

void IOCPServer::OnCloseEvent(OverlappedSocket *socket, int error) {
  assert(socket);
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