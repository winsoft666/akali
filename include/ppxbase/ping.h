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

#ifndef PPX_BASE_PING_H_
#define PPX_BASE_PING_H_
#pragma once

#include <string>
#include <functional>
#include <thread>
#include <vector>
#include "ppxbase/ipaddress.h"
#include "ppxbase/networkprotocoldef.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API Ping {
          public:
            typedef struct _PingRsp {
                int icmp_seq;

                int sent_bytes;

                int recved_bytes;
                IPAddress from_ip;

                int used_time_ms;
                int ttl;

                _PingRsp() {
                    icmp_seq = -1;
                    sent_bytes = -1;
                    recved_bytes = -1;
                    used_time_ms = -1;
                    ttl = -1;
                }

            } PingRsp;

            Ping(
                int packet_size = 32,
                int send_timeout_ms = 3000,
                int recv_timeout_ms = 3000,
                int ttl = 128
            );

            virtual ~Ping();

            bool SyncPing(const IPAddress &ip, unsigned short times, std::vector<PingRsp> &rsps);
          protected:
            void FillPingPacket(__u8 *icmp_packet, __u16 seq, __u16 icmp_packet_size);
            bool DecodeIPPacket(__u8 *ip_packet, __u16 packet_size, PingRsp &rsp);
          private:
            int ttl_;
            int packet_size_;
            int send_timeout_ms_;
            int recv_timeout_ms_;
        };
    }
}

#endif //!PPX_BASE_PING_H_