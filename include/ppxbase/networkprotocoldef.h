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

#ifndef PPX_BASE_NETWORK_PROTOCOLDEF_H__
#define PPX_BASE_NETWORK_PROTOCOLDEF_H__
#pragma once

#include "ppxbase/endianess_detect.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {

        // in Unix os, see ip.h

#ifdef WIN32
#define __u8 unsigned char
#define __u16 unsigned short
#define __u32 unsigned long

#pragma pack(1)

        // See: http://blog.csdn.net/china_jeffery/article/details/78984477#t0
        //
        struct iphdr {
#if defined ARCH_CPU_LITTLE_ENDIAN
            __u8 ihl : 4,
                 version : 4;
#elif defined ARCH_CPU_BIG_ENDIAN
            __u8 version : 4,
                 ihl : 4;
#else
#error "please fix <rtc_base/basictypes.h>"
#endif
            __u8 tos;
            __u16 tot_len;
            __u16 id;
            __u16 frag_off;
            __u8 ttl;
            __u8 protocol;
            __u16 check;
            __u32 saddr;
            __u32 daddr;
            /*The options start here. */
        };

        // See: http://blog.csdn.net/china_jeffery/article/details/79045630
        //
        struct icmp_common_hdr {
            __u8 type;
            __u8 code;
            __u16 check;
            /*Other content start here. */
        };

        struct ping_hdr {
            icmp_common_hdr common_hdr;
            __u16 id;
            __u16 seq;
        };

#pragma pack()

#endif

        // See: http://blog.csdn.net/china_jeffery/article/details/78984477#t2
        //
        PPXBASE_API __u16 GetCheckSum(__u16 *header, __u32 size);
    }
}

#endif // !PPX_BASE_NETWORK_PROTOCOLDEF_H__
