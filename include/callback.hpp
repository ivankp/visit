#pragma once

#include <type_traits>
#include <utility>

namespace detail {

template <typename...>
static constexpr bool false_v = false;

template <std::size_t Index, typename T0, typename... T>
struct NthType {
    using type = typename NthType<Index - 1, T...>::type;
};

template <typename T0, typename... T>
struct NthType<0, T0, T...> {
    using type = T0;
};

template <typename... T>
struct TypeTuple {
    template <std::size_t Index>
    using type = typename NthType<Index, T...>::type;

    static constexpr std::size_t size = sizeof...(T);
};

// These function declarations are SFINAE helpers for use in unevaluated contexts,
// e.g. inside decltype.
// They are used to determine the return and argument types of a callable.

template <typename R, typename... Args>
TypeTuple<R, Args...> callback_types_helper(R (*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callback_types_helper(R (F::*)(Args...));

template <typename F, typename R, typename... Args>
TypeTuple<R, Args...> callback_types_helper(R (F::*)(Args...) const);

template <typename F>
decltype(callback_types_helper(&F::operator())) callback_types_helper(F);

template <typename F, typename = void>
struct callback_types_trait {
    static_assert(false_v<F>,
        "This callback must be neither a template nor an overloaded function."
    );
};

template <typename F>
struct callback_types_trait<F, decltype(callback_types_helper(std::declval<F>()), void())> {
    using types = decltype(callback_types_helper(std::declval<F>()));
};

} // end namespace detail

// return nth return or argument type of a callable
template <typename F, std::size_t I>
using callback_type_t = typename detail::callback_types_trait<F>::types::type<I>;

// return nth return or argument type of a callable with std::decay applied
template <typename F, std::size_t I>
using decayed_callback_type_t = typename std::decay<callback_type_t<F, I>>::type;

// number of arguments of a callable
template <typename F>
constexpr std::size_t callback_num_args = detail::callback_types_trait<F>::types::size - 1;
