#pragma once

#include <callback.hpp>
#include <type_traits>

namespace detail {

template <typename X>
struct TargetArg {
    template <typename T, typename = decltype(
        match<T>(std::declval<X>()),
        convert<T>(std::declval<X>()),
        void{}
    )>
    operator T();
};

}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(detail::TargetArg<X&&>{}), bool{})
{
    using Target = callback_type_t<F, 1>;
    if (!match<std::decay_t<Target>>(x))
        return false;
    callback(convert<Target>(std::forward<X>(x)));
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(detail::TargetArg<X&>{}, x), bool{})
{
    using Target = callback_type_t<F, 1>;
    if (!match<std::decay_t<Target>>(x))
        return false;
    callback(convert<Target>(x), x);
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(x, detail::TargetArg<X&>{}), bool{})
{
    using Target = callback_type_t<F, 2>;
    if (!match<std::decay_t<Target>>(x))
        return false;
    callback(x, convert<Target>(x));
    return true;
}

template <typename X, typename F>
auto Visit(X&& x, F&& callback)
-> decltype(callback(x), bool{})
{
    using Target = callback_type_t<F, 1>;
    if (!match<std::decay_t<Target>>(x))
        return false;
    callback(x);
    return true;
}

template <typename X, typename F, typename... Fs>
bool Visit(X&& x, F&& callback, Fs&&... callbacks) {
    cout << __PRETTY_FUNCTION__ << endl;
    return Visit(std::forward<X>(x), std::forward<F>(callback)) ||
           Visit(std::forward<X>(x), std::forward<Fs>(callbacks)...);
}
