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

#ifndef AKALI_BYTE_ORDER_H_
#define AKALI_BYTE_ORDER_H_
#pragma once
#include "akali/akali_export.h"

#include <stdlib.h>
#include <stddef.h> // for NULL, size_t
#include <stdint.h> // for uintptr_t and (u)int_t types.
#ifdef AKALI_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#if (defined __APPLE__)
#include <libkern/OSByteOrder.h>
#endif

#include "akali/endianess_detect.h"

#ifdef AKALI_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

namespace akali {
#ifdef AKALI_WIN
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)
#elif (defined AKALI_MACOS)
#define htobe16(v) OSSwapHostToBigInt16(v)
#define htobe32(v) OSSwapHostToBigInt32(v)
#define htobe64(v) OSSwapHostToBigInt64(v)
#define be16toh(v) OSSwapBigToHostInt16(v)
#define be32toh(v) OSSwapBigToHostInt32(v)
#define be64toh(v) OSSwapBigToHostInt64(v)

#define htole16(v) OSSwapHostToLittleInt16(v)
#define htole32(v) OSSwapHostToLittleInt32(v)
#define htole64(v) OSSwapHostToLittleInt64(v)
#define le16toh(v) OSSwapLittleToHostInt16(v)
#define le32toh(v) OSSwapLittleToHostInt32(v)
#define le64toh(v) OSSwapLittleToHostInt64(v)
#endif

#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602 // Win8
#define htobe64(v) htonll((v))
#define be64toh(v) ntohll((v))
#endif

#if defined(AKALI_ARCH_LITTLE_ENDIAN)
#define htole16(v) (v)
#define htole32(v) (v)
#define htole64(v) (v)
#define le16toh(v) (v)
#define le32toh(v) (v)
#define le64toh(v) (v)
#elif defined(AKALI_ARCH_BIG_ENDIAN)
#define htole16(v) __builtin_bswap16(v)
#define htole32(v) __builtin_bswap32(v)
#define htole64(v) __builtin_bswap64(v)
#define le16toh(v) __builtin_bswap16(v)
#define le32toh(v) __builtin_bswap32(v)
#define le64toh(v) __builtin_bswap64(v)
#endif

// Reading and writing of little and big-endian numbers from memory
AKALI_API void Set8(void *memory, size_t offset, uint8_t v);

AKALI_API uint8_t Get8(const void *memory, size_t offset);

AKALI_API void SetBE16(void *memory, uint16_t v);

AKALI_API void SetBE32(void *memory, uint32_t v);
#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602 // Win8
AKALI_API void SetBE64(void *memory, uint64_t v);
AKALI_API uint64_t GetBE64(const void *memory);
AKALI_API uint64_t HostToNetwork64(uint64_t n);
AKALI_API uint64_t NetworkToHost64(uint64_t n);
#endif
AKALI_API void SetLE64(void *memory, uint64_t v);
AKALI_API uint16_t GetBE16(const void *memory);

AKALI_API uint32_t GetBE32(const void *memory);

AKALI_API void SetLE16(void *memory, uint16_t v);

AKALI_API void SetLE32(void *memory, uint32_t v);

AKALI_API uint16_t GetLE16(const void *memory);

AKALI_API uint32_t GetLE32(const void *memory);

AKALI_API uint64_t GetLE64(const void *memory);

AKALI_API uint16_t HostToNetwork16(uint16_t n);

AKALI_API uint32_t HostToNetwork32(uint32_t n);

AKALI_API uint16_t NetworkToHost16(uint16_t n);

AKALI_API uint32_t NetworkToHost32(uint32_t n);

} // namespace akali

#endif // !AKALI_BYTE_ORDER_H_