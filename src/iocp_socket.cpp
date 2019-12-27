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

#include "akali/iocp_socket.h"

#if (defined _WIN32 || defined WIN32)

namespace akali {
int GetNumberOfProcesser() {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return si.dwNumberOfProcessors;
}

HANDLE CreateNewCompletionPort() {
  return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE completion_port, HANDLE device,
                                       DWORD completion_key) {
  HANDLE h = CreateIoCompletionPort(device, completion_port, completion_key, 0);
  return (h == completion_port);
}

LPFN_ACCEPTEX GetAcceptExFnPointer(SOCKET s) {
  LPFN_ACCEPTEX fn = NULL;
  GUID GuidAcceptEx = WSAID_ACCEPTEX;
  DWORD bytes = 0;

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,
                               sizeof(GuidAcceptEx), &fn, sizeof(fn), &bytes, NULL, NULL)) {
    return NULL;
  }

  return fn;
}

LPFN_CONNECTEX GetConnectExFnPointer(SOCKET s) {
  LPFN_CONNECTEX fn = NULL;
  GUID GuidConnectEx = WSAID_CONNECTEX;
  DWORD bytes = 0;

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx,
                               sizeof(GuidConnectEx), &fn, sizeof(fn), &bytes, NULL, NULL)) {
    return NULL;
  }

  return fn;
}

LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrsFnPointer(SOCKET s) {
  LPFN_GETACCEPTEXSOCKADDRS fn = NULL;
  GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
  DWORD bytes = 0;

  if (SOCKET_ERROR == WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockAddrs,
                               sizeof(GuidGetAcceptExSockAddrs), &fn, sizeof(fn), &bytes, NULL,
                               NULL)) {
    return NULL;
  }

  return fn;
}
} // namespace akali
#endif