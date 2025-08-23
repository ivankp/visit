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
        detail::false_v<From, To>,
        "VisitADL is not specialized for these types"
    );
};

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(std::forward<X>(x)), bool{})
{
    // Forward when there is only one argument.
    callback(std::forward<X>(x));
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(detail::AnyArg{}), bool{})
{
    using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 1>>;
    if (!ADL::match(x))
        return false;
    // Forward when there is only one argument.
    callback(ADL::convert(std::forward<X>(x)));
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(detail::AnyArg{}, x), bool{})
{
    using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 1>>;
    if (!ADL::match(x))
        return false;
    // The same value cannot be forwarded to multiple arguments.
    callback(ADL::convert(x), x);
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(x, detail::AnyArg{}), bool{})
{
    using ADL = VisitADL<std::decay_t<X>, callback_type_t<F, 2>>;
    if (!ADL::match(x))
        return false;
    // The same value cannot be forwarded to multiple arguments.
    callback(x, ADL::convert(x));
    return true;
}

template <typename X, typename F, typename... Fs>
bool Visit(X&& x, F&& callback, Fs&&... callbacks) {
    return Visit(std::forward<X>(x), std::forward<F>(callback)) ||
           Visit(std::forward<X>(x), std::forward<Fs>(callbacks)...);
}
