﻿#ifndef BASE_CALLBACK_INTERNAL_H_
#define BASE_CALLBACK_INTERNAL_H_
#pragma once

#include "ppxbase/callback_forward.h"
#include "ppxbase/macros.h"
#include "ppxbase/constructormagic.h"
#include "scoped_ref_ptr.h"
#include "ppxbase_export.h"

namespace ppx {
    namespace base {

        struct FakeBindState;

        namespace Internal {

            class CallbackBase;
            class CallbackBaseCopyable;

            class BindStateBase;

            template <typename Functor, typename... BoundArgs>
            struct BindState;

            struct BindStateBaseRefCountTraits {
                static void Destruct(const BindStateBase *);
            };

            template <typename T, bool IsScalar = std::is_scalar<T>::value>
            struct PassingTraits;

            template <typename T>
            struct PassingTraits<T, false> {
                using Type = T &&;
            };

            template <typename T>
            struct PassingTraits<T, true> {
                using Type = T;
            };

            template <typename T>
            using PassingTraitsType = typename PassingTraits<T>::Type;

            // BindStateBase is used to provide an opaque handle that the Callback
            // class can use to represent a function object with bound arguments.  It
            // behaves as an existential type that is used by a corresponding
            // DoInvoke function to perform the function execution.  This allows
            // us to shield the Callback class from the types of the bound argument via
            // "type erasure."
            // At the base level, the only task is to add reference counting data. Don't use
            // RefCountedThreadSafe since it requires the destructor to be a virtual method.
            // Creating a vtable for every BindState template instantiation results in a lot
            // of bloat. Its only task is to call the destructor which can be done with a
            // function pointer.
            class PPXBASE_API BindStateBase
                : public RefCountedBase {
              public:
                using InvokeFuncStorage = void(*)();

              private:
                BindStateBase(InvokeFuncStorage polymorphic_invoke,
                              void(*destructor)(const BindStateBase *));
                BindStateBase(InvokeFuncStorage polymorphic_invoke,
                              void(*destructor)(const BindStateBase *),
                              bool(*is_cancelled)(const BindStateBase *));

                ~BindStateBase() = default;

                friend struct BindStateBaseRefCountTraits;

                friend class CallbackBase;
                friend class CallbackBaseCopyable;

                // Whitelist subclasses that access the destructor of BindStateBase.
                template <typename Functor, typename... BoundArgs>
                friend struct BindState;
                friend struct base::FakeBindState;

                bool IsCancelled() const {
                    return is_cancelled_(this);
                }

                // In C++, it is safe to cast function pointers to function pointers of
                // another type. It is not okay to use void*. We create a InvokeFuncStorage
                // that that can store our function pointer, and then cast it back to
                // the original type on usage.
                InvokeFuncStorage polymorphic_invoke_;

                // Pointer to a function that will properly destroy |this|.
                void(*destructor_)(const BindStateBase *);
                bool(*is_cancelled_)(const BindStateBase *);

                PPX_DISALLOW_COPY_AND_ASSIGN(BindStateBase);
            };

            // Holds the Callback methods that don't require specialization to reduce
            // template bloat.
            // CallbackBase<MoveOnly> is a direct base class of MoveOnly callbacks, and
            // CallbackBase<Copyable> uses CallbackBase<MoveOnly> for its implementation.
            class PPXBASE_API CallbackBase {
              public:
                CallbackBase(CallbackBase &&c);
                CallbackBase &operator=(CallbackBase &&c);

                explicit CallbackBase(const CallbackBaseCopyable &c);
                CallbackBase &operator=(const CallbackBaseCopyable &c);

                explicit CallbackBase(CallbackBaseCopyable &&c);
                CallbackBase &operator=(CallbackBaseCopyable &&c);

                // Returns true if Callback is null (doesn't refer to anything).
                bool is_null() const {
                    return !bind_state_;
                }
                explicit operator bool() const {
                    return !is_null();
                }

                // Returns true if the callback invocation will be nop due to an cancellation.
                // It's invalid to call this on uninitialized callback.
                bool IsCancelled() const;

                // Returns the Callback into an uninitialized state.
                void Reset();

              protected:
                using InvokeFuncStorage = BindStateBase::InvokeFuncStorage;

                // Returns true if this callback equals |other|. |other| may be null.
                bool EqualsInternal(const CallbackBase &other) const;

                // Allow initializing of |bind_state_| via the constructor to avoid default
                // initialization of the scoped_refptr.
                explicit CallbackBase(BindStateBase *bind_state);

                InvokeFuncStorage polymorphic_invoke() const {
                    return bind_state_->polymorphic_invoke_;
                }

                // Force the destructor to be instantiated inside this translation unit so
                // that our subclasses will not get inlined versions.  Avoids more template
                // bloat.
                virtual ~CallbackBase();

                ScopedRefPtr<BindStateBase> bind_state_;
            };

            // CallbackBase<Copyable> is a direct base class of Copyable Callbacks.
            class PPXBASE_API CallbackBaseCopyable : public CallbackBase {
              public:
                CallbackBaseCopyable(const CallbackBaseCopyable &c);
                CallbackBaseCopyable(CallbackBaseCopyable &&c);
                CallbackBaseCopyable &operator=(const CallbackBaseCopyable &c);
                CallbackBaseCopyable &operator=(CallbackBaseCopyable &&c);

              protected:
                explicit CallbackBaseCopyable(BindStateBase *bind_state)
                    : CallbackBase(bind_state) {
                }
                virtual ~CallbackBaseCopyable() {
                }
            };

        }  // namespace internal
    }  // namespace base
}

#endif  // BASE_CALLBACK_INTERNAL_H_
