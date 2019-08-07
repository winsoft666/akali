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

#ifndef PPX_BASE_SOCKET_H_
#define PPX_BASE_SOCKET_H_

#include <errno.h>
#include "ppxbase/constructormagic.h"
#include "ppxbase/socketaddress.h"
#include "ppxbase/assert.h"
#include "ppxbase/logging.h"
#include "ppxbase_export.h"

// Rather than converting errors into a private namespace,
// Reuse the POSIX socket api errors. Note this depends on
// Win32 compatibility.
#if defined(WIN32)
    #undef EWOULDBLOCK  // Remove errno.h's definition for each macro below.
    #define EWOULDBLOCK WSAEWOULDBLOCK
    #undef EINPROGRESS
    #define EINPROGRESS WSAEINPROGRESS
    #undef EALREADY
    #define EALREADY WSAEALREADY
    #undef ENOTSOCK
    #define ENOTSOCK WSAENOTSOCK
    #undef EDESTADDRREQ
    #define EDESTADDRREQ WSAEDESTADDRREQ
    #undef EMSGSIZE
    #define EMSGSIZE WSAEMSGSIZE
    #undef EPROTOTYPE
    #define EPROTOTYPE WSAEPROTOTYPE
    #undef ENOPROTOOPT
    #define ENOPROTOOPT WSAENOPROTOOPT
    #undef EPROTONOSUPPORT
    #define EPROTONOSUPPORT WSAEPROTONOSUPPORT
    #undef ESOCKTNOSUPPORT
    #define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
    #undef EOPNOTSUPP
    #define EOPNOTSUPP WSAEOPNOTSUPP
    #undef EPFNOSUPPORT
    #define EPFNOSUPPORT WSAEPFNOSUPPORT
    #undef EAFNOSUPPORT
    #define EAFNOSUPPORT WSAEAFNOSUPPORT
    #undef EADDRINUSE
    #define EADDRINUSE WSAEADDRINUSE
    #undef EADDRNOTAVAIL
    #define EADDRNOTAVAIL WSAEADDRNOTAVAIL
    #undef ENETDOWN
    #define ENETDOWN WSAENETDOWN
    #undef ENETUNREACH
    #define ENETUNREACH WSAENETUNREACH
    #undef ENETRESET
    #define ENETRESET WSAENETRESET
    #undef ECONNABORTED
    #define ECONNABORTED WSAECONNABORTED
    #undef ECONNRESET
    #define ECONNRESET WSAECONNRESET
    #undef ENOBUFS
    #define ENOBUFS WSAENOBUFS
    #undef EISCONN
    #define EISCONN WSAEISCONN
    #undef ENOTCONN
    #define ENOTCONN WSAENOTCONN
    #undef ESHUTDOWN
    #define ESHUTDOWN WSAESHUTDOWN
    #undef ETOOMANYREFS
    #define ETOOMANYREFS WSAETOOMANYREFS
    #undef ETIMEDOUT
    #define ETIMEDOUT WSAETIMEDOUT
    #undef ECONNREFUSED
    #define ECONNREFUSED WSAECONNREFUSED
    #undef ELOOP
    #define ELOOP WSAELOOP
    #undef ENAMETOOLONG
    #define ENAMETOOLONG WSAENAMETOOLONG
    #undef EHOSTDOWN
    #define EHOSTDOWN WSAEHOSTDOWN
    #undef EHOSTUNREACH
    #define EHOSTUNREACH WSAEHOSTUNREACH
    #undef ENOTEMPTY
    #define ENOTEMPTY WSAENOTEMPTY
    #undef EPROCLIM
    #define EPROCLIM WSAEPROCLIM
    #undef EUSERS
    #define EUSERS WSAEUSERS
    #undef EDQUOT
    #define EDQUOT WSAEDQUOT
    #undef ESTALE
    #define ESTALE WSAESTALE
    #undef EREMOTE
    #define EREMOTE WSAEREMOTE
    #undef EACCES
    #define SOCKET_EACCES WSAEACCES
#endif  // WIN32


namespace ppx {
    namespace base {

        PPXBASE_API inline bool IsBlockingError(int e) {
            return (e == EWOULDBLOCK) || (e == EAGAIN) || (e == EINPROGRESS);
        }

        struct SentPacket {
            SentPacket() : packet_id(-1), send_time_ms(-1) {}
            SentPacket(int packet_id, int64_t send_time_ms)
                : packet_id(packet_id), send_time_ms(send_time_ms) {
            }

            int packet_id;
            int64_t send_time_ms;
        };

        // General interface for the socket implementations of various networks.  The
        // methods match those of normal UNIX sockets very closely.
        class PPXBASE_API Socket {
          public:
            virtual ~Socket() {}


            enum ConnState {
                CS_CLOSED,
                CS_CONNECTING,
                CS_CONNECTED
            };

            virtual ConnState GetState() const {
                return state_;
            }

            virtual int GetError() const {
                return error_;
            }

            virtual void SetError(int error) {
                error_ = error;
            }


            enum Option {
                OPT_DONTFRAGMENT,
                OPT_RCVBUF,      // receive buffer size
                OPT_SNDBUF,      // send buffer size
                OPT_NODELAY,     // whether Nagle algorithm is enabled
                OPT_IPV6_V6ONLY, // Whether the socket is IPv6 only.
                OPT_DSCP,        // DSCP code
                OPT_RTP_SENDTIME_EXTN_ID,  // This is a non-traditional socket option param.
                // This is specific to libjingle and will be used
                // if SendTime option is needed at socket level.
                OPT_BROADCAST,
                OPT_ADD_MEMBERSHIP
            };


            static int TranslateOption(Option opt, int* slevel, int* sopt) {
                switch (opt) {
                case OPT_DONTFRAGMENT:
                    *slevel = IPPROTO_IP;
                    *sopt = IP_DONTFRAGMENT;
                    break;
                case OPT_RCVBUF:
                    *slevel = SOL_SOCKET;
                    *sopt = SO_RCVBUF;
                    break;
                case OPT_SNDBUF:
                    *slevel = SOL_SOCKET;
                    *sopt = SO_SNDBUF;
                    break;
                case OPT_NODELAY:
                    *slevel = IPPROTO_TCP;
                    *sopt = TCP_NODELAY;
                    break;
                case OPT_DSCP:
                    PPX_LOG(LS_WARNING) << "Socket::OPT_DSCP not supported.";
                    return -1;
                case OPT_BROADCAST:
                    *slevel = SOL_SOCKET;
                    *sopt = SO_BROADCAST;
                    break;
                case OPT_ADD_MEMBERSHIP:
                    *slevel = IPPROTO_IP;
                    *sopt = IP_ADD_MEMBERSHIP;
                    break;
                default:
                    PPX_NOT_REACHED("");
                    return -1;
                }
                return 0;
            }

          protected:
            Socket() : error_(0), 
                connect_time_(0), 
                state_(Socket::CS_CLOSED),
                family_(AF_UNSPEC),
                type_(0) {
                error_ = 0;
                connect_time_ = 0;
            }

            int family_;
            int type_;
            Socket::ConnState state_;
            int error_;
            int64_t connect_time_;
          private:
            PPX_DISALLOW_COPY_AND_ASSIGN(Socket);
        };

    }
}

#endif
