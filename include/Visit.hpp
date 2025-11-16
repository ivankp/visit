// https://github.com/ivankp/visit

// clang-format off
#pragma once

#include <Callable.hpp>
#include <type_traits>

namespace visit {

/**
 * \brief Specialize this struct template to enable Visit() and VisitEach()
 * \tparam Node   The type containing the value
 * \tparam Value  The type contained by the node
 */
template <typename Node, typename Value, typename = void>
struct VisitADL {
    static_assert(::detail::false_v<Node, Value>,
        "VisitADL is not specialized for these types."
    );
};

/**
 * \brief Wraps<T...> can be used as a callback argument type to check whether
 *   the contained value type is either of T... without calling convert.
 * \tparam ...  Possible value types
 */
template <typename...>
struct Wraps { };

namespace detail {

template <typename Node, typename Value>
struct VisitADL : ::visit::VisitADL<Node, Value> { };

/**
 * \brief VisitADL specialization for Wraps<...>
 */
template <typename Node, typename... Value>
struct VisitADL<Node, Wraps<Value...>> {
    // Call match() from Value ADL
    template <typename T>
    static bool match(T&& x) {
        return (
            ::visit::VisitADL<Node, Value>::match(static_cast<T&&>(x))
            || ...
        );
    }

    // Don't convert to delay conversion
    template <typename T>
    static Wraps<Value...> convert(T&&) noexcept {
        return { };
    }
};

} // namespace visit::detail

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
        return VISIT_IMPL_FWD(callback) ARGS ? BREAK_LOOP : CONTINUE_LOOP; \
    } else { \
        (void) VISIT_IMPL_FWD(callback) ARGS; \
    }

#define VISIT_IMPL_MATCH(VALUE, NODE, MATCH) \
    using ADL = ::visit::detail::VisitADL<DecayedNode, VALUE>; \
    decltype(auto) match = ADL::match(VISIT_IMPL_FWD(node)); \
    if (!match) \
        return CONTINUE_MATCH; \
    using Tmatch = decltype(match); \
    if constexpr (std::is_same_v<Tmatch, bool>) { \
        VISIT_IMPL_CONTROL(NODE) \
    } else { \
        VISIT_IMPL_CONTROL(MATCH) \
    }

/**
 * \brief Passes the value wrapped inside the node to the callback, if the
 *   callback argument matches the value type.
 * \param node      The node to be visited
 * \param callback  Visitor function
 */
template <typename Value = void, typename Tnode, typename Tcallback>
Control Visit(Tnode&& node, Tcallback&& callback) {
    using DecayedNode = std::decay_t<Tnode>;
    if constexpr (std::is_void_v<Value>) {
        // Deduce value type from callback arguments
        using CallbackTypes = CallableTypes_t<Tcallback>;
        static constexpr bool retControl =
            std::is_same_v<typename CallbackTypes::template Type<0>, bool>;
        if constexpr (CallbackTypes::size == 2) {
            using Arg = typename CallbackTypes::template Type<1>;
            if constexpr (std::is_same_v<DecayedNode, std::decay_t<Arg>>) {
                VISIT_IMPL_CONTROL(( VISIT_IMPL_FWD(node) ))
            } else {
                VISIT_IMPL_MATCH( Arg,
                    ( ADL::convert(VISIT_IMPL_FWD(node)) ),
                    ( ADL::convert(VISIT_IMPL_FWD(match)) )
                );
            }
        } else if constexpr (CallbackTypes::size == 3) {
            using Arg1 = typename CallbackTypes::template Type<1>;
            using Arg2 = typename CallbackTypes::template Type<2>;
            if constexpr (std::is_same_v<DecayedNode, std::decay_t<Arg2>>) {
                VISIT_IMPL_MATCH( Arg1,
                    ( ADL::convert(VISIT_IMPL_FWD(node)), VISIT_IMPL_FWD(node) ),
                    ( ADL::convert(VISIT_IMPL_FWD(match)), VISIT_IMPL_FWD(node) )
                );
            } else if constexpr (std::is_same_v<DecayedNode, std::decay_t<Arg1>>) {
                VISIT_IMPL_MATCH( Arg2,
                    ( VISIT_IMPL_FWD(node), ADL::convert(VISIT_IMPL_FWD(node)) ),
                    ( VISIT_IMPL_FWD(node), ADL::convert(VISIT_IMPL_FWD(match)) )
                );
            } else {
                static_assert(::detail::false_v<Tnode, Arg1, Arg2>,
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
    } else {
        // Value type provided
        using CallableValueNode = IsCallable<Tcallback, Value, Tnode>;
        if constexpr (CallableValueNode::value) {
            static constexpr bool retControl =
                std::is_same_v<typename CallableValueNode::type, bool>;
            VISIT_IMPL_MATCH( Value,
                ( ADL::convert(VISIT_IMPL_FWD(node)), VISIT_IMPL_FWD(node) ),
                ( ADL::convert(VISIT_IMPL_FWD(match)), VISIT_IMPL_FWD(node) )
            );
        } else {
            using CallableNodeValue = IsCallable<Tcallback, Tnode, Value>;
            if constexpr (CallableNodeValue::value) {
                static constexpr bool retControl =
                    std::is_same_v<typename CallableNodeValue::type, bool>;
                VISIT_IMPL_MATCH( Value,
                    ( VISIT_IMPL_FWD(node), ADL::convert(VISIT_IMPL_FWD(node)) ),
                    ( VISIT_IMPL_FWD(node), ADL::convert(VISIT_IMPL_FWD(match)) )
                );
            } else {
                using CallableValue = IsCallable<Tcallback, Value>;
                if constexpr (CallableValue::value) {
                    static constexpr bool retControl =
                        std::is_same_v<typename CallableValue::type, bool>;
                    VISIT_IMPL_MATCH( Value,
                        ( ADL::convert(VISIT_IMPL_FWD(node)) ),
                        ( ADL::convert(VISIT_IMPL_FWD(match)) )
                    );
                } else {
                    using CallableNode = IsCallable<Tcallback, Tnode>;
                    if constexpr (CallableNode::value) {
                        static constexpr bool retControl =
                            std::is_same_v<typename CallableNode::type, bool>;
                        VISIT_IMPL_CONTROL(( VISIT_IMPL_FWD(node) ))
                    }
                }
            }
        }
        return CONTINUE_MATCH;
    }
}

/**
 * \brief Passes the value wrapped inside the node to the first matching callback
 * \param node      The node to be visited
 * \param callback  A pack of callback functions that will be tried in order
 */
template <typename Value = void, typename Tnode, typename... Tcallback>
Control Visit(Tnode&& node, Tcallback&&... callback) {
    static_assert(sizeof...(callback) > 0,
        "Visit() must be called with at least 1 callback argument."
    );
    Control control;
    (void)((control = Visit<Value>(
        VISIT_IMPL_FWD(node),
        VISIT_IMPL_FWD(callback)
    )) || ...); // fold over the callback pack
    return control;
}

template <typename... Value, typename Tnode, typename... Tcallback>
auto Visit(Tnode&& node, Tcallback&&... callback)
-> std::enable_if_t<(sizeof...(Value) > 1), Control>
{
    Control control;
    (void)((control = Visit<Value>(
        VISIT_IMPL_FWD(node),
        VISIT_IMPL_FWD(callback)...
    )) || ...); // fold over the Value pack
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
 * \param container  Container of elements
 * \param proj       Projection function for transforming container elements
 *   into visitable node types
 * \param callback   A pack of callback functions that will be tried in order
 *   for each element of the container
 */
template <
  typename Tcontainer,
  typename Tproj = detail::identity,
  typename... Tcallback
>
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
    // std::invoke(proj, node).
    // But this would require including <functional>.
}

namespace detail {

template <typename Tvalue, typename Tcallback>
auto WrapCallback(Tcallback&& callback) noexcept {
    return [&](Tvalue&& value) mutable -> decltype(auto) {
        return VISIT_IMPL_FWD(callback)(VISIT_IMPL_FWD(value));
        // TODO: allow multiple callbacks
    };
}

} // namespace visit::detail

/**
 * \brief Visits only elements of the container with value type in the
 *   user-provided list
 * \tparam Tvalue    List of value types to visit
 * \param container  Container of elements
 * \param proj       Projection function for transforming container elements
 *   into visitable node types
 * \param callback   A pack of callback functions that will be tried in order
 *   for each element of the container
 */
/*
template <
    typename... Tvalue,
    typename Tcontainer,
    typename Tproj = detail::identity,
    typename Tcallback
>
void VisitEachOf(Tcontainer&& container, Proj proj = {}, Tcallback&& callback) {
    return VisitEach(
        VISIT_IMPL_FWD(container),
        proj,
        detail::WrapCallback<Tvalue>(VISIT_IMPL_FWD(callback))...
        // TODO: allow multiple callbacks
    );
}
*/
// TODO: Can this function handle passing both/either node and value to the callback?

} // namespace visit

#undef VISIT_IMPL_FWD
#undef VISIT_IMPL_CONTROL
#undef VISIT_IMPL_MATCH
