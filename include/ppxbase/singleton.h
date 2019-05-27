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

#ifndef PPX_BASE_SINGLETON_H_
#define PPX_BASE_SINGLETON_H_
#pragma once

#include <mutex>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {

        template<class T>
        class Singleton {
        public:
            static T* Instance();
            static void Release();
        protected:
            Singleton() {}
            Singleton(const Singleton&) {}
            Singleton& operator=(const Singleton&) {}
        private:
            static T* this_;
            static std::mutex m_;
        };


        template<class T>
        T*  Singleton<T>::this_ = nullptr;

        template<class T>
        std::mutex Singleton<T>::m_;


        template<class T>
        T* Singleton<T>::Instance(void) {
            //double-check
            if (this_ == nullptr) {
                std::lock_guard<std::mutex> lg(m_);
                if (this_ == nullptr) {
                    this_ = new T;
                }
            }
            return this_;
        }

        template<class T>
        void Singleton<T>::Release(void) {
            if (this_) {
                delete this_;
            }
        }

#define SINGLETON_CLASS_DECLARE(class_name)	\
	friend class ::ppx::base::Singleton<##class_name>;
    }
}

#endif // !PPX_BASE_SINGLETON_H_
