// https://github.com/ivankp/visit

// clang-format off
#pragma once

#include <Visit.hpp>

namespace visit {

template <
    typename... T,
    typename Container,
    typename Proj = detail::identity
>
auto* FindFirst(Container&& container, Proj proj = { }) {
    using std::begin;
    // Have to remove reference in order to make a pointer type
    using Element = std::remove_reference_t<
        decltype(proj(*begin(std::declval<Container>())))
    >;

    Element* ptr = nullptr;
    VisitEach(static_cast<Container&&>(container), proj,
        [&](Element& element, WrapsEither<T...>) {
            ptr = &element;
            return true; // break VisitEach loop
        }
    );
    return ptr;
}

template <
    typename... T,
    typename Container,
    typename Callback,
    typename Proj = detail::identity
>
void ForEach(Container&& container, Callback&& callback, Proj proj = {}) {
    using std::begin;
    using Element = decltype(proj(*begin(std::declval<Container>())));

    VisitEach(static_cast<Container&&>(container), proj,
        [&](Element&& element, WrapsEither<T...>) {
            return callback(static_cast<Element&&>(element));
        }
    );
}

template <
    typename... T,
    typename Container,
    typename Callback,
    typename Proj = detail::identity
>
void ForEachUnwrap(Container&& container, Callback&& callback, Proj proj = {}) {
    VisitEach(static_cast<Container&&>(container), proj,
        [&](T x) {
            if constexpr (std::is_rvalue_reference_v<T>) {
                return callback(static_cast<T&&>(x));
            } else {
                return callback(x);
            }
        }...
    );
}

}
