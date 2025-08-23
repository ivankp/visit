#pragma once

#include <callback.hpp>
#include <ranges>
#include <type_traits>

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

template <typename Visited, typename... F>
bool Visit(Visited&& x, F&&... callback) {
    using DecayedX = std::decay_t<Visited>;
    return ([&] { // fold over the callback pack
        static constexpr unsigned numArgs = callbackNumArgs<F>;
        if constexpr (numArgs == 1) {
            using Arg = CallbackType_t<F, 1>;
            if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg>>) {
                // Forward when there is only one argument.
                callback(std::forward<Visited>(x));
            } else {
                using ADL = VisitADL<DecayedX, Arg>;
                if (!ADL::match(x))
                    return false;
                // Forward when there is only one argument.
                callback(ADL::convert(std::forward<Visited>(x)));
            }
        } else if constexpr (numArgs == 2) {
            using Arg1 = CallbackType_t<F, 1>;
            using Arg2 = CallbackType_t<F, 2>;
            if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg2>>) {
                using ADL = VisitADL<DecayedX, Arg1>;
                if (!ADL::match(x))
                    return false;
                // The same value cannot be forwarded to multiple arguments.
                callback(ADL::convert(x), x);
            } else if constexpr (std::is_same_v<DecayedX, std::decay_t<Arg1>>) {
                using ADL = VisitADL<DecayedX, Arg2>;
                if (!ADL::match(x))
                    return false;
                // The same value cannot be forwarded to multiple arguments.
                callback(x, ADL::convert(x));
            } else {
                static_assert(detail::false_v<Visited, Arg1, Arg2>,
                    "For a 2-argument callback, one of the arguments must match "
                    "the visited type."
                );
            }
        } else {
            static_assert(detail::false_v<Visited>,
                "Unexpected number of callback arguments."
            );
        }
        return true;
    }() || ...);
}

template <std::ranges::range VisitedRange, typename... F>
auto VisitEach(VisitedRange&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<VisitedRange> x) {
    Visit(x, std::forward<F>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(x, std::forward<F>(callback)...);
    }
}

template <std::ranges::range VisitedRange, typename... F>
auto VisitEachPtr(VisitedRange&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<VisitedRange> x) {
    Visit(*x, std::forward<F>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(*x, std::forward<F>(callback)...);
    }
}
