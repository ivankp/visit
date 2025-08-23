#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace detail {

// These function declarations are SFINAE helpers for use in unevaluated contexts,
// e.g. inside decltype.
// They are used to determine the return and argument types of a callable.

template <typename R, typename... Args>
std::tuple<R, Args...> callback_types_helper(R (*)(Args...));

template <typename F, typename R, typename... Args>
std::tuple<R, Args...> callback_types_helper(R (F::*)(Args...));

template <typename F, typename R, typename... Args>
std::tuple<R, Args...> callback_types_helper(R (F::*)(Args...) const);

template <typename F>
decltype(callback_types_helper(&F::operator())) callback_types_helper(F);

} // end namespace detail

// tuple of return and argument types of a callable
template <typename F>
using callback_types_t = decltype(detail::callback_types_helper(std::declval<F>()));

// return nth return or argument type of a callable
template <typename F, std::size_t I>
using callback_type_t = typename std::tuple_element<I, callback_types_t<F>>::type;

// return nth return or argument type of a callable with std::decay applied
template <typename F, std::size_t I>
using decayed_callback_type_t = typename std::decay<callback_type_t<F, I>>::type;

// number of arguments of a callable
template <typename F>
constexpr bool callback_num_args = std::tuple_size<callback_types_t<F>>::value - 1;
