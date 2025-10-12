// https://github.com/ivankp/visit

// clang-format off
#pragma once

#include <Callable.hpp>
#include <type_traits>

namespace visit {

/**
 * \brief Specialize this struct template to enable Visit() and VisitEach()
 * \tparam From The type of the visited wrapper
 * \tparam To   The type of the wrapped value
 */
template <typename From, typename To, typename = void>
struct VisitADL {
    static_assert(::detail::false_v<From, To>,
        "VisitADL is not specialized for these types."
    );
};

/**
 * \brief Wraps<T> can be used as a callback argument type to check whether the
 * type of the wrapped value is T without calling convert.
 * \tparam T Wrapped type
 */
template <typename T>
struct Wraps { };

/**
 * \brief WrapsEither<T> can be used as a callback argument type to check
 * whether the type of the wrapped value is either of T... without calling
 * convert.
 * \tparam T... Types to patch agains the wrapped type
 */
template <typename... T>
struct WrapsEither { };

namespace detail {

template <typename From, typename To>
struct VisitADL : ::visit::VisitADL<From, To> { };

/**
 * \brief VisitADL specialization for Wraps<T>
 */
template <typename From, typename To>
struct VisitADL<From, Wraps<To>> {
    // Call match from ADL for T
    template <typename Arg>
    static bool match(Arg&& arg) {
        return ::visit::VisitADL<From, To>::match(static_cast<Arg&&>(arg));
    }

    // Don't convert to delay conversion
    template <typename Arg>
    static Wraps<To> convert(Arg&&) noexcept {
        return { };
    }
};

template <typename From, typename... To>
struct VisitADL<From, WrapsEither<To...>> {
    // Call match from ADL for T
    template <typename Arg>
    static bool match(Arg&& arg) {
        return (
            ::visit::VisitADL<From, To>::match(static_cast<Arg&&>(arg)) || ...
        );
    }

    // Don't convert to delay conversion
    template <typename Arg>
    static WrapsEither<To...> convert(Arg&&) noexcept {
        return { };
    }
};

} // namespace detail

enum Control : unsigned char {
    LOOP_BIT = 1,
    CONTINUE_MATCH = 0,
    BREAK_MATCH = 2,
    CONTINUE_LOOP = 2,
    BREAK_LOOP = 3
};

#define VISIT_IMPL_FWD(x) static_cast<T##x&&>(x)

#define VISIT_IMPL_CONTROL(ARGS) \
    if constexpr (retControl) { \
        return callback ARGS ? BREAK_LOOP : CONTINUE_LOOP; \
    } else { \
        (void) callback ARGS; \
    }

#define VISIT_IMPL_MATCH(ARG, FROM, MATCH) \
    using ADL = ::visit::detail::VisitADL<DecayedFrom, ARG>; \
    decltype(auto) match = ADL::match(VISIT_IMPL_FWD(from)); \
    if (!match) \
        return CONTINUE_MATCH; \
    using Tmatch = decltype(match); \
    if constexpr (std::is_same_v<Tmatch, bool>) { \
        VISIT_IMPL_CONTROL(FROM) \
    } else { \
        VISIT_IMPL_CONTROL(MATCH) \
    }

/**
 * \brief Passes the value wrapped inside from to the callback, if the callback
 *     argument matches the type.
 * \param from     The visited wrapper
 * \param callback Visitor function
 */
template <typename Tfrom, typename Tcallback>
Control Visit(Tfrom&& from, Tcallback&& callback) {
    using CallbackTypes = CallableTypes_t<Tcallback>;
    using DecayedFrom = std::decay_t<Tfrom>;
    static constexpr bool retControl =
        std::is_same_v<typename CallbackTypes::template Type<0>, bool>;
    if constexpr (CallbackTypes::size == 2) {
        using Arg = typename CallbackTypes::template Type<1>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg>>) {
            VISIT_IMPL_CONTROL(( VISIT_IMPL_FWD(from) ))
        } else {
            VISIT_IMPL_MATCH( Arg,
                ( ADL::convert(VISIT_IMPL_FWD(from)) ),
                ( ADL::convert(VISIT_IMPL_FWD(match)) )
            );
        }
    } else if constexpr (CallbackTypes::size == 3) {
        using Arg1 = typename CallbackTypes::template Type<1>;
        using Arg2 = typename CallbackTypes::template Type<2>;
        if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg2>>) {
            VISIT_IMPL_MATCH( Arg1,
                ( ADL::convert(VISIT_IMPL_FWD(from)), VISIT_IMPL_FWD(from) ),
                ( ADL::convert(VISIT_IMPL_FWD(match)), VISIT_IMPL_FWD(from) )
            );
        } else if constexpr (std::is_same_v<DecayedFrom, std::decay_t<Arg1>>) {
            VISIT_IMPL_MATCH( Arg2,
                ( VISIT_IMPL_FWD(from), ADL::convert(VISIT_IMPL_FWD(from)) ),
                ( VISIT_IMPL_FWD(from), ADL::convert(VISIT_IMPL_FWD(match)) )
            );
        } else {
            static_assert(::detail::false_v<Tfrom, Arg1, Arg2>,
                "For a 2-argument callback, one of the arguments must match "
                "the visited type."
            );
        }
    } else {
        static_assert(::detail::false_v<Tcallback>,
            "Visit callbacks must have either 1 or 2 arguments."
        );
    }
    return BREAK_MATCH;
}

/**
 * \brief Passes the value wrapped inside `from` to the first matching callback
 * \param from     The visited wrapper
 * \param callback A pack of callback functions that will be tried in order
 */
template <typename Tfrom, typename... Tcallback>
Control Visit(Tfrom&& from, Tcallback&&... callback) {
    static_assert(sizeof...(callback) > 0,
        "Visit() must be called with at least 1 callback argument."
    );
    Control control;
    (void)((control = Visit(
        VISIT_IMPL_FWD(from),
        VISIT_IMPL_FWD(callback)
    )) || ...); // fold over the callback pack
    return control;
}

namespace detail {

/**
 * Callable used as an identity projection.
 * It simply forwards the argument.
 */
struct identity {
    template <typename T>
    [[nodiscard]] constexpr T&& operator()(T&& x) const noexcept {
        return static_cast<T&&>(x);
    }

    using is_transparent = void;
};

}

/**
 * \brief Visits each element of the container
 * \param container Container of elements
 * \param proj      Projection function for transforming elements into visitable types
 * \param callback  A pack of callback functions that will be tried in order
 *                  for each element
 */
template <typename Tcontainer, typename Tproj = detail::identity, typename... Tcallback>
bool VisitEach(Tcontainer&& container, Tproj proj, Tcallback&&... callback) {
    for (auto&& element : VISIT_IMPL_FWD(container)) {
        if (Visit(
            proj(static_cast<decltype(element)&&>(element)),
            VISIT_IMPL_FWD(callback)...
        ) & LOOP_BIT)
            return true;
    }
    return false;
    // This function could, in principle, be implemented using
    // std::invoke(proj, from).
    // But this would require including <functional>.
}

} // namespace visit

#undef VISIT_IMPL_FWD
#undef VISIT_IMPL_CONTROL
#undef VISIT_IMPL_MATCH
