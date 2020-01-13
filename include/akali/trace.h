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

#ifndef AKALI_TRACE_H__
#define AKALI_TRACE_H__
#pragma once

#include "akali/akali_export.h"

namespace akali {
AKALI_API void TraceMsgA(const char* lpFormat, ...);
AKALI_API void TraceMsgW(const wchar_t* lpFormat, ...);
}  // namespace akali

#endif  // !AKALI_TRACE_H__