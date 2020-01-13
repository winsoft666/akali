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

#ifndef AKALI_NETWORK_PROTOCOLDEF_H__
#define AKALI_NETWORK_PROTOCOLDEF_H__
#pragma once

#include "akali/endianess_detect.h"
#include "akali/akali_export.h"

namespace akali {
// in Unix os, see ip.h

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned long __u32;

#pragma pack(1)

// See: http://blog.csdn.net/china_jeffery/article/details/78984477#t0
//
struct iphdr {
#if defined AKALI_ARCH_LITTLE_ENDIAN
  __u8 ihl : 4, version : 4;
#elif defined AKALI_ARCH_BIG_ENDIAN
  __u8 version : 4, ihl : 4;
#else
#error "please fixed"
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

// See: http://blog.csdn.net/china_jeffery/article/details/78984477#t2
//
AKALI_API __u16 GetCheckSum(__u16* header, __u32 size);
}  // namespace akali
#endif  // !AKALI_NETWORK_PROTOCOLDEF_H__
