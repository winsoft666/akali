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

#ifndef AKALI_MEMORY_POOL_H_
#define AKALI_MEMORY_POOL_H_

#include <climits>
#include <cstddef>
#include <mutex>
#include <memory.h>
#include "akali_export.h"
/*
MemoryPool is mostly compliant with the C++ Standard Library allocators.
This means you can use it with `allocator_traits`
([see here] (http://www.cplusplus.com/reference/memory/allocator_traits/))
or just like you would use the `std::allocator`
([see here] (http://www.cplusplus.com/reference/memory/allocator/)).

There are some differences though:

* MemorPool **cannot** allocate multiple objects with a single call to `allocate`
and will simply ignore the count value you pass to the allocate/deallocate function.
Fixing this is not too hard, but it would deteriorate performance and create memory fragmentation.
* This is **NOT** thread safe. You should create a different instance for each thread (suggested)
or find some way of scheduling queries to the allocator.

Also see: https://blog.csdn.net/china_jeffery/article/details/80750042
*/

namespace akali {
template <typename T, size_t BlockSize = 4096, bool ZeroOnDeallocate = true> class MemoryPool {
public:
  /* Member types */
  typedef T value_type;
  typedef T *pointer;
  typedef T &reference;
  typedef const T *const_pointer;
  typedef const T &const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::false_type propagate_on_container_copy_assignment;
  typedef std::true_type propagate_on_container_move_assignment;
  typedef std::true_type propagate_on_container_swap;

  template <typename U> struct rebind { typedef MemoryPool<U> other; };

  /* Member functions */
  MemoryPool() noexcept;
  MemoryPool(const MemoryPool &memoryPool) noexcept;
  MemoryPool(MemoryPool &&memoryPool) noexcept;
  template <class U> MemoryPool(const MemoryPool<U> &memoryPool) noexcept;

  ~MemoryPool() noexcept;

  MemoryPool &operator=(const MemoryPool &memoryPool) = delete;
  MemoryPool &operator=(MemoryPool &&memoryPool) noexcept;

  pointer address(reference x) const noexcept;
  const_pointer address(const_reference x) const noexcept;

  // Can only allocate one object at a time. n and hint are ignored
  pointer allocate(size_type n = 1, const_pointer hint = 0);
  void deallocate(pointer p, size_type n = 1);

  size_type max_size() const noexcept;

  template <class U, class... Args> void construct(U *p, Args &&... args);
  template <class U> void destroy(U *p);

  template <class... Args> pointer newElement(Args &&... args);
  void deleteElement(pointer p);

private:
  struct Element_ {
    Element_ *pre;
    Element_ *next;
  };

  typedef char *data_pointer;
  typedef Element_ element_type;
  typedef Element_ *element_pointer;

  element_pointer data_element_;
  element_pointer free_element_;

  std::recursive_mutex m_;

  size_type padPointer(data_pointer p, size_type align) const noexcept;
  void allocateBlock();

  static_assert(BlockSize >= 2 * sizeof(element_type), "BlockSize too small.");
};

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::size_type
MemoryPool<T, BlockSize, ZeroOnDeallocate>::padPointer(data_pointer p, size_type align) const
    noexcept {
  uintptr_t result = reinterpret_cast<uintptr_t>(p);
  return ((align - result) % align);
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
MemoryPool<T, BlockSize, ZeroOnDeallocate>::MemoryPool() noexcept {
  data_element_ = nullptr;
  free_element_ = nullptr;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
MemoryPool<T, BlockSize, ZeroOnDeallocate>::MemoryPool(const MemoryPool &memoryPool) noexcept
    : MemoryPool() {}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
MemoryPool<T, BlockSize, ZeroOnDeallocate>::MemoryPool(MemoryPool &&memoryPool) noexcept {
  std::lock_guard<std::recursive_mutex> lock(m_);

  data_element_ = memoryPool.data_element_;
  memoryPool.data_element_ = nullptr;
  free_element_ = memoryPool.free_element_;
  memoryPool.free_element_ = nullptr;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
template <class U>
MemoryPool<T, BlockSize, ZeroOnDeallocate>::MemoryPool(const MemoryPool<U> &memoryPool) noexcept
    : MemoryPool() {}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
MemoryPool<T, BlockSize, ZeroOnDeallocate> &
MemoryPool<T, BlockSize, ZeroOnDeallocate>::operator=(MemoryPool &&memoryPool) noexcept {
  std::lock_guard<std::recursive_mutex> lock(m_);

  if (this != &memoryPool) {
    std::swap(data_element_, memoryPool.data_element_);
    std::swap(free_element_, memoryPool.free_element_);
  }
  return *this;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
MemoryPool<T, BlockSize, ZeroOnDeallocate>::~MemoryPool() noexcept {
  std::lock_guard<std::recursive_mutex> lock(m_);

  element_pointer curr = data_element_;
  while (curr != nullptr) {
    element_pointer prev = curr->next;
    operator delete(reinterpret_cast<void *>(curr));
    curr = prev;
  }

  curr = free_element_;
  while (curr != nullptr) {
    element_pointer prev = curr->next;
    operator delete(reinterpret_cast<void *>(curr));
    curr = prev;
  }
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::pointer
MemoryPool<T, BlockSize, ZeroOnDeallocate>::address(reference x) const noexcept {
  return &x;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::const_pointer
MemoryPool<T, BlockSize, ZeroOnDeallocate>::address(const_reference x) const noexcept {
  return &x;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
void MemoryPool<T, BlockSize, ZeroOnDeallocate>::allocateBlock() {
  // Allocate space for the new block and store a pointer to the previous one
  data_pointer new_block = reinterpret_cast<data_pointer>(operator new(BlockSize));
  element_pointer new_ele_pointer = reinterpret_cast<element_pointer>(new_block);
  new_ele_pointer->pre = nullptr;
  new_ele_pointer->next = nullptr;

  if (data_element_) {
    data_element_->pre = new_ele_pointer;
  }

  new_ele_pointer->next = data_element_;
  data_element_ = new_ele_pointer;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::pointer
MemoryPool<T, BlockSize, ZeroOnDeallocate>::allocate(size_type n, const_pointer hint) {
  std::lock_guard<std::recursive_mutex> lock(m_);

  if (free_element_ != nullptr) {
    data_pointer body = reinterpret_cast<data_pointer>(
        reinterpret_cast<data_pointer>(free_element_) + sizeof(element_type));

    size_type bodyPadding = padPointer(body, alignof(element_type));

    pointer result = reinterpret_cast<pointer>(reinterpret_cast<data_pointer>(body + bodyPadding));

    element_pointer tmp = free_element_;

    free_element_ = free_element_->next;

    if (free_element_)
      free_element_->pre = nullptr;

    tmp->next = data_element_;
    if (data_element_)
      data_element_->pre = tmp;
    tmp->pre = nullptr;
    data_element_ = tmp;

    return result;
  }
  else {
    allocateBlock();

    data_pointer body = reinterpret_cast<data_pointer>(
        reinterpret_cast<data_pointer>(data_element_) + sizeof(element_type));

    size_type bodyPadding = padPointer(body, alignof(element_type));

    pointer result = reinterpret_cast<pointer>(reinterpret_cast<data_pointer>(body + bodyPadding));

    return result;
  }
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline void MemoryPool<T, BlockSize, ZeroOnDeallocate>::deallocate(pointer p, size_type n) {
  std::lock_guard<std::recursive_mutex> lock(m_);

  if (p != nullptr) {
    element_pointer ele_p =
        reinterpret_cast<element_pointer>(reinterpret_cast<data_pointer>(p) - sizeof(element_type));

    if (ZeroOnDeallocate) {
      memset(reinterpret_cast<data_pointer>(p), 0, BlockSize - sizeof(element_type));
    }

    if (ele_p->pre) {
      ele_p->pre->next = ele_p->next;
    }

    if (ele_p->next) {
      ele_p->next->pre = ele_p->pre;
    }

    if (ele_p->pre == nullptr) {
      data_element_ = ele_p->next;
    }

    ele_p->pre = nullptr;
    if (free_element_) {
      ele_p->next = free_element_;
      free_element_->pre = ele_p;
    }
    else {
      ele_p->next = nullptr;
    }
    free_element_ = ele_p;
  }
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::size_type
MemoryPool<T, BlockSize, ZeroOnDeallocate>::max_size() const noexcept {
  size_type maxBlocks = -1 / BlockSize;
  return (BlockSize - sizeof(data_pointer)) / sizeof(element_type) * maxBlocks;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
template <class U, class... Args>
inline void MemoryPool<T, BlockSize, ZeroOnDeallocate>::construct(U *p, Args &&... args) {
  new (p) U(std::forward<Args>(args)...);
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
template <class U>
inline void MemoryPool<T, BlockSize, ZeroOnDeallocate>::destroy(U *p) {
  p->~U();
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
template <class... Args>
inline typename MemoryPool<T, BlockSize, ZeroOnDeallocate>::pointer
MemoryPool<T, BlockSize, ZeroOnDeallocate>::newElement(Args &&... args) {
  std::lock_guard<std::recursive_mutex> lock(m_);
  pointer result = allocate();
  construct<value_type>(result, std::forward<Args>(args)...);
  return result;
}

template <typename T, size_t BlockSize, bool ZeroOnDeallocate>
inline void MemoryPool<T, BlockSize, ZeroOnDeallocate>::deleteElement(pointer p) {
  std::lock_guard<std::recursive_mutex> lock(m_);
  if (p != nullptr) {
    p->~value_type();
    deallocate(p);
  }
}
} // namespace akali
#endif // AKALI_MEMORY_POOL_H_
