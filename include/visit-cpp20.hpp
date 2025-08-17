#pragma once

#include <callback.hpp>
#include <type_traits>

namespace detail {

template <typename T, typename X>
concept VisitTarget = requires (X x) {
    match<T>(x);
    convert<T>(x);
};

template <typename X>
struct TargetArg {
    template <VisitTarget<X> T>
    operator T();
};

}

template <typename X, typename... F>
void Visit(X&& x, F&&... callback) {
    ([&] { // fold over the callback pack
        if constexpr (requires { callback(detail::TargetArg<X&&>{}); }) {
            using Target = callback_type_t<F, 1>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            // Forward when there is only one argument.
            callback(convert<Target>(std::forward<X>(x)));
            return true;
        } else if constexpr (requires { callback(detail::TargetArg<X&>{}, x); }) {
            using Target = callback_type_t<F, 1>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(x, detail::TargetArg<X&>{}); }) {
            using Target = callback_type_t<F, 2>;
            if (!match<std::decay_t<Target>>(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(convert<Target>(x), x);
            return true;
        } else if constexpr (requires { callback(std::forward<X>(x)); }) {
            // Forward when there is only one argument.
            callback(std::forward<X>(x));
            return true;
        } else {
            static_assert(
                false,
                "A value of this type cannot be visited using the provided callbacks.\n"
                "Check the callbacks' argument types and that "
                "match<T>(x) and convert<T>(x) are implemented."
            );
        }
    }() || ...);
}
