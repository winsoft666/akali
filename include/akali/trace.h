#ifndef AKALI_TRACE_H__
#define AKALI_TRACE_H__
#pragma once

#include "akali/akali_export.h"

namespace akali {
AKALI_API void TraceMsgA(const char *lpFormat, ...);
AKALI_API void TraceMsgW(const wchar_t *lpFormat, ...);
} // namespace akali

#endif // !AKALI_TRACE_H__