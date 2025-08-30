#pragma once

#include <callback.hpp>
// #include <ranges>
#include <type_traits>

namespace detail {

template <typename A, typename B>
constexpr bool decayToSame = std::is_same_v<std::decay_t<A>, std::decay_t<B>>;

}

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

template <typename Visited, typename F>
bool VisitImpl(Visited&& x, F&& callback) {
    using Args = typename detail::CallbackTypesTrait<F>::Types;
    if constexpr (Args::size == 2) {
        using Arg = typename Args::Type<1>;
        if constexpr (detail::decayToSame<Visited, Arg>) {
            // Forward when there is only one argument.
            callback(std::forward<Visited>(x));
        } else {
            using ADL = VisitADL<std::decay_t<Visited>, Arg>;
            if (!ADL::match(x))
                return false;
            // Forward when there is only one argument.
            callback(ADL::convert(std::forward<Visited>(x)));
        }
    } else if constexpr (Args::size == 3) {
        using Arg1 = typename Args::Type<1>;
        using Arg2 = typename Args::Type<2>;
        if constexpr (detail::decayToSame<Visited, Arg2>) {
            using ADL = VisitADL<std::decay_t<Visited>, Arg1>;
            if (!ADL::match(x))
                return false;
            // The same value cannot be forwarded to multiple arguments.
            callback(ADL::convert(x), x);
        } else if constexpr (detail::decayToSame<Visited, Arg1>) {
            using ADL = VisitADL<std::decay_t<Visited>, Arg2>;
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
}

template <typename Visited, typename... F>
bool Visit(Visited&& x, F&&... callback) {
    return (VisitImpl(
        std::forward<Visited>(x),
        std::forward<F>(callback)
    ) || ...); // fold over the callback pack
}

/*
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
*/
