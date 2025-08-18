#pragma once

#include <callback.hpp>
#include <type_traits>

namespace detail {

struct AnyArg {
    template <typename T>
    operator T();
};

}

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(
        false,
        "VisitADL is not specialized for these types"
    );
};

template <typename X, typename... F>
void Visit(X&& x, F&&... callback) {
    ([&] { // fold over the callback pack
        if constexpr (requires { callback(std::forward<X>(x)); }) {
            // Forward when there is only one argument.
            callback(std::forward<X>(x));
        } else if constexpr (requires { callback(detail::AnyArg{}); }) {
            using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 1>>;
            if (!ADL::match(x))
                return false;
            // Forward when there is only one argument.
            callback(ADL::convert(std::forward<X>(x)));
        } else if constexpr (requires { callback(detail::AnyArg{}, x); }) {
            using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 1>>;
            if (!ADL::match(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(ADL::convert(x), x);
        } else if constexpr (requires { callback(x, detail::AnyArg{}); }) {
            using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 2>>;
            if (!ADL::match(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(x, ADL::convert(x));
        } else {
            static_assert(
                false,
                "A value of this type cannot be visited using the provided callbacks.\n"
                "Check the callbacks' argument types and that "
                "match<T>(x) and convert<T>(x) are implemented."
            );
        }
        return true;
    }() || ...);
}
