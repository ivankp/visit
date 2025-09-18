#pragma once

#include <type_traits>
#include <utility>

namespace detail {

// Creates a dependent false value usable in static_assert
template <typename...>
static constexpr bool false_v = false;

// Get Nth type from a pack of types
template <unsigned Index, typename T0, typename... T>
struct NthType {
    using Type = typename NthType<Index - 1, T...>::Type;
};

template <typename T0, typename... T>
struct NthType<0, T0, T...> {
    using Type = T0;
};

// Wrapper for a pack of types
template <typename... T>
struct TypeTuple {
    template <unsigned Index>
    using Type = typename NthType<Index, T...>::Type;

    static constexpr unsigned size = sizeof...(T);
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

// Wrap callback types deduction into a class template
// to provide a descriptive static_assert.
template <typename F, typename = void>
struct CallbackTypesTrait {
    static_assert(false_v<F>,
        "This callback must be callable with a unique list of arguments. "
        "This may fail for templates and overloaded functions.
    );
};

template <typename F>
struct CallbackTypesTrait<F, decltype(callbackTypesHelper(std::declval<F>()), void())> {
    using Types = decltype(callbackTypesHelper(std::declval<F>()));
};

} // end namespace detail

// return nth return or argument type of a callable
template <typename F, unsigned I>
using CallbackType_t = typename detail::CallbackTypesTrait<F>::Types::Type<I>;

// return nth return or argument type of a callable with std::decay applied
template <typename F, unsigned I>
using DecayedCallbackType_t = typename std::decay<CallbackType_t<F, I>>::Type;

// number of arguments of a callable
template <typename F>
constexpr unsigned callbackNumArgs = detail::CallbackTypesTrait<F>::Types::size - 1;
