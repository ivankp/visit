#pragma once

#include <type_traits>
#include <utility>

namespace detail {

template <typename...>
static constexpr bool false_v = false;

template <std::size_t Index, typename T0, typename... T>
struct NthType {
    using Type = typename NthType<Index - 1, T...>::Type;
};

template <typename T0, typename... T>
struct NthType<0, T0, T...> {
    using Type = T0;
};

template <typename... T>
struct TypeTuple {
    template <std::size_t Index>
    using Type = typename NthType<Index, T...>::Type;

    static constexpr std::size_t size = sizeof...(T);
};

// These function declarations are SFINAE helpers for use in unevaluated contexts,
// e.g. inside decltype.
// They are used to determine the return and argument types of a callable.

template <typename R, typename... Args>
TypeTuple<R, Args...> callbackTypesHelper(R (*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callbackTypesHelper(R (F::*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callbackTypesHelper(R (F::*)(Args...) const);

template <typename F>
decltype(callbackTypesHelper(&F::operator())) callbackTypesHelper(F);

template <typename F, typename = void>
struct CallbackTypesTrait {
    static_assert(false_v<F>,
        "This callback must be neither a template nor an overloaded function."
    );
};

template <typename F>
struct CallbackTypesTrait<F, decltype(callbackTypesHelper(std::declval<F>()), void())> {
    using Types = decltype(callbackTypesHelper(std::declval<F>()));
};

} // end namespace detail

// return nth return or argument type of a callable
template <typename F, std::size_t I>
using CallbackType_t = typename detail::CallbackTypesTrait<F>::Types::Type<I>;

// return nth return or argument type of a callable with std::decay applied
template <typename F, std::size_t I>
using DecayedCallbackType_t = typename std::decay<CallbackType_t<F, I>>::Type;

// number of arguments of a callable
template <typename F>
constexpr std::size_t callbackNumArgs = detail::CallbackTypesTrait<F>::Types::size - 1;
