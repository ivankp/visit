#pragma once

#include <callback.hpp>

namespace visit {

template <typename T, typename X>
concept Target = requires (X x) {
    ::visit::match<T>(x);
    ::visit::convert<T>(x);
};

namespace detail {

template <typename X>
struct TargetArg {
    template <Target<X> T>
    operator T();
};

}

template <typename T, typename... F>
auto Visit(X&& x, F&&... callback) {
    ([&] { // fold over the callback pack
        if constexpr (requires { callback(detail::TargetArg<X>{}); }) {
            using Target = decayed_callback_type_t<F, 1>;
            if (!::visit::match<Target>(x))
                return false;
            callback(::visit::convert<Target>(std::forward<X>(x)));
            return true;
        } else if constexpr (requires { callback(detail::TargetArg<X>{}, x); }) {
            using Target = decayed_callback_type_t<F, 1>;
            if (!::visit::match<Target>(x))
                return false;
            callback(::visit::convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(x, detail::TargetArg<X>{}); }) {
            using Target = decayed_callback_type_t<F, 2>;
            if (!::visit::match<Target>(x))
                return false;
            callback(::visit::convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(x); }) {
            callback(x);
            return true;
        } else {
            static_assert(
                false,
                "This value cannot be visited using the provided callbacks. "
                "Check the callbacks' argument types and that "
                "::visit::match<T>(x) and "
                "::visit::convert<T>(x) are implemented."
            );
        }
    }() || ...);
}

}
