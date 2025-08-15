#pragma once

#include <callback.hpp>
#include <type_traits>

template <typename T, typename X>
concept Target = requires (X x) {
    match<T>(x);
    convert<T>(x);
};

namespace detail {

template <typename X>
struct TargetArg {
    template <Target<X> T>
    operator T();
};

}

template <typename X, typename... F>
void Visit(X&& x, F&&... callback) {
    cout << __PRETTY_FUNCTION__ << endl;
    ([&] { // fold over the callback pack
        if constexpr (requires { callback(detail::TargetArg<X&&>{}); }) {
            using Target = callback_type_t<F, 1>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            callback(convert<Target>(std::forward<X>(x)));
            return true;
        } else if constexpr (requires { callback(detail::TargetArg<X&>{}, x); }) {
            using Target = callback_type_t<F, 1>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            callback(convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(x, detail::TargetArg<X&>{}); }) {
            using Target = callback_type_t<F, 2>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            callback(convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(x); }) {
            callback(x);
            return true;
        } else {
            static_assert(
                false,
                "This value cannot be visited using the provided callbacks.\n"
                "Check the callbacks' argument types and that "
                "match<T>(x) and convert<T>(x) are implemented."
            );
        }
    }() || ...);
}
