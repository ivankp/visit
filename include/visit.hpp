#pragma once

#include <callable.hpp>
#include <type_traits>

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

enum Control : unsigned char {
    BREAK_MATCH = 0,
    CONTINUE_LOOP = 0,
    BREAK_LOOP = 1,
    CONTINUE_MATCH = 2,
    FAILED_MATCH = 4
};

template <typename>
constexpr bool isControl = false;
template <>
constexpr bool isControl<Control> = true;
template <>
constexpr bool isControl<bool> = true;

#define VISIT_CONTROL(...) \
    if constexpr (retControl) { \
        return __VA_ARGS__; \
    } else { \
        (void) __VA_ARGS__; \
    }

template <typename From, typename F>
Control Visit(From&& from, F&& callback) {
    using CallbackTypes = CallableTypes_t<F>;
    using DecayedFrom = std::decay_t<From>;
    static constexpr bool retControl = isControl<typename CallbackTypes::template Type<0>>;
    if constexpr (CallbackTypes::size == 2) {
        using Arg = typename CallbackTypes::template Type<1>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg>>) {
            VISIT_CONTROL(callback(static_cast<From&&>(from)))
        } else {
            using ADL = VisitADL<DecayedFrom, Arg>;
            decltype(auto) match = ADL::match(from);
            if (!match)
                return FAILED_MATCH;
            using Match = decltype(match);
            if constexpr (std::is_same_v<Match, bool>) {
                VISIT_CONTROL(callback(ADL::convert(static_cast<From&&>(from))))
            } else {
                VISIT_CONTROL(callback(ADL::convert(static_cast<Match&&>(match))))
            }
        }
    } else if constexpr (CallbackTypes::size == 3) {
        using Arg1 = typename CallbackTypes::template Type<1>;
        using Arg2 = typename CallbackTypes::template Type<2>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg2>>) {
            using ADL = VisitADL<DecayedFrom, Arg1>;
            decltype(auto) match = ADL::match(from);
            if (!match)
                return FAILED_MATCH;
            using Match = decltype(match);
            if constexpr (std::is_same_v<Match, bool>) {
                VISIT_CONTROL(callback(ADL::convert(from), static_cast<From&&>(from)))
            } else {
                VISIT_CONTROL(callback(ADL::convert(static_cast<Match&&>(match)), static_cast<From&&>(from)))
            }
        } else if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg1>>) {
            using ADL = VisitADL<DecayedFrom, Arg2>;
            decltype(auto) match = ADL::match(from);
            if (!match)
                return FAILED_MATCH;
            using Match = decltype(match);
            if constexpr (std::is_same_v<Match, bool>) {
                VISIT_CONTROL(callback(static_cast<From&&>(from), ADL::convert(from)))
            } else {
                VISIT_CONTROL(callback(static_cast<From&&>(from), ADL::convert(static_cast<Match&&>(match))))
            }
        } else {
            static_assert(detail::false_v<From, Arg1, Arg2>,
                "For a 2-argument callback, one of the arguments must match "
                "the visited type."
            );
        }
    } else {
        static_assert(detail::false_v<F>,
            "Visit callbacks must have either 1 or 2 arguments."
        );
    }
    return BREAK_MATCH;
}

template <typename From, typename... F>
Control Visit(From&& from, F&&... callback) {
    static_assert(sizeof...(callback) > 0,
        "Visit() must be called with at least 1 callback argument."
    );
    Control control;
    (void)(((control = Visit(
        static_cast<From&&>(from),
        static_cast<F&&>(callback)
    )) & 6) && ...); // fold over the callback pack
    return control;
}

namespace detail {

struct identity {
    template <typename T>
    [[nodiscard]] constexpr T&& operator()(T&& x) const noexcept {
        return static_cast<T&&>(x);
    }

    using is_transparent = void;
};

}

template <typename Container, typename Proj = detail::identity, typename... F>
bool VisitEach(Container&& container, Proj proj, F&&... callback) {
    for (auto&& element : static_cast<Container&&>(container)) {
        if (Visit(
            proj(static_cast<decltype(element)&&>(element)),
            static_cast<F&&>(callback)...
        ) & 1)
            return true;
    }
    return false;
    // This function could, in principle, be implemented using
    // std::invoke(proj, from).
    // But this would require including <functional>.
}
