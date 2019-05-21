#ifndef BASE_CALLBACK_FORWARD_H_
#define BASE_CALLBACK_FORWARD_H_
#pragma once

namespace ppx {
    namespace base {

        template <typename Signature>
        class OnceCallback;

        template <typename Signature>
        class RepeatingCallback;

        template <typename Signature>
        using Callback = RepeatingCallback<Signature>;

        // Syntactic sugar to make Callback<void()> easier to declare since it
        // will be used in a lot of APIs with delayed execution.
        using OnceClosure = OnceCallback<void()>;
        using RepeatingClosure = RepeatingCallback<void()>;
        using Closure = Callback<void()>;

    }  // namespace base
}
#endif  // BASE_CALLBACK_FORWARD_H_
