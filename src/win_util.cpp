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

#include "ppxbase/win_util.h"

#ifdef _WIN32
#include <io.h>
#include "ppxbase/safe_release_macro.h"
#include "ppxbase/stringencode.h"

namespace ppx {
    namespace base {
        double RoundEx(double v, int digits) {
            return floor(v * pow(10.f, digits) + 0.5) / pow(10.f, digits);
        }

        bool FloatEqual(const float &f1, const float &f2) {
            const float EPSINON = 0.000001f;
            if (abs(f1 - f2) <= EPSINON) {
                return true;
            }

            return false;
        }

        bool FloatNotEqual(const float &f1, const float &f2) {
            return !FloatEqual(f1, f2);
        }
    }
}

#endif // _WIN32