#pragma once

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
TypeTuple<R, Args...> callableTypesHelper(R (*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callableTypesHelper(R (F::*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callableTypesHelper(R (F::*)(Args...) const);

template <typename F>
decltype(callableTypesHelper(&F::operator())) callableTypesHelper(F);

// Wrap callable types deduction into a class template
// to provide a descriptive static_assert.
template <typename F, typename = void>
struct CallableTypes {
    static_assert(false_v<F>,
        "This callable must have a unique list of arguments. "
        "This may fail for templates and overloaded functions."
    );
};

// TODO: can this indirection be avoided?
// TODO: avoid dependence on std::declval -> wouldn't need <utility>

template <typename F>
struct CallableTypes<F, decltype(callableTypesHelper(std::declval<F>()), void())> {
    using Types = decltype(callableTypesHelper(std::declval<F>()));
};

} // end namespace detail

template <typename F>
using CallableTypes_t = typename detail::CallableTypes<F>::Types;
