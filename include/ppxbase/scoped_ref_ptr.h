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

#pragma once

#include <atomic>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API IRefObj {
        public:
            virtual void AddRef() const = 0;
            virtual void Release() const = 0;
        };

        class PPXBASE_API RefCountedBase : public IRefObj {
        public:
            RefCountedBase() : ref_count_(0) {
            }

            void AddRef() const {
                ref_count_++;
            }

            void Release() const {
                ref_count_--;
                if (ref_count_ == 0) {
                    delete this;
                }
            }

        protected:
            virtual ~RefCountedBase() = default;

        private:
            mutable std::atomic<int> ref_count_;

            RefCountedBase(const RefCountedBase&) = delete;
            void operator=(const RefCountedBase&) = delete;
        };


        template <class T>
        class ScopedRefPtr {
        public:
            ScopedRefPtr() : m_Ptr(nullptr) {}

            ScopedRefPtr(T* p) : m_Ptr(p) {
                if (m_Ptr)
                    m_Ptr->AddRef();
            }

            ScopedRefPtr(const ScopedRefPtr<T>& r) : m_Ptr(r.m_Ptr) {
                if (m_Ptr)
                    m_Ptr->AddRef();
            }

            template <typename U>
            ScopedRefPtr(const ScopedRefPtr<U>& r) : m_Ptr(r.Get()) {
                if (m_Ptr)
                    m_Ptr->AddRef();
            }

            // Move constructors.
            ScopedRefPtr(ScopedRefPtr<T>&& r) : m_Ptr(r.Release()) {}

            template <typename U>
            ScopedRefPtr(ScopedRefPtr<U>&& r) : m_Ptr(r.Release()) {}

            ~ScopedRefPtr() {
                if (m_Ptr)
                    m_Ptr->Release();
            }

            T* Get() const { return m_Ptr; }
            operator T*() const { return m_Ptr; }
            T* operator->() const { return m_Ptr; }
            T** operator&() throw() { return &m_Ptr; }

            // Release a pointer.
            // The return value is the current pointer held by this object.
            // If this object holds a null pointer, the return value is null.
            // After this operation, this object will hold a null pointer,
            // and will not own the object any more.
            T* Release() {
                T* retVal = m_Ptr;
                m_Ptr = nullptr;
                return retVal;
            }

            ScopedRefPtr<T>& operator=(T* p) {
                // AddRef first so that self assignment should work
                if (p)
                    p->AddRef();
                if (m_Ptr)
                    m_Ptr->Release();
                m_Ptr = p;
                return *this;
            }

            ScopedRefPtr<T>& operator=(const ScopedRefPtr<T>& r) {
                return *this = r.m_Ptr;
            }

            template <typename U>
            ScopedRefPtr<T>& operator=(const ScopedRefPtr<U>& r) {
                return *this = r.Get();
            }

            ScopedRefPtr<T>& operator=(ScopedRefPtr<T>&& r) {
                ScopedRefPtr<T>(std::move(r)).Swap(*this);
                return *this;
            }

            template <typename U>
            ScopedRefPtr<T>& operator=(ScopedRefPtr<U>&& r) {
                ScopedRefPtr<T>(std::move(r)).Swap(*this);
                return *this;
            }

            void Swap(T** pp) {
                T* p = m_Ptr;
                m_Ptr = *pp;
                *pp = p;
            }

            void Swap(ScopedRefPtr<T>& r) {
                Swap(&r.m_Ptr);
            }

        protected:
            T* m_Ptr;
        };
    }
}