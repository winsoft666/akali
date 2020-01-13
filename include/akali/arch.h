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

#ifndef AKALI_ARCH_H__
#define AKALI_ARCH_H__
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define AKALI_WIN
#ifdef _WIN64
#else
#endif
#elif __APPLE__
#define AKALI_MACOS
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#elif TARGET_OS_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
#define AKALI_LINUX
#elif __unix__
#define AKALI_UNIX
#elif defined(_POSIX_VERSION)
#define AKALI_POSIX
#else
#error "Unknown compiler"
#endif

// Processor architecture detection.
// For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define AKALI_ARCH_X86_FAMILY
#define AKALI_ARCH_X86_64
#define AKALI_ARCH_64_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(_M_ARM64) || defined(__aarch64__)
#define AKALI_ARCH_ARM_FAMILY
#define AKALI_ARCH_64_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(_M_IX86) || defined(__i386__)
#define AKALI_ARCH_X86_FAMILY
#define AKALI_ARCH_X86
#define AKALI_ARCH_32_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(__ARMEL__)
#define AKALI_ARCH_ARM_FAMILY
#define AKALI_ARCH_32_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(__MIPSEL__)
#define AKALI_ARCH_MIPS_FAMILY
#if defined(__LP64__)
#define AKALI_ARCH_64_BITS
#else
#define AKALI_ARCH_32_BITS
#endif
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(__pnacl__)
#define AKALI_ARCH_32_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#elif defined(__EMSCRIPTEN__)
#define AKALI_ARCH_32_BITS
#define AKALI_ARCH_LITTLE_ENDIAN
#else
#error Please add support for your architecture in rtc_base/system/arch.h
#endif

#if !(defined(AKALI_ARCH_LITTLE_ENDIAN) ^ defined(AKALI_ARCH_BIG_ENDIAN))
#error Define either AKALI_ARCH_LITTLE_ENDIAN or AKALI_ARCH_BIG_ENDIAN
#endif

#endif  // !AKALI_ARCH_H__