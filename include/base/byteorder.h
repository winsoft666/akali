/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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


#ifndef PPX_BASE_BYTE_ORDER_H_
#define PPX_BASE_BYTE_ORDER_H_
#pragma once

#include <stdlib.h>
#include <winsock2.h>
#include "base/endianess_detect.h"
#include "ppx_export.h"

namespace ppx {
    namespace base {
    #define htobe16(v) htons(v)
    #define htobe32(v) htonl(v)
    #define be16toh(v) ntohs(v)
    #define be32toh(v) ntohl(v)
#if _WIN32_WINNT>=0x0602 || WINVER>=0x0602 // Win8
    #define htobe64(v) htonll((v))
    #define be64toh(v) ntohll((v))
#endif

    #if defined(ARCH_CPU_LITTLE_ENDIAN)
        #define htole16(v) (v)
        #define htole32(v) (v)
        #define htole64(v) (v)
        #define le16toh(v) (v)
        #define le32toh(v) (v)
        #define le64toh(v) (v)

    #elif defined(ARCH_CPU_BIG_ENDIAN)
        #define htole16(v) __builtin_bswap16(v)
        #define htole32(v) __builtin_bswap32(v)
        #define htole64(v) __builtin_bswap64(v)
        #define le16toh(v) __builtin_bswap16(v)
        #define le32toh(v) __builtin_bswap32(v)
        #define le64toh(v) __builtin_bswap64(v)
    #else
        #error ARCH_CPU_BIG_ENDIAN or ARCH_CPU_LITTLE_ENDIAN must be defined.
    #endif  // defined(ARCH_CPU_LITTLE_ENDIAN)

        // Reading and writing of little and big-endian numbers from memory
        PPX_API void Set8(void *memory, size_t offset, uint8_t v);

        PPX_API uint8_t Get8(const void *memory, size_t offset);

        PPX_API void SetBE16(void *memory, uint16_t v);

        PPX_API void SetBE32(void *memory, uint32_t v);
#if _WIN32_WINNT>=0x0602 || WINVER>=0x0602 // Win8
        PPX_API void SetBE64(void *memory, uint64_t v);
        PPX_API uint64_t GetBE64(const void *memory);
        PPX_API uint64_t HostToNetwork64(uint64_t n);
        PPX_API uint64_t NetworkToHost64(uint64_t n);
#endif
        PPX_API void SetLE64(void *memory, uint64_t v);
        PPX_API uint16_t GetBE16(const void *memory);

        PPX_API uint32_t GetBE32(const void *memory);

        PPX_API void SetLE16(void *memory, uint16_t v);

        PPX_API void SetLE32(void *memory, uint32_t v);

        PPX_API uint16_t GetLE16(const void *memory);

        PPX_API uint32_t GetLE32(const void *memory);

        PPX_API uint64_t GetLE64(const void *memory);

        PPX_API uint16_t HostToNetwork16(uint16_t n);

        PPX_API uint32_t HostToNetwork32(uint32_t n);

        PPX_API uint16_t NetworkToHost16(uint16_t n);

        PPX_API uint32_t NetworkToHost32(uint32_t n);

    }
}

#endif // !PPX_BASE_BYTE_ORDER_H_