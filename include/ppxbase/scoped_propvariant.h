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

#ifndef PPXBASE_SCOPED_PROPVARIANT_H_
#define PPXBASE_SCOPED_PROPVARIANT_H_
#if (defined _WIN32 || defined WIN32)
#include <propidl.h>
#include "ppxbase/assert.h"
#include "ppxbase/constructormagic.h"

namespace ppx {
    namespace base {

        // A PROPVARIANT that is automatically initialized and cleared upon respective
        // construction and destruction of this class.
        class ScopedPropVariant {
          public:
            ScopedPropVariant() {
                PropVariantInit(&pv_);
            }

            ~ScopedPropVariant() {
                Reset();
            }

            // Returns a pointer to the underlying PROPVARIANT for use as an out param in
            // a function call.
            PROPVARIANT *Receive() {
                PPX_ASSERT(pv_.vt == VT_EMPTY);
                return &pv_;
            }

            // Clears the instance to prepare it for re-use (e.g., via Receive).
            void Reset() {
                if (pv_.vt != VT_EMPTY) {
                    HRESULT result = PropVariantClear(&pv_);
                    PPX_ASSERT(result == S_OK);
                }
            }

            const PROPVARIANT &get() const {
                return pv_;
            }

            const PROPVARIANT *operator&() const {
                return &pv_;
            }

          private:
            PROPVARIANT pv_;

            // Comparison operators for ScopedPropVariant are not supported at this point.
            bool operator==(const ScopedPropVariant &) const;
            bool operator!=(const ScopedPropVariant &) const;
            PPX_DISALLOW_COPY_AND_ASSIGN(ScopedPropVariant);
        };

    }  // namespace win
}  // namespace base

#endif

#endif  // BASE_WIN_SCOPED_PROPVARIANT_H_