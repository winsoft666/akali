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

#ifndef AKALI_IOCP_H_
#define AKALI_IOCP_H_

#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include <winsock2.h>
#include <MSWSock.h>
#include <vector>

#define MAX_BUFFER_LEN 8192
#define EXIT_CODE 0

namespace akali {
typedef enum _OPERATION_TYPE {
  ACCEPT_POSTED,
  CONNECT_POSTED,
  SEND_POSTED,
  RECV_POSTED,
  NULL_POSTED
} OPERATION_TYPE;

typedef struct _PER_IO_CONTEXT {
  OVERLAPPED overlapped;
  SOCKET socket;
  WSABUF wsa_buffer;
  char buffer[MAX_BUFFER_LEN];
  OPERATION_TYPE operation_type;

  _PER_IO_CONTEXT() {
    ZeroMemory(&overlapped, sizeof(overlapped));
    ZeroMemory(buffer, MAX_BUFFER_LEN);
    socket = INVALID_SOCKET;
    wsa_buffer.buf = buffer;
    wsa_buffer.len = MAX_BUFFER_LEN;
    operation_type = NULL_POSTED;
  }

  ~_PER_IO_CONTEXT() {
    // don't close socket
  }

  void ResetBuffer() { ZeroMemory(buffer, MAX_BUFFER_LEN); }

  const char *GetBuffer() const { return wsa_buffer.buf; }

  ULONG GetBufferLength() const { return wsa_buffer.len; }

} PER_IO_CONTEXT;

typedef struct _PER_SOCKET_CONTEXT {
  SOCKET socket;
  SOCKADDR_IN client_addr;
  std::vector<_PER_IO_CONTEXT *> io_ctx_array;

  _PER_SOCKET_CONTEXT() {
    socket = INVALID_SOCKET;
    memset(&client_addr, 0, sizeof(client_addr));
  }

  ~_PER_SOCKET_CONTEXT() {
    if (socket != INVALID_SOCKET) {
      closesocket(socket);
      socket = INVALID_SOCKET;
    }

    for (size_t i = 0; i < io_ctx_array.size(); i++) {
      delete io_ctx_array[i];
    }

    io_ctx_array.clear();
  }

  _PER_IO_CONTEXT *GetNewIoContext() {
    _PER_IO_CONTEXT *p = new _PER_IO_CONTEXT;

    io_ctx_array.push_back(p);

    return p;
  }

  void RemoveContext(_PER_IO_CONTEXT *pContext) {
    for (std::vector<_PER_IO_CONTEXT *>::iterator it = io_ctx_array.begin();
         it != io_ctx_array.end(); it++) {
      if (pContext == *it) {
        delete pContext;
        pContext = NULL;
        io_ctx_array.erase(it);
        break;
      }
    }
  }
} PER_SOCKET_CONTEXT;

AKALI_API int GetNumberOfProcesser();
AKALI_API HANDLE CreateNewCompletionPort();
AKALI_API BOOL AssociateDeviceWithCompletionPort(HANDLE completion_port, HANDLE device,
                                                   DWORD completion_key);

AKALI_API LPFN_ACCEPTEX GetAcceptExFnPointer(SOCKET s);
AKALI_API LPFN_CONNECTEX GetConnectExFnPointer(SOCKET s);
AKALI_API LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrsFnPointer(SOCKET s);
} // namespace akali
#endif
#endif // AKALI_IOCP_H_
