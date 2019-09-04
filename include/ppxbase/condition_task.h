#pragma once

#include <ppltasks.h>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        PPXBASE_API Concurrency::task<void> RunAfter(unsigned int timeout_ms);
    }
}