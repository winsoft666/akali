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

#ifndef AKALI_SOCKET_H_
#define AKALI_SOCKET_H_

#include <errno.h>
#include <assert.h>
#include "akali/constructormagic.h"
#include "akali/socketaddress.h"
#include "akali_export.h"

namespace akali {
AKALI_API inline bool IsBlockingError(int e) {
  return (e == EWOULDBLOCK) || (e == EAGAIN) || (e == EINPROGRESS);
}

class AKALI_API Socket {
public:
  virtual ~Socket() {}

  enum ConnState { CS_CLOSED, CS_CONNECTING, CS_CONNECTED };

  virtual ConnState GetState() const { return state_; }

  virtual int GetError() const { return error_; }

  virtual void SetError(int error) { error_ = error; }

  enum Option {
    OPT_DONTFRAGMENT,
    OPT_RCVBUF,               // receive buffer size
    OPT_SNDBUF,               // send buffer size
    OPT_NODELAY,              // whether Nagle algorithm is enabled
    OPT_IPV6_V6ONLY,          // Whether the socket is IPv6 only.
    OPT_DSCP,                 // DSCP code
    OPT_RTP_SENDTIME_EXTN_ID, // This is a non-traditional socket option param.
    // This is specific to libjingle and will be used
    // if SendTime option is needed at socket level.
    OPT_BROADCAST,
    OPT_ADD_MEMBERSHIP
  };

protected:
  Socket() : error_(0), connect_time_(0), state_(Socket::CS_CLOSED), family_(AF_UNSPEC), type_(0) {
    error_ = 0;
    connect_time_ = 0;
  }

  int family_;
  int type_;
  Socket::ConnState state_;
  int error_;
  int64_t connect_time_;

private:
  AKALI_DISALLOW_COPY_AND_ASSIGN(Socket);
};
} // namespace akali
#endif