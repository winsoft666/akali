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

#ifndef __MD5_MAKER_34DFDR7_H__
#define __MD5_MAKER_34DFDR7_H__
#pragma once

#include <string>
#include "akali_export.h"

namespace akali {
AKALI_API std::string GetStringMd5(const void *buffer, unsigned int buffer_size);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
AKALI_API std::string GetFileMd5(const std::wstring &file_path);
#else
AKALI_API std::string GetFileMd5(const std::string &file_path);
#endif
} // namespace akali
#endif