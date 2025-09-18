#pragma once

#include <callback.hpp>

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

template <typename Visited, typename F>
bool Visit(Visited&& x, F&& callback) {
    using Args = typename detail::CallbackTypesTrait<F>::Types;
    using decayedVisited = std::decay_t<Visited>;
    if constexpr (Args::size == 2) {
        using Arg = typename Args::Type<1>;
        if constexpr (std::is_same_v<decayedVisited, std::decay_t<Arg>>) {
            // Forward when there is only one argument.
            callback(static_cast<Visited&&>(x));
        } else {
            using ADL = VisitADL<decayedVisited, Arg>;
            if (!ADL::match(x))
                return false;
            // Forward when there is only one argument.
            callback(ADL::convert(static_cast<Visited&&>(x)));
        }
    } else if constexpr (Args::size == 3) {
        using Arg1 = typename Args::Type<1>;
        using Arg2 = typename Args::Type<2>;
        if constexpr (std::is_same_v<decayedVisited, std::decay_t<Arg2>>) {
            using ADL = VisitADL<decayedVisited, Arg1>;
            if (!ADL::match(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(ADL::convert(x), x);
        } else if constexpr (std::is_same_v<decayedVisited, std::decay_t<Arg1>>) {
            using ADL = VisitADL<decayedVisited, Arg2>;
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
        static_assert(detail::false_v<F>,
            "Unexpected number of callback arguments."
        );
    }
    return true;
}

template <typename Visited, typename... F>
bool Visit(Visited&& x, F&&... callback) {
    return (Visit(
        static_cast<Visited&&>(x),
        static_cast<F&&>(callback)
    ) || ...); // fold over the callback pack
}

/*
template <std::ranges::range VisitedRange, typename... F>
auto VisitEach(VisitedRange&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<VisitedRange> x) {
    Visit(x, static_cast<F&&>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(x, static_cast<F&&>(callback)...);
    }
}

template <std::ranges::range VisitedRange, typename... F>
auto VisitEachPtr(VisitedRange&& xs, F&&... callback)
requires requires(std::ranges::range_value_t<VisitedRange> x) {
    Visit(*x, static_cast<F&&>(callback)...);
}
{
    for (auto&& x : xs) {
        Visit(*x, static_cast<F&&>(callback)...);
    }
}
*/
