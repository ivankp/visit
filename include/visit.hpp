#pragma once
// clang-format off

#include <callable.hpp>
#include <type_traits>

namespace visit {

template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

enum Control : unsigned char {
    BREAK_LOOP = 1,
    CONTINUE_LOOP = 0,
    BREAK_MATCH = 2,
    CONTINUE_MATCH = 0
};

#define VISIT_FWD(x) static_cast<T##x&&>(x)

#define VISIT_CONTROL(...) \
    if constexpr (retControl) { \
        return (Control)callback(__VA_ARGS__); \
    } else { \
        (void) callback(__VA_ARGS__); \
    }

#define VISIT_MATCH(Arg, FROM, MATCH) \
    using ADL = VisitADL<DecayedFrom, Arg>; \
    decltype(auto) match = ADL::match(from); \
    if (!match) \
        return CONTINUE_MATCH; \
    using Tmatch = decltype(match); \
    if constexpr (std::is_same_v<Tmatch, bool>) { \
        VISIT_CONTROL FROM \
    } else { \
        VISIT_CONTROL MATCH \
    }

template <typename Tfrom, typename Tcallback>
Control Visit(Tfrom&& from, Tcallback&& callback) {
    using CallbackTypes = CallableTypes_t<Tcallback>;
    using DecayedFrom = std::decay_t<Tfrom>;
    static constexpr bool retControl =
        std::is_same_v<typename CallbackTypes::template Type<0>, bool>;
    if constexpr (CallbackTypes::size == 2) {
        using Arg = typename CallbackTypes::template Type<1>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg>>) {
            VISIT_CONTROL(VISIT_FWD(from))
        } else {
            VISIT_MATCH( Arg,
                ( ADL::convert(VISIT_FWD(from)) ),
                ( ADL::convert(VISIT_FWD(match)) )
            );
        }
    } else if constexpr (CallbackTypes::size == 3) {
        using Arg1 = typename CallbackTypes::template Type<1>;
        using Arg2 = typename CallbackTypes::template Type<2>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg2>>) {
            VISIT_MATCH( Arg1,
                ( ADL::convert(VISIT_FWD(from)), VISIT_FWD(from) ),
                ( ADL::convert(VISIT_FWD(match)), VISIT_FWD(from) )
            );
        } else if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg1>>) {
            VISIT_MATCH( Arg2,
                ( VISIT_FWD(from), ADL::convert(VISIT_FWD(from)) ),
                ( VISIT_FWD(from), ADL::convert(VISIT_FWD(match)) )
            );
        } else {
            static_assert(detail::false_v<Tfrom, Arg1, Arg2>,
                "For a 2-argument callback, one of the arguments must match "
                "the visited type."
            );
        }
    } else {
        static_assert(detail::false_v<Tcallback>,
            "Visit callbacks must have either 1 or 2 arguments."
        );
    }
    return BREAK_MATCH;
}

template <typename Tfrom, typename... Tcallback>
Control Visit(Tfrom&& from, Tcallback&&... callback) {
    static_assert(sizeof...(callback) > 0,
        "Visit() must be called with at least 1 callback argument."
    );
    Control control;
    (void)((control = Visit(
        VISIT_FWD(from),
        VISIT_FWD(callback)
    )) || ...); // fold over the callback pack
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

template <typename Tcontainer, typename Tproj = detail::identity, typename... Tcallback>
bool VisitEach(Tcontainer&& container, Tproj proj, Tcallback&&... callback) {
    for (auto&& element : VISIT_FWD(container)) {
        if (Visit(
            proj(static_cast<decltype(element)&&>(element)),
            VISIT_FWD(callback)...
        ) & BREAK_LOOP)
            return true;
    }
    return false;
    // This function could, in principle, be implemented using
    // std::invoke(proj, from).
    // But this would require including <functional>.
}

} // namespace visit

#undef VISIT_FWD
#undef VISIT_CONTROL
#undef VISIT_MATCH
