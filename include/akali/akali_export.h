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

#ifndef AKALI_EXPORT_H_
#define AKALI_EXPORT_H_
#pragma once

#include "akali/arch.h"

#ifdef AKALI_STATIC
#define AKALI_API
#else
#if defined(AKALI_EXPORTS)
#if defined(_MSC_VER)
#define AKALI_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define AKALI_API
#endif
#else
#if defined(_MSC_VER)
#define AKALI_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#else
#define AKALI_API
#endif
#endif
#endif

#if (defined AKALI_WIN || defined AKALI_LINUX)
#define AKALI_SUPPORT 1
#else
#define AKALI_SUPPORT 0
#endif

#endif // !AKALI_EXPORT_H_