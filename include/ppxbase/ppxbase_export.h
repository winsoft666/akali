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

#ifndef PPXBASE_EXPORT_H_
#define PPXBASE_EXPORT_H_
#pragma once

#ifdef PPXBASE_STATIC
#define PPXBASE_API
#else
#if defined(PPXBASE_EXPORTS)
#if defined(_MSC_VER)
#define PPXBASE_API __declspec(dllexport)
#else
#define PPXBASE_API
#endif
#else
#if defined(_MSC_VER)
#define PPXBASE_API __declspec(dllimport)
#else
#define PPXBASE_API
#endif
#endif
#endif

#endif // !PPXBASE_EXPORT_H_