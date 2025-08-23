#pragma once

#include <callback.hpp>
#include <ranges>
#include <type_traits>

namespace detail {

template <typename...>
static constexpr bool false_v = false;

}

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

template <typename X, typename... F>
bool Visit(X&& x, F&&... callback) {
    using DecayedX = std::decay_t<X>;
    return ([&] { // fold over the callback pack
        static constexpr std::size_t numArgs = callback_num_args<F>;
        if constexpr (numArgs == 1) {
            using Arg = callback_type_t<F, 1>;
            if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg>>) {
                // Forward when there is only one argument.
                callback(std::forward<X>(x));
            } else {
                using ADL = VisitADL<DecayedX, Arg>;
                if (!ADL::match(x))
                    return false;
                // Forward when there is only one argument.
                callback(ADL::convert(std::forward<X>(x)));
            }
        } else if constexpr (numArgs == 2) {
            using Arg1 = callback_type_t<F, 1>;
            using Arg2 = callback_type_t<F, 2>;
            if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg2>>) {
                using ADL = VisitADL<DecayedX, Arg2>;
                if (!ADL::match(x))
                    return false;
                // The same value cannot be forwarded to multiple arguments.
                callback(ADL::convert(x), x);
            } else if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg1>>) {
                using ADL = VisitADL<DecayedX, Arg1>;
                if (!ADL::match(x))
                    return false;
                // The same value cannot be forwarded to multiple arguments.
                callback(x, ADL::convert(x));
            } else {
                static_assert(detail::false_v<X, Arg1, Arg2>,
                    "For a 2-argument callback, one of the arguments must match "
                    "the visited type."
                );
            }
        }
        return true;
    }() || ...);
}

template <std::ranges::range Xs, typename... F>
auto VisitEach(Xs&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<Xs> x) {
    Visit(x, std::forward<F>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(x, std::forward<F>(callback)...);
    }
}

template <std::ranges::range Xs, typename... F>
auto VisitEachPtr(Xs&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<Xs> x) {
    Visit(*x, std::forward<F>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(*x, std::forward<F>(callback)...);
    }
}
