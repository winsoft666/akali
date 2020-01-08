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

#ifndef AKALI_SINGLETON_H_
#define AKALI_SINGLETON_H_
#pragma once

#include <mutex>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "akali/constructormagic.h"
#include "akali_export.h"

namespace akali {
template <class T> class Singleton {
public:
  static T *Instance();
  static void Release();

protected:
  Singleton() {}
  Singleton(const Singleton &) {}
  Singleton &operator=(const Singleton &) {}

private:
  static T *this_;
  static std::mutex m_;
};

template <class T> T *Singleton<T>::this_ = nullptr;

template <class T> std::mutex Singleton<T>::m_;

template <class T> T *Singleton<T>::Instance(void) {
  // double-check
  if (this_ == nullptr) {
    std::lock_guard<std::mutex> lg(m_);
    if (this_ == nullptr) {
      this_ = new T;
    }
  }
  return this_;
}

template <class T> void Singleton<T>::Release(void) {
  if (this_) {
    delete this_;
  }
}

#define SINGLETON_CLASS_DECLARE(class_name) friend class ::akali::Singleton<##class_name>;

class SingletonProcess {
public:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  SingletonProcess(const std::string &unique_name)
      : unique_name_(unique_name) {}
#else
  SingletonProcess(const std::string &unique_pid_name)
      : unique_pid_name_(unique_pid_name) {}
#endif
  ~SingletonProcess() = default;
  bool operator()() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    HANDLE mutex = CreateEventA(NULL, TRUE, FALSE, unique_name_.c_str());
    DWORD gle = GetLastError();
    bool ret = true;

    if (mutex) {
      if (gle == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        ret = false;
      }
    }
    else {
      if (gle == ERROR_ACCESS_DENIED)
        ret = false;
    }

    return ret;
#else
    int pid_file = open("/tmp/" + unique_pid_name_ + ".pid", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if (rc) {
      if (EWOULDBLOCK == errno)
        return false;
    }
    return true;
#endif
  }

private:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  std::string unique_name_;
#else
  std::string unique_pid_name_;
#endif
  AKALI_DISALLOW_IMPLICIT_CONSTRUCTORS(SingletonProcess);
};

} // namespace akali
#endif // !AKALI_SINGLETON_H_
